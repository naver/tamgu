/* 
 * File:   puretagger.h
 * Author: Claude Roux
 *
 * Created on November 24, 2015, 11:34 PM
 */

#ifndef PURETAGGER_H
#define	PURETAGGER_H

extern "C" {
#include "decoder.h"
#include "model.h"
#include "options.h"
#include "progress.h"
#include "quark.h"
#include "reader.h"
#include "sequence.h"
#include "tools.h"
#include "trainers.h"
#include "wapiti.h"
}

class pure_tagger {
public:
	string errormsg;
	int error;
	mdl_t *mdl;
	opt_t opt;
	bool init;

	pure_tagger(opt_t* o);
	~pure_tagger();
	bool dotag(char** words, char** results, int);
	void seterror(string msg) {
		errormsg = msg;
		error = -1;
	}
	bool initialisation();
};


#endif


