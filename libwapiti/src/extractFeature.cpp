
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
filename   : extractFeature.cpp
Date       : 2017/04/01
Purpose    :  GUI
Programmer : Claude ROUX (claude.roux@naverlabs.com)
Reviewer   :
*/

/* 
* File:   extractFeature.cpp
* Author: fnoorala
* 
* Created on July 26, 2013, 3:02 PM
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <fstream>
#include <string>
#include <map>
#include <vector>
using namespace std;

#include "tamguboost.h"

string toString(int x) {
	if (x==0)
		return "0";
	char buff[10];
	sprintf(buff,"%d",x);
	return buff;
}

#include "extractFeature.h"

extractFeature::extractFeature(string brown,string mk) {
	//string brownPath="/opt/ParSem/users/Farhad/parSem/ntagger/src/BrownCluster/brownCluster.txt";
	//string mkcls="/opt/ParSem/users/Farhad/parSem/ntagger/src/mkcls/mkcls";
	error=false;
	brownPath=brown;
	mkcls=mk;
	if (initMap(brownPath,brownCluster,0)==false) {
		error=1;
		return;
	}

	if (initMap(mkcls,mkclsCluster,1)==false)
		error=2;
}


extractFeature::~extractFeature() {}

/* features:
* extract features for each word in vector 
*/

void extractFeature::features(const vector<string> &tokenSeq,vector< vector <string> >& crfInput){

	for(int x = 0; x < tokenSeq.size(); x++)
	{   
		//cout<<x<<endl;
		vector<string> temp; 
		string token=tokenSeq[x];
		if(token.length()!=0){

			temp.push_back(token);
			//brown Cluster Features
			vector<string> vBrwon=this->clusterFeatures(brownCluster,token);

			for (unsigned  i=0; i<vBrwon.size(); i++)
				temp.push_back(vBrwon.at(i));

			//MKCLS Cluster Features
			vector<string> vMkcls=this->clusterFeatures(mkclsCluster,token);     

			for (unsigned  i=0; i<vMkcls.size(); i++)
				temp.push_back(vMkcls.at(i));

			//check the token initialize with # (hash-tag)
			temp.push_back(toString(this->hashtag(token)));

			//transform the token to xXShape
			temp.push_back(this->xXshape(token));

			//transform the token to bytes
			temp.push_back(this->charclassshape(token));

			//check the hyphenated pattern in token
			temp.push_back(toString(hyphon(token)));

			//assign the hyph feature to the token
			temp.push_back(toString(this->hyph(token)));

			//assign the capitalization feature to the token
			temp.push_back(cap(token));

			//assign the possessive feature to the token
			temp.push_back(this->posessive(token));

			//check the URL pattern on the token
			temp.push_back(toString(this->url(token)));

			//check the emoticon pattern on the token
			temp.push_back(toString(this->EMO(token)));


			//check the email pattern on the token
			temp.push_back(toString(this->email(token)));

			//check the numberWithCommmas pattern on the token
			temp.push_back(toString(this->numberWithCommas(token)));

			//check the Arrows pattern on the token
			temp.push_back(toString(this->arrows(token)));

			//check the timeLike pattern on the token
			temp.push_back(toString(this->timelike(token)));

			//check the arbitraryAbbreviation pattern on the token
			temp.push_back(toString(this->arbitraryAbbrev(token)));

			//check the embeddedAppostrophe pattern on the token
			temp.push_back(toString(this->embeddedApostrophe(token)));
		}
		crfInput.push_back(temp);
	}
}



string extractFeature::binary(unsigned x){
	std::string s;
	do
	{
		s.push_back('0' + (x & 1));
	} while (x >>= 1);
	//std::reverse(s.begin(), s.end());
	return s;

}

//initMap Method : reads the cluster file and return map<key:word;value:clusterCode>
bool extractFeature::initMap(const string &path,map<string, string>& Cluster,bool mkcls){
	string line;    
	ifstream myfile;
	myfile.open(path.c_str());
	if (myfile.is_open())
	{
		string code;
		string arr[3];
		int i;

		while ( myfile.good() )
		{
			getline (myfile,line);
			i = 0;

            stringstream ssin(line);
			while (ssin.good() && i < 3){
				ssin >> arr[i];
				++i;
			}

			if(mkcls==1){
				if (i>1){
                    code= extractFeature::binary(std::lexical_cast<int> (arr[1]));
					while(code.length()<16){
						code+='0';
					}  

					Cluster.insert(std::pair<string,string>(arr[0],code));
				}
			}
			else{
				code=arr[0];
				while(code.length()<16){
					code+='0';
				} 

				Cluster.insert(std::pair<string,string>(arr[1],code));    
			}

		}
		myfile.close();
		return true;
	}
	return false;
}

