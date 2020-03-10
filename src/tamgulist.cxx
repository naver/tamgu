/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgulist.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgulist.h"
#include "tamguvector.h"
#include "tamgumap.h"
#include "tamgulist.h"
#include "instructions.h"
#include "constobjects.h"
#include "tamguivector.h"
#include "tamgubyte.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<listMethod>  Tamgulist::methods;
Exporting hmap<string, string> Tamgulist::infomethods;
Exporting bin_hash<unsigned long> Tamgulist::exported;

Exporting short Tamgulist::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgulist::AddMethod(TamguGlobal* global, string name, listMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgulist::Setidtype(TamguGlobal* global) {
    Tamgulist::InitialisationModule(global,"");
}


   bool Tamgulist::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    Tamgulist::idtype = global->Getid("list");

    Tamgulist::AddMethod(global, "clear", &Tamgulist::MethodClear, P_NONE, "clear(): clear the container.");
    Tamgulist::AddMethod(global, "flatten", &Tamgulist::MethodFlatten, P_NONE, "flatten(): flatten a vector structure.");

    Tamgulist::AddMethod(global, "remove", &Tamgulist::MethodRemove, P_ONE, "remove(e): remove 'e' from the vector.");

    Tamgulist::AddMethod(global, "reverse", &Tamgulist::MethodReverse, P_NONE, "reverse(): reverse a vector.");
    Tamgulist::AddMethod(global, "unique", &Tamgulist::MethodUnique, P_NONE, "unique(): remove duplicate elements.");

    Tamgulist::AddMethod(global, "permute", &Tamgulist::MethodPermute, P_NONE, "permute(): permute the values in the vector after each call.");

    Tamgulist::AddMethod(global, "join", &Tamgulist::MethodJoin, P_ONE, "join(string sep): Produce a string representation for the container.");
    Tamgulist::AddMethod(global, "shuffle", &Tamgulist::MethodShuffle, P_NONE, "shuffle(): shuffle the values in the list.");
    Tamgulist::AddMethod(global, "last", &Tamgulist::MethodLast, P_NONE, "last(): return the last element.");
    Tamgulist::AddMethod(global, "first", &Tamgulist::MethodFirst, P_NONE, "first(): return the first element.");
    Tamgulist::AddMethod(global, "sum", &Tamgulist::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgulist::AddMethod(global, "product", &Tamgulist::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgulist::AddMethod(global, "push", &Tamgulist::MethodPush, P_ATLEASTONE, "push(v): Push a value at the end the list.");
    Tamgulist::AddMethod(global, "pushlast", &Tamgulist::MethodPush, P_ATLEASTONE, "pushlast(v): Push a value at the end the list.");
    Tamgulist::AddMethod(global, "pushfirst", &Tamgulist::MethodPushfirst, P_ONE, "pushfirst(v): Push a value into the list in first position.");
    Tamgulist::AddMethod(global, "pop", &Tamgulist::MethodPop, P_NONE | P_ONE, "pop(i): Erase an element from the list at position i.");
    Tamgulist::AddMethod(global, "popfirst", &Tamgulist::MethodPopfirst, P_NONE, "popfirst(): remove and return the first element.");
    Tamgulist::AddMethod(global, "poplast", &Tamgulist::MethodPoplast, P_NONE, "poplast(): remove and return the last element.");
    Tamgulist::AddMethod(global, "merge", &Tamgulist::MethodMerge, P_ONE, "merge(v): Merge v into the list.");
    Tamgulist::AddMethod(global, "insert", &Tamgulist::MethodInsert, P_TWO, "insert(int i,v): Insert v at position i.");


    if (version != "") {
        global->newInstance[Tamgulist::idtype] = new Tamgulist(global);
        global->RecordMethods(Tamgulist::idtype, Tamgulist::exported);
    }

    Tamguring::InitialisationModule(global, version);
    return true;
}

Exporting TamguIteration* Tamgulist::Newiteration(bool direction) {
    return new TamguIterationlist(this, direction);
}

Exporting Tamgu* Tamgulist::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    if (context->isBoolean()) {
        Locking _lock(this);
        for (auto& it : values) {
            if (it->same(a) == aTRUE)
                return aTRUE;
        }
        return aFALSE;
    }

    long i = 0;
    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Doublelocking _lock(this, v);
        for (auto& it : values) {
            if (it->same(a) == aTRUE)
                v->values.push_back(i);
            i++;
        }
        return v;
    }

    Locking _lock(this);
    for (auto& it : values) {
        if (it->same(a) == aTRUE)
            return globalTamgu->Provideint(i);
        i++;
    }

    return aNOELEMENT;
}


Exporting Tamgu* Tamgulist::getvalue(BLONG i) {
    Locking _lock(this);
    if (i < 0 || i >= values.size())
        return aNOELEMENT;


    for (auto& it : values) {
        if (!i)
            return (it);
        i--;
    }
    return aNOELEMENT;
}


Exporting void Tamgulist::Cleanreference(short inc) {
    Locking* _lock = _getlock(this);
    for (auto& a : values)
        a->Removecontainerreference(inc);
    _cleanlock(_lock);
}

Exporting void Tamgulist::Setreference(short inc) {
    if (loopmark)
        return;
    
    reference += inc;
    protect=false;
    loopmark=true;
    
    Locking* _lock = _getlock(this);
    for (auto& it : values)
        it->Addreference(inc);
    _cleanlock(_lock);
    
    loopmark=false;
}

Exporting void Tamgulist::Setreference() {
    if (loopmark)
        return;
    
    ++reference;
    protect=false;
    loopmark=true;
    
    Locking* _lock = _getlock(this);
    for (auto& it : values)
        it->Addreference(1);
    _cleanlock(_lock);
    
    loopmark=false;
}


static void resetList(Tamgulist* klist, short inc) {
    klist->reference -= inc;

    Locking* _lock = _getlock(klist);
    list<Tamgu*>& values = klist->values;
    if (values.size() == 0) {
        _cleanlock(_lock);
        return;
    }
    
    for (auto& itx : values)
        itx->Removereference(inc);

    _cleanlock(_lock);
}

Exporting void Tamgulist::Resetreference(short inc) {
    if ((reference + containerreference - inc) > 0)
        resetList(this, inc);
    else {
        resetList(this, inc + 1 - protect);
        if (!protect) {
            if (idtracker != -1)
                globalTamgu->RemoveFromTracker(idtracker);
            delete this;
        }
    }
}

Exporting Tamgu* Tamgulist::Pushfirst(Tamgu* a) {
    Locking _lock(this);
    a = a->Atom();
    values.push_front(a);
    a->Addreference(reference+1);
    return this;
}

Exporting Tamgu* Tamgulist::Push(Tamgu* a) {
    Locking _lock(this);
    a = a->Atom();
    values.push_back(a);
    a->Addreference(reference+1);
    return this;
}

Exporting Tamgu* Tamgulist::Pop(Tamgu* v) {
    Locking _lock(this);
    if (!values.size())
        return aFALSE;

    list<Tamgu*>::iterator it;
    for (it = values.begin(); it != values.end(); ++it) {
        if ((*it)->same(v)) {
            v = *it;
            values.erase(it);
            v->Removereference(reference + 1);
            return aTRUE;
        }
    }
    return aFALSE;
}

Exporting void Tamgulist::Clear() {
    //To set a variable back to empty
    Locking _lock(this);

    for (auto& it : values) {
        it->Removereference(reference + 1);
    }
    values.clear();
}



