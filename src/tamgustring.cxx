/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgustring.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "codeparse.h"
#include "tamgu.h"

#include "tamgutaskell.h"
#include "tamgustring.h"
#include "constobjects.h"
#include "tamguivector.h"
#include "tamgufvector.h"
#include "tamgubvector.h"
#include "tamgusvector.h"
#include "tamguvector.h"
#include "tamgulvector.h"
#include "tamguuvector.h"
#include "tamguconstants.h"
#include "tamgubyte.h"
#include "tamguustring.h"
#include "fractalhmap.h"
#include "instructions.h"
#include "tamgumap.h"
#include "compilecode.h"
#include "tamguautomaton.h"
#include "tamgufile.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//We need to declare once again our local definitions.
Exporting basebin_hash<stringMethod>  Tamgustring::methods;
Exporting hmap<string, string> Tamgustring::infomethods;
Exporting bin_hash<unsigned long> Tamgustring::exported;

Exporting short Tamgustring::idtype = 0;

#ifdef DOSOUTPUT
static bool dosoutput = true;
static void setdosoutput(bool d) { dosoutput = d; }
#define conversion2Dos(x) dosoutput?s_utf8_to_dos(USTR(x)):x
#endif


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgustring::AddMethod(TamguGlobal* global, string name, stringMethod func, unsigned long arity, string infos, short returntype) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
    
    if (returntype != a_null)
        global->returntypes[idname] = returntype;
}



    void Tamgustring::Setidtype(TamguGlobal* global) {
        Tamgustring::idtype = global->Getid("string");
    }

   bool Tamgustring::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    Tamgustring::idtype = global->Getid("string");

    Tamgustring::AddMethod(global, "succ", &Tamgustring::MethodSucc, P_NONE, "succ(): Return the successor of a character.", a_string);
    Tamgustring::AddMethod(global, "pred", &Tamgustring::MethodPred, P_NONE, "pred(): Return the predecessor of a byte.", a_string);

    Tamgustring::AddMethod(global, "doublemetaphone", &Tamgustring::MethodDoubleMetaphone, P_NONE, "doublemetaphone(): Return the string double metaphone conversion into a svector", a_svector);

    Tamgustring::AddMethod(global, "hash", &Tamgustring::MethodHash, P_NONE, "hash(): Return the hash value of a string.", a_int);
    Tamgustring::AddMethod(global, "ord", &Tamgustring::MethodOrd, P_NONE, "ord(): return the ASCII code of the first character, or a list of all ASCII code if the recipient is a vector", a_null);
    Tamgustring::AddMethod(global, "bytes", &Tamgustring::MethodBytes, P_NONE, "bytes(): Return the string as a vector of bytes", a_ivector);
    Tamgustring::AddMethod(global, "base", &Tamgustring::MethodBase, P_ONE | P_TWO, "base(int b, bool bool toconvert=true): Return the value corresponding to the string in base b", a_null);
    Tamgustring::AddMethod(global, "parse", &Tamgustring::MethodParse, P_NONE | P_TWO, "parse(): Parse a string as a piece of code and returns the evaluation as a vector.", a_vector);
    Tamgustring::AddMethod(global, "sizeb", &Tamgustring::MethodSizeb, P_NONE, "sizeb(): Return the size in bytes of the string", a_int);
    Tamgustring::AddMethod(global, "lisp", &Tamgustring::MethodLisp, P_NONE | P_TWO | P_THREE | P_FOUR | P_FIVE | P_SIX, "lisp(): lisp(string o,string c,bool comma,bool separator,bool concatenate): Parse a string as a parenthetic expressions, o is '(' and c is ')' by default. If 'comma' is true, then the decimal character is ',' otherwise it is '.'. If 'separator' is true then '1,000' is accepted as a number. If 'concatenate' is true then '3a' is a valid token", a_null);
    Tamgustring::AddMethod(global, "tags", &Tamgustring::MethodTags, P_TWO | P_THREE | P_FOUR | P_FIVE | P_SIX, "tags(string o,string c,bool comma,bool separator,bool concatenate, svector rules): Parse a string as a parenthetic expressions, where o and c are string (not characters). If 'comma' is true, then the decimal character is ',' otherwise it is '.'. If 'separator' is true then '1,000' is accepted as a number. If 'concatenate' is true then '3a' is a valid token", a_null);
    Tamgustring::AddMethod(global, "scan", &Tamgustring::MethodScan, P_ONE | P_TWO | P_THREE | P_FOUR, "scan(sub, string sep, bool immediate,string remaining): Find the substrings matching sub, with TRE. 'sep' is a separator between strings. 'immediate' always combines with separator, it means that the matching should start at the first character of the string, default is false. 'remaining' also combines with 'separator', it returns the rest of the string after the section that matched.", a_null);
    Tamgustring::AddMethod(global, "getstd", &Tamgustring::MethodGetstd, P_ONE, "getstd(bool): catch or release the standard output", a_null);
    Tamgustring::AddMethod(global, "geterr", &Tamgustring::MethodGeterr, P_ONE, "geterr(bool): catch or release the error output", a_null);

    Tamgustring::AddMethod(global, "levenshtein", &Tamgustring::MethodEditdistance, P_ONE | P_TWO, "levenshtein(string s,bool byte): Return the edit distance with 's' according to Levenshtein algorithm. If byte is true, force a byte level comparison. byte is optionnal.", a_int);
    Tamgustring::AddMethod(global, "editdistance", &Tamgustring::MethodEditdistance, P_ONE | P_TWO | P_THREE, "editdistance(string s,bool byte): Return the edit distance with 's'. If byte is true, force a byte level comparison. byte is optionnal.", a_int);

    Tamgustring::AddMethod(global, "tokenize", &Tamgustring::MethodTokenize, P_NONE | P_ONE | P_TWO | P_THREE | P_FOUR, "tokenize(bool comma,bool separator,bool concatenate, svector rules): Segment a string into words and punctuations. If 'comma' is true, then the decimal character is ',' otherwise it is '.'. If 'separator' is true then '1,000' is accepted as a number. If 'concatenate' is true then '3a' is a valid token. rules is a set of tokenization rules that can be first initialized then modified with _getdefaulttokenizerules", a_null);
    Tamgustring::AddMethod(global, "stokenize", &Tamgustring::MethodStokenize, P_NONE | P_ONE, "stokenize(map keeps): Segment a string into words and punctuations, with a keep.", a_null);
    Tamgustring::AddMethod(global, "split", &Tamgustring::MethodSplit, P_ONE | P_NONE, "split(string splitter): split a string along splitter and store the results  in a vector. If splitter=='', then the string is split into a vector of characters", a_svector);
    Tamgustring::AddMethod(global, "multisplit", &Tamgustring::MethodMultiSplit, P_ATLEASTONE, "multisplit(string splitter1,string splitter2..): split a string along different splitters. Return a svector.", a_svector);
    Tamgustring::AddMethod(global, "splite", &Tamgustring::MethodSplite, P_ONE | P_NONE, "splite(string splitter): split a string along splitter and store the results  in a vector. If splitter=='', then the string is split into a vector of characters. Empty strings are kept in the result.", a_svector);
    Tamgustring::AddMethod(global, "slice", &Tamgustring::MethodSlice, P_ONE, "slice(int sz): Return a vector of all slices of size sz", a_svector);
    Tamgustring::AddMethod(global, "ngrams", &Tamgustring::MethodNgrams, P_ONE, "ngrams(int r): Return a vector of all ngrams of rank r", a_svector);
    Tamgustring::AddMethod(global, "extract", &Tamgustring::MethodExtract, P_ATLEASTTHREE, "extract(int pos,string from,string up1,string up2...): extract substrings between 'from' and 'up1'...'upn' (the shortest string is used). Return a vector of strings", a_svector);

    Tamgustring::AddMethod(global, "find", &Tamgustring::MethodFind, P_TWO | P_ONE, "find(string sub,int pos): Return the position of substring sub starting at position pos", a_int);
    Tamgustring::AddMethod(global, "rfind", &Tamgustring::MethodRfind, P_TWO | P_ONE, "rfind(string sub,int pos): Return the position of substring sub backward starting at position pos", a_int);

    Tamgustring::AddMethod(global, "count", &Tamgustring::MethodCount, P_THREE | P_TWO | P_ONE | P_NONE, "count(string sub,int pos,int mx): Count the number of substrings starting at position pos, ending at mx", a_int);
    Tamgustring::AddMethod(global, "byteposition", &Tamgustring::MethodByteposition, P_ONE, "byteposition(int pos): Convert a character position into a byte position", a_int);
    Tamgustring::AddMethod(global, "charposition", &Tamgustring::MethodCharposition, P_ONE, "charposition(int pos): Convert a byte position into a character position", a_int);

    Tamgustring::AddMethod(global, "isalpha", &Tamgustring::MethodIsalpha, P_NONE, "isalpha(): Test if a string only contains only alphabetical characters", a_boolean);
    Tamgustring::AddMethod(global, "isconsonant", &Tamgustring::MethodIsconsonant, P_NONE, "isconsonant(): Test if a string only contains consonants", a_boolean);
    Tamgustring::AddMethod(global, "isvowel", &Tamgustring::MethodIsvowel, P_NONE, "isvowel(): Test if a string only contains only vowels", a_boolean);
    Tamgustring::AddMethod(global, "ispunctuation", &Tamgustring::MethodIspunctuation, P_NONE, "ispunctuation(): Test if the character is a punctuation", a_boolean);
    Tamgustring::AddMethod(global, "isdigit", &Tamgustring::MethodIsdigit, P_NONE, "isdigit(): Test if a string only contains digits", a_boolean);
    Tamgustring::AddMethod(global, "isupper", &Tamgustring::MethodIsupper, P_NONE, "isupper(): Test if a string only contains uppercase characters", a_boolean);
    Tamgustring::AddMethod(global, "islower", &Tamgustring::MethodIslower, P_NONE, "islower(): Test if a string only contains lowercase characters", a_boolean);
    Tamgustring::AddMethod(global, "isemoji", &Tamgustring::MethodIsemoji, P_NONE, "isemoji(): Test if a string only contains emoji characters", a_boolean);
    Tamgustring::AddMethod(global, "isutf8", &Tamgustring::MethodIsutf8, P_NONE, "isutf8(): Return true is the string is encoded in UTF8", a_boolean);


    Tamgustring::AddMethod(global, "reverse", &Tamgustring::MethodReverse, P_NONE, "reverse(): reverse the string", a_string);
    Tamgustring::AddMethod(global, "format", &Tamgustring::MethodFormat, P_ATLEASTONE, "format(p1,p2,p3): Create a new string from the current string in which each '%x' is associated to one of the parameters, 'x' being the position of that parameter in the argument list. 'x' starts at 1.", a_string);
    Tamgustring::AddMethod(global, "fill", &Tamgustring::MethodFill, P_TWO, "fill(int nb,string c): create a string of nb characters c", a_string);
    Tamgustring::AddMethod(global, "padding", &Tamgustring::MethodPadding, P_TWO | P_THREE, "padding(int nb,string c,bool paddattheend): add nb characters c to the current string. Last parameter is optional", a_string);
    Tamgustring::AddMethod(global, "pop", &Tamgustring::MethodPop, P_NONE | P_ONE | P_TWO, "pop(): remove last character", a_string);
    Tamgustring::AddMethod(global, "evaluate", &Tamgustring::MethodEvaluate, P_NONE, "evaluate(): evaluate the meta-characters within a string and return the evaluated string.", a_string);
    Tamgustring::AddMethod(global, "html", &Tamgustring::MethodTohtml, P_NONE, "html(): Return the string into an HTML compatible string or as a vector of strings", a_string);
    Tamgustring::AddMethod(global, "tohtml", &Tamgustring::MethodTohtml, P_NONE, "tohtml(): Return the string into an HTML compatible string or as a vector of strings", a_string);
    Tamgustring::AddMethod(global, "toxml", &Tamgustring::MethodToxml, P_NONE, "toxml(): Return the string into an XML compatible string or as a vector of strings", a_string);
    Tamgustring::AddMethod(global, "replace", &Tamgustring::MethodReplace, P_TWO, "replace(sub,str): Replace the substrings matching sub with str", a_string);
    Tamgustring::AddMethod(global, "removefirst", &Tamgustring::MethodRemovefirst, P_ONE, "removefirst(int nb): remove the first nb characters of a string", a_string);
    Tamgustring::AddMethod(global, "removelast", &Tamgustring::MethodRemovelast, P_ONE, "removelast(int nb): remove the last nb characters of a string", a_string);
    Tamgustring::AddMethod(global, "utf8", &Tamgustring::MethodUtf8, P_NONE | P_ONE, "utf8(int table): convert a LATIN string into UTF8. table is optional, by default it is ISO/IEC 8859 part 1.", a_string);
    Tamgustring::AddMethod(global, "latin", &Tamgustring::MethodLatin, P_NONE, "latin(): convert an UTF8 string in LATIN", a_string);
    Tamgustring::AddMethod(global, "dos", &Tamgustring::MethodDos, P_NONE, "dos(): convert a string into DOS encoding", a_string);
    Tamgustring::AddMethod(global, "dostoutf8", &Tamgustring::MethodDostoutf8, P_NONE, "dostoutf8(): convert a DOS string into UTF8", a_string);
    Tamgustring::AddMethod(global, "left", &Tamgustring::MethodLeft, P_ONE, "left(int nb): return the first nb characters of a string", a_string);
    Tamgustring::AddMethod(global, "right", &Tamgustring::MethodRight, P_ONE, "right(int nb): return the last nb characters of a string", a_string);
    Tamgustring::AddMethod(global, "mid", &Tamgustring::MethodMid, P_TWO, "mid(int pos,int nb): return the nb characters starting at position pos of a string", a_string);
    Tamgustring::AddMethod(global, "emoji", &Tamgustring::MethodEmoji, P_NONE, "emoji(): Return the textual description of an emoji", a_string);
    Tamgustring::AddMethod(global, "upper", &Tamgustring::MethodUpper, P_NONE, "upper(): Return the string in upper characters", a_string);
    Tamgustring::AddMethod(global, "deaccentuate", &Tamgustring::MethodDeaccentuate, P_NONE, "deaccentuate(): Remove the accents from accented characters", a_string);
    Tamgustring::AddMethod(global, "indent", &Tamgustring::MethodIndent, P_NONE | P_ONE, "indent(int nbblanks): Format a piece of code.", a_string);
    Tamgustring::AddMethod(global, "lower", &Tamgustring::MethodLower, P_NONE, "lower(): Return the string in lower characters", a_string);
    Tamgustring::AddMethod(global, "trim", &Tamgustring::MethodTrim, P_NONE, "trim(): remove the trailing characters", a_string);
    Tamgustring::AddMethod(global, "trimleft", &Tamgustring::MethodTrimleft, P_NONE, "trimleft(): remove the trailing characters on the left", a_string);
    Tamgustring::AddMethod(global, "trimright", &Tamgustring::MethodTrimright, P_NONE, "trimright(): remove the trailing characters on the right", a_string);
    Tamgustring::AddMethod(global, "last", &Tamgustring::MethodLast, P_NONE, "last(): return last character", a_string);
    Tamgustring::AddMethod(global, "insert", &Tamgustring::MethodInsert, P_ONE | P_TWO, "insert(i,s): insert the string s at i. If i is -1, then insert s between each character in the input string", a_string);
    Tamgustring::AddMethod(global, "clear", &Tamgustring::MethodClear, P_NONE, "clear(): Clean the content of a string.", a_null);

    Tamgustring::AddMethod(global, "jamo", &Tamgustring::MethodJamo, P_NONE | P_ONE, "jamo(bool combine): if 'combine' is false split a Korean jamo into its main components, else combine contents into a jamo.", a_null);
    Tamgustring::AddMethod(global, "isjamo", &Tamgustring::MethodIsJamo, P_NONE, "isjamo(): return true if it is a Hangul jamo.", a_null);
    Tamgustring::AddMethod(global, "ishangul", &Tamgustring::MethodIsHangul, P_NONE, "ishangul(): return true if it is a Hangul character.", a_null);
    Tamgustring::AddMethod(global, "normalizehangul", &Tamgustring::MethodNormalizeHangul, P_NONE, "normalizehangul(): Normalize Hangul characters.", a_string);
    Tamgustring::AddMethod(global, "romanization", &Tamgustring::MethodTransliteration, P_NONE, "romanization(): romanization of Hangul characters.", a_null);

    Tamgustring::AddMethod(global, "read", &Tamgustring::MethodRead, P_ONE, "read(string path): read the file content into the current variable.", a_boolean);

    global->newInstance[Tamgustring::idtype] = new Tamgustring("", global);
    global->newInstance[a_stringthrough] = global->newInstance[Tamgustring::idtype];
    global->RecordMethods(Tamgustring::idtype, Tamgustring::exported);
    global->RecordMethods(a_stringthrough, Tamgustring::exported);
    global->RecordMethods(a_sloop, Tamgustring::exported);

    
    //Encoding table name...
    Tamgu* a = new TamguSystemVariable(global, aONE, global->Createid("e_latin_we"), a_short);
    a = new TamguSystemVariable(global, aTWO, global->Createid("e_latin_ce"), a_short);
    a = new TamguSystemVariable(global, aTHREE, global->Createid("e_latin_se"), a_short);
    a = new TamguSystemVariable(global, aFOUR, global->Createid("e_latin_ne"), a_short);
    a = new TamguSystemVariable(global, aFIVE, global->Createid("e_cyrillic"), a_short);
    a = new TamguSystemVariable(global, aSIX, global->Createid("e_arabic"), a_short);
    a = new TamguSystemVariable(global, aSEVEN, global->Createid("e_greek"), a_short);
    a = new TamguSystemVariable(global, aEIGHT, global->Createid("e_hebrew"), a_short);
    a = new TamguSystemVariable(global, aNINE, global->Createid("e_turkish"), a_short);
    a = new TamguSystemVariable(global, aTEN, global->Createid("e_nordic"), a_short);
    a = new TamguSystemVariable(global, aELEVEN, global->Createid("e_thai"), a_short);
    a = new TamguSystemVariable(global, aTHIRTEEN, global->Createid("e_baltic"), a_short);
    a = new TamguSystemVariable(global, aFOURTEEN, global->Createid("e_celtic"), a_short);
    a = new TamguSystemVariable(global, aFIFTEEN, global->Createid("e_latin_ffe"), a_short);
    a = new TamguSystemVariable(global, aSIXTEEN, global->Createid("e_latin_see"), a_short);
    a = new TamguSystemVariable(global, aSEVENTEEN, global->Createid("e_windows"), a_short);
    a = new TamguSystemVariable(global, aSEVENTEEN, global->Createid("e_cp1252"), a_short);

    Tamgua_string::InitialisationModule(global, version);
    
    return true;
}


