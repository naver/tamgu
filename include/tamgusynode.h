/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgusynode.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgusynode_h
#define tamgusynode_h

#include "tamgumapss.h"
#include "tamgusvector.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgusynode;
//This typedef defines a type "synodeMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgusynode::*synodeMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------

class Tamgusynode : public TamguContainer {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging basebin_hash<synodeMethod> methods;
	static Exchanging hmap<string, string> infomethods;
	static Exchanging bin_hash<unsigned long> exported;

	static Exchanging Tamgumapss* validfeatures;
	static Exchanging bool testvalid;

	static Exchanging short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	Tamgu* features;
	Tamgusynode* parent;
	Tamgusynode* child;
	Tamgusynode* next;
	Tamgusynode* previous;
	Tamgusynode* last;
	char buff[5];

	short id;
	short name;

	//---------------------------------------------------------------------------------------------------------------------
	Tamgusynode(short i, TamguGlobal* g, Tamgu* parnt = NULL) : TamguContainer(g, parnt) {
		//Do not forget your variable initialisation
		if (i != a_universal) {
			sprintf_s(buff, 5, "#%d", i);
			name = g->Getid(buff);
		}
		else {
			strcpy_s(buff, 5, "_");
			name = a_universal;
		}

		features = aNULL;
		parent = NULL;
		child = NULL;
		next = NULL;
		previous = NULL;
		last = NULL;
		id = i;
	}

	Tamgusynode(short i = 0) {
		//Do not forget your variable initialisation
		sprintf_s(buff, 5, "#%d", i);
		name = globalTamgu->Getid(buff);
		id = i;

		features = aNULL;
		parent = NULL;
		child = NULL;
		next = NULL;
		previous = NULL;
		last = NULL;
	}

	Tamgusynode(Tamgusynode* syn, TamguGlobal* g) : TamguContainer(g) {
		strcpy_s(buff, 5, syn->buff);
		id = syn->id;
		name = syn->name;

		features = aNULL;
		parent = NULL;
		child = NULL;
		next = NULL;
		previous = NULL;
		last = NULL;
	}

	void AddInstruction(Tamgu* a) {
		features = a;
		features->Setreference(reference);
	}

	Exporting Tamgu* ExtractPredicateVariables(Tamgu* contextualpattern, TamguDeclaration* dom, Tamgu* c, Tamgu* e, short idthread, bool root);
	Exporting bool Unify(TamguDeclaration* dom, Tamgu* a);
	Exporting bool isUnified(TamguDeclaration* dom);
	bool Insertvalue(Tamgu* dom, Tamgu* v, basebin_hash<Tamgu*>&) {
		if (v == this)
			return true;
		return false;
	}

	//----------------------------------------------------------------------------------------------------------------------
	Exporting Tamgu* Put(Tamgu* v, Tamgu* i, short idthread);