Exporting string Tamgulist::String() {
    Locking _lock(this);
    if (loopmark)
        return("[...]");
    TamguCircular _c(this);
    string res;

    res = "[";
    bool beg = true;
    string sx;
    for (auto& it : values) {
        if (beg == false)
            res += ",";
        beg = false;
        sx = it->StringToDisplay();
        if (!it->isString() || it->isContainer())
            res += sx;
        else
            stringing(res, sx);
    }
    res += "]";
    return res;
}

Exporting string Tamgulist::JSonString() {
    Locking _lock(this);
    if (loopmark)
        return("");
    TamguCircular _c(this);
    string res;

    res = "[";
    bool beg = true;
    string sx;
    for (auto& it : values) {
        if (beg == false)
            res += ",";
        beg = false;
        sx = it->JSonString();
        if (!it->isString() || it->isContainer())
            res += sx;
        else
            jstringing(res, sx);
    }
    res += "]";
    return res;
}



Exporting long Tamgulist::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Tamgulist::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Tamgulist::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Tamgulist::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgulist::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting void Tamgulist::Insert(long idx, Tamgu* ke) {
    ke = ke->Atom();
    if (idx<0)
        idx=0;
    Locking _lock(this);
    if (idx>=values.size())
        values.push_back(ke);
    else {
        list<Tamgu*>::iterator itl = values.begin();
        for (long nb = 0; nb < idx; nb++)
            itl++;
        values.insert(itl, ke);
    }
    ke->Addreference(reference+1);
}

Exporting bool Tamgulist::Permute() {
    Locking _lock(this);
    return next_permutation(values.begin(), values.end());
}

Exporting void Tamgulist::Shuffle() {
    Locking _lock(this);

    vector<Tamgu*> vb;
    for (auto& it : values)
        vb.push_back(it);
    size_t sz = vb.size();
    size_t i, f;
    long mx = sz;
    Tamgu* v;
    for (i = 0; i < sz; i++) {
        f = localrandom(mx--);
        if (mx != f) {
            v = vb[mx];
            vb[mx] = vb[f];
            vb[f] = v;
        }
    }

    values.clear();
    for (i = 0; i < sz; i++)
        values.push_back(vb[i]);
}

Exporting Tamgu* Tamgulist::Unique() {
    Locking _lock(this);
    Tamgulist* klist = new Tamgulist;

    map<string, Tamgu*> inter;
    string k;
    for (auto& it : values) {
        k = it->String();
        if (inter.find(k) != inter.end()) {
            if (inter[k]->same(it)->Boolean() == false)
                klist->Push(it);
        }
        else {
            inter[k] = it;
            klist->Push(it);
        }
    }
    return klist;
}

Exporting Tamgu* Tamgulist::Popfirst() {
    Locking _lock(this);
    if (values.size() == 0)
        return aNOELEMENT;
    Tamgu* kres = values.front();
    values.pop_front();
    kres->Resetreference(reference);
    kres->Protect();
    return kres;
}

Exporting Tamgu* Tamgulist::Poplast() {
    Locking _lock(this);
    if (values.size() == 0)
        return aNOELEMENT;
    Tamgu* kres = values.back();
    values.pop_back();
    kres->Resetreference(reference);
    kres->Protect();
    return kres;
}

Exporting Tamgu* Tamgulist::Merging(Tamgu* ke) {
    if (!ke->isContainer()) {
        Push(ke);
        return this;
    }

    if (ke->Size() == 0)
        return this;

    Doublelocking _lock(this, ke);
    //Three cases:
    if (ke->Type() == a_list) {
        Tamgulist* klist = (Tamgulist*)ke;

        for (auto& it : klist->values)
            Push(it);
        return this;
    }

    TamguIteration* itr = ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next())
        Push(itr->Value());
    itr->Release();

    return this;
}

Exporting Tamgu* Tamgulist::Combine(Tamgu* ke) {
    if (ke->Size() == 0)
        return this;
    
    Tamgulist* vect = new Tamgulist;
    Tamgu* val;
    
    if (!ke->isContainer()) {
        Locking _lock(this);
        for (auto& a: values) {
            val=new Tamgulist;
            val->Push(a);
            val->Push(ke);
            vect->Push(val);
        }
        return vect;
    }
    
    
    Doublelocking _lock(this, ke);

    list<Tamgu*>::iterator it = values.begin();
    
    TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        if (it == values.end()) {
            val=new Tamgulist;
            val->Push(itr->IteratorValue());
            vect->Push(val);
            continue;
        }
        
        val=new Tamgulist;
        val->Push(*it);
        val->Push(itr->IteratorValue());
        vect->Push(val);
        it++;
    }
    itr->Release();
    
    for (;it != values.end(); it++) {
        val=new Tamguvector;
        val->Push(*it);
        vect->Push(val);
    }
    
    return vect;
}

Exporting Tamgu* Tamgulist::Map(short idthread) {
    Locking _lock(this);
    if (loopmark)
        return aNULL;
        
    loopmark=true;
    Tamgumap* kmap = globalTamgu->Providemap();

    long nb = 0;
    char ch[20];
    for (auto& it : values) {
        sprintf_s(ch, 20, "%ld", nb);
        kmap->Push(ch, it);
        nb++;
    }
    
    loopmark=false;
    return kmap;
}

Exporting Tamgu* Tamgulist::Vector(short idthread) {
    Locking _lock(this);
    Tamguvector* kvect = globalTamgu->Providevector();
    //We copy all values from ke to this
    kvect->values.reserve(values.size());

    for (auto& it : values)
        kvect->Push(it);
    return kvect;
}

Exporting Tamgu*  Tamgulist::Put(Tamgu* idx, Tamgu* ke, short idthread) {

    Doublelocking _lock(this, ke);

    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }

        if (ke->Type() == a_list) {
            Tamgulist* kvect = (Tamgulist*)ke;
            //We copy all values from ke to this
            Clear();

            for (auto& it : kvect->values) {
                idx = it->Atom();
                idx->Addreference(reference+1);
                values.push_back(idx);
            }
            return aTRUE;
        }

        if (ke->isVectorContainer()) {
            //We copy all values from ke to this
            Clear();
            for (int it = 0; it < ke->Size(); ++it) {
                idx  = ke->getvalue(it);
                idx = idx->Atom();
                idx->Addreference(reference+1);
                values.push_back(idx);
            }
            return aTRUE;
        }
        //We gather all the keys from the MAP
        if (ke->isMapContainer()) {
            Clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
                idx = itr->Key();
                idx = idx->Atom();
                idx->Addreference(reference+1);
                values.push_back(idx);
            }
            itr->Release();
            return aTRUE;
        }
        ke = ke->Vector(idthread);
        if (!ke->isVectorContainer())
            return globalTamgu->Returnerror("Cannot set this value", idthread);

        Clear();
        //We copy all values from ke to this
        for (long it = 0; it < ke->Size(); ++it) {
            idx  = ke->getvalue(it);
            idx = idx->Atom();
            idx->Addreference(reference+1);
            values.push_back(idx);
        }
        ke->Release();
        return aTRUE;
    }

    if (idx->isInterval()) {

        //First we remove the values...
        long lkey = ((TamguIndex*)idx)->left->Integer();
        if (lkey < 0)
            lkey = values.size() + lkey;

        long rkey = ((TamguIndex*)idx)->right->Integer();
        if (rkey < 0)
            rkey = values.size() + rkey;

        if (rkey < lkey || rkey >= values.size() || lkey >= values.size()) {
            if (globalTamgu->erroronkey)
                globalTamgu->Returnerror("Wrong index", idthread);
            return aTRUE;
        }

        list<Tamgu*>::iterator it;
        listValue(it, lkey);
        vector<list<Tamgu*>::iterator > vit;
        Tamgu* krkey;
        while (rkey > lkey) {
            krkey = *it;
            vit.push_back(it);
            krkey->Removereference(reference + 1);
            rkey--;
            ++it;
        }

        for (long i = vit.size() - 1; i >= 0; i--)
            values.erase(vit[i]);

        listValue(it, lkey);
        if (ke->isVectorContainer()) {
            for (int i = 0; i < ke->Size(); i++) {
                krkey = ke->getvalue(i);
                krkey = krkey->Atom();
                values.insert(it, krkey);
                krkey->Addreference(reference+1);
            }
            return aTRUE;
        }

        if (ke->Type() == a_list) {
            for (auto& iti : ((Tamgulist*)ke)->values) {
                krkey = iti;
                krkey = krkey->Atom();
                values.insert(it, krkey);
                krkey->Addreference(reference+1);
            }
            return aTRUE;
        }

        if (ke->isContainer()) {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
                krkey = itr->Value();
                krkey = krkey->Atom();
                values.insert(it, krkey);
                krkey->Addreference(reference+1);
            }
            itr->Release();
            return aTRUE;
        }

        krkey = ke->Atom();
        values.insert(it, krkey);
        krkey->Addreference(reference+1);
        return aTRUE;
    }

    long ikey = idx->Integer();
    list<Tamgu*>::iterator it;
    long mx = values.size();

    if (ikey == mx)
        Push(ke);
    else {
        if (ikey > mx) {
            for (; mx < ikey; mx++)
                values.push_back(aNULL);
            Push(ke);
        }
        else {
            if (ikey < 0) {
                ikey = mx + ikey;
                if (ikey < 0)
                    return globalTamgu->Returnerror("Cannot set this value", idthread);
            }
            ke = ke->Atom();
            listValue(it, ikey);
            if (*it != NULL)
                (*it)->Removereference(reference + 1);
            *it = ke;
            ke->Addreference(reference+1);
        }
    }

    return aTRUE;
}

