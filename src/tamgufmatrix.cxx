/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgufmatrix.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgufmatrix.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<fmatrixMethod>  Tamgufmatrix::methods;
Exporting hmap<string, string> Tamgufmatrix::infomethods;
Exporting basebin_hash<unsigned long> Tamgufmatrix::exported;

Exporting short Tamgufmatrix::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgufmatrix::AddMethod(TamguGlobal* global, string name, fmatrixMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgufmatrix::Setidtype(TamguGlobal* global) {
    Tamgufmatrix::InitialisationModule(global,"");
}


   bool Tamgufmatrix::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    Tamgufmatrix::idtype = global->Getid("fmatrix");

    Tamgufmatrix::AddMethod(global, "_initial", &Tamgufmatrix::MethodInitial, P_ONE | P_TWO, "_initial(int rowsiwe; int columnsize): Initialize the size of the matrix.");
    Tamgufmatrix::AddMethod(global, "transpose", &Tamgufmatrix::MethodTransposed, P_NONE, "transpose(): return the transposed matrix");
    Tamgufmatrix::AddMethod(global, "dimension", &Tamgufmatrix::MethodMatrixSize, P_NONE | P_TWO, "dimension(): return the matrix size.\ndimension(int rowsizeint columnsize): set the matrix size.");
    Tamgufmatrix::AddMethod(global, "determinant", &Tamgufmatrix::MethodDeterminant, P_NONE, "determinant(): return the matrix determinant");
    Tamgufmatrix::AddMethod(global, "sum", &Tamgufmatrix::MethodSum, P_NONE, "sum(): return the sum of all elements");
    Tamgufmatrix::AddMethod(global, "product", &Tamgufmatrix::MethodProduct, P_NONE, "product(): return the product of all elements");
    Tamgufmatrix::AddMethod(global, "invert", &Tamgufmatrix::MethodInversion, P_NONE, "invert(): Return the inverted matrix.");
    Tamgufmatrix::AddMethod(global, "clear", &Tamgufmatrix::MethodClear, P_NONE, "clean(): clean the matrix.");
    Tamgufmatrix::AddMethod(global, "format", &Tamgufmatrix::MethodFormat, P_THREE, "format(string sep, string nxtline, string zero): Return the matrix as a string.");



    if (version != "") {
        global->newInstance[Tamgufmatrix::idtype] = new Tamgufmatrix(global);
        global->RecordMethods(Tamgufmatrix::idtype, Tamgufmatrix::exported);
    }

    return true;
}

TamguIteration* Tamgufmatrix::Newiteration(bool direction) {
    return new TamguIterationfmatrix(this, direction);
}


Tamgu* Tamgufmatrix::Put(Tamgu* idx, Tamgu* ke, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        Clear();
        if (ke->Type() != idtype)
            return aFALSE;
        ((Tamgufmatrix*)ke)->populate(this);
        return aTRUE;
    }

    Tamgu* keyleft;
    Tamgu* keyright;

    //Two possibilities:
    //both indexes
    bool interval = idx->isInterval();
    TamguIndex* kind = (TamguIndex*)idx;
    if (interval == true && kind->left != aNULL && kind->right != aNULL) {
        keyleft = kind->left->Eval(aNULL, aNULL, idthread);
        keyright = kind->right->Eval(aNULL, aNULL, idthread);
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
            keyleft = kind->right->Eval(aNULL, aNULL, idthread);
            c = keyleft->Integer();

            itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
                populate(itx, c, itr->Valuefloat());
                itx++;
            }
            itr->Release();
            return aTRUE;
        }

        //We have the line, not the column
        if (kind->left != NULL && kind->left != aNULL) {
            keyleft = kind->left->Eval(aNULL, aNULL, idthread);
            r = keyleft->Integer();
            itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
                populate(r, itx, itr->Valuefloat());
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
                keyleft = kind->right->Eval(aNULL, aNULL, idthread);
                c = keyleft->Integer();
                itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
                    r = itr->Keyinteger();
                    populate(r, c, itr->Valuefloat());
                }
                itr->Release();
                return aTRUE;
            }

            //We have the line, not the column
            if (kind->left != NULL && kind->left != aNULL) {
                keyleft = kind->left->Eval(aNULL, aNULL, idthread);
                r = keyleft->Integer();
                itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
                    c = itr->Keyinteger();
                    populate(r, c, itr->Valuefloat());
                }
                itr->Release();
                return aTRUE;
            }
        }
    return globalTamgu->Returnerror("MAT(131): Wrong index or Wrong value (recipient should be a map or a mvector)", idthread);
}

