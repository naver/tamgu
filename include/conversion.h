/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : conversion.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef conversion_h
#define conversion_h

//On certain platforms, memset is unknown
#ifndef Tamgu_REGEX
#include <string.h>
#endif

#include <ostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "tamgutypes.h"
#include "tamguboost.h"
#include <set>

#include "binmap.h"

using std::stringstream;
using std::string;
using std::map;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::list;
using std::ios;
using std::ostream;
using std::istream;
using std::fstream;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
using std::ostringstream;

union double64 {
public:
    
    uint64_t bits;
    double v;
    
    double64(double d) {
        v = d;
    }
};

union float32 {
public:
    
    uint32_t bits;
    float v;
    
    float32(float f) {
        v = f;
    }
};


//--------------------------------------------------------------------

const short token_comma = 1;
const short token_separator = 2;
const short token_period = 4;
const short token_keepwithdigit = 8;
const short token_keeprc = 16;

using std::wstring;

union bulongchar {
	uint64_t val;
	unsigned char ustr[8];
	char str[8];
	bulongchar() {
		memset(str, '\0', 8);
	}
	void clear() {
		memset(str, '\0', 8);
	}
};


union w_u_char {
    uint32_t c;
    unsigned char cc[4];
    w_u_char() {
        c = 0;
    }
    
    //little endian
    void invert() {
        uchar c = cc[0];
        cc[0] = cc[1];
        cc[1] = c;
        c = cc[2];
        cc[2] = cc[3];
        cc[3] = c;
    }
};


//--------------------------------------------------------------------------------------------------------

//Types for fast search on wstring
#ifdef WIN32
#define doublechar int32_t
#define shiftint 16
#else
#define doublechar int64_t
#define shiftint 32
#endif

class UTF8_Handler;
//--------------------- Main Initialization, to be launched before using any of the following methods...
Exporting void inittableutf8();
Exporting UTF8_Handler* create_utf8_handler();
Exporting void set_utf8_handler(UTF8_Handler* u);
Exporting void clean_utf8_handler();
//--------------------- Code Indention
Exporting void IndentCode(string& codestr, string& codeindente, long blancs, bool lisp = false, bool taskel = true);
Exporting void IndentationCode(string& codestr, string& codeindente, bool lisp, bool taskel = true);
Exporting long IndentationCode(string& codestr, bool lisp);
Exporting long VirtualIndentation(string& codestr);

//--------------------- Carriage Return normalisation
void cr_normalise(string& code);

//--------------------- Trimming strings
Exporting string& Trim(string& chaine);
Exporting string& Trimleft(string& chaine);
Exporting string& Trimright(string& chaine);

Exporting wstring& Trim(wstring& chaine);
Exporting wstring& Trimleft(wstring& chaine);
Exporting wstring& Trimright(wstring& chaine);

//--------------------- Digits conversion
extern "C" {
double conversionfloathexa(const char* s);
}

double conversiontofloathexa(const char* s, int sign, short& l);
double conversiontofloathexa(const wchar_t* s, int sign, short& l);
double conversionfloathexa(const char* s, short& l);
double conversionfloathexa(const wchar_t* s, short& l);

Exporting BLONG conversionintegerhexa(char* number);
Exporting BLONG conversionintegerhexa(wstring& number);
Exporting double convertfloat(char* s);
Exporting double convertfloat(wstring value);
Exporting long convertinteger(wstring value);
Exporting double convertfloat(string value);
Exporting long convertinteger(string value);

Exporting double convertdouble(wstring value);
Exporting long convertlong(wstring value);
Exporting double convertdouble(string value);
Exporting long convertlong(string value);
//===================================================================
void DoubleMetaphone(const string &str, vector<string>& codes);
//===================================================================
Exporting void convertnumber(long v, string& s);
Exporting void convertnumber(double v, string& s);
Exporting void convertnumber(long v, wstring& s);
Exporting void convertnumber(double v, wstring& s);

Exporting string convertfromnumber(BLONG l);
Exporting wstring wconvertfromnumber(BLONG l);

Exporting string convertfromnumber(long l);
Exporting wstring wconvertfromnumber(long l);

Exporting wstring wconvertfromnumber(short l);
Exporting string convertfromnumber(short l);


Exporting string convertfromnumber(double l);
Exporting string convertfromnumber(float l);
Exporting wstring wconvertfromnumber(double l);
Exporting wstring wconvertfromnumber(float l);

//--------------------- Conversions
Exporting bool valid_latin_table(short table);

Exporting void s_EvaluateMetaCharacters(wstring& s);
Exporting void s_EvaluateMetaCharacters(string& s);

Exporting bool s_is_utf8(unsigned char* contenu, long longueur);

Exporting char c_detect_utf8(unsigned char* utf);
Exporting char c_test_utf8(unsigned char* utf);

Exporting string c_unicode_to_utf8(TAMGUCHAR code);

