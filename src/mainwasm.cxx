/*
 *  LispE
 *
 * Copyright 2020-present NAVER Corp.
 * The 3-Clause BSD License
 */
//
//  mainasm.cxx


//-------------------------------------------------------------------------------------------
#include "tamgu.h"
#include "globaltamgu.h"
#include "tamgusys.h"
#include <emscripten/emscripten.h>
//-------------------------------------------------------------------------------------------
static char THEMAIN[] = "/MAIN\0";
void s_utf8_to_utf16(wstring& w, string& str);
void s_utf16_to_utf8(string& s, wstring& str);
void s_unicode_to_utf16(wstring& w, wstring& u);

extern "C" EMSCRIPTEN_KEEPALIVE void clean_tamgu() {
    if (TamguSelectglobal(0)) {
        TamguDeleteGlobal(0);
    }
}

extern "C" EMSCRIPTEN_KEEPALIVE void reset_tamgu() {
    if (!TamguSelectglobal(0)) {
        TamguCreate();
        return;
    }
    TamguDeleteGlobal(0);
    TamguCreate();
}

extern "C" EMSCRIPTEN_KEEPALIVE int32_t eval_tamgu(int32_t* str, int32_t sz, int32_t mx) {
    wstring code;
    for (long i = 0; i < sz; i++) {
        code += str[i];
    }

    long begin_instruction = 0;
    if (!TamguSelectglobal(0))
        TamguCreate();
    else {
        begin_instruction = TamguLastInstruction(0);
        if (begin_instruction < 0)
            begin_instruction = 0;
    }

    string cde;
    //We first convert from UTF-16 into UTF-8
    //JavaScript strings are in UTF-16
    s_utf16_to_utf8(cde, code);
    Trim(cde);
    if (cde.size() == 0)
        return 0;
    
    long idCode = TamguCompile(cde, THEMAIN, false);

    if (idCode == -1) {
        cde = TamguErrorMessage();
        globalTamgu->Cleanerror(0);
    }
    else {
        Tamgu* a = TamguEval(idCode, begin_instruction);
        if (a == NULL) {
            cde = TamguErrorMessage();
            globalTamgu->Cleanerror(0);
        }
        else {
            cde = a->String();
            a->Release();
        }
    }
    
    //code is encoded in UTF-32
    //We need to convert it back to UTF-16
    //emojis for instance belong to the double UTF-16 family
    wstring result;
    s_utf8_to_utf16(result, cde);
    sz = result.size();
    sz = sz < mx ? sz : mx - 1;

    //We then replace the initial array (str) with our new values
    //WASM cannot handle strings directly, we have to pass them as arrays of uint_32 elements
    for (long i = 0; i < sz; i++)
    str[i] = result[i];
    return sz;
}


extern "C" EMSCRIPTEN_KEEPALIVE int32_t eval_to_float_tamgu(int32_t* str, int32_t sz, double* values, int32_t mx) {
    wstring code;
    for (long i = 0; i < sz; i++) {
        code += str[i];
    }
    
    long begin_instruction = 0;
    if (!TamguSelectglobal(0))
        TamguCreate();
    else {
        begin_instruction = TamguLastInstruction(0);
        if (begin_instruction < 0)
            begin_instruction = 0;
    }
    
    string cde;
    //We first convert from UTF-16 into UTF-8
    //JavaScript strings are in UTF-16
    s_utf16_to_utf8(cde, code);
    Trim(cde);
    if (cde.size() == 0)
        return 0;
    
    long idCode = TamguCompile(cde, THEMAIN, false);
    
    Tamgu* a = aNULL;
    if (idCode == -1) {
        cout << TamguErrorMessage() << endl;
        globalTamgu->Cleanerror(0);
        return 0;
    }
    else {
        a = TamguEval(idCode, begin_instruction);
        if (a == NULL) {
            cout << TamguErrorMessage() << endl;
            globalTamgu->Cleanerror(0);
            return 0;
        }
    }

    if (a->isVectorContainer()) {
        sz = a->Size();
        sz = sz < mx ? sz : mx - 1;
        for (long i = 0; i < sz; i++) {
            values[i] = a->getfloat(i);
        }
    }
    else {
        sz = 1;
        values[0] = a->Float();
    }
    a->Release();
    return sz;
}

