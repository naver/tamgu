/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
Project    : Tamgu (탐구)
    Version    : See tamgu.cxx for the version number
filename   : lispeccxx
Date       :
Purpose    :
Programmer : Claude ROUX (claude.roux@naverlabs.com)
    Reviewer   :
*/

#include "tamgu.h"
#include "tamguversion.h"
#include "instructions.h"

#include "tamgustring.h"
#include "tamgulispe.h"
#include "tamgustring.h"
#include "tamgushort.h"
#include "tamgufloat.h"
#include "tamgudecimal.h"
#include "tamguint.h"
#include "tamgucomplex.h"

#include "tamguvector.h"
#include "tamgulist.h"

#include "tamguivector.h"
#include "tamgubvector.h"
#include "tamgusvector.h"
#include "tamguuvector.h"
#include "tamgufvector.h"
#include "tamgudvector.h"
#include "tamguhvector.h"
#include "tamgulvector.h"

#include "tamgumap.h"
#include "tamgumapi.h"
#include "tamgumapf.h"

#include "tamgutreemap.h"
#include "tamgutreemapi.h"
#include "tamgutreemapf.h"

#include "tamgulisp.h"

// We need to declare once again our local definitions.
basebin_hash<lispeMethod> Tamgulispe::methods;
static ThreadLock classlock;

short Tamgulispe::idtype = 0;
//------------------------------------------------------------------------------------------------------------------
static Tamgulispe *local_lispe = NULL;
//------------------------------------------------------------------------------------------------------------------
class Tamgulispelist : public TamguReference
{
public:
    Element *liste;

    Tamgulispelist(Element *l)
    {
        liste = l;
    }

    ~Tamgulispelist()
    {
        liste->release();
    }
};

class Lispetamgusymbol : public Element
{
public:
    Tamgusymbol *symbol;
    short idthread;

    Lispetamgusymbol(Tamgusymbol *s, short i) : Element(t_atom)
    {
        symbol = s;
        idthread = i;
    }

    string toString(LispE *lisp)
    {
        return globalTamgu->Getsymbol(symbol->Name());
    }

    u_ustring asUString(LispE *lisp)
    {
        return w_to_u(asString(lisp));
    }

    wstring asString(LispE *lisp)
    {
        string s = globalTamgu->Getsymbol(symbol->Name());
        wstring u;
        s_utf8_to_unicode(u, USTR(s), s.size());
        return u;
    }

    Element *eval(LispE *lisp)
    {
        Tamgu *s = symbol->Eval(aNULL, aNULL, idthread);
        Element *e = local_lispe->convert_to_lispe(s, idthread);
        s->Release();
        return e;
    }
};

class Lispecallfunction : public List
{
public:
    short name;
    short idthread;

    Lispecallfunction(Tamgu *f, short i)
    {
        name = f->Name();
        idthread = i;
    }

    Element *eval(LispE *lisp)
    {
        Tamgu *v = globalTamgu->Declaration(name, idthread);
        TamguCallFunction call(v);
        Element *e;
        for (int i = 0; i < size(); i++)
        {
            e = liste[i]->eval(lisp);
            call.arguments.push_back(local_lispe->convert_to_tamgu(e, idthread));
            e->release();
        }

        v = call.Eval(aNULL, aNULL, idthread);
        e = local_lispe->convert_to_lispe(v, idthread);
        v->Release();
        return e;
    }
};

class Lispecallprocedure : public List
{
public:
    short name;
    short idthread;

    Lispecallprocedure(Tamgu *f, short i)
    {
        name = f->Name();
        idthread = i;
    }

    Element *eval(LispE *lisp)
    {
        Tamgu *v = globalTamgu->Declaration(name, idthread);
        TamguCallProcedure call(name);
        Element *e;
        for (int i = 0; i < size(); i++)
        {
            e = liste[i]->eval(lisp);
            call.arguments.push_back(local_lispe->convert_to_tamgu(e, idthread));
            e->release();
        }
        v = call.Eval(aNULL, aNULL, idthread);
        e = local_lispe->convert_to_lispe(v, idthread);
        v->Release();
        return e;
    }
};

class Lispecallcommon : public List
{
public:
    short name;
    short idthread;

    Lispecallcommon(Tamgu *f, short i)
    {
        name = f->Name();
        idthread = i;
    }

