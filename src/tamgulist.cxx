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
#include "tamgutaskell.h"
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

Exporting short Tamgulist::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgulist::AddMethod(TamguGlobal* global, string name, listMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}




    void Tamgulist::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamgulist::InitialisationModule(global,"");
}


   bool Tamgulist::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    

    Tamgulist::idtype = a_list;

       Tamgulist::AddMethod(global, "min", &Tamgulist::MethodMin, P_NONE, "min(): returns the min in the vector.");
       Tamgulist::AddMethod(global, "max", &Tamgulist::MethodMax, P_NONE, "max(): returns the max in the vector.");
       Tamgulist::AddMethod(global, "minmax", &Tamgulist::MethodMinMax, P_NONE, "minmax(): returns the min and the max in the vector.");


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
        global->RecordCompatibilities(Tamgulist::idtype);
    }

    Tamguring::InitialisationModule(global, version);
    Tamgujava_vector::InitialisationModule(global, version);
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
        locking();
        for (const auto& it : values) {
            if (it->same(a) == aTRUE) {
                unlocking();
                return aTRUE;
            }
        }
        unlocking();
        return aFALSE;
    }

    long i = 0;
    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Doublelocking _lock(this, v);
        for (const auto& it : values) {
            if (it->same(a) == aTRUE)
                v->values.push_back(i);
            i++;
        }
        return v;
    }

    locking();
    for (const auto& it : values) {
        if (it->same(a) == aTRUE) {
            unlocking();
            return globalTamgu->ProvideConstint(i);
        }
        i++;
    }
    unlocking();
    return aNOELEMENT;
}


Exporting Tamgu* Tamgulist::getvalue(long i) {
    locking();
    if (i < 0 || i >= values.size()) {
        unlocking();
        return aNOELEMENT;
    }


    for (const auto& it : values) {
        if (!i) {
            unlocking();
            return (it);
        }
        i--;
    }
    unlocking();
    return aNOELEMENT;
}


Exporting void Tamgulist::Cleanreference(short inc) {
    locking();
    for (const auto& a : values)
        a->Removecontainerreference(inc);
    unlocking();
}

Exporting void Tamgulist::Setreference(short inc) {
    locking();

    reference += inc;
    protect=false;
        
    for (const auto& it : values)
        it->Addreference(investigate,inc);
    
    unlocking();
}

Exporting void Tamgulist::Setreference() {
    locking();

    ++reference;
    protect=false;
    
    locking();
    for (const auto& it : values)
        it->Addreference(investigate,1);

    unlocking();
}


static void resetList(Tamgulist* klist, short inc) {
    klist->reference -= inc;

    klist->locking();
    list<Tamgu*>& values = klist->values;
    if (values.size() == 0) {
        klist->unlocking();
        return;
    }
    
    for (auto& itx : values)
        itx->Removereference(inc);

    klist->unlocking();
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
    locking();
    a = a->Atom();
    values.push_front(a);
    a->Addreference(investigate,reference+1);
    unlocking();
    return this;
}

Exporting Tamgu* Tamgulist::Push(Tamgu* a) {
    locking();
    a = a->Atom();
    values.push_back(a);
    unlocking();
    a->Addreference(investigate,reference+1);
    return this;
}

Exporting Tamgu* Tamgulist::Pop(Tamgu* v) {
    locking();
    if (!values.size()) {
        unlocking();
        return aFALSE;
    }

    list<Tamgu*>::iterator it;
    for (it = values.begin(); it != values.end(); ++it) {
        if ((*it)->same(v)) {
            v = *it;
            values.erase(it);
            unlocking();
            v->Removereference(reference + 1);
            return aTRUE;
        }
    }
    unlocking();
    return aFALSE;
}

Exporting void Tamgulist::Clear() {
    //To set a variable back to empty
    locking();

    for (auto& it : values) {
        it->Removereference(reference + 1);
    }
    values.clear();
    unlocking();
}

Exporting string Tamgulist::String() {
    if (!lockingmark())
        return("[...]");

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

    unlockingmark();
    res += "]";
    return res;
}

Exporting void Tamgulist::Setstring(string& res, short idthread) {
    if (!lockingmark()) {
        res = "[...]";
        return;
    }
    

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

    unlockingmark();
    res += "]";
}

Exporting string Tamgulist::JSonString() {
    if (!lockingmark())
        return("");

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

    unlockingmark();
    res += "]";
    return res;
}



Exporting long Tamgulist::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting double Tamgulist::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting BLONG Tamgulist::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting bool Tamgulist::Boolean() {
    locking();
    bool b = values.empty();
    unlocking();
    return !b;
}


//Basic operations
Exporting long Tamgulist::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting void Tamgulist::Insert(long idx, Tamgu* ke) {
    ke = ke->Atom();
    if (idx<0)
        idx=0;
    locking();
    if (idx>=values.size())
        values.push_back(ke);
    else {
        list<Tamgu*>::iterator itl = values.begin();
        for (long nb = 0; nb < idx; nb++)
            itl++;
        values.insert(itl, ke);
    }
    ke->Addreference(investigate,reference+1);
    unlocking();
}

