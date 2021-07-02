/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguustring.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "codeparse.h"
#include "tamgu.h"
#include "tamgutaskell.h"
#include "tamgumapui.h"
#include "tamguustring.h"
#include "constobjects.h"
#include "tamguivector.h"
#include "tamguuvector.h"
#include "tamgubyte.h"
#include "tamgubvector.h"
#include "tamgulvector.h"
#include "tamgufvector.h"
#include "tamgusvector.h"
#include "fractalhmap.h"
#include "instructions.h"
#include "tamgumap.h"
#include "compilecode.h"
#include "tamguautomaton.h"
#include "tamguufile.h"

#include "x_tokenize.h"
//------------------------------------------------------------------------------------------------------------------------
//We need to declare once again our local definitions.
Exporting basebin_hash<ustringMethod>  Tamguustring::methods;
Exporting hmap<string, string> Tamguustring::infomethods;
Exporting basebin_hash<unsigned long> Tamguustring::exported;
Exporting short Tamguustring::idtype = 0;

#ifdef UNIX
#define swprintf_s swprintf
#endif


#ifdef DOSOUTPUT
static bool dosoutput = true;
static void setdosoutput(bool d) { dosoutput = d; }
#define conversion2Dos(x) dosoutput?s_utf8_to_dos(USTR(x)):x
#endif


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguustring::AddMethod(TamguGlobal* global, string name, ustringMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamguustring::Setidtype(TamguGlobal* global) {
    Tamguustring::InitialisationModule(global,"");
}


bool Tamguustring::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();
    
    Tamguustring::idtype = global->Getid("ustring");
    
    Tamguustring::AddMethod(global, "succ", &Tamguustring::MethodSucc, P_NONE, "succ(): Return the successor of a character.");
    Tamguustring::AddMethod(global, "pred", &Tamguustring::MethodPred, P_NONE, "pred(): Return the predecessor of a byte.");
    
    
    Tamguustring::AddMethod(global, "doublemetaphone", &Tamguustring::MethodDoubleMetaphone, P_NONE, "doublemetaphone(): Return the string double metaphone conversion into a svector");
    
    Tamguustring::AddMethod(global, "ord", &Tamguustring::MethodOrd, P_NONE, "ord(): Return the character unicode.");
    Tamguustring::AddMethod(global, "utf16", &Tamguustring::MethodUTF16, P_NONE, "utf16(): Return the character unicode as UTF16 encoding.");
    Tamguustring::AddMethod(global, "bytes", &Tamguustring::MethodBytes, P_NONE, "bytes(): Return the Unicode representation.");
    Tamguustring::AddMethod(global, "hash", &Tamguustring::MethodHash, P_NONE, "hash(): Return the hash value of a string.");
    Tamguustring::AddMethod(global, "reverse", &Tamguustring::MethodReverse, P_NONE, "reverse(): reverse the string");
    Tamguustring::AddMethod(global, "format", &Tamguustring::MethodFormat, P_ATLEASTONE, "format(p1,p2,p3): Create a new string from the current string in which each '%x' is associated to one of the parameters, 'x' being the position of that parameter in the argument list. 'x' starts at 1.");
    Tamguustring::AddMethod(global, "base", &Tamguustring::MethodBase, P_ONE | P_TWO, "base(int b, bool toconvert=true): Return the value corresponding to the string in base b");
    Tamguustring::AddMethod(global, "fill", &Tamguustring::MethodFill, P_TWO, "fill(int nb,string c): create a string of nb characters c");
    Tamguustring::AddMethod(global, "padding", &Tamguustring::MethodPadding, P_TWO | P_THREE, "padding(int nb,string c,bool paddattheend): add nb characters c to the current string. Last parameter is optional");
    Tamguustring::AddMethod(global, "parse", &Tamguustring::MethodParse, P_NONE | P_TWO, "parse(): Parse a string as a piece of code and returns the evaluation as a vector.");
    Tamguustring::AddMethod(global, "pop", &Tamguustring::MethodPop, P_NONE | P_ONE | P_TWO, "pop(): remove last character");
    Tamguustring::AddMethod(global, "sizeb", &Tamguustring::MethodSizeb, P_NONE, "sizeb(): Return the size in bytes of the string");
    Tamguustring::AddMethod(global, "parenthetics", &Tamguustring::MethodParenthetic, P_NONE | P_TWO | P_THREE | P_FOUR | P_FIVE | P_SIX, "lisp(): lisp(string o,string c,bool comma,bool separator,bool concatenate): Parse a string as a parenthetic expressions, o is '(' and c is ')' by default. If 'comma' is true, then the decimal character is ',' otherwise it is '.'. If 'separator' is true then '1,000' is accepted as a number. If 'concatenate' is true then '3a' is a valid token");
    Tamguustring::AddMethod(global, "tags", &Tamguustring::MethodTags, P_TWO | P_THREE | P_FOUR | P_FIVE| P_SIX, "tags(string o,string c,bool comma,bool separator,bool concatenate, svector rules): Parse a string as a parenthetic expressions, where o and c are string (not characters). If 'comma' is true, then the decimal character is ',' otherwise it is '.'. If 'separator' is true then '1,000' is accepted as a number. If 'concatenate' is true then '3a' is a valid token");
    Tamguustring::AddMethod(global, "scan", &Tamguustring::MethodScan, P_ONE | P_TWO | P_THREE | P_FOUR, "scan(sub, string sep, bool immediate,string remaining): Find the substrings matching sub, with TRE. 'sep' is a separator between strings. 'immediate' always combines with a separator, it means that the matching should start at the first character of the string, default is false. 'remaining' also combines with 'separator', it returns the rest of the string after the section that matched.");
    Tamguustring::AddMethod(global, "evaluate", &Tamguustring::MethodEvaluate, P_NONE, "evaluate(): evaluate the meta-characters within a string and return the evaluated string.");
    Tamguustring::AddMethod(global, "html", &Tamguustring::MethodTohtml, P_NONE, "html(): Return the string into an HTML compatible string or as a vector of strings");
    Tamguustring::AddMethod(global, "tohtml", &Tamguustring::MethodTohtml, P_NONE, "tohtml(): Return the string into an HTML compatible string or as a vector of strings");
    Tamguustring::AddMethod(global, "toxml", &Tamguustring::MethodToxml, P_NONE, "toxml(): Return the string into an XML compatible string or as a vector of strings");
    Tamguustring::AddMethod(global, "levenshtein", &Tamguustring::MethodEditdistance, P_ONE | P_TWO, "levenshtein(string s,bool byte): Return the edit distance with 's' according to Levenshtein algorithm. If byte is true, force a byte level comparison. byte is optionnal.");
    Tamguustring::AddMethod(global, "editdistance", &Tamguustring::MethodEditdistance, P_ONE | P_TWO | P_THREE, "editdistance(string s,bool byte): Return the edit distance with 's'. If byte is true, force a byte level comparison. byte is optionnal.");
    Tamguustring::AddMethod(global, "replace", &Tamguustring::MethodReplace, P_TWO, "replace(string sub,string str): Replace the substrings matching sub with str");
    Tamguustring::AddMethod(global, "multisplit", &Tamguustring::MethodMultiSplit, P_ATLEASTONE, "multisplit(string splitter1,string splitter2..): split a string along different splitters. Return a uvector");
    Tamguustring::AddMethod(global, "split", &Tamguustring::MethodSplit, P_ONE | P_NONE, "split(string splitter): split a string along splitter and store the results  in a vector. If splitter=='', then the string is split into a vector of characters");
    Tamguustring::AddMethod(global, "splite", &Tamguustring::MethodSplite, P_ONE | P_NONE, "splite(string splitter): split a string along splitter and store the results  in a vector. If splitter=='', then the string is split into a vector of characters. Empty strings are kept in the result.");
    Tamguustring::AddMethod(global, "tokenize", &Tamguustring::MethodTokenize, P_NONE | P_ONE | P_TWO | P_THREE, "tokenize(bool comma,bool separator, svector rules): Segment a string into words and punctuations. If 'comma' is true, then the decimal character is ',' otherwise it is '.'. If 'separator' is true then '1,000' is accepted as a number. rules is a set of tokenization rules that can be first initialized then modified with _getdefaulttokenizerules");
    Tamguustring::AddMethod(global, "stokenize", &Tamguustring::MethodStokenize, P_NONE | P_ONE, "stokenize(map keeps): Segment a string into words and punctuations, with a keep.");
    Tamguustring::AddMethod(global, "count", &Tamguustring::MethodCount, P_TWO | P_ONE, "count(string sub,int pos): Count the number of substrings starting at position pos");
    Tamguustring::AddMethod(global, "bpe", &Tamguustring::MethodBPE, P_THREE | P_TWO, "bpe(long threshold, long nb, mapui): byte pair encoding");
    Tamguustring::AddMethod(global, "bpereplace", &Tamguustring::MethodBPEReplace, P_TWO, "bpereplace(long nb, mapui): byte pair encoding replacement in a string");
    Tamguustring::AddMethod(global, "find", &Tamguustring::MethodFind, P_TWO | P_ONE, "find(string sub,int pos): Return the position of substring sub starting at position pos");
    Tamguustring::AddMethod(global, "rfind", &Tamguustring::MethodRfind, P_TWO | P_ONE, "rfind(string sub,int pos): Return the position of substring sub backward starting at position pos");
    Tamguustring::AddMethod(global, "removefirst", &Tamguustring::MethodRemovefirst, P_ONE, "removefirst(int nb): remove the first nb characters of a string");
    Tamguustring::AddMethod(global, "removelast", &Tamguustring::MethodRemovelast, P_ONE, "removelast(int nb): remove the last nb characters of a string");
    Tamguustring::AddMethod(global, "isutf8", &Tamguustring::MethodIsutf8, P_NONE, "isutf8(): Return true is the string is encoded in UTF8");
    Tamguustring::AddMethod(global, "slice", &Tamguustring::MethodSlice, P_ONE, "slice(int sz): Return a vector of all slices of size sz");
    Tamguustring::AddMethod(global, "ngrams", &Tamguustring::MethodNgrams, P_ONE, "ngrams(int r): Return a vector of all ngrams of rank r");
    Tamguustring::AddMethod(global, "latin", &Tamguustring::MethodLatin, P_NONE, "latin(): convert an UTF8 string in LATIN");
    Tamguustring::AddMethod(global, "dos", &Tamguustring::MethodDos, P_NONE, "dos(): convert a string into DOS encoding");
    Tamguustring::AddMethod(global, "dostoutf8", &Tamguustring::MethodDostoutf8, P_NONE, "dostoutf8(): convert a DOS string into UTF8");
    Tamguustring::AddMethod(global, "left", &Tamguustring::MethodLeft, P_ONE, "left(int nb): return the first nb characters of a string");
    Tamguustring::AddMethod(global, "right", &Tamguustring::MethodRight, P_ONE, "right(int nb): return the last nb characters of a string");
    Tamguustring::AddMethod(global, "mid", &Tamguustring::MethodMid, P_TWO, "mid(int pos,int nb): return the nb characters starting at position pos of a string");
    Tamguustring::AddMethod(global, "isalpha", &Tamguustring::MethodIsalpha, P_NONE, "isalpha(): Test if a string only contains only alphabetical characters");
    Tamguustring::AddMethod(global, "isconsonant", &Tamguustring::MethodIsconsonant, P_NONE, "isconsonant(): Test if a string only contains consonants");
    Tamguustring::AddMethod(global, "isvowel", &Tamguustring::MethodIsvowel, P_NONE, "isvowel(): Test if a string only contains only vowels");
    Tamguustring::AddMethod(global, "ispunctuation", &Tamguustring::MethodIspunctuation, P_NONE, "ispunctuation(): Test if the character is a punctuation");
    Tamguustring::AddMethod(global, "isdigit", &Tamguustring::MethodIsdigit, P_NONE, "isdigit(): Test if a string only contains digits");
    Tamguustring::AddMethod(global, "extract", &Tamguustring::MethodExtract, P_ATLEASTTHREE, "extract(int pos,string from,string up1,string up2...): extract substrings between 'from' and 'up1'...'upn' (the shortest string is used). Return a vector of strings");
    Tamguustring::AddMethod(global, "isemoji", &Tamguustring::MethodIsemoji, P_NONE, "isemoji(): Test if a string only contains emoji characters");
    Tamguustring::AddMethod(global, "emoji", &Tamguustring::MethodEmoji, P_NONE, "emoji(): Return the textual description of an emoji");
    Tamguustring::AddMethod(global, "isupper", &Tamguustring::MethodIsupper, P_NONE, "isupper(): Test if a string only contains uppercase characters");
    Tamguustring::AddMethod(global, "islower", &Tamguustring::MethodIslower, P_NONE, "islower(): Test if a string only contains lowercase characters");
    Tamguustring::AddMethod(global, "upper", &Tamguustring::MethodUpper, P_NONE, "upper(): Return the string in upper characters");
    Tamguustring::AddMethod(global, "deaccentuate", &Tamguustring::MethodDeaccentuate, P_NONE, "deaccentuate(): Remove the accents from accented characters");
    Tamguustring::AddMethod(global, "lower", &Tamguustring::MethodLower, P_NONE, "lower(): Return the string in lower characters");
    Tamguustring::AddMethod(global, "trim", &Tamguustring::MethodTrim, P_NONE, "trim(): remove the trailing characters");
    Tamguustring::AddMethod(global, "trimleft", &Tamguustring::MethodTrimleft, P_NONE, "trimleft(): remove the trailing characters on the left");
    Tamguustring::AddMethod(global, "trimright", &Tamguustring::MethodTrimright, P_NONE, "trimright(): remove the trailing characters on the right");
    Tamguustring::AddMethod(global, "last", &Tamguustring::MethodLast, P_NONE, "last(): return last character");
    Tamguustring::AddMethod(global, "insert", &Tamguustring::MethodInsert, P_ONE | P_TWO, "insert(int i,string s): insert the string s at i. If i is -1, then insert s between each character in the input string");
    Tamguustring::AddMethod(global, "clear", &Tamguustring::MethodClear, P_NONE, "clear(): Clean the content of a string.");
    
    Tamguustring::AddMethod(global, "jamo", &Tamguustring::MethodJamo, P_NONE | P_ONE, "jamo(bool combine): if 'combine' is false split a Korean jamo into its main components, else combine contents into a jamo.");
    Tamguustring::AddMethod(global, "isjamo", &Tamguustring::MethodIsJamo, P_NONE, "isjamo(): return true if it is a Hangul jamo.");
    Tamguustring::AddMethod(global, "ishangul", &Tamguustring::MethodIsHangul, P_NONE, "ishangul(): return true if it is a Hangul character.");
    Tamguustring::AddMethod(global, "normalizehangul", &Tamguustring::MethodNormalizeHangul, P_NONE, "normalizehangul(): Normalize Hangul characters.");
    Tamguustring::AddMethod(global, "romanization", &Tamguustring::MethodTransliteration, P_NONE, "romanization(): romanization of Hangul characters.");
    
    Tamguustring::AddMethod(global, "read", &Tamguustring::MethodRead, P_ONE, "read(string path): read the file content into the current variable. File shoud be encoded in UTF16 characters");
    Tamguustring::AddMethod(global, "write", &Tamguustring::MethodWrite, P_ONE, "write(string path): write the string content into a file.");
    
    if (version != "") {
        global->newInstance[Tamguustring::idtype] = new Tamguustring(L"", global);
        global->newInstance[a_ustringthrough] = global->newInstance[Tamguustring::idtype];
        global->RecordMethods(Tamguustring::idtype, Tamguustring::exported);
        global->RecordMethods(a_ustringthrough, Tamguustring::exported);
        global->RecordMethods(a_uloop, Tamguustring::exported);
    }
    
    Tamgua_ustring::InitialisationModule(global, version);
    
    return true;
}

TamguIteration* Tamguustring::Newiteration(bool direction) {
    Locking _lock(this);
    return new TamguIterationustring(value, direction);
}

Tamgu* Tamguustring::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    wstring val = UString();
    long sz = val.size();

    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    Tamguustring v("");
    Tamgu* a = aNULL;
    bool testcond = false;
    for (long i = 0; i < sz && !testcond; i++) {
        a->Releasenonconst();
        v.value =  getfullchar(val,i);
		var->Putvalue(&v, idthread);
        
        a = ins->instructions.vecteur[1]->Eval(context, aNULL, idthread);
        
            //Continue does not trigger needInvestigate
        testcond = a->needInvestigate();
    }

    if (testcond) {
        if (a == aBREAK)
            return this;
        else
            return a;
    }
    
    a->Releasenonconst();
    return this;
}


Tamgu* Tamguustring::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    wstring val = a->UString();

    locking();

    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Locking* _vlock = _getlock(v);
        s_findall(value, val, v->values);
        _cleanlock(_vlock);
        unlocking();
        return v;
    }

    long r = s_find(value, val, 0);
    
    unlocking();

    if (context->isString()) {
        if (r==-1)
            return aEMPTYUSTRING;
        return globalTamgu->Providewithustring(val);
    }

    if (context->isNumber())
        return globalTamgu->Provideint(r);
    
    if (r == -1)
        return aFALSE;
    return aTRUE;
}

