/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguannotator.cxx
 Date       : 2018/01/25
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "predicate.h"
#include "tamguannotator.h"
#include "tamguuvector.h"
#include "tamguvector.h"
#include "tamguivector.h"
#include "tamgutransducer.h"
#include "x_tokenize.h"
#include "tamgusynode.h"
#include "tamgumapuu.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<annotatorMethod>  Tamguannotator::methods;

Exporting short Tamguannotator::idtype = 0;

#ifdef UNIX
#define swprintf_s swprintf
#endif

//------------------------------------------------------------------------------------------------------------------

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguannotator::AddMethod(TamguGlobal* global, string name, annotatorMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}




void Tamguannotator::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamguannotator::InitialisationModule(global,"");
}


bool Tamguannotator::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    
    
    //Each new object has a specific name, which will help recognize it in the code that will exploit annotator...
    Tamguannotator::idtype = global->Getid("annotator");
    
    //You declare your methods here:
    // Argument 1 is a call to global, which will record your method into Tamgu (탐구)
    // Argument 2 is the name of your command
    // Argument 3 links your command with its implementation into the object class
    // Argument 4 is a combination of P_... which defines how many parameters this function can accept.
    //Argument 5 is a description of this method.
    Tamguannotator::AddMethod(global, "_initial", &Tamguannotator::MethodCompile, P_ONE, "_initial(string rules): Compile rules and store them into the annotator.");
    Tamguannotator::AddMethod(global, "compile", &Tamguannotator::MethodCompile, P_ONE, "compile(string rules): Compile rules and store them into the annotator.");
    Tamguannotator::AddMethod(global, "compilelexicons", &Tamguannotator::MethodCompileLexicons, P_NONE, "compilelexicons(): Compile lexicons.");
    Tamguannotator::AddMethod(global, "parse", &Tamguannotator::MethodParse, P_ONE|P_TWO, "parse(ustring tokens,bool keeplex): apply rules to a ustring.");
    Tamguannotator::AddMethod(global, "tokenize", &Tamguannotator::MethodTokenize, P_ONE, "tokenize(ustring tokens): apply only lexicons to the string and returns the tokens.");
    Tamguannotator::AddMethod(global, "apply", &Tamguannotator::MethodApply, P_ONE|P_TWO, "apply(uvector tokens,bool keeplex): apply rules to a list of tokens.");
    Tamguannotator::AddMethod(global, "select", &Tamguannotator::MethodSelect, P_ONE, "select(uvector labels): select the rules, whose label is defined in labels.");
    Tamguannotator::AddMethod(global, "clear", &Tamguannotator::MethodClear, P_NONE, "clear(): clear the label selection.");
    Tamguannotator::AddMethod(global, "selection", &Tamguannotator::MethodSelection, P_NONE, "selection(): return label selection.");
    Tamguannotator::AddMethod(global, "lexicon", &Tamguannotator::MethodLexicon, P_ONE, "lexicon(transducer t): Add a basic dictionary to the process.");
    Tamguannotator::AddMethod(global, "checklabel", &Tamguannotator::MethodCheckLabel, P_ONE, "checklabel(ustring label): Check if a label belongs to the annotated text.");
    Tamguannotator::AddMethod(global, "checkword", &Tamguannotator::MethodCheckWord, P_ONE, "checkword(ustring wrd): Returns the list of annotations associated to 'wrd' or check if it exists.");
    Tamguannotator::AddMethod(global, "labels", &Tamguannotator::MethodLabels, P_NONE, "labels(): Return the list of all labels that were extracted.");
    Tamguannotator::AddMethod(global, "words", &Tamguannotator::MethodWords, P_NONE, "words(): Return the list of all words found in the text.");
    Tamguannotator::AddMethod(global, "word", &Tamguannotator::MethodWord, P_ONE, "word(ustring w): Check if a word belongs to the text.");
    Tamguannotator::AddMethod(global, "token", &Tamguannotator::MethodToken, P_NONE|P_ONE, "token(long idx): Return the token at idx or the current token...");
    Tamguannotator::AddMethod(global, "tokens", &Tamguannotator::MethodTokens, P_NONE, "tokens(): Return the list of tokens extracted from the text...");
    Tamguannotator::AddMethod(global, "spans", &Tamguannotator::MethodSpans, P_ONE, "spans(bool s): If 's' is true then spans are computed...");

    Tamguannotator::AddMethod(global, "dependencies", &Tamguannotator::MethodDependencies, P_NONE | P_ONE, "depedencies(bool clear): Map annotations into dependencies and synodes, when clear is true, clear the knowledge base.");

    
    //We need this code, in order to create new instances of our annotator object... DO NOT ALTER
    if (version != "") {
        global->newInstance[Tamguannotator::idtype] = new Tamguannotator(global);
        global->RecordCompatibilities(Tamguannotator::idtype);
    }
    
    return true;
}

Tamgu* Tamguannotator::Put(Tamgu* idx, Tamgu* kval, short idthread) {
    //If you want to put some element into your object
    if (kval->Type() != Tamguannotator::idtype)
        return globalTamgu->Returnerror("Wrong assignment: Expected an 'annotator' variable");

    Tamguannotator* ann = (Tamguannotator*)kval;
    
    if (ownlexicon && lexicon != NULL)
        lexicon->Resetreference(reference);
    
    lexicon = ann->lexicon;

    if (ann->ownlexicon) {
        lexicon->Setreference(reference);
        ownlexicon=true;
    }
    else
        ownlexicon=false;
    
    rules = ann->rules;
    rules->Setreference(reference);
    lexicons = ann->lexicons;
    function = ann->function;

    return aTRUE;
}

Tamgu* Tamguannotator::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    //if your object is called from within an expression...
    if (contextualpattern == aNULL)
        return this;

    Tamguuvector* vect= (Tamguuvector*)Selectauvector(contextualpattern);

    Locking _lock(this);
    if (idthread<=contexts.size())
        return this;
        
    vect->values=contexts[idthread].thewords;
    return vect;
}

//--------------------------------------------------------------------------------------------------
void An_context::clear(Tamguuvector* v) {
    idword=0;
    thewords=v->values;
    
    labels.clear();
    features.clear();
    wordlabels.clear();
    classes.clear();
    lemmaToword.clear();
    wordTolemma.clear();
    if (annotator->rules != NULL) {
        maxloops=annotator->rules->maxloops;
        counters=annotator->rules->maxloops;
    }
    else {
        maxloops=globalTamgu->gTheAnnotationRules->maxloops;
        counters=globalTamgu->gTheAnnotationRules->maxloops;
    }
    
    for (long i = 0; i< nodes.size(); i++)
        nodes[i]->Resetreference();
    nodes.clear();
}