Exporting string s_utf8_to_dos(unsigned char* contenu);
Exporting string s_dos_to_utf8(unsigned char* contenu);
Exporting string s_utf8_to_dos(const char* contenu);
Exporting string s_dos_to_utf8(const char* contenu);
Exporting string s_unicode_to_dos(wstring& contenu);

void s_unicode_to_utf16(wstring& w, wstring& u);
void s_unicode_to_utf16(std::u16string& w, std::u32string& u);
void s_unicode_to_utf16(std::wstring& w, std::u32string& u);
void s_unicode_to_utf8(string& s, std::u32string& str);

Exporting void s_unicode_to_utf8(string& s, wstring& str);
Exporting void s_utf16_to_utf8(string& s, int32_t* str, long sz);
Exporting void s_utf8_to_utf16(wstring& w, string& str);
void sc_utf8_to_utf16(std::u16string& w, unsigned char* str , long sz);

Exporting void s_utf8_to_unicode(wstring& s, unsigned char* str, long sz);
Exporting void s_doubleutf8_to_unicode(wstring& s, wchar_t* str, long sz);
Exporting uchar c_latin_to_utf8(unsigned char code, unsigned char* utf);

Exporting TAMGUCHAR c_latin_table_to_unicode(int table, TAMGUCHAR c);
Exporting TAMGUCHAR c_unicode_to_latin(TAMGUCHAR u);

//--------------------- Conversion with the recipient string cleared
Exporting void sc_unicode_to_utf8(string& s, wstring& str);
Exporting void s_unicode_to_utf8(string& s, wchar_t* str, long sz);
Exporting void sc_utf8_to_unicode(wstring& s, unsigned char* str, long sz);
Exporting void sc_utf8_to_unicode(std::u32string& w, unsigned char* str, long sz);
//--------------------- Character conversion
Exporting void c_HTMLcode(string& s, TAMGUCHAR code);
Exporting void c_XMLcode(string& s, TAMGUCHAR code);
Exporting unsigned char c_utf8_to_unicode(unsigned char* utf, TAMGUCHAR& code);
Exporting unsigned char c_utf8_to_unicode(string* utf, long i, char32_t& code);
Exporting unsigned char c_unicode_to_utf8(TAMGUCHAR code, unsigned char* utf);

Exporting bool c_unicode_to_utf16(uint32_t& r, uint32_t code);
Exporting bool c_utf16_to_unicode(uint32_t& r, uint32_t code, bool second);
Exporting bool c_utf16_to_unicode(char32_t& r, char32_t code, bool second);
//--------------------- String conversion
Exporting unsigned char conversion_utf8_to_latin(short);
Exporting string conversion_utf8_to_latin(string contenu);
Exporting string conversion_latin_to_utf8(string contenu);
Exporting string conversion_latin_to_utf8(unsigned char* contenu, long sz);
Exporting string conversion_unicode_to_latin(wstring& w);

Exporting string conversion_latin_table_to_utf8(short, unsigned char* contenu, long sz);
Exporting wstring conversion_latin_table_to_unicode(short table, unsigned char* contenu, long sz);

Exporting long conversion_utf8_to_fat(unsigned char* contenu, vector<string>& l);

Exporting string s_utf8_to_latin(unsigned char* str, long sz);
Exporting void s_latin_to_utf8(string& res, unsigned char* contenu, long sz);
Exporting void s_latin_to_unicode(wstring& res, unsigned char* contenu, long sz);

//---------------------Korean Manipulation
Exporting bool s_is_hangul(wstring& w);
Exporting bool s_is_jamo(wstring& w);
Exporting wstring s_split_jamo(wstring& w);
Exporting wstring s_combine_jamo(wstring& w);
Exporting bool c_is_hangul(TAMGUCHAR c);
Exporting bool c_is_hangul(unsigned char* m, long& i);
Exporting bool c_is_hangul(string& s);
Exporting wstring s_hangul_normalize(wstring& w);
Exporting string c_translate(TAMGUCHAR c);
Exporting string c_translate(unsigned char* m, long& i);
//--------------------- UNICODE Manipulation
Exporting bool b_alpha(TAMGUCHAR c);

Exporting bool c_is_space(TAMGUCHAR c);
Exporting bool c_is_punctuation(TAMGUCHAR c);
Exporting bool c_is_digit(TAMGUCHAR code);
Exporting bool c_is_hexa(TAMGUCHAR code);
Exporting bool c_is_lower(TAMGUCHAR code);
Exporting bool c_is_upper(TAMGUCHAR code);

Exporting bool c_is_separator(TAMGUCHAR code);

Exporting char c_is_alpha(TAMGUCHAR c);

Exporting TAMGUCHAR c_to_lower(TAMGUCHAR c);
Exporting TAMGUCHAR c_to_upper(TAMGUCHAR c);