Tamgu* TamguConstUString::in(Tamgu* context, Tamgu* a, short idthread) {
        //Three cases along the container type...
        //It is a Boolean, we expect false or true
        //It is an integer, we expect a position in v
        //It is a container, we are looking for all positions...
    
    wstring val = a->UString();
    
    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Locking* _vlock = _getlock(v);
        s_findall(value, val, v->values);
        _cleanlock(_vlock);
        return v;
    }
    
    long r = s_find(value, val, 0);
    
    if (context->isString()) {
        if (r==-1)
            return aEMPTYUSTRING;
        return globalTamgu->Providewithustring(val);
    }
    
    if (context->isNumber())
        return globalTamgu->Provideint(r);
    
    if (r == -1)
        return aFALSE;
    return aTRUE;
}

unsigned long Tamguustring::EditDistance(Tamgu* e) {
    Locking _lock(this);
    wstring s2 = e->UString();
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



#ifdef WSTRING_IS_UTF16
Tamgu* Tamguustring::MethodOrd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring s = UString();
	uint32_t c;
    if (s.size() >= 1) {
        if (contextualpattern->isVectorContainer() || s.size()>1) {
            Tamgu* kvect=SelectContainer(contextualpattern,idthread);
            if (kvect==NULL)
                kvect=new Tamgulvector;
                
            Locking _lock((TamguObject*)kvect);
			for (size_t i = 0; i < s.size(); i++) {
				if (c_utf16_to_unicode(c, s[i], false))
					c_utf16_to_unicode(c, s[++i], true);
				kvect->storevalue((BLONG)c);
			}
            return kvect;
        }
        
		if (c_utf16_to_unicode(c, s[0], false))
			c_utf16_to_unicode(c, s[1], true);

        if (contextualpattern->isNumber()) {
            contextualpattern->storevalue((BLONG)c);
            return contextualpattern;
        }

        return globalTamgu->Providelong(c);
    }
    
    return aNULL;
}

Tamgu* Tamguustring::MethodBytes(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	wstring s = UString();
	if (s.size() > 0) {
        if (contextualpattern->isNumber()) {
            uint32_t c;
            if (c_utf16_to_unicode(c, s[0], false))
                c_utf16_to_unicode(c, s[1], true);
            
            return globalTamgu->Providelong((BLONG)c);
        }
        
        Tamgulvector* kvect=(Tamgulvector*)Selectalvector(contextualpattern);
        kvect->locking();
        for (size_t i = 0; i < s.size(); i++)
            kvect->storevalue((BLONG)s[i]);
        kvect->unlocking();
        return kvect;
    }

	return aNULL;
}

Tamgu* Tamguustring::MethodUTF16(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring s = UString();
    uint32_t code;
    uint32_t subcode;
    Tamgu* kvect=SelectContainer(contextualpattern,idthread);
    if (kvect==NULL)
        kvect=new Tamgulvector;
    
    Locking _lock((TamguObject*)kvect);
    for (size_t i = 0; i < s.size(); i++) {
        kvect->storevalue((BLONG)s[i]);
    }
    return kvect;
}

#else
Tamgu* Tamguustring::MethodOrd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	wstring s = UString();
	if (s.size() >= 1) {
		if (contextualpattern->isVectorContainer() || s.size()>1) {
			Tamgu* kvect = SelectContainer(contextualpattern, idthread);
			if (kvect == NULL)
				kvect = new Tamgulvector;

			Locking _lock((TamguObject*)kvect);
			for (size_t i = 0; i < s.size(); i++)
				kvect->storevalue(s[i]);
			return kvect;
		}

		if (contextualpattern->isNumber()) {
			contextualpattern->storevalue((BLONG)s[0]);
			return contextualpattern;
		}

		return globalTamgu->Providelong(s[0]);
	}

	return aNULL;
}

Tamgu* Tamguustring::MethodUTF16(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring s = UString();
    uint32_t code;
    uint32_t subcode;
    Tamgu* kvect=SelectContainer(contextualpattern,idthread);
    if (kvect==NULL)
        kvect=new Tamgulvector;
    
    Locking _lock((TamguObject*)kvect);
    for (size_t i = 0; i < s.size(); i++) {
        c_unicode_to_utf16(code, s[i]);
        if (!code)
            kvect->storevalue((BLONG)code);
        else {
            subcode = code >> 16;
            code &= 0xFFFF;
            if (subcode)
                kvect->storevalue((BLONG)subcode);
            kvect->storevalue((BLONG)code);
        }
    }
    return kvect;
}

Tamgu* Tamguustring::MethodBytes(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	wstring s = UString();
	if (s.size() > 0) {
        if (contextualpattern->isNumber()) {
            uint32_t c;
            if (c_utf16_to_unicode(c, s[0], false))
                c_utf16_to_unicode(c, s[1], true);

            return globalTamgu->Providelong(c);
        }
        
        Tamgulvector* kvect=(Tamgulvector*)Selectalvector(contextualpattern);
        kvect->locking();
        for (size_t i = 0; i < s.size(); i++)
            kvect->storevalue((BLONG)s[i]);
        kvect->unlocking();
        return kvect;
    }
    
	return aNULL;
}

#endif


Tamgu* Tamguustring::Put(Tamgu* idx, Tamgu* v, short idthread) {

    if (!idx->isIndex()) {
        if (v == this)
            return aFALSE;
        
        locking();
        v->Setstring(value, idthread);
        unlocking();
        return aTRUE;
    }

    long ileft, iright;
    locking();
    char res = StringIndexes(value, idx, ileft, iright, idthread);
    
    if (res == 0) {
        unlocking();
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong key in a container or a string access", idthread);
        return aFALSE;
    }

    if (res == 1) {
        iright = ileft;
        iright = c_char_next(USTR(value), iright);
    }
    else
        iright = iright - ileft;
    value.erase(ileft, iright);
    
    wstring s;
    if (v != aNULL)
        s = v->UString();

    if (s != L"")
        value.insert(ileft, s);    
    unlocking();
    return aTRUE;
}

Tamgu* Tamguustring::Eval(Tamgu* context, Tamgu* idx, short idthread) {
    if (!idx->isIndex() || context == idx)
        return this;

    long ileft, iright;
    locking();
    char res = StringIndexes(value, idx, ileft, iright, idthread);

    if (res == 0) {
        unlocking();
       if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong key in a container or a string access", idthread);
        return aNOELEMENT;
    }

    if (res == 1) {
        Tamguustring* s = globalTamgu->Provideustring(L"");
#ifdef WSTRING_IS_UTF16
		uint32_t c = value[ileft];
		s->value = c;
		if (checklargeutf16(c))
			s->value += value[ileft + 1];
#else
        s->value = value[ileft];
#endif
        unlocking();
        return s;
    }

    idx = globalTamgu->Provideustring(value.substr(ileft, iright - ileft));
    unlocking();
    return idx;
}

void bytepairencoding(wstring& str, long nb, hmap<wstring, long>& dicos);
void bytepairreplace(wstring& str, wstring& res, long nbmax, hmap<wstring, long>& dicos);

Tamgu* Tamguustring::MethodBPE(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    hmap<wstring, long> values;

    long threshold = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    long nb = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    
    if (callfunc->Size() == 3) {
        Tamgu* e = callfunc->Evaluate(2, contextualpattern, idthread);
        if (e->Type() != Tamgumapui::idtype)
            return globalTamgu->Returnerror("Expecting a mapui as argument", idthread);
        values = ((Tamgumapui*)e)->values;
    }

    wstring str = UString();
    
    bytepairencoding(str,nb, values);

    if (contextualpattern->isMapContainer()) {
        wstring r;
        contextualpattern->Clear();
        if (contextualpattern->Type() == Tamgumapui::idtype) {
            for (auto& key : values) {
                if (key.second >= threshold) {
                    ((Tamgumapui*)contextualpattern)->values[key.first] = key.second;
                }
            }
        }
        else {
            Tamguustring us("");
            us.Setreference();
            Tamguint tint(0);
            tint.Setreference();
            for (auto& key : values) {
                if (key.second >= threshold) {
                    tint.value =  key.second;
                    contextualpattern->Push(key.first, &tint);
                }
            }
        }
    }
    else {
        contextualpattern = new Tamgumapui;
        for (auto& key : values) {
            if (key.second >= threshold) {
                ((Tamgumapui*)contextualpattern)->values[key.first] = key.second;
            }
        }
    }
    
    return contextualpattern;
}

Tamgu* Tamguustring::MethodBPEReplace(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    long nb = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    Tamgu* e = callfunc->Evaluate(1, contextualpattern, idthread);
    if (e->Type() != Tamgumapui::idtype)
        return globalTamgu->Returnerror("Expecting a mapui as argument", idthread);

    wstring str = UString();
    Tamguustring* res =  globalTamgu->Provideustring();
    bytepairreplace(str, res->value, nb, ((Tamgumapui*)e)->values);
    return res;
}

Tamgu* Tamguustring::MethodCount(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long i = 0;

    if (callfunc->Size() == 2) {
        i = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    }

    Tamgu* substr = callfunc->Evaluate(0, contextualpattern, idthread);
    
    if (substr->isRegular()) {
        wstring w = UString();
        if (i)
            w = w.substr(i, w.size() - i);
        
        vector<long> values;
        substr->searchall(w,values);
        return globalTamgu->Provideint(values.size()>>1);
    }

    wstring sub = substr->UString();

    locking();
    if (value == L"") {
        unlocking();
        return aZERO;
    }
    
    i = s_count(value, sub, i);
    unlocking();
    
    return globalTamgu->Provideint(i);
}


Tamgu* Tamguustring::MethodFind(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long sz;
    locking();
    if (value==L"") {
        unlocking();
        if (contextualpattern->isVectorContainer())
            return Selectavector(contextualpattern);
        return aMINUSONE;
    }
    else
        sz = value.size();
    unlocking();
    
    Tamgu* ksub = callfunc->Evaluate(0, contextualpattern, idthread);
    
    //we search for a substring starting at position idx
    long i = 0;
    if (callfunc->Size() == 2) {
        i = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
        if (i>=sz) {
            if (contextualpattern->isVectorContainer())
                return Selectavector(contextualpattern);
            return aMINUSONE;
        }
    }
        
    if (contextualpattern->isVectorContainer()) {
        Tamgu* kvect = Selectavector(contextualpattern);
        long j;
        
        vector<long> v;
        if (ksub->isRegular()) {
            locking();
            ksub->searchall(value,v,i);
            unlocking();
        }
        else {
            wstring sub = ksub->UString();
            locking();
            s_findall(value, sub, v, i);
            unlocking();
        }
        
        if (v.size()) {
            Locking _lock((TamguObject*)kvect);
            
            switch (kvect->Type()) {
                case a_bvector:
                    for (j = 0; j < v.size(); j++) {
                        if (v[j] >= i)
                            ((Tamgubvector*)kvect)->values.push_back((uchar)v[j]);
                    }
                    return kvect;
                case a_ivector:
                    for (j = 0; j < v.size(); j++) {
                        if (v[j] >= i)
                            ((Tamguivector*)kvect)->values.push_back((uchar)v[j]);
                    }
                    return kvect;
                case a_fvector:
                    for (j = 0; j < v.size(); j++) {
                        if (v[j] >= i)
                            ((Tamgufvector*)kvect)->values.push_back((double)v[j]);
                    }
                    return kvect;
            }
            
            TamguConstInt loc(0);
            for (j = 0; j < v.size(); j++) {
                if (v[j] >= i) {
                    loc.value = v[j];
                    kvect->Push(&loc);
                }
            }
        }
        
        return kvect;
    }
    
    if (ksub->isRegular()) {
        long e;
        locking();
        if (!ksub->search(value,i,e,i)) {
            unlocking();
            return aMINUSONE;
        }
        unlocking();
        return globalTamgu->Provideint(i);
    }
    
    wstring sub = ksub->UString();
    locking();
    i = s_find(value, sub, i);
    unlocking();
    
    if (i == string::npos)
        return aMINUSONE;
    
    return globalTamgu->Provideint(i);
}

Tamgu* Tamguustring::MethodRfind(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    locking();
    if (value==L"") {
        unlocking();
        if (contextualpattern->isVectorContainer())
            return Selectavector(contextualpattern);
        return aMINUSONE;
    }
    unlocking();

    Tamgu* ksub = callfunc->Evaluate(0, contextualpattern, idthread);
    
    //we search for a substring starting at position idx
    long i = -1;
    if (callfunc->Size() == 2) {
        i = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
        locking();
        i = convertpostochar(value,0, i);
        unlocking();
    }
    
    if (contextualpattern->isVectorContainer()) {
        if (i == -1)
            i = 0;
        
        Tamgu* kvect = Selectavector(contextualpattern);
        long j;
        
        vector<long> v;
        if (ksub->isRegular()) {
            locking();
            ksub->searchall(value,v,i);
            unlocking();
        }
        else {
            wstring sub = ksub->UString();
            locking();
            s_findall(value, sub, v, i);
            unlocking();
        }
        
        if (v.size()) {
            Locking _lock((TamguObject*)kvect);
            
            switch (kvect->Type()) {
                case a_bvector:
                    for (j = v.size()-1; j >=0 ; j--) {
                        if (v[j] < i)
                            break;
                        ((Tamgubvector*)kvect)->values.push_back((uchar)v[j]);
                    }
                    return kvect;
                case a_ivector:
                    for (j = v.size()-1; j >=0 ; j--) {
                        if (v[j] < i)
                            break;
                        ((Tamguivector*)kvect)->values.push_back((uchar)v[j]);
                    }
                    return kvect;
                case a_fvector:
                    for (j = v.size()-1; j >=0 ; j--) {
                        if (v[j] < i)
                            break;
                        ((Tamgufvector*)kvect)->values.push_back((double)v[j]);
                    }
                    return kvect;
            }
            
            TamguConstInt loc(0);
            for (j = v.size()-1; j >=0 ; j--) {
                if (v[j] < i)
                    break;
                loc.value = v[j];
                kvect->Push(&loc);
            }
        }
        
        return kvect;
    }
    
    if (ksub->isRegular()) {
        long e;
        locking();
        if (i == -1)
            i = value.size();
        if (!ksub->searchlast(value,i,e,i)) {
            unlocking();
            return aMINUSONE;
        }
        unlocking();
        return globalTamgu->Provideint(i);
    }
    
    wstring sub = ksub->UString();
    locking();
    i = s_rfind(value, sub, i);
    unlocking();
    
    if (i == string::npos)
        return aMINUSONE;
    
    return globalTamgu->Provideint(i);
}

Tamgu* Tamguustring::MethodDoubleMetaphone(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    
    string s = String();
    Tamgu* kvect = Selectasvector(contextualpattern);
    
    DoubleMetaphone(s,((Tamgusvector*)kvect)->values);
    return kvect;
}


Tamgu* Tamguustring::MethodTokenize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static x_wtokenize xr;
    static bool init = false;
    
    if (!init) {
        xr.juststack=true;
        xr.load();
        xr.keeprc(false);
        init = true;
    }

    short flag = 0;
    bool comma = false;
    bool separator = false;
        
    if (callfunc->Size() >= 1) {
        comma = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
        if (comma)
            flag |= token_comma;
        if (callfunc->Size() >= 2) {
            separator = callfunc->Evaluate(1, contextualpattern, idthread)->Boolean();
            if (separator)
                flag |= token_separator;
            if (callfunc->Size() == 3) {
                vector<string> rules;
                Tamgu* vect = callfunc->Evaluate(2, contextualpattern, idthread);
                for (long i = 0; i< vect->Size(); i++)
                    rules.push_back(vect->getstring(i));
                Tamgu* kvect = Selectauvector(contextualpattern);
                short flags = token_period;
                if (comma)
                    flags |= token_comma;
                if (separator)
                    flags |= token_separator;

                locking();
                Locking _vlock((TamguObject*)kvect);
                if (!vw_tokenize(((Tamguuvector*)kvect)->values, value, flag, rules)) {
                    wstring w = ((Tamguuvector*)kvect)->values[0];
                    string s;
                    sc_unicode_to_utf8(s, w);
                    kvect->Clear();
                    kvect->Release();
                    unlocking();
                    return globalTamgu->Returnerror(s,idthread);
                }
                unlocking();
                return kvect;
            }
        }
    }

    Tamgu* kvect = Selectauvector(contextualpattern);

    if (!globalTamgu->threadMODE) {
        if (comma)
            xr.selectcomma(true);
        else
            xr.selectcomma(false);
        
        if (token_separator)
            xr.separator(true);
        else
            xr.separator(false);
        
        xr.tokenize(value,false,&((Tamguuvector*)kvect)->values);
        return kvect;
    }
    
    x_wtokenize xwr;
    xwr.juststack=true;
    xwr.load();
    xwr.keeprc(false);
    if (comma)
        xwr.selectcomma(true);
    else
        xwr.selectcomma(false);
    
    if (token_separator)
        xwr.separator(true);
    else
        xwr.separator(false);

    locking();
    wstring w  = value;
    unlocking();
    
    Locking _vlock((TamguObject*)kvect);

    xwr.tokenize(w,false,&((Tamguuvector*)kvect)->values);

    return kvect;
}

Tamgu* Tamguustring::MethodStokenize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring thestr = UString();
    Tamgu* keeps = NULL;

    if (callfunc->Size() == 2) {
        keeps = callfunc->Evaluate(0, contextualpattern, idthread);
        if (!keeps->isContainer())
            return globalTamgu->Returnerror("TAMGUI(665): Expecting a container in STOKENIZE", idthread);
    }

    vector<wstring> v;
    map<wstring, bool> k;
    if (keeps != NULL) {
        TamguIteration* itr = keeps->Newiteration(false);
        for (itr->Begin(); itr->End() == aFALSE; itr->Next())
            k[itr->Valueustring()] = true;
        itr->Release();
    }
    s_tokenize(v, thestr, k);

    Tamgu* kvect = Selectauvector(contextualpattern);
    if (kvect->Type() == a_uvector) {
        Locking _lock((TamguObject*)kvect);
        ((Tamguuvector*)kvect)->values = v;
    }
    else {
        TamguConstUString localstring(L"");

        for (long i = 0; i < v.size(); i++) {
            localstring.value = v[i];
            kvect->Push(&localstring);
        }
    }
    return kvect;
}

