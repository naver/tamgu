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

Exporting short Tamgutime::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgutime::AddMethod(TamguGlobal* global, string name, timeMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}




void Tamgutime::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamgutime::InitialisationModule(global,"");
}


   bool Tamgutime::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    

    Tamgutime::idtype = global->Getid("time");

    Tamgutime::AddMethod(global, "reset", &Tamgutime::MethodReset, P_NONE, "reset(): reset the time");

    if (version != "") {
        global->newInstance[Tamgutime::idtype] = new Tamgutime(global);
        global->RecordCompatibilities(Tamgutime::idtype);
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

void Tamgutime::Setstring(string& v, short idthread) {
    char buffer[100];
    double timeval = Float() / 1000000UL;
    sprintf_s(buffer, 100, "%fs", timeval);
    v =  buffer;
}

void Tamgutime::Setstring(wstring& v, short idthread) {
    wchar_t buffer[100];
    double timeval = Float() / 1000000UL;
    swprintf_s(buffer, 100, L"%fs", timeval);
    v = buffer;
}

Tamgu* Tamgutime::MethodReset(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a time
    gettimeofday(&value, NULL);
    return this;
}
