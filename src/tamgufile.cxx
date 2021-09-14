/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgufile.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgufile.h"
#include "tamguautomaton.h"
#include "tamgutaskell.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<fileMethod>  Tamgufile::methods;
Exporting hmap<string, string> Tamgufile::infomethods;
Exporting basebin_hash<unsigned long> Tamgufile::exported;

Exporting short Tamgufile::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgufile::AddMethod(TamguGlobal* global, string name, fileMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgufile::Setidtype(TamguGlobal* global) {
    Tamgufile::InitialisationModule(global,"");
}


   bool Tamgufile::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    Tamgufile::idtype = global->Getid("file");

    Tamgufile::AddMethod(global, "_initial", &Tamgufile::MethodInitial, P_NONE | P_ONE | P_TWO, "_initial(name,op): load a file in read/write/append mode. Default is read.");
    Tamgufile::AddMethod(global, "close", &Tamgufile::MethodClose, P_NONE, "close(): close a file.");
    Tamgufile::AddMethod(global, "signature", &Tamgufile::MethodSignature, P_NONE | P_ONE, "signature(bool): With no argument: return 0 if no signature, 8 if UTF8 or 16 if UTF16 or set the signature.");
    Tamgufile::AddMethod(global, "openread", &Tamgufile::MethodOpenRead, P_ONE, "openread(string filename): open a file in reading mode.");
    Tamgufile::AddMethod(global, "ropen", &Tamgufile::MethodOpenRead, P_ONE, "ropen(string filename): open a file in reading mode.");
    Tamgufile::AddMethod(global, "openwrite", &Tamgufile::MethodOpenWrite, P_ONE, "openwrite(string filename): open a file in writing mode.");
    Tamgufile::AddMethod(global, "wopen", &Tamgufile::MethodOpenWrite, P_ONE, "wopen(string filename): open a file in writing mode.");
    Tamgufile::AddMethod(global, "openappend", &Tamgufile::MethodOpenAppend, P_ONE, "openappend(string filename): open a file in append mode.");
    Tamgufile::AddMethod(global, "aopen", &Tamgufile::MethodOpenAppend, P_ONE, "aopen(string filename): open a file in append mode.");
    Tamgufile::AddMethod(global, "get", &Tamgufile::MethodGet, P_NONE, "get(): get a character.");
    Tamgufile::AddMethod(global, "tell", &Tamgufile::MethodTell, P_NONE, "tell(): return position in the file.");
    Tamgufile::AddMethod(global, "seek", &Tamgufile::MethodSeek, P_ONE, "seek(int p): position the curson at p in the file.");
    Tamgufile::AddMethod(global, "unget", &Tamgufile::MethodUnget, P_ONE, "unget(int c): return character c in the file.");
    Tamgufile::AddMethod(global, "write", &Tamgufile::MethodWrite, P_ONE, "write(string c): write string s in the file.");
    Tamgufile::AddMethod(global, "writeln", &Tamgufile::MethodWriteln, P_ONE, "writeln(string c): write string s in the file, with a final carriage return.");
    Tamgufile::AddMethod(global, "writebin", &Tamgufile::MethodWritebin, P_ONE, "writebin(byte c|bvector b): write one byte to the file or the content of a byte vector.");
    Tamgufile::AddMethod(global, "writeutf16", &Tamgufile::MethodWriteutf16, P_ONE | P_TWO, "writeutf16(ustring s, bool addln): write UTF16 characters to the file. when addln is true, works as writeln.");
    Tamgufile::AddMethod(global, "read", &Tamgufile::MethodRead, P_NONE | P_ONE, "read(int nb): read nb characters from the file or all if nb is not supplied.");
    Tamgufile::AddMethod(global, "readline", &Tamgufile::MethodReadoneline, P_NONE, "readline(): read a line from the document.");
    Tamgufile::AddMethod(global, "eof", &Tamgufile::MethodEof, P_NONE, "eof(): return true, if the end has been reached.");
    Tamgufile::AddMethod(global, "scan", &Tamgufile::MethodScan, P_ONE | P_TWO, "scan(string rgx, string sep): scan file with rgx, sep is optional and is by default a space. Return a list of strings");
    Tamgufile::AddMethod(global, "flush", &Tamgufile::MethodFlush, P_NONE, "flush(): flush the buffer to disk.");

    if (version != "") {
        global->newInstance[Tamgufile::idtype] = new Tamgufile("", global);
        global->RecordMethods(Tamgufile::idtype,Tamgufile::exported);
    }

    return true;
}



