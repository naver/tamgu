/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : predicatefunctions.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef predicatefunctions_h
#define predicatefunctions_h


//----------------------------------------------------------------------------------------------------
class TamguPredicateBetween : public TamguPredicateFunction {
public:

	TamguPredicateBetween(TamguGlobal* g, short n) : TamguPredicateFunction(g, NULL, n) {
        investigate |= is_predicatemethod;
    }

	Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
		return new TamguPredicateBetween(NULL, name);
	}

	bool Checkarity() {
		if (parameters.size() != 3)
			return false;
		return true;
	}
    
    short checkTypePredicate() {
        return a_predicatemethod;
    }

	Exporting Tamgu* PredicateEvalue(TamguInstructionEvaluate* context, VECTE<Tamgu*>& stack, TamguPredicate* currenthead, long depth);
	Exporting TamguPredicate* Duplicate(Tamgu* context, TamguDeclaration* dom, short idthread);
	Exporting bool Checkparameters(TamguDeclaration*);
};

//----------------------------------------------------------------------------------------------------
class TamguPredicateTermSucc : public TamguPredicateTerm {
public:

	TamguPredicateTermSucc(TamguGlobal* g, short n, Tamgu* parent = NULL) : TamguPredicateTerm(g, n, parent) {}
	Exporting Tamgu* Getvalues(TamguDeclaration* dom, char duplicate);

	Tamgu* Newinstance(short idthread, Tamgu* parent = NULL) {
		return new TamguPredicateTermSucc(NULL, name);
	}

};

class TamguPredicateSucc : public TamguPredicateFunction {
public:

	TamguPredicateSucc(TamguGlobal* g, short n) : TamguPredicateFunction(g, NULL, n) {
        investigate |= is_predicatemethod;
    }

	Tamgu* Newinstance(short idthread, Tamgu* parent) {
		if (parent != NULL && parent->Type() == a_parameterpredicate)
			return new TamguPredicateTermSucc(NULL, name);

		return new TamguPredicateSucc(NULL, name);
	}

	bool Checkarity() {
		if (parameters.size() == 2)
			return true;
		return false;
	}

    short checkTypePredicate() {
        return a_predicatemethod;
    }

	Exporting bool Checkparameters(TamguDeclaration*);

	Exporting Tamgu* PredicateEvalue(TamguInstructionEvaluate* context, VECTE<Tamgu*>& stack, TamguPredicate* currenthead, long depth);
	Exporting TamguPredicate* Duplicate(Tamgu* context, TamguDeclaration* dom, short idthread);

	Tamgu* Eval(Tamgu* context, Tamgu* obj, short idthread) {
		if (context->Type() == a_predicateevaluate)
			return TamguPredicateFunction::Eval(context, obj, idthread);
		return Succ();
	}
};

//----------------------------------------------------------------------------------------------------
class TamguPredicateTermPred : public TamguPredicateTerm {
public:

	TamguPredicateTermPred(TamguGlobal* g, short n, Tamgu* parent = NULL) : TamguPredicateTerm(g, n, parent) {}
	Exporting Tamgu* Getvalues(TamguDeclaration* dom, char duplicate);

	Tamgu* Newinstance(short idthread, Tamgu* parent = NULL) {
		return new TamguPredicateTermPred(NULL, name);
	}

};

class TamguPredicatePred : public TamguPredicateFunction {
public:

	TamguPredicatePred(TamguGlobal* g, short n) : TamguPredicateFunction(g, NULL, n) {
        investigate |= is_predicatemethod;
    }

	Tamgu* Newinstance(short idthread, Tamgu* parent) {
		if (parent != NULL && parent->Type() == a_parameterpredicate)
			return new TamguPredicateTermPred(NULL, name);

		return new TamguPredicatePred(NULL, name);
	}


	bool Checkarity() {
		if (parameters.size() == 2)
			return true;
		return false;
	}

    short checkTypePredicate() {
        return a_predicatemethod;
    }

	Exporting bool Checkparameters(TamguDeclaration*);

	Exporting Tamgu* PredicateEvalue(TamguInstructionEvaluate* context, VECTE<Tamgu*>& stack, TamguPredicate* currenthead, long depth);
	Exporting TamguPredicate* Duplicate(Tamgu* context, TamguDeclaration* dom, short idthread);

	Tamgu* Eval(Tamgu* context, Tamgu* obj, short idthread) {
		if (context->Type() == a_predicateevaluate)
			return TamguPredicateFunction::Eval(context, obj, idthread);
		return Pred();
	}

};

//----------------------------------------------------------------------------------------------------
class TamguPredicateTermMethod : public TamguPredicateTerm {
public:
	unsigned long size;

	TamguPredicateTermMethod(TamguGlobal* g, short n, unsigned long sz, Tamgu* parent = NULL) : size(sz), TamguPredicateTerm(g, n, parent) {}
	Exporting Tamgu* Getvalues(TamguDeclaration* dom, char duplicate);

	Tamgu* Newinstance(short idthread, Tamgu* parent = NULL) {
		return new TamguPredicateTermMethod(globalTamgu, name, size);
	}

	bool Checkarity() {
		return Arity(size, (long)parameters.size() - 1);
	}

};

class TamguPredicateMethod : public TamguPredicateFunction {
public:
	unsigned long size;

	TamguPredicateMethod(TamguGlobal* g, short n, unsigned long sz) : TamguPredicateFunction(g, NULL, n) {
        investigate |= is_predicatemethod;
		size = sz; 
	}

    short checkTypePredicate() {
        return a_predicatemethod;
    }

	Tamgu* Newinstance(short idthread, Tamgu* parent = NULL) {
		if (parent != NULL && parent->Type() == a_parameterpredicate)
			return new TamguPredicateTermMethod(globalTamgu, name, size);

		return new TamguPredicateMethod(globalTamgu, name, size);
	}

	bool Checkarity() {
		return Arity(size, parameters.size() - 2);
	}

	Exporting Tamgu* PredicateEvalue(TamguInstructionEvaluate* context, VECTE<Tamgu*>& stack, TamguPredicate* currenthead, long depth);
	Exporting TamguPredicate* Duplicate(Tamgu* context, TamguDeclaration* dom, short idthread);
	Exporting bool Checkparameters(TamguDeclaration*);


};

#endif



