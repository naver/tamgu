/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgutaskell.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "instructions.h"
#include "compilecode.h"
#include "tamgustring.h"
#include "tamgusvector.h"
#include "constobjects.h"
#include "tamgumap.h"
#include "tamguivector.h"
#include "tamgufvector.h"
#include "tamgusvector.h"
#include "tamgutaskell.h"
#include "tamguframeinstance.h"
#include "tamgumapi.h"

//------------------------------HASKELL----------------------------------------

Exporting basebin_hash<tamguCallFibre>  TamguCallFibre::methods;
Exporting hmap<string, string> TamguCallFibre::infomethods;
Exporting basebin_hash<unsigned long> TamguCallFibre::exported;

Exporting short TamguCallFibre::idtype = 0;

    //MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void TamguCallFibre::AddMethod(TamguGlobal* global, string name, tamguCallFibre func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void TamguCallFibre::Setidtype(TamguGlobal* global) {
    TamguCallFibre::InitialisationModule(global,"");
}


   bool TamguCallFibre::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();
    
    
    TamguCallFibre::idtype = global->Getid("fibre");
    
    TamguCallFibre::AddMethod(global, "run", &TamguCallFibre::MethodRun, P_NONE, "run(): execute fibres.");
    TamguCallFibre::AddMethod(global, "block", &TamguCallFibre::MethodBlock, P_NONE, "block(): execute fibres up to the current tail.");
    TamguCallFibre::AddMethod(global, "unblock", &TamguCallFibre::MethodUnblock, P_NONE, "unblock(): remove the current limit on execution.");

    if (version != "") {
        global->newInstance[TamguCallFibre::idtype] = new TamguCallFibre(global);
        global->RecordMethods(TamguCallFibre::idtype, TamguCallFibre::exported);
    }
    
    return true;
}

//------------------------------TASKELL ENVIRONMENT----------------------------------------

Exporting TamguDeclarationAutoClean* TamguGlobal::Providedeclarationclean(short idthread) {
    TamguDeclarationAutoClean* ke;
    if (threadMODE || Addtogarbage()) {
        ke = new TamguDeclarationAutoClean(-1);
        ke->idthread = idthread;
        return ke;
    }


    if (declarationcleanempties.last > 0) {
        ke = declarationcleanreservoire[declarationcleanempties.backpop()];
        ke->used = true;
        ke->idthread = idthread;
        return ke;
    }
    

    long mx = declarationcleanreservoire.size();
    
    while (declarationcleanidx < mx) {
        if (!declarationcleanreservoire[declarationcleanidx]->used) {
            ke = declarationcleanreservoire[declarationcleanidx++];
            ke->used = true;
            ke->idthread = idthread;
            return ke;
        }
        declarationcleanidx++;
    }
    
    long sz = mx >> 1;
    declarationcleanreservoire.resize(mx + sz);
    declarationcleanidx = mx + sz;
    for (long i = mx; i < declarationcleanidx; i++)
        declarationcleanreservoire[i] = new TamguDeclarationAutoClean(i);
    
    declarationcleanidx = mx;
    ke = declarationcleanreservoire[declarationcleanidx++];
    ke->used = true;
    ke->idthread = idthread;
    return ke;
}


//------------------------------TASKELL----------------------------------------
Tamgu* TamguTaskellSelfVariableDeclaration::Eval(Tamgu* a, Tamgu* value, short idthread) {
    switch (directcall) {
        case 0:
            return ((TamguDeclarationAutoClean*)a)->Declarationself(name);
        case 1:
            if (initialization == NULL) {
                directcall = 0;
                return ((TamguDeclarationAutoClean*)a)->Declarationself(name);
            }
            directcall = 2;
        case 2:
            a = ((TamguDeclarationAutoClean*)a)->Declarationself(name);
            value = initialization->Eval(aNULL, aNULL, idthread);
            if (a != value) {
                if (value->isError())
                    return value;
                a->Putvalue(value, idthread);
                value->Releasenonconst();
            }
    }
    
    return a;
}

Tamgu* TamguTaskellSelfVariableDeclaration::Put(Tamgu* domain, Tamgu* value, short idthread) {
    return ((TamguDeclarationAutoClean*)domain)->Declarationself(name);
}

Tamgu* TamguTaskellVariableDeclaration::Eval(Tamgu* a, Tamgu* value, short idthread) {
    //we create our instance...
    switch (directcall) {
        case 0:
            return ((TamguDeclarationAutoClean*)a)->Declarationvalue(name, aNULL, typevariable);
        case 1:
            if (initialization == NULL) {
                directcall = 0;
                return ((TamguDeclarationAutoClean*)a)->Declarationvalue(name, aNULL, typevariable);
            }
            directcall = initialization->isCallTaskellFunction();
            if (!directcall)
                directcall = 2;
            else {
                if (directcall == 3) {
                    if (((TamguCallFunctionTaskell*)initialization)->body->returntype == a_null)
                        ((TamguCallFunctionTaskell*)initialization)->body->returntype = typevariable;
                }
                else {
                    TamguCallFunctionArgsTaskell* call = (TamguCallFunctionArgsTaskell*)initialization;
                    TamguFunctionLambda* bd = call->body;
                    while (bd != NULL) {
                        if (bd->returntype == a_null)
                            bd->returntype = typevariable;
                        bd=bd->Following();
                    }
                }
            }
        case 2:
            value = initialization->Eval(aNULL, aNULL, idthread);
            if (value->isError())
                return value;
            return ((TamguDeclarationAutoClean*)a)->Declarationvalue(name, value, typevariable);
        case 3:
            value = ((TamguCallFunctionTaskell*)initialization)->Eval(aNULL, aNULL, idthread);
            if (value->isError())
                return value;

            return ((TamguDeclarationAutoClean*)a)->Declarationvalue(name, value, typevariable);
        case 4:
            value = ((TamguCallFunctionArgsTaskell*)initialization)->Eval(aNULL, aNULL, idthread);
            if (value->isError())
                return value;
            
            return ((TamguDeclarationAutoClean*)a)->Declarationvalue(name, value, typevariable);
    }
    return a;
}

short TamguCallFunctionArgsTaskell::Typeinfered() {
    if (body != NULL) {
        short ref = body->Typeinfered();
        TamguFunctionLambda* bd = body->Following();
        short ty;
        while (bd != NULL) {
            ty = bd->Typeinfered();
            if (ref == ty || ty == a_none) {
                bd = bd->Following();
                continue;
            }

            if (Isnumber(ref)) {
                if (Isnumber(ty)) {
                    if (ref < ty) {
                        if (ref == a_long)
                            ref = a_float;
                    }
                }
                else {
                    ref = a_none;
                    break;
                }
            }
            else
                if (Istring(ref)) {
                    if (!Istring(ty)) {
                        ref = a_none;
                        break;
                    }
                }
                else {
                    ref = a_none;
                    break;
                }
            bd = bd->Following();
        }
        return ref;
    }
    return a_none;
}

short TamguCallFunctionTaskell::Typeinfered() {
    if (body->returntype != a_null)
        return body->returntype;
    return a_none;
}

bool TamguCallFunctionTaskell::Checkarity() {
    TamguFunctionLambda* bd = body;
    bool compatible=false;
    while (bd != NULL) {
        if (bd->Puretaskelldeclaration()) {//A hdeclared description
            bd = bd->Following();
            continue;
        }
        
        if (arguments.size() == bd->parameters.size()) {
            if (bd->hdeclared) {
                bool found = true;
                short i;
                //In this case, we compare against the Taskell function declaration...
                //Which might be different from the local body arguments themselves...
                for (i = 0; i < bd->taskelldeclarations.size(); i++) {
                    if (!globalTamgu->Compatiblestrict(bd->taskelldeclarations[i]->Type(), arguments[i]->Typeinfered())) {
                        found = false;
                        break;
                    }
                }
                
                if (found) {
                    compatible=true;
                    //we can start at this specific function point...
                    //Now we test against the arguments of the function...
                    found = true;
                    for (i = 0; i < bd->taskelldeclarations.size(); i++) {
                        if (!globalTamgu->Compatiblestrict(bd->parameters[i]->Type(), arguments[i]->Typeinfered())) {
                            found = false;
                            break;
                        }
                    }
                    
                    if (!found) {
                        bd = bd->Following();
                        continue;
                    }
                    
                    body = bd;
                    return true;
                }
            }
            else
                return true;
        }
        bd = bd->Following();
    }
    return compatible;
}


Tamgu* TamguFunctionTaskellParameter::Execute(Tamgu* context, Tamgu* hcall, short idthread) {
    Tamgu* defcall = globalTamgu->Getdefinition(name, idthread, context);

    long sz = defcall->Size();
    if (sz != hcall->Size()) {
        stringstream err;
        err << "Error: mismatch argument in a taskell function call to: " << globalTamgu->Getsymbol(name);
        return globalTamgu->Returnerror(err.str(), idthread);
    }

    if (!sz) {
        TamguCallFunctionTaskell hfunc((TamguFunctionLambda*)defcall);
        return hfunc.Eval(context, aNULL, idthread);
    }


    TamguCallFunctionArgsTaskell hfunc((TamguFunctionLambda*)defcall);
    for (int i = 0; i < sz; i++)
        hfunc.arguments.push_back(hcall->Argument(i));
    return hfunc.Eval(context, aNULL, idthread);

}

