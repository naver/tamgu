/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgufraction.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgufraction.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<fractionMethod>  Tamgufraction::methods;

Exporting short Tamgufraction::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgufraction::AddMethod(TamguGlobal* global, string name, fractionMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}




void Tamgufraction::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamgufraction::InitialisationModule(global,"");
}


   bool Tamgufraction::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    

    Tamgufraction::idtype = a_fraction;

    Tamgufraction::AddMethod(global, "_initial", &Tamgufraction::MethodInitial, P_ONE | P_TWO, "_initial(n,d): initialize a fraction");
    Tamgufraction::AddMethod(global, "nd", &Tamgufraction::MethodInitial, P_ONE | P_TWO, "nd(n,d): initialize a fraction");
    Tamgufraction::AddMethod(global, "simplify", &Tamgufraction::MethodSimplify, P_NONE, "simplify(): simplify a fraction");

    if (version != "") {
        global->newInstance[Tamgufraction::idtype] = new Tamgufraction(global);
        global->RecordCompatibilities(Tamgufraction::idtype);
    }

    return true;
}
