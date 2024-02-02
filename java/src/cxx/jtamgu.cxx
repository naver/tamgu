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
#include <signal.h>
#include "tamguversion.h"
#include "math.h"
#include "globaltamgu.h"
#include "compilecode.h"
#include "tamgustring.h"
#include "tamgudecimal.h"
#include "tamgushort.h"

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
    return env->NewStringUTF(chars);
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

jstring jstringFromAString(JNIEnv* env, string str) {
    return jstringFromChars(env, str.c_str());
}

static void displayerror(JNIEnv *env, string message) {
    jclass Exception = env->FindClass("java/lang/Exception");
    env->ThrowNew(Exception,message.c_str());
    env->DeleteLocalRef(Exception); // release local reference
}

/// ---------------------------------------------------------------------------------------------------
/// Handling JNI crashes
/// ---------------------------------------------------------------------------------------------------
static JavaVM* g_jvm = NULL;
static bool already_sent = false;

// Signal handler
void segfault_handler(int sig, siginfo_t *si, void *uc) {
    // Get JNIEnv* corresponding to current thread
    if (g_jvm != NULL && !already_sent) {
        already_sent = true;
        JNIEnv* env = NULL;
        g_jvm->AttachCurrentThread((void**)&env, NULL);
        TamguStopAll();
        // Throw a Java exception
        jclass cls = env->FindClass("java/lang/Exception");
        if (cls != NULL) {
            env->ThrowNew(cls, "Caught SIGSEGV");
        }

        // Detach current thread
        g_jvm->DetachCurrentThread();
    }
}


JNIEXPORT void JNICALL Java_com_naver_jtamgu_JTamgu_TamguCatchJNICrashImplementation(JNIEnv *env, jobject obj) {
    // Store the JVM
    already_sent = false;
    if (g_jvm == NULL) {        
        env->GetJavaVM(&g_jvm);

        // Register signal handler
        struct sigaction sa;
        sa.sa_flags = SA_SIGINFO;
        sa.sa_sigaction = segfault_handler;
        sigemptyset(&sa.sa_mask);
        if (sigaction(SIGSEGV, &sa, NULL) == -1) {
            perror("sigaction");
        }
    }
}

/// ---------------------------------------------------------------------------------------------------
/// TAMGU Interpreter access Methods
/// ---------------------------------------------------------------------------------------------------
inline Tamgu* Getvariable(TamguCode* codespace, short idcode, short name) {
    return codespace->global->Getmaindeclaration(name, 0);
}

inline bool Checkvariable(TamguCode* codespace, short idcode, short name) {
    return codespace->global->threads[0].variables.check(name);
}

inline Tamgu* Getvariable(TamguGlobal* global, short idcode, short name) {
    return global->Getmaindeclaration(name, 0);
}

inline bool Checkvariable(TamguGlobal* global, short idcode, short name) {
    return global->threads[0].variables.check(name);
}

TamguCode* CheckTamgu(JNIEnv *env, jint handler, jint idcode) {
    TamguCode* tamgucode = TamguCodeSpace(handler, idcode);
    if (tamgucode == NULL) {
        string value = "This handler does not match an existing Tamgu space";
        displayerror(env, value);
        return NULL;
    }
    globalTamgu = tamgucode->global;
    return tamgucode;
}


TamguCode* GetTamguCode(JNIEnv *env, jint handler, string& value, short idcode = 0) {
    TamguCode* tamgucode = TamguCodeSpace(handler, idcode);
    if (tamgucode == NULL) {
        value = "This handler does not match an existing Tamgu space";
        return NULL;
    }
    globalTamgu = tamgucode->global;
    return tamgucode;
}


/**
* Create a GlobalTamgu object
*/
JNIEXPORT jint JNICALL Java_com_naver_jtamgu_JTamgu_CreateTamgu(JNIEnv *env, jobject obj) {
    return TamguCreateGlobal(10);
}


/**
 * Set Garbage Mode On/Off
*/
 
 JNIEXPORT void JNICALL Java_com_naver_jtamgu_JTamgu_TamguSetGarbageModeImplementation(JNIEnv *env, jobject obj, jint mode) {
    Set_keep_track_garbage((bool)mode); 
}

/**
* Count the number of instances in memory
*/
JNIEXPORT jint JNICALL Java_com_naver_jtamgu_JTamgu_CountinstancesImplementation(JNIEnv *env, jobject obj) {
    return Countinstances();
}