Exporting TamguIteration* Tamgustring::Newiteration(bool direction) {
    Locking _lock(this);
    return new TamguIterationstring(value, direction);
}


Tamgu* Tamgustring::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    agnostring val(String());
    
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    Tamgustring v("");
    Tamgu* a;
    val.begin();
    while (!val.end()) {
        v.value =  val.nextemoji();
        var->Putvalue(&v, idthread);
        
        a = ins->instructions.vecteur[1]->Eval(context, aNULL, idthread);
        
            //Continue does not trigger needInvestigate
        if (a->needInvestigate()) {
            if (a == aBREAK)
                break;
            return a;
        }
        
        a->Release();
    }
    
    return this;
}

Tamgu* Tamgustring::Succ() {
    Locking _lock(this);
    if (value == "")
        return globalTamgu->Providestring("");

    wstring v;
    s_utf8_to_unicode(v, USTR(value), value.size());
    v[v.size() - 1] = v.back() + 1;
    return globalTamgu->Provideustring(v);
}

Tamgu* Tamgustring::Pred() {
    Locking _lock(this);
    if (value == "")
        return globalTamgu->Providestring("");

    wstring v;
    s_utf8_to_unicode(v, USTR(value), value.size());

    wchar_t c = v.back();
    if (c <= 1)
        return globalTamgu->Provideustring(v);

    v[v.size() - 1] = c - 1;
    return globalTamgu->Provideustring(v);
}


Exporting Tamgu* Tamgustring::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    string val = a->String();

    locking();
    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Locking* _vlock = _getlock(v);
        s_findall(value, val, v->values);
        v_convertbytetocharposition(USTR(value), v->values);
        _cleanlock(_vlock);
        unlocking();
        return v;
    }

    long r = s_find(value, val, 0);
    unlocking();

    if (context->isString()) {
        if (r==-1)
            return aEMPTYSTRING;
        return globalTamgu->Providestring(val);
    }

    if (context->isNumber())
        return globalTamgu->Provideint(r);

    if (r == -1)
        return aFALSE;

    return aTRUE;
}

Tamgu* TamguConstString::in(Tamgu* context, Tamgu* a, short idthread) {
        //Three cases along the container type...
        //It is a Boolean, we expect false or true
        //It is an integer, we expect a position in v
        //It is a container, we are looking for all positions...
    
    string val = a->String();
    
    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Locking* _vlock = _getlock(v);
        s_findall(value, val, v->values);
        v_convertbytetocharposition(USTR(value), v->values);
        _cleanlock(_vlock);
        return v;
    }
    
    long r = s_find(value, val, 0);
    
    if (context->isString()) {
        if (r==-1)
            return aEMPTYSTRING;
        return globalTamgu->Providestring(val);
    }
    
    if (context->isNumber())
        return globalTamgu->Provideint(r);
    
    if (r == -1)
        return aFALSE;
    
    return aTRUE;
}

Exporting unsigned long Tamgustring::EditDistance(Tamgu* e) {
    locking();
    string s2 = e->String();
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
    unlocking();
    return s2len;
}

Exporting Tamgu* Tamgustring::Put(Tamgu* idx, Tamgu* v, short idthread) {

    if (!idx->isIndex()) {
        locking();
        if (v == this) {
            unlocking();
            return aFALSE;
        }

        value = v->String();
        unlocking();
        return aTRUE;
    }

    string s;
    if (v != aNULL)
        s = v->String();

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
    if (s != "")
        value.insert(ileft, s);
    
    unlocking();
    return aTRUE;

}

Exporting Tamgu* Tamgustring::Eval(Tamgu* context, Tamgu* idx, short idthread) {
    if (!idx->isIndex() || context == idx)
        return this;

    long ileft, iright;
    locking();
    char res = StringIndexes(value, idx, ileft, iright, idthread);

    if (res == 0) {
        unlocking();
        if (globalTamgu->erroronkey)
            globalTamgu->Returnerror("Wrong key in a container or a string access", idthread);
        return aNOELEMENT;
    }

    if (res == 1)
        idx = globalTamgu->Providestring(c_char_get(USTR(value), ileft));
    else
        idx = globalTamgu->Providestring(value.substr(ileft, iright - ileft));
    unlocking();
    return idx;
}

//------------------------------------------------------------------------------------------------------------------

#ifdef WSTRING_IS_UTF16
Tamgu* Tamgustring::MethodOrd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	wstring s = UString();
	uint32_t c;
	if (s.size() >= 1) {
		if (contextualpattern->isVectorContainer() || s.size()>1) {
			Tamgu* kvect = SelectContainer(contextualpattern, idthread);
			if (kvect == NULL)
				kvect = new Tamgulvector;

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

		return new Tamgulong(c);
	}

	return aNULL;
}
#else
Tamgu* Tamgustring::MethodOrd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
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
        
        return new Tamgulong(s[0]);
    }
    
    return aNULL;
}
#endif
//------------------------------------------------------------------------------------------------------------------
Tamgu* Tamgustring::MethodIsutf8(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    locking();
    contextualpattern = booleantamgu[s_is_utf8(USTR(value), value.size())];
    unlocking();
    return contextualpattern;
}

Tamgu* Tamgustring::MethodEvaluate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string thestr;
    
    locking();
    EvaluateMetaInString(thestr,value);
    unlocking();

    return globalTamgu->Providestring(thestr);
}


void XNBrowse(x_node* xn, Tamgu* kf);
Tamgu* Tamgustring::MethodParse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    x_reading xr;
    bnf_tamgu bnf;
    
    locking();
    string str = value;
    unlocking();

    x_node* xn;
    
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

Tamgu* Tamgustring::MethodLisp(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    locking();
    string str = value;
    unlocking();

    string copen("(");
    string cclose(")");
    long nbparams = callfunc->Size();
    vector<string> rules;
    
    if (nbparams == 0)
        return globalTamgu->EvaluateLisp(str, copen, cclose, false, false, false, rules, idthread);
    
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
    
    return globalTamgu->EvaluateLisp(str, copen, cclose, comma, separator, keeprc, rules, idthread);
}

