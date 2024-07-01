/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguwtable.h
 Date       : 2017/07/26
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamguwtable_h
#define tamguwtable_h

#include "tamguwidget.h"
class Tamguwtable;

class TableRow : public Fl_Table {
private:
	Fl_Color cell_bgcolor;				// color of cell's bg color
	Fl_Color cell_fgcolor;				// color of cell's fg color
	Fl_Font font;
	int fontsize;
	hmap<int, hmap<int, string> > rowcolarray;
	hmap<int, string> rowheader;
	hmap<int, string> colheader;
	int maxrow;
	int maxcol;
	int cell_alignement;
	int cell_alignement_header_col;
	int cell_alignement_header_row;

protected:
	void draw_cell(TableContext context, int R = 0, int C = 0, int X = 0, int Y = 0, int W = 0, int H = 0);
	static void table_callback(Fl_Widget*, void*);

public:
	string cell(int R, int C) {
		if (rowcolarray.find(R) != rowcolarray.end())
		if (rowcolarray[R].find(C) != rowcolarray[R].end())
			return rowcolarray[R][C];
		return "";
	}


	void setalignmentheader(int c, bool header) {
		if (header)
			cell_alignement_header_col = c;
		else
			cell_alignement_header_row = c;
	}

	void setalignment(int c) {
		cell_alignement = c;
	}

	void clear() {
		rowheader.clear();
		colheader.clear();
		rowcolarray.clear();
		maxrow = 0;
		maxcol = 0;
		rows(0);		// implies clearing selection
		cols(0);
		Fl_Table::clear();	// clear the table
	}

	void addrowheader(int i, string value) {
		rowheader[i] = value;
	}

	void addcolumnheader(int i, string value) {
		colheader[i] = value;
	}

	void cell(int R, int C, string value) {
		if (rowcolarray.find(R) == rowcolarray.end()) {
			hmap<int, string> col;
			rowcolarray[R] = col;
		}
		if (R > maxrow) {
			maxrow = R;
			rows(R + 1);
		}
		if (C > maxcol) {
			maxcol = C;
			cols(C + 1);
		}
		rowcolarray[R][C] = value;
	}


	TableRow(Tamguwtable* table, int x, int y, int w, int h, const char *l = 0) : Fl_Table(x, y, w, h, l) {

		cell_alignement_header_col = FL_ALIGN_LEFT;
		cell_alignement_header_row = FL_ALIGN_LEFT;
		cell_alignement = FL_ALIGN_LEFT;
		cell_bgcolor = FL_WHITE;
		cell_fgcolor = FL_BLACK;
		font = FL_HELVETICA;
		fontsize = 16;
		maxrow = 0;
		maxcol = 0;
		callback(&table_callback, (void*)table);
	}

