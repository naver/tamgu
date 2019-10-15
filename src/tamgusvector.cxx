/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgusvector.cxx
 Date       : 2017/09/01
 Purpose    : vector implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgutaskell.h"
#include "tamgusvector.h"
#include "tamgustring.h"
#include "tamgumap.h"
#include "tamguvector.h"
#include "tamguivector.h"
#include "tamgufvector.h"
#include "constobjects.h"
#include "instructions.h"
#include "tamgufile.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif


//We need to declare once again our local definitions.
Exporting basebin_hash<svectorMethod>  Tamgusvector::methods;
Exporting hmap<string, string> Tamgusvector::infomethods;
Exporting bin_hash<unsigned long> Tamgusvector::exported;

Exporting short Tamgusvector::idtype = 0;

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgusvector::AddMethod(TamguGlobal* global, string name, svectorMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgusvector::Setidtype(TamguGlobal* global) {
        Tamgusvector::idtype = global->Getid("svector");
    }

   bool Tamgusvector::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Tamgusvector::idtype = global->Getid("svector");

    Tamgusvector::AddMethod(global, "clear", &Tamgusvector::MethodClear, P_NONE, "clear(): clear the container.");

    Tamgusvector::AddMethod(global, "remove", &Tamgusvector::MethodRemove, P_ONE, "remove(e): remove 'e' from the vector.");

    Tamgusvector::AddMethod(global, "sum", &Tamgusvector::MethodSum, P_NONE, "sum(): concatenate the strings in the vector.");
    Tamgusvector::AddMethod(global, "reverse", &Tamgusvector::MethodReverse, P_NONE, "reverse(): reverse a vector.");
    Tamgusvector::AddMethod(global, "unique", &Tamgusvector::MethodUnique, P_NONE, "unique(): remove duplicate elements.");
    Tamgusvector::AddMethod(global, "reserve", &Tamgusvector::MethodReserve, P_ONE, "reserve(int sz): Reserve a size of 'sz' potential element in the vector.");
    Tamgusvector::AddMethod(global, "last", &Tamgusvector::MethodLast, P_NONE, "last(): return the last element.");
    Tamgusvector::AddMethod(global, "join", &Tamgusvector::MethodJoin, P_ONE, "join(string sep): Produce a string representation for the container.");
    Tamgusvector::AddMethod(global, "shuffle", &Tamgusvector::MethodShuffle, P_NONE, "shuffle(): shuffle the values in the vector.");
    Tamgusvector::AddMethod(global, "sort", &Tamgusvector::MethodSort, P_ONE, "sort(bool reverse): sort the elements within.");
    Tamgusvector::AddMethod(global, "push", &Tamgusvector::MethodPush, P_ATLEASTONE, "push(v): Push a value into the vector.");
    Tamgusvector::AddMethod(global, "pop", &Tamgusvector::MethodPop, P_NONE | P_ONE, "pop(i): Erase an element from the vector");
    Tamgusvector::AddMethod(global, "poplast", &Tamgusvector::MethodPoplast, P_NONE, "poplast(): remove and return the last element from the vector");
    Tamgusvector::AddMethod(global, "merge", &Tamgusvector::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");
    Tamgusvector::AddMethod(global, "editdistance", &Tamgusvector::MethodEditDistance, P_ONE, "editdistance(v): Compute the edit distance with vector 'v'.");
    Tamgusvector::AddMethod(global, "insert", &Tamgusvector::MethodInsert, P_TWO, "insert(i,v): Insert v at position i.");
    Tamgusvector::AddMethod(global, "ngrams", &Tamgusvector::MethodNGrams, P_ONE|P_TWO, "ngrams(int nb, int sep): produces a ngrams svector.");
    Tamgusvector::AddMethod(global, "permute", &Tamgusvector::MethodPermute, P_NONE, "permute(): permute the values in the vector after each call.");
    Tamgusvector::AddMethod(global, "read", &Tamgusvector::MethodRead, P_ONE, "read(string path): Read the content of a file into the container.");
    Tamgusvector::AddMethod(global, "write", &Tamgusvector::MethodWrite, P_ONE, "write(string path): write the string content into a file.");


    global->newInstance[Tamgusvector::idtype] = new Tamgusvector(global);
    global->newInstance[a_vectorthrough] = global->newInstance[Tamgusvector::idtype];
    global->RecordMethods(Tamgusvector::idtype, Tamgusvector::exported);
    global->RecordMethods(a_vectorthrough, Tamgusvector::exported);

    Tamgua_svector::InitialisationModule(global, version);
    
    return true;
}

Exporting TamguIteration* Tamgusvector::Newiteration(bool direction) {
    return new TamguIterationsvector(this, direction);
}

Tamgu* Tamgusvector::MethodRead(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //The separator between values
    string filename = callfunc->Evaluate(0, contextualpattern, idthread)->String();
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
    
    Locking _lock(this);
    values.clear();
    file.readall(values);
    
    return this;
}

Tamgu* Tamgusvector::MethodWrite(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //The separator between values
    string filename = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    Tamgufile file;
    
#ifdef WIN32
    fopen_s(&file.thefile, STR(filename), "wb");
#else
    file.thefile=fopen(STR(filename), "wb");
#endif
    
    if (file.thefile == NULL) {
        string msg="Cannot open the file:";
        msg += filename;
        return globalTamgu->Returnerror(msg, idthread);
    }
    
    Locking _lock(this);
    for (long i = 0; i < values.size(); i++)
        file.write(values[i]);

    return this;
}


Exporting Tamgu* Tamgusvector::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    string val = a->String();
     if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Doublelocking _lock(this, v);
        for (size_t i = 0; i < values.size(); i++) {
            if (values[i] == val)
                v->values.push_back(i);
        }
        return v;
    }

   if (context->isNumber()) {
        Locking _lock(this);
        for (size_t i = 0; i < values.size(); i++) {
            if (values[i] == val)
                return globalTamgu->Provideint(i);
        }
        return aMINUSONE;
    }

    Locking _lock(this);
    for (size_t i = 0; i < values.size(); i++) {
        if (values[i] == val)
            return aTRUE;
    }

    return aFALSE;
}


