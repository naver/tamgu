/*
 *  LispE
 *
 * Copyright 2020-present NAVER Corp.
 * The 3-Clause BSD License
 */
//  tokens.h (see tools.cxx for implementation)
//
//

#ifndef lispe_tokens_h
#define lispe_tokens_h


#include <string>
#include <vector>
#include <unordered_map>
#include "lispetools.h"
#include "vecter.h"

using std::u16string;
#define u_uchar char32_t
#define u_ustring std::u32string

using std::vector;
using std::unordered_map;
typedef enum {flagl_none = 0, flagl_negation = 1, flagl_skip = 2, flagl_fail = 4, flagl_start_fail = 8, flagl_blocking_gate = 16, flagl_tail = 32, flagl_action = 63, flagl_vectorized = 64, flagl_postpone = 128, flagl_seen = 256, flagl_visited = 512, flagl_added = 1024} tokeniser_flag;


typedef enum {actl_none, actl_char , actl_uppercase, actl_ckj, actl_space_and_carriage, actl_alphabetical,
    actl_lowercase, actl_digit, actl_greek, actl_non_breaking_space, actl_operator, actl_punctuation, actl_emoji, actl_emojicomp,
    actl_carriage, actl_space, actl_meta, actl_interval, actl_any, actl_epsilon, actl_end} tokeniser_action;

#define check_flag(a,b) ((a&b)==b)
#define remove_flag(a,b) a &= ~b
#define add_flag(a,b) a |= b

bool ckjchar(wchar_t ucs);

const int16_t character_table_size = 1611;
//---------------------------------------------------
//---------------------------------------------------
// Automaton based method
//---------------------------------------------------
//---------------------------------------------------

class tokeniser_node;
template <typename STRNG> class tokeniser_result {
public:
    vecter<int16_t> stacktype;
    vector<long> stackln;
    vector<long> positions;
    
    vector<long> p_stack;
    vector<short> i_stack;
    vector<tokeniser_node*> r_stack;

    u_uchar currentchr;
    u_ustring token;
    
    long line;
    long start;
    long position;
    long buffer_size;

    char sz_read;
    bool store_all;
    
    bool end() {
        return (position >= buffer_size && !currentchr);
    }

    vecter_n<STRNG> stack;
    
    STRNG* buffer;
    vecter_n<STRNG>* stack_ptr;
    
    tokeniser_result<STRNG>(vecter_n<STRNG>* s, bool p_all = true) {
        stack_ptr = s;
        store_all = p_all;
    }
    
    tokeniser_result<STRNG>() {
        stack_ptr = &stack;
        store_all = true;
    }
        
    void store(int32_t label);
    void store_currentchr();
    
    inline void clear_stack() {
        r_stack.clear();
        i_stack.clear();
        p_stack.clear();
    }

    //Keeping the stack small
    inline void check_stack(tokeniser_node* rule) {
        if (r_stack.size() > 1000 && rule == r_stack[1]) {
            r_stack.erase(r_stack.begin() + 1, r_stack.end());
            i_stack.erase(i_stack.begin() + 1, i_stack.end());
            p_stack.erase(p_stack.begin() + 3, p_stack.end());
        }
    }
    
    inline long pop() {
        long i = p_stack.back();
        p_stack.pop_back();
        return i;
    }

    inline void push_stack(tokeniser_node* rule, long i) {
        r_stack.push_back(rule);
        i_stack.push_back(i);
        
        p_stack.push_back(line);
        p_stack.push_back(position);
        p_stack.push_back(token.size());
        p_stack.push_back((long)currentchr);
    }

    inline long pop_stack() {
        long  i = i_stack.back();
        i_stack.pop_back();
        r_stack.pop_back();
        
        currentchr = (u_uchar)pop();
        token = token.substr(0, pop());
        position = pop();
        line = pop();
                
        return i;
    }

    void cleaning() {
        currentchr = 0;
        line = 0;
        start = 0;
        position = 0;
        sz_read = 0;
        
        stacktype.clear();
        stackln.clear();
        positions.clear();
        
        p_stack.clear();
        i_stack.clear();
        r_stack.clear();

        stack.clear();
        token = U"";
    }
    
    void clear(STRNG& u) {
        buffer = &u;
        buffer_size = u.size();
        
        currentchr = 0;
        line = 0;
        start = 0;
        position = 0;
        sz_read = 0;
        stack_ptr->clear();
        stacktype.clear();
        stackln.clear();
        positions.clear();
    }

    void clear(STRNG* u, long sz) {
        buffer = u;
        buffer_size = sz;
        
        currentchr = 0;
        line = 0;
        start = 0;
        position = 0;
        sz_read = 0;
        stack_ptr->clear();
        stacktype.clear();
        stackln.clear();
        positions.clear();
    }

    long size() {
        return stack.size();
    }
    
    void getnext();
    
};

