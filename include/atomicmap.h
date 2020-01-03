/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : atomicmap.h
 Date       : 2017/09/01
 Purpose    : Implementation of a map sans collision 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#ifndef i_atomic_map
#define i_atomic_map

#include <locale>
static std::locale atomicloc;                 // the "C" locale

#ifdef INTELINTRINSICS
#ifdef WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif
#endif

//We will use an array of basearraysize elements
//The mask will correspond to the number of bits used to find a slot in the array.
//For instance, 256 is 8 bits, which corresponds to a mask of 0xFF
//The number of bits that will be removed from the key will be then 8 also.
//For a basearraysize of 512, the mask will be: 0x1FF and the number of bits to remove: 9...
const unsigned long removebits = 7; //the key will be based on 7 bits
const unsigned long basearraysize = 1 << removebits; //2^^removebits == basearraysize
const unsigned long atomicmask = basearraysize - 1; //to extract the key...

const unsigned long elementbits = 6; //We will divide each key by 64 ( <=>  e >> 6)
const unsigned long elementsize = 1 << elementbits;//this is the number of elements (keys/values) in each atomic_element. position is the rest of dividing by 64

const BULONG theONE = 1;

const BULONG ulongval64[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768,
    binONE << 16, binONE << 17, binONE << 18, binONE << 19, binONE << 20, binONE << 21, binONE << 22, binONE << 23, binONE << 24,
    binONE << 25, binONE << 26, binONE << 27, binONE << 28,
    binONE << 29, binONE << 30, binONE << 31, binONE << 32, binONE << 33, binONE << 34, binONE << 35, binONE << 36, binONE << 37,
    binONE << 38, binONE << 39, binONE << 40, binONE << 41,
    binONE << 42, binONE << 43, binONE << 44, binONE << 45, binONE << 46, binONE << 47, binONE << 48, binONE << 49, binONE << 50,
    binONE << 51, binONE << 52, binONE << 53, binONE << 54,
    binONE << 55, binONE << 56, binONE << 57, binONE << 58, binONE << 59, binONE << 60, binONE << 61, binONE << 62, binONE << 63 };

union atomic_float {
    float f;
    unsigned long l;
    
    atomic_float(float e) {
        f = e;
    }
};

union atomic_double {
    double f;
    BULONG l;
    
    atomic_double(double e) {
        f = e;
    }
};

