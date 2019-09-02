/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : liblinear.cxx
 Date       : 2017/09/01
 Purpose    : Encapsulation for Tamgu
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "linear.h"

#include "tamgu.h"
#include "tamguversion.h"
#include "liblinear.h"


static map<string, int> solvers;

//We need to declare once again our local definitions.
hmap<unsigned short, liblinearMethod>  Tamguliblinear::methods;
hmap<string, string> Tamguliblinear::infomethods;
bin_hash<unsigned long> Tamguliblinear::exported;

short Tamguliblinear::idtype = 0;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamguliblinear;
//This typedef defines a type "liblinearMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguliblinear::*liblinearMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

void print_null(const char *s) {}


Tamguliblinear::Tamguliblinear(TamguGlobal* g, Tamgu* parent) : TamguObject(g, parent) {
	//Do not forget your variable initialisation

	solvers["L2R_LR"] = 0;
	solvers["L2R_L2LOSS_SVC_DUAL"] = 1;
	solvers["L2R_L2LOSS_SVC"] = 2;
	solvers["L2R_L1LOSS_SVC_DUAL"] = 3;
	solvers["MCSVM_CS"] = 4;
	solvers["L1R_L2LOSS_SVC"] = 5;
	solvers["L1R_LR"] = 6;
	solvers["L2R_LR_DUAL"] = 7;
	solvers["L2R_L2LOSS_SVR = 11"] = 8;
	solvers["L2R_L2LOSS_SVR_DUAL"] = 9;
	solvers["L2R_L1LOSS_SVR_DUAL"] = 10;

	//Do not forget your variable initialisation
	RAZ();
}