Tamgu* Tamgustring::MethodTags(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    locking();
    string str = value;
    unlocking();

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

Tamgu* Tamgustring::MethodScan(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    
    Tamgu* rgx = callfunc->Evaluate(0, contextualpattern, idthread);
    Au_automate* a = getautomate(rgx);
    if (a == NULL)
        return globalTamgu->Returnerror("Wrong grammar definition");
    
    wstring reg;
    
    locking();
    sc_utf8_to_unicode(reg, USTR(value), value.size());
    unlocking();

    bool fsep = false;
    wstring sep = L"";
    bool immediate=false;
    if (callfunc->Size() > 1) {
        sep = callfunc->Evaluate(1, contextualpattern, idthread)->UString();
        fsep = true;
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
            wstring sub;
            for (long i=0; i< values.size() -1; i++) {
                sub = wlocal.substr(values[i], values[i+1]-values[i]-1);
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
        return globalTamgu->Provideustring(reg);
    }
    
    return booleantamgu[a->match(reg)];
}

Tamgu* Tamgustring::MethodCount(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long i = 0, nb = 0;

    Tamgu* substr = callfunc->Evaluate(0, contextualpattern, idthread);
    long mx = -1;
    if (callfunc->Size() >= 2) {
        i = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
        if (callfunc->Size() == 3)
            mx = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
    }
    
    if (substr->isRegular()) {
        wstring w = UString();
        if (mx != -1)
            w = w.substr(i, mx-i);
        else
            if (i)
                w = w.substr(i, w.size() - i);
        
        vector<long> values;
        substr->searchall(w,values);
        return globalTamgu->Provideint(values.size()>>1);
    }

    agnostring str(String());
    if (str == "")
        return aZERO;

    string sub = substr->String();
    i = str.find(sub, i);
    while (i != -1) {
        if (mx != -1 && i >= mx)
            break;
        nb++;
        i++;
        i = str.find(sub, i);
    }

    return globalTamgu->Provideint(nb);
}

Tamgu* Tamgustring::MethodFind(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* ksub = callfunc->Evaluate(0, contextualpattern, idthread);

    if (!Size()) {
        if (contextualpattern->isVectorContainer())
            return Selectavector(contextualpattern);
        return aMINUSONE;
    }

    //we search for a substring starting at position idx
    long i = 0;
    if (callfunc->Size() == 2)
        i = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();

    if (contextualpattern->isVectorContainer()) {
        Tamgu* kvect = Selectavector(contextualpattern);
        long j;

        wstring str = UString();

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
        wstring str = UString();
        long e;
        if (!ksub->search(str,i,e,i))
            return aMINUSONE;
        return globalTamgu->Provideint(i);
    }

    locking();
    string str = value;
    unlocking();

    string sub = ksub->String();
    i = s_find(str, sub, i);
    
    if (i == -1)
        return aMINUSONE;

    return globalTamgu->Provideint(i);
}

Tamgu* Tamgustring::MethodRfind(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* ksub = callfunc->Evaluate(0, contextualpattern, idthread);
    
    //we search for a substring starting at position idx
    long i = Size();

    if (!i) {
        if (contextualpattern->isVectorContainer())
            return Selectavector(contextualpattern);
        return aMINUSONE;
    }
    
    if (callfunc->Size() == 2)
        i = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    
    if (contextualpattern->isVectorContainer()) {
        Tamgu* kvect = Selectavector(contextualpattern);
        long j;
        
        wstring str = UString();

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
    

    if (ksub->isRegular()) {
        wstring str=UString();
        long e;
        if (!ksub->searchlast(str,i,e,i))
            return aMINUSONE;
        return globalTamgu->Provideint(i);
    }
    
    string str=String();

    string sub = ksub->String();
    i = s_rfind(str, sub, i);

    if (i == -1)
        return aMINUSONE;

    return globalTamgu->Provideint(i);
}

Tamgu* Tamgustring::MethodByteposition(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    locking();
    contextualpattern = globalTamgu->Provideint(c_chartobyteposition(USTR(value),callfunc->Evaluate(0, contextualpattern, idthread)->Integer()));
    unlocking();
    return contextualpattern;
}

Tamgu* Tamgustring::MethodCharposition(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    locking();
    contextualpattern = globalTamgu->Provideint(c_bytetocharposition(USTR(value),callfunc->Evaluate(0, contextualpattern, idthread)->Integer()));
    unlocking();
    return contextualpattern;
}

Tamgu* Tamgustring::MethodTokenize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    locking();
    agnostring thestr(value);
    unlocking();
    
    bool comma = false;
    bool separator = false;
    bool keepwithdigit = false;
    
    vector<string> rules;
    if (callfunc->Size() >= 1) {
        comma = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
        if (callfunc->Size() >= 2) {
            separator = callfunc->Evaluate(1, contextualpattern, idthread)->Boolean();
            if (callfunc->Size() >= 3) {
                keepwithdigit = callfunc->Evaluate(2, contextualpattern, idthread)->Boolean();
                if (callfunc->Size() == 4) {
                    Tamgu* vect = callfunc->Evaluate(3, contextualpattern, idthread);
                    for (long i = 0; i< vect->Size(); i++)
                        rules.push_back(vect->getstring(i));
                }
            }
        }
    }

    Tamgu* kvect = Selectasvector(contextualpattern);

    Locking _lock((TamguObject*)kvect);
    if (rules.size()) {
        if (!thestr.r_tokenize(((Tamgusvector*)kvect)->values,comma, separator, keepwithdigit, rules)) {
            string s = ((Tamgusvector*)kvect)->values[0];
            kvect->Clear();
            kvect->Release();
            return globalTamgu->Returnerror(s,idthread);
        }
    }
    else
        thestr.tokenize(((Tamgusvector*)kvect)->values,comma, separator, keepwithdigit);
    return kvect;
}

Tamgu* Tamgustring::MethodDoubleMetaphone(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    
    Tamgu* kvect = Selectasvector(contextualpattern);
    
    Locking _lock((TamguObject*)kvect);
    DoubleMetaphone(value,((Tamgusvector*)kvect)->values);
    return kvect;
}

Tamgu* Tamgustring::MethodStokenize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    locking();
    agnostring thestr(value);
    unlocking();
    Tamgu* keeps = NULL;

    if (callfunc->Size() == 1) {
        keeps = callfunc->Evaluate(0, contextualpattern, idthread);
        if (!keeps->isContainer())
            return globalTamgu->Returnerror("TAMGUI(665): Expecting a container in STOKENIZE", idthread);
    }

    vector<string> v;
    map<string, bool> k;
    if (keeps != NULL) {
        TamguIteration* itr = (TamguIteration*)keeps->Newiteration(false);
        for (itr->Begin(); itr->End() == aFALSE; itr->Next())
            k[itr->Valuestring()] = true;
        itr->Release();
    }
    thestr.stokenize(v, k);

    Tamgu* kvect = Selectasvector(contextualpattern);
    if (kvect->Type() == Tamgusvector::idtype) {
        Locking _lock((TamguObject*)kvect);
        ((Tamgusvector*)kvect)->values = v;
    }
    else {
        TamguConstString localstring("");

        for (long i = 0; i < v.size(); i++) {
            localstring.value = v[i];
            kvect->Push(&localstring);
        }
    }
    return kvect;
}

static bool comp(long s1, long s2) {
    if (s1<s2)
        return true;
    return false;
}

Tamgu* Tamgustring::MethodMultiSplit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    //Third parameter can be a vector

    Tamgu* kvect = NULL;
    string thesplitter;
    vector<long> ipositions;
    size_t pos = 0, found;
    long i = 0;
    //We first fill our vector with initial values...
    while (!pos && i < callfunc->Size()) {
        thesplitter = callfunc->Evaluate(i, contextualpattern, idthread)->String();
        pos = 0;
        Locking _lock(this);
        while (pos != string::npos) {
            found = s_findbyte(value, thesplitter, pos);
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
        thesplitter = callfunc->Evaluate(i, contextualpattern, idthread)->String();
        pos = 0;
        Locking _lock(this);

        while (pos != string::npos) {
            found = s_findbyte(value, thesplitter, pos);
            if (found != string::npos) {
                bool add = true;
                pos = found + thesplitter.size();
                for (long j = 0; j < ipositions.size(); j += 2) {
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
    if (contextualpattern->Type() == a_svector || !contextualpattern->isVectorContainer())
        kvect = Selectasvector(contextualpattern);
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


Tamgu* Tamgustring::MethodSplit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    //Third parameter can be a vector

    Tamgu* kvect = NULL;

    string localvalue;
    locking();
    string thestr = value;
    unlocking();
    string thesplitter;

    if (contextualpattern->Type() == a_svector || !contextualpattern->isVectorContainer())
        kvect = Selectasvector(contextualpattern);
    else
        kvect = Selectacontainer(contextualpattern, idthread);

    if (callfunc->Size() == 0) {
        unsigned char c;
        //we split along space characters...
        for (long i = 0; i < thestr.size(); i++) {
            c = thestr[i];
            if (c <= 32) {
                if (thesplitter.size()) {
                    kvect->storevalue(thesplitter);
                    thesplitter = "";
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

        wstring w = UString();

        vector<long> values;

        tamgusplitter->searchall(w,values);
        if (!values.size())
            return kvect;
        
        //Now we extract the strings in between
        long pos = 0;
        wstring sub;
        for (long i = 0; i < values.size(); i += 2) {
            sub = w.substr(pos, values[i]-pos);
            if (sub != L"")
                kvect->storevalue(sub);
            pos=values[i+1];
        }
        
        if (pos < w.size()) {
            sub = w.substr(pos,w.size()-pos);
            kvect->storevalue(sub);
        }
        
        return kvect;
    }

    thesplitter=tamgusplitter->String();
    
    //Second parameter is the splitter string
    if (thesplitter == "") {
        if (kvect->Type() == Tamgubvector::idtype) {
            for (long i = 0; i < thestr.size(); i++)
                kvect->storevalue((unsigned char)thestr[i]);
            return kvect;
        }

        agnostring athestr(thestr);
        athestr.begin();
        if (kvect->Type() == Tamguivector::idtype) {
            while (!athestr.end())
                kvect->storevalue((long)athestr.nextcode());
            return kvect;
        }

        while (!athestr.end())
            kvect->storevalue(athestr.nextemoji());
        return kvect;
    }

    size_t pos = 0;
    size_t found = 0;
    while (pos != string::npos) {
        found = s_findbyte(thestr, thesplitter, pos);
        if (found != string::npos) {
            localvalue = thestr.substr(pos, found - pos);
            if (localvalue != "")
                kvect->storevalue(localvalue);
            pos = found + thesplitter.size();
        }
        else
            break;
    }

    localvalue = thestr.substr(pos, thestr.size() - pos);
    if (localvalue != "")
        kvect->storevalue(localvalue);

    return kvect;
}

Tamgu* Tamgustring::MethodSplite(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    //Third parameter can be a vector

    Tamgu* kvect = NULL;

    string localvalue;
    locking();
    string thestr = value;
    unlocking();
    
    string thesplitter;
    
    if (contextualpattern->Type() == a_svector || !contextualpattern->isVectorContainer())
        kvect = Selectasvector(contextualpattern);
    else
        kvect = Selectacontainer(contextualpattern, idthread);
    
    if (callfunc->Size() == 0) {
        unsigned char c;
        //we split along space characters...
        for (long i = 0; i < thestr.size(); i++) {
            c = thestr[i];
            if (c <= 32) {
                if (thesplitter.size()) {
                    kvect->storevalue(thesplitter);
                    thesplitter = "";
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
        
        wstring w = UString();

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
    
    thesplitter=tamgusplitter->String();

    //Second parameter is the splitter string
    if (thesplitter == "") {
        if (kvect->Type() == Tamgubvector::idtype) {
            for (long i = 0; i < thestr.size(); i++)
                kvect->storevalue((unsigned char)thestr[i]);
            return kvect;
        }

        agnostring athestr(thestr);
        athestr.begin();
        if (kvect->Type() == Tamguivector::idtype) {
            while (!athestr.end())
                kvect->storevalue((long)athestr.nextcode());
            return kvect;
        }

        while (!athestr.end())
            kvect->storevalue(athestr.nextemoji());
        return kvect;
    }

    size_t pos = 0;
    size_t found = 0;
    while (pos != string::npos) {
        found = s_findbyte(thestr, thesplitter, pos);
        if (found != string::npos) {
            localvalue = thestr.substr(pos, found - pos);
            kvect->storevalue(localvalue);
            pos = found + thesplitter.size();
        }
        else
            break;
    }

    localvalue = thestr.substr(pos, thestr.size() - pos);
    if (localvalue != "")
        kvect->storevalue(localvalue);

    return kvect;
}

Tamgu* Tamgustring::MethodSlice(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* ke = Selectavector(contextualpattern);
    long num = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    if (num <= 0)
        return ke;

    long i;
    long beg = 0;
    
    Locking _lock(this);
    long sz = value.size();
    if (s_is_utf8(USTR(value), sz)) {
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

    string sub;
    for (i = num; i < sz; i+=num) {
        sub = value.substr(beg,i-beg);
        beg = i;
        ke->storevalue(sub);
    }
    if (i-beg>0) {
        sub = value.substr(beg,i-beg);
        ke->storevalue(sub);
    }
    return ke;
}

Tamgu* Tamgustring::MethodNgrams(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long num = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    if (num <= 0)
        return Selectasvector(contextualpattern);
    
    long i, j, mx;
    
    agnostring s(String());
    Tamgu* ke = Selectasvector(contextualpattern);
    string u;
    
    if (s.isutf8()) {
        vector<string> v;
        s.begin();
        while (!s.end())
            v.push_back(s.nextemoji());
        mx = v.size() - num + 1;
        for (i = 0; i < mx; i++) {
            u = "";
            for (j = i; j < i + num; j++)
                u += v[j];
            ke->storevalue(u);
        }
        return ke;
    }
    string& sb = (string&)s;
    mx = s.size() - num + 1;
    for (i = 0; i < mx; i++) {
        u = "";
        for (j = i; j < i + num; j++)
            u += sb[j];
        ke->storevalue(u);
    }
    return ke;
}

Tamgu* Tamgustring::MethodHash(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static const std::collate<char>& coll = std::use_facet<std::collate<char> >(loc);
    
    locking();
    string s = value;
    unlocking();
    
    if (s == "")
        return aNULL;
    long myhash = coll.hash(s.data(), s.data() + s.length());
    return globalTamgu->Provideint(myhash);
}

Tamgu* Tamgustring::MethodLatin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    return globalTamgu->Providestring(s_utf8_to_latin(USTR(value), value.size()));
}

Tamgu* Tamgustring::MethodUtf8(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long table = 0;
    if (callfunc->Size() != 0) {
        table = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        Locking _lock(this);
        return globalTamgu->Providestring(conversion_latin_table_to_utf8(table, USTR(value), value.size()));
    }
    Locking _lock(this);
    return globalTamgu->Providestring(conversion_latin_to_utf8(USTR(value), value.size()));
}

Tamgu* Tamgustring::MethodDos(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    return globalTamgu->Providestring(s_utf8_to_dos(USTR(value)));
}

Tamgu* Tamgustring::MethodDostoutf8(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    return globalTamgu->Providestring(s_dos_to_utf8(USTR(value)));
}

Tamgu* Tamgustring::MethodLeft(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long nbchar = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    //Second parameter is a position
    Locking _lock(this);
    return globalTamgu->Providestring(s_left(value, nbchar));
}

Tamgu* Tamgustring::MethodRight(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long nbchar = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    Locking _lock(this);
    return globalTamgu->Providestring(s_right(value, nbchar));
}

Tamgu* Tamgustring::MethodMid(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //Second parameter is a position
    long pos = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    //Third parameter is a number of characters
    long nbchar = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    Locking _lock(this);
    return globalTamgu->Providestring(s_middle(value, pos, nbchar));
}

Tamgu* Tamgustring::MethodIslower(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    return booleantamgu[s_is_lower(value)];
}

Tamgu* Tamgustring::MethodIsupper(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    return booleantamgu[s_is_upper(value)];
}

Tamgu* Tamgustring::MethodIsemoji(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    return booleantamgu[s_is_emoji(value)];
}

Tamgu* Tamgustring::MethodEmoji(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    return globalTamgu->Providestring(Emoji(value));
}

Tamgu* Tamgustring::MethodIsdigit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    return booleantamgu[s_is_digit(value)];
}

Tamgu* Tamgustring::MethodIsalpha(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    return booleantamgu[s_is_alpha(value)];
}

Tamgu* Tamgustring::MethodIsconsonant(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    return booleantamgu[s_is_consonant(USTR(value))];
}

Tamgu* Tamgustring::MethodIsvowel(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    return booleantamgu[s_is_vowel(USTR(value))];
}

Tamgu* Tamgustring::MethodDeaccentuate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    return globalTamgu->Providestring(s_deaccentuate(USTR(value)));
}

Tamgu* Tamgustring::MethodIspunctuation(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    return booleantamgu[s_is_punctuation(value)];
}

Tamgu* Tamgustring::MethodLower(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    return globalTamgu->Providestring(s_to_lower(value));
}

Tamgu* Tamgustring::MethodUpper(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    return globalTamgu->Providestring(s_to_upper(value));
}

Tamgu* Tamgustring::MethodTrim(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string s(String());
    return globalTamgu->Providestring(Trim(s));
}

Tamgu* Tamgustring::MethodTrimright(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string s(String());
    return globalTamgu->Providestring(Trimright(s));
}

Tamgu* Tamgustring::MethodTrimleft(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string  s(String());
    return globalTamgu->Providestring(Trimleft(s));
}

Tamgu* Tamgustring::MethodBase(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static vector<wstring> caracs;
    static hash_bin<wchar_t, long> mcaracs;
    wstring w;
    long n, b = -1;

    bool toconvert=true;
    Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
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
        return globalTamgu->Provideustring(res);
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

Tamgu* Tamgustring::MethodPadding(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long res = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    string sval = callfunc->Evaluate(1, contextualpattern, idthread)->String();
    bool direction = true;
    if (callfunc->Size() == 3)
        direction = callfunc->Evaluate(2, contextualpattern, idthread)->Boolean();

    Locking _locks(this);

    bool onechar = false;
    long sz;
    if (sval.size() == 1) {
        if (direction) {
            sz = value.size();
            if (res > sz)
                value.resize(res, sval[0]);
            return this;
        }
        onechar = true;
    }


    sz = res - value.size();
    if (sz <= 0)
        return this;

    string sres;
    if (onechar) {
        char* s = (char*)malloc(sz + 1);
        char c = ' ';
        c = sval[0];
        memset(s, c, sz);
        s[sz] = 0;
        sres = s;
        free(s);
        value = sres + value;
        return this;
    }
    
    string s;
    while (sz > 0) {
        s += sval;
        sz--;
    }
    sres = s;
    
    if (direction)
        value += sres;
    else
        value = sres + value;

    return this;
}

Tamgu* Tamgustring::MethodFill(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    Tamgu* res = callfunc->Evaluate(0, contextualpattern, idthread);
    Tamgu* kval = callfunc->Evaluate(1, contextualpattern, idthread);
    long sz = res->Integer();
    if (sz <= 0)
        return globalTamgu->Providestring("");

    string sval = kval->String();

    Locking _locks(this);
    if (sval != "" && sval.size() == 1) {
        char* s = (char*)malloc(sz + 1);
        char c = ' ';
        c = sval[0];
        memset(s, c, sz);
        s[sz] = 0;
        value = s;
        free(s);
    }
    else {
        string s;
        while (sz > 0) {
            s += sval;
            sz--;
        }
        value = s;
    }
    return this;
}

Tamgu* Tamgustring::MethodTohtml(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
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

Tamgu* Tamgustring::MethodToxml(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
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

Tamgu* Tamgustring::MethodBytes(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    long sz = value.size();
    if (sz >= 1) {
        if (contextualpattern->isVectorContainer() || sz > 1) {
            Tamgu* kvect=SelectContainer(contextualpattern,idthread);
            if (kvect==NULL)
                kvect = globalTamgu->Provideivector();
                
            Locking _lock((TamguObject*)kvect);
            for (size_t i = 0; i < value.size(); i++)
                kvect->storevalue((uchar)value[i]);
            return kvect;
        }

        if (contextualpattern->isNumber()) {
            Tamgu* a = contextualpattern->Newinstance(idthread);
            a->storevalue((uchar)value[0]);
            return a;
        }

        return new Tamgubyte(value[0]);
    }

    return aNULL;
}

Tamgu* Tamgustring::MethodGetstd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //Reading
    bool catchoutput = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
    if (catchoutput) {
        if (globalTamgu->kstd != NULL)
            return globalTamgu->Returnerror("TAMGUI(543): Cannot intercept the current standard output", idthread);
        globalTamgu->kstd = this;
        return aTRUE;
    }
    else {
        if (globalTamgu->kstd == this)
            globalTamgu->kstd = NULL;
    }
    return aTRUE;
}

Tamgu* Tamgustring::MethodGeterr(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //Reading
    bool catchoutput = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
    if (catchoutput) {
        if (globalTamgu->kstd != NULL)
            return globalTamgu->Returnerror("TAMGUI(544): Cannot intercept the current error output", idthread);
        globalTamgu->kerr = this;
        Setreference();
        return aTRUE;
    }
    else {
        if (globalTamgu->kerr == this) {
            globalTamgu->kerr->Resetreference();
            globalTamgu->kerr = NULL;
        }
    }
    return aTRUE;
}

Tamgu* Tamgustring::MethodInsert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    size_t idx;

    //First parameter should be a vector

    //second parameter is anything
    idx = -1;
    string val;
    agnostring ret(String());
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
    return globalTamgu->Providestring(ret);
}

Tamgu* Tamgustring::MethodClear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    value = "";
    return aTRUE;
}

Tamgu* Tamgustring::MethodExtract(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* pos = callfunc->Evaluate(0, contextualpattern, idthread);
    Tamgu* from = callfunc->Evaluate(1, contextualpattern, idthread);

    vector<string> v;
    long i;
    for (i = 2; i < callfunc->Size(); i++)
        v.push_back(callfunc->Evaluate(i, contextualpattern, idthread)->String());

    locking();
    string init = value;
    unlocking();
    
    i = pos->Integer();
    string sfrom = from->String();
    i = init.find(sfrom, i);
    long lgfrom = sfrom.size();
    Tamgu* vect = Selectasvector(contextualpattern);
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

Tamgu* Tamgustring::MethodReverse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    return globalTamgu->Providestring(s_revert(value));
}

Tamgu* Tamgustring::MethodRemovefirst(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* knb = callfunc->Evaluate(0, contextualpattern, idthread);
    long id = knb->Integer();
    //On retire en tete
    Locking _lock(this);

    long iv = size_c(value);
    if (id >= iv)
        return aEMPTYSTRING;
    
    return globalTamgu->Providestring(s_right(value, iv - id));
}

Tamgu* Tamgustring::MethodRemovelast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* knb = callfunc->Evaluate(0, contextualpattern, idthread);
    long id = knb->Integer();
    //On retire en tete
    Locking _lock(this);

    long iv = size_c(value);

    if (id >= iv)
        return aEMPTYSTRING;

    return globalTamgu->Providestring(s_left(value, iv - id));
}

Tamgu* Tamgustring::MethodFormat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    char buffer[101];
    locking();
    string sformat = value;
    unlocking();
    
    //In this case, we might have more than two parameters...
    string e;
    buffer[0] = '%';
    for (long i = callfunc->Size() - 1; i >= 0; i--) {
        e = callfunc->Evaluate(i, contextualpattern, idthread)->String();
        sprintf_s(buffer + 1, 100, "%ld", i + 1);
        sformat = s_replacestrings(sformat, buffer, e);
    }
    return globalTamgu->Providestring(sformat);
}

Tamgu* Tamgustring::MethodReplace(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //Replace an element with another

    Tamgu* treg = callfunc->Evaluate(0, contextualpattern, idthread);
    if (treg->isRegular()) {
        if (treg->Type() == a_treg) {
            wstring rep = callfunc->Evaluate(1, contextualpattern, idthread)->UString();
            wstring w = UString();
            vector<long> values;
            treg->searchall(w,values);
            
            for (long i=values.size()-2; i>=0; i-=2)
                w=w.substr(0,values[i])+rep+w.substr(values[i+1],w.size()-values[i+1]);
            return globalTamgu->Provideustring(w);
        }
#ifdef Tamgu_REGEX
        string rep = callfunc->Evaluate(1, contextualpattern, idthread)->String();
        locking();
        string w = value;
        unlocking();
        
        w = treg->replace(w, rep);
        return globalTamgu->Providestring(w);
#endif
    }

    locking();
    string str = value;
    unlocking();
    
    string reg = treg->String();
    if (reg == "")
        return globalTamgu->Providestring(str);

    string rep = callfunc->Evaluate(1, contextualpattern, idthread)->String();

    str = s_replacestring(str, reg, rep);
    return globalTamgu->Providestring(str);
}

Tamgu* Tamgustring::MethodEditdistance(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* e2 = callfunc->Evaluate(0, contextualpattern, idthread);

    bool bytecall = false;
    if (callfunc->Size() == 3)
        bytecall = callfunc->Evaluate(1, contextualpattern, idthread)->Boolean();

    if (!bytecall) {
        Tamguustring e(UString());
        return globalTamgu->Provideint(e.EditDistance(e2));
    }

    return globalTamgu->Provideint(EditDistance(e2));
}

Tamgu* Tamgustring::MethodPop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
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

Tamgu* Tamgustring::MethodLast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter should be a vector
    string res(String());
    if (res.size() == 0)
        return aNULL;
    res = c_char_index(res, size_c(res) - 1);
    return globalTamgu->Providestring(res);
}

Tamgu* Tamgustring::MethodIndent(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long blanks = 5;
    if (callfunc->Size() == 1)
        blanks = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    string codeindente;
    string code=String();
    IndentCode(code, codeindente, blanks);
    return globalTamgu->Providestring(codeindente);
}

Exporting Tamgu* Tamgustring::Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {
    Tamgu* a;

    Tamgustring* it = globalTamgu->Providestring("");
    it->Setreference();

    uchar addvalue = 0;
    Locking* lock = NULL;
    if (context != aNOELEMENT) {
        addvalue = Selecttype(context);
        if (addvalue)
            lock = new Locking((TamguObject*)context);
    }


    recipient->Putvalue( it, idthread);

    agnostring v(String());
    v.begin();
    while (!v.end()) {
        it->value = v.nextemoji();

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
            if (lock != NULL)
                delete lock;
            return globalTamgu->Errorobject(idthread);
        }

        context = Storealongtype(context, a, idthread, addvalue);
        a->Release();
    }

    it->Resetreference();
    recipient->Forcedclean();

    if (lock != NULL)
        delete lock;
    return context;
}

Exporting Tamgu* Tamgustring::Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct) {

    Tamgu* returnval;

    bool first = false;
    Tamgustring* key = globalTamgu->Providestring("");
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
    agnostring v(String());
    v.begin();
    while (!v.end()) {
        key->value = v.nextemoji();
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


void TamguLoopString::Callfunction() {

    TamguCallFunction kfunc(function);

    Tamguint* ki = globalTamgu->Provideint(position);
    ki->Setreference();
    kfunc.arguments.push_back(this);
    kfunc.arguments.push_back(ki);
    Tamgu* ke = kfunc.Eval(aNULL, aNULL, globalTamgu->GetThreadid());
    ke->Release();
    ki->Resetreference();
}

Tamgu* TamguLoopString::Put(Tamgu* context, Tamgu* ke, short idthread) {
    if (ke->isVectorContainer()) {
        position = 0;
        interval.clear();
        for (long i = 0; i < ke->Size(); i++)
            interval.push_back(ke->getstring(i));
        Locking _lock(this);
        value = interval[0];
        return aTRUE;
    }

    if (ke->Type() == a_sloop) {
        TamguLoopString* kl = (TamguLoopString*)ke;
        interval = kl->interval;
        Locking _lock(this);
        value = kl->value;
        position = kl->position;
        return aTRUE;
    }

    if (interval.size() == 0) {
        Locking _lock(this);
        position = 0;
        value = "";
        return aTRUE;
    }

    position = ke->Integer();
    if (position >= interval.size())
        position = position % interval.size();
    else
        if (position < 0)
            position = (interval.size() + position) % interval.size();
    Locking _lock(this);
    value = interval[position];
    return aTRUE;
}


Tamgu* TamguLoopString::Vector(short idthread) {
    Tamgusvector* kvect = globalTamgu->Providesvector();
    Doublelocking _lock(this, kvect);
    kvect->values = interval;
    return kvect;
}


Tamgu* Tamgustring::MethodJamo(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring s = UString();
    wstring w;
    if (callfunc->Size() == 0 || callfunc->Evaluate(0, contextualpattern, idthread)->Boolean() == false) {
        w = s_split_jamo(s);
        if (contextualpattern->isVectorContainer()) {
            Tamguuvector* vs = (Tamguuvector*)Selectauvector(contextualpattern);
            Locking _lock(vs);
            wstring s;
            for (long i = 0; i < w.size(); i++) {
                s = w[i];
                vs->values.push_back(s);
            }
            return vs;
        }

        return globalTamgu->Provideustring(w);
    }

    //Else combine...
    w = UString();
    w = s_combine_jamo(w);
    return globalTamgu->Provideustring(w);
}

Tamgu* Tamgustring::MethodIsJamo(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring s = UString();
    return booleantamgu[s_is_jamo(s)];
}

Tamgu* Tamgustring::MethodIsHangul(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring s = UString();
    return booleantamgu[s_is_hangul(s)];
}

Tamgu* Tamgustring::andset(Tamgu* a, bool autoself) {
    string s = a->String();
    string u;
    Locking _lock(this);

    long m = min(s.size(), value.size());
    for (long i = 0; i < m; i++) {
        if (s[i] == value[i])
            u += s[i];
    }
    if (autoself) {
        value = u;
        return this;
    }

    return globalTamgu->Providestring(u);
}

Tamgu* Tamgustring::xorset(Tamgu* a, bool autoself) {
    string s = a->String();
    string u;

    Locking _lock(this);
    long m = min(s.size(), value.size());
    for (long i = 0; i < m; i++) {
        if (s[i] != value[i])
            u += value[i];
    }
    if (autoself) {
        value = u;
        return this;
    }

    return globalTamgu->Providestring(u);
}

Tamgu* Tamgustring::MethodNormalizeHangul(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring s = UString();
    return globalTamgu->Provideustring(s_hangul_normalize(s));
}

Tamgu* Tamgustring::MethodTransliteration(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string c;
    long i = 0;
    
    locking();
    string str = value;
    unlocking();
    
    if (contextualpattern->isVectorContainer()) {
        Tamgusvector* vs = (Tamgusvector*)Selectasvector(contextualpattern);
        Locking _lock(vs);
        while (i < value.size()) {
            c = c_translate(USTR(str), i);
            vs->values.push_back(c);
        }
        return vs;
    }

    if (str.size())
        c = c_translate(USTR(str), i);

    return globalTamgu->Providestring(c);
}

Tamgu* Tamgustring::MethodRead(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string filename = callfunc->Evaluate(0, contextualpattern, idthread)->String();

    Tamgufile file;
    
#ifdef WIN32
    fopen_s(&file.thefile, STR(filename), "rb");
#else
    file.thefile=fopen(STR(filename), "rb");
#endif
    
    if (file.thefile == NULL) {
        string msg="Cannot open the file:";
        msg += filename;
        return globalTamgu->Returnerror(msg, idthread);
    }
    
    Locking _lock(this);
    value=file.read(-1);
    
    return this;
}

Tamgu* TamguLoopString::andset(Tamgu* a, bool autoself) {
    string s = a->String();
    string u;
    Locking _lock(this);
    long m = min(s.size(), value.size());
    for (long i = 0; i < m; i++) {
        if (s[i] == value[i])
            u += s[i];
    }
    if (autoself) {
        value = u;
        return this;
    }

    return globalTamgu->Providestring(u);
}

Tamgu* TamguLoopString::xorset(Tamgu* a, bool autoself) {
    string s = a->String();
    string u;
    Locking _lock(this);
    long m = min(s.size(), value.size());
    for (long i = 0; i < m; i++) {
        if (s[i] != value[i])
            u += value[i];
    }
    if (autoself) {
        value = u;
        return this;
    }

    return globalTamgu->Providestring(u);
}


//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//We need to declare once again our local definitions.
Exporting basebin_hash<a_stringMethod>  Tamgua_string::methods;
Exporting hmap<string, string> Tamgua_string::infomethods;
Exporting bin_hash<unsigned long> Tamgua_string::exported;

Exporting short Tamgua_string::idtype = 0;


    //MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgua_string::AddMethod(TamguGlobal* global, string name, a_stringMethod func, unsigned long arity, string infos, short returntype) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
    
    if (returntype != a_null)
        global->returntypes[idname] = returntype;
}

bool Tamgua_string::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();
    
    Tamgua_string::idtype = global->Getid("a_string");
    
    Tamgua_string::AddMethod(global, "doublemetaphone", &Tamgua_string::MethodDoubleMetaphone, P_NONE, "doublemetaphone(): Return the string double metaphone conversion into a svector", a_svector);
    Tamgua_string::AddMethod(global, "hash", &Tamgua_string::MethodHash, P_NONE, "hash(): Return the hash value of a string.", a_int);

    Tamgua_string::AddMethod(global, "ord", &Tamgua_string::MethodOrd, P_NONE, "ord(): return the ASCII code of the first character, or a list of all ASCII code if the recipient is a vector", a_null);
    Tamgua_string::AddMethod(global, "bytes", &Tamgua_string::MethodBytes, P_NONE, "bytes(): Return the string as a vector of bytes", a_ivector);
    Tamgua_string::AddMethod(global, "sizeb", &Tamgua_string::MethodSizeb, P_NONE, "sizeb(): Return the size in bytes of the string", a_int);
    Tamgua_string::AddMethod(global, "levenshtein", &Tamgua_string::MethodEditdistance, P_ONE | P_TWO, "levenshtein(string s,bool byte): Return the edit distance with 's' according to Levenshtein algorithm. If byte is true, force a byte level comparison. byte is optionnal.", a_int);
    Tamgua_string::AddMethod(global, "editdistance", &Tamgua_string::MethodEditdistance, P_ONE | P_TWO | P_THREE, "editdistance(string s,bool byte): Return the edit distance with 's'. If byte is true, force a byte level comparison. byte is optionnal.", a_int);
    Tamgua_string::AddMethod(global, "find", &Tamgua_string::MethodFind, P_TWO | P_ONE, "find(string sub,int pos): Return the position of substring sub starting at position pos", a_int);
    Tamgua_string::AddMethod(global, "rfind", &Tamgua_string::MethodRfind, P_TWO | P_ONE, "rfind(string sub,int pos): Return the position of substring sub backward starting at position pos", a_int);

    Tamgua_string::AddMethod(global, "tokenize", &Tamgua_string::MethodTokenize, P_NONE | P_ONE | P_TWO | P_THREE | P_FOUR, "tokenize(bool comma,bool separator,bool concatenate, svector rules): Segment a string into words and punctuations. If 'comma' is true, then the decimal character is ',' otherwise it is '.'. If 'separator' is true then '1,000' is accepted as a number. If 'concatenate' is true then '3a' is a valid token. rules is a set of tokenization rules that can be first initialized then modified with _getdefaulttokenizerules", a_null);
    Tamgua_string::AddMethod(global, "stokenize", &Tamgua_string::MethodStokenize, P_NONE | P_ONE, "stokenize(map keeps): Segment a string into words and punctuations, with a keep.", a_null);

    Tamgua_string::AddMethod(global, "count", &Tamgua_string::MethodCount, P_THREE | P_TWO | P_ONE | P_NONE, "count(string sub,int pos,int mx): Count the number of substrings starting at position pos, ending at mx", a_int);
    Tamgua_string::AddMethod(global, "byteposition", &Tamgua_string::MethodByteposition, P_ONE, "byteposition(int pos): Convert a character position into a byte position", a_int);
    Tamgua_string::AddMethod(global, "charposition", &Tamgua_string::MethodCharposition, P_ONE, "charposition(int pos): Convert a byte position into a character position", a_int);
    
    Tamgua_string::AddMethod(global, "isalpha", &Tamgua_string::MethodIsalpha, P_NONE, "isalpha(): Test if a string only contains only alphabetical characters", a_boolean);
    Tamgua_string::AddMethod(global, "isconsonant", &Tamgua_string::MethodIsconsonant, P_NONE, "isconsonant(): Test if a string only contains consonants", a_boolean);
    Tamgua_string::AddMethod(global, "isvowel", &Tamgua_string::MethodIsvowel, P_NONE, "isvowel(): Test if a string only contains only vowels", a_boolean);
    Tamgua_string::AddMethod(global, "ispunctuation", &Tamgua_string::MethodIspunctuation, P_NONE, "ispunctuation(): Test if the character is a punctuation", a_boolean);
    Tamgua_string::AddMethod(global, "isdigit", &Tamgua_string::MethodIsdigit, P_NONE, "isdigit(): Test if a string only contains digits", a_boolean);
    Tamgua_string::AddMethod(global, "isupper", &Tamgua_string::MethodIsupper, P_NONE, "isupper(): Test if a string only contains uppercase characters", a_boolean);
    Tamgua_string::AddMethod(global, "islower", &Tamgua_string::MethodIslower, P_NONE, "islower(): Test if a string only contains lowercase characters", a_boolean);
    Tamgua_string::AddMethod(global, "isemoji", &Tamgua_string::MethodIsemoji, P_NONE, "isemoji(): Test if a string only contains emoji characters", a_boolean);
    Tamgua_string::AddMethod(global, "isutf8", &Tamgua_string::MethodIsutf8, P_NONE, "isutf8(): Return true is the string is encoded in UTF8", a_boolean);
    Tamgua_string::AddMethod(global, "ngrams", &Tamgua_string::MethodNgrams, P_ONE, "ngrams(int r): Return a vector of all ngrams of rank r", a_svector);

    Tamgua_string::AddMethod(global, "split", &Tamgua_string::MethodSplit, P_ONE | P_NONE, "split(string splitter): split a string along splitter and store the results  in a vector. If splitter=='', then the string is split into a vector of characters", a_svector);
    Tamgua_string::AddMethod(global, "multisplit", &Tamgua_string::MethodMultiSplit, P_ATLEASTONE, "multisplit(string splitter1,string splitter2..): split a string along different splitters. Return a svector.", a_svector);
    Tamgua_string::AddMethod(global, "splite", &Tamgua_string::MethodSplite, P_ONE | P_NONE, "splite(string splitter): split a string along splitter and store the results  in a vector. If splitter=='', then the string is split into a vector of characters. Empty strings are kept in the result.", a_svector);
    Tamgua_string::AddMethod(global, "slice", &Tamgua_string::MethodSlice, P_ONE, "slice(int sz): Return a vector of all slices of size sz", a_svector);

    Tamgua_string::AddMethod(global, "reverse", &Tamgua_string::MethodReverse, P_NONE, "reverse(): reverse the string", a_string);
    Tamgua_string::AddMethod(global, "format", &Tamgua_string::MethodFormat, P_ATLEASTONE, "format(p1,p2,p3): Create a new string from the current string in which each '%x' is associated to one of the parameters, 'x' being the position of that parameter in the argument list. 'x' starts at 1.", a_string);
    Tamgua_string::AddMethod(global, "fill", &Tamgua_string::MethodFill, P_TWO, "fill(int nb,string c): create a string of nb characters c", a_string);
    Tamgua_string::AddMethod(global, "padding", &Tamgua_string::MethodPadding, P_TWO | P_THREE, "padding(int nb,string c,bool paddattheend): add nb characters c to the current string. Last parameter is optional", a_string);
    Tamgua_string::AddMethod(global, "pop", &Tamgua_string::MethodPop, P_NONE | P_ONE | P_TWO, "pop(): remove last character", a_string);
    Tamgua_string::AddMethod(global, "replace", &Tamgua_string::MethodReplace, P_TWO, "replace(sub,str): Replace the substrings matching sub with str", a_string);
    Tamgua_string::AddMethod(global, "removefirst", &Tamgua_string::MethodRemovefirst, P_ONE, "removefirst(int nb): remove the first nb characters of a string", a_string);
    Tamgua_string::AddMethod(global, "removelast", &Tamgua_string::MethodRemovelast, P_ONE, "removelast(int nb): remove the last nb characters of a string", a_string);
    Tamgua_string::AddMethod(global, "utf8", &Tamgua_string::MethodUtf8, P_NONE | P_ONE, "utf8(int table): convert a LATIN string into UTF8. table is optional, by default it is ISO/IEC 8859 part 1.", a_string);
    Tamgua_string::AddMethod(global, "latin", &Tamgua_string::MethodLatin, P_NONE, "latin(): convert an UTF8 string in LATIN", a_string);
    Tamgua_string::AddMethod(global, "dos", &Tamgua_string::MethodDos, P_NONE, "dos(): convert a string into DOS encoding", a_string);
    Tamgua_string::AddMethod(global, "dostoutf8", &Tamgua_string::MethodDostoutf8, P_NONE, "dostoutf8(): convert a DOS string into UTF8", a_string);
    Tamgua_string::AddMethod(global, "left", &Tamgua_string::MethodLeft, P_ONE, "left(int nb): return the first nb characters of a string", a_string);
    Tamgua_string::AddMethod(global, "right", &Tamgua_string::MethodRight, P_ONE, "right(int nb): return the last nb characters of a string", a_string);
    Tamgua_string::AddMethod(global, "mid", &Tamgua_string::MethodMid, P_TWO, "mid(int pos,int nb): return the nb characters starting at position pos of a string", a_string);

    Tamgua_string::AddMethod(global, "upper", &Tamgua_string::MethodUpper, P_NONE, "upper(): Return the string in upper characters", a_string);
    Tamgua_string::AddMethod(global, "deaccentuate", &Tamgua_string::MethodDeaccentuate, P_NONE, "deaccentuate(): Remove the accents from accented characters", a_string);

    Tamgua_string::AddMethod(global, "lower", &Tamgua_string::MethodLower, P_NONE, "lower(): Return the string in lower characters", a_string);
    Tamgua_string::AddMethod(global, "trim", &Tamgua_string::MethodTrim, P_NONE, "trim(): remove the trailing characters", a_string);
    Tamgua_string::AddMethod(global, "trimleft", &Tamgua_string::MethodTrimleft, P_NONE, "trimleft(): remove the trailing characters on the left", a_string);
    Tamgua_string::AddMethod(global, "trimright", &Tamgua_string::MethodTrimright, P_NONE, "trimright(): remove the trailing characters on the right", a_string);
    Tamgua_string::AddMethod(global, "last", &Tamgua_string::MethodLast, P_NONE, "last(): return last character", a_string);
    Tamgua_string::AddMethod(global, "insert", &Tamgua_string::MethodInsert, P_ONE | P_TWO, "insert(i,s): insert the string s at i. If i is -1, then insert s between each character in the input string", a_string);
    Tamgua_string::AddMethod(global, "clear", &Tamgua_string::MethodClear, P_NONE, "clear(): Clean the content of a string.", a_null);

    Tamgua_string::AddMethod(global, "jamo", &Tamgua_string::MethodJamo, P_NONE | P_ONE, "jamo(bool combine): if 'combine' is false split a Korean jamo into its main components, else combine contents into a jamo.", a_null);
    Tamgua_string::AddMethod(global, "isjamo", &Tamgua_string::MethodIsJamo, P_NONE, "isjamo(): return true if it is a Hangul jamo.", a_null);
    Tamgua_string::AddMethod(global, "ishangul", &Tamgua_string::MethodIsHangul, P_NONE, "ishangul(): return true if it is a Hangul character.", a_null);
    Tamgua_string::AddMethod(global, "normalizehangul", &Tamgua_string::MethodNormalizeHangul, P_NONE, "normalizehangul(): Normalize Hangul characters.", a_string);
    Tamgua_string::AddMethod(global, "romanization", &Tamgua_string::MethodTransliteration, P_NONE, "romanization(): romanization of Hangul characters.", a_null);


    global->newInstance[Tamgua_string::idtype] = new Tamgua_string("", global);
    global->RecordMethods(Tamgua_string::idtype, Tamgua_string::exported);

    return true;
}

Exporting TamguIteration* Tamgua_string::Newiteration(bool direction) {
    string v = value.value();
    return new TamguIterationstring(v, direction);
}


Tamgu* Tamgua_string::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    agnostring val(value.value());
    
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    Tamgustring v("");
    Tamgu* a;
    val.begin();
    while (!val.end()) {
        v.value =  val.nextemoji();
        var->Putvalue(&v, idthread);
        
        a = ins->instructions.vecteur[1]->Eval(context, aNULL, idthread);
        
            //Continue does not trigger needInvestigate
        if (a->needInvestigate()) {
            if (a == aBREAK)
                break;
            return a;
        }
        
        a->Release();
    }
    
    return this;
}

Tamgu* Tamgua_string::Succ() {
    if (value.isempty())
        return new Tamgua_string("");
    
    wstring w;
    string v = value.value();
    s_utf8_to_unicode(w, USTR(v), v.size());
    w[w.size() - 1] = w.back() + 1;
    return globalTamgu->Provideustring(w);
}

Tamgu* Tamgua_string::Pred() {
    if (value.isempty())
        return new Tamgua_string("");
    
    wstring w;
    string v = value.value();
    s_utf8_to_unicode(w, USTR(v), v.size());
    
    wchar_t c = w.back();
    if (c <= 1)
        return globalTamgu->Provideustring(w);
    
    w[w.size() - 1] = c - 1;
    return globalTamgu->Provideustring(w);
}


Exporting Tamgu* Tamgua_string::in(Tamgu* context, Tamgu* a, short idthread) {
        //Three cases along the container type...
        //It is a Boolean, we expect false or true
        //It is an integer, we expect a position in v
        //It is a container, we are looking for all positions...
    
    string val = a->String();
    string vl = value.value();
    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Locking* _vlock = _getlock(v);
        s_findall(vl, val, v->values);
        v_convertbytetocharposition(USTR(vl), v->values);
        _cleanlock(_vlock);
        return v;
    }
    
    long r = s_find(vl, val, 0);
    
    if (context->isString()) {
        if (r==-1)
            return aEMPTYSTRING;
        return new Tamgua_string(val);
    }
    
    if (context->isNumber())
        return globalTamgu->Provideint(r);
    
    if (r == -1)
        return aFALSE;
    
    return aTRUE;
}


Exporting unsigned long Tamgua_string::EditDistance(Tamgu* e) {
    string vl = value.value();
    string s2 = e->String();
    unsigned long s1len, s2len, x, y, lastdiag, olddiag;
    s1len = vl.size();
    s2len = s2.size();
    size_t* column = new size_t[s1len + 1];
    for (y = 1; y <= s1len; y++)
        column[y] = y;
    for (x = 1; x <= s2len; x++) {
        column[0] = x;
        for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
            olddiag = column[y];
            column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + (vl[y - 1] == s2[x - 1] ? 0 : 1));
            lastdiag = olddiag;
        }
    }
    s2len = column[s1len];
    delete[] column;
    return s2len;
}

Exporting Tamgu* Tamgua_string::Put(Tamgu* idx, Tamgu* v, short idthread) {
    
    if (!idx->isIndex()) {
        if (v == this)
            return aFALSE;
        if (v->Type() == Tamgua_string::idtype)
            value = ((Tamgua_string*)v)->value;
        else
            value = v->String();
        return aTRUE;
    }
    
    string s;
    if (v != aNULL)
        s = v->String();
    
    long ileft, iright;

    string vl = value.value();
    char res = StringIndexes(vl, idx, ileft, iright, idthread);
    
    if (res == 0) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong key in a container or a string access", idthread);
        return aFALSE;
    }
    
    if (res == 1) {
        iright = ileft;
        iright = c_char_next(USTR(vl), iright);
    }
    else
        iright = iright - ileft;
    vl.erase(ileft, iright);
    if (s != "")
        vl.insert(ileft, s);
    
    value = vl;
    return aTRUE;
    
}

