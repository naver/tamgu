/*
*  NAVER LABS Europe
*  Copyright NAVER LABS
*  All Rights Reserved
*
*/

/**
* Native implementation for Java API to access the Tamgu C++ API.
*/

#define MULTIGLOBALTAMGU

#include "jni.h"
#include "com_naver_jtamgu_JTamgu.h"

#include "tamgu.h"
#include "tamguversion.h"
#include "math.h"
#include "globaltamgu.h"
#include "compilecode.h"
#include "tamgustring.h"
#include "tamgudecimal.h"

#include "jtamgu.h"

/**
* Converts the built-in Unicode representation of a java string into a UTF-8 c++ string
* Take care if the java input string contains non ASCII characters,
    * utf8 represents them with more than one byte.
* if the input string contains only ASCII characters, the output UTF-8 string
*  is the same than the input string.
* @param str the java string
* @return the utf8 c++ string
*/
string jstringToString(JNIEnv* env, jstring str) {
    /*
    * The GetStringUTFChars function retrieves 8-bit characters from a 16-bit jstring
    * using the Unicode Transformation Format (UTF). UTF represents Unicode as a string
    * of 8 or 16 bit characters without losing any information.
    * The third parameter GetStringUTFChars results the result JNI_TRUE if it made
    * a local copy of the jstring or JNI_FALSE otherwise.
    */
    jboolean iscopy;
    const char* strChars = env->GetStringUTFChars(str, &iscopy);
    string result = string(strChars);
    //cout <<"jstsringToString result = " <<result <<endl;
    // now the the JVM can free the memory used by jstring str
    env->ReleaseStringUTFChars(str, strChars);
    return result;
}

/**
* Constructs a new java.lang.String object from an array of UTF-8 characters.
* IMPORTANT NOTE: to avoid memory leak use deleteLocalJStringRef() to ask JVM to free memeory
*
* @param the pointer to a utf8 c++ string
* @return the java string
*
*/
//VECTA<jstring> listjstring;

jstring jstringFromChars(JNIEnv* env, const char *chars) {
    jstring jstr = env->NewStringUTF(chars);
    //    listjstring.ajoute(jstr);
    return jstr;
}

/**
* converts a c++ string to a java string
* IMPORTANT NOTE: to avoid memory leak use deleteLocalJStringRef() to ask JVM to free memeory
* @param the utf8 c++ string
* @return the java string
*
*/
jstring jstringFromString(JNIEnv* env, string& str) {
    return jstringFromChars(env, str.c_str());
}

static void displayerror(JNIEnv *env, string message) {
    jclass Exception = env->FindClass("java/lang/Exception");
    env->ThrowNew(Exception,message.c_str());
}

/// ---------------------------------------------------------------------------------------------------
/// TAMGU Interpreter access Methods
/// ---------------------------------------------------------------------------------------------------



inline Tamgu* Getvariable(TamguCode* codespace, short idcode, short name) {
    return globalTamgu->Getmaindeclaration(name, 0);
}

inline bool Checkvariable(TamguCode* codespace, short idcode, short name) {
    return globalTamgu->threads[0].variables.check(name);
}

TamguCode* CheckTamgu(JNIEnv *env, jint handler, jint idcode) {
    if (!TamguSelectglobal(handler)) {
        string value = "Cannot find any program loaded for this handler";
        displayerror(env, value);
        return NULL;
    }

    TamguCode* tamgucode = TamguCodeSpace(idcode);
    if (tamgucode == NULL) {
        string value = "This handler does not match an existing Tamgu space";
        displayerror(env, value);
        return NULL;
    }
    
    return tamgucode;
}


TamguCode* GetTamgu(JNIEnv *env, jint handler, string& value, short idcode = 0) {
    if (!TamguSelectglobal(handler)) {
        value = "Cannot find any program loaded for this handler";
        return NULL;
    }

    TamguCode* tamgucode = TamguCodeSpace(idcode);
    if (tamgucode == NULL) {
        value = "This handler does not match an existing Tamgu space";
        return NULL;
    }
    
    return tamgucode;
}

/**
* Execute and load a Tamgu program
* @param filename is the Tamgu program pathname
* @param args is a string in which each argument is seperated from the others with a space
*/
JNIEXPORT jint JNICALL Java_com_naver_jtamgu_JTamgu_LoadProgramImplementation(JNIEnv *env, jobject obj, jstring filename, jstring args) {

    string nameOfFile = jstringToString(env, filename);
    string theargs = jstringToString(env, args);
    short idspace = TamguCreateGlobal(10);
    
    stringstream message;
    TamguSetArguments(theargs);
    short idcode = -1;
    try {
        idcode = TamguLoad(nameOfFile);
        if (idcode == -1) {
            message << "Unknown file: " << nameOfFile << endl;
            displayerror(env, message.str());
            return -1;
        }
        
        if (!TamguLoading(idcode)) {
            message << TamguErrorMessage() << endl;
            displayerror(env, message.str());
            return -1;
        }
    }
    catch (TamguRaiseError* m) {
        message << m->message << " in " << m->filename << " at line: " << m->left << endl;
        displayerror(env, message.str());
        return -1;
    }

    return idspace;
}