Exporting bool Tamgulist::Permute() {
    locking();
    bool v  = next_permutation(values.begin(), values.end());
    unlocking();
    return v;
}

Exporting void Tamgulist::Shuffle() {
    locking();

    vector<Tamgu*> vb;
    for (const auto& it : values)
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
    unlocking();
}

Exporting Tamgu* Tamgulist::Unique() {
    locking();
    Tamgulist* klist = new Tamgulist;

    map<string, Tamgu*> inter;
    string k;
    for (const auto& it : values) {
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
    unlocking();
    return klist;
}

Exporting Tamgu* Tamgulist::Popfirst() {
    locking();
    if (values.size() == 0) {
        unlocking();
        return aNOELEMENT;
    }
    Tamgu* kres = values.front();
    values.pop_front();
    unlocking();
    kres->Resetreference(reference);
    kres->Protect();
    return kres;
}

Exporting Tamgu* Tamgulist::Poplast() {
    locking();
    if (values.size() == 0) {
        unlocking();
        return aNOELEMENT;
    }
    Tamgu* kres = values.back();
    values.pop_back();
    unlocking();
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

        for (const auto& it : klist->values)
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
        locking();
        for (const auto& a: values) {
            val=new Tamgulist;
            val->Push(a);
            val->Push(ke);
            vect->Push(val);
        }
        unlocking();
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
    if (!lockingmark())
        return aNULL;

    Tamgumap* kmap = globalTamgu->Providemap();

    long nb = 0;
    char ch[20];
    for (const auto& it : values) {
        sprintf_s(ch, 20, "%ld", nb);
        kmap->Push(ch, it);
        nb++;
    }
    
    unlockingmark();
    return kmap;
}

Exporting Tamgu* Tamgulist::Vector(short idthread) {
    locking();
    Tamguvector* kvect = globalTamgu->Providevector();
    //We copy all values from ke to this
    kvect->values.reserve(values.size());

    for (const auto& it : values)
        kvect->Push(it);
    unlocking();
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

            for (const auto& it : kvect->values) {
                idx = it->Atom();
                idx->Addreference(investigate,reference+1);
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
                idx->Addreference(investigate,reference+1);
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
                idx->Addreference(investigate,reference+1);
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
            idx->Addreference(investigate,reference+1);
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

        long rkey = ((TamguIndex*)idx)->right->Getinteger(idthread);
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
                krkey->Addreference(investigate,reference+1);
            }
            return aTRUE;
        }

        if (ke->Type() == a_list) {
            for (const auto& iti : ((Tamgulist*)ke)->values) {
                krkey = iti;
                krkey = krkey->Atom();
                values.insert(it, krkey);
                krkey->Addreference(investigate,reference+1);
            }
            return aTRUE;
        }

        if (ke->isContainer()) {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
                krkey = itr->Value();
                krkey = krkey->Atom();
                values.insert(it, krkey);
                krkey->Addreference(investigate,reference+1);
            }
            itr->Release();
            return aTRUE;
        }

        krkey = ke->Atom();
        values.insert(it, krkey);
        krkey->Addreference(investigate,reference+1);
        return aTRUE;
    }

    long ikey = idx->Getinteger(idthread);
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
            ke->Addreference(investigate,reference+1);
        }
    }

    return aTRUE;
}

