/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguautomaton.cxx
 Date       : 2018/01/25
 Purpose    : Regular expressions recorded as automata
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#include "tamgu.h"
#include "tamguversion.h"
#include "tamguautomaton.h"
#include "tamguvector.h"
#include "tamguustring.h"
#include "tamguivector.h"

//We need to declare once again our local definitions.
basebin_hash<TamguregularexpressionMethod>  Tamguregularexpression::methods;
hmap<string, string> Tamguregularexpression::infomethods;
bin_hash<unsigned long> Tamguregularexpression::exported;

short Tamguregularexpression::idtype = 0;

//We only return the emoji head, when a head is present
TAMGUCHAR getechar(wstring& s, long& i);
long conversionpostochar(wstring& w, long spos);
void conversionpostochar(wstring& w, long& b, long& e);
bool CheckNeedConversion(string& w);
bool CheckNeedConversion(wstring& w);
void c_chartobyteposition(unsigned char* contenu, long sz, long& bcpos, long& ecpos);
void c_bytetocharposition(unsigned char* contenu, long& bbpos, long& ebpos);

//------------------------------------------------------------------------------------------------------------------
#define au_error -1
#define au_stop -2
#define au_ok 0
//------------------------------------------------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguregularexpression::AddMethod(TamguGlobal* global, string name, TamguregularexpressionMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

void Tamguregularexpression::Setidtype(TamguGlobal* global) {
    Tamguregularexpression::idtype = global->Getid("treg");
}

bool Tamguregularexpression::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();
    
        //Each new object has a specific name, which will help recognize it in the code that will exploit Tamguregularexpression...
    Tamguregularexpression::idtype = global->Getid("treg");
    
        //You declare your methods here:
        // Argument 1 is a call to global, which will record your method into Tamgu (탐구)
        // Argument 2 is the name of your command
        // Argument 3 links your command with its implementation into the object class
        // Argument 4 is a combination of P_... which defines how many parameters this function can accept.
        //Argument 5 is a description of this method.
    
    Tamguregularexpression::AddMethod(global, "_initial", &Tamguregularexpression::MethodCompile, P_ONE, "_initial(string rgx): compile a tamgu regular expression.");
    Tamguregularexpression::AddMethod(global, "compile", &Tamguregularexpression::MethodCompile, P_ONE, "compile(string rgx): compile a tamgu regular expression.");
    Tamguregularexpression::AddMethod(global, "match", &Tamguregularexpression::MethodMatch, P_ONE, "match(string str): check if the string matches the tamgu regular expression.");
    
        //We need this code, in order to create new instances of our Tamguregularexpression object... DO NOT ALTER
    global->newInstance[Tamguregularexpression::idtype] = new Tamguregularexpression(global);
    global->RecordMethods(Tamguregularexpression::idtype,Tamguregularexpression::exported);
    
    return true;
}

Tamgu* Tamguregularexpression::same(Tamgu* a) {
    if (automate==NULL)
        return aFALSE;
    
    wstring w=a->UString();
    return booleantamgu[automate->match(w)];
}


Tamgu* Tamguregularexpression::different(Tamgu* a) {
    if (automate==NULL)
        return aTRUE;

    wstring w=a->UString();
    return booleantamgu[!automate->match(w)];
}


Tamgu* Tamguregularexpression::MethodMatch(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (automate==NULL)
        return aFALSE;
    
    wstring w = callfunc->Evaluate(0, contextualpattern, idthread)->UString();
    return booleantamgu[automate->match(w)];
}

Tamgu* Tamguregularexpression::Put(Tamgu* idx, Tamgu* rgx, short idthread) {
    Au_automate* a = getautomate(rgx);
    if (a == NULL)
        return globalTamgu->Returnerror("Wrong tamgu regular expression");

    automate = a;
    reg = rgx->UString();
    return aTRUE;
}

Tamgu* Tamguregularexpression::MethodCompile(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* rgx = callfunc->Evaluate(0, contextualpattern, idthread);
    Au_automate* a = getautomate(rgx);
    if (a == NULL)
        return globalTamgu->Returnerror("Wrong tamgu regular expression");

    automate = a;
    reg = rgx->UString();

    return aTRUE;
}

Tamgu* Tamguregularexpression::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    if (automate==NULL) {
        if (context->isNumber() && !context->isContainer())
            return aMINUSONE;
        return aNULL;
    }

    wstring val;
    long i = 0;
    
    if (a->isContainer()) {
        if (a->Type() == a_list) {
            Tamgu* v = NULL;
            bool nmb=false;
            
            if (context->isVectorContainer()) {
                v = Selectavector(context);
                nmb=v->isNumber();
            }
            
            TamguIteration* itr = a->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
                val = itr->Valueustring();
                if (automate->match(val)) {
                    if (v != NULL) {
                        if (nmb)
                            v->storevalue(i);
                        else
                            v->storevalue(val);
                    }
                    else {
                        itr->Release();
                        if (context->isNumber())
                            return globalTamgu->Provideint(i);
                        return aTRUE;
                    }
                }
                ++i;
            }
            
            itr->Release();
            if (v != NULL)
                return v;
            
            if (context->isNumber())
                return aMINUSONE;
            
            return aFALSE;
        }
        
        if (a->isVectorContainer() || a->Type() == a_table) {
            Tamgu* v = NULL;
            bool nmb=false;
            if (context->isVectorContainer()) {
                v = Selectavector(context);
                nmb=v->isNumber();
            }
            
            for (i = 0; i < a->Size(); i++) {
                val = a->getustring(i);
                if (automate->match(val)) {
                    if (v != NULL) {
                        if (nmb)
                            v->storevalue(i);
                        else
                            v->storevalue(val);
                    }
                    else {
                        if (context->isNumber())
                            return globalTamgu->Provideint(i);
                        return aTRUE;
                    }
                }
            }
            if (v != NULL)
                return v;
            
            if (context->isNumber())
                return aMINUSONE;
            
            return aFALSE;
        }
        
        Tamgu* v = NULL;
        if (context->isVectorContainer())
            v = Selectavector(context);
        
        Tamgu* tval;
        TamguIteration* itr = a->Newiteration(false);
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            tval = itr->IteratorKey();
            val = tval->UString();
            if (automate->match(val)) {
                if (v != NULL)
                    v->Push(tval);
                else {
                    itr->Release();
                    if (context->isNumber())
                        return globalTamgu->Provideint(i);
        
                    return aTRUE;
                }
            }
            ++i;
        }
        
        itr->Release();
        if (v != NULL)
            return v;
        
        if (context->isNumber())
            return aMINUSONE;
        
        return aFALSE;
    }
    
    val = a->UString();
    
    if (context->isVectorContainer()) {
        Tamgu* v = Selectavector(context);
        vector<long> pos;
        long i;
        if (v->isNumber()) {
            automate->searchall(val, pos);
            for (i=0;i<pos.size();i++)
                v->storevalue(pos[i]);
            return v;
        }
        
        //In this case, we do not want any position conversion
        automate->bytesearchall(val, pos);
        for (i=0; i < pos.size(); i+=2)
            v->storevalue(val.substr(pos[i], pos[i+1]-pos[i]));
        return v;
    }
    
    if (context->isString()) {
        long e;
        //No position conversion
        if (!automate->bytesearch(val, i, e))
            return aEMPTYSTRING;
        return globalTamgu->Provideustring(val.substr(i, e-i));
    }
    
    i = automate->find(val);
    if (context->isNumber())
        return globalTamgu->Provideint(i);

    //The default value is a Boolean...
    if (i != au_error)
        return aTRUE;
    return aFALSE;
}