void An_context::clean() {
    for (long i = 0; i< nodes.size(); i++)
        nodes[i]->Resetreference();
}

void An_context::set(An_context& c) {
    thewords=c.thewords;
    labels = c.labels;
    features = c.features;
    lemmaToword=c.lemmaToword;
    wordlabels.clear();
    classes.clear();
    wordTolemma = c.wordTolemma;

    nodes.clear();
    for (long i = 0; i< c.nodes.size(); i++) {
        nodes.push_back(c.nodes[i]);
        nodes[i]->Setreference();
    }

    if (annotator->rules != NULL) {
        maxloops=annotator->rules->maxloops;
        counters=annotator->rules->maxloops;
    }
    else {
        maxloops=globalTamgu->gTheAnnotationRules->maxloops;
        counters=globalTamgu->gTheAnnotationRules->maxloops;
    }
}

//--------------------------------------------------------------------------------------------------

bool An_rules::storerule(An_rule* r) {
    if (r->first->arcs.size()==0)
        return false;

    An_arc* ar=r->first->arcs[0];
    wstring w=ar->atom->value();

    if (r->lexicon==true) {
        if (r->first->arcs.size()!=1)
            return false;
        
        lexiconpresent = true;
        //If there are more arcs, then it is a constraint...
        if (ar->state!=NULL && ar->state->arcs.size()) {
            lexconstraints[r->category][w] = ar->state;
            return true;
        }
            
        switch(ar->atom->type()) {
            case ant_token:
            case ant_meta:
                lexicons[r->category].parse(w);
                return true;
            case ant_automaton: {
                if (w.find(L" ")!=-1) {
                    //In this case, we have a multiword expression, to parse it would not make any sense...
                    //We store it in our multiwords lexicon...
                    wstring feat=L"\t+";
                    feat+=r->category;
                    multiwords.push_back(w);
                    multiwords.push_back(feat);
                }
                lexicons[r->category].parse(w);
                return true;
            }
        }
        lexarcs[r->category].push_back(ar);
        return true;
    }
    
    switch(ar->atom->type()) {
        case ant_token:
        case ant_meta:
        case ant_automaton:
            break;
        default:
            starts.push_back(r);
            return true;
    }

    long i;
    vector<wstring> wrds;
    wrds.push_back(w);
    for (i=1; i< r->first->arcs.last;i++) {
        w=r->first->arcs[i]->atom->value();
        switch(r->first->arcs[i]->atom->type()) {
            case ant_token:
            case ant_meta:
            case ant_automaton:
                wrds.push_back(w);
                break;
            default:
                starts.push_back(r);
                return true;
        }
    }
    
    for (i=0;i<wrds.size();i++) {
        //this rule starts with a lexicon arc, we store this arc into the lexiconrulecheck automaton
        //lexiconrulecheck merges all these specific automata into one...
        //We will use it to access lexicon rules through an automaton...
        //The rule is still stored in starts, and its index is the actual of starts now...
        lexiconrulecheck.compiling(wrds[i],starts.size());
    }

    r->islexiconrule=true; //this rule will need the entry to have been cleared against lexiconrulecheck to be triggered
    starts.push_back(r);
    return true;
}

//--------------------------------------------------------------------------------------------------
bool An_arc::scanend() {
    if (marque)
        return state->isend();
    
    marque=true;
    return state->scanend();
}

//we explore the states and find out whether epsilon arcs connect to end state
//in that case, we modify the status of the current node, as it can be an end as well
bool An_state::scanend() {
    if (marque)
        return isend();
    
    if (!arcs.last) {
        status |= an_end;
        return true;
    }
    
    marque=true;
    An_arc* a;
    for (long i=0;i<arcs.last;i++) {
        a=arcs[i];
        if (a->scanend() && a->atom->type() == ant_epsilon)
            status|=an_end;
    }
    return isend();
}

void An_arc::addtotail(An_state* s) {
    if (marque)
        return;
    
    marque=true;

    
    if (state->arcs.last == 0)
        state=s;
    else
        state->addtotail(s);
}

void An_state::addtotail(An_state* s) {
    if (marque)
        return;
    marque=true;
    for (long i=0;i<arcs.last;i++)
        arcs[i]->addtotail(s);

    if (arcs.last && isfinal())
        //in that case, we need to provide an epsilon jump to s...
        arcs.push_back(globalTamgu->gTheAnnotationRules->newarc(new An_epsilon,s));
}

bool An_rules::compare(wstring& chr, wstring& action, An_context* context) {
    if (lexconstraints.find(action) != lexconstraints.end() && lexconstraints[action].find(chr) != lexconstraints[action].end())
        return lexconstraints[action][chr]->apply(chr, context);
    
    if (lexicons.find(action) != lexicons.end()) {
        if (lexicons[action].match(chr))
            return true;
    }
    
    //we will try with the lexarcs...
    if (lexarcs.find(action) != lexarcs.end()) {
        for (long i=0; i< lexarcs[action].size(); i++) {
            if (lexarcs[action][i]->atom->compare(chr,context)==1)
                return true;
        }
    }
    return false;
}

//we cehck if "action" is a lemma or a word in the text
char An_token::check(An_context* context) {
    //we can also check if the word has a lemma...
    if (context->wordTolemma.find(action) == context->wordTolemma.end())
        if (context->lemmaToword.find(action) == context->lemmaToword.end())
            return false;
    
    return true;
}


char An_token::compare(wstring& chr, An_context* context) {
    if (action==chr)
        return true;
    
    //we can also check if the word has a lemma...
    if (context->wordTolemma.find(chr) != context->wordTolemma.end()) {
        for (long i = 0; i< context->wordTolemma[chr].size();i++) {
            if (action == context->wordTolemma[chr][i])
                return true;
        }
    }

    return false;
}

//We check if our label has been found somewhere in the text
char An_label::check(An_context* context) {
    if (context->labels.find(action)!=context->labels.end())
            return true;
    return false;
}