long TamguFunctionTaskellParameter::Size() {
    Tamgu* defcall = globalTamgu->Getdefinition(name, 0, aNULL);
    return defcall->Size();
}

Tamgu* TamguFunctionTaskellParameter::Eval(Tamgu* context, Tamgu* val, short idthread) {
    return globalTamgu->Getdefinition(name, idthread, context);
}

Tamgu* TamguFrameMethodParameter::Eval(Tamgu* context, Tamgu* object, short idthread) {
    return this;
}

Tamgu* TamguFunctionParameter::Eval(Tamgu* context, Tamgu* val, short idthread) {
    return globalTamgu->Getdefinition(name, idthread, context);
}

Tamgu* TamguMethodParameter::Eval(Tamgu* context, Tamgu* object, short idthread) {
    return this;
}

Tamgu* TamguCommonParameter::Eval(Tamgu* context, Tamgu* object, short idthread) {
    return this;
}

Tamgu* TamguProcedureParameter::Eval(Tamgu* context, Tamgu* v, short idthread) {
    return this;
}


Tamgu* TamguCallSQUARE::Eval(Tamgu* context, Tamgu* value, short idthread) {
    value = globalTamgu->Getdeclaration(name, idthread);
    return value->multiply(value, false);
}

Tamgu* TamguCallCUBE::Eval(Tamgu* context, Tamgu* value, short idthread) {
    value = globalTamgu->Getdeclaration(name, idthread);
    value = value->multiply(value, false);
    return value->multiply(value, true);
}

Tamgu* TamguFunctionLambda::DirectEval(Tamgu* environment, Tamgu* a, short idthread) {
    if (!returnonly)
        Checkreturnonly();
    
    bool testcond = false;
    switch(returnonly) {
        case 1:
            a = instructionreturn->DirectEval(environment, aNULL, idthread);
            break;
        case 2:
            a = instructionif->DirectEval(environment, aNULL, idthread);
            break;
        case 3:
            a = instructions.vecteur[0]->Eval(environment, aNULL, idthread);
            break;
        case 4:
            a = aNULL;
            for (short i = 0; i < instructions.last && !testcond; i++) {
                a->Releasenonconst();
                a = instructions.vecteur[i]->Eval(environment, aNULL, idthread);
                testcond = a->isError();
            }
            
            if (testcond)
                return a;
            else {
                a->Releasenonconst();
                a = instructionreturn->DirectEval(environment, aNULL, idthread);
            }
            break;
        default:
            a = aNULL;
            for (short i = 0; i < instructions.last && !testcond; i++) {
                a->Releasenonconst();
                a = instructions.vecteur[i]->Eval(environment, aNULL, idthread);
                testcond = a->needFullInvestigate();
            }
            
            if (!testcond) {
                a->Releasenonconst();
                a = aNULL;
            }
    }
    
    if (a->needFullInvestigate()) {
        if (a->isError())
            return a;
        
        a = a->Returned(idthread);

        if (globalTamgu->Error(idthread)) {
            a->Release();
            return globalTamgu->Errorobject(idthread);
        }
        
        if (!hdeclared || store || a->needInvestigate())
            return a;
        
        short atype = a->Type();
        if (returntype != a_null && returntype != atype) {
            if (globalTamgu->Compatiblestrict(returntype, atype)) {
                //The test atype < returntype is a simple way to tackle number definition order: short < int < long < decimal < float
                if (returntype == a_call || (Isnumber(returntype) && atype < returntype) || Istring(returntype) || a->isFrameinstance())
                    return a;
                
                //In the case when the types are compatible but different, we force the result to be of returntype
                environment = globalTamgu->newInstance.get(returntype)->Newinstance(idthread);
                environment->Putvalue(a, idthread);
                a->Releasenonconst();
                return environment;
            }
            
            if (globalTamgu->debugmode) {
                string s = "Return value does not match declaration... Expected: '";
                s += globalTamgu->Getsymbol(returntype);
                s += "' ... Returned: '";
                s += globalTamgu->Getsymbol(atype) + "'";
                globalTamgu->threads[idthread].nonblockingerror = s;
            }
            
            a->Releasenonconst();
            return aRAISEERROR;
        }
        return a;
    }
    
    a->Releasenonconst();
    return aNULL;
}

Tamgu* TamguFunctionLambda::Eval(Tamgu* environment, Tamgu* a, short idthread) {
    if (!returnonly)
        Checkreturnonly();

    bool testcond = false;
    
    switch(returnonly) {
        case 1:
            a = instructionreturn->DirectEval(environment, aNULL, idthread);
            break;
        case 2:
            a = instructionif->DirectEval(environment, aNULL, idthread);
            break;
        case 3:
            a = instructions.vecteur[0]->Eval(environment, aNULL, idthread);
            break;
        case 4:
            a = aNULL;
            for (short i = 0; i < instructions.last && !testcond; i++) {
                a->Releasenonconst();
                a = instructions.vecteur[i]->Eval(environment, aNULL, idthread);
                testcond = a->isError();
            }
            if (testcond)
                return a;
            else {
                a->Releasenonconst();
                a = instructionreturn->DirectEval(environment, aNULL, idthread);
            }
            break;
        default:
            a = aNULL;
            for (short i = 0; i < instructions.last && !testcond; i++) {
                a->Releasenonconst();
                a = instructions.vecteur[i]->Eval(environment, aNULL, idthread);
                
                testcond = a->needFullInvestigate();
            }
            
            if (!testcond) {
                a->Releasenonconst();
                a = aNULL;
            }
    }


    if (a->needFullInvestigate()) {
        if (a->isError())
            return a;
        
        a = a->Returned(idthread);
        
        if (globalTamgu->Error(idthread)) {
            a->Release();
            return globalTamgu->Errorobject(idthread);
        }
        
        if (!hdeclared || store || a->needInvestigate())
            return a;

        short atype = a->Type();
        if (returntype != a_null && returntype != atype) {
            if (globalTamgu->Compatiblestrict(returntype, atype)) {
                //The test atype < returntype is a simple way to tackle number definition order: short < int < long < decimal < float
                if (returntype == a_call || (Isnumber(returntype) && atype < returntype) || Istring(returntype) || a->isFrameinstance())
                    return a;
                
                //In the case when the types are compatible but different, we force the result to be of returntype
                environment = globalTamgu->newInstance.get(returntype)->Newinstance(idthread);
                environment->Putvalue(a, idthread);
                a->Releasenonconst();
                return environment;
            }
            
            if (globalTamgu->debugmode) {
                string s = "Return value does not match declaration... Expected: '";
                s += globalTamgu->Getsymbol(returntype);
                s += "' ... Returned: '";
                s += globalTamgu->Getsymbol(atype) + "'";
                globalTamgu->threads[idthread].nonblockingerror = s;
            }
            
            a->Releasenonconst();
            return aRAISEERROR;
        }
        return a;
    }
    
    a->Releasenonconst();
    return aNULL;
}


//------------------------------HASKELL----------------------------------------
Tamgu* TamguInstructionHASKELLCASE::Eval(Tamgu* var, Tamgu* context, short idthread) {
    var = instructions[0]->Eval(context, aNULL, idthread);
    Tamgu* ins = instructions[1];
    if (ins->isConstContainer()) {
        ins->Prepare(context, idthread);
        context = ins->same(var);
        var->Releasenonconst();
        return context;
    }

    ins = ins->Eval(context, aNULL, idthread);
    context = ins->same(var);
    var->Releasenonconst();
    ins->Releasenonconst();
    return context;
}


Tamgu* TamguInstructionTaskellMainCASE::Eval(Tamgu* context, Tamgu* value, short idthread) {
    bool testcond = false;
    long i = 0;
    for (; i < (instructions.last - other) && !testcond; i += 2) {
        value = instructions.vecteur[i]->Eval(aTRUE, context, idthread);
        testcond = value->needInvestigate() || value->Boolean();
    }
    
    if (testcond) {
        if (value->needInvestigate())
            return value;
        else
            return instructions.vecteur[i - 1]->Eval(context, aNULL, idthread);
    }

    if (other)
        return instructions.vecteur[instructions.last - 1]->Eval(context, aNULL, idthread);

    return aNULL;
}

Tamgu* TamguInstructionTaskellIF::DirectEval(Tamgu* context, Tamgu* value, short idthread) {
    //if (!compiled) {
    //	instructions.vecteur[0] = instructions.vecteur[0]->Compile(aTRUE);
    //	compiled = true;
    //}

    value = instructions.vecteur[0]->Eval(aTRUE, context, idthread);

    if (value->needInvestigate())
        return value;

    bool truevalue = value->Boolean();
    value->Releasenonconst();

    if (truevalue != negation) {
        if (callreturn)
            return returncall->DirectEval(context, aNULL, idthread);
        
        return instructions.vecteur[1]->Eval(context, aNULL, idthread);
    }
    if (instructions.last == 3)
        return instructions.vecteur[2]->Eval(context, aNULL, idthread);

    return aNULL;
}

