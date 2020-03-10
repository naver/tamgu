/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguautomaton.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#ifndef tamguautomaton_h
#define tamguautomaton_h

#include "tamguboost.h"


typedef enum{ an_epsilon=0, an_end=1, an_remove=2, an_negation=4, an_mandatory=8, an_error=12, an_rule=16, an_final=32, an_beginning=64, an_ending=128} an_flag;
typedef enum{an_meta=1, an_automaton, an_regex, an_token, an_any, an_lemma, an_label, an_orlabels, an_andlabels} an_type;

typedef enum{aut_reg=1,aut_reg_plus,aut_reg_star,
    aut_meta, aut_meta_plus, aut_meta_star,
    aut_any,aut_any_plus,aut_any_star,
    aut_ocrl_brk, aut_ccrl_brk, aut_ccrl_brk_plus, aut_ccrl_brk_star,
    aut_obrk, aut_cbrk, aut_cbrk_plus, aut_cbrk_star,
    aut_opar, aut_cpar, aut_negation
}
aut_actions;

//----------------------Meta character evaluation------------------------------------------------
void EvaluateMetaInString(string& w, string wsub);
void EvaluateMetaInString(wstring& w, wstring& wsub);
void replaceescapedcharacters(string& str, string s);
//------------------------character automatons---------------------------------------------------
class Au_automaton;
class Au_automatons;
class Au_automate;
//----------------------Meta character patterns--------------------------------------------------
void clear_pattern();
void clear_automates();
Au_automate* getautomate(string& w);
Au_automate* getautomate(Tamgu* tmg);
//-----------------------------------------------------------------------------------------------

class Au_state;
class Au_any {
public:
    bool vero;
    an_type type;
    
    Au_any(unsigned char t) {
        type=(an_type)t;
        vero = true;
    }
    
    virtual bool same(Au_any* a) {
        if (a->Type()==type && a->vero == vero)
            return true;
        return false;
    }
    
    an_type Type() {
        return type;
    }
    
    virtual char compare(TAMGUCHAR c) {
        return vero;
    }
    
    void setvero(bool neg) {
        vero = !neg;
    }
};

class Au_char : public Au_any {
public:
    wchar_t action;
    
    Au_char(wchar_t a, unsigned char t) : Au_any(t) {
        action=a;
    }
    
    bool same(Au_any* a) {
        if (a->Type()==type && a->vero == vero && ((Au_char*)a)->action==action)
            return true;
        return false;
    }

    char compare(TAMGUCHAR c) {
        if (action==c)
            return vero;
        return !vero;
    }
    
};

class Au_epsilon : public Au_any {
public:
    
    Au_epsilon()  : Au_any(an_epsilon) {}
    
    bool same(Au_any* a) {
        if (a->Type()==an_epsilon && a->vero == vero)
            return true;
        return false;
    }

    char compare(TAMGUCHAR c) {
        return 2;
    }
    
};

class Au_meta : public Au_any {
public:
    
    wchar_t action;
    
    Au_meta(uchar a, unsigned char t) : Au_any(t) {
        action=a;
    }
    
    bool same(Au_any* a) {
        if (a->Type()==type && a->vero == vero && ((Au_meta*)a)->action==action)
            return true;
        return false;
    }

    //CHSacdnprsx
    char compare(TAMGUCHAR car) {
        
        switch(action) {
            case 'C':
                if (c_is_upper(car))
                    return vero;
                return !vero;
            case 'e':
                if (c_is_emoji(car))
                    return vero;
                return !vero;
            case 'E':
                if (c_is_emojicomp(car))
                    return vero;
                return !vero;
            case 'H':
                if (c_is_hangul(car))
                    return vero;
                return !vero;
            case 'S':
                if (car <= 32 || car == 160)
                    return vero;
                return !vero;
            case 'a':
                if (car=='_' || c_is_alpha(car))
                    return vero;
                return !vero;
            case 'c':
                if (c_is_lower(car))
                    return vero;
                return !vero;
            case 'd':
                if (c_is_digit(car))
                    return vero;
                return !vero;
            case 'n':
                if (car == 160)
                    return vero;
                return !vero;
            case 'p':
                if (c_is_punctuation(car))
                    return vero;
                return !vero;
            case 'r':
                if (car == 10 || car == 13)
                    return vero;
                return !vero;
            case 's':
                if (car == 9 || car == 32 || car == 160)
                    return vero;
                return !vero;
            case 'x': //hexadecimal character
                if ((car>='A' && car <= 'F') || (car>='a' && car <= 'f') || (car >= '0' && car <= '9'))
                    return vero;
                return !vero;
            default:
                if (action == car)
                    return vero;
                return !vero;
        }
    }
};