char An_label::compare(wstring& chr,An_context* context) {
    //We need to compare this word against our lexicon...
    if (context->annotator->compare(chr,action, context))
        return true;
    
    bool act=false;
    if (context->labels.find(action)!=context->labels.end()) {
        if (context->labels[action].find(chr) != context->labels[action].end())
            return true;
        act = true;
    }
    
    if (context->wordTolemma.find(chr) != context->wordTolemma.end()) {
        wstring lem;
        for (long i = 0; i< context->wordTolemma[chr].size();i++) {
            lem = context->wordTolemma[chr][i];
            if (context->annotator->compare(lem,action, context))
                return true;

            if (act) {
                if (context->labels[action].find(lem) !=  context->labels[action].end())
                    return true;
            }
        }
    }

    return false;
}

//If one is false, it is an error
char An_andlabels::check(An_context* context) {
    //We need to compare this word against our lexicon...
    for (long i=0;i<actions.size(); i++) {
        if (context->labels.find(actions[i])==context->labels.end()) {
            if (negations[i]) //if we have a negation then it is still ok for this element
                continue;
            return false;
        }
        if (negations[i])
            return false;
    }
    return true;
}


//If one is false, it is an error
char An_andlabels::compare(wstring& chr,An_context* context) {
    //We need to compare this word against our lexicon...
    for (long i=0;i<actions.size(); i++) {
        wstring& action=actions[i];
        //Either it is part of the lexicon with this specific label
        if (!context->annotator->compare(chr,action, context)) {
            //or we try to find it among the other labels...
            if (context->labels.find(action)==context->labels.end() || context->labels[action].find(chr) == context->labels[action].end()) {
                if (negations[i]) //if we have a negation then it is still ok for this element
                    continue;
                return false;
            }
        }
        if (negations[i])
            return false;
    }
    return true;
}

//if one is true, it is ok
char An_orlabels::check(An_context* context) {
    //We need to compare this word against our lexicon...
    for (long i=0;i<actions.size(); i++) {
        if (context->labels.find(actions[i])!=context->labels.end()) {
            if (negations[i]) //this is then false...
                continue;
            return true;
        }
        if (negations[i])
            return true;
    }
    return false;
}


//if one is true, it is ok
char An_orlabels::compare(wstring& chr,An_context* context) {
    //We need to compare this word against our lexicon...
    for (long i=0;i<actions.size(); i++) {
        wstring& action=actions[i];
        if (!context->annotator->compare(chr,action, context)) {
            if (context->labels.find(action)!=context->labels.end() && context->labels[action].find(chr) != context->labels[action].end()) {
                if (negations[i]) //this is then false...
                    continue;
                return true;
            }
        }
        if (negations[i])
            return true;
    }
    return false;
}

char An_lemma::check(An_context* context) {
    if (context->lemmaToword.find(action)==context->lemmaToword.end())
        return false;
    return true;
}


char An_lemma::compare(wstring& chr,An_context* context) {
    if (context->lemmaToword.find(action)==context->lemmaToword.end())
        return false;
    if (context->lemmaToword[action].find(chr)==context->lemmaToword[action].end())
        return false;
        
    wstring lchr=s_to_lower(chr);
    if (context->lemmaToword[action].find(lchr)==context->lemmaToword[action].end())
        return false;

    return true;
}

An_automaton::An_automaton(string reg) {
    action=new Au_automaton;
    s_utf8_to_unicode(reference, USTR(reg), reg.size());
    if (!action->parse(reference)) {
        delete action;
        action=NULL;
        reference=L"";
    }
}

char An_automaton::compare(wstring& chr,An_context* context) {
    if (action==NULL)
        return false;
    return action->match(chr);
}

An_automaton::~An_automaton() {
    if (action!=NULL)
        delete action;
}

#ifdef Tamgu_REGEX
An_regex::An_regex(wstring& rgx) {
    reference=rgx;
    action=new wregex(reference);
}

char An_regex::compare(wstring& chr,An_context* context) {
    if (action==NULL)
        return false;
    return regex_match(chr, *action);

}

An_regex::~An_regex() {
    if (action!=NULL)
        delete action;
}

#endif

//In this case, we can have only one parameter, the context
char An_call::check(An_context* context) {
    TamguCallFunction localcall(call);
    localcall.arguments.vecteur[0]=context->annotator;
    Tamgu* ret=localcall.Eval(aNULL, aNULL, context->idthread);
    bool res=ret->Boolean();
    ret->Release();
    return res;
}


char An_call::compare(wstring& chr,An_context* context) {
    TamguCallFunction localcall(call);
    Tamguustring* ustr=globalTamgu->Providewithustring(chr);
    ustr->Setreference();
    localcall.arguments.vecteur[0]=ustr;
    localcall.arguments.vecteur[1]=context->annotator;
    Tamgu* ret=localcall.Eval(aNULL, aNULL, context->idthread);
    bool res=ret->Boolean();
    ret->Release();
    ustr->Resetreference();
    return res;
}

//--------------------------------------------------------------------------
void An_state::setmax(short m) {
    idmax=globalTamgu->gTheAnnotationRules->maxloops.size();
    globalTamgu->gTheAnnotationRules->maxloops.push_back(m);
}

//applying rules globally...
bool An_arc::apply(An_context* context) {
    switch(atom->check(context)) {
        case 0:
            if ((status&an_negation)==an_negation)
                return state->apply(context);
            return false;
        case 1:
            if ((status&an_negation)==an_negation)
                return false;
            return state->apply(context);
        case 2:
            if ((status&an_negation)==an_negation)
                return false;
            return state->apply(context);
    }
    return false;
}



bool An_state::apply(An_context* context) {
    if (idmax!=-1) {
        if (context->check(idmax)==false)
            return -1;
    }
    for (long l=0;l<arcs.last;l++) {
        if (arcs[l]->apply(context))
            return true;
    }
    
    if ((status&an_end)==an_end)
        return true;
    
    return false;
}

//applying rules to a vector...
long An_arc::apply(vector<wstring>& words, long i,An_context* context) {
    if (i==words.size())
        return -1;

    long ret=-1;
    switch(atom->compare(words[i],context)) {
        case 0:
        if ((status&an_negation)==an_negation) {
            context->idword=i+1;
            ret=state->apply(words,i+1,context);
            context->idword=i;
        }
        return ret;
        case 1:
        if ((status&an_negation)==an_negation)
            return -1;
        context->idword=i+1;
        ret=state->apply(words,i+1,context);
        context->idword=i;
        return ret;
        case 2:
        if ((status&an_negation)==an_negation)
            return -1;
        return state->apply(words,i,context);
    }
    return -1;
}



long An_state::apply(vector<wstring>& words, long i,An_context* context) {
    if (idmax!=-1) {
        if (context->check(idmax)==false)
            return -1;
    }
    long ret;
    for (long l=0;l<arcs.last;l++) {
        ret=arcs[l]->apply(words,i,context);
        if (ret!=-1)
            return ret;
    }
    
    if ((status&an_end)==an_end)
        return i;
        
    return -1;
}

