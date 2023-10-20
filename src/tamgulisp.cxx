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
#include "predicate.h"

//We need to declare once again our local definitions.
#define checkerror(a) if (a->isError()) return a
#define checkerrorwithrelease(a,v) if (a->isError()) {v->Releasenonconst(); return a;}
#define lockparse() if (globalTamgu->threadMODE) globalTamgu->_parselock.Locking()
#define unlockparse() if (globalTamgu->threadMODE) globalTamgu->_parselock.Unlocking()

//------------------------------------------------------------------------------------------------------------------
Exporting basebin_hash<lispMethod>  Tamgulisp::methods;

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgulisp::AddMethod(TamguGlobal* global, string name, lispMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(a_lisp) != global->infomethods.end() &&
            global->infomethods[a_lisp].find(name) != global->infomethods[a_lisp].end())
    return;

    global->infomethods[a_lisp][name] = infos;
    global->RecordArity(a_lisp, idname, arity);
}


bool Tamgulisp::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();

    Tamgulisp::AddMethod(global, "eval", &Tamgulisp::MethodEval, P_ONE, "eval(e): 'e' is a lisp expression provided as a string.");
    Tamgulisp::AddMethod(global, "load", &Tamgulisp::MethodLoad, P_ONE, "load(path): load a lisp from the file path.");

    global->newInstance[a_lisp] = new Tamgulispcode(global);
    global->RecordCompatibilities(a_lisp);

    global->lispactions[a_block] = P_ATLEASTTWO;

    global->lispactions[a_in] = P_THREE;
    global->lispactions[a_if] = P_THREE | P_FOUR;
    global->lispactions[a_booleanxor] = P_THREE;
    global->lispactions[a_booleanor] = P_ATLEASTTHREE;
    global->lispactions[a_booleanand] = P_ATLEASTTHREE;
    global->lispactions[a_negation] = P_TWO;
    
    global->lispactions[a_load] = P_TWO;
    global->lispactions[a_eval] = P_TWO;
    global->lispactions[a_apply] = P_THREE;
    global->lispactions[a_for] = P_ATLEASTFOUR;
    global->lispactions[a_return] = P_TWO | P_THREE;
    global->lispactions[a_lambda] = P_ATLEASTTHREE;
    global->lispactions[a_defun] = P_ATLEASTFOUR;
    global->lispactions[a_while] = P_ATLEASTTHREE;
    global->lispactions[a_break] = P_ONE;

    
    global->lispactions[a_key] = P_THREE|P_FOUR;
    global->lispactions[a_keys] = P_FOUR|P_THREE;
    global->lispactions[a_list] = P_ATLEASTTHREE;
    global->lispactions[a_merge] = P_ATLEASTTHREE;
    global->lispactions[a_same] = P_THREE;
    global->lispactions[a_different] = P_THREE;
    global->lispactions[a_less] = P_THREE;
    global->lispactions[a_more] = P_THREE;
    global->lispactions[a_lessequal] = P_THREE;
    global->lispactions[a_moreequal] = P_THREE;
    
    global->lispactions[a__map] = P_THREE;
    global->lispactions[a__filter] = P_THREE;
    global->lispactions[a__takewhile] = P_THREE;
    global->lispactions[a__dropwhile] = P_THREE;
    global->lispactions[a__zip] = P_ATLEASTTHREE;
    global->lispactions[a__zipwith] = P_ATLEASTFOUR;
    global->lispactions[a__foldl] = P_FOUR;
    global->lispactions[a__foldr] = P_FOUR;
    global->lispactions[a__foldl1] = P_THREE;
    global->lispactions[a__foldr1] = P_THREE;
    global->lispactions[a__scanl] = P_FOUR;
    global->lispactions[a__scanr] = P_FOUR;
    global->lispactions[a__scanl1] = P_THREE;
    global->lispactions[a__scanr1] = P_THREE;

    global->lispactions[a_or] = P_ATLEASTTWO;
    global->lispactions[a_xor] = P_ATLEASTTWO;
    global->lispactions[a_and] = P_ATLEASTTWO;
    global->lispactions[a_plus] = P_ATLEASTTWO;
    global->lispactions[a_minus] = P_ATLEASTTWO;
    global->lispactions[a_multiply] = P_ATLEASTTWO;
    global->lispactions[a_divide] = P_ATLEASTTWO;
    global->lispactions[a_power] = P_ATLEASTTWO;
    global->lispactions[a_mod] = P_ATLEASTTWO;
    global->lispactions[a_shiftleft] = P_ATLEASTTWO;
    global->lispactions[a_shiftright] = P_ATLEASTTWO;

    global->lispactions[a_callfunction] = P_ATLEASTONE;
    global->lispactions[a_calllisp] = P_ATLEASTONE;
    global->lispactions[a_callprocedure] = P_ATLEASTONE;
    global->lispactions[a_callcommon] = P_ATLEASTTWO;
    global->lispactions[a_callmethod] = P_ATLEASTTWO;

    global->lispactions[a_assignement] = P_THREE; //setq
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

Tamgu* Tamgulisp::Convert(Tamgu* v, short idthread) {
    if (!v->Size())
        return aTRUE;
    
    Tamgu* val;
    Tamgulisp* lsp;
    string s;
    
    
    for (long i = 0; i < v->Size(); i++) {
        val = v->getvalue(i);
        if (val->isString()) {
            s =  val->String();
            push(globalTamgu->Providelispsymbols(s));
        }
        else {
            if (val->isVectorContainer()) {
                if (!val->Size())
                    push(aEMPTYLISP);
                else {
                    lsp = globalTamgu->Providelisp();
                    lsp->Convert(val, idthread);
                    push(lsp);
                }
            }
            else
                pushatom(val);
        }
    }
    return aTRUE;
}

Tamgu* Tamgulisp::Put(Tamgu* c, Tamgu* v, short idthread) {
    if (!v->isVectorContainer())
        return globalTamgu->Returnerror(e_expecting_a_vector03, idthread);
        
    //We then consider strings to be atoms...
    Clear();

    locking();
    c = Convert(v, idthread);
    unlocking();
    
    if (c->isError())
        return c;
    return aTRUE;
}

Tamgulispcode::Tamgulispcode(TamguGlobal* g, Tamgu* parent) : Tamgulisp(g, parent) {
    investigate |= is_constante;
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

Tamgu* TamguGlobal::Providelispsymbols(short symb, short a) {
    return new Tamgusymbol(symb, a, this);
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
            l->push(globalTamgu->Providelispsymbols(a_lambda));
            Tamgu* args = globalTamgu->Providelisp();
            short a;
            for (long i = 0; i < parameters.last; i++) {
                a = parameters[i]->Name();
                args->push(globalTamgu->Providelispsymbols(a));
            }
            l->push(args);
            args = instructions.vecteur[0]->Argument(0);
            l->push(args);
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
                return globalTamgu->systems[name]->value;
    }
    
    string s("Unknown symbol: ");
    s += globalTamgu->Getsymbol(name);
    return globalTamgu->Returnerror(s, idthread);
}

