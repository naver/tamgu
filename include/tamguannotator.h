/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguannotator.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#ifndef tamguannotator_h
#define tamguannotator_h
#include "tamguautomaton.h"
#include "tamguuvector.h"
#include "instructions.h"
#include "tamgutransducer.h"

//----------------------------------------------------------------------------
class Tamguannotator;
class Tamgutransducer;
class An_context;
class Tamgusynode;


//This is the implementation of the ANNOTATION rules...
//An_any is the parent class of all actions...
//It also matches the "any" or ? symbol in rules...
class An_any {
public:
    
    virtual char compare(wstring& w, An_context* context) {
        return true;
    }
    
    virtual char check(An_context* c) {
        return true;
    }
    
    virtual ant_type type() {
        return ant_any;
    }
    
    virtual bool same(An_any* a) {
        if (a->type()==ant_any)
            return true;
        return false;
    }
    
    virtual wstring value() {
        return L"?";
    }
    
    ~An_any() {}
};

//This is a meta
class An_meta : public An_any {
public:
    
    unsigned char action;
    
    An_meta(uchar c) {
        action=c;
    }

    wstring value() {
        wstring w(L"%");
        w+=action;
        return w;
    }

    ant_type type() {
        return ant_meta;
    }

    bool same(An_any* a) {
        if (a->type()==ant_meta && ((An_meta*)a)->action==action)
            return true;
        return false;
    }

    char check(An_context* c) {
        return false;
    }

    char compare(wstring& chr, An_context* context) {
        if (chr==L"")
            return false;
        
        wchar_t car=chr[0];
        switch(action) {
            case 'C':
                return s_is_upper(chr);
            case 'a':
                if (car=='_' || s_is_alpha(chr) || car == 160)
                    return true;
                return false;
            case 'c':
                return s_is_lower(chr);
            case 'd':
                return s_is_digit(chr);
            case 'e':
                return s_is_emoji(chr);
            case 'H':
                return s_is_hangul(chr);
            case 'p':
                return s_is_punctuation(chr);
            case 'S':
                if (car <= 32)
                    return true;
                return false;
            case 's':
                if (car == 9 || car == 32)
                    return true;
                return false;
            case 'r':
                if (car == 10 || car == 13)
                    return true;
                return false;
            case 'x': //hexadecimal character
                if ((car>='A' && car <= 'F') || (car>='a' && car <= 'f') || (car >= '0' && car <= '9'))
                    return true;
                return false;
            default:
                if (action == (uchar)car)
                    return true;
                return false;
        }
    }
};

//this class handles automate expression
class An_automaton : public An_any {
public:
    Au_automaton* action;
    wstring reference;
    
    wstring value() {
        return reference;
    }
    
    bool same(An_any* a) {
        if (a->type()==ant_automaton && ((An_automaton*)a)->reference==reference)
            return true;
        return false;
    }

    An_automaton(string reg);
    
    char compare(wstring& chr, An_context* context);
    
    ant_type type() {
        return ant_automaton;
    }
    
    char check(An_context* c) {
        return false;
    }

    ~An_automaton();
};

#ifdef Tamgu_REGEX
class An_regex : public An_any {
public:
    wregex* action;
    wstring reference;
    
    An_regex(wstring& rgx);
    char compare(wstring& chr, An_context* context);
    
    wstring value() {
        return reference;
    }
    
    bool same(An_any* a) {
        if (a->type()==ant_regex && ((An_regex*)a)->reference==reference)
            return true;
        return false;
    }

    ant_type type() {
        return ant_regex;
    }

    char check(An_context* c) {
        return false;
    }

    ~An_regex();
};

#endif

class An_token : public An_any {
public:
    
    wstring action;
    
    An_token(wstring& tok): action(tok) {}
    
    ant_type type() {
        return ant_token;
    }

    wstring value() {
        return action;
    }
    
    bool same(An_any* a) {
        if (a->type()==ant_token && ((An_token*)a)->action==action)
            return true;
        return false;
    }

    char compare(wstring& chr, An_context* context);
    char check(An_context* c);

};

