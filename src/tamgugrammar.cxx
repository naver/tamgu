/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgugrammar.cxx
 Date       : 2017/09/01
 Purpose    : BNF grammar rules
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#include "codeparse.h"
#include "tamgu.h"
#include "tamgugrammar.h"
#include "atoms.h"
#include "tamguvector.h"
#include "tamgumap.h"
#include "tamguuvector.h"
#include "compilecode.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<grammarMethod>  Tamgugrammar::methods;
Exporting hmap<string, string> Tamgugrammar::infomethods;
Exporting bin_hash<unsigned long> Tamgugrammar::exported;


Exporting short Tamgugrammar::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgugrammar::AddMethod(TamguGlobal* global, string name, grammarMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgugrammar::Setidtype(TamguGlobal* global) {
        Tamgugrammar::idtype = global->Getid("grammar");
    }

   bool Tamgugrammar::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();
    
    Tamgugrammar::idtype = global->Getid("grammar");
    
    Tamgugrammar::AddMethod(global, "_initial", &Tamgugrammar::MethodInitial, P_ONE | P_TWO, "_initial(string rules, int skipblanks): Compile rules.");
    Tamgugrammar::AddMethod(global, "load", &Tamgugrammar::MethodInitial, P_ONE | P_TWO, "load(svector rules, int skipblanks): Load a grammar out of a svector. Each line must be a rule. 'skipblanks' is optional and is 0 by default. '1' means that white spaces and tabs will be skipped. '2' means that all spaces will be skipped.");
    Tamgugrammar::AddMethod(global, "apply", &Tamgugrammar::MethodApply, P_ONE, "apply(string x) : Apply the grammar to a string.");
    
    
    
    global->newInstance[Tamgugrammar::idtype] = new Tamgugrammar(global);
    global->RecordMethods(Tamgugrammar::idtype,Tamgugrammar::exported);
    
    return true;
}



Tamgu* Tamgugrammar::Put(Tamgu* index, Tamgu* value, short idthread) {
    return this;
}

Tamgu* Tamgugrammar::Eval(Tamgu* context, Tamgu* index, short idthread) {
    return this;
}

//---------------------------------------------------------------------------------
static const unsigned char gbegin = 1;
static const unsigned char gend = 2;

static const unsigned char gmeta = 1;
static const unsigned char grule = 2;
static const unsigned char gloop = 4;
static const unsigned char gmulti = 8;
static const unsigned char gnegation = 16;
static const unsigned char gfunction = 32;
static const unsigned char gkeepout = 64;
static const unsigned char gloopmax = 128;

//---------------------------------------------------------------------------------

static bool gramtraversestate(GrammarBaseAutomaton* gram, x_node* x, gramstate* s, long i, vector<wstring>& e, hmap<long, gramstate*>& states);

//We add arcs or state according to the value
static gramarc* gramtraversearc(GrammarBaseAutomaton* gram, x_node* x, gramstate* s, vector<wstring>& e) {
    long i = 0;
    if (x->nodes[0]->token == "negation") {
        i = 1;
    }
    
    if (x->nodes[0]->token == "subgram") {
        subgrammar* a = new subgrammar(gram);
        a->from = s;
        if (i == 1)
            a->status |= gnegation;
        a->status |= gmulti;
        gram->arcs.push_back(a);
        hmap<long, gramstate*> states;
        for (i = 0; i < x->nodes[0]->nodes.size(); i++) {
            gramtraversestate(&a->gram, x->nodes[0]->nodes[i], &a->substate, 0, e, states);
            states.clear();
        }
        s->arcs.push_back(a);
        return a;
    }
    
    gramarc* a = new gramarc;
    sc_utf8_to_unicode(a->wlabel, USTR(x->nodes[i]->value), x->nodes[i]->value.size());
    a->from = s;
    if (i == 1)
        a->status |= gnegation;
    gram->arcs.push_back(a);
    
    
    if (x->nodes[i]->token == "astringdouble" || x->nodes[i]->token == "astringsimple") {
        //we remove the enclosing characters...
        a->wlabel = a->wlabel.substr(1, a->wlabel.size() - 2);
        //we reuse idrule in this case to keep the label size...
        a->idrule = a->wlabel.size();
        if (!a->idrule)
            return NULL;
        if (a->idrule > 1)
            a->status |= gmulti;
    }
    else {
        if (x->nodes[i]->token == "anumber")
            a->wlabel = atoi(STR(x->nodes[i]->value));
        else {
            if (x->nodes[i]->token == "ameta")
                a->status |= gmeta;
            else
                if (x->nodes[i]->token == "asimple") {
                    //we remove the first character, which is a $
                    //we reuse idrule in this case to keep the label size...
                    a->idrule = a->wlabel.size();
                    if (a->idrule > 1)
                        a->status |= gmulti;
                }
                else {
                    if (x->nodes[i]->token == "amulti") {
                        //we remove the first character, which is a $
                        a->wlabel = a->wlabel.substr(1, a->wlabel.size() - 1);
                        //we reuse idrule in this case to keep the label size...
                        a->idrule = a->wlabel.size();
                        if (a->idrule > 1)
                            a->status |= gmulti;
                    }
                    else {
                        if (x->nodes[i]->token == "word") {
                            a->status |= grule;
                            e.push_back(a->wlabel);
                            if (a->wlabel[a->wlabel.size() - 1] == '_')
                                a->status |= gkeepout;
                        }
                    }
                }
        }
    }
    s->arcs.push_back(a);
    return a;
}