Tamgu* Tamgulispvariable::Eval(Tamgu* a, Tamgu* v, short idthread) {
    if (globalTamgu->systems.check(name))
        v = globalTamgu->systems[name]->value;
    else {
        v =  globalTamgu->Getdeclarationandcheck(name, idthread);
        if (v == NULL) {
            string msg = "Unknown variable: ";
            msg += globalTamgu->Getsymbol(name);
            return globalTamgu->Returnerror(msg, idthread);
        }
    }
    return function->Eval(a, v, idthread);
}


Tamgu* TamguLockContainer::car(short idthread) {
    if (isVectorContainer())
        return getvalue(0)->Atom();
    return aNOELEMENT;
}

Tamgu* TamguLockContainer::cdr(short idthread) {
    if (isVectorContainer()) {
        long sz = Size();
        if (sz == 0)
            return aNOELEMENT;
        
        Tamguvector* lp = globalTamgu->Providevector();
        for (long i = 1; i < sz; i++)
            lp->push(getvalue(i));
        return lp;
    }
    return aNOELEMENT;
}

Tamgu* Tamguvector::cdr(short idthread) {
    locking();
    long sz = values.size();
    if (!sz) {
        unlocking();
        return aNOELEMENT;
    }
    Tamguvector* lp = globalTamgu->Providevector();
    for (long i = 1; i < sz; i++)
        lp->push(values[i]);
    unlocking();
    return lp;
}

Tamgu* Tamgulisp::cdr(short idthread) {
    long sz = values.size();
    if (sz == 0)
        return aNOELEMENT;
    
    Tamgulisp* lp = globalTamgu->Providelisp();
    for (long i = 1; i < sz; i++)
        lp->push(values[i]);
    return lp;
}

Tamgu* Tamgulispair::cdr(short idthread) {
    long sz = values.size();
    if (!sz)
        return aNOELEMENT;
    if (sz == 2)
        return values.back()->Atom();
    
    Tamgulispair* lp = new Tamgulispair;
    for (long i = 1; i < sz; i++)
        lp->push(values[i]);
    return lp;

}

Tamgu* Tamgulisp::MethodEval(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* a =  callfunc->Evaluate(0, contextualpattern, idthread);
    if (a->isLisp() || a->isError()) {
        return a;
    }
    
    string s = a->String();
    locking();
    contextualpattern = globalTamgu->EvaluateLisp(contextualpattern, "buffer", s, idthread);
    unlocking();
    return contextualpattern;
}

