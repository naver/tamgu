/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : predicatefunctions.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "predicate.h"
#include "predicatefunctions.h"
#include "tamgusynode.h"

//----------------------------------------------------------------------------------------------------
Exporting TamguPredicateVariableInstance* TamguGlobal::Providevariableinstance(short n, short idthread) {
    if (threadMODE || Addtogarbage())
        return new TamguPredicateVariableInstance(GetName(idthread), n, idthread);

    TamguPredicateVariableInstancebuff* kvi;
    if (pviempties.last > 0) {
        kvi = pvireservoire[pviempties.backpop()];
        kvi->labelname = n;
        kvi->value = aNOELEMENT;
        kvi->used = true;
        return kvi;
    }
    
    long mx = pvireservoire.size();
    while (pviidx < mx) {
        if (!pvireservoire[pviidx]->used) {
            kvi = pvireservoire[pviidx++];
            kvi->labelname = n;
            kvi->value = aNOELEMENT;
            kvi->used = true;
            return kvi;
        }
        pviidx++;
    }
    
    char buffer[20];
    pvireservoire.resize(2*mx);
    pviidx = 2*mx;
    for (long i = mx; i < pviidx; i++) {
        pvireservoire[i] = new TamguPredicateVariableInstancebuff(i);
        sprintf_s(buffer, 20, "&p%ld&", i);
        pvireservoire[i]->name = Getid(buffer);
    }
    
    pviidx = mx;
    kvi = pvireservoire[pviidx++];
    kvi->labelname = n;
    kvi->value = aNOELEMENT;
    kvi->used = true;
    return kvi;
}

//----------------------------------------------------------------------------------------------------


Exporting bool TamguPredicateBetween::Checkparameters(TamguDeclaration* dom) {
    if (!parameters[0]->isUnified(dom) || !parameters[1]->isUnified(dom))
        return false;
    return true;
}

Exporting Tamgu* TamguPredicateBetween::PredicateEvalue(TamguInstructionEvaluate* context, VECTE<Tamgu*>& stack, TamguPredicate* currenthead, long depth) {
    //if we are here, it means that one of the variables has not yet been assigned to...
    //We accept only two cases, either the third variable is unified or is not...
    
    Tamgu* v=parameters[0]->Eval(context, context->dom, context->threadowner);
    long left = v->Integer();
    v->Release();
    v=parameters[1]->Eval(context, context->dom, context->threadowner);
    long right = v->Integer();
    v->Release();
    
    v = parameters[2];
    if (v->isUnified(context->dom)) {
        long val = v->Eval(context, context->dom, context->threadowner)->Integer();
        if (val >= left && val <= right)
            return context->PredicateEvalue(stack, currenthead, depth);
        return aFALSE;
    }


    Tamguint* val = globalTamgu->Provideint(0);
    v->Newvalue(val, context->threadowner);
    Tamgu* res = aFALSE;
    Tamgu* e;
    for (long i = left; i <= right; i++) {
        val->value = i;
        e = context->PredicateEvalue(stack, currenthead, depth);
        if (e == aTRUE) {
            res = e;
            continue;
        }

        if (e == aFALSE)
            continue;

        if (e == aCUT) {
            res = aTRUE;
            break;
        }

        if (e == aBREAK)
            break;
    }

    v->Setvalue(aNULL, aNOELEMENT, context->threadowner, false);
    return res;
}

Exporting TamguPredicate* TamguPredicateBetween::Duplicate(Tamgu* context, TamguDeclaration* dom, short idthread) {
    TamguPredicateBetween* p = new TamguPredicateBetween(globalTamgu, name);
    Tamgu* e;

    for (short i = 0; i < 3; i++) {
        e = parameters[i]->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, true);
        if (e == NULL) {
            p->Release();
            return NULL;
        }
        p->parameters.push_back(e);
    }

    p->Setreference();
    return p;
}

//----------------------------------------------------------------------------------------------------
Exporting Tamgu* TamguPredicateTermSucc::Getvalues(TamguDeclaration* dom, bool duplicate) {
    Tamgu* e = parameters[0]->Getvalues(dom, duplicate);
    if (e == aNOELEMENT)
        return aNOELEMENT;
    return e->Succ();
}

