/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguimage.cxx
 Date       : 2017/09/01
 Purpose    : Image display (based on FLTK)
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgugui.h"
#include "tamguimage.h"

//We need to declare once again our local definitions.
basebin_hash<imageMethod>  Tamguimage::methods;

short Tamguimage::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguimage::AddMethod(TamguGlobal* global, string name, imageMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamguimage::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamguimage::InitialisationModule(global,"");
}

bool Tamguimage::InitialisationModule(TamguGlobal* global, string version) {
	methods.clear();
	
	

	Tamguimage::idtype = global->Getid("image");

	Tamguimage::AddMethod(global, "loadgif", &Tamguimage::MethodLoadImageGIF, P_ONE, "loadgif(string filename): Load a GIF image");

#ifndef FLTKNOJPEG
	Tamguimage::AddMethod(global, "loadjpeg", &Tamguimage::MethodLoadImageJPEG, P_ONE, "loadjpeg(string filename): Load a JPEG image");
#endif



	global->newInstance[Tamguimage::idtype] = new Tamguimage(global);
	global->RecordCompatibilities(Tamguimage::idtype);

	return true;
}


Tamgu* Tamguimage::Put(Tamgu* index, Tamgu* value, short idthread) {
	return this;
}

Tamgu* Tamguimage::Eval(Tamgu* context, Tamgu* index, short idthread) {
	return this;
}

TamguIteration* Tamguimage:: Newiteration(bool direction) {
	return aITERNULL;
}

