/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
Project    : Tamgu (탐구)
    Version    : See tamgu.cxx for the version number
filename   : tamgupython.cxx
Date       : 2017/07/04
Purpose    : it is a bidirectional library, allowing on the one hand to  execute Python from within Tamgu, or Tamgu from within Python
Programmer : Claude ROUX (claude.roux@naverlabs.com)
    Reviewer   :
*/

#include "tamgu.h"
#include "globaltamgu.h"
#include "compilecode.h"
#include "tamguversion.h"
#include "tamguvector.h"
#include "tamguint.h"
#include "tamgufloat.h"
#include "tamgulong.h"
#include "tamgumap.h"
#include "tamgubool.h"
#include "tamgustring.h"
#include <thread>
#include <chrono>
#include <atomic>

#include <Python.h>

#ifdef UNIX
#include <signal.h>
#endif

#if PY_MAJOR_VERSION >= 3
#define PyCheck_String PyUnicode_Check
#define ConvertToPythonLong(i) PyLong_FromLong(i)
#define PyCheck_Number PyLong_Check
#define PyAsNumber PyLong_AsLong

PyObject *ConvertToPythonString(string s)
{
    string res;
    s_latin_to_utf8(res, USTR(s), s.size());
    const char *buff = res.c_str();
    return PyUnicode_FromStringAndSize(buff, res.size());
}

#if PY_MINOR_VERSION >= 9
string PyAsString(PyObject *po)
{
    PyObject *utf8_string = PyUnicode_AsUTF8String(po);
    char *c_string = PyBytes_AsString(utf8_string);
    return c_string;
}
#else

string PyAsString(PyObject *po)
{
    string s;
    Py_ssize_t sz = PyUnicode_GET_LENGTH(po);
    Py_UNICODE *ibuff = PyUnicode_AsUnicode(po);
    for (int i = 0; i < sz; i++)
        s += c_unicode_to_utf8(ibuff[i]);
    return s;
}
#endif
#else
#define ConvertToPythonString(s) PyString_FromString(s)
#define ConvertToPythonLong(i) PyInt_FromLong(i)
#define PyCheck_Number PyInt_Check
#define PyAsNumber PyInt_AsLong
#define PyAsString PyString_AsString
#define PyCheck_String PyString_Check

#if PY_MINOR_VERSION == 4
#define Py_ssize_t int
#define OLDPYTHON 1
#endif

#endif

#include "tamgupython.h"

void clean_signal() {
#ifndef WIN32
    signal(SIGINT,NULL);
#endif
}

// We need to declare once again our local definitions.
basebin_hash<pythonMethod> Tamgupython::methods;
static ThreadLock classlock;
static ThreadLock cleanlock;
short Tamgupython::idtype = 0;