template <class K,class Z> class atomic_element {
public:
    
    K keys[elementsize];
    Z values[elementsize];
    std::recursive_mutex* _lock;

    atomic_element<K,Z>* next;

    BULONG indexes;
    BULONG index;
    std::atomic<bool> first;

    atomic_element() {
        _lock = new std::recursive_mutex;
        index = 0;
        indexes = 0;
        next = NULL;
        first = true;
    }
    

    atomic_element(BULONG k) {
        _lock = NULL;
        index = k;
        indexes = 0;
        next = NULL;
        first = false;
    }

    void clear(bool top) {
        indexes = 0;
        index = 0;
        first = top;
        if (next != NULL)
            next->clear(false);
    }
    
    bool find(BULONG k, uchar r) {
        if (first)
            return false;
        
        if (index == k) {
            if (indexes & ulongval64[r])
                return true;
            return false;
        }
        
        atomic_element<K,Z>* n =  next;
        while (n != NULL) {
            if (n->index == k) {
                if (n->indexes & ulongval64[r])
                    return true;
                return false;
            }

            if (k < n->index)
                return false;
            
            n = n->next;
        }
        return false;
    }

    atomic_element<K,Z>* search(K& v, BULONG k, uchar r) {
        if (first)
            return NULL;

        if (index == k) {
            if ((indexes & ulongval64[r]) && keys[r] == v)
                return this;
            return NULL;
        }
    
        atomic_element<K,Z>* n =  next;
        while (n != NULL) {
            if (n->index == k) {
                if ((n->indexes & ulongval64[r]) && n->keys[r] == v)
                    return n;
                return NULL;
            }

            if (k < n->index)
                return NULL;
            
            n = n->next;
        }
        return NULL;
    }

    //The get should always be called after a find...
    void get(BULONG k, uchar r, Z& v) {
        if (index == k) {
            if (indexes & ulongval64[r])
                v = values[r];
            return;
        }
        
        atomic_element<K,Z>* n =  next;
        while (n != NULL) {
            if (k < n->index)
                break;
            
            if (n->index == k) {
                if (n->indexes & ulongval64[r])
                    v = n->values[r];
                return;
            }
            n = n->next;
        }
    }

    Z getpointer(BULONG k, uchar r) {
        if (index == k) {
            if (indexes & ulongval64[r])
                return values[r];
        }
        
        atomic_element<K,Z>* n =  next;
        while (n != NULL) {
            if (k < n->index)
                break;
            
            if (n->index == k) {
                if (n->indexes & ulongval64[r])
                    return n->values[r];
            }
            n = n->next;
        }
        return NULL;
    }

    void reset(BULONG k, uchar r) {
        if (first)
            return;

        if (index == k) {
            indexes &= ~ulongval64[r];
            return;
        }
        
        atomic_element<K,Z>* n =  next;
        while (n != NULL) {
            if (n->index == k) {
                n->indexes &= ~ulongval64[r];
                return;
            }

            if (k < n->index)
                return;
            
            n = n->next;
        }
    }

#ifdef INTELINTRINSICS
    short sizeone() {
        return bitcounter(indexes);
    }
#else
    short sizeone() {
        short nb = 0;
        int qj;
        BULONG filter = indexes;
        
        while (filter) {
            if (!(filter & 1)) {
                while (!(filter & 65535))
                    filter >>= 16;
                while (!(filter & 255))
                    filter >>= 8;
                while (!(filter & 15))
                    filter >>= 4;
                while (!(filter & 1))
                    filter >>= 1;
            }
            nb++;
            filter >>= 1;
        }
        return nb;
    }
#endif

    long size() {
        if (first)
            return 0;
        
        long nb = sizeone();
        if (next != NULL)
            nb += next->size();
        return nb;
    }
    
    void duplicate(atomic_element<K,Z>* e) {
        if (e->first)
            return;

        first = false;
        index = e->index;
        indexes  = e->indexes;
        for (short i = 0; i < elementsize; i++) {
            values[i] = e->values[i];
            keys[i] = e->keys[i];
        }
        
        if (e->next != NULL) {
            if (next == NULL)
                next = new atomic_element<K,Z>;
            next->duplicate(e->next);
        }
    }
    
    //The initial atomic_map is an array of atomic_element pointers that are created when the atomic_map is created
    //However, they are empty initialy, hence the "first" flag. Elements are sorted out by key values.
    void set(K& ky, BULONG k, uchar r, Z& val) {
        if (first) {
            _lock->lock();
            if (first) {
                index = k;
                indexes |= ulongval64[r];
                keys[r] = ky;
                values[r] = val;
                first = false;
                _lock->unlock();
                return;
            }
            _lock->unlock();
        }
        
        if (index == k) {
            indexes |= ulongval64[r];
            keys[r] = ky;
            values[r] = val;
            return;
        }
        
        atomic_element<K,Z>* prev = this;
        atomic_element<K,Z>* n =  next;
        while (n != NULL) {
            if (n->index == k) {
                n->indexes |= ulongval64[r];
                n->keys[r] = ky;
                n->values[r] = val;
                return;
            }
            
            //we sort them out now... No need to go further...
            if (k < n->index)
                break;

            prev = n;
            n = n->next;
        }
        
        //This is a critical section when we create a new pointer...
        //This is the only moment when we need protection...
        _lock->lock();
        //We need to check again... Weird but necessary, to avoid dangling pointers...
        //Basically, the risk is that two or more threads will try to access the same structure
        //We can accept only one new element to be added at a time in the chain...
        //Because of the lock, a new element with the same index might already have been added
        //when the lock is through again. Hence, the loop again.
        n = next;
        prev = this;
        while (n != NULL) {
            if (n->index == k) {
                n->indexes |= ulongval64[r];
                n->keys[r] = ky;
                n->values[r] = val;
                _lock->unlock();
                return;
            }
            
            if (k < n->index)
                break;
            
            prev = n;
            n = n->next;
        }
        
        //We can now safely create a new element...
        atomic_element<K,Z>* e = new atomic_element<K,Z>(k);
        e->indexes |= ulongval64[r];
        e->keys[r] = ky;
        e->values[r] = val;
        e->next = n;
        prev->next = e;
        _lock->unlock();
    }

    void setpointer(K& ky, BULONG k, uchar r, Z val) {
        if (first) {
            _lock->lock();
            if (first) {
                index = k;
                indexes |= ulongval64[r];
                keys[r] = ky;
                values[r] = val;
                first = false;
                _lock->unlock();
                return;
            }
            _lock->unlock();
        }

        if (index == k) {
            indexes |= ulongval64[r];
            keys[r] = ky;
            values[r] = val;
            return;
        }
        
        atomic_element<K,Z>* prev = this;
        atomic_element<K,Z>* n =  next;
        while (n != NULL) {
            if (n->index == k) {
                n->indexes |= ulongval64[r];
                n->keys[r] = ky;
                n->values[r] = val;
                return;
            }
            
                //we sort them out now... No need to go further...
            if (k < n->index)
                break;
            
            prev = n;
            n = n->next;
        }
        
        _lock->lock();
        //We need to check again... Weird but necessary, to avoid dangling pointers...
        //Basically, the risk is that two or more threads will try to access the same structure
        //We can accept only one new element to be added at a time in the chain...
        //Because of the lock, a new element with the same index might already have been added
        //when the lock is through again. Hence, the loop again.
        n = next;
        prev = this;
        while (n != NULL) {
            if (n->index == k) {
                n->indexes |= ulongval64[r];
                n->keys[r] = ky;
                n->values[r] = val;
                _lock->unlock();
                return;
            }
            
            if (k < n->index)
                break;
            
            prev = n;
            n = n->next;
        }

        atomic_element<K,Z>* e = new atomic_element<K,Z>(k);
        e->indexes |= ulongval64[r];
        e->keys[r] = ky;
        e->values[r] = val;
        e->next = n;
        prev->next = e;
        _lock->unlock();
    }


    ~atomic_element() {
        if (next != NULL)
            delete next;
    }
};

