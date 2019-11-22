/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tools.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#ifdef WIN32
#include "Windows.h"
#endif

#include "codeparse.h"
#include "tamgu.h"
#include "compilecode.h"
#include "tamgustring.h"
#include "tamguvector.h"
#include "tamguivector.h"
#include "tamgufvector.h"
#include "tamgusvector.h"
#include "tamguuvector.h"
#include "tamgubvector.h"
#include "tamgudvector.h"
#include "tamgumap.h"
#include "tamguprimemap.h"
#include "tamguprimemapss.h"
#include "tamguversion.h"
#include "tamgulvector.h"
#include "tamguhvector.h"
#include "tamguautomaton.h"
#include "x_tokenize.h"

#ifdef UNIX
#include <unistd.h>
#include <dlfcn.h>
#include <signal.h>
#include <termios.h>
#include <sys/time.h>
#endif

#ifdef WIN32
#include <io.h>
#define DUP _dup
#define DUP2 _dup2
#define FILENO _fileno
#else
#define FILENO fileno
#define DUP dup
#define DUP2 dup2
#endif

//---------------------------------------------------------
#ifdef WIN32
bool errorsprintf = false;
void wrongSprintf(const wchar_t* expression,
                  const wchar_t* function,
                  const wchar_t* file,
                  unsigned int line,
                  uintptr_t pReserved) {
    errorsprintf = true;
}
#endif

const char ANTISEP = '\\';

#ifdef WIN32
const char SEP = '\\';
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

//Note: copied from: https://stackoverflow.com/questions/10905892/equivalent-of-gettimeday-for-windows
long gettimeofday(struct timeval *tv, struct timezone *tz) {
    
    FILETIME ft;
    unsigned __int64 tmpres = 0;
    
    if (NULL != tv) {
        GetSystemTimeAsFileTime(&ft);
        
        tmpres |= ft.dwHighDateTime;
        tmpres <<= 32;
        tmpres |= ft.dwLowDateTime;
        
        /*converting file time to unix epoch*/
        tmpres -= DELTA_EPOCH_IN_MICROSECS;
        tmpres /= 10;  /*convert into microseconds*/
        tv->tv_sec = (long)(tmpres / 1000000UL);
        tv->tv_usec = (long)(tmpres % 1000000UL);
    }
    
    return 0;
}
#endif

#ifdef UNIX
const char SEP = '/';
#endif

//-----------------------------------------------------------------------------------
unsigned long long Hashkey(string& s) {
    return std::use_facet<std::collate<char> >(loc).hash(s.data(), s.data() + s.length());
}

unsigned long long Hashkey(wstring& s) {
    return std::use_facet<std::collate<wchar_t> >(loc).hash(s.data(), s.data() + s.length());
}
//-----------------------------------------------------------------------------------
char* convertir(char* res) {
    string conversion = conversion_latin_to_utf8(res);
    char* r = (char*)malloc(conversion.size() + 1);
    strcpy(r, STR(conversion));
    return r;
}
//-----------------------------------------------------------------------------------

double plustime(timeval& tempsinitial, timeval& tempsfinal) {
    double init = (((unsigned long)tempsinitial.tv_sec) * 1000 + ((unsigned long)tempsinitial.tv_usec) / 1000.0) + 0.5;
    double final = (((unsigned long)tempsfinal.tv_sec) * 1000 + ((unsigned long)tempsfinal.tv_usec) / 1000.0) + 0.5;
    return(final + init);
}

double minustime(timeval& tempsfinal, timeval& tempsinitial) {
    double init = (((unsigned long)tempsinitial.tv_sec) * 1000 + ((unsigned long)tempsinitial.tv_usec) / 1000.0);
    double final = (((unsigned long)tempsfinal.tv_sec) * 1000 + ((unsigned long)tempsfinal.tv_usec) / 1000.0);
    return(final - init);
}


long timeminus(double init, timeval& tempsfinal) {
    double final = (((unsigned long)tempsfinal.tv_sec) * 1000 + ((unsigned long)tempsfinal.tv_usec) / 1000.0);
    return(final - init);
}

//---------------------------------------------------------
static bool tamgurestrandom = false;
static ThreadLock randomlock;
double localrandom(long mx) {
    if (!mx)
        mx = 1;
    static unsigned long x = 123456789;
    static unsigned long y = 362436069;
    static unsigned long z = 521288629;
    static long w = time(0);
    unsigned long t;
    if (tamgurestrandom) {
        w = time(0);
        tamgurestrandom = false;
    }
    
    Locking _lock(randomlock);
    t = x ^ (x << 11);
    x = y; y = z; z = w;
    w = w ^ (w >> 19) ^ (t ^ (t >> 8));
    return abs(w%mx);
}

double a_localrandom(long mx) {
    if (!mx)
        mx = 1;
    static unsigned long x = 123456789;
    static unsigned long y = 362436069;
    static unsigned long z = 521288629;
    static long w = time(0);
    unsigned long t;
    if (tamgurestrandom) {
        w = time(0);
        tamgurestrandom = false;
    }
    
    t = x ^ (x << 11);
    x = y; y = z; z = w;
    w = w ^ (w >> 19) ^ (t ^ (t >> 8));
    return abs(w%mx);
}


//---------------------------------------------------------
bool IsLong(BLONG v) {
    if (v < -2147483647 || v > 2147483647)
        return true;
    return false;
}

bool IsShort(BLONG v) {
    if (v < -32768 || v > 32767)
        return false;
    
    return true;
}

//---------------------------------------------------------
bool v_comma_split_string(string& thestr, vector<string>& v) {
    size_t sz = thestr.size() - 1;
    if (thestr[0] != '[' || thestr[sz] != ']')
        return false;
    if (sz == 1)
        return true;
    
    size_t pos;
    bool comma = true;
    string value;
    uchar c, nxt;
    
    for (pos = 1; pos < sz; pos++) {
        c = thestr[pos];
        if (c  <= 32)
            continue;
        
        if (c == ',') {
            if (comma) {
                return false;
            }
            comma = true;
            continue;
        }

        if (!comma)
            return false;
        
        comma = false;

        if (c != '"' && c != 39 && c != '@')
            return false;
        
        if (c == '@') {
            nxt = '"';
            if (thestr[++pos] != '"') {
                return false;
            }
        }
        else
            nxt = c;

        value = "";
        pos++;
        while (pos < sz && thestr[pos] != nxt)
            value += thestr[pos++];
        
        if (pos == sz) {
            return false;
        }
        if (c == '@') {
            if (thestr[++pos] != '@') {
                return false;
            }
        }
        v.push_back(value);
    }
    
    if (comma) {
        return false;
    }
    return true;
}

bool v_comma_split_string(wstring& thestr, vector<wstring>& v) {
    size_t sz = thestr.size() - 1;
    if (thestr[0] != '[' || thestr[sz] != ']')
        return false;
    if (sz == 1)
        return true;
    
    size_t pos;
    bool comma = true;
    wstring value;
    wchar_t c, nxt;
    
    for (pos = 1; pos < sz; pos++) {
        c = thestr[pos];
        if (c  <= 32)
            continue;
        
        if (c == ',') {
            if (comma) {
                return false;
            }
            comma = true;
            continue;
        }
        
        if (!comma)
            return false;

        comma = false;

        if (c != '"' && c != 39 && c != '@') {
            return false;
        }
        
        if (c == '@') {
            nxt = '"';
            if (thestr[++pos] != '"') {
                return false;
            }
        }
        else
            nxt = c;

        value = L"";
        pos++;
        while (pos < sz && thestr[pos] != nxt)
            value += thestr[pos++];

        if (pos == sz) {
            return false;
        }
        if (c == '@') {
            if (thestr[++pos] != '@') {
                return false;
            }
        }
        v.push_back(value);
    }
    
    if (comma) {
        return false;
    }
    return true;
}

#define isdigit(c) (c >= '0' && c <= '9')

