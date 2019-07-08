
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
filename   : main.cpp
Date       : 2017/04/01
Purpose    :  GUI
Programmer : Claude ROUX (claude.roux@naverlabs.com)
Reviewer   :
*/

/*
 * File:   main.cpp
 * Author: fnoorala
 *
 * Created on July 16, 2013, 10:37 AM
 */

#define STR(x) (char*)x.c_str()
#define USTR(x) (uchar*)x.c_str()
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <utility>
#include <stdio.h>
#include <string.h>
#include <cstdio>  
#include <time.h>


using namespace std;

#include "tagger.h"


/* opt_help:
 *   Just display the help message describing modes and switch.
 */
static void opt_help(const char *pname) {
	static const char msg[] =

		"\t-h | --help      display this help message\n"
		"\n"
		"Feature Extraction mode\n"
		"    %1$s extract [file to load] [input file] [output file]\n"

		"\t-br | --brown    FILE    brown Cluster file to load\n"
		"\t-mk | --mkcls    FILE    mkcls Cluster file to load\n"

		"Train mode:\n"
		"    %1$s train [options] [input data] [model file]\n"


		"\t-p  | --pattern  FILE    patterns for extracting features\n"
		"\t-d  | --devel    FILE    development dataset\n"
		"\t-t  | --trdata   FILE    training dataset\n"
		"\t-m  | --model    FILE    model file as output\n"
		"\n"
		"Label mode:\n"
		"    %1$s label [options] [input data] [output data]\n"

		"\t-m  | --model    FILE    model file to load\n"
		"\t-t  | --test     FILE    test file to lable\n"
		"\t-r  | --result   FILE    result file as output\n"
		"\n"
		;
	fprintf(stderr, msg, pname);
}