Tamgu* Tamguustring::MethodJamo(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring w;
    Locking _lock(this);

    if (callfunc->Size() == 0 || callfunc->Evaluate(0, contextualpattern, idthread)->Boolean() == false) {
        w = s_split_jamo(value);
        if (contextualpattern->isVectorContainer()) {
            Tamguuvector* vs = (Tamguuvector*)Selectauvector(contextualpattern);
            Locking _vlock(vs);
            wstring s;
            for (long i = 0; i < w.size(); i++) {
                s = w[i];
                vs->values.push_back(s);
            }
            return vs;
        }

        return globalTamgu->Providewithustring(w);
    }
    //Else combine...
    w = s_combine_jamo(value);
    return globalTamgu->Providewithustring(w);
}

Tamgu* Tamguustring::MethodIsJamo(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    return booleantamgu[s_is_jamo(value)];
}

Tamgu* Tamguustring::MethodIsHangul(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    return booleantamgu[s_is_hangul(value)];
}

Tamgu* Tamguustring::MethodNormalizeHangul(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    return globalTamgu->Provideustring(s_hangul_normalize(value));

}

Tamgu* Tamguustring::MethodTransliteration(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string c;
    Locking _lock(this);

    if (contextualpattern->isVectorContainer()) {
        Tamgusvector* vs = (Tamgusvector*)Selectasvector(contextualpattern);
        Locking _vlock(vs);
        for (long i = 0; i < value.size(); i++) {
            c = c_translate(value[i]);
            vs->values.push_back(c);
        }
        return vs;
    }

    if (value.size())
        c = c_translate(value[0]);

    return globalTamgu->Providewithstring(c);
}


static bool comp(long s1, long s2) {
    if (s1<s2)
        return true;
    return false;
}

Tamgu* Tamguustring::MethodRead(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (globalTamgu->isthreading)
        return aFALSE;
    
    string filename = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    
    Tamguufile file;
    if (!file.openfile(filename)) {
        string msg = "Cannot open file: ";
        msg+=filename;
        return globalTamgu->Returnerror(msg,idthread);
    }

    Locking _lock(this);
    value=L"";
    file.readall(value);
    return this;
}

Tamgu* Tamguustring::MethodWrite(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (globalTamgu->isthreading)
        return aFALSE;
    
    string filename = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    
    Tamguufile file;
    if (!file.openfilewrite(filename)) {
        string msg = "Cannot open file: ";
        msg+=filename;
        return globalTamgu->Returnerror(msg,idthread);
    }

    Locking _lock(this);
    file.write(value);
    return this;
}


Tamgu* Tamguustring::MethodMultiSplit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    //Third parameter can be a vector

    Tamgu* kvect = NULL;
    wstring thesplitter;
    vector<long> ipositions;
    size_t pos = 0, found;
    long i = 0;
    //We first fill our vector with initial values...
    while (!pos && i < callfunc->Size()) {
        thesplitter = callfunc->Evaluate(i, contextualpattern, idthread)->UString();
        pos = 0;
        Locking _lock(this);
        while (pos != string::npos) {
            found = s_find(value, thesplitter, pos);
            if (found != string::npos) {
                ipositions.push_back(found);
                pos = found + thesplitter.size();
                ipositions.push_back(pos);
            }
            else
                break;
        }
        i++;
    }
    //we have three forbidden cases:
    //a) x'<x and y'>x
    //b) x'>x and y'<y
    //c) x'>x and y'<y


    for (; i < callfunc->Size(); i++) {
        thesplitter = callfunc->Evaluate(i, contextualpattern, idthread)->UString();
        pos = 0;
        Locking _lock(this);
        while (pos != string::npos) {
            found = s_find(value, thesplitter, pos);
            if (found != string::npos) {
                bool add = true;
                pos = found + thesplitter.size();
                for (int j = 0; j < ipositions.size(); j += 2) {
                    if (found <= ipositions[j] && pos >= ipositions[j]) {
                        add = false;
                        break;
                    }
                    if (found >= ipositions[j] && (pos <= ipositions[j + 1] || found <= ipositions[j + 1])) {
                        add = false;
                        break;
                    }
                }

                if (add) {
                    ipositions.push_back(found);
                    ipositions.push_back(pos);
                }
            }
            else
                break;
        }
    }
    sort(ipositions.begin(), ipositions.end(), comp);


    //First parameter is a string to split
    if (contextualpattern->Type() == a_uvector || !contextualpattern->isVectorContainer())
        kvect = Selectauvector(contextualpattern);
    else
        kvect = Selectacontainer(contextualpattern, idthread);


    Locking _lock(this);
    pos = 0;
    for (i = 0; i < ipositions.size(); i += 2) {
        if (pos != ipositions[i]) {
            thesplitter = value.substr(pos, ipositions[i] - pos);
            kvect->storevalue(thesplitter);
        }
        pos = ipositions[i + 1];
    }

    if (ipositions.back() < value.size())
        kvect->storevalue(value.substr(ipositions.back(), value.size() - ipositions.back()));


    return kvect;
}


Tamgu* Tamguustring::MethodSplit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    //Third parameter can be a vector

    Tamgu* kvect = NULL;
    wstring localvalue;
    
    wstring thestr = UString();
    
    wstring thesplitter;
    bool isuvector = false;
    if (contextualpattern->Type() == a_uvector || !contextualpattern->isVectorContainer()) {
        kvect = Selectauvector(contextualpattern);
        isuvector = true;
    }
    else
        kvect = Selectacontainer(contextualpattern, idthread);

    if (callfunc->Size() == 0) {
#ifdef WSTRING_IS_UTF16
		wstring c;
		for (long i = 0; i < thestr.size(); i++) {
			c = thestr[i];
			if (checklargeutf16(c[0]))
				c += thestr[++i];
			if (c[0] <= 32) {
#else
		wchar_t c;
		for (long i = 0; i < thestr.size(); i++) {
			c = thestr[i];
			if (c <= 32) {
#endif
        //we split along space characters...
                if (thesplitter.size()) {
                    if (isuvector)
                        ((Tamguuvector*)kvect)->values.push_back(thesplitter);
                    else
                        kvect->storevalue(thesplitter);
                    thesplitter = L"";
                }
            }
            else {
                thesplitter += c;
            }
        }

        if (thesplitter.size()) {
            if (isuvector)
                ((Tamguuvector*)kvect)->values.push_back(thesplitter);
            else
                kvect->storevalue(thesplitter);
        }
        return kvect;
    }

    Tamgu* tamgusplitter = callfunc->Evaluate(0, contextualpattern, idthread);
    if (tamgusplitter->isRegular()) {
        wstring w = UString();
        
        if (tamgusplitter->Type() == a_preg) {
            tamgusplitter->splitting(kvect, w);
            return kvect;
        }
        
        vector<long> values;

        tamgusplitter->searchall(w,values);
        if (!values.size())
            return kvect;
        
        //Now we extract the strings in between
        long pos = 0;
        wstring sub;
        for (long i = 0; i < values.size(); i += 2) {
            sub = w.substr(pos, values[i]-pos);
            if (sub != L"") {
                if (isuvector)
                    ((Tamguuvector*)kvect)->values.push_back(sub);
                else
                    kvect->storevalue(sub);
            }
            pos=values[i+1];
        }
        
        if (pos < w.size()) {
            sub = w.substr(pos,w.size()-pos);
            kvect->storevalue(sub);
        }
        
        return kvect;
    }
    
    thesplitter=tamgusplitter->UString();
    
    //Second parameter is the splitter string
    if (thesplitter == L"") {
        if (kvect->isString()) {
            //this is a vector of strings...
            for (long i = 0; i < thestr.size(); i++)
                kvect->storevalue(getfullchar(thestr, i));
            return kvect;
        }
        
		BLONG c;
        for (long i = 0; i < thestr.size(); i++) {
            c = getachar(thestr, i);
            kvect->storevalue(c);
        }
        
        return kvect;
    }

    size_t pos = 0;
    size_t found = 0;
    while (pos != string::npos) {
        found = s_find(thestr, thesplitter, pos);
        if (found != string::npos) {
            localvalue = thestr.substr(pos, found - pos);
            if (localvalue != L"") {
                if (isuvector)
                    ((Tamguuvector*)kvect)->values.push_back(localvalue);
                else
                    kvect->storevalue(localvalue);
            }
            pos = found + thesplitter.size();
        }
        else
            break;
    }

    localvalue = thestr.substr(pos, thestr.size() - pos);
    if (localvalue != L"") {
        if (isuvector)
            ((Tamguuvector*)kvect)->values.push_back(localvalue);
        else
            kvect->storevalue(localvalue);
    }

    return kvect;
}

Tamgu* Tamguustring::MethodSplite(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //Third parameter can be a vector
    
    Tamgu* kvect = NULL;
    wstring localvalue;
    
    wstring thestr = UString();
    
    wstring thesplitter;
    bool isuvector = false;
    if (contextualpattern->Type() == a_uvector || !contextualpattern->isVectorContainer()) {
        kvect = Selectauvector(contextualpattern);
        isuvector = true;
    }
    else
        kvect = Selectacontainer(contextualpattern, idthread);
    
    if (callfunc->Size() == 0) {
        #ifdef WSTRING_IS_UTF16
        wstring c;
        for (long i = 0; i < thestr.size(); i++) {
            c = thestr[i];
            if (checklargeutf16(c[0]))
                c += thestr[++i];
            if (c[0] <= 32) {
        #else
            wchar_t c;
            for (long i = 0; i < thestr.size(); i++) {
                c = thestr[i];
                if (c <= 32) {
        #endif
                //we split along space characters...
                if (thesplitter.size()) {
                    if (isuvector)
                        ((Tamguuvector*)kvect)->values.push_back(thesplitter);
                    else
                        kvect->storevalue(thesplitter);
                    thesplitter = L"";
                }
            }
            else {
                thesplitter += c;
            }
        }
        
        if (thesplitter.size()) {
            if (isuvector)
                ((Tamguuvector*)kvect)->values.push_back(thesplitter);
            else
                kvect->storevalue(thesplitter);
        }
        return kvect;
    }
    
    Tamgu* tamgusplitter = callfunc->Evaluate(0, contextualpattern, idthread);
    if (tamgusplitter->isRegular()) {
        wstring w = UString();
        
        if (tamgusplitter->Type() == a_preg) {
            tamgusplitter->splitting(kvect, w);
            return kvect;
        }
        
        vector<long> values;
        
        tamgusplitter->searchall(w,values);
        if (!values.size())
            return kvect;
        
        //Now we extract the strings in between
        long pos = 0;
        wstring sub;
        for (long i = 0; i < values.size(); i += 2) {
            sub = w.substr(pos, values[i]-pos);
            if (sub != L"") {
                if (isuvector)
                    ((Tamguuvector*)kvect)->values.push_back(sub);
                else
                    kvect->storevalue(sub);
            }
            pos=values[i+1];
        }
        
        if (pos < w.size()) {
            sub = w.substr(pos,w.size()-pos);
            kvect->storevalue(sub);
        }
        
        return kvect;
    }
    
    thesplitter=tamgusplitter->UString();
    
    //Second parameter is the splitter string
    if (thesplitter == L"") {
        if (kvect->isString()) {
            //this is a vector of strings...
            for (long i = 0; i < thestr.size(); i++)
                kvect->storevalue(getfullchar(thestr, i));
            return kvect;
        }
        
        BLONG c;
        for (long i = 0; i < thestr.size(); i++) {
            c = getachar(thestr, i);
            kvect->storevalue(c);
        }
        
        return kvect;
    }
    
    size_t pos = 0;
    size_t found = 0;
    while (pos != string::npos) {
        found = s_find(thestr, thesplitter, pos);
        if (found != string::npos) {
            localvalue = thestr.substr(pos, found - pos);
            if (isuvector)
                ((Tamguuvector*)kvect)->values.push_back(localvalue);
            else
                kvect->storevalue(localvalue);
            pos = found + thesplitter.size();
        }
        else
            break;
    }
    
    localvalue = thestr.substr(pos, thestr.size() - pos);
    if (localvalue != L"") {
        if (isuvector)
            ((Tamguuvector*)kvect)->values.push_back(localvalue);
        else
            kvect->storevalue(localvalue);
    }
    
    return kvect;
}

Tamgu* Tamguustring::MethodSlice(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* ke = Selectavector(contextualpattern);
    long num = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    if (num <= 0)
        return ke;
    
    long i;
    long beg = 0;
    
    Locking _lock(this);
    long sz = value.size();
    wstring w = UString();
    wstring sub;
    sz = w.size();
    for (i = num; i < sz; i+=num) {
        sub = w.substr(beg,i-beg);
        beg = i;
        ke->storevalue(sub);
    }
    if (i-beg>0) {
        sub = w.substr(beg,i-beg);
        ke->storevalue(sub);
    }
    return ke;
}

Tamgu* Tamguustring::MethodNgrams(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long num = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    if (num <= 0)
        return Selectasvector(contextualpattern);

    long i, j, mx;

    wstring u;
    Tamgu* ke = Selectauvector(contextualpattern);

    Locking _lock(this);

    mx = value.size() - num + 1;
    for (i = 0; i < mx; i++) {
        u = L"";
        for (j = i; j < i + num; j++)
            u += getfullchar(value, j);
        ke->storevalue(u);
    }
    return ke;
}

Tamgu* Tamguustring::MethodLatin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    return globalTamgu->Providestring(conversion_unicode_to_latin(value));
}


Tamgu* Tamguustring::MethodDos(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    return globalTamgu->Providestring(s_unicode_to_dos(value));
}

Tamgu* Tamguustring::MethodDostoutf8(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    agnostring thestr(UString());
    return globalTamgu->Providestring(thestr.dostoutf8());
}


Tamgu* Tamguustring::MethodLeft(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    long nbchar = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    Locking _lock(this);
    return globalTamgu->Provideustring(s_left(value, nbchar));
}

Tamgu* Tamguustring::MethodRight(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    long nbchar = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    Locking _lock(this);
    return globalTamgu->Provideustring(s_right(value, nbchar));
}

Tamgu* Tamguustring::MethodMid(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    //Second parameter is a position
    long pos = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    //Third parameter is a number of characters
    long nbchar = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    Locking _lock(this);
    return globalTamgu->Provideustring(s_middle(value, pos, nbchar));
}

Tamgu* Tamguustring::MethodIslower(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    return booleantamgu[s_is_lower(value)];
}

Tamgu* Tamguustring::MethodIsupper(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    return booleantamgu[s_is_upper(value)];
}

Tamgu* Tamguustring::MethodIsemoji(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    return booleantamgu[s_is_emoji(value)];
}

Tamgu* Tamguustring::MethodEmoji(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    return globalTamgu->Providestring(Emoji(value));
}


Tamgu* Tamguustring::MethodIsdigit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    Locking _lock(this);
    return booleantamgu[s_is_digit(value)];
}

Tamgu* Tamguustring::MethodIsalpha(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    Locking _lock(this);
    return booleantamgu[s_is_alpha(value)];
}

Tamgu* Tamguustring::MethodIsconsonant(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    Locking _lock(this);
    return booleantamgu[s_is_consonant(value)];
}

Tamgu* Tamguustring::MethodIsvowel(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    Locking _lock(this);
    return booleantamgu[s_is_vowel(value)];
}

Tamgu* Tamguustring::MethodDeaccentuate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    Locking _lock(this);
    return globalTamgu->Provideustring(s_deaccentuate(value));
}

Tamgu* Tamguustring::MethodIspunctuation(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    Locking _lock(this);
    return booleantamgu[s_is_punctuation(value)];
}

Tamgu* Tamguustring::MethodLower(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring str = UString();
    return globalTamgu->Provideustring(s_to_lower(str));
}

Tamgu* Tamguustring::MethodUpper(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    return globalTamgu->Provideustring(s_to_upper(value));
}

Tamgu* Tamguustring::MethodTrim(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring str = UString();
    return globalTamgu->Provideustring(Trim(str));
}

Tamgu* Tamguustring::MethodTrimright(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring str = UString();
    return globalTamgu->Provideustring(Trimright(str));
}

Tamgu* Tamguustring::MethodTrimleft(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring str = UString();
    return globalTamgu->Provideustring(Trimleft(str));
}

Tamgu* Tamguustring::MethodPadding(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long res = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    wstring sval = callfunc->Evaluate(1, contextualpattern, idthread)->UString();
    bool direction = true;
    if (callfunc->Size() == 3)
        direction = callfunc->Evaluate(2, contextualpattern, idthread)->Boolean();

    long sz;
    if (direction && sval.size() == 1) {
        Locking _lock(this);
        sz = value.size();
        if (res > sz)
            value.resize(res, sval[0]);
        return this;
    }

    sz = res - Size();
    if (sz <= 0)
        return this;

    wstring sres;
    wstring s;
    while (sz > 0) {
        s += sval;
        sz--;
    }
    sres = s;
  
    Locking _lock(this);
    if (direction)
        value += sres;
    else
        value = sres + value;

    return this;
}