extern "C" EMSCRIPTEN_KEEPALIVE int32_t eval_to_int_tamgu(int32_t* str, int32_t sz, int32_t* values, int32_t mx) {
    wstring code;
    for (long i = 0; i < sz; i++) {
        code += str[i];
    }

    long begin_instruction = 0;
    if (!TamguSelectglobal(0))
        TamguCreate();
    else {
        begin_instruction = TamguLastInstruction(0);
        if (begin_instruction < 0)
            begin_instruction = 0;
    }

    string cde;
    //We first convert from UTF-16 into UTF-8
    //JavaScript strings are in UTF-16
    s_utf16_to_utf8(cde, code);
    Trim(cde);
    if (cde.size() == 0)
        return 0;
    
    long idCode = TamguCompile(cde, THEMAIN, false);

    Tamgu* a = aNULL;
    if (idCode == -1) {
        cout << TamguErrorMessage() << endl;
        globalTamgu->Cleanerror(0);
        return 0;
    }
    else {
        a = TamguEval(idCode, begin_instruction);
        if (a == NULL) {
            cout << TamguErrorMessage() << endl;
            globalTamgu->Cleanerror(0);
            return 0;
        }
    }
    
    if (a->isVectorContainer()) {
        sz = a->Size();
        sz = sz < mx ? sz : mx - 1;
        for (long i = 0; i < sz; i++) {
            values[i] = a->getinteger(i);
        }
    }
    else {
        sz = 1;
        values[0] = a->Integer();
    }

    a->Release();
    return sz;
}

extern "C" EMSCRIPTEN_KEEPALIVE int32_t eval_to_string_tamgu(int32_t* str, int32_t sz, int32_t* values, int32_t mx) {
    wstring code;
    for (long i = 0; i < sz; i++) {
        code += str[i];
    }

    long begin_instruction = 0;
    if (!TamguSelectglobal(0))
        TamguCreate();
    else {
        begin_instruction = TamguLastInstruction(0);
        if (begin_instruction < 0)
            begin_instruction = 0;
    }

    string cde;
    //We first convert from UTF-16 into UTF-8
    //JavaScript strings are in UTF-16
    s_utf16_to_utf8(cde, code);
    Trim(cde);
    if (cde.size() == 0)
        return 0;
    
    long idCode = TamguCompile(cde, THEMAIN, false);

    Tamgu* a = aNULL;
    if (idCode == -1) {
        cout << TamguErrorMessage() << endl;
        globalTamgu->Cleanerror(0);
        return 0;
    }
    else {
        a = TamguEval(idCode, begin_instruction);
        if (a == NULL) {
            cout << TamguErrorMessage() << endl;
            globalTamgu->Cleanerror(0);
            return 0;
        }
    }

    long idx = 0;
    wstring result;
    if (a->isVectorContainer()) {
        long nb = a->Size();
        for (long i = 0; i < nb; i++) {
            code = a->getustring(i);
            result = L"";
            s_unicode_to_utf16(result, code);
            //We then replace the initial array (str) with our new values
            //WASM cannot handle strings directly, we have to pass them as arrays of uint_32 elements
            for (long i = 0; i < result.size() && idx < mx; i++)  {
                values[idx++] = result[i];
            }
            if (idx < mx)
                values[idx++] = 0;
            else
                break;
        }
    }
    else {
        code = a->UString();
        s_unicode_to_utf16(result, code);
        //We then replace the initial array (str) with our new values
        //WASM cannot handle strings directly, we have to pass them as arrays of uint_32 elements
        for (long i = 0; i < result.size() && idx < mx; i++)  {
            values[idx++] = result[i];
        }
        if (idx < mx)
            values[idx++] = 0;
    }
    
    a->Release();
    return idx;
}

//Main initialisation of LispE
int main() {
    TamguCreate();
    return 0;
}