//------------------------------------------------------------------------------------------------------------------
//If you want to implement an external library... Otherwise remove it...
extern "C" {
	Exporting bool linear_InitialisationModule(TamguGlobal* global, string version) {
		if (version != TamguVersion())
			return false;

		global->Update();

		return Tamguliblinear::InitialisationModule(global, version);
	}
}
//------------------------------------------------------------------------------------------------------------------

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguliblinear::AddMethod(TamguGlobal* global, string name, liblinearMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Tamguliblinear::InitialisationModule(TamguGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Tamguliblinear::idtype = global->Getid("liblinear");

	Tamguliblinear::AddMethod(global, "_initial", &Tamguliblinear::MethodInitial, P_TWO | P_THREE, "load(string inputdata,smap options,string outputfilename): load your training data with option. outputfilename is optional.");

	Tamguliblinear::AddMethod(global, "load", &Tamguliblinear::MethodLoad, P_TWO | P_THREE, "load(string inputdata,smap options,string outputfilename): load your training data with option. outputfilename is optional.");
	Tamguliblinear::AddMethod(global, "train", &Tamguliblinear::MethodLoad, P_TWO | P_THREE, "train(string inputdata,smap options,string outputfilename): train your training data with option. outputfilename is optional.");
	Tamguliblinear::AddMethod(global, "savemodel", &Tamguliblinear::MethodSave, P_ONE, "savemodel(string outputfilename): save your model in a file");
	Tamguliblinear::AddMethod(global, "options", &Tamguliblinear::MethodOptions, P_ONE, "options(smap actions): Set the training options");
	Tamguliblinear::AddMethod(global, "crossvalidation", &Tamguliblinear::MethodCrossvalidation, P_NONE, "crossvalidation(): Relaunch the cross validation with new parameters");
	Tamguliblinear::AddMethod(global, "cleandata", &Tamguliblinear::MethodClean, P_NONE, "cleandata(): clean internal data");
	Tamguliblinear::AddMethod(global, "trainingset", &Tamguliblinear::MethodTrainingSet, P_TWO, "trainingset(fvector labels,vector data): create your training set out of a iftreemap vector");
	Tamguliblinear::AddMethod(global, "loadmodel", &Tamguliblinear::MethodLoadModel, P_ONE, "loadmodel(string filename): Load your model");
	Tamguliblinear::AddMethod(global, "predictfromfile", &Tamguliblinear::MethodDoPredictFromFile, P_THREE, "predictfromfile(string input,bool flag_predict_probability,int infos): Predict from a file");
	Tamguliblinear::AddMethod(global, "predict", &Tamguliblinear::MethodDoPredict, P_THREE | P_FOUR, "predict(fvector labels,vector data,bool flag_predict_probability,int infos): Predict from a vector of iftreemap. labels is optional.");

	Tamguliblinear::AddMethod(global, "stloadproblems", &Tamguliblinear::MethodLoadproblems, P_ONE, "stloadproblems(svector files): Load and merge a list of training files (libshorttext).");
	Tamguliblinear::AddMethod(global, "staddproblems", &Tamguliblinear::MethodAddproblemvectors, P_TWO, "staddproblems(fvector labels,vector data): create your training set out of a iftreemap vector (libshorttext), which is merged with previous training sets.");
	Tamguliblinear::AddMethod(global, "stcomputeidf", &Tamguliblinear::MethodComputeidf, P_ONE, "stcomputeidf(fvector idfval): Compute the idv_val vector (libshorttext).");
	Tamguliblinear::AddMethod(global, "stnormalize", &Tamguliblinear::MethodNormalize, P_FIVE, "stnormalize(fvector idfval,bool binary, bool norm, bool tf, bool idf): Normalize the idv_val vector (libshorttext).");
	Tamguliblinear::AddMethod(global, "stanalyzeproblems", &Tamguliblinear::MethodAnalysisproblems, P_ONE, "stanalyzeproblems(bool training): Analyze the training files (libshorttext).");
	Tamguliblinear::AddMethod(global, "stclean", &Tamguliblinear::MethodCleaning, P_NONE, "stclean(): Cleaning internal structures (libshorttext).");

	global->newInstance[Tamguliblinear::idtype] = new Tamguliblinear(global);
	global->RecordMethods(Tamguliblinear::idtype,Tamguliblinear::exported);
	
	return true;
}


Tamgu* Tamguliblinear::Put(Tamgu* idx, Tamgu* kval, short idthread) {
	return aTRUE;
}

Tamgu* Tamguliblinear::Eval(Tamgu* context, Tamgu* idx, short idthread) {
	return this;
}


Tamgu* Tamguliblinear::parse_options(Tamgu* kmap) {
	void(*print_func)(const char*) = &print_null;	// default printing to stdout

	// default values
	if (!loaded) {
		param.solver_type = L2R_L2LOSS_SVC_DUAL;
		param.C = 1;
		param.eps = INF; // see setting below
		param.p = 0.1;
		param.nr_weight = 0;
		param.weight_label = NULL;
		param.weight = NULL;
		//param.ovo = 0;
		flag_cross_validation = 0;
		bias = -1;
	}

	string key;
	string val;
	string msg;

	TamguIteration* itr = kmap->Newiteration(false);
	for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
		key = itr->Keystring();
		val = itr->Valuestring();
		switch (key[0])
		{
		case 's':
			if (solvers.find(val) == solvers.end())
				param.solver_type = atoi(STR(val));
			else
				param.solver_type = solvers[val];
			break;
		case 'c':
			param.C = atof(STR(val));
			break;

		case 'p':
			param.p = atof(STR(val));
			break;

		case 'e':
			param.eps = atof(STR(val));
			break;

		case 'B':
			bias = atof(STR(val));
			break;

		case 'w':
			++param.nr_weight;
			param.weight_label = (INT64 *)realloc(param.weight_label, sizeof(int)*param.nr_weight);
			param.weight = (double *)realloc(param.weight, sizeof(double)*param.nr_weight);
			msg = key.substr(1, key.size() - 1);
			param.weight_label[param.nr_weight - 1] = atoi(STR(msg));
			param.weight[param.nr_weight - 1] = atoi(STR(val));
			break;

		case 'M':
			//param.ovo = atoi(STR(val));
			break;
		case 'v':
			flag_cross_validation = 1;
			nr_fold = atoi(STR(val));
			if (nr_fold < 2)
			{
				msg = "LLN(019): n-fold cross validation: n must >= 2";
				return exit_with_help(msg);
			}
			break;
		case 'q':
			print_func = &print_null;
			break;

		default:
			return exit_with_help(key);
		}
	}

	itr->Release();

	set_print_string_function(print_func);

	if (param.eps == INF)
	{
		switch (param.solver_type)
		{
		case L2R_LR:
		case L2R_L2LOSS_SVC:
			param.eps = 0.01;
			break;
		case L2R_L2LOSS_SVR:
			param.eps = 0.001;
			break;
		case L2R_L2LOSS_SVC_DUAL:
		case L2R_L1LOSS_SVC_DUAL:
		case MCSVM_CS:
		case L2R_LR_DUAL:
			param.eps = 0.1;
			break;
		case L1R_L2LOSS_SVC:
		case L1R_LR:
			break;
		case L2R_L1LOSS_SVR_DUAL:
		case L2R_L2LOSS_SVR_DUAL:
			param.eps = 0.1;
			break;
		}
	}
	return aTRUE;
}


