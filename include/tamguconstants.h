/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguconstants.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamguconstants_h
#define tamguconstants_h

//--------------------------------------------------------------------
class TamguGlobal;
class TamguConstiteration;
class TamguPredicate;

//--------------------------------------------------------------------
const uchar b_none = 0;
const uchar b_short = 1;
const uchar b_int = 2;
const uchar b_long = 4;
const uchar b_forcedlong = 8;
const uchar b_decimal = 16;
const uchar b_float = 32;
const uchar b_string = 64;
const uchar b_ustring = 128;
const uchar b_allstrings = b_string | b_ustring;
const uchar b_allnumbers = b_short | b_int | b_long | b_forcedlong | b_decimal | b_float;
const uchar b_letself = b_int | b_float | b_string;
const uchar b_longdecimal = b_long | b_decimal;
const uchar b_floats = b_decimal | b_float;
//--------------------------------------------------------------------
#define _setdebugfull(i,j) Tamgudebug* _d=NULL; bool _db = globalTamgu->debugmode; if (_db) _d=new Tamgudebug(i,j)
#define _setdebugmin(i) Tamgudebug* _d=NULL; bool _db = globalTamgu->debugmode; if (_db) _d=new Tamgudebug(i)
#define _debugpush(i) globalTamgu->Current(i, idthread); if (_db) _d->push()
#define _debugpop() if (_db) _d->pop()
#define _cleandebugfull if (_db) delete _d
#define _cleandebugmin if (_db) delete _d
//--------------------------------------------------------------------
Exporting Tamgu* SelectContainer(Tamgu* context, short idthread);
Exporting Tamgu* Selectacontainer(Tamgu* context, short idthread);
Exporting Tamgu* Selectavector(Tamgu* context);
Exporting Tamgu* SelectaVector(Tamgu* context);
Exporting Tamgu* Selectamap(Tamgu* context);
Exporting Tamgu* Selectasvector(Tamgu* context);
Exporting Tamgu* Selectauvector(Tamgu* context);
Exporting Tamgu* Selectaivector(Tamgu* context);
Exporting Tamgu* Selectahvector(Tamgu* context);
Exporting Tamgu* Selectalvector(Tamgu* context);
Exporting Tamgu* Selectadvector(Tamgu* context);
Exporting Tamgu* Selectafvector(Tamgu* context);
Exporting Tamgu* Selectabvector(Tamgu* context);
Exporting Tamgu* Selectaprimemapss(Tamgu* context);

void convertbytepositiontochar(vector<long>& v, agnostring& s);
void convertbytepositiontochar(vector<double>& v, agnostring& s);
//-----------------------------------------------------------------------------
Tamgu* Mapcompare(Tamgu*a, Tamgu*b, TamguGlobal* global);
//-----------------------------------------------------------------------------
Exporting char StringIndexes(string& svalue, Tamgu* index, long& ileft, long& iright, short idthread);
Exporting char StringIndexes(wstring& svalue, Tamgu* index, long& ileft, long& iright, short idthread);
//-----------------------------------------------------------------------------
unsigned long StringEditDistance(wstring& value, wstring& s2);
unsigned long StringEditDistance(string& value, string& s2);
string StringOr(string v, string sub);
wstring StringOr(wstring v, wstring sub);
string StringAnd(string v, string sub);
wstring StringAnd(wstring v, wstring sub);
string StringXor(string v, string sub);
wstring StringXor(wstring v, wstring sub);
string StringMinus(string v, string sub);
wstring StringMinus(wstring v, wstring sub);
//-----------------------------------------------------------------------------
double DoubleAnd(double value, double l);
double DoubleOr(double value, double l);
double DoubleXor(double value, double l);
double DoubleShiftleft(double value, long l);
double DoubleShiftright(double value, long l);
float FloatAnd(float value, float l);
float FloatOr(float value, float l);
float FloatXor(float value, float l);
float FloatShiftleft(float value, long l);
float FloatShiftright(float value, long l);

