/*
*  NAVER LABS Europe
*  Copyright NAVER LABS
*  All Rights Reserved
*
*/

/**
* Native implementation for Java API to access the Tamgu C++ API.
*/

string jstringToString(JNIEnv* env, jstring str);
jstring jstringFromChars(JNIEnv* env, const char *chars);
jstring jstringFromString(JNIEnv* env, string& str);

//---------------------------------------------------------------------------------------
//Iterating on Java Structures
//--------------------------------------------------------------------------------------

class JavaIteration {
public:
    TamguGlobal* global;
    JavaVM* jvm;
    JNIEnv* current_env;

    jobject object;
    size_t count;
    jint version;
    long i;

    JavaIteration(JNIEnv* e, TamguGlobal* g, jobject o, size_t c)  {
        current_env = e;
        e->GetJavaVM(&jvm);
        version = e->GetVersion();
        global = g;
        object = (jobjectArray)e->NewGlobalRef(o);
        count = c;
    }

    ~JavaIteration() {
        current_env->DeleteLocalRef(object);
    }

    JNIEnv* getEnv() {
        JNIEnv* env = NULL;   
        jint result = jvm->GetEnv((void**)&env,version);
        jvm->AttachCurrentThread((void**)&env, NULL);
        return env;
    }

    void freeEnv() {
        jvm->DetachCurrentThread();
    }

    Tamgu* isEnd() {
        if (i >= count)
            return aTRUE;
        return aFALSE;
    }

    Tamgu* Begin() {
        if (!count)
            return aFALSE;
        i = 0;
        return aTRUE;
    }

    Tamgu* Next() {
        i++;
        return aTRUE;
    }

    virtual Tamgu* Value() {
        return aNOELEMENT;
    }

    virtual Tamgu* Get(long pos) {
        return aNOELEMENT;
    }

    virtual Tamgu* Set(long pos, Tamgu* v) {
        return aNOELEMENT;
    }

    Tamgu* Iterate() {
        return global->Provideint(i);
    }
    
    virtual Tamgu* Clear() {
        return aFALSE;
    }

};

class JavaIterationLong : public JavaIteration {
public:
    jlong* elements;

    JavaIterationLong(JNIEnv* e, TamguGlobal* g, jobjectArray o, size_t c) : JavaIteration(e, g, o, c) {
        elements = e->GetLongArrayElements((jlongArray)object, 0);
    }

    ~JavaIterationLong() {
        current_env->ReleaseLongArrayElements((jlongArray)object, elements, 0);
    }

    Tamgu* Value() {
        if (i >= count)
            return aNOELEMENT;
        return global->Provideint(elements[i]);
    }

    Tamgu* Get(long pos) {
        if (pos < 0 || pos >= count)
            return aNOELEMENT;
        return global->Provideint(elements[pos]);
    }
};

class JavaIterationFloat : public JavaIteration {
public:
    jfloat* elements;

    JavaIterationFloat(JNIEnv* e, TamguGlobal* g, jobjectArray o, size_t c) : JavaIteration(e, g, o, c) {
        elements = e->GetFloatArrayElements((jfloatArray)object, 0);
    }

    ~JavaIterationFloat() {
        current_env->ReleaseFloatArrayElements((jfloatArray)object, elements, 0);
    }

    Tamgu* Value() {
        if (i >= count)
            return aNOELEMENT;
        return global->Providefloat(elements[i]);
    }

    Tamgu* Get(long pos) {
        if (pos >= count)
            return aNOELEMENT;
        return global->Providefloat(elements[pos]);
    }
};

class JavaIterationDouble : public JavaIteration {
public:
    jdouble* elements;

    JavaIterationDouble(JNIEnv* e, TamguGlobal* g, jobjectArray o, size_t c) : JavaIteration(e, g, o, c) {
        elements = e->GetDoubleArrayElements((jdoubleArray)object, 0);
    }

    ~JavaIterationDouble() {
        current_env->ReleaseDoubleArrayElements((jdoubleArray)object, elements, 0);
    }