Tamgu* Tamgulisp::MethodLoad(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
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
    if (v0->isIndex())
        return Tamguvector::Eval(contextualpattern, v0, idthread);
    
        //We need to take into account the different case...
    long sz = values.size();
    if (sz == 0)
        return this;
    
    if (globalTamgu->Error(idthread))
        return globalTamgu->Errorobject(idthread);
    
    Setinstruction(idthread);
    
    Tamgu* a = values[0];
    long i, n;
    Tamgu* v1 = NULL;
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
                block->push(globalTamgu->Providelispsymbols(a_block));
                forin->instructions.push_back(block);
                for (i = 3; i < sz; i++)
                    block->push(values[i]);
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
                block->push(globalTamgu->Providelispsymbols(a_block));
                awhile->instructions.push_back(block);
                for (i = 2; i < sz; i++)
                    block->push(values[i]);
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
                tl->pushatom(v1);
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
            if (sz == 2 && v0->isContainer()) {
                sz = v0->Size();
                if (sz > 0) {
                    Tamgu* value = v0->getvalue(0);
                    value = value->AtomNoConst();
                    Tamgu* val;
                    for (i = 1; i < sz; i++) {
                        val = v0->getvalue(i);
                        value->plus(val, true);
                        val->Release();
                    }
                    v0->Release();
                    return value;
                }
                return v0;
            }
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
            if (sz == 2 && v0->isContainer()) {
                sz = v0->Size();
                if (sz > 0) {
                    Tamgu* value = v0->getvalue(0);
                    value = value->AtomNoConst();
                    Tamgu* val;
                    for (i = 1; i < sz; i++) {
                        val = v0->getvalue(i);
                        value->minus(val, true);
                        val->Release();
                    }
                    v0->Release();
                    return value;
                }
                return v0;
            }
                
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
            if (sz == 2 && v0->isContainer()) {
                sz = v0->Size();
                if (sz > 0) {
                    Tamgu* value = v0->getvalue(0);
                    value = value->AtomNoConst();
                    Tamgu* val;
                    for (i = 1; i < sz; i++) {
                        val = v0->getvalue(i);
                        value->multiply(val, true);
                        val->Release();
                    }
                    v0->Release();
                    return value;
                }
                return v0;
            }
            v0 = v0->Atom();
            for (i = 2; i < sz; i++) {
                v1 = values[i]->Eval(contextualpattern, aNULL, idthread);
                checkerrorwithrelease(v1, v0);
                v0->multiply(v1,true);
                v1->Releasenonconst();
            }
            return v0;
        case a_divide:
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            if (sz == 2 && v0->isContainer()) {
                sz = v0->Size();
                if (sz > 0) {
                    Tamgu* value = v0->getvalue(0);
                    value = value->AtomNoConst();
                    Tamgu* val;
                    Tamgu* err;
                    for (i = 1; i < sz; i++) {
                        val = v0->getvalue(i);
                        err = value->divide(val, true);
                        val->Release();
                        if (err->isError()) {
                            value->Release();
                            v0->Release();
                            return err;
                        }
                    }
                    v0->Release();
                    return value;
                }
                return v0;
            }
            v0 = v0->Atom();
            for (i = 2; i < sz; i++) {
                v1 = values[i]->Eval(contextualpattern, aNULL, idthread);
                checkerrorwithrelease(v1, v0);
                a = v0->divide(v1,true);
                v1->Releasenonconst();
                checkerrorwithrelease(a, v0);
                if (a != v0) {
                    v0->Releasenonconst();
                    v0 = a;
                }
            }
            return v0;
        case a_power:
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            if (sz == 2 && v0->isContainer()) {
                sz = v0->Size();
                if (sz > 0) {
                    Tamgu* value = v0->getvalue(0);
                    value = value->AtomNoConst();
                    Tamgu* val;
                    for (i = 1; i < sz; i++) {
                        val = v0->getvalue(i);
                        value->power(val, true);
                        val->Release();
                    }
                    v0->Release();
                    return value;
                }
                return v0;
            }
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerrorwithrelease(v1, v0);
            v0 = v0->Atom();
            v0->power(v1,true);
            v1->Releasenonconst();
            return v0;
        case a_mod:
            a = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(a);
            if (sz == 2 && v0->isContainer()) {
                sz = v0->Size();
                if (sz > 0) {
                    Tamgu* value = v0->getvalue(0);
                    value = value->AtomNoConst();
                    Tamgu* val;
                    Tamgu* err;
                    for (i = 1; i < sz; i++) {
                        val = v0->getvalue(i);
                        err = value->mod(val, true);
                        val->Release();
                        if (err->isError()) {
                            value->Release();
                            v0->Release();
                            return err;
                        }
                    }
                    v0->Release();
                    return value;
                }
                return v0;
            }
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
            if (sz == 2 && v0->isContainer()) {
                sz = v0->Size();
                if (sz > 0) {
                    Tamgu* value = v0->getvalue(0);
                    value = value->AtomNoConst();
                    Tamgu* val;
                    for (i = 1; i < sz; i++) {
                        val = v0->getvalue(i);
                        value->shiftleft(val, true);
                        val->Release();
                    }
                    v0->Release();
                    return value;
                }
                return v0;
            }
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerrorwithrelease(v1, v0);
            v0 = v0->Atom();
            v0 = v0->shiftleft(v1,true);
            v1->Releasenonconst();
            return v0;
        case a_shiftright:
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            if (sz == 2 && v0->isContainer()) {
                sz = v0->Size();
                if (sz > 0) {
                    Tamgu* value = v0->getvalue(0);
                    value = value->AtomNoConst();
                    Tamgu* val;
                    for (i = 1; i < sz; i++) {
                        val = v0->getvalue(i);
                        value->shiftright(val, true);
                        val->Release();
                    }
                    v0->Release();
                    return value;
                }
                return v0;
            }
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerrorwithrelease(v1, v0);
            v0 = v0->Atom();
            v0 = v0->shiftright(v1,true);
            v1->Releasenonconst();
            return v0;
        case a_and:
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            if (sz == 2 && v0->isContainer()) {
                sz = v0->Size();
                if (sz > 0) {
                    Tamgu* value = v0->getvalue(0);
                    value = value->AtomNoConst();
                    Tamgu* val;
                    for (i = 1; i < sz; i++) {
                        val = v0->getvalue(i);
                        value->andset(val, true);
                        val->Release();
                    }
                    v0->Release();
                    return value;
                }
                return v0;
            }
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
            if (sz == 2 && v0->isContainer()) {
                sz = v0->Size();
                if (sz > 0) {
                    Tamgu* value = v0->getvalue(0);
                    value = value->AtomNoConst();
                    Tamgu* val;
                    for (i = 1; i < sz; i++) {
                        val = v0->getvalue(i);
                        value->xorset(val, true);
                        val->Release();
                    }
                    v0->Release();
                    return value;
                }
                return v0;
            }
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
            if (sz == 2 && v0->isContainer()) {
                sz = v0->Size();
                if (sz > 0) {
                    Tamgu* value = v0->getvalue(0);
                    value = value->AtomNoConst();
                    Tamgu* val;
                    for (i = 1; i < sz; i++) {
                        val = v0->getvalue(i);
                        value->orset(val, true);
                        val->Release();
                    }
                    v0->Release();
                    return value;
                }
                return v0;
            }
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


            if (v1->Type() == a_atom || v1->isNumber() || v1->isString()) {
                a = new Tamgulispair();
                a->pushatom(v0);
                a->pushatom(v1);
                v0->Releasenonconst();
                v1->Releasenonconst();
                return a;
            }
            
            Tamgulisp* tl;

            if (v1->isVectorContainer()) {
                if (v1->Type() == a_pair)
                    tl = new Tamgulispair();
                else
                    tl = globalTamgu->Providelisp();
                tl->pushatom(v0);
                for (i = 0; i < v1->Size(); i++)
                    tl->pushatom(v1->getvalue(i));
                v1->Releasenonconst();
                v0->Releasenonconst();
                return tl;
            }

            if (v1->isNULL()) {
                tl = globalTamgu->Providelisp();
                tl->pushatom(v0);
                v0->Releasenonconst();
                return tl;
            }

            v0->Releasenonconst();
            v1->Releasenonconst();
            return globalTamgu->Returnerror(e_cannot_apply_cons, idthread);
        }
        case a_cond:
        {
            long szv;
            for (i = 1; i < sz; i++) {
                v1 = values[i];
                szv = v1->Size();
                
                if (!v1->isLisp() || szv <= 1)
                    return globalTamgu->Returnerror(e_wrong_cond_element, idthread);
                
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
            return globalTamgu->Returnerror(e_cannot_call_self, idthread);
        case a_atom:
        {
                //it has to ba a function call
            n = a->Name();
            
            if (globalTamgu->isDeclared(n, idthread))
                v0 = globalTamgu->Getdeclaration(n, idthread);
            else
                v0 = globalTamgu->Declaration(n, idthread);

            if (v0 != NULL) {
                if (v0->isFunction()) {
                    if (v0->Type() == a_lisp) {
                        TamguCallLispFunction call(v0);
                        
                        for (i = 1; i < sz; i++)
                            call.arguments.push_back(values[i]);
                        
                        return call.Eval(contextualpattern, aNULL, idthread);
                    }
                    else {
                        TamguCallFunction call(v0);
                        
                        for (i = 1; i < sz; i++)
                            call.arguments.push_back(values[i]);
                        
                        return call.Eval(contextualpattern, aNULL, idthread);
                    }
                }
                
                //It might be an operator
                n = v0->Name();
                if (globalTamgu->lispactions.check(n)) {
                    Tamgulisp lsp(-1);
                    lsp.values.push_back(globalTamgu->Providelispsymbols(n));
                    for (i =  1; i < sz; i++)
                        lsp.values.push_back(values[i]);
                    return lsp.Eval(contextualpattern, aNULL, idthread);
                }
                return aNULL;
            }
            
            if (globalTamgu->predicates.check(n)) {
                TamguInstructionLaunch launch(NULL,NULL);
                TamguPredicate kx(n, NULL, a_predicate, &launch);
                for (i = 1; i < sz; i++)
                    kx.parameters.push_back(values[i]);
                return launch.Eval(contextualpattern, aNULL, idthread);
            }
            
            if (globalTamgu->procedures.check(n)) {
                TamguCallProcedure call(n);
                for (i = 1; i < sz; i++)
                    call.arguments.push_back(values[i]);
                
                return call.Eval(contextualpattern, aNULL, idthread);
            }

            if (globalTamgu->commons.check(n)) {
                if (sz < 2)
                    return globalTamgu->Returnerror(e_wrong_number_of03, idthread);

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
        case a_callfunction:
        {
            //it has to ba a function call
            n = a->Name();
            
            v0 = globalTamgu->Declaration(n, idthread);

            if (v0->Type() == a_lisp) {
                TamguCallLispFunction call(v0);
                
                for (i = 1; i < sz; i++)
                    call.arguments.push_back(values[i]);
                
                return call.Eval(contextualpattern, aNULL, idthread);
            }
            
            TamguCallFunction call(v0);
            
            for (i = 1; i < sz; i++)
                call.arguments.push_back(values[i]);
            
            return call.Eval(contextualpattern, aNULL, idthread);
        }
        case a_calllisp:
        {
            n = a->Name();
            
            v0 = globalTamgu->Declaration(n, idthread);
            
            TamguCallLispFunction call(v0);
            
            for (i = 1; i < sz; i++)
                call.arguments.push_back(values[i]);
            
            return call.Eval(contextualpattern, aNULL, idthread);
        }
        case a_callprocedure:
        {
            n = a->Name();

            TamguCallProcedure call(n);
            for (i = 1; i < sz; i++)
                call.arguments.push_back(values[i]);
            
            if (!call.Checkarity()) {
                globalTamgu->threads[idthread].currentinstruction = this;
                string mess("'");
                mess += globalTamgu->Getsymbol(n);
                mess += "' : Wrong number of arguments...";
                return globalTamgu->Returnerror(mess, idthread);
            }

            return call.Eval(contextualpattern, aNULL, idthread);
        }
        case a_callcommon:
        {
            
            n = a->Name();

            TamguCallCommonMethod call(n);
            for (i = 2; i < sz; i++)
                call.arguments.push_back(values[i]);

            if (!call.Checkarity()) {
                globalTamgu->threads[idthread].currentinstruction = this;
                string mess("'");
                mess += globalTamgu->Getsymbol(n);
                mess += "' : Wrong number of arguments...";
                return globalTamgu->Returnerror(mess, idthread);
            }

            v1 = values[1]->Eval(contextualpattern, aNULL, idthread);

            a = call.Eval(contextualpattern, v1, idthread);
            if (v1 != values[1] && v1 != a)
                v1->Releasenonconst();
            
            return a;
        }
        case a_callmethod:
        {
            n = a->Name();

            TamguCallMethod call(n);
            
            for (i = 2; i < sz; i++)
                call.arguments.push_back(values[i]);
            v1 = values[1]->Eval(contextualpattern, aNULL, idthread);
            if (!globalTamgu->checkarity(v1->Type(), n, sz-2)) {
                if (!globalTamgu->frames.check(v1->Type())) {
                    globalTamgu->threads[idthread].currentinstruction = this;
                    v1->Releasenonconst();
                    string mess("'");
                    mess += globalTamgu->Getsymbol(n);
                    mess += "' : Unknown method or Wrong number of arguments for type: '";
                    mess += v1->Typestring();
                    mess += "'";
                    return globalTamgu->Returnerror(mess, idthread);
                }
            }
            a = call.Eval(contextualpattern, v1, idthread);
            if (v1 != values[1] && v1 != a)
                v1->Releasenonconst();
            
            return a;
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
                if (v0->Size())
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
            v1 = values[1];
            
            if (v1->isFunction())
                return v1;

            if (!v1->isLisp())
                return globalTamgu->Returnerror(e_missing_parameters,idthread);
            
            //The first elements is the parameters
            bool fromEval = !isConst();
            if (fromEval) {
                //A bit of explanation...
                //if contextalpattern is aEMPTYLISP, this is an eval of a lambda expression
                //if contextalpattern is not aEMPTYLISP, it means that this lambda is part of a larger
                //expression, that should not be deleted when back Evaluatelisp, which releases these structures...
                //TamguLispFunction exposes two methods: Popping which put reset to true and Resetreference, which delete this
                //object when reset is true...
                a = new TamguLispFunction(a_lambda); //here, the object can be released in Evaluatelisp
                a->Setreference();
            }
            else
                a = new TamguFunction(a_lambda, globalTamgu);
            
            ((TamguFunction*)a)->idtype = a_lisp;
            
            for (i = 0; i < v1->Size(); i++) {
                n = v1->getvalue(i)->Name();
                if (!n) {
                    a->Remove();
                    return globalTamgu->Returnerror(e_wrong_parameter_definition,idthread);
                }
                if (fromEval)
                    v0 = new TamguSelfVariableDeclaration(NULL, n);
                else
                    v0 = new TamguSelfVariableDeclaration(globalTamgu, n);
                a->AddInstruction(v0);
            }
            a->Setchoice(1);
            
            if (fromEval)
                v1 = new TamguCallReturn(NULL, a);
            else
                v1 = new TamguCallReturn(globalTamgu, a);
            
            if (sz == 3) {
                v1->AddInstruction(values[2]);
                values[2]->Setreference();
            }
            else {
                Tamgulisp* block;
                if (fromEval) {
                    block = globalTamgu->Providelisp();
                    block->Setreference();
                }
                else {
                    block = new Tamgulispcode(globalTamgu);
                    block->idinfo = Currentinfo();
                }
                block->push(globalTamgu->Providelispsymbols(a_block));
                for (i = 2; i < sz; i++)
                    block->push(values[i]);
                v1->AddInstruction(block);                
            }
            a->Setchoice(a_lambda);
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
                //We do this operation to allow for a body access from code...
                 if (contextualpattern->Declarelocal(idthread, n, v0))
                     globalTamgu->Storevariable(idthread, n, v0);
                
                return v0;
            }

            bool fromEval = !isConst();
            if (contextualpattern == aEMPTYLISP)
                contextualpattern = globalTamgu->Getmainframe(0);


            v1 = values[2];
            if (!v1->isLisp()) {
                return globalTamgu->Returnerror(e_missing_parameters,idthread);
            }
            
            lockparse();
            n = v0->Name();
            if (n == 0) {
                return globalTamgu->Returnerror(e_wrong_function_name,idthread);
            }

            if (fromEval) {
                a = new TamguLispFunction(n);
                a->Setreference(2);
            }
            else
                a = new TamguFunction(n, globalTamgu);
            
            ((TamguFunction*)a)->idtype = a_lisp;
            
            //If contextualpattern is a local declaration, then 'a' will also be recorded
            //if it is a MainFrame, it won't
            ret = contextualpattern->Declarelocal(idthread, n, a);
            if (ret == a_mainframe || ret == a_declaration) {
                if (fromEval) {
                    v0 = contextualpattern->Declaration(n);
                    v0->Resetreference();
                }
                else {
                    a->Remove();
                    unlockparse();
                    return globalTamgu->Returnerror("Already declared",idthread);
                }
            }

            for (i = 0; i < v1->Size(); i++) {
                n = v1->getvalue(i)->Name();
                if (!n) {
                    if (fromEval)
                        delete a;
                    unlockparse();
                    return globalTamgu->Returnerror(e_wrong_parameter_definition,idthread);
                }
                if (fromEval)
                    v0 = new TamguSelfVariableDeclaration(NULL, n);
                else
                    v0 = new TamguSelfVariableDeclaration(globalTamgu, n);
                a->AddInstruction(v0);
            }
            a->Setchoice(1);
            if (fromEval)
                v1 = new TamguCallReturn(NULL, a);
            else
                v1 = new TamguCallReturn(globalTamgu, a);
            if (sz == 4) {
                v1->AddInstruction(values[3]);
                values[3]->Setreference();
            }
            else {
                Tamgulisp* block;
                if (fromEval) {
                    block = globalTamgu->Providelisp();
                    block->Setreference();
                }
                else {
                    block = new Tamgulispcode(globalTamgu);
                    block->idinfo = Currentinfo();
                }
                block->push(globalTamgu->Providelispsymbols(a_block));
                for (i = 3; i < sz; i++)
                    block->push(values[i]);
                v1->AddInstruction(block);
            }
            
            n = a->Name();
            //Declarelocal does not record an element in the case of a MainFrame...
            //We need to force it...
            if (contextualpattern->isMainFrame())
                contextualpattern->Declare(n, a);
            
            //This is a weird way to enable the access to the function body from Lisp
            //We record a as a variable as well...
            globalTamgu->Storevariable(idthread, n, a);
            //Declarelocal only declare a function or a variable in a local domain (a function for instance)
            //If the context is the main frame, then nothing happens
            values[1] = a;
            unlockparse();
            return a;
        }
        case a_label:
            //first is the name of the future variable
            v0 = values[1];
            n = v0->Name();
            if (!n)
                return globalTamgu->Returnerror(e_wrong_name, idthread);
            
            if (contextualpattern == aEMPTYLISP)
                contextualpattern = globalTamgu->Getmainframe(0);

            a = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerror(a);
            
            ret = contextualpattern->Declarelocal(idthread, n, a);
            if (globalTamgu->isDeclared(n, idthread)) {
                if (ret == a_mainframe || ret == a_declaration) {
                    if (!isConst()) {
                        a->Popping();
                        a->Resetreference();
                    }
                    else
                        a->Remove();
                    return globalTamgu->Returnerror("Already declared",idthread);
                }
            }

            a->Setreference();
            if (contextualpattern->isMainFrame())
                contextualpattern->Declare(n, a);
            
            //This is a weird way to enable the access to the function body from Lisp
            //We record a as a variable as well...
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
        case a_assignement:
            n = values[1]->Name();
            if (!n)
                return globalTamgu->Returnerror(e_wrong_name, idthread);

            a = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerror(a);
            
            if (contextualpattern->isDeclared(n)) {
                v1 = globalTamgu->Getdeclaration(n, idthread);
                if (v1->isAtom() && v1->Type() == a->Type()) {
                    v1->Putvalue(a, idthread);
                    a->Releasenonconst();
                    return v1;
                }
                v1->Resetreference();
                a = a->Atom();
                a->Setreference();
                contextualpattern->Replacedeclaration(idthread, n, a);
                return a;
            }
            
            a = a->Atom();
            a->Setreference();
            if (!idthread && contextualpattern->isMainFrame()) {
                //The variable needs to be declare fully
                contextualpattern->Declare(n, a);
                //This is a hack, actually, the first position for a variable is a TamguGlobalVariableDeclaration
                //Which is not needed here, but since a main variable is always in position 1, we need to add a dummy value here...
                globalTamgu->Storevariable(idthread, n, aNOELEMENT);
            }
            else
                contextualpattern->Declarelocal(idthread, n, a);
            
            globalTamgu->Storevariable(idthread, n, a);
            return a;
        case a_merge:
        {
            Tamgulisp* tl = globalTamgu->Providelisp();
            v1 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerrorwithrelease(v1, tl);
            if (!v1->isVectorContainer()) {
                v1->Release();
                return globalTamgu->Returnerror(e_can_only_append, idthread);
            }
            long j;
            for (j = 0; j < v1->Size(); j++)
                tl->pushatom(v1->getvalue(j));

            for (i = 2; i < sz; i++) {
                v1 = values[i]->Eval(contextualpattern, aNULL, idthread);
                checkerrorwithrelease(v1, tl);
                if (v1->isVectorContainer()) {
                    for (j = 0; j < v1->Size(); j++)
                        tl->pushatom(v1->getvalue(j));
                }
                else
                    tl->pushatom(v1);
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
            return globalTamgu->Returnerror(e_wrong_number_of05, idthread);
            
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
            return globalTamgu->Returnerror(e_wrong_number_of, idthread);
        }
        case a_body:
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            return v0->Lispbody();
        case a_apply: //(apply func a1..an)
        {
            a = values[1]->Eval(contextualpattern, aNULL, idthread);
            n = a->Name();
            if (!n)
                return globalTamgu->Returnerror(e_cannot_evaluate_this02, idthread);

            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            if (!v1->isVectorContainer()) {
                v1->Release();
                return globalTamgu->Returnerror(e_wrong_list_of06, idthread);
            }
            
            Tamgulisp lsp(-1);
            lsp.values.push_back(globalTamgu->Providelispsymbols(a->Name()));
            for (i = 0; i < v1->Size(); i++)
                lsp.values.push_back(v1->getvalue(i));
            a = lsp.Eval(contextualpattern, aNULL, idthread);
            v1->Releasenonconst();
            return a;
        }
        case a__map:
        {
            //First element is the operation, second element the list
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            if (v0->isFunction()) // in this case it is a lambda, we can simply keep it
                v0 = values[1];
            else {
                if (!v0->isLisp() && !globalTamgu->checkoperator(v0->Action()))
                    return globalTamgu->Returnerror(e_wrong_list_of12, idthread);
            }
            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v1);
            if (!v1->isVectorContainer()) {
                v1->Release();
                return globalTamgu->Returnerror(e_wrong_list_of11, idthread);
            }
            //We prepare our solution... Either the content of v0 is one or two
            
            Tamgulisp lsp(-1);
            if (v0->isLisp()) {
                sz = v0->Size();
                if (sz == 2) {
                    a = ((Tamgulisp*)v0)->values[1]; // (_map '(1 -) '(1 2 3 4))
                    if (globalTamgu->checkoperator(a->Action())) {
                        lsp.pushatom(a);
                        a = ((Tamgulisp*)v0)->values[0];
                        if (!a->isConst())
                            a = a->Eval(aNULL, aNULL, idthread);
                        lsp.push(a);
                        a->Release();
                        lsp.values.push_back(aNULL);
                    }
                    else { // (_map '(- 1) '(1 2 3 4))
                        lsp.pushatom(((Tamgulisp*)v0)->values[0]);
                        lsp.values.push_back(aNULL);
                        a = ((Tamgulisp*)v0)->values[1];
                        if (!a->isConst())
                            a = a->Eval(aNULL, aNULL, idthread);
                        lsp.push(a);
                        a->Release();
                        sz = 1;
                    }
                }
                else {//lambda
                    if (sz == 1)
                        lsp.pushatom(((Tamgulisp*)v0)->values[0]);
                    else
                        lsp.pushatom(v0);
                    lsp.values.push_back(aNULL);
                    sz = 1;
                }
            }
            else {
                sz = 0;
                lsp.values.push_back(v0);
                lsp.values.push_back(aNULL);
                lsp.values.push_back(aNULL);
            }
            
            Tamgulisp* l = globalTamgu->Providelisp();
            Tamgu* e;
            
            Tamgulisp quoted(-1);
            quoted.Setaction(a_quote);
            quoted.Push(aNULL);
            
            if (!sz) {
                lsp.values[1] = &quoted;
                lsp.values[2] = &quoted;
            }
            else
                lsp.values[sz] = &quoted;

            for (i = 0; i < v1->Size(); i++) {
                e = v1->getvalue(i);
                
                quoted.values[1] = e;
                
                e->Setreference();
                a = lsp.Eval(aNULL, aNULL, idthread);
                e->Resetreference();

                l->push(a);
                a->Release();
            }

            if (lsp.values.size()==3) {
                if (sz == 2)
                    lsp.values[1]->Resetreference();
                else
                    if (sz == 1)
                        lsp.values[2]->Resetreference();
            }
            v1->Release();
            v0->Release();
            return l;
        }
        case a__filter:
        {
            //First element is the operation, second element the list
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            if (v0->isFunction())
                v0 = values[1];
            else {
                if (!v0->isLisp() && !globalTamgu->checkoperator(v0->Action()))
                    return globalTamgu->Returnerror(e_wrong_list_of04, idthread);
            }

            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v1);
            if (!v1->isVectorContainer()) {
                v1->Release();
                return globalTamgu->Returnerror(e_wrong_list_of07, idthread);
            }
            //We prepare our solution... Either the content of v0 is one or two
            
            Tamgulisp lsp(-1);
            sz = v0->Size();
            if (sz > 2) {
                lsp.push(v0);
                lsp.values.push_back(aNULL);
            }
            else {
                lsp.pushatom(((Tamgulisp*)v0)->values[0]);
                if (sz == 2) {
                    lsp.values.push_back(aNULL);
                    a = ((Tamgulisp*)v0)->values[1];
                    if (!a->isConst())
                        a = a->Eval(aNULL, aNULL, idthread);
                    lsp.push(a);
                    a->Release();
                }
                else
                    lsp.values.push_back(aNULL);
            }
            
            Tamgulisp* l = globalTamgu->Providelisp();
            Tamgu* e;
            Tamgulisp quoted(-1);
            quoted.Setaction(a_quote);
            quoted.Push(aNULL);
            
            lsp.values[1] = &quoted;

            for (i = 0; i < v1->Size(); i++) {
                e = v1->getvalue(i);
                quoted.values[1] = e;
                
                e->Setreference();
                a = lsp.Eval(aNULL, aNULL, idthread);
                if (a->Boolean() == true)
                    l->push(e);
                e->Resetreference();
                a->Release();
            }
            if (lsp.values.size()==3)
                lsp.values[2]->Resetreference();
            v1->Release();
            v0->Release();
            return l;
        }
        case a__takewhile:
        {
            //First element is the operation, second element the list
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            if (v0->isFunction())
                v0 = values[1];
            else {
                if (!v0->isLisp() && !globalTamgu->checkoperator(v0->Action()))
                    return globalTamgu->Returnerror(e_wrong_list_of03, idthread);
            }

            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v1);
            if (!v1->isVectorContainer()) {
                v1->Release();
                return globalTamgu->Returnerror(e_wrong_list_of10, idthread);
            }
            //We prepare our solution... Either the content of v0 is one or two
            
            Tamgulisp lsp(-1);
            sz = v0->Size();
            if (sz > 2) {
                lsp.push(v0);
                lsp.values.push_back(aNULL);
            }
            else {
                lsp.pushatom(((Tamgulisp*)v0)->values[0]);
                if (sz == 2) {
                    lsp.values.push_back(aNULL);
                    a = ((Tamgulisp*)v0)->values[1];
                    if (!a->isConst())
                        a = a->Eval(aNULL, aNULL, idthread);
                    lsp.push(a);
                    a->Release();
                }
                else
                    lsp.values.push_back(aNULL);
            }
            
            Tamgulisp* l = globalTamgu->Providelisp();
            Tamgu* e;
            Tamgulisp quoted(-1);
            quoted.Setaction(a_quote);
            quoted.Push(aNULL);
            
            lsp.values[1] = &quoted;

            for (i = 0; i < v1->Size(); i++) {
                e = v1->getvalue(i);
                quoted.values[1] = e;
                
                e->Setreference();
                a = lsp.Eval(aNULL, aNULL, idthread);
                if (a->Boolean() == true) {
                    l->push(e);
                    e->Resetreference();
                    a->Release();
                }
                else {
                    e->Resetreference();
                    a->Release();
                    break;
                }
            }
            if (lsp.values.size()==3)
                lsp.values[2]->Resetreference();
            v1->Release();
            v0->Release();
            return l;
        }
        case a__dropwhile:
        {
            //First element is the operation, second element the list
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            if (v0->isFunction())
                v0 = values[1];
            else {
                if (!v0->isLisp() && !globalTamgu->checkoperator(v0->Action()))
                    return globalTamgu->Returnerror(e_wrong_list_of05, idthread);
            }

            v1 = values[2]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v1);
            if (!v1->isVectorContainer()) {
                v1->Release();
                return globalTamgu->Returnerror(e_wrong_list_of08, idthread);
            }
            //We prepare our solution... Either the content of v0 is one or two
            
            Tamgulisp lsp(-1);
            sz = v0->Size();
            if (sz > 2) {
                lsp.push(v0);
                lsp.values.push_back(aNULL);
            }
            else {
                lsp.pushatom(((Tamgulisp*)v0)->values[0]);
                if (sz == 2) {
                    lsp.values.push_back(aNULL);
                    a = ((Tamgulisp*)v0)->values[1];
                    if (!a->isConst())
                        a = a->Eval(aNULL, aNULL, idthread);
                    lsp.push(a);
                    a->Release();
                }
                else
                    lsp.values.push_back(aNULL);
            }
            
            Tamgulisp* l = globalTamgu->Providelisp();
            Tamgu* e;
            Tamgulisp quoted(-1);
            quoted.Setaction(a_quote);
            quoted.Push(aNULL);
            
            lsp.values[1] = &quoted;

            bool accept=false;
            for (i = 0; i < v1->Size(); i++) {
                e = v1->getvalue(i);
                quoted.values[1] = e;
                
                e->Setreference();
                a = lsp.Eval(aNULL, aNULL, idthread);

                if (!accept && a->Boolean() == true) {
                    a->Releasenonconst();
                    e->Resetreference();
                    continue;
                }
                accept=true;
                l->push(e);
                e->Resetreference();
                a->Release();
            }
            if (lsp.values.size()==3)
                lsp.values[2]->Resetreference();
            v1->Release();
            v0->Release();
            return l;
        }
        case a__zip:
        {
            //We combine different lists together...
            vector<Tamgu*> listes;
            long szl = -1;
            for (i = 1; i < sz; i++) {
                v1 = values[i]->Eval(contextualpattern, aNULL, idthread);
                if (v1->isError() || !v1->isVectorContainer()) {
                    for (i = 0; i < listes.size(); i++)
                        listes[i]->Releasenonconst();
                    if (!v1->isError()) {
                        v1->Release();
                        return globalTamgu->Returnerror(e__zip_accepts_only, idthread);
                    }
                    return v1;
                }
                if (i == 1)
                    szl = v1->Size();
                else {
                    if (szl != v1->Size()) {
                        for (i = 0; i < listes.size(); i++)
                            listes[i]->Releasenonconst();
                        return globalTamgu->Returnerror(e_lists_should_all02, idthread);
                    }
                }
                listes.push_back(v1);
            }
            Tamgulisp* l = globalTamgu->Providelisp();
            Tamgulisp* sub;
            for (long j = 0; j < szl; j++) {
                sub = globalTamgu->Providelisp();
                l->push(sub);
                for (i = 0; i <listes.size(); i++)
                    sub->push(listes[i]->getvalue(j));
            }
            for (i = 0; i < listes.size(); i++)
                listes[i]->Releasenonconst();
            return l;
        }
        case a__zipwith:
        {
            //We combine different lists together with an operator
            //First element is the operation
            v0 = values[1]->Eval(aNULL, aNULL, idthread);
            checkerror(v0);
            if (v0->isFunction())
                v0 = values[1];
            else {
                if (!v0->isLisp() && !globalTamgu->checkoperator(v0->Action()))
                    return globalTamgu->Returnerror(e_wrong_list_of02, idthread);
            }

            vector<Tamgu*> listes;
            long szl = -1;
            for (i = 2; i < sz; i++) {
                v1 = values[i]->Eval(contextualpattern, aNULL, idthread);
                if (v1->isError() || !v1->isVectorContainer()) {
                    for (i = 0; i < listes.size(); i++)
                        listes[i]->Releasenonconst();
                    if (!v1->isError()) {
                        v1->Release();
                        return globalTamgu->Returnerror(e__zipwith_accepts_only, idthread);
                    }
                    return v1;
                }
                if (i == 2)
                    szl = v1->Size();
                else {
                    if (szl != v1->Size()) {
                        for (i = 0; i < listes.size(); i++)
                            listes[i]->Releasenonconst();
                        return globalTamgu->Returnerror(e_lists_should_all, idthread);
                    }
                }
                listes.push_back(v1);
            }

            //First element is the operation, second element the list
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            if (v0->isFunction()) // in this case it is a lambda, we can simply keep it
                v0 = values[1];
            else {
                if (!v0->isLisp() && !globalTamgu->checkoperator(v0->Action()))
                    return globalTamgu->Returnerror(e_wrong_list_of09, idthread);
            }

            long j;
            Tamgulisp lsp(-1);
            if (v0->isLisp()) {
                sz = v0->Size();
                //lambda or function
                if (sz == 1)
                    lsp.pushatom(((Tamgulisp*)v0)->values[0]);
                else
                    lsp.pushatom(v0);
                for (j = 0; j < listes.size(); j++)
                    lsp.values.push_back(aNULL);
            }
            else {
                lsp.values.push_back(v0);
                for (j = 0; j < listes.size(); j++)
                    lsp.values.push_back(aNULL);
            }

            Tamgulisp* l = globalTamgu->Providelisp();
            for (j = 0; j < szl; j++) {
                for (i = 0; i <listes.size(); i++) {
                    lsp.values[i+1] = listes[i]->getvalue(j);
                    lsp.values[i+1]->Setreference();
                }
                a = lsp.Eval(aNULL, aNULL, idthread);
                l->push(a);
                a->Releasenonconst();
                for (i = 0; i <listes.size(); i++)
                    lsp.values[i+1]->Resetreference();
            }
            
            for (i = 0; i < listes.size(); i++)
                listes[i]->Releasenonconst();
            return l;
        }
        case a__foldl:
        case a__scanl:
        case a__foldr:
        case a__scanr:
            v1 = values[3]->Eval(contextualpattern,aNULL, idthread);
            if (!v1->isVectorContainer()) {
                v1->Release();
                return globalTamgu->Returnerror(e_expecting_a_list, idthread);
            }
            a = values[2]->Eval(contextualpattern,aNULL, idthread);
            //a will be our first value
        case a__foldl1:
        case a__scanl1:
        case a__foldr1:
        case a__scanr1:
        {
            if (v1 == NULL) {
                //The first value will be the first element of the list
                v1 = values[2]->Eval(contextualpattern,aNULL, idthread);
                if (!v1->isVectorContainer()) {
                    v1->Release();
                    return globalTamgu->Returnerror(e_expecting_a_list, idthread);
                }
                if (v1->Size() == 0) {
                    if (n == a__scanl1 || n == a__scanr1)
                        return aEMPTYLISP;
                    return aNULL;
                }
                if (n >= a__foldr1)
                    a = v1->getvalue(v1->Size()-1); //we take the last one
                else
                    a = v1->getvalue(0);
            }
            
            long szl = v1->Size();
            
            v0 = values[1]->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            if (v0->isFunction())
                v0 = values[1];
            else {
                if (!v0->isLisp() && !globalTamgu->checkoperator(v0->Action()))
                    return globalTamgu->Returnerror(e_wrong_list_of, idthread);
            }
        
            Tamgulisp lsp(-1);
            if (v0->isLisp()) {
                sz = v0->Size();
                //lambda or function
                if (sz == 1)
                    lsp.pushatom(((Tamgulisp*)v0)->values[0]);
                else
                    lsp.pushatom(v0);
            }
            else
                lsp.values.push_back(v0);
            
            lsp.push(aNULL);
            lsp.push(aNULL);
            
            Tamgulisp* l;
            i = 0;
            a->Setreference();
            switch (n) {
                case a__foldl1:
                    i = 1;
                case a__foldl:
                    lsp.values[1] = a;
                    for (; i < szl; i++) {
                        lsp.values[2] = v1->getvalue(i);
                        a = lsp.Eval(aNULL, aNULL, idthread);
                        if (a != lsp.values[1]) {
                            lsp.values[1]->Resetreference();
                            a->Setreference();
                            lsp.values[1] = a;
                        }
                    }
                    v1->Release();
                    a->Protect();
                    return a;
                case a__foldr1:
                    i = szl-2;
                case a__foldr:
                    if (!i)
                        i = szl-1;
                    lsp.values[2] = a;
                    for (;i >= 0; i--) {
                        lsp.values[1] = v1->getvalue(i);
                        a = lsp.Eval(aNULL, aNULL, idthread);
                        if (a != lsp.values[2]) {
                            lsp.values[2]->Resetreference();
                            a->Setreference();
                            lsp.values[2] = a;
                        }
                    }
                    v1->Release();
                    a->Protect();
                    return a;
                case a__scanl1:
                    i = 1;
                case a__scanl:
                    lsp.values[1] = a;
                    l = globalTamgu->Providelisp();
                    l->push(a);
                    for (; i < szl; i++) {
                        lsp.values[2] = v1->getvalue(i);
                        a = lsp.Eval(aNULL, aNULL, idthread);
                        l->push(a);
                        if (a != lsp.values[1]) {
                            lsp.values[1]->Resetreference();
                            a->Setreference();
                            lsp.values[1] = a;
                        }
                    }
                    a->Resetreference();
                    v1->Release();
                    return l;
                case a__scanr1:
                    i = szl-2;
                case a__scanr:
                    if (!i)
                        i = szl-1;
                    lsp.values[2] = a;
                    l = globalTamgu->Providelisp();
                    l->push(a);
                    for (;i >= 0; i--) {
                        lsp.values[1] = v1->getvalue(i);
                        a = lsp.Eval(aNULL, aNULL, idthread);
                        l->push(a);
                        if (a != lsp.values[2]) {
                            lsp.values[2]->Resetreference();
                            a->Setreference();
                            lsp.values[2] = a;
                        }
                    }
                    a->Resetreference();
                    v1->Release();
                    return l->Localreverse();
            }
            return aNULL;
        }
        case a_lisp:
        {
            v0 = a->Eval(contextualpattern, aNULL, idthread);
            checkerror(v0);
            if (v0->isFunction()) {
                if (v0->Type() == a_lisp) {
                    TamguCallLispFunction call(v0);
                    
                    for (i = 1; i < sz; i++)
                        call.arguments.push_back(values[i]);
                    
                    return call.Eval(contextualpattern, aNULL, idthread);
                }
                else {
                    TamguCallFunction call(v0);
                    
                    for (i = 1; i < sz; i++)
                        call.arguments.push_back(values[i]);
                    
                    return call.Eval(contextualpattern, aNULL, idthread);
                }
            }

            
            if (sz == 1)
                return a->Eval(contextualpattern, aNULL, idthread);
            
            Tamgulispcode tl(NULL);
            tl.pushatom(v0);
            for (i = 1; i < sz; i++)
                tl.pushatom(values[i]);
            a = tl.Eval(contextualpattern, aNULL, idthread);
            v0->Releasenonconst();
            return a;
        }
    }
    
    return aNULL;
}