class tokenizer_theautomaton;
//A node in the automaton
class tokeniser_node {
public:
    vector<tokeniser_node*> arcs;
    
    int16_t idpos;
    int16_t flags;
    
    u_uchar label;
	u_uchar endlabel;
    
    tokeniser_action action;
    
    
    tokeniser_node(tokeniser_action a, u_uchar c, u_uchar e) {
        idpos = -1;
        flags = flagl_none;
        action = a;
        label = c;
        endlabel = e;
    }
    
    inline bool pure_arc() {
        return (action > actl_none && action < actl_meta && !check_flags());
    }
    
    inline bool is_epsilon() {
        return (action == actl_epsilon && !check_flags());
    }
    
    inline long size() {
        return arcs.size();
    }
    
    inline bool check_flags() {
        return ((flags & flagl_action) != 0);
    }
    
    inline bool check_negation() {
        return check_flag(flags, flagl_negation);
    }
    
    inline bool check_fail() {
        return check_flag(flags, flagl_fail);
    }
    
    inline bool check_start_fail() {
        return check_flag(flags, flagl_start_fail);
    }
    
    inline bool check_skip() {
        return check_flag(flags, flagl_skip);
    }
    
    inline bool check_vectorized() {
        return check_flag(flags, flagl_vectorized);
    }
    
    inline bool check_visited() {
        return check_flag(flags, flagl_visited);
    }
    
    inline bool check_postpone() {
        return check_flag(flags, flagl_postpone);
    }
    
    inline bool check_seen() {
        return check_flag(flags, flagl_seen);
    }
    
    inline bool check_added() {
        return check_flag(flags, flagl_added);
    }

    inline bool check_blocking_gate() {
        return check_flag(flags, flagl_blocking_gate);
    }

    inline bool check_tail() {
        return check_flag(flags, flagl_tail);
    }

    void processing_postponed_nodes(std::set<long>& visited, vector<tokeniser_node*>& nodes, long idpos);
    void remove_epsilon_nodes(std::unordered_map<long, std::set<long> >& visited, std::set<long>& up, vector<tokeniser_node*>& nodes, bool epsilon);
    void merge(bool top);

    //We remove the epsilon nodes
    void trim_epsilon(vector<tokeniser_node*>& nodes) {
        std::set<long> up;
        std::unordered_map<long, std::set<long> > visited;
        remove_epsilon_nodes(visited, up, nodes, is_epsilon());
    }

    //We mark all nodes that belong to the same automaton for cleaning purposes
    //The nodes that won't have been marked will be deleted.
    void mark_nodes() {
        if (check_visited())
            return;
        add_flag(flags, flagl_visited);
        for (long i = 0; i < arcs.size(); i++) {
            arcs[i]->mark_nodes();
        }
    }

    inline void fail_arcs() {
        tokeniser_node* node;
        //We mark the final arcs, to take into account sequences
        //In a sequence, only the last element is marked with fail.
        for (long i = 0; i < arcs.size(); i++) {
            node = arcs[i];
            //The first node is marked with flagl_start_fail
            add_flag(node->flags, flagl_start_fail);
            while (node->arcs.size()) node = node->arcs.back();
            //if it a sequence, we have the beginning and the end of the sequence that is marked
            //otherwise the node has both flags on.
                     add_flag(node->flags, flagl_fail);
        }
    }
    
    inline void clone(tokeniser_node* a) {
        arcs = a->arcs;
        action = a->action;
        flags = a->flags;
        label = a->label;
        endlabel = a->endlabel;
    }
    
    inline bool empty() {
        return !arcs.size();
    }
    
    inline void append_final(tokeniser_node* a) {
        tokeniser_node* node = this;
        while (node->arcs.size()) node = node->arcs.back();
        if (!node->check_fail())
            node->arcs.push_back(a);
    }
    
