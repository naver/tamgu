/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : predicate.cxx
 Date       : 2017/09/01
 Purpose    : Predicate Implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#include "predicate.h"
#include "tamgutaskell.h"
#include <memory>
#include "compilecode.h"
#include "constobjects.h"
#include "tamgutreemap.h"
#include "tamgufvector.h"
#include "tamgusynode.h"
#include "tamgumapss.h"

#ifdef INTELINTRINSICS
#ifdef WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif
#endif

static const char* d_eval = "eval";
static const char* d_kb = "kb";
static const char* d_rule = "rule";
//-------------------------------------------------------------------------------------------------
inline long hmax(long a, long b) {
    return a < b ? b : a;
}
//-------------------------------------------------------------------------------------------------

Tamgu* Tamgu::Getpredicatezone(short idthread) {
    return globalTamgu->Declarator(predicatezone, idthread);
}


//A little bit of explanation.
//To avoid creating useless elements in the binmap, we initialize the actual name of a "^dependency" (code is a_modifydependency)
//with predicatedependency, which allows for a much more compact storage in the declaration map.
//We intercept this a_modifydependency to replace it on the fly with predicatedependency...
bool TamguDependencyAction::Setinvariable(Tamgu* v, TamguDeclaration* dom, short idthread) {
    if (idvar) {
        if (idvar == a_modifydependency) {
            dom->Declare(predicatedependency, v);
            globalTamgu->Storevariable(idthread, predicatedependency, v);
            return true;
        }
        
        dom->Declare(idvar, v);
        globalTamgu->Storevariable(idthread, idvar, v);
    }
    return false;
}

void TamguDependencyAction::Resetintvariable(TamguDeclaration* dom, short idthread) {
    if (idvar) {
        if (idvar == a_modifydependency) {
            dom->declarations.erase(predicatedependency);
            globalTamgu->Removevariable(idthread, predicatedependency);
            return;
        }
        
        dom->declarations.erase(idvar);
        globalTamgu->Removevariable(idthread, idvar);
    }
}


short TamguDependencyAction::Idvar() {
    if (idvar == a_modifydependency)
        return predicatedependency;
    return idvar;
}

bool TamguDependency::Setinvariable(Tamgu* v, TamguDeclaration* dom, short idthread) {
    if (idvar) {
        if (idvar == a_modifydependency) {
            dom->Declare(predicatedependency, v);
            globalTamgu->Storevariable(idthread, predicatedependency, v);
            return true;
        }
        
        dom->Declare(idvar, v);
        globalTamgu->Storevariable(idthread, idvar, v);
    }
    return false;
}

void TamguDependency::Resetintvariable(TamguDeclaration* dom, short idthread) {
    if (idvar) {
        if (idvar == a_modifydependency) {
            dom->declarations.erase(predicatedependency);
            globalTamgu->Removevariable(idthread, predicatedependency);
            return;
        }
        
        dom->declarations.erase(idvar);
        globalTamgu->Removevariable(idthread, idvar);
    }
}

short TamguDependency::Idvar() {
    if (idvar == a_modifydependency)
        return predicatedependency;
    return idvar;
}
//-------------------------------------------------------------------------------------------------
Tamgu* TamguCallFromPredicateRule::Eval(Tamgu* domain, Tamgu* a, short idthread) {
    static VECTE<Tamgu*> bvalues;
    
    TamguFunction* bd = (TamguFunction*)body->Body(idthread);
    TamguVariableDeclaration* p = NULL;
    
    short i, sz = arguments.size();
    bool strict = bd->strict;
    
    VECTE<Tamgu*>* values = &bvalues;
    if (idthread)
        values = new VECTE<Tamgu*>(sz);
    else
        bvalues.clear();
    
    char error = false;
    for (i = 0; i < sz && !error; i++) {
        p = (TamguVariableDeclaration*)bd->parameters[i];
        //The reason why we have a specific case...
        if (p->typevariable == a_instance)
            a = arguments.vecteur[i]->Execute(domain, aNULL, idthread);
        else
            a = arguments.vecteur[i]->Eval(domain, aNULL, idthread);
        if (a == NULL || a == aNOELEMENT)
            error = 2;
        else {
            values->push_back(a);
            error = globalTamgu->Error(idthread);
        }
    }
    
    if (error) {
        for (i = 0; i < values->size(); i++) {
            a = values->vecteur[i];
            if (!a-isError())
                a->Releasenonconst();
        }
        
        if (idthread)
            delete values;
        
        if (error == 2)
            return aNOELEMENT;
        
        return globalTamgu->Errorobject(idthread);
    }
    
    TamguDeclarationLocal* environment = globalTamgu->Providedeclaration(idthread);
    if (globalTamgu->debugmode)
        environment->idinfo = Currentinfo();
    
    
    short type_a = 0, type_p = 0;
    vector<Tamgu*> instances;
    vector<short> names;
    while (bd != NULL) {
        if (bd->parameters.size() != sz) {
            bd = bd->next;
            type_a = 0;
            error = true;
            continue;
        }
        error = false;
        for (i = 0; i < sz && !error; i++) {
            p = (TamguVariableDeclaration*)bd->parameters[i];
            a = values->vecteur[i];
            type_a = a->Type();
            type_p = p->typevariable;
            //The reason why we have a specific case...
            if (type_p == a_instance) {
                names.push_back(p->Name());
                environment->Declaring(names.back(), a);
                instances.push_back(a);
                a->Setreference();
            }
            else
                error = p->Setarguments(environment, a, idthread, strict);
        }
        
        if (!error)
            break;
        
        for (i = 0; i < instances.size(); i++) {
            instances[i]->Resetreference();
        }
        environment->Cleaning();
        instances.clear();
        names.clear();
        bd = bd->next;
    }
        
    
    for (i = 0; i < sz; i++)
        values->vecteur[i]->Releasenonconst();
    
    if (idthread)
        delete values;
    
    if (error) {
        environment->Releasing();
        if (bd == NULL && !type_a) {
            string err = "Could not find a match for: '";
            err += globalTamgu->Getsymbol(Name());
            err += "'";
            return globalTamgu->Returnerror(err, idthread);
        }

        string err = "Check the argument '";
        err += (char)(i+48);
        err += "' of: '";
        err += globalTamgu->Getsymbol(Name());
        err += "'. ";
        err += "Argument: '" + globalTamgu->Getsymbol(type_a);
        err += "' is incompatible with parameter: '" + globalTamgu->Getsymbol(type_p) + "'";
        return globalTamgu->Returnerror(err, idthread);
    }

    
    //We store the instances in memory
    for (i = 0; i < instances.size();i++)
        globalTamgu->Storevariable(idthread, names[i], instances[i]);

    globalTamgu->Pushstackraw(environment, idthread);
    //We then apply our function within this environment
    a = bd->Run(environment, idthread);
    globalTamgu->Popstack(idthread);
    
    //if a has no reference, then it means that it was recorded into the environment
    if (!a->Reference())
        environment->Releasing();
    else {
        a->Setreference();
        //we clean our structure...
        environment->Releasing();
        a->Protect();
    }
    
    for (i = 0; i < instances.size(); i++) {
        TamguPredicateVariableInstance* ins = (TamguPredicateVariableInstance*)instances[i];
        if (ins->value == aNOELEMENT) {
            a->Releasenonconst();
            return aFALSE;
        }
    }
    return a;
}
//-------------------------------------------------------------------------------------------------
Tamgu* TamguThreadCallFromPredicate::Eval(Tamgu* domain, Tamgu* zone, short idthread) {
    TamguDeclarationLocal* environment = globalTamgu->Providedeclaration(idthread);
    if (globalTamgu->debugmode)
        environment->idinfo = Currentinfo();
    
    TamguVariableDeclaration* p = NULL;
    TamguFunction* bd = (TamguFunction*)body->Body(idthread);
    bool next = (bd->next != NULL);
    bool strict = bd->strict;
    if (!strict && bd->next)
        strict = true;
    
    Tamgu* a = NULL;
    
    bool error = true;
    short i = 0;
    short type_a = 0, type_p = 0;
    
    while (bd != NULL) {
        if (arguments.size() != bd->Size()) {
            bd = bd->next;
            continue;
        }
        
        error = false;
        for (i = 0; i < bd->parameters.size() && !error; i++) {
            p = (TamguVariableDeclaration*)bd->Parameter(i);
            a = arguments[i];
            type_p = p->typevariable;
            type_a = a->Type();
            if (type_p == a_instance) {
                environment->Declaring(p->Name(), a);
                globalTamgu->Storevariable(idthread, p->Name(), a);
                a->Setreference();
            }
            else
                error = p->Setarguments(environment, a, idthread, strict);
        }
        
        if (!error)
            break;
        
        environment->Cleaning();
        bd = bd->next;
    }
    
    if (error) {
        environment->Releasing();
        if (bd == NULL && next) {
            string err = "Could not find a match for: '";
            err += globalTamgu->Getsymbol(Name());
            err += "'";
            return globalTamgu->Returnerror(err, idthread);
        }
        string err = "Check the argument '";
        err += (char)(i+48);
        err += "' of: '";
        err += globalTamgu->Getsymbol(Name());
        err += "'. ";
        err += "Argument: '" + globalTamgu->Getsymbol(type_a);
        err += "' is incompatible with parameter: '" + globalTamgu->Getsymbol(type_p) + "'";
        return globalTamgu->Returnerror(err, idthread);
    }
    
    zone = globalTamgu->Declarator(predicatezone, parentid);
    ((TamguDeclarationPredicate*)zone)->Initlock();
    
    globalTamgu->Pushstackraw(zone, idthread);
    globalTamgu->Pushstackraw(environment, idthread);
    //We then apply our function within this environment
    a = bd->Eval(environment, aNULL, idthread);
    globalTamgu->Popstack(idthread);
    globalTamgu->Popstack(idthread);
    
    environment->Releasing();
    globalTamgu->threads[idthread].variables.clear();
    return aTRUE;
}

//-------------------------------------------------------------------------------------------------
// Predicate variables can have methods...
//-------------------------------------------------------------------------------------------------
Exporting basebin_hash<predicateMethod> TamguPredicate::methods;
static ThreadLock classlock;

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void TamguPredicate::AddMethod(TamguGlobal* global, string name, predicateMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(a_predicate) != global->infomethods.end() &&
        global->infomethods[a_predicate].find(name) != global->infomethods[a_predicate].end())
        return;
    
    global->infomethods[a_predicate][name] = infos;
    global->RecordArity(a_predicate, idname, arity);
    global->RecordArity(a_predicatevar, idname, arity);
}

void TamguPredicate::Setidtype(TamguGlobal* global) {
    Locking lock(classlock);
    if (TamguPredicate::methods.isEmpty())
        TamguPredicate::InitialisationModule(global,"");
}


bool TamguPredicate::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    TamguPredicate::AddMethod(global, "_initial", &TamguPredicate::MethodInitial, P_ONE, "_initial(): Get the associated object.");
    TamguPredicate::AddMethod(global, "leaves", &TamguPredicate::MethodLeaves, P_NONE, "leaves(): Return the leaf values.");
    TamguPredicate::AddMethod(global, "features", &TamguPredicate::MethodFeatures, P_NONE, "features(): Return the dependency features.");
    TamguPredicate::AddMethod(global, "name", &TamguPredicate::MethodName, P_NONE, "name(): Return the predicate name.");
    TamguPredicate::AddMethod(global, "trace", &TamguPredicate::MethodTrace, P_ONE, "trace(bool v): Set trace mode.");
    TamguPredicate::AddMethod(global, "rule", &TamguPredicate::MethodRuleid, P_NONE, "rule(): Return the rule id.");
    
    if (version != "") {
        global->newInstance[a_predicate] = new TamguPredicate(a_predicate, global);
        global->RecordCompatibilities(a_predicate);
        
        global->newInstance[a_predicatevar] = new TamguPredicateVar(global, a_predicatevar);
        global->RecordCompatibilities(a_predicatevar);
        
        global->newInstance[a_instance] = new TamguPredicateVariableInstance(global);
        global->RecordCompatibilities(a_instance);
        
        global->newInstance[a_dependency] = new TamguDependency(global, aNULL, a_dependency, 0);
    }
    short id = global->Getid("name");
    global->methods[a_dependency][id] = global->methods[a_predicate][id];
    
    global->methods[a_term][id] = global->methods[a_predicate][id];
    global->methods[a_concept][id] = global->methods[a_predicate][id];
    
    id = global->Getid("_initial");
    global->methods[a_dependency][id] = global->methods[a_predicate][id];
    id = global->Getid("features");
    global->methods[a_dependency][id] = global->methods[a_predicate][id];
    id = global->Getid("rule");
    global->methods[a_dependency][id] = global->methods[a_predicate][id];
    
    return true;
}

Tamgu* TamguPredicate::MethodRuleid(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    TamguPredicateContainer* apc = globalTamgu->Predicatecontainer();
    if (apc->rules.find(a_dependency) == apc->rules.end())
        return aNULL;
    
    Tamgu* kmap = Selectamap(contextualpattern);
    long id = Idrule();
    if (id != -1) {
        Tamgu* object = apc->rules[a_dependency][id];
        if (object != NULL) {
            long ln = object->Currentline();
            string filename = globalTamgu->Getfilename(object->Currentfile());
            kmap->Push("id", globalTamgu->ProvideConstint(id));
            kmap->Push("line", globalTamgu->ProvideConstint(ln));
            kmap->Push("filename", globalTamgu->Providewithstring(filename));
        }
    }
    return kmap;
}

Tamgu* TamguPredicate::MethodFeatures(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (isDependency()) {
        TamguDependency* dep = (TamguDependency*)this;
        
        if (dep->features == aNULL)
            return globalTamgu->Providemapss();
        return dep->features;
    }
    return globalTamgu->Providemapss();
}

Tamgu* TamguPredicate::MethodInitial(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (isDependency()) {
        Tamgu* vect = callfunc->Evaluate(0, contextualpattern, idthread);
        if (!vect->isVectorContainer())
            return globalTamgu->Returnerror("DEP(010): We expect a vector as variable", idthread);
        
        long sz = vect->Size();
        if (sz < 2)
            return globalTamgu->Returnerror("DEP(011): This vector does not contain enough information. It should contain: [name,features,synode1,synode2...]", idthread);
        TamguDependency* dep = (TamguDependency*)this;
        dep->name = globalTamgu->Getid(vect->getstring(0));
        Tamgu* val = vect->getvalue(1);
        if (val != aNULL && !val->isMapContainer() && val->Type() != a_synode)
            return globalTamgu->Returnerror("DEP(012): Wrong feature definition. It should be either 'null' or a 'map'", idthread);
        
        if (val->isMapContainer()) {
            if (sz < 3)
                return globalTamgu->Returnerror("DEP(011): This vector does not contain enough information. It should contain: [name,features,synode1,synode2...]", idthread);
            Tamgumapss* feat = globalTamgu->Providemapss();
            dep->features = feat;
            feat->Setreference(reference);
            TamguIteration* itr = val->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                feat->values[itr->Keystring()] = itr->Valuestring();
            itr->Release();
        }
        
        long beg = 2;
        if (val->Type() == a_synode)
            beg = 1;
        
        for (long i = beg; i < sz; i++) {
            val = vect->getvalue(i);
            if (val->Type() != a_synode)
                return globalTamgu->Returnerror("DEP(013): Only 'synode' can be used here to create a dependency", idthread);
            dep->parameters.push_back(val);
            val->Setreference(reference);
        }
        return aTRUE;
    }
    
    if (Function() != NULL) {
        TamguPredicateFunction* apf = (TamguPredicateFunction*)this;
        apf->object = callfunc->Evaluate(0, contextualpattern, idthread);
        return aTRUE;
    }
    return aFALSE;
}

Tamgu* TamguPredicate::MethodTrace(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    bool thetrace = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
    if (isPredicate()) {
        if (globalTamgu->predicates.check(name))
            globalTamgu->predicates[name]->trace = thetrace;
        return aTRUE;
    }
    return aFALSE;
}

Tamgu* TamguPredicate::Newinstance(short idthread, Tamgu* f) {
    if (f != NULL)
        return new TamguPredicateFunction(NULL, f, name);
    
    return new TamguPredicate(name, globalTamgu);
}


void TamguPredicateFunction::Setname(short n) {
    name = n;
    globalTamgu->predicates[name] = this;
}

