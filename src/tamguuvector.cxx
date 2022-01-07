/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguuvector.cxx
 Date       : 2017/09/01
 Purpose    : vector implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgutaskell.h"
#include "tamguuvector.h"
#include "tamguustring.h"
#include "tamgustring.h"
#include "tamgumap.h"
#include "tamguvector.h"
#include "tamguivector.h"
#include "tamgufvector.h"
#include "constobjects.h"
#include "instructions.h"
#include "tamguufile.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<uvectorMethod>  Tamguuvector::methods;
Exporting hmap<string, string> Tamguuvector::infomethods;
Exporting basebin_hash<unsigned long> Tamguuvector::exported;

Exporting short Tamguuvector::idtype = 0;

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguuvector::AddMethod(TamguGlobal* global, string name, uvectorMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamguuvector::Setidtype(TamguGlobal* global) {
    Tamguuvector::InitialisationModule(global,"");
}


   bool Tamguuvector::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();



    Tamguuvector::idtype = global->Getid("uvector");

       Tamguuvector::AddMethod(global, "min", &Tamguuvector::MethodMin, P_NONE, "min(): returns the min in the vector.");
       Tamguuvector::AddMethod(global, "max", &Tamguuvector::MethodMax, P_NONE, "max(): returns the max in the vector.");
       Tamguuvector::AddMethod(global, "minmax", &Tamguuvector::MethodMinMax, P_NONE, "minmax(): returns the min and the max in the vector.");

    Tamguuvector::AddMethod(global, "remove", &Tamguuvector::MethodRemove, P_ONE, "remove(ustring e): remove 'e' from the vector.");

    Tamguuvector::AddMethod(global, "sum", &Tamguuvector::MethodSum, P_NONE, "sum(): concatenate the strings in the vector.");
    Tamguuvector::AddMethod(global, "reverse", &Tamguuvector::MethodReverse, P_NONE, "reverse(): reverse a vector.");
    Tamguuvector::AddMethod(global, "unique", &Tamguuvector::MethodUnique, P_NONE, "unique(): remove duplicate elements.");
    Tamguuvector::AddMethod(global, "reserve", &Tamguuvector::MethodReserve, P_ONE, "reserve(int sz): Reserve a size of 'sz' potential element in the vector.");
    Tamguuvector::AddMethod(global, "last", &Tamguuvector::MethodLast, P_NONE, "last(): return the last element.");
    Tamguuvector::AddMethod(global, "join", &Tamguuvector::MethodJoin, P_ONE, "join(string sep): Produce a string representation for the container.");
    Tamguuvector::AddMethod(global, "shuffle", &Tamguuvector::MethodShuffle, P_NONE, "shuffle(): shuffle the values in the vector.");
    Tamguuvector::AddMethod(global, "permute", &Tamguuvector::MethodPermute, P_NONE, "permute(): permute the values in the vector after each call.");
    Tamguuvector::AddMethod(global, "sort", &Tamguuvector::MethodSort, P_ONE, "sort(bool reverse): sort the elements within.");
    Tamguuvector::AddMethod(global, "push", &Tamguuvector::MethodPush, P_ATLEASTONE, "push(v): Push a value into the vector.");
    Tamguuvector::AddMethod(global, "pop", &Tamguuvector::MethodPop, P_NONE | P_ONE, "pop(int i): Erase an element from the vector");
    Tamguuvector::AddMethod(global, "poplast", &Tamguuvector::MethodPoplast, P_NONE, "poplast(): remove and return the last element from the vector");
    Tamguuvector::AddMethod(global, "merge", &Tamguuvector::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");
    Tamguuvector::AddMethod(global, "editdistance", &Tamguuvector::MethodEditDistance, P_ONE, "editdistance(v): Compute the edit distance with vector 'v'.");
    Tamguuvector::AddMethod(global, "insert", &Tamguuvector::MethodInsert, P_TWO, "insert(int i,v): Insert v at position i.");
    Tamguuvector::AddMethod(global, "ngrams", &Tamguuvector::MethodNGrams, P_ONE|P_TWO, "ngrams(int nb, int sep): produces a ngrams svector.");
    Tamguuvector::AddMethod(global, "read", &Tamguuvector::MethodRead, P_ONE, "read(string path): Read the content of a file into the container.");
    Tamguuvector::AddMethod(global, "write", &Tamguuvector::MethodWrite, P_ONE, "write(string path): write the string content into a file.");
    Tamguuvector::AddMethod(global, "convert", &Tamguuvector::MethodConvert, P_NONE, "convert(): detect number values and convert them into actual numbers. Return a vector object.");


    if (version != "") {
        global->newInstance[Tamguuvector::idtype] = new Tamguuvector(global);
        global->RecordMethods(Tamguuvector::idtype, Tamguuvector::exported);
    }

    Tamgua_uvector::InitialisationModule(global, version);
    global->minimal_indexes[Tamguuvector::idtype] = true;
    return true;
}

Exporting TamguIteration* Tamguuvector::Newiteration(bool direction) {
    return new TamguIterationuvector(this, direction);
}

Tamgu* Tamguuvector::MethodRead(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //The separator between values
    string filename = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    Tamguufile file;
    
    if (!file.openfile(filename)) {
        string msg = "Cannot open file: ";
        msg+=filename;
        return globalTamgu->Returnerror(msg,idthread);
    }
    
    locking();
    values.clear();
    file.readall(values);
    unlocking();
    return this;
}

Tamgu* Tamguuvector::MethodConvert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamguvector* vect = (Tamguvector*)SelectaVector(contextualpattern);
    Doublelocking _lock(this, vect);
    
    short ln;
    double d;
    for (long i = 0; i < values.size(); i++) {
        d = conversionfloathexa(WSTR(values[i]), ln);
        if (ln == values[i].size())
            vect->storevalue(d);
        else
            vect->storevalue(values[i]);
    }
    return vect;
}


