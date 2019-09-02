//  Copyright 2013 Google Inc. All Rights Reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.



#ifdef WIN32
#include <Windows.h>
#endif

#include "tamgu.h"
#include "tamguversion.h"

#ifdef WIN32
unsigned __stdcall TrainModelThread(void* arg);
unsigned __stdcall TrainModelThreadVector(void* arg);
#else
void *TrainModelThread(void *id);
void *TrainModelThreadVector(void *id);
#endif


#include "tamguword2vec.h"

class localcallthread {
public:

	tamguword2vec* current;
	long id;

	localcallthread(tamguword2vec* c, long i) {
		id = i;
		current = c;
	}

};

//------------------------------------------------------------------------------------------------------------------
//If you need to implement an external library... Otherwise remove it...
//When Tamgu (탐구) loads a library, it looks for this method in particular, which then make this object available into Tamgu (탐구)
extern "C" {
    Exporting bool word2vec_InitialisationModule(TamguGlobal* global, string version) {
        if (version != TamguVersion())
            return false;

        global->Update();

		Tamguw2vector::InitialisationModule(global, version);
        return Tamguword2vec::InitialisationModule(global, version);
    }
}

//------------------------------------------------------------------------------------------------------------------
//We need to declare once again our local definitions.
hmap<unsigned short, word2vecItemMethod>  Tamguw2vector::methods;
hmap<string, string> Tamguw2vector::infomethods;
bin_hash<unsigned long> Tamguw2vector::exported;

