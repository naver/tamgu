/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgutransducer.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgutransducer.h"
#include "atoms.h"
#include "tamgumapss.h"
#include "instructions.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<transducerMethod>  Tamgutransducer::methods;
Exporting hmap<string, string> Tamgutransducer::infomethods;
Exporting bin_hash<unsigned long> Tamgutransducer::exported;

Exporting short Tamgutransducer::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgutransducer::AddMethod(TamguGlobal* global, string name, transducerMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgutransducer::Setidtype(TamguGlobal* global) {
        Tamgutransducer::idtype = global->Getid("transducer");
    }

   bool Tamgutransducer::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    Tamgutransducer::idtype = global->Getid("transducer");

    Tamgutransducer::AddMethod(global, "_initial", &Tamgutransducer::MethodInitial, P_ATLEASTONE, "_initial(file, file2, file3...): load an automaton file.");

    Tamgutransducer::AddMethod(global, "load", &Tamgutransducer::MethodLoad, P_ATLEASTONE, "load(filename, filename2 etc...): load transducers, with alphabet normalization");
    Tamgutransducer::AddMethod(global, "add", &Tamgutransducer::MethodAdd, P_ONE | P_TWO | P_THREE, "add(string|map, bool normalize, int latintable): add a string or a mapss to the automaton");
    Tamgutransducer::AddMethod(global, "build", &Tamgutransducer::MethodBuild, P_TWO | P_THREE | P_FOUR, "buld(input,output, norm, latintable): Build a transducer file out of a text file containing on the first line surface form, then on next line lemma+features.");
    Tamgutransducer::AddMethod(global, "store", &Tamgutransducer::MethodStore, P_ONE | P_TWO | P_THREE, "store(name, norm, latintable): store an automaton.");
    Tamgutransducer::AddMethod(global, "compilergx", &Tamgutransducer::MethodCompilergx, P_NONE | P_TWO | P_THREE, "compilergx(expression, vector, name): Compile a regular expression combined with a vector and store it with name. If no parameters compile the 'regular' expressions for numbers.");
    Tamgutransducer::AddMethod(global, "lookup", &Tamgutransducer::MethodLookup, P_THREE | P_ONE, "lookup(word, threshold, flags): lookup for a word with a threshold and flags");
    Tamgutransducer::AddMethod(global, "lookdown", &Tamgutransducer::MethodLookdown, P_ONE|P_TWO, "lookdown(word_pos_feat, short lemma): lookdow for the surface form matching a word+pos+features. lemma is optional: if set to 1 or 2 then the string to look for is only a lemma. If set to 2, it also returns the features with the surface form.");
    Tamgutransducer::AddMethod(global, "parse", &Tamgutransducer::MethodProcess, P_ONE|P_TWO|P_FOUR, "parse(sentence, bool option, threshold, flags): parse a sentence based on lexicon content.");
    Tamgutransducer::AddMethod(global, "factorize", &Tamgutransducer::MethodFactorize, P_NONE, "factorize(): factorize the arcs and states of the automaton.");


    global->newInstance[Tamgutransducer::idtype] = new Tamgutransducer(global);
    global->RecordMethods(Tamgutransducer::idtype,Tamgutransducer::exported);

    Tamgu* a = new TamguSystemVariable(global, aONE, global->Createid("a_first"), a_short);
    a = new TamguSystemVariable(global, aTWO, global->Createid("a_change"),   a_short);
    a = new TamguSystemVariable(global, aFOUR, global->Createid("a_delete"),   a_short);
    a = new TamguSystemVariable(global, aEIGHT, global->Createid("a_insert"),   a_short);
    a = new TamguSystemVariable(global, aSIXTEEN, global->Createid("a_switch"),  a_short);
    a = new TamguSystemVariable(global, aTHIRTYTWO, global->Createid("a_nocase"),  a_short);
    a = new TamguSystemVariable(global, aSIXTYFOUR, global->Createid("a_repetition"),  a_short);
    a = new TamguSystemVariable(global, new TamguConstShort(128, global), global->Createid("a_vowel"),  a_short);
    a = new TamguSystemVariable(global, new TamguConstShort(256, global), global->Createid("a_surface"),  a_short);
    a = new TamguSystemVariable(global, new TamguConstShort(512, global), global->Createid("a_longest"),  a_short);
    a = new TamguSystemVariable(global, aONE, global->Createid("a_offsets"),  a_short);
    a = new TamguSystemVariable(global, aTWO, global->Createid("a_features"), a_short);
    
    return true;
}

Tamgu* Tamgutransducer::Put(Tamgu* index, Tamgu* value, short idthread) {
    return this;
}

Tamgu* Tamgutransducer::Eval(Tamgu* context, Tamgu* index, short idthread) {
    return this;
}

