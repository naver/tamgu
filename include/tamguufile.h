/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguufile.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamguufile_h
#define tamguufile_h
#include "tamguustring.h"
#include "tamguint.h"


//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamguufile;
//This typedef defines a type "ufileMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguufile::*ufileMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamguufile : public TamguObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging basebin_hash<ufileMethod> methods;
	
	

	static Exchanging short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
    FILE* thefile;
	string filename;
	string op;
    long nbcharacters;
	bool signature;
	bool first;
    bool invertbytes;

	//---------------------------------------------------------------------------------------------------------------------
	Tamguufile(TamguGlobal* g, Tamgu* parent = NULL) : TamguObject(g, parent) {
		//Do not forget your variable initialisation
		thefile = NULL;
		op = "rb";
		signature = false;
		first = false;
        invertbytes = false;
	}

	Tamguufile() {
		//Do not forget your variable initialisation
		thefile = NULL;
		op = "rb";
		signature = false;
		first = false;
        nbcharacters = 0;
        invertbytes = false;
	}

	//----------------------------------------------------------------------------------------------------------------------
	Tamgu* Put(Tamgu* v, Tamgu* i, short idthread);
	Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread);


	short Type() {
		return Tamguufile::idtype;
	}

	

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
		return "ufile";
	}

	//The next two functions work in conjunction...
	//If your object is very atomic (such as a number or a string)
	//Then when it is passed to a function or stored into a container,
	//it might be worth duplicating it.
	// In that case: duplicateForCall should return true...

	//However if your object is complex and probably unique through out the code
	//The duplicateForCall should return false, and Atom should always reduce to a "return this;" only...
	Tamgu* Atom(bool forced = false) {
		return this;
	}

	bool duplicateForCall() {
		return false;
	}

	bool isFile() {
		return true;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
	bool isDeclared(short n) {
        return methods.check(n);
	}

	Tamgu* Newinstance(short, Tamgu* f = NULL) {
		return new Tamguufile(NULL);
	}

	TamguIteration* Newiteration(bool direction) {
		return aITERNULL;
	}

	static void AddMethod(TamguGlobal* g, string name, ufileMethod func, unsigned long arity, string infos);
	static bool InitialisationModule(TamguGlobal* g, string version);

	void Methods(Tamgu* v) {
            for (const auto& it : globalTamgu->infomethods[idtype])
                 v->storevalue(it.first);
      }

      string Info(string n) {
            if (globalTamgu->infomethods[idtype].find(n) !=  globalTamgu->infomethods[idtype].end())
              return globalTamgu->infomethods[idtype][n];
             return "Unknown method";
	}
    
    Tamgu* Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread);
    Tamgu* Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct);

    
        bool openfile(string filename) {
            Locking _lock(this);
            op = "rb";

    #ifdef WIN32
            fopen_s(&thefile, STR(filename), STR(op));
    #else
            thefile=fopen(STR(filename),STR(op));
    #endif
            if (thefile == NULL)
                return false;
            
            return true;
        }

        bool openfilewrite(string filename) {
            Locking _lock(this);
            op = "wb";

    #ifdef WIN32
            fopen_s(&thefile, STR(filename), STR(op));
    #else
            thefile=fopen(STR(filename),STR(op));
    #endif
            if (thefile == NULL)
                return false;
            
            return true;
        }

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is a sample  of a function that could be implemented for your needs.
    Tamgu* MethodScan(Tamgu* context, short idthread, TamguCall* callfunc);
    
	Tamgu* MethodInitial(Tamgu* context, short idthread, TamguCall* callfunc) {
		Locking _lock(this);
		if (callfunc->Size() == 0)
			return aTRUE;
		filename = callfunc->Evaluate(0, aNULL, idthread)->String();
		op = "r";
		first = false;
		if (callfunc->Size() == 2) {
			op = callfunc->Evaluate(1, aNULL, idthread)->String();
			if (op != "r" && op != "a" && op != "w")
				return globalTamgu->Returnerror(e_unknown_operation_for, idthread);
			if (op == "w")
				first = true;
		}

        if (op == "r" || op == "w" || op == "a")
            op += "b";
        
#ifdef WIN32
        fopen_s(&thefile, STR(filename), STR(op));
#else
        thefile=fopen(STR(filename),STR(op));
#endif
        
        if (thefile == NULL)
            return globalTamgu->Returnerror(e_cannot_open_this, idthread);
        
        if (op[0] == 'r')
            consume_header();

		return aTRUE;
	}

	Tamgu* MethodOpenRead(Tamgu* context, short idthread, TamguCall* callfunc) {
		Locking _lock(this);
		if (thefile != NULL)
			return globalTamgu->Returnerror("File already open", idthread);
		filename = callfunc->Evaluate(0, aNULL, idthread)->String();
		op = "rb";
        
#ifdef WIN32
        fopen_s(&thefile, STR(filename), STR(op));
#else
        thefile=fopen(STR(filename),STR(op));
#endif
        if (thefile == NULL)
			return globalTamgu->Returnerror(e_cannot_open_this, idthread);

		
		return aTRUE;
	}

	Tamgu* MethodOpenWrite(Tamgu* context, short idthread, TamguCall* callfunc) {
		Locking _lock(this);
		if (thefile != NULL)
			return globalTamgu->Returnerror("File already open", idthread);
        
        nbcharacters = 0;
		filename = callfunc->Evaluate(0, aNULL, idthread)->String();
		op = "wb";
		first = true;
#ifdef WIN32
        fopen_s(&thefile, STR(filename), STR(op));
#else
        thefile=fopen(STR(filename),STR(op));
#endif
        if (thefile == NULL)
            return globalTamgu->Returnerror(e_cannot_open_this, idthread);

		return aTRUE;
	}

	Tamgu* MethodOpenAppend(Tamgu* context, short idthread, TamguCall* callfunc) {
		Locking _lock(this);
		if (thefile != NULL)
			return globalTamgu->Returnerror("File already open", idthread);

        nbcharacters = 0;

        filename = callfunc->Evaluate(0, aNULL, idthread)->String();
		op = "ab";
#ifdef WIN32
        fopen_s(&thefile, STR(filename), STR(op));
#else
        thefile=fopen(STR(filename),STR(op));
#endif
        if (thefile == NULL)
            return globalTamgu->Returnerror(e_cannot_open_this, idthread);

		long pos = ftell(thefile);
		if (pos == 0)
			consume_header();
		return aTRUE;
	}


	Tamgu* MethodClose(Tamgu* context, short idthread, TamguCall* callfunc) {
		Locking _lock(this);
        if (thefile != NULL) {
            fclose(thefile);
			thefile = NULL;
			return aTRUE;
		}
		return aFALSE;
	}

	Tamgu* MethodGet(Tamgu* context, short idthread, TamguCall* callfunc) {
		Locking _lock(this);
		if (thefile == NULL || feof(thefile) || op != "rb")
			return globalTamgu->Returnerror(e_wrong_access_to, idthread);

		TAMGUCHAR c = Get();
		return new Tamguustring(c);
	}

	Tamgu* MethodTell(Tamgu* context, short idthread, TamguCall* callfunc) {
		Locking _lock(this);
		if (thefile == NULL || feof(thefile))
			return globalTamgu->Returnerror(e_wrong_access_to, idthread);

		return globalTamgu->ProvideConstint(ftell(thefile));
	}

    Tamgu* MethodFlush(Tamgu* context, short idthread, TamguCall* callfunc) {
        Locking _lock(this);
        if (thefile == NULL || feof(thefile) || op == "rb")
            return globalTamgu->Returnerror(e_wrong_access_to, idthread);
        
        fflush(thefile);
        return aTRUE;
    }

	Tamgu* MethodSeek(Tamgu* context, short idthread, TamguCall* callfunc) {
		Locking _lock(this);
		if (thefile == NULL || feof(thefile))
			return globalTamgu->Returnerror(e_wrong_access_to, idthread);

		long i = callfunc->Evaluate(0, context, idthread)->Integer();

		if (fseek(thefile,i, SEEK_SET))
			return aTRUE;

		return aFALSE;
	}

	Tamgu* MethodUnget(Tamgu* context, short idthread, TamguCall* callfunc) {
		Locking _lock(this);
		if (thefile == NULL || feof(thefile) || op != "rb")
			return globalTamgu->Returnerror(e_wrong_access_to, idthread);

        TAMGUCHAR c = (TAMGUCHAR)callfunc->Evaluate(0, aNULL, idthread)->Integer();

        w_u_char wuc;
#ifdef WSTRING_IS_UTF16
		wuc.c = c;
#else
		if (c_unicode_to_utf16(wuc.c, c)) {
			ungetc(wuc.cc[3], thefile);
			ungetc(wuc.cc[2], thefile);
		}
#endif
		ungetc(wuc.cc[1], thefile);
		ungetc(wuc.cc[0], thefile);
		return aTRUE;
	}

	Tamgu* MethodWrite(Tamgu* context, short idthread, TamguCall* callfunc) {
        Locking _lock(this);
        if (thefile == NULL || feof(thefile) || op == "rb")
            return globalTamgu->Returnerror(e_wrong_access_to, idthread);
        
        wstring s = callfunc->Evaluate(0, aNULL, idthread)->UString();
        
        w_u_char wuc;
        if (first) {
            if (invertbytes) {
                fputc(254, thefile);
                fputc(255, thefile);
            }
            else {
                fputc(255, thefile);
                fputc(254, thefile);
            }
            first = false;
        }
        
        bool full = false;
        for (long i = 0; i < s.size(); i++) {
//On Windows, we are already dealing with UTF16 characters
#ifdef WSTRING_IS_UTF16
			wuc.c = s[i];
#else
            full = c_unicode_to_utf16(wuc.c, s[i]);
#endif
            
            if (invertbytes) {
                if (full) {
                    fputc(wuc.cc[3], thefile);
                    fputc(wuc.cc[2], thefile);
                }
                fputc(wuc.cc[1], thefile);
                fputc(wuc.cc[0], thefile);
                continue;
            }

            if (full) {
                fputc(wuc.cc[2], thefile);
                fputc(wuc.cc[3], thefile);
            }
            fputc(wuc.cc[0], thefile);
            fputc(wuc.cc[1], thefile);
        }

        return aTRUE;
	}

	Tamgu* MethodWriteln(Tamgu* context, short idthread, TamguCall* callfunc) {
		Locking _lock(this);
		if (thefile == NULL || feof(thefile) || op == "rb")
			return globalTamgu->Returnerror(e_wrong_access_to, idthread);

		wstring s = callfunc->Evaluate(0, aNULL, idthread)->UString();

#ifdef WIN32
		s += L"\r\n";
#else
		s+=L"\n";
#endif

        w_u_char wuc;
        if (first) {
            if (invertbytes) {
                fputc(254, thefile);
                fputc(255, thefile);
            }
            else {
                fputc(255, thefile);
                fputc(254, thefile);
            }
            first = false;
        }
        
        bool full = false;
        for (long i = 0; i < s.size(); i++) {
			//On Windows, we are already dealing with UTF16 characters

#ifdef WSTRING_IS_UTF16
			wuc.c = s[i];
#else
			full = c_unicode_to_utf16(wuc.c, s[i]);
#endif

            if (invertbytes) {
                if (full) {
                    fputc(wuc.cc[3], thefile);
                    fputc(wuc.cc[2], thefile);
                }
                fputc(wuc.cc[1], thefile);
                fputc(wuc.cc[0], thefile);
                continue;
            }
            
            if (full) {
                fputc(wuc.cc[2], thefile);
                fputc(wuc.cc[3], thefile);
            }
            fputc(wuc.cc[0], thefile);
            fputc(wuc.cc[1], thefile);
        }

		return aTRUE;
	}

    Tamgu* MethodEndian(Tamgu* context, short idthread, TamguCall* callfunc) {
        if (callfunc->Size() == 0)
            return booleantamgu[invertbytes];
        
        invertbytes = callfunc->Evaluate(0, context, idthread)->Boolean();
        return booleantamgu[invertbytes];
    }
    
	Tamgu* MethodReadoneline(Tamgu* context, short idthread, TamguCall* callfunc) {
		Locking _lock(this);
		if (thefile == NULL || feof(thefile) || op != "rb")
			return globalTamgu->Returnerror(e_wrong_access_to, idthread);

		wstring s;
		readoneline(s);
		return globalTamgu->Providewithustring(s);
	}

	Tamgu* MethodRead(Tamgu* context, short idthread, TamguCall* callfunc);

	Tamgu* MethodEof(Tamgu* context, short idthread, TamguCall* callfunc) {
		Locking _lock(this);
		if (thefile == NULL || feof(thefile))
			return aTRUE;
		return aFALSE;
	}

	Tamgu* MethodSignature(Tamgu* context, short idthread, TamguCall* callfunc) {
		Locking _lock(this);
		signature = callfunc->Evaluate(0, context, idthread)->Boolean();
		return aTRUE;
	}

    void readoneline(wstring& w) {
        TAMGUCHAR c;
        w= L"";
        w_u_char wuc;
        while (!feof(thefile)) {
            c = Get(wuc);
            concat_char_check_utf16(w, c);
            if (c == 13) {
                w += Get(wuc);
                return;
            }
            if (c==10)
                return;
        }
    }

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
	//is returned by the Declaration method.
	Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		return (this->*Tamguufile::methods.get(idname))(contextualpattern, idthread, callfunc);
	}

	void Clear(TamguGlobal* g) {
		//To set a variable back to empty
	}

    void write(wstring& s) {
        w_u_char wuc;
        if (first) {
            if (invertbytes) {
                fputc(254, thefile);
                fputc(255, thefile);
            }
            else {
                fputc(255, thefile);
                fputc(254, thefile);
            }
            first = false;
        }
        
        bool full = false;
        for (long i = 0; i < s.size(); i++) {
                //On Windows, we are already dealing with UTF16 characters
#ifdef WSTRING_IS_UTF16
            wuc.c = s[i];
#else
            full = c_unicode_to_utf16(wuc.c, s[i]);
#endif
            
            if (invertbytes) {
                if (full) {
                    fputc(wuc.cc[3], thefile);
                    fputc(wuc.cc[2], thefile);
                }
                fputc(wuc.cc[1], thefile);
                fputc(wuc.cc[0], thefile);
                continue;
            }
            
            if (full) {
                fputc(wuc.cc[2], thefile);
                fputc(wuc.cc[3], thefile);
            }
            fputc(wuc.cc[0], thefile);
            fputc(wuc.cc[1], thefile);
        }        
    }

    void writeln(wstring s) {
#ifdef WIN32
        s += L"\r\n";
#else
        s += L"\n";
#endif
        w_u_char wuc;
        if (first) {
            if (invertbytes) {
                fputc(254, thefile);
                fputc(255, thefile);
            }
            else {
                fputc(255, thefile);
                fputc(254, thefile);
            }
            first = false;
        }
        
        bool full = false;
        for (long i = 0; i < s.size(); i++) {
                //On Windows, we are already dealing with UTF16 characters
#ifdef WSTRING_IS_UTF16
            wuc.c = s[i];
#else
            full = c_unicode_to_utf16(wuc.c, s[i]);
#endif
            
            if (invertbytes) {
                if (full) {
                    fputc(wuc.cc[3], thefile);
                    fputc(wuc.cc[2], thefile);
                }
                fputc(wuc.cc[1], thefile);
                fputc(wuc.cc[0], thefile);
                continue;
            }
            
            if (full) {
                fputc(wuc.cc[2], thefile);
                fputc(wuc.cc[3], thefile);
            }
            fputc(wuc.cc[0], thefile);
            fputc(wuc.cc[1], thefile);
        }
    }


	string String() {
		return filename;
	}

    bool eof() {
        if (thefile == NULL || feof(thefile))
            return true;
        return false;
    }
    
	long readnb(wstring& w, long nb) {
        w= L"";
        long total = 0;
        w_u_char wuc;
        while (!feof(thefile) && nb) {
			concat_char_check_utf16(w, Get(wuc));
            nb--;
            total++;
        }

		return total;
	}

	long readnb(vector<wstring>& v, long nb) {		

		wstring w;
        TAMGUCHAR c;
        long total = 0;
        w_u_char wuc;
        while (!feof(thefile) && nb>0) {
            c = Get(wuc);
            concat_char_check_utf16(w, c);
            nb--;
            total++;
            if (c==13) {
                nb--;
                total++;
                w += Get(wuc);
                v.push_back(w);
                w=L"";
            }
            else {
                if (c == 10) {
                    v.push_back(w);
                    w=L"";
                }
            }
        }

        if (w != L"")
            v.push_back(w);

		return total;
	}

	void readall(wstring& w) {

        w_u_char wuc;
		wstring line;
		while (!feof(thefile)) {            
			concat_char_check_utf16(w, Get(wuc));
		}
	}


	void readall(vector<wstring>& v) {		

        wstring w;
        
        TAMGUCHAR c;
        w_u_char wuc;
        while (!feof(thefile)) {
            c = Get(wuc);

            concat_char_check_utf16(w, c);

            if (c == 13) {
                w += Get(wuc);
                v.push_back(w);
                w=L"";
            }
            else {
                if (c == 10) {
                    v.push_back(w);
                    w=L"";
                }
            }
        }
        if (w != L"")
            v.push_back(w);
	}


	void consume_header() {
		unsigned char c, cc;
        c = fgetc(thefile);
        cc = fgetc(thefile);
        invertbytes = false;
        if ((c == 254 && cc == 255) || (c == 255 && cc == 254)) {
			signature = true;
            if (c == 254)
                invertbytes = true;
			return;
		}
        ungetc(cc, thefile);
        ungetc(c, thefile);
	}

    TAMGUCHAR Get() {
        w_u_char wuc;
        wuc.c = 0;
        wuc.cc[0] = fgetc(thefile);
		if (feof(thefile))
			return 0;
		wuc.cc[1] = fgetc(thefile);
		if (invertbytes)
			wuc.invert();
#ifdef WSTRING_IS_UTF16
		if (checklargeutf16(wuc.c)) {
			//in this case, we need to read the following one...
			if (invertbytes)
				wuc.invert();
			wuc.cc[2] = fgetc(thefile);
			wuc.cc[3] = fgetc(thefile);
			if (invertbytes)
				wuc.invert();
		}
#else
		uint32_t r;
		if (c_utf16_to_unicode(r, wuc.c, false)) {
            //Two bytes more to read...
            wuc.c = 0;
            wuc.cc[0] = fgetc(thefile);
            wuc.cc[1] = fgetc(thefile);
            if (invertbytes)
                wuc.invert();
            c_utf16_to_unicode(r, wuc.c, true);
            wuc.c = r;
        }
#endif
		return wuc.c;
    }

    TAMGUCHAR Get(w_u_char& wuc) {
        wuc.c = 0;
        wuc.cc[0] = fgetc(thefile);
        if (feof(thefile))
            return 0;
        wuc.cc[1] = fgetc(thefile);
        if (invertbytes)
            wuc.invert();
#ifdef WSTRING_IS_UTF16
		if (checklargeutf16(wuc.c)) {
			//in this case, we need to read the following one...
			if (invertbytes)
				wuc.invert();
			wuc.cc[2] = fgetc(thefile);
			wuc.cc[3] = fgetc(thefile);
			if (invertbytes)
				wuc.invert();
		}
#else
		uint32_t r;
		if (c_utf16_to_unicode(r, wuc.c, false)) {
            //Two bytes more to read...
            wuc.c = 0;
            wuc.cc[0] = fgetc(thefile);
            wuc.cc[1] = fgetc(thefile);
            if (invertbytes)
                wuc.invert();
            c_utf16_to_unicode(r, wuc.c, true);
            wuc.c = r;
        }
#endif
        return wuc.c;
    }
    

	Tamgu* find(Tamgu* context, wstring& s) {
		if (thefile == NULL || feof(thefile))
			return aNULL;

        w_u_char wuc;
		TAMGUCHAR c = Get(wuc);
        long p = ftell(thefile);
		Tamgu* v = NULL;
		if (context->isContainer())
			v = Selectaivector(context);

		s = s_to_lower(s);

		long sz = s.size();
		long i = 0;
		while (!feof(thefile)) {
			if (i == sz) {
				if (v == NULL)
					return globalTamgu->ProvideConstint(p);
				v->storevalue(p);
				i = -1;
			}
			else {
				if (c_to_lower(c) != s[i]) {
					p = ftell(thefile);
					i = -1;
				}
			}
			c = Get(wuc);
			i++;
		}
		if (v == NULL)
			return aMINUSONE;
		return v;
	}

	//------------------------------------------------------------------------------------
	Tamgu* in(Tamgu* context, Tamgu* a, short idthread) {
		wstring s = a->UString();
		Tamgu* res = find(context, s);
		if (context->isBoolean()) {
			if (res == aMINUSONE)
				return aFALSE;
			res->Release();
			return aTRUE;
		}
		return res;
	}

	//------------------------------------------------------------------------------------
	void Clear() {
		//To set a variable back to empty
		if (thefile != NULL)
            fclose(thefile);
		thefile = NULL;
	}

};
#endif