bool IsLong(BLONG v);
bool IsShort(BLONG v);
//-----------------------------------------------------------------------------
bool v_comma_split_string(string& thestr, vector<string>& v);
bool v_comma_split_string(wstring& thestr, vector<wstring>& v);
bool v_comma_split_decimal(string& thestr, vector<float>& v);
bool v_comma_split_float(string& thestr, vector<double>& v);
bool v_comma_split_int(string& thestr, vector<long>& v);
bool v_comma_split_long(string& thestr, vector<BLONG>& v);
bool v_comma_split_byte(string& thestr, vector<uchar>& v);
bool v_comma_split_short(string& thestr, vector<short>& v);
//-----------------------------------------------------------------------------
#define TamguCASEFUNCTION -9876
//-----------------------------------------------------------------------------
inline bool Arity(const unsigned long arity, long sz) {
	unsigned long a = 1 << sz;
	if ((arity&a) != a)
		return false;
	return true;
}
//-----------------------------------------------------------------------------
void Flattening(Tamgu* ret, Tamgu* res);
//-----------------------------------------------------------------------------
char restate_output(long o, long output);
char redirect_output(string& filename, long output);
void clear_output();
//-----------------------------------------------------------------------------
double localrandom(long mx);
double a_localrandom(long mx);
//-----------------------------------------------------------------------------------
unsigned long long Hashkey(string& s);
unsigned long long Hashkey(wstring& s);
//-----------------------------------------------------------------------------------
#ifdef WIN32
long gettimeofday(struct timeval *tv, struct timezone *tz);
#endif

typedef bool(*PauseBack)(void*);
bool WaitingFor(double tm, PauseBack pb, void* data);
long timeminus(double init, timeval& tempsfinal);
double plustime(timeval& tempsinitial, timeval& tempsfinal);
double minustime(timeval& tempsfinal, timeval& tempsinitial);
void Wait(double tm);
//--------------------------------------------------------------------
extern Tamgu* booleantamgu[2];
//-----------------------------------------------------------------------------------
bool DirectoryCreation(char* noms, long pos);
string NormalizeFileName(string n);
//-----------------------------------------------------------------------------------
uchar Selecttype(Tamgu* context);
Tamgu* Storealongtype(Tamgu* context, Tamgu* a, short idthread, uchar& addvalue);
//-----------------------------------------------------------------------------------
extern bool errorsprintf;
void wrongSprintf(const wchar_t* expression,
	const wchar_t* function,
	const wchar_t* file,
	unsigned int line,
	uintptr_t pReserved);
//-----------------------------------------------------------------------------------


//Open mode to open files with ifstream
#ifdef APPLE
#define openMode std::ios::in|std::ios::binary
#else
#define openMode ios::in|ios::binary
#endif

const unsigned long P_NONE = 1 << 0;
const unsigned long P_ONE = 1 << 1;
const unsigned long P_TWO = 1 << 2;
const unsigned long P_THREE = 1 << 3;
const unsigned long P_FOUR = 1 << 4;
const unsigned long P_FIVE = 1 << 5;
const unsigned long P_SIX = 1 << 6;
const unsigned long P_SEVEN = 1 << 7;
const unsigned long P_EIGHT = 1 << 8;
const unsigned long P_NINE = 1 << 9;
const unsigned long P_TEN = 1 << 10;
const unsigned long P_ELEVEN = 1 << 11;
const unsigned long P_TWELVE = 1 << 12;
const unsigned long P_THIRTEEN = 1 << 13;
const unsigned long P_FOURTEEN = 1 << 14;
const unsigned long P_FIFTEEN = 1 << 15;
const unsigned long P_FULL = -1;
const unsigned long P_ATLEASTONE = P_FULL^P_NONE;
const unsigned long P_ATLEASTTWO = P_ATLEASTONE^P_ONE;
const unsigned long P_ATLEASTTHREE = P_ATLEASTTWO^P_TWO;
const unsigned long P_ATLEASTFOUR = P_ATLEASTTHREE^P_THREE;
const unsigned long P_ATLEASTFIVE = P_ATLEASTFOUR^P_FOUR;
const unsigned long P_ATLEASTSIX = P_ATLEASTFIVE^P_FIVE;
const unsigned long P_ATLEASTSEVEN = P_ATLEASTSIX^P_SIX;
const unsigned long P_ATLEASTEIGHT = P_ATLEASTSEVEN^P_SEVEN;
const unsigned long P_ATLEASTNINE = P_ATLEASTEIGHT^P_EIGHT;
const unsigned long P_ATLEASTTEN = P_ATLEASTNINE^P_NINE;
const unsigned long P_ATLEASTELEVEN = P_ATLEASTTEN^P_TEN;
const unsigned long P_ATLEASTTWELVE = P_ATLEASTELEVEN^P_ELEVEN;
const unsigned long P_ATLEASTTHIRTEEN = P_ATLEASTTWELVE^P_TWELVE;
const unsigned long P_ATLEASTFOURTEEN = P_ATLEASTTHIRTEEN^P_THIRTEEN;
const unsigned long P_ATLEASTFIFTEEN = P_ATLEASTFOURTEEN^P_FOURTEEN;