string Tamgulisp::String() {
    string res;
    long it;
    res = "(";
    bool beg = true;
    string sx;
    Tamgu* element;
    for (it = 0; it < values.size(); it++) {
        element = values[it];
        sx = element->StringToDisplay();
        if (!element->isString() || element->isContainer()) {
            if (sx.empty())
                sx = "''";
            if (beg == false)
                res += " ";
            res += sx;
        }
        else {
            if (beg == false)
                res += " ";
            jstringing(res, sx);
        }
        beg = false;

    }
    res += ")";
    return res;
}


string Tamgulispair::String() {
    string res;
    long it;
    res = "(";
    bool beg = true;
    string sx;
    Tamgu* element;
    long sz = (long)values.size() - 1;
    for (it = 0; it <= sz; it++) {
        if (it == sz)
            res += " .";
        element = values[it];
        sx = element->StringToDisplay();
        if (!element->isString() || element->isContainer()) {
            if (sx.empty())
                sx = "''";
            if (beg == false)
                res += " ";
            res += sx;
        }
        else {
            if (beg == false)
                res += " ";
            jstringing(res, sx);
        }
        beg = false;

    }
    res += ")";
    return res;
}

void Tamgulisp::Setstring(string& res, short idthread) {
    long it;
    res = "(";
    bool beg = true;
    string sx;
    Tamgu* element;
    for (it = 0; it < values.size(); it++) {
        element = values[it];
        sx = element->StringToDisplay();
        if (!element->isString() || element->isContainer()) {
            if (sx.empty())
                sx = "''";
            if (beg == false)
                res += " ";
            res += sx;
        }
        else {
            if (beg == false)
                res += " ";
            jstringing(res, sx);
        }
        beg = false;

    }
    res += ")";
}