Exporting Tamgu* Tamgusvector::getvalue(BLONG i) {
    Locking _lock(this);
    if (i < 0 || i >= values.size())
        return aNOELEMENT;
    return globalTamgu->Providestring(values[i]);
}


Exporting Tamgu* Tamgusvector::Push(Tamgu* a) {
    Locking _lock(this);
    values.push_back(a->String());
    return aTRUE;
}

Exporting Tamgu* Tamgusvector::Pop(Tamgu* idx) {
    Locking _lock(this);
    if (!values.size())
        return aFALSE;

    BLONG v = idx->Integer();
    if (v == -1) {
        v = values.size() - 1;
        values.pop_back();
    }
    else {
        if (v < 0 || v >= (BLONG)values.size())
            return aFALSE;
        values.erase(values.begin() + v);
    }
    return aTRUE;
}

Exporting void Tamgusvector::Clear() {
    //To set a variable back to empty
    Locking _lock(this);
    values.clear();
}



Exporting string Tamgusvector::JSonString() {
    string res;
    res = "[";
    bool beg = true;
    string sx;
    for (int i = 0; i < values.size(); i++) {
        if (beg == false)
            res += ",";
        beg = false;
        sx = values[i];
        jstringing(res, sx);
    }
    res += "]";
    return res;
}


Exporting string Tamgusvector::String() {
    string res;
    res = "[";
    bool beg = true;
    string sx;
    for (int i = 0; i < values.size(); i++) {
        if (beg == false)
            res += ",";
        beg = false;
        sx = values[i];
        stringing(res, sx);
    }
    res += "]";
    return res;
}


Exporting long Tamgusvector::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Tamgusvector::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Tamgusvector::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Tamgusvector::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}

Exporting void Tamgusvector::addustringto(wstring ws, int i) {
    Locking _lock(this);
    if (values.size() == 0)
        return;

    string s;
    s_unicode_to_utf8(s, ws);
    if (i < 0)
        values.back() += s;
    else {
        if (i >= values.size())
            return;
        values[i] += s;
    }
}

Exporting void Tamgusvector::addstringto(wchar_t wc, int i) {
    Locking _lock(this);
    if (values.size() == 0)
        return;

    string s = c_unicode_to_utf8(wc);
    if (i < 0)
        values.back() += s;
    else {
        if (i >= values.size())
            return;
        values[i] += s;
    }
}

Exporting void Tamgusvector::addstringto(string s, int i) {
    Locking _lock(this);
    if (values.size() == 0)
        return;

    if (i < 0)
        values.back() += s;
    else {
        if (i >= values.size())
            return;
        values[i] += s;
    }
}


//Basic operations
Exporting long Tamgusvector::Size() {
    Locking _lock(this);
    return values.size();
}

Exporting unsigned long Tamgusvector::EditDistance(Tamgu* e) {
    unsigned long s1len, s2len, x, y, lastdiag, olddiag;
    s1len = Size();
    s2len = e->Size();

    string* v1 = new string[s1len + 1];
    string* v2 = new string[s2len + 1];

    y = max(s1len, s2len);
    for (x = 0; x < y; x++) {
        if (x < s1len)
            v1[x] = values[x];
        if (x < s2len)
            v2[x] = e->getstring(x);
    }

    size_t* column = new size_t[s1len + 1];
    for (y = 1; y <= s1len; y++)
        column[y] = y;

    unsigned int ed;
    for (x = 1; x <= s2len; x++) {
        column[0] = x;
        for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
            olddiag = column[y];
            ed = StringEditDistance(v1[y - 1], v2[x - 1]);
            column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + ed);
            lastdiag = olddiag;
        }
    }

    delete[] v1;
    delete[] v2;

    s2len = column[s1len];
    delete[] column;
    return s2len;

}


Exporting void Tamgusvector::Shuffle() {
    Locking _lock(this);

    size_t sz = values.size();
    size_t i, f;
    int mx = sz;
    string v;
    for (i = 0; i < sz; i++) {
        f = localrandom(mx--);
        if (mx != f) {
            v = values[mx];
            values[mx] = values[f];
            values[f] = v;
        }
    }
}

Exporting Tamgu* Tamgusvector::Unique() {
    Locking _lock(this);
    Tamgusvector* kvect = globalTamgu->Providesvector();
    hmap<string, bool> inter;
    for (long i = 0; i < values.size(); i++) {
        if (inter.find(values[i]) == inter.end()) {
            inter[values[i]] = true;
            kvect->values.push_back(values[i]);
        }
    }
    return kvect;
}

Exporting bool Tamgusvector::Permute() {
    Locking _lock(this);
    return next_permutation(values.begin(), values.end());
}