//------------------------------------------------------------------------------------------------------------------
// If you want to implement an external library... Otherwise remove it...
extern "C"
{
    Exporting bool pytamgu_InitialisationModule(TamguGlobal *global, string version)
    {
        if (version != TamguVersion())
            return false;

        // Global variables are quite complex to export to dlls
        global->Update();

        return Tamgupython::InitialisationModule(global, version);
    }
}
//------------------------------------------------------------------------------------------------------------------
static Tamgu *toTamgu(PyObject *po)
{
    if (PyBool_Check(po) == 1)
    {
        if (po == Py_True)
            return aTRUE;
        return aFALSE;
    }

    if (PyCheck_Number(po) == 1)
    {
        long l = PyLong_AsLong(po);
        return globalTamgu->Provideint(l);
    }

    if (PyLong_Check(po) == 1)
    {
        Tamgulong *l = (Tamgulong *)globalTamgu->Provideinstance(a_long, 0);
        l->value = PyLong_AsLong(po);
        return l;
    }

    if (PyFloat_Check(po) == 1)
    {
        double d = PyFloat_AsDouble(po);
        return globalTamgu->Providefloat(d);
    }

    if (PyCheck_String(po) == 1)
    {
        string s = PyAsString(po);
        return globalTamgu->Providestring(s);
    }

    PyObject *pelement;
    Tamgu *e;
    if (PyList_Check(po) == 1)
    {
        Tamguvector *kvect = globalTamgu->Providevector();
        Py_ssize_t tail = PyList_GET_SIZE(po);
        for (Py_ssize_t i = 0; i < tail; i++)
        {
            // Puis chacun des objets, kj est l'index de l'element
            pelement = PyList_GetItem(po, i);
            e = toTamgu(pelement);
            if (e == aRAISEERROR)
            {
                kvect->Release();
                return aRAISEERROR;
            }
            kvect->Push(e);
        }
        return kvect;
    }

    if (PyDict_Check(po) == 1)
    {
        Tamgumap *kmap = globalTamgu->Providemap();
        Py_ssize_t pos = 0;
        PyObject *key;
        string kval;
        while (PyDict_Next(po, &pos, &key, &pelement))
        {
            if (key != NULL && pelement != NULL)
            {
                e = toTamgu(key);
                if (e == aRAISEERROR)
                {
                    kmap->Release();
                    return aRAISEERROR;
                }
                kval = e->String();
                e->Release();

                e = toTamgu(pelement);
                if (e == aRAISEERROR)
                {
                    kmap->Release();
                    return aRAISEERROR;
                }
                kmap->pushing(kval, e);
            }
        }
        return kmap;
    }

#ifdef OLDPYTHON
    if (PyUnicode_Check(po))
    {
        Py_ssize_t sz = PyUnicode_GET_DATA_SIZE(po);
        const char *sbuff = PyUnicode_AS_DATA(po);
        string s;
        for (Py_ssize_t i = 0; i < sz; i++)
        {
            if (sbuff[i])
                s += sbuff[i];
        }
        return globalTamgu->Providestring(s);
    }
#elif PY_MINOR_VERSION >= 9
    if (PyUnicode_Check(po))
    {
        string s;
        PyObject *utf8_string = PyUnicode_AsUTF8String(po);
        char *c_string = PyBytes_AsString(utf8_string);
        s = c_string;
        return globalTamgu->Providestring(s);
    }
#else
    if (PyUnicode_Check(po))
    {
        Py_ssize_t sz = PyUnicode_GET_LENGTH(po);
        Py_UNICODE *ibuff = PyUnicode_AsUnicode(po);
        string s;
        for (int i = 0; i < sz; i++)
            s += c_unicode_to_utf8(ibuff[i]);
        return globalTamgu->Providestring(s);
    }
#endif
    PyObject *perr = PyObject_Str(po);
    if (perr != NULL)
    {
        string s = PyAsString(perr);
        e = globalTamgu->Providestring(s);
        Py_DECREF(perr);
        return e;
    }

    return aRAISEERROR;
}

static PyObject *toPython(Tamgu *resultat)
{

    PyObject *pcourant;

    long i;
    if (resultat->isVectorContainer())
    {
        long sz = resultat->Size();
        PyObject *vect = PyList_New(0);
        Tamgu *val;
        for (i = 0; i < sz; i++)
        {
            val = resultat->getvalue(i);
            pcourant = toPython(val);
            val->Release();
            PyList_Insert(vect, i, pcourant);
            Py_DECREF(pcourant);
        }
        return vect;
    }

    PyObject *liste;
    if (resultat->Type() == a_list)
    {
        i = 0;
        liste = PyList_New(0);
        TamguIteration *itr = resultat->Newiteration(false);
        for (itr->Begin(); itr->End() == aFALSE; itr->Next())
        {
            pcourant = toPython(itr->IteratorValue());
            PyList_Insert(liste, i, pcourant);
            Py_DECREF(pcourant);
            i++;
        }
        itr->Release();
        return liste;
    }

    agnostring value;
    if (resultat->isMapContainer())
    {
        liste = PyDict_New();
        TamguIteration *itr = resultat->Newiteration(false);
        for (itr->Begin(); itr->End() == aFALSE; itr->Next())
        {
            pcourant = toPython(itr->IteratorValue());
            value = itr->Keystring();
            PyDict_SetItemString(liste, STR(value), pcourant);
            Py_DECREF(pcourant);
        }
        itr->Release();
        return liste;
    }

    if (resultat->Type() == a_boolean)
    {
        if (resultat == aTRUE)
            Py_RETURN_TRUE;
        Py_RETURN_FALSE;
    }

    if (resultat->Type() == a_int || resultat->Type() == a_long)
    {
        long l = resultat->Integer();
        return ConvertToPythonLong(l);
    }

    if (resultat->isNumber())
    {
        double d = resultat->Float();
        return PyFloat_FromDouble(d);
    }

    value = resultat->String();
#ifdef OLDPYTHON
    return PyUnicode_DecodeUTF8(value.c_str(), value.size(), NULL);
#else
    return PyUnicode_FromString(value.c_str());
#endif
    // return ConvertToPythonString(value.c_str());
}