short Tamguw2vector::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguw2vector::AddMethod(TamguGlobal* global, string name, word2vecItemMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Tamguw2vector::InitialisationModule(TamguGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	//Each new object has a specific name, which will help recognize it in the code that will exploit word2vec...
	Tamguw2vector::idtype = global->Getid("w2vector");

	//You declare your methods here:
	// Argument 1 is a call to global, which will record your method into Tamgu (탐구)
	// Argument 2 is the name of your command
	// Argument 3 links your command with its implementation into the object class
	// Argument 4 is a combination of P_... which defines how many parameters this function can accept.
	//Argument 5 is a description of this method.
	//Tamguw2vector::AddMethod(global, "command", &Tamguw2vector::MethodCommand, P_ONE | P_TWO, "command(string test): A sample of how to implement a new method for your object.");
	//We need to create specific objects in order to have our right class method called
	//MethodInitializationWord2Vec, which is declared in this file (see above), implements a specific object that will be returned
	//by the Declaration method above. When a TamguCallMethod object is detected during execution, then Tamgu calls
	//ExecuteMethod, which must have been implemented for our new object (see above again). We also link this new name
	//with a specific method from our new class.

	Tamguw2vector::AddMethod(global, "_initial", &Tamguw2vector::MethodInitial, P_NONE|P_ONE, "_initial(word2vecitem i): initialize from a word2vectitem");
	Tamguw2vector::AddMethod(global, "dot", &Tamguw2vector::MethodDot, P_ONE, "dot(element): Return the dot product between two words. Element is either a string or a w2vector");
	Tamguw2vector::AddMethod(global, "cosine", &Tamguw2vector::MethodCosine, P_ONE, "cosine(element): Return the cosine distance between two words. Element is either a string or a w2vector.");
	Tamguw2vector::AddMethod(global, "distance", &Tamguw2vector::MethodDistance, P_ONE, "distance(element): Return the distance between two words. Element is either a string or a w2vector.");
	Tamguw2vector::AddMethod(global, "threshold", &Tamguw2vector::MethodThreshold, P_NONE | P_ONE, "threshold(element): Return or set the threshold.");
	Tamguw2vector::AddMethod(global, "norm", &Tamguw2vector::MethodNorm, P_NONE, "norm(element): Return the vector norm.");


	//We need this code, in order to create new instances of our word2vec object... DO NOT ALTER
	global->newInstance[Tamguw2vector::idtype] = new Tamguw2vector(global);
	global->RecordMethods(Tamguw2vector::idtype, Tamguw2vector::exported);

	return true;
}

//------------------------------------------------------------------------------------------------------------------

//We need to declare once again our local definitions.
hmap<unsigned short, word2vecMethod>  Tamguword2vec::methods;
hmap<string, string> Tamguword2vec::infomethods;
bin_hash<unsigned long> Tamguword2vec::exported;

short Tamguword2vec::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguword2vec::AddMethod(TamguGlobal* global, string name, word2vecMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Tamguword2vec::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    //Each new object has a specific name, which will help recognize it in the code that will exploit word2vec...
    Tamguword2vec::idtype = global->Getid("word2vec");

    //You declare your methods here:
    // Argument 1 is a call to global, which will record your method into Tamgu (탐구)
    // Argument 2 is the name of your command
    // Argument 3 links your command with its implementation into the object class
    // Argument 4 is a combination of P_... which defines how many parameters this function can accept.
    //Argument 5 is a description of this method.
    //Tamguword2vec::AddMethod(global, "command", &Tamguword2vec::MethodCommand, P_ONE | P_TWO, "command(string test): A sample of how to implement a new method for your object.");
	//We need to create specific objects in order to have our right class method called
	//MethodInitializationWord2Vec, which is declared in this file (see above), implements a specific object that will be returned
	//by the Declaration method above. When a TamguCallMethod object is detected during execution, then Tamgu calls
	//ExecuteMethod, which must have been implemented for our new object (see above again). We also link this new name
	//with a specific method from our new class.
	Tamguword2vec::AddMethod(global, "initialization", &Tamguword2vec::MethodInit, P_ONE, "initialization(map m): Initialization of a word2vec training set");
	Tamguword2vec::AddMethod(global, "trainmodel", &Tamguword2vec::MethodTrainModel, P_NONE | P_ONE, "trainmodel(vector v): Launching the training. If v is not supplied, then the system utilizes the input file given in the initialisation options");
	Tamguword2vec::AddMethod(global, "loadmodel", &Tamguword2vec::MethodLoadModel, P_ONE | P_TWO, "loadmodel(string filename,bool normalize): Loading a model");
	Tamguword2vec::AddMethod(global, "analogy", &Tamguword2vec::MethodAnalogy, P_ONE, "analogy(svector words): Finding analogies for a group of words. Return a fmap");
	Tamguword2vec::AddMethod(global, "accuracy", &Tamguword2vec::MethodAccuracy, P_ONE | P_TWO, "accuracy(vector words,int threshold): Finding accuracies for a vector of many times 4 words. Return a fmap. If threshold is not supplied then its value is 30000");
	Tamguword2vec::AddMethod(global, "distance", &Tamguword2vec::MethodDistance, P_ONE, "distance(svector words): Finding the distance in a vector of words. Return a fmap.");
	Tamguword2vec::AddMethod(global, "vocabulary", &Tamguword2vec::MethodVocabulary, P_NONE, "vocabulary(): Return a itreemap of the vocabulary covered by the training.");
	Tamguword2vec::AddMethod(global, "features", &Tamguword2vec::MethodFeatures, P_NONE, "features(): Return a map of the vocabulary with their feature values.");

	
    //We need this code, in order to create new instances of our word2vec object... DO NOT ALTER
    global->newInstance[Tamguword2vec::idtype] = new Tamguword2vec(global);
    global->RecordMethods(Tamguword2vec::idtype,Tamguword2vec::exported);
    
    return true;
}

TamguIteration* Tamguword2vec::Newiteration(bool direction) {
	//If you do not need an iteration, do:
	return aITERNULL;
}

TamguIteration* Tamguw2vector::Newiteration(bool direction) {
	//If you do not need an iteration, do:
	return aITERNULL;
}


Tamgu* Tamguword2vec::Put(Tamgu* idx, Tamgu* kval, short idthread) {
    //If you want to put some element into your object
    return aTRUE;
}

Tamgu* Tamguw2vector::Eval(Tamgu* context, Tamgu* idx, short idthread) {
	return this;
}

Tamgu* Tamguword2vec::Eval(Tamgu* context, Tamgu* idx, short idthread) {
	if (w2vec == NULL)
		return globalTamgu->Returnerror("W2V(110): Empty model");

	if (idx != NULL && !idx->isConst()) {
		string skey;
		Tamgu* key;
		double threshold = 0;
		if (idx->isIndex()) {
			TamguIndex* kind = (TamguIndex*)idx;
			key = kind->left->Eval(aNULL, aNULL, idthread);
			if (kind->interval == true)
				threshold = kind->right->Getfloat(idthread);
		}
		else
			key = idx->Eval(aNULL, aNULL, idthread);

		skey = key->String();
		key->Release();
		Tamguw2vector* item = new Tamguw2vector;
		if (w2vec->d_vocab.values.find(skey) == w2vec->d_vocab.values.end()) {
			item->initialization(this);
			item->value = skey;
			return item;
		}

		item->w2vreference = this;
		item->value = skey;
		item->threshold = threshold;
		item->wordindex = w2vec->d_vocab.values[skey];
		item->vect = w2vec->avector(item->wordindex);
		item->vectnorm = w2vec->normalize(item->vect, item->norm);
		return item;
	}

	return this;
}

void tamguword2vec::LaunchTrainModel() {
#ifndef WIN32
	pthread_t *pt = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
#endif
	FILE *fo;
	long a, b, c, d;
	if (read_context_file[0] != 0) {
		if (loadcontext(read_context_file))
			return;
	}
	else
		InitNet();

	if (negative > 0) InitUnigramTable();
	start = clock();
	
	localcallthread* lcall;
	globalTamgu->globalLOCK = true;

#ifdef WIN32
	DWORD tid;
	tobejoined.clear();
	for (a = 0; a < num_threads; a++)
		tobejoined.push_back(true);

	if (inputwords == NULL) {
		for (a = 0; a < num_threads; a++) {
			lcall = new localcallthread(this, a);
			_beginthreadex(NULL, 0, TrainModelThread, (void*)lcall, 0, (UINT*)&tid);
		}
	}
	else {
		for (a = 0; a < num_threads; a++) {
			lcall = new localcallthread(this, a);
			_beginthreadex(NULL, 0, TrainModelThreadVector, (void*)lcall, 0, (UINT*)&tid);
		}
	}

	for (a = 0; a < num_threads; a++) {
		Locking _lock(mother);
		if (tobejoined[a] == true)
			a--;
	}
#else
	if (inputwords == NULL)
	for (a = 0; a < num_threads; a++) {
		lcall = new localcallthread(this, a);
		pthread_create(&pt[a], NULL, TrainModelThread, (void *)lcall);
	}
	else
	for (a = 0; a < num_threads; a++) {
		lcall = new localcallthread(this, a);
		pthread_create(&pt[a], NULL, TrainModelThreadVector, (void *)lcall);
	}

	for (a = 0; a < num_threads; a++) pthread_join(pt[a], NULL);
#endif

	if (save_context_file[0]) {
#ifdef WIN32
		fopen_s(&fo, save_context_file, "wb");
#else
		fo = fopen(save_context_file, "wb");
#endif
		// Save the word vectors
		fprintf(fo, "%lld %lld\n", vocab_size, layer1_size);
		long c;
		for (a = 0; a < vocab_size; a++) {
			for (b = 0; b < layer1_size; b++) {
				c = a * layer1_size + b;
				fwrite(&syn0[c], sizeof(real), 1, fo);
				if (syn1 != NULL)
					fwrite(&syn1[c], sizeof(real), 1, fo);
				if (syn1neg != NULL)
					fwrite(&syn1neg[c], sizeof(real), 1, fo);
			}
		}
		fclose(fo);
	}
#ifdef WIN32
	fopen_s(&fo, output_file, "wb");
#else
	fo = fopen(output_file, "wb");
#endif
	if (classes == 0) {
		// Save the word vectors
		fprintf(fo, "%lld %lld\n", vocab_size, layer1_size);
		for (a = 0; a < vocab_size; a++) {
			fprintf(fo, "%s ", vocab[a].word);
			if (binary) for (b = 0; b < layer1_size; b++) fwrite(&syn0[a * layer1_size + b], sizeof(real), 1, fo);
			else for (b = 0; b < layer1_size; b++) fprintf(fo, "%lf ", syn0[a * layer1_size + b]);
			fprintf(fo, "\n");
		}
	}
	else {
		// Run K-means on the word vectors
		int clcn = classes, iter = 10, closeid;
		int *centcn = (int *)malloc(classes * sizeof(int));
		int *cl = (int *)calloc(vocab_size, sizeof(int));
		real closev, x;
		real *cent = (real *)calloc(classes * layer1_size, sizeof(real));
		for (a = 0; a < vocab_size; a++) cl[a] = a % clcn;
		for (a = 0; a < iter; a++) {
			for (b = 0; b < clcn * layer1_size; b++) cent[b] = 0;
			for (b = 0; b < clcn; b++) centcn[b] = 1;
			for (c = 0; c < vocab_size; c++) {
				for (d = 0; d < layer1_size; d++) cent[layer1_size * cl[c] + d] += syn0[c * layer1_size + d];
				centcn[cl[c]]++;
			}
			for (b = 0; b < clcn; b++) {
				closev = 0;
				for (c = 0; c < layer1_size; c++) {
					cent[layer1_size * b + c] /= centcn[b];
					closev += cent[layer1_size * b + c] * cent[layer1_size * b + c];
				}
				closev = sqrt(closev);
				for (c = 0; c < layer1_size; c++) cent[layer1_size * b + c] /= closev;
			}
			for (c = 0; c < vocab_size; c++) {
				closev = -10;
				closeid = 0;
				for (d = 0; d < clcn; d++) {
					x = 0;
					for (b = 0; b < layer1_size; b++) x += cent[layer1_size * d + b] * syn0[c * layer1_size + b];
					if (x > closev) {
						closev = x;
						closeid = d;
					}
				}
				cl[c] = closeid;
			}
		}
		// Save the K-means classes
		for (a = 0; a < vocab_size; a++) fprintf(fo, "%s %d\n", vocab[a].word, cl[a]);
		free(centcn);
		free(cent);
		free(cl);
	}
	fclose(fo);
}

Tamgu* tamguword2vec::init(Tamgu* themap) {

	output_file[0] = 0;
	save_context_file[0] = 0;
	read_context_file[0] = 0;
	save_vocab_file[0] = 0;
	read_vocab_file[0] = 0;

	TamguIteration* itr = themap->Newiteration(false);
	string key;
	for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
		key = itr->Keystring();

		if (key == "size") {
			layer1_size = itr->Valueinteger();
			continue;
		}
		if (key == "train") {
			strcpy_s(train_file, W2VEC_MAX_STRING, STR(itr->Valuestring()));
			continue;
		}

		if (key == "save-context") {
			strcpy_s(save_context_file, W2VEC_MAX_STRING, STR(itr->Valuestring()));
			continue;
		}

		if (key == "read-context") {
			strcpy_s(read_context_file, W2VEC_MAX_STRING, STR(itr->Valuestring()));
			continue;
		}

		if (key == "save-vocab") {
			strcpy_s(save_vocab_file, W2VEC_MAX_STRING, STR(itr->Valuestring()));
			continue;
		}
		if (key == "read-vocab") {
			strcpy_s(read_vocab_file, W2VEC_MAX_STRING, STR(itr->Valuestring()));
			continue;
		}

		if (key == "debug") {
			debug_mode = itr->Valueinteger();
			continue;
		}
		if (key == "binary") {
			binary = itr->Valueinteger();
			continue;
		}
		if (key == "cbow") {
			cbow = itr->Valueinteger();
			if (cbow) alpha = 0.05;
			continue;
		}

		if (key == "alpha") {
			alpha = itr->Valuefloat();
			continue;
		}

		if (key == "output"){
			strcpy_s(output_file, W2VEC_MAX_STRING, STR(itr->Valuestring()));
			continue;
		}

		if (key == "window") {
			window = itr->Valueinteger();
			continue;
		}

		if (key == "sample") {
			sample = itr->Valuefloat();
			continue;
		}

		if (key == "hs") {
			hs = itr->Valueinteger();
			continue;
		}
		if (key == "negative") {
			negative = itr->Valueinteger();
			continue;
		}

		if (key == "threads") {
			num_threads = itr->Valueinteger();
			continue;
		}

		if (key == "iter") {
			iter = itr->Valueinteger();
			continue;
		}

		if (key == "min-count") {
			min_count = itr->Valueinteger();
			continue;
		}

		if (key == "classes") {
			classes = itr->Valueinteger();
			continue;
		}

		string errmsg("W2V(333): Unkown option: ");
		errmsg += key;

		return globalTamgu->Returnerror(errmsg);
	}

	itr->Release();

	if (read_context_file[0] && read_vocab_file[0] == 0)
		return globalTamgu->Returnerror("W2V(032): Missing vocabulary file");

	if (vocab != NULL){
		free(vocab);
		free(vocab_hash);
		free(expTable);
	}

	vocab = (struct W2VEC_vocab_word *)calloc(vocab_max_size, sizeof(struct W2VEC_vocab_word));
	vocab_hash = (int *)calloc(W2VEC_vocab_hash_size, sizeof(int));
	expTable = (real *)malloc((W2VEC_EXP_W2VEC_table_size + 1) * sizeof(real));
	for (int i = 0; i < W2VEC_EXP_W2VEC_table_size; i++) {
		expTable[i] = exp((i / (real)W2VEC_EXP_W2VEC_table_size * 2 - 1) * W2VEC_MAX_EXP); // Precompute the exp() table
		expTable[i] = expTable[i] / (expTable[i] + 1);                   // Precompute f(x) = x / (x + 1)
	}
	return aTRUE;
}

