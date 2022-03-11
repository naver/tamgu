/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : utf8handler.h
 Date       : 2022/03/10
 Purpose    : classes to handle different encoding, including emojis (see tables.h)
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef utf8handler_h
#define utf8handler_h

#include "vecta.h"
#include <set>

using std::u32string;

void c_unicode_to_utf16(wstring& w, uint32_t c);
char* unicode_2_utf8(long code, char* utf);

class Emoji_Arc {
public:
    vecte<Emoji_Arc*> arcs;
    char32_t label;
    bool end;
    
    Emoji_Arc(char32_t e) : label(e), end(false) {}
    
    Emoji_Arc* find(char32_t e) {
        long i = 0;
        for (; i < arcs.last && arcs[i]->label != e; i++) {}
        return (i == arcs.last)?NULL:arcs[i];
    }
    
    long search(char32_t e) {
        long i = 0;
        for (i = 0; i < arcs.last && arcs[i]->label != e; i++) {}
        return (i == arcs.last)?-1:i;
    }
    
    Emoji_Arc* add(char32_t e) {
        Emoji_Arc* a = new Emoji_Arc(e);
        arcs.push_back(a);
        return a;
    }
    
    bool check(std::u32string& u, long p) {
        if (p == u.size())
            return end;
        Emoji_Arc* a = find(u[p]);
        if (a != NULL)
            return a->check(u, p + 1);
        return false;
    }
    
    bool check(string& u, long p) {
        if (p == u.size())
            return end;
        Emoji_Arc* a = find((uchar)u[p]);
        if (a != NULL)
            return a->check(u, p + 1);
        return false;
    }
    
    bool check(wstring& u, long p) {
        if (p == u.size())
            return end;
        Emoji_Arc* a = find(u[p]);
        if (a != NULL)
            return a->check(u, p + 1);
        return false;
    }
    
    bool check(unsigned char* u, long p) {
        if (!u[p])
            return end;
        Emoji_Arc* a = find((uchar)u[p]);
        if (a != NULL)
            return a->check(u, p + 1);
        return false;
    }
    
    bool scan(std::u32string& u, long p, long& i) {
        Emoji_Arc* a = find(u[p]);
        if (a != NULL)
            return a->scan(u, p + 1, i);
        
        if (end) {
            i = p;
            return true;
        }
        return false;
    }
    
    bool scan(string& u, long p, long& i) {
        Emoji_Arc* a = find((uchar)u[p]);
        if (a != NULL) {
            return a->scan(u, p + 1, i);
        }
        if (end) {
            i = p;
            return true;
        }
        return false;
    }
    
    bool scan(wstring& u, long p, long& i) {
        Emoji_Arc* a = find(u[p]);
        if (a != NULL)
            return a->scan(u, p + 1, i);
        
        if (end) {
            i = p;
            return true;
        }
        return false;
    }
    
    bool scan(unsigned char* u, long p, long& i) {
        Emoji_Arc* a = find(u[p]);
        if (a != NULL)
            return a->scan(u, p + 1, i);
        
        if (end) {
            i = p;
            return true;
        }
        return false;
    }
    
    void trim() {
        for (long i = 0; i < arcs.last; i++)
            arcs[i]->trim();
        arcs.trim();
    }
    
    ~Emoji_Arc() {
        arcs.clean();
    }
    
};

class Emojis {
public:
    
    std::unordered_map<char, Emoji_Arc*> utf8_arcs;
    std::unordered_map<wchar_t, Emoji_Arc*> utf16_arcs;
    std::unordered_map<char32_t, Emoji_Arc*> utf32_arcs;
    
    std::set<char32_t> complements;
    
    Emojis() {
        store();
    }
    
    ~Emojis() {
        for (const auto& a : utf8_arcs)
            delete a.second;
        
        for (const auto& a : utf16_arcs)
            delete a.second;
        
        for (const auto& a : utf32_arcs)
            delete a.second;
    }
    
    void traverse(Emoji_Arc* a, char32_t* e) {
        long i = 0;
        long pos;
        while (e[i]) {
            complements.insert(e[i]);
            pos = a->search(e[i]);
            if (pos == -1) {
                a = a->add(e[i]);
            }
            else
                a = a->arcs[pos];
            i++;
        }
        a->end = true;
    }
    
    void traverse(Emoji_Arc* a, string& e) {
        long i = 1;
        long pos;
        while (i < e.size()) {
            pos = a->search((uchar)e[i]);
            if (pos == -1) {
                a = a->add((uchar)e[i]);
            }
            else
                a = a->arcs[pos];
            i++;
        }
        a->end = true;
    }
    
