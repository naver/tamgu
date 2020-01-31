/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : lisp.cxx
 Date       :
 Purpose    :
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#include "tamgu.h"
#include "tamguglobal.h"
#include "compilecode.h"
#include "tamguversion.h"
#include "tamgulisp.h"
#include "instructions.h"

//We need to declare once again our local definitions.
#define checkerror(a) if (a->isError()) return a
#define checkerrorwithrelease(a,v) if (a->isError()) {v->Releasenonconst(); return a;}
//------------------------------------------------------------------------------------------------------------------
Exporting basebin_hash<lispMethod>  Tamgulisp::methods;
Exporting hmap<string, string> Tamgulisp::infomethods;
Exporting bin_hash<unsigned long> Tamgulisp::exported;

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgulisp::AddMethod(TamguGlobal* global, string name, lispMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Tamgulisp::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    Tamgulisp::idtype = a_lisp;

    Tamgulisp::AddMethod(global, "eval", &Tamgulisp::MethodEval, P_ONE, "eval(e): 'e' is a lisp expression provided as a string.");

    global->newInstance[a_lisp] = new Tamgulispcode(global);
    global->RecordMethods(a_lisp, Tamgulisp::exported);

    global->lispactions[a_block] = true;

    global->lispactions[a_if] = true;
    global->lispactions[a_booleanxor] = true;
    global->lispactions[a_booleanor] = true;
    global->lispactions[a_booleanand] = true;
    global->lispactions[a_negation] = true;
    
    global->lispactions[a_for] = true;
    global->lispactions[a_return] = true;
    global->lispactions[a_lambda] = true;
    global->lispactions[a_defun] = true;
    global->lispactions[a_while] = true;
    global->lispactions[a_break] = true;

    
    global->lispactions[a_list] = true;
    global->lispactions[a_append] = true;
    global->lispactions[a_same] = true;
    global->lispactions[a_different] = true;
    global->lispactions[a_less] = true;
    global->lispactions[a_more] = true;
    global->lispactions[a_lessequal] = true;
    global->lispactions[a_moreequal] = true;
    
    global->lispactions[a_or] = true;
    global->lispactions[a_xor] = true;
    global->lispactions[a_and] = true;
    global->lispactions[a_plus] = true;
    global->lispactions[a_minus] = true;
    global->lispactions[a_multiply] = true;
    global->lispactions[a_divide] = true;
    global->lispactions[a_power] = true;
    global->lispactions[a_mod] = true;
    global->lispactions[a_shiftleft] = true;
    global->lispactions[a_shiftright] = true;
    
    global->lispactions[a_setq] = true;
    global->lispactions[a_quote] = true;
    global->lispactions[a_cons] = true;
    global->lispactions[a_cond] = true;
    global->lispactions[a_self] = true;
    global->lispactions[a_atom] = true;
    global->lispactions[a_eq] = true;
    global->lispactions[a_cadr] = true;
    global->lispactions[a_label] = true;
    global->lispactions[a_atomp] = true;
    global->lispactions[a_numberp] = true;
    global->lispactions[a_consp] = true;
    global->lispactions[a_zerop] = true;
    global->lispactions[a_nullp] = true;
    global->lispactions[a_lisp] = true;

    return true;
}

Tamgulispcode::Tamgulispcode(TamguGlobal* g, Tamgu* parent) : Tamgulisp(g, parent) {
    if (g != NULL) {
        long line = g->Getcurrentline();
        short idcode = (short)g->spaceid;
        short idfile = -1;
        if (idcode != -1)
            idfile = g->spaces[idcode]->currentfileid;
        idinfo = g->Addinfo(idcode, idfile, line);
    }
}

Tamgu* TamguGlobal::Providelispsymbols(string& n, Tamgu* parent) {
    short symb = Getid(n);
    
    if (!lispsymbols.check(symb))
        lispsymbols[symb] = new Tamgusymbol(symb, this);
    
    if (parent != NULL)
        parent->AddInstruction(lispsymbols[symb]);
    
    return lispsymbols[symb];
}

Tamgu* TamguGlobal::Providelispsymbols(short symb) {
    if (!lispsymbols.check(symb))
        lispsymbols[symb] = new Tamgusymbol(symb, this);
    return lispsymbols[symb];
}

