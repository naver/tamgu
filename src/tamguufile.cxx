/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguufile.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamguversion.h"
#include "tamguufile.h"
#include "tamguuvector.h"
#include "tamguautomaton.h"
#include "tamgutaskell.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<ufileMethod>  Tamguufile::methods;

Exporting short Tamguufile::idtype = 0;

//------------------------------------------------------------------------------------------------------------------

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguufile::AddMethod(TamguGlobal* global, string name, ufileMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}




void Tamguufile::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamguufile::InitialisationModule(global,"");
}


   bool Tamguufile::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    

    Tamguufile::idtype = global->Getid("ufile");

    Tamguufile::AddMethod(global, "_initial", &Tamguufile::MethodInitial, P_NONE | P_ONE | P_TWO, "_initial(name,op): load a file in read/write/append mode. Default is read.");
    Tamguufile::AddMethod(global, "close", &Tamguufile::MethodClose, P_NONE, "close(): close a file.");
    Tamguufile::AddMethod(global, "openread", &Tamguufile::MethodOpenRead, P_ONE, "openread(string filename): open a file in reading mode.");
    Tamguufile::AddMethod(global, "ropen", &Tamguufile::MethodOpenRead, P_ONE, "ropen(string filename): open a file in reading mode.");
    Tamguufile::AddMethod(global, "openwrite", &Tamguufile::MethodOpenWrite, P_ONE, "openwrite(string filename): open a file in writing mode.");
    Tamguufile::AddMethod(global, "wopen", &Tamguufile::MethodOpenWrite, P_ONE, "wopen(string filename): open a file in writing mode.");
    Tamguufile::AddMethod(global, "openappend", &Tamguufile::MethodOpenAppend, P_ONE, "openappend(string filename): open a file in append mode.");
    Tamguufile::AddMethod(global, "aopen", &Tamguufile::MethodOpenAppend, P_ONE, "aopen(string filename): open a file in append mode.");
    Tamguufile::AddMethod(global, "get", &Tamguufile::MethodGet, P_NONE, "get(): get a character.");
    Tamguufile::AddMethod(global, "tell", &Tamguufile::MethodTell, P_NONE, "tell(): return position in the file.");
    Tamguufile::AddMethod(global, "flush", &Tamguufile::MethodFlush, P_NONE, "flush(): flush the buffer to disk.");
    Tamguufile::AddMethod(global, "seek", &Tamguufile::MethodSeek, P_ONE, "seek(int p): position the curson at p in the file.");
    Tamguufile::AddMethod(global, "unget", &Tamguufile::MethodUnget, P_NONE, "unget(): return last read character in the file.");
    Tamguufile::AddMethod(global, "write", &Tamguufile::MethodWrite, P_ONE, "write(string c): write string s in the file.");
    Tamguufile::AddMethod(global, "writeln", &Tamguufile::MethodWriteln, P_ONE, "writeln(string c): write string s in the file, with a final carriage return.");
    Tamguufile::AddMethod(global, "read", &Tamguufile::MethodRead, P_NONE | P_ONE, "read(int nb): read nb characters from the file or all if nb is not supplied.");
    Tamguufile::AddMethod(global, "readline", &Tamguufile::MethodReadoneline, P_NONE, "readline(): read a line from the document.");
    Tamguufile::AddMethod(global, "eof", &Tamguufile::MethodEof, P_NONE, "eof(): return true, if the end has been reached.");
    Tamguufile::AddMethod(global, "signature", &Tamguufile::MethodSignature, P_NONE, "signature(bool utf8): Set the UTF8 signature mode for files.");
    Tamguufile::AddMethod(global, "scan", &Tamguufile::MethodScan, P_ONE | P_TWO, "scan(string rgx, string sep): scan file with rgx, sep is optional and is by default a space. Return a list of strings");
    Tamguufile::AddMethod(global, "endian", &Tamguufile::MethodEndian, P_NONE | P_ONE, "endian(bool): set endian to bigendian (true) or littleendian(false). With no parameters, return the current setting");

    if (version != "") {
        global->newInstance[Tamguufile::idtype] = new Tamguufile(global);
        global->RecordCompatibilities(Tamguufile::idtype);
    }
    
    short idother;
    idother = global->Getid("wfile");
    if (version != "") {
        global->newInstance[idother] = new Tamguufile(global);
        global->RecordCompatibilities(idother);
    }

    return true;
}



Tamgu* Tamguufile::Put(Tamgu* idx, Tamgu* kval, short idthread) {
    return aTRUE;
}

Tamgu* Tamguufile::Eval(Tamgu* context, Tamgu* idx, short idthread) {
    Locking _lock(this);
    if (thefile == NULL || feof(thefile) || op != "rb")
        return aFALSE;

    if (context->isNumber()) {
        wchar_t c = Get();
        context->storevalue(c);
        return aTRUE;
    }

    wstring line;
    readoneline(line);
    line += L"\n";
    context->storevalue(line);

    return aTRUE;

}

Tamgu* Tamguufile::MethodScan(Tamgu* context, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    if (thefile == NULL || feof(thefile) || op != "rb")
        return globalTamgu->Returnerror("Wrong access to the file", idthread);

    Tamgu* pattern = callfunc->Evaluate(0, context, idthread);
    wstring sep= L" ";
    
    if (callfunc->Size() == 2)
        sep = callfunc->Evaluate(1, context, idthread)->UString();
    //We record the initial position in the file...
    long pos=ftell(thefile);
    //We read a pretty large chunk of it
    wstring wbuff;
    readnb(wbuff, 16384);
    
    Au_automate* a = getautomate(pattern);
    if (a == NULL)
        return globalTamgu->Returnerror("Wrong grammar definition");

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
    
    pos=vectr[vectr.size()-1]-1;
    fseek(thefile, pos, SEEK_SET);

    return vect;
}

//A sample of how to implement a method for your new class...
Tamgu* Tamguufile::MethodRead(Tamgu* context, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    if (thefile == NULL || feof(thefile) || op != "rb")
        return globalTamgu->Returnerror("Wrong access to the file", idthread);

    wstring bf;
    long nb = -1;
    if (callfunc->Size() == 1) {
        nb = callfunc->Evaluate(0, aNULL, idthread)->Integer();
        if (nb != -1) {
            if (context->isVectorContainer()) {
                Tamgu* vect = Selectauvector(context);
                readnb(((Tamguuvector*)vect)->values, nb);
                return vect;
            }

            readnb(bf, nb);
            return globalTamgu->Providewithustring(bf);
        }
    }

    if (context->isContainer()) {
        Tamgu* vect = Selectauvector(context);
        readall(((Tamguuvector*)vect)->values);
        return vect;
    }

    if (context->isNumber())
        return globalTamgu->ProvideConstint(Get());


    readall(bf);
    return globalTamgu->Providewithustring(bf);

}

Tamgu* Tamguufile::Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {
    Locking _lock(this);
    if (thefile == NULL || feof(thefile) || op != "rb")
        return globalTamgu->Returnerror("Wrong access to the file", idthread);

    Tamgu* a;
    uchar addvalue = 0;
    
    if (context != aNOELEMENT)
        addvalue = Selecttype(context);
    
    Tamguustring line(L"");
    while (!eof()) {
        readoneline(line.value);
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

Tamgu* Tamguufile::Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct) {
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
    
    Tamguustring line("");
    while (!eof()) {
        readoneline(line.value);
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