Exporting Tamgu* Tamgulist::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {

        Tamgu* ke;
        //In this case, we copy the elements from the vector to the map, using the position as index
        if (contextualpattern->isMapContainer()) {
            Tamgumap* map = (Tamgumap*)Selectamap(contextualpattern);
            size_t i = 0;

            char ch[20];
            for (auto& it : values) {
                sprintf_s(ch, 20, "%zd", i);
                ke = it;
                map->Push(ch, ke);
                i++;
            }
            return map;
        }

        if (contextualpattern->isNumber()) {
            long v = Size();
            return globalTamgu->Provideint(v);
        }

        //Then some values must be replaced with their evaluation
        if (evaluate == true) {
            Tamgulist* kvect = new Tamgulist;

            for (auto& it : values) {
                ke = it->Eval(aNULL, aNULL, idthread);
                if (ke == aRAISEERROR) {
                    kvect->Release();
                    return aRAISEERROR;
                }
                kvect->Push(ke);
            }
            return kvect;
        }

        return this;
    }

    Tamgu* keyright = NULL;
    
    long ikey = ((TamguIndex*)idx)->left->Getinteger(idthread);
    if (idx->isInterval())
        keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);

    list<Tamgu*>::iterator it;
    int i;
    Tamgulist* kvect;
    if (ikey < 0)
        ikey = values.size() + ikey;
    if (ikey < 0 || ikey >= values.size()) {
        if (ikey != values.size() || keyright == NULL) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    it = values.begin();
    for (i = 0; i < ikey; i++) ++it;


    if (keyright == NULL)
        return *it;

    long iright = keyright->Integer();
    keyright->Release();

    if (iright < 0 || keyright == aNULL) {
        iright = values.size() + iright;
        if (iright<ikey) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    else {
        if (iright>values.size()) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    //In this case, we must create a new list
    kvect = new Tamgulist;
    for (; i < iright; i++, ++it)
        kvect->Push(*it);

    return kvect;
}


Exporting Tamgu* Tamgulist::andset(Tamgu* b, bool itself) {
    Tamgulist* ref;


    Tamgu* ke;
    if (!b->isContainer()) {
        if (itself)
            ref = this;
        else
            ref = (Tamgulist*)Atom(true);
        Locking _lock(this);
        for (auto& itl : ref->values) {
            ke = itl->andset(b, true);
            if (ke->isError()) {
                ref->Release();
                return ke;
            }
        }
        return ref;
    }

    Doublelocking _lock(this, b);

    ref = new Tamgulist;

    TamguIteration* itr = b->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        for (auto& itl : ref->values) {
            ke = itr->IteratorValue();
            if (itl->same(ke) == aTRUE) {
                ref->Push((itl));
                break;
            }
        }
    }
    itr->Release();
    return ref;

}

Exporting Tamgu* Tamgulist::orset(Tamgu* b, bool itself) {
    Tamgulist* ref;

    Tamgu* ke;
    if (itself)
        ref = this;
    else
        ref = (Tamgulist*)Atom(true);
    if (!b->isContainer()) {
        Locking _lock(this);
        for (auto& itl : ref->values) {
            ke = itl->orset(b, true);
            if (ke->isError()) {
                ref->Release();
                return ke;
            }
        }
        return ref;
    }
    
    ref->Merging(b);
    return ref;
}

Exporting Tamgu* Tamgulist::xorset(Tamgu* b, bool itself) {
    Tamgulist* ref;

    
    if (!b->isContainer()) {
        if (itself)
            ref = this;
        else
            ref = (Tamgulist*)Atom(true);
            
        Locking _lock(this);
        
        Tamgu* ke;
        for (auto& itl : ref->values) {
            ke = itl->xorset(b, true);
            if (ke->isError()) {
                ref->Release();
                return ke;
            }
        }
        return ref;
    }

    Doublelocking _lock(this, b);

    ref = new Tamgulist;
    vector<Tamgu*> vals;
    TamguIteration* itr = b->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next())
        vals.push_back(itr->Value());
    itr->Release();

    bool found=false;
    long itv;
    for (auto& itl : ref->values) {
        for (itv = 0; itv < vals.size(); itv++) {
            if (vals[itv] == NULL)
                continue;

            found = false;
            if (itl->same(vals[itv]) == aTRUE) {
                vals[itv]->Release();
                vals[itv] = NULL;
                found = true;
                break;
            }
        }

        if (!found)
            ref->Push(itl);
    }

    for (itv = 0; itv < vals.size(); itv++) {
        if (vals[itv] == NULL)
            continue;
        ref->Push(vals[itv]);
        vals[itv]->Release();
    }

    return ref;
}