//applying rules to a vector...
long An_arc::apply(vector<wstring>& words, long i, vector<long>& kept,An_context* context) {
    if (i==words.size())
        return -1;
        
    long ret=0;
    switch(atom->compare(words[i],context)) {
        case 0:
        if ((status&an_negation)==an_negation) {
            context->idword=i+1;
            ret=state->apply(words,i+1,kept,context);
            context->idword=i;
        }
        return -1;
        case 1:
        if ((status&an_negation)==an_negation)
            return -1;
        context->idword=i+1;
        ret=state->apply(words,i+1,kept,context);
        context->idword=i;
        break;
        case 2:
        ret=state->apply(words,i,kept,context);
        break;
    }

    if (ret!=-1) {
        if ((status&an_remove)==0)
            kept.push_back(i);
    }
    
    return ret;
}

long An_state::apply(vector<wstring>& words, long i, vector<long>& kept,An_context* context) {
    if (idmax!=-1) {
        if (context->check(idmax)==false)
            return -1;
    }

    long ret;
    for (long l=0;l<arcs.last;l++) {
        ret=arcs[l]->apply(words,i, kept,context);
        if (ret!=-1)
            return ret;
    }
    
    if ((status&an_end)==an_end)
        return i;

    return -1;
}

//applying rules to a vector...
bool An_arc::apply(wstring& word, An_context* context) {
    switch(atom->compare(word,context)) {
        case 0:
            if ((status&an_negation)==an_negation)
                return state->apply(word,context);
            return false;
        case 1:
            if ((status&an_negation)==an_negation)
                return false;
            return state->apply(word,context);
        case 2:
            return state->apply(word,context);
    }
    
    return false;
}

bool An_state::apply(wstring& word, An_context* context) {
    if (idmax!=-1) {
        if (context->check(idmax)==false)
            return false;
    }
    
    for (long l=0;l<arcs.last;l++) {
        if (arcs[l]->apply(word,context))
            return true;
    }
    
    if ((status&an_end)==an_end)
        return true;
    
    return false;
}

bool An_rule::applyclass(An_context* context) {
 
    return first->apply(context);
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
/*
 Different cases:
 
 Tokenization can be handled in different ways:
 
 1) Through a lexicon
 2) Through x_wtokenize
 
 Parse with a Lexicon
 The lexicon can have been loaded with "lexicon(transducer t)" (loadedlexicon is then true).
 However, if none was loaded, then we create one on the fly.
 The multiwords expressions are then merged into this lexicon, which will be able to recognize multiword expressions provided by the user
 at parse time.
 
 Parse with x_wtokenize:
 If not external lexicon was provided (loadedlexicon is then false) and no multiword expressions were provided (lexicon->automaton == NULL)
 then we will use x_wtokenize to do the tokenization (non linguistic parsing). In that case, we set compiledlexicon to 3.
 
 */

Tamgu* Tamguannotator::lexiconcompiling() {
    if (compiledlexicon)
        return aFALSE;

    long i;
    //First case, lexicon is NULL but multiwords is not...
    if (lexicon == NULL) {
        lexicon = new Tamgutransducer(globalTamgu);
        lexicon->Setreference(reference);
        compiledlexicon = 2;
        ownlexicon = true;
    }

    if (globalTamgu->gTheAnnotationRules != NULL && globalTamgu->gTheAnnotationRules->multiwords.size()) {
        if (!lexicon->modified) {
            for (i=0;i<globalTamgu->gTheAnnotationRules->multiwords.size();i+=2)
                lexicon->compilergx(globalTamgu->gTheAnnotationRules->multiwords[i],globalTamgu->gTheAnnotationRules->multiwords[i+1]);
            
            lexicon->modified = true;
        }
        compiledlexicon = true;
        loadedlexicon = true;
    }
    
    if (globalTamgu->gTheAnnotationRules != rules && rules->multiwords.size()) {
        //We merge it into our main lexicon
        for (i=0;i<rules->multiwords.size();i+=2)
            lexicon->compilergx(rules->multiwords[i],rules->multiwords[i+1]);
        
        rules->multiwords.clear();
        compiledlexicon = true;
        loadedlexicon = true;
    }

    if (!loadedlexicon || lexicon->automaton == NULL)
        compiledlexicon = 3;
    
    if (rules != NULL && rules->lexiconpresent) {
        checklexicon = 1;
        if (lexicons != rules && lexicons->lexiconpresent)
            checklexicon = 3;
    }
    else {
        if (lexicons != rules && lexicons->lexiconpresent)
            checklexicon = 2;
    }
    
    return aTRUE;
}

Tamgu* Tamguannotator::MethodCompileLexicons(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Initrules();
    return lexiconcompiling();
}


Tamgu* Tamguannotator::MethodCompile(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Initrules();
    
    string code = callfunc->Evaluate(0,contextualpattern,idthread)->String();
    An_rules* old = globalTamgu->gTheAnnotationRules;
    
    if (rules == NULL || rules == globalTamgu->gTheAnnotationRules) {
        rules = new An_rules(globalTamgu);
        rules->Setreference(reference);
    }
    
    globalTamgu->gTheAnnotationRules = rules;
    
    rules = globalTamgu->EvaluateRules(code, idthread);
    if (rules == NULL)
        return globalTamgu->Errorobject(idthread);
    
    if (old !=  NULL)
        globalTamgu->gTheAnnotationRules = old;
    
    if (rules->lexiconpresent) {
        checklexicon = 1;
        if (lexicons != rules && lexicons->lexiconpresent)
            checklexicon = 3;
    }
    else {
        if (lexicons != NULL && lexicons != rules && lexicons->lexiconpresent)
            checklexicon = 2;
    }
    
    return aTRUE;
}

Tamgu* Tamguannotator::MethodLexicon(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* tlexicon = callfunc->Evaluate(0,contextualpattern,idthread);
    if (tlexicon->Type()== Tamgutransducer::idtype) {
        if (ownlexicon && lexicon != NULL)
            lexicon->Resetreference(reference);
        lexicon= (Tamgutransducer*)tlexicon;
        //it can be modified only once
        loadedlexicon = true;
        return aTRUE;
    }
    
    return globalTamgu->Returnerror("Wrong argument. Expecting a 'transducer' object.");
}

Tamgu* Tamguannotator::MethodCheckLabel(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    wstring w=callfunc->Evaluate(0,contextualpattern,idthread)->UString();
    if (contexts[idthread].labels.find(w)!=contexts[idthread].labels.end())
        return aTRUE;
    return aFALSE;
}


Tamgu* Tamguannotator::MethodCheckWord(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    
    wstring w=callfunc->Evaluate(0,contextualpattern,idthread)->UString();
    if (contextualpattern->isContainer()) {
        Tamguuvector*  uvect=(Tamguuvector*)Selectauvector(contextualpattern);
        for (const auto& e: contexts[idthread].labels) {
            if (e.second.find(w) == e.second.end())
                uvect->storevalue(e.first);
        }
        return uvect;
    }
    
    if (contexts[idthread].wordTolemma.find(w) != contexts[idthread].wordTolemma.end())
        return aTRUE;
    return aFALSE;
}

Tamgu* Tamguannotator::MethodLabels(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    Tamgu* vect=Selectauvector(contextualpattern);
    for (const auto& a: contexts[idthread].labels)
        vect->storevalue(a.first);
    return vect;
}


Tamgu* Tamguannotator::MethodWords(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    
    Tamgu* vect=Selectauvector(contextualpattern);
    for (const auto& l: contexts[idthread].lemmaToword) {
        vect->storevalue(l.first);
        for (const auto& w: l.second)
            vect->storevalue(w.first);
    }
    return vect;
}

Tamgu* Tamguannotator::MethodWord(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);

    wstring w=callfunc->Evaluate(0,contextualpattern,idthread)->UString();

    if (contexts[idthread].wordTolemma.find(w) != contexts[idthread].wordTolemma.end())
        return aTRUE;

    if (contexts[idthread].lemmaToword.find(w) != contexts[idthread].lemmaToword.end())
        return aTRUE;

    return aFALSE;
}

