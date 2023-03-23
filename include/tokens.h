/*
 *  LispE
 *
 * Copyright 2020-present NAVER Corp.
 * The 3-Clause BSD License
 */
//  tokens.h (see tools.cxx for implementation)
//
//

#ifndef tokens_h
#define tokens_h


#include <string>
#include <vector>
#include <unordered_map>
#include "conversion.h"

using std::u16string;
#define u_uchar char32_t
#define u_ustring std::u32string

using std::vector;
using std::unordered_map;
typedef enum {flag_none = 0, flag_negation = 1, flag_skip = 2, flag_fail = 4, flag_start_fail = 8, flag_blocking_gate = 16, flag_tail = 32, flag_action = 63, flag_vectorized = 64, flag_postpone = 128, flag_seen = 256, flag_visited = 512, flag_added = 1024} tokenizer_flag;


typedef enum {act_none, act_char , act_uppercase, act_ckj, act_space_and_carriage, act_alphabetical,
    act_lowercase, act_digit, act_greek, act_non_breaking_space, act_operator, act_punctuation,
    act_carriage, act_space, act_meta, act_interval, act_any, act_epsilon, act_end} tokenizer_action;

#define check_flag(a,b) ((a&b)==b)
#define remove_flag(a,b) a &= ~b
#define add_flag(a,b) a |= b

const int16_t table_character_size = 1611;
const int32_t def_lisp_mode = 1611; //This value should not be used in rules
//---------------------------------------------------
//---------------------------------------------------
// Automaton based method
//---------------------------------------------------
//---------------------------------------------------

template <typename STRNG> class tokenizer_result {
public:
    vector<unsigned char> stacktype;
    vector<long> stackln;
    vector<long> bpos;
    vector<long> cpos;
    
    u_uchar currentchr;
    u_ustring token;
    
    long line;
    long b_start;
    long c_start;
    long b_pos;
    long c_pos;
    long buffer_size;
    short count_parentheses;

    char sz_read;
    char check_parentheses;
    bool store_all;
    bool lispmode;
    
    bool end() {
        return (b_pos >= buffer_size && !currentchr);
    }

    vector<STRNG> stack;
    
    STRNG* buffer;
    vector<STRNG>* stack_ptr;
    
    tokenizer_result<STRNG>(vector<STRNG>* s, bool p_all = true) {
        lispmode = false;
        stack_ptr = s;
        store_all = p_all;
    }
    
    tokenizer_result<STRNG>(bool p_all = true) {
        lispmode = false;
        stack_ptr = &stack;
        store_all = p_all;
    }
     
    void setstack(vector<STRNG>* s) {
        stack_ptr = s;
    }
    
    void store_char(u_uchar c) {
        stack_ptr->push_back(STRNG(1, c));
        if (store_all) {
            stacktype.push_back(0);
            stackln.push_back(line);
            bpos.push_back(b_start++);
            cpos.push_back(c_start++);
        }
    }
    
    inline bool check_label(u_ustring& token, int32_t label) {
        //def_lisp_mode is the label for Lisp Expression
        if (label == def_lisp_mode) {
            for (u_uchar c: token) {
                store_char(c);
            }
            if (!lispmode) {
                check_parentheses = 1;
                count_parentheses = 1;
            }
            return true;
        }
        return false;
    }

    inline void check_parenthesis(int16_t& flags) {
        if (check_parentheses) {
            if (check_parentheses == 1) {
                remove_flag(flags, flag_blocking_gate);
                check_parentheses = 2;
            }
            count_parentheses += (currentchr == '(');
            count_parentheses -= (currentchr == ')');
            if (!count_parentheses) {
                check_parentheses = 0;
                add_flag(flags, flag_blocking_gate);
            }
        }
    }
    
    void store(int32_t label);
    
    void store_currentchr();
    
    void clear(STRNG& u) {
        buffer = &u;
        buffer_size = u.size();
        
        check_parentheses = false;
        count_parentheses = 0;
        currentchr = 0;
        line = 0;
        b_start = 0;
        b_pos = 0;
        c_pos = 0;
        sz_read = 0;
        stack_ptr->clear();
        stacktype.clear();
        stackln.clear();
        bpos.clear();
        cpos.clear();
    }

    void clear(STRNG* u, long sz) {
        buffer = u;
        buffer_size = sz;
        
        check_parentheses = false;
        count_parentheses = 0;
        
        currentchr = 0;
        line = 0;
        b_start = 0;
        b_pos = 0;
        c_pos = 0;
        sz_read = 0;
        stack_ptr->clear();
        stacktype.clear();
        stackln.clear();
        bpos.clear();
        cpos.clear();
    }

    long size() {
        return stack.size();
    }
    
    void getnext();
    
};

