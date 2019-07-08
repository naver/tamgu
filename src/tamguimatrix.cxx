/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguimatrix.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamguimatrix.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<imatrixMethod>  Tamguimatrix::methods;
Exporting hmap<string, string> Tamguimatrix::infomethods;
Exporting bin_hash<unsigned long> Tamguimatrix::exported;

Exporting short Tamguimatrix::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguimatrix::AddMethod(TamguGlobal* global, string name, imatrixMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamguimatrix::Setidtype(TamguGlobal* global) {
        Tamguimatrix::idtype = global->Getid("imatrix");
    }

   bool Tamguimatrix::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    Tamguimatrix::idtype = global->Getid("imatrix");

    Tamguimatrix::AddMethod(global, "_initial", &Tamguimatrix::MethodInitial, P_ONE | P_TWO, "_initial(int rowsiwe; int columnsize): Initialize the size of the matrix.");
    Tamguimatrix::AddMethod(global, "transpose", &Tamguimatrix::MethodTransposed, P_NONE, "transpose(): return the transposed matrix");
    Tamguimatrix::AddMethod(global, "dimension", &Tamguimatrix::MethodMatrixSize, P_NONE | P_TWO, "dimension(): return the matrix size.\ndimension(int rowsizeint columnsize): set the matrix size.");
    Tamguimatrix::AddMethod(global, "determinant", &Tamguimatrix::MethodDeterminant, P_NONE, "determinant(): return the matrix determinant");
    Tamguimatrix::AddMethod(global, "sum", &Tamguimatrix::MethodSum, P_NONE, "sum(): return the sum of all elements");
    Tamguimatrix::AddMethod(global, "product", &Tamguimatrix::MethodProduct, P_NONE, "product(): return the product of all elements");
    Tamguimatrix::AddMethod(global, "invert", &Tamguimatrix::MethodInversion, P_NONE, "invert(): Return the inverted matrix.");
    Tamguimatrix::AddMethod(global, "format", &Tamguimatrix::MethodFormat, P_THREE, "format(string sep, string nxtline, string zero): Return the matrix as a string.");



    global->newInstance[Tamguimatrix::idtype] = new Tamguimatrix(global);
    global->RecordMethods(Tamguimatrix::idtype,Tamguimatrix::exported);

    return true;
}



Tamgu* Tamguimatrix::Put(Tamgu* idx, Tamgu* ke, short idthread) {

    Locking _lock(this);
    
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        Clear();
        if (ke->Type() != idtype)
            return aTRUE;
        ((Tamguimatrix*)ke)->populate(this);
        return aTRUE;
    }

    Tamgu* keyleft;
    Tamgu* keyright;

    //Two possibilities:
    //both indexes
    bool interval = idx->isInterval();
    TamguIndex* kind = (TamguIndex*)idx;
    if (interval == true && kind->left != aNULL && kind->right != aNULL) {
        keyleft = kind->left->Get(aNULL, aNULL, idthread);
        keyright = kind->right->Get(aNULL, aNULL, idthread);
        populate(keyleft, keyright, ke);
        return aTRUE;
    }
    long r, c;
    TamguIteration* itr;
    //it should be a map as value or a matriceline
    if (ke->isVectorContainer()) {
        long itx = 0;
        //then it means that only the column key is present
        if (kind->right != NULL && kind->right != aNULL) {
            keyleft = kind->right->Get(aNULL, aNULL, idthread);
            c = keyleft->Integer();

            itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
                populate(itx, c, itr->Valueinteger());
                itx++;
            }
            itr->Release();
            return aTRUE;
        }

        //We have the line, not the column
        if (kind->left != NULL && kind->left != aNULL) {
            keyleft = kind->left->Get(aNULL, aNULL, idthread);
            r = keyleft->Integer();
            itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
                populate(r, itx, itr->Valueinteger());
                itx++;
            }
            itr->Release();
            return aTRUE;
        }
    }
    else
        if (ke->isMapContainer()) {
            //then it means that only the column key is present
            if (kind->right != NULL && kind->right != aNULL) {
                keyleft = kind->right->Get(aNULL, aNULL, idthread);
                c = keyleft->Integer();
                itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
                    r = itr->Keyinteger();
                    populate(r, c, itr->Valueinteger());
                }
                itr->Release();
                return aTRUE;
            }

            //We have the line, not the column
            if (kind->left != NULL && kind->left != aNULL) {
                keyleft = kind->left->Get(aNULL, aNULL, idthread);
                r = keyleft->Integer();
                itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
                    c = itr->Keyinteger();
                    populate(r, c, itr->Valueinteger());
                }
                itr->Release();
                return aTRUE;
            }
        }
    return globalTamgu->Returnerror("MAT(131): Wrong index or Wrong value (recipient should be a map or a mvector)", idthread);
}