static bool gramtraversestate(GrammarBaseAutomaton* gram, x_node* x, gramstate* s, long i, vector<wstring>& e, hmap<long, gramstate*>& states) {
    //First we jump to the first non disjoined element
    while (i < x->nodes.size() && x->nodes[i]->token == "dj")
        i += 2;
    
    //we might be at the end of the structure...
    if (i == x->nodes.size()) {
        s->status = gend;
        return true;
    }
    
    gramarc* a;
    gramstate* gs;
    bool created = false;
    if (states.find(i) == states.end()) {
        created = true;
        gs = new gramstate;
        gram->states.push_back(gs);
        states[i] = gs;
    }
    else
        gs = states[i];
    
    //we create our node...
    //two cases: optional or kleene
    if (x->nodes[i]->nodes[0]->token == "aoptional" || x->nodes[i]->nodes[0]->token == "aoptitem") {
        a = gramtraversearc(gram, x->nodes[i]->nodes[0]->nodes[0], s, e);
        if (a == NULL)
            return false;
        a->state = gs;
        if (created) {
            if (!gramtraversestate(gram, x, gs, i + 1, e, states))
                return false;
        }
        //then we need to skip it...
        //we copy the states after the new arc...
        for (long i = 0; i < a->state->arcs.size(); i++)
            s->arcs.push_back(a->state->arcs[i]);
        if (gs->status == gend)
            s->status = gend;
    }
    else {
        a = gramtraversearc(gram, x->nodes[i]->nodes[0], s, e);
        if (a == NULL)
            return false;
        a->state = gs;
        if (created) {
            if (!gramtraversestate(gram, x, gs, i + 1, e, states))
                return false;
        }
        
        if (x->nodes[i]->nodes.size() == 2) {
            char kleene = x->nodes[i]->nodes[1]->value[0];
            //If there is a kleene then we need to be able to loop
            if (kleene == '*') {
                bool pushback = false;
                if (s->arcs.back() == a) {
                    s->arcs.pop_back();
                    pushback = true;
                }
                //then we might need to skip it...
                for (long i = 0; i < a->state->arcs.size(); i++)
                    s->arcs.push_back(a->state->arcs[i]);
                if (pushback)
                    s->arcs.push_back(a);
                
                if (gs->status == gend)
                    s->status = gend;
            }
            
            //We have different cases...
            //First this is not the last state of our expression such as: ?+,">"
            //But it is not also a longest match (++)
            if (gs->status != gend && x->nodes[i]->nodes[1]->value.size() == 1)
                a->state->arcs.push_back(a);
            else
                a->state->arcs.insert(a->state->arcs.begin(), a);
            a->status |= gloop;
            if (x->nodes[i]->nodes[1]->value.size() == 2)
                a->status |= gloopmax;
        }
    }
    
    if (i < x->nodes.size() - 1 && x->nodes[i + 1]->token == "dj")
        return gramtraversestate(gram, x, s, i + 2, e, states);
    return true;
}

static bool gramtraverse(GrammarBaseAutomaton* gram, x_node* x, gramstate* s, vector<wstring>& e) {
    hmap<long, gramstate*> states;
    return gramtraversestate(gram, x, s, 1, e, states);
}

Tamgu* Tamgugrammar::Loader(string& vrule, short idthread) {
    x_reading xr;
    bnf_tamgu bnf;
    
    
    string s;
    vector<wstring> e;
    string lret;
    x_node* xn = new x_node;
    
    xr.tokenize(vrule);
    bnf.initialize(&xr);
    
    
    if (bnf.m_rules(lret, &xn) != 1 || bnf.currentpos != bnf.fx->stack.size()) {
        if (bnf.currentpos < bnf.fx->stack.size()) {
            string content("Error after:<");
            for (long i = 0; i <= bnf.currentpos; i++) {
                content += bnf.fx->stack[i];
                content += " ";
            }
            content += "...>";
            return globalTamgu->Returnerror(content, idthread);
        }
        return globalTamgu->Returnerror("Cannot evaluate this grammar", idthread);
    }
    
    if (automaton != NULL && clean)
        delete automaton;
    
    automaton = new GrammarAutomaton;
    automaton->skipblanks = skipblanks;
    automaton->function = function;
    
    clean = true;
    long id;
    wstring rulename;
    for (long i = 0; i < xn->nodes.size(); i++) {
        //We need to transform it in an automaton...
        //the first node is the rule name... We need to put it in our dictionary
        sc_utf8_to_unicode(rulename, USTR(xn->nodes[i]->nodes[0]->value), xn->nodes[i]->nodes[0]->value.size());
        gramstate* gs;
        if (automaton->dictionary.find(rulename) == automaton->dictionary.end()) {
            id = automaton->rules.size();
            automaton->dictionary[rulename] = id;
            automaton->rdictionary[id] = rulename;
            gs = new gramstate;
            automaton->states.push_back(gs);
            gs->status = gbegin; //the head of our rule...
            automaton->rules.push_back(gs);
        }
        else {
            id = automaton->dictionary[rulename];
            gs = automaton->rules[id];
        }
        
        if (!gramtraverse(automaton, xn->nodes[i], gs, e)) {
            delete xn;
            return globalTamgu->Returnerror("GRM(002): Wrong negation or empty string.", idthread);
        }
    }
    delete xn;
    
    string el;
    //we check that for each label found in the rule that is a rule, a rule actually exists...
    for (long i = 0; i < e.size(); i++) {
        bool found = true;
        if (automaton->dictionary.find(e[i]) == automaton->dictionary.end()) {
            found = false;
            //is it a function?
            sc_unicode_to_utf8(el, e[i]);
            if (globalTamgu->symbolIds.find(el) != globalTamgu->symbolIds.end()) {
                id = globalTamgu->Getid(el);
                Tamgu* kfunc = globalTamgu->Getdefinition(id, idthread);
                if (kfunc !=NULL && kfunc->isFunction()) {
                    found = true;
                    //we look for all arcs with this label...
                    for (long j = 0; j < automaton->arcs.size(); j++) {
                        gramarc* a = automaton->arcs[j];
                        if (a->wlabel == e[i]) {
                            a->status &= ~grule;
                            a->status |= gfunction;
                            a->idrule = id;
                            if (a->from->arcs.size() == 1)
                                a->from->status |= gfunction;
                        }
                    }
                }
            }
            
            if (!found) {
                string message = "GRM(001): Unknown rule label:";
                message += " ";
                s_unicode_to_utf8(message, e[i]);
                return globalTamgu->Returnerror(message, idthread);
            }
        }
    }
    
    for (long i = 0; i<automaton->arcs.size(); i++) {
        gramarc* a = automaton->arcs[i];
        if ((a->status&grule) == grule)
            a->idrule = automaton->dictionary[a->wlabel];
    }
    
    return aTRUE;
}