	Fl_Color GetCellFGColor() const { return(cell_fgcolor); }
	Fl_Color GetCellBGColor() const { return(cell_bgcolor); }
	void SetCellFGColor(Fl_Color val) { cell_fgcolor = val; }
	void SetCellBGColor(Fl_Color val) { cell_bgcolor = val; }
	void Font(Fl_Font val) {
		font = val;
	}
	void Fontsize(int sz) {
		fontsize = sz;
	}
	Fl_Font Font() {
		return font;
	}
	int Fontsize() {
		return fontsize;
	}
};

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamguwtable;
//This typedef defines a type "wtableMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguwtable::*wtableMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamguwtable : public Tamguwidget {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static basebin_hash<wtableMethod> methods;
    
    

    static short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
	
	TableRow* table;
	char txt[1024];

    //---------------------------------------------------------------------------------------------------------------------
	Tamguwtable(TamguGlobal* g, Tamgu* parent = NULL) : Tamguwidget(g, parent) {
        //Do not forget your variable initialisation
		table = NULL;
		

    }

	void itemclose() {
		if (table != NULL)
			delete table;
		table = NULL;
		clean();
	}

	Fl_Widget* Widget() {
		return table;
	}

	void ResetWidget() {
        if (table != NULL)
            delete table;
		table = NULL;
	}

    //----------------------------------------------------------------------------------------------------------------------
	Tamgu* Put(Tamgu* index, Tamgu* value, short idthread);
	Tamgu* Eval(Tamgu* context, Tamgu* index, short idthread);


    short Type() {
        return Tamguwtable::idtype;
    }

    string Typename() {
        return "wtable";
    }

    Tamgu* Atom(bool forced=false) {
        return this;
    }

    unsigned long CallBackArity() {
        return P_THREE;
    }

    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        return methods.check(n);
    }

    Tamgu* Newinstance(short, Tamgu* f = NULL) {
		Tamguwtable* a = new Tamguwtable(globalTamgu);
		a->function = f;
		return a;
    }

	TamguIteration* Newiteration(bool direction);

    static void AddMethod(TamguGlobal* g, string name, wtableMethod func, unsigned long arity, string infos);
    
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
	Tamgu* MethodCreate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodClear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodAdd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodCell(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodCellAlignment(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodCellAlignmentHeaderCol(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodCellAlignmentHeaderRow(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodRow(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodColumn(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodRowHeight(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodColumnWidth(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodFont(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodFontSize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodColorbg(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodColorfg(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodWhen(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodTableSelectionColor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodBoxType(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodRowHeader(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodColumnHeader(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodRowHeaderHeight(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodColumnHeaderWidth(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    

    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		if (globalTamgu->Error(idthread))
			return globalTamgu->Errorobject(idthread);

		if (Stopall())
			return aNULL;

		///Specific section to handle threads... One exception the redraw function...
		if (idthread && callfunc->Size() != 0) {
			//In this case, we do not want to process the method, we postpone its activation
			RecordGUIObject(callfunc, idthread);
			return aNULL;
		}

		return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }

    void Clear() {
        //To set a variable back to empty
		
    }

    void Clean() {
        //Your cleaning code
    }

	long Size() {
		return 0;
	}

	uchar Byte() {
		
		return 0;
	}

	long Integer() {
		
		return 0;
	}
	
	double Float() {
		
		return 0;
	}
	
	string String() {
		
		return "";
	}

	
	bool Boolean() {
        if (table != NULL)
            return true;
        return false;
	}

    
    //Basic operations    
	Tamgu* andset(Tamgu* a, bool autoself) {
		
	    return this;
	}

	Tamgu* orset(Tamgu* a, bool autoself) {
		
	    return this;
	}

	Tamgu* xorset(Tamgu* a, bool autoself) {
		
	    return this;
	}

	Tamgu* plus(Tamgu* a, bool autoself) {
		
	    return this;
	}
	Tamgu* minus(Tamgu* a, bool autoself) {
		
	    return this;
	}

	Tamgu* multiply(Tamgu* a, bool autoself) {
		
	    return this;
	}

	Tamgu* divide(Tamgu* a, bool autoself) {
		
	    return this;

	}
	Tamgu* mod(Tamgu* a, bool autoself) {
		
	    return this;
	}

	Tamgu* shiftleft(Tamgu* a, bool autoself) {
		
	    return this;
	}


	Tamgu* shiftright(Tamgu* a, bool autoself) {
		
	    return this;
	}

	Tamgu* power(Tamgu* a, bool autoself) {
		
	    return this;
	}

	Tamgu* less(Tamgu* a) {
		return aFALSE;
	}
	Tamgu* more(Tamgu* a) {
		return aFALSE;
	}
	Tamgu* same(Tamgu* a) {
		return aFALSE;
	}
	Tamgu* different(Tamgu* a) {
		return aFALSE;
	}

	Tamgu* lessequal(Tamgu* a) {
		return aFALSE;
	}

	Tamgu* moreequal(Tamgu* a) {
		return aFALSE;
	}

	Tamgu* plusplus() {
		return this;
	}

	Tamgu* minusminus() {
		return this;
	}
};

#endif








