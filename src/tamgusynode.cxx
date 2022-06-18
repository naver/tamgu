/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgusynode.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamguversion.h"
#include "tamgusynode.h"
#include "tamguautomaton.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<synodeMethod>  Tamgusynode::methods;

Tamgumapss* Tamgusynode::validfeatures = NULL;
bool Tamgusynode::testvalid = false;

//------------------------------------------------------------------------------------------------------------------

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
Exporting void Tamgusynode::AddMethod(TamguGlobal* global, string name, synodeMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(a_synode) != global->infomethods.end() &&
            global->infomethods[a_synode].find(name) != global->infomethods[a_synode].end())
    return;

    global->infomethods[a_synode][name] = infos;
    global->RecordArity(a_synode, idname, arity);
}


void Tamgusynode::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamgusynode::InitialisationModule(global,"");
}


   bool Tamgusynode::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    
    validfeatures = NULL;
    Tamgusynode::testvalid = false;
    
    Tamgusynode::AddMethod(global, "_initial", &Tamgusynode::MethodInitial, P_ONE, "_initial(map m): Creates a syntactic node with some features.");
    Tamgusynode::AddMethod(global, "test", &Tamgusynode::MethodTest, P_ONE, "test(string attribute): Test if an attribute is part of the feature structure.");
    Tamgusynode::AddMethod(global, "addchild", &Tamgusynode::MethodAddChild, P_ATLEASTONE, "addchild(synode): Add a first child node.");
    Tamgusynode::AddMethod(global, "addprevious", &Tamgusynode::MethodAddPrevious, P_ONE, "addprevious(synode): Add a previous.");
    Tamgusynode::AddMethod(global, "addnext", &Tamgusynode::MethodAddNext, P_ONE, "addnext(synode): Add a next node.");
    Tamgusynode::AddMethod(global, "parent", &Tamgusynode::MethodParent, P_NONE | P_ONE, "parent(): Return the parent node or check it against the parameter.");
    Tamgusynode::AddMethod(global, "child", &Tamgusynode::MethodChild, P_NONE | P_ONE, "child(): Return the first child node or check it against the parameter.");
    Tamgusynode::AddMethod(global, "last", &Tamgusynode::MethodLast, P_NONE | P_ONE, "last(): Return the last child node or check it against the parameter.");
    Tamgusynode::AddMethod(global, "previous", &Tamgusynode::MethodPrevious, P_NONE | P_ONE, "previous(synode): Return the previous node or check it against the parameter.");
    Tamgusynode::AddMethod(global, "next", &Tamgusynode::MethodNext, P_NONE | P_ONE, "next(synode): Return the next node or check it against the parameter.");
    Tamgusynode::AddMethod(global, "after", &Tamgusynode::MethodAfter, P_NONE | P_ONE, "after(synode): Return true if the node is after.");
    Tamgusynode::AddMethod(global, "before", &Tamgusynode::MethodBefore, P_NONE | P_ONE, "before(synode): Return true if the node is before.");
    Tamgusynode::AddMethod(global, "succeed", &Tamgusynode::MethodSucceed, P_NONE | P_ONE, "succeed(synode): Return true if the node is after.");
    Tamgusynode::AddMethod(global, "precede", &Tamgusynode::MethodPrecede, P_NONE | P_ONE, "precede(synode): Return true if the node is before.");
    Tamgusynode::AddMethod(global, "sisters", &Tamgusynode::MethodSisters, P_NONE | P_ONE, "sisters(synode): Return the list of sister nodes or check if the node is a sister node.");
    Tamgusynode::AddMethod(global, "children", &Tamgusynode::MethodChildren, P_NONE | P_ONE, "children(): Return the list of children for a given node or test if the node is a child.");
    Tamgusynode::AddMethod(global, "sibling", &Tamgusynode::MethodSibling, P_NONE | P_ONE, "sibling(synode): test if the node is a sibling (either a sister or a descendant).");
    Tamgusynode::AddMethod(global, "attributes", &Tamgusynode::MethodAttributes, P_NONE, "attributes(): Return the feature attributes as a vector.");
    Tamgusynode::AddMethod(global, "values", &Tamgusynode::MethodValues, P_NONE, "values(): Return the feature values as a vector.");
    Tamgusynode::AddMethod(global, "nbchildren", &Tamgusynode::MethodNbChildren, P_NONE, "nbchildren(): Return the number of direct children.");
    
    global->returntypes[global->Getid("nbchildren")] = a_int;

    Tamgusynode::AddMethod(global, "definitions", &Tamgusynode::MethodDefinitions, P_ONE, "definitions(mapss): Set the valid feature definitions for all 'synodes'.");

    if (version != "") {
        global->newInstance[a_synode] = new Tamgusynode(-1,global);
        global->RecordCompatibilities(a_synode);
    }
    
    return true;
}

