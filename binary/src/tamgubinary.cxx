/*
*  Tamgu (탐구)
    *
* Copyright 2019-present NAVER Corp.
* under BSD 3-clause
*/
/* --- CONTENTS ---
Project    : Tamgu (탐구)
    Version    : See tamgu.cxx for the version number
filename   : binary.cxx
Date       :
Purpose    :
Programmer : Claude ROUX (claude.roux@naverlabs.com)
    Reviewer   :
*/

#include "tamgu.h"
#include "tamguversion.h"
#include "globaltamgu.h"

#include "tamguint.h"
#include "tamgustring.h"
#include "tamguivector.h"

//We create a long string for a tamgu program (see binary.tmg)
string tamgucode() {
    string code;
    
    //Compute 2 time i
    code = "function calculus(int i) {";
    code += "return 2i;";
    code += "}";
    //Conc added to a string
    code += "function conc(string s) {";
    code += "s += \"_Added\";";
    code += "return s;";
    code += "}";
    //Store random values in a vector of float
    //vector<long> in C++
    code += "function rand() {";
    code += "long f;";
    code += "ivector fv;";
    code += "for (int i in <1000>) {";
    code += "f=random(1000.00);";
    code += "fv.push(f);";
    code += "}";
    code += "return fv;";
    code += "}";
    return code;
}

/*
 All Tamgu objects exposes:
 
 String()   to return a std::string object
 UString()  to return a std::wstring object
 Integer()  to return a long object
 Float()    ro return a double object
 
 */

long calculus(TamguCode* tcd, long val) {
    vector<Tamgu*> params;
    Tamguint tval(val);
    tval.Setreference();
    params.push_back(&tval);
    
    Tamgu* rval = TamguExecute(tcd,"calculus",params);
    //Since there is no garbage collector in Tamgu, tval will be naturally destroyed
    //However this is not the case of rval...
    //First, we get rval value
    val = rval->Integer();
    //Then we get rid of rval...
    rval->Release();
    return val;
}

//We concatenate a value
string conc(TamguCode* tcd, string val) {
    vector<Tamgu*> params;
    Tamgustring tval(val);
    tval.Setreference();
    params.push_back(&tval);
    
    Tamgu* rval = TamguExecute(tcd,"conc",params);
    //Since there is no garbage collector in Tamgu, tval will be naturally destroyed
    //However this is not the case of rval...
    //First, we get rval value, we use String here
    val = rval->String();
    //Then we get rid of rval...
    rval->Release();
    return val;
}


//We return vector of long
void vrand(TamguCode* tcd, vector<long>& val) {
    vector<Tamgu*> params;

    //params is empty
    Tamgu* rval = TamguExecute(tcd,"rand",params);
    //Since there is no garbage collector in Tamgu, tval will be naturally destroyed
    //However this is not the case of rval...
    //First, we get rval value
    //We know that rval is Tamguivector (see tamguivector.h)
    //whose inner value is a vector of long
    val = ((Tamguivector*)rval)->values;
    //Then we get rid of rval...
    rval->Release();
}


//Our initialisation.
//TamguCompile returns a handler to a code space (each file is compiled into a code space)
//We return that code space
TamguCode* initialisation() {
    const char* MAIN="MAIN\0";
    string code = tamgucode();
    TamguCreate();
    long idcode = TamguCompile(code, MAIN, false);
    return TamguCodeSpace(idcode);
}


//Usage
int main(int argc, char *argv[]) {
    TamguCode* tcd = initialisation();
    
    cout << calculus(tcd, 20) << endl;
    cout << conc(tcd, "My string") << endl;
    vector<long> vls;
    vrand(tcd, vls);
    
    for (long i = 0; i < vls.size(); i++)
        cout << vls[i] << " ";
    cout << endl;
}


