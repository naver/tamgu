/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : conversion.cxx
 Date       : 2017/09/01
 Purpose    : Conversion methods to handle UTF8 and UNICODE strings
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#include "conversion.h"
#include "x_tokenize.h"

#ifdef INTELINTRINSICS
#ifdef WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif
#endif

#ifdef WSTRING_IS_UTF16
void concat_char_convert_utf16(wstring& res, TAMGUCHAR code);
#else
#define concat_char_convert_utf16(res,code) res += code;
#endif

static char digitaction[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    '+',0,'-','.',0,
    '0','0','0','0','0','0','0','0','0','0',0,0,0,0,0,0,0,
    'X','X','X','X','X','X',
    0,0,0,0,0,0,0,0,0,'p',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    'x','x','x','x','x','x',0,0,0,0,0,0,0,0,0,'p',
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

//------------------------------------------------------------------------
#define isadigit(c) c >= '0' && c <= '9'
//------------------------------------------------------------------------
//There is a wnscmp method, however there seems to be a problem when used in conjunction with intrinsics instructions...
//Some explanations: we suppose that the first character of both strings has already been identified as being the same
static inline bool wcharcomp(wchar_t* src, wchar_t* search, long lensearch) {
    while (--lensearch) {
        if (src[lensearch] != search[lensearch])
            return false;
    }
    return true;
}

//This method has been implemented to compare two strings, knowing that the first characters
//of each string are the same already... We check from the end of the strings
static inline bool charcomp(unsigned char* src, unsigned char* search, long lensearch) {
    while (--lensearch) {
        if (src[lensearch] != search[lensearch])
            return false;
    }
    return true;
}

static inline char* concatstrings(char* str, char* ctn, long& i, long& size_str, long size_ctn) {
    if ((i + size_ctn) >= size_str) {
        size_str += size_ctn;
        size_str <<= 1;
        char* s = new char[size_str];
        memcpy(s, str, i);
        delete[] str;
        str = s;
    }
    memcpy(str+i,ctn, size_ctn);
    i += size_ctn;
    str[i] = 0;
    return str;
}

static inline wchar_t* concatstrings(wchar_t* str, wchar_t* ctn, long& i, long& size_str, long size_ctn) {
    long j;
    if ((i + size_ctn) >= size_str) {
        size_str += size_ctn;
        size_str <<= 1;
        wchar_t* s = new wchar_t[size_str];
        for (j = 0; j < i; j++)
            s[j] = str[j];
        delete[] str;
        str = s;
    }

    for (j = 0; j < size_ctn; j++)
        str[i++] = ctn[j];
    
    str[i] = 0;
    return str;
}
//------------------------------------------------------------------------
#ifdef INTELINTRINSICS
static const __m128i checkifzero = _mm_set1_epi8(0xFF);

long _bitcounter(binuint64 x) {
    long nb = 0;
    while (x) {
        if (x & 1) nb++;
        x >>= 1;
    }
    return nb;
}

#ifdef AVXSUPPORT

//-------------------------------------------------------------------------------------!
//-------------------------------------------------------------------------------------!
//-------------------------------------------------------------------------------------!
// AVX SUPPORT, we can use __256i numbers, which means that for wstring objects,       !
// the increment will be 8 or 16 depending on whether it is on Windows (UTF16)         !
// or on Unix machines (UTF32).                                                        !
// The same applies to regular strings, where the increment will be 32 (256/8)         !
//-------------------------------------------------------------------------------------!
//-------------------------------------------------------------------------------------!
//-------------------------------------------------------------------------------------!

static const __m256i checkutf8byte = _mm256_set1_epi8(0x80);
static const __m256i checkifbigzero = _mm256_set1_epi8(0xFF);

#define thestringincrement 32

#ifdef WIN32
#define large_char 0x8000
#define szwchar 1
#define doublestringincrement 32
#define thewstringincrement 16
#define stringincrement 16
#define mset_256 _mm256_set1_epi16
#define mcomp_256 _mm256_cmpeq_epi16
#else
#define szwchar 2
#define large_char 0x10000
#define doublestringincrement 16
#define stringincrement 8
#define thewstringincrement 8
#define mset_256 _mm256_set1_epi32
#define mcomp_256 _mm256_cmpeq_epi32
#endif


bool check_ascii(unsigned char* src, long lensrc, long& i) {
    __m256i current_bytes = _mm256_setzero_si256();
    
    i = 0;
    for (; (i + 31) < lensrc; i += 32) {
            //we load our section, the length should be larger than 16
        current_bytes = _mm256_loadu_si256((const __m256i *)(src + i));
        if (!_mm256_testz_si256(checkutf8byte, current_bytes)) {
            while (!(src[i] & 128))
                i++;
            return false;
        }
    }
    
    // last part
    while (i < lensrc) {
        if ((src[i] & 128))
            return false;
        i++;
    }
    
    return true;
}

static const __m256i _thecr = _mm256_set1_epi8('\n');
static const __m256i _thedoublequotes = _mm256_set1_epi8('"');
static const __m256i _thequotes = _mm256_set1_epi8('\'');
static const __m256i _theslash = _mm256_set1_epi8('/');
static const __m256i _theopen = _mm256_set1_epi8('(');
static const __m256i _theclose = _mm256_set1_epi8(')');


static const __m256i _checkingmore[] = {_thecr, _theslash, _theopen, _theclose, _mm256_set1_epi8('<'), _mm256_set1_epi8('>'), _mm256_set1_epi8('='), _mm256_set1_epi8(';')};

static const __m256i _tocheck[] = {_mm256_set1_epi8('"'), _mm256_set1_epi8('\''), _mm256_set1_epi8('@'), _mm256_set1_epi8(':'), _mm256_set1_epi8(','),
    _mm256_set1_epi8('-'), _mm256_set1_epi8('+'),
    _mm256_set1_epi8('0'),_mm256_set1_epi8('1'),_mm256_set1_epi8('2'),_mm256_set1_epi8('3'),_mm256_set1_epi8('4'),_mm256_set1_epi8('5'), _mm256_set1_epi8('6'),_mm256_set1_epi8('7'),
    _mm256_set1_epi8('8'), _mm256_set1_epi8('9'),
    _mm256_set1_epi8('['),_mm256_set1_epi8(']'),_mm256_set1_epi8('{'), _mm256_set1_epi8('}')};


void split_container(unsigned char* src, long lensrc, vector<long>& pos, bool forindent) {
    __m256i current_bytes = _mm256_setzero_si256();
    __m256i val;
    long b;
#ifdef WIN32
    unsigned long q, r;
#else
    uint32_t q, r;
#endif
    long i = 0;
    long sz = sizeof(_tocheck)/sizeof(__m256);
    long szcheck = sizeof(_checkingmore)/sizeof(__m256);

    for (; (i + 31) < lensrc; i += 32) {
        //we load our section, the length should be larger than 32
        current_bytes = _mm256_loadu_si256((const __m256i *)(src + i));
        q = 0;
        for (r = 0; r < sz; r++) {
            val = _mm256_cmpeq_epi8(current_bytes, _tocheck[r]);
            q |= _mm256_movemask_epi8(val);
        }
             
        if (forindent) {
            for (r = 0; r < szcheck; r++) {
                val = _mm256_cmpeq_epi8(current_bytes, _checkingmore[r]);
                q |= _mm256_movemask_epi8(val);
            }
        }
        
        if (q) {
            b = i;
            while (q) {
                bitscanforward(r, q);
                if (r) {
                    b += r;
                    q >>= r;
                }
                pos.push_back(b);
                q >>= 1;
                b++;
            }
        }
    }
    
    if (i < lensrc) {
        uchar str[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        for (b = i; b < lensrc; b++)
            str[b-i] = src[b];
        current_bytes = _mm256_loadu_si256((const __m256i *)str);
        
        q = 0;
        for (r = 0; r < sz; r++) {
            val = _mm256_cmpeq_epi8(current_bytes, _tocheck[r]);
            q |= _mm256_movemask_epi8(val);
        }
        
        if (forindent) {
            for (r = 0; r < szcheck; r++) {
                val = _mm256_cmpeq_epi8(current_bytes, _checkingmore[r]);
                q |= _mm256_movemask_epi8(val);
            }
        }

        if (q) {
            b = i;
            while (q) {
                bitscanforward(r, q);
                if (r) {
                    b += r;
                    q >>= r;
                }
                pos.push_back(b);
                q >>= 1;
                b++;
            }
        }
    }
}


void find_quotes(unsigned char* src, long lensrc, vector<long>& pos, bool lispmode) {
    __m256i current_bytes = _mm256_setzero_si256();
    __m256i val;
    long b;
#ifdef WIN32
    unsigned long q, r;
#else
    uint32_t q, r;
#endif
    long i = 0;
    
    for (; (i + 31) < lensrc; i += 32) {
        //we load our section, the length should be larger than 32
        current_bytes = _mm256_loadu_si256((const __m256i *)(src + i));

        val = _mm256_cmpeq_epi8(current_bytes, _theslash);
        q = _mm256_movemask_epi8(val);

        if (!lispmode) {
            val = _mm256_cmpeq_epi8(current_bytes, _thequotes);
            q |= _mm256_movemask_epi8(val);
        }
        
        val = _mm256_cmpeq_epi8(current_bytes, _thecr);
        q |= _mm256_movemask_epi8(val);

        val = _mm256_cmpeq_epi8(current_bytes, _thedoublequotes);
        q |= _mm256_movemask_epi8(val);

        if (q) {
            b = i;
            while (q) {
                bitscanforward(r, q);
                if (r) {
                    b += r;
                    q >>= r;
                }
                pos.push_back(b);
                q >>= 1;
                b++;
            }
        }
    }
    
    if (i < lensrc) {
        uchar str[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        for (b = i; b < lensrc; b++)
            str[b-i] = src[b];
        current_bytes = _mm256_loadu_si256((const __m256i *)str);
        
        val = _mm256_cmpeq_epi8(current_bytes, _theslash);
        q = _mm256_movemask_epi8(val);

        if (!lispmode) {
            val = _mm256_cmpeq_epi8(current_bytes, _thequotes);
            q |= _mm256_movemask_epi8(val);
        }

        val = _mm256_cmpeq_epi8(current_bytes, _thecr);
        q |= _mm256_movemask_epi8(val);

        val = _mm256_cmpeq_epi8(current_bytes, _thedoublequotes);
        q |= _mm256_movemask_epi8(val);

        if (q) {
            b = i;
            while (q) {
                bitscanforward(r, q);
                if (r) {
                    b += r;
                    q >>= r;
                }
                pos.push_back(b);
                q >>= 1;
                b++;
            }
        }
    }
}

//This version of find_intel also detects the section in which potential UTF8 characters may occur...
//The first section is then recorded into ps, ascii is then set to false to indicate the potential presence
//of a utf8 character in the string...
long find_intel_byte(unsigned char* src, unsigned char* search, long lensrc, long lensearch, long i) {
    if (lensearch > lensrc)
        return -1;

        //First we try to find the section in which the first character might occur
    __m256i current_bytes = _mm256_setzero_si256();

    uchar* s=src;
    unsigned long j;
#ifdef WIN32
	unsigned long q = 0;
#else
    uint32_t q = 0;
#endif
    uchar c = search[0];
    char shift;
    long pos;
    long lenmx = lensrc - 31;

    switch (lensearch) {
        case 1:
        {
            __m256i firstchar = _mm256_set1_epi8(c);
            //We then scan our string for this first character...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 32) {
                    //we load our section, the length should be larger than 16
                    s=src+i;
                    current_bytes = _mm256_loadu_si256((const __m256i *)s);
                    //we check if we have a the first character of our string somewhere
                    //in this 16 character window...
                    current_bytes = _mm256_cmpeq_epi8(firstchar, current_bytes);
                    q = _mm256_movemask_epi8(current_bytes);
                }
                if (q) {
                    bitscanforward(q, q);
                    return (i+q-32);
                }
            }
            break;
        }
        case 2:
        case 3:
        {
            int16_t cc = search[1];
            cc <<= 8;
            cc |= c;
            __m256i firstchar = _mm256_set1_epi16(cc);
        
                //We then scan our string for the first two characters...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 32) {
                    for (shift = 1; shift >= 0; shift--) {
                        //we load our section, the length should be larger than 16
                        s=src + i + shift;
                        //we read 16 characters at a time, but with a sliding widow
                        //of 15 characters (since we are looking for a double character)
                        current_bytes = _mm256_loadu_si256((const __m256i *)s);
                        //we check if we have a the first character of our string somewhere
                        //in this 16 character window...
                        current_bytes = _mm256_cmpeq_epi16(firstchar, current_bytes);
                        q <<= 1;
                        q |= _mm256_movemask_epi8(current_bytes);
                    }
                }
                if (!q)
                    break;
                
                if (lensearch == 2) {
                    bitscanforward(j,q);
                    return (i+j-32);
                }
                pos = i - 32;
                while (q) {
                    bitscanforward(j,q);
                    if (j) {
                        pos+=j;
                        s+=j;
                        q >>= j;
                    }
                    if (s[2] == search[2])
                        return (pos);
                    q >>= 2;
                    s+=2;
                    pos+=2;
                }
            }
            break;
        }
        case 4:
        case 5:
        case 6:
        case 7:
        {
                //We look for the presence of four characters in a row
            int32_t cc = search[3];
            for (shift = 2; shift >= 0; shift--) {
                cc <<= 8;
                cc |= search[shift];
            }
            
            __m256i firstchar = _mm256_set1_epi32(cc);
            
                //We then scan our string for the first two characters...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 32) {
                    //we load our section, the length should be larger than 16
                    for (shift = 3; shift >= 0; shift--) {
                        s=src + i + shift;
                        //we read 16 characters at a time, but with a sliding widow
                        //of 15 characters (since we are looking for a double character)
                        current_bytes = _mm256_loadu_si256((const __m256i *)s);
                        //we check if we have a the first character of our string somewhere
                        //in this 16 character window...
                        current_bytes = _mm256_cmpeq_epi32(firstchar, current_bytes);
                        q <<= 1;
                        q |= _mm256_movemask_epi8(current_bytes);
                    }
                }
                if (!q)
                    break;
                
                if (lensearch == 4) {
                    bitscanforward(j,q);
                    return (i+j-32);
                }
                pos = i - 32;
                while (q) {
                    bitscanforward(j,q);
                    if (j) {
                        pos+=j;
                        s+=j;
                        q >>= j;
                    }
                    if (charcomp(s+3,search+3,lensearch-3))
                        return (pos);
                    q >>= 4;
                    s+=4;
                    pos+=4;
                }
            }
            break;
        }
        default:
        {
                //We look for the presence of eight characters in a row
            int64_t cc = search[7];
            for (shift = 6; shift >= 0; shift--) {
                cc <<= 8;
                cc |= search[shift];
            }
            
            __m256i firstchar = _mm256_set1_epi64x(cc);
            
                //We then scan our string for the first two characters...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 32) {
                    //we load our section, the length should be larger than 16
                    for (shift = 7; shift >= 0; shift--) {
                        s=src + i + shift;
                        //we read 16 characters at a time, but with a sliding widow
                        //of 15 characters (since we are looking for a double character)
                        current_bytes = _mm256_loadu_si256((const __m256i *)s);
                        //we check if we have a the first character of our string somewhere
                        //in this 16 character window...
                        current_bytes = _mm256_cmpeq_epi64(firstchar, current_bytes);
                        q <<= 1;
                        q |= _mm256_movemask_epi8(current_bytes);
                    }
                }
                
                if (!q)
                    break;
                
                if (lensearch == 8) {
                    bitscanforward(j,q);
                    return (i+j-32);
                }
                
                pos = i - 32;
                while (q) {
                    bitscanforward(j,q);
                    if (j) {
                        pos+=j;
                        s+=j;
                        q >>= j;
                    }
                    if (charcomp(s+7,search+7,lensearch-7))
                        return (pos);
                    q >>= 8;
                    s+=8;
                    pos+=8;
                }
            }
        }
    }
    
    lensrc-=lensearch;
    for (;i<=lensrc;i++) {
        if (src[i] == c) {
            if (lensearch == 1 || charcomp(src+i,search,lensearch))
                return i;
        }
    }
    
    return -1;
}

long rfind_intel(unsigned char* src, unsigned char* search, long lensrc, long lensearch, long from) {
    if (lensearch > lensrc)
        return -1;

        //First we try to find the section in which the first character might occur
    __m256i current_bytes = _mm256_setzero_si256();
    
    uchar* s = src;
    unsigned long j;
#ifdef WIN32
	unsigned long q = 0;
#else
    uint32_t q = 0;
#endif

    long i = from;
    uchar c = search[0];
    char shift;

    long pos;
    switch (lensearch) {
        case 1:
        {
            __m256i firstchar = _mm256_set1_epi8(c);
                //We then scan our string for this first character...
            i -= 32;
            while (i >= 0) {
                for (; i >= 0 && !q; i -= 32) {
                    //we load our section, the length should be larger than 16
                    s=src+i;
                    current_bytes = _mm256_loadu_si256((const __m256i *)s);
                    //we check if we have a the first character of our string somewhere
                    //in this 16 character window...
                    current_bytes = _mm256_cmpeq_epi8(firstchar, current_bytes);
                    q = _mm256_movemask_epi8(current_bytes);
                }
                if (!q)
                    break;

                pos = 31; //32-1
                while (q) {
                    bitscanreverse(j,q);
                    if (j) {
                        pos -= 31-j;
                        q <<= 31-j;
                    }
                    if ((i+32+pos) <= from)
                        return (i+32+pos);
                    q <<= 1;
                    pos--;
                }
            }
            break;
        }
        case 2:
        case 3:
        {
            int16_t cc = search[1];
            cc <<= 8;
            cc |= c;
            __m256i firstchar = _mm256_set1_epi16(cc);
            
            i -= 34;
                //We then scan our string for the first two characters...
            while (i >= 0) {
                for (; i >= 0 && !q; i -= 32) {
                    for (shift = 1; shift >= 0; shift--) {
                        //we load our section, the length should be larger than 16
                        s=src + i + shift;
                        //we read 16 characters at a time, but with a sliding widow
                        //of 15 characters (since we are looking for a double character)
                        current_bytes = _mm256_loadu_si256((const __m256i *)s);
                        //we check if we have a the first character of our string somewhere
                        //in this 16 character window...
                        current_bytes = _mm256_cmpeq_epi16(firstchar, current_bytes);
                        q <<= 1;
                        q |= _mm256_movemask_epi8(current_bytes);
                    }
                }
                
                if (!q)
                    break;
                
                //We search backward
                pos = 30; //32-2
                while (q) {
                    bitscanreverse(j,q);
                    if (j) {
                        pos -= 31-j;
                        q <<= 31-j;
                    }
                    if (lensearch == 2 || s[pos+2] == search[2]) {
                        if ((i+pos+32) <= from)
                            return (i+32+pos);
                    }
                    q <<= 2;
                    pos-=2;
                }
            }
            break;
        }
        case 4:
        case 5:
        case 6:
        case 7:
        {
                //We look for the presence of four characters in a row
            int32_t cc = search[3];
            for (shift = 2; shift >= 0; shift--) {
                cc <<= 8;
                cc |= search[shift];
            }

            __m256i firstchar = _mm256_set1_epi32(cc);
            i -= 36;
                //We then scan our string for the first two characters...
            while (i >= 0) {
                for (; i >= 0 && !q; i -= 32) {
                    //we load our section, the length should be larger than 16
                    for (shift = 3; shift >= 0; shift--) {
                        s=src + i + shift;
                        //we read 16 characters at a time, but with a sliding widow
                        //of 15 characters (since we are looking for a double character)
                        current_bytes = _mm256_loadu_si256((const __m256i *)s);
                        //we check if we have a the first character of our string somewhere
                        //in this 16 character window...
                        current_bytes = _mm256_cmpeq_epi32(firstchar, current_bytes);
                        q <<= 1;
                        q |= _mm256_movemask_epi8(current_bytes);
                    }
                }
                if (!q)
                    break;
                
                pos = 28; //32-4
                while (q) {
                    bitscanreverse(j,q);
                    if (j) {
                        pos -= 31-j;
                        q <<= 31-j;
                    }
                    if (lensearch == 4 || charcomp(s+pos+3,search+3,lensearch-3)) {
                        if ((i+pos+32) <= from)
                            return (i+32+pos);
                    }
                    q <<= 4;
                    pos-=4;
                }
            }
            break;
        }
        default:
        {
                //We look for the presence of 8 characters in a row
            int64_t cc = search[7];
            for (shift = 6; shift >= 0; shift--) {
                cc <<= 8;
                cc |= search[shift];
            }

            __m256i firstchar = _mm256_set1_epi64x(cc);
            i -= 40;
                //We then scan our string for the first two characters...
            while (i >= 0) {
                for (; i >= 0 && !q; i -= 32) {
                    //we load our section, the length should be larger than 16
                    for (shift = 7; shift >= 0; shift--) {
                        s=src + i + shift;
                        //we read 16 characters at a time, but with a sliding widow
                        //of 15 characters (since we are looking for a double character)
                        current_bytes = _mm256_loadu_si256((const __m256i *)s);
                        //we check if we have a the first character of our string somewhere
                        //in this 16 character window...
                        current_bytes = _mm256_cmpeq_epi64(firstchar, current_bytes);
                        q <<= 1;
                        q |= _mm256_movemask_epi8(current_bytes);
                    }
                }
                if (!q)
                    break;
                
                pos = 24; //32-8
                while (q) {
                    bitscanreverse(j,q);
                    if (j) {
                        pos -= 31-j;
                        q <<= 31-j;
                    }
                    if (lensearch == 8 || charcomp(s+pos+7,search+7,lensearch-7)) {
                        if ((i+32+pos) <= from)
                            return (i+32+pos);
                    }
                    q <<= 8;
                    pos -= 8;
                }
            }
        }
    }

    //We did not find anything in a string, which was a multiple of 32
    if (i == -32)
        return -1;
    
    if (lensearch <= 32) {
        for (i = 32-lensearch; i>=0; i--) {
            if (src[i] == c  && i <= from) {
                if (lensearch==1 || charcomp(src+i, search, lensearch))
                    return i;
            }
        }
    }
    
    return -1;
}

void find_intel_all(unsigned char* src, long lensrc, string& search, vector<long>& pos) {
    long lensearch = search.size();
    
    if (lensearch > lensrc)
        return;

        //First we try to find the section in which the first character might occur
    __m256i current_bytes = _mm256_setzero_si256();
    long i = 0;
    uchar* s=NULL;
    unsigned long j;
#ifdef WIN32
	unsigned long q = 0;
#else
    uint32_t q = 0;
#endif
    uchar c = search[0];
    long p;
    long lenmx = lensrc-31;

    char shift;

    switch (lensearch) {
        case 1:
        {
            __m256i firstchar = _mm256_set1_epi8(c);
                //We then scan our string for this first character...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 32) {
                    //we load our section, the length should be larger than 16
                    s=src+i;
                    current_bytes = _mm256_loadu_si256((const __m256i *)s);
                    //we check if we have a the first character of our string somewhere
                    //in this 16 character window...
                    current_bytes = _mm256_cmpeq_epi8(firstchar, current_bytes);
                    q = _mm256_movemask_epi8(current_bytes);
                }
                p = i - 32;
                while (q) {
                    bitscanforward(j,q);
                    if (j) {
                        p+=j;
                        q >>= j;
                    }
                    pos.push_back(p);
                    q >>= 1;
                    p++;
                }
            }
            break;
        }
        case 2:
        case 3:
        {
            int16_t cc = search[1];
            cc <<= 8;
            cc |= c;
            __m256i firstchar = _mm256_set1_epi16(cc);
            
                //We then scan our string for the first two characters...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 32) {
                    for (shift = 1; shift >= 0; shift--) {
                        //we load our section, the length should be larger than 16
                        s=src + i + shift;
                        //we read 16 characters at a time, but with a sliding widow
                        //of 15 characters (since we are looking for a double character)
                        current_bytes = _mm256_loadu_si256((const __m256i *)s);
                        //we check if we have a the first character of our string somewhere
                        //in this 16 character window...
                        current_bytes = _mm256_cmpeq_epi16(firstchar, current_bytes);
                        q <<= 1;
                        q |= _mm256_movemask_epi8(current_bytes);
                    }
                }
                p = i - 32;
                while (q) {
                    bitscanforward(j,q);
                    if (j) {
                        p+=j;
                        s+=j;
                        q >>= j;
                    }
                    if (lensearch == 2 || s[2] == search[2])
                        pos.push_back(p);
                    q >>= 2;
                    s+=2;
                    p+=2;
                }
            }
            break;
        }
        case 4:
        case 5:
        case 6:
        case 7:
        {
                //We look for the presence of four characters in a row
            int32_t cc = search[3];
            for (shift = 2; shift >= 0; shift--) {
                cc <<= 8;
                cc |= search[shift];
            }

            __m256i firstchar = _mm256_set1_epi32(cc);
            uchar* p_search = USTR(search);
            
                //We then scan our string for the first two characters...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 32) {
                    //we load our section, the length should be larger than 16
                    for (shift = 3; shift >= 0; shift--) {
                        s=src + i + shift;
                        //we read 16 characters at a time, but with a sliding widow
                        //of 15 characters (since we are looking for a double character)
                        current_bytes = _mm256_loadu_si256((const __m256i *)s);
                        //we check if we have a the first character of our string somewhere
                        //in this 16 character window...
                        current_bytes = _mm256_cmpeq_epi32(firstchar, current_bytes);
                        q <<= 1;
                        q |= _mm256_movemask_epi8(current_bytes);
                    }
                }
                p = i - 32;
                while (q) {
                    bitscanforward(j,q);
                    if (j) {
                        p+=j;
                        s+=j;
                        q >>= j;
                    }
                    if (lensearch == 4 || charcomp(s+3,p_search+3,lensearch-3))
                        pos.push_back(p);
                    q >>= 4;
                    s+=4;
                    p+=4;
                }
            }
            break;
        }
        default:
        {
                //We look for the presence of eight characters in a row
            int64_t cc = search[7];
            for (shift = 6; shift >= 0; shift--) {
                cc <<= 8;
                cc |= search[shift];
            }
            
            __m256i firstchar = _mm256_set1_epi64x(cc);
            uchar* p_search = USTR(search);
            
                //We then scan our string for the first two characters...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 32) {
                    //we load our section, the length should be larger than 16
                    for (shift = 7; shift >= 0; shift--) {
                        s=src + i + shift;
                        //we read 16 characters at a time, but with a sliding widow
                        //of 15 characters (since we are looking for a double character)
                        current_bytes = _mm256_loadu_si256((const __m256i *)s);
                        //we check if we have a the first character of our string somewhere
                        //in this 16 character window...
                        current_bytes = _mm256_cmpeq_epi64(firstchar, current_bytes);
                        q <<= 1;
                        q |= _mm256_movemask_epi8(current_bytes);
                    }
                }
                p = i - 32;
                while (q) {
                    bitscanforward(j,q);
                    if (j) {
                        p+=j;
                        s+=j;
                        q >>= j;
                    }
                    if (lensearch == 8 || charcomp(s+7,p_search+7,lensearch-7))
                        pos.push_back(p);
                    q >>= 8;
                    s+=8;
                    p+=8;
                }
            }
        }
    }
    
    lensrc-=lensearch;
    for (;i<=lensrc;i++) {
        if (src[i] == c) {
            if (lensearch == 1 || charcomp(src+i,USTR(search),lensearch))
                pos.push_back(i);
        }
    }
}

bool replace_intel_all(string& noe, string& src, string& search, string& replace) {
    long lensrc = src.size();
    long lensearch = search.size();
    
    if (lensearch > lensrc)
        return false;

    long lenneo = 1 + lensrc + (replace.size() << 3);
    long ineo = 0;
    char* neo = new char[lenneo];
    neo[0] = 0;
    
        //First we try to find the section in which the first character might occur
    __m256i current_bytes = _mm256_setzero_si256();

    uchar* s=NULL;
    unsigned long j;
    uchar c = search[0];
#ifdef WIN32
	unsigned long q = 0;
#else
    uint32_t q = 0;
#endif
    long i = 0;
    
    char shift;
    long foundHere;
    long from = 0;
    long lenmx = lensrc-31;
    
    switch (lensearch) {
        case 1:
        {
            __m256i firstchar = _mm256_set1_epi8(c);
                //We then scan our string for this first character...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 32) {
                    //we load our section, the length should be larger than 16
                    s=USTR(src)+i;
                    current_bytes = _mm256_loadu_si256((const __m256i *)s);
                    //we check if we have a the first character of our string somewhere
                    //in this 16 character window...
                    current_bytes = _mm256_cmpeq_epi8(firstchar, current_bytes);
                    q = _mm256_movemask_epi8(current_bytes);
                }
                foundHere = i - 32;
                while (q) {
                    bitscanforward(j,q);
                    if (j) {
                        foundHere+=j;
                        q >>= j;
                    }
                    if (from != foundHere)
                        neo = concatstrings(neo, STR(src)+from, ineo, lenneo, foundHere-from);
                    neo = concatstrings(neo, STR(replace), ineo, lenneo, replace.size());
                    from = foundHere+lensearch;
                    q >>= 1;
                    foundHere++;
                }
            }
            break;
        }
        case 2:
        case 3:
        {
            int16_t cc = search[1];
            cc <<= 8;
            cc |= c;
            __m256i firstchar = _mm256_set1_epi16(cc);
            
            //We then scan our string for the first two characters...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 32) {
                    for (shift = 1; shift >= 0; shift--) {
                        //we load our section, the length should be larger than 16
                        s=USTR(src) + i + shift;
                        //we read 16 characters at a time, but with a sliding widow
                        //of 15 characters (since we are looking for a double character)
                        current_bytes = _mm256_loadu_si256((const __m256i *)s);
                        //we check if we have a the first character of our string somewhere
                        //in this 16 character window...
                        current_bytes = _mm256_cmpeq_epi16(firstchar, current_bytes);
                        q <<= 1;
                        q |= _mm256_movemask_epi8(current_bytes);
                    }
                }
                foundHere = i - 32;
                while (q) {
                    bitscanforward(j,q);
                    if (j) {
                        foundHere+=j;
                        s+=j;
                        q >>= j;
                    }
                    if (lensearch == 2 || s[2] == search[2]) {
                        if (from != foundHere)
                            neo = concatstrings(neo, STR(src)+from, ineo, lenneo, foundHere-from);
                        neo = concatstrings(neo, STR(replace), ineo, lenneo, replace.size());
                        from = foundHere+lensearch;
                    }
                    q >>= 2;
                    s+=2;
                    foundHere+=2;
                }
            }
            break;
        }
        case 4:
        case 5:
        case 6:
        case 7:
        {
                //We look for the presence of four characters in a row
            int32_t cc = search[3];
            for (shift = 2; shift >= 0; shift--) {
                cc <<= 8;
                cc |= search[shift];
            }

            __m256i firstchar = _mm256_set1_epi32(cc);
            uchar* p_search = USTR(search);
            
                //We then scan our string for the first two characters...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 32) {
                    //we load our section, the length should be larger than 16
                    for (shift = 3; shift >= 0; shift--) {
                        s=USTR(src) + i + shift;
                        //we read 16 characters at a time, but with a sliding widow
                        //of 15 characters (since we are looking for a double character)
                        current_bytes = _mm256_loadu_si256((const __m256i *)s);
                        //we check if we have a the first character of our string somewhere
                        //in this 16 character window...
                        current_bytes = _mm256_cmpeq_epi32(firstchar, current_bytes);
                        q <<= 1;
                        q |= _mm256_movemask_epi8(current_bytes);
                    }
                }
                foundHere = i-32;
                while (q) {
                    bitscanforward(j,q);
                    if (j) {
                        foundHere+=j;
                        s+=j;
                        q >>= j;
                    }
                    if (lensearch == 4 || charcomp(s+3,p_search+3,lensearch-3)) {
                        if (from != foundHere)
                            neo = concatstrings(neo, STR(src)+from, ineo, lenneo, foundHere-from);
                        neo = concatstrings(neo, STR(replace), ineo, lenneo, replace.size());
                        from = foundHere+lensearch;
                    }
                    q >>= 4;
                    s+=4;
                    foundHere+=4;
                }
            }
            break;
        }
        default:
        {
                //We look for the presence of 8 characters in a row
            int64_t cc = search[7];
            for (shift = 6; shift >= 0; shift--) {
                cc <<= 8;
                cc |= search[shift];
            }
            
            __m256i firstchar = _mm256_set1_epi64x(cc);
            uchar* p_search = USTR(search);
            
                //We then scan our string for the first two characters...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 32) {
                    //we load our section, the length should be larger than 16
                    for (shift = 7; shift >= 0; shift--) {
                        s=USTR(src) + i + shift;
                        //we read 16 characters at a time, but with a sliding widow
                        //of 15 characters (since we are looking for a double character)
                        current_bytes = _mm256_loadu_si256((const __m256i *)s);
                        //we check if we have a the first character of our string somewhere
                        //in this 16 character window...
                        current_bytes = _mm256_cmpeq_epi64(firstchar, current_bytes);
                        q <<= 1;
                        q |= _mm256_movemask_epi8(current_bytes);
                    }
                }
                foundHere = i - 32;
                while (q) {
                    bitscanforward(j,q);
                    if (j) {
                        foundHere+=j;
                        s+=j;
                        q >>= j;
                    }
                    if (lensearch == 8 || charcomp(s+7,p_search+7,lensearch-7)) {
                        if (from != foundHere)
                            neo = concatstrings(neo, STR(src)+from, ineo, lenneo, foundHere-from);
                        neo = concatstrings(neo, STR(replace), ineo, lenneo, replace.size());
                        from = foundHere+lensearch;
                    }
                    q >>= 8;
                    s+=8;
                    foundHere+=8;
                }
            }
            break;
        }
    }
    
    if (from < lensrc) {
        foundHere = src.find(search, from);
        while (foundHere != string::npos) {
            if (from != foundHere)
                neo = concatstrings(neo, STR(src)+from, ineo, lenneo, foundHere-from);
            neo = concatstrings(neo, STR(replace), ineo, lenneo, replace.size());
            from = foundHere + lensearch;
            foundHere = src.find(search, from);
        }
        
        if (from < lensrc) {
            if (!from) {
                noe = src;
                delete[] neo;
                return true;
            }
            
            neo = concatstrings(neo, STR(src)+from, ineo, lenneo, lensrc-from);
        }
    }
    noe = neo;
    delete[] neo;
    return true;
}


long count_strings_intel(unsigned char* src, unsigned char* search, long lensrc, long lensearch) {
    if (lensearch > lensrc)
        return 0;
    
    //First we try to find the section in which the first character might occur
    __m256i current_bytes = _mm256_setzero_si256();
    
    unsigned int j;
#ifdef WIN32
    unsigned long q = 0;
#else
    uint32_t q = 0;
#endif
    uchar c = search[0];
    long i = 0;
    long counter = 0;
    long lenmx = lensrc-31;
    
    char shift;

    switch (lensearch) {
        case 1:
        {
            __m256i firstchar = _mm256_set1_epi8(c);
            //We then scan our string for this first character...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 32) {
                    //we load our section, the length should be larger than 16
                    current_bytes = _mm256_loadu_si256((const __m256i *)(src+i));
                    //we check if we have the first character of our string somewhere
                    //in this 32 characters window...
                    current_bytes = _mm256_cmpeq_epi8(firstchar, current_bytes);
                    q = _mm256_movemask_epi8(current_bytes);
                }
                while (q) {
                    bitscanforward(j,q);
                    counter++;
                    q >>= j;
                    q >>= 1;
                }
            }
            break;
        }
        case 2:
        case 3:
        {
            int16_t cc = search[1];
            cc <<= 8;
            cc |= c;
            __m256i firstchar = _mm256_set1_epi16(cc);
            uchar* s = src;
            //We then scan our string for the first two characters...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 32) {
                    for (shift = 1; shift >= 0; shift--) {
                        //we load our section, the length should be larger than 16
                        s=src + i + shift;
                        //we read 16 characters at a time, but with a sliding widow
                        //of 15 characters (since we are looking for a double character)
                        current_bytes = _mm256_loadu_si256((const __m256i *)s);
                        //we check if we have a the first character of our string somewhere
                        //in this 16 character window...
                        current_bytes = _mm256_cmpeq_epi16(firstchar, current_bytes);
                        q <<= 1;
                        q |= _mm256_movemask_epi8(current_bytes);
                    }
                }
                while (q) {
                    bitscanforward(j,q);
                    if (j) {
                        s+=j;
                        q >>= j;
                    }
                    if (lensearch == 2 || s[2] == search[2])
                        counter++;
                    q >>= 2;
                    s+=2;
                }
            }
            break;
        }
        case 4:
        case 5:
        case 6:
        case 7:
        {
            //We look for the presence of four characters in a row
            int32_t cc = search[3];
            for (shift = 2; shift >= 0; shift--) {
                cc <<= 8;
                cc |= search[shift];
            }
            uchar* s = src;
            __m256i firstchar = _mm256_set1_epi32(cc);
            
            //We then scan our string for the first two characters...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 32) {
                    //we load our section, the length should be larger than 16
                    for (shift = 3; shift >= 0; shift--) {
                        s=src + i + shift;
                        //we read 16 characters at a time, but with a sliding widow
                        //of 15 characters (since we are looking for a double character)
                        current_bytes = _mm256_loadu_si256((const __m256i *)s);
                        //we check if we have a the first character of our string somewhere
                        //in this 16 character window...
                        current_bytes = _mm256_cmpeq_epi32(firstchar, current_bytes);
                        q <<= 1;
                        q |= _mm256_movemask_epi8(current_bytes);
                    }
                }
                while (q) {
                    bitscanforward(j,q);
                    if (j) {
                        s+=j;
                        q >>= j;
                    }
                    if (lensearch == 4 || charcomp(s+3,search+3,lensearch-3))
                        counter++;
                    q >>= 4;
                    s+=4;
                }
            }
            break;
        }
        default:
        {
            //We look for the presence of eight characters in a row
            int64_t cc = search[7];
            for (shift = 6; shift >= 0; shift--) {
                cc <<= 8;
                cc |= search[shift];
            }
            uchar* s = src;
            __m256i firstchar = _mm256_set1_epi64x(cc);
            
            //We then scan our string for the first two characters...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 32) {
                    //we load our section, the length should be larger than 16
                    for (shift = 7; shift >= 0; shift--) {
                        s=src + i + shift;
                        //we read 16 characters at a time, but with a sliding window
                        //of 15 characters (since we are looking for a double character)
                        current_bytes = _mm256_loadu_si256((const __m256i *)s);
                        //we check if we have a the first character of our string somewhere
                        //in this 16 character window...
                        current_bytes = _mm256_cmpeq_epi64(firstchar, current_bytes);
                        q <<= 1;
                        q |= _mm256_movemask_epi8(current_bytes);
                    }
                }
                while (q) {
                    bitscanforward(j,q);
                    if (j) {
                        s+=j;
                        q >>= j;
                    }
                    if (lensearch == 8 || charcomp(s+7,search+7,lensearch-7))
                        counter++;
                    q >>= 8;
                    s+=8;
                }
            }
        }
    }
    
    lensrc-=lensearch;
    for (;i<=lensrc;i++) {
        if (src[i] == c) {
            if (lensearch == 1 || charcomp(src+i,search,lensearch))
                counter++;
        }
    }
    
    return counter;
}

//-------------------------------------------------------------------------------------
//--- WSTRING SECTION
//-------------------------------------------------------------------------------------

    //We have to take into account the fact that wchar_t is 16 bits on Windows, while it is 32 bits on all other platforms...

    //When we increment by 1 on Windows, we move by increment of 16 bits in memory. 256/16 = 16
//When we increment by 1 on Linux, we move by increment of 32 bits in memory. 256/32 = 8

//To detect if we have a potential large character (an emoji)

bool check_large_char(wchar_t* src, long lensrc, long& i) {
        //First we try to find the section in which the first character might occur
    
    __m256i firstchar = mset_256(large_char);
    __m256i current_bytes = _mm256_setzero_si256();

    for (; (i + doublestringincrement - 1) < lensrc; i += doublestringincrement) {
            //we load our section, the length should be larger than stringincrement
        current_bytes = _mm256_loadu_si256((const __m256i *)(src+i));
        current_bytes = _mm256_or_si256(current_bytes, _mm256_loadu_si256((const __m256i *)(src+i+stringincrement)));
        if (!_mm256_testz_si256(firstchar, current_bytes)) {
            while (!(src[i] & large_char))
                i++;
            return true;
        }
    }

    while (i < lensrc) {
        if ((src[i] & large_char))
            return true;
        i++;
    }

    return false;
}

#ifdef WIN32
#define doublecomp _mm256_cmpeq_epi32
#define doubleset _mm256_set1_epi32
#else
#define doublecomp _mm256_cmpeq_epi64
#define doubleset _mm256_set1_epi64x
#endif

long find_intel(wchar_t* src, wchar_t* search, long lensrc, long lensearch, long i) {
    if (lensearch > lensrc)
        return -1;
    
        //First we try to find the section in which the first character might occur
    
    wchar_t c = search[0];
    wchar_t* s=src;
    long j;
    __m256i current_bytes = _mm256_setzero_si256();
    
    switch (lensearch) {
        case 1:
        {
            __m256i firstchar = mset_256(c);
            
                //We then scan our string for this first character...
            for (; (i + stringincrement - 1) < lensrc; i += stringincrement) {
                    //we load our section, the length should be larger than stringincrement
                s=src+i;
                current_bytes = _mm256_loadu_si256((const __m256i *)s);
                
                    //we check if we have the first character of our string somewhere
                    //in this 8 character window...
                current_bytes = mcomp_256(firstchar, current_bytes);
                if (_mm256_movemask_epi8(current_bytes)) {
                        //Our character is present, we now try to find where it is...
                        //we find it in this section...
                    for (j=0; j < stringincrement; j++) {
                        if (*s == c) {
                            return (i+j);
                        }
                        ++s;
                    }
                }
            }
            break;
        }
#ifdef WIN32
        case 2:
        case 3:
#else
        default:
#endif
        {
            doublechar cc = search[1];
            cc <<= shiftint;
            cc |= c;
            char shift;
            __m256i firstchar = doubleset(cc);
            for (; (i + stringincrement - 1) < lensrc; i += stringincrement) {
                    //we load our section, the length should be larger than stringincrement
                for (shift = 0; shift < 2; shift++) {
                    s=src+i+shift;
                    current_bytes = _mm256_loadu_si256((const __m256i *)s);
                        //we check if we have the first character of our string somewhere
                        //in this 8 character window...
                    current_bytes = doublecomp(firstchar, current_bytes);
                    if (_mm256_movemask_epi8(current_bytes)) {
                            //we find it in this section...
                            //Our character is present, we now try to find where it is...
                            //we find it in this section...
                        for (j=0; j < stringincrement-1; j++) {
                            if (*s == c) {
                                if (wcharcomp(s, search, lensearch))
                                    return (i+j+shift);
                            }
                            ++s;
                        }
                    }
                }
            }
            break;
        }
#ifdef WIN32
            //On Windows, wchar_t are 16 bits longs, which allows us to check 4 characters at a time
        default:
        {
            int64_t cc = search[3];
            char shift;
            for (shift = 2; shift >= 0; shift--) {
                cc <<= 16;
                cc |= search[shift];
            }
            __m256i firstchar = _mm256_set1_epi64x(cc);
            for (; (i + 15) < lensrc; i += 16) {
                    //we load our section, the length should be larger than stringincrement
                for (shift = 0; shift < 4; shift++) {
                    s=src+i+shift;
                    current_bytes = _mm256_loadu_si256((const __m256i *)s);
                        //we check if we have the first character of our string somewhere
                        //in this 8 character window...
                    current_bytes = _mm256_cmpeq_epi64(firstchar, current_bytes);
                    if (_mm256_movemask_epi8(current_bytes)) {
                            //we find it in this section...
                            //Our character is present, we now try to find where it is...
                            //we find it in this section...
                        for (j=0; j < 13; j++) {
                            if (*s == c) {
                                if (wcharcomp(s, search, lensearch))
                                    return (i+j+shift);
                            }
                            ++s;
                        }
                    }
                }
            }
        }
#endif
            
    }
    
    lensrc-=lensearch;
    for (;i<=lensrc;i++) {
        if (src[i] == c) {
            if (lensearch==1 || wcharcomp(src+i, search, lensearch))
                return i;
        }
    }
    
    return -1;
}

long rfind_intel(wchar_t* src, wchar_t* search, long lensrc, long lensearch, long from) {
    if (lensearch > lensrc)
        return -1;
    
    //First we try to find the section in which the first character might occur
    wchar_t c = search[0];

    __m256i current_bytes = _mm256_setzero_si256();
    
    wchar_t* s=src;
    long j;

    long i = from;
    
    if (i >= stringincrement)
        i -= stringincrement;

    switch (lensearch) {
        case 1: {
            __m256i firstchar = mset_256(c);
                //We then scan our string for this last character...
            for (;i>=0; i -= stringincrement) {
                    //we load our section, the length should be larger than 8
                s=src+i;
                current_bytes = _mm256_loadu_si256((const __m256i *)s);
                current_bytes = mcomp_256(firstchar, current_bytes);
                
                if (_mm256_movemask_epi8(current_bytes)) {
                        //we find it in this section...
                    for (j=stringincrement-1; j>=0; j--) {
                        if (s[j] == c && (i+j) <= from) {
                            return(i+j);
                        }
                    }
                }
            }
            break;
        }
#ifdef WIN32
        case 2:
        case 3:
#else
        default:
#endif
        {
            doublechar cc = search[1];
            cc <<= shiftint;
            cc |= c;
            char shift;
            __m256i firstchar = doubleset(cc);
            
            for (;i>=0; i -= stringincrement) {
                    //we load our section, the length should be larger than 8
                for (shift = 0; shift < 2; shift++) {
                    s=src+i+shift;
                    current_bytes = _mm256_loadu_si256((const __m256i *)s);
                        //we check if we have the first character of our string somewhere
                        //in this 8 character window...
                    current_bytes = doublecomp(firstchar, current_bytes);
                    if (_mm256_movemask_epi8(current_bytes)) {
                            //we find it in this section...
                        for (j=stringincrement-2; j>=0; j--) {
                            if (s[j] == c  && (i+j+shift) <= from) {
                                if (wcharcomp(s+j, search, lensearch))
                                    return(i+j+shift);
                            }
                        }
                    }
                }
            }
            break;
        }
#ifdef WIN32
        default:
        {
            int64_t cc = search[3];
            char shift;
            for (shift = 2; shift >= 0; shift--) {
                cc <<= 16;
                cc |= search[shift];
            }
            __m256i firstchar = _mm256_set1_epi64x(cc);
            for (;i>=0; i -= stringincrement) {
                    //we load our section, the length should be larger than stringincrement
                for (shift = 0; shift < 4; shift++) {
                    s=src+i+shift;
                    current_bytes = _mm256_loadu_si256((const __m256i *)s);
                        //we check if we have the first character of our string somewhere
                        //in this 8 character window...
                    current_bytes = _mm256_cmpeq_epi64(firstchar, current_bytes);
                    if (_mm256_movemask_epi8(current_bytes)) {
                            //we find it in this section...
                            //Our character is present, we now try to find where it is...
                            //we find it in this section...
                        for (j=14; j >= 0; j++) {
                            if (*s == c  && (i+j+shift) <= from) {
                                if (wcharcomp(s, search, lensearch))
                                    return (i+j+shift);
                            }
                            ++s;
                        }
                    }
                }
            }
        }
#endif
    }
    
    if ((i*-1) == stringincrement)
        return -1;
    
    if (lensearch <= stringincrement) {
        for (j=stringincrement-lensearch; j>=0; j--) {
            if (src[j] == c && j <= from) {
                if (lensearch==1 || wcharcomp(src+j, search, lensearch))
                    return j;
            }
        }
    }

    return -1;
}

doublechar keystr2(wstring& src, long i) {
    doublechar cc = src[i+1];
    cc <<= shiftint;
    cc |= src[i];
    return cc;
}

void find_intel_all(wchar_t* src, wchar_t* search, long lensrc, long lensearch, vector<long>& pos, long i) {
    if (lensearch > lensrc)
        return;

    //First we try to find the section in which the first character might occur
    
    wchar_t c = search[0];
    wchar_t* s=src;
    long j;
    __m256i current_bytes = _mm256_setzero_si256();

    switch (lensearch) {
        case 1:
        {
            __m256i firstchar = mset_256(c);
            
                //We then scan our string for this first character...
            for (; (i + stringincrement - 1) < lensrc; i += stringincrement) {
                    //we load our section, the length should be larger than stringincrement
                s=src+i;
                current_bytes = _mm256_loadu_si256((const __m256i *)s);
                
                    //we check if we have the first character of our string somewhere
                    //in this 8 character window...
                current_bytes = mcomp_256(firstchar, current_bytes);
                if (_mm256_movemask_epi8(current_bytes)) {
                        //Our character is present, we now try to find where it is...
                        //we find it in this section...
                    for (j=0; j < stringincrement; j++) {
                        if (*s == c) {
                            pos.push_back(i+j);
                        }
                        ++s;
                    }
                }
            }
            break;
        }
#ifdef WIN32
        case 2:
        case 3:
#else
        default:
#endif
        {
            doublechar cc = search[1];
            cc <<= shiftint;
            cc |= c;
            char shift;
            __m256i firstchar = doubleset(cc);
            for (; (i + stringincrement - 1) < lensrc; i += stringincrement) {
                    //we load our section, the length should be larger than stringincrement
                for (shift = 0; shift < 2; shift++) {
                    s=src+i+shift;
                    current_bytes = _mm256_loadu_si256((const __m256i *)s);
                        //we check if we have the first character of our string somewhere
                        //in this 8 character window...
                    current_bytes = doublecomp(firstchar, current_bytes);
                    if (_mm256_movemask_epi8(current_bytes)) {
                            //we find it in this section...
                            //Our character is present, we now try to find where it is...
                            //we find it in this section...
                        for (j=0; j < stringincrement-1; j++) {
                            if (*s == c) {
								if (wcharcomp(s, search, lensearch)) {
									pos.push_back(i + j + shift);
									s += lensearch;
									j += lensearch - 1;
									continue;
								}
                            }
                            ++s;
                        }
						if (!shift && j < stringincrement  && *s == c) {
							--i;
						}
						break;
                    }
                }
            }
            break;
        }
#ifdef WIN32
        default:
        {
            int64_t cc = search[3];
            char shift;
            for (shift = 2; shift >= 0; shift--) {
                cc <<= 16;
                cc |= search[shift];
            }
            __m256i firstchar = _mm256_set1_epi64x(cc);
            for (; (i + 15) < lensrc; i += 16) {
                    //we load our section, the length should be larger than stringincrement
                for (shift = 0; shift < 4; shift++) {
                    s=src+i+shift;
                    current_bytes = _mm256_loadu_si256((const __m256i *)s);
                        //we check if we have the first character of our string somewhere
                        //in this 8 character window...
                    current_bytes = _mm256_cmpeq_epi64(firstchar, current_bytes);
                    if (_mm256_movemask_epi8(current_bytes)) {
                            //we find it in this section...
                            //Our character is present, we now try to find where it is...
                            //we find it in this section...
                        for (j=0; j < 13; j++) {
                            if (*s == c) {
								if (wcharcomp(s, search, lensearch)) {
									pos.push_back(i + j + shift);
									s += lensearch;
									j += lensearch - 1;
									continue;
								}
                            }
                            ++s;
                        }
						for (; j < stringincrement; j++) {
							if (*s == c) {
								i -= stringincrement - j - shift;
								break;
							}
							++s;
						}
						break;
                    }
                }
            }
        }
#endif
    }
    
    lensrc-=lensearch;
    for (;i<=lensrc;i++) {
        if (src[i] == c) {
            if (lensearch==1 || wcharcomp(src+i, search, lensearch))
                pos.push_back(i);
        }
    }
}

void find_intel_all_characters(wchar_t* src, wchar_t* search, long lensrc, long lensearch, vector<long>& pos, long i) {
    if (lensearch > lensrc)
        return;
    
    //First we try to find the section in which the first character might occur
    wchar_t c;
    wchar_t* s=src;
    long j,e;
    __m256i current_bytes = _mm256_setzero_si256();
    
    __m256i firstchar;
    
    //We then scan our string for this first character...
    for (; (i + stringincrement - 1) < lensrc; i += stringincrement) {
        //we load our section, the length should be larger than stringincrement
        s=src+i;
        current_bytes = _mm256_loadu_si256((const __m256i *)s);
        
        //we check if we have the first character of our string somewhere
        //in this 8 character window...
        for (e = 0; e < lensearch; e++) {
            c = search[e];
            firstchar = mset_256(c);
            current_bytes = mcomp_256(firstchar, current_bytes);
            if (_mm256_movemask_epi8(current_bytes)) {
                //Our character is present, we now try to find where it is...
                //we find it in this section...
                for (j=0; j < stringincrement; j++) {
                    if (*s == c) {
                        pos.push_back(i+j);
                    }
                    ++s;
                }
            }
        }
    }
    
    lensrc-=lensearch;
    for (j = i;j<=lensrc;j++) {
        for (e = 0; e < lensearch; e++) {
            if (src[j] == search[e])
                pos.push_back(j);
        }
    }
}

bool replace_intel_all(wstring& noe, wstring& src, wstring& search, wstring& replace) {
    long lensrc = src.size();
    long lensearch = search.size();
    
    if (lensearch > lensrc)
        return false;

        //First we try to find the section in which the first character might occur
    __m256i current_bytes = _mm256_setzero_si256();

    wchar_t c = search[0];
    wchar_t* s = NULL;
    long j;
    long i = 0;

    long from = 0;
    long foundHere;
    
    long lenneo = 1 + lensrc + (replace.size() << 3);
    long ineo = 0;
    wchar_t* neo = new wchar_t[lenneo];
    neo[0] = 0;

    if (lensearch == 1) {
        __m256i firstchar = mset_256(c);
        
            //We then scan our string for this first character...
        for (; (i + stringincrement - 1) < lensrc; i += stringincrement) {
                //we load our section, the length should be larger than stringincrement
            s=WSTR(src)+i;
            current_bytes = _mm256_loadu_si256((const __m256i *)s);
            
                //we check if we have the first character of our string somewhere
                //in this 8 character window...
            current_bytes = mcomp_256(firstchar, current_bytes);
            if (_mm256_movemask_epi8(current_bytes)) {
                    //Our character is present, we now try to find where it is...
                    //we find it in this section...
                for (j=0; j < stringincrement; j++) {
                    if (*s == c) {
                        foundHere = i + j;
                        if (from != foundHere)
                            neo = concatstrings(neo, WSTR(src)+from, ineo, lenneo, foundHere-from);
                        neo = concatstrings(neo, WSTR(replace), ineo, lenneo, replace.size());
                        from = foundHere+1;
                    }
                    ++s;
                }
				break;
            }
        }
    }
    else {
        doublechar cc = search[1];
        cc <<= shiftint;
        cc |= c;
        char shift;
        __m256i firstchar = doubleset(cc);
        for (; (i + stringincrement - 1) < lensrc; i += stringincrement) {
                //we load our section, the length should be larger than stringincrement
            for (shift = 0; shift < 2; shift++) {
                s=WSTR(src)+i+shift;
                current_bytes = _mm256_loadu_si256((const __m256i *)s);
                    //we check if we have the first character of our string somewhere
                    //in this 8 character window...
                current_bytes = doublecomp(firstchar, current_bytes);
                if (_mm256_movemask_epi8(current_bytes)) {
                        //we find it in this section...
                        //Our character is present, we now try to find where it is...
                        //we find it in this section...
                    for (j=0; j < stringincrement-1; j++) {
                        if (*s == c) {
                            if (wcharcomp(s,WSTR(search),lensearch)) {
                                foundHere = i + j + shift;
                                if (from != foundHere)
                                    neo = concatstrings(neo, WSTR(src)+from, ineo, lenneo, foundHere-from);
                                neo = concatstrings(neo, WSTR(replace), ineo, lenneo, replace.size());
								from = foundHere + lensearch;
								s += lensearch;
								j += lensearch - 1;
								continue;
                            }
                        }
                        ++s;
                    }
					if (!shift && j < stringincrement  && *s == c) {
						--i;
					}
					break;
                }
            }
        }
    }
    
    if (from < lensrc) {
        foundHere = src.find(search, from);
        while (foundHere != string::npos) {
            if (from != foundHere)
                neo = concatstrings(neo, WSTR(src)+from, ineo, lenneo, foundHere-from);
            neo = concatstrings(neo, WSTR(replace), ineo, lenneo, replace.size());
            from = foundHere + lensearch;
            foundHere = src.find(search, from);
        }
        
        if (from < lensrc) {
            if (!from) {
                noe = src;
                delete[] neo;
                return true;
            }
            
            neo = concatstrings(neo, WSTR(src)+from, ineo, lenneo, lensrc-from);
        }
    }
    noe = neo;
    delete[] neo;
    return true;
}

long count_strings_intel(wchar_t* src, wchar_t* search, long lensrc, long lensearch) {
    if (lensearch > lensrc)
        return 0;

    //First we try to find the section in which the first character might occur
    
    wchar_t c = search[0];
    wchar_t* s=src;
    long j;
    __m256i current_bytes = _mm256_setzero_si256();

    long i = 0;
    long counter = 0;
    if (lensearch == 1) {
        __m256i firstchar = mset_256(c);
        
        
            //We then scan our string for this first character...
        for (; (i + stringincrement - 1) < lensrc; i += stringincrement) {
                //we load our section, the length should be larger than stringincrement
            s=src+i;
            current_bytes = _mm256_loadu_si256((const __m256i *)s);
            
                //we check if we have the first character of our string somewhere
                //in this 8 character window...
            current_bytes = mcomp_256(firstchar, current_bytes);
            if (_mm256_movemask_epi8(current_bytes)) {
                    //Our character is present, we now try to find where it is...
                    //we find it in this section...
                for (j=0; j < stringincrement; j++) {
                    if (*s == c) {
                        counter++;
                    }
                    ++s;
                }
            }
        }
    }
    else {
        doublechar cc = search[1];
        cc <<= shiftint;
        cc |= c;
        char shift;
        __m256i firstchar = doubleset(cc);
        for (; (i + stringincrement - 1) < lensrc; i += stringincrement) {
                //we load our section, the length should be larger than stringincrement
            for (shift = 0; shift < 2; shift++) {
                s=src+i+shift;
                current_bytes = _mm256_loadu_si256((const __m256i *)s);
                    //we check if we have the first character of our string somewhere
                    //in this 8 character window...
                current_bytes = doublecomp(firstchar, current_bytes);
                if (_mm256_movemask_epi8(current_bytes)) {
                        //we find it in this section...
                        //Our character is present, we now try to find where it is...
                        //we find it in this section...
                    for (j=0; j < stringincrement-1; j++) {
                        if (*s == c) {
							if (wcharcomp(s, search, lensearch)) {
								counter++;
								s += lensearch;
								j += lensearch - 1;
								continue;
							}
                        }
                        ++s;
                    }
					if (!shift && j < stringincrement  && *s == c) {
						--i;
					}
					break;
                }
            }
        }
    }
    
    lensrc-=lensearch;
    for (;i<=lensrc;i++) {
        if (src[i] == c) {
            if (lensearch==1 || wcharcomp(src+i, search, lensearch))
                counter++;
        }
    }
    return counter;
}

//------------------------------------------------------------------------------
// BPE algorithm
//------------------------------------------------------------------------------

void invertkeystr2(doublechar cc, wstring& w) {
#ifdef WIN32
    w = cc & 0x0000FFFF;
#else
    w = cc & 0x00000000FFFFFFFF;
#endif
    w += (cc >> shiftint);
}
#else
//-------------------------------------------------------------------------------------!
//-------------------------------------------------------------------------------------!
//-------------------------------------------------------------------------------------!
// NO AVX SUPPORT, we can only use __128i numbers, which means that for wstring objects,  !
// the increment will be 4 = 128/32                                                    !
// The same applies to regular strings, where the increment will be 16 (128/8)         !
//-------------------------------------------------------------------------------------!
//-------------------------------------------------------------------------------------!
//-------------------------------------------------------------------------------------!

static const __m128i checkutf8byte = _mm_set1_epi8(0x80);

#define thestringincrement 16
#define thewstringincrement 4

bool check_ascii(unsigned char* src, long len, long& i) {
    __m128i current_bytes = _mm_setzero_si128();
    
    i = 0;
    for (; (i + 15) < len; i += 16) {
            //we load our section, the length should be larger than 16
        current_bytes = _mm_loadu_si128((const __m128i *)(src + i));
        current_bytes = _mm_or_si128(current_bytes, _mm_loadu_si128((const __m128i *)(src + i + 16)));
        if (!_mm_testz_si128(checkutf8byte, current_bytes)) {
            while (!(src[i] & 128))
                i++;
            return false;
        }
    }
    
    // last part
    while (i < len) {
        if ((src[i] & 128))
            return false;
        i++;
    }
    
    return true;
    
}

static const __m128i _the_quotes = _mm_set1_epi8(39);
static const __m128i _the_doublequotes = _mm_set1_epi8(34);
static const __m128i _the_slash = _mm_set1_epi8(47);
static const __m128i _the_open = _mm_set1_epi8('(');
static const __m128i _the_close = _mm_set1_epi8(')');
static const __m128i _the_cr = _mm_set1_epi8('\n');


static const __m128i _checkingmore[] = {_the_cr, _the_slash, _the_open, _the_close, _mm_set1_epi8('<'), _mm_set1_epi8('>'), _mm_set1_epi8('='), _mm_set1_epi8(';')};
static const __m128i _tocheck[] = {_mm_set1_epi8('"'), _mm_set1_epi8('\''), _mm_set1_epi8('@'), _mm_set1_epi8(':'), _mm_set1_epi8(','),
    _mm_set1_epi8('-'), _mm_set1_epi8('+'),
    _mm_set1_epi8('0'),_mm_set1_epi8('1'),_mm_set1_epi8('2'),_mm_set1_epi8('3'),_mm_set1_epi8('4'),_mm_set1_epi8('5'), _mm_set1_epi8('6'),_mm_set1_epi8('7'),
    _mm_set1_epi8('8'), _mm_set1_epi8('9'),
    _mm_set1_epi8('['),_mm_set1_epi8(']'),_mm_set1_epi8('{'), _mm_set1_epi8('}')};


void split_container(unsigned char* src, long lensrc, vector<long>& pos, bool forindent) {
    __m128i current_bytes = _mm_setzero_si128();
    __m128i val;
    long b;
#ifdef WIN32
    unsigned long q, r;
#else
    uint32_t q, r;
#endif
    long i = 0;
    long sz = sizeof(_tocheck)/sizeof(__m128i);
    long szcheck = sizeof(_checkingmore)/sizeof(__m128i);

    for (; (i + 15) < lensrc; i += 16) {
        //we load our section, the length should be larger than 32
        current_bytes = _mm_loadu_si128((const __m128i *)(src + i));
        q = 0;
        for (r = 0; r < sz; r++) {
            val =_mm_cmpeq_epi8(current_bytes, _tocheck[r]);
            q |= _mm_movemask_epi8(val);
        }
             
        if (forindent) {
            for (r = 0; r < szcheck; r++) {
                val =_mm_cmpeq_epi8(current_bytes, _checkingmore[r]);
                q |= _mm_movemask_epi8(val);
            }
        }
        
        if (q) {
            b = i;
            while (q) {
                bitscanforward(r, q);
                if (r) {
                    b += r;
                    q >>= r;
                }
                pos.push_back(b);
                q >>= 1;
                b++;
            }
        }
    }
    
    if (i < lensrc) {
        uchar str[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        for (b = i; b < lensrc; b++)
            str[b-i] = src[b];
        
        current_bytes = _mm_loadu_si128((const __m128i *)str);

        q = 0;
        for (r = 0; r < sz; r++) {
            val =_mm_cmpeq_epi8(current_bytes, _tocheck[r]);
            q |= _mm_movemask_epi8(val);
        }
        
        if (forindent) {
            for (r = 0; r < szcheck; r++) {
                val =_mm_cmpeq_epi8(current_bytes, _checkingmore[r]);
                q |= _mm_movemask_epi8(val);
            }
        }

        if (q) {
            b = i;
            while (q) {
                bitscanforward(r, q);
                if (r) {
                    b += r;
                    q >>= r;
                }
                pos.push_back(b);
                q >>= 1;
                b++;
            }
        }
    }
}

void find_quotes(unsigned char* src, long lensrc, vector<long>& pos, bool lisp) {
    __m128i current_bytes = _mm_setzero_si128();
    __m128i val;
    long b;
#ifdef WIN32
    unsigned long q, r;
#else
    uint32_t q, r;
#endif
    long i = 0;
    
    for (; (i + 15) < lensrc; i += 16) {
        //we load our section, the length should be larger than 32
        current_bytes = _mm_loadu_si128((const __m128i *)(src + i));

        val =_mm_cmpeq_epi8(current_bytes, _the_slash);
        q = _mm_movemask_epi8(val);

        if (!lisp) {
            val =_mm_cmpeq_epi8(current_bytes, _the_quotes);
            q |= _mm_movemask_epi8(val);
        }
        
        val =_mm_cmpeq_epi8(current_bytes, _the_cr);
        q |= _mm_movemask_epi8(val);

        val =_mm_cmpeq_epi8(current_bytes, _the_doublequotes);
        q |= _mm_movemask_epi8(val);

        if (q) {
            b = i;
            while (q) {
                bitscanforward(r, q);
                if (r) {
                    b += r;
                    q >>= r;
                }
                pos.push_back(b);
                q >>= 1;
                b++;
            }
        }
    }
    
    if (i < lensrc) {
        uchar str[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        for (b = i; b < lensrc; b++)
            str[b-i] = src[b];
        
        current_bytes = _mm_loadu_si128((const __m128i *)str);
        
        val =_mm_cmpeq_epi8(current_bytes, _the_slash);
        q = _mm_movemask_epi8(val);

        if (!lisp) {
            val =_mm_cmpeq_epi8(current_bytes, _the_quotes);
            q |= _mm_movemask_epi8(val);
        }
        
        val =_mm_cmpeq_epi8(current_bytes, _the_cr);
        q |= _mm_movemask_epi8(val);

        val =_mm_cmpeq_epi8(current_bytes, _the_doublequotes);
        q |= _mm_movemask_epi8(val);

        if (q) {
            b = i;
            while (q) {
                bitscanforward(r, q);
                if (r) {
                    b += r;
                    q >>= r;
                }
                pos.push_back(b);
                q >>= 1;
                b++;
            }
        }
    }
}

long find_intel_byte(unsigned char* src, unsigned char* search, long lensrc, long lensearch, long i) {
    if (lensearch > lensrc)
        return -1;

        //First we try to find the section in which the first character might occur
    __m128i current_bytes = _mm_setzero_si128();
    
    uchar* s=src;
    long j;
    uchar c = search[0];
    char shift;
    uint16_t q = 0;
    long pos;
    long lenmx = lensrc-15;
    
    switch (lensearch) {
        case 1:
        {
            __m128i firstchar = _mm_set1_epi8(c);
                //We then scan our string for this first character...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 16) {
                    //we load our section, the length should be larger than 16
                    s=src+i;
                    current_bytes = _mm_loadu_si128((const __m128i *)s);
                    //we check if we have a the first character of our string somewhere
                    //in this 16 character window...
                    current_bytes = _mm_cmpeq_epi8(firstchar, current_bytes);
                    q = _mm_movemask_epi8(current_bytes);
                }
                if (q) {
                    //Our character is present, we now try to find where it is...
                    //we find it in this section...
                    bitscanforward(j,q);
                    return (i+j-16);
                }
            }
            break;
        }
        case 2:
        case 3:
        {
            int16_t cc = search[1];
            cc <<= 8;
            cc |= c;
            __m128i firstchar = _mm_set1_epi16(cc);
            
                //We then scan our string for the first two characters...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 16) {
                    for (shift = 1; shift >= 0; shift--) {
                        //we load our section, the length should be larger than 16
                        s=src + i + shift;
                        //we read 16 characters at a time, but with a sliding widow
                        //of 15 characters (since we are looking for a double character)
                        current_bytes = _mm_loadu_si128((const __m128i *)s);
                        //we check if we have the first character of our string somewhere
                        //in this 16 character window...
                        current_bytes = _mm_cmpeq_epi16(firstchar, current_bytes);
                        q <<= 1;
                        q |= _mm_movemask_epi8(current_bytes);
                    }
                }
                if (!q)
                    continue;

                if (lensearch == 2) {
                    bitscanforward(j,q);
                    return (i+j-16);
                }
                pos = i-16;
                while (q) {
                    bitscanforward(j,q);
                    if (j) {
                        pos+=j;
                        s+=j;
                        q >>= j;
                    }
                    if (s[2] == search[2])
                        return (pos);
                    q >>= 2;
                    s+=2;
                    pos+=2;
                }
            }
            break;
        }
        default:
        {
            int32_t cc = search[3];
            for (shift = 2; shift >= 0; shift--) {
                cc <<= 8;
                cc |= search[shift];
            }

            __m128i firstchar = _mm_set1_epi32(cc);
            
                //We then scan our string for the first two characters...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 16) {
                    //we load our section, the length should be larger than 16
                    for (shift = 3; shift >=0; shift--) {
                        s=src+i+shift;
                        //we read 16 characters at a time, but with a sliding widow
                        //of 15 characters (since we are looking for a double character)
                        current_bytes = _mm_loadu_si128((const __m128i *)s);
                        //we check if we have a the first character of our string somewhere
                        //in this 16 character window...
                        current_bytes = _mm_cmpeq_epi32(firstchar, current_bytes);
                        q <<= 1;
                        q |= _mm_movemask_epi8(current_bytes);
                    }
                }
                if (!q)
                    continue;
                
                if (lensearch == 4) {
                    bitscanforward(j,q);
                    return (i+j-16);
                }
                pos = i-16;
                while (q) {
                    bitscanforward(j,q);
                    if (j) {
                        pos+=j;
                        s+=j;
                        q >>= j;
                    }
                    if (charcomp(s+3,search+3,lensearch-3))
                        return (pos);
                    q >>= 4;
                    s+=4;
                    pos+=4;
                }
            }
        }
    }
    
    lensrc-=lensearch;
    for (;i<=lensrc;i++) {
        if (src[i] == c) {
            if (lensearch == 1 || charcomp(src+i,search,lensearch))
                return i;
        }
    }

    return -1;
}

long rfind_intel(unsigned char* src, unsigned char* search, long lensrc, long lensearch, long from) {
    if (lensearch > lensrc)
        return -1;
        //First we try to find the section in which the first character might occur
    __m128i current_bytes = _mm_setzero_si128();
    
    uchar* s=src;
    long j;
    uchar c = search[0];
    char shift;
    uint16_t q = 0;

    long pos;
    
    long i = from;
    
    switch (lensearch) {
        case 1:
        {
            __m128i firstchar = _mm_set1_epi8(c);
                //We then scan our string for this first character...
            i -= 16;
            while (i >= 0) {
                for (; i >= 0 && !q; i -= 16) {
                    //we load our section, the length should be larger than 16
                    s=src+i;
                    current_bytes = _mm_loadu_si128((const __m128i *)s);
                    //we check if we have a the first character of our string somewhere
                    //in this 16 character window...
                    current_bytes = _mm_cmpeq_epi8(firstchar, current_bytes);
                    q = _mm_movemask_epi8(current_bytes);
                }
                if (!q)
                    break;
                pos = 15; //16-1
                while (q) {
                    bitscanreverse(j,q);
                    if (j) {
                        pos -= 15-j;
                        q <<= 15-j;
                    }
                    if ((i+pos+16) <= from)
                        return (i+pos+16);
                    q <<= 1;
                    pos--;
                }
            }
            break;
        }
        case 2:
        case 3:
        {
            int16_t cc = search[1];
            cc <<= 8;
            cc |= c;
            __m128i firstchar = _mm_set1_epi16(cc);
            
                //We then scan our string for the first two characters...
            i -= 18;
            while (i >= 0) {
                for (; i >= 0 && !q; i -= 16) {
                    for (shift = 1; shift >= 0; shift--) {
                        //we load our section, the length should be larger than 16
                        s=src + i + shift;
                        //we read 16 characters at a time, but with a sliding widow
                        //of 15 characters (since we are looking for a double character)
                        current_bytes = _mm_loadu_si128((const __m128i *)s);
                        //we check if we have the first character of our string somewhere
                        //in this 16 character window...
                        current_bytes = _mm_cmpeq_epi16(firstchar, current_bytes);
                        q <<= 1;
                        q |= _mm_movemask_epi8(current_bytes);
                    }
                }
                if (!q)
                    break;
                //We search backward
                pos = 14; //16-2
                while (q) {
                    bitscanreverse(j,q);
                    if (j) {
                        pos -= 15-j;
                        q <<= 15-j;
                    }
                    if (lensearch == 2 || s[pos+2] == search[2]) {
                        if ((i+pos+16) <= from)
                            return (i+pos+16);
                    }
                    q <<= 2;
                    pos-=2;
                }
            }
            break;
        }
        default:
        {
            int32_t cc = search[3];
            for (shift = 2; shift >= 0; shift--) {
                cc <<= 8;
                cc |= search[shift];
            }
            
            __m128i firstchar = _mm_set1_epi32(cc);
            
            //We then scan our string for the first two characters...
            i -= 20;
            while (i >= 0) {
                for (; i >= 0 && !q; i -= 16) {
                    //we load our section, the length should be larger than 16
                    for (shift = 3; shift >=0; shift--) {
                        s=src+i+shift;
                        //we read 16 characters at a time, but with a sliding widow
                        //of 15 characters (since we are looking for a double character)
                        current_bytes = _mm_loadu_si128((const __m128i *)s);
                        //we check if we have a the first character of our string somewhere
                        //in this 16 character window...
                        current_bytes = _mm_cmpeq_epi32(firstchar, current_bytes);
                        q <<= 1;
                        q |= _mm_movemask_epi8(current_bytes);
                    }
                }
                if (!q)
                    break;
                
                pos = 12; //16-4
                while (q) {
                    bitscanreverse(j,q);
                    if (j) {
                        pos -= 15-j;
                        q <<= 15-j;
                    }
                    if (lensearch == 4 || charcomp(s+pos+3,search+3,lensearch-3)) {
                        if ((i+pos+16) <= from)
                            return (i+pos+16);
                    }
                    q <<= 4;
                    pos-=4;
                }
            }
        }
    }

    if (i == -16)
        return -1;
    
    if (lensearch <= 16) {
        for (j = 16-lensearch; j>=0; j--) {
            if (src[j] == c && j <= from) {
                if (lensearch==1 || charcomp(src+j, search, lensearch))
                    return j;
            }
        }
    }
    
    return -1;
}

void find_intel_all(uchar* src, long lensrc,  string& search, vector<long>& pos) {
    long lensearch = search.size();
    
    if (lensearch > lensrc)
        return;

        //First we try to find the section in which the first character might occur
    __m128i current_bytes = _mm_setzero_si128();
    long i = 0;
    uint16_t q = 0;
    uchar* s=NULL;
    long j;
    uchar c = search[0];
    char shift;
    long p;
    
    long lenmx = lensrc-15;
    
    switch (lensearch) {
        case 1:
        {
            __m128i firstchar = _mm_set1_epi8(c);
                //We then scan our string for this first character...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 16) {
                    //we load our section, the length should be larger than 16
                    s=src+i;
                    current_bytes = _mm_loadu_si128((const __m128i *)s);
                    //we check if we have a the first character of our string somewhere
                    //in this 16 character window...
                    current_bytes = _mm_cmpeq_epi8(firstchar, current_bytes);
                    q = _mm_movemask_epi8(current_bytes);
                }
                if (!q)
                    break;
                
                p = i-16;
                while (q) {
                    bitscanforward(j,q);
                    if (j) {
                        p+=j;
                        q >>= j;
                    }
                    pos.push_back(p);
                    q >>= 1;
                    p++;
                }
            }
            break;
        }
        case 2:
        case 3:
        {
            int16_t cc = search[1];
            cc <<= 8;
            cc |= c;
            __m128i firstchar = _mm_set1_epi16(cc);
            
                //We then scan our string for the first two characters...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 16) {
                    for (shift = 1; shift >= 0; shift--) {
                        //we load our section, the length should be larger than 16
                        s=src + i + shift;
                        //we read 16 characters at a time, but with a sliding widow
                        //of 15 characters (since we are looking for a double character)
                        current_bytes = _mm_loadu_si128((const __m128i *)s);
                        //we check if we have the first character of our string somewhere
                        //in this 16 character window...
                        current_bytes = _mm_cmpeq_epi16(firstchar, current_bytes);
                        q <<= 1;
                        q |= _mm_movemask_epi8(current_bytes);
                    }
                }
                p = i - 16;
                while (q) {
                    bitscanforward(j,q);
                    if (j) {
                        p+=j;
                        s+=j;
                        q >>= j;
                    }
                    if (lensearch == 2 || s[2] == search[2])
                        pos.push_back(p);
                    q >>= 2;
                    s+=2;
                    p+=2;
                }
            }
            break;
        }
        default:
        {
            int32_t cc = search[3];
            for (shift = 2; shift >= 0; shift--) {
                cc <<= 8;
                cc |= search[shift];
            }

            __m128i firstchar = _mm_set1_epi32(cc);
            uchar* p_search = USTR(search);
            
                //We then scan our string for the first two characters...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 16) {
                    //we load our section, the length should be larger than 16
                    //we load our section, the length should be larger than 16
                    for (shift = 3; shift >=0; shift--) {
                        s=src+i+shift;
                        //we read 16 characters at a time, but with a sliding widow
                        //of 15 characters (since we are looking for a double character)
                        current_bytes = _mm_loadu_si128((const __m128i *)s);
                        //we check if we have a the first character of our string somewhere
                        //in this 16 character window...
                        current_bytes = _mm_cmpeq_epi32(firstchar, current_bytes);
                        q <<= 1;
                        q |= _mm_movemask_epi8(current_bytes);
                    }
                }
                p = i - 16;
                while (q) {
                    bitscanforward(j,q);
                    if (j) {
                        p+=j;
                        s+=j;
                        q >>= j;
                    }
                    if (lensearch == 4 || charcomp(s+3,p_search+3,lensearch-3))
                        pos.push_back(p);
                    q >>= 4;
                    s+=4;
                    p+=4;
                }
            }
        }
    }

    lensrc-=lensearch;
    for (;i<=lensrc;i++) {
        if (src[i] == c) {
            if (lensearch == 1 || charcomp(src+i,USTR(search),lensearch))
                pos.push_back(i);
        }
    }
}

bool replace_intel_all(string& noe, string& src, string& search, string& replace) {
    long lensrc = src.size();
    long lensearch = search.size();
    
    if (lensearch > lensrc)
        return false;

        //First we try to find the section in which the first character might occur
    __m128i current_bytes = _mm_setzero_si128();

    long lenneo = 1 + lensrc + (replace.size() << 3);
    long ineo = 0;
    char* neo = new char[lenneo];
    neo[0] = 0;

    uchar* s=NULL;
    long j;
    uchar c = search[0];
    long i = 0;

    char shift;
    long from = 0;
    long foundHere;
    uint16_t q = 0;
    long lenmx = lensrc-15;
    
    switch (lensearch) {
        case 1:
        {
            __m128i firstchar = _mm_set1_epi8(c);
                //We then scan our string for this first character...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 16) {
                    //we load our section, the length should be larger than 16
                    s=USTR(src)+i;
                    current_bytes = _mm_loadu_si128((const __m128i *)s);
                    //we check if we have a the first character of our string somewhere
                    //in this 16 character window...
                    current_bytes = _mm_cmpeq_epi8(firstchar, current_bytes);
                    q = _mm_movemask_epi8(current_bytes);
                }
                foundHere = i - 16;
                while (q) {
                    bitscanforward(j,q);
                    if (j) {
                        foundHere+=j;
                        q >>= j;
                    }
                    if (from != foundHere)
                        neo = concatstrings(neo, STR(src)+from, ineo, lenneo, foundHere-from);
                    neo = concatstrings(neo, STR(replace), ineo, lenneo, replace.size());
                    from = foundHere+lensearch;
                    q >>= 1;
                    foundHere++;
                }
            }
            break;
        }
        case 2:
        case 3:
        {
            int16_t cc = search[1];
            cc <<= 8;
            cc |= c;
            __m128i firstchar = _mm_set1_epi16(cc);
            uchar* p_src = USTR(src);
            
            //We then scan our string for the first two characters...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 16) {
                    for (shift = 1; shift >= 0; shift--) {
                        //we load our section, the length should be larger than 16
                        s= p_src + i + shift;
                        //we read 16 characters at a time, but with a sliding widow
                        //of 15 characters (since we are looking for a double character)
                        current_bytes = _mm_loadu_si128((const __m128i *)s);
                        //we check if we have the first character of our string somewhere
                        //in this 16 character window...
                        current_bytes = _mm_cmpeq_epi16(firstchar, current_bytes);
                        q <<= 1;
                        q |= _mm_movemask_epi8(current_bytes);
                    }
                }
                foundHere = i - 16;
                while (q) {
                    bitscanforward(j,q);
                    if (j) {
                        foundHere+=j;
                        s+=j;
                        q >>= j;
                    }
                    if (lensearch == 2 || s[2] == search[2]) {
                        if (from != foundHere)
                            neo = concatstrings(neo, STR(src)+from, ineo, lenneo, foundHere-from);
                        neo = concatstrings(neo, STR(replace), ineo, lenneo, replace.size());
                        from = foundHere+lensearch;
                    }
                    q >>= 2;
                    s+=2;
                    foundHere+=2;
                }
            }
            break;
        }
        default:
        {
            int32_t cc = search[3];
            for (shift = 2; shift >= 0; shift--) {
                cc <<= 8;
                cc |= search[shift];
            }
            
            __m128i firstchar = _mm_set1_epi32(cc);
            uchar* p_src = USTR(src);
            uchar* p_search = USTR(search);
            //We then scan our string for the first two characters...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 16) {
                    //we load our section, the length should be larger than 16
                    for (shift = 3; shift >=0; shift--) {
                        s= p_src+i+shift;
                        //we read 16 characters at a time, but with a sliding widow
                        //of 15 characters (since we are looking for a double character)
                        current_bytes = _mm_loadu_si128((const __m128i *)s);
                        //we check if we have a the first character of our string somewhere
                        //in this 16 character window...
                        current_bytes = _mm_cmpeq_epi32(firstchar, current_bytes);
                        q <<= 1;
                        q |= _mm_movemask_epi8(current_bytes);
                    }
                }
                foundHere = i - 16;
                while (q) {
                    bitscanforward(j,q);
                    if (j) {
                        foundHere+=j;
                        s+=j;
                        q >>= j;
                    }
                    if (lensearch == 4 || charcomp(s+3,p_search+3,lensearch-3)) {
                        if (from != foundHere)
                            neo = concatstrings(neo, STR(src)+from, ineo, lenneo, foundHere-from);
                        neo = concatstrings(neo, STR(replace), ineo, lenneo, replace.size());
                        from = foundHere+lensearch;
                    }
                    q >>= 4;
                    s+=4;
                    foundHere+=4;
                }
            }
        }
    }
    
    if (from < lensrc) {
        foundHere = src.find(search, from);
        while (foundHere != string::npos) {
            if (from != foundHere)
                neo = concatstrings(neo, STR(src)+from, ineo, lenneo, foundHere-from);
            neo = concatstrings(neo, STR(replace), ineo, lenneo, replace.size());
            from = foundHere + lensearch;
            foundHere = src.find(search, from);
        }
        
        if (from < lensrc) {
            if (!from) {
                noe = src;
                delete[] neo;
                return true;
            }
            
            neo = concatstrings(neo, STR(src)+from, ineo, lenneo, lensrc-from);
        }
    }
    noe = neo;
    delete[] neo;
    return true;
}


long count_strings_intel(unsigned char* src, unsigned char* search, long lensrc, long lensearch) {
    if (lensearch > lensrc)
        return 0;

    long counter = 0;
        //First we try to find the section in which the first character might occur
    __m128i current_bytes = _mm_setzero_si128();
    
    uchar* s=src;
    long j;
    long i = 0;
    uint16_t q = 0;
    
    uchar c = search[0];
    char shift;
    long lenmx = lensrc-15;
    
    switch (lensearch) {
        case 1:
        {
            __m128i firstchar = _mm_set1_epi8(c);
                //We then scan our string for this first character...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 16) {
                    //we load our section, the length should be larger than 16
                    s=src+i;
                    current_bytes = _mm_loadu_si128((const __m128i *)s);
                    //we check if we have a the first character of our string somewhere
                    //in this 16 character window...
                    current_bytes = _mm_cmpeq_epi8(firstchar, current_bytes);
                    q = _mm_movemask_epi8(current_bytes);
                }
                while (q) {
                    bitscanforward(j,q);
                    counter++;
                    if (j)
                        q >>= j;
                    q >>= 1;
                }
            }
            break;
        }
        case 2:
        case 3:
        {
            int16_t cc = search[1];
            cc <<= 8;
            cc |= c;
            __m128i firstchar = _mm_set1_epi16(cc);
            
            //We then scan our string for the first two characters...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 16) {
                    for (shift = 1; shift >= 0; shift--) {
                        //we load our section, the length should be larger than 16
                        s=src + i + shift;
                        //we read 16 characters at a time, but with a sliding widow
                        //of 15 characters (since we are looking for a double character)
                        current_bytes = _mm_loadu_si128((const __m128i *)s);
                        //we check if we have the first character of our string somewhere
                        //in this 16 character window...
                        current_bytes = _mm_cmpeq_epi16(firstchar, current_bytes);
                        q <<= 1;
                        q |= _mm_movemask_epi8(current_bytes);
                    }
                }
                while (q) {
                    bitscanforward(j,q);
                    if (j) {
                        s+=j;
                        q >>= j;
                    }
                    if (lensearch == 2 || s[2] == search[2])
                        counter++;
                    q >>= 2;
                    s+=2;
                }
            }
            break;
        }
        default:
        {
            int32_t cc = search[3];
            for (shift = 2; shift >= 0; shift--) {
                cc <<= 8;
                cc |= search[shift];
            }
            
            __m128i firstchar = _mm_set1_epi32(cc);
            
            //We then scan our string for the first two characters...
            while (i < lenmx) {
                for (; i < lenmx && !q; i += 16) {
                    //we load our section, the length should be larger than 16
                    for (shift = 3; shift >=0; shift--) {
                        s=src+i+shift;
                        //we read 16 characters at a time, but with a sliding widow
                        //of 15 characters (since we are looking for a double character)
                        current_bytes = _mm_loadu_si128((const __m128i *)s);
                        //we check if we have a the first character of our string somewhere
                        //in this 16 character window...
                        current_bytes = _mm_cmpeq_epi32(firstchar, current_bytes);
                        q <<= 1;
                        q |= _mm_movemask_epi8(current_bytes);
                    }
                }
                while (q) {
                    bitscanforward(j,q);
                    if (j) {
                        s+=j;
                        q >>= j;
                    }
                    if (lensearch == 4 || charcomp(s+3,search+3,lensearch-3))
                        counter++;
                    q >>= 4;
                    s+=4;
                }
            }
        }
    }
    
    lensrc-=lensearch;
    for (;i<=lensrc;i++) {
        if (src[i] == c) {
            if (lensearch == 1 || charcomp(src+i,search,lensearch))
                counter++;
        }
    }
    return counter;
}
//-------------------------------------------------------------------------------------
//--- WSTRING SECTION
//-------------------------------------------------------------------------------------

#define doublestringincrement 12
#define stringincrement 4
#define mset_128 _mm_set1_epi32
#define mcomp_128 _mm_cmpeq_epi32
#define large_char 0x10000

bool check_large_char(wchar_t* src, long lensrc, long& i) {
    __m128i firstchar = _mm_set1_epi32(large_char);
    __m128i current_bytes = _mm_setzero_si128();
    
    //We then scan our string for this first character...
    for (; (i + doublestringincrement - 1) < lensrc; i += doublestringincrement) {
            //we load our section, the length should be larger than stringincrement
        current_bytes = _mm_loadu_si128((const __m128i *)(src+i));
        current_bytes = _mm_or_si128(current_bytes, _mm_loadu_si128((const __m128i *)(src+i+stringincrement)));
        current_bytes = _mm_or_si128(current_bytes, _mm_loadu_si128((const __m128i *)(src+i+stringincrement+stringincrement)));
        if (!_mm_testz_si128(firstchar, current_bytes)) {
            while (!(src[i] & large_char))
                i++;
            return true;
        }
    }
    
    while (i < lensrc) {
        if ((src[i] & large_char))
            return true;
        i++;
    }

    return false;
}


long find_intel(wchar_t* src, wchar_t* search, long lensrc, long lensearch, long i) {
    if (lensearch > lensrc)
        return -1;

        //First we try to find the section in which the first character might occur
    
    wchar_t c = search[0];
    __m128i current_bytes = _mm_setzero_si128();
    
    wchar_t* s=src;
    long j;
    if (lensearch == 1) {
        __m128i firstchar = _mm_set1_epi8(c);
        
            //We then scan our string for this first character...
        for (; (i + stringincrement - 1) < lensrc; i += stringincrement) {
                //we load our section, the length should be larger than 4
            s=src+i;
            current_bytes = _mm_loadu_si128((const __m128i *)s);
            
                //we check if we have a the first character of our string somewhere
                //in this 16 character window...
            current_bytes = _mm_cmpeq_epi8(firstchar, current_bytes);
            if (_mm_movemask_epi8(current_bytes)) {
                    //Our character is present, we now try to find where it is...
                    //we find it in this section...
                for (j=0; j < stringincrement; j++) {
                    if (*s == c) {
                        return (i+j);
                    }
                    ++s;
                }
            }
        }
    }
    else {
        uint64_t cc = search[1];
        cc <<= 32;
        cc |= c;
        __m128i firstchar = _mm_set1_epi64x(cc);
        char shift;
            //We then scan our string for this first character...
        for (; (i + stringincrement - 1) < lensrc; i += stringincrement) {
                //we load our section, the length should be larger than 4
            for (shift = 0; shift < 2; shift++) {
                s=src+i+shift;
                current_bytes = _mm_loadu_si128((const __m128i *)s);
                
                    //we check if we have a the first character of our string somewhere
                    //in this 16 character window...
                current_bytes = _mm_cmpeq_epi64(firstchar, current_bytes);
                if (_mm_movemask_epi8(current_bytes)) {
                        //we find it in this section...
                        //Our character is present, we now try to find where it is...
                        //we find it in this section...
                    for (j=0; j < stringincrement-1; j++) {
                        if (*s == c) {
                            if (wcharcomp(s, search, lensearch))
                                return (i+j+shift);
                        }
                        ++s;
                    }
                }
            }
        }
    }
    
    lensrc-=lensearch;
    for (;i<=lensrc;i++) {
        if (src[i] == c) {
            if (lensearch==1 || wcharcomp(src+i, search, lensearch))
                return i;
        }
    }
    
    return -1;
}

long rfind_intel(wchar_t* src, wchar_t* search, long lensrc, long lensearch, long from) {
    if (lensearch > lensrc)
        return -1;

        //First we try to find the section in which the first character might occur
    wchar_t c = search[0];
    
    __m128i current_bytes = _mm_setzero_si128();
    
    wchar_t* s=src;
    long j;
    
    long i = from;
    if (i >= stringincrement)
        i -= stringincrement;

    if (lensearch == 1) {
        __m128i firstchar = mset_128(c);
        //We then scan our string for this first character...
        for (;i>=0; i -= stringincrement) {
                //we load our section, the length should be larger than 16
            s=src+i;
            
            current_bytes = _mm_loadu_si128((const __m128i *)s);
            
                //we check if we have a the first character of our string somewhere
                //in this 16 character window...
            current_bytes = mcomp_128(firstchar, current_bytes);
            if (_mm_movemask_epi8(current_bytes)) {
                    //we find it in this section...
                for (j = stringincrement - 1; j>=0; j--) {
                    if (s[j] == c  && (i+j) <= from) {
                        return(i+j);
                    }
                }
            }
        }
    }
    else {
        uint64_t cc = search[1];
        cc <<= 32;
        cc |= c;
        __m128i firstchar = _mm_set1_epi64x(cc);
        char shift;
        
        for (;i>=0; i -= stringincrement) {
            
            for (shift = 0; shift < 2; shift++) {
                s=src+i+shift;
                current_bytes = _mm_loadu_si128((const __m128i *)s);
                
                    //we check if we have a the first character of our string somewhere
                    //in this 16 character window...
                current_bytes = _mm_cmpeq_epi64(firstchar, current_bytes);
                if (_mm_movemask_epi8(current_bytes)) {
                        //we find it in this section...
                        //we find it in this section...
                    for (j = stringincrement - 2; j>=0; j--) {
                        if (s[j] == c && (i+j+shift) <= from) {
                            if (wcharcomp(s+j, search, lensearch))
                                return(i+j+shift);
                        }
                    }
                }
            }
        }
    }

    if ((i*-1) == stringincrement)
        return -1;
    
    if (lensearch <= stringincrement) {
        for (j = stringincrement-lensearch; j>=0; j--) {
            if (src[j] == c && j <= from) {
                if (lensearch==1 || wcharcomp(src+j, search, lensearch))
                    return j;
            }
        }
    }
    return -1;
}

void find_intel_all(wchar_t* src, wchar_t* search, long lensrc, long lensearch, vector<long>& pos, long i) {
    if (lensearch > lensrc)
        return;

        //First we try to find the section in which the first character might occur
    
    wchar_t c = search[0];
    __m128i current_bytes = _mm_setzero_si128();
    
    wchar_t* s=src;
    long j;
    if (lensearch == 1) {
        __m128i firstchar = _mm_set1_epi8(c);
        
            //We then scan our string for this first character...
        for (; (i + stringincrement - 1) < lensrc; i += stringincrement) {
                //we load our section, the length should be larger than 4
            s=src+i;
            current_bytes = _mm_loadu_si128((const __m128i *)s);
            
                //we check if we have a the first character of our string somewhere
                //in this 16 character window...
            current_bytes = _mm_cmpeq_epi8(firstchar, current_bytes);
            if (_mm_movemask_epi8(current_bytes)) {
                    //Our character is present, we now try to find where it is...
                    //we find it in this section...
                for (j=0; j < stringincrement; j++) {
                    if (*s == c) {
                        pos.push_back(i+j);
                    }
                    ++s;
                }
            }
        }
    }
    else {
        uint64_t cc = search[1];
        cc <<= 32;
        cc |= c;
        __m128i firstchar = _mm_set1_epi64x(cc);
        char shift;
            //We then scan our string for this first character...
        for (; (i + stringincrement - 1) < lensrc; i += stringincrement) {
                //we load our section, the length should be larger than 4
            for (shift = 0; shift < 2; shift++) {
                s=src+i+shift;
                current_bytes = _mm_loadu_si128((const __m128i *)s);
                
                    //we check if we have a the first character of our string somewhere
                    //in this 16 character window...
                current_bytes = _mm_cmpeq_epi64(firstchar, current_bytes);
                if (_mm_movemask_epi8(current_bytes)) {
                        //we find it in this section...
                        //Our character is present, we now try to find where it is...
                        //we find it in this section...
                    for (j=0; j < stringincrement-1; j++) {
                        if (*s == c) {
                            if (wcharcomp(s, search, lensearch)) {
                                pos.push_back(i+j+shift);
                                s += lensearch;
                                j += lensearch - 1;
                            }
                        }
                        ++s;
                    }
                    if (!shift && j < stringincrement && *s == c)
                        --i;
                    break;
                }
            }
        }
    }
    
    lensrc-=lensearch;
    for (;i<=lensrc;i++) {
        if (src[i] == c) {
            if (lensearch==1 || wcharcomp(src+i, search, lensearch))
                pos.push_back(i);
        }
    }
}

void find_intel_all_characters(wchar_t* src, wchar_t* search, long lensrc, long lensearch, vector<long>& pos, long i) {
    if (lensearch > lensrc)
        return;
    
    //First we try to find the section in which the first character might occur
    wchar_t c;
    wchar_t* s=src;
    long j,e;
    __m128i current_bytes = _mm_setzero_si128();

    __m128i firstchar;

    //We then scan our string for this first character...
    for (; (i + stringincrement - 1) < lensrc; i += stringincrement) {
        //we load our section, the length should be larger than stringincrement
        s=src+i;
        current_bytes = _mm_loadu_si128((const __m128i *)s);
        
        //we check if we have the first character of our string somewhere
        //in this 8 character window...
        for (e = 0; e < lensearch; e++) {
            c = search[e];
            firstchar = _mm_set1_epi8(c);
            current_bytes = _mm_cmpeq_epi8(firstchar, current_bytes);
            if (_mm_movemask_epi8(current_bytes)) {
                //Our character is present, we now try to find where it is...
                //we find it in this section...
                for (j=0; j < stringincrement; j++) {
                    if (*s == c) {
                        pos.push_back(i+j);
                    }
                    ++s;
                }
            }
        }
    }
    
    lensrc-=lensearch;
    for (j = i;j<=lensrc;j++) {
        for (e = 0; e < lensearch; e++) {
            if (src[j] == search[e])
                pos.push_back(j);
        }
    }
}

bool replace_intel_all(wstring& noe, wstring& src, wstring& search, wstring& replace) {
    long lensrc = src.size();
    long lensearch = search.size();
    
    if (lensearch > lensrc)
        return false;

        //First we try to find the section in which the first character might occur
    __m128i current_bytes = _mm_setzero_si128();

    wchar_t c = search[0];
    wchar_t* s = NULL;
    long j;
    long i = 0;

    long from = 0;
    long foundHere;
    
    long lenneo = 1 + lensrc + (replace.size() << 3);
    long ineo = 0;
    wchar_t* neo = new wchar_t[lenneo];
    neo[0] = 0;

    if (lensearch == 1) {
        __m128i firstchar = _mm_set1_epi8(c);
        
            //We then scan our string for this first character...
        for (; (i + stringincrement - 1) < lensrc; i += stringincrement) {
                //we load our section, the length should be larger than 4
            s=WSTR(src)+i;
            current_bytes = _mm_loadu_si128((const __m128i *)s);
            
                //we check if we have a the first character of our string somewhere
                //in this 16 character window...
            current_bytes = _mm_cmpeq_epi8(firstchar, current_bytes);
            if (_mm_movemask_epi8(current_bytes)) {
                    //Our character is present, we now try to find where it is...
                    //we find it in this section...
                for (j=0; j < stringincrement; j++) {
                    if (*s == c) {
                        foundHere = i + j;
                        if (from != foundHere)
                            neo = concatstrings(neo, WSTR(src)+from, ineo, lenneo, foundHere-from);
                        neo = concatstrings(neo, WSTR(replace), ineo, lenneo, replace.size());
                        from = foundHere+1;
                    }
                    ++s;
                }
                break;
            }
        }
    }
    else {
        uint64_t cc = search[1];
        cc <<= 32;
        cc |= c;
        __m128i firstchar = _mm_set1_epi64x(cc);
        char shift;
            //We then scan our string for this first character...
        for (; (i + stringincrement - 1) < lensrc; i += stringincrement) {
                //we load our section, the length should be larger than 4
            for (shift = 0; shift < 2; shift++) {
                s=WSTR(src)+i+shift;
                current_bytes = _mm_loadu_si128((const __m128i *)s);
                
                    //we check if we have a the first character of our string somewhere
                    //in this 16 character window...
                current_bytes = _mm_cmpeq_epi64(firstchar, current_bytes);
                if (_mm_movemask_epi8(current_bytes)) {
                        //we find it in this section...
                        //Our character is present, we now try to find where it is...
                        //we find it in this section...
                    for (j=0; j < stringincrement-1; j++) {
                        if (*s == c) {
                            if (wcharcomp(s,WSTR(search),lensearch)) {
                                foundHere = i + j + shift;
                                if (from != foundHere)
                                    neo = concatstrings(neo, WSTR(src)+from, ineo, lenneo, foundHere-from);
                                neo = concatstrings(neo, WSTR(replace), ineo, lenneo, replace.size());
                                from = foundHere+lensearch;
                                s += lensearch;
                                j += lensearch - 1;
                            }
                        }
                        ++s;
                    }
                    if (!shift && j < stringincrement && *s == c)
                        --i;
                    break;
                }
            }
        }
    }
    
    if (from < lensrc) {
        foundHere = src.find(search, from);
        while (foundHere != string::npos) {
            if (from != foundHere)
                neo = concatstrings(neo, WSTR(src)+from, ineo, lenneo, foundHere-from);
            neo = concatstrings(neo, WSTR(replace), ineo, lenneo, replace.size());
            from = foundHere + lensearch;
            foundHere = src.find(search, from);
        }
        
        if (from < lensrc) {
            if (!from) {
                noe = src;
                delete[] neo;
                return true;
            }
            
            neo = concatstrings(neo, WSTR(src)+from, ineo, lenneo, lensrc-from);
        }
    }
    noe = neo;
    delete[] neo;
    return true;
}



long count_strings_intel(wchar_t* src, wchar_t* search, long lensrc, long lensearch) {
if (lensearch > lensrc)
    return 0;
    long counter = 0;
    long i = 0;
    
    
        //First we try to find the section in which the first character might occur
    
    wchar_t c = search[0];
    __m128i current_bytes = _mm_setzero_si128();
    
    wchar_t* s=src;
    long j;
    if (lensearch == 1) {
        __m128i firstchar = _mm_set1_epi8(c);
        
            //We then scan our string for this first character...
        for (; (i + stringincrement - 1) < lensrc; i += stringincrement) {
                //we load our section, the length should be larger than 4
            s=src+i;
            current_bytes = _mm_loadu_si128((const __m128i *)s);
            
                //we check if we have a the first character of our string somewhere
                //in this 16 character window...
            current_bytes = _mm_cmpeq_epi8(firstchar, current_bytes);
            if (_mm_movemask_epi8(current_bytes)) {
                    //Our character is present, we now try to find where it is...
                    //we find it in this section...
                for (j=0; j < stringincrement; j++) {
                    if (*s == c) {
                        counter++;
                    }
                    ++s;
                }
            }
        }
    }
    else {
        uint64_t cc = search[1];
        cc <<= 32;
        cc |= c;
        __m128i firstchar = _mm_set1_epi64x(cc);
        char shift;
            //We then scan our string for this first character...
        for (; (i + stringincrement - 1) < lensrc; i += stringincrement) {
                //we load our section, the length should be larger than 4
            for (shift = 0; shift < 2; shift++) {
                s=src+i+shift;
                current_bytes = _mm_loadu_si128((const __m128i *)s);
                
                    //we check if we have a the first character of our string somewhere
                    //in this 16 character window...
                current_bytes = _mm_cmpeq_epi64(firstchar, current_bytes);
                if (_mm_movemask_epi8(current_bytes)) {
                        //we find it in this section...
                        //Our character is present, we now try to find where it is...
                        //we find it in this section...
                    for (j=0; j < stringincrement-1; j++) {
                        if (*s == c) {
                            if (wcharcomp(s, search, lensearch)) {
                                counter++;
                                s += lensearch;
                                j += lensearch - 1;
                            }
                        }
                        ++s;
                    }
                    if (!shift && j < stringincrement && *s == c)
                        --i;
                    break;
                }
            }
        }
    }
    
    lensrc-=lensearch;
    for (;i<=lensrc;i++) {
        if (src[i] == c) {
            if (lensearch==1 || wcharcomp(src+i, search, lensearch))
                counter++;
        }
    }
    
    return counter;
}
#endif
#else
static const char _tocheck[] = {'"', '\'', '@', ':', ',','-', '+','0','1','2','3','4','5', '6','7','8', '9','[',']','{', '}', 0};
static const char _checkingmore[] = {'\n', '/', '(', ')', '<', '>','=',';', 0};


void split_container(unsigned char* src, long lensrc, vector<long>& pos, bool forindent) {
    uchar c;
    
    for (long e = 0; e < lensrc; e++) {
        c = src[e];
        if (forindent && strchr(_checkingmore, c))
            pos.push_back(e);
        else
            if (strchr(_tocheck, c))
                pos.push_back(e);
    }
}

void find_quotes(unsigned char* src, long lensrc, vector<long>& pos, bool lisp) {
    uchar c;

    for (long e = 0; e < lensrc; e++) {
        c = src[e];
        
        if (lisp && c == '\'')
            continue;
        
        if (c == '/' || c == '"' || c == '\'' || c == '\n')
            pos.push_back(e);
    }
}
#endif

//------------------------------------------------------------------------
void bytepairencoding(wstring& str, long nb, hmap<wstring, long>& dicos) {
    long sz = str.size();
    //First 4 characters...
    wstring s;

    for (long i = 0; i < sz; i++) {
        //first we check if we have a byte pair encoding
        if (nb > 2) {
            s = str.substr(i, nb-1);
            if (c_is_space(s.back()) || c_is_punctuation(s.back()) ||  dicos.find(s) == dicos.end()) {
                i += nb - 1;
                continue;
            }
        }
        
        s = str.substr(i, nb);
        if (dicos.find(s) == dicos.end())
            dicos[s] = 1;
        else
            ++dicos[s];
    }
}

void bytepairreplace(wstring& str, wstring& res, long nbmax, hmap<wstring, long>& dicos) {
    long sz = str.size();
    //First 4 characters...
    wstring s;
    long j;
    bool check;
    for (long i = 0; i < sz; i++) {
        check = false;
        for (j = nbmax; j >= 2; j--) {
            s =  str.substr(i, j);
            if (dicos.find(s) != dicos.end()) {
                if (s[0] == ' ')
                    s[0] = '_';
                else
                    if (!i || str[i-1] == ' ')
                        res += L"▁";
                res += s;
                res += ' ';
                i += j-1;
                check = true;
                break;
            }
        }
        if (!check)
            res += str[i];
    }
}

//------------------------------------------------------------------------

extern double tamgunan;

static TAMGUCHAR latincodetable[] = {
    133, 0x2026, 160, 0x00A0, 161, 0x00A1, 166, 0x00A6, 170, 0x00AA, 172, 0x00AC, 173, 0x00AD, 175, 0x00AF, 176, 0x00B0, 177, 0x00B1,
    181, 0x00B5, 182, 0x00B6, 183, 0x00B7, 184, 0x00B8, 186, 0x00BA, 191, 0x00BF, 215, 0x00D7, 247, 0x00F7, 223, 0x00DF, 150, 0x2013, 151, 0x2014, 171, 0x00AB,
    187, 0x00BB, 139, 0x3008, 155, 0x3009, 134, 0x2020, 135, 0x2021, 162, 0x00A2, 163, 0x00A3, 164, 0x00A4, 128, 0x20AC, 165, 0x00A5, 188, 0x00BC, 189, 0x00BD,
    190, 0x00BE, 185, 0x00B9, 178, 0x00B2, 179, 0x00B3, 39, 0x2018, 39, 0x2019, 34, 0x201C, 34, 0x201D, 0, 0 };

static TAMGUCHAR latin2codetable[] = {
    161, 0x0104, 162, 0x02D8, 163, 0x0141, 164, 0x00A4, 165, 0x013D, 166, 0x015A, 167, 0x00A7, 168, 0x00A8, 169, 0x0160, 170, 0x015E,
    171, 0x0164, 172, 0x0179, 174, 0x017D, 175, 0x017B, 176, 0x00B0, 177, 0x0105, 178, 0x02DB, 179, 0x0142, 180, 0x00B4,
    181, 0x013E, 182, 0x015B, 183, 0x02C7, 184, 0x00B8, 185, 0x0161, 186, 0x015F, 187, 0x0165, 188, 0x017A, 189, 0x02DD, 190, 0x017E,
    191, 0x017C, 192, 0x0154, 193, 0x00C1, 194, 0x00C2, 195, 0x0102, 196, 0x00C4, 197, 0x0139, 198, 0x0106, 199, 0x00C7, 200, 0x010C,
    201, 0x00C9, 202, 0x0118, 203, 0x00CB, 204, 0x011A, 205, 0x00CD, 206, 0x00CE, 207, 0x010E, 208, 0x0110, 209, 0x0143, 210, 0x0147,
    211, 0x00D3, 212, 0x00D4, 213, 0x0150, 214, 0x00D6, 215, 0x00D7, 216, 0x0158, 217, 0x016E, 218, 0x00DA, 219, 0x0170, 220, 0x00DC,
    221, 0x00DD, 222, 0x0162, 223, 0x00DF, 224, 0x0155, 225, 0x00E1, 226, 0x00E2, 227, 0x0103, 228, 0x00E4, 229, 0x013A, 230, 0x0107,
    231, 0x00E7, 232, 0x010D, 233, 0x00E9, 234, 0x0119, 235, 0x00EB, 236, 0x011B, 237, 0x00ED, 238, 0x00EE, 239, 0x010F, 240, 0x0111,
    241, 0x0144, 242, 0x0148, 243, 0x00F3, 244, 0x00F4, 245, 0x0151, 246, 0x00F6, 247, 0x00F7, 248, 0x0159, 249, 0x016F, 250, 0x00FA,
    251, 0x0171, 252, 0x00FC, 253, 0x00FD, 254, 0x0163, 255, 0x02D9,
    0, 0 };


static TAMGUCHAR latin3codetable[] = {
    161, 0x0126, 162, 0x02D8, 163, 0x00A3, 164, 0x00A4, 166, 0x0124, 167, 0x00A7, 168, 0x00A8, 169, 0x0130, 170, 0x015E, 171, 0x011E,
    172, 0x0134, 175, 0x017B, 176, 0x00B0, 177, 0x0127, 178, 0x00B2, 179, 0x00B3, 180, 0x00B4, 181, 0x00B5, 182, 0x0125,
    183, 0x00B7, 184, 0x00B8, 185, 0x0131, 186, 0x015F, 187, 0x011F, 188, 0x0135, 189, 0x00BD, 191, 0x017C, 192, 0x00C0, 193, 0x00C1,
    194, 0x00C2, 196, 0x00C4, 197, 0x010A, 198, 0x0108, 199, 0x00C7, 200, 0x00C8, 201, 0x00C9, 202, 0x00CA, 203, 0x00CB, 204, 0x00CC,
    205, 0x00CD, 206, 0x00CE, 207, 0x00CF, 209, 0x00D1, 210, 0x00D2, 211, 0x00D3, 212, 0x00D4, 213, 0x0120, 214, 0x00D6, 215, 0x00D7,
    216, 0x011C, 217, 0x00D9, 218, 0x00DA, 219, 0x00DB, 220, 0x00DC, 221, 0x016C, 222, 0x015C, 223, 0x00DF, 224, 0x00E0, 225, 0x00E1,
    226, 0x00E2, 228, 0x00E4, 229, 0x010B, 230, 0x0109, 231, 0x00E7, 232, 0x00E8, 233, 0x00E9, 234, 0x00EA, 235, 0x00EB, 236, 0x00EC,
    237, 0x00ED, 238, 0x00EE, 239, 0x00EF, 241, 0x00F1, 242, 0x00F2, 243, 0x00F3, 244, 0x00F4, 245, 0x0121, 246, 0x00F6, 247, 0x00F7,
    248, 0x011D, 249, 0x00F9, 250, 0x00FA, 251, 0x00FB, 252, 0x00FC, 253, 0x016D, 254, 0x015D, 255, 0x02D9,
    0, 0 };

static TAMGUCHAR latin4codetable[] = {
    161, 0x0104, 162, 0x0138, 163, 0x0156, 164, 0x00A4, 165, 0x0128, 166, 0x013B, 167, 0x00A7, 168, 0x00A8, 169, 0x0160, 170, 0x0112,
    171, 0x0122, 172, 0x0166, 174, 0x017D, 175, 0x00AF, 176, 0x00B0, 177, 0x0105, 178, 0x02DB, 179, 0x0157, 180, 0x00B4,
    181, 0x0129, 182, 0x013C, 183, 0x02C7, 184, 0x00B8, 185, 0x0161, 186, 0x0113, 187, 0x0123, 188, 0x0167, 189, 0x014A, 190, 0x017E,
    191, 0x014B, 192, 0x0100, 193, 0x00C1, 194, 0x00C2, 195, 0x00C3, 196, 0x00C4, 197, 0x00C5, 198, 0x00C6, 199, 0x012E, 200, 0x010C,
    201, 0x00C9, 202, 0x0118, 203, 0x00CB, 204, 0x0116, 205, 0x00CD, 206, 0x00CE, 207, 0x012A, 208, 0x0110, 209, 0x0145, 210, 0x014C,
    211, 0x0136, 212, 0x00D4, 213, 0x00D5, 214, 0x00D6, 215, 0x00D7, 216, 0x00D8, 217, 0x0172, 218, 0x00DA, 219, 0x00DB, 220, 0x00DC,
    221, 0x0168, 222, 0x016A, 223, 0x00DF, 224, 0x0101, 225, 0x00E1, 226, 0x00E2, 227, 0x00E3, 228, 0x00E4, 229, 0x00E5, 230, 0x00E6,
    231, 0x012F, 232, 0x010D, 233, 0x00E9, 234, 0x0119, 235, 0x00EB, 236, 0x0117, 237, 0x00ED, 238, 0x00EE, 239, 0x012B, 240, 0x0111,
    241, 0x0146, 242, 0x014D, 243, 0x0137, 244, 0x00F4, 245, 0x00F5, 246, 0x00F6, 247, 0x00F7, 248, 0x00F8, 249, 0x0173, 250, 0x00FA,
    251, 0x00FB, 252, 0x00FC, 253, 0x0169, 254, 0x016B, 255, 0x02D9,
    0, 0 };

static TAMGUCHAR latin5codetable[] = {
    161, 0x401, 162, 0x402, 163, 0x403, 164, 0x404, 165, 0x405, 166, 0x406, 167, 0x407, 168, 0x408, 169, 0x409,
    170, 0x40A, 171, 0x40B, 172, 0x40C, 173, 0x3, 174, 0x40E, 175, 0x40F, 176, 0x410, 177, 0x411, 178, 0x412, 179, 0x413, 180, 0x414, 181, 0x415,
    182, 0x416, 183, 0x417, 184, 0x418, 185, 0x419, 186, 0x41A, 187, 0x41B, 188, 0x41C, 189, 0x41D, 190, 0x41E, 191, 0x41F, 192, 0x420, 193, 0x421,
    194, 0x422, 195, 0x423, 196, 0x424, 197, 0x425, 198, 0x426, 199, 0x427, 200, 0x428, 201, 0x429, 202, 0x42A, 203, 0x42B, 204, 0x42C, 205, 0x42D,
    206, 0x42E, 207, 0x42F, 208, 0x430, 209, 0x431, 210, 0x432, 211, 0x433, 212, 0x434, 213, 0x435, 214, 0x436, 215, 0x437, 216, 0x438, 217, 0x439,
    218, 0x43A, 219, 0x43B, 220, 0x43C, 221, 0x43D, 222, 0x43E, 223, 0x43F, 224, 0x440, 225, 0x441, 226, 0x442, 227, 0x443, 228, 0x444, 229, 0x445,
    230, 0x446, 231, 0x447, 232, 0x448, 233, 0x449, 234, 0x44A, 235, 0x44B, 236, 0x44C, 237, 0x44D, 238, 0x44E, 239, 0x44F, 240, 0x2116, 241, 0x451, 242, 0x452,
    243, 0x453, 244, 0x454, 245, 0x455, 246, 0x456, 247, 0x457, 248, 0x458, 249, 0x459, 250, 0x45A, 251, 0x45B, 252, 0x45C, 253, 0xA7, 254, 0x45E, 255, 0x45F,
    0, 0 };

static TAMGUCHAR latin6codetable[] = {
    164, 0x00A4, 172, 0x060C, 187, 0x061B, 191, 0x061F, 193, 0x0621, 194, 0x0622, 195, 0x0623, 196, 0x0624, 197, 0x0625,
    198, 0x0626, 199, 0x0627, 200, 0x0628, 201, 0x0629, 202, 0x062A, 203, 0x062B, 204, 0x062C, 205, 0x062D, 206, 0x062E, 207, 0x062F,
    208, 0x0630, 209, 0x0631, 210, 0x0632, 211, 0x0633, 212, 0x0634, 213, 0x0635, 214, 0x0636, 215, 0x0637, 216, 0x0638, 217, 0x0639,
    218, 0x063A, 224, 0x0640, 225, 0x0641, 226, 0x0642, 227, 0x0643, 228, 0x0644, 229, 0x0645, 230, 0x0646, 231, 0x0647, 232, 0x0648,
    233, 0x0649, 234, 0x064A, 0, 0 };

static TAMGUCHAR latin7codetable[] = {
    163, 0x00A3, 164, 0x20AC, 165, 0x20AF, 166, 0x00A6, 167, 0x00A7, 168, 0x00A8, 169, 0x00A9, 170, 0x037A,
    171, 0x00AB, 172, 0x00AC, 175, 0x2015, 176, 0x00B0, 177, 0x00B1, 178, 0x00B2, 179, 0x00B3, 180, 0x0384, 181, 0x0385,
    182, 0x0386, 183, 0x00B7, 184, 0x0388, 185, 0x0389, 186, 0x038A, 187, 0x00BB, 188, 0x038C, 189, 0x00BD, 190, 0x038E, 191, 0x038F,
    192, 0x0390, 193, 0x0391, 194, 0x0392, 195, 0x0393, 196, 0x0394, 197, 0x0395, 198, 0x0396, 199, 0x0397, 200, 0x0398, 201, 0x0399,
    202, 0x039A, 203, 0x039B, 204, 0x039C, 205, 0x039D, 206, 0x039E, 207, 0x039F, 208, 0x03A0, 209, 0x03A1, 211, 0x03A3, 212, 0x03A4,
    213, 0x03A5, 214, 0x03A6, 215, 0x03A7, 216, 0x03A8, 217, 0x03A9, 218, 0x03AA, 219, 0x03AB, 220, 0x03AC, 221, 0x03AD, 222, 0x03AE,
    223, 0x03AF, 224, 0x03B0, 225, 0x03B1, 226, 0x03B2, 227, 0x03B3, 228, 0x03B4, 229, 0x03B5, 230, 0x03B6, 231, 0x03B7, 232, 0x03B8,
    233, 0x03B9, 234, 0x03BA, 235, 0x03BB, 236, 0x03BC, 237, 0x03BD, 238, 0x03BE, 239, 0x03BF, 240, 0x03C0, 241, 0x03C1, 242, 0x03C2,
    243, 0x03C3, 244, 0x03C4, 245, 0x03C5, 246, 0x03C6, 247, 0x03C7, 248, 0x03C8, 249, 0x03C9, 250, 0x03CA, 251, 0x03CB, 252, 0x03CC,
    253, 0x03CD, 254, 0x03CE, 0, 0 };

static TAMGUCHAR latin8codetable[] = {
    162, 0x00A2, 163, 0x00A3, 164, 0x00A4, 165, 0x00A5, 166, 0x00A6, 167, 0x00A7, 168, 0x00A8, 169, 0x00A9, 170, 0x00D7, 171, 0x00AB,
    172, 0x00AC, 174, 0x00AE, 175, 0x00AF, 176, 0x00B0, 177, 0x00B1, 178, 0x00B2, 179, 0x00B3, 180, 0x00B4, 181, 0x00B5,
    182, 0x00B6, 183, 0x00B7, 184, 0x00B8, 185, 0x00B9, 186, 0x00F7, 187, 0x00BB, 188, 0x00BC, 189, 0x00BD, 190, 0x00BE, 223, 0x2017,
    224, 0x05D0, 225, 0x05D1, 226, 0x05D2, 227, 0x05D3, 228, 0x05D4, 229, 0x05D5, 230, 0x05D6, 231, 0x05D7, 232, 0x05D8, 233, 0x05D9,
    234, 0x05DA, 235, 0x05DB, 236, 0x05DC, 237, 0x05DD, 238, 0x05DE, 239, 0x05DF, 240, 0x05E0, 241, 0x05E1, 242, 0x05E2, 243, 0x05E3,
    244, 0x05E4, 245, 0x05E5, 246, 0x05E6, 247, 0x05E7, 248, 0x05E8, 249, 0x05E9, 250, 0x05EA, 0, 0 };

static TAMGUCHAR latin9codetable[] = {
    161, 0x00A1, 162, 0x00A2, 163, 0x00A3, 164, 0x00A4, 165, 0x00A5, 166, 0x00A6, 167, 0x00A7, 168, 0x00A8, 169, 0x00A9, 170, 0x00AA,
    171, 0x00AB, 172, 0x00AC, 174, 0x00AE, 175, 0x00AF, 176, 0x00B0, 177, 0x00B1, 178, 0x00B2, 179, 0x00B3, 180, 0x00B4,
    181, 0x00B5, 182, 0x00B6, 183, 0x00B7, 184, 0x00B8, 185, 0x00B9, 186, 0x00BA, 187, 0x00BB, 188, 0x00BC, 189, 0x00BD, 190, 0x00BE,
    191, 0x00BF, 192, 0x00C0, 193, 0x00C1, 194, 0x00C2, 195, 0x00C3, 196, 0x00C4, 197, 0x00C5, 198, 0x00C6, 199, 0x00C7, 200, 0x00C8,
    201, 0x00C9, 202, 0x00CA, 203, 0x00CB, 204, 0x00CC, 205, 0x00CD, 206, 0x00CE, 207, 0x00CF, 208, 0x011E, 209, 0x00D1, 210, 0x00D2,
    211, 0x00D3, 212, 0x00D4, 213, 0x00D5, 214, 0x00D6, 215, 0x00D7, 216, 0x00D8, 217, 0x00D9, 218, 0x00DA, 219, 0x00DB, 220, 0x00DC,
    221, 0x0130, 222, 0x015E, 223, 0x00DF, 224, 0x00E0, 225, 0x00E1, 226, 0x00E2, 227, 0x00E3, 228, 0x00E4, 229, 0x00E5, 230, 0x00E6,
    231, 0x00E7, 232, 0x00E8, 233, 0x00E9, 234, 0x00EA, 235, 0x00EB, 236, 0x00EC, 237, 0x00ED, 238, 0x00EE, 239, 0x00EF, 240, 0x011F,
    241, 0x00F1, 242, 0x00F2, 243, 0x00F3, 244, 0x00F4, 245, 0x00F5, 246, 0x00F6, 247, 0x00F7, 248, 0x00F8, 249, 0x00F9, 250, 0x00FA,
    251, 0x00FB, 252, 0x00FC, 253, 0x0131, 254, 0x015F, 255, 0x00FF, 0, 0 };

static TAMGUCHAR latin10codetable[] = {
    161, 0x0104, 162, 0x0112, 163, 0x0122, 164, 0x012A, 165, 0x0128, 166, 0x0136, 167, 0x00A7, 168, 0x013B, 169, 0x0110, 170, 0x0160,
    171, 0x0166, 172, 0x017D, 174, 0x016A, 175, 0x014A, 176, 0x00B0, 177, 0x0105, 178, 0x0113, 179, 0x0123, 180, 0x012B,
    181, 0x0129, 182, 0x0137, 183, 0x00B7, 184, 0x013C, 185, 0x0111, 186, 0x0161, 187, 0x0167, 188, 0x017E, 189, 0x2015, 190, 0x016B,
    191, 0x014B, 192, 0x0100, 193, 0x00C1, 194, 0x00C2, 195, 0x00C3, 196, 0x00C4, 197, 0x00C5, 198, 0x00C6, 199, 0x012E, 200, 0x010C,
    201, 0x00C9, 202, 0x0118, 203, 0x00CB, 204, 0x0116, 205, 0x00CD, 206, 0x00CE, 207, 0x00CF, 208, 0x00D0, 209, 0x0145, 210, 0x014C,
    211, 0x00D3, 212, 0x00D4, 213, 0x00D5, 214, 0x00D6, 215, 0x0168, 216, 0x00D8, 217, 0x0172, 218, 0x00DA, 219, 0x00DB, 220, 0x00DC,
    221, 0x00DD, 222, 0x00DE, 223, 0x00DF, 224, 0x0101, 225, 0x00E1, 226, 0x00E2, 227, 0x00E3, 228, 0x00E4, 229, 0x00E5, 230, 0x00E6,
    231, 0x012F, 232, 0x010D, 233, 0x00E9, 234, 0x0119, 235, 0x00EB, 236, 0x0117, 237, 0x00ED, 238, 0x00EE, 239, 0x00EF, 240, 0x00F0,
    241, 0x0146, 242, 0x014D, 243, 0x00F3, 244, 0x00F4, 245, 0x00F5, 246, 0x00F6, 247, 0x0169, 248, 0x00F8, 249, 0x0173, 250, 0x00FA,
    251, 0x00FB, 252, 0x00FC, 253, 0x00FD, 254, 0x00FE, 255, 0x0138, 0, 0 };

static TAMGUCHAR latin11codetable[] = {
    161, 0x0E01, 162, 0x0E02, 163, 0x0E03, 164, 0x0E04, 165, 0x0E05, 166, 0x0E06, 167, 0x0E07, 168, 0x0E08, 169, 0x0E09, 170, 0x0E0A,
    171, 0x0E0B, 172, 0x0E0C, 173, 0x0E0D, 174, 0x0E0E, 175, 0x0E0F, 176, 0x0E10, 177, 0x0E11, 178, 0x0E12, 179, 0x0E13, 180, 0x0E14,
    181, 0x0E15, 182, 0x0E16, 183, 0x0E17, 184, 0x0E18, 185, 0x0E19, 186, 0x0E1A, 187, 0x0E1B, 188, 0x0E1C, 189, 0x0E1D, 190, 0x0E1E,
    191, 0x0E1F, 192, 0x0E20, 193, 0x0E21, 194, 0x0E22, 195, 0x0E23, 196, 0x0E24, 197, 0x0E25, 198, 0x0E26, 199, 0x0E27, 200, 0x0E28,
    201, 0x0E29, 202, 0x0E2A, 203, 0x0E2B, 204, 0x0E2C, 205, 0x0E2D, 206, 0x0E2E, 207, 0x0E2F, 208, 0x0E30, 210, 0x0E32,
    211, 0x0E33, 223, 0x0E3F, 224, 0x0E40,
    225, 0x0E41, 226, 0x0E42, 227, 0x0E43, 228, 0x0E44, 229, 0x0E45, 230, 0x0E46,
    239, 0x0E4F, 240, 0x0E50, 241, 0x0E51, 242, 0x0E52, 243, 0x0E53, 244, 0x0E54,
    245, 0x0E55, 246, 0x0E56, 247, 0x0E57, 248, 0x0E58, 249, 0x0E59, 250, 0x0E5A, 251, 0x0E5B, 0, 0 };

static TAMGUCHAR latin12codetable[] = { 0x43, 0x43, 0x4c, 0x4c, 0x41, 0x41, 0x55, 0x55, 0x44, 0x44, 0x49, 0x49,
    0x55, 0x55, 0x53, 0x53, 0x52, 0x52, 0x55, 0x55, 0x46, 0x46, 0x55, 0x55, 0x53, 0x53, 0x54, 0x54,
    0x41, 0x41, 0x4d, 0x4d, 0x47, 0x47, 0x55, 0x55, 0x46, 0x46, 0x45, 0x45,
    0x43, 0x43, 0x49, 0x49, 0x54, 0x54, 0x41, 0x41, 0x4E, 0x4E, 0x4E, 0x4E, 0x4F, 0x4F, 0x4D, 0x4D,
    0x4D, 0x4D, 0x58, 0x58, 0x56, 0x56, 0x49, 0x49, 0x49, 0x49, 0x49, 0x49, 0x2D, 0x2D, 0x58, 0x58, 0, 0 };

static TAMGUCHAR latin13codetable[] = {
    34, 0x201D, 162, 0x00A2, 163, 0x00A3, 164, 0x00A4, 165, 0x201E, 166, 0x00A6, 167, 0x00A7, 168, 0x00D8, 169, 0x00A9, 170, 0x0156,
    171, 0x00AB, 172, 0x00AC, 174, 0x00AE, 175, 0x00C6, 176, 0x00B0, 177, 0x00B1, 178, 0x00B2, 179, 0x00B3, 34, 0x201C,
    181, 0x00B5, 182, 0x00B6, 183, 0x00B7, 184, 0x00F8, 185, 0x00B9, 186, 0x0157, 187, 0x00BB, 188, 0x00BC, 189, 0x00BD, 190, 0x00BE,
    191, 0x00E6, 192, 0x0104, 193, 0x012E, 194, 0x0100, 195, 0x0106, 196, 0x00C4, 197, 0x00C5, 198, 0x0118, 199, 0x0112, 200, 0x010C,
    201, 0x00C9, 202, 0x0179, 203, 0x0116, 204, 0x0122, 205, 0x0136, 206, 0x012A, 207, 0x013B, 208, 0x0160, 209, 0x0143, 210, 0x0145,
    211, 0x00D3, 212, 0x014C, 213, 0x00D5, 214, 0x00D6, 215, 0x00D7, 216, 0x0172, 217, 0x0141, 218, 0x015A, 219, 0x016A, 220, 0x00DC,
    221, 0x017B, 222, 0x017D, 223, 0x00DF, 224, 0x0105, 225, 0x012F, 226, 0x0101, 227, 0x0107, 228, 0x00E4, 229, 0x00E5, 230, 0x0119,
    231, 0x0113, 232, 0x010D, 233, 0x00E9, 234, 0x017A, 235, 0x0117, 236, 0x0123, 237, 0x0137, 238, 0x012B, 239, 0x013C, 240, 0x0161,
    241, 0x0144, 242, 0x0146, 243, 0x00F3, 244, 0x014D, 245, 0x00F5, 246, 0x00F6, 247, 0x00F7, 248, 0x0173, 249, 0x0142, 250, 0x015B,
    251, 0x016B, 252, 0x00FC, 253, 0x017C, 254, 0x017E, 39, 0x2019, 0, 0 };

static TAMGUCHAR latin14codetable[] = {
    161, 0x1E02, 162, 0x1E03, 163, 0x00A3, 164, 0x010A, 165, 0x010B, 166, 0x1E0A, 167, 0x00A7, 168, 0x1E80, 169, 0x00A9, 170, 0x1E82,
    171, 0x1E0B, 172, 0x1EF2, 174, 0x00AE, 175, 0x0178, 176, 0x1E1E, 177, 0x1E1F, 178, 0x0120, 179, 0x0121, 180, 0x1E40,
    181, 0x1E41, 182, 0x00B6, 183, 0x1E56, 184, 0x1E81, 185, 0x1E57, 186, 0x1E83, 187, 0x1E60, 188, 0x1EF3, 189, 0x1E84, 190, 0x1E85,
    191, 0x1E61, 192, 0x00C0, 193, 0x00C1, 194, 0x00C2, 195, 0x00C3, 196, 0x00C4, 197, 0x00C5, 198, 0x00C6, 199, 0x00C7, 200, 0x00C8,
    201, 0x00C9, 202, 0x00CA, 203, 0x00CB, 204, 0x00CC, 205, 0x00CD, 206, 0x00CE, 207, 0x00CF, 208, 0x0174, 209, 0x00D1, 210, 0x00D2,
    211, 0x00D3, 212, 0x00D4, 213, 0x00D5, 214, 0x00D6, 215, 0x1E6A, 216, 0x00D8, 217, 0x00D9, 218, 0x00DA, 219, 0x00DB, 220, 0x00DC,
    221, 0x00DD, 222, 0x0176, 223, 0x00DF, 224, 0x00E0, 225, 0x00E1, 226, 0x00E2, 227, 0x00E3, 228, 0x00E4, 229, 0x00E5, 230, 0x00E6,
    231, 0x00E7, 232, 0x00E8, 233, 0x00E9, 234, 0x00EA, 235, 0x00EB, 236, 0x00EC, 237, 0x00ED, 238, 0x00EE, 239, 0x00EF, 240, 0x0175,
    241, 0x00F1, 242, 0x00F2, 243, 0x00F3, 244, 0x00F4, 245, 0x00F5, 246, 0x00F6, 247, 0x1E6B, 248, 0x00F8, 249, 0x00F9, 250, 0x00FA,
    251, 0x00FB, 252, 0x00FC, 253, 0x00FD, 254, 0x0177, 255, 0x00FF, 0, 0 };

static TAMGUCHAR latin15codetable[] = {
    161, 0x00A1, 162, 0x00A2, 163, 0x00A3, 164, 0x20AC, 165, 0x00A5, 166, 0x0160, 167, 0x00A7, 168, 0x0161, 169, 0x00A9, 170, 0x00AA,
    171, 0x00AB, 172, 0x00AC, 174, 0x00AE, 175, 0x00AF, 176, 0x00B0, 177, 0x00B1, 178, 0x00B2, 179, 0x00B3, 180, 0x017D,
    181, 0x00B5, 182, 0x00B6, 183, 0x00B7, 184, 0x017E, 185, 0x00B9, 186, 0x00BA, 187, 0x00BB, 188, 0x0152, 189, 0x0153, 190, 0x0178,
    191, 0x00BF, 192, 0x00C0, 193, 0x00C1, 194, 0x00C2, 195, 0x00C3, 196, 0x00C4, 197, 0x00C5, 198, 0x00C6, 199, 0x00C7, 200, 0x00C8,
    201, 0x00C9, 202, 0x00CA, 203, 0x00CB, 204, 0x00CC, 205, 0x00CD, 206, 0x00CE, 207, 0x00CF, 208, 0x00D0, 209, 0x00D1, 210, 0x00D2,
    211, 0x00D3, 212, 0x00D4, 213, 0x00D5, 214, 0x00D6, 215, 0x00D7, 216, 0x00D8, 217, 0x00D9, 218, 0x00DA, 219, 0x00DB, 220, 0x00DC,
    221, 0x00DD, 222, 0x00DE, 223, 0x00DF, 224, 0x00E0, 225, 0x00E1, 226, 0x00E2, 227, 0x00E3, 228, 0x00E4, 229, 0x00E5, 230, 0x00E6,
    231, 0x00E7, 232, 0x00E8, 233, 0x00E9, 234, 0x00EA, 235, 0x00EB, 236, 0x00EC, 237, 0x00ED, 238, 0x00EE, 239, 0x00EF, 240, 0x00F0,
    241, 0x00F1, 242, 0x00F2, 243, 0x00F3, 244, 0x00F4, 245, 0x00F5, 246, 0x00F6, 247, 0x00F7, 248, 0x00F8, 249, 0x00F9, 250, 0x00FA,
    251, 0x00FB, 252, 0x00FC, 253, 0x00FD, 254, 0x00FE, 255, 0x00FF, 0, 0 };

static TAMGUCHAR latin16codetable[] = {
    161, 0x0104, 162, 0x0105, 163, 0x0141, 164, 0x20AC, 165, 0x201E, 166, 0x0160, 167, 0x00A7, 168, 0x0161, 169, 0x00A9, 170, 0x0218,
    171, 0x00AB, 172, 0x0179, 174, 0x017A, 175, 0x017B, 176, 0x00B0, 177, 0x00B1, 178, 0x010C, 179, 0x0142, 180, 0x017D,
    34, 0x201D, 182, 0x00B6, 183, 0x00B7, 184, 0x017E, 185, 0x010D, 186, 0x0219, 187, 0x00BB, 188, 0x0152, 189, 0x0153, 190, 0x0178,
    191, 0x017C, 192, 0x00C0, 193, 0x00C1, 194, 0x00C2, 195, 0x0102, 196, 0x00C4, 197, 0x0106, 198, 0x00C6, 199, 0x00C7, 200, 0x00C8,
    201, 0x00C9, 202, 0x00CA, 203, 0x00CB, 204, 0x00CC, 205, 0x00CD, 206, 0x00CE, 207, 0x00CF, 208, 0x0110, 209, 0x0143, 210, 0x00D2,
    211, 0x00D3, 212, 0x00D4, 213, 0x0150, 214, 0x00D6, 215, 0x015A, 216, 0x0170, 217, 0x00D9, 218, 0x00DA, 219, 0x00DB, 220, 0x00DC,
    221, 0x0118, 222, 0x021A, 223, 0x00DF, 224, 0x00E0, 225, 0x00E1, 226, 0x00E2, 227, 0x0103, 228, 0x00E4, 229, 0x0107, 230, 0x00E6,
    231, 0x00E7, 232, 0x00E8, 233, 0x00E9, 234, 0x00EA, 235, 0x00EB, 236, 0x00EC, 237, 0x00ED, 238, 0x00EE, 239, 0x00EF, 240, 0x0111,
    241, 0x0144, 242, 0x00F2, 243, 0x00F3, 244, 0x00F4, 245, 0x0151, 246, 0x00F6, 247, 0x015B, 248, 0x0171, 249, 0x00F9, 250, 0x00FA,
    251, 0x00FB, 252, 0x00FC, 253, 0x0119, 254, 0x021B, 255, 0x00FF, 0, 0 };

//it will be accessed as the 17 table...
static TAMGUCHAR latinwindows1[] = {
    127,0x44,128,0x20ac,129,0x20,130,0x201a,131,0x192,132,0x201e,133,0x2026,134,0x2020,135,0x2021,136,0x2c6,137,0x2030,138,0x160,139,0x2039,
    140,0x152,141,0x20,142,0x17d,143,0x20,144,0x20,145,0x2018,146,0x2019,147,0x201c,148,0x201d,149,0x2022,150,0x2013,151,0x2014,152,0x2dc,
    153,0x2122,154,0x161,155,0x203a,156,0x153,157,0x20,158,0x17e,159,0x178,160,0x20,161,0xa1,162,0xa2,163,0xa3,164,0xa4,165,0xa5,166,0xa6,
    167,0xa7,168,0xa8,169,0xa9,170,0xaa,171,0xab,172,0xac,173,0x20,174,0xae,175,0xaf,176,0xb0,177,0xb1,178,0xb2,179,0xb3,180,0xb4,181,0xb5,
    182,0xb6,183,0xb7,184,0xb8,185,0xb9,186,0xba,187,0xbb,188,0xbc,189,0xbd,190,0xbe,191,0xbf,192,0xc0,193,0xc1,194,0xc2,195,0xc3,196,0xc4,
    197,0xc5,198,0xc6,199,0xc7,200,0xc8,201,0xc9,202,0xca,203,0xcb,204,0xcc,205,0xcd,206,0xce,207,0xcf,208,0xd0,209,0xd1,210,0xd2,211,0xd3,
    212,0xd4,213,0xd5,214,0xd6,215,0xd7,216,0xd8,217,0xd9,218,0xda,219,0xdb,220,0xdc,221,0xdd,222,0xde,223,0xdf,224,0xe0,225,0xe1,226,0xe2,
    227,0xe3,228,0xe4,229,0xe5,230,0xe6,231,0xe7,232,0xe8,233,0xe9,234,0xea,235,0xeb,236,0xec,237,0xed,238,0xee,239,0xef,240,0xf0,241,0xf1,
    242,0xf2,243,0xf3,244,0xf4,245,0xf5,246,0xf6,247,0xf7,248,0xf8,249,0xf9,250,0xfa,251,0xfb,252,0xfc,253,0xfd,254,0xfe,255,0xff,0,0};

static BLONG codingtable[] = { 65,97,2,66,98,2,67,99,2,68,100,2,69,101,2,70,102,2,71,103,2,72,104,2,73,105,2,74,106,2,75,107,2,76,
    108,2,77,109,2,78,110,2,79,111,2,80,112,2,81,113,2,82,114,2,83,115,2,84,116,2,85,117,2,86,118,2,87,119,
    2,88,120,2,89,121,2,90,122,2,97,65,1,98,66,1,99,67,1,100,68,1,101,69,1,102,70,1,103,71,1,104,72,1,
    105,73,1,106,74,1,107,75,1,108,76,1,109,77,1,110,78,1,111,79,1,112,80,1,113,81,1,114,82,1,115,83,1,116,
    84,1,117,85,1,118,86,1,119,87,1,120,88,1,121,89,1,122,90,1,170,170,1,181,924,1,186,186,1,192,224,2,193,225,
    2,194,226,2,195,227,2,196,228,2,197,229,2,198,230,2,199,231,2,200,232,2,201,233,2,202,234,2,203,235,2,204,236,2,
    205,237,2,206,238,2,207,239,2,208,240,2,209,241,2,210,242,2,211,243,2,212,244,2,213,245,2,214,246,2,216,248,2,217,
    249,2,218,250,2,219,251,2,220,252,2,221,253,2,222,254,2,223,223,1,224,192,1,225,193,1,226,194,1,227,195,1,228,196,
    1,229,197,1,230,198,1,231,199,1,232,200,1,233,201,1,234,202,1,235,203,1,236,204,1,237,205,1,238,206,1,239,207,1,
    240,208,1,241,209,1,242,210,1,243,211,1,244,212,1,245,213,1,246,214,1,248,216,1,249,217,1,250,218,1,251,219,1,252,
    220,1,253,221,1,254,222,1,255,376,1,256,257,2,257,256,1,258,259,2,259,258,1,260,261,2,261,260,1,262,263,2,263,262,
    1,264,265,2,265,264,1,266,267,2,267,266,1,268,269,2,269,268,1,270,271,2,271,270,1,272,273,2,273,272,1,274,275,2,
    275,274,1,276,277,2,277,276,1,278,279,2,279,278,1,280,281,2,281,280,1,282,283,2,283,282,1,284,285,2,285,284,1,286,
    287,2,287,286,1,288,289,2,289,288,1,290,291,2,291,290,1,292,293,2,293,292,1,294,295,2,295,294,1,296,297,2,297,296,
    1,298,299,2,299,298,1,300,301,2,301,300,1,302,303,2,303,302,1,304,105,2,305,73,1,306,307,2,307,306,1,308,309,2,
    309,308,1,310,311,2,311,310,1,312,312,1,313,314,2,314,313,1,315,316,2,316,315,1,317,318,2,318,317,1,319,320,2,320,
    319,1,321,322,2,322,321,1,323,324,2,324,323,1,325,326,2,326,325,1,327,328,2,328,327,1,329,329,1,330,331,2,331,330,
    1,332,333,2,333,332,1,334,335,2,335,334,1,336,337,2,337,336,1,338,339,2,339,338,1,340,341,2,341,340,1,342,343,2,
    343,342,1,344,345,2,345,344,1,346,347,2,347,346,1,348,349,2,349,348,1,350,351,2,351,350,1,352,353,2,353,352,1,354,
    355,2,355,354,1,356,357,2,357,356,1,358,359,2,359,358,1,360,361,2,361,360,1,362,363,2,363,362,1,364,365,2,365,364,
    1,366,367,2,367,366,1,368,369,2,369,368,1,370,371,2,371,370,1,372,373,2,373,372,1,374,375,2,375,374,1,376,255,2,
    377,378,2,378,377,1,379,380,2,380,379,1,381,382,2,382,381,1,383,83,1,384,579,1,385,595,2,386,387,2,387,386,1,388,
    389,2,389,388,1,390,596,2,391,392,2,392,391,1,393,598,2,394,599,2,395,396,2,396,395,1,397,397,1,398,477,2,399,601,
    2,400,603,2,401,402,2,402,401,1,403,608,2,404,611,2,405,502,1,406,617,2,407,616,2,408,409,2,409,408,1,410,573,1,
    411,411,1,412,623,2,413,626,2,414,544,1,415,629,2,416,417,2,417,416,1,418,419,2,419,418,1,420,421,2,421,420,1,422,
    640,2,423,424,2,424,423,1,425,643,2,426,426,1,427,427,1,428,429,2,429,428,1,430,648,2,431,432,2,432,431,1,433,650,
    2,434,651,2,435,436,2,436,435,1,437,438,2,438,437,1,439,658,2,440,441,2,441,440,1,442,442,1,443,443,0,444,445,2,
    445,444,1,446,446,1,447,503,1,448,448,0,449,449,0,450,450,0,451,451,0,452,454,2,453,453,0,454,452,1,455,457,2,456,
    456,0,457,455,1,458,460,2,459,459,0,460,458,1,461,462,2,462,461,1,463,464,2,464,463,1,465,466,2,466,465,1,467,468,
    2,468,467,1,469,470,2,470,469,1,471,472,2,472,471,1,473,474,2,474,473,1,475,476,2,476,475,1,477,398,1,478,479,2,
    479,478,1,480,481,2,481,480,1,482,483,2,483,482,1,484,485,2,485,484,1,486,487,2,487,486,1,488,489,2,489,488,1,490,
    491,2,491,490,1,492,493,2,493,492,1,494,495,2,495,494,1,496,496,1,497,499,2,498,498,0,499,497,1,500,501,2,501,500,
    1,502,405,2,503,447,2,504,505,2,505,504,1,506,507,2,507,506,1,508,509,2,509,508,1,510,511,2,511,510,1,512,513,2,
    513,512,1,514,515,2,515,514,1,516,517,2,517,516,1,518,519,2,519,518,1,520,521,2,521,520,1,522,523,2,523,522,1,524,
    525,2,525,524,1,526,527,2,527,526,1,528,529,2,529,528,1,530,531,2,531,530,1,532,533,2,533,532,1,534,535,2,535,534,
    1,536,537,2,537,536,1,538,539,2,539,538,1,540,541,2,541,540,1,542,543,2,543,542,1,544,414,2,545,545,1,546,547,2,
    547,546,1,548,549,2,549,548,1,550,551,2,551,550,1,552,553,2,553,552,1,554,555,2,555,554,1,556,557,2,557,556,1,558,
    559,2,559,558,1,560,561,2,561,560,1,562,563,2,563,562,1,564,564,1,565,565,1,566,566,1,567,567,1,568,568,1,569,569,
    1,570,11365,2,571,572,2,572,571,1,573,410,2,574,11366,2,575,11390,1,576,11391,1,577,578,2,578,577,1,579,384,2,580,649,2,
    581,652,2,582,583,2,583,582,1,584,585,2,585,584,1,586,587,2,587,586,1,588,589,2,589,588,1,590,591,2,591,590,1,592,
    11375,1,593,11373,1,594,11376,1,595,385,1,596,390,1,597,597,1,598,393,1,599,394,1,600,600,1,601,399,1,602,602,1,603,400,
    1,604,604,1,605,605,1,606,606,1,607,607,1,608,403,1,609,609,1,610,610,1,611,404,1,612,612,1,613,613,1,614,614,1,
    615,615,1,616,407,1,617,406,1,618,618,1,619,11362,1,620,620,1,621,621,1,622,622,1,623,412,1,624,624,1,625,11374,1,626,
    413,1,627,627,1,628,628,1,629,415,1,630,630,1,631,631,1,632,632,1,633,633,1,634,634,1,635,635,1,636,636,1,637,11364,
    1,638,638,1,639,639,1,640,422,1,641,641,1,642,642,1,643,425,1,644,644,1,645,645,1,646,646,1,647,647,1,648,430,1,
    649,580,1,650,433,1,651,434,1,652,581,1,653,653,1,654,654,1,655,655,1,656,656,1,657,657,1,658,439,1,659,659,1,660,
    660,0,661,661,1,662,662,1,663,663,1,664,664,1,665,665,1,666,666,1,667,667,1,668,668,1,669,669,1,670,670,1,671,671,
    1,672,672,1,673,673,1,674,674,1,675,675,1,676,676,1,677,677,1,678,678,1,679,679,1,680,680,1,681,681,1,682,682,1,
    683,683,1,684,684,1,685,685,1,686,686,1,687,687,1,688,688,0,689,689,0,690,690,0,691,691,0,692,692,0,693,693,0,694,
    694,0,695,695,0,696,696,0,697,697,0,698,698,0,699,699,0,700,700,0,701,701,0,702,702,0,703,703,0,704,704,0,705,705,
    0,710,710,0,711,711,0,712,712,0,713,713,0,714,714,0,715,715,0,716,716,0,717,717,0,718,718,0,719,719,0,720,720,0,
    721,721,0,736,736,0,737,737,0,738,738,0,739,739,0,740,740,0,748,748,0,750,750,0,880,881,2,881,880,1,882,883,2,883,
    882,1,884,884,0,886,887,2,887,886,1,890,890,0,891,1021,1,892,1022,1,893,1023,1,902,940,2,904,941,2,905,942,2,906,943,
    2,908,972,2,910,973,2,911,974,2,912,912,1,913,945,2,914,946,2,915,947,2,916,948,2,917,949,2,918,950,2,919,951,2,
    920,952,2,921,953,2,922,954,2,923,955,2,924,956,2,925,957,2,926,958,2,927,959,2,928,960,2,929,961,2,931,963,2,932,
    964,2,933,965,2,934,966,2,935,967,2,936,968,2,937,969,2,938,970,2,939,971,2,940,902,1,941,904,1,942,905,1,943,906,
    1,944,944,1,945,913,1,946,914,1,947,915,1,948,916,1,949,917,1,950,918,1,951,919,1,952,920,1,953,921,1,954,922,1,
    955,923,1,956,924,1,957,925,1,958,926,1,959,927,1,960,928,1,961,929,1,962,931,1,963,931,1,964,932,1,965,933,1,966,
    934,1,967,935,1,968,936,1,969,937,1,970,938,1,971,939,1,972,908,1,973,910,1,974,911,1,975,983,2,976,914,1,977,920,
    1,978,978,2,979,979,2,980,980,2,981,934,1,982,928,1,983,975,1,984,985,2,985,984,1,986,987,2,987,986,1,988,989,2,
    989,988,1,990,991,2,991,990,1,992,993,2,993,992,1,994,995,2,995,994,1,996,997,2,997,996,1,998,999,2,999,998,1,1000,
    1001,2,1001,1000,1,1002,1003,2,1003,1002,1,1004,1005,2,1005,1004,1,1006,1007,2,1007,1006,1,1008,922,1,1009,929,1,1010,1017,1,1011,1011,
    1,1012,952,2,1013,917,1,1015,1016,2,1016,1015,1,1017,1010,2,1018,1019,2,1019,1018,1,1020,1020,1,1021,891,2,1022,892,2,1023,893,2,
    1024,1104,2,1025,1105,2,1026,1106,2,1027,1107,2,1028,1108,2,1029,1109,2,1030,1110,2,1031,1111,2,1032,1112,2,1033,1113,2,1034,1114,2,1035,
    1115,2,1036,1116,2,1037,1117,2,1038,1118,2,1039,1119,2,1040,1072,2,1041,1073,2,1042,1074,2,1043,1075,2,1044,1076,2,1045,1077,2,1046,1078,
    2,1047,1079,2,1048,1080,2,1049,1081,2,1050,1082,2,1051,1083,2,1052,1084,2,1053,1085,2,1054,1086,2,1055,1087,2,1056,1088,2,1057,1089,2,
    1058,1090,2,1059,1091,2,1060,1092,2,1061,1093,2,1062,1094,2,1063,1095,2,1064,1096,2,1065,1097,2,1066,1098,2,1067,1099,2,1068,1100,2,1069,
    1101,2,1070,1102,2,1071,1103,2,1072,1040,1,1073,1041,1,1074,1042,1,1075,1043,1,1076,1044,1,1077,1045,1,1078,1046,1,1079,1047,1,1080,1048,
    1,1081,1049,1,1082,1050,1,1083,1051,1,1084,1052,1,1085,1053,1,1086,1054,1,1087,1055,1,1088,1056,1,1089,1057,1,1090,1058,1,1091,1059,1,
    1092,1060,1,1093,1061,1,1094,1062,1,1095,1063,1,1096,1064,1,1097,1065,1,1098,1066,1,1099,1067,1,1100,1068,1,1101,1069,1,1102,1070,1,1103,
    1071,1,1104,1024,1,1105,1025,1,1106,1026,1,1107,1027,1,1108,1028,1,1109,1029,1,1110,1030,1,1111,1031,1,1112,1032,1,1113,1033,1,1114,1034,
    1,1115,1035,1,1116,1036,1,1117,1037,1,1118,1038,1,1119,1039,1,1120,1121,2,1121,1120,1,1122,1123,2,1123,1122,1,1124,1125,2,1125,1124,1,
    1126,1127,2,1127,1126,1,1128,1129,2,1129,1128,1,1130,1131,2,1131,1130,1,1132,1133,2,1133,1132,1,1134,1135,2,1135,1134,1,1136,1137,2,1137,
    1136,1,1138,1139,2,1139,1138,1,1140,1141,2,1141,1140,1,1142,1143,2,1143,1142,1,1144,1145,2,1145,1144,1,1146,1147,2,1147,1146,1,1148,1149,
    2,1149,1148,1,1150,1151,2,1151,1150,1,1152,1153,2,1153,1152,1,1162,1163,2,1163,1162,1,1164,1165,2,1165,1164,1,1166,1167,2,1167,1166,1,
    1168,1169,2,1169,1168,1,1170,1171,2,1171,1170,1,1172,1173,2,1173,1172,1,1174,1175,2,1175,1174,1,1176,1177,2,1177,1176,1,1178,1179,2,1179,
    1178,1,1180,1181,2,1181,1180,1,1182,1183,2,1183,1182,1,1184,1185,2,1185,1184,1,1186,1187,2,1187,1186,1,1188,1189,2,1189,1188,1,1190,1191,
    2,1191,1190,1,1192,1193,2,1193,1192,1,1194,1195,2,1195,1194,1,1196,1197,2,1197,1196,1,1198,1199,2,1199,1198,1,1200,1201,2,1201,1200,1,
    1202,1203,2,1203,1202,1,1204,1205,2,1205,1204,1,1206,1207,2,1207,1206,1,1208,1209,2,1209,1208,1,1210,1211,2,1211,1210,1,1212,1213,2,1213,
    1212,1,1214,1215,2,1215,1214,1,1216,1231,2,1217,1218,2,1218,1217,1,1219,1220,2,1220,1219,1,1221,1222,2,1222,1221,1,1223,1224,2,1224,1223,
    1,1225,1226,2,1226,1225,1,1227,1228,2,1228,1227,1,1229,1230,2,1230,1229,1,1231,1216,1,1232,1233,2,1233,1232,1,1234,1235,2,1235,1234,1,
    1236,1237,2,1237,1236,1,1238,1239,2,1239,1238,1,1240,1241,2,1241,1240,1,1242,1243,2,1243,1242,1,1244,1245,2,1245,1244,1,1246,1247,2,1247,
    1246,1,1248,1249,2,1249,1248,1,1250,1251,2,1251,1250,1,1252,1253,2,1253,1252,1,1254,1255,2,1255,1254,1,1256,1257,2,1257,1256,1,1258,1259,
    2,1259,1258,1,1260,1261,2,1261,1260,1,1262,1263,2,1263,1262,1,1264,1265,2,1265,1264,1,1266,1267,2,1267,1266,1,1268,1269,2,1269,1268,1,
    1270,1271,2,1271,1270,1,1272,1273,2,1273,1272,1,1274,1275,2,1275,1274,1,1276,1277,2,1277,1276,1,1278,1279,2,1279,1278,1,1280,1281,2,1281,
    1280,1,1282,1283,2,1283,1282,1,1284,1285,2,1285,1284,1,1286,1287,2,1287,1286,1,1288,1289,2,1289,1288,1,1290,1291,2,1291,1290,1,1292,1293,
    2,1293,1292,1,1294,1295,2,1295,1294,1,1296,1297,2,1297,1296,1,1298,1299,2,1299,1298,1,1300,1301,2,1301,1300,1,1302,1303,2,1303,1302,1,
    1304,1305,2,1305,1304,1,1306,1307,2,1307,1306,1,1308,1309,2,1309,1308,1,1310,1311,2,1311,1310,1,1312,1313,2,1313,1312,1,1314,1315,2,1315,
    1314,1,1316,1317,2,1317,1316,1,1329,1377,2,1330,1378,2,1331,1379,2,1332,1380,2,1333,1381,2,1334,1382,2,1335,1383,2,1336,1384,2,1337,1385,
    2,1338,1386,2,1339,1387,2,1340,1388,2,1341,1389,2,1342,1390,2,1343,1391,2,1344,1392,2,1345,1393,2,1346,1394,2,1347,1395,2,1348,1396,2,
    1349,1397,2,1350,1398,2,1351,1399,2,1352,1400,2,1353,1401,2,1354,1402,2,1355,1403,2,1356,1404,2,1357,1405,2,1358,1406,2,1359,1407,2,1360,
    1408,2,1361,1409,2,1362,1410,2,1363,1411,2,1364,1412,2,1365,1413,2,1366,1414,2,1369,1369,0,1377,1329,1,1378,1330,1,1379,1331,1,1380,1332,
    1,1381,1333,1,1382,1334,1,1383,1335,1,1384,1336,1,1385,1337,1,1386,1338,1,1387,1339,1,1388,1340,1,1389,1341,1,1390,1342,1,1391,1343,1,
    1392,1344,1,1393,1345,1,1394,1346,1,1395,1347,1,1396,1348,1,1397,1349,1,1398,1350,1,1399,1351,1,1400,1352,1,1401,1353,1,1402,1354,1,1403,
    1355,1,1404,1356,1,1405,1357,1,1406,1358,1,1407,1359,1,1408,1360,1,1409,1361,1,1410,1362,1,1411,1363,1,1412,1364,1,1413,1365,1,1414,1366,
    1,1415,1415,1,1488,1488,1,1489,1489,1,1490,1490,1,1491,1491,1,1492,1492,1,1493,1493,1,1494,1494,1,1495,1495,1,1496,1496,1,1497,1497,1,
    1498,1498,1,1499,1499,1,1500,1500,1,1501,1501,1,1502,1502,1,1503,1503,1,1504,1504,1,1505,1505,1,1506,1506,1,1507,1507,1,1508,1508,1,1509,
    1509,1,1510,1510,1,1511,1511,1,1512,1512,1,1513,1513,1,1514,1514,1,1570,1570,1,1571,1571,1,1576,1576,1,1577,1577,1,1578,1578,1,1579,1579,
    1,1580,1580,1,1581,1581,1,1582,1582,1,1583,1583,1,1584,1584,1,1585,1585,1,1586,1586,1,1587,1587,1,1588,1588,1,1589,1589,1,1590,1590,1,
    1591,1591,1,1592,1592,1,1593,1593,1,1594,1594,1,1601,1601,1,1602,1602,1,1603,1603,1,1604,1604,1,1605,1605,1,1606,1606,1,1607,1607,1,1608,
    1608,1,1609,1609,1,1610,1610,1,65153,65153,1,65154,65154,1,65155,65155,1,65156,65156,1,65167,65167,1,65168,65168,1,65169,65169,1,65170,65170,1,65171,65171,
    1,65172,65172,1,65173,65173,1,65174,65174,1,65175,65175,1,65176,65176,1,65177,65177,1,65178,65178,1,65179,65179,1,65180,65180,1,65181,65181,1,65182,65182,1,
    65183,65183,1,65184,65184,1,65185,65185,1,65186,65186,1,65187,65187,1,65188,65188,1,65189,65189,1,65190,65190,1,65191,65191,1,65192,65192,1,65193,65193,1,65194,
    65194,1,65195,65195,1,65196,65196,1,65197,65197,1,65198,65198,1,65199,65199,1,65200,65200,1,65201,65201,1,65202,65202,1,65203,65203,1,65204,65204,1,65205,65205,
    1,65206,65206,1,65207,65207,1,65208,65208,1,65209,65209,1,65210,65210,1,65211,65211,1,65212,65212,1,65213,65213,1,65214,65214,1,65215,65215,1,65216,65216,1,
    65217,65217,1,65218,65218,1,65219,65219,1,65220,65220,1,65221,65221,1,65222,65222,1,65223,65223,1,65224,65224,1,65225,65225,1,65226,65226,1,65227,65227,1,65228,
    65228,1,65229,65229,1,65230,65230,1,65231,65231,1,65232,65232,1,65233,65233,1,65234,65234,1,65235,65235,1,65236,65236,1,65237,65237,1,65238,65238,1,65239,65239,
    1,65240,65240,1,65241,65241,1,65242,65242,1,65243,65243,1,65244,65244,1,65245,65245,1,65246,65246,1,65247,65247,1,65248,65248,1,65249,65249,1,65250,65250,1,
    65251,65251,1,65252,65252,1,65253,65253,1,65254,65254,1,65255,65255,1,65256,65256,1,65257,65257,1,65258,65258,1,65259,65259,1,65260,65260,1,65261,65261,1,65262,
    65262,1,65263,65263,1,65264,65264,1,65265,65265,1,65266,65266,1,65267,65267,1,65268,65268,1,-1};


	static TAMGUCHAR ponctuations[] = { 0x21, 0x22, 0x23, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E,
    0x3F, 0x40, 0x5B, 0x5C, 0x5D, 0x5E, 0x60, 0x7B, 0x7C, 0x7D, 0x7E, 0x9C, 0xA0, 0xA1, 0xA2, 0xA4, 0xA5, 0xA6, 0xAA, 0xAB, 0xAC, 0xAD,
    0xAF, 0xB0, 0xB1, 0xB5, 0xB6, 0xB7, 0xB8, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xD7, 0xF7, 0x2BC, 0x2013, 0x2014, 0x2015, 0x2018,
    0x2019, 0x201C, 0x201D, 0x2020, 0x2021, 0x2022, 0x2026, 0x2032, 0x2033, 0x203B, 0x212E, 0x2190, 0x2191, 0x2192, 0x2193, 0x2264,
    1470, 1472, 1475, 1478, 1523, 1524, 0x2265, 0x263A, 0x3008, 0x3009, 1548, 1549, 1550, 1551, 1567, 1645, 1757, 1758, 1769, 0xFD3E, 0xFD3F, 0x3001, 0xFF0C, 0x2025, 0x2026, 0x3002,
    0x303D, 0x300C,0x300D, 0x300E,0x300F, 0x301D, 0x301F, 0x301C, 0xff1a, 0xff01, 0xff1f, 0x266a, 0 };


static hmap<string, TAMGUCHAR> htmlcodes;
static hmap<wstring, TAMGUCHAR> whtmlcodes;
static hmap<string, TAMGUCHAR> xmlcodes;
static hmap<string, string> metacodes;
static hmap<wstring, wstring> wmetacodes;
static hmap<string, TAMGUCHAR> utf8_to_dos;

static basebinn_hash<string> dos_to_utf8;
static basebinn_hash<string> codeshtml;
static basebinn_hash<string> codesxml;
static basebinn_hash<string> vowels;
static basebinn_hash<string> consonants;
static basebinn_hash<string> latinmin;
static basebinn_hash<string> latinmaj;

static basebinn_hash<string> transliteration;
static basebinn_hash<wstring> hangulutf8;
static basebinn_hash<wstring> hanguljamo;
static basebin_hash<TAMGUCHAR> hangulconvert;
static hmap<wstring, wstring> hangultojamo;

static basebin_hash<TAMGUCHAR> dos2unicode;
static basebin_hash<TAMGUCHAR> unicode2dos;


static hash_short<bool> punctuations(32, 11);
static hash_short<TAMGUCHAR> utf8codemin(32, 11);

static basebin_hash<TAMGUCHAR> utf8codemaj;

static basebin_hash<TAMGUCHAR> wvowels;
static basebin_hash<TAMGUCHAR> wconsonants;

static basebin_hash<TAMGUCHAR> latincodes;
static basebin_hash<TAMGUCHAR> latin2codes;
static basebin_hash<TAMGUCHAR> latin3codes;
static basebin_hash<TAMGUCHAR> latin4codes;
static basebin_hash<TAMGUCHAR> latin5codes;
static basebin_hash<TAMGUCHAR> latin6codes;
static basebin_hash<TAMGUCHAR> latin7codes;
static basebin_hash<TAMGUCHAR> latin8codes;
static basebin_hash<TAMGUCHAR> latin9codes;
static basebin_hash<TAMGUCHAR> latin10codes;
static basebin_hash<TAMGUCHAR> latin11codes;
static basebin_hash<TAMGUCHAR> latin12codes;
static basebin_hash<TAMGUCHAR> latin13codes;
static basebin_hash<TAMGUCHAR> latin14codes;
static basebin_hash<TAMGUCHAR> latin15codes;
static basebin_hash<TAMGUCHAR> latin16codes;
static basebin_hash<TAMGUCHAR> latinwindowscodes;

static basebin_hash<TAMGUCHAR> codeslatin;

static basebin_hash<basebin_hash<TAMGUCHAR>* > alllatincodes;

static hmap<string, TAMGUCHAR> utf8codes;
//------------------------------------------------------------------------------------------------------------
static char invertnumbers[1000][5];
static bin_hash<string> numbers(false);

static void InitNumbers() {
    char buff[10];
    int sz, pos;
    for (int i=0;i<=999;i++) {
        sz = sprintf_s(buff,10,"%03d",i);
        invertnumbers[i][sz--]=0;
        pos=0;
        while (buff[pos]) invertnumbers[i][sz--]=buff[pos++];
        invertnumbers[i][4]=pos;
    }
    
    wstring w;
    for (int i=0;i<=9999;i++) {
        sz = sprintf_s(buff,10,"%d",i);
        numbers[i]=buff;
    }
}

//------------------------------------------------------------------------------------------------------------
static void InitDosCodes() {
    static bool init=false;
    if (init)
        return;
    init=true;
    
    dos_to_utf8[128] = "Ç";
    utf8_to_dos["Ç"] = 128;
    dos2unicode[128] = 199;
    unicode2dos[199] = 128;
    dos_to_utf8[129] = "ü";
    utf8_to_dos["ü"] = 129;
    dos2unicode[129] = 252;
    unicode2dos[252] = 129;
    dos_to_utf8[130] = "é";
    utf8_to_dos["é"] = 130;
    dos2unicode[130] = 233;
    unicode2dos[233] = 130;
    dos_to_utf8[131] = "â";
    utf8_to_dos["â"] = 131;
    dos2unicode[131] = 226;
    unicode2dos[226] = 131;
    dos_to_utf8[132] = "ä";
    utf8_to_dos["ä"] = 132;
    dos2unicode[132] = 228;
    unicode2dos[228] = 132;
    dos_to_utf8[133] = "à";
    utf8_to_dos["à"] = 133;
    dos2unicode[133] = 224;
    unicode2dos[224] = 133;
    dos_to_utf8[134] = "å";
    utf8_to_dos["å"] = 134;
    dos2unicode[134] = 229;
    unicode2dos[229] = 134;
    dos_to_utf8[135] = "ç";
    utf8_to_dos["ç"] = 135;
    dos2unicode[135] = 231;
    unicode2dos[231] = 135;
    dos_to_utf8[136] = "ê";
    utf8_to_dos["ê"] = 136;
    dos2unicode[136] = 234;
    unicode2dos[234] = 136;
    dos_to_utf8[137] = "ë";
    utf8_to_dos["ë"] = 137;
    dos2unicode[137] = 235;
    unicode2dos[235] = 137;
    dos_to_utf8[138] = "è";
    utf8_to_dos["è"] = 138;
    dos2unicode[138] = 232;
    unicode2dos[232] = 138;
    dos_to_utf8[139] = "ï";
    utf8_to_dos["ï"] = 139;
    dos2unicode[139] = 239;
    unicode2dos[239] = 139;
    dos_to_utf8[140] = "î";
    utf8_to_dos["î"] = 140;
    dos2unicode[140] = 238;
    unicode2dos[238] = 140;
    dos_to_utf8[141] = "ì";
    utf8_to_dos["ì"] = 141;
    dos2unicode[141] = 236;
    unicode2dos[236] = 141;
    dos_to_utf8[142] = "Ä";
    utf8_to_dos["Ä"] = 142;
    dos2unicode[142] = 196;
    unicode2dos[196] = 142;
    dos_to_utf8[143] = "Å";
    utf8_to_dos["Å"] = 143;
    dos2unicode[143] = 197;
    unicode2dos[197] = 143;
    dos_to_utf8[144] = "É";
    utf8_to_dos["É"] = 144;
    dos2unicode[144] = 201;
    unicode2dos[201] = 144;
    dos_to_utf8[145] = "æ";
    utf8_to_dos["æ"] = 145;
    dos2unicode[145] = 230;
    unicode2dos[230] = 145;
    dos_to_utf8[146] = "Æ";
    utf8_to_dos["Æ"] = 146;
    dos2unicode[146] = 198;
    unicode2dos[198] = 146;
    dos_to_utf8[147] = "ô";
    utf8_to_dos["ô"] = 147;
    dos2unicode[147] = 244;
    unicode2dos[244] = 147;
    dos_to_utf8[148] = "ö";
    utf8_to_dos["ö"] = 148;
    dos2unicode[148] = 246;
    unicode2dos[246] = 148;
    dos_to_utf8[149] = "ò";
    utf8_to_dos["ò"] = 149;
    dos2unicode[149] = 242;
    unicode2dos[242] = 149;
    dos_to_utf8[150] = "û";
    utf8_to_dos["û"] = 150;
    dos2unicode[150] = 251;
    unicode2dos[251] = 150;
    dos_to_utf8[151] = "ù";
    utf8_to_dos["ù"] = 151;
    dos2unicode[151] = 249;
    unicode2dos[249] = 151;
    dos_to_utf8[152] = "ÿ";
    utf8_to_dos["ÿ"] = 152;
    dos2unicode[152] = 255;
    unicode2dos[255] = 152;
    dos_to_utf8[153] = "Ö";
    utf8_to_dos["Ö"] = 153;
    dos2unicode[153] = 214;
    unicode2dos[214] = 153;
    dos_to_utf8[154] = "Ü";
    utf8_to_dos["Ü"] = 154;
    dos2unicode[154] = 220;
    unicode2dos[220] = 154;
    dos_to_utf8[155] = "¢";
    utf8_to_dos["¢"] = 155;
    dos2unicode[155] = 162;
    unicode2dos[162] = 155;
    dos_to_utf8[156] = "£";
    utf8_to_dos["£"] = 156;
    dos2unicode[156] = 163;
    unicode2dos[163] = 156;
    dos_to_utf8[157] = "¥";
    utf8_to_dos["¥"] = 157;
    dos2unicode[157] = 165;
    unicode2dos[165] = 157;
    dos_to_utf8[158] = "₧";
    utf8_to_dos["₧"] = 158;
    dos2unicode[158] = 8359;
    unicode2dos[8359] = 158;
    dos_to_utf8[159] = "ƒ";
    utf8_to_dos["ƒ"] = 159;
    dos2unicode[159] = 402;
    unicode2dos[402] = 159;
    dos_to_utf8[160] = "á";
    utf8_to_dos["á"] = 160;
    dos2unicode[160] = 225;
    unicode2dos[225] = 160;
    dos_to_utf8[161] = "í";
    utf8_to_dos["í"] = 161;
    dos2unicode[161] = 237;
    unicode2dos[237] = 161;
    dos_to_utf8[162] = "ó";
    utf8_to_dos["ó"] = 162;
    dos2unicode[162] = 243;
    unicode2dos[243] = 162;
    dos_to_utf8[163] = "ú";
    utf8_to_dos["ú"] = 163;
    dos2unicode[163] = 250;
    unicode2dos[250] = 163;
    dos_to_utf8[164] = "ñ";
    utf8_to_dos["ñ"] = 164;
    dos2unicode[164] = 241;
    unicode2dos[241] = 164;
    dos_to_utf8[165] = "Ñ";
    utf8_to_dos["Ñ"] = 165;
    dos2unicode[165] = 209;
    unicode2dos[209] = 165;
    dos_to_utf8[166] = "ª";
    utf8_to_dos["ª"] = 166;
    dos2unicode[166] = 170;
    unicode2dos[170] = 166;
    dos_to_utf8[167] = "º";
    utf8_to_dos["º"] = 167;
    dos2unicode[167] = 186;
    unicode2dos[186] = 167;
    dos_to_utf8[168] = "¿";
    utf8_to_dos["¿"] = 168;
    dos2unicode[168] = 191;
    unicode2dos[191] = 168;
    dos_to_utf8[169] = "⌐";
    utf8_to_dos["⌐"] = 169;
    dos2unicode[169] = 8976;
    unicode2dos[8976] = 169;
    dos_to_utf8[170] = "¬";
    utf8_to_dos["¬"] = 170;
    dos2unicode[170] = 172;
    unicode2dos[172] = 170;
    dos_to_utf8[171] = "½";
    utf8_to_dos["½"] = 171;
    dos2unicode[171] = 189;
    unicode2dos[189] = 171;
    dos_to_utf8[172] = "¼";
    utf8_to_dos["¼"] = 172;
    dos2unicode[172] = 188;
    unicode2dos[188] = 172;
    dos_to_utf8[173] = "¡";
    utf8_to_dos["¡"] = 173;
    dos2unicode[173] = 161;
    unicode2dos[161] = 173;
    dos_to_utf8[174] = "«";
    utf8_to_dos["«"] = 174;
    dos2unicode[174] = 171;
    unicode2dos[171] = 174;
    dos_to_utf8[175] = "»";
    utf8_to_dos["»"] = 175;
    dos2unicode[175] = 187;
    unicode2dos[187] = 175;
    dos_to_utf8[176] = "░";
    utf8_to_dos["░"] = 176;
    dos2unicode[176] = 9617;
    unicode2dos[9617] = 176;
    dos_to_utf8[177] = "▒";
    utf8_to_dos["▒"] = 177;
    dos2unicode[177] = 9618;
    unicode2dos[9618] = 177;
    dos_to_utf8[178] = "▓";
    utf8_to_dos["▓"] = 178;
    dos2unicode[178] = 9619;
    unicode2dos[9619] = 178;
    dos_to_utf8[179] = "│";
    utf8_to_dos["│"] = 179;
    dos2unicode[179] = 9474;
    unicode2dos[9474] = 179;
    dos_to_utf8[180] = "┤";
    utf8_to_dos["┤"] = 180;
    dos2unicode[180] = 9508;
    unicode2dos[9508] = 180;
    dos_to_utf8[181] = "Á";
    utf8_to_dos["Á"] = 181;
    dos2unicode[181] = 193;
    unicode2dos[193] = 181;
    dos_to_utf8[182] = "Â";
    utf8_to_dos["Â"] = 182;
    dos2unicode[182] = 194;
    unicode2dos[194] = 182;
    dos_to_utf8[183] = "À";
    utf8_to_dos["À"] = 192;
    dos2unicode[183] = 192;
    unicode2dos[192] = 183;
    dos_to_utf8[184] = "╕";
    utf8_to_dos["╕"] = 184;
    dos2unicode[184] = 9557;
    unicode2dos[9557] = 184;
    dos_to_utf8[185] = "╣";
    utf8_to_dos["╣"] = 185;
    dos2unicode[185] = 9571;
    unicode2dos[9571] = 185;
    dos_to_utf8[186] = "║";
    utf8_to_dos["║"] = 186;
    dos2unicode[186] = 9553;
    unicode2dos[9553] = 186;
    dos_to_utf8[187] = "╗";
    utf8_to_dos["╗"] = 187;
    dos2unicode[187] = 9559;
    unicode2dos[9559] = 187;
    dos_to_utf8[188] = "╝";
    utf8_to_dos["╝"] = 188;
    dos2unicode[188] = 9565;
    unicode2dos[9565] = 188;
    dos_to_utf8[189] = "╜";
    utf8_to_dos["╜"] = 189;
    dos2unicode[189] = 9564;
    unicode2dos[9564] = 189;
    dos_to_utf8[190] = "╛";
    utf8_to_dos["╛"] = 190;
    dos2unicode[190] = 9563;
    unicode2dos[9563] = 190;
    dos_to_utf8[191] = "┐";
    utf8_to_dos["┐"] = 191;
    dos2unicode[191] = 9488;
    unicode2dos[9488] = 191;
    dos_to_utf8[192] = "└";
    utf8_to_dos["└"] = 192;
    dos2unicode[192] = 9492;
    unicode2dos[9492] = 192;
    dos_to_utf8[193] = "┴";
    utf8_to_dos["┴"] = 193;
    dos2unicode[193] = 9524;
    unicode2dos[9524] = 193;
    dos_to_utf8[194] = "┬";
    utf8_to_dos["┬"] = 194;
    dos2unicode[194] = 9516;
    unicode2dos[9516] = 194;
    dos_to_utf8[195] = "├";
    utf8_to_dos["├"] = 195;
    dos2unicode[195] = 9500;
    unicode2dos[9500] = 195;
    dos_to_utf8[196] = "─";
    utf8_to_dos["─"] = 196;
    dos2unicode[196] = 9472;
    unicode2dos[9472] = 196;
    dos_to_utf8[197] = "┼";
    utf8_to_dos["┼"] = 197;
    dos2unicode[197] = 9532;
    unicode2dos[9532] = 197;
    dos_to_utf8[198] = "╞";
    utf8_to_dos["╞"] = 198;
    dos2unicode[198] = 9566;
    unicode2dos[9566] = 198;
    dos_to_utf8[199] = "╟";
    utf8_to_dos["╟"] = 199;
    dos2unicode[199] = 9567;
    unicode2dos[9567] = 199;
    dos_to_utf8[200] = "╚";
    utf8_to_dos["╚"] = 200;
    dos2unicode[200] = 9562;
    unicode2dos[9562] = 200;
    dos_to_utf8[201] = "╔";
    utf8_to_dos["╔"] = 201;
    dos2unicode[201] = 9556;
    unicode2dos[9556] = 201;
    dos_to_utf8[202] = "╩";
    utf8_to_dos["╩"] = 202;
    dos2unicode[202] = 9577;
    unicode2dos[9577] = 202;
    dos_to_utf8[203] = "╦";
    utf8_to_dos["╦"] = 203;
    dos2unicode[203] = 9574;
    unicode2dos[9574] = 203;
    dos_to_utf8[204] = "╠";
    utf8_to_dos["╠"] = 204;
    dos2unicode[204] = 9568;
    unicode2dos[9568] = 204;
    dos_to_utf8[205] = "═";
    utf8_to_dos["═"] = 205;
    dos2unicode[205] = 9552;
    unicode2dos[9552] = 205;
    dos_to_utf8[206] = "╬";
    utf8_to_dos["╬"] = 206;
    dos2unicode[206] = 9580;
    unicode2dos[9580] = 206;
    dos_to_utf8[207] = "╧";
    utf8_to_dos["╧"] = 207;
    dos2unicode[207] = 9575;
    unicode2dos[9575] = 207;
    dos_to_utf8[208] = "╨";
    utf8_to_dos["╨"] = 208;
    dos2unicode[208] = 9576;
    unicode2dos[9576] = 208;
    dos_to_utf8[209] = "╤";
    utf8_to_dos["╤"] = 209;
    dos2unicode[209] = 9572;
    unicode2dos[9572] = 209;
    dos_to_utf8[210] = "╥";
    utf8_to_dos["╥"] = 210;
    dos2unicode[210] = 9573;
    unicode2dos[9573] = 210;
    dos_to_utf8[211] = "╙";
    utf8_to_dos["╙"] = 211;
    dos2unicode[211] = 9561;
    unicode2dos[9561] = 211;
    dos_to_utf8[212] = "╘";
    utf8_to_dos["╘"] = 212;
    dos2unicode[212] = 9560;
    unicode2dos[9560] = 212;
    dos_to_utf8[213] = "╒";
    utf8_to_dos["╒"] = 213;
    dos2unicode[213] = 9554;
    unicode2dos[9554] = 213;
    dos_to_utf8[214] = "╓";
    utf8_to_dos["╓"] = 214;
    dos2unicode[214] = 9555;
    unicode2dos[9555] = 214;
    dos_to_utf8[215] = "╫";
    utf8_to_dos["╫"] = 215;
    dos2unicode[215] = 9579;
    unicode2dos[9579] = 215;
    dos_to_utf8[216] = "╪";
    utf8_to_dos["╪"] = 216;
    dos2unicode[216] = 9578;
    unicode2dos[9578] = 216;
    dos_to_utf8[217] = "┘";
    utf8_to_dos["┘"] = 217;
    dos2unicode[217] = 9496;
    unicode2dos[9496] = 217;
    dos_to_utf8[218] = "┌";
    utf8_to_dos["┌"] = 218;
    dos2unicode[218] = 9484;
    unicode2dos[9484] = 218;
    dos_to_utf8[219] = "█";
    utf8_to_dos["█"] = 219;
    dos2unicode[219] = 9608;
    unicode2dos[9608] = 219;
    dos_to_utf8[220] = "▄";
    utf8_to_dos["▄"] = 220;
    dos2unicode[220] = 9604;
    unicode2dos[9604] = 220;
    dos_to_utf8[221] = "▌";
    utf8_to_dos["▌"] = 221;
    dos2unicode[221] = 9612;
    unicode2dos[9612] = 221;
    dos_to_utf8[222] = "▐";
    utf8_to_dos["▐"] = 222;
    dos2unicode[222] = 9616;
    unicode2dos[9616] = 222;
    dos_to_utf8[223] = "▀";
    utf8_to_dos["▀"] = 223;
    dos2unicode[223] = 9600;
    unicode2dos[9600] = 223;
    dos_to_utf8[224] = "α";
    utf8_to_dos["α"] = 224;
    dos2unicode[224] = 945;
    unicode2dos[945] = 224;
    dos_to_utf8[225] = "ß";
    utf8_to_dos["ß"] = 225;
    dos2unicode[225] = 223;
    unicode2dos[223] = 225;
    dos_to_utf8[226] = "Γ";
    utf8_to_dos["Γ"] = 226;
    dos2unicode[226] = 915;
    unicode2dos[915] = 226;
    dos_to_utf8[227] = "π";
    utf8_to_dos["π"] = 227;
    dos2unicode[227] = 960;
    unicode2dos[960] = 227;
    dos_to_utf8[228] = "Σ";
    utf8_to_dos["Σ"] = 228;
    dos2unicode[228] = 931;
    unicode2dos[931] = 228;
    dos_to_utf8[229] = "σ";
    utf8_to_dos["σ"] = 229;
    dos2unicode[229] = 963;
    unicode2dos[963] = 229;
    dos_to_utf8[230] = "µ";
    utf8_to_dos["µ"] = 230;
    dos2unicode[230] = 181;
    unicode2dos[181] = 230;
    dos_to_utf8[231] = "τ";
    utf8_to_dos["τ"] = 231;
    dos2unicode[231] = 964;
    unicode2dos[964] = 231;
    dos_to_utf8[232] = "Φ";
    utf8_to_dos["Φ"] = 232;
    dos2unicode[232] = 934;
    unicode2dos[934] = 232;
    dos_to_utf8[233] = "Θ";
    utf8_to_dos["Θ"] = 233;
    dos2unicode[233] = 920;
    unicode2dos[920] = 233;
    dos_to_utf8[234] = "Ω";
    utf8_to_dos["Ω"] = 234;
    dos2unicode[234] = 937;
    unicode2dos[937] = 234;
    dos_to_utf8[235] = "δ";
    utf8_to_dos["δ"] = 235;
    dos2unicode[235] = 948;
    unicode2dos[948] = 235;
    dos_to_utf8[236] = "∞";
    utf8_to_dos["∞"] = 236;
    dos2unicode[236] = 8734;
    unicode2dos[8734] = 236;
    dos_to_utf8[237] = "φ";
    utf8_to_dos["φ"] = 237;
    dos2unicode[237] = 966;
    unicode2dos[966] = 237;
    dos_to_utf8[238] = "ε";
    utf8_to_dos["ε"] = 238;
    dos2unicode[238] = 949;
    unicode2dos[949] = 238;
    dos_to_utf8[239] = "∩";
    utf8_to_dos["∩"] = 239;
    dos2unicode[239] = 8745;
    unicode2dos[8745] = 239;
    dos_to_utf8[240] = "≡";
    utf8_to_dos["≡"] = 240;
    dos2unicode[240] = 8801;
    unicode2dos[8801] = 240;
    dos_to_utf8[241] = "±";
    utf8_to_dos["±"] = 241;
    dos2unicode[241] = 177;
    unicode2dos[177] = 241;
    dos_to_utf8[242] = "≥";
    utf8_to_dos["≥"] = 242;
    dos2unicode[242] = 8805;
    unicode2dos[8805] = 242;
    dos_to_utf8[243] = "≤";
    utf8_to_dos["≤"] = 243;
    dos2unicode[243] = 8804;
    unicode2dos[8804] = 243;
    dos_to_utf8[244] = "⌠";
    utf8_to_dos["⌠"] = 244;
    dos2unicode[244] = 8992;
    unicode2dos[8992] = 244;
    dos_to_utf8[245] = "⌡";
    utf8_to_dos["⌡"] = 245;
    dos2unicode[245] = 8993;
    unicode2dos[8993] = 245;
    dos_to_utf8[246] = "÷";
    utf8_to_dos["÷"] = 246;
    dos2unicode[246] = 247;
    unicode2dos[247] = 246;
    dos_to_utf8[247] = "≈";
    utf8_to_dos["≈"] = 247;
    dos2unicode[247] = 8776;
    unicode2dos[8776] = 247;
    dos_to_utf8[248] = "°";
    utf8_to_dos["°"] = 248;
    dos2unicode[248] = 176;
    unicode2dos[176] = 248;
    dos_to_utf8[249] = "∙";
    utf8_to_dos["∙"] = 249;
    dos2unicode[249] = 8729;
    unicode2dos[8729] = 249;
    dos_to_utf8[250] = "·";
    utf8_to_dos["·"] = 250;
    dos2unicode[250] = 183;
    unicode2dos[183] = 250;
    dos_to_utf8[251] = "√";
    utf8_to_dos["√"] = 251;
    dos2unicode[251] = 8730;
    unicode2dos[8730] = 251;
    dos_to_utf8[252] = "ⁿ";
    utf8_to_dos["ⁿ"] = 252;
    dos2unicode[252] = 8319;
    unicode2dos[8319] = 252;
    dos_to_utf8[253] = "²";
    utf8_to_dos["²"] = 253;
    dos2unicode[253] = 178;
    unicode2dos[178] = 253;
    dos_to_utf8[254] = "■";
    utf8_to_dos["■"] = 254;
    dos2unicode[254] = 9632;
    unicode2dos[9632] = 254;
}

static void InitHtmlCodes() {
    static bool init=false;
    if (init)
        return;
    init=true;
    
    metacodes["\\n"] = "\n";
    metacodes["\\r"] = "\r";
    metacodes["\\\""] = "\"";
    metacodes["\\t"] = "\t";
    
    wmetacodes[L"\\n"] = L"\n";
    wmetacodes[L"\\r"] = L"\r";
    wmetacodes[L"\\\""] = L"\"";
    wmetacodes[L"\\t"] = L"\t";
    
    codesxml[34] = "&quot;";
    codesxml[38] = "&amp;";
    codesxml[39] = "&apos;";
    codesxml[60] = "&lt;";
    codesxml[62] = "&gt;";
    
    codeshtml[34] = "&quot;";
    codeshtml[38] = "&amp;";
    codeshtml[39] = "&apos;";
    codeshtml[60] = "&lt;";
    codeshtml[62] = "&gt;";
    codeshtml[160] = "&nbsp;";
    codeshtml[161] = "&iexcl;";
    codeshtml[162] = "&cent;";
    codeshtml[163] = "&pound;";
    codeshtml[164] = "&curren;";
    codeshtml[165] = "&yen;";
    codeshtml[166] = "&brvbar;";
    codeshtml[167] = "&sect;";
    codeshtml[168] = "&uml;";
    codeshtml[169] = "&copy;";
    codeshtml[170] = "&ordf;";
    codeshtml[171] = "&laquo;";
    codeshtml[172] = "&not;";
    codeshtml[173] = "&shy;";
    codeshtml[174] = "&reg;";
    codeshtml[175] = "&macr;";
    codeshtml[176] = "&deg;";
    codeshtml[177] = "&plusmn;";
    codeshtml[178] = "&sup2;";
    codeshtml[179] = "&sup3;";
    codeshtml[180] = "&acute;";
    codeshtml[181] = "&micro;";
    codeshtml[182] = "&para;";
    codeshtml[183] = "&middot;";
    codeshtml[184] = "&cedil;";
    codeshtml[185] = "&sup1;";
    codeshtml[186] = "&ordm;";
    codeshtml[187] = "&raquo;";
    codeshtml[188] = "&frac14;";
    codeshtml[189] = "&frac12;";
    codeshtml[190] = "&frac34;";
    codeshtml[191] = "&iquest;";
    codeshtml[192] = "&Agrave;";
    codeshtml[193] = "&Aacute;";
    codeshtml[194] = "&Acirc;";
    codeshtml[195] = "&Atilde;";
    codeshtml[196] = "&Auml;";
    codeshtml[197] = "&Aring;";
    codeshtml[198] = "&AElig;";
    codeshtml[199] = "&Ccedil;";
    codeshtml[200] = "&Egrave;";
    codeshtml[201] = "&Eacute;";
    codeshtml[202] = "&Ecirc;";
    codeshtml[203] = "&Euml;";
    codeshtml[204] = "&Igrave;";
    codeshtml[205] = "&Iacute;";
    codeshtml[206] = "&Icirc;";
    codeshtml[207] = "&Iuml;";
    codeshtml[208] = "&ETH;";
    codeshtml[209] = "&Ntilde;";
    codeshtml[210] = "&Ograve;";
    codeshtml[211] = "&Oacute;";
    codeshtml[212] = "&Ocirc;";
    codeshtml[213] = "&Otilde;";
    codeshtml[214] = "&Ouml;";
    codeshtml[215] = "&times;";
    codeshtml[216] = "&Oslash;";
    codeshtml[217] = "&Ugrave;";
    codeshtml[218] = "&Uacute;";
    codeshtml[219] = "&Ucirc;";
    codeshtml[220] = "&Uuml;";
    codeshtml[221] = "&Yacute;";
    codeshtml[222] = "&THORN;";
    codeshtml[223] = "&szlig;";
    codeshtml[224] = "&agrave;";
    codeshtml[225] = "&aacute;";
    codeshtml[226] = "&acirc;";
    codeshtml[227] = "&atilde;";
    codeshtml[228] = "&auml;";
    codeshtml[229] = "&aring;";
    codeshtml[230] = "&aelig;";
    codeshtml[231] = "&ccedil;";
    codeshtml[232] = "&egrave;";
    codeshtml[233] = "&eacute;";
    codeshtml[234] = "&ecirc;";
    codeshtml[235] = "&euml;";
    codeshtml[236] = "&igrave;";
    codeshtml[237] = "&iacute;";
    codeshtml[238] = "&icirc;";
    codeshtml[239] = "&iuml;";
    codeshtml[240] = "&eth;";
    codeshtml[241] = "&ntilde;";
    codeshtml[242] = "&ograve;";
    codeshtml[243] = "&oacute;";
    codeshtml[244] = "&ocirc;";
    codeshtml[245] = "&otilde;";
    codeshtml[246] = "&ouml;";
    codeshtml[247] = "&divide;";
    codeshtml[248] = "&oslash;";
    codeshtml[249] = "&ugrave;";
    codeshtml[250] = "&uacute;";
    codeshtml[251] = "&ucirc;";
    codeshtml[252] = "&uuml;";
    codeshtml[253] = "&yacute;";
    codeshtml[254] = "&thorn;";
    codeshtml[255] = "&yuml;";
    codeshtml[338] = "&OElig;";
    codeshtml[339] = "&oelig;";
    codeshtml[352] = "&Scaron;";
    codeshtml[353] = "&scaron;";
    codeshtml[376] = "&Yuml;";
    codeshtml[402] = "&fnof;";
    codeshtml[710] = "&circ;";
    codeshtml[732] = "&tilde;";
    codeshtml[913] = "&Alpha;";
    codeshtml[914] = "&Beta;";
    codeshtml[915] = "&Gamma;";
    codeshtml[916] = "&Delta;";
    codeshtml[917] = "&Epsilon;";
    codeshtml[918] = "&Zeta;";
    codeshtml[919] = "&Eta;";
    codeshtml[920] = "&Theta;";
    codeshtml[921] = "&Iota;";
    codeshtml[922] = "&Kappa;";
    codeshtml[923] = "&Lambda;";
    codeshtml[924] = "&Mu;";
    codeshtml[925] = "&Nu;";
    codeshtml[926] = "&Xi;";
    codeshtml[927] = "&Omicron;";
    codeshtml[928] = "&Pi;";
    codeshtml[929] = "&Rho;";
    codeshtml[931] = "&Sigma;";
    codeshtml[932] = "&Tau;";
    codeshtml[933] = "&Upsilon;";
    codeshtml[934] = "&Phi;";
    codeshtml[935] = "&Chi;";
    codeshtml[936] = "&Psi;";
    codeshtml[937] = "&Omega;";
    codeshtml[945] = "&alpha;";
    codeshtml[946] = "&beta;";
    codeshtml[947] = "&gamma;";
    codeshtml[948] = "&delta;";
    codeshtml[949] = "&epsilon;";
    codeshtml[950] = "&zeta;";
    codeshtml[951] = "&eta;";
    codeshtml[952] = "&theta;";
    codeshtml[953] = "&iota;";
    codeshtml[954] = "&kappa;";
    codeshtml[955] = "&lambda;";
    codeshtml[956] = "&mu;";
    codeshtml[957] = "&nu;";
    codeshtml[958] = "&xi;";
    codeshtml[959] = "&omicron;";
    codeshtml[960] = "&pi;";
    codeshtml[961] = "&rho;";
    codeshtml[962] = "&sigmaf;";
    codeshtml[963] = "&sigma;";
    codeshtml[964] = "&tau;";
    codeshtml[965] = "&upsilon;";
    codeshtml[966] = "&phi;";
    codeshtml[967] = "&chi;";
    codeshtml[968] = "&psi;";
    codeshtml[969] = "&omega;";
    codeshtml[977] = "&thetasym;";
    codeshtml[978] = "&upsih;";
    codeshtml[982] = "&piv;";
    codeshtml[8194] = "&ensp;";
    codeshtml[8195] = "&emsp;";
    codeshtml[8201] = "&thinsp;";
    codeshtml[8204] = "&zwnj;";
    codeshtml[8205] = "&zwj;";
    codeshtml[8206] = "&lrm;";
    codeshtml[8207] = "&rlm;";
    codeshtml[8211] = "&ndash;";
    codeshtml[8212] = "&mdash;";
    codeshtml[8216] = "&lsquo;";
    codeshtml[8217] = "&rsquo;";
    codeshtml[8218] = "&sbquo;";
    codeshtml[8220] = "&ldquo;";
    codeshtml[8221] = "&rdquo;";
    codeshtml[8222] = "&bdquo;";
    codeshtml[8224] = "&dagger;";
    codeshtml[8225] = "&Dagger;";
    codeshtml[8226] = "&bull;";
    codeshtml[8230] = "&hellip;";
    codeshtml[8240] = "&permil;";
    codeshtml[8242] = "&prime;";
    codeshtml[8243] = "&Prime;";
    codeshtml[8249] = "&lsaquo;";
    codeshtml[8250] = "&rsaquo;";
    codeshtml[8254] = "&oline;";
    codeshtml[8260] = "&frasl;";
    codeshtml[8364] = "&euro;";
    codeshtml[8465] = "&image;";
    codeshtml[8472] = "&weierp;";
    codeshtml[8476] = "&real;";
    codeshtml[8482] = "&trade;";
    codeshtml[8501] = "&alefsym;";
    codeshtml[8592] = "&larr;";
    codeshtml[8593] = "&uarr;";
    codeshtml[8594] = "&rarr;";
    codeshtml[8595] = "&darr;";
    codeshtml[8596] = "&harr;";
    codeshtml[8629] = "&crarr;";
    codeshtml[8656] = "&lArr;";
    codeshtml[8657] = "&uArr;";
    codeshtml[8658] = "&rArr;";
    codeshtml[8659] = "&dArr;";
    codeshtml[8660] = "&hArr;";
    codeshtml[8704] = "&forall;";
    codeshtml[8706] = "&part;";
    codeshtml[8707] = "&exist;";
    codeshtml[8709] = "&empty;";
    codeshtml[8711] = "&nabla;";
    codeshtml[8712] = "&isin;";
    codeshtml[8713] = "&notin;";
    codeshtml[8715] = "&ni;";
    codeshtml[8719] = "&prod;";
    codeshtml[8721] = "&sum;";
    codeshtml[8722] = "&minus;";
    codeshtml[8727] = "&lowast;";
    codeshtml[8730] = "&radic;";
    codeshtml[8733] = "&prop;";
    codeshtml[8734] = "&infin;";
    codeshtml[8736] = "&ang;";
    codeshtml[8743] = "&and;";
    codeshtml[8744] = "&or;";
    codeshtml[8745] = "&cap;";
    codeshtml[8746] = "&cup;";
    codeshtml[8747] = "&int;";
    codeshtml[8756] = "&there4;";
    codeshtml[8764] = "&sim;";
    codeshtml[8773] = "&cong;";
    codeshtml[8776] = "&asymp;";
    codeshtml[8800] = "&ne;";
    codeshtml[8801] = "&equiv;";
    codeshtml[8804] = "&le;";
    codeshtml[8805] = "&ge;";
    codeshtml[8834] = "&sub;";
    codeshtml[8835] = "&sup;";
    codeshtml[8836] = "&nsub;";
    codeshtml[8838] = "&sube;";
    codeshtml[8839] = "&supe;";
    codeshtml[8853] = "&oplus;";
    codeshtml[8855] = "&otimes;";
    codeshtml[8869] = "&perp;";
    codeshtml[8901] = "&sdot;";
    codeshtml[8968] = "&lceil;";
    codeshtml[8969] = "&rceil;";
    codeshtml[8970] = "&lfloor;";
    codeshtml[8971] = "&rfloor;";
    codeshtml[9001] = "&lang;";
    codeshtml[9002] = "&rang;";
    codeshtml[9674] = "&loz;";
    codeshtml[9824] = "&spades;";
    codeshtml[9827] = "&clubs;";
    codeshtml[9829] = "&hearts;";
    codeshtml[9830] = "&diams;";
    
    xmlcodes["&quot;"] = 34;
    xmlcodes["&amp;"] = 38;
    xmlcodes["&apos;"] = 39;
    xmlcodes["&lt;"] = 60;
    xmlcodes["&gt;"] = 62;
    
    htmlcodes["&quot;"] = 34;
    htmlcodes["&amp;"] = 38;
    htmlcodes["&apos;"] = 39;
    htmlcodes["&lt;"] = 60;
    htmlcodes["&gt;"] = 62;
    htmlcodes["&nbsp;"] = 160;
    htmlcodes["&iexcl;"] = 161;
    htmlcodes["&cent;"] = 162;
    htmlcodes["&pound;"] = 163;
    htmlcodes["&curren;"] = 164;
    htmlcodes["&yen;"] = 165;
    htmlcodes["&brvbar;"] = 166;
    htmlcodes["&sect;"] = 167;
    htmlcodes["&uml;"] = 168;
    htmlcodes["&copy;"] = 169;
    htmlcodes["&ordf;"] = 170;
    htmlcodes["&laquo;"] = 171;
    htmlcodes["&not;"] = 172;
    htmlcodes["&shy;"] = 173;
    htmlcodes["&reg;"] = 174;
    htmlcodes["&macr;"] = 175;
    htmlcodes["&deg;"] = 176;
    htmlcodes["&plusmn;"] = 177;
    htmlcodes["&sup2;"] = 178;
    htmlcodes["&sup3;"] = 179;
    htmlcodes["&acute;"] = 180;
    htmlcodes["&micro;"] = 181;
    htmlcodes["&para;"] = 182;
    htmlcodes["&middot;"] = 183;
    htmlcodes["&cedil;"] = 184;
    htmlcodes["&sup1;"] = 185;
    htmlcodes["&ordm;"] = 186;
    htmlcodes["&raquo;"] = 187;
    htmlcodes["&frac14;"] = 188;
    htmlcodes["&frac12;"] = 189;
    htmlcodes["&frac34;"] = 190;
    htmlcodes["&iquest;"] = 191;
    htmlcodes["&Agrave;"] = 192;
    htmlcodes["&Aacute;"] = 193;
    htmlcodes["&Acirc;"] = 194;
    htmlcodes["&Atilde;"] = 195;
    htmlcodes["&Auml;"] = 196;
    htmlcodes["&Aring;"] = 197;
    htmlcodes["&AElig;"] = 198;
    htmlcodes["&Ccedil;"] = 199;
    htmlcodes["&Egrave;"] = 200;
    htmlcodes["&Eacute;"] = 201;
    htmlcodes["&Ecirc;"] = 202;
    htmlcodes["&Euml;"] = 203;
    htmlcodes["&Igrave;"] = 204;
    htmlcodes["&Iacute;"] = 205;
    htmlcodes["&Icirc;"] = 206;
    htmlcodes["&Iuml;"] = 207;
    htmlcodes["&ETH;"] = 208;
    htmlcodes["&Ntilde;"] = 209;
    htmlcodes["&Ograve;"] = 210;
    htmlcodes["&Oacute;"] = 211;
    htmlcodes["&Ocirc;"] = 212;
    htmlcodes["&Otilde;"] = 213;
    htmlcodes["&Ouml;"] = 214;
    htmlcodes["&times;"] = 215;
    htmlcodes["&Oslash;"] = 216;
    htmlcodes["&Ugrave;"] = 217;
    htmlcodes["&Uacute;"] = 218;
    htmlcodes["&Ucirc;"] = 219;
    htmlcodes["&Uuml;"] = 220;
    htmlcodes["&Yacute;"] = 221;
    htmlcodes["&THORN;"] = 222;
    htmlcodes["&szlig;"] = 223;
    htmlcodes["&agrave;"] = 224;
    htmlcodes["&aacute;"] = 225;
    htmlcodes["&acirc;"] = 226;
    htmlcodes["&atilde;"] = 227;
    htmlcodes["&auml;"] = 228;
    htmlcodes["&aring;"] = 229;
    htmlcodes["&aelig;"] = 230;
    htmlcodes["&ccedil;"] = 231;
    htmlcodes["&egrave;"] = 232;
    htmlcodes["&eacute;"] = 233;
    htmlcodes["&ecirc;"] = 234;
    htmlcodes["&euml;"] = 235;
    htmlcodes["&igrave;"] = 236;
    htmlcodes["&iacute;"] = 237;
    htmlcodes["&icirc;"] = 238;
    htmlcodes["&iuml;"] = 239;
    htmlcodes["&eth;"] = 240;
    htmlcodes["&ntilde;"] = 241;
    htmlcodes["&ograve;"] = 242;
    htmlcodes["&oacute;"] = 243;
    htmlcodes["&ocirc;"] = 244;
    htmlcodes["&otilde;"] = 245;
    htmlcodes["&ouml;"] = 246;
    htmlcodes["&divide;"] = 247;
    htmlcodes["&oslash;"] = 248;
    htmlcodes["&ugrave;"] = 249;
    htmlcodes["&uacute;"] = 250;
    htmlcodes["&ucirc;"] = 251;
    htmlcodes["&uuml;"] = 252;
    htmlcodes["&yacute;"] = 253;
    htmlcodes["&thorn;"] = 254;
    htmlcodes["&yuml;"] = 255;
    htmlcodes["&OElig;"] = 338;
    htmlcodes["&oelig;"] = 339;
    htmlcodes["&Scaron;"] = 352;
    htmlcodes["&scaron;"] = 353;
    htmlcodes["&Yuml;"] = 376;
    htmlcodes["&fnof;"] = 402;
    htmlcodes["&circ;"] = 710;
    htmlcodes["&tilde;"] = 732;
    htmlcodes["&Alpha;"] = 913;
    htmlcodes["&Beta;"] = 914;
    htmlcodes["&Gamma;"] = 915;
    htmlcodes["&Delta;"] = 916;
    htmlcodes["&Epsilon;"] = 917;
    htmlcodes["&Zeta;"] = 918;
    htmlcodes["&Eta;"] = 919;
    htmlcodes["&Theta;"] = 920;
    htmlcodes["&Iota;"] = 921;
    htmlcodes["&Kappa;"] = 922;
    htmlcodes["&Lambda;"] = 923;
    htmlcodes["&Mu;"] = 924;
    htmlcodes["&Nu;"] = 925;
    htmlcodes["&Xi;"] = 926;
    htmlcodes["&Omicron;"] = 927;
    htmlcodes["&Pi;"] = 928;
    htmlcodes["&Rho;"] = 929;
    htmlcodes["&Sigma;"] = 931;
    htmlcodes["&Tau;"] = 932;
    htmlcodes["&Upsilon;"] = 933;
    htmlcodes["&Phi;"] = 934;
    htmlcodes["&Chi;"] = 935;
    htmlcodes["&Psi;"] = 936;
    htmlcodes["&Omega;"] = 937;
    htmlcodes["&alpha;"] = 945;
    htmlcodes["&beta;"] = 946;
    htmlcodes["&gamma;"] = 947;
    htmlcodes["&delta;"] = 948;
    htmlcodes["&epsilon;"] = 949;
    htmlcodes["&zeta;"] = 950;
    htmlcodes["&eta;"] = 951;
    htmlcodes["&theta;"] = 952;
    htmlcodes["&iota;"] = 953;
    htmlcodes["&kappa;"] = 954;
    htmlcodes["&lambda;"] = 955;
    htmlcodes["&mu;"] = 956;
    htmlcodes["&nu;"] = 957;
    htmlcodes["&xi;"] = 958;
    htmlcodes["&omicron;"] = 959;
    htmlcodes["&pi;"] = 960;
    htmlcodes["&rho;"] = 961;
    htmlcodes["&sigmaf;"] = 962;
    htmlcodes["&sigma;"] = 963;
    htmlcodes["&tau;"] = 964;
    htmlcodes["&upsilon;"] = 965;
    htmlcodes["&phi;"] = 966;
    htmlcodes["&chi;"] = 967;
    htmlcodes["&psi;"] = 968;
    htmlcodes["&omega;"] = 969;
    htmlcodes["&thetasym;"] = 977;
    htmlcodes["&upsih;"] = 978;
    htmlcodes["&piv;"] = 982;
    htmlcodes["&ensp;"] = 8194;
    htmlcodes["&emsp;"] = 8195;
    htmlcodes["&thinsp;"] = 8201;
    htmlcodes["&zwnj;"] = 8204;
    htmlcodes["&zwj;"] = 8205;
    htmlcodes["&lrm;"] = 8206;
    htmlcodes["&rlm;"] = 8207;
    htmlcodes["&ndash;"] = 8211;
    htmlcodes["&mdash;"] = 8212;
    htmlcodes["&lsquo;"] = 8216;
    htmlcodes["&rsquo;"] = 8217;
    htmlcodes["&sbquo;"] = 8218;
    htmlcodes["&ldquo;"] = 8220;
    htmlcodes["&rdquo;"] = 8221;
    htmlcodes["&bdquo;"] = 8222;
    htmlcodes["&dagger;"] = 8224;
    htmlcodes["&Dagger;"] = 8225;
    htmlcodes["&bull;"] = 8226;
    htmlcodes["&hellip;"] = 8230;
    htmlcodes["&permil;"] = 8240;
    htmlcodes["&prime;"] = 8242;
    htmlcodes["&Prime;"] = 8243;
    htmlcodes["&lsaquo;"] = 8249;
    htmlcodes["&rsaquo;"] = 8250;
    htmlcodes["&oline;"] = 8254;
    htmlcodes["&frasl;"] = 8260;
    htmlcodes["&euro;"] = 8364;
    htmlcodes["&image;"] = 8465;
    htmlcodes["&weierp;"] = 8472;
    htmlcodes["&real;"] = 8476;
    htmlcodes["&trade;"] = 8482;
    htmlcodes["&alefsym;"] = 8501;
    htmlcodes["&larr;"] = 8592;
    htmlcodes["&uarr;"] = 8593;
    htmlcodes["&rarr;"] = 8594;
    htmlcodes["&darr;"] = 8595;
    htmlcodes["&harr;"] = 8596;
    htmlcodes["&crarr;"] = 8629;
    htmlcodes["&lArr;"] = 8656;
    htmlcodes["&uArr;"] = 8657;
    htmlcodes["&rArr;"] = 8658;
    htmlcodes["&dArr;"] = 8659;
    htmlcodes["&hArr;"] = 8660;
    htmlcodes["&forall;"] = 8704;
    htmlcodes["&part;"] = 8706;
    htmlcodes["&exist;"] = 8707;
    htmlcodes["&empty;"] = 8709;
    htmlcodes["&nabla;"] = 8711;
    htmlcodes["&isin;"] = 8712;
    htmlcodes["&notin;"] = 8713;
    htmlcodes["&ni;"] = 8715;
    htmlcodes["&prod;"] = 8719;
    htmlcodes["&sum;"] = 8721;
    htmlcodes["&minus;"] = 8722;
    htmlcodes["&lowast;"] = 8727;
    htmlcodes["&radic;"] = 8730;
    htmlcodes["&prop;"] = 8733;
    htmlcodes["&infin;"] = 8734;
    htmlcodes["&ang;"] = 8736;
    htmlcodes["&and;"] = 8743;
    htmlcodes["&or;"] = 8744;
    htmlcodes["&cap;"] = 8745;
    htmlcodes["&cup;"] = 8746;
    htmlcodes["&int;"] = 8747;
    htmlcodes["&there4;"] = 8756;
    htmlcodes["&sim;"] = 8764;
    htmlcodes["&cong;"] = 8773;
    htmlcodes["&asymp;"] = 8776;
    htmlcodes["&ne;"] = 8800;
    htmlcodes["&equiv;"] = 8801;
    htmlcodes["&le;"] = 8804;
    htmlcodes["&ge;"] = 8805;
    htmlcodes["&sub;"] = 8834;
    htmlcodes["&sup;"] = 8835;
    htmlcodes["&nsub;"] = 8836;
    htmlcodes["&sube;"] = 8838;
    htmlcodes["&supe;"] = 8839;
    htmlcodes["&oplus;"] = 8853;
    htmlcodes["&otimes;"] = 8855;
    htmlcodes["&perp;"] = 8869;
    htmlcodes["&sdot;"] = 8901;
    htmlcodes["&lceil;"] = 8968;
    htmlcodes["&rceil;"] = 8969;
    htmlcodes["&lfloor;"] = 8970;
    htmlcodes["&rfloor;"] = 8971;
    htmlcodes["&lang;"] = 9001;
    htmlcodes["&rang;"] = 9002;
    htmlcodes["&loz;"] = 9674;
    htmlcodes["&spades;"] = 9824;
    htmlcodes["&clubs;"] = 9827;
    htmlcodes["&hearts;"] = 9829;
    htmlcodes["&diams;"] = 9830;
    
    whtmlcodes[L"&quot;"] = 34;
    whtmlcodes[L"&amp;"] = 38;
    whtmlcodes[L"&apos;"] = 39;
    whtmlcodes[L"&lt;"] = 60;
    whtmlcodes[L"&gt;"] = 62;
    whtmlcodes[L"&nbsp;"] = 160;
    whtmlcodes[L"&iexcl;"] = 161;
    whtmlcodes[L"&cent;"] = 162;
    whtmlcodes[L"&pound;"] = 163;
    whtmlcodes[L"&curren;"] = 164;
    whtmlcodes[L"&yen;"] = 165;
    whtmlcodes[L"&brvbar;"] = 166;
    whtmlcodes[L"&sect;"] = 167;
    whtmlcodes[L"&uml;"] = 168;
    whtmlcodes[L"&copy;"] = 169;
    whtmlcodes[L"&ordf;"] = 170;
    whtmlcodes[L"&laquo;"] = 171;
    whtmlcodes[L"&not;"] = 172;
    whtmlcodes[L"&shy;"] = 173;
    whtmlcodes[L"&reg;"] = 174;
    whtmlcodes[L"&macr;"] = 175;
    whtmlcodes[L"&deg;"] = 176;
    whtmlcodes[L"&plusmn;"] = 177;
    whtmlcodes[L"&sup2;"] = 178;
    whtmlcodes[L"&sup3;"] = 179;
    whtmlcodes[L"&acute;"] = 180;
    whtmlcodes[L"&micro;"] = 181;
    whtmlcodes[L"&para;"] = 182;
    whtmlcodes[L"&middot;"] = 183;
    whtmlcodes[L"&cedil;"] = 184;
    whtmlcodes[L"&sup1;"] = 185;
    whtmlcodes[L"&ordm;"] = 186;
    whtmlcodes[L"&raquo;"] = 187;
    whtmlcodes[L"&frac14;"] = 188;
    whtmlcodes[L"&frac12;"] = 189;
    whtmlcodes[L"&frac34;"] = 190;
    whtmlcodes[L"&iquest;"] = 191;
    whtmlcodes[L"&Agrave;"] = 192;
    whtmlcodes[L"&Aacute;"] = 193;
    whtmlcodes[L"&Acirc;"] = 194;
    whtmlcodes[L"&Atilde;"] = 195;
    whtmlcodes[L"&Auml;"] = 196;
    whtmlcodes[L"&Aring;"] = 197;
    whtmlcodes[L"&AElig;"] = 198;
    whtmlcodes[L"&Ccedil;"] = 199;
    whtmlcodes[L"&Egrave;"] = 200;
    whtmlcodes[L"&Eacute;"] = 201;
    whtmlcodes[L"&Ecirc;"] = 202;
    whtmlcodes[L"&Euml;"] = 203;
    whtmlcodes[L"&Igrave;"] = 204;
    whtmlcodes[L"&Iacute;"] = 205;
    whtmlcodes[L"&Icirc;"] = 206;
    whtmlcodes[L"&Iuml;"] = 207;
    whtmlcodes[L"&ETH;"] = 208;
    whtmlcodes[L"&Ntilde;"] = 209;
    whtmlcodes[L"&Ograve;"] = 210;
    whtmlcodes[L"&Oacute;"] = 211;
    whtmlcodes[L"&Ocirc;"] = 212;
    whtmlcodes[L"&Otilde;"] = 213;
    whtmlcodes[L"&Ouml;"] = 214;
    whtmlcodes[L"&times;"] = 215;
    whtmlcodes[L"&Oslash;"] = 216;
    whtmlcodes[L"&Ugrave;"] = 217;
    whtmlcodes[L"&Uacute;"] = 218;
    whtmlcodes[L"&Ucirc;"] = 219;
    whtmlcodes[L"&Uuml;"] = 220;
    whtmlcodes[L"&Yacute;"] = 221;
    whtmlcodes[L"&THORN;"] = 222;
    whtmlcodes[L"&szlig;"] = 223;
    whtmlcodes[L"&agrave;"] = 224;
    whtmlcodes[L"&aacute;"] = 225;
    whtmlcodes[L"&acirc;"] = 226;
    whtmlcodes[L"&atilde;"] = 227;
    whtmlcodes[L"&auml;"] = 228;
    whtmlcodes[L"&aring;"] = 229;
    whtmlcodes[L"&aelig;"] = 230;
    whtmlcodes[L"&ccedil;"] = 231;
    whtmlcodes[L"&egrave;"] = 232;
    whtmlcodes[L"&eacute;"] = 233;
    whtmlcodes[L"&ecirc;"] = 234;
    whtmlcodes[L"&euml;"] = 235;
    whtmlcodes[L"&igrave;"] = 236;
    whtmlcodes[L"&iacute;"] = 237;
    whtmlcodes[L"&icirc;"] = 238;
    whtmlcodes[L"&iuml;"] = 239;
    whtmlcodes[L"&eth;"] = 240;
    whtmlcodes[L"&ntilde;"] = 241;
    whtmlcodes[L"&ograve;"] = 242;
    whtmlcodes[L"&oacute;"] = 243;
    whtmlcodes[L"&ocirc;"] = 244;
    whtmlcodes[L"&otilde;"] = 245;
    whtmlcodes[L"&ouml;"] = 246;
    whtmlcodes[L"&divide;"] = 247;
    whtmlcodes[L"&oslash;"] = 248;
    whtmlcodes[L"&ugrave;"] = 249;
    whtmlcodes[L"&uacute;"] = 250;
    whtmlcodes[L"&ucirc;"] = 251;
    whtmlcodes[L"&uuml;"] = 252;
    whtmlcodes[L"&yacute;"] = 253;
    whtmlcodes[L"&thorn;"] = 254;
    whtmlcodes[L"&yuml;"] = 255;
    whtmlcodes[L"&OElig;"] = 338;
    whtmlcodes[L"&oelig;"] = 339;
    whtmlcodes[L"&Scaron;"] = 352;
    whtmlcodes[L"&scaron;"] = 353;
    whtmlcodes[L"&Yuml;"] = 376;
    whtmlcodes[L"&fnof;"] = 402;
    whtmlcodes[L"&circ;"] = 710;
    whtmlcodes[L"&tilde;"] = 732;
    whtmlcodes[L"&Alpha;"] = 913;
    whtmlcodes[L"&Beta;"] = 914;
    whtmlcodes[L"&Gamma;"] = 915;
    whtmlcodes[L"&Delta;"] = 916;
    whtmlcodes[L"&Epsilon;"] = 917;
    whtmlcodes[L"&Zeta;"] = 918;
    whtmlcodes[L"&Eta;"] = 919;
    whtmlcodes[L"&Theta;"] = 920;
    whtmlcodes[L"&Iota;"] = 921;
    whtmlcodes[L"&Kappa;"] = 922;
    whtmlcodes[L"&Lambda;"] = 923;
    whtmlcodes[L"&Mu;"] = 924;
    whtmlcodes[L"&Nu;"] = 925;
    whtmlcodes[L"&Xi;"] = 926;
    whtmlcodes[L"&Omicron;"] = 927;
    whtmlcodes[L"&Pi;"] = 928;
    whtmlcodes[L"&Rho;"] = 929;
    whtmlcodes[L"&Sigma;"] = 931;
    whtmlcodes[L"&Tau;"] = 932;
    whtmlcodes[L"&Upsilon;"] = 933;
    whtmlcodes[L"&Phi;"] = 934;
    whtmlcodes[L"&Chi;"] = 935;
    whtmlcodes[L"&Psi;"] = 936;
    whtmlcodes[L"&Omega;"] = 937;
    whtmlcodes[L"&alpha;"] = 945;
    whtmlcodes[L"&beta;"] = 946;
    whtmlcodes[L"&gamma;"] = 947;
    whtmlcodes[L"&delta;"] = 948;
    whtmlcodes[L"&epsilon;"] = 949;
    whtmlcodes[L"&zeta;"] = 950;
    whtmlcodes[L"&eta;"] = 951;
    whtmlcodes[L"&theta;"] = 952;
    whtmlcodes[L"&iota;"] = 953;
    whtmlcodes[L"&kappa;"] = 954;
    whtmlcodes[L"&lambda;"] = 955;
    whtmlcodes[L"&mu;"] = 956;
    whtmlcodes[L"&nu;"] = 957;
    whtmlcodes[L"&xi;"] = 958;
    whtmlcodes[L"&omicron;"] = 959;
    whtmlcodes[L"&pi;"] = 960;
    whtmlcodes[L"&rho;"] = 961;
    whtmlcodes[L"&sigmaf;"] = 962;
    whtmlcodes[L"&sigma;"] = 963;
    whtmlcodes[L"&tau;"] = 964;
    whtmlcodes[L"&upsilon;"] = 965;
    whtmlcodes[L"&phi;"] = 966;
    whtmlcodes[L"&chi;"] = 967;
    whtmlcodes[L"&psi;"] = 968;
    whtmlcodes[L"&omega;"] = 969;
    whtmlcodes[L"&thetasym;"] = 977;
    whtmlcodes[L"&upsih;"] = 978;
    whtmlcodes[L"&piv;"] = 982;
    whtmlcodes[L"&ensp;"] = 8194;
    whtmlcodes[L"&emsp;"] = 8195;
    whtmlcodes[L"&thinsp;"] = 8201;
    whtmlcodes[L"&zwnj;"] = 8204;
    whtmlcodes[L"&zwj;"] = 8205;
    whtmlcodes[L"&lrm;"] = 8206;
    whtmlcodes[L"&rlm;"] = 8207;
    whtmlcodes[L"&ndash;"] = 8211;
    whtmlcodes[L"&mdash;"] = 8212;
    whtmlcodes[L"&lsquo;"] = 8216;
    whtmlcodes[L"&rsquo;"] = 8217;
    whtmlcodes[L"&sbquo;"] = 8218;
    whtmlcodes[L"&ldquo;"] = 8220;
    whtmlcodes[L"&rdquo;"] = 8221;
    whtmlcodes[L"&bdquo;"] = 8222;
    whtmlcodes[L"&dagger;"] = 8224;
    whtmlcodes[L"&Dagger;"] = 8225;
    whtmlcodes[L"&bull;"] = 8226;
    whtmlcodes[L"&hellip;"] = 8230;
    whtmlcodes[L"&permil;"] = 8240;
    whtmlcodes[L"&prime;"] = 8242;
    whtmlcodes[L"&Prime;"] = 8243;
    whtmlcodes[L"&lsaquo;"] = 8249;
    whtmlcodes[L"&rsaquo;"] = 8250;
    whtmlcodes[L"&oline;"] = 8254;
    whtmlcodes[L"&frasl;"] = 8260;
    whtmlcodes[L"&euro;"] = 8364;
    whtmlcodes[L"&image;"] = 8465;
    whtmlcodes[L"&weierp;"] = 8472;
    whtmlcodes[L"&real;"] = 8476;
    whtmlcodes[L"&trade;"] = 8482;
    whtmlcodes[L"&alefsym;"] = 8501;
    whtmlcodes[L"&larr;"] = 8592;
    whtmlcodes[L"&uarr;"] = 8593;
    whtmlcodes[L"&rarr;"] = 8594;
    whtmlcodes[L"&darr;"] = 8595;
    whtmlcodes[L"&harr;"] = 8596;
    whtmlcodes[L"&crarr;"] = 8629;
    whtmlcodes[L"&lArr;"] = 8656;
    whtmlcodes[L"&uArr;"] = 8657;
    whtmlcodes[L"&rArr;"] = 8658;
    whtmlcodes[L"&dArr;"] = 8659;
    whtmlcodes[L"&hArr;"] = 8660;
    whtmlcodes[L"&forall;"] = 8704;
    whtmlcodes[L"&part;"] = 8706;
    whtmlcodes[L"&exist;"] = 8707;
    whtmlcodes[L"&empty;"] = 8709;
    whtmlcodes[L"&nabla;"] = 8711;
    whtmlcodes[L"&isin;"] = 8712;
    whtmlcodes[L"&notin;"] = 8713;
    whtmlcodes[L"&ni;"] = 8715;
    whtmlcodes[L"&prod;"] = 8719;
    whtmlcodes[L"&sum;"] = 8721;
    whtmlcodes[L"&minus;"] = 8722;
    whtmlcodes[L"&lowast;"] = 8727;
    whtmlcodes[L"&radic;"] = 8730;
    whtmlcodes[L"&prop;"] = 8733;
    whtmlcodes[L"&infin;"] = 8734;
    whtmlcodes[L"&ang;"] = 8736;
    whtmlcodes[L"&and;"] = 8743;
    whtmlcodes[L"&or;"] = 8744;
    whtmlcodes[L"&cap;"] = 8745;
    whtmlcodes[L"&cup;"] = 8746;
    whtmlcodes[L"&int;"] = 8747;
    whtmlcodes[L"&there4;"] = 8756;
    whtmlcodes[L"&sim;"] = 8764;
    whtmlcodes[L"&cong;"] = 8773;
    whtmlcodes[L"&asymp;"] = 8776;
    whtmlcodes[L"&ne;"] = 8800;
    whtmlcodes[L"&equiv;"] = 8801;
    whtmlcodes[L"&le;"] = 8804;
    whtmlcodes[L"&ge;"] = 8805;
    whtmlcodes[L"&sub;"] = 8834;
    whtmlcodes[L"&sup;"] = 8835;
    whtmlcodes[L"&nsub;"] = 8836;
    whtmlcodes[L"&sube;"] = 8838;
    whtmlcodes[L"&supe;"] = 8839;
    whtmlcodes[L"&oplus;"] = 8853;
    whtmlcodes[L"&otimes;"] = 8855;
    whtmlcodes[L"&perp;"] = 8869;
    whtmlcodes[L"&sdot;"] = 8901;
    whtmlcodes[L"&lceil;"] = 8968;
    whtmlcodes[L"&rceil;"] = 8969;
    whtmlcodes[L"&lfloor;"] = 8970;
    whtmlcodes[L"&rfloor;"] = 8971;
    whtmlcodes[L"&lang;"] = 9001;
    whtmlcodes[L"&rang;"] = 9002;
    whtmlcodes[L"&loz;"] = 9674;
    whtmlcodes[L"&spades;"] = 9824;
    whtmlcodes[L"&clubs;"] = 9827;
    whtmlcodes[L"&hearts;"] = 9829;
    whtmlcodes[L"&diams;"] = 9830;
}

static void  InitLatinTables() {
    long i = 0;
    
    char ch[5];
    while (latin2codetable[i] != 0) {
        latin2codes[latin2codetable[i]] = latin2codetable[i + 1];
        codeslatin[latin2codetable[i + 1]] = latin2codetable[i];
        c_unicode_to_utf8(latin2codetable[i + 1], (uchar*)ch);
        utf8codes[ch] = latin2codetable[i];
        i += 2;
    }
    
    
    i = 0;
    while (latin3codetable[i] != 0) {
        latin3codes[latin3codetable[i]] = latin3codetable[i + 1];
        codeslatin[latin3codetable[i + 1]] = latin3codetable[i];
        c_unicode_to_utf8(latin3codetable[i + 1], (uchar*)ch);
        utf8codes[ch] = latin3codetable[i];
        i += 2;
    }
    
    
    i = 0;
    while (latin4codetable[i] != 0) {
        latin4codes[latin4codetable[i]] = latin4codetable[i + 1];
        codeslatin[latin4codetable[i + 1]] = latin4codetable[i];
        c_unicode_to_utf8(latin4codetable[i + 1], (uchar*)ch);
        utf8codes[ch] = latin4codetable[i];
        i += 2;
    }
    
    
    i = 0;
    while (latin5codetable[i] != 0) {
        latin5codes[latin5codetable[i]] = latin5codetable[i + 1];
        codeslatin[latin5codetable[i + 1]] = latin5codetable[i];
        c_unicode_to_utf8(latin5codetable[i + 1], (uchar*)ch);
        utf8codes[ch] = latin5codetable[i];
        i += 2;
    }
    
    
    i = 0;
    while (latin6codetable[i] != 0) {
        latin6codes[latin6codetable[i]] = latin6codetable[i + 1];
        codeslatin[latin6codetable[i + 1]] = latin6codetable[i];
        c_unicode_to_utf8(latin6codetable[i + 1], (uchar*)ch);
        utf8codes[ch] = latin6codetable[i];
        i += 2;
    }
    
    
    i = 0;
    while (latin7codetable[i] != 0) {
        latin7codes[latin7codetable[i]] = latin7codetable[i + 1];
        codeslatin[latin7codetable[i + 1]] = latin7codetable[i];
        c_unicode_to_utf8(latin7codetable[i + 1], (uchar*)ch);
        utf8codes[ch] = latin7codetable[i];
        i += 2;
    }
    
    
    i = 0;
    while (latin8codetable[i] != 0) {
        latin8codes[latin8codetable[i]] = latin8codetable[i + 1];
        codeslatin[latin8codetable[i + 1]] = latin8codetable[i];
        c_unicode_to_utf8(latin8codetable[i + 1], (uchar*)ch);
        utf8codes[ch] = latin8codetable[i];
        i += 2;
    }
    
    
    i = 0;
    while (latin9codetable[i] != 0) {
        latin9codes[latin9codetable[i]] = latin9codetable[i + 1];
        codeslatin[latin9codetable[i + 1]] = latin9codetable[i];
        c_unicode_to_utf8(latin9codetable[i + 1], (uchar*)ch);
        utf8codes[ch] = latin9codetable[i];
        i += 2;
    }
    
    
    i = 0;
    while (latin10codetable[i] != 0) {
        latin10codes[latin10codetable[i]] = latin10codetable[i + 1];
        codeslatin[latin10codetable[i + 1]] = latin10codetable[i];
        c_unicode_to_utf8(latin10codetable[i + 1], (uchar*)ch);
        utf8codes[ch] = latin10codetable[i];
        i += 2;
    }
    
    
    i = 0;
    while (latin11codetable[i] != 0) {
        latin11codes[latin11codetable[i]] = latin11codetable[i + 1];
        codeslatin[latin11codetable[i + 1]] = latin11codetable[i];
        c_unicode_to_utf8(latin11codetable[i + 1], (uchar*)ch);
        utf8codes[ch] = latin11codetable[i];
        i += 2;
    }
    
    i = 0;
    while (latin12codetable[i] != 0) {
        latin12codes[latin12codetable[i]] = latin12codetable[i + 1];
        codeslatin[latin12codetable[i + 1]] = latin12codetable[i];
        c_unicode_to_utf8(latin12codetable[i + 1], (uchar*)ch);
        utf8codes[ch] = latin12codetable[i];
        i += 2;
    }
    
    i = 0;
    while (latin13codetable[i] != 0) {
        latin13codes[latin13codetable[i]] = latin13codetable[i + 1];
        codeslatin[latin13codetable[i + 1]] = latin13codetable[i];
        c_unicode_to_utf8(latin13codetable[i + 1], (uchar*)ch);
        utf8codes[ch] = latin13codetable[i];
        i += 2;
    }
    
    
    i = 0;
    while (latin14codetable[i] != 0) {
        latin14codes[latin14codetable[i]] = latin14codetable[i + 1];
        codeslatin[latin14codetable[i + 1]] = latin14codetable[i];
        c_unicode_to_utf8(latin14codetable[i + 1], (uchar*)ch);
        utf8codes[ch] = latin14codetable[i];
        i += 2;
    }
    
    
    i = 0;
    while (latin15codetable[i] != 0) {
        latin15codes[latin15codetable[i]] = latin15codetable[i + 1];
        codeslatin[latin15codetable[i + 1]] = latin15codetable[i];
        c_unicode_to_utf8(latin15codetable[i + 1], (uchar*)ch);
        utf8codes[ch] = latin15codetable[i];
        i += 2;
    }
    
    
    i = 0;
    while (latin16codetable[i] != 0) {
        latin16codes[latin16codetable[i]] = latin16codetable[i + 1];
        codeslatin[latin16codetable[i + 1]] = latin16codetable[i];
        c_unicode_to_utf8(latin16codetable[i + 1], (uchar*)ch);
        utf8codes[ch] = latin16codetable[i];
        i += 2;
    }
    
    i = 0;
    while (latinwindows1[i] != 0) {
        latinwindowscodes[latinwindows1[i]] = latinwindows1[i + 1];
        codeslatin[latinwindows1[i + 1]] = latinwindows1[i];
        c_unicode_to_utf8(latinwindows1[i + 1], (uchar*)ch);
        utf8codes[ch] = latinwindows1[i];
        i += 2;
    }
    
    i = 0;
    while (latincodetable[i] != 0) {
        latincodes[latincodetable[i]] = latincodetable[i + 1];
        codeslatin[latincodetable[i + 1]] = latincodetable[i];
        c_unicode_to_utf8(latincodetable[i + 1], (uchar*)ch);
        utf8codes[ch] = latincodetable[i];
        i += 2;
    }
}

//-----------------------------------------------------------------------------------------
static hmap<TAMGUCHAR, string> emojis;
static hmap<TAMGUCHAR, bool> emojiscomplement;

/*
 Some explanations:
 
 Emojis characters can be simply one unicode character such as: 😀 (0x1F600) or a sequence of unicode characters: 👋🏼 (0x1F44B, 0x1F3FC)
 We keep our emojis in two tables:
 One table with the emoji code associated to its definition in English. If the emoji is a sequence, then this code is the first element of the sequence
 Another table with the unicode characters that can complement an emoji into a sequence of codes
 */

static const char* temojis[] = { "0x1F004","mahjong red dragon","0x1F0CF","joker","0x1F170","A button (blood type)","0x1F171","B button (blood type)","0x1F17E","O button (blood type)",
    "0x1F17F","P button","0x1F18E","AB button (blood type)","0x1F191","CL button","0x1F192","COOL button","0x1F193","FREE button",
    "0x1F194","ID button","0x1F195","NEW button","0x1F196","NG button","0x1F197","OK button","0x1F198","SOS button",
    "0x1F199","UP! button","0x1F19A","VS button","0x1F1E6","flag: Ascension Island","0x1F1E7","flag: Bosnia & Herzegovina","0x1F1E8","flag: Canada",
    "0x1F1E9","flag: Germany","0x1F1EA","flag: Ceuta & Melilla","0x1F1EB","flag: Finland","0x1F1EC","flag: Gabon","0x1F1ED","flag: Hong Kong SAR China",
    "0x1F1EE","flag: Canary Islands","0x1F1EF","flag: Jersey","0x1F1F0","flag: Kenya","0x1F1F1","flag: Laos","0x1F1F2","flag: Morocco",
    "0x1F1F3","flag: Namibia","0x1F1F4","flag: Oman","0x1F1F5","flag: Panama","0x1F1F6","flag: Qatar","0x1F1F7","flag: Réunion",
    "0x1F1F8","flag: Saudi Arabia","0x1F1F9","flag: Tristan da Cunha","0x1F1FA","flag: Ukraine","0x1F1FB","flag: Vatican City","0x1F1FC","flag: Wallis & Futuna",
    "0x1F1FD","flag: Kosovo","0x1F1FE","flag: Yemen","0x1F1FF","flag: South Africa","0x1F201","Japanese here button","0x1F202","Japanese service charge button",
    "0x1F21A","Japanese free of charge button","0x1F22F","Japanese reserved button","0x1F232","Japanese prohibited button","0x1F233","Japanese vacancy button","0x1F234","Japanese passing grade button",
    "0x1F235","Japanese no vacancy button","0x1F236","Japanese not free of charge button","0x1F237","Japanese monthly amount button","0x1F238","Japanese application button","0x1F239","Japanese discount button",
    "0x1F23A","Japanese open for business button","0x1F250","Japanese bargain button","0x1F251","Japanese acceptable button","0x1F300","cyclone","0x1F301","foggy",
    "0x1F302","closed umbrella","0x1F303","night with stars","0x1F304","sunrise over mountains","0x1F305","sunrise","0x1F306","cityscape at dusk",
    "0x1F307","sunset","0x1F308","rainbow","0x1F309","bridge at night","0x1F30A","water wave","0x1F30B","volcano",
    "0x1F30C","milky way","0x1F30D","globe showing Europe-Africa","0x1F30E","globe showing Americas","0x1F30F","globe showing Asia-Australia","0x1F310","globe with meridians",
    "0x1F311","new moon","0x1F312","waxing crescent moon","0x1F313","first quarter moon","0x1F314","waxing gibbous moon","0x1F315","full moon",
    "0x1F316","waning gibbous moon","0x1F317","last quarter moon","0x1F318","waning crescent moon","0x1F319","crescent moon","0x1F31A","new moon face",
    "0x1F31B","first quarter moon face","0x1F31C","last quarter moon face","0x1F31D","full moon face","0x1F31E","sun with face","0x1F31F","glowing star",
    "0x1F320","shooting star","0x1F321","thermometer","0x1F324","sun behind small cloud","0x1F325","sun behind large cloud","0x1F326","sun behind rain cloud",
    "0x1F327","cloud with rain","0x1F328","cloud with snow","0x1F329","cloud with lightning","0x1F32A","tornado","0x1F32B","fog",
    "0x1F32C","wind face","0x1F32D","hot dog","0x1F32E","taco","0x1F32F","burrito","0x1F330","chestnut",
    "0x1F331","seedling","0x1F332","evergreen tree","0x1F333","deciduous tree","0x1F334","palm tree","0x1F335","cactus",
    "0x1F336","hot pepper","0x1F337","tulip","0x1F338","cherry blossom","0x1F339","rose","0x1F33A","hibiscus",
    "0x1F33B","sunflower","0x1F33C","blossom","0x1F33D","ear of corn","0x1F33E","sheaf of rice","0x1F33F","herb",
    "0x1F340","four leaf clover","0x1F341","maple leaf","0x1F342","fallen leaf","0x1F343","leaf fluttering in wind","0x1F344","mushroom",
    "0x1F345","tomato","0x1F346","eggplant","0x1F347","grapes","0x1F348","melon","0x1F349","watermelon",
    "0x1F34A","tangerine","0x1F34B","lemon","0x1F34C","banana","0x1F34D","pineapple","0x1F34E","red apple",
    "0x1F34F","green apple","0x1F350","pear","0x1F351","peach","0x1F352","cherries","0x1F353","strawberry",
    "0x1F354","hamburger","0x1F355","pizza","0x1F356","meat on bone","0x1F357","poultry leg","0x1F358","rice cracker",
    "0x1F359","rice ball","0x1F35A","cooked rice","0x1F35B","curry rice","0x1F35C","steaming bowl","0x1F35D","spaghetti",
    "0x1F35E","bread","0x1F35F","french fries","0x1F360","roasted sweet potato","0x1F361","dango","0x1F362","oden",
    "0x1F363","sushi","0x1F364","fried shrimp","0x1F365","fish cake with swirl","0x1F366","soft ice cream","0x1F367","shaved ice",
    "0x1F368","ice cream","0x1F369","doughnut","0x1F36A","cookie","0x1F36B","chocolate bar","0x1F36C","candy",
    "0x1F36D","lollipop","0x1F36E","custard","0x1F36F","honey pot","0x1F370","shortcake","0x1F371","bento box",
    "0x1F372","pot of food","0x1F373","cooking","0x1F374","fork and knife","0x1F375","teacup without handle","0x1F376","sake",
    "0x1F377","wine glass","0x1F378","cocktail glass","0x1F379","tropical drink","0x1F37A","beer mug","0x1F37B","clinking beer mugs",
    "0x1F37C","baby bottle","0x1F37D","fork and knife with plate","0x1F37E","bottle with popping cork","0x1F37F","popcorn","0x1F380","ribbon",
    "0x1F381","wrapped gift","0x1F382","birthday cake","0x1F383","jack-o-lantern","0x1F384","Christmas tree","0x1F385","Santa Claus",
    "0x1F386","fireworks","0x1F387","sparkler","0x1F388","balloon","0x1F389","party popper","0x1F38A","confetti ball",
    "0x1F38B","tanabata tree","0x1F38C","crossed flags","0x1F38D","pine decoration","0x1F38E","Japanese dolls","0x1F38F","carp streamer",
    "0x1F390","wind chime","0x1F391","moon viewing ceremony","0x1F392","backpack","0x1F393","graduation cap","0x1F396","military medal",
    "0x1F397","reminder ribbon","0x1F399","studio microphone","0x1F39A","level slider","0x1F39B","control knobs","0x1F39E","film frames",
    "0x1F39F","admission tickets","0x1F3A0","carousel horse","0x1F3A1","ferris wheel","0x1F3A2","roller coaster","0x1F3A3","fishing pole",
    "0x1F3A4","microphone","0x1F3A5","movie camera","0x1F3A6","cinema","0x1F3A7","headphone","0x1F3A8","artist palette",
    "0x1F3A9","top hat","0x1F3AA","circus tent","0x1F3AB","ticket","0x1F3AC","clapper board","0x1F3AD","performing arts",
    "0x1F3AE","video game","0x1F3AF","direct hit","0x1F3B0","slot machine","0x1F3B1","pool 8 ball","0x1F3B2","game die",
    "0x1F3B3","bowling","0x1F3B4","flower playing cards","0x1F3B5","musical note","0x1F3B6","musical notes","0x1F3B7","saxophone",
    "0x1F3B8","guitar","0x1F3B9","musical keyboard","0x1F3BA","trumpet","0x1F3BB","violin","0x1F3BC","musical score",
    "0x1F3BD","running shirt","0x1F3BE","tennis","0x1F3BF","skis","0x1F3C0","basketball","0x1F3C1","chequered flag",
    "0x1F3C2","snowboarder","0x1F3C3","person running","0x1F3C4","person surfing","0x1F3C5","sports medal","0x1F3C6","trophy",
    "0x1F3C7","horse racing","0x1F3C8","american football","0x1F3C9","rugby football","0x1F3CA","person swimming","0x1F3CB","person lifting weights",
    "0x1F3CC","person golfing","0x1F3CD","motorcycle","0x1F3CE","racing car","0x1F3CF","cricket game","0x1F3D0","volleyball",
    "0x1F3D1","field hockey","0x1F3D2","ice hockey","0x1F3D3","ping pong","0x1F3D4","snow-capped mountain","0x1F3D5","camping",
    "0x1F3D6","beach with umbrella","0x1F3D7","building construction","0x1F3D8","houses","0x1F3D9","cityscape","0x1F3DA","derelict house",
    "0x1F3DB","classical building","0x1F3DC","desert","0x1F3DD","desert island","0x1F3DE","national park","0x1F3DF","stadium",
    "0x1F3E0","house","0x1F3E1","house with garden","0x1F3E2","office building","0x1F3E3","Japanese post office","0x1F3E4","post office",
    "0x1F3E5","hospital","0x1F3E6","bank","0x1F3E7","ATM sign","0x1F3E8","hotel","0x1F3E9","love hotel",
    "0x1F3EA","convenience store","0x1F3EB","school","0x1F3EC","department store","0x1F3ED","factory","0x1F3EE","red paper lantern",
    "0x1F3EF","Japanese castle","0x1F3F0","castle","0x1F3F3","white flag","0x1F3F4","black flag","0x1F3F5","rosette",
    "0x1F3F7","label","0x1F3F8","badminton","0x1F3F9","bow and arrow","0x1F3FA","amphora","0x1F3FB","light skin tone",
    "0x1F3FC","medium-light skin tone","0x1F3FD","medium skin tone","0x1F3FE","medium-dark skin tone","0x1F3FF","dark skin tone","0x1F400","rat",
    "0x1F401","mouse","0x1F402","ox","0x1F403","water buffalo","0x1F404","cow","0x1F405","tiger",
    "0x1F406","leopard","0x1F407","rabbit","0x1F408","cat","0x1F409","dragon","0x1F40A","crocodile",
    "0x1F40B","whale","0x1F40C","snail","0x1F40D","snake","0x1F40E","horse","0x1F40F","ram",
    "0x1F410","goat","0x1F411","ewe","0x1F412","monkey","0x1F413","rooster","0x1F414","chicken",
    "0x1F415","dog","0x1F416","pig","0x1F417","boar","0x1F418","elephant","0x1F419","octopus",
    "0x1F41A","spiral shell","0x1F41B","bug","0x1F41C","ant","0x1F41D","honeybee","0x1F41E","lady beetle",
    "0x1F41F","fish","0x1F420","tropical fish","0x1F421","blowfish","0x1F422","turtle","0x1F423","hatching chick",
    "0x1F424","baby chick","0x1F425","front-facing baby chick","0x1F426","bird","0x1F427","penguin","0x1F428","koala",
    "0x1F429","poodle","0x1F42A","camel","0x1F42B","two-hump camel","0x1F42C","dolphin","0x1F42D","mouse face",
    "0x1F42E","cow face","0x1F42F","tiger face","0x1F430","rabbit face","0x1F431","cat face","0x1F432","dragon face",
    "0x1F433","spouting whale","0x1F434","horse face","0x1F435","monkey face","0x1F436","dog face","0x1F437","pig face",
    "0x1F438","frog","0x1F439","hamster","0x1F43A","wolf","0x1F43B","bear","0x1F43C","panda",
    "0x1F43D","pig nose","0x1F43E","paw prints","0x1F43F","chipmunk","0x1F440","eyes","0x1F441","eye in speech bubble",
    "0x1F442","ear","0x1F443","nose","0x1F444","mouth","0x1F445","tongue","0x1F446","backhand index pointing up",
    "0x1F447","backhand index pointing down","0x1F448","backhand index pointing left","0x1F449","backhand index pointing right","0x1F44A","oncoming fist","0x1F44B","waving hand",
    "0x1F44C","OK hand","0x1F44D","thumbs up","0x1F44E","thumbs down","0x1F44F","clapping hands","0x1F450","open hands",
    "0x1F451","crown","0x1F452","womans hat","0x1F453","glasses","0x1F454","necktie","0x1F455","t-shirt",
    "0x1F456","jeans","0x1F457","dress","0x1F458","kimono","0x1F459","bikini","0x1F45A","womans clothes",
    "0x1F45B","purse","0x1F45C","handbag","0x1F45D","clutch bag","0x1F45E","mans shoe","0x1F45F","running shoe",
    "0x1F460","high-heeled shoe","0x1F461","womans sandal","0x1F462","womans boot","0x1F463","footprints","0x1F464","bust in silhouette",
    "0x1F465","busts in silhouette","0x1F466","boy","0x1F467","girl","0x1F468","man","0x1F469","woman",
    "0x1F46A","family","0x1F46B","woman and man holding hands","0x1F46C","men holding hands","0x1F46D","women holding hands","0x1F46E","police officer",
    "0x1F46F","people with bunny ears","0x1F470","bride with veil","0x1F471","person: blond hair","0x1F472","man with Chinese cap","0x1F473","person wearing turban",
    "0x1F474","old man","0x1F475","old woman","0x1F476","baby","0x1F477","construction worker","0x1F478","princess",
    "0x1F479","ogre","0x1F47A","goblin","0x1F47B","ghost","0x1F47C","baby angel","0x1F47D","alien",
    "0x1F47E","alien monster","0x1F47F","angry face with horns","0x1F480","skull","0x1F481","person tipping hand","0x1F482","guard",
    "0x1F483","woman dancing","0x1F484","lipstick","0x1F485","nail polish","0x1F486","person getting massage","0x1F487","person getting haircut",
    "0x1F488","barber pole","0x1F489","syringe","0x1F48A","pill","0x1F48B","kiss mark","0x1F48C","love letter",
    "0x1F48D","ring","0x1F48E","gem stone","0x1F48F","kiss","0x1F490","bouquet","0x1F491","couple with heart",
    "0x1F492","wedding","0x1F493","beating heart","0x1F494","broken heart","0x1F495","two hearts","0x1F496","sparkling heart",
    "0x1F497","growing heart","0x1F498","heart with arrow","0x1F499","blue heart","0x1F49A","green heart","0x1F49B","yellow heart",
    "0x1F49C","purple heart","0x1F49D","heart with ribbon","0x1F49E","revolving hearts","0x1F49F","heart decoration","0x1F4A0","diamond with a dot",
    "0x1F4A1","light bulb","0x1F4A2","anger symbol","0x1F4A3","bomb","0x1F4A4","zzz","0x1F4A5","collision",
    "0x1F4A6","sweat droplets","0x1F4A7","droplet","0x1F4A8","dashing away","0x1F4A9","pile of poo","0x1F4AA","flexed biceps",
    "0x1F4AB","dizzy","0x1F4AC","speech balloon","0x1F4AD","thought balloon","0x1F4AE","white flower","0x1F4AF","hundred points",
    "0x1F4B0","money bag","0x1F4B1","currency exchange","0x1F4B2","heavy dollar sign","0x1F4B3","credit card","0x1F4B4","yen banknote",
    "0x1F4B5","dollar banknote","0x1F4B6","euro banknote","0x1F4B7","pound banknote","0x1F4B8","money with wings","0x1F4B9","chart increasing with yen",
    "0x1F4BA","seat","0x1F4BB","laptop computer","0x1F4BC","briefcase","0x1F4BD","computer disk","0x1F4BE","floppy disk",
    "0x1F4BF","optical disk","0x1F4C0","dvd","0x1F4C1","file folder","0x1F4C2","open file folder","0x1F4C3","page with curl",
    "0x1F4C4","page facing up","0x1F4C5","calendar","0x1F4C6","tear-off calendar","0x1F4C7","card index","0x1F4C8","chart increasing",
    "0x1F4C9","chart decreasing","0x1F4CA","bar chart","0x1F4CB","clipboard","0x1F4CC","pushpin","0x1F4CD","round pushpin",
    "0x1F4CE","paperclip","0x1F4CF","straight ruler","0x1F4D0","triangular ruler","0x1F4D1","bookmark tabs","0x1F4D2","ledger",
    "0x1F4D3","notebook","0x1F4D4","notebook with decorative cover","0x1F4D5","closed book","0x1F4D6","open book","0x1F4D7","green book",
    "0x1F4D8","blue book","0x1F4D9","orange book","0x1F4DA","books","0x1F4DB","name badge","0x1F4DC","scroll",
    "0x1F4DD","memo","0x1F4DE","telephone receiver","0x1F4DF","pager","0x1F4E0","fax machine","0x1F4E1","satellite antenna",
    "0x1F4E2","loudspeaker","0x1F4E3","megaphone","0x1F4E4","outbox tray","0x1F4E5","inbox tray","0x1F4E6","package",
    "0x1F4E7","e-mail","0x1F4E8","incoming envelope","0x1F4E9","envelope with arrow","0x1F4EA","closed mailbox with lowered flag","0x1F4EB","closed mailbox with raised flag",
    "0x1F4EC","open mailbox with raised flag","0x1F4ED","open mailbox with lowered flag","0x1F4EE","postbox","0x1F4EF","postal horn","0x1F4F0","newspaper",
    "0x1F4F1","mobile phone","0x1F4F2","mobile phone with arrow","0x1F4F3","vibration mode","0x1F4F4","mobile phone off","0x1F4F5","no mobile phones",
    "0x1F4F6","antenna bars","0x1F4F7","camera","0x1F4F8","camera with flash","0x1F4F9","video camera","0x1F4FA","television",
    "0x1F4FB","radio","0x1F4FC","videocassette","0x1F4FD","film projector","0x1F4FF","prayer beads","0x1F500","shuffle tracks button",
    "0x1F501","repeat button","0x1F502","repeat single button","0x1F503","clockwise vertical arrows","0x1F504","counterclockwise arrows button","0x1F505","dim button",
    "0x1F506","bright button","0x1F507","muted speaker","0x1F508","speaker low volume","0x1F509","speaker medium volume","0x1F50A","speaker high volume",
    "0x1F50B","battery","0x1F50C","electric plug","0x1F50D","magnifying glass tilted left","0x1F50E","magnifying glass tilted right","0x1F50F","locked with pen",
    "0x1F510","locked with key","0x1F511","key","0x1F512","locked","0x1F513","unlocked","0x1F514","bell",
    "0x1F515","bell with slash","0x1F516","bookmark","0x1F517","link","0x1F518","radio button","0x1F519","BACK arrow",
    "0x1F51A","END arrow","0x1F51B","ON! arrow","0x1F51C","SOON arrow","0x1F51D","TOP arrow","0x1F51E","no one under eighteen",
    "0x1F51F","keycap: 10","0x1F520","input latin uppercase","0x1F521","input latin lowercase","0x1F522","input numbers","0x1F523","input symbols",
    "0x1F524","input latin letters","0x1F525","fire","0x1F526","flashlight","0x1F527","wrench","0x1F528","hammer",
    "0x1F529","nut and bolt","0x1F52A","kitchen knife","0x1F52B","pistol","0x1F52C","microscope","0x1F52D","telescope",
    "0x1F52E","crystal ball","0x1F52F","dotted six-pointed star","0x1F530","Japanese symbol for beginner","0x1F531","trident emblem","0x1F532","black square button",
    "0x1F533","white square button","0x1F534","red circle","0x1F535","blue circle","0x1F536","large orange diamond","0x1F537","large blue diamond",
    "0x1F538","small orange diamond","0x1F539","small blue diamond","0x1F53A","red triangle pointed up","0x1F53B","red triangle pointed down","0x1F53C","upwards button",
    "0x1F53D","downwards button","0x1F549","om","0x1F54A","dove","0x1F54B","kaaba","0x1F54C","mosque",
    "0x1F54D","synagogue","0x1F54E","menorah","0x1F550","one oclock","0x1F551","two oclock","0x1F552","three oclock",
    "0x1F553","four oclock","0x1F554","five oclock","0x1F555","six oclock","0x1F556","seven oclock","0x1F557","eight oclock",
    "0x1F558","nine oclock","0x1F559","ten oclock","0x1F55A","eleven oclock","0x1F55B","twelve oclock","0x1F55C","one-thirty",
    "0x1F55D","two-thirty","0x1F55E","three-thirty","0x1F55F","four-thirty","0x1F560","five-thirty","0x1F561","six-thirty",
    "0x1F562","seven-thirty","0x1F563","eight-thirty","0x1F564","nine-thirty","0x1F565","ten-thirty","0x1F566","eleven-thirty",
    "0x1F567","twelve-thirty","0x1F56F","candle","0x1F570","mantelpiece clock","0x1F573","hole","0x1F574","man in suit levitating",
    "0x1F575","detective","0x1F576","sunglasses","0x1F577","spider","0x1F578","spider web","0x1F579","joystick",
    "0x1F57A","man dancing","0x1F587","linked paperclips","0x1F58A","pen","0x1F58B","fountain pen","0x1F58C","paintbrush",
    "0x1F58D","crayon","0x1F590","hand with fingers splayed","0x1F595","middle finger","0x1F596","vulcan salute","0x1F5A4","black heart",
    "0x1F5A5","desktop computer","0x1F5A8","printer","0x1F5B1","computer mouse","0x1F5B2","trackball","0x1F5BC","framed picture",
    "0x1F5C2","card index dividers","0x1F5C3","card file box","0x1F5C4","file cabinet","0x1F5D1","wastebasket","0x1F5D2","spiral notepad",
    "0x1F5D3","spiral calendar","0x1F5DC","clamp","0x1F5DD","old key","0x1F5DE","rolled-up newspaper","0x1F5E1","dagger",
    "0x1F5E3","speaking head","0x1F5E8","left speech bubble","0x1F5EF","right anger bubble","0x1F5F3","ballot box with ballot","0x1F5FA","world map",
    "0x1F5FB","mount fuji","0x1F5FC","Tokyo tower","0x1F5FD","Statue of Liberty","0x1F5FE","map of Japan","0x1F5FF","moai",
    "0x1F600","grinning face","0x1F601","beaming face with smiling eyes","0x1F602","face with tears of joy","0x1F603","grinning face with big eyes","0x1F604","grinning face with smiling eyes",
    "0x1F605","grinning face with sweat","0x1F606","grinning squinting face","0x1F607","smiling face with halo","0x1F608","smiling face with horns","0x1F609","winking face",
    "0x1F60A","smiling face with smiling eyes","0x1F60B","face savoring food","0x1F60C","relieved face","0x1F60D","smiling face with heart-eyes","0x1F60E","smiling face with sunglasses",
    "0x1F60F","smirking face","0x1F610","neutral face","0x1F611","expressionless face","0x1F612","unamused face","0x1F613","downcast face with sweat",
    "0x1F614","pensive face","0x1F615","confused face","0x1F616","confounded face","0x1F617","kissing face","0x1F618","face blowing a kiss",
    "0x1F619","kissing face with smiling eyes","0x1F61A","kissing face with closed eyes","0x1F61B","face with tongue","0x1F61C","winking face with tongue","0x1F61D","squinting face with tongue",
    "0x1F61E","disappointed face","0x1F61F","worried face","0x1F620","angry face","0x1F621","pouting face","0x1F622","crying face",
    "0x1F623","persevering face","0x1F624","face with steam from nose","0x1F625","sad but relieved face","0x1F626","frowning face with open mouth","0x1F627","anguished face",
    "0x1F628","fearful face","0x1F629","weary face","0x1F62A","sleepy face","0x1F62B","tired face","0x1F62C","grimacing face",
    "0x1F62D","loudly crying face","0x1F62E","face with open mouth","0x1F62F","hushed face","0x1F630","anxious face with sweat","0x1F631","face screaming in fear",
    "0x1F632","astonished face","0x1F633","flushed face","0x1F634","sleeping face","0x1F635","dizzy face","0x1F636","face without mouth",
    "0x1F637","face with medical mask","0x1F638","grinning cat with smiling eyes","0x1F639","cat with tears of joy","0x1F63A","grinning cat","0x1F63B","smiling cat with heart-eyes",
    "0x1F63C","cat with wry smile","0x1F63D","kissing cat","0x1F63E","pouting cat","0x1F63F","crying cat","0x1F640","weary cat",
    "0x1F641","slightly frowning face","0x1F642","slightly smiling face","0x1F643","upside-down face","0x1F644","face with rolling eyes","0x1F645","person gesturing NO",
    "0x1F646","person gesturing OK","0x1F647","person bowing","0x1F648","see-no-evil monkey","0x1F649","hear-no-evil monkey","0x1F64A","speak-no-evil monkey",
    "0x1F64B","person raising hand","0x1F64C","raising hands","0x1F64D","person frowning","0x1F64E","person pouting","0x1F64F","folded hands",
    "0x1F680","rocket","0x1F681","helicopter","0x1F682","locomotive","0x1F683","railway car","0x1F684","high-speed train",
    "0x1F685","bullet train","0x1F686","train","0x1F687","metro","0x1F688","light rail","0x1F689","station",
    "0x1F68A","tram","0x1F68B","tram car","0x1F68C","bus","0x1F68D","oncoming bus","0x1F68E","trolleybus",
    "0x1F68F","bus stop","0x1F690","minibus","0x1F691","ambulance","0x1F692","fire engine","0x1F693","police car",
    "0x1F694","oncoming police car","0x1F695","taxi","0x1F696","oncoming taxi","0x1F697","automobile","0x1F698","oncoming automobile",
    "0x1F699","sport utility vehicle","0x1F69A","delivery truck","0x1F69B","articulated lorry","0x1F69C","tractor","0x1F69D","monorail",
    "0x1F69E","mountain railway","0x1F69F","suspension railway","0x1F6A0","mountain cableway","0x1F6A1","aerial tramway","0x1F6A2","ship",
    "0x1F6A3","person rowing boat","0x1F6A4","speedboat","0x1F6A5","horizontal traffic light","0x1F6A6","vertical traffic light","0x1F6A7","construction",
    "0x1F6A8","police car light","0x1F6A9","triangular flag","0x1F6AA","door","0x1F6AB","prohibited","0x1F6AC","cigarette",
    "0x1F6AD","no smoking","0x1F6AE","litter in bin sign","0x1F6AF","no littering","0x1F6B0","potable water","0x1F6B1","non-potable water",
    "0x1F6B2","bicycle","0x1F6B3","no bicycles","0x1F6B4","person biking","0x1F6B5","person mountain biking","0x1F6B6","person walking",
    "0x1F6B7","no pedestrians","0x1F6B8","children crossing","0x1F6B9","mens room","0x1F6BA","womens room","0x1F6BB","restroom",
    "0x1F6BC","baby symbol","0x1F6BD","toilet","0x1F6BE","water closet","0x1F6BF","shower","0x1F6C0","person taking bath",
    "0x1F6C1","bathtub","0x1F6C2","passport control","0x1F6C3","customs","0x1F6C4","baggage claim","0x1F6C5","left luggage",
    "0x1F6CB","couch and lamp","0x1F6CC","person in bed","0x1F6CD","shopping bags","0x1F6CE","bellhop bell","0x1F6CF","bed",
    "0x1F6D0","place of worship","0x1F6D1","stop sign","0x1F6D2","shopping cart","0x1F6D5","hindu temple","0x1F6E0","hammer and wrench",
    "0x1F6E1","shield","0x1F6E2","oil drum","0x1F6E3","motorway","0x1F6E4","railway track","0x1F6E5","motor boat",
    "0x1F6E9","small airplane","0x1F6EB","airplane departure","0x1F6EC","airplane arrival","0x1F6F0","satellite","0x1F6F3","passenger ship",
    "0x1F6F4","kick scooter","0x1F6F5","motor scooter","0x1F6F6","canoe","0x1F6F7","sled","0x1F6F8","flying saucer",
    "0x1F6F9","skateboard","0x1F6FA","auto rickshaw","0x1F7E0","orange circle","0x1F7E1","yellow circle","0x1F7E2","green circle",
    "0x1F7E3","purple circle","0x1F7E4","brown circle","0x1F7E5","red square","0x1F7E6","blue square","0x1F7E7","orange square",
    "0x1F7E8","yellow square","0x1F7E9","green square","0x1F7EA","purple square","0x1F7EB","brown square","0x1F90D","white heart",
    "0x1F90E","brown heart","0x1F90F","pinching hand","0x1F910","zipper-mouth face","0x1F911","money-mouth face","0x1F912","face with thermometer",
    "0x1F913","nerd face","0x1F914","thinking face","0x1F915","face with head-bandage","0x1F916","robot","0x1F917","hugging face",
    "0x1F918","sign of the horns","0x1F919","call me hand","0x1F91A","raised back of hand","0x1F91B","left-facing fist","0x1F91C","right-facing fist",
    "0x1F91D","handshake","0x1F91E","crossed fingers","0x1F91F","love-you gesture","0x1F920","cowboy hat face","0x1F921","clown face",
    "0x1F922","nauseated face","0x1F923","rolling on the floor laughing","0x1F924","drooling face","0x1F925","lying face","0x1F926","person facepalming",
    "0x1F927","sneezing face","0x1F928","face with raised eyebrow","0x1F929","star-struck","0x1F92A","zany face","0x1F92B","shushing face",
    "0x1F92C","face with symbols on mouth","0x1F92D","face with hand over mouth","0x1F92E","face vomiting","0x1F92F","exploding head","0x1F930","pregnant woman",
    "0x1F931","breast-feeding","0x1F932","palms up together","0x1F933","selfie","0x1F934","prince","0x1F935","man in tuxedo",
    "0x1F936","Mrs. Claus","0x1F937","person shrugging","0x1F938","person cartwheeling","0x1F939","person juggling","0x1F93A","person fencing",
    "0x1F93C","people wrestling","0x1F93D","person playing water polo","0x1F93E","person playing handball","0x1F93F","diving mask","0x1F940","wilted flower",
    "0x1F941","drum","0x1F942","clinking glasses","0x1F943","tumbler glass","0x1F944","spoon","0x1F945","goal net",
    "0x1F947","1st place medal","0x1F948","2nd place medal","0x1F949","3rd place medal","0x1F94A","boxing glove","0x1F94B","martial arts uniform",
    "0x1F94C","curling stone","0x1F94D","lacrosse","0x1F94E","softball","0x1F94F","flying disc","0x1F950","croissant",
    "0x1F951","avocado","0x1F952","cucumber","0x1F953","bacon","0x1F954","potato","0x1F955","carrot",
    "0x1F956","baguette bread","0x1F957","green salad","0x1F958","shallow pan of food","0x1F959","stuffed flatbread","0x1F95A","egg",
    "0x1F95B","glass of milk","0x1F95C","peanuts","0x1F95D","kiwi fruit","0x1F95E","pancakes","0x1F95F","dumpling",
    "0x1F960","fortune cookie","0x1F961","takeout box","0x1F962","chopsticks","0x1F963","bowl with spoon","0x1F964","cup with straw",
    "0x1F965","coconut","0x1F966","broccoli","0x1F967","pie","0x1F968","pretzel","0x1F969","cut of meat",
    "0x1F96A","sandwich","0x1F96B","canned food","0x1F96C","leafy green","0x1F96D","mango","0x1F96E","moon cake",
    "0x1F96F","bagel","0x1F970","smiling face with hearts","0x1F971","yawning face","0x1F973","partying face","0x1F974","woozy face",
    "0x1F975","hot face","0x1F976","cold face","0x1F97A","pleading face","0x1F97B","sari","0x1F97C","lab coat",
    "0x1F97D","goggles","0x1F97E","hiking boot","0x1F97F","flat shoe","0x1F980","crab","0x1F981","lion",
    "0x1F982","scorpion","0x1F983","turkey","0x1F984","unicorn","0x1F985","eagle","0x1F986","duck",
    "0x1F987","bat","0x1F988","shark","0x1F989","owl","0x1F98A","fox","0x1F98B","butterfly",
    "0x1F98C","deer","0x1F98D","gorilla","0x1F98E","lizard","0x1F98F","rhinoceros","0x1F990","shrimp",
    "0x1F991","squid","0x1F992","giraffe","0x1F993","zebra","0x1F994","hedgehog","0x1F995","sauropod",
    "0x1F996","T-Rex","0x1F997","cricket","0x1F998","kangaroo","0x1F999","llama","0x1F99A","peacock",
    "0x1F99B","hippopotamus","0x1F99C","parrot","0x1F99D","raccoon","0x1F99E","lobster","0x1F99F","mosquito",
    "0x1F9A0","microbe","0x1F9A1","badger","0x1F9A2","swan","0x1F9A5","sloth","0x1F9A6","otter",
    "0x1F9A7","orangutan","0x1F9A8","skunk","0x1F9A9","flamingo","0x1F9AA","oyster","0x1F9AE","guide dog",
    "0x1F9AF","probing cane","0x1F9B0","red hair","0x1F9B1","curly hair","0x1F9B2","bald","0x1F9B3","white hair",
    "0x1F9B4","bone","0x1F9B5","leg","0x1F9B6","foot","0x1F9B7","tooth","0x1F9B8","superhero",
    "0x1F9B9","supervillain","0x1F9BA","safety vest","0x1F9BB","ear with hearing aid","0x1F9BC","motorized wheelchair","0x1F9BD","manual wheelchair",
    "0x1F9BE","mechanical arm","0x1F9BF","mechanical leg","0x1F9C0","cheese wedge","0x1F9C1","cupcake","0x1F9C2","salt",
    "0x1F9C3","beverage box","0x1F9C4","garlic","0x1F9C5","onion","0x1F9C6","falafel","0x1F9C7","waffle",
    "0x1F9C8","butter","0x1F9C9","mate","0x1F9CA","ice cube","0x1F9CD","person standing","0x1F9CE","person kneeling",
    "0x1F9CF","deaf person","0x1F9D0","face with monocle","0x1F9D1","person","0x1F9D2","child","0x1F9D3","older person",
    "0x1F9D4","man: beard","0x1F9D5","woman with headscarf","0x1F9D6","person in steamy room","0x1F9D7","person climbing","0x1F9D8","person in lotus position",
    "0x1F9D9","mage","0x1F9DA","fairy","0x1F9DB","vampire","0x1F9DC","merperson","0x1F9DD","elf",
    "0x1F9DE","genie","0x1F9DF","zombie","0x1F9E0","brain","0x1F9E1","orange heart","0x1F9E2","billed cap",
    "0x1F9E3","scarf","0x1F9E4","gloves","0x1F9E5","coat","0x1F9E6","socks","0x1F9E7","red envelope",
    "0x1F9E8","firecracker","0x1F9E9","puzzle piece","0x1F9EA","test tube","0x1F9EB","petri dish","0x1F9EC","dna",
    "0x1F9ED","compass","0x1F9EE","abacus","0x1F9EF","fire extinguisher","0x1F9F0","toolbox","0x1F9F1","brick",
    "0x1F9F2","magnet","0x1F9F3","luggage","0x1F9F4","lotion bottle","0x1F9F5","thread","0x1F9F6","yarn",
    "0x1F9F7","safety pin","0x1F9F8","teddy bear","0x1F9F9","broom","0x1F9FA","basket","0x1F9FB","roll of paper",
    "0x1F9FC","soap","0x1F9FD","sponge","0x1F9FE","receipt","0x1F9FF","nazar amulet","0x1FA70","ballet shoes",
    "0x1FA71","one-piece swimsuit","0x1FA72","swim brief","0x1FA73","shorts","0x1FA78","drop of blood","0x1FA79","adhesive bandage",
    "0x1FA7A","stethoscope","0x1FA80","yo-yo","0x1FA81","kite","0x1FA82","parachute","0x1FA90","ringed planet",
    "0x1FA91","chair","0x1FA92","razor","0x1FA93","axe","0x1FA94","diya lamp","0x1FA95","banjo",
    "0x203C","double exclamation mark","0x2049","exclamation question mark","0x2122","trade mark","0x2139","information","0x2194","left-right arrow",
    "0x2195","up-down arrow","0x2196","up-left arrow","0x2197","up-right arrow","0x2198","down-right arrow","0x2199","down-left arrow",
    "0x21A9","right arrow curving left","0x21AA","left arrow curving right","0x231A","watch","0x231B","hourglass done","0x2328","keyboard",
    "0x23CF","eject button","0x23E9","fast-forward button","0x23EA","fast reverse button","0x23EB","fast up button","0x23EC","fast down button",
    "0x23ED","next track button","0x23EE","last track button","0x23EF","play or pause button","0x23F0","alarm clock","0x23F1","stopwatch",
    "0x23F2","timer clock","0x23F3","hourglass not done","0x23F8","pause button","0x23F9","stop button","0x23FA","record button",
    "0x24C2","circled M","0x25AA","black small square","0x25AB","white small square","0x25B6","play button","0x25C0","reverse button",
    "0x25FB","white medium square","0x25FC","black medium square","0x25FD","white medium-small square","0x25FE","black medium-small square","0x2600","sun",
    "0x2601","cloud","0x2602","umbrella","0x2603","snowman","0x2604","comet","0x260E","telephone",
    "0x2611","check box with check","0x2614","umbrella with rain drops","0x2615","hot beverage","0x2618","shamrock","0x261D","index pointing up",
    "0x2620","skull and crossbones","0x2622","radioactive","0x2623","biohazard","0x2626","orthodox cross","0x262A","star and crescent",
    "0x262E","peace symbol","0x262F","yin yang","0x2638","wheel of dharma","0x2639","frowning face","0x263A","smiling face",
    "0x2640","female sign","0x2642","male sign","0x2648","Aries","0x2649","Taurus","0x264A","Gemini",
    "0x264B","Cancer","0x264C","Leo","0x264D","Virgo","0x264E","Libra","0x264F","Scorpio",
    "0x2650","Sagittarius","0x2651","Capricorn","0x2652","Aquarius","0x2653","Pisces","0x265F","chess pawn",
    "0x2660","spade suit","0x2663","club suit","0x2665","heart suit","0x2666","diamond suit","0x2668","hot springs",
    "0x267B","recycling symbol","0x267E","infinity","0x267F","wheelchair symbol","0x2692","hammer and pick","0x2693","anchor",
    "0x2694","crossed swords","0x2695","medical symbol","0x2696","balance scale","0x2697","alembic","0x2699","gear",
    "0x269B","atom symbol","0x269C","fleur-de-lis","0x26A0","warning","0x26A1","high voltage","0x26AA","white circle",
    "0x26AB","black circle","0x26B0","coffin","0x26B1","funeral urn","0x26BD","soccer ball","0x26BE","baseball",
    "0x26C4","snowman without snow","0x26C5","sun behind cloud","0x26C8","cloud with lightning and rain","0x26CE","Ophiuchus","0x26CF","pick",
    "0x26D1","rescue workers helmet","0x26D3","chains","0x26D4","no entry","0x26E9","shinto shrine","0x26EA","church",
    "0x26F0","mountain","0x26F1","umbrella on ground","0x26F2","fountain","0x26F3","flag in hole","0x26F4","ferry",
    "0x26F5","sailboat","0x26F7","skier","0x26F8","ice skate","0x26F9","person bouncing ball","0x26FA","tent",
    "0x26FD","fuel pump","0x2702","scissors","0x2705","check mark button","0x2708","airplane","0x2709","envelope",
    "0x270A","raised fist","0x270B","raised hand","0x270C","victory hand","0x270D","writing hand","0x270F","pencil",
    "0x2712","black nib","0x2714","check mark","0x2716","multiplication sign","0x271D","latin cross","0x2721","star of David",
    "0x2728","sparkles","0x2733","eight-spoked asterisk","0x2734","eight-pointed star","0x2744","snowflake","0x2747","sparkle",
    "0x274C","cross mark","0x274E","cross mark button","0x2753","question mark","0x2754","white question mark","0x2755","white exclamation mark",
    "0x2757","exclamation mark","0x2763","heart exclamation","0x2764","red heart","0x2795","plus sign","0x2796","minus sign",
    "0x2797","division sign","0x27A1","right arrow","0x27B0","curly loop","0x27BF","double curly loop","0x2934","right arrow curving up",
    "0x2935","right arrow curving down","0x2B05","left arrow","0x2B06","up arrow","0x2B07","down arrow","0x2B1B","black large square",
    "0x2B1C","white large square","0x2B50","star","0x2B55","hollow red circle","0x3030","wavy dash","0x303D","part alternation mark",
    "0x3297","Japanese congratulations button","0x3299","Japanese secret button",
    "", "" };


static TAMGUCHAR temojiscomplement[] = {0x1F1E6,0x1F1E7,0x1F1E8,0x1F1E9,0x1F1EA,0x1F1EB,0x1F1EC,0x1F1ED,0x1F1EE,0x1F1EF,
    0x1F1F0,0x1F1F1,0x1F1F2,0x1F1F3,0x1F1F4,0x1F1F5,0x1F1F6,0x1F1F7,0x1F1F8,0x1F1F9,
    0x1F1FA,0x1F1FB,0x1F1FC,0x1F1FD,0x1F1FE,0x1F1FF,0x1F308,0x1F33E,0x1F373,0x1F393,
    0x1F3A4,0x1F3A8,0x1F3EB,0x1F3ED,0x1F3FB,0x1F3FC,0x1F3FD,0x1F3FE,0x1F3FF,0x1F466,
    0x1F467,0x1F468,0x1F469,0x1F48B,0x1F4BB,0x1F4BC,0x1F527,0x1F52C,0x1F5E8,0x1F680,
    0x1F692,0x1F91D,0x1F9AF,0x1F9B0,0x1F9B1,0x1F9B2,0x1F9B3,0x1F9BA,0x1F9BC,0x1F9BD,
    0x1F9D1,0x200D,0x20E3,0x2620,0x2640,0x2642,0x2695,0x2696,0x2708,0x2764,
    0xE0062,0xE0063,0xE0065,0xE0067,0xE006C,0xE006E,0xE0073,0xE0074,0xE0077,0xE007F,
    0xFE0F, 0};

static void InitEmojis() {
    static bool init=false;
    if (init)
        return;
    init=true;
    long i = 0;
    string code;
    TAMGUCHAR c;
    while (temojis[i][0] != 0) {
        code = temojis[i];
        c = (TAMGUCHAR)conversionintegerhexa(STR(code));
        if (c > 127)
            emojis[c] = temojis[i + 1];
        i += 2;
    }
    i = 0;
    while (temojiscomplement[i]) {
        emojiscomplement[temojiscomplement[i]] = true;
        i++;
    }
}

Exporting bool c_is_emoji(TAMGUCHAR c) {
    if (emojis.find(c) != emojis.end())
        return true;
    return false;
}

Exporting bool c_is_emojicomp(TAMGUCHAR c) {
    if (emojiscomplement.find(c) != emojiscomplement.end())
        return true;
    return false;
}


Exporting bool c_is_emoji(unsigned char* m, long& i) {
    TAMGUCHAR v;
    i += c_utf8_to_unicode(m + i, v);
    return c_is_emoji(v);
}

Exporting bool c_is_emojicomp(unsigned char* m, long& i) {
    TAMGUCHAR v;
    i += c_utf8_to_unicode(m + i, v);
    return c_is_emojicomp(v);
}

Exporting bool s_is_emoji(string& s) {
    if (s == "")
        return false;
    long lg = s.size();
    bool check_comp = false;
    long p;
    for (long i = 0; i < lg; i++) {
        p = i;
        if (c_is_emoji(USTR(s), p)) {
            check_comp = true;
            i = p;
            continue;
        }
        
        if (check_comp) {
            p = i;
            if (c_is_emojicomp(USTR(s), p)) {
                i = p;
                continue;
            }
        }
        return false;
    }
    return true;
}

Exporting bool s_is_emoji(wstring& s) {
    if (s == L"")
        return false;
    long lg = s.size();
    bool check_comp = false;
    
    TAMGUCHAR c;
    
    for (long i = 0; i < lg; i++) {
#ifdef WSTRING_IS_UTF16
        if (c_utf16_to_unicode(c, s[i], false))
            c_utf16_to_unicode(c, s[++i], true);
#else
        c = s[i];
#endif
        if (c_is_emoji(c)) {
            check_comp = true;
            continue;
        }
        
        if (check_comp) {
            if (c_is_emojicomp(c))
                continue;
        }
        return false;
    }
    return true;
}

Exporting string Emoji(string& s) {
    if (s.size() == 0)
        return "";
    TAMGUCHAR c;
    c_utf8_to_unicode(USTR(s), c);
    if (emojis.find((TAMGUCHAR)c) != emojis.end())
        return emojis[c];
    return "";
}

Exporting string Emoji(wstring& s) {
    if (s.size() == 0)
        return "";
    TAMGUCHAR c = s[0];
    if (emojis.find(c) != emojis.end())
        return emojis[c];
    return "";
}

Exporting string Emoji(TAMGUCHAR c) {
    if (emojis.find(c) != emojis.end())
        return emojis[c];
    return "";
}

Exporting void l_emojis(map<BLONG, string>& dico) {
    for (auto& it : emojis)
        dico[it.first] = it.second;
}


//-----------------------------------------------------------------------------------------
bool CheckNeedConversion(string& w) {
    #ifdef INTELINTRINSICS
    long first = 0;
        //we check if we have any large characters between 0 and ipos
    return !check_ascii(USTR(w), w.size(), first);
    #endif
    return true;
}

bool CheckNeedConversion(wstring& w) {
    #ifdef INTELINTRINSICS
    long first = 0;
        //we check if we have any large characters between 0 and ipos
    return check_large_char(WSTR(w), w.size(), first);
    #endif
    return true;
}

//We only return the emoji head, when a head is present
TAMGUCHAR getechar(wstring& s, long& i) {
	TAMGUCHAR c = getachar(s, i);
	if (((c & 0x1F000) == 0x1F000) && c_is_emoji(c)) {
		long j = i + 1;
		TAMGUCHAR cc = getachar(s, j);
		while (c_is_emojicomp(cc)) {
			i = j++;
			cc = getachar(s, j);
		}
	}
	return c;
}

long conversionpostochar(wstring& w, long spos) {
    long cpos = 0;
	long realpos = 0;
	TAMGUCHAR c;
	while (realpos != spos) {
		c = getachar(w, realpos);
		if (((c & 0x1F000) == 0x1F000) && c_is_emoji(c)) {
			long j = ++realpos;
			c = getachar(w, j);
			while (c_is_emojicomp(c)) {
				realpos = ++j;
				c = getachar(w, j);
			}
		}
		else
			realpos++;
		cpos++;
	}
	return cpos;
}

void conversionpostochar(wstring& w, long& b, long& e) {
    long sbeg = b;
    long cpos = 0;
    long realpos = 0;
    TAMGUCHAR c;
    while (realpos != e) {
        if (realpos == sbeg)
            b = cpos;
        c = getachar(w, realpos);
        if (((c & 0x1F000) == 0x1F000) && c_is_emoji(c)) {
            long j = ++realpos;
            c = getachar(w, j);
            while (c_is_emojicomp(c)) {
                realpos = ++j;
                c = getachar(w, j);
            }
        }
        else
            realpos++;
        cpos++;
    }
    e = cpos;
}

//-----------------------------------------------------------------------------------------
Exporting void inittableutf8() {
    static bool init = false;
    if (init)
        return;
    init=true;
    
    InitNumbers();
    InitLatinTables();
    InitEmojis();
    
    TAMGUCHAR unicode;
    bulongchar xs;
    bulongchar xse;
    uchar latin;
    long i;
    int maxtable;
    for (maxtable = 0; codingtable[maxtable] != -1; maxtable++);
    
    
    for (i = 0; i < maxtable; i += 3) {
        unicode = codingtable[i];
		TAMGUCHAR equ = codingtable[i + 1];
        char type = codingtable[i + 2];
        xs.clear();
        xse.clear();
        c_unicode_to_utf8(unicode, xs.ustr);
        c_unicode_to_utf8(equ, xse.ustr);
        if (unicode < 256)
            latin = unicode;
        else
            latin = 0;
        
        if (type == 1) {
            utf8codemin[unicode] = equ;
            if (latin != 0)
                latinmin[latin] = xse.str;
        }
        else {
            if (type == 2) {
                utf8codemaj[unicode] = equ;
                if (latin != 0)
                    latinmaj[latin] = xse.str;
            }
        }
    }
    i = 0;
    while (ponctuations[i] != 0) {
        punctuations[ponctuations[i]] = true;
        i++;
    }
    
    
    alllatincodes[0] = &latincodes;
    alllatincodes[1] = &latincodes;
    alllatincodes[2] = &latin2codes;
    alllatincodes[3] = &latin3codes;
    alllatincodes[4] = &latin4codes;
    alllatincodes[5] = &latin5codes;
    alllatincodes[6] = &latin6codes;
    alllatincodes[7] = &latin7codes;
    alllatincodes[8] = &latin8codes;
    alllatincodes[9] = &latin9codes;
    alllatincodes[10] = &latin10codes;
    alllatincodes[11] = &latin11codes;
    alllatincodes[13] = &latin13codes;
    alllatincodes[14] = &latin14codes;
    alllatincodes[15] = &latin15codes;
    alllatincodes[16] = &latin16codes;
    alllatincodes[17] = &latinwindowscodes;
    
    consonants[349] = "s";
    consonants[231] = "c";
    consonants[199] = "C";
    consonants[208] = "D";
    consonants[331] = "n";
    consonants[122] = "z";
    consonants[292] = "H";
    consonants[241] = "n";
    consonants[209] = "N";
    consonants[88] = "X";
    consonants[262] = "C";
    consonants[263] = "c";
    consonants[86] = "V";
    consonants[264] = "C";
    consonants[87] = "W";
    consonants[265] = "c";
    consonants[84] = "T";
    consonants[266] = "C";
    consonants[267] = "c";
    consonants[82] = "R";
    consonants[268] = "C";
    consonants[83] = "S";
    consonants[269] = "c";
    consonants[353] = "s";
    consonants[80] = "P";
    consonants[270] = "D";
    consonants[352] = "S";
    consonants[81] = "Q";
    consonants[271] = "d";
    consonants[78] = "N";
    consonants[272] = "D";
    consonants[273] = "d";
    consonants[66] = "B";
    consonants[284] = "G";
    consonants[67] = "C";
    consonants[285] = "g";
    consonants[286] = "G";
    consonants[287] = "g";
    consonants[288] = "G";
    consonants[289] = "g";
    consonants[290] = "G";
    consonants[291] = "g";
    consonants[293] = "h";
    consonants[329] = "n";
    consonants[120] = "x";
    consonants[294] = "H";
    consonants[328] = "n";
    consonants[295] = "h";
    consonants[347] = "s";
    consonants[106] = "j";
    consonants[308] = "J";
    consonants[346] = "S";
    consonants[107] = "k";
    consonants[309] = "j";
    consonants[345] = "r";
    consonants[104] = "h";
    consonants[310] = "K";
    consonants[344] = "R";
    consonants[311] = "k";
    consonants[342] = "R";
    consonants[103] = "g";
    consonants[313] = "L";
    consonants[341] = "r";
    consonants[100] = "d";
    consonants[314] = "l";
    consonants[340] = "R";
    consonants[315] = "L";
    consonants[98] = "b";
    consonants[316] = "l";
    consonants[99] = "c";
    consonants[317] = "L";
    consonants[318] = "l";
    consonants[319] = "L";
    consonants[320] = "l";
    consonants[321] = "L";
    consonants[322] = "l";
    consonants[323] = "N";
    consonants[324] = "n";
    consonants[325] = "N";
    consonants[326] = "n";
    consonants[327] = "N";
    consonants[343] = "r";
    consonants[348] = "S";
    consonants[350] = "S";
    consonants[351] = "s";
    consonants[354] = "T";
    consonants[355] = "t";
    consonants[356] = "T";
    consonants[357] = "t";
    consonants[358] = "T";
    consonants[359] = "t";
    consonants[372] = "W";
    consonants[373] = "w";
    consonants[377] = "Z";
    consonants[378] = "z";
    consonants[379] = "Z";
    consonants[380] = "z";
    consonants[381] = "Z";
    consonants[382] = "z";
    consonants[384] = "b";
    consonants[385] = "B";
    consonants[386] = "b";
    consonants[387] = "b";
    consonants[388] = "b";
    consonants[389] = "b";
    consonants[391] = "C";
    consonants[392] = "c";
    consonants[393] = "D";
    consonants[394] = "D";
    consonants[395] = "d";
    consonants[396] = "d";
    consonants[401] = "F";
    consonants[402] = "f";
    consonants[403] = "G";
    consonants[408] = "K";
    consonants[409] = "k";
    consonants[452] = "DZ";
    consonants[454] = "dz";
    consonants[455] = "Ǉ";
    consonants[457] = "lj";
    consonants[458] = "NJ";
    consonants[460] = "nj";
    consonants[473] = "Ǚ";
    consonants[484] = "G";
    consonants[485] = "g";
    consonants[486] = "G";
    consonants[487] = "g";
    consonants[488] = "K";
    consonants[489] = "k";
    consonants[496] = "j";
    consonants[497] = "DZ";
    consonants[68] = "D";
    consonants[70] = "F";
    consonants[71] = "G";
    consonants[72] = "H";
    consonants[74] = "J";
    consonants[75] = "K";
    consonants[76] = "L";
    consonants[77] = "M";
    consonants[90] = "Z";
    consonants[102] = "f";
    consonants[108] = "l";
    consonants[109] = "m";
    consonants[110] = "n";
    consonants[112] = "p";
    consonants[113] = "q";
    consonants[114] = "r";
    consonants[115] = "s";
    consonants[116] = "t";
    consonants[118] = "v";
    consonants[119] = "w";
    
    
    //--------------------------------------
    vowels[200] = "E";
    vowels[277] = "e";
    vowels[192] = "A";
    vowels[201] = "E";
    vowels[276] = "E";
    vowels[193] = "A";
    vowels[73] = "I";
    vowels[279] = "e";
    vowels[194] = "A";
    vowels[278] = "E";
    vowels[195] = "A";
    vowels[196] = "A";
    vowels[197] = "A";
    vowels[275] = "e";
    vowels[198] = "AE";
    vowels[202] = "E";
    vowels[203] = "E";
    vowels[204] = "i";
    vowels[205] = "i";
    vowels[206] = "i";
    vowels[207] = "i";
    vowels[210] = "O";
    vowels[211] = "O";
    vowels[257] = "a";
    vowels[212] = "O";
    vowels[256] = "A";
    vowels[213] = "O";
    vowels[259] = "a";
    vowels[214] = "O";
    vowels[216] = "O";
    vowels[217] = "U";
    vowels[218] = "U";
    vowels[219] = "U";
    vowels[220] = "U";
    vowels[221] = "Y";
    vowels[224] = "a";
    vowels[225] = "a";
    vowels[226] = "a";
    vowels[227] = "a";
    vowels[228] = "a";
    vowels[229] = "a";
    vowels[230] = "ae";
    vowels[232] = "e";
    vowels[233] = "e";
    vowels[234] = "e";
    vowels[235] = "e";
    vowels[236] = "i";
    vowels[237] = "i";
    vowels[238] = "i";
    vowels[239] = "i";
    vowels[242] = "o";
    vowels[243] = "o";
    vowels[244] = "o";
    vowels[245] = "o";
    vowels[246] = "o";
    vowels[248] = "o";
    vowels[249] = "u";
    vowels[250] = "u";
    vowels[251] = "u";
    vowels[252] = "u";
    vowels[253] = "y";
    vowels[255] = "y";
    vowels[258] = "A";
    vowels[260] = "A";
    vowels[261] = "a";
    vowels[274] = "E";
    vowels[280] = "E";
    vowels[281] = "e";
    vowels[282] = "E";
    vowels[69] = "E";
    vowels[283] = "e";
    vowels[296] = "I";
    vowels[297] = "i";
    vowels[298] = "I";
    vowels[117] = "u";
    vowels[299] = "i";
    vowels[300] = "I";
    vowels[301] = "i";
    vowels[302] = "I";
    vowels[303] = "i";
    vowels[304] = "I";
    vowels[111] = "o";
    vowels[305] = "i";
    vowels[306] = "IJ";
    vowels[307] = "ij";
    vowels[332] = "O";
    vowels[333] = "o";
    vowels[334] = "O";
    vowels[335] = "o";
    vowels[336] = "O";
    vowels[337] = "o";
    vowels[338] = "OE";
    vowels[339] = "oe";
    vowels[360] = "U";
    vowels[361] = "u";
    vowels[362] = "U";
    vowels[363] = "u";
    vowels[364] = "U";
    vowels[365] = "u";
    vowels[366] = "U";
    vowels[367] = "u";
    vowels[368] = "U";
    vowels[369] = "u";
    vowels[370] = "U";
    vowels[371] = "u";
    vowels[374] = "Y";
    vowels[375] = "y";
    vowels[376] = "Y";
    vowels[404] = "Y";
    vowels[461] = "A";
    vowels[462] = "a";
    vowels[463] = "I";
    vowels[464] = "i";
    vowels[465] = "O";
    vowels[466] = "o";
    vowels[467] = "U";
    vowels[468] = "u";
    vowels[469] = "U";
    vowels[470] = "u";
    vowels[471] = "U";
    vowels[472] = "u";
    vowels[474] = "u";
    vowels[475] = "U";
    vowels[476] = "u";
    vowels[478] = "A";
    vowels[479] = "a";
    vowels[480] = "A";
    vowels[481] = "a";
    vowels[482] = "AE";
    vowels[483] = "ae";
    vowels[490] = "O";
    vowels[491] = "o";
    vowels[492] = "O";
    vowels[493] = "o";
    vowels[97] = "a";
    vowels[105] = "i";
    vowels[121] = "y";
    vowels[65] = "A";
    vowels[101] = "e";
    vowels[79] = "O";
    vowels[85] = "U";
    vowels[89] = "Y";
    
    
    wconsonants[208] = 68;
    wconsonants[104] = 104;
    wconsonants[264] = 67;
    wconsonants[88] = 88;
    wconsonants[349] = 115;
    wconsonants[81] = 81;
    wconsonants[241] = 110;
    wconsonants[266] = 67;
    wconsonants[231] = 99;
    wconsonants[292] = 72;
    wconsonants[199] = 67;
    wconsonants[290] = 71;
    wconsonants[209] = 78;
    wconsonants[331] = 110;
    wconsonants[291] = 103;
    wconsonants[347] = 115;
    wconsonants[486] = 71;
    wconsonants[294] = 72;
    wconsonants[98] = 98;
    wconsonants[122] = 122;
    wconsonants[77] = 77;
    wconsonants[319] = 76;
    wconsonants[262] = 67;
    wconsonants[269] = 99;
    wconsonants[357] = 116;
    wconsonants[263] = 99;
    wconsonants[112] = 112;
    wconsonants[86] = 86;
    wconsonants[103] = 103;
    wconsonants[113] = 113;
    wconsonants[87] = 87;
    wconsonants[265] = 99;
    wconsonants[308] = 74;
    wconsonants[268] = 67;
    wconsonants[284] = 71;
    wconsonants[100] = 100;
    wconsonants[84] = 84;
    wconsonants[313] = 76;
    wconsonants[379] = 90;
    wconsonants[345] = 114;
    wconsonants[267] = 99;
    wconsonants[82] = 82;
    wconsonants[83] = 83;
    wconsonants[353] = 115;
    wconsonants[80] = 80;
    wconsonants[270] = 68;
    wconsonants[352] = 83;
    wconsonants[293] = 104;
    wconsonants[78] = 78;
    wconsonants[271] = 100;
    wconsonants[288] = 71;
    wconsonants[272] = 68;
    wconsonants[273] = 100;
    wconsonants[108] = 108;
    wconsonants[66] = 66;
    wconsonants[67] = 67;
    wconsonants[285] = 103;
    wconsonants[286] = 71;
    wconsonants[341] = 114;
    wconsonants[109] = 109;
    wconsonants[309] = 106;
    wconsonants[381] = 90;
    wconsonants[287] = 103;
    wconsonants[289] = 103;
    wconsonants[329] = 110;
    wconsonants[344] = 82;
    wconsonants[120] = 120;
    wconsonants[328] = 110;
    wconsonants[295] = 104;
    wconsonants[106] = 106;
    wconsonants[314] = 108;
    wconsonants[346] = 83;
    wconsonants[107] = 107;
    wconsonants[342] = 82;
    wconsonants[310] = 75;
    wconsonants[311] = 107;
    wconsonants[340] = 82;
    wconsonants[326] = 110;
    wconsonants[315] = 76;
    wconsonants[316] = 108;
    wconsonants[99] = 99;
    wconsonants[317] = 76;
    wconsonants[318] = 108;
    wconsonants[320] = 108;
    wconsonants[355] = 116;
    wconsonants[321] = 76;
    wconsonants[322] = 108;
    wconsonants[323] = 78;
    wconsonants[324] = 110;
    wconsonants[325] = 78;
    wconsonants[327] = 78;
    wconsonants[343] = 114;
    wconsonants[348] = 83;
    wconsonants[350] = 83;
    wconsonants[351] = 115;
    wconsonants[354] = 84;
    wconsonants[356] = 84;
    wconsonants[358] = 84;
    wconsonants[359] = 116;
    wconsonants[372] = 87;
    wconsonants[373] = 119;
    wconsonants[377] = 90;
    wconsonants[378] = 122;
    wconsonants[380] = 122;
    wconsonants[382] = 122;
    wconsonants[384] = 98;
    wconsonants[385] = 66;
    wconsonants[386] = 98;
    wconsonants[387] = 98;
    wconsonants[388] = 98;
    wconsonants[389] = 98;
    wconsonants[391] = 67;
    wconsonants[392] = 99;
    wconsonants[393] = 68;
    wconsonants[488] = 75;
    wconsonants[394] = 68;
    wconsonants[395] = 100;
    wconsonants[396] = 100;
    wconsonants[401] = 70;
    wconsonants[402] = 102;
    wconsonants[403] = 71;
    wconsonants[408] = 75;
    wconsonants[409] = 107;
    wconsonants[455] = 455;
    wconsonants[473] = 473;
    wconsonants[484] = 71;
    wconsonants[485] = 103;
    wconsonants[487] = 103;
    wconsonants[489] = 107;
    wconsonants[496] = 106;
    wconsonants[90] = 90;
    wconsonants[68] = 68;
    wconsonants[70] = 70;
    wconsonants[71] = 71;
    wconsonants[72] = 72;
    wconsonants[74] = 74;
    wconsonants[115] = 115;
    wconsonants[75] = 75;
    wconsonants[76] = 76;
    wconsonants[102] = 102;
    wconsonants[110] = 110;
    wconsonants[114] = 114;
    wconsonants[116] = 116;
    wconsonants[118] = 118;
    wconsonants[119] = 119;
    
    wvowels[194] = 65;
    wvowels[200] = 69;
    wvowels[277] = 101;
    wvowels[252] = 117;
    wvowels[192] = 65;
    wvowels[243] = 111;
    wvowels[239] = 105;
    wvowels[256] = 65;
    wvowels[276] = 69;
    wvowels[201] = 69;
    wvowels[207] = 105;
    wvowels[233] = 101;
    wvowels[73] = 73;
    wvowels[193] = 65;
    wvowels[279] = 101;
    wvowels[242] = 111;
    wvowels[202] = 69;
    wvowels[275] = 101;
    wvowels[278] = 69;
    wvowels[219] = 85;
    wvowels[195] = 65;
    wvowels[196] = 65;
    wvowels[218] = 85;
    wvowels[259] = 97;
    wvowels[197] = 65;
    wvowels[258] = 65;
    wvowels[217] = 85;
    wvowels[216] = 79;
    wvowels[228] = 97;
    wvowels[203] = 69;
    wvowels[260] = 65;
    wvowels[204] = 105;
    wvowels[226] = 97;
    wvowels[210] = 79;
    wvowels[205] = 105;
    wvowels[249] = 117;
    wvowels[246] = 111;
    wvowels[225] = 97;
    wvowels[206] = 105;
    wvowels[227] = 97;
    wvowels[211] = 79;
    wvowels[257] = 97;
    wvowels[244] = 111;
    wvowels[232] = 101;
    wvowels[280] = 69;
    wvowels[212] = 79;
    wvowels[213] = 79;
    wvowels[214] = 79;
    wvowels[235] = 101;
    wvowels[220] = 85;
    wvowels[221] = 89;
    wvowels[224] = 97;
    wvowels[69] = 69;
    wvowels[253] = 121;
    wvowels[229] = 97;
    wvowels[234] = 101;
    wvowels[236] = 105;
    wvowels[237] = 105;
    wvowels[298] = 73;
    wvowels[238] = 105;
    wvowels[250] = 117;
    wvowels[245] = 111;
    wvowels[376] = 89;
    wvowels[248] = 111;
    wvowels[251] = 117;
    wvowels[261] = 97;
    wvowels[255] = 121;
    wvowels[274] = 69;
    wvowels[281] = 101;
    wvowels[282] = 69;
    wvowels[283] = 101;
    wvowels[364] = 85;
    wvowels[296] = 73;
    wvowels[297] = 105;
    wvowels[117] = 117;
    wvowels[299] = 105;
    wvowels[300] = 73;
    wvowels[301] = 105;
    wvowels[302] = 73;
    wvowels[303] = 105;
    wvowels[304] = 73;
    wvowels[111] = 111;
    wvowels[305] = 105;
    wvowels[332] = 79;
    wvowels[333] = 111;
    wvowels[334] = 79;
    wvowels[335] = 111;
    wvowels[85] = 85;
    wvowels[336] = 79;
    wvowels[337] = 111;
    wvowels[360] = 85;
    wvowels[361] = 117;
    wvowels[362] = 85;
    wvowels[363] = 117;
    wvowels[365] = 117;
    wvowels[366] = 85;
    wvowels[367] = 117;
    wvowels[368] = 85;
    wvowels[369] = 117;
    wvowels[370] = 85;
    wvowels[371] = 117;
    wvowels[374] = 89;
    wvowels[375] = 121;
    wvowels[404] = 89;
    wvowels[461] = 65;
    wvowels[462] = 97;
    wvowels[463] = 73;
    wvowels[480] = 65;
    wvowels[464] = 105;
    wvowels[465] = 79;
    wvowels[466] = 111;
    wvowels[467] = 85;
    wvowels[468] = 117;
    wvowels[469] = 85;
    wvowels[470] = 117;
    wvowels[471] = 85;
    wvowels[479] = 97;
    wvowels[472] = 117;
    wvowels[474] = 117;
    wvowels[475] = 85;
    wvowels[476] = 117;
    wvowels[478] = 65;
    wvowels[481] = 97;
    wvowels[490] = 79;
    wvowels[491] = 111;
    wvowels[492] = 79;
    wvowels[493] = 111;
    wvowels[97] = 97;
    wvowels[105] = 105;
    wvowels[121] = 121;
    wvowels[65] = 65;
    wvowels[101] = 101;
    wvowels[79] = 79;
    wvowels[89] = 89;
    
}

//===================================================================

Exporting bool valid_latin_table(short tableindex) {
    if (!alllatincodes.check(tableindex))
        return false;
    return true;
}
/*
 *----------------------------------------------------------------------
 *
 * strtod --
 *
 *    This procedure converts a floating-point number from an ASCII
 *    decimal representation to internal double-precision format.
 *
 * Results:
 *    The return value is the double-precision floating-point
 *    representation of the characters in string.  If endPtr isn't
 *    NULL, then *endPtr is filled in with the address of the
 *    next character after the last one that was part of the
 *    floating-point number.
 *
 * Side effects:
 *    None.
 *
 *----------------------------------------------------------------------
 */
//===================================================================

//Don de Frédéric Roussey
static const double arConvertExpMinus2[] =
{
  1.e-32, 1.e-31, 1.e-30,
  1.e-29, 1.e-28, 1.e-27, 1.e-26, 1.e-25, 1.e-24, 1.e-23, 1.e-22, 1.e-21, 1.e-20,
  1.e-19, 1.e-18, 1.e-17, 1.e-16, 1.e-15, 1.e-14, 1.e-13, 1.e-12, 1.e-11, 1.e-10,
  1.e-09, 1.e-08, 1.e-07, 1.e-06, 1.e-05, 1.e-04, 1.e-03, 1.e-02, 1.e-01, 1.e-00,
};

static const double arConvertExp[] =
{
  1.e00, 1.e01, 1.e02, 1.e03, 1.e04, 1.e05, 1.e06, 1.e07, 1.e08, 1.e09,
  1.e10, 1.e11, 1.e12, 1.e13, 1.e14, 1.e15, 1.e16, 1.e17, 1.e18, 1.e19,
  1.e20, 1.e21, 1.e22, 1.e23, 1.e24, 1.e25, 1.e26, 1.e27, 1.e28, 1.e29,
  1.e30, 1.e31, 1.e32,
};


static inline double power10(BLONG n)
{
  if (n > 0)
  {
     BLONG n1(n & 0x1f);   //n1 modulo 32
     BLONG n2(n & (~0x1f));//n2 div 32
     n2 >>= 5;
     if (n2 == 0)
        return arConvertExp[n1];
     double d = arConvertExp[n1];
     while (n2--)
        d *= arConvertExp[32];
     return d;
  }
  else if (n < 0)
  {
     BLONG n1(n & 0x1f);   //n1 modulo 32
     BLONG n2(n & (~0x1f));//n2 div 32
     n2 >>= 5;
     if (n2 == -1)
        return arConvertExpMinus2[n1];
     double d = arConvertExpMinus2[n1];
     while (++n2)
        d *= arConvertExpMinus2[0];
     return d;
  }
  return 1;
}
//----------------------------------------------------------------------------------------

extern "C" {
    //Implementation, which replaces strtod, which does not work properly on some platform...
    double conversiontofloathexa(const char* s, int sign) {
        BLONG v = 0;
        bool cont = true;
        uchar c = *s++;
        while (cont) {
            switch (digitaction[c]) {
                case '0':
                    v = (v << 4) | (c & 15);
                    c = *s++;
                    continue;
                case 'X':
                    v = (v << 4) | (c - 55);
                    c = *s++;
                    continue;
                case 'x':
                    v = (v << 4) | (c - 87);
                    c = *s++;
                    continue;
                default:
                    cont = false;
            }
        }
        
        double res = v;

        if (c == '.') {
            uchar mantissa = 0;
            v = 0;
            cont = true;
            c = *s++;
            while (cont) {
                switch (digitaction[c]) {
                    case '0':
                        v = (v << 4) | (c & 15);
                        c = *s++;
                        mantissa += 4;
                        continue;
                    case 'X':
                        v = (v << 4) | (c - 55);
                        mantissa += 4;
                        c = *s++;
                        continue;
                    case 'x':
                        v = (v << 4) | (c - 87);
                        mantissa += 4;
                        c = *s++;
                        continue;
                    default:
                        cont = false;
                }
            }
            
            res += (double)v/(double)(1 << mantissa);
        }
        

        if ((c &0xDF) == 'P') {
            bool sgn = false;
            if (*s == '-') {
                sgn = true;
                ++s;
            }
            else {
                if (*s == '+')
                    ++s;
            }
            
            v = *s++ & 15;
            while (isadigit(*s)) {
                v = (v << 3) + (v << 1) + (*s++ & 15);
            }
            v = 1 << v;
            if (sgn)
                res *= 1 / (double)v;
            else
                res *= v;

        }
        
        return res*sign;
    }
}

double conversionfloathexa(const char* s) {
    while (*s!=0 && *s<=32) ++s;
    //End of string...
    if (*s ==0 )
        return 0;
    
    int sign = 1;

    //Sign
    if (*s=='-') {
        sign = -1;
        ++s;
    }
    else
        if (*s=='+')
            ++s;
    
    if (*s=='0' && s[1]=='x') {
        s+=2;
        return conversiontofloathexa(s, sign);
    }
    
    BLONG v;
    if (isadigit(*s)) {
        v = *s++ & 15;
        while (isadigit(*s)) {
            v = (v << 3) + (v << 1) + (*s++ & 15);
        }
        if (!*s)
            return v*sign;
    }
    else
        return 0;
    
    double res = v;

    if (*s=='.') {
        ++s;
        if (isadigit(*s)) {
            uchar mantissa = 1;
            v = *s++ & 15;
            while (isadigit(*s)) {
                v = (v << 3) + (v << 1) + (*s++ & 15);
                ++mantissa;
            }
            res += (double)v / power10(mantissa);
        }
        else
            return res*sign;
    }
        
    if ((*s &0xDF) == 'E') {
        ++s;
        long sgn = 1;
        if (*s == '-') {
            sgn = -1;
            ++s;
        }
        else {
            if (*s == '+')
                ++s;
        }
        
        if (isadigit(*s)) {
            v = *s++ & 15;
            while (isadigit(*s))
                v = (v << 3) + (v << 1) + (*s++ & 15);
            
            res *= power10(v*sgn);
        }
    }
    return res*sign;
}

//------------------------------------------------------------------------------------
//Keep length
//------------------------------------------------------------------------------------
double conversiontofloathexa(const char* s, int sign, short& l) {
    BLONG v = 0;
    bool cont = true;
    uchar c = *s++;
    l++;
    while (cont) {
        switch (digitaction[c]) {
            case '0':
                v = (v << 4) | (c & 15);
                c = *s++;
                l++;
                continue;
            case 'X':
                v = (v << 4) | (c - 55);
                c = *s++;
                l++;
                continue;
            case 'x':
                v = (v << 4) | (c - 87);
                c = *s++;
                l++;
                continue;
            default:
                cont = false;
        }
    }
    
    double res = v;

    if (c == '.') {
        uchar mantissa = 0;
        v = 0;
        cont = true;
        c = *s++;
        l++;
        while (cont) {
            switch (digitaction[c]) {
                case '0':
                    v = (v << 4) | (c & 15);
                    c = *s++;
                    l++;
                    mantissa += 4;
                    continue;
                case 'X':
                    v = (v << 4) | (c - 55);
                    mantissa+=4;
                    c = *s++;
                    l++;
                    continue;
                case 'x':
                    v = (v << 4) | (c - 87);
                    mantissa += 4;
                    c = *s++;
                    l++;
                    continue;
                default:
                    cont = false;
            }
        }
        
        res += (double)v/(double)(1 << mantissa);
    }
    

    if ((c &0xDF) == 'P') {
        bool sgn = false;
        if (*s == '-') {
            sgn = true;
            ++s;
            l++;
        }
        else {
            if (*s == '+') {
                ++s;
                ++l;
            }
        }
        
        v = *s++ & 15;
        l++;
        while (isadigit(*s)) {
            v = (v << 3) + (v << 1) + (*s++ & 15);
            l++;
        }
        v = 1 << v;
        if (sgn)
            res *= 1 / (double)v;
        else
            res *= v;

    }
    
    return res*sign;
}

double conversionfloathexa(const char* s, short& l) {
    l = 0;
    //End of string...
    if (*s ==0 )
        return 0;
    
    int sign = 1;

    //Sign
    if (*s=='-') {
        sign = -1;
        l++;
        ++s;
    }
    else
        if (*s=='+') {
            ++s;
            l++;
        }
    
    if (*s=='0' && s[1]=='x') {
        s+=2;
        l++;
        return conversiontofloathexa(s, sign, l);
    }
    
    BLONG v;
    if (isadigit(*s)) {
        v = *s++ & 15;
        l++;
        while (isadigit(*s)) {
            v = (v << 3) + (v << 1) + (*s++ & 15);
            l++;
        }
        if (!*s)
            return v*sign;
    }
    else
        return 0;
    
    double res = v;

    if (*s=='.') {
        ++s;
        l++;
        if (isadigit(*s)) {
            uchar mantissa = 1;
            v = *s++ & 15;
            l++;
            while (isadigit(*s)) {
                v = (v << 3) + (v << 1) + (*s++ & 15);
                l++;
                ++mantissa;
            }
            res += (double)v / power10(mantissa);
        }
        else
            return res*sign;
    }
        
    if ((*s &0xDF) == 'E') {
        ++s;
        l++;
        long sgn = 1;
        if (*s == '-') {
            sgn = -1;
            ++s;
            l++;
        }
        else {
            if (*s == '+') {
                ++s;
                ++l;
            }
        }
        
        if (isadigit(*s)) {
            v = *s++ & 15;
            l++;
            while (isadigit(*s)) {
                v = (v << 3) + (v << 1) + (*s++ & 15);
                l++;
            }
            
            res *= power10(v*sgn);
        }
    }
    return res*sign;
}

void noconversiontofloathexa(const char* s, int sign, short& l) {
    bool cont = true;
    uchar c = *s++;
    l++;
    while (cont) {
        switch (digitaction[c]) {
            case '0':
                c = *s++;
                l++;
                continue;
            case 'X':
                c = *s++;
                l++;
                continue;
            case 'x':
                c = *s++;
                l++;
                continue;
            default:
                cont = false;
        }
    }
    
    if (c == '.') {
        cont = true;
        c = *s++;
        l++;
        while (cont) {
            switch (digitaction[c]) {
                case '0':
                    c = *s++;
                    l++;
                    continue;
                case 'X':
                    c = *s++;
                    l++;
                    continue;
                case 'x':
                    c = *s++;
                    l++;
                    continue;
                default:
                    cont = false;
            }
        }
    }
    

    if ((c &0xDF) == 'P') {
        bool sgn = false;
        if (*s == '-') {
            sgn = true;
            ++s;
            l++;
        }
        else {
            if (*s == '+') {
                ++s;
                ++l;
            }
        }
        
        s++;
        l++;
        while (isadigit(*s)) {
            s++;
            l++;
        }
    }
}

void noconversionfloathexa(const char* s, short& l) {
    l = 0;
    //End of string...
    if (*s ==0 )
        return;
    
    int sign = 1;

    //Sign
    if (*s=='-') {
        sign = -1;
        l++;
        ++s;
    }
    else
        if (*s=='+') {
            ++s;
            l++;
        }
    
    if (*s=='0' && s[1]=='x') {
        s+=2;
        l++;
        noconversiontofloathexa(s, sign, l);
        return;
    }
    
    if (isadigit(*s)) {
        s++;
        l++;
        while (isadigit(*s)) {
            s++;
            l++;
        }
        if (!*s)
            return;
    }
    else
        return;
    
    if (*s=='.') {
        ++s;
        l++;
        if (isadigit(*s)) {
            s++;
            l++;
            while (isadigit(*s)) {
                s++;
                l++;
            }
        }
        else
            return;
    }
        
    if ((*s &0xDF) == 'E') {
        ++s;
        l++;
        long sgn = 1;
        if (*s == '-') {
            sgn = -1;
            ++s;
            l++;
        }
        else {
            if (*s == '+') {
                ++s;
                ++l;
            }
        }
        
        if (isadigit(*s)) {
            s++;
            l++;
            while (isadigit(*s)) {
                s++;
                l++;
            }
        }
    }
}
//----- WIDE CHAR Versions
double conversiontofloathexa(const wchar_t* s, int sign, short& l) {
    BLONG v = 0;
    bool cont = true;
    uchar c = *s++;
    l++;
    while (cont) {
        switch (digitaction[c]) {
            case '0':
                v = (v << 4) | (c & 15);
                c = *s++;
                l++;
                continue;
            case 'X':
                v = (v << 4) | (c - 55);
                c = *s++;
                l++;
                continue;
            case 'x':
                v = (v << 4) | (c - 87);
                c = *s++;
                l++;
                continue;
            default:
                cont = false;
        }
    }
    
    double res = v;

    if (c == '.') {
        uchar mantissa = 0;
        v = 0;
        cont = true;
        c = *s++;
        l++;
        while (cont) {
            switch (digitaction[c]) {
                case '0':
                    v = (v << 4) | (c & 15);
                    c = *s++;
                    l++;
                    mantissa += 4;
                    continue;
                case 'X':
                    v = (v << 4) | (c - 55);
                    mantissa+=4;
                    c = *s++;
                    l++;
                    continue;
                case 'x':
                    v = (v << 4) | (c - 87);
                    mantissa += 4;
                    c = *s++;
                    l++;
                    continue;
                default:
                    cont = false;
            }
        }
        
        res += (double)v/(double)(1 << mantissa);
    }
    

    if ((c &0xDF) == 'P') {
        bool sgn = false;
        if (*s == '-') {
            sgn = true;
            ++s;
            l++;
        }
        else {
            if (*s == '+') {
                ++s;
                ++l;
            }
        }
        
        v = *s++ & 15;
        l++;
        while (isadigit(*s)) {
            v = (v << 3) + (v << 1) + (*s++ & 15);
            l++;
        }
        v = 1 << v;
        if (sgn)
            res *= 1 / (double)v;
        else
            res *= v;

    }
    
    return res*sign;
}

double conversionfloathexa(const wchar_t* s, short& l) {
    l = 0;
    //End of string...
    if (*s ==0 )
        return 0;
    
    int sign = 1;

    //Sign
    if (*s=='-') {
        sign = -1;
        l++;
        ++s;
    }
    else
        if (*s=='+') {
            ++s;
            l++;
        }
    
    if (*s=='0' && s[1]=='x') {
        s+=2;
        l++;
        return conversiontofloathexa(s, sign, l);
    }
    
    BLONG v;
    if (isadigit(*s)) {
        v = *s++ & 15;
        l++;
        while (isadigit(*s)) {
            v = (v << 3) + (v << 1) + (*s++ & 15);
            l++;
        }
        if (!*s)
            return v;
    }
    else
        return 0;
    
    double res = v;

    if (*s=='.') {
        ++s;
        l++;
        if (isadigit(*s)) {
            uchar mantissa = 1;
            v = *s++ & 15;
            l++;
            while (isadigit(*s)) {
                v = (v << 3) + (v << 1) + (*s++ & 15);
                l++;
                ++mantissa;
            }
            res += (double)v / power10(mantissa);
        }
        else
            return res;
    }
        
    if ((*s &0xDF) == 'E') {
        ++s;
        l++;
        long sgn = 1;
        if (*s == '-') {
            sgn = -1;
            ++s;
            l++;
        }
        else {
            if (*s == '+') {
                ++s;
                ++l;
            }
        }
        
        if (isadigit(*s)) {
            v = *s++ & 15;
            l++;
            while (isadigit(*s)) {
                v = (v << 3) + (v << 1) + (*s++ & 15);
                l++;
            }
            
            res *= power10(v*sgn);
        }
    }
    return res*sign;
}
//===================================================================
Exporting BLONG conversionintegerhexa(char* number) {
    while (*number!=0 && *number<=32) ++number;
    uchar c = *number;
    if (!c)
        return 0;
    
    int sign = 1;
    if (c == '-') {
        c = *++number;
        sign = -1;
    }
    else
        if (c == '+') {
            c = *++number;
        }
    
    
    ++number;
    BLONG v;
    if (c == '0' && *number == 'x') {
        ++number;
        
        v = 0;
        while (*number) {
            v <<= 4;
            c = *number;
            switch (digitaction[c]) {
                case '0':
                    v |= c & 15;
                    ++number;
                    continue;
                case 'X':
                    v |= c - 55;
                    ++number;
                    continue;
                case 'x':
                    v |= c - 87;
                    ++number;
                    continue;
                default:
                    return v*sign;
            }
        }
    }
    else {
        if (isadigit(c)) {
            v = c & 15;
            while (isadigit(*number))
                v = (v << 3) + (v << 1) + (*number++ & 15);
        }
        else
            return 0;
    }
    
    return v*sign;
}

BLONG conversionintegerhexa(wstring& number) {
    long ipos=0;
    
    while (number[ipos]<=32) ++ipos;
    

    int sign = 1;
    if (number[ipos] == '-') {
        ++ipos;
        sign = -1;
    }
    else
        if (number[ipos] == '+')
            ++ipos;
    
    BLONG v = 0;
    
    uchar c = number[ipos++];
	if (number.size() == ipos)
		return (c - 48);

    if (c == '0' || number[ipos] == 'x') {
        ipos++;
        c = number[ipos++];
        while (c) {
            v <<= 4;
            switch (digitaction[c]) {
                case '0':
                    v |= c & 15;
                    c = number[ipos++];
                    continue;
                case 'X':
                    v |= c - 55;
                    c = number[ipos++];
                    continue;
                case 'x':
                    v |= c - 87;
                    c = number[ipos++];
                    continue;
                default:
                    return v*sign;
            }
        }
    }
    else {
        if (isadigit(c)) {
            v = c & 15;
            c = number[ipos++];
            while (isadigit(c)) {
                v = (v << 3) + (v << 1) + (c & 15);
                c = number[ipos++];
            }
        }
    }
    return v*sign;
}

//===================================================================

//===================================================================
//Conversion from string to double...
Exporting double convertfloat(char* s) {
    return conversionfloathexa(s);
}

Exporting double convertfloat(wstring value) {
    string v;
    sc_unicode_to_utf8(v, value);
    return conversionfloathexa(STR(v));
}

Exporting long convertinteger(wstring value) {
    return conversionintegerhexa(value);
}

Exporting double convertfloat(string v) {
    return conversionfloathexa(STR(v));
}

Exporting long convertinteger(string v) {
    return conversionintegerhexa(STR(v));
}

//-----------------------------------------------------------
Exporting double convertdouble(wstring value) {
    string v;
    sc_unicode_to_utf8(v, value);
    return convertfloat(STR(v));
}

Exporting long convertlong(wstring value) {
    string v;
    sc_unicode_to_utf8(v, value);
    return conversionintegerhexa(STR(v));
}

Exporting double convertdouble(string v) {
    return convertfloat(STR(v));
}

Exporting long convertlong(string v) {
    return conversionintegerhexa(STR(v));
}

//===================================================================
static wstring convertdigits(string& v, wstring w) {
    for (short i=0;i<v.size();i++)
        w+=(wchar_t)v[i];
    return w;
}

static wstring convertdigits(string& v) {
    wstring w;
    for (short i=0;i<v.size();i++)
        w+=(wchar_t)v[i];
    return w;
}

static void convertingdigits(string& v, wstring& w) {
    for (short i=0;i<v.size();i++)
        w+=(wchar_t)v[i];
}


const short nbits =  11;

class doubledecimal {
    public :
    long n;
    double v;
    
    doubledecimal(double d) {
        v = d;
        n=0;
    }
    
    bool check() {
        v=(v-n)*10;
        n=v;
        if (!n && v <= 1e-9)
            return false;
        return true;
    }
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

Exporting void convertnumber(long v, string& s) {
    if (v>=0 && v <= 9999 && numbers.check(v)) {
        s = numbers.get(v);
        return;
    }
    
    bool sgn=false;
    if (v<0) {
        v=-v;
        if (v>=0 && v <= 9999 && numbers.check(v)) {
            s = "-";
            s+=numbers.get(v);
            return;
        }
        sgn=true;
    }
    
    char buff[20];
    char* num = buff+19;
    *num-- = 0;
    
    const char* nb;
    long inter=v;
    long rest=0;
    while (v) {
        inter = v/1000;
        rest = v-inter*1000;
        v = inter;
        nb=invertnumbers[rest];
        while (*nb) *num--=*nb++;
    }
    
    if (rest<10)
        num+=2;
    else
        if (rest<100)
            num++;

    if (sgn)
        *num='-';
    else
        ++num;
    s = num;
}

Exporting void convertnumber(long v, wstring& s) {
    if (v>=0 && v <= 9999 && numbers.check(v)) {
        s=L"";
        convertingdigits(numbers.get(v),s);
        return;
    }
    
    bool sgn=false;
    if (v<0) {
        v=-v;
        if (v>=0 && v <= 9999 && numbers.check(v)) {
            s = L"-";
            convertingdigits(numbers.get(v),s);
            return;
        }
        sgn=true;
    }
    
    wchar_t buff[50];
    
    wchar_t* num = buff+49;
    *num-- = 0;

    char* nb;
    long inter=v;
    long rest=0;
    while (v) {
        inter = v/1000;
        rest = v-inter*1000;
        v = inter;
        nb=invertnumbers[rest];
        while (*nb) *num--=(wchar_t)*nb++;
    }

    if (rest<10)
        num+=2;
    else
        if (rest<100)
            num++;

    if (sgn)
        *num='-';
    else
        ++num;
    
    s = num;
}

Exporting void convertnumber(double val, string& s) {
    if (!val) {
        s =  "0";
        return;
    }
    
    char buff[100];
    
    bool sgn=false;
    
    double v=val;
    if (v<0) {
        v=-v;
        sgn=true;
    }
    
    if (v <= 1e-7) {
        sprintf_s(buff,100,"%g",val);
        s = buff;
        return;
    }

    char* num = buff+50;
    
    *num-- = 0;
    
    //First the main digit
    BLONG vv=v;
    val=v-vv;
    const char* nbb;
    
    char nb=0;
    
    if (vv <= 9999 && numbers.check(vv)) {
        if (!val) {
            if (!sgn) {
                s = numbers[vv];
                return;
            }
            s = "-";
            s+=numbers[vv];
            return;
        }
        nb = numbers[vv].size();
        num-=nb-1;
        strcpy(num, STR(numbers[vv]));
        if (sgn)
            *--num='-';
    }
    else {
        BLONG inter=vv;
        BLONG rest = 0;
        while (vv) {
            inter = vv/1000;
            rest = vv-inter*1000;
            vv = inter;
            nbb=invertnumbers[rest];
            while (*nbb) {
                ++nb;
                *num--=*nbb++;
            }
        }

        if (rest<10) {
            num+=2;
            nb-=2;
        }
        else
            if (rest<100) {
                num++;
                --nb;
            }

        //98192819928999
        if (nb>8) {
            *num=num[1];
            num[1]='.';
            num[7]='e';
            num[8]='+';
            --nb;
            short sz;
            if (nb<10) {
                num[9]='0';
                num[10]=0x30|nb;
                sz=11;
            }
            else {
                nbb=invertnumbers[nb];
                nb = 8+nbb[4]; //the size of the string is stored on position 4
                sz = nb+1;
                while (*nbb) num[nb--]=*nbb++;
            }
            if (sgn) {
                *--num='-';
                ++sz;
            }
            num[sz]=0;
            s = num;
            return;
        }
        if (sgn)
            *num='-';
        else
            num++;
    }
    
    if (val) {
        nbb=num;
        //now, we now that we can start writing at buff+50;
        num=buff+50;
        *num++='.';
        nb = nbits;
        doubledecimal d(val);
        
        while (nb) {
            if (!d.check())
                break;
            *num++=0x30|d.n;
            nb--;
        }
        
        *num=0;
        if (!nb) { //arrondi
            if (num[-1] == '9' && num[-2] == '9' && buff[51] != '9') {
                num--;
                while (*num=='9') --num;
                num+=2;
            }
            
            if (num[-1]>='6' && num[-2] != '9')
                num[-2]++;
            num[-1] = 0;
            --num;
            s = nbb;
            return;
        }
        if (nb==nbits  || num[-1]==48) {
            //we remove the '.'
            num[-1]=0;
            --num;
        }
        s = nbb;
        return;
    }
    
    s = num;
}

Exporting void convertnumber(double val, wstring& s) {
    if (!val) {
        s = L"0";
        return;
    }
    
    wchar_t buff[100];
    bool sgn=false;

    double v=val;
    if (v<0) {
        v=-v;
        sgn=true;
    }

    if (v <= 1e-7) {
        swprintf_s(buff,100,L"%g",val);
        s = buff;
        return;
    }
    
    wchar_t* num = buff+50;
    *num-- = 0;
    
    //First the main digit
    BLONG vv=v;
    val=v-vv;
    
    char nb=0;
    
    if (vv <= 9999 && numbers.check(vv)) {
        if (!val) {
            if (!sgn) {
                s=L"";
                convertingdigits(numbers[vv],s);
                return;
            }
            s = L"-";
            convertingdigits(numbers[vv],s);
            return;
        }
        nb = numbers[vv].size();
        num-=nb-1;
        for (short i=0;i<nb;i++)
            num[i]= (wchar_t)(numbers[vv][i]);
        if (sgn)
            *--num='-';
    }
    else {
        BLONG inter=vv;
        BLONG rest=0;
        while (vv) {
            inter = vv/1000;
            rest = vv-inter*1000;
            vv = inter;
            char* nbb;
            nbb=invertnumbers[rest];
            while (*nbb) {
                ++nb;
                *num--=(wchar_t)*nbb++;
            }
        }

        if (rest<10) {
            num+=2;
            nb-=2;
        }
        else
            if (rest<100) {
                num++;
                --nb;
            }

        if (nb>8) {
            *num=num[1];
            num[1]='.';
            num[7]='e';
            num[8]='+';
            --nb;
            short sz;
            if (nb<10) {
                num[9]='0';
                num[10]=(wchar_t)(0x30|nb);
                sz=11;
            }
            else {
                const char* nbb=invertnumbers[nb];
                nb = 8+nbb[4]; //the size of the string is stored on position 4
                sz = nb+1;
                while (*nbb) num[nb--]=(wchar_t)*nbb++;
            }

            if (sgn) {
                *--num='-';
                ++sz;
            }
            num[sz]=0;
            s = num;
            return;
        }

        if (sgn)
            *num='-';
        else
            num++;
    }
    
    if (val) {
        wchar_t* beg=num;
        //now, we now that we can start writing at buff+50;
        num=buff+50;
        *num++='.';
        nb = nbits;
        doubledecimal d(val);
        
        while (nb) {
            if (!d.check())
                break;
            *num++= (wchar_t)(0x30|d.n);
            nb--;
        }
        
        *num=0;
        if (!nb) { //arrondi
            if (num[-1] == '9' && num[-2] == '9' && buff[51] != '9') {
                num--;
                while (*num=='9') --num;
                num+=2;
            }
            
            if (num[-1]>='6' && num[-2] != '9')
                num[-2]++;
            num[-1] = 0;
            --num;
            s= beg;
            return;
        }
        if (nb==nbits  || num[-1]==48) {
            //we remove the '.'
            num[-1]=0;
            --num;
        }
        s = beg;
        return;
    }
    s = num;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

Exporting string convertfromnumber(BLONG v) {
    if (v>=0 && v <= 9999 && numbers.check(v))
        return numbers.get(v);
    
    bool sgn=false;
    if (v<0) {
        v=-v;
        if (v>=0 && v <= 9999 && numbers.check(v)) {
            string s = "-";
            s+=numbers.get(v);
            return s;
        }
        sgn=true;
    }
    
    char buff[50];
    char* num = buff+49;
    *num-- = 0;
    
    const char* nb;
    BLONG inter=v;
    BLONG rest=0;
    while (v) {
        inter = v/1000;
        rest = v-inter*1000;
        v = inter;
        nb=invertnumbers[rest];
        while (*nb) *num--=*nb++;
    }
    
    if (rest<10)
        num+=2;
    else
        if (rest<100)
            num++;
    
    if (sgn)
        *num='-';
    else
        ++num;
    return string(num,(buff+49-num));
}

Exporting string convertfromnumber(long v) {
    if (v>=0 && v <= 9999 && numbers.check(v))
        return numbers.get(v);
    
    bool sgn=false;
    if (v<0) {
        v=-v;
        if (v>=0 && v <= 9999 && numbers.check(v)) {
            string s = "-";
            s+=numbers.get(v);
            return s;
        }
        sgn=true;
    }
    
    char buff[20];
    char* num = buff+19;
    *num-- = 0;
    
    const char* nb;
    long inter=v;
    long rest=0;
    while (v) {
        inter = v/1000;
        rest = v-inter*1000;
        v = inter;
        nb=invertnumbers[rest];
        while (*nb) *num--=*nb++;
    }
    
    if (rest<10)
        num+=2;
    else
        if (rest<100)
            num++;

    if (sgn)
        *num='-';
    else
        ++num;
    return string(num,(buff+19-num));
}

Exporting string convertfromnumber(short v) {
    if (v>=0 && v <= 9999 && numbers.check(v))
        return numbers.get(v);
    
    bool sgn=false;
    if (v<0) {
        v=-v;
        if (v>=0 && v <= 9999 && numbers.check(v)) {
            string s = "-";
            s+=numbers.get(v);
            return s;
        }
        sgn=true;
    }
    
    char buff[6];
    char* num = buff+5;
    *num-- = 0;
    
    const char* nb;
    short inter=v;
    short rest=0;
    while (v) {
        inter = v/1000;
        rest = v-inter*1000;
        v = inter;
        nb=invertnumbers[rest];
        while (*nb) *num--=*nb++;
    }

    if (rest<10)
        num+=2;
    else
        if (rest<100)
            num++;

    if (sgn)
        *num='-';
    else
        ++num;
    
    return string(num,(buff+5-num));
}

Exporting wstring wconvertfromnumber(BLONG v) {
    if (v>=0 && v <= 9999 && numbers.check(v))
        return convertdigits(numbers.get(v));
    
    bool sgn=false;
    if (v<0) {
        v=-v;
        if (v>=0 && v <= 9999 && numbers.check(v))
            return convertdigits(numbers.get(v),L"-");
        sgn=true;
    }
    
	wchar_t buff[50];
    
    wchar_t* num = buff+49;
    *num-- = 0;

    char* nb;
    BLONG inter=v;
    BLONG rest=0;
    while (v) {
        inter = v/1000;
        rest = v-inter*1000;
        v = inter;
        nb=invertnumbers[rest];
        while (*nb) *num--=(wchar_t)*nb++;
    }

    if (rest<10)
        num+=2;
    else
        if (rest<100)
            num++;

    if (sgn)
        *num='-';
    else
        ++num;
    
    return wstring(num,buff-num+49);
}

Exporting wstring wconvertfromnumber(long v) {
    if (v>=0 && v <= 9999 && numbers.check(v))
        return convertdigits(numbers.get(v));
    
    bool sgn=false;
    if (v<0) {
        v=-v;
        if (v>=0 && v <= 9999 && numbers.check(v))
            return convertdigits(numbers.get(v),L"-");
        sgn=true;
    }
    
	wchar_t buff[20];
    
	wchar_t* num = buff + 19;
    *num-- = 0;
    
    char* nb;
    long inter=v;
    long rest=0;
    while (v) {
        inter = v/1000;
        rest = v-inter*1000;
        v = inter;
        nb=invertnumbers[rest];
        while (*nb) *num--=(wchar_t)*nb++;
    }

    if (rest<10)
        num+=2;
    else
        if (rest<100)
            num++;

    if (sgn)
        *num='-';
    else
        ++num;
    
    return wstring(num,buff-num+19);
}

Exporting wstring wconvertfromnumber(short v) {
    if (v>=0 && v <= 9999 && numbers.check(v))
        return convertdigits(numbers.get(v));
    
    bool sgn=false;
    if (v<0) {
        v=-v;
        if (v>=0 && v <= 9999 && numbers.check(v))
            return convertdigits(numbers.get(v),L"-");
        sgn=true;
    }
    
	wchar_t buff[6];
    
	wchar_t* num = buff + 5;
    *num-- = 0;
    
    char* nb;
    short inter=v;
    short rest = 0;
    while (v) {
        inter = v/1000;
        rest = v-inter*1000;
        v = inter;
        nb=invertnumbers[rest];
        while (*nb) *num--=(wchar_t)*nb++;
    }

    if (rest<10)
        num+=2;
    else
        if (rest<100)
            num++;

    if (sgn)
        *num='-';
    else
        ++num;
    
    return wstring(num,buff-num+5);
}

const short nbbitsfloat = 7;

class floatdecimal {
    public :
    long n;
    float v;
    
    floatdecimal(float d) {
        v = d;
        n=0;
    }
    
    bool check() {
        v=(v-n)*10;
        n=v;
        if (!n && v <= 1e-5)
            return false;
        return true;
    }
};

Exporting string convertfromnumber(double val) {
    if (!val)
        return "0";
    
    char buff[100];
    
    bool sgn=false;
    
    double v=val;
    if (v<0) {
        v=-v;
        sgn=true;
    }
    
    if (v <= 1e-7) {
        long sz =sprintf_s(buff,100,"%g",val);
        return string(buff,sz);
    }

    char* num = buff+50;
    
    *num-- = 0;
    
    //First the main digit
    BLONG vv=v;
    val=v-vv;
    const char* nbb;
    
    char nb=0;
    
    if (vv <= 9999 && numbers.check(vv)) {
        if (!val) {
            if (!sgn)
                return numbers[vv];
            string s = "-";
            s+=numbers[vv];
            return s;
        }
        nb = numbers[vv].size();
        num-=nb-1;
        strcpy(num, STR(numbers[vv]));
        if (sgn)
            *--num='-';
    }
    else {
        BLONG inter=vv;
        BLONG rest = 0;
        while (vv) {
            inter = vv/1000;
            rest = vv-inter*1000;
            vv = inter;
            nbb=invertnumbers[rest];
            while (*nbb) {
                ++nb;
                *num--=*nbb++;
            }
        }

        if (rest<10) {
            num+=2;
            nb-=2;
        }
        else
            if (rest<100) {
                num++;
                --nb;
            }

        //98192819928999
        if (nb>8) {
            *num=num[1];
            num[1]='.';
            num[7]='e';
            num[8]='+';
            --nb;
            short sz;
            if (nb<10) {
                num[9]='0';
                num[10]=0x30|nb;
                sz=11;
            }
            else {
                nbb=invertnumbers[nb];
                nb = 8+nbb[4]; //the size of the string is stored on position 4
                sz = nb+1;
                while (*nbb) num[nb--]=*nbb++;
            }
            if (sgn) {
                *--num='-';
                ++sz;
            }
            num[sz]=0;
            return string(num,sz);
        }
        if (sgn)
            *num='-';
        else
            num++;
    }
    
    if (val) {
        nbb=num;
        //now, we now that we can start writing at buff+50;
        num=buff+50;
        *num++='.';
        nb = nbits;
        doubledecimal d(val);
        
        while (nb) {
            if (!d.check())
                break;
            *num++=0x30|d.n;
            nb--;
        }
        
        *num=0;
        if (!nb) { //arrondi
            if (num[-1] == '9' && num[-2] == '9' && buff[51] != '9') {
                num--;
                while (*num=='9') --num;
                num+=2;
            }
            
            if (num[-1]>='6' && num[-2] != '9')
                num[-2]++;
            num[-1] = 0;
            --num;
            return string(nbb,(num-nbb));
        }
        if (nb==nbits  || num[-1]==48) {
            //we remove the '.'
            num[-1]=0;
            --num;
        }
        return string(nbb,(num-nbb));
    }
    
    return string(num,(buff+50-num));
}

Exporting string convertfromnumber(float val) {
    if (!val)
        return "0";
    
    char buff[100];
    bool sgn=false;
    
    float v=val;
    if (v<0) {
        v=-v;
        sgn=true;
    }
    
    if (v <= 1e-7) {
        long sz =sprintf_s(buff,100,"%g",val);
        return string(buff,sz);
    }

    char* num = buff+50;
    *num-- = 0;
    
    //First the main digit
    long vv=v;
    val=v-vv;
    const char* nbb;
    char nb=0;
    
    if (vv <= 9999 && numbers.check(vv)) {
        if (!val) {
            if (!sgn)
                return numbers[vv];
            string s = "-";
            s+=numbers[vv];
            return s;
        }
        nb = numbers[vv].size();
        num-=nb-1;
        strcpy(num, STR(numbers[vv]));
        if (sgn)
            *--num='-';
    }
    else {
        long inter=vv;
        long rest=0;
        while (vv) {
            inter = vv/1000;
            rest = vv-inter*1000;
            vv = inter;
            nbb=invertnumbers[rest];
            while (*nbb) {
                ++nb;
                *num--=*nbb++;
            }
        }

        if (rest<10) {
            num+=2;
            nb-=2;
        }
        else
            if (rest<100) {
                num++;
                --nb;
            }

        if (nb>8) {
            *num=num[1];
            num[1]='.';
            num[7]='e';
            num[8]='+';
            --nb;
            short sz;
            if (nb<10) {
                num[9]='0';
                num[10]=0x30|nb;
                sz=11;
            }
            else {
                nbb=invertnumbers[nb];
                nb = 8+nbb[4]; //the size of the string is stored on position 4
                sz = nb+1;
                while (*nbb) num[nb--]=*nbb++;
            }
            if (sgn) {
                *--num='-';
                ++sz;
            }
            num[sz]=0;
            return string(num,sz);
        }

        if (sgn)
            *num='-';
        else
            num++;
    }
    
    if (val) {
        nbb=num;
        //now, we now that we can start writing at buff+50;
        num=buff+50;
        *num++='.';
        nb = nbbitsfloat;
        floatdecimal d(val);
        
        while (nb) {
            if (!d.check())
                break;
            *num++=0x30|d.n;
            nb--;
        }
        
        *num=0;
        if (!nb) { //arrondi
            if (num[-1] == '9' && num[-2] == '9' && buff[51] != '9') {
                num--;
                while (*num=='9') --num;
                num+=2;
            }
            
            if (num[-1]>='6' && num[-2] != '9')
                num[-2]++;
            num[-1] = 0;
            --num;
            return string(nbb,(num-nbb));
        }
        if (nb==nbbitsfloat || num[-1]==48) {
            //we remove the '.'
            num[-1]=0;
            --num;
        }
        return string(nbb,(num-nbb));
    }
    
    
    return string(num,(buff+50-num));
}

Exporting wstring wconvertfromnumber(double val) {
    if (!val)
        return L"0";
    
	wchar_t buff[100];
    bool sgn=false;

    double v=val;
    if (v<0) {
        v=-v;
        sgn=true;
    }

    if (v <= 1e-7) {
        long sz =swprintf_s(buff,100,L"%g",val);
        return wstring(buff,sz);
    }
    
    wchar_t* num = buff+50;
    *num-- = 0;
    
    //First the main digit
    BLONG vv=v;
    val=v-vv;
    
    char nb=0;
    
    if (vv <= 9999 && numbers.check(vv)) {
        if (!val) {
            if (!sgn)
                return convertdigits(numbers[vv]);
            return convertdigits(numbers[vv],L"-");
        }
        nb = numbers[vv].size();
        num-=nb-1;
        for (short i=0;i<nb;i++)
            num[i]= (wchar_t)(numbers[vv][i]);
        if (sgn)
            *--num='-';
    }
    else {
        BLONG inter=vv;
        BLONG rest=0;
        while (vv) {
            inter = vv/1000;
            rest = vv-inter*1000;
            vv = inter;
            char* nbb;
            nbb=invertnumbers[rest];
            while (*nbb) {
                ++nb;
                *num--=(wchar_t)*nbb++;
            }
        }

        if (rest<10) {
            num+=2;
            nb-=2;
        }
        else
            if (rest<100) {
                num++;
                --nb;
            }

        if (nb>8) {
            *num=num[1];
            num[1]='.';
            num[7]='e';
            num[8]='+';
            --nb;
            short sz;
            if (nb<10) {
                num[9]='0';
                num[10]=(wchar_t)(0x30|nb);
                sz=11;
            }
            else {
                const char* nbb=invertnumbers[nb];
                nb = 8+nbb[4]; //the size of the string is stored on position 4
                sz = nb+1;
                while (*nbb) num[nb--]=(wchar_t)*nbb++;
            }

            if (sgn) {
                *--num='-';
                ++sz;
            }
            num[sz]=0;
            return wstring(num,sz);
        }

        if (sgn)
            *num='-';
        else
            num++;
    }
    
    if (val) {
        wchar_t* beg=num;
        //now, we now that we can start writing at buff+50;
        num=buff+50;
        *num++='.';
        nb = nbits;
        doubledecimal d(val);
        
        while (nb) {
            if (!d.check())
                break;
            *num++= (wchar_t)(0x30|d.n);
            nb--;
        }
        
        *num=0;
        if (!nb) { //arrondi
            if (num[-1] == '9' && num[-2] == '9' && buff[51] != '9') {
                num--;
                while (*num=='9') --num;
                num+=2;
            }
            
            if (num[-1]>='6' && num[-2] != '9')
                num[-2]++;
            num[-1] = 0;
            --num;
            return wstring(beg,(num-beg));
        }
        if (nb==nbits  || num[-1]==48) {
            //we remove the '.'
            num[-1]=0;
            --num;
        }
        return wstring(beg,(num-beg));
    }
    
    return wstring(num,(buff+50-num));
}

Exporting wstring wconvertfromnumber(float val) {
    if (!val)
        return L"0";
    
	wchar_t buff[100];
    bool sgn=false;
    
    float v=val;
    if (v<0) {
        v=-v;
        sgn=true;
    }
    
    if (v <= 1e-7) {
        long sz =swprintf_s(buff,100,L"%g",val);
        return wstring(buff,sz);
    }

    wchar_t* num = buff+50;
    *num-- = 0;
    
    //First the main digit
    long vv=v;
    val=v-vv;
    char nb=0;
    
    if (vv <= 9999 && numbers.check(vv)) {
        if (!val) {
            if (!sgn)
                return convertdigits(numbers[vv]);
            return convertdigits(numbers[vv],L"-");
        }
        nb = numbers[vv].size();
        num-=nb-1;
        for (short i=0;i<nb;i++)
            num[i]= (wchar_t)(numbers[vv][i]);
        if (sgn)
            *--num='-';
    }
    else {
        long inter=vv;
        long rest=0;
        char* nbb;
        while (vv) {
            inter = vv/1000;
            rest = vv-inter*1000;
            vv = inter;
            nbb=invertnumbers[rest];
            while (*nbb) {
                ++nb;
                *num--=(wchar_t)*nbb++;
            }
        }

        if (rest<10) {
            num+=2;
            nb-=2;
        }
        else
            if (rest<100) {
                num++;
                --nb;
            }

        if (nb>8) {
            *num=num[1];
            num[1]='.';
            num[7]='e';
            num[8]='+';
            --nb;
            short sz;
            if (nb<10) {
                num[9]='0';
                num[10]=(wchar_t)(0x30|nb);
                sz=11;
            }
            else {
                const char* nbb=invertnumbers[nb];
                nb = 8+nbb[4]; //the size of the string is stored on position 4
                sz = nb+1;
                while (*nbb) num[nb--]=(wchar_t)*nbb++;
            }
            
            if (sgn) {
                *--num='-';
                ++sz;
            }
            num[sz]=0;
            return wstring(num,sz);
        }

        if (sgn)
            *num='-';
        else
            num++;
    }
    
    if (val) {
        wchar_t* beg=num;
        //now, we now that we can start writing at buff+50;
        num=buff+50;
        *num++='.';
        nb = nbbitsfloat;
        
        floatdecimal d(val);
        
        while (nb) {
            if (!d.check())
                break;
            *num++= (wchar_t)(0x30|d.n);
            nb--;
        }
        
        *num=0;
        if (!nb) { //arrondi
            if (num[-1] == '9' && num[-2] == '9' && buff[51] != '9') {
                num--;
                while (*num=='9') --num;
                num+=2;
            }
            
            if (num[-1]>='6' && num[-2] != '9')
                num[-2]++;
            num[-1] = 0;
            --num;
            return wstring(beg,(num-beg));
        }
        if (nb==nbbitsfloat || num[-1]==48) {
            //we remove the '.'
            num[-1]=0;
            --num;
        }
        return wstring(beg,(num-beg));
    }
    
    return wstring(num,(buff+50-num));
}

//===================================================================

Exporting void c_HTMLcode(string& s, TAMGUCHAR code) {
    InitHtmlCodes();
    
    if (codeshtml.check(code))
        s = codeshtml[code];
    else {
        char ch[10];
        sprintf_s(ch, 10, "&%ld;", (long)code);
        s = ch;
    }
}

Exporting void c_XMLcode(string& s, TAMGUCHAR code) {
    InitHtmlCodes();
    
    if (codesxml.check(code))
        s = codesxml[code];
    else {
        char ch[10];
        sprintf_s(ch, 10, "&%ld;", (long)code);
        s = ch;
    }
}

void s_EvaluateMetaCharacters(string& s) {
    InitHtmlCodes();
    
	TAMGUCHAR cd = 0;
    
    switch (s[0]) {
        case '\\':
            switch (s.size()) {
                case 0:
                case 1:
                    return;
                case 2:// \n \r \t \" etc...
                    if (metacodes.find(s) != metacodes.end())
                        s = metacodes[s];
                    else
                        s = s[1];
                    return;
                case 4://ASCII decimal: \234
                    if (s[1] == 'x') {
                        s[0] = '0';
                        cd = conversionintegerhexa(STR(s));
                        break;
                    }
                    s[0] = ' ';
                    cd = atoi(STR(s));
                    break;
                case 6://unicode  \u00FE
                    s[0] = '0'; s[1] = 'x';
                    cd = conversionintegerhexa(STR(s));
            }
            break;
        case 'u':
            s[0] = '0'; s[1] = 'x';
            cd = conversionintegerhexa(STR(s));
            break;
        case '&':
            if (s[1] == '#') {
                s = s.substr(2, s.size() - 3);
                cd = atoi(STR(s));
            }
            else
                if (htmlcodes.find(s) != htmlcodes.end())
                    cd = htmlcodes[s];
                else
                    return;
            break;
        case '%': //%CC
            s[0] = 'x';
            s = '0' + s;
            cd = conversionintegerhexa(STR(s));
            s = (char)cd;
            return;
    }
    
    if (!cd)
        return;
    
    uchar inter[5];
    c_unicode_to_utf8(cd, inter);
    s = (char*)inter;
}

void s_EvaluateMetaCharacters(wstring& s) {
    InitHtmlCodes();
    
	TAMGUCHAR cd = 0;
    
    switch (s[0]) {
        case '\\':
            switch (s.size()) {
                case 0:
                case 1:
                    return;
                case 2:// \n \r \t \" etc...
                    if (wmetacodes.find(s) != wmetacodes.end())
                        s = wmetacodes[s];
                    else
                        s = s[1];
                    return;
                case 4://ASCII decimal: \234
                    if (s[1] == 'x') {
                        s[0] = '0';
                        cd = convertinteger(s);
                        break;
                    }
                    s[0] = '+';
                    cd = convertinteger(s);
                    break;
                case 6://unicode  \u00FE
                    s[0] = '0'; s[1] = 'x';
                    cd = convertinteger(s);
            }
            break;
        case 'u':
            s[0] = '0'; s[1] = 'x';
            cd = convertinteger(s);
            break;
        case '&':
            if (s[1] == '#') {
                s = s.substr(2, s.size() - 3);
                cd = convertinteger(s);
            }
            else
                if (whtmlcodes.find(s) != whtmlcodes.end())
                    cd = whtmlcodes[s];
                else
                    return;
            break;
        case '%': //%CC
            s[0] = 'x';
            s = L'0' + s;
            cd = convertinteger(s);
            s = (char)cd;
            return;
    }
    
    if (!cd)
        return;

    s = (wchar_t)cd;
}

//--------------------------------------------------------------------------------
// Conversion one character: UTF8, UNICODE, LATIN
Exporting unsigned char c_unicode_to_utf8(TAMGUCHAR code, unsigned char* utf) {
	if (code < 0x0080) {
		utf[0] = (unsigned char)code;
		utf[1] = 0;
		return 1;
	}
	if (code < 0x0800) {
		utf[0] = 0xc0 | (code >> 6);
		utf[1] = 0x80 | (code& 0x3f);
		utf[2] = 0;
		return 2;
	}
	if (code < 0x10000) {
		utf[0] = 0xe0 | (code >> 12);
		utf[1] = 0x80 | ((code >> 6) & 0x3f);
		utf[2] = 0x80 | (code& 0x3f);
		utf[3] = 0;
		return 3;
	}

	utf[0] = 0xF0 | (code >> 18);
	utf[1] = 0x80 | ((code >> 12) & 0x3f);
	utf[2] = 0x80 | ((code >> 6) & 0x3f);
	utf[3] = 0x80 | (code& 0x3f);
	utf[4] = 0;
	return 4;
}


Exporting string c_unicode_to_utf8(TAMGUCHAR code) {
	char utf[5];
	if (code < 0x0080) {
		utf[0] = (unsigned char)code;
		utf[1] = 0;
		return utf;
	}
	if (code < 0x0800) {
		utf[0] = 0xc0 | (code >> 6);
		utf[1] = 0x80 | (code & 0x3f);
		utf[2] = 0;
		return utf;
	}
	if (code < 0x10000) {
		utf[0] = 0xe0 | (code >> 12);
		utf[1] = 0x80 | ((code >> 6) & 0x3f);
		utf[2] = 0x80 | (code & 0x3f);
		utf[3] = 0;
		return utf;
	}

	utf[0] = 0xF0 | (code >> 18);
	utf[1] = 0x80 | ((code >> 12) & 0x3f);
	utf[2] = 0x80 | ((code >> 6) & 0x3f);
	utf[3] = 0x80 | (code & 0x3f);
	utf[4] = 0;
	return utf;
}

Exporting TAMGUCHAR c_latin_table_to_unicode(int tableindex, TAMGUCHAR c) {
    if (c < 128 || c > 255 || !alllatincodes.check(tableindex))
        return c;
    
    if (alllatincodes[tableindex]->check(c))
        return (*alllatincodes[tableindex])[c];
    
    return c;
}

Exporting uchar c_latin_to_utf8(unsigned char code, unsigned char* utf) {
    if (!(code & 0x0080)) {
        utf[0] = (unsigned char)code;
        utf[1] = 0;
        return 1;
    }
    if (latincodes.check(code))
        c_unicode_to_utf8(latincodes[code], utf);
    else {
        utf[0] = 0xc0 | (code >> 6);
        utf[1] = 0x80 | (code & 0x3f);
        utf[2] = 0;
        return 2;
    }
    return 1;
}


uchar c_latin_to_utf8(unsigned char* utf, unsigned char* res, bool checktable = false) {
    uchar c = utf[0];
    if (!(c & 0x0080)) {
        res[0] = (unsigned char)c;
        res[1] = 0;
        return 1;
    }
    
    uchar nb = c_test_utf8(utf);
    //we take two, this is a UTF8 code...
    if (nb) {
        for (int i = 0; i <= nb; i++)
            res[i] = utf[i];
        res[nb+1] = 0;
        return nb+1;
    }
    
    if (checktable) {
        res[0] = '.';
        res[1] = 0;
        return 1;
    }
    
    if (latincodes.check(c))
        c_unicode_to_utf8(latincodes[c], res);
    else {
        res[0] = 0xc0 | (c >> 6);
        res[1] = 0x80 | (c & 0x3f);
        res[2] = 0;
    }
    return 1;
}

uchar concatenate_latin_unicode(wstring& res, unsigned char* utf, bool checktable) {
	uchar c = utf[0];
	if (!(c & 0x0080)) {
		res += (wchar_t&)c;
		return 1;
	}

	TAMGUCHAR code;
	c = c_utf8_to_unicode(utf, code);

	concat_char_convert_utf16(res, code);

    if (!c) {
		if (checktable)
			res += L'.';
		else
			res += (wchar_t)utf[0];
	}
	return (c + 1);
}

uchar c_latin_to_unicode(unsigned char* utf, TAMGUCHAR& code, bool checktable) {
    uchar c = utf[0];
    if (!(c & 0x0080)) {
        code = c;
        return 1;
    }
    
    //we take two, this is a UTF8 code...
    c= c_utf8_to_unicode(utf, code);
    if (!c) {
        if (checktable)
            code = '.';
        else
            code = utf[0];
    }
    return (c+1);
}

Exporting TAMGUCHAR c_unicode_to_latin(TAMGUCHAR u) {
    if (u < 0x0080)
        return u;
    
    if (codeslatin.check(u))
        return codeslatin[u];
    
    return 0;
}

uchar c_utf8_to_latin(unsigned char* utf, uchar& nb) {
    nb = 1;
    uchar c = utf[0];
    if (!(c & 0x0080))
        return c;
    
    uchar code;
    if ((utf[0] & 0xF0)== 0xF0) {
        if ((utf[1] & 0x80) == 0x80 && (utf[2] & 0x80)== 0x80 && (utf[3] & 0x80)== 0x80) {
            code = (utf[0] & 0x7) << 18;
            code |= (utf[1] & 0x3F) << 12;
            code |= (utf[2] & 0x3F) << 6;
            code |= (utf[3] & 0x3F);
            nb = 4;
            return code;
        }
        return c;
    }
    
    if ((utf[0] & 0xE0)== 0xE0) {
        if ((utf[1] & 0x80)== 0x80 && (utf[2] & 0x80)== 0x80) {
            code = (utf[0] & 0xF) << 12;
            code |= (utf[1] & 0x3F) << 6;
            code |= (utf[2] & 0x3F);
            nb = 3;
            return code;
        }
        return c;
    }
    
    if ((utf[0] & 0xC0)== 0xC0 && (utf[1] & 0x80)== 0x80) {
        code = (utf[0] & 0x1F) << 6;
        code |= (utf[1] & 0x3F);
        nb = 2;
        return code;
    }
    
    return c;
}

Exporting unsigned char c_utf8_to_unicode(unsigned char* utf, TAMGUCHAR& code) {
    code = utf[0];

    if (!(utf[0] & 0x0080))
        return 0;

    //....
    if ((utf[0] & 0xF0)== 0xF0) {
        if ((utf[1] & 0x80) == 0x80 && (utf[2] & 0x80)== 0x80 && (utf[3] & 0x80)== 0x80) {
            code = (utf[0] & 0x7) << 18;
            code |= (utf[1] & 0x3F) << 12;
            code |= (utf[2] & 0x3F) << 6;
            code |= (utf[3] & 0x3F);
            return 3;
        }
        return 0;
    }
    
    if ((utf[0] & 0xE0)== 0xE0) {
        if ((utf[1] & 0x80)== 0x80 && (utf[2] & 0x80)== 0x80) {
            code = (utf[0] & 0xF) << 12;
            code |= (utf[1] & 0x3F) << 6;
            code |= (utf[2] & 0x3F);
            return 2;
        }
        return 0;
    }
    
    if ((utf[0] & 0xC0)== 0xC0 && (utf[1] & 0x80)== 0x80) {
        code = (utf[0] & 0x1F) << 6;
        code |= (utf[1] & 0x3F);
        return 1;
    }
    
    return 0;
}

Exporting char c_test_utf8(unsigned char* utf) {
    if (utf == NULL || !(utf[0] & 0x0080))
        return 0;
    
    if ((utf[0] & 0xF0)== 0xF0) {
        if ((utf[1] & 0x80) == 0x80 && (utf[2] & 0x80)== 0x80 && (utf[3] & 0x80)== 0x80)
            return 3;
        return 0;
    }

    if ((utf[0] & 0xE0)== 0xE0) {
        if ((utf[1] & 0x80)== 0x80 && (utf[2] & 0x80)== 0x80)
            return 2;
        return 0;
    }
    
    if ((utf[0] & 0xC0)== 0xC0 && (utf[1] & 0x80)== 0x80)
        return 1;
    
    return 0;
}

Exporting char c_detect_utf8(unsigned char* utf) {
    if (utf == NULL || !(utf[0] & 0x0080))
        return 0;

    if ((utf[0] & 0xF0)== 0xF0) {
        if ((utf[1] & 0x80) == 0x80 && (utf[2] & 0x80)== 0x80 && (utf[3] & 0x80)== 0x80)
            return 3;
        return 0;
    }
    
    if ((utf[0] & 0xE0)== 0xE0) {
        if ((utf[1] & 0x80)== 0x80 && (utf[2] & 0x80)== 0x80)
            return 2;
        return 0;
    }
    
    if ((utf[0] & 0xC0)== 0xC0 && (utf[1] & 0x80)== 0x80)
        return 1;
    
    return 0;
}

    //Convert a unicode character into a utf16 character
Exporting bool c_unicode_to_utf16(uint32_t& res, uint32_t code) {
        //A unicode character is encoded over 4 bytes: 3 -> 0
        //if we have bits on byte 2, then we need to provide 4 bytes...
    if ((code & 0x1F0000) == 0) {
        res = code;
        return false;
    }
    
        //00000000 000uuuuu xxxxxxyy yyyyyyyy
        //110110ww    wwxxxxxx    110111yy    yyyyyyyy
    
        //wwww is uuuu-1
        //We need to provide 4 bytes...
        //The first byte should by 1101 1000 which is 0xD800
    uint32_t r = 0xD800 | ((code & 0xFC00) >> 10) | ((((code & 0x1F0000) >> 16) - 1) << 6);
    
        //The xxxxx are the six bytes on the right of byte 1
        //the yyyyy
    res = (r << 16) | 0xDC00 | (code & 0x3FF);
    
    return true;
}

Exporting bool c_utf16_to_unicode(uint32_t& r, uint32_t code, bool second) {
    if (second) {
        r |= code & 0x3FF;
        return false;
    }
    
        //if the first byte is  0xD8000000 then it is a four bytes coding
    if ((code & 0xFF00) == 0xD800) {
            //first we extract w
        r = ((((code & 0x03C0) >> 6) + 1) << 16) | ((code & 0x3F) << 10);
        return true;
    }
    
    //else r is code...
    r = code;
    return false;
}


//---------------------------------------------------------------------------
//convert a character position from byte to char and back
void c_chartobyteposition(unsigned char* contenu, long sz, long& bcpos, long& ecpos) {
    if (ecpos > sz)
        return;
    
    long begcpos = bcpos;
    long endcpos = ecpos;
    long i = 0;
    long nb;
    
    while (endcpos > 0 && i<sz) {
        if (!begcpos)
            bcpos = i;
        nb = c_test_utf8(contenu + i);
        if (nb == 3 && c_is_emoji(contenu,i)) {
            i++;
            while (c_is_emojicomp(contenu, i)) {
                i++;
            }
        }
        else
            i += nb + 1;
        endcpos--;
        begcpos--;
    }
    ecpos = i;
}

void c_bytetocharposition(unsigned char* contenu, long& bbpos, long& ebpos) {
    long i = 0;
    long sz = 0;
    long nb;
    long bpos = bbpos;
    
    while (i < ebpos) {
        if (i == bpos)
            bbpos = sz;
        nb = c_test_utf8(contenu + i);
        if (nb == 3 && c_is_emoji(contenu,i)) {
            i++;
            while (c_is_emojicomp(contenu, i)) {
                i++;
            }
        }
        else
            i += nb + 1;
        sz++;
    }
    
    ebpos = sz;
}


Exporting long c_chartobyteposition(unsigned char* contenu, long sz, long charpos) {
    if (charpos > sz)
        return sz;
    
    long i = 0;
#ifdef INTELINTRINSICS
    if (check_ascii(contenu, charpos, i))
        return charpos;
    charpos-=i;
#endif
    
    long nb;
    while (charpos > 0 && i<sz) {
        nb = c_test_utf8(contenu + i);
        if (nb == 3 && c_is_emoji(contenu,i)) {
            i++;
            while (c_is_emojicomp(contenu, i)) {
                i++;
            }
        }
        else
            i += nb + 1;
        charpos--;
    }
    return i;
}

Exporting long c_chartobyteposition(unsigned char* contenu, long charpos) {
    long i = 0;
    long nb;
    while (charpos > 0 && contenu[i]) {
        nb = c_test_utf8(contenu + i);
        if (nb == 3 && c_is_emoji(contenu,i)) {
            i++;
            while (c_is_emojicomp(contenu, i)) {
                i++;
            }
        }
        else
            i += nb + 1;
        charpos--;
    }
    return i;
}

Exporting long c_bytetocharposition(unsigned char* contenu, long charpos) {
    long i = 0;
    long sz = 0;
    long nb;
    while (i < charpos) {
        nb = c_test_utf8(contenu + i);
        if (nb == 3 && c_is_emoji(contenu,i)) {
            i++;
            while (c_is_emojicomp(contenu, i)) {
                i++;
            }
        }
        else
            i += nb + 1;
        sz++;
    }
    return sz;
}

Exporting long c_bytetocharpositionidx(unsigned char* contenu, long charpos, long& sz, long& i) {
    long nb;
    while (i<charpos) {
        nb = c_test_utf8(contenu + i);
        if (nb == 3 && c_is_emoji(contenu,i)) {
            i++;
            while (c_is_emojicomp(contenu, i)) {
                i++;
            }
        }
        else
            i += nb + 1;
        sz++;
    }
    return sz;
}

Exporting long c_chartobytepositionidx(unsigned char* contenu, long charpos, long& sz, long& i) {
    long nb;
    while (charpos>0) {
        nb = c_test_utf8(contenu + i);
        if (nb == 3 && c_is_emoji(contenu,i)) {
            i++;
            while (c_is_emojicomp(contenu, i)) {
                i++;
            }
        }
        else
            i += nb + 1;
        sz++;
        charpos--;
    }
    return i;
}

//---------------------------------------------------------------------------
Exporting long size_utf16(unsigned char* str, long sz, long& charsize) {
    
#ifdef INTELINTRINSICS
    long i;
    if (check_ascii(str, sz, i)) {
        charsize = sz;
        return sz;
    }
#endif
    
    charsize = 0;
    long sizeutf16 = 0;
    TAMGUCHAR c;
    uchar nb;
    while (sz--) {
        if (*str & 0x80) {
            nb = c_utf8_to_unicode(str, c);
            str += nb + 1;
            sz -= nb;
            ++sizeutf16;
            ++charsize;
            if (c & 0xFFFF0000)
                ++sizeutf16;
            continue;
        }
        ++charsize;
        ++sizeutf16;
        ++str;
    }
    return sizeutf16;
}

wstring getfullchar(wstring& s, long& i) {
	TAMGUCHAR c = getachar(s, i);
	wstring res;
	store_char_check_utf16(res, c);
	if (((c & 0x1F000) == 0x1F000) && c_is_emoji(c)) {
		long j = i + 1;
		c = getachar(s, j);
		while (c_is_emojicomp(c)) {
			i = j++;
			concat_char_check_utf16(res, c);
			c = getachar(s, j);
		}
	}
	return res;
}

void getafullchar(wstring& s, wstring& res, long& i) {
    TAMGUCHAR c = getachar(s, i);
	i++;
    concat_char_check_utf16(res, c);
    if (((c & 0x1F000) == 0x1F000) && c_is_emoji(c)) {
        c = getachar(s, i);
		i++;
        long j = i;
        while (c_is_emojicomp(c)) {
            concat_char_check_utf16(res, c);
            i = j;
            c = getachar(s, j);
			j++;
        }
    }
}


// Conversion strings
#ifdef WSTRING_IS_UTF16
//Conversion of a Unicode character to UTF16 before concatenating it to res...
void concat_char_convert_utf16(wstring& res, TAMGUCHAR code) {
	if (!(code & 0xFFFF0000))
		res += (wchar_t)code;
	else {
		TAMGUCHAR c16;
		c_unicode_to_utf16(c16, code);
		res += (wchar_t)(c16 >> 16);
		res += (wchar_t)(c16 & 0xFFFF);
	}
}


//On windows wchar_t is only 16 bits long...
//wstring is encoded as a UTF16 string...
//We concatenate here a UTF16 characters that could be stored on 4 bytes to two uint16_t chars...
//The order of characters here is given by 0xDC00 or 0xD800
Exporting void store_char_check_utf16(wstring& res, TAMGUCHAR code) {
	if ((code & 0xFF00FF000) == 0xD800D8000) {
		if ((code & 0xFF00FF000) == 0xDC00D8000) {
			res = (wchar_t)(code & 0xFFFF);
			res += (wchar_t)(code >> 16);
		}
		else {
			res = (wchar_t)(code >> 16);
			res += (wchar_t)(code & 0xFFFF);
		}
	}
	else {
		res = L"";
		concat_char_convert_utf16(res, code);
	}
}

Exporting void concat_char_check_utf16(wstring& res, TAMGUCHAR code) {
	if ((code & 0xFF00FF000) == 0xD800D8000) {
		if ((code & 0xFF00FF000) == 0xDC00D8000) {
			res += (wchar_t)(code & 0xFFFF);
			res += (wchar_t)(code >> 16);
		}
		else {
			res += (wchar_t)(code >> 16);
			res += (wchar_t)(code & 0xFFFF);
		}
	}
	else
		concat_char_convert_utf16(res, code);
}

Exporting size_t size_w(wchar_t* w) {
	wstring s;
	long i = 0;
	while (w[i] != 0)
		s += w[i++];
	return size_w(s);
}

size_t size_w(wstring& w) {
    long lg = w.size();
	
	long sz = 0;
	long i = 0;

#ifdef INTELINTRINSICS
        //we check if we have any large characters between 0 and ipos
    if (!check_large_char(WSTR(w), lg, sz))
        return lg;
	i = sz;
#endif

	TAMGUCHAR c;
	long j;
	for (; i < lg; i++) {
		if ((w[i] & 0xFF00) == 0xD800) {
			c = getachar(w, i);
			if (((c & 0x1F000) == 0x1F000) && c_is_emoji(c)) {
				j = i + 1;
				c = getachar(w, j);
				while (c_is_emojicomp(c)) {
					i = j++;
					c = getachar(w, j);
				}
			}
		}
		sz++;
	}
	return sz;
}

size_t size_w(wstring& w, long& first) {
    first = -1;

    long sz = 0;
    long lg = w.size();
    long i = 0;
#ifdef INTELINTRINSICS
        //we check if we have any large characters between 0 and ipos
    if (!check_large_char(WSTR(w), lg, sz))
        return lg;
    i = sz;
#endif

	TAMGUCHAR c;
	long j;
	for (; i < lg; i++) {
		if ((w[i] & 0xFF00) == 0xD800) {
			if (first == -1)
				first = i;
			c = getachar(w, i);
			if (((c & 0x1F000) == 0x1F000) && c_is_emoji(c)) {
				j = i + 1;
				c = getachar(w, j);
				while (c_is_emojicomp(c)) {
					i = j++;
					c = getachar(w, j);
				}
			}
		}
		sz++;
	}

	return sz;
}

//We convert a position in characters into an actual string position
long convertchartoposutf16(wstring& w, long first, long cpos) {
	long  i = first;
	long j;
	TAMGUCHAR c;
	while (first != cpos) {
		if ((w[i] & 0xFF00) == 0xD800) {
			i += getChar(w, i, c);
			if (((c & 0x1F000) == 0x1F000) && c_is_emoji(c)) {
				j = getChar(w, i, c);
				while (c_is_emojicomp(c)) {
					i += j;
					j = getChar(w, i, c);
				}
			}
		}
		else
            i++;
		first++;
	}
	return i;
}

long convertpostocharutf16(wstring& w, long first, long spos) {
	long  i = first;
	long j;
	TAMGUCHAR c;
	while (i != spos) {
		if ((w[i] & 0xFF00) == 0xD800) {
			i += getChar(w, i, c);
			if (((c & 0x1F000) == 0x1F000) && c_is_emoji(c)) {
				j = getChar(w, i, c);
				while (c_is_emojicomp(c)) {
					i += j;
					j = getChar(w, i, c);
				}
			}
		}
		else
			i++;
		first++;
	}
	return first;
}

Exporting long convertpostochar(wstring& w, long first, long spos) {
#ifdef INTELINTRINSICS
        //we check if we have any large characters between 0 and ipos
	if (!check_large_char(WSTR(w), w.size(), first))
        return spos;
#endif
    
    if (spos <= first)
        return spos;
    
	long  i = first;
	long j;
	TAMGUCHAR c;
	while (i != spos) {
		if ((w[i] & 0xFF00) == 0xD800) {
			i += getChar(w, i, c);
			if (((c & 0x1F000) == 0x1F000) && c_is_emoji(c)) {
				j = getChar(w, i, c);
				while (c_is_emojicomp(c)) {
					i += j;
					j = getChar(w, i, c);
				}
			}
		}
		else
			i++;
		first++;
	}
	return first;
}

Exporting long convertchartopos(wstring& w, long first, long cpos) {
#ifdef INTELINTRINSICS
        //we check if we have any large characters between 0 and ipos
	if (!check_large_char(WSTR(w), w.size(), first))
        return cpos;
#endif
    
    if (cpos <= first)
        return cpos;
    
	long  i = first;
	long j;
	TAMGUCHAR c;
	while (first != cpos) {
		if ((w[i] & 0xFF00) == 0xD800) {
			i += getChar(w, i, c);
			if (((c & 0x1F000) == 0x1F000) && c_is_emoji(c)) {
				j = getChar(w, i, c);
				while (c_is_emojicomp(c)) {
					i += j;
					j = getChar(w, i, c);
				}
			}
		}
		else
			i++;
		first++;
	}
	return i;
}

long convertpostocharraw(wstring& w, long first, long spos) {
    long  i = first;
    long j;
    TAMGUCHAR c;
    while (i != spos) {
        if ((w[i] & 0xFF00) == 0xD800) {
            i += getChar(w, i, c);
            if (((c & 0x1F000) == 0x1F000) && c_is_emoji(c)) {
                j = getChar(w, i, c);
                while (c_is_emojicomp(c)) {
                    i += j;
                    j = getChar(w, i, c);
                }
            }
        }
        else
            i++;
        first++;
    }
    return first;
}

long convertchartoposraw(wstring& w, long first, long cpos) {
    long  i = first;
    long j;
    TAMGUCHAR c;
    while (first != cpos) {
        if ((w[i] & 0xFF00) == 0xD800) {
            i += getChar(w, i, c);
            if (((c & 0x1F000) == 0x1F000) && c_is_emoji(c)) {
                j = getChar(w, i, c);
                while (c_is_emojicomp(c)) {
                    i += j;
                    j = getChar(w, i, c);
                }
            }
        }
        else
            i++;
        first++;
    }
    return i;
}


void convertpostochar(wstring& w, vector<long>& vspos) {
    long first = 0;
#ifdef INTELINTRINSICS
        //we check if we have any large characters between 0 and ipos
    if (!check_large_char(WSTR(w), w.size(), first))
        return;
#endif

    vector<long> vcpos;
    long realpos = first;
    long charpos = first;
    TAMGUCHAR c;
    long j;
    for (long i = 0; i < vspos.size(); i++) {
        if (vspos[i] <= first) {
            vcpos.push_back(vspos[i]);
            continue;
        }
        
        while (realpos != vspos[i]) {
            if ((w[realpos] & 0xFF00) == 0xD800) {
                realpos += getChar(w, realpos, c);
                if (((c & 0x1F000) == 0x1F000) && c_is_emoji(c)) {
                    j = getChar(w, realpos, c);
                    while (c_is_emojicomp(c)) {
                        realpos += j;
                        j = getChar(w, realpos, c);
                    }
                }
            }
            else
                realpos++;
            
            charpos++;
        }
        vcpos.push_back(charpos);
    }
    vspos = vcpos;
}

Exporting void sc_unicode_to_utf8(string& s, wstring& str) {
	long i = 0;
	char inter[5];
	long ineo = 0;
	long sz = str.size();
    if (!sz) {
        s = "";
		return;
    }
	long szo = 1 + (sz << 1);
	char* neo = new char[szo];
	neo[0] = 0;
	long nb;
	TAMGUCHAR c;

	while (i < sz) {
		if (str[i] < 0x0080 && ineo < szo - 1) {
			neo[ineo++] = (char)str[i];
			i++;
			continue;
		}

		if (c_utf16_to_unicode(c, str[i], false))
			c_utf16_to_unicode(c, str[++i], true);

		nb = c_unicode_to_utf8(c, (uchar*)inter);
		neo = concatstrings(neo, inter, ineo, szo, nb);
		i++;
	}

	neo[ineo] = 0;
	s = neo;
	delete[] neo;
}

Exporting void s_unicode_to_utf8(string& s, wchar_t* str, long sz) {
	if (!sz)
		return;

	long i = 0;
	char inter[5];
	long ineo = 0;
	long szo = 1 + (sz << 1);
	char* neo = new char[szo];
	neo[0] = 0;
	long nb;
	TAMGUCHAR c;

	while (i < sz) {
		if (str[i] < 0x0080 && ineo < szo - 1) {
			neo[ineo++] = (char)str[i];
			i++;
			continue;
		}

		if (c_utf16_to_unicode(c, str[i], false))
			c_utf16_to_unicode(c, str[++i], true);

		nb = c_unicode_to_utf8(c, (uchar*)inter);
		neo = concatstrings(neo, inter, ineo, szo, nb);
		i++;
	}

	neo[ineo] = 0;
	s += neo;
	delete[] neo;
}

Exporting void s_unicode_to_utf8(string& s, wstring& str) {
	long i = 0;
	char inter[5];
	long ineo = 0;
	long sz = str.size();
	if (!sz)
		return;

	long szo = 1 + (sz << 1);
	char* neo = new char[szo];
	neo[0] = 0;
	long nb;
	TAMGUCHAR c;

	while (i < sz) {
		if (str[i] < 0x0080 && ineo < szo - 1) {
			neo[ineo++] = (char)str[i];
			i++;
			continue;
		}

		if (c_utf16_to_unicode(c, str[i], false))
			c_utf16_to_unicode(c, str[++i], true);

		nb = c_unicode_to_utf8(c, (uchar*)inter);
		neo = concatstrings(neo, inter, ineo, szo, nb);
		i++;
	}

	neo[ineo] = 0;
	s += neo;
	delete[] neo;
}

Exporting void s_utf8_to_unicode(wstring& w, unsigned char* str, long sz) {
	if (!sz)
		return;

	long ineo = 0;
	wchar_t* neo = new wchar_t[sz + 1];
	neo[0] = 0;

#ifdef INTELINTRINSICS
	long i;
	if (check_ascii(str, sz, i)) {
		for (i = 0; i < sz; i++)
			neo[ineo++] = (wchar_t)str[i];
		neo[ineo] = 0;
		w += neo;
		delete[] neo;
		return;
	}
#endif
	TAMGUCHAR c, c16;
	uchar nb;
	while (sz--) {
		if (*str & 0x80) {
			nb = c_utf8_to_unicode(str, c);
			str += nb + 1;
			sz -= nb;
			if (!(c & 0xFFFF0000)) {
				neo[ineo++] = (wchar_t)c;
				continue;
			}

			c_unicode_to_utf16(c16, c);
			neo[ineo++] = (wchar_t)(c16 >> 16);
			neo[ineo++] = (wchar_t)(c16 & 0xFFFF);
			continue;
		}

		neo[ineo++] = (wchar_t)*str;
		++str;
	}

	neo[ineo] = 0;
	w += neo;
	delete[] neo;
}

Exporting void sc_utf8_to_unicode(wstring& w, unsigned char* str, long sz) {
    if (!sz) {
        w = L"";
		return;
    }

    long ineo = 0;
    wchar_t* neo = new wchar_t[sz + 1];
    neo[0] = 0;

#ifdef INTELINTRINSICS
	long i;
	if (check_ascii(str, sz, i)) {
		for (i = 0; i < sz; i++)
			neo[ineo++] = (wchar_t)str[i];
        neo[ineo] = 0;
        w = neo;
        delete[] neo;
		return;
	}
#endif
	TAMGUCHAR c, c16;

	uchar nb;
	while (sz--) {
		if (*str & 0x80) {
			nb = c_utf8_to_unicode(str, c);
			str += nb + 1;
			sz -= nb;
			if (!(c & 0xFFFF0000)) {
				neo[ineo++] = (wchar_t)c;
				continue;
			}

			c_unicode_to_utf16(c16, c);
			neo[ineo++] = (wchar_t)(c16 >> 16);
			neo[ineo++] = (wchar_t)(c16 & 0xFFFF);
			continue;
		}
        
		neo[ineo++] = (wchar_t)*str;
		++str;
	}
    
    neo[ineo] = 0;
    w = neo;
    delete[] neo;
}

Exporting void s_latin_to_unicode(wstring& res, unsigned char* contenu, long sz) {
	if (!sz)
		return;

    long ineo = 0;
    wchar_t* neo = new wchar_t[sz+1];
    neo[0] = 0;
    
#ifdef INTELINTRINSICS
    long i;
    if (check_ascii(contenu, sz, i)) {
        for (i = 0; i< sz; i++)
            neo[ineo++]= (wchar_t)contenu[i];
        neo[ineo] = 0;
        res += neo;
        delete[] neo;
        return;
    }
#endif
    
    TAMGUCHAR code, c16;
    uchar nb;
    
    while (sz--) {
        if (*contenu & 0x80) {
                //it could be a utf8 character...
            nb = c_utf8_to_unicode(contenu, code);
            if (!(code & 0xFFFF0000))
                neo[ineo++] = (wchar_t)code;
            else {
                c_unicode_to_utf16(c16, code);
                neo[ineo++] = (wchar_t)(c16 >> 16);
                neo[ineo++] = (wchar_t)(c16 & 0xFFFF);
            }
            if (nb) {
                contenu += nb + 1;
                sz -= nb;
#ifdef INTELINTRINSICS
                if (check_ascii(contenu, sz, i)) {
                    for (i = 0; i< sz; i++)
                        neo[ineo++] = (wchar_t)contenu[i];
                    neo[ineo] = 0;
                    res += neo;
                    delete[] neo;
                    return;
                }
#endif
            }
            
            continue;
        }
        
        neo[ineo++] = (wchar_t)*contenu;
        ++contenu;
    }
    
    neo[ineo] = 0;
    res += neo;
    delete[] neo;
}

Exporting void sc_latin_to_unicode(wstring& res, unsigned char* contenu, long sz) {
    if (!sz) {
        res = L"";
		return;
    }

    long ineo = 0;
    wchar_t* neo = new wchar_t[sz+1];
    neo[0] = 0;

#ifdef INTELINTRINSICS
	long i;
	if (check_ascii(contenu, sz, i)) {
		for (i = 0; i< sz; i++)
			neo[ineo++] = (wchar_t)contenu[i];
        neo[ineo] = 0;
        res= neo;
        delete[] neo;
		return;
	}
#endif

	TAMGUCHAR code, c16;
	uchar nb;

	while (sz--) {
		if (*contenu & 0x80) {
			//it could be a utf8 character...
			nb = c_utf8_to_unicode(contenu, code);
			if (!(code & 0xFFFF0000))
				neo[ineo++] = (wchar_t)code;
			else {
				c_unicode_to_utf16(c16, code);
				neo[ineo++] = (wchar_t)(c16 >> 16);
				neo[ineo++] = (wchar_t)(c16 & 0xFFFF);
			}
			if (nb) {
				contenu += nb + 1;
				sz -= nb;
#ifdef INTELINTRINSICS
				if (check_ascii(contenu, sz, i)) {
					for (i = 0; i< sz; i++)
						neo[ineo++] = (wchar_t)contenu[i];
                    neo[ineo] = 0;
                    res= neo;
                    delete[] neo;
					return;
				}
#endif
			}

			continue;
		}
        
		neo[ineo++] = (wchar_t)*contenu;
		++contenu;
	}
    
    neo[ineo] = 0;
    res= neo;
    delete[] neo;
}

#else

Exporting long convertchartopos(wstring& w, long first, long cpos) {
#ifdef INTELINTRINSICS
        //we check if we have any large characters between 0 and ipos
    if (!check_large_char(WSTR(w), w.size(), first))
        return cpos;
#endif
    
    if (cpos <= first)
        return cpos;

    long realpos = first;
    while (first != cpos) {
        if (((w[realpos] & 0x1F000) == 0x1F000) && c_is_emoji(w[realpos])) {
            realpos++;
            while (c_is_emojicomp(w[realpos])) {
                realpos++;
            }
        }
        else
            realpos++;
        first++;
    }
    return realpos;
}

Exporting long convertpostochar(wstring& w, long first, long spos) {
#ifdef INTELINTRINSICS
        //we check if we have any large characters between 0 and ipos
    if (!check_large_char(WSTR(w), w.size(), first))
        return spos;
#endif

    if (spos <= first)
        return spos;
    
    long realpos = first;
    while (realpos != spos) {
        if (((w[realpos] & 0x1F000) == 0x1F000) && c_is_emoji(w[realpos])) {
            realpos++;
            while (c_is_emojicomp(w[realpos])) {
                realpos++;
            }
        }
        else
            realpos++;
        first++;
    }
    return first;
}

Exporting long convertchartoposraw(wstring& w, long first, long cpos) {
    long realpos = first;
    while (first != cpos) {
        if (((w[realpos] & 0x1F000) == 0x1F000) && c_is_emoji(w[realpos])) {
            realpos++;
            while (c_is_emojicomp(w[realpos])) {
                realpos++;
            }
        }
        else
            realpos++;
        first++;
    }
    return realpos;
}

Exporting long convertpostocharraw(wstring& w, long first, long spos) {
    long realpos = first;
    while (realpos != spos) {
        if (((w[realpos] & 0x1F000) == 0x1F000) && c_is_emoji(w[realpos])) {
            realpos++;
            while (c_is_emojicomp(w[realpos])) {
                realpos++;
            }
        }
        else
            realpos++;
        first++;
    }
    return first;
}


void convertpostochar(wstring& w, vector<long>& vspos) {
    long first = 0;
#ifdef INTELINTRINSICS
        //we check if we have any large characters between 0 and ipos
    if (!check_large_char(WSTR(w), w.size(), first))
        return;
#endif

    vector<long> vcpos;
    long realpos = first;
    long charpos = first;
    for (long i = 0; i < vspos.size(); i++) {
        if (vspos[i] <= first) {
            vcpos.push_back(vspos[i]);
            continue;
        }
        
        while (realpos != vspos[i]) {
            if (((w[realpos] & 0x1F000) == 0x1F000) && c_is_emoji(w[realpos])) {
                realpos++;
                while (c_is_emojicomp(w[realpos])) {
                    realpos++;
                }
            }
            else
                realpos++;
            charpos++;
        }
        vcpos.push_back(charpos);
    }
    vspos = vcpos;
}


//We transform a character position into a position within w
Exporting long c_char_to_pos_emoji(wstring& w, long charpos) {
    long i = 0;
#ifdef INTELINTRINSICS
        //we check if we have any large characters between 0 and ipos
    if (!check_large_char(WSTR(w), charpos, i))
        return charpos;
#endif
    
        //there is one in the block starting at i...
    charpos -= i;
    while (charpos > 0) {
        if (((w[i] & 0x1F000) == 0x1F000) && c_is_emoji(w[i])) {
            i++;
            while (c_is_emojicomp(w[i]))
                i++;
        }
        else
            i++;
        charpos--;
    }
    return i;
}

Exporting void sc_latin_to_unicode(wstring& res, unsigned char* contenu, long sz) {
    if (!sz) {
        res = L"";
		return;
    }

    long ineo = 0;
    wchar_t* neo = new wchar_t[sz+1];
    neo[0] = 0;

#ifdef INTELINTRINSICS
	long i;
	if (check_ascii(contenu,sz,i)) {
		for (i=0; i< sz; i++)
			neo[ineo++]=(wchar_t)contenu[i];
        neo[ineo] = 0;
        res = neo;
        delete[] neo;
		return;
	}
#endif

	wchar_t code;
	uchar nb;

	while (sz--) {
		if (*contenu & 0x80) {
			//it could be a utf8 character...
			nb = c_utf8_to_unicode(contenu, code);
			neo[ineo++] = (wchar_t)code;
			if (nb) {
				contenu += nb+1;
				sz-=nb;
#ifdef INTELINTRINSICS
				if (check_ascii(contenu,sz,i)) {
					for (i=0; i< sz; i++)
						neo[ineo++]=(wchar_t)contenu[i];
                    neo[ineo] = 0;
                    res = neo;
                    delete[] neo;
					return;
				}
#endif
			}

			continue;
		}
		neo[ineo++]= (wchar_t)*contenu;
		++contenu;
	}
    
    neo[ineo] = 0;
    res = neo;
    delete[] neo;
}

Exporting void s_unicode_to_utf8(string& s, wstring& str) {
    long i = 0;
    char inter[5];
    long ineo = 0;
    long sz = str.size();
	if (!sz)
		return;

    long szo = 1 + (sz << 1);
    char* neo = new char[szo];
    neo[0] = 0;
    long nb;

    while (i < sz) {
        if (str[i] < 0x0080 && ineo < szo-1) {
            neo[ineo++] = (char)str[i];
            i++;
            continue;
        }
        
        nb = c_unicode_to_utf8(str[i], (uchar*)inter);
        neo = concatstrings(neo,inter,ineo, szo, nb);
        i++;
    }
    
    neo[ineo] = 0;
    s += neo;
    delete[] neo;
}


Exporting void sc_unicode_to_utf8(string& s, wstring& str) {
	long i = 0;
	char inter[5];
    long ineo = 0;
    long sz = str.size();
    if (!sz) {
        s = "";
		return;
    }
    long szo = 1 + (sz << 1);
    char* neo = new char[szo];
    neo[0] = 0;
    long nb;

    while (i < sz) {
        if (str[i] < 0x0080 && ineo < szo-1) {
            neo[ineo++] = (char)str[i];
            i++;
            continue;
        }
        
        nb = c_unicode_to_utf8(str[i], (uchar*)inter);
        neo = concatstrings(neo,inter,ineo, szo, nb);
        i++;
    }
    
    neo[ineo] = 0;
    s = neo;
    delete[] neo;
}

Exporting void s_unicode_to_utf8(string& s, wchar_t* str, long sz) {
	if (!sz)
		return;
    long i = 0;
    char inter[5];
    long ineo = 0;
    long szo = 1 + (sz << 1);
    char* neo = new char[szo];
    neo[0] = 0;
    long nb;

    while (i < sz) {
        if (str[i] < 0x0080 && ineo < szo-1) {
            neo[ineo++] = (char)str[i];
            i++;
            continue;
        }
        
        nb = c_unicode_to_utf8(str[i], (uchar*)inter);
        neo = concatstrings(neo,inter,ineo, szo, nb);
        i++;
    }
    
    neo[ineo] = 0;
    s += neo;
    delete[] neo;
}

Exporting void s_utf8_to_unicode(wstring& w, unsigned char* str , long sz) {
	if (!sz)
		return;

	long ineo = 0;
    wchar_t* neo = new wchar_t[sz+1];
    neo[0] = 0;

#ifdef INTELINTRINSICS
    long i;
    if (check_ascii(str,sz,i)) {
        for (i=0; i < sz; i++)
            neo[ineo++] = (wchar_t)str[i];
        neo[ineo] = 0;
        w += neo;
        delete[] neo;
        return;
    }
#endif

    wchar_t c;
    uchar nb;
    while (sz--) {
        if (*str & 0x80) {
            nb = c_utf8_to_unicode(str, c);
            str += nb+1;
            sz-=nb;
            neo[ineo++] = c;
            continue;
        }
        neo[ineo++] = (wchar_t)*str;
        ++str;
    }
    neo[ineo] = 0;
    w += neo;
    delete[] neo;
}

Exporting void sc_utf8_to_unicode(wstring& w, unsigned char* str, long sz) {
    w = L"";
	if (!sz)
		return;

    long ineo = 0;
    wchar_t* neo = new wchar_t[sz+1];
    neo[0] = 0;

#ifdef INTELINTRINSICS
    long i;
    if (check_ascii(str,sz,i)) {
        for (i=0; i < sz; i++)
            neo[ineo++] = (wchar_t)str[i];
        neo[ineo] = 0;
        w = neo;
        delete[] neo;
        return;
    }
#endif
    wchar_t c;

    uchar nb;
    while (sz--) {
        if (*str & 0x80) {
            nb = c_utf8_to_unicode(str, c);
            str += nb+1;
            sz-=nb;
            neo[ineo++] = c;
            continue;
        }
        neo[ineo++] = (wchar_t)*str;
        ++str;
    }
    neo[ineo] = 0;
    w = neo;
    delete[] neo;
}
#endif

Exporting void s_doubleutf8_to_unicode(wstring& s, wchar_t* str, long l) {
    string sutf8;
    for (long i = 0; i < l; i++) {
        char lc = str[i] & 0xFF;
        char lr = (str[i] & 0xFF00) >> 8;
        sutf8 += lc;
        sutf8 += lr;
    }
    s_utf8_to_unicode(s, USTR(sutf8), sutf8.size());
}

Exporting long conversion_utf8_to_fat(unsigned char* contenu, vector<string>& l) {
    long firstmul = -1;
    long lg = strlen((char*)contenu);
    l.reserve(lg);
    string res;
    for (long i = 0; i<lg; i++) {
        res = c_char_get(contenu, i);
        if (firstmul == -1 && res.size()>1)
            firstmul = res.size();
        l.push_back(res);
    }
    return firstmul;
}

Exporting unsigned char conversion_utf8_to_latin(short x) {
    if (x >= 32)
        return x;
    return 32;
}

Exporting void conversion_utf8_to_fatOne(unsigned char* contenu, string& s) {
    long i = 0;
    s = c_char_get(contenu, i);
}


Exporting long conversion_utf8_to_fat(string contenu, vector<string>& l) {
    return conversion_utf8_to_fat((unsigned char*)contenu.c_str(), l);
}

Exporting void conversion_utf8_to_fatOne(string contenu, string& s) {
    conversion_utf8_to_fatOne((unsigned char*)contenu.c_str(), s);
}

Exporting string s_utf8_to_latin(unsigned char* contenu, long sz) {
    string res;
    
#ifdef INTELINTRINSICS
    long i;
    if (check_ascii(contenu,sz,i))
        return (char*)contenu;
    //otherwise, it is in ASCII up to i
    uchar c;
    if (i) {
        c=contenu[i];
        contenu[i]=0;
        res=(char*)contenu;
        contenu[i]=c;
        contenu+=i;
        sz -= i;
    }
#endif
    
    uchar nb;
    TAMGUCHAR code;
    
    while (sz--) {
        
        if (*contenu & 0x80) {
            nb = c_utf8_to_unicode(contenu, code);
            switch(code) {
                case 338:
                    res+="OE";
                    break;
                case 339:
                    res+="oe";
                    break;
                default:
                    if (codeslatin.check(code))
                        res += codeslatin[code];
                    else
                        res += ".";
            }
            
            contenu += nb+1;
            sz -= nb;
#ifdef INTELINTRINSICS
            if (check_ascii(contenu,sz,i)) {
                res+=(char*)contenu;
                return res;
            }
            if (i) {
                c=contenu[i];
                contenu[i]=0;
                res += (char*)contenu;
                contenu[i]=c;
                contenu+=i;
                sz -= i;
            }
#endif
            continue;
        }
        
        res+=*contenu;
        ++contenu;
    }
    return res;
}

Exporting string conversion_unicode_to_latin(wstring& w) {
    string res;
    long sz = w.size();
	TAMGUCHAR code;
    for (long i = 0; i < sz; i++) {
        code = w[i];
        if (code < 0x80)
            res += (char)w[i];
        else {
            switch(code) {
                case 338:
                    res+="OE";
                    break;
                case 339:
                    res+="oe";
                    break;
                default:
                    if (codeslatin.check(code))
                        res += codeslatin[code];
                    else
                        res += ".";
            }
        }
    }
    return res;
}

string conversion_latin_to_utf8(unsigned char* contenu, long sz) {
    string res;
    s_latin_to_utf8(res, contenu, sz);
    return res;
}

Exporting string conversion_latin_table_to_utf8(short tableindex, unsigned char* contenu, long sz) {
    string res;
    
    if (!tableindex || tableindex == 1) {
        s_latin_to_utf8(res, contenu, sz);
        return res;
    }
    
    if (!alllatincodes.check(tableindex))
        return "";
    
#ifdef INTELINTRINSICS
    long i;
    if (check_ascii(contenu,sz,i))
        return (char*)contenu;
#endif
    
    uchar ch[5];
    uchar nb;
    
    
	basebin_hash<TAMGUCHAR>& thetable = *alllatincodes[tableindex];
    while (sz--) {
        if (*contenu & 0x80) {
            if (thetable.check(*contenu)) {
                nb = c_unicode_to_utf8(thetable[*contenu], ch);
                res += (char*)ch;
                ++contenu;
                continue;
            }
            
            //it could be a utf8 character...
            nb = c_latin_to_utf8(contenu, ch, true);
            res += (char*)ch;
            contenu += nb--;
            sz -= nb;
            continue;
        }
        
        res += *contenu;
        ++contenu;
    }
    return res;
}

Exporting wstring conversion_latin_table_to_unicode(short tableindex, unsigned char* contenu, long sz) {
    wstring res;
    
    if (!tableindex || tableindex == 1) {
        sc_latin_to_unicode(res,contenu,sz);
        return res;
    }
    
    if (!alllatincodes.check(tableindex))
        return L"";
    
	basebin_hash<TAMGUCHAR>& thetable = *alllatincodes[tableindex];
    uchar nb;
    
#ifdef INTELINTRINSICS
    long i;
    if (check_ascii(contenu,sz,i)) {
        for (i = 0; i < sz; i++)
            res += (wchar_t)contenu[i];
        return res;
    }
#endif
    
    while (sz--) {
        if (*contenu & 0x80) {
            if (thetable.check(*contenu)) {
                res += thetable[*contenu];
                ++contenu;
                continue;
            }

			nb = concatenate_latin_unicode(res, contenu, true);
            contenu += nb--;
            sz -= nb;
            continue;
        }
        
        res += (wchar_t)*contenu;
        ++contenu;
    }
    
    return res;
}

Exporting string s_unicode_to_dos(wstring& contenu) {
    string res;
    short c;
    for (long i = 0; i < contenu.size(); i++) {
        c = contenu[i];
        if (c <= 127 || !unicode2dos.check(c))
            res += (char)c;
        else
            res += (char)unicode2dos[c];
    }
    return res;
}


Exporting string s_utf8_to_dos(unsigned char* contenu) {
    long longueur = strlen((char*)contenu);
    string res;
    string utf;
    char cc = 0;
    long nb;
    for (long i = 0; i < longueur; i++) {
        nb = c_test_utf8(contenu + i);
        if (nb == 0) {
            nb = contenu[i];
            if (nb <= 127 || !unicode2dos.check(nb))
                res += nb;
            else
                res += (char)unicode2dos[nb];
        }
        else {
            cc = contenu[i + nb + 1];
            contenu[i + nb + 1] = 0;
            utf = (char*)contenu + i;
            if (utf8_to_dos.find(utf) != utf8_to_dos.end())
                res += (unsigned char)utf8_to_dos[utf];
            else {
                utf = s_deaccentuate(USTR(utf));
                res += utf;
            }
            contenu[i + nb + 1] = cc;
            i += nb;
        }
    }
    return res;
}

Exporting string s_utf8_to_dos(const char* contenu) {
    InitDosCodes();
    return s_utf8_to_dos((unsigned char*)contenu);
}

Exporting string s_dos_to_utf8(unsigned char* contenu) {
    InitDosCodes();
    long longueur = strlen((char*)contenu);
    string res;
    bulongchar xs;
    for (long i = 0; i < longueur; i++) {
        if (contenu[i] <= 127)
            res += contenu[i];
        else
            res += dos_to_utf8[contenu[i]];
    }
    return res;
}

Exporting string s_dos_to_utf8(const char* contenu) {
    InitDosCodes();
    return s_dos_to_utf8((unsigned char*)contenu);
}

Exporting void s_latin_to_utf8(string& res, unsigned char* contenu, long sz) {
    res = "";
#ifdef INTELINTRINSICS
    long i;
    if (check_ascii(contenu,sz,i)) {
        res = (char*)contenu;
        return;
    }
    
    //otherwise, it is in ASCII up to i
    uchar c;
    if (i) {
        c=contenu[i];
        contenu[i]=0;
        res=(char*)contenu;
        contenu[i]=c;
        contenu+=i;
        sz -= i;
    }
#endif
    uchar ch[5];
    uchar nb;
    
    while (sz--) {
        if (*contenu & 0x80) {
            //it could be a utf8 character...
            nb = c_latin_to_utf8(contenu, ch);
            res += (char*)ch;
            contenu += nb--;
            sz-=nb;
            
#ifdef INTELINTRINSICS
            if (check_ascii(contenu,sz,i)) {
                res += (char*)contenu;
                return;
            }
            
            if (i) {
                c=contenu[i];
                contenu[i]=0;
                res += (char*)contenu;
                contenu[i]=c;
                contenu+=i;
                sz -= i;
            }
#endif
            continue;
        }
        res += *contenu;
        ++contenu;
    }
}

Exporting string conversion_utf8_to_latin(string contenu) {
    return s_utf8_to_latin(USTR(contenu), contenu.size());
}

Exporting string conversion_latin_to_utf8(string contenu) {
    return conversion_latin_to_utf8(USTR(contenu), contenu.size());
}

Exporting string s_replacestring(string& s, string& reg, string& rep) {
    string neo;

#ifdef INTELINTRINSICS
    if (!replace_intel_all(neo, s, reg, rep))
        return s;
#else
    long gsz = reg.size();
    if (!gsz)
        return s;

    long rsz = s.size();
    long from = 0;

    long foundHere;
    while ((foundHere = s.find(reg, from)) != string::npos) {
        if (foundHere != from)
            neo += s.substr(from, foundHere - from);
        neo += rep;
        from = foundHere + gsz;
    }
    if (from < rsz)
        neo += s.substr(from, rsz - from);
#endif
    return neo;
}

Exporting string s_replacestrings(string& s, string reg, string rep) {
    string neo;
    
#ifdef INTELINTRINSICS
    if (!replace_intel_all(neo, s, reg, rep))
        return s;
#else
    long gsz = reg.size();
    if (!gsz)
        return s;
    
    long rsz = s.size();
    long from = 0;
    
    long foundHere;
    while ((foundHere = s.find(reg, from)) != string::npos) {
        if (foundHere != from)
            neo += s.substr(from, foundHere - from);
        neo += rep;
        from = foundHere + gsz;
    }
    if (from < rsz)
        neo += s.substr(from, rsz - from);
#endif
    return neo;
}

Exporting wstring s_replacestring(wstring& s, wstring reg, wstring rep) {
    wstring neo;
    
#ifdef INTELINTRINSICS
    if (!replace_intel_all(neo, s, reg, rep))
        return s;
#else
    long gsz = reg.size();
    if (!gsz)
        return s;
    
    long rsz = s.size();
    long from = 0;
    
    long foundHere;
    while ((foundHere = s.find(reg, from)) != string::npos) {
        if (foundHere != from)
            neo += s.substr(from, foundHere - from);
        neo += rep;
        from = foundHere + gsz;
    }
    if (from < rsz)
        neo += s.substr(from, rsz - from);
#endif
    return neo;
}

Exporting long c_char_next(unsigned char* m, long& i) {
    
    long nb = c_test_utf8(m + i);
    i += nb;
    return (nb + 1);
}

Exporting long c_code_get(unsigned char* m, long& i, TAMGUCHAR& code) {
    long nb = c_utf8_to_unicode(m + i, code);
    i += nb;
    return nb;
}

Exporting string c_char_get_next(unsigned char* m, size_t& i) {
    long nb = c_test_utf8(m + i);
    char str[] = { (char)m[i], 0, 0, 0, 0, 0, 0, 0 };
    
    if (nb == 0) {
        i++;
        return str;
    }
    
    char cc = m[i + nb + 1];
    m[i + nb + 1] = 0;
    strcpy_s(str, 8, (char*)(m + i));
    m[i + nb + 1] = cc;
    i += nb + 1;
    return str;
}

void c_chars_get_next(unsigned char* m, char* str, size_t& i) {
    long nb = c_test_utf8(m + i);
    str[0] = (char)m[i];
    
    switch (nb) {
        case 0:
            str[1] = 0;
            i++;
            return;
        case 1:
            str[1] = m[i + 1];
            str[2] = 0;
            i += 2;
            return;
        case 2:
            str[1] = m[i + 1];
            str[2] = m[i + 2];
            str[3] = 0;
            i += 3;
            return;
        case 3:
            str[1] = m[i + 1];
            str[2] = m[i + 2];
            str[3] = m[i + 3];
            str[4] = 0;
            i += 4;
    }
}

Exporting string c_char_get(unsigned char* m, long& i) {
    char str[] = {(char)m[i],0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    long nb = c_test_utf8(m + i);
    if (nb == 0)
        return str;

    long idx = 1;
    long j = i;
    if (nb == 3 && c_is_emoji(m, j)) {
        str[idx++] = (char)m[i+1];
        str[idx++] = (char)m[i+2];
        str[idx++] = (char)m[i+3];
        i = ++j;
        while (c_is_emojicomp(m, j)) {
            nb = c_test_utf8(m+i);
            str[idx++] = (char)m[i];
            switch (nb) {
                case 1:
                    str[idx++] = (char)m[i+1];
                    break;
                case 2:
                    str[idx++] = (char)m[i+1];
                    str[idx++] = (char)m[i+2];
                    break;
                case 3:
                    str[idx++] = (char)m[i+1];
                    str[idx++] = (char)m[i+2];
                    str[idx++] = (char)m[i+3];
            }
            i = ++j;
        }
        --i;
    }
    else {
        switch (nb) {
            case 1:
                str[idx] = (char)m[i+1];
                break;
            case 2:
                str[idx++] = (char)m[i+1];
                str[idx] = (char)m[i+2];
                break;
            case 3:
                str[idx++] = (char)m[i+1];
                str[idx++] = (char)m[i+2];
                str[idx] = (char)m[i+3];
        }
        i += nb;
    }
    return str;
}

Exporting TAMGUCHAR c_char_get_wide(unsigned char* m, long& i) {
	TAMGUCHAR code;
    i += c_utf8_to_unicode(m + i, code);
    return code;
}

Exporting long size_c(unsigned char* contenu, long sz) {
    long i = 0;
#ifdef INTELINTRINSICS
    if (check_ascii(contenu,sz,i))
        return sz;
#endif

    long size = i;
    long nb;
    
    while (i < sz) {
        nb = c_test_utf8(contenu + i);
        if (nb == 3 && c_is_emoji(contenu,i)) {
            i++;
            while (c_is_emojicomp(contenu, i)) {
                i++;
            }
        }
        else
            i += nb + 1;
        ++size;
    }
    
    return size;
}

Exporting long size_c(unsigned char* contenu, long sz, long& first) {
    long i = 0;
    first = -1;
#ifdef INTELINTRINSICS
    if (check_ascii(contenu,sz,i))
        return sz;
#endif
    
    long nb;
    long size = i;
    while (i < sz) {
        nb = c_test_utf8(contenu + i);
        if (nb && first == -1) {
            first = size;
        }
        if (nb == 3 && c_is_emoji(contenu,i)) {
            i++;
            while (c_is_emojicomp(contenu, i)) {
                i++;
            }
        }
        else
            i += nb + 1;
        ++size;
    }
    
    return size;
}

Exporting long size_c(string& s) {
    if (s == "")
        return 0;
    long lg = s.size();
    uchar* contenu = USTR(s);
    long i = 0;
    
#ifdef INTELINTRINSICS
    if (check_ascii(contenu,lg,i))
        return lg;
#endif
    
    long sz = i;
    long nb;
    while (i < lg) {
        nb = c_test_utf8(contenu + i);
        if (nb == 3 && c_is_emoji(contenu,i)) {
            i++;
            while (c_is_emojicomp(contenu, i)) {
                i++;
            }
        }
        else
            i += nb + 1;
        sz++;
    }
    
    return sz;
}

Exporting long size_c(const char* s) {
    if (s == NULL || s[0] == 0)
        return 0;
    return size_c((unsigned char*)s, strlen((char*)s));
}

Exporting long size_c(wstring& w) {
    long szr = w.size();
    long i = 0;

#ifdef INTELINTRINSICS
    if (!check_large_char(WSTR(w), szr, i))
        return szr;
#endif
    
    long sz = i;
    while (i < szr) {
        if (((w[i] & 0x1F000) == 0x1F000) && c_is_emoji(w[i])) {
            i++;
            while (c_is_emojicomp(w[i])) {
                i++;
            }
        }
        else
            i++;
        sz++;
    }
    return sz;
}

Exporting long size_c(wstring& w, long& emoji) {
    emoji = -1;
    long szr = w.size();
    long i = 0;

#ifdef INTELINTRINSICS
    if (!check_large_char(WSTR(w), szr, i)) {
        return szr;
    }
#endif
    
    long sz = i;
    while (i < szr) {
        if (((w[i] & 0x1F000) == 0x1F000) && c_is_emoji(w[i])) {
            i++;
            while (c_is_emojicomp(w[i])) {
                if (emoji == -1)
                    emoji = i - 1;
                i++;
            }
        }
        else
            i++;
        sz++;
    }
    return sz;
}

//i is character position
long getindex(unsigned char* contenu, long lg, long i) {
    long x = 0;
    
#ifdef INTELINTRINSICS
    if (check_ascii(contenu, lg, x))
        return i;
        //we then remove the length of the previous ASCII sequence
    i -= x;
#endif
    
    long nb;
    while (i > 0 && x < lg) {
        nb = c_test_utf8(contenu + x);
        if (nb == 3 && c_is_emoji(contenu,x)) {
            x++;
            while (c_is_emojicomp(contenu, x)) {
                x++;
            }
        }
        else
            x += nb + 1;
        i--;
    }
    
    return x;
}


Exporting string s_left(string& s, long nb) {
    if (nb <= 0)
        return "";
    
    long lg = s.size();
    nb = getindex(USTR(s), lg, nb);
    if (nb >= lg)
        return s;
    
    return s.substr(0, nb);
}

Exporting wstring s_left(wstring& s, long nb) {
    if (nb <= 0)
        return L"";

    nb = convertchartopos(s, 0, nb);
    if (nb >= s.size())
        return s;
    
    return s.substr(0, nb);
}

Exporting wstring s_right(wstring& s, long nb) {
    if (nb <= 0)
        return L"";
#ifdef WIN32
	long l = size_w(s) - nb;
#else
	long l = size_c(s) - nb;
#endif
	if (l <= 0)
        return s;
    
    l = convertchartopos(s, 0, l);
    return s.substr(l, s.size());
}

Exporting string s_right(string& s, long nb) {
    if (nb <= 0)
        return "";

    long lg = s.size();

	long l = size_c(USTR(s), lg) - nb;

	if (l <= 0)
        return s;
    
    l = getindex(USTR(s), lg, l);
    return s.substr(l, lg);
}

Exporting string s_middle(string& s, long l, long nb) {
    long lg = s.size();

    uchar* contenu = USTR(s);
    
    long i = getindex(contenu, lg, l);
    if (i >= lg)
        return "";

    nb += l;
    nb = getindex(contenu, lg, nb);
    return s.substr(i, nb - i);
}

Exporting wstring s_middle(wstring& s, long l, long nb) {
    if (l <= 0)
        return L"";
    
    long i = convertchartopos(s, 0, l);
    if (i >= s.size())
        return L"";
    
    nb += l;
    nb = convertchartopos(s, 0, nb);
    return s.substr(i, nb - i);
}

Exporting bool c_char_index_assign(string& s, string c, long x) {
    if (x > s.size())
        return false;
    long i;
    uchar* contenu = USTR(s);
    long lg = s.size();
    for (i = 0; i<lg && x>0; i++) {
        i += c_test_utf8(contenu + i);
        x--;
    }
    if (i == lg)
        s += c;
    else {
        x = i;
        s.erase(i, c_test_utf8(contenu + x) + 1);
        s.insert(i, c);
    }
    return true;
}

Exporting bool c_char_index_insert(string& s, string c, size_t x) {
    if (x > s.size())
        return false;
    long i;
    long lg = s.size();
    uchar* contenu = USTR(s);
    for (i = 0; i<lg && x>0; i++) {
        i += c_test_utf8(contenu + i);
        x--;
    }
    if (i == lg)
        s += c;
    else
        s.insert(i, c);
    return true;
}

//i is a character position
Exporting string c_char_index_remove(string& s, size_t i) {
    long lg = s.size();

    string res;
    
    unsigned char* content = USTR(s);
    
    long x = getindex(content, lg, i);
    if (x == i) {
        res = s[i];
        s.erase(i, 1);
        return res;
    }

    lg = x;
    res = c_char_get(content, lg);
    s.erase(x, res.size());
    return res;
}


Exporting string c_char_index(string& s, size_t i) {
    long lg = s.size();
    long x = getindex(USTR(s), lg, i);
    if (x >= lg)
        return "";    
    return c_char_get(USTR(s), x);
}

Exporting string s_char_reverse(string& s, long& length) {
    long lg = s.size();
    string res = "";
    string x;
    length = 0;
    for (long i = 0; i < lg; i++) {
        x = c_char_get(USTR(s), i);
        res.insert(0, x);
        length++;
    }
    return res;
}

Exporting string s_revert(string& s) {
    long lg = s.size();
    string res = "";
    string x;
    for (long i = 0; i < lg; i++) {
        x = c_char_get(USTR(s), i);
        res.insert(0, x);
    }
    return res;
}

wstring s_revert(wstring& s) {
    wstring res = L"";
    for (long i = s.size() - 1; i >= 0; i--)
        res += s[i];
    return res;
}

Exporting char c_is_alpha(unsigned char* m, long& i) {
	TAMGUCHAR v;
    i += c_utf8_to_unicode(m + i, v);
    
    if (utf8codemin.check(v))
        return 1;
    if (utf8codemaj.check(v))
        return 2;
    
    return 0;
}

Exporting string s_insert_sep_byte(string& s, string sep) {
    string res;
    long lg = s.size();
    for (long i = 0; i < lg; i++) {
        if (i)
            res += sep;
        res += s[i];
    }
    return res;
}


Exporting string s_insert_sep(string s, string sep) {
    string res;
    long lg = s.size();
    for (long i = 0; i < lg; i++) {
        if (i)
            res += sep;
        res += c_char_get(USTR(s), i);
    }
    return res;
}

Exporting bool s_is_utf8(unsigned char* contenu, long longueur) {
#ifdef INTELINTRINSICS
    long i;
    if (check_ascii(contenu, longueur, i))
        return false;
#endif

    while (longueur--) {
        if ((*contenu & 0x80) && c_test_utf8(contenu))
            return true;
        ++contenu;
    }
    return false;
}

Exporting TAMGUCHAR c_char_index_code(string& s, size_t i) {
	long lg = s.size();
    uchar* contenu = USTR(s);
    long x;
    for (x = 0; x<lg && i>0; x++) {
        x += c_test_utf8(contenu + x);
        i--;
    }
    TAMGUCHAR c;
    c_utf8_to_unicode(contenu + x, c);
    return c;
}

Exporting void c_char_index_code_all(string& s, vector<long>& vect) {
    long lg = s.size();
    uchar* contenu = USTR(s);
    vect.reserve(lg);
	TAMGUCHAR code;
    for (long i = 0; i < lg; i++) {
        i += c_utf8_to_unicode(contenu + i, code);
        vect.push_back((long)code);
    }
}

Exporting void c_char_index_code_all_long(string& s, vector<TAMGUCHAR>& vect) {
    size_t lg = s.size();
    uchar* contenu = USTR(s);
    vect.reserve(lg);
	TAMGUCHAR code;
    for (size_t i = 0; i < lg; i++) {
        i += c_utf8_to_unicode(contenu + i, code);
        vect.push_back(code);
    }
}


//On renvoie le code LATIN correspondant, 0 sinon
Exporting unsigned char c_utf8_latin(string s) {
	TAMGUCHAR v;
    c_utf8_to_unicode(USTR(s), v);
    if (v > 255)
        return 0;
    return (uchar)v;
}

//On renvoie la chaine UTF8 correspondante, 0 sinon
string c_latin_utf8(int c) {
    bulongchar xs;
    c_latin_to_utf8(c, xs.ustr);
    return xs.str;
}

Exporting bool s_is_punctuation(wstring& str) {
    for (long i = 0; i < str.size(); i++) {
        if (!punctuations.check(str[i]))
            return false;
    }
    return true;
}

Exporting bool c_is_space(TAMGUCHAR code) {
    static unsigned char spaces[] = { 9, 10, 13, 32, 160 };
    if ((code <= 160 && strchr((char*)spaces, (char)code)) || code == 0x202F || code == 0x3000)
        return true;
    return false;
}

Exporting bool s_is_space(string& str) {
    static unsigned char spaces[] = { 9, 10, 13, 32, 160 };
    long lg = str.size();
    uchar* contenu = USTR(str);
	TAMGUCHAR code;
    for (long i = 0; i < lg; i++) {
        i += c_utf8_to_unicode(contenu + i, code);
        if ((code <= 160 && !strchr((char*)spaces, (char)code)) && code != 0x202F && code != 0x3000)
            return false;
    }
    return true;
}

Exporting bool c_is_punctuation(TAMGUCHAR code) {
    return punctuations.check(code);
}

Exporting bool c_is_separator(TAMGUCHAR code) {
    return (c_is_space(code) || c_is_punctuation(code));
}

Exporting bool s_is_punctuation(string& str) {
    long lg = str.size();
    uchar* contenu = USTR(str);
	TAMGUCHAR code;
    for (long i = 0; i < lg; i++) {
        i += c_utf8_to_unicode(contenu + i, code);
        if (!punctuations.check(code))
            return false;
    }
    return true;
}

Exporting bool s_is_upper(string& s) {
    if (s == "")
        return false;
    long lg = s.size();
    for (long i = 0; i < lg; i++) {
        char ty = c_is_alpha(USTR(s), i);
        if (ty != 2)
            return false;
    }
    return true;
}

Exporting bool s_is_alpha(wstring& s) {
    if (s == L"")
        return false;
    for (long i = 0; i < s.size(); i++) {
        if (!utf8codemaj.check(s[i]) && !utf8codemin.check(s[i]))
            return false;
    }
    return true;
}


Exporting bool s_is_upper(wstring& s) {
    if (s == L"")
        return false;
    for (long i = 0; i < s.size(); i++) {
        if (!utf8codemaj.check(s[i]))
            return false;
    }
    return true;
}

Exporting bool s_is_lower(wstring& s) {
    if (s == L"")
        return false;
    for (long i = 0; i < s.size(); i++) {
        if (!utf8codemin.check(s[i]))
            return false;
    }
    return true;
}

Exporting bool c_is_consonant(TAMGUCHAR c) {
    if (!consonants.check(c))
        return false;
    return true;
}

Exporting bool c_is_vowel(TAMGUCHAR c) {
    if (!vowels.check(c))
        return false;
    return true;
}


Exporting bool s_is_consonant(wstring& s) {
    if (s == L"")
        return false;
    for (long i = 0; i < s.size(); i++) {
        if (!consonants.check(s[i]))
            return false;
    }
    return true;
}

Exporting bool s_is_vowel(wstring& s) {
    if (s == L"")
        return false;
    for (long i = 0; i < s.size(); i++) {
        if (!vowels.check(s[i]))
            return false;
    }
    return true;
}


Exporting bool s_is_consonant(unsigned char* s) {
    if (s[0] == 0)
        return false;
    long lg = strlen((char*)s);
	TAMGUCHAR code;
    
    for (long i = 0; i < lg; i++) {
        c_code_get(s, i, code);
        if (!consonants.check(code))
            return false;
    }
    
    return true;
}

Exporting bool s_is_vowel(unsigned char* s) {
    if (s[0] == 0)
        return false;
    long lg = strlen((char*)s);
	TAMGUCHAR code;
    
    for (long i = 0; i < lg; i++) {
        c_code_get(s, i, code);
        if (!vowels.check(code))
            return false;
    }
    
    return true;
}

static inline bool addbit(TAMGUCHAR& code, TAMGUCHAR& v, long& I, long& nb, long mx, long mxsz, char add) {
    if (add == 1)
        code |= v;
    v <<= 1;
    nb++;
    if (nb >= mxsz) {
        I++;
        nb = 0;
        v = 1;
        if (I >= mx)
            return false;
    }
    return true;
}

Exporting void s_binary(unsigned char* s, int mx, long lg, TAMGUCHAR* codes) {
    static basebin_hash<bool> rares;
    static bool init = false;
    
    if (!init) {
        rares['k'] = true;
        rares['z'] = true;
        rares['w'] = true;
        rares['x'] = true;
        rares['q'] = true;
        rares['h'] = true;
        rares['j'] = true;
        rares['K'] = true;
        rares['Z'] = true;
        rares['W'] = true;
        rares['X'] = true;
        rares['Q'] = true;
        rares['H'] = true;
        rares['J'] = true;
        init = false;
    }
    
	TAMGUCHAR code;
    long I = 0;
	TAMGUCHAR v = 1;
    long nb;
    long mxsz = sizeof(wchar_t);
    //z,w,x,q,h,j,k
    string c;
    
    for (long i = 0; i < lg; i++) {
        c_code_get(s, i, code);
        if (code <= 32)
            continue;
        if (vowels.check(code)) { //0001
            if (!addbit(codes[I], v, I, nb, mx, mxsz, 1))
                break;
            if (!addbit(codes[I], v, I, nb, mx, mxsz, 0))
                break;
            if (!addbit(codes[I], v, I, nb, mx, mxsz, 0))
                break;
            if (!addbit(codes[I], v, I, nb, mx, mxsz, 0))
                break;
        }
        else {
            if (consonants.check(code)) {//0010
                c = consonants[code];
                
                if (!addbit(codes[I], v, I, nb, mx, mxsz, 0))
                    break;
                
                if (!addbit(codes[I], v, I, nb, mx, mxsz, 1))
                    break;
                
                if (rares.check(c[0])) {//0110
                    if (!addbit(codes[I], v, I, nb, mx, mxsz, 1))
                        break;
                }
                else
                    if (!addbit(codes[I], v, I, nb, mx, mxsz, 0))
                        break;
                
                if (!addbit(codes[I], v, I, nb, mx, mxsz, 0))
                    break;
            }
            else { //1000
                if (!addbit(codes[I], v, I, nb, mx, mxsz, 0))
                    break;
                if (!addbit(codes[I], v, I, nb, mx, mxsz, 0))
                    break;
                if (!addbit(codes[I], v, I, nb, mx, mxsz, 0))
                    break;
                if (!addbit(codes[I], v, I, nb, mx, mxsz, 1))
                    break;
            }
        }
    }
}

Exporting bool compare_vowel(TAMGUCHAR c, TAMGUCHAR cc) {
    if (wvowels.check(c))
        c=wvowels[c];
    
    if (wvowels.check(cc))
        cc=wvowels[cc];
    
    return (c == cc);
}

Exporting wstring s_deaccentuate(wstring& s) {
    if (s == L"")
        return L"";
    
    long lg = s.size();
	TAMGUCHAR code;
    wstring v;
    
    for (long i = 0; i < lg; i++) {
        code = s[i];
        if (wvowels.check(code)) {
            v += wvowels[code];
            continue;
        }
        if (wconsonants.check(code)) {
            v += wconsonants[code];
            continue;
        }
        v += (wchar_t)code;
    }
    return v;
}

Exporting string s_deaccentuate(unsigned char* s) {
    if (s[0] == 0)
        return "";
    
    long lg = strlen((char*)s);
	TAMGUCHAR code;
    string v;
    long nb, I;
    char m[10];
    unsigned char c;
    
    for (long i = 0; i < lg; i++) {
        I = i + 1;
        nb = c_code_get(s, i, code);
        if (vowels.check(code)) {
            v += vowels[code];
            continue;
        }
        if (consonants.check(code)) {
            v += consonants[code];
            continue;
        }
        if (!nb)
            v += s[i];
        else {
            c = s[I + nb];
            s[I + nb] = 0;
            strcpy_s(m, 10, (char*)s + I - 1);
            s[I + nb] = c;
            v += m;
        }
    }
    return v;
}

Exporting bool s_is_alpha(string& s) {
    if (s == "")
        return false;
    long lg = s.size();
    for (long i = 0; i < lg; i++) {
        char ty = c_is_alpha(USTR(s), i);
        if (ty == 0)
            return false;
    }
    return true;
}

Exporting bool s_is_lower(string& s) {
    if (s == "")
        return false;
    
    long lg = s.size();
    for (long i = 0; i < lg; i++) {
        char ty = c_is_alpha(USTR(s), i);
        if (ty != 1)
            return false;
    }
    return true;
}

Exporting TAMGUCHAR c_to_lower(TAMGUCHAR c) {
    if (utf8codemaj.check(c))
        return utf8codemaj[c];
    return c;
}

Exporting TAMGUCHAR c_to_upper(TAMGUCHAR c) {
    if (utf8codemin.check(c))
        return utf8codemin[c];
    return c;
}

Exporting bool b_alpha(TAMGUCHAR c) {
    if (utf8codemaj.check(c) || utf8codemin.check(c))
        return true;
    return false;
}

//return 0 (not alpha), 1 (lower), 2 (upper)
Exporting char c_is_alpha(TAMGUCHAR code) {
    if (utf8codemin.check(code))
        return 1;
    
    if (utf8codemaj.check(code))
        return 2;
    return 0;
}

Exporting bool c_is_lower(TAMGUCHAR code) {
    return utf8codemin.check(code);
}

Exporting bool c_is_upper(TAMGUCHAR code) {
    return utf8codemaj.check(code);
}

Exporting wstring s_to_lower(wstring& s) {
    wstring res;
    long lg = s.size();
    for (long i = 0; i < lg; i++)
        res += (wchar_t)c_to_lower(s[i]);
    return res;
}

Exporting wstring s_to_upper(wstring& s) {
    wstring res;
    long lg = s.size();
    for (long i = 0; i < lg; i++)
        res += (wchar_t)c_to_upper(s[i]);
    return res;
}


Exporting string s_to_lower(string& s) {
    string res;
	TAMGUCHAR v;
    long lg = s.size();
    for (long i = 0; i < lg; i++) {
        i += c_utf8_to_unicode(USTR(s) + i, v);
        if (utf8codemaj.check(v))
            v = utf8codemaj[v];
        
        res += c_unicode_to_utf8(v);
    }
    return res;
}

Exporting string s_to_upper(string& s) {
    string res;
	TAMGUCHAR v;
    long lg = s.size();
    for (long i = 0; i < lg; i++) {
        i += c_utf8_to_unicode(USTR(s) + i, v);
        if (utf8codemin.check(v))
            v = utf8codemin[v];
        
        res += c_unicode_to_utf8(v);
    }
    return res;
}

Exporting void c_to_lower(string& res, unsigned char* s, long lg) {
    res = "";
	TAMGUCHAR v;
    for (long i = 0; i < lg; i++) {
        i += c_utf8_to_unicode(s + i, v);
        if (utf8codemaj.check(v))
            v = utf8codemaj[v];
        
        res += c_unicode_to_utf8(v);
    }
}

Exporting void c_to_upper(string& res, unsigned char* s, long lg) {
    res = "";
	TAMGUCHAR v;
    for (long i = 0; i < lg; i++) {
        i += c_utf8_to_unicode(s + i, v);
        if (utf8codemin.check(v))
            v = utf8codemin[v];
        
        res += c_unicode_to_utf8(v);
    }
}

//-------------------- FIND

Exporting char* strrstr(char* str, char* sub, long sz, long szsub) {
    if (!szsub)
        return NULL;
#ifdef INTELINTRINSICS
    long i = rfind_intel((uchar*)str, (uchar*)sub, sz, szsub, sz);
    if (i==-1)
        return NULL;
    return (str+i);
#else
    char* pstr;
    char* ptr = strrchr(str, sub[0]);
    while (ptr != NULL) {
        if (!memcmp(ptr, sub, szsub))
            return ptr;
        
        *ptr = 0;
        pstr = strrchr(str, sub[0]);
        *ptr = sub[0];
        ptr = pstr;
    }
    return NULL;
#endif
}

Exporting wchar_t* wcsrstr(wchar_t* str, wchar_t* sub, long sz, long szsub) {
    if (!szsub)
        return NULL;
#ifdef INTELINTRINSICS
    long i = rfind_intel(str, sub, sz, szsub, sz);
    if (i==-1)
        return NULL;
    return (str+i);
#else
    wchar_t* ptr = wcsrchr(str, sub[0]);
    if (ptr == NULL)
        return NULL;
    
    bool found = false;
    while (!found) {
        found = true;
        for (long i = 0; i < szsub; i++) {
            if (ptr[i] != sub[i]) {
                found = false;
                break;
            }
        }
        if (found)
            return ptr;
        
        *ptr = 0;
        wchar_t* pstr = wcsrchr(str, sub[0]);
        *ptr = sub[0];
        if (pstr == NULL)
            return NULL;
        ptr = pstr;
    }
    return NULL;
#endif
}

//we are looking for the substring substr in s
Exporting long s_findbyte(uchar* s, long sz, string& substr, long i) {
#ifdef INTELINTRINSICS
    if (sz >= thestringincrement)
        return find_intel_byte(s, USTR(substr), sz, substr.size(), i);
#endif
    uchar* res = (uchar*)strstr((char*)s+i, STR(substr));
    if (res == NULL)
        return -1;
    return (res-s);
}

//we are looking for the substring substr in s
Exporting long s_rfindbyte(uchar* s, long sz, string& substr, long i) {
#ifdef INTELINTRINSICS
    if (sz >= thestringincrement)
        return rfind_intel(s, USTR(substr), sz, substr.size(), i);
#endif
    string sb((char*)s, sz);
    return sb.rfind(substr,i);
}


//we are looking for the substring substr in s
Exporting long s_findbyte(string& s, string& substr, long i) {
#ifdef INTELINTRINSICS
    long sz = s.size();
    if ((sz-i) >= thestringincrement) {
        i = s.find_first_of(substr[0], i);
        if (i == -1)
            return -1;
        return find_intel_byte(USTR(s), USTR(substr), sz, substr.size(), i);
    }
#endif
    return s.find(substr,i);
}


//we are looking for the substring substr in s
Exporting long s_rfindbyte(string& s, string& substr, long i) {
#ifdef INTELINTRINSICS
    long sz = s.size();
    if (sz >= thestringincrement)
        return rfind_intel(USTR(s), USTR(substr), sz, substr.size(), i);
#endif
    return s.rfind(substr,i);
}

//we are looking for the substring substr in s
Exporting long s_find(string& s, string& substr, long i) {
#ifdef INTELINTRINSICS
    long sz = s.size();
    if ((sz-i) >= thestringincrement) {
        long firstutf8 = 0;
        if (check_ascii(USTR(s), s.size(), firstutf8)) {
            i = s.find_first_of(substr[0], i);
            if (i == -1)
                return -1;
            return find_intel_byte(USTR(s), USTR(substr), sz, substr.size(), i);
        }
        
        
        if (i > firstutf8)
            i = firstutf8 + c_chartobyteposition(USTR(s)+firstutf8, i - firstutf8);
        
        i = s.find_first_of(substr[0], i);
        if (i == -1)
            return -1;

        i = find_intel_byte(USTR(s), USTR(substr), sz, substr.size(), i);
        
        if (i > firstutf8)
            i = firstutf8 + c_bytetocharposition(USTR(s)+firstutf8, i-firstutf8);
        return i;
    }
#endif

    if (i)
        i = c_chartobyteposition(USTR(s), i);

    i = s.find(substr,i);

    if (i != -1)
        return c_bytetocharposition(USTR(s), i);

    return -1;
}

Exporting long s_count(string& str, string& sub, long i) {
#ifdef INTELINTRINSICS
    i = str.find_first_of(sub[0], i);
    if (i == -1)
        return 0;
    return count_strings_intel(USTR(str)+i, USTR(sub), str.size()-i, sub.size());
#else
    long nb = 0;
    i = str.find(sub, i);
    while (i != -1) {
        nb++;
        i = str.find(sub, ++i);
    }
    return nb;
#endif
}

Exporting long s_count(wstring& str, wstring& sub, long i) {
#ifdef INTELINTRINSICS
    i = str.find_first_of(sub[0], i);
    if (i == -1)
        return 0;
    return count_strings_intel(WSTR(str)+i, WSTR(sub), str.size()-i, sub.size());
#else
    long nb = 0;
    i = str.find(sub, i);
    while (i != -1) {
        nb++;
        i = str.find(sub, ++i);
    }
    return nb;
#endif
}


//we are looking for the substring substr in s
//it returns the position in character...
Exporting long s_rfind(string& s, string& substr, long i) {
#ifdef INTELINTRINSICS
    long sz = s.size();
    if (sz >= thestringincrement) {
        long firstutf8 = 0;
        if (check_ascii(USTR(s), s.size(), firstutf8))
            return rfind_intel(USTR(s), USTR(substr), sz, substr.size(), i);
        
        i = rfind_intel(USTR(s), USTR(substr), sz, substr.size(), i);
        
        if (i > firstutf8)
            i = firstutf8 + c_bytetocharposition(USTR(s)+firstutf8, i-firstutf8);
        return i;
    }
#endif
    if (i)
        i = c_chartobyteposition(USTR(s), i);

    i = s.rfind(substr,i);

    if (i != -1)
        return c_bytetocharposition(USTR(s), i);

    return -1;
}

long convertchartoposraw(wstring& w, long first, long spos);
long convertpostocharraw(wstring& w, long first, long spos);

//we are looking for the substring substr in s
Exporting long s_find(wstring& s, wstring& substr, long i) {
#ifdef INTELINTRINSICS
        //we check if we have any large characters between 0 and i
    long sz = s.size();
    if (sz >= thewstringincrement) {
        long first = 0;
        
        if (!check_large_char(WSTR(s), sz, first)) {
            i = s.find_first_of(substr[0], i);
            if (i == -1)
                return -1;
            return find_intel(WSTR(s), WSTR(substr), sz, substr.size(), i);
        }
        
        i = find_intel(WSTR(s), WSTR(substr), sz, substr.size(), i);
        
        if (i != -1) {
            if (i > first)
                return convertpostocharraw(s,first,i);
            return i;
        }
        return -1;
    }
#endif
    
    if (i)
        i = convertchartopos(s, 0, i);
    i = s.find(substr, i);
    if (i != -1)
        return convertpostochar(s,0,i);

    return -1;
}


//we are looking for the substring substr in s
Exporting long s_rfind(wstring& s, wstring& substr, long i) {
#ifdef INTELINTRINSICS
        //we check if we have any large characters between 0 and i
    long sz = s.size();
    if (sz >= thewstringincrement) {
        i = rfind_intel(WSTR(s), WSTR(substr), sz, substr.size(), i);
        
        if (i != -1)
            return convertpostochar(s,0,i);
        
        return -1;
    }
#endif
    if (i)
        i = convertchartopos(s, 0, i);
    i = s.rfind(substr, i);
    if (i != -1)
        return convertpostochar(s,0,i);
    
    return -1;
}

//we are looking for the substring substr in s
Exporting void s_findall(string& s, string& substr, vector<long>& v) {
#ifdef INTELINTRINSICS
    if (s.find_first_of(substr[0]) == -1)
        return;

    find_intel_all(USTR(s), s.size(), substr, v);
#else
    long sz = substr.size();
    long pos = s.find(substr, 0);
    while (pos != -1) {
        v.push_back(pos);
        pos=s.find(substr,pos+sz);
    }
#endif
    
}

void convertpostochar(wstring& w, vector<long>& vspos);
Exporting void s_findall(wstring& s, wstring& substr, vector<long>& v, long pos) {
    if (pos >= s.size())
        return;
    if (pos < 0)
        pos = 0;
    
#ifdef INTELINTRINSICS
    if (s.find_first_of(substr[0]) == -1)
        return;

    find_intel_all(WSTR(s), WSTR(substr), s.size(), substr.size(), v, pos);
#else
    long sz = substr.size();
    pos = s.find(substr, pos);
    while (pos != -1) {
        v.push_back(pos);
        pos=s.find(substr,pos+sz);
    }
#endif
    convertpostochar(s, v);
}

//---------------------------------------------------------------------

Exporting void v_convertbytetocharposition(unsigned char* s, vector<long>& v) {
    long i = 0;
    long c = 0;
    long p = 0;
    long nb;

    while (p < v.size()) {
        while (i < v[p]) {
            nb = c_test_utf8(s + i);
            if (nb == 3 && c_is_emoji(s,i)) {
                i++;
                while (c_is_emojicomp(s, i)) {
                    i++;
                }
            }
            else
                i += nb + 1;
            c++;
        }
        v[p++] = c;
    }
}

Exporting void v_convertchartobyteposition(unsigned char* contenu, vector<long>& v) {
    long c = 0;
    long iv = 0;
    long nb;
    long i = 0;
    long sz = v.size();
    while(iv < sz) {
        while (v[iv] == i)
            v[iv++] = c;

        nb = c_test_utf8(contenu + c);
        if (nb == 3 && c_is_emoji(contenu,c)) {
            c++;
            while (c_is_emojicomp(contenu, c)) {
                c++;
            }
        }
        else
            c += nb + 1;
        i++;
    }
}

Exporting void s_split(string& s, string& splitter, vector<string>& vs, bool keepblanks) {
    vector<long> values;
    Fast_String sub(s.size());

    long szsplit = splitter.size();
    long sz = s.size();
    
    s_findall(s, splitter, values);
    
    long previous = 0;
    long szcpy;
    for (long i = 0; i< values.size(); i++) {
        if (previous < values[i]) {
            szcpy = values[i]-previous;
            sub.set(USTR(s)+previous, szcpy);
            vs.push_back(sub.str());
        }
        else {
            if (keepblanks)
                vs.push_back("");
        }
        previous = values[i] + szsplit;
    }
        
    if (previous < sz) {
        szcpy = sz-previous;
        sub.set(USTR(s)+previous, szcpy);
        vs.push_back(sub.str());
    }
}


Exporting void s_split(wstring& s, wstring& splitter, vector<wstring>& vs, bool keepblanks) {
    size_t pos = 0;
    size_t found = 0;
    wstring sub;
    long sz = splitter.size();
    while (pos != -1) {
        found = s.find(splitter, pos);
        if (found != -1) {
            sub = s.substr(pos, found - pos);
            if (keepblanks)
                vs.push_back(sub);
            else
                if (sub != L"")
                    vs.push_back(sub);
            pos = found + sz;
        }
        else
            break;
    }
    
    sub = s.substr(pos, s.size() - pos);
    if (keepblanks)
        vs.push_back(sub);
    else
        if (sub != L"")
            vs.push_back(sub);
}

void v_split(string& thestr, string thesplitter, vector<string>& vs) {
    s_split(thestr, thesplitter, vs, false);
    for (long i = 0; i < vs.size(); i++)
        Trim(vs[i]);
}

Exporting bool c_is_digit(TAMGUCHAR code) {
    return isadigit(code);
}

Exporting bool c_is_hexa(TAMGUCHAR code) {
    static const char hexas[]= {'a','b','c','d','e','f','A','B','C','D','E','F'};
    
    if (isadigit(code))
        return true;
    
    if (code <= 'f' && strchr(hexas,(char)code))
        return true;
    
    return false;
}

Exporting bool s_threedigits(wstring& s, long i) {
    if (i > s.size() - 3)
        return false;
    
    if (isadigit(s[i]) && isadigit(s[i + 1]) && isadigit(s[i + 2])) {
        if (i < s.size() - 3)
            if (isadigit(s[i + 3]))
                return false;
        return true;
    }
    return false;
}

string& Trim(string& chaine) {
    long d, f;
    for (d = 0; d<chaine.size(); d++) {
        if ((uchar)chaine[d]>32)
            break;
    }
    
    for (f = chaine.size() - 1; f >= 0; f--) {
        if ((uchar)chaine[f] > 32)
            break;
    }
    long lg = f - d + 1;
    if (lg >= 1)
        chaine = chaine.substr(d, lg);
    else
        chaine = "";
    return chaine;
}

string& Trimleft(string& chaine) {
    long d, f;
    f = chaine.size() - 1;
    for (d = 0; d<chaine.size(); d++) {
        if ((uchar)chaine[d]>32)
            break;
    }
    
    long lg = f - d + 1;
    if (lg >= 1)
        chaine = chaine.substr(d, lg);
    else
        chaine = "";
    return chaine;
}

string& Trimright(string& chaine) {
    long d = 0, f;
    
    for (f = chaine.size() - 1; f >= 0; f--) {
        if ((uchar)chaine[f] > 32)
            break;
    }
    
    long lg = f - d + 1;
    if (lg >= 1)
        chaine = chaine.substr(d, lg);
    else
        chaine = "";
    return chaine;
}

wstring& Trim(wstring& chaine) {
    long d, f;
    for (d = 0; d<chaine.size(); d++) {
        if (chaine[d]>32)
            break;
    }
    
    for (f = chaine.size() - 1; f >= 0; f--) {
        if (chaine[f] > 32)
            break;
    }
    long lg = f - d + 1;
    if (lg >= 1)
        chaine = chaine.substr(d, lg);
    else
        chaine = L"";
    return chaine;
}

wstring& Trimleft(wstring& chaine) {
    long d, f;
    f = chaine.size() - 1;
    for (d = 0; d<chaine.size(); d++) {
        if (chaine[d]>32)
            break;
    }
    
    long lg = f - d + 1;
    if (lg >= 1)
        chaine = chaine.substr(d, lg);
    else
        chaine = L"";
    return chaine;
}

wstring& Trimright(wstring& chaine) {
    long d = 0, f;
    
    for (f = chaine.size() - 1; f >= 0; f--) {
        if (chaine[f] > 32)
            break;
    }
    
    long lg = f - d + 1;
    if (lg >= 1)
        chaine = chaine.substr(d, lg);
    else
        chaine = L"";
    return chaine;
}

void s_tokenize(vector<wstring>& vect, wstring& ustr, map<wstring, bool>& wkeeps) {
    
    //First we split along spaces
    long i;
    
    
	TAMGUCHAR c;
    wstring thesplitter;
    wstring punct;
    //we split along space characters...
    for (i = 0; i < ustr.size(); i++) {
        c = ustr[i];
        if (c <= 32) {
            if (thesplitter.size()) {
                if (wkeeps.find(thesplitter) == wkeeps.end()) {
                    if (punct.size()) {
                        thesplitter = thesplitter.substr(0, thesplitter.size() - punct.size());
                        vect.push_back(thesplitter);
                        for (int j = 0; j < punct.size(); j++) {
                            thesplitter = punct[j];
                            vect.push_back(thesplitter);
                        }
                    }
                    else
                        vect.push_back(thesplitter);
                }
                else
                    vect.push_back(thesplitter);
                thesplitter = L"";
                punct = L"";
            }
        }
        else {
            thesplitter += (wchar_t)c;
            if (c_is_punctuation(c))
                punct += (wchar_t)c;
            else
                punct = L"";
        }
    }
    
    if (thesplitter.size()) {
        if (wkeeps.find(thesplitter) == wkeeps.end()) {
            if (punct.size()) {
                thesplitter = thesplitter.substr(0, thesplitter.size() - punct.size());
                vect.push_back(thesplitter);
                for (int j = 0; j < punct.size(); j++) {
                    thesplitter = punct[j];
                    vect.push_back(thesplitter);
                }
            }
            else
                vect.push_back(thesplitter);
        }
        else
            vect.push_back(thesplitter);
    }
}



//string s= "l'été à la plage sur les cîmes";

void s_tokenize(vector<string>& vect, string& thestr, map<string, bool>& keeps) {
    //First we split along spaces
    long i;
    map<wstring, bool> wkeeps;
    wstring ustr;
    string s;
    
    for (auto& it : keeps) {
        s = it.first;
        ustr = L"";
        s_utf8_to_unicode(ustr, USTR(s), s.size());
        wkeeps[ustr] = true;
    }
    sc_utf8_to_unicode(ustr, USTR(thestr), thestr.size());
	TAMGUCHAR c;
    wstring thesplitter;
    wstring punct;
    //we split along space characters...
    for (i = 0; i < ustr.size(); i++) {
        c = ustr[i];
        if (c <= 32) {
            if (thesplitter.size()) {
                if (wkeeps.find(thesplitter) == wkeeps.end()) {
                    if (punct.size()) {
                        thesplitter = thesplitter.substr(0, thesplitter.size() - punct.size());
                        sc_unicode_to_utf8(s, thesplitter);
                        vect.push_back(s);
                        for (int j = 0; j < punct.size(); j++) {
                            thesplitter = punct[j];
                            sc_unicode_to_utf8(s, thesplitter);
                            vect.push_back(s);
                        }
                    }
                    else {
                        sc_unicode_to_utf8(s, thesplitter);
                        vect.push_back(s);
                    }
                }
                else {
                    sc_unicode_to_utf8(s, thesplitter);
                    vect.push_back(s);
                }
                thesplitter = L"";
                punct = L"";
            }
        }
        else {
            thesplitter += (wchar_t)c;
            if (c_is_punctuation(c))
                punct += (wchar_t)c;
            else
                punct = L"";
        }
    }
    if (thesplitter.size()) {
        if (wkeeps.find(thesplitter) == wkeeps.end()) {
            if (punct.size()) {
                thesplitter = thesplitter.substr(0, thesplitter.size() - punct.size());
                sc_unicode_to_utf8(s, thesplitter);
                vect.push_back(s);
                for (int j = 0; j < punct.size(); j++) {
                    s = "";
                    thesplitter = punct[j];
                    s_unicode_to_utf8(s, thesplitter);
                    vect.push_back(s);
                }
            }
            else {
                sc_unicode_to_utf8(s, thesplitter);
                vect.push_back(s);
            }
        }
        else {
            sc_unicode_to_utf8(s, thesplitter);
            vect.push_back(s);
        }
    }
}
//------------------------------------------------------------------------------------------

void getdefaultrules(vector<string>& rules) {
    //Spaces, skipped in the parsing string
    rules.push_back("%s=#");                                                //0     space (not kept)
    rules.push_back("%r=#");                                                //1     cr (can be kept)
    
    rules.push_back("1:{%a %d #. #- #= #@ #% #&}");                         //2 metarule on 1
    rules.push_back("2:{%a %d #% #= ## #& #/ #? #- #@}");                   //3 metarule on 2
    rules.push_back("3:{%a %d #% #= ## #& #/ #? #- #@ #.}");                //4 metarule on 3
    rules.push_back("4:{%d #A-F #a-f}");                                    //2 metarule on 4, for hexadecimal digits
    rules.push_back("5:{%a %d #% #= ## #& #/ #? #-}");                //4 metarule on 3
    
    //Fast tracks for recurrent punctations
    rules.push_back(";=0");                                                 //5     ;
    rules.push_back(",=0");                                                 //6     ,
    rules.push_back("==0");                                                 //7     =
    rules.push_back("~=0");                                                 //8     ~
    rules.push_back("(=0");                                                 //9     (
    rules.push_back(")=0");                                                 //10    )
    rules.push_back("[=0");                                                 //11    [
    rules.push_back("]=0");                                                 //12    ]
    rules.push_back("{=0");                                                 //13    {
    rules.push_back("}=0");                                                 //14    }
    rules.push_back("^=0");                                                 //15    ^
    rules.push_back("*=0");                                                 //16    *
    rules.push_back("%=0");                                                 //17    %
    rules.push_back("<=0");                                                 //18    <
    rules.push_back(">=0");                                                 //19    >
    rules.push_back("|=0");                                                 //20    |
    rules.push_back("&=0");                                                 //21    &
    rules.push_back("$=0");                                                 //22    $
    rules.push_back("\"=0");                                                //24    "
    rules.push_back("'=0");                                                 //23    '
    
    rules.push_back("+0x%4+(.%4+)([p P]([- +])%d+)=0");                         //47 hexadecimal
    rules.push_back("+%d(.%d+)[e E]([- +])%d+=92");                               //25    exponential digits
    rules.push_back("+%d+(.%d+)=93");                                       //26    digits
    rules.push_back("+=0");                                                 //27    +
    
    rules.push_back("-0x%4+(.%4+)([p P]([- +])%d+)=0");                         //47 hexadecimal
    rules.push_back("-%d(.%d+)[e E]([- +])%d+=92");                               //28    exponential digits
    rules.push_back("-%d+(.%d+)=93");                                       //29    digits
    rules.push_back("-=0");                                                 //30    -
    
    rules.push_back("\\+=0");                                               //31    \
    
    rules.push_back(":+=0");                                                //32    :::
    
    rules.push_back(".+=0");                                                //33    ....
    
    rules.push_back("?+=0");                                                //34    ????
    
    rules.push_back("!+=0");                                                //35    !!!!!
    
    rules.push_back("/%1+(/%1+)+=0");                                       //36    UNIX path... (we use metarule %1 to detect which characters are valid)
    rules.push_back("/+=0");                                                //37    /
    
    rules.push_back("#{%a %d %- %_ %@ %+ %=}+=0");                          //38    #digits or hashtag
    rules.push_back("#=0");                                                 //39    #
    
    rules.push_back("http(s)://%2+(.%2+)+=0");                              //40    http (we use metarule %2 to detect which characters are valid)
    
    rules.push_back("0x%4+(.%4+)([p P]([- +])%d+)=0");                         //47 hexadecimal
    rules.push_back("%d+,%d%d%d(,%d%d%d)+=88");                             //42    multi-word expression...
    rules.push_back("%d+.%d%d%d(.%d%d%d)+=88");                             //43    multi-word expression...
    
    rules.push_back("%d(.%d+)e([- +])%d+=91");                                //44    exponential digits
    rules.push_back("%d+(.%d+)=92");                                        //45    digits
    
    // rules start here
    //rules.push_back("%C%a+.=0");                                            //47    e-mail address
    rules.push_back("%5+(.%5+)+@%5+(.%5+)+=0");                                           //47    e-mail address
    rules.push_back("%a:\\%3+(\\%3+)+=0");                                  //46    Windows path...
    rules.push_back("{%a #- %d %H}+=0");                                    //48    label a combination of alpha, digits and hangul characters
    rules.push_back("%n=#");                                                //49    non-breaking space
    rules.push_back("%o=0");                                                //50    operators
    rules.push_back("%p=0");                                                //49    punctuation
    rules.push_back("%.~{%S %p %o}+=0");                                    //51    An unknown UTF8 token separated with spaces, punctuation or operators...
}

static void loadrules(vector<string>& rules, vector<short>& points, vector<short>& separators) {
    getdefaultrules(rules);
    
    //A little hack. When the action is 9x, x is the position of the point into the parsed rule.
    long ipos;
    for (long r=0; r<rules.size();r++) {
        ipos=rules[r].find("=9");
        if (ipos != -1) {
            points.push_back(r);
            points.push_back(rules[r][ipos+2]-48); // this is the position of the point (.) in the rule
        }
        ipos=rules[r].find("=88");
        if (ipos != -1)
            separators.push_back(r);
    }
}

void x_tokenize::resetrules(vector<string>& rules) {
    //A little hack. When the action is 9x, x is the position of the point into the parsed rule.
    points.clear();
    separators.clear();
    
    long ipos;
    for (long r=0; r<rules.size();r++) {
        ipos=rules[r].find("=9");
        if (ipos != -1) {
            points.push_back(r);
            points.push_back(rules[r][ipos+2]-48); // this is the position of the point (.) in the rule
        }
        ipos=rules[r].find("=88");
        if (ipos != -1)
            separators.push_back(r);
    }
}

void x_wtokenize::resetrules(vector<string>& rules) {
    //A little hack. When the action is 9x, x is the position of the point into the parsed rule.
    points.clear();
    separators.clear();
    
    long ipos;
    for (long r=0; r<rules.size();r++) {
        ipos=rules[r].find("=9");
        if (ipos != -1) {
            points.push_back(r);
            points.push_back(rules[r][ipos+2]-48); // this is the position of the point (.) in the rule
        }
        ipos=rules[r].find("=88");
        if (ipos != -1)
            separators.push_back(r);
    }
}

void x_tokenize::setrules() {
    loadrules(rules,points,separators);
}

void x_wtokenize::setrules() {
    loadrules(rules,points,separators);
}


bool v_tokenize(vector<string>& vect, string& thestr, short flags, vector<string>& rules) {
    
    x_tokenize xr;
    
    xr.juststack=true;
    if (rules.size()==0)
        xr.load();
    else {
        xr.resetrules(rules);
        xr.rules = rules;
        string mess;
        if (!xr.parseexternalrules(mess)) {
            vect.push_back(mess);
            return false;
        }
        xr.loaded=true;
    }

    if ((flags&token_comma) == token_comma)
        xr.selectcomma(true);
    else
        xr.selectcomma(false);
    
    if ((flags&token_keeprc) == token_keeprc)
        xr.keeprc(true);
    else
        xr.keeprc(false);
    
    if ((flags&token_separator) == token_separator)
        xr.separator(true);
    else
        xr.separator(false);
    
    xr.tokenize(thestr,false,&vect);
    return true;
}

bool vw_tokenize(vector<wstring>& vect, wstring& ustr, short flags, vector<string>& rules) {
    x_wtokenize xr;
    
    xr.juststack=true;
    if (rules.size()==0)
        xr.load();
    else {
        xr.resetrules(rules);
        xr.rules = rules;
        wstring mess;
        if (!xr.parseexternalrules(mess)) {
            vect.push_back(mess);
            return false;
        }
        xr.loaded=true;
    }

    if ((flags&token_comma) == token_comma)
        xr.selectcomma(true);
    else
        xr.selectcomma(false);
    
    if ((flags&token_keeprc) == token_keeprc)
        xr.keeprc(true);
    else
        xr.keeprc(false);
    
    if ((flags&token_separator) == token_separator)
        xr.separator(true);
    else
        xr.separator(false);
    
    xr.tokenize(ustr,false,&vect);
    return true;
}

void v_tokenize(vector<string>& vect, string& thestr, short flags) {
    
    x_tokenize xr;
    
    xr.juststack=true;
    xr.load();

    if ((flags&token_comma) == token_comma)
        xr.selectcomma(true);
    else
        xr.selectcomma(false);
    
    if ((flags&token_keeprc) == token_keeprc)
        xr.keeprc(true);
    else
        xr.keeprc(false);
    
    if ((flags&token_separator) == token_separator)
        xr.separator(true);
    else
        xr.separator(false);
    
    xr.tokenize(thestr,false,&vect);
}

void vw_tokenize(vector<wstring>& vect, wstring& ustr, short flags) {
    x_wtokenize xr;
    
    xr.juststack=true;
    xr.load();
    
    if ((flags&token_comma) == token_comma)
        xr.selectcomma(true);
    else
        xr.selectcomma(false);
    
    if ((flags&token_keeprc) == token_keeprc)
        xr.keeprc(true);
    else
        xr.keeprc(false);
    
    if ((flags&token_separator) == token_separator)
        xr.separator(true);
    else
        xr.separator(false);
    
    xr.tokenize(ustr,false,&vect);
}

//------------------------------------------------------------------------------------------

Exporting bool s_is_digit(string& str) {
    long lg = str.size();
    long i = 0;
    if (str[0] == '-' || str[0] == '.')
        i++;
    bool digit = false;
    bool dot = false;
    int countdigits = 0;
    for (; i < lg; i++) {
        if (str[i] == '.' || str[i] == ',') {
            if (!digit)
                return false;
            
            if (str[i] == '.') {
                if (dot)
                    return false;
                dot = true;
            }
            else {
                if (countdigits != 3)
                    return false;
            }
            countdigits = 0;
            continue;
        }
        
        if (str[i]<48 || str[i]>57)
            return false;
        countdigits++;
        digit = true;
    }
    
    if (!digit)
        return false;
    return true;
}

Exporting bool s_is_digit(wstring& str) {
    long lg = str.size();
    long i = 0;
    if (str[0] == '-' || str[0] == '.')
        i++;
    bool digit = false;
    bool dot = false;
    int countdigits = 0;
    for (; i < lg; i++) {
        if (str[i] == '.' || str[i] == ',') {
            if (!digit)
                return false;
            
            if (str[i] == '.') {
                if (dot)
                    return false;
                dot = true;
            }
            else {
                if (countdigits != 3)
                    return false;
            }
            countdigits = 0;
            continue;
        }
        
        if (str[i]<48 || str[i]>57)
            return false;
        
        countdigits++;
        digit = true;
    }
    
    if (!digit)
        return false;
    return true;
}

Exporting char s_is_number(uchar* str, char decimal, long& l, double& f) {
    size_t sz = strlen((char*)str);
    double nbdecimal = 1;
    bool founddecimalpoint = false;
    l = 0;
    f = 0;
    for (long i = 0; i < sz; i++) {
        if (isadigit(str[i])) {
            if (!founddecimalpoint)
                l = l * 10 + (str[i] - 48);
            else {
                f = f * 10 + (str[i] - 48);
                nbdecimal *= 10;
            }
            continue;
        }
        if (str[i] == decimal) {
            if (founddecimalpoint)
                return 0;
            founddecimalpoint = true;
            continue;
        }
        return 0;
    }
    if (founddecimalpoint) {
        f = l + (f / nbdecimal);
        return 2;
    }
    return 1;
}

void cr_normalise(string& code) {
    code = s_replacestrings(code, "\r\n", "\n");
    code = s_replacestrings(code, "\r", "\n");
}

char laccolade(string& ligne) {
    long i;
    char trouve = 0;
    long mx = ligne.size();
    unsigned char c;
    bool acc = false;
    for (i = 0; i < mx; i++) {
        c = ligne[i];
        if (c <= 32)
            continue;
        switch (c) {
            case '{':
                acc = true;
                trouve = '{';
                break;
            case '}':
                if (acc)
                    return 0;
                return c;
            case ',':
                trouve = ',';
                break;
            case ':':
                if ((i + 1) < mx && ligne[i + 1] == '-') { //inference rules
                    i++;
                    trouve = '-';
                    break;
                }
            case ')':
                trouve = ')';
                break;
            case '"':
                if (trouve != '{')
                    trouve = 0;
                i++;
                while (i < mx && ligne[i] != '"') {
                    if (ligne[i] == '\\')
                        i++;
                    i++;
                }
                break;
            case '\'':
                if (trouve != '{')
                    trouve = 0;
                i++;
                while (i < mx && ligne[i] != '\'')
                    i++;
                break;
            case ';':
                if (trouve != '{')
                    trouve = 0;
                break;
            case '/':
                if (i < mx - 1 && ligne[i + 1] == '/')
                    return trouve;
            default:
                if (trouve != '{')
                    trouve = 0;
        }
    }
    if (acc && !trouve)
        return '{';
    return trouve;
}

bool TestChaine(string& s, const char* ch, char fin) {
    long i = 0;
    long ln = strlen(ch);
    if (s.size() < ln)
        return false;
    for (; i < ln; i++) {
        if (s[i] != ch[i])
            return false;
    }
    if (i == s.size() || s[i] <= 32 || s[i] == fin || s[i] == '/')
        return true;
    return false;
}

static long blanksize = 4;
Exporting void SetBlankSize(long sz) {
    if (sz >= 1)
        blanksize = sz;
}

Exporting long GetBlankSize() {
    return blanksize;
}

Exporting void v_split_indent(string& thestr, vector<string>& v) {
    //The variable is static to avoid rebuilding it at each call
    static x_forindent xr;
    
    xr.tokenize(thestr);
    
    string value;
    for (long i = 0; i < xr.stack.size(); i++) {
        if (xr.stack[i] == "\n") {
            if (value == "")
                v.push_back("\n");
            else
                v.push_back(Trim(value));
            value = "";
        }
        else {
            if (xr.stack[i].back() == '\n') {
                if (Trim(value) != "")
                    v.push_back(value);
                v.push_back(Trim(xr.stack[i]));
            }
            else
                value += xr.stack[i];
        }
    }
    
    Trim(value);
    if (value != "")
        v.push_back(value);
    else
        v.push_back("\n");
}
    
Exporting void IndentationCode(string& str, string& codeindente, bool lisp, bool taskel) {
    string token;

    uchar* codestr = USTR(str);
    char* blanks;
    
    vector<long> pos;
    long szstr = str.size();
    long r = 0;
    long counterlisp = 0;
    long sz, i, p = 0;
    long iblank = 0;
    long iparenthesis = 0;
    long ligne = 0;
        
    short l, taskelmode = 0;
    short addspace = 0;
    short checkspace = 0;
    
    bool locallisp = false;
    bool prologmode = false;
    bool consumeblanks = true;
    bool beginning = true;
    bool equalsign = false;
    
    uchar c;

    split_container(codestr, szstr, pos, true);
    sz = pos.size();

    for (i = 0; i < sz; i++) {
        c = codestr[pos[i]];
        if (strchr("({[",c)) {
            counterlisp++;
            if (p < counterlisp)
                p = counterlisp;
        }
        else
            if (strchr(")]}",c))
                counterlisp--;
    }
    
    p += 10;
    p *= blanksize;
    string _blanks(p, ' ');
    blanks = STR(_blanks);

    counterlisp = 0;
    i = 0;
    
    while (r < sz) {
        c = codestr[i++];
        
        if (c <= 32) {
            //here we have a CR, the next line should see its white characters being replaced with out indentation pack
            if (c == '\n') {
                codeindente += c;
                consumeblanks = true;
                r++;
                ligne++;
                continue;
            }
            //this is a line beginning, we need to remove the blanks first
            if (consumeblanks)
                continue;
            codeindente += c;
            continue;
        }
        
        beginning = false;
        if (consumeblanks) {
            beginning = true;
            if (!strchr(")]}>", c)) {
                l = iblank;
                switch (checkspace) {
                    case 0:
                        addspace = 0;
                        break;
                    case 1:
                        if (addspace)
                            addspace--;
                        checkspace = 0;
                        break;
                    case 2:
                        checkspace = 1;
                        break;
                    case 3:
                        iblank+=blanksize;
                        checkspace = 1;
                        break;
                    case 4:
                        checkspace = 5;
                        break;
                    case 5:
                    case 6:
                        iblank -= blanksize;
                        break;
                }

                //we need to insert our blanks before...
                if (addspace)
                    iblank += blanksize*addspace;
                if (iblank) {
                    blanks[iblank] = 0;
                    codeindente += blanks;
                    blanks[iblank] = 32;
                }
                iblank = l;
                consumeblanks = false;
            }
        }

        equalsign = (c == '<') ? equalsign : false;
        
        if (i != pos[r] + 1) {
            if (c < 'A') {
                if (c == '.' && prologmode) {
                    iblank -= blanksize;
                    prologmode = false;
                }
                codeindente += c;
                continue;
            }
            
            p = i;
            while (codestr[p] > 32 && p < pos[r]) p++;
            c = codestr[p];
            codestr[p] = 0;
            token = (char*)codestr+i-1;
            codestr[p] = c;
            if (!lisp && !taskelmode) {
                if (token == "case") {
                    if (checkspace == 5) {
                        //we need to remove an extra blank size;
                        codeindente = codeindente.substr(0, codeindente.size()-blanksize);
                        checkspace = 6;
                    }
                    else {
                        if (checkspace < 5) {
                            checkspace = 4;
                            addspace++;
                        }
                    }
                }
                else {
                    if (token == "else") {
                        checkspace = 3;
                    }
                    else {
                        if (token == "if" || token == "elif" || token == "for" || token == "while" || token == "default") {
                            if (checkspace == 6) {
                                //extra space missing
                                string space(blanksize, ' ');
                                codeindente += space;
                            }
                                
                            checkspace = 2;
                            addspace++;
                        }
                        else {
                            if (prologmode && p > 0 && codestr[p-1] == '.') {
                                iblank -= blanksize;
                                prologmode = false;
                            }
                        }
                    }
                }
            }
            codeindente += token;
            i = p;
            continue;
        }
        
        r++;
        switch (c) {
            case ';':
                if (checkspace == 2) {
                    addspace--;
                    checkspace = 0;
                }
                else
                    if (checkspace == 3)
                        checkspace = 0;
                codeindente += c;
                break;
            case ':':
                if (!iblank && codestr[i] == '-') {
                    prologmode = true;
                    iblank += blanksize;
                }
                codeindente += c;
                break;
            case '/':
                switch (codestr[i]) {
                    case '/':
                        p = i + 1;
                        //this is a comment, up to the last CR
                        r++; //the next element in pos is also a '/'
                        while (r < sz) {
                            p = pos[r++];
                            if (codestr[p] == '\n') {
                                r--; //it will be consumed later
                                break;
                            }
                        }
                        c = codestr[p];
                        codestr[p] = 0;
                        codeindente += (char*)codestr+i-1;
                        codestr[p] = c;
                        i = p;
                        break;
                    case '@':
                        p = i;
                        //this is a long comment...
                        while (r < sz) {
                            p = pos[r++];
                            if (codestr[p-1] == '@' && codestr[p] == '/')
                                break;
                        }
                        p++;
                        c = codestr[p];
                        codestr[p] = 0;
                        codeindente += (char*)codestr+i-1;
                        codestr[p] = c;
                        i = p;
                        break;
                    case '*': // /*C comments*/
                        p = i;
                        //this is a long comment...
                        while (r < sz) {
                            p = pos[r++];
                            if (codestr[p-1] == '*' && codestr[p] == '/')
                                break;
                        }
                        p++;
                        c = codestr[p];
                        codestr[p] = 0;
                        codeindente += (char*)codestr+i-1;
                        codestr[p] = c;
                        i = p;
                        break;
                    default:
                        codeindente += c;
                }
                break;
            case '=':
                equalsign = true;
                codeindente += c;
                break;
            case '@':
                if (codestr[i] == '"') {
                    p = i;
                    while (r < sz) {
                        p = pos[r++];
                        if (codestr[p-1] != '\\' && codestr[p] == '"' && codestr[p+1] == '@') {
                            r++;
                            break;
                        }
                    }
                    p+=2;
                    c = codestr[p];
                    codestr[p] = 0;
                    codeindente += (char*)codestr+i-1;
                    codestr[p] = c;
                    i = p;
                }
                else
                    codeindente += c;
                break;
            case '"':
                p = i;
                while (r < sz) {
                    p = pos[r++];
                    if ((codestr[p-1] != '\\' && codestr[p] == '"') || codestr[p] == '\n')
                        break;
                }
                p++;
                c = codestr[p];
                codestr[p] = 0;
                codeindente += (char*)codestr+i-1;
                codestr[p] = c;
                i = p;
                break;
            case '\'':
                if (lisp) {
                    codeindente += c;
                    break;
                }
                else {
                    if (i > 1 && codestr[i-2] == '\\' && codestr[i] == '(') {
                        locallisp = true;
                        lisp = true;
                        codeindente += c;
                        break;
                    }
                }
                p = i;
                while (r < sz) {
                    p = pos[r++];
                    if (codestr[p] == '\'' || codestr[p] == '\n')
                        break;
                }
                p++;
                c = codestr[p];
                codestr[p] = 0;
                codeindente += (char*)codestr+i-1;
                codestr[p] = c;
                i = p;
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                noconversionfloathexa((const char*)codestr+i-1, l);
                p =  i + l - 1;
                while (pos[r] < p) r++;
                c = codestr[p];
                codestr[p] = 0;
                codeindente += (char*)codestr+i-1;
                codestr[p] = c;
                i = p;
                break;
            case '{':
                if (!iparenthesis) { //a little hack to handle {}
                    checkspace = 0;
                    addspace = 0;
                    iblank += blanksize;
                }
                codeindente += c;
                break;
            case '<':
                if (taskel && (beginning || equalsign) && !iparenthesis && codestr[i] != '-') {
                    taskelmode++;
                    codeindente += c;
                    iblank += blanksize;
                }
                else
                    codeindente += c;
                break;
            case '(':
                iparenthesis++;
                if (locallisp)
                    counterlisp++;
                else {
                    if (!lisp && i > 1 && codestr[i-2] == '\\') {
                        locallisp = true;
                        lisp = true;
                        counterlisp = 1;
                    }
                }
            case '[':
                codeindente += c;
                iblank += blanksize;
                break;
            case '>':
                if (taskel && !iparenthesis && taskelmode) {
                    taskelmode--;
                    iblank -= blanksize;
                    if (iblank < 0)
                        iblank = 0;
                    if (consumeblanks) {
                        l = iblank;
                        //we need to insert our blanks before...
                        if (addspace)
                            iblank += blanksize*addspace;
                        if (iblank) {
                            blanks[iblank] = 0;
                            codeindente += blanks;
                            blanks[iblank] = 32;
                        }
                        addspace = 0;
                        iblank = l;
                        consumeblanks = false;
                    }
                }
                codeindente += c;
                break;
            case ')':
                iparenthesis--;
                if (locallisp) {
                    counterlisp--;
                    if (!counterlisp) {
                        locallisp = false;
                        lisp = false;
                    }
                }
                iblank -= blanksize;
                if (iblank < 0)
                    iblank = 0;
                if (consumeblanks) {
                    l = iblank;
                    //we need to insert our blanks before...
                    if (addspace)
                        iblank += blanksize*addspace;
                    if (iblank) {
                        blanks[iblank] = 0;
                        codeindente += blanks;
                        blanks[iblank] = 32;
                    }
                    addspace = 0;
                    iblank = l;
                    consumeblanks = false;
                }
                codeindente += c;
                break;
            case '}':
                if (iparenthesis) {
                    codeindente += c;
                    break;
                }
                checkspace = 0;
                addspace = 0;
            case ']':
                iblank -= blanksize;
                if (iblank < 0)
                    iblank = 0;
                if (consumeblanks) {
                    l = iblank;
                    //we need to insert our blanks before...
                    if (addspace)
                        iblank += blanksize*addspace;
                    if (iblank) {
                        blanks[iblank] = 0;
                        codeindente += blanks;
                        blanks[iblank] = 32;
                    }
                    addspace = 0;
                    iblank = l;
                    consumeblanks = false;
                }
                codeindente += c;
                break;
            default:
                codeindente += c;
        }
    }
    
    for (;i < szstr; i++)
        codeindente += codestr[i];
}
    
Exporting long IndentationCode(string& codestr, bool lisp) {
    static vector<long> pos;
    pos.clear();
    string token;

    long szstr = codestr.size();
    split_container(USTR(codestr), szstr, pos, true);

    long sz = pos.size();
    long r = 0;
    long i = 0;
    long iblank = 0;
    long counterlisp = 0;
    long p;
    long finalblank = 0;

    short addspace = 0;
    short checkspace = 0;
    short iparenthesis = 0;
    short taskelmode = 0;
    short l;

    bool locallisp = false;
    bool prologmode = false;
    bool beginning = true;
    bool equalsign = false;
    bool consumeblanks = true;

    uchar c = 0;

    counterlisp = 0;
    i = 0;
    
    while (r < sz) {
        c = codestr[i++];
        
        if (c <= 32) {
            //here we have a CR, the next line should see its white characters being replaced with out indentation pack
            if (c == '\n') {
                consumeblanks = true;
                r++;
                continue;
            }
            //this is a line beginning, we need to remove the blanks first
            if (consumeblanks)
                continue;
            continue;
        }
        
        beginning = false;
        if (consumeblanks) {
            beginning = true;
            if (!strchr(")]}>", c)) {
                l = iblank;
                switch (checkspace) {
                    case 0:
                        addspace = 0;
                        break;
                    case 1:
                        if (addspace)
                            addspace--;
                        checkspace = 0;
                        break;
                    case 2:
                        checkspace = 1;
                        break;
                    case 3:
                        iblank+=blanksize;
                        checkspace = 1;
                        break;
                    case 4:
                        checkspace = 5;
                        break;
                    case 5:
                    case 6:
                        iblank -= blanksize;
                        break;
                }

                //we need to insert our blanks before...
                if (addspace)
                    iblank += blanksize*addspace;
                if (iblank) {
                    finalblank = iblank;
                }
                else
                    finalblank = 0;
                iblank = l;
                consumeblanks = false;
            }
        }

        equalsign = (c == '<') ? equalsign : false;
        
        if (i != pos[r] + 1) {
            if (c < 'A') {
                if (c == '.' && prologmode) {
                    iblank -= blanksize;
                    prologmode = false;
                }
                continue;
            }
            
            p = i;
            while (codestr[p] > 32 && p < pos[r]) p++;
            c = codestr[p];
            codestr[p] = 0;
            token = (char*)STR(codestr)+i-1;
            codestr[p] = c;
            if (!lisp && !taskelmode) {
                if (token == "case") {
                    if (checkspace == 5) {
                        //we need to remove an extra blank size;
                        checkspace = 6;
                    }
                    else {
                        if (checkspace < 5) {
                            checkspace = 4;
                            addspace++;
                        }
                    }
                }
                else {
                    if (token == "else") {
                        checkspace = 3;
                    }
                    else {
                        if (token == "if" || token == "elif" || token == "for" || token == "while" || token == "default") {
                            if (checkspace == 6) {
                                //extra space missing
                                finalblank = blanksize;
                            }
                                
                            checkspace = 2;
                            addspace++;
                        }
                        else {
                            if (prologmode && p > 0 && codestr[p-1] == '.') {
                                iblank -= blanksize;
                                prologmode = false;
                            }
                        }
                    }
                }
            }
            i = p;
            continue;
        }
        
        r++;
        switch (c) {
            case ';':
                if (checkspace == 2) {
                    addspace--;
                    checkspace = 0;
                }
                else
                    if (checkspace == 3)
                        checkspace = 0;
                break;
            case ':':
                if (!iblank && codestr[i] == '-') {
                    prologmode = true;
                    iblank += blanksize;
                }
                break;
            case '/':
                switch (codestr[i]) {
                    case '/':
                        p = i + 1;
                        //this is a comment, up to the last CR
                        r++; //the next element in pos is also a '/'
                        while (r < sz) {
                            p = pos[r++];
                            if (codestr[p] == '\n') {
                                r--; //it will be consumed later
                                break;
                            }
                        }
                        i = p;
                        break;
                    case '@':
                        p = i;
                        //this is a long comment...
                        while (r < sz) {
                            p = pos[r++];
                            if (codestr[p-1] == '@' && codestr[p] == '/')
                                break;
                        }
                        p++;
                        i = p;
                        break;
                    case '*': // /*C comments*/
                        p = i;
                        //this is a long comment...
                        while (r < sz) {
                            p = pos[r++];
                            if (codestr[p-1] == '*' && codestr[p] == '/')
                                break;
                        }
                        p++;
                        i = p;
                        break;
                }
                break;
            case '=':
                equalsign = true;
                break;
            case '@':
                if (codestr[i] == '"') {
                    p = i;
                    while (r < sz) {
                        p = pos[r++];
                        if (codestr[p-1] != '\\' && codestr[p] == '"' && codestr[p+1] == '@') {
                            r++;
                            break;
                        }
                    }
                    p+=2;
                    i = p;
                }
                break;
            case '"':
                p = i;
                while (r < sz) {
                    p = pos[r++];
                    if ((codestr[p-1] != '\\' && codestr[p] == '"') || codestr[p] == '\n')
                        break;
                }
                p++;
                i = p;
                break;
            case '\'':
                if (lisp) {
                    break;
                }
                else {
                    if (i > 1 && codestr[i-2] == '\\' && codestr[i] == '(') {
                        locallisp = true;
                        lisp = true;
                        break;
                    }
                }
                p = i;
                while (r < sz) {
                    p = pos[r++];
                    if (codestr[p] == '\'' || codestr[p] == '\n')
                        break;
                }
                p++;
                i = p;
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                noconversionfloathexa((const char*)STR(codestr)+i-1, l);
                p =  i + l - 1;
                while (pos[r] < p) r++;
                i = p;
                break;
            case '{':
                if (!iparenthesis) { //a little hack to handle {}
                    checkspace = 0;
                    addspace = 0;
                    iblank += blanksize;
                }
                break;
            case '<':
                if ((beginning || equalsign) && !iparenthesis && codestr[i] != '-') {
                    taskelmode++;
                    iblank += blanksize;
                }
                break;
            case '(':
                iparenthesis++;
                if (locallisp)
                    counterlisp++;
                else {
                    if (!lisp && i > 1 && codestr[i-2] == '\\') {
                        locallisp = true;
                        lisp = true;
                        counterlisp = 1;
                    }
                }
            case '[':
                iblank += blanksize;
                break;
            case '>':
                if (!iparenthesis && taskelmode) {
                    taskelmode--;
                    iblank -= blanksize;
                    if (iblank < 0)
                        iblank = 0;
                    if (consumeblanks) {
                        l = iblank;
                        //we need to insert our blanks before...
                        if (addspace)
                            iblank += blanksize*addspace;
                        if (iblank) {
                            finalblank = iblank;
                        }
                        else
                            finalblank = 0;

                        addspace = 0;
                        iblank = l;
                        consumeblanks = false;
                    }
                }
                break;
            case ')':
                iparenthesis--;
                if (locallisp) {
                    counterlisp--;
                    if (!counterlisp) {
                        locallisp = false;
                        lisp = false;
                    }
                }
                iblank -= blanksize;
                if (iblank < 0)
                    iblank = 0;
                if (consumeblanks) {
                    l = iblank;
                    //we need to insert our blanks before...
                    if (addspace)
                        iblank += blanksize*addspace;
                    if (iblank) {
                        finalblank = iblank;
                    }
                    else
                        finalblank = 0;
                    addspace = 0;
                    iblank = l;
                    consumeblanks = false;
                }
                break;
            case '}':
                if (iparenthesis) {
                    break;
                }
                checkspace = 0;
                addspace = 0;
            case ']':
                iblank -= blanksize;
                if (iblank < 0)
                    iblank = 0;
                if (consumeblanks) {
                    l = iblank;
                    //we need to insert our blanks before...
                    if (addspace)
                        iblank += blanksize*addspace;
                    if (iblank) {
                        finalblank = iblank;
                    }
                    else
                        finalblank = 0;
                    addspace = 0;
                    iblank = l;
                    consumeblanks = false;
                }
                break;
        }
    }

    if (consumeblanks) {
        switch (checkspace) {
            case 0:
                addspace = 0;
                break;
            case 1:
                if (addspace)
                    addspace--;
                checkspace = 0;
                break;
            case 2:
                checkspace = 1;
                break;
            case 3:
                iblank+=blanksize;
                checkspace = 1;
                break;
            case 4:
                checkspace = 5;
                break;
            case 5:
            case 6:
                iblank -= blanksize;
                break;
        }
        
        //we need to insert our blanks before...
        if (addspace)
            iblank += blanksize*addspace;
        if (iblank) {
            finalblank = iblank;
        }
        else
            finalblank = 0;
    }
    return finalblank;
}

Exporting void IndentCode(string& codestr, string& codeindente, long blancs, bool lisp, bool taskel) {
    if (!lisp) {
        if (codestr[0] == '(' && codestr[1] == ')') {
            lisp = true;
        }
    }
    
    long bl = blanksize;
    if (blancs)
        blanksize = blancs;
    codeindente = "";
    IndentationCode(codestr, codeindente, lisp, taskel);
    blanksize = bl;
    if (codeindente.find("/@") != string::npos || codeindente.find("@\"") != string::npos)
        cr_normalise(codeindente);
    
    Trimright(codeindente);
    codeindente += "\n";
}
    
Exporting long VirtualIndentation(string& codestr) {
    Trimright(codestr);
    codestr += "\n";
    cr_normalise(codestr);
    bool lisp = false;
    if (codestr[0] == '(' && codestr[1] == ')')
        lisp = true;
    return IndentationCode(codestr, lisp);
}

//----------------------------------------------------------------------------------------------------------------------

static TAMGUCHAR koreans[] = {
    4352, 4353, 4354, 4355, 4356, 4357, 4358, 4359, 4360, 4361, 4362, 4363, 4364, 4365, 4366, 4367, 4368, 4369, 4370, 4371, 4372, 4373,
    4374, 4375, 4376, 4377, 4378, 4379, 4380, 4381, 4382, 4383, 4384, 4385, 4386, 4387, 4388, 4389, 4390, 4391, 4392, 4393, 4394, 4395,
    4396, 4397, 4398, 4399, 4400, 4401, 4402, 4403, 4404, 4405, 4406, 4407, 4408, 4409, 4410, 4411, 4412, 4413, 4414, 4415, 4416, 4417,
    4418, 4419, 4420, 4421, 4422, 4423, 4424, 4425, 4426, 4427, 4428, 4429, 4430, 4431, 4432, 4433, 4434, 4435, 4436, 4437, 4438, 4439,
    4440, 4441, 4449, 4450, 4451, 4452, 4453, 4454, 4455, 4456, 4457, 4458, 4459, 4460, 4461, 4462, 4463, 4464, 4465, 4466, 4467, 4468,
    4469, 4470, 4471, 4472, 4473, 4474, 4475, 4476, 4477, 4478, 4479, 4480, 4481, 4482, 4483, 4484, 4485, 4486, 4487, 4488, 4489, 4490,
    4491, 4492, 4493, 4494, 4495, 4496, 4497, 4498, 4499, 4500, 4501, 4502, 4503, 4504, 4505, 4506, 4507, 4508, 4509, 4510, 4511, 4512,
    4513, 4514, 4520, 4521, 4522, 4523, 4524, 4525, 4526, 4527, 4528, 4529, 4530, 4531, 4532, 4533, 4534, 4535, 4536, 4537, 4538, 4539,
    4540, 4541, 4542, 4543, 4544, 4545, 4546, 4547, 4548, 4549, 4550, 4551, 4552, 4553, 4554, 4555, 4556, 4557, 4558, 4559, 4560, 4561,
    4562, 4563, 4564, 4565, 4566, 4567, 4568, 4569, 4570, 4571, 4572, 4573, 4574, 4575, 4576, 4577, 4578, 4579, 4580, 4581, 4582, 4583,
    4584, 4585, 4586, 4587, 4588, 4589, 4590, 4591, 4592, 4593, 4594, 4595, 4596, 4597, 4598, 4599, 4600, 4601, 12593, 12594, 12595, 12596,
    12597, 12598, 12599, 12600, 12601, 12602, 12603, 12604, 12605, 12606, 12607, 12608, 12609, 12610, 12611, 12612, 12613, 12614, 12615, 12616,
    12617, 12618, 12619, 12620, 12621, 12622, 12623, 12624, 12625, 12626, 12627, 12628, 12629, 12630, 12631, 12632, 12633, 12634, 12635, 12636,
    12637, 12638, 12639, 12640, 12641, 12642, 12643, 0 };

static TAMGUCHAR jamo[] = {
    44032, 4352, 4449, 0, 44033, 4352, 4449, 4520, 44034, 4352, 4449, 4521, 44035, 4352, 4449, 4522, 44036, 4352, 4449, 4523, 44037, 4352, 4449, 4524,
    44038, 4352, 4449, 4525, 44039, 4352, 4449, 4526, 44040, 4352, 4449, 4527, 44041, 4352, 4449, 4528, 44042, 4352, 4449, 4529, 44043, 4352, 4449, 4530,
    44044, 4352, 4449, 4531, 44045, 4352, 4449, 4532, 44046, 4352, 4449, 4533, 44047, 4352, 4449, 4534, 44048, 4352, 4449, 4535, 44049, 4352, 4449, 4536,
    44050, 4352, 4449, 12612, 44051, 4352, 4449, 4538, 44052, 4352, 4449, 4539, 44053, 4352, 4449, 4540, 44054, 4352, 4449, 4364, 44055, 4352, 4449, 4542,
    44056, 4352, 4449, 4543, 44057, 4352, 4449, 4544, 44058, 4352, 4449, 4545, 44059, 4352, 4449, 4546, 44060, 4352, 4450, 0, 44061, 4352, 4450, 4520,
    44062, 4352, 4450, 4521, 44063, 4352, 4450, 4522, 44064, 4352, 4450, 4523, 44065, 4352, 4450, 4524, 44066, 4352, 4450, 4525, 44067, 4352, 4450, 4526,
    44068, 4352, 4450, 4527, 44069, 4352, 4450, 4528, 44070, 4352, 4450, 4529, 44071, 4352, 4450, 4530, 44072, 4352, 4450, 4531, 44073, 4352, 4450, 4532,
    44074, 4352, 4450, 4533, 44075, 4352, 4450, 4534, 44076, 4352, 4450, 4535, 44077, 4352, 4450, 4536, 44078, 4352, 4450, 12612, 44079, 4352, 4450, 4538,
    44080, 4352, 4450, 4539, 44081, 4352, 4450, 4540, 44082, 4352, 4450, 4364, 44083, 4352, 4450, 4542, 44084, 4352, 4450, 4543, 44085, 4352, 4450, 4544,
    44086, 4352, 4450, 4545, 44087, 4352, 4450, 4546, 44088, 4352, 4451, 0, 44089, 4352, 4451, 4520, 44090, 4352, 4451, 4521, 44091, 4352, 4451, 4522,
    44092, 4352, 4451, 4523, 44093, 4352, 4451, 4524, 44094, 4352, 4451, 4525, 44095, 4352, 4451, 4526, 44096, 4352, 4451, 4527, 44097, 4352, 4451, 4528,
    44098, 4352, 4451, 4529, 44099, 4352, 4451, 4530, 44100, 4352, 4451, 4531, 44101, 4352, 4451, 4532, 44102, 4352, 4451, 4533, 44103, 4352, 4451, 4534,
    44104, 4352, 4451, 4535, 44105, 4352, 4451, 4536, 44106, 4352, 4451, 12612, 44107, 4352, 4451, 4538, 44108, 4352, 4451, 4539, 44109, 4352, 4451, 4540,
    44110, 4352, 4451, 4364, 44111, 4352, 4451, 4542, 44112, 4352, 4451, 4543, 44113, 4352, 4451, 4544, 44114, 4352, 4451, 4545, 44115, 4352, 4451, 4546,
    44116, 4352, 4452, 0, 44117, 4352, 4452, 4520, 44118, 4352, 4452, 4521, 44119, 4352, 4452, 4522, 44120, 4352, 4452, 4523, 44121, 4352, 4452, 4524,
    44122, 4352, 4452, 4525, 44123, 4352, 4452, 4526, 44124, 4352, 4452, 4527, 44125, 4352, 4452, 4528, 44126, 4352, 4452, 4529, 44127, 4352, 4452, 4530,
    44128, 4352, 4452, 4531, 44129, 4352, 4452, 4532, 44130, 4352, 4452, 4533, 44131, 4352, 4452, 4534, 44132, 4352, 4452, 4535, 44133, 4352, 4452, 4536,
    44134, 4352, 4452, 12612, 44135, 4352, 4452, 4538, 44136, 4352, 4452, 4539, 44137, 4352, 4452, 4540, 44138, 4352, 4452, 4364, 44139, 4352, 4452, 4542,
    44140, 4352, 4452, 4543, 44141, 4352, 4452, 4544, 44142, 4352, 4452, 4545, 44143, 4352, 4452, 4546, 44144, 4352, 4453, 0, 44145, 4352, 4453, 4520,
    44146, 4352, 4453, 4521, 44147, 4352, 4453, 4522, 44148, 4352, 4453, 4523, 44149, 4352, 4453, 4524, 44150, 4352, 4453, 4525, 44151, 4352, 4453, 4526,
    44152, 4352, 4453, 4527, 44153, 4352, 4453, 4528, 44154, 4352, 4453, 4529, 44155, 4352, 4453, 4530, 44156, 4352, 4453, 4531, 44157, 4352, 4453, 4532,
    44158, 4352, 4453, 4533, 44159, 4352, 4453, 4534, 44160, 4352, 4453, 4535, 44161, 4352, 4453, 4536, 44162, 4352, 4453, 12612, 44163, 4352, 4453, 4538,
    44164, 4352, 4453, 4539, 44165, 4352, 4453, 4540, 44166, 4352, 4453, 4364, 44167, 4352, 4453, 4542, 44168, 4352, 4453, 4543, 44169, 4352, 4453, 4544,
    44170, 4352, 4453, 4545, 44171, 4352, 4453, 4546, 44172, 4352, 4454, 0, 44173, 4352, 4454, 4520, 44174, 4352, 4454, 4521, 44175, 4352, 4454, 4522,
    44176, 4352, 4454, 4523, 44177, 4352, 4454, 4524, 44178, 4352, 4454, 4525, 44179, 4352, 4454, 4526, 44180, 4352, 4454, 4527, 44181, 4352, 4454, 4528,
    44182, 4352, 4454, 4529, 44183, 4352, 4454, 4530, 44184, 4352, 4454, 4531, 44185, 4352, 4454, 4532, 44186, 4352, 4454, 4533, 44187, 4352, 4454, 4534,
    44188, 4352, 4454, 4535, 44189, 4352, 4454, 4536, 44190, 4352, 4454, 12612, 44191, 4352, 4454, 4538, 44192, 4352, 4454, 4539, 44193, 4352, 4454, 4540,
    44194, 4352, 4454, 4364, 44195, 4352, 4454, 4542, 44196, 4352, 4454, 4543, 44197, 4352, 4454, 4544, 44198, 4352, 4454, 4545, 44199, 4352, 4454, 4546,
    44200, 4352, 4455, 0, 44201, 4352, 4455, 4520, 44202, 4352, 4455, 4521, 44203, 4352, 4455, 4522, 44204, 4352, 4455, 4523, 44205, 4352, 4455, 4524,
    44206, 4352, 4455, 4525, 44207, 4352, 4455, 4526, 44208, 4352, 4455, 4527, 44209, 4352, 4455, 4528, 44210, 4352, 4455, 4529, 44211, 4352, 4455, 4530,
    44212, 4352, 4455, 4531, 44213, 4352, 4455, 4532, 44214, 4352, 4455, 4533, 44215, 4352, 4455, 4534, 44216, 4352, 4455, 4535, 44217, 4352, 4455, 4536,
    44218, 4352, 4455, 12612, 44219, 4352, 4455, 4538, 44220, 4352, 4455, 4539, 44221, 4352, 4455, 4540, 44222, 4352, 4455, 4364, 44223, 4352, 4455, 4542,
    44224, 4352, 4455, 4543, 44225, 4352, 4455, 4544, 44226, 4352, 4455, 4545, 44227, 4352, 4455, 4546, 44228, 4352, 4456, 0, 44229, 4352, 4456, 4520,
    44230, 4352, 4456, 4521, 44231, 4352, 4456, 4522, 44232, 4352, 4456, 4523, 44233, 4352, 4456, 4524, 44234, 4352, 4456, 4525, 44235, 4352, 4456, 4526,
    44236, 4352, 4456, 4527, 44237, 4352, 4456, 4528, 44238, 4352, 4456, 4529, 44239, 4352, 4456, 4530, 44240, 4352, 4456, 4531, 44241, 4352, 4456, 4532,
    44242, 4352, 4456, 4533, 44243, 4352, 4456, 4534, 44244, 4352, 4456, 4535, 44245, 4352, 4456, 4536, 44246, 4352, 4456, 12612, 44247, 4352, 4456, 4538,
    44248, 4352, 4456, 4539, 44249, 4352, 4456, 4540, 44250, 4352, 4456, 4364, 44251, 4352, 4456, 4542, 44252, 4352, 4456, 4543, 44253, 4352, 4456, 4544,
    44254, 4352, 4456, 4545, 44255, 4352, 4456, 4546, 44256, 4352, 4457, 0, 44257, 4352, 4457, 4520, 44258, 4352, 4457, 4521, 44259, 4352, 4457, 4522,
    44260, 4352, 4457, 4523, 44261, 4352, 4457, 4524, 44262, 4352, 4457, 4525, 44263, 4352, 4457, 4526, 44264, 4352, 4457, 4527, 44265, 4352, 4457, 4528,
    44266, 4352, 4457, 4529, 44267, 4352, 4457, 4530, 44268, 4352, 4457, 4531, 44269, 4352, 4457, 4532, 44270, 4352, 4457, 4533, 44271, 4352, 4457, 4534,
    44272, 4352, 4457, 4535, 44273, 4352, 4457, 4536, 44274, 4352, 4457, 12612, 44275, 4352, 4457, 4538, 44276, 4352, 4457, 4539, 44277, 4352, 4457, 4540,
    44278, 4352, 4457, 4364, 44279, 4352, 4457, 4542, 44280, 4352, 4457, 4543, 44281, 4352, 4457, 4544, 44282, 4352, 4457, 4545, 44283, 4352, 4457, 4546,
    44284, 4352, 4458, 0, 44285, 4352, 4458, 4520, 44286, 4352, 4458, 4521, 44287, 4352, 4458, 4522, 44288, 4352, 4458, 4523, 44289, 4352, 4458, 4524,
    44290, 4352, 4458, 4525, 44291, 4352, 4458, 4526, 44292, 4352, 4458, 4527, 44293, 4352, 4458, 4528, 44294, 4352, 4458, 4529, 44295, 4352, 4458, 4530,
    44296, 4352, 4458, 4531, 44297, 4352, 4458, 4532, 44298, 4352, 4458, 4533, 44299, 4352, 4458, 4534, 44300, 4352, 4458, 4535, 44301, 4352, 4458, 4536,
    44302, 4352, 4458, 12612, 44303, 4352, 4458, 4538, 44304, 4352, 4458, 4539, 44305, 4352, 4458, 4540, 44306, 4352, 4458, 4364, 44307, 4352, 4458, 4542,
    44308, 4352, 4458, 4543, 44309, 4352, 4458, 4544, 44310, 4352, 4458, 4545, 44311, 4352, 4458, 4546, 44312, 4352, 4459, 0, 44313, 4352, 4459, 4520,
    44314, 4352, 4459, 4521, 44315, 4352, 4459, 4522, 44316, 4352, 4459, 4523, 44317, 4352, 4459, 4524, 44318, 4352, 4459, 4525, 44319, 4352, 4459, 4526,
    44320, 4352, 4459, 4527, 44321, 4352, 4459, 4528, 44322, 4352, 4459, 4529, 44323, 4352, 4459, 4530, 44324, 4352, 4459, 4531, 44325, 4352, 4459, 4532,
    44326, 4352, 4459, 4533, 44327, 4352, 4459, 4534, 44328, 4352, 4459, 4535, 44329, 4352, 4459, 4536, 44330, 4352, 4459, 12612, 44331, 4352, 4459, 4538,
    44332, 4352, 4459, 4539, 44333, 4352, 4459, 4540, 44334, 4352, 4459, 4364, 44335, 4352, 4459, 4542, 44336, 4352, 4459, 4543, 44337, 4352, 4459, 4544,
    44338, 4352, 4459, 4545, 44339, 4352, 4459, 4546, 44340, 4352, 4460, 0, 44341, 4352, 4460, 4520, 44342, 4352, 4460, 4521, 44343, 4352, 4460, 4522,
    44344, 4352, 4460, 4523, 44345, 4352, 4460, 4524, 44346, 4352, 4460, 4525, 44347, 4352, 4460, 4526, 44348, 4352, 4460, 4527, 44349, 4352, 4460, 4528,
    44350, 4352, 4460, 4529, 44351, 4352, 4460, 4530, 44352, 4352, 4460, 4531, 44353, 4352, 4460, 4532, 44354, 4352, 4460, 4533, 44355, 4352, 4460, 4534,
    44356, 4352, 4460, 4535, 44357, 4352, 4460, 4536, 44358, 4352, 4460, 12612, 44359, 4352, 4460, 4538, 44360, 4352, 4460, 4539, 44361, 4352, 4460, 4540,
    44362, 4352, 4460, 4364, 44363, 4352, 4460, 4542, 44364, 4352, 4460, 4543, 44365, 4352, 4460, 4544, 44366, 4352, 4460, 4545, 44367, 4352, 4460, 4546,
    44368, 4352, 4461, 0, 44369, 4352, 4461, 4520, 44370, 4352, 4461, 4521, 44371, 4352, 4461, 4522, 44372, 4352, 4461, 4523, 44373, 4352, 4461, 4524,
    44374, 4352, 4461, 4525, 44375, 4352, 4461, 4526, 44376, 4352, 4461, 4527, 44377, 4352, 4461, 4528, 44378, 4352, 4461, 4529, 44379, 4352, 4461, 4530,
    44380, 4352, 4461, 4531, 44381, 4352, 4461, 4532, 44382, 4352, 4461, 4533, 44383, 4352, 4461, 4534, 44384, 4352, 4461, 4535, 44385, 4352, 4461, 4536,
    44386, 4352, 4461, 12612, 44387, 4352, 4461, 4538, 44388, 4352, 4461, 4539, 44389, 4352, 4461, 4540, 44390, 4352, 4461, 4364, 44391, 4352, 4461, 4542,
    44392, 4352, 4461, 4543, 44393, 4352, 4461, 4544, 44394, 4352, 4461, 4545, 44395, 4352, 4461, 4546, 44396, 4352, 4462, 0, 44397, 4352, 4462, 4520,
    44398, 4352, 4462, 4521, 44399, 4352, 4462, 4522, 44400, 4352, 4462, 4523, 44401, 4352, 4462, 4524, 44402, 4352, 4462, 4525, 44403, 4352, 4462, 4526,
    44404, 4352, 4462, 4527, 44405, 4352, 4462, 4528, 44406, 4352, 4462, 4529, 44407, 4352, 4462, 4530, 44408, 4352, 4462, 4531, 44409, 4352, 4462, 4532,
    44410, 4352, 4462, 4533, 44411, 4352, 4462, 4534, 44412, 4352, 4462, 4535, 44413, 4352, 4462, 4536, 44414, 4352, 4462, 12612, 44415, 4352, 4462, 4538,
    44416, 4352, 4462, 4539, 44417, 4352, 4462, 4540, 44418, 4352, 4462, 4364, 44419, 4352, 4462, 4542, 44420, 4352, 4462, 4543, 44421, 4352, 4462, 4544,
    44422, 4352, 4462, 4545, 44423, 4352, 4462, 4546, 44424, 4352, 4463, 0, 44425, 4352, 4463, 4520, 44426, 4352, 4463, 4521, 44427, 4352, 4463, 4522,
    44428, 4352, 4463, 4523, 44429, 4352, 4463, 4524, 44430, 4352, 4463, 4525, 44431, 4352, 4463, 4526, 44432, 4352, 4463, 4527, 44433, 4352, 4463, 4528,
    44434, 4352, 4463, 4529, 44435, 4352, 4463, 4530, 44436, 4352, 4463, 4531, 44437, 4352, 4463, 4532, 44438, 4352, 4463, 4533, 44439, 4352, 4463, 4534,
    44440, 4352, 4463, 4535, 44441, 4352, 4463, 4536, 44442, 4352, 4463, 12612, 44443, 4352, 4463, 4538, 44444, 4352, 4463, 4539, 44445, 4352, 4463, 4540,
    44446, 4352, 4463, 4364, 44447, 4352, 4463, 4542, 44448, 4352, 4463, 4543, 44449, 4352, 4463, 4544, 44450, 4352, 4463, 4545, 44451, 4352, 4463, 4546,
    44452, 4352, 4464, 0, 44453, 4352, 4464, 4520, 44454, 4352, 4464, 4521, 44455, 4352, 4464, 4522, 44456, 4352, 4464, 4523, 44457, 4352, 4464, 4524,
    44458, 4352, 4464, 4525, 44459, 4352, 4464, 4526, 44460, 4352, 4464, 4527, 44461, 4352, 4464, 4528, 44462, 4352, 4464, 4529, 44463, 4352, 4464, 4530,
    44464, 4352, 4464, 4531, 44465, 4352, 4464, 4532, 44466, 4352, 4464, 4533, 44467, 4352, 4464, 4534, 44468, 4352, 4464, 4535, 44469, 4352, 4464, 4536,
    44470, 4352, 4464, 12612, 44471, 4352, 4464, 4538, 44472, 4352, 4464, 4539, 44473, 4352, 4464, 4540, 44474, 4352, 4464, 4364, 44475, 4352, 4464, 4542,
    44476, 4352, 4464, 4543, 44477, 4352, 4464, 4544, 44478, 4352, 4464, 4545, 44479, 4352, 4464, 4546, 44480, 4352, 4465, 0, 44481, 4352, 4465, 4520,
    44482, 4352, 4465, 4521, 44483, 4352, 4465, 4522, 44484, 4352, 4465, 4523, 44485, 4352, 4465, 4524, 44486, 4352, 4465, 4525, 44487, 4352, 4465, 4526,
    44488, 4352, 4465, 4527, 44489, 4352, 4465, 4528, 44490, 4352, 4465, 4529, 44491, 4352, 4465, 4530, 44492, 4352, 4465, 4531, 44493, 4352, 4465, 4532,
    44494, 4352, 4465, 4533, 44495, 4352, 4465, 4534, 44496, 4352, 4465, 4535, 44497, 4352, 4465, 4536, 44498, 4352, 4465, 12612, 44499, 4352, 4465, 4538,
    44500, 4352, 4465, 4539, 44501, 4352, 4465, 4540, 44502, 4352, 4465, 4364, 44503, 4352, 4465, 4542, 44504, 4352, 4465, 4543, 44505, 4352, 4465, 4544,
    44506, 4352, 4465, 4545, 44507, 4352, 4465, 4546, 44508, 4352, 4466, 0, 44509, 4352, 4466, 4520, 44510, 4352, 4466, 4521, 44511, 4352, 4466, 4522,
    44512, 4352, 4466, 4523, 44513, 4352, 4466, 4524, 44514, 4352, 4466, 4525, 44515, 4352, 4466, 4526, 44516, 4352, 4466, 4527, 44517, 4352, 4466, 4528,
    44518, 4352, 4466, 4529, 44519, 4352, 4466, 4530, 44520, 4352, 4466, 4531, 44521, 4352, 4466, 4532, 44522, 4352, 4466, 4533, 44523, 4352, 4466, 4534,
    44524, 4352, 4466, 4535, 44525, 4352, 4466, 4536, 44526, 4352, 4466, 12612, 44527, 4352, 4466, 4538, 44528, 4352, 4466, 4539, 44529, 4352, 4466, 4540,
    44530, 4352, 4466, 4364, 44531, 4352, 4466, 4542, 44532, 4352, 4466, 4543, 44533, 4352, 4466, 4544, 44534, 4352, 4466, 4545, 44535, 4352, 4466, 4546,
    44536, 4352, 4467, 0, 44537, 4352, 4467, 4520, 44538, 4352, 4467, 4521, 44539, 4352, 4467, 4522, 44540, 4352, 4467, 4523, 44541, 4352, 4467, 4524,
    44542, 4352, 4467, 4525, 44543, 4352, 4467, 4526, 44544, 4352, 4467, 4527, 44545, 4352, 4467, 4528, 44546, 4352, 4467, 4529, 44547, 4352, 4467, 4530,
    44548, 4352, 4467, 4531, 44549, 4352, 4467, 4532, 44550, 4352, 4467, 4533, 44551, 4352, 4467, 4534, 44552, 4352, 4467, 4535, 44553, 4352, 4467, 4536,
    44554, 4352, 4467, 12612, 44555, 4352, 4467, 4538, 44556, 4352, 4467, 4539, 44557, 4352, 4467, 4540, 44558, 4352, 4467, 4364, 44559, 4352, 4467, 4542,
    44560, 4352, 4467, 4543, 44561, 4352, 4467, 4544, 44562, 4352, 4467, 4545, 44563, 4352, 4467, 4546, 44564, 4352, 4468, 0, 44565, 4352, 4468, 4520,
    44566, 4352, 4468, 4521, 44567, 4352, 4468, 4522, 44568, 4352, 4468, 4523, 44569, 4352, 4468, 4524, 44570, 4352, 4468, 4525, 44571, 4352, 4468, 4526,
    44572, 4352, 4468, 4527, 44573, 4352, 4468, 4528, 44574, 4352, 4468, 4529, 44575, 4352, 4468, 4530, 44576, 4352, 4468, 4531, 44577, 4352, 4468, 4532,
    44578, 4352, 4468, 4533, 44579, 4352, 4468, 4534, 44580, 4352, 4468, 4535, 44581, 4352, 4468, 4536, 44582, 4352, 4468, 12612, 44583, 4352, 4468, 4538,
    44584, 4352, 4468, 4539, 44585, 4352, 4468, 4540, 44586, 4352, 4468, 4364, 44587, 4352, 4468, 4542, 44588, 4352, 4468, 4543, 44589, 4352, 4468, 4544,
    44590, 4352, 4468, 4545, 44591, 4352, 4468, 4546, 44592, 4352, 4469, 0, 44593, 4352, 4469, 4520, 44594, 4352, 4469, 4521, 44595, 4352, 4469, 4522,
    44596, 4352, 4469, 4523, 44597, 4352, 4469, 4524, 44598, 4352, 4469, 4525, 44599, 4352, 4469, 4526, 44600, 4352, 4469, 4527, 44601, 4352, 4469, 4528,
    44602, 4352, 4469, 4529, 44603, 4352, 4469, 4530, 44604, 4352, 4469, 4531, 44605, 4352, 4469, 4532, 44606, 4352, 4469, 4533, 44607, 4352, 4469, 4534,
    44608, 4352, 4469, 4535, 44609, 4352, 4469, 4536, 44610, 4352, 4469, 12612, 44611, 4352, 4469, 4538, 44612, 4352, 4469, 4539, 44613, 4352, 4469, 4540,
    44614, 4352, 4469, 4364, 44615, 4352, 4469, 4542, 44616, 4352, 4469, 4543, 44617, 4352, 4469, 4544, 44618, 4352, 4469, 4545, 44619, 4352, 4469, 4546,
    44620, 4353, 4449, 0, 44621, 4353, 4449, 4520, 44622, 4353, 4449, 4521, 44623, 4353, 4449, 4522, 44624, 4353, 4449, 4523, 44625, 4353, 4449, 4524,
    44626, 4353, 4449, 4525, 44627, 4353, 4449, 4526, 44628, 4353, 4449, 4527, 44629, 4353, 4449, 4528, 44630, 4353, 4449, 4529, 44631, 4353, 4449, 4530,
    44632, 4353, 4449, 4531, 44633, 4353, 4449, 4532, 44634, 4353, 4449, 4533, 44635, 4353, 4449, 4534, 44636, 4353, 4449, 4535, 44637, 4353, 4449, 4536,
    44638, 4353, 4449, 12612, 44639, 4353, 4449, 4538, 44640, 4353, 4449, 4539, 44641, 4353, 4449, 4540, 44642, 4353, 4449, 4364, 44643, 4353, 4449, 4542,
    44644, 4353, 4449, 4543, 44645, 4353, 4449, 4544, 44646, 4353, 4449, 4545, 44647, 4353, 4449, 4546, 44648, 4353, 4450, 0, 44649, 4353, 4450, 4520,
    44650, 4353, 4450, 4521, 44651, 4353, 4450, 4522, 44652, 4353, 4450, 4523, 44653, 4353, 4450, 4524, 44654, 4353, 4450, 4525, 44655, 4353, 4450, 4526,
    44656, 4353, 4450, 4527, 44657, 4353, 4450, 4528, 44658, 4353, 4450, 4529, 44659, 4353, 4450, 4530, 44660, 4353, 4450, 4531, 44661, 4353, 4450, 4532,
    44662, 4353, 4450, 4533, 44663, 4353, 4450, 4534, 44664, 4353, 4450, 4535, 44665, 4353, 4450, 4536, 44666, 4353, 4450, 12612, 44667, 4353, 4450, 4538,
    44668, 4353, 4450, 4539, 44669, 4353, 4450, 4540, 44670, 4353, 4450, 4364, 44671, 4353, 4450, 4542, 44672, 4353, 4450, 4543, 44673, 4353, 4450, 4544,
    44674, 4353, 4450, 4545, 44675, 4353, 4450, 4546, 44676, 4353, 4451, 0, 44677, 4353, 4451, 4520, 44678, 4353, 4451, 4521, 44679, 4353, 4451, 4522,
    44680, 4353, 4451, 4523, 44681, 4353, 4451, 4524, 44682, 4353, 4451, 4525, 44683, 4353, 4451, 4526, 44684, 4353, 4451, 4527, 44685, 4353, 4451, 4528,
    44686, 4353, 4451, 4529, 44687, 4353, 4451, 4530, 44688, 4353, 4451, 4531, 44689, 4353, 4451, 4532, 44690, 4353, 4451, 4533, 44691, 4353, 4451, 4534,
    44692, 4353, 4451, 4535, 44693, 4353, 4451, 4536, 44694, 4353, 4451, 12612, 44695, 4353, 4451, 4538, 44696, 4353, 4451, 4539, 44697, 4353, 4451, 4540,
    44698, 4353, 4451, 4364, 44699, 4353, 4451, 4542, 44700, 4353, 4451, 4543, 44701, 4353, 4451, 4544, 44702, 4353, 4451, 4545, 44703, 4353, 4451, 4546,
    44704, 4353, 4452, 0, 44705, 4353, 4452, 4520, 44706, 4353, 4452, 4521, 44707, 4353, 4452, 4522, 44708, 4353, 4452, 4523, 44709, 4353, 4452, 4524,
    44710, 4353, 4452, 4525, 44711, 4353, 4452, 4526, 44712, 4353, 4452, 4527, 44713, 4353, 4452, 4528, 44714, 4353, 4452, 4529, 44715, 4353, 4452, 4530,
    44716, 4353, 4452, 4531, 44717, 4353, 4452, 4532, 44718, 4353, 4452, 4533, 44719, 4353, 4452, 4534, 44720, 4353, 4452, 4535, 44721, 4353, 4452, 4536,
    44722, 4353, 4452, 12612, 44723, 4353, 4452, 4538, 44724, 4353, 4452, 4539, 44725, 4353, 4452, 4540, 44726, 4353, 4452, 4364, 44727, 4353, 4452, 4542,
    44728, 4353, 4452, 4543, 44729, 4353, 4452, 4544, 44730, 4353, 4452, 4545, 44731, 4353, 4452, 4546, 44732, 4353, 4453, 0, 44733, 4353, 4453, 4520,
    44734, 4353, 4453, 4521, 44735, 4353, 4453, 4522, 44736, 4353, 4453, 4523, 44737, 4353, 4453, 4524, 44738, 4353, 4453, 4525, 44739, 4353, 4453, 4526,
    44740, 4353, 4453, 4527, 44741, 4353, 4453, 4528, 44742, 4353, 4453, 4529, 44743, 4353, 4453, 4530, 44744, 4353, 4453, 4531, 44745, 4353, 4453, 4532,
    44746, 4353, 4453, 4533, 44747, 4353, 4453, 4534, 44748, 4353, 4453, 4535, 44749, 4353, 4453, 4536, 44750, 4353, 4453, 12612, 44751, 4353, 4453, 4538,
    44752, 4353, 4453, 4539, 44753, 4353, 4453, 4540, 44754, 4353, 4453, 4364, 44755, 4353, 4453, 4542, 44756, 4353, 4453, 4543, 44757, 4353, 4453, 4544,
    44758, 4353, 4453, 4545, 44759, 4353, 4453, 4546, 44760, 4353, 4454, 0, 44761, 4353, 4454, 4520, 44762, 4353, 4454, 4521, 44763, 4353, 4454, 4522,
    44764, 4353, 4454, 4523, 44765, 4353, 4454, 4524, 44766, 4353, 4454, 4525, 44767, 4353, 4454, 4526, 44768, 4353, 4454, 4527, 44769, 4353, 4454, 4528,
    44770, 4353, 4454, 4529, 44771, 4353, 4454, 4530, 44772, 4353, 4454, 4531, 44773, 4353, 4454, 4532, 44774, 4353, 4454, 4533, 44775, 4353, 4454, 4534,
    44776, 4353, 4454, 4535, 44777, 4353, 4454, 4536, 44778, 4353, 4454, 12612, 44779, 4353, 4454, 4538, 44780, 4353, 4454, 4539, 44781, 4353, 4454, 4540,
    44782, 4353, 4454, 4364, 44783, 4353, 4454, 4542, 44784, 4353, 4454, 4543, 44785, 4353, 4454, 4544, 44786, 4353, 4454, 4545, 44787, 4353, 4454, 4546,
    44788, 4353, 4455, 0, 44789, 4353, 4455, 4520, 44790, 4353, 4455, 4521, 44791, 4353, 4455, 4522, 44792, 4353, 4455, 4523, 44793, 4353, 4455, 4524,
    44794, 4353, 4455, 4525, 44795, 4353, 4455, 4526, 44796, 4353, 4455, 4527, 44797, 4353, 4455, 4528, 44798, 4353, 4455, 4529, 44799, 4353, 4455, 4530,
    44800, 4353, 4455, 4531, 44801, 4353, 4455, 4532, 44802, 4353, 4455, 4533, 44803, 4353, 4455, 4534, 44804, 4353, 4455, 4535, 44805, 4353, 4455, 4536,
    44806, 4353, 4455, 12612, 44807, 4353, 4455, 4538, 44808, 4353, 4455, 4539, 44809, 4353, 4455, 4540, 44810, 4353, 4455, 4364, 44811, 4353, 4455, 4542,
    44812, 4353, 4455, 4543, 44813, 4353, 4455, 4544, 44814, 4353, 4455, 4545, 44815, 4353, 4455, 4546, 44816, 4353, 4456, 0, 44817, 4353, 4456, 4520,
    44818, 4353, 4456, 4521, 44819, 4353, 4456, 4522, 44820, 4353, 4456, 4523, 44821, 4353, 4456, 4524, 44822, 4353, 4456, 4525, 44823, 4353, 4456, 4526,
    44824, 4353, 4456, 4527, 44825, 4353, 4456, 4528, 44826, 4353, 4456, 4529, 44827, 4353, 4456, 4530, 44828, 4353, 4456, 4531, 44829, 4353, 4456, 4532,
    44830, 4353, 4456, 4533, 44831, 4353, 4456, 4534, 44832, 4353, 4456, 4535, 44833, 4353, 4456, 4536, 44834, 4353, 4456, 12612, 44835, 4353, 4456, 4538,
    44836, 4353, 4456, 4539, 44837, 4353, 4456, 4540, 44838, 4353, 4456, 4364, 44839, 4353, 4456, 4542, 44840, 4353, 4456, 4543, 44841, 4353, 4456, 4544,
    44842, 4353, 4456, 4545, 44843, 4353, 4456, 4546, 44844, 4353, 4457, 0, 44845, 4353, 4457, 4520, 44846, 4353, 4457, 4521, 44847, 4353, 4457, 4522,
    44848, 4353, 4457, 4523, 44849, 4353, 4457, 4524, 44850, 4353, 4457, 4525, 44851, 4353, 4457, 4526, 44852, 4353, 4457, 4527, 44853, 4353, 4457, 4528,
    44854, 4353, 4457, 4529, 44855, 4353, 4457, 4530, 44856, 4353, 4457, 4531, 44857, 4353, 4457, 4532, 44858, 4353, 4457, 4533, 44859, 4353, 4457, 4534,
    44860, 4353, 4457, 4535, 44861, 4353, 4457, 4536, 44862, 4353, 4457, 12612, 44863, 4353, 4457, 4538, 44864, 4353, 4457, 4539, 44865, 4353, 4457, 4540,
    44866, 4353, 4457, 4364, 44867, 4353, 4457, 4542, 44868, 4353, 4457, 4543, 44869, 4353, 4457, 4544, 44870, 4353, 4457, 4545, 44871, 4353, 4457, 4546,
    44872, 4353, 4458, 0, 44873, 4353, 4458, 4520, 44874, 4353, 4458, 4521, 44875, 4353, 4458, 4522, 44876, 4353, 4458, 4523, 44877, 4353, 4458, 4524,
    44878, 4353, 4458, 4525, 44879, 4353, 4458, 4526, 44880, 4353, 4458, 4527, 44881, 4353, 4458, 4528, 44882, 4353, 4458, 4529, 44883, 4353, 4458, 4530,
    44884, 4353, 4458, 4531, 44885, 4353, 4458, 4532, 44886, 4353, 4458, 4533, 44887, 4353, 4458, 4534, 44888, 4353, 4458, 4535, 44889, 4353, 4458, 4536,
    44890, 4353, 4458, 12612, 44891, 4353, 4458, 4538, 44892, 4353, 4458, 4539, 44893, 4353, 4458, 4540, 44894, 4353, 4458, 4364, 44895, 4353, 4458, 4542,
    44896, 4353, 4458, 4543, 44897, 4353, 4458, 4544, 44898, 4353, 4458, 4545, 44899, 4353, 4458, 4546, 44900, 4353, 4459, 0, 44901, 4353, 4459, 4520,
    44902, 4353, 4459, 4521, 44903, 4353, 4459, 4522, 44904, 4353, 4459, 4523, 44905, 4353, 4459, 4524, 44906, 4353, 4459, 4525, 44907, 4353, 4459, 4526,
    44908, 4353, 4459, 4527, 44909, 4353, 4459, 4528, 44910, 4353, 4459, 4529, 44911, 4353, 4459, 4530, 44912, 4353, 4459, 4531, 44913, 4353, 4459, 4532,
    44914, 4353, 4459, 4533, 44915, 4353, 4459, 4534, 44916, 4353, 4459, 4535, 44917, 4353, 4459, 4536, 44918, 4353, 4459, 12612, 44919, 4353, 4459, 4538,
    44920, 4353, 4459, 4539, 44921, 4353, 4459, 4540, 44922, 4353, 4459, 4364, 44923, 4353, 4459, 4542, 44924, 4353, 4459, 4543, 44925, 4353, 4459, 4544,
    44926, 4353, 4459, 4545, 44927, 4353, 4459, 4546, 44928, 4353, 4460, 0, 44929, 4353, 4460, 4520, 44930, 4353, 4460, 4521, 44931, 4353, 4460, 4522,
    44932, 4353, 4460, 4523, 44933, 4353, 4460, 4524, 44934, 4353, 4460, 4525, 44935, 4353, 4460, 4526, 44936, 4353, 4460, 4527, 44937, 4353, 4460, 4528,
    44938, 4353, 4460, 4529, 44939, 4353, 4460, 4530, 44940, 4353, 4460, 4531, 44941, 4353, 4460, 4532, 44942, 4353, 4460, 4533, 44943, 4353, 4460, 4534,
    44944, 4353, 4460, 4535, 44945, 4353, 4460, 4536, 44946, 4353, 4460, 12612, 44947, 4353, 4460, 4538, 44948, 4353, 4460, 4539, 44949, 4353, 4460, 4540,
    44950, 4353, 4460, 4364, 44951, 4353, 4460, 4542, 44952, 4353, 4460, 4543, 44953, 4353, 4460, 4544, 44954, 4353, 4460, 4545, 44955, 4353, 4460, 4546,
    44956, 4353, 4461, 0, 44957, 4353, 4461, 4520, 44958, 4353, 4461, 4521, 44959, 4353, 4461, 4522, 44960, 4353, 4461, 4523, 44961, 4353, 4461, 4524,
    44962, 4353, 4461, 4525, 44963, 4353, 4461, 4526, 44964, 4353, 4461, 4527, 44965, 4353, 4461, 4528, 44966, 4353, 4461, 4529, 44967, 4353, 4461, 4530,
    44968, 4353, 4461, 4531, 44969, 4353, 4461, 4532, 44970, 4353, 4461, 4533, 44971, 4353, 4461, 4534, 44972, 4353, 4461, 4535, 44973, 4353, 4461, 4536,
    44974, 4353, 4461, 12612, 44975, 4353, 4461, 4538, 44976, 4353, 4461, 4539, 44977, 4353, 4461, 4540, 44978, 4353, 4461, 4364, 44979, 4353, 4461, 4542,
    44980, 4353, 4461, 4543, 44981, 4353, 4461, 4544, 44982, 4353, 4461, 4545, 44983, 4353, 4461, 4546, 44984, 4353, 4462, 0, 44985, 4353, 4462, 4520,
    44986, 4353, 4462, 4521, 44987, 4353, 4462, 4522, 44988, 4353, 4462, 4523, 44989, 4353, 4462, 4524, 44990, 4353, 4462, 4525, 44991, 4353, 4462, 4526,
    44992, 4353, 4462, 4527, 44993, 4353, 4462, 4528, 44994, 4353, 4462, 4529, 44995, 4353, 4462, 4530, 44996, 4353, 4462, 4531, 44997, 4353, 4462, 4532,
    44998, 4353, 4462, 4533, 44999, 4353, 4462, 4534, 45000, 4353, 4462, 4535, 45001, 4353, 4462, 4536, 45002, 4353, 4462, 12612, 45003, 4353, 4462, 4538,
    45004, 4353, 4462, 4539, 45005, 4353, 4462, 4540, 45006, 4353, 4462, 4364, 45007, 4353, 4462, 4542, 45008, 4353, 4462, 4543, 45009, 4353, 4462, 4544,
    45010, 4353, 4462, 4545, 45011, 4353, 4462, 4546, 45012, 4353, 4463, 0, 45013, 4353, 4463, 4520, 45014, 4353, 4463, 4521, 45015, 4353, 4463, 4522,
    45016, 4353, 4463, 4523, 45017, 4353, 4463, 4524, 45018, 4353, 4463, 4525, 45019, 4353, 4463, 4526, 45020, 4353, 4463, 4527, 45021, 4353, 4463, 4528,
    45022, 4353, 4463, 4529, 45023, 4353, 4463, 4530, 45024, 4353, 4463, 4531, 45025, 4353, 4463, 4532, 45026, 4353, 4463, 4533, 45027, 4353, 4463, 4534,
    45028, 4353, 4463, 4535, 45029, 4353, 4463, 4536, 45030, 4353, 4463, 12612, 45031, 4353, 4463, 4538, 45032, 4353, 4463, 4539, 45033, 4353, 4463, 4540,
    45034, 4353, 4463, 4364, 45035, 4353, 4463, 4542, 45036, 4353, 4463, 4543, 45037, 4353, 4463, 4544, 45038, 4353, 4463, 4545, 45039, 4353, 4463, 4546,
    45040, 4353, 4464, 0, 45041, 4353, 4464, 4520, 45042, 4353, 4464, 4521, 45043, 4353, 4464, 4522, 45044, 4353, 4464, 4523, 45045, 4353, 4464, 4524,
    45046, 4353, 4464, 4525, 45047, 4353, 4464, 4526, 45048, 4353, 4464, 4527, 45049, 4353, 4464, 4528, 45050, 4353, 4464, 4529, 45051, 4353, 4464, 4530,
    45052, 4353, 4464, 4531, 45053, 4353, 4464, 4532, 45054, 4353, 4464, 4533, 45055, 4353, 4464, 4534, 45056, 4353, 4464, 4535, 45057, 4353, 4464, 4536,
    45058, 4353, 4464, 12612, 45059, 4353, 4464, 4538, 45060, 4353, 4464, 4539, 45061, 4353, 4464, 4540, 45062, 4353, 4464, 4364, 45063, 4353, 4464, 4542,
    45064, 4353, 4464, 4543, 45065, 4353, 4464, 4544, 45066, 4353, 4464, 4545, 45067, 4353, 4464, 4546, 45068, 4353, 4465, 0, 45069, 4353, 4465, 4520,
    45070, 4353, 4465, 4521, 45071, 4353, 4465, 4522, 45072, 4353, 4465, 4523, 45073, 4353, 4465, 4524, 45074, 4353, 4465, 4525, 45075, 4353, 4465, 4526,
    45076, 4353, 4465, 4527, 45077, 4353, 4465, 4528, 45078, 4353, 4465, 4529, 45079, 4353, 4465, 4530, 45080, 4353, 4465, 4531, 45081, 4353, 4465, 4532,
    45082, 4353, 4465, 4533, 45083, 4353, 4465, 4534, 45084, 4353, 4465, 4535, 45085, 4353, 4465, 4536, 45086, 4353, 4465, 12612, 45087, 4353, 4465, 4538,
    45088, 4353, 4465, 4539, 45089, 4353, 4465, 4540, 45090, 4353, 4465, 4364, 45091, 4353, 4465, 4542, 45092, 4353, 4465, 4543, 45093, 4353, 4465, 4544,
    45094, 4353, 4465, 4545, 45095, 4353, 4465, 4546, 45096, 4353, 4466, 0, 45097, 4353, 4466, 4520, 45098, 4353, 4466, 4521, 45099, 4353, 4466, 4522,
    45100, 4353, 4466, 4523, 45101, 4353, 4466, 4524, 45102, 4353, 4466, 4525, 45103, 4353, 4466, 4526, 45104, 4353, 4466, 4527, 45105, 4353, 4466, 4528,
    45106, 4353, 4466, 4529, 45107, 4353, 4466, 4530, 45108, 4353, 4466, 4531, 45109, 4353, 4466, 4532, 45110, 4353, 4466, 4533, 45111, 4353, 4466, 4534,
    45112, 4353, 4466, 4535, 45113, 4353, 4466, 4536, 45114, 4353, 4466, 12612, 45115, 4353, 4466, 4538, 45116, 4353, 4466, 4539, 45117, 4353, 4466, 4540,
    45118, 4353, 4466, 4364, 45119, 4353, 4466, 4542, 45120, 4353, 4466, 4543, 45121, 4353, 4466, 4544, 45122, 4353, 4466, 4545, 45123, 4353, 4466, 4546,
    45124, 4353, 4467, 0, 45125, 4353, 4467, 4520, 45126, 4353, 4467, 4521, 45127, 4353, 4467, 4522, 45128, 4353, 4467, 4523, 45129, 4353, 4467, 4524,
    45130, 4353, 4467, 4525, 45131, 4353, 4467, 4526, 45132, 4353, 4467, 4527, 45133, 4353, 4467, 4528, 45134, 4353, 4467, 4529, 45135, 4353, 4467, 4530,
    45136, 4353, 4467, 4531, 45137, 4353, 4467, 4532, 45138, 4353, 4467, 4533, 45139, 4353, 4467, 4534, 45140, 4353, 4467, 4535, 45141, 4353, 4467, 4536,
    45142, 4353, 4467, 12612, 45143, 4353, 4467, 4538, 45144, 4353, 4467, 4539, 45145, 4353, 4467, 4540, 45146, 4353, 4467, 4364, 45147, 4353, 4467, 4542,
    45148, 4353, 4467, 4543, 45149, 4353, 4467, 4544, 45150, 4353, 4467, 4545, 45151, 4353, 4467, 4546, 45152, 4353, 4468, 0, 45153, 4353, 4468, 4520,
    45154, 4353, 4468, 4521, 45155, 4353, 4468, 4522, 45156, 4353, 4468, 4523, 45157, 4353, 4468, 4524, 45158, 4353, 4468, 4525, 45159, 4353, 4468, 4526,
    45160, 4353, 4468, 4527, 45161, 4353, 4468, 4528, 45162, 4353, 4468, 4529, 45163, 4353, 4468, 4530, 45164, 4353, 4468, 4531, 45165, 4353, 4468, 4532,
    45166, 4353, 4468, 4533, 45167, 4353, 4468, 4534, 45168, 4353, 4468, 4535, 45169, 4353, 4468, 4536, 45170, 4353, 4468, 12612, 45171, 4353, 4468, 4538,
    45172, 4353, 4468, 4539, 45173, 4353, 4468, 4540, 45174, 4353, 4468, 4364, 45175, 4353, 4468, 4542, 45176, 4353, 4468, 4543, 45177, 4353, 4468, 4544,
    45178, 4353, 4468, 4545, 45179, 4353, 4468, 4546, 45180, 4353, 4469, 0, 45181, 4353, 4469, 4520, 45182, 4353, 4469, 4521, 45183, 4353, 4469, 4522,
    45184, 4353, 4469, 4523, 45185, 4353, 4469, 4524, 45186, 4353, 4469, 4525, 45187, 4353, 4469, 4526, 45188, 4353, 4469, 4527, 45189, 4353, 4469, 4528,
    45190, 4353, 4469, 4529, 45191, 4353, 4469, 4530, 45192, 4353, 4469, 4531, 45193, 4353, 4469, 4532, 45194, 4353, 4469, 4533, 45195, 4353, 4469, 4534,
    45196, 4353, 4469, 4535, 45197, 4353, 4469, 4536, 45198, 4353, 4469, 12612, 45199, 4353, 4469, 4538, 45200, 4353, 4469, 4539, 45201, 4353, 4469, 4540,
    45202, 4353, 4469, 4364, 45203, 4353, 4469, 4542, 45204, 4353, 4469, 4543, 45205, 4353, 4469, 4544, 45206, 4353, 4469, 4545, 45207, 4353, 4469, 4546,
    45208, 4354, 4449, 0, 45209, 4354, 4449, 4520, 45210, 4354, 4449, 4521, 45211, 4354, 4449, 4522, 45212, 4354, 4449, 4523, 45213, 4354, 4449, 4524,
    45214, 4354, 4449, 4525, 45215, 4354, 4449, 4526, 45216, 4354, 4449, 4527, 45217, 4354, 4449, 4528, 45218, 4354, 4449, 4529, 45219, 4354, 4449, 4530,
    45220, 4354, 4449, 4531, 45221, 4354, 4449, 4532, 45222, 4354, 4449, 4533, 45223, 4354, 4449, 4534, 45224, 4354, 4449, 4535, 45225, 4354, 4449, 4536,
    45226, 4354, 4449, 12612, 45227, 4354, 4449, 4538, 45228, 4354, 4449, 4539, 45229, 4354, 4449, 4540, 45230, 4354, 4449, 4364, 45231, 4354, 4449, 4542,
    45232, 4354, 4449, 4543, 45233, 4354, 4449, 4544, 45234, 4354, 4449, 4545, 45235, 4354, 4449, 4546, 45236, 4354, 4450, 0, 45237, 4354, 4450, 4520,
    45238, 4354, 4450, 4521, 45239, 4354, 4450, 4522, 45240, 4354, 4450, 4523, 45241, 4354, 4450, 4524, 45242, 4354, 4450, 4525, 45243, 4354, 4450, 4526,
    45244, 4354, 4450, 4527, 45245, 4354, 4450, 4528, 45246, 4354, 4450, 4529, 45247, 4354, 4450, 4530, 45248, 4354, 4450, 4531, 45249, 4354, 4450, 4532,
    45250, 4354, 4450, 4533, 45251, 4354, 4450, 4534, 45252, 4354, 4450, 4535, 45253, 4354, 4450, 4536, 45254, 4354, 4450, 12612, 45255, 4354, 4450, 4538,
    45256, 4354, 4450, 4539, 45257, 4354, 4450, 4540, 45258, 4354, 4450, 4364, 45259, 4354, 4450, 4542, 45260, 4354, 4450, 4543, 45261, 4354, 4450, 4544,
    45262, 4354, 4450, 4545, 45263, 4354, 4450, 4546, 45264, 4354, 4451, 0, 45265, 4354, 4451, 4520, 45266, 4354, 4451, 4521, 45267, 4354, 4451, 4522,
    45268, 4354, 4451, 4523, 45269, 4354, 4451, 4524, 45270, 4354, 4451, 4525, 45271, 4354, 4451, 4526, 45272, 4354, 4451, 4527, 45273, 4354, 4451, 4528,
    45274, 4354, 4451, 4529, 45275, 4354, 4451, 4530, 45276, 4354, 4451, 4531, 45277, 4354, 4451, 4532, 45278, 4354, 4451, 4533, 45279, 4354, 4451, 4534,
    45280, 4354, 4451, 4535, 45281, 4354, 4451, 4536, 45282, 4354, 4451, 12612, 45283, 4354, 4451, 4538, 45284, 4354, 4451, 4539, 45285, 4354, 4451, 4540,
    45286, 4354, 4451, 4364, 45287, 4354, 4451, 4542, 45288, 4354, 4451, 4543, 45289, 4354, 4451, 4544, 45290, 4354, 4451, 4545, 45291, 4354, 4451, 4546,
    45292, 4354, 4452, 0, 45293, 4354, 4452, 4520, 45294, 4354, 4452, 4521, 45295, 4354, 4452, 4522, 45296, 4354, 4452, 4523, 45297, 4354, 4452, 4524,
    45298, 4354, 4452, 4525, 45299, 4354, 4452, 4526, 45300, 4354, 4452, 4527, 45301, 4354, 4452, 4528, 45302, 4354, 4452, 4529, 45303, 4354, 4452, 4530,
    45304, 4354, 4452, 4531, 45305, 4354, 4452, 4532, 45306, 4354, 4452, 4533, 45307, 4354, 4452, 4534, 45308, 4354, 4452, 4535, 45309, 4354, 4452, 4536,
    45310, 4354, 4452, 12612, 45311, 4354, 4452, 4538, 45312, 4354, 4452, 4539, 45313, 4354, 4452, 4540, 45314, 4354, 4452, 4364, 45315, 4354, 4452, 4542,
    45316, 4354, 4452, 4543, 45317, 4354, 4452, 4544, 45318, 4354, 4452, 4545, 45319, 4354, 4452, 4546, 45320, 4354, 4453, 0, 45321, 4354, 4453, 4520,
    45322, 4354, 4453, 4521, 45323, 4354, 4453, 4522, 45324, 4354, 4453, 4523, 45325, 4354, 4453, 4524, 45326, 4354, 4453, 4525, 45327, 4354, 4453, 4526,
    45328, 4354, 4453, 4527, 45329, 4354, 4453, 4528, 45330, 4354, 4453, 4529, 45331, 4354, 4453, 4530, 45332, 4354, 4453, 4531, 45333, 4354, 4453, 4532,
    45334, 4354, 4453, 4533, 45335, 4354, 4453, 4534, 45336, 4354, 4453, 4535, 45337, 4354, 4453, 4536, 45338, 4354, 4453, 12612, 45339, 4354, 4453, 4538,
    45340, 4354, 4453, 4539, 45341, 4354, 4453, 4540, 45342, 4354, 4453, 4364, 45343, 4354, 4453, 4542, 45344, 4354, 4453, 4543, 45345, 4354, 4453, 4544,
    45346, 4354, 4453, 4545, 45347, 4354, 4453, 4546, 45348, 4354, 4454, 0, 45349, 4354, 4454, 4520, 45350, 4354, 4454, 4521, 45351, 4354, 4454, 4522,
    45352, 4354, 4454, 4523, 45353, 4354, 4454, 4524, 45354, 4354, 4454, 4525, 45355, 4354, 4454, 4526, 45356, 4354, 4454, 4527, 45357, 4354, 4454, 4528,
    45358, 4354, 4454, 4529, 45359, 4354, 4454, 4530, 45360, 4354, 4454, 4531, 45361, 4354, 4454, 4532, 45362, 4354, 4454, 4533, 45363, 4354, 4454, 4534,
    45364, 4354, 4454, 4535, 45365, 4354, 4454, 4536, 45366, 4354, 4454, 12612, 45367, 4354, 4454, 4538, 45368, 4354, 4454, 4539, 45369, 4354, 4454, 4540,
    45370, 4354, 4454, 4364, 45371, 4354, 4454, 4542, 45372, 4354, 4454, 4543, 45373, 4354, 4454, 4544, 45374, 4354, 4454, 4545, 45375, 4354, 4454, 4546,
    45376, 4354, 4455, 0, 45377, 4354, 4455, 4520, 45378, 4354, 4455, 4521, 45379, 4354, 4455, 4522, 45380, 4354, 4455, 4523, 45381, 4354, 4455, 4524,
    45382, 4354, 4455, 4525, 45383, 4354, 4455, 4526, 45384, 4354, 4455, 4527, 45385, 4354, 4455, 4528, 45386, 4354, 4455, 4529, 45387, 4354, 4455, 4530,
    45388, 4354, 4455, 4531, 45389, 4354, 4455, 4532, 45390, 4354, 4455, 4533, 45391, 4354, 4455, 4534, 45392, 4354, 4455, 4535, 45393, 4354, 4455, 4536,
    45394, 4354, 4455, 12612, 45395, 4354, 4455, 4538, 45396, 4354, 4455, 4539, 45397, 4354, 4455, 4540, 45398, 4354, 4455, 4364, 45399, 4354, 4455, 4542,
    45400, 4354, 4455, 4543, 45401, 4354, 4455, 4544, 45402, 4354, 4455, 4545, 45403, 4354, 4455, 4546, 45404, 4354, 4456, 0, 45405, 4354, 4456, 4520,
    45406, 4354, 4456, 4521, 45407, 4354, 4456, 4522, 45408, 4354, 4456, 4523, 45409, 4354, 4456, 4524, 45410, 4354, 4456, 4525, 45411, 4354, 4456, 4526,
    45412, 4354, 4456, 4527, 45413, 4354, 4456, 4528, 45414, 4354, 4456, 4529, 45415, 4354, 4456, 4530, 45416, 4354, 4456, 4531, 45417, 4354, 4456, 4532,
    45418, 4354, 4456, 4533, 45419, 4354, 4456, 4534, 45420, 4354, 4456, 4535, 45421, 4354, 4456, 4536, 45422, 4354, 4456, 12612, 45423, 4354, 4456, 4538,
    45424, 4354, 4456, 4539, 45425, 4354, 4456, 4540, 45426, 4354, 4456, 4364, 45427, 4354, 4456, 4542, 45428, 4354, 4456, 4543, 45429, 4354, 4456, 4544,
    45430, 4354, 4456, 4545, 45431, 4354, 4456, 4546, 45432, 4354, 4457, 0, 45433, 4354, 4457, 4520, 45434, 4354, 4457, 4521, 45435, 4354, 4457, 4522,
    45436, 4354, 4457, 4523, 45437, 4354, 4457, 4524, 45438, 4354, 4457, 4525, 45439, 4354, 4457, 4526, 45440, 4354, 4457, 4527, 45441, 4354, 4457, 4528,
    45442, 4354, 4457, 4529, 45443, 4354, 4457, 4530, 45444, 4354, 4457, 4531, 45445, 4354, 4457, 4532, 45446, 4354, 4457, 4533, 45447, 4354, 4457, 4534,
    45448, 4354, 4457, 4535, 45449, 4354, 4457, 4536, 45450, 4354, 4457, 12612, 45451, 4354, 4457, 4538, 45452, 4354, 4457, 4539, 45453, 4354, 4457, 4540,
    45454, 4354, 4457, 4364, 45455, 4354, 4457, 4542, 45456, 4354, 4457, 4543, 45457, 4354, 4457, 4544, 45458, 4354, 4457, 4545, 45459, 4354, 4457, 4546,
    45460, 4354, 4458, 0, 45461, 4354, 4458, 4520, 45462, 4354, 4458, 4521, 45463, 4354, 4458, 4522, 45464, 4354, 4458, 4523, 45465, 4354, 4458, 4524,
    45466, 4354, 4458, 4525, 45467, 4354, 4458, 4526, 45468, 4354, 4458, 4527, 45469, 4354, 4458, 4528, 45470, 4354, 4458, 4529, 45471, 4354, 4458, 4530,
    45472, 4354, 4458, 4531, 45473, 4354, 4458, 4532, 45474, 4354, 4458, 4533, 45475, 4354, 4458, 4534, 45476, 4354, 4458, 4535, 45477, 4354, 4458, 4536,
    45478, 4354, 4458, 12612, 45479, 4354, 4458, 4538, 45480, 4354, 4458, 4539, 45481, 4354, 4458, 4540, 45482, 4354, 4458, 4364, 45483, 4354, 4458, 4542,
    45484, 4354, 4458, 4543, 45485, 4354, 4458, 4544, 45486, 4354, 4458, 4545, 45487, 4354, 4458, 4546, 45488, 4354, 4459, 0, 45489, 4354, 4459, 4520,
    45490, 4354, 4459, 4521, 45491, 4354, 4459, 4522, 45492, 4354, 4459, 4523, 45493, 4354, 4459, 4524, 45494, 4354, 4459, 4525, 45495, 4354, 4459, 4526,
    45496, 4354, 4459, 4527, 45497, 4354, 4459, 4528, 45498, 4354, 4459, 4529, 45499, 4354, 4459, 4530, 45500, 4354, 4459, 4531, 45501, 4354, 4459, 4532,
    45502, 4354, 4459, 4533, 45503, 4354, 4459, 4534, 45504, 4354, 4459, 4535, 45505, 4354, 4459, 4536, 45506, 4354, 4459, 12612, 45507, 4354, 4459, 4538,
    45508, 4354, 4459, 4539, 45509, 4354, 4459, 4540, 45510, 4354, 4459, 4364, 45511, 4354, 4459, 4542, 45512, 4354, 4459, 4543, 45513, 4354, 4459, 4544,
    45514, 4354, 4459, 4545, 45515, 4354, 4459, 4546, 45516, 4354, 4460, 0, 45517, 4354, 4460, 4520, 45518, 4354, 4460, 4521, 45519, 4354, 4460, 4522,
    45520, 4354, 4460, 4523, 45521, 4354, 4460, 4524, 45522, 4354, 4460, 4525, 45523, 4354, 4460, 4526, 45524, 4354, 4460, 4527, 45525, 4354, 4460, 4528,
    45526, 4354, 4460, 4529, 45527, 4354, 4460, 4530, 45528, 4354, 4460, 4531, 45529, 4354, 4460, 4532, 45530, 4354, 4460, 4533, 45531, 4354, 4460, 4534,
    45532, 4354, 4460, 4535, 45533, 4354, 4460, 4536, 45534, 4354, 4460, 12612, 45535, 4354, 4460, 4538, 45536, 4354, 4460, 4539, 45537, 4354, 4460, 4540,
    45538, 4354, 4460, 4364, 45539, 4354, 4460, 4542, 45540, 4354, 4460, 4543, 45541, 4354, 4460, 4544, 45542, 4354, 4460, 4545, 45543, 4354, 4460, 4546,
    45544, 4354, 4461, 0, 45545, 4354, 4461, 4520, 45546, 4354, 4461, 4521, 45547, 4354, 4461, 4522, 45548, 4354, 4461, 4523, 45549, 4354, 4461, 4524,
    45550, 4354, 4461, 4525, 45551, 4354, 4461, 4526, 45552, 4354, 4461, 4527, 45553, 4354, 4461, 4528, 45554, 4354, 4461, 4529, 45555, 4354, 4461, 4530,
    45556, 4354, 4461, 4531, 45557, 4354, 4461, 4532, 45558, 4354, 4461, 4533, 45559, 4354, 4461, 4534, 45560, 4354, 4461, 4535, 45561, 4354, 4461, 4536,
    45562, 4354, 4461, 12612, 45563, 4354, 4461, 4538, 45564, 4354, 4461, 4539, 45565, 4354, 4461, 4540, 45566, 4354, 4461, 4364, 45567, 4354, 4461, 4542,
    45568, 4354, 4461, 4543, 45569, 4354, 4461, 4544, 45570, 4354, 4461, 4545, 45571, 4354, 4461, 4546, 45572, 4354, 4462, 0, 45573, 4354, 4462, 4520,
    45574, 4354, 4462, 4521, 45575, 4354, 4462, 4522, 45576, 4354, 4462, 4523, 45577, 4354, 4462, 4524, 45578, 4354, 4462, 4525, 45579, 4354, 4462, 4526,
    45580, 4354, 4462, 4527, 45581, 4354, 4462, 4528, 45582, 4354, 4462, 4529, 45583, 4354, 4462, 4530, 45584, 4354, 4462, 4531, 45585, 4354, 4462, 4532,
    45586, 4354, 4462, 4533, 45587, 4354, 4462, 4534, 45588, 4354, 4462, 4535, 45589, 4354, 4462, 4536, 45590, 4354, 4462, 12612, 45591, 4354, 4462, 4538,
    45592, 4354, 4462, 4539, 45593, 4354, 4462, 4540, 45594, 4354, 4462, 4364, 45595, 4354, 4462, 4542, 45596, 4354, 4462, 4543, 45597, 4354, 4462, 4544,
    45598, 4354, 4462, 4545, 45599, 4354, 4462, 4546, 45600, 4354, 4463, 0, 45601, 4354, 4463, 4520, 45602, 4354, 4463, 4521, 45603, 4354, 4463, 4522,
    45604, 4354, 4463, 4523, 45605, 4354, 4463, 4524, 45606, 4354, 4463, 4525, 45607, 4354, 4463, 4526, 45608, 4354, 4463, 4527, 45609, 4354, 4463, 4528,
    45610, 4354, 4463, 4529, 45611, 4354, 4463, 4530, 45612, 4354, 4463, 4531, 45613, 4354, 4463, 4532, 45614, 4354, 4463, 4533, 45615, 4354, 4463, 4534,
    45616, 4354, 4463, 4535, 45617, 4354, 4463, 4536, 45618, 4354, 4463, 12612, 45619, 4354, 4463, 4538, 45620, 4354, 4463, 4539, 45621, 4354, 4463, 4540,
    45622, 4354, 4463, 4364, 45623, 4354, 4463, 4542, 45624, 4354, 4463, 4543, 45625, 4354, 4463, 4544, 45626, 4354, 4463, 4545, 45627, 4354, 4463, 4546,
    45628, 4354, 4464, 0, 45629, 4354, 4464, 4520, 45630, 4354, 4464, 4521, 45631, 4354, 4464, 4522, 45632, 4354, 4464, 4523, 45633, 4354, 4464, 4524,
    45634, 4354, 4464, 4525, 45635, 4354, 4464, 4526, 45636, 4354, 4464, 4527, 45637, 4354, 4464, 4528, 45638, 4354, 4464, 4529, 45639, 4354, 4464, 4530,
    45640, 4354, 4464, 4531, 45641, 4354, 4464, 4532, 45642, 4354, 4464, 4533, 45643, 4354, 4464, 4534, 45644, 4354, 4464, 4535, 45645, 4354, 4464, 4536,
    45646, 4354, 4464, 12612, 45647, 4354, 4464, 4538, 45648, 4354, 4464, 4539, 45649, 4354, 4464, 4540, 45650, 4354, 4464, 4364, 45651, 4354, 4464, 4542,
    45652, 4354, 4464, 4543, 45653, 4354, 4464, 4544, 45654, 4354, 4464, 4545, 45655, 4354, 4464, 4546, 45656, 4354, 4465, 0, 45657, 4354, 4465, 4520,
    45658, 4354, 4465, 4521, 45659, 4354, 4465, 4522, 45660, 4354, 4465, 4523, 45661, 4354, 4465, 4524, 45662, 4354, 4465, 4525, 45663, 4354, 4465, 4526,
    45664, 4354, 4465, 4527, 45665, 4354, 4465, 4528, 45666, 4354, 4465, 4529, 45667, 4354, 4465, 4530, 45668, 4354, 4465, 4531, 45669, 4354, 4465, 4532,
    45670, 4354, 4465, 4533, 45671, 4354, 4465, 4534, 45672, 4354, 4465, 4535, 45673, 4354, 4465, 4536, 45674, 4354, 4465, 12612, 45675, 4354, 4465, 4538,
    45676, 4354, 4465, 4539, 45677, 4354, 4465, 4540, 45678, 4354, 4465, 4364, 45679, 4354, 4465, 4542, 45680, 4354, 4465, 4543, 45681, 4354, 4465, 4544,
    45682, 4354, 4465, 4545, 45683, 4354, 4465, 4546, 45684, 4354, 4466, 0, 45685, 4354, 4466, 4520, 45686, 4354, 4466, 4521, 45687, 4354, 4466, 4522,
    45688, 4354, 4466, 4523, 45689, 4354, 4466, 4524, 45690, 4354, 4466, 4525, 45691, 4354, 4466, 4526, 45692, 4354, 4466, 4527, 45693, 4354, 4466, 4528,
    45694, 4354, 4466, 4529, 45695, 4354, 4466, 4530, 45696, 4354, 4466, 4531, 45697, 4354, 4466, 4532, 45698, 4354, 4466, 4533, 45699, 4354, 4466, 4534,
    45700, 4354, 4466, 4535, 45701, 4354, 4466, 4536, 45702, 4354, 4466, 12612, 45703, 4354, 4466, 4538, 45704, 4354, 4466, 4539, 45705, 4354, 4466, 4540,
    45706, 4354, 4466, 4364, 45707, 4354, 4466, 4542, 45708, 4354, 4466, 4543, 45709, 4354, 4466, 4544, 45710, 4354, 4466, 4545, 45711, 4354, 4466, 4546,
    45712, 4354, 4467, 0, 45713, 4354, 4467, 4520, 45714, 4354, 4467, 4521, 45715, 4354, 4467, 4522, 45716, 4354, 4467, 4523, 45717, 4354, 4467, 4524,
    45718, 4354, 4467, 4525, 45719, 4354, 4467, 4526, 45720, 4354, 4467, 4527, 45721, 4354, 4467, 4528, 45722, 4354, 4467, 4529, 45723, 4354, 4467, 4530,
    45724, 4354, 4467, 4531, 45725, 4354, 4467, 4532, 45726, 4354, 4467, 4533, 45727, 4354, 4467, 4534, 45728, 4354, 4467, 4535, 45729, 4354, 4467, 4536,
    45730, 4354, 4467, 12612, 45731, 4354, 4467, 4538, 45732, 4354, 4467, 4539, 45733, 4354, 4467, 4540, 45734, 4354, 4467, 4364, 45735, 4354, 4467, 4542,
    45736, 4354, 4467, 4543, 45737, 4354, 4467, 4544, 45738, 4354, 4467, 4545, 45739, 4354, 4467, 4546, 45740, 4354, 4468, 0, 45741, 4354, 4468, 4520,
    45742, 4354, 4468, 4521, 45743, 4354, 4468, 4522, 45744, 4354, 4468, 4523, 45745, 4354, 4468, 4524, 45746, 4354, 4468, 4525, 45747, 4354, 4468, 4526,
    45748, 4354, 4468, 4527, 45749, 4354, 4468, 4528, 45750, 4354, 4468, 4529, 45751, 4354, 4468, 4530, 45752, 4354, 4468, 4531, 45753, 4354, 4468, 4532,
    45754, 4354, 4468, 4533, 45755, 4354, 4468, 4534, 45756, 4354, 4468, 4535, 45757, 4354, 4468, 4536, 45758, 4354, 4468, 12612, 45759, 4354, 4468, 4538,
    45760, 4354, 4468, 4539, 45761, 4354, 4468, 4540, 45762, 4354, 4468, 4364, 45763, 4354, 4468, 4542, 45764, 4354, 4468, 4543, 45765, 4354, 4468, 4544,
    45766, 4354, 4468, 4545, 45767, 4354, 4468, 4546, 45768, 4354, 4469, 0, 45769, 4354, 4469, 4520, 45770, 4354, 4469, 4521, 45771, 4354, 4469, 4522,
    45772, 4354, 4469, 4523, 45773, 4354, 4469, 4524, 45774, 4354, 4469, 4525, 45775, 4354, 4469, 4526, 45776, 4354, 4469, 4527, 45777, 4354, 4469, 4528,
    45778, 4354, 4469, 4529, 45779, 4354, 4469, 4530, 45780, 4354, 4469, 4531, 45781, 4354, 4469, 4532, 45782, 4354, 4469, 4533, 45783, 4354, 4469, 4534,
    45784, 4354, 4469, 4535, 45785, 4354, 4469, 4536, 45786, 4354, 4469, 12612, 45787, 4354, 4469, 4538, 45788, 4354, 4469, 4539, 45789, 4354, 4469, 4540,
    45790, 4354, 4469, 4364, 45791, 4354, 4469, 4542, 45792, 4354, 4469, 4543, 45793, 4354, 4469, 4544, 45794, 4354, 4469, 4545, 45795, 4354, 4469, 4546,
    45796, 4355, 4449, 0, 45797, 4355, 4449, 4520, 45798, 4355, 4449, 4521, 45799, 4355, 4449, 4522, 45800, 4355, 4449, 4523, 45801, 4355, 4449, 4524,
    45802, 4355, 4449, 4525, 45803, 4355, 4449, 4526, 45804, 4355, 4449, 4527, 45805, 4355, 4449, 4528, 45806, 4355, 4449, 4529, 45807, 4355, 4449, 4530,
    45808, 4355, 4449, 4531, 45809, 4355, 4449, 4532, 45810, 4355, 4449, 4533, 45811, 4355, 4449, 4534, 45812, 4355, 4449, 4535, 45813, 4355, 4449, 4536,
    45814, 4355, 4449, 12612, 45815, 4355, 4449, 4538, 45816, 4355, 4449, 4539, 45817, 4355, 4449, 4540, 45818, 4355, 4449, 4364, 45819, 4355, 4449, 4542,
    45820, 4355, 4449, 4543, 45821, 4355, 4449, 4544, 45822, 4355, 4449, 4545, 45823, 4355, 4449, 4546, 45824, 4355, 4450, 0, 45825, 4355, 4450, 4520,
    45826, 4355, 4450, 4521, 45827, 4355, 4450, 4522, 45828, 4355, 4450, 4523, 45829, 4355, 4450, 4524, 45830, 4355, 4450, 4525, 45831, 4355, 4450, 4526,
    45832, 4355, 4450, 4527, 45833, 4355, 4450, 4528, 45834, 4355, 4450, 4529, 45835, 4355, 4450, 4530, 45836, 4355, 4450, 4531, 45837, 4355, 4450, 4532,
    45838, 4355, 4450, 4533, 45839, 4355, 4450, 4534, 45840, 4355, 4450, 4535, 45841, 4355, 4450, 4536, 45842, 4355, 4450, 12612, 45843, 4355, 4450, 4538,
    45844, 4355, 4450, 4539, 45845, 4355, 4450, 4540, 45846, 4355, 4450, 4364, 45847, 4355, 4450, 4542, 45848, 4355, 4450, 4543, 45849, 4355, 4450, 4544,
    45850, 4355, 4450, 4545, 45851, 4355, 4450, 4546, 45852, 4355, 4451, 0, 45853, 4355, 4451, 4520, 45854, 4355, 4451, 4521, 45855, 4355, 4451, 4522,
    45856, 4355, 4451, 4523, 45857, 4355, 4451, 4524, 45858, 4355, 4451, 4525, 45859, 4355, 4451, 4526, 45860, 4355, 4451, 4527, 45861, 4355, 4451, 4528,
    45862, 4355, 4451, 4529, 45863, 4355, 4451, 4530, 45864, 4355, 4451, 4531, 45865, 4355, 4451, 4532, 45866, 4355, 4451, 4533, 45867, 4355, 4451, 4534,
    45868, 4355, 4451, 4535, 45869, 4355, 4451, 4536, 45870, 4355, 4451, 12612, 45871, 4355, 4451, 4538, 45872, 4355, 4451, 4539, 45873, 4355, 4451, 4540,
    45874, 4355, 4451, 4364, 45875, 4355, 4451, 4542, 45876, 4355, 4451, 4543, 45877, 4355, 4451, 4544, 45878, 4355, 4451, 4545, 45879, 4355, 4451, 4546,
    45880, 4355, 4452, 0, 45881, 4355, 4452, 4520, 45882, 4355, 4452, 4521, 45883, 4355, 4452, 4522, 45884, 4355, 4452, 4523, 45885, 4355, 4452, 4524,
    45886, 4355, 4452, 4525, 45887, 4355, 4452, 4526, 45888, 4355, 4452, 4527, 45889, 4355, 4452, 4528, 45890, 4355, 4452, 4529, 45891, 4355, 4452, 4530,
    45892, 4355, 4452, 4531, 45893, 4355, 4452, 4532, 45894, 4355, 4452, 4533, 45895, 4355, 4452, 4534, 45896, 4355, 4452, 4535, 45897, 4355, 4452, 4536,
    45898, 4355, 4452, 12612, 45899, 4355, 4452, 4538, 45900, 4355, 4452, 4539, 45901, 4355, 4452, 4540, 45902, 4355, 4452, 4364, 45903, 4355, 4452, 4542,
    45904, 4355, 4452, 4543, 45905, 4355, 4452, 4544, 45906, 4355, 4452, 4545, 45907, 4355, 4452, 4546, 45908, 4355, 4453, 0, 45909, 4355, 4453, 4520,
    45910, 4355, 4453, 4521, 45911, 4355, 4453, 4522, 45912, 4355, 4453, 4523, 45913, 4355, 4453, 4524, 45914, 4355, 4453, 4525, 45915, 4355, 4453, 4526,
    45916, 4355, 4453, 4527, 45917, 4355, 4453, 4528, 45918, 4355, 4453, 4529, 45919, 4355, 4453, 4530, 45920, 4355, 4453, 4531, 45921, 4355, 4453, 4532,
    45922, 4355, 4453, 4533, 45923, 4355, 4453, 4534, 45924, 4355, 4453, 4535, 45925, 4355, 4453, 4536, 45926, 4355, 4453, 12612, 45927, 4355, 4453, 4538,
    45928, 4355, 4453, 4539, 45929, 4355, 4453, 4540, 45930, 4355, 4453, 4364, 45931, 4355, 4453, 4542, 45932, 4355, 4453, 4543, 45933, 4355, 4453, 4544,
    45934, 4355, 4453, 4545, 45935, 4355, 4453, 4546, 45936, 4355, 4454, 0, 45937, 4355, 4454, 4520, 45938, 4355, 4454, 4521, 45939, 4355, 4454, 4522,
    45940, 4355, 4454, 4523, 45941, 4355, 4454, 4524, 45942, 4355, 4454, 4525, 45943, 4355, 4454, 4526, 45944, 4355, 4454, 4527, 45945, 4355, 4454, 4528,
    45946, 4355, 4454, 4529, 45947, 4355, 4454, 4530, 45948, 4355, 4454, 4531, 45949, 4355, 4454, 4532, 45950, 4355, 4454, 4533, 45951, 4355, 4454, 4534,
    45952, 4355, 4454, 4535, 45953, 4355, 4454, 4536, 45954, 4355, 4454, 12612, 45955, 4355, 4454, 4538, 45956, 4355, 4454, 4539, 45957, 4355, 4454, 4540,
    45958, 4355, 4454, 4364, 45959, 4355, 4454, 4542, 45960, 4355, 4454, 4543, 45961, 4355, 4454, 4544, 45962, 4355, 4454, 4545, 45963, 4355, 4454, 4546,
    45964, 4355, 4455, 0, 45965, 4355, 4455, 4520, 45966, 4355, 4455, 4521, 45967, 4355, 4455, 4522, 45968, 4355, 4455, 4523, 45969, 4355, 4455, 4524,
    45970, 4355, 4455, 4525, 45971, 4355, 4455, 4526, 45972, 4355, 4455, 4527, 45973, 4355, 4455, 4528, 45974, 4355, 4455, 4529, 45975, 4355, 4455, 4530,
    45976, 4355, 4455, 4531, 45977, 4355, 4455, 4532, 45978, 4355, 4455, 4533, 45979, 4355, 4455, 4534, 45980, 4355, 4455, 4535, 45981, 4355, 4455, 4536,
    45982, 4355, 4455, 12612, 45983, 4355, 4455, 4538, 45984, 4355, 4455, 4539, 45985, 4355, 4455, 4540, 45986, 4355, 4455, 4364, 45987, 4355, 4455, 4542,
    45988, 4355, 4455, 4543, 45989, 4355, 4455, 4544, 45990, 4355, 4455, 4545, 45991, 4355, 4455, 4546, 45992, 4355, 4456, 0, 45993, 4355, 4456, 4520,
    45994, 4355, 4456, 4521, 45995, 4355, 4456, 4522, 45996, 4355, 4456, 4523, 45997, 4355, 4456, 4524, 45998, 4355, 4456, 4525, 45999, 4355, 4456, 4526,
    46000, 4355, 4456, 4527, 46001, 4355, 4456, 4528, 46002, 4355, 4456, 4529, 46003, 4355, 4456, 4530, 46004, 4355, 4456, 4531, 46005, 4355, 4456, 4532,
    46006, 4355, 4456, 4533, 46007, 4355, 4456, 4534, 46008, 4355, 4456, 4535, 46009, 4355, 4456, 4536, 46010, 4355, 4456, 12612, 46011, 4355, 4456, 4538,
    46012, 4355, 4456, 4539, 46013, 4355, 4456, 4540, 46014, 4355, 4456, 4364, 46015, 4355, 4456, 4542, 46016, 4355, 4456, 4543, 46017, 4355, 4456, 4544,
    46018, 4355, 4456, 4545, 46019, 4355, 4456, 4546, 46020, 4355, 4457, 0, 46021, 4355, 4457, 4520, 46022, 4355, 4457, 4521, 46023, 4355, 4457, 4522,
    46024, 4355, 4457, 4523, 46025, 4355, 4457, 4524, 46026, 4355, 4457, 4525, 46027, 4355, 4457, 4526, 46028, 4355, 4457, 4527, 46029, 4355, 4457, 4528,
    46030, 4355, 4457, 4529, 46031, 4355, 4457, 4530, 46032, 4355, 4457, 4531, 46033, 4355, 4457, 4532, 46034, 4355, 4457, 4533, 46035, 4355, 4457, 4534,
    46036, 4355, 4457, 4535, 46037, 4355, 4457, 4536, 46038, 4355, 4457, 12612, 46039, 4355, 4457, 4538, 46040, 4355, 4457, 4539, 46041, 4355, 4457, 4540,
    46042, 4355, 4457, 4364, 46043, 4355, 4457, 4542, 46044, 4355, 4457, 4543, 46045, 4355, 4457, 4544, 46046, 4355, 4457, 4545, 46047, 4355, 4457, 4546,
    46048, 4355, 4458, 0, 46049, 4355, 4458, 4520, 46050, 4355, 4458, 4521, 46051, 4355, 4458, 4522, 46052, 4355, 4458, 4523, 46053, 4355, 4458, 4524,
    46054, 4355, 4458, 4525, 46055, 4355, 4458, 4526, 46056, 4355, 4458, 4527, 46057, 4355, 4458, 4528, 46058, 4355, 4458, 4529, 46059, 4355, 4458, 4530,
    46060, 4355, 4458, 4531, 46061, 4355, 4458, 4532, 46062, 4355, 4458, 4533, 46063, 4355, 4458, 4534, 46064, 4355, 4458, 4535, 46065, 4355, 4458, 4536,
    46066, 4355, 4458, 12612, 46067, 4355, 4458, 4538, 46068, 4355, 4458, 4539, 46069, 4355, 4458, 4540, 46070, 4355, 4458, 4364, 46071, 4355, 4458, 4542,
    46072, 4355, 4458, 4543, 46073, 4355, 4458, 4544, 46074, 4355, 4458, 4545, 46075, 4355, 4458, 4546, 46076, 4355, 4459, 0, 46077, 4355, 4459, 4520,
    46078, 4355, 4459, 4521, 46079, 4355, 4459, 4522, 46080, 4355, 4459, 4523, 46081, 4355, 4459, 4524, 46082, 4355, 4459, 4525, 46083, 4355, 4459, 4526,
    46084, 4355, 4459, 4527, 46085, 4355, 4459, 4528, 46086, 4355, 4459, 4529, 46087, 4355, 4459, 4530, 46088, 4355, 4459, 4531, 46089, 4355, 4459, 4532,
    46090, 4355, 4459, 4533, 46091, 4355, 4459, 4534, 46092, 4355, 4459, 4535, 46093, 4355, 4459, 4536, 46094, 4355, 4459, 12612, 46095, 4355, 4459, 4538,
    46096, 4355, 4459, 4539, 46097, 4355, 4459, 4540, 46098, 4355, 4459, 4364, 46099, 4355, 4459, 4542, 46100, 4355, 4459, 4543, 46101, 4355, 4459, 4544,
    46102, 4355, 4459, 4545, 46103, 4355, 4459, 4546, 46104, 4355, 4460, 0, 46105, 4355, 4460, 4520, 46106, 4355, 4460, 4521, 46107, 4355, 4460, 4522,
    46108, 4355, 4460, 4523, 46109, 4355, 4460, 4524, 46110, 4355, 4460, 4525, 46111, 4355, 4460, 4526, 46112, 4355, 4460, 4527, 46113, 4355, 4460, 4528,
    46114, 4355, 4460, 4529, 46115, 4355, 4460, 4530, 46116, 4355, 4460, 4531, 46117, 4355, 4460, 4532, 46118, 4355, 4460, 4533, 46119, 4355, 4460, 4534,
    46120, 4355, 4460, 4535, 46121, 4355, 4460, 4536, 46122, 4355, 4460, 12612, 46123, 4355, 4460, 4538, 46124, 4355, 4460, 4539, 46125, 4355, 4460, 4540,
    46126, 4355, 4460, 4364, 46127, 4355, 4460, 4542, 46128, 4355, 4460, 4543, 46129, 4355, 4460, 4544, 46130, 4355, 4460, 4545, 46131, 4355, 4460, 4546,
    46132, 4355, 4461, 0, 46133, 4355, 4461, 4520, 46134, 4355, 4461, 4521, 46135, 4355, 4461, 4522, 46136, 4355, 4461, 4523, 46137, 4355, 4461, 4524,
    46138, 4355, 4461, 4525, 46139, 4355, 4461, 4526, 46140, 4355, 4461, 4527, 46141, 4355, 4461, 4528, 46142, 4355, 4461, 4529, 46143, 4355, 4461, 4530,
    46144, 4355, 4461, 4531, 46145, 4355, 4461, 4532, 46146, 4355, 4461, 4533, 46147, 4355, 4461, 4534, 46148, 4355, 4461, 4535, 46149, 4355, 4461, 4536,
    46150, 4355, 4461, 12612, 46151, 4355, 4461, 4538, 46152, 4355, 4461, 4539, 46153, 4355, 4461, 4540, 46154, 4355, 4461, 4364, 46155, 4355, 4461, 4542,
    46156, 4355, 4461, 4543, 46157, 4355, 4461, 4544, 46158, 4355, 4461, 4545, 46159, 4355, 4461, 4546, 46160, 4355, 4462, 0, 46161, 4355, 4462, 4520,
    46162, 4355, 4462, 4521, 46163, 4355, 4462, 4522, 46164, 4355, 4462, 4523, 46165, 4355, 4462, 4524, 46166, 4355, 4462, 4525, 46167, 4355, 4462, 4526,
    46168, 4355, 4462, 4527, 46169, 4355, 4462, 4528, 46170, 4355, 4462, 4529, 46171, 4355, 4462, 4530, 46172, 4355, 4462, 4531, 46173, 4355, 4462, 4532,
    46174, 4355, 4462, 4533, 46175, 4355, 4462, 4534, 46176, 4355, 4462, 4535, 46177, 4355, 4462, 4536, 46178, 4355, 4462, 12612, 46179, 4355, 4462, 4538,
    46180, 4355, 4462, 4539, 46181, 4355, 4462, 4540, 46182, 4355, 4462, 4364, 46183, 4355, 4462, 4542, 46184, 4355, 4462, 4543, 46185, 4355, 4462, 4544,
    46186, 4355, 4462, 4545, 46187, 4355, 4462, 4546, 46188, 4355, 4463, 0, 46189, 4355, 4463, 4520, 46190, 4355, 4463, 4521, 46191, 4355, 4463, 4522,
    46192, 4355, 4463, 4523, 46193, 4355, 4463, 4524, 46194, 4355, 4463, 4525, 46195, 4355, 4463, 4526, 46196, 4355, 4463, 4527, 46197, 4355, 4463, 4528,
    46198, 4355, 4463, 4529, 46199, 4355, 4463, 4530, 46200, 4355, 4463, 4531, 46201, 4355, 4463, 4532, 46202, 4355, 4463, 4533, 46203, 4355, 4463, 4534,
    46204, 4355, 4463, 4535, 46205, 4355, 4463, 4536, 46206, 4355, 4463, 12612, 46207, 4355, 4463, 4538, 46208, 4355, 4463, 4539, 46209, 4355, 4463, 4540,
    46210, 4355, 4463, 4364, 46211, 4355, 4463, 4542, 46212, 4355, 4463, 4543, 46213, 4355, 4463, 4544, 46214, 4355, 4463, 4545, 46215, 4355, 4463, 4546,
    46216, 4355, 4464, 0, 46217, 4355, 4464, 4520, 46218, 4355, 4464, 4521, 46219, 4355, 4464, 4522, 46220, 4355, 4464, 4523, 46221, 4355, 4464, 4524,
    46222, 4355, 4464, 4525, 46223, 4355, 4464, 4526, 46224, 4355, 4464, 4527, 46225, 4355, 4464, 4528, 46226, 4355, 4464, 4529, 46227, 4355, 4464, 4530,
    46228, 4355, 4464, 4531, 46229, 4355, 4464, 4532, 46230, 4355, 4464, 4533, 46231, 4355, 4464, 4534, 46232, 4355, 4464, 4535, 46233, 4355, 4464, 4536,
    46234, 4355, 4464, 12612, 46235, 4355, 4464, 4538, 46236, 4355, 4464, 4539, 46237, 4355, 4464, 4540, 46238, 4355, 4464, 4364, 46239, 4355, 4464, 4542,
    46240, 4355, 4464, 4543, 46241, 4355, 4464, 4544, 46242, 4355, 4464, 4545, 46243, 4355, 4464, 4546, 46244, 4355, 4465, 0, 46245, 4355, 4465, 4520,
    46246, 4355, 4465, 4521, 46247, 4355, 4465, 4522, 46248, 4355, 4465, 4523, 46249, 4355, 4465, 4524, 46250, 4355, 4465, 4525, 46251, 4355, 4465, 4526,
    46252, 4355, 4465, 4527, 46253, 4355, 4465, 4528, 46254, 4355, 4465, 4529, 46255, 4355, 4465, 4530, 46256, 4355, 4465, 4531, 46257, 4355, 4465, 4532,
    46258, 4355, 4465, 4533, 46259, 4355, 4465, 4534, 46260, 4355, 4465, 4535, 46261, 4355, 4465, 4536, 46262, 4355, 4465, 12612, 46263, 4355, 4465, 4538,
    46264, 4355, 4465, 4539, 46265, 4355, 4465, 4540, 46266, 4355, 4465, 4364, 46267, 4355, 4465, 4542, 46268, 4355, 4465, 4543, 46269, 4355, 4465, 4544,
    46270, 4355, 4465, 4545, 46271, 4355, 4465, 4546, 46272, 4355, 4466, 0, 46273, 4355, 4466, 4520, 46274, 4355, 4466, 4521, 46275, 4355, 4466, 4522,
    46276, 4355, 4466, 4523, 46277, 4355, 4466, 4524, 46278, 4355, 4466, 4525, 46279, 4355, 4466, 4526, 46280, 4355, 4466, 4527, 46281, 4355, 4466, 4528,
    46282, 4355, 4466, 4529, 46283, 4355, 4466, 4530, 46284, 4355, 4466, 4531, 46285, 4355, 4466, 4532, 46286, 4355, 4466, 4533, 46287, 4355, 4466, 4534,
    46288, 4355, 4466, 4535, 46289, 4355, 4466, 4536, 46290, 4355, 4466, 12612, 46291, 4355, 4466, 4538, 46292, 4355, 4466, 4539, 46293, 4355, 4466, 4540,
    46294, 4355, 4466, 4364, 46295, 4355, 4466, 4542, 46296, 4355, 4466, 4543, 46297, 4355, 4466, 4544, 46298, 4355, 4466, 4545, 46299, 4355, 4466, 4546,
    46300, 4355, 4467, 0, 46301, 4355, 4467, 4520, 46302, 4355, 4467, 4521, 46303, 4355, 4467, 4522, 46304, 4355, 4467, 4523, 46305, 4355, 4467, 4524,
    46306, 4355, 4467, 4525, 46307, 4355, 4467, 4526, 46308, 4355, 4467, 4527, 46309, 4355, 4467, 4528, 46310, 4355, 4467, 4529, 46311, 4355, 4467, 4530,
    46312, 4355, 4467, 4531, 46313, 4355, 4467, 4532, 46314, 4355, 4467, 4533, 46315, 4355, 4467, 4534, 46316, 4355, 4467, 4535, 46317, 4355, 4467, 4536,
    46318, 4355, 4467, 12612, 46319, 4355, 4467, 4538, 46320, 4355, 4467, 4539, 46321, 4355, 4467, 4540, 46322, 4355, 4467, 4364, 46323, 4355, 4467, 4542,
    46324, 4355, 4467, 4543, 46325, 4355, 4467, 4544, 46326, 4355, 4467, 4545, 46327, 4355, 4467, 4546, 46328, 4355, 4468, 0, 46329, 4355, 4468, 4520,
    46330, 4355, 4468, 4521, 46331, 4355, 4468, 4522, 46332, 4355, 4468, 4523, 46333, 4355, 4468, 4524, 46334, 4355, 4468, 4525, 46335, 4355, 4468, 4526,
    46336, 4355, 4468, 4527, 46337, 4355, 4468, 4528, 46338, 4355, 4468, 4529, 46339, 4355, 4468, 4530, 46340, 4355, 4468, 4531, 46341, 4355, 4468, 4532,
    46342, 4355, 4468, 4533, 46343, 4355, 4468, 4534, 46344, 4355, 4468, 4535, 46345, 4355, 4468, 4536, 46346, 4355, 4468, 12612, 46347, 4355, 4468, 4538,
    46348, 4355, 4468, 4539, 46349, 4355, 4468, 4540, 46350, 4355, 4468, 4364, 46351, 4355, 4468, 4542, 46352, 4355, 4468, 4543, 46353, 4355, 4468, 4544,
    46354, 4355, 4468, 4545, 46355, 4355, 4468, 4546, 46356, 4355, 4469, 0, 46357, 4355, 4469, 4520, 46358, 4355, 4469, 4521, 46359, 4355, 4469, 4522,
    46360, 4355, 4469, 4523, 46361, 4355, 4469, 4524, 46362, 4355, 4469, 4525, 46363, 4355, 4469, 4526, 46364, 4355, 4469, 4527, 46365, 4355, 4469, 4528,
    46366, 4355, 4469, 4529, 46367, 4355, 4469, 4530, 46368, 4355, 4469, 4531, 46369, 4355, 4469, 4532, 46370, 4355, 4469, 4533, 46371, 4355, 4469, 4534,
    46372, 4355, 4469, 4535, 46373, 4355, 4469, 4536, 46374, 4355, 4469, 12612, 46375, 4355, 4469, 4538, 46376, 4355, 4469, 4539, 46377, 4355, 4469, 4540,
    46378, 4355, 4469, 4364, 46379, 4355, 4469, 4542, 46380, 4355, 4469, 4543, 46381, 4355, 4469, 4544, 46382, 4355, 4469, 4545, 46383, 4355, 4469, 4546,
    46384, 4356, 4449, 0, 46385, 4356, 4449, 4520, 46386, 4356, 4449, 4521, 46387, 4356, 4449, 4522, 46388, 4356, 4449, 4523, 46389, 4356, 4449, 4524,
    46390, 4356, 4449, 4525, 46391, 4356, 4449, 4526, 46392, 4356, 4449, 4527, 46393, 4356, 4449, 4528, 46394, 4356, 4449, 4529, 46395, 4356, 4449, 4530,
    46396, 4356, 4449, 4531, 46397, 4356, 4449, 4532, 46398, 4356, 4449, 4533, 46399, 4356, 4449, 4534, 46400, 4356, 4449, 4535, 46401, 4356, 4449, 4536,
    46402, 4356, 4449, 12612, 46403, 4356, 4449, 4538, 46404, 4356, 4449, 4539, 46405, 4356, 4449, 4540, 46406, 4356, 4449, 4364, 46407, 4356, 4449, 4542,
    46408, 4356, 4449, 4543, 46409, 4356, 4449, 4544, 46410, 4356, 4449, 4545, 46411, 4356, 4449, 4546, 46412, 4356, 4450, 0, 46413, 4356, 4450, 4520,
    46414, 4356, 4450, 4521, 46415, 4356, 4450, 4522, 46416, 4356, 4450, 4523, 46417, 4356, 4450, 4524, 46418, 4356, 4450, 4525, 46419, 4356, 4450, 4526,
    46420, 4356, 4450, 4527, 46421, 4356, 4450, 4528, 46422, 4356, 4450, 4529, 46423, 4356, 4450, 4530, 46424, 4356, 4450, 4531, 46425, 4356, 4450, 4532,
    46426, 4356, 4450, 4533, 46427, 4356, 4450, 4534, 46428, 4356, 4450, 4535, 46429, 4356, 4450, 4536, 46430, 4356, 4450, 12612, 46431, 4356, 4450, 4538,
    46432, 4356, 4450, 4539, 46433, 4356, 4450, 4540, 46434, 4356, 4450, 4364, 46435, 4356, 4450, 4542, 46436, 4356, 4450, 4543, 46437, 4356, 4450, 4544,
    46438, 4356, 4450, 4545, 46439, 4356, 4450, 4546, 46440, 4356, 4451, 0, 46441, 4356, 4451, 4520, 46442, 4356, 4451, 4521, 46443, 4356, 4451, 4522,
    46444, 4356, 4451, 4523, 46445, 4356, 4451, 4524, 46446, 4356, 4451, 4525, 46447, 4356, 4451, 4526, 46448, 4356, 4451, 4527, 46449, 4356, 4451, 4528,
    46450, 4356, 4451, 4529, 46451, 4356, 4451, 4530, 46452, 4356, 4451, 4531, 46453, 4356, 4451, 4532, 46454, 4356, 4451, 4533, 46455, 4356, 4451, 4534,
    46456, 4356, 4451, 4535, 46457, 4356, 4451, 4536, 46458, 4356, 4451, 12612, 46459, 4356, 4451, 4538, 46460, 4356, 4451, 4539, 46461, 4356, 4451, 4540,
    46462, 4356, 4451, 4364, 46463, 4356, 4451, 4542, 46464, 4356, 4451, 4543, 46465, 4356, 4451, 4544, 46466, 4356, 4451, 4545, 46467, 4356, 4451, 4546,
    46468, 4356, 4452, 0, 46469, 4356, 4452, 4520, 46470, 4356, 4452, 4521, 46471, 4356, 4452, 4522, 46472, 4356, 4452, 4523, 46473, 4356, 4452, 4524,
    46474, 4356, 4452, 4525, 46475, 4356, 4452, 4526, 46476, 4356, 4452, 4527, 46477, 4356, 4452, 4528, 46478, 4356, 4452, 4529, 46479, 4356, 4452, 4530,
    46480, 4356, 4452, 4531, 46481, 4356, 4452, 4532, 46482, 4356, 4452, 4533, 46483, 4356, 4452, 4534, 46484, 4356, 4452, 4535, 46485, 4356, 4452, 4536,
    46486, 4356, 4452, 12612, 46487, 4356, 4452, 4538, 46488, 4356, 4452, 4539, 46489, 4356, 4452, 4540, 46490, 4356, 4452, 4364, 46491, 4356, 4452, 4542,
    46492, 4356, 4452, 4543, 46493, 4356, 4452, 4544, 46494, 4356, 4452, 4545, 46495, 4356, 4452, 4546, 46496, 4356, 4453, 0, 46497, 4356, 4453, 4520,
    46498, 4356, 4453, 4521, 46499, 4356, 4453, 4522, 46500, 4356, 4453, 4523, 46501, 4356, 4453, 4524, 46502, 4356, 4453, 4525, 46503, 4356, 4453, 4526,
    46504, 4356, 4453, 4527, 46505, 4356, 4453, 4528, 46506, 4356, 4453, 4529, 46507, 4356, 4453, 4530, 46508, 4356, 4453, 4531, 46509, 4356, 4453, 4532,
    46510, 4356, 4453, 4533, 46511, 4356, 4453, 4534, 46512, 4356, 4453, 4535, 46513, 4356, 4453, 4536, 46514, 4356, 4453, 12612, 46515, 4356, 4453, 4538,
    46516, 4356, 4453, 4539, 46517, 4356, 4453, 4540, 46518, 4356, 4453, 4364, 46519, 4356, 4453, 4542, 46520, 4356, 4453, 4543, 46521, 4356, 4453, 4544,
    46522, 4356, 4453, 4545, 46523, 4356, 4453, 4546, 46524, 4356, 4454, 0, 46525, 4356, 4454, 4520, 46526, 4356, 4454, 4521, 46527, 4356, 4454, 4522,
    46528, 4356, 4454, 4523, 46529, 4356, 4454, 4524, 46530, 4356, 4454, 4525, 46531, 4356, 4454, 4526, 46532, 4356, 4454, 4527, 46533, 4356, 4454, 4528,
    46534, 4356, 4454, 4529, 46535, 4356, 4454, 4530, 46536, 4356, 4454, 4531, 46537, 4356, 4454, 4532, 46538, 4356, 4454, 4533, 46539, 4356, 4454, 4534,
    46540, 4356, 4454, 4535, 46541, 4356, 4454, 4536, 46542, 4356, 4454, 12612, 46543, 4356, 4454, 4538, 46544, 4356, 4454, 4539, 46545, 4356, 4454, 4540,
    46546, 4356, 4454, 4364, 46547, 4356, 4454, 4542, 46548, 4356, 4454, 4543, 46549, 4356, 4454, 4544, 46550, 4356, 4454, 4545, 46551, 4356, 4454, 4546,
    46552, 4356, 4455, 0, 46553, 4356, 4455, 4520, 46554, 4356, 4455, 4521, 46555, 4356, 4455, 4522, 46556, 4356, 4455, 4523, 46557, 4356, 4455, 4524,
    46558, 4356, 4455, 4525, 46559, 4356, 4455, 4526, 46560, 4356, 4455, 4527, 46561, 4356, 4455, 4528, 46562, 4356, 4455, 4529, 46563, 4356, 4455, 4530,
    46564, 4356, 4455, 4531, 46565, 4356, 4455, 4532, 46566, 4356, 4455, 4533, 46567, 4356, 4455, 4534, 46568, 4356, 4455, 4535, 46569, 4356, 4455, 4536,
    46570, 4356, 4455, 12612, 46571, 4356, 4455, 4538, 46572, 4356, 4455, 4539, 46573, 4356, 4455, 4540, 46574, 4356, 4455, 4364, 46575, 4356, 4455, 4542,
    46576, 4356, 4455, 4543, 46577, 4356, 4455, 4544, 46578, 4356, 4455, 4545, 46579, 4356, 4455, 4546, 46580, 4356, 4456, 0, 46581, 4356, 4456, 4520,
    46582, 4356, 4456, 4521, 46583, 4356, 4456, 4522, 46584, 4356, 4456, 4523, 46585, 4356, 4456, 4524, 46586, 4356, 4456, 4525, 46587, 4356, 4456, 4526,
    46588, 4356, 4456, 4527, 46589, 4356, 4456, 4528, 46590, 4356, 4456, 4529, 46591, 4356, 4456, 4530, 46592, 4356, 4456, 4531, 46593, 4356, 4456, 4532,
    46594, 4356, 4456, 4533, 46595, 4356, 4456, 4534, 46596, 4356, 4456, 4535, 46597, 4356, 4456, 4536, 46598, 4356, 4456, 12612, 46599, 4356, 4456, 4538,
    46600, 4356, 4456, 4539, 46601, 4356, 4456, 4540, 46602, 4356, 4456, 4364, 46603, 4356, 4456, 4542, 46604, 4356, 4456, 4543, 46605, 4356, 4456, 4544,
    46606, 4356, 4456, 4545, 46607, 4356, 4456, 4546, 46608, 4356, 4457, 0, 46609, 4356, 4457, 4520, 46610, 4356, 4457, 4521, 46611, 4356, 4457, 4522,
    46612, 4356, 4457, 4523, 46613, 4356, 4457, 4524, 46614, 4356, 4457, 4525, 46615, 4356, 4457, 4526, 46616, 4356, 4457, 4527, 46617, 4356, 4457, 4528,
    46618, 4356, 4457, 4529, 46619, 4356, 4457, 4530, 46620, 4356, 4457, 4531, 46621, 4356, 4457, 4532, 46622, 4356, 4457, 4533, 46623, 4356, 4457, 4534,
    46624, 4356, 4457, 4535, 46625, 4356, 4457, 4536, 46626, 4356, 4457, 12612, 46627, 4356, 4457, 4538, 46628, 4356, 4457, 4539, 46629, 4356, 4457, 4540,
    46630, 4356, 4457, 4364, 46631, 4356, 4457, 4542, 46632, 4356, 4457, 4543, 46633, 4356, 4457, 4544, 46634, 4356, 4457, 4545, 46635, 4356, 4457, 4546,
    46636, 4356, 4458, 0, 46637, 4356, 4458, 4520, 46638, 4356, 4458, 4521, 46639, 4356, 4458, 4522, 46640, 4356, 4458, 4523, 46641, 4356, 4458, 4524,
    46642, 4356, 4458, 4525, 46643, 4356, 4458, 4526, 46644, 4356, 4458, 4527, 46645, 4356, 4458, 4528, 46646, 4356, 4458, 4529, 46647, 4356, 4458, 4530,
    46648, 4356, 4458, 4531, 46649, 4356, 4458, 4532, 46650, 4356, 4458, 4533, 46651, 4356, 4458, 4534, 46652, 4356, 4458, 4535, 46653, 4356, 4458, 4536,
    46654, 4356, 4458, 12612, 46655, 4356, 4458, 4538, 46656, 4356, 4458, 4539, 46657, 4356, 4458, 4540, 46658, 4356, 4458, 4364, 46659, 4356, 4458, 4542,
    46660, 4356, 4458, 4543, 46661, 4356, 4458, 4544, 46662, 4356, 4458, 4545, 46663, 4356, 4458, 4546, 46664, 4356, 4459, 0, 46665, 4356, 4459, 4520,
    46666, 4356, 4459, 4521, 46667, 4356, 4459, 4522, 46668, 4356, 4459, 4523, 46669, 4356, 4459, 4524, 46670, 4356, 4459, 4525, 46671, 4356, 4459, 4526,
    46672, 4356, 4459, 4527, 46673, 4356, 4459, 4528, 46674, 4356, 4459, 4529, 46675, 4356, 4459, 4530, 46676, 4356, 4459, 4531, 46677, 4356, 4459, 4532,
    46678, 4356, 4459, 4533, 46679, 4356, 4459, 4534, 46680, 4356, 4459, 4535, 46681, 4356, 4459, 4536, 46682, 4356, 4459, 12612, 46683, 4356, 4459, 4538,
    46684, 4356, 4459, 4539, 46685, 4356, 4459, 4540, 46686, 4356, 4459, 4364, 46687, 4356, 4459, 4542, 46688, 4356, 4459, 4543, 46689, 4356, 4459, 4544,
    46690, 4356, 4459, 4545, 46691, 4356, 4459, 4546, 46692, 4356, 4460, 0, 46693, 4356, 4460, 4520, 46694, 4356, 4460, 4521, 46695, 4356, 4460, 4522,
    46696, 4356, 4460, 4523, 46697, 4356, 4460, 4524, 46698, 4356, 4460, 4525, 46699, 4356, 4460, 4526, 46700, 4356, 4460, 4527, 46701, 4356, 4460, 4528,
    46702, 4356, 4460, 4529, 46703, 4356, 4460, 4530, 46704, 4356, 4460, 4531, 46705, 4356, 4460, 4532, 46706, 4356, 4460, 4533, 46707, 4356, 4460, 4534,
    46708, 4356, 4460, 4535, 46709, 4356, 4460, 4536, 46710, 4356, 4460, 12612, 46711, 4356, 4460, 4538, 46712, 4356, 4460, 4539, 46713, 4356, 4460, 4540,
    46714, 4356, 4460, 4364, 46715, 4356, 4460, 4542, 46716, 4356, 4460, 4543, 46717, 4356, 4460, 4544, 46718, 4356, 4460, 4545, 46719, 4356, 4460, 4546,
    46720, 4356, 4461, 0, 46721, 4356, 4461, 4520, 46722, 4356, 4461, 4521, 46723, 4356, 4461, 4522, 46724, 4356, 4461, 4523, 46725, 4356, 4461, 4524,
    46726, 4356, 4461, 4525, 46727, 4356, 4461, 4526, 46728, 4356, 4461, 4527, 46729, 4356, 4461, 4528, 46730, 4356, 4461, 4529, 46731, 4356, 4461, 4530,
    46732, 4356, 4461, 4531, 46733, 4356, 4461, 4532, 46734, 4356, 4461, 4533, 46735, 4356, 4461, 4534, 46736, 4356, 4461, 4535, 46737, 4356, 4461, 4536,
    46738, 4356, 4461, 12612, 46739, 4356, 4461, 4538, 46740, 4356, 4461, 4539, 46741, 4356, 4461, 4540, 46742, 4356, 4461, 4364, 46743, 4356, 4461, 4542,
    46744, 4356, 4461, 4543, 46745, 4356, 4461, 4544, 46746, 4356, 4461, 4545, 46747, 4356, 4461, 4546, 46748, 4356, 4462, 0, 46749, 4356, 4462, 4520,
    46750, 4356, 4462, 4521, 46751, 4356, 4462, 4522, 46752, 4356, 4462, 4523, 46753, 4356, 4462, 4524, 46754, 4356, 4462, 4525, 46755, 4356, 4462, 4526,
    46756, 4356, 4462, 4527, 46757, 4356, 4462, 4528, 46758, 4356, 4462, 4529, 46759, 4356, 4462, 4530, 46760, 4356, 4462, 4531, 46761, 4356, 4462, 4532,
    46762, 4356, 4462, 4533, 46763, 4356, 4462, 4534, 46764, 4356, 4462, 4535, 46765, 4356, 4462, 4536, 46766, 4356, 4462, 12612, 46767, 4356, 4462, 4538,
    46768, 4356, 4462, 4539, 46769, 4356, 4462, 4540, 46770, 4356, 4462, 4364, 46771, 4356, 4462, 4542, 46772, 4356, 4462, 4543, 46773, 4356, 4462, 4544,
    46774, 4356, 4462, 4545, 46775, 4356, 4462, 4546, 46776, 4356, 4463, 0, 46777, 4356, 4463, 4520, 46778, 4356, 4463, 4521, 46779, 4356, 4463, 4522,
    46780, 4356, 4463, 4523, 46781, 4356, 4463, 4524, 46782, 4356, 4463, 4525, 46783, 4356, 4463, 4526, 46784, 4356, 4463, 4527, 46785, 4356, 4463, 4528,
    46786, 4356, 4463, 4529, 46787, 4356, 4463, 4530, 46788, 4356, 4463, 4531, 46789, 4356, 4463, 4532, 46790, 4356, 4463, 4533, 46791, 4356, 4463, 4534,
    46792, 4356, 4463, 4535, 46793, 4356, 4463, 4536, 46794, 4356, 4463, 12612, 46795, 4356, 4463, 4538, 46796, 4356, 4463, 4539, 46797, 4356, 4463, 4540,
    46798, 4356, 4463, 4364, 46799, 4356, 4463, 4542, 46800, 4356, 4463, 4543, 46801, 4356, 4463, 4544, 46802, 4356, 4463, 4545, 46803, 4356, 4463, 4546,
    46804, 4356, 4464, 0, 46805, 4356, 4464, 4520, 46806, 4356, 4464, 4521, 46807, 4356, 4464, 4522, 46808, 4356, 4464, 4523, 46809, 4356, 4464, 4524,
    46810, 4356, 4464, 4525, 46811, 4356, 4464, 4526, 46812, 4356, 4464, 4527, 46813, 4356, 4464, 4528, 46814, 4356, 4464, 4529, 46815, 4356, 4464, 4530,
    46816, 4356, 4464, 4531, 46817, 4356, 4464, 4532, 46818, 4356, 4464, 4533, 46819, 4356, 4464, 4534, 46820, 4356, 4464, 4535, 46821, 4356, 4464, 4536,
    46822, 4356, 4464, 12612, 46823, 4356, 4464, 4538, 46824, 4356, 4464, 4539, 46825, 4356, 4464, 4540, 46826, 4356, 4464, 4364, 46827, 4356, 4464, 4542,
    46828, 4356, 4464, 4543, 46829, 4356, 4464, 4544, 46830, 4356, 4464, 4545, 46831, 4356, 4464, 4546, 46832, 4356, 4465, 0, 46833, 4356, 4465, 4520,
    46834, 4356, 4465, 4521, 46835, 4356, 4465, 4522, 46836, 4356, 4465, 4523, 46837, 4356, 4465, 4524, 46838, 4356, 4465, 4525, 46839, 4356, 4465, 4526,
    46840, 4356, 4465, 4527, 46841, 4356, 4465, 4528, 46842, 4356, 4465, 4529, 46843, 4356, 4465, 4530, 46844, 4356, 4465, 4531, 46845, 4356, 4465, 4532,
    46846, 4356, 4465, 4533, 46847, 4356, 4465, 4534, 46848, 4356, 4465, 4535, 46849, 4356, 4465, 4536, 46850, 4356, 4465, 12612, 46851, 4356, 4465, 4538,
    46852, 4356, 4465, 4539, 46853, 4356, 4465, 4540, 46854, 4356, 4465, 4364, 46855, 4356, 4465, 4542, 46856, 4356, 4465, 4543, 46857, 4356, 4465, 4544,
    46858, 4356, 4465, 4545, 46859, 4356, 4465, 4546, 46860, 4356, 4466, 0, 46861, 4356, 4466, 4520, 46862, 4356, 4466, 4521, 46863, 4356, 4466, 4522,
    46864, 4356, 4466, 4523, 46865, 4356, 4466, 4524, 46866, 4356, 4466, 4525, 46867, 4356, 4466, 4526, 46868, 4356, 4466, 4527, 46869, 4356, 4466, 4528,
    46870, 4356, 4466, 4529, 46871, 4356, 4466, 4530, 46872, 4356, 4466, 4531, 46873, 4356, 4466, 4532, 46874, 4356, 4466, 4533, 46875, 4356, 4466, 4534,
    46876, 4356, 4466, 4535, 46877, 4356, 4466, 4536, 46878, 4356, 4466, 12612, 46879, 4356, 4466, 4538, 46880, 4356, 4466, 4539, 46881, 4356, 4466, 4540,
    46882, 4356, 4466, 4364, 46883, 4356, 4466, 4542, 46884, 4356, 4466, 4543, 46885, 4356, 4466, 4544, 46886, 4356, 4466, 4545, 46887, 4356, 4466, 4546,
    46888, 4356, 4467, 0, 46889, 4356, 4467, 4520, 46890, 4356, 4467, 4521, 46891, 4356, 4467, 4522, 46892, 4356, 4467, 4523, 46893, 4356, 4467, 4524,
    46894, 4356, 4467, 4525, 46895, 4356, 4467, 4526, 46896, 4356, 4467, 4527, 46897, 4356, 4467, 4528, 46898, 4356, 4467, 4529, 46899, 4356, 4467, 4530,
    46900, 4356, 4467, 4531, 46901, 4356, 4467, 4532, 46902, 4356, 4467, 4533, 46903, 4356, 4467, 4534, 46904, 4356, 4467, 4535, 46905, 4356, 4467, 4536,
    46906, 4356, 4467, 12612, 46907, 4356, 4467, 4538, 46908, 4356, 4467, 4539, 46909, 4356, 4467, 4540, 46910, 4356, 4467, 4364, 46911, 4356, 4467, 4542,
    46912, 4356, 4467, 4543, 46913, 4356, 4467, 4544, 46914, 4356, 4467, 4545, 46915, 4356, 4467, 4546, 46916, 4356, 4468, 0, 46917, 4356, 4468, 4520,
    46918, 4356, 4468, 4521, 46919, 4356, 4468, 4522, 46920, 4356, 4468, 4523, 46921, 4356, 4468, 4524, 46922, 4356, 4468, 4525, 46923, 4356, 4468, 4526,
    46924, 4356, 4468, 4527, 46925, 4356, 4468, 4528, 46926, 4356, 4468, 4529, 46927, 4356, 4468, 4530, 46928, 4356, 4468, 4531, 46929, 4356, 4468, 4532,
    46930, 4356, 4468, 4533, 46931, 4356, 4468, 4534, 46932, 4356, 4468, 4535, 46933, 4356, 4468, 4536, 46934, 4356, 4468, 12612, 46935, 4356, 4468, 4538,
    46936, 4356, 4468, 4539, 46937, 4356, 4468, 4540, 46938, 4356, 4468, 4364, 46939, 4356, 4468, 4542, 46940, 4356, 4468, 4543, 46941, 4356, 4468, 4544,
    46942, 4356, 4468, 4545, 46943, 4356, 4468, 4546, 46944, 4356, 4469, 0, 46945, 4356, 4469, 4520, 46946, 4356, 4469, 4521, 46947, 4356, 4469, 4522,
    46948, 4356, 4469, 4523, 46949, 4356, 4469, 4524, 46950, 4356, 4469, 4525, 46951, 4356, 4469, 4526, 46952, 4356, 4469, 4527, 46953, 4356, 4469, 4528,
    46954, 4356, 4469, 4529, 46955, 4356, 4469, 4530, 46956, 4356, 4469, 4531, 46957, 4356, 4469, 4532, 46958, 4356, 4469, 4533, 46959, 4356, 4469, 4534,
    46960, 4356, 4469, 4535, 46961, 4356, 4469, 4536, 46962, 4356, 4469, 12612, 46963, 4356, 4469, 4538, 46964, 4356, 4469, 4539, 46965, 4356, 4469, 4540,
    46966, 4356, 4469, 4364, 46967, 4356, 4469, 4542, 46968, 4356, 4469, 4543, 46969, 4356, 4469, 4544, 46970, 4356, 4469, 4545, 46971, 4356, 4469, 4546,
    46972, 4357, 4449, 0, 46973, 4357, 4449, 4520, 46974, 4357, 4449, 4521, 46975, 4357, 4449, 4522, 46976, 4357, 4449, 4523, 46977, 4357, 4449, 4524,
    46978, 4357, 4449, 4525, 46979, 4357, 4449, 4526, 46980, 4357, 4449, 4527, 46981, 4357, 4449, 4528, 46982, 4357, 4449, 4529, 46983, 4357, 4449, 4530,
    46984, 4357, 4449, 4531, 46985, 4357, 4449, 4532, 46986, 4357, 4449, 4533, 46987, 4357, 4449, 4534, 46988, 4357, 4449, 4535, 46989, 4357, 4449, 4536,
    46990, 4357, 4449, 12612, 46991, 4357, 4449, 4538, 46992, 4357, 4449, 4539, 46993, 4357, 4449, 4540, 46994, 4357, 4449, 4364, 46995, 4357, 4449, 4542,
    46996, 4357, 4449, 4543, 46997, 4357, 4449, 4544, 46998, 4357, 4449, 4545, 46999, 4357, 4449, 4546, 47000, 4357, 4450, 0, 47001, 4357, 4450, 4520,
    47002, 4357, 4450, 4521, 47003, 4357, 4450, 4522, 47004, 4357, 4450, 4523, 47005, 4357, 4450, 4524, 47006, 4357, 4450, 4525, 47007, 4357, 4450, 4526,
    47008, 4357, 4450, 4527, 47009, 4357, 4450, 4528, 47010, 4357, 4450, 4529, 47011, 4357, 4450, 4530, 47012, 4357, 4450, 4531, 47013, 4357, 4450, 4532,
    47014, 4357, 4450, 4533, 47015, 4357, 4450, 4534, 47016, 4357, 4450, 4535, 47017, 4357, 4450, 4536, 47018, 4357, 4450, 12612, 47019, 4357, 4450, 4538,
    47020, 4357, 4450, 4539, 47021, 4357, 4450, 4540, 47022, 4357, 4450, 4364, 47023, 4357, 4450, 4542, 47024, 4357, 4450, 4543, 47025, 4357, 4450, 4544,
    47026, 4357, 4450, 4545, 47027, 4357, 4450, 4546, 47028, 4357, 4451, 0, 47029, 4357, 4451, 4520, 47030, 4357, 4451, 4521, 47031, 4357, 4451, 4522,
    47032, 4357, 4451, 4523, 47033, 4357, 4451, 4524, 47034, 4357, 4451, 4525, 47035, 4357, 4451, 4526, 47036, 4357, 4451, 4527, 47037, 4357, 4451, 4528,
    47038, 4357, 4451, 4529, 47039, 4357, 4451, 4530, 47040, 4357, 4451, 4531, 47041, 4357, 4451, 4532, 47042, 4357, 4451, 4533, 47043, 4357, 4451, 4534,
    47044, 4357, 4451, 4535, 47045, 4357, 4451, 4536, 47046, 4357, 4451, 12612, 47047, 4357, 4451, 4538, 47048, 4357, 4451, 4539, 47049, 4357, 4451, 4540,
    47050, 4357, 4451, 4364, 47051, 4357, 4451, 4542, 47052, 4357, 4451, 4543, 47053, 4357, 4451, 4544, 47054, 4357, 4451, 4545, 47055, 4357, 4451, 4546,
    47056, 4357, 4452, 0, 47057, 4357, 4452, 4520, 47058, 4357, 4452, 4521, 47059, 4357, 4452, 4522, 47060, 4357, 4452, 4523, 47061, 4357, 4452, 4524,
    47062, 4357, 4452, 4525, 47063, 4357, 4452, 4526, 47064, 4357, 4452, 4527, 47065, 4357, 4452, 4528, 47066, 4357, 4452, 4529, 47067, 4357, 4452, 4530,
    47068, 4357, 4452, 4531, 47069, 4357, 4452, 4532, 47070, 4357, 4452, 4533, 47071, 4357, 4452, 4534, 47072, 4357, 4452, 4535, 47073, 4357, 4452, 4536,
    47074, 4357, 4452, 12612, 47075, 4357, 4452, 4538, 47076, 4357, 4452, 4539, 47077, 4357, 4452, 4540, 47078, 4357, 4452, 4364, 47079, 4357, 4452, 4542,
    47080, 4357, 4452, 4543, 47081, 4357, 4452, 4544, 47082, 4357, 4452, 4545, 47083, 4357, 4452, 4546, 47084, 4357, 4453, 0, 47085, 4357, 4453, 4520,
    47086, 4357, 4453, 4521, 47087, 4357, 4453, 4522, 47088, 4357, 4453, 4523, 47089, 4357, 4453, 4524, 47090, 4357, 4453, 4525, 47091, 4357, 4453, 4526,
    47092, 4357, 4453, 4527, 47093, 4357, 4453, 4528, 47094, 4357, 4453, 4529, 47095, 4357, 4453, 4530, 47096, 4357, 4453, 4531, 47097, 4357, 4453, 4532,
    47098, 4357, 4453, 4533, 47099, 4357, 4453, 4534, 47100, 4357, 4453, 4535, 47101, 4357, 4453, 4536, 47102, 4357, 4453, 12612, 47103, 4357, 4453, 4538,
    47104, 4357, 4453, 4539, 47105, 4357, 4453, 4540, 47106, 4357, 4453, 4364, 47107, 4357, 4453, 4542, 47108, 4357, 4453, 4543, 47109, 4357, 4453, 4544,
    47110, 4357, 4453, 4545, 47111, 4357, 4453, 4546, 47112, 4357, 4454, 0, 47113, 4357, 4454, 4520, 47114, 4357, 4454, 4521, 47115, 4357, 4454, 4522,
    47116, 4357, 4454, 4523, 47117, 4357, 4454, 4524, 47118, 4357, 4454, 4525, 47119, 4357, 4454, 4526, 47120, 4357, 4454, 4527, 47121, 4357, 4454, 4528,
    47122, 4357, 4454, 4529, 47123, 4357, 4454, 4530, 47124, 4357, 4454, 4531, 47125, 4357, 4454, 4532, 47126, 4357, 4454, 4533, 47127, 4357, 4454, 4534,
    47128, 4357, 4454, 4535, 47129, 4357, 4454, 4536, 47130, 4357, 4454, 12612, 47131, 4357, 4454, 4538, 47132, 4357, 4454, 4539, 47133, 4357, 4454, 4540,
    47134, 4357, 4454, 4364, 47135, 4357, 4454, 4542, 47136, 4357, 4454, 4543, 47137, 4357, 4454, 4544, 47138, 4357, 4454, 4545, 47139, 4357, 4454, 4546,
    47140, 4357, 4455, 0, 47141, 4357, 4455, 4520, 47142, 4357, 4455, 4521, 47143, 4357, 4455, 4522, 47144, 4357, 4455, 4523, 47145, 4357, 4455, 4524,
    47146, 4357, 4455, 4525, 47147, 4357, 4455, 4526, 47148, 4357, 4455, 4527, 47149, 4357, 4455, 4528, 47150, 4357, 4455, 4529, 47151, 4357, 4455, 4530,
    47152, 4357, 4455, 4531, 47153, 4357, 4455, 4532, 47154, 4357, 4455, 4533, 47155, 4357, 4455, 4534, 47156, 4357, 4455, 4535, 47157, 4357, 4455, 4536,
    47158, 4357, 4455, 12612, 47159, 4357, 4455, 4538, 47160, 4357, 4455, 4539, 47161, 4357, 4455, 4540, 47162, 4357, 4455, 4364, 47163, 4357, 4455, 4542,
    47164, 4357, 4455, 4543, 47165, 4357, 4455, 4544, 47166, 4357, 4455, 4545, 47167, 4357, 4455, 4546, 47168, 4357, 4456, 0, 47169, 4357, 4456, 4520,
    47170, 4357, 4456, 4521, 47171, 4357, 4456, 4522, 47172, 4357, 4456, 4523, 47173, 4357, 4456, 4524, 47174, 4357, 4456, 4525, 47175, 4357, 4456, 4526,
    47176, 4357, 4456, 4527, 47177, 4357, 4456, 4528, 47178, 4357, 4456, 4529, 47179, 4357, 4456, 4530, 47180, 4357, 4456, 4531, 47181, 4357, 4456, 4532,
    47182, 4357, 4456, 4533, 47183, 4357, 4456, 4534, 47184, 4357, 4456, 4535, 47185, 4357, 4456, 4536, 47186, 4357, 4456, 12612, 47187, 4357, 4456, 4538,
    47188, 4357, 4456, 4539, 47189, 4357, 4456, 4540, 47190, 4357, 4456, 4364, 47191, 4357, 4456, 4542, 47192, 4357, 4456, 4543, 47193, 4357, 4456, 4544,
    47194, 4357, 4456, 4545, 47195, 4357, 4456, 4546, 47196, 4357, 4457, 0, 47197, 4357, 4457, 4520, 47198, 4357, 4457, 4521, 47199, 4357, 4457, 4522,
    47200, 4357, 4457, 4523, 47201, 4357, 4457, 4524, 47202, 4357, 4457, 4525, 47203, 4357, 4457, 4526, 47204, 4357, 4457, 4527, 47205, 4357, 4457, 4528,
    47206, 4357, 4457, 4529, 47207, 4357, 4457, 4530, 47208, 4357, 4457, 4531, 47209, 4357, 4457, 4532, 47210, 4357, 4457, 4533, 47211, 4357, 4457, 4534,
    47212, 4357, 4457, 4535, 47213, 4357, 4457, 4536, 47214, 4357, 4457, 12612, 47215, 4357, 4457, 4538, 47216, 4357, 4457, 4539, 47217, 4357, 4457, 4540,
    47218, 4357, 4457, 4364, 47219, 4357, 4457, 4542, 47220, 4357, 4457, 4543, 47221, 4357, 4457, 4544, 47222, 4357, 4457, 4545, 47223, 4357, 4457, 4546,
    47224, 4357, 4458, 0, 47225, 4357, 4458, 4520, 47226, 4357, 4458, 4521, 47227, 4357, 4458, 4522, 47228, 4357, 4458, 4523, 47229, 4357, 4458, 4524,
    47230, 4357, 4458, 4525, 47231, 4357, 4458, 4526, 47232, 4357, 4458, 4527, 47233, 4357, 4458, 4528, 47234, 4357, 4458, 4529, 47235, 4357, 4458, 4530,
    47236, 4357, 4458, 4531, 47237, 4357, 4458, 4532, 47238, 4357, 4458, 4533, 47239, 4357, 4458, 4534, 47240, 4357, 4458, 4535, 47241, 4357, 4458, 4536,
    47242, 4357, 4458, 12612, 47243, 4357, 4458, 4538, 47244, 4357, 4458, 4539, 47245, 4357, 4458, 4540, 47246, 4357, 4458, 4364, 47247, 4357, 4458, 4542,
    47248, 4357, 4458, 4543, 47249, 4357, 4458, 4544, 47250, 4357, 4458, 4545, 47251, 4357, 4458, 4546, 47252, 4357, 4459, 0, 47253, 4357, 4459, 4520,
    47254, 4357, 4459, 4521, 47255, 4357, 4459, 4522, 47256, 4357, 4459, 4523, 47257, 4357, 4459, 4524, 47258, 4357, 4459, 4525, 47259, 4357, 4459, 4526,
    47260, 4357, 4459, 4527, 47261, 4357, 4459, 4528, 47262, 4357, 4459, 4529, 47263, 4357, 4459, 4530, 47264, 4357, 4459, 4531, 47265, 4357, 4459, 4532,
    47266, 4357, 4459, 4533, 47267, 4357, 4459, 4534, 47268, 4357, 4459, 4535, 47269, 4357, 4459, 4536, 47270, 4357, 4459, 12612, 47271, 4357, 4459, 4538,
    47272, 4357, 4459, 4539, 47273, 4357, 4459, 4540, 47274, 4357, 4459, 4364, 47275, 4357, 4459, 4542, 47276, 4357, 4459, 4543, 47277, 4357, 4459, 4544,
    47278, 4357, 4459, 4545, 47279, 4357, 4459, 4546, 47280, 4357, 4460, 0, 47281, 4357, 4460, 4520, 47282, 4357, 4460, 4521, 47283, 4357, 4460, 4522,
    47284, 4357, 4460, 4523, 47285, 4357, 4460, 4524, 47286, 4357, 4460, 4525, 47287, 4357, 4460, 4526, 47288, 4357, 4460, 4527, 47289, 4357, 4460, 4528,
    47290, 4357, 4460, 4529, 47291, 4357, 4460, 4530, 47292, 4357, 4460, 4531, 47293, 4357, 4460, 4532, 47294, 4357, 4460, 4533, 47295, 4357, 4460, 4534,
    47296, 4357, 4460, 4535, 47297, 4357, 4460, 4536, 47298, 4357, 4460, 12612, 47299, 4357, 4460, 4538, 47300, 4357, 4460, 4539, 47301, 4357, 4460, 4540,
    47302, 4357, 4460, 4364, 47303, 4357, 4460, 4542, 47304, 4357, 4460, 4543, 47305, 4357, 4460, 4544, 47306, 4357, 4460, 4545, 47307, 4357, 4460, 4546,
    47308, 4357, 4461, 0, 47309, 4357, 4461, 4520, 47310, 4357, 4461, 4521, 47311, 4357, 4461, 4522, 47312, 4357, 4461, 4523, 47313, 4357, 4461, 4524,
    47314, 4357, 4461, 4525, 47315, 4357, 4461, 4526, 47316, 4357, 4461, 4527, 47317, 4357, 4461, 4528, 47318, 4357, 4461, 4529, 47319, 4357, 4461, 4530,
    47320, 4357, 4461, 4531, 47321, 4357, 4461, 4532, 47322, 4357, 4461, 4533, 47323, 4357, 4461, 4534, 47324, 4357, 4461, 4535, 47325, 4357, 4461, 4536,
    47326, 4357, 4461, 12612, 47327, 4357, 4461, 4538, 47328, 4357, 4461, 4539, 47329, 4357, 4461, 4540, 47330, 4357, 4461, 4364, 47331, 4357, 4461, 4542,
    47332, 4357, 4461, 4543, 47333, 4357, 4461, 4544, 47334, 4357, 4461, 4545, 47335, 4357, 4461, 4546, 47336, 4357, 4462, 0, 47337, 4357, 4462, 4520,
    47338, 4357, 4462, 4521, 47339, 4357, 4462, 4522, 47340, 4357, 4462, 4523, 47341, 4357, 4462, 4524, 47342, 4357, 4462, 4525, 47343, 4357, 4462, 4526,
    47344, 4357, 4462, 4527, 47345, 4357, 4462, 4528, 47346, 4357, 4462, 4529, 47347, 4357, 4462, 4530, 47348, 4357, 4462, 4531, 47349, 4357, 4462, 4532,
    47350, 4357, 4462, 4533, 47351, 4357, 4462, 4534, 47352, 4357, 4462, 4535, 47353, 4357, 4462, 4536, 47354, 4357, 4462, 12612, 47355, 4357, 4462, 4538,
    47356, 4357, 4462, 4539, 47357, 4357, 4462, 4540, 47358, 4357, 4462, 4364, 47359, 4357, 4462, 4542, 47360, 4357, 4462, 4543, 47361, 4357, 4462, 4544,
    47362, 4357, 4462, 4545, 47363, 4357, 4462, 4546, 47364, 4357, 4463, 0, 47365, 4357, 4463, 4520, 47366, 4357, 4463, 4521, 47367, 4357, 4463, 4522,
    47368, 4357, 4463, 4523, 47369, 4357, 4463, 4524, 47370, 4357, 4463, 4525, 47371, 4357, 4463, 4526, 47372, 4357, 4463, 4527, 47373, 4357, 4463, 4528,
    47374, 4357, 4463, 4529, 47375, 4357, 4463, 4530, 47376, 4357, 4463, 4531, 47377, 4357, 4463, 4532, 47378, 4357, 4463, 4533, 47379, 4357, 4463, 4534,
    47380, 4357, 4463, 4535, 47381, 4357, 4463, 4536, 47382, 4357, 4463, 12612, 47383, 4357, 4463, 4538, 47384, 4357, 4463, 4539, 47385, 4357, 4463, 4540,
    47386, 4357, 4463, 4364, 47387, 4357, 4463, 4542, 47388, 4357, 4463, 4543, 47389, 4357, 4463, 4544, 47390, 4357, 4463, 4545, 47391, 4357, 4463, 4546,
    47392, 4357, 4464, 0, 47393, 4357, 4464, 4520, 47394, 4357, 4464, 4521, 47395, 4357, 4464, 4522, 47396, 4357, 4464, 4523, 47397, 4357, 4464, 4524,
    47398, 4357, 4464, 4525, 47399, 4357, 4464, 4526, 47400, 4357, 4464, 4527, 47401, 4357, 4464, 4528, 47402, 4357, 4464, 4529, 47403, 4357, 4464, 4530,
    47404, 4357, 4464, 4531, 47405, 4357, 4464, 4532, 47406, 4357, 4464, 4533, 47407, 4357, 4464, 4534, 47408, 4357, 4464, 4535, 47409, 4357, 4464, 4536,
    47410, 4357, 4464, 12612, 47411, 4357, 4464, 4538, 47412, 4357, 4464, 4539, 47413, 4357, 4464, 4540, 47414, 4357, 4464, 4364, 47415, 4357, 4464, 4542,
    47416, 4357, 4464, 4543, 47417, 4357, 4464, 4544, 47418, 4357, 4464, 4545, 47419, 4357, 4464, 4546, 47420, 4357, 4465, 0, 47421, 4357, 4465, 4520,
    47422, 4357, 4465, 4521, 47423, 4357, 4465, 4522, 47424, 4357, 4465, 4523, 47425, 4357, 4465, 4524, 47426, 4357, 4465, 4525, 47427, 4357, 4465, 4526,
    47428, 4357, 4465, 4527, 47429, 4357, 4465, 4528, 47430, 4357, 4465, 4529, 47431, 4357, 4465, 4530, 47432, 4357, 4465, 4531, 47433, 4357, 4465, 4532,
    47434, 4357, 4465, 4533, 47435, 4357, 4465, 4534, 47436, 4357, 4465, 4535, 47437, 4357, 4465, 4536, 47438, 4357, 4465, 12612, 47439, 4357, 4465, 4538,
    47440, 4357, 4465, 4539, 47441, 4357, 4465, 4540, 47442, 4357, 4465, 4364, 47443, 4357, 4465, 4542, 47444, 4357, 4465, 4543, 47445, 4357, 4465, 4544,
    47446, 4357, 4465, 4545, 47447, 4357, 4465, 4546, 47448, 4357, 4466, 0, 47449, 4357, 4466, 4520, 47450, 4357, 4466, 4521, 47451, 4357, 4466, 4522,
    47452, 4357, 4466, 4523, 47453, 4357, 4466, 4524, 47454, 4357, 4466, 4525, 47455, 4357, 4466, 4526, 47456, 4357, 4466, 4527, 47457, 4357, 4466, 4528,
    47458, 4357, 4466, 4529, 47459, 4357, 4466, 4530, 47460, 4357, 4466, 4531, 47461, 4357, 4466, 4532, 47462, 4357, 4466, 4533, 47463, 4357, 4466, 4534,
    47464, 4357, 4466, 4535, 47465, 4357, 4466, 4536, 47466, 4357, 4466, 12612, 47467, 4357, 4466, 4538, 47468, 4357, 4466, 4539, 47469, 4357, 4466, 4540,
    47470, 4357, 4466, 4364, 47471, 4357, 4466, 4542, 47472, 4357, 4466, 4543, 47473, 4357, 4466, 4544, 47474, 4357, 4466, 4545, 47475, 4357, 4466, 4546,
    47476, 4357, 4467, 0, 47477, 4357, 4467, 4520, 47478, 4357, 4467, 4521, 47479, 4357, 4467, 4522, 47480, 4357, 4467, 4523, 47481, 4357, 4467, 4524,
    47482, 4357, 4467, 4525, 47483, 4357, 4467, 4526, 47484, 4357, 4467, 4527, 47485, 4357, 4467, 4528, 47486, 4357, 4467, 4529, 47487, 4357, 4467, 4530,
    47488, 4357, 4467, 4531, 47489, 4357, 4467, 4532, 47490, 4357, 4467, 4533, 47491, 4357, 4467, 4534, 47492, 4357, 4467, 4535, 47493, 4357, 4467, 4536,
    47494, 4357, 4467, 12612, 47495, 4357, 4467, 4538, 47496, 4357, 4467, 4539, 47497, 4357, 4467, 4540, 47498, 4357, 4467, 4364, 47499, 4357, 4467, 4542,
    47500, 4357, 4467, 4543, 47501, 4357, 4467, 4544, 47502, 4357, 4467, 4545, 47503, 4357, 4467, 4546, 47504, 4357, 4468, 0, 47505, 4357, 4468, 4520,
    47506, 4357, 4468, 4521, 47507, 4357, 4468, 4522, 47508, 4357, 4468, 4523, 47509, 4357, 4468, 4524, 47510, 4357, 4468, 4525, 47511, 4357, 4468, 4526,
    47512, 4357, 4468, 4527, 47513, 4357, 4468, 4528, 47514, 4357, 4468, 4529, 47515, 4357, 4468, 4530, 47516, 4357, 4468, 4531, 47517, 4357, 4468, 4532,
    47518, 4357, 4468, 4533, 47519, 4357, 4468, 4534, 47520, 4357, 4468, 4535, 47521, 4357, 4468, 4536, 47522, 4357, 4468, 12612, 47523, 4357, 4468, 4538,
    47524, 4357, 4468, 4539, 47525, 4357, 4468, 4540, 47526, 4357, 4468, 4364, 47527, 4357, 4468, 4542, 47528, 4357, 4468, 4543, 47529, 4357, 4468, 4544,
    47530, 4357, 4468, 4545, 47531, 4357, 4468, 4546, 47532, 4357, 4469, 0, 47533, 4357, 4469, 4520, 47534, 4357, 4469, 4521, 47535, 4357, 4469, 4522,
    47536, 4357, 4469, 4523, 47537, 4357, 4469, 4524, 47538, 4357, 4469, 4525, 47539, 4357, 4469, 4526, 47540, 4357, 4469, 4527, 47541, 4357, 4469, 4528,
    47542, 4357, 4469, 4529, 47543, 4357, 4469, 4530, 47544, 4357, 4469, 4531, 47545, 4357, 4469, 4532, 47546, 4357, 4469, 4533, 47547, 4357, 4469, 4534,
    47548, 4357, 4469, 4535, 47549, 4357, 4469, 4536, 47550, 4357, 4469, 12612, 47551, 4357, 4469, 4538, 47552, 4357, 4469, 4539, 47553, 4357, 4469, 4540,
    47554, 4357, 4469, 4364, 47555, 4357, 4469, 4542, 47556, 4357, 4469, 4543, 47557, 4357, 4469, 4544, 47558, 4357, 4469, 4545, 47559, 4357, 4469, 4546,
    47560, 4358, 4449, 0, 47561, 4358, 4449, 4520, 47562, 4358, 4449, 4521, 47563, 4358, 4449, 4522, 47564, 4358, 4449, 4523, 47565, 4358, 4449, 4524,
    47566, 4358, 4449, 4525, 47567, 4358, 4449, 4526, 47568, 4358, 4449, 4527, 47569, 4358, 4449, 4528, 47570, 4358, 4449, 4529, 47571, 4358, 4449, 4530,
    47572, 4358, 4449, 4531, 47573, 4358, 4449, 4532, 47574, 4358, 4449, 4533, 47575, 4358, 4449, 4534, 47576, 4358, 4449, 4535, 47577, 4358, 4449, 4536,
    47578, 4358, 4449, 12612, 47579, 4358, 4449, 4538, 47580, 4358, 4449, 4539, 47581, 4358, 4449, 4540, 47582, 4358, 4449, 4364, 47583, 4358, 4449, 4542,
    47584, 4358, 4449, 4543, 47585, 4358, 4449, 4544, 47586, 4358, 4449, 4545, 47587, 4358, 4449, 4546, 47588, 4358, 4450, 0, 47589, 4358, 4450, 4520,
    47590, 4358, 4450, 4521, 47591, 4358, 4450, 4522, 47592, 4358, 4450, 4523, 47593, 4358, 4450, 4524, 47594, 4358, 4450, 4525, 47595, 4358, 4450, 4526,
    47596, 4358, 4450, 4527, 47597, 4358, 4450, 4528, 47598, 4358, 4450, 4529, 47599, 4358, 4450, 4530, 47600, 4358, 4450, 4531, 47601, 4358, 4450, 4532,
    47602, 4358, 4450, 4533, 47603, 4358, 4450, 4534, 47604, 4358, 4450, 4535, 47605, 4358, 4450, 4536, 47606, 4358, 4450, 12612, 47607, 4358, 4450, 4538,
    47608, 4358, 4450, 4539, 47609, 4358, 4450, 4540, 47610, 4358, 4450, 4364, 47611, 4358, 4450, 4542, 47612, 4358, 4450, 4543, 47613, 4358, 4450, 4544,
    47614, 4358, 4450, 4545, 47615, 4358, 4450, 4546, 47616, 4358, 4451, 0, 47617, 4358, 4451, 4520, 47618, 4358, 4451, 4521, 47619, 4358, 4451, 4522,
    47620, 4358, 4451, 4523, 47621, 4358, 4451, 4524, 47622, 4358, 4451, 4525, 47623, 4358, 4451, 4526, 47624, 4358, 4451, 4527, 47625, 4358, 4451, 4528,
    47626, 4358, 4451, 4529, 47627, 4358, 4451, 4530, 47628, 4358, 4451, 4531, 47629, 4358, 4451, 4532, 47630, 4358, 4451, 4533, 47631, 4358, 4451, 4534,
    47632, 4358, 4451, 4535, 47633, 4358, 4451, 4536, 47634, 4358, 4451, 12612, 47635, 4358, 4451, 4538, 47636, 4358, 4451, 4539, 47637, 4358, 4451, 4540,
    47638, 4358, 4451, 4364, 47639, 4358, 4451, 4542, 47640, 4358, 4451, 4543, 47641, 4358, 4451, 4544, 47642, 4358, 4451, 4545, 47643, 4358, 4451, 4546,
    47644, 4358, 4452, 0, 47645, 4358, 4452, 4520, 47646, 4358, 4452, 4521, 47647, 4358, 4452, 4522, 47648, 4358, 4452, 4523, 47649, 4358, 4452, 4524,
    47650, 4358, 4452, 4525, 47651, 4358, 4452, 4526, 47652, 4358, 4452, 4527, 47653, 4358, 4452, 4528, 47654, 4358, 4452, 4529, 47655, 4358, 4452, 4530,
    47656, 4358, 4452, 4531, 47657, 4358, 4452, 4532, 47658, 4358, 4452, 4533, 47659, 4358, 4452, 4534, 47660, 4358, 4452, 4535, 47661, 4358, 4452, 4536,
    47662, 4358, 4452, 12612, 47663, 4358, 4452, 4538, 47664, 4358, 4452, 4539, 47665, 4358, 4452, 4540, 47666, 4358, 4452, 4364, 47667, 4358, 4452, 4542,
    47668, 4358, 4452, 4543, 47669, 4358, 4452, 4544, 47670, 4358, 4452, 4545, 47671, 4358, 4452, 4546, 47672, 4358, 4453, 0, 47673, 4358, 4453, 4520,
    47674, 4358, 4453, 4521, 47675, 4358, 4453, 4522, 47676, 4358, 4453, 4523, 47677, 4358, 4453, 4524, 47678, 4358, 4453, 4525, 47679, 4358, 4453, 4526,
    47680, 4358, 4453, 4527, 47681, 4358, 4453, 4528, 47682, 4358, 4453, 4529, 47683, 4358, 4453, 4530, 47684, 4358, 4453, 4531, 47685, 4358, 4453, 4532,
    47686, 4358, 4453, 4533, 47687, 4358, 4453, 4534, 47688, 4358, 4453, 4535, 47689, 4358, 4453, 4536, 47690, 4358, 4453, 12612, 47691, 4358, 4453, 4538,
    47692, 4358, 4453, 4539, 47693, 4358, 4453, 4540, 47694, 4358, 4453, 4364, 47695, 4358, 4453, 4542, 47696, 4358, 4453, 4543, 47697, 4358, 4453, 4544,
    47698, 4358, 4453, 4545, 47699, 4358, 4453, 4546, 47700, 4358, 4454, 0, 47701, 4358, 4454, 4520, 47702, 4358, 4454, 4521, 47703, 4358, 4454, 4522,
    47704, 4358, 4454, 4523, 47705, 4358, 4454, 4524, 47706, 4358, 4454, 4525, 47707, 4358, 4454, 4526, 47708, 4358, 4454, 4527, 47709, 4358, 4454, 4528,
    47710, 4358, 4454, 4529, 47711, 4358, 4454, 4530, 47712, 4358, 4454, 4531, 47713, 4358, 4454, 4532, 47714, 4358, 4454, 4533, 47715, 4358, 4454, 4534,
    47716, 4358, 4454, 4535, 47717, 4358, 4454, 4536, 47718, 4358, 4454, 12612, 47719, 4358, 4454, 4538, 47720, 4358, 4454, 4539, 47721, 4358, 4454, 4540,
    47722, 4358, 4454, 4364, 47723, 4358, 4454, 4542, 47724, 4358, 4454, 4543, 47725, 4358, 4454, 4544, 47726, 4358, 4454, 4545, 47727, 4358, 4454, 4546,
    47728, 4358, 4455, 0, 47729, 4358, 4455, 4520, 47730, 4358, 4455, 4521, 47731, 4358, 4455, 4522, 47732, 4358, 4455, 4523, 47733, 4358, 4455, 4524,
    47734, 4358, 4455, 4525, 47735, 4358, 4455, 4526, 47736, 4358, 4455, 4527, 47737, 4358, 4455, 4528, 47738, 4358, 4455, 4529, 47739, 4358, 4455, 4530,
    47740, 4358, 4455, 4531, 47741, 4358, 4455, 4532, 47742, 4358, 4455, 4533, 47743, 4358, 4455, 4534, 47744, 4358, 4455, 4535, 47745, 4358, 4455, 4536,
    47746, 4358, 4455, 12612, 47747, 4358, 4455, 4538, 47748, 4358, 4455, 4539, 47749, 4358, 4455, 4540, 47750, 4358, 4455, 4364, 47751, 4358, 4455, 4542,
    47752, 4358, 4455, 4543, 47753, 4358, 4455, 4544, 47754, 4358, 4455, 4545, 47755, 4358, 4455, 4546, 47756, 4358, 4456, 0, 47757, 4358, 4456, 4520,
    47758, 4358, 4456, 4521, 47759, 4358, 4456, 4522, 47760, 4358, 4456, 4523, 47761, 4358, 4456, 4524, 47762, 4358, 4456, 4525, 47763, 4358, 4456, 4526,
    47764, 4358, 4456, 4527, 47765, 4358, 4456, 4528, 47766, 4358, 4456, 4529, 47767, 4358, 4456, 4530, 47768, 4358, 4456, 4531, 47769, 4358, 4456, 4532,
    47770, 4358, 4456, 4533, 47771, 4358, 4456, 4534, 47772, 4358, 4456, 4535, 47773, 4358, 4456, 4536, 47774, 4358, 4456, 12612, 47775, 4358, 4456, 4538,
    47776, 4358, 4456, 4539, 47777, 4358, 4456, 4540, 47778, 4358, 4456, 4364, 47779, 4358, 4456, 4542, 47780, 4358, 4456, 4543, 47781, 4358, 4456, 4544,
    47782, 4358, 4456, 4545, 47783, 4358, 4456, 4546, 47784, 4358, 4457, 0, 47785, 4358, 4457, 4520, 47786, 4358, 4457, 4521, 47787, 4358, 4457, 4522,
    47788, 4358, 4457, 4523, 47789, 4358, 4457, 4524, 47790, 4358, 4457, 4525, 47791, 4358, 4457, 4526, 47792, 4358, 4457, 4527, 47793, 4358, 4457, 4528,
    47794, 4358, 4457, 4529, 47795, 4358, 4457, 4530, 47796, 4358, 4457, 4531, 47797, 4358, 4457, 4532, 47798, 4358, 4457, 4533, 47799, 4358, 4457, 4534,
    47800, 4358, 4457, 4535, 47801, 4358, 4457, 4536, 47802, 4358, 4457, 12612, 47803, 4358, 4457, 4538, 47804, 4358, 4457, 4539, 47805, 4358, 4457, 4540,
    47806, 4358, 4457, 4364, 47807, 4358, 4457, 4542, 47808, 4358, 4457, 4543, 47809, 4358, 4457, 4544, 47810, 4358, 4457, 4545, 47811, 4358, 4457, 4546,
    47812, 4358, 4458, 0, 47813, 4358, 4458, 4520, 47814, 4358, 4458, 4521, 47815, 4358, 4458, 4522, 47816, 4358, 4458, 4523, 47817, 4358, 4458, 4524,
    47818, 4358, 4458, 4525, 47819, 4358, 4458, 4526, 47820, 4358, 4458, 4527, 47821, 4358, 4458, 4528, 47822, 4358, 4458, 4529, 47823, 4358, 4458, 4530,
    47824, 4358, 4458, 4531, 47825, 4358, 4458, 4532, 47826, 4358, 4458, 4533, 47827, 4358, 4458, 4534, 47828, 4358, 4458, 4535, 47829, 4358, 4458, 4536,
    47830, 4358, 4458, 12612, 47831, 4358, 4458, 4538, 47832, 4358, 4458, 4539, 47833, 4358, 4458, 4540, 47834, 4358, 4458, 4364, 47835, 4358, 4458, 4542,
    47836, 4358, 4458, 4543, 47837, 4358, 4458, 4544, 47838, 4358, 4458, 4545, 47839, 4358, 4458, 4546, 47840, 4358, 4459, 0, 47841, 4358, 4459, 4520,
    47842, 4358, 4459, 4521, 47843, 4358, 4459, 4522, 47844, 4358, 4459, 4523, 47845, 4358, 4459, 4524, 47846, 4358, 4459, 4525, 47847, 4358, 4459, 4526,
    47848, 4358, 4459, 4527, 47849, 4358, 4459, 4528, 47850, 4358, 4459, 4529, 47851, 4358, 4459, 4530, 47852, 4358, 4459, 4531, 47853, 4358, 4459, 4532,
    47854, 4358, 4459, 4533, 47855, 4358, 4459, 4534, 47856, 4358, 4459, 4535, 47857, 4358, 4459, 4536, 47858, 4358, 4459, 12612, 47859, 4358, 4459, 4538,
    47860, 4358, 4459, 4539, 47861, 4358, 4459, 4540, 47862, 4358, 4459, 4364, 47863, 4358, 4459, 4542, 47864, 4358, 4459, 4543, 47865, 4358, 4459, 4544,
    47866, 4358, 4459, 4545, 47867, 4358, 4459, 4546, 47868, 4358, 4460, 0, 47869, 4358, 4460, 4520, 47870, 4358, 4460, 4521, 47871, 4358, 4460, 4522,
    47872, 4358, 4460, 4523, 47873, 4358, 4460, 4524, 47874, 4358, 4460, 4525, 47875, 4358, 4460, 4526, 47876, 4358, 4460, 4527, 47877, 4358, 4460, 4528,
    47878, 4358, 4460, 4529, 47879, 4358, 4460, 4530, 47880, 4358, 4460, 4531, 47881, 4358, 4460, 4532, 47882, 4358, 4460, 4533, 47883, 4358, 4460, 4534,
    47884, 4358, 4460, 4535, 47885, 4358, 4460, 4536, 47886, 4358, 4460, 12612, 47887, 4358, 4460, 4538, 47888, 4358, 4460, 4539, 47889, 4358, 4460, 4540,
    47890, 4358, 4460, 4364, 47891, 4358, 4460, 4542, 47892, 4358, 4460, 4543, 47893, 4358, 4460, 4544, 47894, 4358, 4460, 4545, 47895, 4358, 4460, 4546,
    47896, 4358, 4461, 0, 47897, 4358, 4461, 4520, 47898, 4358, 4461, 4521, 47899, 4358, 4461, 4522, 47900, 4358, 4461, 4523, 47901, 4358, 4461, 4524,
    47902, 4358, 4461, 4525, 47903, 4358, 4461, 4526, 47904, 4358, 4461, 4527, 47905, 4358, 4461, 4528, 47906, 4358, 4461, 4529, 47907, 4358, 4461, 4530,
    47908, 4358, 4461, 4531, 47909, 4358, 4461, 4532, 47910, 4358, 4461, 4533, 47911, 4358, 4461, 4534, 47912, 4358, 4461, 4535, 47913, 4358, 4461, 4536,
    47914, 4358, 4461, 12612, 47915, 4358, 4461, 4538, 47916, 4358, 4461, 4539, 47917, 4358, 4461, 4540, 47918, 4358, 4461, 4364, 47919, 4358, 4461, 4542,
    47920, 4358, 4461, 4543, 47921, 4358, 4461, 4544, 47922, 4358, 4461, 4545, 47923, 4358, 4461, 4546, 47924, 4358, 4462, 0, 47925, 4358, 4462, 4520,
    47926, 4358, 4462, 4521, 47927, 4358, 4462, 4522, 47928, 4358, 4462, 4523, 47929, 4358, 4462, 4524, 47930, 4358, 4462, 4525, 47931, 4358, 4462, 4526,
    47932, 4358, 4462, 4527, 47933, 4358, 4462, 4528, 47934, 4358, 4462, 4529, 47935, 4358, 4462, 4530, 47936, 4358, 4462, 4531, 47937, 4358, 4462, 4532,
    47938, 4358, 4462, 4533, 47939, 4358, 4462, 4534, 47940, 4358, 4462, 4535, 47941, 4358, 4462, 4536, 47942, 4358, 4462, 12612, 47943, 4358, 4462, 4538,
    47944, 4358, 4462, 4539, 47945, 4358, 4462, 4540, 47946, 4358, 4462, 4364, 47947, 4358, 4462, 4542, 47948, 4358, 4462, 4543, 47949, 4358, 4462, 4544,
    47950, 4358, 4462, 4545, 47951, 4358, 4462, 4546, 47952, 4358, 4463, 0, 47953, 4358, 4463, 4520, 47954, 4358, 4463, 4521, 47955, 4358, 4463, 4522,
    47956, 4358, 4463, 4523, 47957, 4358, 4463, 4524, 47958, 4358, 4463, 4525, 47959, 4358, 4463, 4526, 47960, 4358, 4463, 4527, 47961, 4358, 4463, 4528,
    47962, 4358, 4463, 4529, 47963, 4358, 4463, 4530, 47964, 4358, 4463, 4531, 47965, 4358, 4463, 4532, 47966, 4358, 4463, 4533, 47967, 4358, 4463, 4534,
    47968, 4358, 4463, 4535, 47969, 4358, 4463, 4536, 47970, 4358, 4463, 12612, 47971, 4358, 4463, 4538, 47972, 4358, 4463, 4539, 47973, 4358, 4463, 4540,
    47974, 4358, 4463, 4364, 47975, 4358, 4463, 4542, 47976, 4358, 4463, 4543, 47977, 4358, 4463, 4544, 47978, 4358, 4463, 4545, 47979, 4358, 4463, 4546,
    47980, 4358, 4464, 0, 47981, 4358, 4464, 4520, 47982, 4358, 4464, 4521, 47983, 4358, 4464, 4522, 47984, 4358, 4464, 4523, 47985, 4358, 4464, 4524,
    47986, 4358, 4464, 4525, 47987, 4358, 4464, 4526, 47988, 4358, 4464, 4527, 47989, 4358, 4464, 4528, 47990, 4358, 4464, 4529, 47991, 4358, 4464, 4530,
    47992, 4358, 4464, 4531, 47993, 4358, 4464, 4532, 47994, 4358, 4464, 4533, 47995, 4358, 4464, 4534, 47996, 4358, 4464, 4535, 47997, 4358, 4464, 4536,
    47998, 4358, 4464, 12612, 47999, 4358, 4464, 4538, 48000, 4358, 4464, 4539, 48001, 4358, 4464, 4540, 48002, 4358, 4464, 4364, 48003, 4358, 4464, 4542,
    48004, 4358, 4464, 4543, 48005, 4358, 4464, 4544, 48006, 4358, 4464, 4545, 48007, 4358, 4464, 4546, 48008, 4358, 4465, 0, 48009, 4358, 4465, 4520,
    48010, 4358, 4465, 4521, 48011, 4358, 4465, 4522, 48012, 4358, 4465, 4523, 48013, 4358, 4465, 4524, 48014, 4358, 4465, 4525, 48015, 4358, 4465, 4526,
    48016, 4358, 4465, 4527, 48017, 4358, 4465, 4528, 48018, 4358, 4465, 4529, 48019, 4358, 4465, 4530, 48020, 4358, 4465, 4531, 48021, 4358, 4465, 4532,
    48022, 4358, 4465, 4533, 48023, 4358, 4465, 4534, 48024, 4358, 4465, 4535, 48025, 4358, 4465, 4536, 48026, 4358, 4465, 12612, 48027, 4358, 4465, 4538,
    48028, 4358, 4465, 4539, 48029, 4358, 4465, 4540, 48030, 4358, 4465, 4364, 48031, 4358, 4465, 4542, 48032, 4358, 4465, 4543, 48033, 4358, 4465, 4544,
    48034, 4358, 4465, 4545, 48035, 4358, 4465, 4546, 48036, 4358, 4466, 0, 48037, 4358, 4466, 4520, 48038, 4358, 4466, 4521, 48039, 4358, 4466, 4522,
    48040, 4358, 4466, 4523, 48041, 4358, 4466, 4524, 48042, 4358, 4466, 4525, 48043, 4358, 4466, 4526, 48044, 4358, 4466, 4527, 48045, 4358, 4466, 4528,
    48046, 4358, 4466, 4529, 48047, 4358, 4466, 4530, 48048, 4358, 4466, 4531, 48049, 4358, 4466, 4532, 48050, 4358, 4466, 4533, 48051, 4358, 4466, 4534,
    48052, 4358, 4466, 4535, 48053, 4358, 4466, 4536, 48054, 4358, 4466, 12612, 48055, 4358, 4466, 4538, 48056, 4358, 4466, 4539, 48057, 4358, 4466, 4540,
    48058, 4358, 4466, 4364, 48059, 4358, 4466, 4542, 48060, 4358, 4466, 4543, 48061, 4358, 4466, 4544, 48062, 4358, 4466, 4545, 48063, 4358, 4466, 4546,
    48064, 4358, 4467, 0, 48065, 4358, 4467, 4520, 48066, 4358, 4467, 4521, 48067, 4358, 4467, 4522, 48068, 4358, 4467, 4523, 48069, 4358, 4467, 4524,
    48070, 4358, 4467, 4525, 48071, 4358, 4467, 4526, 48072, 4358, 4467, 4527, 48073, 4358, 4467, 4528, 48074, 4358, 4467, 4529, 48075, 4358, 4467, 4530,
    48076, 4358, 4467, 4531, 48077, 4358, 4467, 4532, 48078, 4358, 4467, 4533, 48079, 4358, 4467, 4534, 48080, 4358, 4467, 4535, 48081, 4358, 4467, 4536,
    48082, 4358, 4467, 12612, 48083, 4358, 4467, 4538, 48084, 4358, 4467, 4539, 48085, 4358, 4467, 4540, 48086, 4358, 4467, 4364, 48087, 4358, 4467, 4542,
    48088, 4358, 4467, 4543, 48089, 4358, 4467, 4544, 48090, 4358, 4467, 4545, 48091, 4358, 4467, 4546, 48092, 4358, 4468, 0, 48093, 4358, 4468, 4520,
    48094, 4358, 4468, 4521, 48095, 4358, 4468, 4522, 48096, 4358, 4468, 4523, 48097, 4358, 4468, 4524, 48098, 4358, 4468, 4525, 48099, 4358, 4468, 4526,
    48100, 4358, 4468, 4527, 48101, 4358, 4468, 4528, 48102, 4358, 4468, 4529, 48103, 4358, 4468, 4530, 48104, 4358, 4468, 4531, 48105, 4358, 4468, 4532,
    48106, 4358, 4468, 4533, 48107, 4358, 4468, 4534, 48108, 4358, 4468, 4535, 48109, 4358, 4468, 4536, 48110, 4358, 4468, 12612, 48111, 4358, 4468, 4538,
    48112, 4358, 4468, 4539, 48113, 4358, 4468, 4540, 48114, 4358, 4468, 4364, 48115, 4358, 4468, 4542, 48116, 4358, 4468, 4543, 48117, 4358, 4468, 4544,
    48118, 4358, 4468, 4545, 48119, 4358, 4468, 4546, 48120, 4358, 4469, 0, 48121, 4358, 4469, 4520, 48122, 4358, 4469, 4521, 48123, 4358, 4469, 4522,
    48124, 4358, 4469, 4523, 48125, 4358, 4469, 4524, 48126, 4358, 4469, 4525, 48127, 4358, 4469, 4526, 48128, 4358, 4469, 4527, 48129, 4358, 4469, 4528,
    48130, 4358, 4469, 4529, 48131, 4358, 4469, 4530, 48132, 4358, 4469, 4531, 48133, 4358, 4469, 4532, 48134, 4358, 4469, 4533, 48135, 4358, 4469, 4534,
    48136, 4358, 4469, 4535, 48137, 4358, 4469, 4536, 48138, 4358, 4469, 12612, 48139, 4358, 4469, 4538, 48140, 4358, 4469, 4539, 48141, 4358, 4469, 4540,
    48142, 4358, 4469, 4364, 48143, 4358, 4469, 4542, 48144, 4358, 4469, 4543, 48145, 4358, 4469, 4544, 48146, 4358, 4469, 4545, 48147, 4358, 4469, 4546,
    48148, 4359, 4449, 0, 48149, 4359, 4449, 4520, 48150, 4359, 4449, 4521, 48151, 4359, 4449, 4522, 48152, 4359, 4449, 4523, 48153, 4359, 4449, 4524,
    48154, 4359, 4449, 4525, 48155, 4359, 4449, 4526, 48156, 4359, 4449, 4527, 48157, 4359, 4449, 4528, 48158, 4359, 4449, 4529, 48159, 4359, 4449, 4530,
    48160, 4359, 4449, 4531, 48161, 4359, 4449, 4532, 48162, 4359, 4449, 4533, 48163, 4359, 4449, 4534, 48164, 4359, 4449, 4535, 48165, 4359, 4449, 4536,
    48166, 4359, 4449, 12612, 48167, 4359, 4449, 4538, 48168, 4359, 4449, 4539, 48169, 4359, 4449, 4540, 48170, 4359, 4449, 4364, 48171, 4359, 4449, 4542,
    48172, 4359, 4449, 4543, 48173, 4359, 4449, 4544, 48174, 4359, 4449, 4545, 48175, 4359, 4449, 4546, 48176, 4359, 4450, 0, 48177, 4359, 4450, 4520,
    48178, 4359, 4450, 4521, 48179, 4359, 4450, 4522, 48180, 4359, 4450, 4523, 48181, 4359, 4450, 4524, 48182, 4359, 4450, 4525, 48183, 4359, 4450, 4526,
    48184, 4359, 4450, 4527, 48185, 4359, 4450, 4528, 48186, 4359, 4450, 4529, 48187, 4359, 4450, 4530, 48188, 4359, 4450, 4531, 48189, 4359, 4450, 4532,
    48190, 4359, 4450, 4533, 48191, 4359, 4450, 4534, 48192, 4359, 4450, 4535, 48193, 4359, 4450, 4536, 48194, 4359, 4450, 12612, 48195, 4359, 4450, 4538,
    48196, 4359, 4450, 4539, 48197, 4359, 4450, 4540, 48198, 4359, 4450, 4364, 48199, 4359, 4450, 4542, 48200, 4359, 4450, 4543, 48201, 4359, 4450, 4544,
    48202, 4359, 4450, 4545, 48203, 4359, 4450, 4546, 48204, 4359, 4451, 0, 48205, 4359, 4451, 4520, 48206, 4359, 4451, 4521, 48207, 4359, 4451, 4522,
    48208, 4359, 4451, 4523, 48209, 4359, 4451, 4524, 48210, 4359, 4451, 4525, 48211, 4359, 4451, 4526, 48212, 4359, 4451, 4527, 48213, 4359, 4451, 4528,
    48214, 4359, 4451, 4529, 48215, 4359, 4451, 4530, 48216, 4359, 4451, 4531, 48217, 4359, 4451, 4532, 48218, 4359, 4451, 4533, 48219, 4359, 4451, 4534,
    48220, 4359, 4451, 4535, 48221, 4359, 4451, 4536, 48222, 4359, 4451, 12612, 48223, 4359, 4451, 4538, 48224, 4359, 4451, 4539, 48225, 4359, 4451, 4540,
    48226, 4359, 4451, 4364, 48227, 4359, 4451, 4542, 48228, 4359, 4451, 4543, 48229, 4359, 4451, 4544, 48230, 4359, 4451, 4545, 48231, 4359, 4451, 4546,
    48232, 4359, 4452, 0, 48233, 4359, 4452, 4520, 48234, 4359, 4452, 4521, 48235, 4359, 4452, 4522, 48236, 4359, 4452, 4523, 48237, 4359, 4452, 4524,
    48238, 4359, 4452, 4525, 48239, 4359, 4452, 4526, 48240, 4359, 4452, 4527, 48241, 4359, 4452, 4528, 48242, 4359, 4452, 4529, 48243, 4359, 4452, 4530,
    48244, 4359, 4452, 4531, 48245, 4359, 4452, 4532, 48246, 4359, 4452, 4533, 48247, 4359, 4452, 4534, 48248, 4359, 4452, 4535, 48249, 4359, 4452, 4536,
    48250, 4359, 4452, 12612, 48251, 4359, 4452, 4538, 48252, 4359, 4452, 4539, 48253, 4359, 4452, 4540, 48254, 4359, 4452, 4364, 48255, 4359, 4452, 4542,
    48256, 4359, 4452, 4543, 48257, 4359, 4452, 4544, 48258, 4359, 4452, 4545, 48259, 4359, 4452, 4546, 48260, 4359, 4453, 0, 48261, 4359, 4453, 4520,
    48262, 4359, 4453, 4521, 48263, 4359, 4453, 4522, 48264, 4359, 4453, 4523, 48265, 4359, 4453, 4524, 48266, 4359, 4453, 4525, 48267, 4359, 4453, 4526,
    48268, 4359, 4453, 4527, 48269, 4359, 4453, 4528, 48270, 4359, 4453, 4529, 48271, 4359, 4453, 4530, 48272, 4359, 4453, 4531, 48273, 4359, 4453, 4532,
    48274, 4359, 4453, 4533, 48275, 4359, 4453, 4534, 48276, 4359, 4453, 4535, 48277, 4359, 4453, 4536, 48278, 4359, 4453, 12612, 48279, 4359, 4453, 4538,
    48280, 4359, 4453, 4539, 48281, 4359, 4453, 4540, 48282, 4359, 4453, 4364, 48283, 4359, 4453, 4542, 48284, 4359, 4453, 4543, 48285, 4359, 4453, 4544,
    48286, 4359, 4453, 4545, 48287, 4359, 4453, 4546, 48288, 4359, 4454, 0, 48289, 4359, 4454, 4520, 48290, 4359, 4454, 4521, 48291, 4359, 4454, 4522,
    48292, 4359, 4454, 4523, 48293, 4359, 4454, 4524, 48294, 4359, 4454, 4525, 48295, 4359, 4454, 4526, 48296, 4359, 4454, 4527, 48297, 4359, 4454, 4528,
    48298, 4359, 4454, 4529, 48299, 4359, 4454, 4530, 48300, 4359, 4454, 4531, 48301, 4359, 4454, 4532, 48302, 4359, 4454, 4533, 48303, 4359, 4454, 4534,
    48304, 4359, 4454, 4535, 48305, 4359, 4454, 4536, 48306, 4359, 4454, 12612, 48307, 4359, 4454, 4538, 48308, 4359, 4454, 4539, 48309, 4359, 4454, 4540,
    48310, 4359, 4454, 4364, 48311, 4359, 4454, 4542, 48312, 4359, 4454, 4543, 48313, 4359, 4454, 4544, 48314, 4359, 4454, 4545, 48315, 4359, 4454, 4546,
    48316, 4359, 4455, 0, 48317, 4359, 4455, 4520, 48318, 4359, 4455, 4521, 48319, 4359, 4455, 4522, 48320, 4359, 4455, 4523, 48321, 4359, 4455, 4524,
    48322, 4359, 4455, 4525, 48323, 4359, 4455, 4526, 48324, 4359, 4455, 4527, 48325, 4359, 4455, 4528, 48326, 4359, 4455, 4529, 48327, 4359, 4455, 4530,
    48328, 4359, 4455, 4531, 48329, 4359, 4455, 4532, 48330, 4359, 4455, 4533, 48331, 4359, 4455, 4534, 48332, 4359, 4455, 4535, 48333, 4359, 4455, 4536,
    48334, 4359, 4455, 12612, 48335, 4359, 4455, 4538, 48336, 4359, 4455, 4539, 48337, 4359, 4455, 4540, 48338, 4359, 4455, 4364, 48339, 4359, 4455, 4542,
    48340, 4359, 4455, 4543, 48341, 4359, 4455, 4544, 48342, 4359, 4455, 4545, 48343, 4359, 4455, 4546, 48344, 4359, 4456, 0, 48345, 4359, 4456, 4520,
    48346, 4359, 4456, 4521, 48347, 4359, 4456, 4522, 48348, 4359, 4456, 4523, 48349, 4359, 4456, 4524, 48350, 4359, 4456, 4525, 48351, 4359, 4456, 4526,
    48352, 4359, 4456, 4527, 48353, 4359, 4456, 4528, 48354, 4359, 4456, 4529, 48355, 4359, 4456, 4530, 48356, 4359, 4456, 4531, 48357, 4359, 4456, 4532,
    48358, 4359, 4456, 4533, 48359, 4359, 4456, 4534, 48360, 4359, 4456, 4535, 48361, 4359, 4456, 4536, 48362, 4359, 4456, 12612, 48363, 4359, 4456, 4538,
    48364, 4359, 4456, 4539, 48365, 4359, 4456, 4540, 48366, 4359, 4456, 4364, 48367, 4359, 4456, 4542, 48368, 4359, 4456, 4543, 48369, 4359, 4456, 4544,
    48370, 4359, 4456, 4545, 48371, 4359, 4456, 4546, 48372, 4359, 4457, 0, 48373, 4359, 4457, 4520, 48374, 4359, 4457, 4521, 48375, 4359, 4457, 4522,
    48376, 4359, 4457, 4523, 48377, 4359, 4457, 4524, 48378, 4359, 4457, 4525, 48379, 4359, 4457, 4526, 48380, 4359, 4457, 4527, 48381, 4359, 4457, 4528,
    48382, 4359, 4457, 4529, 48383, 4359, 4457, 4530, 48384, 4359, 4457, 4531, 48385, 4359, 4457, 4532, 48386, 4359, 4457, 4533, 48387, 4359, 4457, 4534,
    48388, 4359, 4457, 4535, 48389, 4359, 4457, 4536, 48390, 4359, 4457, 12612, 48391, 4359, 4457, 4538, 48392, 4359, 4457, 4539, 48393, 4359, 4457, 4540,
    48394, 4359, 4457, 4364, 48395, 4359, 4457, 4542, 48396, 4359, 4457, 4543, 48397, 4359, 4457, 4544, 48398, 4359, 4457, 4545, 48399, 4359, 4457, 4546,
    48400, 4359, 4458, 0, 48401, 4359, 4458, 4520, 48402, 4359, 4458, 4521, 48403, 4359, 4458, 4522, 48404, 4359, 4458, 4523, 48405, 4359, 4458, 4524,
    48406, 4359, 4458, 4525, 48407, 4359, 4458, 4526, 48408, 4359, 4458, 4527, 48409, 4359, 4458, 4528, 48410, 4359, 4458, 4529, 48411, 4359, 4458, 4530,
    48412, 4359, 4458, 4531, 48413, 4359, 4458, 4532, 48414, 4359, 4458, 4533, 48415, 4359, 4458, 4534, 48416, 4359, 4458, 4535, 48417, 4359, 4458, 4536,
    48418, 4359, 4458, 12612, 48419, 4359, 4458, 4538, 48420, 4359, 4458, 4539, 48421, 4359, 4458, 4540, 48422, 4359, 4458, 4364, 48423, 4359, 4458, 4542,
    48424, 4359, 4458, 4543, 48425, 4359, 4458, 4544, 48426, 4359, 4458, 4545, 48427, 4359, 4458, 4546, 48428, 4359, 4459, 0, 48429, 4359, 4459, 4520,
    48430, 4359, 4459, 4521, 48431, 4359, 4459, 4522, 48432, 4359, 4459, 4523, 48433, 4359, 4459, 4524, 48434, 4359, 4459, 4525, 48435, 4359, 4459, 4526,
    48436, 4359, 4459, 4527, 48437, 4359, 4459, 4528, 48438, 4359, 4459, 4529, 48439, 4359, 4459, 4530, 48440, 4359, 4459, 4531, 48441, 4359, 4459, 4532,
    48442, 4359, 4459, 4533, 48443, 4359, 4459, 4534, 48444, 4359, 4459, 4535, 48445, 4359, 4459, 4536, 48446, 4359, 4459, 12612, 48447, 4359, 4459, 4538,
    48448, 4359, 4459, 4539, 48449, 4359, 4459, 4540, 48450, 4359, 4459, 4364, 48451, 4359, 4459, 4542, 48452, 4359, 4459, 4543, 48453, 4359, 4459, 4544,
    48454, 4359, 4459, 4545, 48455, 4359, 4459, 4546, 48456, 4359, 4460, 0, 48457, 4359, 4460, 4520, 48458, 4359, 4460, 4521, 48459, 4359, 4460, 4522,
    48460, 4359, 4460, 4523, 48461, 4359, 4460, 4524, 48462, 4359, 4460, 4525, 48463, 4359, 4460, 4526, 48464, 4359, 4460, 4527, 48465, 4359, 4460, 4528,
    48466, 4359, 4460, 4529, 48467, 4359, 4460, 4530, 48468, 4359, 4460, 4531, 48469, 4359, 4460, 4532, 48470, 4359, 4460, 4533, 48471, 4359, 4460, 4534,
    48472, 4359, 4460, 4535, 48473, 4359, 4460, 4536, 48474, 4359, 4460, 12612, 48475, 4359, 4460, 4538, 48476, 4359, 4460, 4539, 48477, 4359, 4460, 4540,
    48478, 4359, 4460, 4364, 48479, 4359, 4460, 4542, 48480, 4359, 4460, 4543, 48481, 4359, 4460, 4544, 48482, 4359, 4460, 4545, 48483, 4359, 4460, 4546,
    48484, 4359, 4461, 0, 48485, 4359, 4461, 4520, 48486, 4359, 4461, 4521, 48487, 4359, 4461, 4522, 48488, 4359, 4461, 4523, 48489, 4359, 4461, 4524,
    48490, 4359, 4461, 4525, 48491, 4359, 4461, 4526, 48492, 4359, 4461, 4527, 48493, 4359, 4461, 4528, 48494, 4359, 4461, 4529, 48495, 4359, 4461, 4530,
    48496, 4359, 4461, 4531, 48497, 4359, 4461, 4532, 48498, 4359, 4461, 4533, 48499, 4359, 4461, 4534, 48500, 4359, 4461, 4535, 48501, 4359, 4461, 4536,
    48502, 4359, 4461, 12612, 48503, 4359, 4461, 4538, 48504, 4359, 4461, 4539, 48505, 4359, 4461, 4540, 48506, 4359, 4461, 4364, 48507, 4359, 4461, 4542,
    48508, 4359, 4461, 4543, 48509, 4359, 4461, 4544, 48510, 4359, 4461, 4545, 48511, 4359, 4461, 4546, 48512, 4359, 4462, 0, 48513, 4359, 4462, 4520,
    48514, 4359, 4462, 4521, 48515, 4359, 4462, 4522, 48516, 4359, 4462, 4523, 48517, 4359, 4462, 4524, 48518, 4359, 4462, 4525, 48519, 4359, 4462, 4526,
    48520, 4359, 4462, 4527, 48521, 4359, 4462, 4528, 48522, 4359, 4462, 4529, 48523, 4359, 4462, 4530, 48524, 4359, 4462, 4531, 48525, 4359, 4462, 4532,
    48526, 4359, 4462, 4533, 48527, 4359, 4462, 4534, 48528, 4359, 4462, 4535, 48529, 4359, 4462, 4536, 48530, 4359, 4462, 12612, 48531, 4359, 4462, 4538,
    48532, 4359, 4462, 4539, 48533, 4359, 4462, 4540, 48534, 4359, 4462, 4364, 48535, 4359, 4462, 4542, 48536, 4359, 4462, 4543, 48537, 4359, 4462, 4544,
    48538, 4359, 4462, 4545, 48539, 4359, 4462, 4546, 48540, 4359, 4463, 0, 48541, 4359, 4463, 4520, 48542, 4359, 4463, 4521, 48543, 4359, 4463, 4522,
    48544, 4359, 4463, 4523, 48545, 4359, 4463, 4524, 48546, 4359, 4463, 4525, 48547, 4359, 4463, 4526, 48548, 4359, 4463, 4527, 48549, 4359, 4463, 4528,
    48550, 4359, 4463, 4529, 48551, 4359, 4463, 4530, 48552, 4359, 4463, 4531, 48553, 4359, 4463, 4532, 48554, 4359, 4463, 4533, 48555, 4359, 4463, 4534,
    48556, 4359, 4463, 4535, 48557, 4359, 4463, 4536, 48558, 4359, 4463, 12612, 48559, 4359, 4463, 4538, 48560, 4359, 4463, 4539, 48561, 4359, 4463, 4540,
    48562, 4359, 4463, 4364, 48563, 4359, 4463, 4542, 48564, 4359, 4463, 4543, 48565, 4359, 4463, 4544, 48566, 4359, 4463, 4545, 48567, 4359, 4463, 4546,
    48568, 4359, 4464, 0, 48569, 4359, 4464, 4520, 48570, 4359, 4464, 4521, 48571, 4359, 4464, 4522, 48572, 4359, 4464, 4523, 48573, 4359, 4464, 4524,
    48574, 4359, 4464, 4525, 48575, 4359, 4464, 4526, 48576, 4359, 4464, 4527, 48577, 4359, 4464, 4528, 48578, 4359, 4464, 4529, 48579, 4359, 4464, 4530,
    48580, 4359, 4464, 4531, 48581, 4359, 4464, 4532, 48582, 4359, 4464, 4533, 48583, 4359, 4464, 4534, 48584, 4359, 4464, 4535, 48585, 4359, 4464, 4536,
    48586, 4359, 4464, 12612, 48587, 4359, 4464, 4538, 48588, 4359, 4464, 4539, 48589, 4359, 4464, 4540, 48590, 4359, 4464, 4364, 48591, 4359, 4464, 4542,
    48592, 4359, 4464, 4543, 48593, 4359, 4464, 4544, 48594, 4359, 4464, 4545, 48595, 4359, 4464, 4546, 48596, 4359, 4465, 0, 48597, 4359, 4465, 4520,
    48598, 4359, 4465, 4521, 48599, 4359, 4465, 4522, 48600, 4359, 4465, 4523, 48601, 4359, 4465, 4524, 48602, 4359, 4465, 4525, 48603, 4359, 4465, 4526,
    48604, 4359, 4465, 4527, 48605, 4359, 4465, 4528, 48606, 4359, 4465, 4529, 48607, 4359, 4465, 4530, 48608, 4359, 4465, 4531, 48609, 4359, 4465, 4532,
    48610, 4359, 4465, 4533, 48611, 4359, 4465, 4534, 48612, 4359, 4465, 4535, 48613, 4359, 4465, 4536, 48614, 4359, 4465, 12612, 48615, 4359, 4465, 4538,
    48616, 4359, 4465, 4539, 48617, 4359, 4465, 4540, 48618, 4359, 4465, 4364, 48619, 4359, 4465, 4542, 48620, 4359, 4465, 4543, 48621, 4359, 4465, 4544,
    48622, 4359, 4465, 4545, 48623, 4359, 4465, 4546, 48624, 4359, 4466, 0, 48625, 4359, 4466, 4520, 48626, 4359, 4466, 4521, 48627, 4359, 4466, 4522,
    48628, 4359, 4466, 4523, 48629, 4359, 4466, 4524, 48630, 4359, 4466, 4525, 48631, 4359, 4466, 4526, 48632, 4359, 4466, 4527, 48633, 4359, 4466, 4528,
    48634, 4359, 4466, 4529, 48635, 4359, 4466, 4530, 48636, 4359, 4466, 4531, 48637, 4359, 4466, 4532, 48638, 4359, 4466, 4533, 48639, 4359, 4466, 4534,
    48640, 4359, 4466, 4535, 48641, 4359, 4466, 4536, 48642, 4359, 4466, 12612, 48643, 4359, 4466, 4538, 48644, 4359, 4466, 4539, 48645, 4359, 4466, 4540,
    48646, 4359, 4466, 4364, 48647, 4359, 4466, 4542, 48648, 4359, 4466, 4543, 48649, 4359, 4466, 4544, 48650, 4359, 4466, 4545, 48651, 4359, 4466, 4546,
    48652, 4359, 4467, 0, 48653, 4359, 4467, 4520, 48654, 4359, 4467, 4521, 48655, 4359, 4467, 4522, 48656, 4359, 4467, 4523, 48657, 4359, 4467, 4524,
    48658, 4359, 4467, 4525, 48659, 4359, 4467, 4526, 48660, 4359, 4467, 4527, 48661, 4359, 4467, 4528, 48662, 4359, 4467, 4529, 48663, 4359, 4467, 4530,
    48664, 4359, 4467, 4531, 48665, 4359, 4467, 4532, 48666, 4359, 4467, 4533, 48667, 4359, 4467, 4534, 48668, 4359, 4467, 4535, 48669, 4359, 4467, 4536,
    48670, 4359, 4467, 12612, 48671, 4359, 4467, 4538, 48672, 4359, 4467, 4539, 48673, 4359, 4467, 4540, 48674, 4359, 4467, 4364, 48675, 4359, 4467, 4542,
    48676, 4359, 4467, 4543, 48677, 4359, 4467, 4544, 48678, 4359, 4467, 4545, 48679, 4359, 4467, 4546, 48680, 4359, 4468, 0, 48681, 4359, 4468, 4520,
    48682, 4359, 4468, 4521, 48683, 4359, 4468, 4522, 48684, 4359, 4468, 4523, 48685, 4359, 4468, 4524, 48686, 4359, 4468, 4525, 48687, 4359, 4468, 4526,
    48688, 4359, 4468, 4527, 48689, 4359, 4468, 4528, 48690, 4359, 4468, 4529, 48691, 4359, 4468, 4530, 48692, 4359, 4468, 4531, 48693, 4359, 4468, 4532,
    48694, 4359, 4468, 4533, 48695, 4359, 4468, 4534, 48696, 4359, 4468, 4535, 48697, 4359, 4468, 4536, 48698, 4359, 4468, 12612, 48699, 4359, 4468, 4538,
    48700, 4359, 4468, 4539, 48701, 4359, 4468, 4540, 48702, 4359, 4468, 4364, 48703, 4359, 4468, 4542, 48704, 4359, 4468, 4543, 48705, 4359, 4468, 4544,
    48706, 4359, 4468, 4545, 48707, 4359, 4468, 4546, 48708, 4359, 4469, 0, 48709, 4359, 4469, 4520, 48710, 4359, 4469, 4521, 48711, 4359, 4469, 4522,
    48712, 4359, 4469, 4523, 48713, 4359, 4469, 4524, 48714, 4359, 4469, 4525, 48715, 4359, 4469, 4526, 48716, 4359, 4469, 4527, 48717, 4359, 4469, 4528,
    48718, 4359, 4469, 4529, 48719, 4359, 4469, 4530, 48720, 4359, 4469, 4531, 48721, 4359, 4469, 4532, 48722, 4359, 4469, 4533, 48723, 4359, 4469, 4534,
    48724, 4359, 4469, 4535, 48725, 4359, 4469, 4536, 48726, 4359, 4469, 12612, 48727, 4359, 4469, 4538, 48728, 4359, 4469, 4539, 48729, 4359, 4469, 4540,
    48730, 4359, 4469, 4364, 48731, 4359, 4469, 4542, 48732, 4359, 4469, 4543, 48733, 4359, 4469, 4544, 48734, 4359, 4469, 4545, 48735, 4359, 4469, 4546,
    48736, 4360, 4449, 0, 48737, 4360, 4449, 4520, 48738, 4360, 4449, 4521, 48739, 4360, 4449, 4522, 48740, 4360, 4449, 4523, 48741, 4360, 4449, 4524,
    48742, 4360, 4449, 4525, 48743, 4360, 4449, 4526, 48744, 4360, 4449, 4527, 48745, 4360, 4449, 4528, 48746, 4360, 4449, 4529, 48747, 4360, 4449, 4530,
    48748, 4360, 4449, 4531, 48749, 4360, 4449, 4532, 48750, 4360, 4449, 4533, 48751, 4360, 4449, 4534, 48752, 4360, 4449, 4535, 48753, 4360, 4449, 4536,
    48754, 4360, 4449, 12612, 48755, 4360, 4449, 4538, 48756, 4360, 4449, 4539, 48757, 4360, 4449, 4540, 48758, 4360, 4449, 4364, 48759, 4360, 4449, 4542,
    48760, 4360, 4449, 4543, 48761, 4360, 4449, 4544, 48762, 4360, 4449, 4545, 48763, 4360, 4449, 4546, 48764, 4360, 4450, 0, 48765, 4360, 4450, 4520,
    48766, 4360, 4450, 4521, 48767, 4360, 4450, 4522, 48768, 4360, 4450, 4523, 48769, 4360, 4450, 4524, 48770, 4360, 4450, 4525, 48771, 4360, 4450, 4526,
    48772, 4360, 4450, 4527, 48773, 4360, 4450, 4528, 48774, 4360, 4450, 4529, 48775, 4360, 4450, 4530, 48776, 4360, 4450, 4531, 48777, 4360, 4450, 4532,
    48778, 4360, 4450, 4533, 48779, 4360, 4450, 4534, 48780, 4360, 4450, 4535, 48781, 4360, 4450, 4536, 48782, 4360, 4450, 12612, 48783, 4360, 4450, 4538,
    48784, 4360, 4450, 4539, 48785, 4360, 4450, 4540, 48786, 4360, 4450, 4364, 48787, 4360, 4450, 4542, 48788, 4360, 4450, 4543, 48789, 4360, 4450, 4544,
    48790, 4360, 4450, 4545, 48791, 4360, 4450, 4546, 48792, 4360, 4451, 0, 48793, 4360, 4451, 4520, 48794, 4360, 4451, 4521, 48795, 4360, 4451, 4522,
    48796, 4360, 4451, 4523, 48797, 4360, 4451, 4524, 48798, 4360, 4451, 4525, 48799, 4360, 4451, 4526, 48800, 4360, 4451, 4527, 48801, 4360, 4451, 4528,
    48802, 4360, 4451, 4529, 48803, 4360, 4451, 4530, 48804, 4360, 4451, 4531, 48805, 4360, 4451, 4532, 48806, 4360, 4451, 4533, 48807, 4360, 4451, 4534,
    48808, 4360, 4451, 4535, 48809, 4360, 4451, 4536, 48810, 4360, 4451, 12612, 48811, 4360, 4451, 4538, 48812, 4360, 4451, 4539, 48813, 4360, 4451, 4540,
    48814, 4360, 4451, 4364, 48815, 4360, 4451, 4542, 48816, 4360, 4451, 4543, 48817, 4360, 4451, 4544, 48818, 4360, 4451, 4545, 48819, 4360, 4451, 4546,
    48820, 4360, 4452, 0, 48821, 4360, 4452, 4520, 48822, 4360, 4452, 4521, 48823, 4360, 4452, 4522, 48824, 4360, 4452, 4523, 48825, 4360, 4452, 4524,
    48826, 4360, 4452, 4525, 48827, 4360, 4452, 4526, 48828, 4360, 4452, 4527, 48829, 4360, 4452, 4528, 48830, 4360, 4452, 4529, 48831, 4360, 4452, 4530,
    48832, 4360, 4452, 4531, 48833, 4360, 4452, 4532, 48834, 4360, 4452, 4533, 48835, 4360, 4452, 4534, 48836, 4360, 4452, 4535, 48837, 4360, 4452, 4536,
    48838, 4360, 4452, 12612, 48839, 4360, 4452, 4538, 48840, 4360, 4452, 4539, 48841, 4360, 4452, 4540, 48842, 4360, 4452, 4364, 48843, 4360, 4452, 4542,
    48844, 4360, 4452, 4543, 48845, 4360, 4452, 4544, 48846, 4360, 4452, 4545, 48847, 4360, 4452, 4546, 48848, 4360, 4453, 0, 48849, 4360, 4453, 4520,
    48850, 4360, 4453, 4521, 48851, 4360, 4453, 4522, 48852, 4360, 4453, 4523, 48853, 4360, 4453, 4524, 48854, 4360, 4453, 4525, 48855, 4360, 4453, 4526,
    48856, 4360, 4453, 4527, 48857, 4360, 4453, 4528, 48858, 4360, 4453, 4529, 48859, 4360, 4453, 4530, 48860, 4360, 4453, 4531, 48861, 4360, 4453, 4532,
    48862, 4360, 4453, 4533, 48863, 4360, 4453, 4534, 48864, 4360, 4453, 4535, 48865, 4360, 4453, 4536, 48866, 4360, 4453, 12612, 48867, 4360, 4453, 4538,
    48868, 4360, 4453, 4539, 48869, 4360, 4453, 4540, 48870, 4360, 4453, 4364, 48871, 4360, 4453, 4542, 48872, 4360, 4453, 4543, 48873, 4360, 4453, 4544,
    48874, 4360, 4453, 4545, 48875, 4360, 4453, 4546, 48876, 4360, 4454, 0, 48877, 4360, 4454, 4520, 48878, 4360, 4454, 4521, 48879, 4360, 4454, 4522,
    48880, 4360, 4454, 4523, 48881, 4360, 4454, 4524, 48882, 4360, 4454, 4525, 48883, 4360, 4454, 4526, 48884, 4360, 4454, 4527, 48885, 4360, 4454, 4528,
    48886, 4360, 4454, 4529, 48887, 4360, 4454, 4530, 48888, 4360, 4454, 4531, 48889, 4360, 4454, 4532, 48890, 4360, 4454, 4533, 48891, 4360, 4454, 4534,
    48892, 4360, 4454, 4535, 48893, 4360, 4454, 4536, 48894, 4360, 4454, 12612, 48895, 4360, 4454, 4538, 48896, 4360, 4454, 4539, 48897, 4360, 4454, 4540,
    48898, 4360, 4454, 4364, 48899, 4360, 4454, 4542, 48900, 4360, 4454, 4543, 48901, 4360, 4454, 4544, 48902, 4360, 4454, 4545, 48903, 4360, 4454, 4546,
    48904, 4360, 4455, 0, 48905, 4360, 4455, 4520, 48906, 4360, 4455, 4521, 48907, 4360, 4455, 4522, 48908, 4360, 4455, 4523, 48909, 4360, 4455, 4524,
    48910, 4360, 4455, 4525, 48911, 4360, 4455, 4526, 48912, 4360, 4455, 4527, 48913, 4360, 4455, 4528, 48914, 4360, 4455, 4529, 48915, 4360, 4455, 4530,
    48916, 4360, 4455, 4531, 48917, 4360, 4455, 4532, 48918, 4360, 4455, 4533, 48919, 4360, 4455, 4534, 48920, 4360, 4455, 4535, 48921, 4360, 4455, 4536,
    48922, 4360, 4455, 12612, 48923, 4360, 4455, 4538, 48924, 4360, 4455, 4539, 48925, 4360, 4455, 4540, 48926, 4360, 4455, 4364, 48927, 4360, 4455, 4542,
    48928, 4360, 4455, 4543, 48929, 4360, 4455, 4544, 48930, 4360, 4455, 4545, 48931, 4360, 4455, 4546, 48932, 4360, 4456, 0, 48933, 4360, 4456, 4520,
    48934, 4360, 4456, 4521, 48935, 4360, 4456, 4522, 48936, 4360, 4456, 4523, 48937, 4360, 4456, 4524, 48938, 4360, 4456, 4525, 48939, 4360, 4456, 4526,
    48940, 4360, 4456, 4527, 48941, 4360, 4456, 4528, 48942, 4360, 4456, 4529, 48943, 4360, 4456, 4530, 48944, 4360, 4456, 4531, 48945, 4360, 4456, 4532,
    48946, 4360, 4456, 4533, 48947, 4360, 4456, 4534, 48948, 4360, 4456, 4535, 48949, 4360, 4456, 4536, 48950, 4360, 4456, 12612, 48951, 4360, 4456, 4538,
    48952, 4360, 4456, 4539, 48953, 4360, 4456, 4540, 48954, 4360, 4456, 4364, 48955, 4360, 4456, 4542, 48956, 4360, 4456, 4543, 48957, 4360, 4456, 4544,
    48958, 4360, 4456, 4545, 48959, 4360, 4456, 4546, 48960, 4360, 4457, 0, 48961, 4360, 4457, 4520, 48962, 4360, 4457, 4521, 48963, 4360, 4457, 4522,
    48964, 4360, 4457, 4523, 48965, 4360, 4457, 4524, 48966, 4360, 4457, 4525, 48967, 4360, 4457, 4526, 48968, 4360, 4457, 4527, 48969, 4360, 4457, 4528,
    48970, 4360, 4457, 4529, 48971, 4360, 4457, 4530, 48972, 4360, 4457, 4531, 48973, 4360, 4457, 4532, 48974, 4360, 4457, 4533, 48975, 4360, 4457, 4534,
    48976, 4360, 4457, 4535, 48977, 4360, 4457, 4536, 48978, 4360, 4457, 12612, 48979, 4360, 4457, 4538, 48980, 4360, 4457, 4539, 48981, 4360, 4457, 4540,
    48982, 4360, 4457, 4364, 48983, 4360, 4457, 4542, 48984, 4360, 4457, 4543, 48985, 4360, 4457, 4544, 48986, 4360, 4457, 4545, 48987, 4360, 4457, 4546,
    48988, 4360, 4458, 0, 48989, 4360, 4458, 4520, 48990, 4360, 4458, 4521, 48991, 4360, 4458, 4522, 48992, 4360, 4458, 4523, 48993, 4360, 4458, 4524,
    48994, 4360, 4458, 4525, 48995, 4360, 4458, 4526, 48996, 4360, 4458, 4527, 48997, 4360, 4458, 4528, 48998, 4360, 4458, 4529, 48999, 4360, 4458, 4530,
    49000, 4360, 4458, 4531, 49001, 4360, 4458, 4532, 49002, 4360, 4458, 4533, 49003, 4360, 4458, 4534, 49004, 4360, 4458, 4535, 49005, 4360, 4458, 4536,
    49006, 4360, 4458, 12612, 49007, 4360, 4458, 4538, 49008, 4360, 4458, 4539, 49009, 4360, 4458, 4540, 49010, 4360, 4458, 4364, 49011, 4360, 4458, 4542,
    49012, 4360, 4458, 4543, 49013, 4360, 4458, 4544, 49014, 4360, 4458, 4545, 49015, 4360, 4458, 4546, 49016, 4360, 4459, 0, 49017, 4360, 4459, 4520,
    49018, 4360, 4459, 4521, 49019, 4360, 4459, 4522, 49020, 4360, 4459, 4523, 49021, 4360, 4459, 4524, 49022, 4360, 4459, 4525, 49023, 4360, 4459, 4526,
    49024, 4360, 4459, 4527, 49025, 4360, 4459, 4528, 49026, 4360, 4459, 4529, 49027, 4360, 4459, 4530, 49028, 4360, 4459, 4531, 49029, 4360, 4459, 4532,
    49030, 4360, 4459, 4533, 49031, 4360, 4459, 4534, 49032, 4360, 4459, 4535, 49033, 4360, 4459, 4536, 49034, 4360, 4459, 12612, 49035, 4360, 4459, 4538,
    49036, 4360, 4459, 4539, 49037, 4360, 4459, 4540, 49038, 4360, 4459, 4364, 49039, 4360, 4459, 4542, 49040, 4360, 4459, 4543, 49041, 4360, 4459, 4544,
    49042, 4360, 4459, 4545, 49043, 4360, 4459, 4546, 49044, 4360, 4460, 0, 49045, 4360, 4460, 4520, 49046, 4360, 4460, 4521, 49047, 4360, 4460, 4522,
    49048, 4360, 4460, 4523, 49049, 4360, 4460, 4524, 49050, 4360, 4460, 4525, 49051, 4360, 4460, 4526, 49052, 4360, 4460, 4527, 49053, 4360, 4460, 4528,
    49054, 4360, 4460, 4529, 49055, 4360, 4460, 4530, 49056, 4360, 4460, 4531, 49057, 4360, 4460, 4532, 49058, 4360, 4460, 4533, 49059, 4360, 4460, 4534,
    49060, 4360, 4460, 4535, 49061, 4360, 4460, 4536, 49062, 4360, 4460, 12612, 49063, 4360, 4460, 4538, 49064, 4360, 4460, 4539, 49065, 4360, 4460, 4540,
    49066, 4360, 4460, 4364, 49067, 4360, 4460, 4542, 49068, 4360, 4460, 4543, 49069, 4360, 4460, 4544, 49070, 4360, 4460, 4545, 49071, 4360, 4460, 4546,
    49072, 4360, 4461, 0, 49073, 4360, 4461, 4520, 49074, 4360, 4461, 4521, 49075, 4360, 4461, 4522, 49076, 4360, 4461, 4523, 49077, 4360, 4461, 4524,
    49078, 4360, 4461, 4525, 49079, 4360, 4461, 4526, 49080, 4360, 4461, 4527, 49081, 4360, 4461, 4528, 49082, 4360, 4461, 4529, 49083, 4360, 4461, 4530,
    49084, 4360, 4461, 4531, 49085, 4360, 4461, 4532, 49086, 4360, 4461, 4533, 49087, 4360, 4461, 4534, 49088, 4360, 4461, 4535, 49089, 4360, 4461, 4536,
    49090, 4360, 4461, 12612, 49091, 4360, 4461, 4538, 49092, 4360, 4461, 4539, 49093, 4360, 4461, 4540, 49094, 4360, 4461, 4364, 49095, 4360, 4461, 4542,
    49096, 4360, 4461, 4543, 49097, 4360, 4461, 4544, 49098, 4360, 4461, 4545, 49099, 4360, 4461, 4546, 49100, 4360, 4462, 0, 49101, 4360, 4462, 4520,
    49102, 4360, 4462, 4521, 49103, 4360, 4462, 4522, 49104, 4360, 4462, 4523, 49105, 4360, 4462, 4524, 49106, 4360, 4462, 4525, 49107, 4360, 4462, 4526,
    49108, 4360, 4462, 4527, 49109, 4360, 4462, 4528, 49110, 4360, 4462, 4529, 49111, 4360, 4462, 4530, 49112, 4360, 4462, 4531, 49113, 4360, 4462, 4532,
    49114, 4360, 4462, 4533, 49115, 4360, 4462, 4534, 49116, 4360, 4462, 4535, 49117, 4360, 4462, 4536, 49118, 4360, 4462, 12612, 49119, 4360, 4462, 4538,
    49120, 4360, 4462, 4539, 49121, 4360, 4462, 4540, 49122, 4360, 4462, 4364, 49123, 4360, 4462, 4542, 49124, 4360, 4462, 4543, 49125, 4360, 4462, 4544,
    49126, 4360, 4462, 4545, 49127, 4360, 4462, 4546, 49128, 4360, 4463, 0, 49129, 4360, 4463, 4520, 49130, 4360, 4463, 4521, 49131, 4360, 4463, 4522,
    49132, 4360, 4463, 4523, 49133, 4360, 4463, 4524, 49134, 4360, 4463, 4525, 49135, 4360, 4463, 4526, 49136, 4360, 4463, 4527, 49137, 4360, 4463, 4528,
    49138, 4360, 4463, 4529, 49139, 4360, 4463, 4530, 49140, 4360, 4463, 4531, 49141, 4360, 4463, 4532, 49142, 4360, 4463, 4533, 49143, 4360, 4463, 4534,
    49144, 4360, 4463, 4535, 49145, 4360, 4463, 4536, 49146, 4360, 4463, 12612, 49147, 4360, 4463, 4538, 49148, 4360, 4463, 4539, 49149, 4360, 4463, 4540,
    49150, 4360, 4463, 4364, 49151, 4360, 4463, 4542, 49152, 4360, 4463, 4543, 49153, 4360, 4463, 4544, 49154, 4360, 4463, 4545, 49155, 4360, 4463, 4546,
    49156, 4360, 4464, 0, 49157, 4360, 4464, 4520, 49158, 4360, 4464, 4521, 49159, 4360, 4464, 4522, 49160, 4360, 4464, 4523, 49161, 4360, 4464, 4524,
    49162, 4360, 4464, 4525, 49163, 4360, 4464, 4526, 49164, 4360, 4464, 4527, 49165, 4360, 4464, 4528, 49166, 4360, 4464, 4529, 49167, 4360, 4464, 4530,
    49168, 4360, 4464, 4531, 49169, 4360, 4464, 4532, 49170, 4360, 4464, 4533, 49171, 4360, 4464, 4534, 49172, 4360, 4464, 4535, 49173, 4360, 4464, 4536,
    49174, 4360, 4464, 12612, 49175, 4360, 4464, 4538, 49176, 4360, 4464, 4539, 49177, 4360, 4464, 4540, 49178, 4360, 4464, 4364, 49179, 4360, 4464, 4542,
    49180, 4360, 4464, 4543, 49181, 4360, 4464, 4544, 49182, 4360, 4464, 4545, 49183, 4360, 4464, 4546, 49184, 4360, 4465, 0, 49185, 4360, 4465, 4520,
    49186, 4360, 4465, 4521, 49187, 4360, 4465, 4522, 49188, 4360, 4465, 4523, 49189, 4360, 4465, 4524, 49190, 4360, 4465, 4525, 49191, 4360, 4465, 4526,
    49192, 4360, 4465, 4527, 49193, 4360, 4465, 4528, 49194, 4360, 4465, 4529, 49195, 4360, 4465, 4530, 49196, 4360, 4465, 4531, 49197, 4360, 4465, 4532,
    49198, 4360, 4465, 4533, 49199, 4360, 4465, 4534, 49200, 4360, 4465, 4535, 49201, 4360, 4465, 4536, 49202, 4360, 4465, 12612, 49203, 4360, 4465, 4538,
    49204, 4360, 4465, 4539, 49205, 4360, 4465, 4540, 49206, 4360, 4465, 4364, 49207, 4360, 4465, 4542, 49208, 4360, 4465, 4543, 49209, 4360, 4465, 4544,
    49210, 4360, 4465, 4545, 49211, 4360, 4465, 4546, 49212, 4360, 4466, 0, 49213, 4360, 4466, 4520, 49214, 4360, 4466, 4521, 49215, 4360, 4466, 4522,
    49216, 4360, 4466, 4523, 49217, 4360, 4466, 4524, 49218, 4360, 4466, 4525, 49219, 4360, 4466, 4526, 49220, 4360, 4466, 4527, 49221, 4360, 4466, 4528,
    49222, 4360, 4466, 4529, 49223, 4360, 4466, 4530, 49224, 4360, 4466, 4531, 49225, 4360, 4466, 4532, 49226, 4360, 4466, 4533, 49227, 4360, 4466, 4534,
    49228, 4360, 4466, 4535, 49229, 4360, 4466, 4536, 49230, 4360, 4466, 12612, 49231, 4360, 4466, 4538, 49232, 4360, 4466, 4539, 49233, 4360, 4466, 4540,
    49234, 4360, 4466, 4364, 49235, 4360, 4466, 4542, 49236, 4360, 4466, 4543, 49237, 4360, 4466, 4544, 49238, 4360, 4466, 4545, 49239, 4360, 4466, 4546,
    49240, 4360, 4467, 0, 49241, 4360, 4467, 4520, 49242, 4360, 4467, 4521, 49243, 4360, 4467, 4522, 49244, 4360, 4467, 4523, 49245, 4360, 4467, 4524,
    49246, 4360, 4467, 4525, 49247, 4360, 4467, 4526, 49248, 4360, 4467, 4527, 49249, 4360, 4467, 4528, 49250, 4360, 4467, 4529, 49251, 4360, 4467, 4530,
    49252, 4360, 4467, 4531, 49253, 4360, 4467, 4532, 49254, 4360, 4467, 4533, 49255, 4360, 4467, 4534, 49256, 4360, 4467, 4535, 49257, 4360, 4467, 4536,
    49258, 4360, 4467, 12612, 49259, 4360, 4467, 4538, 49260, 4360, 4467, 4539, 49261, 4360, 4467, 4540, 49262, 4360, 4467, 4364, 49263, 4360, 4467, 4542,
    49264, 4360, 4467, 4543, 49265, 4360, 4467, 4544, 49266, 4360, 4467, 4545, 49267, 4360, 4467, 4546, 49268, 4360, 4468, 0, 49269, 4360, 4468, 4520,
    49270, 4360, 4468, 4521, 49271, 4360, 4468, 4522, 49272, 4360, 4468, 4523, 49273, 4360, 4468, 4524, 49274, 4360, 4468, 4525, 49275, 4360, 4468, 4526,
    49276, 4360, 4468, 4527, 49277, 4360, 4468, 4528, 49278, 4360, 4468, 4529, 49279, 4360, 4468, 4530, 49280, 4360, 4468, 4531, 49281, 4360, 4468, 4532,
    49282, 4360, 4468, 4533, 49283, 4360, 4468, 4534, 49284, 4360, 4468, 4535, 49285, 4360, 4468, 4536, 49286, 4360, 4468, 12612, 49287, 4360, 4468, 4538,
    49288, 4360, 4468, 4539, 49289, 4360, 4468, 4540, 49290, 4360, 4468, 4364, 49291, 4360, 4468, 4542, 49292, 4360, 4468, 4543, 49293, 4360, 4468, 4544,
    49294, 4360, 4468, 4545, 49295, 4360, 4468, 4546, 49296, 4360, 4469, 0, 49297, 4360, 4469, 4520, 49298, 4360, 4469, 4521, 49299, 4360, 4469, 4522,
    49300, 4360, 4469, 4523, 49301, 4360, 4469, 4524, 49302, 4360, 4469, 4525, 49303, 4360, 4469, 4526, 49304, 4360, 4469, 4527, 49305, 4360, 4469, 4528,
    49306, 4360, 4469, 4529, 49307, 4360, 4469, 4530, 49308, 4360, 4469, 4531, 49309, 4360, 4469, 4532, 49310, 4360, 4469, 4533, 49311, 4360, 4469, 4534,
    49312, 4360, 4469, 4535, 49313, 4360, 4469, 4536, 49314, 4360, 4469, 12612, 49315, 4360, 4469, 4538, 49316, 4360, 4469, 4539, 49317, 4360, 4469, 4540,
    49318, 4360, 4469, 4364, 49319, 4360, 4469, 4542, 49320, 4360, 4469, 4543, 49321, 4360, 4469, 4544, 49322, 4360, 4469, 4545, 49323, 4360, 4469, 4546,
    49324, 4361, 4449, 0, 49325, 4361, 4449, 4520, 49326, 4361, 4449, 4521, 49327, 4361, 4449, 4522, 49328, 4361, 4449, 4523, 49329, 4361, 4449, 4524,
    49330, 4361, 4449, 4525, 49331, 4361, 4449, 4526, 49332, 4361, 4449, 4527, 49333, 4361, 4449, 4528, 49334, 4361, 4449, 4529, 49335, 4361, 4449, 4530,
    49336, 4361, 4449, 4531, 49337, 4361, 4449, 4532, 49338, 4361, 4449, 4533, 49339, 4361, 4449, 4534, 49340, 4361, 4449, 4535, 49341, 4361, 4449, 4536,
    49342, 4361, 4449, 12612, 49343, 4361, 4449, 4538, 49344, 4361, 4449, 4539, 49345, 4361, 4449, 4540, 49346, 4361, 4449, 4364, 49347, 4361, 4449, 4542,
    49348, 4361, 4449, 4543, 49349, 4361, 4449, 4544, 49350, 4361, 4449, 4545, 49351, 4361, 4449, 4546, 49352, 4361, 4450, 0, 49353, 4361, 4450, 4520,
    49354, 4361, 4450, 4521, 49355, 4361, 4450, 4522, 49356, 4361, 4450, 4523, 49357, 4361, 4450, 4524, 49358, 4361, 4450, 4525, 49359, 4361, 4450, 4526,
    49360, 4361, 4450, 4527, 49361, 4361, 4450, 4528, 49362, 4361, 4450, 4529, 49363, 4361, 4450, 4530, 49364, 4361, 4450, 4531, 49365, 4361, 4450, 4532,
    49366, 4361, 4450, 4533, 49367, 4361, 4450, 4534, 49368, 4361, 4450, 4535, 49369, 4361, 4450, 4536, 49370, 4361, 4450, 12612, 49371, 4361, 4450, 4538,
    49372, 4361, 4450, 4539, 49373, 4361, 4450, 4540, 49374, 4361, 4450, 4364, 49375, 4361, 4450, 4542, 49376, 4361, 4450, 4543, 49377, 4361, 4450, 4544,
    49378, 4361, 4450, 4545, 49379, 4361, 4450, 4546, 49380, 4361, 4451, 0, 49381, 4361, 4451, 4520, 49382, 4361, 4451, 4521, 49383, 4361, 4451, 4522,
    49384, 4361, 4451, 4523, 49385, 4361, 4451, 4524, 49386, 4361, 4451, 4525, 49387, 4361, 4451, 4526, 49388, 4361, 4451, 4527, 49389, 4361, 4451, 4528,
    49390, 4361, 4451, 4529, 49391, 4361, 4451, 4530, 49392, 4361, 4451, 4531, 49393, 4361, 4451, 4532, 49394, 4361, 4451, 4533, 49395, 4361, 4451, 4534,
    49396, 4361, 4451, 4535, 49397, 4361, 4451, 4536, 49398, 4361, 4451, 12612, 49399, 4361, 4451, 4538, 49400, 4361, 4451, 4539, 49401, 4361, 4451, 4540,
    49402, 4361, 4451, 4364, 49403, 4361, 4451, 4542, 49404, 4361, 4451, 4543, 49405, 4361, 4451, 4544, 49406, 4361, 4451, 4545, 49407, 4361, 4451, 4546,
    49408, 4361, 4452, 0, 49409, 4361, 4452, 4520, 49410, 4361, 4452, 4521, 49411, 4361, 4452, 4522, 49412, 4361, 4452, 4523, 49413, 4361, 4452, 4524,
    49414, 4361, 4452, 4525, 49415, 4361, 4452, 4526, 49416, 4361, 4452, 4527, 49417, 4361, 4452, 4528, 49418, 4361, 4452, 4529, 49419, 4361, 4452, 4530,
    49420, 4361, 4452, 4531, 49421, 4361, 4452, 4532, 49422, 4361, 4452, 4533, 49423, 4361, 4452, 4534, 49424, 4361, 4452, 4535, 49425, 4361, 4452, 4536,
    49426, 4361, 4452, 12612, 49427, 4361, 4452, 4538, 49428, 4361, 4452, 4539, 49429, 4361, 4452, 4540, 49430, 4361, 4452, 4364, 49431, 4361, 4452, 4542,
    49432, 4361, 4452, 4543, 49433, 4361, 4452, 4544, 49434, 4361, 4452, 4545, 49435, 4361, 4452, 4546, 49436, 4361, 4453, 0, 49437, 4361, 4453, 4520,
    49438, 4361, 4453, 4521, 49439, 4361, 4453, 4522, 49440, 4361, 4453, 4523, 49441, 4361, 4453, 4524, 49442, 4361, 4453, 4525, 49443, 4361, 4453, 4526,
    49444, 4361, 4453, 4527, 49445, 4361, 4453, 4528, 49446, 4361, 4453, 4529, 49447, 4361, 4453, 4530, 49448, 4361, 4453, 4531, 49449, 4361, 4453, 4532,
    49450, 4361, 4453, 4533, 49451, 4361, 4453, 4534, 49452, 4361, 4453, 4535, 49453, 4361, 4453, 4536, 49454, 4361, 4453, 12612, 49455, 4361, 4453, 4538,
    49456, 4361, 4453, 4539, 49457, 4361, 4453, 4540, 49458, 4361, 4453, 4364, 49459, 4361, 4453, 4542, 49460, 4361, 4453, 4543, 49461, 4361, 4453, 4544,
    49462, 4361, 4453, 4545, 49463, 4361, 4453, 4546, 49464, 4361, 4454, 0, 49465, 4361, 4454, 4520, 49466, 4361, 4454, 4521, 49467, 4361, 4454, 4522,
    49468, 4361, 4454, 4523, 49469, 4361, 4454, 4524, 49470, 4361, 4454, 4525, 49471, 4361, 4454, 4526, 49472, 4361, 4454, 4527, 49473, 4361, 4454, 4528,
    49474, 4361, 4454, 4529, 49475, 4361, 4454, 4530, 49476, 4361, 4454, 4531, 49477, 4361, 4454, 4532, 49478, 4361, 4454, 4533, 49479, 4361, 4454, 4534,
    49480, 4361, 4454, 4535, 49481, 4361, 4454, 4536, 49482, 4361, 4454, 12612, 49483, 4361, 4454, 4538, 49484, 4361, 4454, 4539, 49485, 4361, 4454, 4540,
    49486, 4361, 4454, 4364, 49487, 4361, 4454, 4542, 49488, 4361, 4454, 4543, 49489, 4361, 4454, 4544, 49490, 4361, 4454, 4545, 49491, 4361, 4454, 4546,
    49492, 4361, 4455, 0, 49493, 4361, 4455, 4520, 49494, 4361, 4455, 4521, 49495, 4361, 4455, 4522, 49496, 4361, 4455, 4523, 49497, 4361, 4455, 4524,
    49498, 4361, 4455, 4525, 49499, 4361, 4455, 4526, 49500, 4361, 4455, 4527, 49501, 4361, 4455, 4528, 49502, 4361, 4455, 4529, 49503, 4361, 4455, 4530,
    49504, 4361, 4455, 4531, 49505, 4361, 4455, 4532, 49506, 4361, 4455, 4533, 49507, 4361, 4455, 4534, 49508, 4361, 4455, 4535, 49509, 4361, 4455, 4536,
    49510, 4361, 4455, 12612, 49511, 4361, 4455, 4538, 49512, 4361, 4455, 4539, 49513, 4361, 4455, 4540, 49514, 4361, 4455, 4364, 49515, 4361, 4455, 4542,
    49516, 4361, 4455, 4543, 49517, 4361, 4455, 4544, 49518, 4361, 4455, 4545, 49519, 4361, 4455, 4546, 49520, 4361, 4456, 0, 49521, 4361, 4456, 4520,
    49522, 4361, 4456, 4521, 49523, 4361, 4456, 4522, 49524, 4361, 4456, 4523, 49525, 4361, 4456, 4524, 49526, 4361, 4456, 4525, 49527, 4361, 4456, 4526,
    49528, 4361, 4456, 4527, 49529, 4361, 4456, 4528, 49530, 4361, 4456, 4529, 49531, 4361, 4456, 4530, 49532, 4361, 4456, 4531, 49533, 4361, 4456, 4532,
    49534, 4361, 4456, 4533, 49535, 4361, 4456, 4534, 49536, 4361, 4456, 4535, 49537, 4361, 4456, 4536, 49538, 4361, 4456, 12612, 49539, 4361, 4456, 4538,
    49540, 4361, 4456, 4539, 49541, 4361, 4456, 4540, 49542, 4361, 4456, 4364, 49543, 4361, 4456, 4542, 49544, 4361, 4456, 4543, 49545, 4361, 4456, 4544,
    49546, 4361, 4456, 4545, 49547, 4361, 4456, 4546, 49548, 4361, 4457, 0, 49549, 4361, 4457, 4520, 49550, 4361, 4457, 4521, 49551, 4361, 4457, 4522,
    49552, 4361, 4457, 4523, 49553, 4361, 4457, 4524, 49554, 4361, 4457, 4525, 49555, 4361, 4457, 4526, 49556, 4361, 4457, 4527, 49557, 4361, 4457, 4528,
    49558, 4361, 4457, 4529, 49559, 4361, 4457, 4530, 49560, 4361, 4457, 4531, 49561, 4361, 4457, 4532, 49562, 4361, 4457, 4533, 49563, 4361, 4457, 4534,
    49564, 4361, 4457, 4535, 49565, 4361, 4457, 4536, 49566, 4361, 4457, 12612, 49567, 4361, 4457, 4538, 49568, 4361, 4457, 4539, 49569, 4361, 4457, 4540,
    49570, 4361, 4457, 4364, 49571, 4361, 4457, 4542, 49572, 4361, 4457, 4543, 49573, 4361, 4457, 4544, 49574, 4361, 4457, 4545, 49575, 4361, 4457, 4546,
    49576, 4361, 4458, 0, 49577, 4361, 4458, 4520, 49578, 4361, 4458, 4521, 49579, 4361, 4458, 4522, 49580, 4361, 4458, 4523, 49581, 4361, 4458, 4524,
    49582, 4361, 4458, 4525, 49583, 4361, 4458, 4526, 49584, 4361, 4458, 4527, 49585, 4361, 4458, 4528, 49586, 4361, 4458, 4529, 49587, 4361, 4458, 4530,
    49588, 4361, 4458, 4531, 49589, 4361, 4458, 4532, 49590, 4361, 4458, 4533, 49591, 4361, 4458, 4534, 49592, 4361, 4458, 4535, 49593, 4361, 4458, 4536,
    49594, 4361, 4458, 12612, 49595, 4361, 4458, 4538, 49596, 4361, 4458, 4539, 49597, 4361, 4458, 4540, 49598, 4361, 4458, 4364, 49599, 4361, 4458, 4542,
    49600, 4361, 4458, 4543, 49601, 4361, 4458, 4544, 49602, 4361, 4458, 4545, 49603, 4361, 4458, 4546, 49604, 4361, 4459, 0, 49605, 4361, 4459, 4520,
    49606, 4361, 4459, 4521, 49607, 4361, 4459, 4522, 49608, 4361, 4459, 4523, 49609, 4361, 4459, 4524, 49610, 4361, 4459, 4525, 49611, 4361, 4459, 4526,
    49612, 4361, 4459, 4527, 49613, 4361, 4459, 4528, 49614, 4361, 4459, 4529, 49615, 4361, 4459, 4530, 49616, 4361, 4459, 4531, 49617, 4361, 4459, 4532,
    49618, 4361, 4459, 4533, 49619, 4361, 4459, 4534, 49620, 4361, 4459, 4535, 49621, 4361, 4459, 4536, 49622, 4361, 4459, 12612, 49623, 4361, 4459, 4538,
    49624, 4361, 4459, 4539, 49625, 4361, 4459, 4540, 49626, 4361, 4459, 4364, 49627, 4361, 4459, 4542, 49628, 4361, 4459, 4543, 49629, 4361, 4459, 4544,
    49630, 4361, 4459, 4545, 49631, 4361, 4459, 4546, 49632, 4361, 4460, 0, 49633, 4361, 4460, 4520, 49634, 4361, 4460, 4521, 49635, 4361, 4460, 4522,
    49636, 4361, 4460, 4523, 49637, 4361, 4460, 4524, 49638, 4361, 4460, 4525, 49639, 4361, 4460, 4526, 49640, 4361, 4460, 4527, 49641, 4361, 4460, 4528,
    49642, 4361, 4460, 4529, 49643, 4361, 4460, 4530, 49644, 4361, 4460, 4531, 49645, 4361, 4460, 4532, 49646, 4361, 4460, 4533, 49647, 4361, 4460, 4534,
    49648, 4361, 4460, 4535, 49649, 4361, 4460, 4536, 49650, 4361, 4460, 12612, 49651, 4361, 4460, 4538, 49652, 4361, 4460, 4539, 49653, 4361, 4460, 4540,
    49654, 4361, 4460, 4364, 49655, 4361, 4460, 4542, 49656, 4361, 4460, 4543, 49657, 4361, 4460, 4544, 49658, 4361, 4460, 4545, 49659, 4361, 4460, 4546,
    49660, 4361, 4461, 0, 49661, 4361, 4461, 4520, 49662, 4361, 4461, 4521, 49663, 4361, 4461, 4522, 49664, 4361, 4461, 4523, 49665, 4361, 4461, 4524,
    49666, 4361, 4461, 4525, 49667, 4361, 4461, 4526, 49668, 4361, 4461, 4527, 49669, 4361, 4461, 4528, 49670, 4361, 4461, 4529, 49671, 4361, 4461, 4530,
    49672, 4361, 4461, 4531, 49673, 4361, 4461, 4532, 49674, 4361, 4461, 4533, 49675, 4361, 4461, 4534, 49676, 4361, 4461, 4535, 49677, 4361, 4461, 4536,
    49678, 4361, 4461, 12612, 49679, 4361, 4461, 4538, 49680, 4361, 4461, 4539, 49681, 4361, 4461, 4540, 49682, 4361, 4461, 4364, 49683, 4361, 4461, 4542,
    49684, 4361, 4461, 4543, 49685, 4361, 4461, 4544, 49686, 4361, 4461, 4545, 49687, 4361, 4461, 4546, 49688, 4361, 4462, 0, 49689, 4361, 4462, 4520,
    49690, 4361, 4462, 4521, 49691, 4361, 4462, 4522, 49692, 4361, 4462, 4523, 49693, 4361, 4462, 4524, 49694, 4361, 4462, 4525, 49695, 4361, 4462, 4526,
    49696, 4361, 4462, 4527, 49697, 4361, 4462, 4528, 49698, 4361, 4462, 4529, 49699, 4361, 4462, 4530, 49700, 4361, 4462, 4531, 49701, 4361, 4462, 4532,
    49702, 4361, 4462, 4533, 49703, 4361, 4462, 4534, 49704, 4361, 4462, 4535, 49705, 4361, 4462, 4536, 49706, 4361, 4462, 12612, 49707, 4361, 4462, 4538,
    49708, 4361, 4462, 4539, 49709, 4361, 4462, 4540, 49710, 4361, 4462, 4364, 49711, 4361, 4462, 4542, 49712, 4361, 4462, 4543, 49713, 4361, 4462, 4544,
    49714, 4361, 4462, 4545, 49715, 4361, 4462, 4546, 49716, 4361, 4463, 0, 49717, 4361, 4463, 4520, 49718, 4361, 4463, 4521, 49719, 4361, 4463, 4522,
    49720, 4361, 4463, 4523, 49721, 4361, 4463, 4524, 49722, 4361, 4463, 4525, 49723, 4361, 4463, 4526, 49724, 4361, 4463, 4527, 49725, 4361, 4463, 4528,
    49726, 4361, 4463, 4529, 49727, 4361, 4463, 4530, 49728, 4361, 4463, 4531, 49729, 4361, 4463, 4532, 49730, 4361, 4463, 4533, 49731, 4361, 4463, 4534,
    49732, 4361, 4463, 4535, 49733, 4361, 4463, 4536, 49734, 4361, 4463, 12612, 49735, 4361, 4463, 4538, 49736, 4361, 4463, 4539, 49737, 4361, 4463, 4540,
    49738, 4361, 4463, 4364, 49739, 4361, 4463, 4542, 49740, 4361, 4463, 4543, 49741, 4361, 4463, 4544, 49742, 4361, 4463, 4545, 49743, 4361, 4463, 4546,
    49744, 4361, 4464, 0, 49745, 4361, 4464, 4520, 49746, 4361, 4464, 4521, 49747, 4361, 4464, 4522, 49748, 4361, 4464, 4523, 49749, 4361, 4464, 4524,
    49750, 4361, 4464, 4525, 49751, 4361, 4464, 4526, 49752, 4361, 4464, 4527, 49753, 4361, 4464, 4528, 49754, 4361, 4464, 4529, 49755, 4361, 4464, 4530,
    49756, 4361, 4464, 4531, 49757, 4361, 4464, 4532, 49758, 4361, 4464, 4533, 49759, 4361, 4464, 4534, 49760, 4361, 4464, 4535, 49761, 4361, 4464, 4536,
    49762, 4361, 4464, 12612, 49763, 4361, 4464, 4538, 49764, 4361, 4464, 4539, 49765, 4361, 4464, 4540, 49766, 4361, 4464, 4364, 49767, 4361, 4464, 4542,
    49768, 4361, 4464, 4543, 49769, 4361, 4464, 4544, 49770, 4361, 4464, 4545, 49771, 4361, 4464, 4546, 49772, 4361, 4465, 0, 49773, 4361, 4465, 4520,
    49774, 4361, 4465, 4521, 49775, 4361, 4465, 4522, 49776, 4361, 4465, 4523, 49777, 4361, 4465, 4524, 49778, 4361, 4465, 4525, 49779, 4361, 4465, 4526,
    49780, 4361, 4465, 4527, 49781, 4361, 4465, 4528, 49782, 4361, 4465, 4529, 49783, 4361, 4465, 4530, 49784, 4361, 4465, 4531, 49785, 4361, 4465, 4532,
    49786, 4361, 4465, 4533, 49787, 4361, 4465, 4534, 49788, 4361, 4465, 4535, 49789, 4361, 4465, 4536, 49790, 4361, 4465, 12612, 49791, 4361, 4465, 4538,
    49792, 4361, 4465, 4539, 49793, 4361, 4465, 4540, 49794, 4361, 4465, 4364, 49795, 4361, 4465, 4542, 49796, 4361, 4465, 4543, 49797, 4361, 4465, 4544,
    49798, 4361, 4465, 4545, 49799, 4361, 4465, 4546, 49800, 4361, 4466, 0, 49801, 4361, 4466, 4520, 49802, 4361, 4466, 4521, 49803, 4361, 4466, 4522,
    49804, 4361, 4466, 4523, 49805, 4361, 4466, 4524, 49806, 4361, 4466, 4525, 49807, 4361, 4466, 4526, 49808, 4361, 4466, 4527, 49809, 4361, 4466, 4528,
    49810, 4361, 4466, 4529, 49811, 4361, 4466, 4530, 49812, 4361, 4466, 4531, 49813, 4361, 4466, 4532, 49814, 4361, 4466, 4533, 49815, 4361, 4466, 4534,
    49816, 4361, 4466, 4535, 49817, 4361, 4466, 4536, 49818, 4361, 4466, 12612, 49819, 4361, 4466, 4538, 49820, 4361, 4466, 4539, 49821, 4361, 4466, 4540,
    49822, 4361, 4466, 4364, 49823, 4361, 4466, 4542, 49824, 4361, 4466, 4543, 49825, 4361, 4466, 4544, 49826, 4361, 4466, 4545, 49827, 4361, 4466, 4546,
    49828, 4361, 4467, 0, 49829, 4361, 4467, 4520, 49830, 4361, 4467, 4521, 49831, 4361, 4467, 4522, 49832, 4361, 4467, 4523, 49833, 4361, 4467, 4524,
    49834, 4361, 4467, 4525, 49835, 4361, 4467, 4526, 49836, 4361, 4467, 4527, 49837, 4361, 4467, 4528, 49838, 4361, 4467, 4529, 49839, 4361, 4467, 4530,
    49840, 4361, 4467, 4531, 49841, 4361, 4467, 4532, 49842, 4361, 4467, 4533, 49843, 4361, 4467, 4534, 49844, 4361, 4467, 4535, 49845, 4361, 4467, 4536,
    49846, 4361, 4467, 12612, 49847, 4361, 4467, 4538, 49848, 4361, 4467, 4539, 49849, 4361, 4467, 4540, 49850, 4361, 4467, 4364, 49851, 4361, 4467, 4542,
    49852, 4361, 4467, 4543, 49853, 4361, 4467, 4544, 49854, 4361, 4467, 4545, 49855, 4361, 4467, 4546, 49856, 4361, 4468, 0, 49857, 4361, 4468, 4520,
    49858, 4361, 4468, 4521, 49859, 4361, 4468, 4522, 49860, 4361, 4468, 4523, 49861, 4361, 4468, 4524, 49862, 4361, 4468, 4525, 49863, 4361, 4468, 4526,
    49864, 4361, 4468, 4527, 49865, 4361, 4468, 4528, 49866, 4361, 4468, 4529, 49867, 4361, 4468, 4530, 49868, 4361, 4468, 4531, 49869, 4361, 4468, 4532,
    49870, 4361, 4468, 4533, 49871, 4361, 4468, 4534, 49872, 4361, 4468, 4535, 49873, 4361, 4468, 4536, 49874, 4361, 4468, 12612, 49875, 4361, 4468, 4538,
    49876, 4361, 4468, 4539, 49877, 4361, 4468, 4540, 49878, 4361, 4468, 4364, 49879, 4361, 4468, 4542, 49880, 4361, 4468, 4543, 49881, 4361, 4468, 4544,
    49882, 4361, 4468, 4545, 49883, 4361, 4468, 4546, 49884, 4361, 4469, 0, 49885, 4361, 4469, 4520, 49886, 4361, 4469, 4521, 49887, 4361, 4469, 4522,
    49888, 4361, 4469, 4523, 49889, 4361, 4469, 4524, 49890, 4361, 4469, 4525, 49891, 4361, 4469, 4526, 49892, 4361, 4469, 4527, 49893, 4361, 4469, 4528,
    49894, 4361, 4469, 4529, 49895, 4361, 4469, 4530, 49896, 4361, 4469, 4531, 49897, 4361, 4469, 4532, 49898, 4361, 4469, 4533, 49899, 4361, 4469, 4534,
    49900, 4361, 4469, 4535, 49901, 4361, 4469, 4536, 49902, 4361, 4469, 12612, 49903, 4361, 4469, 4538, 49904, 4361, 4469, 4539, 49905, 4361, 4469, 4540,
    49906, 4361, 4469, 4364, 49907, 4361, 4469, 4542, 49908, 4361, 4469, 4543, 49909, 4361, 4469, 4544, 49910, 4361, 4469, 4545, 49911, 4361, 4469, 4546,
    49912, 4362, 4449, 0, 49913, 4362, 4449, 4520, 49914, 4362, 4449, 4521, 49915, 4362, 4449, 4522, 49916, 4362, 4449, 4523, 49917, 4362, 4449, 4524,
    49918, 4362, 4449, 4525, 49919, 4362, 4449, 4526, 49920, 4362, 4449, 4527, 49921, 4362, 4449, 4528, 49922, 4362, 4449, 4529, 49923, 4362, 4449, 4530,
    49924, 4362, 4449, 4531, 49925, 4362, 4449, 4532, 49926, 4362, 4449, 4533, 49927, 4362, 4449, 4534, 49928, 4362, 4449, 4535, 49929, 4362, 4449, 4536,
    49930, 4362, 4449, 12612, 49931, 4362, 4449, 4538, 49932, 4362, 4449, 4539, 49933, 4362, 4449, 4540, 49934, 4362, 4449, 4364, 49935, 4362, 4449, 4542,
    49936, 4362, 4449, 4543, 49937, 4362, 4449, 4544, 49938, 4362, 4449, 4545, 49939, 4362, 4449, 4546, 49940, 4362, 4450, 0, 49941, 4362, 4450, 4520,
    49942, 4362, 4450, 4521, 49943, 4362, 4450, 4522, 49944, 4362, 4450, 4523, 49945, 4362, 4450, 4524, 49946, 4362, 4450, 4525, 49947, 4362, 4450, 4526,
    49948, 4362, 4450, 4527, 49949, 4362, 4450, 4528, 49950, 4362, 4450, 4529, 49951, 4362, 4450, 4530, 49952, 4362, 4450, 4531, 49953, 4362, 4450, 4532,
    49954, 4362, 4450, 4533, 49955, 4362, 4450, 4534, 49956, 4362, 4450, 4535, 49957, 4362, 4450, 4536, 49958, 4362, 4450, 12612, 49959, 4362, 4450, 4538,
    49960, 4362, 4450, 4539, 49961, 4362, 4450, 4540, 49962, 4362, 4450, 4364, 49963, 4362, 4450, 4542, 49964, 4362, 4450, 4543, 49965, 4362, 4450, 4544,
    49966, 4362, 4450, 4545, 49967, 4362, 4450, 4546, 49968, 4362, 4451, 0, 49969, 4362, 4451, 4520, 49970, 4362, 4451, 4521, 49971, 4362, 4451, 4522,
    49972, 4362, 4451, 4523, 49973, 4362, 4451, 4524, 49974, 4362, 4451, 4525, 49975, 4362, 4451, 4526, 49976, 4362, 4451, 4527, 49977, 4362, 4451, 4528,
    49978, 4362, 4451, 4529, 49979, 4362, 4451, 4530, 49980, 4362, 4451, 4531, 49981, 4362, 4451, 4532, 49982, 4362, 4451, 4533, 49983, 4362, 4451, 4534,
    49984, 4362, 4451, 4535, 49985, 4362, 4451, 4536, 49986, 4362, 4451, 12612, 49987, 4362, 4451, 4538, 49988, 4362, 4451, 4539, 49989, 4362, 4451, 4540,
    49990, 4362, 4451, 4364, 49991, 4362, 4451, 4542, 49992, 4362, 4451, 4543, 49993, 4362, 4451, 4544, 49994, 4362, 4451, 4545, 49995, 4362, 4451, 4546,
    49996, 4362, 4452, 0, 49997, 4362, 4452, 4520, 49998, 4362, 4452, 4521, 49999, 4362, 4452, 4522, 50000, 4362, 4452, 4523, 50001, 4362, 4452, 4524,
    50002, 4362, 4452, 4525, 50003, 4362, 4452, 4526, 50004, 4362, 4452, 4527, 50005, 4362, 4452, 4528, 50006, 4362, 4452, 4529, 50007, 4362, 4452, 4530,
    50008, 4362, 4452, 4531, 50009, 4362, 4452, 4532, 50010, 4362, 4452, 4533, 50011, 4362, 4452, 4534, 50012, 4362, 4452, 4535, 50013, 4362, 4452, 4536,
    50014, 4362, 4452, 12612, 50015, 4362, 4452, 4538, 50016, 4362, 4452, 4539, 50017, 4362, 4452, 4540, 50018, 4362, 4452, 4364, 50019, 4362, 4452, 4542,
    50020, 4362, 4452, 4543, 50021, 4362, 4452, 4544, 50022, 4362, 4452, 4545, 50023, 4362, 4452, 4546, 50024, 4362, 4453, 0, 50025, 4362, 4453, 4520,
    50026, 4362, 4453, 4521, 50027, 4362, 4453, 4522, 50028, 4362, 4453, 4523, 50029, 4362, 4453, 4524, 50030, 4362, 4453, 4525, 50031, 4362, 4453, 4526,
    50032, 4362, 4453, 4527, 50033, 4362, 4453, 4528, 50034, 4362, 4453, 4529, 50035, 4362, 4453, 4530, 50036, 4362, 4453, 4531, 50037, 4362, 4453, 4532,
    50038, 4362, 4453, 4533, 50039, 4362, 4453, 4534, 50040, 4362, 4453, 4535, 50041, 4362, 4453, 4536, 50042, 4362, 4453, 12612, 50043, 4362, 4453, 4538,
    50044, 4362, 4453, 4539, 50045, 4362, 4453, 4540, 50046, 4362, 4453, 4364, 50047, 4362, 4453, 4542, 50048, 4362, 4453, 4543, 50049, 4362, 4453, 4544,
    50050, 4362, 4453, 4545, 50051, 4362, 4453, 4546, 50052, 4362, 4454, 0, 50053, 4362, 4454, 4520, 50054, 4362, 4454, 4521, 50055, 4362, 4454, 4522,
    50056, 4362, 4454, 4523, 50057, 4362, 4454, 4524, 50058, 4362, 4454, 4525, 50059, 4362, 4454, 4526, 50060, 4362, 4454, 4527, 50061, 4362, 4454, 4528,
    50062, 4362, 4454, 4529, 50063, 4362, 4454, 4530, 50064, 4362, 4454, 4531, 50065, 4362, 4454, 4532, 50066, 4362, 4454, 4533, 50067, 4362, 4454, 4534,
    50068, 4362, 4454, 4535, 50069, 4362, 4454, 4536, 50070, 4362, 4454, 12612, 50071, 4362, 4454, 4538, 50072, 4362, 4454, 4539, 50073, 4362, 4454, 4540,
    50074, 4362, 4454, 4364, 50075, 4362, 4454, 4542, 50076, 4362, 4454, 4543, 50077, 4362, 4454, 4544, 50078, 4362, 4454, 4545, 50079, 4362, 4454, 4546,
    50080, 4362, 4455, 0, 50081, 4362, 4455, 4520, 50082, 4362, 4455, 4521, 50083, 4362, 4455, 4522, 50084, 4362, 4455, 4523, 50085, 4362, 4455, 4524,
    50086, 4362, 4455, 4525, 50087, 4362, 4455, 4526, 50088, 4362, 4455, 4527, 50089, 4362, 4455, 4528, 50090, 4362, 4455, 4529, 50091, 4362, 4455, 4530,
    50092, 4362, 4455, 4531, 50093, 4362, 4455, 4532, 50094, 4362, 4455, 4533, 50095, 4362, 4455, 4534, 50096, 4362, 4455, 4535, 50097, 4362, 4455, 4536,
    50098, 4362, 4455, 12612, 50099, 4362, 4455, 4538, 50100, 4362, 4455, 4539, 50101, 4362, 4455, 4540, 50102, 4362, 4455, 4364, 50103, 4362, 4455, 4542,
    50104, 4362, 4455, 4543, 50105, 4362, 4455, 4544, 50106, 4362, 4455, 4545, 50107, 4362, 4455, 4546, 50108, 4362, 4456, 0, 50109, 4362, 4456, 4520,
    50110, 4362, 4456, 4521, 50111, 4362, 4456, 4522, 50112, 4362, 4456, 4523, 50113, 4362, 4456, 4524, 50114, 4362, 4456, 4525, 50115, 4362, 4456, 4526,
    50116, 4362, 4456, 4527, 50117, 4362, 4456, 4528, 50118, 4362, 4456, 4529, 50119, 4362, 4456, 4530, 50120, 4362, 4456, 4531, 50121, 4362, 4456, 4532,
    50122, 4362, 4456, 4533, 50123, 4362, 4456, 4534, 50124, 4362, 4456, 4535, 50125, 4362, 4456, 4536, 50126, 4362, 4456, 12612, 50127, 4362, 4456, 4538,
    50128, 4362, 4456, 4539, 50129, 4362, 4456, 4540, 50130, 4362, 4456, 4364, 50131, 4362, 4456, 4542, 50132, 4362, 4456, 4543, 50133, 4362, 4456, 4544,
    50134, 4362, 4456, 4545, 50135, 4362, 4456, 4546, 50136, 4362, 4457, 0, 50137, 4362, 4457, 4520, 50138, 4362, 4457, 4521, 50139, 4362, 4457, 4522,
    50140, 4362, 4457, 4523, 50141, 4362, 4457, 4524, 50142, 4362, 4457, 4525, 50143, 4362, 4457, 4526, 50144, 4362, 4457, 4527, 50145, 4362, 4457, 4528,
    50146, 4362, 4457, 4529, 50147, 4362, 4457, 4530, 50148, 4362, 4457, 4531, 50149, 4362, 4457, 4532, 50150, 4362, 4457, 4533, 50151, 4362, 4457, 4534,
    50152, 4362, 4457, 4535, 50153, 4362, 4457, 4536, 50154, 4362, 4457, 12612, 50155, 4362, 4457, 4538, 50156, 4362, 4457, 4539, 50157, 4362, 4457, 4540,
    50158, 4362, 4457, 4364, 50159, 4362, 4457, 4542, 50160, 4362, 4457, 4543, 50161, 4362, 4457, 4544, 50162, 4362, 4457, 4545, 50163, 4362, 4457, 4546,
    50164, 4362, 4458, 0, 50165, 4362, 4458, 4520, 50166, 4362, 4458, 4521, 50167, 4362, 4458, 4522, 50168, 4362, 4458, 4523, 50169, 4362, 4458, 4524,
    50170, 4362, 4458, 4525, 50171, 4362, 4458, 4526, 50172, 4362, 4458, 4527, 50173, 4362, 4458, 4528, 50174, 4362, 4458, 4529, 50175, 4362, 4458, 4530,
    50176, 4362, 4458, 4531, 50177, 4362, 4458, 4532, 50178, 4362, 4458, 4533, 50179, 4362, 4458, 4534, 50180, 4362, 4458, 4535, 50181, 4362, 4458, 4536,
    50182, 4362, 4458, 12612, 50183, 4362, 4458, 4538, 50184, 4362, 4458, 4539, 50185, 4362, 4458, 4540, 50186, 4362, 4458, 4364, 50187, 4362, 4458, 4542,
    50188, 4362, 4458, 4543, 50189, 4362, 4458, 4544, 50190, 4362, 4458, 4545, 50191, 4362, 4458, 4546, 50192, 4362, 4459, 0, 50193, 4362, 4459, 4520,
    50194, 4362, 4459, 4521, 50195, 4362, 4459, 4522, 50196, 4362, 4459, 4523, 50197, 4362, 4459, 4524, 50198, 4362, 4459, 4525, 50199, 4362, 4459, 4526,
    50200, 4362, 4459, 4527, 50201, 4362, 4459, 4528, 50202, 4362, 4459, 4529, 50203, 4362, 4459, 4530, 50204, 4362, 4459, 4531, 50205, 4362, 4459, 4532,
    50206, 4362, 4459, 4533, 50207, 4362, 4459, 4534, 50208, 4362, 4459, 4535, 50209, 4362, 4459, 4536, 50210, 4362, 4459, 12612, 50211, 4362, 4459, 4538,
    50212, 4362, 4459, 4539, 50213, 4362, 4459, 4540, 50214, 4362, 4459, 4364, 50215, 4362, 4459, 4542, 50216, 4362, 4459, 4543, 50217, 4362, 4459, 4544,
    50218, 4362, 4459, 4545, 50219, 4362, 4459, 4546, 50220, 4362, 4460, 0, 50221, 4362, 4460, 4520, 50222, 4362, 4460, 4521, 50223, 4362, 4460, 4522,
    50224, 4362, 4460, 4523, 50225, 4362, 4460, 4524, 50226, 4362, 4460, 4525, 50227, 4362, 4460, 4526, 50228, 4362, 4460, 4527, 50229, 4362, 4460, 4528,
    50230, 4362, 4460, 4529, 50231, 4362, 4460, 4530, 50232, 4362, 4460, 4531, 50233, 4362, 4460, 4532, 50234, 4362, 4460, 4533, 50235, 4362, 4460, 4534,
    50236, 4362, 4460, 4535, 50237, 4362, 4460, 4536, 50238, 4362, 4460, 12612, 50239, 4362, 4460, 4538, 50240, 4362, 4460, 4539, 50241, 4362, 4460, 4540,
    50242, 4362, 4460, 4364, 50243, 4362, 4460, 4542, 50244, 4362, 4460, 4543, 50245, 4362, 4460, 4544, 50246, 4362, 4460, 4545, 50247, 4362, 4460, 4546,
    50248, 4362, 4461, 0, 50249, 4362, 4461, 4520, 50250, 4362, 4461, 4521, 50251, 4362, 4461, 4522, 50252, 4362, 4461, 4523, 50253, 4362, 4461, 4524,
    50254, 4362, 4461, 4525, 50255, 4362, 4461, 4526, 50256, 4362, 4461, 4527, 50257, 4362, 4461, 4528, 50258, 4362, 4461, 4529, 50259, 4362, 4461, 4530,
    50260, 4362, 4461, 4531, 50261, 4362, 4461, 4532, 50262, 4362, 4461, 4533, 50263, 4362, 4461, 4534, 50264, 4362, 4461, 4535, 50265, 4362, 4461, 4536,
    50266, 4362, 4461, 12612, 50267, 4362, 4461, 4538, 50268, 4362, 4461, 4539, 50269, 4362, 4461, 4540, 50270, 4362, 4461, 4364, 50271, 4362, 4461, 4542,
    50272, 4362, 4461, 4543, 50273, 4362, 4461, 4544, 50274, 4362, 4461, 4545, 50275, 4362, 4461, 4546, 50276, 4362, 4462, 0, 50277, 4362, 4462, 4520,
    50278, 4362, 4462, 4521, 50279, 4362, 4462, 4522, 50280, 4362, 4462, 4523, 50281, 4362, 4462, 4524, 50282, 4362, 4462, 4525, 50283, 4362, 4462, 4526,
    50284, 4362, 4462, 4527, 50285, 4362, 4462, 4528, 50286, 4362, 4462, 4529, 50287, 4362, 4462, 4530, 50288, 4362, 4462, 4531, 50289, 4362, 4462, 4532,
    50290, 4362, 4462, 4533, 50291, 4362, 4462, 4534, 50292, 4362, 4462, 4535, 50293, 4362, 4462, 4536, 50294, 4362, 4462, 12612, 50295, 4362, 4462, 4538,
    50296, 4362, 4462, 4539, 50297, 4362, 4462, 4540, 50298, 4362, 4462, 4364, 50299, 4362, 4462, 4542, 50300, 4362, 4462, 4543, 50301, 4362, 4462, 4544,
    50302, 4362, 4462, 4545, 50303, 4362, 4462, 4546, 50304, 4362, 4463, 0, 50305, 4362, 4463, 4520, 50306, 4362, 4463, 4521, 50307, 4362, 4463, 4522,
    50308, 4362, 4463, 4523, 50309, 4362, 4463, 4524, 50310, 4362, 4463, 4525, 50311, 4362, 4463, 4526, 50312, 4362, 4463, 4527, 50313, 4362, 4463, 4528,
    50314, 4362, 4463, 4529, 50315, 4362, 4463, 4530, 50316, 4362, 4463, 4531, 50317, 4362, 4463, 4532, 50318, 4362, 4463, 4533, 50319, 4362, 4463, 4534,
    50320, 4362, 4463, 4535, 50321, 4362, 4463, 4536, 50322, 4362, 4463, 12612, 50323, 4362, 4463, 4538, 50324, 4362, 4463, 4539, 50325, 4362, 4463, 4540,
    50326, 4362, 4463, 4364, 50327, 4362, 4463, 4542, 50328, 4362, 4463, 4543, 50329, 4362, 4463, 4544, 50330, 4362, 4463, 4545, 50331, 4362, 4463, 4546,
    50332, 4362, 4464, 0, 50333, 4362, 4464, 4520, 50334, 4362, 4464, 4521, 50335, 4362, 4464, 4522, 50336, 4362, 4464, 4523, 50337, 4362, 4464, 4524,
    50338, 4362, 4464, 4525, 50339, 4362, 4464, 4526, 50340, 4362, 4464, 4527, 50341, 4362, 4464, 4528, 50342, 4362, 4464, 4529, 50343, 4362, 4464, 4530,
    50344, 4362, 4464, 4531, 50345, 4362, 4464, 4532, 50346, 4362, 4464, 4533, 50347, 4362, 4464, 4534, 50348, 4362, 4464, 4535, 50349, 4362, 4464, 4536,
    50350, 4362, 4464, 12612, 50351, 4362, 4464, 4538, 50352, 4362, 4464, 4539, 50353, 4362, 4464, 4540, 50354, 4362, 4464, 4364, 50355, 4362, 4464, 4542,
    50356, 4362, 4464, 4543, 50357, 4362, 4464, 4544, 50358, 4362, 4464, 4545, 50359, 4362, 4464, 4546, 50360, 4362, 4465, 0, 50361, 4362, 4465, 4520,
    50362, 4362, 4465, 4521, 50363, 4362, 4465, 4522, 50364, 4362, 4465, 4523, 50365, 4362, 4465, 4524, 50366, 4362, 4465, 4525, 50367, 4362, 4465, 4526,
    50368, 4362, 4465, 4527, 50369, 4362, 4465, 4528, 50370, 4362, 4465, 4529, 50371, 4362, 4465, 4530, 50372, 4362, 4465, 4531, 50373, 4362, 4465, 4532,
    50374, 4362, 4465, 4533, 50375, 4362, 4465, 4534, 50376, 4362, 4465, 4535, 50377, 4362, 4465, 4536, 50378, 4362, 4465, 12612, 50379, 4362, 4465, 4538,
    50380, 4362, 4465, 4539, 50381, 4362, 4465, 4540, 50382, 4362, 4465, 4364, 50383, 4362, 4465, 4542, 50384, 4362, 4465, 4543, 50385, 4362, 4465, 4544,
    50386, 4362, 4465, 4545, 50387, 4362, 4465, 4546, 50388, 4362, 4466, 0, 50389, 4362, 4466, 4520, 50390, 4362, 4466, 4521, 50391, 4362, 4466, 4522,
    50392, 4362, 4466, 4523, 50393, 4362, 4466, 4524, 50394, 4362, 4466, 4525, 50395, 4362, 4466, 4526, 50396, 4362, 4466, 4527, 50397, 4362, 4466, 4528,
    50398, 4362, 4466, 4529, 50399, 4362, 4466, 4530, 50400, 4362, 4466, 4531, 50401, 4362, 4466, 4532, 50402, 4362, 4466, 4533, 50403, 4362, 4466, 4534,
    50404, 4362, 4466, 4535, 50405, 4362, 4466, 4536, 50406, 4362, 4466, 12612, 50407, 4362, 4466, 4538, 50408, 4362, 4466, 4539, 50409, 4362, 4466, 4540,
    50410, 4362, 4466, 4364, 50411, 4362, 4466, 4542, 50412, 4362, 4466, 4543, 50413, 4362, 4466, 4544, 50414, 4362, 4466, 4545, 50415, 4362, 4466, 4546,
    50416, 4362, 4467, 0, 50417, 4362, 4467, 4520, 50418, 4362, 4467, 4521, 50419, 4362, 4467, 4522, 50420, 4362, 4467, 4523, 50421, 4362, 4467, 4524,
    50422, 4362, 4467, 4525, 50423, 4362, 4467, 4526, 50424, 4362, 4467, 4527, 50425, 4362, 4467, 4528, 50426, 4362, 4467, 4529, 50427, 4362, 4467, 4530,
    50428, 4362, 4467, 4531, 50429, 4362, 4467, 4532, 50430, 4362, 4467, 4533, 50431, 4362, 4467, 4534, 50432, 4362, 4467, 4535, 50433, 4362, 4467, 4536,
    50434, 4362, 4467, 12612, 50435, 4362, 4467, 4538, 50436, 4362, 4467, 4539, 50437, 4362, 4467, 4540, 50438, 4362, 4467, 4364, 50439, 4362, 4467, 4542,
    50440, 4362, 4467, 4543, 50441, 4362, 4467, 4544, 50442, 4362, 4467, 4545, 50443, 4362, 4467, 4546, 50444, 4362, 4468, 0, 50445, 4362, 4468, 4520,
    50446, 4362, 4468, 4521, 50447, 4362, 4468, 4522, 50448, 4362, 4468, 4523, 50449, 4362, 4468, 4524, 50450, 4362, 4468, 4525, 50451, 4362, 4468, 4526,
    50452, 4362, 4468, 4527, 50453, 4362, 4468, 4528, 50454, 4362, 4468, 4529, 50455, 4362, 4468, 4530, 50456, 4362, 4468, 4531, 50457, 4362, 4468, 4532,
    50458, 4362, 4468, 4533, 50459, 4362, 4468, 4534, 50460, 4362, 4468, 4535, 50461, 4362, 4468, 4536, 50462, 4362, 4468, 12612, 50463, 4362, 4468, 4538,
    50464, 4362, 4468, 4539, 50465, 4362, 4468, 4540, 50466, 4362, 4468, 4364, 50467, 4362, 4468, 4542, 50468, 4362, 4468, 4543, 50469, 4362, 4468, 4544,
    50470, 4362, 4468, 4545, 50471, 4362, 4468, 4546, 50472, 4362, 4469, 0, 50473, 4362, 4469, 4520, 50474, 4362, 4469, 4521, 50475, 4362, 4469, 4522,
    50476, 4362, 4469, 4523, 50477, 4362, 4469, 4524, 50478, 4362, 4469, 4525, 50479, 4362, 4469, 4526, 50480, 4362, 4469, 4527, 50481, 4362, 4469, 4528,
    50482, 4362, 4469, 4529, 50483, 4362, 4469, 4530, 50484, 4362, 4469, 4531, 50485, 4362, 4469, 4532, 50486, 4362, 4469, 4533, 50487, 4362, 4469, 4534,
    50488, 4362, 4469, 4535, 50489, 4362, 4469, 4536, 50490, 4362, 4469, 12612, 50491, 4362, 4469, 4538, 50492, 4362, 4469, 4539, 50493, 4362, 4469, 4540,
    50494, 4362, 4469, 4364, 50495, 4362, 4469, 4542, 50496, 4362, 4469, 4543, 50497, 4362, 4469, 4544, 50498, 4362, 4469, 4545, 50499, 4362, 4469, 4546,
    50500, 4363, 4449, 0, 50501, 4363, 4449, 4520, 50502, 4363, 4449, 4521, 50503, 4363, 4449, 4522, 50504, 4363, 4449, 4523, 50505, 4363, 4449, 4524,
    50506, 4363, 4449, 4525, 50507, 4363, 4449, 4526, 50508, 4363, 4449, 4527, 50509, 4363, 4449, 4528, 50510, 4363, 4449, 4529, 50511, 4363, 4449, 4530,
    50512, 4363, 4449, 4531, 50513, 4363, 4449, 4532, 50514, 4363, 4449, 4533, 50515, 4363, 4449, 4534, 50516, 4363, 4449, 4535, 50517, 4363, 4449, 4536,
    50518, 4363, 4449, 12612, 50519, 4363, 4449, 4538, 50520, 4363, 4449, 4539, 50521, 4363, 4449, 4540, 50522, 4363, 4449, 4364, 50523, 4363, 4449, 4542,
    50524, 4363, 4449, 4543, 50525, 4363, 4449, 4544, 50526, 4363, 4449, 4545, 50527, 4363, 4449, 4546, 50528, 4363, 4450, 0, 50529, 4363, 4450, 4520,
    50530, 4363, 4450, 4521, 50531, 4363, 4450, 4522, 50532, 4363, 4450, 4523, 50533, 4363, 4450, 4524, 50534, 4363, 4450, 4525, 50535, 4363, 4450, 4526,
    50536, 4363, 4450, 4527, 50537, 4363, 4450, 4528, 50538, 4363, 4450, 4529, 50539, 4363, 4450, 4530, 50540, 4363, 4450, 4531, 50541, 4363, 4450, 4532,
    50542, 4363, 4450, 4533, 50543, 4363, 4450, 4534, 50544, 4363, 4450, 4535, 50545, 4363, 4450, 4536, 50546, 4363, 4450, 12612, 50547, 4363, 4450, 4538,
    50548, 4363, 4450, 4539, 50549, 4363, 4450, 4540, 50550, 4363, 4450, 4364, 50551, 4363, 4450, 4542, 50552, 4363, 4450, 4543, 50553, 4363, 4450, 4544,
    50554, 4363, 4450, 4545, 50555, 4363, 4450, 4546, 50556, 4363, 4451, 0, 50557, 4363, 4451, 4520, 50558, 4363, 4451, 4521, 50559, 4363, 4451, 4522,
    50560, 4363, 4451, 4523, 50561, 4363, 4451, 4524, 50562, 4363, 4451, 4525, 50563, 4363, 4451, 4526, 50564, 4363, 4451, 4527, 50565, 4363, 4451, 4528,
    50566, 4363, 4451, 4529, 50567, 4363, 4451, 4530, 50568, 4363, 4451, 4531, 50569, 4363, 4451, 4532, 50570, 4363, 4451, 4533, 50571, 4363, 4451, 4534,
    50572, 4363, 4451, 4535, 50573, 4363, 4451, 4536, 50574, 4363, 4451, 12612, 50575, 4363, 4451, 4538, 50576, 4363, 4451, 4539, 50577, 4363, 4451, 4540,
    50578, 4363, 4451, 4364, 50579, 4363, 4451, 4542, 50580, 4363, 4451, 4543, 50581, 4363, 4451, 4544, 50582, 4363, 4451, 4545, 50583, 4363, 4451, 4546,
    50584, 4363, 4452, 0, 50585, 4363, 4452, 4520, 50586, 4363, 4452, 4521, 50587, 4363, 4452, 4522, 50588, 4363, 4452, 4523, 50589, 4363, 4452, 4524,
    50590, 4363, 4452, 4525, 50591, 4363, 4452, 4526, 50592, 4363, 4452, 4527, 50593, 4363, 4452, 4528, 50594, 4363, 4452, 4529, 50595, 4363, 4452, 4530,
    50596, 4363, 4452, 4531, 50597, 4363, 4452, 4532, 50598, 4363, 4452, 4533, 50599, 4363, 4452, 4534, 50600, 4363, 4452, 4535, 50601, 4363, 4452, 4536,
    50602, 4363, 4452, 12612, 50603, 4363, 4452, 4538, 50604, 4363, 4452, 4539, 50605, 4363, 4452, 4540, 50606, 4363, 4452, 4364, 50607, 4363, 4452, 4542,
    50608, 4363, 4452, 4543, 50609, 4363, 4452, 4544, 50610, 4363, 4452, 4545, 50611, 4363, 4452, 4546, 50612, 4363, 4453, 0, 50613, 4363, 4453, 4520,
    50614, 4363, 4453, 4521, 50615, 4363, 4453, 4522, 50616, 4363, 4453, 4523, 50617, 4363, 4453, 4524, 50618, 4363, 4453, 4525, 50619, 4363, 4453, 4526,
    50620, 4363, 4453, 4527, 50621, 4363, 4453, 4528, 50622, 4363, 4453, 4529, 50623, 4363, 4453, 4530, 50624, 4363, 4453, 4531, 50625, 4363, 4453, 4532,
    50626, 4363, 4453, 4533, 50627, 4363, 4453, 4534, 50628, 4363, 4453, 4535, 50629, 4363, 4453, 4536, 50630, 4363, 4453, 12612, 50631, 4363, 4453, 4538,
    50632, 4363, 4453, 4539, 50633, 4363, 4453, 4540, 50634, 4363, 4453, 4364, 50635, 4363, 4453, 4542, 50636, 4363, 4453, 4543, 50637, 4363, 4453, 4544,
    50638, 4363, 4453, 4545, 50639, 4363, 4453, 4546, 50640, 4363, 4454, 0, 50641, 4363, 4454, 4520, 50642, 4363, 4454, 4521, 50643, 4363, 4454, 4522,
    50644, 4363, 4454, 4523, 50645, 4363, 4454, 4524, 50646, 4363, 4454, 4525, 50647, 4363, 4454, 4526, 50648, 4363, 4454, 4527, 50649, 4363, 4454, 4528,
    50650, 4363, 4454, 4529, 50651, 4363, 4454, 4530, 50652, 4363, 4454, 4531, 50653, 4363, 4454, 4532, 50654, 4363, 4454, 4533, 50655, 4363, 4454, 4534,
    50656, 4363, 4454, 4535, 50657, 4363, 4454, 4536, 50658, 4363, 4454, 12612, 50659, 4363, 4454, 4538, 50660, 4363, 4454, 4539, 50661, 4363, 4454, 4540,
    50662, 4363, 4454, 4364, 50663, 4363, 4454, 4542, 50664, 4363, 4454, 4543, 50665, 4363, 4454, 4544, 50666, 4363, 4454, 4545, 50667, 4363, 4454, 4546,
    50668, 4363, 4455, 0, 50669, 4363, 4455, 4520, 50670, 4363, 4455, 4521, 50671, 4363, 4455, 4522, 50672, 4363, 4455, 4523, 50673, 4363, 4455, 4524,
    50674, 4363, 4455, 4525, 50675, 4363, 4455, 4526, 50676, 4363, 4455, 4527, 50677, 4363, 4455, 4528, 50678, 4363, 4455, 4529, 50679, 4363, 4455, 4530,
    50680, 4363, 4455, 4531, 50681, 4363, 4455, 4532, 50682, 4363, 4455, 4533, 50683, 4363, 4455, 4534, 50684, 4363, 4455, 4535, 50685, 4363, 4455, 4536,
    50686, 4363, 4455, 12612, 50687, 4363, 4455, 4538, 50688, 4363, 4455, 4539, 50689, 4363, 4455, 4540, 50690, 4363, 4455, 4364, 50691, 4363, 4455, 4542,
    50692, 4363, 4455, 4543, 50693, 4363, 4455, 4544, 50694, 4363, 4455, 4545, 50695, 4363, 4455, 4546, 50696, 4363, 4456, 0, 50697, 4363, 4456, 4520,
    50698, 4363, 4456, 4521, 50699, 4363, 4456, 4522, 50700, 4363, 4456, 4523, 50701, 4363, 4456, 4524, 50702, 4363, 4456, 4525, 50703, 4363, 4456, 4526,
    50704, 4363, 4456, 4527, 50705, 4363, 4456, 4528, 50706, 4363, 4456, 4529, 50707, 4363, 4456, 4530, 50708, 4363, 4456, 4531, 50709, 4363, 4456, 4532,
    50710, 4363, 4456, 4533, 50711, 4363, 4456, 4534, 50712, 4363, 4456, 4535, 50713, 4363, 4456, 4536, 50714, 4363, 4456, 12612, 50715, 4363, 4456, 4538,
    50716, 4363, 4456, 4539, 50717, 4363, 4456, 4540, 50718, 4363, 4456, 4364, 50719, 4363, 4456, 4542, 50720, 4363, 4456, 4543, 50721, 4363, 4456, 4544,
    50722, 4363, 4456, 4545, 50723, 4363, 4456, 4546, 50724, 4363, 4457, 0, 50725, 4363, 4457, 4520, 50726, 4363, 4457, 4521, 50727, 4363, 4457, 4522,
    50728, 4363, 4457, 4523, 50729, 4363, 4457, 4524, 50730, 4363, 4457, 4525, 50731, 4363, 4457, 4526, 50732, 4363, 4457, 4527, 50733, 4363, 4457, 4528,
    50734, 4363, 4457, 4529, 50735, 4363, 4457, 4530, 50736, 4363, 4457, 4531, 50737, 4363, 4457, 4532, 50738, 4363, 4457, 4533, 50739, 4363, 4457, 4534,
    50740, 4363, 4457, 4535, 50741, 4363, 4457, 4536, 50742, 4363, 4457, 12612, 50743, 4363, 4457, 4538, 50744, 4363, 4457, 4539, 50745, 4363, 4457, 4540,
    50746, 4363, 4457, 4364, 50747, 4363, 4457, 4542, 50748, 4363, 4457, 4543, 50749, 4363, 4457, 4544, 50750, 4363, 4457, 4545, 50751, 4363, 4457, 4546,
    50752, 4363, 4458, 0, 50753, 4363, 4458, 4520, 50754, 4363, 4458, 4521, 50755, 4363, 4458, 4522, 50756, 4363, 4458, 4523, 50757, 4363, 4458, 4524,
    50758, 4363, 4458, 4525, 50759, 4363, 4458, 4526, 50760, 4363, 4458, 4527, 50761, 4363, 4458, 4528, 50762, 4363, 4458, 4529, 50763, 4363, 4458, 4530,
    50764, 4363, 4458, 4531, 50765, 4363, 4458, 4532, 50766, 4363, 4458, 4533, 50767, 4363, 4458, 4534, 50768, 4363, 4458, 4535, 50769, 4363, 4458, 4536,
    50770, 4363, 4458, 12612, 50771, 4363, 4458, 4538, 50772, 4363, 4458, 4539, 50773, 4363, 4458, 4540, 50774, 4363, 4458, 4364, 50775, 4363, 4458, 4542,
    50776, 4363, 4458, 4543, 50777, 4363, 4458, 4544, 50778, 4363, 4458, 4545, 50779, 4363, 4458, 4546, 50780, 4363, 4459, 0, 50781, 4363, 4459, 4520,
    50782, 4363, 4459, 4521, 50783, 4363, 4459, 4522, 50784, 4363, 4459, 4523, 50785, 4363, 4459, 4524, 50786, 4363, 4459, 4525, 50787, 4363, 4459, 4526,
    50788, 4363, 4459, 4527, 50789, 4363, 4459, 4528, 50790, 4363, 4459, 4529, 50791, 4363, 4459, 4530, 50792, 4363, 4459, 4531, 50793, 4363, 4459, 4532,
    50794, 4363, 4459, 4533, 50795, 4363, 4459, 4534, 50796, 4363, 4459, 4535, 50797, 4363, 4459, 4536, 50798, 4363, 4459, 12612, 50799, 4363, 4459, 4538,
    50800, 4363, 4459, 4539, 50801, 4363, 4459, 4540, 50802, 4363, 4459, 4364, 50803, 4363, 4459, 4542, 50804, 4363, 4459, 4543, 50805, 4363, 4459, 4544,
    50806, 4363, 4459, 4545, 50807, 4363, 4459, 4546, 50808, 4363, 4460, 0, 50809, 4363, 4460, 4520, 50810, 4363, 4460, 4521, 50811, 4363, 4460, 4522,
    50812, 4363, 4460, 4523, 50813, 4363, 4460, 4524, 50814, 4363, 4460, 4525, 50815, 4363, 4460, 4526, 50816, 4363, 4460, 4527, 50817, 4363, 4460, 4528,
    50818, 4363, 4460, 4529, 50819, 4363, 4460, 4530, 50820, 4363, 4460, 4531, 50821, 4363, 4460, 4532, 50822, 4363, 4460, 4533, 50823, 4363, 4460, 4534,
    50824, 4363, 4460, 4535, 50825, 4363, 4460, 4536, 50826, 4363, 4460, 12612, 50827, 4363, 4460, 4538, 50828, 4363, 4460, 4539, 50829, 4363, 4460, 4540,
    50830, 4363, 4460, 4364, 50831, 4363, 4460, 4542, 50832, 4363, 4460, 4543, 50833, 4363, 4460, 4544, 50834, 4363, 4460, 4545, 50835, 4363, 4460, 4546,
    50836, 4363, 4461, 0, 50837, 4363, 4461, 4520, 50838, 4363, 4461, 4521, 50839, 4363, 4461, 4522, 50840, 4363, 4461, 4523, 50841, 4363, 4461, 4524,
    50842, 4363, 4461, 4525, 50843, 4363, 4461, 4526, 50844, 4363, 4461, 4527, 50845, 4363, 4461, 4528, 50846, 4363, 4461, 4529, 50847, 4363, 4461, 4530,
    50848, 4363, 4461, 4531, 50849, 4363, 4461, 4532, 50850, 4363, 4461, 4533, 50851, 4363, 4461, 4534, 50852, 4363, 4461, 4535, 50853, 4363, 4461, 4536,
    50854, 4363, 4461, 12612, 50855, 4363, 4461, 4538, 50856, 4363, 4461, 4539, 50857, 4363, 4461, 4540, 50858, 4363, 4461, 4364, 50859, 4363, 4461, 4542,
    50860, 4363, 4461, 4543, 50861, 4363, 4461, 4544, 50862, 4363, 4461, 4545, 50863, 4363, 4461, 4546, 50864, 4363, 4462, 0, 50865, 4363, 4462, 4520,
    50866, 4363, 4462, 4521, 50867, 4363, 4462, 4522, 50868, 4363, 4462, 4523, 50869, 4363, 4462, 4524, 50870, 4363, 4462, 4525, 50871, 4363, 4462, 4526,
    50872, 4363, 4462, 4527, 50873, 4363, 4462, 4528, 50874, 4363, 4462, 4529, 50875, 4363, 4462, 4530, 50876, 4363, 4462, 4531, 50877, 4363, 4462, 4532,
    50878, 4363, 4462, 4533, 50879, 4363, 4462, 4534, 50880, 4363, 4462, 4535, 50881, 4363, 4462, 4536, 50882, 4363, 4462, 12612, 50883, 4363, 4462, 4538,
    50884, 4363, 4462, 4539, 50885, 4363, 4462, 4540, 50886, 4363, 4462, 4364, 50887, 4363, 4462, 4542, 50888, 4363, 4462, 4543, 50889, 4363, 4462, 4544,
    50890, 4363, 4462, 4545, 50891, 4363, 4462, 4546, 50892, 4363, 4463, 0, 50893, 4363, 4463, 4520, 50894, 4363, 4463, 4521, 50895, 4363, 4463, 4522,
    50896, 4363, 4463, 4523, 50897, 4363, 4463, 4524, 50898, 4363, 4463, 4525, 50899, 4363, 4463, 4526, 50900, 4363, 4463, 4527, 50901, 4363, 4463, 4528,
    50902, 4363, 4463, 4529, 50903, 4363, 4463, 4530, 50904, 4363, 4463, 4531, 50905, 4363, 4463, 4532, 50906, 4363, 4463, 4533, 50907, 4363, 4463, 4534,
    50908, 4363, 4463, 4535, 50909, 4363, 4463, 4536, 50910, 4363, 4463, 12612, 50911, 4363, 4463, 4538, 50912, 4363, 4463, 4539, 50913, 4363, 4463, 4540,
    50914, 4363, 4463, 4364, 50915, 4363, 4463, 4542, 50916, 4363, 4463, 4543, 50917, 4363, 4463, 4544, 50918, 4363, 4463, 4545, 50919, 4363, 4463, 4546,
    50920, 4363, 4464, 0, 50921, 4363, 4464, 4520, 50922, 4363, 4464, 4521, 50923, 4363, 4464, 4522, 50924, 4363, 4464, 4523, 50925, 4363, 4464, 4524,
    50926, 4363, 4464, 4525, 50927, 4363, 4464, 4526, 50928, 4363, 4464, 4527, 50929, 4363, 4464, 4528, 50930, 4363, 4464, 4529, 50931, 4363, 4464, 4530,
    50932, 4363, 4464, 4531, 50933, 4363, 4464, 4532, 50934, 4363, 4464, 4533, 50935, 4363, 4464, 4534, 50936, 4363, 4464, 4535, 50937, 4363, 4464, 4536,
    50938, 4363, 4464, 12612, 50939, 4363, 4464, 4538, 50940, 4363, 4464, 4539, 50941, 4363, 4464, 4540, 50942, 4363, 4464, 4364, 50943, 4363, 4464, 4542,
    50944, 4363, 4464, 4543, 50945, 4363, 4464, 4544, 50946, 4363, 4464, 4545, 50947, 4363, 4464, 4546, 50948, 4363, 4465, 0, 50949, 4363, 4465, 4520,
    50950, 4363, 4465, 4521, 50951, 4363, 4465, 4522, 50952, 4363, 4465, 4523, 50953, 4363, 4465, 4524, 50954, 4363, 4465, 4525, 50955, 4363, 4465, 4526,
    50956, 4363, 4465, 4527, 50957, 4363, 4465, 4528, 50958, 4363, 4465, 4529, 50959, 4363, 4465, 4530, 50960, 4363, 4465, 4531, 50961, 4363, 4465, 4532,
    50962, 4363, 4465, 4533, 50963, 4363, 4465, 4534, 50964, 4363, 4465, 4535, 50965, 4363, 4465, 4536, 50966, 4363, 4465, 12612, 50967, 4363, 4465, 4538,
    50968, 4363, 4465, 4539, 50969, 4363, 4465, 4540, 50970, 4363, 4465, 4364, 50971, 4363, 4465, 4542, 50972, 4363, 4465, 4543, 50973, 4363, 4465, 4544,
    50974, 4363, 4465, 4545, 50975, 4363, 4465, 4546, 50976, 4363, 4466, 0, 50977, 4363, 4466, 4520, 50978, 4363, 4466, 4521, 50979, 4363, 4466, 4522,
    50980, 4363, 4466, 4523, 50981, 4363, 4466, 4524, 50982, 4363, 4466, 4525, 50983, 4363, 4466, 4526, 50984, 4363, 4466, 4527, 50985, 4363, 4466, 4528,
    50986, 4363, 4466, 4529, 50987, 4363, 4466, 4530, 50988, 4363, 4466, 4531, 50989, 4363, 4466, 4532, 50990, 4363, 4466, 4533, 50991, 4363, 4466, 4534,
    50992, 4363, 4466, 4535, 50993, 4363, 4466, 4536, 50994, 4363, 4466, 12612, 50995, 4363, 4466, 4538, 50996, 4363, 4466, 4539, 50997, 4363, 4466, 4540,
    50998, 4363, 4466, 4364, 50999, 4363, 4466, 4542, 51000, 4363, 4466, 4543, 51001, 4363, 4466, 4544, 51002, 4363, 4466, 4545, 51003, 4363, 4466, 4546,
    51004, 4363, 4467, 0, 51005, 4363, 4467, 4520, 51006, 4363, 4467, 4521, 51007, 4363, 4467, 4522, 51008, 4363, 4467, 4523, 51009, 4363, 4467, 4524,
    51010, 4363, 4467, 4525, 51011, 4363, 4467, 4526, 51012, 4363, 4467, 4527, 51013, 4363, 4467, 4528, 51014, 4363, 4467, 4529, 51015, 4363, 4467, 4530,
    51016, 4363, 4467, 4531, 51017, 4363, 4467, 4532, 51018, 4363, 4467, 4533, 51019, 4363, 4467, 4534, 51020, 4363, 4467, 4535, 51021, 4363, 4467, 4536,
    51022, 4363, 4467, 12612, 51023, 4363, 4467, 4538, 51024, 4363, 4467, 4539, 51025, 4363, 4467, 4540, 51026, 4363, 4467, 4364, 51027, 4363, 4467, 4542,
    51028, 4363, 4467, 4543, 51029, 4363, 4467, 4544, 51030, 4363, 4467, 4545, 51031, 4363, 4467, 4546, 51032, 4363, 4468, 0, 51033, 4363, 4468, 4520,
    51034, 4363, 4468, 4521, 51035, 4363, 4468, 4522, 51036, 4363, 4468, 4523, 51037, 4363, 4468, 4524, 51038, 4363, 4468, 4525, 51039, 4363, 4468, 4526,
    51040, 4363, 4468, 4527, 51041, 4363, 4468, 4528, 51042, 4363, 4468, 4529, 51043, 4363, 4468, 4530, 51044, 4363, 4468, 4531, 51045, 4363, 4468, 4532,
    51046, 4363, 4468, 4533, 51047, 4363, 4468, 4534, 51048, 4363, 4468, 4535, 51049, 4363, 4468, 4536, 51050, 4363, 4468, 12612, 51051, 4363, 4468, 4538,
    51052, 4363, 4468, 4539, 51053, 4363, 4468, 4540, 51054, 4363, 4468, 4364, 51055, 4363, 4468, 4542, 51056, 4363, 4468, 4543, 51057, 4363, 4468, 4544,
    51058, 4363, 4468, 4545, 51059, 4363, 4468, 4546, 51060, 4363, 4469, 0, 51061, 4363, 4469, 4520, 51062, 4363, 4469, 4521, 51063, 4363, 4469, 4522,
    51064, 4363, 4469, 4523, 51065, 4363, 4469, 4524, 51066, 4363, 4469, 4525, 51067, 4363, 4469, 4526, 51068, 4363, 4469, 4527, 51069, 4363, 4469, 4528,
    51070, 4363, 4469, 4529, 51071, 4363, 4469, 4530, 51072, 4363, 4469, 4531, 51073, 4363, 4469, 4532, 51074, 4363, 4469, 4533, 51075, 4363, 4469, 4534,
    51076, 4363, 4469, 4535, 51077, 4363, 4469, 4536, 51078, 4363, 4469, 12612, 51079, 4363, 4469, 4538, 51080, 4363, 4469, 4539, 51081, 4363, 4469, 4540,
    51082, 4363, 4469, 4364, 51083, 4363, 4469, 4542, 51084, 4363, 4469, 4543, 51085, 4363, 4469, 4544, 51086, 4363, 4469, 4545, 51087, 4363, 4469, 4546,
    51088, 4364, 4449, 0, 51089, 4364, 4449, 4520, 51090, 4364, 4449, 4521, 51091, 4364, 4449, 4522, 51092, 4364, 4449, 4523, 51093, 4364, 4449, 4524,
    51094, 4364, 4449, 4525, 51095, 4364, 4449, 4526, 51096, 4364, 4449, 4527, 51097, 4364, 4449, 4528, 51098, 4364, 4449, 4529, 51099, 4364, 4449, 4530,
    51100, 4364, 4449, 4531, 51101, 4364, 4449, 4532, 51102, 4364, 4449, 4533, 51103, 4364, 4449, 4534, 51104, 4364, 4449, 4535, 51105, 4364, 4449, 4536,
    51106, 4364, 4449, 12612, 51107, 4364, 4449, 4538, 51108, 4364, 4449, 4539, 51109, 4364, 4449, 4540, 51110, 4364, 4449, 4364, 51111, 4364, 4449, 4542,
    51112, 4364, 4449, 4543, 51113, 4364, 4449, 4544, 51114, 4364, 4449, 4545, 51115, 4364, 4449, 4546, 51116, 4364, 4450, 0, 51117, 4364, 4450, 4520,
    51118, 4364, 4450, 4521, 51119, 4364, 4450, 4522, 51120, 4364, 4450, 4523, 51121, 4364, 4450, 4524, 51122, 4364, 4450, 4525, 51123, 4364, 4450, 4526,
    51124, 4364, 4450, 4527, 51125, 4364, 4450, 4528, 51126, 4364, 4450, 4529, 51127, 4364, 4450, 4530, 51128, 4364, 4450, 4531, 51129, 4364, 4450, 4532,
    51130, 4364, 4450, 4533, 51131, 4364, 4450, 4534, 51132, 4364, 4450, 4535, 51133, 4364, 4450, 4536, 51134, 4364, 4450, 12612, 51135, 4364, 4450, 4538,
    51136, 4364, 4450, 4539, 51137, 4364, 4450, 4540, 51138, 4364, 4450, 4364, 51139, 4364, 4450, 4542, 51140, 4364, 4450, 4543, 51141, 4364, 4450, 4544,
    51142, 4364, 4450, 4545, 51143, 4364, 4450, 4546, 51144, 4364, 4451, 0, 51145, 4364, 4451, 4520, 51146, 4364, 4451, 4521, 51147, 4364, 4451, 4522,
    51148, 4364, 4451, 4523, 51149, 4364, 4451, 4524, 51150, 4364, 4451, 4525, 51151, 4364, 4451, 4526, 51152, 4364, 4451, 4527, 51153, 4364, 4451, 4528,
    51154, 4364, 4451, 4529, 51155, 4364, 4451, 4530, 51156, 4364, 4451, 4531, 51157, 4364, 4451, 4532, 51158, 4364, 4451, 4533, 51159, 4364, 4451, 4534,
    51160, 4364, 4451, 4535, 51161, 4364, 4451, 4536, 51162, 4364, 4451, 12612, 51163, 4364, 4451, 4538, 51164, 4364, 4451, 4539, 51165, 4364, 4451, 4540,
    51166, 4364, 4451, 4364, 51167, 4364, 4451, 4542, 51168, 4364, 4451, 4543, 51169, 4364, 4451, 4544, 51170, 4364, 4451, 4545, 51171, 4364, 4451, 4546,
    51172, 4364, 4452, 0, 51173, 4364, 4452, 4520, 51174, 4364, 4452, 4521, 51175, 4364, 4452, 4522, 51176, 4364, 4452, 4523, 51177, 4364, 4452, 4524,
    51178, 4364, 4452, 4525, 51179, 4364, 4452, 4526, 51180, 4364, 4452, 4527, 51181, 4364, 4452, 4528, 51182, 4364, 4452, 4529, 51183, 4364, 4452, 4530,
    51184, 4364, 4452, 4531, 51185, 4364, 4452, 4532, 51186, 4364, 4452, 4533, 51187, 4364, 4452, 4534, 51188, 4364, 4452, 4535, 51189, 4364, 4452, 4536,
    51190, 4364, 4452, 12612, 51191, 4364, 4452, 4538, 51192, 4364, 4452, 4539, 51193, 4364, 4452, 4540, 51194, 4364, 4452, 4364, 51195, 4364, 4452, 4542,
    51196, 4364, 4452, 4543, 51197, 4364, 4452, 4544, 51198, 4364, 4452, 4545, 51199, 4364, 4452, 4546, 51200, 4364, 4453, 0, 51201, 4364, 4453, 4520,
    51202, 4364, 4453, 4521, 51203, 4364, 4453, 4522, 51204, 4364, 4453, 4523, 51205, 4364, 4453, 4524, 51206, 4364, 4453, 4525, 51207, 4364, 4453, 4526,
    51208, 4364, 4453, 4527, 51209, 4364, 4453, 4528, 51210, 4364, 4453, 4529, 51211, 4364, 4453, 4530, 51212, 4364, 4453, 4531, 51213, 4364, 4453, 4532,
    51214, 4364, 4453, 4533, 51215, 4364, 4453, 4534, 51216, 4364, 4453, 4535, 51217, 4364, 4453, 4536, 51218, 4364, 4453, 12612, 51219, 4364, 4453, 4538,
    51220, 4364, 4453, 4539, 51221, 4364, 4453, 4540, 51222, 4364, 4453, 4364, 51223, 4364, 4453, 4542, 51224, 4364, 4453, 4543, 51225, 4364, 4453, 4544,
    51226, 4364, 4453, 4545, 51227, 4364, 4453, 4546, 51228, 4364, 4454, 0, 51229, 4364, 4454, 4520, 51230, 4364, 4454, 4521, 51231, 4364, 4454, 4522,
    51232, 4364, 4454, 4523, 51233, 4364, 4454, 4524, 51234, 4364, 4454, 4525, 51235, 4364, 4454, 4526, 51236, 4364, 4454, 4527, 51237, 4364, 4454, 4528,
    51238, 4364, 4454, 4529, 51239, 4364, 4454, 4530, 51240, 4364, 4454, 4531, 51241, 4364, 4454, 4532, 51242, 4364, 4454, 4533, 51243, 4364, 4454, 4534,
    51244, 4364, 4454, 4535, 51245, 4364, 4454, 4536, 51246, 4364, 4454, 12612, 51247, 4364, 4454, 4538, 51248, 4364, 4454, 4539, 51249, 4364, 4454, 4540,
    51250, 4364, 4454, 4364, 51251, 4364, 4454, 4542, 51252, 4364, 4454, 4543, 51253, 4364, 4454, 4544, 51254, 4364, 4454, 4545, 51255, 4364, 4454, 4546,
    51256, 4364, 4455, 0, 51257, 4364, 4455, 4520, 51258, 4364, 4455, 4521, 51259, 4364, 4455, 4522, 51260, 4364, 4455, 4523, 51261, 4364, 4455, 4524,
    51262, 4364, 4455, 4525, 51263, 4364, 4455, 4526, 51264, 4364, 4455, 4527, 51265, 4364, 4455, 4528, 51266, 4364, 4455, 4529, 51267, 4364, 4455, 4530,
    51268, 4364, 4455, 4531, 51269, 4364, 4455, 4532, 51270, 4364, 4455, 4533, 51271, 4364, 4455, 4534, 51272, 4364, 4455, 4535, 51273, 4364, 4455, 4536,
    51274, 4364, 4455, 12612, 51275, 4364, 4455, 4538, 51276, 4364, 4455, 4539, 51277, 4364, 4455, 4540, 51278, 4364, 4455, 4364, 51279, 4364, 4455, 4542,
    51280, 4364, 4455, 4543, 51281, 4364, 4455, 4544, 51282, 4364, 4455, 4545, 51283, 4364, 4455, 4546, 51284, 4364, 4456, 0, 51285, 4364, 4456, 4520,
    51286, 4364, 4456, 4521, 51287, 4364, 4456, 4522, 51288, 4364, 4456, 4523, 51289, 4364, 4456, 4524, 51290, 4364, 4456, 4525, 51291, 4364, 4456, 4526,
    51292, 4364, 4456, 4527, 51293, 4364, 4456, 4528, 51294, 4364, 4456, 4529, 51295, 4364, 4456, 4530, 51296, 4364, 4456, 4531, 51297, 4364, 4456, 4532,
    51298, 4364, 4456, 4533, 51299, 4364, 4456, 4534, 51300, 4364, 4456, 4535, 51301, 4364, 4456, 4536, 51302, 4364, 4456, 12612, 51303, 4364, 4456, 4538,
    51304, 4364, 4456, 4539, 51305, 4364, 4456, 4540, 51306, 4364, 4456, 4364, 51307, 4364, 4456, 4542, 51308, 4364, 4456, 4543, 51309, 4364, 4456, 4544,
    51310, 4364, 4456, 4545, 51311, 4364, 4456, 4546, 51312, 4364, 4457, 0, 51313, 4364, 4457, 4520, 51314, 4364, 4457, 4521, 51315, 4364, 4457, 4522,
    51316, 4364, 4457, 4523, 51317, 4364, 4457, 4524, 51318, 4364, 4457, 4525, 51319, 4364, 4457, 4526, 51320, 4364, 4457, 4527, 51321, 4364, 4457, 4528,
    51322, 4364, 4457, 4529, 51323, 4364, 4457, 4530, 51324, 4364, 4457, 4531, 51325, 4364, 4457, 4532, 51326, 4364, 4457, 4533, 51327, 4364, 4457, 4534,
    51328, 4364, 4457, 4535, 51329, 4364, 4457, 4536, 51330, 4364, 4457, 12612, 51331, 4364, 4457, 4538, 51332, 4364, 4457, 4539, 51333, 4364, 4457, 4540,
    51334, 4364, 4457, 4364, 51335, 4364, 4457, 4542, 51336, 4364, 4457, 4543, 51337, 4364, 4457, 4544, 51338, 4364, 4457, 4545, 51339, 4364, 4457, 4546,
    51340, 4364, 4458, 0, 51341, 4364, 4458, 4520, 51342, 4364, 4458, 4521, 51343, 4364, 4458, 4522, 51344, 4364, 4458, 4523, 51345, 4364, 4458, 4524,
    51346, 4364, 4458, 4525, 51347, 4364, 4458, 4526, 51348, 4364, 4458, 4527, 51349, 4364, 4458, 4528, 51350, 4364, 4458, 4529, 51351, 4364, 4458, 4530,
    51352, 4364, 4458, 4531, 51353, 4364, 4458, 4532, 51354, 4364, 4458, 4533, 51355, 4364, 4458, 4534, 51356, 4364, 4458, 4535, 51357, 4364, 4458, 4536,
    51358, 4364, 4458, 12612, 51359, 4364, 4458, 4538, 51360, 4364, 4458, 4539, 51361, 4364, 4458, 4540, 51362, 4364, 4458, 4364, 51363, 4364, 4458, 4542,
    51364, 4364, 4458, 4543, 51365, 4364, 4458, 4544, 51366, 4364, 4458, 4545, 51367, 4364, 4458, 4546, 51368, 4364, 4459, 0, 51369, 4364, 4459, 4520,
    51370, 4364, 4459, 4521, 51371, 4364, 4459, 4522, 51372, 4364, 4459, 4523, 51373, 4364, 4459, 4524, 51374, 4364, 4459, 4525, 51375, 4364, 4459, 4526,
    51376, 4364, 4459, 4527, 51377, 4364, 4459, 4528, 51378, 4364, 4459, 4529, 51379, 4364, 4459, 4530, 51380, 4364, 4459, 4531, 51381, 4364, 4459, 4532,
    51382, 4364, 4459, 4533, 51383, 4364, 4459, 4534, 51384, 4364, 4459, 4535, 51385, 4364, 4459, 4536, 51386, 4364, 4459, 12612, 51387, 4364, 4459, 4538,
    51388, 4364, 4459, 4539, 51389, 4364, 4459, 4540, 51390, 4364, 4459, 4364, 51391, 4364, 4459, 4542, 51392, 4364, 4459, 4543, 51393, 4364, 4459, 4544,
    51394, 4364, 4459, 4545, 51395, 4364, 4459, 4546, 51396, 4364, 4460, 0, 51397, 4364, 4460, 4520, 51398, 4364, 4460, 4521, 51399, 4364, 4460, 4522,
    51400, 4364, 4460, 4523, 51401, 4364, 4460, 4524, 51402, 4364, 4460, 4525, 51403, 4364, 4460, 4526, 51404, 4364, 4460, 4527, 51405, 4364, 4460, 4528,
    51406, 4364, 4460, 4529, 51407, 4364, 4460, 4530, 51408, 4364, 4460, 4531, 51409, 4364, 4460, 4532, 51410, 4364, 4460, 4533, 51411, 4364, 4460, 4534,
    51412, 4364, 4460, 4535, 51413, 4364, 4460, 4536, 51414, 4364, 4460, 12612, 51415, 4364, 4460, 4538, 51416, 4364, 4460, 4539, 51417, 4364, 4460, 4540,
    51418, 4364, 4460, 4364, 51419, 4364, 4460, 4542, 51420, 4364, 4460, 4543, 51421, 4364, 4460, 4544, 51422, 4364, 4460, 4545, 51423, 4364, 4460, 4546,
    51424, 4364, 4461, 0, 51425, 4364, 4461, 4520, 51426, 4364, 4461, 4521, 51427, 4364, 4461, 4522, 51428, 4364, 4461, 4523, 51429, 4364, 4461, 4524,
    51430, 4364, 4461, 4525, 51431, 4364, 4461, 4526, 51432, 4364, 4461, 4527, 51433, 4364, 4461, 4528, 51434, 4364, 4461, 4529, 51435, 4364, 4461, 4530,
    51436, 4364, 4461, 4531, 51437, 4364, 4461, 4532, 51438, 4364, 4461, 4533, 51439, 4364, 4461, 4534, 51440, 4364, 4461, 4535, 51441, 4364, 4461, 4536,
    51442, 4364, 4461, 12612, 51443, 4364, 4461, 4538, 51444, 4364, 4461, 4539, 51445, 4364, 4461, 4540, 51446, 4364, 4461, 4364, 51447, 4364, 4461, 4542,
    51448, 4364, 4461, 4543, 51449, 4364, 4461, 4544, 51450, 4364, 4461, 4545, 51451, 4364, 4461, 4546, 51452, 4364, 4462, 0, 51453, 4364, 4462, 4520,
    51454, 4364, 4462, 4521, 51455, 4364, 4462, 4522, 51456, 4364, 4462, 4523, 51457, 4364, 4462, 4524, 51458, 4364, 4462, 4525, 51459, 4364, 4462, 4526,
    51460, 4364, 4462, 4527, 51461, 4364, 4462, 4528, 51462, 4364, 4462, 4529, 51463, 4364, 4462, 4530, 51464, 4364, 4462, 4531, 51465, 4364, 4462, 4532,
    51466, 4364, 4462, 4533, 51467, 4364, 4462, 4534, 51468, 4364, 4462, 4535, 51469, 4364, 4462, 4536, 51470, 4364, 4462, 12612, 51471, 4364, 4462, 4538,
    51472, 4364, 4462, 4539, 51473, 4364, 4462, 4540, 51474, 4364, 4462, 4364, 51475, 4364, 4462, 4542, 51476, 4364, 4462, 4543, 51477, 4364, 4462, 4544,
    51478, 4364, 4462, 4545, 51479, 4364, 4462, 4546, 51480, 4364, 4463, 0, 51481, 4364, 4463, 4520, 51482, 4364, 4463, 4521, 51483, 4364, 4463, 4522,
    51484, 4364, 4463, 4523, 51485, 4364, 4463, 4524, 51486, 4364, 4463, 4525, 51487, 4364, 4463, 4526, 51488, 4364, 4463, 4527, 51489, 4364, 4463, 4528,
    51490, 4364, 4463, 4529, 51491, 4364, 4463, 4530, 51492, 4364, 4463, 4531, 51493, 4364, 4463, 4532, 51494, 4364, 4463, 4533, 51495, 4364, 4463, 4534,
    51496, 4364, 4463, 4535, 51497, 4364, 4463, 4536, 51498, 4364, 4463, 12612, 51499, 4364, 4463, 4538, 51500, 4364, 4463, 4539, 51501, 4364, 4463, 4540,
    51502, 4364, 4463, 4364, 51503, 4364, 4463, 4542, 51504, 4364, 4463, 4543, 51505, 4364, 4463, 4544, 51506, 4364, 4463, 4545, 51507, 4364, 4463, 4546,
    51508, 4364, 4464, 0, 51509, 4364, 4464, 4520, 51510, 4364, 4464, 4521, 51511, 4364, 4464, 4522, 51512, 4364, 4464, 4523, 51513, 4364, 4464, 4524,
    51514, 4364, 4464, 4525, 51515, 4364, 4464, 4526, 51516, 4364, 4464, 4527, 51517, 4364, 4464, 4528, 51518, 4364, 4464, 4529, 51519, 4364, 4464, 4530,
    51520, 4364, 4464, 4531, 51521, 4364, 4464, 4532, 51522, 4364, 4464, 4533, 51523, 4364, 4464, 4534, 51524, 4364, 4464, 4535, 51525, 4364, 4464, 4536,
    51526, 4364, 4464, 12612, 51527, 4364, 4464, 4538, 51528, 4364, 4464, 4539, 51529, 4364, 4464, 4540, 51530, 4364, 4464, 4364, 51531, 4364, 4464, 4542,
    51532, 4364, 4464, 4543, 51533, 4364, 4464, 4544, 51534, 4364, 4464, 4545, 51535, 4364, 4464, 4546, 51536, 4364, 4465, 0, 51537, 4364, 4465, 4520,
    51538, 4364, 4465, 4521, 51539, 4364, 4465, 4522, 51540, 4364, 4465, 4523, 51541, 4364, 4465, 4524, 51542, 4364, 4465, 4525, 51543, 4364, 4465, 4526,
    51544, 4364, 4465, 4527, 51545, 4364, 4465, 4528, 51546, 4364, 4465, 4529, 51547, 4364, 4465, 4530, 51548, 4364, 4465, 4531, 51549, 4364, 4465, 4532,
    51550, 4364, 4465, 4533, 51551, 4364, 4465, 4534, 51552, 4364, 4465, 4535, 51553, 4364, 4465, 4536, 51554, 4364, 4465, 12612, 51555, 4364, 4465, 4538,
    51556, 4364, 4465, 4539, 51557, 4364, 4465, 4540, 51558, 4364, 4465, 4364, 51559, 4364, 4465, 4542, 51560, 4364, 4465, 4543, 51561, 4364, 4465, 4544,
    51562, 4364, 4465, 4545, 51563, 4364, 4465, 4546, 51564, 4364, 4466, 0, 51565, 4364, 4466, 4520, 51566, 4364, 4466, 4521, 51567, 4364, 4466, 4522,
    51568, 4364, 4466, 4523, 51569, 4364, 4466, 4524, 51570, 4364, 4466, 4525, 51571, 4364, 4466, 4526, 51572, 4364, 4466, 4527, 51573, 4364, 4466, 4528,
    51574, 4364, 4466, 4529, 51575, 4364, 4466, 4530, 51576, 4364, 4466, 4531, 51577, 4364, 4466, 4532, 51578, 4364, 4466, 4533, 51579, 4364, 4466, 4534,
    51580, 4364, 4466, 4535, 51581, 4364, 4466, 4536, 51582, 4364, 4466, 12612, 51583, 4364, 4466, 4538, 51584, 4364, 4466, 4539, 51585, 4364, 4466, 4540,
    51586, 4364, 4466, 4364, 51587, 4364, 4466, 4542, 51588, 4364, 4466, 4543, 51589, 4364, 4466, 4544, 51590, 4364, 4466, 4545, 51591, 4364, 4466, 4546,
    51592, 4364, 4467, 0, 51593, 4364, 4467, 4520, 51594, 4364, 4467, 4521, 51595, 4364, 4467, 4522, 51596, 4364, 4467, 4523, 51597, 4364, 4467, 4524,
    51598, 4364, 4467, 4525, 51599, 4364, 4467, 4526, 51600, 4364, 4467, 4527, 51601, 4364, 4467, 4528, 51602, 4364, 4467, 4529, 51603, 4364, 4467, 4530,
    51604, 4364, 4467, 4531, 51605, 4364, 4467, 4532, 51606, 4364, 4467, 4533, 51607, 4364, 4467, 4534, 51608, 4364, 4467, 4535, 51609, 4364, 4467, 4536,
    51610, 4364, 4467, 12612, 51611, 4364, 4467, 4538, 51612, 4364, 4467, 4539, 51613, 4364, 4467, 4540, 51614, 4364, 4467, 4364, 51615, 4364, 4467, 4542,
    51616, 4364, 4467, 4543, 51617, 4364, 4467, 4544, 51618, 4364, 4467, 4545, 51619, 4364, 4467, 4546, 51620, 4364, 4468, 0, 51621, 4364, 4468, 4520,
    51622, 4364, 4468, 4521, 51623, 4364, 4468, 4522, 51624, 4364, 4468, 4523, 51625, 4364, 4468, 4524, 51626, 4364, 4468, 4525, 51627, 4364, 4468, 4526,
    51628, 4364, 4468, 4527, 51629, 4364, 4468, 4528, 51630, 4364, 4468, 4529, 51631, 4364, 4468, 4530, 51632, 4364, 4468, 4531, 51633, 4364, 4468, 4532,
    51634, 4364, 4468, 4533, 51635, 4364, 4468, 4534, 51636, 4364, 4468, 4535, 51637, 4364, 4468, 4536, 51638, 4364, 4468, 12612, 51639, 4364, 4468, 4538,
    51640, 4364, 4468, 4539, 51641, 4364, 4468, 4540, 51642, 4364, 4468, 4364, 51643, 4364, 4468, 4542, 51644, 4364, 4468, 4543, 51645, 4364, 4468, 4544,
    51646, 4364, 4468, 4545, 51647, 4364, 4468, 4546, 51648, 4364, 4469, 0, 51649, 4364, 4469, 4520, 51650, 4364, 4469, 4521, 51651, 4364, 4469, 4522,
    51652, 4364, 4469, 4523, 51653, 4364, 4469, 4524, 51654, 4364, 4469, 4525, 51655, 4364, 4469, 4526, 51656, 4364, 4469, 4527, 51657, 4364, 4469, 4528,
    51658, 4364, 4469, 4529, 51659, 4364, 4469, 4530, 51660, 4364, 4469, 4531, 51661, 4364, 4469, 4532, 51662, 4364, 4469, 4533, 51663, 4364, 4469, 4534,
    51664, 4364, 4469, 4535, 51665, 4364, 4469, 4536, 51666, 4364, 4469, 12612, 51667, 4364, 4469, 4538, 51668, 4364, 4469, 4539, 51669, 4364, 4469, 4540,
    51670, 4364, 4469, 4364, 51671, 4364, 4469, 4542, 51672, 4364, 4469, 4543, 51673, 4364, 4469, 4544, 51674, 4364, 4469, 4545, 51675, 4364, 4469, 4546,
    51676, 4365, 4449, 0, 51677, 4365, 4449, 4520, 51678, 4365, 4449, 4521, 51679, 4365, 4449, 4522, 51680, 4365, 4449, 4523, 51681, 4365, 4449, 4524,
    51682, 4365, 4449, 4525, 51683, 4365, 4449, 4526, 51684, 4365, 4449, 4527, 51685, 4365, 4449, 4528, 51686, 4365, 4449, 4529, 51687, 4365, 4449, 4530,
    51688, 4365, 4449, 4531, 51689, 4365, 4449, 4532, 51690, 4365, 4449, 4533, 51691, 4365, 4449, 4534, 51692, 4365, 4449, 4535, 51693, 4365, 4449, 4536,
    51694, 4365, 4449, 12612, 51695, 4365, 4449, 4538, 51696, 4365, 4449, 4539, 51697, 4365, 4449, 4540, 51698, 4365, 4449, 4364, 51699, 4365, 4449, 4542,
    51700, 4365, 4449, 4543, 51701, 4365, 4449, 4544, 51702, 4365, 4449, 4545, 51703, 4365, 4449, 4546, 51704, 4365, 4450, 0, 51705, 4365, 4450, 4520,
    51706, 4365, 4450, 4521, 51707, 4365, 4450, 4522, 51708, 4365, 4450, 4523, 51709, 4365, 4450, 4524, 51710, 4365, 4450, 4525, 51711, 4365, 4450, 4526,
    51712, 4365, 4450, 4527, 51713, 4365, 4450, 4528, 51714, 4365, 4450, 4529, 51715, 4365, 4450, 4530, 51716, 4365, 4450, 4531, 51717, 4365, 4450, 4532,
    51718, 4365, 4450, 4533, 51719, 4365, 4450, 4534, 51720, 4365, 4450, 4535, 51721, 4365, 4450, 4536, 51722, 4365, 4450, 12612, 51723, 4365, 4450, 4538,
    51724, 4365, 4450, 4539, 51725, 4365, 4450, 4540, 51726, 4365, 4450, 4364, 51727, 4365, 4450, 4542, 51728, 4365, 4450, 4543, 51729, 4365, 4450, 4544,
    51730, 4365, 4450, 4545, 51731, 4365, 4450, 4546, 51732, 4365, 4451, 0, 51733, 4365, 4451, 4520, 51734, 4365, 4451, 4521, 51735, 4365, 4451, 4522,
    51736, 4365, 4451, 4523, 51737, 4365, 4451, 4524, 51738, 4365, 4451, 4525, 51739, 4365, 4451, 4526, 51740, 4365, 4451, 4527, 51741, 4365, 4451, 4528,
    51742, 4365, 4451, 4529, 51743, 4365, 4451, 4530, 51744, 4365, 4451, 4531, 51745, 4365, 4451, 4532, 51746, 4365, 4451, 4533, 51747, 4365, 4451, 4534,
    51748, 4365, 4451, 4535, 51749, 4365, 4451, 4536, 51750, 4365, 4451, 12612, 51751, 4365, 4451, 4538, 51752, 4365, 4451, 4539, 51753, 4365, 4451, 4540,
    51754, 4365, 4451, 4364, 51755, 4365, 4451, 4542, 51756, 4365, 4451, 4543, 51757, 4365, 4451, 4544, 51758, 4365, 4451, 4545, 51759, 4365, 4451, 4546,
    51760, 4365, 4452, 0, 51761, 4365, 4452, 4520, 51762, 4365, 4452, 4521, 51763, 4365, 4452, 4522, 51764, 4365, 4452, 4523, 51765, 4365, 4452, 4524,
    51766, 4365, 4452, 4525, 51767, 4365, 4452, 4526, 51768, 4365, 4452, 4527, 51769, 4365, 4452, 4528, 51770, 4365, 4452, 4529, 51771, 4365, 4452, 4530,
    51772, 4365, 4452, 4531, 51773, 4365, 4452, 4532, 51774, 4365, 4452, 4533, 51775, 4365, 4452, 4534, 51776, 4365, 4452, 4535, 51777, 4365, 4452, 4536,
    51778, 4365, 4452, 12612, 51779, 4365, 4452, 4538, 51780, 4365, 4452, 4539, 51781, 4365, 4452, 4540, 51782, 4365, 4452, 4364, 51783, 4365, 4452, 4542,
    51784, 4365, 4452, 4543, 51785, 4365, 4452, 4544, 51786, 4365, 4452, 4545, 51787, 4365, 4452, 4546, 51788, 4365, 4453, 0, 51789, 4365, 4453, 4520,
    51790, 4365, 4453, 4521, 51791, 4365, 4453, 4522, 51792, 4365, 4453, 4523, 51793, 4365, 4453, 4524, 51794, 4365, 4453, 4525, 51795, 4365, 4453, 4526,
    51796, 4365, 4453, 4527, 51797, 4365, 4453, 4528, 51798, 4365, 4453, 4529, 51799, 4365, 4453, 4530, 51800, 4365, 4453, 4531, 51801, 4365, 4453, 4532,
    51802, 4365, 4453, 4533, 51803, 4365, 4453, 4534, 51804, 4365, 4453, 4535, 51805, 4365, 4453, 4536, 51806, 4365, 4453, 12612, 51807, 4365, 4453, 4538,
    51808, 4365, 4453, 4539, 51809, 4365, 4453, 4540, 51810, 4365, 4453, 4364, 51811, 4365, 4453, 4542, 51812, 4365, 4453, 4543, 51813, 4365, 4453, 4544,
    51814, 4365, 4453, 4545, 51815, 4365, 4453, 4546, 51816, 4365, 4454, 0, 51817, 4365, 4454, 4520, 51818, 4365, 4454, 4521, 51819, 4365, 4454, 4522,
    51820, 4365, 4454, 4523, 51821, 4365, 4454, 4524, 51822, 4365, 4454, 4525, 51823, 4365, 4454, 4526, 51824, 4365, 4454, 4527, 51825, 4365, 4454, 4528,
    51826, 4365, 4454, 4529, 51827, 4365, 4454, 4530, 51828, 4365, 4454, 4531, 51829, 4365, 4454, 4532, 51830, 4365, 4454, 4533, 51831, 4365, 4454, 4534,
    51832, 4365, 4454, 4535, 51833, 4365, 4454, 4536, 51834, 4365, 4454, 12612, 51835, 4365, 4454, 4538, 51836, 4365, 4454, 4539, 51837, 4365, 4454, 4540,
    51838, 4365, 4454, 4364, 51839, 4365, 4454, 4542, 51840, 4365, 4454, 4543, 51841, 4365, 4454, 4544, 51842, 4365, 4454, 4545, 51843, 4365, 4454, 4546,
    51844, 4365, 4455, 0, 51845, 4365, 4455, 4520, 51846, 4365, 4455, 4521, 51847, 4365, 4455, 4522, 51848, 4365, 4455, 4523, 51849, 4365, 4455, 4524,
    51850, 4365, 4455, 4525, 51851, 4365, 4455, 4526, 51852, 4365, 4455, 4527, 51853, 4365, 4455, 4528, 51854, 4365, 4455, 4529, 51855, 4365, 4455, 4530,
    51856, 4365, 4455, 4531, 51857, 4365, 4455, 4532, 51858, 4365, 4455, 4533, 51859, 4365, 4455, 4534, 51860, 4365, 4455, 4535, 51861, 4365, 4455, 4536,
    51862, 4365, 4455, 12612, 51863, 4365, 4455, 4538, 51864, 4365, 4455, 4539, 51865, 4365, 4455, 4540, 51866, 4365, 4455, 4364, 51867, 4365, 4455, 4542,
    51868, 4365, 4455, 4543, 51869, 4365, 4455, 4544, 51870, 4365, 4455, 4545, 51871, 4365, 4455, 4546, 51872, 4365, 4456, 0, 51873, 4365, 4456, 4520,
    51874, 4365, 4456, 4521, 51875, 4365, 4456, 4522, 51876, 4365, 4456, 4523, 51877, 4365, 4456, 4524, 51878, 4365, 4456, 4525, 51879, 4365, 4456, 4526,
    51880, 4365, 4456, 4527, 51881, 4365, 4456, 4528, 51882, 4365, 4456, 4529, 51883, 4365, 4456, 4530, 51884, 4365, 4456, 4531, 51885, 4365, 4456, 4532,
    51886, 4365, 4456, 4533, 51887, 4365, 4456, 4534, 51888, 4365, 4456, 4535, 51889, 4365, 4456, 4536, 51890, 4365, 4456, 12612, 51891, 4365, 4456, 4538,
    51892, 4365, 4456, 4539, 51893, 4365, 4456, 4540, 51894, 4365, 4456, 4364, 51895, 4365, 4456, 4542, 51896, 4365, 4456, 4543, 51897, 4365, 4456, 4544,
    51898, 4365, 4456, 4545, 51899, 4365, 4456, 4546, 51900, 4365, 4457, 0, 51901, 4365, 4457, 4520, 51902, 4365, 4457, 4521, 51903, 4365, 4457, 4522,
    51904, 4365, 4457, 4523, 51905, 4365, 4457, 4524, 51906, 4365, 4457, 4525, 51907, 4365, 4457, 4526, 51908, 4365, 4457, 4527, 51909, 4365, 4457, 4528,
    51910, 4365, 4457, 4529, 51911, 4365, 4457, 4530, 51912, 4365, 4457, 4531, 51913, 4365, 4457, 4532, 51914, 4365, 4457, 4533, 51915, 4365, 4457, 4534,
    51916, 4365, 4457, 4535, 51917, 4365, 4457, 4536, 51918, 4365, 4457, 12612, 51919, 4365, 4457, 4538, 51920, 4365, 4457, 4539, 51921, 4365, 4457, 4540,
    51922, 4365, 4457, 4364, 51923, 4365, 4457, 4542, 51924, 4365, 4457, 4543, 51925, 4365, 4457, 4544, 51926, 4365, 4457, 4545, 51927, 4365, 4457, 4546,
    51928, 4365, 4458, 0, 51929, 4365, 4458, 4520, 51930, 4365, 4458, 4521, 51931, 4365, 4458, 4522, 51932, 4365, 4458, 4523, 51933, 4365, 4458, 4524,
    51934, 4365, 4458, 4525, 51935, 4365, 4458, 4526, 51936, 4365, 4458, 4527, 51937, 4365, 4458, 4528, 51938, 4365, 4458, 4529, 51939, 4365, 4458, 4530,
    51940, 4365, 4458, 4531, 51941, 4365, 4458, 4532, 51942, 4365, 4458, 4533, 51943, 4365, 4458, 4534, 51944, 4365, 4458, 4535, 51945, 4365, 4458, 4536,
    51946, 4365, 4458, 12612, 51947, 4365, 4458, 4538, 51948, 4365, 4458, 4539, 51949, 4365, 4458, 4540, 51950, 4365, 4458, 4364, 51951, 4365, 4458, 4542,
    51952, 4365, 4458, 4543, 51953, 4365, 4458, 4544, 51954, 4365, 4458, 4545, 51955, 4365, 4458, 4546, 51956, 4365, 4459, 0, 51957, 4365, 4459, 4520,
    51958, 4365, 4459, 4521, 51959, 4365, 4459, 4522, 51960, 4365, 4459, 4523, 51961, 4365, 4459, 4524, 51962, 4365, 4459, 4525, 51963, 4365, 4459, 4526,
    51964, 4365, 4459, 4527, 51965, 4365, 4459, 4528, 51966, 4365, 4459, 4529, 51967, 4365, 4459, 4530, 51968, 4365, 4459, 4531, 51969, 4365, 4459, 4532,
    51970, 4365, 4459, 4533, 51971, 4365, 4459, 4534, 51972, 4365, 4459, 4535, 51973, 4365, 4459, 4536, 51974, 4365, 4459, 12612, 51975, 4365, 4459, 4538,
    51976, 4365, 4459, 4539, 51977, 4365, 4459, 4540, 51978, 4365, 4459, 4364, 51979, 4365, 4459, 4542, 51980, 4365, 4459, 4543, 51981, 4365, 4459, 4544,
    51982, 4365, 4459, 4545, 51983, 4365, 4459, 4546, 51984, 4365, 4460, 0, 51985, 4365, 4460, 4520, 51986, 4365, 4460, 4521, 51987, 4365, 4460, 4522,
    51988, 4365, 4460, 4523, 51989, 4365, 4460, 4524, 51990, 4365, 4460, 4525, 51991, 4365, 4460, 4526, 51992, 4365, 4460, 4527, 51993, 4365, 4460, 4528,
    51994, 4365, 4460, 4529, 51995, 4365, 4460, 4530, 51996, 4365, 4460, 4531, 51997, 4365, 4460, 4532, 51998, 4365, 4460, 4533, 51999, 4365, 4460, 4534,
    52000, 4365, 4460, 4535, 52001, 4365, 4460, 4536, 52002, 4365, 4460, 12612, 52003, 4365, 4460, 4538, 52004, 4365, 4460, 4539, 52005, 4365, 4460, 4540,
    52006, 4365, 4460, 4364, 52007, 4365, 4460, 4542, 52008, 4365, 4460, 4543, 52009, 4365, 4460, 4544, 52010, 4365, 4460, 4545, 52011, 4365, 4460, 4546,
    52012, 4365, 4461, 0, 52013, 4365, 4461, 4520, 52014, 4365, 4461, 4521, 52015, 4365, 4461, 4522, 52016, 4365, 4461, 4523, 52017, 4365, 4461, 4524,
    52018, 4365, 4461, 4525, 52019, 4365, 4461, 4526, 52020, 4365, 4461, 4527, 52021, 4365, 4461, 4528, 52022, 4365, 4461, 4529, 52023, 4365, 4461, 4530,
    52024, 4365, 4461, 4531, 52025, 4365, 4461, 4532, 52026, 4365, 4461, 4533, 52027, 4365, 4461, 4534, 52028, 4365, 4461, 4535, 52029, 4365, 4461, 4536,
    52030, 4365, 4461, 12612, 52031, 4365, 4461, 4538, 52032, 4365, 4461, 4539, 52033, 4365, 4461, 4540, 52034, 4365, 4461, 4364, 52035, 4365, 4461, 4542,
    52036, 4365, 4461, 4543, 52037, 4365, 4461, 4544, 52038, 4365, 4461, 4545, 52039, 4365, 4461, 4546, 52040, 4365, 4462, 0, 52041, 4365, 4462, 4520,
    52042, 4365, 4462, 4521, 52043, 4365, 4462, 4522, 52044, 4365, 4462, 4523, 52045, 4365, 4462, 4524, 52046, 4365, 4462, 4525, 52047, 4365, 4462, 4526,
    52048, 4365, 4462, 4527, 52049, 4365, 4462, 4528, 52050, 4365, 4462, 4529, 52051, 4365, 4462, 4530, 52052, 4365, 4462, 4531, 52053, 4365, 4462, 4532,
    52054, 4365, 4462, 4533, 52055, 4365, 4462, 4534, 52056, 4365, 4462, 4535, 52057, 4365, 4462, 4536, 52058, 4365, 4462, 12612, 52059, 4365, 4462, 4538,
    52060, 4365, 4462, 4539, 52061, 4365, 4462, 4540, 52062, 4365, 4462, 4364, 52063, 4365, 4462, 4542, 52064, 4365, 4462, 4543, 52065, 4365, 4462, 4544,
    52066, 4365, 4462, 4545, 52067, 4365, 4462, 4546, 52068, 4365, 4463, 0, 52069, 4365, 4463, 4520, 52070, 4365, 4463, 4521, 52071, 4365, 4463, 4522,
    52072, 4365, 4463, 4523, 52073, 4365, 4463, 4524, 52074, 4365, 4463, 4525, 52075, 4365, 4463, 4526, 52076, 4365, 4463, 4527, 52077, 4365, 4463, 4528,
    52078, 4365, 4463, 4529, 52079, 4365, 4463, 4530, 52080, 4365, 4463, 4531, 52081, 4365, 4463, 4532, 52082, 4365, 4463, 4533, 52083, 4365, 4463, 4534,
    52084, 4365, 4463, 4535, 52085, 4365, 4463, 4536, 52086, 4365, 4463, 12612, 52087, 4365, 4463, 4538, 52088, 4365, 4463, 4539, 52089, 4365, 4463, 4540,
    52090, 4365, 4463, 4364, 52091, 4365, 4463, 4542, 52092, 4365, 4463, 4543, 52093, 4365, 4463, 4544, 52094, 4365, 4463, 4545, 52095, 4365, 4463, 4546,
    52096, 4365, 4464, 0, 52097, 4365, 4464, 4520, 52098, 4365, 4464, 4521, 52099, 4365, 4464, 4522, 52100, 4365, 4464, 4523, 52101, 4365, 4464, 4524,
    52102, 4365, 4464, 4525, 52103, 4365, 4464, 4526, 52104, 4365, 4464, 4527, 52105, 4365, 4464, 4528, 52106, 4365, 4464, 4529, 52107, 4365, 4464, 4530,
    52108, 4365, 4464, 4531, 52109, 4365, 4464, 4532, 52110, 4365, 4464, 4533, 52111, 4365, 4464, 4534, 52112, 4365, 4464, 4535, 52113, 4365, 4464, 4536,
    52114, 4365, 4464, 12612, 52115, 4365, 4464, 4538, 52116, 4365, 4464, 4539, 52117, 4365, 4464, 4540, 52118, 4365, 4464, 4364, 52119, 4365, 4464, 4542,
    52120, 4365, 4464, 4543, 52121, 4365, 4464, 4544, 52122, 4365, 4464, 4545, 52123, 4365, 4464, 4546, 52124, 4365, 4465, 0, 52125, 4365, 4465, 4520,
    52126, 4365, 4465, 4521, 52127, 4365, 4465, 4522, 52128, 4365, 4465, 4523, 52129, 4365, 4465, 4524, 52130, 4365, 4465, 4525, 52131, 4365, 4465, 4526,
    52132, 4365, 4465, 4527, 52133, 4365, 4465, 4528, 52134, 4365, 4465, 4529, 52135, 4365, 4465, 4530, 52136, 4365, 4465, 4531, 52137, 4365, 4465, 4532,
    52138, 4365, 4465, 4533, 52139, 4365, 4465, 4534, 52140, 4365, 4465, 4535, 52141, 4365, 4465, 4536, 52142, 4365, 4465, 12612, 52143, 4365, 4465, 4538,
    52144, 4365, 4465, 4539, 52145, 4365, 4465, 4540, 52146, 4365, 4465, 4364, 52147, 4365, 4465, 4542, 52148, 4365, 4465, 4543, 52149, 4365, 4465, 4544,
    52150, 4365, 4465, 4545, 52151, 4365, 4465, 4546, 52152, 4365, 4466, 0, 52153, 4365, 4466, 4520, 52154, 4365, 4466, 4521, 52155, 4365, 4466, 4522,
    52156, 4365, 4466, 4523, 52157, 4365, 4466, 4524, 52158, 4365, 4466, 4525, 52159, 4365, 4466, 4526, 52160, 4365, 4466, 4527, 52161, 4365, 4466, 4528,
    52162, 4365, 4466, 4529, 52163, 4365, 4466, 4530, 52164, 4365, 4466, 4531, 52165, 4365, 4466, 4532, 52166, 4365, 4466, 4533, 52167, 4365, 4466, 4534,
    52168, 4365, 4466, 4535, 52169, 4365, 4466, 4536, 52170, 4365, 4466, 12612, 52171, 4365, 4466, 4538, 52172, 4365, 4466, 4539, 52173, 4365, 4466, 4540,
    52174, 4365, 4466, 4364, 52175, 4365, 4466, 4542, 52176, 4365, 4466, 4543, 52177, 4365, 4466, 4544, 52178, 4365, 4466, 4545, 52179, 4365, 4466, 4546,
    52180, 4365, 4467, 0, 52181, 4365, 4467, 4520, 52182, 4365, 4467, 4521, 52183, 4365, 4467, 4522, 52184, 4365, 4467, 4523, 52185, 4365, 4467, 4524,
    52186, 4365, 4467, 4525, 52187, 4365, 4467, 4526, 52188, 4365, 4467, 4527, 52189, 4365, 4467, 4528, 52190, 4365, 4467, 4529, 52191, 4365, 4467, 4530,
    52192, 4365, 4467, 4531, 52193, 4365, 4467, 4532, 52194, 4365, 4467, 4533, 52195, 4365, 4467, 4534, 52196, 4365, 4467, 4535, 52197, 4365, 4467, 4536,
    52198, 4365, 4467, 12612, 52199, 4365, 4467, 4538, 52200, 4365, 4467, 4539, 52201, 4365, 4467, 4540, 52202, 4365, 4467, 4364, 52203, 4365, 4467, 4542,
    52204, 4365, 4467, 4543, 52205, 4365, 4467, 4544, 52206, 4365, 4467, 4545, 52207, 4365, 4467, 4546, 52208, 4365, 4468, 0, 52209, 4365, 4468, 4520,
    52210, 4365, 4468, 4521, 52211, 4365, 4468, 4522, 52212, 4365, 4468, 4523, 52213, 4365, 4468, 4524, 52214, 4365, 4468, 4525, 52215, 4365, 4468, 4526,
    52216, 4365, 4468, 4527, 52217, 4365, 4468, 4528, 52218, 4365, 4468, 4529, 52219, 4365, 4468, 4530, 52220, 4365, 4468, 4531, 52221, 4365, 4468, 4532,
    52222, 4365, 4468, 4533, 52223, 4365, 4468, 4534, 52224, 4365, 4468, 4535, 52225, 4365, 4468, 4536, 52226, 4365, 4468, 12612, 52227, 4365, 4468, 4538,
    52228, 4365, 4468, 4539, 52229, 4365, 4468, 4540, 52230, 4365, 4468, 4364, 52231, 4365, 4468, 4542, 52232, 4365, 4468, 4543, 52233, 4365, 4468, 4544,
    52234, 4365, 4468, 4545, 52235, 4365, 4468, 4546, 52236, 4365, 4469, 0, 52237, 4365, 4469, 4520, 52238, 4365, 4469, 4521, 52239, 4365, 4469, 4522,
    52240, 4365, 4469, 4523, 52241, 4365, 4469, 4524, 52242, 4365, 4469, 4525, 52243, 4365, 4469, 4526, 52244, 4365, 4469, 4527, 52245, 4365, 4469, 4528,
    52246, 4365, 4469, 4529, 52247, 4365, 4469, 4530, 52248, 4365, 4469, 4531, 52249, 4365, 4469, 4532, 52250, 4365, 4469, 4533, 52251, 4365, 4469, 4534,
    52252, 4365, 4469, 4535, 52253, 4365, 4469, 4536, 52254, 4365, 4469, 12612, 52255, 4365, 4469, 4538, 52256, 4365, 4469, 4539, 52257, 4365, 4469, 4540,
    52258, 4365, 4469, 4364, 52259, 4365, 4469, 4542, 52260, 4365, 4469, 4543, 52261, 4365, 4469, 4544, 52262, 4365, 4469, 4545, 52263, 4365, 4469, 4546,
    52264, 4366, 4449, 0, 52265, 4366, 4449, 4520, 52266, 4366, 4449, 4521, 52267, 4366, 4449, 4522, 52268, 4366, 4449, 4523, 52269, 4366, 4449, 4524,
    52270, 4366, 4449, 4525, 52271, 4366, 4449, 4526, 52272, 4366, 4449, 4527, 52273, 4366, 4449, 4528, 52274, 4366, 4449, 4529, 52275, 4366, 4449, 4530,
    52276, 4366, 4449, 4531, 52277, 4366, 4449, 4532, 52278, 4366, 4449, 4533, 52279, 4366, 4449, 4534, 52280, 4366, 4449, 4535, 52281, 4366, 4449, 4536,
    52282, 4366, 4449, 12612, 52283, 4366, 4449, 4538, 52284, 4366, 4449, 4539, 52285, 4366, 4449, 4540, 52286, 4366, 4449, 4364, 52287, 4366, 4449, 4542,
    52288, 4366, 4449, 4543, 52289, 4366, 4449, 4544, 52290, 4366, 4449, 4545, 52291, 4366, 4449, 4546, 52292, 4366, 4450, 0, 52293, 4366, 4450, 4520,
    52294, 4366, 4450, 4521, 52295, 4366, 4450, 4522, 52296, 4366, 4450, 4523, 52297, 4366, 4450, 4524, 52298, 4366, 4450, 4525, 52299, 4366, 4450, 4526,
    52300, 4366, 4450, 4527, 52301, 4366, 4450, 4528, 52302, 4366, 4450, 4529, 52303, 4366, 4450, 4530, 52304, 4366, 4450, 4531, 52305, 4366, 4450, 4532,
    52306, 4366, 4450, 4533, 52307, 4366, 4450, 4534, 52308, 4366, 4450, 4535, 52309, 4366, 4450, 4536, 52310, 4366, 4450, 12612, 52311, 4366, 4450, 4538,
    52312, 4366, 4450, 4539, 52313, 4366, 4450, 4540, 52314, 4366, 4450, 4364, 52315, 4366, 4450, 4542, 52316, 4366, 4450, 4543, 52317, 4366, 4450, 4544,
    52318, 4366, 4450, 4545, 52319, 4366, 4450, 4546, 52320, 4366, 4451, 0, 52321, 4366, 4451, 4520, 52322, 4366, 4451, 4521, 52323, 4366, 4451, 4522,
    52324, 4366, 4451, 4523, 52325, 4366, 4451, 4524, 52326, 4366, 4451, 4525, 52327, 4366, 4451, 4526, 52328, 4366, 4451, 4527, 52329, 4366, 4451, 4528,
    52330, 4366, 4451, 4529, 52331, 4366, 4451, 4530, 52332, 4366, 4451, 4531, 52333, 4366, 4451, 4532, 52334, 4366, 4451, 4533, 52335, 4366, 4451, 4534,
    52336, 4366, 4451, 4535, 52337, 4366, 4451, 4536, 52338, 4366, 4451, 12612, 52339, 4366, 4451, 4538, 52340, 4366, 4451, 4539, 52341, 4366, 4451, 4540,
    52342, 4366, 4451, 4364, 52343, 4366, 4451, 4542, 52344, 4366, 4451, 4543, 52345, 4366, 4451, 4544, 52346, 4366, 4451, 4545, 52347, 4366, 4451, 4546,
    52348, 4366, 4452, 0, 52349, 4366, 4452, 4520, 52350, 4366, 4452, 4521, 52351, 4366, 4452, 4522, 52352, 4366, 4452, 4523, 52353, 4366, 4452, 4524,
    52354, 4366, 4452, 4525, 52355, 4366, 4452, 4526, 52356, 4366, 4452, 4527, 52357, 4366, 4452, 4528, 52358, 4366, 4452, 4529, 52359, 4366, 4452, 4530,
    52360, 4366, 4452, 4531, 52361, 4366, 4452, 4532, 52362, 4366, 4452, 4533, 52363, 4366, 4452, 4534, 52364, 4366, 4452, 4535, 52365, 4366, 4452, 4536,
    52366, 4366, 4452, 12612, 52367, 4366, 4452, 4538, 52368, 4366, 4452, 4539, 52369, 4366, 4452, 4540, 52370, 4366, 4452, 4364, 52371, 4366, 4452, 4542,
    52372, 4366, 4452, 4543, 52373, 4366, 4452, 4544, 52374, 4366, 4452, 4545, 52375, 4366, 4452, 4546, 52376, 4366, 4453, 0, 52377, 4366, 4453, 4520,
    52378, 4366, 4453, 4521, 52379, 4366, 4453, 4522, 52380, 4366, 4453, 4523, 52381, 4366, 4453, 4524, 52382, 4366, 4453, 4525, 52383, 4366, 4453, 4526,
    52384, 4366, 4453, 4527, 52385, 4366, 4453, 4528, 52386, 4366, 4453, 4529, 52387, 4366, 4453, 4530, 52388, 4366, 4453, 4531, 52389, 4366, 4453, 4532,
    52390, 4366, 4453, 4533, 52391, 4366, 4453, 4534, 52392, 4366, 4453, 4535, 52393, 4366, 4453, 4536, 52394, 4366, 4453, 12612, 52395, 4366, 4453, 4538,
    52396, 4366, 4453, 4539, 52397, 4366, 4453, 4540, 52398, 4366, 4453, 4364, 52399, 4366, 4453, 4542, 52400, 4366, 4453, 4543, 52401, 4366, 4453, 4544,
    52402, 4366, 4453, 4545, 52403, 4366, 4453, 4546, 52404, 4366, 4454, 0, 52405, 4366, 4454, 4520, 52406, 4366, 4454, 4521, 52407, 4366, 4454, 4522,
    52408, 4366, 4454, 4523, 52409, 4366, 4454, 4524, 52410, 4366, 4454, 4525, 52411, 4366, 4454, 4526, 52412, 4366, 4454, 4527, 52413, 4366, 4454, 4528,
    52414, 4366, 4454, 4529, 52415, 4366, 4454, 4530, 52416, 4366, 4454, 4531, 52417, 4366, 4454, 4532, 52418, 4366, 4454, 4533, 52419, 4366, 4454, 4534,
    52420, 4366, 4454, 4535, 52421, 4366, 4454, 4536, 52422, 4366, 4454, 12612, 52423, 4366, 4454, 4538, 52424, 4366, 4454, 4539, 52425, 4366, 4454, 4540,
    52426, 4366, 4454, 4364, 52427, 4366, 4454, 4542, 52428, 4366, 4454, 4543, 52429, 4366, 4454, 4544, 52430, 4366, 4454, 4545, 52431, 4366, 4454, 4546,
    52432, 4366, 4455, 0, 52433, 4366, 4455, 4520, 52434, 4366, 4455, 4521, 52435, 4366, 4455, 4522, 52436, 4366, 4455, 4523, 52437, 4366, 4455, 4524,
    52438, 4366, 4455, 4525, 52439, 4366, 4455, 4526, 52440, 4366, 4455, 4527, 52441, 4366, 4455, 4528, 52442, 4366, 4455, 4529, 52443, 4366, 4455, 4530,
    52444, 4366, 4455, 4531, 52445, 4366, 4455, 4532, 52446, 4366, 4455, 4533, 52447, 4366, 4455, 4534, 52448, 4366, 4455, 4535, 52449, 4366, 4455, 4536,
    52450, 4366, 4455, 12612, 52451, 4366, 4455, 4538, 52452, 4366, 4455, 4539, 52453, 4366, 4455, 4540, 52454, 4366, 4455, 4364, 52455, 4366, 4455, 4542,
    52456, 4366, 4455, 4543, 52457, 4366, 4455, 4544, 52458, 4366, 4455, 4545, 52459, 4366, 4455, 4546, 52460, 4366, 4456, 0, 52461, 4366, 4456, 4520,
    52462, 4366, 4456, 4521, 52463, 4366, 4456, 4522, 52464, 4366, 4456, 4523, 52465, 4366, 4456, 4524, 52466, 4366, 4456, 4525, 52467, 4366, 4456, 4526,
    52468, 4366, 4456, 4527, 52469, 4366, 4456, 4528, 52470, 4366, 4456, 4529, 52471, 4366, 4456, 4530, 52472, 4366, 4456, 4531, 52473, 4366, 4456, 4532,
    52474, 4366, 4456, 4533, 52475, 4366, 4456, 4534, 52476, 4366, 4456, 4535, 52477, 4366, 4456, 4536, 52478, 4366, 4456, 12612, 52479, 4366, 4456, 4538,
    52480, 4366, 4456, 4539, 52481, 4366, 4456, 4540, 52482, 4366, 4456, 4364, 52483, 4366, 4456, 4542, 52484, 4366, 4456, 4543, 52485, 4366, 4456, 4544,
    52486, 4366, 4456, 4545, 52487, 4366, 4456, 4546, 52488, 4366, 4457, 0, 52489, 4366, 4457, 4520, 52490, 4366, 4457, 4521, 52491, 4366, 4457, 4522,
    52492, 4366, 4457, 4523, 52493, 4366, 4457, 4524, 52494, 4366, 4457, 4525, 52495, 4366, 4457, 4526, 52496, 4366, 4457, 4527, 52497, 4366, 4457, 4528,
    52498, 4366, 4457, 4529, 52499, 4366, 4457, 4530, 52500, 4366, 4457, 4531, 52501, 4366, 4457, 4532, 52502, 4366, 4457, 4533, 52503, 4366, 4457, 4534,
    52504, 4366, 4457, 4535, 52505, 4366, 4457, 4536, 52506, 4366, 4457, 12612, 52507, 4366, 4457, 4538, 52508, 4366, 4457, 4539, 52509, 4366, 4457, 4540,
    52510, 4366, 4457, 4364, 52511, 4366, 4457, 4542, 52512, 4366, 4457, 4543, 52513, 4366, 4457, 4544, 52514, 4366, 4457, 4545, 52515, 4366, 4457, 4546,
    52516, 4366, 4458, 0, 52517, 4366, 4458, 4520, 52518, 4366, 4458, 4521, 52519, 4366, 4458, 4522, 52520, 4366, 4458, 4523, 52521, 4366, 4458, 4524,
    52522, 4366, 4458, 4525, 52523, 4366, 4458, 4526, 52524, 4366, 4458, 4527, 52525, 4366, 4458, 4528, 52526, 4366, 4458, 4529, 52527, 4366, 4458, 4530,
    52528, 4366, 4458, 4531, 52529, 4366, 4458, 4532, 52530, 4366, 4458, 4533, 52531, 4366, 4458, 4534, 52532, 4366, 4458, 4535, 52533, 4366, 4458, 4536,
    52534, 4366, 4458, 12612, 52535, 4366, 4458, 4538, 52536, 4366, 4458, 4539, 52537, 4366, 4458, 4540, 52538, 4366, 4458, 4364, 52539, 4366, 4458, 4542,
    52540, 4366, 4458, 4543, 52541, 4366, 4458, 4544, 52542, 4366, 4458, 4545, 52543, 4366, 4458, 4546, 52544, 4366, 4459, 0, 52545, 4366, 4459, 4520,
    52546, 4366, 4459, 4521, 52547, 4366, 4459, 4522, 52548, 4366, 4459, 4523, 52549, 4366, 4459, 4524, 52550, 4366, 4459, 4525, 52551, 4366, 4459, 4526,
    52552, 4366, 4459, 4527, 52553, 4366, 4459, 4528, 52554, 4366, 4459, 4529, 52555, 4366, 4459, 4530, 52556, 4366, 4459, 4531, 52557, 4366, 4459, 4532,
    52558, 4366, 4459, 4533, 52559, 4366, 4459, 4534, 52560, 4366, 4459, 4535, 52561, 4366, 4459, 4536, 52562, 4366, 4459, 12612, 52563, 4366, 4459, 4538,
    52564, 4366, 4459, 4539, 52565, 4366, 4459, 4540, 52566, 4366, 4459, 4364, 52567, 4366, 4459, 4542, 52568, 4366, 4459, 4543, 52569, 4366, 4459, 4544,
    52570, 4366, 4459, 4545, 52571, 4366, 4459, 4546, 52572, 4366, 4460, 0, 52573, 4366, 4460, 4520, 52574, 4366, 4460, 4521, 52575, 4366, 4460, 4522,
    52576, 4366, 4460, 4523, 52577, 4366, 4460, 4524, 52578, 4366, 4460, 4525, 52579, 4366, 4460, 4526, 52580, 4366, 4460, 4527, 52581, 4366, 4460, 4528,
    52582, 4366, 4460, 4529, 52583, 4366, 4460, 4530, 52584, 4366, 4460, 4531, 52585, 4366, 4460, 4532, 52586, 4366, 4460, 4533, 52587, 4366, 4460, 4534,
    52588, 4366, 4460, 4535, 52589, 4366, 4460, 4536, 52590, 4366, 4460, 12612, 52591, 4366, 4460, 4538, 52592, 4366, 4460, 4539, 52593, 4366, 4460, 4540,
    52594, 4366, 4460, 4364, 52595, 4366, 4460, 4542, 52596, 4366, 4460, 4543, 52597, 4366, 4460, 4544, 52598, 4366, 4460, 4545, 52599, 4366, 4460, 4546,
    52600, 4366, 4461, 0, 52601, 4366, 4461, 4520, 52602, 4366, 4461, 4521, 52603, 4366, 4461, 4522, 52604, 4366, 4461, 4523, 52605, 4366, 4461, 4524,
    52606, 4366, 4461, 4525, 52607, 4366, 4461, 4526, 52608, 4366, 4461, 4527, 52609, 4366, 4461, 4528, 52610, 4366, 4461, 4529, 52611, 4366, 4461, 4530,
    52612, 4366, 4461, 4531, 52613, 4366, 4461, 4532, 52614, 4366, 4461, 4533, 52615, 4366, 4461, 4534, 52616, 4366, 4461, 4535, 52617, 4366, 4461, 4536,
    52618, 4366, 4461, 12612, 52619, 4366, 4461, 4538, 52620, 4366, 4461, 4539, 52621, 4366, 4461, 4540, 52622, 4366, 4461, 4364, 52623, 4366, 4461, 4542,
    52624, 4366, 4461, 4543, 52625, 4366, 4461, 4544, 52626, 4366, 4461, 4545, 52627, 4366, 4461, 4546, 52628, 4366, 4462, 0, 52629, 4366, 4462, 4520,
    52630, 4366, 4462, 4521, 52631, 4366, 4462, 4522, 52632, 4366, 4462, 4523, 52633, 4366, 4462, 4524, 52634, 4366, 4462, 4525, 52635, 4366, 4462, 4526,
    52636, 4366, 4462, 4527, 52637, 4366, 4462, 4528, 52638, 4366, 4462, 4529, 52639, 4366, 4462, 4530, 52640, 4366, 4462, 4531, 52641, 4366, 4462, 4532,
    52642, 4366, 4462, 4533, 52643, 4366, 4462, 4534, 52644, 4366, 4462, 4535, 52645, 4366, 4462, 4536, 52646, 4366, 4462, 12612, 52647, 4366, 4462, 4538,
    52648, 4366, 4462, 4539, 52649, 4366, 4462, 4540, 52650, 4366, 4462, 4364, 52651, 4366, 4462, 4542, 52652, 4366, 4462, 4543, 52653, 4366, 4462, 4544,
    52654, 4366, 4462, 4545, 52655, 4366, 4462, 4546, 52656, 4366, 4463, 0, 52657, 4366, 4463, 4520, 52658, 4366, 4463, 4521, 52659, 4366, 4463, 4522,
    52660, 4366, 4463, 4523, 52661, 4366, 4463, 4524, 52662, 4366, 4463, 4525, 52663, 4366, 4463, 4526, 52664, 4366, 4463, 4527, 52665, 4366, 4463, 4528,
    52666, 4366, 4463, 4529, 52667, 4366, 4463, 4530, 52668, 4366, 4463, 4531, 52669, 4366, 4463, 4532, 52670, 4366, 4463, 4533, 52671, 4366, 4463, 4534,
    52672, 4366, 4463, 4535, 52673, 4366, 4463, 4536, 52674, 4366, 4463, 12612, 52675, 4366, 4463, 4538, 52676, 4366, 4463, 4539, 52677, 4366, 4463, 4540,
    52678, 4366, 4463, 4364, 52679, 4366, 4463, 4542, 52680, 4366, 4463, 4543, 52681, 4366, 4463, 4544, 52682, 4366, 4463, 4545, 52683, 4366, 4463, 4546,
    52684, 4366, 4464, 0, 52685, 4366, 4464, 4520, 52686, 4366, 4464, 4521, 52687, 4366, 4464, 4522, 52688, 4366, 4464, 4523, 52689, 4366, 4464, 4524,
    52690, 4366, 4464, 4525, 52691, 4366, 4464, 4526, 52692, 4366, 4464, 4527, 52693, 4366, 4464, 4528, 52694, 4366, 4464, 4529, 52695, 4366, 4464, 4530,
    52696, 4366, 4464, 4531, 52697, 4366, 4464, 4532, 52698, 4366, 4464, 4533, 52699, 4366, 4464, 4534, 52700, 4366, 4464, 4535, 52701, 4366, 4464, 4536,
    52702, 4366, 4464, 12612, 52703, 4366, 4464, 4538, 52704, 4366, 4464, 4539, 52705, 4366, 4464, 4540, 52706, 4366, 4464, 4364, 52707, 4366, 4464, 4542,
    52708, 4366, 4464, 4543, 52709, 4366, 4464, 4544, 52710, 4366, 4464, 4545, 52711, 4366, 4464, 4546, 52712, 4366, 4465, 0, 52713, 4366, 4465, 4520,
    52714, 4366, 4465, 4521, 52715, 4366, 4465, 4522, 52716, 4366, 4465, 4523, 52717, 4366, 4465, 4524, 52718, 4366, 4465, 4525, 52719, 4366, 4465, 4526,
    52720, 4366, 4465, 4527, 52721, 4366, 4465, 4528, 52722, 4366, 4465, 4529, 52723, 4366, 4465, 4530, 52724, 4366, 4465, 4531, 52725, 4366, 4465, 4532,
    52726, 4366, 4465, 4533, 52727, 4366, 4465, 4534, 52728, 4366, 4465, 4535, 52729, 4366, 4465, 4536, 52730, 4366, 4465, 12612, 52731, 4366, 4465, 4538,
    52732, 4366, 4465, 4539, 52733, 4366, 4465, 4540, 52734, 4366, 4465, 4364, 52735, 4366, 4465, 4542, 52736, 4366, 4465, 4543, 52737, 4366, 4465, 4544,
    52738, 4366, 4465, 4545, 52739, 4366, 4465, 4546, 52740, 4366, 4466, 0, 52741, 4366, 4466, 4520, 52742, 4366, 4466, 4521, 52743, 4366, 4466, 4522,
    52744, 4366, 4466, 4523, 52745, 4366, 4466, 4524, 52746, 4366, 4466, 4525, 52747, 4366, 4466, 4526, 52748, 4366, 4466, 4527, 52749, 4366, 4466, 4528,
    52750, 4366, 4466, 4529, 52751, 4366, 4466, 4530, 52752, 4366, 4466, 4531, 52753, 4366, 4466, 4532, 52754, 4366, 4466, 4533, 52755, 4366, 4466, 4534,
    52756, 4366, 4466, 4535, 52757, 4366, 4466, 4536, 52758, 4366, 4466, 12612, 52759, 4366, 4466, 4538, 52760, 4366, 4466, 4539, 52761, 4366, 4466, 4540,
    52762, 4366, 4466, 4364, 52763, 4366, 4466, 4542, 52764, 4366, 4466, 4543, 52765, 4366, 4466, 4544, 52766, 4366, 4466, 4545, 52767, 4366, 4466, 4546,
    52768, 4366, 4467, 0, 52769, 4366, 4467, 4520, 52770, 4366, 4467, 4521, 52771, 4366, 4467, 4522, 52772, 4366, 4467, 4523, 52773, 4366, 4467, 4524,
    52774, 4366, 4467, 4525, 52775, 4366, 4467, 4526, 52776, 4366, 4467, 4527, 52777, 4366, 4467, 4528, 52778, 4366, 4467, 4529, 52779, 4366, 4467, 4530,
    52780, 4366, 4467, 4531, 52781, 4366, 4467, 4532, 52782, 4366, 4467, 4533, 52783, 4366, 4467, 4534, 52784, 4366, 4467, 4535, 52785, 4366, 4467, 4536,
    52786, 4366, 4467, 12612, 52787, 4366, 4467, 4538, 52788, 4366, 4467, 4539, 52789, 4366, 4467, 4540, 52790, 4366, 4467, 4364, 52791, 4366, 4467, 4542,
    52792, 4366, 4467, 4543, 52793, 4366, 4467, 4544, 52794, 4366, 4467, 4545, 52795, 4366, 4467, 4546, 52796, 4366, 4468, 0, 52797, 4366, 4468, 4520,
    52798, 4366, 4468, 4521, 52799, 4366, 4468, 4522, 52800, 4366, 4468, 4523, 52801, 4366, 4468, 4524, 52802, 4366, 4468, 4525, 52803, 4366, 4468, 4526,
    52804, 4366, 4468, 4527, 52805, 4366, 4468, 4528, 52806, 4366, 4468, 4529, 52807, 4366, 4468, 4530, 52808, 4366, 4468, 4531, 52809, 4366, 4468, 4532,
    52810, 4366, 4468, 4533, 52811, 4366, 4468, 4534, 52812, 4366, 4468, 4535, 52813, 4366, 4468, 4536, 52814, 4366, 4468, 12612, 52815, 4366, 4468, 4538,
    52816, 4366, 4468, 4539, 52817, 4366, 4468, 4540, 52818, 4366, 4468, 4364, 52819, 4366, 4468, 4542, 52820, 4366, 4468, 4543, 52821, 4366, 4468, 4544,
    52822, 4366, 4468, 4545, 52823, 4366, 4468, 4546, 52824, 4366, 4469, 0, 52825, 4366, 4469, 4520, 52826, 4366, 4469, 4521, 52827, 4366, 4469, 4522,
    52828, 4366, 4469, 4523, 52829, 4366, 4469, 4524, 52830, 4366, 4469, 4525, 52831, 4366, 4469, 4526, 52832, 4366, 4469, 4527, 52833, 4366, 4469, 4528,
    52834, 4366, 4469, 4529, 52835, 4366, 4469, 4530, 52836, 4366, 4469, 4531, 52837, 4366, 4469, 4532, 52838, 4366, 4469, 4533, 52839, 4366, 4469, 4534,
    52840, 4366, 4469, 4535, 52841, 4366, 4469, 4536, 52842, 4366, 4469, 12612, 52843, 4366, 4469, 4538, 52844, 4366, 4469, 4539, 52845, 4366, 4469, 4540,
    52846, 4366, 4469, 4364, 52847, 4366, 4469, 4542, 52848, 4366, 4469, 4543, 52849, 4366, 4469, 4544, 52850, 4366, 4469, 4545, 52851, 4366, 4469, 4546,
    52852, 4367, 4449, 0, 52853, 4367, 4449, 4520, 52854, 4367, 4449, 4521, 52855, 4367, 4449, 4522, 52856, 4367, 4449, 4523, 52857, 4367, 4449, 4524,
    52858, 4367, 4449, 4525, 52859, 4367, 4449, 4526, 52860, 4367, 4449, 4527, 52861, 4367, 4449, 4528, 52862, 4367, 4449, 4529, 52863, 4367, 4449, 4530,
    52864, 4367, 4449, 4531, 52865, 4367, 4449, 4532, 52866, 4367, 4449, 4533, 52867, 4367, 4449, 4534, 52868, 4367, 4449, 4535, 52869, 4367, 4449, 4536,
    52870, 4367, 4449, 12612, 52871, 4367, 4449, 4538, 52872, 4367, 4449, 4539, 52873, 4367, 4449, 4540, 52874, 4367, 4449, 4364, 52875, 4367, 4449, 4542,
    52876, 4367, 4449, 4543, 52877, 4367, 4449, 4544, 52878, 4367, 4449, 4545, 52879, 4367, 4449, 4546, 52880, 4367, 4450, 0, 52881, 4367, 4450, 4520,
    52882, 4367, 4450, 4521, 52883, 4367, 4450, 4522, 52884, 4367, 4450, 4523, 52885, 4367, 4450, 4524, 52886, 4367, 4450, 4525, 52887, 4367, 4450, 4526,
    52888, 4367, 4450, 4527, 52889, 4367, 4450, 4528, 52890, 4367, 4450, 4529, 52891, 4367, 4450, 4530, 52892, 4367, 4450, 4531, 52893, 4367, 4450, 4532,
    52894, 4367, 4450, 4533, 52895, 4367, 4450, 4534, 52896, 4367, 4450, 4535, 52897, 4367, 4450, 4536, 52898, 4367, 4450, 12612, 52899, 4367, 4450, 4538,
    52900, 4367, 4450, 4539, 52901, 4367, 4450, 4540, 52902, 4367, 4450, 4364, 52903, 4367, 4450, 4542, 52904, 4367, 4450, 4543, 52905, 4367, 4450, 4544,
    52906, 4367, 4450, 4545, 52907, 4367, 4450, 4546, 52908, 4367, 4451, 0, 52909, 4367, 4451, 4520, 52910, 4367, 4451, 4521, 52911, 4367, 4451, 4522,
    52912, 4367, 4451, 4523, 52913, 4367, 4451, 4524, 52914, 4367, 4451, 4525, 52915, 4367, 4451, 4526, 52916, 4367, 4451, 4527, 52917, 4367, 4451, 4528,
    52918, 4367, 4451, 4529, 52919, 4367, 4451, 4530, 52920, 4367, 4451, 4531, 52921, 4367, 4451, 4532, 52922, 4367, 4451, 4533, 52923, 4367, 4451, 4534,
    52924, 4367, 4451, 4535, 52925, 4367, 4451, 4536, 52926, 4367, 4451, 12612, 52927, 4367, 4451, 4538, 52928, 4367, 4451, 4539, 52929, 4367, 4451, 4540,
    52930, 4367, 4451, 4364, 52931, 4367, 4451, 4542, 52932, 4367, 4451, 4543, 52933, 4367, 4451, 4544, 52934, 4367, 4451, 4545, 52935, 4367, 4451, 4546,
    52936, 4367, 4452, 0, 52937, 4367, 4452, 4520, 52938, 4367, 4452, 4521, 52939, 4367, 4452, 4522, 52940, 4367, 4452, 4523, 52941, 4367, 4452, 4524,
    52942, 4367, 4452, 4525, 52943, 4367, 4452, 4526, 52944, 4367, 4452, 4527, 52945, 4367, 4452, 4528, 52946, 4367, 4452, 4529, 52947, 4367, 4452, 4530,
    52948, 4367, 4452, 4531, 52949, 4367, 4452, 4532, 52950, 4367, 4452, 4533, 52951, 4367, 4452, 4534, 52952, 4367, 4452, 4535, 52953, 4367, 4452, 4536,
    52954, 4367, 4452, 12612, 52955, 4367, 4452, 4538, 52956, 4367, 4452, 4539, 52957, 4367, 4452, 4540, 52958, 4367, 4452, 4364, 52959, 4367, 4452, 4542,
    52960, 4367, 4452, 4543, 52961, 4367, 4452, 4544, 52962, 4367, 4452, 4545, 52963, 4367, 4452, 4546, 52964, 4367, 4453, 0, 52965, 4367, 4453, 4520,
    52966, 4367, 4453, 4521, 52967, 4367, 4453, 4522, 52968, 4367, 4453, 4523, 52969, 4367, 4453, 4524, 52970, 4367, 4453, 4525, 52971, 4367, 4453, 4526,
    52972, 4367, 4453, 4527, 52973, 4367, 4453, 4528, 52974, 4367, 4453, 4529, 52975, 4367, 4453, 4530, 52976, 4367, 4453, 4531, 52977, 4367, 4453, 4532,
    52978, 4367, 4453, 4533, 52979, 4367, 4453, 4534, 52980, 4367, 4453, 4535, 52981, 4367, 4453, 4536, 52982, 4367, 4453, 12612, 52983, 4367, 4453, 4538,
    52984, 4367, 4453, 4539, 52985, 4367, 4453, 4540, 52986, 4367, 4453, 4364, 52987, 4367, 4453, 4542, 52988, 4367, 4453, 4543, 52989, 4367, 4453, 4544,
    52990, 4367, 4453, 4545, 52991, 4367, 4453, 4546, 52992, 4367, 4454, 0, 52993, 4367, 4454, 4520, 52994, 4367, 4454, 4521, 52995, 4367, 4454, 4522,
    52996, 4367, 4454, 4523, 52997, 4367, 4454, 4524, 52998, 4367, 4454, 4525, 52999, 4367, 4454, 4526, 53000, 4367, 4454, 4527, 53001, 4367, 4454, 4528,
    53002, 4367, 4454, 4529, 53003, 4367, 4454, 4530, 53004, 4367, 4454, 4531, 53005, 4367, 4454, 4532, 53006, 4367, 4454, 4533, 53007, 4367, 4454, 4534,
    53008, 4367, 4454, 4535, 53009, 4367, 4454, 4536, 53010, 4367, 4454, 12612, 53011, 4367, 4454, 4538, 53012, 4367, 4454, 4539, 53013, 4367, 4454, 4540,
    53014, 4367, 4454, 4364, 53015, 4367, 4454, 4542, 53016, 4367, 4454, 4543, 53017, 4367, 4454, 4544, 53018, 4367, 4454, 4545, 53019, 4367, 4454, 4546,
    53020, 4367, 4455, 0, 53021, 4367, 4455, 4520, 53022, 4367, 4455, 4521, 53023, 4367, 4455, 4522, 53024, 4367, 4455, 4523, 53025, 4367, 4455, 4524,
    53026, 4367, 4455, 4525, 53027, 4367, 4455, 4526, 53028, 4367, 4455, 4527, 53029, 4367, 4455, 4528, 53030, 4367, 4455, 4529, 53031, 4367, 4455, 4530,
    53032, 4367, 4455, 4531, 53033, 4367, 4455, 4532, 53034, 4367, 4455, 4533, 53035, 4367, 4455, 4534, 53036, 4367, 4455, 4535, 53037, 4367, 4455, 4536,
    53038, 4367, 4455, 12612, 53039, 4367, 4455, 4538, 53040, 4367, 4455, 4539, 53041, 4367, 4455, 4540, 53042, 4367, 4455, 4364, 53043, 4367, 4455, 4542,
    53044, 4367, 4455, 4543, 53045, 4367, 4455, 4544, 53046, 4367, 4455, 4545, 53047, 4367, 4455, 4546, 53048, 4367, 4456, 0, 53049, 4367, 4456, 4520,
    53050, 4367, 4456, 4521, 53051, 4367, 4456, 4522, 53052, 4367, 4456, 4523, 53053, 4367, 4456, 4524, 53054, 4367, 4456, 4525, 53055, 4367, 4456, 4526,
    53056, 4367, 4456, 4527, 53057, 4367, 4456, 4528, 53058, 4367, 4456, 4529, 53059, 4367, 4456, 4530, 53060, 4367, 4456, 4531, 53061, 4367, 4456, 4532,
    53062, 4367, 4456, 4533, 53063, 4367, 4456, 4534, 53064, 4367, 4456, 4535, 53065, 4367, 4456, 4536, 53066, 4367, 4456, 12612, 53067, 4367, 4456, 4538,
    53068, 4367, 4456, 4539, 53069, 4367, 4456, 4540, 53070, 4367, 4456, 4364, 53071, 4367, 4456, 4542, 53072, 4367, 4456, 4543, 53073, 4367, 4456, 4544,
    53074, 4367, 4456, 4545, 53075, 4367, 4456, 4546, 53076, 4367, 4457, 0, 53077, 4367, 4457, 4520, 53078, 4367, 4457, 4521, 53079, 4367, 4457, 4522,
    53080, 4367, 4457, 4523, 53081, 4367, 4457, 4524, 53082, 4367, 4457, 4525, 53083, 4367, 4457, 4526, 53084, 4367, 4457, 4527, 53085, 4367, 4457, 4528,
    53086, 4367, 4457, 4529, 53087, 4367, 4457, 4530, 53088, 4367, 4457, 4531, 53089, 4367, 4457, 4532, 53090, 4367, 4457, 4533, 53091, 4367, 4457, 4534,
    53092, 4367, 4457, 4535, 53093, 4367, 4457, 4536, 53094, 4367, 4457, 12612, 53095, 4367, 4457, 4538, 53096, 4367, 4457, 4539, 53097, 4367, 4457, 4540,
    53098, 4367, 4457, 4364, 53099, 4367, 4457, 4542, 53100, 4367, 4457, 4543, 53101, 4367, 4457, 4544, 53102, 4367, 4457, 4545, 53103, 4367, 4457, 4546,
    53104, 4367, 4458, 0, 53105, 4367, 4458, 4520, 53106, 4367, 4458, 4521, 53107, 4367, 4458, 4522, 53108, 4367, 4458, 4523, 53109, 4367, 4458, 4524,
    53110, 4367, 4458, 4525, 53111, 4367, 4458, 4526, 53112, 4367, 4458, 4527, 53113, 4367, 4458, 4528, 53114, 4367, 4458, 4529, 53115, 4367, 4458, 4530,
    53116, 4367, 4458, 4531, 53117, 4367, 4458, 4532, 53118, 4367, 4458, 4533, 53119, 4367, 4458, 4534, 53120, 4367, 4458, 4535, 53121, 4367, 4458, 4536,
    53122, 4367, 4458, 12612, 53123, 4367, 4458, 4538, 53124, 4367, 4458, 4539, 53125, 4367, 4458, 4540, 53126, 4367, 4458, 4364, 53127, 4367, 4458, 4542,
    53128, 4367, 4458, 4543, 53129, 4367, 4458, 4544, 53130, 4367, 4458, 4545, 53131, 4367, 4458, 4546, 53132, 4367, 4459, 0, 53133, 4367, 4459, 4520,
    53134, 4367, 4459, 4521, 53135, 4367, 4459, 4522, 53136, 4367, 4459, 4523, 53137, 4367, 4459, 4524, 53138, 4367, 4459, 4525, 53139, 4367, 4459, 4526,
    53140, 4367, 4459, 4527, 53141, 4367, 4459, 4528, 53142, 4367, 4459, 4529, 53143, 4367, 4459, 4530, 53144, 4367, 4459, 4531, 53145, 4367, 4459, 4532,
    53146, 4367, 4459, 4533, 53147, 4367, 4459, 4534, 53148, 4367, 4459, 4535, 53149, 4367, 4459, 4536, 53150, 4367, 4459, 12612, 53151, 4367, 4459, 4538,
    53152, 4367, 4459, 4539, 53153, 4367, 4459, 4540, 53154, 4367, 4459, 4364, 53155, 4367, 4459, 4542, 53156, 4367, 4459, 4543, 53157, 4367, 4459, 4544,
    53158, 4367, 4459, 4545, 53159, 4367, 4459, 4546, 53160, 4367, 4460, 0, 53161, 4367, 4460, 4520, 53162, 4367, 4460, 4521, 53163, 4367, 4460, 4522,
    53164, 4367, 4460, 4523, 53165, 4367, 4460, 4524, 53166, 4367, 4460, 4525, 53167, 4367, 4460, 4526, 53168, 4367, 4460, 4527, 53169, 4367, 4460, 4528,
    53170, 4367, 4460, 4529, 53171, 4367, 4460, 4530, 53172, 4367, 4460, 4531, 53173, 4367, 4460, 4532, 53174, 4367, 4460, 4533, 53175, 4367, 4460, 4534,
    53176, 4367, 4460, 4535, 53177, 4367, 4460, 4536, 53178, 4367, 4460, 12612, 53179, 4367, 4460, 4538, 53180, 4367, 4460, 4539, 53181, 4367, 4460, 4540,
    53182, 4367, 4460, 4364, 53183, 4367, 4460, 4542, 53184, 4367, 4460, 4543, 53185, 4367, 4460, 4544, 53186, 4367, 4460, 4545, 53187, 4367, 4460, 4546,
    53188, 4367, 4461, 0, 53189, 4367, 4461, 4520, 53190, 4367, 4461, 4521, 53191, 4367, 4461, 4522, 53192, 4367, 4461, 4523, 53193, 4367, 4461, 4524,
    53194, 4367, 4461, 4525, 53195, 4367, 4461, 4526, 53196, 4367, 4461, 4527, 53197, 4367, 4461, 4528, 53198, 4367, 4461, 4529, 53199, 4367, 4461, 4530,
    53200, 4367, 4461, 4531, 53201, 4367, 4461, 4532, 53202, 4367, 4461, 4533, 53203, 4367, 4461, 4534, 53204, 4367, 4461, 4535, 53205, 4367, 4461, 4536,
    53206, 4367, 4461, 12612, 53207, 4367, 4461, 4538, 53208, 4367, 4461, 4539, 53209, 4367, 4461, 4540, 53210, 4367, 4461, 4364, 53211, 4367, 4461, 4542,
    53212, 4367, 4461, 4543, 53213, 4367, 4461, 4544, 53214, 4367, 4461, 4545, 53215, 4367, 4461, 4546, 53216, 4367, 4462, 0, 53217, 4367, 4462, 4520,
    53218, 4367, 4462, 4521, 53219, 4367, 4462, 4522, 53220, 4367, 4462, 4523, 53221, 4367, 4462, 4524, 53222, 4367, 4462, 4525, 53223, 4367, 4462, 4526,
    53224, 4367, 4462, 4527, 53225, 4367, 4462, 4528, 53226, 4367, 4462, 4529, 53227, 4367, 4462, 4530, 53228, 4367, 4462, 4531, 53229, 4367, 4462, 4532,
    53230, 4367, 4462, 4533, 53231, 4367, 4462, 4534, 53232, 4367, 4462, 4535, 53233, 4367, 4462, 4536, 53234, 4367, 4462, 12612, 53235, 4367, 4462, 4538,
    53236, 4367, 4462, 4539, 53237, 4367, 4462, 4540, 53238, 4367, 4462, 4364, 53239, 4367, 4462, 4542, 53240, 4367, 4462, 4543, 53241, 4367, 4462, 4544,
    53242, 4367, 4462, 4545, 53243, 4367, 4462, 4546, 53244, 4367, 4463, 0, 53245, 4367, 4463, 4520, 53246, 4367, 4463, 4521, 53247, 4367, 4463, 4522,
    53248, 4367, 4463, 4523, 53249, 4367, 4463, 4524, 53250, 4367, 4463, 4525, 53251, 4367, 4463, 4526, 53252, 4367, 4463, 4527, 53253, 4367, 4463, 4528,
    53254, 4367, 4463, 4529, 53255, 4367, 4463, 4530, 53256, 4367, 4463, 4531, 53257, 4367, 4463, 4532, 53258, 4367, 4463, 4533, 53259, 4367, 4463, 4534,
    53260, 4367, 4463, 4535, 53261, 4367, 4463, 4536, 53262, 4367, 4463, 12612, 53263, 4367, 4463, 4538, 53264, 4367, 4463, 4539, 53265, 4367, 4463, 4540,
    53266, 4367, 4463, 4364, 53267, 4367, 4463, 4542, 53268, 4367, 4463, 4543, 53269, 4367, 4463, 4544, 53270, 4367, 4463, 4545, 53271, 4367, 4463, 4546,
    53272, 4367, 4464, 0, 53273, 4367, 4464, 4520, 53274, 4367, 4464, 4521, 53275, 4367, 4464, 4522, 53276, 4367, 4464, 4523, 53277, 4367, 4464, 4524,
    53278, 4367, 4464, 4525, 53279, 4367, 4464, 4526, 53280, 4367, 4464, 4527, 53281, 4367, 4464, 4528, 53282, 4367, 4464, 4529, 53283, 4367, 4464, 4530,
    53284, 4367, 4464, 4531, 53285, 4367, 4464, 4532, 53286, 4367, 4464, 4533, 53287, 4367, 4464, 4534, 53288, 4367, 4464, 4535, 53289, 4367, 4464, 4536,
    53290, 4367, 4464, 12612, 53291, 4367, 4464, 4538, 53292, 4367, 4464, 4539, 53293, 4367, 4464, 4540, 53294, 4367, 4464, 4364, 53295, 4367, 4464, 4542,
    53296, 4367, 4464, 4543, 53297, 4367, 4464, 4544, 53298, 4367, 4464, 4545, 53299, 4367, 4464, 4546, 53300, 4367, 4465, 0, 53301, 4367, 4465, 4520,
    53302, 4367, 4465, 4521, 53303, 4367, 4465, 4522, 53304, 4367, 4465, 4523, 53305, 4367, 4465, 4524, 53306, 4367, 4465, 4525, 53307, 4367, 4465, 4526,
    53308, 4367, 4465, 4527, 53309, 4367, 4465, 4528, 53310, 4367, 4465, 4529, 53311, 4367, 4465, 4530, 53312, 4367, 4465, 4531, 53313, 4367, 4465, 4532,
    53314, 4367, 4465, 4533, 53315, 4367, 4465, 4534, 53316, 4367, 4465, 4535, 53317, 4367, 4465, 4536, 53318, 4367, 4465, 12612, 53319, 4367, 4465, 4538,
    53320, 4367, 4465, 4539, 53321, 4367, 4465, 4540, 53322, 4367, 4465, 4364, 53323, 4367, 4465, 4542, 53324, 4367, 4465, 4543, 53325, 4367, 4465, 4544,
    53326, 4367, 4465, 4545, 53327, 4367, 4465, 4546, 53328, 4367, 4466, 0, 53329, 4367, 4466, 4520, 53330, 4367, 4466, 4521, 53331, 4367, 4466, 4522,
    53332, 4367, 4466, 4523, 53333, 4367, 4466, 4524, 53334, 4367, 4466, 4525, 53335, 4367, 4466, 4526, 53336, 4367, 4466, 4527, 53337, 4367, 4466, 4528,
    53338, 4367, 4466, 4529, 53339, 4367, 4466, 4530, 53340, 4367, 4466, 4531, 53341, 4367, 4466, 4532, 53342, 4367, 4466, 4533, 53343, 4367, 4466, 4534,
    53344, 4367, 4466, 4535, 53345, 4367, 4466, 4536, 53346, 4367, 4466, 12612, 53347, 4367, 4466, 4538, 53348, 4367, 4466, 4539, 53349, 4367, 4466, 4540,
    53350, 4367, 4466, 4364, 53351, 4367, 4466, 4542, 53352, 4367, 4466, 4543, 53353, 4367, 4466, 4544, 53354, 4367, 4466, 4545, 53355, 4367, 4466, 4546,
    53356, 4367, 4467, 0, 53357, 4367, 4467, 4520, 53358, 4367, 4467, 4521, 53359, 4367, 4467, 4522, 53360, 4367, 4467, 4523, 53361, 4367, 4467, 4524,
    53362, 4367, 4467, 4525, 53363, 4367, 4467, 4526, 53364, 4367, 4467, 4527, 53365, 4367, 4467, 4528, 53366, 4367, 4467, 4529, 53367, 4367, 4467, 4530,
    53368, 4367, 4467, 4531, 53369, 4367, 4467, 4532, 53370, 4367, 4467, 4533, 53371, 4367, 4467, 4534, 53372, 4367, 4467, 4535, 53373, 4367, 4467, 4536,
    53374, 4367, 4467, 12612, 53375, 4367, 4467, 4538, 53376, 4367, 4467, 4539, 53377, 4367, 4467, 4540, 53378, 4367, 4467, 4364, 53379, 4367, 4467, 4542,
    53380, 4367, 4467, 4543, 53381, 4367, 4467, 4544, 53382, 4367, 4467, 4545, 53383, 4367, 4467, 4546, 53384, 4367, 4468, 0, 53385, 4367, 4468, 4520,
    53386, 4367, 4468, 4521, 53387, 4367, 4468, 4522, 53388, 4367, 4468, 4523, 53389, 4367, 4468, 4524, 53390, 4367, 4468, 4525, 53391, 4367, 4468, 4526,
    53392, 4367, 4468, 4527, 53393, 4367, 4468, 4528, 53394, 4367, 4468, 4529, 53395, 4367, 4468, 4530, 53396, 4367, 4468, 4531, 53397, 4367, 4468, 4532,
    53398, 4367, 4468, 4533, 53399, 4367, 4468, 4534, 53400, 4367, 4468, 4535, 53401, 4367, 4468, 4536, 53402, 4367, 4468, 12612, 53403, 4367, 4468, 4538,
    53404, 4367, 4468, 4539, 53405, 4367, 4468, 4540, 53406, 4367, 4468, 4364, 53407, 4367, 4468, 4542, 53408, 4367, 4468, 4543, 53409, 4367, 4468, 4544,
    53410, 4367, 4468, 4545, 53411, 4367, 4468, 4546, 53412, 4367, 4469, 0, 53413, 4367, 4469, 4520, 53414, 4367, 4469, 4521, 53415, 4367, 4469, 4522,
    53416, 4367, 4469, 4523, 53417, 4367, 4469, 4524, 53418, 4367, 4469, 4525, 53419, 4367, 4469, 4526, 53420, 4367, 4469, 4527, 53421, 4367, 4469, 4528,
    53422, 4367, 4469, 4529, 53423, 4367, 4469, 4530, 53424, 4367, 4469, 4531, 53425, 4367, 4469, 4532, 53426, 4367, 4469, 4533, 53427, 4367, 4469, 4534,
    53428, 4367, 4469, 4535, 53429, 4367, 4469, 4536, 53430, 4367, 4469, 12612, 53431, 4367, 4469, 4538, 53432, 4367, 4469, 4539, 53433, 4367, 4469, 4540,
    53434, 4367, 4469, 4364, 53435, 4367, 4469, 4542, 53436, 4367, 4469, 4543, 53437, 4367, 4469, 4544, 53438, 4367, 4469, 4545, 53439, 4367, 4469, 4546,
    53440, 4368, 4449, 0, 53441, 4368, 4449, 4520, 53442, 4368, 4449, 4521, 53443, 4368, 4449, 4522, 53444, 4368, 4449, 4523, 53445, 4368, 4449, 4524,
    53446, 4368, 4449, 4525, 53447, 4368, 4449, 4526, 53448, 4368, 4449, 4527, 53449, 4368, 4449, 4528, 53450, 4368, 4449, 4529, 53451, 4368, 4449, 4530,
    53452, 4368, 4449, 4531, 53453, 4368, 4449, 4532, 53454, 4368, 4449, 4533, 53455, 4368, 4449, 4534, 53456, 4368, 4449, 4535, 53457, 4368, 4449, 4536,
    53458, 4368, 4449, 12612, 53459, 4368, 4449, 4538, 53460, 4368, 4449, 4539, 53461, 4368, 4449, 4540, 53462, 4368, 4449, 4364, 53463, 4368, 4449, 4542,
    53464, 4368, 4449, 4543, 53465, 4368, 4449, 4544, 53466, 4368, 4449, 4545, 53467, 4368, 4449, 4546, 53468, 4368, 4450, 0, 53469, 4368, 4450, 4520,
    53470, 4368, 4450, 4521, 53471, 4368, 4450, 4522, 53472, 4368, 4450, 4523, 53473, 4368, 4450, 4524, 53474, 4368, 4450, 4525, 53475, 4368, 4450, 4526,
    53476, 4368, 4450, 4527, 53477, 4368, 4450, 4528, 53478, 4368, 4450, 4529, 53479, 4368, 4450, 4530, 53480, 4368, 4450, 4531, 53481, 4368, 4450, 4532,
    53482, 4368, 4450, 4533, 53483, 4368, 4450, 4534, 53484, 4368, 4450, 4535, 53485, 4368, 4450, 4536, 53486, 4368, 4450, 12612, 53487, 4368, 4450, 4538,
    53488, 4368, 4450, 4539, 53489, 4368, 4450, 4540, 53490, 4368, 4450, 4364, 53491, 4368, 4450, 4542, 53492, 4368, 4450, 4543, 53493, 4368, 4450, 4544,
    53494, 4368, 4450, 4545, 53495, 4368, 4450, 4546, 53496, 4368, 4451, 0, 53497, 4368, 4451, 4520, 53498, 4368, 4451, 4521, 53499, 4368, 4451, 4522,
    53500, 4368, 4451, 4523, 53501, 4368, 4451, 4524, 53502, 4368, 4451, 4525, 53503, 4368, 4451, 4526, 53504, 4368, 4451, 4527, 53505, 4368, 4451, 4528,
    53506, 4368, 4451, 4529, 53507, 4368, 4451, 4530, 53508, 4368, 4451, 4531, 53509, 4368, 4451, 4532, 53510, 4368, 4451, 4533, 53511, 4368, 4451, 4534,
    53512, 4368, 4451, 4535, 53513, 4368, 4451, 4536, 53514, 4368, 4451, 12612, 53515, 4368, 4451, 4538, 53516, 4368, 4451, 4539, 53517, 4368, 4451, 4540,
    53518, 4368, 4451, 4364, 53519, 4368, 4451, 4542, 53520, 4368, 4451, 4543, 53521, 4368, 4451, 4544, 53522, 4368, 4451, 4545, 53523, 4368, 4451, 4546,
    53524, 4368, 4452, 0, 53525, 4368, 4452, 4520, 53526, 4368, 4452, 4521, 53527, 4368, 4452, 4522, 53528, 4368, 4452, 4523, 53529, 4368, 4452, 4524,
    53530, 4368, 4452, 4525, 53531, 4368, 4452, 4526, 53532, 4368, 4452, 4527, 53533, 4368, 4452, 4528, 53534, 4368, 4452, 4529, 53535, 4368, 4452, 4530,
    53536, 4368, 4452, 4531, 53537, 4368, 4452, 4532, 53538, 4368, 4452, 4533, 53539, 4368, 4452, 4534, 53540, 4368, 4452, 4535, 53541, 4368, 4452, 4536,
    53542, 4368, 4452, 12612, 53543, 4368, 4452, 4538, 53544, 4368, 4452, 4539, 53545, 4368, 4452, 4540, 53546, 4368, 4452, 4364, 53547, 4368, 4452, 4542,
    53548, 4368, 4452, 4543, 53549, 4368, 4452, 4544, 53550, 4368, 4452, 4545, 53551, 4368, 4452, 4546, 53552, 4368, 4453, 0, 53553, 4368, 4453, 4520,
    53554, 4368, 4453, 4521, 53555, 4368, 4453, 4522, 53556, 4368, 4453, 4523, 53557, 4368, 4453, 4524, 53558, 4368, 4453, 4525, 53559, 4368, 4453, 4526,
    53560, 4368, 4453, 4527, 53561, 4368, 4453, 4528, 53562, 4368, 4453, 4529, 53563, 4368, 4453, 4530, 53564, 4368, 4453, 4531, 53565, 4368, 4453, 4532,
    53566, 4368, 4453, 4533, 53567, 4368, 4453, 4534, 53568, 4368, 4453, 4535, 53569, 4368, 4453, 4536, 53570, 4368, 4453, 12612, 53571, 4368, 4453, 4538,
    53572, 4368, 4453, 4539, 53573, 4368, 4453, 4540, 53574, 4368, 4453, 4364, 53575, 4368, 4453, 4542, 53576, 4368, 4453, 4543, 53577, 4368, 4453, 4544,
    53578, 4368, 4453, 4545, 53579, 4368, 4453, 4546, 53580, 4368, 4454, 0, 53581, 4368, 4454, 4520, 53582, 4368, 4454, 4521, 53583, 4368, 4454, 4522,
    53584, 4368, 4454, 4523, 53585, 4368, 4454, 4524, 53586, 4368, 4454, 4525, 53587, 4368, 4454, 4526, 53588, 4368, 4454, 4527, 53589, 4368, 4454, 4528,
    53590, 4368, 4454, 4529, 53591, 4368, 4454, 4530, 53592, 4368, 4454, 4531, 53593, 4368, 4454, 4532, 53594, 4368, 4454, 4533, 53595, 4368, 4454, 4534,
    53596, 4368, 4454, 4535, 53597, 4368, 4454, 4536, 53598, 4368, 4454, 12612, 53599, 4368, 4454, 4538, 53600, 4368, 4454, 4539, 53601, 4368, 4454, 4540,
    53602, 4368, 4454, 4364, 53603, 4368, 4454, 4542, 53604, 4368, 4454, 4543, 53605, 4368, 4454, 4544, 53606, 4368, 4454, 4545, 53607, 4368, 4454, 4546,
    53608, 4368, 4455, 0, 53609, 4368, 4455, 4520, 53610, 4368, 4455, 4521, 53611, 4368, 4455, 4522, 53612, 4368, 4455, 4523, 53613, 4368, 4455, 4524,
    53614, 4368, 4455, 4525, 53615, 4368, 4455, 4526, 53616, 4368, 4455, 4527, 53617, 4368, 4455, 4528, 53618, 4368, 4455, 4529, 53619, 4368, 4455, 4530,
    53620, 4368, 4455, 4531, 53621, 4368, 4455, 4532, 53622, 4368, 4455, 4533, 53623, 4368, 4455, 4534, 53624, 4368, 4455, 4535, 53625, 4368, 4455, 4536,
    53626, 4368, 4455, 12612, 53627, 4368, 4455, 4538, 53628, 4368, 4455, 4539, 53629, 4368, 4455, 4540, 53630, 4368, 4455, 4364, 53631, 4368, 4455, 4542,
    53632, 4368, 4455, 4543, 53633, 4368, 4455, 4544, 53634, 4368, 4455, 4545, 53635, 4368, 4455, 4546, 53636, 4368, 4456, 0, 53637, 4368, 4456, 4520,
    53638, 4368, 4456, 4521, 53639, 4368, 4456, 4522, 53640, 4368, 4456, 4523, 53641, 4368, 4456, 4524, 53642, 4368, 4456, 4525, 53643, 4368, 4456, 4526,
    53644, 4368, 4456, 4527, 53645, 4368, 4456, 4528, 53646, 4368, 4456, 4529, 53647, 4368, 4456, 4530, 53648, 4368, 4456, 4531, 53649, 4368, 4456, 4532,
    53650, 4368, 4456, 4533, 53651, 4368, 4456, 4534, 53652, 4368, 4456, 4535, 53653, 4368, 4456, 4536, 53654, 4368, 4456, 12612, 53655, 4368, 4456, 4538,
    53656, 4368, 4456, 4539, 53657, 4368, 4456, 4540, 53658, 4368, 4456, 4364, 53659, 4368, 4456, 4542, 53660, 4368, 4456, 4543, 53661, 4368, 4456, 4544,
    53662, 4368, 4456, 4545, 53663, 4368, 4456, 4546, 53664, 4368, 4457, 0, 53665, 4368, 4457, 4520, 53666, 4368, 4457, 4521, 53667, 4368, 4457, 4522,
    53668, 4368, 4457, 4523, 53669, 4368, 4457, 4524, 53670, 4368, 4457, 4525, 53671, 4368, 4457, 4526, 53672, 4368, 4457, 4527, 53673, 4368, 4457, 4528,
    53674, 4368, 4457, 4529, 53675, 4368, 4457, 4530, 53676, 4368, 4457, 4531, 53677, 4368, 4457, 4532, 53678, 4368, 4457, 4533, 53679, 4368, 4457, 4534,
    53680, 4368, 4457, 4535, 53681, 4368, 4457, 4536, 53682, 4368, 4457, 12612, 53683, 4368, 4457, 4538, 53684, 4368, 4457, 4539, 53685, 4368, 4457, 4540,
    53686, 4368, 4457, 4364, 53687, 4368, 4457, 4542, 53688, 4368, 4457, 4543, 53689, 4368, 4457, 4544, 53690, 4368, 4457, 4545, 53691, 4368, 4457, 4546,
    53692, 4368, 4458, 0, 53693, 4368, 4458, 4520, 53694, 4368, 4458, 4521, 53695, 4368, 4458, 4522, 53696, 4368, 4458, 4523, 53697, 4368, 4458, 4524,
    53698, 4368, 4458, 4525, 53699, 4368, 4458, 4526, 53700, 4368, 4458, 4527, 53701, 4368, 4458, 4528, 53702, 4368, 4458, 4529, 53703, 4368, 4458, 4530,
    53704, 4368, 4458, 4531, 53705, 4368, 4458, 4532, 53706, 4368, 4458, 4533, 53707, 4368, 4458, 4534, 53708, 4368, 4458, 4535, 53709, 4368, 4458, 4536,
    53710, 4368, 4458, 12612, 53711, 4368, 4458, 4538, 53712, 4368, 4458, 4539, 53713, 4368, 4458, 4540, 53714, 4368, 4458, 4364, 53715, 4368, 4458, 4542,
    53716, 4368, 4458, 4543, 53717, 4368, 4458, 4544, 53718, 4368, 4458, 4545, 53719, 4368, 4458, 4546, 53720, 4368, 4459, 0, 53721, 4368, 4459, 4520,
    53722, 4368, 4459, 4521, 53723, 4368, 4459, 4522, 53724, 4368, 4459, 4523, 53725, 4368, 4459, 4524, 53726, 4368, 4459, 4525, 53727, 4368, 4459, 4526,
    53728, 4368, 4459, 4527, 53729, 4368, 4459, 4528, 53730, 4368, 4459, 4529, 53731, 4368, 4459, 4530, 53732, 4368, 4459, 4531, 53733, 4368, 4459, 4532,
    53734, 4368, 4459, 4533, 53735, 4368, 4459, 4534, 53736, 4368, 4459, 4535, 53737, 4368, 4459, 4536, 53738, 4368, 4459, 12612, 53739, 4368, 4459, 4538,
    53740, 4368, 4459, 4539, 53741, 4368, 4459, 4540, 53742, 4368, 4459, 4364, 53743, 4368, 4459, 4542, 53744, 4368, 4459, 4543, 53745, 4368, 4459, 4544,
    53746, 4368, 4459, 4545, 53747, 4368, 4459, 4546, 53748, 4368, 4460, 0, 53749, 4368, 4460, 4520, 53750, 4368, 4460, 4521, 53751, 4368, 4460, 4522,
    53752, 4368, 4460, 4523, 53753, 4368, 4460, 4524, 53754, 4368, 4460, 4525, 53755, 4368, 4460, 4526, 53756, 4368, 4460, 4527, 53757, 4368, 4460, 4528,
    53758, 4368, 4460, 4529, 53759, 4368, 4460, 4530, 53760, 4368, 4460, 4531, 53761, 4368, 4460, 4532, 53762, 4368, 4460, 4533, 53763, 4368, 4460, 4534,
    53764, 4368, 4460, 4535, 53765, 4368, 4460, 4536, 53766, 4368, 4460, 12612, 53767, 4368, 4460, 4538, 53768, 4368, 4460, 4539, 53769, 4368, 4460, 4540,
    53770, 4368, 4460, 4364, 53771, 4368, 4460, 4542, 53772, 4368, 4460, 4543, 53773, 4368, 4460, 4544, 53774, 4368, 4460, 4545, 53775, 4368, 4460, 4546,
    53776, 4368, 4461, 0, 53777, 4368, 4461, 4520, 53778, 4368, 4461, 4521, 53779, 4368, 4461, 4522, 53780, 4368, 4461, 4523, 53781, 4368, 4461, 4524,
    53782, 4368, 4461, 4525, 53783, 4368, 4461, 4526, 53784, 4368, 4461, 4527, 53785, 4368, 4461, 4528, 53786, 4368, 4461, 4529, 53787, 4368, 4461, 4530,
    53788, 4368, 4461, 4531, 53789, 4368, 4461, 4532, 53790, 4368, 4461, 4533, 53791, 4368, 4461, 4534, 53792, 4368, 4461, 4535, 53793, 4368, 4461, 4536,
    53794, 4368, 4461, 12612, 53795, 4368, 4461, 4538, 53796, 4368, 4461, 4539, 53797, 4368, 4461, 4540, 53798, 4368, 4461, 4364, 53799, 4368, 4461, 4542,
    53800, 4368, 4461, 4543, 53801, 4368, 4461, 4544, 53802, 4368, 4461, 4545, 53803, 4368, 4461, 4546, 53804, 4368, 4462, 0, 53805, 4368, 4462, 4520,
    53806, 4368, 4462, 4521, 53807, 4368, 4462, 4522, 53808, 4368, 4462, 4523, 53809, 4368, 4462, 4524, 53810, 4368, 4462, 4525, 53811, 4368, 4462, 4526,
    53812, 4368, 4462, 4527, 53813, 4368, 4462, 4528, 53814, 4368, 4462, 4529, 53815, 4368, 4462, 4530, 53816, 4368, 4462, 4531, 53817, 4368, 4462, 4532,
    53818, 4368, 4462, 4533, 53819, 4368, 4462, 4534, 53820, 4368, 4462, 4535, 53821, 4368, 4462, 4536, 53822, 4368, 4462, 12612, 53823, 4368, 4462, 4538,
    53824, 4368, 4462, 4539, 53825, 4368, 4462, 4540, 53826, 4368, 4462, 4364, 53827, 4368, 4462, 4542, 53828, 4368, 4462, 4543, 53829, 4368, 4462, 4544,
    53830, 4368, 4462, 4545, 53831, 4368, 4462, 4546, 53832, 4368, 4463, 0, 53833, 4368, 4463, 4520, 53834, 4368, 4463, 4521, 53835, 4368, 4463, 4522,
    53836, 4368, 4463, 4523, 53837, 4368, 4463, 4524, 53838, 4368, 4463, 4525, 53839, 4368, 4463, 4526, 53840, 4368, 4463, 4527, 53841, 4368, 4463, 4528,
    53842, 4368, 4463, 4529, 53843, 4368, 4463, 4530, 53844, 4368, 4463, 4531, 53845, 4368, 4463, 4532, 53846, 4368, 4463, 4533, 53847, 4368, 4463, 4534,
    53848, 4368, 4463, 4535, 53849, 4368, 4463, 4536, 53850, 4368, 4463, 12612, 53851, 4368, 4463, 4538, 53852, 4368, 4463, 4539, 53853, 4368, 4463, 4540,
    53854, 4368, 4463, 4364, 53855, 4368, 4463, 4542, 53856, 4368, 4463, 4543, 53857, 4368, 4463, 4544, 53858, 4368, 4463, 4545, 53859, 4368, 4463, 4546,
    53860, 4368, 4464, 0, 53861, 4368, 4464, 4520, 53862, 4368, 4464, 4521, 53863, 4368, 4464, 4522, 53864, 4368, 4464, 4523, 53865, 4368, 4464, 4524,
    53866, 4368, 4464, 4525, 53867, 4368, 4464, 4526, 53868, 4368, 4464, 4527, 53869, 4368, 4464, 4528, 53870, 4368, 4464, 4529, 53871, 4368, 4464, 4530,
    53872, 4368, 4464, 4531, 53873, 4368, 4464, 4532, 53874, 4368, 4464, 4533, 53875, 4368, 4464, 4534, 53876, 4368, 4464, 4535, 53877, 4368, 4464, 4536,
    53878, 4368, 4464, 12612, 53879, 4368, 4464, 4538, 53880, 4368, 4464, 4539, 53881, 4368, 4464, 4540, 53882, 4368, 4464, 4364, 53883, 4368, 4464, 4542,
    53884, 4368, 4464, 4543, 53885, 4368, 4464, 4544, 53886, 4368, 4464, 4545, 53887, 4368, 4464, 4546, 53888, 4368, 4465, 0, 53889, 4368, 4465, 4520,
    53890, 4368, 4465, 4521, 53891, 4368, 4465, 4522, 53892, 4368, 4465, 4523, 53893, 4368, 4465, 4524, 53894, 4368, 4465, 4525, 53895, 4368, 4465, 4526,
    53896, 4368, 4465, 4527, 53897, 4368, 4465, 4528, 53898, 4368, 4465, 4529, 53899, 4368, 4465, 4530, 53900, 4368, 4465, 4531, 53901, 4368, 4465, 4532,
    53902, 4368, 4465, 4533, 53903, 4368, 4465, 4534, 53904, 4368, 4465, 4535, 53905, 4368, 4465, 4536, 53906, 4368, 4465, 12612, 53907, 4368, 4465, 4538,
    53908, 4368, 4465, 4539, 53909, 4368, 4465, 4540, 53910, 4368, 4465, 4364, 53911, 4368, 4465, 4542, 53912, 4368, 4465, 4543, 53913, 4368, 4465, 4544,
    53914, 4368, 4465, 4545, 53915, 4368, 4465, 4546, 53916, 4368, 4466, 0, 53917, 4368, 4466, 4520, 53918, 4368, 4466, 4521, 53919, 4368, 4466, 4522,
    53920, 4368, 4466, 4523, 53921, 4368, 4466, 4524, 53922, 4368, 4466, 4525, 53923, 4368, 4466, 4526, 53924, 4368, 4466, 4527, 53925, 4368, 4466, 4528,
    53926, 4368, 4466, 4529, 53927, 4368, 4466, 4530, 53928, 4368, 4466, 4531, 53929, 4368, 4466, 4532, 53930, 4368, 4466, 4533, 53931, 4368, 4466, 4534,
    53932, 4368, 4466, 4535, 53933, 4368, 4466, 4536, 53934, 4368, 4466, 12612, 53935, 4368, 4466, 4538, 53936, 4368, 4466, 4539, 53937, 4368, 4466, 4540,
    53938, 4368, 4466, 4364, 53939, 4368, 4466, 4542, 53940, 4368, 4466, 4543, 53941, 4368, 4466, 4544, 53942, 4368, 4466, 4545, 53943, 4368, 4466, 4546,
    53944, 4368, 4467, 0, 53945, 4368, 4467, 4520, 53946, 4368, 4467, 4521, 53947, 4368, 4467, 4522, 53948, 4368, 4467, 4523, 53949, 4368, 4467, 4524,
    53950, 4368, 4467, 4525, 53951, 4368, 4467, 4526, 53952, 4368, 4467, 4527, 53953, 4368, 4467, 4528, 53954, 4368, 4467, 4529, 53955, 4368, 4467, 4530,
    53956, 4368, 4467, 4531, 53957, 4368, 4467, 4532, 53958, 4368, 4467, 4533, 53959, 4368, 4467, 4534, 53960, 4368, 4467, 4535, 53961, 4368, 4467, 4536,
    53962, 4368, 4467, 12612, 53963, 4368, 4467, 4538, 53964, 4368, 4467, 4539, 53965, 4368, 4467, 4540, 53966, 4368, 4467, 4364, 53967, 4368, 4467, 4542,
    53968, 4368, 4467, 4543, 53969, 4368, 4467, 4544, 53970, 4368, 4467, 4545, 53971, 4368, 4467, 4546, 53972, 4368, 4468, 0, 53973, 4368, 4468, 4520,
    53974, 4368, 4468, 4521, 53975, 4368, 4468, 4522, 53976, 4368, 4468, 4523, 53977, 4368, 4468, 4524, 53978, 4368, 4468, 4525, 53979, 4368, 4468, 4526,
    53980, 4368, 4468, 4527, 53981, 4368, 4468, 4528, 53982, 4368, 4468, 4529, 53983, 4368, 4468, 4530, 53984, 4368, 4468, 4531, 53985, 4368, 4468, 4532,
    53986, 4368, 4468, 4533, 53987, 4368, 4468, 4534, 53988, 4368, 4468, 4535, 53989, 4368, 4468, 4536, 53990, 4368, 4468, 12612, 53991, 4368, 4468, 4538,
    53992, 4368, 4468, 4539, 53993, 4368, 4468, 4540, 53994, 4368, 4468, 4364, 53995, 4368, 4468, 4542, 53996, 4368, 4468, 4543, 53997, 4368, 4468, 4544,
    53998, 4368, 4468, 4545, 53999, 4368, 4468, 4546, 54000, 4368, 4469, 0, 54001, 4368, 4469, 4520, 54002, 4368, 4469, 4521, 54003, 4368, 4469, 4522,
    54004, 4368, 4469, 4523, 54005, 4368, 4469, 4524, 54006, 4368, 4469, 4525, 54007, 4368, 4469, 4526, 54008, 4368, 4469, 4527, 54009, 4368, 4469, 4528,
    54010, 4368, 4469, 4529, 54011, 4368, 4469, 4530, 54012, 4368, 4469, 4531, 54013, 4368, 4469, 4532, 54014, 4368, 4469, 4533, 54015, 4368, 4469, 4534,
    54016, 4368, 4469, 4535, 54017, 4368, 4469, 4536, 54018, 4368, 4469, 12612, 54019, 4368, 4469, 4538, 54020, 4368, 4469, 4539, 54021, 4368, 4469, 4540,
    54022, 4368, 4469, 4364, 54023, 4368, 4469, 4542, 54024, 4368, 4469, 4543, 54025, 4368, 4469, 4544, 54026, 4368, 4469, 4545, 54027, 4368, 4469, 4546,
    54028, 4369, 4449, 0, 54029, 4369, 4449, 4520, 54030, 4369, 4449, 4521, 54031, 4369, 4449, 4522, 54032, 4369, 4449, 4523, 54033, 4369, 4449, 4524,
    54034, 4369, 4449, 4525, 54035, 4369, 4449, 4526, 54036, 4369, 4449, 4527, 54037, 4369, 4449, 4528, 54038, 4369, 4449, 4529, 54039, 4369, 4449, 4530,
    54040, 4369, 4449, 4531, 54041, 4369, 4449, 4532, 54042, 4369, 4449, 4533, 54043, 4369, 4449, 4534, 54044, 4369, 4449, 4535, 54045, 4369, 4449, 4536,
    54046, 4369, 4449, 12612, 54047, 4369, 4449, 4538, 54048, 4369, 4449, 4539, 54049, 4369, 4449, 4540, 54050, 4369, 4449, 4364, 54051, 4369, 4449, 4542,
    54052, 4369, 4449, 4543, 54053, 4369, 4449, 4544, 54054, 4369, 4449, 4545, 54055, 4369, 4449, 4546, 54056, 4369, 4450, 0, 54057, 4369, 4450, 4520,
    54058, 4369, 4450, 4521, 54059, 4369, 4450, 4522, 54060, 4369, 4450, 4523, 54061, 4369, 4450, 4524, 54062, 4369, 4450, 4525, 54063, 4369, 4450, 4526,
    54064, 4369, 4450, 4527, 54065, 4369, 4450, 4528, 54066, 4369, 4450, 4529, 54067, 4369, 4450, 4530, 54068, 4369, 4450, 4531, 54069, 4369, 4450, 4532,
    54070, 4369, 4450, 4533, 54071, 4369, 4450, 4534, 54072, 4369, 4450, 4535, 54073, 4369, 4450, 4536, 54074, 4369, 4450, 12612, 54075, 4369, 4450, 4538,
    54076, 4369, 4450, 4539, 54077, 4369, 4450, 4540, 54078, 4369, 4450, 4364, 54079, 4369, 4450, 4542, 54080, 4369, 4450, 4543, 54081, 4369, 4450, 4544,
    54082, 4369, 4450, 4545, 54083, 4369, 4450, 4546, 54084, 4369, 4451, 0, 54085, 4369, 4451, 4520, 54086, 4369, 4451, 4521, 54087, 4369, 4451, 4522,
    54088, 4369, 4451, 4523, 54089, 4369, 4451, 4524, 54090, 4369, 4451, 4525, 54091, 4369, 4451, 4526, 54092, 4369, 4451, 4527, 54093, 4369, 4451, 4528,
    54094, 4369, 4451, 4529, 54095, 4369, 4451, 4530, 54096, 4369, 4451, 4531, 54097, 4369, 4451, 4532, 54098, 4369, 4451, 4533, 54099, 4369, 4451, 4534,
    54100, 4369, 4451, 4535, 54101, 4369, 4451, 4536, 54102, 4369, 4451, 12612, 54103, 4369, 4451, 4538, 54104, 4369, 4451, 4539, 54105, 4369, 4451, 4540,
    54106, 4369, 4451, 4364, 54107, 4369, 4451, 4542, 54108, 4369, 4451, 4543, 54109, 4369, 4451, 4544, 54110, 4369, 4451, 4545, 54111, 4369, 4451, 4546,
    54112, 4369, 4452, 0, 54113, 4369, 4452, 4520, 54114, 4369, 4452, 4521, 54115, 4369, 4452, 4522, 54116, 4369, 4452, 4523, 54117, 4369, 4452, 4524,
    54118, 4369, 4452, 4525, 54119, 4369, 4452, 4526, 54120, 4369, 4452, 4527, 54121, 4369, 4452, 4528, 54122, 4369, 4452, 4529, 54123, 4369, 4452, 4530,
    54124, 4369, 4452, 4531, 54125, 4369, 4452, 4532, 54126, 4369, 4452, 4533, 54127, 4369, 4452, 4534, 54128, 4369, 4452, 4535, 54129, 4369, 4452, 4536,
    54130, 4369, 4452, 12612, 54131, 4369, 4452, 4538, 54132, 4369, 4452, 4539, 54133, 4369, 4452, 4540, 54134, 4369, 4452, 4364, 54135, 4369, 4452, 4542,
    54136, 4369, 4452, 4543, 54137, 4369, 4452, 4544, 54138, 4369, 4452, 4545, 54139, 4369, 4452, 4546, 54140, 4369, 4453, 0, 54141, 4369, 4453, 4520,
    54142, 4369, 4453, 4521, 54143, 4369, 4453, 4522, 54144, 4369, 4453, 4523, 54145, 4369, 4453, 4524, 54146, 4369, 4453, 4525, 54147, 4369, 4453, 4526,
    54148, 4369, 4453, 4527, 54149, 4369, 4453, 4528, 54150, 4369, 4453, 4529, 54151, 4369, 4453, 4530, 54152, 4369, 4453, 4531, 54153, 4369, 4453, 4532,
    54154, 4369, 4453, 4533, 54155, 4369, 4453, 4534, 54156, 4369, 4453, 4535, 54157, 4369, 4453, 4536, 54158, 4369, 4453, 12612, 54159, 4369, 4453, 4538,
    54160, 4369, 4453, 4539, 54161, 4369, 4453, 4540, 54162, 4369, 4453, 4364, 54163, 4369, 4453, 4542, 54164, 4369, 4453, 4543, 54165, 4369, 4453, 4544,
    54166, 4369, 4453, 4545, 54167, 4369, 4453, 4546, 54168, 4369, 4454, 0, 54169, 4369, 4454, 4520, 54170, 4369, 4454, 4521, 54171, 4369, 4454, 4522,
    54172, 4369, 4454, 4523, 54173, 4369, 4454, 4524, 54174, 4369, 4454, 4525, 54175, 4369, 4454, 4526, 54176, 4369, 4454, 4527, 54177, 4369, 4454, 4528,
    54178, 4369, 4454, 4529, 54179, 4369, 4454, 4530, 54180, 4369, 4454, 4531, 54181, 4369, 4454, 4532, 54182, 4369, 4454, 4533, 54183, 4369, 4454, 4534,
    54184, 4369, 4454, 4535, 54185, 4369, 4454, 4536, 54186, 4369, 4454, 12612, 54187, 4369, 4454, 4538, 54188, 4369, 4454, 4539, 54189, 4369, 4454, 4540,
    54190, 4369, 4454, 4364, 54191, 4369, 4454, 4542, 54192, 4369, 4454, 4543, 54193, 4369, 4454, 4544, 54194, 4369, 4454, 4545, 54195, 4369, 4454, 4546,
    54196, 4369, 4455, 0, 54197, 4369, 4455, 4520, 54198, 4369, 4455, 4521, 54199, 4369, 4455, 4522, 54200, 4369, 4455, 4523, 54201, 4369, 4455, 4524,
    54202, 4369, 4455, 4525, 54203, 4369, 4455, 4526, 54204, 4369, 4455, 4527, 54205, 4369, 4455, 4528, 54206, 4369, 4455, 4529, 54207, 4369, 4455, 4530,
    54208, 4369, 4455, 4531, 54209, 4369, 4455, 4532, 54210, 4369, 4455, 4533, 54211, 4369, 4455, 4534, 54212, 4369, 4455, 4535, 54213, 4369, 4455, 4536,
    54214, 4369, 4455, 12612, 54215, 4369, 4455, 4538, 54216, 4369, 4455, 4539, 54217, 4369, 4455, 4540, 54218, 4369, 4455, 4364, 54219, 4369, 4455, 4542,
    54220, 4369, 4455, 4543, 54221, 4369, 4455, 4544, 54222, 4369, 4455, 4545, 54223, 4369, 4455, 4546, 54224, 4369, 4456, 0, 54225, 4369, 4456, 4520,
    54226, 4369, 4456, 4521, 54227, 4369, 4456, 4522, 54228, 4369, 4456, 4523, 54229, 4369, 4456, 4524, 54230, 4369, 4456, 4525, 54231, 4369, 4456, 4526,
    54232, 4369, 4456, 4527, 54233, 4369, 4456, 4528, 54234, 4369, 4456, 4529, 54235, 4369, 4456, 4530, 54236, 4369, 4456, 4531, 54237, 4369, 4456, 4532,
    54238, 4369, 4456, 4533, 54239, 4369, 4456, 4534, 54240, 4369, 4456, 4535, 54241, 4369, 4456, 4536, 54242, 4369, 4456, 12612, 54243, 4369, 4456, 4538,
    54244, 4369, 4456, 4539, 54245, 4369, 4456, 4540, 54246, 4369, 4456, 4364, 54247, 4369, 4456, 4542, 54248, 4369, 4456, 4543, 54249, 4369, 4456, 4544,
    54250, 4369, 4456, 4545, 54251, 4369, 4456, 4546, 54252, 4369, 4457, 0, 54253, 4369, 4457, 4520, 54254, 4369, 4457, 4521, 54255, 4369, 4457, 4522,
    54256, 4369, 4457, 4523, 54257, 4369, 4457, 4524, 54258, 4369, 4457, 4525, 54259, 4369, 4457, 4526, 54260, 4369, 4457, 4527, 54261, 4369, 4457, 4528,
    54262, 4369, 4457, 4529, 54263, 4369, 4457, 4530, 54264, 4369, 4457, 4531, 54265, 4369, 4457, 4532, 54266, 4369, 4457, 4533, 54267, 4369, 4457, 4534,
    54268, 4369, 4457, 4535, 54269, 4369, 4457, 4536, 54270, 4369, 4457, 12612, 54271, 4369, 4457, 4538, 54272, 4369, 4457, 4539, 54273, 4369, 4457, 4540,
    54274, 4369, 4457, 4364, 54275, 4369, 4457, 4542, 54276, 4369, 4457, 4543, 54277, 4369, 4457, 4544, 54278, 4369, 4457, 4545, 54279, 4369, 4457, 4546,
    54280, 4369, 4458, 0, 54281, 4369, 4458, 4520, 54282, 4369, 4458, 4521, 54283, 4369, 4458, 4522, 54284, 4369, 4458, 4523, 54285, 4369, 4458, 4524,
    54286, 4369, 4458, 4525, 54287, 4369, 4458, 4526, 54288, 4369, 4458, 4527, 54289, 4369, 4458, 4528, 54290, 4369, 4458, 4529, 54291, 4369, 4458, 4530,
    54292, 4369, 4458, 4531, 54293, 4369, 4458, 4532, 54294, 4369, 4458, 4533, 54295, 4369, 4458, 4534, 54296, 4369, 4458, 4535, 54297, 4369, 4458, 4536,
    54298, 4369, 4458, 12612, 54299, 4369, 4458, 4538, 54300, 4369, 4458, 4539, 54301, 4369, 4458, 4540, 54302, 4369, 4458, 4364, 54303, 4369, 4458, 4542,
    54304, 4369, 4458, 4543, 54305, 4369, 4458, 4544, 54306, 4369, 4458, 4545, 54307, 4369, 4458, 4546, 54308, 4369, 4459, 0, 54309, 4369, 4459, 4520,
    54310, 4369, 4459, 4521, 54311, 4369, 4459, 4522, 54312, 4369, 4459, 4523, 54313, 4369, 4459, 4524, 54314, 4369, 4459, 4525, 54315, 4369, 4459, 4526,
    54316, 4369, 4459, 4527, 54317, 4369, 4459, 4528, 54318, 4369, 4459, 4529, 54319, 4369, 4459, 4530, 54320, 4369, 4459, 4531, 54321, 4369, 4459, 4532,
    54322, 4369, 4459, 4533, 54323, 4369, 4459, 4534, 54324, 4369, 4459, 4535, 54325, 4369, 4459, 4536, 54326, 4369, 4459, 12612, 54327, 4369, 4459, 4538,
    54328, 4369, 4459, 4539, 54329, 4369, 4459, 4540, 54330, 4369, 4459, 4364, 54331, 4369, 4459, 4542, 54332, 4369, 4459, 4543, 54333, 4369, 4459, 4544,
    54334, 4369, 4459, 4545, 54335, 4369, 4459, 4546, 54336, 4369, 4460, 0, 54337, 4369, 4460, 4520, 54338, 4369, 4460, 4521, 54339, 4369, 4460, 4522,
    54340, 4369, 4460, 4523, 54341, 4369, 4460, 4524, 54342, 4369, 4460, 4525, 54343, 4369, 4460, 4526, 54344, 4369, 4460, 4527, 54345, 4369, 4460, 4528,
    54346, 4369, 4460, 4529, 54347, 4369, 4460, 4530, 54348, 4369, 4460, 4531, 54349, 4369, 4460, 4532, 54350, 4369, 4460, 4533, 54351, 4369, 4460, 4534,
    54352, 4369, 4460, 4535, 54353, 4369, 4460, 4536, 54354, 4369, 4460, 12612, 54355, 4369, 4460, 4538, 54356, 4369, 4460, 4539, 54357, 4369, 4460, 4540,
    54358, 4369, 4460, 4364, 54359, 4369, 4460, 4542, 54360, 4369, 4460, 4543, 54361, 4369, 4460, 4544, 54362, 4369, 4460, 4545, 54363, 4369, 4460, 4546,
    54364, 4369, 4461, 0, 54365, 4369, 4461, 4520, 54366, 4369, 4461, 4521, 54367, 4369, 4461, 4522, 54368, 4369, 4461, 4523, 54369, 4369, 4461, 4524,
    54370, 4369, 4461, 4525, 54371, 4369, 4461, 4526, 54372, 4369, 4461, 4527, 54373, 4369, 4461, 4528, 54374, 4369, 4461, 4529, 54375, 4369, 4461, 4530,
    54376, 4369, 4461, 4531, 54377, 4369, 4461, 4532, 54378, 4369, 4461, 4533, 54379, 4369, 4461, 4534, 54380, 4369, 4461, 4535, 54381, 4369, 4461, 4536,
    54382, 4369, 4461, 12612, 54383, 4369, 4461, 4538, 54384, 4369, 4461, 4539, 54385, 4369, 4461, 4540, 54386, 4369, 4461, 4364, 54387, 4369, 4461, 4542,
    54388, 4369, 4461, 4543, 54389, 4369, 4461, 4544, 54390, 4369, 4461, 4545, 54391, 4369, 4461, 4546, 54392, 4369, 4462, 0, 54393, 4369, 4462, 4520,
    54394, 4369, 4462, 4521, 54395, 4369, 4462, 4522, 54396, 4369, 4462, 4523, 54397, 4369, 4462, 4524, 54398, 4369, 4462, 4525, 54399, 4369, 4462, 4526,
    54400, 4369, 4462, 4527, 54401, 4369, 4462, 4528, 54402, 4369, 4462, 4529, 54403, 4369, 4462, 4530, 54404, 4369, 4462, 4531, 54405, 4369, 4462, 4532,
    54406, 4369, 4462, 4533, 54407, 4369, 4462, 4534, 54408, 4369, 4462, 4535, 54409, 4369, 4462, 4536, 54410, 4369, 4462, 12612, 54411, 4369, 4462, 4538,
    54412, 4369, 4462, 4539, 54413, 4369, 4462, 4540, 54414, 4369, 4462, 4364, 54415, 4369, 4462, 4542, 54416, 4369, 4462, 4543, 54417, 4369, 4462, 4544,
    54418, 4369, 4462, 4545, 54419, 4369, 4462, 4546, 54420, 4369, 4463, 0, 54421, 4369, 4463, 4520, 54422, 4369, 4463, 4521, 54423, 4369, 4463, 4522,
    54424, 4369, 4463, 4523, 54425, 4369, 4463, 4524, 54426, 4369, 4463, 4525, 54427, 4369, 4463, 4526, 54428, 4369, 4463, 4527, 54429, 4369, 4463, 4528,
    54430, 4369, 4463, 4529, 54431, 4369, 4463, 4530, 54432, 4369, 4463, 4531, 54433, 4369, 4463, 4532, 54434, 4369, 4463, 4533, 54435, 4369, 4463, 4534,
    54436, 4369, 4463, 4535, 54437, 4369, 4463, 4536, 54438, 4369, 4463, 12612, 54439, 4369, 4463, 4538, 54440, 4369, 4463, 4539, 54441, 4369, 4463, 4540,
    54442, 4369, 4463, 4364, 54443, 4369, 4463, 4542, 54444, 4369, 4463, 4543, 54445, 4369, 4463, 4544, 54446, 4369, 4463, 4545, 54447, 4369, 4463, 4546,
    54448, 4369, 4464, 0, 54449, 4369, 4464, 4520, 54450, 4369, 4464, 4521, 54451, 4369, 4464, 4522, 54452, 4369, 4464, 4523, 54453, 4369, 4464, 4524,
    54454, 4369, 4464, 4525, 54455, 4369, 4464, 4526, 54456, 4369, 4464, 4527, 54457, 4369, 4464, 4528, 54458, 4369, 4464, 4529, 54459, 4369, 4464, 4530,
    54460, 4369, 4464, 4531, 54461, 4369, 4464, 4532, 54462, 4369, 4464, 4533, 54463, 4369, 4464, 4534, 54464, 4369, 4464, 4535, 54465, 4369, 4464, 4536,
    54466, 4369, 4464, 12612, 54467, 4369, 4464, 4538, 54468, 4369, 4464, 4539, 54469, 4369, 4464, 4540, 54470, 4369, 4464, 4364, 54471, 4369, 4464, 4542,
    54472, 4369, 4464, 4543, 54473, 4369, 4464, 4544, 54474, 4369, 4464, 4545, 54475, 4369, 4464, 4546, 54476, 4369, 4465, 0, 54477, 4369, 4465, 4520,
    54478, 4369, 4465, 4521, 54479, 4369, 4465, 4522, 54480, 4369, 4465, 4523, 54481, 4369, 4465, 4524, 54482, 4369, 4465, 4525, 54483, 4369, 4465, 4526,
    54484, 4369, 4465, 4527, 54485, 4369, 4465, 4528, 54486, 4369, 4465, 4529, 54487, 4369, 4465, 4530, 54488, 4369, 4465, 4531, 54489, 4369, 4465, 4532,
    54490, 4369, 4465, 4533, 54491, 4369, 4465, 4534, 54492, 4369, 4465, 4535, 54493, 4369, 4465, 4536, 54494, 4369, 4465, 12612, 54495, 4369, 4465, 4538,
    54496, 4369, 4465, 4539, 54497, 4369, 4465, 4540, 54498, 4369, 4465, 4364, 54499, 4369, 4465, 4542, 54500, 4369, 4465, 4543, 54501, 4369, 4465, 4544,
    54502, 4369, 4465, 4545, 54503, 4369, 4465, 4546, 54504, 4369, 4466, 0, 54505, 4369, 4466, 4520, 54506, 4369, 4466, 4521, 54507, 4369, 4466, 4522,
    54508, 4369, 4466, 4523, 54509, 4369, 4466, 4524, 54510, 4369, 4466, 4525, 54511, 4369, 4466, 4526, 54512, 4369, 4466, 4527, 54513, 4369, 4466, 4528,
    54514, 4369, 4466, 4529, 54515, 4369, 4466, 4530, 54516, 4369, 4466, 4531, 54517, 4369, 4466, 4532, 54518, 4369, 4466, 4533, 54519, 4369, 4466, 4534,
    54520, 4369, 4466, 4535, 54521, 4369, 4466, 4536, 54522, 4369, 4466, 12612, 54523, 4369, 4466, 4538, 54524, 4369, 4466, 4539, 54525, 4369, 4466, 4540,
    54526, 4369, 4466, 4364, 54527, 4369, 4466, 4542, 54528, 4369, 4466, 4543, 54529, 4369, 4466, 4544, 54530, 4369, 4466, 4545, 54531, 4369, 4466, 4546,
    54532, 4369, 4467, 0, 54533, 4369, 4467, 4520, 54534, 4369, 4467, 4521, 54535, 4369, 4467, 4522, 54536, 4369, 4467, 4523, 54537, 4369, 4467, 4524,
    54538, 4369, 4467, 4525, 54539, 4369, 4467, 4526, 54540, 4369, 4467, 4527, 54541, 4369, 4467, 4528, 54542, 4369, 4467, 4529, 54543, 4369, 4467, 4530,
    54544, 4369, 4467, 4531, 54545, 4369, 4467, 4532, 54546, 4369, 4467, 4533, 54547, 4369, 4467, 4534, 54548, 4369, 4467, 4535, 54549, 4369, 4467, 4536,
    54550, 4369, 4467, 12612, 54551, 4369, 4467, 4538, 54552, 4369, 4467, 4539, 54553, 4369, 4467, 4540, 54554, 4369, 4467, 4364, 54555, 4369, 4467, 4542,
    54556, 4369, 4467, 4543, 54557, 4369, 4467, 4544, 54558, 4369, 4467, 4545, 54559, 4369, 4467, 4546, 54560, 4369, 4468, 0, 54561, 4369, 4468, 4520,
    54562, 4369, 4468, 4521, 54563, 4369, 4468, 4522, 54564, 4369, 4468, 4523, 54565, 4369, 4468, 4524, 54566, 4369, 4468, 4525, 54567, 4369, 4468, 4526,
    54568, 4369, 4468, 4527, 54569, 4369, 4468, 4528, 54570, 4369, 4468, 4529, 54571, 4369, 4468, 4530, 54572, 4369, 4468, 4531, 54573, 4369, 4468, 4532,
    54574, 4369, 4468, 4533, 54575, 4369, 4468, 4534, 54576, 4369, 4468, 4535, 54577, 4369, 4468, 4536, 54578, 4369, 4468, 12612, 54579, 4369, 4468, 4538,
    54580, 4369, 4468, 4539, 54581, 4369, 4468, 4540, 54582, 4369, 4468, 4364, 54583, 4369, 4468, 4542, 54584, 4369, 4468, 4543, 54585, 4369, 4468, 4544,
    54586, 4369, 4468, 4545, 54587, 4369, 4468, 4546, 54588, 4369, 4469, 0, 54589, 4369, 4469, 4520, 54590, 4369, 4469, 4521, 54591, 4369, 4469, 4522,
    54592, 4369, 4469, 4523, 54593, 4369, 4469, 4524, 54594, 4369, 4469, 4525, 54595, 4369, 4469, 4526, 54596, 4369, 4469, 4527, 54597, 4369, 4469, 4528,
    54598, 4369, 4469, 4529, 54599, 4369, 4469, 4530, 54600, 4369, 4469, 4531, 54601, 4369, 4469, 4532, 54602, 4369, 4469, 4533, 54603, 4369, 4469, 4534,
    54604, 4369, 4469, 4535, 54605, 4369, 4469, 4536, 54606, 4369, 4469, 12612, 54607, 4369, 4469, 4538, 54608, 4369, 4469, 4539, 54609, 4369, 4469, 4540,
    54610, 4369, 4469, 4364, 54611, 4369, 4469, 4542, 54612, 4369, 4469, 4543, 54613, 4369, 4469, 4544, 54614, 4369, 4469, 4545, 54615, 4369, 4469, 4546,
    54616, 4370, 4449, 0, 54617, 4370, 4449, 4520, 54618, 4370, 4449, 4521, 54619, 4370, 4449, 4522, 54620, 4370, 4449, 4523, 54621, 4370, 4449, 4524,
    54622, 4370, 4449, 4525, 54623, 4370, 4449, 4526, 54624, 4370, 4449, 4527, 54625, 4370, 4449, 4528, 54626, 4370, 4449, 4529, 54627, 4370, 4449, 4530,
    54628, 4370, 4449, 4531, 54629, 4370, 4449, 4532, 54630, 4370, 4449, 4533, 54631, 4370, 4449, 4534, 54632, 4370, 4449, 4535, 54633, 4370, 4449, 4536,
    54634, 4370, 4449, 12612, 54635, 4370, 4449, 4538, 54636, 4370, 4449, 4539, 54637, 4370, 4449, 4540, 54638, 4370, 4449, 4364, 54639, 4370, 4449, 4542,
    54640, 4370, 4449, 4543, 54641, 4370, 4449, 4544, 54642, 4370, 4449, 4545, 54643, 4370, 4449, 4546, 54644, 4370, 4450, 0, 54645, 4370, 4450, 4520,
    54646, 4370, 4450, 4521, 54647, 4370, 4450, 4522, 54648, 4370, 4450, 4523, 54649, 4370, 4450, 4524, 54650, 4370, 4450, 4525, 54651, 4370, 4450, 4526,
    54652, 4370, 4450, 4527, 54653, 4370, 4450, 4528, 54654, 4370, 4450, 4529, 54655, 4370, 4450, 4530, 54656, 4370, 4450, 4531, 54657, 4370, 4450, 4532,
    54658, 4370, 4450, 4533, 54659, 4370, 4450, 4534, 54660, 4370, 4450, 4535, 54661, 4370, 4450, 4536, 54662, 4370, 4450, 12612, 54663, 4370, 4450, 4538,
    54664, 4370, 4450, 4539, 54665, 4370, 4450, 4540, 54666, 4370, 4450, 4364, 54667, 4370, 4450, 4542, 54668, 4370, 4450, 4543, 54669, 4370, 4450, 4544,
    54670, 4370, 4450, 4545, 54671, 4370, 4450, 4546, 54672, 4370, 4451, 0, 54673, 4370, 4451, 4520, 54674, 4370, 4451, 4521, 54675, 4370, 4451, 4522,
    54676, 4370, 4451, 4523, 54677, 4370, 4451, 4524, 54678, 4370, 4451, 4525, 54679, 4370, 4451, 4526, 54680, 4370, 4451, 4527, 54681, 4370, 4451, 4528,
    54682, 4370, 4451, 4529, 54683, 4370, 4451, 4530, 54684, 4370, 4451, 4531, 54685, 4370, 4451, 4532, 54686, 4370, 4451, 4533, 54687, 4370, 4451, 4534,
    54688, 4370, 4451, 4535, 54689, 4370, 4451, 4536, 54690, 4370, 4451, 12612, 54691, 4370, 4451, 4538, 54692, 4370, 4451, 4539, 54693, 4370, 4451, 4540,
    54694, 4370, 4451, 4364, 54695, 4370, 4451, 4542, 54696, 4370, 4451, 4543, 54697, 4370, 4451, 4544, 54698, 4370, 4451, 4545, 54699, 4370, 4451, 4546,
    54700, 4370, 4452, 0, 54701, 4370, 4452, 4520, 54702, 4370, 4452, 4521, 54703, 4370, 4452, 4522, 54704, 4370, 4452, 4523, 54705, 4370, 4452, 4524,
    54706, 4370, 4452, 4525, 54707, 4370, 4452, 4526, 54708, 4370, 4452, 4527, 54709, 4370, 4452, 4528, 54710, 4370, 4452, 4529, 54711, 4370, 4452, 4530,
    54712, 4370, 4452, 4531, 54713, 4370, 4452, 4532, 54714, 4370, 4452, 4533, 54715, 4370, 4452, 4534, 54716, 4370, 4452, 4535, 54717, 4370, 4452, 4536,
    54718, 4370, 4452, 12612, 54719, 4370, 4452, 4538, 54720, 4370, 4452, 4539, 54721, 4370, 4452, 4540, 54722, 4370, 4452, 4364, 54723, 4370, 4452, 4542,
    54724, 4370, 4452, 4543, 54725, 4370, 4452, 4544, 54726, 4370, 4452, 4545, 54727, 4370, 4452, 4546, 54728, 4370, 4453, 0, 54729, 4370, 4453, 4520,
    54730, 4370, 4453, 4521, 54731, 4370, 4453, 4522, 54732, 4370, 4453, 4523, 54733, 4370, 4453, 4524, 54734, 4370, 4453, 4525, 54735, 4370, 4453, 4526,
    54736, 4370, 4453, 4527, 54737, 4370, 4453, 4528, 54738, 4370, 4453, 4529, 54739, 4370, 4453, 4530, 54740, 4370, 4453, 4531, 54741, 4370, 4453, 4532,
    54742, 4370, 4453, 4533, 54743, 4370, 4453, 4534, 54744, 4370, 4453, 4535, 54745, 4370, 4453, 4536, 54746, 4370, 4453, 12612, 54747, 4370, 4453, 4538,
    54748, 4370, 4453, 4539, 54749, 4370, 4453, 4540, 54750, 4370, 4453, 4364, 54751, 4370, 4453, 4542, 54752, 4370, 4453, 4543, 54753, 4370, 4453, 4544,
    54754, 4370, 4453, 4545, 54755, 4370, 4453, 4546, 54756, 4370, 4454, 0, 54757, 4370, 4454, 4520, 54758, 4370, 4454, 4521, 54759, 4370, 4454, 4522,
    54760, 4370, 4454, 4523, 54761, 4370, 4454, 4524, 54762, 4370, 4454, 4525, 54763, 4370, 4454, 4526, 54764, 4370, 4454, 4527, 54765, 4370, 4454, 4528,
    54766, 4370, 4454, 4529, 54767, 4370, 4454, 4530, 54768, 4370, 4454, 4531, 54769, 4370, 4454, 4532, 54770, 4370, 4454, 4533, 54771, 4370, 4454, 4534,
    54772, 4370, 4454, 4535, 54773, 4370, 4454, 4536, 54774, 4370, 4454, 12612, 54775, 4370, 4454, 4538, 54776, 4370, 4454, 4539, 54777, 4370, 4454, 4540,
    54778, 4370, 4454, 4364, 54779, 4370, 4454, 4542, 54780, 4370, 4454, 4543, 54781, 4370, 4454, 4544, 54782, 4370, 4454, 4545, 54783, 4370, 4454, 4546,
    54784, 4370, 4455, 0, 54785, 4370, 4455, 4520, 54786, 4370, 4455, 4521, 54787, 4370, 4455, 4522, 54788, 4370, 4455, 4523, 54789, 4370, 4455, 4524,
    54790, 4370, 4455, 4525, 54791, 4370, 4455, 4526, 54792, 4370, 4455, 4527, 54793, 4370, 4455, 4528, 54794, 4370, 4455, 4529, 54795, 4370, 4455, 4530,
    54796, 4370, 4455, 4531, 54797, 4370, 4455, 4532, 54798, 4370, 4455, 4533, 54799, 4370, 4455, 4534, 54800, 4370, 4455, 4535, 54801, 4370, 4455, 4536,
    54802, 4370, 4455, 12612, 54803, 4370, 4455, 4538, 54804, 4370, 4455, 4539, 54805, 4370, 4455, 4540, 54806, 4370, 4455, 4364, 54807, 4370, 4455, 4542,
    54808, 4370, 4455, 4543, 54809, 4370, 4455, 4544, 54810, 4370, 4455, 4545, 54811, 4370, 4455, 4546, 54812, 4370, 4456, 0, 54813, 4370, 4456, 4520,
    54814, 4370, 4456, 4521, 54815, 4370, 4456, 4522, 54816, 4370, 4456, 4523, 54817, 4370, 4456, 4524, 54818, 4370, 4456, 4525, 54819, 4370, 4456, 4526,
    54820, 4370, 4456, 4527, 54821, 4370, 4456, 4528, 54822, 4370, 4456, 4529, 54823, 4370, 4456, 4530, 54824, 4370, 4456, 4531, 54825, 4370, 4456, 4532,
    54826, 4370, 4456, 4533, 54827, 4370, 4456, 4534, 54828, 4370, 4456, 4535, 54829, 4370, 4456, 4536, 54830, 4370, 4456, 12612, 54831, 4370, 4456, 4538,
    54832, 4370, 4456, 4539, 54833, 4370, 4456, 4540, 54834, 4370, 4456, 4364, 54835, 4370, 4456, 4542, 54836, 4370, 4456, 4543, 54837, 4370, 4456, 4544,
    54838, 4370, 4456, 4545, 54839, 4370, 4456, 4546, 54840, 4370, 4457, 0, 54841, 4370, 4457, 4520, 54842, 4370, 4457, 4521, 54843, 4370, 4457, 4522,
    54844, 4370, 4457, 4523, 54845, 4370, 4457, 4524, 54846, 4370, 4457, 4525, 54847, 4370, 4457, 4526, 54848, 4370, 4457, 4527, 54849, 4370, 4457, 4528,
    54850, 4370, 4457, 4529, 54851, 4370, 4457, 4530, 54852, 4370, 4457, 4531, 54853, 4370, 4457, 4532, 54854, 4370, 4457, 4533, 54855, 4370, 4457, 4534,
    54856, 4370, 4457, 4535, 54857, 4370, 4457, 4536, 54858, 4370, 4457, 12612, 54859, 4370, 4457, 4538, 54860, 4370, 4457, 4539, 54861, 4370, 4457, 4540,
    54862, 4370, 4457, 4364, 54863, 4370, 4457, 4542, 54864, 4370, 4457, 4543, 54865, 4370, 4457, 4544, 54866, 4370, 4457, 4545, 54867, 4370, 4457, 4546,
    54868, 4370, 4458, 0, 54869, 4370, 4458, 4520, 54870, 4370, 4458, 4521, 54871, 4370, 4458, 4522, 54872, 4370, 4458, 4523, 54873, 4370, 4458, 4524,
    54874, 4370, 4458, 4525, 54875, 4370, 4458, 4526, 54876, 4370, 4458, 4527, 54877, 4370, 4458, 4528, 54878, 4370, 4458, 4529, 54879, 4370, 4458, 4530,
    54880, 4370, 4458, 4531, 54881, 4370, 4458, 4532, 54882, 4370, 4458, 4533, 54883, 4370, 4458, 4534, 54884, 4370, 4458, 4535, 54885, 4370, 4458, 4536,
    54886, 4370, 4458, 12612, 54887, 4370, 4458, 4538, 54888, 4370, 4458, 4539, 54889, 4370, 4458, 4540, 54890, 4370, 4458, 4364, 54891, 4370, 4458, 4542,
    54892, 4370, 4458, 4543, 54893, 4370, 4458, 4544, 54894, 4370, 4458, 4545, 54895, 4370, 4458, 4546, 54896, 4370, 4459, 0, 54897, 4370, 4459, 4520,
    54898, 4370, 4459, 4521, 54899, 4370, 4459, 4522, 54900, 4370, 4459, 4523, 54901, 4370, 4459, 4524, 54902, 4370, 4459, 4525, 54903, 4370, 4459, 4526,
    54904, 4370, 4459, 4527, 54905, 4370, 4459, 4528, 54906, 4370, 4459, 4529, 54907, 4370, 4459, 4530, 54908, 4370, 4459, 4531, 54909, 4370, 4459, 4532,
    54910, 4370, 4459, 4533, 54911, 4370, 4459, 4534, 54912, 4370, 4459, 4535, 54913, 4370, 4459, 4536, 54914, 4370, 4459, 12612, 54915, 4370, 4459, 4538,
    54916, 4370, 4459, 4539, 54917, 4370, 4459, 4540, 54918, 4370, 4459, 4364, 54919, 4370, 4459, 4542, 54920, 4370, 4459, 4543, 54921, 4370, 4459, 4544,
    54922, 4370, 4459, 4545, 54923, 4370, 4459, 4546, 54924, 4370, 4460, 0, 54925, 4370, 4460, 4520, 54926, 4370, 4460, 4521, 54927, 4370, 4460, 4522,
    54928, 4370, 4460, 4523, 54929, 4370, 4460, 4524, 54930, 4370, 4460, 4525, 54931, 4370, 4460, 4526, 54932, 4370, 4460, 4527, 54933, 4370, 4460, 4528,
    54934, 4370, 4460, 4529, 54935, 4370, 4460, 4530, 54936, 4370, 4460, 4531, 54937, 4370, 4460, 4532, 54938, 4370, 4460, 4533, 54939, 4370, 4460, 4534,
    54940, 4370, 4460, 4535, 54941, 4370, 4460, 4536, 54942, 4370, 4460, 12612, 54943, 4370, 4460, 4538, 54944, 4370, 4460, 4539, 54945, 4370, 4460, 4540,
    54946, 4370, 4460, 4364, 54947, 4370, 4460, 4542, 54948, 4370, 4460, 4543, 54949, 4370, 4460, 4544, 54950, 4370, 4460, 4545, 54951, 4370, 4460, 4546,
    54952, 4370, 4461, 0, 54953, 4370, 4461, 4520, 54954, 4370, 4461, 4521, 54955, 4370, 4461, 4522, 54956, 4370, 4461, 4523, 54957, 4370, 4461, 4524,
    54958, 4370, 4461, 4525, 54959, 4370, 4461, 4526, 54960, 4370, 4461, 4527, 54961, 4370, 4461, 4528, 54962, 4370, 4461, 4529, 54963, 4370, 4461, 4530,
    54964, 4370, 4461, 4531, 54965, 4370, 4461, 4532, 54966, 4370, 4461, 4533, 54967, 4370, 4461, 4534, 54968, 4370, 4461, 4535, 54969, 4370, 4461, 4536,
    54970, 4370, 4461, 12612, 54971, 4370, 4461, 4538, 54972, 4370, 4461, 4539, 54973, 4370, 4461, 4540, 54974, 4370, 4461, 4364, 54975, 4370, 4461, 4542,
    54976, 4370, 4461, 4543, 54977, 4370, 4461, 4544, 54978, 4370, 4461, 4545, 54979, 4370, 4461, 4546, 54980, 4370, 4462, 0, 54981, 4370, 4462, 4520,
    54982, 4370, 4462, 4521, 54983, 4370, 4462, 4522, 54984, 4370, 4462, 4523, 54985, 4370, 4462, 4524, 54986, 4370, 4462, 4525, 54987, 4370, 4462, 4526,
    54988, 4370, 4462, 4527, 54989, 4370, 4462, 4528, 54990, 4370, 4462, 4529, 54991, 4370, 4462, 4530, 54992, 4370, 4462, 4531, 54993, 4370, 4462, 4532,
    54994, 4370, 4462, 4533, 54995, 4370, 4462, 4534, 54996, 4370, 4462, 4535, 54997, 4370, 4462, 4536, 54998, 4370, 4462, 12612, 54999, 4370, 4462, 4538,
    55000, 4370, 4462, 4539, 55001, 4370, 4462, 4540, 55002, 4370, 4462, 4364, 55003, 4370, 4462, 4542, 55004, 4370, 4462, 4543, 55005, 4370, 4462, 4544,
    55006, 4370, 4462, 4545, 55007, 4370, 4462, 4546, 55008, 4370, 4463, 0, 55009, 4370, 4463, 4520, 55010, 4370, 4463, 4521, 55011, 4370, 4463, 4522,
    55012, 4370, 4463, 4523, 55013, 4370, 4463, 4524, 55014, 4370, 4463, 4525, 55015, 4370, 4463, 4526, 55016, 4370, 4463, 4527, 55017, 4370, 4463, 4528,
    55018, 4370, 4463, 4529, 55019, 4370, 4463, 4530, 55020, 4370, 4463, 4531, 55021, 4370, 4463, 4532, 55022, 4370, 4463, 4533, 55023, 4370, 4463, 4534,
    55024, 4370, 4463, 4535, 55025, 4370, 4463, 4536, 55026, 4370, 4463, 12612, 55027, 4370, 4463, 4538, 55028, 4370, 4463, 4539, 55029, 4370, 4463, 4540,
    55030, 4370, 4463, 4364, 55031, 4370, 4463, 4542, 55032, 4370, 4463, 4543, 55033, 4370, 4463, 4544, 55034, 4370, 4463, 4545, 55035, 4370, 4463, 4546,
    55036, 4370, 4464, 0, 55037, 4370, 4464, 4520, 55038, 4370, 4464, 4521, 55039, 4370, 4464, 4522, 55040, 4370, 4464, 4523, 55041, 4370, 4464, 4524,
    55042, 4370, 4464, 4525, 55043, 4370, 4464, 4526, 55044, 4370, 4464, 4527, 55045, 4370, 4464, 4528, 55046, 4370, 4464, 4529, 55047, 4370, 4464, 4530,
    55048, 4370, 4464, 4531, 55049, 4370, 4464, 4532, 55050, 4370, 4464, 4533, 55051, 4370, 4464, 4534, 55052, 4370, 4464, 4535, 55053, 4370, 4464, 4536,
    55054, 4370, 4464, 12612, 55055, 4370, 4464, 4538, 55056, 4370, 4464, 4539, 55057, 4370, 4464, 4540, 55058, 4370, 4464, 4364, 55059, 4370, 4464, 4542,
    55060, 4370, 4464, 4543, 55061, 4370, 4464, 4544, 55062, 4370, 4464, 4545, 55063, 4370, 4464, 4546, 55064, 4370, 4465, 0, 55065, 4370, 4465, 4520,
    55066, 4370, 4465, 4521, 55067, 4370, 4465, 4522, 55068, 4370, 4465, 4523, 55069, 4370, 4465, 4524, 55070, 4370, 4465, 4525, 55071, 4370, 4465, 4526,
    55072, 4370, 4465, 4527, 55073, 4370, 4465, 4528, 55074, 4370, 4465, 4529, 55075, 4370, 4465, 4530, 55076, 4370, 4465, 4531, 55077, 4370, 4465, 4532,
    55078, 4370, 4465, 4533, 55079, 4370, 4465, 4534, 55080, 4370, 4465, 4535, 55081, 4370, 4465, 4536, 55082, 4370, 4465, 12612, 55083, 4370, 4465, 4538,
    55084, 4370, 4465, 4539, 55085, 4370, 4465, 4540, 55086, 4370, 4465, 4364, 55087, 4370, 4465, 4542, 55088, 4370, 4465, 4543, 55089, 4370, 4465, 4544,
    55090, 4370, 4465, 4545, 55091, 4370, 4465, 4546, 55092, 4370, 4466, 0, 55093, 4370, 4466, 4520, 55094, 4370, 4466, 4521, 55095, 4370, 4466, 4522,
    55096, 4370, 4466, 4523, 55097, 4370, 4466, 4524, 55098, 4370, 4466, 4525, 55099, 4370, 4466, 4526, 55100, 4370, 4466, 4527, 55101, 4370, 4466, 4528,
    55102, 4370, 4466, 4529, 55103, 4370, 4466, 4530, 55104, 4370, 4466, 4531, 55105, 4370, 4466, 4532, 55106, 4370, 4466, 4533, 55107, 4370, 4466, 4534,
    55108, 4370, 4466, 4535, 55109, 4370, 4466, 4536, 55110, 4370, 4466, 12612, 55111, 4370, 4466, 4538, 55112, 4370, 4466, 4539, 55113, 4370, 4466, 4540,
    55114, 4370, 4466, 4364, 55115, 4370, 4466, 4542, 55116, 4370, 4466, 4543, 55117, 4370, 4466, 4544, 55118, 4370, 4466, 4545, 55119, 4370, 4466, 4546,
    55120, 4370, 4467, 0, 55121, 4370, 4467, 4520, 55122, 4370, 4467, 4521, 55123, 4370, 4467, 4522, 55124, 4370, 4467, 4523, 55125, 4370, 4467, 4524,
    55126, 4370, 4467, 4525, 55127, 4370, 4467, 4526, 55128, 4370, 4467, 4527, 55129, 4370, 4467, 4528, 55130, 4370, 4467, 4529, 55131, 4370, 4467, 4530,
    55132, 4370, 4467, 4531, 55133, 4370, 4467, 4532, 55134, 4370, 4467, 4533, 55135, 4370, 4467, 4534, 55136, 4370, 4467, 4535, 55137, 4370, 4467, 4536,
    55138, 4370, 4467, 12612, 55139, 4370, 4467, 4538, 55140, 4370, 4467, 4539, 55141, 4370, 4467, 4540, 55142, 4370, 4467, 4364, 55143, 4370, 4467, 4542,
    55144, 4370, 4467, 4543, 55145, 4370, 4467, 4544, 55146, 4370, 4467, 4545, 55147, 4370, 4467, 4546, 55148, 4370, 4468, 0, 55149, 4370, 4468, 4520,
    55150, 4370, 4468, 4521, 55151, 4370, 4468, 4522, 55152, 4370, 4468, 4523, 55153, 4370, 4468, 4524, 55154, 4370, 4468, 4525, 55155, 4370, 4468, 4526,
    55156, 4370, 4468, 4527, 55157, 4370, 4468, 4528, 55158, 4370, 4468, 4529, 55159, 4370, 4468, 4530, 55160, 4370, 4468, 4531, 55161, 4370, 4468, 4532,
    55162, 4370, 4468, 4533, 55163, 4370, 4468, 4534, 55164, 4370, 4468, 4535, 55165, 4370, 4468, 4536, 55166, 4370, 4468, 12612, 55167, 4370, 4468, 4538,
    55168, 4370, 4468, 4539, 55169, 4370, 4468, 4540, 55170, 4370, 4468, 4364, 55171, 4370, 4468, 4542, 55172, 4370, 4468, 4543, 55173, 4370, 4468, 4544,
    55174, 4370, 4468, 4545, 55175, 4370, 4468, 4546, 55176, 4370, 4469, 0, 55177, 4370, 4469, 4520, 55178, 4370, 4469, 4521, 55179, 4370, 4469, 4522,
    55180, 4370, 4469, 4523, 55181, 4370, 4469, 4524, 55182, 4370, 4469, 4525, 55183, 4370, 4469, 4526, 55184, 4370, 4469, 4527, 55185, 4370, 4469, 4528,
    55186, 4370, 4469, 4529, 55187, 4370, 4469, 4530, 55188, 4370, 4469, 4531, 55189, 4370, 4469, 4532, 55190, 4370, 4469, 4533, 55191, 4370, 4469, 4534,
    55192, 4370, 4469, 4535, 55193, 4370, 4469, 4536, 55194, 4370, 4469, 12612, 55195, 4370, 4469, 4538, 55196, 4370, 4469, 4539, 55197, 4370, 4469, 4540,
    55198, 4370, 4469, 4364, 55199, 4370, 4469, 4542, 55200, 4370, 4469, 4543, 55201, 4370, 4469, 4544, 55202, 4370, 4469, 4545, 55203, 4370, 4469, 4546,
    0, 0, 0, 0
};

static void InitKoreanCodes() {
    static bool init=false;
    if (init)
        return;
    
    init=true;
    wstring w;
    long i = 0;
    while (koreans[i] != 0) {
        w = koreans[i];
        hangulutf8[koreans[i]] = w;
        i++;
    }
    
    i = 0;
    while (jamo[i] != 0) {
        w = jamo[i + 1];
        w += jamo[i + 2];
        if (jamo[i + 3])
            w += jamo[i + 3];
        hanguljamo[jamo[i]] = w;
        hangultojamo[w] = jamo[i];
        i += 4;
    }
    
    //Korean consonants
    consonants[4352] = "ᄀ";
    consonants[4353] = "ᄁ";
    consonants[4354] = "ᄂ";
    consonants[4355] = "ᄃ";
    consonants[4356] = "ᄄ";
    consonants[4357] = "ᄅ";
    consonants[4358] = "ᄆ";
    consonants[4359] = "ᄇ";
    consonants[4360] = "ᄈ";
    consonants[4361] = "ᄉ";
    consonants[4362] = "ᄊ";
    consonants[4363] = "ᄋ";
    consonants[4364] = "ᄌ";
    consonants[4365] = "ᄍ";
    consonants[4366] = "ᄎ";
    consonants[4367] = "ᄏ";
    consonants[4368] = "ᄐ";
    consonants[4369] = "ᄑ";
    consonants[4370] = "ᄒ";
    consonants[4371] = "ᄓ";
    consonants[4372] = "ᄔ";
    consonants[4373] = "ᄕ";
    consonants[4374] = "ᄖ";
    consonants[4375] = "ᄗ";
    consonants[4376] = "ᄘ";
    consonants[4377] = "ᄙ";
    consonants[4378] = "ᄚ";
    consonants[4379] = "ᄛ";
    consonants[4380] = "ᄜ";
    consonants[4381] = "ᄝ";
    consonants[4382] = "ᄞ";
    consonants[4383] = "ᄟ";
    consonants[4384] = "ᄠ";
    consonants[4385] = "ᄡ";
    consonants[4386] = "ᄢ";
    consonants[4387] = "ᄣ";
    consonants[4388] = "ᄤ";
    consonants[4389] = "ᄥ";
    consonants[4390] = "ᄦ";
    consonants[4391] = "ᄧ";
    consonants[4392] = "ᄨ";
    consonants[4393] = "ᄩ";
    consonants[4394] = "ᄪ";
    consonants[4395] = "ᄫ";
    consonants[4396] = "ᄬ";
    consonants[4397] = "ᄭ";
    consonants[4398] = "ᄮ";
    consonants[4399] = "ᄯ";
    consonants[4400] = "ᄰ";
    consonants[4401] = "ᄱ";
    consonants[4402] = "ᄲ";
    consonants[4403] = "ᄳ";
    consonants[4404] = "ᄴ";
    consonants[4405] = "ᄵ";
    consonants[4406] = "ᄶ";
    consonants[4407] = "ᄷ";
    consonants[4408] = "ᄸ";
    consonants[4409] = "ᄹ";
    consonants[4410] = "ᄺ";
    consonants[4411] = "ᄻ";
    consonants[4412] = "ᄼ";
    consonants[4413] = "ᄽ";
    consonants[4414] = "ᄾ";
    consonants[4415] = "ᄿ";
    consonants[4416] = "ᅀ";
    consonants[4417] = "ᅁ";
    consonants[4418] = "ᅂ";
    consonants[4419] = "ᅃ";
    consonants[4420] = "ᅄ";
    consonants[4421] = "ᅅ";
    consonants[4422] = "ᅆ";
    consonants[4423] = "ᅇ";
    consonants[4424] = "ᅈ";
    consonants[4425] = "ᅉ";
    consonants[4426] = "ᅊ";
    consonants[4427] = "ᅋ";
    consonants[4428] = "ᅌ";
    consonants[4429] = "ᅍ";
    consonants[4430] = "ᅎ";
    consonants[4431] = "ᅏ";
    consonants[4432] = "ᅐ";
    consonants[4433] = "ᅑ";
    consonants[4434] = "ᅒ";
    consonants[4435] = "ᅓ";
    consonants[4436] = "ᅔ";
    consonants[4437] = "ᅕ";
    consonants[4438] = "ᅖ";
    consonants[4439] = "ᅗ";
    consonants[4440] = "ᅘ";
    consonants[4441] = "ᅙ";
    
    consonants[4520] = "ᆨ";
    consonants[4521] = "ᆩ";
    consonants[4522] = "ᆪ";
    consonants[4523] = "ᆫ";
    consonants[4524] = "ᆬ";
    consonants[4525] = "ᆭ";
    consonants[4526] = "ᆮ";
    consonants[4527] = "ᆯ";
    consonants[4528] = "ᆰ";
    consonants[4529] = "ᆱ";
    consonants[4530] = "ᆲ";
    consonants[4531] = "ᆳ";
    consonants[4532] = "ᆴ";
    consonants[4533] = "ᆵ";
    consonants[4534] = "ᆶ";
    consonants[4535] = "ᆷ";
    consonants[4536] = "ᆸ";
    consonants[4537] = "ᆹ";
    consonants[4538] = "ᆺ";
    consonants[4539] = "ᆻ";
    consonants[4540] = "ᆼ";
    consonants[4541] = "ᆽ";
    consonants[4542] = "ᆾ";
    consonants[4543] = "ᆿ";
    consonants[4544] = "ᇀ";
    consonants[4545] = "ᇁ";
    consonants[4546] = "ᇂ";
    consonants[4547] = "ᇃ";
    consonants[4548] = "ᇄ";
    consonants[4549] = "ᇅ";
    consonants[4550] = "ᇆ";
    consonants[4551] = "ᇇ";
    consonants[4552] = "ᇈ";
    consonants[4553] = "ᇉ";
    consonants[4554] = "ᇊ";
    consonants[4555] = "ᇋ";
    consonants[4556] = "ᇌ";
    consonants[4557] = "ᇍ";
    consonants[4558] = "ᇎ";
    consonants[4559] = "ᇏ";
    consonants[4560] = "ᇐ";
    consonants[4561] = "ᇑ";
    consonants[4562] = "ᇒ";
    consonants[4563] = "ᇓ";
    consonants[4564] = "ᇔ";
    consonants[4565] = "ᇕ";
    consonants[4566] = "ᇖ";
    consonants[4567] = "ᇗ";
    consonants[4568] = "ᇘ";
    consonants[4569] = "ᇙ";
    consonants[4570] = "ᇚ";
    consonants[4571] = "ᇛ";
    consonants[4572] = "ᇜ";
    consonants[4573] = "ᇝ";
    consonants[4574] = "ᇞ";
    consonants[4575] = "ᇟ";
    consonants[4576] = "ᇠ";
    consonants[4577] = "ᇡ";
    consonants[4578] = "ᇢ";
    consonants[4579] = "ᇣ";
    consonants[4580] = "ᇤ";
    consonants[4581] = "ᇥ";
    consonants[4582] = "ᇦ";
    consonants[4583] = "ᇧ";
    consonants[4584] = "ᇨ";
    consonants[4585] = "ᇩ";
    consonants[4586] = "ᇪ";
    consonants[4587] = "ᇫ";
    consonants[4588] = "ᇬ";
    consonants[4589] = "ᇭ";
    consonants[4590] = "ᇮ";
    consonants[4591] = "ᇯ";
    consonants[4592] = "ᇰ";
    consonants[4593] = "ᇱ";
    consonants[4594] = "ᇲ";
    consonants[4595] = "ᇳ";
    consonants[4596] = "ᇴ";
    consonants[4597] = "ᇵ";
    consonants[4598] = "ᇶ";
    consonants[4599] = "ᇷ";
    consonants[4600] = "ᇸ";
    consonants[4601] = "ᇹ";
    
    consonants[12593] = "ㄱ";
    consonants[12594] = "ㄲ";
    consonants[12595] = "ㄳ";
    consonants[12596] = "ㄴ";
    consonants[12597] = "ㄵ";
    consonants[12598] = "ㄶ";
    consonants[12599] = "ㄷ";
    consonants[12600] = "ㄸ";
    consonants[12601] = "ㄹ";
    consonants[12602] = "ㄺ";
    consonants[12603] = "ㄻ";
    consonants[12604] = "ㄼ";
    consonants[12605] = "ㄽ";
    consonants[12606] = "ㄾ";
    consonants[12607] = "ㄿ";
    consonants[12608] = "ㅀ";
    consonants[12609] = "ㅁ";
    consonants[12610] = "ㅂ";
    consonants[12611] = "ㅃ";
    consonants[12612] = "ㅄ";
    consonants[12613] = "ㅅ";
    consonants[12614] = "ㅆ";
    consonants[12615] = "ㅇ";
    consonants[12616] = "ㅈ";
    consonants[12617] = "ㅉ";
    consonants[12618] = "ㅊ";
    consonants[12619] = "ㅋ";
    consonants[12620] = "ㅌ";
    consonants[12621] = "ㅍ";
    consonants[12622] = "ㅎ";
    
    //Korean vowels
    vowels[12623] = "ㅏ";
    vowels[12624] = "ㅐ";
    vowels[12625] = "ㅑ";
    vowels[12626] = "ㅒ";
    vowels[12627] = "ㅓ";
    vowels[12628] = "ㅔ";
    vowels[12629] = "ㅕ";
    vowels[12630] = "ㅖ";
    vowels[12631] = "ㅗ";
    vowels[12632] = "ㅘ";
    vowels[12633] = "ㅙ";
    vowels[12634] = "ㅚ";
    vowels[12635] = "ㅛ";
    vowels[12636] = "ㅜ";
    vowels[12637] = "ㅝ";
    vowels[12638] = "ㅞ";
    vowels[12639] = "ㅟ";
    vowels[12640] = "ㅠ";
    vowels[12641] = "ㅡ";
    vowels[12642] = "ㅢ";
    vowels[12643] = "ㅣ";
    
    vowels[4449] = "ᅡ";
    vowels[4450] = "ᅢ";
    vowels[4451] = "ᅣ";
    vowels[4452] = "ᅤ";
    vowels[4453] = "ᅥ";
    vowels[4454] = "ᅦ";
    vowels[4455] = "ᅧ";
    vowels[4456] = "ᅨ";
    vowels[4457] = "ᅩ";
    vowels[4458] = "ᅪ";
    vowels[4459] = "ᅫ";
    vowels[4460] = "ᅬ";
    vowels[4461] = "ᅭ";
    vowels[4462] = "ᅮ";
    vowels[4463] = "ᅯ";
    vowels[4464] = "ᅰ";
    vowels[4465] = "ᅱ";
    vowels[4466] = "ᅲ";
    vowels[4467] = "ᅳ";
    vowels[4468] = "ᅴ";
    vowels[4469] = "ᅵ";
    vowels[4470] = "ᅶ";
    vowels[4471] = "ᅷ";
    vowels[4472] = "ᅸ";
    vowels[4473] = "ᅹ";
    vowels[4474] = "ᅺ";
    vowels[4475] = "ᅻ";
    vowels[4476] = "ᅼ";
    vowels[4477] = "ᅽ";
    vowels[4478] = "ᅾ";
    vowels[4479] = "ᅿ";
    vowels[4480] = "ᆀ";
    vowels[4481] = "ᆁ";
    vowels[4482] = "ᆂ";
    vowels[4483] = "ᆃ";
    vowels[4484] = "ᆄ";
    vowels[4485] = "ᆅ";
    vowels[4486] = "ᆆ";
    vowels[4487] = "ᆇ";
    vowels[4488] = "ᆈ";
    vowels[4489] = "ᆉ";
    vowels[4490] = "ᆊ";
    vowels[4491] = "ᆋ";
    vowels[4492] = "ᆌ";
    vowels[4493] = "ᆍ";
    vowels[4494] = "ᆎ";
    vowels[4495] = "ᆏ";
    vowels[4496] = "ᆐ";
    vowels[4497] = "ᆑ";
    vowels[4498] = "ᆒ";
    vowels[4499] = "ᆓ";
    vowels[4500] = "ᆔ";
    vowels[4501] = "ᆕ";
    vowels[4502] = "ᆖ";
    vowels[4503] = "ᆗ";
    vowels[4504] = "ᆘ";
    vowels[4505] = "ᆙ";
    vowels[4506] = "ᆚ";
    vowels[4507] = "ᆛ";
    vowels[4508] = "ᆜ";
    vowels[4509] = "ᆝ";
    vowels[4510] = "ᆞ";
    vowels[4511] = "ᆟ";
    vowels[4512] = "ᆠ";
    vowels[4513] = "ᆡ";
    vowels[4514] = "ᆢ";
    
    hangulconvert[4520] = 4352; //ᄀ
    hangulconvert[4521] = 4353; //ᄁ
    hangulconvert[4523] = 4354; //ᄂ
    hangulconvert[4526] = 4355; //ᄃ
    hangulconvert[4527] = 4357; //ᄅ
    hangulconvert[4535] = 4358; //ᄆ
    hangulconvert[4536] = 4359; //ᄇ
    hangulconvert[4538] = 4361; //ᄉ
    hangulconvert[4539] = 4362; //ᄊ
    hangulconvert[4540] = 4363; //ᄋ
    hangulconvert[4541] = 4364; //ᄌ
    hangulconvert[4542] = 4366; //ᄎ
    hangulconvert[4543] = 4367; //ᄏ
    hangulconvert[4544] = 4368; //ᄐ
    hangulconvert[4545] = 4369; //ᄑ
    hangulconvert[4546] = 4370; //ᄒ
    hangulconvert[4549] = 4371; //ᄓ
    hangulconvert[4550] = 4373; //ᄕ
    hangulconvert[4554] = 4375; //ᄗ
    hangulconvert[4557] = 4376; //ᄘ
    hangulconvert[4560] = 4377; //ᄙ
    hangulconvert[4569] = 4378; //ᄚ
    hangulconvert[4572] = 4380; //ᄜ
    hangulconvert[4578] = 4381; //ᄝ
    hangulconvert[4580] = 4394; //ᄪ
    hangulconvert[4582] = 4395; //ᄫ
    hangulconvert[4583] = 4397; //ᄭ
    hangulconvert[4584] = 4399; //ᄯ
    hangulconvert[4585] = 4400; //ᄰ
    hangulconvert[4586] = 4402; //ᄲ
    hangulconvert[4587] = 4416; //ᅀ
    hangulconvert[4588] = 4417; //ᅁ
    hangulconvert[4592] = 4428; //ᅌ
    hangulconvert[4595] = 4438; //ᅖ
    hangulconvert[4596] = 4439; //ᅗ
    hangulconvert[4601] = 4441; //ᅙ
    
    hangulconvert[12593] = 4352; //ᄀ
    hangulconvert[12594] = 4353; //ᄁ
    hangulconvert[12596] = 4354; //ᄂ
    hangulconvert[12599] = 4355; //ᄃ
    hangulconvert[12600] = 4356; //ᄄ
    hangulconvert[12601] = 4357; //ᄅ
    hangulconvert[12609] = 4358; //ᄆ
    hangulconvert[12610] = 4359; //ᄇ
    hangulconvert[12611] = 4360; //ᄈ
    hangulconvert[12613] = 4361; //ᄉ
    hangulconvert[12614] = 4362; //ᄊ
    hangulconvert[12615] = 4363; //ᄋ
    hangulconvert[12616] = 4364; //ᄌ
    hangulconvert[12617] = 4365; //ᄍ
    hangulconvert[12618] = 4366; //ᄎ
    hangulconvert[12619] = 4367; //ᄏ
    hangulconvert[12620] = 4368; //ᄐ
    hangulconvert[12621] = 4369; //ᄑ
    hangulconvert[12622] = 4370; //ᄒ
    
    hangulconvert[12623] = 4449; //ᅡ
    hangulconvert[12624] = 4450; //ᅢ
    hangulconvert[12625] = 4451; //ᅣ
    hangulconvert[12626] = 4452; //ᅤ
    hangulconvert[12627] = 4453; //ᅥ
    hangulconvert[12628] = 4454; //ᅦ
    hangulconvert[12629] = 4455; //ᅧ
    hangulconvert[12630] = 4456; //ᅨ
    hangulconvert[12631] = 4457; //ᅩ
    hangulconvert[12632] = 4458; //ᅪ
    hangulconvert[12633] = 4459; //ᅫ
    hangulconvert[12634] = 4460; //ᅬ
    hangulconvert[12635] = 4461; //ᅭ
    hangulconvert[12636] = 4462; //ᅮ
    hangulconvert[12637] = 4463; //ᅯ
    hangulconvert[12638] = 4464; //ᅰ
    hangulconvert[12639] = 4465; //ᅱ
    hangulconvert[12640] = 4466; //ᅲ
    hangulconvert[12641] = 4467; //ᅳ
    hangulconvert[12642] = 4468; //ᅴ
    hangulconvert[12643] = 4469; //ᅵ
    hangulconvert[12602] = 4528; //ᆰ
    hangulconvert[12603] = 4529; //ᆱ
    hangulconvert[12604] = 4530; //ᆲ
    hangulconvert[12605] = 4531; //ᆳ
    hangulconvert[12606] = 4532; //ᆴ
    hangulconvert[12607] = 4533; //ᆵ
    hangulconvert[12608] = 4534; //ᆶ
    hangulconvert[12612] = 4385; //ᄡ
    hangulconvert[12595] = 4522; //ㄳ
    hangulconvert[12597] = 4524; //ㄵ
    hangulconvert[12598] = 4525; //ㄶ
    
    
    transliteration[4521] = "kk"; //ᆩ
    transliteration[4526] = "d"; //ᆮ
    transliteration[4542] = "ch"; //ᆾ
    transliteration[4543] = "k"; //ᆿ
    transliteration[4544] = "t"; //ᇀ
    transliteration[4545] = "p"; //ᇁ
    transliteration[4546] = "h"; //ᇂ
    transliteration[4523] = "n"; //ᄂ
    transliteration[4540] = "-/ng"; //ᄋ
    transliteration[4520] = "g/k"; //ᄀ
    transliteration[4527] = "r/l"; //ᄅ
    transliteration[4535] = "m"; //ᄆ
    transliteration[4538] = "s"; //ᄉ
    transliteration[4539] = "ss"; //ᄊ
    transliteration[4536] = "b"; //ᄇ
    transliteration[12612] = "/bs"; //ᄡ
    transliteration[4352] = "g/k"; //ᄀ
    transliteration[4353] = "kk"; //ᄁ
    transliteration[4354] = "n"; //ᄂ
    transliteration[4355] = "d"; //ᄃ
    transliteration[4356] = "tt"; //ᄄ
    transliteration[4357] = "r/l"; //ᄅ
    transliteration[4358] = "m"; //ᄆ
    transliteration[4359] = "b"; //ᄇ
    transliteration[4360] = "bb/pp"; //ᄈ
    transliteration[4361] = "s"; //ᄉ
    transliteration[4362] = "ss"; //ᄊ
    transliteration[4363] = "-/ng"; //ᄋ
    transliteration[4364] = "j"; //ᄌ
    transliteration[4365] = "jj"; //ᄍ
    transliteration[4366] = "ch"; //ᄎ
    transliteration[4367] = "k"; //ᄏ
    transliteration[4368] = "t"; //ᄐ
    transliteration[4369] = "p"; //ᄑ
    transliteration[4370] = "h"; //ᄒ
    transliteration[4385] = "/bs"; //ᄡ
    transliteration[4449] = "a"; //ᅡ
    transliteration[4450] = "ae"; //ᅢ
    transliteration[4451] = "ya"; //ᅣ
    transliteration[4452] = "yae"; //ᅤ
    transliteration[4453] = "eo"; //ᅥ
    transliteration[4454] = "e"; //ᅦ
    transliteration[4455] = "yeo"; //ᅧ
    transliteration[4456] = "ye"; //ᅨ
    transliteration[4457] = "o"; //ᅩ
    transliteration[4458] = "wa"; //ᅪ
    transliteration[4459] = "wae"; //ᅫ
    transliteration[4460] = "oe"; //ᅬ
    transliteration[4461] = "yo"; //ᅭ
    transliteration[4462] = "u"; //ᅮ
    transliteration[4463] = "weo"; //ᅯ
    transliteration[4464] = "we"; //ᅰ
    transliteration[4465] = "ui"; //ᅱ
    transliteration[4466] = "yu"; //ᅲ
    transliteration[4467] = "eu"; //ᅳ
    transliteration[4468] = "eui"; //ᅴ
    transliteration[4469] = "i"; //ᅵ
    transliteration[4528] = "/lg"; //ᆰ
    transliteration[4529] = "/lm"; //ᆱ
    transliteration[4530] = "/lb"; //ᆲ
    transliteration[4531] = "/ls"; //ᆳ
    transliteration[4532] = "/lt"; //ᆴ
    transliteration[4533] = "/lp"; //ᆵ
    transliteration[4534] = "/lh"; //ᆶ
    transliteration[4522] = "/gs"; //ᆪ
    transliteration[4524] = "/nj"; //ᆬ
    transliteration[4525] = "/nh"; //ᆭ
    transliteration[12595] = "/gs"; //ㄳ
    transliteration[12597] = "/nj"; //ㄵ
    transliteration[12598] = "/nh"; //ㄶ
}

bool c_is_hangul(string& s) {
    InitKoreanCodes();
    
	TAMGUCHAR c;
    c_utf8_to_unicode(USTR(s), c);
    if (hangulutf8.check(c) || hanguljamo.check(c))
        return true;
    return false;
}

bool s_is_hangul(wstring& w) {
    InitKoreanCodes();
    
    for (long i = 0; i < w.size(); i++) {
        if (!hangulutf8.check(w[i]) && !hanguljamo.check(w[i]))
            return false;
    }
    return true;
}

bool c_is_hangul(TAMGUCHAR c) {
    InitKoreanCodes();
    
    if (hangulutf8.check(c) || hanguljamo.check(c))
        return true;
    return false;
}

string c_translate(TAMGUCHAR c) {
    InitKoreanCodes();
    
    if (transliteration.check(c))
        return transliteration[c];
    return "";
}

string c_translate(unsigned char* m, long& i) {
    InitKoreanCodes();
    
	TAMGUCHAR c;
    i += c_utf8_to_unicode(m + i, c);
    if (transliteration.check(c))
        return transliteration[c];
    return "";
}

bool c_is_hangul(unsigned char* m, long& i) {
    InitKoreanCodes();
    
	TAMGUCHAR c;
    i += c_utf8_to_unicode(m + i, c);
    if (!hangulutf8.check(c) && !hanguljamo.check(c))
        return false;
    return true;
}


bool s_is_jamo(wstring& w) {
    InitKoreanCodes();
    
    for (long i = 0; i < w.size(); i++) {
        if (!hanguljamo.check(w[i]))
            return false;
    }
    
    return true;
}

wstring s_split_jamo(wstring& w) {
    InitKoreanCodes();
    
    wstring j;
    for (long i = 0; i < w.size(); i++) {
        if (hanguljamo.check(w[i]))
            j += hanguljamo[w[i]];
        else
            j += w[i];
    }
    return j;
}

wstring s_combine_jamo(wstring& w) {
    InitKoreanCodes();
    wstring res, h2, h3;
    long sz = w.size();
    
    for (long i = 0; i < sz; i++) {
        h2 = w[i];
        if (i < sz - 1)
            h2 += w[i+1];
        if (i < sz - 2)
            h3 = h2 + w[i+2];
        if (hangultojamo.find(h3) != hangultojamo.end()) {
            res += hangultojamo[h3];
            i += 2;
        }
        else {
            if (hangultojamo.find(h2) != hangultojamo.end()) {
                res += hangultojamo[h2];
                i++;
            }
            else
                res += w[i];
        }
    }
    return res;
}

wstring s_hangul_normalize(wstring& w) {
    InitKoreanCodes();
    
    wstring res;
    for (long i = 0; i < w.size(); i++) {
        if (hangulconvert.check(w[i]))
            res += hangulconvert[w[i]];
        else
            res += w[i];
    }
    return res;
}

//------------------------------------------------------------------------------
// DoubleMetaphone algorithm
//------------------------------------------------------------------------------

const unsigned int max_length = 32;

char IsVowel(string& s, unsigned long pos) {
    if (pos >= s.size())
        return 0;
    
    return vowels.check(s[pos]);
}

bool SlavoGermanic(string &s)
{
    if (s.find("W") != -1 || s.find("K") != -1 || s.find("CZ") != -1|| s.find("WITZ") != -1)
        return true;
    else
        return false;
}


char GetAt(string &s, unsigned long pos)
{
    if (pos >= s.length()) {
        return '\0';
    }
    
    return s[pos];
}


void SetAt(string &s, unsigned long pos, char c)
{
    if (pos >= s.length()) {
        return;
    }
    
    s[pos] = c;
}


/*
 Caveats: the START value is 0 based
 */
int StringAt(string &s, unsigned long start, unsigned long length, char** sub)
{
    char *test;
    const char *pos;
    
    if (start >= s.length()) {
        return 0;
    }
    
    pos = (s.c_str() + start);
    
    long i = 0;
    do {
        test = sub[i++];
        if (*test && (strncmp(pos, test, length) == 0)) {
            return 1;
        }
    } while (strcmp(test, ""));
    
    
    return 0;
}

static const char* v_ach[] = { "ACH", ""};
static const char* v_aggioggi[] = { "AGGI", "OGGI", ""};
static const char* v_aioi[] = { "AI", "OI", ""};
static const char* v_alle[] = { "ALLE", ""};
static const char* v_ao[] = { "A", "O", ""};
static const char* v_aoue[] = { "A", "O", "U", "E", ""};
static const char* v_asos[] = { "AS", "OS", ""};
static const char* v_auou[] = { "AU", "OU", ""};
static const char* v_bachermacher[] = { "BACHER", "MACHER", ""};
static const char* v_bh[] = { "B", "H", ""};
static const char* v_bhd[] = { "B", "H", "D", ""};
static const char* v_caesar[] = { "CAESAR", ""};
static const char* v_cc[] = { "CC", ""};
static const char* v_ceci[] = { "CE", "CI", ""};
static const char* v_cglrt[] = { "C", "G", "L", "R", "T", ""};
static const char* v_ch[] = { "CH", ""};
static const char* v_chae[] = { "CHAE", ""};
static const char* v_chia[] = { "CHIA", ""};
static const char* v_chore[] = { "CHORE", ""};
static const char* v_cia[] = { "CIA", ""};
static const char* v_cicecy[] = { "CI", "CE", "CY", ""};
static const char* v_ciociecia[] = { "CIO", "CIE", "CIA", ""};
static const char* v_ckcgcq[] = { "CK", "CG", "CQ", ""};
static const char* v_ckq[] = { "C", "K", "Q", ""};
static const char* v_cqg[] = { " C", " Q", " G", ""};
static const char* v_cx[] = { "C", "X", ""};
static const char* v_cz[] = { "CZ", ""};
static const char* v_dangerrangermanger[] = { "DANGER", "RANGER", "MANGER", ""};
static const char* v_dg[] = { "DG", ""};
static const char* v_dtdd[] = { "DT", "DD", ""};
static const char* v_ei[] = { "E", "I", ""};
static const char* v_eiy[] = { "E", "I", "Y", ""};
static const char* v_er[] = { "ER", ""};
static const char* v_eren[] = { "ER", "EN", ""};
static const char* v_esepebeleyibilinieeier[] = { "ES", "EP", "EB", "EL", "EY", "IB", "IL", "IN", "IE", "EI", "ER", ""};
static const char* v_et[] = { "ET", ""};
static const char* v_ewskiewskyowskiowsky[] = { "EWSKI", "EWSKY", "OWSKI", "OWSKY", ""};
static const char* v_ey[] = { "EY", ""};
static const char* v_gnknpnwrps[] = { "GN", "KN", "PN", "WR", "PS", ""};
static const char* v_haracharis[] = { "HARAC", "HARIS", ""};
static const char* v_heimhoekholmholz[] = { "HEIM", "HOEK", "HOLM", "HOLZ", ""};
static const char* v_horhymhiahem[] = { "HOR", "HYM", "HIA", "HEM", ""};
static const char* v_hu[] = { "HU", ""};
static const char* v_iaueau[] = { "IAU", "EAU", ""};
static const char* v_ie[] = { "IE", ""};
static const char* v_ieh[] = { "I", "E", "H", ""};
static const char* v_ier[] = { "IER ", ""};
static const char* v_iey[] = { "I", "E", "Y", ""};
static const char* v_illoillaalle[] = { "ILLO", "ILLA", "ALLE", ""};
static const char* v_islysl[] = { "ISL", "YSL", ""};
static const char* v_jose[] = { "JOSE", ""};
static const char* v_li[] = { "LI", ""};
static const char* v_lrnmbhfvw[] = { "L", "R", "N", "M", "B", "H", "F", "V", "W", " ", ""};
static const char* v_ltksnmbz[] = { "L", "T", "K", "S", "N", "M", "B", "Z", ""};
static const char* v_mc[] = { "MC", ""};
static const char* v_mema[] = { "ME", "MA", ""};
static const char* v_mnlw[] = { "M", "N", "L", "W", ""};
static const char* v_omam[] = { "OM", "AM", ""};
static const char* v_ooerenuyedem[] = { "OO", "ER", "EN", "UY", "ED", "EM", ""};
static const char* v_orchesarchitorchid[] = { "ORCHES", "ARCHIT", "ORCHID", ""};
static const char* v_pb[] = { "P", "B", ""};
static const char* v_rgyogy[] = { "RGY", "OGY", ""};
static const char* v_san[] = { "SAN ", ""};
static const char* v_sc[] = { "SC", ""};
static const char* v_sch[] = { "SCH", ""};
static const char* v_sh[] = { "SH", ""};
static const char* v_sian[] = { "SIAN", ""};
static const char* v_siosia[] = { "SIO", "SIA", ""};
static const char* v_skl[] = { "S", "K", "L", ""};
static const char* v_sugar[] = { "SUGAR", ""};
static const char* v_sz[] = { "S", "Z", ""};
static const char* v_td[] = { "T", "D", ""};
static const char* v_th[] = { "TH", ""};
static const char* v_tiatch[] = { "TIA", "TCH", ""};
static const char* v_tion[] = { "TION", ""};
static const char* v_ts[] = { "T", "S", ""};
static const char* v_tth[] = { "TTH", ""};
static const char* v_ucceeucces[] = { "UCCEE", "UCCES", ""};
static const char* v_umb[] = { "UMB", ""};
static const char* v_vanvon[] = { "VAN ", "VON ", ""};
static const char* v_wh[] = { "WH", ""};
static const char* v_wicz[] = { "WICZ", ""};
static const char* v_wiczwitz[] = { "WICZ", "WITZ", ""};
static const char* v_wr[] = { "WR", ""};
static const char* v_z[] = { "Z", ""};
static const char* v_zoziza[] = { "ZO", "ZI", "ZA", ""};

void DoubleMetaphone(const string &str, vector<string>& codes) {
    string original;
    string primary;
    string secondary;
    
    long   length = str.length();
    long   current = 0;
    long   last = length - 1;
    
    /* we need the real length and last prior to padding */
    original = str; // make a copy
    
    //One case, for which we need an extra-replacement
    original = s_replacestrings(original,"Ç","S");
    original = s_deaccentuate(USTR(original));
    original = s_to_upper(original);
    
    /* Pad original so we can index beyond end */
    original += "     ";
    
    bool slavogermanic = SlavoGermanic(original);
    
    /* skip these when at start of word */
    if (StringAt(original, 0, 2, (char**)v_gnknpnwrps)) {
        current += 1;
    }
    
    /* Initial 'X' is pronounced 'Z' e.g. 'Xavier' */
    if (GetAt(original, 0) == 'X') {
        primary += "S";    /* 'Z' maps to 'S' */
        secondary += "S";
        current += 1;
    }
    
    /* main loop */
    while ((primary.length() < max_length) || (secondary.length() < max_length)) {
        if (current >= length) {
            break;
        }
        
        switch (GetAt(original, current)) {
            case 'A':
            case 'E':
            case 'I':
            case 'O':
            case 'U':
            case 'Y':
                if (current == 0) {
                    /* all init vowels now map to 'A' */
                    primary += "A";
                    secondary += "A";
                }
                current += 1;
                break;
                
            case 'B':
                /* "-mb", e.g", "dumb", already skipped over... */
                primary += "P";
                secondary += "P";
                
                if (GetAt(original, current + 1) == 'B')
                    current += 2;
                else
                    current += 1;
                break;
                
            case 'C':
                /* various germanic */
                if ((current > 1) &&
                    !IsVowel(original, current - 2) &&
                    StringAt(original, (current - 1), 3, (char**)v_ach) &&
                    ((GetAt(original, current + 2) != 'I') &&
                     ((GetAt(original, current + 2) != 'E') ||
                      StringAt(original, (current - 2), 6, (char**)v_bachermacher)))) {
                         primary += "K";
                         secondary += "K";
                         current += 2;
                         break;
                     }
                
                /* special case 'caesar' */
                if ((current == 0) && StringAt(original, current, 6, (char**)v_caesar)) {
                    primary += "S";
                    secondary += "S";
                    current += 2;
                    break;
                }
                
                /* italian 'chianti' */
                if (StringAt(original, current, 4, (char**)v_chia)) {
                    primary += "K";
                    secondary += "K";
                    current += 2;
                    break;
                }
                
                if (StringAt(original, current, 2, (char**)v_ch)) {
                    /* find 'michael' */
                    if ((current > 0) && StringAt(original, current, 4, (char**)v_chae)) {
                        primary += "K";
                        secondary += "X";
                        current += 2;
                        break;
                    }
                    
                    /* greek roots e.g. 'chemistry', 'chorus' */
                    if ((current == 0) &&
                        (StringAt(original, (current + 1), 5, (char**)v_haracharis) ||
                         StringAt(original, (current + 1), 3, (char**)v_horhymhiahem)) &&
                        !StringAt(original, 0, 5, (char**)v_chore)) {
                        primary += "K";
                        secondary += "K";
                        current += 2;
                        break;
                    }
                    
                    /* germanic, greek, or otherwise 'ch' for 'kh' sound */
                    if ((StringAt(original, 0, 4, (char**)v_vanvon) ||
                         StringAt(original, 0, 3, (char**)v_sch)) ||
                        /*  'architect but not 'arch', 'orchestra', 'orchid' */
                        StringAt(original, (current - 2), 6, (char**)v_orchesarchitorchid) ||
                        StringAt(original, (current + 2), 1, (char**)v_ts) ||
                        ((StringAt(original, (current - 1), 1, (char**)v_aoue) ||
                          (current == 0)) &&
                         /* e.g., 'wachtler', 'wechsler', but not 'tichner' */
                         StringAt(original, (current + 2), 1, (char**)v_lrnmbhfvw))) {
                            primary += "K";
                            secondary += "K";
                        } else {
                            if (current > 0) {
                                if (StringAt(original, 0, 2, (char**)v_mc)) {
                                    /* e.g., "McHugh" */
                                    primary += "K";
                                    secondary += "K";
                                } else {
                                    primary += "X";
                                    secondary += "K";
                                }
                            } else {
                                primary += "X";
                                secondary += "X";
                            }
                        }
                    current += 2;
                    break;
                }
                /* e.g, 'czerny' */
                if (StringAt(original, current, 2, (char**)v_cz) &&
                    !StringAt(original, (current - 2), 4, (char**)v_wicz)) {
                    primary += "S";
                    secondary += "X";
                    current += 2;
                    break;
                }
                
                /* e.g., 'focaccia' */
                if (StringAt(original, (current + 1), 3, (char**)v_cia)) {
                    primary += "X";
                    secondary += "X";
                    current += 3;
                    break;
                }
                
                /* double 'C', but not if e.g. 'McClellan' */
                if (StringAt(original, current, 2, (char**)v_cc) &&
                    !((current == 1) && (GetAt(original, 0) == 'M'))) {
                    /* 'bellocchio' but not 'bacchus' */
                    if (StringAt(original, (current + 2), 1, (char**)v_ieh) &&
                        !StringAt(original, (current + 2), 2, (char**)v_hu)) {
                        /* 'accident', 'accede' 'succeed' */
                        if (((current == 1) && (GetAt(original, current - 1) == 'A')) ||
                            StringAt(original, (current - 1), 5, (char**)v_ucceeucces)) {
                            primary += "KS";
                            secondary += "KS";
                            /* 'bacci', 'bertucci', other italian */
                        } else {
                            primary += "X";
                            secondary += "X";
                        }
                        current += 3;
                        break;
                    } else {  /* Pierce's rule */
                        primary += "K";
                        secondary += "K";
                        current += 2;
                        break;
                    }
                }
                
                if (StringAt(original, current, 2, (char**)v_ckcgcq)) {
                    primary += "K";
                    secondary += "K";
                    current += 2;
                    break;
                }
                
                if (StringAt(original, current, 2, (char**)v_cicecy)) {
                    /* italian vs. english */
                    if (StringAt(original, current, 3, (char**)v_ciociecia)) {
                        primary += "S";
                        secondary += "X";
                    } else {
                        primary += "S";
                        secondary += "S";
                    }
                    current += 2;
                    break;
                }
                
                /* else */
                primary += "K";
                secondary += "K";
                
                /* name sent in 'mac caffrey', 'mac gregor */
                if (StringAt(original, (current + 1), 2, (char**)v_cqg))
                    current += 3;
                else
                    if (StringAt(original, (current + 1), 1, (char**)v_ckq) &&
                        !StringAt(original, (current + 1), 2, (char**)v_ceci))
                        current += 2;
                    else
                        current += 1;
                break;
                
            case 'D':
                if (StringAt(original, current, 2, (char**)v_dg)) {
                    if (StringAt(original, (current + 2), 1, (char**)v_iey)) {
                        /* e.g. 'edge' */
                        primary += "J";
                        secondary += "J";
                        current += 3;
                        break;
                    } else {
                        /* e.g. 'edgar' */
                        primary += "TK";
                        secondary += "TK";
                        current += 2;
                        break;
                    }
                }
                
                if (StringAt(original, current, 2, (char**)v_dtdd)) {
                    primary += "T";
                    secondary += "T";
                    current += 2;
                    break;
                }
                
                /* else */
                primary += "T";
                secondary += "T";
                current += 1;
                break;
                
            case 'F':
                if (GetAt(original, current + 1) == 'F')
                    current += 2;
                else
                    current += 1;
                primary += "F";
                secondary += "F";
                break;
                
            case 'G':
                if (GetAt(original, current + 1) == 'H') {
                    if ((current > 0) && !IsVowel(original, current - 1)) {
                        primary += "K";
                        secondary += "K";
                        current += 2;
                        break;
                    }
                    
                    if (current < 3) {
                        /* 'ghislane', ghiradelli */
                        if (current == 0) {
                            if (GetAt(original, current + 2) == 'I') {
                                primary += "J";
                                secondary += "J";
                            } else {
                                primary += "K";
                                secondary += "K";
                            }
                            current += 2;
                            break;
                        }
                    }
                    /* Parker's rule (with some further refinements) - e.g., 'hugh' */
                    if (((current > 1) &&
                         StringAt(original, (current - 2), 1, (char**)v_bhd)) ||
                        /* e.g., 'bough' */
                        ((current > 2) &&
                         StringAt(original, (current - 3), 1, (char**)v_bhd)) ||
                        /* e.g., 'broughton' */
                        ((current > 3) &&
                         StringAt(original, (current - 4), 1, (char**)v_bh))) {
                            current += 2;
                            break;
                        } else {
                            /* e.g., 'laugh', 'McLaughlin', 'cough', 'gough', 'rough', 'tough' */
                            if ((current > 2) &&
                                (GetAt(original, current - 1) == 'U') &&
                                StringAt(original, (current - 3), 1, (char**)v_cglrt)) {
                                primary += "F";
                                secondary += "F";
                            } else if ((current > 0) &&
                                       GetAt(original, current - 1) != 'I') {
                                primary += "K";
                                secondary += "K";
                            }
                            
                            current += 2;
                            break;
                        }
                }
                
                if (GetAt(original, current + 1) == 'N') {
                    if ((current == 1) &&
                        IsVowel(original, 0) &&
                        !slavogermanic) {
                        primary += "KN";
                        secondary += "N";
                    } else
                    /* not e.g. 'cagney' */
                        if (!StringAt(original, (current + 2), 2, (char**)v_ey) &&
                            (GetAt(original, current + 1) != 'Y') &&
                            !slavogermanic) {
                            primary += "N";
                            secondary += "KN";
                        } else {
                            primary += "KN";
                            secondary += "KN";
                        }
                    current += 2;
                    break;
                }
                
                /* 'tagliaro' */
                if (StringAt(original, (current + 1), 2, (char**)v_li) &&
                    !slavogermanic) {
                    primary += "KL";
                    secondary += "L";
                    current += 2;
                    break;
                }
                
                /* -ges-,-gep-,-gel-, -gie- at beginning */
                if ((current == 0) &&
                    ((GetAt(original, current + 1) == 'Y') ||
                     StringAt(original, (current + 1), 2, (char**)v_esepebeleyibilinieeier))) {
                        primary += "K";
                        secondary += "J";
                        current += 2;
                        break;
                    }
                
                /*  -ger-,  -gy- */
                if ((StringAt(original, (current + 1), 2, (char**)v_er) ||
                     (GetAt(original, current + 1) == 'Y')) &&
                    !StringAt(original, 0, 6, (char**)v_dangerrangermanger) &&
                    !StringAt(original, (current - 1), 1, (char**)v_ei) &&
                    !StringAt(original, (current - 1), 3, (char**)v_rgyogy)) {
                    primary += "K";
                    secondary += "J";
                    current += 2;
                    break;
                }
                
                /*  italian e.g, 'biaggi' */
                if (StringAt(original, (current + 1), 1, (char**)v_eiy) ||
                    StringAt(original, (current - 1), 4, (char**)v_aggioggi)) {
                    /* obvious germanic */
                    if ((StringAt(original, 0, 4, (char**)v_vanvon) ||
                         StringAt(original, 0, 3, (char**)v_sch)) ||
                        StringAt(original, (current + 1), 2, (char**)v_et))
                    {
                        primary += "K";
                        secondary += "K";
                    } else {
                        /* always soft if french ending */
                        if (StringAt(original, (current + 1), 4, (char**)v_ier)) {
                            primary += "J";
                            secondary += "J";
                        } else {
                            primary += "J";
                            secondary += "K";
                        }
                    }
                    current += 2;
                    break;
                }
                
                if (GetAt(original, current + 1) == 'G')
                    current += 2;
                else
                    current += 1;
                primary += "K";
                secondary += "K";
                break;
                
            case 'H':
                /* only keep if first & before vowel or btw. 2 vowels */
                if (((current == 0) ||
                     IsVowel(original, current - 1)) &&
                    IsVowel(original, current + 1)) {
                    primary += "H";
                    secondary += "H";
                    current += 2;
                }
                else        /* also takes care of 'HH' */
                    current += 1;
                break;
                
            case 'J':
                /* obvious spanish, 'jose', 'san jacinto' */
                if (StringAt(original, current, 4, (char**)v_jose) ||
                    StringAt(original, 0, 4, (char**)v_san)) {
                    if (((current == 0) && (GetAt(original, current + 4) == ' ')) ||
                        StringAt(original, 0, 4, (char**)v_san)) {
                        primary += "H";
                        secondary += "H";
                    } else {
                        primary += "J";
                        secondary += "H";
                    }
                    current += 1;
                    break;
                }
                
                if ((current == 0) && !StringAt(original, current, 4, (char**)v_jose)) {
                    primary += "J";    /* Yankelovich/Jankelowicz */
                    secondary += "A";
                } else {
                    /* spanish pron. of e.g. 'bajador' */
                    if (IsVowel(original, current - 1) &&
                        !slavogermanic &&
                        ((GetAt(original, current + 1) == 'A') ||
                         (GetAt(original, current + 1) == 'O'))) {
                            primary += "J";
                            secondary += "H";
                        } else {
                            if (current == last) {
                                primary += "J";
                                secondary += "";
                            } else {
                                if (!StringAt(original, (current + 1), 1, (char**)v_ltksnmbz) &&
                                    !StringAt(original, (current - 1), 1, (char**)v_skl)) {
                                    primary += "J";
                                    secondary += "J";
                                }
                            }
                        }
                }
                
                if (GetAt(original, current + 1) == 'J')    /* it could happen! */
                    current += 2;
                else
                    current += 1;
                break;
                
            case 'K':
                if (GetAt(original, current + 1) == 'K')
                    current += 2;
                else
                    current += 1;
                primary += "K";
                secondary += "K";
                break;
                
            case 'L':
                if (GetAt(original, current + 1) == 'L') {
                    /* spanish e.g. 'cabrillo', 'gallegos' */
                    if (((current == (length - 3)) &&
                         StringAt(original, (current - 1), 4, (char**)v_illoillaalle)) ||
                        ((StringAt(original, (last - 1), 2, (char**)v_asos) ||
                          StringAt(original, last, 1, (char**)v_ao)) &&
                         StringAt(original, (current - 1), 4, (char**)v_alle))) {
                            primary += "L";
                            secondary += "";
                            current += 2;
                            break;
                        }
                    current += 2;
                }
                else
                    current += 1;
                primary += "L";
                secondary += "L";
                break;
                
            case 'M':
                if ((StringAt(original, (current - 1), 3, (char**)v_umb) &&
                     (((current + 1) == last) ||
                      StringAt(original, (current + 2), 2, (char**)v_er))) ||
                    /* 'dumb','thumb' */
                    (GetAt(original, current + 1) == 'M')) {
                    current += 2;
                } else {
                    current += 1;
                }
                primary += "M";
                secondary += "M";
                break;
                
            case 'N':
                if (GetAt(original, current + 1) == 'N') {
                    current += 2;
                } else {
                    current += 1;
                }
                primary += "N";
                secondary += "N";
                break;
                
            case 'P':
                if (GetAt(original, current + 1) == 'H') {
                    primary += "F";
                    secondary += "F";
                    current += 2;
                    break;
                }
                
                /* also account for "campbell", "raspberry" */
                if (StringAt(original, (current + 1), 1, (char**)v_pb))
                    current += 2;
                else
                    current += 1;
                primary += "P";
                secondary += "P";
                break;
                
            case 'Q':
                if (GetAt(original, current + 1) == 'Q')
                    current += 2;
                else
                    current += 1;
                primary += "K";
                secondary += "K";
                break;
                
            case 'R':
                /* french e.g. 'rogier', but exclude 'hochmeier' */
                if ((current == last) &&
                    !slavogermanic &&
                    StringAt(original, (current - 2), 2, (char**)v_ie) &&
                    !StringAt(original, (current - 4), 2, (char**)v_mema)) {
                    primary += "";
                    secondary += "R";
                } else {
                    primary += "R";
                    secondary += "R";
                }
                
                if (GetAt(original, current + 1) == 'R')
                    current += 2;
                else
                    current += 1;
                break;
                
            case 'S':
                /* special cases 'island', 'isle', 'carlisle', 'carlysle' */
                if (StringAt(original, (current - 1), 3, (char**)v_islysl)) {
                    current += 1;
                    break;
                }
                
                /* special case 'sugar-' */
                if ((current == 0) && StringAt(original, current, 5, (char**)v_sugar)) {
                    primary += "X";
                    secondary += "S";
                    current += 1;
                    break;
                }
                
                if (StringAt(original, current, 2, (char**)v_sh)) {
                    /* germanic */
                    if (StringAt(original, (current + 1), 4, (char**)v_heimhoekholmholz)) {
                        primary += "S";
                        secondary += "S";
                    } else {
                        primary += "X";
                        secondary += "X";
                    }
                    current += 2;
                    break;
                }
                
                /* italian & armenian */
                if (StringAt(original, current, 3, (char**)v_siosia) ||
                    StringAt(original, current, 4, (char**)v_sian)) {
                    if (!slavogermanic) {
                        primary += "S";
                        secondary += "X";
                    } else {
                        primary += "S";
                        secondary += "S";
                    }
                    current += 3;
                    break;
                }
                
                /* german & anglicisations, e.g. 'smith' match 'schmidt', 'snider' match 'schneider'
                 also, -sz- in slavic language altho in hungarian it is pronounced 's' */
                if (((current == 0) &&
                     StringAt(original, (current + 1), 1, (char**)v_mnlw)) ||
                    StringAt(original, (current + 1), 1, (char**)v_z)) {
                    primary += "S";
                    secondary += "X";
                    if (StringAt(original, (current + 1), 1, (char**)v_z))
                        current += 2;
                    else
                        current += 1;
                    break;
                }
                
                if (StringAt(original, current, 2, (char**)v_sc)) {
                    /* Schlesinger's rule */
                    if (GetAt(original, current + 2) == 'H') {
                        /* dutch origin, e.g. 'school', 'schooner' */
                        if (StringAt(original, (current + 3), 2, (char**)v_ooerenuyedem)) {
                            /* 'schermerhorn', 'schenker' */
                            if (StringAt(original, (current + 3), 2, (char**)v_eren)) {
                                primary += "X";
                                secondary += "SK";
                            } else {
                                primary += "SK";
                                secondary += "SK";
                            }
                            current += 3;
                            break;
                        } else {
                            if ((current == 0) && !IsVowel(original, 3) &&
                                (GetAt(original, 3) != 'W')) {
                                primary += "X";
                                secondary += "S";
                            } else {
                                primary += "X";
                                secondary += "X";
                            }
                            current += 3;
                            break;
                        }
                    }
                    
                    if (StringAt(original, (current + 2), 1, (char**)v_iey)) {
                        primary += "S";
                        secondary += "S";
                        current += 3;
                        break;
                    }
                    /* else */
                    primary += "SK";
                    secondary += "SK";
                    current += 3;
                    break;
                }
                
                /* french e.g. 'resnais', 'artois' */
                if ((current == last) &&
                    StringAt(original, (current - 2), 2, (char**)v_aioi)) {
                    primary += "";
                    secondary += "S";
                } else {
                    primary += "S";
                    secondary += "S";
                }
                
                if (StringAt(original, (current + 1), 1, (char**)v_sz))
                    current += 2;
                else
                    current += 1;
                break;
                
            case 'T':
                if (StringAt(original, current, 4, (char**)v_tion)) {
                    primary += "X";
                    secondary += "X";
                    current += 3;
                    break;
                }
                
                if (StringAt(original, current, 3, (char**)v_tiatch)) {
                    primary += "X";
                    secondary += "X";
                    current += 3;
                    break;
                }
                
                if (StringAt(original, current, 2, (char**)v_th) ||
                    StringAt(original, current, 3, (char**)v_tth)) {
                    /* special case 'thomas', 'thames' or germanic */
                    if (StringAt(original, (current + 2), 2, (char**)v_omam) ||
                        StringAt(original, 0, 4, (char**)v_vanvon) ||
                        StringAt(original, 0, 3, (char**)v_sch)) {
                        primary += "T";
                        secondary += "T";
                    } else {
                        primary += "0"; /* yes, zero */
                        secondary += "T";
                    }
                    current += 2;
                    break;
                }
                
                if (StringAt(original, (current + 1), 1, (char**)v_td)) {
                    current += 2;
                } else {
                    current += 1;
                }
                primary += "T";
                secondary += "T";
                break;
                
            case 'V':
                if (GetAt(original, current + 1) == 'V') {
                    current += 2;
                } else {
                    current += 1;
                }
                primary += "F";
                secondary += "F";
                break;
                
            case 'W':
                /* can also be in middle of word */
                if (StringAt(original, current, 2, (char**)v_wr)) {
                    primary += "R";
                    secondary += "R";
                    current += 2;
                    break;
                }
                
                if ((current == 0) &&
                    (IsVowel(original, current + 1) ||
                     StringAt(original, current, 2, (char**)v_wh))) {
                        /* Wasserman should match Vasserman */
                        if (IsVowel(original, current + 1)) {
                            primary += "A";
                            secondary += "F";
                        } else {
                            /* need Uomo to match Womo */
                            primary += "A";
                            secondary += "A";
                        }
                    }
                
                /* Arnow should match Arnoff */
                if (((current == last) && IsVowel(original, current - 1)) ||
                    StringAt(original, (current - 1), 5, (char**)v_ewskiewskyowskiowsky) ||
                    StringAt(original, 0, 3, (char**)v_sch)) {
                    primary += "";
                    secondary += "F";
                    current += 1;
                    break;
                }
                
                /* polish e.g. 'filipowicz' */
                if (StringAt(original, current, 4, (char**)v_wiczwitz)) {
                    primary += "TS";
                    secondary += "FX";
                    current += 4;
                    break;
                }
                
                /* else skip it */
                current += 1;
                break;
                
            case 'X':
                /* french e.g. breaux */
                if (!((current == last) &&
                      (StringAt(original, (current - 3), 3, (char**)v_iaueau) ||
                       StringAt(original, (current - 2), 2, (char**)v_auou)))) {
                          primary += "KS";
                          secondary += "KS";
                      }
                
                
                if (StringAt(original, (current + 1), 1, (char**)v_cx))
                    current += 2;
                else
                    current += 1;
                break;
                
            case 'Z':
                /* chinese pinyin e.g. 'zhao' */
                if (GetAt(original, current + 1) == 'H') {
                    primary += "J";
                    secondary += "J";
                    current += 2;
                    break;
                } else if (StringAt(original, (current + 1), 2, (char**)v_zoziza) ||
                           (slavogermanic &&
                            ((current > 0) &&
                             GetAt(original, current - 1) != 'T'))) {
                                primary += "S";
                                secondary += "TS";
                            } else {
                                primary += "S";
                                secondary += "S";
                            }
                
                if (GetAt(original, current + 1) == 'Z')
                    current += 2;
                else
                    current += 1;
                break;
                
            default:
                current += 1;
        }
        /* printf("PRIMARY: %s\n", primary.str);
         printf("SECONDARY: %s\n", secondary.str);  */
    }
    
    
    if (primary.length() > max_length)
        SetAt(primary, max_length, '\0');
    
    if (secondary.length() > max_length)
        SetAt(secondary, max_length, '\0');
    
    codes.push_back(primary);
    codes.push_back(secondary);
}