    Tamgu* Value() {
        if (i >= count)
            return aNOELEMENT;
        return global->Providefloat(elements[i]);
    }

    Tamgu* Get(long pos) {
        if (pos >= count)
            return aNOELEMENT;
        return global->Providefloat(elements[pos]);
    }
};

class JavaIterationString : public JavaIteration {
public:

    JavaIterationString(JNIEnv* e, TamguGlobal* g, jobjectArray o, size_t c) : JavaIteration(e, g, o, c) {}

    Tamgu* Value() {
        if (i >= count)
            return aNOELEMENT;
        
        JNIEnv* env = getEnv();
        jstring element = (jstring)env->GetObjectArrayElement((jobjectArray)object, i);
        string value = jstringToString(env, element);
        freeEnv();
        return global->Providestring(value);
    }

    Tamgu* Get(long pos) {
        if (pos >= count)
            return aNOELEMENT;
        JNIEnv* env = getEnv();
        jstring element = (jstring)env->GetObjectArrayElement((jobjectArray)object, pos);
        string value = jstringToString(env, element);
        freeEnv();
        return global->Providestring(value);
    }

};

class JavaIterationList : public JavaIteration {
public:
    jclass jclazzlist;
    jmethodID java_util_List_add;
    jmethodID java_util_List_set;
    jmethodID java_util_List_get;
    jmethodID java_util_List_clear;

    JavaIterationList(JNIEnv* e, TamguGlobal* g, jobject o, size_t c) : JavaIteration(e, g, o, c) {
        jclazzlist = e->GetObjectClass(o);
        java_util_List_add = e->GetMethodID(jclazzlist, "add", "(Ljava/lang/Object;)Z");
        java_util_List_set = e->GetMethodID(jclazzlist, "set", "(ILjava/lang/Object;)Ljava/lang/Object;");
        java_util_List_get = e->GetMethodID(jclazzlist, "get", "(I)Ljava/lang/Object;");
        java_util_List_clear = e->GetMethodID(jclazzlist, "clear", "()V");
    }
    
    Tamgu* Clear() {
        JNIEnv* env = getEnv();
        env->CallObjectMethod(object, java_util_List_clear);        
        count = 0;
        freeEnv();
        return aTRUE;
    }
    
};

class JavaIterationListFloat : public JavaIterationList {
public:

    jclass floatClass;
    jmethodID floatValue;
    jmethodID create_an_object;

    JavaIterationListFloat(JNIEnv* e, TamguGlobal* g, jobject o, size_t c) : JavaIterationList(e, g, o, c) {
        jclass cl = e->FindClass("java/lang/Float");
        floatClass = (jclass)e->NewGlobalRef(cl);
        e->DeleteLocalRef(cl);
        floatValue = e->GetMethodID(floatClass, "floatValue", "()F");
        create_an_object = e->GetMethodID(floatClass, "<init>", "(F)V");
    }

    ~JavaIterationListFloat() {
        current_env->DeleteLocalRef(floatClass);
    }

    Tamgu* Value() {
        if (i >= count)
            return aNOELEMENT;

        JNIEnv* env = getEnv();
        jobject element =  env->CallObjectMethod(object, java_util_List_get, i);
        jfloat v  = env->CallFloatMethod(element, floatValue);
        Tamgu* value = new Tamgudecimal(v);
        env->DeleteLocalRef(element);
        freeEnv();

        return value;
    }

    Tamgu* Get(long pos) {
        if (pos >= count)
            return aNOELEMENT;

        JNIEnv* env = getEnv();
        jobject element =  env->CallObjectMethod(object, java_util_List_get, pos);
        jfloat v  = env->CallFloatMethod(element, floatValue);
        Tamgu* value = global->Providefloat(v);
        env->DeleteLocalRef(element);
        freeEnv();
        return value;
    }

    Tamgu* Set(long pos, Tamgu* v) {
        JNIEnv* env = getEnv();
        
        jobject value = env->NewObject(floatClass, create_an_object, (jfloat)v->Decimal());
        
        if (pos >= count) {
            env->CallObjectMethod(object, java_util_List_add, value);
            count++;
        }
        else
            env->CallObjectMethod(object, java_util_List_set, pos, value);
        
        freeEnv();
        return aTRUE;
    }
};