bool v_comma_split_decimal(string& thestr, vector<float>& v) {
    size_t sz = thestr.size() - 1;
    if (thestr[0] != '[' || thestr[sz] != ']')
        return false;
    if (sz == 1)
        return true;
    
    size_t pos;
    bool comma = true;
    float d;
    short l;
    uchar c;
    
    for (pos = 1; pos < sz; pos++) {
        c = thestr[pos];
        if (c  <= 32)
            continue;
        
        if (c == ',') {
            if (comma) {
                return false;
            }
            comma = true;
            continue;
        }
        
        if (!comma)
            return false;

        comma = false;

        if (c == '-' || c == '+' || isdigit(c)) {
            d = conversionfloathexa(STR(thestr)+pos, l);
            v.push_back(d);
            pos += l - 1;
            continue;
        }
        return false;
    }
    
    if (comma) {
        return false;
    }
    return true;
}

bool v_comma_split_float(string& thestr, vector<double>& v) {
    size_t sz = thestr.size() - 1;
    if (thestr[0] != '[' || thestr[sz] != ']')
        return false;
    if (sz == 1)
        return true;
    
    size_t pos;
    bool comma = true;
    double d;
    short l;
    uchar c;
    
    for (pos = 1; pos < sz; pos++) {
        c = thestr[pos];
        if (c  <= 32)
            continue;
        
        if (c == ',') {
            if (comma) {
                return false;
            }
            comma = true;
            continue;
        }

        if (!comma)
            return false;
        comma = false;
        
        if (c == '-' || c == '+' || isdigit(c)) {
            d = conversionfloathexa(STR(thestr)+pos, l);
            v.push_back(d);
            pos += l - 1;
            continue;
        }

        return false;
    }
    
    if (comma) {
        return false;
    }
    return true;
}

bool v_comma_split_int(string& thestr, vector<long>& v) {
    size_t sz = thestr.size() - 1;
    if (thestr[0] != '[' || thestr[sz] != ']')
        return false;
    if (sz == 1)
        return true;
    
    size_t pos;
    bool comma = true;
    long d;
    short l;
    uchar c;
    
    for (pos = 1; pos < sz; pos++) {
        c = thestr[pos];
        if (c  <= 32)
            continue;
        
        if (c == ',') {
            if (comma) {
                return false;
            }
            comma = true;
            continue;
        }

        if (!comma)
            return false;
        comma = false;
        
        if (c == '-' || c == '+' || isdigit(c)) {
            d = conversionfloathexa(STR(thestr)+pos, l);
            v.push_back(d);
            pos += l - 1;
            continue;
        }

        return false;
    }

    if (comma) {
        return false;
    }
    return true;
}

bool v_comma_split_long(string& thestr, vector<BLONG>& v) {
    size_t sz = thestr.size() - 1;
    if (thestr[0] != '[' || thestr[sz] != ']')
        return false;
    if (sz == 1)
        return true;
    
    size_t pos;
    bool comma = true;
    BLONG d;
    short l;
    uchar c;
    
    for (pos = 1; pos < sz; pos++) {
        c = thestr[pos];
        if (c  <= 32)
            continue;
        
        if (c == ',') {
            if (comma) {
                return false;
            }
            comma = true;
            continue;
        }

        if (!comma)
            return false;
        comma = false;
        
        if (c == '-' || c == '+' || isdigit(c)) {
            d = conversionfloathexa(STR(thestr)+pos, l);
            v.push_back(d);
            pos += l - 1;
            continue;
        }

        return false;
    }

    if (comma) {
        return false;
    }
    return true;
}

bool v_comma_split_byte(string& thestr, vector<uchar>& v) {
    size_t sz = thestr.size() - 1;
    if (thestr[0] != '[' || thestr[sz] != ']')
        return false;
    if (sz == 1)
        return true;
    
    size_t pos;
    bool comma = true;
    uchar d;
    short l;
    uchar c;
    
    for (pos = 1; pos < sz; pos++) {
        c = thestr[pos];
        if (c  <= 32)
            continue;
        
        if (c == ',') {
            if (comma) {
                return false;
            }
            comma = true;
            continue;
        }

        if (!comma)
            return false;
        comma = false;
        
        if (c == '-' || c == '+' || isdigit(c)) {
            d = conversionfloathexa(STR(thestr)+pos, l);
            v.push_back(d);
            pos += l - 1;
            continue;
        }

        return false;
    }

    if (comma) {
        return false;
    }
    return true;
}

bool v_comma_split_short(string& thestr, vector<short>& v) {
    size_t sz = thestr.size() - 1;
    if (thestr[0] != '[' || thestr[sz] != ']')
        return false;
    if (sz == 1)
        return true;
    
    size_t pos;
    bool comma = true;
    short d;
    short l;
    uchar c;
    
    for (pos = 1; pos < sz; pos++) {
        c = thestr[pos];
        if (c  <= 32)
            continue;
        
        if (c == ',') {
            if (comma) {
                return false;
            }
            comma = true;
            continue;
        }

        if (!comma)
            return false;
        comma = false;
        
        if (c == '-' || c == '+' || isdigit(c)) {
            d = conversionfloathexa(STR(thestr)+pos, l);
            v.push_back(d);
            pos += l - 1;
            continue;
        }

        return false;
    }

    if (comma) {
        return false;
    }
    
    return true;
}
//---------------------------------------------------------------------------------------------

Tamgu* Mapcompare(Tamgu*a, Tamgu*b, TamguGlobal* global) {
    if (!b->isMapContainer() || a->Size() != b->Size())
        return aFALSE;
    
    if (!a->Size())
        return aTRUE;
    
    Tamgu* key;
    Tamgu* value;
    Tamgu* vb;
    Tamgu* res;
    Locking _lock((TamguObject*)a);
    
    TamguIteration* itr = a->Newiteration(false);
    for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
        value = itr->IteratorValue();
        key = itr->IteratorKey();
        res = aFALSE;
        switch (key->Type()) {
            case a_long:
            case a_int:
                vb = b->Value(key->Integer());
                if (vb != aNOELEMENT)
                    res = vb->same(value);
                break;
            case a_float:
                vb = b->Value(key->Float());
                if (vb != aNOELEMENT)
                    res = vb->same(value);
                break;
            default:
                vb = b->Value(key->String());
                if (vb != aNOELEMENT)
                    res = vb->same(value);
        }
        
        vb->Release();
        if (res == aFALSE) {
            itr->Release();
            return aFALSE;
        }
    }
    itr->Release();
    return aTRUE;
}


//---------------------------------------------------------------------------
#ifdef WSTRING_IS_UTF16

long checkemoji(wstring& svalue, long i) {
	if ((svalue[i] & 0xFF00) == 0xD800) {
		TAMGUCHAR c = getachar(svalue, i);
		if (((c & 0x1F000) == 0x1F000) && c_is_emoji(c)) {
			long j = i + 1;
			c = getachar(svalue, j);
			while (c_is_emojicomp(c)) {
				j++;
				c = getachar(svalue, j);
			}
			i = j;
		}
	}
	return i;
}