Exporting Tamgu* Tamgua_string::Eval(Tamgu* context, Tamgu* idx, short idthread) {
    if (!idx->isIndex() || context == idx)
        return this;

    string vl = value.value();
    long ileft, iright;
    char res = StringIndexes(vl, idx, ileft, iright, idthread);
    
    if (res == 0) {
        if (globalTamgu->erroronkey)
            globalTamgu->Returnerror("Wrong key in a container or a string access", idthread);
        return aNOELEMENT;
    }
    
    
    if (res == 1)
        idx = new Tamgua_string(value[ileft]);
    else {
        idx = new Tamgua_string((char)0);
        value.substr(((Tamgua_string*)idx)->value, ileft, iright - ileft);
    }
    return idx;
}


Tamgu* Tamgua_string::MethodOrd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
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
        
        return new Tamgulong(s[0]);
    }
    
    return aNULL;
}

    //------------------------------------------------------------------------------------------------------------------
Tamgu* Tamgua_string::MethodIsutf8(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string s = value.value();
    contextualpattern = booleantamgu[s_is_utf8(USTR(s), s.size())];
    return contextualpattern;
}

Tamgu* Tamgua_string::MethodBytes(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string vl = value.value();
    long sz = vl.size();
    if (sz >= 1) {
        if (contextualpattern->isVectorContainer() || sz > 1) {
            Tamgu* kvect=SelectContainer(contextualpattern,idthread);
            if (kvect==NULL)
                kvect = globalTamgu->Provideivector();
            
            Locking _lock((TamguObject*)kvect);
            for (size_t i = 0; i < vl.size(); i++)
                kvect->storevalue((uchar)vl[i]);
            return kvect;
        }
        
        if (contextualpattern->isNumber()) {
            Tamgu* a = contextualpattern->Newinstance(idthread);
            a->storevalue((uchar)vl[0]);
            return a;
        }
        
        return new Tamgubyte(vl[0]);
    }
    
    return aNULL;
}