Tamgu* Tamguuvector::MethodWrite(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //The separator between values
    string filename = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    Tamguufile file;
    
    if (!file.openfilewrite(filename)) {
        string msg = "Cannot open file: ";
        msg+=filename;
        return globalTamgu->Returnerror(msg,idthread);
    }

    if (file.thefile == NULL) {
        string msg="Cannot open the file:";
        msg += filename;
        return globalTamgu->Returnerror(msg, idthread);
    }
    
    locking();
    for (long i = 0; i < values.size(); i++) {
        file.write(values[i]);
    }
    unlocking();
    
    return this;
}

Exporting Tamgu* Tamguuvector::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    wstring val = a->UString();
     if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Doublelocking _lock(this, v);
        for (size_t i = 0; i < values.size(); i++) {
            if (values[i] == val)
                v->values.push_back(i);
        }
        return v;
    }

   locking();
   if (context->isNumber()) {
       for (size_t i = 0; i < values.size(); i++) {
           if (values[i] == val) {
               unlocking();
               return globalTamgu->ProvideConstint(i);
           }
       }
       unlocking();
       return aMINUSONE;
   }

   for (size_t i = 0; i < values.size(); i++) {
       if (values[i] == val) {
           unlocking();
           return aTRUE;
       }
   }
   unlocking();
   return aFALSE;
}



Exporting Tamgu* Tamguuvector::getvalue(BLONG i) {
    locking();
    if (i < 0 || i >= values.size()) {
        unlocking();
        return aNOELEMENT;
    }
    Tamgu* r = globalTamgu->Provideustring(values[i]);
    unlocking();
    return r;
}

Exporting Tamgu* Tamguuvector::Push(Tamgu* a) {
    locking();
    values.push_back(a->UString());
    unlocking();
    return aTRUE;
}

Exporting Tamgu* Tamguuvector::Pop(Tamgu* idx) {
    locking();
    if (!values.size()) {
        unlocking();
        return aFALSE;
    }

    BLONG v = idx->Integer();
    if (v == -1) {
        v = values.size() - 1;
        values.pop_back();
    }
    else {
        if (v < 0 || v >= (BLONG)values.size()) {
            unlocking();
            return aFALSE;
        }
        values.erase(values.begin() + v);
    }
    unlocking();
    return aTRUE;
}

Exporting void Tamguuvector::Clear() {
    //To set a variable back to empty
    locking();
    values.clear();
    unlocking();
}



Exporting string Tamguuvector::JSonString() {
    string res;
    res = "[";
    bool beg = true;
    string sx;
    locking();
    for (int i = 0; i < values.size(); i++) {
        if (beg == false)
            res += ",";
        beg = false;
        sc_unicode_to_utf8(sx, values[i]);
        jstringing(res, sx);
    }
    unlocking();
    res += "]";
    return res;
}

Exporting string Tamguuvector::String() {
    string res;
    res = "[";
    bool beg = true;
    string sx;
    locking();
    for (int i = 0; i < values.size(); i++) {
        if (beg == false)
            res += ",";
        beg = false;
        sc_unicode_to_utf8(sx, values[i]);
        stringing(res, sx);
    }
    unlocking();
    res += "]";
    return res;
}

Exporting wstring Tamguuvector::UString() {
    locking();
    wstring res;
    res = L"[";
    bool beg = true;
    wstring sx;
    for (int i = 0; i < values.size(); i++) {
        if (beg == false)
            res += L",";
        beg = false;
        sx += values[i];
        ustringing(res, sx);
    }
    unlocking();
    res += L"]";
    return res;
}

Exporting void Tamguuvector::Setstring(string& res, short idthread) {
    res = "[";
    bool beg = true;
    string sx;
    locking();
    for (int i = 0; i < values.size(); i++) {
        if (beg == false)
            res += ",";
        beg = false;
        sc_unicode_to_utf8(sx, values[i]);
        stringing(res, sx);
    }
    unlocking();
    res += "]";
}

Exporting void Tamguuvector::Setstring(wstring& res, short idthread) {
    locking();
    res = L"[";
    bool beg = true;
    wstring sx;
    for (int i = 0; i < values.size(); i++) {
        if (beg == false)
            res += L",";
        beg = false;
        sx += values[i];
        ustringing(res, sx);
    }
    unlocking();
    res += L"]";
}


Exporting long Tamguuvector::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting double Tamguuvector::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting BLONG Tamguuvector::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting bool Tamguuvector::Boolean() {
    locking();
    bool b = values.empty();
    unlocking();
    return !b;
}


//Basic operations
Exporting long Tamguuvector::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting void Tamguuvector::addustringto(wstring ws, int i) {
    locking();
    if (values.size() == 0) {
        unlocking();
        return;
    }

    string s;
    if (i < 0)
        values.back() += ws;
    else {
        if (i >= values.size())
            return;
        values[i] += ws;
    }
    unlocking();
}

Exporting void Tamguuvector::addstringto(wchar_t wc, int i) {
    locking();
    if (values.size() == 0) {
        unlocking();
        return;
    }

    if (i < 0)
        values.back() += wc;
    else {
        if (i >= values.size())
            return;
        values[i] += wc;
    }
    unlocking();
}