Tamgu* Tamguliblinear::MethodComputeidf(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (svmproblems.size() == 0 || num_srcs == 0)
		return globalTamgu->Returnerror("LLN(044): No data", idthread);

	Tamgu* kvf = callfunc->Evaluate(0, contextualpattern, idthread);
	if (!kvf->isVectorContainer())
		return globalTamgu->Returnerror("LLN(043): Expecting a vector container", idthread);

	if (kvf->Type() == a_fvector) {
		for (int i = 0; i < svmproblems.size(); i++)
			compute_idf(&svmproblems[i].prob, ((Tamgufvector*)kvf)->values);
	}
	else {
		kvf->Clear();
		vector<double> v;
		for (int i = 0; i < svmproblems.size(); i++)
			compute_idf(&svmproblems[i].prob, v);
		for (int i = 0; i < v.size(); i++)
			kvf->Push(globalTamgu->Providefloat(v[i]));
	}
	return aTRUE;
}

Tamgu* Tamguliblinear::MethodNormalize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (svmproblems.size() || num_srcs == 0)
		return globalTamgu->Returnerror("LLN(044): No data", idthread);

	Tamgu* kvf = callfunc->Evaluate(0, contextualpattern, idthread);
	if (!kvf->isVectorContainer())
		return globalTamgu->Returnerror("LLN(043): Expecting a vector container", idthread);

	vector<double> v;
	if (kvf->Type() == a_fvector)
		v = ((Tamgufvector*)kvf)->values;
	else {
		for (int i = 0; i < v.size(); i++)
			v.push_back(kvf->getvalue(i)->Float());
	}

	int binary = callfunc->Evaluate(1, contextualpattern, idthread)->Boolean();
	int norm = callfunc->Evaluate(2, contextualpattern, idthread)->Boolean();
	int tf = callfunc->Evaluate(3, contextualpattern, idthread)->Boolean();
	int idf = callfunc->Evaluate(4, contextualpattern, idthread)->Boolean();

	for (int i = 0; i < svmproblems.size(); i++)
		normalize(&svmproblems[i].prob, binary, norm, tf, idf, v);

	if (kvf->Type() == a_fvector)
		((Tamgufvector*)kvf)->values = v;
	else {
		kvf->Clear();
		for (int i = 0; i < v.size(); i++)
			kvf->Push(globalTamgu->Providefloat(v[i]));
	}

	return aTRUE;
}

Tamgu* Tamguliblinear::MethodAddproblemvectors(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	Tamgu* klabels = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* kdata = callfunc->Evaluate(1, contextualpattern, idthread);
	if (klabels->Type() != a_fvector || kdata->Type() != a_vector)
		return globalTamgu->Returnerror("LLN(007): Wrong type for labels or data", idthread);

	if (klabels->Size() != kdata->Size())
		return globalTamgu->Returnerror("LLN(016): The two containers should have the same size", idthread);
	int err = load_vector_problems((Tamgufvector*)klabels, (Tamguvector*)kdata);
	if (err) {
		string message(errormessage);
		return globalTamgu->Returnerror(message, idthread);
	}
	return aTRUE;
}