void Tamguregularexpression::searchall(string& txt, vector<long>& v) {
    if (automate==NULL)
        return;
    
    automate->searchall(txt, v);
}

void Tamguregularexpression::searchall(wstring& txt, vector<long>& v) {
    if (automate==NULL)
        return;
    automate->searchall(txt, v);
}

void Tamguregularexpression::searchall(string& txt, vector<long>& v, long init) {
    if (automate==NULL)
        return;
    automate->searchall(txt, v, init);
}

void Tamguregularexpression::searchall(wstring& txt, vector<long>& v, long init) {
    if (automate==NULL)
        return;
    automate->searchall(txt, v, init);
}


bool Tamguregularexpression::search(wstring& w, long& b, long& e, long init) {
    if (automate==NULL)
        return false;
    return automate->search(w,b,e,init);
}

bool Tamguregularexpression::search(string& w, long& b, long& e, long init) {
    if (automate==NULL)
        return false;
    return automate->search(w,b,e,init);
}

bool Tamguregularexpression::searchlast(wstring& w, long& b, long& e, long init) {
    if (automate==NULL)
        return false;
    return automate->searchlast(w,b,e,init);
}

bool Tamguregularexpression::searchlast(string& w, long& b, long& e, long init) {
    if (automate==NULL)
        return false;
    return automate->searchlast(w,b,e,init);
}


#ifdef Tamgu_REGEX
//We need to declare once again our local definitions.
basebin_hash<TamguposixregularexpressionMethod>  Tamguposixregularexpression::methods;
hmap<string, string> Tamguposixregularexpression::infomethods;
bin_hash<unsigned long> Tamguposixregularexpression::exported;

short Tamguposixregularexpression::idtype = 0;

//------------------------------------------------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguposixregularexpression::AddMethod(TamguGlobal* global, string name, TamguposixregularexpressionMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Tamguposixregularexpression::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();
    
    //Each new object has a specific name, which will help recognize it in the code that will exploit Tamguposixregularexpression...
    Tamguposixregularexpression::idtype = global->Getid("preg");
    
    //You declare your methods here:
    // Argument 1 is a call to global, which will record your method into Tamgu (탐구)
    // Argument 2 is the name of your command
    // Argument 3 links your command with its implementation into the object class
    // Argument 4 is a combination of P_... which defines how many parameters this function can accept.
    //Argument 5 is a description of this method.
    
    Tamguposixregularexpression::AddMethod(global, "_initial", &Tamguposixregularexpression::MethodCompile, P_ONE, "_initial(string rgx): compile a tamgu regular expression.");
    Tamguposixregularexpression::AddMethod(global, "compile", &Tamguposixregularexpression::MethodCompile, P_ONE, "compile(string rgx): compile a tamgu regular expression.");
    Tamguposixregularexpression::AddMethod(global, "match", &Tamguposixregularexpression::MethodMatch, P_ONE, "match(string str): check if the string matches the tamgu regular expression.");
    
    //We need this code, in order to create new instances of our Tamguposixregularexpression object... DO NOT ALTER
    global->newInstance[Tamguposixregularexpression::idtype] = new Tamguposixregularexpression(global);
    global->RecordMethods(Tamguposixregularexpression::idtype,Tamguposixregularexpression::exported);
    
    return true;
}

wregex* wgetposixregex(Tamgu* p);
regex* getposixregex(Tamgu* p);

void Tamguposixregularexpression::searchall(string& val, vector<long>& v) {
    if (pattern == NULL)
        return;

    const sregex_iterator end;
    for (sregex_iterator i(val.begin(), val.end(), *pattern); i != end; ++i) {
        v.push_back(i->position());
        v.push_back(i->position() + i->length());
    }
}

void Tamguposixregularexpression::searchall(wstring& val, vector<long>& v) {
    if (pattern == NULL)
        return;
    
    const wsregex_iterator end;
    for (wsregex_iterator i(val.begin(), val.end(), *wpattern); i != end; ++i) {
        v.push_back(i->position());
        v.push_back(i->position() + i->length());
    }
}

void Tamguposixregularexpression::searchall(string& val, vector<long>& v, long init) {
    if (pattern == NULL)
        return;

    const sregex_iterator end;
    if (init != 0) {
        if (init >= val.size())
            return;
        
        string w = val.substr(init,val.size()-init);
        for (sregex_iterator i(w.begin(), w.end(), *pattern); i != end; ++i) {
            v.push_back(i->position());
        }
    }
    else {
        for (sregex_iterator i(val.begin(), val.end(), *pattern); i != end; ++i) {
            v.push_back(i->position());
        }
    }
}

void Tamguposixregularexpression::searchall(wstring& val, vector<long>& v, long init) {
    if (pattern == NULL)
        return;
    
    const wsregex_iterator end;
    if (init != 0) {
        if (init>=val.size())
            return;
        wstring w = val.substr(init,val.size()-init);

        for (wsregex_iterator i(w.begin(), w.end(), *wpattern); i != end; ++i) {
            v.push_back(i->position());
        }
    }
    else {
        for (wsregex_iterator i(val.begin(), val.end(), *wpattern); i != end; ++i) {
            v.push_back(i->position());
        }
    }
}

bool Tamguposixregularexpression::search(wstring& w, long& b, long& e, long init) {
    if (pattern == NULL)
        return false;

    wsmatch result;
    b = au_error;

    if (init) {
        if (init >= w.size())
            return false;
    
        wstring val=w.substr(init,w.size()-init);

        if (regex_search(val, result, *wpattern)) {
            b = result.position();
            e = b + result.length();
            return true;
        }
        return false;
    }

    if (regex_search(w, result, *wpattern)) {
        b = result.position();
        e = b + result.length();
        return true;
    }
    
    return false;
}

bool Tamguposixregularexpression::search(string& w, long& b, long& e, long init) {
    if (pattern == NULL)
        return false;
    
    smatch result;
    b = au_error;
    
    if (init) {
        if (init >= w.size())
            return false;
        
        string val=w.substr(init,w.size()-init);
        if (regex_search(val, result, *pattern)) {
            b = result.position();
            e = b + result.length();
            return true;
        }
        return false;
    }
    
    if (regex_search(w, result, *pattern)) {
        b = result.position();
        e = b + result.length();
        return true;
    }
    
    return false;
}

bool Tamguposixregularexpression::searchlast(wstring& w, long& b, long& e, long init) {
    if (pattern == NULL)
        return false;

    const wsregex_iterator end;

    wsmatch result;
    b = au_error;
    
    
    if (init) {
        if (init >= w.size())
            return false;
        
        wstring val=w.substr(init,w.size()-init);
        
        for (wsregex_iterator i(val.begin(), val.end(), *wpattern); i != end; ++i) {
            b = i->position();
            e = b + i->length();
        }

        if (b != au_error)
            return true;

        return false;
    }
    
    for (wsregex_iterator i(w.begin(), w.end(), *wpattern); i != end; ++i) {
        b = i->position();
        e = b + i->length();
    }

    if (b != au_error)
        return true;

    return false;
}

bool Tamguposixregularexpression::searchlast(string& w, long& b, long& e, long init) {
    if (pattern == NULL)
        return false;
    
    const sregex_iterator end;
    
    smatch result;
    b = au_error;
    
    if (init) {
        if (init >= w.size())
            return false;
        
        string val=w.substr(init,w.size()-init);
        
        for (sregex_iterator i(val.begin(), val.end(), *pattern); i != end; ++i) {
            b = i->position();
            e = b + i->length();
        }
        
        if (b != au_error)
            return true;
        
        return false;
    }
    

    for (sregex_iterator i(w.begin(), w.end(), *pattern); i != end; ++i) {
        b = i->position();
        e = b + i->length();
    }
    
    if (b != au_error)
        return true;
    
    return false;
}