    Element *eval(LispE *lisp)
    {
        Tamgu *v = globalTamgu->Declaration(name, idthread);
        TamguCallCommonMethod call(name);
        Element *e;
        for (int i = 0; i < size(); i++)
        {
            e = liste[i]->eval(lisp);
            call.arguments.push_back(local_lispe->convert_to_tamgu(e, idthread));
            e->release();
        }
        v = call.Eval(aNULL, aNULL, idthread);
        e = local_lispe->convert_to_lispe(v, idthread);
        v->Release();
        return e;
    }
};

//------------------------------------------------------------------------------------------------------------------
Tamgulispe::Tamgulispe()
{
    lisp = new LispE();
    if (local_lispe == NULL)
        local_lispe = this;
}

Tamgulispe::~Tamgulispe()
{
    for (long i = 0; i < buffers.size(); i++)
        buffers[i]->Resetreference();
    if (local_lispe == this)
        local_lispe = NULL;

    if (lisp != NULL)
        delete lisp;
}
//------------------------------------------------------------------------------------------------------------------

// MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgulispe::AddMethod(TamguGlobal *global, string name, lispeMethod func, unsigned long arity, string infos)
{
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
        global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
        return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}

void Tamgulispe::Setidtype(TamguGlobal *global)
{
    Locking lock(classlock);
    if (Tamgulispe::methods.isEmpty())
        Tamgulispe::InitialisationModule(global, "");
}

Tamgu *Tamgulispe::eval(Element *l, short idthread)
{
    try
    {
        // cerr << "Code:" << l->toString(lisp) << endl;
        Element *e = l->eval(lisp);
        Tamgu *t = convert_to_tamgu(e, idthread);
        e->release();
        return t;
    }
    catch (Error *err)
    {
        string s = err->toString(lisp);
        return globalTamgu->Returnerror(s, idthread);
    }
}

bool Tamgulispe::InitialisationModule(TamguGlobal *global, string version)
{
    methods.clear();

    // Each new object has a specific name, which will help recognize it in the code that will exploit lispe...
    Tamgulispe::idtype = global->Getid("lispe");

    // You declare your methods here:
    //  Argument 1 is a call to global, which will record your method into Tamgu (탐구)
    //  Argument 2 is the name of your command
    //  Argument 3 links your command with its implementation into the object class
    //  Argument 4 is a combination of P_... which defines how many parameters this function can accept.
    // Argument 5 is a description of this method.
    Tamgulispe::AddMethod(global, "execute", &Tamgulispe::MethodExecute, P_FULL, "execute(string code): Execution of a lisp code.");
    Tamgulispe::AddMethod(global, "eval", &Tamgulispe::MethodEval, P_FULL, "eval(string code): Execution of a lisp code.");

    // We need this code, in order to create new instances of our lispe object... DO NOT ALTER
    global->newInstance[Tamgulispe::idtype] = new Tamgulispe(global);
    global->RecordCompatibilities(Tamgulispe::idtype);
    local_lispe = NULL;
    return true;
}

TamguIteration *Tamgulispe::Newiteration(bool direction)
{
    // If you do not need an iteration, do:
    return aITERNULL;
}

Tamgu *Tamgulispe::Put(Tamgu *idx, Tamgu *kval, short idthread)
{
    // If you want to put some element into your object
    return aTRUE;
}

Tamgu *Tamgulispe::Eval(Tamgu *context, Tamgu *idx, short idthread)
{
    // if your object is called from within an expression...
    return this;
}