//clusterFeatures Method : return cluster code in depths 2,4,6,..,16 for each token 
vector<string> extractFeature::clusterFeatures(const map<string, string> &myMap,const string &token){
	vector<string> clusterFeature;
	string passToCluster=token;
	boost::algorithm::to_lower(passToCluster);

	if ( myMap.find(passToCluster) == myMap.end() ) {
		// not found
		//normalize the token and search again
		static const regex repeat("([\\w])\\1{1,}");
		static const regex doublequota("[“”]");
		static const regex otherquota("[‘’´`]");
		const std::string repl2 = "\"";
		const std::string repl1= "'";
		const std::string repl3 = "$1";
		passToCluster= regex_replace(passToCluster, otherquota, repl1);
		passToCluster= regex_replace(passToCluster, doublequota, repl2);
		passToCluster= regex_replace(passToCluster, repeat, repl3);

		if ( myMap.find(passToCluster) == myMap.end() ){

			for(int a=0 ;  a<9; a++){
				clusterFeature.push_back("0");
			} 
		}
		else{


			string fuzz_st= myMap.find(passToCluster)->second;

			for( int a = 2; a <=18; a = a + 2){
				clusterFeature.push_back(fuzz_st.substr(0,a));
			} 

		}  

	}
	else {

		string st= myMap.find(passToCluster)->second;

		for( int a = 2; a <=18; a = a + 2){
			clusterFeature.push_back(st.substr(0,a));
		}
	}

	return clusterFeature;

}

//hyphon method: check is hyphone in token
int extractFeature::hyphon(const string &token){
	std::size_t found=token.find("_");
	if (found!=std::string::npos)
		return 1;
	else
		return 0;

}

//hyph method : check if token has hyphen if yes returns the size of hyphonated part if no return 0
int extractFeature::hyph(string &s){
	size_t pos = 0;
	std::string token;
	vector<string> result;
	const string delimiter="-";
	if (hyphon(s)==1){
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			result.push_back(s);
			s.erase(0, pos + delimiter.length());
		}
		result.push_back(s);
		return result.size();
	}
	else 
		return 0;
}

//hash-tag method: find hash-tag pattern and return 1 if found else 0
bool extractFeature::hashtag(const string &s){

	static const regex hash("#[\\w\\W]+");
	return regex_match(s, hash);

}

//url method: find URL pattern and return 1 if found else 0
bool extractFeature::url(const string &s){
	string punctChars ;
	string punctSeq ;
	string entity;

	punctChars="['\"“”‘’.?!…,:;]";
	punctSeq = "['\"“”‘’]+|[.?!,…]+|[:;]+";
	entity = "&(?:amp|lt|gt|quot);"; 
	// URLs
	string urlStart1;
	string commonTLDs;
	string  ccTLDs;
	string urlStart2 ;
	string urlBody ;
	string urlExtraCrapBeforeEnd ;
	string urlEnd;



	urlStart1 = "(?:https?://|\\bwww\\.)";
	commonTLDs = "(?:com|org|edu|gov|net|mil|aero|asia|biz|cat|coop|info|int|jobs|mobi|museum|name|pro|tel|travel|xxx)";
	ccTLDs	= "(?:ac|ad|ae|af|ag|ai|al|am|an|ao|aq|ar|as|at|au|aw|ax|az|ba|bb|bd|be|bf|bg|bh|bi|bj|bm|bn|bo|br|bs|bt|bv|bw|by|bz|ca|cc|cd|cf|cg|ch|ci|ck|cl|cm|cn|co|cr|cs|cu|cv|cx|cy|cz|dd|de|dj|dk|dm|do|dz|ec|ee|eg|eh|er|es|et|eu|fi|fj|fk|fm|fo|fr|ga|gb|gd|ge|gf|gg|gh|gi|gl|gm|gn|gp|gq|gr|gs|gt|gu|gw|gy|hk|hm|hn|hr|ht|hu|id|ie|il|im|in|io|iq|ir|is|it|je|jm|jo|jp|ke|kg|kh|ki|km|kn|kp|kr|kw|ky|kz|la|lb|lc|li|lk|lr|ls|lt|lu|lv|ly|ma|mc|md|me|mg|mh|mk|ml|mm|mn|mo|mp|mq|mr|ms|mt|mu|mv|mw|mx|my|mz|na|nc|ne|nf|ng|ni|nl|no|np|nr|nu|nz|om|pa|pe|pf|pg|ph|pk|pl|pm|pn|pr|ps|pt|pw|py|qa|re|ro|rs|ru|rw|sa|sb|sc|sd|se|sg|sh|si|sj|sk|sl|sm|sn|so|sr|ss|st|su|sv|sy|sz|tc|td|tf|tg|th|tj|tk|tl|tm|tn|to|tp|tr|tt|tv|tw|tz|ua|ug|uk|us|uy|uz|va|vc|ve|vg|vi|vn|vu|wf|ws|ye|yt|za|zm|zw)";	
	urlStart2 = "\\b(?:[A-Za-z\\d-])+(?:\\.[A-Za-z0-9]+){0,3}\\.(?:"+commonTLDs+"|"+ccTLDs+")"+"(?:\\."+ccTLDs+")?(?=\\W|$)";
	urlBody = "(?:[^\\.\\s<>][^\\s<>]*?)?";
	urlExtraCrapBeforeEnd = "(?:"+punctChars+"|"+entity+")+?";
	urlEnd = "(?:\\.\\.+|[<>]|\\s|$)";
	static const regex  url("(?:"+urlStart1+"|"+urlStart2+")"+urlBody+"(?=(?:"+urlExtraCrapBeforeEnd+")?"+urlEnd+")");
	return regex_match(s, url);

}