Tamgu* TamguInstructionTaskellIF::Eval(Tamgu* context, Tamgu* value, short idthread) {
    //if (!compiled) {
    //    instructions.vecteur[0] = instructions.vecteur[0]->Compile(aTRUE);
    //    compiled = true;
    //}
    
    value = instructions.vecteur[0]->Eval(aTRUE, context, idthread);
    
    if (value->needInvestigate())
        return value;
    
    bool truevalue = value->Boolean();
    value->Releasenonconst();
    
    if (truevalue != negation) {
        if (callreturn)
            return returncall->DirectEval(context, aNULL, idthread);
        
        return instructions.vecteur[1]->Eval(context, aNULL, idthread);
    }
    if (instructions.last == 3)
        return instructions.vecteur[2]->Eval(context, aNULL, idthread);
    
    return aNULL;
}

Tamgu* TamguHBloc::Eval(Tamgu* context, Tamgu* a, short idthread) {
    short size = instructions.last;

    Tamgu* environment = context;
    
    if (variablesWillBeCreated) {
        environment = globalTamgu->Providedeclaration(idthread);
        if (globalTamgu->debugmode)
            ((TamguDeclarationLocal*)environment)->idinfo = Currentinfo();
        globalTamgu->Pushstackraw(environment, idthread);
        ((TamguDeclarationLocal*)environment)->pushed = true;
    }

    a = aNULL;
    bool testcond = false;
    for (short i = 0; i < size && !testcond; i++) {
        a->Releasenonconst();
        a = instructions[i];

        a = a->Eval(environment, aNULL, idthread);

        testcond = a->needFullInvestigate();
    }
    
    if (testcond) {
        if (!variablesWillBeCreated)
            return a->Returned(idthread);
        
        if (environment->isEmpty()) {
            environment->Release();
            return a->Returned(idthread);
        }
        
        context = a->Returned(idthread);
        if (!context->Reference())
            environment->Release();
        else {
            context->Setreference();
            environment->Release();
            context->Protect();
        }
        return context;
    }

    a->Releasenonconst();
    if (variablesWillBeCreated)
        environment->Release();

    return aNULL;
}

//------------------------------HASKELL----------------------------------------
Tamgu* TamguCallFunctionTaskell::Eval(Tamgu* context, Tamgu* res, short idthread) {
    TamguDeclarationAutoClean* environment = globalTamgu->Providedeclarationclean(idthread);
    
    if (globalTamgu->debugmode)
        globalTamgu->threads[idthread].nonblockingerror = "";

    for (long i = 0; i < body->names.size(); i++)
        body->localvariables[i]->Eval(environment, aHASKELL, idthread);

    switch (body->lambdadomain->instructions.last) {
        case 0:
        res = body->DirectEval(environment, aNULL, idthread);
        break;
        case 2:
        res = GetTaskell2(context, environment, body, idthread);
        break;
        case 3:
        res = GetTaskell3(context, environment, body, idthread);
        break;
        case 5:
        res = GetTaskell5(context, environment, body, idthread);
        break;
        default:
        res = GetTaskellN(context, environment, body, idthread);
    }


    if (res->isError()) {
        environment->Cleanenvironment();

        if (body->maybe) {
            globalTamgu->Cleanerror(idthread);
            return aNOTHING;
        }
        
        globalTamgu->Current(this, idthread);
        if (res == aRAISEERROR && context->Type() != a_taskelldeclaration)
            return globalTamgu->Returnerror("Lambda expression failed...", idthread);
        return res;
    }

    if (!environment->isEmpty() && res->Reference()) {
        res->Setreference();
        environment->Cleaning();
        res->Protect();
    }

    environment->Cleanenvironment();
    
    return res;
}

Tamgu* TamguCallFunctionArgsTaskell::Eval(Tamgu* context, Tamgu* res, short idthread) {

    Tamgu* argms[3];
    Tamgu** args=argms;

    short i, sz = arguments.size();

    if (sz>3)
        args = new Tamgu*[sz];

    if (curryfied) {
        TamguGetCurryfiedFunction* func = new TamguGetCurryfiedFunction(body);
        for (i = 0; i < sz; i++) {
            res = arguments[i]->Eval(aNULL, aHASKELL, idthread);
            
            if (res->isError()) {
                func->Release();
                return res;
            }
            func->arguments.push_back(res);
        }
        return func;
    }

    for (i = 0; i < sz; i++) {
        res = arguments[i]->Eval(aNULL, aHASKELL, idthread);
        
        if (res->isError()) {
            for (short j = 0; j < i; j++)
                args[j]->Resetreference();
            if (args != argms) {
                delete[] args;
            }
            return res;
        }
        
        res->Setreference();
        args[i] = res;
    }
    
    TamguDeclarationAutoClean* environment = globalTamgu->Providedeclarationclean(idthread);
    
    TamguFunctionLambda* bd = body;

    Tamgu* arg;
    
    short param_name, typevariable;
    bool execute = false;

    res = aNULL;

    if (globalTamgu->debugmode)
        globalTamgu->threads[idthread].nonblockingerror = "";

    while (bd != NULL) {
        if (sz != bd->parameters.size()) {
            bd = bd->Following();
            continue;
        }

        //In this case, we check all constant arguments beforehand...
        if (bd->constantparameters) {
            //The flag: constantargument has been set at compiling time...
            execute = true;
            //It is the first constant element in our list...
            for (i = bd->firstconstant; i < sz; i++) {
                arg = args[i];

                if (bd->checkconstant(i) && bd->parameters[i]->same(arg) == aFALSE) {
                    if (globalTamgu->debugmode) {
                        string s = "Mismatch between argument and declaration...";
                        s += "Argument: '" + arg->String();
                        s += "' ...Declaration: '" + bd->parameters[i]->String() + "'";
                        globalTamgu->threads[idthread].nonblockingerror = s;
                    }
                    execute = false;
                    break;
                }
            }
            
            if (!execute) {
                bd = bd->Following();
                continue;
            }
            
            execute = false;
        }
        
        for (i = 0; i < sz; i++) {
            if (bd->constantparameters && bd->checkconstant(i))
                continue;
            
            res = bd->parameters[i];
            arg = args[i];

            if (res->isConstContainer()) {
                //They could be some variables, that need to be created...
                if (!res->Size() && arg->Size()) {
                    res = aRAISEERROR;
                    break;
                }
                
                res->Prepare(environment, idthread);
                res = res->Put(aRAISEERROR, arg, idthread);
                
                if (res->isError()) {
                    if (globalTamgu->debugmode)
                        globalTamgu->threads[idthread].nonblockingerror = "Cannot project this container onto the current argument...";
                    res = aRAISEERROR;
                    break;
                }
                continue;
            }
            
            param_name = res->Name();
            if (!param_name) {//it could be either a value comparison (see above for constantargument or a data frame comparison)
                if (hdeclared == true && bd->taskelldeclarations[i]->Size()) {
                    short argsz = arg->Size();

                    if (argsz != bd->taskelldeclarations[i]->Size()) {
                        if (globalTamgu->debugmode) {
                            string s = "Argument number does not match data structure declaration size... ";
                            s += "Argument: '" + globalTamgu->Getsymbol(arg->Type());
                            s += "'... Declaration: '" + globalTamgu->Getsymbol(bd->taskelldeclarations[i]->Type()) + "'";
                            globalTamgu->threads[idthread].nonblockingerror = s;
                        }
                        res = aRAISEERROR;
                        break;
                    }
                    for (long j = 0; j < argsz; j++) {
                        if (arg->Typevariable(j) != bd->taskelldeclarations[i]->Typevariable(j)) {
                            if (globalTamgu->debugmode) {
                                string s = "Mismatch between argument and declaration...";
                                s += "Argument: '" + globalTamgu->Getsymbol(arg->Typevariable(j));
                                s += "'... Declaration: '" + globalTamgu->Getsymbol(bd->taskelldeclarations[i]->Typevariable(j)) + "'";
                                globalTamgu->threads[idthread].nonblockingerror = s;
                            }
                            res = aRAISEERROR;
                            break;
                        }
                    }
                    if (res == aRAISEERROR)
                        break;
                }
                if (res->Compare(environment, arg, idthread) == aFALSE) {
                    if (globalTamgu->debugmode) {
                        string s = "Mismatch between argument and data structure declaration... ";
                        s += "Argument: '" + globalTamgu->Getsymbol(arg->Type());
                        s += "' ...Declaration: '" + globalTamgu->Getsymbol(res->Type()) + "'";
                        globalTamgu->threads[idthread].nonblockingerror = s;
                    }
                    res = aRAISEERROR;
                    break;
                }
                continue;
            }
            
            typevariable = res->Typevariable();

            if (hdeclared == true && typevariable != a_self) {
                if (typevariable != arg->Type() && !globalTamgu->Compatiblestrict(typevariable, arg->Type())) {
                    if (globalTamgu->debugmode) {
                        string s = "Mismatch between argument and declaration...";
                        s += "Argument: '" + globalTamgu->Getsymbol(arg->Type());
                        s += "' ...Declaration: '" + globalTamgu->Getsymbol(res->Type()) + "'";
                        globalTamgu->threads[idthread].nonblockingerror = s;
                    }
                    res = aRAISEERROR;
                    break;
                }
                
                if (arg->isConst()) {
                    arg = globalTamgu->newInstance.get(typevariable)->Newvalue(arg, idthread);
                    arg->Setreference();
                    args[i] = arg;
                }                
            }
            
            environment->Declare(param_name, arg);
            globalTamgu->Storevariable(idthread, param_name, arg);
            arg->Setreference();
        }
        
        if (res != aRAISEERROR) {
            for (i = 0; i < bd->names.size(); i++)
                bd->localvariables[i]->Eval(environment, aHASKELL, idthread);

            switch (bd->lambdadomain->instructions.last) {
                case 0:
                    res = bd->DirectEval(environment, aNULL, idthread);
                    break;
                case 2:
                    res = GetTaskell2(context, environment, bd, idthread);
                    break;
                case 3:
                    res = GetTaskell3(context, environment, bd, idthread);
                    break;
                case 5:
                    res = GetTaskell5(context, environment, bd, idthread);
                    break;
                default:
                    res = GetTaskellN(context, environment, bd, idthread);
            }
            
            if (res != aBREAK) {
                if (!res->isError()) {
                    execute = true;
                    break;
                }
                
                if (bd->maybe) {
                        //In this case, it could be an error, but we have Maybe...
                        //We return Nothing...
                    globalTamgu->Cleanerror(idthread);
                    environment->Cleanenvironment();
                    for (i = 0; i < sz; i++) {
                        if (args[i] != res)
                            args[i]->Resetreference();
                    }
                    if (args != argms)
                        delete[] args;
                    return aNOTHING;
                }
            }
        }
        
        if (globalTamgu->Error(idthread)) {
            if (!res->isError())
                res = globalTamgu->Errorobject(idthread);
            break;
        }
        environment->Cleaning();
        
        res->Releasenonconst();
        
        res = aNULL;
        bd = bd->Following();
    }

    for (i = 0; i < sz; i++) {
        if (args[i] != res)
            args[i]->Resetreference();
    }
    if (args != argms)
        delete[] args;

    if (!environment->isEmpty() && res->Reference()) {
        res->Setreference();
        environment->Cleaning();
        res->Protect();
    }

    environment->Cleanenvironment();

    if (!execute && !globalTamgu->Error(idthread)) {
        if (body->maybe) {
            res->Releasenonconst();
            return aNOTHING;
        }
        
        globalTamgu->Current(this, idthread);
        string err = "Error: no suitable declaration was found for function: ";
        err += globalTamgu->Getsymbol(name);
        err += " --> ";
        err += globalTamgu->Getnonblockingerror(idthread);
        res->Releasenonconst();
        return globalTamgu->Returnerror(err, idthread);
    }

    if (globalTamgu->concepts.check(name)) {
        TamguFunction* func = NULL;
        if (globalTamgu->roles.check(name) && globalTamgu->rolefunction != NULL)
            func = globalTamgu->rolefunction;
        else
            if (globalTamgu->properties.check(name) && globalTamgu->propertyfunction != NULL)
                func = globalTamgu->propertyfunction;
            else
                if (globalTamgu->conceptfunction != NULL)
                    func = globalTamgu->conceptfunction;

        if (func != NULL) {
            TamguCallFunction2 callfunc(func);
            Tamgu* fname = globalTamgu->Providestring(globalTamgu->Getsymbol(name));
            callfunc.arguments.push_back(fname);
            callfunc.arguments.push_back(res);
            res->Setreference();
            fname->Setreference();
            Tamgu* rval = callfunc.Eval(aNULL, aNULL, idthread);
            if (rval != aNULL) {
                fname->Resetreference();
                res->Resetreference();
                return rval;
            }
            res->Protect();
        }
    }

    return res;
}

