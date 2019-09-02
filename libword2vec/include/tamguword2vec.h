/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguword2vec.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamguword2vec_h
#define tamguword2vec_h

#include <math.h>

#ifdef WIN32
#include <process.h>
#define testfinite _finite
#else
#include <pthread.h>
#define testfinite std::isfinite
#define fscanf_s fscanf
#endif

#include "tamgumap.h"
#include "tamgutreemapsi.h"
#include "tamgumapsf.h"

#define W2VEC_MAX_STRING 512
#define W2VEC_EXP_W2VEC_table_size 1000
#define W2VEC_MAX_EXP 6
#define W2VEC_MAX_SENTENCE_LENGTH 1000
#define W2VEC_MAX_CODE_LENGTH 40

const long long max_size = 10000;         // max length of strings
const long long analogy_N = 40;                  // number of closest words that will be shown
const long long distance_N = 40;                  // number of closest words that will be shown
const long long accuracy_N = 1;                   // number of closest a_words
const long long max_w = 50;              // max length of vocabulary entries

const int W2VEC_vocab_hash_size = 30000000;  // Maximum 30 * 0.7 = 21M words in the vocabulary

typedef float real;                    // Precision of float numbers

#ifdef W2VSMALL
static const unsigned long long learningconstant = 2521490391;
#else
static const unsigned long long learningconstant = 25214903917;
#endif


struct W2VEC_vocab_word {
	long long cn;
	int *point;
	char *word, *code, codelen;
};


const int W2VEC_table_size = 1e8;

// Used later for sorting by word counts
int W2VVocabCompare(const void *a, const void *b) {
	return ((struct W2VEC_vocab_word *)b)->cn - ((struct W2VEC_vocab_word *)a)->cn;
}

class Tamguword2vec;

//------------------------------------------------------------------------------------------------------------------------------------
class tamguword2vec {
public:

	Tamguword2vec* mother;
	char train_file[W2VEC_MAX_STRING], output_file[W2VEC_MAX_STRING], save_context_file[W2VEC_MAX_STRING], read_context_file[W2VEC_MAX_STRING];
	char save_vocab_file[W2VEC_MAX_STRING], read_vocab_file[W2VEC_MAX_STRING];
	struct W2VEC_vocab_word *vocab;
	int binary, cbow, debug_mode, window, min_count, num_threads, min_reduce;
	int *vocab_hash;
	long long vocab_max_size, vocab_size, layer1_size;
	long long train_words, word_count_actual, iter, file_size, classes;
	real alpha, starting_alpha, sample;
	real *syn0, *syn1, *syn1neg, *expTable;
	clock_t start;
	int hs, negative;
	int *table;
	float *M;
	char *a_vocab;
	long long a_words, a_size;
	Tamgutreemapsi d_vocab;
	Tamgu* inputwords;
#ifdef WIN32
	vector<bool> tobejoined;
#endif
	
	tamguword2vec(Tamguword2vec* m) {
		mother = m;
		output_file[0] = 0;
		save_context_file[0] = 0;
		read_context_file[0] = 0;
		save_vocab_file[0] = 0;
		read_vocab_file[0] = 0;

		d_vocab.reference = 30000;
		vocab = NULL;
		binary = 0; cbow = 1; debug_mode = 2; window = 5; min_count = 5; num_threads = 12; min_reduce = 1;
		vocab_hash = NULL;
		vocab_max_size = 1000; vocab_size = 0; layer1_size = 100;
		train_words = 0; word_count_actual = 0; iter = 5; file_size = 0; classes = 0;
		alpha = 0.025; sample = 1e-3;
		hs = 0; negative = 5;
		M = NULL;
		a_vocab = NULL;
		inputwords = NULL;
		syn0 = NULL;
		syn1 = NULL;
		syn1neg = NULL;
		expTable = NULL;
	}

	~tamguword2vec() {
		if (vocab != NULL){
			free(vocab);
			free(vocab_hash);
			free(expTable);
		}
		clearvocab();
#ifdef WIN32
		if (syn0 != NULL)
			_aligned_free(syn0);
		if (syn1 != NULL)
			_aligned_free(syn1);
		if (syn1neg != NULL)
			_aligned_free(syn1neg);
#else
		if (syn0 != NULL)
			free(syn0);
		if (syn1 != NULL)
			free(syn1);
		if (syn1neg != NULL)
			free(syn1neg);
#endif
	}

	void clearvocab() {
		if (M != NULL)
			free(M);
		M = NULL;
		if (a_vocab != NULL)
			free(a_vocab);
		a_vocab = NULL;
	}

	Tamgu* init(Tamgu* themap);
	void InitUnigramTable();
	void ReadWord(char *word, FILE *fin);

	// Returns hash value of a word
	int GetWordHash(char *word) {
		unsigned long long a, hash = 0;
		for (a = 0; a < strlen(word); a++) hash = hash * 257 + word[a];
		hash = hash % W2VEC_vocab_hash_size;
		return hash;
	}

	// Returns position of a word in the vocabulary; if the word is not found, returns -1
	int SearchVocab(char *word) {
		unsigned int hash = GetWordHash(word);
		while (1) {
			if (vocab_hash[hash] == -1) return -1;
			if (!strcmp(word, vocab[vocab_hash[hash]].word)) return vocab_hash[hash];
			hash = (hash + 1) % W2VEC_vocab_hash_size;
		}
		return -1;
	}

	// Reads a word and returns its index in the vocabulary
	int ReadWordIndex(FILE *fin) {
		char word[W2VEC_MAX_STRING];
		ReadWord(word, fin);
		if (feof(fin)) return -1;
		return SearchVocab(word);
	}

	// Adds a word to the vocabulary
	int AddWordToVocab(char *word) {
		unsigned int hash, length = strlen(word) + 1;
		if (length > W2VEC_MAX_STRING) length = W2VEC_MAX_STRING;
		vocab[vocab_size].word = (char *)calloc(length, sizeof(char));
		strcpy_s(vocab[vocab_size].word, length, word);
		vocab[vocab_size].cn = 0;
		vocab_size++;
		// Reallocate memory if needed
		if (vocab_size + 2 >= vocab_max_size) {
			vocab_max_size += 1000;
			vocab = (struct W2VEC_vocab_word *)realloc(vocab, vocab_max_size * sizeof(struct W2VEC_vocab_word));
		}
		hash = GetWordHash(word);
		while (vocab_hash[hash] != -1) hash = (hash + 1) % W2VEC_vocab_hash_size;
		vocab_hash[hash] = vocab_size - 1;
		return vocab_size - 1;
	}

	// Sorts the vocabulary by frequency using word counts
	void SortVocab() {
		int a, size;
		unsigned int hash;
		// Sort the vocabulary and keep </s> at the first position
		qsort(&vocab[1], vocab_size - 1, sizeof(struct W2VEC_vocab_word), W2VVocabCompare);
		for (a = 0; a < W2VEC_vocab_hash_size; a++) vocab_hash[a] = -1;
		size = vocab_size;
		train_words = 0;
		for (a = 0; a < size; a++) {
			// Words occuring less than min_count times will be discarded from the vocab
			if ((vocab[a].cn < min_count) && (a != 0)) {
				vocab_size--;
				free(vocab[a].word);
			}
			else {
				// Hash will be re-computed, as after the sorting it is not actual
				hash = GetWordHash(vocab[a].word);
				while (vocab_hash[hash] != -1) hash = (hash + 1) % W2VEC_vocab_hash_size;
				vocab_hash[hash] = a;
				train_words += vocab[a].cn;
			}
		}
		vocab = (struct W2VEC_vocab_word *)realloc(vocab, (vocab_size + 1) * sizeof(struct W2VEC_vocab_word));
		// Allocate memory for the binary tree construction
		for (a = 0; a < vocab_size; a++) {
			vocab[a].code = (char *)calloc(W2VEC_MAX_CODE_LENGTH, sizeof(char));
			vocab[a].point = (int *)calloc(W2VEC_MAX_CODE_LENGTH, sizeof(int));
		}
	}

