/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tagger.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef TAGGER_H
#define	TAGGER_H

#include "extractFeature.h"

class tagger {
public:
	int error;
	string modelPath;
	extractFeature ext;
	mdl_t *mdl;
	opt_t opt;

    tagger(string mpath,string brown,string mk);
    tagger(string brown,string mk);
    tagger(string mpath, int flag);
    
    virtual ~tagger();
    bool  dotrain(string &pattern,string &train,string &test);

    bool dotag(vector<string>& wordSequence,vector<string>& predictedTag);
    data_t* fill_rawStruct(const vector< vector <string> > &);
    bool readfile(const string &, vector<string> &, vector<string>& );
    void crfInputFile (const string &,string &);
    bool dolabel(string &testset,string &result);
    
    


  
private:

};

#endif	/* TAGGER_H */