/**
 * Execute and load a Tamgu program as a String
 * @param code is the Tamgu program as a String
 * @param args is a string in which each argument is seperated from the others with a space
 */
static long nbfilename = 0;
JNIEXPORT jint JNICALL Java_com_naver_jtamgu_JTamgu_LoadStringProgramImplementation(JNIEnv *env, jobject obj, jstring code, jstring args) {

    string jcode = jstringToString(env, code);
    string theargs = jstringToString(env, args);

    short idspace = TamguCreateGlobal(10);

#ifdef WIN32
    string name = "C:\\JAVA";
#else
    string name = "/JAVA";
#endif

    char buff[20];
    sprintf(buff, "_%d", nbfilename);
    nbfilename++;
    name += buff;
    stringstream message;
    TamguSetArguments(theargs);
    short idcode = -1;
    idcode = TamguCompileMain(jcode, name);
    if (idcode == -1) {
        message << TamguErrorMessage() << endl;
        displayerror(env, message.str());
        return -1;
    }
    
    if (!TamguLoading(idcode)) {
        message << TamguErrorMessage() << endl;
        displayerror(env, message.str());
        return -1;
    }
    
    return idspace;
}

JNIEXPORT jboolean JNICALL Java_com_naver_jtamgu_JTamgu_CheckProgramImplementation(JNIEnv *env, jobject obj, jstring code, jstring args, jobjectArray messages, jobjectArray lines, jobjectArray positions) {

    string jcode = jstringToString(env, code);
    string theargs = jstringToString(env, args);

    short idspace = TamguCreateGlobal(10);

#ifdef WIN32
    string name = "C:\\JAVA";
#else
    string name = "/JAVA";
#endif

    char buff[20];
    sprintf(buff, "_%d", nbfilename);
    nbfilename++;
    name += buff;
    TamguSetArguments(theargs);
    short idcode = -1;
    std::vector<TamguFullError*> errors;
    if (TamguCheckCompile(jcode, name, errors))
        return true;
    
    long nb = errors.size();
    JavaIterationListString jmess(env, messages, 0);
    JavaIterationListInteger jlines(env, lines, 0);
    JavaIterationListInteger jpos(env, positions, 0);
    
    for (long i = 0; i < nb; i++) {
        jmess.Set(i, errors[i]->error);
        jlines.Set(i, errors[i]->line);
        jpos.Set(i, errors[i]->posbinary);
        delete errors[i];
    }
    
    return false;
}


/**
 * Execute and load a Tamgu program as a String
 * @param code is the Tamgu program as a String
 * @param args is a string in which each argument is seperated from the others with a space
 */
JNIEXPORT jint JNICALL Java_com_naver_jtamgu_JTamgu_LoadAStringProgramImplementation(JNIEnv *env, jobject obj, jint handler, jstring code) {

    if (!TamguSelectglobal(handler)) {
        string value = "Cannot find any program loaded for this handler";
        displayerror(env, value);
        return -1;
    }

    string jcode = jstringToString(env, code);
#ifdef WIN32
    string name = "C:\\JAVA";
#else
    string name = "/JAVA";
#endif

    char buff[20];
    sprintf(buff, "_%d", nbfilename);
    nbfilename++;
    name += buff;
    stringstream message;
    short idcode = TamguCompileNewSpace(jcode, name);
    if (idcode == -1) {
        message << TamguErrorMessage() << endl;
        displayerror(env, message.str());
        return -1;
    }
    
    if (!TamguLoading(idcode)) {
        message << TamguErrorMessage() << endl;
        displayerror(env, message.str());
        return -1;
    }

    return idcode;
}



/**
* Execute and load a Tamgu function
* @param funcname is the Tamgu function name
* @param parameters is a vector of string, each corresponding to a parameter of the function...
*/
JNIEXPORT jstring JNICALL Java_com_naver_jtamgu_JTamgu_ExecuteFunctionImplementation(JNIEnv *env, jobject obj, jint handler, jstring funcname, jobjectArray parameters) {

    string nameOfFunction = jstringToString(env, funcname);
    vector<Tamgu*> params;
    int stringCount = env->GetArrayLength(parameters);
    string value;
    jstring element;
    bool theerror = false;

    TamguCode* tamgucode = GetTamgu(env, handler, value);
    if (tamgucode == NULL) {
        element = jstringFromString(env, value);
        displayerror(env, value);
        return element;
    }
    
    for (int i = 0; i < stringCount; i++) {
        element = (jstring)env->GetObjectArrayElement(parameters, i);
        value = jstringToString(env, element);
        params.push_back(new Tamgustring(value));
    }

    Tamgu* resultat = TamguExecute(tamgucode, nameOfFunction, params);
	
    if (globalTamgu->Error(0) || resultat->isError()) {
        if (resultat->isError()) {
            value = resultat->String();
        }
        else
            value = globalTamgu->Errorstring(0);
        globalTamgu->Cleanerror(0);
        displayerror(env, value);
    }
    else {
        value = resultat->String();
        resultat->Resetreference();
    }
	TamguReleaseglobal(handler);
	
    element = jstringFromString(env, value);
    return element;
}