//This is the function that is called to process <and ...> and <or...>
//The main difference between the two calls is that when bd return FALSE for a AND it is a fail, while it suffices of one  TRUE for a OR to succeed.
//When a NULL is returned for a AND, it is OK, while it is a FAIL for a OR... Hence, the default value for AND is TRUE while it is FALSE for OR
Exporting Tamgu* Tamgu::Filterboolean(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* def) {

    Tamgu* returnval;
    TamguIteration* iter = Newiteration(false);
    char val = -1;
    char v;


    for (iter->Begin(); iter->End() == aFALSE; iter->Next()) {
        var->Putvalue(iter->IteratorValue(), idthread);
        returnval = bd->DirectEval(env, aNULL, idthread);

        if (returnval->needInvestigate()) {
            iter->Release();
            var->Forcedclean();
            return returnval;
        }

        if (returnval == aFALSE || returnval == aTRUE) {
            def = returnval;
            break;
        }
        
        if (def == aNULL) {//This is a XOR, returnval is aBREAKONE or aBREAKZERO...
            v = returnval->Byte();
            if (val == -1)
                val = v;
            else {
                if (val != v) {
                    def = aTRUE;
                    break;
                }
                val = v;
            }
        }
    }

    var->Forcedclean();
    iter->Release();
    if (def == aNULL)
        return aFALSE;
    
    return def;
}