Tamgu *Tamgulispe::convert_to_tamgu(Element *e, short idthread)
{
    switch (e->type)
    {
    case t_short:
        return new Tamgushort(e->asShort());
    case t_integer:
        return globalTamgu->Provideint(e->asInteger());
    case t_float:
        return new Tamgudecimal(e->asFloat());
    case t_number:
        return globalTamgu->Providefloat(e->asNumber());
    case t_complex:
        return new Tamgucomplexe(((Complexnumber *)e)->content);
    case t_string:
        return new Tamguustring(e->asString(lisp));
    case t_stringbyte:
        return new Tamgustring(e->toString(lisp));
    case t_floats:
    {
        Tamgudvector *v = new Tamgudvector();
        ((Floats *)v)->liste.to_vector(v->values);
        return v;
    }
    case t_shorts:
    {
        Tamguhvector *v = new Tamguhvector();
        ((Shorts *)v)->liste.to_vector(v->values);
        return v;
    }
    case t_integers:
    {
        Tamguivector *v = globalTamgu->Provideivector();
        ((Integers *)v)->liste.to_vector(v->values);
        return v;
    }
    case t_numbers:
    {
        Tamgufvector *v = globalTamgu->Providefvector();
        ((Numbers *)v)->liste.to_vector(v->values);
        return v;
    }
    case t_strings:
    {
        Tamgusvector *v = globalTamgu->Providesvector();
        Stringbytes b((Strings *)e);
        b.liste.to_vector(v->values);
        return v;
    }
    case t_stringbytes:
    {
        Tamgusvector *v = globalTamgu->Providesvector();
        ((Stringbytes *)e)->liste.to_vector(v->values);
        return v;
    }
    case t_list:
    {
        Tamguvector *v = globalTamgu->Providevector();
        List *l = (List *)e;
        Tamgu* a;
        for (long i = 0; i < l->size(); i++)
        {
            a = convert_to_tamgu(l->liste[i], idthread);
            v->values.push_back(a);
            a->Setreference();
        }
        return v;
    }
    case t_llist:
    {
        Tamgulist *lst = new Tamgulist();
        LList *l = (LList *)e;
        u_link *u = l->liste.begin();
        while (u != NULL)
        {
            lst->Push(convert_to_tamgu(u->value, idthread));
            u = u->next();
        }
        return lst;
    }
    case t_matrix_string:
    case t_matrix_stringbyte:
    case t_matrix_short:
    case t_matrix_number:
    case t_matrix_float:
    case t_matrix_integer:
    case t_tensor_string:
    case t_tensor_stringbyte:
    case t_tensor_short:
    case t_tensor_number:
    case t_tensor_float:
    case t_tensor_integer:
    {
        Tamguvector *v = globalTamgu->Providevector();
        List *l = (List *)e;
        for (long i = 0; i < e->size(); i++)
        {
            v->Push(convert_to_tamgu(l->liste[i], idthread));
        }
        return v;
    }
    case t_dictionary:
    case t_set:
    {
        Tamgumap *m = globalTamgu->Providemap();
        Dictionary *d = (Dictionary *)e;
        std::unordered_map<u_ustring, Element *>::iterator it;
        string v;
        u_ustring u;
        for (it = d->dictionary.begin(); it != d->dictionary.end(); it++)
        {
            u = it->first;
            s_unicode_to_utf8(v, u);
            m->push(v, convert_to_tamgu(it->second, idthread));
        }
        return m;
    }
    case t_dictionaryi:
    {
        Tamgumapi *m = new Tamgumapi();
        Dictionary_i *d = (Dictionary_i *)e;
        std::unordered_map<long, Element *>::iterator it;
        for (it = d->dictionary.begin(); it != d->dictionary.end(); it++)
        {
            m->Push(it->first, convert_to_tamgu(it->second, idthread));
        }
        return m;
    }
    case t_dictionaryn:
    {
        Tamgumapf *m = new Tamgumapf();
        Dictionary_n *d = (Dictionary_n *)e;
        std::unordered_map<double, Element *>::iterator it;
        for (it = d->dictionary.begin(); it != d->dictionary.end(); it++)
        {
            m->Push(it->first, convert_to_tamgu(it->second, idthread));
        }
        return m;
    }
    case t_sets:
    {
        Tamgumap *m = globalTamgu->Providemap();
        Set_s *d = (Set_s *)e;
        string v;
        u_ustring u;
        for (auto& a : d->ensemble) {
            u = a;
            s_unicode_to_utf8(v, u);
            m->Push(v, aTRUE);
        }
        return m;
    }
    case t_seti:
    {
        Tamgumapi *m = new Tamgumapi();
        Set_i *d = (Set_i *)e;
        for (auto& a : d->ensemble) {
            m->Push(a, aTRUE);
        }
        return m;
    }
    case t_setn:
    {
        Tamgumapf *m = new Tamgumapf();
        Set_n *d = (Set_n *)e;
        for (auto& a : d->ensemble) {
            m->Push(a, aTRUE);
        }
        return m;
    }
    case t_tree:
    {
        Tamgutreemap *m = globalTamgu->Providetreemap();
        Tree *d = (Tree *)e;
        std::map<u_ustring, Element *>::iterator it;
        string v;
        u_ustring u;
        for (it = d->tree.begin(); it != d->tree.end(); it++)
        {
            u = it->first;
            s_unicode_to_utf8(v, u);
            m->Push(v, convert_to_tamgu(it->second, idthread));
        }
        return m;
    }
    case t_treei:
    {
        Tamgutreemapi *m = new Tamgutreemapi();
        Tree_i *d = (Tree_i *)e;
        std::map<long, Element *>::iterator it;
        for (it = d->tree.begin(); it != d->tree.end(); it++)
        {
            m->Push(it->first, convert_to_tamgu(it->second, idthread));
        }
        return m;
    }
    case t_treen:
    {
        Tamgutreemapf *m = new Tamgutreemapf();
        Tree_n *d = (Tree_n *)e;
        std::map<double, Element *>::iterator it;
        for (it = d->tree.begin(); it != d->tree.end(); it++)
        {
            m->Push(it->first, convert_to_tamgu(it->second, idthread));
        }
        return m;
    }
    default:
        return new Tamguustring(e->asString(lisp));
    }
}