Tamgu* Tamguimatrix::Get(Tamgu* context, Tamgu* idx, short idthread) {
    Locking _lock(this);

    if (!idx->isIndex())
        return this;

    //Two possibilities:
    //both indexes
    bool interval = idx->isInterval();
    TamguIndex* kind = (TamguIndex*)idx;
    long r, c;
    if (interval == true && kind->left != aNULL && kind->right != aNULL) {
        r = kind->left->Get(aNULL, aNULL, idthread)->Integer();
        c = kind->right->Get(aNULL, aNULL, idthread)->Integer();
        if (values.check(r) && values[r].check(c))
            return globalTamgu->Provideint(values[r][c]);
        return aZERO;
    }

    //then it means that only the column key is present
    if (kind->right != NULL && kind->right != aNULL) {
        c = kind->right->Get(aNULL, aNULL, idthread)->Integer();
        if (c < 0 || c >= columnsize)
            return aNULL;

        Tamgu* kvect = Selectaivector(context);

        for (unsigned short r = 0; r < rowsize; r++) {
            if (values.check(r) && values[r].check(c))
                kvect->storevalue(values[r][c]);
            else
                kvect->storevalue((long)0);
        }
        return kvect;
    }

    if (kind->left != NULL && kind->left != aNULL) {
        //We have the line, not the column
        r = kind->left->Get(aNULL, aNULL, idthread)->Integer();
        if (r < 0 || r >= rowsize)
            return aNULL;

        Tamgu* kvect = Selectaivector(context);
        if (values.check(r) == false)
            return kvect;

        TAMGUILINEINTEGER& line = values[r];
        for (unsigned short i = 0; i < columnsize; i++) {
            if (line.check(i))
                kvect->storevalue(line[i]);
            else
                kvect->storevalue((long)0);
        }
        return kvect;
    }
    return this;
}


Tamgu* Tamguimatrix::MethodTransposed(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamguimatrix* res = new Tamguimatrix;
    for (unsigned short r = 0; r < rowsize; r++) {
        if (values.check(r)) {
            for (unsigned short c = 0; c < columnsize; c++) {
                if (values[r].check(c))
                    res->populate(c, r, values[r][c]);
            }
        }
    }
    res->rowsize = columnsize;
    res->columnsize = rowsize;
    return res;
}

Tamgu* Tamguimatrix::MethodMatrixSize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (callfunc->Size() == 0) {
        Tamgu* kvect = Selectaivector(contextualpattern);
        kvect->storevalue((long)rowsize);
        kvect->storevalue((long)columnsize);
        return kvect;
    }
    //we use the values to set the new matrix size...
    long szr = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    long szc = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    rowsize = szr;
    columnsize = szc;
    values.update(szr, szc);
    return aTRUE;
}

Tamgu* Tamguimatrix::MethodDeterminant(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (columnsize < 2 || columnsize != rowsize)
        return 0;
    long det = determinant(rowsize, columnsize, values);
    return globalTamgu->Provideint(det);
}

Tamgu* Tamguimatrix::MethodSum(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long v = 0;
    for (unsigned short r = 0; r < rowsize; r++) {
        if (values.check(r)) {
            for (unsigned short c = 0; c < columnsize; c++) {
                if (values[r].check(c))
                    v += values[r][c];
            }
        }
    }
    return globalTamgu->Provideint(v);
}

Tamgu* Tamguimatrix::MethodProduct(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long v = 1;
    for (unsigned short r = 0; r < rowsize; r++) {
        if (values.check(r)) {
            for (unsigned short c = 0; c < columnsize; c++) {
                if (values[r].check(c))
                    v *= values[r][c];
            }
        }
    }
    return globalTamgu->Provideint(v);
}

Tamgu* Tamguimatrix::MethodInversion(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (rowsize != columnsize)
        return aNULL;
    Tamguimatrix* Y = new Tamguimatrix;
    if (!inversion(*Y)) {
        Y->Release();
        return aNULL;
    }
    return Y;
}