    //We addd a and b on the final nodes for each arc
    //This is a way to handle {..}+
    inline void append_final(tokeniser_node* a, tokeniser_node* b) {
        tokeniser_node* node = this;
        while (node->arcs.size()) node = node->arcs.back();
        if (!node->check_fail()) {
            node->arcs.push_back(a);
            node->arcs.push_back(b);
        }
    }
    
    inline void append(tokeniser_node* a) {
        arcs.push_back(a);
    }
    
    bool check(u_uchar chr, utf8_handler* access, std::set<u_uchar>& operators, bool not_neg);
    
    inline void getnext(u_ustring& w, u_uchar& res, long& pos, long& l) {
        if (pos>=w.size()) {
            res =  0;
            return;
        }
        
        if (w[pos]==L'\n')
            l++;
        res = w[pos++];
    }
    
};

//---------------------------------------------------------------
//The automaton itself
//---------------------------------------------------------------

class tokenizer_theautomaton {
public:
    tokeniser_node* vectorization_table[character_table_size];
    hmap<int32_t,vector<tokeniser_node*> > indexed_on_label;

    vector<tokeniser_node*> compiled_rules;
    vector<tokeniser_node*> nodes;
    
    vecter_n<u_ustring> rules;
    
    std::set<u_uchar> operators;
    
    utf8_handler* access;
    tokeniser_node* initial;
    
    tokeniser_flag negation;
    bool disjunction;
    bool loaded;
    bool displayautomata;
    
    tokenizer_theautomaton(utf8_handler* a) {
        disjunction =  false;
        displayautomata = false;
        negation = flagl_none;
        initial = NULL;
        access = a;
        for (int16_t i = 1; i < character_table_size; i++) {
            vectorization_table[i] = NULL;
        }
        vectorization_table[0] = node(actl_epsilon, 0);
        loaded = false;
        setoperators();
    }
    
    ~tokenizer_theautomaton() {
        for (long i = 0; i < nodes.size(); i++) {
            delete nodes[i];
        }
    }

    void compile();

    void reset() {
        for (long i = 0; i < nodes.size(); i++) {
            delete nodes[i];
        }
        nodes.clear();
        compiled_rules.clear();
        for (int16_t i = 1; i < character_table_size; i++) {
            vectorization_table[i] = NULL;
        }
        vectorization_table[0] = node(actl_epsilon, 0);
        indexed_on_label.clear();
        setoperators();
        loaded = false;
    }
    
    
    void setoperators() {
        operators.clear();
        std::set<u_ustring> ops;
        ops.insert(U"¬");
        ops.insert(U"§");
        ops.insert(U"•");
        ops.insert(U"°");
        ops.insert(U"%");
        ops.insert(U"@");
        ops.insert(U"=");
        ops.insert(U"!");
        ops.insert(U"+");
        ops.insert(U"-");
        ops.insert(U"*");
        ops.insert(U"/");
        ops.insert(U"^");
        ops.insert(U"<");
        ops.insert(U">");
        ops.insert(U"~");
        ops.insert(U",");
        ops.insert(U"&");
        ops.insert(U"|");
        ops.insert(U"#");
        ops.insert(U"?");
        ops.insert(U"\\");
        ops.insert(U"$");
        for (auto& u: ops)
            operators.insert(u[0]);
    }
    
    void replacemetas(map<u_ustring, u_ustring>& metalines, u_ustring& line) {
        if (line.find(U"%") == -1 || line.size()==1)
            return;
        
        u_ustring rep;
        u_ustring fd;
        for (const auto& k : metalines) {
            if (line.find(k.first) != -1) {
                fd = k.first;
                rep = k.second;
                line = str_ureplacestring(line, fd, rep);
            }
        }
    }
    
    void initialize() {
        setrules();
        compile();
    }
    
    void set_rules(vecter_n<u_ustring>& r) {
        reset();
        rules = r;
        compile();
    }
    
    void get_rules(vecter_n<u_ustring>& r) {
        if (!rules.size()) {
            setrules();
            r=rules;
            rules.clear();
            return;
        }
        
        r=rules;
    }
    
    virtual void setrules();
    
    void display(tokeniser_node*, int nbblanks, bool);
    void asString(std::set<int16_t>& shared, std::wstringstream& str, tokeniser_node*, int nbblanks, bool);
    
