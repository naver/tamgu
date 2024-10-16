//-----------------------------------------------------------------
//In elements
const encode = function stringToIntegerArray(str, array) {
    for (let i = 0; i < str.length; i++) {
        array[i] = str.charCodeAt(i);
    }
    array[str.length] = 0;
};

const decode = function integerArrayToString(array, sz) {
    str = "";        
    sz *= 4;
    for (let pointer=0; pointer < sz; pointer+=4) {
        str += String.fromCharCode(Module.HEAP32[pointer + array>>2]);
    }
    Module._free(array);
    return str;
};

const decode_float_as_string = function floatArrayToString(array, sz) {
    str = "";        
    sz *= 8;
    for (let pointer=0; pointer < sz; pointer+=8) {
        str += String.fromCharCode(Module.HEAPF64[pointer + array>>3]);
    }
    Module._free(array);
    return str;
};

const decode_as_strings = function stringArrayToStrings(array, sz, str_vector, pos_vector) {
    if (sz != 0) {
        characters = decode_int(array, sz);
        let s = "";
        let nxt = false;
        for (var i = 0; i < sz; i++) {
            if (nxt) {
                pos_vector.push(characters[i]);
                nxt = false;
            }
            else {
                if (characters[i] == 0) {
                    str_vector.push(s);
                    nxt = true;
                    s = "";
                }
                else
                    s += String.fromCharCode(characters[i]);
            }
        }
        Module._free(array);
    }
};

const decode_float = function toFloatArray(array, sz) {
    value = new Float64Array(sz);
    sz *= 8;
    i = 0;
    for (let pointer=0; pointer < sz; pointer+=8) {
        value[i] = Module.HEAPF64[pointer + array>>3];
        i++;
    }
    Module._free(array);
    return value;
}

const decode_int = function toIntArray(array, sz) {
    value = new Int32Array(sz);
    sz *= 4;
    i = 0;
    for (let pointer=0; pointer < sz; pointer+=4) {
        value[i] = Module.HEAP32[pointer + array>>2];
        i++;
    }
    Module._free(array);
    return value;
}

const decode_size = function toFirstIntArray(array) {
    val = Module.HEAP32[array>>2];
    Module._free(array);
    return val;
}

function provideCharactersAsInts(code) {
    //We need some room for error messages
    nb = code.length + 1;
    nb = Math.max(20, nb);
    arr = new Int32Array(nb);
    for (i = 0; i < code.length; i++) {
        arr[i] = code.charCodeAt(i);
    }
    arr[code.length] = 0;
    a_buffer = Module._malloc(nb * 4);
    Module.HEAP32.set(arr, a_buffer >> 2)
    return a_buffer;
}

function provideIntegers(nb, values) {
    a_buffer = Module._malloc((nb + 1) * 4);
    Module.HEAP32.set(values, a_buffer >> 2)
    return a_buffer;
}

function provideFloats(nb, values) {
    a_buffer = Module._malloc((nb + 1) * 8);
    Module.HEAPF64.set(values, a_buffer >> 3)
    return a_buffer;
}

function provide_integers(nb) {
    return Module._malloc((nb + 1) * 4);
}

function provide_floats(nb) {
    return Module._malloc((nb + 1) * 8);
}

function callResetTamgu() {
    assert(runDependencies == 0, 'cannot call main when async dependencies remain! (listen on Module["onRuntimeInitialized"])');
    assert(__ATPRERUN__.length == 0, 'cannot call main when preRun functions remain to be called');

    entryFunction = Module['_reset_tamgu'];

    try {

        entryFunction();
        position_in_buffer = 0;
        return "true";
    }
    catch (e) {
        return handleException(e);
    }
}

//The displaymode makes it possible to analyse partial code definition such as: 10*20
//The system will then be able to produce the code to correctly display the relevant information...
function callEvalToFloats(code, displaymode) {
    assert(runDependencies == 0, 'cannot call main when async dependencies remain! (listen on Module["onRuntimeInitialized"])');
    assert(__ATPRERUN__.length == 0, 'cannot call main when preRun functions remain to be called');

    entryFunction = Module['_eval_to_floats_tamgu'];

    string_to_array = provideCharactersAsInts(code);

    the_size = provide_integers(2);

    var float_result;
    try {
        float_result = entryFunction(string_to_array, code.length, the_size, displaymode);
    }
    catch (e) {
        Module._free(string_to_array);
        Module._free(the_size);
        return handleException(e);
    }
    sz = decode_size(the_size);
    if (sz < 0) {
        sz *= -1;
        Module._free(string_to_array);
        str = decode_float_as_string(float_result, sz)
        throw new Error(str);
    }
    Module._free(string_to_array);
    return decode_float(float_result, sz);
}