static string python_error_string()
{
    string err;
    PyObject *ptype = NULL;
    PyObject *pvalue = NULL;
    PyObject *ptraceback = NULL;
    PyErr_Fetch(&ptype, &pvalue, &ptraceback);
    if (pvalue != NULL)
    {
        PyObject *perr = PyObject_Str(pvalue);
        err = PyAsString(perr);
        Py_DECREF(perr);
        Py_XDECREF(pvalue);
    }
    if (ptype != NULL)
        Py_XDECREF(ptype);
    if (ptraceback != NULL)
        Py_XDECREF(ptraceback);

    PyErr_Clear();
    return err;
}

// MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgupython::AddMethod(TamguGlobal *global, string name, pythonMethod func, unsigned long arity, string infos)
{
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
        global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
        return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}

void Tamgupython::Setidtype(TamguGlobal *global)
{
    Locking lock(classlock);
    if (Tamgupython::methods.isEmpty())
        Tamgupython::InitialisationModule(global, "");
}

bool Tamgupython::InitialisationModule(TamguGlobal *global, string version)
{
    methods.clear();

    Tamgupython::idtype = global->Getid("python");

    Tamgupython::AddMethod(global, "setpath", &Tamgupython::MethodSetpath, P_ONE, "setpath(path1,path2...): Set paths to look for Python imports.");
    Tamgupython::AddMethod(global, "run", &Tamgupython::MethodRun, P_ONE | P_TWO | P_THREE, "run(string code, string return_variable): Execute python code");
    Tamgupython::AddMethod(global, "runmodule", &Tamgupython::MethodRunModule, P_TWO | P_THREE, "runmodule(string module, string code, string return_variable): Execute python code in a module");
    Tamgupython::AddMethod(global, "getmodule", &Tamgupython::MethodGetModule, P_ONE | P_TWO, "getmodule(string module, string name): Get all variables or one variable from the module");
    Tamgupython::AddMethod(global, "import", &Tamgupython::MethodImport, P_ONE, "import(string python): import a python file");
    Tamgupython::AddMethod(global, "execute", &Tamgupython::MethodExecute, P_ATLEASTONE, "execute(string funcname,p1,p2...): execute a python function with p1,p2 as parameters");
    Tamgupython::AddMethod(global, "close", &Tamgupython::MethodClose, P_NONE, "close(): close the current session");

    global->newInstance[Tamgupython::idtype] = new Tamgupython(global);
    global->RecordCompatibilities(Tamgupython::idtype);

    return true;
}

void Tamgupython::Clean() {
    Locking lock(cleanlock);
    if (init_python) {
        Py_Finalize();
        init_python = false;
        dictionaries.clear();
        pModule = NULL;
    }
}

Tamgu *Tamgupython::Put(Tamgu *idx, Tamgu *kval, short idthread)
{
    return aTRUE;
}

Tamgu *Tamgupython::Eval(Tamgu *context, Tamgu *idx, short idthread)
{
    return this;
}

