/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : comparetemplate.h
 Date       : 2017/09/01
 Purpose    : Comparison template to implement fast comparison between numbers or strings 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#ifndef compare_h
#define compare_h

#include "tamgu.h"


class TamguInstructionCOMPARE : public TamguInstruction {
public:
    bool taskellif;
    
    TamguInstructionCOMPARE(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructions, g, parent) {
        taskellif=false;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    Tamgu* Compile(TamguGlobal* global, Tamgu* parent);
    
    void Setpartialtest(bool b) {
        taskellif=true;
    }

    bool Checkarity() {
        return (instructions.size() == 2);
    }

    bool isPartialtest() {
        return taskellif;
    }
};

class c_compare : public Tamgu {
public:
    Tamgu* left;
    
    c_compare(TamguGlobal* g, TamguInstruction* ins)  {
        if (g != NULL)
            g->RecordInTracker(this);
        
        left = ins->instructions.vecteur[0];
    }
    
    virtual void ScanVariables(vector<short>& vars) {
        left->ScanVariables(vars);
    }
};

//This a specific implementation in which both values have to be gathered first...
class c_double_compare : public c_compare {
public:
    Tamgu* right;
  
    c_double_compare(TamguGlobal* g, TamguInstruction* ins) : c_compare(g, ins) {
        right = ins->instructions.vecteur[1];
    }
    
    void ScanVariables(vector<short>& vars) {
        left->ScanVariables(vars);
        right->ScanVariables(vars);
    }
};

class c_different : public c_double_compare {
public:
    
    c_different(TamguGlobal* g, TamguInstruction* ins) : c_double_compare(g,ins) {}
    
    Tamgu* Eval(Tamgu* r, Tamgu* l, short idthread) {
        l = left->Eval(r, aNULL, idthread);
        r = right->Eval(r, aNULL, idthread);

        Tamgu* rp = l->different(r);
        l->Release();
        r->Release();
        return rp;
    }
};

class c_same : public c_double_compare {
public:
    
    c_same(TamguGlobal* g, TamguInstruction* ins) : c_double_compare(g,ins) {}
    
    Tamgu* Eval(Tamgu* r, Tamgu* l, short idthread) {
        l = left->Eval(r, aNULL, idthread);
        r = right->Eval(r, aNULL, idthread);

        Tamgu* rp = l->same(r);
        l->Release();
        r->Release();
        return rp;
    }
};


class c_more : public c_double_compare {
public:
    
    c_more(TamguGlobal* g, TamguInstruction* ins) : c_double_compare(g,ins) {}
    
    Tamgu* Eval(Tamgu* r, Tamgu* l, short idthread) {
        l = left->Eval(r, aNULL, idthread);
        r = right->Eval(r, aNULL, idthread);

        Tamgu* rp = l->more(r);
        l->Release();
        r->Release();
        return rp;
    }
};


class c_moreequal : public c_double_compare {
public:
    
    c_moreequal(TamguGlobal* g, TamguInstruction* ins) : c_double_compare(g,ins) {}
    
    Tamgu* Eval(Tamgu* r, Tamgu* l, short idthread) {
        l = left->Eval(r, aNULL, idthread);
        r = right->Eval(r, aNULL, idthread);

        Tamgu* rp = l->moreequal(r);
        l->Release();
        r->Release();
        return rp;
    }
};

class c_less : public c_double_compare {
public:
    
    c_less(TamguGlobal* g, TamguInstruction* ins) : c_double_compare(g,ins) {}
    
    Tamgu* Eval(Tamgu* r, Tamgu* l, short idthread) {
        l = left->Eval(r, aNULL, idthread);
        r = right->Eval(r, aNULL, idthread);
        
        if (l->isNULL() || r->isNULL()) {
            l->Release();
            r->Release();
            return aFALSE;
        }
        
        Tamgu* rp = l->less(r);
        l->Release();
        r->Release();
        return rp;
    }
};


class c_lessequal : public c_double_compare {
public:
    
    c_lessequal(TamguGlobal* g, TamguInstruction* ins) : c_double_compare(g,ins) {}
    