function callEvalToInts(code, displaymode) {
    assert(runDependencies == 0, 'cannot call main when async dependencies remain! (listen on Module["onRuntimeInitialized"])');
    assert(__ATPRERUN__.length == 0, 'cannot call main when preRun functions remain to be called');


    entryFunction = Module['_eval_to_ints_tamgu'];

    string_to_array = provideCharactersAsInts(code);

    the_size = provide_integers(2);

    var int_result;

    try {
        int_result = entryFunction(string_to_array, code.length, the_size, displaymode);
    }
    catch (e) {
        Module._free(string_to_array);
        Module._free(the_size);
        return handleException(e);
    }
    sz = decode_size(the_size);
    if (sz < 0) {
        sz *= -1;
        Module._free(string_to_array);
        str = decode(int_result, sz)
        throw new Error(str);
    }
    Module._free(string_to_array);
    return decode_int(int_result, sz);
}

function callEvalToStrings(code, displaymode) {
    assert(runDependencies == 0, 'cannot call main when async dependencies remain! (listen on Module["onRuntimeInitialized"])');
    assert(__ATPRERUN__.length == 0, 'cannot call main when preRun functions remain to be called');


    entryFunction = Module['_eval_to_strings_tamgu'];

    string_to_array = provideCharactersAsInts(code);

    the_size = provide_integers(2);

    try {

        string_result = entryFunction(string_to_array, code.length, the_size, displaymode);
        sz = decode_size(the_size);
        if (sz < 0) {
            sz *= -1;
            Module._free(string_to_array);
            str = decode(string_result, sz)
            throw new Error(str);
        }
        characters = decode_int(string_result, sz);
        let str_vector = [];
        let s = "";
        for (var i = 0; i < sz; i++) {
            if (characters[i] == 0) {
                str_vector.push(s);
                s = "";
            }
            else
                s += String.fromCharCode(characters[i]);
        }
        if (s != "")
            str_vector.push(s);

        Module._free(string_to_array);
        return str_vector
    }
    catch (e) {
        Module._free(string_to_array);
        Module._free(the_size);
        return handleException(e);
    }
}

//Important, values should have been created with provideIntegers
function callSetqInts(a_variable, vals, nb) {
    assert(runDependencies == 0, 'cannot call main when async dependencies remain! (listen on Module["onRuntimeInitialized"])');
    assert(__ATPRERUN__.length == 0, 'cannot call main when preRun functions remain to be called');


    if (values instanceof Int32Array == false)
        throw new Error("Expecting a Int32Array");

    entryFunction = Module['_eval_setq_ints_tamgu'];

    string_to_array = provideCharactersAsInts(a_variable);

    values = provideIntegers(nb, vals);

    var ret;

    try {
        ret = entryFunction(string_to_array, a_variable.length, values, nb);
    }
    catch (e) {
        Module._free(value_to_array);
        Module._free(values);
       return handleException(e);
    }

    Module._free(values);
    if (ret < 0) {
        ret *= -1;
        str = decode(string_to_array, ret)
       throw new Error(str);
    }
    Module._free(value_to_array);
    return true;
}

function callSetqFloats(a_variable, vals, nb) {
    assert(runDependencies == 0, 'cannot call main when async dependencies remain! (listen on Module["onRuntimeInitialized"])');
    assert(__ATPRERUN__.length == 0, 'cannot call main when preRun functions remain to be called');

    if (values instanceof Float64Array == false)
        throw new Error("Expecting a Int32Array");


    entryFunction = Module['_eval_setq_floats_tamgu'];

    string_to_array = provideCharactersAsInts(a_variable);
    var ret;

    values = provideFloats(nb, vals);

    try {
        ret = entryFunction(string_to_array, a_variable.length, values, nb);
    }
    catch (e) {
        Module._free(value_to_array);
        Module._free(values);
        return handleException(e);
    }

    Module._free(values);
    if (ret < 0) {
        ret *= -1;
        str = decode(string_to_array, ret)
        throw new Error(str);
    }
    Module._free(value_to_array);
    return true;
}

