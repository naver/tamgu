/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgulist.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgulist_h
#define tamgulist_h

#include "tamguint.h"
#include "tamgustring.h"
#include "tamgufvector.h"
#include "tamguvector.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgulist;
//This typedef defines a type "listMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgulist::*listMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------


class Tamgulist : public TamguObjectLockContainer {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<listMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    list<Tamgu*> values;
    bool evaluate;

    //---------------------------------------------------------------------------------------------------------------------
    Tamgulist(TamguGlobal* g, Tamgu* parent = NULL) : TamguObjectLockContainer(g, parent) {
        //Do not forget your variable initialisation
        evaluate = false;
    }

    Tamgulist() {
        //Do not forget your variable initialisation
        evaluate = false;
    }

    //----------------------------------------------------------------------------------------------------------------------
    Exporting Tamgu* Put(Tamgu* index, Tamgu* value, short idthread);Exporting Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    short Type() {
        return Tamgulist::idtype;
    }

    

    static void Setidtype(TamguGlobal* global);
    
    string Typename() {
        return "list";
    }

    bool isObjectContainer() {
        return true;
    }

    bool isContainerClass() {
        return true;
    }

    bool isVectorContainer() {
        return false;
    }

    bool isMapContainer() {
        return false;
    }

    bool duplicateForCall() {
        return false;
    }

    Exporting Tamgu* getvalue(BLONG i);

    Tamgu* Value(Tamgu* a) {
        return getvalue(a->Integer());
    }

    Exporting void Storevalue(string& u);
    Exporting void Storevalue(wstring& u);