Exporting bool scan_emoji(string& w, long& i);
Exporting bool scan_emoji(wstring& w, long& i);
Exporting bool scan_emoji(unsigned char* w, long& i);
Exporting bool get_emoji(unsigned char* w, string& res, long& i);
Exporting bool get_emoji(string& w, string& res, long& i);
Exporting bool get_emoji(wstring& w, wstring& res, long& i);
Exporting bool store_emoji(unsigned char* w, string& res, long& i);
Exporting bool store_emoji(string& w, string& res, long& i);
Exporting bool store_emoji(wstring& w, wstring& res, long& i);

    
Exporting bool c_is_emoji(TAMGUCHAR c);
Exporting bool c_is_emojicomp(TAMGUCHAR c);
Exporting bool c_is_emoji(unsigned char* m, long& i);
Exporting bool c_is_emojicomp(unsigned char* m, long& i);
Exporting string Emoji(TAMGUCHAR c);
Exporting string Emoji(string&);
Exporting string Emoji(wstring& s);
Exporting bool s_is_emoji(string& s);
Exporting bool s_is_emoji(wstring& s);
Exporting void l_emojis(map<BLONG, string>& dico);

//--------------------- Character manipulation
Exporting void c_to_lower(string&, unsigned char* s, long lg);
Exporting void c_to_upper(string&, unsigned char* s, long lg);

Exporting char c_is_alpha(unsigned char* m, long& i);

Exporting string c_latin_utf8(long c);
Exporting std::set<char32_t>& vpunctuations();
//--------------------- String manipulation
Exporting bool s_is_upper(string& s);
Exporting bool s_is_alpha(string& s);
Exporting bool s_is_lower(string& s);
Exporting bool s_is_digit(string& str);
Exporting bool s_is_punctuation(string& str);
Exporting bool s_is_space(string& str);


Exporting bool s_is_punctuation(wstring& str);
Exporting bool s_is_alpha(wstring& s);
Exporting bool s_is_upper(wstring& s);
Exporting bool s_is_lower(wstring& s);
Exporting bool s_is_vowel(wstring& s);
Exporting bool s_is_consonant(wstring& s);
Exporting bool s_is_digit(wstring& str);

Exporting bool s_is_consonant(unsigned char* s);
Exporting bool s_is_vowel(unsigned char* s);

Exporting string s_to_lower(string& s);
Exporting string s_to_upper(string& s);
Exporting string s_deaccentuate(unsigned char* s);

Exporting wstring s_to_upper(wstring& s);
Exporting wstring s_to_lower(wstring& s);
Exporting wstring s_deaccentuate(wstring& s);
Exporting bool compare_vowel(TAMGUCHAR c, TAMGUCHAR cc);

Exporting bool c_is_consonant(TAMGUCHAR c);
Exporting bool c_is_vowel(TAMGUCHAR c);

Exporting char s_is_number(unsigned char* str, char decimal, long& l, double& f);
Exporting void s_binary(unsigned char* s, int mx, long lg, TAMGUCHAR* codes);

//--------------------- Finding or replacing
Exporting char* strrstr(char* str, char* sub, long sz, long szsub);
Exporting wchar_t* wcsrstr(wchar_t* str, wchar_t* sub, long sz, long szsub);
Exporting void s_findall(string& s, string& substr, vector<long>& v);
Exporting void s_findall(wstring& s, wstring& substr, vector<long>& v, long init=0);

Exporting long s_findbyte(uchar* s, long sz, string& substr, long i);
Exporting long s_rfindbyte(uchar* s, long sz, string& substr, long i);

Exporting long s_findbyte(string& s, string& substr, long i);
Exporting long s_rfindbyte(string& s, string& substr, long i);
Exporting long s_find(string& s, string& fd, long pos);
Exporting long s_rfind(string& s, string& fd, long pos);
Exporting long s_find(wstring& s, wstring& substr, long i);
Exporting long s_rfind(wstring& s, wstring& substr, long pos);

Exporting long s_count(string& str, string& sub, long i);
Exporting long s_count(wstring& str, wstring& sub, long i);
//--------------------- Extracting substring
Exporting string s_left(string& s, long nb);
Exporting string s_right(string& s, long nb);
Exporting string s_middle(string& s, long g, long nb);

Exporting string s_replacestring(string& s, string& r, string& rep);
Exporting string s_replacestrings(string& s, string r, string rep);

Exporting string s_insert_sep(string s, string sep);
Exporting string s_insert_sep_byte(string& s, string sep);
Exporting string s_char_reverse(string& s, long& length);
Exporting string s_revert(string& s);

Exporting wstring s_left(wstring& s, long nb);
Exporting wstring s_right(wstring& s, long nb);
Exporting wstring s_middle(wstring& s, long l, long nb);
Exporting wstring s_replacestring(wstring& str, wstring reg, wstring rep);
std::u32string s_ureplacestring(std::u32string& s, std::u32string reg, std::u32string rep);

Exporting wstring s_revert(wstring& s);


Exporting long size_utf16(unsigned char* str, long sz, long& charsize);
Exporting long size_c(unsigned char* contenu, long sz, long& first);
Exporting long size_c(unsigned char* s, long sz);
Exporting long size_c(string& s);
Exporting long size_c(const char* s);
Exporting long size_c(wstring& w);
Exporting long size_c(wstring& w, long&);