Tamgu* Tamgua_string::MethodEditdistance(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* e2 = callfunc->Evaluate(0, contextualpattern, idthread);
    
    bool bytecall = false;
    if (callfunc->Size() == 3)
        bytecall = callfunc->Evaluate(1, contextualpattern, idthread)->Boolean();
    
    if (!bytecall) {
        Tamguustring e(UString());
        return globalTamgu->Provideint(e.EditDistance(e2));
    }
    
    return globalTamgu->Provideint(EditDistance(e2));
}

Tamgu* Tamgua_string::MethodPop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
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

Tamgu* Tamgua_string::MethodLast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //First parameter should be a vector
    string res(value.value());
    if (res.size() == 0)
        return aNULL;
    res = c_char_index(res, size_c(res) - 1);
    return new Tamgua_string(res);
}

Tamgu* Tamgua_string::MethodCount(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long i = 0, nb = 0;
    
    long mx = -1;
    if (callfunc->Size() >= 2) {
        i = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
        if (callfunc->Size() == 3)
            mx = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
    }
    
    
    Tamgu* substr = callfunc->Evaluate(0, contextualpattern, idthread);
    
    if (substr->isRegular()) {
        wstring w = UString();
        if (mx != -1)
            w = w.substr(i, mx-i);
        else
            if (i)
                w = w.substr(i, w.size() - i);
        
        vector<long> values;
        substr->searchall(w,values);
        return globalTamgu->Provideint(values.size()>>1);
    }

    agnostring str(value.value());
    if (str == "")
        return aZERO;
    
    string sub = substr->String();
    i = str.find(sub, i);
    while (i != -1) {
        if (mx != -1 && i >= mx)
            break;
        nb++;
        i++;
        i = str.find(sub, i);
    }
    
    return globalTamgu->Provideint(nb);
}