Exporting void Tamguuvector::addstringto(string ms, int i) {
    locking();
    if (values.size() == 0) {
        unlocking();
        return;
    }

    wstring s;
    s_utf8_to_unicode(s, USTR(ms), ms.size());
    if (i < 0)
        values.back() += s;
    else {
        if (i >= values.size())
            return;
        values[i] += s;
    }
    unlocking();
}

Exporting Tamgu* Tamguuvector::Map(short idthread) {
    locking();
    Tamgumap* kmap = globalTamgu->Providemap();

    char buff[100];
    for (int it = 0; it < values.size(); it++) {
        sprintf_s(buff, 100, "%d", it);
        kmap->Push(buff, globalTamgu->Provideustring(values[it]));
    }
    unlocking();
    return kmap;
}

Exporting Tamgu* Tamguuvector::Vector(short idthread) {
    locking();
    Tamguvector* kvect = globalTamgu->Providevector();
    kvect->values.reserve(values.size());
    for (int i = 0; i < values.size(); i++)
        kvect->Push(globalTamgu->Provideustring(values[i]));
    unlocking();
    return kvect;
}

Exporting Tamgu*  Tamguuvector::Put(Tamgu* idx, Tamgu* ke, short idthread) {

    Doublelocking _lock(this, ke);

    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            values.clear();
            return aTRUE;
        }

        if (ke->Type() == a_uvector) {
            values = ((Tamguuvector*)ke)->values;
            return aTRUE;
        }

        long sz = ke->Size();
        if (ke->isVectorContainer()) {
            values.clear();
            if (sz) {
                values.reserve(sz);
                for (long i=0; i< sz; i++)
                    values.push_back(ke->getustring(i));
            }
            return aTRUE;
        }
        
        if (ke->isMapContainer()) {
            values.clear();
            if (sz) {
                values.reserve(sz);
                TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    values.push_back(itr->Keyustring());
                itr->Release();
            }
            return aTRUE;
        }
        if (ke->isContainer()) {
            values.clear();
            if (sz) {
                values.reserve(sz);
                TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    values.push_back(itr->Valueustring());
                itr->Release();
            }
            return aTRUE;
        }
        if (ke->isString()) {
            values.clear();
            wstring sv = ke->UString();
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
                values.push_back(ke->getustring(it));
        }
        ke->Release();
        return aTRUE;
    }
    else {
        //In this specific case, we try to replace a bloc of values with a new bloc
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

            if (rkey != lkey)
                rkey--;

            while (rkey >= lkey) {
                values.erase(values.begin() + rkey);
                rkey--;
            }

            if (ke->isVectorContainer()) {
                for (long i = ke->Size() - 1; i >= 0; i--) {
                    values.insert(values.begin() + lkey, ke->getustring(i));
                }
                return aTRUE;
            }

            if (ke->isContainer()) {
                TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    values.insert(values.begin() + lkey, itr->Valueustring());
                itr->Release();
                return aTRUE;
            }

            values.insert(values.begin() + lkey, ke->UString());
        }
        else {
            long ikey = idx->Getinteger(idthread);
            long sz = values.size();
            long nb;
            wstring n = ke->UString();
            if (ikey < 0) {
                ikey = sz + ikey;
                if (ikey < 0)
                    return globalTamgu->Returnerror("Cannot set this value", idthread);
            }

            if (ikey >= sz) {
                for (nb = values.size(); nb < ikey; nb++)
                    values.push_back(L"");
                values.push_back(n);
            }
            else
                values[ikey] = n;
        }
    }
    return aTRUE;
}