Tamgu* Tamguustring::MethodFill(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* res = callfunc->Evaluate(0, contextualpattern, idthread);
    Tamgu* kval = callfunc->Evaluate(1, contextualpattern, idthread);
    long sz = res->Integer();
    if (sz <= 0)
        return globalTamgu->Provideustring(L"");

    wstring sval = kval->UString();
  
    Locking _lock(this);
    value = L"";
    while (sz > 0) {
        value += sval;
        sz--;
    }
    return this;
}

Tamgu* Tamguustring::MethodExtract(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    Tamgu* pos = callfunc->Evaluate(0, contextualpattern, idthread);
    Tamgu* from = callfunc->Evaluate(1, contextualpattern, idthread);
    vector<wstring> v;
    long i;
    for (i = 3; i < callfunc->Size(); i++)
        v.push_back(callfunc->Evaluate(i, contextualpattern, idthread)->UString());
  
    wstring init = UString();
    
    i = pos->Integer();
    wstring sfrom = from->UString();
    i = init.find(sfrom, i);
    long lgfrom = sfrom.size();
    Tamgu* vect = Selectauvector(contextualpattern);
    long nb, npos;

    while (i != -1) {
        npos = -1;
        for (long j = 0; j < v.size(); j++) {
            long spos = init.find(v[j], i + lgfrom);
            if (spos != -1 && (npos == -1 || spos < npos))
                npos = spos;
        }
        if (npos == -1)
            break;
        nb = npos - i - lgfrom;
        vect->storevalue(init.substr(i + lgfrom, nb));
        i = init.find(sfrom, npos + 1);
    }

    return vect;
}

Tamgu* Tamguustring::MethodReverse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    return globalTamgu->Provideustring(s_revert(value));
}


Tamgu* Tamguustring::MethodRemovefirst(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* knb = callfunc->Evaluate(0, contextualpattern, idthread);
    long id = knb->Integer();

    //On retire en tete
    Locking _lock(this);
    long iv = value.size();
    if (id >= iv)
        return aEMPTYUSTRING;

    return globalTamgu->Provideustring(value.substr(id, iv-id));
}

Tamgu* Tamguustring::MethodRemovelast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* knb = callfunc->Evaluate(0, contextualpattern, idthread);
    long id = knb->Integer();

    Locking _lock(this);
    long iv = value.size();
    if (id >= iv)
        return aEMPTYUSTRING;

    return globalTamgu->Provideustring(s_left(value, iv - id));
}

Tamgu* Tamguustring::MethodFormat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    wchar_t buffer[101];
    wstring sformat = UString();
    //In this case, we might have more than two parameters...
    wstring e;
    buffer[0] = L'%';
    for (long i = callfunc->Size() - 1; i >= 0; i--) {
        e = callfunc->Evaluate(i, contextualpattern, idthread)->UString();
        swprintf_s(buffer + 1, 100, L"%d", i + 1);
        sformat = s_replacestring(sformat, buffer, e);
    }
    return globalTamgu->Providewithustring(sformat);

}

Tamgu* Tamguustring::MethodReplace(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //Replace an element with another
    Tamgu* treg = callfunc->Evaluate(0, contextualpattern, idthread);
    if (treg->isRegular()) {
        wstring rep = callfunc->Evaluate(1, contextualpattern, idthread)->UString();
        
        
        wstring w;
        {
            Locking _lock(this);
            w = value;
        }
        
        if (treg->Type() == a_treg) {
            vector<long> values;
            
            treg->searchall(w,values);
            
            for (long i=values.size()-2; i>=0; i-=2)
                w=w.substr(0,values[i])+rep+w.substr(values[i+1],w.size()-values[i+1]);
        }
#ifdef Tamgu_REGEX
        else
            w = treg->wreplace(w, rep);
#endif
        
        return globalTamgu->Providewithustring(w);
    }
    
    wstring str = UString();
    wstring reg = treg->UString();
    if (reg == L"")
        return globalTamgu->Providewithustring(str);
    
    wstring rep = callfunc->Evaluate(1, contextualpattern, idthread)->UString();
    
    str = s_replacestring(str, reg, rep);
    return globalTamgu->Providewithustring(str);
}

Tamgu* Tamguustring::MethodEditdistance(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* e2 = callfunc->Evaluate(0, contextualpattern, idthread);
    return globalTamgu->Provideint(EditDistance(e2));
}


Tamgu* Tamguustring::MethodLast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter should be a vector
    Locking _lock(this);
    if (value.size() == 0)
        return globalTamgu->Provideustring(L"");
    wstring w = s_right(value, 1);
    return globalTamgu->Providewithustring(w);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

Tamgu* Tamguustring::MethodEvaluate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring w;
    
    Locking _lock(this);
    EvaluateMetaInString(w, value);
    
    return globalTamgu->Providewithustring(w);
}


void XNBrowse(x_node* xn, Tamgu* kf);
Tamgu* Tamguustring::MethodParse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    x_reading xr;
    bnf_tamgu bnf;

    x_node* xn;
    
    string str = String();
    xr.tokenize(str);
    if (xr.size() == 0)
        return Selectavector(contextualpattern);

    xn = bnf.x_parsing(&xr, FULL, false);
    string message;
    if (xn == NULL) {
        if (bnf.errornumber != -1) {
            string msg("EVL(100):");
            msg += bnf.x_errormsg(bnf.errornumber);
            char ch[20];
            sprintf_s(ch, 20, " (line:%ld)", bnf.lineerror);
            msg += ch;
            return globalTamgu->Returnerror(msg, idthread);
        }
        return globalTamgu->Returnerror("Unknown expression", idthread);
    }
    
    Tamgu* kvect = Selectavector(contextualpattern);
    XNBrowse(xn, kvect);
    return kvect;
}

Tamgu* Tamguustring::MethodParenthetic(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string str = String();
    string copen("(");
    string cclose(")");
    long nbparams = callfunc->Size();
    vector<string> rules;
    
    if (nbparams == 0)
        return globalTamgu->EvaluateParenthetic(str, copen, cclose, false, false, false, rules, idthread);
    
    copen = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    cclose = callfunc->Evaluate(1, contextualpattern, idthread)->String();
    if (copen.size() != 1 || cclose.size() != 1)
        return globalTamgu->Returnerror("TAMGUI(831): Wrong opening or closing character in LISP", idthread);
    
    bool comma = false;
    bool separator = false;
    bool keeprc = false;
    
    if (nbparams >= 3) {
        comma = callfunc->Evaluate(2, contextualpattern, idthread)->Boolean();
        if (nbparams >= 4) {
            separator = callfunc->Evaluate(3, contextualpattern, idthread)->Boolean();
            if (nbparams >= 5) {
                keeprc = callfunc->Evaluate(4, contextualpattern, idthread)->Boolean();
                if (nbparams == 6) {
                    Tamgu* vect = callfunc->Evaluate(5, contextualpattern, idthread);
                    for (long i = 0; i< vect->Size(); i++)
                        rules.push_back(vect->getstring(i));
                }
            }
        }
    }
    
    return globalTamgu->EvaluateParenthetic(str, copen, cclose, comma, separator, keeprc, rules, idthread);
}

Tamgu* Tamguustring::MethodTags(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string str = String();
    string copen("(");
    string cclose(")");
    
    long nbparams = callfunc->Size();
    
    vector<string> rules;
    copen = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    cclose = callfunc->Evaluate(1, contextualpattern, idthread)->String();
    if (copen.size() != 1 || cclose.size() != 1)
        return globalTamgu->Returnerror("TAMGUI(832): Wrong opening or closing character in TAGS", idthread);
    
    bool comma = false;
    bool separator = false;
    bool keeprc = false;
    
    if (nbparams >= 3) {
        comma = callfunc->Evaluate(2, contextualpattern, idthread)->Boolean();
        if (nbparams >= 4) {
            separator = callfunc->Evaluate(3, contextualpattern, idthread)->Boolean();
            if (nbparams >= 5) {
                keeprc = callfunc->Evaluate(4, contextualpattern, idthread)->Boolean();
                if (nbparams == 6) {
                    Tamgu* vect = callfunc->Evaluate(5, contextualpattern, idthread);
                    for (long i = 0; i< vect->Size(); i++)
                        rules.push_back(vect->getstring(i));
                }
            }
        }
    }
    
    return globalTamgu->EvaluateTags(str, copen, cclose, comma, separator, keeprc, rules, idthread);
}


Tamgu* Tamguustring::MethodScan(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    Tamgu* rgx = callfunc->Evaluate(0, contextualpattern, idthread);
    Au_automate* a = getautomate(rgx);
    if (a == NULL)
        return globalTamgu->Returnerror("Wrong grammar definition");

    wstring reg;
    {
        Locking _lock(this);
        reg = value;
    }
    
    bool fsep=false;
    wstring sep = L"";
    bool immediate=false;
    if (callfunc->Size() > 1) {
        sep = callfunc->Evaluate(1, contextualpattern, idthread)->UString();
        fsep=true;
        if (callfunc->Size() >= 3)
            immediate = callfunc->Evaluate(2, contextualpattern, idthread)->Boolean();
    }
    
    if (fsep || contextualpattern->isVectorContainer()) {
        
        Tamgu* vect = Selectavector(contextualpattern);
        
        vector<long> values;
        
        if (fsep) {
            wstring wlocal=reg;
            
            values.push_back(0);
            long sz = reg.size();
            
            for (long i =0; i< sz; i++) {
                values[0] = i;
                a->find(reg,sep, values);
                
                if (values.size()>1) {
                    if (i) {
                        for (long dx =1; dx < values.size();dx++)
                            values[dx] += i;
                    }
                    break;
                }
                else
                    if (immediate)
                        return vect;
                
                reg=reg.c_str()+1;
            }
            
            if (values.size() <= 1)
                return vect;
            
            Locking _vlock((TamguObject*)vect);
            
            if (contextualpattern->isNumber()) {
                //This a number vector, we return the positions...
                for (long i = 0; i< values.size(); i++)
                    vect->storevalue(values[i]);
                return vect;
            }
            
            wstring sub;
            for (long i=0; i< values.size() -1; i++) {
                sub = wlocal.substr(values[i], values[i+1]-values[i] -1);
                if (sub != L"")
                    vect->storevalue(sub);
            }
            if (callfunc->Size() == 4) {
                Tamgu* s = callfunc->Evaluate(3,contextualpattern, idthread);
                s->storevalue(wlocal.substr(values.back(), wlocal.size()-values.back()));
            }
            return vect;
            
        }
        
        a->searchall(reg,values);
        
        Locking _vlock((TamguObject*)vect);
        
        if (contextualpattern->isNumber()) {
            //This a number vector, we return the positions...
            for (long i = 0; i< values.size(); i++)
                vect->storevalue(values[i]);
            return vect;
        }
        
        wstring sub;
        for (long i=0; i< values.size(); i+=2) {
            sub = reg.substr(values[i], values[i+1]-values[i]);
            if (sub != L"")
                vect->storevalue(sub);
        }
        return vect;
    }


    if (contextualpattern->isNumber() && contextualpattern->isAtom()) {
        long res,end;
        a->search(reg,res,end);
        return globalTamgu->Provideint(res);
    }

    if (contextualpattern->isString()) {
        long res,end;
        if (a->search(reg,res,end)) {
            reg = reg.substr(res, end-res);
            return globalTamgu->Providewithustring(reg);
        }
        else
            return aEMPTYUSTRING;
    }

    return booleantamgu[a->match(reg)];
}

Tamgu* Tamguustring::MethodHash(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static const std::collate<char>& coll = std::use_facet<std::collate<char> >(loc);
    string s = String();
    if (s == "")
        return aNULL;
    long myhash = coll.hash(s.data(), s.data() + s.length());
    return globalTamgu->Provideint(myhash);
}

Tamgu* Tamguustring::MethodBase(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static vector<wstring> caracs;
    static hash_bin<wchar_t, long> mcaracs;
    wstring w;
    long n, b = -1;

    Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
    bool toconvert=true;
    if (callfunc->Size() == 2)
        toconvert = callfunc->Evaluate(1, contextualpattern, idthread)->Boolean();

    //We can reset the chararacter list...
    if (ke->isVectorContainer()) {
        caracs.clear();
        mcaracs.clear();
        b = ke->Size();
        for (n = 0; n < b; n++) {
            w = ke->getustring(n);
            mcaracs[w[0]] = caracs.size();
            caracs.push_back(w);
        }
        if (b)
            return globalTamgu->Provideint(b);
    }

    if (caracs.size() == 0) {
        w = L"0";
        for (n = 0; n < 10; n++) {
            mcaracs[w[0]] = caracs.size();
            caracs.push_back(w);
            w[0]++;
        }
        w = L"A";
        for (n = 10; n < 36; n++) {
            mcaracs[w[0]] = caracs.size();
            caracs.push_back(w);
            w[0]++;
        }
        w = L"a";
        for (n = 36; n < 62; n++) {
            mcaracs[w[0]] = caracs.size();
            caracs.push_back(w);
            w[0]++;
        }
        w = L"#";
        mcaracs[w[0]] = caracs.size();
        caracs.push_back(w);
        w = L"@";
        mcaracs[w[0]] = caracs.size();
        caracs.push_back(w);
        w = L"";
        if (!b)
            return aTRUE;
    }

    b = ke->Integer();

    if (b > caracs.size()) {
        char message[1024];
        sprintf_s(message, 1024, "TAMGUI(510): base too large, max is: %ld", caracs.size());
        return globalTamgu->Returnerror(message, idthread);
    }

    BLONG v = 0;
    if (!toconvert) {
        //we convert a base 10 number into the local base
        wstring res=UString();
        v=convertlong(res);
        long rest;
        res = L"";
        while (v) {
            rest = v%b;
            v /=b;
            res = caracs[rest]+res;
        }
        return globalTamgu->Providewithustring(res);
    }
    
    w = UString();
    wchar_t wc;
    for (n = 0; n < w.size(); n++) {
        wc = w[n];
        if (mcaracs.check(wc) == false || mcaracs[wc] >= b)
            return globalTamgu->Returnerror("TAMGUI(511): Cannot analyze this string in this base.", idthread);

        v *= b;
        v += mcaracs[wc];
    }
    return globalTamgu->Provideint(v);
}

Tamgu* Tamguustring::MethodTohtml(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    agnostring s(String());
    string val;
    Tamgu* res;
    vector<long> values;
    s.unicodes(values);
    if (contextualpattern->isContainer()) {
        Tamgu* kvect = Selectasvector(contextualpattern);
        for (long i = 0; i < values.size(); i++) {
            if (values[i] >= 128 || values[i] == 34 || values[i] == 38 || values[i] == 60 || values[i] == 62)
                c_HTMLcode(val, values[i]);
            else
                val = (char)values[i];
            kvect->storevalue(val);
        }
        return kvect;
    }

    res = globalTamgu->Providestring("");
    for (long i = 0; i < values.size(); i++) {
        if (values[i] >= 128 || values[i] == 34 || values[i] == 38 || values[i] == 60 || values[i] == 62)
            c_HTMLcode(val, values[i]);
        else
            val = (char)values[i];
        ((Tamgustring*)res)->value += val;
    }
    return res;
}

Tamgu* Tamguustring::MethodToxml(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    agnostring s(String());
    string val;
    Tamgu* res;
    vector<long> values;
    s.unicodes(values);
    if (contextualpattern->isContainer()) {
        Tamgu* kvect = Selectasvector(contextualpattern);
        for (long i = 0; i < values.size(); i++) {
            if (values[i] >= 128 || values[i] == 34 || values[i] == 38 || values[i] == 60 || values[i] == 62)
                c_XMLcode(val, values[i]);
            else
                val = (char)values[i];
            kvect->storevalue(val);
        }
        return kvect;
    }

    res = globalTamgu->Providestring("");
    for (long i = 0; i < values.size(); i++) {
        if (values[i] >= 128 || values[i] == 34 || values[i] == 38 || values[i] == 60 || values[i] == 62)
            c_XMLcode(val, values[i]);
        else
            val = (char)values[i];
        ((Tamgustring*)res)->value += val;
    }
    return res;
}

Tamgu* Tamguustring::MethodInsert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    size_t idx;

    //First parameter should be a vector

    //second parameter is anything
    idx = -1;
    string val;
    agnostring ret(UString());
    if (callfunc->Size() == 2) {
        idx = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        val = callfunc->Evaluate(1, contextualpattern, idthread)->String();
    }
    else
        val = callfunc->Evaluate(0, contextualpattern, idthread)->String();

    if (idx == -1)
        ret = ret.insertseparator(val);
    else {
        if (ret.insert(val, idx) == false)
            return aNULL;
    }
    return globalTamgu->Provideustring(ret.utf8tounicode());
}

Tamgu* Tamguustring::MethodClear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    value = L"";
    return aTRUE;
}

#ifdef WSTRING_IS_UTF16
void Tamguustring::Pop(long i) {
    Locking _lock(this);
    long sz = size_w(value);
    if (!sz)
        return;
    if (i == -1) {
        //the last character
        i = convertchartopos(value, 0, sz-1);
        while (i < value.size())
            value.pop_back();
        return;
    }

    if (i < 0 || i >= sz)
        return;
    
    long j = convertchartopos(value, 0, i);
    if (j == i) {
        value.erase(i, 1);
        return;
    }
    i = j;
    if (c_is_emoji(getachar(value, j))) {
        j++;
        while (c_is_emojicomp(getachar(value, j)))
            j++;
    }
    else
        j++;
    value.erase(i, j-i);
}
#endif
        
Tamgu* Tamguustring::MethodPop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter should be a vector

    //second parameter is an integer (the number of pop)
    long i = 0;
    long nb = 1;
    if (callfunc->Size() == 2)
        nb = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    if (callfunc->Size() >= 1)
        i = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    else
        i = -1;

    while (nb > 0) {
        Pop(i);
        nb--;
    }

    return aTRUE;
}