Exporting Tamgu* Tamgulist::plus(Tamgu* b, bool itself) {
    Tamgulist* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgulist*)Atom(true);


    Tamgu* ke;
    Tamgu* kv;
    Doublelocking _lock(ref, b);
    if (loopmark)
        return aNULL;

    TamguCircular _c(this);


    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (auto& itl : ref->values) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = itl->plus(kv, true);
            if (ke->isError()) {
                itr->Release();
                ref->Release();
                return ke;
            }
            itr->Next();
        }
        itr->Release();
        return ref;
    }


    for (auto& itl : ref->values) {
        ke = itl->plus(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Tamgu* Tamgulist::minus(Tamgu* b, bool itself) {
    Tamgulist* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgulist*)Atom(true);


    Tamgu* ke;
    Tamgu* kv;

    Doublelocking _lock(ref, b);
    if (loopmark)
        return aNULL;

    TamguCircular _c(this);


    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (auto& itl : ref->values) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = itl->minus(kv, true);
            if (ke->isError()) {
                itr->Release();
                ref->Release();
                return ke;
            }
            itr->Next();
        }
        itr->Release();
        return ref;
    }


    for (auto& itl : ref->values) {
        ke = itl->minus(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Tamgu* Tamgulist::multiply(Tamgu* b, bool itself) {
    Tamgulist* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgulist*)Atom(true);


    Tamgu* ke;
    Tamgu* kv;

    Doublelocking _lock(ref, b);
    if (loopmark)
        return aNULL;

    TamguCircular _c(this);


    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (auto& itl : ref->values) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = itl->multiply(kv, true);
            if (ke->isError()) {
                ref->Release();
                itr->Release();
                return ke;
            }
            itr->Next();
        }
        itr->Release();
        return ref;
    }


    for (auto& itl : ref->values) {
        ke = itl->multiply(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Tamgu* Tamgulist::divide(Tamgu* b, bool itself) {
    Tamgulist* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgulist*)Atom(true);


    Tamgu* ke;
    Tamgu* kv;

    Doublelocking _lock(ref, b);
    if (loopmark)
        return aNULL;

    TamguCircular _c(this);


    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (auto& itl : ref->values) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = itl->divide(kv, true);
            if (ke->isError()) {
                ref->Release();
                itr->Release();
                return ke;
            }
            itr->Next();
        }
        itr->Release();
        return ref;
    }


    for (auto& itl : ref->values) {
        ke = itl->divide(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Tamgu* Tamgulist::power(Tamgu* b, bool itself) {
    Tamgulist* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgulist*)Atom(true);


    Tamgu* ke;
    Tamgu* kv;

    Doublelocking _lock(ref, b);
    if (loopmark)
        return aNULL;

    TamguCircular _c(this);


    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (auto& itl : ref->values) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = itl->power(kv, true);
            if (ke->isError()) {
                ref->Release();
                itr->Release();
                return ke;
            }
            itr->Next();
        }
        itr->Release();
        return ref;
    }


    for (auto& itl : ref->values) {
        ke = itl->power(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Tamgu* Tamgulist::shiftleft(Tamgu* b, bool itself) {
    Tamgulist* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgulist*)Atom(true);


    Tamgu* ke;
    Tamgu* kv;

    Doublelocking _lock(ref, b);
    if (loopmark)
        return aNULL;

    TamguCircular _c(this);


    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (auto& itl : ref->values) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = itl->shiftleft(kv, true);
            if (ke->isError()) {
                ref->Release();
                itr->Release();
                return ke;
            }
            itr->Next();
        }
        itr->Release();
        return ref;
    }


    for (auto& itl : ref->values) {
        ke = itl->shiftleft(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Tamgu* Tamgulist::shiftright(Tamgu* b, bool itself) {
    Tamgulist* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgulist*)Atom(true);


    Tamgu* kv;

    Tamgu* ke;
    Doublelocking _lock(ref, b);
    if (loopmark)
        return aNULL;

    TamguCircular _c(this);


    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (auto& itl : ref->values) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = itl->shiftright(kv, true);
            if (ke->isError()) {
                ref->Release();
                itr->Release();
                return ke;
            }
            itr->Next();
        }
        itr->Release();
        return ref;
    }


    for (auto& itl : ref->values) {
        ke = itl->shiftright(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Tamgu* Tamgulist::mod(Tamgu* b, bool itself) {
    Tamgulist* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgulist*)Atom(true);


    Tamgu* ke;
    Tamgu* kv;

    Doublelocking _lock(ref, b);
    if (loopmark)
        return aNULL;

    TamguCircular _c(this);


    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (auto& itl : ref->values) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = itl->mod(kv, true);
            if (ke->isError()) {
                ref->Release();
                itr->Release();
                return ke;
            }
            itr->Next();
        }
        itr->Release();
        return ref;
    }


    for (auto& itl : ref->values) {
        ke = itl->mod(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}



Exporting Tamgu* Tamgulist::same(Tamgu* a) {

    if (a->Type() != a_list)
        return aFALSE;
    Tamgulist* v = (Tamgulist*)a;
    Doublelocking _lock(this, v);

    if (v->values.size() != values.size())
        return aFALSE;

    list<Tamgu*>::iterator it;
    list<Tamgu*>::iterator itv;
    for (it = values.begin(), itv = v->values.begin(); it != values.end(); ++it, ++itv) {
        if ((*it)->same(*itv) == aFALSE)
            return aFALSE;
    }
    return aTRUE;
}
//----------------------------------------------------------------------------------
Exporting void Tamgulist::Storevalue(string& u) {
    Tamgu* a = globalTamgu->Providestring(u);
    a->Addreference(reference+1);
    Locking* _lock = _getlock(this);
    values.push_back(a);
    _cleanlock(_lock);
}

Exporting void Tamgulist::Storevalue(wstring& u) {
    Tamgu* a = globalTamgu->Provideustring(u);
    a->Addreference(reference+1);
    Locking* _lock = _getlock(this);
    values.push_back(a);
    _cleanlock(_lock);
}

Exporting void Tamgulist::storevalue(string u) {
    Tamgu* a = globalTamgu->Providestring(u);
    a->Addreference(reference+1);
    Locking* _lock = _getlock(this);
    values.push_back(a);
    _cleanlock(_lock);
}

Exporting void Tamgulist::storevalue(wstring u) {
    Tamgu* a = globalTamgu->Provideustring(u);
    a->Addreference(reference+1);
    Locking* _lock = _getlock(this);
    values.push_back(a);
    _cleanlock(_lock);
}

Exporting void Tamgulist::storevalue(long u) {
    Tamgu* a = globalTamgu->Provideint(u);
    a->Addreference(reference+1);
    Locking* _lock = _getlock(this);
    values.push_back(a);
    _cleanlock(_lock);
}

Exporting void Tamgulist::storevalue(short u) {
    Tamgu* a = new Tamgushort(u);
    a->Addreference(reference+1);
    Locking* _lock = _getlock(this);
    values.push_back(a);
    _cleanlock(_lock);
}

Exporting void Tamgulist::storevalue(BLONG u) {
    Tamgu* a = new Tamgulong(u);
    a->Addreference(reference+1);
    Locking* _lock = _getlock(this);
    values.push_back(a);
    _cleanlock(_lock);
}

Exporting void Tamgulist::storevalue(float u) {
    Tamgu* a = new Tamgudecimal(u);
    a->Addreference(reference+1);
    Locking* _lock = _getlock(this);
    values.push_back(a);
    _cleanlock(_lock);
}

Exporting void Tamgulist::storevalue(double u) {
    Tamgu* a = globalTamgu->Providefloat(u);
    a->Addreference(reference+1);
    Locking* _lock = _getlock(this);
    values.push_back(a);
    _cleanlock(_lock);
}

Exporting void Tamgulist::storevalue(unsigned char u) {
    Tamgu* a = new Tamgubyte(u);
    a->Addreference(reference+1);
    Locking* _lock = _getlock(this);
    values.push_back(a);
    _cleanlock(_lock);
}

Exporting void Tamgulist::storevalue(wchar_t u) {
    wstring w;
    w = u;
    Tamgu* a = globalTamgu->Provideustring(w);
    a->Addreference(reference+1);
    Locking* _lock = _getlock(this);
    values.push_back(a);
    _cleanlock(_lock);
}


    //----------------------------------------------------------------------------------------------------------

    //We need to declare once again our local definitions.
Exporting basebin_hash<a_listMethod>  Tamguring::methods;
Exporting hmap<string, string> Tamguring::infomethods;
Exporting bin_hash<unsigned long> Tamguring::exported;

Exporting short Tamguring::idtype = 0;

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

    //MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguring::AddMethod(TamguGlobal* global, string name, a_listMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Tamguring::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();
    
    
    Tamguring::idtype = global->Getid("ring");
    
    Tamguring::AddMethod(global, "clear", &Tamguring::MethodClear, P_NONE, "clear(): clear the container.");
    Tamguring::AddMethod(global, "flatten", &Tamguring::MethodFlatten, P_NONE, "flatten(): flatten a vector structure.");
    
    Tamguring::AddMethod(global, "remove", &Tamguring::MethodRemove, P_ONE, "remove(e): remove 'e' from the vector.");
    Tamguring::AddMethod(global, "ngrams", &Tamguring::MethodNGrams, P_ONE|P_TWO, "ngrams(int nb, int sep): produces a ngrams svector.");
    
    Tamguring::AddMethod(global, "reverse", &Tamguring::MethodReverse, P_NONE, "reverse(): reverse a vector.");
    Tamguring::AddMethod(global, "unique", &Tamguring::MethodUnique, P_NONE, "unique(): remove duplicate elements.");
    Tamguring::AddMethod(global, "join", &Tamguring::MethodJoin, P_ONE, "join(string sep): Produce a string representation for the container.");
    Tamguring::AddMethod(global, "first", &Tamguring::MethodFirst, P_NONE, "first(): return the first element.");
    Tamguring::AddMethod(global, "last", &Tamguring::MethodLast, P_NONE, "last(): return the last element.");
    Tamguring::AddMethod(global, "sum", &Tamguring::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamguring::AddMethod(global, "product", &Tamguring::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamguring::AddMethod(global, "push", &Tamguring::MethodPush, P_ATLEASTONE, "push(v): Push a value into the vector.");
    Tamguring::AddMethod(global, "pushlast", &Tamguring::MethodPush, P_ATLEASTONE, "pushlast(v): Push a value at the end the vector.");
    Tamguring::AddMethod(global, "pushfirst", &Tamguring::MethodPushfirst, P_ATLEASTONE, "pushfirst(v): Push a value into the vector at the beginning.");
    Tamguring::AddMethod(global, "pop", &Tamguring::MethodPop, P_NONE | P_ONE, "pop(int i): Erase an element from the vector");
    Tamguring::AddMethod(global, "poplast", &Tamguring::MethodPoplast, P_NONE, "poplast(): Remove and return the last element of the ring");
    Tamguring::AddMethod(global, "popfirst", &Tamguring::MethodPopfirst, P_NONE, "popfirst(): Remove and return the first element of the ring");
    Tamguring::AddMethod(global, "merge", &Tamguring::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");
    Tamguring::AddMethod(global, "insert", &Tamguring::MethodInsert, P_TWO, "insert(int i,v): Insert v at position i.");
    
    if (version != "") {
        global->newInstance[Tamguring::idtype] = new Tamguring(global);
        global->RecordMethods(Tamguring::idtype, Tamguring::exported);
        global->newInstance[a_constvector] = new TamguConstvector(global);
        global->RecordMethods(a_constvector, Tamguring::exported);
    }
    
    return true;
}

Exporting TamguIteration* Tamguring::Newiteration(bool direction) {
    return new TamguIterationring(this, direction);
}

Exporting void Tamguring::Cleanreference(short inc) {
    atomic_ring_iterator<Tamgu*> it(values);
    while (!it.end()) {
        it.second->Removecontainerreference(inc);
        it.next();
    }
}

Exporting void Tamguring::Setreference(short inc) {
    if (loopmark)
        return;
    
    reference += inc;
    protect=false;
    loopmark=true;
    
    atomic_ring_iterator<Tamgu*> it(values);
    while (!it.end()) {
        it.second->Addreference(inc);
        it.next();
    }
    
    loopmark=false;
}

Exporting void Tamguring::Setreference() {
    if (loopmark)
        return;
    
    ++reference;
    protect=false;
    loopmark=true;
    
    atomic_ring_iterator<Tamgu*> it(values);
    while (!it.end()) {
        it.second->Addreference(1);
        it.next();
    }
    
    loopmark=false;
}


static void resetVector(Tamguring* kvect, short inc) {
    kvect->reference -= inc;
    
    atomic_ring_iterator<Tamgu*> it(kvect->values);
    while (!it.end()) {
        it.second->Removereference(inc);
        it.next();
    }
}

Exporting void Tamguring::Resetreference(short inc) {
    if ((reference + containerreference - inc) > 0)
        resetVector(this, inc);
    else {
        resetVector(this, inc + 1 - protect);
        if (!protect) {
            if (idtracker != -1)
                globalTamgu->RemoveFromTracker(idtracker);
            delete this;
        }
    }
}


Exporting Tamgu* Tamguring::in(Tamgu* context, Tamgu* a, short idthread) {
        //Three cases along the container type...
        //It is a Boolean, we expect false or true
        //It is an integer, we expect a position in v
        //It is a container, we are looking for all positions...
    
    atomic_ring_iterator<Tamgu*> it(values);
    
    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Locking _lock(v);
        while (!it.end()) {
            if (a->same(it.second) == aTRUE)
                v->values.push_back(it.first);
            it.next();
        }
        return v;
    }
    
    if (context->isNumber()) {
        while (!it.end()) {
            if (a->same(it.second) == aTRUE)
                return globalTamgu->Provideint(it.first);
            it.next();
        }
        return aMINUSONE;
    }
    
    while (!it.end()) {
        if (a->same(it.second) == aTRUE)
            return aTRUE;
        it.next();
    }
    
    return aFALSE;
}

Exporting Tamgu* Tamguring::Pushfirst(Tamgu* a) {
    a = a->Atom();
    a->Addreference(reference+1);
    a = values.push_front(a);
    a->Removereference(reference + 1);
    return aTRUE;
}


Exporting Tamgu* Tamguring::Push(Tamgu* a) {
    a = a->Atom();
    a->Addreference(reference+1);
    a = values.push_back(a);
    a->Removereference(reference + 1);
    return aTRUE;
}

Exporting Tamgu* Tamguring::Push(TamguGlobal* g, Tamgu* a, short idhtread) {
    a = a->Atom();
    a->Addreference(reference+1);
    a = values.push_back(a);
    a->Removereference(reference + 1);
    return aTRUE;
}

Exporting Tamgu* Tamguring::Pop(Tamgu* idx) {
    BLONG v = idx->Integer();
    
    if (v == -1) {
        idx = values.remove_back();
    }
    else {
        if (v < -1)
            return aFALSE;
        idx = values.remove(v);
    }
    
    if (idx->isNULL())
        return aFALSE;
    
    idx->Removereference(reference + 1);
    return aTRUE;
}



Exporting string Tamguring::String() {
    if (loopmark)
        return("[...]");
    TamguCircular _c(this);
    string res;
    res = "[";
    bool beg = true;
    string sx;
    Tamgu* element;
    atomic_ring_iterator<Tamgu*> it(values);
    while (!it.end()) {
        element = it.second;
        sx = element->StringToDisplay();
        if (!element->isString() || element->isContainer()) {
            if (sx == "")
                sx = "''";
            if (beg == false) {
                if (sx[0] != '|')
                    res += ",";
            }
            res += sx;
        }
        else {
            if (beg == false)
                res += ",";
            stringing(res, sx);
        }
        beg = false;
        it.next();
    }
    res += "]";
    return res;
}

Exporting Tamgu* Tamguring::Map(short idthread) {
    if (loopmark)
        return aNULL;
    
    loopmark=true;
    Tamgumap* kmap = globalTamgu->Providemap();
    char ch[20];
    atomic_ring_iterator<Tamgu*> it(values);
    while (!it.end()) {
        sprintf_s(ch, 20, "%d", it.first);
        kmap->Push(ch, it.second);
        it.next();
    }
    loopmark=false;
    return kmap;
}

Exporting string Tamguring::JSonString() {
    if (loopmark)
        return("");
    TamguCircular _c(this);
    string res;
    
    res = "[";
    bool beg = true;
    string sx;
    Tamgu* element;
    atomic_ring_iterator<Tamgu*> it(values);
    while (!it.end()) {
        element = it.second;
        sx = element->JSonString();
        if (!element->isString() || element->isContainer()) {
            if (beg == false) {
                if (sx[0] != '|')
                    res += ",";
            }
        }
        else {
            if (beg == false)
                res += ",";
        }
        res += sx;
        beg = false;
        it.next();
    }
    res += "]";
    return res;
}

Exporting long Tamguring::Integer() {
    return values.size();
}

Exporting double Tamguring::Float() {
    return values.size();
}

Exporting BLONG Tamguring::Long() {
    return values.size();
}

Exporting bool Tamguring::Boolean() {
    if (values.size())
        return true;
    return false;
}

    //Basic operations
Exporting long Tamguring::Size() {
    return values.size();
}

Exporting Tamgu* Tamguring::andset(Tamgu *b, bool itself) {
    Tamguring* ref;
    
    if (!b->isContainer()) {
        if (itself)
            ref = this;
        else
            ref = (Tamguring*)Atom(true);
        
        atomic_ring_iterator<Tamgu*> it(ref->values);
        while (!it.end()) {
            it.second->andset(b, true);
            it.next();
        }
        return ref;
    }
    
    ref = new Tamguring();
    TamguIteration* itr = b->Newiteration(false);
    atomic_ring_iterator<Tamgu*> it;
    Tamgu* ke;
    
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        it.begin(values);
        while (!it.end()) {
            ke = itr->IteratorValue();
            if (it.second->same(ke) == aTRUE) {
                ref->Push(ke);
                break;
            }
            it.next();
        }
    }
    itr->Release();
    
    return ref;
    
}

Exporting Tamgu* Tamguring::orset(Tamgu *b, bool itself) {
    Tamguring* ref;
    
    Tamgu* ke;
    if (itself)
        ref = this;
    else
        ref = (Tamguring*)Atom(true);
    
    if (!b->isContainer()) {
        
        atomic_ring_iterator<Tamgu*> it(ref->values);
        while (!it.end()) {
            ke = it.second->orset(b, true);
            it.next();
        }
        return ref;
    }
    
    ref->Merging(b);
    return ref;
}

Exporting Tamgu* Tamguring::xorset(Tamgu *b, bool itself) {
    Tamguring* ref;
    
    if (!b->isContainer()) {
        if (itself)
            ref = this;
        else
            ref = (Tamguring*)Atom(true);
        
        atomic_ring_iterator<Tamgu*> it(ref->values);
        while (!it.end()) {
            it.second->xorset(b, true);
            it.next();
        }
        return ref;
    }
    
    ref = new Tamguring();
    bool found=false;
    vector<Tamgu*> vals;
    Tamgu* ke;
    
    TamguIteration* itr = b->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next())
        vals.push_back(itr->Value());
    itr->Release();
    
    long itv;
    atomic_ring_iterator<Tamgu*> it(values);
    
    while (!it.end()) {
        ke = it.second;
        found = false;
        for (itv = 0; itv < vals.size(); itv++)  {
            if (vals[itv] == NULL)
                continue;
            
            if (vals[itv]->same(ke) == aTRUE) {
                vals[itv]->Release();
                vals[itv] = NULL;
                found = true;
                break;
            }
        }
        if (!found)
            ref->Push(ke);
        it.next();
    }
    
    for (itv = 0; itv < vals.size(); itv++) {
        if (vals[itv] == NULL)
            continue;
        ref->Push(vals[itv]);
        vals[itv]->Release();
    }
    
    return ref;
}


