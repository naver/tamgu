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

#include "tables.h"
#include "utf8handler.h"

Exchanging UTF8_Handler* utf8_handler = NULL;

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
    
#ifdef WIN32
	unsigned long j;
	unsigned long q = 0;
#else
	unsigned int j;
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

//-----------------------------------------------------------------------------------------
char* unicode_2_utf8(long code, char* utf) {
    unsigned char c = (bool(code & 0xFFFF0000) << 3) | (bool(code & 0xF800)<<2) | (bool(code & 0x780) << 1);
    switch (c) {
        case 8:
        case 10:
        case 12:
        case 14:
            utf[0] = 0xF0 | (code >> 18);
            utf[1] = 0x80 | ((code >> 12) & 0x3f);
            utf[2] = 0x80 | ((code >> 6) & 0x3f);
            utf[3] = 0x80 | (code& 0x3f);
            utf[4] = 0;
            break;
        case 4:
        case 6:
            utf[0] = 0xe0 | (code >> 12);
            utf[1] = 0x80 | ((code >> 6) & 0x3f);
            utf[2] = 0x80 | (code& 0x3f);
            utf[3] = 0;
            break;
        case 2:
            utf[0] = 0xc0 | (code >> 6);
            utf[1] = 0x80 | (code& 0x3f);
            utf[2] = 0;
            break;
        default:
            utf[0] = (unsigned char)code;
            utf[1] = 0;
            break;
    }
    return utf;
}

#ifdef WIN32
TAMGUCHAR getonechar(unsigned char* s, long& i) {
	TAMGUCHAR result, code;
	i += c_utf8_to_unicode(s + i, code);
	if (c_utf16_to_unicode(result, code, false)) {
		i += c_utf8_to_unicode(s + i, code);
		c_utf16_to_unicode(result, code, true);
	}
	return result;
}
#else
TAMGUCHAR getonechar(unsigned char* s, long& i) {
	TAMGUCHAR code;
	i += c_utf8_to_unicode(s + i, code);
	return code;
}
#endif

//-----------------------------------------------------------------------------------------
Exporting bool c_is_emoji(TAMGUCHAR c) {
    return utf8_handler->c_is_emoji(c);
}
Exporting bool c_is_emojicomp(TAMGUCHAR c)  {
    return utf8_handler->c_is_emojicomp(c);
}
Exporting bool c_is_emoji(unsigned char* m, long& i)  {
    return utf8_handler->c_is_emoji(m, i);
}
Exporting bool c_is_emojicomp(unsigned char* m, long& i)  {
    return utf8_handler->c_is_emojicomp(m, i);
}
Exporting string Emoji(TAMGUCHAR c)  {
    return "Not implemented yet";
}
Exporting string Emoji(string&)  {
    return "Not implemented yet";
}
Exporting string Emoji(wstring& s)  {
    return "Not implemented yet";
}

Exporting bool s_is_emoji(string& s)  {
    return utf8_handler->s_is_emoji(s);
}

Exporting bool s_is_emoji(wstring& s)  {
    return utf8_handler->s_is_emoji(s);
}

Exporting bool scan_emoji(wstring& w, long& i) {
    return utf8_handler->scan_emoji(w, i);
}

Exporting bool scan_emoji(string& w, long& i) {
    return utf8_handler->scan_emoji(w, i);
}

Exporting bool scan_emoji(unsigned char* w, long& i) {
    return utf8_handler->scan_emoji(w, i);
}

Exporting bool get_emoji(unsigned char* w, string& res, long& i) {
    return utf8_handler->get_emoji(w, res, i);
}

Exporting bool get_emoji(string& w, string& res, long& i) {
    return utf8_handler->get_emoji(w, res, i);
}

Exporting bool get_emoji(wstring& w, wstring& res, long& i) {
    return utf8_handler->get_emoji(w, res, i);
}

Exporting bool store_emoji(unsigned char* w, string& res, long& i) {
    return utf8_handler->store_emoji(w, res, i);
}

Exporting bool store_emoji(string& w, string& res, long& i) {
    return utf8_handler->store_emoji(w, res, i);
}

Exporting bool store_emoji(wstring& w, wstring& res, long& i) {
    return utf8_handler->store_emoji(w, res, i);
}

Exporting void l_emojis(map<BLONG, string>& dico)  {}

Exporting void convertnumber(long v, string& s) {
   utf8_handler->convertnumber(v, s);
}

Exporting void convertnumber(double v, string& s) {
   utf8_handler->convertnumber(v, s);
}

Exporting void convertnumber(long v, wstring& s) {
   utf8_handler->convertnumber(v, s);
}

Exporting void convertnumber(double v, wstring& s) {
   utf8_handler->convertnumber(v, s);
}

Exporting string convertfromnumber(BLONG l) {
  return utf8_handler->convertfromnumber(l);
}

Exporting wstring wconvertfromnumber(BLONG l) {
  return utf8_handler->wconvertfromnumber(l);
}

Exporting string convertfromnumber(long l) {
  return utf8_handler->convertfromnumber(l);
}

Exporting wstring wconvertfromnumber(long l) {
  return utf8_handler->wconvertfromnumber(l);
}

Exporting wstring wconvertfromnumber(short l) {
  return utf8_handler->wconvertfromnumber(l);
}

Exporting string convertfromnumber(short l) {
  return utf8_handler->convertfromnumber(l);
}

Exporting string convertfromnumber(double l) {
  return utf8_handler->convertfromnumber(l);
}

Exporting string convertfromnumber(float l) {
  return utf8_handler->convertfromnumber(l);
}

Exporting wstring wconvertfromnumber(double l) {
  return utf8_handler->wconvertfromnumber(l);
}

Exporting wstring wconvertfromnumber(float l) {
  return utf8_handler->wconvertfromnumber(l);
}