//xXshape method : transform token to other format (x for lower_case, X for upper_case and 9 for number)
string extractFeature::xXshape(const string &s ){

	static const regex upperCase("[A-Z]");
	static const regex lowerCase("[a-z]");
	static const regex digit("[0-9]");
	const std::string rep_Up = "X";
	const std::string rep_Low= "x";
	const std::string rep_Dig = "9";
	string transformed= regex_replace(s, upperCase, rep_Up);
	transformed= regex_replace(transformed, lowerCase, rep_Low);
	transformed= regex_replace(transformed, digit, rep_Dig);
	return transformed;

};

static char ec_testutf8(unsigned char* utf) {
	if ( utf[0] < 0x0080 )
		return 0;
	unsigned char c=utf[0];
	if ((c&0xE0)==0xC0) {
		//We take two bytes
		if ((utf[1]&0xC0)!=0x80)
			return 0;
		return 1;
	}
	//we take three
	if ((c&0xF0)==0xE0) {
		if ((utf[1]&0xC0)!=0x80 || (utf[2]&0xC0)!=0x80)
			return 0;
		return 2;
	}

	int i;
	//we take four
	if ((c&0xF8)==0xF0) {
		for (i=1;i<=3;i++)
			if ((utf[i]&0xC0)!=0x80)
				return 0;
		return 3;
	}

	//we take five
	if ((c&0xFC)==0xF8) {
		for (i=1;i<=4;i++)
			if ((utf[i]&0xC0)!=0x80)
				return 0;
		return 4;
	}

	//we take six
	if ((c&0xFF)==0xFC) {
		for (i=1;i<=5;i++)
			if ((utf[i]&0xC0)!=0x80)
				return 0;
		return 5;
	}
	return 0;
}

static string ec_char_get(unsigned char* m,int& i) {		
	int nb=ec_testutf8(m+i);		
	char str[]={m[i],0,0,0,0,0,0,0};

	if (nb==0)
		return str;

	char cc=m[i+nb+1];
	m[i+nb+1]=0;
	strcpy(str,(char*)(m+i));
	m[i+nb+1]=cc;
	i+=nb;
	return str;
}


//charclassshape : transform string to other string by replacing lowercase letter with 'x' and UpperCase with 'X' and digit with '9'
string extractFeature::charclassshape(const string &s ){
	string charclass;
	string onechar;

	for (int i=0;i<s.size();i++) {
		onechar=ec_char_get((unsigned char*)s.c_str(),i);
		for (int j=0;j<onechar.size();j++){
			int hh= (int)(unsigned char)onechar[j];
			charclass+=toString(hh)+'-';
		}

	}
	charclass=charclass.substr(0,charclass.size()-1);
	return charclass;
}

//cap method : return type of capitalization 
string extractFeature:: cap(const string &s){
	static const regex htt("http://?+");
	int t=0;
	int numChar = 0;
	int numCap = 0;
	for (t=0; t< s.size(); t++) {
		if(isalpha(s[t]))
			numChar +=1 ;
		else
			numChar +=0;
		if(isupper(s[t]))
			numCap +=1 ;
		else
			numCap +=0;
	}
	if(numChar==numCap && numChar!=0)
		if(numChar<=1)
			return("allCap,shortCap");
		else
			return("allCap,longCap");
	else if(numChar >= 2 && numCap==1 && isupper(s[0]))
		return("initCap");
	else if( numCap >=1 &&  numChar >= 2 && (s[0]!='@') &&  (!regex_match(s,htt)))
		return("mixCap");
	else
		if(numChar>0)
			return("nocap");
		else
			return("-");

}