class Au_arc {
public:
    Au_any* action;
    Au_state* state;
    unsigned char mark;
    bool inloop;
    
    Au_arc(Au_any* a) {
        action=a;
        inloop=false;
        state=NULL;
        mark=false;
    }
    
    ~Au_arc() {
        delete action;
    }
    
    an_type Type() {
        return action->Type();
    }

    bool same(Au_arc* a) {
        return action->same(a->action);
    }
    
    bool checkfinalepsilon();

    bool find(wstring& w, wstring& sep, long i, vector<long>& res);

};

class Au_state {
public:
    VECTE<Au_arc*> arcs;
    uchar status;
    unsigned char mark;
    
    Au_state() {
        status = an_epsilon;
        mark=false;
    }

    void storerulearcs(hmap<long,bool>& rules);

    virtual long idx() {return -1;}
    bool isend() {
        if ((status&an_end)==an_end)
            return true;
        return false;
    }
    
    void removeend() {
        status &=~an_end;
    }

    bool ismandatory() {
        if ((status&an_mandatory)==an_mandatory)
            return true;
        return false;
    }

    void removemandatory() {
        status &=~an_mandatory;
    }
    
    bool isfinal() {
        if (arcs.last == 0)
            return true;
        
        if ((status&an_final)==an_final)
            return true;
        return false;
    }
    
    bool isrule() {
        if ((status&an_rule)==an_rule)
            return true;
        return false;
    }
    
    char get(wstring& w, long i, hmap<long,bool>& rules);

    bool match(wstring& w, long i);
    bool find(wstring& w, wstring& sep, long i, vector<long>& res);

    long loop(wstring& w, long i);
    
    void removeepsilon();
    void addrule(Au_arc*);
    void merge(Au_state*);
    
    Au_arc* build(Au_automatons* g, wstring& token, uchar type, Au_state* common, bool nega);
    Au_state* build(Au_automatons* g, long i,vector<wstring>& tokens, vector<aut_actions>& types, Au_state* common);
};

class Au_state_final : public Au_state {
public:
    
    long rule;
    
    Au_state_final(long r) {
        rule=r;
        status=an_rule;
    }

    long idx() {return rule;}

};

class Au_automaton {
public:
    
    Au_state* first;
    
    Au_automaton() {
        first=NULL;
    }

    Au_automaton(string rgx);

    
    bool get(wstring& w, hmap<long,bool>& rules);
    bool match(string& w);
    bool match(wstring& w);
    bool search(wstring& w);
    long find(wstring& w);
    long find(wstring& w, long i);


    bool search(wstring& w, long& first, long& last, long init = 0);
    bool search(string& w, long& first, long& last, long init = 0);
    bool searchc(string& w, long& first, long& last, long& firstc, long init = 0);
    bool searchraw(string& w, long& first, long& last, long init = 0);

    bool searchlast(wstring& w, long& first, long& last, long init = 0);
    bool searchlast(string& w, long& first, long& last, long init = 0);
    bool searchlastc(string& w, long& first, long& last, long& firstc, long init = 0);
    bool searchlastraw(string& w, long& first, long& last, long init = 0);

    bool bytesearch(wstring& w, long& first, long& last);
    void bytesearchall(wstring& w, vector<long>& res);

    void searchall(wstring& w, vector<long>& res, long init = 0);
    void searchall(string& w, vector<long>& res, long init = 0);
    void searchallraw(string& w, vector<long>& res, long init = 0);
    
    void find(string& w, string& sep, vector<long>& res);
    void find(wstring& w, wstring& sep, vector<long>& res);
    virtual bool parse(wstring& rgx, Au_automatons* automatons=NULL);
    
};


class Au_automatons {
public:
    VECTE<Au_state*> states;
    VECTE<Au_arc*> arcs;
    
    Au_state* state() {
        Au_state* s=new Au_state;
        states.push_back(s);
        return s;
    }
    
    Au_state* statefinal(long r) {
        Au_state_final* s=new Au_state_final(r);
        states.push_back(s);
        return s;
    }
    
    Au_arc* arc(Au_any* a, Au_state* s=NULL) {
        Au_arc* ac=new Au_arc(a);
        arcs.push_back(ac);
        if (s==NULL)
            ac->state=state();
        else
            ac->state=s;
        return ac;
    }
    
    void clean() {
        states.wipe();
        arcs.wipe();
    }