/**
* Execute and load a Tamgu function
* @param funcname is the Tamgu function name
* @param parameters is a vector of string, each corresponding to a parameter of the function...
*/
JNIEXPORT jobjectArray JNICALL Java_com_naver_jtamgu_JTamgu_ExecuteFunctionArrayImplementation(JNIEnv *env, jobject obj, jint handler, jstring funcname, jobjectArray parameters) {

    string nameOfFunction = jstringToString(env, funcname);
    vector<Tamgu*> params;
    int stringCount = env->GetArrayLength(parameters);
    string value;
    jstring element;
    bool theerror = false;

    jobjectArray ret;

    TamguCode* tamgucode = GetTamgu(env, handler, value);
    if (tamgucode == NULL) {
        ret = (jobjectArray)env->NewObjectArray(1, env->FindClass("java/lang/String"), env->NewStringUTF(""));
        element = jstringFromString(env, value);
        env->SetObjectArrayElement(ret, 0, element);
        env->DeleteLocalRef(element);
        displayerror(env, value);
        return ret;
    }

    for (int i = 0; i < stringCount; i++) {
        element = (jstring)env->GetObjectArrayElement(parameters, i);
        value = jstringToString(env, element);
        params.push_back(new Tamgustring(value));
    }

	//JtamguLocking _lock;

    Tamgu* resultat = TamguExecute(tamgucode, nameOfFunction, params);
	
    if (globalTamgu->Error(0)  || resultat->isError()) {
        if (resultat->isError()) {
            value = resultat->String();
        }
        else
            value = globalTamgu->Errorstring(0);
        ret = (jobjectArray)env->NewObjectArray(1, env->FindClass("java/lang/String"), env->NewStringUTF(""));
        element = jstringFromString(env, value);
        env->SetObjectArrayElement(ret, 0, element);
        env->DeleteLocalRef(element);
        globalTamgu->Cleanerror(0);
        displayerror(env, value);
        TamguReleaseglobal(handler);
        return ret;
    }

	TamguReleaseglobal(handler);
	
    if (resultat->isVectorContainer()) {
        ret = (jobjectArray)env->NewObjectArray(resultat->Size(), env->FindClass("java/lang/String"), env->NewStringUTF(""));
        for (long i = 0; i < resultat->Size(); i++) {
            value = resultat->getstring(i);
            element = jstringFromString(env, value);
            env->SetObjectArrayElement(ret, i, element);
            env->DeleteLocalRef(element);
        }
    }

    resultat->Resetreference();
    return ret;
}

/**
* Execute and load a Tamgu function
* @param funcname is the Tamgu function name
* @param parameters is a vector of string, each corresponding to a parameter of the function...
*/
JNIEXPORT jstring JNICALL Java_com_naver_jtamgu_JTamgu_ExecuteSpaceFunctionImplementation(JNIEnv *env, jobject obj, jint handler, jint idcode, jstring funcname, jobjectArray parameters) {

    string nameOfFunction = jstringToString(env, funcname);
    vector<Tamgu*> params;
    int stringCount = env->GetArrayLength(parameters);
    string value;
    jstring element;
    bool theerror = false;

    TamguCode* tamgucode = GetTamgu(env, handler, value, idcode);
    if (tamgucode == NULL) {
        element = jstringFromString(env, value);
        displayerror(env, value);
        return element;
    }
    
    for (int i = 0; i < stringCount; i++) {
        element = (jstring)env->GetObjectArrayElement(parameters, i);
        value = jstringToString(env, element);
        params.push_back(new Tamgustring(value));
    }

    Tamgu* resultat = TamguExecute(tamgucode, nameOfFunction, params);
    
    if (globalTamgu->Error(0) || resultat->isError()) {
        if (resultat->isError()) {
            value = resultat->String();
        }
        else
            value = globalTamgu->Errorstring(0);
        globalTamgu->Cleanerror(0);
        displayerror(env, value);
    }
    else {
        value = resultat->String();
        resultat->Resetreference();
    }
    TamguReleaseglobal(handler);
    
    element = jstringFromString(env, value);
    return element;
}