    Exporting void storevalue(string u);
    Exporting void storevalue(float u);
    Exporting void storevalue(short u);
    Exporting void storevalue(wstring u);
    Exporting void storevalue(long u);
    Exporting void storevalue(BLONG u);
    Exporting void storevalue(double u);
    Exporting void storevalue(unsigned char u);
    Exporting void storevalue(wchar_t u);

    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        if (exported.find(n) != exported.end())
            return true;
        return false;
    }

    Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
        return new Tamgulist();
    }

    Tamgu* Newvalue(Tamgu* a, short idthread) {
        Tamgulist* v = new Tamgulist();
        if (a->isContainer()) {
            TamguIteration* it = a->Newiteration(false);
            for (it->Begin(); it->End() == aFALSE; it->Next())
                v->Push(it->IteratorValue());
            it->Release();
            return v;
        }
        
        for (long i=0; i<values.size();i++)
            v->Push(a);
        return v;
    }


    Exporting TamguIteration* Newiteration(bool direction);

    static void AddMethod(TamguGlobal* g, string name, listMethod func, unsigned long arity, string infos);
    
    static bool InitialisationModule(TamguGlobal* g, string version);

    
    void Methods(Tamgu* v) {
        string s;
        
        for (auto& it : infomethods) {
            s=it.first;
            v->storevalue(s);
        }
    }
    

    string Info(string n) {

        if (infomethods.find(n) != infomethods.end())
            return infomethods[n];
        return "Unknown method";
    }

    Tamgu* Atom(bool forced = false) {
        if (forced) {
            Tamgulist* v = new Tamgulist;
            Locking _lock(this);

            for (auto& it : values)
                v->Push(it);
            return v;
        }
        return this;
    }


    //---------------------------------------------------------------------------------------------------------------------
    
    void unmark() {
        Locking _lock(this);
        if (loopmark)
            return;
        
        loopmark=true;
        usermark=false;

        for (auto& it : values)
            it->unmark();
        loopmark=false;
    }

    Exporting void Cleanreference(short inc);

    Exporting void Setreference(short r);
    Exporting void Setreference();
    Exporting void Resetreference(short r = 1);
    void Setprotect(bool n) {
        if (loopmark)
            return;
        loopmark=true;
        protect = n;


        Locking* _lock = _getlock(this);
        for (auto& it : values)
            it->Setprotect(n);
        _cleanlock(_lock);
        
        loopmark=false;
    }

    void Protectcontainervalues() {
        if (loopmark)
            return;
        loopmark=true;
        protect = true;

        Locking* _lock = _getlock(this);
        for (auto& it : values)
            it->Setprotect(true);
        _cleanlock(_lock);
        
        loopmark=false;
    }

    void Popping() {
        if (loopmark)
            return;
        loopmark=true;
        protect = false;
        if (Reference() <= 0)
            protect = true;

        Locking* _lock = _getlock(this);
        for (auto& it : values)
            it->Popping();
        _cleanlock(_lock);
        
        loopmark=false;
    }

    bool listValue(list<Tamgu*>::iterator& it, long itx) {
        Locking _lock(this);
        if (itx >= values.size())
            return false;
        it = values.begin();
        while (itx > 0) {
            it++;
            itx--;
        }
        return true;
    }
    //---------------------------------------------------------------------------------------------------------------------
    Tamgu* MethodPush(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* v;
        for (size_t i = 0; i < callfunc->Size(); i++) {
            v = callfunc->Evaluate(i, contextualpattern, idthread);
            if (v->isError())
                return v;
            Push(v);
            v->Release();
        }
        return aTRUE;
    }

    Tamgu* MethodRemove(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* a = callfunc->Evaluate(0, contextualpattern, idthread);
        list<Tamgu*>::iterator it;
        for (it = values.begin(); it != values.end(); it++) {
            if ((*it)->same(a)->Boolean()) {
                values.erase(it);
                return aTRUE;
            }
        }
        return aFALSE;
    }

    Tamgu* MethodFlatten(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* res = globalTamgu->Providevector();
        Flattening(res, this);
        return res;
    }

    Tamgu* MethodClear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Clear();
        return aTRUE;
    }

    Tamgu* MethodPushfirst(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
        return Pushfirst(v);
    }

    Tamgu* MethodLast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Locking _lock(this);
        if (values.size() == 0)
            return aNOELEMENT;
        return values.back();
    }

    Tamgu* MethodFirst(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Locking _lock(this);
        if (values.size() == 0)
            return aNOELEMENT;
        return values.front();
    }

    Tamgu* MethodJoin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //The separator between values
        string sep = callfunc->Evaluate(0, contextualpattern, idthread)->String();
        Locking _lock(this);
        bool beg = true;
        string res;

        for (auto& it : values) {
            if (beg == false)
                res += sep;
            beg = false;
            res += it->String();
        }

        return globalTamgu->Providestring(res);
    }

    Tamgu* MethodMerge(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
        Merging(v);
        return this;
    }

    Tamgu* MethodSum(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        double v = Sum();
        return globalTamgu->Providefloat(v);
    }

    Tamgu* MethodProduct(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        double v = Product();
        return globalTamgu->Providefloat(v);
    }

    Tamgu* MethodInsert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        long i = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        Tamgu* v = callfunc->Evaluate(1, contextualpattern, idthread);
        Insert(i, v);
        return aTRUE;
    }

    Tamgu* MethodPop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* pos = aMINUSONE;
        if (callfunc->Size() == 1)
            pos = callfunc->Evaluate(0, contextualpattern, idthread);
        return Pop(pos);
    }

    Tamgu* MethodPopfirst(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Popfirst();
    }

    Tamgu* MethodPoplast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Poplast();
    }


    Tamgu* MethodUnique(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Unique();
    }

    Tamgu* MethodShuffle(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Shuffle();
        return this;
    }

    Tamgu* MethodPermute(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return booleantamgu[Permute()];
    }

    Tamgu* MethodReverse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Inverse();
    }

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
    Exporting Tamgu* Pushfirst(Tamgu*);Exporting Tamgu* Push(Tamgu*);
    
    Tamgu* push(Tamgu* v) {
        values.push_back(v);
        v->Addreference();
        return this;
    }

    Exporting Tamgu* Pop(Tamgu*);
    Exporting Tamgu* Popfirst();
    Exporting Tamgu* Poplast();
    Tamgu* Last() {
        Locking _lock(this);
        if (values.size() == 0)
            return aNOELEMENT;
        return values.back();
    }

    Tamgu* First() {
        Locking _lock(this);
        if (values.size() == 0)
            return aNOELEMENT;
        return values.front();
    }

    Exporting Tamgu* Unique();
    Exporting void Shuffle();
    Exporting Tamgu* Merging(Tamgu*);
    Exporting Tamgu* Combine(Tamgu*);
    Exporting bool Permute();
    Exporting void Insert(long idx, Tamgu* ke);
    Tamgu* Inverse() {
        Locking _lock(this);
        Tamgulist* vect = new Tamgulist;
        list<Tamgu*>::reverse_iterator it;
        for (it = values.rbegin(); it != values.rend(); it++)
            vect->Push(*it);
        return vect;
    }

    Exporting Tamgu* Map(short idthread);Exporting Tamgu* Vector(short idthread);
    
    Tamgu* Thesum(long i, long j) {
        long sz = values.size();
        if (!sz)
            return aZERO;

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
            
        j++;
        if (j>sz)
            j = sz;

        long n = 0;
        list<Tamgu*>::iterator it = values.begin();
        if ((*it)->isContainer()) {
            Tamgufvector* sub = globalTamgu->Providefvector();
            Tamgu* res;
            for (; it != values.end(); it++) {
                if (n == j)
                    break;
                if (n >= i) {
                    res = (*it)->Thesum(0, (*it)->Size());
                    sub->values.push_back(res->Float());
                    res->Release();
                }
                n++;
            }

            return sub;
        }
        
        if ((*it)->isNumber()) {
            double v = 0;
            for (; it != values.end(); it++) {
                if (n == j)
                    break;
                if (n >= i)
                    v += (*it)->Float();
                n++;
            }

            return globalTamgu->Providefloat(v);
        }

        string v;
        for (; it != values.end(); it++) {
            if (n == j)
                break;
            if (n >= i)
                v += (*it)->String();
            n++;
        }

        return globalTamgu->Providestring(v);
    }

    Tamgu* Theproduct(long i, long j) {
        long sz = values.size();
        if (!sz)
            return aZERO;

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
            
        j++;
        if (j>sz)
            j = sz;


        long n = 0;
        list<Tamgu*>::iterator it = values.begin();
        if ((*it)->isContainer()) {
            Tamgufvector* sub = globalTamgu->Providefvector();
            Tamgu* res;
            for (; it != values.end(); it++) {
                if (n == j)
                    break;
                if (n >= i) {
                    res = (*it)->Theproduct(0, (*it)->Size());
                    sub->values.push_back(res->Float());
                    res->Release();
                }
                n++;
            }

            return sub;
        }
        
        double v = 1;

        for (auto& it : values) {
            if (n == j)
                break;
            if (n >= i)
                v *= it->Float();
            n++;
        }

        return globalTamgu->Providefloat(v);
    }

    double Sum() {
        Locking* _lock = _getlock(this);
        if (values.size() == 0)
            return 0;
        double v = 0;

        for (auto& it : values)
            v += it->Float();
        _cleanlock(_lock);
       return v;
    }

    double Product() {
        Locking* _lock = _getlock(this);
        if (values.size() == 0)
            return 0;
        double v = 1;

        for (auto& it : values)
            v += it->Float();
        _cleanlock(_lock);
        return v;
    }
    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }

    Exporting void Clear();
    Exporting string JSonString();Exporting string String();
    //wstring UString() {}

    Exporting long Integer();Exporting double Float();Exporting BLONG Long();Exporting bool Boolean();

    //Basic operations
    Exporting long Size();
    Exporting Tamgu* in(Tamgu* context, Tamgu* a, short idthread);
    Exporting Tamgu* andset(Tamgu* a, bool itself);Exporting Tamgu* orset(Tamgu* a, bool itself);Exporting Tamgu* xorset(Tamgu* a, bool itself);Exporting Tamgu* plus(Tamgu* a, bool itself);Exporting Tamgu* minus(Tamgu* a, bool itself);Exporting Tamgu* multiply(Tamgu* a, bool itself);Exporting Tamgu* divide(Tamgu* a, bool itself);Exporting Tamgu* power(Tamgu* a, bool itself);Exporting Tamgu* shiftleft(Tamgu* a, bool itself);Exporting Tamgu* shiftright(Tamgu* a, bool itself);Exporting Tamgu* mod(Tamgu* a, bool itself);
    Exporting Tamgu* same(Tamgu* a);
};

