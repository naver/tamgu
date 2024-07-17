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
static ThreadLock classlock;

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgubyte::AddMethod(TamguGlobal* global, string name, byteMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(a_byte) != global->infomethods.end() &&
        global->infomethods[a_byte].find(name) != global->infomethods[a_byte].end())
        return;
    
    global->infomethods[a_byte][name] = infos;
    global->RecordArity(a_byte, idname, arity);
}

void Tamgubyte::Setidtype(TamguGlobal* global) {
  Locking lock(classlock);
  if (Tamgubyte::methods.isEmpty())
    Tamgubyte::InitialisationModule(global,"");
}


bool Tamgubyte::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    Tamgubyte::AddMethod(global, "succ", &Tamgubyte::MethodSucc, P_NONE, "succ(): Return the successor of a byte.");
    Tamgubyte::AddMethod(global, "pred", &Tamgubyte::MethodPred, P_NONE, "pred(): Return the predecessor of a byte.");
    

    if (version != "") {
        global->newInstance[a_byte] = new Tamgubyte(0, global);
        global->RecordCompatibilities(a_byte);
    }

    return true;
}


Tamgu* Tamgubyte::divide(Tamgu* a, bool autoself) {
    unsigned char ib = a->Byte();
    if (ib == 0)
        return globalTamgu->Returnerror(e_error_divided_by);
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
        return globalTamgu->Returnerror(e_error_divided_by);
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