    void clean(long s, long a) {
        long i,ii;
        //We delete the states marked for destruction...
        for (i=s;i<states.size();i++) {
            if (states.vecteur[i] != NULL && states.vecteur[i]->mark == an_remove) {
                delete states.vecteur[i];
                states.vecteur[i]=NULL;
            }
        }

        
        //Compacting
        //We remove the NULL...
        for (i=s;i<states.size();i++) {
            if (states.vecteur[i]==NULL) {
                ii=i;
                while (ii<states.last && states.vecteur[ii]==NULL) ii++;
                if (ii==states.last) {
                    states.last=i;
                    break;
                }
                states.vecteur[i]=states.vecteur[ii];
                states.vecteur[ii]=NULL;
            }
        }

        //We delete the arcs marked for destruction...
        for (i=a;i<arcs.size();i++) {
            if (arcs.vecteur[i] != NULL && arcs.vecteur[i]->mark == an_remove) {
                delete arcs.vecteur[i];
                arcs.vecteur[i]=NULL;
            }
        }
        
        //Compacting
        //We remove the NULL...
        for (i=a;i<arcs.size();i++) {
            if (arcs.vecteur[i]==NULL) {
                ii=i;
                while (ii<arcs.last && arcs.vecteur[ii]==NULL) ii++;
                if (ii==arcs.last) {
                    arcs.last=i;
                    break;
                }
                arcs.vecteur[i]=arcs.vecteur[ii];
                arcs.vecteur[ii]=NULL;
            }
        }
    }

    void clearmarks() {
        long i;
        for (i=0;i<states.last;i++) {
            if (states.vecteur[i]!=NULL)
                states.vecteur[i]->mark=false;
        }
        for (i=0;i<arcs.last;i++) {
            if (arcs.vecteur[i] != NULL)
                arcs.vecteur[i]->mark=false;
        }
    }
    
    void clear(long s, long a) {
        long i;
        for (i=s;i<states.last;i++) {
            if (states.vecteur[i]!=NULL)
                states.vecteur[i]->mark=false;
        }
        for (i=a;i<arcs.last;i++) {
            if (arcs.vecteur[i] != NULL)
                arcs.vecteur[i]->mark=false;
        }
    }
    
    void boundaries(long& s, long& a) {
        s=states.size();
        a=arcs.size();
    }
    
    ~Au_automatons() {
        states.wipe();
        arcs.wipe();
    }
};

class Au_automate : public Au_automaton {
public:
    Au_automatons garbage;
    
    Au_automate() {
        first=NULL;
    }
    
    Au_automate(string rgx);
    Au_automate(wstring& rgx);

    bool compile(wstring& rgx) {
        return parse(rgx, &garbage);
    }

    bool compiling(wstring& rgx,long feature);
};

//------------------------------------------------------------
//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamguregularexpression;
//This typedef defines a type "regularexpressionMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguregularexpression::*regularexpressionMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------

class Tamguregular : public TamguObject {
public:
    
    wstring reg;


    Tamguregular(TamguGlobal* g = NULL, Tamgu* parent = NULL) : TamguObject(g, parent) {
        //Do not forget your variable initialisation
    }
  
    string String() {
        string re;
        s_unicode_to_utf8(re, reg);
        return re;
    }
    
    wstring UString() {
        return reg;
    }
    

    bool isRegular() {
        return true;
    }

};

class Tamguregularexpression : public Tamguregular {
public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<regularexpressionMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;
    
    static short idtype;
    
    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    Au_automate* automate;
    //---------------------------------------------------------------------------------------------------------------------
    Tamguregularexpression(TamguGlobal* g = NULL, Tamgu* parent = NULL) : Tamguregular(g, parent) {
        //Do not forget your variable initialisation
        automate = NULL;
    }
    
    Tamguregularexpression(string& t, Au_automate* a, TamguGlobal* g = NULL, Tamgu* parent = NULL) : Tamguregular(g, parent) {
        //Do not forget your variable initialisation
        automate = a;
        s_utf8_to_unicode(reg, USTR(t), t.size());
    }