Exporting Tamgu* Tamgusvector::same(Tamgu* a) {
    Doublelocking _lock(this, a);

    if (a->Type() != idtype) {
        if (a->isVectorContainer()) {
            if (a->Size() != values.size())
                return aFALSE;
            for (int i = 0; i < values.size(); i++) {
                if (values[i] != a->getstring(i))
                    return aFALSE;
            }
            return aTRUE;
        }
        return aFALSE;
    }

    Tamgusvector* m = (Tamgusvector*)a;
    if (m->values.size() != values.size())
        return aFALSE;

    for (int i = 0; i < values.size(); i++) {
        if (m->values[i] != values[i])
            return aFALSE;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgusvector::Map(short idthread) {
    Locking _lock(this);
    Tamgumap* kmap = globalTamgu->Providemap();

    char buff[100];
    for (int it = 0; it < values.size(); it++) {
        sprintf_s(buff, 100, "%d", it);
        kmap->Push(buff, globalTamgu->Providestring(values[it]));
    }
    return kmap;
}

Exporting Tamgu* Tamgusvector::Vector(short idthread) {
    Locking _lock(this);
    Tamguvector* kvect = globalTamgu->Providevector();
    kvect->values.reserve(values.size());
    for (int i = 0; i < values.size(); i++)
        kvect->Push(globalTamgu->Providestring(values[i]));
    return kvect;
}


Exporting Tamgu* Tamgusvector::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    Locking _lock(this);

    if (!idx->isIndex()) {
        if (contextualpattern->isLoop())
            return this;

        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer()) {
            Tamgu* kv = Selectasvector(contextualpattern);
            if (kv->Type() == a_svector)
                ((Tamgusvector*)kv)->values = values;
            else
                for (int i = 0; i < values.size(); i++)
                    kv->Push(globalTamgu->Providestring(values[i]));
            return kv;
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

    long ikey;
    bool stringkey = false;
    if (key->Type() == a_string) {
        stringkey = true;
        string sf = key->String();
        bool found = false;
        if (kind->signleft) {
            for (ikey = values.size() - 1; ikey >= 0; ikey--) {
                if (sf == values[ikey]) {
                    found = true;
                    break;
                }
            }
        }
        else {
            for (ikey = 0; ikey < values.size(); ikey++) {
                if (sf == values[ikey]) {
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    else
        ikey = key->Integer();

    key->Release();
    
    if (ikey < 0)
        ikey = values.size() + ikey;

    if (ikey < 0 || ikey >= values.size()) {
        if (ikey != values.size() || keyright == NULL) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }

    if (keyright == NULL)
        return globalTamgu->Providestring(values[ikey]);

    Tamgusvector* kvect;
    long iright;
    if (keyright->Type() == a_string) {
        string sf = keyright->String();
        bool found = false;
        if (kind->signright) {
            for (iright = values.size() - 1; iright >= 0; iright--) {
                if (sf == values[iright]) {
                    found = true;
                    iright++;
                    break;
                }
            }
        }
        else {
            for (iright = 0; iright < values.size(); iright++) {
                if (sf == values[iright]) {
                    found = true;
                    iright++;
                    break;
                }
            }
        }
        if (!found) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    else {
        if (keyright == aNULL)
            iright = 0;
        else {
            iright = keyright->Integer();
            if (stringkey && iright >= 0)
                iright = ikey + iright + 1;
        }
    }

    keyright->Release();
    if (iright < 0 || keyright == aNULL) {
        iright = values.size() + iright;
        if (iright<ikey) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    else  {
        if (iright>values.size()) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }

    //In this case, we must create a new vector
    kvect = globalTamgu->Providesvector();
    for (long i = ikey; i < iright; i++)
        kvect->values.push_back(values[i]);
    return kvect;
}

Exporting Tamgu*  Tamgusvector::Put(Tamgu* idx, Tamgu* ke, short idthread) {

    Doublelocking _lock(this, ke);

    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            values.clear();
            return aTRUE;
        }

        if (ke->Type() == a_svector) {
            values = ((Tamgusvector*)ke)->values;
            return aTRUE;
        }
        
        long sz = ke->Size();
        if (ke->isVectorContainer()) {
            values.clear();
            if (sz) {
                values.reserve(sz);
                for (long i=0; i< sz; i++)
                    values.push_back(ke->getstring(i));
            }
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            values.clear();
            if (sz) {
                values.reserve(sz);
                TamguIteration* itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    values.push_back(itr->Keystring());
                itr->Release();
            }
            return aTRUE;
        }
        if (ke->isContainer()) {
            values.clear();
            if (sz) {
                values.reserve(sz);
                TamguIteration* itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    values.push_back(itr->Valuestring());
                itr->Release();
            }
            return aTRUE;
        }
        if (ke->isString()) {
            values.clear();
            string sv = ke->String();
            if (!v_comma_split_string(sv, values))
                return globalTamgu->Returnerror("Cannot set this value", idthread);
            return aTRUE;
        }
        ke = ke->Vector(idthread);
        if (!ke->isVectorContainer())
            return globalTamgu->Returnerror("Cannot set this value", idthread);
        
        sz = ke->Size();
        values.clear();
        if (sz) {
            values.reserve(sz);
            //We copy all values from ke to this
            for (long it = 0; it < ke->Size(); ++it)
                values.push_back(ke->getstring(it));
        }
        ke->Release();
        return aTRUE;
    }
    else {
        //In this specific case, we try to replace a bloc of values with a new bloc
        if (idx->isInterval()) {
            //First we remove the values...
            long lkey = idx->Integer();
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

            if (rkey != lkey)
                rkey--;

            while (rkey >= lkey) {
                values.erase(values.begin() + rkey);
                rkey--;
            }

            if (ke->isVectorContainer()) {
                for (int i = ke->Size() - 1; i >= 0; i--) {
                    values.insert(values.begin() + lkey, ke->getstring(i));
                }
                return aTRUE;
            }

            if (ke->isContainer()) {
                TamguIteration* itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    values.insert(values.begin() + lkey, itr->Valuestring());
                itr->Release();
                return aTRUE;
            }

            values.insert(values.begin() + lkey, ke->String());
        }
        else {
            long ikey = idx->Integer();
            int sz = values.size();
            int nb;
            string n = ke->String();
            if (ikey < 0) {
                ikey = sz + ikey;
                if (ikey < 0)
                    return globalTamgu->Returnerror("Cannot set this value", idthread);
            }

            if (ikey >= sz) {
                for (nb = values.size(); nb < ikey; nb++)
                    values.push_back("");
                values.push_back(n);
            }
            else
                values[ikey] = n;
        }
    }
    return aTRUE;
}

Exporting Tamgu* Tamgusvector::Merging(Tamgu* ke) {
    if (!ke->isContainer()) {
        values.push_back(ke->String());
        return this;
    }

    if (ke->Size() == 0)
        return this;

    Doublelocking _lock(this, ke);
    //Three cases:
    if (ke->Type() == idtype) {
        Tamgusvector* kvect = (Tamgusvector*)ke;
        values.insert(values.end(), kvect->values.begin(), kvect->values.end());
        return this;
    }

    TamguIteration* itr = ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next())
        values.push_back(itr->Valuestring());
    itr->Release();
    return this;
}

Exporting Tamgu* Tamgusvector::Combine(Tamgu* ke) {
    if (ke->Size() == 0)
        return this;
    
    long i;
    
    if (!ke->isContainer()) {
        string v=ke->String();
        Locking _lock(this);
        for (i=0; i< values.size();i++)
            values[i]+=v;
        return this;
    }
    
    
    Doublelocking _lock(this, ke);
    i = 0;
    TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        if (i == values.size()) {
            values.push_back(itr->Valuestring());
            i++;
            continue;
        }
        
        values[i] += itr->Valuestring();
        i++;
    }
    itr->Release();
    return this;
}

Exporting Tamgu* Tamgusvector::andset(Tamgu *b, bool itself) {
    if (!b->isContainer())
        return aNULL;

    Tamgusvector* ref;

    long size = Size();
    size_t it;

    ref = globalTamgu->Providesvector();
    Locking _lock(this);
    TamguIteration* itr = b->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        for (it = 0; it < size; it++) {
            if (values[it] == itr->Valuestring()) {
                ref->values.push_back(values[it]);
                break;
            }
        }
    }
    itr->Release();
    return ref;

}

Exporting Tamgu* Tamgusvector::orset(Tamgu *b, bool itself) {
    Tamgusvector* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgusvector*)Atom(true);

    ref->Merging(b);
    return ref;
}

Exporting Tamgu* Tamgusvector::xorset(Tamgu *b, bool itself) {
    if (!b->isVectorContainer())
        return aNULL;


    long size = Size();
    size_t it;

    Tamgusvector* ref = globalTamgu->Providesvector();
    Locking _lock(this);
    string s;
    hmap<string, bool> store;
    for (it = 0; it < b->Size(); it++)
        store[b->getstring(it)] = true;

    for (it = 0; it < size; it++) {
        if (store.find(values[it]) == store.end())
            store[values[it]] = true;
    }

    for (auto& i : store)
        ref->values.push_back(i.first);

    return ref;
}

Exporting Tamgu* Tamgusvector::plus(Tamgu* b, bool itself) {
    Tamgusvector* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgusvector*)Atom(true);

    int it;
    Doublelocking _lock(ref, b);
    long size = values.size();

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] += itr->Valuestring();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    string v = b->String();
    for (it = 0; it < size; it++) {
        ref->values[it] += v;
    }

    return ref;
}