//value is a int
function callSetqInt(a_variable, value) {
    assert(runDependencies == 0, 'cannot call main when async dependencies remain! (listen on Module["onRuntimeInitialized"])');
    assert(__ATPRERUN__.length == 0, 'cannot call main when preRun functions remain to be called');


    entryFunction = Module['_eval_setq_int_tamgu'];

    string_to_array = provideCharactersAsInts(a_variable);
    var ret;

    try {
        ret = entryFunction(string_to_array, a_variable.length, value);
    }
    catch (e) {
        Module._free(value_to_array);
        return handleException(e);
    }

    if (ret < 0) {
        ret *= -1;
        str = decode(string_to_array, ret)
        throw new Error(str);
    }
    Module._free(value_to_array);
    return true;
}

//value is float
function callSetqFloat(a_variable, value) {
    assert(runDependencies == 0, 'cannot call main when async dependencies remain! (listen on Module["onRuntimeInitialized"])');
    assert(__ATPRERUN__.length == 0, 'cannot call main when preRun functions remain to be called');


    entryFunction = Module['_eval_setq_float_tamgu'];

    string_to_array = provideCharactersAsInts(a_variable);
    var ret;

    try {

        ret = entryFunction(string_to_array, a_variable.length, value);
    }
    catch (e) {
        Module._free(value_to_array);
        return handleException(e);
    }

    if (ret < 0) {
        ret *= -1;
        str = decode(string_to_array, ret)
        throw new Error(str);
    }
    Module._free(value_to_array);
    return true;
}

//value is a string
function callSetqString(a_variable, value) {
    assert(runDependencies == 0, 'cannot call main when async dependencies remain! (listen on Module["onRuntimeInitialized"])');
    assert(__ATPRERUN__.length == 0, 'cannot call main when preRun functions remain to be called');


    if (typeof value != "string") {
        throw new Error("Expecting a string as input");
    }

    entryFunction = Module['_eval_setq_string_tamgu'];

    string_to_array = provideCharactersAsInts(a_variable);
    value_to_array = provideCharactersAsInts(value);

    var ret;

    try {

        ret = entryFunction(string_to_array, a_variable.length, value_to_array, value.length);
    }
    catch (e) {
        Module._free(value_to_array);
        Module._free(string_to_array);
        return handleException(e);
    }

    Module._free(value_to_array);
    if (ret < 0) {
        ret *= -1;
        str = decode(string_to_array, ret)
        throw new Error(str);
    }
    Module._free(string_to_array);
    return true;
}

function callEvalAsInt(code, displaymode) {
    assert(runDependencies == 0, 'cannot call main when async dependencies remain! (listen on Module["onRuntimeInitialized"])');
    assert(__ATPRERUN__.length == 0, 'cannot call main when preRun functions remain to be called');


    entryFunction = Module['_eval_as_int_tamgu'];

    string_to_array = provideCharactersAsInts(code);
    var ret;
    try {

        ret = entryFunction(string_to_array,  code.length, Math.max(20, code.length), displaymode);
    }
    catch (e) {
        Module._free(string_to_array);
        return handleException(e);
    }
    if (ret < 0) {
        ret *= -1;
        str = decode(string_to_array, ret)
        throw new Error(str);
    }
    Module._free(string_to_array);
    return ret;
}

function callEvalAsFloat(code, displaymode) {
    assert(runDependencies == 0, 'cannot call main when async dependencies remain! (listen on Module["onRuntimeInitialized"])');
    assert(__ATPRERUN__.length == 0, 'cannot call main when preRun functions remain to be called');


    entryFunction = Module['_eval_as_float_tamgu'];

    string_to_array = provideCharactersAsInts(code);
    var ret;
    try {

        ret = entryFunction(string_to_array, code.length, Math.max(20, code.length), displaymode);
    }
    catch (e) {
        Module._free(string_to_array);
       return handleException(e);
    }
    if (ret < 0) {
        ret *= -1;
        str = decode(string_to_array, ret)
        throw new Error(str);
    }
    Module._free(string_to_array);
    return ret;
}

