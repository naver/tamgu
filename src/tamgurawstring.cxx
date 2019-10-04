/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgurawstring.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "codeparse.h"
#include "tamgu.h"
#include "tamgurawstring.h"
#include "constobjects.h"
#include "tamguivector.h"
#include "tamgufvector.h"
#include "tamgubvector.h"
#include "tamgusvector.h"
#include "tamguvector.h"
#include "tamguconstants.h"
#include "tamgubyte.h"
#include "tamguustring.h"
#include "fractalhmap.h"
#include "instructions.h"
#include "tamgumap.h"
#include "compilecode.h"
#include "tamguautomaton.h"
#include "tamgufile.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<rawstringMethod>  Tamgurawstring::methods;
Exporting hmap<string, string> Tamgurawstring::infomethods;
Exporting bin_hash<unsigned long> Tamgurawstring::exported;

Exporting short Tamgurawstring::idtype = 0;

#ifdef DOSOUTPUT
static bool dosoutput = true;
static void setdosoutput(bool d) { dosoutput = d; }
#define conversion2Dos(x) dosoutput?s_utf8_to_dos(USTR(x)):x
#endif

char RawStringIndexes(char* svalue, long sz, Tamgu* index, long& ileft, long& iright, short idthread) {
    
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
    
    char* strleft;
    
    if (left->isRegular()) {
        //this is a regular expression...
        string val(svalue, sz);
        if (sleft) {
            //we need the last one...
            if (!left->searchlast(val, ileft, iright))
                return 0;
        }
        else {
            if (!left->search(val, ileft, iright))
                return 0;
        }
    }
    else {
        if (left->isString()) {
            string sub = left->String();
            if (releft)
                left->Release();
            
            //then we are looking for a substring
            if (sleft)
                strleft = strrstr(svalue, STR(sub), sz, sub.size());
            else
                strleft = strstr(svalue, STR(sub));
            
            if (strleft == NULL)
                return 0;

            ileft = strleft-svalue;
            
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
        return 1;
    }
    
    if (right->isRegular()) {

        string val(svalue, sz);
        
        //this is a regular expression...
        long r = iright;
        
        if (sright) {
            //we need the last one...
            if (!right->searchlast(val, r, iright, r))
                return 0;
        }
        else {
            if (!right->search(val, r, iright, r))
                return 0;
        }
        if (iright <= ileft)
            return 0;
        return 2;
    }
    
    if (right->isString()) {
        string sub = right->String();
        if (reright)
            right->Release();
        
        if (sright)
            strleft = strrstr(svalue, STR(sub), sz, sub.size());
        else {
            if (iright != -1)
                strleft = strstr(svalue + iright, STR(sub));
            else
                strleft = strstr(svalue + ileft, STR(sub));
        }
        
        if (strleft == NULL)
            return 0;
        
        iright = strleft-svalue+sub.size();
        if (iright<=ileft)
            return 0;
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
        else {
            iright += right->Integer();
            if (reright)
                right->Release();
        }
    }
    
    if (iright > sz)
        iright = sz;
    else
        if (iright <= ileft)
            return 0;
    return 2;
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgurawstring::AddMethod(TamguGlobal* global, string name, rawstringMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgurawstring::Setidtype(TamguGlobal* global) {
        Tamgurawstring::idtype = global->Getid("rawstring");
    }

   bool Tamgurawstring::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    Tamgurawstring::idtype = global->Getid("rawstring");

    Tamgurawstring::AddMethod(global, "_initial", &Tamgurawstring::MethodInitial, P_ONE, "_initial(string|int): Set the initial value of the raw string.");

    Tamgurawstring::AddMethod(global, "hash", &Tamgurawstring::MethodHash, P_NONE, "hash(): Return the hash value of a string.");
    Tamgurawstring::AddMethod(global, "reverse", &Tamgurawstring::MethodReverse, P_NONE, "reverse(): reverse the string");
    Tamgurawstring::AddMethod(global, "ord", &Tamgurawstring::MethodOrd, P_NONE, "ord(): return the ASCII code of the first character, or a list of all ASCII code if the recipient is a vector");
    Tamgurawstring::AddMethod(global, "bytes", &Tamgurawstring::MethodBytes, P_NONE, "bytes(): Return the string as a vector of bytes");
    Tamgurawstring::AddMethod(global, "format", &Tamgurawstring::MethodFormat, P_ATLEASTONE, "format(p1,p2,p3): Create a new string from the current string in which each '%x' is associated to one of the parameters, 'x' being the position of that parameter in the argument list. 'x' starts at 1.");
    Tamgurawstring::AddMethod(global, "base", &Tamgurawstring::MethodBase, P_ONE | P_TWO, "base(int b, bool toconvert=true): Return the value corresponding to the string in base b");
    Tamgurawstring::AddMethod(global, "fill", &Tamgurawstring::MethodFill, P_TWO, "fill(int nb,string c): create a string of nb characters c");
    Tamgurawstring::AddMethod(global, "padding", &Tamgurawstring::MethodPadding, P_TWO | P_THREE, "padding(int nb,string c,bool paddattheend): add nb characters c to the current string. Last parameter is optional");
    Tamgurawstring::AddMethod(global, "parse", &Tamgurawstring::MethodParse, P_NONE | P_TWO, "parse(): Parse a string as a piece of code and returns the evaluation as a vector.");
    Tamgurawstring::AddMethod(global, "pop", &Tamgurawstring::MethodPop, P_NONE | P_ONE | P_TWO, "pop(): remove last character");
    Tamgurawstring::AddMethod(global, "buffersize", &Tamgurawstring::MethodSizeb, P_NONE, "buffersize(): Return the size of the internal buffer");
    Tamgurawstring::AddMethod(global, "resize", &Tamgurawstring::MethodResize, P_NONE, "resize(): Redefine the internal buffer size. Cannot be inferior to string size.");
    Tamgurawstring::AddMethod(global, "lisp", &Tamgurawstring::MethodLisp, P_NONE | P_TWO | P_THREE | P_FOUR | P_FIVE | P_SIX, "lisp(string o,string c,bool comma,bool separator,bool concatenate, svector rules): Parse a string as a parenthetic expressions, o is '(' and c is ')' by default. If 'comma' is true, then the decimal character is ',' otherwise it is '.'. If 'separator' is true then '1,000' is accepted as a number. If 'concatenate' is true then '3a' is a valid token");
    Tamgurawstring::AddMethod(global, "tags", &Tamgurawstring::MethodTags, P_TWO | P_THREE | P_FOUR | P_FIVE | P_SIX, "tags(string o,string c,bool comma,bool separator,bool concatenate, svector rules): Parse a string as a parenthetic expressions, where o and c are string (not characters). If 'comma' is true, then the decimal character is ',' otherwise it is '.'. If 'separator' is true then '1,000' is accepted as a number. If 'concatenate' is true then '3a' is a valid token");
    Tamgurawstring::AddMethod(global, "scan", &Tamgurawstring::MethodScan, P_ONE | P_TWO | P_THREE | P_FOUR, "scan(sub, string sep, bool immediate,string remaining): Find the substrings matching sub, with TRE. 'sep' is a separator between strings. 'immediate' always combines with separator, it means that the matching should start at the first character of the string, default is false. 'remaining' also combines with 'separator', it returns the rest of the string after the section that matched.");
    Tamgurawstring::AddMethod(global, "evaluate", &Tamgurawstring::MethodEvaluate, P_NONE, "evaluate(): evaluate the meta-characters within a string and return the evaluated string.");
    Tamgurawstring::AddMethod(global, "html", &Tamgurawstring::MethodTohtml, P_NONE, "html(): Return the string into an HTML compatible string or as a vector of strings");
    Tamgurawstring::AddMethod(global, "tohtml", &Tamgurawstring::MethodTohtml, P_NONE, "tohtml(): Return the string into an HTML compatible string or as a vector of strings");
    Tamgurawstring::AddMethod(global, "toxml", &Tamgurawstring::MethodToxml, P_NONE, "toxml(): Return the string into an XML compatible string or as a vector of strings");
    Tamgurawstring::AddMethod(global, "levenshtein", &Tamgurawstring::MethodEditdistance, P_ONE | P_TWO, "levenshtein(string s,bool byte): Return the edit distance with 's' according to Levenshtein algorithm. If byte is true, force a byte level comparison. byte is optionnal.");
    Tamgurawstring::AddMethod(global, "editdistance", &Tamgurawstring::MethodEditdistance, P_ONE | P_TWO | P_THREE, "editdistance(string s,bool byte): Return the edit distance with 's'. If byte is true, force a byte level comparison. byte is optionnal.");
    Tamgurawstring::AddMethod(global, "replace", &Tamgurawstring::MethodReplace, P_TWO, "replace(sub,str): Replace the substrings matching sub with str");
    Tamgurawstring::AddMethod(global, "getstd", &Tamgurawstring::MethodGetstd, P_ONE, "getstd(bool): catch or release the standard output");
    Tamgurawstring::AddMethod(global, "geterr", &Tamgurawstring::MethodGeterr, P_ONE, "geterr(bool): catch or release the error output");
    Tamgurawstring::AddMethod(global, "split", &Tamgurawstring::MethodSplit, P_ONE | P_NONE, "split(string splitter): split a string along splitter and store the results  in a vector. If splitter=='', then the string is split into a vector of characters");
    Tamgurawstring::AddMethod(global, "splite", &Tamgurawstring::MethodSplite, P_ONE | P_NONE, "splite(string splitter): split a string along splitter and store the results  in a vector. If splitter=='', then the string is split into a vector of characters. Empty strings are kept in the result.");
    Tamgurawstring::AddMethod(global, "tokenize", &Tamgurawstring::MethodTokenize, P_NONE | P_ONE | P_TWO | P_THREE | P_FOUR, "tokenize(bool comma,bool separator,bool concatenate, svector rules): Segment a string into words and punctuations. If 'comma' is true, then the decimal character is ',' otherwise it is '.'. If 'separator' is true then '1,000' is accepted as a number. If 'concatenate' is true then '3a' is a valid token. rules is a set of tokenization rules that can be first initialized then modified with _getdefaulttokenizerules");
    Tamgurawstring::AddMethod(global, "stokenize", &Tamgurawstring::MethodStokenize, P_NONE | P_ONE, "stokenize(map keeps): Segment a string into words and punctuations, with a keep.");
    Tamgurawstring::AddMethod(global, "count", &Tamgurawstring::MethodCount, P_TWO | P_ONE | P_NONE, "count(string sub,int pos): Count the number of substrings starting at position pos");
    Tamgurawstring::AddMethod(global, "find", &Tamgurawstring::MethodFind, P_TWO | P_ONE, "find(string sub,int pos): Return the position of substring sub starting at position pos");
    Tamgurawstring::AddMethod(global, "rfind", &Tamgurawstring::MethodRfind, P_TWO | P_ONE, "rfind(string sub,int pos): Return the position of substring sub backward starting at position pos");
    Tamgurawstring::AddMethod(global, "removefirst", &Tamgurawstring::MethodRemovefirst, P_ONE, "removefirst(int nb): remove the first nb characters of a string");
    Tamgurawstring::AddMethod(global, "removelast", &Tamgurawstring::MethodRemovelast, P_ONE, "removelast(int nb): remove the last nb characters of a string");
    Tamgurawstring::AddMethod(global, "isutf8", &Tamgurawstring::MethodIsutf8, P_NONE, "isutf8(): Return true is the string is encoded in UTF8");
    Tamgurawstring::AddMethod(global, "slice", &Tamgurawstring::MethodSlice, P_ONE, "slice(int sz): Return a vector of all slices of size sz");
    Tamgurawstring::AddMethod(global, "ngrams", &Tamgurawstring::MethodNgrams, P_ONE, "ngrams(int r): Return a vector of all ngrams of rank r");
    Tamgurawstring::AddMethod(global, "utf8", &Tamgurawstring::MethodUtf8, P_NONE | P_ONE, "utf8(int table): convert a LATIN string into UTF8. table is optional, by default it is ISO/IEC 8859 part 1.");
    Tamgurawstring::AddMethod(global, "latin", &Tamgurawstring::MethodLatin, P_NONE, "latin(): convert an UTF8 string in LATIN");
    Tamgurawstring::AddMethod(global, "dos", &Tamgurawstring::MethodDos, P_NONE, "dos(): convert a string into DOS encoding");
    Tamgurawstring::AddMethod(global, "dostoutf8", &Tamgurawstring::MethodDostoutf8, P_NONE, "dostoutf8(): convert a DOS string into UTF8");
    Tamgurawstring::AddMethod(global, "left", &Tamgurawstring::MethodLeft, P_ONE, "left(int nb): return the first nb characters of a string");
    Tamgurawstring::AddMethod(global, "byteposition", &Tamgurawstring::MethodByteposition, P_ONE, "byteposition(int pos): Convert a character position into a byte position");
    Tamgurawstring::AddMethod(global, "charposition", &Tamgurawstring::MethodCharposition, P_ONE, "charposition(int pos): Convert a byte position into a character position");
    Tamgurawstring::AddMethod(global, "right", &Tamgurawstring::MethodRight, P_ONE, "right(int nb): return the last nb characters of a string");
    Tamgurawstring::AddMethod(global, "mid", &Tamgurawstring::MethodMid, P_TWO, "mid(int pos,int nb): return the nb characters starting at position pos of a string");
    Tamgurawstring::AddMethod(global, "isalpha", &Tamgurawstring::MethodIsalpha, P_NONE, "isalpha(): Test if a string only contains only alphabetical characters");
    Tamgurawstring::AddMethod(global, "isconsonant", &Tamgurawstring::MethodIsconsonant, P_NONE, "isconsonant(): Test if a string only contains consonants");
    Tamgurawstring::AddMethod(global, "isvowel", &Tamgurawstring::MethodIsvowel, P_NONE, "isvowel(): Test if a string only contains only vowels");
    Tamgurawstring::AddMethod(global, "ispunctuation", &Tamgurawstring::MethodIspunctuation, P_NONE, "ispunctuation(): Test if the character is a punctuation");
    Tamgurawstring::AddMethod(global, "isdigit", &Tamgurawstring::MethodIsdigit, P_NONE, "isdigit(): Test if a string only contains digits");
    Tamgurawstring::AddMethod(global, "extract", &Tamgurawstring::MethodExtract, P_ATLEASTTHREE, "extract(int pos,string from,string up1,string up2...): extract substrings between 'from' and 'up1'...'upn' (the shortest string is used). Return a vector of strings");
    Tamgurawstring::AddMethod(global, "isupper", &Tamgurawstring::MethodIsupper, P_NONE, "isupper(): Test if a string only contains uppercase characters");
    Tamgurawstring::AddMethod(global, "islower", &Tamgurawstring::MethodIslower, P_NONE, "islower(): Test if a string only contains lowercase characters");
    Tamgurawstring::AddMethod(global, "upper", &Tamgurawstring::MethodUpper, P_NONE, "upper(): Return the string in upper characters");
    Tamgurawstring::AddMethod(global, "deaccentuate", &Tamgurawstring::MethodDeaccentuate, P_NONE, "deaccentuate(): Remove the accents from accented characters");
    Tamgurawstring::AddMethod(global, "lower", &Tamgurawstring::MethodLower, P_NONE, "lower(): Return the string in lower characters");
    Tamgurawstring::AddMethod(global, "trim", &Tamgurawstring::MethodTrim, P_NONE, "trim(): remove the trailing characters");
    Tamgurawstring::AddMethod(global, "trimleft", &Tamgurawstring::MethodTrimleft, P_NONE, "trimleft(): remove the trailing characters on the left");
    Tamgurawstring::AddMethod(global, "trimright", &Tamgurawstring::MethodTrimright, P_NONE, "trimright(): remove the trailing characters on the right");
    Tamgurawstring::AddMethod(global, "last", &Tamgurawstring::MethodLast, P_NONE, "last(): return last character");
    Tamgurawstring::AddMethod(global, "insert", &Tamgurawstring::MethodInsert, P_ONE | P_TWO, "insert(i,s): insert the string s at i. If i is -1, then insert s between each character in the input string");
    Tamgurawstring::AddMethod(global, "clear", &Tamgurawstring::MethodClear, P_NONE, "clear(): Clean the content of a string.");

    Tamgurawstring::AddMethod(global, "read", &Tamgurawstring::MethodRead, P_ONE, "read(string path): read the file content into the current variable.");


    global->newInstance[Tamgurawstring::idtype] = new Tamgurawstring(global);
    global->RecordMethods(Tamgurawstring::idtype,Tamgurawstring::exported);

    return true;
}


TamguIteration* Tamgurawstring::Newiteration(bool direction) {
    
    if (value==NULL)
        return new TamguIterationrawstring("", direction);

    return new TamguIterationrawstring((char*)value, direction);
}

Tamgu* Tamgurawstring::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    Tamgustring v("");
    Tamgu* a;
    for (long i = 0; i < stringsize; i++) {
        v.value =  value[i];
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


Tamgu* Tamgurawstring::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    
    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        if (value == NULL)
            return v;

        string val = a->String();
        string s((char*)value, stringsize);
        s_findall(s, val, v->values);
        return v;
    }

    if (value == NULL) {
        if (context->isString())
            return aEMPTYSTRING;
        if (context->isNumber())
            return aMINUSONE;
        return aFALSE;
    }

    string val = a->String();
    string kvalue((char*)value,stringsize);
    long r = s_find(kvalue, val, 0);

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

unsigned long Tamgurawstring::EditDistance(Tamgu* e) {
    
    string s2 = e->String();
    unsigned long s1len, s2len, x, y, lastdiag, olddiag;
    s1len = stringsize;
    s2len = s2.size();
    if (stringsize == 0)
        return s2len;

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

Tamgu* Tamgurawstring::Put(Tamgu* idx, Tamgu* ke, short idthread) {


    string s = ke->String();

    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (value == NULL) {
            stringsize = s.size();
            buffersize = stringsize + 1;
            value = new uchar[buffersize];
            strcpy_s((char*)value, buffersize, STR(s));
            return aTRUE;
        }

        if (buffersize <= s.size())
            return globalTamgu->Returnerror("String overflow", idthread);

        strcpy((char*)value, STR(s));
        stringsize = s.size();
    }
    else {
        if (value == NULL) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong key in a container or a string access", idthread);
            return aFALSE;
        }

        long ileft = -1, iright = -1;
        char res = RawStringIndexes((char*)value, stringsize, idx, ileft, iright, idthread);

        if (res == 0) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong key in a container or a string access", idthread);
            return aFALSE;
        }

        if (res == 1)
            iright = ileft + 1;

        //the size is:
        long computesize = stringsize - (iright - ileft) + s.size();
        if (computesize > buffersize)
            return globalTamgu->Returnerror("String overflow", idthread);

        stringsize = computesize;
        s += (char*)value + iright;
        value[ileft] = 0;
        strcat((char*)value, STR(s));
        return aTRUE;
    }
    return aTRUE;
}