Tamgu* Tamguposixregularexpression::splitting(Tamgu* vect, wstring& str) {
    if (pattern == NULL)
        return vect;
    
    match_results<wstring::const_iterator> result;
    if (regex_match(str, result, *wpattern) == true) {
        for (long i = 1; i < result.size(); i++)
            vect->storevalue(result[i]);
    }
    return vect;
}

Tamgu* Tamguposixregularexpression::splitting(Tamgu* vect, string& str) {
    if (pattern == NULL)
        return vect;
    
    match_results<string::const_iterator> result;
    if (regex_match(str, result, *pattern) == true) {
        for (long i = 1; i < result.size(); i++)
            vect->storevalue(result[i]);
    }
    return vect;
}

Tamgu* Tamguposixregularexpression::same(Tamgu* a) {
    if (pattern == NULL)
        return aFALSE;

    wstring w=a->UString();
    return booleantamgu[regex_match(w, *wpattern)];
}


Tamgu* Tamguposixregularexpression::different(Tamgu* a) {
    if (pattern == NULL)
        return aTRUE;

    wstring w=a->UString();
    return booleantamgu[!regex_match(w, *wpattern)];
}


Tamgu* Tamguposixregularexpression::MethodMatch(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (pattern == NULL)
        return aFALSE;

    wstring w = callfunc->Evaluate(0, contextualpattern, idthread)->UString();
    return booleantamgu[regex_match(w,*wpattern)];
}


Tamgu* Tamguposixregularexpression::Put(Tamgu* idx, Tamgu* rgx, short idthread) {
    wregex* a = wgetposixregex(rgx);
    if (a == NULL)
        return globalTamgu->Returnerror("Wrong tamgu regular expression");
    
    wpattern = a;
    pattern = getposixregex(rgx);
    reg = rgx->UString();

    return aTRUE;
}

Tamgu* Tamguposixregularexpression::MethodCompile(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* rgx = callfunc->Evaluate(0, contextualpattern, idthread);
    
    wregex* a = wgetposixregex(rgx);
    
    if (a == NULL)
        return globalTamgu->Returnerror("Wrong tamgu regular expression");
    
    wpattern = a;
    pattern = getposixregex(rgx);

    reg = rgx->UString();

    return aTRUE;
}

Tamgu* Tamguposixregularexpression::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    if (pattern == NULL) {
        if (context->isNumber() && !context->isContainer())
            return aMINUSONE;
        return aNULL;
    }
    
    long i = 0;
    
    if (a->isContainer()) {
        wstring val;
        if (a->Type() == a_list) {
            Tamgu* v = NULL;
            bool nmb=false;
            
            if (context->isVectorContainer()) {
                v = Selectavector(context);
                nmb=v->isNumber();
            }
            
            TamguIteration* itr = a->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
                val = itr->Valueustring();
                if (regex_match(val, *wpattern)) {
                    if (v != NULL) {
                        if (nmb)
                            v->storevalue(i);
                        else
                            v->storevalue(val);
                    }
                    else {
                        itr->Release();
                        if (context->isNumber())
                            return globalTamgu->Provideint(i);
                        return aTRUE;
                    }
                }
                ++i;
            }
            
            itr->Release();
            if (v != NULL)
                return v;
            
            if (context->isNumber())
                return aMINUSONE;
            return aFALSE;
        }
        
        if (a->isVectorContainer() || a->Type() == a_table) {
            Tamgu* v = NULL;
            bool nmb=false;
            if (context->isVectorContainer()) {
                v = Selectavector(context);
                nmb=v->isNumber();
            }
            
            for (i = 0; i < a->Size(); i++) {
                val = a->getustring(i);
                if (regex_match(val, *wpattern)) {
                    if (v != NULL) {
                        if (nmb)
                            v->storevalue(i);
                        else
                            v->storevalue(val);
                    }
                    else {
                        if (context->isNumber())
                            return globalTamgu->Provideint(i);
                        return aTRUE;
                    }
                }
            }
            if (v != NULL)
                return v;
            
            if (context->isNumber())
                return aMINUSONE;
            return aFALSE;
        }
        
        Tamgu* v = NULL;
        if (context->isVectorContainer())
            v = Selectavector(context);
        
        Tamgu* tval;
        TamguIteration* itr = a->Newiteration(false);
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            tval = itr->IteratorKey();
            val = tval->UString();
            if (regex_match(val, *wpattern)) {
                if (v != NULL)
                    v->Push(tval);
                else {
                    itr->Release();
                    if (context->isNumber())
                        return globalTamgu->Provideint(i);
                    return aTRUE;
                }
            }
            ++i;
        }
        
        itr->Release();
        if (v != NULL)
            return v;
        
        if (context->isNumber())
            return aMINUSONE;
        return aFALSE;
    }

    
    if (context->isVectorContainer()) {
        Tamgu* v = Selectavector(context);
        
        if (v->isNumber()) {
            wstring val = a->UString();
            bool conv = CheckNeedConversion(val);
            long d, f;
            const wsregex_iterator end;
            for (wsregex_iterator i(val.begin(), val.end(), *wpattern); i != end; ++i) {
                d = i->position();
                f = i->position() + i->length();
                if (conv)
                    conversionpostochar(val,d,f);
                v->storevalue(d);
                v->storevalue(f);
            }
            return v;
        }
        

        if (a->Type() == a_string) {
            string val = a->String();
            const sregex_token_iterator end;
            for (sregex_token_iterator i(val.begin(), val.end(), *pattern); i != end; ++i)
                v->storevalue(*i);
            return v;
        }
        
        wstring val = a->UString();
        const wsregex_token_iterator end;
        for (wsregex_token_iterator i(val.begin(), val.end(), *wpattern); i != end; ++i)
            v->storevalue(*i);
        
        return v;
    }
    

    if (context->isString()) {
        if (a->Type() == a_string) {
            smatch result;
            string val = a->String();
            if (regex_search(val, result, *pattern) == true)
                return globalTamgu->Providestring(result[0]);
        }
        else {
            wsmatch result;
            wstring val = a->UString();
            if (regex_search(val, result, *wpattern) == true)
                return globalTamgu->Provideustring(result[0]);
        }
        return aEMPTYSTRING;
    }

    if (context->isNumber()) {
        wsmatch result;
        wstring val = a->UString();
        if (regex_search(val, result, *wpattern) == true) {
            if (CheckNeedConversion(val))
                return globalTamgu->Provideint(conversionpostochar(val,result.position()));
            return globalTamgu->Provideint(result.position());
        }
        return aMINUSONE;
    }
    
    if (a->Type() == a_string) {
        smatch result;
        string val = a->String();
        return booleantamgu[regex_search(val, result, *pattern)];
    }
    
    wsmatch result;
    wstring val = a->UString();
    return booleantamgu[regex_search(val, result, *wpattern)];
}

#endif
//------------------------------------------------------------------------------------------------------------
static basebinn_hash<wstring> patterns;

bool store_pattern(uchar x, wstring& pattern) {
    static char alreadyused[] ="CHSacdenprsx";
    if (strchr(alreadyused,(char)x))
        return false;
    
    Locking _lock(globalTamgu->_parselock);
    
    if (pattern == L"") {
        patterns.erase(x);
        return true;
    }
    
    //There could macros within macros...
    basebinn_hash<wstring>::iterator it;
    wstring mac=L"%X";
    for (it=patterns.begin();it != patterns.end(); it++) {
        mac[1]=it->first;
        pattern = s_replacestring(pattern,mac,it->second);
    }
    
    patterns[x] = pattern;
    return true;
}