Exporting bool TamguPredicateSucc::Checkparameters(TamguDeclaration* dom) {
    if (!parameters[0]->isUnified(dom) && !parameters[1]->isUnified(dom))
        return false;
    return true;
}

Exporting Tamgu* TamguPredicateSucc::PredicateEvalue(TamguInstructionEvaluate* context, VECTE<Tamgu*>& stack, TamguPredicate* currenthead, long depth) {
    Tamgu* v;
    if (parameters[0]->isUnified(context->dom)) {
        Tamgu* v1 = parameters[0]->Eval(context, context->dom, context->threadowner);

        if (parameters[1]->isUnified(context->dom)) {
            v = parameters[1]->Eval(context, context->dom, context->threadowner);
            if (v == aUNIVERSAL)
                return aFALSE;

            Tamgu* v2 = v->Succ();
            v->Release();
            if (v1->same(v2)) {
                v2->Release();
                return context->PredicateEvalue(stack, currenthead, depth);
            }
            v2->Release();
            return aFALSE;
        }

        v = v1->Succ();
        parameters[1]->Newvalue(v, context->threadowner);
        v = context->PredicateEvalue(stack, currenthead, depth);
        parameters[1]->Setvalue(aNULL, aNOELEMENT, context->threadowner, false);
        return v;
    }

    if (parameters[1]->isUnified(context->dom)) {
        v = parameters[1]->Eval(context, context->dom, context->threadowner);
        if (v == aUNIVERSAL)
            return aFALSE;

        Tamgu* v2 = v->Pred();
        v->Release();
        parameters[0]->Newvalue(v2, context->threadowner);
        v = context->PredicateEvalue(stack, currenthead, depth);
        parameters[0]->Setvalue(aNULL, aNOELEMENT, context->threadowner, false);
        return v2;
    }

    return aFALSE;
}


Exporting TamguPredicate* TamguPredicateSucc::Duplicate(Tamgu* context, TamguDeclaration* dom, short idthread) {
    if (parameters.size() != 2)
        return NULL;

    TamguPredicateSucc* p = new TamguPredicateSucc(globalTamgu, name);
    Tamgu* e;

    for (short i = 0; i < 2; i++) {
        e = parameters[i]->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, true);
        if (e == NULL) {
            p->Release();
            return NULL;
        }
        p->parameters.push_back(e);
    }

    p->Setreference();
    return p;
}

//----------------------------------------------------------------------------------------------------
Exporting Tamgu* TamguPredicateTermPred::Getvalues(TamguDeclaration* dom, bool duplicate) {
    Tamgu* e = parameters[0]->Getvalues(dom, duplicate);
    if (e == aNOELEMENT)
        return aNOELEMENT;
    return e->Pred();
}

Exporting bool TamguPredicatePred::Checkparameters(TamguDeclaration* dom) {
    if (!parameters[0]->isUnified(dom) && !parameters[1]->isUnified(dom))
        return false;
    return true;
}

Exporting Tamgu* TamguPredicatePred::PredicateEvalue(TamguInstructionEvaluate* context, VECTE<Tamgu*>& stack, TamguPredicate* currenthead, long depth) {
    Tamgu* v;
    if (parameters[0]->isUnified(context->dom)) {
        Tamgu* v1 = parameters[0]->Eval(context, context->dom, context->threadowner);

        if (parameters[1]->isUnified(context->dom)) {
            v = parameters[1]->Eval(context, context->dom, context->threadowner);
            if (v == aUNIVERSAL)
                return aFALSE;

            Tamgu* v2 = v->Pred();
            v->Release();
            if (v1->same(v2)) {
                v2->Release();
                return context->PredicateEvalue(stack, currenthead, depth);
            }
            v2->Release();
            return aFALSE;
        }

        v = v1->Pred();
        parameters[1]->Newvalue(v, context->threadowner);
        v = context->PredicateEvalue(stack, currenthead, depth);
        parameters[1]->Setvalue(aNULL, aNOELEMENT, context->threadowner, false);
        return v;
    }

    if (parameters[1]->isUnified(context->dom)) {
        v = parameters[1]->Eval(context, context->dom, context->threadowner);
        if (v == aUNIVERSAL)
            return aFALSE;

        Tamgu* v2 = v->Succ();
        v->Release();
        parameters[0]->Newvalue(v2, context->threadowner);
        v = context->PredicateEvalue(stack, currenthead, depth);
        parameters[0]->Setvalue(aNULL, aNOELEMENT, context->threadowner, false);
        return v2;
    }

    return aFALSE;
}