Exporting bool valid_latin_table(short tableindex) {
    return utf8_handler->valid_latin_table(tableindex);
}
Exporting void c_HTMLcode(string& s, TAMGUCHAR code) {
    utf8_handler->c_HTMLcode(s, code);
}
Exporting void c_XMLcode(string& s, TAMGUCHAR code) {
    utf8_handler->c_XMLcode(s, code);
}
Exporting void s_EvaluateMetaCharacters(string& s) {
    utf8_handler->s_EvaluateMetaCharacters(s);
}
Exporting void s_EvaluateMetaCharacters(wstring& s) {
    utf8_handler->s_EvaluateMetaCharacters(s);
}
Exporting TAMGUCHAR c_latin_table_to_unicode(int tableindex, TAMGUCHAR c) {
    return utf8_handler->c_latin_table_to_unicode(tableindex, c);
}
Exporting uchar c_latin_to_utf8(unsigned char code, unsigned char* utf) {
    return utf8_handler->c_latin_to_utf8(code, utf);
}
Exporting uchar c_latin_to_utf8(unsigned char* utf, unsigned char* res, bool checktable = false) {
    return utf8_handler->c_latin_to_utf8(utf, res, checktable);
}
Exporting TAMGUCHAR c_unicode_to_latin(TAMGUCHAR u) {
    return utf8_handler->c_unicode_to_latin(u);
}
Exporting string s_utf8_to_latin(unsigned char* contenu, long sz) {
    return utf8_handler->s_utf8_to_latin(contenu, sz);
}
Exporting string conversion_unicode_to_latin(wstring& w) {
    return utf8_handler->conversion_unicode_to_latin(w);
}
Exporting string conversion_latin_table_to_utf8(short tableindex, unsigned char* contenu, long sz) {
    return utf8_handler->conversion_latin_table_to_utf8(tableindex, contenu, sz);
}
Exporting wstring conversion_latin_table_to_unicode(short tableindex, unsigned char* contenu, long sz) {
    return utf8_handler->conversion_latin_table_to_unicode(tableindex, contenu, sz);
}
Exporting string s_unicode_to_dos(wstring& contenu) {
    return utf8_handler->s_unicode_to_dos(contenu);
}
Exporting string s_utf8_to_dos(unsigned char* contenu) {
    return utf8_handler->s_utf8_to_dos(contenu);
}
Exporting string s_dos_to_utf8(unsigned char* contenu) {
    return utf8_handler->s_dos_to_utf8(contenu);
}
Exporting string s_utf8_to_dos(const char* contenu) {
    return utf8_handler->s_utf8_to_dos(contenu);
}
Exporting string s_dos_to_utf8(const char* contenu) {
    return utf8_handler->s_dos_to_utf8(contenu);
}
Exporting void s_latin_to_utf8(string& res, unsigned char* contenu, long sz) {
    utf8_handler->s_latin_to_utf8(res, contenu, sz);
}
Exporting string conversion_latin_to_utf8(unsigned char* contenu, long sz) {
    return utf8_handler->conversion_latin_to_utf8(contenu, sz);
}
Exporting string conversion_utf8_to_latin(string contenu) {
    return utf8_handler->conversion_utf8_to_latin(contenu);
}
Exporting string conversion_latin_to_utf8(string contenu) {
    return utf8_handler->conversion_latin_to_utf8(contenu);
}
Exporting char c_is_alpha(unsigned char* m, long& i) {
    return utf8_handler->c_is_alpha(m, i);
}
Exporting bool s_is_punctuation(wstring& str) {
    return utf8_handler->s_is_punctuation(str);
}
Exporting bool c_is_punctuation(TAMGUCHAR code) {
    return utf8_handler->c_is_punctuation(code);
}
Exporting bool c_is_separator(TAMGUCHAR code) {
    return utf8_handler->c_is_separator(code);
}
Exporting bool s_is_punctuation(string& str) {
    return utf8_handler->s_is_punctuation(str);
}
Exporting bool s_is_upper(string& s) {
    return utf8_handler->s_is_upper(s);
}
Exporting bool s_is_alpha(wstring& s) {
    return utf8_handler->s_is_alpha(s);
}
Exporting bool s_is_upper(wstring& s) {
    return utf8_handler->s_is_upper(s);
}
Exporting bool s_is_lower(wstring& s) {
    return utf8_handler->s_is_lower(s);
}
Exporting bool c_is_consonant(TAMGUCHAR c) {
    return utf8_handler->c_is_consonant(c);
}
Exporting bool c_is_vowel(TAMGUCHAR c) {
    return utf8_handler->c_is_vowel(c);
}
Exporting bool s_is_consonant(wstring& s) {
    return utf8_handler->s_is_consonant(s);
}
Exporting bool s_is_vowel(wstring& s) {
    return utf8_handler->s_is_vowel(s);
}
Exporting bool s_is_consonant(unsigned char* s) {
    return utf8_handler->s_is_consonant(s);
}
Exporting bool s_is_vowel(unsigned char* s) {
    return utf8_handler->s_is_vowel(s);
}
Exporting void s_binary(unsigned char* s, int mx, long lg, TAMGUCHAR* codes) {
    utf8_handler->s_binary(s, mx, lg, codes);
}
Exporting bool compare_vowel(TAMGUCHAR c, TAMGUCHAR cc) {
    return utf8_handler->compare_vowel(c, cc);
}
Exporting wstring s_deaccentuate(wstring& s) {
    return utf8_handler->s_deaccentuate(s);
}
Exporting string s_deaccentuate(unsigned char* s) {
    return utf8_handler->s_deaccentuate(s);
}
Exporting bool s_is_alpha(string& s) {
    return utf8_handler->s_is_alpha(s);
}
Exporting bool s_is_lower(string& s) {
    return utf8_handler->s_is_lower(s);
}
Exporting TAMGUCHAR c_to_lower(TAMGUCHAR c) {
    return utf8_handler->c_to_lower(c);
}
Exporting TAMGUCHAR c_to_upper(TAMGUCHAR c) {
    return utf8_handler->c_to_upper(c);
}
Exporting bool b_alpha(TAMGUCHAR c) {
    return utf8_handler->b_alpha(c);
}
Exporting char c_is_alpha(TAMGUCHAR code) {
    return utf8_handler->c_is_alpha(code);
}
Exporting bool c_is_lower(TAMGUCHAR code) {
    return utf8_handler->c_is_lower(code);
}
Exporting bool c_is_upper(TAMGUCHAR code) {
    return utf8_handler->c_is_upper(code);
}
Exporting wstring s_to_lower(wstring& s) {
    return utf8_handler->s_to_lower(s);
}
Exporting wstring s_to_upper(wstring& s) {
    return utf8_handler->s_to_upper(s);
}
Exporting string s_to_lower(string& s) {
    return utf8_handler->s_to_lower(s);
}
Exporting string s_to_upper(string& s) {
    return utf8_handler->s_to_upper(s);
}
Exporting void c_to_lower(string& res, unsigned char* s, long lg) {
    utf8_handler->c_to_lower(res, s, lg);
}
Exporting void c_to_upper(string& res, unsigned char* s, long lg) {
    utf8_handler->c_to_upper(res, s, lg);
}
Exporting bool c_is_hangul(string& s) {
    return utf8_handler->c_is_hangul(s);
}
Exporting bool s_is_hangul(wstring& w) {
    return utf8_handler->s_is_hangul(w);
}
Exporting bool c_is_hangul(TAMGUCHAR c) {
    return utf8_handler->c_is_hangul(c);
}
Exporting string c_translate(TAMGUCHAR c) {
    return utf8_handler->c_translate(c);
}
Exporting string c_translate(unsigned char* m, long& i) {
    return utf8_handler->c_translate(m, i);
}
Exporting bool c_is_hangul(unsigned char* m, long& i) {
    return utf8_handler->c_is_hangul(m, i);
}
Exporting bool s_is_jamo(wstring& w) {
    return utf8_handler->s_is_jamo(w);
}
Exporting wstring s_split_jamo(wstring& w) {
    return utf8_handler->s_split_jamo(w);
}
Exporting wstring s_combine_jamo(wstring& w) {
    return utf8_handler->s_combine_jamo(w);
}
Exporting wstring s_hangul_normalize(wstring& w) {
    return utf8_handler->s_hangul_normalize(w);
}