Tamgu* Tamgugrammar::MethodInitial(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string vrule = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    if (callfunc->Size() == 2)
        skipblanks = callfunc->Evaluate(1, contextualpattern, idthread)->Byte();
    
    return Loader(vrule, idthread);
}

//----------------------------------------------------------------------------------------------------------------
static long mx = 0;
static long pmx = 0;

class profond {
public:
    long i;
    profond() {
        i = pmx;
        pmx++;
        if (mx < pmx)
            mx = pmx;
    }
    
    ~profond() {
        pmx--;
    }
};


bool GrammarBaseAutomaton::callfunction(short idthread, wstring& a, Tamgu* b, long pos) {
    TamguCallFunction kfunc(function);
    
    TamguConstUString ba(a);
    TamguConstInt bpos(pos);
    
    kfunc.arguments.push_back(&ba);
    kfunc.arguments.push_back(b);
    kfunc.arguments.push_back(&bpos);
    Tamgu* res = kfunc.Eval(aNULL, aNULL, idthread);
    if (res->isError())
        return false;
    return res->Boolean();
}

bool GrammarBaseAutomaton::callfunction(Tamgu* func, short idthread, Tamgu* b, long pos) {
    TamguCallFunction kfunc(func);
    
    TamguConstInt bpos(pos);
    kfunc.arguments.push_back(b);
    kfunc.arguments.push_back(&bpos);
    Tamgu* res = kfunc.Eval(aNULL, aNULL, idthread);
    if (res->isError())
        return false;
    return res->Boolean();
}

gramarc::gramarc() {
    state = NULL;
    from = NULL;
    status = 0;
    idrule = -1;
}


long subgrammar::test(wstring& label, wstring& r, long i) {
    long nxt = i;
    if (substate.compare(globalTamgu->GetThreadid(), &gram, label, nxt, aNULL, true)) {
        r = label.substr(i, nxt - i);
        return (nxt - 1);
    }
    return -1;
}

char subgrammar::test(wstring& label) {
    long i = 0;
    if (substate.compare(globalTamgu->GetThreadid(), &gram, label, i, aNULL, true) && i == label.size())
        return true;
    return false;
}

long gramarc::test(wstring& label, wstring& r, long i) {
    //we check whether it is the same string as the label
    long j = 0;
    while (j<wlabel.size()) {
        if (i == label.size())
            return -1;
        if (label[i++] != wlabel[j++])
            return -1;
    }
    r = wlabel;
    return (i - 1);
}

char gramarc::test(wstring& car) {
    char good = true;
    char bad = false;
    
    if ((status&gnegation) == gnegation) {
        good = false;
        bad = true;
    }
    if ((status&gmeta) != gmeta) {
        if (wlabel == car)
            return good;
        return bad;
    }
    
    if (wlabel[0] == '?') {
        if (wlabel[1] == '_')
            return gkeepout;
        return true;
    }
    
    if (wlabel[2] == '_') {
        if (good)
            good = gkeepout;
        else
            bad = gkeepout;
    }
    
    switch (wlabel[1]) {
        case '%':
        case '?':
            if (car[0] == wlabel[1])
                return good;
            return bad;
        case 'C':
            if (s_is_upper(car))
                return good;
            return bad;
        case 'e':
            if (s_is_emoji(car))
                return true;
            return false;
        case 'H':
            if (s_is_hangul(car))
                return good;
            return bad;
        case 'S':
            if (car[0] <= 32 || car[0] == 160)
                return good;
            return bad;
        case 'a':
            if (s_is_alpha(car))
                return good;
            return bad;
        case 'c':
            if (s_is_lower(car))
                return good;
            return bad;
        case 'd':
            if (s_is_digit(car))
                return good;
            return bad;
        case 'n':
            if (car[0] == 160)
                return good;
            return bad;
        case 'p':
            if (s_is_punctuation(car))
                return good;
            return bad;
        case 'r':
            if (car[0] == 10 || car[0] == 13)
                return good;
            return bad;
        case 's':
            if (car[0] == 9 || car[0] == 32 || car[0] == 160)
                return good;
            return bad;
    }
    
    return bad;
}

char gramarc::test(TAMGUCHAR car) {
    char good = true;
    char bad = false;
    
    if ((status&gnegation) == gnegation) {
        good = false;
        bad = true;
    }
    if ((status&gmeta) != gmeta) {
        if (wlabel[0] == car)
            return good;
        return bad;
    }
    
    if (wlabel[0] == '?') {
        if (wlabel[1] == '_')
            return gkeepout;
        return true;
    }
    
    if (wlabel[2] == '_') {
        if (good)
            good = gkeepout;
        else
            bad = gkeepout;
    }
    
    switch (wlabel[1]) {
        case '%':
        case '?':
            if (car == wlabel[1])
                return good;
            return bad;
        case 'C':
            if (c_is_upper(car))
                return good;
            return bad;
        case 'a':
            if (b_alpha(car))
                return good;
            return bad;
        case 'c':
            if (c_is_lower(car))
                return good;
            return bad;
        case 'd':
            if (car >= '0' && car <= '9')
                return good;
            return bad;
        case 'e':
            if (c_is_emoji(car))
                return true;
            return false;
        case 'H':
            if (c_is_hangul(car))
                return good;
            return bad;
        case 'p':
            if (c_is_punctuation(car))
                return good;
            return bad;
        case 'S':
            if (car <= 32 || car == 160)
                return good;
            return bad;
        case 's':
            if (car == 9 || car == 32 || car == 160)
                return good;
            return bad;
        case 'r':
            if (car == 10 || car == 13)
                return good;
            return bad;
    }
    
    return bad;
}

