/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgusys.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgusys.h"
#include "atoms.h"
#include "tamgumap.h"
#include "tamgudate.h"
#include "instructions.h"

#ifdef WIN32
#include "Windows.h"
#include <io.h>
#endif

#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "directorylisting.h"

#ifdef WIN32
#define DUP _dup
#define DUP2 _dup2
#define FILENO _fileno
#else
#include <unistd.h>
#define FILENO fileno
#define DUP dup
#define DUP2 dup2
#endif

//We need to declare once again our local definitions.
Exporting basebin_hash<sysMethod>  Tamgusys::methods;
Exporting hmap<string, string> Tamgusys::infomethods;
Exporting bin_hash<unsigned long> Tamgusys::exported;

Exporting short Tamgusys::idtype = 0;

#ifdef WIN32
char* Getenv(char* name);
#else
#define Getenv getenv
#endif


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgusys::AddMethod(TamguGlobal* global, string name, sysMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgusys::Setidtype(TamguGlobal* global) {
        Tamgusys::idtype = global->Getid("sys");
    }

   bool Tamgusys::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    Tamgusys::idtype = global->Getid("sys");

    Tamgusys::AddMethod(global, "command", &Tamgusys::MethodCommand, P_ONE | P_TWO, "command(string com, string outputfile): execute the command 'com'. outputfile is optional and can be used to redirect output to a file.");
    Tamgusys::AddMethod(global, "isdirectory", &Tamgusys::MethodisDirectory, P_ONE, "isdirectory(string path): return 'true' if path is a directory");
    Tamgusys::AddMethod(global, "createdirectory", &Tamgusys::MethodCreateDirectory, P_ONE, "createdirectory(string path): create the directory of path 'path'");
    Tamgusys::AddMethod(global, "mkdir", &Tamgusys::MethodCreateDirectory, P_ONE, "mkdir(string path): create the directory of path 'path'");
    Tamgusys::AddMethod(global, "listdirectory", &Tamgusys::MethodListDirectory, P_ONE, "listdirectory(string path): return the content of the directory 'path'");
    Tamgusys::AddMethod(global, "ls", &Tamgusys::MethodListDirectory, P_ONE, "ls(string path): return the content of the directory 'path'");
    Tamgusys::AddMethod(global, "realpath", &Tamgusys::MethodRealPath, P_ONE, "realpath(path relpath): return the actual path matching 'realpath'");
    Tamgusys::AddMethod(global, "fileinfo", &Tamgusys::MethodFileInfo, P_ONE, "fileinfo(string pathname): returns the info of the file 'pathname'");
    Tamgusys::AddMethod(global, "env", &Tamgusys::MethodEnv, P_ONE | P_TWO, "env(string var)|env(string varstring val): return or set the content of the environment variable 'var'");
    Tamgusys::AddMethod(global, "popen", &Tamgusys::MethodPopen, P_ONE, "popen(string command): execute a command and store the result in a svector.");

    global->newInstance[Tamgusys::idtype] = new Tamgusys(global);
    global->RecordMethods(Tamgusys::idtype,Tamgusys::exported);

    Tamgu* a = new TamguSystemVariable(global, new TamguConstsys(global), global->Createid("_sys"), Tamgusys::idtype);
    
    return true;
}

void SetEnvironmentVariable(string command, string val) {
    #ifdef WIN32
    command += "=";
    command += val;
    _putenv(STR(command));
    #else
    setenv(command.c_str(), val.c_str(), 1);
    #endif
}

Tamgu* Tamgusys::Put(Tamgu* idx, Tamgu* kval, short idthread) {
    if (idx != NULL && !idx->isConst()) {
        lastcommand = idx->String();
        string val = kval->String();
        #ifdef WIN32
        lastcommand += "=";
        lastcommand += val;
        _putenv(STR(lastcommand));
        #else
        setenv(lastcommand.c_str(), val.c_str(), 1);
        #endif

    }
    return aTRUE;
}

Tamgu* Tamgusys::Eval(Tamgu* context, Tamgu* idx, short idthread) {
    if (idx != NULL && !idx->isConst()) {
        lastcommand = idx->String();
        char* rep = Getenv(STR(lastcommand));
        if (rep == NULL)
            return globalTamgu->Providestring("");
        return globalTamgu->Providestring(rep);
    }
    return this;
}

#ifdef WIN32
#define PATH_MAX 4096
#endif

Tamgu* Tamgusys::MethodPopen(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //0 is the first parameter and so on...
    Tamgu* kcmd = callfunc->Evaluate(0, aNULL, idthread);
    contextualpattern =  Selectasvector(contextualpattern);
    
    FILE *fp;
    int status;
	
	char path[PATH_MAX];

	string cmd = kcmd->String();
    
#ifdef WIN32
	fp = _popen(STR(cmd), "r");
#else
	fp = popen(STR(cmd), "r");

#endif
    if (fp == NULL)
        return globalTamgu->Returnerror("Error while opening 'popen' stream");
        
	while (fgets(path, PATH_MAX, fp) != NULL) {
        cmd  = path;
        contextualpattern->storevalue(cmd);
    }


#ifdef WIN32
	status = _pclose(fp);
#else
	status = pclose(fp);
#endif

	if (status == -1)
        return globalTamgu->Returnerror("Error while closing 'popen' stream");
    

    return contextualpattern;
}