#ifdef WSTRING_IS_UTF16
Exporting size_t size_w(wchar_t* w);
Exporting size_t size_w(wstring& w, long& first);
size_t size_w(wstring& w);
long convertchartoposutf16(wstring& w, long first, long i);
long convertpostocharutf16(wstring& w, long first, long i);
inline bool checklargeutf16(wchar_t c) {
	if ((c & 0xFF00) == 0xD800)
		return true;
	return false;
}
#endif

Exporting void s_split(string& s, string& splitter, vector<string>& strs, bool keepblanks = false);
Exporting void s_split(wstring& s, wstring& splitter, vector<wstring>& strs, bool keepblanks = false);
Exporting void v_split(string& thestr, string thesplitter, vector<string>& v);
Exporting void v_split_indent(string& thestr, vector<string>& v);

//--------------------- Character extraction from potential UTF8 string-----------------
Exporting TAMGUCHAR c_char_get_wide(unsigned char* m, long& i);

Exporting unsigned char c_utf8_latin(string s);

Exporting long convertpostochar(wstring& w, long first, long spos);
Exporting long convertchartopos(wstring& w, long first, long cpos);

Exporting long c_code_get(unsigned char* m, long& i, TAMGUCHAR& code);
Exporting long c_chartobyteposition(unsigned char* contenu, long sz, long charpos);
Exporting long c_chartobyteposition(unsigned char* contenu, long charpos);
Exporting long c_bytetocharposition(unsigned char* contenu, long charpos);
Exporting long c_chartobytepositionidx(unsigned char* contenu, long charpos, long& nb, long& i);
Exporting long c_bytetocharpositionidx(unsigned char* contenu, long charpos, long& nb, long& i);
Exporting long c_char_next(unsigned char* m, long& i);
Exporting void v_convertbytetocharposition(unsigned char* s, vector<long>& v);
Exporting void v_convertchartobyteposition(unsigned char* s, vector<long>& v);

Exporting string c_char_index(string& s, size_t i);
Exporting string c_char_index_remove(string& s, size_t i);

Exporting bool c_char_index_assign(string& s, string c, long i);
Exporting bool c_char_index_insert(string& s, string c, size_t i);

Exporting TAMGUCHAR c_char_index_code(string& s, size_t i);

Exporting void c_char_index_code_all(string& s, vector<long>& vect);
Exporting void c_char_index_code_all_long(string& s, vector<TAMGUCHAR>& vect);

Exporting string c_char_get(unsigned char* m, long& i);
Exporting string c_char_get_next(unsigned char* m, size_t& i);
void c_chars_get_next(unsigned char* m, char* str, size_t& i);

//--------------------- Tokenization
Exporting bool s_threedigits(wstring& s, long i);


bool vw_tokenize(vector<wstring>& vect, wstring& thestr, short flag, vector<string>&);
bool v_tokenize(vector<string>& vect, string& thestr, short flag, vector<string>&);
void vw_tokenize(vector<wstring>& vect, wstring& thestr, short flag);
void s_tokenize(vector<wstring>& vect, wstring& thestr, map<wstring, bool>&);
void v_tokenize(vector<string>& vect, string& thestr, short flag);
void s_tokenize(vector<string>& vect, string& thestr, map<string, bool>&);

//--------------------- agnostring implementation

class agnostring : public string {
public:
	size_t bytepos;
	size_t charpos;

	agnostring() : string() {}
	agnostring(agnostring& s) : string(s.c_str()) { bytepos = 0; charpos = 0; }

	agnostring(const char* s) : string(s) { bytepos = 0; charpos = 0; }
	agnostring(const unsigned char* s) : string((const char*)s) { bytepos = 0; charpos = 0; }

	agnostring(char* s) : string(s) { bytepos = 0; charpos = 0; }
	agnostring(unsigned char* s) : string((char*)s) { bytepos = 0; charpos = 0; }

	agnostring(string s) : string(s) { bytepos = 0; charpos = 0; }
	agnostring(wstring s) { s_unicode_to_utf8(*this, s);  bytepos = 0; charpos = 0; }

	agnostring(TAMGUCHAR b) : string(c_unicode_to_utf8(b)) { bytepos = 0; charpos = 0; }

	wchar_t tonumber() {
		return conversionintegerhexa((char*)c_str());
	}

	string tolower() {
		return s_to_lower(*this);
	}

	string toupper() {
		return s_to_upper(*this);
	}

	bool isupper() {
		return s_is_upper(*this);
	}

	string emoji() {
		return Emoji(*this);
	}

	bool isemoji() {
		return s_is_emoji(*this);
	}

	bool isalpha() {
		return s_is_alpha(*this);
	}

	bool islower() {
		return s_is_lower(*this);
	}

	bool isconsonant() {
		return s_is_consonant((unsigned char*)c_str());
	}

	bool isvowel() {
		return s_is_vowel((unsigned char*)c_str());
	}

	bool ispunctuation() {
		return s_is_punctuation(*this);
	}