// A sample of how to implement a method for your new class...
Tamgu *Tamgupython::MethodSetpath(Tamgu *contextualpattern, short idthread, TamguCall *callfunc)
{

    Locking lock(classlock);
    if (!init_python)
    {
        Py_Initialize();
        init_python = true;
    }

    string path;
    stringstream code;
    code << "import sys\n";
#ifdef WIN32
    string local("\\");
    string with("\\\\");
#endif
    for (int i = 0; i < callfunc->Size(); i++)
    {
        // 0 is the first parameter and so on...
        path = callfunc->Evaluate(i, contextualpattern, idthread)->String();
#ifdef WIN32
        path = s_replacestring(path, local, with);
        code << "sys.path.append('" << path << "')\n";
#else
        code << "sys.path.append('" << path << "')\n";
#endif
    }
    PyRun_SimpleString(code.str().c_str());

    if (PyErr_Occurred())
    {
        string err = "PYT(996):";
        err += python_error_string();
        return globalTamgu->Returnerror(err, idthread);
    }
    // you may return any value of course...
    return aTRUE;
}

Tamgu *Tamgupython::MethodClose(Tamgu *contextualpattern, short idthread, TamguCall *callfunc)
{
    if (init_python == false)
        return aFALSE;

    Locking lock(classlock);
    Clean();
    return aTRUE;
}

Tamgu* Tamgupython::Run(string& code, short idthread, PyObject* py_dict)
{

    PyObject *py_result = PyRun_StringFlags(code.c_str(), Py_file_input, py_dict, py_dict, nullptr);

    if (!py_result)
    {
        if (PyErr_Occurred())
        {
            string return_variable = "PYT(997):";
            return_variable += python_error_string();
            // Imprime l'erreur ou la traite selon vos besoins.
            return globalTamgu->Returnerror(return_variable, idthread);
        }
    }
    else
    {
        Py_DECREF(py_result); // Nettoie le résultat si rien ne s'est mal passé
    }
    return aTRUE;
}

// Function to run the Python script in a separate thread
Tamgu* Tamgupython::Run_elapse(string& code, int elapse_time, short idthread, PyObject* py_dict) {
    static char* import_code = "import signal\nimport time\n";
    static char* func_code = "def timeout_handler(signum, frame):\n  signal.signal(signal.SIGALRM, signal.SIG_DFL)\n  signal.alarm(0)\n  raise TimeoutError(\"Time out\")\n";
    static char* init_code = "signal.signal(signal.SIGALRM, timeout_handler)\nsignal.alarm(";
    const char* clean_code = "signal.signal(signal.SIGALRM, signal.SIG_DFL)\nsignal.alarm(0)\n";
    
    stringstream c;
    c << import_code << func_code << init_code << elapse_time << ")\n";
    string cde = c.str();
    PyRun_StringFlags(cde.c_str(), Py_file_input, py_dict, py_dict, nullptr);

    PyObject* py_result = PyRun_StringFlags(code.c_str(), Py_file_input, py_dict, py_dict, nullptr);

    if (!py_result) {
        if (PyErr_Occurred()) {
            cde = "PYT(997):";
            cde += python_error_string();
            cerr << cde << endl;
            if (cde.find("Time out") == -1)
                PyRun_StringFlags(clean_code, Py_file_input, py_dict, py_dict, nullptr);
            // Handle the error as needed
            return globalTamgu->Returnerror(cde, idthread);
        }
    } else {
        Py_DECREF(py_result); // Clean up the result if nothing went wrong
    }

    //we clean our signal before leaving
    PyRun_StringFlags(clean_code, Py_file_input, py_dict, py_dict, nullptr);
    return aTRUE;
}