Tamgu* Tamgusys::MethodCommand(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //0 is the first parameter and so on...
    Tamgu* kcmd = callfunc->Evaluate(0, aNULL, idthread);
    string filename;
    if (callfunc->Size() == 2)
        filename = callfunc->Evaluate(1, aNULL, idthread)->String();
    lastcommand = kcmd->String();
    if (lastcommand != "") {
        if (filename != "") {
            int o = DUP(1);
            FILE* stream = fopen(STR(filename), "w");
            if (stream == NULL)
                return globalTamgu->Returnerror("SYS(001): Cannot open output file", idthread);
            DUP2(FILENO(stream), 1);
            system(lastcommand.c_str());
            fflush(stdout);
            fclose(stream);
            DUP2(o, 1);
            stream = fopen(STR(filename), "r");
            string content;
            char buffer[2049];
            size_t nb;
            while (!feof(stream)) {
                nb = fread(buffer, 1, 2048, stream);
                buffer[nb] = 0;
                content += buffer;
            }
            fclose(stream);
            return globalTamgu->Providestring(content);
        }
        else {
            int res = system(lastcommand.c_str());
            return globalTamgu->Provideint(res);
        }
    }
    //you may return any value of course...
    return aTRUE;
}

Tamgu* Tamgusys::MethodisDirectory(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string dirName_in = callfunc->Evaluate(0, aNULL, idthread)->String();
    #ifdef WIN32
    DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return aFALSE;  //something is wrong with your path!
    if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
        return aTRUE;   // this is a directory!
    #else
    struct stat st;
    if (stat(STR(dirName_in), &st) == 0)
        if ((st.st_mode & S_IFMT) == S_IFDIR)
            return aTRUE;
    #endif
    return aFALSE;    // this is not a directory!
}

Tamgu* Tamgusys::MethodCreateDirectory(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* kpath = callfunc->Evaluate(0, aNULL, idthread);
    string path = kpath->String();
    if (DirectoryCreation(STR(path), 0) == false)
        return aFALSE;
    return aTRUE;
}

Tamgu* Tamgusys::MethodListDirectory(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* kpath = callfunc->Evaluate(0, aNULL, idthread);
    string path = kpath->String();
    directorylisting dir(path);
    bool go = dir.initial();

    if (contextualpattern->isNumber()) {
        long i = 0;
        if (go) {
            while (dir.getnext()) {
                i++;
            }
        }
        return globalTamgu->Provideint(i);
    }
    
    Tamgu* kvect = Selectasvector(contextualpattern);

    if (!go)
        return kvect;
    
    string name;
    while (dir.getnext(name)) {
        if (kvect->Type() == a_svector)
            ((Tamgusvector*)kvect)->values.push_back(name);
        else
            kvect->Push(globalTamgu->Providestring(name));
    }
    return kvect;
}

Tamgu* Tamgusys::MethodRealPath(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* kpath = callfunc->Evaluate(0, aNULL, idthread);
    char localpath[4096];
    string path = kpath->String();
    #ifdef WIN32
    _fullpath(localpath, STR(path), 4096);
    #else
    realpath(STR(path), localpath);
    #endif
    return globalTamgu->Providestring(localpath);
}

Tamgu* Tamgusys::MethodFileInfo(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* kpath = callfunc->Evaluate(0, aNULL, idthread);
    string filename = kpath->String();
    #ifdef WIN32
    struct _stat scible;
    int stcible = _stat(STR(filename), &scible);
    #else
    struct stat scible;
    int stcible = stat(STR(filename), &scible);
    #endif
    if (stcible >= 0) {
        Tamgu* size = globalTamgu->Provideint(scible.st_size);
        Tamgu* change = new Tamgudate(scible.st_mtime);
        Tamgu* adate = new Tamgudate(scible.st_atime);
        Tamgu* cdate = new Tamgudate(scible.st_ctime);
        kpath = globalTamgu->Providestring(filename);
        Tamgumap* mp = (Tamgumap*)Selectamap(contextualpattern);
        mp->Push("size", size);
        mp->Push("date", change);
        mp->Push("access", adate);
        mp->Push("change", cdate);
        mp->Push("pathname", kpath);
        if ((scible.st_mode & S_IFMT) == S_IFDIR)
            mp->Push("directory", aTRUE);
        else
            mp->Push("directory", aFALSE);
        return mp;
    }
    return aNULL;
}

Tamgu* Tamgusys::MethodEnv(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //0 is the first parameter and so on...
    Tamgu* kname = callfunc->Evaluate(0, aNULL, idthread);
    lastcommand = kname->String();
    if (callfunc->Size() == 1) {
        if (lastcommand != "") {
            char* rep = Getenv(STR(lastcommand));
            if (rep == NULL)
                return globalTamgu->Providestring("");
            return globalTamgu->Providestring(rep);
        }
        //you may not return any value of course...
        return aNULL;
    }
    else {
        Tamgu* kvalue = callfunc->Evaluate(1, aNULL, idthread);
        if (lastcommand != "") {
            string val = kvalue->String();
            #ifdef WIN32
            lastcommand += "=";
            lastcommand += val;
            _putenv(STR(lastcommand));
            #else
            setenv(lastcommand.c_str(), val.c_str(), 1);
            #endif
            return aTRUE;
        }
    }
    //you may return any value of course...
    return aFALSE;
}