string extractFeature::posessive(const string &token){
	int t=0;
	int numChar = 0;
	for (t=0; t< token.size(); t++) {
		if(isalpha(token[t]))
			numChar +=1;
		else
			numChar +=0;
	}
	if(numChar>1){
		string possSign=token.substr(token.size()-2,token.size());
		boost::algorithm::to_lower(possSign);
		if( possSign=="'s"){

			return("Possisive|"+cap(token));
		}
		else
			return("-");
	}
	else
		return("-");
}


string extractFeature::OR (const char* first, ...){
	const char* str;
	va_list vl;

	str=first;
	va_start(vl,first);
	string prefix="(?:";
	string ret;
	do {
		ret+=prefix+string(str);
		prefix="|";
		str=va_arg(vl,char*);
	} while (str!=NULL);
	ret+=")";
	va_end(vl);
	return ret;
}


bool extractFeature::EMO(const string &s){      
	string NormalEyes = "[:=]";
	string Wink = "[;]";

	string NoseArea = "(|o|O|-)";
	string HappyMouths = "[D\\)\\]]";
	string SadMouths = "[\\(\\[]";
	string Tongue = "[pP]";
	string OtherMouths = "[doO/\\]";
	string Happy_RE = "(\\^_\\^|" + NormalEyes + NoseArea + HappyMouths + ")";
	string Sad_RE = NormalEyes + NoseArea + SadMouths;

	string Wink_RE = Wink + NoseArea + HappyMouths;
	string Tongue_RE = NormalEyes + NoseArea + Tongue;
	string Other_RE = "("+NormalEyes+"|"+Wink+")" + NoseArea + OtherMouths ;

	string Emoticon = "("+NormalEyes+"|"+Wink+")" +NoseArea +"("+Tongue+"|"+OtherMouths+"|"+SadMouths+"|"+HappyMouths+")";

	static const regex  emoticon(Emoticon);
	return regex_match(s, emoticon);


}


bool extractFeature::email(const string &s){      
	string Email = "(?:\\W|^|$)[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,4}(?:\\W|^|$)";
	static const regex  email(Email);

	return regex_match(s, email); 
}

bool extractFeature::numberWithCommas(const string &s){

	string numberWithCommas = string("(?:(?<!\\d)\\d{1,3},)+?\\d{3}")+ string("(?=(?:[^,\\d]|$))");

	static const regex  nuWithCom(numberWithCommas);

	return regex_match(s, nuWithCom);
}

bool extractFeature::arbitraryAbbrev(const string &s){

	// Abbreviations
	string entity = "&(?:amp|lt|gt|quot);"; 
	string boundaryNotDot = string("(?:$|\\s|[“\\u0022?!,:;]|") + string(entity) + ")";
	string aa1 = string("(?:[A-Za-z]\\.){2,}(?=") + string(boundaryNotDot) + ")";
	string aa2 = string("[^A-Za-z](?:[A-Za-z]\\.){1,}[A-Za-z](?=") + string(boundaryNotDot) + ")";
	string standardAbbreviations = "\\b(?:[Mm]r|[Mm]rs|[Mm]s|[Dd]r|[Ss]r|[Jj]r|[Rr]ep|[Ss]en|[Ss]t)\\.";
	string artAbbrev = "(?:" + string(aa1) +"|"+string(aa2) + "|" + string(standardAbbreviations) + ")";
	static const regex  arbitraryAbbrev(artAbbrev);

	return regex_match(s, arbitraryAbbrev);
}

bool extractFeature::arrows(const string &s){
	string arr = "(?:<*[-―—=]*>+|<+[-―—=]*>*)+";
	static const regex  Arrows(arr);

	return regex_match(s, Arrows);

}

bool extractFeature::timelike(const string &s){
	string timeLike = "\\d+(?::\\d+){1,2}";
	static const regex  tm(timeLike);

	return regex_match(s, tm);

}

bool extractFeature::embeddedApostrophe (const string &s){
	string thingsThatSplitWords = "[^\\s\\.,?\"]";

	string EmbeddedApostrophe = thingsThatSplitWords+"+['’′]" + thingsThatSplitWords + "*";  
	// string EmbeddedApostrophe = "\\S+'\\S+"; 
	static const regex  emappost(EmbeddedApostrophe);
	return regex_match(s, emappost);
}