//--------------------------------------------------------------------------------
const long CIRCULARDETECTION = -25;
//--------------------------------------------------------------------------------
#define predicatename globalTamgu->gpredicatename
#define predicatedico globalTamgu->gpredicatedico
#define predicatezone globalTamgu->gpredicatezone
#define predicatedependency globalTamgu->gpredicatedependency
#define predicatefeature globalTamgu->gpredicatefeature
//--------------------------------------------------------------------------------

//Some external variables...
class TamguCallBreak;
class TamguLet;

extern Exchanging Tamgu* aNULL;
extern Exchanging Tamgu* aUNIVERSAL;
extern Exchanging Tamgu* aTRUE;
extern Exchanging Tamgu* aFALSE;

extern Exchanging Tamgu* aMINUSTEN;
extern Exchanging Tamgu* aMINUSNINE;
extern Exchanging Tamgu* aMINUSEIGHT;
extern Exchanging Tamgu* aMINUSSEVEN;
extern Exchanging Tamgu* aMINUSSIX;
extern Exchanging Tamgu* aMINUSFIVE;
extern Exchanging Tamgu* aMINUSFOUR;
extern Exchanging Tamgu* aMINUSTHREE;
extern Exchanging Tamgu* aMINUSTWO;
extern Exchanging Tamgu* aMINUSONE;
extern Exchanging Tamgu* aZEROPOINTZERO;
extern Exchanging Tamgu* aZERO;
extern Exchanging Tamgu* aONE;
extern Exchanging Tamgu* aTWO;
extern Exchanging Tamgu* aTHREE;
extern Exchanging Tamgu* aFOUR;
extern Exchanging Tamgu* aFIVE;
extern Exchanging Tamgu* aSIX;
extern Exchanging Tamgu* aSEVEN;
extern Exchanging Tamgu* aEIGHT;
extern Exchanging Tamgu* aNINE;
extern Exchanging Tamgu* aTEN;
extern Exchanging Tamgu* aELEVEN;
extern Exchanging Tamgu* aTWELVE;
extern Exchanging Tamgu* aTHIRTEEN;
extern Exchanging Tamgu* aFOURTEEN;
extern Exchanging Tamgu* aFIFTEEN;
extern Exchanging Tamgu* aSIXTEEN;
extern Exchanging Tamgu* aSEVENTEEN;
extern Exchanging Tamgu* aEIGHTEEN;
extern Exchanging Tamgu* aNINETEEN;
extern Exchanging Tamgu* aTWENTY;
extern Exchanging Tamgu* aTHIRTYTWO;
extern Exchanging Tamgu* aSIXTYFOUR;

extern Exchanging Tamgu* aEMPTYLISP;
extern Exchanging Tamgu* aEMPTYSTRING;
extern Exchanging Tamgu* aEMPTYUSTRING;
extern Exchanging Tamgu* aBREAK;
extern Exchanging Tamgu* aCONTINUE;
extern Exchanging Tamgu* aRETURN;
extern Exchanging Tamgu* aPIPE;
extern Exchanging Tamgu* aNOELEMENT;
extern Exchanging Tamgu* aDEFAULT;
extern Exchanging Tamgu* aEND;
extern Exchanging Tamgu* aRAISEERROR;
extern Exchanging TamguCallBreak* aBREAKFALSE;
extern Exchanging TamguCallBreak* aBREAKTRUE;
extern Exchanging TamguCallBreak* aBREAKZERO;
extern Exchanging TamguCallBreak* aBREAKONE;
extern Exchanging Tamgu* aAFFECTATION;
extern Exchanging TamguConstiteration* aITERNULL;
extern Exchanging TamguPredicate* aFAIL;
extern Exchanging TamguPredicate* aCUT;
extern Exchanging TamguPredicate* aSTOP;
extern Exchanging Tamgu* aHASKELL;
extern Exchanging TamguDeclaration* aNULLDECLARATION;
extern Exchanging TamguLet* aNULLLet;
extern Exchanging Tamgu* aNOTHING;