class tokenizer_node;
typedef bool (tokenizer_node::*checkLabel)(u_uchar c, std::set<u_uchar>& operators);

//A node in the automaton
class tokenizer_node {
public:
    vector<tokenizer_node*> arcs;
    
    int16_t idpos;
    int16_t flags;
    
    u_uchar label;
	u_uchar endlabel;
    
    tokenizer_action action;
    
    
    tokenizer_node(tokenizer_action a, u_uchar c, u_uchar e) {
        idpos = -1;
        flags = flag_none;
        action = a;
        label = c;
        endlabel = e;
    }
    
    inline bool pure_arc() {
        return (action > act_none && action < act_meta && !check_flags());
    }
    
    inline bool is_epsilon() {
        return (action == act_epsilon && !check_flags());
    }
    
    inline long size() {
        return arcs.size();
    }
    
    inline bool check_flags() {
        return ((flags & flag_action) != 0);
    }
    
    inline bool check_negation() {
        return check_flag(flags, flag_negation);
    }
    
    inline bool check_fail() {
        return check_flag(flags, flag_fail);
    }
    
    inline bool check_start_fail() {
        return check_flag(flags, flag_start_fail);
    }
    
    inline bool check_skip() {
        return check_flag(flags, flag_skip);
    }
    
    inline bool check_vectorized() {
        return check_flag(flags, flag_vectorized);
    }
    
    inline bool check_visited() {
        return check_flag(flags, flag_visited);
    }
    
    inline bool check_postpone() {
        return check_flag(flags, flag_postpone);
    }
    
    inline bool check_seen() {
        return check_flag(flags, flag_seen);
    }
    
    inline bool check_added() {
        return check_flag(flags, flag_added);
    }

    inline bool check_blocking_gate() {
        return check_flag(flags, flag_blocking_gate);
    }

    inline bool check_tail() {
        return check_flag(flags, flag_tail);
    }

    void processing_postponed_nodes(std::set<long>& visited, vector<tokenizer_node*>& nodes, long idpos);
    void remove_epsilon_nodes(std::unordered_map<long, std::set<long> >& visited, std::set<long>& up, vector<tokenizer_node*>& nodes, bool epsilon);
    void merge(bool top);

    //We remove the epsilon nodes
    void trim_epsilon(vector<tokenizer_node*>& nodes) {
        std::set<long> up;
        std::unordered_map<long, std::set<long> > visited;
        remove_epsilon_nodes(visited, up, nodes, is_epsilon());
    }

    //We mark all nodes that belong to the same automaton for cleaning purposes
    //The nodes that won't have been marked will be deleted.
    void mark_nodes() {
        if (check_visited())
            return;
        add_flag(flags, flag_visited);
        for (long i = 0; i < arcs.size(); i++) {
            arcs[i]->mark_nodes();
        }
    }

    inline void fail_arcs() {
        tokenizer_node* node;
        //We mark the final arcs, to take into account sequences
        //In a sequence, only the last element is marked with fail.
        for (long i = 0; i < arcs.size(); i++) {
            node = arcs[i];
            //The first node is marked with flag_start_fail
            add_flag(node->flags, flag_start_fail);
            while (node->arcs.size()) node = node->arcs.back();
            //if it a sequence, we have the beginning and the end of the sequence that is marked
            //otherwise the node has both flags on.
            add_flag(node->flags, flag_fail);
        }
    }
    
    inline void clone(tokenizer_node* a) {
        arcs = a->arcs;
        action = a->action;
        flags = a->flags;
        label = a->label;
        endlabel = a->endlabel;
    }
    
    inline bool empty() {
        return !arcs.size();
    }
    
    inline void append_final(tokenizer_node* a) {
        tokenizer_node* node = this;
        while (node->arcs.size()) node = node->arcs.back();
        if (!node->check_fail())
            node->arcs.push_back(a);
    }
    