Tamgu* Tamguuvector::EvalWithSimpleIndex(Tamgu* key, short idthread, bool sign) {
    key = key->Eval(aNULL, aNULL, idthread);
    locking();
    long ikey;
    if (key->isJustString()) {
        wstring sf = key->UString();
        bool found = false;
        if (sign) {
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
            unlocking();
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
        if (ikey != values.size()) {
            unlocking();
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    
    key = globalTamgu->Provideustring(values[ikey]);
    unlocking();
    return key;
}

Exporting Tamgu* Tamguuvector::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    if (!idx->isIndex()) {
        if (contextualpattern->isLoop())
            return this;

        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer()) {
            Tamgu* kv = Selectauvector(contextualpattern);
            locking();
            if (kv->Type() == a_uvector)
                ((Tamguuvector*)kv)->values = values;
            else
                for (int i = 0; i < values.size(); i++)
                    kv->Push(globalTamgu->Provideustring(values[i]));
            unlocking();
            return kv;
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

    locking();
    long ikey;
    bool stringkey = false;
    if (key->isString()) {
        stringkey = true;
        wstring sf = key->UString();
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
            unlocking();
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
            unlocking();
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }

    if (keyright == NULL) {
        keyright = globalTamgu->Provideustring(values[ikey]);
        unlocking();
        return keyright;
    }

    long iright;
    if (keyright->isString()) {
        wstring sf = keyright->UString();
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
            unlocking();
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

    Tamguuvector* kvect;

    if (keyright != kind->right)
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
    else  {
        if (iright>values.size()) {
            unlocking();
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }

    //In this case, we must create a new vector
    kvect = globalTamgu->Provideuvector();
    for (long i = ikey; i < iright; i++)
        kvect->values.push_back(values[i]);
    unlocking();
    return kvect;
}

Exporting Tamgu* Tamguuvector::same(Tamgu* a) {
    Doublelocking _lock(this, a);

    if (a->Type() != idtype) {
        if (a->isVectorContainer()) {
            if (a->Size() != values.size())
                return aFALSE;
            for (int i = 0; i < values.size(); i++) {
                if (values[i] != a->getustring(i))
                    return aFALSE;
            }
            return aTRUE;
        }
        return aFALSE;
    }

    Tamguuvector* m = (Tamguuvector*)a;
    if (m->values.size() != values.size())
        return aFALSE;

    for (int i = 0; i < values.size(); i++) {
        if (m->values[i] != values[i])
            return aFALSE;
    }
    return aTRUE;
}

Exporting bool Tamguuvector::Permute() {
    locking();
    bool r = next_permutation(values.begin(), values.end());
    unlocking();
    return r;
}

Exporting void Tamguuvector::Shuffle() {
    locking();

    size_t sz = values.size();
    size_t i, f;
    long mx = sz;
    wstring v;
    for (i = 0; i < sz; i++) {
        f = localrandom(mx--);
        if (mx != f) {
            v = values[mx];
            values[mx] = values[f];
            values[f] = v;
        }
    }
    unlocking();
}

Exporting Tamgu* Tamguuvector::Unique() {
    locking();
    Tamguuvector* kvect = globalTamgu->Provideuvector();
    hmap<wstring, bool> inter;
    for (int i = 0; i < values.size(); i++) {
        try {
            inter.at(values[i]);
        }
        catch(const std::out_of_range& oor) {
            inter[values[i]] = true;
            kvect->values.push_back(values[i]);
        }
    }
    unlocking();
    return kvect;
}

Exporting unsigned long Tamguuvector::EditDistance(Tamgu* e) {
    unsigned long s1len, s2len, x, y, lastdiag, olddiag;
    s1len = Size();
    s2len = e->Size();

    wstring* v1 = new wstring[s1len + 1];
    wstring* v2 = new wstring[s2len + 1];

    y =  maxlocal(s1len, s2len);
    for (x = 0; x < y; x++) {
        if (x < s1len)
            v1[x] = values[x];
        if (x < s2len)
            v2[x] = e->getustring(x);
    }

    size_t* column = new size_t[s1len + 1];
    for (y = 1; y <= s1len; y++)
        column[y] = y;

    unsigned long ed;
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

Exporting Tamgu* Tamguuvector::andset(Tamgu *b, bool itself) {
    if (!b->isContainer())
        return aNULL;

    Tamguuvector* ref;

    long size = Size();
    size_t it;

    ref = globalTamgu->Provideuvector();
    locking();
    TamguIteration* itr = b->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        for (it = 0; it < size; it++) {
            if (values[it] == itr->Valueustring()) {
                ref->values.push_back(values[it]);
                break;
            }
        }
    }
    unlocking();
    itr->Release();
    return ref;

}

Exporting Tamgu* Tamguuvector::orset(Tamgu *b, bool itself) {
    Tamguuvector* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamguuvector*)Atom(true);

    ref->Merging(b);
    return ref;
}

Exporting Tamgu* Tamguuvector::xorset(Tamgu *b, bool itself) {
    if (!b->isVectorContainer())
        return aNULL;


    long size = Size();
    size_t it;

    Tamguuvector* ref = globalTamgu->Provideuvector();
    locking();
    wstring s;
    hmap<wstring, bool> store;
    for (it = 0; it < b->Size(); it++)
        store[b->getustring(it)] = true;

    for (it = 0; it < size; it++) {
        if (store.find(values[it]) == store.end())
            store[values[it]] = true;
    }

    for (auto& i : store)
        ref->values.push_back(i.first);
    unlocking();
    return ref;
}

Exporting Tamgu* Tamguuvector::plus(Tamgu* b, bool itself) {
    Tamguuvector* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamguuvector*)Atom(true);

    int it;
    Doublelocking _lock(ref, b);
    long size = values.size();

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] += itr->Valueustring();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    wstring v = b->UString();
    for (it = 0; it < size; it++) {
        ref->values[it] += v;
    }

    return ref;
}

Exporting Tamgu* Tamguuvector::minus(Tamgu *b, bool itself) {
    Tamguuvector* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamguuvector*)Atom(true);

    int it;
    Locking _lock(ref);
    long size = values.size();

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] = StringMinus(ref->values[it], itr->Valueustring());
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    wstring v = b->UString();
    for (it = 0; it < size; it++) {
        ref->values[it] = StringMinus(ref->values[it], v);
    }

    return ref;
}


static bool comp(wstring s1, wstring s2) {
    if (s1<s2)
        return true;
    return false;
}

static bool icomp(wstring s1, wstring s2) {
    if (s1>s2)
        return true;
    return false;
}

Exporting void Tamguuvector::Sort(bool direction) {
    locking();
    if (direction == false)
        sort(values.begin(), values.end(), comp);
    else
        sort(values.begin(), values.end(), icomp);
    unlocking();
}


Exporting Tamgu* Tamguuvector::Merging(Tamgu* ke) {
    if (!ke->isContainer()) {
        locking();
        values.push_back(ke->UString());
        unlocking();
        return this;
    }

    if (ke->Size() == 0)
        return this;

    Doublelocking _lock(this, ke);	//Three cases:

    if (ke->Type() == idtype) {
        Tamguuvector* kvect = (Tamguuvector*)ke;
        values.insert(values.end(), kvect->values.begin(), kvect->values.end());
        return this;
    }

    TamguIteration* itr = ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next())
        values.push_back(itr->Valueustring());
    itr->Release();

    return this;
}