Tamgu* Tamguannotator::MethodTokens(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);
    
    Tamguuvector* vect= (Tamguuvector*)Selectauvector(contextualpattern);
    vect->values=contexts[idthread].thewords;
    return vect;
}

Tamgu* Tamguannotator::MethodToken(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(this);

    if (contexts[idthread].idword >= contexts[idthread].thewords.size())
        return aNULL;

    if (callfunc->Size()==0) {
        if (contextualpattern->isNumber())
            return globalTamgu->ProvideConstint(contexts[idthread].idword);
            
        return globalTamgu->Provideustring(contexts[idthread].thewords[contexts[idthread].idword]);
    }
    
    long idx=callfunc->Evaluate(0,contextualpattern,idthread)->Integer();
    
    if (idx<0 || idx >=contexts[idthread].thewords.size())
        return aNULL;
    
    if (contextualpattern->isNumber())
        return globalTamgu->ProvideConstint(idx);

    return globalTamgu->Provideustring(contexts[idthread].thewords[idx]);
}

Tamgu* Tamguannotator::MethodSelection(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* uvect=Selectauvector(contextualpattern);
    for (const auto& s: selections)
        uvect->storevalue(s.first);
    return uvect;
}

//---------------------------------------------------------------------------------------------------
//We transform each word into a synode, with labels both as dependencies and features...
Tamgu* Tamguannotator::MethodDependencies(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //first for each token, we crate a synode...
    Tamgusynode* syn;
    Tamgumapuu feats;
    An_context& cnt = contexts[idthread];
    vector<wstring>& wrds = cnt.thewords;
    long sz = wrds.size();
    long l;
    wchar_t buff[]={'l','e','m','m','a','0',0};
    wchar_t idx[] = {0,0,0,0,0};
    long i;
    
    if (cnt.nodes.size() == 0) {
        for (i = 0; i < sz; i++) {
            syn = new Tamgusynode(i);
            cnt.nodes.push_back(syn);
            //We use the different labels as feature...
            //The first feature is the lemma
            feats.values.clear();
            swprintf_s(idx,4,L"%d",i);
            feats.values[L"idx"] = idx;
            feats.values[L"surface"] = wrds[i];
            if (cnt.wordTolemma.find(wrds[i]) != cnt.wordTolemma.end()) {
                feats.values[L"lemma"] = cnt.wordTolemma[wrds[i]][0];
                for (l = 1; l <cnt.wordTolemma[wrds[i]].size(); l++) {
                    buff[5]=(wchar_t)(l+48);
                    feats.values[buff] =cnt.wordTolemma[wrds[i]][l];
                }
            }
            else
                feats.values[L"lemma"] = wrds[i];
            
            if (cnt.features.find(wrds[i]) != cnt.features.end()) {
                for (l=0; l < cnt.features[wrds[i]].size(); l++)
                    feats.values[cnt.features[wrds[i]][l]] = L"+";
            }
            syn->features = globalTamgu->Providemapss();
            syn->features->Put(aNULL, &feats, idthread);
            syn->Setreference();
        }
    }
    
    //We then transform the labels into dependencies...
    //First we flush the knowledge base
    bool clear=false;
    if (callfunc->Size()==1)
        clear=callfunc->Evaluate(0,contextualpattern,idthread)->Boolean();
    
    TamguDependency* dep;
    short idname;
    Tamguustring nm(L"");
    for (i = 0; i< cnt.classes.size(); i++) {
        idname  = globalTamgu->Getid(cnt.classes[i]);
        dep = new TamguDependency(NULL, aNULL, idname, 0);
        if (cnt.wordlabels[i].size() > 1) {
            Tamguvector* vect =  globalTamgu->Providevector();
            for (long u=0; u< cnt.wordlabels[i].size(); u++)
                vect->Push(cnt.nodes[cnt.wordlabels[i][u]]);
            dep->parameters.push_back(vect);
        }
        else
            dep->parameters.push_back(cnt.nodes[cnt.wordlabels[i][0]]);
        
        //We push it into our knowledge base...
        dep->Setreference();
        globalTamgu->StorePredicate(aNULLDECLARATION, dep, true);
    }
    
    return aTRUE;
}

