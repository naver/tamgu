/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguframeinstance.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamguframeinstance.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<frameinstanceMethod>  Tamguframeseeder::methods;
Exporting hmap<string, string> Tamguframeseeder::infomethods;
Exporting basebin_hash<unsigned long> Tamguframeseeder::exported;

Exporting short Tamguframeseeder::idtype = 0;

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguframeseeder::AddMethod(TamguGlobal* global, string name, frameinstanceMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



void Tamguframeseeder::Setidtype(TamguGlobal* global) {
    Tamguframeseeder::InitialisationModule(global,"");
}


   bool Tamguframeseeder::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    Tamguframeseeder::idtype = global->Getid("frameinstance");
    Tamguframeseeder::AddMethod(global, "_initial", &Tamguframeinstance::MethodInitial, P_NONE, "_initial(): Initialization of a frame.");
    Tamguframeseeder::AddMethod(global, "type", &Tamguframeinstance::MethodType, P_NONE, "type(): Return the frame's type.");


    if (version != "") global->newInstance[Tamguframeseeder::idtype] = new Tamguframeseeder(NULL, global);

    return true;
}


Tamguframeseeder* Tamguframeseeder::RecordFrame(short name, TamguFrame* f, TamguGlobal* global) {

    //Little hack so that a frame will always have a type() method...
    f->declarations[a_type] = new TamguFunction(a_type, global);

    Tamguframeseeder* a = new Tamguframeseeder(f, global);
    globalTamgu->frames[name] = f;
    global->newInstance[name] = a;

    return a;
}

//a Tamguframeseeder is used to record a frame creator, which can then be used to generate actual frame instances...
Tamgu* Tamguframeseeder::Newinstance(short idthread, Tamgu* a) {
    //the interval between the variable names is less than 64, we can use a much faster access method, which
    //is based on basemin_hash, a simple array...
    if (frame->choosemin)
        a = new Tamguframemininstance(frame);
    else
        a = new Tamguframeinstance(frame);
    
    //The frame does not contain any frame variables...
    if (!frame->post) {
        for (long i = 0; i < frame->variables.size(); i++)
            frame->variables[i]->Eval(a, aNULL, idthread);
        return a;
    }

    //This frame contains frame variables, we need to postpone the creation
    //of these variables to the end of the process...
    //The variable can be declared either directly in the list of variable declarations
    //or it can instantiated within an _initial function declared in the current frame...
    Tamgu* o;
    //we create a frame object, with what was declared into the frame...
    for (long i = 0; i < frame->variables.size(); i++) {
        o = frame->variables[i];
        if (o->isFrame()) {//these cases will be handled in the Postinstantiation method...
            //we create a temporate variable, with no actual type...
            a->Declare(o->Name(), new TamguLet);
            continue;
        }
        o = o->Eval(a, aNULL, idthread);
    }

    return a;
}

void Tamguframeinstance::Postinstantiation(short idthread, bool setreference) {
    if (!frame->post)
        return;
    
    //Now we are going to create frame variables...
    Tamgu* o;
    Tamgu* v;
    
    Pushframe(idthread);
    //we create a frame object, with what was declared into the frame...
    //We have two cases:
    // Either: we have a creation within the _initial function of frame, where this local frame is declared
    // Or: we need to use the default initialization that was declared where the local frame itself is declared
    //frame toto {
    //        within w(10); //default initialization, see o->Eval(this, aNULL, idthread);
    //        function _initial(int k) {
    //            w=within(k); // initialization within a _initial function that replaces the other default initialization
    
    for (long i = 0; i < frame->variables.size(); i++) {
        o = frame->variables[i];
        if (o->isFrame()) {//these cases will be handled in the Postinstantiation method...
            short ipos = Idposition(o->Name());
            v = declarations.vecteur[ipos]->Value();
            //we delete the variable, a TamguLet created in Newinstance...
            delete declarations.vecteur[ipos];
            declarations.vecteur[ipos] = aNULL;
            
            if (v != aNOELEMENT) { //we are dealing with an initialization within a _initial function
                if (v->Typevariable() != o->Typevariable()) {
                    v->Resetreference(reference);
                    stringstream msg;
                    msg << "Type mismatch for '" << globalTamgu->Getsymbol(o->Name()) << "':";
                    globalTamgu->Returnerror(msg.str(), idthread);
                    return;
                }
                declarations.vecteur[ipos] = v;
            }
            else //we rollback to the default initialization...
                o->Eval(this, aNULL, idthread);
            
            if (setreference)
                declarations[ipos]->Setreference(reference);
        }
    }
    Popframe(idthread);
}