/**
* Count the number of active instances
*/
JNIEXPORT jint JNICALL Java_com_naver_jtamgu_JTamgu_CountactiveinstancesImplementation(JNIEnv *env, jobject obj) {
    return Count_active_instances();
}

/**
* Count the number of instances in memory
*/
JNIEXPORT jint JNICALL Java_com_naver_jtamgu_JTamgu_MaxcountinstancesImplementation(JNIEnv *env, jobject obj) {
    return Maxcountinstances();
}


/**
* Count the number of active instances
*/
JNIEXPORT jint JNICALL Java_com_naver_jtamgu_JTamgu_MaxcountactiveinstancesImplementation(JNIEnv *env, jobject obj) {
    return Max_count_active_instances();
}

/**
* Execute and load a Tamgu program
* @param filename is the Tamgu program pathname
* @param args is a string in which each argument is seperated from the others with a space
*/
JNIEXPORT jint JNICALL Java_com_naver_jtamgu_JTamgu_LoadProgramImplementation(JNIEnv *env, jobject obj, jstring filename, jstring args) {

    string nameOfFile = jstringToString(env, filename);
    string theargs = jstringToString(env, args);
    short handler = TamguCreateGlobal(10);
    
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

    return handler;
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

    short handler = TamguCreateGlobal(1);

    stringstream os;

#ifdef WIN32
    os << "C:\\JAVA" << nbfilename;
#else
    os << "/JAVA" << nbfilename;
#endif

    string name = os.str();
    nbfilename++;

    TamguSetArguments(theargs);
    short idcode = -1;
    idcode = TamguCompileMain(jcode, name);

    os.str("");
    os.clear();

    if (idcode == -1) {
        os << TamguErrorMessage() << endl;
        displayerror(env, os.str());
        return -1;
    }
    
    if (!TamguLoading(idcode)) {
        os << TamguErrorMessage() << endl;
        displayerror(env, os.str());
        return -1;
    }
    
    return handler;
}

JNIEXPORT jboolean JNICALL Java_com_naver_jtamgu_JTamgu_CheckProgramImplementation(JNIEnv *env, jobject obj, jstring code, jstring args, jobjectArray messages, jobjectArray lines, jobjectArray positions) {

    string jcode = jstringToString(env, code);
    string theargs = jstringToString(env, args);

    //short idspace = TamguCreateGlobal(10);

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
    
    string value;
    long nb = errors.size();
    JavaIterationListString jmess(env, globalTamgu, messages, 0);
    JavaIterationListInteger jlines(env, globalTamgu, lines, 0);
    JavaIterationListInteger jpos(env, globalTamgu, positions, 0);
    
    for (long i = 0; i < nb; i++) {
        jmess.Set(i, errors[i]->error);
        jlines.Set(i, errors[i]->line);
        jpos.Set(i, errors[i]->posbinary);
        delete errors[i];
    }
    
    return false;
}

