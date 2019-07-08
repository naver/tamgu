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

static ThreadLock jLock;

class JtamguLocking {
private:
	std::lock_guard<std::recursive_mutex>* g;

public:
	JtamguLocking() {
		g = new std::lock_guard<std::recursive_mutex>(*jLock.lock);
	}

	~JtamguLocking() {
        delete g;
	}
};

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

/**
* Execute and load a Tamgu program
* @param filename is the Tamgu program pathname
* @param args is a string in which each argument is seperated from the others with a space
*/
JNIEXPORT jint JNICALL Java_com_naver_jtamgu_JTamgu_LoadProgramImplementation(JNIEnv *env, jobject obj, jstring filename, jstring args) {

    string nameOfFile = jstringToString(env, filename);
    string theargs = jstringToString(env, args);
#ifdef WIN32
	short idglobal = TamguCreateGlobal(1000);
#else
    short idglobal = TamguCreateGlobal(500);
#endif
    
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

    return idglobal;
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

#ifdef WIN32
    short idglobal = TamguCreateGlobal(1000);
    string name = "C:\\JAVA";
#else
    short idglobal = TamguCreateGlobal(500);
    string name = "/JAVA";
#endif

    char buff[20];
    sprintf(buff, "_%d", nbfilename);
    nbfilename++;
    name += buff;
    stringstream message;
    TamguSetArguments(theargs);
    short idcode = -1;
    idcode = TamguCompile(jcode, name, false);
    if (idcode == -1) {
        message << TamguErrorMessage() << endl;
        displayerror(env, message.str());
        return -1;
    }
    return idglobal;
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

    for (int i = 0; i < stringCount; i++) {
        element = (jstring)env->GetObjectArrayElement(parameters, i);
        value = jstringToString(env, element);
        params.push_back(new Tamgustring(value));
    }

	if (!TamguSelectglobal(handler)) {
		value = "Cannot find any program loaded for this handler";
        displayerror(env, value);
		return NULL;
	}

    TamguCode* tamgucode = TamguCodeSpace(0);
    if (tamgucode == NULL) {
        value = "This handler does not match an existing Tamgu space";
        displayerror(env, value);
        return NULL;
    }

    Tamgu* resultat = TamguExecute(tamgucode, nameOfFunction, params);
	
    if (globalTamgu->Error(0)) {
        value = globalTamgu->Errorstring(0);
		TamguReleaseglobal(handler);
        displayerror(env, value);
        return NULL;
    }

	TamguReleaseglobal(handler);
	
    value = resultat->String();
    resultat->Resetreference();
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


    for (int i = 0; i < stringCount; i++) {
        element = (jstring)env->GetObjectArrayElement(parameters, i);
        value = jstringToString(env, element);
        params.push_back(new Tamgustring(value));
    }

	jobjectArray ret;
	//JtamguLocking _lock;

	if (!TamguSelectglobal(handler)) {
        value = "Cannot find any program loaded for this handler";
        displayerror(env, value);
		return NULL;
	}

	TamguCode* tamgucode = TamguCodeSpace(0);
    if (tamgucode == NULL) {
        value = "This handler does not match an existing Tamgu space";
        displayerror(env, value);
        return NULL;
    }

    Tamgu* resultat = TamguExecute(tamgucode, nameOfFunction, params);
	
    if (globalTamgu->Error(0)) {
        value = globalTamgu->Errorstring(0);
		TamguReleaseglobal(handler);
        displayerror(env, value);
        return NULL;
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