Exporting Tamgu* Tamgulist::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {

    if (!idx->isIndex()) {

        Tamgu* ke;
        //In this case, we copy the elements from the vector to the map, using the position as index
        if (contextualpattern->isMapContainer()) {
            Tamgumap* map = (Tamgumap*)Selectamap(contextualpattern);
            size_t i = 0;

            char ch[20];
            locking();
            for (const auto& it : values) {
                sprintf_s(ch, 20, "%zd", i);
                ke = it;
                map->Push(ch, ke);
                i++;
            }
            unlocking();
            return map;
        }

        if (contextualpattern->isNumber()) {
            long v = Size();
            return globalTamgu->ProvideConstint(v);
        }

        //Then some values must be replaced with their evaluation
        if (evaluate == true) {
            Tamgulist* kvect = new Tamgulist;

            locking();
            for (const auto& it : values) {
                ke = it->Eval(aNULL, aNULL, idthread);
                if (ke == aRAISEERROR) {
                    kvect->Release();
                    return aRAISEERROR;
                }
                kvect->Push(ke);
            }
            unlocking();
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
    
    locking();
	if (ikey < 0 || ikey >= values.size()) {
		unlocking();
		if (globalTamgu->erroronkey)
			return globalTamgu->Returnerror("Wrong index", idthread);
		return aNOELEMENT;
	}

	it = values.begin();
    for (i = 0; i < ikey; i++) ++it;


    if (keyright == NULL) {
        keyright = *it;
        unlocking();
        return keyright;
    }

    long iright = keyright->Integer();
    if (keyright != ((TamguIndex*)idx)->right)
        keyright->Release();

    if (iright < 0 || keyright == aNULL) {
        iright = values.size() + iright;
        if (iright<ikey) {
            unlocking();
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    else {
        if (iright>values.size()) {
            unlocking();
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    //In this case, we must create a new list
    kvect = new Tamgulist;
    for (; i < iright; i++, ++it)
        kvect->Push(*it);

    unlocking();
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
        locking();
        for (const auto& itl : ref->values) {
            ke = itl->andset(b, true);
            if (ke->isError()) {
                unlocking();
                ref->Release();
                return ke;
            }
        }
        unlocking();
        return ref;
    }

    Doublelocking _lock(this, b);

    ref = new Tamgulist;

    TamguIteration* itr = b->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        for (const auto& itl : ref->values) {
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
        locking();
        for (const auto& itl : ref->values) {
            ke = itl->orset(b, true);
            if (ke->isError()) {
                unlocking();
                ref->Release();
                return ke;
            }
        }
        unlocking();
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
            
        locking();
        
        Tamgu* ke;
        for (const auto& itl : ref->values) {
            ke = itl->xorset(b, true);
            if (ke->isError()) {
                unlocking();
                ref->Release();
                return ke;
            }
        }
        unlocking();
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
    for (const auto& itl : ref->values) {
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
        for (const auto& itl : ref->values) {
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


    for (const auto& itl : ref->values) {
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
        for (const auto& itl : ref->values) {
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


    for (const auto& itl : ref->values) {
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
        for (const auto& itl : ref->values) {
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


    for (const auto& itl : ref->values) {
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
        for (const auto& itl : ref->values) {
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


    for (const auto& itl : ref->values) {
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
        for (const auto& itl : ref->values) {
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


    for (const auto& itl : ref->values) {
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
        for (const auto& itl : ref->values) {
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


    for (const auto& itl : ref->values) {
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
        for (const auto& itl : ref->values) {
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


    for (const auto& itl : ref->values) {
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
        for (const auto& itl : ref->values) {
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


    for (const auto& itl : ref->values) {
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
    Tamgu* a = globalTamgu->Providewithstring(u);
    a->Addreference(investigate,reference+1);
    locking();
    values.push_back(a);
    unlocking();
}

Exporting void Tamgulist::Storevalue(wstring& u) {
    Tamgu* a = globalTamgu->Providewithustring(u);
    a->Addreference(investigate,reference+1);
    locking();
    values.push_back(a);
    unlocking();
}

Exporting void Tamgulist::storevalue(string u) {
    Tamgu* a = globalTamgu->Providewithstring(u);
    a->Addreference(investigate,reference+1);
    locking();
    values.push_back(a);
    unlocking();
}

Exporting void Tamgulist::storevalue(wstring u) {
    Tamgu* a = globalTamgu->Providewithustring(u);
    a->Addreference(investigate,reference+1);
    locking();
    values.push_back(a);
    unlocking();
}

Exporting void Tamgulist::storevalue(long u) {
    Tamgu* a = globalTamgu->ProvideConstint(u);
    locking();
    values.push_back(a);
    unlocking();
}

Exporting void Tamgulist::storevalue(short u) {
    Tamgu* a = new Tamgushort(u);
    a->Addreference(investigate,reference+1);
    locking();
    values.push_back(a);
    unlocking();
}

Exporting void Tamgulist::storevalue(BLONG u) {
    Tamgu* a = globalTamgu->Providelong(u);
    a->Addreference(investigate,reference+1);
    locking();
    values.push_back(a);
    unlocking();
}

Exporting void Tamgulist::storevalue(float u) {
    Tamgu* a = new Tamgudecimal(u);
    a->Addreference(investigate,reference+1);
    locking();
    values.push_back(a);
    unlocking();
}

Exporting void Tamgulist::storevalue(double u) {
    Tamgu* a = globalTamgu->Providefloat(u);
    a->Addreference(investigate,reference+1);
    locking();
    values.push_back(a);
    unlocking();
}

Exporting void Tamgulist::storevalue(unsigned char u) {
    Tamgu* a = new Tamgubyte(u);
    a->Addreference(investigate,reference+1);
    locking();
    values.push_back(a);
    unlocking();
}

Exporting void Tamgulist::storevalue(wchar_t u) {
    wstring w;
    w = u;
    Tamgu* a = globalTamgu->Providewithustring(w);
    a->Addreference(investigate,reference+1);
    locking();
    values.push_back(a);
    unlocking();
}


    //----------------------------------------------------------------------------------------------------------

    //We need to declare once again our local definitions.
Exporting basebin_hash<a_listMethod>  Tamguring::methods;

Exporting short Tamguring::idtype = 0;

    //MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguring::AddMethod(TamguGlobal* global, string name, a_listMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


bool Tamguring::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    
    
    
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
        global->RecordCompatibilities(Tamguring::idtype);
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
    reference += inc;
    protect=false;
    
    atomic_ring_iterator<Tamgu*> it(values);
    while (!it.end()) {
        it.second->Addreference(investigate,inc);
        it.next();
    }
}

Exporting void Tamguring::Setreference() {
    ++reference;
    protect=false;
    
    atomic_ring_iterator<Tamgu*> it(values);
    while (!it.end()) {
        it.second->Addreference(investigate,1);
        it.next();
    }
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
                return globalTamgu->ProvideConstint(it.first);
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
    a->Addreference(investigate,reference+1);
    a = values.push_front(a);
    a->Removereference(reference + 1);
    return aTRUE;
}


Exporting Tamgu* Tamguring::Push(Tamgu* a) {
    a = a->Atom();
    a->Addreference(investigate,reference+1);
    a = values.push_back(a);
    a->Removereference(reference + 1);
    return aTRUE;
}

Exporting Tamgu* Tamguring::Push(TamguGlobal* g, Tamgu* a, short idhtread) {
    a = a->Atom();
    a->Addreference(investigate,reference+1);
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
    if (!lockingmark())
        return("[...]");
    
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
            if (sx.empty())
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
    unlockingmark();
    res += "]";
    return res;
}

Exporting void Tamguring::Setstring(string& res, short idthread) {
    if (!lockingmark()) {
        res = "[...]";
        return;
    }

    res = "[";
    bool beg = true;
    string sx;
    Tamgu* element;
    atomic_ring_iterator<Tamgu*> it(values);
    while (!it.end()) {
        element = it.second;
        sx = element->StringToDisplay();
        if (!element->isString() || element->isContainer()) {
            if (sx.empty())
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
    unlockingmark();
    res += "]";
}

Exporting Tamgu* Tamguring::Map(short idthread) {
    if (!lockingmark())
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
    unlockingmark();
    return kmap;
}

Exporting string Tamguring::JSonString() {
    if (!lockingmark())
        return("");

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
    unlockingmark();
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
    if (!lockingmark())
        return aNULL;
        
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
        unlockingmark();
        return ref;
    }
    
    
    atomic_ring_iterator<Tamgu*> it(ref->values);
    while (!it.end()) {
        it.second->plus(b, true);
        it.next();
    }
    unlockingmark();
    return ref;
}

Exporting Tamgu* Tamguring::minus(Tamgu *b, bool itself) {
    Tamguring* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamguring*)Atom(true);
    
    Tamgu* kv;
    if (!lockingmark())
        return aNULL;
        
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
        unlockingmark();
        return ref;
    }
    
    
    atomic_ring_iterator<Tamgu*> it(ref->values);
    while (!it.end()) {
        it.second->minus(b, true);
        it.next();
    }
    unlockingmark();
    return ref;
    
}

Exporting Tamgu* Tamguring::multiply(Tamgu *b, bool itself) {
    Tamguring* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamguring*)Atom(true);
    
    Tamgu* kv;
    if (!lockingmark())
        return aNULL;
        
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
        unlockingmark();
        return ref;
    }
    
    
    atomic_ring_iterator<Tamgu*> it(ref->values);
    while (!it.end()) {
        it.second->multiply(b, true);
        it.next();
    }
    unlockingmark();
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
    if (!lockingmark())
        return aNULL;
        
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
                unlockingmark();
                return ke;
            }
            itr->Next();
            it.next();
        }
        itr->Release();
        unlockingmark();
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
    unlockingmark();
    return ref;
}

Exporting Tamgu* Tamguring::power(Tamgu *b, bool itself) {
    Tamguring* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamguring*)Atom(true);
    
    Tamgu* kv;
    if (!lockingmark())
        return aNULL;
        
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
        unlockingmark();
        return ref;
    }
    
    
    atomic_ring_iterator<Tamgu*> it(ref->values);
    while (!it.end()) {
        it.second->power(b, true);
        it.next();
    }
    unlockingmark();
    return ref;
}

Exporting Tamgu* Tamguring::shiftleft(Tamgu *b, bool itself) {
    Tamguring* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamguring*)Atom(true);
    
    Tamgu* kv;
    if (!lockingmark())
        return aNULL;
    
    
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
        unlockingmark();
        return ref;
    }
    
    
    atomic_ring_iterator<Tamgu*> it(ref->values);
    while (!it.end()) {
        it.second->shiftright(b, true);
        it.next();
    }
    
    unlockingmark();
    return ref;
}

Exporting Tamgu* Tamguring::shiftright(Tamgu *b, bool itself) {
    Tamguring* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamguring*)Atom(true);
    
    Tamgu* kv;
    if (!lockingmark())
        return aNULL;
        
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
        unlockingmark();
        return ref;
    }
    
    
    atomic_ring_iterator<Tamgu*> it(ref->values);
    while (!it.end()) {
        it.second->shiftright(b, true);
        it.next();
    }
    unlockingmark();
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
    if (!lockingmark())
        return aNULL;
        
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
                unlockingmark();
                return ke;
            }
            itr->Next();
            it.next();
        }
        itr->Release();
        unlockingmark();
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
    unlockingmark();
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
        
        if (value->isNULL()) {
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
                    idx->Addreference(investigate,reference+1);
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
                
                for (const auto& it : kvect->values)
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
                    idx->Addreference(investigate,reference+1);
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
                    idx->Addreference(investigate,reference+1);
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
                idx->Addreference(investigate,reference+1);
                val = values.push_back(idx);
                if (!val->isConst())
                    val->Removereference(reference + 1);
            }
        }
        value->Releasenonconst();
        return aTRUE;
    }
    
    long ikey = idx->Getinteger(idthread);
    long mx = values.size();
    if (ikey < 0) {
        ikey = mx + ikey;
        if (ikey < 0)
            return globalTamgu->Returnerror("Cannot set this value", idthread);
    }
    value = value->Atom();
    value->Addreference(investigate,reference+1);
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
            return globalTamgu->ProvideConstint(v);
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
    
    if (keyright != kind->right)
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
    Tamgu* a = globalTamgu->Providewithstring(u);
    a->Addreference(investigate,reference+1);
    a = values.push_back(a);
    if (a != NULL)
        a->Removereference();
}

Exporting void Tamguring::Storevalue(wstring& u) {
    Tamgu* a = globalTamgu->Providewithustring(u);
    a->Addreference(investigate,reference+1);
    a = values.push_back(a);
    if (a != NULL)
        a->Removereference();
}

Exporting void Tamguring::storevalue(string u) {
    Tamgu* a = globalTamgu->Providewithstring(u);
    a->Addreference(investigate,reference+1);
    a = values.push_back(a);
    if (a != NULL)
        a->Removereference();
}

Exporting void Tamguring::storevalue(wstring u) {
    Tamgu* a = globalTamgu->Providewithustring(u);
    a->Addreference(investigate,reference+1);
    a = values.push_back(a);
    if (a != NULL)
        a->Removereference();
}

Exporting void Tamguring::storevalue(long u) {
    Tamgu* a = globalTamgu->ProvideConstint(u);
    a = values.push_back(a);
    if (a != NULL)
        a->Removereference();
}

Exporting void Tamguring::storevalue(short u) {
    Tamgu* a = new Tamgushort(u);
    a->Addreference(investigate,reference+1);
    a = values.push_back(a);
    if (a != NULL)
        a->Removereference();
}

Exporting void Tamguring::storevalue(BLONG u) {
    Tamgu* a = globalTamgu->Providelong(u);
    a->Addreference(investigate,reference+1);
    a = values.push_back(a);
    if (a != NULL)
        a->Removereference();
}

Exporting void Tamguring::storevalue(float u) {
    Tamgu* a = new Tamgudecimal(u);
    a->Addreference(investigate,reference+1);
    a = values.push_back(a);
    if (a != NULL)
        a->Removereference();
}

Exporting void Tamguring::storevalue(double u) {
    Tamgu* a = globalTamgu->Providefloat(u);
    a->Addreference(investigate,reference+1);
    a = values.push_back(a);
    if (a != NULL)
        a->Removereference();
}

Exporting void Tamguring::storevalue(unsigned char u) {
    Tamgu* a = new Tamgubyte(u);
    a->Addreference(investigate,reference+1);
    a = values.push_back(a);
    if (a != NULL)
        a->Removereference();
}

Exporting void Tamguring::storevalue(wchar_t u) {
    wstring w;
    w = u;
    Tamgu* a = globalTamgu->Providewithustring(w);
    a->Addreference(investigate,reference+1);
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
        
        return globalTamgu->ProvideConstfloat(v);
    }
    
    string v;
    atomic_ring_iterator<Tamgu*> it(values, i);
    while (!it.end() && it.first < j) {
        v += it.second->String();
        it.next();
    }
    return globalTamgu->Providewithstring(v);
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
    
    return globalTamgu->ProvideConstfloat(v);
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
            k->Addreference(investigate,reference+1);
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



//We need to declare once again our local definitions.
Exporting basebin_hash<javavectorMethod>  Tamgujava_vector::methods;

Exporting short Tamgujava_vector::idtype = 0;

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgujava_vector::AddMethod(TamguGlobal* global, string name, javavectorMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
        global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
        return;
    
    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


bool Tamgujava_vector::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    Tamgujava_vector::idtype = global->Getid("java_vector");
    
    Tamgujava_vector::AddMethod(global, "min", &Tamgujava_vector::MethodMin, P_NONE, "min(): returns the min in the vector.");
    Tamgujava_vector::AddMethod(global, "max", &Tamgujava_vector::MethodMax, P_NONE, "max(): returns the max in the vector.");
    Tamgujava_vector::AddMethod(global, "minmax", &Tamgujava_vector::MethodMinMax, P_NONE, "minmax(): returns the min and the max in the vector.");
    Tamgujava_vector::AddMethod(global, "shape", &Tamgujava_vector::MethodShape, P_NONE|P_ATLEASTONE, "shape(int s1, int s2...): defines the vector shape.");
    Tamgujava_vector::AddMethod(global, "sum", &Tamgujava_vector::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgujava_vector::AddMethod(global, "product", &Tamgujava_vector::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgujava_vector::AddMethod(global, "push", &Tamgujava_vector::MethodPush, P_ATLEASTONE, "push(v): Push a value into the vector.");
    
    if (version != "") {
        global->newInstance[Tamgujava_vector::idtype] = new Tamgujava_vector(global);
        global->RecordCompatibilities(Tamgujava_vector::idtype);
    }
    
    return true;
}



Tamgu* Tamgujava_vector::MethodPush(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (iter != NULL) {
        Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
        iter->storevalue(iter->Size(), v);
    }    
    return aTRUE;
}

Tamgu* Tamgujava_vector::Newinstance(short idthread, Tamgu* f) {
    return new Tamgujava_vector_instance();
}

Tamgu* Tamgujava_vector::Newvalue(Tamgu* a, short idthread) {
    return this;
}

Exporting Tamgu* Tamgujava_vector::MethodShape(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long nb = callfunc->Size();
    if (!nb) {
        Tamguivector* ivector = globalTamgu->Provideivector();
        for (long i = 0; i < shape.size(); i++) {
            ivector->storevalue(shape[i]);
        }
        return ivector;
    }
    
    shape.clear();
    Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
    if (v->isVectorContainer()) {
        if (nb != 1)
            globalTamgu->Returnerror("Wrong shape definition", idthread);
        nb = v->Size();
        for (long i = 0; i < nb; i++) {
            shape.push_back(v->getinteger(i));
        }
        return aTRUE;
    }
    
    shape.push_back(v->Integer());

    for (long i = 1; i < nb; i++)
        shape.push_back(callfunc->Evaluate(i, contextualpattern, idthread)->Integer());
    return aTRUE;
}

Exporting Tamgu* Tamgujava_vector::in(Tamgu* c, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    long sz = Size();

    if (c->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(c);
        
        for (long i = 0; i < sz; i++) {
            c = getvalue(i);
            if (a->same(c) == aTRUE)
                v->values.push_back(i);
            c->Release();
        }
        return v;
    }
    
    if (c->isNumber()) {
        for (long i = 0; i < sz; i++) {
            c = getvalue(i);
            if (a->same(c) == aTRUE) {
                c->Release();
                return globalTamgu->ProvideConstint(i);
            }
            c->Release();
        }
        return aMINUSONE;
    }
    
    for (long i = 0; i < sz; i++) {
        c = getvalue(i);
        if (a->same(c) == aTRUE) {
            c->Release();
            return aTRUE;
        }
        c->Release();
    }
    
    return aFALSE;
}

Exporting string Tamgujava_vector::String() {
    string res;
    res = "[";
    bool beg = true;
    string sx;
    Tamgu* element;
    long sz = Size();
    for (long i = 0; i < sz; i++) {
        element = getvalue(i);
        sx = element->StringToDisplay();
        if (!element->isString() || element->isContainer()) {
            if (sx.empty())
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
        element->Release();
    }
    res += "]";
    return res;
}


Exporting string Tamgujava_vector::JSonString() {
    string res;
    
    res = "[";
    bool beg = true;
    string sx;
    Tamgu* element;
    long sz = Size();
    for (long i = 0; i < sz; i++) {
        element = getvalue(i);
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
        element->Release();
    }
    res += "]";
    return res;
}


Exporting Tamgu* Tamgujava_vector::andset(Tamgu *b, bool itself) {
    Tamguvector* ref = globalTamgu->Providevector();
    long sz = Size();
    long i;
    
    Tamgu* a;
    if (!b->isContainer()) {
        for (i = 0; i < sz; i++) {
            a = getvalue(i);
            a->andset(b, true);
            ref->push(a);
        }
        return ref;
    }
    
    TamguIteration* itr = b->Newiteration(false);
    Tamgu* ke;
    
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        for (i = 0; i < sz; i++) {
            ke =  itr->IteratorValue();
            a = getvalue(i);
            if (a->same(ke) == aTRUE) {
                ref->Push(a);
                break;
            }
            a->Release();
        }
    }
    itr->Release();
    
    return ref;
    
}

Exporting Tamgu* Tamgujava_vector::orset(Tamgu *b, bool itself) {
    Tamguvector* ref = globalTamgu->Providevector();
    long sz = Size();
    long i;
    
    Tamgu* a;
    if (!b->isContainer()) {
        for (i = 0; i < sz; i++) {
            a = getvalue(i);
            a->orset(b, true);
            ref->push(a);
        }
        return ref;
    }

    for (i = 0; i < sz; i++) {
        a = getvalue(i);
        ref->push(a);
    }

    TamguIteration* itr = b->Newiteration(false);
    
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        a =  itr->IteratorValue();
        ref->push(a);
    }
    return ref;
}

Exporting Tamgu* Tamgujava_vector::xorset(Tamgu *b, bool itself) {
    Tamguvector* ref = globalTamgu->Providevector();
    long sz = Size();
    long i;
    
    Tamgu* a;
    if (!b->isContainer()) {
        for (i = 0; i < sz; i++) {
            a = getvalue(i);
            a->xorset(b, true);
            ref->push(a);
        }
        return ref;
    }
    
    bool found=false;
    vector<Tamgu*> vals;
    Tamgu* ke;
    
    TamguIteration* itr = b->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next())
        vals.push_back(itr->Value());
    itr->Release();
    
    long itv;
    
    for (i = 0; i < sz; i++) {
        ke = getvalue(i);
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
        else
            ke->Release();
    }
    
    for (itv = 0; itv < vals.size(); itv++) {
        if (vals[itv] == NULL)
            continue;
        ref->Push(vals[itv]);
        vals[itv]->Release();
    }
    
    return ref;
}


Exporting Tamgu* Tamgujava_vector::plus(Tamgu* b, bool itself) {
    Tamguvector* ref = globalTamgu->Providevector();
    Tamgu* kv;
    long sz = Size();
    long i;
    
    Tamgu* a;
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        
        for (i = 0; i < sz; i++) {
            if (itr->End() == aTRUE)
                break;
            
            a = getvalue(i);
            kv = itr->IteratorValue();
            a->plus(kv, true);
            itr->Next();
            ref->push(a);
        }
        itr->Release();
        return ref;
    }
    
    
    for (i = 0; i < sz; i++) {
        a = getvalue(i);
        a->plus(b, true);
        ref->push(a);
    }
    return ref;
}

Exporting Tamgu* Tamgujava_vector::minus(Tamgu *b, bool itself) {
    Tamguvector* ref = globalTamgu->Providevector();
    Tamgu* kv;
    long sz = Size();
    long i;
    
    Tamgu* a;
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        
        for (i = 0; i < sz; i++) {
            if (itr->End() == aTRUE)
                break;
            
            a = getvalue(i);
            kv = itr->IteratorValue();
            a->minus(kv, true);
            itr->Next();
            ref->push(a);
        }
        itr->Release();
        return ref;
    }
    
    
    for (i = 0; i < sz; i++) {
        a = getvalue(i);
        a->minus(b, true);
        ref->push(a);
    }
    return ref;
    
}

Exporting Tamgu* Tamgujava_vector::multiply(Tamgu *b, bool itself) {
    Tamguvector* ref = globalTamgu->Providevector();
    Tamgu* kv;
    long sz = Size();
    long i;
    
    Tamgu* a;
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        
        for (i = 0; i < sz; i++) {
            if (itr->End() == aTRUE)
                break;
            
            a = getvalue(i);
            kv = itr->IteratorValue();
            a->multiply(kv, true);
            itr->Next();
            ref->push(a);
        }
        itr->Release();
        return ref;
    }
    
    
    for (i = 0; i < sz; i++) {
        a = getvalue(i);
        a->multiply(b, true);
        ref->push(a);
    }
    return ref;
}

Exporting Tamgu* Tamgujava_vector::divide(Tamgu *b, bool itself) {
    Tamguvector* ref = globalTamgu->Providevector();
    Tamgu* kv;
    long sz = Size();
    long i;
    
    Tamgu* a;
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        
        for (i = 0; i < sz; i++) {
            if (itr->End() == aTRUE)
                break;
            
            a = getvalue(i);
            kv = itr->IteratorValue();
            kv = a->divide(kv, true);
            if (kv->isError()) {
                itr->Release();
                a->Release();
                ref->Release();
                return kv;
            }
            itr->Next();
            ref->push(a);
        }
        itr->Release();
        return ref;
    }
    
    
    for (i = 0; i < sz; i++) {
        a = getvalue(i);
        kv = a->divide(b, true);
        if (kv->isError()) {
            a->Release();
            ref->Release();
            return kv;
        }
        ref->push(a);
    }
    return ref;
}

Exporting Tamgu* Tamgujava_vector::power(Tamgu *b, bool itself) {
    Tamguvector* ref = globalTamgu->Providevector();
    Tamgu* kv;
    long sz = Size();
    long i;
    
    Tamgu* a;
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        
        for (i = 0; i < sz; i++) {
            if (itr->End() == aTRUE)
                break;
            
            a = getvalue(i);
            kv = itr->IteratorValue();
            a->power(kv, true);
            itr->Next();
            ref->push(a);
        }
        itr->Release();
        return ref;
    }
    
    
    for (i = 0; i < sz; i++) {
        a = getvalue(i);
        a->power(b, true);
        ref->push(a);
    }
    return ref;
}

Exporting Tamgu* Tamgujava_vector::shiftleft(Tamgu *b, bool itself) {
    Tamguvector* ref = globalTamgu->Providevector();
    Tamgu* kv;
    long sz = Size();
    long i;
    
    Tamgu* a;
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        
        for (i = 0; i < sz; i++) {
            if (itr->End() == aTRUE)
                break;
            
            a = getvalue(i);
            kv = itr->IteratorValue();
            a->shiftleft(kv, true);
            itr->Next();
            ref->push(a);
        }
        itr->Release();
        return ref;
    }
    
    
    for (i = 0; i < sz; i++) {
        a = getvalue(i);
        a->shiftleft(b, true);
        ref->push(a);
    }
    return ref;
}

Exporting Tamgu* Tamgujava_vector::shiftright(Tamgu *b, bool itself) {
    Tamguvector* ref = globalTamgu->Providevector();
    Tamgu* kv;
    long sz = Size();
    long i;
    
    Tamgu* a;
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        
        for (i = 0; i < sz; i++) {
            if (itr->End() == aTRUE)
                break;
            
            a = getvalue(i);
            kv = itr->IteratorValue();
            a->shiftright(kv, true);
            itr->Next();
            ref->push(a);
        }
        itr->Release();
        return ref;
    }
    
    
    for (i = 0; i < sz; i++) {
        a = getvalue(i);
        a->shiftright(b, true);
        ref->push(a);
    }
    return ref;
}