Tamgu *Tamgupython::MethodRun(Tamgu *contextualpattern, short idthread, TamguCall *callfunc) {
    Locking lock(classlock);
    if (!init_python) { 
        Py_Initialize();
        init_python = true;
    }

    // 0 is the first parameter and so on...
    Tamgu *kcmd = callfunc->Evaluate(0, contextualpattern, idthread);
    std::string code = kcmd->String();
    std::string return_variable;
    
    int elapse_time = -1;
    if (callfunc->Size() >= 2) {
        kcmd = callfunc->Evaluate(1, contextualpattern, idthread);
        return_variable = kcmd->String();
        if (callfunc->Size() == 3)
            elapse_time = callfunc->Evaluate(2, contextualpattern, idthread)->Int();
    }

    PyObject* py_main = PyImport_AddModule("__main__");
    PyObject* py_dict = PyModule_GetDict(py_main);

    if (code != "") {
        //First, we test if the syntax is correct:
        PyObject *compiled_code = Py_CompileString(code.c_str(), "<string>", Py_file_input);
        if (compiled_code == NULL) {
            return_variable = "PYT(996):";
            return_variable += python_error_string();
            // Imprime l'erreur ou la traite selon vos besoins.
            kcmd = globalTamgu->Returnerror(return_variable, idthread);
        } else {
            Py_DECREF(compiled_code);
            if (elapse_time == -1)
                kcmd = Run(code, idthread, py_dict);
            else {
                struct sigaction old_action;
                sigaction(SIGALRM, nullptr, &old_action);
                kcmd = Run_elapse(code, elapse_time, idthread, py_dict);
                sigaction(SIGALRM, &old_action, nullptr);
            }
        }
        if (kcmd->isError()) {
            clean_signal();
            Clean();
            return kcmd;
        }
    }

    Tamgu* returning_value = aTRUE;
    if (return_variable != "") {
        PyObject* py_result_val = PyDict_GetItemString(py_dict, return_variable.c_str());
        if (py_result_val != NULL) {
            returning_value = toTamgu(py_result_val);
        } else {
            return_variable += ": This Python variable is unknown";
            returning_value = globalTamgu->Returnerror(return_variable, idthread);
        }
    }

    Clean();
    clean_signal();

    // You may return any value of course...
    return returning_value;
}

Tamgu *Tamgupython::MethodRunModule(Tamgu *contextualpattern, short idthread, TamguCall *callfunc)
{

    Locking lock(classlock);
    if (!init_python)
    {
        Py_Initialize();
        init_python = true;
    }

    // 0 is the first parameter and so on...
    Tamgu *module_name = callfunc->Evaluate(0, contextualpattern, idthread);
    Tamgu *kcmd = callfunc->Evaluate(1, contextualpattern, idthread);

    string name = module_name->String();
    string code = kcmd->String();

    PyObject *py_main = PyImport_AddModule("__main__");
    PyObject *py_dict = PyModule_GetDict(py_main);

    string return_variable;
    if (callfunc->Size() == 3)
        return_variable = callfunc->Evaluate(2, contextualpattern, idthread)->String();

    PyObject *local_dict = NULL;

    if (code != "")
    {

        PyObject *compiled_code = Py_CompileString(code.c_str(), "<string>", Py_file_input);
        if (compiled_code == NULL) {
            return_variable = "PYT(996):";
            return_variable += python_error_string();
            // Imprime l'erreur ou la traite selon vos besoins.
            clean_signal();
            return globalTamgu->Returnerror(return_variable, idthread);
        }
        Py_DECREF(compiled_code);

        local_dict = PyDict_New();
        PyObject *py_result = PyRun_StringFlags(code.c_str(), Py_file_input, py_dict, local_dict, nullptr);

        if (!py_result)
        {
            if (PyErr_Occurred())
            {
                Py_DECREF(local_dict);
                return_variable = "PYT(997):";
                return_variable += python_error_string();
                // Imprime l'erreur ou la traite selon vos besoins.
                clean_signal();
                return globalTamgu->Returnerror(return_variable, idthread);
            }
        }
        else
        {
            Py_DECREF(py_result); // Nettoie le résultat si rien ne s'est mal passé
        }
        dictionaries[name] = local_dict;
    }

    Tamgu* return_value = aTRUE;
    if (return_variable != "") {
        
         PyObject* py_result_val = PyDict_GetItemString(local_dict, return_variable.c_str());
         if (py_result_val != NULL) {
            return_value = toTamgu(py_result_val);
         }
         else {
            return_variable += ": This Python variable is unknown";
            return_value = globalTamgu->Returnerror(return_variable, idthread);
         }
    }

    clean_signal();
    // you may return any value of course...
    return return_value;
}