void clear_pattern() {
    patterns.clear();
}
//----------------------------------------------------------------------------------------
static hmap<wstring,Au_automate*> automates;

#ifdef Tamgu_REGEX
static hmap<wstring,wregex*> wposixregex;
static hmap<string,regex*> posixregex;
#endif

void clear_automates() {
    for (auto& a : automates)
        delete a.second;
    automates.clear();

#ifdef Tamgu_REGEX
    for (auto& w: wposixregex)
        delete w.second;
    
    for (auto& p: posixregex)
        delete p.second;
    
    posixregex.clear();
    wposixregex.clear();
#endif
}

Au_automate* getautomate(Tamgu* t) {
    if (t->Type() == a_treg)
        return ((Tamguregularexpression*)t)->automate;
    

    wstring w = t->UString();
    Locking _lock(globalTamgu->_parselock);
    
    if (automates.find(w) != automates.end())
        return automates[w];
    
    wstring pattern(w);
    Au_automate* au = new Au_automate;
    if (!au->compile(pattern)) {
        delete au;
        return NULL;
    }
    
    automates[w] = au;
    return au;
}

Au_automate* getautomate(string& s) {
    wstring w;
    
    s_utf8_to_unicode(w, USTR(s), s.size());
    
    Locking _lock(globalTamgu->_parselock);
    if (automates.find(w) != automates.end())
        return automates[w];
    
    wstring pattern(w);
    Au_automate* au = new Au_automate;
    if (!au->compile(pattern)) {
        delete au;
        return NULL;
    }
    
    automates[w] = au;
    return au;
}

#ifdef Tamgu_REGEX
wregex* wgetposixregex(Tamgu* p) {
    if (p->Type() == a_preg)
        return ((Tamguposixregularexpression*)p)->wpattern;

    wstring w = p->UString();

    Locking _lock(globalTamgu->_parselock);
    if (wposixregex.find(w) != wposixregex.end())
        return wposixregex[w];
    
    string s = p->String();
    wstring pattern(w);
    
    regex* a = NULL;
    wregex* au = NULL;
    try {
        au = new wregex(w);
        a = new regex(s);
        wposixregex[w] = au;
        posixregex[s] = a;
    }
    catch (...) {
        if (au != NULL)
            delete au;
        if (a != NULL)
            delete a;
        return NULL;
    }

    return au;
}


wregex* wgetposixregex(string& s) {
    wstring w;
    
    s_utf8_to_unicode(w, USTR(s), s.size());
    
    Locking _lock(globalTamgu->_parselock);
    if (wposixregex.find(w) != wposixregex.end())
        return wposixregex[w];
    
    wstring pattern(w);

    regex* a = NULL;
    wregex* au = NULL;
    try {
        au = new wregex(w);
        a = new regex(s);
        wposixregex[w] = au;
        posixregex[s] = a;
    }
    catch (...) {
        if (au != NULL)
            delete au;
        if (a != NULL)
            delete a;
        return NULL;
    }
    
    return au;
}

regex* getposixregex(Tamgu* p) {
    if (p->Type() == a_preg)
        return ((Tamguposixregularexpression*)p)->pattern;
    
    string s = p->String();

    Locking _lock(globalTamgu->_parselock);
    if (posixregex.find(s) != posixregex.end())
        return posixregex[s];

    wstring w = p->UString();

    wstring pattern(w);
    
    regex* a = NULL;
    wregex* au = NULL;
    try {
        au = new wregex(w);
        a = new regex(s);
        wposixregex[w] = au;
        posixregex[s] = a;
    }
    catch (...) {
        if (au != NULL)
            delete au;
        if (a != NULL)
            delete a;
        return NULL;
    }
    
    return a;
}


regex* getposixregex(string& s) {
    
    Locking _lock(globalTamgu->_parselock);
    if (posixregex.find(s) != posixregex.end())
        return posixregex[s];

    wstring w;
    
    s_utf8_to_unicode(w, USTR(s), s.size());

    wstring pattern(w);
    
    regex* a = NULL;
    wregex* au = NULL;
    try {
        au = new wregex(w);
        a = new regex(s);
        wposixregex[w] = au;
        posixregex[s] = a;
    }
    catch (...) {
        if (au != NULL)
            delete au;
        if (a != NULL)
            delete a;
        return NULL;
    }
    
    return a;
}
#endif

//---------------------------------------AUTOMATON------------------------------------------------------------
static bool tokenize(wstring& rg, vector<wstring>& stack, vector<aut_actions>& vtypes) {
    wstring sub;
    long sz = rg.size();
    uchar type=aut_reg;
    long inbracket = 0;
    
    for (long i = 0; i < sz; i++) {
        switch (rg[i]) {
            case 10:
            case 13:
                continue;
            case '\\': //escape character
                if ((i + 1) < sz) {
                    sub = rg[++i];
                    //we accept: \ddd, exactly three digits
                    if (c_is_digit(rg[i]) && (i+2)<sz && c_is_digit(rg[i+1]) && c_is_digit(rg[i+2])) {
                        sub+=rg[i+1];
                        sub+=rg[i+2];
                        i+=2;
                        sub = (wchar_t)convertinteger(sub);
                    }
                    else //we also accept four hexa: \xFFFF
                        if (rg[i] == 'x' && (i+4)<sz && c_is_hexa(rg[i+1]) && c_is_hexa(rg[i+2]) && c_is_hexa(rg[i+3]) && c_is_hexa(rg[i+4])) {
                            sub=L"0x";
                            sub+=rg[i+1];
                            sub+=rg[i+2];
                            sub+=rg[i+3];
                            sub+=rg[i+4];
                            i+=4;
                            sub = (wchar_t)convertinteger(sub);
                        }
                    type = aut_reg;
                }
                break;
            case '%':
                sub = L"%";
                if ((i + 1) < sz) {
                    type = aut_meta;
                    i++;
                    sub += rg[i];
                }
                break;
            case '~': // negation of the next character
                sub = L'~';
                type = aut_negation;
                break;
            case '?':
                sub = L'?';
                type = aut_any;
                break;
            case '(':
                sub = L'(';
                type = aut_opar;
                break;
            case ')':
                stack.push_back(L")");
                vtypes.push_back(aut_cpar);
                continue; //we cannot have: ..)+ or ..)*
            case '[':
                sub = L'[';
                type = aut_obrk;
                inbracket++;
                break;
            case ']':
                sub = L']';
                type = aut_cbrk;
                inbracket--;
                break;
            case '{': //curly bracket
                sub = L'{';
                type = aut_ocrl_brk;
                inbracket++;
                break;
            case '}':
                sub = L'}';
                type = aut_ccrl_brk;
                inbracket--;
                break;
            case '-':
                if (inbracket && i < sz-1 && vtypes.back() == aut_reg) {
                    //{a-z} or [a-z]
                    //in that case, we build the character list between the current character and the next one...
                    sub = stack.back();
                    wstring nxt;
                    nxt = rg[++i];
                    ++sub[0];
                    while (sub[0] <= nxt[0]) {
                        stack.push_back(sub);
                        vtypes.push_back(aut_reg);
                        ++sub[0];
                    }
                    continue;
                }
            default:
                sub = rg[i];
                type = aut_reg;
        }
        
        if ((i + 1) < sz) {
            if (rg[i + 1] == L'+') {
                i++;
                type += 1;
                sub += rg[i];
            }
            else {
                if (rg[i + 1] == L'*') {
                    i++;
                    type += 2;
                    sub += rg[i];
                }
            }
        }
        
        stack.push_back(sub);
        vtypes.push_back((aut_actions)type);
    }
    
    if (inbracket)
        return false;
    
    return true;
}
//----------------------------------------------------------------
//an epsilon, which points to a final state with no arcs attached
bool Au_arc::checkfinalepsilon() {
    if (action->Type() == an_epsilon && state->isend() && !state->arcs.last)
        return true;
    return false;
}