Tamgu* TamguGlobal::Providelispoperators(short op) {
    if (!lispoperators.check(op))
        lispoperators[op] = new Tamguoperator(op, this);
    return lispoperators[op];
}

Tamgu* Tamgusymbol::Eval(Tamgu* a, Tamgu* v, short idthread) {
    if (globalTamgu->isDeclared(name, idthread))
        return globalTamgu->Getdeclaration(name, idthread);
    
    switch (name) {
        case a_true:
            return aTRUE;
        case a_false:
            return aFALSE;
        case a_null:
            return aNULL;
        case a_empty:
            return aNULL;
        default:
            if (globalTamgu->systems.check(name))
                return globalTamgu->systems[name];
    }
    
    string s("Unknown symbol: ");
    s += globalTamgu->Getsymbol(name);
    return globalTamgu->Returnerror(s, idthread);
}

Tamgu* TamguLockContainer::car(short idthread) {
    if (isVectorContainer()) {
        Locking _lock(this);
        return getvalue(0)->Atom();
    }
    return aNOELEMENT;
}

Tamgu* TamguLockContainer::cdr(short idthread) {
    if (isVectorContainer()) {
        long sz = Size();
        if (sz == 0)
            return aNOELEMENT;
        
        Tamgulisp* lp = new Tamgulisp;
        for (long i = 1; i < sz; i++)
            lp->Push(getvalue(i));
        return lp;
    }
    return aNOELEMENT;
}

Tamgu* Tamguvector::cdr(short idthread) {
    Locking _lock(this);
    long sz = values.size();
    if (sz == 0)
        return aNOELEMENT;
    
    Tamgulisp* lp = new Tamgulisp;
    for (long i = 1; i < sz; i++)
        lp->Push(values[i]);
    return lp;
}

Tamgu* Tamgulisp::MethodEval(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* a =  callfunc->Evaluate(0, contextualpattern, idthread);
    if (a->isLisp())
        return a->Eval(contextualpattern, aNULL, idthread);
    
    string s = a->String();
    return globalTamgu->EvaluateLisp(contextualpattern, s, idthread);
}
    //For once it is the original historical Eval...