//---------------------------------------------------------------------------
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
//---------------------------------------------------------------------------
#ifdef DOSOUTPUT
#define conversion2dos(x) s_utf8_to_dos(USTR(x))
#else
#define conversion2dos(X) X
#endif

//---------------------------------------------------------------------------
const short u_tamguintsz = sizeof(long);
const short u_tamgudoublesz = sizeof(double);
const short u_tamgulongsz = sizeof(BLONG);

union u_tamgudouble {
	double v;
	unsigned short t[u_tamgudoublesz];
	u_tamgudouble(double e) {
		v = e;
	}
};


union u_tamguinteger {
	long v;
	unsigned short t[u_tamguintsz];
	u_tamguinteger(long e) {
		v = e;
	}
};

union u_tamgulong {
	BLONG v;
	unsigned short t[u_tamgulongsz];
	u_tamgulong(BLONG e) {
		v = e;
	}
};
//---------------------------------------------------------------------------
//The order should follow the order in which Createid are implemented in TamguGlobal::RecordConstantNames
typedef enum{
	a_null,
	a_true, a_false, a_zero, a_one, a_boolean, a_minusone, a_byte, a_short, a_int, a_long, a_decimal, a_fraction, a_float,
	a_bloop, a_iloop, a_lloop, a_dloop, a_floop, 
	a_intthrough, a_longthrough, a_decimalthrough, a_floatthrough, a_stringthrough, a_ustringthrough, a_vectorthrough, a_mapthrough,
	a_string, a_ustring, a_sloop, a_uloop,
	a_constvector, a_vector, a_bvector, a_fvector, a_ivector, a_hvector, a_svector, a_uvector, a_dvector, a_lvector, a_list,
	a_constmap, a_map, a_treemap, a_primemap, a_binmap, a_mapss,
	a_error, a_const, a_none, a_pipe,
	a_break, a_continue, a_return,
	a_empty, a_mainframe,
	a_call, a_callfunction, a_callthread, a_callmethod, a_callprocedure, a_callindex, a_calltaskell, a_lambda,
	a_variable,	a_declaration, a_instructions, a_function, a_frame, a_frameinstance, a_extension, 
	a_initial, a_iteration, a_default,
	a_forinrange, a_sequence, a_self, 
	a_idreturnvariable, a_breaktrue, a_breakfalse, a_vectormerge, 
	a_instructionequ, a_let, a_assign, a_tamgu, 	
	a_this, a_index, a_interval, a_type, a_final,
	a_infinitive, a_cycle, a_replicate,
	a_fail, a_cut, a_stop, a_predicateentree, a_universal, a_asserta, a_assertz, a_retract, a_remove, a_predicatevar, a_predicate,
	a_term, a_instance, a_predicateruleelement, a_predicatecontainer, a_predicaterule, a_predicateinstruction, a_knowledgebase, 
	a_dependencybase, a_predicatedomain, a_predicatelaunch,
	a_predicateelement, a_parameterpredicate, a_predicateevaluate, a_dependency,
	a_stream, a_affectation,
	a_plusequ, a_minusequ, a_multiplyequ, a_divideequ, a_modequ, a_powerequ, a_shiftleftequ, a_shiftrightequ, a_orequ, a_xorequ, a_andequ, a_mergeequ, a_combineequ, a_addequ,
	a_plus, a_minus, a_multiply, a_divide, a_power, a_shiftleft, a_shiftright, a_mod, a_or, a_xor, a_and, a_merge, a_combine, a_add, a_conjunction, a_disjunction,
	a_less, a_more, a_same, a_different, a_lessequal, a_moreequal, 
	a_plusplus, a_minusminus, a_in, a_notin, a_match, a_bloc, a_blocloopin, a_filein, a_blocboolean, a_parameter,
	a_if, a_try, a_switch, a_while, a_for, a_catchbloc, a_booleanand, a_booleanor, a_taskell, a_forcedaffectation,
	a_square, a_cube, a_counter, a_synode, 
	a_modifydependency, a_actionvariable, a_taskelldeclaration, a_drop,  a_concept, a_negation, a_taskellinstruction, a_methods,
    a_treg, a_table, a_ifnot, a_Nothing, a_preg, a_rules, a_iftaskell, a_casetaskell, a_size, a_post, a_fibre, a_booleanxor, a_push,
    a_quote, a_cons, a_cond, a_atom, a_eq, a_cadr, a_defun, a_label, a_atomp, a_numberp, a_consp, a_zerop, a_nullp, a_block,
    a_setq, a_append, a_eval, a_key, a_keys, a_load, a_body, a_lisp
} tamgubasictypes;