    tokeniser_node* node(tokeniser_action a, u_uchar c, u_uchar e = 0) {
        if (a == actl_meta) {
            switch (c) {
                case 'C':
                    a =  actl_uppercase;
                    break;
                case 'H':
                    a =  actl_ckj;
                    break;
                case 'S':
                    a =  actl_space_and_carriage;
                    break;
                case 'E':
                    a =  actl_emojicomp;
                    break;
                case 'a':
                    a =  actl_alphabetical;
                    break;
                case 'c':
                    a =  actl_lowercase;
                    break;
                case 'd':
                    a =  actl_digit;
                    break;
                case 'e':
                    a =  actl_emoji;
                    break;
                case 'h':
                    a =  actl_greek;
                    break;
                case 'n':
                    a =  actl_non_breaking_space;
                    break;
                case 'o':
                    a =  actl_operator;
                    break;
                case 'p':
                    a =  actl_punctuation;
                    break;
                case 'r':
                    a =  actl_carriage;
                    break;
                case 's':
                    a =  actl_space;
                    break;
            }
        }
        
        tokeniser_node* n = new tokeniser_node(a,c,e);
        add_flag(n->flags, negation);
        negation = flagl_none;
        n->idpos = nodes.size();
        nodes.push_back(n);
        return n;
    }
    /*
     There are different ways to access rules when evaluating the first character of a token.
     a) Either it is in the vectorization table
     b) Or it is in the tokenizer_rules vector
     
     The vectorization table makes it possible to index rules on their first character.
     It is composed of 1611 slots (character_table_size), where each slot corresponds to
     the Unicode code of a character. For instance, 'A' has the code 65. At the position 65 we have rules that deals with
     alphabetical characters or with rules that starts with 'A'. A slot can contain more than 1 rule, which means that the order in
     which these rules are stored depends on the order in which they have been declared.
     
     The tokenizer_rules contains the rule, which could not be indexed on their first characters.
     Basically when parsing a code, we first try the rules which are indexed on this character if we can,
     otherwise we roll backs on tokenizer_rules.
     */

    void vectorization(tokeniser_node* a, u_uchar c) {
        if (!c || c >= character_table_size) {
            if (a->check_added())
                return;
            add_flag(a->flags, flagl_added);
            c = 0;
        }
        else {
            if (vectorization_table[c] == NULL)
                vectorization_table[c] = node(actl_epsilon, 0);
        }
        
        vectorization_table[c]->append(a);
        add_flag(a->flags, flagl_vectorized);
        if (initial == NULL)
            initial = a;
    }
    
    tokeniser_node* append(tokeniser_node* root, tokeniser_node* n) {
        if (initial == NULL)
            initial = n;
        
        if (root != NULL) {
            if (disjunction) {
                root->append(n);
                return root;
            }
            //If it is a disjunction, then root action is epsilon
            //the new arc needs to be added to each sub-arc.
            root->append(n);
        }
        else {
            //The position 0 in vectorization_table is used to store no indexable rules
            if (!n->check_vectorized())
                vectorization(n, 0);
        }
        
        return n;
    }
        
    template <typename STRNG> bool iterative_traversal(tokeniser_result<STRNG>& rst, tokeniser_node* rule) {
        //In this case, we apply a iterative analysis...
        //The first element is the one that should be found to stop
        tokeniser_node* r;
        long i = 0;
        
        rst.clear_stack();
        rst.push_stack(rule, 0);
        if (!check_flag(rule->action, actl_epsilon)) {
            if (!rule->check_skip())
                rst.token += rst.currentchr;
            rst.getnext();
        }

        long sz = rule->size();
        while (rst.r_stack.size() && !rst.end()) {
            for (; i < sz && !rst.end(); i++) {
                r = rule->arcs[i];
                if (r->action == actl_end) {
                    rst.store(r->label);
                    return true;
                }
                
                if (r->check(rst.currentchr, access, operators, !r->check_negation())) {
                    if (r == rule) {
                        i = -1;
                        if (!check_flag(r->action, actl_epsilon)) {
                            if (!r->check_skip())
                                rst.token += rst.currentchr;
                            rst.getnext();
                        }
                    }
                    else {
                        rst.check_stack(r);
                        rst.push_stack(r, i + 1);
                        if (!check_flag(r->action, actl_epsilon)) {
                            if (!r->check_skip())
                                rst.token += rst.currentchr;
                            rst.getnext();
                        }
                        rule = r;
                        sz = rule->size();
                        i = 0;
                        break;
                    }
                }
            }
            if (i == sz) {
                i = rst.pop_stack();
                if (rst.r_stack.size()) {
                    rule = rst.r_stack.back();
                    sz = rule->size();
                }
            }
        }
        return false;
    }
    