class An_label : public An_any {
public:
    
    wstring action;
    
    bool same(An_any* a) {
        if (a->type()==ant_label && ((An_label*)a)->action==action)
            return true;
        return false;
    }
    
    wstring value() {
        return action;
    }
    
    An_label(wstring& tok) {
        action=tok.substr(1,tok.size()-1);
    }
    
    ant_type type() {
        return ant_label;
    }
    
    char compare(wstring& chr, An_context* context);
    char check(An_context* c);
};

class An_orlabels : public An_any {
public:
    
    vector<wstring> actions;
    vector<bool> negations;

    bool same(An_any* a) {
        if (a->type()==ant_orlabels && ((An_orlabels*)a)->actions==actions)
            return true;
        return false;
    }
    
    ant_type type() {
        return ant_orlabels;
    }
    
    char compare(wstring& chr, An_context* context);
    char check(An_context* c);
};

class An_andlabels : public An_any {
public:
    
    vector<wstring> actions;
    vector<bool> negations;
    
    bool same(An_any* a) {
        if (a->type()==ant_andlabels && ((An_andlabels*)a)->actions==actions)
            return true;
        return false;
    }
    
    ant_type type() {
        return ant_andlabels;
    }
    
    char compare(wstring& chr, An_context* context);
    char check(An_context* c);
};

class An_lemma : public An_any {
public:
    
    wstring action;
    
    wstring value() {
        return action;
    }

    An_lemma(wstring& tok) {
        action=tok.substr(1,tok.size()-1);
    }
    
    bool same(An_any* a) {
        if (a->type()==ant_lemma && ((An_lemma*)a)->action==action)
            return true;
        return false;
    }

    ant_type type() {
        return ant_lemma;
    }
    
    char compare(wstring& chr, An_context* context);
    char check(An_context* c);
};


class An_call : public An_any {
public:
    TamguCallFunction call;
    
    An_call(Tamgu* f, bool two) : call(f) {
        //We prepare our variables...
        if (two) {
            call.arguments.push_back(aNULL);
            call.arguments.push_back(aNULLLet);
        }
        else
            call.arguments.push_back(aNULLLet);
    }
    
    char compare(wstring& chr, An_context* context);
    char check(An_context* c);

    bool same(An_any* a) {
        return false;
    }

};

class An_epsilon : public An_any {
public:

    ant_type type() {
        return ant_epsilon;
    }
    
    wstring value() {
        return L"";
    }
    
    char compare(wstring& chr, An_context* context) {
        return 2;
    }

    char check(An_context* c) {
        return 2;
    }

    bool same(An_any* a) {
        if (a->type()==ant_epsilon)
            return true;
        return false;
    }
};

class An_state;
class An_arc {
public:

    An_state* state;
    An_any* atom;
    
    bool marque;
    uchar status;
    
    An_arc(An_any* a) {
        atom=a;
        state=NULL;
        marque=false;
        status=0;
    }
    
    void add(An_state* s) {
        state=s;
    }
    
    long apply(vector<wstring>& words, long i, An_context* context);
    long apply(vector<wstring>& words, long i, vector<long>& kept, An_context* context);
    bool apply(wstring& word, An_context* context);
    bool apply(An_context* context);
    
    void addtotail(An_state* s);
    bool scanend();
    
    ~An_arc() {
        delete atom;
    }
};

//A state is an atom and a list of states...
class An_state {
public:
    
    VECTE<An_arc*> arcs;
    short idmax;
    short id;
    uchar status;
    bool marque;

    An_state(short i) {
        id = i;
        status=0;
        marque=false;
        idmax=-1;
    }
    
    bool isend() {
        if ((status&an_end)==an_end)
            return true;
        return false;
    }
    
    void setfinal() {
        status |= an_final;
    }

    void removefinal() {
        status &= ~an_final;
    }
    
    bool isfinal() {
        if ((status&an_final)==an_final) {
            status &=~an_final;
            return true;
        }
        return false;
    }
    
    void setmax(short m);