function callIndent(code) {
    assert(runDependencies == 0, 'cannot call main when async dependencies remain! (listen on Module["onRuntimeInitialized"])');
    assert(__ATPRERUN__.length == 0, 'cannot call main when preRun functions remain to be called');

    entryFunction = Module['_eval_indent_tamgu'];

    string_to_array = provideCharactersAsInts(code);
    the_size = provide_integers(2);

    try {
        string_result = entryFunction(string_to_array, code.length, the_size);
        Module._free(string_to_array);
        sz = decode_size(the_size);
        return decode(string_result, sz);
    }
    catch (e) {
        Module._free(string_to_array);
        Module._free(the_size);
        return handleException(e);
    }
}

function callMethodInfo() {
    assert(runDependencies == 0, 'cannot call main when async dependencies remain! (listen on Module["onRuntimeInitialized"])');
    assert(__ATPRERUN__.length == 0, 'cannot call main when preRun functions remain to be called');

    entryFunction = Module['_eval_method_info_tamgu'];
    
    the_size = provide_integers(2);

    try {
        string_result = entryFunction(the_size);
        sz = decode_size(the_size);
        return decode(string_result, sz);
    }
    catch (e) {
        Module._free(the_size);
        return handleException(e);
    }
}

function callMethodInfoWithCode(code) {
    assert(runDependencies == 0, 'cannot call main when async dependencies remain! (listen on Module["onRuntimeInitialized"])');
    assert(__ATPRERUN__.length == 0, 'cannot call main when preRun functions remain to be called');

    entryFunction = Module['_eval_method_info_tamgu_with_code'];
    string_to_array = provideCharactersAsInts(code);

    the_size = provide_integers(2);

    try {
        string_result = entryFunction(string_to_array, code.length, the_size);
        Module._free(string_to_array);
        sz = decode_size(the_size);
        return decode(string_result, sz);
    }
    catch (e) {
        Module._free(string_to_array);
        Module._free(the_size);
        return handleException(e);
    }
}

function callMethodEventString(code) {
    assert(runDependencies == 0, 'cannot call main when async dependencies remain! (listen on Module["onRuntimeInitialized"])');
    assert(__ATPRERUN__.length == 0, 'cannot call main when preRun functions remain to be called');

    entryFunction = Module['_eval_event_string'];
    the_size = provide_integers(2);

    try {
        string_result = entryFunction(the_size);
        sz = decode_size(the_size);
        return decode(string_result, sz);
    }
    catch (e) {
        Module._free(the_size);
        return handleException(e);
    }
}

function callCheckCode(code, str_vector, pos_vector) {
    assert(runDependencies == 0, 'cannot call main when async dependencies remain! (listen on Module["onRuntimeInitialized"])');
    assert(__ATPRERUN__.length == 0, 'cannot call main when preRun functions remain to be called');

    entryFunction = Module['_eval_checkcode_tamgu'];

    string_to_array = provideCharactersAsInts(code);
    the_size = provide_integers(2);

    try {
        string_result = entryFunction(string_to_array, code.length, the_size);
        Module._free(string_to_array);
        sz = decode_size(the_size);
        decode_as_strings(string_result, sz, str_vector, pos_vector);
    }
    catch (e) {
        Module._free(string_to_array);
        Module._free(the_size);
        return handleException(e);
    }
}

function callEval(code, displaymode) {
    assert(runDependencies == 0, 'cannot call main when async dependencies remain! (listen on Module["onRuntimeInitialized"])');
    assert(__ATPRERUN__.length == 0, 'cannot call main when preRun functions remain to be called');


    entryFunction = Module['_eval_tamgu'];

    string_to_array = provideCharactersAsInts(code);

    the_size = provide_integers(2);
    clean = false;
    try {

        var result = entryFunction(string_to_array, code.length, the_size, displaymode);
        Module._free(string_to_array);
        sz = decode_size(the_size);
        clean = true;
        if (sz < 0) {
            sz *= -1;
            str = decode(result, sz)
            throw new Error(str);    
        }
        return decode(result, sz);
    }
    catch (e) {
        if (clean == false) {
            Module._free(string_to_array);
            Module._free(the_size);
        }
        return handleException(e);
    }
}

//-----------------------------------------------------------------