Exporting Tamgu* Tamgusvector::minus(Tamgu *b, bool itself) {
    Tamgusvector* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgusvector*)Atom(true);

    int it;
    Locking _lock(ref);
    long size = values.size();

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] = StringMinus(ref->values[it], itr->Valuestring());
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    string v = b->String();
    for (it = 0; it < size; it++) {
        ref->values[it] = StringMinus(ref->values[it], v);
    }

    return ref;
}


static bool comp(string s1, string s2) {
    if (s1<s2)
        return true;
    return false;
}

static bool icomp(string s1, string s2) {
    if (s1>s2)
        return true;
    return false;
}

Exporting void Tamgusvector::Sort(bool direction) {
    Locking _lock(this);
    if (direction == false)
        sort(values.begin(), values.end(), comp);
    else
        sort(values.begin(), values.end(), icomp);
}


Exporting Tamgu* Tamgusvector::Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {
    Tamgu* a;
    uchar addvalue = 0;

    if (context != aNOELEMENT)
        addvalue = Selecttype(context);

    Tamgustring* it = globalTamgu->Providestring("");
    it->Setreference();

    recipient->Putvalue( it, idthread);

    for (size_t i = 0; i < values.size(); i++) {
        it->value = values[i];

        a = bd->DirectEval(environment, aNULL, idthread);
        if (a->isNULL())
            continue;

        if (globalTamgu->Error(idthread) || a->needInvestigate()) {
            if (a == aBREAK)
                break;

            it->Resetreference();
            recipient->Forcedclean();
            a->Release();
            context->Release();
            return globalTamgu->Errorobject(idthread);
        }

        context = Storealongtype(context, a, idthread, addvalue);
        a->Release();
    }

    it->Resetreference();
    recipient->Forcedclean();

    return context;
}