Exporting Tamgu* Tamguuvector::Combine(Tamgu* ke) {
    if (ke->Size() == 0)
        return this;
    
    long i;
    
    if (!ke->isContainer()) {
        wstring v=ke->UString();
        locking();
        for (i=0; i< values.size();i++)
            values[i]+=v;
        unlocking();
        return this;
    }
    
    
    Doublelocking _lock(this, ke);
    i = 0;
    TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        if (i == values.size()) {
            values.push_back(itr->Valueustring());
            i++;
            continue;
        }
        
        values[i] += itr->Valueustring();
        i++;
    }
    itr->Release();
    return this;
}

Exporting Tamgu* Tamguuvector::Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {
    Tamgu* a;
    Tamguustring* it = globalTamgu->Provideustring();
    it->Setreference();

    uchar addvalue = 0;

    if (context != aNOELEMENT)
        addvalue = Selecttype(context);

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

Exporting Tamgu* Tamguuvector::Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct) {

    Tamgu* returnval;

    bool first = false;
    Tamguustring* key = globalTamgu->Provideustring();
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

class UComp {
    public:
    TamguCallFunction2 compare;
    short idthread;
    TamguConstUString p;
    TamguConstUString s;

    UComp(TamguFunction* comp, short idt) : compare(comp), p(L""), s(L""), idthread(idt) {
        compare.arguments.push_back(&p);
        compare.arguments.push_back(&s);
    }

    bool get() {
        return compare.Eval(aNULL, aNULL, idthread)->Boolean();
    }
};

class UComparison {
    public:
    UComp* compare;

    UComparison(UComp* c)  {
        compare = c;
    }

    bool operator() (const wstring& i, const wstring& j) {
        compare->p.value = i;
        compare->s.value = j;
        return compare->get();
    }
};

Tamgu* Tamguuvector::MethodSort(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* comp = callfunc->Evaluate(0, contextualpattern, idthread);
    if (comp->isFunction()) {
        if (comp->Size() != 2)
            return globalTamgu->Returnerror("Expecting a comparison function with two parameters", idthread);
        UComp kcomp((TamguFunction*)comp->Body(idthread), idthread);
        UComparison kfcomp(&kcomp);
        sort(values.begin(), values.end(), kfcomp);
        return this;
    }
    Sort(comp->Boolean());
    return this;
}


Exporting Tamgu* Tamguuvector::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    
    long sz = Size();
    Tamgu* a = aNULL;
    bool testcond = false;
    locking();
    for (long i = 0; i < sz && !testcond; i++) {
        a->Releasenonconst();
        var->storevalue(values[i]);
        a = ins->instructions.vecteur[1]->Eval(context, aNULL, idthread);

        //Continue does not trigger needInvestigate
        testcond = a->needInvestigate();
    }
    unlocking();
    
    if (testcond) {
        if (a == aBREAK)
            return this;
        return a;
    }

    a->Releasenonconst();
    return this;
}




    //We need to declare once again our local definitions.
Exporting basebin_hash<a_uvectorMethod>  Tamgua_uvector::methods;
Exporting hmap<string, string> Tamgua_uvector::infomethods;
Exporting basebin_hash<unsigned long> Tamgua_uvector::exported;

Exporting short Tamgua_uvector::idtype = 0;

    //MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgua_uvector::AddMethod(TamguGlobal* global, string name, a_uvectorMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Tamgua_uvector::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();
    
    
    Tamgua_uvector::idtype = global->Getid("a_uvector");
    
    Tamgua_uvector::AddMethod(global, "clear", &Tamgua_uvector::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgua_uvector::AddMethod(global, "remove", &Tamgua_uvector::MethodRemove, P_ONE, "remove(ustring e): remove 'e' from the vector.");
    
    Tamgua_uvector::AddMethod(global, "sum", &Tamgua_uvector::MethodSum, P_NONE, "sum(): concatenate the strings in the vector.");
    Tamgua_uvector::AddMethod(global, "reverse", &Tamgua_uvector::MethodReverse, P_NONE, "reverse(): reverse a vector.");
    Tamgua_uvector::AddMethod(global, "unique", &Tamgua_uvector::MethodUnique, P_NONE, "unique(): remove duplicate elements.");
    Tamgua_uvector::AddMethod(global, "reserve", &Tamgua_uvector::MethodReserve, P_ONE, "reserve(int sz): Reserve a size of 'sz' potential element in the vector.");
    Tamgua_uvector::AddMethod(global, "last", &Tamgua_uvector::MethodLast, P_NONE, "last(): return the last element.");
    Tamgua_uvector::AddMethod(global, "join", &Tamgua_uvector::MethodJoin, P_ONE, "join(string sep): Produce a string representation for the container.");
    Tamgua_uvector::AddMethod(global, "push", &Tamgua_uvector::MethodPush, P_ATLEASTONE, "push(v): Push a value into the vector.");
    Tamgua_uvector::AddMethod(global, "pop", &Tamgua_uvector::MethodPop, P_NONE | P_ONE, "pop(int i): Erase an element from the vector");
    Tamgua_uvector::AddMethod(global, "poplast", &Tamgua_uvector::MethodPoplast, P_NONE, "poplast(): remove and return the last element from the vector");
    Tamgua_uvector::AddMethod(global, "merge", &Tamgua_uvector::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");
    Tamgua_uvector::AddMethod(global, "insert", &Tamgua_uvector::MethodInsert, P_TWO, "insert(int i,v): Insert v at position i.");
    Tamgua_uvector::AddMethod(global, "ngrams", &Tamgua_uvector::MethodNGrams, P_ONE|P_TWO, "ngrams(int nb, int sep): produces a ngrams a_uvector.");

    if (version != "") {
        global->newInstance[Tamgua_uvector::idtype] = new Tamgua_uvector(global);
        global->RecordMethods(Tamgua_uvector::idtype, Tamgua_uvector::exported);
    }
    
    return true;
}

Exporting TamguIteration* Tamgua_uvector::Newiteration(bool direction) {
    return new TamguIterationa_uvector(this, direction);
}

Exporting Tamgu* Tamgua_uvector::in(Tamgu* context, Tamgu* a, short idthread) {
        //Three cases along the container type...
        //It is a Boolean, we expect false or true
        //It is an integer, we expect a position in v
        //It is a container, we are looking for all positions...
    wstring val = a->UString();
    if (context->isNumber()) {
        atomic_value_vector_iterator<atomic_wstring> it(values);
        for (; !it.end(); it.next()) {
            if (it.second.value() == val)
                return globalTamgu->ProvideConstint(it.first);
        }
        return aMINUSONE;
    }
    
    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Locking _lock(v);
        atomic_value_vector_iterator<atomic_wstring> it(values);
        for (; !it.end(); it.next()) {
            if (it.second.value() == val)
                v->values.push_back(it.first);
        }
        return v;
    }
    
    atomic_value_vector_iterator<atomic_wstring> it(values);
    for (; !it.end(); it.next()) {
        if (it.second.value() == val)
            return aTRUE;
    }
    
    return aFALSE;
}


