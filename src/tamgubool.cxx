/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgubool.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgubool.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<boolMethod>  Tamguboolean::methods;

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguboolean::AddMethod(TamguGlobal* global, string name, boolMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(a_boolean) != global->infomethods.end() &&
            global->infomethods[a_boolean].find(name) != global->infomethods[a_boolean].end())
    return;

    global->infomethods[a_boolean][name] = infos;
    global->RecordArity(a_boolean, idname, arity);
    global->RecordArity(a_bloop, idname, arity);
}




void Tamguboolean::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamguboolean::InitialisationModule(global,"");
}


   bool Tamguboolean::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    Tamguboolean::AddMethod(global, "invert", &Tamguboolean::MethodInvert, P_NONE, "invert(): Invert ");
    Tamguboolean::AddMethod(global, "succ", &Tamguboolean::MethodInvert, P_NONE, "succ(): successor of a Boolean.");

    if (version != "") {
        global->newInstance[a_boolean] = new Tamguboolean(0, global);
        global->RecordCompatibilities(a_boolean);
        global->RecordCompatibilities(a_bloop);
    }

    return Tamguatomicbool::InitialisationModule(global, version);
}

Exporting basebin_hash<atomicboolMethod>  Tamguatomicbool::methods;

Exporting short Tamguatomicbool::idtype = 0;


    //MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguatomicbool::AddMethod(TamguGlobal* global, string name, atomicboolMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


bool Tamguatomicbool::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    
    
    Tamguatomicbool::idtype = global->Getid("a_bool");
    
    Tamguatomicbool::AddMethod(global, "invert", &Tamguatomicbool::MethodInvert, P_NONE, "invert(): Invert ");
    Tamguatomicbool::AddMethod(global, "succ", &Tamguatomicbool::MethodInvert, P_NONE, "succ(): successor of a Boolean.");
    
    if (version != "") {
        global->newInstance[Tamguatomicbool::idtype] = new Tamguatomicbool(0, global);
        global->RecordCompatibilities(Tamguatomicbool::idtype);
    }
    
    return true;
}
