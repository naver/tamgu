/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgubitmap.h
 Date       : 2017/07/15
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgubitmap_h
#define tamgubitmap_h

#include "tamgugui.h"
//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgubitmap;
//This typedef defines a type "bitmapMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgubitmap::*bitmapMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamgubitmap : public TamguObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static basebin_hash<bitmapMethod> methods;
    
    

    static short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    Fl_Bitmap* bitmap;
	uchar* bm;
	int szw, szh;

    //---------------------------------------------------------------------------------------------------------------------
    Tamgubitmap(Fl_Bitmap* v, TamguGlobal* g, Tamgu* parent = NULL) : TamguObject(g, parent) {
        //Do not forget your variable initialisation
        bitmap=v;
		bm = NULL;
    }

    Tamgubitmap() : TamguObject() {
        //Do not forget your variable initialisation
        bitmap=NULL;
		szw = 0;
		szh = 0;
		bm = NULL;
    }

	~Tamgubitmap() {
		if (bitmap != NULL)
			delete bitmap;
		if (bm != NULL)
			delete[] bm;
	}

    //----------------------------------------------------------------------------------------------------------------------

    short Type() {
        return Tamgubitmap::idtype;
    }

    string Typename() {
        return "bitmap";
    }

	bool duplicateForCall() {
		return false;
	}

    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        return methods.check(n);
    }

    Tamgu* Newinstance(short, Tamgu* f = NULL) {
		return new Tamgubitmap;
    }

    static void AddMethod(TamguGlobal* g, string name, bitmapMethod func, unsigned long arity, string infos);

	static bool InitialisationModule(TamguGlobal* g, string version);


	
     void Setidtype(TamguGlobal* global);
     void Methods(Tamgu* v) {
            for (const auto& it : globalTamgu->infomethods[idtype])
                 v->storevalue(it.first);
      }

      string Info(string n) {
            if (globalTamgu->infomethods[idtype].find(n) !=  globalTamgu->infomethods[idtype].end())
              return globalTamgu->infomethods[idtype][n];
             return "Unknown method";
	}



	bool isString() {
		return false;
	}

	bool isNumber() {
		return false;
	}

	bool isContainer() {
		return false;
	}

	virtual bool isValueContainer() {
		return false;
	}

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
    //Tamgu* MethodSize(TamguGlobal* global,Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {return aZERO;}

	Tamgu* MethodInitial(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		//If you do not have any parameters, then your function might return whatever you want... Here itself
		Tamgu* kbitmaps = callfunc->Evaluate(0, aNULL, idthread);
		if (!kbitmaps->isVectorContainer())
			return globalTamgu->Returnerror("WND(809): Wrong type for the first parameter", idthread);

		Tamgu* ke = callfunc->Evaluate(1, aNULL, idthread);

		szw = ke->Integer();
		ke->Release();
		ke = callfunc->Evaluate(2, aNULL, idthread);
		szh = ke->Integer();
		ke->Release();

		int sz = kbitmaps->Size();

		int tst = (szw*szh) >> 3;
		if (tst != sz)
			return globalTamgu->Returnerror("WND(811): Wrong size for the bitmap. sz=(width*height)/8", idthread);

		int rowBytes = (szw + 7) >> 3;
		bm = new uchar[rowBytes*szh];
		for (int i = 0; i < sz; i++)
			bm[i] = (uchar)kbitmaps->getinteger(i);
		bitmap = new Fl_Bitmap(bm, szw, szh);
		kbitmaps->Release();
		return this;
	}

	Tamgu* MethodLoad(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		if (bitmap != NULL)
			return globalTamgu->Returnerror("WND(808): Bitmap already loaded", idthread);
		Tamgu* kbitmaps = callfunc->Evaluate(0, contextualpattern, idthread);
		szw = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
		szh = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
		if (!kbitmaps->isVectorContainer())
			return globalTamgu->Returnerror("WND(809): Wrong type for the first parameter", idthread);

		int sz = kbitmaps->Size();
		int tst = (szw*szh) >> 3;
		if (tst != sz)
			return globalTamgu->Returnerror("WND(811): Wrong size for the bitmap. sz=(width*height)/8", idthread);

		int rowBytes = (szw + 7) >> 3;
		bm = new uchar[rowBytes*szh];

		for (int i = 0; i < sz; i++)
			bm[i] = (uchar)kbitmaps->getinteger(i);
		bitmap = new Fl_Bitmap(bm, szw, szh);
		//delete[] bm;
		return aTRUE;
	}

    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }
};

#endif