Exporting Tamgu* Tamguring::plus(Tamgu* b, bool itself) {
    Tamguring* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamguring*)Atom(true);
    
    Tamgu* kv;
    if (loopmark)
        return aNULL;
    
    TamguCircular _c(this);
    
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_ring_iterator<Tamgu*> it(ref->values);
        while (!it.end()) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            it.second->plus(kv, true);
            itr->Next();
            it.next();
        }
        itr->Release();
        return ref;
    }
    
    
    atomic_ring_iterator<Tamgu*> it(ref->values);
    while (!it.end()) {
        it.second->plus(b, true);
        it.next();
    }
    
    return ref;
}

Exporting Tamgu* Tamguring::minus(Tamgu *b, bool itself) {
    Tamguring* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamguring*)Atom(true);
    
    Tamgu* kv;
    if (loopmark)
        return aNULL;
    
    TamguCircular _c(this);
    
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_ring_iterator<Tamgu*> it(ref->values);
        while (!it.end()) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            it.second->minus(kv, true);
            itr->Next();
            it.next();
        }
        itr->Release();
        return ref;
    }
    
    
    atomic_ring_iterator<Tamgu*> it(ref->values);
    while (!it.end()) {
        it.second->minus(b, true);
        it.next();
    }
    
    return ref;
    
}