Tamgu* Tamgutransducer::MethodInitial(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    name = callfunc->Evaluate(0, contextualpattern, idthread)->String();

    automaton = new TamguDoubleSideAutomaton;
    if (!automaton->load(name)) {
        name += " cannot be loaded";
        return globalTamgu->Returnerror(name, idthread);
    }

    TamguDoubleSideAutomaton* bis;
    for (int i=1;i<callfunc->Size(); i++) {
        bis = new TamguDoubleSideAutomaton;
        string nm = callfunc->Evaluate(i, contextualpattern, idthread)->String();
        if (!bis->load(nm)) {
            nm+=" cannot be loaded...";
            delete bis;
            return globalTamgu->Returnerror(nm,idthread);
        }
        automaton->merge(bis);
        delete bis;
    }
    
    automaton->fillencoding(false);
    automaton->sorting();
    automaton->start.shuffle();
    return aTRUE;
}

Tamgu* Tamgutransducer::MethodLoad(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    
    Tamgu* filename = callfunc->Evaluate(0, contextualpattern, idthread);
    if (automaton != NULL)
        delete automaton;
    automaton = new TamguDoubleSideAutomaton;
    string sname = filename->String();
    if (!automaton->load(sname)) {
        sname += " cannot be loaded";
        return globalTamgu->Returnerror(sname, idthread);
    }
    
    TamguDoubleSideAutomaton* bis;
    for (int i=1;i<callfunc->Size(); i++) {
        bis = new TamguDoubleSideAutomaton;
        string nm = callfunc->Evaluate(i, contextualpattern, idthread)->String();
        if (!bis->load(nm)) {
            delete bis;
            nm+=" cannot be loaded...";
            return globalTamgu->Returnerror(nm,idthread);
        }
        automaton->merge(bis);
        delete bis;
    }

    automaton->fillencoding(false);
    automaton->start.shuffle();
    return aTRUE;
}

Tamgu* Tamgutransducer::MethodStore(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    if (automaton == NULL)
        return aFALSE;

    string name = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    if (callfunc->Size() >= 2) {
        automaton->normalize = callfunc->Evaluate(1, contextualpattern, idthread)->Boolean();
        if (callfunc->Size() == 3)
            automaton->encoding_table = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
    }

    if (automaton->store(name))
        return aTRUE;

    return aFALSE;

}

Tamgu* Tamgutransducer::MethodAdd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (automaton == NULL)
        automaton = new TamguDoubleSideAutomaton;

    Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);

    if (callfunc->Size() >= 2) {
        automaton->normalize = callfunc->Evaluate(1, contextualpattern, idthread)->Boolean();
        if (callfunc->Size() == 3)
            automaton->encoding_table = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
    }

    if (ke->Type() == Tamgumapss::idtype)
        automaton->addmap(((Tamgumapss*)ke)->values);
    else {
        hmap<string, string> values;
        if (ke->isVectorContainer()) {
            if ((ke->Size() % 2) != 0)
                return globalTamgu->Returnerror("XTR(010): Expecting a vector with an even number of values.", idthread);
            string s;
            string l;
            for (int i = 0; i < ke->Size(); i += 2) {
                s = ke->getstring(i);
                l = ke->getstring(i + 1);
                values[s] = l;
            }

        }
        else {
            if (ke->isMapContainer()) {
                TamguIteration* itr = ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
                    values[itr->Keystring()] = itr->Valuestring();
                }
                itr->Release();
            }
        }
        automaton->addmap(values);
    }

    return aTRUE;
}

Tamgu* Tamgutransducer::MethodBuild(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string input = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    string output = callfunc->Evaluate(1, contextualpattern, idthread)->String();
    bool norm = false;
    long latinencoding = 1;

    if (callfunc->Size() >= 3) {
        norm = callfunc->Evaluate(2, contextualpattern, idthread)->Boolean();
        if (callfunc->Size() == 4)
            latinencoding = callfunc->Evaluate(3, contextualpattern, idthread)->Integer();
    }
    
    if (automaton != NULL)
        delete automaton;

    automaton = new TamguDoubleSideAutomaton();
    return booleantamgu[compileAutomaton(*automaton, input, output, latinencoding, norm)];
}

Tamgu* Tamgutransducer::MethodFactorize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (automaton == NULL)
        return aNULL;

    automaton->factorize(0);
    
    return aTRUE;
}

