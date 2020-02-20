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
#include "tamgufile.h"
#include "tamguglobal.h"
#include "instructions.h"
#include "compilecode.h"
#include "tamguversion.h"
#include "tamgulisp.h"

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
    Tamgulisp::AddMethod(global, "load", &Tamgulisp::MethodLoad, P_ONE, "load(path): load a lisp from the file path.");

    global->newInstance[a_lisp] = new Tamgulispcode(global);
    global->RecordMethods(a_lisp, Tamgulisp::exported);

    global->lispactions[a_block] = P_ATLEASTTWO;

    global->lispactions[a_in] = P_THREE;
    global->lispactions[a_if] = P_THREE | P_FOUR;
    global->lispactions[a_booleanxor] = P_THREE;
    global->lispactions[a_booleanor] = P_ATLEASTTHREE;
    global->lispactions[a_booleanand] = P_ATLEASTTHREE;
    global->lispactions[a_negation] = P_TWO;
    
    global->lispactions[a_load] = P_TWO;
    global->lispactions[a_eval] = P_TWO;
    global->lispactions[a_for] = P_ATLEASTFOUR;
    global->lispactions[a_return] = P_TWO | P_THREE;
    global->lispactions[a_lambda] = P_ATLEASTTHREE;
    global->lispactions[a_defun] = P_ATLEASTFOUR;
    global->lispactions[a_while] = P_ATLEASTTHREE;
    global->lispactions[a_break] = P_ONE;

    
    global->lispactions[a_key] = P_THREE|P_FOUR;
    global->lispactions[a_keys] = P_FOUR|P_THREE;
    global->lispactions[a_list] = P_ATLEASTTHREE;
    global->lispactions[a_append] = P_ATLEASTTHREE;
    global->lispactions[a_same] = P_THREE;
    global->lispactions[a_different] = P_THREE;
    global->lispactions[a_less] = P_THREE;
    global->lispactions[a_more] = P_THREE;
    global->lispactions[a_lessequal] = P_THREE;
    global->lispactions[a_moreequal] = P_THREE;
    
    global->lispactions[a_or] = P_ATLEASTTHREE;
    global->lispactions[a_xor] = P_ATLEASTTHREE;
    global->lispactions[a_and] = P_ATLEASTTHREE;
    global->lispactions[a_plus] = P_ATLEASTTHREE;
    global->lispactions[a_minus] = P_ATLEASTTHREE;
    global->lispactions[a_multiply] = P_ATLEASTTHREE;
    global->lispactions[a_divide] = P_THREE;
    global->lispactions[a_power] = P_THREE;
    global->lispactions[a_mod] = P_THREE;
    global->lispactions[a_shiftleft] = P_THREE;
    global->lispactions[a_shiftright] = P_THREE;
    
    global->lispactions[a_setq] = P_THREE;
    global->lispactions[a_quote] = P_TWO;
    global->lispactions[a_cons] = P_THREE;
    global->lispactions[a_cond] = P_ATLEASTTWO;
    global->lispactions[a_self] = P_ATLEASTTWO;
    global->lispactions[a_body] = P_TWO;
    global->lispactions[a_atom] = P_FULL;
    global->lispactions[a_eq] = P_THREE;
    global->lispactions[a_cadr] = P_TWO;
    global->lispactions[a_label] = P_THREE;
    global->lispactions[a_atomp] = P_TWO;
    global->lispactions[a_numberp] = P_TWO;
    global->lispactions[a_consp] = P_TWO;
    global->lispactions[a_zerop] = P_TWO;
    global->lispactions[a_nullp] = P_TWO;
    global->lispactions[a_lisp] = P_ATLEASTONE;

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
    short symb;
    if (n == "t")
        symb  = a_true;
    else
        symb = Getid(n);
    
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