/**
* Execute and load a Tamgu function
* @param funcname is the Tamgu function name
* @param parameters is a vector of string, each corresponding to a parameter of the function...
*/
JNIEXPORT jobjectArray JNICALL Java_com_naver_jtamgu_JTamgu_ExecuteSpaceFunctionArrayImplementation(JNIEnv *env, jobject obj, jint handler, jint idcode, jstring funcname, jobjectArray parameters) {

    string nameOfFunction = jstringToString(env, funcname);
    vector<Tamgu*> params;
    int stringCount = env->GetArrayLength(parameters);
    string value;
    jstring element;
    bool theerror = false;

    jobjectArray ret;

    TamguCode* tamgucode = GetTamgu(env, handler, value, idcode);
    if (tamgucode == NULL) {
        ret = (jobjectArray)env->NewObjectArray(1, env->FindClass("java/lang/String"), env->NewStringUTF(""));
        element = jstringFromString(env, value);
        env->SetObjectArrayElement(ret, 0, element);
        env->DeleteLocalRef(element);
        displayerror(env, value);
        return ret;
    }

    for (int i = 0; i < stringCount; i++) {
        element = (jstring)env->GetObjectArrayElement(parameters, i);
        value = jstringToString(env, element);
        params.push_back(new Tamgustring(value));
    }

    //JtamguLocking _lock;

    Tamgu* resultat = TamguExecute(tamgucode, nameOfFunction, params);
    
    if (globalTamgu->Error(0)  || resultat->isError()) {
        if (resultat->isError()) {
            value = resultat->String();
        }
        else
            value = globalTamgu->Errorstring(0);
        ret = (jobjectArray)env->NewObjectArray(1, env->FindClass("java/lang/String"), env->NewStringUTF(""));
        element = jstringFromString(env, value);
        env->SetObjectArrayElement(ret, 0, element);
        env->DeleteLocalRef(element);
        globalTamgu->Cleanerror(0);
        displayerror(env, value);
        TamguReleaseglobal(handler);
        return ret;
    }

    TamguReleaseglobal(handler);
    
    if (resultat->isVectorContainer()) {
        ret = (jobjectArray)env->NewObjectArray(resultat->Size(), env->FindClass("java/lang/String"), env->NewStringUTF(""));
        for (long i = 0; i < resultat->Size(); i++) {
            value = resultat->getstring(i);
            element = jstringFromString(env, value);
            env->SetObjectArrayElement(ret, i, element);
            env->DeleteLocalRef(element);
        }
    }

    resultat->Resetreference();
    return ret;
}
/**
* Clean memory for all TamguGlobal Objects
*/
JNIEXPORT jint JNICALL Java_com_naver_jtamgu_JTamgu_CleanAllImplementation(JNIEnv *env, jobject obj) {
	TamguCleanAllGlobals();
	return 1;
}


JNIEXPORT jint JNICALL Java_com_naver_jtamgu_JTamgu_CleanErrorsImplementation(JNIEnv *env, jobject obj, jint handler) {
    if (!TamguSelectglobal(handler)) {
        string value = "Cannot find any program loaded for this handler";
        displayerror(env, value);
        return -1;
    }
    globalTamgu->Cleanerror(0);
    return 1;
}

/**
* Clean memory for a Global
*/
JNIEXPORT jint JNICALL Java_com_naver_jtamgu_JTamgu_CleanImplementation(JNIEnv *env, jobject obj, jint handler) {
	if (TamguDeleteGlobal(handler))
		return 1;
	return 0;
}

JNIEXPORT jstring JNICALL Java_com_naver_jtamgu_JTamgu_TamguVersionImplementation(JNIEnv *env, jobject obj) {
    string version = TamguVersion();
    jstring element = jstringFromString(env, version);
    return element;
}