template <class K, class Z> class atomic_map {
public:
    std::atomic<atomic_element<K,Z>* > atomicarray[basearraysize];

    atomic_map() {
        for (int i = 0; i < basearraysize; i++)
            atomicarray[i] = new atomic_element<K, Z>();
    }
    
    ~atomic_map() {
        for (int i = 0; i < basearraysize; i++) {
            atomic_element<K,Z>* e = atomicarray[i];
            delete e;
        }
    }
    //A bit of explanation...
    //We use the last "removebits" bits to define which slot in the array will be chosen
    //Each atomic_element contains elementsize elements, the position within is computed with
    //the rest of a division by elementsize (hence elementbits == 6)
    //k is then used as a key, which will identify each chunk of 64 elements...

    BULONG gethash(short v) {
        return v;
    }
    
    BULONG gethash(BLONG v) {
        return v;
    }

    BULONG gethash(long v) {
        return v;
    }
    
    BULONG gethash(float v) {
        atomic_float ad(v);
        return ad.l;
    }
    
    BULONG gethash(double v) {
        atomic_double ad(v);
        return ad.l;
    }

    BULONG gethash(string& s) {
        return std::use_facet<std::collate<char> >(atomicloc).hash(s.data(), s.data() + s.length());
    }
    
    BULONG gethash(wstring& s) {
        return std::use_facet<std::collate<wchar_t> >(atomicloc).hash(s.data(), s.data() + s.length());
    }
    
    BULONG gethash(std::thread::id& h) {
        return std::hash<std::thread::id>()(h);
    }
    
    Z getpointer(K ky) {
        BULONG idx = gethash(ky);
        short kk = idx & atomicmask;
        idx >>= removebits;
        BULONG k = idx >> elementbits;
        uchar r = idx - (k << elementbits);
        return (*atomicarray[kk]).getpointer(k, r);
    }
    
    void get(K ky, Z& vl) {
        BULONG idx = gethash(ky);
        short kk = idx & atomicmask;
        idx >>= removebits;
        BULONG k = idx >> elementbits;
        uchar r = idx - (k << elementbits);
        (*atomicarray[kk]).get(k, r, vl);
    }
    
    bool check(K v) {
        BULONG idx = gethash(v);
        short kk = idx & atomicmask;
        idx >>= removebits;
        BULONG k = idx >> elementbits;
        uchar r = idx - (k << elementbits);
        return (*atomicarray[kk]).find(k, r);
    }
    
    void set(K ky, Z val) {
        BULONG idx = gethash(ky);
        short kk = idx & atomicmask;
        idx >>= removebits;
        BULONG k = idx >> elementbits;
        uchar r = idx - (k << elementbits);
        return (*atomicarray[kk]).set(ky, k, r, val);
    }

    void setpointer(K ky, Z val) {
        BULONG idx = gethash(ky);
        short kk = idx & atomicmask;
        idx >>= removebits;
        BULONG k = idx >> elementbits;
        uchar r = idx - (k << elementbits);
        return (*atomicarray[kk]).setpointer(ky, k, r, val);
    }
    

    atomic_element<K,Z>* search(K v, short& kk, BULONG& k, uchar& r) {
        BULONG idx = gethash(v);
        kk = idx & atomicmask;
        idx >>= removebits;
        k = idx >> elementbits;
        r = idx - (k << elementbits);
        return (*atomicarray[kk]).search(v, k, r);
    }
    
    void erase(K v) {
        BULONG idx = gethash(v);
        short kk = idx & atomicmask;
        idx >>= removebits;
        BULONG k = idx >> elementbits;
        uchar r = idx - (k << elementbits);
        (*atomicarray[kk]).reset(k, r);
    }

    void ids(K ky, short& kk, BULONG& k, uchar& r) {
        BULONG idx = gethash(ky);
        kk = idx & atomicmask;
        idx >>= removebits;
        k = idx >> elementbits;
        r = idx - (k << elementbits);
    }
    
    size_t size() {
        long sz = 0;
        for (int i = 0; i < basearraysize; i++)
            sz += (*atomicarray[i]).size();
        return sz;
    }

    void checkconsistence(vector<long>& counter) {
        long nb = 0;
        for (int i = 0; i < basearraysize; i++) {
            atomic_element<K,Z>* e = atomicarray[i];
            if (e->first) {
                counter.push_back(0);
                continue;
            }
            
            nb = 0;
            while (e != NULL) {
                e = e->next;
                ++nb;
            }
            counter.push_back(nb);
        }
    }
    
    void compact() {
        for (int i = 0; i < basearraysize; i++) {
            atomic_element<K,Z>* e = (*atomicarray[i]).next;
            atomic_element<K,Z>* p = atomicarray[i];
            
            while (e != NULL) {
                if (!e->indexes) {
                    p->next = e->next;
                    delete e;
                    e = p->next;
                }
                else {
                    p = e;
                    e = e->next;
                }
            }
        }
    }

    void Begin(atomic_element<K,Z>** e) {
        *e = atomicarray[0];
    }
    
    bool End(atomic_element<K,Z>* e) {
        if (e == NULL)
            return true;
        return false;
    }
    
    bool iterate(atomic_element<K,Z>** ele, K& key, Z& val, short& indexarray, uchar& idx, BULONG& filter) {
        atomic_element<K,Z>* e = *ele;
        int qj;
        
        while (indexarray < basearraysize) {
            while (e) {
                if (idx == 255) {
                    filter = e->indexes;
                    idx = 0;
                }
                while (filter) {
#ifdef INTELINTRINSICS
                    if (!(filter & 1)) {
                        if (!(filter & 0x00000000FFFFFFFF)) {
                            filter >>= 32;
                            idx += 32;
                        }
                        qj = _bit_scan_forward((uint32_t)(filter & 0x00000000FFFFFFFF));
                        filter >>= qj;
                        idx += qj;
                    }
#else
                    if (!(filter & 1)) {
                        while (!(filter & 65535)) {
                            filter >>= 16;
                            idx += 16;
                        }
                        while (!(filter & 255)) {
                            filter >>= 8;
                            idx += 8;
                        }
                        while (!(filter & 15)) {
                            filter >>= 4;
                            idx += 4;
                        }
                        while (!(filter & 1)) {
                            filter >>= 1;
                            ++idx;
                        }
                    }
#endif
                    key = e->keys[idx];
                    filter >>= 1;
                    val = e->values[idx++];
                    return false;
                }
                
                e = e->next;
                idx = 255;
                *ele = e;
            }
            
            indexarray++;
            if (indexarray == basearraysize)
                break;
            
            e = atomicarray[indexarray];
            idx = 255;
            *ele = e;
        }
        
        *ele = NULL;
        return true;
    }

    Z iteratepointer(atomic_element<K,Z>** ele, K& key, short& indexarray, uchar& idx, BULONG& filter, bool& end) {
        atomic_element<K,Z>* e = *ele;
        int qj;
        
        while (indexarray < basearraysize) {
            while (e) {
                if (idx == 255) {
                    filter = e->indexes;
                    idx = 0;
                }
                while (filter) {
#ifdef INTELINTRINSICS
                    if (!(filter & 1)) {
                        if (!(filter & 0x00000000FFFFFFFF)) {
                            filter >>= 32;
                            idx += 32;
                        }
                        qj = _bit_scan_forward((uint32_t)(filter & 0x00000000FFFFFFFF));
                        filter >>= qj;
                        idx += qj;
                    }
#else
                    if (!(filter & 1)) {
                        while (!(filter & 65535)) {
                            filter >>= 16;
                            idx += 16;
                        }
                        while (!(filter & 255)) {
                            filter >>= 8;
                            idx += 8;
                        }
                        while (!(filter & 15)) {
                            filter >>= 4;
                            idx += 4;
                        }
                        while (!(filter & 1)) {
                            filter >>= 1;
                            ++idx;
                        }
                    }
#endif
                    key = e->keys[idx];
                    filter >>= 1;
                    end = false;
                    return e->values[idx++];
                }
                
                e = e->next;
                idx = 255;
                *ele = e;
            }
            
            indexarray++;
            if (indexarray == basearraysize)
                break;
            
            e = atomicarray[indexarray];
            idx = 255;
            *ele = e;
        }
        
        *ele = NULL;
        end = true;
        return NULL;
    }

    void find(K& v, atomic_element<K,Z>** ele, short& indexarray, uchar& r, BULONG& filter, bool& end) {
        BULONG k;
        *ele = search(v, indexarray, k, r);
        if (*ele == NULL) {
            end = true;
            return;
        }
        filter = theONE << r;
    }

    void clear() {
        for (int i = 0; i < basearraysize; i++)
            (*atomicarray[i]).clear(true);
    }

    //affectation
    void operator =(atomic_map<K,Z>& t) {
        clear();
        (*atomicarray[0])._lock->lock();
        for (int i = 0; i < basearraysize; i++)
            (*atomicarray[i]).duplicate(t.atomicarray[i]);
        (*atomicarray[0])._lock->unlock();
    }
};


//Use this iterator if the Z is a pointer...
template <class K,class Z> class atomic_iterator {
public:
  
    atomic_map<K,Z>* mm;
    atomic_element<K, Z>* e;
    
    K first;
    Z second;

    BULONG filter;
    short indexarray;
    uchar idx;
    bool ending;

    atomic_iterator<K,Z>() {
        ending = false;
        e = NULL;
        idx = 255;
        indexarray = 0;
    }

    atomic_iterator<K,Z>(atomic_map<K,Z>& m) {
        mm = &m;
        e = NULL;
        idx = 255;
        indexarray = 0;
        mm->Begin(&e);
        second = mm->iteratepointer(&e, first, indexarray, idx, filter, ending);
    }

    void next() {
        second = mm->iteratepointer(&e, first, indexarray, idx, filter, ending);
    }
    
    bool end() {
        return ending;
    }
    
    void begin(atomic_map<K,Z>& m) {
        mm = &m;
        e = NULL;
        idx = 255;
        indexarray = 0;
        mm->Begin(&e);
        second = mm->iteratepointer(&e, first, indexarray, idx, filter, ending);
    }

};


#endif