Tamgu* Tamgufile::MethodScan(Tamgu* context, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    if (thefile == NULL || feof(thefile) || op != "rb")
        return globalTamgu->Returnerror("File not open", idthread);

    Tamgu* pattern = callfunc->Evaluate(0, context, idthread);
    wstring sep= L" ";
    
    if (callfunc->Size() == 2)
        sep = callfunc->Evaluate(1, context, idthread)->UString();
    //We record the initial position in the file...
    long pos=ftell(thefile);
    //We read a pretty large chunk of it
    string buff = read(16384);
    
    Au_automate* a = getautomate(pattern);
    
    if (a == NULL)
        return globalTamgu->Returnerror("Wrong grammar definition");

    wstring wbuff;
    
    s_utf8_to_unicode(wbuff, USTR(buff), buff.size());
    
    vector<long> vectr;
    vectr.push_back(0);
    a->find(wbuff,sep, vectr);
    if (vectr.size()==1) {
        fseek(thefile, pos, SEEK_SET);
        return aNULL;
    }
    
    Tamguuvector* vect = (Tamguuvector*)Selectauvector(context);
    wstring sub;
    for (long i=0; i< vectr.size()-1; i++) {
        sub = wbuff.substr(vectr[i], vectr[i+1]-vectr[i]-1);
        if (sub != L"")
            vect->values.push_back(sub);
    }
    
    pos=c_chartobyteposition(USTR(buff), vectr[vectr.size()-1]-1);
    fseek(thefile, pos, SEEK_SET);

    return vect;
}


Tamgu* Tamgufile::MethodRead(Tamgu* context, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    if (thefile == NULL || feof(thefile) || op != "rb")
        return globalTamgu->Returnerror("Wrong access to the file", idthread);

    string bf;
    long nb = -1;
    long total;
    if (callfunc->Size() == 1) {
        nb = callfunc->Evaluate(0, aNULL, idthread)->Integer();
        if (nb != -1) {
            if (context->isVectorContainer()) {
                Tamgu* vect = Selectavector(context);

                if (vect->Type() == a_vector) {
                    vector<string> v;
                    readnb(nb, v);
                    for (size_t i = 0; i < v.size(); i++)
                        vect->storevalue(v[i]);
                    return vect;
                }

                switch (vect->Type()) {
                    case a_svector:
                    readnb(nb, ((Tamgusvector*)vect)->values);
                    break;
                    case a_uvector:
                    readnb(nb, ((Tamguuvector*)vect)->values);
                    break;
                    default:
                    total = readnb(nb);
                    for (nb = 0; nb < fchars.size(); nb++)
                        vect->storevalue(fchars[nb]);
                }
                return vect;
            }

            readin(bf, nb);
            return globalTamgu->Providewithstring(bf);
        }
    }

    if (context->isContainer()) {
        Tamgu* vect = Selectavector(context);
        
        if (vect->Type() == a_vector) {
            vector<string> v;
            readall(v);
            for (size_t i = 0; i < v.size(); i++)
                vect->storevalue(v[i]);
            return vect;
        }

        switch (vect->Type()) {
            case a_svector:
            readall(((Tamgusvector*)vect)->values);
            break;
            case a_uvector:
            readall(((Tamguuvector*)vect)->values);
            break;
            default:
            total = readnb(-1);
            for (nb = 0; nb < fchars.size(); nb++)
                vect->storevalue(fchars[nb]);
        }
        return vect;

    }

    if (context->isNumber())
        return globalTamgu->ProvideConstint(get());

    bf = read(-1);
    return globalTamgu->Providewithstring(bf);
}

Tamgu* Tamgufile::Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {
    Locking _lock(this);
    if (thefile == NULL || feof(thefile) || op != "rb")
        return globalTamgu->Returnerror("Wrong access to the file", idthread);

    Tamgu* a;
    uchar addvalue = 0;
    
    if (context != aNOELEMENT)
        addvalue = Selecttype(context);
    
    Tamgustring line("");
    while (!eof()) {
        line.value = readoneline();
        recipient->Putvalue(&line, idthread);
        
        a = bd->DirectEval(environment, aNULL, idthread);
        if (a->isNULL())
            continue;
        
        if (globalTamgu->Error(idthread) || a->needInvestigate()) {
            if (a == aBREAK)
                break;
            
            recipient->Clean();
            recipient->Forcedclean();
            a->Release();
            context->Release();
            return globalTamgu->Errorobject(idthread);
        }
        
        context = Storealongtype(context, a, idthread, addvalue);
        a->Release();
    }
    
    recipient->Clean();
    recipient->Forcedclean();
    return context;
}

Tamgu* Tamgufile::Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct) {
    Locking _lock(this);
    if (thefile == NULL || feof(thefile) || op != "rb")
        return globalTamgu->Returnerror("Wrong access to the file", idthread);

    Tamgu* returnval;
    
    bool first = false;
    Tamgu* key;
    
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
    
    Tamgustring line("");
    while (!eof()) {
        line.value = readoneline();
        key = &line;
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
                var->Clean();
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
    
    var->Clean();
    var->Forcedclean();
    if (kcont != NULL)
        return kcont;
    return accu->Value();
}