Exporting Tamgu* Tamgusvector::Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct) {

    Tamgu* returnval;

    bool first = false;
    Tamgustring* key = globalTamgu->Providestring("");
    key->Setreference();

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

    for (size_t i = 0; i < values.size(); i++) {
        key->value = values[i];
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
                key->Resetreference();
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

    key->Resetreference();
    var->Forcedclean();
    if (kcont != NULL)
        return kcont;
    return accu->Value();
}

class SComp {
    public:
    TamguCallFunction compare;
    short idthread;
    TamguConstString p;
    TamguConstString s;

    SComp(TamguFunction* comp, short idt) : compare(comp), p(""), s(""), idthread(idt) {
        compare.arguments.push_back(&p);
        compare.arguments.push_back(&s);
    }

    bool get() {
        return compare.Eval(aNULL, aNULL, idthread)->Boolean();
    }
};

class SComparison {
    public:
    SComp* compare;

    SComparison(SComp* c)  {
        compare = c;
    }

    bool operator() (const string& i, const string& j) {
        compare->p.value = i;
        compare->s.value = j;
        return compare->get();
    }
};

Tamgu* Tamgusvector::MethodSort(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* comp = callfunc->Evaluate(0, contextualpattern, idthread);
    if (comp->isFunction()) {
        if (comp->Size() != 2)
            return globalTamgu->Returnerror("Expecting a comparison function with two parameters", idthread);
        SComp kcomp((TamguFunction*)comp->Body(idthread), idthread);
        SComparison kfcomp(&kcomp);
        sort(values.begin(), values.end(), kfcomp);
        return this;
    }
    Sort(comp->Boolean());
    return this;
}


Exporting Tamgu* Tamgusvector::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Locking _lock(this);
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    
    Tamgu* a;
    for (long i = 0; i < values.size(); i++) {
        var->storevalue(values[i]);

        a = ins->instructions.vecteur[1]->Eval(context, aNULL, idthread);

        //Continue does not trigger needInvestigate
        if (a->needInvestigate()) {
            if (a == aBREAK)
                break;
            return a;
        }

        a->Release();
    }

    return this;
}


//We need to declare once again our local definitions.
Exporting basebin_hash<a_svectorMethod>  Tamgua_svector::methods;
Exporting hmap<string, string> Tamgua_svector::infomethods;
Exporting bin_hash<unsigned long> Tamgua_svector::exported;

Exporting short Tamgua_svector::idtype = 0;

    //MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgua_svector::AddMethod(TamguGlobal* global, string name, a_svectorMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Tamgua_svector::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();
    
    
    Tamgua_svector::idtype = global->Getid("a_svector");
    
    Tamgua_svector::AddMethod(global, "clear", &Tamgua_svector::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgua_svector::AddMethod(global, "remove", &Tamgua_svector::MethodRemove, P_ONE, "remove(e): remove 'e' from the vector.");
    
    Tamgua_svector::AddMethod(global, "sum", &Tamgua_svector::MethodSum, P_NONE, "sum(): concatenate the strings in the vector.");
    Tamgua_svector::AddMethod(global, "reverse", &Tamgua_svector::MethodReverse, P_NONE, "reverse(): reverse a vector.");
    Tamgua_svector::AddMethod(global, "unique", &Tamgua_svector::MethodUnique, P_NONE, "unique(): remove duplicate elements.");
    Tamgua_svector::AddMethod(global, "reserve", &Tamgua_svector::MethodReserve, P_ONE, "reserve(int sz): Reserve a size of 'sz' potential element in the vector.");
    Tamgua_svector::AddMethod(global, "last", &Tamgua_svector::MethodLast, P_NONE, "last(): return the last element.");
    Tamgua_svector::AddMethod(global, "join", &Tamgua_svector::MethodJoin, P_ONE, "join(string sep): Produce a string representation for the container.");
    Tamgua_svector::AddMethod(global, "push", &Tamgua_svector::MethodPush, P_ATLEASTONE, "push(v): Push a value into the vector.");
    Tamgua_svector::AddMethod(global, "pop", &Tamgua_svector::MethodPop, P_NONE | P_ONE, "pop(i): Erase an element from the vector");
    Tamgua_svector::AddMethod(global, "poplast", &Tamgua_svector::MethodPoplast, P_NONE, "poplast(): remove and return the last element from the vector");
    Tamgua_svector::AddMethod(global, "merge", &Tamgua_svector::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");
    Tamgua_svector::AddMethod(global, "insert", &Tamgua_svector::MethodInsert, P_TWO, "insert(i,v): Insert v at position i.");
    Tamgua_svector::AddMethod(global, "ngrams", &Tamgua_svector::MethodNGrams, P_ONE|P_TWO, "ngrams(int nb, int sep): produces a ngrams a_svector.");
    
    
    
    
    global->newInstance[Tamgua_svector::idtype] = new Tamgua_svector(global);
    global->RecordMethods(Tamgua_svector::idtype, Tamgua_svector::exported);
    
    return true;
}

Exporting TamguIteration* Tamgua_svector::Newiteration(bool direction) {
    return new TamguIterationa_svector(this, direction);
}

Exporting Tamgu* Tamgua_svector::in(Tamgu* context, Tamgu* a, short idthread) {
        //Three cases along the container type...
        //It is a Boolean, we expect false or true
        //It is an integer, we expect a position in v
        //It is a container, we are looking for all positions...
    string val = a->String();
    if (context->isNumber()) {
        atomic_value_vector_iterator<atomic_string> it(values);
        for (; !it.end(); it.next()) {
            if (it.second.value() == val)
                return globalTamgu->Provideint(it.first);
        }
        return aMINUSONE;
    }
    
    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Locking _lock(v);
        atomic_value_vector_iterator<atomic_string> it(values);
        for (; !it.end(); it.next()) {
            if (it.second.value() == val)
                v->values.push_back(it.first);
        }
        return v;
    }
    
    atomic_value_vector_iterator<atomic_string> it(values);
    for (; !it.end(); it.next()) {
        if (it.second.value() == val)
            return aTRUE;
    }
    
    return aFALSE;
}


Exporting Tamgu* Tamgua_svector::getvalue(BLONG i) {
    if (i < 0 || i >= values.size())
        return aNOELEMENT;
    return globalTamgu->Providestring(values[i].value());
}