    void traverse(Emoji_Arc* a, wstring& e) {
        long i = 1;
        long pos;
        while (i < e.size()) {
            pos = a->search(e[i]);
            if (pos == -1) {
                a = a->add(e[i]);
            }
            else
                a = a->arcs[pos];
            i++;
        }
        a->end = true;
    }
    
    void store();
    
    bool isemoji(char32_t c) {
        return (c > 57 && utf32_arcs.find(c) != utf32_arcs.end());
    }
    
    bool isemojicomplement(char32_t c) {
        return (complements.find(c) != complements.end());
    }
    
    bool isemoji(std::u32string& u) {
        if (!u.size())
            return false;
        
        std::unordered_map<char32_t, Emoji_Arc*>::iterator it = utf32_arcs.find(u[0]);
        return (it != utf32_arcs.end() && it->second->check(u, 1));
    }
    
    bool scan(std::u32string& u, long& i) {
        std::unordered_map<char32_t, Emoji_Arc*>::iterator it = utf32_arcs.find(u[i]);
        if (it == utf32_arcs.end())
            return false;
        long p = ++i;
        bool found = it->second->scan(u, p, i);
        --i;
        return found;
    }
    
    bool get(std::u32string& u, std::u32string& res, long& i) {
        std::unordered_map<char32_t, Emoji_Arc*>::iterator it = utf32_arcs.find(u[i]);
        if (it == utf32_arcs.end())
            return false;
        long p = ++i;
        if (it->second->scan(u, p, i)) {
            res = u.substr(p - 1, i - p + 1);
            --i;
            return true;
        }
        --i;
        return false;
    }
    
    bool store(std::u32string& u, std::u32string& res, long& i) {
        std::unordered_map<char32_t, Emoji_Arc*>::iterator it = utf32_arcs.find(u[i]);
        if (it == utf32_arcs.end())
            return false;
        long p = ++i;
        if (it->second->scan(u, p, i)) {
            res += u.substr(p - 1, i - p + 1);
            --i;
            return true;
        }
        --i;
        return false;
    }
    
    bool isemoji(string& u) {
        if (!u.size())
            return false;
        std::unordered_map<char, Emoji_Arc*>::iterator it = utf8_arcs.find(u[0]);
        return (it != utf8_arcs.end() && it->second->check(u, 1));
    }
    
    bool scan(string& u, long& i) {
        std::unordered_map<char, Emoji_Arc*>::iterator it = utf8_arcs.find(u[i]);
        if (it == utf8_arcs.end())
            return false;
        long p = ++i;
        bool found = it->second->scan(u, p, i);
        --i;
        return found;
    }
    
    bool get(string& u, string& res, long& i) {
        std::unordered_map<char, Emoji_Arc*>::iterator it = utf8_arcs.find(u[i]);
        if (it == utf8_arcs.end())
            return false;
        long p = ++i;
        if (it->second->scan(u, p, i)) {
            res = u.substr(p - 1, i - p + 1);
            --i;
            return true;
        }
        --i;
        return false;
    }
    
    bool store(string& u, string& res, long& i) {
        std::unordered_map<char, Emoji_Arc*>::iterator it = utf8_arcs.find(u[i]);
        if (it == utf8_arcs.end())
            return false;
        long p = ++i;
        if (it->second->scan(u, p, i)) {
            res += u.substr(p - 1, i - p + 1);
            --i;
            return true;
        }
        --i;
        return false;
    }
    
    bool isemoji(unsigned char* u) {
        if (!u[0])
            return false;
        std::unordered_map<char, Emoji_Arc*>::iterator it = utf8_arcs.find(u[0]);
        return (it != utf8_arcs.end() && it->second->check(u, 1));
    }
    
    bool scan(unsigned char* u, long& i) {
        std::unordered_map<char, Emoji_Arc*>::iterator it = utf8_arcs.find(u[i]);
        if (it == utf8_arcs.end())
            return false;
        long p = ++i;
        bool found = it->second->scan(u, p, i);
        --i;
        return found;
    }
    
    bool get(unsigned char* u, string& res, long& i) {
        std::unordered_map<char, Emoji_Arc*>::iterator it = utf8_arcs.find(u[i]);
        if (it == utf8_arcs.end())
            return false;
        long p = ++i;
        if (it->second->scan(u, p, i)) {
            res = "";
            p--;
            while (p != i)
                res += u[p++];
            --i;
            return true;
        }
        --i;
        return false;
    }
    
