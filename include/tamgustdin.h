/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgustdin.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgustdin_h
#define tamgustdin_h
#include "tamgu.h"
#include "tamgusvector.h"
#include "tamguivector.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgustdin;
//This typedef defines a type "stdinMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgustdin::*stdinMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamgustdin : public TamguObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<stdinMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging basebin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    istream* is;

    //---------------------------------------------------------------------------------------------------------------------
    Tamgustdin(TamguGlobal* g, Tamgu* parent = NULL) : TamguObject(g, parent) {
        //Do not forget your variable initialisation
        is = &cin;
    }

    Tamgustdin() {
        //Do not forget your variable initialisation
        is = &cin;
    }

    //----------------------------------------------------------------------------------------------------------------------
    Tamgu* Put(Tamgu* v, Tamgu* i, short idthread);
    
    Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread);


    short Type() {
        return Tamgustdin::idtype;
    }

    

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
        return "stdin";
    }

    bool isFile() {
        return true;
    }

    //The next two functions work in conjunction...
    //If your object is very atomic (such as a number or a string)
    //Then when it is passed to a function or stored into a container,
    //it might be worth duplicating it.
    // In that case: duplicateForCall should return true...
    
    //However if your object is complex and probably unique through out the code
    //The duplicateForCall should return false, and Atom should always reduce to a "return this;" only...
    Tamgu* Atom(bool forced=false) {
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
        return new Tamgustdin(NULL);
    }

    TamguIteration* Newiteration(bool direction) {
        return aITERNULL;
    }

    static void AddMethod(TamguGlobal* g, string name, stdinMethod func, unsigned long arity, string infos);
    static bool InitialisationModule(TamguGlobal* g, string version);

    void Methods(Tamgu* v) {

        for (const auto& it : infomethods)
            v->storevalue(it.first);
    }

    string Info(string n) {

        if (infomethods.find(n) != infomethods.end())
            return infomethods[n];
        return "Unknown method";
    }


    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is a sample  of a function that could be implemented for your needs.
    
    Tamgu* MethodEof(Tamgu* context, short idthread, TamguCall* callfunc) {
        Locking _lock(this);
        if (is == NULL || is->eof())
            return aTRUE;
        return aFALSE;
    }

    Tamgu* MethodGet(Tamgu* context, short idthread, TamguCall* callfunc) {
        Locking _lock(this);
        if (is == NULL)
            return globalTamgu->Returnerror("Wrong access to the file", idthread);

        char c = is->get();
        if (context->isNumber())
            return globalTamgu->ProvideConstint(c);
        string s;
        s = c;
        return globalTamgu->Providewithstring(s);
    }

    Tamgu* MethodRead(Tamgu* context, short idthread, TamguCall* callfunc) {
        Locking _lock(this);
        if (is == NULL)
            return globalTamgu->Returnerror("Wrong access to the file", idthread);

        string bf;
        long nb = -1;
        if (callfunc->Size() == 1) {
            nb = callfunc->Evaluate(0, aNULL, idthread)->Integer();
            if (nb != -1) {
                if (context->isVectorContainer()) {
                    Tamgu* vect = Selectasvector(context);
                    readnb(((Tamgusvector*)vect)->values, nb);
                    return vect;
                }

                readnb(bf, nb);
                return globalTamgu->Providewithstring(bf);
            }
        }


        if (context->isNumber())
            return globalTamgu->ProvideConstint(is->get());

        if (context->isContainer()) {
            Tamgu* vect = Selectasvector(context);
            readall(((Tamgusvector*)vect)->values);
            return vect;
        }

        readall(bf);
        return globalTamgu->Providewithstring(bf);

    }

    Tamgu* MethodReadLine(Tamgu* context, short idthread, TamguCall* callfunc) {
        Locking _lock(this);
        if (is == NULL)
            return globalTamgu->Returnerror("Wrong access to the file", idthread);
            

        if (context->isContainer()) {
            Tamgu* vect = Selectaivector(context);
            readline(((Tamguivector*)vect)->values);
            return vect;
        }
        
        string bf;
        readline(bf);
        return globalTamgu->Providewithstring(bf);
        
    }

    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }

    long readnb(string& v, long nb) {
        char fbuffer[4096];
        long i;
        long rd;
        long total = 0;
        while (total != nb) {
            if (is->eof())
                return total;
            is->read(fbuffer, 4096);
            rd = is->gcount();
            total += rd;
            for (i = 0; i < rd; i++)
                v += fbuffer[i];
        }

        return total;
    }

    long readnb(vector<string>& v, long nb) {
        string w;

        char fbuffer[4096];
        long i;
        long rd;
        long total = 0;

        while (total != nb) {
            if (is->eof())
                return total;
            is->read(fbuffer, 4095);
            rd = is->gcount();
            total += rd;
            for (i = 0; i < rd; i++) {
                w = fbuffer[i];
                v.push_back(w);
            }
        }

        return total;
    }

    void readline(string& v) {
        getline(*is, v);
    }
    
    void readline(vector<long>& v) {
        char c = is->get();
        while (!is->eof() && c!=10 && c!=13) {
            v.push_back(c);
            c = is->get();
        }
    }
    
    void readall(string& v) {
        string line;
        while (!is->eof()) {
            getline(*is, line);
            v += line;
        }
    }


    void readall(vector<string>& v) {
        string line;
        while (!is->eof()) {
            getline(*is, line);
            v.push_back(line);
        }
    }


    void Clear() {
        //To set a variable back to empty
        is = NULL;
    }

};
#endif