void StoreGlobalValues(JNIEnv *env, TamguGlobal* global, TamguCode* tamgucode, short idcode, jobject map) {

    jclass c_map = env->GetObjectClass(map);
    jmethodID id_entrySet = env->GetMethodID(c_map, "entrySet", "()Ljava/util/Set;");
    
    jclass c_entryset = env->FindClass("java/util/Set");
    jmethodID id_iterator = env->GetMethodID(c_entryset, "iterator", "()Ljava/util/Iterator;");
    
    jclass c_iterator = env->FindClass("java/util/Iterator");
    jmethodID id_hasNext = env->GetMethodID(c_iterator, "hasNext", "()Z");
    jmethodID id_next = env->GetMethodID(c_iterator, "next", "()Ljava/lang/Object;");

    jclass c_entry = env->FindClass("java/util/Map$Entry");
    jmethodID id_getKey = env->GetMethodID(c_entry, "getKey", "()Ljava/lang/Object;");
    jmethodID id_getValue = env->GetMethodID(c_entry, "getValue", "()Ljava/lang/Object;");
    
    jclass c_string = env->FindClass("java/lang/String");
    jmethodID id_toString = env->GetMethodID(c_string, "toString", "()Ljava/lang/String;");
    
    jobject obj_entrySet = env->CallObjectMethod(map, id_entrySet);

    jobject obj_iterator = env->CallObjectMethod(obj_entrySet, id_iterator);

    bool hasNext = (bool) env->CallBooleanMethod(obj_iterator, id_hasNext);

    short name;
    Tamgu* var;
    Tamgu* value;
    while(hasNext) {
        jobject entry = env->CallObjectMethod(obj_iterator, id_next);

        jobject okey = env->CallObjectMethod(entry, id_getKey);
        jobject ovalue = env->CallObjectMethod(entry, id_getValue);

        jstring jstrKey = (jstring) env->CallObjectMethod(okey, id_toString);
        jstring jstrValue = (jstring) env->CallObjectMethod(ovalue, id_toString);

        string strKey = jstringToString(env, jstrKey);
        string strValue = jstringToString(env, jstrValue);

        if (idcode) {
            strKey += "&";
            strKey += std::to_string(idcode);
        }
        
        //First we need to find the associate global variable
        name = globalTamgu->Getid(strKey);
        
        if (Checkvariable(tamgucode, idcode, name)) {
            //Then we need to access the variable itself...
            var = Getvariable(tamgucode, idcode, name);
            //Then we need to put the value in it...
            value = globalTamgu->Providestring(strValue);
            var->Put(aNULL, value, 0);
            value->Release();
        }
        env->DeleteLocalRef(okey);
        env->DeleteLocalRef(ovalue);
        env->DeleteLocalRef(jstrKey);
        env->DeleteLocalRef(jstrValue);
        hasNext = (bool) env->CallBooleanMethod(obj_iterator, id_hasNext);
    }
}

void RetrieveGlobalValues(JNIEnv *env, TamguGlobal* global, TamguCode* tamgucode, short idcode, jobject map) {

    jclass c_map = env->GetObjectClass(map);
    jmethodID id_entrySet = env->GetMethodID(c_map, "entrySet", "()Ljava/util/Set;");
    
    jclass c_entryset = env->FindClass("java/util/Set");
    jmethodID id_iterator = env->GetMethodID(c_entryset, "iterator", "()Ljava/util/Iterator;");
    
    jclass c_iterator = env->FindClass("java/util/Iterator");
    jmethodID id_hasNext = env->GetMethodID(c_iterator, "hasNext", "()Z");
    jmethodID id_next = env->GetMethodID(c_iterator, "next", "()Ljava/lang/Object;");

    jclass c_entry = env->FindClass("java/util/Map$Entry");
    jmethodID id_getKey = env->GetMethodID(c_entry, "getKey", "()Ljava/lang/Object;");
    jmethodID id_setValue = env->GetMethodID(c_entry, "setValue", "(Ljava/lang/Object;)Ljava/lang/Object;");
    
    
    jclass c_string = env->FindClass("java/lang/String");
    jmethodID id_toString = env->GetMethodID(c_string, "toString", "()Ljava/lang/String;");
    
    jobject obj_entrySet = env->CallObjectMethod(map, id_entrySet);

    jobject obj_iterator = env->CallObjectMethod(obj_entrySet, id_iterator);

    bool hasNext = (bool) env->CallBooleanMethod(obj_iterator, id_hasNext);

    short name;
    Tamgu* var;
    Tamgu* value;
    while(hasNext) {
        jobject entry = env->CallObjectMethod(obj_iterator, id_next);

        jobject okey = env->CallObjectMethod(entry, id_getKey);
        jstring jstrKey = (jstring) env->CallObjectMethod(okey, id_toString);

        string strKey = jstringToString(env, jstrKey);

        if (idcode) {
            strKey += "&";
            strKey += std::to_string(idcode);
        }

        //First we need to find the associate global variable
        name = globalTamgu->Getid(strKey);
        
        //Then we need to access the variable itself...
        if (Checkvariable(tamgucode, idcode, name)) {
            var = Getvariable(tamgucode, idcode, name);
            string strValue = var->JSonString();
            
            jstring element = jstringFromString(env, strValue);
            env->CallObjectMethod(entry, id_setValue, element);
            env->DeleteLocalRef(element);
        }
        env->DeleteLocalRef(okey);
        env->DeleteLocalRef(jstrKey);
        hasNext = (bool) env->CallBooleanMethod(obj_iterator, id_hasNext);
    }
}