class JavaIterationListInteger : public JavaIterationList {
public:

    jclass intClass;
    jmethodID intValue;
    jmethodID create_an_object;

    JavaIterationListInteger(JNIEnv* e, TamguGlobal* g, jobject o, size_t c) : JavaIterationList(e, g, o, c) {
        jclass cl = e->FindClass("java/lang/Integer");
        intClass = (jclass)e->NewGlobalRef(cl);
        e->DeleteLocalRef(cl);
        intValue = e->GetMethodID(intClass, "intValue", "()I");
        create_an_object = e->GetMethodID(intClass, "<init>", "(I)V");
    }

    ~JavaIterationListInteger() {
        current_env->DeleteLocalRef(intClass);
    }

    Tamgu* Value() {
        if (i >= count)
            return aNOELEMENT;

        JNIEnv* env = getEnv();
        jobject element =  env->CallObjectMethod(object, java_util_List_get, i);
        jint v  = env->CallIntMethod(element, intValue);
        Tamgu* value = global->Provideint(v);
        env->DeleteLocalRef(element);
        freeEnv();
        return value;
    }

    Tamgu* Get(long pos) {
        if (pos >= count)
            return aNOELEMENT;

        JNIEnv* env = getEnv();
        jobject element =  env->CallObjectMethod(object, java_util_List_get, pos);
        jint v  = env->CallIntMethod(element, intValue);
        Tamgu* value = global->Provideint(v);
        env->DeleteLocalRef(element);
        freeEnv();
        return value;
    }

    Tamgu* Set(long pos, Tamgu* v) {
        JNIEnv* env = getEnv();
        jobject value = env->NewObject(intClass, create_an_object, (jint)v->Integer());

        if (pos >= count) {
            env->CallObjectMethod(object, java_util_List_add, value);
            count++;
        }
        else
            env->CallObjectMethod(object, java_util_List_set, pos, value);
        freeEnv();
        return aTRUE;
    }

    Tamgu* Set(long pos, long v) {
        JNIEnv* env = getEnv();
        jobject value = env->NewObject(intClass, create_an_object, (jint)v);

        if (pos >= count) {
            env->CallObjectMethod(object, java_util_List_add, value);
            count++;
        }
        else
            env->CallObjectMethod(object, java_util_List_set, pos, value);
        freeEnv();
        return aTRUE;
    }

};


class JavaIterationListDouble : public JavaIterationList {
public:

    jclass doubleClass;
    jmethodID doubleValue;
    jmethodID create_an_object;

    JavaIterationListDouble(JNIEnv* e, TamguGlobal* g, jobject o, size_t c) : JavaIterationList(e, g, o, c) {
        jclass cl = e->FindClass("java/lang/Double");
        doubleClass = (jclass)e->NewGlobalRef(cl);
        e->DeleteLocalRef(cl);
        doubleValue = e->GetMethodID(doubleClass, "doubleValue", "()D");
        create_an_object = e->GetMethodID(doubleClass, "<init>", "(D)V");
    }

    ~JavaIterationListDouble() {
        current_env->DeleteLocalRef(doubleClass);
    }

    Tamgu* Value() {
        if (i >= count)
            return aNOELEMENT;

        JNIEnv* env = getEnv();
        jobject element =  env->CallObjectMethod(object, java_util_List_get, i);
        jdouble v  = env->CallDoubleMethod(element, doubleValue);
        Tamgu* value = global->Providefloat(v);
        env->DeleteLocalRef(element);
        freeEnv();
        return value;
    }

    Tamgu* Get(long pos) {
        if (pos >= count)
            return aNOELEMENT;

        JNIEnv* env = getEnv();
        jobject element =  env->CallObjectMethod(object, java_util_List_get, pos);
        jdouble v  = env->CallDoubleMethod(element, doubleValue);
        Tamgu* value = global->Providefloat(v);
        env->DeleteLocalRef(element);
        freeEnv();
        return value;
    }