Exporting Tamgu* Tamguring::multiply(Tamgu *b, bool itself) {
    Tamguring* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamguring*)Atom(true);
    
    Tamgu* kv;
    if (loopmark)
        return aNULL;
    
    TamguCircular _c(this);
    
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_ring_iterator<Tamgu*> it(ref->values);
        while (!it.end()) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            it.second->multiply(kv, true);
            itr->Next();
            it.next();
        }
        itr->Release();
        return ref;
    }
    
    
    atomic_ring_iterator<Tamgu*> it(ref->values);
    while (!it.end()) {
        it.second->multiply(b, true);
        it.next();
    }
    
    return ref;
}

Exporting Tamgu* Tamguring::divide(Tamgu *b, bool itself) {
    Tamguring* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamguring*)Atom(true);
    
    Tamgu* ke;
    Tamgu* kv;
    if (loopmark)
        return aNULL;
    
    TamguCircular _c(this);
    
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_ring_iterator<Tamgu*> it(ref->values);
        while (!it.end()) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = it.second->divide(kv, true);
            if (ke->isError()) {
                itr->Release();
                ref->Release();
                return ke;
            }
            itr->Next();
            it.next();
        }
        itr->Release();
        return ref;
    }
    
    
    atomic_ring_iterator<Tamgu*> it(ref->values);
    while (!it.end()) {
        ke = it.second->divide(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
        it.next();
    }
    
    return ref;
}

Exporting Tamgu* Tamguring::power(Tamgu *b, bool itself) {
    Tamguring* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamguring*)Atom(true);
    
    Tamgu* kv;
    if (loopmark)
        return aNULL;
    
    TamguCircular _c(this);
    
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_ring_iterator<Tamgu*> it(ref->values);
        while (!it.end()) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            it.second->power(kv, true);
            itr->Next();
            it.next();
        }
        itr->Release();
        return ref;
    }
    
    
    atomic_ring_iterator<Tamgu*> it(ref->values);
    while (!it.end()) {
        it.second->power(b, true);
        it.next();
    }
    
    return ref;
}

Exporting Tamgu* Tamguring::shiftleft(Tamgu *b, bool itself) {
    Tamguring* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamguring*)Atom(true);
    
    Tamgu* kv;
    if (loopmark)
        return aNULL;
    
    TamguCircular _c(this);
    
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_ring_iterator<Tamgu*> it(ref->values);
        while (!it.end()) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            it.second->shiftleft(kv, true);
            itr->Next();
            it.next();
        }
        itr->Release();
        return ref;
    }
    
    
    atomic_ring_iterator<Tamgu*> it(ref->values);
    while (!it.end()) {
        it.second->shiftright(b, true);
        it.next();
    }
    
    return ref;
}

