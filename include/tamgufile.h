/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgufile.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgufile_h
#define tamgufile_h

#include "tamguint.h"
#include "tamguivector.h"
#include "tamgusvector.h"
#include "tamguuvector.h"
#include "tamgustring.h"
#include "tamguustring.h"
#include "tamgubyte.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgufile;
//This typedef defines a type "fileMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgufile::*fileMethod)(Tamgu* context, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamgufile : public TamguObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging basebin_hash<fileMethod> methods;
	static Exchanging hmap<string, string> infomethods;
	static Exchanging basebin_hash<unsigned long> exported;

	static Exchanging short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	FILE* thefile;
	string filename;
	string op;
	bool signature;
	bool first;
	unsigned char fbuffer[4096];
	vector<uchar> fchars;

	//---------------------------------------------------------------------------------------------------------------------
	Tamgufile(string f, TamguGlobal* g, Tamgu* parent = NULL) : TamguObject(g, parent) {
		//Do not forget your variable initialisation
		filename = f;
		op = "r";
		thefile = NULL;
		signature = false;
		first = false;
	}

	Tamgufile(FILE* v = NULL) {
		//Do not forget your variable initialisation
		thefile = v;
		op = "r";
		signature = false;
		first = false;
	}

	~Tamgufile() {
		if (thefile != NULL)
			fclose(thefile);
	}

	//----------------------------------------------------------------------------------------------------------------------
	Tamgu* Put(Tamgu* value, short idthread) {

		return this;
	}

	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
        locking();
        if (thefile == NULL || feof(thefile) || op != "rb") {
            unlocking();
			return aFALSE;
        }

		if (context->isNumber()) {
			unsigned char c = fgetc(thefile);
			context->storevalue(c);
            unlocking();
			return aTRUE;
		}

		string line = readoneline();
        if (!line.size()) {
            unlocking();
            return aFALSE;
        }
        
		context->storevalue(line);
        unlocking();
		return aTRUE;
	}


	short Type() {
		return Tamgufile::idtype;
	}

	

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
		return "file";
	}

	bool isAtom() {
		return true;
	}

	bool isFile() {
		return true;
	}

	Tamgu* Atom(bool forced = false) {
		return this;
	}

	bool duplicateForCall() {
		return false;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
	bool isDeclared(short n) {
		if (exported.find(n) != exported.end())
			return true;
		return false;
	}

	Tamgu* Newinstance(short, Tamgu* f = NULL) {
		return new Tamgufile(NULL);
	}

	TamguIteration* Newiteration(bool direction) {
		return aITERNULL;
	}

	static void AddMethod(TamguGlobal* g, string name, fileMethod func, unsigned long arity, string infos);
	
	static bool InitialisationModule(TamguGlobal* g, string version);

	
	void Methods(Tamgu* v) {

		for (auto& it : infomethods)
			v->storevalue(it.first);
	}
	string Info(string n) {
		if (infomethods.find(n) != infomethods.end())
			return infomethods[n];
		return "Unknown method";
	}
    
    Tamgu* Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread);
    Tamgu* Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct);

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	//Tamgu* MethodSize(TamguGlobal* global,Tamgu* context, short idthread, TamguCall* callfunc) {return aZERO;}
	Tamgu* MethodInitial(Tamgu* context, short idthread, TamguCall* callfunc) {
		if (callfunc->Size() == 0)
			return aTRUE;
        locking();
		filename = callfunc->Evaluate(0, aNULL, idthread)->String();
		op = "r";
		first = false;
		if (callfunc->Size() == 2) {
			op = callfunc->Evaluate(1, aNULL, idthread)->String();
            if (op != "r" && op != "a" && op != "w") {
                unlocking();
				return globalTamgu->Returnerror("Unknown operation for this file", idthread);
            }
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
        if (thefile == NULL) {
            unlocking();
			return globalTamgu->Returnerror("Cannot open this file", idthread);
        }

		if (op[0] == 'r')
			consume_header();
        unlocking();
		return aTRUE;
	}

    Tamgu* MethodFlush(Tamgu* context, short idthread, TamguCall* callfunc) {
        locking();
        if (thefile == NULL || feof(thefile) || op != "wb") {
            unlocking();
            return globalTamgu->Returnerror("Wrong access to the file", idthread);
        }
        fflush(thefile);
        unlocking();
        return aTRUE;
    }

	Tamgu* MethodOpenRead(Tamgu* context, short idthread, TamguCall* callfunc) {
		locking();
        if (thefile != NULL) {
            unlocking();
			return globalTamgu->Returnerror("File already open", idthread);
        }
		filename = callfunc->Evaluate(0, aNULL, idthread)->String();
		op = "rb";
#ifdef WIN32
		fopen_s(&thefile, STR(filename), STR(op));
#else
		thefile = fopen(STR(filename), STR(op));
#endif
        if (thefile == NULL) {
            unlocking();
			return globalTamgu->Returnerror("Cannot open this file", idthread);
        }

		consume_header();
        unlocking();
		return aTRUE;
	}

	Tamgu* MethodOpenWrite(Tamgu* context, short idthread, TamguCall* callfunc) {
        locking();
        if (thefile != NULL) {
            unlocking();
            return globalTamgu->Returnerror("File already open", idthread);
        }
		filename = callfunc->Evaluate(0, aNULL, idthread)->String();
		op = "wb";
		first = true;
#ifdef WIN32
		fopen_s(&thefile, STR(filename), STR(op));
#else
		thefile = fopen(STR(filename), STR(op));
#endif
        if (thefile == NULL) {
            unlocking();
            return globalTamgu->Returnerror("Cannot open this file", idthread);
        }

        unlocking();
		return aTRUE;
	}

	Tamgu* MethodOpenAppend(Tamgu* context, short idthread, TamguCall* callfunc) {
        locking();
        if (thefile != NULL) {
            unlocking();
            return globalTamgu->Returnerror("File already open", idthread);
        }
		filename = callfunc->Evaluate(0, aNULL, idthread)->String();
		op = "ab";
#ifdef WIN32
		fopen_s(&thefile, STR(filename), STR(op));
#else
		thefile = fopen(STR(filename), STR(op));
#endif
        if (thefile == NULL) {
            unlocking();
			return globalTamgu->Returnerror("Cannot open this file", idthread);
        }
        unlocking();
		return aTRUE;
	}


	Tamgu* MethodClose(Tamgu* context, short idthread, TamguCall* callfunc) {
		locking();
		if (thefile != NULL) {
			fclose(thefile);
			thefile = NULL;
            unlocking();
			return aTRUE;
		}
        unlocking();
		return aFALSE;
	}

	Tamgu* MethodSignature(Tamgu* context, short idthread, TamguCall* callfunc) {
		locking();

		if (callfunc->Size() == 1) {
			signature = callfunc->Evaluate(0, context, idthread)->Boolean();
            unlocking();
			return aTRUE;
		}

		if (thefile != NULL) {
			uchar c = fgetc(thefile);
			uchar cc = fgetc(thefile);

            if ((c == 254 && cc == 255) || (c == 255 && cc == 254)) {
                ungetc(cc, thefile);
				ungetc(c, thefile);
                unlocking();
				return globalTamgu->ProvideConstint(16);
			}

			uchar ccc = fgetc(thefile);

			ungetc(ccc, thefile);
			ungetc(cc, thefile);
			ungetc(c, thefile);

			if (c == 239 && cc == 187 && ccc == 191) {
				signature = true;
                unlocking();
				return globalTamgu->ProvideConstint(8);
			}
		}
        unlocking();
		return aZERO;
	}


	Tamgu* MethodGet(Tamgu* context, short idthread, TamguCall* callfunc) {
        locking();
        if (thefile == NULL || feof(thefile) || op != "wb") {
            unlocking();
            return globalTamgu->Returnerror("Wrong access to the file", idthread);
        }

		if (context->isString()) {
			wchar_t c = getc(true);
            unlocking();
			if (context->Type() == a_ustring)
				return new Tamguustring(c);

			agnostring s(c);
			return globalTamgu->Providewithstring(s);
		}

		uchar c = get();
        unlocking();
		return new Tamgubyte(c);
	}

	Tamgu* MethodTell(Tamgu* context, short idthread, TamguCall* callfunc) {
        locking();
        if (thefile == NULL || feof(thefile) || op != "wb") {
            unlocking();
            return globalTamgu->Returnerror("Wrong access to the file", idthread);
        }

        long e = tell();
        unlocking();
		return globalTamgu->ProvideConstint(e);
	}

	Tamgu* MethodSeek(Tamgu* context, short idthread, TamguCall* callfunc) {
        locking();
        if (thefile == NULL || feof(thefile) || op != "wb") {
            unlocking();
            return globalTamgu->Returnerror("Wrong access to the file", idthread);
        }
		
        long i = callfunc->Evaluate(0, context, idthread)->Integer();
        i = seek(i);
        unlocking();
        //--------------------------------------------------------------------
        return booleantamgu[i];
	}

	Tamgu* MethodUnget(Tamgu* context, short idthread, TamguCall* callfunc) {
        locking();
        if (thefile == NULL || feof(thefile) || op != "wb") {
            unlocking();
            return globalTamgu->Returnerror("Wrong access to the file", idthread);
        }


		char c = callfunc->Evaluate(0, aNULL, idthread)->Byte();
		ungetc(c, thefile);
        unlocking();
		return aTRUE;
	}

	Tamgu* MethodWrite(Tamgu* context, short idthread, TamguCall* callfunc) {
        locking();
        if (thefile == NULL || feof(thefile) || op != "wb") {
            unlocking();
            return globalTamgu->Returnerror("Wrong access to the file", idthread);
        }


		if (first) {
			if (signature) {
				putc(239, thefile);
				putc(187, thefile);
				putc(191, thefile);
			}
			first = false;
		}

		string s = callfunc->Evaluate(0, aNULL, idthread)->String();
		fwrite(STR(s), 1, s.size(), thefile);
        unlocking();
		return aTRUE;
	}

	Tamgu* MethodWriteln(Tamgu* context, short idthread, TamguCall* callfunc) {
        locking();
        if (thefile == NULL || feof(thefile) || op != "wb") {
            unlocking();
            return globalTamgu->Returnerror("Wrong access to the file", idthread);
        }


		if (first) {
			if (signature) {
				putc(239, thefile);
				putc(187, thefile);
				putc(191, thefile);
			}
			first = false;
		}

		string s = callfunc->Evaluate(0, aNULL, idthread)->String();
		s += Endl;
		fwrite(STR(s), 1, s.size(), thefile);
        unlocking();
		return aTRUE;
	}

    Tamgu* MethodWriteutf16(Tamgu* context, short idthread, TamguCall* callfunc) {
        locking();
        if (thefile == NULL || feof(thefile) || op != "wb") {
            unlocking();
            return globalTamgu->Returnerror("Wrong access to the file", idthread);
        }


        wstring s = callfunc->Evaluate(0, aNULL, idthread)->UString();
        
        if (callfunc->Size() == 2) {
            bool addln = callfunc->Evaluate(0, aNULL, idthread)->Boolean();
            if (addln) {
#ifdef WIN32
                s += L"\r\n";
#else
                s += L"\n";
#endif
            }
        }
        
        w_u_char wuc;
        if (first) {
            fputc(255, thefile);
            fputc(254, thefile);
            first = false;
        }
        
        for (long i = 0; i < s.size(); i++) {
#ifdef WSTRING_IS_UTF16
			wuc.c = s[i];
#else
            if (c_unicode_to_utf16(wuc.c, s[i])) {
                fputc(wuc.cc[2], thefile);
                fputc(wuc.cc[3], thefile);
            }
#endif
            fputc(wuc.cc[0], thefile);
            fputc(wuc.cc[1], thefile);
        }
        unlocking();
        return aTRUE;
    }
    

	Tamgu* MethodWritebin(Tamgu* context, short idthread, TamguCall* callfunc) {
        locking();
        if (thefile == NULL || feof(thefile) || op != "wb") {
            unlocking();
            return globalTamgu->Returnerror("Wrong access to the file", idthread);
        }


		if (first) {
			if (signature) {
				putc(239, thefile);
				putc(187, thefile);
				putc(191, thefile);
			}
			first = false;
		}
        
        uchar c;
        Tamgu* value = callfunc->Evaluate(0, aNULL, idthread);
        if (value->isContainer()) {
            long sz = value->Size();
            for (long i = 0; i < sz ; i++) {
                c =  value->getbyte(i);
                fputc(c, thefile);
            }
            return aTRUE;
        }

		c = value->Byte();
		fputc(c, thefile);
        unlocking();
		return aTRUE;
	}

	Tamgu* MethodReadoneline(Tamgu* context, short idthread, TamguCall* callfunc) {
        locking();
        if (thefile == NULL || feof(thefile) || op != "wb") {
            unlocking();
            return globalTamgu->Returnerror("Wrong access to the file", idthread);
        }


		string s = readoneline();
        unlocking();
		return globalTamgu->Providewithstring(s);
	}

	Tamgu* MethodRead(Tamgu* context, short idthread, TamguCall* callfunc);

	Tamgu* MethodEof(Tamgu* context, short idthread, TamguCall* callfunc) {
        locking();
        if (thefile == NULL || feof(thefile)) {
            unlocking();
			return aTRUE;
        }
        unlocking();
		return aFALSE;
	}

    Tamgu* MethodScan(Tamgu* context, short idthread, TamguCall* callfunc);

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
	//is returned by the Declaration method.
	Tamgu* CallMethod(short idname, Tamgu* context, short idthread, TamguCall* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		return (this->*methods.get(idname))(context, idthread, callfunc);
	}


	//---------------------------------------------------------------------------------------------------------------------

	void unget(uchar c) {
		ungetc(c, thefile);
	}

    void write(string& s) {
        fwrite(STR(s), 1, s.size(), thefile);
    }

    void writeln(string s) {
        #ifdef WIN32
            s += "\r\n";
        #else
            s += "\n";
        #endif
        fwrite(STR(s), 1, s.size(), thefile);
    }
    

	void consume_header() {
		uchar c = fgetc(thefile);
		uchar cc = fgetc(thefile);
		uchar ccc = fgetc(thefile);

		if (c == 239 && cc == 187 && ccc == 191) {
			signature = true;
			return;
		}

		ungetc(ccc, thefile);
		ungetc(cc, thefile);
		ungetc(c, thefile);
	}


	unsigned char get() {
		return fgetc(thefile);
	}

	wchar_t getc(bool utf8) {
		if (!utf8)
			return get();

		unsigned char c = get();
		if (c < 0x0080)
			return c;

		unsigned char utf[] = { 0, 0, 0, 0, 0, 0 };
		wchar_t code = c;
		if ((c & 0xE0) == 0xC0) {
			//We take two bytes
			utf[1] = get();
			if ((utf[1] & 0xC0) != 0x80) {
				unget(utf[1]);
				return c;
			}
			code = (c ^ 0xc0) << 6;
			code |= utf[1] ^ 0x80;
			return code;
		}
		//we take three
		if ((c & 0xF0) == 0xE0) {
			utf[1] = get();
			if ((utf[1] & 0xC0) != 0x80) {
				unget(utf[1]);
				return c;
			}
			utf[2] = get();
			if ((utf[2] & 0xC0) != 0x80) {
				unget(utf[1]);
				unget(utf[2]);
				return c;
			}
			code = (c ^ 0xE0) << 12;
			code |= (utf[1] ^ 0x80) << 6;
			code |= (utf[2] ^ 0x80);
			return code;
		}

		//we take four
		if ((c & 0xF8) == 0xF0) {
			utf[1] = get();
			if ((utf[1] & 0xC0) != 0x80) {
				unget(utf[1]);
				return c;
			}
			utf[2] = get();
			if ((utf[2] & 0xC0) != 0x80) {
				unget(utf[1]);
				unget(utf[2]);
				return c;
			}
			utf[3] = get();
			if ((utf[3] & 0xC0) != 0x80) {
				unget(utf[1]);
				unget(utf[2]);
				unget(utf[3]);
				return c;
			}
			code = (c ^ 0xF0) << 18;
			code |= (utf[1] ^ 0x80) << 12;
			code |= (utf[2] ^ 0x80) << 6;
			code |= (utf[3] ^ 0x80);
			return code;
		}

		//we take five
		if ((c & 0xFC) == 0xF8) {
			utf[1] = get();
			if ((utf[1] & 0xC0) != 0x80) {
				unget(utf[1]);
				return c;
			}
			utf[2] = get();
			if ((utf[2] & 0xC0) != 0x80) {
				unget(utf[1]);
				unget(utf[2]);
				return c;
			}
			utf[3] = get();
			if ((utf[3] & 0xC0) != 0x80) {
				unget(utf[1]);
				unget(utf[2]);
				unget(utf[3]);
				return c;
			}
			utf[4] = get();
			if ((utf[4] & 0xC0) != 0x80) {
				unget(utf[1]);
				unget(utf[2]);
				unget(utf[3]);
				unget(utf[4]);
				return c;
			}
			code = (c ^ 0xF8) << 24;
			code |= (utf[1] ^ 0x80) << 18;
			code |= (utf[2] ^ 0x80) << 12;
			code |= (utf[3] ^ 0x80) << 6;
			code |= (utf[4] ^ 0x80);
			return code;
		}

		//we take six
		if ((c & 0xFF) == 0xFC) {
			utf[1] = get();
			if ((utf[1] & 0xC0) != 0x80) {
				unget(utf[1]);
				return c;
			}
			utf[2] = get();
			if ((utf[2] & 0xC0) != 0x80) {
				unget(utf[1]);
				unget(utf[2]);
				return c;
			}
			utf[3] = get();
			if ((utf[3] & 0xC0) != 0x80) {
				unget(utf[1]);
				unget(utf[2]);
				unget(utf[3]);
				return c;
			}
			utf[4] = get();
			if ((utf[4] & 0xC0) != 0x80) {
				unget(utf[1]);
				unget(utf[2]);
				unget(utf[3]);
				unget(utf[4]);
				return c;
			}
			utf[5] = get();
			if ((utf[4] & 0xC0) != 0x80) {
				unget(utf[1]);
				unget(utf[2]);
				unget(utf[3]);
				unget(utf[4]);
				unget(utf[5]);
				return c;
			}
			code = (c ^ 0xFC) << 30;
			code |= (utf[1] ^ 0x80) << 24;
			code |= (utf[2] ^ 0x80) << 18;
			code |= (utf[3] ^ 0x80) << 12;
			code |= (utf[4] ^ 0x80) << 6;
			code |= (utf[5] ^ 0x80);
			return code;
		}
		return c;
	}

	long readnb(long nb) {
		fchars.clear();
		fbuffer[0] = 0;

		long nbread, i;
		long total = 0;

		while (nb) {
			if (feof(thefile))
				return total;

			nbread = fread(fbuffer, 1, 4095, thefile);
			total += nbread;

			if (nbread == 0)
				return total;

			if (nb != -1) {
				if (nbread >= nb) {
					nbread = nb;
					nb = 0;
				}
				else
					nb -= nbread;
			}

			fbuffer[nbread] = 0;
			i = 0;
			if (first) {
				if (signature) {
					if (fbuffer[0] == 239 && fbuffer[1] == 187 && fbuffer[2] == 191)
						i = 3;
				}
				first = false;
			}
			for (; i < nbread; i++)
				fchars.push_back(fbuffer[i]);
		}
		return total;
	}

    void read(Fast_String& str, long nb) {
        long nbread;

        while (nb) {
            if (feof(thefile))
                return;

            fbuffer[0] = 0;
            nbread = fread(fbuffer, 1, 4095, thefile);

            if (nbread == 0)
                return;

            if (nb != -1) {
                if (nbread >= nb) {
                    nbread = nb;
                    nb = 0;
                }
                else
                    nb -= nbread;
            }

            fbuffer[nbread] = 0;
            if (first) {
                first = false;
                if (signature) {
                    if (fbuffer[0] == 239 && fbuffer[1] == 187 && fbuffer[2] == 191) {
                        str.add(fbuffer+3, nbread-3);
                        continue;
                    }
                }
            }

            str.add(fbuffer, nbread);
        }
    }
    
	string read(long nb) {

        string str;

		long nbread;

		while (nb) {
			if (feof(thefile))
				return str;

			fbuffer[0] = 0;
			nbread = fread(fbuffer, 1, 4095, thefile);

			if (nbread == 0)
				return str;

			if (nb != -1) {
				if (nbread >= nb) {
					nbread = nb;
					nb = 0;
				}
				else
					nb -= nbread;
			}

			fbuffer[nbread] = 0;
			if (first) {
				first = false;
				if (signature) {
					if (fbuffer[0] == 239 && fbuffer[1] == 187 && fbuffer[2] == 191) {
						str += (char*)fbuffer + 3;
						continue;
					}
				}
			}

			str += (char*)fbuffer;
		}
		return str;
	}

    void readin(string& str, long nb) {
        str = "";
        long nbread;

        while (nb) {
            if (feof(thefile))
                return;

            fbuffer[0] = 0;
            nbread = fread(fbuffer, 1, 4095, thefile);

            if (nbread == 0)
                return;

            if (nb != -1) {
                if (nbread >= nb) {
                    nbread = nb;
                    nb = 0;
                }
                else
                    nb -= nbread;
            }

            fbuffer[nbread] = 0;
            if (first) {
                first = false;
                if (signature) {
                    if (fbuffer[0] == 239 && fbuffer[1] == 187 && fbuffer[2] == 191) {
                        str += (char*)fbuffer + 3;
                        continue;
                    }
                }
            }

            str += (char*)fbuffer;
        }
    }
    
	wstring wread(long nb) {
		wstring str;

		wchar_t b;
		while (nb) {
			if (feof(thefile))
				return str;
			b = getc(true);
			str += b;
		}
		return str;
	}

	string readoneline() {
		fbuffer[0] = 0;
        fbuffer[4093] = 0;
		fgets((char*)fbuffer, 4095, thefile);
		if (first) {
			if (signature) {
				if (fbuffer[0] == 239 && fbuffer[1] == 187 && fbuffer[2] == 191)
					memcpy(fbuffer, fbuffer + 3, strlen((char*)fbuffer) - 3);
			}
			first = false;
		}
        
        long sz = strlen((char*)fbuffer);
        if (sz < 4094 || fbuffer[4093] == '\n')
            return (char*)fbuffer;
        
        string s = (char*)fbuffer;
        while (!feof(thefile)) {
            fbuffer[0] = 0;
            fbuffer[4093] = 0;
            fgets((char*)fbuffer, 4095, thefile);
            sz = s.size();
            s += (char*)fbuffer;
            sz = s.size() - sz;
            if (sz < 4094 || fbuffer[4093] == '\n')
                return s;
        }
        return s;
	}

	void readall(vector<string>& fstrings) {
		fstrings.clear();

        string line;
		while (!feof(thefile)) {
            line = readoneline();
            if (line.size())
                fstrings.push_back(line);
		}
	}

	void readall(vector<wstring>& fstrings) {
		fstrings.clear();
		string line;
        wstring w;

		while (!feof(thefile)) {
            line = readoneline();
            if (line.size()) {
                sc_utf8_to_unicode(w, USTR(line), line.size());
                fstrings.push_back(w);
            }
		}
	}

	void readnb(long nb, vector<string>& fstrings) {
		fstrings.clear();

		size_t total = 0;
		while (!feof(thefile)) {
			fbuffer[0] = 0;
			fgets((char*)fbuffer, 4095, thefile);
			if (first) {
				if (signature) {
					if (fbuffer[0] == 239 && fbuffer[1] == 187 && fbuffer[2] == 191)
						memcpy(fbuffer, fbuffer + 3, strlen((char*)fbuffer) - 3);
				}
				first = false;
			}
			fstrings.push_back((char*)fbuffer);
			total += fstrings.back().size();
			if (total >= nb)
				return;
		}
	}

	void readnb(long nb, vector<wstring>& fstrings) {
		fstrings.clear();
		agnostring s;

		long total = 0;
		while (!feof(thefile)) {
			fbuffer[0] = 0;
			fgets((char*)fbuffer, 4095, thefile);
			if (first) {
				if (signature) {
					if (fbuffer[0] == 239 && fbuffer[1] == 187 && fbuffer[2] == 191)
						memcpy(fbuffer, fbuffer + 3, strlen((char*)fbuffer) - 3);
				}
				first = false;
			}
			s = (char*)fbuffer;
			fstrings.push_back(s.latintounicode());
			total += s.size();
			if (total >= nb)
				return;
		}
	}

	long tell() {
		if (thefile == NULL || feof(thefile) || feof(thefile))
			return -1;
		return ftell(thefile);
	}

	bool seek(long i) {
		if (thefile == NULL || feof(thefile) || feof(thefile))
			return false;

		fseek(thefile, i, SEEK_SET);
		return true;
	}

	Tamgu* find(Tamgu* context, agnostring& sx, bool u) {
		if (thefile == NULL || feof(thefile) || feof(thefile))
			return aNULL;

		wchar_t c;
		bool utf8 = false;
		if (u) {
			sx = sx.tolower();
			utf8 = sx.isutf8();
		}
		string& s = sx;
		int i = 0;
		long p = tell();
		Tamgu* v = NULL;
		if (context->isVectorContainer())
			v = Selectaivector(context);

		c = getc(utf8);
		while (!feof(thefile)) {
			if (utf8) {
				if (sx.end()) {
					if (v == NULL)
						return globalTamgu->ProvideConstint(p);
					v->storevalue(p);
					sx.charpos = 0;
					sx.bytepos = 0;
				}
				c = c_to_lower(c);
				if (c != sx.nextcode()) {
					sx.charpos = 0;
					sx.bytepos = 0;
					p = tell();
				}
			}
			else {
				if (i >= s.size()) {
					if (v == NULL)
						return globalTamgu->ProvideConstint(p);
					v->storevalue(p);
					i = 0;
				}
				if (u)
					c = c_to_lower(c);
				if ((uchar)c == (uchar)s[i])
					i++;
				else {
					i = 0;
					p = tell();
				}
			}
			c = getc(utf8);
		}
		if (v == NULL)
			return aMINUSONE;
		return v;
	}

    bool eof() {
        if (thefile == NULL || feof(thefile))
            return true;
        return false;
    }
    
	//------------------------------------------------------------------------------------
	Tamgu* in(Tamgu* context, Tamgu* a, short idthread) {
		agnostring s(a->String());
		Tamgu* res = find(context, s, s.isutf8());
		if (context->isBoolean()) {
			if (res == aMINUSONE)
				return aFALSE;
			res->Release();
			return aTRUE;
		}
		return res;
	}

	//------------------------------------------------------------------------------------
	long Size() {
        locking();
		struct stat scible;
		int stcible = -1;
		long size = -1;

		if (thefile != NULL) {
#if (_MSC_VER >= 1900)
			stcible = fstat(_fileno(thefile), &scible);
#else
#if  defined(WIN32) | defined(APPLE)
			stcible = fstat(thefile->_file, &scible);
#else
			stcible = fstat(thefile->_fileno, &scible);
#endif
#endif
			if (stcible >= 0)
				size = scible.st_size;
		}
        unlocking();
		return size;
	}

	void Clear() {
		//To set a variable back to empty
        locking();
		if (thefile != NULL)
			fclose(thefile);
		thefile = NULL;
        unlocking();
	}

	

	string String() {
		return filename;
	}

	long Integer() {
		return Size();
	}

	float Decimal() {
		return (float)Size();
	}

	double Float() {
		return Size();
	}

	BLONG Long() {
		return Size();
	}

	bool Boolean() {
		if (thefile == NULL)
			return false;
		return true;
	}

};

#endif