Tamgu* Tamgua_string::MethodFind(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* ksub = callfunc->Evaluate(0, contextualpattern, idthread);
    
    if (!Size()) {
        if (contextualpattern->isVectorContainer())
            return Selectavector(contextualpattern);
        return aMINUSONE;
    }
    
        //we search for a substring starting at position idx
    long i = 0;
    if (callfunc->Size() == 2)
        i = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    
    if (contextualpattern->isVectorContainer()) {
        Tamgu* kvect = Selectavector(contextualpattern);
        long j;
        
        wstring str = UString();
        
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
        wstring str = UString();
        long e;
        if (!ksub->search(str,i,e,i))
            return aMINUSONE;
        return globalTamgu->Provideint(i);
    }
    
    string str = value.value();
    
    string sub = ksub->String();
    i = s_find(str, sub, i);
    
    if (i == -1)
        return aMINUSONE;
    
    return globalTamgu->Provideint(i);
}

Tamgu* Tamgua_string::MethodRfind(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* ksub = callfunc->Evaluate(0, contextualpattern, idthread);
    
        //we search for a substring starting at position idx
    long i = value.size();
    
    if (!i) {
        if (contextualpattern->isVectorContainer())
            return Selectavector(contextualpattern);
        return aMINUSONE;
    }
    
    if (callfunc->Size() == 2)
        i = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    
    if (contextualpattern->isVectorContainer()) {
        Tamgu* kvect = Selectavector(contextualpattern);
        long j;
        
        wstring str = UString();
        
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
    
    
    if (ksub->isRegular()) {
        wstring str=UString();
        long e;
        if (!ksub->searchlast(str,i,e,i))
            return aMINUSONE;
        return globalTamgu->Provideint(i);
    }
    
    string str = value.value();
    
    string sub = ksub->String();
    i = s_rfind(str, sub, i);
    
    if (i == -1)
        return aMINUSONE;
    
    return globalTamgu->Provideint(i);
}

Tamgu* Tamgua_string::MethodByteposition(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string  s = value.value();
    return globalTamgu->Provideint(c_chartobyteposition(USTR(s),callfunc->Evaluate(0, contextualpattern, idthread)->Integer()));
}

Tamgu* Tamgua_string::MethodCharposition(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string  s = value.value();
    return globalTamgu->Provideint(c_bytetocharposition(USTR(s),callfunc->Evaluate(0, contextualpattern, idthread)->Integer()));
}

Tamgu* Tamgua_string::MethodLatin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string  s = value.value();
    return new Tamgua_string(s_utf8_to_latin(USTR(s), s.size()));
}