	bool isspace() {
		return s_is_space(*this);
	}

	string deaccentuate() {
		return s_deaccentuate((unsigned char*)c_str());
	}

	bool isdigit() {
		return s_is_digit(*this);
	}

	char isnumber(char decimal, long& l, double& f) {
		return s_is_number((unsigned char*)c_str(), decimal, l, f);
	}

	bool isutf8() {
		return s_is_utf8((unsigned char*)c_str(), size());
	}

	string utf8tolatin() {
		return s_utf8_to_latin((unsigned char*)c_str(), size());
	}

	wstring utf8tounicode() {
		wstring ws;
		s_utf8_to_unicode(ws, (unsigned char*)c_str(), size());
		return ws;
	}

	wstring latintounicode() {
		wstring ws;
		s_utf8_to_unicode(ws, (unsigned char*)c_str(), size());
		return ws;
	}

	string latintoutf8() {
		return conversion_latin_to_utf8((unsigned char*)c_str(), size());
	}

	string latintoutf8(short table) {
		return conversion_latin_table_to_utf8(table, (unsigned char*)c_str(), size());
	}

	wstring latintounicode(short table) {
		return conversion_latin_table_to_unicode(table, (unsigned char*)c_str(), size());
	}

	string dostoutf8() {
		return s_dos_to_utf8((unsigned char*)c_str());
	}

	string utf8todos() {
		return s_utf8_to_dos((unsigned char*)c_str());
	}

	void begin() {
		bytepos = 0;
		charpos = 0;
	}

	bool end() {
		if (bytepos >= size())
			return true;
		return false;
	}

	size_t getbytepos() {
		return bytepos;
	}

	void setbytepos(size_t i) {
		bytepos = i;
		charpos = c_bytetocharposition((unsigned char*)c_str(), (long)bytepos);
	}

	size_t getcharpos() {
		return charpos;
	}

	void setcharpos(long i) {
		charpos = i;
		bytepos = c_chartobyteposition((unsigned char*)c_str(), (long)charpos);
	}

    void getpos(size_t& b, size_t& c) {
        b = bytepos;
        c = charpos;
    }

    void setpos(size_t b, size_t c) {
        bytepos = b;
        charpos = c;
    }

    //In this case, we also take into account emojis and their complement
    string nextemoji() {
        string s;
        charpos++;
        long bp = bytepos;
        if (!get_emoji((unsigned char*)c_str(), s, bp)) {
            TAMGUCHAR v;
            bytepos += 1 + c_utf8_to_unicode((unsigned char*)c_str() + bytepos, v);
            s = c_unicode_to_utf8(v);
        }
        else
            bytepos = bp + 1;
        return s;
    }
    

    string next() {
        char e[] = {0,0,0,0,0};

        charpos++;
        c_chars_get_next((unsigned char*)c_str(), e, bytepos);
        return e;
    }

    string next(long& line) {
        char e[] = {0,0,0,0,0};

        charpos++;
        c_chars_get_next((unsigned char*)c_str(), e, bytepos);
        if (e[0]=='\n')
            line++;
        
        return e;
    }

    void nextc(char* e) {
        charpos++;
        c_chars_get_next((unsigned char*)c_str(), e, bytepos);
    }

    void nextc(char* e, long& line) {
        charpos++;
        c_chars_get_next((unsigned char*)c_str(), e, bytepos);
        if (e[0]=='\n')
            line++;
    }

    void nextc(long& line) {
        charpos++;
        bytepos +=  1 + c_test_utf8(((unsigned char*)c_str()) + bytepos);
        if (at(bytepos) == '\n')
            line++;
    }

	TAMGUCHAR nextcode() {
		charpos++;
		TAMGUCHAR v;
		bytepos += 1 + c_utf8_to_unicode((unsigned char*)c_str() + bytepos, v);
		return v;
	}

	void replacecurrent(TAMGUCHAR w) {
		uchar c[4];
		c_unicode_to_utf8(w, c);
		string& s = *this;

		for (long u = 0; c[u]; u++)
			s[bytepos+u] = c[u];
	}


void switchcurrent() {
        size_t i = bytepos;
        string lc = c_char_get_next((unsigned char*)c_str(), i);
        string nx = c_char_get_next((unsigned char*)c_str(), i);
        nx+=lc;
        string& s = *this;
        for (long u = 0; u < nx.size(); u++)
            s[bytepos+u] = nx[u];
    }
    
	void following() {
		charpos++;
		bytepos += 1 + c_test_utf8((unsigned char*)c_str() + bytepos);
	}

	string current() {
		size_t i = bytepos;
		return c_char_get_next((unsigned char*)c_str(), i);
	}

	wstring wcurrent() {
		size_t i = bytepos;
		string s = c_char_get_next((unsigned char*)c_str(), i);
		wstring ws;
		s_utf8_to_unicode(ws, USTR(s), s.size());
		return ws;
	}

	string next(size_t& i) {
		return c_char_get_next((unsigned char*)c_str(), i);
	}