Exporting Tamgu* Tamgujava_vector::mod(Tamgu *b, bool itself) {
    Tamguvector* ref = globalTamgu->Providevector();
    Tamgu* kv;
    long sz = Size();
    long i;
    
    Tamgu* a;
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        
        for (i = 0; i < sz; i++) {
            if (itr->End() == aTRUE)
                break;
            
            a = getvalue(i);
            kv = itr->IteratorValue();
            kv = a->mod(kv, true);
            if (kv->isError()) {
                itr->Release();
                a->Release();
                ref->Release();
                return kv;
            }
            itr->Next();
            ref->push(a);
        }
        itr->Release();
        return ref;
    }
    
    
    for (i = 0; i < sz; i++) {
        a = getvalue(i);
        kv = a->mod(b, true);
        if (kv->isError()) {
            a->Release();
            ref->Release();
            return kv;
        }
        ref->push(a);
    }
    return ref;
}


Exporting Tamgu*  Tamgujava_vector::Put(Tamgu* idx, Tamgu* value, short idthread) {
    if (value->Type() == a_iteration_java) {
        if (iter != NULL)
            iter->Resetreference();
        iter = (TamguJavaIteration*)value;
        iter->Setreference();
    }
    if (idx->isIndex() && iter != NULL) {
    //In this specific case, we try to replace a bloc of values with a new bloc
        if (idx->isInterval())
            globalTamgu->Returnerror("Cannot set a java_vector with an interval", idthread);

        long ikey = idx->Getinteger(idthread);
        iter->storevalue(ikey, value);
        value->Release();
    }
    return aTRUE;
}