    bool store(unsigned char* u, string& res, long& i) {
        std::unordered_map<char32_t, Emoji_Arc*>::iterator it = utf32_arcs.find(u[i]);
        if (it == utf32_arcs.end())
            return false;
        long p = ++i;
        if (it->second->scan(u, p, i)) {
            p--;
            while (p != i)
                res += u[p++];
            --i;
            return true;
        }
        --i;
        return false;
    }
    
    bool isemoji16(wstring& u) {
        if (!u.size())
            return false;
        std::unordered_map<wchar_t, Emoji_Arc*>::iterator it = utf16_arcs.find(u[0]);
        return (it != utf16_arcs.end() && it->second->check(u, 1));
    }
    
    bool scan16(wstring& u, long& i) {
        std::unordered_map<wchar_t, Emoji_Arc*>::iterator it = utf16_arcs.find(u[i]);
        if (it == utf16_arcs.end())
            return false;
        long p = ++i;
        bool found = it->second->scan(u, p, i);
        --i;
        return found;
    }
    
    bool get16(wstring& u, wstring& res, long& i) {
        std::unordered_map<wchar_t, Emoji_Arc*>::iterator it = utf16_arcs.find(u[i]);
        if (it == utf16_arcs.end())
            return false;
        long p = ++i;
        if (it->second->scan(u, p, i)) {
            res = u.substr(p - 1, i - p + 1);
            --i;
            return true;
        }
        --i;
        return false;
    }
    
    bool store16(wstring& u, wstring& res, long& i) {
        std::unordered_map<wchar_t, Emoji_Arc*>::iterator it = utf16_arcs.find(u[i]);
        if (it == utf16_arcs.end())
            return false;
        long p = ++i;
        if (it->second->scan(u, p, i)) {
            res += u.substr(p - 1, i - p + 1);
            --i;
            return true;
        }
        --i;
        return false;
    }
    
    //A bit of explanation: on Windows, wstring is implemented as a UTF16 sequence
    //While on other platforms, it is implemented as UTF32 sequences.
    //Hence, if you use the standard scan, get and store with wstring, you need
    //different implementation along the platforms for wstring.
#ifdef WIN32
    //On Windows, we use utf16_arcs
    bool isemoji(wstring& u) {
        if (!u.size())
            return false;
        std::unordered_map<wchar_t, Emoji_Arc*>::iterator it = utf16_arcs.find(u[0]);
        return (it != utf16_arcs.end() && it->second->check(u, 1));
    }
    
    bool scan(wstring& u, long& i) {
        std::unordered_map<wchar_t, Emoji_Arc*>::iterator it = utf16_arcs.find(u[i]);
        if (it == utf16_arcs.end())
            return false;
        long p = ++i;
        bool found = it->second->scan(u, p, i);
        --i;
        return found;
    }
    
    bool get(wstring& u, wstring& res, long& i) {
        std::unordered_map<wchar_t, Emoji_Arc*>::iterator it = utf16_arcs.find(u[i]);
        if (it == utf16_arcs.end())
            return false;
        long p = ++i;
        if (it->second->scan(u, p, i)) {
            res = u.substr(p - 1, i - p + 1);
            --i;
            return true;
        }
        --i;
        return false;
    }
    
    bool store(wstring& u, wstring& res, long& i) {
        std::unordered_map<wchar_t, Emoji_Arc*>::iterator it = utf16_arcs.find(u[i]);
        if (it == utf16_arcs.end())
            return false;
        long p = ++i;
        if (it->second->scan(u, p, i)) {
            res += u.substr(p - 1, i - p + 1);
            --i;
            return true;
        }
        --i;
        return false;
    }
#else
    //On other platforms, we use utf32_arcs
    bool isemoji(wstring& u) {
        if (!u.size())
            return false;
        std::unordered_map<char32_t, Emoji_Arc*>::iterator it = utf32_arcs.find(u[0]);
        return (it != utf32_arcs.end() && it->second->check(u, 1));
    }
    
    bool scan(wstring& u, long& i) {
        std::unordered_map<char32_t, Emoji_Arc*>::iterator it = utf32_arcs.find(u[i]);
        if (it == utf32_arcs.end())
            return false;
        long p = ++i;
        bool found = it->second->scan(u, p, i);
        --i;
        return found;
    }
    
    bool get(wstring& u, wstring& res, long& i) {
        std::unordered_map<char32_t, Emoji_Arc*>::iterator it = utf32_arcs.find(u[i]);
        if (it == utf32_arcs.end())
            return false;
        long p = ++i;
        if (it->second->scan(u, p, i)) {
            res = u.substr(p - 1, i - p + 1);
            --i;
            return true;
        }
        --i;
        return false;
    }
    