Element *Tamgulispe::convert_to_lispe(Tamgu *e, short idthread)
{
    switch (e->Type())
    {
    case a_short:
        return lisp->provideShort((int16_t)e->Short());
    case a_long:
    case a_int:
        return lisp->provideInteger(e->Integer());
    case a_decimal:
        return lisp->provideFloat(e->Decimal());
    case a_float:
        return lisp->provideNumber(e->Float());
    case a_complex:
        return lisp->provideComplex(((Tamgucomplexe *)e)->value);
    case a_string:
    {
        string v = e->String();
        return new Stringbyte(v);
    }
    case a_ustring:
    {
        wstring v = e->UString();
        return lisp->provideString(v);
    }
    case a_hvector:
    {
        Shorts *f = new Shorts();
        Tamguhvector *v = (Tamguhvector *)e;
        for (long i = 0; i < v->Size(); i++)
            f->liste.push_back(v->values[i]);
        return f;
    }
    case a_dvector:
    {
        Floats *f = lisp->provideFloats();
        Tamgudvector *v = (Tamgudvector *)e;
        for (long i = 0; i < v->Size(); i++)
            f->liste.push_back(v->values[i]);
        return f;
    }
    case a_ivector:
    {
        Integers *f = lisp->provideIntegers();
        Tamguivector *v = (Tamguivector *)e;
        for (long i = 0; i < v->Size(); i++)
            f->liste.push_back(v->values[i]);
        return f;
    }
    case a_fvector:
    {
        Numbers *f = lisp->provideNumbers();
        Tamgufvector *v = (Tamgufvector *)e;
        for (long i = 0; i < v->Size(); i++)
            f->liste.push_back(v->values[i]);
        return f;
    }
    case a_uvector:
    {
        Strings *f = lisp->provideStrings();
        Tamguuvector *v = (Tamguuvector *)e;
        u_ustring u;

        for (long i = 0; i < v->Size(); i++)
        {
            u = _w_to_u(v->values[i]);
            f->liste.push_back(u);
        }
        return f;
    }
    case a_svector:
    {
        Stringbytes *f = new Stringbytes();
        Tamgusvector *v = (Tamgusvector *)e;
        for (long i = 0; i < v->Size(); i++)
        {
            f->liste.push_back(v->values[i]);
        }
        return f;
    }
    case a_vector:
    {
        List *l = lisp->provideList();
        Tamguvector *v = (Tamguvector *)e;
        for (long i = 0; i < v->Size(); i++)
        {
            l->append(convert_to_lispe(v->values[i], idthread));
        }
        return l;
    }
    case a_list:
    {
        LList *l = new LList(&lisp->delegation->mark);
        Tamgulist *v = (Tamgulist *)e;
        for (auto &it : v->values)
        {
            l->append(convert_to_lispe(it, idthread));
        }
        l->liste.reverse(lisp);
        return l;
    }
    case a_map:
    {
        Tamgumap *m = (Tamgumap *)e;
        Dictionary *d = lisp->provideDictionary();
        string v;
        for (auto &it : m->values)
        {
            v = it.first;
            d->recording(v, convert_to_lispe(it.second, idthread));
        }
        return d;
    }
    case a_mapi:
    {
        Tamgumapi *m = (Tamgumapi *)e;
        Dictionary_i *d = lisp->provideDictionary_i();
        for (auto &it : m->values)
        {
            d->recording(it.first, convert_to_lispe(it.second, idthread));
        }
        return d;
    }
    case a_mapf:
    {
        Tamgumapf *m = (Tamgumapf *)e;
        Dictionary_n *d = lisp->provideDictionary_n();
        for (auto &it : m->values)
        {
            d->recording(it.first, convert_to_lispe(it.second, idthread));
        }
        return d;
    }
    case a_treemap:
    {
        Tree *d = lisp->provideTree();
        Tamgutreemap *m = (Tamgutreemap *)e;
        string v;
        for (auto &it : m->values)
        {
            v = it.first;
            d->recording(v, convert_to_lispe(it.second, idthread));
        }
        return d;
    }
    case a_treemapi:
    {
        Tree_i *d = lisp->provideTree_i();
        Tamgutreemapi *m = (Tamgutreemapi *)e;
        for (auto &it : m->values)
        {
            d->recording(it.first, convert_to_lispe(it.second, idthread));
        }
        return d;
    }
    default:
        if (e->Type() == Tamgutreemapf::idtype)
        {
            Tree_n *d = lisp->provideTree_n();
            Tamgutreemapf *m = (Tamgutreemapf *)e;
            for (auto &it : m->values)
            {
                d->recording(it.first, convert_to_lispe(it.second, idthread));
            }
            return d;
        }
        if (e->isMapContainer())
        {
            string s = e->Typestring();
            char atree = 0;
            if (s.find("tree") != -1)
                atree = 3;
            TamguIteration *itr = e->Newiteration(false);
            Tamgu *k;
            Element *v;
            Element *d = NULL;
            char key_type = 0;
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
            {
                // We first evaluate the kind of keys
                k = itr->IteratorKey();
                v = convert_to_lispe(itr->IteratorValue(), idthread);
                if (!key_type)
                {
                    if (k->isInteger() || k->isLong() || k->isShort())
                    {
                        if (atree)
                            d = new Tree_i();
                        else
                            d = new Dictionary_i();
                        key_type = 1 + atree;
                    }
                    else
                    {
                        if (k->isNumber())
                        {
                            if (atree)
                                d = new Tree_n();
                            else
                                d = new Dictionary_n();
                            key_type = 2 + atree;
                        }
                        else
                        {
                            if (atree)
                                d = new Tree();
                            else
                                d = new Dictionary();
                            key_type = 3 + atree;
                        }
                    }
                }
                switch (key_type)
                {
                case 1:
                    ((Dictionary_i *)d)->recording(k->Integer(), v);
                    break;
                case 2:
                    ((Dictionary_n *)d)->recording(k->Float(), v);
                    break;
                case 3:
                    s = k->String();
                    ((Dictionary *)d)->recording(s, v);
                    break;
                case 4:
                    ((Tree_i *)d)->recording(k->Integer(), v);
                    break;
                case 5:
                    ((Tree_n *)d)->recording(k->Float(), v);
                    break;
                case 6:
                    s = k->String();
                    ((Tree *)d)->recording(s, v);
                    break;
                }
            }
            itr->Release();
            return d;
        }
        return new Stringbyte(e->String());
    }
}

