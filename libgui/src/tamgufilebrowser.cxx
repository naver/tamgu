/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgufilebrowser.cxx
 Date       : 2017/09/01
 Purpose    : File browser (based on FLTK)
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgufilebrowser.h"
#include "instructions.h"

static void filebrowser_callback(Fl_File_Chooser *fc, void* data) {
	Tamgufilebrowser* slid = (Tamgufilebrowser*)data;
	Tamgu* func = slid->function;
	TamguCallFunction kfunc(func);
	slid->Setreference();
	slid->object->Setreference();

	kfunc.arguments.push_back(slid);
	kfunc.arguments.push_back(slid->object);
	kfunc.Eval(aNULL, aNULL, globalTamgu->GetThreadid());
	slid->Resetreference();
	slid->object->Resetreference();
}


//We need to declare once again our local definitions.
basebin_hash<filebrowserMethod>  Tamgufilebrowser::methods;



short Tamgufilebrowser::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgufilebrowser::AddMethod(TamguGlobal* global, string name, filebrowserMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamgufilebrowser::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamgufilebrowser::InitialisationModule(global,"");
}

bool Tamgufilebrowser::InitialisationModule(TamguGlobal* global, string version) {
	methods.clear();
	
	

	Tamgufilebrowser::idtype = global->Getid("filebrowser");

	Tamgufilebrowser::AddMethod(global, "_initial", &Tamguwidget::MethodInitial, P_ONE, "_initial(variable): Creation of a window with a call back object");
	Tamgufilebrowser::AddMethod(global, "create", &Tamgufilebrowser::MethodCreate, P_FOUR, "create(string , intialdirectory, string filter, bool directory, string label): Open a file browser select directories if directory is true");
	Tamgufilebrowser::AddMethod(global, "value", &Tamgufilebrowser::MethodValue, P_NONE, "value(): Return the selected file");
	Tamgufilebrowser::AddMethod(global, "close", &Tamgufilebrowser::MethodClose, P_NONE, "close(): Close the file browser");
	Tamgufilebrowser::AddMethod(global, "ok", &Tamgufilebrowser::MethodOk, P_NONE, "ok(): return true if ok was pressed");


	global->newInstance[Tamgufilebrowser::idtype] = new Tamgufilebrowser(global);
	global->RecordCompatibilities(Tamgufilebrowser::idtype);

	return true;
}


Tamgu* Tamgufilebrowser::Put(Tamgu* index, Tamgu* value, short idthread) {
	return this;
}

Tamgu* Tamgufilebrowser::Eval(Tamgu* context, Tamgu* index, short idthread) {
	return this;
}

TamguIteration* Tamgufilebrowser::Newiteration(bool direction) {
	return aITERNULL;
}


Tamgu* Tamgufilebrowser::MethodCreate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	if (filebrowser != NULL)
		return Returningerror("WND(100): Cannot recreate an existing filebrowser", idthread);

	

	//0 is the first parameter and so on...
	string d = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	string f = callfunc->Evaluate(1, contextualpattern, idthread)->String();
	int t = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
	string label = callfunc->Evaluate(3, contextualpattern, idthread)->String();
	strcpy_s(txt, 1024, label.c_str());
	strcpy_s(directory, 1024, d.c_str());
	strcpy_s(filter, 1024, f.c_str());
	filebrowser = new Fl_File_Chooser(directory, filter, t, txt);
	//filebrowser->callback(filebrowser_callback,this);
	filebrowser->show();
	while (filebrowser->shown())
		Fl::wait();
	if (function != NULL) {
		filebrowser_callback(filebrowser, this);
		return aTRUE;
	}
	string buf;
	if (filebrowser->value() != NULL)
		buf = filebrowser->value();
	delete filebrowser;
	return globalTamgu->Providestring(buf);
}

Tamgu* Tamgufilebrowser::MethodValue(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	if (filebrowser == NULL)
		return Returningerror("WND(101): File browser not initialized", idthread);
	if (filebrowser->value() == NULL)
		return globalTamgu->Providestring("");
	string buf = filebrowser->value();
	return globalTamgu->Providestring(buf);
}

Tamgu* Tamgufilebrowser::MethodClose(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	if (filebrowser == NULL)
		return aFALSE;
	delete filebrowser;
	filebrowser = NULL;
	return aTRUE;
}

Tamgu* Tamgufilebrowser::MethodOk(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	if (filebrowser == NULL)
		return Returningerror("WND(101): File browser not initialized", idthread);
	if (filebrowser->shown())
		return aFALSE;
	return aTRUE;
}