Exporting char StringIndexes(wstring& svalue, Tamgu* index, long& ileft, long& iright, short idthread) {
    
    Tamgu* left = index;
    Tamgu* right = NULL;
    long sz = svalue.size();

	//On Windows wstring are encoded in UTF16
	//We need to check for large UTF16 characters (on 4 bytes)
	long pivot = -1;
	long szchar = size_w(svalue, pivot);

    bool sleft = false;
    bool sright = false;
    bool releft = false;
    bool reright = false;
    
    iright = -1;
    
    if (index->isIndex()) {
        TamguIndex* kind = (TamguIndex*)index;
        sleft = kind->signleft;
        sright = kind->signright;
        left = kind->left->Eval(aNULL, aNULL, idthread);
        if (left != kind->left)
            releft = true;
        
        if (kind->interval == true) {
            right = kind->right->Eval(aNULL, aNULL, idthread);
            if (right != kind->right)
                reright = true;
        }
    }
    
    
    if (left->isRegular()) {
        //this is a regular expression...
        if (sleft) {
            //we need the last one...
            if (!left->searchlast(svalue, ileft, iright))
                return 0;
        }
        else {
            if (!left->search(svalue, ileft, iright))
                return 0;
        }
    }
    else {
        if (left->isString()) {
            wstring sub = left->UString();
            if (releft)
                left->Release();
            
            //then we are looking for a substring
            if (sleft)
                ileft = s_rfind(svalue, sub, sz);
            else
                ileft = s_find(svalue, sub, 0);
            
            if (ileft == -1)
                return 0;
            
            iright = ileft + sub.size();
        }
        else {
            ileft = left->Integer();
            if (releft)
                left->Release();

			if (pivot == -1) {
				if (ileft < 0)
					ileft += sz;
				if (ileft >= sz)
					return 0;
			}
			else {
				//We have to take into account a large UTF16 character
				if (ileft < 0)
					ileft += szchar;
				if (ileft >= szchar)
					return 0;
				if (ileft >= pivot)
					ileft = convertchartoposutf16(svalue, pivot, ileft);
			}
			if (ileft < 0)
				ileft = 0;
        }        
    }
    
    //We return as a non interval
    if (right == NULL) {
        if (iright != -1)
            return 2;
		if (pivot != -1) {
			iright = checkemoji(svalue, ileft);
			if (iright != ileft)
				return 2;
		}
        return 1;
    }
    
    if (right->isRegular()) {
        //this is a regular expression...
        long r = iright;
        
        if (sright) {
            //we need the last one...
            if (!right->searchlast(svalue, r, iright, r))
                return 0;
        }
        else {
            if (!right->search(svalue, r, iright, r))
                return 0;
        }
        if (iright <= ileft)
            return 0;
        return 2;
    }
    
    if (right->isString()) {
        wstring wr = right->UString();
        if (reright)
            right->Release();
        
        if (sright) {
            iright = s_rfind(svalue, wr, sz);
            if (iright <= ileft)
                return 0;
        }
        else {
            if (iright != -1)
                iright = s_find(svalue, wr, iright);
            else
                iright = s_find(svalue, wr, ileft);
        }
        
        if (iright == -1)
            return 0;
        
        iright += wr.size();
        return 2;
    }
    
    if (iright == -1) {//absolute position
        iright = right->Integer();
        if (reright)
            right->Release();
        
		if (iright < 0 || right == aNULL) {
			iright += szchar;
			if (iright < 0)
				return 0;
		}
	
		if (iright >= szchar)
			iright = sz;
		else {
			if (pivot != -1 && pivot < iright) {
				iright = convertchartoposutf16(svalue, pivot, iright);
				if (iright <= ileft)
					return 0;
			}
		}

		return 2;
	}

	long shift;
	//this is a shift
	if (right == aNULL)
		iright = sz;
	else {
		shift = right->Integer();
		if (reright)
			right->Release();

		if (iright > pivot)
			iright = convertpostocharutf16(svalue, pivot, iright) + shift;
		else
			iright += shift;

		if (iright > pivot)
			//This is position compared to the end of the string
			//First we need to transform shift in bytes
			iright = convertchartoposutf16(svalue, pivot, iright);
	}

	if (iright >= szchar)
        iright = sz;
    else
        if (iright <= ileft)
            return 0;
    return 2;
}
#else

long char_to_pos_emoji(wstring& w, long& ileft, long emoji) {
    //emoji is the position of the first emoji in the string
    long sz = w.size();
    while (emoji != ileft) {
        if (c_is_emoji(w[emoji])) {
            emoji++;
            while (c_is_emojicomp(w[emoji])) {
                emoji++;
                ileft++;
            }
        }
        else
            emoji++;
    }
    if (emoji < sz && c_is_emoji(w[emoji])) {
        emoji++;
        while (emoji < sz && c_is_emojicomp(w[emoji])) {
            emoji++;
        }
    }
    else
        emoji++;
    return emoji;
}