char gramstate::compare(short idthread, GrammarBaseAutomaton* gram, vector<wstring>& labels, long& i, Tamgu* v, bool asstring) {
    if (globalTamgu->Error(idthread))
        return gend;
    
    if (gram->skipblanks) {
        if (gram->skipblanks == 1) {
            while (i < labels.size() && (labels[i] == L" " || labels[i] == L"\t")) i++;
        }
        else {
            while (i < labels.size() && (labels[i][0] <= 32)) i++;
        }
    }
    
    if ((status&gend) == gend) {
        if (!arcs.size() || i == labels.size())
            return true;
    }
    else {
        if (i == labels.size()) { //if the next element is a function call, then we still can have a look...
            if ((status&gfunction) != gfunction)
                return false;
        }
    }
    
    long c = i;
    bool callfunc = false;
    bool topop = false;
    char found;
    char ret;
    
    if (gram->function != NULL)
        callfunc = true;
    
    for (long u = 0; u < arcs.size(); u++) {
        gramarc* a = arcs[u];
        if ((a->status&gfunction) == gfunction) {
            Tamgu* kfunc = globalTamgu->Getdefinition(a->idrule, idthread);
            if (v != aNULL) {
                v->Setreference();
                if (!gram->callfunction(kfunc, idthread, v, i)) {
                    v->Setprotect(1);
                    v->Resetreference();
                    v->Setprotect(1);
                    continue;
                }
                v->Setprotect(1);
                v->Resetreference();
                v->Setprotect(1);
            }
            
            if ((ret = a->state->compare(idthread, gram, labels, i, v, asstring)))
                return ret;
            
            i = c;
            continue;
        }
        
        if ((a->status&grule) == grule) {
            //then we need to apply a rule to it...
            Tamgu* sube = aNULL;
            Tamguvector* sub = NULL;
            if (v != aNULL) {
                sub = globalTamgu->Providevector();
                sub->storevalue(a->wlabel);
                sube = sub;
            }
            
            ret = gram->rules[a->idrule]->compare(idthread, gram, labels, i, sube, asstring);
            found = false;
            topop = false;
            
            if ((a->status&gnegation) == gnegation) {
                if (!ret) {
                    found = true;
                    if (!(a->status&gkeepout)) {
                        if (asstring) {
                            if (v->Size() == 1 || v->Last()->isVectorContainer()) {
                                v->storevalue("");
                                topop = true;
                            }
                            v->addustringto(labels[i++], -1);
                        }
                        else {
                            v->storevalue(labels[i++]);
                            topop = true;
                        }
                    }
                    else
                        i++;
                    sub->Clear();
                }
            }
            else {
                if (ret)
                    found = true;
                else
                    sube->Release();
            }
            
            if (found) {
                if (ret == gend) {
                    sube->Release();
                    return ret;
                }
                
                if (v != aNULL && !(a->status&gkeepout)) {
                    if (callfunc) {
                        sub->Setreference();
                        if (!gram->callfunction(idthread, a->wlabel, sub, i)) {
                            sub->Resetreference();
                            continue;
                        }
                        
                        sub->protect = 1;
                        sub->Resetreference();
                        sub->protect = 1;
                    }
                    
                    if (a->wlabel[0] == '_') {
                        //then we merge its content with the current structure
                        if (sub->values.size() > 0) {
                            sub->Pop(aZERO);
                            v->Merging(sub);
                            topop = true;
                        }
                        sub->Clear();
                    }
                    else
                        if (sub->values.size()) {
                            topop = true;
                            v->Push(sub);
                        }
                }
                
                sube->Release();
                
                found = false;
                //This is a loop, and we have gotten to here, it means that we had some succesfull loops...
                //We do not need to go back into recursion...
                if ((a->status&gloop) == gloop && a->state == this) {
                    //this could be a final state...
                    if (i == labels.size()) {
                        if ((status&gend) == gend)
                            found = true;
                    }
                    else {
                        c = i;
                        if ((a->status&gloopmax) == gloopmax) {
                            if ((ret = a->state->compare(idthread, gram, labels, i, v, asstring)))
                                return ret;
                            if (topop)
                                v->Pop(aMINUSONE);
                            i = c;
                        }
                        else
                            u = -1;
                        
                        topop = false;
                        continue;
                    }
                }
                
                if (found || (ret = a->state->compare(idthread, gram, labels, i, v, asstring))) {
                    if (ret == gend)
                        return ret;
                    
                    if (a->wlabel[0] == '_' && sub != NULL && !(a->status&gkeepout)) {
                        if (asstring) {
                            sub = (Tamguvector*)v;
                            long x;
                            //Then we try to merge the value together...
                            string mrg;
                            bool replace = true;
                            long sz = sub->values.size();
                            for (x = 1; x < sz; x++) {
                                if (sub->values[x]->Type() != a_string) {
                                    replace = false;
                                    break;
                                }
                                mrg += ((Tamgustring*)sub->values[x])->value;
                            }
                            if (replace) {
                                for (x = 1; x < sz; x++)
                                    v->Pop(aMINUSONE);
                                v->storevalue(mrg);
                            }
                        }
                    }
                    return true;
                }
            }
            
            if (topop)
                v->Pop(aMINUSONE);
            i = c;
            continue;
        }

		found = a->test(labels[i]);
        
        if (found) {
            if (found == 1 && v != aNULL) {
                if (asstring) {
                    if (v->Size() == 1 || v->Last()->isVectorContainer()) {
                        v->storevalue("");
                        topop = true;
                    }
                    if ((a->status&gmulti) == gmulti)
                        v->addustringto(a->wlabel, -1);
                    else
                        v->addustringto(labels[i], -1);
                }
                else {
                    v->storevalue(labels[i]);
                    topop = true;
                }
            }
            
            if ((a->status&gloop) == gloop && a->state == this) {
                i++;
                
                if (i == labels.size()) {
                    if ((status&gend) == gend)
                        return true;
                    return false;
                }
                
                c = i;
                if ((a->status&gloopmax) == gloopmax) {
                    wstring prev = v->Last()->UString();
                    if ((ret = a->state->compare(idthread, gram, labels, i, v, asstring)))
                        return ret;
                    v->Pop(aMINUSONE);
                    if (!topop)
                        v->storevalue(prev);
                    i = c;
                }
                else
                    u = -1;
                
                topop = false;
                continue;
            }
            
            if ((ret = a->state->compare(idthread, gram, labels, ++i, v, asstring)))
                return ret;
        }
        
        if (topop) {
            v->Pop(aMINUSONE);
            topop = false;
        }
        i = c;
    }
    
    if ((status&gend) == gend)
        return true;
    
    return false;
}