Tamgu* Tamguustring::Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {
    Tamgu* a;
    
    Tamguustring* it = globalTamgu->Provideustring(L"");
    it->Setreference();

    uchar addvalue = 0;

    if (context != aNOELEMENT)
        addvalue = Selecttype(context);

    recipient->Putvalue( it, idthread);

    wstring wvalue = UString();
    
    for (long i = 0; i < wvalue.size(); i++) {
        it->value = getfullchar(wvalue, i);
        
        a = bd->DirectEval(environment, aNULL, idthread);
        if (a->isNULL())
            continue;

        if (globalTamgu->Error(idthread) || a->needInvestigate()) {
            if (a == aBREAK)
                break;
            it->Resetreference();
            recipient->Forcedclean();
            a->Release();
            context->Release();
            return globalTamgu->Errorobject(idthread);
        }

        context = Storealongtype(context, a, idthread, addvalue);
        a->Release();
    }

    it->Resetreference();
    recipient->Forcedclean();

    return context;
}

Tamgu* Tamguustring::Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct) {

    Tamgu* returnval;

    bool first = false;
    Tamguustring* key = globalTamgu->Provideustring(L"");
    key->Setreference();

    if (init != aNOELEMENT) {
        accu->Putvalue(init, idthread);
        if (kcont != NULL) {
            if (direct)
                kcont->Insert(0, init);
            else
                kcont->Push(init);
        }
    }
    else
        first = true; //we are dealing with a foldr1 or a foldl1

    wstring wvalue = UString();
    
    for (long i = 0; i < wvalue.size(); i++) {
        key->value = getfullchar(wvalue, i);
        
		if (first) {
            returnval = key->Atom();//We use the first value of the list to seed our accumulator variable
            first = false;
        }
        else {
            var->Putvalue(key, idthread);
            returnval = bd->DirectEval(env, aNULL, idthread);

            if (returnval == aBREAK) {
                accu = returnval;
                break;
            }

            if (globalTamgu->Error(idthread)) {
                env->Cleaning(idthread);
                key->Resetreference();
                var->Forcedclean();
                if (kcont != NULL)
                    kcont->Release();
                return globalTamgu->Errorobject(idthread);
            }
        }

        if (returnval != aNULL) {
            
            accu->Putvalue(returnval, idthread);
            if (kcont != NULL) {
                if (direct)
                    kcont->Insert(0, returnval);
                else
                    kcont->Push(returnval);
            }
            returnval->Release();
        }
    }

    key->Resetreference();
    var->Forcedclean();
    if (kcont != NULL)
        return kcont;
    return accu->Value();
}


Tamgu* Tamguustring::andset(Tamgu* a, bool autoself) {
    wstring s = a->UString();
    wstring u;
    Locking _lock(this);
    long m = minlocal(s.size(), value.size());
    for (long i = 0; i < m; i++) {
        if (s[i] == value[i])
            u += s[i];
    }
    if (autoself) {
        value = u;
        return this;
    }

    return globalTamgu->Providewithustring(u);
}

Tamgu* Tamguustring::xorset(Tamgu* a, bool autoself) {
    wstring s = a->UString();
    wstring u;
    Locking _lock(this);
    long m = minlocal(s.size(), value.size());
    for (long i = 0; i < m; i++) {
        if (s[i] != value[i])
            u += value[i];
    }
    if (autoself) {
        value = u;
        return this;
    }

    return globalTamgu->Providewithustring(u);
}

void TamguLoopUString::Callfunction() {

    TamguCallFunction kfunc(function);

    Tamguint* ki = globalTamgu->Provideint(position);
    ki->Setreference();
    kfunc.arguments.push_back(this);
    kfunc.arguments.push_back(ki);
    Tamgu* ke = kfunc.Eval(aNULL, aNULL, globalTamgu->GetThreadid());
    ke->Release();
    ki->Resetreference();
}

Tamgu* TamguLoopUString::Put(Tamgu* context, Tamgu* ke, short idthread) {
    if (ke->isVectorContainer()) {
        position = 0;
        interval.clear();
        for (long i = 0; i < ke->Size(); i++)
            interval.push_back(ke->getustring(i));
        Locking _lock(this);
        value = interval[0];
        return aTRUE;
    }

    if (ke->Type() == a_uloop) {
        TamguLoopUString* kl = (TamguLoopUString*)ke;
        interval = kl->interval;
        Locking _lock(this);
        value = kl->value;
        position = kl->position;
        return aTRUE;
    }

    if (interval.size() == 0) {
        Locking _lock(this);
        position = 0;
        value = L"";
        return aTRUE;
    }

    position = ke->Integer();
    Locking _lock(this);
    if (position >= interval.size())
        position = position % interval.size();
    else
        if (position<0)
            position = (interval.size() + position) % interval.size();
    value = interval[position];
    return aTRUE;
}


Tamgu* TamguLoopUString::Vector(short idthread) {
    Tamguuvector* kvect = globalTamgu->Provideuvector();
    Doublelocking _lock(this, kvect);
    kvect->values = interval;
    return kvect;
}


Tamgu* TamguLoopUString::andset(Tamgu* a, bool autoself) {
    wstring s = a->UString();
    wstring u;
    Locking _lock(this);
    long m = minlocal(s.size(), value.size());
    for (long i = 0; i < m; i++) {
        if (s[i] == value[i])
            u += s[i];
    }
    if (autoself) {
        value = u;
        return this;
    }

    return globalTamgu->Providewithustring(u);
}

Tamgu* TamguLoopUString::xorset(Tamgu* a, bool autoself) {
    wstring s = a->UString();
    wstring u;
    Locking _lock(this);
    long m = minlocal(s.size(), value.size());
    for (long i = 0; i < m; i++) {
        if (s[i] != value[i])
            u += value[i];
    }
    if (autoself) {
        value = u;
        return this;
    }

    return globalTamgu->Providewithustring(u);
}

//------------------------------------------------------------------------------------------------------------------------
//We need to declare once again our local definitions.
Exporting basebin_hash<a_ustringMethod>  Tamgua_ustring::methods;
Exporting hmap<string, string> Tamgua_ustring::infomethods;
Exporting basebin_hash<unsigned long> Tamgua_ustring::exported;


Exporting short Tamgua_ustring::idtype = 0;

    //MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgua_ustring::AddMethod(TamguGlobal* global, string name, a_ustringMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Tamgua_ustring::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();
    
    Tamgua_ustring::idtype = global->Getid("a_ustring");
    
    Tamgua_ustring::AddMethod(global, "succ", &Tamgua_ustring::MethodSucc, P_NONE, "succ(): Return the successor of a character.");
    Tamgua_ustring::AddMethod(global, "pred", &Tamgua_ustring::MethodPred, P_NONE, "pred(): Return the predecessor of a byte.");
    
    
    Tamgua_ustring::AddMethod(global, "doublemetaphone", &Tamgua_ustring::MethodDoubleMetaphone, P_NONE, "doublemetaphone(): Return the string double metaphone conversion into a svector");
    
    Tamgua_ustring::AddMethod(global, "ord", &Tamgua_ustring::MethodOrd, P_NONE, "ord(): Return the character unicode.");
	Tamgua_ustring::AddMethod(global, "bytes", &Tamgua_ustring::MethodBytes, P_NONE, "bytes(): Return the Unicode representation.");
	Tamgua_ustring::AddMethod(global, "hash", &Tamgua_ustring::MethodHash, P_NONE, "hash(): Return the hash value of a string.");
    Tamgua_ustring::AddMethod(global, "reverse", &Tamgua_ustring::MethodReverse, P_NONE, "reverse(): reverse the string");
    Tamgua_ustring::AddMethod(global, "format", &Tamgua_ustring::MethodFormat, P_ATLEASTONE, "format(p1,p2,p3): Create a new string from the current string in which each '%x' is associated to one of the parameters, 'x' being the position of that parameter in the argument list. 'x' starts at 1.");
    Tamgua_ustring::AddMethod(global, "base", &Tamgua_ustring::MethodBase, P_ONE | P_TWO, "base(int b, bool toconvert=true): Return the value corresponding to the string in base b");
    Tamgua_ustring::AddMethod(global, "fill", &Tamgua_ustring::MethodFill, P_TWO, "fill(int nb,string c): create a string of nb characters c");
    Tamgua_ustring::AddMethod(global, "padding", &Tamgua_ustring::MethodPadding, P_TWO | P_THREE, "padding(int nb,string c,bool paddattheend): add nb characters c to the current string. Last parameter is optional");
    Tamgua_ustring::AddMethod(global, "parse", &Tamgua_ustring::MethodParse, P_NONE | P_TWO, "parse(): Parse a string as a piece of code and returns the evaluation as a vector.");
    Tamgua_ustring::AddMethod(global, "pop", &Tamgua_ustring::MethodPop, P_NONE | P_ONE | P_TWO, "pop(): remove last character");
    Tamgua_ustring::AddMethod(global, "sizeb", &Tamgua_ustring::MethodSizeb, P_NONE, "sizeb(): Return the size in bytes of the string");
    Tamgua_ustring::AddMethod(global, "parenthetics", &Tamgua_ustring::MethodParenthetic, P_NONE | P_TWO | P_THREE | P_FOUR | P_FIVE | P_SIX, "lisp(): lisp(string o,string c,bool comma,bool separator,bool concatenate): Parse a string as a parenthetic expressions, o is '(' and c is ')' by default. If 'comma' is true, then the decimal character is ',' otherwise it is '.'. If 'separator' is true then '1,000' is accepted as a number. If 'concatenate' is true then '3a' is a valid token");
    Tamgua_ustring::AddMethod(global, "tags", &Tamgua_ustring::MethodTags, P_TWO | P_THREE | P_FOUR | P_FIVE| P_SIX, "tags(string o,string c,bool comma,bool separator,bool concatenate, svector rules): Parse a string as a parenthetic expressions, where o and c are string (not characters). If 'comma' is true, then the decimal character is ',' otherwise it is '.'. If 'separator' is true then '1,000' is accepted as a number. If 'concatenate' is true then '3a' is a valid token");
    Tamgua_ustring::AddMethod(global, "scan", &Tamgua_ustring::MethodScan, P_ONE | P_TWO | P_THREE | P_FOUR, "scan(sub, string sep, bool immediate,string remaining): Find the substrings matching sub, with TRE. 'sep' is a separator between strings. 'immediate' always combines with a separator, it means that the matching should start at the first character of the string, default is false. 'remaining' also combines with 'separator', it returns the rest of the string after the section that matched.");
    Tamgua_ustring::AddMethod(global, "evaluate", &Tamgua_ustring::MethodEvaluate, P_NONE, "evaluate(): evaluate the meta-characters within a string and return the evaluated string.");
    Tamgua_ustring::AddMethod(global, "levenshtein", &Tamgua_ustring::MethodEditdistance, P_ONE | P_TWO, "levenshtein(string s,bool byte): Return the edit distance with 's' according to Levenshtein algorithm. If byte is true, force a byte level comparison. byte is optionnal.");
    Tamgua_ustring::AddMethod(global, "editdistance", &Tamgua_ustring::MethodEditdistance, P_ONE | P_TWO | P_THREE, "editdistance(string s,bool byte): Return the edit distance with 's'. If byte is true, force a byte level comparison. byte is optionnal.");
    Tamgua_ustring::AddMethod(global, "replace", &Tamgua_ustring::MethodReplace, P_TWO, "replace(string sub,string str): Replace the substrings matching sub with str");
    Tamgua_ustring::AddMethod(global, "multisplit", &Tamgua_ustring::MethodMultiSplit, P_ATLEASTONE, "multisplit(string splitter1,string splitter2..): split a string along different splitters. Return a uvector");
    Tamgua_ustring::AddMethod(global, "split", &Tamgua_ustring::MethodSplit, P_ONE | P_NONE, "split(string splitter): split a string along splitter and store the results  in a vector. If splitter=='', then the string is split into a vector of characters");
    Tamgua_ustring::AddMethod(global, "splite", &Tamgua_ustring::MethodSplite, P_ONE | P_NONE, "splite(string splitter): split a string along splitter and store the results  in a vector. If splitter=='', then the string is split into a vector of characters. Empty strings are kept in the result.");
    Tamgua_ustring::AddMethod(global, "tokenize", &Tamgua_ustring::MethodTokenize, P_NONE | P_ONE | P_TWO | P_THREE, "tokenize(bool comma,bool separator, svector rules): Segment a string into words and punctuations. If 'comma' is true, then the decimal character is ',' otherwise it is '.'. If 'separator' is true then '1,000' is accepted as a number. rules is a set of tokenization rules that can be first initialized then modified with _getdefaulttokenizerules");
    Tamgua_ustring::AddMethod(global, "stokenize", &Tamgua_ustring::MethodStokenize, P_NONE | P_ONE, "stokenize(map keeps): Segment a string into words and punctuations, with a keep.");
    Tamgua_ustring::AddMethod(global, "count", &Tamgua_ustring::MethodCount, P_TWO | P_ONE | P_NONE, "count(string sub,int pos): Count the number of substrings starting at position pos");
    Tamgua_ustring::AddMethod(global, "find", &Tamgua_ustring::MethodFind, P_TWO | P_ONE, "find(string sub,int pos): Return the position of substring sub starting at position pos");
    Tamgua_ustring::AddMethod(global, "rfind", &Tamgua_ustring::MethodRfind, P_TWO | P_ONE, "rfind(string sub,int pos): Return the position of substring sub backward starting at position pos");
    Tamgua_ustring::AddMethod(global, "removefirst", &Tamgua_ustring::MethodRemovefirst, P_ONE, "removefirst(int nb): remove the first nb characters of a string");
    Tamgua_ustring::AddMethod(global, "removelast", &Tamgua_ustring::MethodRemovelast, P_ONE, "removelast(int nb): remove the last nb characters of a string");
    Tamgua_ustring::AddMethod(global, "isutf8", &Tamgua_ustring::MethodIsutf8, P_NONE, "isutf8(): Return true is the string is encoded in UTF8");
    Tamgua_ustring::AddMethod(global, "slice", &Tamgua_ustring::MethodSlice, P_ONE, "slice(int sz): Return a vector of all slices of size sz");
    Tamgua_ustring::AddMethod(global, "ngrams", &Tamgua_ustring::MethodNgrams, P_ONE, "ngrams(int r): Return a vector of all ngrams of rank r");
    Tamgua_ustring::AddMethod(global, "latin", &Tamgua_ustring::MethodLatin, P_NONE, "latin(): convert an UTF8 string in LATIN");
    Tamgua_ustring::AddMethod(global, "dos", &Tamgua_ustring::MethodDos, P_NONE, "dos(): convert a string into DOS encoding");
    Tamgua_ustring::AddMethod(global, "dostoutf8", &Tamgua_ustring::MethodDostoutf8, P_NONE, "dostoutf8(): convert a DOS string into UTF8");
    Tamgua_ustring::AddMethod(global, "left", &Tamgua_ustring::MethodLeft, P_ONE, "left(int nb): return the first nb characters of a string");
    Tamgua_ustring::AddMethod(global, "right", &Tamgua_ustring::MethodRight, P_ONE, "right(int nb): return the last nb characters of a string");
    Tamgua_ustring::AddMethod(global, "mid", &Tamgua_ustring::MethodMid, P_TWO, "mid(int pos,int nb): return the nb characters starting at position pos of a string");
    Tamgua_ustring::AddMethod(global, "isalpha", &Tamgua_ustring::MethodIsalpha, P_NONE, "isalpha(): Test if a string only contains only alphabetical characters");
    Tamgua_ustring::AddMethod(global, "isconsonant", &Tamgua_ustring::MethodIsconsonant, P_NONE, "isconsonant(): Test if a string only contains consonants");
    Tamgua_ustring::AddMethod(global, "isvowel", &Tamgua_ustring::MethodIsvowel, P_NONE, "isvowel(): Test if a string only contains only vowels");
    Tamgua_ustring::AddMethod(global, "ispunctuation", &Tamgua_ustring::MethodIspunctuation, P_NONE, "ispunctuation(): Test if the character is a punctuation");
    Tamgua_ustring::AddMethod(global, "isdigit", &Tamgua_ustring::MethodIsdigit, P_NONE, "isdigit(): Test if a string only contains digits");
    Tamgua_ustring::AddMethod(global, "extract", &Tamgua_ustring::MethodExtract, P_ATLEASTTHREE, "extract(int pos,string from,string up1,string up2...): extract substrings between 'from' and 'up1'...'upn' (the shortest string is used). Return a vector of strings");
    Tamgua_ustring::AddMethod(global, "isemoji", &Tamgua_ustring::MethodIsemoji, P_NONE, "isemoji(): Test if a string only contains emoji characters");
    Tamgua_ustring::AddMethod(global, "emoji", &Tamgua_ustring::MethodEmoji, P_NONE, "emoji(): Return the textual description of an emoji");
    Tamgua_ustring::AddMethod(global, "isupper", &Tamgua_ustring::MethodIsupper, P_NONE, "isupper(): Test if a string only contains uppercase characters");
    Tamgua_ustring::AddMethod(global, "islower", &Tamgua_ustring::MethodIslower, P_NONE, "islower(): Test if a string only contains lowercase characters");
    Tamgua_ustring::AddMethod(global, "upper", &Tamgua_ustring::MethodUpper, P_NONE, "upper(): Return the string in upper characters");
    Tamgua_ustring::AddMethod(global, "deaccentuate", &Tamgua_ustring::MethodDeaccentuate, P_NONE, "deaccentuate(): Remove the accents from accented characters");
    Tamgua_ustring::AddMethod(global, "lower", &Tamgua_ustring::MethodLower, P_NONE, "lower(): Return the string in lower characters");
    Tamgua_ustring::AddMethod(global, "trim", &Tamgua_ustring::MethodTrim, P_NONE, "trim(): remove the trailing characters");
    Tamgua_ustring::AddMethod(global, "trimleft", &Tamgua_ustring::MethodTrimleft, P_NONE, "trimleft(): remove the trailing characters on the left");
    Tamgua_ustring::AddMethod(global, "trimright", &Tamgua_ustring::MethodTrimright, P_NONE, "trimright(): remove the trailing characters on the right");
    Tamgua_ustring::AddMethod(global, "last", &Tamgua_ustring::MethodLast, P_NONE, "last(): return last character");
    Tamgua_ustring::AddMethod(global, "insert", &Tamgua_ustring::MethodInsert, P_ONE | P_TWO, "insert(int i,string s): insert the string s at i. If i is -1, then insert s between each character in the input string");
    Tamgua_ustring::AddMethod(global, "clear", &Tamgua_ustring::MethodClear, P_NONE, "clear(): Clean the content of a string.");
    
    Tamgua_ustring::AddMethod(global, "jamo", &Tamgua_ustring::MethodJamo, P_NONE | P_ONE, "jamo(bool combine): if 'combine' is false split a Korean jamo into its main components, else combine contents into a jamo.");
    Tamgua_ustring::AddMethod(global, "isjamo", &Tamgua_ustring::MethodIsJamo, P_NONE, "isjamo(): return true if it is a Hangul jamo.");
    Tamgua_ustring::AddMethod(global, "ishangul", &Tamgua_ustring::MethodIsHangul, P_NONE, "ishangul(): return true if it is a Hangul character.");
    Tamgua_ustring::AddMethod(global, "normalizehangul", &Tamgua_ustring::MethodNormalizeHangul, P_NONE, "normalizehangul(): Normalize Hangul characters.");
    Tamgua_ustring::AddMethod(global, "romanization", &Tamgua_ustring::MethodTransliteration, P_NONE, "romanization(): romanization of Hangul characters.");


    if (version != "") {
        global->newInstance[Tamgua_ustring::idtype] = new Tamgua_ustring(L"", global);
        global->RecordMethods(Tamgua_ustring::idtype, Tamgua_ustring::exported);
    }
    
    return true;
}