inline bool Isnumber(short a) {
	if (a >= a_short && a <= a_floatthrough)
		return true;
	return false;
}

inline bool Istring(short a) {
	if (a >= a_string && a <= a_ustringthrough)
		return true;
	return false;
}

inline void jstringing(string& res, string& value) {
	if (value.find("\\") != -1)
		value = s_replacestrings(value, "\\", "\\\\");

	if (value.find("\"") == -1) {
		res += "\"";
		res += value;
		res += "\"";
	}
	else {
		string v = s_replacestrings(value, "\"", "\\\"");
		res += "\"";
		res += v;
		res += "\"";
	}
}

inline void stringing(string& res, string& value) {
	if (value.find("'") != -1) {
		if (value.find("\"") != -1) {
			string v = s_replacestrings(value, "\"", "\\\"");
			res += "\"";
			res += v;
			res += "\"";
		}
		else {
			res += "\"";
			res += value;
			res += "\"";
		}
	}
	else {
		res += "'";
		res += value;
		res += "'";
	}
}

inline void ustringing(wstring& res, wstring& value) {
	if (value.find(L"'") != -1) {
		if (value.find(L"\"") != -1) {
			wstring v = s_replacestring(value, L"\"", L"\\\"");
			res += L"\"";
			res += v;
			res += L"\"";
		}
		else {
			res += L"\"";
			res += value;
			res += L"\"";
		}
	}
	else {
		res += L"'";
		res += value;
		res += L"'";
	}
}

inline void sstringing(string& res, string value) {
	if (value.find("'") != -1) {
		if (value.find("\"") != -1) {
			string v = s_replacestrings(value, "\"", "\\\"");
			res += "\"";
			res += v;
			res += "\"";
		}
		else {
			res += "\"";
			res += value;
			res += "\"";
		}
	}
	else {
		res += "'";
		res += value;
		res += "'";
	}
}

inline void jstringing(stringstream& res, string& value) {
	if (value.find("\"") == -1)
		res << "\"" << value << "\"";
	else {
		string v = s_replacestrings(value, "\"", "\\\"");
		res << "'" << v << "'";
	}
}


inline void stringing(stringstream& res, string& value) {
	if (value.find("'") != -1) {
		if (value.find("\"") != -1)
			res << "@\"" << value << "\"@";
		else
			res << "\"" << value << "\"";
	}
	else
		res << "'" << value << "'";
}

inline void ujstringing(std::wstringstream& res, wstring& value) {
	if (value.find(L"\"") == -1)
		res << L"\"" << value << L"\"";
	else {
		wstring v = s_replacestring(value, L"\"", L"\\\"");
		res << L"'" << v << L"'";
	}
}


inline void ustringing(std::wstringstream& res, wstring& value) {
	if (value.find(L"'") != -1) {
		if (value.find(L"\"") != -1)
			res << L"@\"" << value << L"\"@";
		else
			res << L"\"" << value << L"\"";
	}
	else
		res << L"'" << value << L"'";
}
#endif