    bool store(wstring& u, wstring& res, long& i) {
        std::unordered_map<char32_t, Emoji_Arc*>::iterator it = utf32_arcs.find(u[i]);
        if (it == utf32_arcs.end())
            return false;
        long p = ++i;
        if (it->second->scan(u, p, i)) {
            res += u.substr(p - 1, i - p + 1);
            --i;
            return true;
        }
        --i;
        return false;
    }
    
#endif
};

class UTF8_Handler {
public:
    
    Emojis emojis_characters;

    hmap<string, TAMGUCHAR> htmlcodes;
    hmap<wstring, TAMGUCHAR> whtmlcodes;
    hmap<string, TAMGUCHAR> xmlcodes;
    hmap<string, string> metacodes;
    hmap<wstring, wstring> wmetacodes;
    hmap<string, TAMGUCHAR> utf8_to_dos;

    basebinn_hash<string> dos_to_utf8;
    basebinn_hash<string> codeshtml;
    basebinn_hash<string> codesxml;
    basebinn_hash<string> vowels;
    basebinn_hash<string> consonants;
    basebinn_hash<string> latinmin;
    basebinn_hash<string> latinmaj;

    basebinn_hash<string> transliteration;
    basebinn_hash<wstring> hangulutf8;
    basebinn_hash<wstring> hanguljamo;
    basebin_hash<TAMGUCHAR> hangulconvert;
    hmap<wstring, wstring> hangultojamo;

    basebin_hash<TAMGUCHAR> dos2unicode;
    basebin_hash<TAMGUCHAR> unicode2dos;


    hash_short<bool> punctuations;
    hash_short<TAMGUCHAR> utf8codemin;

    basebin_hash<TAMGUCHAR> utf8codemaj;

    basebin_hash<TAMGUCHAR> wvowels;
    basebin_hash<TAMGUCHAR> wconsonants;

    basebin_hash<TAMGUCHAR> latincodes;
    basebin_hash<TAMGUCHAR> latin2codes;
    basebin_hash<TAMGUCHAR> latin3codes;
    basebin_hash<TAMGUCHAR> latin4codes;
    basebin_hash<TAMGUCHAR> latin5codes;
    basebin_hash<TAMGUCHAR> latin6codes;
    basebin_hash<TAMGUCHAR> latin7codes;
    basebin_hash<TAMGUCHAR> latin8codes;
    basebin_hash<TAMGUCHAR> latin9codes;
    basebin_hash<TAMGUCHAR> latin10codes;
    basebin_hash<TAMGUCHAR> latin11codes;
    basebin_hash<TAMGUCHAR> latin12codes;
    basebin_hash<TAMGUCHAR> latin13codes;
    basebin_hash<TAMGUCHAR> latin14codes;
    basebin_hash<TAMGUCHAR> latin15codes;
    basebin_hash<TAMGUCHAR> latin16codes;
    basebin_hash<TAMGUCHAR> latinwindowscodes;

    basebin_hash<TAMGUCHAR> codeslatin;

    basebin_hash<basebin_hash<TAMGUCHAR>* > alllatincodes;

    hmap<string, TAMGUCHAR> utf8codes;
    char invertnumbers[1000][5];
    bin_hash<string> numbers;

    bool scan_emoji(unsigned char* u, long& i);
    bool get_emoji(unsigned char* u, string& res, long& i);
    bool store_emoji(unsigned char* u, string& res, long& i);

    bool scan_emoji(string& u, long& i);
    bool get_emoji(string& u, string& res, long& i);
    bool store_emoji(string& u, string& res, long& i);

    bool scan_emoji(wstring& u, long& i);
    bool get_emoji(wstring& u, wstring& res, long& i);
    bool store_emoji(wstring& u, wstring& res, long& i);

    bool scan_emoji16(wstring& u, long& i);
    bool get_emoji16(wstring& u, wstring& res, long& i);
    bool store_emoji16(wstring& u, wstring& res, long& i);

    bool scan_emoji(u32string& u, long& i);
    bool get_emoji(u32string& u, u32string& res, long& i);
    bool store_emoji(u32string& u, u32string& res, long& i);

    bool c_is_emojicomp(TAMGUCHAR c);
    bool c_is_emoji(TAMGUCHAR c);
    bool u_is_emoji(u32string& s);
    bool s_is_emoji(wstring& s);
    bool s_is_emoji(string& s);
    bool c_is_emojicomp(unsigned char* m, long& i);
    bool c_is_emoji(unsigned char* m, long& i);

    void InitDosCodes();
    void InitHtmlCodes();
    void InitLatinTables();
    void InitKoreanCodes();
    void InitNumbers();
    