Tamgu *Tamgupython::MethodGetModule(Tamgu *contextualpattern, short idthread, TamguCall *callfunc)
{

    Locking lock(classlock);
    if (!init_python)    
    {
        return globalTamgu->Returnerror("No Python interpreter has been initialized", idthread);
    }

    // 0 is the first parameter and so on...
    string module_name = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    if (dictionaries.find(module_name) == dictionaries.end()) {
        module_name += "is unknown";
        return globalTamgu->Returnerror(module_name, idthread);
    }

    PyObject *local_dict = dictionaries[module_name];
    Tamgu *tmg;

    if (callfunc->Size() == 1)
    {
        Tamgumap *variables = globalTamgu->Providemap();
        // Access local variables in local_dict
        PyObject *local_keys = PyDict_Keys(local_dict);
        Py_ssize_t num_locals = PyList_Size(local_keys);
        for (Py_ssize_t i = 0; i < num_locals; i++)
        {
            PyObject *key = PyList_GetItem(local_keys, i);     // Get each variable name (key)
            PyObject *value = PyDict_GetItem(local_dict, key); // Get the corresponding value

            // Convert the key (variable name) to a string and print the variable name and value
            const char *variable_name = PyUnicode_AsUTF8(key);

            if (variable_name != NULL && value != NULL)
            {
                module_name = variable_name;
                tmg = toTamgu(value);
                variables->pushing(module_name, tmg);
            }
        }

        Py_DECREF(local_keys); // Clean up the keys list

        clean_signal();
        return variables;
    }

    string return_variable = callfunc->Evaluate(1, contextualpattern, idthread)->String();

    if (return_variable != "")
    {
        PyObject *py_result_val = PyDict_GetItemString(local_dict, return_variable.c_str());
        if (py_result_val != NULL)
        {
            return toTamgu(py_result_val);
        }
    }

    return_variable += ": This Python variable is unknown";
    return globalTamgu->Returnerror(return_variable, idthread);
}

Tamgu *Tamgupython::MethodImport(Tamgu *contextualpattern, short idthread, TamguCall *callfunc)
{
    if (pModule != NULL)
        return globalTamgu->Returnerror("PYT(020): Module already imported", idthread);

    Locking lock(classlock);
    if (!init_python)
    {
        Py_Initialize();
        init_python = true;
    }

    // 0 is the first parameter and so on...
    Tamgu *kcmd = callfunc->Evaluate(0, contextualpattern, idthread);
    pythonfilename = kcmd->String();
    if (pythonfilename != "")
    {
        PyObject *pName = ConvertToPythonString(pythonfilename.c_str());
        pModule = PyImport_Import(pName);
        Py_DECREF(pName);
    }

    if (PyErr_Occurred())
    {
        string err = "PYT(998):";
        err += python_error_string();
        return globalTamgu->Returnerror(err, idthread);
    }

    // you may return any value of course...
    return aTRUE;
}