void Tamguframemininstance::Postinstantiation(short idthread, bool setreference) {
    if (!frame->post)
        return;

    //Now we are going to create frame variables...
    Tamgu* o;
    Tamgu* v;

    Pushframe(idthread);
    //we create a frame object, with what was declared into the frame...
    //We have two cases:
    // Either: we have a creation within the _initial function of frame, where this local frame is declared
    // Or: we need to use the default initialization that was declared where the local frame itself is declared
    //frame toto {
    //		within w(10); //default initialization, see o->Eval(this, aNULL, idthread);
    //		function _initial(int k) {
    //			w=within(k); // initialization within a _initial function that replaces the other default initialization

    for (long i = 0; i < frame->variables.size(); i++) {
        o = frame->variables[i];
        if (o->isFrame()) {//these cases will be handled in the Postinstantiation method...
            short ipos = o->Name();
            v = declarations[ipos]->Value();
            //we delete the variable, a TamguLet created in Newinstance...
            delete declarations[ipos];
            declarations[ipos] = aNULL;

            if (v != aNOELEMENT) { //we are dealing with an initialization within a _initial function
                if (v->Typevariable() != o->Typevariable()) {
                    v->Resetreference(reference);
                    stringstream msg;
                    msg << "Type mismatch for '" << globalTamgu->Getsymbol(o->Name()) << "':";
                    globalTamgu->Returnerror(msg.str(), idthread);
                    return;
                }
                declarations[ipos] = v;
            }
            else //we rollback to the default initialization...
                o->Eval(this, aNULL, idthread);
                
            if (setreference)
                declarations[ipos]->Setreference(reference);
        }
    }
    Popframe(idthread);
}

Tamgu* TamguframeBaseInstance::Eval(Tamgu* context, Tamgu* idx, short idthread) {
    if (idx->isIndex()) {
        //In this case, we check if we have an index function
        TamguIndex* aid = (TamguIndex*)idx;
        Tamgu* func;
        if (aid->interval)
            func = frame->Declaration(a_interval);
        else
            func = frame->Declaration(a_index);
            
        if (func == NULL)
            return globalTamgu->Returnerror("Cannot process indexes", idthread);

        VECTE<Tamgu*> arguments;
        Tamgu* a = aid->left->Eval(context, aNULL, idthread);
        a->Setreference();
        arguments.push_back(a);

        if (aid->interval) {
            a = aid->right->Eval(context, aNULL, idthread);
            a->Setreference();
            arguments.push_back(a);
        }

        //No value...
        arguments.push_back(aNOELEMENT);

        func = Execute(func, arguments, idthread);

        if (arguments[0] == aid->left)
            arguments[0]->Protect();
        else
            arguments[0]->Resetreference();

        if (aid->interval) {
            if (arguments[1] == aid->right)
                arguments[1]->Protect();
            else
                arguments[1]->Resetreference();
        }


        return func;
    }

    return this;
}

//We could have implemented the execution of a method as a simple TamguCallFunction, however in the case of a multhreading application
//there can be some issues. If the value that is returned by our method belongs to the frame object, then in some cases, there could be
//a desynchronisation between the reference setting in the function with the reference setting in other threads.
//Hence, this specific implementation in which returned variables are protected by the frame instance lock.
Tamgu* TamguframeBaseInstance::Execute(Tamgu* body, VECTE<Tamgu*>& arguments, short idthread) {


    Tamgu* p;
    TamguFunction* bd = (TamguFunction*)body->Body(idthread);

    
    if (bd->isThread()) {
        TamguCallThread callthread(bd);
        callthread.arguments = arguments;
        callthread.idinfo = bd->idinfo;
        return callthread.Eval(this, NULL, idthread);
    }

    TamguDeclarationLocal* environment = globalTamgu->Providedeclaration(this, idthread, false);

    bool error = true;
    while (bd != NULL) {
        if (arguments.size() != bd->Size()) {
            bd = bd->next;
            continue;
        }

        locking();
        error = false;
        for (size_t i = 0; i < arguments.size(); i++) {
            p = bd->Parameter(i);
            if (!p->Setvalue(environment, arguments[i], idthread)) {
                error = true;
                break;
            }
        }
        unlocking();

        if (!error)
            break;

        environment->Cleaning();

        bd = bd->next;
    }

    if (error) {
        string err = "Check the arguments of: ";
        err += globalTamgu->Getsymbol(body->Name());
        return globalTamgu->Returnerror(err, idthread);
    }

    Pushframe(idthread);
    //We then apply our function within this environment
    Tamgu* a = bd->Eval(environment, this, idthread);
    Popframe(idthread);

    if (a->Reference()) {
        a->Setreference();
        //we clean our structure...
        environment->Releasing();
        a->Protect();
    }
    else
        environment->Releasing();

    return a;
}

Tamgu* TamguframeBaseInstance::Execute(Tamgu* body, short idthread) {


    TamguFunction* bd = (TamguFunction*)body->Body(idthread);

    while (bd != NULL) {
        if (bd->Size() == 0)
            break;
        bd = bd->next;
    }

    if (bd == NULL) {
        string err = "Check the arguments of: ";
        err += globalTamgu->Getsymbol(body->Name());
        return globalTamgu->Returnerror(err, idthread);
    }

    if (bd->isThread()) {
        TamguCallThread callthread(bd);
        callthread.idinfo = bd->idinfo;
        return callthread.Eval(this, NULL, idthread);
    }

    TamguDeclarationLocal* environment = globalTamgu->Providedeclaration(this, idthread, false);

    Pushframe(idthread);
    //We then apply our function within this environment
    Tamgu* a = bd->Eval(environment, this, idthread);
    Popframe(idthread);

    if (a->Reference()) {
        a->Setreference();
        //we clean our structure...
        environment->Releasing();
        a->Protect();
    }
    else
        environment->Releasing();

    return a;
}