//-----------------------------------------------------------------------------------------
//             Initialisation of the UTF8 Handler
//-----------------------------------------------------------------------------------------
Exporting void inittableutf8() {
    if (utf8_handler == NULL)
        utf8_handler = new UTF8_Handler();
}

Exporting UTF8_Handler* create_utf8_handler() {
    inittableutf8();
    return utf8_handler;
}

Exporting void set_utf8_handler(UTF8_Handler* u) {
    if (utf8_handler != u) {
        if (utf8_handler != NULL)
            delete utf8_handler;
        utf8_handler = u;
    }
}

Exporting void clean_utf8_handler() {
    if (utf8_handler == NULL)
        delete utf8_handler;
    utf8_handler = NULL;
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
    long p = i;
    TAMGUCHAR c = getachar(s, i);
    if (utf8_handler->scan_emoji(s, p))
        i = p;
	return c;
}

long conversionpostochar(wstring& w, long spos) {
    long cpos = 0;
	long realpos = 0;
	while (realpos != spos) {
        if (!utf8_handler->scan_emoji(w, realpos))
		    getachar(w, realpos);
        realpos++;
		cpos++;
	}
	return cpos;
}

void conversionpostochar(wstring& w, long& b, long& e) {
    long sbeg = b;
    long cpos = 0;
    long realpos = 0;
    while (realpos != e) {
        if (realpos == sbeg)
            b = cpos;
        if (!utf8_handler->scan_emoji(w, realpos))
            getachar(w, realpos);
        realpos++;
        cpos++;
    }
    e = cpos;
}