char gramstate::compare(short idthread, GrammarBaseAutomaton* gram, wstring& labels, long& i, Tamgu* v, bool asstring) {
    if (globalTamgu->Error(idthread))
        return gend;
    
    if (gram->skipblanks) {
        if (gram->skipblanks == 1) {
            while (i < labels.size() && (labels[i] == L' ' || labels[i] == L'\t')) i++;
        }
        else {
            while (i < labels.size() && (labels[i] <= 32)) i++;
        }
    }
    
    if ((status&gend) == gend) {
        if (!arcs.size() || i == labels.size())
            return true;
    }
    else {
        if (i == labels.size()) { //if the next element is a function call, then we still can have a look...
            if ((status&gfunction) != gfunction)
                return false;
        }
    }
    
    long c = i;
    bool callfunc = false;
    bool topop = false;
    char found;
    char ret;
    
    if (gram->function != NULL)
        callfunc = true;
    
	for (long u = 0; u < arcs.size(); u++) {
		gramarc* a = arcs[u];
		if ((a->status&gfunction) == gfunction) {
			Tamgu* kfunc = globalTamgu->Getdefinition(a->idrule, idthread);
			if (v != aNULL) {
				v->Setreference();
				if (!gram->callfunction(kfunc, idthread, v, i)) {
					v->Setprotect(1);
					v->Resetreference();
					v->Setprotect(1);
					continue;
				}
				v->Setprotect(1);
				v->Resetreference();
				v->Setprotect(1);
			}

			if ((ret = a->state->compare(idthread, gram, labels, i, v, asstring)))
				return ret;

			i = c;
			continue;
		}

		if ((a->status&grule) == grule) {
			//then we need to apply a rule to it...
			Tamgu* sube = aNULL;
			Tamguvector* sub = NULL;
			if (v != aNULL) {
				sub = globalTamgu->Providevector();
				sub->storevalue(a->wlabel);
				sube = sub;
			}


			ret = gram->rules[a->idrule]->compare(idthread, gram, labels, i, sube, asstring);
			found = false;
			topop = false;

			if ((a->status&gnegation) == gnegation) {
				if (!ret) {
					found = true;
					if (!(a->status&gkeepout)) {
						if (asstring) {
							if (v->Size() == 1 || v->Last()->isVectorContainer()) {
								v->storevalue(L"");
								topop = true;
							}
							v->addstringto(labels[i++], -1);
						}
						else {
							v->storevalue(labels[i++]);
							topop = true;
						}
					}
					else
						++i;
					sub->Clear();
				}
			}
			else {
				if (ret)
					found = true;
				else
					sube->Release();
			}

			if (found) {
				if (ret == gend) {
					sube->Release();
					return ret;
				}

				if (v != aNULL && !(a->status&gkeepout)) {
					if (callfunc) {
						sub->Setreference();
						if (!gram->callfunction(idthread, a->wlabel, sub, i)) {
							sub->Resetreference();
							continue;
						}

						sub->protect = 1;
						sub->Resetreference();
						sub->protect = 1;
					}

					if (a->wlabel[0] == '_') {
						//then we merge its content with the current structure
						if (sub->values.size() > 0) {
							sub->Pop(aZERO);
							v->Merging(sub);
							topop = true;
						}
						sub->Clear();
					}
					else
						if (sub->values.size()) {
							v->Push(sub);
							topop = true;
						}
				}

				sube->Release();

				found = false;
				//This is a loop, and we have gotten to here, it means that we had some succesfull loops...
				//We do not need to go back into recursion...
				if ((a->status&gloop) == gloop && a->state == this) {
					//this could be a final state...
					if (i == labels.size()) {
						if ((status&gend) == gend)
							found = true;
					}
					else {
						c = i;
						if ((a->status&gloopmax) == gloopmax) {
							if ((ret = a->state->compare(idthread, gram, labels, i, v, asstring)))
								return ret;
							if (topop)
								v->Pop(aMINUSONE);
							i = c;
						}
						else
							u = -1;
						topop = false;
						continue;
					}
				}

				if (found || (ret = a->state->compare(idthread, gram, labels, i, v, asstring))) {
					if (ret == gend)
						return ret;

					if (a->wlabel[0] == '_' && sub != NULL && !(a->status&gkeepout)) {
						if (asstring) {
							sub = (Tamguvector*)v;
							long x;
							//Then we try to merge the value together...
							wstring mrg;
							bool replace = true;
							long sz = sub->values.size();
							for (x = 1; x < sz; x++) {
								if (!sub->values[x]->isString()) {
									replace = false;
									break;
								}
								mrg += sub->getustring(x);
							}
							if (replace) {
								for (x = 1; x < sz; x++)
									v->Pop(aMINUSONE);
								v->storevalue(mrg);
							}
						}
					}
					return true;
				}
			}

			if (topop)
				v->Pop(aMINUSONE);

			i = c;
			continue;
		}

		wstring labelw;
		if ((a->status&gmulti) == gmulti) {
			found = true;
			if (asstring) {
				//a->idrule contains the label size in characters...
				long limit = a->test(labels, labelw, i);
				if (limit == -1)
					found = false;
				else
					i = limit;
			}
			else {
				if (labels[i] != a->wlabel[0])
					found = false;
			}
		}
		else {
#ifdef WSTRING_IS_UTF16
			if (!checklargeutf16(labels[i]))
				found = a->test(labels[i]);
			else {
				uint32_t r;
				c_utf16_to_unicode(r, labels[i], false);
				c_utf16_to_unicode(r, labels[++i], true);
				found = a->test(r);
			}
#else
			found = a->test(labels[i]);
#endif
		}
        if (found) {
            if (found == 1 && v != aNULL) {
                if (asstring) {
                    if (v->Size() == 1 || v->Last()->isVectorContainer()) {
                        v->storevalue(L"");
                        topop = true;
                    }
                    if ((a->status&gmulti) == gmulti)
                        v->addustringto(labelw, -1);
                    else
                        v->addstringto(labels[i], -1);
                }
                else {
                    v->storevalue(labels[i]);
                    topop = true;
                }
            }
            
            if ((a->status&gloop) == gloop && a->state == this) {
                i++;
                
                if (i == labels.size()) {
                    if ((status&gend) == gend)
                        return true;
                    return false;
                }
                
                c = i;
                if ((a->status&gloopmax) == gloopmax) {
                    wstring prev = v->Last()->UString();
                    if ((ret = a->state->compare(idthread, gram, labels, i, v, asstring)))
                        return ret;
                    v->Pop(aMINUSONE);
                    if (!topop)
                        v->storevalue(prev);
                    i = c;
                }
                else
                    u = -1;
                
                topop = false;
                continue;
            }
            
            if ((ret = a->state->compare(idthread, gram, labels, ++i, v, asstring)))
                return ret;
        }
        
        if (topop) {
            v->Pop(aMINUSONE);
            topop = false;
        }
        i = c;
    }
    
    if ((status&gend) == gend)
        return true;
    
    return false;
}