//-------------------------------------------------------------------------------------------------
TamguDependency::TamguDependency(TamguGlobal* g, Tamgu* f, short n, short id) : idrule(0), TamguPredicate(n, g, a_dependency) {
    if (f->Type() == a_mapss) {
        features = globalTamgu->Providemapss();
        string key;
        for (const auto& it : ((Tamgumapss*)f)->values) {
            key = it.first;
            if (key[0] == '~' || it.second == "~")
                continue;
            if (key[0] == '=')
                key = &key.c_str()[1];
            ((Tamgumapss*)features)->values[key] = it.second;
        }
    }
    else
        features = aNULL;
    
    chosen = false;
    idvar = id;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void PreLocalPrint(string s) {
    Locking _lock(globalTamgu->_printlock);
    
    Tamgustring* kval = (Tamgustring*)globalTamgu->kstd;
    
    if (kval == NULL) {
        if (globalTamgu->displayfunction == NULL)
            *globalTamgu->os << s;
        else
            globalTamgu->displayfunction(s, globalTamgu->displayobject);
        if (globalTamgu->doubledisplay) {
#ifdef DOSOUTPUT
            string sdos = conversion2dos(s);
            cout << sdos;
#else
            cout << s;
#endif
        }
    }
    else
        kval->value += s;
}

//-----------------------------------------------
//We implement a local reshuffling...

bool TamguGlobal::predicateTrace(short n) {
    return predicates[n]->trace;
}

//----------------------------------------------------------------------------------------------------
void TamguPredicateVariableInstance::Resetreference(short inc) {
    if (value != aNOELEMENT)
        value->Resetreferencenoprotect(inc);
    
    reference -= inc;
    if (reference <= 0) {
        if (protect) {
            reference = 0;
        }
        else {
            if (idtracker != -1)
                globalTamgu->RemoveFromTracker(idtracker);
            delete this;
        }
    }
}

void TamguPredicateVariableInstancebuff::Resetreference(short inc) {
    if (value != aNOELEMENT)
        value->Resetreferencenoprotect(inc);
    
    reference -= inc;
    if (reference <= 0) {
        reference = 0;
        value = aNOELEMENT;
        if (!protect) {
            protect = true;
            if (!globalTamgu->threadMODE && used)
                globalTamgu->pviempties.push_back(idx);
            used = false;
        }
    }
}

void TamguPredicateVariableInstancebuff::Resetreferencenoprotect(short inc) {
    if (value != aNOELEMENT)
        value->Resetreferencenoprotect(inc);
    
    reference -= inc;
    if (reference <= 0) {
        value = aNOELEMENT;
        protect = true;
        reference = 0;
        if (!globalTamgu->threadMODE && used)
            globalTamgu->pviempties.push_back(idx);
        used = false;
    }
}

void TamguPredicateVariableInstancebuff::Removereference(short inc) {
    if (value != aNOELEMENT)
        value->Resetreferencenoprotect(inc);
    
    reference -= inc;
    if (reference <= 0) {
        value = aNOELEMENT;
        reference = 0;
        if (!protect) {
            protect = true;
            if (!globalTamgu->threadMODE && used)
                globalTamgu->pviempties.push_back(idx);
            used = false;
        }
    }
}

void TamguPredicateVariableInstance::affiche() {
    if (value != aNOELEMENT)
        cerr << "=" << value->String();
    cerr << "/" << reference;
}

bool compshort(short n, short nn) {
    if (n < nn)
        return true;
    return false;
}

void AffichageDom(TamguDeclaration* dom, long depth) {
    for (long i = 0; i < depth; i++)
        cerr << " ";
    basebin_hash<Tamgu*>::iterator it;
    vector<short> names;
    for (it = dom->declarations.begin(); it != dom->declarations.end(); it++)
        names.push_back(it->first);
    
    sort(names.begin(), names.end(), compshort);
    bool first = true;
    for (long i = 0; i < names.size(); i++) {
        if (!first)
            cerr << ",";
        first = false;
        Tamgu* v = dom->declarations[names[i]];
        if (names[i] != v->Name())
            cerr << names[i] << ":" << v->Name();
        else {
            cerr << names[i];
            ((TamguPredicateVariableInstance*)v)->affiche();
        }
    }
    cerr << endl;
    
}

//------------------------------------------------------------------------------------------
bool TamguGlobal::TestPredicate(TamguDeclaration* dom, TamguPredicate* p) {
    if (p->name != a_universal && knowledgebase.find(p->name) == knowledgebase.end())
        return false;
    
    if (!p->isDependency()) {
        string argument_key;
        if (p->Stringpredicatekey(argument_key)) {
            vector<TamguPredicate*>& v = knowledgebase_on_first[argument_key];
            for (long i = 0; i < v.size(); i++) {
                if (p->Unify(dom, v[i]))
                    return true;
            }
            return false;
        }
        
        if (p->Stringpredicatekeysecond(argument_key)) {
            vector<TamguPredicate*>& v = knowledgebase_on_second[argument_key];
            for (long i = 0; i < v.size(); i++) {
                if (p->Unify(dom, v[i]))
                    return true;
            }
            return false;
        }
        
        if (p->Stringpredicatekeythird(argument_key)) {
            vector<TamguPredicate*>& v = knowledgebase_on_third[argument_key];
            for (long i = 0; i < v.size(); i++) {
                if (p->Unify(dom, v[i]))
                    return true;
            }
            return false;
        }
    }
    
    if (p->name == a_universal) {
        hmap<short, vector<TamguPredicate*> >::iterator it;
        for (it = knowledgebase.begin(); it != knowledgebase.end(); it++) {
            vector<TamguPredicate*>& v = it->second;
            for (long i = 0; i < v.size(); i++) {
                if (p->Unify(dom, v[i]))
                    return true;
            }
        }
        return false;
    }
    vector<TamguPredicate*>& v = knowledgebase[p->name];
    for (long i = 0; i < v.size(); i++) {
        if (p->Unify(dom, v[i]))
            return true;
    }
    return false;
}


bool TamguGlobal::Checkpredicate(short name) {
    Locking* _lock = _getlockg(_knowledgelock);
    bool ret = (name == a_universal)?knowledgebase.size():(knowledgebase.find(name) != knowledgebase.end());
    _cleanlockg(_lock);
    return ret;
}

char TamguGlobal::isaValidPredicate(TamguDeclaration* dom, TamguPredicate* p, hmap<unsigned short, vector<TamguPredicateRule*> >& rulebase) {
    Locking* _lock = _getlockg(_knowledgelock);
    
    if (p->name != a_universal && knowledgebase.find(p->name) == knowledgebase.end()) {
        _cleanlockg(_lock);
        return 1;
    }
    
    long i;
    if (!p->isDependency()) {
        string argument_key;
        
        if (p->Stringpredicatekey(argument_key)) {
            vector<TamguPredicate*>& v = knowledgebase_on_first[argument_key];
            for (i = 0; i < v.size(); i++) {
                if (p->Unify(dom, v[i])) {
                    _cleanlockg(_lock);
                    return 2;
                }
            }
            //we keep a chance of finding a rule to match with
            _cleanlockg(_lock);
            return (rulebase.find(p->name) != rulebase.end());
        }
        
        if (p->Stringpredicatekeysecond(argument_key)) {
            vector<TamguPredicate*>& v = knowledgebase_on_second[argument_key];
            for (i = 0; i < v.size(); i++) {
                if (p->Unify(dom, v[i])) {
                    _cleanlockg(_lock);
                    return 2;
                }
            }
            _cleanlockg(_lock);
            //we keep a chance of finding a rule to match with
            return (rulebase.find(p->name) != rulebase.end());
        }
        
        if (p->Stringpredicatekeythird(argument_key)) {
            vector<TamguPredicate*>& v = knowledgebase_on_third[argument_key];
            for (i = 0; i < v.size(); i++) {
                if (p->Unify(dom, v[i])) {
                    _cleanlockg(_lock);
                    return 2;
                }
            }
            _cleanlockg(_lock);
            //we keep a chance of finding a rule to match with
            return (rulebase.find(p->name) != rulebase.end());
        }
    }
    
    if (p->name == a_universal) {
        hmap<short, vector<TamguPredicate*> >::iterator it;
        for (it = knowledgebase.begin(); it != knowledgebase.end(); it++) {
            vector<TamguPredicate*>& v = it->second;
            for (i = 0; i < v.size(); i++) {
                if (p->Unify(dom, v[i])) {
                    _cleanlockg(_lock);
                    return 2;
                }
            }
        }
        _cleanlockg(_lock);
        return 0;
    }
    
    vector<TamguPredicate*>& v = knowledgebase[p->name];
    for (i = 0; i < v.size(); i++) {
        if (p->Unify(dom, v[i])) {
            _cleanlockg(_lock);
            return 2;
        }
    }
    
    _cleanlockg(_lock);
    //we keep a chance of finding a rule to match with
    return (rulebase.find(p->name) != rulebase.end());
}

bool TamguGlobal::GetPredicates(TamguDeclaration* dom, TamguPredicate* p, vector<TamguPredicate*>& res, bool cut) {
    long sz;
    long i;
    bool ret = false;
    
    Locking* _lock = _getlockg(_knowledgelock);
    
    if (!p->isDependency()) {
        string argument_key;
        if (p->Stringpredicatekey(argument_key)) {
            vector<TamguPredicate*>& v = knowledgebase_on_first[argument_key];
            sz = v.size();
            res.reserve(sz);
            if (sz > 3) {
                if (p->Stringpredicatekeysecond(argument_key)) {
                    //Then we have a double constraint
                    for (i = 0; i < sz; i++) {
                        v[i]->shared = true;
                    }
                    vector<TamguPredicate*>& v_second = knowledgebase_on_second[argument_key];
                    for (i = 0; i < v_second.size(); i++) {
                        if (v_second[i]->isChosen())
                            continue;
                        if (v_second[i]->shared) {
                            if (p->Unify(dom, v_second[i])) {
                                res.push_back(v_second[i]);
                                if (cut) {
                                    _cleanlockg(_lock);
                                    return true;
                                }
                                ret = true;
                            }
                        }
                    }
                    for (i = 0; i < sz; i++) {
                        v[i]->shared = false;
                    }
                    _cleanlockg(_lock);
                    return ret;
                }
            }
            for (i = 0; i < sz; i++) {
                if (v[i]->isChosen())
                    continue;
                
                if (p->Unify(dom, v[i])) {
                    res.push_back(v[i]);
                    if (cut) {
                        _cleanlockg(_lock);
                        return true;
                    }
                    ret = true;
                }
            }
            _cleanlockg(_lock);
            return ret;
        }
        
        if (p->Stringpredicatekeysecond(argument_key)) {
            vector<TamguPredicate*>& v = knowledgebase_on_second[argument_key];
            sz = v.size();
            res.reserve(sz);
            for (i = 0; i < sz; i++) {
                if (v[i]->isChosen())
                    continue;
                
                if (p->Unify(dom, v[i])) {
                    res.push_back(v[i]);
                    if (cut) {
                        _cleanlockg(_lock);
                        return true;
                    }
                    ret = true;
                }
            }
            _cleanlockg(_lock);
            return ret;
        }
        
        if (p->Stringpredicatekeythird(argument_key)) {
            vector<TamguPredicate*>& v = knowledgebase_on_third[argument_key];
            sz = v.size();
            res.reserve(sz);
            for (i = 0; i < sz; i++) {
                if (v[i]->isChosen())
                    continue;
                
                if (p->Unify(dom, v[i])) {
                    res.push_back(v[i]);
                    if (cut) {
                        _cleanlockg(_lock);
                        return true;
                    }
                    ret = true;
                }
            }
            _cleanlockg(_lock);
            return ret;
        }
    }
    
    if (p->name == a_universal) {
        sz = knowledgebase.size();
        res.reserve(sz>>1);
        hmap<short, vector<TamguPredicate*> >::iterator it;
        for (it = knowledgebase.begin(); it != knowledgebase.end(); it++) {
            vector<TamguPredicate*>& v = it->second;
            for (i = 0; i < v.size(); i++) {
                if (v[i]->isChosen())
                    continue;
                
                if (p->Unify(dom, v[i])) {
                    res.push_back(v[i]);
                    if (cut) {
                        _cleanlockg(_lock);
                        return true;
                    }
                    ret = true;
                }
            }
        }
        _cleanlockg(_lock);
        return ret;
    }
    
    vector<TamguPredicate*>& v = knowledgebase[p->name];
    sz = v.size();
    res.reserve(sz);
    for (i = 0; i < sz; i++) {
        if (v[i]->isChosen())
            continue;
        
        if (p->Unify(dom, v[i])) {
            res.push_back(v[i]);
            if (cut) {
                _cleanlockg(_lock);
                return true;
            }
            ret = true;
        }
    }
    
    _cleanlockg(_lock);
    return ret;
}


bool TamguGlobal::StorePredicate(TamguDeclaration* dom, TamguPredicate* pv, bool last) {
    Locking* _lock = _getlockg(_knowledgelock);
    if (TestPredicate(dom, pv)) {
        _cleanlockg(_lock);
        return false;
    }
    
    if (pv->Name() == a_universal) {
        if (pv->Idvar() != 0) {
            //we share the same name as with the dependency that was associated with a variable...
            Tamgu* a = dom->Declaration(pv->Idvar());
            if (a != NULL && a->Type() == a_dependency)
                pv->name = a->Name();
            else {
                _cleanlockg(_lock);
                return false;
            }
        }
        else {
            _cleanlockg(_lock);
            return false;
        }
    }
    
    if (last)
        knowledgebase[pv->name].push_back(pv);
    else
        knowledgebase[pv->name].insert(knowledgebase[pv->name].begin(), pv);
    
    if (!pv->isDependency()) {
        string argument_key;
        if (pv->Stringpredicatekey(argument_key))
            knowledgebase_on_first[argument_key].push_back(pv);
        
        if (pv->Stringpredicatekeysecond(argument_key))
            knowledgebase_on_second[argument_key].push_back(pv);
        
        if (pv->Stringpredicatekeythird(argument_key))
            knowledgebase_on_third[argument_key].push_back(pv);
    }
    
    pv->Setreference();
    _cleanlockg(_lock);
    return true;
}

bool TamguGlobal::RemovePredicates(TamguDeclaration* dom, TamguPredicate* p) {
    Locking* _lock = _getlockg(_knowledgelock);
    if (knowledgebase.find(p->name) == knowledgebase.end()) {
        _cleanlockg(_lock);
        return false;
    }
    
    bool res = false;
    
    vector<TamguPredicate*>& vk = knowledgebase[p->name];
    long i;
    
    if (!p->isDependency()) {
        string argument_key;
        
        if (p->Stringpredicatekey(argument_key)) {
            vector<TamguPredicate*>& v = knowledgebase_on_first[argument_key];
            long j;
            for (i = (long)v.size() - 1; i >= 0; i--) {
                if (p->same(v[i]) == aTRUE) {
                    for (j = (long)vk.size() - 1; j >= 0; j--) {
                        if (vk[j] == v[i]) {
                            vk.erase(vk.begin() + j);
                            break;
                        }
                    }
                    v[i]->Resetreference();
                    v.erase(v.begin() + i);
                    res = true;
                }
            }
            _cleanlockg(_lock);
            return res;
        }
        
        if (p->Stringpredicatekeysecond(argument_key)) {
            vector<TamguPredicate*>& v = knowledgebase_on_second[argument_key];
            long j;
            for (i = (long)v.size() - 1; i >= 0; i--) {
                if (p->same(v[i]) == aTRUE) {
                    for (j = (long)vk.size() - 1; j >= 0; j--) {
                        if (vk[j] == v[i]) {
                            vk.erase(vk.begin() + j);
                            break;
                        }
                    }
                    v[i]->Resetreference();
                    v.erase(v.begin() + i);
                    res = true;
                }
            }
            _cleanlockg(_lock);
            return res;
        }
        
        if (p->Stringpredicatekeythird(argument_key)) {
            vector<TamguPredicate*>& v = knowledgebase_on_third[argument_key];
            long j;
            for (i = (long)v.size() - 1; i >= 0; i--) {
                if (p->same(v[i]) == aTRUE) {
                    for (j = (long)vk.size() - 1; j >= 0; j--) {
                        if (vk[j] == v[i]) {
                            vk.erase(vk.begin() + j);
                            break;
                        }
                    }
                    v[i]->Resetreference();
                    v.erase(v.begin() + i);
                    res = true;
                }
            }
            _cleanlockg(_lock);
            return res;
        }
    }
    
    for (i = (long)vk.size() - 1; i >= 0; i--) {
        if (p->same(vk[i]) == aTRUE) {
            vk[i]->Resetreference();
            vk.erase(vk.begin() + i);
            res = true;
        }
    }
    
    _cleanlockg(_lock);
    return res;
}

bool TamguGlobal::RemoveThePredicate(TamguDeclaration* dom, TamguPredicate* p) {
    Locking* _lock = _getlockg(_knowledgelock);
    vector<TamguPredicate*>& v = knowledgebase[p->name];
    for (long i = (long)v.size() - 1; i >= 0; i--) {
        if (p == v[i]) {
            v.erase(v.begin() + i);
            if (!p->isDependency()) {
                string argument_key;
                
                if (p->Stringpredicatekey(argument_key)) {
                    vector<TamguPredicate*>& vk = knowledgebase_on_first[argument_key];
                    for (i = (long)vk.size() - 1; i >= 0; i--) {
                        if (p == vk[i]) {
                            vk.erase(vk.begin() + i);
                            break;
                        }
                    }
                }
                if (p->Stringpredicatekeysecond(argument_key)) {
                    vector<TamguPredicate*>& vk = knowledgebase_on_second[argument_key];
                    for (i = (long)vk.size() - 1; i >= 0; i--) {
                        if (p == vk[i]) {
                            vk.erase(vk.begin() + i);
                            break;
                        }
                    }
                }
                
                if (p->Stringpredicatekeythird(argument_key)) {
                    vector<TamguPredicate*>& vk = knowledgebase_on_third[argument_key];
                    for (i = (long)vk.size() - 1; i >= 0; i--) {
                        if (p == vk[i]) {
                            vk.erase(vk.begin() + i);
                            break;
                        }
                    }
                }
                
            }
            p->Resetreference();
            _cleanlockg(_lock);
            return true;
        }
    }
    _cleanlockg(_lock);
    return false;
}


//----------------------------------------------------------------------------------------

Tamgu* TamguPredicateVariableInstance::same(Tamgu* a) {
    Tamgu* dom = globalTamgu->Declarator(predicatezone, globalTamgu->GetThreadid());
    if (dom == aNULL)
        return aFALSE;
    
    dom->Setdomainlock();
    Tamgu* v = Value((TamguDeclaration*)dom);
    
    if (v == aNOELEMENT) {
        dom->Resetdomainlock();
        return aFALSE;
    }
    v = v->same(a);
    dom->Resetdomainlock();
    return v;
}

Tamgu* TamguPredicate::same(Tamgu* a) {
    if (a == this)
        return aTRUE;
    
    if (!a->isPredicate() || parameters.size() != a->Size() || name != a->Name())
        return aFALSE;
    
    for (long i = 0; i < parameters.size(); i++) {
        if (parameters[i]->same(a->Parameter(i)) == aFALSE)
            return aFALSE;
    }
    
    return aTRUE;
}

Tamgu* TamguDependencyAction::same(Tamgu* a) {
    if (!a->isPredicate() || parameters.size() != a->Size())
        return aFALSE;
    
    for (long i = 0; i < parameters.size(); i++) {
        if (parameters[i]->same(a->Parameter(i)) == aFALSE)
            return aFALSE;
    }
    
    if (features == aNULL)
        return aTRUE;
    
    if (((TamguPredicate*)a)->isDependency()) {
        Tamgu* feats = ((TamguDependency*)a)->features;
        if (feats == aNULL)
            return aTRUE;
        
        string key;
        string val;
        
        bool neg;
        
        hmap<string, string >& values = ((Tamgumapss*)feats)->values;
        hmap<string, string >& avalues = ((Tamgumapss*)features)->values;
        
        for (const auto& it : values) {
            neg = false;
            key = it.first;
            if (key[0] == '~') {
                neg = true;
                key = &key.c_str()[1];
            }
            
            val = it.second;
            if (val == "~") {
                if (avalues.find(key) != avalues.end()) {
                    if (!neg)
                        return aFALSE;
                }
                continue;
            }
            
            if (avalues.find(key) != avalues.end()) {
                if (val != avalues[key]) {
                    if (!neg)
                        return aFALSE;
                }
                else {
                    if (neg)
                        return aFALSE;
                }
            }
            else {
                if (!neg)
                    return aFALSE;
            }
        }
    }
    
    return aTRUE;
}

Tamgu* TamguPredicateTerm::same(Tamgu* a) {
    if (a->Type() != a_term || parameters.size() != a->Size() || !a->isName(name))
        return aFALSE;
    for (long i = 0; i < parameters.size(); i++) {
        if (parameters[i]->same(a->Parameter(i)) == aFALSE)
            return aFALSE;
    }
    return aTRUE;
}

Tamgu* TamguPredicateConcept::same(Tamgu* a) {
    if (!a->isConcept() || parameters.size() != a->Size() || !globalTamgu->Checkhierarchy(name, a->Name()))
        return aFALSE;
    for (long i = 0; i < parameters.size(); i++) {
        if (parameters[i]->same(a->Parameter(i)) == aFALSE)
            return aFALSE;
    }
    return aTRUE;
}
//----------------------------------------------------------------------------------------
// This method is used to detect if a variable or a set of variable (as in a vector) in a predicate has been unified.
// This is especially usefull to detect if a variable can be used as an index.
// By design, every object that is not one of the following (which inherits the Tamgu version) returns true for this method
bool TamguBasePredicateVariable::isUnified(TamguDeclaration* dom) {
    return false;
}

bool Tamguvector::isUnified(TamguDeclaration* dom) {
    for (BLONG i = 0; i < values.size(); i++) {
        if (values[i]->isUnified(dom) == false)
            return false;
    }
    return true;
}

bool TamguConstmap::isUnified(TamguDeclaration* dom) {
    for (BLONG i = 0; i < values.size(); i++) {
        if (keys[i]->isUnified(dom) == false)
            return false;
        if (values[i]->isUnified(dom) == false)
            return false;
    }
    return true;
}

bool TamguPredicateTerm::isUnified(TamguDeclaration* dom) {
    for (long i = 0; i < parameters.size(); i++) {
        if (parameters[i]->isUnified(dom) == false)
            return false;
    }
    return true;
}

bool TamguPredicateConcept::isUnified(TamguDeclaration* dom) {
    for (long i = 0; i < parameters.size(); i++) {
        if (parameters[i]->isUnified(dom) == false)
            return false;
    }
    return true;
}


bool TamguPredicateVariableInstance::isUnified(TamguDeclaration* dom) {
    Tamgu* v;
    if (dom != NULL)
        v = Value(dom);
    else
        v = value;
    if (v == aNOELEMENT)
        return false;
    
    if (v == aUNIVERSAL)
        return true;
    
    return v->isUnified(dom);
}

bool TamguPredicate::isUnified(TamguDeclaration* dom) {
    for (long i = 0; i < parameters.size(); i++) {
        if (parameters[i]->isUnified(dom) == false)
            return false;
    }
    return true;
}

//--------------------------------------------------------------
bool Tamgu::Unify(TamguDeclaration* dom, Tamgu* a) {
    if (this == aUNIVERSAL || a == aUNIVERSAL)
        return true;
    
    if (a->same(this)->Boolean() == true)
        return true;
    return false;
}

bool TamguPredicateVariableInstance::Unify(TamguDeclaration* dom, Tamgu* a) {
    Tamgu* v = Value(dom);
    
    if (v == aNOELEMENT || v == aUNIVERSAL)
        return true;
    
    return v->Unify(dom, a);
}

bool TamguPredicateTerm::Unify(TamguDeclaration* dom, Tamgu* a) {
    if (a->Type() != a_term || !a->isName(name) || a->Size() != parameters.size())
        return false;
    TamguPredicateTerm* term = (TamguPredicateTerm*)a;
    for (long i = 0; i < parameters.size(); i++) {
        if (parameters[i]->Unify(dom, term->parameters[i]) == false)
            return false;
    }
    return true;
}

bool TamguPredicateConcept::Unify(TamguDeclaration* dom, Tamgu* a) {
    if (!a->isConcept() || parameters.size() != a->Size() || !globalTamgu->Checkhierarchy(name, a->Name()))
        return false;
    
    TamguPredicateConcept* term = (TamguPredicateConcept*)a;
    for (long i = 0; i < parameters.size(); i++) {
        if (parameters[i]->Unify(dom, term->parameters[i]) == false)
            return false;
    }
    
    return true;
}

bool Tamguvector::Unify(TamguDeclaration* dom, Tamgu* a) {
    if (a->Type() != a_vector || a->Size() != values.size())
        return false;
    
    Tamguvector* vect = (Tamguvector*)a;
    for (BLONG i = 0; i < values.size(); i++) {
        if (values[i]->Unify(dom, vect->values[i]) == false)
            return false;
    }
    return true;
}

bool TamguConstmap::Unify(TamguDeclaration* dom, Tamgu* a) {
    if (a->Type() != a_constmap || a->Size() != values.size())
        return false;
    
    TamguConstmap* kmap = (TamguConstmap*)a;
    for (BLONG i = 0; i < values.size(); i++) {
        if (keys[i]->Unify(dom, kmap->keys[i]) == false)
            return false;
        if (values[i]->Unify(dom, kmap->values[i]) == false)
            return false;
    }
    return true;
}

bool TamguPredicate::Unify(TamguDeclaration* dom, Tamgu* a) {
    if (!a->isPredicate() || !a->isName(name) || a->Size() != parameters.size())
        return false;
    
    TamguPredicate* pred = (TamguPredicate*)a;
    for (long i = 0; i < parameters.size(); i++) {
        if (parameters[i]->Unify(dom, pred->parameters[i]) == false)
            return false;
    }
    return true;
}

void TamguDependency::Setfeatures(Tamgu* feats) {
    if (feats == aNULL)
        return;
    if (features == aNULL)
        features = globalTamgu->Providemapss();
    
    Tamgumapss* f = (Tamgumapss*)features;
    string key;
    string val;
    
    for (const auto& it : ((Tamgumapss*)feats)->values) {
        key = it.first;
        val = it.second;
        if (key[0] == '=')
            key = &key.c_str()[1];
        
        if (val == "~") {
            if (f->values.find(key) != f->values.end())
                f->values.erase(key);
            continue;
        }
        
        f->values[key] = val;
    }
}

bool TamguDependency::Unify(TamguDeclaration* dom, Tamgu* a) {
    if (!a->isPredicate() || a->Size() != parameters.size() || (name != a_universal && !a->isName(name)))
        return false;
    
    TamguPredicate* pred = (TamguPredicate*)a;
    //a is from the database
    if (features != aNULL) {
        
        bool computed = false;
        if (!pred->isDependency()) {
            if (features->Size() != 0) {
                //In this case, all features should be negative...
                hmap<string, string >& values = ((Tamgumapss*)features)->values;
                for (const auto& it : values) {
                    if (it.second != "~")
                        return false;
                }
            }
            computed = true;
        }
        
        if (!computed) {
            Tamgu* feat = ((TamguDependency*)pred)->features;
            hmap<string, string >& values = ((Tamgumapss*)features)->values;
            
            if (feat == aNULL) {
                if (features->Size() != 0) {
                    //In this case, all features should be negative...
                    hmap<string, string >& values = ((Tamgumapss*)features)->values;
                    for (const auto& it : values) {
                        if (it.second != "~")
                            return false;
                    }
                }
                computed = true;
                
            }
            if (!computed) {
                string key;
                string val;
                bool aff = false;
                bool neg;
                
                hmap<string, string >& avalues = ((Tamgumapss*)feat)->values;
                hmap<string, string > assignation;
                for (const auto& it : values) {
                    aff = false;
                    neg = false;
                    key = it.first;
                    if (key[0] == '=') {
                        aff = true;
                        key = &key.c_str()[1];
                    }
                    else {
                        if (key[0] == '~') {
                            neg = true;
                            key = &key.c_str()[1];
                        }
                    }
                    val = it.second;
                    if (val == "~") {
                        if (avalues.find(key) != avalues.end()) {
                            if (!aff) {
                                if (!neg)
                                    return false;
                            }
                            else
                                assignation[key] = val;
                        }
                        continue;
                    }
                    if (avalues.find(key) != avalues.end()) {
                        if (val != avalues[key]) {
                            if (!neg)
                                return false;
                        }
                        else {
                            if (neg)
                                return false;
                        }
                    }
                    else {
                        if (aff)
                            assignation[key] = val;
                        else {
                            if (!neg)
                                return false;
                        }
                    }
                }
                for (const auto& it : assignation) {
                    if (it.second == "~")
                        avalues.erase(it.first);
                    else
                        avalues[it.first] = it.second;
                }
            }
        }
    }
    
    for (long i = 0; i < parameters.size(); i++) {
        if (parameters[i]->Unify(dom, pred->parameters[i]) == false)
            return false;
    }
    
    
    return true;
}

//--------------------------------------------------------------------
Tamgu* TamguBasePredicateVariable::Put(Tamgu* ke, Tamgu* dom, short idthread) {
    if (!dom->isDeclared(predicatedico)) {
        dom = globalTamgu->Declarator(predicatedico, idthread);
        if (dom == aNULL)
            return globalTamgu->Returnerror(e_unknown_predicate_variable, idthread);
    }
    
    if (dom->Failed())
        return aTRUE;
    
    
    Tamgu* d = dom->Declaration(predicatedico);
    
    if (d == NULL && !d->Checkdico(name)) {
        dom->Setfail(true);
        return aTRUE;
    }
    
    return d->Getdico(name)->Put(ke, dom, idthread);
}


Tamgu* TamguPredicateVariableInstance::Put(Tamgu* dom, Tamgu* ke, short idthread) {
    dom = dom->Getpredicatezone(idthread);
    if (dom != aNULL) {
        dom->Setdomainlock();
        TamguPredicateVariableInstance* v = (TamguPredicateVariableInstance*)Variable((TamguDeclaration*)dom);
        if (v->value == ke) {
            dom->Resetdomainlock();
            return aTRUE;
        }
        
        Tamgu* val = v->value;
        v->value = ke->Atom(true);
        val->Resetreference(reference);
        v->value->Setreference(reference);
        dom->Resetdomainlock();
        return v->value;
    }
    else
        return globalTamgu->Returnerror(e_unknown_predicate_variable, idthread);
}

Tamgu* TamguPredicateTerm::Put(Tamgu* dom, Tamgu* ke, short idthread) {
    dom = dom->Getpredicatezone(idthread);
    if (dom != aNULL) {
        dom->Setdomainlock();
        if (ke->Type() != a_term || ke->Size() != parameters.size() || !ke->isName(name)) {
            dom->Setfail(true);
            dom->Resetdomainlock();
            return aTRUE;
        }
        TamguPredicateTerm* kpf = (TamguPredicateTerm*)ke;
        
        for (long i = 0; i < parameters.size(); i++)
            parameters[i]->Put(dom, kpf->parameters[i], idthread);
        dom->Resetdomainlock();
    }
    return aTRUE;
}

Tamgu* TamguPredicateConcept::Put(Tamgu* dom, Tamgu* ke, short idthread) {
    dom = dom->Getpredicatezone(idthread);
    if (dom != aNULL) {
        dom->Setdomainlock();
        if (ke->Type() != a_concept || ke->Size() != parameters.size() || globalTamgu->Checkhierarchy(name, ke->Name())) {
            dom->Setfail(true);
            dom->Resetdomainlock();
            return aTRUE;
        }
        
        TamguPredicateConcept* kpf = (TamguPredicateConcept*)ke;
        
        for (long i = 0; i < parameters.size(); i++)
            parameters[i]->Put(dom, kpf->parameters[i], idthread);
        dom->Resetdomainlock();
    }
    return aTRUE;
}


Tamgu* TamguPredicate::Put(Tamgu* dom, Tamgu* val, short idthread) {
    return globalTamgu->Returnerror("PRE(009): You cannot instanciate a predicate this way", idthread);
}

Tamgu* TamguPredicateVar::Put(Tamgu* dom, Tamgu* val, short idthread) {
    if (val->Type() != ptype)
        val = aFAIL;
    
    //val cannot contain any PredicateVariable
    long i;
    Tamgu* e;
    vector<Tamgu*> v;
    
    for (i = 0; i < val->Size(); i++) {
        e = val->Parameter(i)->Eval(dom, aNULL, idthread);
        if (!e->isUnified((TamguDeclaration*)dom)) {
            for (i = 0; i < v.size(); i++) {
                v[i]->Release();
            }
            v.clear();
            val = aFAIL;
            break;
        }
        v.push_back(e->Atom(true));
    }
    
    for (i = 0; i < parameters.size(); i++)
        parameters[i]->Resetreference(reference);
    
    parameters.clear();
    
    name = val->Name();
    for (i = 0; i < v.size(); i++) {
        v[i]->Setreference(reference);
        parameters.push_back(v[i]);
    }
    
    return aTRUE;
}

void TamguPredicateTerm::Setreference() {
    ++reference;
    protect = 0;
    for (long i = 0; i < parameters.size(); i++)
        parameters[i]->Setreference();
}

void TamguPredicateTerm::Setreference(short inc) {
    reference += inc;
    protect = 0;
    for (long i = 0; i < parameters.size(); i++)
        parameters[i]->Setreference(inc);
}

void TamguPredicateTerm::Setprotect(bool n) {
    protect = n;
    for (long i = 0; i < parameters.size(); i++)
        parameters[i]->Setprotect(n);
}

void TamguPredicateTerm::Popping() {
    protect = false;
    if (reference <= 0)
        protect = true;
    for (long i = 0; i < parameters.size(); i++)
        parameters[i]->Popping();
}

void TamguPredicateTerm::Resetreference(short inc) {
    for (long i = 0; i < parameters.size(); i++)
        parameters[i]->Resetreference(inc);
    TamguReference::Resetreference(inc);
}

void TamguPredicateConcept::Setreference() {
    ++reference;
    protect = 0;
    for (long i = 0; i < parameters.size(); i++)
        parameters[i]->Setreference();
}

void TamguPredicateConcept::Setreference(short inc) {
    reference += inc;
    protect = 0;
    for (long i = 0; i < parameters.size(); i++)
        parameters[i]->Setreference(inc);
}

void TamguPredicateConcept::Setprotect(bool n) {
    protect = n;
    for (long i = 0; i < parameters.size(); i++)
        parameters[i]->Setprotect(n);
}

void TamguPredicateConcept::Popping() {
    protect = false;
    if (reference <= 0)
        protect = true;
    for (long i = 0; i < parameters.size(); i++)
        parameters[i]->Popping();
}

void TamguPredicateConcept::Resetreference(short inc) {
    for (long i = 0; i < parameters.size(); i++)
        parameters[i]->Resetreference(inc);
    TamguReference::Resetreference(inc);
}


void TamguPredicate::Setreference() {
    ++reference;
    protect = 0;
    for (long i = 0; i < parameters.size(); i++)
        if (!parameters[i]->isInstruction())
            parameters[i]->Setreference();
}

void TamguPredicate::Setreference(short inc) {
    reference += inc;
    protect = 0;
    for (long i = 0; i < parameters.size(); i++)
        if (!parameters[i]->isInstruction())
            parameters[i]->Setreference(inc);
}

void TamguPredicate::Setprotect(bool v) {
    protect = v;
    for (long i = 0; i < parameters.size(); i++)
        if (!parameters[i]->isInstruction())
            parameters[i]->Setprotect(v);
}

void TamguPredicate::Popping() {
    protect = false;
    if (reference <= 0)
        protect = true;
    for (long i = 0; i < parameters.size(); i++)
        if (!parameters[i]->isInstruction())
            parameters[i]->Popping();
}

void TamguPredicate::Resetreference(short inc) {
    for (long i = 0; i < parameters.size(); i++) {
        if (!parameters[i]->isInstruction()) {
            if (protect)
                parameters[i]->Setprotect(1);
            parameters[i]->Resetreference(inc);
        }
    }
    inc = reference - inc;
    if (inc <= 0) {
        reference.store(0);
        if (!protect) {
            if (idtracker != -1)
                globalTamgu->RemoveFromTracker(idtracker);
            delete this;
        }
    }
    else
        reference.store(inc);
}

void TamguPredicateAsVariableInstance::Setreference() {
    predicate_name->Setreference();
    TamguPredicate::Setreference();
}

void TamguPredicateAsVariableInstance::Setreference(short inc) {
    predicate_name->Setreference(inc);
    TamguPredicate::Setreference(inc);
}

void TamguPredicateAsVariableInstance::Setprotect(bool v) {
    predicate_name->Setprotect(v);
    TamguPredicate::Setprotect(v);
}

void TamguPredicateAsVariableInstance::Popping() {
    predicate_name->Popping();
    TamguPredicate::Popping();
}

void TamguPredicateAsVariableInstance::Resetreference(short inc) {
    predicate_name->Resetreference(inc);
    TamguPredicate::Resetreference(inc);
}

void TamguPredicateFunction::Resetreference(short inc) {
    TamguPredicate::Resetreference(inc);
}


string TamguPredicateAction::String() {
    string s;
    switch (name) {
        case a_asserta:
        case a_dependency_asserta:
            s = "asserta";
            break;
        case a_assertz:
        case a_dependency_assertz:
            s = "assertz";
            break;
        case a_retract:
        case a_dependency_retract:
            s = "retract";
            break;
        case a_findall:
            s = "findall";
            break;
        default:
            return("");
    }
    
    s += "(";
    if (parameters.size()) {
        for (long i = 0; i < parameters.size(); i++) {
            if (i)
                s += ",";
            if (parameters[i]->isString())
                s += parameters[i]->String();
            else
                s += parameters[i]->String();
        }
    }
    s += ")";
    return(s);
}

void TamguPredicateAction::Setstring(string& s, short idthread) {
    switch (name) {
        case a_asserta:
        case a_dependency_asserta:
            s = "asserta";
            break;
        case a_assertz:
        case a_dependency_assertz:
            s = "assertz";
            break;
        case a_retract:
        case a_dependency_retract:
            s = "retract";
            break;
        default:
            s = "";
            return;
    }
    
    s += "(";
    if (parameters.size()) {
        for (long i = 0; i < parameters.size(); i++) {
            if (i)
                s += ",";
            if (parameters[i]->isString())
                s += parameters[i]->String();
            else
                s += parameters[i]->String();
        }
    }
    s += ")";
}

string TamguBasePredicateVariable::String() {
    Tamgu* dom = globalTamgu->Declarator(predicatedico, globalTamgu->GetThreadid());
    if (dom == aNULL)
        return globalTamgu->Getsymbol(name);
    
    Tamgu* d = dom->Declaration(predicatedico);
    
    if (d == NULL || !d->Checkdico(name))
        return globalTamgu->Getsymbol(name);
    
    stringstream v;
    TamguPredicateVariableInstance* vx = (TamguPredicateVariableInstance*)d->Getdico(name)->Variable((TamguDeclaration*)dom);
    v << globalTamgu->Getsymbol(name) << vx->name;
    if (vx->value != aUNIVERSAL && vx->value != aNOELEMENT)
        v << ":" << vx->value->String();
    if (globalTamgu->short_string)
        return v.str().substr(0, globalTamgu->short_string);
    return v.str();
}

string TamguPredicateVariableInstance::String() {
    Tamgu* dom = globalTamgu->Declarator(predicatezone, globalTamgu->GetThreadid());
    if (dom == aNULL)
        return globalTamgu->Getsymbol(name);
    
    dom->Setdomainlock();
    TamguPredicateVariableInstance* vx = (TamguPredicateVariableInstance*)Variable((TamguDeclaration*)dom);
    dom->Resetdomainlock();
    
    stringstream s;
    if (merge) {
        if (vx->value->Type() == a_vector && vx->value->Size() == 1) {
            Tamguvector* kvect = (Tamguvector*)vx->value;
            string val = kvect->values[0]->String();
            s << "|";
            if (val[0] == '[' || val[0] == '{') {
                if (val[1] != ']' && val[1] != '}') {
                    val = val.substr(1, val.size() - 2);
                    s << val;
                }
            }
        }
        else
            s << "|" << vx->value->String();
    }
    else {
        if (vx->value == aUNIVERSAL)
            s << "_";
        else {
            if (vx->value != aNOELEMENT) {
                s << vx->value->String();
            }
            else
                if (globalTamgu->Getsymbol(vx->name) != "")
                    s << globalTamgu->Getsymbol(vx->name) << vx->name;
                else
                    s << globalTamgu->Getsymbol(vx->labelname) << vx->name;
        }
    }
    return s.str();
}

string TamguPredicateTerm::String() {
    string v;
    if (name != a_universal)
        v = globalTamgu->Getsymbol(name);
    v += "(";
    for (long i = 0; i < parameters.size(); i++) {
        if (i)
            v += ",";
        if (globalTamgu->short_string) {
            string vsub = parameters[i]->String();
            if (vsub.size() < globalTamgu->short_string)
                v += vsub;
            else
                v += vsub.substr(0, globalTamgu->short_string) + "..";
        }
        else
            v += parameters[i]->String();
        
    }
    v += ")";
    return v;
}

string TamguPredicateConcept::String() {
    string v;
    if (name != a_universal)
        v = globalTamgu->Getsymbol(name);
    v += "(";
    for (long i = 0; i < parameters.size(); i++) {
        if (i)
            v += ",";
        if (globalTamgu->short_string) {
            string vsub = parameters[i]->String();
            if (vsub.size() < globalTamgu->short_string)
                v += vsub;
            else
                v += vsub.substr(0, globalTamgu->short_string) + "..";
        }
        else
            v += parameters[i]->String();
    }
    v += ")";
    return v;
}


string TamguPredicate::String() {
    string v = globalTamgu->Getsymbol(name);
    string vsub;
    long sz = parameters.size();
    
    if (sz) {
        v += "(";
        for (long i = 0; i < sz; i++) {
            if (i)
                v += ",";
            if (parameters[i]->isString())
                vsub = parameters[i]->JSonString();
            else
                vsub = parameters[i]->String();
            
            if (globalTamgu->short_string) {
                if (vsub.size() < globalTamgu->short_string)
                    v += vsub;
                else
                    v += vsub.substr(0, globalTamgu->short_string) + "..";
            }
            else
                v += vsub;
        }
        v += ")";
    }
    return v;
}

void TamguPredicateTerm::Setstring(string& v, short idthread) {
    if (name == a_universal)
        v = "";
    else
        v = globalTamgu->Getsymbol(name);
    v += "(";
    for (long i = 0; i < parameters.size(); i++) {
        if (i)
            v += ",";
        if (parameters[i]->isString())
            v += parameters[i]->JSonString();
        else
            v += parameters[i]->String();
    }
    v += ")";
}

void TamguPredicateConcept::Setstring(string& v, short idthread) {
    if (name == a_universal)
        v = "";
    else
        v = globalTamgu->Getsymbol(name);
    v += "(";
    for (long i = 0; i < parameters.size(); i++) {
        if (i)
            v += ",";
        if (parameters[i]->isString())
            v += parameters[i]->JSonString();
        else
            v += parameters[i]->String();
    }
    v += ")";
}


void TamguPredicate::Setstring(string& v, short idthread) {
    v = globalTamgu->Getsymbol(name);
    long sz = parameters.size();
    
    if (sz) {
        v += "(";
        for (long i = 0; i < sz; i++) {
            if (i)
                v += ",";
            if (parameters[i]->isString())
                v += parameters[i]->JSonString();
            else
                v += parameters[i]->String();
        }
        v += ")";
    }
}


string TamguPredicate::JSonString() {
    stringstream res;
    
    res << "[\"" << globalTamgu->Getsymbol(name) << "\"";
    for (long i = 0; i < parameters.size(); i++)
        res << "," << parameters[i]->JSonString();
    res << "]";
    return res.str();
}



string TamguDependency::String() {
    string v = globalTamgu->Getsymbol(name);
    if (features != aNULL) {
        string x = features->String();
        x[0] = '[';
        x[x.size() - 1] = ']';
        v += x;
    }
    
    string vsub;
    long sz = parameters.size();
    if (sz) {
        v += "(";
        for (long i = 0; i < sz; i++) {
            if (i)
                v += ",";
            if (parameters[i]->isString())
                vsub = parameters[i]->JSonString();
            else
                vsub = parameters[i]->String();
            
            if (globalTamgu->short_string) {
                if (vsub.size() < globalTamgu->short_string)
                    v += vsub;
                else
                    v += vsub.substr(0, globalTamgu->short_string) + "..";
            }
            else
                v += vsub;
        }
        v += ")";
    }
    return v;
}

void TamguDependency::Setstring(string& v, short idthread) {
    v = globalTamgu->Getsymbol(name);
    if (features != aNULL) {
        string x = features->String();
        x[0] = '[';
        x[x.size() - 1] = ']';
        v += x;
    }
    
    long sz = parameters.size();
    if (sz) {
        v += "(";
        for (long i = 0; i < sz; i++) {
            if (i)
                v += ",";
            if (parameters[i]->isString())
                v += parameters[i]->JSonString();
            else
                v += parameters[i]->String();
        }
        v += ")";
    }
}

string TamguDependency::JSonString() {
    stringstream res;
    
    res << "[\"" << globalTamgu->Getsymbol(name) << "\"";
    if (features == aNULL)
        res << "{}";
    else
        res << features->JSonString();
    for (long i = 0; i < parameters.size(); i++)
        res << "," << parameters[i]->JSonString();
    res << "]";
    return res.str();
}

//----------------------------------------------------------------------------------------------
Tamgu* TamguPredicate::Vector(short idthread) {
    Tamguvector* kvect = globalTamgu->Providevector();
    
    TamguConstString local(globalTamgu->Getsymbol(name));
    kvect->Push(&local);
    Tamgu* e;
    for (long i = 0; i < parameters.size(); i++) {
        e = parameters[i];
        if (e->Type() == a_term) {
            Tamgu* vloc = e->Vector(idthread);
            kvect->Push(vloc);
        }
        else
            if (e->Type() == a_predicatevar) {
                e = globalTamgu->Getdefinition(e->Name(), idthread);
                if (e == NULL || e == aNOELEMENT) {
                    local.value = globalTamgu->Getsymbol(parameters[i]->Name());
                    kvect->Push(&local);
                }
                else
                    kvect->Push(e);
            }
            else
                kvect->Push(e);
    }
    return kvect;
}

Tamgu* TamguPredicate::Map(short idthread) {
    Tamgumap* kmap = globalTamgu->Providemap();
    
    TamguConstString local(globalTamgu->Getsymbol(name));
    
    kmap->Push("name", &local);
    char buff[] = { '0', 0, 0 };
    long pos = 0;
    Tamgu* e;
    for (long i = 0; i < parameters.size(); i++) {
        e = parameters[i];
        if (i && (i % 10) == 0) {
            buff[1] = '0';
            if (!pos) {
                buff[0] = '1';
                pos = 1;
            }
            else
                buff[0]++;
        }
        if (e->Type() == a_term) {
            Tamgu* locmap = e->Map(idthread);
            kmap->Push(buff, locmap);
        }
        else
            if (e->Type() == a_predicatevar) {
                e = globalTamgu->Getdefinition(e->Name(), idthread);
                if (e == NULL || e == aNOELEMENT) {
                    local.value = globalTamgu->Getsymbol(parameters[i]->Name());
                    kmap->Push(buff, &local);
                }
                else
                    kmap->Push(buff, e);
            }
            else
                kmap->Push(buff, e);
        buff[pos]++;
    }
    return kmap;
}


Tamgu* TamguPredicateTerm::Vector(short idthread) {
    Tamguvector* kvect = globalTamgu->Providevector();
    
    TamguConstString local(globalTamgu->Getsymbol(name));
    
    kvect->Push(&local);
    Tamgu* e;
    for (long i = 0; i < parameters.size(); i++) {
        e = parameters[i];
        if (e->Type() == a_term) {
            Tamgu* vloc = e->Vector(idthread);
            kvect->Push(vloc);
        }
        else
            if (e->Type() == a_predicatevar) {
                e = globalTamgu->Getdefinition(e->Name(), idthread);
                if (e == NULL || e == aNOELEMENT) {
                    local.value = globalTamgu->Getsymbol(parameters[i]->Name());
                    kvect->Push(&local);
                }
                else
                    kvect->Push(e);
            }
            else
                kvect->Push(e);
    }
    return kvect;
}

Tamgu* TamguPredicateTerm::Map(short idthread) {
    Tamgumap* kmap = globalTamgu->Providemap();
    
    TamguConstString local(globalTamgu->Getsymbol(name));
    
    kmap->Push("name", &local);
    char buff[] = { '0', 0, 0 };
    long pos = 0;
    Tamgu* e;
    for (long i = 0; i < parameters.size(); i++) {
        e = parameters[i];
        if (i && (i % 10) == 0) {
            buff[1] = '0';
            if (!pos) {
                buff[0] = '1';
                pos = 1;
            }
            else
                buff[0]++;
        }
        if (e->Type() == a_term) {
            Tamgu* locmap = e->Map(idthread);
            kmap->Push(buff, locmap);
        }
        else
            if (e->Type() == a_predicatevar) {
                e = globalTamgu->Getdefinition(e->Name(), idthread);
                if (e == NULL || e == aNOELEMENT) {
                    local.value = globalTamgu->Getsymbol(parameters[i]->Name());
                    kmap->Push(buff, &local);
                }
                else
                    kmap->Push(buff, e);
            }
            else
                kmap->Push(buff, e);
        buff[pos]++;
    }
    return kmap;
}


Tamgu* TamguPredicateConcept::Vector(short idthread) {
    Tamguvector* kvect = globalTamgu->Providevector();
    
    TamguConstString local(globalTamgu->Getsymbol(name));
    
    Tamgu* e;
    for (long i = 0; i < parameters.size(); i++) {
        e = parameters[i];
        if (e->isConcept()) {
            Tamgu* vloc = e->Vector(idthread);
            kvect->Push(vloc);
        }
        else
            if (e->Type() == a_predicatevar) {
                e = globalTamgu->Getdefinition(e->Name(), idthread);
                if (e == NULL || e == aNOELEMENT) {
                    local.value = globalTamgu->Getsymbol(parameters[i]->Name());
                    kvect->Push(&local);
                }
                else
                    kvect->Push(e);
            }
            else
                kvect->Push(e);
    }
    
    return kvect;
}

Tamgu* TamguPredicateConcept::Map(short idthread) {
    Tamgumap* kmap = globalTamgu->Providemap();
    
    TamguConstString local(globalTamgu->Getsymbol(name));
    
    kmap->Push("name", &local);
    char buff[] = { '0', 0, 0 };
    long pos = 0;
    Tamgu* e;
    for (long i = 0; i < parameters.size(); i++) {
        e = parameters[i];
        if (i && (i % 10) == 0) {
            buff[1] = '0';
            if (!pos) {
                buff[0] = '1';
                pos = 1;
            }
            else
                buff[0]++;
        }
        if (e->isConcept()) {
            Tamgu* locmap = e->Map(idthread);
            kmap->Push(buff, locmap);
        }
        else
            if (e->Type() == a_predicatevar) {
                e = globalTamgu->Getdefinition(e->Name(), idthread);
                if (e == NULL || e == aNOELEMENT) {
                    local.value = globalTamgu->Getsymbol(parameters[i]->Name());
                    kmap->Push(buff, &local);
                }
                else
                    kmap->Push(buff, e);
            }
            else
                kmap->Push(buff, e);
        buff[pos]++;
    }
    return kmap;
}

bool TamguPredicate::Boolean() {
    return true;
}

Tamgu* Tamgu::Predicate(TamguDeclaration* dom, short idthread) {
    string name;
    TamguPredicate* pred = NULL;
    Tamgu* e;
    long idname = -1;
    if (isVectorContainer()) {
        //It is a vector container
        TamguIteration* itr = Newiteration(false);
        bool first = true;
        //the first element should be the head name
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            if (first == true) {
                name = itr->Valuestring();
                idname = globalTamgu->Getid(name);
                if (!globalTamgu->predicates.check(idname))
                    globalTamgu->predicates[idname] = new TamguPredicateFunction(globalTamgu, NULL, idname);
                pred = new TamguPredicate(globalTamgu, idname);
                first = false;
                continue;
            }
            //Then we proceed with the parameters...
            e = itr->IteratorValue();
            pred->parameters.push_back(e->Atom(true));
            e->Release();
        }
        itr->Release();
        pred->Setprotect(1);
        return pred;
    }
    
    if (isMapContainer()) {
        string n_key = "name";
        e = Value(n_key);
        if (e == aNOELEMENT)
            return globalTamgu->Returnerror("PRE(003): Unknown predicate name", idthread);
        name = e->String();
        if (globalTamgu->symbolIds.find(name) == globalTamgu->symbolIds.end())
            return globalTamgu->Returnerror("PRE(003): Unknown predicate name", idthread);
        idname = globalTamgu->Getid(name);
        if (globalTamgu->predicates.find(idname) == globalTamgu->predicates.end())
            return globalTamgu->Returnerror("PRE(003): Unknown predicate name", idthread);
        
        pred = new TamguPredicate(globalTamgu, idname);
        
        long sz = Size() - 1;
        char buff[] = { '0', 0, 0 };
        long pos = 0;
        for (long i = 0; i < sz; i++) {
            if (i && (i % 10) == 0) {
                buff[1] = '0';
                if (!pos) {
                    buff[0] = '1';
                    pos = 1;
                }
                else
                    buff[0]++;
            }
            n_key = buff;
            e = Value(n_key);
            if (e == aNOELEMENT)
                return globalTamgu->Returnerror("PRE(004): Missing index parameter in the map", idthread);
            pred->parameters.push_back(e->Atom(true));
            buff[pos]++;
        }
        
        pred->Setprotect(1);
        return pred;
    }
    
    return globalTamgu->Returnerror("PRE(005): Cannot build a predicate out of that object", idthread);
}


