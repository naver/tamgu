/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguimatrix.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamguimatrix_h
#define tamguimatrix_h

#include "tamgu.h"
#include "vecte.h"
#include "tamgustring.h"
#include "matrix.h"

#ifdef WIN32
#include <math.h>
#endif

#define NMAX 100
#define TINY 1.5e-16
#define REAL double
#define ZERO (double)0.0
#define ONE (double)1.0
#define TWO (double)2.0

#ifdef UNIX
#include <math.h>
#include <float.h>
#define ABS(x) fabs((double)x)
#define TRUE true
#define FALSE false
#else
#define ABS(x) abs((double)x)
#endif

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamguimatrix;
//This typedef defines a type "imatrixMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguimatrix::*imatrixMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
class Tamguimatrix : public TamguObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging basebin_hash<imatrixMethod> methods;
	static Exchanging hmap<string, string> infomethods;
	static Exchanging basebin_hash<unsigned long> exported;

	static Exchanging short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	TAMGUIMATRIX<TAMGUILINEINTEGER> values;

	long rowsize;
	long columnsize;

	//---------------------------------------------------------------------------------------------------------------------
	Tamguimatrix(TamguGlobal* g, Tamgu* parent = NULL) : TamguObject(g, parent) {
     investigate |= is_number;
		//Do not forget your variable initialisation
	}

	Tamguimatrix() {
     investigate |= is_number;
		//Do not forget your variable initialisation
	}

	void Methods(Tamgu* v) {

		for (auto& it : infomethods)
			v->storevalue(it.first);
	}
	string Info(string n) {
		if (infomethods.find(n) != infomethods.end())
			return infomethods[n];
		return "Unknown method";
	}
	//----------------------------------------------------------------------------------------------------------------------
	Tamgu* Put(Tamgu* v, Tamgu* i, short idthread);
	Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread);
	short Type() {
		return Tamguimatrix::idtype;
	}
	

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
		return "imatrix";
	}
	
	
	bool isBoolean() {
		return false;
	}
	bool isFrame() {
		return false;
	}
	Tamgu* Atom(bool forced = false) {
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
		return new Tamguimatrix;
	}
	TamguIteration* Newiteration(bool direction) {
		return aITERNULL;
	}
	static void AddMethod(TamguGlobal* g, string name, imatrixMethod func, unsigned long arity, string infos);
	static bool InitialisationModule(TamguGlobal* g, string version);
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	Tamgu* MethodInitial(Tamgu* context, short idthread, TamguCall* callfunc) {
		rowsize = callfunc->Evaluate(0, context, idthread)->Integer(); //would match i as in the example above: mynewtype toto(i,j);
		columnsize = 1;
		if (callfunc->Size() == 2)
			columnsize = callfunc->Evaluate(1, context, idthread)->Integer(); //would match j as in the example above: mynewtype toto(i,j);	
		values.init(rowsize);
		return aTRUE;
	}
	Tamgu* MethodTransposed(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodMatrixSize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodDeterminant(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodSum(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodProduct(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodInversion(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    
    Tamgu* MethodFormat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        locking();
        
        string sep=callfunc->Evaluate(0,contextualpattern,idthread)->String();
        string nxt=callfunc->Evaluate(1,contextualpattern,idthread)->String();
        string zro=callfunc->Evaluate(2,contextualpattern,idthread)->String();
        
        stringstream buff;
        
        for (long r = 0; r < rowsize; r++) {
            for (long c = 0; c < columnsize; c++) {
                if (c)
                    buff << sep;
                if (values.check(r) && values[r].check(c))
                    buff << values[r][c];
                else
                    buff << zro;
            }
            buff << nxt;
        }
        unlocking();
        return globalTamgu->Providestring(buff.str());
    }

	//---------------------------------------------------------------------------------------------------------------------
	//ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
	//is returned by the Declaration method.
	Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		locking();
		contextualpattern = (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
        unlocking();
        return contextualpattern;
	}
	void cleaning() {
		values.clear();
		rowsize = 0;
		columnsize = 0;
	}
	void init(long rs, long cs) {
		rowsize = rs;
		columnsize = cs;
		values.init(rs);
	}
	void populate(long r, long c, long val) {
		values[r][c] = val;
	}
	void populate(Tamgu* krow, Tamgu* kcol, Tamgu* value) {
		long r = krow->Integer();
		long c = kcol->Integer();
		long val = value->Integer();
		values[r][c] = val;
	}
	void populate(Tamguimatrix* ke) {
		ke->cleaning();
		ke->init(rowsize, columnsize);
		for (long r = 0; r < rowsize; r++) {
			if (values.check(r)) {
				for (long c = 0; c < columnsize; c++) {
					if (values[r].check(c))
						ke->values[r][c] = values[r][c];
				}
			}
		}
	}

	long val(long r, long c) {
		if (values.check(r) && values[r].check(c))
			return values[r][c];
		return 0;
	}

	long LUDCMP(long n, VECTE<long>& INDX, long& d, Tamguimatrix& m) {

		d = 1;
		double AMAX, DUM, SUM;
		long  I, IMAX = 0, J, K;
		vector<double> VV;

		for (I = 0; I < n; I++)  {
			AMAX = 0.0;
			for (J = 0; J<n; J++)  {
				SUM = m.val(I, J);
				if (ABS(SUM) > AMAX)
					AMAX = ABS(SUM);
			}

			if (AMAX < TINY)
				return 1;
			VV.push_back(1.0 / AMAX);
		} // i loop 

		for (J = 0; J < n; J++)  {

			for (I = 0; I < J; I++)  {
				SUM = m.val(I, J);
				for (K = 0; K < I; K++)
					SUM = SUM - m.val(I, K)*m.val(K, J);
				m.values[I][J] = SUM;
			} // i loop 
			AMAX = 0.0;

			for (I = J; I < n; I++)  {
				SUM = m.val(I, J);
				for (K = 0; K < J; K++)
					SUM = SUM - m.val(I, K)*m.val(K, J);
				m.values[I][J] = SUM;
				DUM = VV[I] * ABS(SUM);
				if (DUM >= AMAX) {
					IMAX = I;
					AMAX = DUM;
				}
			} // i loop

			if (J != IMAX)  {
				for (K = 0; K < n; K++)  {
					DUM = m.val(IMAX, K);
					m.values[IMAX][K] = m.val(J, K);
					m.values[J][K] = DUM;
				} // k loop 
				d = -d;
				VV[IMAX] = VV[J];
			}

			INDX.at(J, IMAX);

			if (ABS(m.val(J, J)) < TINY)
				m.values[J][J] = TINY;

			if (J != n - 1)  {
				DUM = 1.0 / m.val(J, J);
				for (I = J + 1; I < n; I++)
					m.values[I][J] *= DUM;
			}
		} // j loop 

		return 0;

	} // subroutine LUDCMP 

	void LUBKSB(long n, VECTE<long>& INDX, VECTE<double>& B, Tamguimatrix& m)  {
		double SUM;
		long  I, II, J, LL;

		II = -1;

		for (I = 0; I < n; I++)  {
			LL = INDX[I];
			SUM = B[LL];
			B.at(LL, B[I]);
			if (II != -1)
			for (J = II; J < I; J++)
				SUM = SUM - m.val(I, J)*B[J];
			else
			if (SUM != 0.0)
				II = I;
			B.at(I, SUM);
		} // i loop

		for (I = n - 1; I >= 0; I--)  {
			SUM = B[I];
			if (I < n - 1)  {
				for (J = I + 1; J < n; J++)
					SUM = SUM - m.val(I, J)*B[J];
			}
			B.at(I, SUM / m.val(I, I));
		} // i loop 

	} // LUBKSB    

	long determinant(long rsize, long csize, TAMGUIMATRIX<TAMGUILINEINTEGER>& m) {
		if (rsize == 2 && csize == 2) {
			//then in that case
			if (m.check(0) && m.check(1))
				return (m[0][0] * m[1][1] - m[1][0] * m[0][1]);

			return 0;
		}


		long i;
		for (i = 0; i < rsize; i++) {
			if (m.check(i) == false)
				return 0;
		}

		i = 0;
		long det = 0;
		for (long j = 0; j < rsize; j++) {
			if (!m[i].check(j) || m[i][j] == 0)
				continue;

			TAMGUIMATRIX<TAMGUILINEINTEGER> sub(rsize - 1);

			long pc = 0;
			long pr = 0;
			for (long r = 0; r < rsize; r++) {
				if (r == i)
					continue;
				pc = 0;
				for (long c = 0; c < csize; c++) {
					if (c == j)
						continue;
					sub[pr][pc] = m[r][c];
					pc++;
				}
				pr++;
			}
			long sg = (long)pow(-1, (i + j + 2));
			det += m[i][j] * sg*determinant(rsize - 1, csize - 1, sub);
		}
		return det;
	}

	bool inversion(Tamguimatrix& Y) {
		//else Local decomposition

		Y.init(rowsize, columnsize);

		Tamguimatrix m(NULL, NULL);
		populate(&m);

		VECTE<long> INDX(rowsize);
		long id;
		//call LU decomposition, on the matrixbyrow values
		long rc = LUDCMP(rowsize, INDX, id, m);
		if (rc == 1)
			return false;


		VECTE<double> temp(rowsize);
		long i;
		//We create an identity matrix, which will contain the final result... 
		for (i = 0; i < rowsize; i++)
			Y.values[i][i] = 1;

		for (long j = 0; j < rowsize; j++) {
			for (i = 0; i < rowsize; i++)
				temp.at(i, Y.val(i, j));
			LUBKSB(rowsize, INDX, temp, m);
			for (i = 0; i < rowsize; i++) {
				Y.values[i][j] = temp[i];
			}
		}

		return true;
	}

	Tamguimatrix* duplicate() {
		Tamguimatrix* ke = new Tamguimatrix;

		for (long r = 0; r < rowsize; r++) {
			if (values.check(r)) {
				for (long c = 0; c < columnsize; c++) {
					if (values[r].check(c))
						ke->values[r][c] = values[r][c];
				}
			}
		}

		ke->rowsize = rowsize;
		ke->columnsize = columnsize;
		return ke;
	}

	string String() {
		locking();
		stringstream buff;

		for (long r = 0; r < rowsize; r++) {
			for (long c = 0; c < columnsize; c++) {
				if (c)
					buff << ",";
				if (values.check(r) && values[r].check(c))
					buff << values[r][c];
				else
					buff << "0";
			}
			buff << endl;
		}
        unlocking();
		return buff.str();
	}

    void Setstring(string& v, short idthread) {
        locking();
        stringstream buff;

        for (long r = 0; r < rowsize; r++) {
            for (long c = 0; c < columnsize; c++) {
                if (c)
                    buff << ",";
                if (values.check(r) && values[r].check(c))
                    buff << values[r][c];
                else
                    buff << "0";
            }
            buff << endl;
        }
        unlocking();
        v = buff.str();
    }

    void Setstring(wstring& v, short idthread) {
        locking();
        std::wstringstream buff;

        for (long r = 0; r < rowsize; r++) {
            for (long c = 0; c < columnsize; c++) {
                if (c)
                    buff << L",";
                if (values.check(r) && values[r].check(c))
                    buff << values[r][c];
                else
                    buff << L"0";
            }
            buff << endl;
        }
        unlocking();
        v = buff.str();
    }

	long Size() {
		return (rowsize*columnsize);
	}


	long Integer() { return Size(); }
	double Float() { return Size(); }
	bool Boolean() { return true; }

	Tamgu* same(Tamgu* a) {
		if (a->Type() != idtype)
			return aFALSE;

		Locking _lock(this);
		Tamguimatrix* ma = this;
		if (ma->rowsize != rowsize && ma->columnsize != columnsize)
			return aFALSE;

		for (long r = 0; r < rowsize; r++) {
			if (ma->values.check(r)) {
				if (!values.check(r))
					return aFALSE;
				for (long c = 0; c < columnsize; c++) {
					if (ma->values[r].check(c)) {
						if (!values[r].check(c))
							return aFALSE;

						if (ma->values[r][c] != values[r][c])
							return aFALSE;
					}
					else {
						if (values[r].check(c))
							return aFALSE;
					}
				}
			}
			else
			if (values.check(r))
				return aFALSE;
		}
		return aTRUE;
	}

	Tamgu* orset(Tamgu* b, bool autoself) {
		if (b->Type() != idtype) 
			return this;
		
		locking();
		Tamguimatrix* ma = this;
		Tamguimatrix* mb = (Tamguimatrix*)b;
		Tamguimatrix* res = new Tamguimatrix;

		long v;
		for (long r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r) && mb->values.check(r)) {
				for (long c = 0; c < ma->columnsize; c++) {
					if (ma->values[r].check(c) && mb->values[r].check(c)) {
						v = (long)ma->values[r][c] | (long)mb->values[r][c];
						res->populate(r, c, (long)v);
					}
				}
			}
		}
        unlocking();
		return res;
	}

	//intersection of two sets
	Tamgu* andset(Tamgu* b, bool autoself) {
		if (b->Type() != idtype)
			return this;

		locking();
		Tamguimatrix* ma = this;
		Tamguimatrix* mb = (Tamguimatrix*)b;
		Tamguimatrix* res = new Tamguimatrix;

		long v;
		for (long r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r) && mb->values.check(r)) {
				for (long c = 0; c < ma->columnsize; c++) {
					if (ma->values[r].check(c) && mb->values[r].check(c)) {
						v = (long)ma->values[r][c] & (long)mb->values[r][c];
						res->populate(r, c, (long)v);
					}
				}
			}
		}
        unlocking();
		return res;
	}

	Tamgu* xorset(Tamgu* b, bool autoself) {
		if (b->Type() != idtype)
			return this;

		locking();
		Tamguimatrix* ma = this;
		Tamguimatrix* mb = (Tamguimatrix*)b;
		Tamguimatrix* res = new Tamguimatrix;

		long v;
		for (long r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r) && mb->values.check(r)) {
				for (long c = 0; c < ma->columnsize; c++) {
					if (ma->values[r].check(c) && mb->values[r].check(c)) {
						v = (long)ma->values[r][c] ^ (long)mb->values[r][c];
						res->populate(r, c, (long)v);
					}
				}
			}
		}
        unlocking();
		return res;
	}

	//Beware that a might be the current element
	//plus: a+b
	Tamgu* plus(Tamgu* b, bool autoself) {

		if (b->Type() != idtype)
			return this;

		locking();
		Tamguimatrix* ma = this;
		Tamguimatrix* mb = (Tamguimatrix*)b;
		Tamguimatrix* res = new Tamguimatrix;
		long v;
		if (b->Type() != idtype) {
			long vb = b->Integer();
			for (long r = 0; r < ma->rowsize; r++) {
				if (ma->values.check(r)) {
					for (long c = 0; c < ma->columnsize; c++) {
						if (ma->values[r].check(c)) {
							v = ma->values[r][c] + vb;
							res->populate(r, c, v);
						}
					}
				}
			}
            unlocking();
			return res;
		}

		for (long r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r) && mb->values.check(r)) {
				for (long c = 0; c < ma->columnsize; c++) {
					if (ma->values[r].check(c) && mb->values[r].check(c)) {
						v = ma->values[r][c] + mb->values[r][c];
						res->populate(r, c, v);
					}
				}
			}
		}
        unlocking();
		return res;
	}

	//minus: a-b
	Tamgu* minus(Tamgu* b, bool autoself) {
		if (b->Type() != idtype)
			return this;

		locking();
		Tamguimatrix* ma = this;
		Tamguimatrix* mb = (Tamguimatrix*)b;
		Tamguimatrix* res = new Tamguimatrix;

		long v;
		if (b->Type() != idtype) {
			long vb = b->Integer();
			for (long r = 0; r < ma->rowsize; r++) {
				if (ma->values.check(r)) {
					for (long c = 0; c < ma->columnsize; c++) {
						if (ma->values[r].check(c)) {
							v = ma->values[r][c] - vb;
							res->populate(r, c, v);
						}
					}
				}
			}
            unlocking();
			return res;
		}


		for (long r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r) && mb->values.check(r)) {
				for (long c = 0; c < ma->columnsize; c++) {
					if (ma->values[r].check(c) && mb->values[r].check(c)) {
						v = ma->values[r][c] - mb->values[r][c];
						res->populate(r, c, v);
					}
				}
			}
		}
        unlocking();
		return res;
	}

	Tamgu* multiply(Tamgu* b, bool autoself) {
		if (b->Type() != idtype)
			return this;

		locking();
		Tamguimatrix* ma = this;
		Tamguimatrix* res = new Tamguimatrix;

		long v;
		long r, c, k;

		if (b->Type() != idtype) {
			long vb = b->Integer();
			for (r = 0; r < ma->rowsize; r++) {
				if (ma->values.check(r)) {
					for (c = 0; c < ma->columnsize; c++) {
						if (ma->values[r].check(c)) {
							v = ma->values[r][c] * vb;
							res->populate(r, c, v);
						}
					}
				}
			}
            unlocking();
			return res;
		}

		Tamguimatrix* mb = (Tamguimatrix*)b;
        if (ma->columnsize != mb->rowsize) {
            unlocking();
			return globalTamgu->Returnerror("MAT(209): Matrix size mismatch");
        }

		res->init(ma->rowsize, mb->columnsize);
		for (r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r)) {
				for (c = 0; c < mb->columnsize; c++) {
					v = 0;
					for (k = 0; k < ma->columnsize; k++) {
						if (ma->values[r].check(k) && mb->values.check(k) && mb->values[k].check(c))
							v += ma->values[r][k] * mb->values[k][c];
					}
					res->populate(r, c, v);
				}
			}
		}
        unlocking();
		return res;
	}


	//Division: a/b
	Tamgu* divide(Tamgu* b, bool autoself) {
		if (b->Type() != idtype)
			return this;

		locking();
		Tamguimatrix* ma = this;
		Tamguimatrix* res = new Tamguimatrix;

		long v;
		long r;
		long c;

		if (b->Type() != idtype) {
			long vb = b->Integer();
            if (vb == 0) {
                unlocking();
                return globalTamgu->Returnerror("MAT(203): Cannot divide by 0", globalTamgu->GetThreadid());
            }

			for (r = 0; r < ma->rowsize; r++) {
				if (ma->values.check(r)) {
					for (c = 0; c < ma->columnsize; c++) {
						if (ma->values[r].check(c)) {
							v = ma->values[r][c] / vb;
							res->populate(r, c, v);
						}
					}
				}
			}
            unlocking();
			return res;
		}

		Tamguimatrix* mb = (Tamguimatrix*)b;

		Tamguimatrix inverted(NULL, NULL);

        if (!mb->inversion(inverted)) {
            unlocking();
            return globalTamgu->Returnerror("MAT(202): Cannot divide with this matrix", globalTamgu->GetThreadid());
        }
        
        if (ma->columnsize != inverted.rowsize) {
            unlocking();
            return globalTamgu->Returnerror("MAT(202): Cannot divide with this matrix", globalTamgu->GetThreadid());
        }

		long k;
		for (r = 0; r < ma->rowsize; r++) {
			if (ma->values.check(r)) {
				for (c = 0; c < inverted.columnsize; c++) {
					v = 0;
					for (k = 0; k < ma->columnsize; k++) {
						if (ma->values[r].check(k) && inverted.values.check(k) && inverted.values[k].check(c))
							v += ma->values[r][k] * inverted.values[k][c];
					}
					res->populate(r, c, v);
				}
			}
		}

		res->rowsize = ma->rowsize;
		res->columnsize = inverted.columnsize;
        unlocking();
		return res;

	}

};