	// Reduces the vocabulary by removing infrequent tokens
	void ReduceVocab() {
		int a, b = 0;
		unsigned int hash;
		for (a = 0; a < vocab_size; a++) if (vocab[a].cn > min_reduce) {
			vocab[b].cn = vocab[a].cn;
			vocab[b].word = vocab[a].word;
			b++;
		}
		else free(vocab[a].word);
		vocab_size = b;
		for (a = 0; a < W2VEC_vocab_hash_size; a++) vocab_hash[a] = -1;
		for (a = 0; a < vocab_size; a++) {
			// Hash will be re-computed, as it is not actual
			hash = GetWordHash(vocab[a].word);
			while (vocab_hash[hash] != -1) hash = (hash + 1) % W2VEC_vocab_hash_size;
			vocab_hash[hash] = a;
		}
		fflush(stdout);
		min_reduce++;
	}

	// Create binary Huffman tree using the word counts
	// Frequent words will have short uniqe binary codes
	void CreateBinaryTree() {
		long long a, b, i, min1i, min2i, pos1, pos2, point[W2VEC_MAX_CODE_LENGTH];
		char code[W2VEC_MAX_CODE_LENGTH];
		long long *count = (long long *)calloc(vocab_size * 2 + 1, sizeof(long long));
		long long *binary = (long long *)calloc(vocab_size * 2 + 1, sizeof(long long));
		long long *parent_node = (long long *)calloc(vocab_size * 2 + 1, sizeof(long long));
		for (a = 0; a < vocab_size; a++) count[a] = vocab[a].cn;
		for (a = vocab_size; a < vocab_size * 2; a++) count[a] = 1e15;
		pos1 = vocab_size - 1;
		pos2 = vocab_size;
		// Following algorithm constructs the Huffman tree by adding one node at a time
		for (a = 0; a < vocab_size - 1; a++) {
			// First, find two smallest nodes 'min1, min2'
			if (pos1 >= 0) {
				if (count[pos1] < count[pos2]) {
					min1i = pos1;
					pos1--;
				}
				else {
					min1i = pos2;
					pos2++;
				}
			}
			else {
				min1i = pos2;
				pos2++;
			}
			if (pos1 >= 0) {
				if (count[pos1] < count[pos2]) {
					min2i = pos1;
					pos1--;
				}
				else {
					min2i = pos2;
					pos2++;
				}
			}
			else {
				min2i = pos2;
				pos2++;
			}
			count[vocab_size + a] = count[min1i] + count[min2i];
			parent_node[min1i] = vocab_size + a;
			parent_node[min2i] = vocab_size + a;
			binary[min2i] = 1;
		}
		// Now assign binary code to each vocabulary word
		for (a = 0; a < vocab_size; a++) {
			b = a;
			i = 0;
			while (1) {
				code[i] = binary[b];
				point[i] = b;
				i++;
				b = parent_node[b];
				if (b == vocab_size * 2 - 2) break;
			}
			vocab[a].codelen = i;
			vocab[a].point[0] = vocab_size - 2;
			for (b = 0; b < i; b++) {
				vocab[a].code[i - b - 1] = code[b];
				vocab[a].point[i - b] = point[b] - vocab_size;
			}
		}
		free(count);
		free(binary);
		free(parent_node);
	}

	int LearnVocabInContext() {
		char word[W2VEC_MAX_STRING];
		FILE *fin;
		long long a, i;
#ifdef WIN32
		fopen_s(&fin, train_file, "rb");
#else
		fin = fopen(train_file, "rb");
#endif		
		if (fin == NULL) {
			globalTamgu->Returnerror("W2V(001):ERROR: training data file not found!");
			return -1;
		}
		while (1) {
			ReadWord(word, fin);
			if (feof(fin)) break;
			train_words++;
			if ((debug_mode > 1) && (train_words % 100000 == 0)) {
				printf("%lldK%c", train_words / 1000, 13);
				fflush(stdout);
			}
			i = SearchVocab(word);
			if (i == -1) {
				a = AddWordToVocab(word);
				vocab[a].cn = 1;
			}
			else vocab[i].cn++;
			if (vocab_size > W2VEC_vocab_hash_size * 0.7) ReduceVocab();
		}
		SortVocab();
		if (debug_mode > 0) {
			printf("Vocab size: %lld\n", vocab_size);
			printf("Words in train file: %lld\n", train_words);
		}
		file_size = ftell(fin);
		fclose(fin);
		return(0);
	}

	int LearnVocabFromTrainFile() {
		char word[W2VEC_MAX_STRING];
		FILE *fin;
		long long a, i;
		for (a = 0; a < W2VEC_vocab_hash_size; a++) vocab_hash[a] = -1;
#ifdef WIN32
		fopen_s(&fin, train_file, "rb");
#else
		fin = fopen(train_file, "rb");
#endif		
		if (fin == NULL) {
			globalTamgu->Returnerror("W2V(001):ERROR: training data file not found!");
			return -1;
		}
		vocab_size = 0;
		AddWordToVocab((char *)"</s>");
		while (1) {
			ReadWord(word, fin);
			if (feof(fin)) break;
			train_words++;
			if ((debug_mode > 1) && (train_words % 100000 == 0)) {
				printf("%lldK%c", train_words / 1000, 13);
				fflush(stdout);
			}
			i = SearchVocab(word);
			if (i == -1) {
				a = AddWordToVocab(word);
				vocab[a].cn = 1;
			}
			else vocab[i].cn++;
			if (vocab_size > W2VEC_vocab_hash_size * 0.7) ReduceVocab();
		}
		SortVocab();
		if (debug_mode > 0) {
			printf("Vocab size: %lld\n", vocab_size);
			printf("Words in train file: %lld\n", train_words);
		}
		file_size = ftell(fin);
		fclose(fin);
		return 0;
	}

	void FeedVocabFromVector(Tamgu* words) {
		inputwords = words;
		long long sz = inputwords->Size();

		long long a, i, w;
		for (a = 0; a < W2VEC_vocab_hash_size; a++) vocab_hash[a] = -1;

		vocab_size = 0;
		AddWordToVocab((char *)"</s>");
		string aword;
		char baseword[W2VEC_MAX_STRING + 1];
		for (w = 0; w<sz; w++) {
			aword = inputwords->getstring(w);
			if (aword.size() >= W2VEC_MAX_STRING)
				aword = aword.substr(0, W2VEC_MAX_STRING - 1);
			memcpy(baseword, STR(aword), aword.size());
			baseword[aword.size()] = 0;
			train_words++;
			if ((debug_mode > 1) && (train_words % 100000 == 0)) {
				printf("%lldK%c", train_words / 1000, 13);
				fflush(stdout);
			}
			i = SearchVocab(baseword);
			if (i == -1) {
				a = AddWordToVocab(baseword);
				vocab[a].cn = 1;
			}
			else vocab[i].cn++;
			if (vocab_size > W2VEC_vocab_hash_size * 0.7) ReduceVocab();
		}
		SortVocab();
	}