Exporting Tamgu* Tamguring::shiftright(Tamgu *b, bool itself) {
    Tamguring* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamguring*)Atom(true);
    
    Tamgu* kv;
    if (loopmark)
        return aNULL;
    
    TamguCircular _c(this);
    
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_ring_iterator<Tamgu*> it(ref->values);
        while (!it.end()) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            it.second->shiftright(kv, true);
            itr->Next();
            it.next();
        }
        itr->Release();
        return ref;
    }
    
    
    atomic_ring_iterator<Tamgu*> it(ref->values);
    while (!it.end()) {
        it.second->shiftright(b, true);
        it.next();
    }
    
    return ref;
}

Exporting Tamgu* Tamguring::mod(Tamgu *b, bool itself) {
    Tamguring* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamguring*)Atom(true);
    
    Tamgu* ke;
    Tamgu* kv;
    if (loopmark)
        return aNULL;
    
    TamguCircular _c(this);
    
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_ring_iterator<Tamgu*> it(ref->values);
        while (!it.end()) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = it.second->mod(kv, true);
            if (ke->isError()) {
                itr->Release();
                ref->Release();
                return ke;
            }
            itr->Next();
            it.next();
        }
        itr->Release();
        return ref;
    }
    
    
    atomic_ring_iterator<Tamgu*> it(ref->values);
    while (!it.end()) {
        ke = it.second->mod(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
        it.next();
    }
    
    return ref;
}


Exporting Tamgu* Tamguring::Unique() {
    Tamguvector* kvect = globalTamgu->Providevector();
    map<string, Tamgu*> inter;
    string k;
    atomic_ring_iterator<Tamgu*> it(values);
    while (!it.end()) {
        k = it.second->String();
        if (inter.find(k) != inter.end()) {
            if (inter[k]->same(it.second)->Boolean() == false)
                kvect->Push(it.second);
        }
        else {
            inter[k] = it.second;
            kvect->Push(it.second);
        }
        it.next();
    }
    return kvect;
}


Exporting Tamgu*  Tamguring::Put(Tamgu* idx, Tamgu* value, short idthread) {
    if (!idx->isIndex()) {
        if (value == this)
            return aTRUE;
        
        if (value == aNULL) {
            Clear();
            return aTRUE;
        }
        
        if (value->Type() == Tamguring::idtype) {
            //this is one of the rare cases when we need a lock...
            values._lock.lock();
            Clear();
            values = ((Tamguring*)value)->values;
            atomic_ring_iterator<Tamgu*> it(values);
            for (;!it.end();it.next()) {
                if (!it.second->isConst()) {
                    idx = it.second->Atom();
                    idx->Addreference(reference+1);
                    value = it.replace(idx);
                    if (value != idx)
                        value->Removereference(reference+1);
                }
            }
            values._lock.unlock();
            return aTRUE;
        }
        
        if (value->Type() == a_list) {
            Locking* _lock = _getlockif(value);
            
            Tamgulist* kvect = (Tamgulist*)value;
                //We copy all values from value to this
            Clear();
            {
                Locking _lock(kvect);
                
                for (auto& it : kvect->values)
                    Push(it);
            }
            
            _cleanlockif(_lock);
            return aTRUE;
        }
        
        long sz = value->Size();
        if (value->isVectorContainer()) {
            Locking* _lock = _getlockif(value);
            
            
                //We copy all values from value to this
            Clear();
            if (sz) {
                Tamgu* val;
                for (int it = 0; it < sz; it++) {
                    idx  = value->getvalue(it);
                    idx = idx->Atom();
                    idx->Addreference(reference+1);
                    val = values.push_back(idx);
                    if (!val->isConst())
                        val->Removereference(reference + 1);
                }
            }
            
            _cleanlockif(_lock);
            return aTRUE;
        }
            //We gather all the keys from the MAP
        if (value->isMapContainer()) {
            Clear();
            if (sz) {
                Tamgu* val;
                TamguIteration* itr = value->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
                    idx  = itr->Key();
                    idx = idx->Atom();
                    idx->Addreference(reference+1);
                    val = values.push_back(idx);
                    if (!val->isConst())
                        val->Removereference(reference + 1);
                }
                itr->Release();
            }
            return aTRUE;
        }
        
        value = value->Vector(idthread);
        if (!value->isVectorContainer())
            return globalTamgu->Returnerror("Cannot set this value", idthread);
        
        sz = value->Size();
        
        Clear();
        if (sz) {
            Tamgu* val;
            //We copy all values from ke to this
            for (long it = 0; it < sz; ++it) {
                idx = ((Tamguvector*)value)->values[it];
                idx = idx->Atom();
                idx->Addreference(reference+1);
                val = values.push_back(idx);
                if (!val->isConst())
                    val->Removereference(reference + 1);
            }
        }
        value->Releasenonconst();
        return aTRUE;
    }
    
    long ikey = idx->Integer();
    long mx = values.size();
    if (ikey < 0) {
        ikey = mx + ikey;
        if (ikey < 0)
            return globalTamgu->Returnerror("Cannot set this value", idthread);
    }
    value = value->Atom();
    value->Addreference(reference+1);
    value = values.replace(ikey, value);
    value->Removereference(reference + 1);
    return aTRUE;
}