//---------------------------------------------------------------------------------------------------
void Tamguannotator::Applylexicon(vector<wstring>& words, An_context* context) {
    wstring splitter=L"+";
    wstring tab=L"\t";

    wstring wrd;
    wstring lem;
    wstring wfeat;

    vector<wstring> features;
    long w;

    vector<wstring> reading;
    
    for (w=0; w< words.size();w++) {
        wrd = words[w];
        s_split(wrd, tab, reading);
        if (!reading.size())
            continue;
        
        wrd=reading[0];
        long sz=reading.size();
        for (long e = 1; e < sz; e+=2) {
            lem=reading[e];
            if (context->lemmaToword.find(lem) == context->lemmaToword.end() || context->lemmaToword[lem].find(wrd)==context->lemmaToword[lem].end()) {
                context->lemmaToword[lem][wrd]=true;
                if (lem != wrd) //if the two forms are the same, the test in An_token and An_label would be redundant...
                    context->wordTolemma[wrd].push_back(lem);
            }
            if ((e+1) < sz) {
                s_split(reading[e+1], splitter, features);
                for (long f=0;f<features.size();f++) {
                    wfeat=features[f];
                    context->labels[wfeat][wrd]=true;
                    context->features[wrd].push_back(wfeat);
                }
                features.clear();
            }
        }
        words[w] = wrd;
        reading.clear();
    }
    
    vector<string> readings;
    wstring line;
    
    for (w=0; w<words.size();w++) {
        wrd = words[w];
        
        lexicon->automaton->up(wrd, readings, 0, 0, context->idthread);
        if (readings.size()==0) {
            lem=s_to_lower(wrd);
            lexicon->automaton->up(lem, readings, 0, 0, context->idthread);
        }
        
        //Each value is separated from the other with a +
        for (long a=0;a<readings.size();a++) {
            sc_utf8_to_unicode(line, USTR(readings[a]), readings[a].size());
            long pos=line.find(L"\t");
            if (pos!=-1) {
                lem=line.substr(0,pos);
                if (context->lemmaToword.find(lem) == context->lemmaToword.end() || context->lemmaToword[lem].find(wrd)==context->lemmaToword[lem].end()) {
                    context->lemmaToword[lem][wrd]=true;
                    if (lem != wrd) //if the two forms are the same, the test in An_token and An_label would be redundant...
                        context->wordTolemma[wrd].push_back(lem);
                }
                
                line=line.substr(pos+1,line.size()-pos-1);
                s_split(line,splitter,features,false);
                for (long f=0;f<features.size();f++) {
                    wfeat=features[f];
                    context->labels[wfeat][wrd]=true;
                    context->features[wrd].push_back(wfeat);
                }
                features.clear();
            }
        }
    }
}

Tamgu* Tamguannotator::Apply(Tamguuvector* uvect, Tamgu* res, bool computelexicon, short idthread) {
    //We reset our analysis...
    //Each thread has its own context, which consists of state counters, labels and lemmas.
    An_context* context=NULL;
    {
        Locking _lock(this);
        if (idthread<=contexts.size()) {
            //we add the necessary elements...
            for (long i=contexts.size();i<=idthread;i++) {
                //it is initialized with a Tamguannotator and the corresponding idthread...
                An_context a(this,i);
                contexts.push_back(a);
            }
        }
        
        //we use the current thread context...
        context=&contexts[idthread];
        //which we clear...
        context->clear(uvect);
        uvect->Release();
    }

    
    if (lexicon != NULL && lexicon->automaton != NULL)
        Applylexicon(context->thewords, context);

    return Apply(context, res,computelexicon, idthread);
}
    
Tamgu* Tamguannotator::Apply(An_context* context,Tamgu* res, bool computelexicon, short idthread) {
    static vector<long> basevinter;
    static vector<long> basekept;
    static hmap<long,bool> basevalidrules;

    vector<long>* vinter = &basevinter;
    vector<long>* kept = &basekept;
    hmap<long,bool>* validrules = &basevalidrules;

    if (idthread) {
        vinter = new vector<long>;
        kept = new vector<long>;
        validrules = new hmap<long,bool>;
    }
    else {
        vinter->clear();
        kept->clear();
        validrules->clear();
    }
    
    vector<An_rule*>& vrules= rules->starts;
    long i=0;
    long current, r;
    bool found;
    bool lexiconrulecheck;
    An_rule* currentrule;
    long catrule=-1;
    vector<wstring>& tokens = context->thewords;

    while (i<tokens.size()) {
        found=false;
        //We check first if a lexicon rule (a rule starting with a lexical item) is available
        //If it is available, then validrules will contain the rule indexes of the rules that start with an element that is compatible with the current word...
        lexiconrulecheck=rules->lexiconrulecheck.get(tokens[i],*validrules);
        
        for (r=0; r<vrules.size();r++) {
            currentrule = vrules[r];
            if (currentrule->classing || currentrule->removing) {
                if (catrule==-1)
                    catrule=r;
                continue;
            }

            //We have two cases, either no lexicon rules was detected through lexiconrulecheck in gTheAnnotationRules, and it this case, we skip all lexicon rules...
            if (!lexiconrulecheck) {
                if (currentrule->islexiconrule) //we skip these rules, we already know that they cannot apply
                    continue;
            }
            else {//Or lexicon rules have been detected and we only keep the ones that have been stored in validrules...
                if (currentrule->islexiconrule && validrules->find(r) == validrules->end())
                    continue;
            }
            
            if (selections.size() && selections.find(currentrule->category)==selections.end())
                continue;
                
            if (currentrule->hasmax)
                context->clearcounters();
                
            if (!currentrule->hasremove) {
                current=currentrule->apply(tokens,i,context);
                if (current!=-1) {
                    vinter->clear();
                    if (!spans) {
                        res->storevalue(currentrule->category);
                        found = true;
                        for (;i<current;i++) {
                            if (computelexicon)
                                context->labels[currentrule->category][tokens[i]] = true;
                            vinter->push_back(i);
                        }
                        
                        context->classes.push_back(currentrule->category);
                        context->wordlabels.push_back(*vinter);
                        break;
                    }

                    Tamguvector* sub=globalTamgu->Providevector();
                    sub->storevalue(currentrule->category);
                    for (;i<current;i++) {
                        vinter->push_back(i);
                        sub->storevalue(i);
                        if (computelexicon)
                            context->labels[currentrule->category][tokens[i]] = true;
                    }
                    context->classes.push_back(currentrule->category);
                    context->wordlabels.push_back(*vinter);
                    res->Push(sub);
                    found=true;
                    break;
                }
                continue;
            }
            
            //Else we need to take into account which elements are kept...
            kept->clear();
            current=currentrule->apply(tokens,i, *kept,context);
            if (current!=-1) {
                vinter->clear();
                if (!spans) {
                    res->storevalue(currentrule->category);
                    found = true;
                    
                    for (i=kept->size()-1;i>=0; i--) {
                        if (computelexicon)
                            context->labels[currentrule->category][tokens[kept->at(i)]] = true;
                        vinter->push_back(i);
                    }
                    
                    context->classes.push_back(currentrule->category);
                    context->wordlabels.push_back(*vinter);

                    i=current;
                    break;
                }
                
                Tamguvector* sub=globalTamgu->Providevector();
                sub->storevalue(currentrule->category);
                for (i=kept->size()-1;i>=0; i--) {
                    vinter->push_back(i);
                    sub->storevalue(kept->at(i));
                    if (computelexicon)
                        context->labels[currentrule->category][tokens[kept->at(i)]] = true;
                }
                
                context->classes.push_back(currentrule->category);
                context->wordlabels.push_back(*vinter);

                res->Push(sub);
                i=current;
                found=true;
                break;
            }

            
        }
        if (!found)
            i++;

        if (globalTamgu->Error(idthread)) {
            if (idthread) {
                delete vinter;
                delete kept;
                delete validrules;
            }
            context->thewords.clear();
            return globalTamgu->Errorobject(idthread);
        }
    }

    if (catrule != -1) {
        for (r = catrule; r<vrules.size();r++) {
            currentrule = vrules[r];
            if (currentrule->classing || currentrule->removing) {
                //If the category has not been found so far, no need to apply the rule...
                if (currentrule->removing && context->labels.find(currentrule->category) == context->labels.end())
                    continue;
                
                if (currentrule->hasmax)
                    context->clearcounters();
                
                //this is a rule that can only apply to existing classes...
                if (currentrule->applyclass(context)) {
                    if (currentrule->classing) {
                        if (!spans) {
                            res->storevalue(currentrule->category);
                            continue;
                        }
                        
                        Tamguvector* sub=globalTamgu->Providevector();
                        sub->storevalue(currentrule->category);
                        sub->storevalue((long)0);
                        res->Push(sub);
                        continue;
                    }
                    
                    //We remove it...
                    context->labels.erase(currentrule->category);
                    Tamguint val((long)0);
                    long ei;
                    for (ei=context->classes.size()-1; ei>=0; ei--) {
                        if (context->classes[ei] == currentrule->category) {
                            context->classes.erase(context->classes.begin()+ei);
                            context->wordlabels.erase(context->wordlabels.begin()+ei);
                        }
                    }
                    
                    if (!spans) {
                        for (ei=res->Size()-1; ei>=0; ei--) {
                            if (res->getustring(ei) ==  currentrule->category) {
                                val.value=ei;
                                res->Pop(&val);
                            }
                        }
                        continue;
                    }
                    
                    for (ei=res->Size()-1; ei>=0; ei--) {
                        if (res->getvalue(ei)->getustring(0) ==  currentrule->category) {
                            val.value=ei;
                            res->Pop(&val);
                        }
                    }
                }
            }
        }
    }

    if (idthread) {
        delete vinter;
        delete kept;
        delete validrules;
    }

    return res;
}