void tamguword2vec::InitUnigramTable() {
	int a, i;
	long long train_words_pow = 0;
	real d1, power = 0.75;
	table = (int *)malloc(W2VEC_table_size * sizeof(int));
	for (a = 0; a < vocab_size; a++) train_words_pow += pow(vocab[a].cn, power);
	i = 0;
	d1 = pow(vocab[i].cn, power) / (real)train_words_pow;
	for (a = 0; a < W2VEC_table_size; a++) {
		table[a] = i;
		if (a / (real)W2VEC_table_size > d1) {
			i++;
			d1 += pow(vocab[i].cn, power) / (real)train_words_pow;
		}
		if (i >= vocab_size) i = vocab_size - 1;
	}
}

// Reads a single word from a file, assuming space + tab + EOL to be word boundaries
void tamguword2vec::ReadWord(char *word, FILE *fin) {
	int a = 0, ch;
	while (!feof(fin)) {
		ch = fgetc(fin);
		if (ch == 13) continue;
		if ((ch == ' ') || (ch == '\t') || (ch == '\n')) {
			if (a > 0) {
				if (ch == '\n') ungetc(ch, fin);
				break;
			}
			if (ch == '\n') {
				strcpy_s(word, W2VEC_MAX_STRING, (char *)"</s>");
				return;
			}
			else continue;
		}
		word[a] = ch;
		a++;
		if (a >= W2VEC_MAX_STRING - 1) a--;   // Truncate too long words
	}
	word[a] = 0;
}