    //Since, the code is slightly different according to the type of string we are handling, we create a template function to handle all these cases in one code.
    template <typename STRNG> bool traverse(tokeniser_result<STRNG>& rst, tokeniser_node* rule, bool alreadychecked, bool top) {
        if (rule->action == actl_end) {
            rst.store(rule->label);
            return true;
        }
        if (rst.end())
            return false;
        
        if (top) {
            rst.start = rst.position - rst.sz_read;
            rst.token = U"";
        }
        

        u_uchar chr = rst.currentchr;
        long l = rst.line;
        long p = rst.position;
        long tokenlen = rst.token.size();

        long sz = rule->arcs.size();

        while (sz == 1 && !rst.end()) {
            if (alreadychecked || rule->check(rst.currentchr, access, operators, !rule->check_negation())) {
                //In this case we fail a sequence of nodes that should not go up to here
                // For instance, we want the system to fail if abc is detected with rule: ~{[abc]...}
                if (rule->check_fail())
                    return true;

                if (rule->check_tail())
                    return iterative_traversal<STRNG>(rst, rule);

                if (!check_flag(rule->action, actl_epsilon)) {
                    if (!rule->check_skip())
                        rst.token += rst.currentchr;
                    rst.getnext();
                }
                rule = rule->arcs[0];
                if (rule->action == actl_end) {
                    rst.store(rule->label);
                    return true;
                }
                alreadychecked = false;
                sz = rule->arcs.size();
            }
            else {
                rst.token = rst.token.substr(0, tokenlen);
                rst.currentchr = chr;
                rst.line = l;
                rst.position = p;
                return false;
            }
        }
        

        if (alreadychecked || rule->check(rst.currentchr, access, operators, !rule->check_negation())) {
            if (rule->check_fail())
                return true;

            if (rule->check_tail())
                return iterative_traversal<STRNG>(rst, rule);

            //If it is not an epsilon, there was an actual character comparison
            alreadychecked = false;
            if (!check_flag(rule->action, actl_epsilon)) {
                if (!rule->check_skip())
                    rst.token += rst.currentchr;
                alreadychecked = true;
                rst.getnext();
            }
            
            tokeniser_node* r;
            for (long i = 0; i < sz; i++) {
                r = rule->arcs[i];
                if (r->action == actl_end) {
                    rst.store(r->label);
                    return true;
                }
                if (r->check(rst.currentchr, access, operators, !r->check_negation())) {
                    //Tail recursion detected, we simply iterate...
                    //alreadycheck is true is this arc is not an epsilon
                    if (alreadychecked && r == rule) {
                        i = -1;
                        if (!r->check_skip())
                            rst.token += rst.currentchr;
                        rst.getnext();
                    }
                    else {
                        /*
                        else we go into recursion
                        Note the check_fail and check_start_fail to take into account: ~{...}
                        If one element of the disjunction is true, the whole disjunction must fail
                        The only case when fail and start_fail are different is when a sequence is negated
                        in the disjunction. In that case, start_fail occurs on the beginning of the sequence
                        and fail on the last element of the disjunction. Hence, if the sequence succeeds deep in recursion
                        we will fail it by returning !start_fail.
                        */
                        if ((r->check_fail() || traverse<STRNG>(rst, r, true, false)))
                            return (!r->check_start_fail());
                    }
                }
            }
        }
        
        rst.token = rst.token.substr(0, tokenlen);
        rst.currentchr = chr;
        rst.line = l;
        rst.position = p;
        return false;
    }
    
    template <typename STRNG> inline bool execute_rule(u_uchar c, tokeniser_result<STRNG>& rst) {
        return (c < character_table_size) && vectorization_table[c]?traverse<STRNG>(rst, vectorization_table[c], false, true):false;
    }
    
    template <typename STRNG> inline bool execute_rule(tokeniser_result<STRNG>& rst) {
        return traverse<STRNG>(rst, vectorization_table[0], false, true);
    }