Exporting bool Tamgusynode::Checkattribute(string key) {
    if (testvalid == false)
        return true;

    if (validfeatures->values.find(key) == validfeatures->values.end())
        return false;
    return true;
}

Exporting bool Tamgusynode::Checkfeature(string key, string value) {
    if (testvalid == false)
        return true;

    if (validfeatures->values.find(key) == validfeatures->values.end())
        return false;

    if (validfeatures->values[key] != value) {
        //In the case of a feature with a variable value, we use a blank string
        if (validfeatures->values[key].empty())
            return true;
        return false;
    }

    return true;
}


Exporting Tamgu* Tamgusynode::Put(Tamgu* idx, Tamgu* kval, short idthread) {
    if (!idx->isIndex()) {
        if (kval->isMapContainer()) {
            if (kval == features)
                return aTRUE;

            if (features == aNULL) {
                features = globalTamgu->Providemapss();
                features->Setreference(reference);
            }
            
            if (!Checkfeatures(kval, idthread))
                return globalTamgu->Errorobject(idthread);

            features->Put(aNULL, kval, idthread);
            return aTRUE;
        }

        if (kval->Type() != a_synode)
            return globalTamgu->Returnerror("Expecting a 'synode' or a 'map'", idthread);

        Tamgusynode* as = (Tamgusynode*)kval;
        
        if (as->features == aNULL) {
            if (features != aNULL) {
                features->Resetreference(reference);
                features = aNULL;
            }
            return aTRUE;
        }

        if (features == aNULL) {
            features = globalTamgu->Providemapss();
            features->Setreference(reference);
        }

        features->Put(aNULL, as->features, idthread);
        return aTRUE;
    }

    if (features == aNULL) {
        features = globalTamgu->Providemapss();
        features->Setreference(reference);
    }

    if (kval->String() == "~") {
        ((Tamgumapss*)features)->values.erase(idx->String());
    }
    else
        features->Put(idx, kval, idthread);

    return aTRUE;
}

Exporting Tamgu* Tamgusynode::Merge(Tamgu* a) {
    if (a->Type() != a_mapss)
        return aFALSE;

    if (features == aNULL)
        features = globalTamgu->Providemapss();
        
    hmap<string, string >& values = ((Tamgumapss*)a)->values;
    hmap<string, string >& avalues = ((Tamgumapss*)features)->values;
    hmap<string, string> assignation;

    bool aff;
    bool neg;
    bool rgx;

    string key;
    string val;

    for (auto &it: values) {
        aff = neg = rgx = false;

        key = it.first;

        switch (key[0]) {
            case ':':
            rgx = true;
            if (key[1] == '~') {
                neg = true;
                key = &key.c_str()[2];
            }
            else
                key = &key.c_str()[1];
            break;
            case '=':
            aff = true;
            key = &key.c_str()[1];
            break;
            case '~':
            neg = true;
            key = &key.c_str()[1];
        }

        val = it.second;

        if (val == "~") {
            if (avalues.find(key) != avalues.end()) {
                if (!aff) {
                    if (!neg)
                        return aFALSE;
                }
                else
                    assignation[key] = val;
            }
            continue;
        }

        if (rgx) {
            if (avalues.find(key) == avalues.end()) {
                if (!neg)
                    return aFALSE;
                continue;
            }

            Au_automaton* au = globalTamgu->rules[val];
            if (au == NULL)
                globalTamgu->rules[val] = au = new Au_automaton(val);
            string reg = avalues[key];
            if (au->match(reg)==false) {
                if (!neg)
                    return aFALSE;
            }
            else {
                if (neg)
                    return aFALSE;
            }
            continue;
        }

        if (avalues.find(key) != avalues.end()) {
            if (val != avalues[key]) {
                if (!neg)
                    return aFALSE;
            }
            else {
                if (neg)
                    return aFALSE;
            }
        }
        else {
            if (aff)
                assignation[key] = val;
            else {
                if (!neg)
                    return aFALSE;
            }
        }
    }

    for (auto &it : assignation) {
        if (it.second == "~")
            avalues.erase(it.first);
        else
            avalues[it.first] = it.second;
    }

    return aTRUE;
}