void Au_state::removeepsilon() {
    if (mark)
        return;
    
    mark=true;
    vector<long> removals;
    long i;
    for (i=0;i<arcs.size();i++) {
        if (arcs[i]->checkfinalepsilon()) {
            //We can remove it...
            removals.push_back(i);
        }
        else
            arcs[i]->state->removeepsilon();
    }
    
    for (i = removals.size()-1; i>=0; i--) {
        status|=an_end;
        arcs.erase(removals[i]);
    }
}
//----------------------------------------------------------------

void Au_state::addrule(Au_arc* r) {
    if (mark)
        return;
    
    mark=true;
    //This is a terminal node...
    if (isend())
        arcs.push_back(r);
    
    for (long i=0;i<arcs.size();i++)
        arcs[i]->state->addrule(r);
    
    return;
}

//----------------------------------------------------------------
//A bit of explanation... If the string is UTF16, then getechar can modify "i", when it returns
//This is why we need to keep the current position before calling the function
//In the case of 32 bits wstring, this variable is useless... i won't budge after a call to getachar...
#ifdef WSTRING_IS_UTF16
#define _d_current_i long _current_i = i;
#else
#define _d_current_i
#define _current_i i
#endif

bool Au_state::match(wstring& w, long i) {
    if ((status&an_error) == an_error)
        return false;

    if (i==w.size()) {
        if (isend())
            return true;
        return false;
    }

	_d_current_i
    TAMGUCHAR c = getechar(w, i);

    for (long j=0;j<arcs.last;j++) {
        switch(arcs[j]->action->compare(c)) {
            case 0:
                break;
            case 1:
                if (arcs[j]->state->match(w,i+1))
                    return true;
                break;
            case 2:
                if (arcs[j]->state->match(w,_current_i))
                    return true;
        }
    }
    
    return false;
}

bool Au_automaton::match(string& v) {
    wstring w;
    s_utf8_to_unicode(w, USTR(v), v.size());
    return first->match(w,0);
}

bool Au_automaton::match(wstring& w) {
    return first->match(w,0);
}

//----------------------------------------------------------------

void Au_state::storerulearcs(hmap<long,bool>& rules) {
    if (mark)
        return;
    
    if (isrule())
        rules[idx()]=true;
    
    mark=true;
    for (long j=0;j<arcs.last;j++)
        arcs[j]->state->storerulearcs(rules);
    mark=false;
}

char Au_state::get(wstring& w, long i, hmap<long,bool>& rules) {
    if ((status & an_error) == an_error)
        return au_stop;
    
    if (i==w.size()) {
        if (isend()) {
            storerulearcs(rules);
            return au_ok;
        }
        return au_error;
    }

    char l = au_error;
    long j;
    _d_current_i

    TAMGUCHAR c = getechar(w, i);

    for (j=0;j<arcs.last;j++) {
        switch(arcs[j]->action->compare(c)) {
            case 0:
                l = au_error;
                continue;
            case 1:
                l = arcs[j]->state->get(w,i+1, rules);
                break;
            case 2:
                l = arcs[j]->state->get(w, _current_i, rules);
        }
        if (l != au_error) {
            if (l == au_stop)
                break;
            return l;
        }
    }

    return au_error;
}

bool Au_automaton::get(wstring& w, hmap<long,bool>& rules) {
    if (first == NULL)
        return false;
    rules.clear();
    if (first->get(w,0,rules) == au_ok)
        return true;
    return false;
}
//----------------------------------------------------------------

long Au_state::loop(wstring& w, long i) {
    if ((status & an_error) == an_error)
        return au_stop;
    
    if (i==w.size()) {
        if (isend())
            return i;
        return au_error;
    }

    if (status & an_beginning) {
        if (i)
            return au_error;
    }
    
    long l = au_error;
    long j;
	_d_current_i

    TAMGUCHAR c = getechar(w, i);

    for (j=0;j<arcs.last;j++) {
        switch(arcs[j]->action->compare(c)) {
            case 0:
                l = au_error;
                continue;
            case 1:
                l = arcs[j]->state->loop(w,i+1);
                break;
            case 2:
				l = arcs[j]->state->loop(w, _current_i);
        }
        if (l != au_error) {
            if (l == au_stop)
                break;
            return l;
        }
    }
    
    if (isend()) {
        if (status & an_ending) {
            if (i != w.size())
                return au_error;
        }
        return i;
    }
    
    return au_error;
}

long Au_automaton::find(wstring& w) {
    long sz = w.size();
    for (long d=0;d<sz;d++) {
        if (first->loop(w,d) != au_error) {
            if (CheckNeedConversion(w))
                return conversionpostochar(w,d);
            return d;
        }
    }
    return au_error;
}

long Au_automaton::find(wstring& w, long i) {
    long sz = w.size();
    for (long d = i ; d < sz; d++) {
        if (first->loop(w,d) != au_error) {
            if (CheckNeedConversion(w))
                return conversionpostochar(w,d);
            return d;
        }
    }
    return au_error;
}

bool Au_automaton::search(wstring& w) {
    long sz = w.size();
    for (long d=0;d<sz;d++) {
        if (first->loop(w,d) != au_error)
            return true;
    }
    return false;
}

bool Au_automaton::search(wstring& w, long& b, long& e, long init) {
    long sz = w.size();
    for (b=init;b<sz;b++) {
        e=first->loop(w,b);
        if (e != au_error) {
            if (CheckNeedConversion(w)) {
                conversionpostochar(w,b,e);
            }
            return true;
        }
    }
    b=au_error;
    return false;
}

bool Au_automaton::search(string& s, long& b, long& e, long init) {
    wstring w;
    long sz = s.size();
    if (CheckNeedConversion(s)) {
        s_utf8_to_unicode(w, USTR(s), sz);
        if (init)
            init = c_bytetocharposition(USTR(s), init);
        if (search(w, b, e, init)) {
            c_chartobyteposition(USTR(s),sz, b, e);
            return true;
        }
    }
    else {
        for (long i = 0; i < sz; i++)
            w +=  (wchar_t)(uchar)s[i] ;
        if (search(w, b, e, init))
            return true;
    }
    return false;
}

bool Au_automaton::searchc(string& s, long& b, long& e, long& fc, long init) {
    wstring w;
    long sz = s.size();
    if (CheckNeedConversion(s)) {
        s_utf8_to_unicode(w, USTR(s), sz);
        if (init)
            init = c_bytetocharposition(USTR(s), init);
        if (search(w, b, e, init)) {
            fc=b;
            c_chartobyteposition(USTR(s),sz, b, e);
            return true;
        }
    }
    else {
        for (long i = 0; i < sz; i++)
            w +=  (wchar_t)(uchar)s[i] ;
        if (search(w, b, e, init)) {
            fc=b;
            return true;
        }
    }
    return false;
}

//The string is not a UTF8 string, no need for char to byte position conversion, nor to complex string conversion
bool Au_automaton::searchraw(string& s, long& b, long& e, long init) {
    wstring w;
    long sz = s.size();
    w.reserve(sz);
    for (long i =0; i< sz; i++)
        w +=  (wchar_t)(uchar)s[i] ;
    
    if (search(w, b, e, init))
        return true;
    return false;
}