Exporting Tamgu* Tamgua_uvector::getvalue(BLONG i) {
    if (i < 0 || i >= values.size())
        return aNOELEMENT;
    return globalTamgu->Provideustring(values[i].value());
}


Exporting Tamgu* Tamgua_uvector::Push(Tamgu* a) {
    pushback(a->UString());
    return aTRUE;
}

Exporting Tamgu* Tamgua_uvector::Pop(Tamgu* idx) {
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

Exporting void Tamgua_uvector::Clear() {
        //To set a variable back to empty
    values.clear();
}



Exporting string Tamgua_uvector::JSonString() {
    string res;
    res = "[";
    bool beg = true;
    string sx;
    wstring w;
    atomic_value_vector_iterator<atomic_wstring> it(values);
    for (; !it.end(); it.next()) {
        if (beg == false)
            res += ",";
        beg = false;
        w = it.second.value();
        sc_unicode_to_utf8(sx, w);
        jstringing(res, sx);
    }
    res += "]";
    return res;
}


Exporting string Tamgua_uvector::String() {
    string res;
    res = "[";
    bool beg = true;
    string sx;
    wstring w;
    atomic_value_vector_iterator<atomic_wstring> it(values);
    for (; !it.end(); it.next()) {
        if (beg == false)
            res += ",";
        beg = false;
        w = it.second.value();
        sc_unicode_to_utf8(sx, w);
        stringing(res, sx);
    }
    res += "]";
    return res;
}

Exporting wstring Tamgua_uvector::UString() {
    wstring res;
    res = L"[";
    bool beg = true;
    wstring sx;
    atomic_value_vector_iterator<atomic_wstring> it(values);
    for (; !it.end(); it.next()) {
        if (beg == false)
            res += L",";
        beg = false;
        sx = it.second.value();
        ustringing(res, sx);
    }
    res += L"]";
    return res;
}


Exporting long Tamgua_uvector::Integer() {
    return values.size();
}

Exporting double Tamgua_uvector::Float() {
    return values.size();
}

Exporting BLONG Tamgua_uvector::Long() {
    return values.size();
}

Exporting bool Tamgua_uvector::Boolean() {
    if (values.size() == 0)
        return false;
    return true;
}

Exporting void Tamgua_uvector::addustringto(wstring s, int i) {
    if (values.size() == 0)
        return;
    
    if (i < 0) {
        wstring n = values.back().value();
        n += s;
        values.setback(atomic_wstring(n));
    }
    else {
        if (i >= values.size())
            return;
        wstring n = values[i].value();
        n += s;
        values.set(atomic_wstring(n), i);
    }
}

Exporting void Tamgua_uvector::addstringto(wchar_t wc, int i) {
    if (values.size() == 0)
        return;

    wstring s;
    s = wc;
    if (i < 0) {
        wstring n = values.back().value();
        n += s;
        values.setback(atomic_wstring(n));
    }
    else {
        if (i >= values.size())
            return;
        wstring n = values[i].value();
        n += s;
        values.set(atomic_wstring(n), i);
    }
}

Exporting void Tamgua_uvector::addstringto(string v, int i) {
    if (values.size() == 0)
        return;
    
    wstring s;
    s_utf8_to_unicode(s, USTR(v), v.size());
    
    if (i < 0) {
        wstring n = values.back().value();
        n += s;
        values.setback(atomic_wstring(n));
    }
    else {
        if (i >= values.size())
            return;
        wstring n = values[i].value();
        n += s;
        values.set(atomic_wstring(n), i);
    }
}


    //Basic operations
Exporting long Tamgua_uvector::Size() {
    return values.size();
}


Exporting Tamgu* Tamgua_uvector::Unique() {
    Tamguuvector* kvect = globalTamgu->Provideuvector();
    hmap<wstring, bool> inter;
    atomic_value_vector_iterator<atomic_wstring> it(values);
    wstring w;
    for (; !it.end(); it.next()) {
        w = it.second.value();
        if (inter.find(w) == inter.end()) {
            inter[w] = true;
            kvect->values.push_back(w);
        }
    }
    return kvect;
}

Exporting Tamgu* Tamgua_uvector::same(Tamgu* a) {
    Locking _lock(a);
    
    long i =0;
    
    if (a->Type() != idtype) {
        if (a->isVectorContainer()) {
            if (a->Size() != values.size())
                return aFALSE;
            atomic_value_vector_iterator<atomic_wstring> it(values);
            for (; !it.end(); it.next()) {
                if (it.second.value() != a->getustring(i))
                    return aFALSE;
                i++;
            }
            return aTRUE;
        }
        return aFALSE;
    }
    
    Tamgua_uvector* m = (Tamgua_uvector*)a;
    if (m->values.size() != values.size())
        return aFALSE;
    
    atomic_value_vector_iterator<atomic_wstring> it(values);
    for (; !it.end(); it.next()) {
        if (m->values[i].value() != values[i].value())
            return aFALSE;
        i++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgua_uvector::Map(short idthread) {
    Tamgumap* kmap = globalTamgu->Providemap();
    
    char buff[100];
    atomic_value_vector_iterator<atomic_wstring> it(values);
    for (; !it.end(); it.next()) {
        sprintf_s(buff, 100, "%ld", it.first);
        kmap->Push(buff, globalTamgu->Provideustring(it.second.value()));
    }
    return kmap;
}

Exporting Tamgu* Tamgua_uvector::Vector(short idthread) {
    Tamguvector* kvect = globalTamgu->Providevector();
    kvect->values.reserve(values.size());
    atomic_value_vector_iterator<atomic_wstring> it(values);
    for (; !it.end(); it.next())
        kvect->Push(globalTamgu->Provideustring(it.second.value()));
    return kvect;
}


Exporting Tamgu* Tamgua_uvector::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    
    if (!idx->isIndex()) {
        if (contextualpattern->isLoop())
            return this;

            //particular case, the contextualpattern is a vector, which means that we expect a set of keys
            //as a result
        if (contextualpattern->isVectorContainer()) {
            Tamgu* kv = Selectauvector(contextualpattern);
            if (kv->Type() == Tamgua_uvector::idtype)
                ((Tamgua_uvector*)kv)->values = values;
            else {
                atomic_value_vector_iterator<atomic_wstring> it(values);
                for (; !it.end(); it.next())
                    kv->storevalue(it.second.value());
            }
            return kv;
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
    
    long ikey = 0;
    bool stringkey = false;
    if (key->isString()) {
        stringkey = true;
        wstring sf = key->UString();
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
            atomic_value_vector_iterator<atomic_wstring> it(values);
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
        return globalTamgu->Provideustring(values[ikey].value());
    
    Tamguuvector* kvect;
    long iright;
    if (keyright->isString()) {
        wstring sf = keyright->UString();
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
    
    if (keyright != kind->right)
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
    kvect = globalTamgu->Provideuvector();
    for (long i = ikey; i < iright; i++)
        kvect->values.push_back(values[i].value());
    return kvect;
}

Exporting Tamgu*  Tamgua_uvector::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    
    Locking _lock(ke);
    
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;
        
        if (ke->isNULL()) {
            values.clear();
            return aTRUE;
        }
        
        if (ke->Type() == Tamgua_uvector::idtype) {
            values = ((Tamgua_uvector*)ke)->values;
            return aTRUE;
        }
        
        long sz = ke->Size();
        if (ke->isVectorContainer()) {
            values.clear();
            if (sz) {
                for (long i=0; i< sz; i++)
                    pushback(ke->getustring(i));
            }
            return aTRUE;
        }
        
        if (ke->isMapContainer()) {
            values.clear();
            if (sz) {
                TamguIteration* itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    pushback(itr->Keyustring());
                itr->Release();
            }
            return aTRUE;
        }
        if (ke->isContainer()) {
            values.clear();
            if (sz) {
                TamguIteration* itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    pushback(itr->Valueustring());
                itr->Release();
            }
            return aTRUE;
        }
        if (ke->isString()) {
            values.clear();
            wstring sv = ke->UString();
            vector<wstring> vals;
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
                pushback(ke->getustring(it));
        }
        ke->Release();
        return aTRUE;
    }
    else {
            //In this specific case, we try to replace a bloc of values with a new bloc
        if (idx->isInterval()) {
                //First we remove the values...
            long lkey = idx->Getinteger(idthread);
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
            
            if (rkey != lkey)
                rkey--;
            
            while (rkey >= lkey) {
                values.erase(rkey);
                rkey--;
            }
            
            if (ke->isVectorContainer()) {
                wstring s;
                for (long i = ke->Size() - 1; i >= 0; i--) {
                    s = ke->getustring(i);
                    values.insert(lkey, s);
                }
                return aTRUE;
            }
            
            if (ke->isContainer()) {
                TamguIteration* itr = ke->Newiteration(false);
                wstring s;
                for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
                    s = itr->Valueustring();
                    values.insert(lkey, s);
                }
                itr->Release();
                return aTRUE;
            }
            
            wstring s = ke->UString();
            values.insert(lkey, s);
        }
        else {
            long ikey = idx->Getinteger(idthread);
            long sz = values.size();
            long nb;
            wstring n = ke->UString();
            if (ikey < 0) {
                ikey = sz + ikey;
                if (ikey < 0)
                    return globalTamgu->Returnerror("Cannot set this value", idthread);
            }
            
            if (ikey >= sz) {
                for (nb = values.size(); nb < ikey; nb++)
                    pushback(L"");
                values.push_back(n);
            }
            else
                values.set(n, ikey);
        }
    }
    return aTRUE;
}

Exporting Tamgu* Tamgua_uvector::Merging(Tamgu* ke) {
    if (!ke->isContainer()) {
        pushback(ke->UString());
        return this;
    }
    
    if (ke->Size() == 0)
        return this;
    
    Locking _lock( ke);
        //Three cases:
    TamguIteration* itr = ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next())
        pushback(itr->Valueustring());
    itr->Release();
    return this;
}

Exporting Tamgu* Tamgua_uvector::Combine(Tamgu* ke) {
    if (ke->Size() == 0)
        return this;
    
    wstring w;
    if (!ke->isContainer()) {
        wstring v=ke->UString();
        atomic_value_vector_iterator<atomic_wstring> it(values);
        for (; !it.end();it.next()) {
            w = it.second.value();
            w += v;
            it.replace(w);
        }
        return this;
    }
    
    
    Locking _lock( ke);
    atomic_value_vector_iterator<atomic_wstring> it(values);
    TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        if (!it.end()) {
            pushback(itr->Valueustring());
            continue;
        }
        
        w = it.second.value();
        w +=  itr->Valueustring();
        it.replace(w);
        it.next();
    }
    itr->Release();
    return this;
}

Exporting Tamgu* Tamgua_uvector::andset(Tamgu *b, bool itself) {
    if (!b->isContainer())
        return aNULL;
    
    Tamguuvector* ref;
    
    ref = globalTamgu->Provideuvector();
    wstring w;
    TamguIteration* itr = b->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        atomic_value_vector_iterator<atomic_wstring> it(values);
        for (; !it.end();it.next()) {
            w = it.second.value();
            if (w == itr->Valueustring()) {
                ref->values.push_back(w);
                break;
            }
        }
    }
    itr->Release();
    return ref;
    
}