/**
* Execute and load a Tamgu function
* @param funcname is the Tamgu function name
* @param parameters is a vector of string, each corresponding to a parameter of the function...
*/
JNIEXPORT jstring JNICALL Java_com_naver_jtamgu_JTamgu_ExecuteFunctionImplementationWithGlobals(JNIEnv *env, jobject obj, jint handler, jint idcode, jobject globalvariables, jstring funcname, jobjectArray parameters) {

    string nameOfFunction = jstringToString(env, funcname);
    vector<Tamgu*> params;
    int stringCount = env->GetArrayLength(parameters);
    string value;
    jstring element;
    bool theerror = false;

    TamguCode* tamgucode = GetTamgu(env, handler, value, idcode);
    if (tamgucode == NULL) {
        element = jstringFromString(env, value);
        displayerror(env, value);
        return element;
    }

    for (int i = 0; i < stringCount; i++) {
        element = (jstring)env->GetObjectArrayElement(parameters, i);
        value = jstringToString(env, element);
        params.push_back(new Tamgustring(value));
    }

    StoreGlobalValues(env, globalTamgu, tamgucode, idcode, globalvariables);
    Tamgu* resultat = TamguExecute(tamgucode, nameOfFunction, params);
    
    if (globalTamgu->Error(0) || resultat->isError()) {
        if (resultat->isError()) {
            value = resultat->String();
        }
        else
            value = globalTamgu->Errorstring(0);
        globalTamgu->Cleanerror(0);
        displayerror(env, value);
    }
    else {
        RetrieveGlobalValues(env, globalTamgu, tamgucode, idcode, globalvariables);
        value = resultat->String();
        resultat->Resetreference();
    }
    TamguReleaseglobal(handler);
    
    element = jstringFromString(env, value);
    return element;
}

//---------------------------------------------------------------------------------------
//Iterating on Java Structures
//---------------------------------------------------------------------------------------

Tamgu* JavaSize(void* jiv) {
    return globalTamgu->Provideint(((JavaIteration*)jiv)->count);
}

Tamgu* JavaGet(void* jiv, long i) {
    return ((JavaIteration*)jiv)->Get(i);
}

Tamgu* JavaSet(void* jiv, long i, Tamgu* v) {
    return ((JavaIteration*)jiv)->Set(i, v);
}

Tamgu* JavaNext(void* jiv) {
    return ((JavaIteration*)jiv)->Next();
}

Tamgu* JavaValue(void* jiv) {
    return ((JavaIteration*)jiv)->Value();
}

Tamgu* JavaEnd(void* jiv) {
    return ((JavaIteration*)jiv)->isEnd();
}

Tamgu* JavaBegin(void* jiv) {
    return ((JavaIteration*)jiv)->Begin();
}

Tamgu* JavaIterator(void* jiv) {
    return ((JavaIteration*)jiv)->Iterate();
}

Tamgu* JavaDelete(void* j_iv) {
    JavaIteration* jiv = (JavaIteration*)j_iv;
    delete jiv;
    return aTRUE;
}

JNIEXPORT jboolean JNICALL Java_com_naver_jtamgu_JTamgu_Setlongiteratorimplementation(JNIEnv *env, jobject obj, jint handler, jint idcode, jstring variable_name, jlongArray values) {
    TamguCode* tamgucode = CheckTamgu(env, handler, idcode);
    if (tamgucode == NULL)
        return false;

    size_t count = env->GetArrayLength(values);

    string str_name = jstringToString(env, variable_name);
    
    if (idcode) {
        str_name += "&";
        str_name += std::to_string(idcode);
    }

    short name = globalTamgu->Getid(str_name);
    
    //Then we need to access the variable itself...
    if (Checkvariable(tamgucode, idcode, name)) {
        Tamgu* var = Getvariable(tamgucode, idcode, name);
        JavaIterationLong* jiv = new JavaIterationLong(env, (jobjectArray)values, count);
        TamguJavaIteration* it = new TamguJavaIteration(jiv, JavaBegin, JavaNext, JavaEnd, JavaIterator, JavaValue, JavaDelete, JavaSize, JavaGet, JavaSet);
        var->Put(aNULL, it, 0);
        return true;
    }
    string message = "Could not find a Tamgu variable with that name: ";
    message += str_name;
    displayerror(env, message);
    return false;
}

JNIEXPORT jboolean JNICALL Java_com_naver_jtamgu_JTamgu_Setfloatiteratorimplementation(JNIEnv *env, jobject obj, jint handler, jint idcode, jstring variable_name, jfloatArray values) {
    TamguCode* tamgucode = CheckTamgu(env, handler, idcode);
    if (tamgucode == NULL)
        return false;

    size_t count = env->GetArrayLength(values);

    string str_name = jstringToString(env, variable_name);

    if (idcode) {
        str_name += "&";
        str_name += std::to_string(idcode);
    }

    short name = globalTamgu->Getid(str_name);
    
    //Then we need to access the variable itself...
    if (Checkvariable(tamgucode, idcode, name)) {
        Tamgu* var = Getvariable(tamgucode, idcode, name);
        JavaIterationFloat* jiv = new JavaIterationFloat(env, (jobjectArray)values, count);
        TamguJavaIteration* it = new TamguJavaIteration(jiv, JavaBegin, JavaNext, JavaEnd, JavaIterator, JavaValue, JavaDelete, JavaSize, JavaGet, JavaSet);
        var->Put(aNULL, it, 0);
        return true;
    }
    string message = "Could not find a Tamgu variable with that name: ";
    message += str_name;
    displayerror(env, message);
    return false;
}

