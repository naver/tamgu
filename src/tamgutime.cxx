/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgutime.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgutime.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<timeMethod>  Tamgutime::methods;
Exporting hmap<string, string> Tamgutime::infomethods;
Exporting bin_hash<unsigned long> Tamgutime::exported;

Exporting short Tamgutime::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgutime::AddMethod(TamguGlobal* global, string name, timeMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgutime::Setidtype(TamguGlobal* global) {
    Tamgutime::InitialisationModule(global,"");
}


   bool Tamgutime::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    Tamgutime::idtype = global->Getid("time");

    Tamgutime::AddMethod(global, "reset", &Tamgutime::MethodReset, P_NONE, "reset(): reset the time");

    if (version != "") {
        global->newInstance[Tamgutime::idtype] = new Tamgutime(global);
        global->RecordMethods(Tamgutime::idtype,Tamgutime::exported);
    }

    return true;
}



Tamgu* Tamgutime::Put(Tamgu* index, Tamgu* val, short idthread) {
    if (val->Type() == Tamgutime::idtype) {
        value = ((Tamgutime*)val)->value;
        return aTRUE;
    }
    return aFALSE;
}

Tamgu* Tamgutime::Eval(Tamgu* context, Tamgu* index, short idthread) {
    return this;
}

string Tamgutime::String() {
    char buffer[100];
    double timeval = Float() / 1000000UL;
    sprintf_s(buffer, 100, "%fs", timeval);
    return buffer;
}


Tamgu* Tamgutime::MethodReset(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a time
    gettimeofday(&value, NULL);
    return this;
}
