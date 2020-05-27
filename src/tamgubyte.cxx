/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgubyte.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgubyte.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<byteMethod>  Tamgubyte::methods;
Exporting hmap<string, string> Tamgubyte::infomethods;
Exporting basebin_hash<unsigned long> Tamgubyte::exported;

Exporting short Tamgubyte::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgubyte::AddMethod(TamguGlobal* global, string name, byteMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgubyte::Setidtype(TamguGlobal* global) {
    Tamgubyte::InitialisationModule(global,"");
}


   bool Tamgubyte::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    Tamgubyte::idtype = global->Getid("byte");

    Tamgubyte::AddMethod(global, "succ", &Tamgubyte::MethodSucc, P_NONE, "succ(): Return the successor of a byte.");
    Tamgubyte::AddMethod(global, "pred", &Tamgubyte::MethodPred, P_NONE, "pred(): Return the predecessor of a byte.");
    

    if (version != "") {
        global->newInstance[Tamgubyte::idtype] = new Tamgubyte(0, global);
        global->RecordMethods(Tamgubyte::idtype,Tamgubyte::exported);
    }

    return true;
}


Tamgu* Tamgubyte::divide(Tamgu* a, bool autoself) {
    unsigned char ib = a->Byte();
    if (ib == 0)
        return globalTamgu->Returnerror("Error: Divided by 0");
    if (autoself) {
        value /= ib;
        return this;
    }
    unsigned char i = value / ib;
    return new Tamgubyte(i);
}



Tamgu* Tamgubyte::mod(Tamgu* a, bool autoself) {
    unsigned char ib = a->Byte();
    if (ib == 0)
        return globalTamgu->Returnerror("Error: Divided by 0");
    if (autoself) {
        value %= ib;
        return this;
    }
    unsigned char i = value % ib;
    return new Tamgubyte(i);
}

Tamgu* Tamgubyte::power(Tamgu* a, bool autoself) {
    if (autoself) {
        value = pow(Float(), a->Float());
        return this;
    }
    double i = pow(Float(), a->Float());
    return new Tamgubyte(i);
}
