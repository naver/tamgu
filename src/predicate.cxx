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

//A little bit of explanation.
//To avoid creating useless elements in the binmap, we initialize the actual name of a "^dependency" (code is a_modifydependency)
//with predicatedependency, which allows for a much more compact storage in the declaration map.
//We intercept this a_modifydependency to replace it on the fly with predicatedependency...
bool TamguDependencyKnowledgeBaseFunction::Setinvariable(Tamgu* v, TamguDeclaration* dom, short idthread) {
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

void TamguDependencyKnowledgeBaseFunction::Resetintvariable(TamguDeclaration* dom, short idthread) {
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


short TamguDependencyKnowledgeBaseFunction::Idvar() {
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
// Predicate variables can have methods...
//-------------------------------------------------------------------------------------------------
Exporting basebin_hash<predicateMethod> TamguPredicate::methods;
Exporting hmap<string, string> TamguPredicate::infomethods;
Exporting basebin_hash<unsigned long> TamguPredicate::exported;

Exporting short TamguPredicate::idtype = 0;

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void TamguPredicate::AddMethod(TamguGlobal* global, string name, predicateMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void TamguPredicate::Setidtype(TamguGlobal* global) {
    TamguPredicate::InitialisationModule(global,"");
}


   bool TamguPredicate::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();
    
    TamguPredicate::idtype = global->Getid("predicate");

    TamguPredicate::AddMethod(global, "_initial", &TamguPredicate::MethodInitial, P_ONE, "_initial(): Get the associated object.");
    TamguPredicate::AddMethod(global, "leaves", &TamguPredicate::MethodLeaves, P_NONE, "leaves(): Return the leaf values.");
    TamguPredicate::AddMethod(global, "features", &TamguPredicate::MethodFeatures, P_NONE, "features(): Return the dependency features.");
    TamguPredicate::AddMethod(global, "name", &TamguPredicate::MethodName, P_NONE, "name(): Return the predicate name.");
    TamguPredicate::AddMethod(global, "trace", &TamguPredicate::MethodTrace, P_ONE, "trace(bool v): Set trace mode.");
    TamguPredicate::AddMethod(global, "rule", &TamguPredicate::MethodRuleid, P_NONE, "rule(): Return the rule id.");

    if (version != "") {
        global->newInstance[TamguPredicate::idtype] = new TamguPredicate(a_predicate, global);
        global->RecordMethods(TamguPredicate::idtype, TamguPredicate::exported);

        global->newInstance[a_predicatevar] = new TamguPredicateVar(global, a_predicatevar);
        global->RecordMethods(a_predicatevar, TamguPredicate::exported);

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
// Cleaning the variable instances created during the predicate evaluation...

inline void ClearInstances(TamguDeclaration* dom, basebin_hash<Tamgu*>& basedomain) {
    Tamgu** tb;
    binuint64 filter;
    short idx;
    long j;
    unsigned long qj;

    for (long ii = 0; ii < dom->declarations.tsize; ii++) {
        filter = dom->declarations.indexes[ii];
        if (filter) {
            tb = dom->declarations.table[ii];
            j = 0;
            while (filter) {
#ifdef INTELINTRINSICS
                if (!(filter & 1)) {
                    if (!(filter & 0x00000000FFFFFFFF)) {
                        filter >>= 32;
                        j += 32;
                    }
					bitscanforward(qj, (uint32_t)(filter & 0x00000000FFFFFFFF));
                    filter >>= qj;
                    j += qj;
                }
#else
                if (!(filter & 1)) {
                    while (!(filter & 65535)) {
                        filter >>= 16;
                        j = j + 16;
                    }
                    while (!(filter & 255)) {
                        filter >>= 8;
                        j = j + 8;
                    }
                    while (!(filter & 15)) {
                        filter >>= 4;
                        j = j + 4;
                    }
                    while (!(filter & 1)) {
                        filter >>= 1;
                        j++;
                    }
                }
#endif
                //we do not delete the instances that were created by the underlying calls to PredicateEvalue
                //More exactly, when we push a new analysis of a rule into action, we want to clean the variables that were created
                //within this call, not the current ones.
                idx = ((ii + dom->declarations.base) << binbits) + j;
                if (!basedomain.check(idx) || basedomain[idx] != tb[j])
                    tb[j]->Resetreferencenoprotect();
                filter >>= 1;
                j++;
            }
        }
    }

    dom->declarations = basedomain;
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
        if (!protect) {
            if (idx == -1)
                delete this;
            else {
                protect = true;
                used = false;
                if (!globalTamgu->threadMODE)
                    globalTamgu->pviempties.push_back(idx);
            }
        }
    }
}

void TamguPredicateVariableInstance::Resetreferencenoprotect(short inc) {
    if (value != aNOELEMENT)
        value->Resetreferencenoprotect(inc);
    
    reference -= inc;
    if (reference <= 0) {
        if (idx == -1)
            delete this;
        else {
            protect = true;
            reference = 0;
            used = false;
            if (!globalTamgu->threadMODE)
                globalTamgu->pviempties.push_back(idx);
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
bool ThreadStruct::TestPredicate(TamguDeclaration* dom, TamguPredicate* p) {
    if (p->name != a_universal && knowledgebase.find(p->name) == knowledgebase.end())
        return false;
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

char ThreadStruct::isaValidPredicate(TamguDeclaration* dom, TamguPredicate* p, hmap<unsigned short, vector<TamguPredicateRule*> >& rulebase) {
    if (p->name != a_universal && knowledgebase.find(p->name) == knowledgebase.end())
        return 1;
        
    long i;
    if (p->name == a_universal) {
        hmap<short, vector<TamguPredicate*> >::iterator it;
        for (it = knowledgebase.begin(); it != knowledgebase.end(); it++) {
            vector<TamguPredicate*>& v = it->second;
            for (i = 0; i < v.size(); i++) {
                if (p->Unify(dom, v[i]))
                    return 2;
            }
        }
        return 0;
    }

    vector<TamguPredicate*>& v = knowledgebase[p->name];
    for (i = 0; i < v.size(); i++) {
        if (p->Unify(dom, v[i]))
            return 2;
    }
    //we keep a chance of finding a rule to match with
    if (rulebase.find(p->name) != rulebase.end())
        return 1;
    return 0;
}

bool ThreadStruct::GetPredicates(TamguDeclaration* dom, TamguPredicate* p, vector<TamguPredicate*>& res, bool cut) {
    
    bool ret = false;
    long i;

    if (p->name == a_universal) {
        hmap<short, vector<TamguPredicate*> >::iterator it;
        for (it = knowledgebase.begin(); it != knowledgebase.end(); it++) {
            vector<TamguPredicate*>& v = it->second;
            for (i = 0; i < v.size(); i++) {
                if (v[i]->isChosen())
                    continue;

                if (p->Unify(dom, v[i])) {
                    res.push_back(v[i]);
                    if (cut)
                        return true;
                    ret = true;
                }
            }
        }
        return ret;
    }
    vector<TamguPredicate*>& v = knowledgebase[p->name];
    for (i = 0; i < v.size(); i++) {
        if (v[i]->isChosen())
            continue;

        if (p->Unify(dom, v[i])) {
            res.push_back(v[i]);
            if (cut)
                return true;
            ret = true;
        }
    }
    return ret;
}


bool ThreadStruct::StorePredicate(TamguDeclaration* dom, TamguPredicate* pv, bool last) {
    if (TestPredicate(dom, pv))
        return false;
        
    if (pv->Name() == a_universal) {
        if (pv->Idvar() != 0) {
            //we share the same name as with the dependency that was associated with a variable...
            Tamgu* a = dom->Declaration(pv->Idvar());
            if (a != NULL && a->Type() == a_dependency)
                pv->name = a->Name();
            else
                return false;
        }
        else
            return false;
    }

    if (last)
        knowledgebase[pv->name].push_back(pv);
    else
        knowledgebase[pv->name].insert(knowledgebase[pv->name].begin(), pv);

    pv->Setreference();
    return true;
}

bool ThreadStruct::RemovePredicates(TamguDeclaration* dom, TamguPredicate* p) {

    if (knowledgebase.find(p->name) == knowledgebase.end())
        return false;

    vector<TamguPredicate*>& v = knowledgebase[p->name];
    bool res = false;
    for (BLONG i = v.size() - 1; i >= 0; i--) {
        if (p->same(v[i]) == aTRUE) {
            v[i]->Resetreference();
            v.erase(v.begin() + i);
            res = true;
        }
    }
    return res;
}

bool ThreadStruct::RemoveThePredicate(TamguDeclaration* dom, TamguPredicate* p) {
    
    vector<TamguPredicate*>& v = knowledgebase[p->name];
    for (BLONG i = 0; i < v.size(); i++) {
        if (p == v[i]) {
            p->Resetreference();
            v.erase(v.begin() + i);
            return true;
        }
    }
    return false;
}


//----------------------------------------------------------------------------------------

Tamgu* TamguPredicateVariableInstance::same(Tamgu* a) {
    Tamgu* dom = globalTamgu->Declarator(predicatezone, globalTamgu->GetThreadid());
    if (dom == aNULL)
        return aFALSE;

    Tamgu* v = Value((TamguDeclaration*)dom);

    if (v == aNOELEMENT)
        return aFALSE;
    return v->same(a);
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

Tamgu* TamguDependencyKnowledgeBaseFunction::same(Tamgu* a) {
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
            return globalTamgu->Returnerror("Unknown predicate variable", idthread);
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
    if (!dom->isDeclared(predicatezone)) {
        dom = globalTamgu->Declarator(predicatezone, idthread);
        if (dom == aNULL)
            return globalTamgu->Returnerror("Unknown predicate variable", idthread);
    }

    TamguPredicateVariableInstance* v = (TamguPredicateVariableInstance*)Variable((TamguDeclaration*)dom);
    if (v->value == ke)
        return aTRUE;

    Tamgu* val = v->value;
    v->value = ke->Atom(true);
    val->Resetreference(reference);
    v->value->Setreference(reference);
    return v->value;
}

Tamgu* TamguPredicateTerm::Put(Tamgu* dom, Tamgu* ke, short idthread) {
    if (!dom->isDeclared(predicatezone)) {
        dom = globalTamgu->Declarator(predicatezone, idthread);
        if (dom == aNULL)
            return aFALSE;
    }

    if (ke->Type() != a_term || ke->Size() != parameters.size() || !ke->isName(name)) {
        dom->Setfail(true);
        return aTRUE;
    }
    TamguPredicateTerm* kpf = (TamguPredicateTerm*)ke;

    for (long i = 0; i < parameters.size(); i++)
        parameters[i]->Put(dom, kpf->parameters[i], idthread);
    return aTRUE;
}

Tamgu* TamguPredicateConcept::Put(Tamgu* dom, Tamgu* ke, short idthread) {
    if (!dom->isDeclared(predicatezone)) {
        dom = globalTamgu->Declarator(predicatezone, idthread);
        if (dom == aNULL)
            return aFALSE;
    }

    if (ke->Type() != a_concept || ke->Size() != parameters.size() || globalTamgu->Checkhierarchy(name, ke->Name())) {
        dom->Setfail(true);
        return aTRUE;
    }

    TamguPredicateConcept* kpf = (TamguPredicateConcept*)ke;
    
    for (long i = 0; i < parameters.size(); i++)
        parameters[i]->Put(dom, kpf->parameters[i], idthread);
    return aTRUE;
}


Tamgu* TamguPredicate::Put(Tamgu* dom, Tamgu* val, short idthread) {
    return globalTamgu->Returnerror("PRE(009): You cannot instanciate a predicate this way", idthread);
}

Tamgu* TamguPredicateVar::Put(Tamgu* dom, Tamgu* val, short idthread) {
    if (val->Type() != ptype) {
        globalTamgu->Returnerror("PRE(001): Cannot instantiate a predicate with this value", idthread);
        return aTRUE;
    }

    //val cannot contain any PredicateVariable
    long i;
    Tamgu* e;
    vector<Tamgu*> v;

    for (i = 0; i < val->Size(); i++) {
        e = val->Parameter(i)->Eval(dom, aNULL, idthread);
        if (!e->isUnified((TamguDeclaration*)dom)) {
            string message("PRE(001): Cannot instantiate a predicate with this value");
            globalTamgu->Returnerror(message, idthread);
            return aTRUE;
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
    TamguReference::Resetreference(inc);
}

void TamguPredicateFunction::Resetreference(short inc) {
    TamguPredicate::Resetreference(inc);
}


string TamguPredicateKnowledgeBaseFunction::String() {
    string s;
    switch (name) {
        case a_asserta:
        s = "asserta";
        break;
        case a_assertz:
        s = "assertz";
        break;
        case a_retract:
        s = "retract";
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

void TamguPredicateKnowledgeBaseFunction::Setstring(string& s, short idthread) {
    switch (name) {
        case a_asserta:
            s = "asserta";
            break;
        case a_assertz:
            s = "assertz";
            break;
        case a_retract:
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
    return v.str();
}

string TamguPredicateVariableInstance::String() {
    Tamgu* dom = globalTamgu->Declarator(predicatezone, globalTamgu->GetThreadid());
    if (dom == aNULL)
        return globalTamgu->Getsymbol(name);

    TamguPredicateVariableInstance* vx = (TamguPredicateVariableInstance*)Variable((TamguDeclaration*)dom);

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
    return s.str().c_str();
}

string TamguPredicateTerm::String() {
    string v;
    if (name != a_universal)
        v = globalTamgu->Getsymbol(name);
    v += "(";
    for (long i = 0; i < parameters.size(); i++) {
        if (i)
            v += ",";
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
        v += parameters[i]->JSonString();
    }
    v += ")";
    return v;
}


string TamguPredicate::String() {
    string v = globalTamgu->Getsymbol(name);
    long sz = parameters.size();

    if (sz) {
        v += "(";
        for (long i = 0; i < sz; i++) {
            if (i)
                v += ",";
            v += parameters[i]->JSonString();
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
        v += parameters[i]->JSonString();
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
        v += parameters[i]->JSonString();
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
            v += parameters[i]->JSonString();
        }
        v += ")";
    }
}


string TamguPredicate::JSonString() {
    stringstream res;

    res << "[\"" << globalTamgu->Getsymbol(name) << "\",";
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

    long sz = parameters.size();
    if (sz) {
        v += "(";
        for (long i = 0; i < sz; i++) {
            if (i)
                v += ",";
            v += parameters[i]->JSonString();
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
            v += parameters[i]->JSonString();
        }
        v += ")";
    }
}

string TamguDependency::JSonString() {
    stringstream res;

    res << "[\"" << globalTamgu->Getsymbol(name) << "\",";
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
        e = Value("name");
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
            e = Value(buff);
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
    ptype = t;
    disjunction = false;
    negation = false;
    investigate |= is_predicate;
}

//This is a hack (quite ugly) which is used to avoid recording into the tracker temporary elements...
TamguPredicate::TamguPredicate(TamguGlobal* g, short n) : name(n) {
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
    Tamgu* v = this;
    short i;
    unsigned short r;
    while (dom->declarations.get(v->Name(), i, r) && v != dom->declarations.table[i][r])
        v = dom->declarations.table[i][r];
    return ((TamguPredicateVariableInstance*)v)->value;
}

Tamgu* TamguPredicateVariableInstance::Variable(TamguDeclaration* dom) {
    Tamgu* v = this;
    short i;
    unsigned short r;
    while (dom->declarations.get(v->Name(), i, r) && v != dom->declarations.table[i][r])
        v = dom->declarations.table[i][r];
    return v;
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
        kpvi = globalTamgu->Providepvi(a_universal);
        kpvi->Setreference();
        dom->declarations[kpvi->Name()] = kpvi;
    }
    else {
        if (context->Checkdico(name))
            kpvi = context->Getdico(name);
        else {
            kpvi = globalTamgu->Providepvi(name);
            context->Setdico(name, kpvi);
            kpvi->Setreference();
            dom->declarations[kpvi->Name()] = kpvi;
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
        kpvi = globalTamgu->Providepvi(name);
        context->Setdico(name, kpvi);
        kpvi->Setreference();
        dom->declarations[kpvi->Name()] = kpvi;
    }

    if (previousinstance == NULL)
        return kpvi;

    return kpvi->ExtractPredicateVariables(context, dom, previousinstance, NULL, idthread, root);
}

Tamgu* TamguPredicateVariableInstance::ExtractPredicateVariables(Tamgu* context, TamguDeclaration* dom, Tamgu* previousinstance, Tamgu* E, short idthread, bool root) {
    if (previousinstance->Type() == a_instance) {
        previousinstance = previousinstance->Variable(dom);
        Tamgu* val = previousinstance->VariableValue(dom, idthread);
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
        if (dom->Declaration(previousinstance->Name()) != this) {
            dom->declarations[previousinstance->Name()] = this;
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
            TamguPredicateVariableInstance* C = (TamguPredicateVariableInstance*)param->Variable(dom);
            if (C->value == aUNIVERSAL)
                return param;

            if (C->value == aNOELEMENT) {
                short ln = ((TamguPredicateVariableInstance*)param)->labelname;
                //In this case, we need to duplicate this variable, in order to clean it later on...
                //Otherwise, it will keep its value for the rest of the evaluation
                //we create a new predicate instance, with the same label but a new name...
                C = globalTamgu->Providepvi(ln);
                if (e->isConst()) {
                    //then we store our value...
                    C->value = e->Atom(true);
                }
                else
                    C->value = e;

                contextualpattern->Setdico(ln, C);

                dom->declarations[C->Name()] = C;
                dom->declarations[param->Name()] = C;
                C->Setreference(2);
                return e;
            }

            //else in this case, we use the value of that variable to compare it with the one expected stored in 'e'
            param = C->value;
        }

        if (e != aUNIVERSAL && param != NULL && param != aNOELEMENT && param != aNULL)
            if (e->same(param)->Boolean() == false)
                return NULL;
    }
    return e;
}


Tamgu* TamguConstvectormerge::ExtractPredicateVariables(Tamgu* context, TamguDeclaration* dom, Tamgu* csend, Tamgu* c, short idthread, bool root) {
    c = csend;
    if (csend != NULL) {
        if (csend->Type() != a_vector) {
            if (csend->Type() != a_instance)
                return NULL;
            c = csend->Variable(dom);
            Tamgu* val = c->VariableValue(dom, idthread);
            if (val != aNOELEMENT)
                c = val;
        }
    }

    long szc = -1;
    Tamgu* e;
    csend = c;
    long i;
    Tamguvector* localvect = NULL;
    if (c != NULL && c->Type() == a_vector) {
        szc = c->Size();
        if (szc) {
            Tamgu* lst = c->Last();
            if (lst->isMerge()) {
                localvect = globalTamgu->Providevector();
                for (i = 0; i < szc - 1; i++)
                    localvect->values.push_back(c->getvalue(i));
                lst = lst->VariableValue(dom, idthread)->getvalue(0)->VariableValue(dom, idthread);
                for (i = 0; i < lst->Size(); i++)
                    localvect->values.push_back(lst->getvalue(i));
                c = localvect;
                szc = c->Size();
            }
            else
                localvect = NULL;
        }
    }

    Tamguvector* vect = globalTamgu->Providevector();
    for (i = 0; i < values.size(); i++) {
        e = values[i];
        if (szc != -1) {
            //then, if it is the last element, then we associate our element with the rest of the vector
            if (i == values.size() - 1) {
                if (values.size() == 1)
                    e = e->ExtractPredicateVariables(context, dom, c, NULL, idthread, false);
                else {
                    //Else we need to extract a subset of that vector
                    Tamguvector subvect;
                    for (long j = i; j < c->Size(); j++)
                        subvect.values.push_back(((Tamguvector*)c)->values[j]);
                    e = e->ExtractPredicateVariables(context, dom, &subvect, NULL, idthread, false);
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
                e = e->ExtractPredicateVariables(context, dom, ((Tamguvector*)c)->values[i], NULL, idthread, false);
            }
        }
        else
            e = e->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, false);

        if (e == NULL) {
            if (localvect != NULL) {
                localvect->values.clear();
                localvect->Release();
            }
            vect->Release();
            return NULL;
        }
        vect->Push(e);
    }

    if (localvect != NULL) {
        localvect->values.clear();
        localvect->Release();
    }
    
    TamguPredicateVariableInstance* kpvi = globalTamgu->Providepvi(a_map);

    if (isMerge())
        kpvi->merge = true;

    kpvi->value = vect;
    kpvi->Setreference();
    dom->declarations[kpvi->name] = kpvi;

    if (csend != NULL && csend->Type() == a_instance) {
        context->Setdico(csend->Name(), kpvi);
        dom->declarations[csend->Name()] = kpvi;
        kpvi->Setreference();
    }
    return kpvi;
}

Tamgu* Tamguvector::ExtractPredicateVariables(Tamgu* context, TamguDeclaration* dom, Tamgu* csend, Tamgu* c, short idthread, bool root) {
    c = csend;
    if (csend != NULL) {
        if (!csend->isVectorContainer()) {
            if (csend->Type() != a_instance)
                return NULL;
            c = csend->Variable(dom);
            Tamgu* val = c->VariableValue(dom, idthread);
            if (val != aNOELEMENT)
                c = val;
        }
    }


    csend = c;
    long i, j;
    long sz = values.size();
    long szc = -1;
    Tamgu* kval;
    TamguPredicateVariableInstance* kpvi;
    Tamguvector* localvect = NULL;
    if (c != NULL && c->isVectorContainer()) {
        szc = c->Size();
        if (szc) {
            if (c->Last()->isMerge()) {
                if (!sz)
                    return NULL;
                kpvi = (TamguPredicateVariableInstance*)c->Last();
                localvect = globalTamgu->Providevector();
                for (i = 0; i < szc - 1; i++)
                    localvect->values.push_back(c->getvalue(i));
                kval = kpvi->value->getvalue(0)->VariableValue(dom, idthread);
                szc = kval->Size();
                for (i = 0; i < szc; i++)
                    localvect->values.push_back(kval->getvalue(i));
                c = localvect;
                szc = localvect->values.size();
            }
            else
                localvect = NULL;
        }
        if (!sz || !values.back()->isMerge()) {
            if (szc != sz) {
                if (localvect != NULL) {
                    localvect->values.clear();
                    localvect->Release();
                }
                return NULL;
            }
        }
    }

    Tamgu* e;
    Tamguvector* vect = globalTamgu->Providevector();
    bool merge = false;

    for (i = 0; i < sz; i++) {
        e = values[i];
        if (szc != -1) {
            //In the case of merge, we need to extract from c the rest of the vector...
            if (e->isMerge()) {
                if (c == localvect) {
                    merge = true;
                    localvect->values.erase(localvect->values.begin(), localvect->values.begin() + i);
                    e = e->ExtractPredicateVariables(context, dom, localvect, NULL, idthread, false);
                }
                else {
                    localvect = globalTamgu->Providevector();

                    merge = false;
                    for (j = i; j < c->Size(); j++) {
                        kval = c->getvalue(j);
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
                if (szc <= i) {
                    vect->Release();
                    if (localvect != NULL) {
                        localvect->values.clear();
                        localvect->Release();
                    }
                    return NULL;
                }
                kval = c->getvalue(i);
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
        kpvi = globalTamgu->Providepvi(a_vector);

        kpvi->value = vect;
        dom->declarations[kpvi->name] = kpvi;
        kpvi->Setreference();
        if (csend != NULL && csend->Type() == a_instance) {
            context->Setdico(csend->Name(), kpvi);
            dom->declarations[csend->Name()] = kpvi;
            kpvi->Setreference();
        }
        return kpvi;
    }
    return vect;
}

Tamgu* TamguConstmap::ExtractPredicateVariables(Tamgu* context, TamguDeclaration* dom, Tamgu* C, Tamgu* E, short idthread, bool root) {
    Tamgu* c = C;
    if (C != NULL) {
        if (!C->isMapContainer()) {
            if (C->Type() != a_instance)
                return NULL;
            c = C->Variable(dom);
            Tamgu* val = c->VariableValue(dom, idthread);
            if (val != aNOELEMENT)
                c = val;
        }
    }


    long sz = values.size();
    long szc = -1;
    if (c != NULL && c->isMapContainer()) {
        szc = c->Size();
        if ((!szc && sz) || (szc && !sz))
            return NULL;
        //No merge, then the size should be equal
        if (!sz || keys.back()->Name() != a_pipe) {
            if (sz != szc)
                return NULL;
        }
    }

    TamguIteration* iter;
    if (c == NULL)
        iter = new TamguIteration(false);
    else
        iter = c->Newiteration(false);
    iter->Begin();
    TamguConstmap* kmap = new TamguConstmap;
    Tamgu* ek;
    Tamgu* ev;
    Tamgu* ck;
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
                Tamgu* remaining = c->Newinstance(idthread);
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
        TamguPredicateVariableInstance* kpvi = globalTamgu->Providepvi(a_map);

        kpvi->value = kmap;
        dom->declarations[kpvi->name] = kpvi;
        kpvi->Setreference();
        if (c != NULL && c->Type() == a_instance) {
            context->Setdico(c->Name(), kpvi);
            dom->declarations[c->Name()] = kpvi;
            kpvi->Setreference();
        }
        return kpvi;
    }
    return kmap;
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
        TamguPredicateVariableInstance* kpvi = globalTamgu->Providepvi(a_term);

        kpvi->value = term;
        kpvi->Setreference();
        dom->declarations[kpvi->name] = kpvi;

        if (c != NULL && c->Type() == a_instance) {
            context->Setdico(c->Name(), kpvi);
            dom->declarations[c->Name()] = kpvi;
            kpvi->Setreference();
        }
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
        TamguPredicateVariableInstance* kpvi = globalTamgu->Providepvi(a_term);

        kpvi->value = e;
        kpvi->Setreference();
        dom->declarations[kpvi->name] = kpvi;

        if (c != NULL && c->Type() == a_instance) {
            context->Setdico(c->Name(), kpvi);
            dom->declarations[c->Name()] = kpvi;
            kpvi->Setreference();
        }
        return kpvi;
    }

    return concept;
}
//---------------------------------------------------------------------

TamguPredicate* TamguPredicateKnowledgeBaseFunction::Duplicate(Tamgu* context, TamguDeclaration* dom, short idthread) {
    Tamgu * e;

    TamguPredicateKnowledgeBaseFunction* p;

    if (context == NULL && dom == NULL) {
        p = (TamguPredicateKnowledgeBaseFunction*)Newinstance(0);

        for (long i = 0; i < parameters.size(); i++) {
            e = parameters[i]->Atom(true);
            p->parameters.push_back(e);
        }
        p->Setreference();
        return p;
    }

    p = (TamguPredicateKnowledgeBaseFunction*)Newinstance(0);

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

TamguPredicate* TamguPredicate::Copyfrom(Tamgu* context, TamguDeclaration* dom, TamguPredicate* C, short idthread) {
    Tamgu * e;

    TamguPredicate* p = new TamguPredicate(globalTamgu, name);

    for (long i = 0; i < parameters.size(); i++) {
        e = parameters[i]->ExtractPredicateVariables(context, dom, C->parameters[i], NULL, idthread, true);
        if (e == NULL) {
            p->Release();
            return NULL;
        }

        p->parameters.push_back(e);
        e->Setreference();
    }

    p->reference++;
    return p;
}

//---------------------------------------------------------------------

Exporting Tamgu* Tamgu::EvaluePredicateVariables(Tamgu* context, TamguDeclaration* dom, short idthread) {
    Tamgu* e = Eval(context, dom, idthread);
    return e->Atom(true);
}

Tamgu* TamguBasePredicateVariable::EvaluePredicateVariables(Tamgu* context, TamguDeclaration* dom, short idthread) {
    TamguPredicateVariableInstance* kpi = globalTamgu->Providepvi(name);
    dom->declarations[kpi->name] = kpi;
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

//---------------------------------------------------------------------

TamguPredicate* TamguInstructionEvaluate::PredicateUnification(VECTE<Tamgu*>& goals, long& posreplace, long& from) {
    posreplace = -1;
    from = 0;
    TamguPredicate* headpredicate = NULL;
    Tamgu* e;
    Tamgu* res;
    bool keep = false;
    bool test;

    for (long i = 0; i < goals.size(); i++) {
        e = goals[i];
        if (e->isPredicate()) {
            if (e == aFAIL || e == aSTOP)
                return (TamguPredicate*)e;

            if (e == aCUT)
                test = false;
            else //isUnified tests whether the current predicate is fully "unifiable": all variables have been unified...
                test = ((TamguPredicate*)e)->isUnified(dom);

            short eval = ((TamguPredicate*)e)->kbaction();

            if (test) {
                //If it is an action, such as assert then we return the current headpredicate...
                if (eval >= a_asserta && eval <= a_remove) {
                    if (!keep) {
                        headpredicate = (TamguPredicate*)e;
                        from = i + 1;
                    }
                    return headpredicate;
                }

                //We then evaluate it... This function returns 3 values:
                // 0: fail
                // 1: does not match any fact in the knowledgebase fact
                // 2: found a matching fact in the knowledge base
                eval = globalTamgu->isaValidPredicate(dom, ((TamguPredicate*)e), rules, threadowner);
                if (e->isNegation()) {
                    //if we have a match in the knowledge base, then it is an error
                    if (eval == 2)
                        return aFAIL;
                    //A case a little bit complex
                    //If we have a negation then we will need to evaluate more thoroughly this element
                    //It might not belong to the knowledgebase
                    if (eval == 1) {
                        if (!keep) {
                            headpredicate = (TamguPredicate*)e;
                            keep = true;
                            from = i + 1;
                        }
                    }
                }
                else {
                    if (eval == 0)
                        return aFAIL;
                    else
                        if (eval == 1)
                            test = false;
                        else //this is a case when a dependency is associated to a variable or under a hat... We still need to handle it
                            //for instance, it could need to be deleted or changed...
                    if (((TamguPredicate*)e)->Idvar())
                        test = false;
                }
            }

            if (!test) { //still some variables to go

                if (headpredicate == NULL && ((TamguPredicate*)e)->Checkparameters(dom)) {
                    if (eval == a_asserta || eval == a_assertz)
                        return aFAIL;
                    headpredicate = (TamguPredicate*)e;
                    posreplace = i;
                    keep = true;
                }
            }
            else {
                if (!keep) //If we have not found anything yet as a head, we will start the evaluation from the following element...
                from = i + 1;
            }
        }
        else
            if (headpredicate == NULL) {
                res = e->Eval(this, dom, threadowner);
                if (res->Boolean() == false) {
                    //if res==aNOELEMENT, then it means that there are no unified variables...
                    if (!e->isNegation() && !dom->Failed() && res != aNOELEMENT)
                        return aFAIL;
                    keep = true;
                }
                else {
                    if (e->isNegation())
                        return aFAIL;
                    if (!keep)
                        from = i + 1;
                }
            }
            else
                e->setSucessfull(false);
    }
    return headpredicate;
}

//-------------------------------------------------------------------
Tamgu* Tamgu::Getvalues(TamguDeclaration* dom, bool duplicate) {
    if (duplicate && !isConst())
        return Atom(true);
    return this;
}

Tamgu* Tamgusynode::Getvalues(TamguDeclaration* dom, bool duplicate) {
    return this;
}

Tamgu* TamguPredicateVariableInstance::Getvalues(TamguDeclaration* dom, bool duplicate) {
    Tamgu* v = Value(dom);
    v = v->Getvalues(dom, duplicate);
    if (merge && v->Type() == a_vector)
        v->Setmerge();
    return v;
}

Tamgu* TamguPredicateTerm::Getvalues(TamguDeclaration* dom, bool duplicate) {
    TamguPredicateTerm* term = new TamguPredicateTerm(globalTamgu, name);
    Tamgu* e;
    for (long i = 0; i < parameters.size(); i++) {
        e = parameters[i]->Getvalues(dom, duplicate);
        if (e == aNOELEMENT) {
            term->Release();
            return aNOELEMENT;
        }
        term->parameters.push_back(e);
    }
    return term;
}

Tamgu* TamguPredicateConcept::Evalue(TamguDeclaration* dom, short idthread, bool duplicate) {
    TamguCallFunctionArgsTaskell hfunc(fconcept);

    Tamgu* e;
    for (long i = 0; i < parameters.size(); i++) {
        e = parameters[i]->Evalue(dom, idthread, duplicate);
        if (e == aNOELEMENT)
            return aNOELEMENT;

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


Tamgu* TamguPredicateConcept::Getvalues(TamguDeclaration* dom, bool duplicate) {
    return Evalue(dom, globalTamgu->GetThreadid(), duplicate);
}

Tamgu* Tamguvector::Getvalues(TamguDeclaration* dom, bool duplicate) {
    Tamguvector* kvect = globalTamgu->Providevector();
    Tamgu* e;
    for (BLONG i = 0; i < values.size(); i++) {
        e = values[i]->Getvalues(dom, duplicate);
        if (e == aNOELEMENT) {
            kvect->Release();
            return aNOELEMENT;
        }
        if (e == NULL || e == aUNIVERSAL)
            continue;
        if (e->isMerge()) {
            Tamgu* kv = e->getvalue(0);
            if (kv->Type() == a_vector) {
                Tamguvector* vect = (Tamguvector*)kv;
                for (BLONG v = 0; v < vect->values.size(); v++)
                    kvect->Push(vect->values[v]);
                if (!e->isObject())
                    kv->Resetreference();
                else
                    e->Release();
            }
        }
        else
            kvect->Push(e);
    }
    return kvect;
}

Tamgu* TamguConstmap::Getvalues(TamguDeclaration* dom, bool duplicate) {
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
        func->Setreference();
        kfunc.arguments.push_back(pv);
        kfunc.arguments.push_back(func);
        e = kfunc.Eval(this, dom, threadowner);
        func->Resetreference();
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
            string n = globalTamgu->Getsymbol(kpi->name);
            if (kpi->isNegation())
                s << "~";
            s << n;
            if (kpi->isConst())
                continue;
            s << "(";
            for (j = 0; j < kpi->parameters.size(); j++) {
                n = kpi->parameters[j]->String();
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
            if (se->Name() != -1 || (se->Action() >= a_affectation && se->Action() <= a_notin))
                s << se->String();
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
}

//---------------------------------------------------------------------

Tamgu* TamguPredicateRuleItem::Unify(Tamgu* context, TamguDeclaration* dom, basebin_hash<TamguPredicateVariableInstance*>* dico, short idthread) {
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

class localpredict {
    public:
    VECTE<Tamgu*> localgoals;
    short idthread;

    localpredict(short id) {
        idthread = id;
        globalTamgu->Pushpredicate(id);

    }

    ~localpredict() {
        globalTamgu->Poppredicate(idthread);
    }

};

class predictvalue {
    public:

    VECTE<Tamgu*> localgoals;
    VECTE<Tamgu*> currentgoals;
    
    short idthread;

    predictvalue(long id) {
        idthread = id;
        globalTamgu->Pushpredicate(id);

    }

    ~predictvalue() {
        globalTamgu->Poppredicate(idthread);
    }
};

class kbpredict {
    public:

    vector<TamguPredicate*> kbase;
    basebin_hash<Tamgu*> kept;

};

Tamgu* TamguInstructionEvaluate::PredicateEvalue(VECTE<Tamgu*>& goals, TamguPredicate* currenthead, long depth) {
    if (fulltraversal == STOPSEARCH)
        return aCUT;

    if (globalTamgu->Error(threadowner))
        return aFALSE;

    Tamgu* res = aFALSE;
    long sz = goals.size();
    long i;

    //--------------------------------------------------------------------------------
    // Disjunction
    //If the top element in the goal stack is a disjunction, we then evaluate the next one...
    //The current one will be evaluated after... The disjunction is evaluated backward...
    if (sz && goals[0]->isDisjunction()) {
        std::unique_ptr<localpredict> Oo(new localpredict(threadowner));

        Oo->localgoals.taillor(sz);
        for (i = 1; i < sz; i++)
            Oo->localgoals.vecteur[i - 1] = goals.vecteur[i];
        Oo->localgoals.last = sz - 1;
        //---------------------------------------------------------------
        res = PredicateEvalue(Oo->localgoals, currenthead, depth);
        //---------------------------------------------------------------
        if (res == aCUT)
            return aTRUE;
        if (res == aBREAK || res == aTRUE)
            return res;
    }

    long posreplace, from;
    //--------------------------------------------------------------------------------
    //The evaluation of the goals, in which we detect which are the goals that full unified...
    // It returns NULL if everything has been unified, or the next predicate to be unified...
    TamguPredicate* headpredicate = PredicateUnification(goals, posreplace, from);
    
    //--------------------------------------------------------------------------------
    //We want to stop any further analysis...
    if (headpredicate == aSTOP) {
        fulltraversal = STOPSEARCH;
        return aCUT;
    }
    //--------------------------------------------------------------------------------
    //If the unification went wrong...
    if (headpredicate == aFAIL) {
        if (trace) {
            stringstream scerr;
            scerr << " --> Fail" << endl;
            PreLocalPrint(scerr.str());
        }
        return res;
    }

    //--------------------------------------------------------------------------------
    long j;
    //if we have a CUT...
    if (headpredicate == aCUT) {
        std::unique_ptr<localpredict> Oo(new localpredict(threadowner));

        for (j = from; j < sz; j++) {
            if (j == posreplace || goals[j]->hasbeenSuccesfull())
                continue;
            Oo->localgoals.push_back(goals[j]);
        }
        //---------------------------------------------------------------
        res = PredicateEvalue(Oo->localgoals, currenthead, depth);
        //---------------------------------------------------------------
        if (res == aTRUE)
            return aCUT;
        return res;
    }

    //--------------------------------------------------------------------------------
    //It is already all unified... The goal list is now empty, we can create our resulting predicates.
    if (headpredicate == NULL) {
        if (trace)
            PreLocalPrint(Endl);
        for (j = from; j < sz; j++) {
            if (!goals[j]->hasbeenSuccesfull())
                return res;
        }
        res = PredicateCreate(headrule, depth);
        if (res == aTRUE && fulltraversal == SEARCHONE) {
            fulltraversal = STOPSEARCH;
            return aCUT;
        }

        return res;
    }

    //--------------------------------------------------------------------------------
    // Functions that have been constructed on the basis of existing methods...
    if (headpredicate->isPredicateMethod()) {
        std::unique_ptr<localpredict> Oo(new localpredict(threadowner));

        if (headpredicate->negation) {

            Oo->localgoals.push_back(headpredicate);
            //We do not need to explore the whole world, one TRUE is enough to decide
            Oo->localgoals.push_back(aCUT);

            headpredicate->Setnegation(false);
            TamguPredicate* rec = headrule;
            //We do not need to build anything, the headrule is set to NULL to tell the system
            //That we are only interested in a local evaluation...
            headrule = NULL;
            //---------------------------------------------------------------
            res = PredicateEvalue(Oo->localgoals, currenthead, depth + 1);
            //---------------------------------------------------------------
            headrule = rec;
            headpredicate->Setnegation(true);

            //---------------------------------------------------------------
            if (res == aFALSE) {//Ok we can continue with our remaining goals...
                Oo->localgoals.clear();
                for (j = from; j < sz; j++) {
                    if (j == posreplace || goals[j]->hasbeenSuccesfull())
                        continue;
                    Oo->localgoals.push_back(goals[j]);
                }

                return PredicateEvalue(Oo->localgoals, currenthead, depth + 1);
            }

            //---------------------------------------------------------------
            //It was TRUE then it is FALSE...
            return aFALSE;
        }
        

        for (j = from; j < sz; j++) {
            if (j == posreplace || goals[j]->hasbeenSuccesfull())
                continue;
            Oo->localgoals.push_back(goals[j]);
        }

        //---------------------------------------------------------------
        return headpredicate->PredicateEvalue(this, Oo->localgoals, currenthead, depth);
        //---------------------------------------------------------------
    }

    //--------------------------------------------------------------------------------
    // Assertion and dependency removal are treated here...
    short kbaction = headpredicate->kbaction();
    TamguPredicate* kvpred;
    if (kbaction >= a_asserta && kbaction <= a_remove) {
        Tamgu* a = NULL;
        if (kbaction == a_remove) {
            a = dom->Declaration(predicatedependency);
            if (a == NULL || !a->isPredicate())
                return aFALSE;

            if (headpredicate->Idvar()) {
                //If it is a dependency modification
                //we remove it from the database...
                dom->Declare(predicatefeature, a);
                a->Setreference(); //we do not want this object to be fully deleted here
            }

            if (!globalTamgu->RemoveThePredicate(dom, (TamguPredicate*)a, threadowner))
                return aFALSE;
        }
        else {
            if (headpredicate->isDependency()) {
                a = dom->Declaration(predicatefeature);
                if (a != NULL) {//we clear it...
                    dom->declarations.erase(predicatefeature);
                    kvpred = new TamguDependency(globalTamgu,
                        ((TamguPredicate*)a)->Features(),
                        headpredicate->name,
                        headpredicate->Idvar());

                    kvpred->Setfeatures(headpredicate->Features());
                }
                else {
                    kvpred = new TamguDependency(globalTamgu,
                        headpredicate->Features(),
                        headpredicate->name,
                        headpredicate->Idvar());
                }
                kvpred->Setidrule(headpredicate->Idrule());
            }
            else
                kvpred = new TamguPredicate(globalTamgu, headpredicate->name);

            for (i = 0; i < headpredicate->parameters.size(); i++) {
                res = headpredicate->parameters[i]->Eval(this, dom, threadowner);
                kvpred->parameters.push_back(res->Atom(true));
            }

            if (kbaction == a_retract) {
                bool success = globalTamgu->RemovePredicates(dom, kvpred, threadowner);
                kvpred->Release();
                if (!success)
                    return aFALSE;
            }
            else {
                if (kbaction == a_asserta)
                    globalTamgu->StorePredicate(dom, kvpred, false, threadowner);
                else
                    globalTamgu->StorePredicate(dom, kvpred, true, threadowner);
            }
            
            if (a != NULL) {
                a->Resetreference();
                //we then reinject our novel dependency into our dom
                //it might be usefull for other loops...
                dom->Declare(predicatedependency, kvpred);
            }
        }

        std::unique_ptr<localpredict> Oo(new localpredict(threadowner));
        for (j = from; j < sz; j++) {
            if (j == posreplace || goals[j]->hasbeenSuccesfull())
                continue;
            Oo->localgoals.push_back(goals[j]);
        }
        //---------------------------------------------------------------
        return PredicateEvalue(Oo->localgoals, currenthead, depth);
        //---------------------------------------------------------------
    }

    if (trace)
        PreLocalPrint(Endl);

    //--------------------------------------------------------------------------------
    std::unique_ptr<predictvalue> Oo(new predictvalue(threadowner));
    //Some variables still need to be evaluated

    //We will try to solve headpredicate...
    //first it could be part of the knowledgebase

    //We only keep the elements that are still to be evaluated...
    bool check = false;
    for (j = from; j < sz; j++) {
        if (goals[j]->hasbeenSuccesfull())
            continue;
        if (check) {
            if (!goals[j]->isDisjunction())
                check = false;
            continue;
        }
        if (j == posreplace)
            posreplace = Oo->currentgoals.size();

        if (goals[j]->isDisjunction())
            check = true;
        Oo->currentgoals.push_back(goals[j]);
    }

    check = false;
    Tamgu* localres;
    
    //--------------------------------------------------------------------------------
    if (globalTamgu->Checkpredicate(headpredicate->name, threadowner)) {
        std::unique_ptr<kbpredict> Ko(new kbpredict());
        // This section is used to test our current predicate against the knowledgebase...
        if (globalTamgu->GetPredicates(dom, headpredicate, Ko->kbase, false, threadowner)) {
            //we set the res value to aTRUE, which will be considered as an error
            if (headpredicate->isNegation())
                res = aTRUE;
            else {
                sz = Oo->currentgoals.size();
                if (sz) {
                    Oo->localgoals.reserve(sz - 1);
                    Oo->localgoals.last = sz - 1;
                }
                i = 0;
                for (j = 0; j < sz; j++) {
                    if (j == posreplace)
                        continue;
                    Oo->localgoals.vecteur[i++] = Oo->currentgoals.vecteur[j];
                }

                sz = Ko->kbase.size();

                basebin_hash<Tamgu*>::iterator it;
                //we remove our element at the position posreplace in goal stack
                for (i = 0; i < sz; i++) {

                    Ko->kbase[i]->Setchosen(true);
                    //we then unify our values in dom with respect to the element in the goal stack (stored in headpredicate) and the extracted elements
                    for (j = 0; j < headpredicate->parameters.size(); j++)
                        headpredicate->parameters[j]->Insertvalue(dom, Ko->kbase[i]->parameters[j], Ko->kept);

                    if (trace)
                        Displaypredicatestack(d_kb, dom, headpredicate, Oo->localgoals, depth, this);
                        
                    //---------------------------------------------------------------
                    //In the case of dependency goals, we might need to store in a variable the current goal...
                    headpredicate->Setinvariable(Ko->kbase[i], dom, threadowner);
                    //If it is a potential dependency modification, then we protect it ahead of processing...
                    //Otherwise, the dependency could linger in kbase, being deleted...
                    //The explanation is that, we could have many occurrences of the same dependency name in the rule.
                    //If we store the current one as being a potential modification, and we go into recursion, then this current dependency might be
                    //deleted, however, since there are more than one loop in the knwoeledge base with the same query on the these dependencies,
                    //the current dependency might find itself in this second survey...
                    Ko->kbase[i]->Setreference();
                    //---------------------------------------------------------------
                    //We then continue on the rest of the goals...
                    localres = PredicateEvalue(Oo->localgoals, currenthead, depth + 1);
                    //---------------------------------------------------------------
                    //we then remove it, if it was inserted
                    headpredicate->Resetintvariable(dom, threadowner);
                    Ko->kbase[i]->Setchosen(false);

                    //we can safely return back to the actual reference of that dependency...
                    Ko->kbase[i]->Resetreference();
                    //---------------------------------------------------------------


                    if (localres == aTRUE || localres == aCUT) {
                        if (check) {
                            for (it = Ko->kept.begin(); it.table != NULL; it++)
                                dom->declarations[it->first]->Cleans(it->second, false);
                            if (localres == aCUT)
                                return aTRUE;
                            return localres;
                        }
                        else
                            res = localres;
                    }

                    //We clean them, then...
                    for (it = Ko->kept.begin(); it.table != NULL; it++)
                        dom->declarations[it->first]->Cleans(it->second, true);

                    Ko->kept.clear();

                    if (localres == aCUT)
                        return aTRUE;

                    if (localres == aBREAK)
                        return localres;

                    if (globalTamgu->Error(threadowner))
                        return aFALSE;
                }
            }
        }
    }

    //--------------------------------------------------------------------------------
    //Else there is no rules matching this predicate...
    if (rules.find(headpredicate->name) == rules.end()) {
        if (headpredicate->negation) {
            //---------------------------------------------------------------
            if (res == aFALSE) {
                //we then continue to analyse the rest of the goal stack
                return PredicateEvalue(Oo->currentgoals, currenthead, depth + 1);
            }
            //---------------------------------------------------------------
            return aFALSE;
        }
        return res; //no rule available
    }


    //--------------------------------------------------------------------------------
    //We have a negation...
    if (headpredicate->negation) {
        //We need to build a local call, which should return FALSE... We need only one TRUE to detect if the negation should fail
        Oo->localgoals.clear();

        Oo->localgoals.push_back(headpredicate);
        //We do not need to explore the whole world, one TRUE is enough to decide
        Oo->localgoals.push_back(aCUT);

        headpredicate->Setnegation(false);
        TamguPredicate* rec = headrule;
        //We do not need to build anything, the headrule is set to NULL to tell the system
        //That we are only interested in a local evaluation...
        headrule = NULL;
        //---------------------------------------------------------------
        res = PredicateEvalue(Oo->localgoals, currenthead, depth + 1);
        //---------------------------------------------------------------
        headrule = rec;
        headpredicate->Setnegation(true);
        
        //---------------------------------------------------------------
        if (res == aFALSE) //Ok we can continue with our remaining goals...
        return PredicateEvalue(Oo->currentgoals, currenthead, depth + 1);
        //---------------------------------------------------------------
        //It was TRUE then it is FALSE...
        return aFALSE;
    }

    //--------------------------------------------------------------------------------
    //we extract the rules matching our current target predicate...
    vector<TamguPredicateRule*>& rulebase = rules[headpredicate->name];

    sz = rulebase.size();
    TamguPredicateRule* rb;


    //--------------------------------------------------------------------------------
    //Now in the following section we evaluate which rules to enrich our goals with...
    //we sort the values according to some random algorithm
    if (sz)
        currenthead = (TamguPredicate*)Oo->currentgoals.remove(posreplace);

    VECTE<Tamgu*> rulegoals;
    basebin_hash<Tamgu*> basedomain = dom->declarations;
    std::unique_ptr<basebin_hash<TamguPredicateVariableInstance*> > refdico(new basebin_hash<TamguPredicateVariableInstance*>);
    Tamgu* e;

    for (i = 0; i < sz; i++) {
        rb = rulebase[i];
        if (rb->head->parameters.size() != currenthead->parameters.size())
            continue;

        Oo->localgoals = Oo->currentgoals;
        //Variable management
        dico = refdico.get();

        //localres = rulebase[i]->Evalue(this, currenthead, Oo->localgoals, posreplace, depth, cut);

        if (trace)
            Displaypredicatestack(d_rule, dom, rb->head, Oo->localgoals, depth, this);

        //headpredicate is not stored in the garbage collector, no need for that. Hence, we only need to destroy it to get rid of it...
        headpredicate = rb->head->Copyfrom(this, dom, currenthead, threadowner);
        if (headpredicate == NULL) {
            ClearInstances(dom, basedomain);

            if (trace) {
                stringstream scerr;
                scerr << " --> Fail" << endl;
                PreLocalPrint(scerr.str());
            }
            localres = aFALSE;
        }
        else {

            if (trace)
                PreLocalPrint(Endl);

            localres = aTRUE;
            if (rb->instructions.size() != 0) {
                for (j = 0; j < rb->instructions.size(); j++) {
                    e = ((TamguPredicateRuleItem*)rb->instructions[j])->Unify(this, dom, dico, threadowner);
                    if (e ==  NULL) {
                        localres = aFALSE;
                        break;
                    }
                    rulegoals.push_back(e);
                }
                
                if (localres == aTRUE) {
                    for (j = rulegoals.size() - 1; j >= 0; j--)
                        Oo->localgoals.insert(posreplace, rulegoals[j]);
                }
            }

            if (localres == aTRUE) {
                if (trace)
                    Displaypredicatestack(d_eval, dom, headpredicate, Oo->localgoals, depth, this);

                //---------------------------------------------------------------
                //THEN We use these new goals and apply PredicateEvaluate to this new list...
                localres = PredicateEvalue(Oo->localgoals, headpredicate, depth + 1);
                //---------------------------------------------------------------

            }

            //We clean the parameters and delete the head...
            headpredicate->Clear();
            delete headpredicate;

            for (j = 0; j < rulegoals.size(); j++) {
                if (rulegoals[j]->isConst())
                    continue;
                rulegoals[j]->Clear();
                delete rulegoals[j];
            }

            rulegoals.clear();
            ClearInstances(dom, basedomain);
        }

        refdico->clear();

        ///-----------------------OLD PREVIOUS LINES-------------------------------
        if (localres == aCUT)
            return aTRUE;

        if (localres == aBREAK)
            return localres;

        if (localres == aTRUE)
            res = localres;
            
        if (globalTamgu->Error(threadowner))
            return aFALSE;
    }

    return res;
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
    if (contextualpattern->Type() != a_predicateevaluate) {
        if (!dom->isDeclared(predicatedico)) {
            dom = globalTamgu->Declarator(predicatedico, idthread);
            if (dom == aNULL)
                return aNOELEMENT;
        }

        contextualpattern = dom->Declaration(predicatedico);
    }
    else
        dom = globalTamgu->Declarator(predicatedico, idthread);

    if (contextualpattern == NULL || !contextualpattern->Checkdico(name)) {
        dom->Setfail(true);
        return aNOELEMENT;
    }

    return contextualpattern->Getdico(name)->Eval(contextualpattern, dom, idthread);
}

Tamgu* TamguPredicateVariable::Eval(Tamgu* contextualpattern, Tamgu* dom, short idthread) {
    if (contextualpattern->Type() != a_predicateevaluate) {
        if (!dom->isDeclared(predicatedico)) {
            dom = globalTamgu->Declarator(predicatedico, idthread);
            if (dom == aNULL)
                return aNOELEMENT;
        }

        contextualpattern = dom->Declaration(predicatedico);
    }
    else
        dom = globalTamgu->Declarator(predicatedico, idthread);

    if (contextualpattern == NULL || !contextualpattern->Checkdico(name)) {
        dom->Setfail(true);
        return aNOELEMENT;
    }

    Tamgu* val = contextualpattern->Getdico(name);

    if (call != NULL) {
        val = val->Eval(contextualpattern, dom, idthread);
        return call->Eval(contextualpattern, val, idthread);
    }

    if (affectation)
        return val;

    return val->Eval(contextualpattern, dom, idthread);
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
            return globalTamgu->Returnerror("Wrong index", idthread);
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
            return globalTamgu->Returnerror("Wrong index", idthread);
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

        if (!globalTamgu->StorePredicate((TamguDeclaration*)dom, pv, true, idthread))
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

    if (!globalTamgu->RemovePredicates((TamguDeclaration*)dom, &pv, idthread)) {
        pv.Clear();
        return aFALSE;
    }
    pv.Clear();
    return aTRUE;
}


Tamgu* TamguInstructionPredicate::Eval(Tamgu* contextualpattern, Tamgu* dom, short idthread) {
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
            return globalTamgu->Returnerror("Wrong index", idthread);
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

    if (dom->Failed() || globalTamgu->Error(idthread))
        return aFALSE;

    if (negation) {
        if (!res->Boolean()) {
            success = true;
            return aTRUE;
        }
        return aFALSE;
    }


    if (res->Boolean())
        success = true;

    return res;
}

string TamguPredicateLocalInstruction::String() {
    TamguDeclaration* dom = (TamguDeclaration*)globalTamgu->Declarator(predicatezone, globalTamgu->GetThreadid());
    if (dom == aNULL)
        return "";

    dom->declarations[predicatedico] = this;
    string v = instruction->String();

    if (v == "") {
        Tamgu* ins = instruction;
        v = "[";
        short act = ins->Action();
        if (act == a_affectation) {
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
                    v += ins->Argument(i)->String();
                }
                v += ")";
            }
        }
        else {
            if (act != a_none && act != a_affectation)
                v += globalTamgu->operator_strings[act];
            else
                v += "ins";
        }
        v += "]";
    }
    dom->declarations.erase(predicatedico);
    if (instruction->Function() != NULL)
        v += instruction->Function()->String();
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
                for (long p = ins.size() - 1; p >= 0; p--) {
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
    TamguDeclarationPredicate domain;

    domain.declarations = ((TamguDeclaration*)domcall)->declarations;
    VECTE<Tamgu*> goals;
    goals.push_back(head);
    headrule = head;

    domain.Declare(predicatezone, this);
    globalTamgu->Pushstack(&domain, threadowner);

    if (contextualpattern->isContainer())
        fulltraversal = FULLSEARCH;

    dom = &domain;
    PredicateEvalue(goals, NULL, 0);

    globalTamgu->Popstack(threadowner);
    domain.declarations.erase(predicatezone);

    basebin_hash<Tamgu*>::iterator it;

    for (it = domain.declarations.begin(); it != domain.declarations.end(); it++) {
        if (!domcall->isDeclared(it->first)) {
            it->second->Setprotect(0);
            it->second->Resetreference();
        }
    }

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

Tamgu* TamguInstructionLaunch::Eval(Tamgu* context, Tamgu* val, short idthread) {
    //head is our definition
    //First we create an actionable TamguPredicate
    long i;
    
    Tamgu* dom;
    if (val == aAFFECTATION)
        dom = globalTamgu->Topstack(idthread);
    else
        dom = context;

    TamguPredicateFunction pv(globalTamgu, globalTamgu->predicates[head->name]->Function(), head->name);
    if (pv.function != NULL)
        pv.object = globalTamgu->predicates[head->name]->Returnobject();

    TamguInstructionEvaluate kl(globalTamgu, &pv);
    kl.threadowner = idthread;

    //Important variables...
    globalTamgu->SetPredicateVariableFlags();

    TamguDeclarationPredicate domain;

    basebin_hash<TamguPredicateVariableInstance*> localdico;
    kl.dico = &localdico;
    TamguPredicateContainer* kpcont = globalTamgu->GetPredicatecontainer(idthread);

    if (kpcont != NULL)
        kl.rules = kpcont->rules;

    if (globalTamgu->predicateTrace(head->name))
        kl.trace = true;

    Tamgu* e;
    for (i = 0; i < head->parameters.size(); i++) {
        e = head->parameters[i]->ExtractPredicateVariables(&kl, &domain, NULL, NULL, idthread, true);
        e->Setreference();
        pv.parameters.push_back(e);
    }

    e = kl.Eval(context, &domain, idthread);

    for (i = 0; i < pv.Size(); i++)
        pv.parameters[i]->Resetreference();


    basebin_hash<Tamgu*>::iterator it;
    for (it = domain.declarations.begin(); it != domain.declarations.end(); it++) {
        it->second->Setprotect(0);
        it->second->Resetreference();
    }

    return e;
}
