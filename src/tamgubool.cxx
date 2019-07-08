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
Exporting basebin_hash<boolMethod>  Tamgubool::methods;
Exporting hmap<string, string> Tamgubool::infomethods;
Exporting bin_hash<unsigned long> Tamgubool::exported;

Exporting short Tamgubool::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgubool::AddMethod(TamguGlobal* global, string name, boolMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgubool::Setidtype(TamguGlobal* global) {
        Tamgubool::idtype = global->Getid("bool");
    }

   bool Tamgubool::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    Tamgubool::idtype = global->Getid("bool");

    Tamgubool::AddMethod(global, "invert", &Tamgubool::MethodInvert, P_NONE, "invert(): Invert ");
    Tamgubool::AddMethod(global, "succ", &Tamgubool::MethodInvert, P_NONE, "succ(): successor of a Boolean.");

    global->newInstance[Tamgubool::idtype] = new Tamgubool(0, global);
    global->RecordMethods(Tamgubool::idtype,Tamgubool::exported);
    global->RecordMethods(a_bloop,Tamgubool::exported);

    return Tamguatomicbool::InitialisationModule(global, version);
}

Exporting basebin_hash<atomicboolMethod>  Tamguatomicbool::methods;
Exporting hmap<string, string> Tamguatomicbool::infomethods;
Exporting bin_hash<unsigned long> Tamguatomicbool::exported;

Exporting short Tamguatomicbool::idtype = 0;


    //MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguatomicbool::AddMethod(TamguGlobal* global, string name, atomicboolMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Tamguatomicbool::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();
    
    Tamguatomicbool::idtype = global->Getid("a_bool");
    
    Tamguatomicbool::AddMethod(global, "invert", &Tamguatomicbool::MethodInvert, P_NONE, "invert(): Invert ");
    Tamguatomicbool::AddMethod(global, "succ", &Tamguatomicbool::MethodInvert, P_NONE, "succ(): successor of a Boolean.");
    
    global->newInstance[Tamguatomicbool::idtype] = new Tamguatomicbool(0, global);
    global->RecordMethods(Tamguatomicbool::idtype,Tamguatomicbool::exported);
    
    return true;
}