Tamgu* Tamgutransducer::MethodProcess(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (automaton == NULL)
        return aNULL;
    Tamgu* kvect = Selectavector(contextualpattern);
    string words = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    words+=" ";
    long option=0;
    if (callfunc->Size()>=2)
        option = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    
    long threshold = 0;
    short flags = 0;
    if (callfunc->Size() == 4) {
        threshold = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
        flags = callfunc->Evaluate(3, contextualpattern, idthread)->Integer();
    }

    charReadString currentreader(words);
    if ((option&1)==1)
        currentreader.addoffsets = true;
    if ((option&2)==2)
        currentreader.addfeatures = true;

    currentreader.begin();
    Tamgusvector* kvs = globalTamgu->Providesvector();
    while (!currentreader.end()) {
        if (automaton->process(currentreader, kvs->values, option, threshold, flags, idthread)) {
            if (kvs->Size()==0)
                continue;
            
            if (option == 1) { //no features, but offsets...
                kvs->storevalue(currentreader.cbegin);
                kvs->storevalue(currentreader.cend);
            }
            
            kvect->Push(kvs);
            kvs = globalTamgu->Providesvector();
        }
    }
    kvs->Release();
    return kvect;
}

Tamgu* Tamgutransducer::MethodLookup(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (automaton == NULL)
        return aNULL;

    wstring word = callfunc->Evaluate(0, contextualpattern, idthread)->UString();
    long threshold = 0;
    short flags = 0;
    if (callfunc->Size() == 3) {
        threshold = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
        flags = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
    }

    if (contextualpattern->isContainer()) {
        Tamgu* kvs = Selectasvector(contextualpattern);
        automaton->up(word, ((Tamgusvector*)kvs)->values, threshold, flags, idthread);
        return kvs;
    }

    vector<string> readings;
    automaton->up(word, readings, threshold, flags, idthread);

    if (readings.size())
        return globalTamgu->Providestring(readings[0]);
    return aNULL;
}

Tamgu* Tamgutransducer::MethodLookdown(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    if (automaton == NULL)
        return aNULL;

    char lemma = 0;
    wstring word = callfunc->Evaluate(0, contextualpattern, idthread)->UString();
    if (callfunc->Size() == 2)
        lemma = callfunc->Evaluate(1, contextualpattern, idthread)->Byte();

    if (contextualpattern->isContainer()) {
        Tamgu* kvs = Selectasvector(contextualpattern);
        automaton->down(word, ((Tamgusvector*)kvs)->values, idthread, lemma);
        return kvs;
    }

    vector<string> readings;
    automaton->down(word, readings,idthread, lemma);

    if (readings.size())
        return globalTamgu->Providestring(readings[0]);
    return aNULL;
}

Tamgu* Tamgutransducer::MethodCompilergx(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (automaton == NULL)
        automaton = new TamguDoubleSideAutomaton();

    if (callfunc->Size() == 0) {
        automaton->regulars();
        return aTRUE;
    }
    
    agnostring rgx(callfunc->Evaluate(0, contextualpattern, idthread)->String());
    Tamgu* kfeat = callfunc->Evaluate(1, contextualpattern, idthread);
    if (!kfeat->isVectorContainer())
        return globalTamgu->Returnerror("XTR(001): We expect the second argument to be a vector.", idthread);


    //we first transform each of our features into indexes...
    vector<uint32_t> indexes;
    string s;
    for (int i = 0; i < kfeat->Size(); i++) {
        s = kfeat->getstring(i);
        if (s[0] != '\t')
            s = "\t" + s;
        indexes.push_back(automaton->index(s));
    }

    if (!automaton->start.parse(*automaton, rgx, indexes))
        return globalTamgu->Returnerror("XTR(002): Wrong regular expression", idthread);

    if (callfunc->Size() == 3) {
        string name = callfunc->Evaluate(2, contextualpattern, idthread)->String();
        automaton->store(name);
    }

    return aTRUE;
}

bool Tamgutransducer::compilergx(wstring& rgx, wstring& feat) {
    if (automaton == NULL)
        automaton = new TamguDoubleSideAutomaton();
        
    //first we need to replace spaces with an escape character...
    rgx=s_replacestring(rgx,L" ", L"% ");
    rgx+=L"!1"; //We need a slot to add the final features...

    vector<uint32_t> indexes;
    string s;
    s_unicode_to_utf8(s,feat);
    if (s[0] != '\t')
        s = "\t" + s;
    indexes.push_back(automaton->index(s));
    return automaton->start.parse(*automaton, rgx, indexes);
}


bool Tamgutransducer::parse(string& words, Tamguvector* kvect, short idthread, long threshold, short flags,  bool option) {
    if (automaton == NULL)
        return false;

    words+=" ";

    charReadString currentreader(words);
    if (option)
        currentreader.addfeatures = true;

    currentreader.begin();
    Tamgusvector* kvs = globalTamgu->Providesvector();
    while (!currentreader.end()) {
        if (automaton->process(currentreader, kvs->values, 0, threshold, flags, idthread)) {
            if (kvs->Size()==0)
                continue;
            
            kvs->storevalue(currentreader.cbegin);
            kvs->storevalue(currentreader.cend);
            
            kvect->Push(kvs);
            kvs = globalTamgu->Providesvector();
        }
    }
    kvs->Release();
    return true;
}