TamguPredicate::TamguPredicate(short n, TamguGlobal* g, short t, Tamgu* parent) : name(n), TamguReference(g, parent) {
    fully_unified = false;
    shared = false;
    ptype = t;
    terminal = false;
    disjunction = false;
    negation = false;
    investigate |= is_predicate;
}

//This is a hack (quite ugly) which is used to avoid recording into the tracker temporary elements...
TamguPredicate::TamguPredicate(TamguGlobal* g, short n) : name(n) {
    fully_unified = false;
    shared = false;
    terminal = false;
    ptype = a_predicate;
    disjunction = false;
    negation = false;
    investigate |= is_predicate;
}

TamguPredicateConcept::TamguPredicateConcept(TamguGlobal* g, short n, Tamgu* parent) : TamguBasePredicateVariable(g, n, parent) {
    value = NULL;
    fconcept = (TamguFunctionLambda*)globalTamgu->concepts[n];
}

void TamguPredicateRuleElement::Addtail(TamguPredicateRule* pred, long& disj) {
    if (instructions.size() == 0)
        return;
    if (instructions[0]->Type() != a_predicateruleelement) {
        if (disjunction && disj == -1)
            disj = pred->instructions.size();
        TamguPredicateRuleItem* i = new TamguPredicateRuleItem(globalTamgu, instructions[0], negation, disjunction, pred->instructions.size());
        pred->instructions.push_back(i);
    }
    
    if (instructions.back()->Type() == a_predicateruleelement)
        ((TamguPredicateRuleElement*)instructions.back())->Addtail(pred, disj);
    Release();
}