JNIEXPORT jboolean JNICALL Java_com_naver_jtamgu_JTamgu_GetErrorImplementation(JNIEnv *env, jobject obj,
                                                                               jint handler,
                                                                               jobjectArray messages,
                                                                               jobjectArray files,
                                                                               jobjectArray lines) {
    string value;
    jstring element;
    bool theerror = false;

    TamguCode* tamgucode = GetTamguCode(env, handler, value);
    if (tamgucode == NULL)
        return false;

    std::vector<string> c_errors;
    std::vector<string> c_files;
    std::vector<long> c_lines;
    if (!TamguErrorVector(tamgucode->global, c_errors, c_files, c_lines))
        return true;
    
    long nb = c_errors.size();
    JavaIterationListString jmess(env, tamgucode->global, messages, 0);
    JavaIterationListString jfiles(env, tamgucode->global, files, 0);
    JavaIterationListInteger jlines(env, tamgucode->global, lines, 0);

    for (long i = 0; i < nb; i++) {
        jmess.Set(i, c_errors[i]);
        jfiles.Set(i, c_files[i]);
        jlines.Set(i, c_lines[i]);
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

    TamguCode* tamgucode = GetTamguCode(env, handler, value);
    if (tamgucode == NULL) {
        element = jstringFromString(env, value);
        displayerror(env, value);
        return element;
    }
    
    TamguGlobal* global = tamgucode->global;
    for (int i = 0; i < stringCount; i++) {
        element = (jstring)env->GetObjectArrayElement(parameters, i);
        value = jstringToString(env, element);
        params.push_back(new Tamgustring(value));
    }

    Tamgu* resultat = TamguExecutionCode(tamgucode, nameOfFunction, params);
	
    if (global->Error(0) || resultat->isError()) {
        if (resultat->isError()) {
            value = resultat->String();
        }
        else
            value = global->Errorstring(0);
        global->Cleanerror(0);
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

    TamguCode* tamgucode = GetTamguCode(env, handler, value);
    if (tamgucode == NULL) {
        ret = (jobjectArray)env->NewObjectArray(1, env->FindClass("java/lang/String"), env->NewStringUTF(""));
        element = jstringFromString(env, value);
        env->SetObjectArrayElement(ret, 0, element);
        env->DeleteLocalRef(element);
        displayerror(env, value);
        return ret;
    }

    TamguGlobal* global = tamgucode->global;

    for (int i = 0; i < stringCount; i++) {
        element = (jstring)env->GetObjectArrayElement(parameters, i);
        value = jstringToString(env, element);
        params.push_back(new Tamgustring(value));
    }

	//JtamguLocking _lock;

    Tamgu* resultat = TamguExecutionCode(tamgucode, nameOfFunction, params);
	
    if (global->Error(0)  || resultat->isError()) {
        if (resultat->isError()) {
            value = resultat->String();
        }
        else
            value = global->Errorstring(0);
        ret = (jobjectArray)env->NewObjectArray(1, env->FindClass("java/lang/String"), env->NewStringUTF(""));
        element = jstringFromString(env, value);
        env->SetObjectArrayElement(ret, 0, element);
        env->DeleteLocalRef(element);
        global->Cleanerror(0);
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

    TamguCode* tamgucode = GetTamguCode(env, handler, value, idcode);
    if (tamgucode == NULL) {
        element = jstringFromString(env, value);
        displayerror(env, value);
        return element;
    }
    TamguGlobal* global = tamgucode->global;
    for (int i = 0; i < stringCount; i++) {
        element = (jstring)env->GetObjectArrayElement(parameters, i);
        value = jstringToString(env, element);
        params.push_back(new Tamgustring(value));
    }

    Tamgu* resultat = TamguExecutionCode(tamgucode, nameOfFunction, params);
    
    if (global->Error(0) || resultat->isError()) {
        if (resultat->isError()) {
            value = resultat->String();
        }
        else
            value = global->Errorstring(0);
        global->Cleanerror(0);
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

    TamguCode* tamgucode = GetTamguCode(env, handler, value, idcode);
    if (tamgucode == NULL) {
        ret = (jobjectArray)env->NewObjectArray(1, env->FindClass("java/lang/String"), env->NewStringUTF(""));
        element = jstringFromString(env, value);
        env->SetObjectArrayElement(ret, 0, element);
        env->DeleteLocalRef(element);
        displayerror(env, value);
        return ret;
    }
    TamguGlobal* global = tamgucode->global;
    for (int i = 0; i < stringCount; i++) {
        element = (jstring)env->GetObjectArrayElement(parameters, i);
        value = jstringToString(env, element);
        params.push_back(new Tamgustring(value));
    }

    //JtamguLocking _lock;

    Tamgu* resultat = TamguExecutionCode(tamgucode, nameOfFunction, params);
    
    if (global->Error(0)  || resultat->isError()) {
        if (resultat->isError()) {
            value = resultat->String();
        }
        else
            value = global->Errorstring(0);
        ret = (jobjectArray)env->NewObjectArray(1, env->FindClass("java/lang/String"), env->NewStringUTF(""));
        element = jstringFromString(env, value);
        env->SetObjectArrayElement(ret, 0, element);
        env->DeleteLocalRef(element);
        global->Cleanerror(0);
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
    TamguGlobal* global = getGlobalTamgu(handler);
    if (global == NULL) {
        string value = "Cannot find any program loaded for this handler";
        displayerror(env, value);
        return -1;
    }
    global->Cleanerror(0);
    return 1;
}

/**
* Clean memory for a Global
*/
JNIEXPORT jint JNICALL Java_com_naver_jtamgu_JTamgu_CleanImplementation(JNIEnv *env, jobject obj, jint handler) {
	if (TamguDeleteGlobalinThread(handler))
		return 1;
	return 0;
}

/**
* Stop a process
*/
JNIEXPORT jint JNICALL Java_com_naver_jtamgu_JTamgu_StopExecution(JNIEnv *env, jobject obj, jint handler) {
	if (TamguStop(handler))
		return 1;
	return 0;
}

/**
* Clean memory for a Global
*/
JNIEXPORT jint JNICALL Java_com_naver_jtamgu_JTamgu_CleanSpaceImplementation(JNIEnv *env, jobject obj, jint handler, jint idspace) {
	if (TamguDeleteSpace(handler, idspace))
		return 1;
	return 0;
}

JNIEXPORT void JNICALL Java_com_naver_jtamgu_JTamgu_ScanGarbageImplementation(JNIEnv *env, jobject obj, jobject java_issues) {
    hmap<std::string, long> issues;
    Garbaging(issues);
    jclass hashMapClass = env->FindClass("java/util/HashMap");

    jmethodID constructor = env->GetMethodID(hashMapClass, "<init>", "()V");
    jobject javaIssuesObj = env->NewObject(hashMapClass, constructor);
    jmethodID putMethod = env->GetMethodID(hashMapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
  
    jclass cl = env->FindClass("java/lang/Integer");
    jclass intClass = (jclass)env->NewGlobalRef(cl);
    jmethodID intValue = env->GetMethodID(intClass, "intValue", "()I");
    jmethodID create_an_object = env->GetMethodID(intClass, "<init>", "(I)V");

    // Iterate through the 'issues' map and add its contents to 'java_issues'
    for (const auto& entry : issues) {
        jstring keyStr = jstringFromAString(env, entry.first);
        jint valueInt = entry.second;
        
        // Convert the integer value to a Java Integer object
        jobject value = env->NewObject(intClass, create_an_object, (jint)valueInt);
        
        // Add the key-value pair to the Java HashMap
        env->CallObjectMethod(java_issues, putMethod, keyStr, value);
        
        // Release local references
        env->DeleteLocalRef(keyStr);
        env->DeleteLocalRef(value);
  }

    env->DeleteLocalRef(hashMapClass);
    env->DeleteLocalRef(javaIssuesObj);
    env->DeleteLocalRef(cl);
    env->DeleteLocalRef(intClass);

}

JNIEXPORT void JNICALL Java_com_naver_jtamgu_JTamgu_ScanGarbagelongImplementation(JNIEnv *env, jobject obj, jobject java_issues) {
    hmap<long, long> issues;
    Garbaging(issues);
    jclass hashMapClass = env->FindClass("java/util/HashMap");

    jmethodID constructor = env->GetMethodID(hashMapClass, "<init>", "()V");
    jobject javaIssuesObj = env->NewObject(hashMapClass, constructor);
    jmethodID putMethod = env->GetMethodID(hashMapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
  
    jclass cl = env->FindClass("java/lang/Integer");
    jclass intClass = (jclass)env->NewGlobalRef(cl);
    env->DeleteLocalRef(cl);
    jmethodID intValue = env->GetMethodID(intClass, "intValue", "()I");
    jmethodID create_an_object = env->GetMethodID(intClass, "<init>", "(I)V");

    // Iterate through the 'issues' map and add its contents to 'java_issues'
    for (const auto& entry : issues) {
        jint keyInt = entry.first;
        jint valueInt = entry.second;
        
        // Convert the integer value to a Java Integer object
        jobject key = env->NewObject(intClass, create_an_object, (jint)keyInt);
        jobject value = env->NewObject(intClass, create_an_object, (jint)valueInt);

        // Add the key-value pair to the Java HashMap
        env->CallObjectMethod(java_issues, putMethod, key, value);
        
        // Release local references
        env->DeleteLocalRef(key);
        env->DeleteLocalRef(value);
    }

    env->DeleteLocalRef(hashMapClass);
    env->DeleteLocalRef(javaIssuesObj);
    env->DeleteLocalRef(cl);
    env->DeleteLocalRef(intClass);
}

JNIEXPORT void JNICALL Java_com_naver_jtamgu_JTamgu_TamguIdleImplementation(JNIEnv *env, jobject obj, jint diff, jobject jidles) {
    vector<long> idles;
    TamguIdle(diff, idles);

    if (idles.size()) {
        jsize count = env->GetArrayLength((jobjectArray)jidles);
        JavaIterationListInteger* li = new JavaIterationListInteger(env, NULL, jidles, count);
        for (long i = 0; i < idles.size(); i++) {
            li->Push(idles[i]);
        }
        delete li;
    }
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
        name = global->Getid(strKey);
        
        if (Checkvariable(global, idcode, name)) {
            //Then we need to access the variable itself...
            var = Getvariable(global, idcode, name);
            //Then we need to put the value in it...
            value = global->Providestring(strValue);
            var->Put(aNULL, value, 0);
            value->Release();
        }
        env->DeleteLocalRef(entry);
        env->DeleteLocalRef(okey);
        env->DeleteLocalRef(ovalue);
        env->DeleteLocalRef(jstrKey);
        env->DeleteLocalRef(jstrValue);
        hasNext = (bool) env->CallBooleanMethod(obj_iterator, id_hasNext);
    }

    env->DeleteLocalRef(c_map);
    env->DeleteLocalRef(c_entryset);
    env->DeleteLocalRef(c_iterator);
    env->DeleteLocalRef(c_entry);
    env->DeleteLocalRef(c_string);
    env->DeleteLocalRef(obj_entrySet);
    env->DeleteLocalRef(obj_iterator);
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
        name = global->Getid(strKey);
        
        //Then we need to access the variable itself...
        if (Checkvariable(tamgucode, idcode, name)) {
            var = Getvariable(tamgucode, idcode, name);
            string strValue = var->JSonString();
            
            jstring element = jstringFromString(env, strValue);
            env->CallObjectMethod(entry, id_setValue, element);
            env->DeleteLocalRef(element);
        }
        env->DeleteLocalRef(entry);
        env->DeleteLocalRef(okey);
        env->DeleteLocalRef(jstrKey);
        hasNext = (bool) env->CallBooleanMethod(obj_iterator, id_hasNext);
    }

    env->DeleteLocalRef(c_map);
    env->DeleteLocalRef(c_entryset);
    env->DeleteLocalRef(c_iterator);
    env->DeleteLocalRef(c_entry);
    env->DeleteLocalRef(c_string);
    env->DeleteLocalRef(obj_entrySet);
    env->DeleteLocalRef(obj_iterator);
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

    TamguCode* tamgucode = GetTamguCode(env, handler, value, idcode);
    if (tamgucode == NULL) {
        element = jstringFromString(env, value);
        displayerror(env, value);
        return element;
    }

    TamguGlobal* global = tamgucode->global;

    for (int i = 0; i < stringCount; i++) {
        element = (jstring)env->GetObjectArrayElement(parameters, i);
        value = jstringToString(env, element);
        params.push_back(new Tamgustring(value));
    }

    StoreGlobalValues(env, global, tamgucode, idcode, globalvariables);
    Tamgu* resultat = TamguExecutionCode(tamgucode, nameOfFunction, params);
    
    if (global->Error(0) || resultat->isError()) {
        if (resultat->isError()) {
            value = resultat->String();
        }
        else
            value = global->Errorstring(0);
        global->Cleanerror(0);
        displayerror(env, value);
    }
    else {
        RetrieveGlobalValues(env, global, tamgucode, idcode, globalvariables);
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
    return ((JavaIteration*)jiv)->global->Provideint(((JavaIteration*)jiv)->count);
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
    Tamgu* v = ((JavaIteration*)jiv)->Value();
    return v;
}

Tamgu* JavaEnd(void* jiv) {
    return ((JavaIteration*)jiv)->isEnd();
}

Tamgu* JavaBegin(void* jiv) {
    return ((JavaIteration*)jiv)->Begin();
}

Tamgu* JavaClear(void* jiv) {
    return ((JavaIteration*)jiv)->Clear();
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

    short name = tamgucode->global->Getid(str_name);
    
    //Then we need to access the variable itself...
    if (Checkvariable(tamgucode, idcode, name)) {
        Tamgu* var = Getvariable(tamgucode, idcode, name);
        JavaIterationLong* jiv = new JavaIterationLong(env, tamgucode->global, (jobjectArray)values, count);
        TamguJavaIteration* it = new TamguJavaIteration(jiv, JavaBegin, JavaNext, JavaEnd, JavaIterator, JavaValue, JavaDelete, JavaSize, JavaGet, JavaSet, JavaClear);
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

    short name = tamgucode->global->Getid(str_name);
    
    //Then we need to access the variable itself...
    if (Checkvariable(tamgucode, idcode, name)) {
        Tamgu* var = Getvariable(tamgucode, idcode, name);
        JavaIterationFloat* jiv = new JavaIterationFloat(env, tamgucode->global, (jobjectArray)values, count);
        TamguJavaIteration* it = new TamguJavaIteration(jiv, JavaBegin, JavaNext, JavaEnd, JavaIterator, JavaValue, JavaDelete, JavaSize, JavaGet, JavaSet, JavaClear);
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

    short name = tamgucode->global->Getid(str_name);
    
    //Then we need to access the variable itself...
    if (Checkvariable(tamgucode, idcode, name)) {
        Tamgu* var = Getvariable(tamgucode, idcode, name);
        JavaIterationDouble* jiv = new JavaIterationDouble(env, tamgucode->global, (jobjectArray)values, count);
        TamguJavaIteration* it = new TamguJavaIteration(jiv, JavaBegin, JavaNext, JavaEnd, JavaIterator, JavaValue, JavaDelete, JavaSize, JavaGet, JavaSet, JavaClear);
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

    short name = tamgucode->global->Getid(str_name);
    
    //Then we need to access the variable itself...
    if (Checkvariable(tamgucode, idcode, name)) {
        Tamgu* var = Getvariable(tamgucode, idcode, name);
        JavaIterationString* jiv = new JavaIterationString(env, tamgucode->global, values, count);
        TamguJavaIteration* it = new TamguJavaIteration(jiv, JavaBegin, JavaNext, JavaEnd, JavaIterator, JavaValue, JavaDelete, JavaSize, JavaGet, JavaSet, JavaClear);
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

    short name = tamgucode->global->Getid(str_name);
    
    //Then we need to access the variable itself...
    if (Checkvariable(tamgucode, idcode, name)) {
        Tamgu* var = Getvariable(tamgucode, idcode, name);
        JavaIterationListFloat* jiv = new JavaIterationListFloat(env, tamgucode->global, values, count);
        TamguJavaIteration* it = new TamguJavaIteration(jiv, JavaBegin, JavaNext, JavaEnd, JavaIterator, JavaValue, JavaDelete, JavaSize, JavaGet, JavaSet, JavaClear);
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

    short name = tamgucode->global->Getid(str_name);
    
    //Then we need to access the variable itself...
    if (Checkvariable(tamgucode, idcode, name)) {
        Tamgu* var = Getvariable(tamgucode, idcode, name);
        JavaIterationListInteger* jiv = new JavaIterationListInteger(env, tamgucode->global, values, count);
        TamguJavaIteration* it = new TamguJavaIteration(jiv, JavaBegin, JavaNext, JavaEnd, JavaIterator, JavaValue, JavaDelete, JavaSize, JavaGet, JavaSet, JavaClear);
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

    short name = tamgucode->global->Getid(str_name);
    
    //Then we need to access the variable itself...
    if (Checkvariable(tamgucode, idcode, name)) {
        Tamgu* var = Getvariable(tamgucode, idcode, name);
        JavaIterationListLong* jiv = new JavaIterationListLong(env, tamgucode->global, values, count);
        TamguJavaIteration* it = new TamguJavaIteration(jiv, JavaBegin, JavaNext, JavaEnd, JavaIterator, JavaValue, JavaDelete, JavaSize, JavaGet, JavaSet, JavaClear);
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

    short name = tamgucode->global->Getid(str_name);
    
    //Then we need to access the variable itself...
    if (Checkvariable(tamgucode, idcode, name)) {
        Tamgu* var = Getvariable(tamgucode, idcode, name);
        JavaIterationListDouble* jiv = new JavaIterationListDouble(env, tamgucode->global, values, count);
        TamguJavaIteration* it = new TamguJavaIteration(jiv, JavaBegin, JavaNext, JavaEnd, JavaIterator, JavaValue, JavaDelete, JavaSize, JavaGet, JavaSet, JavaClear);
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

    short name = tamgucode->global->Getid(str_name);
    
    //Then we need to access the variable itself...
    if (Checkvariable(tamgucode, idcode, name)) {
        Tamgu* var = Getvariable(tamgucode, idcode, name);
        JavaIterationListString* jiv = new JavaIterationListString(env, tamgucode->global, values, count);
        TamguJavaIteration* it = new TamguJavaIteration(jiv, JavaBegin, JavaNext, JavaEnd, JavaIterator, JavaValue, JavaDelete, JavaSize, JavaGet, JavaSet, JavaClear);
        var->Put(aNULL, it, 0);
        return true;
    }
    
    string message = "Could not find a Tamgu variable with that name: ";
    message += str_name;
    displayerror(env, message);
    return false;
}