Exporting TamguPredicate* TamguPredicatePred::Duplicate(Tamgu* context, TamguDeclaration* dom, short idthread) {
    if (parameters.size() != 2)
        return NULL;

    TamguPredicatePred* p = new TamguPredicatePred(globalTamgu, name);
    Tamgu* e;

    for (short i = 0; i < 2; i++) {
        e = parameters[i]->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, true);
        if (e == NULL) {
            p->Release();
            return NULL;
        }
        p->parameters.push_back(e);
    }

    p->Setreference();
    return p;
}

//----------------------------------------------------------------------------------------------------
Exporting Tamgu* TamguPredicateTermMethod::Getvalues(TamguDeclaration* dom, bool duplicate) {
    Tamgu* head = parameters[0]->Getvalues(dom, duplicate);
    if (head == aNOELEMENT)
        return aNOELEMENT;

    Tamgu* e;
    TamguCallFromCall call(name);
    short i;
    for (i = 1; i < parameters.size(); i++) {
        e = parameters[i]->Getvalues(dom, duplicate);
        if (e == aNOELEMENT) {
            head->Release();
            return aNOELEMENT;
        }
        call.arguments.push_back(e);
    }

    short idthread = globalTamgu->GetThreadid();
    Tamgu* res = call.Eval(dom, head, idthread);

    for (i = 0; i < call.arguments.size(); i++)
        call.arguments[i]->Release();
    head->Release();
    if (res->isError()) {
        globalTamgu->Cleanerror(idthread);
        return aNOELEMENT;
    }
    return res;
}
//----------------------------------------------------------------------------------------------------

Exporting bool TamguPredicateMethod::Checkparameters(TamguDeclaration* dom) {
    short last = (short)parameters.size() - 1;
    for (short i = 0; i < last; i++) {
        if (!parameters[i]->isUnified(dom))
            return false;
    }

    return true;
}