    Tamgu* Eval(Tamgu* r, Tamgu* l, short idthread) {
        l = left->Eval(r, aNULL, idthread);
        r = right->Eval(r, aNULL, idthread);

        if (l->isNULL() || r->isNULL()) {
            if (l == r)
                return aTRUE;
            l->Release();
            r->Release();
            return aFALSE;
        }

        Tamgu* rp = l->lessequal(r);
        l->Release();
        r->Release();
        return rp;
    }
};

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//Constant based comparison... right is a constant value...
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
class c_moreequal_decimal : public c_compare {
public:
    float right;
    
    c_moreequal_decimal(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Decimal();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getdecimal(idthread) >= right) ? aTRUE : aFALSE;
    }
};



class c_moreequal_long : public c_compare {
public:
    BLONG right;
    
    c_moreequal_long(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Long();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getlong(idthread) >= right) ? aTRUE : aFALSE;
    }
};



class c_moreequal_ustring : public c_compare {
public:
    wstring right;
    
    c_moreequal_ustring(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->UString();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getustring(idthread) >= right) ? aTRUE : aFALSE;
    }
};



class c_moreequal_short : public c_compare {
public:
    short right;
    
    c_moreequal_short(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Short();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getshort(idthread) >= right) ? aTRUE : aFALSE;
    }
};



class c_moreequal_int : public c_compare {
public:
    long right;
    
    c_moreequal_int(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Integer();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getinteger(idthread) >= right) ? aTRUE : aFALSE;
    }
};



class c_moreequal_string : public c_compare {
public:
    string right;
    
    c_moreequal_string(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->String();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getstring(idthread) >= right) ? aTRUE : aFALSE;
    }
};



class c_moreequal_float : public c_compare {
public:
    double right;
    
    c_moreequal_float(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Float();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getfloat(idthread) >= right) ? aTRUE : aFALSE;
    }
};



class c_less_decimal : public c_compare {
public:
    float right;
    
    c_less_decimal(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Decimal();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getdecimal(idthread) < right) ? aTRUE : aFALSE;
    }
};



class c_less_long : public c_compare {
public:
    BLONG right;
    
    c_less_long(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Long();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getlong(idthread) < right) ? aTRUE : aFALSE;
    }
};



class c_less_ustring : public c_compare {
public:
    wstring right;
    
    c_less_ustring(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->UString();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getustring(idthread) < right) ? aTRUE : aFALSE;
    }
};



class c_less_short : public c_compare {
public:
    short right;
    
    c_less_short(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Short();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getshort(idthread) < right) ? aTRUE : aFALSE;
    }
};



class c_less_int : public c_compare {
public:
    long right;
    
    c_less_int(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Integer();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getinteger(idthread) < right) ? aTRUE : aFALSE;
    }
};



class c_less_string : public c_compare {
public:
    string right;
    
    c_less_string(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->String();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getstring(idthread) < right) ? aTRUE : aFALSE;
    }
};



class c_less_float : public c_compare {
public:
    double right;
    
    c_less_float(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Float();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getfloat(idthread) < right) ? aTRUE : aFALSE;
    }
};



class c_more_decimal : public c_compare {
public:
    float right;
    
    c_more_decimal(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Decimal();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getdecimal(idthread) > right) ? aTRUE : aFALSE;
    }
};



class c_more_long : public c_compare {
public:
    BLONG right;
    
    c_more_long(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Long();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getlong(idthread) > right) ? aTRUE : aFALSE;
    }
};



class c_more_ustring : public c_compare {
public:
    wstring right;
    
    c_more_ustring(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->UString();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getustring(idthread) > right) ? aTRUE : aFALSE;
    }
};



class c_more_short : public c_compare {
public:
    short right;
    
    c_more_short(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Short();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getshort(idthread) > right) ? aTRUE : aFALSE;
    }
};



class c_more_int : public c_compare {
public:
    long right;
    
    c_more_int(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Integer();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getinteger(idthread) > right) ? aTRUE : aFALSE;
    }
};



class c_more_string : public c_compare {
public:
    string right;
    
    c_more_string(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->String();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getstring(idthread) > right) ? aTRUE : aFALSE;
    }
};



class c_more_float : public c_compare {
public:
    double right;
    
