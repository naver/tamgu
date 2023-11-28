/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 * This file can only be used with the Tamgu (탐구) library or the executable
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgurawstring.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgurawstring_h
#define tamgurawstring_h

#include "tamguint.h"
#include "tamgustring.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgurawstring;
//This typedef defines a type "rawstringMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgurawstring::*rawstringMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamgurawstring : public TamguReference {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging basebin_hash<rawstringMethod> methods;
	
	

	static Exchanging short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	unsigned char* value;
	long buffersize;
	long stringsize;


	//---------------------------------------------------------------------------------------------------------------------
	Tamgurawstring(TamguGlobal* g, Tamgu* parent = NULL) : TamguReference(g, parent) {
        investigate |= is_string;
		//Do not forget your variable initialisation
		value = NULL;
		buffersize = 0;
		stringsize = 0;
	}

	Tamgurawstring() {
        investigate |= is_string;
		//Do not forget your variable initialisation
		value = NULL;
		buffersize = 0;
		stringsize = 0;

	}

	Tamgurawstring(string v) {
        investigate |= is_string;
		//Do not forget your variable initialisation
		value = NULL;
		buffersize = 0;
		stringsize = v.size();
		if (stringsize != 0) {
			buffersize = stringsize + 1;
			value = new uchar[buffersize];
			strcpy_s((char*)value, buffersize, STR(v));
		}
	}

	~Tamgurawstring() {
		if (value != NULL)
			delete[] value;
	}
	//----------------------------------------------------------------------------------------------------------------------
	Tamgu* Put(Tamgu* index, Tamgu* v, short idthread);
    Tamgu* Loopin(TamguInstruction* ins, Tamgu* context, short idthread);

    Tamgu* Putvalue(Tamgu* v, short idthread) {
        string s = v->String();

        if (value == NULL) {
            stringsize = s.size();
            buffersize = stringsize + 1;
            value = new uchar[buffersize];
            strcpy_s((char*)value, buffersize, STR(s));
            return this;
        }

        if (buffersize <= s.size())
            return globalTamgu->Returnerror(e_string_overflow, idthread);

        strcpy((char*)value, STR(s));
        stringsize = s.size();
        return this;
    }

    Tamgu* Clonevalue(Tamgu* v, short idthread) {
        string s = v->String();

        if (value == NULL) {
            stringsize = s.size();
            buffersize = stringsize + 1;
            value = new uchar[buffersize];
            strcpy_s((char*)value, buffersize, STR(s));
            return this;
        }

        if (buffersize <= s.size())
            return globalTamgu->Returnerror(e_string_overflow, idthread);

        strcpy((char*)value, STR(s));
        stringsize = s.size();
        return this;
    }

	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);


	Tamgu* Vector(short idthread) {		
		if (value==NULL)
			return globalTamgu->Returnerror(e_unknown_expression, idthread);
		string v((char*)value);
		return globalTamgu->EvaluateVector(v, idthread);
	}

	Tamgu* Map(short idthread) {		
		if (value == NULL)
			return globalTamgu->Returnerror(e_unknown_expression, idthread);
		string v((char*)value);
		return globalTamgu->EvaluateMap(v, idthread);
	}

	Tamgu* Push(Tamgu* a) {
		if (value == NULL)
			return globalTamgu->Returnerror(e_string_overflow, globalTamgu->GetThreadid());

		string s = a->String();
		if ((stringsize + s.size()) >= buffersize)
			return globalTamgu->Returnerror(e_string_overflow, globalTamgu->GetThreadid());

		for (long i = 0; i < s.size(); i++)
			value[stringsize + i] = s[i];
		stringsize += s.size();
		return this;
	}

	void Pop(long i) {
		if (value == NULL || !stringsize)
			return;

		if (i == -1) {
			i = value[stringsize - 1] = 0;
			stringsize--;
			return;
		}

		if (i >= stringsize || i < 0)
			return;

		stringsize--;
		strcpy_s((char*)value + i, i, (char*)value + i + 1);
	}

	virtual short Type() {
		return Tamgurawstring::idtype;
	}

	

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
		return "rawstring";
	}

	

	bool isAtom() {
		return true;
	}

    bool checkAtomType(short ty) {
        return (ty == Tamgurawstring::idtype);
    }

	bool isValueContainer() {
		return true;
	}

	Tamgu* Atom(bool forced = false) {
		if (forced || !protect || reference) {
			Tamgurawstring* v = new Tamgurawstring;
			if (value != NULL) {
				v->value = new unsigned char[buffersize];
				v->buffersize = buffersize;
				v->stringsize = stringsize;
				strcpy((char*)v->value, (char*)value);
			}
			return v;
		}

		return this;
	}

    Tamgu* Atomref() {
        TamguReference* r = (TamguReference*)Atom();
        r->reference = 1;
        r->protect = false;
        return r;
    }


	void Storevalue(string& s) {
		if (value == NULL) {
			globalTamgu->Returnerror(e_string_overflow, globalTamgu->GetThreadid());
			return;
		}

		if ((stringsize + s.size()) >= buffersize) {
			globalTamgu->Returnerror(e_string_overflow, globalTamgu->GetThreadid());
			return;
		}

		for (long i = 0; i < s.size(); i++)
			value[stringsize + i] = s[i];
		stringsize += s.size();
	}

	void Storevalue(wstring& s) {
		string v;
		s_unicode_to_utf8(v, s);
		storevalue(v);
	}

    void storevalue(string s) {
        if (value == NULL) {
            globalTamgu->Returnerror(e_string_overflow, globalTamgu->GetThreadid());
            return;
        }
        
        if ((stringsize + s.size()) >= buffersize) {
            globalTamgu->Returnerror(e_string_overflow, globalTamgu->GetThreadid());
            return;
        }
        
        for (long i = 0; i < s.size(); i++)
            value[stringsize + i] = s[i];
        stringsize += s.size();
    }
    
    void storevalue(wstring s) {
        string v;
        s_unicode_to_utf8(v, s);
        storevalue(v);
    }
    

	void storevalue(wchar_t u) {
		wstring w;
		w = u;
		string v;
		s_unicode_to_utf8(v, w);
		storevalue(v);
	}

	void Forcedclean() {
		//we cannot reset or resize a rawstring in a thread, since they are not protected with locks.
		if (globalTamgu->isthreading) {
			globalTamgu->Returnerror(e_cannot_reset_a, globalTamgu->GetThreadid());
			return;
		}

		if (value != NULL) {
			delete[] value;
			value = NULL;
			stringsize = 0;
			buffersize = 0;
		}
	}

	//---------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
	bool isDeclared(short n) {
        return methods.check(n);
	}

	Tamgu* Newinstance(short, Tamgu* f = NULL) {
		return new Tamgurawstring;
	}

	Tamgu* Newvalue(Tamgu* a, short idthread) {
		return new Tamgurawstring(a->String());
	}


	TamguIteration* Newiteration(bool direction);

	static void AddMethod(TamguGlobal* g, string name, rawstringMethod func, unsigned long arity, string infos);
	
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



	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	Tamgu* MethodInitial(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		Tamgu* av = callfunc->Evaluate(0, contextualpattern, idthread);

		if (av->isNumber()) {
			stringsize = av->Integer();
			buffersize = stringsize + 1;
			value = new uchar[buffersize];
			value[0] = 0;
			return aTRUE;
		}


		string v = av->String();
		stringsize = v.size();
		buffersize = stringsize + 1;
		value = new uchar[buffersize];
		strcpy_s((char*)value, buffersize, STR(v));
		return aTRUE;
	}

	Tamgu* MethodSizeb(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->ProvideConstint(buffersize);
	}

	Tamgu* MethodResize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		if (globalTamgu->isthreading)
			return globalTamgu->Returnerror(e_cannot_reset_a, idthread);

		long bsz = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
		if (!bsz || bsz <= stringsize)
			return aFALSE;

		buffersize = bsz;
		uchar* v = new uchar[buffersize];
		strcpy_s((char*)v, buffersize, (char*)value);
		delete[] value;
		value = v;
		return aTRUE;
	}

	Tamgu* MethodHash(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodReverse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodOrd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodBytes(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodFormat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodBase(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodFill(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodPadding(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodParse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodPop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodParenthetic(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodTags(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodScan(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodEvaluate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodTohtml(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodToxml(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodEditdistance(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodReplace(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodGetstd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodGeterr(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodSplit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodSplite(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodTokenize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodStokenize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodCount(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodFind(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodRfind(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodRemovefirst(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodRemovelast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodIsutf8(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodNgrams(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodSlice(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodUtf8(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLatin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodDos(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodDostoutf8(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLeft(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodByteposition(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodCharposition(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodRight(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodMid(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodIsalpha(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodIsconsonant(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodIsvowel(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodIspunctuation(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodIsdigit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodExtract(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodIsupper(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodIslower(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodUpper(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodDeaccentuate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLower(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodStartWith(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodTrim(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodTrimleft(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodTrimright(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodInsert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodClear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    Tamgu* MethodRead(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodWrite(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
	//is returned by the Declaration method.
	Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
	}

	//---------------------------------------------------------------------------------------------------------------------
	unsigned long EditDistance(Tamgu* e);
	//---------------------------------------------------------------------------------------------------------------------
	void Clear() {
		if (value != NULL) {
			delete[] value;
			value = NULL;
			stringsize = 0;
			buffersize = 0;
		}
	}


	string JSonString() {
		string res;
        if (value != NULL) {
			string v((char*)value, stringsize);
            jstringing(res, v);
        }
		return res;
	}

	string String() {
		if (value == NULL)
			return "";
		return (char*)value;
	}

	wstring UString() {
		wstring res;
		if (value == NULL)
			return res;

		s_utf8_to_unicode(res, value, buffersize);
		return res;

	}
    
    void Setstring(string& v, short idthread) {
        if (value == NULL)
            v = "";
        v = (char*)value;
    }

    void Setstring(wstring& res, short idthread) {
        if (value == NULL)
            res = L"";
        else
            s_utf8_to_unicode(res, value, buffersize);
    }

	long Integer() {
		if (value == NULL)
			return 0;
		return (long)conversionintegerhexa((char*)value);
	}

	virtual long Size() {
		return stringsize;
	}

	float Decimal() {
		if (value == NULL)
			return 0;
		return convertfloat((char*)value);
	}

	double Float() {
		if (value == NULL)
			return 0;
		return convertfloat((char*)value);
	}

	bool Boolean() {
        return stringsize;
	}

	BLONG Long() {
		if (value == NULL)
			return 0;
		return conversionintegerhexa((char*)value);
	}

	Tamgu* in(Tamgu* context, Tamgu* a, short idthread);

	//we add the current value with a
	Tamgu* plus(Tamgu* b, bool itself) {
		if (value == NULL)
			return aNULL;

		if (itself) {
			string add = b->String();
			long sz = add.size();
			if ((stringsize + sz) >= buffersize)
				return globalTamgu->Returnerror(e_string_overflow, globalTamgu->GetThreadid());

			if (sz == 1)
				value[stringsize] = add[0];
			else {
				for (int i = 0; i < sz; i++)
					value[stringsize + i] = add[i];
			}
			stringsize += sz;
			return this;
		}

		string add = String() + b->String();
		return globalTamgu->Providewithstring(add);
	}


	//we remove a from the current value
	virtual Tamgu* minus(Tamgu* b, bool itself) {
		if (value == NULL)
			return aNULL;

		string add = b->String();
		long nb = add.size();

		if (itself) {
			if (nb > stringsize)
				return this;

			char* pt = strstr((char*)value, STR(add));
			if (pt == NULL)
				return this;
			strcpy(pt, pt + nb);
			stringsize -= nb;
			return this;
		}

        if (value==NULL)
            return globalTamgu->Providestring("");
        
        string v((char*)value,stringsize);

		if (nb > stringsize)
			return globalTamgu->Providewithstring(v);

		long pos = v.rfind(add);
		if (pos == string::npos)
			return globalTamgu->Providewithstring(v);
		add = v.substr(0, pos);
		pos += nb;
		add += v.substr(pos, v.size() - pos);

		return globalTamgu->Providewithstring(add);
	}


	Tamgu* less(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aFALSE;
#endif
		if (value == NULL)
			return aTRUE;

		string v = a->String();
		if (v < a->String())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* more(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aFALSE;
#endif
		if (value == NULL)
			return aFALSE;
		string v = a->String();
		if (v > a->String())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* same(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aFALSE;
#endif
		if (value == NULL) {
			if (a->Size() == 0)
				return aTRUE;
			return aFALSE;
		}

		string v = a->String();
		if (v == a->String())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* different(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aTRUE;
#endif
		if (value == NULL) {
			if (a->Size() != 0)
				return aTRUE;
			return aFALSE;
		}
		string v = a->String();
		if (v != a->String())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* lessequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aFALSE;
#endif
		if (value == NULL) {
			if (a->Size() == 0)
				return aTRUE;
			return aFALSE;
		}
		string v = a->String();
		if (v <= a->String())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* moreequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aFALSE;
#endif
		if (value == NULL) {
			if (a->Size() == 0)
				return aTRUE;
			return aFALSE;
		}
		string v = a->String();
		if (v >= a->String())
			return aTRUE;
		return aFALSE;
	}
};

class TamguIterationrawstring : public TamguIteration {
public:

	agnostring ref;

	TamguIterationrawstring(string v, bool d, TamguGlobal* g = NULL) : ref(v), TamguIteration(d, g) {
		if (d)
			ref = ref.revert();
	}

	Tamgu* Key() {
		return globalTamgu->ProvideConstint((long)ref.getcharpos());
	}

	Tamgu* Value() {
		return new Tamgurawstring(ref.current());
	}

	string Keystring() {
		
		return convertfromnumber((long)ref.getcharpos());
		
	}

	wstring Valueustring() {
		return ref.wcurrent();
	}

	string Valuestring() {
		return ref.current();
	}

	long Keyinteger() {
		return (long)ref.getcharpos();
	}

	long Valueinteger() {
		string s = ref.current();		
		return conversionintegerhexa(STR(s));
	}

	double Keyfloat() {
		return (double)ref.getcharpos();
	}

	double Valuefloat() {
		string s = ref.current();
		return convertfloat(STR(s));
	}

	void Next() {
		ref.following();
	}

	Tamgu* End() {
        return booleantamgu[ref.end()];
    }

	Tamgu* Begin() {
		ref.begin();
		return aTRUE;
	}


};

#endif