void TamguPredicateRule::addfinal(TamguPredicateContainer* kpc) {
    kpc->rules[name].push_back(this);
}


//-------------------------------------------------------------------------------------

bool TamguPredicateVariableInstance::Insertvalue(Tamgu* dom, Tamgu* val, basebin_hash<Tamgu*>& kept) {
    TamguPredicateVariableInstance* v = (TamguPredicateVariableInstance*)Variable((TamguDeclaration*)dom);
    
    if (v->value != aNOELEMENT && v->value != aUNIVERSAL) {
        if (v->value->Insertvalue(dom, val, kept))
            return true;
    }
    //specific case...
    if (val->isConcept())
        val = val->Evalue((TamguDeclaration*)dom, globalTamgu->GetThreadid(), true);
    
    kept[name] = v->value;
    v->value = val;
    val->Setreference(reference);
    return true;
}

bool TamguPredicateTerm::Insertvalue(Tamgu* dom, Tamgu* val, basebin_hash<Tamgu*>& kept) {
    TamguPredicateTerm* kpt = (TamguPredicateTerm*)val;
    for (long i = 0; i < parameters.size(); i++)
        parameters[i]->Insertvalue(dom, kpt->parameters[i], kept);
    return true;
}

bool TamguPredicateConcept::Insertvalue(Tamgu* dom, Tamgu* val, basebin_hash<Tamgu*>& kept) {
    TamguPredicateConcept* kpt = (TamguPredicateConcept*)val;
    for (long i = 0; i < parameters.size(); i++)
        parameters[i]->Insertvalue(dom, kpt->parameters[i], kept);
    return true;
}

bool Tamguvector::Insertvalue(Tamgu* dom, Tamgu* val, basebin_hash<Tamgu*>& kept) {
    Tamguvector* kpt = (Tamguvector*)val;
    for (BLONG i = 0; i < values.size(); i++)
        values[i]->Insertvalue(dom, kpt->values[i], kept);
    return true;
}

bool Tamguframevector::Insertvalue(Tamgu* dom, Tamgu* val, basebin_hash<Tamgu*>& kept) {
    Tamguvector* kpt = (Tamguvector*)val;
    for (BLONG i = 0; i < values.size(); i++) {
        if (!check_frame(kpt->values[i]))
            return globalTamgu->Returnerror(e_error_on_frame_vector);
    }
    for (BLONG i = 0; i < values.size(); i++) {
        values[i]->Insertvalue(dom, kpt->values[i], kept);
    }
    return true;
}


bool TamguConstmap::Insertvalue(Tamgu* dom, Tamgu* val, basebin_hash<Tamgu*>& kept) {
    TamguConstmap* kpt = (TamguConstmap*)val;
    for (BLONG i = 0; i < values.size(); i++) {
        keys[i]->Insertvalue(dom, kpt->keys[i], kept);
        values[i]->Insertvalue(dom, kpt->values[i], kept);
    }
    return true;
}

//---------------------------------------------------------------------
//Unification of the rules with the predicate variables...
Tamgu* TamguPredicateVariableInstance::Value(TamguDeclaration* dom) {
    Tamgu* v = dom->declarations.scoot(this, name);
    return ((TamguPredicateVariableInstance*)v)->value;
}

Tamgu* TamguPredicateVariableInstance::Variable(TamguDeclaration* dom) {
    return dom->declarations.scoot(this, name);
}

Tamgu* TamguPredicateVariableInstance::VariableValue(TamguDeclaration* dom, short idthread) {
    if (value->isConcept())
        return value->Evalue(dom, idthread, false);
    return value;
}

//---------------------------------------------------------------------

Tamgu* Tamgusynode::ExtractPredicateVariables(Tamgu* context, TamguDeclaration* dom, Tamgu* previousinstance, Tamgu* e, short idthread, bool root) {
    //In this case, we are dealing with an element from the new rule to apply...
    //The previousinstance is the element which has been extracted from the goal stack...
    TamguPredicateVariableInstance* kpvi;
    if (name == a_universal) {
        kpvi = globalTamgu->Providevariableinstance(a_universal, idthread);
        kpvi->Setreference();
        dom->Setdomainlock();
        dom->declarations[kpvi->Name()] = kpvi;
        dom->Resetdomainlock();
    }
    else {
        if (context->Checkdico(name))
            kpvi = context->Getdico(name);
        else {
            kpvi = globalTamgu->Providevariableinstance(name, idthread);
            dom->Setdomainlock();
            context->Setdico(name, kpvi);
            kpvi->Setreference();
            dom->declarations[kpvi->Name()] = kpvi;
            dom->Resetdomainlock();
        }
    }
    
    if (previousinstance == NULL) {
        if (kpvi->value != this) {
            kpvi->value->Resetreference(kpvi->reference);
            kpvi->value = this;
            Setreference(kpvi->reference);
        }
        return kpvi;
    }
    
    return kpvi->ExtractPredicateVariables(context, dom, previousinstance, NULL, idthread, root);
}


Tamgu* TamguBasePredicateVariable::ExtractPredicateVariables(Tamgu* context, TamguDeclaration* dom, Tamgu* previousinstance, Tamgu* e, short idthread, bool root) {
    //In this case, we are dealing with an element from the new rule to apply...
    //The previousinstance is the element which has been extracted from the goal stack...
    TamguPredicateVariableInstance* kpvi;
    if (context->Checkdico(name))
        kpvi = context->Getdico(name);
    else {
        kpvi = globalTamgu->Providevariableinstance(name, idthread);
        dom->Setdomainlock();
        context->Setdico(name, kpvi);
        kpvi->Setreference();
        dom->declarations[kpvi->Name()] = kpvi;
        dom->Resetdomainlock();
    }
    
    if (previousinstance == NULL)
        return kpvi;
    
    return kpvi->ExtractPredicateVariables(context, dom, previousinstance, NULL, idthread, root);
}

Tamgu* TamguPredicateVariableExchange::ExtractPredicateVariables(Tamgu* context, TamguDeclaration* dom, Tamgu* previousinstance, Tamgu* e, short idthread, bool root) {
    //In this case, we are dealing with an element from the new rule to apply...
    //The previousinstance is the element which has been extracted from the goal stack...
    short sz = globalTamgu->Stacksize(idthread);
    Tamgu* top = NULL;
    short i = sz - 1;
    while (i >= 0) {
        top = globalTamgu->Stack(i--, idthread);
        if (top->Type() == a_predicatedomain)
            break;
    }
    TamguPredicateVariableInstanceExchange* kpvi = new TamguPredicateVariableInstanceExchange(top, exchange, globalTamgu->GetName(idthread), name, idthread);
    dom->Setdomainlock();
    context->Setdico(name, kpvi);
    kpvi->Setreference();
    dom->declarations[kpvi->Name()] = kpvi;
    dom->Resetdomainlock();
    return kpvi;
}

Tamgu* TamguPredicateVariableInstance::ExtractPredicateVariables(Tamgu* context, TamguDeclaration* dom,
                                                                 Tamgu* previousinstance, Tamgu* E, short thread_or_name, bool root) {
    if (previousinstance->Type() == a_instance) {
        previousinstance = previousinstance->Variable(dom);
        Tamgu* val = previousinstance->VariableValue(dom, thread_or_name);
        if (val != aNOELEMENT) {
            if (value == aNOELEMENT) {
                value = val->Atom(true);
                value->Setreference(reference);
            }
            else {
                if (!value->Unify(dom, val))
                    return NULL;
            }
        }
        
        //previousinstance is the variable, which should be matched with the new one ("this") through a new rule...
        //Example:
        //    acall(?X) : ...
        //    p = acall(?A);
        //In this example, previousinstance is ?A and "this" is ?X, the system tries to match ?A with ?X
        //We then simply record in our domain a link between the old name and the new name...
        //since the "dico" records a link between each variable and their pointer, we can simply traverse the domain
        //to find the top value... see TamguPredicateVariableInstance::Value and TamguPredicateVariableInstance::Variable
        //We have here a simple chained list of instance variables...
        
        //We reuse thread_or_name in this context...
        thread_or_name = previousinstance->Name();
        if (dom->Declaration(thread_or_name) != this) {
            dom->declarations[thread_or_name] = this;
            Setreference();
        }
        return this;
    }
    
    if (previousinstance != aNOELEMENT) {
        if (value == aNOELEMENT) {
            value = previousinstance->Atom(true);
            value->Setreference(reference);
        }
        else {
            if (!value->Unify(dom, previousinstance))
                return NULL;
        }
    }
    
    return this;
}

Tamgu* Tamgu::ExtractPredicateVariables(Tamgu* contextualpattern, TamguDeclaration* dom, Tamgu* param, Tamgu* e, short idthread, bool root) {
    if (e == NULL) {
        dom->Setfail(false);
        e = Eval(contextualpattern, dom, idthread);
        if (dom->Failed())
            return this;
    }
    if (param != NULL) {
        if (param->Type() == a_instance) {
            TamguPredicateVariableInstance* variable = (TamguPredicateVariableInstance*)param->Variable(dom);
            if (variable->value == aUNIVERSAL) {
                e->Release();
                return param;
            }
            
            if (variable->value == aNOELEMENT) {
                //In this case, we need to duplicate this variable, in order to clean it later on...
                //Otherwise, it will keep its value for the rest of the evaluation
                //we create a new predicate instance, with the same label but a new name...
                short ln = ((TamguPredicateVariableInstance*)param)->labelname;
                TamguPredicateVariableInstance* kpvi = globalTamgu->Providevariableinstance(ln, idthread);
                
                kpvi->value = e->AtomNoConst();
                
                dom->Setdomainlock();
                contextualpattern->Setdico(ln, kpvi);
                dom->declarations[kpvi->Name()] = kpvi;
                dom->declarations[param->Name()] = kpvi;
                kpvi->Setreference(2);
                dom->Resetdomainlock();
                return kpvi;
            }
            
            //else in this case, we use the value of that variable to compare it with the one expected stored in 'e'
            param = variable->value;
        }
        
        if (e != aUNIVERSAL && !param->isNULL() && !e->same(param)->Boolean()) {
            e->Release();
            return NULL;
        }
    }
    return e;
}


Tamgu* TamguConstvectormerge::ExtractPredicateVariables(Tamgu* context, TamguDeclaration* dom,
                                                        Tamgu* source_variable, Tamgu* local_source,
                                                        short idthread, bool root) {
    local_source = source_variable;
    Tamgu* kval;

    if (source_variable != NULL) {
        if (source_variable->Type() != a_vector) {
            if (source_variable->Type() != a_instance)
                return NULL;
            local_source = source_variable->Variable(dom);
            kval = local_source->VariableValue(dom, idthread);
            if (kval != aNOELEMENT)
                local_source = kval;
        }
    }
    
    long szc = -1;
    source_variable = local_source;
    long i;
    Tamguvector* localvect = NULL;
    if (local_source != NULL) {
        if (local_source->Type() == a_vector) {
            szc = local_source->Size();
            if (szc) {
                Tamgu* lst = local_source->Last();
                if (lst->isMerge()) {
                    localvect = globalTamgu->Providevector();
                    for (i = 0; i < szc - 1; i++)
                        localvect->values.push_back(local_source->getvalue(i));
                    lst = lst->VariableValue(dom, idthread)->getvalue(0)->VariableValue(dom, idthread);
                    for (i = 0; i < lst->Size(); i++)
                        localvect->values.push_back(lst->getvalue(i));
                    local_source = localvect;
                    szc = local_source->Size();
                }
                else
                    localvect = NULL;
            }
        }
        else
            if (!local_source->isPredicateVariable()) {
                return NULL;
            }
    }
    
    Tamguvector* vect = globalTamgu->Providevector();
    for (i = 0; i < values.size(); i++) {
        kval = values[i];
        if (szc != -1) {
            //then, if it is the last element, then we associate our element with the rest of the vector
            if (i == (long)values.size() - 1) {
                if (values.size() == 1)
                    kval = kval->ExtractPredicateVariables(context, dom, local_source, NULL, idthread, false);
                else {
                    //Else we need to extract a subset of that vector
                    Tamguvector subvect;
                    for (long j = i; j < local_source->Size(); j++)
                        subvect.values.push_back(((Tamguvector*)local_source)->values[j]);
                    kval = kval->ExtractPredicateVariables(context, dom, &subvect, NULL, idthread, false);
                }
            }
            else {
                if (szc <= i) {
                    if (localvect != NULL) {
                        localvect->values.clear();
                        localvect->Release();
                    }
                    vect->Release();
                    return NULL;
                }
                kval = kval->ExtractPredicateVariables(context, dom, ((Tamguvector*)local_source)->values[i], NULL, idthread, false);
            }
        }
        else
            kval = kval->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, false);
        
        if (kval == NULL) {
            if (localvect != NULL) {
                localvect->values.clear();
                localvect->Release();
            }
            vect->Release();
            return NULL;
        }
        vect->Push(kval);
    }
    
    if (localvect != NULL) {
        localvect->values.clear();
        localvect->Release();
    }
    
    dom->Setdomainlock();
    TamguPredicateVariableInstance* kpvi = globalTamgu->Providevariableinstance(a_map, idthread);
    
    if (isMerge())
        kpvi->merge = true;
    
    kpvi->value = vect;
    kpvi->Setreference();
    dom->declarations[kpvi->name] = kpvi;
    
    if (source_variable != NULL && source_variable->Type() == a_instance) {
        context->Setdico(source_variable->Name(), kpvi);
        dom->declarations[source_variable->Name()] = kpvi;
        kpvi->Setreference();
    }
    dom->Resetdomainlock();
    return kpvi;
}