Tamgu *Tamgupython::MethodExecute(Tamgu *contextualpattern, short idthread, TamguCall *callfunc)
{
    if (pModule == NULL)
        return globalTamgu->Returnerror("PYT(002): No Python file in memory", idthread);

    Locking lock(classlock);
    int nbelements = callfunc->Size();
    if (nbelements == 0)
        return globalTamgu->Returnerror("PYT(003): Missing parameters", idthread);
    string funcname = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    PyObject *pFunc = PyObject_GetAttrString(pModule, STR(funcname));
    if (pFunc == NULL || !PyCallable_Check(pFunc))
        return globalTamgu->Returnerror("PYT(004): Unknown Python function", idthread);
    PyObject *pArgs = PyTuple_New(nbelements - 1);
    PyObject *pe;
    if (nbelements > 1)
    {
        for (int i = 1; i < nbelements; i++)
        {
            pe = toPython(callfunc->Evaluate(i, contextualpattern, idthread));
            PyTuple_SetItem(pArgs, i - 1, pe);
        }
    }
    pe = PyObject_CallObject(pFunc, pArgs);
    Py_DECREF(pArgs);
    clean_signal();
    
    if (PyErr_Occurred())
    {
        if (pe != NULL)
            Py_DECREF(pe);
        string err = "PYT(999):";
        err += python_error_string();
        return globalTamgu->Returnerror(err, idthread);
    }
    if (pe != NULL)
    {
        Tamgu *o = toTamgu(pe);
        Py_DECREF(pe);
        if (o == aRAISEERROR)
            return aNULL;
        return o;
    }
    return aTRUE;
}