Exporting Tamgu* Tamguring::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    
    if (!idx->isIndex()) {
        if (contextualpattern->isLoop())
            return this;
        
            //In this case, we copy the elements from the vector to the map, using the position as index
        if (contextualpattern->isMapContainer()) {
            Tamgu* ke;
            Tamgu* map = Selectamap(contextualpattern);
            char ch[20];
            atomic_ring_iterator<Tamgu*> it(values);
            while (!it.end()) {
                sprintf_s(ch, 20, "%d", it.first);
                ke = it.second;
                map->Push(ch, ke);
                it.next();
            }
            return map;
        }
        
        if (contextualpattern->isNumber()) {
            long v = Size();
            return globalTamgu->Provideint(v);
        }
        return this;
    }
    
    Tamgu* key;
    Tamgu* keyright = NULL;
    
    TamguIndex* kind = (TamguIndex*)idx;
    key = kind->left->Eval(aNULL, aNULL, idthread);
    if (kind->interval == true)
        keyright = kind->right->Eval(aNULL, aNULL, idthread);
    
    long mx = values.size();
    long ikey = key->Integer();
    key->Release();
    if (ikey < 0)
        ikey = mx + ikey;
    
    if (ikey >= 0)
        key = values[ikey];
    
    if (ikey < 0 || key == NULL) {
        if (ikey != mx || keyright == NULL) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    
    if (keyright == NULL)
        return key;
    
    Tamguvector* kvect;
    long iright;
    
    if (keyright == aNULL)
        iright = 0;
    else
        iright = keyright->Integer();
    
    keyright->Release();
    
    if (iright < 0 || keyright == aNULL) {
        iright = mx + iright;
        if (iright<ikey) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    else {
        if (iright>mx) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    
        //In this case, we must create a new vector
    kvect = globalTamgu->Providevector();
    
    atomic_ring_iterator<Tamgu*> it(values, ikey);
    while (!it.end() && it.first < iright) {
        kvect->Push(it.second);
        it.next();
    }
    
    return kvect;
}


Exporting void Tamguring::Clear() {
    atomic_ring_iterator<Tamgu*> it(values);
    while (!it.end()) {
        it.second->Removereference(reference + 1);
        it.next();
    }
    values.clear();
}


Exporting void Tamguring::Storevalue(string& u) {
    Tamgu* a = globalTamgu->Providestring(u);
    a->Addreference(reference+1);
    a = values.push_back(a);
    if (a != NULL)
        a->Removereference();
}

Exporting void Tamguring::Storevalue(wstring& u) {
    Tamgu* a = globalTamgu->Provideustring(u);
    a->Addreference(reference+1);
    a = values.push_back(a);
    if (a != NULL)
        a->Removereference();
}

Exporting void Tamguring::storevalue(string u) {
    Tamgu* a = globalTamgu->Providestring(u);
    a->Addreference(reference+1);
    a = values.push_back(a);
    if (a != NULL)
        a->Removereference();
}

Exporting void Tamguring::storevalue(wstring u) {
    Tamgu* a = globalTamgu->Provideustring(u);
    a->Addreference(reference+1);
    a = values.push_back(a);
    if (a != NULL)
        a->Removereference();
}

Exporting void Tamguring::storevalue(long u) {
    Tamgu* a = globalTamgu->Provideint(u);
    a->Addreference(reference+1);
    a = values.push_back(a);
    if (a != NULL)
        a->Removereference();
}

Exporting void Tamguring::storevalue(short u) {
    Tamgu* a = new Tamgushort(u);
    a->Addreference(reference+1);
    a = values.push_back(a);
    if (a != NULL)
        a->Removereference();
}

Exporting void Tamguring::storevalue(BLONG u) {
    Tamgu* a = new Tamgulong(u);
    a->Addreference(reference+1);
    a = values.push_back(a);
    if (a != NULL)
        a->Removereference();
}

Exporting void Tamguring::storevalue(float u) {
    Tamgu* a = new Tamgudecimal(u);
    a->Addreference(reference+1);
    a = values.push_back(a);
    if (a != NULL)
        a->Removereference();
}

Exporting void Tamguring::storevalue(double u) {
    Tamgu* a = globalTamgu->Providefloat(u);
    a->Addreference(reference+1);
    a = values.push_back(a);
    if (a != NULL)
        a->Removereference();
}

Exporting void Tamguring::storevalue(unsigned char u) {
    Tamgu* a = new Tamgubyte(u);
    a->Addreference(reference+1);
    a = values.push_back(a);
    if (a != NULL)
        a->Removereference();
}

Exporting void Tamguring::storevalue(wchar_t u) {
    wstring w;
    w = u;
    Tamgu* a = globalTamgu->Provideustring(w);
    a->Addreference(reference+1);
    a = values.push_back(a);
    if (a != NULL)
        a->Removereference();
}


Exporting Tamgu* Tamguring::Thesum(long i, long j) {
    long sz = values.size();
    if (!sz)
        return aNULL;
    
    if (i < 0)
        i += sz;
    if (j < 0)
        j += sz;
    
    if (i < 0)
        i = 0;
    else
        if (i >= sz)
            i = sz - 1;
    
    if (j <= i)
        j = i + 1;
    
    if (j >= sz)
        j = sz - 1;
    
    if (values[0]->isContainer()) {
        Tamgufvector* sub = globalTamgu->Providefvector();
        Tamgu* res;
        atomic_ring_iterator<Tamgu*> it(values, i);
        while (!it.end() && it.first <= j) {
            res = it.second->Thesum(0, (long)it.second->Size());
            sub->values.push_back(res->Float());
            res->Release();
            it.next();
        }
        return sub;
    }
    
    if (values[0]->isNumber()) {
        double v = 0;
        atomic_ring_iterator<Tamgu*> it(values, i);
        while (!it.end() && it.first <= j) {
            v += it.second->Float();
            it.next();
        }
        
        return globalTamgu->Providefloat(v);
    }
    
    string v;
    atomic_ring_iterator<Tamgu*> it(values, i);
    while (!it.end() && it.first < j) {
        v += it.second->String();
        it.next();
    }
    return globalTamgu->Providestring(v);
}

Exporting Tamgu* Tamguring::Theproduct(long i, long j) {
    long sz = values.size();
    if (!sz)
        return aNULL;
    
    if (i < 0)
        i += sz;
    if (j < 0)
        j += sz;
    
    if (i < 0)
        i = 0;
    else
        if (i >= sz)
            i = sz - 1;
    
    if (j <= i)
        j = i + 1;
    
    if (j >= sz)
        j = sz - 1;
    
    if (values[0]->isContainer()) {
        Tamgufvector* sub = globalTamgu->Providefvector();
        Tamgu* res;
        atomic_ring_iterator<Tamgu*> it(values, i);
        while (!it.end() && it.first <= j) {
            res = it.second->Theproduct(0, (long)it.second->Size());
            sub->values.push_back(res->Float());
            res->Release();
            it.next();
        }
        return sub;
    }
    
    double v = 1;
    atomic_ring_iterator<Tamgu*> it(values, i);
    while (!it.end() && it.first <= j) {
        v *= it.second->Float();
        it.next();
    }
    
    return globalTamgu->Providefloat(v);
}

Exporting Tamgu* Tamguring::same(Tamgu* a) {
    if (a->Type() != idtype) {
        if (a->isVectorContainer()) {
            if (a->Size() != values.size())
                return aFALSE;
            Tamgu* v;
            atomic_ring_iterator<Tamgu*> it(values);
            for (; !it.end(); it.next()) {
                v = a->getvalue(it.first);
                if (!v->same(it.second)->Boolean()) {
                    v->Release();
                    return aFALSE;
                }
                v->Release();
            }
            return aTRUE;
        }
        return aFALSE;
    }
    
    Tamguvector* v = (Tamguvector*)a;
    Locking _lock(v);
    
    if (values.size() != v->values.size())
        return aFALSE;
    
    atomic_ring_iterator<Tamgu*> it(values);
    for (; !it.end(); it.next()) {
        if (it.second->same(v->values[it.first]) == aFALSE)
            return aFALSE;
    }
    return aTRUE;
}

Exporting Tamgu* Tamguring::Merging(Tamgu* ke) {
    if (!ke->isContainer()) {
        Push(ke);
        return this;
    }
    
    if (ke->Size() == 0)
        return this;
    
    
    if (ke->isVectorValueContainer()) {
        Locking* _lock = _getlockif(ke);
        Tamgu* k;
        for (long i = 0; i < ke->Size(); i++) {
            k = ke->getvalue(i);
            k->Addreference(reference+1);
            values.push_back(k);
        }
        _cleanlockif(_lock);
        return this;
    }
    
    TamguIteration* itr = ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next())
        Push(itr->Value());
    itr->Release();
    return this;
}

Exporting Tamgu* Tamguring::Combine(Tamgu* ke) {
    if (ke->Size() == 0)
        return this;
    
    Tamguvector* vect = globalTamgu->Providevector();
    Tamgu* val;
    
    if (!ke->isContainer()) {
        atomic_ring_iterator<Tamgu*> it(values);
        for (; !it.end(); it.next()) {
            val=new Tamguvector;
            val->Push(it.second);
            val->Push(ke);
            vect->Push(val);
        }
        return vect;
    }
    
    
    atomic_ring_iterator<Tamgu*> it(values);
    TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        if (it.end()) {
            val=new Tamguvector;
            val->Push(itr->IteratorValue());
            vect->Push(val);
            continue;
        }
        
        val=new Tamguvector;
        val->Push(it.second);
        val->Push(itr->IteratorValue());
        vect->Push(val);
        it.next();
    }
    itr->Release();
    
    for (;!it.end();it.next()) {
        val=new Tamguvector;
        val->Push(it.second);
        vect->Push(val);
    }
    
    return vect;
}