	Exporting Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread);


	short Type() {
		return Tamgusynode::idtype;
	}

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
		return "synode";
	}

	short Name() {
		return name;
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

	Tamgu* Map(short idthread) {
		return features;
	}

	Exporting Tamgu* Merge(Tamgu* a);

	Tamgu* Merging(Tamgu* a) {
		Merge(a);
		return this;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
	bool isDeclared(short n) {
		if (exported.find(n) != exported.end())
			return true;
		return false;
	}

	bool isNew() {
		if (parent != NULL)
			return false;
		return true;
	}

	Tamgu* Newinstance(short, Tamgu* f = NULL) {
		return new Tamgusynode;
	}

	TamguIteration* Newiteration(bool direction) {
		if (features == aNULL)
			return aITERNULL;

		return features->Newiteration(direction);
	}

	Tamgu* in(Tamgu* context, Tamgu* a, short idthread) {
		if (features == aNULL) {
			if (context->isBoolean())
				return aFALSE;

			if (context->isVectorContainer())
				return Selectasvector(context);

			return aNULL;
		}

		return features->in(context, a, idthread);
	}

	Exporting static void AddMethod(TamguGlobal* g, string name, synodeMethod func, unsigned long arity, string infos);
	Exporting static bool InitialisationModule(TamguGlobal* g, string version);
	Exporting static bool Checkattribute(string key);
	Exporting static bool Checkfeature(string key, string value);

	Tamgu* Getvalues(TamguDeclaration* dom, bool duplicate);

	bool Checkfeatures(Tamgu* m, short idthread) {
		if (!testvalid)
			return true;

		TamguIteration* itr = m->Newiteration(false);
		for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
			if (!Checkfeature(itr->Keystring(), itr->Valuestring())) {
				stringstream msg;
				msg << "Unknown feature: '" << itr->Keystring() << ":" << itr->Valuestring() << "'";
				globalTamgu->Returnerror(msg.str(), idthread);
				itr->Release();
				return false;
			}
		}

		itr->Release();
		return true;
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
    
    void Setreference() {
        features->Setreference();
        TamguReference::Setreference();
        Tamgusynode* syn = child;
        while (syn != NULL) {
            syn->Setreference();
            syn = syn->next;
        }
    }
    
    void Setreference(short r) {
        features->Setreference(r);
        TamguReference::Setreference(r);
        Tamgusynode* syn = child;
        while (syn != NULL) {
            syn->Setreference(r);
            syn = syn->next;
        }
    }
    
	void Resetreference(short r = 1) {
		Tamgusynode* syn = child;
		while (syn != NULL) {
			Tamgusynode* nx = syn->next;
			syn->Resetreference(r);
			syn = nx;
		}
		features->Resetreference(r);
		TamguReference::Resetreference(r);
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is a sample  of a function that could be implemented for your needs.

	Tamgu* MethodInitial(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		Tamgu* res = callfunc->Evaluate(0, contextualpattern, idthread);
		if (!res->isMapContainer())
			return globalTamgu->Returnerror("Expecting a map as value", idthread);

		if (res->Type() == Tamgumapss::idtype)
			features = res;
		else {
			features = globalTamgu->Providemapss();
			features->Put(aNULL, res, idthread);
		}

		features->Setreference(reference);
		return this;
	}

	bool compare(Tamgusynode* as) {
		if (as == NULL || as->features == aNULL)
			return true;

		string key;
		string val;
		bool aff = false;


		hmap<string, string >& values = ((Tamgumapss*)features)->values;
		hmap<string, string >& avalues = ((Tamgumapss*)as->features)->values;
		for (auto& it : avalues) {
			aff = false;
			key = it.first;
			if (key[0] == '=') {
				aff = true;
				key = &key.c_str()[1];
			}
			val = it.second;
			if (val == "~") {
				if (values.find(key) != values.end()) {
					if (!aff)
						return false;
					values.erase(key);
				}
				continue;
			}
			if (values.find(key) != values.end()) {
				if (val != values[key])
					return false;
			}
			else {
				if (!aff)
					return false;
			}
		}

		return true;
	}

	Tamgusynode* Root() {
		if (parent == NULL)
			return this;
		return parent->Root();
	}

	void Leftdepth(Tamgusynode* syn, Tamguvector& vect, long& pos) {
		if (syn == this)
			pos = vect.values.size();

		vect.values.push_back(this);

		if (child != NULL)
			child->Leftdepth(syn, vect, pos);
		if (next != NULL)
			next->Leftdepth(syn, vect, pos);
	}

	void Lefttraverse(Tamguvector& vect, long& pos) {
		Tamgusynode* root = Root();
		root->Leftdepth(this, vect, pos);
	}

	void Traverse(Tamgusynode* syn, Tamgusynode* cmp, Tamgu* vect) {
		if (syn->compare(cmp) && syn != this)
			vect->Push(syn);
		if (syn->child != NULL)
			Traverse(syn->child, cmp, vect);
		if (syn->next != NULL)
			Traverse(syn->next, cmp, vect);
	}

	Tamgusynode* traverse(Tamgusynode* syn) {
		if (syn == this)
			return this;

		Tamgusynode* a;
		if (child != NULL) {
			a = child->traverse(syn);
			if (a != NULL)
				return a;
		}
		if (next != NULL) {
			a = next->traverse(syn);
			if (a != NULL)
				return a;
		}
		return NULL;
	}



	//----------------------------------------------------------------
	Tamgu* MethodTest(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		if (features->Type() != Tamgumapss::idtype)
			return aFALSE;

		string s = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	
		if (((Tamgumapss*)features)->values.find(s) == ((Tamgumapss*)features)->values.end())
			return aFALSE;
		return aTRUE;
	}

	//----------------------------------------------------------------
	Tamgu* MethodParent(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		if (callfunc->Size() == 0) {
			if (parent == NULL)
				return aNULL;
			return parent;
		}

		Tamgu* a = callfunc->Evaluate(0, contextualpattern, idthread);
		if (a->Type() != a_synode || parent == NULL)
			return aNULL;

		Tamgusynode* syn = (Tamgusynode*)a;
		if (parent == syn)
			return parent;

		if (parent->compare(syn))
			return parent;

		return aNULL;
	}

	Tamgu* MethodLast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		if (callfunc->Size() == 0) {
			if (last == NULL)
				return aNULL;
			return last;
		}

		Tamgu* a = callfunc->Evaluate(0, contextualpattern, idthread);
		if (a->Type() != a_synode || last == NULL)
			return aNULL;

		Tamgusynode* syn = (Tamgusynode*)a;
		if (last == syn)
			return last;

		if (last->compare(syn))
			return last;

		return aNULL;
	}


	Tamgu* MethodChild(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		if (callfunc->Size() == 0) {
			if (child == NULL)
				return aNULL;
			return child;
		}

		Tamgu* a = callfunc->Evaluate(0, contextualpattern, idthread);
		if (a->Type() != a_synode || child == NULL)
			return aNULL;

		Tamgusynode* syn = (Tamgusynode*)a;
		if (child == syn)
			return child;

		if (child->compare(syn))
			return child;

		return aNULL;
	}

	Tamgu* MethodPrevious(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		if (callfunc->Size() == 0) {
			if (previous == NULL)
				return aNULL;
			return previous;
		}

		Tamgu* a = callfunc->Evaluate(0, contextualpattern, idthread);
		if (a->Type() != a_synode || previous == NULL)
			return aNULL;

		Tamgusynode* syn = (Tamgusynode*)a;
		if (previous == syn)
			return previous;

		if (previous->compare(syn))
			return previous;

		return aNULL;
	}

	Tamgu* MethodNext(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {		
		if (callfunc->Size() == 0) {
			if (next == NULL)
				return aNULL;
			return next;
		}

		Tamgu* a = callfunc->Evaluate(0, contextualpattern, idthread);
		if (a->Type() != a_synode || next == NULL)
			return aNULL;

		Tamgusynode* syn = (Tamgusynode*)a;
		if (next == syn)
			return next;

		if (next->compare(syn))
			return next;

		return aNULL;

	}

	Tamgu* MethodAfter(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		Tamgusynode* s;
		if (callfunc->Size() == 0) {
			Tamgu* vect = Selectavector(contextualpattern);
			s = next;
			while (s != NULL) {
				vect->Push(s);
				s = s->next;
			}
			return vect;
		}

		Tamgu* a = callfunc->Evaluate(0, contextualpattern, idthread);
		if (a->Type() != a_synode || parent == NULL)
			return aNULL;

		Tamgusynode* syn = (Tamgusynode*)a;
		if (syn == this)
			return syn;

		if (!syn->id) {
			s = next;
			while (s != NULL) {
				if (s == syn)
					return s;
				s = s->next;
			}

			return aNULL;
		}

		Tamgu* vect = Selectavector(contextualpattern);
		s = next;
		while (s != NULL) {
			if (s->compare(syn))
				vect->Push(s);
			s = s->next;
		}

		return vect;
	}

	Tamgu* MethodBefore(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		Tamgusynode* s;
		if (callfunc->Size() == 0) {
			Tamgu* vect = Selectavector(contextualpattern);
			s = previous;
			while (s != NULL) {
				vect->Push(s);
				s = s->previous;
			}
			return vect;
		}

		Tamgu* a = callfunc->Evaluate(0, contextualpattern, idthread);
		if (a->Type() != a_synode || parent == NULL)
			return aNULL;

		Tamgusynode* syn = (Tamgusynode*)a;
		if (syn == this)
			return syn;

		if (!syn->id) {
			s = previous;
			while (s != NULL) {
				if (s == syn)
					return s;
				s = s->previous;
			}

			return aNULL;
		}

		Tamgu* vect = Selectavector(contextualpattern);
		s = previous;
		while (s != NULL) {
			if (s->compare(syn))
				vect->Push(s);
			s = s->previous;
		}

		return vect;
	}


	Tamgu* MethodPrecede(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {


		long pos = -1;
		long i;
		if (callfunc->Size() == 0) {
			Tamguvector nodes;
			Lefttraverse(nodes, pos);
			Tamgu* vect = Selectavector(contextualpattern);
			for (i = pos + 1; i < nodes.values.size(); i++)
				vect->Push(nodes.values[i]);

			return vect;
		}

		Tamgu* a = callfunc->Evaluate(0, contextualpattern, idthread);
		if (a->Type() != a_synode || parent == NULL)
			return aNULL;

		Tamgusynode* syn = (Tamgusynode*)a;
		if (syn == this)
			return syn;

		Tamguvector nodes;
		Lefttraverse(nodes, pos);

		if (!syn->id) {
			for (i = pos + 1; i < nodes.values.size(); i++) {
				if (nodes.values[i] == syn)
					return syn;
			}
			return aNULL;
		}

		Tamgusynode* s;
		Tamgu* vect = Selectavector(contextualpattern);
		for (i = pos + 1; i < nodes.values.size(); i++) {
			s = (Tamgusynode*)nodes.values[i];
			if (s->compare(syn))
				vect->Push(s);
		}

		return vect;
	}

	Tamgu* MethodSucceed(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

		long pos = -1;
		long i;
		if (callfunc->Size() == 0) {
			Tamguvector nodes;
			Lefttraverse(nodes, pos);
			Tamgu* vect = Selectavector(contextualpattern);
			for (i = 0; i < pos; i++)
				vect->Push(nodes.values[i]);

			return vect;
		}

		Tamgu* a = callfunc->Evaluate(0, contextualpattern, idthread);
		if (a->Type() != a_synode || parent == NULL)
			return aNULL;

		Tamgusynode* syn = (Tamgusynode*)a;
		if (syn == this)
			return syn;
		
		Tamguvector nodes;
		Lefttraverse(nodes, pos);

		if (!syn->id) {
			for (i = 0; i < pos; i++) {
				if (nodes.values[i] == syn)
					return syn;
			}
			return aNULL;
		}

		Tamgusynode* s;
		Tamgu* vect = Selectavector(contextualpattern);
		for (i = 0; i < pos; i++) {
			s = (Tamgusynode*)nodes.values[i];
			if (s->compare(syn))
				vect->Push(s);
		}

		return vect;
	}


	Tamgu* MethodSisters(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		Tamgusynode* s;
		if (callfunc->Size() == 0) {
			if (parent == NULL)
				return aNULL;

			Tamgu* vect = Selectavector(contextualpattern);
			s = parent->child;
			while (s != NULL) {
				if (s != this)
					vect->Push(s);
				s = s->next;
			}
			return vect;
		}

		Tamgu* a = callfunc->Evaluate(0, contextualpattern, idthread);
		if (a->Type() != a_synode || parent == NULL)
			return aNULL;

		Tamgusynode* syn = (Tamgusynode*)a;
		if (syn == this)
			return syn;

		if (!syn->id) {
			s = parent->child;
			while (s != NULL) {
				if (s == syn)
					return s;
				s = s->next;
			}

			return aNULL;
		}

		Tamgu* vect = Selectavector(contextualpattern);
		s = parent->child;
		while (s != NULL) {
			if (s == this) {
				s = s->next;
				continue;
			}
			if (s->compare(syn))
				vect->Push(s);
			s = s->next;
		}

		return vect;
	}

	Tamgu* MethodNbChildren(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		if (child == NULL)
			return aZERO;
		long nb = 0;
		Tamgusynode* s = child;

		while (s != NULL) {
			nb++;
			s = s->next;
		}

		return globalTamgu->Provideint(nb);
	}

	Tamgu* MethodChildren(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		Tamgu* vect = aNULL;
		if (callfunc->Size() == 0) {
			vect = Selectavector(contextualpattern);
			if (child == NULL)
				return aNULL;

			vect = Selectavector(contextualpattern);
			Tamgusynode* s = child;
			while (s != NULL) {
				vect->Push(s);
				s = s->next;
			}
			return vect;
		}

		Tamgu* a = callfunc->Evaluate(0, contextualpattern, idthread);
		
		if (a->Type() != a_synode || child == NULL)
			return aNULL;

		Tamgusynode* syn = (Tamgusynode*)a;
		
		if (syn->id != 0)
			vect = Selectavector(contextualpattern);

		Tamgusynode* s = child;
		while (s != NULL) {
			if (!syn->id) {
				if (s == syn)
					return s;
			}
			else {
				if (s->compare(syn))
					vect->Push(s);
			}
			s = s->next;
		}

		return vect;
	}

	Tamgu* MethodSibling(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		if (callfunc->Size() == 0) {			
			if (child == NULL)
				return aNULL;

			Tamgu* vect = Selectavector(contextualpattern);
			Traverse(child, NULL, vect);
			return vect;
		}

		Tamgu* a = callfunc->Evaluate(0, contextualpattern, idthread);
		if (a->Type() != a_synode)
			return aNULL;

		Tamgusynode* syn = (Tamgusynode*)a;

		if (!syn->id) {
			if (parent != NULL)
				syn = parent->child->traverse(syn);
			else
				syn = traverse(syn);

			if (syn != NULL)
				return syn;
			return aNULL;
		}

		Tamgu* vect = Selectavector(contextualpattern);
		Traverse(child, syn, vect);
		return vect;
	}

    void addchild(Tamgusynode* syn) {
        syn->parent = this;
        
        syn->Setreference(reference);
        
        if (child == NULL)
            child = syn;
        else {
            syn->previous = last;
            last->next = syn;
        }
        last = syn;
    }
    
	Tamgu* MethodAddChild(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		Tamgu* a;
		Tamgusynode* syn;
		for (int i = 0; i < callfunc->Size(); i++) {
			a = callfunc->Evaluate(i, contextualpattern, idthread);
			if (a->Type() != a_synode)
				return globalTamgu->Returnerror("Expecting a 'synode' here", idthread);

			syn = (Tamgusynode*)a;
			if (!syn->isNew())
				return globalTamgu->Returnerror("This 'synode' is already part of a tree", idthread);

			syn->parent = this;

			syn->Setreference(reference);

			if (child == NULL)
				child = syn;
			else {
				syn->previous = last;
				last->next = syn;
			}

			last = syn;
		}
		return this;
	}
	
	Tamgu* MethodAddNext(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		Tamgu* a = callfunc->Evaluate(0, contextualpattern, idthread);
        if (a->Type() != a_synode)
            return globalTamgu->Returnerror("Expecting a 'synode' here", idthread);

		Tamgusynode* syn = (Tamgusynode*)a;
		if (!syn->isNew())
			return globalTamgu->Returnerror("This 'synode' is already part of a tree", idthread);

		syn->previous = this;
		syn->next = next;
		if (next != NULL)
			next->previous = syn;
		next = syn;

		if (parent != NULL) {
			parent->last = syn;
			syn->parent = parent;
			syn->Setreference(parent->reference);
		}

		return syn;
	}

	Tamgu* MethodAddPrevious(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		Tamgu* a = callfunc->Evaluate(0, contextualpattern, idthread);
        if (a->Type() != a_synode)
            return globalTamgu->Returnerror("Expecting a 'synode' here", idthread);

		Tamgusynode* syn = (Tamgusynode*)a;
		if (!syn->isNew())
			return globalTamgu->Returnerror("This 'synode' is already part of a tree", idthread);

		syn->next = this;
		syn->previous = previous;
		if (previous != NULL)
			previous->next = syn;
		previous = syn;

		if (parent != NULL) {
			if (parent->child == this)
				parent->child = syn;
			syn->parent = parent;
			syn->Setreference(parent->reference);
		}

		return syn;
	}

	Tamgu* MethodDefinitions(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		Tamgu* fmap = callfunc->Evaluate(0, contextualpattern, idthread);
		if (!fmap->isMapContainer())
			return globalTamgu->Returnerror("Expecting a 'map' as argument", idthread);

		if (validfeatures == NULL)
			validfeatures = globalTamgu->Providemapss();

		TamguIteration* itr = fmap->Newiteration(false);
		for (itr->Begin(); itr->End() != aTRUE; itr->Next())
			validfeatures->values[itr->Keystring()] = itr->Valuestring();
		itr->Release();

		if (validfeatures->values.size() != 0)
			testvalid = true;

		return aTRUE;
	}

	Tamgu* MethodAttributes(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		Tamgusvector* vstr = (Tamgusvector*)Selectasvector(contextualpattern);
		if (features == aNULL)
			return vstr;

		Tamgumapss* f = (Tamgumapss*)features;
		Locking _lock(f);
		hmap<string, string>::iterator it;
		for (it = f->values.begin(); it != f->values.end(); it++)
			vstr->values.push_back(it->first);
		return vstr;
	}

	Tamgu* MethodValues(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		Tamgusvector* vstr = (Tamgusvector*)Selectasvector(contextualpattern);
		if (features == aNULL)
			return vstr;

		Tamgumapss* f = (Tamgumapss*)features;
		Locking _lock(f);

		for (auto& it : f->values)
			vstr->values.push_back(it.first);
		return vstr;
	}
	//---------------------------------------------------------------------------------------------------------------------
	//ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
	//is returned by the Declaration method.
	Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
	}
	void Clear(TamguGlobal* g) {
		//To set a variable back to empty
	}
	
	bool isAssignable() {
		return true;
	}
	string JSonString() {
		if (features == aNULL)
			return "{}";

		return features->JSonString();
	}

	string String() {
		string s;
		if (features->isMapContainer()) {
			s = features->String();
			s[0] = '[';
			s[s.size() - 1] = ']';
		}

		stringstream r;
		r << "#" << id << s;
		return r.str();
	}

	long Integer() { return id; }
	short Short() { return id; }
	double Float() { return id; }
	bool Boolean() { return false; }
	BLONG Long() { return id; }
	unsigned char Byte() {
		return id;
	}

	long Size() { return features->Size(); }

	Exporting Tamgu* same(Tamgu* a);
	Exporting Tamgu* different(Tamgu* a);
	Exporting Tamgu* plus(Tamgu* a, bool itself);
};


class TamguCallsynode : public TamguContainer {
public:
	Tamgusynode* syn;
	Tamgu* features;

	TamguCallsynode(Tamgusynode* s, TamguGlobal* g, Tamgu* parnt = NULL) : TamguContainer(g, parnt) {
		syn = s;
		features = aNULL;
	}

	void AddInstruction(Tamgu* f) {
		features = f;
		f->Setreference();
	}

	bool isAssignable() {
		return true;
	}

	Exporting Tamgu* Eval(Tamgu* v, Tamgu* i, short idthread);
	Exporting Tamgu* Put(Tamgu* v, Tamgu* i, short idthread);
};


#endif




