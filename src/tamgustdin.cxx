/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgustdin.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "instructions.h"
#include "tamguversion.h"
#include "tamgustdin.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<stdinMethod>  Tamgustdin::methods;
Exporting hmap<string, string> Tamgustdin::infomethods;
Exporting bin_hash<unsigned long> Tamgustdin::exported;

Exporting short Tamgustdin::idtype = 0;

//------------------------------------------------------------------------------------------------------------------

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgustdin::AddMethod(TamguGlobal* global, string name, stdinMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgustdin::Setidtype(TamguGlobal* global) {
        Tamgustdin::idtype = global->Getid("_stdin");
    }

   bool Tamgustdin::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    Tamgustdin::idtype = global->Getid("_stdin");

    Tamgustdin::AddMethod(global, "get", &Tamgustdin::MethodGet, P_NONE, "get(): get a character.");
    Tamgustdin::AddMethod(global, "read", &Tamgustdin::MethodRead, P_NONE | P_ONE, "read(int nb): read nb characters from the standard flow: stdin.");
    Tamgustdin::AddMethod(global, "readline", &Tamgustdin::MethodReadLine, P_NONE, "readline(): read up to a CR.");
    Tamgustdin::AddMethod(global, "eof", &Tamgustdin::MethodEof, P_NONE, "eof(): return true, if the end has been reached.");

    global->newInstance[Tamgustdin::idtype] = new Tamgustdin(global);
    global->RecordMethods(Tamgustdin::idtype,Tamgustdin::exported);
    
    Tamgu* a = new TamguSystemVariable(global, new Tamgustdin(global), global->Createid("stdin"), Tamgustdin::idtype);

    return true;
}



Tamgu* Tamgustdin::Put(Tamgu* idx, Tamgu* kval, short idthread) {
    return aTRUE;
}

Tamgu* Tamgustdin::Eval(Tamgu* context, Tamgu* idx, short idthread) {
    Locking _lock(this);
    if (is == NULL || is->eof())
        return aFALSE;
    
    if (context->isNumber()) {
        long c = is->get();
        context->storevalue(c);
        return aTRUE;
    }
    
    string line;
    readline(line);
    context->storevalue(line);
    
    return aTRUE;
}