	wchar_t nextcode(long& i) {
		TAMGUCHAR v;
		i += 1 + c_utf8_to_unicode((unsigned char*)c_str() + i, v);
		return v;
	}

	void following(long& i) {
		i += 1 + c_test_utf8((unsigned char*)c_str() + i);
	}

	char getbyte(long i) {
		string& s = *this;
		return s[i];
	}

	string operator [](long i) {
		return c_char_index(*this, i);
	}

	string trim() {
		return Trim(*this);
	}

	string trimleft() {
		return Trimleft(*this);
	}

	string trimright() {
		return Trimright(*this);
	}

	string replace(string r, string rep) {
		return s_replacestring(*this, r, rep);
	}

	bool insert(string c, size_t i) {
		return c_char_index_insert(*this, c, i);
	}

	long sfindpos(string fd, long pos) {
		return s_find(*this, fd, pos);
	}

	long rfindpos(string fd, long pos) {
		return s_rfind(*this, fd, pos);
	}

    long findbyte(string fd, long pos) {
        return s_findbyte(*this, fd, pos);
    }
    
    long rfindbyte(string fd, long pos) {
        return s_rfindbyte(*this, fd, pos);
    }

	void findall(string fd, vector<long>& v) {
		s_findall(*this, fd, v);
	}

	string extract(long g, long nb) {
		return s_middle(*this, g, nb);
	}

	string left(long nb) {
		return s_left(*this, nb);
	}

	string right(long nb) {
		return s_right(*this, nb);
	}

	long chartobyteposition(long pos) {
		return c_chartobyteposition((unsigned char*)c_str(), pos);
	}

	long bytetocharposition(long pos) {
		return c_bytetocharposition((unsigned char*)c_str(), pos);
	}

	bool r_tokenize(vector<string>& vect, bool comma, bool separator, bool keepwithdigit, vector<string>& rules) {
		short flags = token_period;
		if (comma)
			flags |= token_comma;
		if (separator)
			flags |= token_separator;
		if (keepwithdigit)
			flags |= token_keepwithdigit;
		return v_tokenize(vect, *this, flags, rules);
	}

    void tokenize(vector<string>& vect, bool comma, bool separator, bool keepwithdigit) {
        short flags = token_period;
        if (comma)
            flags |= token_comma;
        if (separator)
            flags |= token_separator;
        if (keepwithdigit)
            flags |= token_keepwithdigit;
        v_tokenize(vect, *this, flags);
    }
    
	void tokenize(vector<string>& vect, short flag) {
		v_tokenize(vect, *this, flag);
	}

	void stokenize(vector<string>& vect, map<string, bool>& keeps) {
		s_tokenize(vect, *this, keeps);
	}

	void tokenize(vector<wstring>& vect, bool comma, bool separator, bool keepwithdigit) {
		short flags = token_period;
		if (comma)
			flags |= token_comma;
		if (separator)
			flags |= token_separator;
		if (keepwithdigit)
			flags |= token_keepwithdigit;

		wstring ws;
		s_utf8_to_unicode(ws, (unsigned char*)c_str(), size());
		vw_tokenize(vect, ws, flags);
	}


	void tokenize(vector<wstring>& vect, short flag) {
		wstring ws;
		s_utf8_to_unicode(ws, (unsigned char*)c_str(), size());
		vw_tokenize(vect, ws, flag);
	}

	void stokenize(vector<wstring>& vect, map<wstring, bool>& keeps) {
		wstring ws;
		s_utf8_to_unicode(ws, (unsigned char*)c_str(), size());
		s_tokenize(vect, ws, keeps);
	}

	void split(string thesplitter, vector<string>& v) {
		v_split(*this, thesplitter, v);
	}

	void unicodes(vector<long>& vect) {
		c_char_index_code_all(*this, vect);
	}

	size_t sizec() {
		return size_c(c_str());
	}

	string revert() {
		return s_revert(*this);
	}

	Exporting string insertseparator(string sep) {
		return s_insert_sep(*this, sep);
	}

	wchar_t code(size_t i) {
		return c_char_index_code(*this, i);
	}

	long count(string x) {
		long cp = 0;
		long ps = 0;
		while ((ps = find(x, ps)) != string::npos) {
			cp++;
			ps++;
		}
		return cp;
	}

	long count(wchar_t b) {
		long cp = 0;
		begin();
		while (!end()) {
			if (b == nextcode())
				cp++;
		}
		return cp;
	}
};

//------------------------------------------------------------------------------------------
//This is a specific class for OS storing UTF16 characters in wstring (mainly Windows)
//This class transforms a UTF16 wstring into a buffer of uint32_t unicode codes.
//It simplifies the computing of size and detection of characters, since large unicode codes are stored
//as one element instead of two... The problem is especially acute with emojis.
#ifdef WSTRING_IS_UTF16
class unicodestring {
	TAMGUCHAR* buffer;
	long bsz;
	long sz;

public:

	unicodestring() {
		buffer = NULL;
		bsz = 0;
		sz = 0;
	}