	void SaveVocab() {
		long long i;
		FILE *fo;
#ifdef WIN32
		fopen_s(&fo, save_vocab_file, "wb");
#else
		fo = fopen(save_vocab_file, "wb");
#endif		

		for (i = 0; i < vocab_size; i++) fprintf(fo, "%s %lld\n", vocab[i].word, vocab[i].cn);
		fclose(fo);
	}

	int ReadVocab() {
		long long a, i = 0;
		char c;
		char word[W2VEC_MAX_STRING];
		FILE* fin;

#ifdef WIN32
		fopen_s(&fin, read_vocab_file, "wb");
#else
		fin = fopen(read_vocab_file, "rb");
#endif

		if (fin == NULL) {
			globalTamgu->Returnerror("W2V(002): Vocabulary file not found");
			return -1;
		}
		for (a = 0; a < W2VEC_vocab_hash_size; a++) vocab_hash[a] = -1;
		vocab_size = 0;
		while (1) {
			ReadWord(word, fin);
			if (feof(fin)) break;
			a = AddWordToVocab(word);
			fscanf_s(fin, "%lld%c", &vocab[a].cn, &c);
			i++;
		}

		//If we do not have a context file, then we do not process the train file as all its vocabulary has been extracted into the vocab file
		if (!read_context_file[0]) {
			SortVocab();
#ifdef WIN32
			fopen_s(&fin, train_file, "rb");
#else
			fin = fopen(train_file, "rb");
#endif		
			if (fin == NULL) {
				globalTamgu->Returnerror("W2V(003): ERROR: training data file not found!");
				return -1;
			}

			fseek(fin, 0, SEEK_END);
			file_size = ftell(fin);
			fclose(fin);
			if (debug_mode > 0) {
				printf("Vocab size: %lld\n", vocab_size);
				printf("Words in train file: %lld\n", train_words);
			}
		}
		else {
			if (debug_mode > 0) {
				printf("Vocab size: %lld\n", vocab_size);
			}
		}
		return 0;
	}


	void InitNet() {
		long long a, b;
		unsigned long long next_random = 1;
#ifdef WIN32
		syn0 = (real*)_aligned_malloc((long long)vocab_size * layer1_size * sizeof(real), 128);
#else
		a = posix_memalign((void **)&syn0, 128, (long long)vocab_size * layer1_size * sizeof(real));
#endif
		if (syn0 == NULL) { printf("Memory allocation failed\n"); exit(1); }
		if (hs) {
#ifdef WIN32
			syn1 = (real*)_aligned_malloc((long long)vocab_size * layer1_size * sizeof(real), 128);
#else
			a = posix_memalign((void **)&syn1, 128, (long long)vocab_size * layer1_size * sizeof(real));
#endif
			if (syn1 == NULL) { printf("Memory allocation failed\n"); exit(1); }
			for (a = 0; a < vocab_size; a++) for (b = 0; b < layer1_size; b++)
				syn1[a * layer1_size + b] = 0;
		}
		if (negative>0) {
#ifdef WIN32
			syn1neg = (real*)_aligned_malloc((long long)vocab_size * layer1_size * sizeof(real), 128);
#else
			a = posix_memalign((void **)&syn1neg, 128, (long long)vocab_size * layer1_size * sizeof(real));
#endif
			if (syn1neg == NULL) { printf("Memory allocation failed\n"); exit(1); }
			for (a = 0; a < vocab_size; a++) for (b = 0; b < layer1_size; b++)
				syn1neg[a * layer1_size + b] = 0;
		}
		for (a = 0; a < vocab_size; a++) for (b = 0; b < layer1_size; b++) {
			next_random = next_random * learningconstant + 11;
			syn0[a * layer1_size + b] = (((next_random & 0xFFFF) / (real)65536) - 0.5) / layer1_size;
		}
		CreateBinaryTree();
	}

	int loadcontext(string vectorfile) {
		FILE *f;

#ifdef WIN32
		fopen_s(&f, STR(vectorfile), "rb");
#else
		f = fopen(STR(vectorfile), "rb");
#endif
		if (f == NULL) {
			globalTamgu->Returnerror("W2V(005): Input file not found");
			return -1;
		}

		long long vsz, lsz;
		fscanf_s(f, "%lld", &vsz);
		fscanf_s(f, "%lld", &lsz);
		if (lsz != layer1_size) {
			globalTamgu->Returnerror("W2V(045): Mismatch between context and vocabulary file");
			return -1;
		}
		InitNet();

		long long a, b, c;
		for (a = 0; a < vsz; a++) {
			for (b = 0; b < layer1_size; b++) {
				c = a * layer1_size + b;
				fread(&syn0[c], sizeof(real), 1, f);
				if (syn1 != NULL)
					fread(&syn1[c], sizeof(real), 1, f);
				if (syn1neg != NULL)
					fread(&syn1neg[c], sizeof(real), 1, f);
			}
		}

		fclose(f);
		return(0);
	}


	int TrainModel() {
		inputwords = NULL;
		//printf("Starting training using file %s\n", train_file);
		starting_alpha = alpha;
		if (read_vocab_file[0] != 0)  {
			if (ReadVocab() != 0)
				return -1;

			if (read_context_file[0] != 0) {
				if (LearnVocabInContext() != 0)
					return -1;
			}
		}
		else {
			if (LearnVocabFromTrainFile() != 0)
				return -1;
		}
		if (save_vocab_file[0] != 0) SaveVocab();
		if (output_file[0] == 0) return-1;
		LaunchTrainModel();
		return 0;
	}

	int TrainModelFromVector(Tamgu* v) {
		if (output_file[0] == 0) return-1;
		starting_alpha = alpha;
		FeedVocabFromVector(v);
		if (save_vocab_file[0] != 0) SaveVocab();
		LaunchTrainModel();
		return 0;
	}

	void LaunchTrainModel();

	int loadmodel(string vectorfile, bool normalize) {
		if (M != NULL)
			clearvocab();

		FILE *f;
#ifdef WIN32
		fopen_s(&f, STR(vectorfile), "rb");
#else
		f = fopen(STR(vectorfile), "rb");
#endif
		if (f == NULL) {
			globalTamgu->Returnerror("W2V(005): Input file not found");
			return -1;
		}

		fscanf_s(f, "%lld", &a_words);
		fscanf_s(f, "%lld", &a_size);
		a_vocab = (char *)malloc((long long)a_words * max_w * sizeof(char));
		M = (float *)malloc((long long)a_words * (long long)a_size * sizeof(float));
		if (M == NULL) {
			printf("Cannot allocate memory: %lld MB    %lld  %lld\n", (long long)a_words * a_size * sizeof(float) / 1048576, a_words, a_size);
			exit(-1);
		}

		long long a, b;
		float len;
		for (b = 0; b < a_words; b++) {
			if ((b % 10000) == 0)
				cerr << ".";
			a = 0;
			while (1) {
				a_vocab[b * max_w + a] = fgetc(f);
				if (feof(f) || (a_vocab[b * max_w + a] == ' ')) break;
				if ((a < max_w) && (a_vocab[b * max_w + a] != '\n')) a++;
			}
			a_vocab[b * max_w + a] = 0;

			if (normalize) {
				for (a = 0; a < max_w; a++) a_vocab[b * max_w + a] = toupper(a_vocab[b * max_w + a]);
			}

			for (a = 0; a < a_size; a++) fread(&M[a + b * a_size], sizeof(float), 1, f);
			len = 0;
			for (a = 0; a < a_size; a++) len += M[a + b * a_size] * M[a + b * a_size];
			len = sqrt(len);
			for (a = 0; a < a_size; a++) M[a + b * a_size] /= len;
		}
		fclose(f);
		cerr << endl;
		d_vocab.values.clear();
		for (b = 0; b < a_words; b++)
			d_vocab.values[&a_vocab[b * max_w]] = b;

		return 0;
	}