class TamguIterationlist : public TamguIteration {
    public:

    long itx;
    list<Tamgu*>::iterator it;
    list<Tamgu*>::reverse_iterator rit;
    Tamgulist* ref;
    Locking _lock;

    TamguIterationlist(Tamgulist* v, bool d, TamguGlobal* g = NULL) : _lock(v), TamguIteration(d, g) {
        ref = v;
        itx = 0;
    }

    Tamgu* Key() {
        return globalTamgu->Provideint(itx);
    }

    Tamgu* Value() {
        if (reverse)
            return (*rit);
        return (*it);
    }

    string Keystring() {
        
        return convertfromnumber(itx);
        
    }

    string Valuestring() {
        if (reverse)
            return (*rit)->String();
        return (*it)->String();
    }

    long Keyinteger() {
        return itx;
    }

    BLONG Keylong() {
        return itx;
    }

    long Valueinteger() {
        if (reverse)
            return (*rit)->Integer();
        return (*it)->Integer();
    }

    double Keyfloat() {
        return itx;
    }

    float Valuedecimal() {
        if (reverse)
            return (*rit)->Decimal();
        return (*it)->Decimal();
    }

    double Valuefloat() {
        if (reverse)
            return (*rit)->Float();
        return (*it)->Float();
    }

    void Next() {
        if (reverse) {
            itx--;
            rit++;
        }
        else {
            itx++;
            it++;
        }
    }

