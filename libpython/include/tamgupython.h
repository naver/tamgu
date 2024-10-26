/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgupython.h
 Date       : 2017/09/01
 Purpose    : TAMGU API to Python 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#ifndef tamgupython_h
#define tamgupython_h

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgupython;
//This typedef defines a type "pythonMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgupython::*pythonMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamgupython : public TamguObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    Exchanging static basebin_hash<pythonMethod> methods;
    
    

    static short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    string pythonfilename;
    PyObject* pModule;

    hmap<string, PyObject*> dictionaries;
    bool init_python;

    //---------------------------------------------------------------------------------------------------------------------
    Tamgupython(TamguGlobal* g, Tamgu* parent = NULL) : TamguObject(g, parent) {
        //Do not forget your variable initialisation
        pModule = NULL;
        pythonfilename = "";
        init_python = false;
    }

    Tamgupython() {
        //Do not forget your variable initialisation
        pModule = NULL;
        pythonfilename = "";
        init_python = false;
    }

    //----------------------------------------------------------------------------------------------------------------------
    Tamgu* Put(Tamgu* v, Tamgu* i, short idthread);
    
    Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread);


    short Type() {
        return Tamgupython::idtype;
    }

    string Typename() {
        return "python";
    }

    //The next two functions work in conjunction...
    //If your object is very atomic (such as a number or a string)
    //Then when it is passed to a function or stored into a container,
    //it might be worth duplicating it.
    // In that case: needNewInstance should return true...
    
    //However if your object is complex and probably unique through out the code
    //The needNewInstance should return false, and Atom should always reduce to a "return this;" only...
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
        return methods.check(n);
    }

    Tamgu* Newinstance(short, Tamgu* f = NULL) {
        return new Tamgupython;
    }

    TamguIteration* Newiteration(bool direction) {
        return aITERNULL;
    }

    static void AddMethod(TamguGlobal* g, string name, pythonMethod func, unsigned long arity, string infos);
    static bool InitialisationModule(TamguGlobal* g, string version);

    
     void Setidtype(TamguGlobal* global);
     void Methods(Tamgu* v) {
            for (const auto& it : globalTamgu->infomethods[idtype])
                 v->storevalue(it.first);
      }

      string Info(string n) {
            if (globalTamgu->infomethods[idtype].find(n) !=  globalTamgu->infomethods[idtype].end())
              return globalTamgu->infomethods[idtype][n];
             return "Unknown method";
    }


    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is a sample  of a function that could be implemented for your needs.
    
    Tamgu* MethodSetpath(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    Tamgu* MethodClose(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);


    Tamgu* MethodRun(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodRunModule(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodGetModule(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);    

    Tamgu* MethodImport(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    Tamgu* MethodExecute(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    Tamgu* Run(string& code, short idthread, PyObject* py_dict);
    Tamgu* Run_elapse(string& code, int elapse_time, short idthread, PyObject* py_dict);
    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*Tamgupython::methods.get(idname))(contextualpattern, idthread, callfunc);
    }

    void Clear(TamguGlobal* g) {
        //To set a variable back to empty
    }

    void Clean();


};
#endif