bool Au_automaton::searchlast(wstring& w, long& b, long& e, long init) {
    b=au_error;
    long f;
    long sz = w.size();
    for (long d=init;d<sz;d++) {
        f=first->loop(w,d);
        if (f!=au_error) {
            b=d;
            e=f;
            d=f-1;
        }
    }
    
    if (b!=au_error) {
        if (CheckNeedConversion(w)) {
            conversionpostochar(w,b,e);
        }
        return true;
    }
    return false;
}

bool Au_automaton::searchlast(string& s, long& b, long& e, long init) {
    wstring w;
    long sz = s.size();
    
    if (CheckNeedConversion(s)) {
        s_utf8_to_unicode(w, USTR(s), sz);
        if (init)
            init = c_bytetocharposition(USTR(s), init);
        
        if (searchlast(w, b, e, init)) {
            c_chartobyteposition(USTR(s),sz, b, e);
            return true;
        }
    }
    else {
        for (long i = 0; i < sz; i++)
            w +=  (wchar_t)(uchar)s[i] ;
        if (searchlast(w, b, e, init))
            return true;
    }
    
    return false;
}

bool Au_automaton::searchlastc(string& s, long& b, long& e, long& fc, long init) {
    wstring w;
    long sz = s.size();
    
    if (CheckNeedConversion(s)) {
        s_utf8_to_unicode(w, USTR(s), sz);
        if (init)
            init = c_bytetocharposition(USTR(s), init);
        
        if (searchlast(w, b, e, init)) {
            fc = b;
            c_chartobyteposition(USTR(s),sz, b, e);
            return true;
        }
    }
    else {
        for (long i = 0; i < sz; i++)
            w +=  (wchar_t)(uchar)s[i] ;
        if (searchlast(w, b, e, init)) {
            fc=b;
            return true;
        }
    }
    return false;
}

//The string is not a UTF8 string, no need for char to byte position conversion, nor to complex string conversion
bool Au_automaton::searchlastraw(string& s, long& b, long& e, long init) {
    wstring w;
    long sz = s.size();
    w.reserve(sz);
    for (long i =0; i< sz; i++)
        w +=  (wchar_t)(uchar)s[i] ;
    return searchlast(w, b, e, init);
}

//----------------------------------------------------------------
void Au_automaton::searchall(wstring& w, vector<long>& res, long init) {
    long f;
    long sz = w.size();
    bool conv = CheckNeedConversion(w);
    
    for (long d=init;d<sz;d++) {
        f=first->loop(w,d);
        if (f!=au_error) {
            if (conv) {
                init = f;
                conversionpostochar(w,d,init);
                res.push_back(d);
                res.push_back(init);
            }
            else {
                res.push_back(d);
                res.push_back(f);
            }
            d=f-1;
        }
    }
}


void Au_automaton::searchall(string& s, vector<long>& res, long init) {
    wstring w;
    if (CheckNeedConversion(s)) {
        s_utf8_to_unicode(w, USTR(s), s.size());
        if (init)
            init = c_bytetocharposition(USTR(s), init);
        searchall(w, res, init);
        v_convertchartobyteposition(USTR(s), res);
    }
    else {
        long sz = s.size();
        w.reserve(sz);
        for (long i =0; i< sz; i++)
            w +=  (wchar_t)(uchar)s[i] ;
        searchall(w, res, init);
    }
}

//The string is not a UTF8 string, no need for char to byte position conversion
void Au_automaton::searchallraw(string& s, vector<long>& res, long init) {
    wstring w;
    long sz = s.size();
    w.reserve(sz);
    for (long i =0; i< sz; i++)
        w+=(wchar_t)(uchar)s[i];
    searchall(w, res, init);
}

//----------------------------------------------------------------
bool Au_arc::find(wstring& w, wstring& wsep, long i, vector<long>& res) {
    if (i==w.size())
        return false;
    
	_d_current_i
    TAMGUCHAR c = getechar(w, i);

    switch(action->compare(c)) {
        case 0:
            return false;
        case 1:
            if (w[i] == wsep[0])
                res.push_back(i+1);
            return state->find(w, wsep, i+1, res);
        case 2:
			return state->find(w, wsep, _current_i, res);
    }
    return false;
}

bool Au_state::find(wstring& w, wstring& wsep, long i, vector<long>& res) {
    long ps=res.size();
    long j;
    
    if (status & an_beginning) {
        if (i)
            return au_error;
    }
    
    for (j=0;j<arcs.last;j++) {
        if (!arcs[j]->find(w, wsep, i, res)) {
            while (ps != res.size()) {
                res.pop_back();
            }
        }
        else
            return true;
    }
    
    if (isend()) {
        if (status & an_ending) {
            if (i != w.size())
                return false;
        }
        
        if (res[res.size()-1]!=i+1)
            res.push_back(i+1);
        return true;
    }
    
    return false;
}

void Au_automaton::find(string& s, string& sep, vector<long>& res) {
    wstring w;
    s_utf8_to_unicode(w, USTR(s), s.size());
    wstring wsep;
    s_utf8_to_unicode(wsep, USTR(sep), sep.size());
    first->find(w,wsep,0,res);
}

void Au_automaton::find(wstring& w, wstring& wsep, vector<long>& res) {
    first->find(w,wsep,0,res);
}
//----------------------------------------------------------------
//The next two methods return raw indexes... No conversion needed
//This is used in Tamguregularexpression::in
bool Au_automaton::bytesearch(wstring& w, long& b, long& e) {
    long sz = w.size();
    for (b=0; b<sz; b++) {
        e=first->loop(w,b);
        if (e!=au_error)
            return true;
    }
    b=au_error;
    return false;
}


void Au_automaton::bytesearchall(wstring& w, vector<long>& res) {
    long f;
    long sz = w.size();
    for (long d=0; d<sz; d++) {
        f=first->loop(w,d);
        if (f!=au_error) {
            res.push_back(d);
            res.push_back(f);
            d=f-1;
        }
    }
}

//----------------------------------------------------------------

void Au_state::merge(Au_state* a) {
    if (a->mark)
        return;
    a->mark=true;
    
    status |= a->status;
    
    long sz=arcs.size();
    for (long i=0;i<a->arcs.size();i++) {
        if (a->arcs[i]->state->isrule()) {
            arcs.push_back(a->arcs[i]);
            continue;
        }
        
        long found=au_error;
        for (long j=0;j<sz;j++) {
            if (a->arcs[i]->same(arcs[j])) {
                found=j;
                break;
            }
        }
        if (found==au_error)
            arcs.push_back(a->arcs[i]);
        else {
            arcs[found]->state->merge(a->arcs[i]->state);
            a->arcs[i]->state->mark=an_remove;
            a->arcs[i]->mark=an_remove;
        }
    }
}
//----------------------------------------------------------------

Au_automate::Au_automate(string rgx) {
    wstring wrgx;
    s_utf8_to_unicode(wrgx, USTR(rgx), rgx.size());
    first=NULL;
    parse(wrgx,&garbage);
}


Au_automate::Au_automate(wstring& wrgx) {
    first=NULL;
    parse(wrgx,&garbage);
}

Au_automaton::Au_automaton(string rgx) {
    wstring wrgx;
    s_utf8_to_unicode(wrgx, USTR(rgx), rgx.size());
    first=NULL;
    parse(wrgx);
}