    Tamgu* End() {
        if (reverse) {
            if (rit == ref->values.rend())
                return aTRUE;
        }
        else {
            if (it == ref->values.end())
                return aTRUE;
        }
        return aFALSE;
    }

    Tamgu* Begin() {
        if (reverse) {
            itx = ref->values.size() - 1;
            rit = ref->values.rbegin();
        }
        else {
            itx = 0;
            it = ref->values.begin();
        }
        return aTRUE;
    }
};

    //---------------------------------------------------------------------------------------------------------------------
    //We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
    //of how to declare a new method.
class Tamguring;
    //This typedef defines a type "vectorMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguring::*a_listMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    //---------------------------------------------------------------------------------------------------------------------
class TamguIterationring;

    //---------------------------------------------------------------------------------------------------------------------

class Tamguring : public TamguObjectContainer {
public:
        //We export the methods that will be exposed for our new object
        //this is a static object, which is common to everyone
        //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<a_listMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;
    
    static Exchanging short idtype;
    
        //---------------------------------------------------------------------------------------------------------------------
        //This SECTION is for your specific implementation...
        //Your personal variables here...
    atomic_ring<Tamgu*> values;
    bool isconst;
    
        //---------------------------------------------------------------------------------------------------------------------
    Tamguring(TamguGlobal* g, Tamgu* parent = NULL) : values(aNULL), TamguObjectContainer(g, parent) {
            //Do not forget your variable initialisation
        isconst = false;
    }
    
    Tamguring() : values(aNULL)  {
            //Do not forget your variable initialisation
        isconst = false;
    }
    
        //----------------------------------------------------------------------------------------------------------------------
    Exporting Tamgu* Put(Tamgu* index, Tamgu* value, short idthread);
    Exporting Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    
    void SetConst() {
        isconst = true;
    }
    
    short Type() {
        return Tamguring::idtype;
    }
    
