
/*
*  Tamgu (탐구): NAVER LABS Europe
*
*  Copyright (C) 2017: Tamgu (탐구) Language
* This file can only be used with the Tamgu (탐구) library or the executable
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : Tamgu (탐구)
Version    : See tamgu.cxx for the version number
filename   : puretagger.cpp
Date       : 2017/04/01
Purpose    :  GUI
Programmer : Claude ROUX (claude.roux@naverlabs.com)
Reviewer   :
*/

/* 
 * File:   puretagger.cpp
 * Author: Claude Roux
 * 
 * Created on November 24, 2015, 11:34 PM
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <ostream>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
using namespace std;

#include "puretagger.h"

#ifdef WINDLL
#define Exported __declspec(dllexport)
#else
#define Exported
#endif

extern "C" {
	Exported void* CreatePureTagger(opt_t* o) {
		pure_tagger* pt = new pure_tagger(o);
		return pt;
	}

	Exported void CleanPureTagger(void* pt) {
		if (pt != NULL)
			delete (pure_tagger*)pt;
	}

	Exported bool InitPureTagger(void* tp) {
		if (tp != NULL) {
			pure_tagger* pt = (pure_tagger*)tp;
			if (!pt->initialisation())
				return false;
			return true;
		}
		return false;
	}

	Exported bool LabelPureTagger(void* tp, char** words, char** results, int sz) {
		if (tp != NULL) {
			pure_tagger* pt = (pure_tagger*)tp;
			if (!pt->initialisation())
				return false;
			pt->dotag(words, results, sz);
		}
		return true;
	}
}

//-----------------------------------------------------------------------------------------------
extern "C" {
	void tag_label_vector(mdl_t *mdl, char** words, int nbwords, char** results);
}

pure_tagger::pure_tagger(opt_t* o) : opt(*o) {

	error = 0;
	opt.label = true;
	init = false;
	//set option in model in order to show just label for each token
	//mdl->opt->label = true;
}

pure_tagger::~pure_tagger() {
	if (mdl != NULL)
		mdl_free(mdl);
}


bool pure_tagger::initialisation() {
	if (init)
		return true;

	mdl = mdl_new(rdr_new(opt.maxent));
	mdl->opt = &opt;
	if (mdl->opt->model == NULL) {
		seterror("you must specify a model");
		return false;
	}
	FILE *file = fopen(mdl->opt->model, "r");
	if (file == NULL) {
		seterror("cannot open input model file");
		return false;
	}

	mdl_load(mdl, file);
	init = true;
	return true;
}

bool pure_tagger::dotag(char** words,char** results,int sz) {

	tag_label_vector(mdl, words,sz, results);

	return true;
}