Exporting Tamgu* Tamgujava_vector::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    
    if (!idx->isIndex()) {
        if (contextualpattern->isLoop())
            return this;
        
        //In this case, we copy the elements from the vector to the map, using the position as index
        if (contextualpattern->isMapContainer()) {
            Tamgu* ke;
            Tamgu* map = Selectamap(contextualpattern);
            char ch[20];
            long sz = Size();
            for (long i = 0; i < sz; i++ ) {
                sprintf_s(ch, 20, "%ld", i);
                ke = getvalue(i);
                map->Push(ch, ke);
            }
            return map;
        }
        
        if (contextualpattern->isNumber()) {
            long v = Size();
            return globalTamgu->ProvideConstint(v);
        }
        return this;
    }
    
    Tamgu* key;
    Tamgu* keyright = NULL;
    
    TamguIndex* kind = (TamguIndex*)idx;
    key = kind->left->Eval(aNULL, aNULL, idthread);
    if (kind->interval == true)
        keyright = kind->right->Eval(aNULL, aNULL, idthread);
    
    long mx = Size();
    long ikey = key->Integer();
    key->Release();
    if (ikey < 0)
        ikey = mx + ikey;
    
    if (ikey >= 0)
        key = getvalue(ikey);
    
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
    
    if (keyright != kind->right)
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
    
    for (long i = ikey; i < iright; i++) {
        kvect->Push(getvalue(i));
    }
    
    return kvect;
}


