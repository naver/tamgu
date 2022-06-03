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
Exporting TamguPredicateVariableInstance* TamguGlobal::Providepvi(short n) {
    if (threadMODE || Addtogarbage())
        return new TamguPredicateVariableInstance(predicatename++, n);
    
    TamguPredicateVariableInstance* kvi;
    if (pviempties.last > 0) {
        kvi = pvireservoire[pviempties.backpop()];
        kvi->labelname = n;
        kvi->value = aNOELEMENT;
        kvi->name = predicatename++;
        kvi->used = true;
        return kvi;
    }
    
    long mx = pvireservoire.size();
    
    while (pviidx < mx) {
        if (!pvireservoire[pviidx]->used) {
            kvi = pvireservoire[pviidx++];
            kvi->labelname = n;
            kvi->value = aNOELEMENT;
            kvi->name = predicatename++;
            kvi->used = true;
            return kvi;
        }
        pviidx++;
    }
    
    long sz = mx >> 2;
    pvireservoire.resize(mx + sz);
    pviidx = mx + sz;
    for (long i = mx; i < pviidx; i++)
        pvireservoire[i] = new TamguPredicateVariableInstance(i);
    
    pviidx = mx;
    kvi = pvireservoire[pviidx++];
    kvi->labelname = n;
    kvi->value = aNOELEMENT;
    kvi->name = predicatename++;
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
    short last = parameters.size() - 1;
    for (short i = 0; i < last; i++) {
        if (!parameters[i]->isUnified(dom))
            return false;
    }

    return true;
}

Exporting Tamgu* TamguPredicateMethod::PredicateEvalue(TamguInstructionEvaluate* context, VECTE<Tamgu*>& stack, TamguPredicate* currenthead, long depth) {
    vector<Tamgu*> args;
    short last = parameters.size() - 1;
    short i;
    for (i = 0; i < last; i++) {
        if (parameters[i]->isUnified(context->dom))
            args.push_back(parameters[i]->Eval(context, context->dom, context->threadowner));
    }

    Tamgu* res = aFALSE;
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

        res = call.Eval(context->dom, args[0], context->threadowner);
        if (res->isVectorContainer()) {
            bool found = false;
            for (i = 0; i < res->Size(); i++) {
                r = res->getvalue(i);
                if (r->same(v) == aTRUE) {
                    found = true;
                    res->Release();
                    if (negation)
                        res = aFALSE;
                    else
                        res = context->PredicateEvalue(stack, currenthead, depth);
                    break;
                }
            }

            if (!found) {
                res->Release();
                if (negation)
                    res = aTRUE;
                else
                    res = aFALSE;
            }
        }
        else {
            if (res->same(v) == aTRUE) {
                res->Release();
                if (negation)
                    res = aFALSE;
                else
                    res = context->PredicateEvalue(stack, currenthead, depth);
            }
            else {
                if (res->isError())
                    globalTamgu->Cleanerror(context->threadowner);
                res->Release();
                res = aFALSE;
            }
        }

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

    r = call.Eval(context->dom, args[0], context->threadowner);

    if (r != aNOELEMENT && r != aNULL) {
        if (!negation) {
            long iter = 0;
            long mx = 1;

            if (r->isVectorContainer()) {
                v = aNOELEMENT;
                mx = r->Size();
            }
            else
                v = r;

            while (iter < mx) {
                if (r->isVectorContainer())
                    v = r->getvalue(iter);

                parameters[last]->Newvalue(v, context->threadowner);

                v = context->PredicateEvalue(stack, currenthead, depth);

                if (v == aCUT) {
                    res = aTRUE;
                    break;
                }

                if (v->isError()) {
                    globalTamgu->Cleanerror(context->threadowner);
                    res = aFALSE;
                    break;
                }

                if (v == aTRUE)
                    res = aTRUE;

                iter++;
            }
        }
        r->Release();
    }
    else {
        if (negation)
            res = aTRUE;
    }

    for (i = 0; i < last; i++)
        args[i]->Release();
        
    //we put back our initial value...
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

    gpredicatename = 0;
    gpredicatedico = 0;
    gpredicatezone = 0;
    gpredicatedependency = 0;
    gpredicatefeature = 0;

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
    hmap<short, vector<TamguPredicate*> >& knowledge = globalTamgu->threads[idthread].knowledgebase;
    Tamguvector* kvect = globalTamgu->Providevector();
    if (callfunc->Size() == 0) {

        hmap<short, vector<TamguPredicate*> >::iterator it;
        for (it = knowledge.begin(); it != knowledge.end(); it++) {
            vector<TamguPredicate*>& vect = it->second;
            for (size_t i = 0; i < vect.size(); i++)
                kvect->Push(vect[i]);
        }
        return kvect;
    }
    label = callfunc->Evaluate(0, context, idthread)->String();
    short name = globalTamgu->Getid(label);
    if (knowledge.find(name) == knowledge.end())
        return kvect;
    vector<TamguPredicate*>& vect = knowledge[name];
    for (size_t i = 0; i < vect.size(); i++)
        kvect->Push(vect[i]);
    return kvect;
}