TamguIteration* Tamgua_ustring::Newiteration(bool direction) {
    wstring w = value.value();
    return new TamguIterationustring(w, direction);
}

Tamgu* Tamgua_ustring::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    wstring val = value.value();
    long sz = val.size();

    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    Tamguustring v("");
    Tamgu* a = aNULL;
    bool testcond = false;
    for (long i = 0; i < sz && !testcond; i++) {
        a->Releasenonconst();
        v.value =  getfullchar(val, i);
        var->Putvalue(&v, idthread);
        
        a = ins->instructions.vecteur[1]->Eval(context, aNULL, idthread);
        
        //Continue does not trigger needInvestigate
        testcond = a->needInvestigate();
    }
    
    if (testcond) {
        if (a == aBREAK)
            return this;
        return a;
    }
    
    a->Releasenonconst();
    
    return this;
}


Tamgu* Tamgua_ustring::in(Tamgu* context, Tamgu* a, short idthread) {
        //Three cases along the container type...
        //It is a Boolean, we expect false or true
        //It is an integer, we expect a position in v
        //It is a container, we are looking for all positions...
    
    wstring val = a->UString();
    wstring w = value.value();
    
    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Locking* _vlock = _getlock(v);
        s_findall(w, val, v->values);
        _cleanlock(_vlock);
        return v;
    }
    
    long r = s_find(w, val, 0);
    
    
    if (context->isString()) {
        if (r==-1)
            return aEMPTYUSTRING;
        return new Tamgua_ustring(val);
    }
    
    if (context->isNumber())
        return globalTamgu->Provideint(r);
    
    if (r == -1)
        return aFALSE;
    return aTRUE;
}

unsigned long Tamgua_ustring::EditDistance(Tamgu* e) {
    wstring w = value.value();
    wstring s2 = e->UString();
    unsigned long s1len, s2len, x, y, lastdiag, olddiag;
    s1len = w.size();
    s2len = s2.size();
    size_t* column = new size_t[s1len + 1];
    for (y = 1; y <= s1len; y++)
        column[y] = y;
    for (x = 1; x <= s2len; x++) {
        column[0] = x;
        for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
            olddiag = column[y];
            column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + (w[y - 1] == s2[x - 1] ? 0 : 1));
            lastdiag = olddiag;
        }
    }
    s2len = column[s1len];
    delete[] column;
    return s2len;
}


#ifdef WSTRING_IS_UTF16
Tamgu* Tamgua_ustring::MethodOrd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	wstring s = value.value();
	uint32_t c;
	if (s.size() >= 1) {
		if (contextualpattern->isVectorContainer() || s.size() > 1) {
			Tamgu* kvect = SelectContainer(contextualpattern, idthread);
			if (kvect == NULL)
				kvect = new Tamgulvector;

			Locking _lock((TamguObject*)kvect);
			for (size_t i = 0; i < s.size(); i++) {
				if (c_utf16_to_unicode(c, s[i], false))
					c_utf16_to_unicode(c, s[++i], true);
				kvect->storevalue((BLONG)c);
				return kvect;
			}
		}

		if (c_utf16_to_unicode(c, s[0], false))
			c_utf16_to_unicode(c, s[1], true);
		if (contextualpattern->isNumber()) {
			contextualpattern->storevalue((BLONG)c);
			return contextualpattern;
		}

		return globalTamgu->Providelong(c);
	}

	return aNULL;
}

Tamgu* Tamgua_ustring::MethodBytes(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	wstring s = value.value();
	if (s.size() > 0) {
        if (contextualpattern->isNumber()) {
            uint32_t c;
            if (c_utf16_to_unicode(c, s[0], false))
                c_utf16_to_unicode(c, s[1], true);
            
            return globalTamgu->Providelong((BLONG)c);
        }
        
        Tamgulvector* kvect=(Tamgulvector*)Selectalvector(contextualpattern);
        kvect->locking();
        for (size_t i = 0; i < s.size(); i++)
            kvect->storevalue((BLONG)s[i]);
        kvect->unlocking();
        return kvect;
	}

	return aNULL;
}
#else
Tamgu* Tamgua_ustring::MethodOrd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring s = value.value();
    if (s.size() >= 1) {
        if (contextualpattern->isVectorContainer() || s.size()>1) {
            Tamgu* kvect=SelectContainer(contextualpattern,idthread);
            if (kvect==NULL)
                kvect=new Tamgulvector;
            
            Locking _lock((TamguObject*)kvect);
            for (size_t i = 0; i < s.size(); i++)
                kvect->storevalue(s[i]);
            return kvect;
        }
        
        if (contextualpattern->isNumber()) {
            contextualpattern->storevalue((BLONG)s[0]);
            return contextualpattern;
        }
        
        return globalTamgu->Providelong(s[0]);
    }
    
    return aNULL;
}

Tamgu* Tamgua_ustring::MethodBytes(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	wstring s = value.value();
	if (s.size() > 0) {
        if (contextualpattern->isNumber()) {
            uint32_t c;
            if (c_utf16_to_unicode(c, s[0], false))
                c_utf16_to_unicode(c, s[1], true);
            
            return globalTamgu->Providelong((BLONG)c);
        }
        
        Tamgulvector* kvect=(Tamgulvector*)Selectalvector(contextualpattern);
        kvect->locking();
        for (size_t i = 0; i < s.size(); i++)
            kvect->storevalue((BLONG)s[i]);
        kvect->unlocking();
        return kvect;
	}

	return aNULL;
}
#endif

Tamgu* Tamgua_ustring::Put(Tamgu* idx, Tamgu* v, short idthread) {
    
    if (!idx->isIndex()) {
        if (v == this) {
            return aFALSE;
        }
        
        if (v->Type() == Tamgua_ustring::idtype)
            value = ((Tamgua_ustring*)v)->value;
        else
            value = v->UString();
        
        return aTRUE;
    }

    wstring s;
    if (v != aNULL)
        s = v->UString();

    wstring w = value.value();
    
    long ileft, iright;
    char res = StringIndexes(w, idx, ileft, iright, idthread);
    
    if (res == 0) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong key in a container or a string access", idthread);
        return aFALSE;
    }
    
    if (res == 1) {
        iright = ileft;
        iright = c_char_next(USTR(w), iright);
    }
    else
        iright = iright - ileft;
    
    w.erase(ileft, iright);
    if (s != L"")
        w.insert(ileft, s);

    value = w;
    return aTRUE;
}

Tamgu* Tamgua_ustring::Eval(Tamgu* context, Tamgu* idx, short idthread) {
    if (!idx->isIndex() || context == idx)
        return this;
    
    long ileft, iright;
    wstring w = value.value();
    
    char res = StringIndexes(w, idx, ileft, iright, idthread);
    
    if (res == 0) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong key in a container or a string access", idthread);
        return aNOELEMENT;
    }
    
	if (res == 1) {
#ifdef WSTRING_IS_UTF16
		wstring r;
		r = w[ileft];
		if (checklargeutf16(r[0]))
			r += w[ileft + 1];
		idx = new Tamgua_ustring(r);
#else
		idx = new Tamgua_ustring(value[ileft]);
#endif
	}
    else {
        idx = new Tamgua_ustring((wchar_t)0);
        value.substr(((Tamgua_ustring*)idx)->value, ileft, iright - ileft);
    }
    return idx;
}

Tamgu* Tamgua_ustring::MethodCount(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long i = 0;
    
    if (callfunc->Size() == 2) {
        i = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    }
    
    Tamgu* substr = callfunc->Evaluate(0, contextualpattern, idthread);
    
    if (substr->isRegular()) {
        wstring w = UString();
        if (i)
            w = w.substr(i, w.size() - i);
        
        vector<long> values;
        substr->searchall(w,values);
        return globalTamgu->Provideint(values.size()>>1);
    }

    wstring str = value.value();
    if (str == L"")
        return aZERO;
    wstring sub = substr->UString();

    return globalTamgu->Provideint(s_count(str, sub, i));
}


Tamgu* Tamgua_ustring::MethodFind(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* ksub = callfunc->Evaluate(0, contextualpattern, idthread);
    
    long sz = Size();
    if (!sz) {
        if (contextualpattern->isVectorContainer())
            return Selectavector(contextualpattern);
        return aMINUSONE;
    }
    
        //we search for a substring starting at position idx
    long i = 0;
    if (callfunc->Size() == 2) {
        i = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
        if (i>=sz) {
            if (contextualpattern->isVectorContainer())
                return Selectavector(contextualpattern);
            return aMINUSONE;
        }
    }
    
    wstring str = value.value();
    
    if (contextualpattern->isVectorContainer()) {
        Tamgu* kvect = Selectavector(contextualpattern);
        long j;
        
        vector<long> v;
        if (ksub->isRegular()) {
            ksub->searchall(str,v,i);
        }
        else {
            wstring sub = ksub->UString();
            s_findall(str, sub, v, i);
        }
        
        if (v.size()) {
            Locking _lock((TamguObject*)kvect);
            
            switch (kvect->Type()) {
                case a_bvector:
                    for (j = 0; j < v.size(); j++) {
                        if (v[j] >= i)
                            ((Tamgubvector*)kvect)->values.push_back((uchar)v[j]);
                    }
                    return kvect;
                case a_ivector:
                    for (j = 0; j < v.size(); j++) {
                        if (v[j] >= i)
                            ((Tamguivector*)kvect)->values.push_back((uchar)v[j]);
                    }
                    return kvect;
                case a_fvector:
                    for (j = 0; j < v.size(); j++) {
                        if (v[j] >= i)
                            ((Tamgufvector*)kvect)->values.push_back((double)v[j]);
                    }
                    return kvect;
            }
            
            TamguConstInt loc(0);
            for (j = 0; j < v.size(); j++) {
                if (v[j] >= i) {
                    loc.value = v[j];
                    kvect->Push(&loc);
                }
            }
        }
        
        return kvect;
    }
    
    if (ksub->isRegular()) {
        long e;
        if (!ksub->search(str,i,e,i))
            return aMINUSONE;
        return globalTamgu->Provideint(i);
    }
    
    wstring sub = ksub->UString();
    i = s_find(str, sub, i);
    
    if (i == string::npos)
        return aMINUSONE;
    
    return globalTamgu->Provideint(i);
}

Tamgu* Tamgua_ustring::MethodRfind(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* ksub = callfunc->Evaluate(0, contextualpattern, idthread);
    
        //we search for a substring starting at position idx
    wstring str = value.value();
    if (!str.size()) {
        if (contextualpattern->isVectorContainer())
            return Selectavector(contextualpattern);
        return aMINUSONE;
    }

    long i = -1;
    if (callfunc->Size() == 2) {
        i = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
        i = convertpostochar(str,0, i);
    }

    if (contextualpattern->isVectorContainer()) {
        if (i == -1)
            i = 0;

        Tamgu* kvect = Selectavector(contextualpattern);
        long j;
        
        vector<long> v;
        if (ksub->isRegular()) {
            ksub->searchall(str,v,i);
        }
        else {
            wstring sub = ksub->UString();
            s_findall(str, sub, v, i);
        }
        
        if (v.size()) {
            Locking _lock((TamguObject*)kvect);
            
            switch (kvect->Type()) {
                case a_bvector:
                    for (j = v.size()-1; j >=0 ; j--) {
                        if (v[j] < i)
                            break;
                        ((Tamgubvector*)kvect)->values.push_back((uchar)v[j]);
                    }
                    return kvect;
                case a_ivector:
                    for (j = v.size()-1; j >=0 ; j--) {
                        if (v[j] < i)
                            break;
                        ((Tamguivector*)kvect)->values.push_back((uchar)v[j]);
                    }
                    return kvect;
                case a_fvector:
                    for (j = v.size()-1; j >=0 ; j--) {
                        if (v[j] < i)
                            break;
                        ((Tamgufvector*)kvect)->values.push_back((double)v[j]);
                    }
                    return kvect;
            }
            
            TamguConstInt loc(0);
            for (j = v.size()-1; j >=0 ; j--) {
                if (v[j] < i)
                    break;
                loc.value = v[j];
                kvect->Push(&loc);
            }
        }
        
        return kvect;
    }
    
    if (i == -1)
        i = str.size();

    if (ksub->isRegular()) {
        long e;
        if (!ksub->searchlast(str,i,e,i))
            return aMINUSONE;
        return globalTamgu->Provideint(i);
    }
    
    wstring sub = ksub->UString();
    i = s_rfind(str, sub, i);
    
    if (i == string::npos)
        return aMINUSONE;
    
    return globalTamgu->Provideint(i);
}

Tamgu* Tamgua_ustring::MethodDoubleMetaphone(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    
    string s = String();
    Tamgu* kvect = Selectasvector(contextualpattern);
    
    DoubleMetaphone(s,((Tamgusvector*)kvect)->values);
    return kvect;
}


Tamgu* Tamgua_ustring::MethodTokenize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring thestr = value.value();
    short flag = 0;
    bool comma = false;
    bool separator = false;
    
    vector<string> rules;
    
    if (callfunc->Size() >= 1) {
        comma = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
        if (comma)
            flag |= token_comma;
        if (callfunc->Size() >= 2) {
            separator = callfunc->Evaluate(1, contextualpattern, idthread)->Boolean();
            if (separator)
                flag |= token_separator;
            if (callfunc->Size() == 3) {
                Tamgu* vect = callfunc->Evaluate(2, contextualpattern, idthread);
                for (long i = 0; i< vect->Size(); i++)
                    rules.push_back(vect->getstring(i));
            }
        }
    }
    
    Tamgu* kvect = Selectauvector(contextualpattern);
    
    Locking _vlock((TamguObject*)kvect);
    if (rules.size()) {
        if (!vw_tokenize(((Tamguuvector*)kvect)->values, thestr, flag, rules)) {
            wstring w = ((Tamguuvector*)kvect)->values[0];
            string s;
            sc_unicode_to_utf8(s, w);
            kvect->Clear();
            kvect->Release();
            return globalTamgu->Returnerror(s,idthread);
        }
    }
    else
        vw_tokenize(((Tamguuvector*)kvect)->values, thestr, flag);
    return kvect;
}

Tamgu* Tamgua_ustring::MethodStokenize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring thestr = value.value();
    Tamgu* keeps = NULL;
    
    if (callfunc->Size() == 2) {
        keeps = callfunc->Evaluate(0, contextualpattern, idthread);
        if (!keeps->isContainer())
            return globalTamgu->Returnerror("TAMGUI(665): Expecting a container in STOKENIZE", idthread);
    }
    
    vector<wstring> v;
    map<wstring, bool> k;
    if (keeps != NULL) {
        TamguIteration* itr = keeps->Newiteration(false);
        for (itr->Begin(); itr->End() == aFALSE; itr->Next())
            k[itr->Valueustring()] = true;
        itr->Release();
    }
    s_tokenize(v, thestr, k);
    
    Tamgu* kvect = Selectauvector(contextualpattern);
    if (kvect->Type() == a_uvector) {
        Locking _lock((TamguObject*)kvect);
        ((Tamguuvector*)kvect)->values = v;
    }
    else {
        TamguConstUString localstring(L"");
        
        for (long i = 0; i < v.size(); i++) {
            localstring.value = v[i];
            kvect->Push(&localstring);
        }
    }
    return kvect;
}