class TamguIterationimatrix : public TamguIteration {
public:
	Tamguimatrix* value;
	long r;
	long c;
	long mr, mc;

	TamguIterationimatrix(Tamguimatrix* v, bool d, TamguGlobal* g = NULL) : TamguIteration(d, g) {
		value = v;
	}

	Tamgu* Key() {
		char buff[100];
		sprintf_s(buff, 100, "%ld:%ld", r, c);
		return new Tamgustring(buff);
	}

	Tamgu* Value() {
		long v = 0;
		if (value->values.check(r) && value->values[r].check(c))
			v = value->values[r][c];

		return new Tamguint(v);
	}

	string Valuestring() {
		long v = 0;
		if (value->values.check(r) && value->values[r].check(c))
			v = value->values[r][c];
		
		return convertfromnumber(v);
		
	}

	long Valueinteger() {
		long v = 0;
		if (value->values.check(r) && value->values[r].check(c))
			v = value->values[r][c];
		return v;
	}

	double Valuefloat() {
		double v = 0;
		if (value->values.check(r) && value->values[r].check(c))
			v = (double)value->values[r][c];
		return v;
	}

	void Next() {
		r++;
		if (r == mr) {
			mc++;
			r = 0;
		}
	}

	Tamgu* End() {
		if (r == mr && c == mc)
			return aTRUE;
		return aFALSE;
	}

	Tamgu* Begin() {
		r = 0;
		c = 0;
		mr = value->rowsize;
		mc = value->columnsize;

		if (!mr && !mc)
			return aFALSE;

		return aTRUE;
	}

};

#endif