void RemoveAPredicate(TamguPredicate* p, short idthread) {
    string argument_key;
    if (p->Stringpredicatekey(argument_key)) {
        vector<TamguPredicate*>& vect = globalTamgu->threads[idthread].knowledgebase_on_first[argument_key];
        for (size_t i = 0; i < vect.size(); i++) {
            if (vect[i] == p) {
                vect.erase(vect.begin() + i);
                return;
            }
        }
    }
    if (p->Stringpredicatekeysecond(argument_key)) {
        vector<TamguPredicate*>& vect = globalTamgu->threads[idthread].knowledgebase_on_second[argument_key];
        for (size_t i = 0; i < vect.size(); i++) {
            if (vect[i] == p) {
                vect.erase(vect.begin() + i);
                return;
            }
        }
    }
    p->Resetreference();
}

Tamgu* ProcRetractAll(Tamgu* context, short idthread, TamguCall* callfunc) {
    //We display all predicates or one...
    hmap<short, vector<TamguPredicate*> >& knowledge = globalTamgu->threads[idthread].knowledgebase;
    if (callfunc->Size() == 0) {

        hmap<short, vector<TamguPredicate*> >::iterator it;
        for (it = knowledge.begin(); it != knowledge.end(); it++) {
            vector<TamguPredicate*>& vect = it->second;
            for (size_t i = 0; i < vect.size(); i++)
                vect[i]->Resetreference();
            vect.clear();
        }
        globalTamgu->threads[idthread].knowledgebase.clear();
        globalTamgu->threads[idthread].knowledgebase_on_first.clear();
        globalTamgu->threads[idthread].knowledgebase_on_second.clear();
        return aTRUE;
    }
    
    string label = callfunc->Evaluate(0, context, idthread)->String();
    short name = globalTamgu->Getid(label);
    if (knowledge.find(name) == knowledge.end())
        return aFALSE;
    vector<TamguPredicate*>& vect = knowledge[name];
    string keyfirst;
    for (size_t i = 0; i < vect.size(); i++)
        RemoveAPredicate(vect[i], idthread);
    vect.clear();
    return aTRUE;
}

Tamgu* ProcPredicateRetract(Tamgu* context, short idthread, TamguCall* callfunc) {
    Tamgu* a = callfunc->arguments[0];
    if (!a->isPredicate()) {
        a = callfunc->Evaluate(0, context, idthread);
        if (!a->isPredicate())
            return globalTamgu->Returnerror("Expecting a predicate as argument", idthread);
    }
    TamguPredicate* head = (TamguPredicate*)a;
    long i;
    for (i = 0; i < head->parameters.size(); i++) {
        head->parameters[i] = head->parameters[i]->Eval(context, aNULL, idthread);
        head->parameters[i]->Setreference();
    }

    if (globalTamgu->RemovePredicates(aNULLDECLARATION, head, idthread))
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
            return globalTamgu->Returnerror("Expecting a predicate as argument", idthread);
    }
    TamguPredicate* head = (TamguPredicate*)a;
    long i;
    for (i = 0; i < head->parameters.size(); i++) {
        head->parameters[i] = head->parameters[i]->Eval(context, aNULL, idthread);
        head->parameters[i]->Setreference();
    }

    globalTamgu->StorePredicate(aNULLDECLARATION, head, false, idthread);

    for (i = 0; i < head->parameters.size(); i++)
        head->parameters[i]->Resetreference();
    return aTRUE;
}