Exporting char StringIndexes(wstring& svalue, Tamgu* index, long& ileft, long& iright, short idthread) {

	Tamgu* left = index;
	Tamgu* right = NULL;
    long emoji = -1;
	long sz = size_c(svalue, emoji);

	bool sleft = false;
	bool sright = false;
	bool releft = false;
	bool reright = false;

	iright = -1;

	if (index->isIndex()) {
		TamguIndex* kind = (TamguIndex*)index;
		sleft = kind->signleft;
		sright = kind->signright;
		left = kind->left->Eval(aNULL, aNULL, idthread);
		if (left != kind->left)
			releft = true;

		if (kind->interval == true) {
			right = kind->right->Eval(aNULL, aNULL, idthread);
			if (right != kind->right)
				reright = true;
		}
	}


	if (left->isRegular()) {
		//this is a regular expression...
		if (sleft) {
			//we need the last one...
			if (!left->searchlast(svalue, ileft, iright))
				return 0;
		}
		else {
			if (!left->search(svalue, ileft, iright))
				return 0;
		}
	}
	else {
		if (left->isString()) {
			wstring sub = left->UString();
			if (releft)
				left->Release();

			//then we are looking for a substring
			if (sleft)
				ileft = s_rfind(svalue, sub, sz);
			else
				ileft = s_find(svalue, sub, 0);

			if (ileft == -1)
				return 0;

			iright = ileft + sub.size();
		}
		else {
			ileft = left->Integer();
			if (releft)
				left->Release();
			if (ileft < 0)
				ileft += sz;
			if (ileft >= sz)
				return 0;
			if (ileft < 0)
				ileft = 0;
		}

	}
    
	//We return as a non interval
	if (right == NULL) {
		if (iright != -1)
			return 2;
        if (emoji != -1 && ileft >= emoji) {
            iright = char_to_pos_emoji(svalue, ileft, emoji);
            return 2;
        }
		return 1;
	}

	if (right->isRegular()) {
		//this is a regular expression...
		long r = iright;

		if (sright) {
			//we need the last one...
			if (!right->searchlast(svalue, r, iright, r))
				return 0;
		}
		else {
			if (!right->search(svalue, r, iright, r))
				return 0;
		}
		if (iright <= ileft)
			return 0;
		return 2;
	}

	if (right->isString()) {
		wstring wr = right->UString();
		if (reright)
			right->Release();

		if (sright) {
			iright = s_rfind(svalue, wr, sz);
			if (iright <= ileft)
				return 0;
		}
		else {
			if (iright != -1)
				iright = s_find(svalue, wr, iright);
			else
				iright = s_find(svalue, wr, ileft);
		}

		if (iright == -1)
			return 0;

		iright += wr.size();
		return 2;
	}

	if (iright == -1) {//absolute position
		iright = right->Integer();
		if (reright)
			right->Release();

		if (iright < 0 || right == aNULL)
			iright += sz;
	}
	else {
		//this is a shift
		if (right == aNULL)
			iright = sz;
		else
			iright += right->Integer();
	}

    if (emoji != -1) {
        if (ileft >= emoji)
            char_to_pos_emoji(svalue, ileft, emoji);

        if (iright > sz)
            iright = sz;

        if (iright >= emoji)
            char_to_pos_emoji(svalue, iright, emoji);

        if (iright <= ileft)
            return 0;
        return 2;
    }
    
	if (iright > sz)
		iright = sz;
	else
		if (iright <= ileft)
			return 0;
	return 2;
}
#endif
//---------------------------------------------------------------------------
long char_to_byteposition(unsigned char* contenu, long sz, long i, long charpos) {
    charpos-=i;
    long nb;

    while (charpos > 0 && i < sz) {
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

long char_to_byteposition(unsigned char* contenu, long sz, long i, long charpos, long& emoji) {
    emoji = -1;
    charpos-=i;
    long nb;
    
    while (charpos > 0 && i < sz) {
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
    
    if (c_test_utf8(contenu+i) == 3) {
        long ii = i;
        if (c_is_emoji(contenu,ii)) {
            ii++;
            while (c_is_emojicomp(contenu, ii)) {
                ii++;
                emoji = ii;
            }
        }
    }
    return i;
}


Exporting char StringIndexes(string& svalue, Tamgu* index, long& ileft, long& iright, short idthread) {
    
    
    Tamgu* left = index;
    Tamgu* right = NULL;
    
    
    bool sleft = false;
    bool sright = false;
    bool releft = false;
    bool reright = false;
    
    iright = -1;
    
    if (index->isIndex()) {
        TamguIndex* kind = (TamguIndex*)index;
        sleft = kind->signleft;
        sright = kind->signright;
        left = kind->left->Eval(aNULL, aNULL, idthread);
        if (left != kind->left)
            releft = true;
        
        if (kind->interval == true) {
            right = kind->right->Eval(aNULL, aNULL, idthread);
            if (right != kind->right)
                reright = true;
        }
    }
    
    
    long sz = -1, szc = -1, pivot = -1, emoji = -1;
    char utf8 = 0;
    
    if (left->isRegular()) {
        //this is a regular expression...
        if (sleft) {
            //we need the last one...
            if (!left->searchlast(svalue, ileft, iright))
                return 0;
        }
        else {
            if (!left->search(svalue, ileft, iright))
                return 0;
        }
    }
    else {
        sz = svalue.size();
        if (left->isString()) {
            string sub = left->String();
            if (releft)
                left->Release();
            
            //then we are looking for a substring
            if (sleft)
                ileft = s_rfindbyte(svalue, sub, sz);
            else
                ileft = s_findbyte(svalue, sub, 0);
            
            if (ileft == -1)
                return 0;
            
            iright = ileft + sub.size();
        }
        else {
            ileft = left->Integer();
            if (releft)
                left->Release();
            
            //This is a tricky case... ileft is in character position not bytes
            szc = size_c(USTR(svalue), sz, pivot);
            if (pivot == -1)
                utf8 = 1;
            else
                utf8 = 2; //to mark that we have computed it...

            if (ileft <0) {
                //This a position from the end...
                //We need to compute the size in characters to the size in bytes...
                if (utf8 == 2) {
                    ileft += szc;
                    if (ileft < 0)
                        ileft = 0;
                    else {
                        if (ileft >= pivot) // the pivot is the first UTF8 element slot in characters
                            ileft = char_to_byteposition(USTR(svalue), sz, pivot, ileft, emoji);
                    }
                }
                else {
                    ileft += sz;
                    if (ileft < 0)
                        ileft = 0;
                }
            }
            else {
                if (ileft > szc)
                    ileft = sz;
                else {
                    if (utf8 == 2 && ileft >= pivot)
                        ileft = char_to_byteposition(USTR(svalue), sz, pivot, ileft, emoji);
                }
            }
        }
    }
    
    
    //We return as a non interval
    if (right == NULL) {
        if (iright == -1) {
            if (ileft < emoji) {
                iright = emoji;
                return 2;
            }
            return 1;
        }
        return 2;
    }
    
    if (right->isRegular()) {
        //this is a regular expression...
        long r = iright;
        
        if (sright) {
            //we need the last one...
            if (!right->searchlast(svalue, r, iright, r))
                return 0;
        }
        else {
            if (!right->search(svalue, r, iright, ileft))
                return 0;
        }
        if (iright <= ileft)
            return 0;
        return 2;
    }
    
    if (sz == -1)
        sz = svalue.size();
    
    if (right->isString()) {
        string sub = right->String();
        if (reright)
            right->Release();
        
        if (sright) {
            iright = s_rfindbyte(svalue, sub, sz);
            if (iright <= ileft)
                return 0;
        }
        else {
            if (iright == -1)
                iright = s_findbyte(svalue, sub, ileft);
            else
                iright = s_findbyte(svalue, sub, iright);
        }
        if (iright == -1)
            return 0;
        
        iright += sub.size();
        return 2;
    }
    
    if (!utf8) { //we need to compute them
        szc = size_c(USTR(svalue), sz, pivot);
        if (pivot == -1)
            utf8 = 1;
        else
            utf8 = 2;
    }
    
    //Now we have two cases...
    //First, this is a pure integer interval, in that case, iright is the absolute position
    if (iright == -1) {
        iright = right->Integer();
        if (reright)
            right->Release();
        
        if (iright < 0 || right == aNULL) {
            if (utf8 == 2) {
                //This is position compared to the end of the string
                iright += szc;
                if (iright <= 0)
                    return 0;
                if (iright > pivot) {
                    iright = char_to_byteposition(USTR(svalue), sz, pivot, iright);
                }
            }
            else
                iright += sz;
        }
        else {
            if (iright > szc)
                iright = sz;
            else {
                if (utf8 == 2 && iright > pivot) {
                    iright = char_to_byteposition(USTR(svalue), sz, pivot, iright);
                }
            }
        }
        
        if (iright <= ileft)
            return 0;
        return 2;
    }
    
    long shift = right->Integer();
    if (reright)
        right->Release();
    if (shift < 0)
        return 0;
    
    if (right == aNULL)
        iright = sz;
    else {
        if (utf8 == 2) {
            if (iright > pivot)
                iright = c_bytetocharposition(USTR(svalue), iright)+shift;
            else
                iright += shift;
            
            if (iright > pivot) {
                //This is position compared to the end of the string
                //First we need to transform shift in bytes
                iright = char_to_byteposition(USTR(svalue), sz, pivot, iright);
            }
        }
        else
            iright += shift;
    }
    
    if (iright > sz)
        iright = sz;
    else
        if (iright <= ileft)
            return 0;
    return 2;
}

//---------------------------------------------------------

unsigned long StringEditDistance(wstring& value, wstring& s2) {
    unsigned long s1len, s2len, x, y, lastdiag, olddiag;
    s1len = value.size();
    s2len = s2.size();
    size_t* column = new size_t[s1len + 1];
    for (y = 1; y <= s1len; y++)
        column[y] = y;
    for (x = 1; x <= s2len; x++) {
        column[0] = x;
        for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
            olddiag = column[y];
            column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + (value[y - 1] == s2[x - 1] ? 0 : 1));
            lastdiag = olddiag;
        }
    }
    s2len = column[s1len];
    delete[] column;
    return s2len;
}


unsigned long StringEditDistance(string& value, string& s2) {
    unsigned long s1len, s2len, x, y, lastdiag, olddiag;
    s1len = value.size();
    s2len = s2.size();
    size_t* column = new size_t[s1len + 1];
    for (y = 1; y <= s1len; y++)
        column[y] = y;
    for (x = 1; x <= s2len; x++) {
        column[0] = x;
        for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
            olddiag = column[y];
            column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + (value[y - 1] == s2[x - 1] ? 0 : 1));
            lastdiag = olddiag;
        }
    }
    s2len = column[s1len];
    delete[] column;
    return s2len;
}

string StringXor(string value, string s) {
    string u;
    
    long m = min(s.size(), value.size());
    for (long i = 0; i < m; i++) {
        if (s[i] != value[i])
            u += value[i];
    }
    
    return u;

}

wstring StringXor(wstring value, wstring s) {
    wstring u;
    
    long m = min(s.size(), value.size());
    for (long i = 0; i < m; i++) {
        if (s[i] != value[i])
            u += value[i];
    }
    
    return u;
    
}

string StringAnd(string value, string s) {
    string u;
    
    long m = min(s.size(), value.size());
    for (long i = 0; i < m; i++) {
        if (s[i] == value[i])
            u += s[i];
    }
    return u;
}

wstring StringAnd(wstring value, wstring s) {
    wstring u;
    
    long m = min(s.size(), value.size());
    for (long i = 0; i < m; i++) {
        if (s[i] == value[i])
            u += s[i];
    }
    return u;
}

string StringOr(string value, string s) {
    return value + s;
}

wstring StringOr(wstring value, wstring s) {
    return value + s;
}