Tamgu* Tamguvector::ExtractPredicateVariables(Tamgu* context, TamguDeclaration* dom, Tamgu* source_variable, Tamgu* local_source, short idthread, bool root) {
    local_source = source_variable;
    Tamgu* kval;

    if (source_variable != NULL) {
        if (!source_variable->isVectorContainer()) {
            if (source_variable->Type() != a_instance)
                return NULL;
            local_source = source_variable->Variable(dom);
            kval = local_source->VariableValue(dom, idthread);
            if (kval != aNOELEMENT)
                local_source = kval;
        }
    }
    
    source_variable = local_source;
    long i, j;
    long container_size = values.size();
    long source_size = -1;
    TamguPredicateVariableInstance* kpvi;
    Tamguvector* localvect = NULL;
    if (local_source != NULL) {
        if (local_source->isVectorContainer()) {
            source_size = local_source->Size();
            if (source_size) {
                if (local_source->Last()->isMerge()) {
                    if (!container_size) {
                        return NULL;
                    }
                    kpvi = (TamguPredicateVariableInstance*)local_source->Last();
                    localvect = globalTamgu->Providevector();
                    for (i = 0; i < source_size - 1; i++)
                        localvect->values.push_back(local_source->getvalue(i));
                    kval = kpvi->value->getvalue(0)->VariableValue(dom, idthread);
                    source_size = kval->Size();
                    for (i = 0; i < source_size; i++)
                        localvect->values.push_back(kval->getvalue(i));
                    local_source = localvect;
                    source_size = localvect->values.size();
                }
                else
                    localvect = NULL;
            }
            if (!container_size || !values.back()->isMerge()) {
                if (source_size != container_size) {
                    if (localvect != NULL) {
                        localvect->values.clear();
                        localvect->Release();
                    }
                    return NULL;
                }
            }
        }
        else
            if (!local_source->isPredicateVariable()) {
                return NULL;
            }
    }
    
    Tamgu* e;
    Tamguvector* vect = globalTamgu->Providevector();
    bool merge = false;
    
    for (i = 0; i < container_size; i++) {
        e = values[i];
        if (source_size != -1) {
            //In the case of merge, we need to extract from c the rest of the vector...
            if (e->isMerge()) {
                if (local_source == localvect) {
                    merge = true;
                    localvect->values.erase(localvect->values.begin(), localvect->values.begin() + i);
                    e = e->ExtractPredicateVariables(context, dom, localvect, NULL, idthread, false);
                }
                else {
                    localvect = globalTamgu->Providevector();
                    
                    merge = false;
                    for (j = i; j < local_source->Size(); j++) {
                        kval = local_source->getvalue(j);
                        if (kval->isObject()) {
                            merge = true;
                            kval->Setreference();
                        }
                        localvect->values.push_back(kval);
                    }
                    e = e->ExtractPredicateVariables(context, dom, localvect, NULL, idthread, false);
                    if (merge) {
                        for (j = 0; j < localvect->values.size(); j++)
                            localvect->values[j]->Resetreference();
                    }
                    merge = true;
                    localvect->values.clear();
                    localvect->Release();
                    localvect = NULL;
                }
            }
            else {
                if (source_size <= i) {
                    vect->Release();
                    if (localvect != NULL) {
                        localvect->values.clear();
                        localvect->Release();
                    }
                    return NULL;
                }
                kval = local_source->getvalue(i);
                e = e->ExtractPredicateVariables(context, dom, kval, NULL, idthread, false);
                kval->Release();
            }
        }
        else
            e = e->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, false);
        
        if (e == NULL) {
            vect->Release();
            if (localvect != NULL) {
                localvect->values.clear();
                localvect->Release();
            }
            return NULL;
        }
        vect->Push(e);
    }
    
    if (localvect != NULL) {
        localvect->values.clear();
        localvect->Release();
    }
    
    if (root || merge) {
        kpvi = globalTamgu->Providevariableinstance(a_vector, idthread);
        
        dom->Setdomainlock();
        kpvi->value = vect;
        dom->declarations[kpvi->name] = kpvi;
        kpvi->Setreference();
        if (source_variable != NULL && source_variable->Type() == a_instance) {
            context->Setdico(source_variable->Name(), kpvi);
            dom->declarations[source_variable->Name()] = kpvi;
            kpvi->Setreference();
        }
        dom->Resetdomainlock();
        return kpvi;
    }
    return vect;
}

Tamgu* TamguConstmap::ExtractPredicateVariables(Tamgu* context, TamguDeclaration* dom, Tamgu* source_variable, Tamgu* local_source, short idthread, bool root) {
    local_source = source_variable;
    Tamgu* ck;

    if (source_variable != NULL) {
        if (!source_variable->isMapContainer()) {
            if (source_variable->Type() != a_instance)
                return NULL;
            local_source = source_variable->Variable(dom);
            ck = local_source->VariableValue(dom, idthread);
            if (ck != aNOELEMENT)
                local_source = ck;
        }
    }
    
    
    long sz = values.size();
    long szc = -1;
    if (local_source != NULL) {
        if (local_source->isMapContainer()) {
            szc = local_source->Size();
            if ((!szc && sz) || (szc && !sz)) {
                return NULL;
            }
            //No merge, then the size should be equal
            if (!sz || keys.back()->Name() != a_pipe) {
                if (sz != szc) {
                    return NULL;
                }
            }
        }
        else
            if (!local_source->isPredicateVariable()) {
                return NULL;
            }
    }
    
    TamguIteration* iter;
    if (local_source == NULL)
        iter = new TamguIteration(false);
    else
        iter = local_source->Newiteration(false);
    iter->Begin();
    TamguConstmap* kmap = new TamguConstmap;
    Tamgu* ek;
    Tamgu* ev;
    Tamgu* cv;
    long i;
    for (i = 0; i < sz; i++) {
        ev = values[i];
        ek = keys[i];
        if (szc != -1) {
            //In the case of merge, we need to extract from c the rest of the vector...
            if (ek->Name() == a_pipe) {
                //A merge...
                //We need to access all the remaining elements...
                Tamgu* remaining = local_source->Newinstance(idthread);
                while (iter->End() == aFALSE) {
                    ck = iter->IteratorKey();
                    cv = iter->IteratorValue();
                    remaining->Push(ck, cv);
                    ck->Release();
                    cv->Release();
                    iter->Next();
                }
                ev = ev->ExtractPredicateVariables(context, dom, remaining, NULL, idthread, false);
                remaining->Release();
            }
            else {
                if (szc <= i) {
                    iter->Release();
                    kmap->Release();
                    return NULL;
                }
                ck = iter->IteratorKey();
                cv = iter->IteratorValue();
                ek = ek->ExtractPredicateVariables(context, dom, ck, NULL, idthread, false);
                ev = ev->ExtractPredicateVariables(context, dom, cv, NULL, idthread, false);
                ck->Release();
                cv->Release();
                iter->Next();
            }
        }
        else {
            ek = ek->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, false);
            ev = ev->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, false);
        }
        
        if (ek == NULL || ev == NULL) {
            iter->Release();
            kmap->Release();
            return NULL;
        }
        
        kmap->keys.push_back(ek);
        kmap->values.push_back(ev);
    }
    iter->Release();
    
    if (root || merge) {
        TamguPredicateVariableInstance* kpvi = globalTamgu->Providevariableinstance(a_map, idthread);
        
        dom->Setdomainlock();
        kpvi->value = kmap;
        dom->declarations[kpvi->name] = kpvi;
        kpvi->Setreference();
        if (local_source != NULL && local_source->Type() == a_instance) {
            context->Setdico(local_source->Name(), kpvi);
            dom->declarations[local_source->Name()] = kpvi;
            kpvi->Setreference();
        }
        dom->Resetdomainlock();
        return kpvi;
    }
    return kmap;
}

Tamgu* TamguPredicate::ExtractPredicateVariables(Tamgu* context, TamguDeclaration* dom, Tamgu* C, Tamgu* E, short idthread, bool root) {
    Tamgu* c = C;
    long i;
    bool param = false;
    TamguPredicate* pred;
    
    if (C != NULL) {
        if (C->Type() == a_predicate) {
            if (!C->isName(name) || C->Size() != parameters.size()) {
                return NULL;
            }
            param = true;
        }
        else {
            if (C->Type() != a_instance)
                return NULL;
            c = C->Variable(dom);
            Tamgu* val = c->VariableValue(dom, idthread);
            if (val != aNOELEMENT) {
                c = val;
                if (c->Type() != a_predicate || !c->isName(name) || c->Size() != parameters.size())
                    return NULL;
            }
            
            if (c->Type() == a_predicate)
                param = true;
        }
    }
    
    Tamgu* e;
    //term = new TamguPredicateTerm(globalTamgu, name, label);
    pred = (TamguPredicate*)Newinstance(idthread);
    pred->name = name;
    
    for (i = 0; i < parameters.size(); i++) {
        e = parameters[i];
        if (param)
            e = e->ExtractPredicateVariables(context, dom, c->Parameter(i), NULL, idthread, false);
        else
            e = e->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, false);
        
        if (e == NULL) {
            pred->Release();
            return NULL;
        }
        pred->parameters.push_back(e);
    }
    
    if (root) {
        TamguPredicateVariableInstance* kpvi = globalTamgu->Providevariableinstance(a_term, idthread);
        
        dom->Setdomainlock();
        kpvi->value = pred;
        kpvi->Setreference();
        dom->declarations[kpvi->name] = kpvi;
        
        if (c != NULL && c->Type() == a_instance) {
            context->Setdico(c->Name(), kpvi);
            dom->declarations[c->Name()] = kpvi;
            kpvi->Setreference();
        }
        dom->Resetdomainlock();
        return kpvi;
    }
    
    return pred;
}

Tamgu* TamguPredicateTerm::ExtractPredicateVariables(Tamgu* context, TamguDeclaration* dom, Tamgu* C, Tamgu* E, short idthread, bool root) {
    Tamgu* c = C;
    long i;
    bool param = false;
    TamguPredicateTerm* term;
    
    if (C != NULL) {
        if (C->Type() == a_term) {
            if (!C->isName(name) || C->Size() != parameters.size()) {
                return NULL;
            }
            param = true;
        }
        else {
            if (C->Type() != a_instance)
                return NULL;
            c = C->Variable(dom);
            Tamgu* val = c->VariableValue(dom, idthread);
            if (val != aNOELEMENT) {
                c = val;
                if (c->Type() != a_term || !c->isName(name) || c->Size() != parameters.size())
                    return NULL;
            }
            
            if (c->Type() == a_term)
                param = true;
        }
    }
    
    Tamgu* e;
    //term = new TamguPredicateTerm(globalTamgu, name, label);
    term = (TamguPredicateTerm*)Newinstance(idthread);
    term->Set(name);
    
    for (i = 0; i < parameters.size(); i++) {
        e = parameters[i];
        if (param)
            e = e->ExtractPredicateVariables(context, dom, c->Parameter(i), NULL, idthread, false);
        else
            e = e->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, false);
        
        if (e == NULL) {
            term->Release();
            return NULL;
        }
        term->parameters.push_back(e);
    }
    
    if (root) {
        TamguPredicateVariableInstance* kpvi = globalTamgu->Providevariableinstance(a_term, idthread);
        
        dom->Setdomainlock();
        kpvi->value = term;
        kpvi->Setreference();
        dom->declarations[kpvi->name] = kpvi;
        
        if (c != NULL && c->Type() == a_instance) {
            context->Setdico(c->Name(), kpvi);
            dom->declarations[c->Name()] = kpvi;
            kpvi->Setreference();
        }
        dom->Resetdomainlock();
        return kpvi;
    }
    
    return term;
}

Tamgu* TamguPredicateConcept::ExtractPredicateVariables(Tamgu* context, TamguDeclaration* dom, Tamgu* C, Tamgu* E, short idthread, bool root) {
    Tamgu* c = C;
    bool param = false;
    TamguPredicateConcept* concept;
    
    if (C != NULL) {
        if (C->isConcept()) {
            if (!C->isName(name) || C->Size() != parameters.size()) {
                return NULL;
            }
            param = true;
        }
        else {
            if (C->Type() != a_instance)
                return NULL;
            c = C->Variable(dom);
            Tamgu* val = c->VariableValue(dom, idthread);
            if (val != aNOELEMENT) {
                c = val;
                if (!c->isConcept() || !c->isName(name) || c->Size() != parameters.size())
                    return NULL;
            }
            
            if (c->isConcept())
                param = true;
        }
    }
    
    concept = (TamguPredicateConcept*)Newinstance(idthread);
    concept->Set(name);
    
    Tamgu* e;
    long i;
    bool compute = true;
    for (i = 0; i < parameters.size(); i++) {
        e = parameters[i];
        if (param)
            e = e->ExtractPredicateVariables(context, dom, c->Parameter(i), NULL, idthread, false);
        else
            e = e->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, false);
        
        if (e == NULL) {
            concept->Release();
            return NULL;
        }
        
        if (e == aNOELEMENT)
            compute = false;
        
        concept->parameters.push_back(e);
    }
    
    if (compute) {
        e = concept->Evalue(dom, idthread, true);
        if (e == aNOELEMENT)
            e = concept;
        else
            concept->Release();
    }
    else
        e = concept;
    
    if (root) {
        TamguPredicateVariableInstance* kpvi = globalTamgu->Providevariableinstance(a_term, idthread);
        
        dom->Setdomainlock();
        kpvi->value = e;
        kpvi->Setreference();
        dom->declarations[kpvi->name] = kpvi;
        
        if (c != NULL && c->Type() == a_instance) {
            context->Setdico(c->Name(), kpvi);
            dom->declarations[c->Name()] = kpvi;
            kpvi->Setreference();
        }
        dom->Resetdomainlock();
        return kpvi;
    }
    
    return concept;
}
//---------------------------------------------------------------------

TamguPredicate* TamguPredicateAction::Duplicate(Tamgu* context, TamguDeclaration* dom, short idthread) {
    Tamgu * e;
    
    TamguPredicateAction* p;
    
    if (context == NULL && dom == NULL) {
        p = (TamguPredicateAction*)Newinstance(0);
        
        for (long i = 0; i < parameters.size(); i++) {
            e = parameters[i]->Atom(true);
            p->parameters.push_back(e);
        }
        p->Setreference();
        return p;
    }
    
    p = (TamguPredicateAction*)Newinstance(0);
    
    long i, j;
    for (i = 0; i < parameters.size(); i++) {
        e = parameters[i]->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, true);
        if (e == NULL) {
            for (j = 0; j < i; j++) {
                //These elements will be cleared in TamguPredicateRule::Evalue
                if (p->parameters[j]->Type() == a_instance)
                    p->parameters[j] = aNULL;
            }
            p->Release();
            return NULL;
        }
        
        p->parameters.push_back(e);
        e->Setprotect(0);
    }
    
    p->Setreference();
    return p;
}

TamguPredicate* TamguPredicate::Duplicate(Tamgu* context, TamguDeclaration* dom, short idthread) {
    Tamgu * e;
    
    TamguPredicate* p;
    
    if (context == NULL && dom == NULL) {
        p = new TamguPredicate(globalTamgu, name);
        for (long i = 0; i < parameters.size(); i++) {
            e = parameters[i]->Atom(true);
            p->parameters.push_back(e);
        }
        p->Setreference();
        return p;
    }
    
    p = new TamguPredicate(globalTamgu, name);
    
    p->negation = negation;
    p->terminal = terminal;
    
    long i, j;
    for (i = 0; i < parameters.size(); i++) {
        e = parameters[i]->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, true);
        if (e == NULL) {
            for (j = 0; j < i; j++) {
                //These elements will be cleared in TamguPredicateRule::Evalue
                if (p->parameters[j]->Type() == a_instance)
                    p->parameters[j] = aNULL;
            }
            p->Release();
            return NULL;
        }
        
        p->parameters.push_back(e);
        e->Setprotect(0);
    }
    
    p->Setreference();
    return p;
}

TamguPredicate* TamguPredicateAsVariable::Duplicate(Tamgu* context, TamguDeclaration* dom, short idthread) {
    Tamgu * e;
    
    TamguPredicateAsVariableInstance* p = new TamguPredicateAsVariableInstance(name, idvar);
    
    p->negation = negation;
    
    long i, j;
    for (i = 0; i < parameters.size(); i++) {
        e = parameters[i]->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, true);
        if (e == NULL) {
            for (j = 0; j < i; j++) {
                //These elements will be cleared in TamguPredicateRule::Evalue
                if (p->parameters[j]->Type() == a_instance)
                    p->parameters[j] = aNULL;
            }
            p->Release();
            return NULL;
        }
        
        p->parameters.push_back(e);
        e->Setprotect(0);
    }
    p->predicate_name = predicate_name->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, true);
    p->Setreference();
    return p;
}

TamguPredicate* TamguDependency::Duplicate(Tamgu* context, TamguDeclaration* dom, short idthread) {
    Tamgu * e;
    
    TamguDependency* p;
    
    if (context == NULL && dom == NULL) {
        p = new TamguDependency(globalTamgu, aNULL, name, idvar);
        if (features->Type() == a_mapss) {
            p->features = globalTamgu->Providemapss();
            ((Tamgumapss*)p->features)->values = ((Tamgumapss*)features)->values;
        }
        
        for (long i = 0; i < parameters.size(); i++) {
            e = parameters[i]->Atom(true);
            p->parameters.push_back(e);
        }
        p->Setreference();
        return p;
    }
    
    p = new TamguDependency(globalTamgu, aNULL, name, idvar);
    if (features->Type() == a_mapss) {
        p->features = globalTamgu->Providemapss();
        ((Tamgumapss*)p->features)->values = ((Tamgumapss*)features)->values;
    }
    
    p->negation = negation;
    
    long i, j;
    for (i = 0; i < parameters.size(); i++) {
        e = parameters[i]->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, true);
        if (e == NULL) {
            for (j = 0; j < i; j++) {
                //These elements will be cleared in TamguPredicateRule::Evalue
                if (p->parameters[j]->Type() == a_instance)
                    p->parameters[j] = aNULL;
            }
            p->Release();
            return NULL;
        }
        
        p->parameters.push_back(e);
        e->Setprotect(0);
    }
    
    p->Setreference();
    return p;
}

bool TamguPredicate::Copyfrom(TamguPredicate* new_predicate, Tamgu* context, TamguDeclaration* dom, vector<Tamgu*>& headpredicatevalues, short idthread) {
    Tamgu* e;
    
    for (long i = 0; i < parameters.size(); i++) {
        e = parameters[i]->ExtractPredicateVariables(context, dom, headpredicatevalues[i], NULL, idthread, true);
        if (e == NULL) {
            new_predicate->Clear();
            return false;
        }
        
        new_predicate->parameters.push_back(e);
        e->Setreference();
    }
    
    return true;
}

//---------------------------------------------------------------------

Exporting Tamgu* Tamgu::EvaluePredicateVariables(Tamgu* context, TamguDeclaration* dom, short idthread) {
    Tamgu* e = Eval(context, dom, idthread);
    return e->Atom(true);
}

Tamgu* TamguBasePredicateVariable::EvaluePredicateVariables(Tamgu* context, TamguDeclaration* dom, short idthread) {
    TamguPredicateVariableInstance* kpi = globalTamgu->Providevariableinstance(name, idthread);
    dom->Setdomainlock();
    dom->declarations[kpi->name] = kpi;
    dom->Resetdomainlock();
    return kpi;
}

Tamgu* TamguPredicateVariableInstance::EvaluePredicateVariables(Tamgu* context, TamguDeclaration* dom, short idthread) {
    Tamgu* v = Variable(dom);
    Tamgu* val = v->VariableValue(dom, idthread);
    if (val != aNOELEMENT)
        return val;
    return v;
}

Tamgu* Tamguvector::EvaluePredicateVariables(Tamgu* context, TamguDeclaration* dom, short idthread) {
    Tamgu* e;
    
    Tamguvector* vect = globalTamgu->Providevector();
    for (BLONG i = 0; i < values.size(); i++) {
        e = values[i]->EvaluePredicateVariables(context, dom, idthread);
        if (e == NULL)
            vect->Push(values[i]);
        else
            vect->Push(e);
    }
    return vect;
}

Tamgu* TamguConstmap::EvaluePredicateVariables(Tamgu* context, TamguDeclaration* dom, short idthread) {
    Tamgu* e;
    
    TamguConstmap* kmap = new TamguConstmap;
    for (BLONG i = 0; i < values.size(); i++) {
        e = keys[i]->EvaluePredicateVariables(context, dom, idthread);
        if (e == NULL)
            kmap->keys.push_back(values[i]);
        else
            kmap->keys.push_back(e);
        
        e = values[i]->EvaluePredicateVariables(context, dom, idthread);
        if (e == NULL)
            kmap->values.push_back(values[i]);
        else
            kmap->values.push_back(e);
    }
    return kmap;
}

Tamgu* TamguPredicateTerm::EvaluePredicateVariables(Tamgu* context, TamguDeclaration* dom, short idthread) {
    Tamgu * e;
    TamguPredicateTerm* model = new TamguPredicateTerm(globalTamgu, name);
    
    for (long i = 0; i < parameters.size(); i++) {
        e = parameters[i]->EvaluePredicateVariables(context, dom, idthread);
        if (e == NULL)
            model->parameters.push_back(parameters[i]);
        else
            model->parameters.push_back(e);
    }
    return model;
}