#ifdef WIN32
unsigned __stdcall TrainModelThread(void* id) {
#else
void *TrainModelThread(void *id) {
#endif

	localcallthread* lcall = (localcallthread*)id;
	tamguword2vec* current = lcall->current;
	unsigned long long next_random = lcall->id;
#ifdef WIN32
	long currentid = lcall->id;
#endif
	delete lcall;

	long long a, b, d, cw, word, last_word, sentence_length = 0, sentence_position = 0;
	long long word_count = 0, last_word_count = 0, sen[W2VEC_MAX_SENTENCE_LENGTH + 1];
	long long l1, l2, c, target, label, local_iter = current->iter;

	real f, g;
	clock_t now;
	real *neu1 = (real *)calloc(current->layer1_size, sizeof(real));
	real *neu1e = (real *)calloc(current->layer1_size, sizeof(real));
	FILE *fi;
#ifdef WIN32
	fopen_s(&fi, current->train_file, "rb");
#else
	fi = fopen(current->train_file, "rb");
#endif
	fseek(fi, current->file_size / (long long)current->num_threads * (long long)id, SEEK_SET);
	while (1) {
		if (word_count - last_word_count > 10000) {
			current->word_count_actual += word_count - last_word_count;
			last_word_count = word_count;
			if ((current->debug_mode > 1)) {
				now = clock();
				printf("%cAlpha: %f  Progress: %.2f%%  Words/thread/sec: %.2fk  ", 13, current->alpha,
					current->word_count_actual / (real)(current->iter * current->train_words + 1) * 100,
					current->word_count_actual / ((real)(now - current->start + 1) / (real)CLOCKS_PER_SEC * 1000));
				fflush(stdout);
			}
			current->alpha = current->starting_alpha * (1 - current->word_count_actual / (real)(current->iter * current->train_words + 1));
			if (current->alpha < current->starting_alpha * 0.0001) current->alpha = current->starting_alpha * 0.0001;
		}
		if (sentence_length == 0) {
			while (1) {
				word = current->ReadWordIndex(fi);
				if (feof(fi)) break;
				if (word == -1) continue;
				word_count++;
				if (word == 0) break;
				// The subsampling randomly discards frequent words while keeping the ranking same
				if (current->sample > 0) {
					real ran = (sqrt(current->vocab[word].cn / (current->sample * current->train_words)) + 1) * (current->sample * current->train_words) / current->vocab[word].cn;
					next_random = next_random * learningconstant + 11;
					if (ran < (next_random & 0xFFFF) / (real)65536) continue;
				}
				sen[sentence_length] = word;
				sentence_length++;
				if (sentence_length >= W2VEC_MAX_SENTENCE_LENGTH) break;
			}
			sentence_position = 0;
		}
		if (feof(fi) || (word_count > current->train_words / current->num_threads)) {
			current->word_count_actual += word_count - last_word_count;
			local_iter--;
			if (local_iter == 0) break;
			word_count = 0;
			last_word_count = 0;
			sentence_length = 0;
			fseek(fi, current->file_size / (long long)current->num_threads * (long long)id, SEEK_SET);
			continue;
		}
		word = sen[sentence_position];
		if (word == -1) continue;
		for (c = 0; c < current->layer1_size; c++) neu1[c] = 0;
		for (c = 0; c < current->layer1_size; c++) neu1e[c] = 0;
		next_random = next_random * learningconstant + 11;
		b = next_random % current->window;
		if (current->cbow) {  //train the cbow architecture
			// in -> hidden
			cw = 0;
			for (a = b; a < current->window * 2 + 1 - b; a++) if (a != current->window) {
				c = sentence_position - current->window + a;
				if (c < 0) continue;
				if (c >= sentence_length) continue;
				last_word = sen[c];
				if (last_word == -1) continue;
				for (c = 0; c < current->layer1_size; c++) neu1[c] += current->syn0[c + last_word * current->layer1_size];
				cw++;
			}
			if (cw) {
				for (c = 0; c < current->layer1_size; c++) neu1[c] /= cw;
				if (current->hs) for (d = 0; d < current->vocab[word].codelen; d++) {
					f = 0;
					l2 = current->vocab[word].point[d] * current->layer1_size;
					// Propagate hidden -> output
					for (c = 0; c < current->layer1_size; c++) f += neu1[c] * current->syn1[c + l2];
					if (f <= -W2VEC_MAX_EXP) continue;
					else if (f >= W2VEC_MAX_EXP) continue;
					else f = current->expTable[(int)((f + W2VEC_MAX_EXP) * (W2VEC_EXP_W2VEC_table_size / W2VEC_MAX_EXP / 2))];
					// 'g' is the gradient multiplied by the learning rate
					g = (1 - current->vocab[word].code[d] - f) * current->alpha;
					// Propagate errors output -> hidden
					for (c = 0; c < current->layer1_size; c++) neu1e[c] += g * current->syn1[c + l2];
					// Learn weights hidden -> output
					for (c = 0; c < current->layer1_size; c++) current->syn1[c + l2] += g * neu1[c];
				}
				// NEGATIVE SAMPLING
				if (current->negative > 0) for (d = 0; d < current->negative + 1; d++) {
					if (d == 0) {
						target = word;
						label = 1;
					}
					else {
						next_random = next_random * learningconstant + 11;
						target = current->table[(next_random >> 16) % W2VEC_table_size];
						if (target == 0) target = next_random % (current->vocab_size - 1) + 1;
						if (target == word) continue;
						label = 0;
					}
					l2 = target * current->layer1_size;
					f = 0;
					for (c = 0; c < current->layer1_size; c++) f += neu1[c] * current->syn1neg[c + l2];
					if (!testfinite(f)) g = (label - 0) * current->alpha;
					else
					if (f > W2VEC_MAX_EXP) g = (label - 1) * current->alpha;
					else if (f < -W2VEC_MAX_EXP) g = (label - 0) * current->alpha;
					else g = (label - current->expTable[(int)((f + W2VEC_MAX_EXP) * (W2VEC_EXP_W2VEC_table_size / W2VEC_MAX_EXP / 2))]) * current->alpha;
					for (c = 0; c < current->layer1_size; c++) neu1e[c] += g * current->syn1neg[c + l2];
					for (c = 0; c < current->layer1_size; c++) current->syn1neg[c + l2] += g * neu1[c];
				}
				// hidden -> in
				for (a = b; a < current->window * 2 + 1 - b; a++) if (a != current->window) {
					c = sentence_position - current->window + a;
					if (c < 0) continue;
					if (c >= sentence_length) continue;
					last_word = sen[c];
					if (last_word == -1) continue;
					for (c = 0; c < current->layer1_size; c++) current->syn0[c + last_word * current->layer1_size] += neu1e[c];
				}
			}
		}
		else {  //train skip-gram
			for (a = b; a < current->window * 2 + 1 - b; a++) if (a != current->window) {
				c = sentence_position - current->window + a;
				if (c < 0) continue;
				if (c >= sentence_length) continue;
				last_word = sen[c];
				if (last_word == -1) continue;
				l1 = last_word * current->layer1_size;
				for (c = 0; c < current->layer1_size; c++) neu1e[c] = 0;
				// HIERARCHICAL SOFTMAX
				if (current->hs) for (d = 0; d < current->vocab[word].codelen; d++) {
					f = 0;
					l2 = current->vocab[word].point[d] * current->layer1_size;
					// Propagate hidden -> output
					for (c = 0; c < current->layer1_size; c++) f += current->syn0[c + l1] * current->syn1[c + l2];
					if (f <= -W2VEC_MAX_EXP) continue;
					else if (f >= W2VEC_MAX_EXP) continue;
					else f = current->expTable[(int)((f + W2VEC_MAX_EXP) * (W2VEC_EXP_W2VEC_table_size / W2VEC_MAX_EXP / 2))];
					// 'g' is the gradient multiplied by the learning rate
					g = (1 - current->vocab[word].code[d] - f) * current->alpha;
					// Propagate errors output -> hidden
					for (c = 0; c < current->layer1_size; c++) neu1e[c] += g * current->syn1[c + l2];
					// Learn weights hidden -> output
					for (c = 0; c < current->layer1_size; c++) current->syn1[c + l2] += g * current->syn0[c + l1];
				}
				// NEGATIVE SAMPLING
				if (current->negative > 0) for (d = 0; d < current->negative + 1; d++) {
					if (d == 0) {
						target = word;
						label = 1;
					}
					else {
						next_random = next_random * learningconstant + 11;
						target = current->table[(next_random >> 16) % W2VEC_table_size];
						if (target == 0) target = next_random % (current->vocab_size - 1) + 1;
						if (target == word) continue;
						label = 0;
					}
					l2 = target * current->layer1_size;
					f = 0;
					for (c = 0; c < current->layer1_size; c++) f += current->syn0[c + l1] * current->syn1neg[c + l2];
					if (!testfinite(f)) g = (label - 0) * current->alpha;
					else
					if (f > W2VEC_MAX_EXP) g = (label - 1) * current->alpha;
					else if (f < -W2VEC_MAX_EXP) g = (label - 0) * current->alpha;
					else g = (label - current->expTable[(int)((f + W2VEC_MAX_EXP) * (W2VEC_EXP_W2VEC_table_size / W2VEC_MAX_EXP / 2))]) * current->alpha;
					for (c = 0; c < current->layer1_size; c++) neu1e[c] += g * current->syn1neg[c + l2];
					for (c = 0; c < current->layer1_size; c++) current->syn1neg[c + l2] += g * current->syn0[c + l1];
				}
				// Learn weights input -> hidden
				for (c = 0; c < current->layer1_size; c++) current->syn0[c + l1] += neu1e[c];
			}
		}
		sentence_position++;
		if (sentence_position >= sentence_length) {
			sentence_length = 0;
			continue;
		}
	}
	fclose(fi);
	free(neu1);
	free(neu1e);
#ifdef WIN32
	Locking _lock(current->mother);
	current->tobejoined[currentid] = false;
	return(0);
#else
	pthread_exit(NULL);
#endif
}

#ifdef WIN32
unsigned __stdcall TrainModelThreadVector(void* id) {
#else
void *TrainModelThreadVector(void *id) {
#endif

	localcallthread* lcall = (localcallthread*)id;
	tamguword2vec* current = lcall->current;
	unsigned long long next_random = lcall->id;
#ifdef WIN32
	long currentid = lcall->id;
#endif
	delete lcall;

	long long a, b, d, cw, word, last_word, sentence_length = 0, sentence_position = 0;
	long long word_count = 0, last_word_count = 0, sen[W2VEC_MAX_SENTENCE_LENGTH + 1];
	long long l1, l2, c, target, label, local_iter = current->iter;
	
	real f, g;
	clock_t now;
	real *neu1 = (real *)calloc(current->layer1_size, sizeof(real));
	real *neu1e = (real *)calloc(current->layer1_size, sizeof(real));

	//We need to select the part of the vector which are going to analyze...
	Tamgu* inputwords = current->inputwords;

	int baseidx = inputwords->Size() / (long long)current->num_threads * (long long)id;
	int idx = baseidx;

	while (1) {
		if (word_count - last_word_count > 10000) {
			current->word_count_actual += word_count - last_word_count;
			last_word_count = word_count;
			if ((current->debug_mode > 1)) {
				now = clock();
				printf("%cAlpha: %f  Progress: %.2f%%  Words/thread/sec: %.2fk  ", 13, current->alpha,
					current->word_count_actual / (real)(current->iter * current->train_words + 1) * 100,
					current->word_count_actual / ((real)(now - current->start + 1) / (real)CLOCKS_PER_SEC * 1000));
				fflush(stdout);
			}
			current->alpha = current->starting_alpha * (1 - current->word_count_actual / (real)(current->iter * current->train_words + 1));
			if (current->alpha < current->starting_alpha * 0.0001) current->alpha = current->starting_alpha * 0.0001;
		}
		if (sentence_length == 0) {
			string aword;
			char bword[W2VEC_MAX_STRING + 1];
			while (1) {
				aword = inputwords->getstring(idx++);
				if (aword.size() >= W2VEC_MAX_STRING)
					aword = aword.substr(0, W2VEC_MAX_STRING - 1);
				memcpy(bword, STR(aword), aword.size());
				bword[aword.size()] = 0;
				word = current->SearchVocab(bword);

				if (idx >= inputwords->Size()) break;

				if (word == -1) continue;
				word_count++;
				if (word == 0) break;
				// The subsampling randomly discards frequent words while keeping the ranking same
				if (current->sample > 0) {
					real ran = (sqrt(current->vocab[word].cn / (current->sample * current->train_words)) + 1) * (current->sample * current->train_words) / current->vocab[word].cn;
					next_random = next_random * learningconstant + 11;
					if (ran < (next_random & 0xFFFF) / (real)65536) continue;
				}
				sen[sentence_length] = word;
				sentence_length++;
				if (sentence_length >= W2VEC_MAX_SENTENCE_LENGTH) break;
			}
			sentence_position = 0;
		}
		if (idx >= inputwords->Size() || (word_count > current->train_words / current->num_threads)) {
			current->word_count_actual += word_count - last_word_count;
			local_iter--;
			if (local_iter == 0) break;
			word_count = 0;
			last_word_count = 0;
			sentence_length = 0;
			idx = baseidx;
			continue;
		}
		word = sen[sentence_position];
		if (word == -1) continue;
		for (c = 0; c < current->layer1_size; c++) neu1[c] = 0;
		for (c = 0; c < current->layer1_size; c++) neu1e[c] = 0;
		next_random = next_random * learningconstant + 11;
		b = next_random % current->window;
		if (current->cbow) {  //train the cbow architecture
			// in -> hidden
			cw = 0;
			for (a = b; a < current->window * 2 + 1 - b; a++) if (a != current->window) {
				c = sentence_position - current->window + a;
				if (c < 0) continue;
				if (c >= sentence_length) continue;
				last_word = sen[c];
				if (last_word == -1) continue;
				for (c = 0; c < current->layer1_size; c++) neu1[c] += current->syn0[c + last_word * current->layer1_size];
				cw++;
			}
			if (cw) {
				for (c = 0; c < current->layer1_size; c++) neu1[c] /= cw;
				if (current->hs) for (d = 0; d < current->vocab[word].codelen; d++) {
					f = 0;
					l2 = current->vocab[word].point[d] * current->layer1_size;
					// Propagate hidden -> output
					for (c = 0; c < current->layer1_size; c++) f += neu1[c] * current->syn1[c + l2];
					if (f <= -W2VEC_MAX_EXP) continue;
					else if (f >= W2VEC_MAX_EXP) continue;
					else f = current->expTable[(int)((f + W2VEC_MAX_EXP) * (W2VEC_EXP_W2VEC_table_size / W2VEC_MAX_EXP / 2))];
					// 'g' is the gradient multiplied by the learning rate
					g = (1 - current->vocab[word].code[d] - f) * current->alpha;
					// Propagate errors output -> hidden
					for (c = 0; c < current->layer1_size; c++) neu1e[c] += g * current->syn1[c + l2];
					// Learn weights hidden -> output
					for (c = 0; c < current->layer1_size; c++) current->syn1[c + l2] += g * neu1[c];
				}
				// NEGATIVE SAMPLING
				if (current->negative > 0) for (d = 0; d < current->negative + 1; d++) {
					if (d == 0) {
						target = word;
						label = 1;
					}
					else {
						next_random = next_random * learningconstant + 11;
						target = current->table[(next_random >> 16) % W2VEC_table_size];
						if (target == 0) target = next_random % (current->vocab_size - 1) + 1;
						if (target == word) continue;
						label = 0;
					}
					l2 = target * current->layer1_size;
					f = 0;
					for (c = 0; c < current->layer1_size; c++) f += neu1[c] * current->syn1neg[c + l2];
					if (f > W2VEC_MAX_EXP) g = (label - 1) * current->alpha;
					else if (f < -W2VEC_MAX_EXP) g = (label - 0) * current->alpha;
					else g = (label - current->expTable[(int)((f + W2VEC_MAX_EXP) * (W2VEC_EXP_W2VEC_table_size / W2VEC_MAX_EXP / 2))]) * current->alpha;
					for (c = 0; c < current->layer1_size; c++) neu1e[c] += g * current->syn1neg[c + l2];
					for (c = 0; c < current->layer1_size; c++) current->syn1neg[c + l2] += g * neu1[c];
				}
				// hidden -> in
				for (a = b; a < current->window * 2 + 1 - b; a++) if (a != current->window) {
					c = sentence_position - current->window + a;
					if (c < 0) continue;
					if (c >= sentence_length) continue;
					last_word = sen[c];
					if (last_word == -1) continue;
					for (c = 0; c < current->layer1_size; c++) current->syn0[c + last_word * current->layer1_size] += neu1e[c];
				}
			}
		}
		else {  //train skip-gram
			for (a = b; a < current->window * 2 + 1 - b; a++) if (a != current->window) {
				c = sentence_position - current->window + a;
				if (c < 0) continue;
				if (c >= sentence_length) continue;
				last_word = sen[c];
				if (last_word == -1) continue;
				l1 = last_word * current->layer1_size;
				for (c = 0; c < current->layer1_size; c++) neu1e[c] = 0;
				// HIERARCHICAL SOFTMAX
				if (current->hs) for (d = 0; d < current->vocab[word].codelen; d++) {
					f = 0;
					l2 = current->vocab[word].point[d] * current->layer1_size;
					// Propagate hidden -> output
					for (c = 0; c < current->layer1_size; c++) f += current->syn0[c + l1] * current->syn1[c + l2];
					if (f <= -W2VEC_MAX_EXP) continue;
					else if (f >= W2VEC_MAX_EXP) continue;
					else f = current->expTable[(int)((f + W2VEC_MAX_EXP) * (W2VEC_EXP_W2VEC_table_size / W2VEC_MAX_EXP / 2))];
					// 'g' is the gradient multiplied by the learning rate
					g = (1 - current->vocab[word].code[d] - f) * current->alpha;
					// Propagate errors output -> hidden
					for (c = 0; c < current->layer1_size; c++) neu1e[c] += g * current->syn1[c + l2];
					// Learn weights hidden -> output
					for (c = 0; c < current->layer1_size; c++) current->syn1[c + l2] += g * current->syn0[c + l1];
				}
				// NEGATIVE SAMPLING
				if (current->negative > 0) for (d = 0; d < current->negative + 1; d++) {
					if (d == 0) {
						target = word;
						label = 1;
					}
					else {
						next_random = next_random * learningconstant + 11;
						target = current->table[(next_random >> 16) % W2VEC_table_size];
						if (target == 0) target = next_random % (current->vocab_size - 1) + 1;
						if (target == word) continue;
						label = 0;
					}
					l2 = target * current->layer1_size;
					f = 0;
					for (c = 0; c < current->layer1_size; c++) f += current->syn0[c + l1] * current->syn1neg[c + l2];
					if (f > W2VEC_MAX_EXP) g = (label - 1) * current->alpha;
					else if (f < -W2VEC_MAX_EXP) g = (label - 0) * current->alpha;
					else g = (label - current->expTable[(int)((f + W2VEC_MAX_EXP) * (W2VEC_EXP_W2VEC_table_size / W2VEC_MAX_EXP / 2))]) * current->alpha;
					for (c = 0; c < current->layer1_size; c++) neu1e[c] += g * current->syn1neg[c + l2];
					for (c = 0; c < current->layer1_size; c++) current->syn1neg[c + l2] += g * current->syn0[c + l1];
				}
				// Learn weights input -> hidden
				for (c = 0; c < current->layer1_size; c++) current->syn0[c + l1] += neu1e[c];
			}
		}
		sentence_position++;
		if (sentence_position >= sentence_length) {
			sentence_length = 0;
			continue;
		}
	}
	free(neu1);
	free(neu1e);
#ifdef WIN32
	Locking _lock(current->mother);
	current->tobejoined[currentid] = false;
	return(0);
#else
	pthread_exit(NULL);
#endif
}


