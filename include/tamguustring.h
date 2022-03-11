/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguustring.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamguustring_h
#define tamguustring_h


#include "tamguint.h"

//---------------------------------------------------------------------------------------------------------------------
const long base_atomic_wstring_size = 7;
const long atomic_wstring_size = 1 << base_atomic_wstring_size;

class atomic_wstring_element {
public:
    
    wchar_t buffer[atomic_wstring_size+1];
    atomic_wstring_element* next;
    
    atomic_wstring_element() {
        buffer[0] = 0;
        buffer[atomic_wstring_size] = 0;
        next = NULL;
    }
    
    ~atomic_wstring_element() {
        if (next != NULL)
            delete next;
    }
    
    void reset() {
        buffer[0] = 0;
        buffer[atomic_wstring_size] = 0;
        if (next != NULL)
            next->reset();
    }
    
    void copy(wstring& v, long sz, long pos, std::recursive_mutex* lock) {
        long i;
        if ((sz-pos) < atomic_wstring_size) {
            for (i = 0; i < sz-pos; i++)
                buffer[i] = v[i+pos];
            buffer[sz-pos]=0;
        }
        else {
            for (i = 0; i < atomic_wstring_size; i++)
                buffer[i] = v[i+pos];
            buffer[atomic_wstring_size] = 0;
            
            if (sz > pos+atomic_wstring_size) {
                if (next == NULL) {
                    lock->lock();
                    if (next == NULL)
                        next = new atomic_wstring_element;
                    lock->unlock();
                    
                }
                next->copy(v,sz,pos+atomic_wstring_size, lock);
            }
        }
    }
    
    long size() {
#ifdef WSTRING_IS_UTF16
		long sz = size_w(buffer);
#else
		long sz = wcslen(buffer);
#endif
		if (next != NULL)
            sz += next->size();
        return sz;
    }
    
    void copy(atomic_wstring_element* a, std::recursive_mutex* lock) {
        long sz = wcslen(a->buffer);

        for (long i = 0; i < sz; i++)
            buffer[i] = a->buffer[i];
        
        buffer[sz] = 0;
        
        if (a->next) {
            if (next == NULL) {
                lock->lock();
                if (next == NULL)
                    next = new atomic_wstring_element;
                lock->unlock();
            }
            
            next->copy(a->next, lock);
        }
    }
    
    void concat(wstring& v, long nb, long i, std::recursive_mutex* lock ) {
        atomic_wstring_element* n = this;
        while (nb) {
            if (n->next == NULL) {
                lock->lock();
                if (n->next == NULL)
                    n->next = new atomic_wstring_element;
                lock->unlock();
            }
            n = n->next;
            --nb;
        }
        
        long szv = v.size();
        long j = 0;
        while (i < atomic_wstring_size && j < szv)
            n->buffer[i++] = v[j++];
        
        if (i == atomic_wstring_size || j == szv)
            n->buffer[i] = 0;
        
        if (j < szv) {
            if (n->next == NULL) {
                lock->lock();
                if (n->next == NULL)
                    n->next = new atomic_wstring_element;
                lock->unlock();
            }
            n->next->copy(v, szv, j, lock);
        }
    }
    
    bool compare(atomic_wstring_element* a) {
        for (long i = 0; i < atomic_wstring_size; i++)
            if (buffer[i] != a->buffer[i])
                return false;
        
        if (a->next) {
            if (next)
                return next->compare(a->next);
            else
                return false;
        }
        return true;
    }
    
};

class atomic_wstring {
public:
    atomic_wstring_element* head;
    std::recursive_mutex* lock;
    
    atomic_wstring() {
        lock = new std::recursive_mutex;
        head = new atomic_wstring_element;
    }
    
    
    atomic_wstring(wstring& v) {
        lock = new std::recursive_mutex;
        head = new atomic_wstring_element;
        head->copy(v, v.size(), 0, lock);
    }
    
    ~atomic_wstring() {
        delete lock;
        delete head;
    }
    
    void reset() {
        head->reset();
    }
    
    void clear() {
        head->reset();
    }
    
    void concat(wstring v) {
        long sz = v.size();
        if (!sz)
            return;
        
        if (isempty()) {
            head->copy(v, sz, 0, lock);
            return;
        }
        
        long size  = head->size();
        
        long nb = size >> base_atomic_wstring_size;
        long i = size - (nb << base_atomic_wstring_size);
        head->concat(v, nb, i, lock);
    }
    
    atomic_wstring& operator=(wstring v) {
        reset();
        long sz = v.size();
        if (sz)
            head->copy(v, sz, 0, lock);
        return *this;
    }
    
    atomic_wstring& operator=(atomic_wstring& a) {
        reset();
        if (!a.isempty())
            head->copy(a.head, lock);
        return *this;
    }
    
    char operator[](long pos) {
        long nb = pos >> base_atomic_wstring_size;
        long i = pos - (nb << base_atomic_wstring_size);
        atomic_wstring_element* n = head;
        while (nb > 0) {
            if (n == NULL)
                return 0;
            --nb;
            n = n->next;
        }
        
        if (n == NULL || i >= wcslen(n->buffer))
            return 0;
        
        return n->buffer[i];
    }
    