    c_more_float(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Float();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getfloat(idthread) > right) ? aTRUE : aFALSE;
    }
};



class c_lessequal_decimal : public c_compare {
public:
    float right;
    
    c_lessequal_decimal(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Decimal();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getdecimal(idthread) <= right) ? aTRUE : aFALSE;
    }
};



class c_lessequal_long : public c_compare {
public:
    BLONG right;
    
    c_lessequal_long(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Long();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getlong(idthread) <= right) ? aTRUE : aFALSE;
    }
};



class c_lessequal_ustring : public c_compare {
public:
    wstring right;
    
    c_lessequal_ustring(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->UString();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getustring(idthread) <= right) ? aTRUE : aFALSE;
    }
};



class c_lessequal_short : public c_compare {
public:
    short right;
    
    c_lessequal_short(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Short();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getshort(idthread) <= right) ? aTRUE : aFALSE;
    }
};



class c_lessequal_int : public c_compare {
public:
    long right;
    
    c_lessequal_int(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Integer();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getinteger(idthread) <= right) ? aTRUE : aFALSE;
    }
};



class c_lessequal_string : public c_compare {
public:
    string right;
    
    c_lessequal_string(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->String();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getstring(idthread) <= right) ? aTRUE : aFALSE;
    }
};



class c_lessequal_float : public c_compare {
public:
    double right;
    
    c_lessequal_float(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Float();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getfloat(idthread) <= right) ? aTRUE : aFALSE;
    }
};



class c_different_decimal : public c_compare {
public:
    float right;
    
    c_different_decimal(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Decimal();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getdecimal(idthread) != right) ? aTRUE : aFALSE;
    }
};



class c_different_long : public c_compare {
public:
    BLONG right;
    
    c_different_long(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Long();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getlong(idthread) != right) ? aTRUE : aFALSE;
    }
};



class c_different_ustring : public c_compare {
public:
    wstring right;
    
    c_different_ustring(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->UString();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getustring(idthread) != right) ? aTRUE : aFALSE;
    }
};



class c_different_short : public c_compare {
public:
    short right;
    
    c_different_short(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Short();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getshort(idthread) != right) ? aTRUE : aFALSE;
    }
};



class c_different_int : public c_compare {
public:
    long right;
    
    c_different_int(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Integer();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getinteger(idthread) != right) ? aTRUE : aFALSE;
    }
};



class c_different_string : public c_compare {
public:
    string right;
    
    c_different_string(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->String();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getstring(idthread) != right) ? aTRUE : aFALSE;
    }
};



class c_different_float : public c_compare {
public:
    double right;
    
    c_different_float(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Float();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getfloat(idthread) != right) ? aTRUE : aFALSE;
    }
};



class c_same_decimal : public c_compare {
public:
    float right;
    
    c_same_decimal(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Decimal();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getdecimal(idthread) == right) ? aTRUE : aFALSE;
    }
};



class c_same_long : public c_compare {
public:
    BLONG right;
    
    c_same_long(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Long();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getlong(idthread) == right) ? aTRUE : aFALSE;
    }
};



class c_same_ustring : public c_compare {
public:
    wstring right;
    
    c_same_ustring(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->UString();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getustring(idthread) == right) ? aTRUE : aFALSE;
    }
};



class c_same_short : public c_compare {
public:
    short right;
    
    c_same_short(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Short();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getshort(idthread) == right) ? aTRUE : aFALSE;
    }
};



class c_same_int : public c_compare {
public:
    long right;
    
    c_same_int(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Integer();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getinteger(idthread) == right) ? aTRUE : aFALSE;
    }
};



class c_same_string : public c_compare {
public:
    string right;
    
    c_same_string(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->String();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getstring(idthread) == right) ? aTRUE : aFALSE;
    }
};



class c_same_float : public c_compare {
public:
    double right;
    
    c_same_float(TamguGlobal* g, TamguInstruction* ins) : c_compare(g,ins) {
        right = ins->instructions.vecteur[1]->Float();
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* a, short idthread) {
        return (left->Getfloat(idthread) == right) ? aTRUE : aFALSE;
    }
};



#endif