    long apply(vector<wstring>& words, long i, An_context* context);
    long apply(vector<wstring>& words, long i, vector<long>& kept, An_context* context);
    bool apply(wstring& word, An_context* context);
    bool apply(An_context* context);

    //we explore the states and find out whether epsilon arcs connect to end state
    //in that case, we modify the status of the current node, as it can be an end as well
    bool scanend();
    
    void addtotail(An_state* s);
};

class An_rule;
class An_rules : public TamguObject {
public:
    
    Au_automate lexiconrulecheck;
#if (defined(APPLE) || defined(WIN32) || __GNUC__ > 4)
    hmap<wstring, Au_automaton> lexicons;
    hmap<wstring, vector<An_arc*> > lexarcs;
    hmap<wstring, hmap<wstring, An_state*> > lexconstraints;
#else
    map<wstring, Au_automaton> lexicons;
    map<wstring, vector<An_arc*> > lexarcs;
    map<wstring, map<wstring, An_state*> > lexconstraints;
#endif
    vector<long> maxloops;
    VECTE<An_state*> states;
    VECTE<An_arc*> arcs;

    vector<An_rule*> starts;
    vector<wstring> multiwords;

    long asize, ssize;

    bool remove;
    bool negation;
    bool lexiconpresent;


    An_rules(TamguGlobal* g) : TamguObject(g) {
        asize=0;
        ssize=0;
        remove=false;
        negation=false;
        lexiconpresent=false;
    }
    
    
    short Type() {
        return a_rules;
    }
    
    bool compare(wstring& chr, wstring& action, An_context* context);
    An_state* newstate() {
        An_state* as=new An_state(states.size());
        states.push_back(as);
        return as;
    }
    
    An_arc* newarc(An_any* a, An_state* s=NULL) {
        An_arc* as=new An_arc(a);
        if (remove)
            as->status|=an_remove;
        if (negation)
            as->status|=an_negation;
        as->state=s;
        arcs.push_back(as);
        return as;
    }

    bool storerule(An_rule* r);
    void unmarque() {
        long i;
        for (i=asize;i<arcs.last;i++)
            arcs[i]->marque=false;
        for (i=ssize;i<states.last;i++)
            states[i]->marque=false;
    }
    
    void cleanfinal() {
        for (long i=ssize;i<states.last;i++)
            states[i]->status &= ~an_final;
    }
    
    void getpos() {
        asize = arcs.last;
        ssize = states.size();
    }
    
    ~An_rules() {
        states.wipe();
        arcs.wipe();
    }
};

class An_rulesConst : public An_rules {
public:
    
    An_rulesConst(TamguGlobal* g) : An_rules(g) {}
    
    void Release() {}
    void Resetreference(short) {}
    void Setreference(short) {}
    void Setreference() {}
    void Addreference(unsigned short inv, short inc=1) {}


};

//---------------------------------------------------------------
class An_context {
public:
#if (defined(APPLE) || defined(WIN32) || __GNUC__ > 4)
    hmap<wstring, hmap<wstring, bool> > labels;
    hmap<wstring, vector<wstring> > features;
    hmap<wstring, hmap<wstring,bool> > lemmaToword;
    hmap<wstring, vector<wstring> > wordTolemma;
#else //there is a bug in GCC to handle unordered_map with wstring and objects
    map<wstring, hmap<wstring, bool> > labels;
    map<wstring, hmap<wstring,bool> > lemmaToword;
    map<wstring, vector<wstring> > wordTolemma;
    map<wstring, vector<wstring> > features;
#endif

    vector<wstring> classes;
    vector<vector<long> > wordlabels;

    vector<long> maxloops;
    vector<long> counters;
    vector<Tamgusynode*> nodes;

    vector<wstring> thewords;
    Tamguannotator* annotator;
    
    long idword;
    short idthread;
    
    An_context(Tamguannotator* a, short idth) {
        annotator=a;
        idthread=idth;
        idword=0;
    }
    
    char check(long i) {
        if (counters[i]==maxloops[i])
            return false;
        counters[i]++;
        return true;
    }
    