	int analogy(Tamgu* vwords, Tamgumapsf* resmap) {
		if (M == NULL) {
			globalTamgu->Returnerror("W2V(011): Load a model first");
			return -1;
		}

		if (vwords->Size() < 3) {
			globalTamgu->Returnerror("W2V(006): Requires at least two words for analogy");
			return -1;
		}

		char bestw[analogy_N][max_size];
		char st[100][max_size];
		float dist, len, bestd[analogy_N], vec[max_size];
		long long a, b, c, d, cn, bi[100];

		for (a = 0; a < analogy_N; a++) bestd[a] = 0;
		for (a = 0; a < analogy_N; a++) bestw[a][0] = 0;

		cn = 0;
		int mx = vwords->Size();
		string aword;
		for (cn = 0; cn < mx; cn++) {
			aword = vwords->getstring(cn);
			if (aword.size() >= W2VEC_MAX_STRING)
				aword = aword.substr(0, W2VEC_MAX_STRING - 1);
			memcpy(st[cn], STR(aword), aword.size());
			st[cn][aword.size()] = 0;
		}

		map<string, long>::iterator it;
		for (a = 0; a < mx; a++) {
			it = d_vocab.values.find(st[a]);
			if (it == d_vocab.values.end()) {
				globalTamgu->Returnerror("W2V(008): Out of dictionary word!");
				b = 0;
				break;
			}
			b = it->second;
			bi[a] = b;
		}

		if (b == 0)
			return(-1);

		for (a = 0; a < a_size; a++) vec[a] = M[a + bi[1] * a_size] - M[a + bi[0] * a_size] + M[a + bi[2] * a_size];
		len = 0;
		for (a = 0; a < a_size; a++) len += vec[a] * vec[a];
		len = sqrt(len);
		for (a = 0; a < a_size; a++) vec[a] /= len;
		for (a = 0; a < analogy_N; a++) bestd[a] = 0;
		for (a = 0; a < analogy_N; a++) bestw[a][0] = 0;
		for (c = 0; c < a_words; c++) {
			if (c == bi[0]) continue;
			if (c == bi[1]) continue;
			if (c == bi[2]) continue;
			a = 0;
			for (b = 0; b < mx; b++) if (bi[b] == c) a = 1;
			if (a == 1) continue;
			dist = 0;
			for (a = 0; a < a_size; a++) dist += vec[a] * M[a + c * a_size];
			for (a = 0; a < analogy_N; a++) {
				if (dist > bestd[a]) {
					for (d = analogy_N - 1; d > a; d--) {
						bestd[d] = bestd[d - 1];
						strcpy_s(bestw[d], max_size, bestw[d - 1]);
					}
					bestd[a] = dist;
					strcpy_s(bestw[a], max_size, &a_vocab[c * max_w]);
					break;
				}
			}
		}
		for (a = 0; a < analogy_N; a++)
			resmap->values[bestw[a]] = bestd[a];

		return 0;
	}

	int accuracy(long long threshold, Tamgu* vwords, Tamgumapsf* resmap) {
		if (M == NULL) {
			globalTamgu->Returnerror("W2V(011): Load a model first");
			return -1;
		}

		char bestw[accuracy_N][max_size];
		float dist, bestd[accuracy_N], vec[max_size];
		long long a, b, c, d, b1, b2, b3;
		int TCN, CCN = 0, TACN = 0, CACN = 0, SECN = 0, SYCN = 0, SEAC = 0, SYAC = 0, QID = 0, TQ = 0, TQS = 0;
		TCN = 0;
		TamguIteration* itr = vwords->Newiteration(false);
		Tamgu* subvect;
		agnostring key;

		map<string, long>::iterator it;
		for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
			subvect = itr->IteratorValue();
			if (subvect->Size() != 4) {
				globalTamgu->Returnerror("W2V(030): Expecting sub-vectors of 4 elements");
				return -1;
			}
			for (a = 0; a < accuracy_N; a++) bestd[a] = 0;
			for (a = 0; a < accuracy_N; a++) bestw[a][0] = 0;

			key = subvect->getstring(0);
			key = key.toupper();
			it = d_vocab.values.find(key);
			if (it == d_vocab.values.end())
				continue;
			b1 = it->second;

			key = subvect->getstring(1);
			key = key.toupper();
			it = d_vocab.values.find(key);
			if (it == d_vocab.values.end())
				continue;
			b2 = it->second;

			key = subvect->getstring(2);
			key = key.toupper();
			it = d_vocab.values.find(key);
			if (it == d_vocab.values.end())
				continue;
			b3 = it->second;

			key = subvect->getstring(3);
			key = key.toupper();
			it = d_vocab.values.find(key);
			if (it == d_vocab.values.end())
				continue;
			b = it->second;

			for (a = 0; a < accuracy_N; a++) bestd[a] = 0;
			for (a = 0; a < accuracy_N; a++) bestw[a][0] = 0;
			TQ++;

			for (a = 0; a < a_size; a++) vec[a] = (M[a + b2 * a_size] - M[a + b1 * a_size]) + M[a + b3 * a_size];
			TQS++;
			for (c = 0; c < a_words; c++) {
				if (c == b1) continue;
				if (c == b2) continue;
				if (c == b3) continue;
				dist = 0;
				for (a = 0; a < a_size; a++) dist += vec[a] * M[a + c * a_size];
				for (a = 0; a < accuracy_N; a++) {
					if (dist > bestd[a]) {
						for (d = accuracy_N - 1; d > a; d--) {
							bestd[d] = bestd[d - 1];
							strcpy_s(bestw[d], max_size, bestw[d - 1]);
						}
						bestd[a] = dist;
						strcpy_s(bestw[a], max_size, &a_vocab[c * max_w]);
						break;
					}
				}
			}

			if (key == bestw[0]) {
				CCN++;
				CACN++;
			}
			TCN++;
			TACN++;
		}

		itr->Release();
		if (TCN == 0) TCN = 1;
		resmap->values["accuracy top1"] = CCN / (float)TCN * 100;
		resmap->values["CCN"] = CCN;
		resmap->values["TCN"] = TCN;
		resmap->values["total accuracy"] = CACN / (float)TACN * 100;
		if (!SECN)
			resmap->values["semantic accuracy"] = 0;
		else
			resmap->values["semantic accuracy"] = SEAC / (float)SECN * 100;