Tamgu* ProcPredicateAssertz(Tamgu* context, short idthread, TamguCall* callfunc) {
    Tamgu* a = callfunc->arguments[0];
    if (!a->isPredicate()) {
        a = callfunc->Evaluate(0, context, idthread);
        if (!a->isPredicate())
            return globalTamgu->Returnerror("Expecting a predicate as argument", idthread);
    }
    TamguPredicate* head = (TamguPredicate*)a;
    long i;
    for (i = 0; i < head->parameters.size(); i++) {
        head->parameters[i] = head->parameters[i]->Eval(context, aNULL, idthread);
        head->parameters[i]->Setreference();
    }

    globalTamgu->StorePredicate(aNULLDECLARATION, head, true, idthread);

    for (i = 0; i < head->parameters.size(); i++)
        head->parameters[i]->Resetreference();
    return aTRUE;
}

Tamgu* ProcPredicateSucc(Tamgu* context, short idthread, TamguCall* callfunc) {
    return callfunc->Evaluate(0, context, idthread)->Succ();
}

Tamgu* ProcPredicatePred(Tamgu* context, short idthread, TamguCall* callfunc) {
    return callfunc->Evaluate(0, context, idthread)->Pred();
}


Tamgu* ProcDependencies(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    int i;
    
    bool trace = false;
    if (callfunc->Size())
        trace = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();

    if (globalTamgu->predicates.check(a_dependency) == false)
        return aNULL;

    TamguPredicateFunction pv(globalTamgu, globalTamgu->predicates[a_dependency]->Function(), a_dependency);

    TamguInstructionEvaluate kl(globalTamgu, &pv);
    kl.threadowner = idthread;

    kl.trace = trace;

    //Important variables...
    globalTamgu->SetPredicateVariableFlags();

    TamguDeclarationPredicate domain;

    basebin_hash<TamguPredicateVariableInstance*> localdico;
    kl.dico = &localdico;

    TamguPredicateContainer* kpcont = globalTamgu->GetPredicatecontainer(idthread);

    if (kpcont != NULL)
        kl.rules = kpcont->rules;

    Tamgu* e;
    TamguPredicateVariable v1(NULL, globalTamgu->Getid("#1"));
    TamguPredicateVariable v2(NULL, globalTamgu->Getid("#2"));

    e = v1.ExtractPredicateVariables(&kl, &domain, NULL, NULL, idthread, true);
    e->Setreference();
    pv.parameters.push_back(e);

    e = v2.ExtractPredicateVariables(&kl, &domain, NULL, NULL, idthread, true);
    e->Setreference();
    pv.parameters.push_back(e);

    //We force in this case the full traversal of all rules...
    kl.fulltraversal = FULLSEARCH;

    e = kl.Eval(contextualpattern, &domain, idthread);

    for (i = 0; i < pv.Size(); i++)
        pv.parameters[i]->Resetreference();

    basebin_hash<Tamgu*>::iterator it;

    for (it = domain.declarations.begin(); it != domain.declarations.end(); it++) {
        it->second->Setprotect(0);
        it->second->Resetreference();
    }

    return e;
}


//----------------------------------------------------------------------------------------------------
void TamguGlobal::RecordPredicates() {
    if (aFAIL == NULL) {
        aFAIL = new TamguConstPredicate(NULL, a_fail);
        aCUT = new TamguConstPredicate(NULL, a_cut);
        aSTOP = new TamguConstPredicate(NULL, a_stop);
    }
    
    RecordOneProcedure("asserta", &ProcPredicateAsserta, P_ONE | P_ONE);
    RecordOneProcedure("assertz", &ProcPredicateAssertz, P_ONE | P_ONE);
    RecordOneProcedure("predicatedump", &ProcPredicateDump, P_NONE | P_ONE);
    RecordOneProcedure("findall", &ProcPredicateDump, P_NONE | P_ONE);
    RecordOneProcedure("retract", &ProcPredicateRetract, P_ONE);
    RecordOneProcedure("retractall", &ProcRetractAll, P_NONE | P_ONE);
    RecordOneProcedure("succ", &ProcPredicateSucc, P_ONE);
    RecordOneProcedure("pred", &ProcPredicatePred, P_ONE);
    RecordOneProcedure("_dependencies", &ProcDependencies, P_NONE | P_ONE);

    for (long i = 0; i < 1000; i++)
        pvireservoire.push_back(new TamguPredicateVariableInstance(i));
    
    predicates[a_universal] = new TamguPredicateFunction(this, NULL, a_universal);
    pviidx = 0;
    
    char var_predicate_name[] = {'_','0', 0};
    for (char i = 48; i < 58; i++) {
        var_predicate_name[1] = i;
        Getid(var_predicate_name);
    }
}