Tamgu* Tamgua_ustring::MethodJamo(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring w;
    wstring vl = value.value();
    
    if (callfunc->Size() == 0 || callfunc->Evaluate(0, contextualpattern, idthread)->Boolean() == false) {
        w = s_split_jamo(vl);
        if (contextualpattern->isVectorContainer()) {
            Tamguuvector* vs = (Tamguuvector*)Selectauvector(contextualpattern);
            Locking _vlock(vs);
            wstring s;
            for (long i = 0; i < w.size(); i++) {
                s = w[i];
                vs->values.push_back(s);
            }
            return vs;
        }
        
        return new Tamgua_ustring(w);
    }
        //Else combine...
    w = s_combine_jamo(vl);
    return new Tamgua_ustring(w);
}

Tamgu* Tamgua_ustring::MethodIsJamo(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring w = value.value();
    return booleantamgu[s_is_jamo(w)];
}

Tamgu* Tamgua_ustring::MethodIsHangul(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring w = value.value();
    return booleantamgu[s_is_hangul(w)];
}

Tamgu* Tamgua_ustring::MethodNormalizeHangul(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring w = value.value();
    return new Tamgua_ustring(s_hangul_normalize(w));
    
}

Tamgu* Tamgua_ustring::MethodTransliteration(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string c;
    wstring w = value.value();

    if (contextualpattern->isVectorContainer()) {
        Tamgusvector* vs = (Tamgusvector*)Selectasvector(contextualpattern);
        Locking _vlock(vs);
        for (long i = 0; i < value.size(); i++) {
            c = c_translate(w[i]);
            vs->values.push_back(c);
        }
        return vs;
    }
    
    if (w.size())
        c = c_translate(w[0]);
    
    return globalTamgu->Providewithstring(c);
}

Tamgu* Tamgua_ustring::MethodMultiSplit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    
        //Third parameter can be a vector

    wstring w = value.value();
    Tamgu* kvect = NULL;
    wstring thesplitter;
    vector<long> ipositions;
    size_t pos = 0, found;
    long i = 0;
        //We first fill our vector with initial values...
    while (!pos && i < callfunc->Size()) {
        thesplitter = callfunc->Evaluate(i, contextualpattern, idthread)->UString();
        pos = 0;
        while (pos != string::npos) {
            found = s_find(w, thesplitter, pos);
            if (found != string::npos) {
                ipositions.push_back(found);
                pos = found + thesplitter.size();
                ipositions.push_back(pos);
            }
            else
                break;
        }
        i++;
    }
        //we have three forbidden cases:
        //a) x'<x and y'>x
        //b) x'>x and y'<y
        //c) x'>x and y'<y
    
    
    for (; i < callfunc->Size(); i++) {
        thesplitter = callfunc->Evaluate(i, contextualpattern, idthread)->UString();
        pos = 0;
        while (pos != string::npos) {
            found = s_find(w, thesplitter, pos);
            if (found != string::npos) {
                bool add = true;
                pos = found + thesplitter.size();
                for (int j = 0; j < ipositions.size(); j += 2) {
                    if (found <= ipositions[j] && pos >= ipositions[j]) {
                        add = false;
                        break;
                    }
                    if (found >= ipositions[j] && (pos <= ipositions[j + 1] || found <= ipositions[j + 1])) {
                        add = false;
                        break;
                    }
                }
                
                if (add) {
                    ipositions.push_back(found);
                    ipositions.push_back(pos);
                }
            }
            else
                break;
        }
    }
    sort(ipositions.begin(), ipositions.end(), comp);
    
    
        //First parameter is a string to split
    if (contextualpattern->Type() == a_uvector || !contextualpattern->isVectorContainer())
        kvect = Selectauvector(contextualpattern);
    else
        kvect = Selectacontainer(contextualpattern, idthread);
    
    
    pos = 0;
    for (i = 0; i < ipositions.size(); i += 2) {
        if (pos != ipositions[i]) {
            thesplitter = w.substr(pos, ipositions[i] - pos);
            kvect->storevalue(thesplitter);
        }
        pos = ipositions[i + 1];
    }
    
    if (ipositions.back() < value.size())
        kvect->storevalue(w.substr(ipositions.back(), w.size() - ipositions.back()));
    
    
    return kvect;
}


Tamgu* Tamgua_ustring::MethodSplit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    
        //Third parameter can be a vector
    
    Tamgu* kvect = NULL;
    wstring localvalue;
    
    wstring thestr = value.value();
    
    wstring thesplitter;
    if (contextualpattern->Type() == a_svector || !contextualpattern->isVectorContainer())
        kvect = Selectasvector(contextualpattern);
    else
        kvect = Selectacontainer(contextualpattern, idthread);
    
    if (callfunc->Size() == 0) {
#ifdef WSTRING_IS_UTF16
		wstring c;
		for (long i = 0; i < thestr.size(); i++) {
			c = thestr[i];
			if (checklargeutf16(c[0]))
				c += thestr[++i];
			if (c[0] <= 32) {
#else
		wchar_t c;
		for (long i = 0; i < thestr.size(); i++) {
			c = thestr[i];
			if (c <= 32) {
#endif
				if (thesplitter.size()) {
                    kvect->storevalue(thesplitter);
                    thesplitter = L"";
                }
            }
            else {
                thesplitter += c;
            }
        }
        
        if (thesplitter.size())
            kvect->storevalue(thesplitter);
        return kvect;
    }
    
    Tamgu* tamgusplitter = callfunc->Evaluate(0, contextualpattern, idthread);
    if (tamgusplitter->isRegular()) {
        
        if (tamgusplitter->Type() == a_preg) {
            tamgusplitter->splitting(kvect, thestr);
            return kvect;
        }
        
        vector<long> values;
        
        tamgusplitter->searchall(thestr,values);
        if (!values.size())
            return kvect;
        
            //Now we extract the strings in between
        long pos = 0;
        wstring sub;
        for (long i = 0; i < values.size(); i += 2) {
            sub = thestr.substr(pos, values[i]-pos);
            if (sub != L"")
                kvect->storevalue(sub);
            pos=values[i+1];
        }
        
        if (pos < thestr.size()) {
            sub = thestr.substr(pos,thestr.size()-pos);
            kvect->storevalue(sub);
        }
        
        return kvect;
    }
    
    thesplitter=tamgusplitter->UString();
            
    //Second parameter is the splitter string
    if (thesplitter == L"") {
        
        if (kvect->isString()) {
                //this is a vector of strings...
            for (long i = 0; i < thestr.size(); i++)
                kvect->storevalue(getfullchar(thestr, i));
            return kvect;
        }

		BLONG c;
        for (long i = 0; i < thestr.size(); i++) {
            c = getachar(thestr, i);
            kvect->storevalue(c);
        }
        
        return kvect;
    }
            
    size_t pos = 0;
    size_t found = 0;
    while (pos != string::npos) {
        found = s_find(thestr, thesplitter, pos);
        if (found != string::npos) {
            localvalue = thestr.substr(pos, found - pos);
            if (localvalue != L"")
                kvect->storevalue(localvalue);
            pos = found + thesplitter.size();
        }
        else
            break;
    }
            
    localvalue = thestr.substr(pos, thestr.size() - pos);
    if (localvalue != L"")
        kvect->storevalue(localvalue);
    
    return kvect;
}

Tamgu* Tamgua_ustring::MethodSplite(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    
        //Third parameter can be a vector
    
    Tamgu* kvect = NULL;
    wstring localvalue;
    
    wstring thestr = value.value();
    
    wstring thesplitter;
    if (contextualpattern->Type() == a_svector || !contextualpattern->isVectorContainer())
        kvect = Selectasvector(contextualpattern);
    else
        kvect = Selectacontainer(contextualpattern, idthread);
    
    if (callfunc->Size() == 0) {
#ifdef WSTRING_IS_UTF16
		wstring c;
		for (long i = 0; i < thestr.size(); i++) {
			c = thestr[i];
			if (checklargeutf16(c[0]))
				c += thestr[++i];
			if (c[0] <= 32) {
#else
		wchar_t c;
		for (long i = 0; i < thestr.size(); i++) {
			c = thestr[i];
			if (c <= 32) {
#endif
				if (thesplitter.size()) {
                    kvect->storevalue(thesplitter);
                    thesplitter = L"";
                }
            }
            else
                thesplitter += c;
        }
        
        if (thesplitter.size())
            kvect->storevalue(thesplitter);
        return kvect;
    }
    
    Tamgu* tamgusplitter = callfunc->Evaluate(0, contextualpattern, idthread);
    if (tamgusplitter->isRegular()) {
        wstring w = value.value();
        
        if (tamgusplitter->Type() == a_preg) {
            tamgusplitter->splitting(kvect, w);
            return kvect;
        }
        
        vector<long> values;
        
        tamgusplitter->searchall(w,values);
        if (!values.size())
            return kvect;
        
            //Now we extract the strings in between
        long pos = 0;
        wstring sub;
        for (long i = 0; i < values.size(); i += 2) {
            sub = w.substr(pos, values[i]-pos);
            kvect->storevalue(sub);
            pos=values[i+1];
        }
        
        if (pos < w.size()) {
            sub = w.substr(pos,w.size()-pos);
            kvect->storevalue(sub);
        }
        
        return kvect;
    }
    
    thesplitter=tamgusplitter->UString();
    
    //Second parameter is the splitter string
    if (thesplitter == L"") {
        
        if (kvect->isString()) {
                //this is a vector of strings...
            for (long i = 0; i < thestr.size(); i++)
                kvect->storevalue(getfullchar(thestr, i));
            return kvect;
        }

		BLONG c;
        for (long i = 0; i < thestr.size(); i++) {
            c = getachar(thestr, i);
            kvect->storevalue(c);
        }
        
        return kvect;
    }

    size_t pos = 0;
    size_t found = 0;
    while (pos != string::npos) {
        found = s_find(thestr, thesplitter, pos);
        if (found != string::npos) {
            localvalue = thestr.substr(pos, found - pos);
            kvect->storevalue(localvalue);
            pos = found + thesplitter.size();
        }
        else
            break;
    }
    
    localvalue = thestr.substr(pos, thestr.size() - pos);
    if (localvalue != L"")
        kvect->storevalue(localvalue);
    
    return kvect;
}

Tamgu* Tamgua_ustring::MethodSlice(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* ke = Selectavector(contextualpattern);
    long num = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    if (num <= 0)
        return ke;
    
    long i;
    long beg = 0;
    
    wstring w = value.value();
    long sz = w.size();
    wstring sub;
    sz = w.size();
    for (i = num; i < sz; i+=num) {
        sub = w.substr(beg,i-beg);
        beg = i;
        ke->storevalue(sub);
    }
    if (i-beg>0) {
        sub = w.substr(beg,i-beg);
        ke->storevalue(sub);
    }
    return ke;
}

Tamgu* Tamgua_ustring::MethodNgrams(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long num = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    if (num <= 0)
        return Selectasvector(contextualpattern);
    
    long i, j, mx;
    
    wstring u;
    Tamgu* ke = Selectauvector(contextualpattern);
    
    wstring w = value.value();

    mx = value.size() - num + 1;
    for (i = 0; i < mx; i++) {
        u = L"";
        for (j = i; j < i + num; j++)
            u += w[j];
        ke->storevalue(u);
    }
    return ke;
}

Tamgu* Tamgua_ustring::MethodLatin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring w = value.value();
    return globalTamgu->Providestring(conversion_unicode_to_latin(w));
}


Tamgu* Tamgua_ustring::MethodDos(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring w = value.value();
    return globalTamgu->Providestring(s_unicode_to_dos(w));
}

Tamgu* Tamgua_ustring::MethodDostoutf8(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //First parameter is a string
    agnostring thestr(UString());
    return globalTamgu->Providestring(thestr.dostoutf8());
}


Tamgu* Tamgua_ustring::MethodLeft(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //First parameter is a string
    long nbchar = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    wstring w = value.value();
    return new Tamgua_ustring(s_left(w, nbchar));
}

Tamgu* Tamgua_ustring::MethodRight(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //First parameter is a string
    long nbchar = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    wstring w = value.value();
    return new Tamgua_ustring(s_right(w, nbchar));
}

Tamgu* Tamgua_ustring::MethodMid(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //First parameter is a string
        //Second parameter is a position
    long pos = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        //Third parameter is a number of characters
    long nbchar = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    wstring w = value.value();
    return new Tamgua_ustring(s_middle(w, pos, nbchar));
}

Tamgu* Tamgua_ustring::MethodIslower(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring w = value.value();
    return booleantamgu[s_is_lower(w)];
}

Tamgu* Tamgua_ustring::MethodIsupper(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring w = value.value();
    return booleantamgu[s_is_upper(w)];
}

Tamgu* Tamgua_ustring::MethodIsemoji(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring w = value.value();
    return booleantamgu[s_is_emoji(w)];
}

Tamgu* Tamgua_ustring::MethodEmoji(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring w = value.value();
    return globalTamgu->Providestring(Emoji(w));
}


Tamgu* Tamgua_ustring::MethodIsdigit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //First parameter is a string
    wstring w = value.value();
    return booleantamgu[s_is_digit(w)];
}

Tamgu* Tamgua_ustring::MethodIsalpha(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //First parameter is a string
    wstring w = value.value();
    return booleantamgu[s_is_alpha(w)];
}

Tamgu* Tamgua_ustring::MethodIsconsonant(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //First parameter is a string
    wstring w = value.value();
    return booleantamgu[s_is_consonant(w)];
}

Tamgu* Tamgua_ustring::MethodIsvowel(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //First parameter is a string
    wstring w = value.value();
    return booleantamgu[s_is_vowel(w)];
}

Tamgu* Tamgua_ustring::MethodDeaccentuate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //First parameter is a string
    wstring w = value.value();
    return new Tamgua_ustring(s_deaccentuate(w));
}

Tamgu* Tamgua_ustring::MethodIspunctuation(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //First parameter is a string
    wstring w = value.value();
    return booleantamgu[s_is_punctuation(w)];
}

Tamgu* Tamgua_ustring::MethodLower(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring w = value.value();
    return new Tamgua_ustring(s_to_lower(w));
}

Tamgu* Tamgua_ustring::MethodUpper(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring w = value.value();
    return new Tamgua_ustring(s_to_upper(w));
}

Tamgu* Tamgua_ustring::MethodTrim(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring str = value.value();
    return new Tamgua_ustring(Trim(str));
}

Tamgu* Tamgua_ustring::MethodTrimright(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring str = value.value();
    return new Tamgua_ustring(Trimright(str));
}

Tamgu* Tamgua_ustring::MethodTrimleft(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring str = value.value();
    return new Tamgua_ustring(Trimleft(str));
}

Tamgu* Tamgua_ustring::MethodPadding(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* res = callfunc->Evaluate(0, contextualpattern, idthread);
    Tamgu* kval = callfunc->Evaluate(1, contextualpattern, idthread);
    bool direction = true;
    if (callfunc->Size() == 3)
        direction = callfunc->Evaluate(2, contextualpattern, idthread)->Boolean();
    
    
    wstring sval = kval->UString();
    long sz = res->Integer() - Size();
    if (sz <= 0)
        return this;
    
    wstring s;
    while (sz > 0) {
        s += sval;
        sz--;
    }
    
    if (direction)
        value.concat(s);
    else
        value = s + value.value();
    
    return this;
}

Tamgu* Tamgua_ustring::MethodFill(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* res = callfunc->Evaluate(0, contextualpattern, idthread);
    Tamgu* kval = callfunc->Evaluate(1, contextualpattern, idthread);
    long sz = res->Integer();
    if (sz <= 0)
        return new Tamgua_ustring(L"");
    
    wstring sval = kval->UString();
    
    wstring w = value.value();
    value = L"";
    while (sz > 0) {
        w += sval;
        sz--;
    }
    value = w;
    return this;
}

Tamgu* Tamgua_ustring::MethodExtract(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    
    Tamgu* pos = callfunc->Evaluate(0, contextualpattern, idthread);
    Tamgu* from = callfunc->Evaluate(1, contextualpattern, idthread);
    vector<wstring> v;
    long i;
    for (i = 3; i < callfunc->Size(); i++)
        v.push_back(callfunc->Evaluate(i, contextualpattern, idthread)->UString());
    
    wstring init = value.value();
    
    i = pos->Integer();
    wstring sfrom = from->UString();
    i = init.find(sfrom, i);
    long lgfrom = sfrom.size();
    Tamgu* vect = Selectauvector(contextualpattern);
    long nb, npos;
    
    while (i != -1) {
        npos = -1;
        for (long j = 0; j < v.size(); j++) {
            long spos = init.find(v[j], i + lgfrom);
            if (spos != -1 && (npos == -1 || spos < npos))
                npos = spos;
        }
        if (npos == -1)
            break;
        nb = npos - i - lgfrom;
        vect->storevalue(init.substr(i + lgfrom, nb));
        i = init.find(sfrom, npos + 1);
    }
    
    return vect;
}

Tamgu* Tamgua_ustring::MethodReverse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring w = value.value();
    return new Tamgua_ustring(s_revert(w));
}


Tamgu* Tamgua_ustring::MethodRemovefirst(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* knb = callfunc->Evaluate(0, contextualpattern, idthread);
    long id = knb->Integer();
    
        //On retire en tete
    wstring w = value.value();
    long iv = w.size();
    if (id >= iv)
        return aEMPTYUSTRING;
    
    return new Tamgua_ustring(w.substr(id, iv-id));
}