Tamgu* Tamgua_string::MethodUtf8(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long table = 0;
    string  s = value.value();
    if (callfunc->Size() != 0) {
        table = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        return new Tamgua_string(conversion_latin_table_to_utf8(table, USTR(s), s.size()));
    }
    return new Tamgua_string(conversion_latin_to_utf8(USTR(s), s.size()));
}

Tamgu* Tamgua_string::MethodDos(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string  s = value.value();
    return new Tamgua_string(s_utf8_to_dos(USTR(s)));
}

Tamgu* Tamgua_string::MethodDostoutf8(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string  s = value.value();
    return new Tamgua_string(s_dos_to_utf8(USTR(s)));
}

Tamgu* Tamgua_string::MethodLeft(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long nbchar = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        //Second parameter is a position
    string  s = value.value();
    return new Tamgua_string(s_left(s, nbchar));
}

Tamgu* Tamgua_string::MethodRight(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long nbchar = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    string  s = value.value();
    return new Tamgua_string(s_right(s, nbchar));
}

Tamgu* Tamgua_string::MethodMid(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //Second parameter is a position
    long pos = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        //Third parameter is a number of characters
    long nbchar = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    string  s = value.value();
    return new Tamgua_string(s_middle(s, pos, nbchar));
}

Tamgu* Tamgua_string::MethodIslower(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string  s = value.value();
    return booleantamgu[s_is_lower(s)];
}

Tamgu* Tamgua_string::MethodIsupper(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string  s = value.value();
    return booleantamgu[s_is_upper(s)];
}

Tamgu* Tamgua_string::MethodIsemoji(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string  s = value.value();
    return booleantamgu[s_is_emoji(s)];
}

Tamgu* Tamgua_string::MethodEmoji(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string  s = value.value();
    return new Tamgua_string(Emoji(s));
}

Tamgu* Tamgua_string::MethodIsdigit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string  s = value.value();
    return booleantamgu[s_is_digit(s)];
}

Tamgu* Tamgua_string::MethodIsalpha(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string  s = value.value();
    return booleantamgu[s_is_alpha(s)];
}

Tamgu* Tamgua_string::MethodIsconsonant(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string  s = value.value();
    return booleantamgu[s_is_consonant(USTR(s))];
}

Tamgu* Tamgua_string::MethodIsvowel(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string  s = value.value();
    return booleantamgu[s_is_vowel(USTR(s))];
}

Tamgu* Tamgua_string::MethodDeaccentuate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string  s = value.value();
    return new Tamgua_string(s_deaccentuate(USTR(s)));
}

Tamgu* Tamgua_string::MethodIspunctuation(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string  s = value.value();
    return booleantamgu[s_is_punctuation(s)];
}

Tamgu* Tamgua_string::MethodLower(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string  s = value.value();
    return new Tamgua_string(s_to_lower(s));
}

Tamgu* Tamgua_string::MethodUpper(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string  s = value.value();
    return new Tamgua_string(s_to_upper(s));
}

Tamgu* Tamgua_string::MethodTrim(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string  s = value.value();
    return new Tamgua_string(Trim(s));
}

Tamgu* Tamgua_string::MethodTrimright(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string  s = value.value();
    return new Tamgua_string(Trimright(s));
}

Tamgu* Tamgua_string::MethodTrimleft(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string  s = value.value();
    return new Tamgua_string(Trimleft(s));
}

Tamgu* Tamgua_string::MethodReverse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string v = value.value();
    return new Tamgua_string(s_revert(v));
}

Tamgu* Tamgua_string::MethodRemovefirst(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* knb = callfunc->Evaluate(0, contextualpattern, idthread);
    long id = knb->Integer();
        //On retire en tete
    string vl = value.value();
    long iv = size_c(vl);
    if (id >= iv)
        return aEMPTYSTRING;
    
    return new Tamgua_string(s_right(vl, iv - id));
}

Tamgu* Tamgua_string::MethodRemovelast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* knb = callfunc->Evaluate(0, contextualpattern, idthread);
    long id = knb->Integer();
        //On retire en tete
    string vl = value.value();
    long iv = size_c(vl);
    
    if (id >= iv)
        return aEMPTYSTRING;
    
    return new Tamgua_string(s_left(vl, iv - id));
}

Tamgu* Tamgua_string::MethodFormat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    char buffer[101];
    string sformat = value.value();
    
        //In this case, we might have more than two parameters...
    string e;
    buffer[0] = '%';
    for (long i = callfunc->Size() - 1; i >= 0; i--) {
        e = callfunc->Evaluate(i, contextualpattern, idthread)->String();
        sprintf_s(buffer + 1, 100, "%ld", i + 1);
        sformat = s_replacestrings(sformat, buffer, e);
    }
    return new Tamgua_string(sformat);
}

Tamgu* Tamgua_string::MethodReplace(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //Replace an element with another
    
    Tamgu* treg = callfunc->Evaluate(0, contextualpattern, idthread);
    if (treg->isRegular()) {
        if (treg->Type() == a_treg) {
            wstring rep = callfunc->Evaluate(1, contextualpattern, idthread)->UString();
            wstring w = UString();
            vector<long> values;
            treg->searchall(w,values);
            
            for (long i=values.size()-2; i>=0; i-=2)
                w=w.substr(0,values[i])+rep+w.substr(values[i+1],w.size()-values[i+1]);
            return globalTamgu->Provideustring(w);
        }
#ifdef Tamgu_REGEX
        string rep = callfunc->Evaluate(1, contextualpattern, idthread)->String();
        string w = value.value();
        
        w = treg->replace(w, rep);
        return new Tamgua_string(w);
#endif
    }
    
    string str = value.value();
    
    string reg = treg->String();
    if (reg == "")
        return new Tamgua_string(str);
    
    string rep = callfunc->Evaluate(1, contextualpattern, idthread)->String();
    
    str = s_replacestring(str, reg, rep);
    return new Tamgua_string(str);
}

Tamgu* Tamgua_string::MethodInsert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    size_t idx;
    
        //First parameter should be a vector
    
        //second parameter is anything
    idx = -1;
    string val;
    agnostring ret(value.value());
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
    return new Tamgua_string(ret);
}

Tamgu* Tamgua_string::MethodClear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    value.clear();
    return aTRUE;
}

Tamgu* Tamgua_string::MethodFill(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    
    Tamgu* res = callfunc->Evaluate(0, contextualpattern, idthread);
    Tamgu* kval = callfunc->Evaluate(1, contextualpattern, idthread);
    long sz = res->Integer();
    if (sz <= 0)
        return new Tamgua_string("");
    
    string sval = kval->String();
    
    if (sval != "" && sval.size() == 1) {
        char* s = (char*)malloc(sz + 1);
        char c = ' ';
        c = sval[0];
        memset(s, c, sz);
        s[sz] = 0;
        value = s;
        free(s);
    }
    else {
        string s;
        while (sz > 0) {
            s += sval;
            sz--;
        }
        value = s;
    }
    return this;
}


Exporting Tamgu* Tamgua_string::Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {
    Tamgu* a;
    
    Tamgustring* it = globalTamgu->Providestring("");
    it->Setreference();
    
    uchar addvalue = 0;
    Locking* lock = NULL;
    if (context != aNOELEMENT) {
        addvalue = Selecttype(context);
        if (addvalue)
            lock = new Locking((TamguObject*)context);
    }
    
    
    recipient->Putvalue( it, idthread);
    
    agnostring v(value.value());
    v.begin();
    while (!v.end()) {
        it->value = v.nextemoji();
        
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
            if (lock != NULL)
                delete lock;
            return globalTamgu->Errorobject(idthread);
        }
        
        context = Storealongtype(context, a, idthread, addvalue);
        a->Release();
    }
    
    it->Resetreference();
    recipient->Forcedclean();
    
    if (lock != NULL)
        delete lock;
    return context;
}