Tamgu* TamguPredicateConcept::EvaluePredicateVariables(Tamgu* context, TamguDeclaration* dom, short idthread) {
    Tamgu * e;
    TamguPredicateConcept* model = new TamguPredicateConcept(globalTamgu, name);
    
    for (long i = 0; i < parameters.size(); i++) {
        e = parameters[i]->EvaluePredicateVariables(context, dom, idthread);
        if (e == NULL)
            model->parameters.push_back(parameters[i]);
        else
            model->parameters.push_back(e);
    }
    
    return model;
}

//-------------------------------------------------------------------
Tamgu* Tamgu::Getvalues(TamguDeclaration* dom, char duplicate) {
    if (duplicate == true && !isConst())
        return Atom(true);
    return this;
}

Tamgu* Tamgusynode::Getvalues(TamguDeclaration* dom, char duplicate) {
    return this;
}

Tamgu* TamguPredicateVariableInstance::Getvalues(TamguDeclaration* dom, char duplicate) {
    Tamgu* v = Value(dom);
    v = v->Getvalues(dom, duplicate);
    if (merge && v->isVectorContainer())
        v->Setmerge();
    return v;
}

Tamgu* TamguPredicateVariableInstanceExchange::Getvalues(TamguDeclaration* dom, char duplicate) {
    Tamgu* v = Value(dom);
    v = v->Getvalues(dom, duplicate);
    if (merge && v->isVectorContainer())
        v->Setmerge();
    Tamgu* var = exchange->Eval(domain, aNULL, thread_id);
    var->Put(domain, v, thread_id);
    return v;
}

//TamguPredicate can be extracted from a database, in that case, we want the values
//to be fully unified. We also have to be carreful not to duplicate an already duplicated
//value...
Tamgu* TamguPredicate::Getvalues(TamguDeclaration* dom, char duplicate) {
    if (duplicate != true || fully_unified)
        return this;
    
    duplicate = false;
    for (long i = 0; i < parameters.size(); i++) {
        if (parameters[i]->Type() == a_instance) {
            duplicate = true;
            break;
        }
    }
    
    if (!duplicate)
        return this;
    
    TamguPredicate* term = new TamguPredicate(globalTamgu, name);
    term->fully_unified = true;
    Tamgu* e;
    for (long i = 0; i < parameters.size(); i++) {
        e = parameters[i]->Getvalues(dom, duplicate);
        if (e == aNOELEMENT) {
            if (!duplicate) {
                term->parameters.push_back(parameters[i]);
                continue;
            }
            term->Release();
            return aNOELEMENT;
        }
        term->parameters.push_back(e);
    }
    return term;
}

Tamgu* TamguPredicateTerm::Getvalues(TamguDeclaration* dom, char duplicate) {
    TamguPredicateTerm* term = new TamguPredicateTerm(globalTamgu, name);
    Tamgu* e;
    for (long i = 0; i < parameters.size(); i++) {
        e = parameters[i]->Getvalues(dom, duplicate);
        if (e == aNOELEMENT) {
            if (duplicate == 2) {
                term->parameters.push_back(parameters[i]);
                continue;
            }

            term->Release();
            return aNOELEMENT;
        }
        term->parameters.push_back(e);
    }
    return term;
}

Tamgu* TamguPredicateConcept::Evalue(TamguDeclaration* dom, short idthread, char duplicate) {
    TamguCallFunctionArgsTaskell hfunc(fconcept);
    
    Tamgu* e;
    for (long i = 0; i < parameters.size(); i++) {
        e = parameters[i]->Evalue(dom, idthread, duplicate);
        if (e == aNOELEMENT) {
            if (duplicate == 2) {
                hfunc.arguments.push_back(parameters[i]);
                continue;
            }
            return aNOELEMENT;
        }
        
        if (value != NULL) {
            if (e != args[i])
                value = NULL;
        }
        hfunc.arguments.push_back(e);
    }
    
    if (value != NULL)
        return value;
    
    //All variables will then be evaluated...
    value = hfunc.Eval(aNULL, aNULL, idthread);
    
    //IF THE FUNCTION returns aNOELEMENT, then we cannot do anything can we...
    if (value->isNULL() || value == aFALSE) {
        value = NULL;
        return aNOELEMENT;
    }
    
    args = hfunc.arguments;
    return value;
}


Tamgu* TamguPredicateConcept::Getvalues(TamguDeclaration* dom, char duplicate) {
    return Evalue(dom, globalTamgu->GetThreadid(), duplicate);
}

Tamgu* Tamguvector::Getvalues(TamguDeclaration* dom, char duplicate) {
    Tamguvector* kvect = globalTamgu->Providevector();
    Tamgu* e;
    for (BLONG i = 0; i < values.size(); i++) {
        e = values[i]->Getvalues(dom, duplicate);
        if (e == aNOELEMENT) {
            if (duplicate == 2) {
                kvect->Push(values[i]);
                continue;
            }
            kvect->Release();
            return aNOELEMENT;
        }
        if (e == NULL || e == aUNIVERSAL)
            continue;
        if (e->isMerge()) {
            Tamgu* kv;
            size_t v;
            for (v = 0; v < e->Size(); v++) {
                kv = e->getvalue(v);
                if (kv->Type() == a_vector) {
                    Tamguvector* vect = (Tamguvector*)kv;
                    for (v = 0; v < vect->values.size(); v++)
                        kvect->Push(vect->values[v]);
                    
                    if (!e->isObject())
                        kv->Resetreference();
                    break;
                }
                else
                    kvect->Push(kv);
            }
            if (e->isObject())
                e->Release();
        }
        else
            kvect->Push(e);
    }
    return kvect;
}

Tamgu* Tamguvector::Unifies(TamguDeclaration* dom) {
    long sz = values.size();
    if (sz < 2)
        return this;
    for (long i = 0; i < sz; i++) {
        if (values[i]->Type() == a_instance)
            return Getvalues(dom, 2);
    }
    return this;
}

Tamgu* TamguConstmap::Getvalues(TamguDeclaration* dom, char duplicate) {
    Tamgutreemap* kmap = globalTamgu->Providetreemap();
    Tamgu* ev = aNULL;
    Tamgu* ek;
    for (BLONG i = 0; i < values.size(); i++) {
        ek = keys[i]->Getvalues(dom, duplicate);
        if (ek == aNOELEMENT) {
            ev->Release();
            kmap->Release();
            return aNOELEMENT;
        }
        if (ek == NULL || ek == aUNIVERSAL)
            continue;
        
        ev = values[i]->Getvalues(dom, duplicate);
        if (ev == aNOELEMENT) {
            if (duplicate == 2) {
                kmap->Push(STR(ek->String()), values[i]);
                continue;
            }
            kmap->Release();
            return aNOELEMENT;
        }
        
        if (ev == NULL || ev == aUNIVERSAL)
            continue;
        
        if (ek->Name() == a_pipe) {
            //a merging...
            //in that case, ev must be a map...
            if (ev->isMapContainer()) {
                Tamgu* tobecleaned = ev;
                //We take all the values from ev into the current map...
                TamguIteration* it = ev->Newiteration(false);
                for (it->Begin(); it->End() == aFALSE; it->Next()) {
                    ek = it->IteratorKey();
                    ev = it->IteratorValue();
                    kmap->Push(STR(ek->String()), ev);
                }
                tobecleaned->Release();
                it->Release();
                continue;
            }
        }
        
        kmap->Push(STR(ek->String()), ev);
        ek->Release();
        ev->Release();
    }
    return kmap;
}

Tamgu* TamguConstmap::Unifies(TamguDeclaration* dom) {
    long sz = values.size();
    if (sz < 2)
        return this;
    for (long i = 0; i < sz; i++) {
        if (keys[i]->Type() == a_instance || values[i]->Type() == a_instance)
            return Getvalues(dom, 2);
    }
    return this;
}


Tamgu* TamguInstructionEvaluate::PredicateCreate(TamguPredicate* headrule, long depth) {
    //This a specific case, when the system is looking for a subgoal (as in a negation)
    if (headrule == NULL)
        return aTRUE;
    
    TamguPredicate* pv = new TamguPredicate(globalTamgu, headrule->name);
    Tamgu* e;
    long i;
    for (i = 0; i < headrule->parameters.size(); i++) {
        e = headrule->parameters[i]->Getvalues(dom, true);
        if (e == aNOELEMENT) {
            pv->Release();
            return aFALSE;
        }
        pv->parameters.push_back(e);
        e->Setprotect(0);
    }
    
    if (trace) {
        stringstream s;
        for (i = 0; i < depth; i++)
            s << " ";
        s << "success:" << depth << "=" << pv->String() << endl;
        PreLocalPrint(s.str());
    }
    
    pv->Setreference();
    
    Tamgu* func = headrule->Function();
    if (func != NULL) {
        TamguCallFunction2 kfunc(func);
        func = ((TamguPredicateFunction*)headrule)->object;
        kfunc.arguments.push_back(pv);
        kfunc.Push(func);
        e = kfunc.Eval(this, dom, threadowner);
        func->Protect();
        if (!e->Boolean()) {
            pv->Resetreference();
            if (e->isError())
                return e;
            return aBREAK;
        }
    }
    
    results.push_back(pv);
    return aTRUE;
}

//-----------------------------------------------------
//-----------------------------------------------------
void Displaypredicatestack(const char* ty, TamguDeclaration* dom, TamguPredicate* kpr, VECTE<Tamgu*>& goals, long depth, TamguInstructionEvaluate* ctx) {
    //AffichageDom(dom,depth);
    stringstream s;
    bool displayed = false;
    bool first = true;
    Tamgu* se;
    long i, j, sz = goals.size();
    char sep = ',';
    string n;
    for (i = -1; i < sz; i++) {
        if (i == -1)
            se = kpr;
        else
            se = goals[i];
        if (se == NULL)
            continue;
        if (se->isPredicate()) {
            if (!displayed) {
                for (j = 0; j < depth; j++)
                    s << " ";
                s << ty << ":" << depth << "=";
            }
            
            if (first == false)
                s << sep;
            if (i != -1)
                first = false;
            displayed = true;
            TamguPredicate* kpi = (TamguPredicate*)se;
            n = globalTamgu->Getsymbol(kpi->name);
            if (kpi->isNegation())
                s << "~";
            s << n;
            if (kpi->isConst())
                continue;
            s << "(";
            for (j = 0; j < kpi->parameters.size(); j++) {
                n = kpi->parameters[j]->String();
                if (n.size() > globalTamgu->short_string)
                    n = n.substr(0, globalTamgu->short_string) + "..";
                if (j)
                    s << ",";
                s << n;
            }
            s << ")";
        }
        else {
            if (!displayed) {
                for (j = 0; j < depth; j++)
                    s << " ";
                s << ty << ":" << depth << "=";
            }
            if (first == false)
                s << sep;
            if (i != -1)
                first = false;
            displayed = true;
            if (se->Name() != -1 || (se->Action() >= a_assignement && se->Action() <= a_notin)) {
                n = se->String();
                s << n;
            }
            else
                s << "[i]";
        }
        if (i == -1) {
            if (sz)
                s << " --> ";
            else
                s << ".";
        }
        sep = ',';
        if (se->isDisjunction())
            sep = ';';
    }
    
    if (displayed)
        PreLocalPrint(s.str());
    displayed = false;
}

//---------------------------------------------------------------------

Tamgu* TamguPredicateRuleItem::Unify(Tamgu* context, TamguDeclaration* dom, PredicateInstanceDeclaration* dico, short idthread) {
    if (item->isConst())
        return item;
    
    if (item->isPredicate()) {
        TamguPredicate* kvpred = ((TamguPredicate*)item)->Duplicate(context, dom, idthread);
        if (kvpred == NULL)
            return NULL;
        kvpred->set(negation,disjunction);
        return kvpred;
    }
    
    return new TamguPredicateLocalInstruction(globalTamgu, dico, item, negation, disjunction);
}

//---------------------------------------------------------------------
class kbpredict {
public:
    
    vector<TamguPredicate*> kbase;
    basebin_hash<Tamgu*> kept;
    basebin_hash<Tamgu*>::iterator it;
};


class predictenvironment {
public:
    
    VECTE<Tamgu*> localgoals;
    VECTE<Tamgu*> currentgoals;
    vector<Tamgu*> headpredicatevalues;
    PredicateInstanceDeclaration refdico;
    TamguPredicate head_predicate;
    TamguPredicate* current_head;
    
    vector<Tamgu*> rulegoals;
    basebin_hash<Tamgu*> basedomain;
    
    kbpredict* KnowledgeBaseFacts;
    
    predictenvironment(long sz, TamguPredicate* head, TamguDeclarationPredicate* dom) :
    refdico(head->parameters.size()),
    localgoals(sz),
    currentgoals(sz),
    basedomain(dom->declarations),
    head_predicate(head->name) {
        current_head = head;
        KnowledgeBaseFacts = NULL;
        head_predicate.reference = 1;
    }
    
    ~predictenvironment() {
        for (long i = 0; i < headpredicatevalues.size(); i++) {
            //if (headpredicatevalues[i] != current_head->parameters[i])
                headpredicatevalues[i]->Release();
        }
        if (KnowledgeBaseFacts != NULL)
            delete KnowledgeBaseFacts;
    }

    void unify(TamguPredicate* predicate_value, TamguDeclaration* dom, short idthread) {
        Tamgu* val;
        for (long i = 0; i< predicate_value->parameters.size(); i++) {
            val = predicate_value->parameters[i]->Value();
            if (val != aNOELEMENT) {
                headpredicatevalues[i]->Release();
                headpredicatevalues[i] = val;
            }
            
            headpredicatevalues[i]->Setreference();
        }
    }
    
    void protect() {
        for (long i = 0; i< headpredicatevalues.size(); i++) {
            headpredicatevalues[i]->Protect();
        }
    }
    
    void unifyheadpredicate(TamguDeclaration* dom, short idthread) {
        Tamgu* var;
        Tamgu* val;
        for (long i = 0; i< current_head->parameters.size(); i++) {
            var = current_head->parameters[i];
            val = var->Variable(dom);
            if (val != NULL) {
                val = val->VariableValue(dom, idthread);
                if (val != aNOELEMENT) {
                    var = val->Unifies(dom);
                }
            }
            current_head->Replaceparameter(i, var, idthread);
        }
    }

    void unifyheadpredicatevalue(TamguDeclaration* dom, short idthread) {
        Tamgu* var;
        Tamgu* val;
        for (long i = 0; i< current_head->parameters.size(); i++) {
            var = current_head->parameters[i];
            val = var->Variable(dom);
            if (val != NULL) {
                val = val->VariableValue(dom, idthread);
                if (val != aNOELEMENT) {
                    var = val->Unifies(dom);
                }
            }
            headpredicatevalues.push_back(var);
        }
    }
    
};

#define storing_fact_name 2

char TamguPredicateAsVariable::setPredicateNameVariable(TamguPredicate* p, short idthread) {
    Tamgu* p_name = globalTamgu->Providestring(p->Namestring());
    predicate_name->Setvalue(aNULL, p_name, idthread, false);
    p_name->Setreference(predicate_name->Reference());
    return storing_fact_name;
}