JNIEXPORT jboolean JNICALL Java_com_naver_jtamgu_JTamgu_Setdoubleiteratorimplementation(JNIEnv *env, jobject obj, jint handler, jint idcode, jstring variable_name, jdoubleArray values) {
    TamguCode* tamgucode = CheckTamgu(env, handler, idcode);
    if (tamgucode == NULL)
        return false;

    size_t count = env->GetArrayLength(values);

    string str_name = jstringToString(env, variable_name);

    if (idcode) {
        str_name += "&";
        str_name += std::to_string(idcode);
    }

    short name = globalTamgu->Getid(str_name);
    
    //Then we need to access the variable itself...
    if (Checkvariable(tamgucode, idcode, name)) {
        Tamgu* var = Getvariable(tamgucode, idcode, name);
        JavaIterationDouble* jiv = new JavaIterationDouble(env, (jobjectArray)values, count);
        TamguJavaIteration* it = new TamguJavaIteration(jiv, JavaBegin, JavaNext, JavaEnd, JavaIterator, JavaValue, JavaDelete, JavaSize, JavaGet, JavaSet);
        var->Put(aNULL, it, 0);
        return true;
    }
    string message = "Could not find a Tamgu variable with that name: ";
    message += str_name;
    displayerror(env, message);
    return false;
}

JNIEXPORT jboolean JNICALL Java_com_naver_jtamgu_JTamgu_Setstringiteratorimplementation(JNIEnv *env, jobject obj, jint handler, jint idcode, jstring variable_name, jobjectArray values) {
    TamguCode* tamgucode = CheckTamgu(env, handler, idcode);
    if (tamgucode == NULL)
        return false;

    size_t count = env->GetArrayLength(values);

    string str_name = jstringToString(env, variable_name);

    if (idcode) {
        str_name += "&";
        str_name += std::to_string(idcode);
    }

    short name = globalTamgu->Getid(str_name);
    
    //Then we need to access the variable itself...
    if (Checkvariable(tamgucode, idcode, name)) {
        Tamgu* var = Getvariable(tamgucode, idcode, name);
        JavaIterationString* jiv = new JavaIterationString(env, values, count);
        TamguJavaIteration* it = new TamguJavaIteration(jiv, JavaBegin, JavaNext, JavaEnd, JavaIterator, JavaValue, JavaDelete, JavaSize, JavaGet, JavaSet);
        var->Put(aNULL, it, 0);
        return true;
    }
    
    string message = "Could not find a Tamgu variable with that name: ";
    message += str_name;
    displayerror(env, message);
    return false;
}


JNIEXPORT jboolean JNICALL Java_com_naver_jtamgu_JTamgu_SetListFloatiteratorimplementation(JNIEnv *env, jobject obj, jint handler, jint idcode, jstring variable_name, jobject values) {
    TamguCode* tamgucode = CheckTamgu(env, handler, idcode);
    if (tamgucode == NULL)
        return false;

    jsize count = env->GetArrayLength((jobjectArray)values);

    string str_name = jstringToString(env, variable_name);

    if (idcode) {
        str_name += "&";
        str_name += std::to_string(idcode);
    }

    short name = globalTamgu->Getid(str_name);
    
    //Then we need to access the variable itself...
    if (Checkvariable(tamgucode, idcode, name)) {
        Tamgu* var = Getvariable(tamgucode, idcode, name);
        JavaIterationListFloat* jiv = new JavaIterationListFloat(env, values, count);
        TamguJavaIteration* it = new TamguJavaIteration(jiv, JavaBegin, JavaNext, JavaEnd, JavaIterator, JavaValue, JavaDelete, JavaSize, JavaGet, JavaSet);
        var->Put(aNULL, it, 0);
        return true;
    }
    
    string message = "Could not find a Tamgu variable with that name: ";
    message += str_name;
    displayerror(env, message);
    return false;
}