	unicodestring(wstring& w) {
		bsz = 1 + (w.size() << 1);
		buffer = new TAMGUCHAR[bsz];
		TAMGUCHAR c;
		sz = 0;
		for (long i = 0; i < w.size(); i++) {
			if (c_utf16_to_unicode(c, w[i], false))
				c_utf16_to_unicode(c, w[++i], true);
			buffer[sz++] = c;
		}
		buffer[sz] = 0;
	}

	unicodestring(unicodestring& w) {
		sz = w.sz;
		bsz = w.bsz;
		buffer = new TAMGUCHAR[bsz];
		for (long i = 0; i <= sz; i++) {
			buffer[i] = w.buffer[i];
		}
	}

	~unicodestring() {
		if (buffer != NULL)
			delete[] buffer;
	}

	wstring value() {
		wstring w;
		TAMGUCHAR c;
		for (long i = 0; i < sz; i++) {
			if (c_unicode_to_utf16(c, buffer[i])) {
				w += (wchar_t)(c >> 16);
				w += (wchar_t)(c & 0xFFFF);
			}
			else
				w += (wchar_t)buffer[i];
		}
		return w;
	}

	void replace(TAMGUCHAR rep, long pos, long ssz) {
		buffer[pos++] = rep;
		while (pos + ssz <= sz) {
			buffer[pos] = buffer[pos + ssz - 1];
			pos++;
		}
		sz -= ssz - 1;
		buffer[sz] = 0;
	}

	long size() {
		return sz;
	}

	unicodestring substr(long pos, long offset) {
		unicodestring u;
		u.bsz = offset << 1;
		u.buffer = new TAMGUCHAR[bsz];
		for (; pos < sz && offset; pos++) {
			u.buffer[u.sz++] = buffer[pos];
			--offset;
		}
		u.buffer[u.sz];
		return u;
	}

	long find(wstring s, long init) {
		long ps = -1;
		long ssz = s.size();
		if (ssz > sz)
			return -1;
		for (long i = init; i <= sz - ssz; i++) {
			if (buffer[i] == s[0]) {
				if (ssz == 1)
					return i;
				ps = 1;
				while (ps < ssz && buffer[i + ps] == s[ps]) ps++;
				if (ps == ssz)
					return i;
			}
		}
		return -1;
	}

	inline void operator =(int c) {
		if (sz >= bsz) {
			if (buffer != NULL)
				delete[] buffer;

			bsz = 2;
			buffer = new TAMGUCHAR[bsz];
		}
		buffer[0] = c;
		sz = 1;
	}

	inline void operator =(wstring& w) {
		if (w.size() >= bsz) {
			if (buffer != NULL)
				delete[] buffer;

			bsz = 1 + (w.size() << 1);
			buffer = new TAMGUCHAR[bsz];
		}
		sz = 0;
		TAMGUCHAR c;
		for (long i = 0; i < w.size(); i++) {
			if (c_utf16_to_unicode(c, w[i], false))
				c_utf16_to_unicode(c, w[++i], true);
			buffer[sz++] = c;
		}
		buffer[sz] = 0;
	}

	inline TAMGUCHAR& operator[](long i) {
		return buffer[i];
	}

	inline void operator +=(wstring& w) {
		if (sz + w.size() >= bsz) {
			bsz = (sz + w.size()) * 2;
			if (buffer == NULL)
				buffer = new TAMGUCHAR[bsz];
			else {
				TAMGUCHAR* bb = new TAMGUCHAR[bsz];
				for (long i = 0; i <= sz; i++)
					bb[i] = buffer[i];
				delete[] buffer;
				buffer = bb;
			}
		}
		TAMGUCHAR c;
		for (long i = 0; i < w.size(); i++) {
			if (c_utf16_to_unicode(c, w[i], false))
				c_utf16_to_unicode(c, w[++i], true);
			buffer[sz++] = c;
		}
		buffer[sz] = 0;
	}
};

#define TAMGUVALUE(w) w.value()
Exporting void concat_char_check_utf16(wstring& res, TAMGUCHAR code);
Exporting void store_char_check_utf16(wstring& res, TAMGUCHAR code);
inline TAMGUCHAR getachar(wstring& s, long& i) {
    TAMGUCHAR c;
    if (c_utf16_to_unicode(c, s[i], false))
        c_utf16_to_unicode(c, s[++i], true);
    return c;
}

inline long getChar(wstring& s, long i, TAMGUCHAR& c) {
	if (c_utf16_to_unicode(c, s[i], false)) {
		c_utf16_to_unicode(c, s[i + 1], true);
		return 2;
	}
	return 1;
}

#else
#define unicodestring wstring
#define TAMGUVALUE(w) w
#define concat_char_check_utf16(w,c) w += c
#define store_char_check_utf16(w,c) w = c
#define getachar(s,i) s[i]
#endif

//return both a wstring, which may contain an emoji
wstring getfullchar(wstring& s, long& i);