static void Unique(vector<wstring>& values) {
    vector<wstring> vals;
    std::set<wstring> inter;
    
    long sz=values.size();
    for (int i = 0; i < sz; i++) {
        if (inter.insert(values[i]).second)
            vals.push_back(values[i]);
    }
    values = vals;
}

//Parse a string
Tamgu* Tamguannotator::MethodParse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Initrules();
    
    Tamgu* txt = callfunc->Evaluate(0,contextualpattern,idthread);
    uchar computelexicon=false;
    if (callfunc->Size()==2)
        computelexicon=callfunc->Evaluate(1,contextualpattern,idthread)->Boolean();
    
    Tamgu* res= Selectavector(contextualpattern);
    return Execution(res, txt, idthread, computelexicon);
}

Tamgu* Tamguannotator::MethodTokenize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Initrules();
    
    Tamgu* txt = callfunc->Evaluate(0,contextualpattern,idthread);
    Tamgu* res= Selectavector(contextualpattern);
    return Execution(res, txt, idthread, 2);
}

Tamgu* Tamguannotator::Execution(Tamgu* res, Tamgu* txt, short idthread, uchar computelexicon) {
    Tamguuvector vectoroftokens;
    vectoroftokens.reference=2;
    long i,pos,j, idx;
    Tamgu* v;
    Tamguvector* sub;
    Tamguivector* ivect;
    
    Tamgu* finalres;
    
    if (spans)
        finalres=globalTamgu->Providevector();
    else
        finalres=globalTamgu->Provideuvector();

    lexiconcompiling();
    
    //Two cases...
    //We have lexicons...
    if (compiledlexicon != 3) {
        //First case, lexicons is 0 but multiwords is not...
        
        //Now we parse...
        string words=txt->String();
        Tamguvector tokvect;
        lexicon->parse(words,&tokvect,0,0,true);
        
        if (function != NULL) {
            tokvect.Setreference(1);
            Setreference(1);
            TamguCallFunction2 localcall(function);
            localcall.arguments.push_back(&tokvect);
            localcall.arguments.push_back(this);
            Tamgu* ret=localcall.Eval(aNULL, aNULL, idthread);
            tokvect.Protect();
            Resetreference(1);
            if (ret->isVectorContainer() && ret != &tokvect) {
                //We clear tokvect...
                tokvect.Clear();
                for (long idx=0;idx<ret->Size();idx++)
                    tokvect.push(ret->getvalue(idx));
            }
        }
        
        //we have a structure here, where each element is a svector containing the surface, then lemmas then offsets...
        wstring w;
        if (computelexicon == 2) {
            //We only return the tokens in this case...
            for (i=0;i<tokvect.values.size();i++) {
                w = tokvect.values[i]->getustring(0);
                for (long e=1;e<tokvect.values[i]->Size()-2;e++) {
                    w += L"\t";
                    w += tokvect.values[i]->getustring(e);
                }
                res->storevalue(w);
            }
            return res;
        }
        
        for (i=0;i<tokvect.values.size();i++) {
            w = tokvect.values[i]->getustring(0);
            for (long e=1;e<tokvect.values[i]->Size()-2;e++) {
                w += L"\t";
                w += tokvect.values[i]->getustring(e);
            }
            vectoroftokens.values.push_back(w);
        }
        
        res=Apply(&vectoroftokens,res, computelexicon, idthread);
        
        long sz;
        for (i=0;i<res->Size();i++) {
            v=res->getvalue(i);
            if (!spans) {
                finalres->Push(v);
                continue;
            }
            //The first element is the label, we do not need to touch it...
            sub=globalTamgu->Providevector();
            sub->Push(v->getvalue(0));
            for (j=1;j<v->Size();j++) {
                ivect=globalTamgu->Provideivector();
                //For each index, we need to get the right positions in cpos...
                idx=v->getinteger(j); //the index of the token in uvect
                sz=tokvect.values[idx]->Size();
                ivect->values.push_back(tokvect.values[idx]->getinteger(sz-2));
                ivect->values.push_back(tokvect.values[idx]->getinteger(sz-1));
                sub->Push(ivect);
            }
            finalres->Push(sub);
        }
        tokvect.Clear();
        res->Release();
        if (!spans)
            Unique(((Tamguuvector*)finalres)->values);
        return finalres;
    }
    
    tokenizer_result<wstring> xr;
    wstring wrds=txt->UString();
    xr.setstack(&vectoroftokens.values);
    globalTamgu->tamgu_tokenizer.tokenize<wstring>(wrds,xr);
    res=Apply(&vectoroftokens,res, computelexicon, idthread);
    //res is a vector, which contains vectors in which the first value is the label and the other values the indexes...
    for (i=0;i<res->Size();i++) {
        v=res->getvalue(i);
        if (!spans) {
            finalres->Push(v);
            continue;
        }
        //The first element is the label, we do not need to touch it...
        sub=globalTamgu->Providevector();
        sub->Push(v->getvalue(0));
        for (j=1;j<v->Size();j++) {
            ivect=globalTamgu->Provideivector();
            //For each index, we need to get the right positions in cpos...
            idx=v->getinteger(j); //the index of the token in uvect
            pos=xr.cpos[idx]; //its position within the initial string
            ivect->values.push_back(pos);
            ivect->values.push_back(pos+vectoroftokens.values[idx].size());
            sub->Push(ivect);
        }
        finalres->Push(sub);
    }
    
    res->Release();
    
    if (!spans)
        Unique(((Tamguuvector*)finalres)->values);

    return finalres;
}