Tamgu* Tamgua_ustring::MethodRemovelast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* knb = callfunc->Evaluate(0, contextualpattern, idthread);
    long id = knb->Integer();
    
    wstring w = value.value();
    long iv = w.size();
    if (id >= iv)
        return aEMPTYUSTRING;
    
    return new Tamgua_ustring(s_left(w, iv - id));
}

Tamgu* Tamgua_ustring::MethodFormat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    
    wchar_t buffer[101];
    wstring sformat = value.value();
        //In this case, we might have more than two parameters...
    wstring e;
    buffer[0] = L'%';
    for (long i = callfunc->Size() - 1; i >= 0; i--) {
        e = callfunc->Evaluate(i, contextualpattern, idthread)->UString();
        swprintf_s(buffer + 1, 100, L"%d", i + 1);
        sformat = s_replacestring(sformat, buffer, e);
    }
    return new Tamgua_ustring(sformat);
    
}

Tamgu* Tamgua_ustring::MethodReplace(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //Replace an element with another
    Tamgu* treg = callfunc->Evaluate(0, contextualpattern, idthread);
    if (treg->isRegular()) {
        wstring rep = callfunc->Evaluate(1, contextualpattern, idthread)->UString();
        
        
        wstring w = value.value();

        if (treg->Type() == a_treg) {
            vector<long> values;
            
            treg->searchall(w,values);
            
            for (long i=values.size()-2; i>=0; i-=2)
                w=w.substr(0,values[i])+rep+w.substr(values[i+1],w.size()-values[i+1]);
        }
#ifdef Tamgu_REGEX
        else
            w = treg->wreplace(w, rep);
#endif
        
        return new Tamgua_ustring(w);
    }
    
    wstring str = value.value();
    wstring reg = treg->UString();
    if (reg == L"")
        return new Tamgua_ustring(str);
    
    wstring rep = callfunc->Evaluate(1, contextualpattern, idthread)->UString();
    
    str = s_replacestring(str, reg, rep);
    return new Tamgua_ustring(str);
}

Tamgu* Tamgua_ustring::MethodEditdistance(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* e2 = callfunc->Evaluate(0, contextualpattern, idthread);
    return globalTamgu->Provideint(EditDistance(e2));
}


Tamgu* Tamgua_ustring::MethodLast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //First parameter should be a vector
    if (value.isempty())
        return new Tamgua_ustring(L"");
    
    wstring w = value.value();
    w = s_right(w, 1);
    return new Tamgua_ustring(w);
}

    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------

Tamgu* Tamgua_ustring::MethodEvaluate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring w;
    wstring wl = value.value();
    
    EvaluateMetaInString(w, wl);
    
    return new Tamgua_ustring(w);
}


void XNBrowse(x_node* xn, Tamgu* kf);
Tamgu* Tamgua_ustring::MethodParse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    x_reading xr;
    bnf_tamgu bnf;
    
    x_node* xn;
    
    string str = String();
    xr.tokenize(str);
    if (xr.size() == 0)
        return Selectavector(contextualpattern);

    xn = bnf.x_parsing(&xr, FULL, false);
    string message;
    if (xn == NULL) {
        if (bnf.errornumber != -1) {
            string msg("EVL(100):");
            msg += bnf.x_errormsg(bnf.errornumber);
            char ch[20];
            sprintf_s(ch, 20, " (line:%ld)", bnf.lineerror);
            msg += ch;
            return globalTamgu->Returnerror(msg, idthread);
        }
        return globalTamgu->Returnerror("Unknown expression", idthread);
    }
    
    Tamgu* kvect = Selectavector(contextualpattern);
    XNBrowse(xn, kvect);
    return kvect;
}

Tamgu* Tamgua_ustring::MethodParenthetic(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string str = String();
    string copen("(");
    string cclose(")");
    long nbparams = callfunc->Size();
    vector<string> rules;
    
    if (nbparams == 0)
        return globalTamgu->EvaluateParenthetic(str, copen, cclose, false, false, false, rules, idthread);
    
    copen = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    cclose = callfunc->Evaluate(1, contextualpattern, idthread)->String();
    if (copen.size() != 1 || cclose.size() != 1)
        return globalTamgu->Returnerror("TAMGUI(831): Wrong opening or closing character in LISP", idthread);
    
    bool comma = false;
    bool separator = false;
    bool keeprc = false;
    
    if (nbparams >= 3) {
        comma = callfunc->Evaluate(2, contextualpattern, idthread)->Boolean();
        if (nbparams >= 4) {
            separator = callfunc->Evaluate(3, contextualpattern, idthread)->Boolean();
            if (nbparams >= 5) {
                keeprc = callfunc->Evaluate(4, contextualpattern, idthread)->Boolean();
                if (nbparams == 6) {
                    Tamgu* vect = callfunc->Evaluate(5, contextualpattern, idthread);
                    for (long i = 0; i< vect->Size(); i++)
                        rules.push_back(vect->getstring(i));
                }
            }
        }
    }
    
    return globalTamgu->EvaluateParenthetic(str, copen, cclose, comma, separator, keeprc, rules, idthread);
}

Tamgu* Tamgua_ustring::MethodTags(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string str = String();
    string copen("(");
    string cclose(")");
    
    long nbparams = callfunc->Size();
    
    vector<string> rules;
    copen = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    cclose = callfunc->Evaluate(1, contextualpattern, idthread)->String();
    if (copen.size() != 1 || cclose.size() != 1)
        return globalTamgu->Returnerror("TAMGUI(832): Wrong opening or closing character in TAGS", idthread);
    
    bool comma = false;
    bool separator = false;
    bool keeprc = false;
    
    if (nbparams >= 3) {
        comma = callfunc->Evaluate(2, contextualpattern, idthread)->Boolean();
        if (nbparams >= 4) {
            separator = callfunc->Evaluate(3, contextualpattern, idthread)->Boolean();
            if (nbparams >= 5) {
                keeprc = callfunc->Evaluate(4, contextualpattern, idthread)->Boolean();
                if (nbparams == 6) {
                    Tamgu* vect = callfunc->Evaluate(5, contextualpattern, idthread);
                    for (long i = 0; i< vect->Size(); i++)
                        rules.push_back(vect->getstring(i));
                }
            }
        }
    }
    
    return globalTamgu->EvaluateTags(str, copen, cclose, comma, separator, keeprc, rules, idthread);
}


Tamgu* Tamgua_ustring::MethodScan(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    
    Tamgu* rgx = callfunc->Evaluate(0, contextualpattern, idthread);
    Au_automate* a = getautomate(rgx);
    if (a == NULL)
        return globalTamgu->Returnerror("Wrong grammar definition");
    
    wstring reg = value.value();
    
    bool fsep=false;
    wstring sep = L"";
    bool immediate=false;
    if (callfunc->Size() > 1) {
        sep = callfunc->Evaluate(1, contextualpattern, idthread)->UString();
        fsep=true;
        if (callfunc->Size() >= 3)
            immediate = callfunc->Evaluate(2, contextualpattern, idthread)->Boolean();
    }
    
    if (fsep || contextualpattern->isVectorContainer()) {
        
        Tamgu* vect = Selectavector(contextualpattern);
        
        vector<long> values;
        
        if (fsep) {
            wstring wlocal=reg;
            
            values.push_back(0);
            long sz = reg.size();
            
            for (long i =0; i< sz; i++) {
                values[0] = i;
                a->find(reg,sep, values);
                
                if (values.size()>1) {
                    if (i) {
                        for (long dx =1; dx < values.size();dx++)
                            values[dx] += i;
                    }
                    break;
                }
                else
                    if (immediate)
                        return vect;
                
                reg=reg.c_str()+1;
            }
            
            if (values.size() <= 1)
                return vect;
            
            Locking _vlock((TamguObject*)vect);
            
            if (contextualpattern->isNumber()) {
                    //This a number vector, we return the positions...
                for (long i = 0; i< values.size(); i++)
                    vect->storevalue(values[i]);
                return vect;
            }
            
            wstring sub;
            for (long i=0; i< values.size() -1; i++) {
                sub = wlocal.substr(values[i], values[i+1]-values[i] -1);
                if (sub != L"")
                    vect->storevalue(sub);
            }
            if (callfunc->Size() == 4) {
                Tamgu* s = callfunc->Evaluate(3,contextualpattern, idthread);
                s->storevalue(wlocal.substr(values.back(), wlocal.size()-values.back()));
            }
            return vect;
            
        }
        
        a->searchall(reg,values);
        
        Locking _vlock((TamguObject*)vect);
        
        if (contextualpattern->isNumber()) {
                //This a number vector, we return the positions...
            for (long i = 0; i< values.size(); i++)
                vect->storevalue(values[i]);
            return vect;
        }
        
        wstring sub;
        for (long i=0; i< values.size(); i+=2) {
            sub = reg.substr(values[i], values[i+1]-values[i]);
            if (sub != L"")
                vect->storevalue(sub);
        }
        return vect;
    }
    
    
    if (contextualpattern->isNumber() && contextualpattern->isAtom()) {
        long res,end;
        a->search(reg,res,end);
        return globalTamgu->Provideint(res);
    }
    
    if (contextualpattern->isString()) {
        long res,end;
        a->search(reg,res,end);
        reg = reg.substr(res, end-res);
        return new Tamgua_ustring(reg);
    }
    
    return booleantamgu[a->match(reg)];
}

Tamgu* Tamgua_ustring::MethodHash(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static const std::collate<char>& coll = std::use_facet<std::collate<char> >(loc);
    string s = String();
    if (s == "")
        return aNULL;
    long myhash = coll.hash(s.data(), s.data() + s.length());
    return globalTamgu->Provideint(myhash);
}

Tamgu* Tamgua_ustring::MethodBase(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static vector<wstring> caracs;
    static hash_bin<wchar_t, long> mcaracs;
    wstring w;
    long n, b = -1;
    
    Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
    bool toconvert=true;
    if (callfunc->Size() == 2)
        toconvert = callfunc->Evaluate(1, contextualpattern, idthread)->Boolean();
    
        //We can reset the chararacter list...
    if (ke->isVectorContainer()) {
        caracs.clear();
        mcaracs.clear();
        b = ke->Size();
        for (n = 0; n < b; n++) {
            w = ke->getustring(n);
            mcaracs[w[0]] = caracs.size();
            caracs.push_back(w);
        }
        if (b)
            return globalTamgu->Provideint(b);
    }
    
    if (caracs.size() == 0) {
        w = L"0";
        for (n = 0; n < 10; n++) {
            mcaracs[w[0]] = caracs.size();
            caracs.push_back(w);
            w[0]++;
        }
        w = L"A";
        for (n = 10; n < 36; n++) {
            mcaracs[w[0]] = caracs.size();
            caracs.push_back(w);
            w[0]++;
        }
        w = L"a";
        for (n = 36; n < 62; n++) {
            mcaracs[w[0]] = caracs.size();
            caracs.push_back(w);
            w[0]++;
        }
        w = L"#";
        mcaracs[w[0]] = caracs.size();
        caracs.push_back(w);
        w = L"@";
        mcaracs[w[0]] = caracs.size();
        caracs.push_back(w);
        w = L"";
        if (!b)
            return aTRUE;
    }
    
    b = ke->Integer();
    
    if (b > caracs.size()) {
        char message[1024];
        sprintf_s(message, 1024, "TAMGUI(510): base too large, max is: %ld", caracs.size());
        return globalTamgu->Returnerror(message, idthread);
    }
    
    BLONG v = 0;
    if (!toconvert) {
            //we convert a base 10 number into the local base
        wstring res=UString();
        v=convertlong(res);
        long rest;
        res = L"";
        while (v) {
            rest = v%b;
            v /=b;
            res = caracs[rest]+res;
        }
        return new Tamgua_ustring(res);
    }
    
    w = value.value();
    wchar_t wc;
    for (n = 0; n < w.size(); n++) {
        wc = w[n];
        if (mcaracs.check(wc) == false || mcaracs[wc] >= b)
            return globalTamgu->Returnerror("TAMGUI(511): Cannot analyze this string in this base.", idthread);
        
        v *= b;
        v += mcaracs[wc];
    }
    return globalTamgu->Provideint(v);
}

Tamgu* Tamgua_ustring::MethodInsert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    size_t idx;
    
        //First parameter should be a vector
    
        //second parameter is anything
    idx = -1;
    string val;
    agnostring ret(UString());
    if (callfunc->Size() == 2) {
        idx = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        val = callfunc->Evaluate(1, contextualpattern, idthread)->String();
    }
    else
        val = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    
    if (idx == -1)
        ret = ret.insertseparator(val);
    else {
        if (ret.insert(val, idx) == false)
            return aNULL;
    }
    return new Tamgua_ustring(ret.utf8tounicode());
}

Tamgu* Tamgua_ustring::MethodClear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    value.clear();
    return aTRUE;
}


#ifdef WSTRING_IS_UTF16
void Tamgua_ustring::Pop(long i) {
    if (value.isempty())
        return;
    wstring w = value.value();
    long sz = size_w(w);
    if (!sz)
        return;
    if (i == -1) {
            //the last character
        i = convertchartopos(w, 0, sz-1);
        while (i < w.size())
            w.pop_back();
        value = w;
        return;
    }
    
    if (i < 0 || i >= sz)
        return;
    
    long j = convertchartopos(w, 0, i);
    if (j == i) {
        w.erase(i, 1);
        value = w;
        return;
    }
    i = j;
    if (c_is_emoji(getachar(w, j))) {
        j++;
        while (c_is_emojicomp(getachar(w, j)))
            j++;
    }
    else
        j++;
    w.erase(i, j-i);
    value = w;
}
#endif

Tamgu* Tamgua_ustring::MethodPop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //second parameter is an integer (the number of pop)
    long i = 0;
    long nb = 1;
    if (callfunc->Size() == 2)
        nb = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    if (callfunc->Size() >= 1)
        i = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    else
        i = -1;
    
    while (nb > 0) {
        Pop(i);
        nb--;
    }
    
    return aTRUE;
}


Tamgu* Tamgua_ustring::Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {
    Tamgu* a;
    
    Tamguustring* it = globalTamgu->Provideustring(L"");
    it->Setreference();
    
    uchar addvalue = 0;
    
    if (context != aNOELEMENT)
        addvalue = Selecttype(context);
    
    recipient->Putvalue( it, idthread);
    
    wstring wvalue = value.value();
    
    for (long i = 0; i < wvalue.size(); i++) {
        it->value = getfullchar(wvalue, i);

        a = bd->DirectEval(environment, aNULL, idthread);
        if (a->isNULL())
            continue;
        
        if (globalTamgu->Error(idthread) || a->needInvestigate()) {
            if (a == aBREAK)
                break;
            it->Resetreference();
            recipient->Forcedclean();
            a->Release();
            context->Release();
            return globalTamgu->Errorobject(idthread);
        }
        
        context = Storealongtype(context, a, idthread, addvalue);
        a->Release();
    }
    
    it->Resetreference();
    recipient->Forcedclean();
    
    return context;
}

Tamgu* Tamgua_ustring::Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct) {
    
    Tamgu* returnval;
    
    bool first = false;
    Tamguustring* key = globalTamgu->Provideustring(L"");
    key->Setreference();
    
    if (init != aNOELEMENT) {
        accu->Putvalue(init, idthread);
        if (kcont != NULL) {
            if (direct)
                kcont->Insert(0, init);
            else
                kcont->Push(init);
        }
    }
    else
        first = true; //we are dealing with a foldr1 or a foldl1
    
    wstring wvalue = value.value();
    
    for (long i = 0; i < wvalue.size(); i++) {
        key->value = getfullchar(wvalue, i);
        
        if (first) {
            returnval = key->Atom();//We use the first value of the list to seed our accumulator variable
            first = false;
        }
        else {
            var->Putvalue(key, idthread);
            returnval = bd->DirectEval(env, aNULL, idthread);
            
            if (returnval == aBREAK) {
                accu = returnval;
                break;
            }
            
            if (globalTamgu->Error(idthread)) {
                env->Cleaning(idthread);
                key->Resetreference();
                var->Forcedclean();
                if (kcont != NULL)
                    kcont->Release();
                return globalTamgu->Errorobject(idthread);
            }
        }
        
        if (returnval != aNULL) {
            
            accu->Putvalue(returnval, idthread);
            if (kcont != NULL) {
                if (direct)
                    kcont->Insert(0, returnval);
                else
                    kcont->Push(returnval);
            }
            returnval->Release();
        }
    }
    
    key->Resetreference();
    var->Forcedclean();
    if (kcont != NULL)
        return kcont;
    return accu->Value();
}


Tamgu* Tamgua_ustring::andset(Tamgu* a, bool autoself) {
    wstring s = a->UString();
    wstring u;
    wstring w = value.value();
    long m = minlocal(s.size(), w.size());
    for (long i = 0; i < m; i++) {
        if (s[i] == w[i])
            u += s[i];
    }
    if (autoself) {
        value = u;
        return this;
    }
    
    return new Tamgua_ustring(u);
}

Tamgu* Tamgua_ustring::xorset(Tamgu* a, bool autoself) {
    wstring s = a->UString();
    wstring u;
    wstring w = value.value();
    long m = minlocal(s.size(), w.size());
    for (long i = 0; i < m; i++) {
        if (s[i] != w[i])
            u += w[i];
    }
    if (autoself) {
        value = u;
        return this;
    }
    
    return new Tamgua_ustring(u);
}