//The <all...>, <and ...> and the <or ...> <xor ...> operators...
Tamgu* TamguCallFunctionTaskell::GetTaskell2(Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {

    Tamgu* var = bd->lambdadomain->instructions[0]->Eval(environment, aNULL, idthread);
    Tamgu* alist = bd->lambdadomain->instructions[1]->Eval(environment, aNULL, idthread);

    switch (bd->lambdadomain->name) {
        case 1:
        context = alist->Filterboolean(idthread, environment, bd, var, aTRUE);
            break;
        case 2:
            context = alist->Filterboolean(idthread, environment, bd, var, aFALSE); //It is a OR
            break;
        case 3:
            context = alist->Filterboolean(idthread, environment, bd, var, aNULL); //It is a XOR
            break;
    }

    alist->Release();
    var->Release();
    return context;
}

Exporting Tamgu* Tamgu::Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {
    TamguIteration* loop = Newiteration(false);
    if (loop == aITERNULL)
        return context;

    Tamgu* a;
    uchar addvalue = 0;

    if (context != aNOELEMENT)
        addvalue = Selecttype(context);

    char getkey = 0;
    if (isMapContainer()) {
        if (recipient->isMapContainer()) {
            recipient->Prepare(environment, idthread);
            getkey = 2;
        }
        else
            getkey = 1;
    }

    loop->Begin();
    while (!loop->End()->Boolean()) {
        switch (getkey) {
            case 0:
            recipient->Putvalue(loop->IteratorValue(), idthread);
            break;
            case 1:
            recipient->Putvalue(loop->IteratorKey(), idthread);
            break;
            case 2:
            recipient->Setvalue(loop, aNULL, idthread);
        }

        a = bd->DirectEval(environment, aNULL, idthread);
        if (a->isNULL()) {
            loop->Next();
            continue;
        }

        if (a->needInvestigate()) {
            if (a == aBREAK)
                break;

            delete loop;
            recipient->Forcedclean();
            context->Releasenonconst();
            return a;
        }


        context = Storealongtype(context, a, idthread, addvalue);
        a->Releasenonconst();
        loop->Next();
    }

    delete loop;
    recipient->Forcedclean();
    return context;

}

Tamgu* TamguCallFunctionTaskell::GetTaskell3(Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {
    //First we create our variables...
    //The first variable is the one that receives the iteration in the vector...

    Tamgu* recipient = NULL;
    
    if (bd->lambdadomain->local) {
            //We might have some variables declared in "where"
        for (short i = 0; i < bd->wherevariables.last; i++) {
            if (bd->wherevariables.vecteur[i]->Computevariablecheck(idthread)) {
                recipient = bd->wherevariables.vecteur[i]->Eval(environment, aNULL, idthread);
                if (recipient->isError()) {
                    return recipient;
                }
                recipient->Releasenonconst();
            }
        }
        recipient = NULL;
    }
    
    if (bd->lambdadomain->declarations.check(a_counter))
        bd->lambdadomain->Declaration(a_counter)->Eval(environment, aNULL, idthread);
    else
        if (bd->lambdadomain->declarations.check(a_drop))
            bd->lambdadomain->Declaration(a_drop)->Eval(environment, aNULL, idthread);

    bd->lambdadomain->instructions[0]->Setaffectation(true);

    //We put our system in store mode, which means that the returntype SHOULD not be tested in TamguFunctionLambda
    //The reason is that the context variable is the variable that is going to store the data and its type has already been tested
    //against returntype above, or below when context is missing... In that case, we create a context whose type will match returntype.
    bd->store = true;

    recipient = bd->lambdadomain->instructions[0]->Eval(environment, aNULL, idthread);
    Tamgu* vect = bd->lambdadomain->instructions[1]->Eval(environment, aNULL, idthread);

    //if it is constvector or a constmap, we need to declare the variables in them...
    if (recipient->isConstContainer())
        recipient->Prepare(environment, idthread);

    //We then need to take into account our context...
    if (context->isContainer()) {
        if (bd->returntype != a_null && bd->returntype != context->Type())
            context = globalTamgu->newInstance.get(bd->returntype)->Newinstance(idthread);
        else {
            if (vect == context || context->Size() || !context->isAffectation())
                context = context->Newinstance(idthread);
        }
    }
    else {
        if (context->isString())
            context = context->Newinstance(idthread);
        else {
            if (bd->returntype == a_null)
                context = aNOELEMENT; //by default it will be a simple vector...
            else
                context = globalTamgu->newInstance.get(bd->returntype)->Newinstance(idthread);
        }
    }
    
    //we need to loop...
    recipient = vect->Looptaskell(recipient, context, environment, bd, idthread);

    vect->Release();
    return recipient;
}


Exporting Tamgu* Tamgu::Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct) {

    Tamgu* returnval;
    TamguIteration* iter = Newiteration(false);

    bool first = false;
    Tamgu* key;

    if (init != aNOELEMENT) {
        accu->Putvalue(init, idthread);
        if (kcont != NULL) {
            if (direct)
                kcont->Insert(0, init);
            else
                kcont->Push(init);
        }
    }
    else
        first = true; //we are dealing with a foldr1 or a foldl1

    for (iter->Begin(); iter->End() == aFALSE; iter->Next()) {
        key = iter->IteratorValue();
        if (first) {
            returnval = key->Atom();//We use the first value of the list to seed our accumulator variable
            first = false;
        }
        else {
            var->Putvalue(key, idthread);
            returnval = bd->DirectEval(env, aNULL, idthread);
            if (returnval->isNULL())
                continue;

            if (returnval->needInvestigate()) {
                if (returnval == aBREAK) {
                    accu = returnval;
                    break;
                }

                var->Forcedclean();
                accu->Forcedclean();
                iter->Release();
                if (kcont != NULL)
                    kcont->Release();
                return returnval;
            }
        }

        accu->Putvalue(returnval, idthread);
        
        if (kcont != NULL) {
            if (direct)
                kcont->Insert(0, returnval);
            else
                kcont->Push(returnval);
        }
        returnval->Releasenonconst();
    }

    var->Forcedclean();
    iter->Release();
    if (kcont != NULL)
        return kcont;
    return accu->Value();
}

Exporting Tamgu* Tamgu::Filterreverse(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init) {

    Tamgu* returnval;
    TamguIteration* iter = Newiteration(false);

    bool first = false;
    Tamgu* key;

    if (init != aNOELEMENT) {
        accu->Putvalue(init, idthread);
        if (kcont != NULL) {
            kcont->Insert(0, init);
        }
    }
    else
        first = true; //we are dealing with a foldr1 or a foldl1

    vector<Tamgu*> values;
    
    for (iter->Begin(); iter->End() == aFALSE; iter->Next()) {
        key = iter->IteratorValue();
        key = key->Atomref();
        values.push_back(key);
    }
    iter->Release();
    
    for (long i = values.size()-1; i >= 0; i--) {
        key = values[i];
        if (first) {
            returnval = key->Atom();//We use the first value of the list to seed our accumulator variable
            first = false;
        }
        else {
            var->Putvalue(key, idthread);
            returnval = bd->DirectEval(env, aNULL, idthread);
            if (returnval->isNULL())
                continue;

            if (returnval->needInvestigate()) {
                if (returnval == aBREAK) {
                    accu = returnval;
                    break;
                }

                var->Forcedclean();
                accu->Forcedclean();
                iter->Release();
                if (kcont != NULL)
                    kcont->Release();
                return returnval;
            }
        }

        accu->Putvalue(returnval, idthread);
        
        if (kcont != NULL)
            kcont->Insert(0, returnval);
        returnval->Releasenonconst();
        key->Resetreference();
    }

    var->Forcedclean();
    if (kcont != NULL)
        return kcont;
    return accu->Value();
}


Tamgu* TamguCallFunctionTaskell::GetTaskell5(Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {

    Tamgu* kcont = NULL;

    Tamgu* key;


    Tamgu* init = bd->lambdadomain->instructions[3]->Eval(aNULL, aNULL, idthread);

    Tamgu* klist = bd->lambdadomain->instructions[1]->Eval(aNULL, aNULL, idthread);
    if (executionbreak || globalTamgu->Error(idthread))
        return aNULL;

    if (klist->Size() == 0 && !klist->isInfinite()) {
        key = bd->lambdadomain->instructions[4];
        if (key == aZERO || key == aONE) {// this is a scan
            kcont = klist->Newinstance(idthread);
            kcont->Push(init);
        }
        else
            kcont = init;
        init = NULL;
        return kcont;
    }

    Tamgu* var = bd->lambdadomain->instructions[0]->Eval(environment, aNULL, idthread);
    Tamgu* accu = bd->lambdadomain->instructions[2]->Eval(environment, aNULL, idthread);

    if (globalTamgu->Error(idthread))
        return globalTamgu->Errorobject(idthread);

    key = bd->lambdadomain->instructions[4];
    bool direct = key->Boolean();

    if (key == aZERO || key == aONE) // this is a scan
        kcont = klist->Newinstance(idthread);


    if (kcont != NULL) {
        if (!kcont->isContainer() && !kcont->isString()) {
            kcont->Releasenonconst();
            kcont = globalTamgu->Providevector();
        }
        if (direct)
            klist->Filterreverse(idthread, environment, bd, var, kcont, accu, init);
        else
            klist->Filter(idthread, environment, bd, var, kcont, accu, init, direct);
        klist->Releasenonconst();
        return kcont;
    }

    if (direct)
        key = klist->Filterreverse(idthread, environment, bd, var, kcont, accu, init);
    else
        key = klist->Filter(idthread, environment, bd, var, kcont, accu, init, direct);
    
    klist->Releasenonconst();
    return key;
}


Tamgu* TamguCallFunctionTaskell::Process(TaskellLoop* loop, Tamgu* context, Tamgu* env, TamguFunctionLambda* bd, short idthread) {
    Tamgu* a;

    loop->reset();
    while (!loop->end()) {
        if (globalTamgu->Error(idthread))
            return globalTamgu->Errorobject(idthread);
            
        loop->execute(idthread);
        if (loop->next != NULL)
            Process(loop->next, context, env, bd, idthread);
        else {
            a = bd->DirectEval(env, aNULL, idthread);

            if (a->needInvestigate()) {
                if (a == aBREAK)
                    break;
                return a;
            }


            if (a->isNULL())
                continue;

            if (context->isVectorContainer())
                context->Push(a);
            else
                context->Pushinmap(a, idthread);
            a->Releasenonconst();
        }
    }

    return context;
}

Tamgu* TamguCallFunctionTaskell::GetTaskellN(Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {


    TaskellLoop loops(idthread, environment);

    //First we store the recipients and the variables.
    Tamgu* recipient;
    Tamgu* vect;
    TamguIteration* loop;
    TaskellLoop* hloop = &loops;
    bool first = true;
    bool forcerenew = false;
    short sz = bd->lambdadomain->instructions.last;

    if (bd->lambdadomain->local) {
        //We might have some variables declared in "where"
        for (short i = 0; i < bd->wherevariables.last; i++) {
            if (bd->wherevariables.vecteur[i]->Computevariablecheck(idthread)) {
                recipient = bd->wherevariables.vecteur[i]->Eval(environment, aNULL, idthread);
                if (recipient->isError()) {
                    return recipient;
                }
                recipient->Releasenonconst();
            }
        }
    }
    
    //we put our system in store mode, which means that the returntype SHOULD not be tested in TamguFunctionLambda
    bd->store = true;

    if (context->isContainer()) {
        if (context->Size() || !context->isAffectation() || (bd->returntype != a_null && bd->returntype != context->Type()))
            forcerenew=true;
    }

    short i;
    for (i = 0; i < sz; i += 3) {
        if (bd->lambdadomain->instructions[i + 2] == aFALSE)  {
            if (first)
                first = false;
            else {
                hloop = new TaskellLoop(idthread, environment);
                loops.add(hloop);
            }
        }

        bd->lambdadomain->instructions[i]->Setaffectation(true);
        recipient = bd->lambdadomain->instructions[i]->Eval(environment, aNULL, idthread);
        vect = bd->lambdadomain->instructions[i + 1]->Eval(environment, aNULL, idthread);
        if (recipient->isConstContainer()) {
            recipient->Prepare(environment, idthread);
            hloop->maprecipient = true;
        }

        if (vect == aNOELEMENT) {
            //Missing variables to finalize the loop...
            //we need to postpone the evaluation...
            loop = aITERNULL;
            vect = bd->lambdadomain->instructions[i + 1];
        }
        else
            loop = vect->Newiteration(false);

        if (vect == context)
            forcerenew=true;

        hloop->push(recipient, vect, loop);
    }

    vect = loops.vects[0];
    //We then need to take into account our context...
    if (context->isContainer()) {
        if (!forcerenew)
            context = Selectacontainer(context, idthread);
        else {
            if (bd->returntype == a_null)
                context = context->Newinstance(idthread);
            else
                context = globalTamgu->newInstance.get(bd->returntype)->Newinstance(idthread);
        }
    }
    else {
        if (context->isString())
            context = context->Newinstance(idthread);
        else {
            if (bd->returntype == a_null)
                context = globalTamgu->Providevector(); //by default it will be a simple vector...
            else
                context = globalTamgu->newInstance.get(bd->returntype)->Newinstance(idthread);
        }
    }

    recipient = Process(&loops, context, environment, bd, idthread);

    if (recipient->isError()) {
        context->Releasenonconst();
        return recipient;
    }

    return context;
}


Tamgu* TamguCallFibre::Put(Tamgu* context, Tamgu* v, short idthread) {
    Tamgu* defcall = v;
    if (v->isFunctionParameter())
        defcall = globalTamgu->Getdefinition(v->Name(), idthread, context);

    if (defcall == NULL || !defcall->isTaskellFunction())
        return globalTamgu->Returnerror("Wrong assignement: Expected a taskell function");

    body = (TamguFunctionLambda*)defcall;
    returntype = body->returntype;
    
    return aTRUE;
}

Tamgu* TamguCallFibre::Eval(Tamgu* context, Tamgu* res, short idthread) {
    Tamgu** args = NULL;
    
    short i, sz = arguments.size();
    
    args = new Tamgu*[sz];
    
    for (i = 0; i < sz; i++) {
        res = arguments[i]->Eval(aNULL, aHASKELL, idthread);
        
        if (res->isError()) {
            for (short j = 0; j < i; j++)
                args[j]->Resetreference();
            return res;
        }
        
        res->Setreference();
        args[i] = res;
    }
    
    TamguDeclarationAutoClean* environment = globalTamgu->Providedeclarationclean(idthread);
    
    res = variable->Eval(aNULL, aNULL, idthread);
    if (res->Type() != a_fibre)
        return globalTamgu->Returnerror("Cannot execute this fibre... No function attached");
    
    TamguCallFibre* thevar = (TamguCallFibre*)res;
    
    res = thevar->Body(idthread);
    if (!res->isTaskellFunction())
        return globalTamgu->Returnerror("Cannot execute this fibre... No function attached");
    
    TamguFunctionLambda* bd = (TamguFunctionLambda*)res;
    
    Tamgu* arg;
    
    short param_name, typevariable;
    bool execute = false;
    
    res = aNULL;
    
    if (globalTamgu->debugmode)
        globalTamgu->threads[idthread].nonblockingerror = "";
    
    bool cleanenv = true;
    
    while (bd != NULL) {
        if (sz != bd->parameters.size()) {
            bd = bd->Following();
            continue;
        }
        
            //In this case, we check all constant arguments beforehand...
        if (bd->constantparameters) {
                //The flag: constantargument has been set at compiling time...
            execute = true;
                //It is the first constant element in our list...
            for (i = bd->firstconstant; i < sz; i++) {
                arg = args[i];
                
                if (bd->checkconstant(i) && bd->parameters[i]->same(arg) == aFALSE) {
                    if (globalTamgu->debugmode) {
                        string s = "Mismatch between argument and declaration...";
                        s += "Argument: '" + arg->String();
                        s += "' ...Declaration: '" + bd->parameters[i]->String() + "'";
                        globalTamgu->threads[idthread].nonblockingerror = s;
                    }
                    execute = false;
                    break;
                }
            }
            
            if (!execute) {
                bd = bd->Following();
                continue;
            }
            
            execute = false;
        }
        
        for (i = 0; i < sz; i++) {
            if (bd->constantparameters && bd->checkconstant(i))
                continue;
            
            res = bd->parameters[i];
            arg = args[i];
            
            if (res->isConstContainer()) {
                    //They could be some variables, that need to be created...
                if (!res->Size() && arg->Size()) {
                    res = aRAISEERROR;
                    break;
                }
                
                res->Prepare(environment, idthread);
                res = res->Put(aRAISEERROR, arg, idthread);
                
                if (res->isError()) {
                    if (globalTamgu->debugmode)
                        globalTamgu->threads[idthread].nonblockingerror = "Cannot project this container onto the current argument...";
                    res = aRAISEERROR;
                    break;
                }
                continue;
            }
            
            param_name = res->Name();
            if (!param_name) {//it could be either a value comparison (see above for constantargument or a data frame comparison)
                if (hdeclared == true && bd->taskelldeclarations[i]->Size()) {
                    short argsz = arg->Size();
                    
                    if (argsz != bd->taskelldeclarations[i]->Size()) {
                        if (globalTamgu->debugmode) {
                            string s = "Argument number does not match data structure declaration size... ";
                            s += "Argument: '" + globalTamgu->Getsymbol(arg->Type());
                            s += "'... Declaration: '" + globalTamgu->Getsymbol(bd->taskelldeclarations[i]->Type()) + "'";
                            globalTamgu->threads[idthread].nonblockingerror = s;
                        }
                        res = aRAISEERROR;
                        break;
                    }
                    for (long j = 0; j < argsz; j++) {
                        if (arg->Typevariable(j) != bd->taskelldeclarations[i]->Typevariable(j)) {
                            if (globalTamgu->debugmode) {
                                string s = "Mismatch between argument and declaration...";
                                s += "Argument: '" + globalTamgu->Getsymbol(arg->Typevariable(j));
                                s += "'... Declaration: '" + globalTamgu->Getsymbol(bd->taskelldeclarations[i]->Typevariable(j)) + "'";
                                globalTamgu->threads[idthread].nonblockingerror = s;
                            }
                            res = aRAISEERROR;
                            break;
                        }
                    }
                    if (res == aRAISEERROR)
                        break;
                }
                if (res->Compare(environment, arg, idthread) == aFALSE) {
                    if (globalTamgu->debugmode) {
                        string s = "Mismatch between argument and data structure declaration... ";
                        s += "Argument: '" + globalTamgu->Getsymbol(arg->Type());
                        s += "' ...Declaration: '" + globalTamgu->Getsymbol(res->Type()) + "'";
                        globalTamgu->threads[idthread].nonblockingerror = s;
                    }
                    res = aRAISEERROR;
                    break;
                }
                continue;
            }
            
            typevariable = res->Typevariable();
            
            if (hdeclared == true && typevariable != a_self) {
                if (typevariable != arg->Type() && !globalTamgu->Compatiblestrict(typevariable, arg->Type())) {
                    if (globalTamgu->debugmode) {
                        string s = "Mismatch between argument and declaration...";
                        s += "Argument: '" + globalTamgu->Getsymbol(arg->Type());
                        s += "' ...Declaration: '" + globalTamgu->Getsymbol(res->Type()) + "'";
                        globalTamgu->threads[idthread].nonblockingerror = s;
                    }
                    res = aRAISEERROR;
                    break;
                }
                
                if (arg->isConst()) {
                    arg = globalTamgu->newInstance.get(typevariable)->Newvalue(arg, idthread);
                    arg->Setreference();
                    args[i] = arg;
                }
            }
            
            environment->Declare(param_name, arg);
            globalTamgu->Storevariable(idthread, param_name, arg);
            arg->Setreference();
        }
        
        cleanenv = true;
        if (res != aRAISEERROR) {
            for (i = 0; i < bd->names.size(); i++)
                bd->localvariables[i]->Eval(environment, aHASKELL, idthread);
            
            switch (bd->lambdadomain->instructions.last) {
                case 0:
                    res = bd->DirectEval(environment, aNULL, idthread);
                    break;
                case 2:
                    res = GetTaskell2(context, environment, bd, idthread);
                    break;
                case 5:
                    res = GetTaskell5(context, environment, bd, idthread);
                    break;
                default:
                    res = thevar->GetTaskelFibres(context, environment, bd, args, idthread);
                    cleanenv = false;
            }
            
            if (!res->isError()) {
                execute = true;
                break;
            }
            
            if (bd->maybe) {
                    //In this case, it could be an error, but we have Maybe...
                    //We return Nothing...
                globalTamgu->Cleanerror(idthread);
                environment->Cleanenvironment();
                for (i = 0; i < sz; i++) {
                    if (args[i] != res)
                        args[i]->Resetreference();
                }
                
                delete[] args;
                return aNOTHING;
            }
        }
        
        if (globalTamgu->Error(idthread)) {
            if (!res->isError())
                res = globalTamgu->Errorobject(idthread);
            break;
        }
        environment->Cleaning();
        
        res->Releasenonconst();
        
        res = aNULL;
        bd = bd->Following();
    }

    if (!cleanenv)
        return aTRUE;

    for (i = 0; i < sz; i++) {
        if (args[i] != res)
            args[i]->Resetreference();
    }
    
    delete[] args;
    
    if (!environment->isEmpty() && res->Reference()) {
        res->Setreference();
        environment->Cleaning();
        res->Protect();
    }
    
    environment->Cleanenvironment();
    
    if (!execute && !globalTamgu->Error(idthread)) {
        if (body != NULL && body->maybe) {
            res->Releasenonconst();
            return aNOTHING;
        }
        
        globalTamgu->Current(this, idthread);
        string err = "Error: no suitable declaration was found for function: ";
        err += globalTamgu->Getsymbol(name);
        err += " --> ";
        err += globalTamgu->Getnonblockingerror(idthread);
        res->Releasenonconst();
        return globalTamgu->Returnerror(err, idthread);
    }
    
    return res;
}

Tamgu* TamguCallFibre::GetTaskelFibres(Tamgu* context, TamguDeclarationAutoClean* environment, TamguFunctionLambda* bd, Tamgu** args, short idthread) {
    
    
    TaskellLoop* loops = new TaskellLoop(idthread, environment);
    
        //First we store the recipients and the variables.
    Tamgu* recipient;
    Tamgu* vect;
    TamguIteration* loop;
    TaskellLoop* hloop = loops;
    bool first = true;
    short sz = bd->lambdadomain->instructions.last;
    
        //we put our system in store mode, which means that the returntype SHOULD not be tested in TamguFunctionLambda
    bd->store = true;
    
    short i;
    for (i = 0; i < sz; i += 3) {
        if (bd->lambdadomain->instructions[i + 2] == aFALSE)  {
            if (first)
                first = false;
            else {
                hloop = new TaskellLoop(idthread, environment);
                loops->add(hloop);
            }
        }
        
        bd->lambdadomain->instructions[i]->Setaffectation(true);
        recipient = bd->lambdadomain->instructions[i]->Eval(environment, aNULL, idthread);
        vect = bd->lambdadomain->instructions[i + 1]->Eval(environment, aNULL, idthread);
        if (recipient->isConstContainer()) {
            recipient->Prepare(environment, idthread);
            hloop->maprecipient = true;
        }
        
        if (vect == aNOELEMENT) {
                //Missing variables to finalize the loop...
                //we need to postpone the evaluation...
            loop = aITERNULL;
            vect = bd->lambdadomain->instructions[i + 1];
        }
        else
            loop = NULL;
        
        hloop->push(recipient, vect, loop);
    }
    
    if (context->isContent()) {
        Tamgu* cnt;
        if (bd->returntype != a_null)
            cnt = globalTamgu->newInstance.get(bd->returntype)->Newinstance(idthread);
        else
            cnt = globalTamgu->Providevector();
        
        context->Put(aNULL, cnt, idthread);
        context = cnt;
    }

    vect = loops->vects[0];
    context->Clear();
    context->Setreference();

    //We need to remove the variables now...
    environment->Removevariable(idthread);
    add(loops, context, bd, environment, args, bd->Size());
    
    return aTRUE;
}

long TaskellLoopLinkedList::idx = 0;
Tamgu* TaskellLoopLinkedList::Process(short idthread) {
    if (globalTamgu->Error(idthread))
        return globalTamgu->Errorobject(idthread);

    TaskellLoop* l;
    if (first) {
        l = loop;
        while (l != NULL) {
            l->reset();
            stack.push_back(l);
            l = l->next;
        }
        first = false;
    }
    else {
        if (stack.size() == 0)
            return aBREAK;
    }
    
    list<TaskellLoop*> backup;
    
    while (stack.back()->end()) {
        l = stack.back();
        backup.push_front(l);
        stack.pop_back();
        if (stack.size() == 0)
            return aBREAK;
    }
    
    environment->Initializevariable(idthread);
    
    stack.back()->execute(idthread);

    for (auto& it : backup) {
        it->reset();
        it->execute(idthread);
        stack.push_back(it);
    }
    
    Tamgu* a = current->DirectEval(environment, aNULL, idthread);
    
    environment->Removevariable(idthread);
    
    if (a->needInvestigate())
        return a;
    
    if (!recipient->isNULL()) {
        if (recipient->isVectorContainer())
            recipient->Push(a);
        else
            recipient->Pushinmap(a, idthread);
    }

    a->Releasenonconst();
    return aTRUE;
}

Exporting void TamguGlobal::Clearfibres(short idthread) {
    for (short i = 0; i < threads[idthread].fibres.last; i++) {
        if (threads[idthread].fibres[i] != NULL) {
            threads[idthread].fibres[i]->clean();
            threads[idthread].fibres.vecteur[i] = NULL;
        }
    }
    threads[idthread].fibres.clear();
}

//Only one thread can launch a run at a time...
//Different threads can add new elements to the list, but only one can run fibres
Tamgu* TamguCallFibre::Run(short idthread) {
    Locking lock(_lock);

    //We need to clean fibres from within the same thread... because of the iterator locks
    if (idfibre == -1) {
        threadid = idthread;
        idfibre = globalTamgu->threads[idthread].fibres.last;
        globalTamgu->threads[idthread].fibres.push_back(this);
    }
    
    Tamgu* res;
    while (head.next) {
        reset = false;
        TaskellLoopLinkedList* h = head.next;
        TaskellLoopLinkedList* previous = &head;
        if (h->block)
            return aTRUE;
        
        while (h != NULL) {
            if (globalTamgu->Error(idthread))
                return aFALSE;
            
            //A block was inserted, we need to reset the whole loop from the beginning...
            //If the current tail is blocked, we DO NOT execute it...
            if (reset || h->block)
                break;
            
            res = h->Process(idthread);
            if (res->isError())
                return aFALSE;
            
            if (res == aBREAK) {
                //The structure can be modified in different threads...
                Locking addlock(_addlock);
                if (block == h) {
                    block = NULL;
                    h->block = false;
                }
                
                if (tail == h)
                    tail = previous;
                previous->next = h->next;
                delete h;
                h = previous->next;
            }
            else {
                previous = h;
                h = h->next;
            }
        }
    }
    
    return aTRUE;
}

//-------------------------------------------------------------
// The following classes are used to execute a function through a variable in a Taskell expression: <Apply(func,a) = <func a>>
// For all the Tamgu...Parameter, Get needs to return itself (it is a function or a method), hence the implementation of an Execute to do the job

string TamguParameterFunction::String() {
    string nm = globalTamgu->Getsymbol(idtype);
    if (nm.find("call") != string::npos)
        nm = nm.substr(4, nm.size() - 4);
    nm += ":";
    nm += globalTamgu->Getsymbol(name);
    return nm;
}

void TamguParameterFunction::Setstring(string& nm, short idthread) {
    nm = globalTamgu->Getsymbol(idtype);
    if (nm.find("call") != string::npos)
        nm = nm.substr(4, nm.size() - 4);
    nm += ":";
    nm += globalTamgu->Getsymbol(name);
}

Tamgu* TamguFunctionParameter::Execute(Tamgu* context, Tamgu* callfunction, short idthread) {
    Tamgu* body = globalTamgu->Getdefinition(name, idthread, context);

    if (!body->isFunction())
        return globalTamgu->Returnerror("This object is not a function", idthread);

    TamguCallFunction fcall(body);
    fcall.arguments = ((TamguCall*)callfunction)->arguments;

    return fcall.Eval(context, aNULL, idthread);
}

Tamgu* TamguMethodParameter::Execute(Tamgu* context, Tamgu* callfunction, short idthread) {
        //In this case, the first argument is our object...
    if (callfunction->Size() == 0)
        return globalTamgu->Returnerror("Missing object", idthread);
    
    TamguCall* acall = (TamguCall*)callfunction;
    
    TamguCallFromCall fcall(name);
    for (short i = 1; i < acall->arguments.size(); i++)
        fcall.arguments.push_back(acall->arguments[i]);
    
    Tamgu* object = acall->arguments[0]->Eval(context, aNULL, idthread);
    Tamgu* a = fcall.Eval(context, object, idthread);
    object->Releasenonconst();
    return a;
}

Tamgu* TamguOperatorParameter::Execute(Tamgu* context, Tamgu* callfunction, short idthread) {
    TamguInstructionAPPLYOPERATIONROOT root(NULL);
    Tamgu* act = globalTamgu->actions[op];
    
    long i;
    if (left) {
        for (i = instructions.size()-1; i>=0; i--)
            root.instructions.push_back(instructions.vecteur[i]);
    }
    
    for (i = callfunction->Size()-1; i>=0; i--)
        root.instructions.push_back(callfunction->Argument(i));

    if (!left) {
        for (i = instructions.size()-1; i>=0; i--)
            root.instructions.push_back(instructions.vecteur[i]);
    }
    root.instructions.push_back(act);
    short d = (short)root.instructions.last - 1;
    return root.ccompute(idthread, true, d);
}

Tamgu* TamguFrameMethodParameter::Execute(Tamgu* context, Tamgu* callfunction, short idthread) {
    //In this case, the first argument is our object...
    if (callfunction->Size() == 0)
        return globalTamgu->Returnerror("Missing object", idthread);

    TamguCall* acall = (TamguCall*)callfunction;

    TamguCallFrameFunction fcall(name);
    for (short i = 1; i < acall->arguments.size(); i++)
        fcall.arguments.push_back(acall->arguments[i]);

    Tamgu* object = acall->arguments[0]->Eval(context, aNULL, idthread);
    Tamgu* a = fcall.Eval(context, object, idthread);
    object->Releasenonconst();
    return a;
}

Tamgu* TamguCommonParameter::Execute(Tamgu* context, Tamgu* callfunction, short idthread) {
    //In this case, the first argument is our object...
    if (callfunction->Size() == 0)
        return globalTamgu->Returnerror("Missing object", idthread);

    TamguCall* acall = (TamguCall*)callfunction;

    TamguCallCommonMethod fcall(name);
    for (short i = 1; i < acall->arguments.size(); i++)
        fcall.arguments.push_back(acall->arguments[i]);

    Tamgu* object = acall->arguments[0]->Eval(context, aNULL, idthread);
    Tamgu* a = fcall.Eval(context, object, idthread);
    object->Releasenonconst();
    return a;
}

Tamgu* TamguProcedureParameter::Execute(Tamgu* context, Tamgu* callfunction, short idthread) {

    TamguCallProcedure pcall(name);
    pcall.arguments = ((TamguCall*)callfunction)->arguments;

    return pcall.Eval(context, aNULL, idthread);
}

//This is the entry point of such an expression...
Tamgu* TamguGetFunction::Eval(Tamgu* context, Tamgu* c, short idthread) {
    c = globalTamgu->Getdeclaration(name, idthread);
    if (c->isFunctionParameter())
        return c->Execute(context, this, idthread);

    c = c->Body(idthread);
    if (c->isFunctionParameter())
        return c->Execute(context, this, idthread);

    if (c->isFunction()) {
        
        if (c->isCurryfied()) {
            long sz = c->Size()+arguments.size();
            Tamgu* cc = c->Function();
            long i;

            if (cc->isTaskellFunction()) {
                if (sz) {
                    TamguCallFunctionArgsTaskell hfunc((TamguFunctionLambda*)cc);
                    for (i = 0; i < c->Size(); i++) {
                        cc = ((TamguGetCurryfiedFunction*)c)->arguments[i]->Eval(context, aNULL, idthread);
                        hfunc.arguments.push_back(cc);
                    }
                    
                    for (i = 0; i < arguments.size(); i++) {
                        cc = arguments[i]->Eval(context, aNULL, idthread);
                        hfunc.arguments.push_back(cc);
                    }
                    cc = hfunc.Eval(context, aNULL, idthread);
                    for (i = 0; i < hfunc.arguments.size(); i++)
                        hfunc.arguments[i]->Release();
                    return cc;
                }
                
                TamguCallFunctionTaskell hfunc((TamguFunctionLambda*)cc);
                return hfunc.Eval(context, aNULL, idthread);
            }
            
            TamguCallFunction acall(cc);
            for (i = 0; i < c->Size(); i++) {
                cc = ((TamguGetCurryfiedFunction*)c)->arguments[i]->Eval(context, aNULL, idthread);
                acall.arguments.push_back(cc);
            }
            
            for (i = 0; i < arguments.size(); i++) {
                cc = arguments[i]->Eval(context, aNULL, idthread);
                acall.arguments.push_back(cc);
            }
            cc = acall.Eval(context, aNULL, idthread);
            for (i = 0; i < acall.arguments.size(); i++)
                acall.arguments[i]->Release();
            return cc;
        }

        if (c->isTaskellFunction()) {
            if (arguments.size()) {
                TamguCallFunctionArgsTaskell hfunc((TamguFunctionLambda*)c);
                hfunc.arguments = arguments;
                return hfunc.Eval(context, aNULL, idthread);
            }

            TamguCallFunctionTaskell hfunc((TamguFunctionLambda*)c);
            return hfunc.Eval(context, aNULL, idthread);
        }


        TamguCallFunction acall(c);
        acall.arguments = arguments;
        return acall.Eval(context, aNULL, idthread);
    }

    string message = "Cannot evaluate this variable as a function call: ";
    message += globalTamgu->Getsymbol(name);
    return globalTamgu->Returnerror(message, idthread);
}

Tamgu* TamguGetFunctionThrough::Eval(Tamgu* context, Tamgu* callfunction, short idthread) {
    callfunction = call->Eval(context, aNULL, idthread);

    if (callfunction->isFunctionParameter())
        return callfunction->Execute(context, this, idthread);

    if (callfunction->isTaskellFunction()) {

        if (arguments.size()) {
            TamguCallFunctionArgsTaskell hfunc((TamguFunctionLambda*)callfunction);
            hfunc.arguments = arguments;
            return hfunc.Eval(context, aNULL, idthread);
        }

        TamguCallFunctionTaskell hfunc((TamguFunctionLambda*)callfunction);
        return hfunc.Eval(context, aNULL, idthread);
    }

    if (callfunction->isFunction()) {
        TamguCallFunction acall(callfunction);
        acall.arguments = arguments;
        return acall.Eval(context, aNULL, idthread);
    }

    string message = "Cannot evaluate this as a function call: ";
    message += globalTamgu->Getsymbol(name);
    return globalTamgu->Returnerror(message, idthread);
}

//----------------------------------------------------------------------------------

//In these two cases, these methods are used to apply a method using the first argument as calling object
//This is used in Taskell such as: <trim x>
Tamgu* TamguGetMethod::Eval(Tamgu* context, Tamgu* callfunction, short idthread) {
    TamguCallFromCall fcall(name);
    for (short i = 1; i < arguments.size(); i++)
        fcall.arguments.push_back(arguments[i]);

    Tamgu* object = arguments[0]->Eval(context, aNULL, idthread);
    Tamgu* a = fcall.Eval(context, object, idthread);
    object->Releasenonconst();
    return a;
}

Tamgu* TamguGetCommon::Eval(Tamgu* context, Tamgu* callfunction, short idthread) {

    TamguCallCommonMethod fcall(name);
    for (short i = 1; i < arguments.size(); i++)
        fcall.arguments.push_back(arguments[i]);

    Tamgu* object;
    if (is_size) {
        TamguTaskellCounter* counter = new TamguTaskellCounter(0);
        counter->Setreference();
        object = arguments[0]->Eval(counter, aNULL, idthread);
        if (object->isError()) {
            counter->Resetreference();
            return object;
        }
        if (object != counter && object->isContainer()) {
            counter->value = object->Size();
            object->Release();
        }
        counter->Protect();
        return counter;
    }
    
    object = arguments[0]->Eval(context, aNULL, idthread);
    Tamgu* a = fcall.Eval(context, object, idthread);
    object->Releasenonconst();
    return a;
}

Tamgu* TamguFrameParameter::Compare(Tamgu* env, Tamgu* a, short idthread) {
    if (!a->isFrameinstance() || a->Frame()->Name() != framename)
        return aFALSE;

    Tamguframeinstance* af = (Tamguframeinstance*)a->Value();
    short idvar;
    
    for (int i = 0; i < equivalence.size(); i += 2) {
        a = af->Declaration(equivalence[i]);
        idvar = equivalence[i + 1];
        if (idvar < values.size()) { //in that case, we know that a value was stored in it...
            if (a->same(values[idvar]) == aFALSE)
                return aFALSE;
            continue;
        }
        env->Declare(idvar, a);
        a->Setreference();
        globalTamgu->Storevariable(idthread, equivalence[i + 1], a);
    }

    return aTRUE;
}

Tamgu* TamguCallFrameMethod::Eval(Tamgu* context, Tamgu* value, short idthread) {
    //the first argument must be a frame instance...
    if (arguments.size() == 0)
        return globalTamgu->Returnerror("Missing object", idthread);

    value = arguments[0]->Eval(context, aNULL, idthread);
    if (!value->isFrameinstance())
        return globalTamgu->Returnerror("Expecting object", idthread);

    TamguCallFrameFunction callfunc((TamguFrame*)value->Frame(), name);
    Tamgu* a;
    int i;
    for (i = 1; i < arguments.size(); i++) {
        a = arguments[i]->Eval(context, aNULL, idthread);
        callfunc.arguments.push_back(a);
        a->Setreference();
    }

    a = callfunc.Eval(context, value, idthread);

    for (i = 0; i < callfunc.arguments.size(); i++)
        callfunc.arguments[i]->Resetreference();

    return a;
}

bool TamguCallFrameMethod::Checkarity() {
    return Arity(globalTamgu->framemethods[name], arguments.size());
}


//In this case, if we compare it to the Conjunction Get, we merge sub-results with existing variables...
//We do not replace it...
Tamgu* TamguInstructionDisjunction::Eval(Tamgu* dom, Tamgu* result, short idthread) {
    if (dom->Type() != a_taskelldeclaration)
        return globalTamgu->Returnerror("This operator can only be used within Taskell expressions", idthread);

    Tamgu* ins;
    short i = 0;


    Tamguvector* results = globalTamgu->Providevector();
    if (negation)
        results->Push(globalTamgu->Providestring("¬"));
    results->Push(globalTamgu->Providestring("∨"));

    result = aNULL;
    for (i = 0; i < instructions.size(); i++) {
        ins = instructions[i];
        //either this parameter has not been instantiated yet and it needs to be...
        result = ins->Eval(dom, aNULL, idthread);
        //We need to find out about its main parameter:
        if (ins->isNegation())
            result->Insert(0, globalTamgu->Providestring("¬"));
            
        results->Push(result);
        result->Releasenonconst();
    }

    return results;
}

Tamgu* TamguInstructionConjunction::Eval(Tamgu* dom, Tamgu* result, short idthread) {
    if (dom->Type() != a_taskelldeclaration)
        return globalTamgu->Returnerror("This operator can only be used within Taskell expressions", idthread);

    Tamgu* param;
    Tamgu* ins;
    VECTE<short> names;
    short i = 0;
    short p;
    short n;

    TamguDeclarationAutoClean* ada = (TamguDeclarationAutoClean*)dom;

    Tamgu* previous = aNULL;
    result = aNULL;
    for (i = 0; i < instructions.size(); i++) {
        ins = instructions[i];
        //either this parameter has not been instantiated yet and it needs to be...
        result = ins->Eval(dom, aNULL, idthread);
        //We need to find out about its main parameter:
        if (ins->isNegation())
            result->Insert(0, globalTamgu->Providestring("¬"));

        if (ins->Size()) {
            n = 0;
            p = ins->Size() - 1; //the last parameter receives the result of the operation...

            while (!n && p >= 0) {
                param = ins->Argument(p);
                n = param->Name();
                p--;
            }

            if (n) {
                if (names.search(n) == -1)
                    names.push_back(n);
                ada->Replacedeclaration(n, result);
                previous->Releasenonconst();
                previous = aNULL;
            }
            else {
                previous->Releasenonconst();
                previous = result;
            }
        }
    }
    
    if (negation)
        result->Insert(0, globalTamgu->Providestring("¬"));

    return result;
}