    void clearcounters() {
        for (long i=0;i<counters.size();i++)
            counters[i]=0;
    }
    
    void clean();
    void clear(Tamguuvector* v);
    
    void set(An_context& c);
};

class An_rule : public Tamgu {
public:
    
    An_state* first;
    An_state* last;
    An_state* next;

    wstring category;
    bool hasremove;
    bool hasmax;
    bool lexicon;
    bool classing;
    bool removing;
    bool islexiconrule;

    An_rule() {
        islexiconrule=false;
        lexicon=false;
        classing=false;
        removing=false;
        hasremove=false;
        hasmax=false;
        first=globalTamgu->gTheAnnotationRules->newstate();
        first->status=0;
        last=first;
        next=NULL;
    }
    
    void unmarque() {
        globalTamgu->gTheAnnotationRules->unmarque();
    }
    
    void cleanfinal() {
        globalTamgu->gTheAnnotationRules->cleanfinal();
    }
    
    bool applyclass(An_context* context);
    
    long apply(vector<wstring>& words, long i, An_context* context) {
        return first->apply(words,i,context);
    }

    long apply(vector<wstring>& words, long i, vector<long>& kept, An_context* context) {
        return first->apply(words,i, kept,context);
    }

    An_arc* addarc(An_any* a,bool first) {
        An_arc* arc=globalTamgu->gTheAnnotationRules->newarc(a);
        if (last->arcs.last && first && !globalTamgu->gTheAnnotationRules->negation)
            last->arcs.insert(0,arc);
        else
            last->arcs.push_back(arc);
        
        last->removefinal();
        if (next==NULL)
            arc->add(globalTamgu->gTheAnnotationRules->newstate());
        else
            arc->add(next);
        last=arc->state;
        return arc;
    }
    
    void scanend() {
        unmarque();
        first->scanend();
    }
};


//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
//This typedef defines a type "annotatorMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguannotator::*annotatorMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------

class Tamguannotator : public TamguObject {
public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<annotatorMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging basebin_hash<unsigned long> exported;
    
	static Exchanging short idtype;
    
    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    hmap<wstring,bool> selections;
    
    vector<An_context> contexts;
    
    An_rules* rules;
    An_rules* lexicons;
    
    Tamgu* function;
    Tamgutransducer* lexicon;
    
    char compiledlexicon;
    char checklexicon;
    bool spans;
    bool ownlexicon;
    bool loadedlexicon;
    
    //---------------------------------------------------------------------------------------------------------------------
    Tamguannotator(TamguGlobal* g = NULL, Tamgu* parent = NULL) : TamguObject(g, parent) {
        //Do not forget your variable initialisation
        rules = NULL;
        lexicons = NULL;
        function=NULL;
        spans=true;
        lexicon = NULL;
        compiledlexicon = false;
        checklexicon = 0;
        ownlexicon=false;
        loadedlexicon = false;
    }

    Tamguannotator(Tamgu* f)  {
        //Do not forget your variable initialisation
        rules = NULL;
        lexicons = NULL;
        function=f;
        spans=true;
        lexicon = NULL;
        compiledlexicon = false;
        checklexicon = 0;
        ownlexicon=false;
        loadedlexicon = false;
    }

    ~Tamguannotator() {
        for (long i=0; i< contexts.size(); i++)
            contexts[i].clean();
    }
    