Exporting Tamgu* Tamgujava_vector::Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {
    
    Tamgu* a;
    uchar addvalue = 0;
    
    if (context != aNOELEMENT)
        addvalue = Selecttype(context);
    
    long sz = Size();
    for (long i = 0; i < sz; i++) {
        recipient->Putvalue(getvalue(i), idthread);
        
        a = bd->DirectEval(environment, aNULL, idthread);
        if (a->isNULL())
            continue;
        
        if (globalTamgu->Error(idthread) || a->needInvestigate()) {
            if (a == aBREAK)
                break;
            
            recipient->Forcedclean();
            a->Release();
            context->Release();
            return globalTamgu->Errorobject(idthread);
        }
        
        context = Storealongtype(context, a, idthread, addvalue);
        a->Release();
    }
    
    recipient->Forcedclean();
    return context;
}

Exporting Tamgu* Tamgujava_vector::Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct) {
    
    Tamgu* returnval;
    
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
    
    long sz = Size();
    for (long i = 0; i < sz; i++) {
        key = getvalue(i);
        if (first) {
            returnval = key->Atom();//We use the first value of the list to seed our accumulator variable
            first = false;
        }
        else {
            var->Putvalue(key, idthread);
            returnval = bd->DirectEval(env, aNULL, idthread);
            
            if (returnval == aBREAK) {
                accu = returnval;
                break;
            }
            
            if (globalTamgu->Error(idthread)) {
                var->Forcedclean();
                if (kcont != NULL)
                    kcont->Release();
                return globalTamgu->Errorobject(idthread);
            }
        }
        
        if (returnval != aNULL) {
            
            accu->Putvalue(returnval, idthread);
            if (kcont != NULL) {
                if (direct)
                    kcont->Insert(0, returnval);
                else
                    kcont->Push(returnval);
            }
            returnval->Release();
        }
    }
    
    var->Forcedclean();
    if (kcont != NULL)
        return kcont;
    return accu->Value();
}