Tamgu* Tamgurawstring::Eval(Tamgu* context, Tamgu* idx, short idthread) {
    if (!idx->isIndex() || context == idx)
        return this;

    if (value == NULL)
        return aNULL;

    long ileft = -1, iright = -1;

    //char res = StringIndexes(v, idx, dom, ileft, iright, idthread, callfunc);
    char res = RawStringIndexes((char*)value, stringsize, idx, ileft, iright, idthread);

    if (res == 0) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong key in a container or a string access", idthread);
        return aNOELEMENT;
    }

    if (res == 1) {
        char v[] = { (char)value[ileft], 0 };
        return globalTamgu->Providestring(v);
    }

    if (iright == stringsize)
        return globalTamgu->Providestring((char*)value + ileft);

    uchar c = value[iright];
    value[iright] = 0;
    idx = globalTamgu->Providestring((char*)value + ileft);
    value[iright] = c;
    return idx;
}

//------------------------------------------------------------------------------------------------------------------
Tamgu* Tamgurawstring::MethodOrd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring s = UString();
    if (s.size() >= 1) {
        if (contextualpattern->isNumber())
            return globalTamgu->Provideint((int)s[0]);

        if (contextualpattern->isVectorContainer() || s.size() > 1) {
            Tamgu* kvect = Selectaivector(contextualpattern);
            if (kvect->Type() == a_ivector) {
                Locking _lock((TamguObject*)kvect);
                for (int i = 0; i < s.size(); i++)
                    ((Tamguivector*)kvect)->values.push_back((long)s[i]);
            }
            else {
                TamguConstInt loc(0);
                for (int i = 0; i < s.size(); i++) {
                    loc.value = (long)s[i];
                    kvect->Push(&loc);
                }
            }
            return kvect;
        }

        return globalTamgu->Provideint((long)s[0]);
    }

    return aNULL;
}