//===================================================================

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
    
    if (*s=='0' && (s[1]=='x' || s[1] == 'b')) {
        if (s[1] == 'x') {
            s+=2;
            return conversiontofloathexa(s, sign);
        }
        s+=2;
        BLONG v = 0;
        while (isadigit(*s)) {
            v <<= 1;
            v |= *s - 48;
            s++;
        }
        return v*sign;
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
    
    if (*s=='0' && (s[1]=='x' || s[1] == 'b')) {
        if (s[1] == 'x') {
            s+=2;
            l++;
            return conversiontofloathexa(s, sign, l);
        }
        s+=2;
        l++;
        BLONG v = 0;
        while (isadigit(*s)) {
            v <<= 1;
            v |= *s - 48;
            s++;
            l++;
        }
        return v*sign;
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
    
    if (*s=='0' && (s[1]=='x' || s[1] == 'b')) {
        if (s[1] == 'x') {
            s+=2;
            l++;
            noconversiontofloathexa(s, sign, l);
            return;
        }
        s+=2;
        l++;
        while (isadigit(*s)) {
            s++;
            l++;
        }
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
    
    if (*s=='0' && (s[1]=='x' || s[1] == 'b')) {
        if (s[1] == 'x') {
            s+=2;
            l++;
            return conversiontofloathexa(s, sign, l);
        }
        s+=2;
        l++;
        BLONG v = 0;
        while (isadigit(*s)) {
            v <<= 1;
            v |= *s - 48;
            s++;
            l++;
        }
        return v*sign;
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
    BLONG v = 0;
    if (c == '0' && (*number == 'x' || *number == 'b')) {
        if (*number == 'x') {
            ++number;
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
            ++number;
            while (*number) {
                v <<= 1;
                v |= *number - 48;
                ++number;
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

    if (c == '0' || (number[ipos] == 'x' || number[ipos] == 'b')) {
        if (number[ipos] == 'x') {
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
            ipos++;
            c = number[ipos++];
            while (c) {
                v <<= 1;
                v |= c - 48;
                c = number[ipos++];
            }
            return v*sign;
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



//===================================================================

//--------------------------------------------------------------------------------
// Conversion one character: UTF8, UNICODE, LATIN
Exporting unsigned char c_unicode_to_utf8(TAMGUCHAR code, unsigned char* utf) {
    unsigned char c = (bool(code & 0xFFFF0000) << 3) | (bool(code & 0xF800)<<2) | (bool(code & 0x780) << 1);
    switch (c) {
        case 8:
        case 10:
        case 12:
        case 14:
            utf[0] = 0xF0 | (code >> 18);
            utf[1] = 0x80 | ((code >> 12) & 0x3f);
            utf[2] = 0x80 | ((code >> 6) & 0x3f);
            utf[3] = 0x80 | (code& 0x3f);
            utf[4] = 0;
            return 4;
        case 4:
        case 6:
            utf[0] = 0xe0 | (code >> 12);
            utf[1] = 0x80 | ((code >> 6) & 0x3f);
            utf[2] = 0x80 | (code& 0x3f);
            utf[3] = 0;
            return 3;
        case 2:
            utf[0] = 0xc0 | (code >> 6);
            utf[1] = 0x80 | (code& 0x3f);
            utf[2] = 0;
            return 2;
        default:
            utf[0] = (unsigned char)code;
            utf[1] = 0;
            return 1;
    }
}


Exporting string c_unicode_to_utf8(TAMGUCHAR code) {
	char utf[5];
    unsigned char c = (bool(code & 0xFFFF0000) << 3) | (bool(code & 0xF800)<<2) | (bool(code & 0x780) << 1);
    switch (c) {
        case 8:
        case 10:
        case 12:
        case 14:
            utf[0] = 0xF0 | (code >> 18);
            utf[1] = 0x80 | ((code >> 12) & 0x3f);
            utf[2] = 0x80 | ((code >> 6) & 0x3f);
            utf[3] = 0x80 | (code& 0x3f);
            utf[4] = 0;
            break;
        case 4:
        case 6:
            utf[0] = 0xe0 | (code >> 12);
            utf[1] = 0x80 | ((code >> 6) & 0x3f);
            utf[2] = 0x80 | (code& 0x3f);
            utf[3] = 0;
            break;
        case 2:
            utf[0] = 0xc0 | (code >> 6);
            utf[1] = 0x80 | (code& 0x3f);
            utf[2] = 0;
            break;
        default:
            utf[0] = (unsigned char)code;
            utf[1] = 0;
            break;
    }
    return utf;    
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


uchar c_utf8_to_latin(unsigned char* utf, uchar& nb) {
    nb = 1;
    uchar c = utf[0];
    uchar code;
    
    uchar check = c & 0xF0;

    switch (check) {
        case 0xF0 : {
            if ((utf[1] & 0x80) == 0x80 && (utf[2] & 0x80)== 0x80 && (utf[3] & 0x80)== 0x80) {
                code = (c & 0x7) << 18;
                code |= (utf[1] & 0x3F) << 12;
                code |= (utf[2] & 0x3F) << 6;
                code |= (utf[3] & 0x3F);
                nb = 4;
                return code;
            }
            return c;
        }
        case 0xE0: {
            if ((utf[1] & 0x80)== 0x80 && (utf[2] & 0x80)== 0x80) {
                code = (c & 0xF) << 12;
                code |= (utf[1] & 0x3F) << 6;
                code |= (utf[2] & 0x3F);
                nb = 3;
                return code;
            }
            return c;
        }
        case 0xC0: {
            if ((utf[1] & 0x80)== 0x80) {
                code = (c & 0x1F) << 6;
                code |= (utf[1] & 0x3F);
                nb = 2;
                return code;
            }
        }
    }
    return c;
}

Exporting unsigned char c_utf8_to_unicode(unsigned char* utf, TAMGUCHAR& code) {
    unsigned char check = utf[0] & 0xF0;
    
    code = utf[0];

    switch (check) {
        case 0xF0: {
            if ((utf[1] & 0x80) == 0x80 && (utf[2] & 0x80)== 0x80 && (utf[3] & 0x80)== 0x80) {
                code = (code & 0x7) << 18;
                code |= (utf[1] & 0x3F) << 12;
                code |= (utf[2] & 0x3F) << 6;
                code |= (utf[3] & 0x3F);
                return 3;
            }
            return 0;
        }
        case 0xE0: {
            if ((utf[1] & 0x80)== 0x80 && (utf[2] & 0x80)== 0x80) {
                code = (code & 0xF) << 12;
                code |= (utf[1] & 0x3F) << 6;
                code |= (utf[2] & 0x3F);
                return 2;
            }
            return 0;
        }
        case 0xC0: {
            if ((utf[1] & 0x80)== 0x80) {
                code = (code & 0x1F) << 6;
                code |= (utf[1] & 0x3F);
                return 1;
            }
        }
    }
    
    return 0;
}

Exporting char c_test_utf8(unsigned char* utf) {
    if (utf == NULL)
        return 0;
    
    unsigned char check = utf[0] & 0xF0;
    
    switch (check) {
        case 0xC0:
            return bool((utf[1] & 0x80)== 0x80)*1;
        case 0xE0:
            return bool(((utf[1] & 0x80)== 0x80 && (utf[2] & 0x80)== 0x80))*2;
        case 0xF0:
            return bool(((utf[1] & 0x80) == 0x80 && (utf[2] & 0x80)== 0x80 && (utf[3] & 0x80)== 0x80))*3;
    }
    return 0;
}

Exporting char c_detect_utf8(unsigned char* utf) {
    return c_test_utf8(utf);
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

void c_unicode_to_utf16(wstring& w, uint32_t c) {
    if (!(c & 0xFFFF0000))
        w = (wchar_t)c;
    else {
        uint32_t c16;
        c_unicode_to_utf16(c16, c);
        w = (wchar_t)(c16 >> 16);
        w += (wchar_t)(c16 & 0xFFFF);
    }
}

//---------------------------------------------------------------------------
//convert a character position from byte to char and back
void c_chartobyteposition(unsigned char* contenu, long sz, long& bcpos, long& ecpos) {
    if (ecpos > sz)
        return;
    
    long begcpos = bcpos;
    long endcpos = ecpos;
    long i = 0;
    
    while (endcpos > 0 && i<sz) {
        if (!begcpos)
            bcpos = i;
        if (!utf8_handler->scan_emoji(contenu, i))
            i += c_test_utf8(contenu + i);
        i++;
        endcpos--;
        begcpos--;
    }
    ecpos = i;
}

void c_bytetocharposition(unsigned char* contenu, long& bbpos, long& ebpos) {
    long i = 0;
    long sz = 0;
    long bpos = bbpos;
    
    while (i < ebpos) {
        if (i == bpos)
            bbpos = sz;
        if (!utf8_handler->scan_emoji(contenu, i))
            i += c_test_utf8(contenu + i);
        i++;
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
    
    while (charpos > 0 && i<sz) {
        if (!utf8_handler->scan_emoji(contenu, i))
            i += c_test_utf8(contenu + i);
        i++;
        charpos--;
    }
    return i;
}

Exporting long c_chartobyteposition(unsigned char* contenu, long charpos) {
    long i = 0;
    while (charpos > 0 && contenu[i]) {
        if (!utf8_handler->scan_emoji(contenu, i))
            i += c_test_utf8(contenu + i);
        i++;
        charpos--;
    }
    return i;
}

Exporting long c_bytetocharposition(unsigned char* contenu, long charpos) {
    long i = 0;
    long sz = 0;
    while (i < charpos) {
        if (!utf8_handler->scan_emoji(contenu, i))
            i += c_test_utf8(contenu + i);
        i++;
        sz++;
    }
    return sz;
}

Exporting long c_bytetocharpositionidx(unsigned char* contenu, long charpos, long& sz, long& i) {
    while (i<charpos) {
        if (!utf8_handler->scan_emoji(contenu, i))
            i += c_test_utf8(contenu + i);
        i++;
        sz++;
    }
    return sz;
}

Exporting long c_chartobytepositionidx(unsigned char* contenu, long charpos, long& sz, long& i) {
    while (charpos>0) {
        if (!utf8_handler->scan_emoji(contenu, i))
            i += c_test_utf8(contenu + i);
        i++;
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
    wstring res;
    if (!utf8_handler->get_emoji(s, res, i))
        store_char_check_utf16(res, getachar(s, i));
	return res;
}

void getafullchar(wstring& s, wstring& res, long& i) {
    if (!utf8_handler->store_emoji(s, res, i))
        store_char_check_utf16(res, getachar(s, i));
    i++;
}

//-----------------------------------------------------------------------------------
//---------------------------- WSTRING_IS_UTF16 SECTION -----------------------------
// In this section, we handle wstring as a string of UTF16 characters
// Conversion strings. This is true for Windows for instance...
//-----------------------------------------------------------------------------------
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
    for (; i < lg; i++) {
        if (!utf8_handler->scan_emoji(w, i)) {
            if ((w[i] & 0xFF00) == 0xD800)
                i++;
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

	bool long_c;
	for (; i < lg; i++) {
        long_c = false;
        if ((w[i] & 0xFF00) == 0xD800) {
            long_c = true;
            if (first == -1)
                first = i;
        }
        if (!utf8_handler->scan_emoji(w, i)) {
            if (long_c)
                i++;
        }
		sz++;
	}

	return sz;
}

//We convert a position in characters into an actual string position
long convertchartoposutf16(wstring& w, long first, long cpos) {
	long  i = first;
	long j;
	while (first != cpos) {
        if (!utf8_handler->scan_emoji(w, i)) {
            if ((w[i] & 0xFF00) == 0xD800) {
                i++;
            }
        }
        i++;
		first++;
	}
	return i;
}

long convertpostocharutf16(wstring& w, long first, long spos) {
	long  i = first;
	long j;
	while (i != spos) {
        if (!utf8_handler->scan_emoji(w, i)) {
            if ((w[i] & 0xFF00) == 0xD800) {
                i++;
            }
        }
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
        if (!utf8_handler->scan_emoji(w, i)) {
            if ((w[i] & 0xFF00) == 0xD800) {
                i++;
            }
        }
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
	TAMGUCHAR c;
	while (first != cpos) {
        if (!utf8_handler->scan_emoji(w, i)) {
            if ((w[i] & 0xFF00) == 0xD800) {
                i++;
            }
        }
        i++;
		first++;
	}
	return i;
}

long convertpostocharraw(wstring& w, long first, long spos) {
    long  i = first;
    while (i != spos) {
        if (!utf8_handler->scan_emoji(w, i)) {
            if ((w[i] & 0xFF00) == 0xD800) {
                i++;
            }
        }
        i++;
        first++;
    }
    return first;
}

long convertchartoposraw(wstring& w, long first, long cpos) {
    long  i = first;
    TAMGUCHAR c;
    while (first != cpos) {
        if (!utf8_handler->scan_emoji(w, i)) {
            if ((w[i] & 0xFF00) == 0xD800) {
                i++;
            }
        }
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
            if (!utf8_handler->scan_emoji(w, realpos)) {
                if ((w[realpos] & 0xFF00) == 0xD800) {
                    realpos++;
                }
            }
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
//-----------------------------------------------------------------------------------
//---------------------------- WSTRING_IS_UTF16 SECTION END -------------------------
//-----------------------------------------------------------------------------------
#else
//-----------------------------------------------------------------------------------
// In this case, wstring handles UTF32 characters...
//-----------------------------------------------------------------------------------
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
        utf8_handler->scan_emoji(w, realpos);
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
        utf8_handler->scan_emoji(w, realpos);
        realpos++;
        first++;
    }
    return first;
}

Exporting long convertchartoposraw(wstring& w, long first, long cpos) {
    long realpos = first;
    while (first != cpos) {
        utf8_handler->scan_emoji(w, realpos);
        realpos++;
        first++;
    }
    return realpos;
}

Exporting long convertpostocharraw(wstring& w, long first, long spos) {
    long realpos = first;
    while (realpos != spos) {
        utf8_handler->scan_emoji(w, realpos);
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
            utf8_handler->scan_emoji(w, realpos);
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
        utf8_handler->scan_emoji(w, i);
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
//-----------------------------------------------------------------------------------
//------------------END SPECIFIC SECTION FOR UTF16 wstring---------------------------
//-----------------------------------------------------------------------------------
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
    return conversion_utf8_to_fat(USTR(contenu), l);
}

Exporting void conversion_utf8_to_fatOne(string contenu, string& s) {
    conversion_utf8_to_fatOne(USTR(contenu), s);
}

//----------------------------------------------------------------------------------------------------
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
    str[0] = (char)m[i];
    switch (c_test_utf8(m + i)) {
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
    string str;
    str = (char)m[i];
    long nb = c_test_utf8(m + i);
    if (nb == 0)
        return str;

    if (!utf8_handler->get_emoji(m, str, i)) {
        switch (nb) {
            case 1:
                str = (char)m[i+1];
                break;
            case 2:
                str = (char)m[i+1];
                str += (char)m[i+2];
                break;
            case 3:
                str = (char)m[i+1];
                str += (char)m[i+2];
                str += (char)m[i+3];
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
    
    while (i < sz) {
        if (!utf8_handler->scan_emoji(contenu, i))
            i += c_test_utf8(contenu + i);
        i++;
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
        if (nb && first == -1)
            first = size;
        
        if (!utf8_handler->scan_emoji(contenu, i))
            i += nb;
        i++;
        ++size;
    }
    
    return size;
}

Exporting long size_c(string& s) {
    if (s.empty())
        return 0;
    long lg = s.size();
    uchar* contenu = USTR(s);
    long i = 0;
    
#ifdef INTELINTRINSICS
    if (check_ascii(contenu,lg,i))
        return lg;
#endif
    
    long sz = i;
    while (i < lg) {
        if (!utf8_handler->scan_emoji(contenu, i))
            i += c_test_utf8(contenu + i);
        i++;
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
        utf8_handler->scan_emoji(w, i);
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
        utf8_handler->scan_emoji(w, i);
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
    
    while (i > 0 && x < lg) {
        if (!utf8_handler->scan_emoji(contenu, x))
            x += c_test_utf8(contenu + x);
        x++;
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

    while (p < v.size()) {
        while (i < v[p]) {
            if (!utf8_handler->scan_emoji(s, i))
                i += c_test_utf8(s + i);
            i++;
            c++;
        }
        v[p++] = c;
    }
}

Exporting void v_convertchartobyteposition(unsigned char* contenu, vector<long>& v) {
    long c = 0;
    long iv = 0;
    long i = 0;
    long sz = v.size();
    while(iv < sz) {
        while (v[iv] == i)
            v[iv++] = c;
        if (!utf8_handler->scan_emoji(contenu, c))
            c += c_test_utf8(contenu + c);
        c++;
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
    
    for (const auto& it : keeps) {
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
    rules.push_back("0b[1 0]+=0");                                          //binary numbers
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
            if (value.empty())
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
            case '-':
                if (!iblank && codestr[i] == '-' && codestr[i+1] == '>') {
                    prologmode = true;
                    iblank += blanksize;
                }
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
            case '-':
                if (!iblank && codestr[i] == '-' && codestr[i+1] == '>') {
                    prologmode = true;
                    iblank += blanksize;
                }
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

//----------------------------------------------------------------------------------------------------
//----------------------------- UTF8_Handler Implementation ------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
UTF8_Handler::UTF8_Handler() : punctuations(32, 11), utf8codemin(32, 11), numbers(false) {
    InitNumbers();
    InitLatinTables();
    InitDosCodes();
    InitHtmlCodes();
    InitKoreanCodes();
            
    TAMGUCHAR unicode;
    bulongchar xs;
    bulongchar xse;
    uchar latin;
    long i;
    int maxtable;
    for (maxtable = 0; codingtable[maxtable] != -1; maxtable++);
    
    
    for (i = 0; i < maxtable; i += 3) {
        unicode = (TAMGUCHAR)codingtable[i];
        TAMGUCHAR equ = (TAMGUCHAR)codingtable[i + 1];
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

//------------------------------------------------------------------------------------------------------------
void UTF8_Handler::InitKoreanCodes() {
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

//------------------------------------------------------------------------------------------------------------

void UTF8_Handler::InitNumbers() {
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
void UTF8_Handler::InitDosCodes() {
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

void UTF8_Handler::InitHtmlCodes() {
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

void UTF8_Handler::InitLatinTables() {
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

void Emojis::store() {
    long i = 0;
    long j;
    char32_t utf32_c;
    Emoji_Arc* a;
    string utf8;
    wstring utf16;
    wstring utf16_c;
    char utf_c[5];
    while (emoji_sequences[i][0]) {
        utf32_c = emoji_sequences[i][0];
        std::unordered_map<char32_t, Emoji_Arc*>::iterator it = utf32_arcs.find(utf32_c);
        if (it == utf32_arcs.end()) {
            a = new Emoji_Arc(utf32_c);
            utf32_arcs[utf32_c] = a;
        }
        else
            a = it->second;
        
        if (emoji_sequences[i][1])
            traverse(a, &emoji_sequences[i][1]);
        else
            a->end = true;
        
        utf8 = unicode_2_utf8(utf32_c, utf_c);
        j = 1;
        while (emoji_sequences[i][j]) {
            utf8 += unicode_2_utf8(emoji_sequences[i][j], utf_c);
            j++;
        }
        
        std::unordered_map<char, Emoji_Arc*>::iterator itbytes = utf8_arcs.find(utf8[0]);
        if (itbytes == utf8_arcs.end()) {
            a = new Emoji_Arc((uchar)utf8[0]);
            utf8_arcs[(uchar)utf8[0]] = a;
        }
        else
            a = itbytes->second;
        
        traverse(a, utf8);
        
        //UTF16
        c_unicode_to_utf16(utf16, (uint32_t)utf32_c);
        j = 1;
        while (emoji_sequences[i][j]) {
            c_unicode_to_utf16(utf16_c, (uint32_t)emoji_sequences[i][j]);
            utf16 += utf16_c;
            j++;
        }
        
        std::unordered_map<wchar_t, Emoji_Arc*>::iterator itutf16 = utf16_arcs.find(utf16[0]);
        if (itutf16 == utf16_arcs.end()) {
            a = new Emoji_Arc(utf16[0]);
            utf16_arcs[utf16[0]] = a;
        }
        else
            a = itutf16->second;
        
        traverse(a, utf16);
        
        i++;
    }
    
    for (const auto& a : utf32_arcs)
        a.second->trim();
    
    for (const auto& a : utf8_arcs)
        a.second->trim();
    
    for (const auto& a : utf16_arcs)
        a.second->trim();
}
//----------------------------------------------------------------------------------------------------

string UTF8_Handler::s_utf8_to_latin(unsigned char* contenu, long sz) {
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

string UTF8_Handler::conversion_unicode_to_latin(wstring& w) {
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

string UTF8_Handler::conversion_latin_to_utf8(unsigned char* contenu, long sz) {
    string res;
    s_latin_to_utf8(res, contenu, sz);
    return res;
}

string UTF8_Handler::conversion_latin_table_to_utf8(short tableindex, unsigned char* contenu, long sz) {
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

wstring UTF8_Handler::conversion_latin_table_to_unicode(short tableindex, unsigned char* contenu, long sz) {
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

string UTF8_Handler::s_unicode_to_dos(wstring& contenu) {
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


string UTF8_Handler::s_utf8_to_dos(unsigned char* contenu) {
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

string UTF8_Handler::s_utf8_to_dos(const char* contenu) {
    return s_utf8_to_dos((unsigned char*)contenu);
}

string UTF8_Handler::s_dos_to_utf8(unsigned char* contenu) {
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

string UTF8_Handler::s_dos_to_utf8(const char* contenu) {
    return s_dos_to_utf8((unsigned char*)contenu);
}

void UTF8_Handler::s_latin_to_utf8(string& res, unsigned char* contenu, long sz) {
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

string UTF8_Handler::conversion_utf8_to_latin(string& contenu) {
    return s_utf8_to_latin(USTR(contenu), contenu.size());
}

string UTF8_Handler::conversion_latin_to_utf8(string& contenu) {
    return conversion_latin_to_utf8(USTR(contenu), contenu.size());
}

TAMGUCHAR UTF8_Handler::c_latin_table_to_unicode(int tableindex, TAMGUCHAR c) {
    if (c < 128 || c > 255 || !alllatincodes.check(tableindex))
        return c;
    
    if (alllatincodes[tableindex]->check(c))
        return (*alllatincodes[tableindex])[c];
    
    return c;
}

uchar UTF8_Handler::c_latin_to_utf8(unsigned char code, unsigned char* utf) {
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


uchar UTF8_Handler::c_latin_to_utf8(unsigned char* utf, unsigned char* res, bool checktable) {
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

TAMGUCHAR UTF8_Handler::c_unicode_to_latin(TAMGUCHAR u) {
    if (u < 0x0080)
        return u;
    
    if (codeslatin.check(u))
        return codeslatin[u];
    
    return 0;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

bool UTF8_Handler::scan_emoji(unsigned char* u, long& i) {
    return emojis_characters.scan(u, i);
}

bool UTF8_Handler::get_emoji(unsigned char* u, string& res, long& i) {
    return emojis_characters.get(u, res, i);
}

bool UTF8_Handler::store_emoji(unsigned char* u, string& res, long& i) {
    return emojis_characters.store(u, res, i);
}

bool UTF8_Handler::scan_emoji(string& u, long& i) {
    return emojis_characters.scan(u, i);
}

bool UTF8_Handler::get_emoji(string& u, string& res, long& i) {
    return emojis_characters.get(u, res, i);
}

bool UTF8_Handler::store_emoji(string& u, string& res, long& i) {
    return emojis_characters.store(u, res, i);
}

#ifdef WIN32
Exporting void concat_to_wstring(wstring& res, TAMGUCHAR code) {
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
        res += (wchar_t)code;
}

bool UTF8_Handler::scan_emoji(wstring& u, long& i) {
    return emojis_characters.scan16(u, i);
}

bool UTF8_Handler::get_emoji(wstring& u, wstring& res, long& i) {
    return emojis_characters.get16(u, res, i);
}

bool UTF8_Handler::store_emoji(wstring& u, wstring& res, long& i) {
    return emojis_characters.store16(u, res, i);
}
#else
bool UTF8_Handler::scan_emoji(wstring& u, long& i) {
    return emojis_characters.scan(u, i);
}

bool UTF8_Handler::get_emoji(wstring& u, wstring& res, long& i) {
    return emojis_characters.get(u, res, i);
}

bool UTF8_Handler::store_emoji(wstring& u, wstring& res, long& i) {
    return emojis_characters.store(u, res, i);
}
#endif

bool UTF8_Handler::scan_emoji16(wstring& u, long& i) {
    return emojis_characters.scan16(u, i);
}

bool UTF8_Handler::get_emoji16(wstring& u, wstring& res, long& i) {
    return emojis_characters.get16(u, res, i);
}

bool UTF8_Handler::store_emoji16(wstring& u, wstring& res, long& i) {
    return emojis_characters.store16(u, res, i);
}

bool UTF8_Handler::scan_emoji(u32string& u, long& i) {
    return emojis_characters.scan(u, i);
}

bool UTF8_Handler::get_emoji(u32string& u, u32string& res, long& i) {
    return emojis_characters.get(u, res, i);
}

bool UTF8_Handler::store_emoji(u32string& u, u32string& res, long& i) {
    return emojis_characters.store(u, res, i);
}

bool UTF8_Handler::c_is_emoji(TAMGUCHAR c) {
    return emojis_characters.isemoji(c);
}

bool UTF8_Handler::c_is_emojicomp(TAMGUCHAR c) {
    return emojis_characters.isemojicomplement(c);
}

bool UTF8_Handler::c_is_emoji(unsigned char* m, long& i) {
    return emojis_characters.isemoji(getonechar(m, i));
}

bool UTF8_Handler::c_is_emojicomp(unsigned char* m, long& i) {
    return emojis_characters.isemojicomplement(getonechar(m, i));
}

bool UTF8_Handler::s_is_emoji(string& s) {
    if (s.empty())
        return false;
    
    long lg = s.size();
    long p;
    for (long i = 0; i < lg; i++) {
        p = i;
        if (scan_emoji(s, p)) {
            i = p;
            continue;
        }
        return false;
    }
    return true;
}

bool UTF8_Handler::u_is_emoji(u32string& s) {
    if (s == U"")
        return false;
    long lg = s.size();
    
    for (long i = 0; i < lg; i++) {
        if (scan_emoji(s, i))
            continue;
        return false;
    }
    return true;
}

bool UTF8_Handler::s_is_emoji(wstring& s) {
    if (s == L"")
        return false;
    long lg = s.size();
    
    for (long i = 0; i < lg; i++) {
        if (scan_emoji(s, i))
            continue;
        return false;
    }
    return true;
}

void UTF8_Handler::convertnumber(long v, string& s) {
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

void UTF8_Handler::convertnumber(long v, wstring& s) {
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

void UTF8_Handler::convertnumber(double val, string& s) {
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

void UTF8_Handler::convertnumber(double val, wstring& s) {
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

string UTF8_Handler::convertfromnumber(BLONG v) {
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

string UTF8_Handler::convertfromnumber(long v) {
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

string UTF8_Handler::convertfromnumber(short v) {
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

wstring UTF8_Handler::wconvertfromnumber(BLONG v) {
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

wstring UTF8_Handler::wconvertfromnumber(long v) {
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

wstring UTF8_Handler::wconvertfromnumber(short v) {
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

string UTF8_Handler::convertfromnumber(double val) {
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

string UTF8_Handler::convertfromnumber(float val) {
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

wstring UTF8_Handler::wconvertfromnumber(double val) {
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

wstring UTF8_Handler::wconvertfromnumber(float val) {
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

void UTF8_Handler::c_HTMLcode(string& s, TAMGUCHAR code) {
    if (codeshtml.check(code))
        s = codeshtml[code];
    else {
        char ch[10];
        sprintf_s(ch, 10, "&%ld;", (long)code);
        s = ch;
    }
}

void UTF8_Handler::c_XMLcode(string& s, TAMGUCHAR code) {
    if (codesxml.check(code))
        s = codesxml[code];
    else {
        char ch[10];
        sprintf_s(ch, 10, "&%ld;", (long)code);
        s = ch;
    }
}

void UTF8_Handler::s_EvaluateMetaCharacters(string& s) {
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

void UTF8_Handler::s_EvaluateMetaCharacters(wstring& s) {
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


bool UTF8_Handler::s_is_punctuation(wstring& str) {
    for (long i = 0; i < str.size(); i++) {
        if (!punctuations.check(str[i]))
            return false;
    }
    return true;
}

bool UTF8_Handler::c_is_punctuation(TAMGUCHAR code) {
    return punctuations.check(code);
}

bool UTF8_Handler::c_is_separator(TAMGUCHAR code) {
    return (c_is_space(code) || c_is_punctuation(code));
}

bool UTF8_Handler::s_is_punctuation(string& str) {
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

bool UTF8_Handler::s_is_upper(string& s) {
    if (s.empty())
        return false;
    long lg = s.size();
    for (long i = 0; i < lg; i++) {
        char ty = c_is_alpha(USTR(s), i);
        if (ty != 2)
            return false;
    }
    return true;
}

bool UTF8_Handler::s_is_alpha(wstring& s) {
    if (s == L"")
        return false;
    for (long i = 0; i < s.size(); i++) {
        if (!utf8codemaj.check(s[i]) && !utf8codemin.check(s[i]))
            return false;
    }
    return true;
}


bool UTF8_Handler::s_is_upper(wstring& s) {
    if (s == L"")
        return false;
    for (long i = 0; i < s.size(); i++) {
        if (!utf8codemaj.check(s[i]))
            return false;
    }
    return true;
}

bool UTF8_Handler::s_is_lower(wstring& s) {
    if (s == L"")
        return false;
    for (long i = 0; i < s.size(); i++) {
        if (!utf8codemin.check(s[i]))
            return false;
    }
    return true;
}

bool UTF8_Handler::c_is_consonant(TAMGUCHAR c) {
    if (!consonants.check(c))
        return false;
    return true;
}

bool UTF8_Handler::c_is_vowel(TAMGUCHAR c) {
    if (!vowels.check(c))
        return false;
    return true;
}


bool UTF8_Handler::s_is_consonant(wstring& s) {
    if (s == L"")
        return false;
    for (long i = 0; i < s.size(); i++) {
        if (!consonants.check(s[i]))
            return false;
    }
    return true;
}

bool UTF8_Handler::s_is_vowel(wstring& s) {
    if (s == L"")
        return false;
    for (long i = 0; i < s.size(); i++) {
        if (!vowels.check(s[i]))
            return false;
    }
    return true;
}


bool UTF8_Handler::s_is_consonant(unsigned char* s) {
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

bool UTF8_Handler::s_is_vowel(unsigned char* s) {
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

void UTF8_Handler::s_binary(unsigned char* s, int mx, long lg, TAMGUCHAR* codes) {
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

bool UTF8_Handler::compare_vowel(TAMGUCHAR c, TAMGUCHAR cc) {
    if (wvowels.check(c))
        c=wvowels[c];
    
    if (wvowels.check(cc))
        cc=wvowels[cc];
    
    return (c == cc);
}

wstring UTF8_Handler::s_deaccentuate(wstring& s) {
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

string UTF8_Handler::s_deaccentuate(unsigned char* s) {
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

bool UTF8_Handler::s_is_alpha(string& s) {
    if (s.empty())
        return false;
    long lg = s.size();
    for (long i = 0; i < lg; i++) {
        char ty = c_is_alpha(USTR(s), i);
        if (ty == 0)
            return false;
    }
    return true;
}

bool UTF8_Handler::s_is_lower(string& s) {
    if (s.empty())
        return false;
    
    long lg = s.size();
    for (long i = 0; i < lg; i++) {
        char ty = c_is_alpha(USTR(s), i);
        if (ty != 1)
            return false;
    }
    return true;
}

TAMGUCHAR UTF8_Handler::c_to_lower(TAMGUCHAR c) {
    if (utf8codemaj.check(c))
        return utf8codemaj[c];
    return c;
}

TAMGUCHAR UTF8_Handler::c_to_upper(TAMGUCHAR c) {
    if (utf8codemin.check(c))
        return utf8codemin[c];
    return c;
}

bool UTF8_Handler::b_alpha(TAMGUCHAR c) {
    if (utf8codemaj.check(c) || utf8codemin.check(c))
        return true;
    return false;
}

//return 0 (not alpha), 1 (lower), 2 (upper)
char UTF8_Handler::c_is_alpha(TAMGUCHAR code) {
    if (utf8codemin.check(code))
        return 1;
    
    if (utf8codemaj.check(code))
        return 2;
    return 0;
}

bool UTF8_Handler::c_is_lower(TAMGUCHAR code) {
    return utf8codemin.check(code);
}

bool UTF8_Handler::c_is_upper(TAMGUCHAR code) {
    return utf8codemaj.check(code);
}

wstring UTF8_Handler::s_to_lower(wstring& s) {
    wstring res;
    long lg = s.size();
    for (long i = 0; i < lg; i++)
        res += (wchar_t)c_to_lower(s[i]);
    return res;
}

wstring UTF8_Handler::s_to_upper(wstring& s) {
    wstring res;
    long lg = s.size();
    for (long i = 0; i < lg; i++)
        res += (wchar_t)c_to_upper(s[i]);
    return res;
}


string UTF8_Handler::s_to_lower(string& s) {
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

string UTF8_Handler::s_to_upper(string& s) {
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

void UTF8_Handler::c_to_lower(string& res, unsigned char* s, long lg) {
    res = "";
	TAMGUCHAR v;
    for (long i = 0; i < lg; i++) {
        i += c_utf8_to_unicode(s + i, v);
        if (utf8codemaj.check(v))
            v = utf8codemaj[v];
        
        res += c_unicode_to_utf8(v);
    }
}

void UTF8_Handler::c_to_upper(string& res, unsigned char* s, long lg) {
    res = "";
	TAMGUCHAR v;
    for (long i = 0; i < lg; i++) {
        i += c_utf8_to_unicode(s + i, v);
        if (utf8codemin.check(v))
            v = utf8codemin[v];
        
        res += c_unicode_to_utf8(v);
    }
}

bool UTF8_Handler::c_is_hangul(string& s) {
	TAMGUCHAR c;
    c_utf8_to_unicode(USTR(s), c);
    if (hangulutf8.check(c) || hanguljamo.check(c))
        return true;
    return false;
}

bool UTF8_Handler::s_is_hangul(wstring& w) {
    for (long i = 0; i < w.size(); i++) {
        if (!hangulutf8.check(w[i]) && !hanguljamo.check(w[i]))
            return false;
    }
    return true;
}

bool UTF8_Handler::c_is_hangul(TAMGUCHAR c) {
    if (hangulutf8.check(c) || hanguljamo.check(c))
        return true;
    return false;
}

string UTF8_Handler::c_translate(TAMGUCHAR c) {
    if (transliteration.check(c))
        return transliteration[c];
    return "";
}

string UTF8_Handler::c_translate(unsigned char* m, long& i) {
	TAMGUCHAR c;
    i += c_utf8_to_unicode(m + i, c);
    if (transliteration.check(c))
        return transliteration[c];
    return "";
}

bool UTF8_Handler::c_is_hangul(unsigned char* m, long& i) {
	TAMGUCHAR c;
    i += c_utf8_to_unicode(m + i, c);
    if (!hangulutf8.check(c) && !hanguljamo.check(c))
        return false;
    return true;
}


bool UTF8_Handler::s_is_jamo(wstring& w) {
    for (long i = 0; i < w.size(); i++) {
        if (!hanguljamo.check(w[i]))
            return false;
    }
    
    return true;
}

wstring UTF8_Handler::s_split_jamo(wstring& w) {
    wstring j;
    for (long i = 0; i < w.size(); i++) {
        if (hanguljamo.check(w[i]))
            j += hanguljamo[w[i]];
        else
            j += w[i];
    }
    return j;
}

wstring UTF8_Handler::s_combine_jamo(wstring& w) {
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

wstring UTF8_Handler::s_hangul_normalize(wstring& w) {
    wstring res;
    for (long i = 0; i < w.size(); i++) {
        if (hangulconvert.check(w[i]))
            res += hangulconvert[w[i]];
        else
            res += w[i];
    }
    return res;
}

char UTF8_Handler::c_is_alpha(unsigned char* m, long& i) {
    TAMGUCHAR v;
    i += c_utf8_to_unicode(m + i, v);
    
    if (utf8codemin.check(v))
        return 1;
    if (utf8codemaj.check(v))
        return 2;
    
    return 0;
}


//------------------------------------------------------------------------------
// DoubleMetaphone algorithm
//------------------------------------------------------------------------------

const unsigned int max_length = 32;

char IsVowel(string& s, unsigned long pos) {
    if (pos >= s.size())
        return 0;
    return utf8_handler->vowels.check(s[pos]);
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


