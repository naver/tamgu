/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : extractFeature.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef EXTRACTFEATURE_H
#define	EXTRACTFEATURE_H

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


#define STR(x) (char*)x.c_str()
#define USTR(x) (uchar*)x.c_str()

class extractFeature
{
    public:
		int error;
        extractFeature(string brown,string mk);
        extractFeature() {}
        virtual ~extractFeature();
        vector<string> readfile(const string &,vector<string> & );
        bool initMap(const string &,map<string, string>& Cluster,bool);
        vector<string> clusterFeatures(const map<string, string> & ,const string &);
	int hyphon(const string &);
        int hyph( string &);

        bool hashtag(const string & );
        string xXshape(const string &);
        string charclassshape(const string &);
        string cap(const string  &);
        string posessive(const string &);
        bool url(const string &);

        string  OR (const char* first, ...);

        bool EMO(const string &);
        bool email(const string &);
        bool arbitraryAbbrev(const string &);
        bool numberWithCommas(const string &);
        bool  arrows(const string &);
        bool  timelike(const string & );
        bool  embeddedApostrophe (const string &);
        string binary(unsigned x);
        void features(const vector<string> &,vector< vector <string> >&);

        data_t*  fill_rawStruct(const vector< vector <string> > & );
        void crfInputFile (const string &, string &);
        vector<string> dotag(vector<string> &, string &);

		string brownPath;
		string mkcls;

		map<string, string> brownCluster;
		map<string, string> mkclsCluster;

    protected:
    private:



};

#endif	/* EXTRACTFEATURE_H */