//Called from Apply for maps...
char gramstate::comparemap(short idthread, wstring& lkey, GrammarBaseAutomaton* gram, vector<wstring>& labels, long& i, Tamgu* m, bool asstring) {
    if (globalTamgu->Error(idthread))
        return gend;
    
    if (gram->skipblanks) {
        if (gram->skipblanks == 1) {
            while (i < labels.size() && (labels[i] == L" " || labels[i] == L"\t")) i++;
        }
        else {
            while (i < labels.size() && (labels[i][0] <= 32)) i++;
        }
    }
    
    if ((status&gend) == gend) {
        if (!arcs.size() || i == labels.size())
            return true;
    }
    else {
        if (i == labels.size()) { //if the next element is a function call, then we still can have a look...
            if ((status&gfunction) != gfunction)
                return false;
        }
    }
    
    long c = i;
    Tamguvector* v = (Tamguvector*)m->Value(lkey);
    
    bool topop = false;
    bool callfunc = false;
    char found;
    char ret;
    
    if (gram->function != NULL)
        callfunc = true;
    
    for (long u = 0; u < arcs.size(); u++) {
        gramarc* a = arcs[u];
        if ((a->status&gfunction) == gfunction) {
            Tamgu* kfunc = globalTamgu->Getdefinition(a->idrule, idthread);
            if (!gram->callfunction(kfunc, idthread, v, i))
                continue;
            
            if ((ret = a->state->comparemap(idthread, lkey, gram, labels, i, m, asstring)))
                return ret;
            
            i = c;
            continue;
        }
        
        if ((a->status&grule) == grule) {
            //then we need to apply a rule to it...
            Tamgu* sub = m->Newinstance(idthread);
            Tamguvector* vsub = globalTamgu->Providevector();
            sub->Push(a->wlabel, vsub);
            
            ret = gram->rules[a->idrule]->comparemap(idthread, a->wlabel, gram, labels, i, sub, asstring);
            found = false;
            topop = false;
            
            if ((a->status&gnegation) == gnegation) {
                if (!ret) {
                    found = true;
                    if (!(a->status&gkeepout)) {
                        if (asstring) {
                            if (v->Size() == 0 || v->Last()->isMapContainer()) {
                                v->storevalue("");
                                topop = true;
                            }
                            v->addustringto(labels[i++], -1);
                        }
                        else {
                            v->storevalue(labels[i++]);
                            topop = true;
                        }
                    }
                    else
                        ++i;
                    sub->Clear();
                }
            }
            else {
                if (ret)
                    found = true;
                else
                    sub->Release();
            }
            
            if (found) {
                if (ret == gend) {
                    sub->Release();
                    return ret;
                }
                
                if (callfunc) {
                    if (!gram->callfunction(idthread, a->wlabel, vsub, i)) {
                        sub->Release();
                        continue;
                    }
                }
                
                //Different cases:
                //a) asstring --> concatenate strings
                //b) else Pushing strings...
                if (!(a->status&gkeepout)) {
                    if (sub->Size()) {
                        topop = true;
                        if (a->wlabel[0] == '_') {
                            //then we merge its content with the current structure
                            //we check if all elements in vsub are strings...
                            v->Merging(vsub);
                        }
                        else
                            v->Push(sub);
                    }
                }
                
                sub->Release();
                
                found = false;
                //This is a loop, and we have gotten to here, it means that we had some succesfull loops...
                //We do not need to go back into recursion...
                if ((a->status&gloop) == gloop && a->state == this) {
                    //this could be a final state...
                    if (i == labels.size()) {
                        if ((status&gend) == gend)
                            found = true;
                    }
                    else {
                        c = i;
                        if ((a->status&gloopmax) == gloopmax) {
                            if ((ret = a->state->comparemap(idthread, lkey, gram, labels, i, m, asstring)))
                                return ret;
                            if (topop)
                                v->Pop(aMINUSONE);
                            i = c;
                        }
                        else
                            u = -1;
                        topop = false;
                        continue;
                    }
                }
                
                if (found || (ret = a->state->comparemap(idthread, lkey, gram, labels, i, m, asstring))) {
                    if (ret == gend)
                        return ret;
                    
                    if (!(a->status&gkeepout) && a->wlabel[0] == '_') {
                        if (asstring) {
                            long x;
                            //Then we try to merge the value together...
                            string mrg;
                            bool replace = true;
                            for (x = 0; x < v->values.size(); x++) {
                                if (v->values[x]->Type() != a_string) {
                                    replace = false;
                                    break;
                                }
                                mrg += ((Tamgustring*)v->values[x])->value;
                            }
                            if (replace) {
                                v->Clear();
                                if (mrg != "")
                                    v->storevalue(mrg);
                            }
                        }
                    }
                    return true;
                }
            }
            
            if (topop)
                v->Pop(aMINUSONE);
            
            i = c;
            continue;
        }
        
        found = a->test(labels[i]);
        
        if (found) {
            if (found == 1) {
                if (asstring) {
                    if (v->Size() == 0 || v->Last()->isMapContainer()) {
                        v->storevalue("");
                        topop = true;
                    }
                    if (v->values.back()->Type() == a_string) {
                        if ((a->status&gmulti) == gmulti)
                            v->addustringto(a->wlabel, -1);
                        else
                            v->addustringto(labels[i], -1);
                    }
                    else {
                        if ((a->status&gmulti) == gmulti)
                            v->storevalue(a->wlabel);
                        else
                            v->storevalue(labels[i]);
                    }
                }
                else {
                    v->storevalue(labels[i]);
                    topop = true;
                }
            }
            
            if ((a->status&gloop) == gloop && a->state == this) {
                i++;
                
                if (i == labels.size()) {
                    if ((status&gend) == gend)
                        return true;
                    return false;
                }
                
                c = i;
                if ((a->status&gloopmax) == gloopmax) {
                    wstring prev = v->Last()->UString();
                    if ((ret = a->state->comparemap(idthread, lkey, gram, labels, i, m, asstring)))
                        return ret;
                    v->Pop(aMINUSONE);
                    if (!topop)
                        v->storevalue(prev);
                    i = c;
                }
                else
                    u = -1;
                
                topop = false;
                continue;
            }
            
            if ((ret = a->state->comparemap(idthread, lkey, gram, labels, ++i, m, asstring)))
                return ret;
        }
        
        if (topop) {
            v->Pop(aMINUSONE);
            topop = false;
        }
        i = c;
    }
    
    if ((status&gend) == gend)
        return true;
    
    return false;
}