Exporting Tamgu* Tamgusynode::same(Tamgu* a) {
    if (a == this)
        return aTRUE;

    if (a == aNULL && features == aNULL)
        return aTRUE;
        
    return aFALSE;
}

Exporting Tamgu* Tamgusynode::different(Tamgu* a) {
    if (a == this)
        return aFALSE;

    if (a == aNULL && features == aNULL)
        return aFALSE;

    return aTRUE;
}

Exporting bool Tamgusynode::Unify(TamguDeclaration* dom, Tamgu* a) {
    if (a == this)
        return true;

    if (a->Type() != a_synode) {
        if (a == aNULL && features == aNULL)
            return true;
        return false;
    }

    Tamgusynode* as = (Tamgusynode*)a;

    if (features == aNULL || as->features == aNULL)
        return true;

    if (as->Merge(features) == aTRUE)
        return true;
    return false;
}


Exporting bool Tamgusynode::isUnified(TamguDeclaration* dom) {
    if (!id || id == a_universal)
        return true;
    return false;
}

Exporting Tamgu* Tamgusynode::plus(Tamgu* a, bool itself) {
    if (!a->isMapContainer() && a->Type() != a_synode)
        return this;
        
    Tamgumapss* feat;
    if (itself) {
        if (features == aNULL) {
            feat = globalTamgu->Providemapss();
            features = feat;
        }
        else
            feat = (Tamgumapss*)features;
    }
    else {
        feat = globalTamgu->Providemapss();
        if (features != aNULL)
            feat->values = ((Tamgumapss*)features)->values;
    }

    if (a->isMapContainer()) {
        short idthread = globalTamgu->GetThreadid();
        if (!Checkfeatures(a, idthread))
            return globalTamgu->Errorobject(idthread);
    }

    TamguIteration* itr = a->Newiteration(false);
    string val;
    string key;
    for (itr->Begin(); itr->End() != aTRUE; itr->Next())  {
        key = itr->Keystring();
        if (key[0] == '=')
            key = key.substr(1, key.size() - 1);
        val = itr->Valuestring();
        if (val == "~") {
            if (feat->values.find(key) != feat->values.end())
                feat->values.erase(key);
        }
        else
            feat->values[key] = val;
    }
    itr->Release();
    return feat;
}

Exporting Tamgu* TamguCallsynode::Eval(Tamgu* actual, Tamgu* value, short idthread) {
    if (affectation)
        return this;

    actual = syn->Eval(actual, value, idthread);
    if (actual->Type() != a_synode)
        return aFALSE;

    //we might have some action to do over "as"
    return ((Tamgusynode*)actual)->Merge(features);
}

Exporting Tamgu* TamguCallsynode::Put(Tamgu* actual, Tamgu* value, short idthread) {
    actual = syn->Eval(actual, aNULL, idthread);
    if (actual->Type() != a_synode)
        return aFALSE;

    string vstr = value->String();
    Tamgusynode* asyn = (Tamgusynode*)actual;
    if (vstr == "~") {
        if (asyn->features == aNULL)
            return aFALSE;
        hmap<string,string>& asynfeat = ((Tamgumapss*)asyn->features)->values;
        for (const auto& it : ((Tamgumapss*)features)->values) {
            if (asynfeat.find(it.first) != asynfeat.end())
                asynfeat.erase(it.first);
        }
        return aTRUE;
    }
    Tamgumapss feats;
    for (const auto& it : ((Tamgumapss*)features)->values)
        feats.values[it.first] = vstr;
    return asyn->plus(&feats, true);
}
