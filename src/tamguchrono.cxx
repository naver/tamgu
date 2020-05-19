/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguchrono.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "instructions.h"
#include "constobjects.h"
#include "tamguchrono.h"

#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>

//We need to declare once again our local definitions.
Exporting basebin_hash<chronoMethod>  Tamguchrono::methods;
Exporting hmap<string, string> Tamguchrono::infomethods;
Exporting bin_hash<unsigned long> Tamguchrono::exported;

Exporting short Tamguchrono::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguchrono::AddMethod(TamguGlobal* global, string name, chronoMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamguchrono::Setidtype(TamguGlobal* global) {
    Tamguchrono::InitialisationModule(global,"");
}


   bool Tamguchrono::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    Tamguchrono::idtype = global->Getid("chrono");

    Tamguchrono::AddMethod(global, "_initial", &Tamguchrono::MethodUnit, P_ONE | P_NONE, "unit(int i): 1 is second. 2 is milliseconds. 3 is microsecond. 4 is nanosecond.");
    Tamguchrono::AddMethod(global, "reset", &Tamguchrono::MethodReset, P_NONE, "reset(): reset the chrono");
    Tamguchrono::AddMethod(global, "unit", &Tamguchrono::MethodUnit, P_ONE, "unit(int i): 1 is second. 2 is milliseconds. 3 is microsecond. 4 is nanosecond.");

	if (version != "") {
		global->newInstance[Tamguchrono::idtype] = new Tamguchrono(global);
		global->RecordMethods(Tamguchrono::idtype, Tamguchrono::exported);

		Tamgu* a = new TamguSystemVariable(global, new TamguConstInt(1), global->Createid("c_second"), a_float);
		a = new TamguSystemVariable(global, new TamguConstInt(2), global->Createid("c_millisecond"), a_float);
		a = new TamguSystemVariable(global, new TamguConstInt(3), global->Createid("c_microsecond"), a_float);
		a = new TamguSystemVariable(global, new TamguConstInt(4), global->Createid("c_nanosecond"), a_float);
	}

	return true;
}

Tamgu* Tamguchrono::Put(Tamgu* index, Tamgu* val, short idthread) {
    if (val->Type() == Tamguchrono::idtype) {
        value = ((Tamguchrono*)val)->value;
        return aTRUE;
    }
    return aFALSE;
}

Tamgu* Tamguchrono::Eval(Tamgu* context, Tamgu* index, short idthread) {
    return this;
}

string Tamguchrono::String() {
    char buffer[100];
    double chronoval = value.time_since_epoch().count();
    sprintf_s(buffer, 100, "%fs", chronoval);
    return buffer;
}

Tamgu* Tamguchrono::MethodUnit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (callfunc->Size() == 0)
        return aFALSE;
    unit = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    return aTRUE;
}

Tamgu* Tamguchrono::MethodReset(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a chrono
    value = std::chrono::high_resolution_clock::now();
    return this;
}

Tamgu* Tamguchrono::minus(Tamgu* bb, bool autoself) {
    if (bb->Type() != idtype)
        return aNULL;
    
    Tamguchrono* b = (Tamguchrono*)bb;
    
    switch (unit) {
        case 1:
            return globalTamgu->Providefloat(std::chrono::duration_cast<std::chrono::seconds>( value - b->value ).count());
        case 2:
            return globalTamgu->Providefloat(std::chrono::duration_cast<std::chrono::milliseconds>( value - b->value ).count());
        case 3:
            return globalTamgu->Providefloat(std::chrono::duration_cast<std::chrono::microseconds>( value - b->value ).count());
        case 4:
            return globalTamgu->Providefloat(std::chrono::duration_cast<std::chrono::nanoseconds>( value - b->value ).count());
    }
    return globalTamgu->Providefloat(std::chrono::duration_cast<std::chrono::seconds>( value - b->value ).count());
}