Tamgu* Tamguannotator::MethodApply(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Initrules();
    
    Tamgu* kvect = callfunc->Evaluate(0,contextualpattern,idthread);
    bool computelexicon=false;
    if (callfunc->Size()==2)
        computelexicon=callfunc->Evaluate(1,contextualpattern,idthread)->Boolean();

    Tamgu* res;

    if (kvect->Type() == Tamguannotator::idtype) {
        Tamguannotator* ann = (Tamguannotator*)kvect;
        An_context* context = NULL;
        {
            Locking _lock(this);
            if (idthread<=contexts.size()) {
                //we add the necessary elements...
                for (long i=contexts.size();i<=idthread;i++) {
                    //it is initialized with a Tamguannotator and the corresponding idthread...
                    An_context a(this,i);
                    contexts.push_back(a);
                }
            }
            
            //we use the current thread context...
            context=&contexts[idthread];
            //which we set, if it is different from the current one...
            if (ann != this)
                context->set(ann->contexts[idthread]);
        }

        if (spans) {
            res= Selectavector(contextualpattern);
            return Apply(context, res, computelexicon, idthread);
        }
        
        res = Selectauvector(contextualpattern);
        contextualpattern = Apply(context, res, computelexicon, idthread);
        Unique(((Tamguuvector*)res)->values);
        return contextualpattern;
    }
    
    if (!kvect->isVectorContainer())
        globalTamgu->Returnerror("Argument should be an uvector");
        
    if (kvect->Size() == 0)
        return aNULL;
    
    Tamguuvector* uvect;
    
    if (kvect->Type() != a_uvector) {
        uvect=new Tamguuvector;
        Tamgu* first = kvect->getvalue(0);
        if (first->isVectorContainer()) {
            // [[word,lemma,f1,f2..], [word, lemma, f1,f2]...]
            //In this case, the first element of each sub-vector is the surface form...
            //the second element must be the lemma and the rest are features...
            //If one element is not a vector, we simply keep it as it is... No lemma, no features...
            //We need a context then
            An_context* context = NULL;
            {
                Locking _lock(this);
                if (idthread<=contexts.size()) {
                    //we add the necessary elements...
                    for (long i=contexts.size();i<=idthread;i++) {
                        //it is initialized with a Tamguannotator and the corresponding idthread...
                        An_context a(this,i);
                        contexts.push_back(a);
                    }
                }
                
                //we use the current thread context...
                context=&contexts[idthread];
                //which we set, if it is different from the current one...
                //
                context->clear(uvect);
                //We will not need uvect anymore in this configuration...
                uvect->Release();
            }
            //We have three maps to fill in: lemmatToword, wordTolemma and labels...
            
            wstring wrd;
            wstring lemma;
            wstring feat;
            long sz,w;
            for (long i=0; i< kvect->Size(); i++) {
                Tamgu* subvect = kvect->getvalue(i);
                if (subvect->isVectorContainer()) {
                    sz = subvect->Size();
                    //The we need to investigate...
                    if (sz>0) {
                        //First the surface form
                        wrd = subvect->getustring(0);
                        //Which we store in words
                        context->thewords.push_back(wrd);
                        if (sz > 1) {
                            //the lemma
                            lemma = subvect->getustring(1);
                            if (context->lemmaToword.find(lemma) == context->lemmaToword.end() || context->lemmaToword[lemma].find(wrd)==context->lemmaToword[lemma].end()) {
                                context->lemmaToword[lemma][wrd]=true;
                                if (lemma != wrd) //if the two forms are the same, the test in An_token and An_label would be redundant...
                                    context->wordTolemma[wrd].push_back(lemma);
                            }
                            for (w=2; w<sz; w++) {
                                feat=subvect->getustring(w);
                                context->labels[feat][wrd]=true;
                            }
                        }
                    }
                }
                else {
                    wrd = subvect->UString();
                    context->thewords.push_back(wrd);
                }
            }
            
            if (spans) {
                res= Selectavector(contextualpattern);
                return Apply(context, res, computelexicon, idthread);
            }
            
            res = Selectauvector(contextualpattern);
            contextualpattern = Apply(context, res, computelexicon, idthread);
            Unique(((Tamguuvector*)res)->values);
            return contextualpattern;
        }

        first->Release();
        uvect->Put(kvect,aNULL,idthread);
    }
    else
        uvect=(Tamguuvector*)kvect;
    
    if (spans)
        res = Selectavector(contextualpattern);
    else
        res = Selectauvector(contextualpattern);
    
    if (spans)
        return Apply(uvect,res, computelexicon, idthread);
    
    contextualpattern = Apply(uvect,res, computelexicon, idthread);
    Unique(((Tamguuvector*)res)->values);
    return contextualpattern;
}