bool Au_automaton::parse(wstring& w, Au_automatons* aus) {
    //static x_wautomaton xtok;
    //first we tokenize
    
    //First we detect the potential %X, where X is a macro
    
    vector<wstring> toks;
    vector<aut_actions> types;
    if (!tokenize(w,toks, types))
        return false;
    
    {
        Locking _lock(globalTamgu->_parselock);
        if (patterns.size() != 0) {
            bool found = false;
            wstring wconc;
            for (long i =0; i< toks.size(); i++) {
                if (types[i] == aut_meta && patterns.check((uchar)toks[i][1])) {
                    //We have a candidate...
                    wconc += patterns[(uchar)toks[i][1]];
                    found = true;
                }
                else
                    wconc += toks[i];
            }
            if (found) {
                toks.clear();
                types.clear();
                if (!tokenize(wconc,toks, types))
                    return false;
            }
        }
    }
    
    if (aus==NULL) {
        if (globalTamgu->gAutomatons==NULL)
            globalTamgu->gAutomatons=new Au_automatons;
        aus=globalTamgu->gAutomatons;
    }
    
    if (first==NULL)
        first=aus->state();
    
    Au_state base;
    long ab,sb;
    aus->boundaries(sb,ab);
    
    if (base.build(aus, 0, toks,types,NULL)==NULL)
        return false;
    
    base.removeepsilon();
    base.mark=false;
    aus->clear(sb,ab);
    first->merge(&base);
    
    //we delete the elements that have been marked for deletion...
    aus->clean(sb,ab);
    return true;
}

bool Au_automate::compiling(wstring& w,long r) {
    //static x_wautomaton xtok;
    //first we tokenize
    
    vector<wstring> toks;
    vector<aut_actions> types;
    if (!tokenize(w,toks, types))
        return false;
    
    {
        Locking _lock(globalTamgu->_parselock);
        if (patterns.size() != 0) {
            bool found = false;
            wstring wconc;
            for (long i =0; i< toks.size(); i++) {
                if (types[i] == aut_meta && patterns.check((uchar)toks[i][1])) {
                    //We have a candidate...
                    wconc += patterns[(uchar)toks[i][1]];
                    found = true;
                }
                else
                    wconc += toks[i];
            }
            if (found) {
                toks.clear();
                types.clear();
                if (!tokenize(wconc,toks, types))
                    return false;
            }
        }
    }
    
    if (first==NULL)
        first=garbage.state();
    
    Au_state base;
    long ab,sb;
    garbage.boundaries(sb,ab);
    
    if (base.build(&garbage, 0, toks,types,NULL)==NULL)
        return false;
    
    Au_state* af=garbage.statefinal(r);
    Au_arc* fin=garbage.arc(new Au_epsilon(),af);
    base.addrule(fin);
    
    base.mark=false;
    garbage.clear(sb,ab);
    first->merge(&base);
    
    garbage.clean(sb,ab);
    return true;
}

//----------------------------------------------------------------------------------------
Au_state* Au_state::build(Au_automatons* aus, long i,vector<wstring>& toks, vector<aut_actions>& types, Au_state* common) {
    mark=false;
    Au_arc* ar;
    bool nega = false;

    if (i==toks.size()) {
        status |= an_end;
        if (common != NULL) {
            ar=aus->arc(new Au_epsilon(), common);
            arcs.push_back(ar);
        }
        
        return this;
    }
    
    if (types[i] == aut_negation) {
        ++i;
        nega = true;
    }
    
    long j;
    bool stop;
    short count;
    vector<wstring> ltoks;
    vector<aut_actions> ltypes;
    Au_state* ret = NULL;
    uchar localtype = types[i];
    
    switch(localtype) {
        case aut_ocrl_brk: { //{..}
            i++;
            Au_state* commonend=aus->state();
            VECTE<Au_arc*> locals;
            stop=false;
            while (i<toks.size() && !stop) {
                switch(types[i]) {
                    case aut_ccrl_brk_plus: //}+
                    case aut_ccrl_brk_star: //}*
                    case aut_ccrl_brk: //}
                        stop=true;
                        break;
                    case aut_ocrl_brk:
                    case aut_obrk:
                    case aut_opar: { //a sub expression introduced with (), {} or []
                        uchar current_action=types[i]; // the current type...
                        uchar lbound=current_action+1;
                        uchar hbound=current_action+3;
                        if (current_action==aut_opar)
                            hbound=lbound; //the only value is aut_cpar...
                        ltoks.clear(); //we extract the sub-element itself...
                        ltypes.clear();
                        count=1;
                        ltoks.push_back(toks[i]);
                        ltypes.push_back(types[i]);
                        i++;
                        while (i<toks.size() && count) {
                            ltoks.push_back(toks[i]);
                            ltypes.push_back(types[i]);
                            if (types[i]==current_action) //which can be other sub-elements of the same kind...
                                count++;
                            else {
                                if (types[i]>=lbound && types[i]<=hbound) //the stop value, with a control over the potential embedding...
                                    count--;
                            }
                            i++;
                        }
                        if (i==toks.size() || !ltoks.size()) //We could not find the closing character, this is an error...
                            return NULL;
                        
                        Au_state s;
                        ret=s.build(aus, 0,ltoks,ltypes,commonend);
                        if (ret==NULL)
                            return NULL;
                        
                        //It cannot be an end state, commonend will be...
                        ret->removeend();
                        
                        for (j=0;j<s.arcs.last;j++) {
                            if (s.arcs[j]->state == commonend)
                                continue;
                            
                            locals.push_back(s.arcs[j]);
                            arcs.push_back(s.arcs[j]);
                        }
                        break;
                    }
                    default:
                        ar=build(aus, toks[i],types[i],commonend, false);
                        if (ar==NULL)
                            return NULL;
                        locals.push_back(ar);
                        i++;
                }
            }
            
            if (i==toks.size())
                return NULL;

            if (nega) {
                commonend->status = an_error;
                //in this case, commonend is the path to error...
                //we create a parallel path, which lead to either a loop or a
                ar=aus->arc(new Au_any(aut_any));
                arcs.push_back(ar);
                commonend = ar->state;
                locals.push_back(ar);
            }

            //aut_ccrl_brk_plus: closing curly bracked+
            //aut_ccrl_brk_star: closing curly bracked*
            if (types[i]==aut_ccrl_brk_plus || types[i]==aut_ccrl_brk_star) {//The plus and the star for the disjunction {...}
                for (j=0;j<locals.size();j++)
                    commonend->arcs.push_back(locals[j]);
                if (types[i]==aut_ccrl_brk_star) {
                    ar=aus->arc(new Au_epsilon());
                    arcs.push_back(ar);
                    commonend->arcs.push_back(ar);
                    commonend=ar->state;
                }
                else
                    commonend->status |= an_mandatory; //this is now an end to any automaton traversal...
            }
            else
                commonend->status |= an_mandatory;
            
            ret = commonend->build(aus, i+1,toks,types,common);
            if (ret != NULL && ret->isend() && !ret->ismandatory())
                status |= an_end;
            return ret;
        }
        case aut_opar: {//(..)
            if (nega)
                return NULL;

            i++;
            count=1;
            while (i<toks.size()) {
                if (types[i]==aut_opar) //embeddings
                    count++;
                else {
                    if (types[i]==aut_cpar) {
                        count--;
                        if (!count)
                            break;
                    }
                }
                ltoks.push_back(toks[i]);
                ltypes.push_back(types[i]);
                i++;
            }
            if (i==toks.size() || !ltoks.size())
                return NULL;
            ret=build(aus, 0,ltoks,ltypes,NULL);
            if (ret==NULL)
                return NULL;
            
            ret->removeend();
            //We jump...
            ar=aus->arc(new Au_epsilon(), ret);
            arcs.push_back(ar);
            //These are the cases, when we have a x* at the end of an expression...
            //The current node can be an end too
            ret->removemandatory();
            ret = ret->build(aus, i+1,toks,types,common);
            if (ret != NULL && ret->isend() && !ret->ismandatory())
                status |= an_end;
            return ret;
        }
        case aut_obrk: { //[..]
            i++;
            count=1;
            ltoks.clear();
            ltypes.clear();
            while (i<toks.size()) {
                if (types[i]==aut_obrk) //embeddings
                    count++;
                else {
                    if (types[i]==aut_cbrk_plus || types[i]==aut_cbrk_star || types[i]==aut_cbrk) {
                        count--;
                        if (!count)
                            break;
                    }
                }
                ltoks.push_back(toks[i]);
                ltypes.push_back(types[i]);
                i++;
            }
            if (i==toks.size() || !ltoks.size())
                return NULL;
            
            Au_state s;
            ret=s.build(aus, 0,ltoks,ltypes,NULL);
            if (ret==NULL)
                return NULL;
            
            ret->removeend();
            ret->status |= an_mandatory; //if it is a +, we expect at least one value, cannot be a potential end

            if (types[i]!=aut_cbrk) {//the plus
                //s is our starting point, it contains all the arcs we need...
                for (j=0;j<s.arcs.last;j++) {
                    arcs.push_back(s.arcs[j]);
                    ret->arcs.push_back(s.arcs[j]);
                } //we need to jump back to our position...
                
                if (types[i]==aut_cbrk_star) {//this is a star, we need an epsilon to escape it...
                    ar=aus->arc(new Au_epsilon(), ret);
                    arcs.push_back(ar);
                    ret->status &= ~an_mandatory;
                }
            }
            else {
                for (j=0;j<s.arcs.last;j++)
                    arcs.push_back(s.arcs[j]);
            }
            
            //These are the cases, when we have a x* at the end of an expression...
            //The current node can be an end too
            ret = ret->build(aus, i+1,toks,types,common);
            if (ret != NULL && ret->isend() && !ret->ismandatory())
                status |= an_end;
            return ret;
        }
    }
    
    Au_state* next;
    if ((i+1)==toks.size())
        next=common;
    else
        next=NULL;
    
    if (toks[i] == L"^") {
        status |= an_beginning;
        return build(aus, i+1,toks,types,common);
    }
    
    if (toks[i] == L"$") {
        ret = build(aus, i+1,toks,types,common);
        ret->status |= an_ending;
        return ret;
    }

    Au_arc* retarc=build(aus, toks[i], localtype,next, nega);
    if (retarc==NULL)
        return NULL;

    next = retarc->state->build(aus, i+1,toks,types,common);
	    
    if (next != NULL && next->isend() && !next->ismandatory()) {
        //If the current element is a *, then it can be skipped up to the end...
        switch(localtype) {
            case aut_meta_star:
            case aut_reg_star:
            case aut_any_star:
                status |= an_end;
                break;
            default: //from now on, we are in a mandatory section
                next->status |= an_mandatory;
        }
    }
    
    return next;
}