Tamgu* TamguFunction::Lispbody() {
    if (instructions.last == 1) {
        if (instructions.vecteur[0]->isReturned()) {
            Tamgu* l = globalTamgu->Providelisp();
            l->Push(globalTamgu->Providelispsymbols(a_lambda));
            Tamgu* args = globalTamgu->Providelisp();
            short a;
            for (long i = 0; i < parameters.last; i++) {
                a = parameters[i]->Name();
                args->Push(globalTamgu->Providelispsymbols(a));
            }
            l->Push(args);
            args = instructions.vecteur[0]->Argument(0);
            l->Push(args);
            return l;
        }
    }
    return aNULL;
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
        
        Tamgulisp* lp = globalTamgu->Providelisp();
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
    
    Tamgulisp* lp = globalTamgu->Providelisp();
    for (long i = 1; i < sz; i++)
        lp->Push(values[i]);
    return lp;
}

Tamgu* Tamgulisp::MethodEval(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    Tamgu* a =  callfunc->Evaluate(0, contextualpattern, idthread);
    if (a->isLisp() || a->isError())
        return a;
    
    string s = a->String();
    return globalTamgu->EvaluateLisp(contextualpattern, "buffer", s, idthread);
}

Tamgu* Tamgulisp::MethodLoad(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    string filename =  callfunc->Evaluate(0, contextualpattern, idthread)->String();
    
    Tamgufile file;
    
#ifdef WIN32
    fopen_s(&file.thefile, STR(filename), "rb");
#else
    file.thefile=fopen(STR(filename), "rb");
#endif
    
    if (file.thefile == NULL) {
        string msg="Cannot open the file:";
        msg += filename;
        return globalTamgu->Returnerror(msg, idthread);
    }
    string body = file.read(-1);
    return globalTamgu->EvaluateLisp(contextualpattern, filename, body, idthread);
}

    //For once it is the original historical Eval...
