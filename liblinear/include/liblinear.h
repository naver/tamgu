/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : liblinear.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamguliblinear_h
#define tamguliblinear_h


#include "tamguvector.h"
#include "tamguivector.h"
#include "tamgufvector.h"
#include "tamgutreemapif.h"
#include "tamgumapif.h"
#include "tamgutreemapi.h"
#include "tamgutreemap.h"
#include "tamgumapsf.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamguliblinear;
//This typedef defines a type "liblinearMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguliblinear::*liblinearMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------
#define Malloc(type,n) (type *)malloc((n)*sizeof(type))
#define INF HUGE_VAL
struct SVMProblem {
	struct problem prob;
	struct feature_node* x_space;
	INT64 len_x_space;

	SVMProblem() {}

};

//---------------------------------------------------------------------------------------------------------------------

class Tamguliblinear : public TamguObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static hmap<unsigned short, liblinearMethod> methods;
    static hmap<string, string> infomethods;
    static bin_hash<unsigned long> exported;

    static short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
	struct feature_node *x_space;
	struct parameter param;
	struct problem prob;
	struct model* model_;
	INT64 flag_cross_validation;
	INT64 nr_fold;
	double bias;
	char *line;
	INT64 max_line_len;
	string infilename;
	char errormessage[1024];
	string model_file_name;
	bool loaded;
	INT64 flag_predict_probability;
	struct feature_node *x;
	INT64 max_nr_attr;
	int infos;
	vector<SVMProblem> svmproblems;
	int num_srcs;
	//---------------------------------------------------------------------------------------------------------------------
	void RAZ() {
		num_srcs = 0;
		svmproblems.clear();
		infos = 1;
		line = NULL;
		x_space = NULL;
		model_ = NULL;
		loaded = false;
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
		max_nr_attr = 64;
		flag_predict_probability = 0;
		x = NULL;
		prob.y = NULL;
		prob.x = NULL;
	}


	void CLEAN() {
		if (model_ != NULL)
			free_and_destroy_model(&model_);

		model_ = NULL;
		if (x_space != NULL) {
			destroy_param(&param);
			free(x_space);
		}

		if (prob.y != NULL)
			free(prob.y);
		if (prob.x != NULL)
			free(prob.x);

		prob.y = NULL;
		prob.x = NULL;
		x_space = NULL;
		if (line != NULL)
			free(line);

		line = NULL;

		if (x != NULL)
			free(x);
		x = NULL;
	}

    //---------------------------------------------------------------------------------------------------------------------
	Tamguliblinear(TamguGlobal* g, Tamgu* parent = NULL);

	~Tamguliblinear() {
		CLEAN();
	}

    //----------------------------------------------------------------------------------------------------------------------
	Tamgu* Put(Tamgu* v, Tamgu* i, short idthread);
    
	Tamgu* Get(Tamgu* context, Tamgu* v, short idthread);


    short Type() {
        return Tamguliblinear::idtype;
    }

    string Typename() {
        return "liblinear";
    }

	//The next two functions work in conjunction...
	//If your object is very atomic (such as a number or a string)
	//Then when it is passed to a function or stored into a container,
	//it might be worth duplicating it.
	// In that case: needNewInstance should return true...
	
	//However if your object is complex and probably unique through out the code
	//The needNewInstance should return false, and Atom should always reduce to a "return this;" only...
    Tamgu* Atom(bool forced=false) {
        if (forced || !protect)
            return new  Tamguliblinear(NULL);
        return this;
    }

	bool duplicateForCall() {
		return false;
	}

    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        if (exported.find(n) != exported.end())
            return true;
        return false;
    }

	Tamgu* Newinstance(short, Tamgu* f = NULL) {
		return new Tamguliblinear(NULL);
    }

	TamguIteration* Newiteration(bool direction) {
		return aITERNULL;
	}

	static void AddMethod(TamguGlobal* g, string name, liblinearMethod func, unsigned long arity, string infos);
	static bool InitialisationModule(TamguGlobal* g, string version);


	void Methods(Tamgu* v) {
		hmap<string, string>::iterator it;
		for (it = infomethods.begin(); it != infomethods.end(); it++)
			v->storevalue(it->first);
	}

	string Info(string n) {

		if (infomethods.find(n) != infomethods.end())
			return infomethods[n];
		return "Unknown method";
	}


	//---------------------------------------------------------------------------------------------------------------------
	//This section implements the libshortext specifics.

	void freeSVMProblem(SVMProblem svmprob) {
		struct problem *prob = &(svmprob.prob);
		if (prob->x != NULL) free(prob->x);
		if (prob->y != NULL) free(prob->y);
		if (svmprob.x_space != NULL) free(svmprob.x_space);
	}

	void compute_idf(struct problem *prob, vector<double>& idf_val)
	{
		INT64 i, j;
		//double* idf_val = Malloc(double, prob->n);
		//memset(idf_val, 0, sizeof(double) * prob->n);
		idf_val.reserve(prob->n + 1);
		for (i = 0; i < prob->l; ++i)
			idf_val.push_back(0);

		for (i = 0; i < prob->l; ++i)
		{
			struct feature_node* xi = prob->x[i];
			while (xi->index != -1)
			{
				++idf_val[xi->index - 1];
				++xi;
			}
		}

		for (j = 0; j < prob->n; ++j)
		{
			if (idf_val[j] > 0)
				idf_val[j] = log(prob->l / idf_val[j]);
			else
				idf_val[j] = 0;
		}
	}

	void normalize(struct problem *prob, int binary, int norm, int tf, int idf, vector<double>& idf_val)
	{
		INT64 i;

		for (i = 0; i < prob->l; ++i)
		{
			struct feature_node* xi;

			if (binary)
			{
				xi = prob->x[i];
				while (xi->index != -1)
				{
					xi->value = xi->value != 0;
					++xi;
				}
			}

			if (tf)
			{
				double norm = 0;
				xi = prob->x[i];
				while (xi->index != -1)
				{
					norm += xi->value;
					++xi;
				}

				xi = prob->x[i];
				if (norm != 0)
				while (xi->index != -1)
				{
					xi->value /= norm;
					++xi;
				}
			}

			if (idf)
			{
				xi = prob->x[i];
				while (xi->index != -1)
				{
					xi->value *= idf_val[xi->index - 1];
					++xi;
				}
			}

			if (norm)
			{
				double norm = 0;
				xi = prob->x[i];
				while (xi->index != -1)
				{
					norm += xi->value * xi->value;
					++xi;
				}

				norm = sqrt(norm);

				xi = prob->x[i];
				if (norm != 0)
				while (xi->index != -1)
				{
					xi->value /= norm;
					++xi;
				}
			}
		}
	}

	SVMProblem load_short_problem(Tamgufvector* labels, Tamguvector* data, int& error_code) {
		INT64 max_index, inst_max_index, i;
		INT64 elements, j;
		struct problem xprob;
		SVMProblem svmprob;

		/**
		* error_code:
		* 0	no error
		* > 0	input format error. The error_code value
		* 	indicates the line number.
		* -1	can not open file
		* -2	memory exhausted
		*/
		error_code = 0;


		xprob.l = 0;
		elements = 0;
		for (j = 0; j < data->Size(); j++)
			elements += data->values[j]->Size();
		xprob.l = labels->Size();


		xprob.bias = bias;
		if (xprob.bias >= 0) elements += xprob.l;

		errno = 0;
		xprob.y = Malloc(double, xprob.l);
		xprob.x = Malloc(struct feature_node *, xprob.l);
		struct feature_node* x_space = Malloc(struct feature_node, elements + xprob.l);

		if (errno == ENOMEM)
		{
			error_code = -2;
			return svmprob;
		}

		Tamgutreemapif* ktmif;
		Tamgutreemapif mapbase;
		Tamgu* mymap;

		max_index = 0;
		j = 0;
		for (i = 0; i < xprob.l; i++)
		{
			inst_max_index = 0; // strtol gives 0 if wrong format
			xprob.x[i] = &x_space[j];
			xprob.y[i] = labels->values[i];

			mymap = data->values[i];

			if (mymap->Type() != Tamgutreemapif::idtype) {
				if (!mymap->isContainer()) {
					sprintf_s(errormessage, 1024, "LLN(011): Wrong map type");
					error_code = -100;
					return svmprob;
				}

				mapbase.values.clear();
				TamguIteration* itr = mymap->Newiteration(false);
				for (itr->Begin(); itr->End() == aFALSE; itr->Next())
					mapbase.values[itr->Keyinteger()] = itr->Valuefloat();
				itr->Release();
				ktmif = &mapbase;
			}
			else
				ktmif = (Tamgutreemapif*)mymap;

			map<long, double>::iterator it;
			for (it = ktmif->values.begin(); it != ktmif->values.end(); it++) {

				errno = 0;
				x_space[j].index = (int)it->first;
				if (x_space[j].index <= inst_max_index)
				{
					error_code = i + 1;
					return svmprob;
				}
				else
					inst_max_index = x_space[j].index;

				errno = 0;
				x_space[j].value = it->second;
				++j;
			}

			if (inst_max_index > max_index)
				max_index = inst_max_index;

			if (xprob.bias >= 0)
				x_space[j++].value = xprob.bias;

			x_space[j++].index = -1;
		}

		if (xprob.bias >= 0)
		{
			xprob.n = max_index + 1;
			for (i = 1; i < xprob.l; i++)
				(xprob.x[i] - 2)->index = xprob.n;
			x_space[j - 2].index = xprob.n;
		}
		else
			xprob.n = max_index;

		svmprob.prob = xprob;
		svmprob.x_space = x_space;
		svmprob.len_x_space = j;

		return svmprob;
	}

	// read in a problem (in libsvm format)
	SVMProblem read_short_problem(const char *filename, double bias, INT64 *error_code)
	{
		INT64 max_index, inst_max_index, i;
		INT64 elements, j;
		FILE* fp;

#ifdef WIN32
		fopen_s(&fp, filename, "r");
#else
		fp = fopen(filename, "r");
#endif
		char *endptr;
		char *idx, *val, *label;
		struct problem xprob;
		SVMProblem svmprob;

		/**
		* error_code:
		* 0	no error
		* > 0	input format error. The error_code value
		* 	indicates the line number.
		* -1	can not open file
		* -2	memory exhausted
		*/
		*error_code = 0;

		if (fp == NULL)
		{
			*error_code = -1;
			return svmprob;
		}

		xprob.l = 0;
		elements = 0;
		max_line_len = 1024;
		line = Malloc(char, max_line_len);
		while (readline(fp) != NULL)
		{
#ifdef WIN32
			char* next_p = NULL;
			char *p = strtok_s(line, " \t", &next_p); // label
#else
			char *p = strtok(line, " \t"); // label
#endif

			// features
			while (1)
			{
#ifdef WIN32
				p = strtok_s(NULL, " \t", &next_p); 
#else
				p = strtok(NULL, " \t");
#endif
				if (p == NULL || *p == '\n') // check '\n' as ' ' may be after the last feature
					break;
				elements++;
			}
			xprob.l++;
		}
		rewind(fp);

		xprob.bias = bias;
		if (xprob.bias >= 0) elements += xprob.l;

		errno = 0;
		xprob.y = Malloc(double, xprob.l);
		xprob.x = Malloc(struct feature_node *, xprob.l);
		struct feature_node* x_space = Malloc(struct feature_node, elements + xprob.l);

		if (errno == ENOMEM)
		{
			free(line);
			fclose(fp);
			*error_code = -2;
			return svmprob;
		}

		max_index = 0;
		j = 0;
		char* next_p = NULL;
		for (i = 0; i < xprob.l; i++)
		{
			inst_max_index = 0; // strtol gives 0 if wrong format
			readline(fp);
			xprob.x[i] = &x_space[j];
#ifdef WIN32
			label = strtok_s(line, " \t\n", &next_p);
#else
			label = strtok(line, " \t\n");
#endif
			if (label == NULL) // empty line
			{
				free(line);
				fclose(fp);
				*error_code = i + 1;
				return svmprob;
			}

			xprob.y[i] = strtod(label, &endptr);
			if (endptr == label || *endptr != '\0')
			{
				free(line);
				fclose(fp);
				*error_code = i + 1;
				return svmprob;
			}

			while (1)
			{
#ifdef WIN32
				idx = strtok_s(NULL, ":", &next_p);
				val = strtok_s(NULL, " \t", &next_p);
#else
				idx = strtok(NULL, ":");
				val = strtok(NULL, " \t");
#endif
				if (val == NULL)
					break;

				errno = 0;
#ifdef WIN32
				x_space[j].index = (INT64)strtol(idx, &endptr, 10);
#else
				x_space[j].index = (INT64)strtoll(idx, &endptr, 10);
#endif
				if (endptr == idx || errno != 0 || *endptr != '\0' || x_space[j].index <= inst_max_index)
				{
					free(line);
					fclose(fp);
					*error_code = i + 1;
					return svmprob;
				}
				else
					inst_max_index = x_space[j].index;

				errno = 0;
				x_space[j].value = strtod(val, &endptr);
				//if(binary) x_space[j].value = x_space[j].value != 0;
				if (endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
				{
					free(line);
					fclose(fp);
					*error_code = i + 1;
					return svmprob;
				}

				++j;
			}

			if (inst_max_index > max_index)
				max_index = inst_max_index;

			if (xprob.bias >= 0)
				x_space[j++].value = xprob.bias;

			x_space[j++].index = -1;
		}

		if (xprob.bias >= 0)
		{
			xprob.n = max_index + 1;
			for (i = 1; i < xprob.l; i++)
				(xprob.x[i] - 2)->index = xprob.n;
			x_space[j - 2].index = xprob.n;
		}
		else
			xprob.n = max_index;

		fclose(fp);
		free(line);

		svmprob.prob = xprob;
		svmprob.x_space = x_space;
		svmprob.len_x_space = j;

		return svmprob;
	}

	int load_vector_problems(Tamgufvector* labels, Tamguvector* data) {
		int error_code = 0;
		SVMProblem svmp = load_short_problem(labels, data, error_code);

		if (error_code != 0) {
			switch (error_code) {
			case -2:
				sprintf_s(errormessage, 1024, "ERROR: Memory exhausted when reading");
				break;
			default: /* error_code  > 0 input format error*/
				sprintf_s(errormessage, 1024, "ERROR: input format error at line %ld", (long)error_code);
				break;
			}
			freeSVMProblem(svmp);
			return -1;
		}

		num_srcs++;
		svmproblems.push_back(svmp);
		// Make sure # of instances are all equal.
		for (int i = 1; i < svmproblems.size(); i++)
		{
			if (svmproblems[i].prob.l != svmproblems[i - 1].prob.l)
			{
				sprintf_s(errormessage, 1024, "ERROR: #insts = %ld, but #insts in previous = %ld\n",
					(long)svmproblems[i].prob.l, (long)svmproblems[i - 1].prob.l);
				return 3;
			}
		}

		return 0;
	}

	int load_merge_problems(vector<string>& sources) {
		int i;
		const double bias = -1;
		num_srcs = sources.size();
		INT64 error_code = 0;

		/**
		* error_code:
		* 0	no error
		* > 0	input format error. The error_code value
		* 	indicates the line number.
		* -1	can not open file
		* -2	memory exhausted
		* -3	input files contain different numbsers of instances
		* -4   no file given
		*/

		if (num_srcs <= 0)
			return -4;

		svmproblems.clear();

		for (i = 0; i < num_srcs; i++)
		{
			char* srcs = STR(sources[i]);
			svmproblems.push_back(read_short_problem(srcs, bias, &error_code));
			if (error_code != 0) {
				switch (error_code) {
				case -1:
					sprintf_s(errormessage, 1024, "ERROR: Cannot open input file: %s\n", srcs);
					break;
				case -2:
					sprintf_s(errormessage, 1024, "ERROR: Memory exhausted when reading %s\n", srcs);
					break;
				default: /* error_code  > 0 input format error*/
					sprintf_s(errormessage, 1024, "ERROR: input format error at line %ld in %s\n", (long)error_code, srcs);
					break;
				}

				for (int j = 0; j < i; j++)
					freeSVMProblem(svmproblems[j]);

				svmproblems.clear();
				num_srcs = 0;
				return error_code;
			}
		}

		// Make sure # of instances are all equal.
		for (i = 1; i < num_srcs; i++)
		{
			if (svmproblems[i].prob.l != svmproblems[i - 1].prob.l)
			{
				sprintf_s(errormessage, 1024, "ERROR: #insts in %s = %ld, but #insts in %s = %ld\n",
					STR(sources[i]), (long)svmproblems[i].prob.l, STR(sources[i - 1]), (long)svmproblems[i - 1].prob.l);
				return 3;
			}
		}

		return error_code;
	}

	int analysis_problems(INT64* offsets, Tamguvector* kvect, bool training) {
		int i, j;
		INT64 error_code = 0;

		/**
		* error_code:
		* 0	no error
		* > 0	input format error. The error_code value
		* 	indicates the line number.
		* -1	can not open file
		* -2	memory exhausted
		* -3	input files contain different numbsers of instances
		* -4   no file given
		*/


		// Overwrite offsets
		if (training) {
			offsets[0] = svmproblems[0].prob.n;
			for (i = 1; i < num_srcs; i++)
				offsets[i] = offsets[i - 1] + svmproblems[i].prob.n;
		}


		for (j = 0; j < svmproblems[0].prob.l; j++)
		{
			INT64 base = 0;
			Tamguvector* kvi = globalTamgu->Providevector();
			kvi->values.push_back(globalTamgu->Providefloat(svmproblems[0].prob.y[j]));
			Tamgumapif* kval = new Tamgumapif;
			kvi->Push(kval);
			kvect->Push(kvi);
			for (i = 0; i < num_srcs; i++)
			{
				struct feature_node* node;

				for (node = svmproblems[i].prob.x[j]; node->index != -1; node++)
				{
					INT64 index = base + node->index;
					if (index <= offsets[i])
						kval->values[index] = node->value;
					else
						break;
				}
				base = offsets[i];
			}
		}
		return 0;
	}

	Tamgu* do_predict_with_file(FILE *input, Tamguvector* output) {

		Tamgutreemap* kinfo = NULL;
		if (infos) {
			kinfo = new Tamgutreemap;
			kinfo->Setreference(output->reference + 1);
			output->values.push_back(kinfo);
		}


		INT64 correct = 0;
		INT64 total = 0;
		double error = 0;
		double sump = 0, sumt = 0, sumpp = 0, sumtt = 0, sumpt = 0;

		INT64 nr_class = get_nr_class(model_);
		double *prob_estimates = NULL;
		INT64 j, n;
		INT64 nr_feature = get_nr_feature(model_);
		if (model_->bias >= 0)
			n = nr_feature + 1;
		else
			n = nr_feature;

		Tamguivector* kindexlabels = NULL;
		if (flag_predict_probability) {
			INT64 *labels;

			if (!check_probability_model(model_))
				return globalTamgu->Returnerror("LLN(013): probability output is only supported for logistic regression", globalTamgu->GetThreadid());

			labels = (INT64 *)malloc(nr_class*sizeof(INT64));
			get_labels(model_, labels);
			prob_estimates = (double *)malloc(nr_class*sizeof(double));
			if (infos) {
				kindexlabels = globalTamgu->Provideivector();
				kindexlabels->Setreference(kinfo->reference + 1);
				kinfo->values["labels"] = kindexlabels;
				for (j = 0; j < nr_class; j++)
					kindexlabels->values.push_back(labels[j]);
			}
			free(labels);
		}

		char buff[20];

		max_line_len = 1024;
		line = (char *)malloc(max_line_len*sizeof(char));
		INT64 ix = 0;
		char* next_p = NULL;
		while (readline(input) != NULL)
		{
			INT64 i = 0;
			double target_label, predict_label;
			char *idx, *val, *label, *endptr;
			INT64 inst_max_index = 0; // strtol gives 0 if wrong format

#ifdef WIN32
			label = strtok_s(line, " \t\n", &next_p);
#else
			label = strtok(line, " \t\n");
#endif
			if (label == NULL) // empty line
				exit_input_error(total + 1);

			target_label = strtod(label, &endptr);
			if (endptr == label || *endptr != '\0')
				exit_input_error(total + 1);

			while (1)
			{
				if (i >= max_nr_attr - 2)	// need one more for index = -1
				{
					max_nr_attr *= 2;
					x = (struct feature_node *) realloc(x, max_nr_attr*sizeof(struct feature_node));
				}

#ifdef WIN32
				idx = strtok_s(NULL, ":", &next_p);
				val = strtok_s(NULL, " \t", &next_p);
#else
				idx = strtok(NULL, ":");
				val = strtok(NULL, " \t");
#endif;

				if (val == NULL)
					break;
				errno = 0;
				x[i].index = (int)strtol(idx, &endptr, 10);
				if (endptr == idx || errno != 0 || *endptr != '\0' || x[i].index <= inst_max_index)
					exit_input_error(total + 1);
				else
					inst_max_index = x[i].index;

				errno = 0;
				x[i].value = strtod(val, &endptr);
				if (endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
					exit_input_error(total + 1);

				// feature indices larger than those in training are not used
				if (x[i].index <= nr_feature)
					++i;
			}

			if (model_->bias >= 0)
			{
				x[i].index = n;
				x[i].value = model_->bias;
				i++;
			}
			x[i].index = -1;

			if (flag_predict_probability)
			{
				INT64 j;
				Tamgutreemapi* mp = new Tamgutreemapi;
				mp->Setreference(output->reference + 1);
				output->values.push_back(mp);
				predict_label = predict_probability(model_, x, prob_estimates);
				if (infos == 1 || kindexlabels == NULL) {
					Tamgufvector* kvfloat = globalTamgu->Providefvector();
					kvfloat->Setreference(mp->reference + 1);
					mp->values[predict_label] = kvfloat;
					for (j = 0; j < model_->nr_class; j++)
						kvfloat->values.push_back(prob_estimates[j]);
				}
				else {
					if (infos == 2) {
						Tamgutreemapif* kmlabelfloat = new Tamgutreemapif;
						kmlabelfloat->Setreference(mp->reference + 1);
						mp->values[predict_label] = kmlabelfloat;
						int ilab;
						for (j = 0; j < model_->nr_class; j++) {
							ilab = kindexlabels->values[j];
							kmlabelfloat->values[ilab] = prob_estimates[j];
						}
					}
				}
			}
			else
			{
				predict_label = predict(model_, x);
				output->Push(globalTamgu->Providefloat(predict_label));
				ix++;
			}

			if (predict_label == target_label)
				++correct;
			error += (predict_label - target_label)*(predict_label - target_label);
			sump += predict_label;
			sumt += target_label;
			sumpp += predict_label*predict_label;
			sumtt += target_label*target_label;
			sumpt += predict_label*target_label;
			++total;
		}

		if (infos) {
			if (model_->param.solver_type == L2R_L2LOSS_SVR ||
				model_->param.solver_type == L2R_L1LOSS_SVR_DUAL ||
				model_->param.solver_type == L2R_L2LOSS_SVR_DUAL)	{
				kinfo->Push("Mean squared error (regression)", globalTamgu->Providefloat(error / total));
				kinfo->Push("Squared correlation coefficient (regression)",
					globalTamgu->Providefloat(((total*sumpt - sump*sumt)*(total*sumpt - sump*sumt)) / ((total*sumpp - sump*sump)*(total*sumtt - sumt*sumt))));
			}
			else {
				kinfo->Push("Accuracy", globalTamgu->Providefloat(correct / total * 100));
				kinfo->Push("Accuracy (correct)", globalTamgu->Providefloat(correct));
				kinfo->Push("Accuracy (total)", globalTamgu->Providefloat(total));
			}
		}

		if (flag_predict_probability)
			free(prob_estimates);

		return output;
	}

	Tamgu* do_predict(Tamgufvector* klabels, Tamguvector* input, Tamguvector* output) {
		Tamgutreemap* kinfo = NULL;
		if (infos) {
			kinfo = new Tamgutreemap;
			kinfo->Setreference(output->reference + 1);
			output->values.push_back(kinfo);
		}

		INT64 correct = 0;
		INT64 total = 0;
		double error = 0;
		double sump = 0, sumt = 0, sumpp = 0, sumtt = 0, sumpt = 0;

		INT64 nr_class = get_nr_class(model_);
		double *prob_estimates = NULL;
		INT64 j, n;
		INT64 nr_feature = get_nr_feature(model_);
		if (model_->bias >= 0)
			n = nr_feature + 1;
		else
			n = nr_feature;

		Tamguivector* kindexlabels = NULL;
		if (flag_predict_probability)
		{
			INT64 *labels;

			if (!check_probability_model(model_))
				return globalTamgu->Returnerror("LLN(013): probability output is only supported for logistic regression", globalTamgu->GetThreadid());

			labels = (INT64 *)malloc(nr_class*sizeof(INT64));
			get_labels(model_, labels);
			prob_estimates = (double *)malloc(nr_class*sizeof(double));
			if (infos) {
				kindexlabels = globalTamgu->Provideivector();
				kindexlabels->Setreference(kinfo->reference + 1);
				kinfo->values["labels"] = kindexlabels;
				for (j = 0; j < nr_class; j++)
					kindexlabels->values.push_back(labels[j]);
			}
			free(labels);
		}

		max_line_len = 1024;
		line = (char *)malloc(max_line_len*sizeof(char));
		map<long, double>::iterator it;

		double target_label = 0, predict_label;
		Tamgutreemapif mapbase;
		Tamgutreemapif* ktmif;

		for (INT64 ix = 0; ix < input->values.size(); ix++) {
			INT64 i = 0;
			INT64 inst_max_index = 0; // strtol gives 0 if wrong format
			if (klabels != NULL)
				target_label = klabels->values[ix];

			if (input->values[ix]->Type() != Tamgutreemapif::idtype) {
				if (!input->values[ix]->isContainer())
					return globalTamgu->Returnerror("LLN(011): Wrong map type", globalTamgu->GetThreadid());
				mapbase.values.clear();
				TamguIteration* itr = input->values[ix]->Newiteration(false);
				for (itr->Begin(); itr->End() == aFALSE; itr->Next())
					mapbase.values[itr->Keyinteger()] = itr->Valuefloat();
				itr->Release();
				ktmif = &mapbase;
			}
			else
				ktmif = (Tamgutreemapif*)input->values[ix];

			map<long, double>::iterator it;
			for (it = ktmif->values.begin(); it != ktmif->values.end(); it++) {

				if (i >= max_nr_attr - 2)	// need one more for index = -1
				{
					max_nr_attr *= 2;
					x = (struct feature_node *) realloc(x, max_nr_attr*sizeof(struct feature_node));
				}

				errno = 0;
				x[i].index = (int)it->first;
				if (x[i].index <= inst_max_index)
					exit_input_error(total + 1);
				else
					inst_max_index = x[i].index;

				errno = 0;
				x[i].value = it->second;
				// feature indices larger than those in training are not used
				if (x[i].index <= nr_feature)
					++i;
			}

			if (model_->bias >= 0)
			{
				x[i].index = n;
				x[i].value = model_->bias;
				i++;
			}
			x[i].index = -1;

			if (flag_predict_probability)
			{
				INT64 j;
				Tamgutreemapi* mp = new Tamgutreemapi;
				mp->Setreference(output->reference + 1);
				output->values.push_back(mp);
				predict_label = predict_probability(model_, x, prob_estimates);
				if (infos == 1 || kindexlabels == NULL) {
					Tamgufvector* kvfloat = globalTamgu->Providefvector();
					kvfloat->Setreference(mp->reference + 1);
					mp->values[predict_label] = kvfloat;
					for (j = 0; j < model_->nr_class; j++)
						kvfloat->values.push_back(prob_estimates[j]);
				}
				else {
					if (infos == 2) {
						Tamgutreemapif* kmlabelfloat = new Tamgutreemapif;
						kmlabelfloat->Setreference(mp->reference + 1);
						mp->values[predict_label] = kmlabelfloat;
						int ilab;
						for (j = 0; j < model_->nr_class; j++) {
							ilab = kindexlabels->values[j];
							kmlabelfloat->values[ilab] = prob_estimates[j];
						}
					}
				}
			}
			else
			{
				predict_label = predict(model_, x);
				output->Push(globalTamgu->Providefloat(predict_label));
				ix++;
			}

			if (predict_label == target_label)
				++correct;
			error += (predict_label - target_label)*(predict_label - target_label);
			sump += predict_label;
			sumt += target_label;
			sumpp += predict_label*predict_label;
			sumtt += target_label*target_label;
			sumpt += predict_label*target_label;
			++total;
		}

		if (infos) {
			if (model_->param.solver_type == L2R_L2LOSS_SVR ||
				model_->param.solver_type == L2R_L1LOSS_SVR_DUAL ||
				model_->param.solver_type == L2R_L2LOSS_SVR_DUAL)	{
				kinfo->Push("Mean squared error (regression)", globalTamgu->Providefloat(error / total));
				kinfo->Push("Squared correlation coefficient (regression)",
					globalTamgu->Providefloat(((total*sumpt - sump*sumt)*(total*sumpt - sump*sumt)) / ((total*sumpp - sump*sump)*(total*sumtt - sumt*sumt))));
			}
			else {
				kinfo->Push("Accuracy", globalTamgu->Providefloat(correct / total * 100));
				kinfo->Push("Accuracy (correct)", globalTamgu->Providefloat(correct));
				kinfo->Push("Accuracy (total)", globalTamgu->Providefloat(total));
			}
		}

		if (flag_predict_probability)
			free(prob_estimates);

		return output;
	}

	Tamgu* do_cross_validation() {
		INT64 i;
		INT64 total_correct = 0;
		double total_error = 0;
		double sumv = 0, sumy = 0, sumvv = 0, sumyy = 0, sumvy = 0;
		double *target = Malloc(double, prob.l);

		Tamgumapsf* kres = new Tamgumapsf;

		cross_validation(&prob, &param, nr_fold, target);
		if (param.solver_type == L2R_L2LOSS_SVR ||
			param.solver_type == L2R_L1LOSS_SVR_DUAL ||
			param.solver_type == L2R_L2LOSS_SVR_DUAL)
		{
			for (i = 0; i < prob.l; i++)
			{
				double y = prob.y[i];
				double v = target[i];
				total_error += (v - y)*(v - y);
				sumv += v;
				sumy += y;
				sumvv += v*v;
				sumyy += y*y;
				sumvy += v*y;
			}
			kres->values["Cross Validation Mean squared error"] = total_error / prob.l;
			kres->values["Cross Validation Squared correlation coefficient"] =
				((prob.l*sumvy - sumv*sumy)*(prob.l*sumvy - sumv*sumy)) /
				((prob.l*sumvv - sumv*sumv)*(prob.l*sumyy - sumy*sumy));
		}
		else
		{
			for (i = 0; i < prob.l; i++)
			if (target[i] == prob.y[i])
				++total_correct;
			kres->values["Cross Validation Accuracy"] = 100.0*total_correct / prob.l;
		}

		free(target);
		return kres;
	}

	Tamgu* exit_with_help(string o) {
		string msg = "LLN(001): Error with options:";
		msg += o;
		return globalTamgu->Returnerror(msg, globalTamgu->GetThreadid());
	}

	Tamgu* parse_options(Tamgu* kmap);

	Tamgu* exit_input_error(int line_num) {
		char msg[100];
		sprintf_s(msg, 100, "LLN(002): Wrong input format at line %d", line_num);
		return globalTamgu->Returnerror(msg, globalTamgu->GetThreadid());
	}

	char* readline(FILE *input)
	{
		INT64 len;

		if (fgets(line, max_line_len, input) == NULL)
			return NULL;

		while (strrchr(line, '\n') == NULL)
		{
			max_line_len *= 2;
			line = (char *)realloc(line, max_line_len);
			len = (int)strlen(line);
			if (fgets(line + len, max_line_len - len, input) == NULL)
				break;
		}
		return line;
	}

	Tamgu* read_problem(const char *filename) {
		INT64 max_index, inst_max_index, i;
		INT64 elements, j;
		FILE *fp; 
#ifdef WIN32
		fopen_s(&fp, filename, "r");
#else
		fp = fopen(filename, "r");
#endif
		char *endptr;
		char *idx, *val, *label;

		if (fp == NULL)
		{
			char buff[100];
			sprintf_s(buff, 100, "can't open input file %s", filename);
			return globalTamgu->Returnerror(buff, globalTamgu->GetThreadid());
		}

		prob.l = 0;
		elements = 0;
		max_line_len = 1024;
		line = Malloc(char, max_line_len);
		char* next_p = NULL;
		while (readline(fp) != NULL)
		{
#ifdef WIN32
			char *p = strtok_s(line, " \t", &next_p); // label
#else
			char *p = strtok(line, " \t"); // label
#endif

			// features
			while (1)
			{
#ifdef WIN32
				p = strtok_s(NULL, " \t", &next_p);
#else
				p = strtok(NULL, " \t");
#endif
				if (p == NULL || *p == '\n') // check '\n' as ' ' may be after the last feature
					break;
				elements++;
			}
			elements++; // for bias term
			prob.l++;
		}
		rewind(fp);

		prob.bias = bias;

		prob.y = Malloc(double, prob.l);
		prob.x = Malloc(struct feature_node *, prob.l);
		x_space = Malloc(struct feature_node, elements + prob.l);

		max_index = 0;
		j = 0;
		for (i = 0; i < prob.l; i++)
		{
			inst_max_index = 0; // strtol gives 0 if wrong format
			readline(fp);
			prob.x[i] = &x_space[j];
#ifdef WIN32
			label = strtok_s(line, " \t\n", &next_p);
#else
			label = strtok(line, " \t\n");
#endif
			if (label == NULL) // empty line
				return exit_input_error(i + 1);

			prob.y[i] = strtod(label, &endptr);
			if (endptr == label || *endptr != '\0')
				return exit_input_error(i + 1);

			while (1)
			{
#ifdef WIN32
				idx = strtok_s(NULL, ":", &next_p);
				val = strtok_s(NULL, " \t", &next_p);
#else
				idx = strtok(NULL, ":");
				val = strtok(NULL, " \t");
#endif
				if (val == NULL)
					break;

				errno = 0;
				x_space[j].index = (int)strtol(idx, &endptr, 10);
				if (endptr == idx || errno != 0 || *endptr != '\0' || x_space[j].index <= inst_max_index)
					return exit_input_error(i + 1);
				else
					inst_max_index = x_space[j].index;

				errno = 0;
				x_space[j].value = strtod(val, &endptr);
				if (endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
					return exit_input_error(i + 1);

				++j;
			}

			if (inst_max_index > max_index)
				max_index = inst_max_index;

			if (prob.bias >= 0)
				x_space[j++].value = prob.bias;

			x_space[j++].index = -1;
		}

		if (prob.bias >= 0)
		{
			prob.n = max_index + 1;
			for (i = 1; i < prob.l; i++)
				(prob.x[i] - 2)->index = prob.n;
			x_space[j - 2].index = prob.n;
		}
		else
			prob.n = max_index;

		fclose(fp);
		return aTRUE;
	}

	//Load the training data from vectors...
	Tamgu* load_problem(Tamgufvector* labels, Tamguvector* data) {
		if (loaded)
			return globalTamgu->Returnerror("LLN(010): Clean your data first", globalTamgu->GetThreadid());

		INT64 max_index, inst_max_index, i;
		INT64 elements, j;

		prob.l = 0;
		elements = 0;
		for (j = 0; j < data->Size(); j++)
			elements += data->values[j]->Size() + 1;
		prob.l = labels->Size();

		prob.bias = bias;

		prob.y = Malloc(double, prob.l);
		prob.x = Malloc(struct feature_node *, prob.l);
		x_space = Malloc(struct feature_node, elements + prob.l);

		max_index = 0;
		j = 0;
		Tamgutreemapif* ktmif;
		Tamgutreemapif mapbase;
		Tamgu* mymap;

		for (i = 0; i < prob.l; i++)
		{
			inst_max_index = 0; // strtol gives 0 if wrong format
			prob.x[i] = &x_space[j];

			prob.y[i] = labels->values[i];

			mymap = data->values[i];

			if (mymap->Type() != Tamgutreemapif::idtype) {
				if (!mymap->isContainer())
					return globalTamgu->Returnerror("LLN(011): Wrong map type", globalTamgu->GetThreadid());
				mapbase.values.clear();
				TamguIteration* itr = mymap->Newiteration(false);
				for (itr->Begin(); itr->End() == aFALSE; itr->Next())
					mapbase.values[itr->Keyinteger()] = itr->Valuefloat();
				itr->Release();
				ktmif = &mapbase;
			}
			else
				ktmif = (Tamgutreemapif*)mymap;

			map<long, double>::iterator it;
			for (it = ktmif->values.begin(); it != ktmif->values.end(); it++) {

				errno = 0;
				x_space[j].index = (int)it->first;
				if (x_space[j].index <= inst_max_index)
					return exit_input_error(i + 1);
				else
					inst_max_index = x_space[j].index;

				errno = 0;
				x_space[j].value = it->second;
				++j;
			}

			if (inst_max_index > max_index)
				max_index = inst_max_index;

			if (prob.bias >= 0)
				x_space[j++].value = prob.bias;

			x_space[j++].index = -1;
		}

		if (prob.bias >= 0)
		{
			prob.n = max_index + 1;
			for (i = 1; i < prob.l; i++)
				(prob.x[i] - 2)->index = prob.n;
			x_space[j - 2].index = prob.n;
		}
		else
			prob.n = max_index;

		return aTRUE;
	}

	//---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is a sample  of a function that could be implemented for your needs.
	
	Tamgu* MethodInitial(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		//Your first parameter is at position 0 etc...		
		Tamgu* mfn = callfunc->Evaluate(0, contextualpattern, idthread);
		model_file_name = mfn->String();
		mfn->Release();

		flag_predict_probability = 0;
		if ((model_ = load_model(STR(model_file_name))) == 0)
			return globalTamgu->Returnerror("LLN(012): can't open model file", idthread);

		loaded = true;
		return this;
	}


	Tamgu* MethodLoad(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodSave(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodOptions(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodCrossvalidation(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodClean(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodTrainingSet(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLoadModel(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodDoPredictFromFile(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodDoPredict(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLoadproblems(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodAddproblemvectors(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodComputeidf(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodNormalize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodAnalysisproblems(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodCleaning(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);


    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods[idname])(contextualpattern, idthread, callfunc);
    }

    void Clear(TamguGlobal* g) {
        //To set a variable back to empty
    }

    void Clean() {
        //Your cleaning code
    }

	long Integer() { return 0; }
	short Short() { return 0;}
	double Float() { return 0; }
	bool Boolean() { return false; }
	BLONG Long() { return 0; }
	unsigned char Byte() {
		return 0;
	}

	long Size() { return 0; }

};
#endif