    template <typename STRNG> void apply(tokeniser_result<STRNG>& rst) {
        rst.getnext();
        while (!rst.end()) {
            //The first step consists of finding the first rule to apply
            //If the current character is in table, then we start from here
            if (!execute_rule<STRNG>(rst.currentchr, rst) && !execute_rule<STRNG>(rst)) {
                rst.store_currentchr();
                rst.getnext();
            }
        }
    }
  
    template <typename STRNG> long tokenize(STRNG& thestr, tokeniser_result<STRNG>& r) {
        if (!loaded) {
            setrules();
            compile();
        }
        //only stack is necessary
        r.clear(thestr);
        apply<STRNG>(r);
        return r.size();
    }    
};

//---------------------------------------------------------------
//LispE tokenizer
//---------------------------------------------------------------
class lispe_tokenizer : public tokenizer_theautomaton {
public:
    
    lispe_tokenizer(utf8_handler* a) : tokenizer_theautomaton(a) {}
    
    void setrules() {
        tokenizer_theautomaton::setrules();
        rules[0] = U"%s+=#";
        long i;
        for (i = 0; i < rules.size(); i++) {
            if (rules[i] == U";;?+;;=:67")
                break;
        }
        //Comments are no longer recorded...
        if (i != rules.size()) {
            rules[i] = U";;?+;;=:#";
            rules[i+1] = U";?+#10=#";
            rules[i+2] = U"%#!?+%r=#";
        }
    }
    
};
//---------------------------------------------------------------
//Segmenter
//---------------------------------------------------------------
class segmenter_theautomaton : public tokenizer_theautomaton {
public:
    char decimal_separator;
    bool blanks;
    
    segmenter_theautomaton(utf8_handler* a) : tokenizer_theautomaton(a) {
        decimal_separator = 0;
        blanks = false;
    }
    
    void setrules();
    
    bool check_rule(int32_t label) {
        return (indexed_on_label.count(label) && indexed_on_label[label].size());
    }
    
    //Rules do not skip blanks anymore
    void keepblanks(bool kp) {
        if (check_rule(99) && blanks != kp) {
            blanks = kp;
            if (kp)
                remove_flag(indexed_on_label[99][0]->flags, flagl_blocking_gate);
            else
                add_flag(indexed_on_label[99][0]->flags, flagl_blocking_gate);
        }
    }
    
    void setdecimalmode(char sep) {
        if (check_rule(22) && check_rule(66)  && check_rule(88) && decimal_separator != sep) {
            long i;
            decimal_separator = sep;
            switch (decimal_separator) {
                case 0:
                    for (i = 0; i < indexed_on_label[22].size(); i++) {
                        remove_flag(indexed_on_label[22][i]->flags, flagl_blocking_gate);
                    }
                    for (i = 0; i < indexed_on_label[66].size(); i++) {
                        add_flag(indexed_on_label[66][i]->flags, flagl_blocking_gate);
                    }
                    for (i = 0; i < indexed_on_label[88].size(); i++) {
                        add_flag(indexed_on_label[88][i]->flags, flagl_blocking_gate);
                    }
                    break;
                case 1:
                    for (i = 0; i < indexed_on_label[22].size(); i++) {
                        add_flag(indexed_on_label[22][i]->flags, flagl_blocking_gate);
                    }
                    for (i = 0; i < indexed_on_label[66].size(); i++) {
                        remove_flag(indexed_on_label[66][i]->flags, flagl_blocking_gate);
                    }
                    for (i = 0; i < indexed_on_label[88].size(); i++) {
                        add_flag(indexed_on_label[88][i]->flags, flagl_blocking_gate);
                    }
                    break;
                case 2:
                    for (i = 0; i < indexed_on_label[22].size(); i++) {
                        add_flag(indexed_on_label[22][i]->flags, flagl_blocking_gate);
                    }
                    for (i = 0; i < indexed_on_label[66].size(); i++) {
                        add_flag(indexed_on_label[66][i]->flags, flagl_blocking_gate);
                    }
                    for (i = 0; i < indexed_on_label[88].size(); i++) {
                        remove_flag(indexed_on_label[88][i]->flags, flagl_blocking_gate);
                    }
                    break;
            }
        }
    }
};

#endif






