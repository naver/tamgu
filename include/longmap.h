/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : longmap.h
 Date       : 2017/09/01
 Purpose    : Implementation of a map sans collision 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */


#ifndef i_longmap
#define i_longmap

#ifdef INTELINTRINSICS
#ifdef WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif
#endif

const unsigned long binlongbits = 6;
const unsigned long binlongsize = 1 << binlongbits;
const binuint64 binlongONE = 1;

const binuint64 binlongval64[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768,
    binONE << 16, binONE << 17, binONE << 18, binONE << 19, binONE << 20, binONE << 21, binONE << 22, binONE << 23, binONE << 24,
    binONE << 25, binONE << 26, binONE << 27, binONE << 28,
    binONE << 29, binONE << 30, binONE << 31, binONE << 32, binONE << 33, binONE << 34, binONE << 35, binONE << 36, binONE << 37,
    binONE << 38, binONE << 39, binONE << 40, binONE << 41,
    binONE << 42, binONE << 43, binONE << 44, binONE << 45, binONE << 46, binONE << 47, binONE << 48, binONE << 49, binONE << 50,
    binONE << 51, binONE << 52, binONE << 53, binONE << 54,
    binONE << 55, binONE << 56, binONE << 57, binONE << 58, binONE << 59, binONE << 60, binONE << 61, binONE << 62, binONE << 63 };

template <class Z> class binlong_hash {
public:
    long tsize;
    Z** table;
    binuint64* indexes;
    bool initialization;
    
    binlong_hash(bool init = true, long sz = 4)  {
        initialization = init;
        tsize = sz;
        table = new Z*[tsize];
        indexes = new binuint64[tsize];
        
        for (long i=0; i<tsize;i++) {
            table[i] = NULL;
            indexes[i] = 0;
        }
    }
    
    binlong_hash(binlong_hash<Z>& t) {
        tsize = t.tsize;
        table = new Z*[tsize];
        indexes = new binuint64[tsize];
        initialization = t.initialization;
        
        for (long i = 0; i < t.tsize; i++) {
            if (t.indexes[i]) {
                table[i] = new Z[binsize];
                indexes[i] = t.indexes[i];
                for (bint j = 0; j < binsize; j++)
                    table[j] = t.table[j];
            }
            else {
                table[i] = NULL;
                indexes[i] = 0;
            }
        }
    }
    
    ~binlong_hash() {
        for (long i = 0; i < tsize; i++) {
            if (table[i] != NULL)
                delete[] table[i];
        }
        delete[] table;
        delete[] indexes;
    }
    
    Z&  get(unsigned long r) {
        unsigned long i = r >> binbits;
        r = r - (i << binbits);
        return table[i][r];
    }
    
    bool get(unsigned long p, long& i, long& r) {
        i = p >> binbits;
        if (i >= tsize || !indexes[i])
            return false;
        r = p - (i << binbits);
        if (!(indexes[i] & binlongval64[r]))
            return false;
        
        return true;
    }
    
    bool check(unsigned long r) {
        unsigned long i = r >> binbits;
        if (i >= tsize || !indexes[i])
            return false;
        r = r - (i << binbits);
        if (!(indexes[i] & binlongval64[r]))
            return false;
        
        return true;
    }
    
    Z search(unsigned long r) {
        unsigned long i = r >> binbits;
        if (i >= tsize || !indexes[i])
            return NULL;
        
        r = r - (i << binbits);
        return table[i][r];
    }
    
    //nettoyage
    void clear() {
        for (long i = 0; i < tsize; i++) {
            if (table[i] != NULL) {
                delete[] table[i];
                table[i] = NULL;
                indexes[i] = 0;
            }
        }
    }
    
    bool empty() {
        for (long i = 0; i < tsize; i++) {
            if (indexes[i])
                return false;
        }
        return true;
    }
    
    void erase(unsigned long r) {
        unsigned long i = r >> binbits;
        if (table[i] == NULL)
            return;
        
        r = r - (i << binbits);
        indexes[i] &= ~binlongval64[r];
        table[i][r] = NULL;
    }
    
#ifdef INTELINTRINSICS
    size_t size() {
        bint nb = 0;
        
        for (long i = 0; i < tsize; i++) {
            nb += _mm_popcnt_u64(indexes[i]);
        }
        return nb;
    }
#else
    size_t size() {
        bint nb = 0;
        binuint64 filter;
        
        for (long i = 0; i < tsize; i++) {
            if (table[i] != NULL) {
                filter = indexes[i];
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
            }
        }
        
        return nb;
    }
#endif

    
    void resize(long sz) {        
        Z** ntable = new Z*[sz];
        binuint64* nindexes = new binuint64[sz];
        
        long i;
        for (i = 0; i < tsize; i++) {
            ntable[i] = table[i];
            nindexes[i] = indexes[i];
        }
        
        tsize = sz;
        for (; i < tsize; i++)  {
            ntable[i] = NULL;
            nindexes[i] = 0;
        }
        delete[] table;
        delete[] indexes;
        table = ntable;
        indexes = nindexes;
    }
    
    Z& operator [](unsigned long r) {
        unsigned long i = r >> binbits;
        r = r - (i << binbits);
        if (i >= tsize)
            resize(i + (i>>2));
        if (table[i] == NULL) {
            table[i] = new Z[binsize];
            if (initialization)
                memset(table[i], NULL, sizeof(Z) << binbits);
        }
        indexes[i] |= binlongval64[r];
        //indexes[i] |= (1 << r);
        return table[i][r];
    }
    
    void put(unsigned long r, Z v) {
        unsigned long i = r >> binbits;
        r = r - (i << binbits);
        if (i >= tsize)
            resize(i + 2);
        if (table[i] == NULL) {
            table[i] = new Z[binsize];
            if (initialization)
                memset(table[i], NULL, sizeof(Z) << binbits);
        }
        indexes[i] |= binlongval64[r];
        //indexes[i] |= (1 << r);
        table[i][r] = v;
    }
    
    //affectation
    void operator =(binlong_hash<Z>& t) {
        long i;
        
        for (i = 0; i < tsize; i++) {
            if (indexes[i])
                delete[] table[i];
        }
        
        if (tsize != t.tsize) {
            delete[] table;
            delete[] indexes;
            
            tsize = t.tsize;
            table = new Z*[tsize];
            indexes = new binuint64[tsize];
        }
        
        initialization = t.initialization;
        
        for (i = 0; i < t.tsize; i++) {
            if (t.indexes[i]) {
                table[i] = new Z[binsize];
                indexes[i] = t.indexes[i];
                for (bint j = 0; j < binsize; j++)
                    table[j] = t.table[j];
            }
            else {
                table[i] = NULL;
                indexes[i] = 0;
            }
        }
    }
};

#endif