//------------------------------------------------------------------------------------------------------------------
void Tamgulispe::build(Tamgu *contextualpattern, TamguCall *callfunc, short idthread, string &code)
{
    long nbelements = callfunc->Size();

    if (nbelements == 1)
    {
        Tamgu* var = callfunc->Argument(0);
        Tamgu* a = callfunc->Evaluate(0, contextualpattern, idthread);
        code = a->String();
        if (var->isCallVariable() && code.find(" ") == string::npos) {
            string name = globalTamgu->Getsymbol(var->Name());
            short label = lisp->encode(name);
            Element* e = convert_to_lispe(a, idthread);
            lisp->storing_variable(e, label);
            code = "";
        }        
        return;
    }

    bool encoded = false;
    binSet variables;
    for (long i = 0; i < nbelements; i++)
    {
        if (i)
            code += " ";
        Tamgu* a = callfunc->Argument(i);
        if (a->isCallVariable())
        {
            string name = globalTamgu->Getsymbol(a->Name());
            code += name;
            short label = lisp->encode(name);
            if (!variables.check(label))
            {
                a = callfunc->Evaluate(i, contextualpattern, idthread);
                variables.push(label);
                Element* e = convert_to_lispe(a, idthread);
                lisp->storing_variable(e, label);
            }
        }
        else
        {
            a = callfunc->Evaluate(i, contextualpattern, idthread);
            if (a->isContainer())
            {
                code += "'";
                code += a->JSonString();
            }
            else
                code += a->String();
            encoded = true;
        }
    }

    if (!encoded)
        code = "";
}
//------------------------------------------------------------------------------------------------------------------
// A sample of how to implement a method for your new class...
Tamgu *Tamgulispe::MethodExecute(Tamgu *contextualpattern, short idthread, TamguCall *callfunc)
{

    string code;
    build(contextualpattern, callfunc, idthread, code);
    if (code == "")
        return aTRUE;

    Element *e = lisp->execute(code);
    if (e->isError())
    {
        code = e->toString(lisp);
        e->release();
        return globalTamgu->Returnerror(code, idthread);
    }
    Tamgu *a = convert_to_tamgu(e, idthread);
    e->release();
    return a;
}
//------------------------------------------------------------------------------------------------------------------
// A sample of how to implement a method for your new class...
Tamgu *Tamgulispe::MethodEval(Tamgu *contextualpattern, short idthread, TamguCall *callfunc)
{

    string code;
    build(contextualpattern, callfunc, idthread, code);

    if (code == "")
        return aTRUE;

    Element *e = lisp->eval(code);
    if (e->isError())
    {
        code = e->toString(lisp);
        e->release();
        return globalTamgu->Returnerror(code, idthread);
    }
    Tamgu *a = convert_to_tamgu(e, idthread);
    e->release();
    return a;
}