Exporting Tamgu* TamguPredicateMethod::PredicateEvalue(TamguInstructionEvaluate* context, VECTE<Tamgu*>& stack, TamguPredicate* currenthead, long depth) {
    vector<Tamgu*> args;
    short last = (short)parameters.size() - 1;
    short i;
    for (i = 0; i < last; i++) {
        if (parameters[i]->isUnified(context->dom))
            args.push_back(parameters[i]->Eval(context, context->dom, context->threadowner));
    }

    Tamgu* res = aFALSE;
    Tamgu* val;
    Tamgu* v;
    Tamgu* r;
    if (parameters[last]->isUnified(context->dom)) {
        v = parameters[1]->Eval(context, context->dom, context->threadowner);
        if (v == aUNIVERSAL) {
            for (i = 0; i < last; i++)
                args[i]->Release();
            
            return booleantamgu[negation];
        }

        TamguCallFromCall call(name);
        for (i = 1; i < last; i++)
            call.arguments.push_back(args[i]);

        val = call.Eval(context->dom, args[0], context->threadowner);
        if (val->isError()) {
            globalTamgu->Cleanerror(context->threadowner);
            v->Release();
            return aFALSE;
        }
        
        if (val->isVectorContainer()) {
            for (i = 0; i < val->Size(); i++) {
                r = val->getvalue(i);
                if (r->same(v) == aTRUE) {
                    r->Release();
                    res = context->PredicateEvalue(stack, currenthead, depth);
                    break;
                }
                r->Release();
            }
        }
        else {
            res = val->same(v);
            if (res == aTRUE)
                res = context->PredicateEvalue(stack, currenthead, depth);
        }
        
        val->Release();
        v->Release();
        for (i = 0; i < last; i++)
            args[i]->Release();
        return res;
    }

    TamguCallFromCall call(name);
    for (i = 1; i < last; i++)
        call.arguments.push_back(args[i]);

    Tamgu* prev = parameters[last]->Value();

    //if the value that is stored in the parameter is not empty
    if (prev != aNOELEMENT) {
        //then we push it into our new function
        call.arguments.push_back(prev);
        //but we remove it from the parameter value, to avoid its accidental destruction...
        parameters[last]->Setvalue(aNULL, aNOELEMENT, 0, true);
    }

    val = call.Eval(context->dom, args[0], context->threadowner);
    if (val->isError()) {
        globalTamgu->Cleanerror(context->threadowner);
        if (prev != aNOELEMENT)
            parameters[last]->Setvalue(aNULL, prev, 0, false);
        return aFALSE;
    }
    
    if (val == aNOELEMENT || val == aNULL) {
        if (prev != aNOELEMENT)
            parameters[last]->Setvalue(aNULL, prev, 0, false);
        return aFALSE;
    }
    
    if (val->isVectorContainer()) {
        for (i = 0; i < val->Size(); i++) {
            r = val->getvalue(i);
            parameters[last]->Newvalue(r, context->threadowner);
            r = context->PredicateEvalue(stack, currenthead, depth);
            if (r == aTRUE) {
                res = aTRUE;
                continue;
            }
            if (r == aCUT) {
                res = aTRUE;
                break;
            }
            if (r == aFALSE)
                break;
        }
    }
    else {
        parameters[last]->Newvalue(val, context->threadowner);
        res = context->PredicateEvalue(stack, currenthead, depth);
    }
    val->Release();

    for (i = 0; i < last; i++)
        args[i]->Release();
        
    parameters[last]->Setvalue(aNULL, prev, 0, false);
    return res;
}



Exporting TamguPredicate* TamguPredicateMethod::Duplicate(Tamgu* context, TamguDeclaration* dom, short idthread) {
    TamguPredicateMethod* p = new TamguPredicateMethod(globalTamgu, name, size);
    Tamgu* e;

    for (short i = 0; i < parameters.size(); i++) {
        e = parameters[i]->ExtractPredicateVariables(context, dom, NULL, NULL, idthread, true);
        if (e == NULL) {
            p->Release();
            return NULL;
        }
        p->parameters.push_back(e);
    }

    p->Setreference();
    return p;
}