Exporting Tamgu* Tamgua_svector::Push(Tamgu* a) {
    pushback(a->String());
    return aTRUE;
}

Exporting Tamgu* Tamgua_svector::Pop(Tamgu* idx) {
    if (!values.size())
        return aFALSE;
    
    BLONG v = idx->Integer();
    if (v == -1) {
        v = values.size() - 1;
        values.pop_back();
    }
    else {
        if (v < 0 || v >= (BLONG)values.size())
            return aFALSE;
        values.pop(v);
    }
    return aTRUE;
}

Exporting void Tamgua_svector::Clear() {
        //To set a variable back to empty
    values.clear();
}



Exporting string Tamgua_svector::JSonString() {
    string res;
    res = "[";
    bool beg = true;
    string sx;
    atomic_value_vector_iterator<atomic_string> it(values);
    for (; !it.end(); it.next()) {
        if (beg == false)
            res += ",";
        beg = false;
        sx = it.second.value();
        jstringing(res, sx);
    }
    res += "]";
    return res;
}


Exporting string Tamgua_svector::String() {
    string res;
    res = "[";
    bool beg = true;
    string sx;
    atomic_value_vector_iterator<atomic_string> it(values);
    for (; !it.end(); it.next()) {
        if (beg == false)
            res += ",";
        beg = false;
        sx = it.second.value();
        stringing(res, sx);
    }
    res += "]";
    return res;
}


Exporting long Tamgua_svector::Integer() {
    return values.size();
}

Exporting double Tamgua_svector::Float() {
    return values.size();
}

Exporting BLONG Tamgua_svector::Long() {
    return values.size();
}

Exporting bool Tamgua_svector::Boolean() {
    if (values.size() == 0)
        return false;
    return true;
}

Exporting void Tamgua_svector::addustringto(wstring ws, int i) {
    if (values.size() == 0)
        return;
    
    string s;
    s_unicode_to_utf8(s, ws);
    if (i < 0) {
        string n = values.back().value();
        n += s;
        values.setback(atomic_string(n));
    }
    else {
        if (i >= values.size())
            return;
        string n = values[i].value();
        n += s;
        values.set(atomic_string(n), i);
    }
}

Exporting void Tamgua_svector::addstringto(wchar_t wc, int i) {
    if (values.size() == 0)
        return;
    
    string s = c_unicode_to_utf8(wc);
    if (i < 0) {
        string n = values.back().value();
        n += s;
        values.setback(atomic_string(n));
    }
    else {
        if (i >= values.size())
            return;
        string n = values[i].value();
        n += s;
        values.set(atomic_string(n), i);
    }
}

Exporting void Tamgua_svector::addstringto(string s, int i) {
    if (values.size() == 0)
        return;
    
    if (i < 0) {
        string n = values.back().value();
        n += s;
        values.setback(atomic_string(n));
    }
    else {
        if (i >= values.size())
            return;
        string n = values[i].value();
        n += s;
        values.set(atomic_string(n), i);
    }
}


    //Basic operations
Exporting long Tamgua_svector::Size() {
    return values.size();
}


Exporting Tamgu* Tamgua_svector::Unique() {
    Tamgusvector* kvect = globalTamgu->Providesvector();
    hmap<string, bool> inter;
    atomic_value_vector_iterator<atomic_string> it(values);
    string w;
    for (; !it.end(); it.next()) {
        w = it.second.value();
        if (inter.find(w) == inter.end()) {
            inter[w] = true;
            kvect->values.push_back(w);
        }
    }
    return kvect;
}