char gramstate::comparemap(short idthread, wstring& lkey, GrammarBaseAutomaton* gram, wstring& labels, long& i, Tamgu* m, bool asstring) {
    if (globalTamgu->Error(idthread))
        return gend;
    
    if (gram->skipblanks) {
        if (gram->skipblanks == 1) {
            while (i < labels.size() && (labels[i] == L' ' || labels[i] == L'\t')) i++;
        }
        else {
            while (i < labels.size() && (labels[i] <= 32)) i++;
        }
    }
    
    if ((status&gend) == gend) {
        if (!arcs.size() || i == labels.size())
            return true;
    }
    else {
        if (i == labels.size()) { //if the next element is a function call, then we still can have a look...
            if ((status&gfunction) != gfunction)
                return false;
        }
    }
    
    long c = i;
    Tamguvector* v = (Tamguvector*)m->Value(lkey);
    
    bool topop = false;
    bool callfunc = false;
    char found;
    char ret;
    
    if (gram->function != NULL)
        callfunc = true;
    
    for (long u = 0; u < arcs.size(); u++) {
        gramarc* a = arcs[u];
        if ((a->status&gfunction) == gfunction) {
            Tamgu* kfunc = globalTamgu->Getdefinition(a->idrule, idthread);
            if (!gram->callfunction(kfunc, idthread, v, i))
                continue;
            
            if ((ret = a->state->comparemap(idthread, lkey, gram, labels, i, m, asstring)))
                return ret;
            
            i = c;
            continue;
        }
        
        if ((a->status&grule) == grule) {
            //then we need to apply a rule to it...
            Tamgu* sub = m->Newinstance(idthread);
            Tamguvector* vsub = globalTamgu->Providevector();
            sub->Push(a->wlabel, vsub);
            
            ret = gram->rules[a->idrule]->comparemap(idthread, a->wlabel, gram, labels, i, sub, asstring);
            found = false;
            topop = false;
            
            if ((a->status&gnegation) == gnegation) {
                if (!ret) {
                    found = true;
                    if (!(a->status&gkeepout)) {
                        if (asstring) {
                            if (v->Size() == 0 || v->Last()->isMapContainer()) {
                                v->storevalue("");
                                topop = true;
                            }
                            v->addstringto(labels[i++], -1);
                        }
                        else {
                            v->storevalue(labels[i++]);
                            topop = true;
                        }
                    }
                    else
                        ++i;
                    sub->Clear();
                }
            }
            else {
                if (ret)
                    found = true;
                else
                    sub->Release();
            }
            
            if (found) {
                if (ret == gend) {
                    sub->Release();
                    return ret;
                }
                
                if (callfunc) {
                    if (!gram->callfunction(idthread, a->wlabel, vsub, i)) {
                        sub->Release();
                        continue;
                    }
                }
                
                //Different cases:
                //a) asstring --> concatenate strings
                //b) else Pushing strings...
                if (!(a->status&gkeepout)) {
                    if (sub->Size()) {
                        topop = true;
                        if (a->wlabel[0] == '_') {
                            //then we merge its content with the current structure
                            //we check if all elements in vsub are strings...
                            v->Merging(vsub);
                        }
                        else
                            v->Push(sub);
                    }
                }
                
                sub->Release();
                
                found = false;
                //This is a loop, and we have gotten to here, it means that we had some succesfull loops...
                //We do not need to go back into recursion...
                if ((a->status&gloop) == gloop && a->state == this) {
                    //this could be a final state...
                    if (i == labels.size()) {
                        if ((status&gend) == gend)
                            found = true;
                    }
                    else {
                        c = i;
                        if ((a->status&gloopmax) == gloopmax) {
                            if ((ret = a->state->comparemap(idthread, lkey, gram, labels, i, m, asstring)))
                                return ret;
                            i = c;
                        }
                        else
                            u = -1;
                        
                        topop = false;
                        continue;
                    }
                }
                
                if (found || (ret = a->state->comparemap(idthread, lkey, gram, labels, i, m, asstring))) {
                    if (ret == gend)
                        return ret;
                    
                    if (!(a->status&gkeepout) && a->wlabel[0] == '_') {
                        if (asstring) {
                            long x;
                            //Then we try to merge the value together...
                            wstring mrg;
                            bool replace = true;
                            for (x = 0; x < v->values.size(); x++) {
                                if (!v->values[x]->isString()) {
                                    replace = false;
                                    break;
                                }
                                mrg += v->getustring(x);
                            }
                            if (replace) {
                                v->Clear();
                                if (mrg != L"")
                                    v->storevalue(mrg);
                            }
                        }
                    }
                    return true;
                }
            }
            
            if (topop)
                v->Pop(aMINUSONE);
            
            i = c;
            continue;
        }
        
        wstring labelw;
        if ((a->status&gmulti) == gmulti) {
            found = true;
            if (asstring) {
                //a->idrule contains the label size in characters...
                long limit = a->test(labels, labelw, i);
                if (limit == -1)
                    found = false;
                else
                    i = limit;
            }
            else {
                if (labels[i] != a->wlabel[0])
                    found = false;
            }
        }
		else {
#ifdef WSTRING_IS_UTF16
			if (!checklargeutf16(labels[i]))
				found = a->test(labels[i]);
			else {
				uint32_t r;
				c_utf16_to_unicode(r, labels[i], false);
				c_utf16_to_unicode(r, labels[++i], true);
				found = a->test(r);
			}
#else
			found = a->test(labels[i]);
#endif
		}
        if (found) {
            if (found == 1) {
                if (asstring) {
                    if (v->Size() == 0 || v->Last()->isMapContainer()) {
                        v->storevalue(L"");
                        topop = true;
                    }
                    if (v->values.back()->isString()) {
                        if ((a->status&gmulti) == gmulti)
                            v->addustringto(labelw, -1);
                        else
                            v->addstringto(labels[i], -1);
                    }
                    else {
                        if ((a->status&gmulti) == gmulti)
                            v->storevalue(a->wlabel);
                        else
                            v->storevalue(labels[i]);
                    }
                }
                else {
                    v->storevalue(labels[i]);
                    topop = true;
                }
            }
            
            if ((a->status&gloop) == gloop && a->state == this) {
                i++;
                
                if (i == labels.size()) {
                    if ((status&gend) == gend)
                        return true;
                    return false;
                }
                
                c = i;
                if ((a->status&gloopmax) == gloopmax) {
                    wstring prev = v->Last()->UString();
                    if ((ret = a->state->comparemap(idthread, lkey, gram, labels, i, m, asstring)))
                        return ret;
                    v->Pop(aMINUSONE);
                    if (!topop)
                        v->storevalue(prev);
                    i = c;
                }
                else
                    u = -1;
                
                topop = false;
                continue;
            }
            
            if ((ret = a->state->comparemap(idthread, lkey, gram, labels, ++i, m, asstring)))
                return ret;
        }
        
        if (topop) {
            v->Pop(aMINUSONE);
            topop = false;
        }
        i = c;
    }
    
    if ((status&gend) == gend)
        return true;
    
    return false;
}