string StringMinus(string v, string sub) {
    size_t pos = v.find(sub);
    if (pos == string::npos)
        return v;
    
    size_t nb = sub.size();
    //we keep our string up to p
    sub = v.substr(0, pos);
    //then we skip the nb characters matching the size of v
    pos += nb;
    //then we concatenate with the rest of the string
    sub += v.substr(pos, v.size() - pos);
    return sub;
}

wstring StringMinus(wstring v, wstring sub) {
    size_t pos = v.find(sub);
    if (pos == wstring::npos)
        return v;
    
    size_t nb = sub.size();
    //we keep our wstring up to p
    sub = v.substr(0, pos);
    //then we skip the nb characters matching the size of v
    pos += nb;
    //then we concatenate with the rest of the wstring
    sub += v.substr(pos, v.size() - pos);
    return sub;
}
//---------------------------------------------------------

double DoubleAnd(double value, double l) {
    double64 d(value);
    double64 dl(value);
    d.bits &= dl.bits;
    return d.v;
}

double DoubleOr(double value, double l) {
    double64 d(value);
    double64 dl(value);
    d.bits |= dl.bits;
    return d.v;
}

double DoubleXor(double value, double l) {
    double64 d(value);
    double64 dl(value);
    d.bits ^= dl.bits;
    return d.v;
}

double DoubleShiftleft(double value, long l) {
    double64 d(value);
    d.bits <<= l;
    return d.v;
}

double DoubleShiftright(double value, long l) {
    double64 d(value);
    d.bits >>= l;
    return d.v;
}

float FloatAnd(float value, float l) {
    float32 d(value);
    float32 dl(value);
    d.bits &= dl.bits;
    return d.v;
}

float FloatOr(float value, float l) {
    float32 d(value);
    float32 dl(value);
    d.bits |= dl.bits;
    return d.v;
}

float FloatXor(float value, float l) {
    float32 d(value);
    float32 dl(value);
    d.bits ^= dl.bits;
    return d.v;
}

float FloatShiftleft(float value, long l) {
    float32 d(value);
    d.bits <<= l;
    return d.v;
}

float FloatShiftright(float value, long l) {
    float32 d(value);
    d.bits >>= l;
    return d.v;
}


//---------------------------------------------------------
void convertbytepositiontochar(vector<long>& v, agnostring& s) {
    vector<long> vres;
    long i = 0;
    long r = 0;
    for (long j = 0; j < v.size(); j++) {
        c_bytetocharpositionidx(USTR(s), v[j], r, i);
        vres.push_back(r);
    }
    v = vres;
}

void convertbytepositiontochar(vector<double>& v, agnostring& s) {
    vector<double> vres;
    long i = 0;
    long r = 0;
    for (long j = 0; j < v.size(); j++) {
        c_bytetocharpositionidx(USTR(s), v[j], r, i);
        vres.push_back(r);
    }
    v = vres;
}

//---------------------------------------------------------
void XNBrowse(x_node* xn, Tamgu* kf) {
    if (xn == NULL)
        return;
    Tamguprimemap* kmap = new Tamguprimemap;
    kf->Push(kmap);
    
    if (xn->nodes.size()) {
        string key = xn->token;
        if (xn->value != "")
            key += "_" + xn->value;
        Tamguvector* kvect = globalTamgu->Providevector();
        kmap->Push(key, kvect);
        for (size_t i = 0; i < xn->nodes.size(); i++) {
            XNBrowse(xn->nodes[i], kvect);
        }
    }
    else
        kmap->Push(xn->token, globalTamgu->Providestring(xn->value));
}

//---------------------------------------------------------
Exporting Tamgu* Selectaprimemapss(Tamgu* context) {
    if (context->Type()==Tamguprimemapss::idtype && context->isAffectation()) {
        context->Clear();
        return context;
    }
    
    return new Tamguprimemapss;
}

Exporting Tamgu* SelectContainer(Tamgu* context, short idthread) {
    if (context->isContainer()) {
        if(context->isAffectation()) {
            context->Clear();
            return context;
        }
        
        return context->Newinstance(idthread);
    }
    
    return NULL;
}


Exporting Tamgu* Selectacontainer(Tamgu* context, short idthread) {
    if (context->isContainer()) {
        if (context->isAffectation()) {
            context->Clear();
            return context;
        }
        return context->Newinstance(idthread);
    }
    
    return globalTamgu->Providevector();
}

Exporting Tamgu* Selectamap(Tamgu* context) {
    if (context->isMapContainer() && context->isAffectation()) {
        context->Clear();
        return context;
    }
    
    return globalTamgu->Providemap();
}


Exporting Tamgu* Selectavector(Tamgu* context) {
    if (context->isVectorContainer() && context->isAffectation()) {
        context->Clear();
        return context;
    }
    
    return globalTamgu->Providevector();
}

Exporting Tamgu* SelectaVector(Tamgu* context) {
    if (context->isVectorContainer() && context->isAffectation() && context->Type() == a_vector) {
        context->Clear();
        return context;
    }
    
    return globalTamgu->Providevector();
}


Exporting Tamgu* Selectasvector(Tamgu* context) {
    if (context->isVectorContainer() && context->isAffectation() && context->Type() == a_svector) {
        context->Clear();
        return context;
    }
    
    return globalTamgu->Providesvector();
}

Exporting Tamgu* Selectauvector(Tamgu* context) {
    if (context->isVectorContainer() && context->isAffectation() && context->Type() == a_uvector) {
        context->Clear();
        return context;
    }
    
    return globalTamgu->Provideuvector();
}

Exporting Tamgu* Selectaivector(Tamgu* context) {
    if (context->isVectorContainer() && context->isAffectation() && context->Type() == a_ivector) {
        context->Clear();
        return context;
    }
    
    return globalTamgu->Provideivector();
}

Exporting Tamgu* Selectahvector(Tamgu* context) {
    if (context->isVectorContainer() && context->isAffectation() && context->Type() == a_hvector) {
        context->Clear();
        return context;
    }
    
    return new Tamguhvector;
}

Exporting Tamgu* Selectalvector(Tamgu* context) {
    if (context->isVectorContainer() && context->isAffectation() && context->Type() == a_lvector) {
        context->Clear();
        return context;
    }
    
    return new Tamgulvector;
}

Exporting Tamgu* Selectadvector(Tamgu* context) {
    if (context->isVectorContainer() && context->isAffectation() && context->Type() == a_dvector) {
        context->Clear();
        return context;
    }
    
    return new Tamgudvector;
}

Exporting Tamgu* Selectafvector(Tamgu* context) {
    if (context->isVectorContainer() && context->isAffectation() && context->Type() == a_fvector) {
        context->Clear();
        return context;
    }
    
    return globalTamgu->Providefvector();
}

Exporting Tamgu* Selectabvector(Tamgu* context) {
    if (context->isVectorContainer() && context->isAffectation() && context->Type() == a_bvector) {
        context->Clear();
        return context;
    }
    
    return new Tamgubvector;
}
//--------------------------------------------------------------------
Exporting Tamgu* TamguGlobal::EvaluateLisp(string& s, string& o, string& c, bool comma, bool separator, bool keeprc, vector<string>& rules, short idthread) {
    x_tokenize xr;
    bnf_tamgu bnf;
    
    if (rules.size()==0)
        xr.load();
    else {
        xr.resetrules(rules);
        xr.rules = rules;
        string mess;
        if (!xr.parseexternalrules(mess))
            return globalTamgu->Returnerror(mess, idthread);
        xr.loaded=true;
    }

    if (comma)
        xr.selectcomma(true);
    else
        xr.selectcomma(false);
    
    if (keeprc)
        xr.keeprc(true);
    else
        xr.keeprc(false);
    
    if (separator)
        xr.separator(true);
    else
        xr.separator(false);

    xr.tokenize(s);
    
    bnf.baseline = linereference;
    
    bnf.initialize(&xr);
    
    bnf_tamgu* previous = currentbnf;
    currentbnf = &bnf;
    
    string lret;
    x_node* xn = new x_node;
    
    bnf.Y_var_0 = o[0];
    bnf.Y_var_1 = c[0];
    
    if (bnf.m_lisp(lret, &xn) != 1)
        return Returnerror("Unknown expression", idthread);
    
    Tamgu* kret = globalTamgu->Providevector();
    TamguCode* code = spaces[0];
    code->compilemode = false;
    
    try {
        code->Traverse(xn, kret);
    }
    catch (TamguRaiseError* m) {
        code->global = this;
        kret->Release();
        kret = Returnerror(m->message, idthread);
        delete m;
    }
    
    currentbnf = previous;
    delete xn;
    return kret;
}