    //----------------------------------------------------------------------------------------------------------------------
    Tamgu* Put(Tamgu* v, Tamgu* i, short idthread);
    
    Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread);

    short Type() {
        return Tamguannotator::idtype;
    }
    
    

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
        return "annotator";
    }
    
    //The next two functions work in conjunction...
    //If your object is very atomic (such as a number or a string)
    //Then when it is passed to a function or stored into a container,
    //it might be worth duplicating it.
    // In that case: duplicateForCall should return true...
    
    //However if your object is complex and probably unique through out the code
    //Then duplicateForCall should return false, and Atom should always reduce to a "return this;" only...
    Tamgu* Atom(bool forced=false) {
        return this;
    }
    
    bool duplicateForCall() {
        return false;
    }
    
    unsigned long CallBackArity() {
        return P_TWO;
    }

    Tamgu* lexiconcompiling();
    
    bool compare(wstring& chr, wstring& action, An_context* context) {
        switch(checklexicon) {
            case 1:
                return rules->compare(chr, action, context);
            case 2:
                return lexicons->compare(chr,action, context);
            case 3:
                if (rules->compare(chr, action, context) || lexicons->compare(chr, action, context))
                    return true;
        }
        return false;
    }

    void Initrules() {
        if (rules == NULL) {
            if (globalTamgu->gTheAnnotationRules == NULL)
                return;
            rules = globalTamgu->gTheAnnotationRules;
        }
        if (lexicons == NULL) {
            if (globalTamgu->gTheAnnotationRules == NULL)
                lexicons = rules;
            else
                lexicons = globalTamgu->gTheAnnotationRules;
        }
    }
    
    void Resetreference(short ref) {
        if (rules != NULL)
            rules->Resetreference(ref);
        if (ownlexicon && lexicon != NULL)
            lexicon->Resetreference(ref);
        TamguObject::Resetreference(ref);
    }
    
    void Setreference(short ref) {
        if (rules != NULL)
            rules->Setreference(ref);
        if (ownlexicon && lexicon != NULL)
            lexicon->Setreference(ref);
        TamguObject::Setreference(ref);
    }
    
    void Setreference() {
        if (rules != NULL)
            rules->Setreference();
        if (ownlexicon && lexicon != NULL)
            lexicon->Setreference();
        TamguObject::Setreference();
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
        return new Tamguannotator(f);
    }
    
    TamguIteration* Newiteration(bool direction) {
        return aITERNULL;
    }
    
    static void AddMethod(TamguGlobal* g, string name, annotatorMethod func, unsigned long arity, string infos);
    static bool InitialisationModule(TamguGlobal* g, string version);
    
    void Methods(Tamgu* v) {
        hmap<string, string>::iterator it;
        for (it = infomethods.begin(); it != infomethods.end(); it++)
            v->storevalue(it->first);
    }
    
    string Info(string n) {
        
        if (infomethods.find(n) != infomethods.end())
            return infomethods[n];
        return "Unknown method";
    }
    

    Tamgu* Apply(Tamguuvector* uvect, Tamgu* res, bool computelexicon, short idthread);
    Tamgu* Apply(An_context* context,Tamgu* res, bool computelexicon, short idthread);

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is a sample  of a function that could be implemented for your needs.
    void Applylexicon(vector<wstring>& words, An_context* context);
    Tamgu* Execution(Tamgu* res, Tamgu* txt, short idthread, uchar computelexicon);
    
    Tamgu* MethodParse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodTokenize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodApply(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodClear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        selections.clear();
        return aTRUE;
    }
    
    Tamgu* MethodSelection(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodCompile(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodLexicon(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodCheckLabel(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodCheckWord(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodLabels(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodTokens(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodWords(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodToken(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodWord(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    Tamgu* MethodDependencies(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    Tamgu* MethodSpans(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        spans = callfunc->Evaluate(0, contextualpattern,idthread)->Boolean();
        return aTRUE;
    }

    Tamgu* MethodSelect(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* vect=callfunc->Evaluate(0,contextualpattern,idthread);
        if (vect->isVectorContainer()) {
            wstring v;
            for (long i=0;i<vect->Size();i++) {
                v=vect->getustring(i);
                selections[v]=true;
            }
            return aTRUE;
        }
        
        return globalTamgu->Returnerror("Expected a vector as input");
    }

    Tamgu* MethodCompileLexicons(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    
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
    
    
    
    string String() {
        return "";
    }
    
    wstring UString() {
        return L"";
    }
    
    long Integer() { return 0; }
    short Short() { return 0;}
    double Float() { return 0; }
    bool Boolean() { return false; }
    BLONG Long() { return 0; }
    unsigned char Byte() {
        return 0;
    }
    
    long Size() { return 0; }
    
};


#endif