//--------------------------------------------------------------------------------------------------------
class Fast_String {
    uchar buff[80];
    long lenneo;
    long ineo;

    public:
    
    uchar* neo;
    
    Fast_String(long l) : lenneo(l), ineo(0), neo(new uchar[l]) {}
    
    ~Fast_String() {
        delete[] neo;
    }

    inline void substr(long from, long to) {
        ineo = to-from;
        memcpy(neo, neo+from, ineo);
        neo[ineo] = 0;
    }
    
    inline void substr(uchar* val, long from, long to) {
        ineo = to-from;
        if (ineo >= lenneo) {
            lenneo += ineo;
            lenneo <<= 1;
            delete[] neo;
            neo = new uchar[lenneo];
        }
        memcpy(neo, val+from, ineo);
        neo[ineo] = 0;
    }
    
    inline void set(string& ctn) {
        ineo = ctn.size();
        if (ineo >= lenneo) {
            lenneo += ineo;
            lenneo <<= 1;
            delete[] neo;
            neo = new uchar[lenneo];
        }
        memcpy(neo,STR(ctn), ineo);
        neo[ineo] = 0;
    }

    inline void set(uchar* ctn, long size_ctn) {
        ineo = size_ctn;
        if (ineo >= lenneo) {
            lenneo += ineo;
            lenneo <<= 1;
            delete[] neo;
            neo = new uchar[lenneo];
        }
        memcpy(neo,ctn, ineo);
        neo[ineo] = 0;
    }
    
    inline void reset(long i) {
        ineo = i;
        neo[ineo] = 0;
    }
    
    inline void add(uchar* ctn, long size_ctn) {
        if ((ineo + size_ctn) >= lenneo) {
            lenneo += size_ctn;
            lenneo <<= 1;
            uchar* s = new uchar[lenneo];
            memcpy(s, neo, ineo);
            delete[] neo;
            neo = s;
        }
        memcpy(neo+ineo,ctn, size_ctn);
        ineo += size_ctn;
        neo[ineo] = 0;
    }
    
    inline void add(uchar c) {
        if ((ineo + 1) >= lenneo) {
            lenneo <<= 1;
            uchar* s = new uchar[lenneo];
            memcpy(s, neo, ineo);
            delete[] neo;
            neo = s;
        }
        neo[ineo++] = c;
    }
    
    void addonechar(unsigned char* m, long i) {
        long nb = c_test_utf8(m + i);
        if (!nb) {
            neo[0] = m[i];
            neo[1] = 0;
            ineo = 1;
            return;
        }

        ineo = 0;
        neo[ineo++] = m[i];
        long j = i;
        if (scan_emoji(m, j)) {
            for (long u = i + 1; u <= j; u++)
                add(m[u]);
            add(0);
        }
        else {
            switch (nb) {
                case 1:
                    neo[ineo++] = (char)m[i+1];
                    break;
                case 2:
                    neo[ineo++] = (char)m[i+1];
                    neo[ineo++] = (char)m[i+2];
                    break;
                case 3:
                    neo[ineo++] = (char)m[i+1];
                    neo[ineo++] = (char)m[i+2];
                    neo[ineo++] = (char)m[i+3];
            }
            neo[ineo] = 0;
        }
    }

    void addonechar(long i) {
        long nb = c_test_utf8(neo + i);
        if (!nb) {
            neo[0] = neo[i];
            neo[1] = 0;
            ineo = 1;
            return;
        }

        ineo = 0;
        buff[ineo++] = neo[i];
        long j = i;
        if (scan_emoji(neo, j)) {
            for (long u = i + 1; u <= j; u++)
                buff[ineo++] = neo[u];
        }
        else {
            switch (nb) {
                case 1:
                    buff[ineo++] = neo[i+1];
                    break;
                case 2:
                    buff[ineo++] = neo[i+1];
                    buff[ineo++] = neo[i+2];
                    break;
                case 3:
                    buff[ineo++] = neo[i+1];
                    buff[ineo++] = neo[i+2];
                    buff[ineo++] = neo[i+3];
            }
        }
        memcpy(neo,buff,ineo);
        neo[ineo] = 0;
    }

    inline long size() {
        return ineo;
    }
    
    inline char* str() {
        return (char*)neo;
    }
    
    inline uchar operator[](long i) {
        return neo[i];
    }
    
    inline uchar get(long i) {
        return neo[i];
    }
    
    inline void downsize(long sz) {
        if (sz < lenneo) {
            delete[] neo;
            lenneo = sz;
            neo = new uchar[lenneo];
        }
        memset(neo, '\0', lenneo);
        ineo = 0;
    }

    inline void clear() {
        ineo = 0;
        neo[0]=0;
    }
    
    inline void signature() {
        if (neo[0] == 239 && neo[1] == 187 && neo[2] == 191) {
            ineo -= 3;
            memcpy(neo, neo + 3, ineo);
        }
    }
    
    inline wstring& latintounicode(wstring& ws) {
        sc_utf8_to_unicode(ws, neo, ineo);
        return ws;
    }
};

#endif