Tamgu* Tamguframemininstance::Put(Tamgu* idx, Tamgu* value, short idthread) {
    if (idx->isIndex()) {
        //In this case, we check if we have an index function
        TamguIndex* aid = (TamguIndex*)idx;
        Tamgu* func;
        
        if (aid->interval)
            func = frame->Declaration(a_interval);
        else
            func = frame->Declaration(a_index);
        
        if (func == NULL)
            return globalTamgu->Returnerror("Cannot process indexes", idthread);
        
        
        VECTE<Tamgu*> arguments;
        Tamgu* a = aid->left->Eval(aNULL, aNULL, idthread);
        a->Setreference();
        arguments.push_back(a);
        
        if (aid->interval) {
            a = aid->right->Eval(aNULL, aNULL, idthread);
            a->Setreference();
            arguments.push_back(a);
        }
        
        arguments.push_back(value);
        value->Setreference();
        
        func = Execute(func, arguments, idthread);
        
        if (arguments[0] == aid->left)
            arguments[0]->Protect();
        else
            arguments[0]->Resetreference();
        
        if (aid->interval) {
            if (arguments[1] == aid->right)
                arguments[1]->Protect();
            else
                arguments[1]->Resetreference();
        }
        value->Resetreference();
        
        
        return func;
    }
    
    if (globalTamgu->Compatible(frame->Name(), value->Type()) == false)
        return globalTamgu->Returnerror("Wrong frame assignment", idthread);
    
    locking();
    TamguframeBaseInstance* instance = (TamguframeBaseInstance*)value;
    
    short nm;
    for (short ii = 0; ii < frame->vnames.last; ii++) {
        nm = frame->vnames[ii];
        declarations[nm]->Put(aNULL, instance->Declaration(nm), idthread);
    }
    unlocking();
    
    return aTRUE;
}

Tamgu* Tamguframemininstance::Putvalue(Tamgu* value, short idthread) {
    if (globalTamgu->Compatible(frame->Name(), value->Type()) == false)
        return globalTamgu->Returnerror("Wrong frame assignment", idthread);
    
    locking();
    TamguframeBaseInstance* instance = (TamguframeBaseInstance*)value;
    
    short nm;
    for (short ii = 0; ii < frame->vnames.last; ii++) {
        nm = frame->vnames[ii];
        declarations[nm]->Putvalue(instance->Declaration(nm), idthread);
    }

    unlocking();
    
    return aTRUE;
}

Tamgu* Tamguframeinstance::Put(Tamgu* idx, Tamgu* value, short idthread) {
    if (idx->isIndex()) {
        //In this case, we check if we have an index function
        TamguIndex* aid = (TamguIndex*)idx;
        Tamgu* func;
        
        if (aid->interval)
            func = frame->Declaration(a_interval);
        else
            func = frame->Declaration(a_index);
        
        if (func == NULL)
            return globalTamgu->Returnerror("Cannot process indexes", idthread);
        
        
        VECTE<Tamgu*> arguments;
        Tamgu* a = aid->left->Eval(aNULL, aNULL, idthread);
        a->Setreference();
        arguments.push_back(a);
        
        if (aid->interval) {
            a = aid->right->Eval(aNULL, aNULL, idthread);
            a->Setreference();
            arguments.push_back(a);
        }
        
        arguments.push_back(value);
        value->Setreference();
        
        func = Execute(func, arguments, idthread);
        
        if (arguments[0] == aid->left)
            arguments[0]->Protect();
        else
            arguments[0]->Resetreference();
        
        if (aid->interval) {
            if (arguments[1] == aid->right)
                arguments[1]->Protect();
            else
                arguments[1]->Resetreference();
        }
        value->Resetreference();
        
        
        return func;
    }
    
    if (globalTamgu->Compatible(frame->Name(), value->Type()) == false)
        return globalTamgu->Returnerror("Wrong frame assignment", idthread);
    
    locking();
    TamguframeBaseInstance* instance = (TamguframeBaseInstance*)value;
    
    for (short ii = 0; ii < declarations.last; ii++)
        declarations[ii]->Put(aNULL, instance->Declaration(frame->vnames[ii]), idthread);
    unlocking();
    
    return aTRUE;
}

Tamgu* Tamguframeinstance::Putvalue(Tamgu* value, short idthread) {
    if (globalTamgu->Compatible(frame->Name(), value->Type()) == false)
        return globalTamgu->Returnerror("Wrong frame assignment", idthread);
    
    locking();
    TamguframeBaseInstance* instance = (TamguframeBaseInstance*)value;
    
    for (short ii = 0; ii < declarations.last; ii++)
        declarations[ii]->Putvalue(instance->Declaration(frame->vnames[ii]), idthread);
    
    unlocking();
    
    return aTRUE;
}