Tamgu* Tamgulisp::Eval(Tamgu* contextualpattern, Tamgu* v0, short idthread) {
        //We need to take into account the different case...
    long sz = values.size();
    if (sz == 0)
        return this;
    
    if (globalTamgu->Error(idthread))
        return globalTamgu->Errorobject(idthread);
    
    globalTamgu->Current(this, idthread);
    
    Tamgu* a = values[0];
    long i, n;
    Tamgu* v1;
    char ret;
    
    switch (a->Action()) {
        case a_break:
            return aBREAK;
        case a_return:
            if (sz == 3) {
                if (values[2]->isReturned()) {
                    v1 = values[1]->Eval(contextualpattern, aNULL, idthread);
                    values[2]->Put(contextualpattern, v1, idthread);
                    return values[2];
                }
                else
                    return globalTamgu->Returnerror("Wrong number of arguments in 'return'", idthread);
            }
            
            if (sz != 2)
                return globalTamgu->Returnerror("Wrong number of arguments in 'return'", idthread);
            
            v1 = values[1]->Eval(contextualpattern, aNULL, idthread);
            v0 = new TamguCallReturn(globalTamgu);
            values.push_back(v0);
            v0->Put(contextualpattern,v1, idthread);
            return v0;
        case a_for: //(for x v (block...))
        {
            n = values[1]->Name();

            if (sz >= 2 && values[2]->isInstruction()) {
                if (globalTamgu->isDeclared(n, idthread)) {
                    v0 = globalTamgu->Getdeclaration(n, idthread);
                    values[2]->Instruction(0)->Putinstruction(0,v0);
                }

                return values[2]->Eval(contextualpattern, aNULL, idthread)->Returned(idthread);
            }
            
            if (sz < 3)
                return globalTamgu->Returnerror("Wrong number of arguments in 'for'", idthread);

            TamguInstructionFORINVECTOR* forin = new TamguInstructionFORINVECTOR(globalTamgu);
            forin->variablesWillBeCreated = true;

            if (globalTamgu->isDeclared(n, idthread)) {
                v0 = globalTamgu->Getdeclaration(n, idthread);
                n = 0;
            }
            else
                v0 = new TamguSelfVariableDeclaration(NULL, n);

            TamguInstruction* local = new TamguInstruction(globalTamgu);
            forin->instructions.push_back(local);

            local->instructions.push_back(v0);
            local->instructions.push_back(values[2]);
            if (sz == 4)
                forin->instructions.push_back(values[3]);
            else {
                Tamgulisp* block = new Tamgulispcode(globalTamgu);
                block->values.push_back(globalTamgu->Providelispsymbols(a_block));
                forin->instructions.push_back(block);
                for (i = 3; i < sz; i++)
                    block->values.push_back(values[i]);
            }
            
            values[2] = forin;
            return forin->Eval(contextualpattern,aNULL, idthread)->Returned(idthread);
        }
        case a_while: //while condition block)
        {
            if (sz >= 2 && values[1]->isInstruction())
                return values[1]->Eval(contextualpattern, aNULL, idthread)->Returned(idthread);
            
            if (sz < 3)
                return globalTamgu->Returnerror("Wrong number of arguments in 'while'", idthread);

            TamguInstructionWHILE* awhile = new TamguInstructionWHILE(globalTamgu);
            awhile->instructions.push_back(values[1]); // the condition
            if (sz == 3)
                awhile->instructions.push_back(values[2]);
            else {
                Tamgulisp* block = new Tamgulispcode(globalTamgu);
                block->values.push_back(globalTamgu->Providelispsymbols(a_block));
                awhile->instructions.push_back(block);
                for (i = 2; i < sz; i++)
                    block->values.push_back(values[i]);
            }
            values[1] = awhile;
            return awhile->Eval(contextualpattern, aNULL, idthread)->Returned(idthread);
        }
        case a_booleanor:
        {
            v1 = aNULL;
            for (i = 1; i < sz && !v1->isError(); i++) {
                v1->Releasenonconst();
                v1 = values[i]->Eval(contextualpattern, aNULL, idthread);
                //checkerror(v1);
                if (v1->Boolean()) {
                    v1->Releasenonconst();
                    return aTRUE;
                }
            }
            if (v1->isError())
                return v1;
            v1->Releasenonconst();
            return aFALSE;
        }
        case a_booleanand:
        {
            v1 = aNULL;
            for (i = 1; i < sz && !v1->isError(); i++) {
                v1->Releasenonconst();
                v1 = values[i]->Eval(contextualpattern, aNULL, idthread);
                //checkerror(v1);
                if (!v1->Boolean()) {
                    v1->Releasenonconst();
                    return aFALSE;
                }
            }
            if (v1->isError())
                return v1;
            v1->Releasenonconst();
            return aTRUE;
        }
        case a_booleanxor:
            if (sz != 3)
                return globalTamgu->Returnerror("Wrong number of arguments in 'xor'", idthread);
            
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            //checkerror(v0);
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            //checkerror(v1);
            
            if (v0->Boolean() == v1->Boolean())
                a = aFALSE;
            else
                a = aTRUE;
            
            v0->Releasenonconst();
            v1->Releasenonconst();
            return a;
        case a_negation:
            if (sz != 2)
                return globalTamgu->Returnerror("Wrong number of arguments in 'not'", idthread);
            v1 = values[1]->Eval(contextualpattern, aNULL, idthread);
            //checkerror(v1);
            if (v1 == aTRUE)
                return aFALSE;
            else
                return aTRUE;
        case a_list:
        {
            Tamgulisp* tl = new Tamgulisp;
            if (sz == 1)
                return tl;
            v1 = aNULL;
            for (i = 1; i < sz && !v1->isError(); i++) {
                v1 = values[i]->Eval(contextualpattern, aNULL, idthread);
                tl->Push(v1);
            }
            
            if (v1->isError()) {
                tl->Release();
                return v1;
            }
            return tl;
        }
        case a_same:
            if (sz != 3)
                return globalTamgu->Returnerror("Wrong number of arguments in '=='", idthread);
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerrorwithrelease(v1, v0);
            a = v0->same(v1);
            v0->Releasenonconst();
            v1->Releasenonconst();
            return a;
        case a_different:
            if (sz != 3)
                return globalTamgu->Returnerror("Wrong number of arguments in '!='", idthread);
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerrorwithrelease(v1, v0);
            a = v0->different(v1);
            v0->Releasenonconst();
            v1->Releasenonconst();
            return a;
        case a_less:
            if (sz != 3)
                return globalTamgu->Returnerror("Wrong number of arguments in '<'", idthread);
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerrorwithrelease(v1, v0);
            a = v0->less(v1);
            v0->Releasenonconst();
            v1->Releasenonconst();
            return a;
        case a_more:
            if (sz != 3)
                return globalTamgu->Returnerror("Wrong number of arguments in '>'", idthread);
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerrorwithrelease(v1, v0);
            a = v0->more(v1);
            v0->Releasenonconst();
            v1->Releasenonconst();
            return a;
        case a_lessequal:
            if (sz != 3)
                return globalTamgu->Returnerror("Wrong number of arguments in '<='", idthread);
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerrorwithrelease(v1, v0);
            a = v0->lessequal(v1);
            v0->Releasenonconst();
            v1->Releasenonconst();
            return a;
        case a_moreequal:
            if (sz != 3)
                return globalTamgu->Returnerror("Wrong number of arguments in '>='", idthread);
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerrorwithrelease(v1, v0);
            a = v0->moreequal(v1);
            v0->Releasenonconst();
            v1->Releasenonconst();
            return a;
        case a_plus:
            if (sz < 3)
                return globalTamgu->Returnerror("Wrong number of arguments in '+'", idthread);
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            v0 = v0->Atom();
            for (i = 2; i < sz ; i++) {
                v1 = values[i]->Eval(contextualpattern, aNULL, idthread);
                checkerrorwithrelease(v1, v0);
                v0->plus(v1,true);
                v1->Releasenonconst();
            }
            return v0;
        case a_minus:
            if (sz < 3)
                return globalTamgu->Returnerror("Wrong number of arguments in '-'", idthread);
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            v0 = v0->Atom();
            for (i = 2; i < sz; i++) {
                v1 = values[i]->Eval(contextualpattern, aNULL, idthread);
                checkerrorwithrelease(v1, v0);
                v0->minus(v1,true);
                v1->Releasenonconst();
            }
            return v0;
        case a_multiply:
            if (sz < 3)
                return globalTamgu->Returnerror("Wrong number of arguments in '*'", idthread);
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            v0 = v0->Atom();
            for (i = 2; i < sz; i++) {
                v1 = values[i]->Eval(contextualpattern, aNULL, idthread);
                checkerrorwithrelease(v1, v0);
                v0->multiply(v1,true);
                v1->Releasenonconst();
            }
            return v0;
        case a_divide:
            if (sz != 3)
                return globalTamgu->Returnerror("Wrong number of arguments in '/'", idthread);
            a = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(a);
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerrorwithrelease(v1, a);
            v0 = a->Atom();
            v0 = v0->divide(v1,true);
            v1->Releasenonconst();
            if (v0->isError())
                a->Releasenonconst();
            return v0;
        case a_power:
            if (sz != 3)
                return globalTamgu->Returnerror("Wrong number of argument", idthread);
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerrorwithrelease(v1, v0);
            v0 = v0->Atom();
            v0->power(v1,true);
            v1->Releasenonconst();
            return v0;
        case a_mod:
            if (sz != 3)
                return globalTamgu->Returnerror("Wrong number of arguments in '%'", idthread);
            a = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(a);
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerrorwithrelease(v1, a);
            v0 = a->Atom();
            v0 = v0->mod(v1,true);
            v1->Releasenonconst();
            if (v0->isError())
                a->Releasenonconst();
            return v0;
        case a_shiftleft:
            if (sz != 3)
                return globalTamgu->Returnerror("Wrong number of arguments in '<<'", idthread);
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerrorwithrelease(v1, v0);
            v0 = v0->Atom();
            v0 = v0->shiftleft(v1,true);
            v1->Releasenonconst();
            return v0;
        case a_shiftright:
            if (sz != 3)
                return globalTamgu->Returnerror("Wrong number of arguments in '>>'", idthread);
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerrorwithrelease(v1, v0);
            v0 = v0->Atom();
            v0 = v0->shiftright(v1,true);
            v1->Releasenonconst();
            return v0;
        case a_and:
            if (sz < 3)
                return globalTamgu->Returnerror("Wrong number of arguments in '&'", idthread);
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            v0 = v0->Atom();
            for (i = 2; i < sz; i++) {
                v1 = values[i]->Eval(contextualpattern, aNULL, idthread);
                checkerrorwithrelease(v1, v0);
                v0->andset(v1,true);
                v1->Releasenonconst();
            }
            return v0;
        case a_xor:
            if (sz < 3)
                return globalTamgu->Returnerror("Wrong number of arguments in '^'", idthread);
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            v0 = v0->Atom();
            for (i = 2; i < sz; i++) {
                v1 = values[i]->Eval(contextualpattern, aNULL, idthread);
                checkerrorwithrelease(v1, v0);
                v0->xorset(v1,true);
                v1->Releasenonconst();
            }
            return v0;
        case a_or:
            if (sz < 3)
                return globalTamgu->Returnerror("Wrong number of arguments in '|'", idthread);
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            v0 = v0->Atom();
            for (i = 2; i < sz; i++) {
                v1 = values[i]->Eval(contextualpattern, aNULL, idthread);
                checkerrorwithrelease(v1, v0);
                v0->orset(v1,true);
                v1->Releasenonconst();
            }
            return v0;
        case a_if:
            if (sz != 3 && sz != 4)
                return globalTamgu->Returnerror("Wrong number of arguments in 'if'", idthread);
                //Comparison first element
            a = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(a);
            if (a == aTRUE)
                return values[2]->Eval(contextualpattern, aNULL, idthread);
            if (sz == 4)
                return values[3]->Eval(contextualpattern, aNULL, idthread);
            return aNULL;
        case a_quote:
            if (sz != 2)
                return aNULL;
            return values[1];
        case a_cons:
        {
            //merging an element into the next list
            if (sz != 3)
                return globalTamgu->Returnerror("Wrong number of arguments in 'cons'", idthread);
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerrorwithrelease(v1, v0);
            
            //For functions, Lispbody will return their body, otherwise it does nothing
            v0 = v0->Lispbody();
            v1 = v1->Lispbody();
            
            if (!v1->isLisp() && !v1->isVectorContainer())
                return aNULL;
            
            Tamgulisp* tl = new Tamgulisp;
            tl->Push(v0);
            for (i = 0; i < v1->Size(); i++)
                tl->Push(v1->getvalue(i));
            v0->Releasenonconst();
            v1->Releasenonconst();
            return tl;
        }
        case a_cond:
            if (sz == 1)
                return aNULL;
            for (i = 1; i < sz; i++) {
                v1 = values[i];
                if (!v1->isLisp() || v1->Size() != 2)
                    return globalTamgu->Returnerror("Wrong cond element", idthread);
                a = v1->getvalue(0)->Eval(contextualpattern, aNULL, idthread);
                checkerror(a);
                if (a == aTRUE)
                    return v1->getvalue(1)->Eval(contextualpattern, aNULL, idthread);
            }
            return aNULL;
        case a_self:
            //We recall our top function
            v0 = globalTamgu->Topstacklisp(idthread);
            if (v0 != NULL) {
                TamguCallFunction call(v0);
                for (i = 1; i < sz; i++)
                    call.arguments.push_back(values[i]);
                return call.Eval(contextualpattern, aNULL, idthread);
            }
            return globalTamgu->Returnerror("Cannot call 'self'", idthread);
        case a_atom:
        {
                //it has to ba a function call
            n = a->Name();
            
            v0 = NULL;
            if (globalTamgu->isDeclared(n, idthread))
                v0 = globalTamgu->Getdeclaration(n, idthread);
            else
                v0 = globalTamgu->Declaration(n, idthread);

            if (v0 != NULL) {
                if (v0->isFunction()) {
                    if (!globalTamgu->Pushstacklisp(v0, idthread))
                        return globalTamgu->Returnerror("Stack overflow", idthread);
                    
                    TamguCallFunction call(v0);
                    
                    for (i = 1; i < sz; i++)
                        call.arguments.push_back(values[i]);
                    
                    a = call.Eval(contextualpattern, aNULL, idthread);
                    globalTamgu->Popstacklisp(idthread);
                    return a;
                }
                return aNULL;
            }
            
            if (globalTamgu->procedures.check(n)) {
                TamguCallProcedure call(n);
                for (i = 1; i < sz; i++)
                    call.arguments.push_back(values[i]);
                
                return call.Eval(contextualpattern, aNULL, idthread);
            }

            if (globalTamgu->commons.check(n)) {
                if (sz < 2)
                    return globalTamgu->Returnerror("Wrong number of arguments in 'call method'", idthread);

                TamguCallCommonMethod call(n);
                for (i = 2; i < sz; i++)
                    call.arguments.push_back(values[i]);
                
                v1 = values[1]->Eval(contextualpattern, aNULL, idthread);
                a = call.Eval(contextualpattern, v1, idthread);
                if (v1 != values[1])
                    v1->Releasenonconst();
                
                return a;
            }
            
            if (globalTamgu->allmethods.check(n)) {
                TamguCallMethod call(n);

                for (i = 2; i < sz; i++)
                    call.arguments.push_back(values[i]);
                
                v1 = values[1]->Eval(contextualpattern, aNULL, idthread);
                a = call.Eval(contextualpattern, v1, idthread);
                if (v1 != values[1])
                    v1->Releasenonconst();
                
                return a;
            }
            
            string s("Unknown symbol: ");
            s += globalTamgu->Getsymbol(n);
            return globalTamgu->Returnerror(s, idthread);
        }
        case a_eq:
            if (sz != 3)
                return globalTamgu->Returnerror("Wrong number of arguments in 'eq'", idthread);
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerrorwithrelease(v1, v0);
            
            if (v0->isNumber()) {
                if (v1->isNumber())
                    return v0->same(v1);
                return aFALSE;
            }
            else {
                if (v1->isNumber())
                    return aFALSE;
            }
            
            if (v1->isLisp() && v0->isVectorContainer() && !v1->Size()) {
                if (v1->Size())
                    a = aFALSE;
                else
                    a = aTRUE;
            }
            else {
                n = v0->Name();
                if (n && n == v1->Name())
                    a = aTRUE;
                else
                    a = aFALSE;
            }
            v0->Releasenonconst();
            v1->Releasenonconst();
            return a;
        case a_cadr:
        {
            if (sz != 2)
                return globalTamgu->Returnerror("Expecting a list", idthread);
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            for (i = 0; i < a->Size(); i++) {
                if (a->getbyte(i) == 'a')
                    v1 = v0->car(idthread);
                else
                    v1 = v0->cdr(idthread);
                if (v1 == aNOELEMENT) {
                    v0->Releasenonconst();
                    return aNULL;
                }
                v0->Releasenonconst();
                v0 = v1;
            }
            return v0;
        }
        case a_lambda:
        {
                //The first elements is the parameters
            if (sz < 3)
                return globalTamgu->Returnerror("Missing body",idthread);
            v1 = values[1];
            
            if (v1->isFunction()) {
                n = v1->Name();
                //Declarelocal only declare a function or a variable in a local domain (a function for instance)
                //If the context is the main frame, then nothing happens (it has been installed already), the method returns false
                //otherwise, we store it as a variable. When the local domain is cleaned, the variable is also removed from the variable stack
                if (contextualpattern->Declarelocal(idthread, n, v1))
                    globalTamgu->Storevariable(idthread, n, v1);
                return v1;
            }

            if (!v1->isLisp())
                return globalTamgu->Returnerror("Missing parameters",idthread);

            char name[10];
            sprintf_s(name, 10, "%%l_%ld", idtracker);
            n = globalTamgu->Getid(name);
            
            a = new TamguFunction(n, globalTamgu);
            for (i = 0; i < v1->Size(); i++) {
                n = v1->getvalue(i)->Name();
                if (!n) {
                    a->Remove();
                    return globalTamgu->Returnerror("Wrong parameter definition",idthread);
                }
                v0 = new TamguSelfVariableDeclaration(globalTamgu, n);
                a->AddInstruction(v0);
            }
            a->Setchoice(1);
            
            v1 = new TamguCallReturn(globalTamgu, a);
            if (sz == 3) {
                v1->AddInstruction(values[2]);
                values[2]->Setreference();
            }
            else {
                Tamgulisp* block = new Tamgulispcode(globalTamgu);
                block->Setreference();
                block->Push(globalTamgu->Providelispsymbols(a_block));
                for (i = 2; i < sz; i++)
                    block->values.push_back(values[i]);
                v1->AddInstruction(block);                
            }
            a->Setchoice(a_lambda);
            n = a->Name();
            globalTamgu->Storevariable(idthread, n, a);
            //Declarelocal only declare a function or a variable in a local domain (a function for instance)
            //If the context is the main frame, then nothing is done
            contextualpattern->Declarelocal(idthread, n, a);
            values[1] = a;
            return a;
        }
        case a_defun:
        {

            v0 = values[1];

            if (v0->isFunction()) {
                n = v0->Name();
                
                //Declarelocal only declare a function or a variable in a local domain (a function for instance)
                //If the context is the main frame, then nothing happens (it has been installed already), the method returns false
                //otherwise, we store it as a variable. When the local domain is cleaned, the variable is also removed from the variable stack
                if (contextualpattern->Declarelocal(idthread, n, v0))
                    globalTamgu->Storevariable(idthread, n, v0);
                return v0;
            }

            n = v0->Name();
            if (n == 0)
                return globalTamgu->Returnerror("Wrong function name",idthread);

                //the second element is the name...
                //Then the body
            if (!contextualpattern->hasDeclaration())
                return globalTamgu->Returnerror("Wrong declaration: out of scope",idthread);
            
            if (sz < 4)
                return globalTamgu->Returnerror("Missing body",idthread);
            
            v1 = values[2];
            if (!v1->isLisp())
                return globalTamgu->Returnerror("Missing parameters",idthread);
            
            a = new TamguFunction(n, globalTamgu);

            ret = contextualpattern->Declarelocal(idthread, n, a);
            if (ret == a_mainframe || ret == a_declaration) {
                a->Remove();
                return globalTamgu->Returnerror("Already declared",idthread);
            }

            for (i = 0; i < v1->Size(); i++) {
                n = v1->getvalue(i)->Name();
                if (!n)
                    return globalTamgu->Returnerror("Wrong parameter definition",idthread);
                v0 = new TamguSelfVariableDeclaration(globalTamgu, n);
                a->AddInstruction(v0);
            }
            a->Setchoice(1);
            v1 = new TamguCallReturn(globalTamgu, a);
            if (sz == 4) {
                v1->AddInstruction(values[3]);
                values[3]->Setreference();
            }
            else {
                Tamgulisp* block = new Tamgulispcode(globalTamgu);
                block->Setreference();
                block->Push(globalTamgu->Providelispsymbols(a_block));
                for (i = 3; i < sz; i++)
                    block->values.push_back(values[i]);
                v1->AddInstruction(block);
            }
            
            n = a->Name();
            globalTamgu->Storevariable(idthread, n, a);
            //Declarelocal only declare a function or a variable in a local domain (a function for instance)
            //If the context is the main frame, then nothing happens
            values[1] = a;
            return a;
        }
        case a_label:
            if (sz != 3)
                return globalTamgu->Returnerror("Wrong number of arguments in 'label'", idthread);
            //first is the name of the future variable
            v0 = values[1];
            n = v0->Name();
            if (!n)
                return globalTamgu->Returnerror("Wrong name", idthread);
            
            a = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerror(a);
            
            ret = contextualpattern->Declarelocal(idthread, n, a);
            if (globalTamgu->isDeclared(n, idthread)) {
                if (ret == a_mainframe || ret == a_declaration) {
                    a->Remove();
                    return globalTamgu->Returnerror("Already declared",idthread);
                }
            }

            a->Setreference();
            globalTamgu->Storevariable(idthread, n, a);
            //Declarelocal only declare a function or a variable in a local domain (a function for instance)
            //If the context is the main frame, then nothing happens
            return a;
        case a_atomp:
            if (sz != 2)
                return globalTamgu->Returnerror("Wrong number of arguments in 'atomp'", idthread);
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            if (v0->Type() == a_atom) {
                v0->Releasenonconst();
                return aTRUE;
            }
            v0->Releasenonconst();
            return aFALSE;
        case a_numberp:
            if (sz != 2)
                return globalTamgu->Returnerror("Wrong number of arguments in 'numberp'", idthread);
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            if (v0->isNumber())  {
                v0->Releasenonconst();
                return aTRUE;
            }
            v0->Releasenonconst();
            return aFALSE;
        case a_consp:
            if (sz != 2)
                return globalTamgu->Returnerror("Wrong number of arguments in 'consp'", idthread);
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            if (v0->isVectorContainer()) {
                v0->Releasenonconst();
                return aTRUE;
            }
            v0->Releasenonconst();
            return aFALSE;
        case a_zerop:
            if (sz != 2)
                return globalTamgu->Returnerror("Wrong number of arguments in 'zerop'", idthread);
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            n = v0->Integer();
            v0->Releasenonconst();
            if (!n)
                return aTRUE;
            return aFALSE;
        case a_nullp:
            if (sz != 2)
                return globalTamgu->Returnerror("Wrong number of arguments in 'nullp'", idthread);
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            if (v0 == aNULL)
                return aTRUE;
            v0->Releasenonconst();
            return aFALSE;
        case a_block:
            if (sz < 2)
                return globalTamgu->Returnerror("Wrong number of arguments in 'block'", idthread);
            v1 = aNULL;
            for (i = 1; i < sz && !v1->needInvestigate(); i++) {
                v1->Releasenonconst();
                v1 = values[i]->Eval(contextualpattern, aNULL, idthread);
            }
            return v1;
        case a_setq:
            if (sz != 3)
                return globalTamgu->Returnerror("Wrong number of arguments in 'setq'", idthread);
            n = values[1]->Name();
            if (!n)
                return globalTamgu->Returnerror("Wrong name", idthread);

            a = values[2]->Eval(contextualpattern, aNULL, idthread);
            
            if (globalTamgu->isDeclared(n, idthread)) {
                v1 = globalTamgu->Getdeclaration(n, idthread);
                if (v1->isAtom()) {
                    v1->Putvalue(a, idthread);
                    a->Releasenonconst();
                    return v1;
                }
                v1->Resetreference();
                a->Setreference();
                contextualpattern->Replacedeclaration(idthread, n, a);
                return a;
            }
            
            globalTamgu->Storevariable(idthread, n, a);
            contextualpattern->Declarelocal(idthread, n, a);
            return a;
        case a_append:
        {
            if (sz < 3)
                return globalTamgu->Returnerror("Wrong number of arguments in 'setq'", idthread);
            Tamgulisp* tl = new Tamgulisp;
            long j;
            for (i = 1; i < sz; i++) {
                v1 = values[i]->Eval(contextualpattern, aNULL, idthread);
                checkerrorwithrelease(v1, tl);
                if (v1->isVectorContainer()) {
                    for (j = 0; j < v1->Size(); j++)
                        tl->Push(v1->getvalue(j));
                }
                else
                    tl->Push(v1);
                v1->Releasenonconst();
            }
            return tl;
        }
        case a_lisp:
        {
            v0 = a->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            if (v0->isFunction()) {
                if (!globalTamgu->Pushstacklisp(v0, idthread))
                    return globalTamgu->Returnerror("Stack overflow", idthread);
                
                TamguCallFunction call(v0);
                
                for (i = 1; i < sz; i++)
                    call.arguments.push_back(values[i]);
                
                a = call.Eval(contextualpattern, aNULL, idthread);
                globalTamgu->Popstacklisp(idthread);
                return a;
            }

            
            if (sz == 1)
                return a->Eval(contextualpattern, aNULL, idthread);
            
            Tamgulisp tl;
            tl.Push(v0);
            for (i = 1; i < sz; i++)
                tl.Push(values[i]);
            a = tl.Eval(contextualpattern, aNULL, idthread);
            v0->Releasenonconst();
            return a;
        }
    }
    
    return aNULL;
}

string Tamgulisp::String() {
    Locking _lock(this);
    if (loopmark)
        return("(...)");
    TamguCircular _c(this);
    string res;
    int it;
    res = "(";
    bool beg = true;
    string sx;
    Tamgu* element;
    for (it = 0; it < values.size(); it++) {
        element = values[it];
        sx = element->StringToDisplay();
        if (!element->isString() || element->isContainer()) {
            if (sx == "")
                sx = "''";
            if (beg == false) {
                if (sx[0] != '|')
                    res += " ";
            }
            res += sx;
        }
        else {
            if (beg == false)
                res += " ";
            stringing(res, sx);
        }
        beg = false;

    }
    res += ")";
    return res;
}