    string Typename() {
        return "a_list";
    }
    
    bool isObjectContainer() {
        return true;
    }
    
    bool isContainerClass() {
        return true;
    }
    
    bool isVectorContainer() {
        return true;
    }
    
    Tamgu* Atom(bool forced = false) {
        if (forced) {
            Tamguring* v = new Tamguring();
            Tamgu* a;
            for (size_t i = 0; i < values.size(); i++) {
                a = values[i]->Atom(true);
                a->Addreference(1);
                v->values.push_back(a);
            }
            
            return v;
        }
        return this;
    }
    
    bool duplicateForCall() {
        return isconst;
    }
    
        //---------------------------------------------------------------------------------------------------------------------
        //Declaration
        //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        if (exported.find(n) != exported.end())
            return true;
        return false;
    }
    
    Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
        return new Tamguring();
    }
    
    Tamgu* Newvalue(Tamgu* a, short idthread) {
        Tamguring* v = new Tamguring();
        if (a->isContainer()) {
            TamguIteration* it = a->Newiteration(false);
            for (it->Begin(); it->End() == aFALSE; it->Next())
                v->Push(it->IteratorValue());
            it->Release();
            return v;
        }
        
        atomic_ring_iterator<Tamgu*> it(values);
        for (; !it.end(); it.next())
            v->Push(a);
        return v;
    }
    
    void AddInstruction(Tamgu* a) {
        a->Addreference(1);
        values.push_back(a);
    }
    
    
    
    Exporting TamguIteration* Newiteration(bool direction);
    
    
    static void AddMethod(TamguGlobal* g, string name, a_listMethod func, unsigned long arity, string infos);
    
    static bool InitialisationModule(TamguGlobal* g, string version);
    
    
    void Methods(Tamgu* v) {
        string s;
        
        for (auto& it : infomethods) {
            s=it.first;
            v->storevalue(s);
        }
    }
    
    string Info(string n) {
        if (infomethods.find(n) != infomethods.end())
            return infomethods[n];
        return "Unknown method";
    }
        //---------------------------------------------------------------------------------------------------------------------
    Tamgu* getvalue(BLONG i) {
        if (i < 0)
            return aNOELEMENT;
        
        Tamgu* a = values[i];
        if (a == NULL)
            return aNOELEMENT;
        return a;
    }
    
    string getstring(long i) {
        if (i < 0)
            return "";
        
        Tamgu* a = values[i];
        if (a == NULL)
            return "";
        return a->String();
    }
    
    wstring getustring(long i) {
        if (i < 0)
            return L"";
        
        Tamgu* a = values[i];
        if (a == NULL)
            return L"";
        return a->UString();
    }
    
    double getfloat(long i) {
        if (i < 0)
            return 0;
        
        Tamgu* a = values[i];
        if (a == NULL)
            return 0;
        return a->Float();
    }
    
    long getinteger(long i) {
        if (i < 0)
            return 0;
        
        Tamgu* a = values[i];
        if (a == NULL)
            return 0;
        return a->Integer();
    }
    
    float getdecimal(long i) {
        if (i < 0)
            return 0;
        
        Tamgu* a = values[i];
        if (a == NULL)
            return 0;
        return a->Decimal();
    }
    
    BLONG getlong(long i) {
        if (i < 0)
            return 0;
        
        Tamgu* a = values[i];
        if (a == NULL)
            return 0;
        return a->Long();
    }
    
    uchar getbyte(long i) {
        if (i < 0)
            return 0;
        
        Tamgu* a = values[i];
        if (a == NULL)
            return 0;
        
        return a->Byte();
    }

    short getshort(long i) {
        if (i < 0)
            return 0;
        
        Tamgu* a = values[i];
        if (a == NULL)
            return 0;
        
        return a->Short();
    }
    
    Tamgu* Value(Tamgu* a) {
        long i = a->Integer();
        if (i < 0)
            return aNOELEMENT;
        
        a = values[i];
        if (a == NULL)
            return aNOELEMENT;
        
        return a;
    }
    
    Exporting void Storevalue(string& u);
    Exporting void Storevalue(wstring& u);
    
    Exporting void storevalue(string u);
    Exporting void storevalue(float u);
    Exporting void storevalue(short u);
    Exporting void storevalue(wstring u);
    Exporting void storevalue(long u);
    Exporting void storevalue(BLONG u);
    Exporting void storevalue(double u);
    Exporting void storevalue(unsigned char u);
    Exporting void storevalue(wchar_t u);
        //---------------------------------------------------------------------------------------------------------------------
    
    void unmark() {
        if (loopmark)
            return;
        
        loopmark=true;
        usermark=false;
        
        atomic_ring_iterator<Tamgu*> it(values);
        
        while (!it.end()) {
            it.second->unmark();
            it.next();
        }
        
        loopmark=false;
    }
    
    Exporting void Cleanreference(short inc);
    
    Exporting void Setreference(short r);
    Exporting void Setreference();
    Exporting void Resetreference(short r = 1);
    void Setprotect(bool n) {
        if (loopmark)
            return;
        loopmark=true;
        protect = n;
        
        atomic_ring_iterator<Tamgu*> it(values);
        while (!it.end()) {
            it.second->Setprotect(n);
            it.next();
        }
        
        loopmark=false;
    }
    
    void Protectcontainervalues() {
        if (loopmark)
            return;
        loopmark=true;
        protect = true;
        
        atomic_ring_iterator<Tamgu*> it(values);
        while (!it.end()) {
            it.second->Setprotect(true);
            it.next();
        }
        
        loopmark=false;
    }
    
    
    void Popping() {
        if (loopmark)
            return;
        loopmark=true;
        protect = false;
        if (Reference() <= 0)
            protect = true;
        
        atomic_ring_iterator<Tamgu*> it(values);
        while (!it.end()) {
            it.second->Popping();
            it.next();
        }
        
        loopmark=false;
    }
        //---------------------------------------------------------------------------------------------------------------------
        //This SECTION is for your specific implementation...
        //This is an example of a function that could be implemented for your needs.
    Tamgu* MethodClear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Clear();
        return aTRUE;
    }
    
    Tamgu* MethodFlatten(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* res = Selectavector(contextualpattern);
        Flattening(res, this);
        return res;
    }
    
    Tamgu* MethodPush(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* v;
        for (size_t i = 0; i < callfunc->Size(); i++) {
            v = callfunc->Evaluate(i, contextualpattern, idthread);
            if (v->isError())
                return v;
            Push(v);
            v->Release();
        }
        return aTRUE;
    }

    Tamgu* MethodPushfirst(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* v;
        for (size_t i = 0; i < callfunc->Size(); i++) {
            v = callfunc->Evaluate(i, contextualpattern, idthread);
            if (v->isError())
                return v;
            Pushfirst(v);
            v->Release();
        }
        return aTRUE;
    }
    
    Tamgu* MethodPopfirst(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Popfirst();
    }
    
    Tamgu* MethodPoplast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Poplast();
    }

    Tamgu* MethodNGrams(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        string u;
        string sep;
        long num =callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        
        if (callfunc->Size() == 2)
            sep = callfunc->Evaluate(1, contextualpattern, idthread)->String();
        
        Tamgu* kvect = Selectasvector(contextualpattern);
        long mx = values.size() - num + 1;
        for (int i = 0; i < mx; i++) {
            u = "";
            for (int j = i; j < i + num; j++) {
                if (j != i)
                    u += sep;
                u += getstring(j);
            }
            kvect->storevalue(u);
        }
        return kvect;
    }
    
    Tamgu* MethodLast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        if (values.size() == 0)
            return aNOELEMENT;
        return values.back();
    }
    
    Tamgu* MethodFirst(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        if (values.size() == 0)
            return aNOELEMENT;
        return values.front();
    }
    
    Tamgu* MethodMerge(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
        return Merging(v);
    }
    
    Tamgu* MethodSum(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        double v = Sum();
        return globalTamgu->Providefloat(v);
    }
    
    Tamgu* MethodProduct(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        double v = Product();
        return globalTamgu->Providefloat(v);
    }
    
    Tamgu* MethodInsert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        long i = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        Tamgu* v = callfunc->Evaluate(1, contextualpattern, idthread);
        return Insertion(i, v);
    }
    
    Tamgu* MethodPop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* pos = aMINUSONE;
        if (callfunc->Size() == 1)
            pos = callfunc->Evaluate(0, contextualpattern, idthread);
        return Pop(pos);
    }
    
    Tamgu* MethodJoin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
            //The separator between values
        string sep = callfunc->Evaluate(0, contextualpattern, idthread)->String();
        bool beg = true;
        string res;
        for (size_t it = 0; it < values.size();it++) {
            if (beg == false)
                res += sep;
            beg = false;
            res += values[it]->String();
        }
        
        return globalTamgu->Providestring(res);
    }
    
    Tamgu* MethodUnique(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Unique();
    }
    
    Tamgu* MethodReverse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Inverse();
    }
    
    Tamgu* MethodRemove(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* a = callfunc->Evaluate(0, contextualpattern, idthread);
        size_t found = -1;
        for (size_t i = 0; i < values.size(); i++) {
            if (values[i]->same(a)->Boolean()) {
                found = i;
                break;
            }
        }
        if (found != -1) {
            values.erase(found)->Removereference(reference + 1);
            return aTRUE;
        }
        return aFALSE;
    }
    
        //---------------------------------------------------------------------------------------------------------
    
        //Raw push
    Tamgu* push(Tamgu* v) {
        values.push_back(v);
        v->Addreference();
        return this;
    }
    
    Exporting Tamgu* Push(Tamgu*);
    Exporting Tamgu* Pushfirst(Tamgu*);
    Exporting Tamgu* Push(TamguGlobal* g, Tamgu* a, short idhtread);
    
    Exporting Tamgu* Pop(Tamgu*);
    
    Tamgu* Popfirst() {
        if (values.size() == 0)
            return aNOELEMENT;
        Tamgu* res = values.remove_front();
        res->Resetreference(reference);
        res->Protect();
        return res;
    }
    
    Tamgu* Poplast() {
        if (values.size() == 0)
            return aNOELEMENT;
        
        Tamgu* res = values.remove_back();
        res->Resetreference(reference);
        res->Protect();
        return res;
    }
    
    void addstringto(string s, int i) {
        if (values.size() == 0)
            return;
        
        Tamgu* ke;
        if (i < 0) {
            ke = values.back();
        }
        else {
            ke = values[i];
            if (ke == NULL)
                return;
        }
        ke->addstringto(s);
    }
    
    void addustringto(wstring s, int i) {
        if (values.size() == 0)
            return;
        
        Tamgu* ke;
        if (i < 0) {
            ke = values.back();
        }
        else {
            ke = values[i];
            if (ke == NULL)
                return;
        }
        ke->addustringto(s);
    }
    
    void addstringto(wchar_t s, int i) {
        if (values.size() == 0)
            return;
        
        Tamgu* ke;
        if (i < 0) {
            ke = values.back();
        }
        else {
            if (i >= values.size())
                return;
            ke = values[i];
        }
        ke->addstringto(s);
    }
    
        //---------------------------------------------------------------------------------------------------------------------
    
        //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
        //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
            //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
            //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }
    
    Tamgu* Last() {
        if (values.size() == 0)
            return aNULL;
        return values.back();
    }
    
    Tamgu* First() {
        if (values.size() == 0)
            return aNULL;
        return values.front();
    }
    
    Exporting Tamgu* Unique();
    
    Tamgu* Inverse() {
        Tamguvector* vect = globalTamgu->Providevector();
        atomic_ring_iterator<Tamgu*> it(values);
        while (!it.end()) {
            vect->values.insert(vect->values.begin(), it.second);
            it.next();
        }
        return vect;
    }
    
    Exporting Tamgu* Merging(Tamgu*);
    Exporting Tamgu* Combine(Tamgu*);
    
    Exporting Tamgu* Map(short idthread);
    Tamgu* Vector(short idthread) {
        Tamguvector* v = globalTamgu->Providevector();
        atomic_ring_iterator<Tamgu*> it(values);
        while (!it.end()) {
            values.push_back(it.second);
            it.next();
        }
        return v;
    }
    
    Exporting Tamgu* Thesum(long i, long j);
    Exporting Tamgu* Theproduct(long i, long j);
    
    double Sum() {
        double v = 0;
        atomic_ring_iterator<Tamgu*> it(values);
        while (!it.end()) {
            v += it.second->Float();
            it.next();
        }
        
        return v;
    }
    
    double Product() {
        if (values.size() == 0)
            return 0;
        
        atomic_ring_iterator<Tamgu*> it(values);
        double v = 1;
        while (!it.end()) {
            v *= it.second->Float();
            it.next();
        }
        return v;
    }
    
    
    Exporting void Clear();
    
    
    Exporting string JSonString();
    Exporting string String();
    
        //wstring UString() {}
    
    Exporting long Integer();
    Exporting double Float();
    Exporting BLONG Long();
    Exporting bool Boolean();
    
    Tamgu* Insertion(long idx, Tamgu* ke) {
        ke = ke->Atom();
        if (!values.insert(idx, ke)) {
            ke->Release();
            return globalTamgu->Returnerror("Error: Ring full");
        }
        ke->Setreference(reference+1);
        return aTRUE;
    }
    
    
        //Basic operations
    Exporting long Size();
    
    Exporting Tamgu* in(Tamgu* context, Tamgu* a, short idthread);
    
    Exporting Tamgu* andset(Tamgu* a, bool itself);
    Exporting Tamgu* orset(Tamgu* a, bool itself);
    Exporting Tamgu* xorset(Tamgu* a, bool itself);
    Exporting Tamgu* plus(Tamgu* a, bool itself);
    Exporting Tamgu* minus(Tamgu* a, bool itself);
    Exporting Tamgu* multiply(Tamgu* a, bool itself);
    Exporting Tamgu* divide(Tamgu* a, bool itself);
    Exporting Tamgu* power(Tamgu* a, bool itself);
    Exporting Tamgu* shiftleft(Tamgu* a, bool itself);
    Exporting Tamgu* shiftright(Tamgu* a, bool itself);
    Exporting Tamgu* mod(Tamgu* a, bool itself);
    
    Exporting Tamgu* same(Tamgu* a);
    
};
    //---------------------------------------------------------------------------------
class TamguIterationring : public TamguIteration {
public:
    atomic_ring_iterator<Tamgu*> ref;
    
    TamguIterationring(Tamguring* v, bool d, TamguGlobal* g = NULL) : ref(v->values), TamguIteration(d, g) {}
    
    Tamgu* Key() {
        return globalTamgu->Provideint(ref.first);
    }
    
    Tamgu* Value() {
        return ref.second;
    }
    
    string Keystring() {
        
        return convertfromnumber((long)ref.first);
        
    }
    
    string Valuestring() {
        return ref.second->String();
    }
    
    long Keyinteger() {
        return ref.first;
    }
    
    long Valueinteger() {
        return ref.second->Integer();
    }
    
    double Keyfloat() {
        return ref.first;
    }
    
    float Valuedecimal() {
        return ref.second->Decimal();
    }
    
    double Valuefloat() {
        return ref.second->Float();
    }
    
    void Next() {
        ref.next();
    }
    
    Tamgu* End() {
        return booleantamgu[ref.end()];
    }
    
    Tamgu* Begin() {
        return aTRUE;
    }
    
};

    //---------------------------------------------------------------------------------

#endif
