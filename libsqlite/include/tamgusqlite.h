/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgusqlite.h
 Date       : 2017/09/01
 Purpose    : TAMGU API to SQLite 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#ifndef tamgusqlite_h
#define tamgusqlite_h

#include "sqlite3.h"
#include "tamguvector.h"
#include "tamgumap.h"
#include "tamguint.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgusqlite;
//This typedef defines a type "sqliteMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgusqlite::*sqliteMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

class Tamgusqlite;

class TamguIterationsqlite : public TamguIteration {
public:
	Tamgusqlite* value;
	sqlite3_stmt *stmt;
	int returncode;
	vector<string> columnnames;
	int columnCount;
	Tamgumap* currentrow;
	int nb;
	long garbage_i;

	void Storevalue();

	TamguIterationsqlite(Tamgusqlite* v, TamguGlobal* g);

	Tamgu* Key() {
		return new Tamguint(nb);
	}

	Tamgu* Value();
	void Next();

	Tamgu* End();
	Tamgu* Begin();
};

//---------------------------------------------------------------------------------------------------------------------

class Tamgusqlite : public TamguObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static basebin_hash<sqliteMethod> methods;
    static hmap<string, string> infomethods;
    static bin_hash<unsigned long> exported;

    static short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
	
	string command;
	string dbname;
	sqlite3 *db;
	string sqlcommand;
	string bindcommand;
	sqlite3_stmt* stmt;
	vector<int> insertstructure;


    //---------------------------------------------------------------------------------------------------------------------
    Tamgusqlite(TamguGlobal* g, Tamgu* parent = NULL) : TamguObject(g, parent) {
        //Do not forget your variable initialisation
		
		dbname = "";
		db = NULL;
		sqlcommand = "";
		stmt = NULL;
	}

	~Tamgusqlite() {
		if (db != NULL)
			sqlite3_close(db);
	}


	void Methods(Tamgu* v) {
		hmap<string, string>::iterator it;
		for (it = infomethods.begin(); it != infomethods.end(); it++)
			v->storevalue(it->first);
	}

	string Info(string n) {

		if (infomethods.find(n) != infomethods.end())
			return infomethods[n];
		return "Unknown method";
	}


    //----------------------------------------------------------------------------------------------------------------------
	Tamgu* Put(Tamgu* v, Tamgu* i, short idthread);
    
	Tamgu* Get(Tamgu* context, Tamgu* v, short idthread);


    short Type() {
        return Tamgusqlite::idtype;
    }

    string Typename() {
        return "sqlite";
    }

    bool isString() {
        return false;
    }

    bool isNumber() {
        return false;
    }

    bool isBoolean() {
        return false;
    }

    bool isFrame() {
        return false;
    }

    Tamgu* Atom(bool forced=false) {
        return this;
    }

	//If it is too complex to duplicate an element (for instance when passed to a function)
	//then you can use return false...  Usually in that case, Atom should always be just: return this;
	bool duplicateForCall() {
		return false;
	}


    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        if (exported.find(n) != exported.end())
            return true;
        return false;
    }

    Tamgu* Newinstance(short, Tamgu* f = NULL) {
		return new Tamgusqlite(globalTamgu);
    }

	TamguIteration* Newiteration(bool direction) {
		return new TamguIterationsqlite(this, globalTamgu);
    }

    static void AddMethod(TamguGlobal* g, string name, sqliteMethod func, unsigned long arity, string infos);
    static bool InitialisationModule(TamguGlobal* g, string version);

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.

	Tamgu* MethodOpen(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodClose(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodCreate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodInsert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodInsertThroughBind(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodRun(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodExecute(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodBegin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodCommit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodMemory(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);


    

    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }

    void Clear() {
        //To set a variable back to empty
    }

    void Clean() {
        //Your cleaning code
    }

    string String() {
        Locking _lock(this);
        return "";
    }

    //wstring UString() {
    //   Locking _lock(this);
    //  return L"";
    //	}

    long Integer() {
        Locking _lock(this);
        return 0;
    }
    double Float() {
        Locking _lock(this);
        return 0;
    }
    BLONG Long() {
        Locking _lock(this);
        return 0;
    }

    bool Boolean() {
        Locking _lock(this);
        return false;
    }

    
    //Basic operations
    long Size() {
        Locking _lock(this);
        return 0;
    }
    
};

#endif


