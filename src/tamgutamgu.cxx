/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgutamgu.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgutamgu.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<tamguMethod>  Tamgutamgu::methods;
Exporting hmap<string, string> Tamgutamgu::infomethods;
Exporting bin_hash<unsigned long> Tamgutamgu::exported;

Exporting hmap<string, Tamgutamgu*> Tamgutamgu::recorded;

Tamgutamgu::Tamgutamgu(TamguGlobal* g, Tamgu* parent) : TamguObject(g, parent) {
    //Do not forget your variable initialisation
    acode = NULL;
}

Tamgutamgu::Tamgutamgu(string fn, TamguCode* a, TamguGlobal* g) : TamguObject(g) {
    //This is usually called only for the main program... See TamguRecordFile for the call
    filename = fn;
    acode = a;
    Tamgutamgu::recorded[filename] = this;
}

Tamgu* TamguRecordedFiles(string filename) {
    filename = NormalizeFileName(filename);
    if (Tamgutamgu::recorded.find(filename) != Tamgutamgu::recorded.end())
        return Tamgutamgu::recorded[filename];

    return NULL;
}

Tamgu* TamguRecordFile(string filename, TamguCode* a, TamguGlobal* g) {
    filename = NormalizeFileName(filename);
    if (Tamgutamgu::recorded.find(filename) != Tamgutamgu::recorded.end())
        return Tamgutamgu::recorded[filename];

    return new Tamgutamgu(filename, a, g);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgutamgu::AddMethod(TamguGlobal* global, string name, tamguMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Tamgutamgu::InitialisationModule(TamguGlobal* global, string version) {
    
    methods.clear();
    infomethods.clear();
    exported.clear();
    recorded.clear();


    Tamgutamgu::AddMethod(global, "method", &Tamgutamgu::MethodMethod, P_FULL, "method(...): Execute a method within the local space.");
    Tamgutamgu::AddMethod(global, "methods", &Tamgutamgu::MethodMethods, P_NONE, "methods(): Return the list of methods in the local space.");


    global->newInstance[a_tamgu] = new Tamgutamgu(global);
    global->RecordMethods(a_tamgu, Tamgutamgu::exported);

    return true;
}

Tamgu* Tamgutamgu::Put(Tamgu* index, Tamgu* value, short idthread) {
    filename = value->String();
    return Load();
}

Tamgu* Tamgutamgu::Eval(Tamgu* context, Tamgu* index, short idthread) {
    return this;
}


Tamgu* Tamgutamgu::Load() {
    filename = NormalizeFileName(filename);
    //Important, we then store this object into recorded...
    recorded[filename] = this;
    acode = globalTamgu->Loadfile(filename);
    if (acode==NULL) {
        stringstream msg;
        msg << "Unknown file: "<<filename;
        return globalTamgu->Returnerror(msg.str());
    }
    
    TamguFrame* frame = (TamguFrame*)acode->Mainframe();
    Tamgu* res;
    //We then initialize the different variables stored in this file...
    //No function are actually executed here, except within an initialisation
    for (size_t i = 0; i < frame->instructions.size(); i++) {
        if (frame->instructions[i]->isVariable()) {
            res = frame->instructions[i]->Eval(frame, aNULL, 0);
            if (res->isError())
                return res;
        }
    }
    return this;
}