Tamgu* Tamgufmatrix::Eval(Tamgu* context, Tamgu* idx, short idthread) {
    Locking _lock(this);

    if (!idx-isIndex())
        return this;

    //Two possibilities:
    //both indexes
    bool interval = idx->isInterval();
    TamguIndex* kind = (TamguIndex*)idx;
    long r, c;
    if (interval == true && kind->left != aNULL && kind->right != aNULL) {
        r = kind->left->Eval(aNULL, aNULL, idthread)->Integer();
        c = kind->right->Eval(aNULL, aNULL, idthread)->Integer();
        if (values.check(r) && values[r].check(c))
            return globalTamgu->Providefloat(values[r][c]);
        return aZERO;
    }

    //then it means that only the column key is present
    if (kind->right != NULL && kind->right != aNULL) {
        c = kind->right->Eval(aNULL, aNULL, idthread)->Integer();
        if (c < 0 || c >= columnsize)
            return aNULL;

        Tamgu* kvect = Selectafvector(context);

        for (long r = 0; r < rowsize; r++) {
            if (values.check(r) && values[r].check(c))
                kvect->storevalue(values[r][c]);
            else
                kvect->storevalue((double)0);
        }
        return kvect;
    }

    if (kind->left != NULL && kind->left != aNULL) {
        //We have the line, not the column
        r = kind->left->Eval(aNULL, aNULL, idthread)->Integer();
        if (r < 0 || r >= rowsize)
            return aNULL;

        Tamgu* kvect = Selectafvector(context);
        if (values.check(r) == false)
            return kvect;

        TAMGUILINEFLOAT& line = values[r];
        for (long i = 0; i < columnsize; i++) {
            if (line.check(i))
                kvect->storevalue(line[i]);
            else
                kvect->storevalue((double)0);
        }
        return kvect;
    }
    return this;
}


Tamgu* Tamgufmatrix::MethodTransposed(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgufmatrix* res = new Tamgufmatrix;
    for (long r = 0; r < rowsize; r++) {
        if (values.check(r)) {
            for (long c = 0; c < columnsize; c++) {
                if (values[r].check(c))
                    res->populate(c, r, values[r][c]);
            }
        }
    }
    res->rowsize = columnsize;
    res->columnsize = rowsize;
    return res;
}

Tamgu* Tamgufmatrix::MethodMatrixSize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
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

Tamgu* Tamgufmatrix::MethodDeterminant(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (columnsize < 2 || columnsize != rowsize)
        return 0;
    double det = determinant(rowsize, columnsize, values);
    return globalTamgu->Providefloat(det);
}

Tamgu* Tamgufmatrix::MethodSum(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    double v = 0;
    for (long r = 0; r < rowsize; r++) {
        if (values.check(r)) {
            for (long c = 0; c < columnsize; c++) {
                if (values[r].check(c))
                    v += values[r][c];
            }
        }
    }
    return globalTamgu->Providefloat(v);
}

Tamgu* Tamgufmatrix::MethodProduct(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    double v = 1;
    for (long r = 0; r < rowsize; r++) {
        if (values.check(r)) {
            for (long c = 0; c < columnsize; c++) {
                if (values[r].check(c))
                    v *= values[r][c];
            }
        }
    }
    return globalTamgu->Providefloat(v);
}

Tamgu* Tamgufmatrix::MethodInversion(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (rowsize != columnsize)
        return aNULL;
    Tamgufmatrix* Y = new Tamgufmatrix;
    if (!inversion(*Y)) {
        Y->Release();
        return aNULL;
    }
    return Y;
}