int main(int argc, char** argv) {

	const char* pname = argv[0];

	char* brownPath;
	char* mkclspath;
	char* pattern;
	char* input;
	char* train;
	char* output;
	char* model;


	if (argc == 1) {

		opt_help(pname);

	}
	else if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
		opt_help(pname);
		return -1;
	}
	else if (!strcmp(argv[1], "extract")){
		if ((2) >= argc) {
			opt_help(pname);
			cout << endl << "Please specify the required parameters" << endl;
			return -1;
		}
		else{
			argc--, argv++;

			for (int i = 1; i < argc; i++) {
				//check brown cluster file path
				if (!strcmp(argv[i], "-br") || !strcmp(argv[i], "--brown")) {
					if ((i + 1) >= argc) {
						opt_help(pname);
						cout << endl << "Please specify a filename for brown cluster" << endl;
						return -1;
					}
					else{

						brownPath = argv[++i];
					}
					continue;
				}
				if (!strcmp(argv[i], "-mk") || !strcmp(argv[i], "--mkcls")) {
					if ((i + 1) >= argc) {
						opt_help(pname);
						cout << endl << "Please specify a filename for mkcl cluster" << endl;
						return -1;
					}
					else{
						mkclspath = argv[++i];
					}

					continue;
				}
				if (!strcmp(argv[i], "-in") || !strcmp(argv[i], "--input")) {
					if ((i + 1) >= argc) {
						opt_help(pname);
						cout << endl << "Please specify the input filename" << endl;
						return -1;
					}
					else{
						input = argv[++i];

					}
					continue;
				}
				if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--output")) {
					if ((i + 1) >= argc) {
						opt_help(pname);
						cout << endl << "Please specify the output filename" << endl;
						return -1;
					}
					else{
						output = argv[++i];
					}
				}

				tagger* tg = new tagger(brownPath, mkclspath);

				string in = input;
				string out = output;

				tg->crfInputFile(in, out);

			}
		}
	}
	else if (!strcmp(argv[1], "label")){
		if ((2) >= argc) {
			opt_help(pname);
			cout << endl << "Please specify the required parameters" << endl;
			return -1;
		}
		else{
			argc--, argv++;
			for (int i = 1; i < argc; i++) {
				//check brown cluster file path
				if (!strcmp(argv[i], "-br") || !strcmp(argv[i], "--brown")) {
					if ((i + 1) >= argc) {
						opt_help(pname);
						cout << endl << "Please specify a filename for brown cluster" << endl;
						return -1;
					}
					else{

						brownPath = argv[++i];
					}
					continue;
				}
				if (!strcmp(argv[i], "-mk") || !strcmp(argv[i], "--mkcls")) {
					if ((i + 1) >= argc) {
						opt_help(pname);
						cout << endl << "Please specify a filename for mkcl cluster" << endl;
						return -1;
					}
					else{
						mkclspath = argv[++i];
					}

					continue;
				}
				if (!strcmp(argv[i], "-m") || !strcmp(argv[i], "--model")) {
					if ((i + 1) >= argc) {
						opt_help(pname);
						cout << endl << "Please specify the model filename" << endl;
						return -1;
					}
					else{
						model = argv[++i];

					}
					continue;
				}
				if (!strcmp(argv[i], "-t") || !strcmp(argv[i], "--test")) {
					if ((i + 1) >= argc) {
						opt_help(pname);
						cout << endl << "Please specify the test filename" << endl;
						return -1;
					}
					else{
						input = argv[++i];
					}

					continue;
				}
				if (!strcmp(argv[i], "-r") || !strcmp(argv[i], "--result")) {
					if ((i + 1) >= argc) {
						opt_help(pname);
						cout << endl << "Please specify the result filename" << endl;
						return -1;
					}
					else{
						output = argv[++i];
					}

				}

				tagger* tg = new tagger(model, 2);

				string in = input;
				string out = output;

				tg->dolabel(in, out);

			}
		}
	}
	else if (!strcmp(argv[1], "train")){
		if ((2) >= argc) {
			opt_help(pname);
			cout << endl << "Please specify the required parameters" << endl;
			return -1;
		}
		else{
			argc--, argv++;
			for (int i = 1; i < argc; i++) {
				//check brown cluster file path
				if (!strcmp(argv[i], "-br") || !strcmp(argv[i], "--brown")) {
					if ((i + 1) >= argc) {
						opt_help(pname);
						cout << endl << "Please specify a filename for brown cluster" << endl;
						return -1;
					}
					else{

						brownPath = argv[++i];
					}
					continue;
				}
				if (!strcmp(argv[i], "-mk") || !strcmp(argv[i], "--mkcls")) {
					if ((i + 1) >= argc) {
						opt_help(pname);
						cout << endl << "Please specify a filename for mkcl cluster" << endl;
						return -1;
					}
					else{
						mkclspath = argv[++i];
					}

					continue;
				}
				if (!strcmp(argv[i], "-p") || !strcmp(argv[i], "--pattern")) {
					if ((i + 1) >= argc) {
						opt_help(pname);
						cout << endl << "Please specify the pattern filename" << endl;
						return -1;
					}
					else{
						pattern = argv[++i];

					}
					continue;
				}
				if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--devel")) {
					if ((i + 1) >= argc) {
						opt_help(pname);
						cout << endl << "Please specify the development filename" << endl;
						return -1;
					}
					else{
						input = argv[++i];
					}

					continue;
				}
				if (!strcmp(argv[i], "-t") || !strcmp(argv[i], "--trdata")) {
					if ((i + 1) >= argc) {
						opt_help(pname);
						cout << endl << "Please specify the training filename" << endl;
						return -1;
					}
					else{
						train = argv[++i];
					}
					continue;
				}
				if (!strcmp(argv[i], "-m") || !strcmp(argv[i], "--model")) {
					if ((i + 1) >= argc) {
						opt_help(pname);
						cout << endl << "Please specify the model filename as an output" << endl;
						return -1;
					}
					else{
						model = argv[++i];
					}

				}

				tagger* tg = new tagger(model, 1);

				string pt = pattern;
				string dev = input;
				string tr = train;


				tg->dotrain(pt, tr, dev);

			}
		}
	}
	else {
		printf("unknown  mode <%s>\n", argv[1]);
		opt_help(pname);
		return -1;
	}


	return 0;
}