    //We addd a and b on the final nodes for each arc
    //This is a way to handle {..}+
    inline void append_final(tokenizer_node* a, tokenizer_node* b) {
        tokenizer_node* node = this;
        while (node->arcs.size()) node = node->arcs.back();
        if (!node->check_fail()) {
            node->arcs.push_back(a);
            node->arcs.push_back(b);
        }
    }
    
    bool ckjchar(char32_t ucs) {
        return
        (ucs >= 0x1100 &&
         (ucs <= 0x115f ||                    /* Hangul Jamo init. consonants */
          ucs == 0x2329 || ucs == 0x232a ||
          (ucs >= 0x2e80 && ucs <= 0xa4cf &&
           ucs != 0x303f) ||                  /* CJK ... Yi */
          (ucs >= 0xac00 && ucs <= 0xd7a3) || /* Hangul Syllables */
          (ucs >= 0xf900 && ucs <= 0xfaff) || /* CJK Compatibility Ideographs */
          (ucs >= 0xfe10 && ucs <= 0xfe19) || /* Vertical forms */
          (ucs >= 0xfe30 && ucs <= 0xfe6f) || /* CJK Compatibility Forms */
          (ucs >= 0xff00 && ucs <= 0xff60) || /* Fullwidth Forms */
          (ucs >= 0xffe0 && ucs <= 0xffe6) ||
          (ucs >= 0x20000 && ucs <= 0x2fffd) ||
          (ucs >= 0x30000 && ucs <= 0x3fffd)));
    }

    inline void append(tokenizer_node* a) {
        arcs.push_back(a);
    }
    
    inline bool c_is_nbs_space(u_uchar code) {
        return (code == 160 || code == 0x202F || code == 0x3000);
    }

    inline bool c_is_space(u_uchar code) {
        return (code  == 9 || code == 32 || code == 160 || code == 0x202F || code == 0x3000);
    }

    inline bool c_is_space_or_cr(u_uchar code) {
        return (code  == 9 || code == 10 || code == 13 || code == 32 || code == 160 || code == 0x202F || code == 0x3000);
    }
    
    inline bool check(u_uchar chr, std::set<u_uchar>& operators, bool not_neg) {
        if (check_blocking_gate() || !chr)
            return false;
        
        switch(action) {
            case act_char:
                return (not_neg == (label == chr));
            case act_uppercase:
                return (not_neg == c_is_upper(chr));
            case act_ckj://Chinese/Japanese/Korean
                return (not_neg == ckjchar(chr));
            case act_space_and_carriage:
                return (not_neg == c_is_space_or_cr(chr));
            case act_alphabetical:
                return (not_neg == (chr == '_' || c_is_alpha(chr)));
            case act_lowercase:
                return (not_neg == c_is_lower(chr));
            case act_digit:
                return (not_neg == c_is_digit(chr));
            case act_greek:
                return (not_neg == (chr >= 913 && chr <= 987));
            case act_non_breaking_space: //non-breaking space
                return (not_neg == c_is_nbs_space(chr));
            case act_operator:
                return (not_neg == (operators.find(chr) != operators.end()));
            case act_punctuation:
                return (not_neg == c_is_punctuation(chr));
            case act_carriage:
                return (not_neg == (chr == 10 || chr == 13));
            case act_space:
                return (not_neg == c_is_space(chr));
            case act_interval:
                return (not_neg == (chr >= label && chr <= endlabel));
            case act_any:
            case act_epsilon:
                return true;
            case act_none:
            case act_end:
            case act_meta:
                return false;
        }
        return false;
    }

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

class tokenizer_automaton {
public:
    tokenizer_node* vectorization_table[table_character_size];
    vector<tokenizer_node*> nodes;
    
    vector<u_ustring> rules;
    
    std::set<u_uchar> operators;
    hmap<int32_t,vector<tokenizer_node*> > indexed_on_label;
    
    tokenizer_node* initial;
    
    tokenizer_flag negation;
    bool disjunction;
    bool loaded;
    bool lisp_detection;
    tokenizer_node* lisp_quote;
    