//--------------------------------------------------------------------------------------------
void TamguGlobal::RecordPredicateFunctions() {
    gpredicatezone = Getid("&p_zone&");
    gpredicatedico = Getid("&p_dico&");
    gpredicatedependency = Getid("&p_dependency&");
    gpredicatefeature = Getid("&p_feature&");


    short n = Getid("between");
    predicates[n] = new TamguPredicateBetween(this, n);
    n = Getid("succ");
    predicates[n] = new TamguPredicateSucc(this, n);
    n = Getid("pred");
    predicates[n] = new TamguPredicatePred(this, n);

    bin_hash<unsigned long>::iterator it;
    string name;
    for (it = allmethods.begin(); it != allmethods.end(); it++) {
        name = "p_" + Getsymbol(it->first);
        n = Getid(name);
        predicates[n] = new TamguPredicateMethod(this, it->first, it->second);
    }
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
// Procedures that deal with predicates...
//----------------------------------------------------------------------------------------------------

Tamgu* ProcPredicateDump(Tamgu* context, short idthread, TamguCall* callfunc) {
    //We display all predicates or one...
    string label;
    
    Locking* _lock = _getlock(globalTamgu->_knowledgelock);
    
    hmap<short, vector<TamguPredicate*> >& knowledge = globalTamgu->knowledgebase;
    
    Tamguvector* kvect = globalTamgu->Providevector();
    if (callfunc->Size() == 0) {
        hmap<short, vector<TamguPredicate*> >::iterator it;
        for (it = knowledge.begin(); it != knowledge.end(); it++) {
            vector<TamguPredicate*>& vect = it->second;
            for (size_t i = 0; i < vect.size(); i++)
                kvect->Push(vect[i]);
        }
        _cleanlock(_lock);
        return kvect;
    }
    
    short name;
    Tamgu* val;
    for (long i = 1; i < callfunc->Size(); i++) {
        val = callfunc->Evaluate(i, context, idthread);
        if (val->isVectorContainer()) {
            for (long j = 0; j < val->Size(); j++) {
                label = val->getstring(j);
                name = globalTamgu->Getid(label);
                if (knowledge.find(name) == knowledge.end())
                    continue;

                vector<TamguPredicate*>& vect = knowledge[name];
                for (size_t k = 0; k < vect.size(); k++) {
                    kvect->Push(vect[k]);
                }
            }
        }
        else {
            label = val->String();
            name = globalTamgu->Getid(label);
            if (knowledge.find(name) == knowledge.end())
                continue;
            
            vector<TamguPredicate*>& vect = knowledge[name];
            for (size_t k = 0; k < vect.size(); k++) {
                kvect->Push(vect[k]);
            }
        }
    }
    
    _cleanlock(_lock);
    return kvect;
}

Tamgu* ProcPredicateStore(Tamgu* context, short idthread, TamguCall* callfunc) {
    //We display all predicates or one...
    string label;
    
    Locking* _lock = _getlock(globalTamgu->_knowledgelock);
    
    hmap<short, vector<TamguPredicate*> >& knowledge = globalTamgu->knowledgebase;
    
    string filename = callfunc->Evaluate(0, context, idthread)->String();
    
    ofstream file(filename, ofstream::binary);
    if (file.fail())
        return globalTamgu->Returnerror("Cannot open fail in 'predicatedump'", idthread);
    
    if (callfunc->Size() == 1) {
        hmap<short, vector<TamguPredicate*> >::iterator it;
        for (it = knowledge.begin(); it != knowledge.end(); it++) {
            vector<TamguPredicate*>& vect = it->second;
            for (size_t i = 0; i < vect.size(); i++)
                file << vect[i]->String() << "." << endl;
        }
        _cleanlock(_lock);
        file.close();
        return aTRUE;
    }
    
    short name;
    Tamgu* val;
    for (long i = 1; i < callfunc->Size(); i++) {
        val = callfunc->Evaluate(i, context, idthread);
        if (val->isVectorContainer()) {
            for (long j = 0; j < val->Size(); j++) {
                label = val->getstring(j);
                name = globalTamgu->Getid(label);
                if (knowledge.find(name) == knowledge.end())
                    continue;

                vector<TamguPredicate*>& vect = knowledge[name];
                for (size_t k = 0; k < vect.size(); k++) {
                    file << vect[k]->String() << "." << endl;
                }
            }
        }
        else {
            label = val->String();
            name = globalTamgu->Getid(label);
            if (knowledge.find(name) == knowledge.end())
                continue;
            
            vector<TamguPredicate*>& vect = knowledge[name];
            for (size_t k = 0; k < vect.size(); k++) {
                file << vect[k]->String() << "." << endl;
            }
        }
    }
    file.close();
    _cleanlock(_lock);
    return aTRUE;
}


void RemoveAPredicate(TamguPredicate* p, short idthread) {
    string argument_key;
    
    Locking* _lock = _getlock(globalTamgu->_knowledgelock);
    
    if (p->Stringpredicatekey(argument_key)) {
        vector<TamguPredicate*>& vect = globalTamgu->knowledgebase_on_first[argument_key];
        for (size_t i = 0; i < vect.size(); i++) {
            if (vect[i] == p) {
                vect.erase(vect.begin() + i);
                _cleanlock(_lock);
                return;
            }
        }
    }
    if (p->Stringpredicatekeysecond(argument_key)) {
        vector<TamguPredicate*>& vect = globalTamgu->knowledgebase_on_second[argument_key];
        for (size_t i = 0; i < vect.size(); i++) {
            if (vect[i] == p) {
                vect.erase(vect.begin() + i);
                _cleanlock(_lock);
                return;
            }
        }
    }

    if (p->Stringpredicatekeythird(argument_key)) {
        vector<TamguPredicate*>& vect = globalTamgu->knowledgebase_on_third[argument_key];
        for (size_t i = 0; i < vect.size(); i++) {
            if (vect[i] == p) {
                vect.erase(vect.begin() + i);
                _cleanlock(_lock);
                return;
            }
        }
    }

    _cleanlock(_lock);
    p->Resetreference();
}

Tamgu* ProcRetractAll(Tamgu* context, short idthread, TamguCall* callfunc) {
    //We display all predicates or one...
    Locking* _lock = _getlock(globalTamgu->_knowledgelock);
    
    hmap<short, vector<TamguPredicate*> >& knowledge = globalTamgu->knowledgebase;
    if (callfunc->Size() == 0) {
        hmap<short, vector<TamguPredicate*> >::iterator it;
        for (it = knowledge.begin(); it != knowledge.end(); it++) {
            vector<TamguPredicate*>& vect = it->second;
            for (size_t i = 0; i < vect.size(); i++)
                vect[i]->Resetreference();
            vect.clear();
        }
        globalTamgu->knowledgebase.clear();
        globalTamgu->knowledgebase_on_first.clear();
        globalTamgu->knowledgebase_on_second.clear();
        globalTamgu->knowledgebase_on_third.clear();
        _cleanlock(_lock);
        return aTRUE;
    }
    
    string label = callfunc->Evaluate(0, context, idthread)->String();
    short name = globalTamgu->Getid(label);
    if (knowledge.find(name) == knowledge.end()) {
        _cleanlock(_lock);
        return aFALSE;
    }
    vector<TamguPredicate*>& vect = knowledge[name];
    string keyfirst;
    for (size_t i = 0; i < vect.size(); i++)
        RemoveAPredicate(vect[i], idthread);
    vect.clear();
    
    _cleanlock(_lock);
    return aTRUE;
}

Tamgu* ProcPredicateRetract(Tamgu* context, short idthread, TamguCall* callfunc) {
    Tamgu* a = callfunc->arguments[0];
    if (!a->isPredicate()) {
        a = callfunc->Evaluate(0, context, idthread);
        if (!a->isPredicate())
            return globalTamgu->Returnerror(e_expecting_a_predicate, idthread);
    }
    TamguPredicate* head = (TamguPredicate*)a;
    long i;
    for (i = 0; i < head->parameters.size(); i++) {
        head->parameters[i] = head->parameters[i]->Eval(context, aNULL, idthread);
        head->parameters[i]->Setreference();
    }

    if (globalTamgu->RemovePredicates(aNULLDECLARATION, head))
        a = aTRUE;
    else
        a = aFALSE;

    for (i = 0; i < head->parameters.size(); i++)
        head->parameters[i]->Resetreference();

    return a;
}

Tamgu* ProcPredicateAsserta(Tamgu* context, short idthread, TamguCall* callfunc) {
    Tamgu* a = callfunc->arguments[0];
    if (!a->isPredicate()) {
        a = callfunc->Evaluate(0, context, idthread);
        if (!a->isPredicate())
            return globalTamgu->Returnerror(e_expecting_a_predicate, idthread);
    }
    TamguPredicate* head = (TamguPredicate*)a;
    long i;
    for (i = 0; i < head->parameters.size(); i++) {
        head->parameters[i] = head->parameters[i]->Eval(context, aNULL, idthread);
        head->parameters[i]->Setreference();
    }

    globalTamgu->StorePredicate(aNULLDECLARATION, head, false);

    for (i = 0; i < head->parameters.size(); i++)
        head->parameters[i]->Resetreference();
    return aTRUE;
}

Tamgu* ProcPredicateAssertz(Tamgu* context, short idthread, TamguCall* callfunc) {
    Tamgu* a = callfunc->arguments[0];
    if (!a->isPredicate()) {
        a = callfunc->Evaluate(0, context, idthread);
        if (!a->isPredicate())
            return globalTamgu->Returnerror(e_expecting_a_predicate, idthread);
    }
    TamguPredicate* head = (TamguPredicate*)a;
    long i;
    for (i = 0; i < head->parameters.size(); i++) {
        head->parameters[i] = head->parameters[i]->Eval(context, aNULL, idthread);
        head->parameters[i]->Setreference();
    }

    globalTamgu->StorePredicate(aNULLDECLARATION, head, true);

    for (i = 0; i < head->parameters.size(); i++)
        head->parameters[i]->Resetreference();
    return aTRUE;
}

Tamgu* ProcBound(Tamgu* context, short idthread, TamguCall* callfunc) {
    Tamgu* a = callfunc->arguments[0];
    return a->isBound(context, idthread);
}

Tamgu* ProcPredicateSucc(Tamgu* context, short idthread, TamguCall* callfunc) {
    return callfunc->Evaluate(0, context, idthread)->Succ();
}

Tamgu* ProcPredicatePred(Tamgu* context, short idthread, TamguCall* callfunc) {
    return callfunc->Evaluate(0, context, idthread)->Pred();
}


class PredicateFunctionEnvironment {
public:
    TamguDeclarationPredicate domain;
    PredicateInstanceDeclaration localdico;
    TamguPredicateFunction pv;
    TamguInstructionEvaluate kl;
    short idthread;
    
    PredicateFunctionEnvironment(short idt, bool trace) :
    pv(globalTamgu, globalTamgu->predicates[a_dependency]->Function(), a_dependency),
    kl(&pv, idt, trace, true),
    localdico(5) {
        pv.reference = 1;
        idthread = idt;

        //Important variables...
        globalTamgu->SetPredicateVariableFlags(idthread);
        kl.dico = &localdico;

        TamguPredicateContainer* kpcont = globalTamgu->GetPredicatecontainer(idthread);
        if (kpcont != NULL)
            kl.rules = kpcont->rules;
    }
    
    ~PredicateFunctionEnvironment() {
        basebin_hash<Tamgu*>::iterator it;
        for (it = domain.declarations.begin(); it != domain.declarations.end(); it++) {
            it->second->Resetreferencenoprotect();
        }

        for (long i = 0; i < pv.Size(); i++)
            pv.parameters[i]->Resetreference();
    }
    
    
   inline Tamgu* eval(Tamgu* context) {
       TamguPredicateVariable v1(NULL, globalTamgu->Getid("#1"));
       TamguPredicateVariable v2(NULL, globalTamgu->Getid("#2"));

       Tamgu* e = v1.ExtractPredicateVariables(&kl, &domain, NULL, NULL, idthread, true);
       e->Setreference();
       pv.parameters.push_back(e);

       e = v2.ExtractPredicateVariables(&kl, &domain, NULL, NULL, idthread, true);
       e->Setreference();
       pv.parameters.push_back(e);

       //We force in this case the full traversal of all rules...
       kl.fulltraversal = FULLSEARCH;

       return kl.Eval(context, &domain, idthread);
    }
};


Tamgu* ProcDependencies(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (globalTamgu->predicates.check(a_dependency) == false)
        return aNULL;

    bool trace = false;
    if (callfunc->Size())
        trace = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();

    std::unique_ptr<PredicateFunctionEnvironment> pfe(new PredicateFunctionEnvironment(idthread, trace));

    return pfe->eval(contextualpattern);
}

//----------------------------------------------------------------------------------------------------
void TamguGlobal::RecordPredicates() {
    if (aFAIL == NULL) {
        aFAIL = new TamguConstPredicate(NULL, a_fail);
        aTERMINAL = new TamguConstPredicate(NULL, a_terminal);
        aCUTFALSE = new TamguConstPredicate(NULL, a_cutfalse);
        aCUT = new TamguConstPredicate(NULL, a_cut);
        aSTOP = new TamguConstPredicate(NULL, a_stop);
        aPREDICATETRUE = new TamguConstPredicateTrue(a_true);
    }
    
    RecordOneProcedure("asserta", "Assert a clause (fact or rule) into the database as the first clause of the predicate", &ProcPredicateAsserta, P_ONE | P_ONE);
    RecordOneProcedure("assertz", "Assert a clause (fact or rule) into the database as the last clause of the predicate", &ProcPredicateAssertz, P_ONE | P_ONE);
    RecordOneProcedure("predicatedump", "predicatedump(string label1, string label2...): Return all predicates in the knowledge base that match the given pattern", &ProcPredicateDump, P_FULL);
    RecordOneProcedure("predicatestore", "predicatestore(string path, string label1, string label2...): Store all predicates in the knowledge base that match the given pattern. If 'label' is not provided then the whole database is stored.", &ProcPredicateStore, P_ATLEASTONE);
    RecordOneProcedure("retract", "Retract a clause (fact) from the database", &ProcPredicateRetract, P_ONE);
    RecordOneProcedure("retractall", "Retract all clauses (facts) from the database that match the given pattern", &ProcRetractAll, P_NONE | P_ONE);
    RecordOneProcedure("succ", "The next element", &ProcPredicateSucc, P_ONE);
    RecordOneProcedure("pred", "The previous element", &ProcPredicatePred, P_ONE);
    RecordOneProcedure("_dependencies", "Return the list of dependencies in memory", &ProcDependencies, P_NONE | P_ONE);
    RecordOneProcedure("_var", "Checks if a variable is bound.", &ProcBound, P_ONE);

    char buffer[20];
    for (long i = 0; i < 100; i++) {
        pvireservoire.push_back(new TamguPredicateVariableInstancebuff(i));
        sprintf_s(buffer, 20, "&p%ld&", i);
        pvireservoire.back()->name = Getid(buffer);
    }
    
    predicates[a_universal] = new TamguPredicateFunction(this, NULL, a_universal);
    pviidx = 0;
    
    char var_predicate_name[] = {'_','0', 0};
    for (char i = 48; i < 58; i++) {
        var_predicate_name[1] = i;
        Getid(var_predicate_name);
    }
}

/*
 class PredicateFunctionEnvironment {
 public:
     TamguDeclarationPredicate domain;
     PredicateInstanceDeclaration localdico;
     TamguPredicateFunction pv;
     TamguInstructionEvaluate kl;
     short idthread;
     
     PredicateFunctionEnvironment(short idt, bool trace) :
     pv(globalTamgu, globalTamgu->predicates[a_dependency]->Function(), a_dependency),
     kl(&pv, idt, trace, true),
     localdico(5) {
         pv.reference = 1;
         idthread = idt;

         //Important variables...
         globalTamgu->SetPredicateVariableFlags(idthread);
         kl.dico = &localdico;

         TamguPredicateContainer* kpcont = globalTamgu->GetPredicatecontainer(idthread);
         if (kpcont != NULL)
             kl.rules = kpcont->rules;
     }
     
     ~PredicateFunctionEnvironment() {
         basebin_hash<Tamgu*>::iterator it;
         for (it = domain.declarations.begin(); it != domain.declarations.end(); it++) {
             it->second->Resetreferencenoprotect();
         }

         for (long i = 0; i < pv.Size(); i++)
             pv.parameters[i]->Resetreference();
     }
     
     
    inline Tamgu* eval(Tamgu* context) {
        TamguPredicateVariable v1(NULL, globalTamgu->Getid("#1"));
        TamguPredicateVariable v2(NULL, globalTamgu->Getid("#2"));

        Tamgu* e = v1.ExtractPredicateVariables(&kl, &domain, NULL, NULL, idthread, true);
        e->Setreference();
        pv.parameters.push_back(e);

        e = v2.ExtractPredicateVariables(&kl, &domain, NULL, NULL, idthread, true);
        e->Setreference();
        pv.parameters.push_back(e);

        //We force in this case the full traversal of all rules...
        kl.fulltraversal = FULLSEARCH;

        return kl.Eval(context, &domain, idthread);
     }
 };

 Tamgu* ProcDependencies(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
     if (globalTamgu->predicates.check(a_dependency) == false)
         return aNULL;

     std::unique_ptr<PredicateFunctionEnvironment> pfe(new PredicateFunctionEnvironment(idthread, callfunc->Evaluate(0, contextualpattern, idthread)->Boolean()));

     return pfe->eval(contextualpattern);
 }

 */