void TamguPredicateAsVariable::resetPredicateNameVariable(short idthread) {
    predicate_name->Setvalue(aNULL, aNOELEMENT, idthread, false);
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//This is the most important method to detect in the predicate stack, the ones that can be dismissed
//or those that would fail the whole branch
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

TamguPredicate* TamguInstructionEvaluate::PredicateUnification(VECTE<Tamgu*>& goals, long& posreplace, long& from) {
    TamguPredicate* headpredicate = NULL;
    Tamgu* e;
    char test;
    bool keep = false;
    

    posreplace = -1;
    from = 0;

    for (long i = 0; i < goals.last; i++) {
        e = goals[i];
        switch (e->checkTypePredicate()) {
            case a_true:
                continue;
            case a_fail:
            case a_stop:
                return headpredicate == NULL?(TamguPredicate*)e:headpredicate;
            case a_cut:
                if (headpredicate == NULL && e->Checkparameters(dom)) {
                    headpredicate = (TamguPredicate*)e;
                    posreplace = i;
                    keep = true;
                }
                continue;
            case a_instructions:
                if (headpredicate == NULL) {
                    /*
                     T -> TRUE
                     F -> FALSE
                     N -> NOELEMENT (no unified variable)
                     
                     neg  T T T F F F
                     res  T F N T F N
                     test F T T T F F
                     SUCC F T T T F T
                     */
                    bool neg = e->isNegation();
                    Tamgu* res = e->Eval(this, dom, threadowner);
                    test = res->Boolean() ^ neg;
                    while (e->checkDisjunction()) {
                        e = goals[++i];
                        if (!test) {
                            res = e->Eval(this, dom, threadowner);
                            neg = e->isNegation();
                            test = res->Boolean() ^ neg;
                        }
                    }
                    if (!test) {
                        if (res != aNOELEMENT)
                            return aFAIL;
                        keep = true;
                    }
                    else {
                        if (!keep)
                            from = i + 1;
                    }
                }
                else
                    e->setSucessfull(false);
                continue;
            case a_asserta:
            case a_assertz:
            case a_dependency_asserta:
            case a_dependency_assertz:
                if (e->isUnified(dom)) {
                    if (!keep) {
                        headpredicate = (TamguPredicate*)e;
                        from = i + 1;
                    }
                    return headpredicate;
                }
                if (headpredicate == NULL && e->Checkparameters(dom))
                    return aFAIL;
                continue;
            case a_retract:
            case a_dependency_retract:
            case a_dependency_remove:
                if (e->isUnified(dom)) {
                    if (!keep) {
                        headpredicate = (TamguPredicate*)e;
                        from = i + 1;
                    }
                    return headpredicate;
                }
                if (headpredicate == NULL && e->Checkparameters(dom)) {
                    headpredicate = (TamguPredicate*)e;
                    posreplace = i;
                    keep = true;
                }
                continue;
            case a_findall:
                if (!keep) {
                    headpredicate = (TamguPredicate*)e;
                    from = i + 1;
                }
                return headpredicate;
            case a_predicatemethod:
                test = false;
                break;
            case a_predicate:
                test = e->isUnified(dom);
                if (test) {
                    //We then evaluate it... This function returns 3 values:
                    // 0: fail
                    // 1: does not match any fact in the knowledgebase fact
                    // 2: found a matching fact in the knowledge base
                    test = globalTamgu->isaValidPredicate(dom, ((TamguPredicate*)e), rules) + (3 * e->isNegation());
                    switch (test) {
                        case 0:
                        case 5: //eval == 2 + negation*3
                            return aFAIL;
                        case 1:
                            test = false;
                            break;
                        case 2:
                            test = !((TamguPredicate*)e)->Idvar();
                            break;
                        case 4: //eval == 1 + negation*3
                            if (!keep) {
                                headpredicate = (TamguPredicate*)e;
                                keep = true;
                                from = i + 1;
                            }
                            continue;
                    }
                }
                break;
            default:
                return aFAIL;
        }

        if (!test) { //still some variables to go
            if (headpredicate == NULL && e->Checkparameters(dom)) {
                headpredicate = (TamguPredicate*)e;
                posreplace = i;
                keep = true;
            }
        }
        else {
            //If we have not found anything yet as a head, we will start the evaluation from the following element...
            if (!keep) {
                if (e->isPredicateNameVariable()) {
                    headpredicate = (TamguPredicate*)e;
                    posreplace = i;
                    keep = true;
                }
                else
                    from = i + 1;
            }
        }
    }
    return headpredicate;
}

///--------------------------------------------------------------------------------------------------------
Tamgu* TamguInstructionEvaluate::Predpredicatemethod(VECTE<Tamgu*>& stack, TamguPredicate* headpredicate,TamguPredicate* currenthead,long depth) {
    //--------------------------------------------------------------------------------
    if (headpredicate->negation) {
        headpredicate->Setnegation(false);
        TamguPredicate* prev = headrule;
        headrule = NULL;
        //---------------------------------------------------------------
        VECTE<Tamgu*>* localstack = new VECTE<Tamgu*>(3);
        localstack->push_back(aCUT);
        Tamgu* result = headpredicate->PredicateEvalue(this, *localstack, currenthead, depth);
        delete localstack;
        //---------------------------------------------------------------
        headpredicate->Setnegation(true);
        headrule = prev;
        //---------------------------------------------------------------
        //It was TRUE then it is FALSE or conversely
        if (result == aFALSE)
            return PredicateEvalue(stack, currenthead, depth);
        return aFALSE;
    }
    
    return headpredicate->PredicateEvalue(this, stack, currenthead, depth);
}

Tamgu* TamguInstructionEvaluate::Predassertaz(TamguPredicate* headpredicate, bool order) {
    TamguPredicate* predicate_value = new TamguPredicate(globalTamgu, headpredicate->name);
    Tamgu* result;
    
    for (long i = 0; i < headpredicate->parameters.size(); i++) {
        result = headpredicate->parameters[i]->Eval(this, dom, threadowner);
        predicate_value->parameters.push_back(result->Atom(true));
    }
    
    globalTamgu->StorePredicate(dom, predicate_value, order);
    return aTRUE;
}

Tamgu* TamguInstructionEvaluate::Predretract(TamguPredicate* headpredicate) {
    TamguPredicate* predicate_value = new TamguPredicate(globalTamgu, headpredicate->name);
    Tamgu* result;
    
    for (long i = 0; i < headpredicate->parameters.size(); i++) {
        result = headpredicate->parameters[i]->Eval(this, dom, threadowner);
        predicate_value->parameters.push_back(result->Atom(true));
    }
    
    bool order = globalTamgu->RemovePredicates(dom, predicate_value);
    predicate_value->Release();
    return booleantamgu[order];
}

Tamgu* TamguInstructionEvaluate::Predfindall(TamguPredicate* headpredicate) {
    std::vector<TamguPredicate*> facts;
    Tamguvector* v;
    Tamgu* result;
    TamguPredicate* predicate_value = new TamguPredicate(globalTamgu, headpredicate->name);
    long i;
    if (((TamguPredicateFindall*)headpredicate)->equal) {
        for (i = 0; i < headpredicate->parameters.size() - 1; i++) {
            result = headpredicate->parameters[i]->Eval(this, dom, threadowner);
            if (result == aNOELEMENT)
                predicate_value->parameters.push_back(aUNIVERSAL);
            else
                predicate_value->parameters.push_back(result);
        }
        // This section is used to test our current predicate against the knowledgebase...
        if (!globalTamgu->GetPredicates(dom, predicate_value, facts, false)) {
            predicate_value->Release();
            return aFALSE;
        }
        v = globalTamgu->Providevector();
        if (((TamguPredicateFindall*)headpredicate)->equal) {
            for (short f = 0; f < facts.size(); f++) {
                v->Push(facts[f]);
            }
        }
    }
    else {
        std::vector<short> free_variables;
        for (i = 0; i < headpredicate->parameters.size() - 2; i++) {
            result = headpredicate->parameters[i]->Eval(this, dom, threadowner);
            if (result == aNOELEMENT) {
                predicate_value->parameters.push_back(aUNIVERSAL);
                free_variables.push_back(i);
            }
            else
                predicate_value->parameters.push_back(result);
        }
        
        // This section is used to test our current predicate against the knowledgebase...
        if (!globalTamgu->GetPredicates(dom, predicate_value, facts, false)) {
            predicate_value->Release();
            return aFALSE;
        }
        
        //These positions correspond to free variables in the parameter lists of each fact
        v = globalTamgu->Providevector();
        Tamgu* val;
        for (short f = 0; f < facts.size(); f++) {
            //First we unify our variables with their values from the fact
            for (i = 0; i < free_variables.size(); i++) {
                val = facts[f]->parameters[free_variables[i]];
                headpredicate->parameters[free_variables[i]]->Put(dom, val, threadowner);
            }
            //Our pattern is on -2 position
            val = headpredicate->parameters[headpredicate->parameters.size() - 2]->Getvalues(dom, true);
            if (val == aNOELEMENT) {
                v->Release();
                return aFALSE;
            }
            v->Push(val);
        }
        //We clear these values
        for (i = 0; i < free_variables.size(); i++) {
            headpredicate->parameters[free_variables[i]]->Put(dom, aNOELEMENT, threadowner);
        }
    }
    
    result = headpredicate->parameters[headpredicate->parameters.size() - 1];
    result->Put(dom, v, threadowner);
    predicate_value->Release();
    return aTRUE;
}

Tamgu* TamguInstructionEvaluate::Preddependency_assertaz(TamguPredicate* headpredicate, bool order) {
    Tamgu* a = dom->Declaration(predicatefeature);
    TamguPredicate* predicate_value;
    if (a != NULL) {//we clear it...
        dom->declarations.erase(predicatefeature);
        predicate_value = new TamguDependency(globalTamgu,
                                                ((TamguPredicate*)a)->Features(),
                                                headpredicate->name,
                                                headpredicate->Idvar());
        
        predicate_value->Setfeatures(headpredicate->Features());
    }
    else {
        predicate_value = new TamguDependency(globalTamgu,
                                                headpredicate->Features(),
                                                headpredicate->name,
                                                headpredicate->Idvar());
    }
    predicate_value->Setidrule(headpredicate->Idrule());
    
    Tamgu* result;
    for (long i = 0; i < headpredicate->parameters.size(); i++) {
        result = headpredicate->parameters[i]->Eval(this, dom, threadowner);
        predicate_value->parameters.push_back(result->Atom(true));
    }
    
    globalTamgu->StorePredicate(dom, predicate_value, order);
    
    if (a != NULL) {
        a->Resetreference();
        //we then reinject our novel dependency into our dom
        //it might be usefull for other loops...
        dom->Declare(predicatedependency, predicate_value);
    }
    
    return aTRUE;
}

Tamgu* TamguInstructionEvaluate::Preddependency_retract(TamguPredicate* headpredicate) {
    Tamgu* a = dom->Declaration(predicatefeature);
    TamguPredicate* predicate_value;
    if (a != NULL) {//we clear it...
        dom->declarations.erase(predicatefeature);
        predicate_value = new TamguDependency(globalTamgu,
                                                ((TamguPredicate*)a)->Features(),
                                                headpredicate->name,
                                                headpredicate->Idvar());
        
        predicate_value->Setfeatures(headpredicate->Features());
    }
    else {
        predicate_value = new TamguDependency(globalTamgu,
                                                headpredicate->Features(),
                                                headpredicate->name,
                                                headpredicate->Idvar());
    }
    predicate_value->Setidrule(headpredicate->Idrule());
    
    Tamgu* result;
    for (long i = 0; i < headpredicate->parameters.size(); i++) {
        result = headpredicate->parameters[i]->Eval(this, dom, threadowner);
        predicate_value->parameters.push_back(result->Atom(true));
    }
    
    bool order = globalTamgu->RemovePredicates(dom, predicate_value);
    predicate_value->Release();
    if (!order)
        return aFALSE;
    
    if (a != NULL) {
        a->Resetreference();
        //we then reinject our novel dependency into our dom
        //it might be usefull for other loops...
        dom->Declare(predicatedependency, predicate_value);
    }
    return aTRUE;
}

Tamgu* TamguInstructionEvaluate::Preddependency_remove(TamguPredicate* headpredicate) {
    Tamgu* a = dom->Declaration(predicatedependency);
    if (a == NULL || !a->isPredicate())
        return aFALSE;
    
    if (headpredicate->Idvar()) {
        //If it is a dependency modification
        //we remove it from the database...
        dom->Declare(predicatefeature, a);
        a->Setreference(); //we do not want this object to be fully deleted here
    }
    
    return booleantamgu[globalTamgu->RemoveThePredicate(dom, (TamguPredicate*)a)];
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//This is the main loop to handle fact checking
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

Tamgu* TamguInstructionEvaluate::Checkfacts(predictenvironment* Oo,long posreplace,TamguPredicate* currenthead,long depth) {
    Oo->KnowledgeBaseFacts = new kbpredict();
    TamguPredicate* predicate_value;
    Tamgu* result;
    long sz, i, j;
    char check_predicate_name;
    bool order = false;
    
    // This section is used to test our current predicate against the knowledgebase...
    if (globalTamgu->GetPredicates(dom, Oo->current_head, Oo->KnowledgeBaseFacts->kbase, false)) {
        //we set the res value to aTRUE, which will be considered as an error
        if (Oo->current_head->isNegation())
            result = aTRUE;
        else {
            sz = Oo->currentgoals.size();
            if (sz) {
                Oo->localgoals.reserve(sz);
                Oo->localgoals.last = sz - 1;
            }
            i = 0;
            for (j = 0; j < sz; j++) {
                if (j != posreplace)
                    Oo->localgoals.vecteur[i++] = Oo->currentgoals.vecteur[j];
            }
            
            sz = Oo->KnowledgeBaseFacts->kbase.size();
                        
            //we remove our element at the position posreplace in goal stack
            for (i = 0; i < sz; i++) {
                predicate_value = Oo->KnowledgeBaseFacts->kbase[i];
                dico = &Oo->refdico;
                check_predicate_name = Oo->current_head->setPredicateNameVariable(predicate_value, threadowner);
                
                
                predicate_value->Setchosen(true);
                
                //we then unify our values in dom with respect to the element in the goal stack (stored in headpredicate) and the extracted elements
                for (j = 0; j < Oo->current_head->parameters.size(); j++)
                    Oo->current_head->parameters[j]->Insertvalue(dom, predicate_value->parameters[j], Oo->KnowledgeBaseFacts->kept);
                
                if (trace)
                    Displaypredicatestack(d_kb, dom, Oo->current_head, Oo->localgoals, depth, this);
                
                //---------------------------------------------------------------
                //In the case of dependency goals, we might need to store in a variable the current goal...
                Oo->current_head->Setinvariable(predicate_value, dom, threadowner);
                //If it is a potential dependency modification, then we protect it ahead of processing...
                //Otherwise, the dependency could linger in kbase, being deleted...
                //The explanation is that, we could have many occurrences of the same dependency name in the rule.
                //If we store the current one as being a potential modification, and we go into recursion, then this current dependency might be
                //deleted, however, since there are more than one loop in the knwoeledge base with the same query on the these dependencies,
                //the current dependency might find itself in this second survey...
                predicate_value->Setreference();
                //---------------------------------------------------------------
                //We then continue on the rest of the goals...
                Tamgu* localresult = PredicateEvalue(Oo->localgoals, currenthead, depth);
                //---------------------------------------------------------------
                //we then remove it, if it was inserted
                Oo->current_head->Resetintvariable(dom, threadowner);
                if (check_predicate_name == storing_fact_name)
                    Oo->current_head->resetPredicateNameVariable(threadowner);
                
                predicate_value->Setchosen(false);
                
                //we can safely return back to the actual reference of that dependency...
                predicate_value->Resetreference();
                //---------------------------------------------------------------
                
                short kbaction = localresult->checkTypePredicate();
                basebin_hash<Tamgu*>::iterator& it = Oo->KnowledgeBaseFacts->it;
                switch (kbaction) {
                    case a_cutfalse:
                        if (order) {
                            for (it = Oo->KnowledgeBaseFacts->kept.begin(); it.table != NULL; it++)
                                dom->declarations[it->first]->Cleans(it->second, false);
                            return aFALSE;
                        }
                        break;
                    case a_cut:
                        if (order) {
                            for (it = Oo->KnowledgeBaseFacts->kept.begin(); it.table != NULL; it++)
                                dom->declarations[it->first]->Cleans(it->second, false);
                            return aTRUE;
                        }
                        break;
                    case a_true:
                        if (order) {
                            for (it = Oo->KnowledgeBaseFacts->kept.begin(); it.table != NULL; it++)
                                dom->declarations[it->first]->Cleans(it->second, false);
                            return aTRUE;
                        }
                        result = aTRUE;
                        break;
                }
                
                //We clean them, then...
                for (it = Oo->KnowledgeBaseFacts->kept.begin(); it.table != NULL; it++)
                    dom->declarations[it->first]->Cleans(it->second, true);
                
                Oo->KnowledgeBaseFacts->kept.clear();
                
                switch (kbaction) {
                    case a_terminal:
                        return aTERMINAL;
                    case a_cutfalse:
                        return aCUTFALSE;
                    case a_cut:
                        return dependency_mode?aTRUE:aCUT;
                    case a_break:
                        return aBREAK;
                    default:
                        if (globalTamgu->Error(threadowner))
                            return aFALSE;
                }
            }
        }
    }
    return aNULL;
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//This is the main loop for predicates and all
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
Tamgu* TamguInstructionEvaluate::PredicateEvalue(VECTE<Tamgu*>& goals, TamguPredicate* currenthead, long depth) {
    if (fulltraversal == STOPSEARCH || globalTamgu->Error(threadowner))
        return fulltraversal == STOPSEARCH?aCUT:aFALSE;
    
    long sz = goals.size();
    if (!globalTamgu->check_stack_base_address(threadowner, reinterpret_cast<uintptr_t>(&sz)))
        return globalTamgu->Returnstackoverflow(threadowner);
    
    long i;
    //--------------------------------------------------------------------------------
    // Disjunction
    //If the top element in the goal stack is a disjunction, we then evaluate the next one...
    //The current one will be evaluated after... The disjunction is evaluated backward...
    if (sz && goals[0]->isDisjunction()) {
        VECTE<Tamgu*>* localgoals = new VECTE<Tamgu*>(sz + 1);
        for (i = 1; i < sz; i++)
            localgoals->vecteur[i - 1] = goals.vecteur[i];
        localgoals->last = sz - 1;
        //---------------------------------------------------------------
        Tamgu* result = PredicateEvalue(*localgoals, currenthead, depth);
        //---------------------------------------------------------------
        delete localgoals;
        switch (result->checkTypePredicate()) {
            case a_true:
            case a_break:
                return result;
            case a_cutfalse:
                return aFALSE;
            case a_cut:
                return aTRUE;
        }
    }
    
    Tamgu* result = aNULL;
    long posreplace, from;
    //--------------------------------------------------------------------------------
    //The evaluation of the goals, in which we detect which are the goals that full unified...
    // It returns NULL if everything has been unified, or the next predicate to be unified...
    TamguPredicate* headpredicate = PredicateUnification(goals, posreplace, from);
    //--------------------------------------------------------------------------------
    //It is already all unified... The goal list is now empty, we can create our resulting predicates.
    if (headpredicate == NULL) {
        if (trace)
            PreLocalPrint(Endl);
        for (i = from; i < sz; i++) {
            if (!goals[i]->hasbeenSuccesfull())
                return aFALSE;
        }
        result = PredicateCreate(headrule, depth);
        if (result == aTRUE && fulltraversal == SEARCHONE) {
            fulltraversal = STOPSEARCH;
            return aCUT;
        }
        
        return result;
    }
    
    //--------------------------------------------------------------------------------
    bool order = true;

    switch (headpredicate->checkTypePredicate()) {
        case a_stop: {
            //--------------------------------------------------------------------------------
            //We want to stop any further analysis...
            fulltraversal = STOPSEARCH;
            return aCUT;
        }
        case a_fail: {
            //--------------------------------------------------------------------------------
            //If the unification went wrong...
            if (trace) {
                PreLocalPrint(" --> Fail\n");
            }
            return aFALSE;
        }
        case a_cut: {
            //--------------------------------------------------------------------------------
            //if we have a CUT...
            VECTE<Tamgu*>* localgoals = new VECTE<Tamgu*>(sz);
            for (i = from; i < sz; i++) {
                if (i != posreplace && !goals[i]->hasbeenSuccesfull())
                    localgoals->push_back(goals.vecteur[i]);
            }
            //---------------------------------------------------------------
            switch (PredicateEvalue(*localgoals, currenthead, depth)->checkTypePredicate()) {
                case a_false:
                case a_fail:
                    delete localgoals;
                    return aCUTFALSE;
                case a_terminal:
                    delete localgoals;
                    return aTERMINAL;
                default:
                    delete localgoals;
                    return aCUT;
            }
        }
        case a_predicatemethod: {
            VECTE<Tamgu*>* localgoals = new VECTE<Tamgu*>(sz);
            for (i = from; i < sz; i++) {
                if (i != posreplace && !goals[i]->hasbeenSuccesfull())
                    localgoals->push_back(goals.vecteur[i]);
            }
            result = Predpredicatemethod(*localgoals, headpredicate, currenthead, depth);
            delete localgoals;
            return result;
        }
        case a_asserta:
            result = Predassertaz(headpredicate, false);
            break;
        case a_assertz: {
            result = Predassertaz(headpredicate, true);
            break;
        }
        case a_retract: {
            result = Predretract(headpredicate);
            break;
        }
        case a_findall: {
            result = Predfindall(headpredicate);
            break;
        }
        case a_dependency_asserta:
            result = Preddependency_assertaz(headpredicate, false);
            break;
        case a_dependency_assertz: {
            result = Preddependency_assertaz(headpredicate, true);
            break;
        }
        case a_dependency_retract: {
            result = Preddependency_retract(headpredicate);
            break;
        }
        case a_dependency_remove: {
            result = Preddependency_remove(headpredicate);
            break;
        }
        default:
            order = false;
    }
    
    if (order) {
        if (result == aTRUE) {
            VECTE<Tamgu*>* localgoals = new VECTE<Tamgu*>(sz);
            for (i = from; i < sz; i++) {
                if (i != posreplace && !goals[i]->hasbeenSuccesfull())
                    localgoals->push_back(goals.vecteur[i]);
            }

            //---------------------------------------------------------------
            result = PredicateEvalue(*localgoals, currenthead, depth);
            delete localgoals;
            return result;
            //---------------------------------------------------------------
        }
        return aFALSE;
    }
    
    if (trace)
        PreLocalPrint(Endl);

    //--------------------------------------------------------------------------------
    if (headpredicate->isTerminal())
        return aTERMINAL;
    //--------------------------------------------------------------------------------
    // We use this specific structure to handle as many variables out of the execution stack.
    std::unique_ptr<predictenvironment> Oo(new predictenvironment(hmax(sz + 1, 10), headpredicate, dom));
    //Some variables still need to be evaluated
    
    //We will try to solve headpredicate...
    //first it could be part of the knowledgebase
    
    //We only keep the elements that are still to be evaluated...
    for (i = from; i < sz; i++) {
        if (!goals[i]->hasbeenSuccesfull()) {
            if (!order) {
                if (i == posreplace)
                    posreplace = Oo->currentgoals.size();
                Oo->currentgoals.push_back(goals[i]);
            }
            order = goals[i]->isDisjunction();
        }
    }
            
    //--------------------------------------------------------------------------------
    if (globalTamgu->Checkpredicate(headpredicate->name)) {
        result = Checkfacts(Oo.get(), posreplace, currenthead, depth);
        if (result != aNULL)
            return result;
    }
    
    //--------------------------------------------------------------------------------
    //Else there is no rules matching this predicate...
    //--------------------------------------------------------------------------------
    if (rules.find(headpredicate->name) == rules.end() || !rules[headpredicate->name].size()) {
        if (headpredicate->negation)
            return PredicateEvalue(Oo->currentgoals, currenthead, depth);
        else
            return aFALSE;
    }
    
    //--------------------------------------------------------------------------------
    //We have a negation...
    //--------------------------------------------------------------------------------
    if (headpredicate->negation) {
        
        //We need to build a local call, which should return FALSE... We need only one TRUE to detect if the negation should fail
        Oo->localgoals.clear();
        
        Oo->localgoals.push_back(headpredicate);
        //We do not need to explore the whole world, one TRUE is enough to decide
        Oo->localgoals.push_back(aCUT);
        
        headpredicate->Setnegation(false);
        TamguPredicate* predicate_value = headrule;
        //We do not need to build anything, the headrule is set to NULL to tell the system
        //That we are only interested in a local evaluation...
        headrule = NULL;
        //---------------------------------------------------------------
        if (!PredicateEvalue(Oo->localgoals, currenthead, depth)->Boolean()) {
            headrule = predicate_value;
            headpredicate->Setnegation(true);
            return PredicateEvalue(Oo->currentgoals, currenthead, depth);
        }
        
        //---------------------------------------------------------------
        //It was TRUE then it is FALSE...
        headrule = predicate_value;
        headpredicate->Setnegation(true);
        return aFALSE;
    }
    
    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------
    //This is the section where we evaluate one rule after the other
    //--------------------------------------------------------------------------------
    //we extract the rules matching our current target predicate...
    sz = rules[headpredicate->name].size();
    
    //--------------------------------------------------------------------------------
    //Now in the following section we evaluate which rules to enrich our goals with...
    //we sort the values according to some random algorithm
    
    //We get rid of the predicate at posreplace, since we are going to evaluate it...
    Oo->currentgoals.eraseraw(posreplace);

    TamguPredicateRule* predicate_rule;
    result = aFALSE;
    Tamgu* localresult;
    long j;
    
    Oo->unifyheadpredicatevalue(dom, threadowner);
    
    for (i = 0; i < sz; i++) {
        predicate_rule = rules[headpredicate->name][i];
        if (predicate_rule->head->parameters.size() != headpredicate->parameters.size())
            continue;
        
        //Variable management
        dico = &Oo->refdico;
        
        if (trace) {
            Displaypredicatestack(d_rule, dom, predicate_rule->head, Oo->localgoals, depth, this);
            PreLocalPrint(Endl);
        }

        //The first step consists of unifying when possible the different arguments of the head rule
        //We build a head_predicate, which is a new predicate sharing the same name as the predicate rule head
        //with its arguments unified when possible.
        //Note that if the next call to PredicateEvalue is the last one, then this element will be used as its
        //final result.
        if (!predicate_rule->head->Copyfrom(&Oo->head_predicate, this, dom, Oo->headpredicatevalues, threadowner)) {
            dom->declarations.clearcommon(Oo->basedomain);
            dom->declarations = Oo->basedomain;
            
            if (trace) {
                PreLocalPrint(" --> Fail\n");
            }
            Oo->refdico.clear();
            
            if (globalTamgu->Error(threadowner))
                return aFALSE;
            continue;
        }
        
        //We apply the rule with currentgoals...
        Oo->localgoals = Oo->currentgoals;
        
        
        localresult = aTRUE;
        order = false;
        from = predicate_rule->instructions.size();
        if (from) {
            for (j = 0; j < from; j++) {
                localresult = ((TamguPredicateRuleItem*)predicate_rule->instructions[j])->Unify(this, dom, dico, threadowner);
                if (localresult ==  NULL)
                    break;
                if (localresult == aTRUE)
                    continue;
                if (localresult == aCUT)
                    order = true;
                Oo->rulegoals.push_back(localresult);
            }
        }
        
        //We use a_predicate to handle tail recursion
        bool terminal_recursion = false;
        if (localresult != NULL) {
            from = Oo->rulegoals.size();
            if (from) {
                if (from == 1)
                    Oo->localgoals.insert(posreplace, Oo->rulegoals[0]);
                else {
                    Oo->localgoals.move(posreplace, from);
                    for (j = from - 1; j >= 0; j--)
                        Oo->localgoals.vecteur[posreplace + j] = Oo->rulegoals[j];
                }
                terminal_recursion = Oo->rulegoals.back()->checkTerminal(headpredicate->name, headpredicate->parameters.size());
            }
            if (trace)
                Displaypredicatestack(d_eval, dom, &Oo->head_predicate, Oo->localgoals, depth, this);
            
            //---------------------------------------------------------------
            //THEN We use these new goals and apply PredicateEvaluate to this new list...
            localresult = PredicateEvalue(Oo->localgoals, &Oo->head_predicate, depth + 1);
            //---------------------------------------------------------------
        }
        
        from = Oo->rulegoals.size();
        if (localresult == aTERMINAL) {
            if (terminal_recursion) {
                Oo->unify((TamguPredicate*)Oo->rulegoals.back(), dom, threadowner);
                i = -1;
                localresult = aFALSE;
            }
            else
              Oo->unifyheadpredicate(dom, threadowner);
        }
        
        from = Oo->rulegoals.size();
        for (j = 0; j < from; j++) {
            if (Oo->rulegoals[j]->isConst())
                continue;
            Oo->rulegoals[j]->Clear();
            delete Oo->rulegoals[j];
        }
        
        
        //We clean the parameters
        dom->declarations.clearcommon(Oo->basedomain);
        Oo->head_predicate.Clear();
                
        Oo->rulegoals.clear();
        
        Oo->refdico.clear();
        
        ///-----------------------OLD PREVIOUS LINES-------------------------------
        
        switch (localresult->checkTypePredicate()) {
            case a_true:
                if (order)
                    return aTRUE;
                result = localresult;
                break;
            case a_break:
                return aBREAK;
            case a_cutfalse:
                return aFALSE;
            case a_cut:
                return (!order && !dependency_mode)?aCUT:aTRUE;
            case a_terminal:
                return aTERMINAL;
            default: {
                if (i == -1)
                    Oo->protect();
                if (globalTamgu->Error(threadowner)) {
                    return aFALSE;
                }
            }
        }
    }
    
    return result;
}


//---------------------------------------------------------------------------------------------------------------------

TamguPredicateRuleItem::TamguPredicateRuleItem(TamguGlobal* g, Tamgu* parent, bool n, bool d, long i) : TamguTracked(a_predicateruleelement, g) {
    negation = n;
    disjunction = d;
    position = i;
    item = parent;
}

Tamgu* Tamgusynode::Eval(Tamgu* contextualpattern, Tamgu* dom, short idthread) {
    if (contextualpattern->Type() == a_predicateevaluate) {
        if (!contextualpattern->Checkdico(name)) {
            dom->Setfail(true);
            return aNOELEMENT;
        }
        
        if (affectation)
            return contextualpattern->Getdico(name);
        
        dom = globalTamgu->Declarator(predicatedico, idthread);
        
        return contextualpattern->Getdico(name)->Eval(contextualpattern, dom, idthread);
    }
    
    if (dom->isIndex()) {
        if (features == aNULL)
            return aNOELEMENT;
        
        return features->Eval(contextualpattern, dom, idthread);
    }
    
    return this;
}

Tamgu* TamguBasePredicateVariable::Eval(Tamgu* contextualpattern, Tamgu* dom, short idthread) {
    if (!dom->isDeclared(predicatedico)) {
        dom = globalTamgu->Declarator(predicatedico, idthread);
        if (dom == aNULL)
            return aNOELEMENT;
    }
    
    contextualpattern = dom->Declaration(predicatedico);
    
    if (contextualpattern == NULL || !contextualpattern->Checkdico(name)) {
        dom->Setfail(true);
        return aNOELEMENT;
    }
    
    return contextualpattern->Getdico(name)->Eval(contextualpattern, dom, idthread);
}

Tamgu* TamguPredicateVariableASSIGNMENT::Eval(Tamgu* contextualpattern, Tamgu* dom, short idthread) {
    Tamgu* var = contextualpattern->Getdico(name);
    if (var == NULL) {
        var = globalTamgu->Providevariableinstance(name, idthread);
        dom->Setdomainlock();
        contextualpattern->Setdico(name, (TamguPredicateVariableInstance*)var);
        var->Setreference();
        ((TamguDeclaration*)dom)->declarations[var->Name()] = var;
        dom->Resetdomainlock();
    }
    
    
    TamguRawSelf vself;
    Tamgu* value = instructions.vecteur[1]->Eval(&vself, aASSIGNMENT, idthread);
    if (vself.value != aNOELEMENT) {
        value = vself.value;
        value->Protect();
        vself.value = aNOELEMENT;
    }
    if (value->isError())
        return value;
    
    if (value == aNOELEMENT) {
        var->Putvalue(aNOELEMENT, idthread);
        return value;
    }
    
    //Then we might have an issue, if value is a sub element of variable...
    //for instance: v=v[0];
    //we need to prevent a Clear() on "variable" to delete this element...
    value->Setprotect(true);
    
    var = var->Put(dom, value, idthread);
    
    value->Resetreferencenoprotect(0);
    
    if (var->isError())
        return var;
    
    if (globalTamgu->isthreading)
        globalTamgu->Triggeronfalse(var);
    
    return aTRUE;
}

Tamgu* TamguPredicateVariableASSIGNMENTCall::Eval(Tamgu* contextualpattern, Tamgu* dom, short idthread) {
    Tamgu* var = contextualpattern->Getdico(name);
    if (var == NULL) {
        var = globalTamgu->Providevariableinstance(name, idthread);
        dom->Setdomainlock();
        contextualpattern->Setdico(name, (TamguPredicateVariableInstance*)var);
        var->Setreference();
        ((TamguDeclaration*)dom)->declarations[var->Name()] = var;
        dom->Resetdomainlock();
    }
    
    Tamgu* value = instructions.vecteur[1]->Eval(contextualpattern, aASSIGNMENT, idthread);
    
    if (value->isError())
        return value;
    
    if (value == aNOELEMENT) {
        var->Putvalue(aNOELEMENT, idthread);
        return value;
    }
    
    //Then we might have an issue, if value is a sub element of variable...
    //for instance: v=v[0];
    //we need to prevent a Clear() on "variable" to delete this element...
    value->Setprotect(true);
    
    var = var->Put(dom, value, idthread);
    
    value->Resetreferencenoprotect(0);
    
    if (var->isError())
        return var;
    
    if (globalTamgu->isthreading)
        globalTamgu->Triggeronfalse(var);
    
    return aTRUE;
}

Tamgu* TamguPredicateVariable::isBound(Tamgu* dom, short idthread) {
    if (!dom->isDeclared(predicatedico)) {
        dom = globalTamgu->Declarator(predicatedico, idthread);
        if (dom == aNULL)
            return aNOELEMENT;
    }
    dom = dom->Declaration(predicatedico);
    dom = dom->Getdico(name);
    return (dom == NULL || dom->Value() == aNOELEMENT)?aFALSE:aTRUE;
}

Tamgu* TamguPredicateVariable::Eval(Tamgu* contextualpattern, Tamgu* dom, short idthread) {
    if (!dom->isDeclared(predicatedico)) {
        dom = globalTamgu->Declarator(predicatedico, idthread);
        if (dom == aNULL)
            return aNOELEMENT;
    }
    
    contextualpattern = dom->Declaration(predicatedico);
    
    Tamgu* val = contextualpattern->Getdico(name);
    if (val == NULL)
        return aNOELEMENT;
    
    val = val->Eval(contextualpattern, dom, idthread);
    
    if (function != NULL)
        return function->Eval(contextualpattern, val, idthread);
    
    return val;
}

Tamgu* TamguPredicateVariable::Execute(Tamgu* contextualpattern, Tamgu* dom, short idthread) {
    TamguPredicateVariableInstance* kpvi = contextualpattern->Getdico(name);
    if (kpvi == NULL) {
        kpvi = globalTamgu->Providevariableinstance(name, idthread);
        if (!dom->isDeclared(predicatedico))
            dom = globalTamgu->Declarator(predicatedico, idthread);
        dom->Setdomainlock();
        contextualpattern->Setdico(name, kpvi);
        kpvi->Setreference();
        ((TamguDeclaration*)dom)->declarations[kpvi->Name()] = kpvi;
        dom->Resetdomainlock();
    }
    return kpvi;
}

Tamgu* TamguPredicateVariableInstance::Eval(Tamgu* contextualpattern, Tamgu* dom, short idthread) {
    Tamgu* v = Value((TamguDeclaration*)dom);
    if (v == aNOELEMENT) {
        dom->Setfail(true);
        return v;
    }
    
    if (v->isConcept())
        return v->Evalue((TamguDeclaration*)dom, idthread, false);
    return v->Getvalues((TamguDeclaration*)dom, false);
}

Tamgu* TamguPredicateTerm::Eval(Tamgu* contextualpattern, Tamgu* ke, short idthread) {
    if (!ke->isIndex() || ke->isNULL())
        return this;
    
    char ty = 1;
    long left = -1, right = -1;
    ty = ke->Indexes(idthread, parameters.size(), left, right);
    if (ty == 0) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror(e_wrong_index, idthread);
        return aNOELEMENT;
    }
    
    if (ty == 1) {
        if (parameters[left]->Type() == a_predicatevar)
            return globalTamgu->Getdefinition(parameters[left]->Name(), idthread);
        
        return parameters[left];
    }
    
    //In this case, we must create a new vector
    Tamguvector* kvect = globalTamgu->Providevector();
    for (long i = left; i < right; i++) {
        if (parameters[i]->Type() == a_predicatevar)
            kvect->Push(globalTamgu->Getdefinition(parameters[i]->Name(), idthread));
        else
            kvect->values.push_back(parameters[i]);
    }
    return kvect;
}

Tamgu* TamguPredicateConcept::Eval(Tamgu* contextualpattern, Tamgu* ke, short idthread) {
    if (!ke->isIndex() || ke->isNULL())
        return this;
    
    char ty = 1;
    long left = -1, right = -1;
    ty = ke->Indexes(idthread, parameters.size(), left, right);
    if (ty == 0) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror(e_wrong_index, idthread);
        return aNOELEMENT;
    }
    
    if (ty == 1) {
        if (parameters[left]->Type() == a_predicatevar)
            return globalTamgu->Getdefinition(parameters[left]->Name(), idthread);
        
        return parameters[left];
    }
    
    //In this case, we must create a new vector
    Tamguvector* kvect = globalTamgu->Providevector();
    for (long i = left; i < right; i++) {
        if (parameters[i]->Type() == a_predicatevar)
            kvect->Push(globalTamgu->Getdefinition(parameters[i]->Name(), idthread));
        else
            kvect->values.push_back(parameters[i]);
    }
    return kvect;
}

Tamgu* TamguPredicateKnowledgeBase::Eval(Tamgu* contextualpattern, Tamgu* dom, short idthread) {
    //we add it...
    if (add) {
        //In this case, we need to evaluate the parameters
        TamguPredicate* pv;
        
        if (ptype == a_dependencybase)
            pv = new TamguDependency(globalTamgu, ((TamguDependencyKnowledgeBase*)this)->features, name, 0);
        else
            pv = new TamguPredicate(globalTamgu, name);
        
        Tamgu* e;
        for (long i = 0; i < parameters.size(); i++) {
            e = parameters[i]->EvaluePredicateVariables(contextualpattern, (TamguDeclaration*)dom, idthread);
            pv->parameters.push_back(e);
            e->Setprotect(0);
        }
        
        if (!globalTamgu->StorePredicate((TamguDeclaration*)dom, pv, true))
            pv->Release();
        
        return aTRUE;
    }
    
    //or we remove it...
    TamguPredicate pv(globalTamgu, name);
    Tamgu* e;
    for (long i = 0; i < parameters.size(); i++) {
        e = parameters[i]->Eval(contextualpattern, dom, idthread);
        e->Setprotect(1);
        pv.parameters.push_back(e);
    }
    
    if (!globalTamgu->RemovePredicates((TamguDeclaration*)dom, &pv)) {
        pv.Clear();
        return aFALSE;
    }
    pv.Clear();
    return aTRUE;
}


Tamgu* TamguInstructionPredicate::Eval(Tamgu* contextualpattern, Tamgu* dom, short idthread) {
    if (!dom->isDeclaration())
        return globalTamgu->Returnerror("Error: Wrong predicate call", idthread);
    
    return instructions[0]->Eval(contextualpattern, dom, idthread);
}


//We test our value here...
Tamgu* TamguPredicate::Eval(Tamgu* contextualpattern, Tamgu* ke, short idthread) {
    //In this case, we have our variables in contextualpattern
    if (!ke->isIndex() || ke->isNULL())
        return this;
    
    char ty = 1;
    long left = -1, right = -1;
    
    ty = ke->Indexes(idthread, parameters.size(), left, right);
    if (ty == 0) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror(e_wrong_index, idthread);
        return aNOELEMENT;
    }
    
    if (ty == 1) {
        if (parameters[left]->Type() == a_predicatevar)
            return globalTamgu->Getdefinition(parameters[left]->Name(), idthread);
        return parameters[left];
    }
    
    //In this case, we must create a new vector
    Tamguvector* kvect = globalTamgu->Providevector();
    for (long i = left; i < right; i++) {
        if (parameters[i]->Type() == a_predicatevar)
            kvect->Push(globalTamgu->Getdefinition(parameters[i]->Name(), idthread));
        else
            kvect->values.push_back(parameters[i]);
    }
    return kvect;
}