    void substr(atomic_wstring& v, long pos, long sz) {
        long nb = pos >> base_atomic_wstring_size;
        long i = pos - (nb << base_atomic_wstring_size);
        atomic_wstring_element* n = head;
        while (nb > 0) {
            if (n == NULL)
                return;
            --nb;
            n = n->next;
        }
        
        if (n == NULL || i >= wcslen(n->buffer))
            return;
        
        
        atomic_wstring_element* nv = v.head;
        long start = 0;
        while (sz) {
            if (i == atomic_wstring_size) {
                n = n->next;
                if (n == NULL)
                    break;
                i = 0;
            }
            if (start == atomic_wstring_size) {
                if (nv->next == NULL) {
                    lock->lock();
                    if (nv->next == NULL)
                        nv->next = new atomic_wstring_element;
                    lock->unlock();
                }
                nv = nv->next;
                start = 0;
            }
            nv->buffer[start++] = n->buffer[i++];
            --sz;
        }
        
        nv->buffer[start] = 0;
    }
    
    bool compare(atomic_wstring& a) {
        if (a.isempty()) {
            if (!isempty())
                return false;
            return true;
        }
        
        if (head->size() != a.size())
            return false;
        
        return head->compare(a.head);
    }
    
    friend bool operator!=(atomic_wstring& a, atomic_wstring& b) {
        return !a.compare(b);
    }
    
    friend bool operator==(atomic_wstring& a, atomic_wstring& b) {
        return a.compare(b);
    }
    
    bool operator==(wstring v) {
        atomic_wstring_element* n = head;
        long vi = 0;
        long i = 0;
        long sz = v.size();
        while (vi < sz) {
            if (i == atomic_wstring_size) {
                n = n->next;
                if (n == NULL)
                    return false;
                i = 0;
            }
            if (n->buffer[i] != v[vi])
                return false;
            ++i;
            ++vi;
        }
        return true;
    }
    
    bool operator<(wstring v) {
        atomic_wstring_element* n = head;
        long vi = 0;
        long i = 0;
        long sz = v.size();
        if (!head->buffer[0] && sz)
            return true;
        
        while (vi < sz) {
            if (i == atomic_wstring_size) {
                n = n->next;
                if (n == NULL)
                    return true;
                i = 0;
            }
            
            if (n->buffer[i] < v[vi])
                return true;
            if (n->buffer[i] > v[vi])
                return false;
            
            ++i;
            ++vi;
        }
        return false;
    }
    
    bool operator>(wstring v) {
        atomic_wstring_element* n = head;
        long vi = 0;
        long i = 0;
        long sz = v.size();
        if (!head->buffer[0] && sz)
            return false;
        
        while (vi < sz) {
            if (i == atomic_wstring_size) {
                n = n->next;
                if (n == NULL)
                    return false;
                i = 0;
            }
            if (n->buffer[i] > v[vi])
                return true;
            if (n->buffer[i] < v[vi])
                return false;
            ++i;
            ++vi;
        }
        return false;
    }
    
    bool operator<=(wstring v) {
        atomic_wstring_element* n = head;
        long vi = 0;
        long i = 0;
        long sz = v.size();
        if (!head->buffer[0] && sz)
            return true;
        
        while (vi < sz) {
            if (i == atomic_wstring_size) {
                n = n->next;
                if (n == NULL)
                    return true;
                i = 0;
            }
            if (n->buffer[i] < v[vi])
                return true;
            if (n->buffer[i] > v[vi])
                return false;
            ++i;
            ++vi;
        }
        return true;
    }
    
    bool operator>=(wstring v) {
        atomic_wstring_element* n = head;
        long vi = 0;
        long i = 0;
        long sz = v.size();
        if (!head->buffer[0] && sz)
            return false;
        while (vi < sz) {
            if (i == atomic_wstring_size) {
                n = n->next;
                if (n == NULL)
                    return true;
                i = 0;
            }
            if (n->buffer[i] > v[vi])
                return true;
            if (n->buffer[i] < v[vi])
                return false;
            ++i;
            ++vi;
        }
        return true;
    }
    
   wstring value() {
        atomic_wstring_element* n = head;
        
        wstring v;
        while (n) {
            v += n->buffer;
            n = n->next;
        }
        
        return v;
    }
    
    bool isempty() {
        if (!head->buffer[0])
            return true;
        return false;
    }
    
    long size() {
        return head->size();
    }
    
};

//---------------------------------------------------------------------------------------------------------------------

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamguustring;
//This typedef defines a type "ustringMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguustring::*ustringMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamguustring : public TamguObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging basebin_hash<ustringMethod> methods;
	static Exchanging hmap<string, string> infomethods;
	static Exchanging basebin_hash<unsigned long> exported;

	static Exchanging short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	wstring value;

	//---------------------------------------------------------------------------------------------------------------------
	Tamguustring(wstring v, TamguGlobal* g, Tamgu* parent = NULL) : TamguObject(g, parent) {
        investigate |= is_pure_string;
		//Do not forget your variable initialisation
		value = v;
	}

	Tamguustring(wstring v) {
        investigate |= is_pure_string;
		//Do not forget your variable initialisation
		value = v;
	}

	Tamguustring(string v) {
     investigate |= is_string;
		//Do not forget your variable initialisation
		s_utf8_to_unicode(value, USTR(v), v.size());
	}

	Tamguustring(wchar_t c) {
     investigate |= is_string;
		//Do not forget your variable initialisation
		value = c;
	}
    
#ifdef WSTRING_IS_UTF16
    Tamguustring(uint32_t c) {
     investigate |= is_string;
        concat_char_check_utf16(value, c);
    }