//------------------------------------------------------------------------------------------------------------------
Tamgu* Tamgurawstring::MethodIsutf8(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (value == NULL)
        return aFALSE;

    return booleantamgu[s_is_utf8(value, stringsize)];
}

Tamgu* Tamgurawstring::MethodEvaluate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string thestr;
    EvaluateMetaInString(thestr,(char*)value);
    
    return globalTamgu->Providestring(thestr);
}

Tamgu* Tamgurawstring::MethodScan(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    
    Tamgu* rgx = callfunc->Evaluate(0, contextualpattern, idthread);
    Au_automate* a = getautomate(rgx);
    if (a == NULL)
        return globalTamgu->Returnerror("Wrong grammar definition");
    
    wstring reg;
    sc_utf8_to_unicode(reg, value, stringsize);
    
    wstring sep = L"";
    bool fsep=false;
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
        return globalTamgu->Provideustring(reg);
    }
    
    return booleantamgu[a->match(reg)];
}


void XNBrowse(x_node* xn, Tamgu* kf);
Tamgu* Tamgurawstring::MethodParse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
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

Tamgu* Tamgurawstring::MethodLisp(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string str = String();
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

Tamgu* Tamgurawstring::MethodTags(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string str = String();
    string copen("(");
    string cclose(")");
    
    vector<string> rules;

    long nbparams = callfunc->Size();
    
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



Tamgu* Tamgurawstring::MethodCount(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
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

    agnostring str(String());
    if (str == "")
        return aZERO;

    string sub = substr->String();
    return globalTamgu->Provideint(s_count(str, sub, i));
}

Tamgu* Tamgurawstring::MethodFind(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* ksub = callfunc->Evaluate(0, contextualpattern, idthread);

    if (!stringsize) {
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
    
    string str((char*)value,stringsize);

    string sub = ksub->String();
    i = s_find(str, sub, i);
    
    if (i == -1)
        return aMINUSONE;
    
    return globalTamgu->Provideint(i);
}

Tamgu* Tamgurawstring::MethodRfind(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* ksub = callfunc->Evaluate(0, contextualpattern, idthread);
    
    //we search for a substring starting at position idx
    if (!stringsize) {
        if (contextualpattern->isVectorContainer())
            return Selectavector(contextualpattern);
        return aMINUSONE;
    }

    long i = -1;
    if (callfunc->Size() == 2)
        i = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    
    
    if (contextualpattern->isVectorContainer()) {
        Tamgu* kvect = Selectavector(contextualpattern);
        long j;

        wstring str=UString();
        
        if (i == -1)
            i = 0;
        
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
        i = stringsize;

    if (ksub->isRegular()) {
        wstring str=UString();
        long e;
        if (!ksub->searchlast(str,i,e,i))
            return aMINUSONE;
        return globalTamgu->Provideint(i);
    }
    
    string str((char*)value, stringsize);
    string sub = ksub->String();
    i = s_rfind(str, sub, i);
    
    if (i == string::npos)
        return aMINUSONE;
    
    return globalTamgu->Provideint(i);
}


Tamgu* Tamgurawstring::MethodByteposition(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    agnostring s(String());
    return globalTamgu->Provideint(s.chartobyteposition(callfunc->Evaluate(0, contextualpattern, idthread)->Integer()));
}

Tamgu* Tamgurawstring::MethodCharposition(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    agnostring s(String());
    return globalTamgu->Provideint(s.bytetocharposition(callfunc->Evaluate(0, contextualpattern, idthread)->Integer()));
}

Tamgu* Tamgurawstring::MethodTokenize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    
    agnostring thestr(String());
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

Tamgu* Tamgurawstring::MethodStokenize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    agnostring thestr(String());
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

        for (int i = 0; i < v.size(); i++) {
            localstring.value = v[i];
            kvect->Push(&localstring);
        }
    }
    return kvect;
}

Tamgu* Tamgurawstring::MethodSplit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    //Third parameter can be a vector

    Tamgu* kvect = NULL;

    string localvalue;
    string thestr =  String();
    string thesplitter;
    
    if (contextualpattern->Type() == a_svector || !contextualpattern->isVectorContainer())
        kvect = Selectasvector(contextualpattern);
    else
        kvect = Selectacontainer(contextualpattern, idthread);
    
    if (callfunc->Size() == 0) {
        unsigned char c;
        //we split along space characters...
        for (int i = 0; i < thestr.size(); i++) {
            c = thestr[i];
            if (c <= 32) {
                if (thesplitter.size()) {
                    kvect->storevalue(thesplitter);
                    thesplitter = "";
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

        if (tamgusplitter->Type() == a_preg) {
            string w = String();
            tamgusplitter->splitting(kvect, w);
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
        for (int i = 0; i < thestr.size(); i++)
            kvect->storevalue((unsigned char)thestr[i]);
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

Tamgu* Tamgurawstring::MethodSplite(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    //Third parameter can be a vector

    Tamgu* kvect = NULL;

    string localvalue;
    string thestr =  String();
    string thesplitter;
    if (contextualpattern->Type() == a_svector || !contextualpattern->isVectorContainer())
        kvect = Selectasvector(contextualpattern);
    else
        kvect = Selectacontainer(contextualpattern, idthread);
    
    if (callfunc->Size() == 0) {
        unsigned char c;
        //we split along space characters...
        for (int i = 0; i < thestr.size(); i++) {
            c = thestr[i];
            if (c <= 32) {
                if (thesplitter.size()) {
                    kvect->storevalue(thesplitter);
                    thesplitter = "";
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

        if (tamgusplitter->Type() == a_preg) {
            string w = String();
            tamgusplitter->splitting(kvect, w);
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
        for (int i = 0; i < thestr.size(); i++)
            kvect->storevalue((unsigned char)thestr[i]);
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

Tamgu* Tamgurawstring::MethodNgrams(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (value==NULL)
        return Selectasvector(contextualpattern);

    long num = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    if (num <= 0)
        return Selectasvector(contextualpattern);

    Tamgu* ke = Selectasvector(contextualpattern);
    long i, j, mx;

    string u;

    string sb((char*)value);
    mx = stringsize - num + 1;
    for (i = 0; i < mx; i++) {
        u = "";
        for (j = i; j < i + num; j++)
            u += sb[j];
        ke->storevalue(u);
    }
    return ke;
}

Tamgu* Tamgurawstring::MethodSlice(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* ke = Selectavector(contextualpattern);
    long num = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    if (num <= 0)
        return ke;
    
    long i;
    long beg = 0;
    
    long sz = stringsize;
    if (s_is_utf8(value, sz)) {
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
    
    string val=String();
    string sub;
    for (i = num; i < sz; i+=num) {
        sub = val.substr(beg,i-beg);
        beg = i;
        ke->storevalue(sub);
    }
    if (i-beg>0) {
        sub = val.substr(beg,i-beg);
        ke->storevalue(sub);
    }
    return ke;
}

Tamgu* Tamgurawstring::MethodHash(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static const std::collate<char>& coll = std::use_facet<std::collate<char> >(loc);
    string s =  String();
    if (s == "")
        return aNULL;
    long myhash = coll.hash(s.data(), s.data() + s.length());
    return globalTamgu->Provideint(myhash);
}

Tamgu* Tamgurawstring::MethodLatin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    agnostring thestr(String());
    return globalTamgu->Providestring(thestr.utf8tolatin());
}

Tamgu* Tamgurawstring::MethodUtf8(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    long table = 0;
    if (callfunc->Size() != 0) {
        table = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        return globalTamgu->Providestring(conversion_latin_table_to_utf8(table, value, stringsize));
    }
    return globalTamgu->Providestring(conversion_latin_to_utf8(value, stringsize));
}

Tamgu* Tamgurawstring::MethodDos(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    agnostring thestr(String());
    return globalTamgu->Providestring(thestr.utf8todos());
}

Tamgu* Tamgurawstring::MethodDostoutf8(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    agnostring thestr(String());
    return globalTamgu->Providestring(thestr.dostoutf8());
}

Tamgu* Tamgurawstring::MethodLeft(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    
    long nbchar = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    agnostring thestr(String());
    //Second parameter is a position
    return globalTamgu->Providestring(thestr.left(nbchar));
}

Tamgu* Tamgurawstring::MethodRight(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    long nbchar = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    agnostring thestr(String());
    return globalTamgu->Providestring(thestr.right(nbchar));
}

Tamgu* Tamgurawstring::MethodMid(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    //Second parameter is a position
    long pos = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    //Third parameter is a number of characters
    long nbchar = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    agnostring thestr(String());
    return globalTamgu->Providestring(thestr.extract(pos, nbchar));
}

Tamgu* Tamgurawstring::MethodIslower(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    agnostring thestr(String());
    return booleantamgu[thestr.islower()];
}

Tamgu* Tamgurawstring::MethodIsupper(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    agnostring thestr(String());
    return booleantamgu[thestr.isupper()];
}

Tamgu* Tamgurawstring::MethodIsdigit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    agnostring thestr(String());
    return booleantamgu[thestr.isdigit()];
}

Tamgu* Tamgurawstring::MethodIsalpha(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    agnostring thestr(String());
    return booleantamgu[thestr.isalpha()];
}

Tamgu* Tamgurawstring::MethodIsconsonant(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    agnostring thestr(String());
    return booleantamgu[thestr.isconsonant()];
}

Tamgu* Tamgurawstring::MethodIsvowel(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    agnostring thestr(String());
    return booleantamgu[thestr.isvowel()];
}

Tamgu* Tamgurawstring::MethodDeaccentuate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    agnostring thestr(String());
    return globalTamgu->Providestring(thestr.deaccentuate());
}

Tamgu* Tamgurawstring::MethodIspunctuation(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    agnostring thestr(String());
    return booleantamgu[thestr.ispunctuation()];
}

Tamgu* Tamgurawstring::MethodLower(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    agnostring thestr(String());
    return globalTamgu->Providestring(thestr.tolower());
}

Tamgu* Tamgurawstring::MethodUpper(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    agnostring thestr(String());
    return globalTamgu->Providestring(thestr.toupper());
}

Tamgu* Tamgurawstring::MethodTrim(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    agnostring thestr(String());
    return globalTamgu->Providestring(thestr.trim());
}

Tamgu* Tamgurawstring::MethodTrimright(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    agnostring thestr(String());
    return globalTamgu->Providestring(thestr.trimright());
}

Tamgu* Tamgurawstring::MethodTrimleft(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    agnostring thestr(String());
    return globalTamgu->Providestring(thestr.trimleft());
}

Tamgu* Tamgurawstring::MethodBase(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
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
        return globalTamgu->Returnerror(message,idthread);
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

Tamgu* Tamgurawstring::MethodPadding(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* res = callfunc->Evaluate(0, contextualpattern, idthread);
    Tamgu* kval = callfunc->Evaluate(1, contextualpattern, idthread);
    bool direction = true;
    if (callfunc->Size() == 3)
        direction = callfunc->Evaluate(2, contextualpattern, idthread)->Boolean();


    string sval = kval->String();

    long sz = res->Integer() - stringsize;
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
    string v = String();

    if (direction)
        v += sres;
    else
        v = sres + v;

    if (v.size() >= buffersize)
        return globalTamgu->Returnerror("String overflow", idthread);

    strcpy_s((char*)value, buffersize, STR(v));
    stringsize = v.size();

    return this;
}

Tamgu* Tamgurawstring::MethodFill(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    
    Tamgu* res = callfunc->Evaluate(0, contextualpattern, idthread);
    Tamgu* kval = callfunc->Evaluate(1, contextualpattern, idthread);
    long sz = res->Integer();
    
    if (sz >= buffersize)
        sz = buffersize - 1;

    if (sz <= 0)
        return globalTamgu->Providestring("");

    string sval = kval->String();
    
    if (sval != "" && sval.size() == 1) {
        char c = ' ';
        c = sval[0];
        memset((char*)value, c, sz);
        value[sz] = 0;
        stringsize = sz;
    }
    else {
        string s;
        while (sz > 0) {
            s += sval;
            sz -= sval.size();
        }
        strcpy_s((char*)value, buffersize, STR(s));
    }
    return this;
}

Tamgu* Tamgurawstring::MethodTohtml(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
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

Tamgu* Tamgurawstring::MethodToxml(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
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

Tamgu* Tamgurawstring::MethodBytes(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (stringsize >= 1) {
        if (contextualpattern->isVectorContainer() || stringsize > 1) {
            Tamgu* kvect = SelectContainer(contextualpattern, idthread);
            if (kvect == NULL)
                kvect = globalTamgu->Provideivector();

            Locking _lock((TamguObject*)kvect);
            for (size_t i = 0; i < stringsize; i++)
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

Tamgu* Tamgurawstring::MethodGetstd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
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

Tamgu* Tamgurawstring::MethodGeterr(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
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

Tamgu* Tamgurawstring::MethodInsert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
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

Tamgu* Tamgurawstring::MethodClear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    Clear();
    return aTRUE;
}

Tamgu* Tamgurawstring::MethodExtract(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* pos = callfunc->Evaluate(0, contextualpattern, idthread);
    Tamgu* from = callfunc->Evaluate(1, contextualpattern, idthread);

    vector<string> v;
    long i;
    for (i = 2; i < callfunc->Size(); i++)
        v.push_back(callfunc->Evaluate(i, contextualpattern, idthread)->String());

    string init = String();
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

Tamgu* Tamgurawstring::MethodReverse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    agnostring s(String());
    return globalTamgu->Providestring(s.revert());
}

Tamgu* Tamgurawstring::MethodRemovefirst(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* knb = callfunc->Evaluate(0, contextualpattern, idthread);
    long id = knb->Integer();
    agnostring s(String());
    //On retire en tete
    long iv = s.sizec();
    if (id >= iv)
        return aEMPTYSTRING;

    s = s.right(iv - id);
    return globalTamgu->Providestring(s);
}

Tamgu* Tamgurawstring::MethodRemovelast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* knb = callfunc->Evaluate(0, contextualpattern, idthread);
    long id = knb->Integer();

    agnostring s(String());
    //On retire en tete
    long iv = s.sizec();
    
    if (id >= iv)
        return aEMPTYSTRING;

    return globalTamgu->Providestring(s.left(iv - id));
}

Tamgu* Tamgurawstring::MethodFormat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    char buffer[101];
    string sformat = String();
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

Tamgu* Tamgurawstring::MethodReplace(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
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
        
        string w = String();
        
        w = treg->replace(w, rep);
        return globalTamgu->Providestring(w);
#endif
    }

    string str = String();
    string reg = treg->String();
    if (reg == "")
        return globalTamgu->Providestring(str);

    string rep = callfunc->Evaluate(1, contextualpattern, idthread)->String();

    str = s_replacestring(str, reg, rep);
    return globalTamgu->Providestring(str);
}

Tamgu* Tamgurawstring::MethodEditdistance(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* e1 = this;
    Tamgu* e2 = callfunc->Evaluate(0, contextualpattern, idthread);
    if (e1->Type() == Tamguustring::idtype)
        return globalTamgu->Provideint(e1->EditDistance(e2));

    bool bytecall = false;
    if (callfunc->Size() == 3)
        bytecall = callfunc->Evaluate(1, contextualpattern, idthread)->Boolean();

    if (!bytecall) {
        Tamguustring e(e1->UString());
        return globalTamgu->Provideint(e.EditDistance(e2));
    }

    return globalTamgu->Provideint(e1->EditDistance(e2));
}

Tamgu* Tamgurawstring::MethodPop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
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

Tamgu* Tamgurawstring::MethodLast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter should be a vector
    string res(String());
    if (res.size() == 0)
        return aNULL;
    res = c_char_index(res, res.size() - 1);
    return globalTamgu->Providestring(res);
}

Tamgu* Tamgurawstring::MethodRead(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (globalTamgu->isthreading)
        return aFALSE;
    
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
    
    string val=file.read(-1);
    fclose(file.thefile);
    
    if (value == NULL) {
        stringsize = val.size();
        buffersize = stringsize + 1;
        value = new uchar[buffersize];
        strcpy_s((char*)value, buffersize, STR(val));
        return aTRUE;
    }
    
    stringsize = val.size();

    if (buffersize <= val.size()) {
        buffersize = stringsize + 1;
        delete[] value;
        value = new uchar[buffersize];
    }
    
    strcpy_s((char*)value, buffersize, STR(val));
    
    return aTRUE;
}