Element *parcours(Tamgu *v, LispE *lsp, short idthread)
{
    short a;
    if (v->isVectorContainer())
    {
        List *l;
        long i = 1;
        if (v->Size())
        {
            Tamgu *f = v->getvalue(0);
            a = f->Action();
            switch (a)
            {
            case a_callfunction:
                l = new Lispecallfunction(f, idthread);
                break;
            case a_callprocedure:
                l = new Lispecallprocedure(f, idthread);
                break;
            case a_callcommon:
                l = new Lispecallcommon(f, idthread);
                break;
            case a_atom:
            {
                l = lsp->provideList();
                a = f->Name();
                string s = globalTamgu->Getsymbol(a);
                a = lsp->encode(s);
                Element *e = lsp->provideAtomOrInstruction(a);
                l->append(e);
                break;
            }
            default:
                i = 0;
                l = lsp->provideList();
            }
            for (; i < v->Size(); i++)
                l->append(parcours(v->getvalue(i), lsp, idthread));

            return l;
        }
        else
            return lsp->n_null;
    }
    if (v->isConst())
        return local_lispe->convert_to_lispe(v, idthread);

    a = v->Action();
    if (a == a_atom)
    {
        if (globalTamgu->isDeclared(v->Name(), idthread))
            return new Lispetamgusymbol((Tamgusymbol *)v, idthread);
        a = v->Name();
    }

    string s = globalTamgu->Getsymbol(a);
    a = lsp->encode(s);
    return lsp->provideAtomOrInstruction(a);
}

static Tamgu *Calllispe(Tamgulisp *v, short idthread)
{
    Tamgulispelist *ll;
    Element *e;
    if (v->lispe == NULL)
    {
        if (local_lispe == NULL)
            local_lispe = new Tamgulispe(globalTamgu);
        try
        {
            e = parcours(v, local_lispe->lisp, idthread);
        }
        catch (Error *err)
        {
            string s = err->toString(local_lispe->lisp);
            return globalTamgu->Returnerror(s, idthread);
        }
        ll = new Tamgulispelist(e);
        local_lispe->buffers.push_back(ll);
        ll->Setreference();
        v->lispe = ll;
    }
    else
        ll = (Tamgulispelist *)v->lispe;

    return local_lispe->eval(ll->liste, idthread);
}
//------------------------------------------------------------------------------------------------------------------
// If you need to implement an external library... Otherwise remove it...
// When Tamgu (탐구) loads a library, it looks for this method in particular, which then make this object available into Tamgu (탐구)
extern "C"
{
    Exporting bool tamgulispe_InitialisationModule(TamguGlobal *global, string version)
    {
        if (version != TamguVersion())
            return false;

        global->Update();
        global->lispe = Calllispe;

        return Tamgulispe::InitialisationModule(global, version);
    }
}