		QID++;
		resmap->values["questions"] = TQS;
		resmap->values["total questions"] = TQ;
		resmap->values["ratio questions"] = TQS / (float)TQ * 100;
		return 0;
	}

	float* avector(long wrdidx) {
		float* vec = new float[max_size];
		long long a;
		for (a = 0; a < a_size; a++)
			vec[a] = M[a + wrdidx * a_size];
		return vec;
	}

	float* normalize(float* vec, float& norm) {
		long long a;
		norm = 0;
		for (a = 0; a < a_size; a++) norm += vec[a] * vec[a];
		float* vecret = new float[max_size];
		norm = sqrt(norm);
		for (a = 0; a < a_size; a++) vecret[a] = vec[a] / norm;

		return vecret;
	}

	long long wordindex(string& wrd) {
		if (d_vocab.values.find(wrd) == d_vocab.values.end())
			return -1;
		return d_vocab.values[wrd];
	}

	float dot(float* v1, float* v2) {
		float d = 0;
		long long a;
		for (a = 0; a < a_size; a++)
			d += v1[a] * v2[a];
		return fabs(d);
	}


	float dot(float* v1, string& wrd, float& norm) {
		if (d_vocab.values.find(wrd) == d_vocab.values.end())
			return 0;

		long long a;
		float* v2 = avector(d_vocab.values[wrd]);
		norm = 0;
		float d = 0;
		for (a = 0; a < a_size; a++) {
			d += v1[a] * v2[a];
			norm += v2[a] * v2[a];
		}
		norm = sqrt(norm);
		delete[] v2;
		return fabs(d);
	}

	float dot(float* v1, vector<long>& v2, float& norm) {
		if (v2.size() != a_size)
			return 0;

		float d = 0;
		long long a;
		norm = 0;
		for (a = 0; a < a_size; a++) {
			d += v1[a] * v2[a];
			norm += v2[a] * v2[a];
		}
		norm = sqrt(norm);
		return fabs(d);
	}

	float dot(float* v1, vector<double>& v2, float& norm) {
		if (v2.size() != a_size)
			return 0;

		float d = 0;
		long long a;
		norm = 0;
		for (a = 0; a < a_size; a++) {
			d += v1[a] * v2[a];
			norm += v2[a] * v2[a];
		}
		norm = sqrt(norm);
		return fabs(d);
	}

	float dot(float* v1, vector<Tamgu*>& v2, float& norm) {
		if (v2.size() != a_size)
			return 0;

		float d = 0;
		long long a;
		double v;
		norm = 0;
		for (a = 0; a < a_size; a++) {
			v = v2[a]->Float();
			d += v1[a] * v;
			norm += v*v;
		}
		norm = sqrt(norm);
		return fabs(d);
	}

	float distance(float* vecnorm, long long wrdidx) {
		float dist = 0;
		if (wrdidx != -1) {
			for (long long a = 0; a < a_size; a++)
				dist += vecnorm[a] * M[a + wrdidx * a_size];
		}
		return fabs(dist);
	}

	int distance(string wrd, Tamgumapsf* resmap) {
		char bestw[distance_N][max_size];
		char st[100][max_size];
		float dist, len, bestd[distance_N], vec[max_size];
		long long a, b, c, d, cn, bi[100];

		for (a = 0; a < distance_N; a++) bestd[a] = 0;
		for (a = 0; a < distance_N; a++) bestw[a][0] = 0;

		if (d_vocab.values.find(wrd) == d_vocab.values.end())
			return 0;

		long long idx = d_vocab.values[wrd];

		strcpy_s(st[0], max_size, STR(wrd));
		bi[0] = idx;
		cn = 1;

		len = 0;
		for (a = 0; a < a_size; a++)  {
			vec[a] = M[a + idx * a_size];
			len += vec[a] * vec[a];
		}
		len = sqrt(len);
		for (a = 0; a < a_size; a++) vec[a] /= len;

		for (a = 0; a < distance_N; a++) bestd[a] = -1;
		for (a = 0; a < distance_N; a++) bestw[a][0] = 0;

		for (c = 0; c < a_words; c++) {
			if (c == idx)
				continue;

			dist = 0;
			for (a = 0; a < a_size; a++) dist += vec[a] * M[a + c * a_size];
			for (a = 0; a < distance_N; a++) {
				if (dist > bestd[a]) {
					for (d = distance_N - 1; d > a; d--) {
						bestd[d] = bestd[d - 1];
						strcpy_s(bestw[d], max_size, bestw[d - 1]);
					}
					bestd[a] = dist;
					strcpy_s(bestw[a], max_size, &a_vocab[c * max_w]);
					break;
				}
			}
		}

		for (a = 0; a < distance_N; a++)  {
			if (bestw[a][0] != 0)
				resmap->values[bestw[a]] = bestd[a];
		}

		return 0;
	}

	int distance(Tamgu* vwords, Tamgumapsf* resmap) {
		char bestw[distance_N][max_size];
		char st[100][max_size];
		float dist, len, bestd[distance_N], vec[max_size];
		long long a, b, c, d, cn, bi[100];

		for (a = 0; a < distance_N; a++) bestd[a] = 0;
		for (a = 0; a < distance_N; a++) bestw[a][0] = 0;


		TamguIteration* itr = vwords->Newiteration(false);
		a = 0;
		c = 0;
		for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
			strcpy_s(st[a++], max_size, STR(itr->Valuestring()));
			c++;
		}
		itr->Release();

		map<string, long>::iterator it;
		cn = 0;
		for (a = 0; a < c; a++) {
			it = d_vocab.values.find(st[a]);
			if (it == d_vocab.values.end())
				continue;
			b = it->second;
			bi[cn++] = b;
		}

		for (a = 0; a < a_size; a++) vec[a] = 0;
		for (b = 0; b < cn; b++) {
			if (bi[b] == -1)
				continue;
			for (a = 0; a < a_size; a++)
				vec[a] += M[a + bi[b] * a_size];
		}

		len = 0;
		for (a = 0; a < a_size; a++) len += vec[a] * vec[a];
		len = sqrt(len);
		for (a = 0; a < a_size; a++) vec[a] /= len;
		for (a = 0; a < distance_N; a++) bestd[a] = -1;
		for (a = 0; a < distance_N; a++) bestw[a][0] = 0;
		for (c = 0; c < a_words; c++) {
			a = 0;
			for (b = 0; b < cn; b++) {
				if (bi[b] == c) {
					a = 1;
					break;
				}
			}

			if (a == 1) continue;
			dist = 0;
			for (a = 0; a < a_size; a++) dist += vec[a] * M[a + c * a_size];
			for (a = 0; a < distance_N; a++) {
				if (dist > bestd[a]) {
					for (d = distance_N - 1; d > a; d--) {
						bestd[d] = bestd[d - 1];
						strcpy_s(bestw[d], max_size, bestw[d - 1]);
					}
					bestd[a] = dist;
					strcpy_s(bestw[a], max_size, &a_vocab[c * max_w]);
					break;
				}
			}
		}

		for (a = 0; a < distance_N; a++)  {
			if (bestw[a][0] != 0)
				resmap->values[bestw[a]] = bestd[a];
		}

		return 0;
	}

};

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
//This typedef defines a type "word2vecMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguword2vec::*word2vecMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------
class Tamguword2vec : public TamguObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static hmap<unsigned short, word2vecMethod> methods;
	static hmap<string, string> infomethods;
	static bin_hash<unsigned long> exported;

	static short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	tamguword2vec* w2vec;
	//---------------------------------------------------------------------------------------------------------------------
	Tamguword2vec(TamguGlobal* g = NULL, Tamgu* parent = NULL) : TamguObject(g, parent) {
		//Do not forget your variable initialisation
		w2vec = new tamguword2vec(this);
	}

	~Tamguword2vec() {
		delete w2vec;
	}

	//----------------------------------------------------------------------------------------------------------------------
	Tamgu* Put(Tamgu* v, Tamgu* i, short idthread);

	Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread);


	short Type() {
		return Tamguword2vec::idtype;
	}

	string Typename() {
		return "word2vec";
	}

	//The next two functions work in conjunction...
	//If your object is very atomic (such as a number or a string)
	//Then when it is passed to a function or stored into a container,
	//it might be worth duplicating it.
	// In that case: duplicateForCall should return true...

	//However if your object is complex and probably unique through out the code
	//Then duplicateForCall should return false, and Atom should always reduce to a "return this;" only...
	Tamgu* Atom(bool forced = false) {
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
		return new Tamguword2vec;
	}

	TamguIteration* Newiteration(bool direction);

	static void AddMethod(TamguGlobal* g, string name, word2vecMethod func, unsigned long arity, string infos);
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
	//This SECTION is for your specific implementation...
	//This is a sample  of a function that could be implemented for your needs.

	Tamgu* MethodInit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		Tamgu* kmap = callfunc->Evaluate(0, contextualpattern, idthread);
		return w2vec->init(kmap);
	}

	Tamgu* MethodTrainModel(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		if (callfunc->Size() == 0) {
			if (w2vec->TrainModel() == -1)
				return aFALSE;
			return aTRUE;
		}

		Tamgu* kvect = callfunc->Evaluate(0, contextualpattern, idthread);
		if (kvect->isVectorContainer() == false)
			return globalTamgu->Returnerror("W2V(029): Expecting a vector as input");

		w2vec->TrainModelFromVector(kvect);
		return aTRUE;
	}

	Tamgu* MethodLoadModel(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		bool normalize = false;
		if (callfunc->Size() == 2)
			normalize = callfunc->Evaluate(1, contextualpattern, idthread)->Boolean();

		if (w2vec->loadmodel(callfunc->Evaluate(0, contextualpattern, idthread)->String(), normalize) == -1)
			return aFALSE;
		return aTRUE;
	}

	Tamgu* MethodAnalogy(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		Tamgu* kvect = callfunc->Evaluate(0, contextualpattern, idthread);
		if (kvect->isVectorContainer() == false)
			return globalTamgu->Returnerror("W2V(021): Need a vector as input to analogy");

		Tamgumapsf* resmap;
		if (contextualpattern->Type() == Tamgumapsf::idtype) {
			resmap = (Tamgumapsf*)contextualpattern;
			resmap->Clear();
		}
		else
			resmap = new Tamgumapsf;

		if (w2vec->analogy(kvect, resmap) == -1)
			return aNULL;

		return resmap;
	}

	Tamgu* MethodAccuracy(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		Tamgu* kvect = callfunc->Evaluate(0, contextualpattern, idthread);
		if (kvect->isVectorContainer() == false)
			return globalTamgu->Returnerror("W2V(022): Need a vector as input to accuracy");

		Tamgumapsf* resmap;
		if (contextualpattern->Type() == Tamgumapsf::idtype) {
			resmap = (Tamgumapsf*)contextualpattern;
			resmap->Clear();
		}
		else
			resmap = new Tamgumapsf;

		long long threshold = 30000;
		if (callfunc->Size() == 2)
			threshold = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();

		if (w2vec->accuracy(threshold, kvect, resmap) == -1)
			return aNULL;

		return resmap;
	}

	Tamgu* MethodFeatures(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		Tamgumap* kmap;
		if (contextualpattern->Type() == a_map) {
			kmap = (Tamgumap*)contextualpattern;
			kmap->Clear();
		}
		else
			kmap = new Tamgumap;

		size_t a, b;
		string key;
		long long wrds = w2vec->a_words;
		long long sz = w2vec->a_size;
		Tamgufvector* kvect;

		for (b = 0; b < wrds; b++) {
			key = &w2vec->a_vocab[b * max_w];
			kvect = new Tamgufvector;
			kmap->Push(key, kvect);
			kvect->values.reserve(sz);
			for (a = 0; a < sz; a++)
				kvect->values.push_back(w2vec->M[a + b * sz]);
		}

		return kmap;
	}

	Tamgu* MethodVocabulary(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return &w2vec->d_vocab;
	}

	Tamgu* MethodDistance(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		Tamgu* kvect = callfunc->Evaluate(0, contextualpattern, idthread);


		Tamgumapsf* resmap;
		if (contextualpattern->Type() == Tamgumapsf::idtype) {
			resmap = (Tamgumapsf*)contextualpattern;
			resmap->Clear();
		}
		else
			resmap = new Tamgumapsf;

		if (kvect->isVectorContainer() == false) {
			if (w2vec->distance(kvect->String(), resmap) == -1) {
				resmap->Release();
				return aNULL;
			}
			return resmap;
		}

		if (w2vec->distance(kvect, resmap) == -1) {
			resmap->Release();
			return aNULL;
		}

		return resmap;
	}


	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
	//is returned by the Declaration method.
	Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		return (this->*methods[idname])(contextualpattern, idthread, callfunc);
	}


	void Clean() {
		//Your cleaning code
	}

	string String() {
		return "";
	}

	wstring UString() {
		return L"";
	}

	long Integer() { return 0; }
	short Short() { return 0; }
	double Float() { return 0; }
	bool Boolean() { return false; }
	BLONG Long() { return 0; }
	unsigned char Byte() {
		return 0;
	}

	long Size() { return 0; }

	Tamgu* in(Tamgu* context, Tamgu* a, short idthread) {
		string as = a->String();
		if (w2vec->d_vocab.values.find(as) == w2vec->d_vocab.values.end())
			return aFALSE;
		return aTRUE;
	}

};

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamguw2vector;
//This typedef defines a type "word2vecMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguw2vector::*word2vecItemMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------
class Tamguw2vector : public TamguObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static hmap<unsigned short, word2vecItemMethod> methods;
	static hmap<string, string> infomethods;
	static bin_hash<unsigned long> exported;

	static short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...

	string value;
	Tamguword2vec* w2vreference;
	double threshold;
	long long wordindex;
	float* vect;
	float* vectnorm;
	float norm;


	//---------------------------------------------------------------------------------------------------------------------
	Tamguw2vector(TamguGlobal* g = NULL, Tamgu* parent = NULL) : TamguObject(g, parent) {
		//Do not forget your variable initialisation
		w2vreference = NULL;
		vect = NULL;
		vectnorm = NULL;
		threshold = 0.5;
		wordindex = -1;
		norm = 0;
	}

	~Tamguw2vector() {
		if (vect != NULL)
			delete[] vect;
		if (vectnorm != NULL)
			delete[] vectnorm;
	}

	//----------------------------------------------------------------------------------------------------------------------
	Tamgu* Put(Tamgu* e, Tamgu* i, short idthread) {
		if (e->Type() != idtype)
			return globalTamgu->Returnerror("W2V(109) : Cannot initialize with this value");

		Tamguw2vector* item = (Tamguw2vector*)e;
		if (item->vect != NULL) {
			value = item->value;
			w2vreference = item->w2vreference;
			threshold = item->threshold;
			wordindex = item->wordindex;
			norm = item->norm;
			long sz = item->w2vreference->w2vec->a_size;
			vect = new float[max_size];
			vectnorm = new float[max_size];
			for (long long a = 0; a < w2vreference->w2vec->a_size; a++) {
				vect[a] = item->vect[a];
				vectnorm[a] = item->vectnorm[a];
			}
		}
		return aTRUE;
	}


	Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread);


	short Type() {
		return Tamguw2vector::idtype;
	}

	string Typename() {
		return "word2vec";
	}

	//The next two functions work in conjunction...
	//If your object is very atomic (such as a number or a string)
	//Then when it is passed to a function or stored into a container,
	//it might be worth duplicating it.
	// In that case: duplicateForCall should return true...

	//However if your object is complex and probably unique through out the code
	//Then duplicateForCall should return false, and Atom should always reduce to a "return this;" only...
	Tamgu* Atom(bool forced = false) {
		if (forced || !protect) {
			Tamguw2vector* item = new Tamguw2vector;
			if (vect != NULL) {
				item->value = value;
				item->w2vreference = w2vreference;
				item->threshold = threshold;
				item->wordindex = wordindex;
				item->norm = norm;
				long sz = w2vreference->w2vec->a_size;
				item->vect = new float[max_size];
				item->vectnorm = new float[max_size];
				for (long long a = 0; a < w2vreference->w2vec->a_size; a++) {
					item->vect[a] = vect[a];
					item->vectnorm[a] = vectnorm[a];
				}
			}
			return item;
		}

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
		return new Tamguw2vector;
	}

	TamguIteration* Newiteration(bool direction);

	static void AddMethod(TamguGlobal* g, string name, word2vecItemMethod func, unsigned long arity, string infos);
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
	//This SECTION is for your specific implementation...
	//This is a sample  of a function that could be implemented for your needs.

	Tamgu* MethodInitial(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		//If you do not have any parameters, then your function might return whatever you want... Here itself
		if (callfunc->Size() == 0)
			return this;

		//Your first parameter is at position 0 etc...
		Tamgu* e = callfunc->Evaluate(0, contextualpattern, idthread);
		if (e->Type() != idtype)
			return globalTamgu->Returnerror("W2V(109) : Cannot initialize with this value");

		Tamguw2vector* item = (Tamguw2vector*)e;
		value = item->value;
		w2vreference = item->w2vreference;
		threshold = item->threshold;
		wordindex = item->wordindex;
		norm = item->norm;

		if (e->Reference() == 0) {
			//It has been created just now...			
			vect = item->vect;
			vectnorm = item->vectnorm;
			item->vect = NULL;
			item->vectnorm = NULL;
			return e;
		}

		if (item->vect != NULL) {
			vect = new float[max_size];
			vectnorm = new float[max_size];
			for (long long a = 0; a < w2vreference->w2vec->a_size; a++) {
				vect[a] = item->vect[a];
				vectnorm[a] = item->vectnorm[a];
			}
		}
		return e;
	}
	//---------------------------------------------------------------------------------------------------------------------
	Tamgu* MethodCosine(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		if (vect == NULL || norm == 0)
			return aZERO;

		Tamgu* kfirst = callfunc->Evaluate(0, contextualpattern, idthread);
		float lastdistance = 0;
		if (kfirst->Type() == idtype) {
			Tamguw2vector* kwrd = (Tamguw2vector*)kfirst;
			if (kwrd->vect == NULL || kwrd->norm == 0)
				return aZERO;
			lastdistance = w2vreference->w2vec->dot(vect, kwrd->vect);
			lastdistance /= norm*kwrd->norm;
			return globalTamgu->Providefloat(lastdistance);
		}

		float wnorm = 0;
		if (kfirst->Type() == a_string) {
			string wrd = kfirst->String();
			lastdistance = w2vreference->w2vec->dot(vect, wrd, wnorm);
			if (wnorm == 0)
				return aZERO;

			lastdistance /= norm*wnorm;
			return globalTamgu->Providefloat(lastdistance);
		}

		if (kfirst->Type() == a_ivector) {
			lastdistance = w2vreference->w2vec->dot(vect, ((Tamguivector*)kfirst)->values, wnorm);
			lastdistance /= norm*wnorm;
			return globalTamgu->Providefloat(lastdistance);
		}

		if (kfirst->Type() == a_fvector) {
			lastdistance = w2vreference->w2vec->dot(vect, ((Tamgufvector*)kfirst)->values, wnorm);
			if (wnorm == 0)
				return aZERO;
			lastdistance /= norm*wnorm;
			return globalTamgu->Providefloat(lastdistance);
		}

		if (kfirst->Type() == a_vector) {
			lastdistance = w2vreference->w2vec->dot(vect, ((Tamguvector*)kfirst)->values, wnorm);
			if (wnorm == 0)
				return aZERO;
			lastdistance /= norm*wnorm;
			return globalTamgu->Providefloat(lastdistance);
		}

		return aZERO;
	}

	Tamgu* MethodDistance(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		if (vect == NULL || norm == 0)
			return aZERO;

		float lastdistance = 0;

		Tamgu* kfirst = callfunc->Evaluate(0, contextualpattern, idthread);
		if (kfirst->Type() == idtype) {
			Tamguw2vector* kwrd = (Tamguw2vector*)kfirst;
			if (kwrd->wordindex != -1 && wordindex != -1) {
				lastdistance = w2vreference->w2vec->distance(vectnorm, kwrd->wordindex);
				return globalTamgu->Providefloat(lastdistance);
			}

			if (kwrd->vect == NULL || kwrd->norm == 0)
				return aZERO;

			lastdistance = w2vreference->w2vec->dot(vect, kwrd->vect);
			lastdistance /= norm*kwrd->norm;
			return globalTamgu->Providefloat(lastdistance);
		}

		if (kfirst->Type() == a_string) {
			string wrd = kfirst->String();
			long long idx = w2vreference->w2vec->wordindex(wrd);
			if (idx == -1)
				return aZERO;

			if (wordindex != -1) {
				lastdistance = w2vreference->w2vec->distance(vectnorm, idx);
				return globalTamgu->Providefloat(lastdistance);
			}

			float wnorm = 0;
			lastdistance = w2vreference->w2vec->dot(vect, wrd, wnorm);
			if (wnorm == 0)
				return aZERO;

			lastdistance /= norm*wnorm;
			return globalTamgu->Providefloat(lastdistance);
		}

		return globalTamgu->Returnerror("W2V(111): Unknown item");
	}

	Tamgu* MethodDot(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		Tamgu* kfirst = callfunc->Evaluate(0, contextualpattern, idthread);
		float d;
		if (kfirst->Type() == idtype) {
			Tamguw2vector* kwrd = (Tamguw2vector*)kfirst;
			d = w2vreference->w2vec->dot(vect, kwrd->vect);
			return globalTamgu->Providefloat(d);
		}

		float wnorm = 0;
		if (kfirst->Type() == a_string) {
			string wrd = kfirst->String();
			d = w2vreference->w2vec->dot(vectnorm, wrd, wnorm);
			return globalTamgu->Providefloat(d);
		}

		if (kfirst->Type() == a_ivector) {
			d = w2vreference->w2vec->dot(vect, ((Tamguivector*)kfirst)->values, wnorm);
			return globalTamgu->Providefloat(d);
		}

		if (kfirst->Type() == a_fvector) {
			d = w2vreference->w2vec->dot(vect, ((Tamgufvector*)kfirst)->values, wnorm);
			return globalTamgu->Providefloat(d);
		}

		if (kfirst->Type() == a_vector) {
			d = w2vreference->w2vec->dot(vect, ((Tamguvector*)kfirst)->values, wnorm);
			return globalTamgu->Providefloat(d);
		}

		return globalTamgu->Returnerror("W2V(111): Unknown item");
	}


	Tamgu* MethodNorm(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(norm);
	}

	Tamgu* MethodVectorNorm(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		Tamgu* kvect = Selectafvector(contextualpattern);
		if (vect == NULL)
			return kvect;

		kvect->Reserve(w2vreference->w2vec->a_size);
		for (long long a = 0; a < w2vreference->w2vec->a_size; a++)
			kvect->storevalue(vectnorm[a]);

		return kvect;
	}


	Tamgu* MethodThreshold(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		if (callfunc->Size() == 0)
			return globalTamgu->Providefloat(threshold);
		threshold = callfunc->Evaluate(0, contextualpattern, idthread)->Float();
		return aTRUE;
	}

	//ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
	//is returned by the Declaration method.
	Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		return (this->*methods[idname])(contextualpattern, idthread, callfunc);
	}


	void Clean() {
		//Your cleaning code
	}

	//How to interprete your class according to the basic following types:
	string String() {
		stringstream v;
		v << value << "_" << wordindex;
		return v.str();
	}
	long Integer() { return norm; }
	double Float() { return norm; }
	bool Boolean() { return true; }

	bool initialization(Tamguword2vec* ref) {
		if (vect == NULL) {
			w2vreference = ref;
			vect = new float[max_size];
			vectnorm = new float[max_size];
			for (long long a = 0; a < w2vreference->w2vec->a_size; a++) {
				vect[a] = 0;
				vectnorm[a] = 0;
			}
			return true;
		}
		if (w2vreference != ref)
			return false;
		return true;
	}

	Tamgu* plus(Tamgu* a, Tamgu* b, Tamgu* context, bool autoself) {
		if (b->Type() != idtype || a->Type() != idtype)
			return aNULL;

		Tamguw2vector* item = (Tamguw2vector*)b;

		long long i;
		if (autoself) {
			if (!initialization(item->w2vreference))
				return aNULL;

			norm = 0;
			for (i = 0; i < w2vreference->w2vec->a_size; i++) {
				vect[i] += item->vect[i];
				norm += vect[i] * vect[i];
			}
			if (norm != 0) {
				norm = sqrt(norm);
				for (i = 0; i < w2vreference->w2vec->a_size; i++)
					vectnorm[i] = vect[i] / norm;
			}
			wordindex = -1;
			value += "&";
			value += item->value;
			return this;
		}

		Tamguw2vector* base = (Tamguw2vector*)a;
		if (base->w2vreference != item->w2vreference)
			return aNULL;

		Tamguw2vector* res = new Tamguw2vector;
		res->initialization(item->w2vreference);
		for (i = 0; i < item->w2vreference->w2vec->a_size; i++) {
			res->vect[i] = base->vect[i] + item->vect[i];
			res->norm += res->vect[i] * res->vect[i];
		}
		if (res->norm != 0) {
			res->norm = sqrt(res->norm);
			for (i = 0; i < item->w2vreference->w2vec->a_size; i++)
				res->vectnorm[i] = res->vect[i] / res->norm;
		}
		res->value = base->value;
		res->value += "&";
		res->value += item->value;
		return res;
	}

	Tamgu* minus(Tamgu* a, Tamgu* b, Tamgu* context, bool autoself) {
		if (b->Type() != idtype || a->Type() != idtype)
			return aNULL;

		Tamguw2vector* item = (Tamguw2vector*)b;

		long long i;
		if (autoself) {
			if (!initialization(item->w2vreference))
				return aNULL;

			norm = 0;
			for (i = 0; i < w2vreference->w2vec->a_size; i++) {
				vect[i] -= item->vect[i];
				norm += vect[i] * vect[i];
			}
			if (norm != 0) {
				norm = sqrt(norm);
				for (i = 0; i < w2vreference->w2vec->a_size; i++)
					vectnorm[i] = vect[i] / norm;
			}
			wordindex = -1;
			value += "&";
			value += item->value;
			return this;
		}

		Tamguw2vector* base = (Tamguw2vector*)a;
		if (base->w2vreference != item->w2vreference)
			return aNULL;

		Tamguw2vector* res = new Tamguw2vector;
		res->initialization(item->w2vreference);
		for (i = 0; i < item->w2vreference->w2vec->a_size; i++) {
			res->vect[i] = base->vect[i] - item->vect[i];
			res->norm += res->vect[i] * res->vect[i];
		}
		if (res->norm != 0) {
			res->norm = sqrt(res->norm);
			for (i = 0; i < item->w2vreference->w2vec->a_size; i++)
				res->vectnorm[i] = res->vect[i] / res->norm;
		}
		res->value = base->value;
		res->value += "<-";
		res->value += item->value;
		return res;
	}

	//Here, we use the distance to check if two elements are the same...
	Tamgu* same(Tamgu* a) {
		if (a == this)
			return aTRUE;

		if (a == aNULL || a == aNOELEMENT) {
			if (w2vreference == NULL || w2vreference->w2vec == NULL)
				return aTRUE;
			return aFALSE;
		}

		float lastdistance = 0;
		if (a->Type() == idtype) {
			Tamguw2vector* w = (Tamguw2vector*)a;
			if (w->w2vreference != w2vreference)
				return aFALSE;

			if (wordindex != -1 && w->wordindex == wordindex)
				return aTRUE;

			if (w->wordindex != -1) {
				lastdistance = w2vreference->w2vec->distance(vectnorm, w->wordindex);
				if (lastdistance >= threshold)
					return aTRUE;
				return aFALSE;
			}

			if (w->vect == NULL || w->norm == 0 || norm == 0)
				return aFALSE;

			lastdistance = w2vreference->w2vec->dot(vect, w->vect);
			lastdistance /= norm*w->norm;
			if (lastdistance >= threshold)
				return aTRUE;
			return aFALSE;
		}

		string wrd = a->String();
		long long idx = w2vreference->w2vec->wordindex(wrd);
		if (idx == -1)
			return aFALSE;
		if (wordindex == idx)
			return aTRUE;

		lastdistance = w2vreference->w2vec->distance(vectnorm, idx);
		if (lastdistance >= threshold)
			return aTRUE;
		return aFALSE;
	}


	Tamgu* Vector(short idthread) {
		Tamgufvector* kvect = new Tamgufvector;

		kvect->Reserve(w2vreference->w2vec->a_size);
		for (long long a = 0; a < w2vreference->w2vec->a_size; a++)
			kvect->storevalue(vect[a]);

		return kvect;
	}


};


#endif