Tamgu* TamguPredicateLocalInstruction::Eval(Tamgu* contextualpattern, Tamgu* domain, short idthread) {
    //we need to get the right evaluation for the possible variables hidden in the instruction
    //we have been through a variable renaming, which is stored in dico[localname]=declaration name
    
    TamguDeclaration* dom = (TamguDeclaration*)domain;
    
    success = false;
    dom->Setfail(false);
    
    dom->Declare(predicatedico, this);
    Tamgu* res = instruction->Eval(this, dom, idthread);
    dom->declarations.erase(predicatedico);
    
    if (res->isError())
        return aFALSE;
    
    if (res == aNOELEMENT) {
        //In that case, a variable could not be unified...
        dom->Setfail(false);
        return res;
    }
    
    if (dom->Failed())
        return aFALSE;
    
    success = res->Boolean();
    
    if (negation) {
        success = 1 - success;
        return booleantamgu[success];
    }
    
    return res;
}

string TamguPredicateLocalInstruction::String() {
    TamguDeclaration* dom = (TamguDeclaration*)globalTamgu->Declarator(predicatezone, globalTamgu->GetThreadid());
    if (dom == aNULL)
        return "";
    
    dom->Setdomainlock();
    dom->declarations[predicatedico] = this;
    string v = instruction->String();
    
    if (v.empty()) {
        Tamgu* ins = instruction;
        v = "[";
        short act = ins->Action();
        if (act == a_assignement) {
            v += ins->Instruction(0)->String();
            v += " is ";
            ins = ins->Instruction(1);
        }
        
        if (ins->Name()) {
            v += globalTamgu->Getsymbol(ins->Name());
            if (ins->isCall()) {
                v += "(";
                for (long i = 0; i < ins->Size(); i++) {
                    if (i)
                        v += ",";
                    if (globalTamgu->short_string) {
                        string vsub = ins->Argument(i)->String();
                        if (vsub.size() < globalTamgu->short_string)
                            v += vsub;
                        else
                            v += vsub.substr(0, globalTamgu->short_string) + "..";
                    }
                    else
                        v += ins->Argument(i)->String();
                }
                v += ")";
            }
        }
        else {
            if (act != a_none && act != a_assignement)
                v += globalTamgu->operator_strings[act];
            else
                v += "ins";
        }
        v += "]";
    }
    dom->declarations.erase(predicatedico);
    if (instruction->Function() != NULL)
        v += instruction->Function()->String();
    dom->Resetdomainlock();
    return v;
}

//-------------------------------------------------------------------------------------------------------
Tamgu* TamguPredicateRule::Eval(Tamgu*, Tamgu*, short) {
    return aTRUE;
}

void TamguPredicateRule::Reorderdisjunction(long disjposition) {
    vector<Tamgu*> ins;
    bool disjfound = false;
    long first = -1;
    for (long i = disjposition; i < instructions.size(); i++) {
        if (instructions[i]->isDisjunction()) {
            ins.push_back(instructions[i]);
            first = i;
            disjfound = true;
        }
        else {
            if (disjfound) {
                ins.push_back(instructions[i]);
                //we reverse it...in order to simplify the disjunction application
                //The last element will become the first and the first the last....
                for (long p = (long)ins.size() - 1; p >= 0; p--) {
                    ins[p]->Setdisjunction(true);
                    instructions[first++] = ins[p];
                }
                ins[0]->Setdisjunction(false); //of course the new last element should not be disjuncted..
                first = -1;
                ins.clear();
            }
            disjfound = false;
        }
    }
}

//This is the entry point of a predicate analysis...
Tamgu* TamguInstructionEvaluate::Eval(Tamgu* contextualpattern, Tamgu* domcall, short idthread) {
    //head is our definition
    //First we create an actionable TamguPredicate
    //we gather all the rules and elements, we might be interested in
    //First the elements from the knowledge base
    //Second the rules from rulebase
    
    //if we have a vector as a recipient, then we want to explore the whole tree, otherwise, one solution is enough
    bool cut = true;
    if (contextualpattern->isVectorContainer() || head->Function() != NULL)
        cut = false;
    
    //we use our current element as a way to transfer our stuff...
    TamguDeclarationPredicate* domain = new TamguDeclarationPredicate;
    
    domain->declarations = ((TamguDeclaration*)domcall)->declarations;
    VECTE<Tamgu*> goals;
    goals.push_back(head);
    headrule = head;
    
    domain->Declare(predicatezone, this);
    globalTamgu->Pushstack(domain, threadowner);
    
    if (contextualpattern->isContainer())
        fulltraversal = FULLSEARCH;
    
    dom = domain;
    PredicateEvalue(goals, NULL, 0);
    
    globalTamgu->Popstack(threadowner);
    domain->declarations.erase(predicatezone);
    
    basebin_hash<Tamgu*>::iterator it;
    
    for (it = domain->declarations.begin(); it != domain->declarations.end(); it++) {
        if (!domcall->isDeclared(it->first)) {
            it->second->Setprotect(0);
            it->second->Resetreference();
        }
    }
    
    delete domain;
    
    long i;
    
    if (contextualpattern->isVectorContainer()) {
        Tamgu* kvect = Selectavector(contextualpattern);
        for (i = 0; i < results.size(); i++) {
            kvect->Push(results[i]);
            results[i]->Resetreference();
        }
        return kvect;
    }
    
    if (results.size() == 0)
        return aFALSE;
    
    if (contextualpattern->isBoolean()) {
        for (i = 0; i < results.size(); i++)
            results[i]->Resetreference();
        return aTRUE;
    }
    
    if (contextualpattern->isString() && !contextualpattern->isConst()) {
        string v = results[0]->String();
        results[0]->Resetreference();
        return globalTamgu->Providewithstring(v);
    }
    
    if (contextualpattern->isNumber()) {
        for (i = 0; i < results.size(); i++)
            results[i]->Resetreference();
        return globalTamgu->ProvideConstint(results.size());
    }
    
    if (contextualpattern->isPredicate()) {
        results[0]->Setprotect(1);
        results[0]->Resetreference();
        return results[0];
    }
    
    for (i = 0; i < results.size(); i++)
        results[i]->Resetreference();
    
    return aTRUE;
}


class PredicateEnvironment {
public:
    TamguDeclarationPredicate domain;
    PredicateInstanceDeclaration localdico;
    TamguPredicateFunction pv;
    TamguInstructionEvaluate kl;
    short idthread;
    
    PredicateEnvironment(TamguPredicate* head, short idt, bool trace) :
    pv(globalTamgu, globalTamgu->predicates[head->name]->Function(), head->name),
    localdico(head->parameters.size()),
    kl(&pv, idt, trace) {
        idthread = idt;
        pv.reference = 1;
        if (pv.function != NULL)
            pv.object = globalTamgu->predicates[head->name]->Returnobject();
        kl.dico = &localdico;
        
        //Important variables...
        globalTamgu->SetPredicateVariableFlags(idthread);
        
        TamguPredicateContainer* kpcont = globalTamgu->GetPredicatecontainer(idthread);
        
        if (kpcont != NULL)
            kl.rules = kpcont->rules;

        Tamgu* e;
        for (short i = 0; i < head->parameters.size(); i++) {
            e = head->parameters[i]->ExtractPredicateVariables(&kl, &domain, NULL, NULL, idthread, true);
            e->Setreference();
            pv.parameters.push_back(e);
        }
    }
    
    ~PredicateEnvironment() {
        basebin_hash<Tamgu*>::iterator it;
        for (it = domain.declarations.begin(); it != domain.declarations.end(); it++) {
            it->second->Resetreferencenoprotect();
        }
        
        for (short i = 0; i < pv.Size(); i++)
            pv.parameters[i]->Resetreference();
    }
    
    
   inline  Tamgu* eval(Tamgu* context) {
        return kl.Eval(context, &domain, idthread);
    }
};

Tamgu* TamguInstructionLaunch::Eval(Tamgu* context, Tamgu* val, short idthread) {
    //head is our definition
    //First we create an actionable TamguPredicate
    bool trace = globalTamgu->predicateTrace(head->name);
    std::unique_ptr<PredicateEnvironment> pe(new PredicateEnvironment(head, idthread, trace));
    
    globalTamgu->short_string = 20;
    Tamgu* e = pe->eval(context);
    globalTamgu->short_string = 0;
        
    return e;
}