Exporting Tamgu* Tamgua_svector::same(Tamgu* a) {
    Locking _lock(a);
    
    long i =0;

    if (a->Type() != idtype) {
        if (a->isVectorContainer()) {
            if (a->Size() != values.size())
                return aFALSE;
            atomic_value_vector_iterator<atomic_string> it(values);
            for (; !it.end(); it.next()) {
                if (it.second.value() != a->getstring(i))
                    return aFALSE;
                i++;
            }
            return aTRUE;
        }
        return aFALSE;
    }
    
    Tamgua_svector* m = (Tamgua_svector*)a;
    if (m->values.size() != values.size())
        return aFALSE;
    
    atomic_value_vector_iterator<atomic_string> it(values);
    for (; !it.end(); it.next()) {
        if (m->values[i].value() != values[i].value())
            return aFALSE;
        i++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgua_svector::Map(short idthread) {
    Tamgumap* kmap = globalTamgu->Providemap();
    
    char buff[100];
    atomic_value_vector_iterator<atomic_string> it(values);
    for (; !it.end(); it.next()) {
        sprintf_s(buff, 100, "%ld", it.first);
        kmap->Push(buff, globalTamgu->Providestring(it.second.value()));
    }
    return kmap;
}

Exporting Tamgu* Tamgua_svector::Vector(short idthread) {
    Tamguvector* kvect = globalTamgu->Providevector();
    kvect->values.reserve(values.size());
    atomic_value_vector_iterator<atomic_string> it(values);
    for (; !it.end(); it.next())
        kvect->Push(globalTamgu->Providestring(it.second.value()));
    return kvect;
}


Exporting Tamgu* Tamgua_svector::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    
    if (!idx->isIndex()) {
        if (contextualpattern->isLoop())
            return this;

            //particular case, the contextualpattern is a vector, which means that we expect a set of keys
            //as a result
        if (contextualpattern->isVectorContainer()) {
            Tamgu* kv = Selectasvector(contextualpattern);
            if (kv->Type() == Tamgua_svector::idtype)
                ((Tamgua_svector*)kv)->values = values;
            else {
                atomic_value_vector_iterator<atomic_string> it(values);
                for (; !it.end(); it.next())
                    kv->storevalue(it.second.value());
            }
            return kv;
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
    
    long ikey = 0;
    bool stringkey = false;
    if (key->Type() == a_string) {
        stringkey = true;
        string sf = key->String();
        bool found = false;
        if (kind->signleft) {
            for (ikey = values.size() - 1; ikey >= 0; ikey--) {
                if (sf == values[ikey].value()) {
                    found = true;
                    break;
                }
            }
        }
        else {
            atomic_value_vector_iterator<atomic_string> it(values);
            for (; !it.end(); it.next()) {
                if (sf == it.second.value()) {
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    else
        ikey = key->Integer();
    
    key->Release();
    
    if (ikey < 0)
        ikey = values.size() + ikey;
    
    if (ikey < 0 || ikey >= values.size()) {
        if (ikey != values.size() || keyright == NULL) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    
    if (keyright == NULL)
        return globalTamgu->Providestring(values[ikey].value());
    
    Tamgusvector* kvect;
    long iright;
    if (keyright->Type() == a_string) {
        string sf = keyright->String();
        bool found = false;
        if (kind->signright) {
            for (iright = values.size() - 1; iright >= 0; iright--) {
                if (sf == values[iright].value()) {
                    found = true;
                    iright++;
                    break;
                }
            }
        }
        else {
            for (iright = 0; iright < values.size(); iright++) {
                if (sf == values[iright].value()) {
                    found = true;
                    iright++;
                    break;
                }
            }
        }
        if (!found) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    else {
        if (keyright == aNULL)
            iright = 0;
        else {
            iright = keyright->Integer();
            if (stringkey && iright >= 0)
                iright = ikey + iright + 1;
        }
    }
    
    keyright->Release();
    if (iright < 0 || keyright == aNULL) {
        iright = values.size() + iright;
        if (iright<ikey) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    else  {
        if (iright>values.size()) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    
        //In this case, we must create a new vector
    kvect = globalTamgu->Providesvector();
    for (long i = ikey; i < iright; i++)
        kvect->values.push_back(values[i].value());
    return kvect;
}

Exporting Tamgu*  Tamgua_svector::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    
    Locking _lock(ke);
    
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;
        
        if (ke->isNULL()) {
            values.clear();
            return aTRUE;
        }
        
        if (ke->Type() == Tamgua_svector::idtype) {
            values = ((Tamgua_svector*)ke)->values;
            return aTRUE;
        }
        
        long sz = ke->Size();
        if (ke->isVectorContainer()) {
            values.clear();
            if (sz) {
                for (long i=0; i< sz; i++)
                    pushback(ke->getstring(i));
            }
            return aTRUE;
        }
        
        if (ke->isMapContainer()) {
            values.clear();
            if (sz) {
                TamguIteration* itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    pushback(itr->Keystring());
                itr->Release();
            }
            return aTRUE;
        }
        if (ke->isContainer()) {
            values.clear();
            if (sz) {
                TamguIteration* itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    pushback(itr->Valuestring());
                itr->Release();
            }
            return aTRUE;
        }
        if (ke->isString()) {
            values.clear();
            string sv = ke->String();
            vector<string> vals;
            if (!v_comma_split_string(sv, vals))
                return globalTamgu->Returnerror("Cannot set this value", idthread);
            for (long i = 0; i < vals.size(); i++)
                pushback(vals[i]);
            return aTRUE;
        }
        ke = ke->Vector(idthread);
        if (!ke->isVectorContainer())
            return globalTamgu->Returnerror("Cannot set this value", idthread);
        
        sz = ke->Size();
        values.clear();
        if (sz) {
                //We copy all values from ke to this
            for (long it = 0; it < ke->Size(); ++it)
                pushback(ke->getstring(it));
        }
        ke->Release();
        return aTRUE;
    }
    else {
            //In this specific case, we try to replace a bloc of values with a new bloc
        if (idx->isInterval()) {
                //First we remove the values...
            long lkey = idx->Integer();
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
            
            if (rkey != lkey)
                rkey--;
            
            while (rkey >= lkey) {
                values.erase(rkey);
                rkey--;
            }
            
            if (ke->isVectorContainer()) {
                string s;
                for (long i = ke->Size() - 1; i >= 0; i--) {
                    s = ke->getstring(i);
                    values.insert(lkey, s);
                }
                return aTRUE;
            }
            
            if (ke->isContainer()) {
                TamguIteration* itr = ke->Newiteration(false);
                string s;
                for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
                    s = itr->Valuestring();
                    values.insert(lkey, s);
                }
                itr->Release();
                return aTRUE;
            }
            
            string s = ke->String();
            values.insert(lkey, s);
        }
        else {
            long ikey = idx->Integer();
            long sz = values.size();
            long nb;
            string n = ke->String();
            if (ikey < 0) {
                ikey = sz + ikey;
                if (ikey < 0)
                    return globalTamgu->Returnerror("Cannot set this value", idthread);
            }
            
            if (ikey >= sz) {
                for (nb = values.size(); nb < ikey; nb++)
                    pushback("");
                values.push_back(n);
            }
            else
                values.set(n, ikey);
        }
    }
    return aTRUE;
}

Exporting Tamgu* Tamgua_svector::Merging(Tamgu* ke) {
    if (!ke->isContainer()) {
        pushback(ke->String());
        return this;
    }
    
    if (ke->Size() == 0)
        return this;
    
    Locking _lock( ke);
        //Three cases:
    TamguIteration* itr = ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next())
        pushback(itr->Valuestring());
    itr->Release();
    return this;
}

Exporting Tamgu* Tamgua_svector::Combine(Tamgu* ke) {
    if (ke->Size() == 0)
        return this;
    
    string w;
    if (!ke->isContainer()) {
        string v=ke->String();
        atomic_value_vector_iterator<atomic_string> it(values);
        for (; !it.end();it.next()) {
            w = it.second.value();
            w += v;
            it.replace(w);
        }
        return this;
    }
    
    
    Locking _lock( ke);
    atomic_value_vector_iterator<atomic_string> it(values);
    TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        if (!it.end()) {
            pushback(itr->Valuestring());
            continue;
        }

        w = it.second.value();
        w +=  itr->Valuestring();
        it.replace(w);
        it.next();
    }
    itr->Release();
    return this;
}

Exporting Tamgu* Tamgua_svector::andset(Tamgu *b, bool itself) {
    if (!b->isContainer())
        return aNULL;
    
    Tamgusvector* ref;
    
    ref = globalTamgu->Providesvector();
    string w;
    TamguIteration* itr = b->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        atomic_value_vector_iterator<atomic_string> it(values);
        for (; !it.end();it.next()) {
            w = it.second.value();
            if (w == itr->Valuestring()) {
                ref->values.push_back(w);
                break;
            }
        }
    }
    itr->Release();
    return ref;
    
}

Exporting Tamgu* Tamgua_svector::orset(Tamgu *b, bool itself) {
    Tamgua_svector* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_svector*)Atom(true);
    
    ref->Merging(b);
    return ref;
}

Exporting Tamgu* Tamgua_svector::xorset(Tamgu *b, bool itself) {
    if (!b->isVectorContainer())
        return aNULL;
    
    
    size_t it;
    
    Tamgusvector* ref = globalTamgu->Providesvector();
    string s;
    hmap<string, bool> store;
    for (it = 0; it < b->Size(); it++)
        store[b->getstring(it)] = true;
    
    string w;
    atomic_value_vector_iterator<atomic_string> itx(values);
    for (; !itx.end();itx.next()) {
        w = itx.second.value();
        if (store.find(w) == store.end())
            store[w] = true;
    }
    
    for (auto& i : store)
        ref->values.push_back(i.first);
    
    return ref;
}

Exporting Tamgu* Tamgua_svector::plus(Tamgu* b, bool itself) {
    Tamgua_svector* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_svector*)Atom(true);
    
    Locking _lock(b);
    
    string w;
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_value_vector_iterator<atomic_string> itx(values);
        for (; !itx.end();itx.next()) {
            if (itr->End() == aTRUE)
                break;
            w = itx.second.value();
            w += itr->Valuestring();
            itx.replace(w);
            itr->Next();
        }
        itr->Release();
        return ref;
    }
    
    string v = b->String();
    atomic_value_vector_iterator<atomic_string> itx(values);
    for (; !itx.end();itx.next()) {
        w = itx.second.value();
        w += v;
        itx.replace(w);
    }
    
    return ref;
}

Exporting Tamgu* Tamgua_svector::minus(Tamgu *b, bool itself) {
    Tamgua_svector* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_svector*)Atom(true);
    
    Locking _lock(b);
    
    string w;
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_value_vector_iterator<atomic_string> itx(values);
        for (; !itx.end();itx.next()) {
            if (itr->End() == aTRUE)
                break;
            w = itx.second.value();
            w = StringMinus(w, itr->Valuestring());
            itx.replace(w);
            itr->Next();
        }
        itr->Release();
        return ref;
    }
    
    string v = b->String();
    atomic_value_vector_iterator<atomic_string> itx(values);
    for (; !itx.end();itx.next()) {
        w = itx.second.value();
        w = StringMinus(w, v);
        itx.replace(w);
    }
    
    return ref;
}