//------- Python Lib section ---------------------------------
extern "C"
{
    static PyObject *tamgu_load(PyObject *self, PyObject *args)
    {
        char *filename;
        char *pargs;
        int mapping = 0;
        int nbelements = PyTuple_Size(args);
        if (nbelements == 2)
        {
            if (!PyArg_ParseTuple(args, "ss", &filename, &pargs))
            {
                PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
                return ConvertToPythonLong(-1);
            }
        }
        else
        {
            if (!PyArg_ParseTuple(args, "ssi", &filename, &pargs, &mapping))
            {
                PyErr_SetString(PyExc_AttributeError, "Wrong parameters");
                return ConvertToPythonLong(-1);
            }
        }

        TamguCreate(1000);
        TamguSetArguments(pargs);
        string flname(filename);
        int handler = TamguLoad(flname);
        if (handler == -1)
            return ConvertToPythonLong(-100);

        TamguCode *tamgucode = TamguCodeSpace(handler);

        short idthread = globalTamgu->GetThreadid();
        tamgucode->Execute(tamgucode->firstinstruction, idthread);

        if (globalTamgu->Error(globalTamgu->GetThreadid()))
        {
            string message = globalTamgu->Errorstring(idthread);
            cerr << STR(message) << ":" << handler << endl;
            PyErr_SetString(PyExc_AttributeError, STR(message));
            return ConvertToPythonLong(-50);
        }

        if (mapping == 1)
        {
            stringstream code;

            if (tamgucode == NULL)
            {
                PyErr_SetString(PyExc_AttributeError, "Unknown tamgu space");
                return ConvertToPythonLong(-1);
            }

            basebin_hash<Tamgu *>::iterator it;
            TamguMainFrame *amf = (TamguMainFrame *)tamgucode->Mainframe();
            Tamgu *kfunc;
            string name;
            char var;
            for (it = amf->declarations.begin(); it != amf->declarations.end(); it++)
            {
                kfunc = it->second;
                if (kfunc != NULL && kfunc->isFunction())
                {
                    name = globalTamgu->Getsymbol(it->first);
                    code << "def " << name << "(";
                    var = 97;
                    int i;
                    for (i = 0; i < kfunc->Size(); i++)
                    {
                        if (i)
                            code << ",";
                        code << var;
                        var++;
                    }
                    code << "):\n\treturn pytamgu.execute(" << handler << ",'" << name << "'";
                    var = 97;
                    for (i = 0; i < kfunc->Size(); i++)
                    {
                        code << "," << var;
                        var++;
                    }
                    code << ")\n";
                }
            }

            if (code.str().size() != 0)
                PyRun_SimpleString(code.str().c_str());
        }

        return ConvertToPythonLong(handler);
    }

    static PyObject *tamgu_execute(PyObject *self, PyObject *args)
    {
        int nbelements = PyTuple_Size(args);

        if (nbelements < 2)
        {
            PyErr_SetString(PyExc_AttributeError, "Wrong parameters, at least two parameters are required: tamgu handler and function name");
            return ConvertToPythonLong(-1);
        }

        PyObject *pelement;
        pelement = PyTuple_GetItem(args, 0);
        if (!PyCheck_Number(pelement))
        {
            PyErr_SetString(PyExc_AttributeError, "Expecting a tamgu handler");
            return ConvertToPythonLong(-2);
        }

        int handler = PyAsNumber(pelement);

        TamguCode *tamgucode = TamguCodeSpace(handler);
        if (tamgucode == NULL)
        {
            PyErr_SetString(PyExc_AttributeError, "This handler does not match an existing Tamgu space");
            return ConvertToPythonLong(-3);
        }

        pelement = PyTuple_GetItem(args, 1);
        if (!PyCheck_String(pelement))
        {
            PyErr_SetString(PyExc_AttributeError, "Expecting a string");
            return ConvertToPythonLong(-4);
        }

        string functionname = PyAsString(pelement);

        vector<Tamgu *> params;
        Tamgu *ke;
        int i;

        for (i = 2; i < nbelements; i++)
        {
            pelement = PyTuple_GetItem(args, i);
            ke = toTamgu(pelement);
            if (ke == aRAISEERROR)
            {
                for (i = 0; i < params.size(); i++)
                    params[i]->Resetreference();
                PyErr_SetString(PyExc_AttributeError, "No mapping for this python element");
                return ConvertToPythonLong(-5);
            }
            ke->Setreference();
            params.push_back(ke);
        }

        short idthread = globalTamgu->GetThreadid();
        Tamgu *resultat = TamguExecute(tamgucode, functionname, params);

        if (globalTamgu->Error(idthread))
        {
            resultat->Release();
            for (i = 0; i < params.size(); i++)
                params[i]->Resetreference();
            string value = globalTamgu->Errorstring(idthread);
            PyErr_SetString(PyExc_AttributeError, STR(value));
            return ConvertToPythonLong(-6);
        }

        pelement = toPython(resultat);

        resultat->Release();
        for (i = 0; i < params.size(); i++)
            params[i]->Resetreference();
        return pelement;
    }

    static PyObject *tamgu_clean(PyObject *self, PyObject *args)
    {
        TamguExtinguish();
        return ConvertToPythonLong(0);
    }

    static PyObject *pytamguError;

    static PyMethodDef pytamguMethods[] = {

        {"load", tamgu_load, METH_VARARGS, "load(filename,arguments) load a program and return a handler"},
        {"execute", tamgu_execute, METH_VARARGS, "execute(handler,function_name,arguments) Execute an Tamgu function  function, the argument should be a vector of strings"},
        {"clean", tamgu_clean, METH_VARARGS, "clean() Clean the Tamgu space"},
        {NULL, NULL, 0, NULL}};

#if PY_MAJOR_VERSION >= 3
    static struct PyModuleDef pytamgumodule = {
        PyModuleDef_HEAD_INIT,
        "pytamgu", /* name of module */
        NULL,      /* module documentation, may be NULL */
        -1,        /* size of per-interpreter state of the module,
             or -1 if the module keeps state in global variables. */
        pytamguMethods};

    Exporting PyMODINIT_FUNC PyInit_pytamgu(void)
    {
        PyObject *m;

        m = PyModule_Create(&pytamgumodule);
        if (m == NULL)
            return NULL;

        pytamguError = PyErr_NewException("pytamgu.error", NULL, NULL);
        Py_INCREF(pytamguError);
        PyModule_AddObject(m, "error", pytamguError);

        return m;
    }

#else
    Exporting void initpytamgu()
    {
        PyObject *module = Py_InitModule("pytamgu", pytamguMethods);
        PyObject *pDict = PyModule_GetDict(module);
        pytamguError = PyErr_NewException("pytamgu.error", NULL, NULL);
        PyDict_SetItemString(pDict, "error", pytamguError);
    }
#endif
}