Exporting Tamgu* Tamgua_string::Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct) {
    
    Tamgu* returnval;
    
    bool first = false;
    Tamgustring* key = globalTamgu->Providestring("");
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
    agnostring v(value.value());
    v.begin();
    while (!v.end()) {
        key->value = v.nextemoji();
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

Tamgu* Tamgua_string::MethodPadding(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long res = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    string sval = callfunc->Evaluate(1, contextualpattern, idthread)->String();
    bool direction = true;
    if (callfunc->Size() == 3)
        direction = callfunc->Evaluate(2, contextualpattern, idthread)->Boolean();
    
    long sz = res - value.size();
    if (sz <= 0)
        return this;
    
    string sres;
    if (sval != "" && sval.size() == 1) {
        char* s = (char*)malloc(sz + 1);
        char c = ' ';
        c = sval[0];
        memset(s, c, sz);
        s[sz] = 0;
        sres = s;
        free(s);
    }
    else {
        string s;
        while (sz > 0) {
            s += sval;
            sz--;
        }
        sres = s;
    }
    if (direction)
        value.concat(sres);
    else
        value = sres + value.value();
    
    return this;
}




Tamgu* Tamgua_string::andset(Tamgu* a, bool autoself) {
    string s = a->String();
    string u;

    string vl = value.value();
    
    long m = min(s.size(), value.size());
    for (long i = 0; i < m; i++) {
        if (s[i] == vl[i])
            u += s[i];
    }
    if (autoself) {
        value = u;
        return this;
    }
    
    return new Tamgua_string(u);
}

Tamgu* Tamgua_string::xorset(Tamgu* a, bool autoself) {
    string s = a->String();
    string u;
    string vl = value.value();
    
    long m = min(s.size(), vl.size());
    for (long i = 0; i < m; i++) {
        if (s[i] != vl[i])
            u += vl[i];
    }
    
    if (autoself) {
        value = u;
        return this;
    }
    
    return new Tamgua_string(u);
}

Tamgu* Tamgua_string::MethodJamo(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring s = UString();
    wstring w;
    if (callfunc->Size() == 0 || callfunc->Evaluate(0, contextualpattern, idthread)->Boolean() == false) {
        w = s_split_jamo(s);
        if (contextualpattern->isVectorContainer()) {
            Tamguuvector* vs = (Tamguuvector*)Selectauvector(contextualpattern);
            Locking _lock(vs);
            wstring s;
            for (long i = 0; i < w.size(); i++) {
                s = w[i];
                vs->values.push_back(s);
            }
            return vs;
        }
        
        return globalTamgu->Provideustring(w);
    }
    
        //Else combine...
    w = UString();
    w = s_combine_jamo(w);
    return globalTamgu->Provideustring(w);
}

Tamgu* Tamgua_string::MethodIsJamo(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring s = UString();
    return booleantamgu[s_is_jamo(s)];
}

Tamgu* Tamgua_string::MethodIsHangul(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring s = UString();
    return booleantamgu[s_is_hangul(s)];
}


Tamgu* Tamgua_string::MethodTransliteration(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string c;
    long i = 0;
    
    string str = value.value();
    
    if (contextualpattern->isVectorContainer()) {
        Tamgusvector* vs = (Tamgusvector*)Selectasvector(contextualpattern);
        Locking _lock(vs);
        while (i < value.size()) {
            c = c_translate(USTR(str), i);
            vs->values.push_back(c);
        }
        return vs;
    }
    
    if (str.size())
        c = c_translate(USTR(str), i);
    
    return new Tamgua_string(c);
}

Tamgu* Tamgua_string::MethodNormalizeHangul(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring s = UString();
    return globalTamgu->Provideustring(s_hangul_normalize(s));
}


Tamgu* Tamgua_string::MethodMultiSplit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    
        //Third parameter can be a vector
    
    Tamgu* kvect = NULL;
    string thesplitter;
    vector<long> ipositions;
    string vl = value.value();
    size_t pos = 0, found;
    long i = 0;
        //We first fill our vector with initial values...
    while (!pos && i < callfunc->Size()) {
        thesplitter = callfunc->Evaluate(i, contextualpattern, idthread)->String();
        pos = 0;
        while (pos != string::npos) {
            found = s_findbyte(vl, thesplitter, pos);
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
        thesplitter = callfunc->Evaluate(i, contextualpattern, idthread)->String();
        pos = 0;
        
        while (pos != string::npos) {
            found = s_findbyte(vl, thesplitter, pos);
            if (found != string::npos) {
                bool add = true;
                pos = found + thesplitter.size();
                for (long j = 0; j < ipositions.size(); j += 2) {
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
    if (contextualpattern->Type() == a_svector || !contextualpattern->isVectorContainer())
        kvect = Selectasvector(contextualpattern);
    else
        kvect = Selectacontainer(contextualpattern, idthread);
    
    
    pos = 0;
    for (i = 0; i < ipositions.size(); i += 2) {
        if (pos != ipositions[i]) {
            thesplitter = vl.substr(pos, ipositions[i] - pos);
            kvect->storevalue(thesplitter);
        }
        pos = ipositions[i + 1];
    }
    
    if (ipositions.back() < vl.size())
        kvect->storevalue(vl.substr(ipositions.back(), vl.size() - ipositions.back()));
    
    return kvect;
}


Tamgu* Tamgua_string::MethodSplit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    
        //Third parameter can be a vector
    
    Tamgu* kvect = NULL;
    
    string localvalue;
    string thestr = value.value();
    string thesplitter;
    
    if (contextualpattern->Type() == a_svector || !contextualpattern->isVectorContainer())
        kvect = Selectasvector(contextualpattern);
    else
        kvect = Selectacontainer(contextualpattern, idthread);
    
    if (callfunc->Size() == 0) {
        unsigned char c;
            //we split along space characters...
        for (long i = 0; i < thestr.size(); i++) {
            c = thestr[i];
            if (c <= 32) {
                if (thesplitter.size()) {
                    kvect->storevalue(thesplitter);
                    thesplitter = "";
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
        
        wstring w = UString();
        
        vector<long> values;
        
        tamgusplitter->searchall(w,values);
        if (!values.size())
            return kvect;
        
            //Now we extract the strings in between
        long pos = 0;
        wstring sub;
        for (long i = 0; i < values.size(); i += 2) {
            sub = w.substr(pos, values[i]-pos);
            if (sub != L"")
                kvect->storevalue(sub);
            pos=values[i+1];
        }
        
        if (pos < w.size()) {
            sub = w.substr(pos,w.size()-pos);
            kvect->storevalue(sub);
        }
        
        return kvect;
    }
    
    thesplitter=tamgusplitter->String();
    
        //Second parameter is the splitter string
    if (thesplitter == "") {
        if (kvect->Type() == Tamgubvector::idtype) {
            for (long i = 0; i < thestr.size(); i++)
                kvect->storevalue((unsigned char)thestr[i]);
            return kvect;
        }
        
        agnostring athestr(thestr);
        athestr.begin();
        if (kvect->Type() == Tamguivector::idtype) {
            while (!athestr.end())
                kvect->storevalue((long)athestr.nextcode());
            return kvect;
        }
        
        while (!athestr.end())
            kvect->storevalue(athestr.nextemoji());
        return kvect;
    }
    
    size_t pos = 0;
    size_t found = 0;
    while (pos != string::npos) {
        found = s_findbyte(thestr, thesplitter, pos);
        if (found != string::npos) {
            localvalue = thestr.substr(pos, found - pos);
            if (localvalue != "")
                kvect->storevalue(localvalue);
            pos = found + thesplitter.size();
        }
        else
            break;
    }
    
    localvalue = thestr.substr(pos, thestr.size() - pos);
    if (localvalue != "")
        kvect->storevalue(localvalue);
    
    return kvect;
}

Tamgu* Tamgua_string::MethodSplite(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    
        //Third parameter can be a vector
    
    Tamgu* kvect = NULL;
    
    string localvalue;
    string thestr = value.value();
    
    string thesplitter;
    
    if (contextualpattern->Type() == a_svector || !contextualpattern->isVectorContainer())
        kvect = Selectasvector(contextualpattern);
    else
        kvect = Selectacontainer(contextualpattern, idthread);
    
    if (callfunc->Size() == 0) {
        unsigned char c;
            //we split along space characters...
        for (long i = 0; i < thestr.size(); i++) {
            c = thestr[i];
            if (c <= 32) {
                if (thesplitter.size()) {
                    kvect->storevalue(thesplitter);
                    thesplitter = "";
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
        
        wstring w = UString();
        
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
    
    thesplitter=tamgusplitter->String();
    
        //Second parameter is the splitter string
    if (thesplitter == "") {
        if (kvect->Type() == Tamgubvector::idtype) {
            for (long i = 0; i < thestr.size(); i++)
                kvect->storevalue((unsigned char)thestr[i]);
            return kvect;
        }
        
        agnostring athestr(thestr);
        athestr.begin();
        if (kvect->Type() == Tamguivector::idtype) {
            while (!athestr.end())
                kvect->storevalue((long)athestr.nextcode());
            return kvect;
        }
        
        while (!athestr.end())
            kvect->storevalue(athestr.nextemoji());
        return kvect;
    }
    
    size_t pos = 0;
    size_t found = 0;
    while (pos != string::npos) {
        found = s_findbyte(thestr, thesplitter, pos);
        if (found != string::npos) {
            localvalue = thestr.substr(pos, found - pos);
            kvect->storevalue(localvalue);
            pos = found + thesplitter.size();
        }
        else
            break;
    }
    
    localvalue = thestr.substr(pos, thestr.size() - pos);
    if (localvalue != "")
        kvect->storevalue(localvalue);
    
    return kvect;
}

Tamgu* Tamgua_string::MethodSlice(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* ke = Selectavector(contextualpattern);
    long num = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    if (num <= 0)
        return ke;
    
    long i;
    long beg = 0;

    string vl = value.value();
    long sz = vl.size();
    if (s_is_utf8(USTR(vl), sz)) {
        wstring w;
        s_utf8_to_unicode(w, USTR(vl), vl.size());
        
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
    
    string sub;
    for (i = num; i < sz; i+=num) {
        sub = vl.substr(beg,i-beg);
        beg = i;
        ke->storevalue(sub);
    }
    if (i-beg>0) {
        sub = vl.substr(beg,i-beg);
        ke->storevalue(sub);
    }
    return ke;
}

Tamgu* Tamgua_string::MethodNgrams(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long num = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    if (num <= 0)
        return Selectasvector(contextualpattern);
    
    long i, j, mx;
    
    agnostring s(value.value());
    Tamgu* ke = Selectasvector(contextualpattern);
    string u;
    
    if (s.isutf8()) {
        vector<string> v;
        s.begin();
        while (!s.end())
            v.push_back(s.nextemoji());
        mx = v.size() - num + 1;
        for (i = 0; i < mx; i++) {
            u = "";
            for (j = i; j < i + num; j++)
                u += v[j];
            ke->storevalue(u);
        }
        return ke;
    }
    string& sb = (string&)s;
    mx = s.size() - num + 1;
    for (i = 0; i < mx; i++) {
        u = "";
        for (j = i; j < i + num; j++)
            u += sb[j];
        ke->storevalue(u);
    }
    return ke;
}

Tamgu* Tamgua_string::MethodDoubleMetaphone(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    
    Tamgu* kvect = Selectasvector(contextualpattern);
    
    string s = value.value();
    DoubleMetaphone(s,((Tamgusvector*)kvect)->values);
    return kvect;
}

Tamgu* Tamgua_string::MethodHash(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static const std::collate<char>& coll = std::use_facet<std::collate<char> >(loc);
    
    string s = value.value();
    
    if (s == "")
        return aNULL;
    long myhash = coll.hash(s.data(), s.data() + s.length());
    return globalTamgu->Provideint(myhash);
}


Tamgu* Tamgua_string::MethodTokenize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    
    agnostring thestr(value.value());
    
    bool comma = false;
    bool separator = false;
    bool keepwithdigit = false;
    
    vector<string> rules;
    if (callfunc->Size() >= 1) {
        comma = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
        if (callfunc->Size() >= 2) {
            separator = callfunc->Evaluate(1, contextualpattern, idthread)->Boolean();
            if (callfunc->Size() >= 3) {
                keepwithdigit = callfunc->Evaluate(2, contextualpattern, idthread)->Boolean();
                if (callfunc->Size() == 4) {
                    Tamgu* vect = callfunc->Evaluate(3, contextualpattern, idthread);
                    for (long i = 0; i< vect->Size(); i++)
                        rules.push_back(vect->getstring(i));
                }
            }
        }
    }
    
    Tamgu* kvect = Selectasvector(contextualpattern);
    
    Locking _lock((TamguObject*)kvect);
    if (rules.size()) {
        if (!thestr.r_tokenize(((Tamgusvector*)kvect)->values,comma, separator, keepwithdigit, rules)) {
            string s = ((Tamgusvector*)kvect)->values[0];
            kvect->Clear();
            kvect->Release();
            return globalTamgu->Returnerror(s,idthread);
        }
    }
    else
        thestr.tokenize(((Tamgusvector*)kvect)->values,comma, separator, keepwithdigit);
    return kvect;
}

Tamgu* Tamgua_string::MethodStokenize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    
    agnostring thestr(value.value());

    Tamgu* keeps = NULL;
    
    if (callfunc->Size() == 1) {
        keeps = callfunc->Evaluate(0, contextualpattern, idthread);
        if (!keeps->isContainer())
            return globalTamgu->Returnerror("TAMGUI(665): Expecting a container in STOKENIZE", idthread);
    }
    
    vector<string> v;
    map<string, bool> k;
    if (keeps != NULL) {
        TamguIteration* itr = (TamguIteration*)keeps->Newiteration(false);
        for (itr->Begin(); itr->End() == aFALSE; itr->Next())
            k[itr->Valuestring()] = true;
        itr->Release();
    }
    thestr.stokenize(v, k);
    
    Tamgu* kvect = Selectasvector(contextualpattern);
    if (kvect->Type() == Tamgusvector::idtype) {
        Locking _lock((TamguObject*)kvect);
        ((Tamgusvector*)kvect)->values = v;
    }
    else {
        TamguConstString localstring("");
        
        for (long i = 0; i < v.size(); i++) {
            localstring.value = v[i];
            kvect->Push(&localstring);
        }
    }
    return kvect;
}