Tamgu* GrammarAutomaton::Apply(short idthread, Tamgu* res, vector<wstring>& labels, bool asstring) {
    wstring lkey;
    mx = 0;
    pmx = 0;
    
    for (long r = 0; r<rules.size(); r++) {
        lkey = rdictionary[r];
        long i = 0;
        
        if (res->isMapContainer()) {
            res->Push(lkey, new Tamguvector);
            if (rules[r]->comparemap(idthread, lkey, this, labels, i, res, asstring) && i == labels.size())
                return res;
        }
        else {
            if (res->isVectorContainer())
                res->storevalue(lkey);
            
            if (rules[r]->compare(idthread, this, labels, i, res, asstring) && i == labels.size()) {
                if (res == aNULL)
                    return aTRUE;
                
                return res;
            }
        }
        
        res->Clear();
    }
    
    if (res == aNULL)
        return aFALSE;
    
    res->Clear();
    return res;
}


Tamgu* Tamgugrammar::Apply(short idthread, Tamgu* res, vector<wstring>& labels, bool asstring) {
    if (automaton == NULL)
        return res;
    
    Tamgu* r = automaton->Apply(idthread, res, labels, asstring);
    if (globalTamgu->Error(idthread)) {
        r->Clear();
        return globalTamgu->Errorobject(idthread);
    }
    return r;
}


//a in b...
Tamgu* Tamgugrammar::in(Tamgu* context, Tamgu* a, short idthread) {
    if (a->Type() == a_uvector)
        return Apply(idthread, aNULL, ((Tamguuvector*)a)->values, false);
    
    if (a->isVectorContainer()) {
        vector<wstring> v;
        for (long i = 0; i < a->Size(); i++)
            v.push_back(a->getustring(i));
        return Apply(idthread, aNULL, v, false);
    }
    
    wstring w = a->UString();
    return Apply(idthread, aNULL, w, false);
}


Tamgu* GrammarAutomaton::Apply(short idthread, Tamgu* res, wstring& labels, bool asstring) {
    wstring lkey;
    mx = 0;
    pmx = 0;
    
    for (long r = 0; r<rules.size(); r++) {
        lkey = rdictionary[r];
        long i = 0;
        
        if (res->isMapContainer()) {
            
            res->Push(lkey, new Tamguvector);
            if (rules[r]->comparemap(idthread, lkey, this, labels, i, res, asstring) && i == labels.size())
                return res;
        }
        else {
            if (res->isVectorContainer())
                res->storevalue(lkey);
            
            if (rules[r]->compare(idthread, this, labels, i, res, asstring) && i == labels.size()) {
                if (res == aNULL)
                    return aTRUE;
                
                return res;
            }
        }
        
        res->Clear();
    }
    
    if (res == aNULL)
        return aFALSE;
    
    res->Clear();
    return res;
}

Tamgu* Tamgugrammar::Apply(short idthread, Tamgu* res, wstring& labels, bool asstring) {
    if (automaton == NULL)
        return res;
    
    Tamgu* r = automaton->Apply(idthread, res, labels, asstring);
    if (globalTamgu->Error(idthread)) {
        res->Release();
        return globalTamgu->Errorobject(idthread);
    }
    
    if (r != res)
        res->Release();
    
    return r;
}

//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
Tamgu* Tamgugrammar::MethodApply(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter should be a Tamgu (탐구) rule
    
    Tamgu* kvect = Selectacontainer(contextualpattern, idthread);
    
    Tamgu* kval = callfunc->Evaluate(0, contextualpattern, idthread);
    if (kval->Type() == a_uvector)
        return Apply(idthread, kvect, ((Tamguuvector*)kval)->values, false);
    
    //Else we build our vector out of the characters...
    if (kval->isVectorContainer()) {
        vector<wstring> v;
        for (long i = 0; i < kval->Size(); i++)
            v.push_back(kval->getustring(i));
        return Apply(idthread, kvect, v, false);
    }
    
    if (kvect->isVectorContainer()) {
        if (kvect->Type() != a_vector) {
            kvect->Release();
            kvect = globalTamgu->Providevector();
        }
    }
    else {
        if (kvect->isMapContainer()) {
            if (kvect->Type() != a_map && kvect->Type() != a_treemap) {
                kvect->Release();
                kvect = globalTamgu->Providemap();
            }
        }
    }
    
    wstring wstr = kval->UString();
    return Apply(idthread, kvect, wstr, true);
}