Tamgu* Tamguliblinear::MethodLoadproblems(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	Tamgu* kvect = callfunc->Evaluate(0, contextualpattern, idthread);
	vector<string> sources;
	if (!kvect->isVectorContainer())
		return globalTamgu->Returnerror("LLN(047): Wrong type: expecting a vector", idthread);
	if (kvect->Type() == a_svector)
		sources = ((Tamgusvector*)kvect)->values;
	else {
		for (int i = 0; i < kvect->Size(); i++)
			sources.push_back(kvect->getvalue(i)->String());
	}
	int err = load_merge_problems(sources);
	if (err) {
		string msg = "LLN(049): ";
		msg += errormessage;
		return globalTamgu->Returnerror(msg, idthread);
	}
	return aTRUE;
}

Tamgu* Tamguliblinear::MethodAnalysisproblems(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (svmproblems.size() || num_srcs == 0)
		return globalTamgu->Returnerror("LLN(044): No data", idthread);

	bool training = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
	INT64 offsets[1000];
	Tamguvector* kvect = (Tamguvector*)Selectavector(contextualpattern);
	analysis_problems(offsets, kvect, training);
	return aTRUE;
}

Tamgu* Tamguliblinear::MethodCleaning(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (svmproblems.size() || num_srcs == 0)
		return globalTamgu->Returnerror("LLN(044): No data", idthread);

	for (int i = 0; i < num_srcs; i++)
		freeSVMProblem(svmproblems[i]);
	svmproblems.clear();
	num_srcs = 0;
	return aTRUE;
}

Tamgu* Tamguliblinear::MethodCrossvalidation(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (!loaded)
		return globalTamgu->Returnerror("LLN(008): Load your training data first", idthread);
	return do_cross_validation();
}

Tamgu* Tamguliblinear::MethodLoadModel(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (loaded)
		return globalTamgu->Returnerror("LLN(011): Clean your data first", idthread);

	flag_predict_probability = 0;
	model_file_name = callfunc->Evaluate(0, contextualpattern, idthread)->String();

	if ((model_ = load_model(STR(model_file_name))) == 0)
		return globalTamgu->Returnerror("LLN(012): can't open model file", idthread);
	loaded = true;
	return aTRUE;
}

Tamgu* Tamguliblinear::MethodLoad(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	infilename = callfunc->Evaluate(0, contextualpattern, idthread)->String();

	Tamgu* koptions = callfunc->Evaluate(1, contextualpattern, idthread);
	if (koptions->isContainer() == false)
		return globalTamgu->Returnerror("LLN(003): Options should be a map", idthread);

	model_file_name = "";
	if (callfunc->Size() == 3)
		model_file_name = callfunc->Evaluate(2, contextualpattern, idthread)->String();

	if (parse_options(koptions) == aRAISEERROR)
		return aRAISEERROR;

	//Loading the data
	if (read_problem(STR(infilename)) == aRAISEERROR)
		return aRAISEERROR;

	const char *error_msg = check_parameter(&prob, &param);

	string msgerr;
	if (error_msg)
	{
		msgerr = "LLN(005): ERROR: ";
		msgerr += error_msg;
		return globalTamgu->Returnerror(msgerr, idthread);
	}

	Tamgu* retour = aTRUE;
	model_ = train(&prob, &param);
	if (model_file_name != "") {
		if (save_model(STR(model_file_name), model_)) {
			msgerr = "LLN(004): can't save model to file ";
			msgerr += model_file_name;
			return globalTamgu->Returnerror(msgerr, idthread);
		}
	}

	loaded = true;
	return aTRUE;
}