    //----------------------------------------------------------------------------------------------------------------------
    short Type() {
        return Tamguregularexpression::idtype;
    }
    
    

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
        return "Tamguregularexpression";
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
    
    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        if (exported.find(n) != exported.end())
            return true;
        return false;
    }
    
    Tamgu* Newinstance(short, Tamgu* f = NULL) {
        return new Tamguregularexpression;
    }
    
    static void AddMethod(TamguGlobal* g, string name, regularexpressionMethod func, unsigned long arity, string infos);
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
    
    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is a sample  of a function that could be implemented for your needs.
    
    Tamgu* MethodMatch(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodCompile(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    
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
    
    Tamgu* Put(Tamgu* idx, Tamgu* rgx, short idthread);
    
    Tamgu* in(Tamgu* context, Tamgu* a, short idthread);
    
    Tamgu* same(Tamgu* a);
    Tamgu* different(Tamgu* a);

    void searchall(string& txt, vector<long>&);
    void searchall(wstring& txt, vector<long>&);
    bool search(wstring& w, long& f, long& l, long init =0);
    bool search(string& w, long& f, long& l, long init = 0);
    void searchall(string& txt, vector<long>&, long init);
    void searchall(wstring& txt, vector<long>&, long init);
    bool searchlast(wstring& w, long& first, long& last, long init = 0);
    bool searchlast(string& w, long& first, long& last, long init = 0);

};

class Tamguregularexpressionconstant : public Tamguregularexpression {
public:

    Tamguregularexpressionconstant(string& t, Au_automate* a, TamguGlobal* g = NULL, Tamgu* parent = NULL) : Tamguregularexpression(t, a, g, parent) {}
    
    void Release() {}
    void Resetreference(short) {}
    void Setreference(short) {}
    void Setreference() {}

};

#ifdef Tamgu_REGEX

//------------------------------------------------------------
//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamguposixregularexpression;
//This typedef defines a type "regularexpressionMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguposixregularexpression::*posixregularexpressionMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------

class Tamguposixregularexpression : public Tamguregular {
public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<posixregularexpressionMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;
    
    static short idtype;
    
    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...

    wregex* wpattern;
    regex* pattern;

    //---------------------------------------------------------------------------------------------------------------------
    Tamguposixregularexpression(TamguGlobal* g = NULL, Tamgu* parent = NULL) : Tamguregular(g, parent) {
        //Do not forget your variable initialisation
        pattern = NULL;
    }
    
    Tamguposixregularexpression(string& t, regex* a, wregex* wa, TamguGlobal* g = NULL, Tamgu* parent = NULL) : Tamguregular(g, parent) {
        //Do not forget your variable initialisation
        s_utf8_to_unicode(reg, USTR(t), t.size());
        pattern = a;
        wpattern = wa;
    }
    
    //----------------------------------------------------------------------------------------------------------------------
    
    short Type() {
        return Tamguposixregularexpression::idtype;
    }
    
    string Typename() {
        return "Tamguposixregularexpression";
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
    
    void searchall(string& txt, vector<long>&);
    void searchall(wstring& txt, vector<long>&);
    void searchall(string& txt, vector<long>&, long init);
    void searchall(wstring& txt, vector<long>&, long init);

    bool search(wstring& w, long& f, long& l, long init =0);
    bool search(string& w, long& f, long& l, long init = 0);

    bool searchlast(wstring& w, long& first, long& last, long init = 0);
    bool searchlast(string& w, long& first, long& last, long init = 0);
    
    wstring wreplace(wstring& w, wstring& rep) {
        return  regex_replace(w, *wpattern, rep);
    }
    
    string replace(string& w, string& rep) {
        return  regex_replace(w, *pattern, rep);
    }
    
    Tamgu* splitting(Tamgu* contextual, wstring& w);
    Tamgu* splitting(Tamgu* contextual, string& w);
    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        if (exported.find(n) != exported.end())
            return true;
        return false;
    }
    
    Tamgu* Newinstance(short, Tamgu* f = NULL) {
        return new Tamguposixregularexpression;
    }
    
    static void AddMethod(TamguGlobal* g, string name, posixregularexpressionMethod func, unsigned long arity, string infos);
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
    
    
    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is a sample  of a function that could be implemented for your needs.
    
    Tamgu* MethodMatch(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodCompile(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    
    
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
    
    Tamgu* Put(Tamgu* idx, Tamgu* rgx, short idthread);
    
    Tamgu* in(Tamgu* context, Tamgu* a, short idthread);
    
    Tamgu* same(Tamgu* a);
    Tamgu* different(Tamgu* a);
    
};

class Tamguposixregularexpressionconstant : public Tamguposixregularexpression {
public:
    
    Tamguposixregularexpressionconstant(string& t, regex* a, wregex* wa, TamguGlobal* g = NULL, Tamgu* parent = NULL) : Tamguposixregularexpression(t, a, wa, g, parent) {}
    
    void Release() {}
    void Resetreference(short) {}
    void Setreference(short) {}
    void Setreference() {}

};

#endif

#endif