Exporting Tamgu* Tamgua_uvector::orset(Tamgu *b, bool itself) {
    Tamgua_uvector* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_uvector*)Atom(true);
    
    ref->Merging(b);
    return ref;
}

Exporting Tamgu* Tamgua_uvector::xorset(Tamgu *b, bool itself) {
    if (!b->isVectorContainer())
        return aNULL;
    
    
    size_t it;
    
    Tamguuvector* ref = globalTamgu->Provideuvector();
    string s;
    hmap<wstring, bool> store;
    for (it = 0; it < b->Size(); it++)
        store[b->getustring(it)] = true;
    
    wstring w;
    atomic_value_vector_iterator<atomic_wstring> itx(values);
    for (; !itx.end();itx.next()) {
        w = itx.second.value();
        if (store.find(w) == store.end())
            store[w] = true;
    }
    
    for (auto& i : store)
        ref->values.push_back(i.first);
    
    return ref;
}

Exporting Tamgu* Tamgua_uvector::plus(Tamgu* b, bool itself) {
    Tamgua_uvector* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_uvector*)Atom(true);
    
    Locking _lock(b);
    
    wstring w;
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_value_vector_iterator<atomic_wstring> itx(values);
        for (; !itx.end();itx.next()) {
            if (itr->End() == aTRUE)
                break;
            w = itx.second.value();
            w += itr->Valueustring();
            itx.replace(w);
            itr->Next();
        }
        itr->Release();
        return ref;
    }
    
    wstring v = b->UString();
    atomic_value_vector_iterator<atomic_wstring> itx(values);
    for (; !itx.end();itx.next()) {
        w = itx.second.value();
        w += v;
        itx.replace(w);
    }
    
    return ref;
}