JNIEXPORT jboolean JNICALL Java_com_naver_jtamgu_JTamgu_SetListIntiteratorimplementation(JNIEnv *env, jobject obj, jint handler, jint idcode, jstring variable_name, jobject values) {
    TamguCode* tamgucode = CheckTamgu(env, handler, idcode);
    if (tamgucode == NULL)
        return false;

    jsize count = env->GetArrayLength((jobjectArray)values);

    string str_name = jstringToString(env, variable_name);

    if (idcode) {
        str_name += "&";
        str_name += std::to_string(idcode);
    }

    short name = globalTamgu->Getid(str_name);
    
    //Then we need to access the variable itself...
    if (Checkvariable(tamgucode, idcode, name)) {
        Tamgu* var = Getvariable(tamgucode, idcode, name);
        JavaIterationListInteger* jiv = new JavaIterationListInteger(env, values, count);
        TamguJavaIteration* it = new TamguJavaIteration(jiv, JavaBegin, JavaNext, JavaEnd, JavaIterator, JavaValue, JavaDelete, JavaSize, JavaGet, JavaSet);
        var->Put(aNULL, it, 0);
        return true;
    }
    
    string message = "Could not find a Tamgu variable with that name: ";
    message += str_name;
    displayerror(env, message);
    return false;
}

JNIEXPORT jboolean JNICALL Java_com_naver_jtamgu_JTamgu_SetListLongiteratorimplementation(JNIEnv *env, jobject obj, jint handler, jint idcode, jstring variable_name, jobject values) {
    TamguCode* tamgucode = CheckTamgu(env, handler, idcode);
    if (tamgucode == NULL)
        return false;

    jsize count = env->GetArrayLength((jobjectArray)values);

    string str_name = jstringToString(env, variable_name);

    if (idcode) {
        str_name += "&";
        str_name += std::to_string(idcode);
    }

    short name = globalTamgu->Getid(str_name);
    
    //Then we need to access the variable itself...
    if (Checkvariable(tamgucode, idcode, name)) {
        Tamgu* var = Getvariable(tamgucode, idcode, name);
        JavaIterationListLong* jiv = new JavaIterationListLong(env, values, count);
        TamguJavaIteration* it = new TamguJavaIteration(jiv, JavaBegin, JavaNext, JavaEnd, JavaIterator, JavaValue, JavaDelete, JavaSize, JavaGet, JavaSet);
        var->Put(aNULL, it, 0);
        return true;
    }
    
    string message = "Could not find a Tamgu variable with that name: ";
    message += str_name;
    displayerror(env, message);
    return false;
}

JNIEXPORT jboolean JNICALL Java_com_naver_jtamgu_JTamgu_SetListDoubleiteratorimplementation(JNIEnv *env, jobject obj, jint handler, jint idcode, jstring variable_name, jobject values) {
    TamguCode* tamgucode = CheckTamgu(env, handler, idcode);
    if (tamgucode == NULL)
        return false;

    jsize count = env->GetArrayLength((jobjectArray)values);

    string str_name = jstringToString(env, variable_name);

    if (idcode) {
        str_name += "&";
        str_name += std::to_string(idcode);
    }

    short name = globalTamgu->Getid(str_name);
    
    //Then we need to access the variable itself...
    if (Checkvariable(tamgucode, idcode, name)) {
        Tamgu* var = Getvariable(tamgucode, idcode, name);
        JavaIterationListDouble* jiv = new JavaIterationListDouble(env, values, count);
        TamguJavaIteration* it = new TamguJavaIteration(jiv, JavaBegin, JavaNext, JavaEnd, JavaIterator, JavaValue, JavaDelete, JavaSize, JavaGet, JavaSet);
        var->Put(aNULL, it, 0);
        return true;
    }
    
    string message = "Could not find a Tamgu variable with that name: ";
    message += str_name;
    displayerror(env, message);
    return false;
}

JNIEXPORT jboolean JNICALL Java_com_naver_jtamgu_JTamgu_SetListStringiteratorimplementation(JNIEnv *env, jobject obj, jint handler, jint idcode, jstring variable_name, jobject values) {
    TamguCode* tamgucode = CheckTamgu(env, handler, idcode);
    if (tamgucode == NULL)
        return false;

    jsize count = env->GetArrayLength((jobjectArray)values);

    string str_name = jstringToString(env, variable_name);

    if (idcode) {
        str_name += "&";
        str_name += std::to_string(idcode);
    }

    short name = globalTamgu->Getid(str_name);
    
    //Then we need to access the variable itself...
    if (Checkvariable(tamgucode, idcode, name)) {
        Tamgu* var = Getvariable(tamgucode, idcode, name);
        JavaIterationListString* jiv = new JavaIterationListString(env, values, count);
        TamguJavaIteration* it = new TamguJavaIteration(jiv, JavaBegin, JavaNext, JavaEnd, JavaIterator, JavaValue, JavaDelete, JavaSize, JavaGet, JavaSet);
        var->Put(aNULL, it, 0);
        return true;
    }
    
    string message = "Could not find a Tamgu variable with that name: ";
    message += str_name;
    displayerror(env, message);
    return false;
}