    bool valid_latin_table(short tableindex) {
        if (!alllatincodes.check(tableindex))
            return false;
        return true;
    }

    void convertnumber(long v, string& s);
    void convertnumber(double v, string& s);
    void convertnumber(long v, wstring& s);
    void convertnumber(double v, wstring& s);

    string convertfromnumber(BLONG l);
    wstring wconvertfromnumber(BLONG l);

    string convertfromnumber(long l);
    wstring wconvertfromnumber(long l);

    wstring wconvertfromnumber(short l);
    string convertfromnumber(short l);


    string convertfromnumber(double l);
    string convertfromnumber(float l);
    wstring wconvertfromnumber(double l);
    wstring wconvertfromnumber(float l);

    void c_HTMLcode(string& s, TAMGUCHAR code);
    void c_XMLcode(string& s, TAMGUCHAR code);
    void s_EvaluateMetaCharacters(string& s);
    void s_EvaluateMetaCharacters(wstring& s);
    
    TAMGUCHAR c_latin_table_to_unicode(int tableindex, TAMGUCHAR c);
    uchar c_latin_to_utf8(unsigned char code, unsigned char* utf);
    uchar c_latin_to_utf8(unsigned char* utf, unsigned char* res, bool checktable = false);
    TAMGUCHAR c_unicode_to_latin(TAMGUCHAR u);
    string s_utf8_to_latin(unsigned char* contenu, long sz);
    string conversion_unicode_to_latin(wstring& w);
    string conversion_latin_table_to_utf8(short tableindex, unsigned char* contenu, long sz);
    wstring conversion_latin_table_to_unicode(short tableindex, unsigned char* contenu, long sz);
    string s_unicode_to_dos(wstring& contenu);
    string s_utf8_to_dos(unsigned char* contenu);
    string s_dos_to_utf8(unsigned char* contenu);
    string s_utf8_to_dos(const char* contenu);
    string s_dos_to_utf8(const char* contenu);
    void s_latin_to_utf8(string& res, unsigned char* contenu, long sz);
    string conversion_latin_to_utf8(unsigned char* contenu, long sz);
    string conversion_utf8_to_latin(string& contenu);
    string conversion_latin_to_utf8(string& contenu);
    
    char c_is_alpha(unsigned char* m, long& i);
    bool s_is_punctuation(wstring& str);
    
    bool c_is_punctuation(TAMGUCHAR code);
    bool c_is_separator(TAMGUCHAR code);
    bool s_is_punctuation(string& str);
    bool s_is_upper(string& s);
    bool s_is_alpha(wstring& s);
    bool s_is_upper(wstring& s);
    bool s_is_lower(wstring& s);
    bool c_is_consonant(TAMGUCHAR c);
    bool c_is_vowel(TAMGUCHAR c);
    bool s_is_consonant(wstring& s);
    bool s_is_vowel(wstring& s);
    bool s_is_consonant(unsigned char* s);
    bool s_is_vowel(unsigned char* s);
    void s_binary(unsigned char* s, int mx, long lg, TAMGUCHAR* codes);
    bool compare_vowel(TAMGUCHAR c, TAMGUCHAR cc);
    wstring s_deaccentuate(wstring& s);
    string s_deaccentuate(unsigned char* s);
    
    bool s_is_alpha(string& s);
    bool s_is_lower(string& s);
    TAMGUCHAR c_to_lower(TAMGUCHAR c);
    TAMGUCHAR c_to_upper(TAMGUCHAR c);
    bool b_alpha(TAMGUCHAR c);
    char c_is_alpha(TAMGUCHAR code);
    bool c_is_lower(TAMGUCHAR code);
    bool c_is_upper(TAMGUCHAR code);
    wstring s_to_lower(wstring& s);
    wstring s_to_upper(wstring& s);
    string s_to_lower(string& s);
    string s_to_upper(string& s);
    void c_to_lower(string& res, unsigned char* s, long lg);
    void c_to_upper(string& res, unsigned char* s, long lg);
    
    bool c_is_hangul(string& s);
    bool s_is_hangul(wstring& w);
    bool c_is_hangul(TAMGUCHAR c);
    string c_translate(TAMGUCHAR c);
    string c_translate(unsigned char* m, long& i);
    bool c_is_hangul(unsigned char* m, long& i);
    bool s_is_jamo(wstring& w);
    wstring s_split_jamo(wstring& w);
    wstring s_combine_jamo(wstring& w);
    wstring s_hangul_normalize(wstring& w);
    
    UTF8_Handler();
    
};

#endif


