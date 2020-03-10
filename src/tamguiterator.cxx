/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguiterator.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamguiterator.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<iteratorMethod>  Tamguiterator::methods;
Exporting hmap<string, string> Tamguiterator::infomethods;
Exporting bin_hash<unsigned long> Tamguiterator::exported;

Exporting short Tamguiterator::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguiterator::AddMethod(TamguGlobal* global, string name, iteratorMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamguiterator::Setidtype(TamguGlobal* global) {
    Tamguiterator::InitialisationModule(global,"");
}


   bool Tamguiterator::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    Tamguiterator::idtype = global->Getid("iterator");

    Tamguiterator::AddMethod(global, "begin", &Tamguiterator::MethodBegin, P_NONE, "begin(): Start the iteration.");
    Tamguiterator::AddMethod(global, "key", &Tamguiterator::MethodKey, P_NONE, "key(): Return current key.");
    Tamguiterator::AddMethod(global, "value", &Tamguiterator::MethodValue, P_NONE, "value(): Return the current value.");
    Tamguiterator::AddMethod(global, "next", &Tamguiterator::MethodNext, P_NONE, "next(): next iteration.");
    Tamguiterator::AddMethod(global, "end", &Tamguiterator::MethodEnd, P_NONE, "end(): return true when the iteration is over.");
    
    if (version != "") {
        global->newInstance[Tamguiterator::idtype] = new Tamguiterator(global);
        global->RecordMethods(Tamguiterator::idtype,Tamguiterator::exported);
    }

    return true;
}