Exporting Tamgu* TamguGlobal::EvaluateTags(string& s, string& o, string& c, bool comma, bool separator, bool keeprc, vector<string>& rules, short idthread){
    x_tokenize xr;
    bnf_tamgu bnf;
    
    if (rules.size()==0)
        xr.load();
    else {
        xr.resetrules(rules);
        xr.rules = rules;
        string mess;
        if (!xr.parseexternalrules(mess))
            return globalTamgu->Returnerror(mess, idthread);
        xr.loaded=true;
    }

    if (comma)
        xr.selectcomma(true);
    else
        xr.selectcomma(false);
    
    if (keeprc)
        xr.keeprc(true);
    else
        xr.keeprc(false);
    
    if (separator)
        xr.separator(true);
    else
        xr.separator(false);

    xr.tokenize(s);
    
    bnf.baseline = linereference;
    bnf.initialize(&xr);
    
    bnf_tamgu* previous = currentbnf;
    currentbnf = &bnf;
    
    string lret;
    x_node* xn = new x_node;
    
    
    bnf.VS_var_2 = o;
    bnf.VS_var_3 = c;
    
    if (bnf.m_tag(lret, &xn) != 1)
        return Returnerror("Unknown expression", idthread);
    
    Tamgu* kret = globalTamgu->Providevector();
    
    TamguCode* code = spaces[0];
    code->compilemode = false;
    
    try {
        code->Traverse(xn, kret);
    }
    catch (TamguRaiseError* m) {
        code->global = this;
        kret->Release();
        kret = Returnerror(m->message, idthread);
        delete m;
    }
    
    currentbnf = previous;
    delete xn;
    return kret;
}

Exporting An_rules* TamguGlobal::EvaluateRules(string& body, short idthread) {
    x_reading xr;
    bnf_tamgu bnf;
    
    bnf.baseline = linereference;
    bnf.initialize(&xr);
    
    bnf_tamgu* previous = currentbnf;
    
    xr.tokenize(body);
    if (xr.size() == 0) {
        stringstream message;
        message << "Empry string" << endl;
        globalTamgu->Returnerror(message.str(), idthread);
        return NULL;
    }
    
    lineerror = -1;
    
    x_node* xn = bnf.x_parsing(&xr, FULL);
    
    currentbnf = &bnf;
    string lret;
    
    TamguCode* code = spaces[0];
    
    if (xn == NULL) {
        stringstream& message = globalTamgu->threads[0].message;
        globalTamgu->lineerror = bnf.lineerror;
        code->currentline = globalTamgu->lineerror;
        message << "Error while parsing string: ";
        if (bnf.errornumber != -1)
            message << bnf.x_errormsg(bnf.errornumber);
        else
            message << bnf.labelerror;
        
        globalTamgu->Returnerror(message.str(), idthread);
        return NULL;
    }
    
    Tamgu* kret = aNULL;
    code->compilemode = false;
    
    try {
        kret = code->Traverse(xn, aNULL);
    }
    catch (TamguRaiseError* m) {
        code->global = this;
        kret->Release();
        kret = Returnerror(m->message, idthread);
        delete m;
        currentbnf = previous;
        delete xn;
        return NULL;
    }
    
    currentbnf = previous;
    delete xn;
    
    return globalTamgu->gTheAnnotationRules;
}

Exporting Tamgu* TamguGlobal::EvaluateVector(string& s, short idthread) {
    x_reading xr;
    bnf_tamgu bnf;
    
    xr.tokenize(s);
    
    bnf.baseline = linereference;
    bnf.initialize(&xr);
    
    bnf_tamgu* previous = currentbnf;
    currentbnf = &bnf;
    string lret;
    x_node* xn = new x_node;
    
    
    if (bnf.m_jvector(lret, &xn) != 1)
        return Returnerror("Unknown expression", idthread);
    
    Tamgu* kret = aNULL;
    TamguCode* code = spaces[0];
    code->compilemode = false;
    
    try {
        kret = code->Traverse(xn, NULL);
    }
    catch (TamguRaiseError* m) {
        code->global = this;
        kret->Release();
        kret = Returnerror(m->message, idthread);
        delete m;
    }
    
    currentbnf = previous;
    delete xn;
    return kret;
}

Exporting Tamgu* TamguGlobal::EvaluateMap(string& s, short idthread) {
    x_reading xr;
    bnf_tamgu bnf;
    
    xr.tokenize(s);
    
    bnf.baseline = linereference;
    bnf.initialize(&xr);
    
    bnf_tamgu* previous = currentbnf;
    currentbnf = &bnf;
    string lret;
    x_node* xn = new x_node;
    
    
    if (bnf.m_jmap(lret, &xn) != 1)
        return Returnerror("Unknown expression", idthread);
    
    Tamgu* kret = aNULL;
    TamguCode* code = spaces[0];
    code->compilemode = false;
    
    try {
        kret = code->Traverse(xn, NULL);
    }
    catch (TamguRaiseError* m) {
        code->global = this;
        kret->Release();
        kret = Returnerror(m->message, idthread);
        delete m;
    }
    
    delete xn;
    currentbnf = previous;
    return kret;
}

Exporting Tamgu* TamguGlobal::EvaluateJSON(string& s, short idthread) {
    x_reading xr;
    bnf_tamgu bnf;
    
    xr.tokenize(s);
    
    bnf.baseline = linereference;
    bnf.initialize(&xr);
    
    bnf_tamgu* previous = currentbnf;
    currentbnf = &bnf;
    string lret;
    x_node* xn = new x_node;
    
    if (bnf.m_jexpression(lret, &xn) != 1)
        return Returnerror("Unknown expression", idthread);
    
    Tamgu* kret = aNULL;
    TamguCode* code = spaces[0];
    code->compilemode = false;
    
    try {
        kret = code->Traverse(xn, NULL);
    }
    catch (TamguRaiseError* m) {
        code->global = this;
        kret->Release();
        kret = Returnerror(m->message, idthread);
        delete m;
    }
    
    currentbnf = previous;
    delete xn;
    return kret;
}

//--------------------------------------------------------------------
bool WaitingFor(double tm, PauseBack pb, void* data) {
    double init;
    double res;
    timeval tempsfinal;
    
    gettimeofday(&tempsfinal, NULL);
    init = (((unsigned long)tempsfinal.tv_sec) * 1000 + ((unsigned long)tempsfinal.tv_usec) / 1000.0);
    res = tm * 1000;
    
    double diff = 0;
    while (diff < res) {
        if (pb(data) == true)
            return true;
        gettimeofday(&tempsfinal, NULL);
        diff = timeminus(init, tempsfinal);
    }
    return false;
}

void Wait(double tm) {
    double init;
    double res;
    timeval tempsfinal;
    
    gettimeofday(&tempsfinal, NULL);
    init = (((unsigned long)tempsfinal.tv_sec) * 1000 + ((unsigned long)tempsfinal.tv_usec) / 1000.0);
    res = tm * 1000;
    
    double diff = 0;
    while (diff < res) {
        gettimeofday(&tempsfinal, NULL);
        diff = timeminus(init, tempsfinal);
    }
}