Exporting Tamgu* Tamgua_svector::Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {
    Tamgu* a;
    uchar addvalue = 0;
    
    if (context != aNOELEMENT)
        addvalue = Selecttype(context);
    
    Tamgustring* it = globalTamgu->Providestring("");
    it->Setreference();
    
    recipient->Putvalue( it, idthread);
    
    atomic_value_vector_iterator<atomic_string> itx(values);
    for (; !itx.end();itx.next()) {
        it->value = itx.second.value();
        
        a = bd->DirectEval(environment, aNULL, idthread);
        if (a->isNULL())
            continue;
        
        if (globalTamgu->Error(idthread) || a->needInvestigate()) {
            if (a == aBREAK)
                break;
            
            it->Resetreference();
            recipient->Forcedclean();
            a->Release();
            context->Release();
            return globalTamgu->Errorobject(idthread);
        }
        
        context = Storealongtype(context, a, idthread, addvalue);
        a->Release();
    }
    
    it->Resetreference();
    recipient->Forcedclean();
    
    return context;
}

Exporting Tamgu* Tamgua_svector::Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct) {
    
    Tamgu* returnval;
    
    bool first = false;
    Tamgustring* key = globalTamgu->Providestring("");
    key->Setreference();
    
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
    
    atomic_value_vector_iterator<atomic_string> itx(values);
    for (; !itx.end();itx.next()) {
        key->value = itx.second.value();
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
                key->Resetreference();
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
    
    key->Resetreference();
    var->Forcedclean();
    if (kcont != NULL)
        return kcont;
    return accu->Value();
}


Exporting Tamgu* Tamgua_svector::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    Tamgu* a;
    atomic_value_vector_iterator<atomic_string> itx(values);
    for (; !itx.end();itx.next()) {
        var->storevalue(itx.second.value());
        
        a = ins->instructions.vecteur[1]->Eval(context, aNULL, idthread);
        
            //Continue does not trigger needInvestigate
        if (a->needInvestigate()) {
            if (a == aBREAK)
                break;
            return a;
        }
        
        a->Release();
    }
    
    return this;
}