    Tamgu* Set(long pos, Tamgu* v) {
        JNIEnv* env = getEnv();
        jobject value = env->NewObject(doubleClass, create_an_object, v->Float());

        if (pos >= count) {
            env->CallObjectMethod(object, java_util_List_add, value);
            count++;
        }
        else
            env->CallObjectMethod(object, java_util_List_set, pos, value);
      
        freeEnv();
        return aTRUE;
    }

};

class JavaIterationListLong : public JavaIterationList {
public:

    jclass longClass;
    jmethodID longValue;
    jmethodID create_an_object;

    JavaIterationListLong(JNIEnv* e, TamguGlobal* g, jobject o, size_t c) : JavaIterationList(e, g, o, c) {
        jclass cl = e->FindClass("java/lang/Long");
        longClass = (jclass)e->NewGlobalRef(cl);
        e->DeleteLocalRef(cl);
        longValue = e->GetMethodID(longClass, "longValue", "()J");
        create_an_object = e->GetMethodID(longClass, "<init>", "(J)V");
    }

    ~JavaIterationListLong() {
        current_env->DeleteLocalRef(longClass);
    }

    Tamgu* Value() {
        if (i >= count)
            return aNOELEMENT;

        JNIEnv* env = getEnv();
        jobject element =  env->CallObjectMethod(object, java_util_List_get, i);
        jlong v  = env->CallLongMethod(element, longValue);
        Tamgu* value = global->Provideint(v);
        env->DeleteLocalRef(element);
        freeEnv();
        return value;
    }

    Tamgu* Get(long pos) {
        if (pos >= count)
            return aNOELEMENT;

        JNIEnv* env = getEnv();
        jobject element =  env->CallObjectMethod(object, java_util_List_get, pos);
        jlong v  = env->CallLongMethod(element, longValue);
        Tamgu* value = global->Provideint(v);
        env->DeleteLocalRef(element);
        freeEnv();
        return value;
    }

    Tamgu* Set(long pos, Tamgu* v) {
        JNIEnv* env = getEnv();
        jobject value = env->NewObject(longClass, create_an_object, (jlong)v->Long());

        if (pos >= count) {
            env->CallObjectMethod(object, java_util_List_add, value);
            count++;
        }
        else
            env->CallObjectMethod(object, java_util_List_set, pos, value);
        freeEnv();
        return aTRUE;
    }

};

class JavaIterationListString : public JavaIterationList {
public:


    JavaIterationListString(JNIEnv* e, TamguGlobal* g, jobject o, size_t c) : JavaIterationList(e, g, o, c) {}

    Tamgu* Value() {
        if (i >= count)
            return aNOELEMENT;

        JNIEnv* env = getEnv();
        jstring element = (jstring)env->GetObjectArrayElement((jobjectArray)object, i);
        string value = jstringToString(env, element);
        freeEnv();
        return global->Providestring(value);
    }

    Tamgu* Get(long pos) {
        if (pos >= count)
            return aNOELEMENT;

        JNIEnv* env = getEnv();
        jstring element =  (jstring)env->CallObjectMethod(object, java_util_List_get, pos);

        string value = jstringToString(env, element);
        freeEnv();
        return global->Providestring(value);
    }

    Tamgu* Set(long pos, Tamgu* v) {
        JNIEnv* env = getEnv();

        string str = v->String();

        jstring value = jstringFromString(env, str);
        if (pos >= count) {
            env->CallObjectMethod(object, java_util_List_add, value);
            count++;
        }
        else
            env->CallObjectMethod(object, java_util_List_set, pos, value);
        freeEnv();
        return aTRUE;
    }

    Tamgu* Set(long pos, string str) {
        JNIEnv* env = getEnv();
        jstring value = jstringFromString(env, str);
     
        if (pos >= count) {
            env->CallObjectMethod(object, java_util_List_add, value);
            count++;
        }
        else
            env->CallObjectMethod(object, java_util_List_set, pos, value);
        freeEnv();
        return aTRUE;
    }

};