#ifdef WIN32
bool DirectoryCreation(char* noms, long pos) {
    char* pt = strchr(noms + pos, SEP);
    while (pt != NULL) {
        *pt = 0;
        CreateDirectoryA(noms, NULL);
        *pt = SEP;
        pt = strchr(pt + 1, SEP);
    }
    
    bool created = true;
    if (CreateDirectoryA(noms, NULL) == FALSE)
        created = false;
    
    return created;
}
#else
bool DirectoryCreation(char* noms, long pos) {
    char* pt = strchr(noms + pos, SEP);
    while (pt != NULL) {
        *pt = 0;
        mkdir(noms, S_IRWXU | S_IRWXG | S_IRWXO);
        *pt = SEP;
        pt = strchr(pt + 1, SEP);
    }
    
    bool created = true;
    if (mkdir(noms, S_IRWXU | S_IRWXG | S_IRWXO) != 0)
        created = false;
    return created;
}
#endif

#ifdef APPLE
extern "C" {
    int    gethostuuid(uuid_t id, const struct timespec *wait) {
        return -1;
    }
}
#endif
//-----------------------------------------------------------------------
static FILE* localstreamerr = NULL;
static FILE* localstream = NULL;

void clear_output() {
    if (localstream != NULL) {
        fclose(localstream);
        localstream = NULL;
    }
    
    if (localstreamerr != NULL) {
        fclose(localstreamerr);
        localstreamerr = NULL;
    }
}

char redirect_output(string& filename, long output) {
    if (output == 1) {
        if (localstream != NULL)
            return -2;
        
        int o = DUP(output);
        
        localstream = fopen(STR(filename), "w");
        if (localstream == NULL)
            return -1;
        DUP2(FILENO(localstream), output);
        return o;
    }
    
    if (localstreamerr != NULL)
        return -2;
    
    int o = DUP(output);
    
    localstreamerr = fopen(STR(filename), "w");
    if (localstreamerr == NULL)
        return -1;
    DUP2(FILENO(localstreamerr), output);
    return o;
    
}

char restate_output(long o, long output) {
    if (output == 1) {
        if (localstream == NULL)
            return -1;
        
        fflush(stdout);
        fclose(localstream);
        localstream = NULL;
        DUP2(o, output);
        return 0;
    }
    
    if (localstreamerr == NULL)
        return -1;
    
    fflush(stdout);
    fclose(localstreamerr);
    localstreamerr = NULL;
    DUP2(o, output);
    return 0;
}
//-----------------------------------------------------------------------------
void Flattening(Tamgu* ret, Tamgu* res) {
    TamguIteration* itr = res->Newiteration(false);
    Tamgu* kval;
    for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
        kval = itr->IteratorValue();
        if (kval->isContainer())
            Flattening(ret, kval);
        else
            ret->Push(kval);
    }
    itr->Release();
}
//-----------------------------------------------------------------------------
#ifdef WIN32
char* Getenv(char* name) {
    static char rep[4096];
    size_t size = 0;
    getenv_s(&size, rep, 4096, name);
    return rep;
}
#else
#define Getenv getenv
#endif

void FileNameNomalization(char* fileName, char* buffer, long buffersz) {
    //Tout d'abord on ramene tous les chemins a de l'UNIX
    long l = strlen(buffer);
    //On normalise les chemins
    for (int i = 0; i < l; i++) {
        if (buffer[i] == ANTISEP)
            buffer[i] = SEP;
    }
    
    //s'il y a une variable d'environnement
    char* vari = strchr(buffer, '$');
    fileName[0] = 0;
    while (vari) {
        char* reper = Getenv(vari + 1);
        char* pt = strchr(vari + 1, SEP);
        
        if (pt != NULL)
            *pt = 0;
        
        //On recopie la partie qui precede la variable
        int lvar = vari - buffer;
        int lnom = strlen(fileName);
        memcpy(fileName + lnom, buffer, lvar);
        fileName[lvar + lnom] = 0;
        
        if (reper != NULL)
            strcat_s(fileName, buffersz, reper);
        
        if (pt != NULL) {
            *pt = SEP;
            static char inter[1000];
            strcpy_s(inter,1000, pt);
            strcpy_s(buffer, buffersz, inter);
        }
        else
            buffer[0] = 0;
        vari = strchr(buffer, '$');
    }
    
    strcat_s(fileName, buffersz, buffer);
    char localpath[4096];
#ifdef WIN32
    _fullpath(localpath, fileName, 4096);
#else
    realpath(fileName, localpath);
#endif
    
    strcpy_s(fileName, buffersz, localpath);
}

string NormalizeFileName(string n) {
    if (n == "")
        return "";
    
    char buff[4096];
    char name[4096];
    strcpy_s(name, 4096, STR(n));
    
    FileNameNomalization(buff, name, 4096);
    return buff;
}

Exporting bool TamguGlobal::RecordExternalLibrary(string name, TamguExternalModule module) {
    if (externalLibraries.find(name) != externalLibraries.end())
        return false;
    
    if (module == NULL)
        return true;
    
    externalLibraries[name] = module;
    return true;
}

//------------------------------------------------------------
uchar Selecttype(Tamgu* context) {
    if (context->isMapContainer())
        return a_map;
    
    switch (context->Typevariable()) {
        case a_ivector:
            return a_ivector;
        case a_fvector:
            return a_fvector;
        case a_svector:
            return a_svector;
        case a_uvector:
            return a_uvector;
        case a_vector:
            return a_vector;
        case a_string:
            return a_string;
        case a_ustring:
            return a_ustring;
        case a_dvector:
            return a_dvector;
        case a_lvector:
            return a_lvector;
        case a_counter:
            return a_counter;
        default:
            if (context->isVectorContainer())
                return a_vector;
    }
    return 0;
}

Tamgu* Storealongtype(Tamgu* context, Tamgu* a, short idthread, uchar& addvalue) {
    switch (addvalue) {
        case a_vector:
            context->Push(a);
            return context;
        case a_hvector:
            ((Tamguhvector*)context)->values.push_back(a->Short());
            return context;
        case a_ivector:
            ((Tamguivector*)context)->values.push_back(a->Integer());
            return context;
        case a_fvector:
            ((Tamgufvector*)context)->values.push_back(a->Float());
            return context;
        case a_svector:
            ((Tamgusvector*)context)->values.push_back(a->String());
            return context;
        case a_uvector:
            ((Tamguuvector*)context)->values.push_back(a->UString());
            return context;
        case a_dvector:
            ((Tamgudvector*)context)->values.push_back(a->Decimal());
            return context;
        case a_lvector:
            ((Tamgulvector*)context)->values.push_back(a->Long());
            return context;
        case a_string:
            ((Tamgustring*)context)->value += a->String();
            return context;
        case a_ustring:
            ((Tamguustring*)context)->value += a->UString();
            return context;
        case a_map:
            context->Pushinmap(a, idthread);
            return context;
        case a_counter:
            ((TamguTaskellCounter*)context)->value++;
            return context;
    }
    
    switch (a->Typevariable()) {
        case a_short:
            context = new Tamguhvector;
            ((Tamguhvector*)context)->values.push_back(a->Short());
            addvalue = a_hvector;
            return context;
        case a_int:
            context = globalTamgu->Provideivector();
            ((Tamguivector*)context)->values.push_back(a->Integer());
            addvalue = a_ivector;
            return context;
        case a_long:
            context = new Tamgulvector;
            ((Tamgulvector*)context)->values.push_back(a->Long());
            addvalue = a_lvector;
            return context;
        case a_float:
            context = globalTamgu->Providefvector();
            ((Tamgufvector*)context)->values.push_back(a->Float());
            addvalue = a_fvector;
            return context;
        case a_string:
            context = globalTamgu->Providesvector();
            ((Tamgusvector*)context)->values.push_back(a->String());
            addvalue = a_svector;
            return context;
        case a_ustring:
            context = globalTamgu->Provideuvector();
            ((Tamguuvector*)context)->values.push_back(a->UString());
            addvalue = a_uvector;
            return context;
        case a_decimal:
            context = new Tamgudvector;
            ((Tamgudvector*)context)->values.push_back(a->Decimal());
            addvalue = a_dvector;
            return context;
        default:
            if (a->isMapContainer()) {
                context = globalTamgu->Providemap();
                context->Pushinmap(a, idthread);
                addvalue = a_map;
            }
            else {
                context = globalTamgu->Providevector();
                context->Push(a);
                addvalue = a_vector;
            }
    }
    return context;
}
//------------------------------------------------------------

