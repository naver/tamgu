#include "tamgu.h"
#include "tamguversion.h"
#include "tamguwapiti.h"
#include "tamgustring.h"

#ifndef WIN32
#define _strdup strdup
#endif

//We need to declare once again our local definitions.
hmap<unsigned short, wapitiMethod>  Tamguwapiti::methods;
hmap<string, string> Tamguwapiti::infomethods;
basebin_hash<unsigned long> Tamguwapiti::exported;

short Tamguwapiti::idtype = 0;

//------------------------------------------------------------------------------------------------------------------
//If you need to implement an external library... Otherwise remove it...
//When Tamgu (탐구) loads a library, it looks for this method in particular, which then make this object available into Tamgu (탐구)
extern "C" {
    Exporting bool wapiti_InitialisationModule(TamguGlobal* global, string version) {
        if (version != TamguVersion())
            return false;

        global->Update();

        return Tamguwapiti::InitialisationModule(global, version);
    }
}
//------------------------------------------------------------------------------------------------------------------

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguwapiti::AddMethod(TamguGlobal* global, string name, wapitiMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Tamguwapiti::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    //Each new object has a specific name, which will help recognize it in the code that will exploit wapiti...
    Tamguwapiti::idtype = global->Getid("wapiti");

    //You declare your methods here:
    // Argument 1 is a call to global, which will record your method into Tamgu (탐구)
    // Argument 2 is the name of your command
    // Argument 3 links your command with its implementation into the object class
    // Argument 4 is a combination of P_... which defines how many parameters this function can accept.
    //Argument 5 is a description of this method.
	Tamguwapiti::AddMethod(global, "_initial", &Tamguwapiti::MethodLoadModel, P_TWO | P_ONE, "_initial(string crfmodel, vector options): Loading a CRF model.");
	Tamguwapiti::AddMethod(global, "loadmodel", &Tamguwapiti::MethodLoadModel, P_TWO | P_ONE, "loadmodel(string crfmodel, vector options): Loading a CRF model.");
	Tamguwapiti::AddMethod(global, "options", &Tamguwapiti::MethodOptions, P_ONE, "options(map options): Setting options.");
	Tamguwapiti::AddMethod(global, "train", &Tamguwapiti::MethodTraining, P_NONE, "train(): Launch training.");
	Tamguwapiti::AddMethod(global, "label", &Tamguwapiti::MethodLabelling, P_ONE, "label(vector words): Launch labelling.");
	Tamguwapiti::AddMethod(global, "lasterror", &Tamguwapiti::MethodLasterror, P_NONE, "lasterror(): Return the last error.");

    //We need this code, in order to create new instances of our wapiti object... DO NOT ALTER
    global->newInstance[Tamguwapiti::idtype] = new Tamguwapiti(global);
    global->RecordMethods(Tamguwapiti::idtype,Tamguwapiti::exported);
    
    return true;
}



Tamgu* Tamguwapiti::MethodLoadModel(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (loadingmode)
		return globalTamgu->Returnerror("WPT(004): Model already loaded", idthread);
	string model = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	for (int i = 0; i < 100; i++)
		args[i][0] = 0;

	strcpy_s(args[0], 100, "wapiti");
	strcpy_s(args[1], 100, "label");
	strcpy_s(args[2], 100, "-m");
	strcpy_s(args[3], 100, STR(model));
    long nb = 4;
    if (callfunc->Size()==2) {
        Tamgu* vopts = callfunc->Evaluate(1, contextualpattern, idthread);
        if (!vopts->isVectorContainer())
            globalTamgu->Returnerror("options should be a vector of strings");
        
        string opt;
        for (long o = 0; o < vopts->Size(); o++) {
            opt = vopts->getstring(o);
            strcpy_s(args[nb++], 100, STR(opt));
        }
    }
    
	initopt(opt);
	opt_parse(nb, args, &opt);
	char* erreur = CRFError();
	if (erreur[0] != 0) {
		model = "WPT(099): ";
		model += erreur;
		return globalTamgu->Returnerror(model, idthread);
	}

	loadingmode = 1;
	return aTRUE;
}

Tamgu* Tamguwapiti::MethodOptions(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//if (loadingmode)
		//return globalTamgu->Returnerror("WPT(004): Model already loaded", idthread);

	Tamgu* koptions = callfunc->Evaluate(0, contextualpattern, idthread);
	if (!koptions->isVectorContainer())
		return globalTamgu->Returnerror("WPT(014): Expecting a vector container", idthread);

	int i;
	for (i = 0; i < 100; i++)
		args[i][0] = 0;

	TamguIteration* itr = koptions->Newiteration(false);
	string s;
	i = 0;
	strcpy_s(args[i++], 100, "wapiti");
	for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
		s = itr->Valuestring();
		strcpy_s(args[i++], 100, STR(s));
	}
	itr->Release();

	initopt(opt);
	opt_parse(i, args, &opt);
	char* erreur = CRFError();
	if (erreur[0] != 0) {
		string model = "WPT(099): ";
		model += erreur;
		return globalTamgu->Returnerror(model, idthread);
	}
	loadingmode = 1;
	return aTRUE;
}

Tamgu* Tamguwapiti::MethodTraining(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (!loadingmode || opt.mode != 0)
		return globalTamgu->Returnerror("WPT(021): No options provided", idthread);
	DoTrain(&opt);
	return aTRUE;
}

Tamgu* Tamguwapiti::MethodLasterror(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	string s = WAPITILastError();
	return globalTamgu->Providestring(s);
}

Tamgu* Tamguwapiti::MethodLabelling(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (!loadingmode || opt.mode == 0)
		return globalTamgu->Returnerror("WPT(022): No options provided", idthread);

	WAPITIInit();

	Tamgu* kwords = callfunc->Evaluate(0, contextualpattern, idthread);
	if (!kwords->isVectorContainer())
		return globalTamgu->Returnerror("WPT(015): Expecting a vector container", idthread);

	int sz = kwords->Size();
	if (sz == 0)
		return aFALSE;

	if (crftagger == NULL) {
		crftagger = CreatePureTagger(&opt);
		loadingmode = 2;
		if (!InitPureTagger(crftagger))
			return aFALSE;
	}

	char** wordSequence = new char*[sz + 1];
	char** predictedTag = new char*[sz + 1];
	vector<string> wrds;
	int i;
	for (i = 0; i < sz; i++) {
		wrds.push_back(kwords->getstring(i));
		wordSequence[i] = _strdup(STR(wrds[i]));
		predictedTag[i] = NULL;
	}

	if (!LabelPureTagger(crftagger, wordSequence, predictedTag, sz))
		return aFALSE;

	kwords = Selectasvector(contextualpattern);
	string v;
	for (i = 0; i < sz; i++)
		kwords->storevalue(predictedTag[i]);

	delete[] wordSequence;
	delete[] predictedTag;
	return kwords;
}