void Tamgulispair::Setstring(string& res, short idthread) {
    long it;
    res = "(";
    bool beg = true;
    string sx;
    Tamgu* element;
    long sz = (long)values.size() - 1;
    for (it = 0; it <= sz; it++) {
        if (it == sz)
            res += " .";
        element = values[it];
        sx = element->StringToDisplay();
        if (!element->isString() || element->isContainer()) {
            if (sx.empty())
                sx = "''";
            if (beg == false)
                res += " ";
            res += sx;
        }
        else {
            if (beg == false)
                res += " ";
            jstringing(res, sx);
        }
        beg = false;

    }
    res += ")";
}



Tamgu* TamguCallLispFunction::Eval(Tamgu* domain, Tamgu* a, short idthread) {
    TamguFunction* bd = (TamguFunction*)body;
    size_t sz = bd->parameters.size();

    if (arguments.size() != sz) {
        string msg = "Wrong number of arguments in call to: '";
        msg +=  globalTamgu->Getsymbol(name);
        msg += "'";
        return globalTamgu->Returnerror(msg, idthread);
    }

    TamguDeclarationLocal* environment = globalTamgu->Providedeclaration(idthread);
    if (globalTamgu->debugmode)
        environment->idinfo = Currentinfo();

    TamguVariableDeclaration* p;

    for (size_t i = 0; i < sz; i++) {
        p = (TamguVariableDeclaration*)bd->parameters[i];
        a = arguments[i]->Eval(domain, aNULL, idthread);
        if (p->Setarguments(environment, a, idthread, false)) {
            a->Releasenonconst();
            environment->Release();
            string err = "Check the arguments of: ";
            err += globalTamgu->Getsymbol(Name());
            return globalTamgu->Returnerror(err, idthread);
        }
        
        a->Releasenonconst();
    }
    
    if (!globalTamgu->Pushstacklisp(this, idthread))
        return globalTamgu->Returnerror(e_stack_overflow, idthread);

    globalTamgu->Pushstackraw(environment, idthread);
    //We then apply our function within this environment
    a = bd->Eval(environment, aNULL, idthread);
    globalTamgu->Popstack(idthread);
    globalTamgu->Popstacklisp(idthread);

    //if a has no reference, then it means that it was recorded into the environment
    if (!a->Reference())
        environment->Releasing();
    else {
        a->Setreference();
        //we clean our structure...
        environment->Releasing();
        a->Protect();
    }

    return a;
}