Tamgu* Tamguliblinear::MethodSave(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (model_ == NULL || !loaded)
		return globalTamgu->Returnerror("LLN(021): No model to save", idthread);

	model_file_name = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	if (save_model(STR(model_file_name), model_)) {
		string msgerr = "LLN(004): can't save model to file ";
		msgerr += model_file_name;
		return globalTamgu->Returnerror(msgerr, idthread);
	}

	return aTRUE;
}

Tamgu* Tamguliblinear::MethodClean(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (!loaded)
		return aFALSE;
	CLEAN();
	RAZ();
	return aTRUE;
}

Tamgu* Tamguliblinear::MethodOptions(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	Tamgu* koptions = callfunc->Evaluate(0, contextualpattern, idthread);
	if (koptions->isContainer() == false)
		return globalTamgu->Returnerror("LLN(003): Options should be a map", idthread);
	if (parse_options(koptions) == aRAISEERROR)
		return aRAISEERROR;
	return aTRUE;
}

Tamgu* Tamguliblinear::MethodTrainingSet(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (loaded)
		return globalTamgu->Returnerror("LLN(011): Clean your data first", idthread);
	Tamgu* klabels = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* kdata = callfunc->Evaluate(1, contextualpattern, idthread);

	if (klabels->Type() != a_fvector || kdata->Type() != a_vector)
		return globalTamgu->Returnerror("LLN(007): Wrong type for labels or data", idthread);


	if (klabels->Size() != kdata->Size())
		return globalTamgu->Returnerror("LLN(016): The two containers should have the same size", idthread);

	return load_problem((Tamgufvector*)klabels, (Tamguvector*)kdata);
}

Tamgu* Tamguliblinear::MethodDoPredictFromFile(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (!loaded)
		return globalTamgu->Returnerror("LLN(018): Load your model first", idthread);

	string inputfile = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	{
		ifstream fl(STR(inputfile), openMode);
		if (fl.fail())
			return globalTamgu->Returnerror("LLN(014): Can't open input file", idthread);
	}

	FILE* input;
#ifdef WIN32
	fopen_s(&input, STR(inputfile), "r");
#else
	input = fopen(STR(inputfile), "r");
#endif

	if (input == NULL)
		return globalTamgu->Returnerror("LLN(014): Can't open input file", idthread);

	flag_predict_probability = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();

	x = (struct feature_node *) malloc(max_nr_attr*sizeof(struct feature_node));
	infos = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();

	Tamguvector* output = (Tamguvector*)Selectavector(contextualpattern);
	do_predict_with_file(input, output);
	fclose(input);
	free(x);
	x = NULL;
	return output;
}

Tamgu* Tamguliblinear::MethodDoPredict(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (!loaded)
		return globalTamgu->Returnerror("LLN(018): Load your model first", idthread);

	Tamgu* klabels = NULL;
	Tamgu* kdata = NULL;

	if (callfunc->Size() == 4) {
		klabels = callfunc->Evaluate(0, contextualpattern, idthread);
		kdata = callfunc->Evaluate(1, contextualpattern, idthread);
		flag_predict_probability = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
		infos = callfunc->Evaluate(3, contextualpattern, idthread)->Integer();
		if (klabels->Type() != a_fvector || kdata->Type() != a_vector)
			return globalTamgu->Returnerror("LLN(007): Wrong type for labels or data", idthread);
		if (klabels->Size() != kdata->Size())
			return globalTamgu->Returnerror("LLN(016): The two containers should have the same size", idthread);
	}
	else {
		kdata = callfunc->Evaluate(0, contextualpattern, idthread);
		flag_predict_probability = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
		infos = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
		if (kdata->Type() != a_vector)
			return globalTamgu->Returnerror("LLN(007): Wrong type for labels or data", idthread);
	}

	Tamguvector* output = (Tamguvector*)Selectavector(contextualpattern);

	x = (struct feature_node *) malloc(max_nr_attr*sizeof(struct feature_node));
	do_predict((Tamgufvector*)klabels, (Tamguvector*)kdata, output);
	free(x);
	x = NULL;
	return output;
}