#endif

	//----------------------------------------------------------------------------------------------------------------------
    void Putatomicvalue(Tamgu* v) {
        locking();
        v->Setstring(value, 0);
        unlocking();
    }
    
    void Setstring(wstring& v, short idthread) {
        locking();
        v = value;
        unlocking();
    }

    void Setstring(string& v, short idthread) {
        locking();
        sc_unicode_to_utf8(v, value);
        unlocking();
    }

	Tamgu* Put(Tamgu* index, Tamgu* v, short idthread);

	Tamgu* Putvalue(Tamgu* v, short idthread) {
        locking();
        v->Setstring(value, idthread);
        unlocking();
		return this;
	}

    Tamgu* EvalIndex(Tamgu* context, TamguIndex* idx, short idthread);
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);

	Tamgu* Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* env, TamguFunctionLambda* bd, short idthread);
	Tamgu* Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct);

	Tamgu* Vector(short idthread) {		
		string s = String();
		return globalTamgu->EvaluateVector(s, idthread);
	}

	Tamgu* Map(short idthread) {		
		string s = String();
		return globalTamgu->EvaluateMap(s, idthread);
	}

	short Type() {
        return a_ustring;
	}

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
		return "ustring";
	}

	

	bool isValueContainer() {
		return true;
	}

	bool isAtom() {
		return true;
	}

    bool checkAtomType(short ty) {
        return (ty == a_ustring);
    }

	

	bool isBoolean() {
		return false;
	}

	bool isFrame() {
		return false;
	}

	Tamgu* Atom(bool forced = false) {
        if (!globalTamgu->threadMODE) {
            if (forced || !protect || reference)
                return globalTamgu->Providewithustring(value);
            return this;
        }

        locking();
        Tamgu* r = this;
        if (forced || !protect || reference)
            r = globalTamgu->Providewithustring(value);
        unlocking();
        return r;
	}
    
    Tamgu* Atomref() {
        TamguReference* r = (TamguReference*)Atom();
        r->reference = 1;
        r->protect = false;
        return r;
    }
    

	string Bytes() {
		string buffer;
        if (!globalTamgu->threadMODE) {
            s_unicode_to_utf8(buffer, value);
            return buffer;
        }
        Locking _lock(this);
		s_unicode_to_utf8(buffer, value);
		return buffer;
	}

	void storevalue(float u) {
		
		wstring s = wconvertfromnumber(u);
        locking();
        value = s;
        unlocking();
	}

	void storevalue(short u) {
		
		wstring s = wconvertfromnumber(u);
        locking();
        value = s;
        unlocking();

	}

	void storevalue(long u) {
		
		wstring s = wconvertfromnumber(u);
        locking();
        value = s;
        unlocking();

	}

	void storevalue(BLONG u) {
		
		wstring s = wconvertfromnumber(u);
        locking();
        value = s;
        unlocking();

	}

	void storevalue(double u) {
		
		wstring s = wconvertfromnumber(u);
        locking();
        value = s;
        unlocking();

	}

    void Storevalue(string& u) {
        locking();
        sc_utf8_to_unicode(value, USTR(u), u.size());
        unlocking();
    }
    
    void Storevalue(wstring& u) {
        locking();
        value = u;
        unlocking();
    }
    
    void storevalue(string u) {
        locking();
        sc_utf8_to_unicode(value, USTR(u), u.size());
        unlocking();
    }
    
    void storevalue(wstring u) {
        locking();
        value = u;
        unlocking();
    }
    
	void storevalue(wchar_t u) {
        locking();
        value = u;
        unlocking();
	}

	void addustringto(wstring ws) {
        locking();
		value += ws;
        unlocking();
	}

	void addstringto(string s) {
        locking();
		s_utf8_to_unicode(value, USTR(s), s.size());
        unlocking();
	}

	void addstringto(wchar_t s) {
        locking();
		value += s;
        unlocking();
	}


	//---------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
	bool isDeclared(short n) {
		if (exported.find(n) != exported.end())
			return true;
		return false;
	}

    Tamgu* Newpureinstance(short idthread) {
        return new Tamguustring("");
    }
    
	Tamgu* Newinstance(short, Tamgu* f = NULL) {
		return globalTamgu->Provideustring(L"");
	}

	Tamgu* Newvalue(Tamgu* a, short idthread) {
		return globalTamgu->Provideustring(a->UString());
	}

	TamguIteration* Newiteration(bool direction);

	static void AddMethod(TamguGlobal* g, string name, ustringMethod func, unsigned long arity, string infos);
	
	static bool InitialisationModule(TamguGlobal* g, string version);
    Tamgu* Loopin(TamguInstruction* ins, Tamgu* context, short idthread);
    
	
    void Methods(Tamgu* v) {
        for (const auto& it : infomethods)
            v->storevalue(it.first);
    }

	string Info(string n) {
		if (infomethods.find(n) != infomethods.end())
			return infomethods[n];
		return "Unknown method";
	}

	Tamgu* Succ() {
        Locking _lock(this);
		if (value == L"")
			return globalTamgu->Provideustring(L"");

		wstring v = value;
		v[v.size() - 1] = v[v.size() - 1] + 1;
		return globalTamgu->Providewithustring(v);
	}

	Tamgu* Pred() {
        Locking _lock(this);
		if (value == L"")
			return globalTamgu->Provideustring(L"");

		wstring v = value;
		wchar_t c = v[v.size() - 1];
		if (c <= 1)
			return globalTamgu->Providewithustring(value);

		v[v.size() - 1] = c - 1;
		return globalTamgu->Providewithustring(v);
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	Tamgu* MethodSucc(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return Succ();
	}

	Tamgu* MethodPred(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return Pred();
	}

    Tamgu* MethodDoubleMetaphone(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodOrd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodUTF16(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodBytes(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodSizeb(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		Locking _lock(this);
		return globalTamgu->ProvideConstint((long)value.size());
	}
	Tamgu* MethodHash(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodReverse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodFormat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodBase(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodFill(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodPadding(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodParse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodPop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodParenthetic(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodTags(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodScan(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodEvaluate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodTohtml(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodToxml(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodEditdistance(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodReplace(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodMultiSplit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodSplit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodSplite(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodTokenize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodStokenize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodCount(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodBPE(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodBPEReplace(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	Tamgu* MethodFind(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodRfind(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodRemovefirst(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodRemovelast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodIsutf8(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return aTRUE;
	}
    Tamgu* MethodNgrams(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodSlice(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLatin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodDos(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodDostoutf8(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLeft(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodRight(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodMid(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodIsalpha(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodIsconsonant(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodIsvowel(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodIspunctuation(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodIsdigit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodExtract(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodEmoji(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodIsemoji(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodIsupper(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodIslower(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodUpper(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodDeaccentuate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLower(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodTrim(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodTrimleft(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodTrimright(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodInsert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodClear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodJamo(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodIsJamo(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodIsHangul(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodNormalizeHangul(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodTransliteration(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    Tamgu* MethodRead(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodWrite(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	//---------------------------------------------------------------------------------------------------------------------
	//ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
	//is returned by the Declaration method.
	Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
	}
	//---------------------------------------------------------------------------------------------------------------------
	void Clear() {
		Locking _lock(this);
		value = L"";
	}
	unsigned long EditDistance(Tamgu* e);
	
	
	string JSonString() {
		string res;
		string v = String();
		jstringing(res, v);
		return res;
	}
    
	string String() {
		string s;
        if (!globalTamgu->threadMODE) {
            s_unicode_to_utf8(s, value);
            return s;
        }
        
		Locking _lock(this);
		s_unicode_to_utf8(s, value);
		return s;
	}
	wstring UString() {
        if (!globalTamgu->threadMODE)
            return value;
		Locking _lock(this);
		return value;
	}

	wstring Getustring(short idthread) {
        if (!globalTamgu->threadMODE)
            return value;
        
		Locking _lock(this);
		return value;
	}

	string Getstring(short idthread) {
		string s;
        if (!globalTamgu->threadMODE) {
            s_unicode_to_utf8(s, value);
            return s;
        }
        
		Locking _lock(this);
		s_unicode_to_utf8(s, value);
		return s;
	}

	long Getinteger(short idthread) {
		return Integer();
	}

	double Getfloat(short idthread) {
		return Float();
	}

	float Getdecimal(short idthread) {
		return Decimal();
	}

	BLONG Getlong(short idthread) {
		return Long();
	}

	long Integer() {
        if (!globalTamgu->threadMODE) {
            if (value == L"")
                return 0;
            string v;
            sc_unicode_to_utf8(v, value);
            return conversionintegerhexa(STR(v));
        }

        Locking _lock(this);
		if (value == L"")
			return 0;
		string v;
		sc_unicode_to_utf8(v, value);
		return conversionintegerhexa(STR(v));
	}
	float Decimal() {
        if (!globalTamgu->threadMODE) {
            if (value == L"")
                return 0;
            string v;
            sc_unicode_to_utf8(v, value);
            return convertfloat(STR(v));
        }
        
		Locking _lock(this);
		if (value == L"")
			return 0;
		string v;
		sc_unicode_to_utf8(v, value);
		return convertfloat(STR(v));
	}
	double Float() {
        if (!globalTamgu->threadMODE) {
            if (value == L"")
                return 0;
            string v;
            sc_unicode_to_utf8(v, value);
            return convertfloat(STR(v));
        }

        Locking _lock(this);
		if (value == L"")
			return 0;
		string v;
		sc_unicode_to_utf8(v, value);
		return convertfloat(STR(v));
	}
    
	BLONG Long() {
		Locking _lock(this);
		return Integer();
	}
	bool Boolean() {
		Locking _lock(this);
		if (value == L"")
			return false;
		return true;
	}
	unsigned char Byte() {		
		string buffer;
        if (!globalTamgu->threadMODE) {
            s_unicode_to_utf8(buffer, value);
            return buffer[0];
        }

        Locking _lock(this);
		s_unicode_to_utf8(buffer, value);
		return buffer[0];
	}
    
	Tamgu* Push(Tamgu* a) {
        if (!globalTamgu->threadMODE) {
            value += a->UString();
            return this;
        }
        
        Locking _lock(this);
		value += a->UString();
		return this;
	}
	
#ifdef WSTRING_IS_UTF16
    void Pop(long i);
#else
	void Pop(long i) {
		Locking _lock(this);
        long sz = size_c(value);
        if (!sz)
            return;
        if (i == -1) {
            //the last character
            i = convertchartopos(value, 0, sz-1);
            while (i < value.size())
                value.pop_back();
            return;
        }
        
        if (i < 0 || i >= sz)
            return;
        
        long j = convertchartopos(value, 0, i);
        if (j == i) {
            value.erase(i,1);
            return;
        }
        
        i = j;
        scan_emoji(value, j);
        j++;
        value.erase(i, j-i);
	}
#endif
    
    virtual long Size() {
        if (!globalTamgu->threadMODE)
            return value.size();
        
        Locking _lock(this);
        return value.size();
    }

    
#ifdef WSTRING_IS_UTF16
	//Basic operations
    virtual long CommonSize() {
        if (!globalTamgu->threadMODE)
            return size_w(value);
        Locking _lock(this);
        return size_w(value);
    }
#else
	virtual long CommonSize() {
		if (!globalTamgu->threadMODE)
			return size_c(value);

		Locking _lock(this);
        return size_c(value);
	}
#endif

	Tamgu* in(Tamgu* context, Tamgu* a, short idthread);
	
	Tamgu* andset(Tamgu* a, bool autoself);
	Tamgu* xorset(Tamgu* a, bool autoself);

	Tamgu* Merging(Tamgu* a) {
        if (!globalTamgu->threadMODE) {
            value += a->UString();
            return this;
        }
        
		Locking _lock(this);
		value += a->UString();
		return this;
	}

    Tamgu* Combine(Tamgu* a) {
        locking();
        wstring v=a->UString();
        wstring k;
        long zs= minlocal(value.size(),v.size());
        for (long u=0;u<zs;u++) {
            k+=value[u];
            k+=v[u];
        }
        if (v.size()!=value.size()) {
            if (zs==v.size())
                k+=value.substr(zs,value.size()-zs);
            else
                k+=v.substr(zs,v.size()-zs);
        }
        value=k;
        unlocking();
        return this;
    }

	virtual Tamgu* plus(Tamgu* a, bool autoself) {
        locking();

		if (autoself) {
			value += a->UString();
            unlocking();
			return this;
		}
		a = globalTamgu->Provideustring(value + a->UString());
        unlocking();
        return a;
	}

	virtual Tamgu* minus(Tamgu* a, bool itself) {
        locking();
		wstring v = a->UString();
		size_t nb = v.size();
		size_t pos = value.find(v);
        if (pos == string::npos) {
            unlocking();
			return this;
        }

		//we keep our string up to p
		v = value.substr(0, pos);
		//then we skip the nb characters matching the size of v
		pos += nb;
		//then we concatenate with the rest of the string
		v += value.substr(pos, value.size() - pos);
		if (itself) {
			value = v;
            unlocking();
			return this;
		}
        unlocking();
		return globalTamgu->Providewithustring(v);
	}

    Tamgu* less(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aFALSE;
#endif

        wstring v = a->UString();

        if (!globalTamgu->threadMODE) {
            if (value < v)
                return aTRUE;
            return aFALSE;
        }
        Locking _lock(this);
        if (value < v)
            return aTRUE;
        return aFALSE;
    }
    
    Tamgu* more(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aFALSE;
#endif
        wstring v = a->UString();
        if (!globalTamgu->threadMODE) {
            if (value > v)
                return aTRUE;
            return aFALSE;
        }
        Locking _lock(this);
        if (value > v)
            return aTRUE;
        return aFALSE;
    }
    
    Tamgu* same(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aFALSE;
#endif
        wstring v = a->UString();
        if (!globalTamgu->threadMODE) {
            if (value == v)
                return aTRUE;
            return aFALSE;
        }
        Locking _lock(this);
        if (value == v)
            return aTRUE;
        return aFALSE;
    }
    
    Tamgu* different(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aTRUE;
#endif
        wstring v = a->UString();
        if (!globalTamgu->threadMODE) {
            if (value != v)
                return aTRUE;
            return aFALSE;
        }
        Locking _lock(this);
        if (value != v)
            return aTRUE;
        return aFALSE;
    }
    
    Tamgu* lessequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aFALSE;
#endif
        wstring v = a->UString();
        if (!globalTamgu->threadMODE) {
            if (value <= v)
                return aTRUE;
            return aFALSE;
        }
        Locking _lock(this);
        if (value <= v)
            return aTRUE;
        return aFALSE;
    }
    
    Tamgu* moreequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aFALSE;
#endif
        wstring v = a->UString();
        if (!globalTamgu->threadMODE) {
            if (value >= v)
                return aTRUE;
            return aFALSE;
        }
        
        Locking _lock(this);
        if (value >= v)
            return aTRUE;
        return aFALSE;
    }
};

class Tamguustringbuff : public Tamguustring {
public:
	long idx;
	bool used;

	Tamguustringbuff(long i) : Tamguustring(L"") {
		//Do not forget your variable initialisation
		idx = i;
		used = false;
	}

    bool Candelete() {
        return false;
    }

	void Resetreference(short r) {
        r = reference - r;
        if (r <= 0) {
            reference.store(0);
            if (!protect) {
                protect = true;
                
                used = false;
                if (!globalTamgu->threadMODE)
                    globalTamgu->uempties.push_back(idx);
            }
        }
        else
            reference.store(r);
	}

};

class TamguIterationustring : public TamguIteration {
public:

	BLONG itx;
	wstring ref;
	wstring current;

	TamguIterationustring(wstring& v, bool d, TamguGlobal* g = NULL) : TamguIteration(d, g) {
		ref = v;
		itx = 0;
	}

	void getchar() {
		current = ref[itx];
#ifdef WSTRING_IS_UTF16
		if (!checklargeutf16(current[0]))
			return;
		if (reverse)
			current += ref[--itx];
		else
			current += ref[++itx];
#endif
	}

	Tamgu* Key() {
		return globalTamgu->ProvideConstint(itx);
	}

	Tamgu* Value() {
		return globalTamgu->Providewithustring(current);
	}

	string Keystring() {
		return convertfromnumber(itx);
	}

	wstring Valueustring() {
		return current;
	}

	string Valuestring() {
		string s;
		s_unicode_to_utf8(s, current);
		return s;
	}

	long Keyinteger() {
		return itx;
	}

	long Valueinteger() {
		return conversionintegerhexa(current);
	}

	double Keyfloat() {
		return itx;
	}

	double Valuefloat() {
		return convertfloat(current);
	}

	void Next() {
		if (reverse)
			itx--;
		else
			itx++;
	}

	Tamgu* End() {
		if (reverse) {
			if (itx < 0) {
				current = L"";
				return aTRUE;
			}
		}
		else {
			if (itx >= ref.size()) {
				current = L"";
				return aTRUE;
			}
		}
		getchar();
		return aFALSE;
	}

	Tamgu* Begin() {
		if (reverse) {
			itx = ref.size() - 1;
		}
		else
			itx = 0;
		End();
		return aTRUE;
	}


};

    //---------------------------------------------------------------------------------------------------------------------

    //We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
    //of how to declare a new method.
class Tamgua_ustring;
    //This typedef defines a type "a_ustringMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgua_ustring::*a_ustringMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    //---------------------------------------------------------------------------------------------------------------------

class Tamgua_ustring : public TamguReference {
public:
        //We export the methods that will be exposed for our new object
        //this is a static object, which is common to everyone
        //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<a_ustringMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging basebin_hash<unsigned long> exported;
    
    static Exchanging short idtype;
    
        //---------------------------------------------------------------------------------------------------------------------
        //This SECTION is for your specific implementation...
        //Your personal variables here...
    atomic_wstring value;

    
        //---------------------------------------------------------------------------------------------------------------------
    Tamgua_ustring(wstring v, TamguGlobal* g, Tamgu* parent = NULL) : TamguReference(g, parent) {
        investigate |= is_string;
            //Do not forget your variable initialisation
        value = v;
    }
    
    Tamgua_ustring(wstring v) {
        investigate |= is_string;
            //Do not forget your variable initialisation
        value = v;
    }
    
    Tamgua_ustring(atomic_wstring& v) {
        investigate |= is_string;
            //Do not forget your variable initialisation
        value = v;
    }
    
    Tamgua_ustring(string v) {
        investigate |= is_string;
            //Do not forget your variable initialisation
        wstring w;
        s_utf8_to_unicode(w, USTR(v), v.size());
        value = w;
    }
    
    Tamgua_ustring(wchar_t v) {
        investigate |= is_string;
        //Do not forget your variable initialisation
        value.head->buffer[0] = v;
        value.head->buffer[1] = 0;
    }
    
    
        //----------------------------------------------------------------------------------------------------------------------
    void Putatomicvalue(Tamgu* v) {
        value = v->UString();
    }
    
    
    Tamgu* Put(Tamgu* index, Tamgu* v, short idthread);
    
    Tamgu* Putvalue(Tamgu* v, short idthread) {
        value = v->UString();
        return this;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    
    Tamgu* Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* env, TamguFunctionLambda* bd, short idthread);
    Tamgu* Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct);
    
    Tamgu* Vector(short idthread) {
        string s = String();
        return globalTamgu->EvaluateVector(s, idthread);
    }
    
    Tamgu* Map(short idthread) {
        string s = String();
        return globalTamgu->EvaluateMap(s, idthread);
    }
    
    short Type() {
		return Tamgua_ustring::idtype;
    }
    
	void Setidtype(TamguGlobal* global) {
		Tamgua_ustring::InitialisationModule(global, "");
	}


    string Typename() {
        return "a_ustring";
    }
    
    
    
    bool isValueContainer() {
        return true;
    }
    
    bool isAtom() {
        return true;
    }
    
    bool checkAtomType(short ty) {
        return (ty == a_ustring);
    }

    
    
    bool isBoolean() {
        return false;
    }
    
    bool isFrame() {
        return false;
    }
    
    Tamgu* Atom(bool forced = false) {
        if (forced || !protect || reference)
            return new Tamgua_ustring(value);
        
        return this;
    }
    
    Tamgu* Atomref() {
        TamguReference* r;
        if (!protect || reference)
            r = new Tamgua_ustring(value);
        else
            r = this;
        r->reference = 1;
        r->protect = false;
        return r;
    }    
    
    string Bytes() {
        string buffer;
        wstring w = value.value();
        s_unicode_to_utf8(buffer, w);
        return buffer;
    }
    
    void storevalue(float u) {
        value = wconvertfromnumber(u);
    }
    
    void storevalue(short u) {
        value = wconvertfromnumber(u);
    }
    
    void storevalue(long u) {
        value = wconvertfromnumber(u);
    }
    
    void storevalue(BLONG u) {
        value = wconvertfromnumber(u);
    }
    
    void storevalue(double u) {
        value = wconvertfromnumber(u);
    }
    
    void Storevalue(string& u) {
        wstring w;
        sc_utf8_to_unicode(w, USTR(u), u.size());
        value =  w;
    }
    
    void Storevalue(wstring& u) {
        value = u;
    }
    
    void storevalue(string u) {
        wstring w;
        sc_utf8_to_unicode(w, USTR(u), u.size());
        value = w;
    }
    
    void storevalue(wstring u) {
        value = u;
    }
    
    void storevalue(wchar_t u) {
        wstring w;
        w = u;
        value = w;
    }
    
    void adda_ustringto(wstring ws) {
        value.concat(ws);
    }
    
    void addstringto(string s) {
        wstring w;
        s_utf8_to_unicode(w, USTR(s), s.size());
        value =  w;
    }
    
    void addstringto(wchar_t s) {
        wstring w;
        w = s;
        value.concat(w);
    }
    
    
        //---------------------------------------------------------------------------------------------------------------------
        //Declaration
        //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        if (exported.find(n) != exported.end())
            return true;
        return false;
    }
    
    Tamgu* Newinstance(short, Tamgu* f = NULL) {
        return new Tamgua_ustring(L"");
    }
    
    Tamgu* Newvalue(Tamgu* a, short idthread) {
        return new Tamgua_ustring(a->UString());
    }
    
    TamguIteration* Newiteration(bool direction);
    
    static void AddMethod(TamguGlobal* g, string name, a_ustringMethod func, unsigned long arity, string infos);
    
    static bool InitialisationModule(TamguGlobal* g, string version);
    Tamgu* Loopin(TamguInstruction* ins, Tamgu* context, short idthread);
    
    
    void Methods(Tamgu* v) {
        for (const auto& it : infomethods)
            v->storevalue(it.first);
    }

    string Info(string n) {
        if (infomethods.find(n) != infomethods.end())
            return infomethods[n];
        return "Unknown method";
    }
    
    Tamgu* Succ() {
        if (value.isempty())
            return new Tamgua_ustring(L"");
        
        wstring v = value.value();
        v[v.size() - 1] = v[v.size() - 1] + 1;
        return new Tamgua_ustring(v);
    }
    
    Tamgu* Pred() {
        if (value.isempty())
            return globalTamgu->Provideustring(L"");
        
        wstring v = value.value();
        wchar_t c = v[v.size() - 1];
        if (c <= 1)
            return new Tamgua_ustring(value);
        
        v[v.size() - 1] = c - 1;
        return new Tamgua_ustring(v);
    }
    
        //---------------------------------------------------------------------------------------------------------------------
        //This SECTION is for your specific implementation...
        //This is an example of a function that could be implemented for your needs.
    Tamgu* MethodSucc(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Succ();
    }
    
    Tamgu* MethodPred(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Pred();
    }
    
    Tamgu* MethodDoubleMetaphone(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodOrd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodBytes(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodSizeb(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return globalTamgu->ProvideConstint((long)value.size());
    }
    
    Tamgu* MethodHash(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodReverse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodFormat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodBase(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodFill(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodPadding(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodParse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodPop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodParenthetic(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodTags(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodScan(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodEvaluate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodEditdistance(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodReplace(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodMultiSplit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodSplit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodSplite(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodTokenize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodStokenize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodCount(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodFind(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodRfind(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodRemovefirst(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodRemovelast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodIsutf8(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return aTRUE;
    }
    Tamgu* MethodNgrams(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodSlice(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodLatin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodDos(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodDostoutf8(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodLeft(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodRight(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodMid(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodIsalpha(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodIsconsonant(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodIsvowel(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodIspunctuation(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodIsdigit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodExtract(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodEmoji(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodIsemoji(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodIsupper(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodIslower(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodUpper(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodDeaccentuate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodLower(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodTrim(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodTrimleft(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodTrimright(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodLast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodInsert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodClear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodJamo(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodIsJamo(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodIsHangul(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodNormalizeHangul(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodTransliteration(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    
        //---------------------------------------------------------------------------------------------------------------------
        //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
        //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
            //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
            //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }
        //---------------------------------------------------------------------------------------------------------------------
    void Clear() {
        value.clear();
    }
    unsigned long EditDistance(Tamgu* e);
    
    
    string JSonString() {
        string res;
        string v = String();
        jstringing(res, v);
        return res;
    }
    
    string String() {
        string s;
        wstring w = value.value();
        s_unicode_to_utf8(s, w);
        return s;
    }
    wstring UString() {
        return value.value();
    }
    
    wstring Getustring(short idthread) {
        return value.value();
    }
    
    string Getstring(short idthread) {
        string s;
        wstring w = value.value();
        s_unicode_to_utf8(s, w);
        return s;
    }
    
    long Getinteger(short idthread) {
        return Integer();
    }
    
    double Getfloat(short idthread) {
        return Float();
    }
    
    float Getdecimal(short idthread) {
        return Decimal();
    }
    
    BLONG Getlong(short idthread) {
        return Long();
    }
    
    long Integer() {
        if (value.isempty())
            return 0;
        string v;
        wstring w = value.value();
        sc_unicode_to_utf8(v, w);
        return conversionintegerhexa(STR(v));
    }
    float Decimal() {
        if (value.isempty())
            return 0;
        string v;
        wstring w = value.value();
        sc_unicode_to_utf8(v, w);
        return convertfloat(STR(v));
    }
    double Float() {
        if (value.isempty())
            return 0;
        string v;
        wstring w = value.value();
        sc_unicode_to_utf8(v, w);
        return convertfloat(STR(v));
    }
    
    BLONG Long() {
        return Integer();
    }
    bool Boolean() {
        return !value.isempty();
    }
    unsigned char Byte() {
        string buffer;
        wstring w = value.value();
        s_unicode_to_utf8(buffer, w);
        return buffer[0];
    }
    
    Tamgu* Push(Tamgu* a) {
        value.concat(a->UString());
        return this;
    }
    
#ifdef WSTRING_IS_UTF16
    void Pop(long i);
#else
    void Pop(long i) {
        if (value.isempty())
            return;
        wstring w = value.value();
        long sz = size_c(w);
        if (i == -1) {
                //the last character
            i = convertchartopos(w, 0, sz-1);
            while (i < w.size())
                w.pop_back();
            value = w;
            return;
        }
        
        if (i < 0 || i >= sz)
            return;
        
        long j = convertchartopos(w, 0, i);
        if (j == i) {
            w.erase(i,1);
            value = w;
            return;
        }
        i = j;
        scan_emoji(w, j);
        j++;
        w.erase(i, j-i);
        value = w;
    }
#endif
    
    //Basic operations
    long Size() {
        return (long)value.size();
    }
    
#ifdef WSTRING_IS_UTF16
    long CommonSize() {
        wstring w = value.value();
        return size_w(w);
    }
#else
    long CommonSize() {
        return (long)value.size();
    }
#endif
    
    Tamgu* in(Tamgu* context, Tamgu* a, short idthread);
    
    Tamgu* andset(Tamgu* a, bool autoself);
    Tamgu* xorset(Tamgu* a, bool autoself);
    
    Tamgu* Merging(Tamgu* a) {
        value.concat(a->UString());
        return this;
    }
    
    Tamgu* Combine(Tamgu* a) {
        wstring w = value.value();
        wstring v=a->UString();
        wstring k;
        long zs= minlocal(w.size(),v.size());
        for (long u=0;u<zs;u++) {
            k+=w[u];
            k+=v[u];
        }
        if (v.size()!=value.size()) {
            if (zs==v.size())
                k+=w.substr(zs,w.size()-zs);
            else
                k+=v.substr(zs,v.size()-zs);
        }
        value = k;
        return this;
    }
    
    virtual Tamgu* plus(Tamgu* a, bool autoself) {
        if (autoself) {
            value.concat(a->UString());
            return this;
        }
        
        Tamgua_ustring* n = new Tamgua_ustring(value);
        n->value.concat(a->UString());
        return n;
    }
    
    virtual Tamgu* minus(Tamgu* a, bool itself) {
        wstring vl = value.value();
        wstring v = a->UString();
        size_t nb = v.size();
        size_t pos = vl.find(v);
        if (pos == string::npos) {
            return this;
        }
        
            //we keep our string up to p
        v = vl.substr(0, pos);
            //then we skip the nb characters matching the size of v
        pos += nb;
            //then we concatenate with the rest of the string
        v += vl.substr(pos, vl.size() - pos);
        if (itself) {
            value = v;
            return this;
        }
        return new Tamgua_ustring(v);
    }
    
    Tamgu* less(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aFALSE;
#endif
        if (value < a->UString())
            return aTRUE;
        return aFALSE;
    }
    
    Tamgu* more(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aFALSE;
#endif
        if (value > a->UString())
            return aTRUE;
        return aFALSE;
    }
    
    Tamgu* same(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aFALSE;
#endif
        if (value == a->UString())
            return aTRUE;
        return aFALSE;
    }
    
    Tamgu* different(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aTRUE;
#endif
        if (value == a->UString())
            return aFALSE;
        return aTRUE;
    }
    
    Tamgu* lessequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aFALSE;
#endif
        if (value <= a->UString())
            return aTRUE;
        return aFALSE;
    }
    
    Tamgu* moreequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aFALSE;
#endif
        if (value >= a->UString())
            return aTRUE;
        return aFALSE;
    }
};


class TamguLoopUString : public Tamguustring {
public:
    
    vector<wstring> interval;
    long position;
    Tamgu* function;

    TamguLoopUString(TamguGlobal* g) : Tamguustring(L"") {
        
        position = 0;
        function = NULL;
    }

    bool isLoop() {
        return true;
    }
    
    
    void Callfunction();

    unsigned long CallBackArity() {
        return P_TWO;
    }

    void Addfunction(Tamgu* f) {
        function = f;
    }

    short Type() {
        return a_uloop;
    }

    Tamgu* Put(Tamgu*, Tamgu*, short);
    Tamgu* Vector(short idthread);
    Tamgu* Putvalue(Tamgu* ke, short idthread) {
        return Put(aNULL, ke, idthread);
    }

    long Size() {
        return interval.size();
    }

    long CommonSize() {
        return interval.size();
    }
    
    void Next() {
        if (interval.size() == 0)
            return;

        position++;
        if (position >= interval.size()) {
            if (function != NULL)
                Callfunction();
            position = 0;
        }
        value = interval[position];
    }

    Tamgu* plusplus() {
        if (interval.size() == 0)
            return this;
        position++;
        if (position >= interval.size()) {
            if (function != NULL)
                Callfunction();
            position = 0;
        }
        value = interval[position];
        return this;
    }

    Tamgu* minusminus() {
        if (interval.size() == 0)
            return this;
        position--;
        if (position < 0) {
            if (function != NULL)
                Callfunction();
            position = interval.size() - 1;
        }
        value = interval[position];
        return this;
    }

    Tamgu* Atom(bool forced) {
        return this;
    }

    bool duplicateForCall() {
        return false;
    }

    Tamgu* Newinstance(short, Tamgu* f = NULL) {
        TamguLoopUString* a = new TamguLoopUString(globalTamgu);
        a->function = f;
        return a;
    }

    Tamgu* andset(Tamgu* a, bool autoself);
    Tamgu* xorset(Tamgu* a, bool autoself);

    Tamgu* plus(Tamgu* b, bool autoself) {
        if (interval.size() == 0)
            return this;

        if (autoself) {
            position += b->Integer();

            position = abs(position) % interval.size();

            value = interval[position];
            return this;
        }

        return Tamguustring::plus(b, autoself);
    }

    Tamgu* minus(Tamgu* b, bool autoself) {
        if (interval.size() == 0)
            return this;

        if (autoself) {
            position -= b->Integer();

            position = (interval.size() - abs(position)) % interval.size();

            value = interval[position];
            return this;
        }

        return Tamguustring::minus(b, autoself);
    }
};


#endif