Exporting Tamgu* Tamgua_uvector::minus(Tamgu *b, bool itself) {
    Tamgua_uvector* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_uvector*)Atom(true);
    
    Locking _lock(b);
    
    wstring w;
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_value_vector_iterator<atomic_wstring> itx(values);
        for (; !itx.end();itx.next()) {
            if (itr->End() == aTRUE)
                break;
            w = itx.second.value();
            w = StringMinus(w, itr->Valueustring());
            itx.replace(w);
            itr->Next();
        }
        itr->Release();
        return ref;
    }
    
    wstring v = b->UString();
    atomic_value_vector_iterator<atomic_wstring> itx(values);
    for (; !itx.end();itx.next()) {
        w = itx.second.value();
        w = StringMinus(w, v);
        itx.replace(w);
    }
    
    return ref;
}

Exporting Tamgu* Tamgua_uvector::Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {
    Tamgu* a;
    uchar addvalue = 0;
    
    if (context != aNOELEMENT)
        addvalue = Selecttype(context);
    
    Tamguustring* it = globalTamgu->Provideustring(L"");
    it->Setreference();
    
    recipient->Putvalue( it, idthread);
    
    atomic_value_vector_iterator<atomic_wstring> itx(values);
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

Exporting Tamgu* Tamgua_uvector::Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct) {
    
    Tamgu* returnval;
    
    bool first = false;
    Tamguustring* key = globalTamgu->Provideustring(L"");
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
    
    atomic_value_vector_iterator<atomic_wstring> itx(values);
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


Exporting Tamgu* Tamgua_uvector::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    Tamgu* a = aNULL;
    bool testcond = false;
    
    atomic_value_vector_iterator<atomic_wstring> itx(values);
    for (; !itx.end() && !testcond;itx.next()) {
        a->Releasenonconst();
        var->storevalue(itx.second.value());
        
        a = ins->instructions.vecteur[1]->Eval(context, aNULL, idthread);
        
            //Continue does not trigger needInvestigate
        testcond = a->needInvestigate();
    }
    
    if (testcond) {
        if (a == aBREAK)
            return this;
        return a;
    }
    
    a->Releasenonconst();
    return this;
}