Tamgu* Tamgulisp::Eval(Tamgu* contextualpattern, Tamgu* v0, short idthread) {
        //We need to take into account the different case...
    long sz = values.size();
    if (sz == 0)
        return this;
    
    if (globalTamgu->Error(idthread))
        return globalTamgu->Errorobject(idthread);
    
    Setinstruction(idthread);
    
    Tamgu* a = values[0];
    long i, n;
    Tamgu* v1;
    char ret;
    
    n = a->Action();
    if (!Arity(globalTamgu->lispactions.get(n), sz)) {
        string msg = "Wrong number of arguments in '";
        msg += globalTamgu->Getsymbol(n);
        msg += "'";
        return globalTamgu->Returnerror(msg,idthread);
    }
    
    switch (n) {
        case a_break:
            return aBREAK;
        case a_return:
            if (sz == 3) {
                if (values[2]->isReturned()) {
                    v1 = values[1]->Eval(contextualpattern, aNULL, idthread);
                    values[2]->Put(contextualpattern, v1, idthread);
                    return values[2];
                }
            }
            
            v1 = values[1]->Eval(contextualpattern, aNULL, idthread);
            v0 = new TamguCallReturn(globalTamgu);
            values.push_back(v0);
            v0->Put(contextualpattern,v1, idthread);
            return v0;
        case a_for: //(for x v (block...))
        {
            n = values[1]->Name();

            if (values[2]->isInstruction()) {
                if (globalTamgu->isDeclared(n, idthread)) {
                    v0 = globalTamgu->Getdeclaration(n, idthread);
                    values[2]->Instruction(0)->Putinstruction(0,v0);
                }

                return values[2]->Eval(contextualpattern, aNULL, idthread)->Returned(idthread);
            }
            
            TamguInstructionFORIN* forin = new TamguInstructionFORIN(globalTamgu);
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
                Tamgulispcode* block = new Tamgulispcode(globalTamgu);
                block->idinfo = Currentinfo();
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
            if (values[1]->isInstruction())
                return values[1]->Eval(contextualpattern, aNULL, idthread)->Returned(idthread);
            
            TamguInstructionWHILE* awhile = new TamguInstructionWHILE(globalTamgu);
            awhile->instructions.push_back(values[1]); // the condition
            if (sz == 3)
                awhile->instructions.push_back(values[2]);
            else {
                Tamgulispcode* block = new Tamgulispcode(globalTamgu);
                block->idinfo = Currentinfo();
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
            v1 = values[1]->Eval(contextualpattern, aNULL, idthread);
            //checkerror(v1);
            if (v1 == aTRUE)
                return aFALSE;
            else
                return aTRUE;
        case a_list:
        {
            Tamgulisp* tl = globalTamgu->Providelisp();
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
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerrorwithrelease(v1, v0);
            a = v0->same(v1);
            v0->Releasenonconst();
            v1->Releasenonconst();
            return a;
        case a_different:
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerrorwithrelease(v1, v0);
            a = v0->different(v1);
            v0->Releasenonconst();
            v1->Releasenonconst();
            return a;
        case a_less:
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerrorwithrelease(v1, v0);
            a = v0->less(v1);
            v0->Releasenonconst();
            v1->Releasenonconst();
            return a;
        case a_more:
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerrorwithrelease(v1, v0);
            a = v0->more(v1);
            v0->Releasenonconst();
            v1->Releasenonconst();
            return a;
        case a_lessequal:
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerrorwithrelease(v1, v0);
            a = v0->lessequal(v1);
            v0->Releasenonconst();
            v1->Releasenonconst();
            return a;
        case a_moreequal:
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerrorwithrelease(v1, v0);
            a = v0->moreequal(v1);
            v0->Releasenonconst();
            v1->Releasenonconst();
            return a;
        case a_plus:
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
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerrorwithrelease(v1, v0);
            v0 = v0->Atom();
            v0->power(v1,true);
            v1->Releasenonconst();
            return v0;
        case a_mod:
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
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerrorwithrelease(v1, v0);
            v0 = v0->Atom();
            v0 = v0->shiftleft(v1,true);
            v1->Releasenonconst();
            return v0;
        case a_shiftright:
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerrorwithrelease(v1, v0);
            v0 = v0->Atom();
            v0 = v0->shiftright(v1,true);
            v1->Releasenonconst();
            return v0;
        case a_and:
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
        case a_in:
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            if (v0->isRegular())
                a = v0->in(contextualpattern, v1, idthread);
            else
                a = v1->in(contextualpattern, v0, idthread);
            v0->Releasenonconst();
            v1->Releasenonconst();
            return a;
        case a_if:
                //Comparison first element
            a = values[1]->Eval(aTRUE, aNULL, idthread);
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
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerrorwithrelease(v1, v0);
                        
            if (!v1->isVectorContainer())
                return aNULL;
            
            Tamgulisp* tl = globalTamgu->Providelisp();
            tl->Push(v0);
            for (i = 0; i < v1->Size(); i++)
                tl->Push(v1->getvalue(i));
            v0->Releasenonconst();
            v1->Releasenonconst();
            return tl;
        }
        case a_cond:
        {
            long szv;
            for (i = 1; i < sz; i++) {
                v1 = values[i];
                szv = v1->Size();
                
                if (!v1->isLisp() || szv <= 1)
                    return globalTamgu->Returnerror("Wrong cond element", idthread);
                
                Tamgulisp* code = (Tamgulisp*)v1;

                a = code->values[0]->Eval(contextualpattern, aNULL, idthread);
                checkerror(a);
                if (a == aTRUE) {
                    v0 = aNULL;
                    for (int j = 1; j < szv && !v0->isError(); j++) {
                        v0->Release();
                        v0 = code->values[j]->Eval(contextualpattern, aNULL, idthread);
                    }
                    return v0;
                }
            }
            return aNULL;
        }
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
            
            string s("Unknown function: ");
            s += globalTamgu->Getsymbol(n);
            return globalTamgu->Returnerror(s, idthread);
        }
        case a_eq:
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
                Tamgulispcode* block = new Tamgulispcode(globalTamgu);
                block->idinfo = Currentinfo();
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
                Tamgulispcode* block = new Tamgulispcode(globalTamgu);
                block->idinfo = Currentinfo();
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
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            if (v0->Type() == a_atom) {
                v0->Releasenonconst();
                return aTRUE;
            }
            v0->Releasenonconst();
            return aFALSE;
        case a_numberp:
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            if (v0->isNumber())  {
                v0->Releasenonconst();
                return aTRUE;
            }
            v0->Releasenonconst();
            return aFALSE;
        case a_consp:
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            if (v0->isVectorContainer()) {
                v0->Releasenonconst();
                return aTRUE;
            }
            v0->Releasenonconst();
            return aFALSE;
        case a_zerop:
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            n = v0->Integer();
            v0->Releasenonconst();
            if (!n)
                return aTRUE;
            return aFALSE;
        case a_nullp:
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            if (v0 == aNULL)
                return aTRUE;
            v0->Releasenonconst();
            return aFALSE;
        case a_block:
            v1 = aNULL;
            for (i = 1; i < sz && !v1->needInvestigate(); i++) {
                v1->Releasenonconst();
                v1 = values[i]->Eval(contextualpattern, aNULL, idthread);
            }
            return v1;
        case a_setq:
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
            Tamgulisp* tl = globalTamgu->Providelisp();
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
        case a_eval:
        {
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            TamguCall call(a_eval);
            call.arguments.push_back(v0);
            a = MethodEval(contextualpattern, idthread, &call);
            v0->Releasenonconst();
            return a;
        }
        case a_load:
        {
            TamguCall call(a_load);
            call.arguments.push_back(values[1]);
            return MethodLoad(contextualpattern, idthread, &call);
        }
        case a_key: //We handle vectors and maps... (key m k v), v is optional
        {
            if (sz >= 3) {
                v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
                //We return the value according to the key
                v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
                TamguIndex idx(false);
                idx.left = v1;

                if (sz == 3) {
                    //We simply return the value...
                    a = v0->Eval(contextualpattern, &idx, idthread);
                    v0->Releasenonconst();
                    v1->Releasenonconst();
                    return a;
                }
                if (sz == 4) {
                    a = values[3]->Eval(contextualpattern, aNULL, idthread);
                    if (v0->isConst())
                        v0 = v0->Newvalue(v0, idthread);
                    v0->Put(&idx, a, idthread);
                    a->Releasenonconst();
                    v1->Releasenonconst();
                    return v0; //We return the map...
                }
            }
            return globalTamgu->Returnerror("Wrong number of arguments in 'key'", idthread);
            
        }
        case a_keys: //We handle vectors and maps... (keys m kleft kright v), v is optional
        {
            if (sz >= 4) {
                v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
                //We return the value according to the key
                v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
                Tamgu* v2 = values[3]->Eval(contextualpattern, aNULL, idthread);
                TamguIndex idx(true);
                idx.left = v1;
                idx.right = v2;

                if (sz == 4) {
                    //We simply return the value...
                    a = v0->Eval(contextualpattern,&idx, idthread);
                    v0->Releasenonconst();
                    v1->Releasenonconst();
                    v2->Releasenonconst();
                    return a;
                }
                if (sz == 5) {
                    a = values[4]->Eval(contextualpattern, aNULL, idthread);
                    if (v0->isConst())
                        v0 = v0->Newvalue(v0, idthread);
                    v0->Put(&idx, a, idthread);
                    a->Releasenonconst();
                    v1->Releasenonconst();
                    v2->Releasenonconst();
                    return v0; //We return the map...
                }
            }
            return globalTamgu->Returnerror("Wrong number of arguments in 'keys'", idthread);
        }
        case a_body:
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            return v0->Lispbody();
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
            
            Tamgulispcode tl(NULL);
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