bool checkmeta(wstring& tok) {
    switch(tok[1]) {
        case 'C':
        case 'E':
        case 'H':
        case 'S':
        case 'a':
        case 'c':
        case 'd':
        case 'e':
        case 'n':
        case 'p':
        case 'r':
        case 's':
        case 'x': //hexadecimal character
            return true;
        default:
            return false;
    }
}

Au_arc* Au_state::build(Au_automatons* aus, wstring& token, uchar type, Au_state* common, bool nega) {
    //First we scan the arcs, in case, it was already created...
    Au_any* a=NULL;
    
    //value arc: meta or character...
    switch(type) {
        case aut_any: //?
        case aut_any_plus: //?+
        case aut_any_star: //?*
            a=new Au_any(type);
            break;
        case aut_meta:
        case aut_meta_plus: //%x+
        case aut_meta_star: //%x*
            if (checkmeta(token)) //if we are dealing with an escaped character
                a = new Au_meta(token[1], type);
            else
                a = new Au_char(token[1], type);
            break;
        case aut_reg:
        case aut_reg_plus: //x+
        case aut_reg_star: //x*
            a=new Au_char(token[0], type);
            break;
        default:
            return NULL;
    }
    
    a->setvero(nega);
    
    //we check if we already have such an arc...
    for (long j=0;j<arcs.size();j++) {
        if (arcs[j]->action->same(a)) {
            delete a;
            arcs[j]->mark=false;
            return arcs[j];
        }
    }
    
    //Different case...
    //first if a is not NULL and a is a loop
    Au_arc* current=aus->arc(a, common);
    Au_arc* ar;
    switch(type) {
        case aut_meta_plus: //+
        case aut_reg_plus:
        case aut_any_plus:
            current->state->status |= an_mandatory; //we mark that this state as a mandatory one
            current->state->arcs.push_back(current); //the loop
            arcs.push_back(current);
            return current;
        case aut_meta_star: //*
        case aut_reg_star:
        case aut_any_star:
            ar=aus->arc(new Au_epsilon(),current->state);
            arcs.push_back(ar);
            current->state->arcs.push_back(current); //the loop
            arcs.push_back(current);
            return current;
        default:
            current->state->status |= an_mandatory; //we mark that this state as a mandatory one
            if (arcs.last>0) //We always insert our arc at the top to force its recognition before any loop...
                arcs.insert(0,current);
            else
                arcs.push_back(current);
    }
    return current;
}
//---------------------------------------------------------------------------------------------------------
// Detecting meta-characters in strings...
void replaceescapedcharacters(string& thestr, string sub) {
    
    if (sub.find("\\")==-1) {
        thestr=sub;
        return;
    }
    
    for (long i=0;i<sub.size();i++) {
        if (sub[i]=='\\') {
            switch(sub[++i]) {
                case 'n':
                    thestr+="\n";
                    break;
                case 'r':
                    thestr+="\r";
                    break;
                case 't':
                    thestr+="\t";
                    break;
                default:
                    thestr+=sub[i];
                    
            }
        }
        else
            thestr+=sub[i];
    }
}

void EvaluateMetaInString(wstring& w, wstring& wsub) {
    static Au_automate a("{[%%%x%x][&%c+;][&#%d+;][u%x%x%x%x][\\u%x%x%x%x][\\%d%d%d][\\?]}");
    
    vector<long> vectr;
    a.searchall(wsub,vectr);
    if (vectr.size()==0) {
        w=wsub;
        return;
    }
    
    w=wsub;
    wstring s;
    for (long i=vectr.size()-2;i>=0; i-=2) {
        s=wsub.substr(vectr[i], vectr[i+1]-vectr[i]);
        s_EvaluateMetaCharacters(s);
        w=w.substr(0,vectr[i])+s+w.substr(vectr[i+1],w.size()-vectr[i+1]);
    }
}

void EvaluateMetaInString(string& thestr, string sub) {
    static Au_automate a("{[%%%x%x][&%c+;][&#%d+;][u%x%x%x%x][\\u%x%x%x%x][\\%d%d%d][\\?]}");
    
    wstring wsub;
    s_utf8_to_unicode(wsub, USTR(sub), sub.size());
    
    vector<long> vectr;
    a.searchall(wsub,vectr);
    if (vectr.size()==0) {
        thestr=sub;
        return;
    }
    
    wstring w=wsub;
    wstring s;
    for (long i=vectr.size()-2;i>=0; i-=2) {
        s=wsub.substr(vectr[i], vectr[i+1]-vectr[i]);
        s_EvaluateMetaCharacters(s);
        w=w.substr(0,vectr[i])+s+w.substr(vectr[i+1],w.size()-vectr[i+1]);
    }
    
    s_unicode_to_utf8(thestr,w);
}