#ifdef WIN32
bool TamguCode::Loadlibrary(string n, string& library_name) {
    if (n == "")
        return false;
    
    string name = NormalizeFileName(n);
    HINSTANCE LoadMe;
    string lname = name;
    int pos = lname.find_last_of(SEP);
    string subname = lname;
    if (pos != -1) {
        subname = lname.substr(pos + 1, lname.size() - pos - 1);
        pos = subname.find(".");
        if (pos != -1)
            subname = subname.substr(0, pos);
    }
    
    string moduleinitname = subname;
    moduleinitname += "_InitialisationModule";
    
    //If it has already been loaded, we return...
    library_name = lname;
    TamguExternalModule LibEntryPoint;
    
    if (globalTamgu->RecordExternalLibrary(moduleinitname, NULL) == false) {
        LibEntryPoint = globalTamgu->externalLibraries[moduleinitname];
        (*LibEntryPoint)(globalTamgu, TamguVersion());
        return true;
    }
    
    LoadMe = LoadLibraryA(STR(lname));
    if (LoadMe == 0) {
        string atanlib;
        if (Getenv("TAMGULIBS") != NULL)
            atanlib = Getenv("TAMGULIBS");
        else {
            stringstream message;
            message << "Please set TAMGULIBS: " << name;
            throw new TamguRaiseError(message, filename, current_start, current_end);
        }
        
        if (Getenv("PATH") != NULL) {
            string path = "Path=";
            path += atanlib;
            path += ";";
            path += Getenv("PATH");
            _putenv(STR(path));
        }
        
        atanlib += "\\";
        atanlib += subname;
        atanlib += ".dll";
        atanlib = NormalizeFileName(atanlib);
        library_name = atanlib;
        LoadMe = LoadLibraryA(STR(library_name));
    }
    // Check to see if the library was loaded successfully
    
    if (LoadMe == 0) {
        DWORD err = GetLastError();
        stringstream message;
        message << "Cannot load library: " << name;
        throw new TamguRaiseError(message, filename, current_start, current_end);
    }
    
    LibEntryPoint = (TamguExternalModule)GetProcAddress(LoadMe, STR(moduleinitname));
    
    if (LibEntryPoint == NULL) {
        stringstream message;
        message << "No entry point in this library: " << name;
        throw new TamguRaiseError(message, filename, current_start, current_end);
    }
    
    globalTamgu->RecordExternalLibrary(moduleinitname, LibEntryPoint);
    
    if ((*LibEntryPoint)(globalTamgu, TamguVersion()) == false) {
        stringstream message;
        message << "Wrong version: " << name;
        throw new TamguRaiseError(message, filename, current_start, current_end);
    }
    
    return true;
}
#else
#ifdef TAMGUINEEDCLOSELIBS
vector<void*> TAMGULIBSTOCLOSE;

void TamguCloseLibraries() {
    for (int x = 0; x<TAMGULIBSTOCLOSE.size(); x++)
        dlclose(TAMGULIBSTOCLOSE[x]);
}
#endif

bool TamguCode::Loadlibrary(string n, string& library_name) {
    string name = NormalizeFileName(n);
    void* LoadMe;
    
    char lname[4096];
    strcpy_s(lname, 4096, STR(name));
    char* error;
    char* pt = strrchr(lname, '/');
    char buff[4096];
    bool addso = true;
    string basename;
    if (strstr(lname + strlen(lname) - 3, ".so"))
        addso = false;
    if (pt != NULL) {
        if (memcmp(pt + 1, "lib", 3)) {
            if (addso)
                sprintf(buff, "lib%s.so", pt + 1);
            else
                sprintf(buff, "lib%s", pt + 1);
        }
        else {
            if (addso)
                sprintf(buff, "%s.so", pt + 1);
            else
                strcpy(buff, pt + 1);
        }
        basename = buff;
        strcpy(pt + 1, buff);
    }
    else {
        if (memcmp(lname, "lib", 3)) {
            if (addso)
                sprintf(buff, "lib%s.so", lname);
            else
                sprintf(buff, "lib%s", lname);
        }
        else {
            if (addso)
                sprintf(buff, "%s.so", lname);
            else
                strcpy(buff, lname);
        }
        basename = buff;
        strcpy(lname, buff);
    }
    
    FileNameNomalization(buff, lname, 4096);
    strcpy(lname, buff);
    string subname = basename.substr(3, basename.size() - 6);
    
    string moduleinitname = subname;
    moduleinitname += "_InitialisationModule";
    
    TamguExternalModule LibEntryPoint;

    library_name = lname;
    //If it has already been loaded, we return...
    if (global->RecordExternalLibrary(moduleinitname, NULL) == false) {
        LibEntryPoint = global->externalLibraries[moduleinitname];
        (*LibEntryPoint)(global,TamguVersion());
        return true;
    }

    LoadMe = dlopen(lname, RTLD_LAZY);
    string baselib;
    if (LoadMe == NULL) {
        string atanlib;
        if (Getenv("TAMGULIBS") != NULL)
            atanlib = Getenv("TAMGULIBS");
        else {
            stringstream message;
            message << "Please set TAMGULIBS: " << name;
            throw new TamguRaiseError(message, filename, current_start, current_end);
        }
        
        string ldlibpath;
        if (Getenv("LD_LIBRARY_PATH") != NULL) {
            ldlibpath = Getenv("LD_LIBRARY_PATH");
            ldlibpath = ":" + ldlibpath;
        }
        
        ldlibpath = atanlib + ldlibpath;
        setenv("LD_LIBRARY_PATH", ldlibpath.c_str(), 1);
        
#ifdef APPLE
        ldlibpath="";
        if (Getenv("DYLD_LIBRARY_PATH") != NULL) {
            ldlibpath = Getenv("DYLD_LIBRARY_PATH");
            ldlibpath = ":" + ldlibpath;
        }
        
        ldlibpath = atanlib + ldlibpath;
        setenv("DYLD_LIBRARY_PATH", ldlibpath.c_str(), 1);
#endif
        
        ldlibpath = "";
        if (Getenv("PATH") != NULL) {
            ldlibpath = Getenv("PATH");
            ldlibpath = ":" + ldlibpath;
        }
        ldlibpath = atanlib + ldlibpath;
        setenv("PATH", ldlibpath.c_str(), 1);
        
        if (atanlib.back() != '/')
            atanlib += "/";
        
        baselib = atanlib;
        atanlib += basename;
        atanlib = NormalizeFileName(atanlib);
        library_name = atanlib;
        LoadMe = dlopen(STR(atanlib), RTLD_LAZY);
    }
    
    // Check to see if the library was loaded successfully
    
    if (LoadMe == NULL) {
        //We try without lib in the name
        baselib += n;
        if (addso)
            baselib += ".so";
        LoadMe = dlopen(STR(baselib), RTLD_LAZY);
    }
    
    if (LoadMe == NULL) {
        error = dlerror();
        stringstream message;
        message << error << ": " << lname;
        throw new TamguRaiseError(message, filename, current_start, current_end);
    }
    
#ifdef TAMGUINEEDCLOSELIBS
    TamguLIBSTOCLOSE.push_back(LoadMe);
#endif
    
    
    LibEntryPoint = (TamguExternalModule)dlsym(LoadMe, STR(moduleinitname));
    
    if ((error = dlerror()) != NULL) {
        stringstream message;
        message << error << ": " << name;
        throw new TamguRaiseError(message, filename, current_start, current_end);
    }
    
    global->RecordExternalLibrary(moduleinitname, LibEntryPoint);
    
    if ((*LibEntryPoint)(global,TamguVersion()) == false) {
        stringstream message;
        message << "Wrong version: " << name;
        throw new TamguRaiseError(message, filename, current_start, current_end);
    }
    
    return true;
}
#endif