    tokenizer_automaton() {
        lisp_quote = NULL;
        lisp_detection = false;
        disjunction =  false;
        negation = flag_none;
        initial = NULL;
        for (int16_t i = 1; i < table_character_size; i++) {
            vectorization_table[i] = NULL;
        }
        vectorization_table[0] = node(act_epsilon, 0);
        loaded = false;
        setoperators();
    }
    
    ~tokenizer_automaton() {
        for (long i = 0; i < nodes.size(); i++) {
            delete nodes[i];
        }
    }
    
    void reset() {
        for (long i = 0; i < nodes.size(); i++) {
            delete nodes[i];
        }
        nodes.clear();
        for (int16_t i = 1; i < table_character_size; i++) {
            vectorization_table[i] = NULL;
        }
        vectorization_table[0] = node(act_epsilon, 0);
        indexed_on_label.clear();
        setoperators();
        loaded = false;
    }
    
    void display(tokenizer_node* a, int nbblanks, bool top);
    
    void setoperators() {
        operators.clear();
        std::set<u_ustring> ops;
        ops.insert(U"≠");
        ops.insert(U"∨");
        ops.insert(U"∧");
        ops.insert(U"÷");
        ops.insert(U"×");
        ops.insert(U"²");
        ops.insert(U"³");
        ops.insert(U"¬");
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
                line = s_ureplacestring(line, fd, rep);
            }
        }
    }
    
    void initialize() {
        setrules();
        compile();
    }
    
    void set_rules(vector<u_ustring>& r) {
        reset();
        rules = r;
        compile();
    }
    
    void get_rules(vector<u_ustring>& r) {
        if (!rules.size()) {
            setrules();
            r=rules;
            rules.clear();
            return;
        }
        
        r=rules;
    }
    
    void put_rules(vector<string>& r) {
        u_ustring u;
        for (long i = 0; i < r.size(); i++) {
            sc_utf8_to_unicode(u, USTR(r[i]), r[i].size());
            rules.push_back(u);
        }
    }
    
    virtual void setrules();
        
    tokenizer_node* node(tokenizer_action a, u_uchar c, u_uchar e = 0) {
        if (a == act_meta) {
            switch (c) {
                case 'C':
                    a =  act_uppercase;
                    break;
                case 'H':
                    a =  act_ckj;
                    break;
                case 'S':
                    a =  act_space_and_carriage;
                    break;
                case 'a':
                    a =  act_alphabetical;
                    break;
                case 'c':
                    a =  act_lowercase;
                    break;
                case 'd':
                    a =  act_digit;
                    break;
                case 'h':
                    a =  act_greek;
                    break;
                case 'n':
                    a =  act_non_breaking_space;
                    break;
                case 'o':
                    a =  act_operator;
                    break;
                case 'p':
                    a =  act_punctuation;
                    break;
                case 'r':
                    a =  act_carriage;
                    break;
                case 's':
                    a =  act_space;
                    break;
            }
        }
        
        tokenizer_node* n = new tokenizer_node(a,c,e);
        add_flag(n->flags, negation);
        negation = flag_none;
        n->idpos = nodes.size();
        nodes.push_back(n);
        return n;
    }
    /*
     There are different ways to access rules when evaluating the first character of a token.
     a) Either it is in the vectorization table
     b) Or it is in the tokenizer_rules vector
     
     The vectorization table makes it possible to index rules on their first character.
     It is composed of 1611 slots (table_character_size), where each slot corresponds to
     the Unicode code of a character. For instance, 'A' has the code 65. At the position 65 we have rules that deals with
     alphabetical characters or with rules that starts with 'A'. A slot can contain more than 1 rule, which means that the order in
     which these rules are stored depends on the order in which they have been declared.
     
     The tokenizer_rules contains the rule, which could not be indexed on their first characters.
     Basically when parsing a code, we first try the rules which are indexed on this character if we can,
     otherwise we roll backs on tokenizer_rules.
     */

    void vectorization(tokenizer_node* a, u_uchar c) {
        if (!c || c >= table_character_size) {
            if (a->check_added())
                return;
            add_flag(a->flags, flag_added);
            c = 0;
        }
        else {
            if (vectorization_table[c] == NULL)
                vectorization_table[c] = node(act_epsilon, 0);
        }
        
        vectorization_table[c]->append(a);
        add_flag(a->flags, flag_vectorized);
        if (initial == NULL)
            initial = a;
    }
    
    tokenizer_node* append(tokenizer_node* root, tokenizer_node* n) {
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
    
    int32_t convert(u_ustring& u) {
        int32_t v = 0;
        for (auto& a : u) {
            v = (v*10) + (a - 48);
        }
        return v;
    }
    
    long compile();
    
    template <typename STRNG> bool iterative_traversal(tokenizer_result<STRNG>& rst, tokenizer_node* rule) {
        //In this case, we apply a iterative analysis...
        //The first element is the one that should be found to stop
        tokenizer_node* r;
        long sz = rule->size();
        long p = rst.b_pos;
        long c = rst.c_pos;
        if (!rule->check_skip())
            rst.token += rst.currentchr;
        rst.getnext();
        long tokenlen;
        for (long i = 0; i < sz && !rst.end(); i++) {
            r = rule->arcs[i];
            if (r->check(rst.currentchr, operators, !r->check_negation())) {
                if (r->arcs.size() == 1) {
                    tokenlen = rst.token.size();
                    while (r->arcs.size() == 1) {
                        if (!r->check_skip())
                            rst.token += rst.currentchr;
                        rst.getnext();
                        r = r->arcs[0];
                        if (r->action == act_end) {
                            rst.store(r->label);
                            return true;
                        }
                        if (!r->check(rst.currentchr, operators, !rule->check_negation())) {
                            rst.b_pos = p;
                            rst.c_pos = c;
                            rst.token = rst.token.substr(0, tokenlen);
                            rst.getnext();
                            break;
                        }
                    }
                }
                i = -1;
                if (!r->check_skip())
                    rst.token += rst.currentchr;
                p = rst.b_pos;
                c = rst.c_pos;
                rst.getnext();
            }
        }
        return false;
    }
    
    //Since, the code is slightly different according to the type of string we are handling, we create a template function to handle all these cases in one code.
    template <typename STRNG> bool traverse(tokenizer_result<STRNG>& rst, tokenizer_node* rule, bool alreadychecked, bool top) {
        if (rule->action == act_end) {
            rst.store(rule->label);
            return true;
        }
        if (rst.end())
            return false;
        
        if (top) {
            rst.b_start = rst.b_pos - rst.sz_read;
            rst.c_start = rst.c_pos - 1;
            rst.token = U"";
        }
        
        bool tail = rule->check_tail();

        u_uchar chr = rst.currentchr;
        long l = rst.line;
        long p = rst.b_pos;
        long c = rst.c_pos;
        long tokenlen = rst.token.size();
        
        long sz = rule->arcs.size();

        while (sz == 1 && !rst.end()) {
            if (alreadychecked || rule->check(rst.currentchr, operators, !rule->check_negation())) {
                //In this case we fail a sequence of nodes that should not go up to here
                // For instance, we want the system to fail if abc is detected with rule: ~{[abc]...}
                if (rule->check_fail())
                    return true;
                
                if (!check_flag(rule->action, act_epsilon)) {
                    if (!rule->check_skip())
                        rst.token += rst.currentchr;
                    rst.getnext();
                }
                rule = rule->arcs[0];
                if (rule->action == act_end) {
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
                rst.b_pos = p;
                rst.c_pos = c;
                return false;
            }
        }

        if (tail)
            return iterative_traversal<STRNG>(rst, rule);

        if (alreadychecked || rule->check(rst.currentchr, operators, !rule->check_negation())) {
            if (rule->check_fail())
                return true;

            //If it is not an epsilon, there was an actual character comparison
            alreadychecked = false;
            if (!check_flag(rule->action, act_epsilon)) {
                if (!rule->check_skip())
                    rst.token += rst.currentchr;
                alreadychecked = true;
                rst.getnext();
            }
        

            tokenizer_node* r;
            for (long i = 0; i < sz; i++) {
                r = rule->arcs[i];
                if (r->action == act_end) {
                    rst.store(r->label);
                    return true;
                }
                if (r->check(rst.currentchr, operators, !r->check_negation())) {
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
        rst.b_pos = p;
        rst.c_pos = c;
        return false;
    }
    
    template <typename STRNG> inline bool execute_rule(u_uchar c, tokenizer_result<STRNG>& rst) {
        return (c < table_character_size) && vectorization_table[c]?traverse<STRNG>(rst, vectorization_table[c], false, true):false;
    }
    
    template <typename STRNG> inline bool execute_rule(tokenizer_result<STRNG>& rst) {
        return traverse<STRNG>(rst, vectorization_table[0], false, true);
    }

    template <typename STRNG> void apply(tokenizer_result<STRNG>& rst) {
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

    template <typename STRNG> void apply_with_lisp(tokenizer_result<STRNG>& rst) {
        select_lisp_mode(rst.lispmode);
        rst.getnext();
        while (!rst.end()) {
            rst.check_parenthesis(lisp_quote->flags);
            //The first step consists of finding the first rule to apply
            //If the current character is in table, then we start from here
            if (!execute_rule<STRNG>(rst.currentchr, rst) && !execute_rule<STRNG>(rst)) {
                rst.store_currentchr();
                rst.getnext();
            }
        }
    }

    //In Lisp, the quote does not define a string
    inline void select_lisp_mode(bool mode) {
        if (mode)
            remove_flag(lisp_quote->flags, flag_blocking_gate);
        else
            add_flag(lisp_quote->flags, flag_blocking_gate);
    }
    
    template <typename STRNG> long tokenize(STRNG& thestr, tokenizer_result<STRNG>& r) {
        if (!loaded) {
            setrules();
            compile();
        }
            
        //only stack is necessary
        r.clear(thestr);
        if (lisp_detection && lisp_quote != NULL)
            apply_with_lisp<STRNG>(r);
        else
            apply<STRNG>(r);
        return r.size();
    }
    
};

//---------------------------------------------------------------
//Segmenter
//---------------------------------------------------------------
class segmenter_automaton : public tokenizer_automaton {
public:
    bool decimal_separator;
    bool blanks;
    bool separator;
    
    segmenter_automaton() {
        decimal_separator = 0;
        blanks = false;
        separator = false;
    }
    
    void setrules();
    
    bool check_rule(int32_t label) {
        return (indexed_on_label.find(label) != indexed_on_label.end() && indexed_on_label[label].size());
    }
    
    //Rules do not skip blanks anymore
    void keepblanks(bool kp) {
        if (check_rule(99) && blanks != kp) {
            blanks = kp;
            if (kp)
                remove_flag(indexed_on_label[99][0]->flags, flag_blocking_gate);
            else
                add_flag(indexed_on_label[99][0]->flags, flag_blocking_gate);
        }
    }
    
    void setdecimalmode(bool sep) {
        if (check_rule(33) && check_rule(66) && decimal_separator != sep) {
            long i;
            decimal_separator = sep;
            if (!decimal_separator) {
                for (i = 0; i < indexed_on_label[33].size(); i++) {
                    remove_flag(indexed_on_label[33][i]->flags, flag_blocking_gate);
                }
                for (i = 0; i < indexed_on_label[66].size(); i++) {
                    add_flag(indexed_on_label[66][i]->flags, flag_blocking_gate);
                }
            }
            else {
                for (i = 0; i < indexed_on_label[66].size(); i++) {
                    remove_flag(indexed_on_label[66][i]->flags, flag_blocking_gate);
                }
                for (i = 0; i < indexed_on_label[33].size(); i++) {
                    add_flag(indexed_on_label[33][i]->flags, flag_blocking_gate);
                }
            }
        }
    }
    
    void setseparator(bool sep) {
        if (check_rule(22) && check_rule(44) && separator != sep) {
            long i;
            separator = sep;
            if (separator) {
                for (i = 0; i < indexed_on_label[44].size(); i++) {
                    remove_flag(indexed_on_label[44][i]->flags, flag_blocking_gate);
                }
                for (i = 0; i < indexed_on_label[22].size(); i++) {
                    add_flag(indexed_on_label[22][i]->flags, flag_blocking_gate);
                }
            }
            else {
                for (i = 0; i < indexed_on_label[22].size(); i++) {
                    remove_flag(indexed_on_label[22][i]->flags, flag_blocking_gate);
                }
                for (i = 0; i < indexed_on_label[44].size(); i++) {
                    add_flag(indexed_on_label[44][i]->flags, flag_blocking_gate);
                }
            }
        }
    }
};


#endif






