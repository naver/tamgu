/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : codecompile.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

//This section is to parse the syntactic tree of your program...

#include "codeparse.h"
#include "tamgu.h"
#include "tamgutaskell.h"
#include "compilecode.h"
#include "instructions.h"
#include "constobjects.h"
#include "tamguframeinstance.h"
#include "tamguvector.h"
#include "tamgumap.h"
#include "tamgutamgu.h"
#include "predicate.h"
#include "tamgusynode.h"
#include "tamgumapss.h"
#include "tamguivector.h"
#include "tamgufvector.h"
#include "tamgulvector.h"
#include "tamgusvector.h"
#include "tamguuvector.h"
#include "tamguhvector.h"
#include "tamgudvector.h"
#include "tamguannotator.h"
#include "equationtemplate.h"
#include "comparetemplate.h"
#include "tamgulisp.h"
#include "tokens.h"
#include "tamgucomplex.h"

#include <memory>



//----------------------------------------------------------------------------------------
//Specialization of extracting one character from a buffer for tokenizer_result
//tokenizer_result is a template that depends on the type of the string being manupulated.
//However each type has a specific expected behaviour
//----------------------------------------------------------------------------------------
bool v_tokenize(vector<string>& vect, string& thestr, short flags, vector<string>& rules) {
    
    tokenizer_result<string> xr;
    segmenter_automaton sa;

    sa.put_rules(rules);
    long err = sa.compile();
    if (err != -1) {
        string error = "Error: rule '";
        error += rules[err] + "' is malformed.";
        globalTamgu->Returnerror(error);
        return false;
    }

    if (check_flag(flags, token_comma))
        sa.setdecimalmode(true);
    if (check_flag(flags, token_keeprc))
        sa.keepblanks(true);
    if (check_flag(flags, token_separator))
        sa.setseparator(true);


    xr.setstack(&vect);
    sa.tokenize<string>(thestr,xr);

    return true;
}

bool vw_tokenize(vector<wstring>& vect, wstring& ustr, short flags, vector<string>& rules) {
    tokenizer_result<wstring> xr;
    segmenter_automaton sa;

    sa.put_rules(rules);
    long err = sa.compile();
    if (err != -1) {
        string error = "Error: rule '";
        error += rules[err] + "' is malformed.";
        globalTamgu->Returnerror(error);
        return false;
    }

    if (check_flag(flags, token_comma))
        sa.setdecimalmode(true);
    if (check_flag(flags, token_keeprc))
        sa.keepblanks(true);
    if (check_flag(flags, token_separator))
        sa.setseparator(true);

    xr.setstack(&vect);
    sa.tokenize<wstring>(ustr,xr);
    return true;
}

template<> void tokenizer_result<u_ustring>::getnext() {
    if (b_pos >= buffer_size) {
        currentchr =  0;
        return;
    }
    
    line += (currentchr == '\n');
    sz_read = 1;
    currentchr = (*buffer)[b_pos++];
    c_pos++;
}

#ifdef WIN32
template<> void tokenizer_result<wstring>::getnext() {
    if (b_pos >= buffer_size) {
        currentchr =  0;
        return;
    }
    
    line += (currentchr == '\n');
    sz_read = 1;
    if (c_utf16_to_unicode(currentchr, (*buffer)[b_pos++], false)) {
        sz_read = 2;
        c_utf16_to_unicode(currentchr, (*buffer)[b_pos++], true);
    }
    c_pos++;
}
#else
template<> void tokenizer_result<wstring>::getnext() {
    if (b_pos >= buffer_size) {
        currentchr =  0;
        return;
    }
    
    line += (currentchr == '\n');
    sz_read = 1;
    currentchr = (*buffer)[b_pos++];
    c_pos++;
}
#endif

template<> void tokenizer_result<u16string>::getnext() {
    if (b_pos >= buffer_size) {
        currentchr =  0;
        return;
    }
    
    line += (currentchr == '\n');
    sz_read = 1;
    if (c_utf16_to_unicode(currentchr, (*buffer)[b_pos++], false)) {
        sz_read = 2;
        c_utf16_to_unicode(currentchr, (*buffer)[b_pos++], true);
    }
    c_pos++;
}

template<> void tokenizer_result<string>::getnext() {
    if (b_pos >= buffer_size) {
        currentchr =  0;
        return;
    }
    
    line += (currentchr == '\n');
    sz_read = 1 + c_utf8_to_unicode(buffer, b_pos, currentchr);
    b_pos += sz_read;
    c_pos++;
}


template<> void tokenizer_result<u_ustring>::store_currentchr() {
    stack_ptr->push_back(u_ustring(1, currentchr));
    if (store_all) {
        stacktype.push_back(0);
        stackln.push_back(line);
        bpos.push_back(b_start);
        cpos.push_back(c_start);
    }
    b_start = b_pos;
    c_start = c_pos;
}

#ifdef WIN32
template<> void tokenizer_result<wstring>::store_currentchr() {
    uint32_t c;
    wstring w;
    if (c_unicode_to_utf16(c, (uint32_t)currentchr)) {
        w = (wchar_t)(c >> 16);
        w += (wchar_t)(c & 0xFFFF);
    }
    else
        w = (wchar_t)c;

    
    stack_ptr->push_back(w);
    if (store_all) {
        stacktype.push_back(0);
        stackln.push_back(line);
        bpos.push_back(b_start);
        cpos.push_back(c_start);
    }
    b_start = b_pos;
    c_start = c_pos;
}
#else
template<> void tokenizer_result<wstring>::store_currentchr() {
    stack_ptr->push_back(wstring(1, currentchr));
    if (store_all) {
        stacktype.push_back(0);
        stackln.push_back(line);
        bpos.push_back(b_start);
        cpos.push_back(c_start);
    }
    b_start = b_pos;
    c_start = c_pos;
}
#endif

template<> void tokenizer_result<std::u16string>::store_currentchr() {
    uint32_t c;
    u16string w;
    if (c_unicode_to_utf16(c, (uint32_t)currentchr)) {
        w = (char16_t)(c >> 16);
        w += (char16_t)(c & 0xFFFF);
    }
    else
        w = (char16_t)c;
        
    
    stack_ptr->push_back(w);
    if (store_all) {
        stacktype.push_back(0);
        stackln.push_back(line);
        bpos.push_back(b_start);
        cpos.push_back(c_start);
    }
    b_start = b_pos;
    c_start = c_pos;
}

template<> void tokenizer_result<string>::store_currentchr() {
    string w = c_unicode_to_utf8(currentchr);
    stack_ptr->push_back(w);
    if (store_all) {
        stacktype.push_back(0);
        stackln.push_back(line);
        bpos.push_back(b_start);
        cpos.push_back(c_start);
    }
    b_start = b_pos;
    c_start = c_pos;
}


template<> void tokenizer_result<u_ustring>::store(int32_t label) {
    if (label != -1) {
        if (check_label(token, label))
            return;
        stack_ptr->push_back(token);
        token = U"";
        if (store_all) {
            stacktype.push_back((unsigned char)label);
            stackln.push_back(line);
            bpos.push_back(b_start);
            cpos.push_back(c_start);
        }
    }
}

#ifdef WIN32
template<> void tokenizer_result<wstring>::store(int32_t label) {
    if (label != -1) {
        if (check_label(token, label))
            return;
        wstring w;
        s_unicode_to_utf16(w, token);
        stack_ptr->push_back(w);
        token = U"";
        if (store_all) {
            stacktype.push_back((unsigned char)label);
            stackln.push_back(line);
            bpos.push_back(b_start);
            cpos.push_back(c_start);
        }
    }
}
#else
template<> void tokenizer_result<wstring>::store(int32_t label) {
    if (label != -1) {
        if (check_label(token, label))
            return;
        stack_ptr->push_back((wstring&)token);
        token = U"";
        if (store_all) {
            stacktype.push_back((unsigned char)label);
            stackln.push_back(line);
            bpos.push_back(b_start);
            cpos.push_back(c_start);
        }
    }
}
#endif

template<> void tokenizer_result<u16string>::store(int32_t label) {
    if (label != -1) {
        if (check_label(token, label))
            return;
        u16string w;
        s_unicode_to_utf16(w, token);
        stack_ptr->push_back(w);
        token = U"";
        if (store_all) {
            stacktype.push_back((unsigned char)label);
            stackln.push_back(line);
            bpos.push_back(b_start);
            cpos.push_back(c_start);
        }
    }
}

template<> void tokenizer_result<string>::store(int32_t label) {
    if (label != -1) {
        if (check_label(token, label))
            return;
        string w;
        s_unicode_to_utf8(w, token);
        stack_ptr->push_back(w);
        token = U"";
        if (store_all) {
            stacktype.push_back((unsigned char)label);
            stackln.push_back(line);
            bpos.push_back(b_start);
            cpos.push_back(c_start);
        }
    }
}

//------------------------------------------------------------
//Display a rule as an automaton
//------------------------------------------------------------

void tokenizer_automaton::display(tokenizer_node* a, int nbblanks, bool top) {
    static std::set<int16_t> found;
    //We start from initial
    if (top) {
        found.clear();
    }
    //This node was already in our set
    bool ret = false;
    if (found.count(a->idpos))
        ret = true;
    else
        found.insert(a->idpos);
    
    string blanks(nbblanks, ' ');
    cout << blanks;
    
    if (a->check_negation())
        cout << "~";
    if (a->check_skip())
        cout << '-';
    if (a->check_start_fail()) {
        if (a->check_fail())
            cout << "<BE>";
        else
            cout << "<B>";
    }
    else {
        if (a->check_fail())
            cout << "<E>";
    }
    
    if (a->action == act_meta) {
        cout << '%';
    }
    
    if (a->action == act_end) {
        if (a->label < 33)
            cout << '$' << (int)a->label;
        else
            cout << '$' << (uchar)a->label;
    }
    else {
        if (!a->label) {
            if (a->action == act_any)
                cout << "ANY";
            else
                cout << "EPSILON";
        }
        else {
            if (a->label < 33)
                cout << (int)a->label;
            else
                cout << (uchar)a->label;
            if (a->endlabel) {
                if (a->endlabel < 33)
                    cout << "-" << (int)a->endlabel;
                else
                    cout << "-" << (uchar)a->endlabel;
            }
        }
    }
    
    if (ret && a->action != act_end) {
        cout << "_SEE_" << a->idpos << endl;
        return;
    }
    
    cout << '_' << a->idpos << endl;

    //we explore the subarcs to give them a position
    for (long i = 0; i < a->arcs.size(); i++) {
        display(a->arcs[i], nbblanks + 2, false);
    }
    if (top)
        cout << endl;
}
//------------------------------------------------------------
//The Rule Compiler
//------------------------------------------------------------

long tokenizer_automaton::compile() {
    /*
     
     The rules are compiled into tokenizer_node arcs and the results are stored in vectorization_table.
     This table contains: 1611 elements (see table_character_size).
     
     When a character code is < 1611, then the associated rules are stored in vectorization_table at character code location.
     For instance, if a rule starts with "A", then the rule will be stored at position 65 (ASCII code for "A")in vectorization_table.
     
     For rules that cannot be indexed, either because they start with an epsilon or because the initial character is > 1611, the
     rule is then stored at position 0.
     
     We have different potential action associated with an arc
     act_char             -> regular character
     act_meta             -> meta-character (%x)
     act_interval         -> interval between two characters: #A-F
     act_any              -> any character
     act_epsilon          -> an epsilon arc, which has created to handle disjunction or optionalaties
     act_end              -> the termination act, associated with a rule identifier, the returned value when the rule has applied
     
     We also have different flags to handle the node status
     flag_negation (1)        -> negation (~)
     flag_skip (2)            -> Character not stored (-)
     flag_fail (4)            -> Set to Element in negated disjunctions. When element is true, rule fails thank to this flag.
     flag_start_fail (8)      -> Set the initial element of a negated disjunction element. Different from flag_fail only for sequences
     flag_gate (16)           -> When this flag is activated, the path always fails
     flag_action (31)          -> flag_negation | flag_skip | flag_fail | flag_start_fail | flag_gate | flag_lisp
     
     //The next flags are used to mark node utilisation
     flag_vectorized (32)   //has been added to vectorization_table as an index based on the first value of the rule
     flag_added (64)       //has been added to vectorization_table at position 0
     flag_postpone (128)    //While removing epsilon, a node that has already been seen but not fully evaluated since
     flag_visited (256)     //A node that has already been visited
     flag_seen (512)       //A node that has already been seen when processing postponed nodes

     -> IMPORTANT: for meta-characters such as %a or %d, we also use each possible value for these meta-characters as index in vectorization_table.
     */
    
    //The meta-characters for which there is a specific semantics
    static char x_actions[]="CHSacdhnoprs";
    
    lisp_detection = false;
    
    map<u_ustring, u_ustring> metalines;
    vector<tokenizer_node*> brackets;
    vector<char> currentbracket;
    
    u_ustring line;
    u_ustring action_id;
    
    tokenizer_node* anode = NULL;
    tokenizer_node* current = NULL;
    tokenizer_node* root = NULL;
    
    u_uchar e;
    u_uchar cc;
    
    int32_t irule, final_action = -1;
    long pos;
    
    bool metarule_available = false;
    bool error = false;
    long begin_nodes;
    bool first;
    bool first_value;
    
    bool tail = false;
    
    
    for (irule = 0; irule < rules.size() && !error; irule++) {
        tail = false;
        line = rules[irule];
        begin_nodes = nodes.size();
        
        if (line[1]==':') { //we detect a meta-rule...
            cc = line[0];
            line=line.c_str()+2;
            action_id = U"%";
            action_id += cc;
            if (metarule_available)
                replacemetas(metalines,line);
            metalines[action_id] = line;
            metarule_available = true;
            continue;
        }
        
        anode = NULL;
        current = NULL;
        disjunction = false;
        initial = NULL;
        root = NULL;
        //We need a dummy value in currentbracket
        currentbracket.clear();
        currentbracket.push_back('*');
        
        //first we look for the = sign at the end of the string...
        
        pos = line.rfind(U"=",line.size()-1);
        if (pos == -1)
            error = true;
        else {
            // =: forces tail recursion
            if (line[pos + 1] == ':') {
                tail = true;
                pos++;
            }
                
            action_id = line.c_str()+pos+1;
            
            switch (action_id[0]) {
                case '!':
                    final_action = -2;
                    break;
                case '#':
                    final_action = -1;
                    break;
                case 'L': //for Lisp detection mode (£)
                    final_action = def_lisp_mode;
                    lisp_detection = true;
                    break;
                default:
                    final_action = convert(action_id);
            }
            
            if (tail)
                line=line.substr(0,pos - 1);
            else
                line=line.substr(0,pos);
            
            //We have some meta rules
            if (metarule_available)
                replacemetas(metalines,line);
        }
        
        first = true;
        first_value = false;
        for (pos = 0; pos < line.size() && !error; pos++) {
            cc = line[pos];
            if (!currentbracket.size()) {
                error = true;
                break;
            }
            
            switch(cc) {
                case '%':
                    //We record the first rule not starting with a character
                    cc = line[pos+1];
                    
                    if (first) {
                        //Note: current = append(current, anode) might seem redundant
                        //but it is important to call it when in a disjunction, as when
                        //the disjunction is the first element, we need to vectorize
                        //for each element that it contains. In that case, append will push
                        //the value in current and returns again current as the main root.
                        switch (cc) {
                            case 'C': //Upper-case characters
                                anode = node(act_meta, cc);
                                for (e = 'A'; e < table_character_size; e++) {
                                    if (c_is_upper(e))
                                        vectorization(anode, e);
                                }
                                current = append(current, anode);
                                break;
                            case 'H':
                                anode = node(act_meta, cc);
                                current = append(current, anode);
                                break;
                            case 'S': //table space or CR
                                anode = node(act_meta, cc);
                                vectorization(anode, 9);
                                vectorization(anode, 10);
                                vectorization(anode, 13);
                                vectorization(anode, 32);
                                vectorization(anode, 160);
                                vectorization(anode, 0); //for other non-breaking spaces
                                current = append(current, anode);
                                break;
                            case 'a': //Alphabetical characters
                                anode = node(act_meta, cc);
                                vectorization(anode, '_');
                                for (e = 'A'; e < table_character_size; e++) {
                                    if (c_is_alpha(e))
                                        vectorization(anode, e);
                                }
                                current = append(current, anode);
                                break;
                            case 'c': //Lower-case characters
                                anode = node(act_meta, cc);
                                for (e = 'a'; e < table_character_size; e++) {
                                    if (c_is_lower(e))
                                        vectorization(anode, e);
                                }
                                current = append(current, anode);
                                break;
                            case 'd':
                                anode = node(act_meta, cc);
                                for (e = '0'; e <= '9'; e++)
                                    vectorization(anode, e);
                                current = append(current, anode);
                                break;
                            case 'h': //Greek characters
                                anode = node(act_meta, cc);
                                for (e = 913; e <= 987; e++) {
                                    if (c_is_alpha(e))
                                        vectorization(anode, e);
                                }
                                current = append(current, anode);
                                break;
                            case 'n':
                                anode = node(act_meta, cc);
                                vectorization(anode, 160);
                                //there are more than one non breaking space
                                vectorization(anode, 0);
                                current = append(current, anode);
                                break;
                            case 'o': {
                                anode = node(act_meta, cc);
                                for (const auto& a : operators)
                                    vectorization(anode, a);
                                current = append(current, anode);
                                break;
                            }
                            case 'p': {
                                anode = node(act_meta, cc);
                                for (const auto& a: vpunctuations())
                                    vectorization(anode, a);
                                current = append(current, anode);
                                break;
                            }
                            case 'r': //CR
                                anode = node(act_meta, cc);
                                vectorization(anode, 10);
                                vectorization(anode, 13);
                                current = append(current, anode);
                                break;
                            case 's': //tab or space or non-breaking spaces
                                anode = node(act_meta, cc);
                                vectorization(anode, 9);
                                vectorization(anode, 32);
                                vectorization(anode, 160);
                                vectorization(anode, 0); //for other non-breaking space characters
                                current = append(current, anode);
                                break;
                            default:
                                anode = node(act_char, cc);
                                vectorization(anode, cc);
                                current = append(current, anode);
                        }
                        pos++;
                        break;
                    }
                    
                    //This is a direct comparison or a meta
                    if (strchr(x_actions,cc) == NULL)
                        anode = node(act_char, cc);
                    else
                        anode = node(act_meta, cc);
                    
                    root = current;
                    current = append(current, anode);
                    pos++;
                    break;
                case '(':
                    //The first element of a rule can never be optional
                    if (first) {
                        currentbracket.push_back('^');
                        first_value = true;
                    }
                    
                    anode = node(act_epsilon, 0);
                    current = append(current, anode);
                    brackets.push_back(current);
                    currentbracket.push_back('(');
                    disjunction = false;
                    break;
                case ')':
                    if (brackets.size()) {
                        root = brackets.back();
                        brackets.pop_back();
                        anode = node(act_epsilon, 0);
                        root->append(anode);
                        current = append(current, anode);
                        currentbracket.pop_back();
                        first_value = false;
                        if (currentbracket.back() == '^') {
                            currentbracket.pop_back();
                            first_value = true;
                        }
                        disjunction = (currentbracket.back() == '{');
                        if (first_value) {
                            first_value = false;
                            first = true;
                            continue;
                        }
                    }
                    else
                        error = true;
                    break;
                case '[': { //recording a sequence
                    //This is a sequence of elements
                    //If we are in a disjunction, then first_value might be true
                    //if this disjunction starts a rule.
                    //We add a specific marker to return to the right value
                    //once the structure analysis is completed.
                    if (first_value)
                        currentbracket.push_back('^');
                    currentbracket.push_back('[');
                    brackets.push_back(current);
                    current = node(act_epsilon, 0);
                    brackets.push_back(current);
                    disjunction = false;
                    break;
                }
                case ']':
                    if (brackets.size()) {
                        anode = brackets.back();
                        brackets.pop_back();
                        current = brackets.back();
                        brackets.pop_back();
                        if (current->check_negation()) {
                            //We mark the last node as a fail arc
                            current->fail_arcs();
                            anode = node(act_any, 0);
                            current->append(anode);
                        }
                        
                        root = current;
                        current->append(anode->arcs[0]);
                        currentbracket.pop_back();
                        if (currentbracket.back() == '^') {
                            //we were in a disjunction, we need to put first_value back to true
                            currentbracket.pop_back();
                            first_value = true;
                        }
                        disjunction = (currentbracket.back() == '{');
                    }
                    else
                        error = true;
                    break;
                case '{':
                    anode = node(act_epsilon, 0);
                    disjunction = false;
                    current = append(current, anode);
                    brackets.push_back(current);
                    disjunction = true;
                    currentbracket.push_back('{');
                    //In this case, we need to dispatch elements to vectorize
                    if (first)
                        first_value = true;
                    break;
                case '}':
                    if (brackets.size()) {
                        first_value = false;
                        disjunction = false;
                        current = brackets.back();
                        brackets.pop_back();
                        root = current;
                        if (current->check_negation()) {
                            current->fail_arcs();
                            anode = node(act_any, 0);
                            current->append(anode);
                        }
                        
                        cc = line[pos+1];
                        if (cc != '+' && cc != '*') {
                            anode = node(act_epsilon, 0);
                            for (e = 0; e < current->size(); e++) {
                                current->arcs[e]->append_final(anode);
                            }
                            if (current->check_negation())
                                current->append(anode);
                            current = anode;
                        }
                        
                        currentbracket.pop_back();
                        disjunction = (currentbracket.back() == '{');
                    }
                    else
                        error = true;
                    break;
                case '+':
                    if (pos) { //only if it is not the first character
                        //append_final appends first current, then anode
                        //on each final nodes in current (to take into account [...])
                        if (check_flag(current->action, act_epsilon)) {
                            anode = node(act_epsilon, 0);
                            for (e = 0; e < current->size(); e++) {
                                current->arcs[e]->append_final(current, anode);
                            }
                            if (current->arcs[0]->check_start_fail())
                                current->append(anode);
                        }
                        else {
                            anode = node(act_epsilon, 0);
                            current->append(current);
                            current = append(current, anode);
                        }
                        current = anode;
                        break;
                    }
                    else
                        error = true;
                    break;
                case '*':
                    if (pos) { //only if it is not the first character
                        //append_final appends first current, then anode
                        //on each final nodes in current (to take into account [...])
                        //We need to add one more layer of node
                        if (root == NULL) {
                            error = true;
                            break;
                        }
                        
                        if (check_flag(current->action, act_epsilon)) {
                            anode = node(act_epsilon, 0);
                            for (e = 0; e < current->size(); e++) {
                                current->arcs[e]->append_final(current, anode);
                            }
                            if (current->arcs[0]->check_start_fail())
                                current->append(anode);
                            current = anode;
                        }
                        else {
                            anode = node(act_epsilon, 0);
                            current->append(current);
                            current = append(current, anode);
                            current = anode;
                        }
                        //We add an arc to skip the whole structure since *
                        //also means 0 case...
                        root->append(anode);
                        break;
                    }
                    else
                        error = true;
                    break;
                case '#': //introduce an interval or a code
                    //interval
                    if (line[pos+2] == '-') {
                        anode = node(act_interval, line[pos+1], line[pos+3]);
                        if (first) {
                            for (u_uchar c = line[pos+1]; c <= line[pos+3]; c++) {
                                vectorization(anode, c);
                            }
                        }
                        root = current;
                        current = append(current, anode);
                        pos += 3;
                    }
                    else {//code
                        e = 0;
                        if (isdigit(line[pos+1])) {
                            pos++;
                            e = line[pos++] - 48;
                            while (pos < line.size() && isdigit(line[pos])) {
                                e *= 10;
                                e += line[pos++] - 48;
                            }
                            pos--;
                            anode = node(act_char, e);
                            if (first)
                                vectorization(anode, e);
                            root = current;
                            current = append(current, anode);
                        }
                        else
                            error = true;
                    }
                    break;
                case '-':
                    if (pos)
                        add_flag(current->flags, flag_skip);
                    else
                        error = true;
                    break;
                case '~':
                    negation = flag_negation;
                    break;
                case '?':
                    anode = node(act_any, cc);
                    root = current;
                    current = append(current, anode);
                    break;
                case ' ': //in disjunction we separate each element with a space
                    if (currentbracket.back() == '{')
                        continue;
                default: {
                    //Elements are separated with blanks in disjunction
                    anode = node(act_char, cc);
                    if (first)
                        vectorization(anode , cc);
                    root = current;
                    current = append(current, anode);
                }
            }
            first = first_value;
            //In this case, we need to index on the first element
            //not the following one...
            if (first_value && (currentbracket.back() == '[' || currentbracket.back() == '('))
                first_value = false;
        }
        
        if (initial == NULL || error || brackets.size())
            return irule;
        
        if (final_action == -2) {
            final_action = 0;
            if (action_id.size() > 1) {
                action_id = action_id.substr(1, action_id.size());
                final_action = convert(action_id);
            }
            add_flag(initial->flags, flag_blocking_gate);
            //This is a gated mechanism. When a rule applies that returns L (see above)
            //Then the Lisp quote reading is activated. Since, this is now the first rule for quotes
            //then quotes will be handled in the Lisp way. We use select_lisp_mode to activate or
            //deactivate the Lisp quote rule, by adding or removing the flag_gate flag.
            if (lisp_detection && initial->label == '\'' && vectorization_table[39]->arcs[0] == initial)
                lisp_quote = initial;
        }
            
        anode = node(act_end, final_action);
        append(current, anode);
                    
        initial->trim_epsilon(nodes);
        if (final_action >= 0)
            indexed_on_label[final_action].push_back(initial);

        if (tail) {
            anode = initial;
            while (anode->arcs.size() == 1)
                anode = anode->arcs[0];
            add_flag(anode->flags, flag_tail);
        }
    }
    
    //Cleaning useless nodes
    //Removing all marks except flag_negation, flag_skip and flag_fail
    for (auto& nd : nodes)
        nd->flags &= flag_action;
    
    //When there is only one rule in vectorization_table, no need to keep the epsilon
    vectorization_table[0]->mark_nodes();
    
    //the position 0 can never be removed, thus we start at 1
    for (e = 1; e < table_character_size; e++) {
        if (vectorization_table[e] != NULL) {
            if (vectorization_table[e]->size() == 1)
                vectorization_table[e] = vectorization_table[e]->arcs[0];
            vectorization_table[e]->mark_nodes();
        }
    }
    
    vector<tokenizer_node*> intermediary;
    for (auto& nd : nodes) {
        if (!nd->check_visited())
            delete nd;
        else {
            nd->idpos = intermediary.size();
            intermediary.push_back(nd);
        }
    }
    
    nodes = intermediary;
    loaded = true;
    return -1;
}

//------------------------------------------------------------
//Cleaning epsilon nodes in tokenize automata
//------------------------------------------------------------

//These nodes have been detected as sub-nodes of themselves. Basically, it is a loop generated out of a Kleene operator (a loop)
void tokenizer_node::processing_postponed_nodes(std::set<long>& visited, vector<tokenizer_node*>& nodes, long idpos) {
    if (check_seen())
        return;
    
    add_flag(flags, flag_seen);
    
    long found = -1;
    for (long i = 0; i < arcs.size(); i++) {
        if (arcs[i]->idpos == idpos) {
            found = i;
        }
        else
            arcs[i]->processing_postponed_nodes(visited, nodes, idpos);
    }

    if (found != -1) {
        vector<tokenizer_node*> local;
        std::set<long> v;
        v.insert(idpos);
        for (long i = 0; i < arcs.size(); i++) {
            if (i == found) {
                for (auto& e : visited) {
                    if (v.find(e) != v.end())
                        continue;
                    local.push_back(nodes[e]);
                    v.insert(e);
                }
            }
            else {
                if (v.find(arcs[i]->idpos) == v.end()) {
                    local.push_back(arcs[i]);
                    v.insert(arcs[i]->idpos);
                }
            }
        }
        arcs = local;
    }
}

//We are looking for epsilon nodes that can be merged...
//When we find an epsilon nodes, we go down in recursion to gather all existing nodes that are not epsilon down the way
//We then replace these epsilon nodes with these nodes.
//When we have an epsilon node that points to a termination node, this epsilon node becomes the new termination node.
void tokenizer_node::remove_epsilon_nodes(std::unordered_map<long,std::set<long> >& visited,
                                          std::set<long>& current,
                                          vector<tokenizer_node*>& nodes, bool epsilon) {
    if (check_visited())
        return;

    add_flag(flags, flag_visited);
    
    std::set<long> locals;
    std::set<long>* vect;
    tokenizer_node* n;
    long i;
    
    long sz = size();
    
    if (sz) {
        //We merge the arcs that share the same definition
        long key;
        std::unordered_map<long, tokenizer_node*> commons;
        vector<tokenizer_node*> nds;
        for (i = 0; i < sz; i++) {
            n = arcs[i];
            if (n->pure_arc()) {
                key = n->action*256 + n->label;
                if (commons.find(key) != commons.end()) {
                    tokenizer_node* nbase = commons[key];
                    for (key = 0; key < n->size(); key++)
                        nbase->arcs.push_back(n->arcs[key]);
                }
                else {
                    commons[key] = n;
                    nds.push_back(n);
                }
            }
            else
                nds.push_back(n);
        }
        //We have found some common arcs
        if (nds.size() != sz) {
            arcs.clear();
            arcs = nds;
            sz = arcs.size();
        }
    }

    if (epsilon)
        vect = &current;
    else
        vect = &locals;
    
    for (i = 0; i < sz; i++) {
        n = arcs[i];
        if (n->is_epsilon()) {
            if (n->size() == 1 && n->arcs[0]->action == act_end) {
                add_flag(n->arcs[0]->flags, flag_visited);
                n->clone(n->arcs[0]);
                n->arcs.clear();
                vect->insert(n->idpos);
            }
            else {
                if (n->check_visited()) {
                    if (visited.find(n->idpos) != visited.end()) {
                        for (auto& e: visited[n->idpos])
                            vect->insert(e);
                    }
                    else {
                        add_flag(n->flags, flag_postpone);
                        vect->insert(n->idpos);
                    }
                }
                else {
                    n->remove_epsilon_nodes(visited, *vect, nodes, true);
                    visited[n->idpos] = *vect;
                    if (n->check_postpone() && visited.find(n->idpos) != visited.end())
                        n->processing_postponed_nodes(visited[n->idpos], nodes, n->idpos);
                }
            }
        }
        else {
            vect->insert(n->idpos);
            n->remove_epsilon_nodes(visited, *vect, nodes, false);
        }
    }
    
    if (locals.size() != 0) {
        arcs.clear();
        for (auto& e : locals) {
            n = nodes[e];
            if (n->pure_arc())
                arcs.push_back(n);
        }
        
        for (auto& e : locals) {
            n = nodes[e];
            if (!n->pure_arc())
                arcs.push_back(n);
        }
        
        for (auto& e : locals) {
            n = nodes[e];
            if (n->check_postpone() && visited.find(n->idpos) != visited.end())
                processing_postponed_nodes(visited[n->idpos], nodes, n->idpos);
        }
    }
}


void tokenizer_automaton::setrules() {
    /*
     a) A metarule is composed of two parts: c:expression, where c is the metacharacter that be accessed through %c and expression is a single body rule.
     
     for instance, we could have encoded %o as:
     rules.push_back(U"o:[≠ ∨ ∧ ÷ × ² ³ ¬]");
     
     IMPORTANT: These rules should be declared with one single operation.
     Their body will replace the call to a %c in other rules  (see the test on metas in the parse section)
     
     If you use a character that is already a meta-character (such as "a" or "d"), then the meta-character will be replaced with
     this new description... However, its content might still use the standard declaration:
     
     rules.push_back(U"a:{%a %d %p}"); "%1 is now a combination of alphabetical characters, digits and punctuations
     
     
     b) A rule is composed of two parts: body = action (action is either an integer or a #.)
     
     body uses the following instructions:
     
     x   is a character that should be recognized
     
     #x     comparison with character of code x...
     #x-y   comparison between x and y. x and y should be ascii characters...
     
     %x  is a meta-character with the following possibilities:
     
     ?  is any character
     %a  is any alphabetical character (including unicode ones such as éè)
     %C  is any uppercase character
     %c  is any lowercase character
     %d  is any digits
     %h  is a Greek letter
     %H  is any hangul character
     %n  is a non-breaking space
     %o  is any operators
     %p  is any punctuations
     %r  is a carriage return both \n and \r
     %s  is a space (32) or a tab (09)
     %S  is both a carriage return or a space (%s or %r)
     
     %nn  you can create new metarules associated with any OTHER characters...
     
     
     (..) is a sequence of optional instructions
     [..] is a sequence of characters in a disjunction
     {..} is a disjunction of meta-characters
     x*   means that the instruction can be repeated zero or n times
     x+   means that the instruction can be repeated at least once
     x-   means that the character should be recognized but not stored in the parsing string
     ~..  means that all character will be recognized except for those in the list after the tilda.
     
     IMPORTANT: spaces are considered characters except in disjunctions
     
     
     Final Action
     ------------
     When a rule applies it returns its final action value, which is usually a numerical value.
     However, there are some special values:
     
     £ : These are characters that trigger the Lisp mode. This is a case when quotes do not encapsulate strings anymore
     !value : Use to add gate keeping in the automatin. For instance, the rule for single quotes is activated.
     #: The rule applies but nothing is stored in the stacks
     
     */
    
    //Spaces, skipped in the parsing string
    rules.push_back(U"%S+=#");                         //0     space (not kept)
    
    
    rules.push_back(U"\\%(=L");  //detecting Lisp Expression
    rules.push_back(U"\\'%(=L");

    //Fast tracks for recurrent punctations
    rules.push_back(U";=0");                         //4     ;
    rules.push_back(U",=0");                         //5     ,
    rules.push_back(U"==0");                         //6     =
    rules.push_back(U"!=0");                         //8     !

    //Special characters that need to be escaped
    rules.push_back(U"%(=0");                         //9     (
    rules.push_back(U"%)=0");                         //10    )
    rules.push_back(U"%[=0");                         //11    [
    rules.push_back(U"%]=0");                         //12    ]
    rules.push_back(U"%{=0");                         //13    {
    rules.push_back(U"%}=0");                         //14    }
    rules.push_back(U"%%=0");                         //21    %
    rules.push_back(U"%?=0");                         //29    ?
    rules.push_back(U"%+=0");                         //18    +
    rules.push_back(U"%-=0");                         //19    -
    rules.push_back(U"%*=0");                         //20    *
    rules.push_back(U"%~=0");                         //7     ~
    rules.push_back(U"%#=0");                         //28    #

    rules.push_back(U"..=0");                        //15    ..
    rules.push_back(U".=0");                         //16    .
    rules.push_back(U"^=0");                         //17    ^
    rules.push_back(U"<=0");                         //22    <
    rules.push_back(U">=0");                         //23    >
    rules.push_back(U"|=0");                         //24    |
    rules.push_back(U"&=0");                         //25    &
    rules.push_back(U":=0");                         //26    :
    rules.push_back(U"$=0");                         //27    $
    rules.push_back(U"\\=0");                        //29    ?

    //Comments
    rules.push_back(U"//?*%r=#");                  //30    comments starting with // with no carriage return (CR) inside (not kept)
    rules.push_back(U"/@?*@/=#");                   //32    long comments starting with /@ and finishing with @/ (not kept)
    rules.push_back(U"/=0");                         //34    /
    
    //Strings
    //Double quote
    rules.push_back(U"\"{[\\-\"] ~%r}*\"=:1");     //string "" does not contain CR and can escape characters

    //Single quote
    rules.push_back(U"'=!");                         //4 In the case of Lisp, this rule is activated
    rules.push_back(U"'~%r*'=:2");                    //38    string '' does not contain CR and does not process escape characters
    
    //Long quotes
    rules.push_back(U"@-\"?*\"@-=5");               //40    string @" "@ can contain CR and escape characters (we do not keep the @s)
    
    //tamgu regular expression strings
    rules.push_back(U"r-\"~%r+\"=9");              //42    string r"" tamgu regular expression (we do not keep the r in the parse)
    rules.push_back(U"r-'~%r+'=10");               //42    string r"" tamgu regular expression (we do not keep the r in the parse)
    rules.push_back(U"p-\"~%r+\"=11");             //42    string p"" tamgu posix expression (we do not keep the p in the parse)
    rules.push_back(U"p-'~%r+'=12");               //42    string p"" tamgu posix expression (we do not keep the p in the parse)
    rules.push_back(U"f-\"~%r+\"=13");              //42    string f"" tamgu format expression
    rules.push_back(U"f-'~%r+'=14");              //42    string f'' tamgu format expression

    //Unicode double quote strings
    rules.push_back(U"u-\"{[\\-\"] ~%r}*\"=1");     //string "" does not contain CR and can escape characters
    
    //Unicode single quote strings
    rules.push_back(U"u-'~%r*'=7");                //44    string u'' unicode string
    
    //Unicode long quote strings
    rules.push_back(U"u-@-\"?+\"@-=8");             //46    string u@".."@ unicode string
    
    rules.push_back(U"0b{1 0}+=3");                       //binary numbers
    rules.push_back(U"0c({%- %+})%d+(.%d+)({eE}({%- %+})%d+):({%- %+})(%d+(.%d+)({eE}({%- %+})%d+))i=3");    //complex numbers

    rules.push_back(U"1:{%d #A-F #a-f}");            //2     metarule on 1, for hexadecimal digits
    rules.push_back(U"0x%1+(.%1+)({pP}({%- %+})%d+)=3");  //47 hexadecimal: can handle 0x1.16bca4f9165dep-3
    
    rules.push_back(U"%d+(.%d+)({eE}({%- %+})%d+)=3");    //48    exponential digits

    // Rules start here
    //This character should be interpreted as one
    rules.push_back(U"{%a %d %H}+=4");               //49    label a combination of alpha, digits and hangul characters
    rules.push_back(U"%n=#");                        //1     non-breaking space (not kept)
    rules.push_back(U"%o=0");                        //51    operators
    rules.push_back(U"%p=0");                        //50    punctuation
    rules.push_back(U"~{%S %p %o}+=4");            //52    An unknown UTF8 token separated with spaces, punctuation or operators...
}


void segmenter_automaton::setrules() {
    rules.push_back(U"#10=!99");                                  //gated arcs to keep carriage or not
    rules.push_back(U"%S+=#");                                  //we skip all spaces
    

    //regular numbers
    rules.push_back(U"%d+{[rd][th][nd][er][ième][ieme]}=1");    //3rd or 4th
    rules.push_back(U"%d(%d)(:%d%d){[am][pm]}=1");              //American hours 6:30am, 9pm
    rules.push_back(U"%d(%d){:h}%d%d=1");                       //European hours 6h30, 21:00

    rules.push_back(U"{%- %+}0b{1 0}+=1");  //binaries
    rules.push_back(U"0b{1 0}+=1");  //binaires
    rules.push_back(U"0c({%- %+})%d+(.%d+)({eE}({%- %+})%d+):({%- %+})(%d+(.%d+)({eE}({%- %+})%d+))i=1");    //complex numbers

    //Tokenizing numbers
    //22 for blocs of 3, with a comma
    //44 for blocs of 3, with a point or a space
    //rules are either 22 or 44 never both
    //33 using a decimal point
    //66 using a decimal comma
    //rules are either 33 or 66 never both
    //The order of the rules is important, if blocs of 3 are to be taken into account
    rules.push_back(U"{%- %+}%d+[,%d%d%d]+=22");          //digits separated with a , by block of 3
    rules.push_back(U"{%- %+}%d+[{.#32}%d%d%d]+=!44");    //digits separated with a . or a space by block of 3
    rules.push_back(U"%d+[,%d%d%d]+=22");  //digits separated with a , by block of 3
    rules.push_back(U"%d+[{.#32}%d%d%d]+=!44"); //digits separated with a . or a space by block of 3

    //We put this meta-rule here to avoid checking all the rules belows with it
    rules.push_back(U"1:{%d #A-F #a-f}");                    //metarule on 1, for hexadecimal digits

    rules.push_back(U"{%- %+}0x%1+(.%1+({pP}({%- %+})%d+))=33");  //hexadecimal with a decimal point
    rules.push_back(U"{%- %+}0x%1+(,%1+({pP}({%- %+})%d+))=!66"); //Gated: hexadecimal with a decimal comma
    rules.push_back(U"{%- %+}%d+(.%d+({eE}({%- %+})%d+))=33");    //Numbers with decimal point
    rules.push_back(U"{%- %+}%d+(,%d+({eE}({%- %+})%d+))=!66"); //Gated: Numbers with decimal comma

    rules.push_back(U"0x%1+(.%1+({p P}({%- %+})%d+))=33");   //hexadecimal
    rules.push_back(U"0x%1+(,%1+({p P}({%- %+})%d+))=!66"); //Gated rules
    rules.push_back(U"%d+(.%d+({eE}({%- %+})%d+))=33");      //digits
    rules.push_back(U"%d+(,%d+({eE}({%- %+})%d+))=!66"); //Gated rules

    rules.push_back(U"%#{%a %d}+=1");       //Regular strings
    rules.push_back(U"${%a %d}+=1");       //Regular strings
    
    rules.push_back(U"http(s)://{%a %d . = %# & %? / %- %+}+=1");       //http
    rules.push_back(U"{%a %d . %- %+}+@{%a %d . = & %? %# %- %+}+=1");       //mail address
    
    rules.push_back(U"%o=1");                 //operator
    rules.push_back(U"%p=1");                  //punctuation
    rules.push_back(U"%H{%H %d}*=1");       //Asian characters (Chinese, Korean, Japanese)
    rules.push_back(U"%h{%h %- %d}*=1");       //Greek
    rules.push_back(U"%a{%a %- '’ %d}*=1");       //Regular strings
    rules.push_back(U"~{%S %o %p}+=1");           //Any combination of Unicode characters ending at a space or a punctuation
}

void tags_automaton::setrules() {
    rules.push_back(U"#10=!99");                                  //gated arcs to keep carriage or not
    rules.push_back(U"%S+=#");                                  //we skip all spaces
    
    //regular numbers
    rules.push_back(U"%d+{[rd][th][nd][er][ième][ieme]}=4");    //3rd or 4th
    rules.push_back(U"%d(%d)(:%d%d){[am][pm]}=4");              //American hours 6:30am, 9pm
    rules.push_back(U"%d(%d){:h}%d%d=4");                       //European hours 6h30, 21:00
    
    //Double quote
    rules.push_back(U"\"{[\\-\"] ~%r}*\"=:1");     //string "" does not contain CR and can escape characters

    //Single quote
    rules.push_back(U"'=!");                         //4 In the case of Lisp, this rule is activated
    rules.push_back(U"'~%r*'=:2");                    //38    string '' does not contain CR and does not process escape characters

    
    //We put this meta-rule here to avoid checking all the rules belows with it
    rules.push_back(U"1:{%d #A-F #a-f}");                    //metarule on 1, for hexadecimal digits
    
    //Tokenizing numbers
    //22 for blocs of 3, with a comma
    //44 for blocs of 3, with a point or a space
    //rules are either 22 or 44 never both
    //33 using a decimal point
    //66 using a decimal comma
    //rules are either 33 or 66 never both
    //The order of the rules is important, if blocs of 3 are to be taken into account
    rules.push_back(U"{%- %+}%d+[,%d%d%d]+=22");          //digits separated with a , by block of 3
    rules.push_back(U"{%- %+}%d+[{.#32}%d%d%d]+=!44");    //digits separated with a . or a space by block of 3
    rules.push_back(U"%d+[,%d%d%d]+=22");  //digits separated with a , by block of 3
    rules.push_back(U"%d+[{.#32}%d%d%d]+=!44"); //digits separated with a . or a space by block of 3

    rules.push_back(U"{%- %+}0x%1+(.%1+({pP}({%- %+})%d+))=33");  //hexadecimal with a decimal point
    rules.push_back(U"{%- %+}0x%1+(,%1+({pP}({%- %+})%d+))=!66"); //Gated: hexadecimal with a decimal comma
    rules.push_back(U"{%- %+}%d+(.%d+({eE}({%- %+})%d+))=33");    //Numbers with decimal point
    rules.push_back(U"{%- %+}%d+(,%d+({eE}({%- %+})%d+))=!66"); //Gated: Numbers with decimal comma

    rules.push_back(U"0x%1+(.%1+({p P}({%- %+})%d+))=33");   //hexadecimal
    rules.push_back(U"0x%1+(,%1+({p P}({%- %+})%d+))=!66"); //Gated rules
    rules.push_back(U"%d+(.%d+({eE}({%- %+})%d+))=33");      //digits
    rules.push_back(U"%d+(,%d+({eE}({%- %+})%d+))=!66"); //Gated rules

    rules.push_back(U"{%- %+}0b{1 0}+=4");  //binaries
    rules.push_back(U"0b{1 0}+=4");  //binaires
    rules.push_back(U"0c({%- %+})%d+(.%d+)({eE}({%- %+})%d+):({%- %+})(%d+(.%d+)({eE}({%- %+})%d+))i=4");    //complex numbers

    rules.push_back(U"%#{%a %d}+=4");       //Regular strings
    rules.push_back(U"${%a %d}+=4");       //Regular strings
    
    rules.push_back(U"http(s)://{%a %d . = %# & %? / %- %+}+=4");       //http
    rules.push_back(U"{%a %d . %- %+}+@{%a %d . = & %? %# %- %+}+=4");       //mail address
    
    rules.push_back(U"%o=4");                 //operator
    rules.push_back(U"%p=4");                  //punctuation
    rules.push_back(U"%H{%H %d}*=4");       //Asian characters (Chinese, Korean, Japanese)
    rules.push_back(U"%h{%h %- %d}*=4");       //Greek
    rules.push_back(U"%a{%a %- '’ %d}*=4");       //Regular strings
    rules.push_back(U"~{%S %o %p}+=4");           //Any combination of Unicode characters ending at a space or a punctuation
}
//--------------------------------------------------------------------
Tamgu* ProcCreateFrame(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
bool Activategarbage(bool v);
//--------------------------------------------------------------------
static bool windowmode;
extern "C" {
    char WindowModeActivated(void) {
        return windowmode;
    }
}

void InitWindowMode() {
    windowmode = false;
}

static x_node* creationxnode(string t, string v, x_node* parent = NULL) {
	x_node* n = new x_node;
	n->token = t;
	n->value = v;
	if (parent != NULL) {
		n->start = parent->start;
		n->end = parent->end;
		parent->nodes.push_back(n);
	}
	return n;
}

static void setstartend(x_node* x, x_node* ref) {
	x->start = ref->start;
	x->end = ref->end;
	for (int i = 0; i < x->nodes.size(); i++)
		setstartend(x->nodes[i], ref);
}

//--------------------------------------------------------------------
uchar Returnequ(short ty, bool top = false) {
	switch (ty) {
	case a_boolean:
	case a_byte:
	case a_short:
	case a_bloop:
		return b_short;
	case a_int:
	case a_iloop:
	case a_intthrough:
		return b_int;
	case a_long:
	case a_lloop:
	case a_longthrough:
		return b_long;
	case a_decimal:
	case a_dloop:
	case a_decimalthrough:
		return b_decimal;
	case a_fraction:
	case a_float:
	case a_floop:
	case a_floatthrough:
		return b_float;
	case a_string:
	case a_sloop:
	case a_stringthrough:
		return b_string;
    case a_let:
    case a_self:
            if (top)
                return b_letself;
            else
                return 255;
	case a_ustring:
	case a_uloop:
	case a_ustringthrough:
		return b_ustring;
	}

	return 255;
}

Tamgu* Evaluatetype(uchar thetype, uchar ref, Tamgu* a) {
	uchar t = Returnequ(a->Type());
	Tamgu* ret = NULL;
	if ((thetype & b_allnumbers)) {
		if (t & b_allstrings) {
			//return a value according to ref
			switch (ref) {
			case b_short:
				ret = new Tamgushort(a->Short());
				break;
			case b_int:
				ret = globalTamgu->Provideint(a->Integer());
				break;
			case b_long:
				ret = globalTamgu->Providelong(a->Long());
				break;
			case b_decimal:
				ret = new Tamgudecimal(a->Decimal());
				break;
			case b_float:
				ret = globalTamgu->Providefloat(a->Float());
				break;
			}
			if (ret != NULL) {
				a->Release();
				return ret;
			}
			return a;
		}
		if (t & b_allnumbers) {
			if (ref <= t)
				return a;

			switch (ref) {
            case b_short:
                ret = new Tamgushort(a->Short());
                break;
			case b_int:
				ret = globalTamgu->Provideint(a->Integer());
				break;
			case b_long:
				ret = globalTamgu->Providelong(a->Long());
				break;
			case b_decimal:
				ret = new Tamgudecimal(a->Decimal());
				break;
			case b_float:
				ret = globalTamgu->Providefloat(a->Float());
				break;
			}
			if (ret != NULL) {
				a->Release();
				return ret;
			}
		}
		return a;
	}

	if ((thetype & b_allstrings) && (t & b_allnumbers)) {
		if (thetype == a_ustring) {
			ret = globalTamgu->Provideustring(a->UString());
			a->Release();
			return ret;
		}
		ret = globalTamgu->Providestring(a->String());
		a->Release();
		return ret;
	}
	return a;
}

Tamgu* TamguActionVariable::update(short idthread) {
    switch(typevar) {
        case a_short:
            return new TamguActionVariableShort(name, typevar);
        case a_int:
            return new TamguActionVariableInt(name, typevar);
        case a_long:
            return new TamguActionVariableLong(name, typevar);
        case a_decimal:
            return new TamguActionVariableDecimal(name, typevar);
        case a_float:
            return new TamguActionVariableFloat(name, typevar);
        case a_string:
            return new TamguActionVariableString(name, typevar);
        case a_ustring:
            return new TamguActionVariableUString(name, typevar);
    }
    return this;
}

//--------------------------------------------------------------------
static bool isnegation(string label) {
    if (label=="negation" || label=="not" || label=="non")
        return true;
    return false;
}
//--------------------------------------------------------------------

Tamgu* TamguCallTamguVariable::Declaration(short id) {
    return aa->acode->mainframe.Declaration(id);
}

void TamguCode::Senderror(string msg) {
    stringstream message;
    message << msg;
    throw new TamguRaiseError(message, filename, current_start, current_end);
}
//--------------------------------------------------------------------
//The main function, which is used to traverse the parse tree and creates each instruction and declaration...
Tamgu* TamguCode::Traverse(x_node* xn, Tamgu* parent) {
	if (xn == NULL)
		return NULL;
    
    last_node = xn;
	currentline = Computecurrentline(0, xn);
	if (global->parseFunctions.find(xn->token) != global->parseFunctions.end())
		return (this->*global->parseFunctions[xn->token])(xn, parent);

	Tamgu* a;
	Tamgu* res = NULL;
	for (size_t i = 0; i < xn->nodes.size(); i++) {
		a = Traverse(xn->nodes[i], parent);
		if (res == NULL)
			res = a;
	}

	return res;
}

Tamgu* TamguCode::Traverse_in_error(x_node* xn, Tamgu* parent, long line) {
    if (xn == NULL || last_node == xn)
        return NULL;
    
    currentline = Computecurrentline(0, xn);
    if (currentline > line)
        return Traverse(xn, parent);
    
    Tamgu* a = NULL;
    for (size_t i = 0; i < xn->nodes.size() && a == NULL; i++) {
        a = Traverse_in_error(xn->nodes[i], parent, line);
    }

    return a;
}

void TamguCode::BrowseVariable(x_node* xn, Tamgu* kf) {
	if (xn == NULL)
		return;

	if (xn->token == "variable" || xn->token == "purevariable") {
		short id = global->Getid(xn->nodes[0]->value);
		if (!kf->isDeclared(id) || kf->Declaration(id) == aNOELEMENT) {
			Tamgu* var = new TamguTaskellSelfVariableDeclaration(global, id, a_self, kf);
			kf->Declare(id, var);
		}
		return;
	}

	for (size_t i = 0; i < xn->nodes.size(); i++)
		BrowseVariable(xn->nodes[i], kf);
}

void TamguCode::BrowseVariableVector(x_node* xn, Tamgu* kf) {
	if (xn == NULL)
		return;

	if (xn->token == "variable" || xn->token == "purevariable") {
		short id = global->Getid(xn->nodes[0]->value);
		if (!kf->isDeclared(id) || kf->Declaration(id) == aNOELEMENT) {
			Tamgu* var = new TamguTaskellSelfVariableDeclaration(global, id, a_self, kf);
			kf->Declare(id, var);
		}
		return;
	}

	for (size_t i = 0; i < xn->nodes.size(); i++)
		BrowseVariableVector(xn->nodes[i], kf);
}

void TamguCode::BrowseVariableMap(x_node* xn, Tamgu* kf) {
	if (xn == NULL)
		return;

	if (xn->token == "variable" || xn->token == "purevariable") {
		short id = global->Getid(xn->nodes[0]->value);
		if (!kf->isDeclared(id) || kf->Declaration(id) == aNOELEMENT) {
			Tamgu* var = new TamguTaskellSelfVariableDeclaration(global, id, a_self, kf);
			kf->Declare(id, var);
		}
		return;
	}

	for (size_t i = 0; i < xn->nodes.size(); i++)
		BrowseVariableMap(xn->nodes[i], kf);
}

void TamguCode::DeclareVariable(x_node* xn, Tamgu* kf) {
	if (xn == NULL)
		return;

	if (xn->token == "variable" || xn->token == "purevariable") {
		short id = global->Getid(xn->nodes[0]->value);
		if (!kf->isDeclared(id))
			kf->Declare(id, aNOELEMENT);
		return;
	}

	for (size_t i = 0; i < xn->nodes.size(); i++)
		DeclareVariable(xn->nodes[i], kf);
}

//--------------------------------------------------------------------
long TamguCode::Computecurrentline(int i, x_node* xn) {
	current_start = global->currentbnf->x_linenumber(xn->start + i);
	current_end = global->currentbnf->x_linenumber(xn->end + i);
	return (long)current_start;
}

//--------------------------------------------------------------------
//This method is used to retrieve lines from stack_error
void TamguGlobal::Getstack(vector<string>& stacklines, vector<string>& files, vector<long>& linenumbers) {
    long file_id = -1, ln, current_file = -1;

    string name = "string";
    string line;
    Tamgu* a;
    vector<string> lines;
    if (store_in_code_lines)
        lines = codelines;
    
    for (long i = 0; i < stack_error.size(); i++) {
        stringstream message;
        a = stack_error[i];
        ln = a->Currentline();
        if (ln != -1) {
            line = a->String();
            if (!store_in_code_lines) {
                file_id = a->Currentfile();
                if (file_id !=-1) {
                    name = Getfilename(file_id);
                    if (current_file == -1 || current_file != file_id) {
                        ifstream thefile(name, ios::binary);
                        lines.clear();
                        lines.push_back("");
                        while (!thefile.eof()) {
                            std::getline(thefile, line);
                            lines.push_back(line);
                        }
                    }
                }
            }

            if (!lines.empty() && ln < lines.size()) {
                line = "[";
                line += Trim(lines[ln]);
                line += "]";
            }
            
            stacklines.push_back(message.str());
            linenumbers.push_back(ln);
            files.push_back(name);
        }
    }
}

void TamguGlobal::Getstack(std::stringstream& message) {
    long file_id = -1, ln, current_file = -1;
    long nb = 0;
    string name;
    string line;
    Tamgu* a;
    vector<string> lines;
    if (store_in_code_lines)
        lines = codelines;
    
    if (stack_error.size() > 1) {
        for (long i = 0; i < stack_error.size(); i++) {
            a = stack_error[i];
            ln = a->Currentline();
            if (ln != -1) {
                line = a->String();
                if (!store_in_code_lines) {
                    file_id = a->Currentfile();
                    if (file_id !=-1) {
                        name = Getfilename(file_id);
                        if ((current_file == -1 || current_file != file_id) && name != "") {
                            ifstream thefile(name, ios::binary);
                            lines.clear();
                            lines.push_back("");
                            while (!thefile.eof()) {
                                std::getline(thefile, line);
                                lines.push_back(line);
                            }
                        }
                    }
                }
                
                if (!lines.empty() && ln < lines.size()) {
                    string l = Trim(lines[ln]);
                    if (l.size() > 23) {
                        l = l.substr(0,20);
                        l += "...";
                    }
                    line = "[";
                    line += l;
                    line += "]";
                }
                
                message << nb++ << ": " << line << " at " << ln;
                if (file_id != -1 && name != "")
                    message << " in " << name  << endl;
                else
                    message << endl;
            }
        }
    }
}
//--------------------------------------------------------------------
void TamguGlobal::RecordCompileFunctions() {
	parseFunctions["declaration"] = &TamguCode::C_parameterdeclaration;
	parseFunctions["multideclaration"] = &TamguCode::C_multideclaration;
	parseFunctions["subfunc"] = &TamguCode::C_subfunc;
	parseFunctions["subfuncbis"] = &TamguCode::C_subfunc;
    parseFunctions["regularcall"] = &TamguCode::C_regularcall;
	parseFunctions["negcall"] = &TamguCode::C_regularcall;
	parseFunctions["purecall"] = &TamguCode::C_regularcall;

    parseFunctions["framevariable"] = &TamguCode::C_framevariable;
    parseFunctions["variable"] = &TamguCode::C_variable;
	parseFunctions["purevariable"] = &TamguCode::C_variable;

    
    parseFunctions["shapeindexes"] = &TamguCode::C_shapeindexes;
    parseFunctions["indexes"] = &TamguCode::C_indexes;
	parseFunctions["interval"] = &TamguCode::C_interval;

	parseFunctions["parameters"] = &TamguCode::C_parameters;
	parseFunctions["dependencyparameters"] = &TamguCode::C_parameters;

    parseFunctions["achar"] = &TamguCode::C_achar;
    parseFunctions["astring"] = &TamguCode::C_astring;
	parseFunctions["afullstring"] = &TamguCode::C_astring;
	parseFunctions["astringdouble"] = &TamguCode::C_astring;
	parseFunctions["astringsimple"] = &TamguCode::C_astring;

    parseFunctions["atreg"] = &TamguCode::C_rstring;
    parseFunctions["astreg"] = &TamguCode::C_rstring;

    parseFunctions["apreg"] = &TamguCode::C_pstring;
    parseFunctions["aspreg"] = &TamguCode::C_pstring;

    parseFunctions["fstring"] = &TamguCode::C_fstring;
    parseFunctions["festring"] = &TamguCode::C_fstring;

    
	parseFunctions["ufullstring"] = &TamguCode::C_ustring;
	parseFunctions["ustringdouble"] = &TamguCode::C_ustring;
	parseFunctions["ustringsimple"] = &TamguCode::C_ustring;

	parseFunctions["punct"] = &TamguCode::C_astring;

	parseFunctions["anumber"] = &TamguCode::C_anumber;
	parseFunctions["frame"] = &TamguCode::C_frame;
	parseFunctions["extension"] = &TamguCode::C_extension;

	parseFunctions["valvector"] = &TamguCode::C_valvector;
	parseFunctions["valvectortail"] = &TamguCode::C_valvector;
	parseFunctions["oneparenthetic"] = &TamguCode::C_valvector;
	parseFunctions["onetag"] = &TamguCode::C_valvector;
	parseFunctions["valpredicatevector"] = &TamguCode::C_valvector;

	parseFunctions["jvector"] = &TamguCode::C_jsonvector;

	parseFunctions["valmap"] = &TamguCode::C_valmap;
	parseFunctions["maptail"] = &TamguCode::C_valmap;
	parseFunctions["valmaptail"] = &TamguCode::C_valmap;
	parseFunctions["hvalmaptail"] = &TamguCode::C_valmap;
	parseFunctions["valmappredicate"] = &TamguCode::C_valmap;
	parseFunctions["mapmerging"] = &TamguCode::C_valmap;
	parseFunctions["jmap"] = &TamguCode::C_jsonmap;


	parseFunctions["features"] = &TamguCode::C_features;
	parseFunctions["dico"] = &TamguCode::C_dico;
	parseFunctions["dicoval"] = &TamguCode::C_dico;
	parseFunctions["predicatedico"] = &TamguCode::C_dico;

	parseFunctions["jdico"] = &TamguCode::C_jsondico;

	parseFunctions["intentionvector"] = &TamguCode::C_intentionvector;
	parseFunctions["intentionwithdouble"] = &TamguCode::C_intentionwithdouble;

	parseFunctions["breakcall"] = &TamguCode::C_uniquecall;
	parseFunctions["returncall"] = &TamguCode::C_uniquecall;
	parseFunctions["breakpointcall"] = &TamguCode::C_uniquecall;
	parseFunctions["continuecall"] = &TamguCode::C_uniquecall;

	parseFunctions["aliasdeclaration"] = &TamguCode::C_alias;
    parseFunctions["function"] = &TamguCode::C_createfunction;
	parseFunctions["blocs"] = &TamguCode::C_blocs;

	parseFunctions["operatoraffectation"] = &TamguCode::C_operator;
	parseFunctions["operator"] = &TamguCode::C_operator;
	parseFunctions["hoper"] = &TamguCode::C_operator;
	parseFunctions["comparator"] = &TamguCode::C_operator;
	parseFunctions["hcomparator"] = &TamguCode::C_operator;
	parseFunctions["orand"] = &TamguCode::C_operator;
	parseFunctions["increment"] = &TamguCode::C_increment;
	parseFunctions["affectation"] = &TamguCode::C_affectation;
	parseFunctions["affectationpredicate"] = &TamguCode::C_affectation;
	parseFunctions["predplusplus"] = &TamguCode::C_increment;
	parseFunctions["comparepredicate"] = &TamguCode::C_comparepredicate;

    parseFunctions["curryingright"] = &TamguCode::C_curryingright;
    parseFunctions["curryingleft"] = &TamguCode::C_curryingleft;
    
	parseFunctions["multiply"] = &TamguCode::C_multiply;
	parseFunctions["hmultiply"] = &TamguCode::C_multiply;
    parseFunctions["operation"] = &TamguCode::C_operation;
	parseFunctions["fulloperation"] = &TamguCode::C_operation;
	parseFunctions["hoperation"] = &TamguCode::C_operation;
	parseFunctions["plusplus"] = &TamguCode::C_plusplus;
	parseFunctions["power"] = &TamguCode::C_plusplus;
	parseFunctions["operationin"] = &TamguCode::C_operationin;
    parseFunctions["comparison"] = &TamguCode::C_comparison;
    parseFunctions["taskcomparison"] = &TamguCode::C_taskcomparison;
	parseFunctions["hcomparison"] = &TamguCode::C_comparison;
	parseFunctions["abool"] = &TamguCode::C_constant;
	parseFunctions["aconstant"] = &TamguCode::C_constant;
	parseFunctions["negated"] = &TamguCode::C_negated;

	parseFunctions["parenthetic"] = &TamguCode::C_parenthetic;
    parseFunctions["optionalboolean"] = &TamguCode::C_booleanexpression;
	parseFunctions["hoptionalboolean"] = &TamguCode::C_parenthetic;
	parseFunctions["hforcecompare"] = &TamguCode::C_parenthetic;

	parseFunctions["iftest"] = &TamguCode::C_ifcondition;
	parseFunctions["localif"] = &TamguCode::C_ifcondition;
	parseFunctions["testelif"] = &TamguCode::C_ifcondition;

    parseFunctions["expression"] = &TamguCode::C_expression;

    parseFunctions["negation"] = &TamguCode::C_negation;
    parseFunctions["not"] = &TamguCode::C_negation;
    parseFunctions["non"] = &TamguCode::C_negation;

    parseFunctions["comparing"] = &TamguCode::C_booleanexpression;
	parseFunctions["booleanexpression"] = &TamguCode::C_booleanexpression;
	parseFunctions["hbooleanexpression"] = &TamguCode::C_booleanexpression;
	parseFunctions["hforcebooleanexpression"] = &TamguCode::C_booleanexpression;

	parseFunctions["switch"] = &TamguCode::C_switch;
	parseFunctions["testswitch"] = &TamguCode::C_testswitch;

	parseFunctions["loop"] = &TamguCode::C_loop;
	parseFunctions["doloop"] = &TamguCode::C_doloop;
	parseFunctions["for"] = &TamguCode::C_for;
	parseFunctions["blocfor"] = &TamguCode::C_blocfor;
	parseFunctions["forin"] = &TamguCode::C_forin;
    
    parseFunctions["namespace"] = &TamguCode::C_namespace;

	parseFunctions["trycatch"] = &TamguCode::C_trycatch;
    parseFunctions["catchon"] = &TamguCode::C_catchon;

	parseFunctions["alist"] = &TamguCode::C_alist;
	parseFunctions["valtail"] = &TamguCode::C_alist;
	parseFunctions["apredicatelist"] = &TamguCode::C_alist;
	parseFunctions["merging"] = &TamguCode::C_alist;
	parseFunctions["alistnomerge"] = &TamguCode::C_alist;

	//Predicates
	parseFunctions["cut"] = &TamguCode::C_cut;
	parseFunctions["fail"] = &TamguCode::C_cut;
	parseFunctions["stop"] = &TamguCode::C_cut;
	parseFunctions["dcg"] = &TamguCode::C_dcg;
    parseFunctions["rawfact"] = &TamguCode::C_rawfact;
	parseFunctions["predicatefact"] = &TamguCode::C_predicatefact;
	parseFunctions["predicatedefinition"] = &TamguCode::C_predicatefact;
	parseFunctions["predicate"] = &TamguCode::C_predicate;
	parseFunctions["predicateexpression"] = &TamguCode::C_predicateexpression;
	parseFunctions["predicatevariable"] = &TamguCode::C_predicatevariable;
	parseFunctions["assertpredicate"] = &TamguCode::C_assertpredicate;
	parseFunctions["term"] = &TamguCode::C_term;
	parseFunctions["tuple"] = &TamguCode::C_term;
	parseFunctions["predicatecall"] = &TamguCode::C_regularcall;

	parseFunctions["dependencyrule"] = &TamguCode::C_dependencyrule;
	parseFunctions["dependency"] = &TamguCode::C_dependency;
	parseFunctions["dependencyfact"] = &TamguCode::C_dependency;
	parseFunctions["dependencyresult"] = &TamguCode::C_dependencyresult;

	//Taskell vector handling...
    parseFunctions["hnegated"] = &TamguCode::C_negated;

	parseFunctions["declarationtaskell"] = &TamguCode::C_declarationtaskell;
	parseFunctions["taskelldico"] = &TamguCode::C_dico;
	parseFunctions["taskellcall"] = &TamguCode::C_taskellcall;
	parseFunctions["taskellcase"] = &TamguCode::C_taskellcase;
	parseFunctions["guard"] = &TamguCode::C_guard;
	parseFunctions["letmin"] = &TamguCode::C_letmin;
	parseFunctions["hinexpression"] = &TamguCode::C_hinexpression;
	parseFunctions["whereexpression"] = &TamguCode::C_whereexpression;
	parseFunctions["taskellvector"] = &TamguCode::C_taskellvector;
	parseFunctions["taskellmap"] = &TamguCode::C_taskellmap;

	parseFunctions["dataassignment"] = &TamguCode::C_dataassignment;
	parseFunctions["conceptfunction"] = &TamguCode::C_conceptfunction;
		
	parseFunctions["taskellexpression"] = &TamguCode::C_taskellexpression;
	parseFunctions["taskellkeymap"] = &TamguCode::C_taskellexpression;

	parseFunctions["repeating"] = &TamguCode::C_cycling;
	parseFunctions["cycling"] = &TamguCode::C_cycling;

	parseFunctions["flipping"] = &TamguCode::C_flipping;
	parseFunctions["mapping"] = &TamguCode::C_mapping;
	parseFunctions["filtering"] = &TamguCode::C_filtering;
	parseFunctions["taking"] = &TamguCode::C_filtering;

	parseFunctions["zipping"] = &TamguCode::C_zipping;
	parseFunctions["pairing"] = &TamguCode::C_zipping;

	parseFunctions["folding1"] = &TamguCode::C_folding;
	parseFunctions["folding"] = &TamguCode::C_folding;
    parseFunctions["taskellalltrue"] = &TamguCode::C_taskellalltrue;
    parseFunctions["taskellboolchecking"] = &TamguCode::C_taskellboolchecking;
    parseFunctions["hfunctioncall"] = &TamguCode::C_hfunctioncall;
    parseFunctions["hfunctionoperation"] = &TamguCode::C_hfunctioncall;
	parseFunctions["hcompose"] = &TamguCode::C_hcompose;
	parseFunctions["let"] = &TamguCode::C_multideclaration;
	parseFunctions["hlambda"] = &TamguCode::C_hlambda;
	
	parseFunctions["hontology"] = &TamguCode::C_ontology;
	parseFunctions["telque"] = &TamguCode::C_telque;
    parseFunctions["subtelque"] = &TamguCode::C_telque;
	parseFunctions["hbloc"] = &TamguCode::C_hbloc;
	parseFunctions["hdata"] = &TamguCode::C_hdata;
    parseFunctions["hdatadeclaration"] = &TamguCode::C_hdatadeclaration;
    parseFunctions["hdeclaration"] = &TamguCode::C_hdeclaration;
    parseFunctions["taskellbasic"] = &TamguCode::C_taskellbasic;

    parseFunctions["annotationrule"] = &TamguCode::C_annotationrule;
    parseFunctions["annotation"] = &TamguCode::C_annotation;
    parseFunctions["listoftokens"] = &TamguCode::C_listoftokens;
    parseFunctions["sequenceoftokens"] = &TamguCode::C_sequenceoftokens;
    parseFunctions["optionaltokens"] = &TamguCode::C_optionaltokens;
    parseFunctions["removetokens"] = &TamguCode::C_removetokens;
    parseFunctions["token"] = &TamguCode::C_token;
    
    //LISP implementation
    parseFunctions["tlvariable"] = &TamguCode::C_tamgulispvariable;
    parseFunctions["tlatom"] = &TamguCode::C_tamgulispatom;
    parseFunctions["tlquote"] = &TamguCode::C_tamgulispquote;
    parseFunctions["tlist"] = &TamguCode::C_tamgulisp;
    parseFunctions["tlkeys"] = &TamguCode::C_valmap;
    parseFunctions["tlkey"] = &TamguCode::C_dico;
}


//------------------------------------------------------------------------
TamguInstruction* TamguCode::TamguCreateInstruction(Tamgu* parent, short op) {
    TamguInstruction* res;
    switch (op) {
        case a_plusequ:
        case a_minusequ:
        case a_multiplyequ:
        case a_divideequ:
        case a_modequ:
        case a_powerequ:
        case a_shiftleftequ:
        case a_shiftrightequ:
        case a_orequ:
        case a_xorequ:
        case a_andequ:
        case a_mergeequ:
        case a_combineequ:
        case a_addequ:
            res = new TamguInstructionAPPLYOPERATIONEQU(global, parent);
            break;
        case a_stream:
            res = new TamguInstructionSTREAM(global, parent);
            break;
        case a_assignement:
            res = new TamguInstructionASSIGNMENT(global, parent);
            break;
        case a_less:
        case a_more:
        case a_same:
        case a_different:
        case a_lessequal:
        case a_moreequal:
            res = new TamguInstructionCOMPARE(global, parent);
            break;
        case a_plus:
        case a_minus:
        case a_multiply:
        case a_divide:
        case a_mod:
        case a_power:
        case a_shiftleft:
        case a_shiftright:
        case a_or:
        case a_xor:
        case a_and:
        case a_merge:
        case a_combine:
        case a_add:
            res = new TamguInstructionAPPLYOPERATION(global, parent);
            break;
        case a_booleanor:
            res = new TamguInstructionOR(global, parent);
            break;
        case a_booleanxor:
            res = new TamguInstructionXOR(global, parent);
            break;
        case a_booleanand:
            res = new TamguInstructionAND(global, parent);
            break;
        case a_conjunction:
            res = new TamguInstructionConjunction(global, parent);
            break;
        case a_disjunction:
            res = new TamguInstructionDisjunction(global, parent);
            break;
        case a_notin:
        case a_in:
            res = new TamguInstructionIN(global, parent);
            break;
        case a_match:
            res = new TamguInstructionHASKELLCASE(global, parent);
            break;
        case a_ifnot:
            res = new TamguInstructionOperationIfnot(global, parent);
            break;
        default:
            res = new TamguInstruction(a_instructions, global, parent);
    }

	res->Setaction(op);
	return res;
}

TamguCallFunction* TamguCode::CreateCallFunction(Tamgu* function, Tamgu* parent) {
    if (function->Nextfunction() != NULL)
        return new TamguCallFunction(function, global, parent);
    
    switch (function->Size()) {
        case 0:
            return new TamguCallFunction0(function, global, parent);
        case 1:
            return new TamguCallFunction1(function, global, parent);
        case 2:
            return new TamguCallFunction2(function, global, parent);
        case 3:
            return new TamguCallFunction3(function, global, parent);
        case 4:
            return new TamguCallFunction4(function, global, parent);
        case 5:
            return new TamguCallFunction5(function, global, parent);
    }
    
    return new TamguCallFunction(function, global, parent);
}

Tamgu* TamguCode::CloningInstruction(TamguInstruction* ki) {
	if (ki->action == a_bloc || ki->action == a_blocboolean || !ki->isInstruction())
		return ki;

	TamguInstruction* k = TamguCreateInstruction(NULL, ki->Action());
	ki->Clone(k);
	k->instructions.clear();
	variables.clear();
	for (short i = 0; i < ki->instructions.last; i++)
		k->instructions.push_back(EvaluateVariable(ki->instructions.vecteur[i]));

    k->Setpartialtest(ki->isPartialtest());
	ki->Remove();
	if (k->Action() >= a_less && k->Action() <= a_moreequal) {
		//Let's try analysing this stuff...
		Tamgu* kcomp = k->Compile(NULL);
		if (k == kcomp)
			return k;

		k->Remove();
		return kcomp;
	}

	return k;
}

Tamgu* TamguCode::EvaluateVariable(Tamgu* var) {
	if (var->Action() == a_variable) {
		short n = var->Name();
		if (variables.check(n)) {
			var->Remove();
			return variables[n];
		}

		TamguActionVariable* act;
		if (var->Typeinfered() == a_self || var->Typeinfered() == a_let)
			act = new TamguActionLetVariable(n, var->Typeinfered());
		else
			act = new TamguActionVariable(n, var->Typeinfered());
		var->Remove();
		variables[n] = act;
		return act;
	}

	return var;
}

//-------------------------------------------------------------------------------------

static uchar Evaluateatomtype(Tamgu* ins, bool top = false) {
	short ty = ins->Typeinfered();
	if (ty == a_none)
		return 255;

	if (ty < a_short || ty > a_ustring) {
		if (ins->Function() != NULL) {
			if (!globalTamgu->newInstance.check(ty))
				return 255;

			Tamgu* a = globalTamgu->newInstance[ty];
			if (a->isValueContainer() && ins->Function()->isIndex()) {
				if (a->isDecimal())
					ty = a_decimal;
				else
				if (a->isFloat())
					ty = a_float;
				else
				if (a->isString()) {
					if (a->Type() == a_ustring)
						ty = a_ustring;
					else
						ty = a_string;
				}
				else
				if (a->isLong())
					ty = a_long;
				else
				if (a->isShort())
					ty = a_short;
				else
				if (a->isNumber())
					ty = a_int;
			}
			else
				return 255;
			return Returnequ(ty);
		}

		if (ins->isDecimal())
			ty = a_decimal;
		else
		if (ins->isFloat())
			ty = a_float;
		else
		if (ins->isString()) {
			if (ins->Type() == a_ustring)
				ty = a_ustring;
			else
				ty = a_string;
		}
		else
		if (ins->isLong())
			ty = a_long;
		else
		if (ins->isShort())
			ty = a_short;
		if (ins->isNumber())
			ty = a_int;
		else
            if (ty == a_self || ty == a_let)
                ty = a_self;
            else
                return 255;
	}
	return Returnequ(ty, top);
}

bool TamguInstructionAPPLYOPERATIONROOT::Stacking(Tamgu* ins, char top) {	
	if (top && !head) {
		//we might need to detect the type of the all instruction set, which is based on
		//the deepest element in the structure on the left...
		Tamgu* loop = ins;
		while (loop != NULL && loop->isInstruction()) loop = loop->Instruction(0);
		if (loop != NULL) {
			head = Evaluateatomtype(loop);
			alls = head;
		}
		else
			head = 255;
	}

	if (ins->isInstruction()) {
		bool simple = true;

		if (!ins->isEQU() && !ins->isOperation()) {
			instructions.push_back(ins);
            if (ins->Action() == a_ifnot)
                ins->Setaction(a_none);
			return simple;
		}

		char t = top;

		if (ins->Action() == a_divide) {
			alls |= b_float;
			if (t == 2)
				t = 0;
		}

		if (ins->Subcontext() && instructions.size() == 0) {
			instructions.push_back(aPIPE);
			sub = true;
			t = 2;
			simple = false;
		}

		TamguInstruction* ai = (TamguInstruction*)ins;
		for (short i = (short)ai->instructions.size() - 1; i >= 0; i--) {
			if (ai->instructions[i]->isOperation()) {
				instructions.push_back(aPIPE);
				sub = true;
				t = 2;
				simple = false;
			}
			if (!Stacking(ai->instructions[i], t))
				simple = false;
			if (t == 2)
				t = 0;
		}

		if (!ins->isEQU()) {
			instructions.push_back(globalTamgu->actions[ins->Action()]);			
			if (top != 1)
				ins->Remove();
		}

		return simple;
	}

	Tamgu* taskell = ins->Composition();
	if (taskell != aNOELEMENT) {
		if (taskell->isROOTOPERATION()) {
			//in that case, we need to merge the instructions from taskell into our current structure
			TamguInstructionAPPLYOPERATIONROOT* local = (TamguInstructionAPPLYOPERATIONROOT*)taskell;
			instructions.push_back(aPIPE);
			sub = true;
			for (short i = 0; i < local->instructions.last; i++)
				instructions.push_back(local->instructions[i]);

			taskell->Remove();
			ins->Remove();
			return true;
		}
		ins->Remove();
		ins = taskell;
	}

	bool remove = false;
	if (ins->Action() == a_variable && ins->Type() != a_actionvariable) {
		short n = ins->Name();
        if (variables.check(n))
            instructions.push_back(variables[n]);
        else {
            TamguActionVariable* act;
            if (ins->isGlobalVariable()) {
                if (globalTamgu->globalvariables.check(n))
                    instructions.push_back(globalTamgu->globalvariables[n]);
                else {
                    if (ins->Typeinfered() == a_self || ins->Typeinfered() == a_let)
                        act = new TamguActionGlobalLetVariable(n, ins->Typeinfered());
                    else
                        act = new TamguActionGlobalVariable(n, ins->Typeinfered());
                    remove = true;
                    globalTamgu->globalvariables[n] = act;
                    instructions.push_back(act);
                }
            }
            else {
                if (ins->Typeinfered() == a_self || ins->Typeinfered() == a_let)
                    act = new TamguActionLetVariable(n, ins->Typeinfered());
                else
                    act = new TamguActionVariable(n, ins->Typeinfered());
                remove = true;
                variables[n] = act;
                instructions.push_back(act);
            }
        }
    }
	else
		instructions.push_back(ins);

    if (ins->Typeinfered() == a_complex)
        complex = true;

	if (top) {
		uchar ty = Evaluateatomtype(ins);
		alls |= ty;

		if (ins->Typeinfered() == a_fraction)
			fraction = true;
	}

	if (remove)
		ins->Remove();

	return true;
}

Tamgu* TamguInstructionAPPLYOPERATIONEQU::update(uchar btype) {
    if (btype == 255 || thetype == 255)
        return this;
    
    if (thetype == b_letself)
        thetype = btype;
    
    if (thetype & b_allnumbers) {
        if (btype & b_allnumbers) {
            switch (thetype) {
                case b_short:
                    return new TamguInstructionEQUShort(this, globalTamgu);
                case b_int:
                    return new TamguInstructionEQUInteger(this, globalTamgu);
                case b_long:
                    return new TamguInstructionEQULong(this, globalTamgu);
                case b_decimal:
                    return new TamguInstructionEQUDecimal(this, globalTamgu);
                case b_float: {
                    if (fraction)
                        return this;
                    return new TamguInstructionEQUFloat(this, globalTamgu);
                }
            }
        }
        return this;
    }

    if (btype & b_allstrings) {
        if (thetype == b_string)
            return new TamguInstructionEQUString(this, globalTamgu);
        if (thetype == b_ustring)
            return new TamguInstructionEQUUString(this, globalTamgu);
    }
    return this;
}

Tamgu* TamguInstructionAPPLYOPERATIONROOT::Returnlocal(TamguGlobal* g, Tamgu* parent) {

	if (!thetype) {
		if (alls == 255 || complex)
			thetype = 255;
		else
			thetype = head;
	}

	if (sub && thetype != 255) {
        //Otherwise, we will go through ccompute
        if (alls != 255) {
            //If we have only numbers or only strings, we can apply one of the regular strategies...
            if ((alls & b_allnumbers) == alls) {
                sub = false;
                if ((thetype & b_allstrings) == thetype)
                    thetype=alls;
            }
            else {
                if ((alls & b_allstrings) == alls) {
                    if ((thetype & b_allnumbers) != thetype)
                        sub = false;
                }
            }
        }
    }

	if (!sub) {
        if (thetype == b_letself)
            return new TamguInstructionCompute(this, g);

		switch (thetype) {
		case b_string:
			return new TamguInstructionSTRING(this, g);
        case b_ustring:
			return new TamguInstructionUSTRING(this, g);
		case b_short:
		case b_int:
		case b_long:
		case b_decimal:
		case b_float:
			//First we try to get the top numerical value
			alls |= thetype;
			if (alls != 255) {
				if ((alls & b_float) || (alls & b_longdecimal) == b_longdecimal) {
					if (fraction)
						return new TamguInstructionFRACTION(this, g);
					return new TamguInstructionFLOAT(this, g);
				}

				if ((alls & b_decimal))
					return new TamguInstructionDECIMAL(this, g);

				if ((alls & b_long) || parent == NULL)
					return new TamguInstructionLONG(this, g);

				if ((alls & b_int))
					return new TamguInstructionINTEGER(this, g);

				if ((alls & b_short))
					return new TamguInstructionSHORT(this, g);
			}

			switch (thetype) {
			case b_short:
				return new TamguInstructionSHORT(this, g);
			case b_int:
				return new TamguInstructionINTEGER(this, g);
			case b_long:
				return new TamguInstructionLONG(this, g);
			case b_decimal:
				return new TamguInstructionDECIMAL(this, g);
			case b_float:
				if (fraction)
					return new TamguInstructionFRACTION(this, g);
				return new TamguInstructionFLOAT(this, g);
			}
		}
	}

    try {
        if (complex)
            return new TamguInstructionComplex(this, g);
        
        return new TamguInstructionCompute(this, g);
    }
    catch(TamguRaiseError* msg) {
        TamguCode* code = globalTamgu->Getcurrentcode();
        msg->filename = code->filename;
        msg->left = code->current_start;
        msg->right = code->current_end;
        throw msg;
    }
}

//Composition returns a potential ROOT instruction that could be merged within a ROOT...
Tamgu* TamguCallFunctionTaskell::Composition() {
	if (body->lambdadomain->instructions.size() == 0 && body->instructions.size() == 1)
		return body->instructions.back()->Argument(0);

	return aNOELEMENT;
}

Tamgu* TamguInstructionAPPLYOPERATION::Compile(Tamgu* parent) {
	TamguInstructionAPPLYOPERATIONROOT* kroot = new TamguInstructionAPPLYOPERATIONROOT(globalTamgu);
    if (parent != NULL) {
		kroot->thetype = Evaluateatomtype(parent, true);
        parent = aONE;
    }

	kroot->Stacking(this, true);
	kroot->Setsize();
    
	Tamgu* kvroot = kroot->Returnlocal(globalTamgu, parent);
	if (kvroot != kroot) {
		kroot->Remove();
		return kvroot;
	}

	return kroot;
}

Tamgu* TamguInstructionCOMPARE::Compile(Tamgu* parent) {
	uchar left;
	uchar right;
    uchar check;
    
    if (returntype != a_null) {
        check = right = left = Returnequ(returntype);
    }
    else {
        if (parent == NULL) {
            left = Evaluateatomtype(instructions.vecteur[0]);
            right = Evaluateatomtype(instructions.vecteur[1]);

            if (left == 255) {
                //regular expressions...
                short ty = instructions.vecteur[0]->Typeinfered();
                if (ty == a_treg || ty == a_preg)
                    right = 255;
            }
        }
        else {
            left = Evaluateatomtype(instructions.vecteur[0]->Eval(aNULL, aNULL, 0));
            right = Evaluateatomtype(instructions.vecteur[1]->Eval(aNULL, aNULL, 0));
            if (left == 255) {
                //regular expressions...
                short ty = instructions.vecteur[0]->Eval(aNULL, aNULL, 0)->Typeinfered();
                if (ty == a_treg || ty == a_preg)
                    right = 255;
            }
        }
    }
    //we add one specific constraint, which is that the right instruction MUST be a constant
    if (instructions.vecteur[1]->isConst()) {
        if ((left != 255 && right != 255) || (taskellif && right != 255)) {
            if ((left&b_allnumbers) && (right&b_allnumbers)) {
                if (left == 255) {
                    check=right;
                    if (right < b_long) //since we cannot know, we go for the biggest
                        check=b_long;
                    else
                        if (right==b_decimal)
                            right=b_float;
                }
                else
                    check = left | right;
                
                if ((check & b_float) || (check & b_longdecimal) == b_longdecimal) {
                    switch (action) {
                        case a_less:
                            return new c_less_float(globalTamgu, this);
                        case a_more:
                            return new c_more_float(globalTamgu, this);
                        case a_same:
                            return new c_same_float(globalTamgu, this);
                        case a_different:
                            return new c_different_float(globalTamgu, this);
                        case a_lessequal:
                            return new c_lessequal_float(globalTamgu, this);
                        case a_moreequal:
                            return new c_moreequal_float(globalTamgu, this);
                    }
                    
                    return this;
                }
                
                if ((check & b_decimal)) {
                    switch (action) {
                        case a_less:
                            return new c_less_decimal(globalTamgu, this);
                        case a_more:
                            return new c_more_decimal(globalTamgu, this);
                        case a_same:
                            return new c_same_decimal(globalTamgu, this);
                        case a_different:
                            return new c_different_decimal(globalTamgu, this);
                        case a_lessequal:
                            return new c_lessequal_decimal(globalTamgu, this);
                        case a_moreequal:
                            return new c_moreequal_decimal(globalTamgu, this);
                    }
                    
                    return this;
                }
                
                if ((check & b_long)) {
                    switch (action) {
                        case a_less:
                            return new c_less_long(globalTamgu, this);
                        case a_more:
                            return new c_more_long(globalTamgu, this);
                        case a_same:
                            return new c_same_long(globalTamgu, this);
                        case a_different:
                            return new c_different_long(globalTamgu, this);
                        case a_lessequal:
                            return new c_lessequal_long(globalTamgu, this);
                        case a_moreequal:
                            return new c_moreequal_long(globalTamgu, this);
                    }
                    return this;
                }
                
                if ((check & b_int)) {
                    switch (action) {
                        case a_less:
                            return new c_less_int(globalTamgu, this);
                        case a_more:
                            return new c_more_int(globalTamgu, this);
                        case a_same:
                            return new c_same_int(globalTamgu, this);
                        case a_different:
                            return new c_different_int(globalTamgu, this);
                        case a_lessequal:
                            return new c_lessequal_int(globalTamgu, this);
                        case a_moreequal:
                            return new c_moreequal_int(globalTamgu, this);
                    }
                    return this;
                }
                
                if ((check & b_short)) {
                    switch (action) {
                        case a_less:
                            return new c_less_short(globalTamgu, this);
                        case a_more:
                            return new c_more_short(globalTamgu, this);
                        case a_same:
                            return new c_same_short(globalTamgu, this);
                        case a_different:
                            return new c_different_short(globalTamgu, this);
                        case a_lessequal:
                            return new c_lessequal_short(globalTamgu, this);
                        case a_moreequal:
                            return new c_moreequal_short(globalTamgu, this);
                    }
                    return this;
                }
            }
            
            if ((left&b_allstrings) && (right&b_allstrings)) {
                if (left == 255)
                    check=right;
                else
                    check = left | right;
                
                if ((check & b_string)) {
                    switch (action) {
                        case a_less:
                            return new c_less_string(globalTamgu, this);
                        case a_more:
                            return new c_more_string(globalTamgu, this);
                        case a_same:
                            return new c_same_string(globalTamgu, this);
                        case a_different:
                            return new c_different_string(globalTamgu, this);
                        case a_lessequal:
                            return new c_lessequal_string(globalTamgu, this);
                        case a_moreequal:
                            return new c_moreequal_string(globalTamgu, this);
                    }
                    return this;
                }
                
                if ((check & b_ustring)) {
                    switch (action) {
                        case a_less:
                            return new c_less_ustring(globalTamgu, this);
                        case a_more:
                            return new c_more_ustring(globalTamgu, this);
                        case a_same:
                            return new c_same_ustring(globalTamgu, this);
                        case a_different:
                            return new c_different_ustring(globalTamgu, this);
                        case a_lessequal:
                            return new c_lessequal_ustring(globalTamgu, this);
                        case a_moreequal:
                            return new c_moreequal_ustring(globalTamgu, this);
                    }
                    return this;
                }
            }
        }
    }
    
    switch (action) {
        case a_less:
            return new c_less(globalTamgu, this);
        case a_more:
            return new c_more(globalTamgu, this);
        case a_same:
            return new c_same(globalTamgu, this);
        case a_different:
            return new c_different(globalTamgu, this);
        case a_lessequal:
            return new c_lessequal(globalTamgu, this);
        case a_moreequal:
            return new c_moreequal(globalTamgu, this);
    }
    
	return this;
}


//-------------------------------------------------------------------------------------


static bool TestFunction(string& token, bool func) {
	if (token == "subfunc" || token == "subfuncbis")
		return true;
	if (func == false) {
		if (token == "indexes" || token == "interval")
			return true;
	}
	return false;
}

//------------------------------------------------------------------------
long TamguGlobal::Getcurrentline() {
	if (spaceid >= spaces.size())
		return -1;
	return spaces[spaceid]->Getcurrentline();
}

string TamguGlobal::Getcurrentfile() {
	if (spaceid >= spaces.size())
		return "";
	short idfile = spaces[spaceid]->currentfileid;
	return Getfilename(idfile);
}

short TamguGlobal::Getfileid(short& idc, string f) {
	short idf = -1;
	for (idc = 0; idc < spaces.size(); idc++) {
		idf = spaces[idc]->Getfileid(f);
		if (idf != -1)
			return idf;
	}
	return -1;
}

Exporting string TamguGlobal::Getfilename(short fileid) {
	if (fileid != -1 && fileid < filenames.size())
		return filenames[fileid];
	return "";
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
// There is only three possible spot in which an object can have been declared
// On the top of the stack
// On the current domain (which a frame that is being declared)
// In the global space...
Tamgu* TamguCode::Declaror(short id) {
    long i = global->Stacksize() - 1;
    
    for (; i >= 0 && !global->DStack(i)->isDeclared(id); i--) {}
    
    if (i >= 0)
        return global->DStack(i);
    return NULL;
}

Tamgu* TamguCode::Declaror(short id, Tamgu* parent) {
	if (parent->isDeclared(id))
		return parent;

    long i = global->Stacksize() - 1;
    
    for (; i >= 0 && !global->DStack(i)->isDeclared(id); i--) {}
    
    if (i >= 0)
        return global->DStack(i);
    return NULL;
}
    
Tamgu* TamguCode::Frame() {
    long i = global->Stacksize() - 1;
    
    for (; i >= 0 && !global->DStack(i)->isFrame(); i--) {}
    
    if (i >= 0)
        return global->DStack(i);
    return NULL;

}

Tamgu* TamguCode::Declaration(short id) {
    long i = global->Stacksize() - 1;
    
    for (; i >= 0 && !global->DStack(i)->isDeclared(id); i--) {}
    
    if (i >= 0)
        return global->DStack(i)->Declaration(id);
    return NULL;
}

Tamgu* TamguCode::Declaration(short id, Tamgu* parent) {
	if (parent->isDeclared(id))
		return parent->Declaration(id);

    long i = global->Stacksize() - 1;
    
    for (; i >= 0 && !global->DStack(i)->isDeclared(id); i--) {}
    
    if (i >= 0)
        return global->DStack(i)->Declaration(id);
    return NULL;
}

bool TamguCode::isDeclared(short id) {
    long i = global->Stacksize() - 1;
    
    for (; i >= 0 && !global->DStack(i)->isDeclared(id); i--) {}
    
    return (i >= 0);
}

Tamgu* TamguMainFrame::Declaration(short idname) {
    if (declarations.check(idname))
        return declarations.get(idname);
    return NULL;
}

void TamguMainFrame::Declare(short idname, Tamgu* a) {
	declarations[idname] = a;
	if (a->isFunction())
		exported[idname] = true;
}

bool TamguMainFrame::isDeclared(short idname) {
    return declarations.check(idname);
}

inline Tamgu* ThreadStruct::GetTopFrame() {
    long i = stack.last - 1;
    for (; i >= 0 && !stack.vecteur[i]->isFrame(); i--) {}
    if (i >= 0)
        return stack.vecteur[i];
    
	return NULL;
}

Tamgu* TamguGlobal::GetTopFrame(short idthread) {
	return threads[idthread].GetTopFrame();
}

inline Tamgu* ThreadStruct::Declarator(short id) {
    long i = stack.last - 1;

    for (; i >= 0 && !stack.vecteur[i]->isDeclared(id); i--) {}

    if (i >= 0)
        return stack.vecteur[i];
	return aNULL;
}


Tamgu* TamguGlobal::Declarator(short id, short idthread) {
	return threads[idthread].Declarator(id);
}

inline Tamgu* ThreadStruct::Getdefinition(short id) {
    long i = stack.last - 1;
    for (; i >= 0 && !stack.vecteur[i]->Declaration(id); i--) {}
    if (i >= 0)
        return stack.vecteur[i]->Declaration(id);
	return aNULL;
}

inline Tamgu* ThreadStruct::Declaration(short id) {
    long i = stack.last - 1;
    for (; i >= 0 && !stack.vecteur[i]->Declaration(id); i--) {}
    if (i >= 0)
        return stack.vecteur[i]->Declaration(id);
    return NULL;
}


Tamgu* TamguGlobal::Declaration(short id, short idthread) {
    return threads[idthread].Declaration(id);
}

Tamgu* TamguGlobal::Getframedefinition(short frname, short idname, short idthread) {
    return threads[idthread].variables.get(frname).back()->Declaration(idname);
}

Tamgu* TamguGlobal::Getdefinition(short id, short idthread, Tamgu* current) {
	if (current != aNULL) {
		current = current->Declaration(id);
		if (current != NULL)
			return current;
	}

	return threads[idthread].Getdefinition(id);
}

inline bool ThreadStruct::isDeclared(short id) {
    long i = stack.last - 1;
    for (; i >= 0 && !stack.vecteur[i]->isDeclared(id); i--) {}
    return (i >= 0);
}

bool TamguGlobal::isDeclared(short id, short idthread) {
	if (threads[idthread].variables.check(id) && threads[idthread].variables[id].last)
		return true;
	return false;
}
//---------------------------------------------------------------------
Tamgu* TamguCode::C_expression(x_node* xn, Tamgu* parent) {
    //We only care, if the first element in node is negation...
    if (xn->nodes[0]->token == "negation" && xn->nodes.size() == 2) {
        long name = global->Getid("not");
        Tamgu* call = new TamguCallProcedure(name, global, parent);
        Traverse(xn->nodes[1], call);
        return call;
    }
    
    Tamgu* a;
    Tamgu* res = NULL;
    for (size_t i = 0; i < xn->nodes.size(); i++) {
        a = Traverse(xn->nodes[i], parent);
        if (res == NULL)
            res = a;
    }
    
    return res;
}

Tamgu* TamguCode::C_parameterdeclaration(x_node* xn, Tamgu* parent) {
	Tamgu* top = global->Topstack();

	string& type = xn->nodes[0]->value;
	bool isprivate = false;
	if (xn->nodes[0]->nodes[0]->token == "private") {
		isprivate = true;
		type = xn->nodes[0]->nodes[1]->value;
	}

	if (global->symbolIds.find(type) == global->symbolIds.end()) {
        if (name_space != "") {
            type = name_space + type;
            if (global->symbolIds.find(type) == global->symbolIds.end()) {
                stringstream message;
                message << "Unknown type: '" << xn->nodes[0]->value << "'";
                throw new TamguRaiseError(message, filename, current_start, current_end);
            }
        }
        else {
            stringstream message;
            message << "Unknown type: '" << type << "'";
            throw new TamguRaiseError(message, filename, current_start, current_end);
        }
	}

	short tid = global->symbolIds[type];
	Tamgu* element = NULL;
    if (!global->newInstance.check(tid)) {
		stringstream message;
		message << "Unknown type: '" << type << "'";
		throw new TamguRaiseError(message, filename, current_start, current_end);
	}

    string name;
    if (name_space.empty())
        name = xn->nodes[1]->value;
    else
        name = name_space + xn->nodes[1]->value;

    short idname = global->Getid(name);
    if (global->newInstance.check(idname)) {
        stringstream message;
        message << "Variable name cannot be a type: " << name;
        throw new TamguRaiseError(message, filename, current_start, current_end);
    }

	if (top->isDeclared(idname)) {
		stringstream message;
		message << "This variable has already been declared: " << xn->nodes[1]->value;
		throw new TamguRaiseError(message, filename, current_start, current_end);
	}

	//We declare a variable, which we store in our declaration domain...
	TamguVariableDeclaration* a;
    if (tid == a_self || tid == a_let) {
        if (parent->isTaskellFunction())
            a = new TamguTaskellSelfVariableDeclaration(global, idname, tid, parent);
        else
            a = new TamguSelfVariableDeclaration(global, idname, tid, parent);
    }
	else {
		if (parent->isMainFrame()) {
			a = new TamguGlobalVariableDeclaration(global, idname, tid, isprivate, false, parent);
			global->Storevariable(0, idname, aNOELEMENT); //a dummy version to avoid certain bugs in the console
			//Basically, if a program fails before allocating this variable, and the variable is still requested in the console, it might crash...
		}
		else {
            if (parent->isFrame()) {
                if (global->atomics.check(tid))
                    a = new TamguFrameAtomicVariableDeclaration(global, idname, tid, isprivate, iscommon, parent);
                else
                    a = new TamguFrameVariableDeclaration(global, idname, tid, isprivate, iscommon, parent);
            }
            else {
                if (parent->isTaskellFunction())
                    a = new TamguTaskellVariableDeclaration(global, idname, tid, isprivate, false, parent);
                else {
                    if (global->atomics.check(tid))
                        a = new TamguAtomicVariableDeclaration(global, idname, tid, isprivate, false, parent);
                    else
                        a = new TamguVariableDeclaration(global, idname, tid, isprivate, false, parent);
                }
            }
		}
	}

	if (xn->nodes.size() == 3) {
		TamguInstruction bloc;
		//this is a temporary affectation, in order to push the type of the variable
		//into its affectation value...
		bloc.action = a_assignement;
		bloc.instructions.push_back(a);
		a->choice = 0;
		Traverse(xn->nodes[2], &bloc);
		a->AddInstruction(bloc.instructions[1]);
	}

	top->Declare(idname, a);

	return element;
}

Tamgu* TamguCode::C_multideclaration(x_node* xn, Tamgu* parent) {
	Tamgu* top = global->Topstack();

	string& type = xn->nodes[0]->value;
    if (type == "window")
        windowmode = true;

    
	bool oldprive = isprivate;
	bool oldcommon = iscommon;
	bool oldconstant = isconstant;
	if (xn->nodes[0]->nodes.size() && xn->nodes[0]->nodes[0]->token == "feature") {
		string& s = xn->nodes[0]->nodes[0]->nodes[0]->value;
		if (s == "private") {
			isprivate = true;
			if (xn->nodes[0]->nodes[0]->nodes.size() != 1)
				iscommon = true;
		}
		else
		if (s == "common")
			iscommon = true;
		else
		if (s == "const")
			isconstant = true;
		type = xn->nodes[0]->nodes[1]->value;
	}

	size_t last = xn->nodes.size() - 1;
	bool recall = false;

	x_node* xnew;
	if (xn->nodes[last]->token == "declarationlist") {
		xnew = new x_node(xn->nodes[0]->token, type, xn);
		xn->nodes[last]->nodes.insert(xn->nodes[last]->nodes.begin(), xnew);
		recall = true;
	}

	if (global->symbolIds.find(type) == global->symbolIds.end()) {
        if (name_space != "") {
            type = name_space + type;
            if (global->symbolIds.find(type) == global->symbolIds.end()) {
                stringstream message;
                message << "Unknown type: '" << xn->nodes[0]->value << "'";
                throw new TamguRaiseError(message, filename, current_start, current_end);
            }
        }
        else {
            stringstream message;
            message << "Unknown type: '" << type << "'";
            throw new TamguRaiseError(message, filename, current_start, current_end);
        }
	}

	short tid = global->symbolIds[type];

    if (!global->newInstance.check(tid)) {
		stringstream message;
		message << "Unknown type: '" << type << "'";
		throw new TamguRaiseError(message, filename, current_start, current_end);
	}

    string name;
    if (name_space.empty())
        name = xn->nodes[1]->value;
    else
        name = name_space + xn->nodes[1]->value;
    
	short idname = global->Getid(name);
    if (global->newInstance.check(idname)) {
        stringstream message;
        message << "Variable name cannot be a type: '" << name << "'";
        throw new TamguRaiseError(message, filename, current_start, current_end);
    }
    
	if (tid == a_tamgu) {
		if (xn->nodes.size() != 3) {
			stringstream message;
			message << "Missing parameter: '" << name << "'";
			throw new TamguRaiseError(message, filename, current_start, current_end);
		}

		Tamgu* ret = Callingprocedure(xn->nodes[2], tid);
		top->Declare(idname, ret);
		return parent;
	}

	if (top->isDeclared(idname) && top->Declaration(idname) != aNOELEMENT) {
		stringstream message;
		message << "This variable has already been declared: " << name;
		throw new TamguRaiseError(message, filename, current_start, current_end);
	}

	if (top->isFunction()) {
		//this a specific case, where we cannot accept a variable which declared both in a function and in its frame...
		for (long i = 1; i < global->threads[0].stack.last - 1; i++) {
			if (global->threads[0].stack[i]->isFrame() && global->threads[0].stack[i]->isDeclared(idname)) {
				stringstream message;
				message << "This variable has already been declared as a frame variable: " << name;
				throw new TamguRaiseError(message, filename, current_start, current_end);
			}
		}
	}

	if (recall) {
		C_multideclaration(xn->nodes[last], parent);
		xnew = xn->nodes[last];
		xn->nodes.erase(xn->nodes.begin() + last);
		delete xnew;
	}


	//We declare a variable, which we store in our declaration domain...
	if (tid == a_predicate)
		global->predicates[idname] = new TamguPredicateFunction(global, NULL, idname);

	TamguVariableDeclaration* a;
    if (tid >= a_intthrough && tid <= a_mapthrough)
        a = new TamguThroughVariableDeclaration(idname, tid, name, type, parent);
    else {
        if (tid == a_self) {
            if (parent->isTaskellFunction())
                a = new TamguTaskellSelfVariableDeclaration(global, idname, tid, parent);
            else {
                a = new TamguSelfVariableDeclaration(global, idname, tid, parent);
                if (parent->isFrame() && !parent->isMainFrame())
                    global->framevariables[idname] = a;
            }
        }
        else
            if (tid == a_let) {//in this case, we postpone the storage. If an initalization is provided then the type of this element will be the type of its initialization
                if (parent->isTaskellFunction())
                    a = new TamguTaskellSelfVariableDeclaration(global, idname, tid, NULL);
                else
                    a = new TamguSelfVariableDeclaration(global, idname, tid, NULL);
            }
            else {
                if (parent->isMainFrame()) {
                    if (idcode) {
                        //If a piece of code is loaded with a tamgu variable
                        //then we might have a problem with global variables names that could collide
                        //in this case, we modify slightly the variable name in order to avoid this problem...
                        char ch[10];
                        sprintf_s(ch,10,"&%d",idcode);
                        string nm(name);
                        nm+=ch;
                        idname=global->Getid(nm);
                        global->idSymbols[idname]=name;
                    }
                    a = new TamguGlobalVariableDeclaration(global, idname, tid, isprivate, isconstant, parent);
                    global->Storevariable(0, idname, aNOELEMENT); //a dummy version to avoid certain bugs in the console
                }
                else {
                    if (parent->isFrame()) {
                        if (global->atomics.check(tid))
                            a = new TamguFrameAtomicVariableDeclaration(global, idname, tid, isprivate, iscommon, parent);
                        else
                            a = new TamguFrameVariableDeclaration(global, idname, tid, isprivate, iscommon, parent);
                    }
                    else {
                        if (parent->isTaskellFunction())
                            a = new TamguTaskellVariableDeclaration(global, idname, tid, isprivate, isconstant, parent);
                        else {
                            if (global->atomics.check(tid))
                                a = new TamguAtomicVariableDeclaration(global, idname, tid, isprivate, isconstant, parent);
                            else
                                a = new TamguVariableDeclaration(global, idname, tid, isprivate, isconstant, parent);
                        }
                    }
                }
            }
    }

    if (tid != a_let)
        top->Declare(idname, a);

	if (xn->nodes.size() >= 3) {
		for (size_t pos = 2; pos < xn->nodes.size(); pos++) {
			if (xn->nodes[pos]->token == "depend") {
                //The "with" function is detected here.
                //The callback function is then stored into the variable.
                //It will be passed to our actual object through Newinstance, which has two parameters...
				a->choice = 1;
				string funcname = xn->nodes[pos]->nodes[0]->value;
				short idfuncname = global->Getid(funcname);
				Tamgu* func = Declaration(idfuncname);
				if (func == NULL || !func->isFunction()) {
					stringstream message;
					message << "We can only associate a function through 'with': " << name;
					throw new TamguRaiseError(message, filename, current_start, current_end);
				}

				if (func->isVariable()) {
					x_node nvar("variable", "", xn);
					creationxnode("word", global->Getsymbol(func->Name()), &nvar);
					func = Traverse(&nvar, parent);
				}

				a->function = func;
                if (!a->Checkarity()) {
                    stringstream message;
                    message << "Wrong number of arguments or incompatible argument: '" << funcname << "' for '" << name << "'";
                    throw new TamguRaiseError(message, filename, current_start, current_end);
                }
				continue;
			}

			if (xn->nodes[pos]->token == "wnexpressions") {
				//This is a temporary assignment, in order to push the type of the variable
				//into its assignment value...
				TamguInstruction bloc;
				bloc.action = a_assignement;
				bloc.instructions.push_back(a);
				a->choice = 0;
				Traverse(xn->nodes[pos], &bloc);
				a->AddInstruction(bloc.instructions[1]);
				continue;
			}

			if (xn->nodes[pos]->token == "parameters") {
				//Then we need to call a _initial method...
				if (!global->newInstance[tid]->isDeclared(a_initial)) {
					stringstream message;
					message << "Missing '_initial' function for this object: '" << global->idSymbols[tid] << " " << name << "'";
					throw new TamguRaiseError(message, filename, current_start, current_end);
				}
				TamguCall* call;
				if (global->newInstance[tid]->isFrame()) {
					call = new TamguCallFrameFunction((TamguFrame*)global->newInstance[tid]->Frame(), a_initial, global, a);
					Traverse(xn->nodes[pos], call);
					if (!call->Checkarity()) {
						stringstream message;
						message << "Wrong number of arguments or incompatible argument in '_initial' function for this object: '" << global->idSymbols[tid] << " " << name << "'";
						throw new TamguRaiseError(message, filename, current_start, current_end);
					}
				}
				else {
					call = new TamguCallFromCall(a_initial, global, a);
					Traverse(xn->nodes[pos], call);
				}
				continue;
			}

			//this is an assignment
			TamguInstructionASSIGNMENT inst(NULL, NULL);
			inst.action = a_assignement;
			inst.instructions.push_back(a);
            if (parent->isTaskellFunction() && xn->nodes[pos]->token == "hmetafunctions") {
                x_node x("telque", "");
                x.nodes.push_back(xn->nodes[pos]);
                Traverse(&x, &inst);
                x.nodes.clear();
            }
            else
                Traverse(xn->nodes[pos], &inst);
			a->AddInstruction(inst.instructions[1]);
		}
	}
	else {
		if (global->newInstance[tid]->isFrame() && global->newInstance[tid]->isDeclared(a_initial)) {
            TamguFrame* localframe = (TamguFrame*)global->newInstance[tid]->Frame();
			TamguCall* call = new TamguCallFrameFunction(localframe, a_initial, global, a);
			if (!call->Checkarity()) {
                stringstream message;
                if (localframe == NULL || localframe->Declaration(a_initial) == NULL)
                    message << "No '_initial' function' for " << global->idSymbols[tid] << " '" << name << "'";
                else {
                    if (call->Size() == 0)
                        message << "No '_initial' function' for " << global->idSymbols[tid] << " '" << name << "' with no arguments";
                    else
                        message << "No '_initial' function' for " << global->idSymbols[tid] << " '" << name << "' with " << call->Size() << " arguments";
                }
				throw new TamguRaiseError(message, filename, current_start, current_end);
			}
		}
	}

    if (tid == a_let) {
        //This a specific piece of code to replace a let variable with its type, when an initialization is provided...
        //let i=10; i will be replaced with a integer variable...
        tid = ((TamguSelfVariableDeclaration*)a)->Checklettype();
        TamguSelfVariableDeclaration* alet = (TamguSelfVariableDeclaration*)a;
        if (tid != a_let) {
            if (parent->isMainFrame()) {
                if (idcode) {
                    //If a piece of code is loaded with a tamgu variable
                    //then we might have a problem with global variables names that could collide
                    //in this case, we modify slightly the variable name in order to avoid this problem...
                    char ch[10];
                    sprintf_s(ch,10,"&%d",idcode);
                    string nm(name);
                    nm+=ch;
                    idname=global->Getid(nm);
                    global->idSymbols[idname]=name;
                }
                a = new TamguGlobalVariableDeclaration(global, idname, tid, isprivate, isconstant, parent);
                global->Storevariable(0, idname, aNOELEMENT); //a dummy version to avoid certain bugs in the console
            }
            else {
                if (parent->isFrame()) {
                    if (global->atomics.check(tid))
                        a = new TamguFrameAtomicVariableDeclaration(global, idname, tid, isprivate, iscommon, parent);
                    else
                        a = new TamguFrameVariableDeclaration(global, idname, tid, isprivate, iscommon, parent);
                }
                else {
                    if (parent->isTaskellFunction())
                        a = new TamguTaskellVariableDeclaration(global, idname, tid, isprivate, isconstant, parent);
                    else {
                        if (global->atomics.check(tid))
                            a = new TamguAtomicVariableDeclaration(global, idname, tid, isprivate, isconstant, parent);
                        else
                            a = new TamguVariableDeclaration(global, idname, tid, isprivate, isconstant, parent);
                    }
                }
            }
            a->Copiing(alet);
            top->Declare(idname, a);
            alet->Remove();
        }
        else {
            parent->AddInstruction(alet);
            top->Declare(idname, alet);
            if (parent->isMainFrame())
                global->Storevariable(0, idname, aNOELEMENT);
        }
    }
        
	isprivate = oldprive;
	iscommon = oldcommon;
	isconstant = oldconstant;

	return aNULL;
}

Tamgu* TamguCode::C_subfunc(x_node* xn, Tamgu* parent) {
	string name = xn->nodes[0]->nodes[0]->value;
	if (xn->nodes[0]->nodes[0]->nodes.size() != 1)
		name = xn->nodes[0]->nodes[0]->nodes[1]->value;

    bool checkmethodarity = false;
    short tyvar = -1;
	parent->Addfunctionmode();
	short id = global->Getid(name);
	Tamgu* function = NULL;
	//We have then two cases, either parent is a frame instance or it is a variable...
	if (parent->isFrame()) {
		Tamgu* frame = parent->Frame();
		function = frame->Declaration(id);
		if (function == NULL || function->isPrivate() || !function->isFunction()) {
            //We check if it is not a derivation from an extension
            if (!((TamguFrame*)frame)->theextensionvar) {
                stringstream message;
                message << "Unknown function: '" << name << "'";
                throw new TamguRaiseError(message, filename, current_start, current_end);
            }
            else
                tyvar = ((TamguFrame*)frame)->Topframe()->thetype;
		}
        else
            function = new TamguCallFrameFunction((TamguFrame*)frame, id, global, parent);
	}
    
	if (function == NULL) {
        if (tyvar == -1)
            tyvar = parent->Typevariable();
        if (tyvar==a_tamgu) {
            
            if (id == a_methods) { //.methods() is the method that returns all methods within a tamgu objet...
                function = new TamguCallFromCall(id, global, parent);
            }
            else {
                function = parent->Declaration(id);
                if (function!=NULL)
                    function = CreateCallFunction(function, parent);
            }
        }
        else
		if (global->commons.check(id) && !global->extensionmethods.check(id))
			function = new TamguCallCommonMethod(id, global, parent);
		else {
            if (global->extensions.check(tyvar) && global->extensions[tyvar]->isDeclared(id))
				function = new TamguCallCommonMethod(id, global, parent);
			else {
                if (global->checkmethod(tyvar, id)) {
					function = new TamguCallMethod(id, global, parent);
                    checkmethodarity = true;
                }
				else {
					if (global->allmethods.check(id)) {
						if (tyvar == a_self || tyvar == a_let || xn->token == "subfunc" || xn->token == "subfuncbis") {
							if (global->extensionmethods.check(id))
								function = new TamguCallCommonMethod(id, global, parent);
							else
								function = new TamguCallFromCall(id, global, parent);
						}
					}
					else {
						if (global->framemethods.check(id))
							function = new TamguCallFrameFunction(NULL, id, global, parent);
					}
				}
			}
		}
	}

	if (function == NULL) {
		stringstream message;
		message << "Unknown method: '" << name << "'";
		throw new TamguRaiseError(message, filename, current_start, current_end);
	}

	//we then parse the arguments of the function call
	if (xn->nodes[0]->nodes.size() >= 2) {
		x_node* sub = xn->nodes[0];
		if (sub->nodes[1]->token != "parameters")
			function->Addfunctionmode();
        Tamgu* frame = global->Topstack();
        //If this subfunc is called from a frame variable
        //then the top of the stack is the frame to which
        //this variable belongs.
        //We need to remove this element from the stack
        //in order to properly assess the arguments.
        //Otherwise, the arguments could be confused with
        //arguments in the frame.
        bool pushback = false;
        if (frame->isFrameinstance()) {
            global->Popstack();
            pushback = true;
        }
		parent = Traverse(sub->nodes[1], function);
        if (pushback)
            global->Pushstack(frame);
		if (sub->nodes.size() == 3) {
			function->Addfunctionmode();
			parent = Traverse(sub->nodes[2], function);
		}
	}

    if (checkmethodarity && !global->checkarity(tyvar, id, function->Size())) {
        stringstream message;
        message << "Wrong number of arguments or incompatible argument: '" << name << "'";
        throw new TamguRaiseError(message, filename, current_start, current_end);
    }
    else {
        if (!function->Checkarity()) {
            stringstream message;
            message << "Wrong number of arguments or incompatible argument: '" << name << "'";
            throw new TamguRaiseError(message, filename, current_start, current_end);
        }
    }

	return parent;
}

Tamgu* TamguCode::Callingprocedure(x_node* xn, short id) {
	TamguCallProcedure proc(id);
	Traverse(xn, &proc);
	//First we execute our code to evaluate the current global variables...
	Tamgu* call = global->EvaluateMainVariable();
	if (!call->isError())
		call = proc.Eval(&mainframe, aNULL, 0);

	if (call->isError()) {
		stringstream message;
		message << call->String();
		throw new TamguRaiseError(message, filename, current_start, current_end);
	}
	return call;
}

bool TamguCode::isaFunction(string& name, short id) {
	//Is it a procedure?
	if (global->procedures.check(id))
		return true;

	//This HAS to be a function declaration...
	if (isDeclared(id)) {
		Tamgu* call = Declaration(id);
		if (call->isTaskellFunction())
			return true;
		if (call->isFunction())
			return true;
	}
	return false;
}

Tamgu* TamguCode::C_regularcall(x_node* xn, Tamgu* parent) {
	if (isnegation(xn->nodes[0]->token)) {
		Tamgu* call = Traverse(xn->nodes[1], parent);
		call->Setnegation(true);
		return call;
	}

	string name = xn->nodes[0]->value;
	short id = global->Getid(name);
    short idname = -1;
	string params;
    bool terminal_predicate = false;
    TamguPredicateVariable* var = NULL;

    if (xn->token == "predicatecall") {
		params = "predicateparameters";
        if (name[0] == '_' && isdigit(name[1]) && name.size() == 2) {
            name += "%"+currentpredicatename;
            idname = global->Getid(name);
            if (!global->predicatevariables.check(idname)) {
                var = new TamguPredicateVariable(global, idname, NULL);
                global->predicatevariables[idname] = var;
                mainframe.Declare(id, var);
            }
            else
                var = global->predicatevariables[idname];
            id = a_universal;
        }
        else {
            if (name.back() == '#') {
                name = name.substr(0, name.size()-1);
                terminal_predicate = true;
                id = global->Getid(name);
            }
        }
    }
	else
		params = "parameters";

	//It could be a predicate, then two cases as part of an expression or as a call to evaluation
	if (global->predicates.check(id) || (id == a_universal && params == "predicateparameters")) {
		//then it is a PredicateInstance
		if (parent->Type() != a_predicateruleelement && parent->Type() != a_parameterpredicate) {
			global->predicatevariables.clear();
			TamguInstructionLaunch* kbloc = new TamguInstructionLaunch(global, parent);
			parent = kbloc;
		}
        
		TamguPredicate* kx = global->predicates[id];
		if (kx != NULL && kx->isPredicateMethod()) {
			kx = (TamguPredicate*)kx->Newinstance(0, parent);
			parent->AddInstruction(kx);
		}
        else {
            if (idname != -1)
                kx = new TamguPredicateAsVariable(global, parent, var, id, idname);
            else {
                kx = new TamguPredicate(id, global, a_predicate, parent);
                kx->terminal = terminal_predicate;
            }
        }

        
        //There could be a tail function to be called with
        if (xn->nodes.back()->token == params)
            ComputePredicateParameters(xn->nodes.back(), kx);
        else {
            for (long i = 1; i < xn->nodes.size(); i++) {
                if (xn->nodes[i]->token == params)
                    ComputePredicateParameters(xn->nodes[i], kx);
            }
            Traverse(xn->nodes.back(), kx);
        }

		if (!kx->Checkarity()) {
			stringstream message;
			message << "Wrong number of arguments or incompatible argument: '" << name << "'";
			throw new TamguRaiseError(message, filename, current_start, current_end);
		}
		return kx;
	}

	//It it is a concept, which is called from within a predicate...
	//concepts are different from term, in particular, their evaluation is done outside of the predicate realm.
	if (global->concepts.check(id) && parent->Type() == a_parameterpredicate) {
		TamguPredicateConcept* kx = new TamguPredicateConcept(global, id, parent);
		if (xn->nodes.back()->token == params)
			ComputePredicateParameters(xn->nodes.back(), kx);
		return kx;
	}

	//It could be a term, but we favor any frame or type interpretation first...
	if (!global->newInstance.check(id)) {
		if (global->terms.check(id) || parent->Type() == a_parameterpredicate) {
			TamguPredicateTerm* kx = new TamguPredicateTerm(global, id, parent);
			if (global->terms.find(id) == global->terms.end())
				global->terms[id] = name;
			if (xn->nodes.back()->token == params)
				ComputePredicateParameters(xn->nodes.back(), kx);
			return kx;
		}
	}

	Tamgu* call = NULL;
	if (global->systemfunctions.find(name) != global->systemfunctions.end()) {
        if (!parent->isMainFrame() && global->systemfunctions[name]) {
            stringstream message;
            message << "You cannot call '"<<name << "' from a function";
            throw new TamguRaiseError(message, filename, current_start, current_end);
        }
		Callingprocedure(xn->nodes[1], id);
        //if the value stored in systemfunctions is true, then it means that we have
        //a local call otherwise, it means that the function should be called twice.
        //At compile time and at run time
        if (global->systemfunctions[name])
            return parent;
	}

	if (id == a_return)
		call = new TamguCallReturn(global, parent);
	else {
		short framename = 0;
		if (xn->nodes[0]->nodes.size() == 2) {
			//A frameup call
			framename = global->Getid(xn->nodes[0]->nodes[0]->nodes[0]->value);
			if (framename != a_mainframe) {
				TamguFrame* top = global->frames[framename];
				if (top == NULL) {
					stringstream message;
					message << "Unknown frame: '" << xn->nodes[0]->nodes[0]->nodes[0]->value << "'";
					throw new TamguRaiseError(message, filename, current_start, current_end);
				}
				call = Frame();
				if (call == NULL || !call->isParent(top)) {
					stringstream message;
					message << "Frame function unreachable: '" << name << "'";
					throw new TamguRaiseError(message, filename, current_start, current_end);
				}
				name = xn->nodes[0]->nodes[1]->value;
				id = global->Getid(name);
				call = new TamguCallTopFrameFunction(top, id, global, parent);
			}
			else {
				//The function call: _MAIN::call(..), which means that we force the function to be a global function outside of a frame...
				name = xn->nodes[0]->nodes[1]->value;
				id = global->Getid(name);
			}
		}

		if (!framename || framename == a_mainframe) {
			//Is it a procedure?
			//Is it a method? It should have been cleared when the variable was detected...
			if (global->allmethods.check(id) && parent->isCall()) {
				//We create a call method...
				parent->Addfunctionmode();
				if (global->commons.check(id))
					call = new TamguCallCommonMethod(id, global, parent);
				else
					call = new TamguCallFromCall(id, global, parent);
			}
			else {
				if (global->procedures.check(id)) {
					//We create a procedure call
					call = new TamguCallProcedure(id, global, parent);
				}
			}

			//This HAS to be a function declaration...
			if (call == NULL && isDeclared(id)) {
                
				if (!framename)
					call = Declaration(id);
                else //In this case, we are calling a function from the main frame... _MAIN::call...
                    call = mainframe.Declaration(id);
                
                if (call != NULL) {
                    if (call->Typevariable() == a_fibre) {
                        call = new TamguCallFibre(global, parent);
                        if (fibrevariables.check(id))
                            call->AddInstruction(fibrevariables[id]);
                        else {
                            x_node xn;
                            creationxnode("name",name,&xn);
                            C_variable(&xn, call);
                            fibrevariables[id] = ((TamguCallFibre*)call)->variable;
                        }
                    }
                    else {
                        if (call->isAlias()) {
                            call = new TamguCallAlias((TamguAlias*)call, global, parent);
                            call->Addargmode();
                        }
                        else {
                            if (call->isTaskellFunction()) {
                                call = new TamguCallFunctionArgsTaskell((TamguFunctionLambda*)call, global, parent);
                            }
                            else {
                                if (call->isFunction()) {
                                    if (call->isThread())
                                        call = new TamguCallThread(call, global, parent);
                                    else {
                                        if (call->isVariable())
                                            call = new TamguFunctionDeclarationCall(call->Name(), global, parent);
                                        else {
                                            if (call->Type() == a_lisp)
                                                call = new TamguCallLispFunction(call, global, parent);
                                            else
                                                call = CreateCallFunction(call, parent);
                                        }
                                    }
                                }
                                else
                                    call = NULL;
                            }
                        }
                    }
                }
            }
        }
    }


	//We then parse the arguments of that function...
	if (call == NULL) {
		if (global->allmethods.check(id)) {
			call = new TamguGetMethod(id, global, parent);
			if (xn->nodes.size() != 2) {
				stringstream message;
				message << "Missing argument in: '" << name << "'";
				throw new TamguRaiseError(message, filename, current_start, current_end);
			}
			Traverse(xn->nodes[1], call);
			call->CheckTaskellComposition();
			return call;
		}
	}

	if (call == NULL) {
		stringstream message;
		message << "Unknown function or procedure: '" << name << "'";
		throw new TamguRaiseError(message, filename, current_start, current_end);
	}

	parent->Addargmode();
    //If we have parameters...
    if (xn->nodes.size() >= 2) {
        Traverse(xn->nodes[1], call);
        if (xn->nodes.size() == 3) {
            call->Addfunctionmode();
            Traverse(xn->nodes[2], call);
        }
    }
    
	if (!call->Checkarity()) {
		stringstream message;
		message << "Wrong number of arguments or incompatible argument: '" << name << "'";
		throw new TamguRaiseError(message, filename, current_start, current_end);
	}

    if (call->isAlias())
        call->Setstopindex();
    
	return call;
}

static bool Replacement(x_node* x, x_node* rep, vector<x_node*>& keep) {
	if (x->token == "affectation") {
		keep.push_back(x->nodes[2]);
		x->nodes[2] = rep;
		return true;
	}

	for (int i = 0; i < x->nodes.size(); i++) {
		if (Replacement(x->nodes[i], rep, keep))
			return true;
	}
	return false;
}

Tamgu* TamguCode::C_conceptfunction(x_node* xn, Tamgu* parent) {
	string funcname = xn->nodes[1]->value;
	short idf = global->Getid(funcname);
	Tamgu* kfunc = NULL;
	kfunc = Declaration(idf, parent);
	//We have a WITH description
	if (kfunc == NULL || !kfunc->isFunction()) {
		stringstream message;
		message << "Unknown function: '" << funcname << "'";
		throw new TamguRaiseError(message, filename, current_start, current_end);
	}

	if (xn->nodes[0]->value == "concept")
		global->conceptfunction = (TamguFunction*)kfunc;
	else
	if (xn->nodes[0]->value == "property")
		global->propertyfunction = (TamguFunction*)kfunc;
	else
		global->rolefunction = (TamguFunction*)kfunc;

	return parent;
}

Tamgu* TamguCode::C_dataassignment(x_node* xn, Tamgu* parent) {
    static tokenizer_result<string> xr;
    static bnf_tamgu bnf;
    
	string name = xn->nodes[0]->value;

	short id = global->Getid(name);

	if (global->frames.check(id) == false) {
		stringstream message;
		message << "Expecting a frame: '" << name << "'";
		throw new TamguRaiseError(message, filename, current_start, current_end);
	}

	//Then, we simply need a method...
	//We create a call method...
	//the parameter in this case is the object we need...
    
	if (global->procedures.check(id)) {
		stringstream framecode;
		string var("_");
		var += name;

		framecode << name << " " << var << ";";
		int i;
		for (i = 1; i < xn->nodes.size(); i++) {
			framecode << var << "." << xn->nodes[i]->nodes[0]->value << "= _#;";
		}
		framecode << "return(" << var << ");";
        
        string s_frame = framecode.str();
        global->tamgu_tokenizer.tokenize<string>(s_frame, xr);
		
		x_node* xstring = bnf.x_parsing(&xr, FULL);
		setstartend(xstring, xn);

		vector<x_node*> keep;
		for (i = 1; i < xn->nodes.size(); i++)
			Replacement(xstring->nodes[0]->nodes[i]->nodes[0]->nodes[0], xn->nodes[i]->nodes[1], keep);

		TamguFunction* ai = new TamguFunction(1, global);
		ai->returntype = id;
		ai->choice = true;
		ai->adding = true;
		parent->AddInstruction(ai);
		xstring->nodes[0]->token = "bloc";
		Traverse(xstring->nodes[0], ai);

		for (i = 1; i < xn->nodes.size(); i++)
			Replacement(xstring->nodes[0]->nodes[i]->nodes[0]->nodes[0], keep[i - 1], keep);

        delete xstring;
		return parent;
	}

	stringstream message;
	message << "Expecting a frame: '" << name << "'";
	throw new TamguRaiseError(message, filename, current_start, current_end);

}

Tamgu* TamguCode::C_taskellcall(x_node* xn, Tamgu* parent) {
	string name = xn->nodes[0]->value;

	if (xn->nodes[0]->token == "power") {
		short nm = global->Getid(xn->nodes[1]->nodes[0]->value);
		short op = global->string_operators[name];
		if (op == a_square)
			return new TamguCallSQUARE(global, nm, parent);
		if (op == a_cube)
			return new TamguCallCUBE(global, nm, parent);
	}

	short id = global->Getid(name);

	//Then, we simply need a method...
	//We create a call method...
	//the parameter in this case is the object we need...

	if (global->procedures.check(id)) {
		TamguCallProcedure* call = new TamguCallProcedure(id, global, parent);
		if (xn->nodes.size() == 2)
			Traverse(xn->nodes[1], call);
		call->CheckTaskellComposition();
		return call;
	}

	if (global->commons.check(id)) {
		TamguGetCommon* call = new TamguGetCommon(id, global, parent);
		if (xn->nodes.size() != 2) {
			stringstream message;
			message << "Missing argument in: '" << name << "'";
			throw new TamguRaiseError(message, filename, current_start, current_end);
		}
		Traverse(xn->nodes[1], call);
		call->CheckTaskellComposition();
		return call;
	}

	if (global->allmethods.check(id)) {
		TamguGetMethod* call = new TamguGetMethod(id, global, parent);
		if (xn->nodes.size() != 2) {
			stringstream message;
			message << "Missing argument in: '" << name << "'";
			throw new TamguRaiseError(message, filename, current_start, current_end);
		}
		Traverse(xn->nodes[1], call);
		call->CheckTaskellComposition();
		return call;
	}

	if (global->framemethods.check(id)) {
		TamguCallFrameMethod* call = new TamguCallFrameMethod(id, global, parent);
		if (xn->nodes.size() != 2) {
			stringstream message;
			message << "Missing argument in: '" << name << "'";
			throw new TamguRaiseError(message, filename, current_start, current_end);
		}
		Traverse(xn->nodes[1], call);
		call->CheckTaskellComposition();
		return call;
	}

	Tamgu* call = Declaration(id);
	if (call != NULL) {
		short tyvar = call->Typevariable();
		if (tyvar == a_lambda) {
			if (xn->nodes.size() == 2) {
				call = new TamguCallFunctionArgsTaskell((TamguFunctionLambda*)call, global, parent);
				Traverse(xn->nodes[1], call);
			}
			else
				call = new TamguCallFunctionTaskell((TamguFunctionLambda*)call, global, parent);

            if (!call->Checkarity()) {
                if (parent->Type() != a_taskellinstruction || !insidecall || call->Body(0)->Size() < call->Size()) {
                    stringstream message;
                    message << "Wrong number of arguments or incompatible argument: '" << name << "'";
                    throw new TamguRaiseError(message, filename, current_start, current_end);
                }
                ((TamguCall*)call)->curryfied = true;
			}
            
			call->CheckTaskellComposition();
			return call;
		}

		if (tyvar == a_function || tyvar == a_lisp) {
            if (parent->Type() == a_taskellinstruction && insidecall)
                call = new TamguCallFunction(call, global, parent);
            else
                call = CreateCallFunction(call, parent);
            
			if (xn->nodes.size() == 2)
				Traverse(xn->nodes[1], call);

			if (!call->Checkarity()) {
                if (parent->Type() != a_taskellinstruction || !insidecall || call->Body(0)->Size() < call->Size()) {
                    stringstream message;
                    message << "Wrong number of arguments or incompatible argument: '" << name << "'";
                    throw new TamguRaiseError(message, filename, current_start, current_end);
                }
                ((TamguCall*)call)->curryfied = true;
			}
			call->CheckTaskellComposition();
			return call;
		}

		//second possibility, this is a simple variable...
		if (tyvar == a_self || tyvar == a_let || tyvar == a_call) {
			call = new TamguGetFunction(id, global, parent);
			//We create a call method...
			//the parameter in this case is the object we need...
			if (xn->nodes.size() == 2)
				Traverse(xn->nodes[1], call);

			call->CheckTaskellComposition();
			return call;
		}

	}

	stringstream message;
	message << "Unknown function or procedure: '" << name << "'";
	throw new TamguRaiseError(message, filename, current_start, current_end);
	return parent;
}

Tamgu* TamguCode::C_framevariable(x_node* xn, Tamgu* parent) {
    x_node* subxn = xn->nodes[0];
    Tamgu* av = NULL;

    short idname = parent->Typevariable();
    //We check if idtype is a frame
    if (!global->frames.check(idname) && idname != a_self && idname != a_let && !parent->isIndex()) {
        stringstream message;
        message << "This variable is not a frame type: '" << global->Getsymbol(idname) << " ";
        idname = parent->Name();
        message <<  global->Getsymbol(idname) << "'";
        throw new TamguRaiseError(message, filename, current_start, current_end);
    }
    
    if (subxn->token == "variable")
        idname = global->Getid(subxn->nodes[0]->value);
    else
        idname = global->Getid(xn->nodes[0]->value);
    
    Tamgu* dom = Declaror(idname);
    if (dom == NULL) {
        stringstream message;
        message << "Unknown frame variable: '" << global->Getsymbol(idname) << "'";
        throw new TamguRaiseError(message, filename, current_start, current_end);
    }
    
    if (dom->isFrame()) {
        Tamgu* a = dom->Declaration(idname);
        short tyvar = a->Typevariable();
        if (parent->isCallVariable())
            av = new TamguCallFromFrameVariable(a->Name(), tyvar, global, parent);
        else
            av = new TamguCallFrameVariable(a->Name(), (TamguFrame*)dom, tyvar, global, parent);
        
        if (subxn->nodes.size() != 1) {
            if (global->frames.check(tyvar))
                global->Pushstack(global->frames[tyvar]);
            
            Traverse(subxn->nodes[1], av);
            
            if (global->frames.check(tyvar))
                global->Popstack();
        }
        return av;
    }

    if (!global->framevariables.check(idname)) {
        stringstream message;
        if (subxn->token == "variable")
            xn = subxn;
        message << "This variable is not a frame field: '" << xn->nodes[0]->value << "'";
        throw new TamguRaiseError(message, filename, current_start, current_end);
    }
    
    av = new TamguCallFromFrameVariable(idname, 0, global, parent);

    if (subxn->nodes.size() != 1) {
        Traverse(subxn->nodes[1], av);
    }

    return av;
}


Tamgu* TamguCode::C_variable(x_node* xn, Tamgu* parent) {
    string name;
    if (name_space.empty())
        name = xn->nodes[0]->value;
    else
        name = name_space + xn->nodes[0]->value;
    
	short idname = global->Getid(name);

	Tamgu* av;
	short tyvar = 0;
	Tamgu* a = aNULL;

	if (global->systems.check(idname)) {
		switch (idname) {
		case a_null:
			av = aNULL;
			parent->AddInstruction(aNULL);
			break;
		case a_empty:
			av = aNOELEMENT;
			parent->AddInstruction(aNOELEMENT);
			break;
		case a_universal:
			av = aUNIVERSAL;
			parent->AddInstruction(aUNIVERSAL);
			break;
		default:
			av = new TamguCallSystemVariable(idname, global->systems[idname]->Typevariable(), global, parent);
		}
	}
	else {
		Tamgu* dom = Declaror(idname);
        if (idcode && dom==NULL) {
            char ch[10];
            sprintf_s(ch,10,"&%d",idcode);
            string nm(name);
            nm+=ch;
            idname=global->Getid(nm);
            dom = Declaror(idname);
            global->idSymbols[idname]=name;
        }

		if (dom == NULL) {
			//It could be a procedure or a method
			if (global->procedures.check(idname))
				av = new TamguProcedureParameter(idname, global, parent);
			else
			if (global->commons.check(idname))
				av = new TamguCommonParameter(idname, global, parent);
			else
			if (global->allmethods.check(idname))
				av = new TamguMethodParameter(idname, global, parent);
			else
			if (global->framemethods.check(idname))
				av = new TamguFrameMethodParameter(idname, global, parent);
			else {
                Tamgu* frame = parent->Frame();
                if (frame == NULL || !frame->isDeclared(idname)) {
                    stringstream message;
                    message << "Unknown variable: '" << name << "'";
                    throw new TamguRaiseError(message, filename, current_start, current_end);
                }
                a = frame->Declaration(idname);
                tyvar = a->Typevariable();
                if (parent->isCallVariable())
                    av = new TamguCallFromFrameVariable(a->Name(), tyvar, global, parent);
                else
                    av = new TamguCallFrameVariable(a->Name(), (TamguFrame*)frame, tyvar, global, parent);
            }
		}
		else {
            if (idname == a_this) {
                Tamgu* frame = global->GetTopFrame();
                if (frame ==  NULL) {
                    stringstream message;
                    message << "You cannot use 'this' out of a frame or an extension";
                    throw new TamguRaiseError(message, filename, current_start, current_end);
                }
                av = new TamguCallThis(frame->Typeframe(), global, parent);
            }
			else {
                a = dom->Declaration(idname);
                if (a->isPredicateVariable())
                    av = new TamguPredicateVariable(global, a->Name(), parent);
                else
                //This is a variable call
                if (a->Type() == a_tamgu)
                    av = new TamguCallTamguVariable(idname, (Tamgutamgu*)a, global, parent);
                else {
                    if (a->isAlias()) {
                        av = new TamguCallAlias((TamguAlias*)a, global, parent);
                    }
                    else {
                        tyvar = a->Typevariable();
                        switch (tyvar) {
                            case a_lambda:
                                av = new TamguFunctionTaskellParameter(idname, global, parent);
                                break;
                            case a_function:
                                av = new TamguFunctionParameter(idname, global, parent);
                                break;
                            case a_self:
                            case a_let:
                                if (dom->isFrame()) {
                                    a = dom->Declaration(idname);
                                    if (parent->isCallVariable())
                                        av = new TamguCallFromFrameVariable(a->Name(), tyvar, global, parent);
                                    else
                                        av = new TamguCallFrameVariable(a->Name(), (TamguFrame*)dom, tyvar, global, parent);
                                }
                                else
                                    av = new TamguCallSelfVariable(idname, tyvar, global, parent);
                                break;
                            case a_intthrough:
                            case a_longthrough:
                            case a_decimalthrough:
                            case a_floatthrough:
                            case a_stringthrough:
                            case a_ustringthrough:
                            case a_vectorthrough:
                            case a_mapthrough:
                                av = new TamguCallThroughVariable(idname, tyvar, name, parent);
                                break;
                            default: {
                                if (dom->isFrame()) {
                                    a = dom->Declaration(idname);
                                    if (parent->isCallVariable())
                                        av = new TamguCallFromFrameVariable(a->Name(), tyvar, global, parent);
                                    else
                                        av = new TamguCallFrameVariable(a->Name(), (TamguFrame*)dom, tyvar, global, parent);
                                }
                                else {
                                    if (dom->isFunction())
                                        av = new TamguCallFunctionVariable(idname, tyvar, global, parent);
                                    else {
                                        if (a->isConstant())
                                            av = new TamguCallConstantVariable(idname, tyvar, global, parent);
                                        else {
                                            if (dom->isMainFrame())
                                                av = new TamguCallGlobalVariable(idname, tyvar, global, parent);
                                            else
                                                av = new TamguCallVariable(idname, tyvar, global, parent);
                                        }
                                    }
                                }
                            }
                        }
                    }
				}
			}
		}
	}


	//we might have four cases then: interval, indexes, method or variable (in a frame)
    if (xn->nodes.size() != 1) {
        char modif = 0;
        if (global->frames.check(tyvar)) {
            modif = 1;
            global->Pushstack(global->frames[tyvar]);
            if (!global->frames[tyvar]->isFrameinstance()) {
                global->frames[tyvar]->investigate |= is_frameinstance;
                modif = 2;
            }
        }

		Traverse(xn->nodes[1], av);

        if (modif) {
            if (modif == 2)
                global->frames[tyvar]->investigate &= ~is_frameinstance;
            global->Popstack();
        }
    }
    
    if (av->isAlias())
        av->Setstopindex();

	return av;
}

Tamgu* TamguCode::C_indexes(x_node* xn, Tamgu* parent) {
	TamguIndex* idx = new TamguIndex(false, global, parent);
	TamguInstruction ai;

	if (xn->nodes[0]->token == "minus") {
		idx->signleft = true;
		Traverse(xn->nodes[0]->nodes[0], &ai);
	}
	else
		Traverse(xn->nodes[0], &ai);

	idx->AddInstruction(ai.instructions[0]);

	if (xn->nodes.size() == 2)
		Traverse(xn->nodes[1], idx);
    else {
        short type = parent->Typevariable();
        if (global->minimal_indexes.check(type))
            idx->minimal = true;
    }

    if (!idx->Checklegit()) {
        stringstream message;
        message << "Wrong Index Value";
        throw new TamguRaiseError(message, filename, current_start, current_end);
        return aNULL;
    }
    
    idx->Checkconst();
	return idx;
}

Tamgu* TamguCode::C_shapeindexes(x_node* xn, Tamgu* parent) {
    size_t nodesize = xn->nodes.size();
    
    if (TestFunction(xn->nodes[nodesize - 1]->token, false))
        nodesize--;


    TamguShape* idx = new TamguShape(global, parent);
    //The first element is an index
    
    for (long i = 0; i < nodesize; i++)
        Traverse(xn->nodes[i], idx);

    if (nodesize != xn->nodes.size()) {
        idx->stop = 2;
        Traverse(xn->nodes[nodesize], idx);
    }

    return idx;
}


Tamgu* TamguCode::C_interval(x_node* xn, Tamgu* parent) {
	size_t xsz = xn->nodes.size();

	TamguIndex* idx = new TamguIndex(true, global, parent);

	//If we have another function at the end of the structure, we do not want to take it into account
	if (TestFunction(xn->nodes[xsz - 1]->token, false))
		xsz--;

	if (xsz == 1) {
		idx->interval = true;
		idx->AddInstruction(aZERO);
		idx->AddInstruction(aNULL);
	}
	else {
		bool beforesep = true;
		for (int i = 0; i < xsz; i++) {
			if (xn->nodes[i]->token == "sep") {
				beforesep = false;
				if (idx->left == aNOELEMENT)
					idx->left = aNULL;
				continue;
			}

			TamguInstruction ai;

			if (xn->nodes[i]->token == "minus") {
				if (beforesep)
					idx->signleft = true;
				else
					idx->signright = true;
				Traverse(xn->nodes[i]->nodes[0], &ai);
			}
			else
				Traverse(xn->nodes[i], &ai);
			idx->AddInstruction(ai.instructions[0]);
		}
		if (!beforesep && idx->right == NULL)
			idx->right = aNULL;
	}

	//If we have a call
	if (xsz != xn->nodes.size()) {
		idx->interval = false;
		Traverse(xn->nodes[xsz], idx);
		idx->interval = true;
	}

    if (!idx->Checklegit()) {
        stringstream message;
        message << "Wrong Index Value";
        throw new TamguRaiseError(message, filename, current_start, current_end);
        return aNULL;
    }

    idx->Checkconst();
	return idx;
}

Tamgu* TamguCode::C_achar(x_node* xn, Tamgu* parent) {
    string thestr = xn->value;
    //compilemode indicates whether the code is compiled from a file or from a string while executing some code...
    if (compilemode)
        return global->ProvideConstString(thestr, parent);

    return new Tamgustring(thestr, NULL, parent);
}

Tamgu* TamguCode::C_astring(x_node* xn, Tamgu* parent) {
	string thestr;

	if (xn->token == "astring" || xn->token == "punct")
		thestr = xn->value;
	else
	if (xn->token == "astringdouble" && compilemode)
		replaceescapedcharacters(thestr, xn->value.substr(1, xn->value.size() - 2));
	else
		thestr = xn->value.substr(1, xn->value.size() - 2);

	//compilemode indicates whether the code is compiled from a file or from a string while executing some code...
	if (compilemode)
		return global->ProvideConstString(thestr, parent);

	return new Tamgustring(thestr, NULL, parent);
}

#ifndef Tamgu_REGEX
Tamgu* TamguCode::C_pstring(x_node* xn, Tamgu* parent) {
    stringstream message;
    message << "Posix regular expressions not available";
    throw new TamguRaiseError(message, filename, current_start, current_end);
    return aNULL;
}
#else
wregex* wgetposixregex(string& s);
regex* getposixregex(string& s);

Tamgu* TamguCode::C_pstring(x_node* xn, Tamgu* parent) {
    string thestr = xn->value.substr(1, xn->value.size() - 2);
    
    wregex* wa = wgetposixregex(thestr);
    
    if (wa == NULL) {
        stringstream message;
        message << "Unknown posix regular expression: '" << thestr << "'";
        throw new TamguRaiseError(message, filename, current_start, current_end);
    }
    
    regex* a = getposixregex(thestr);
    
    Tamguposixregularexpressionconstant* tre = new Tamguposixregularexpressionconstant(thestr, a, wa, global, parent);
    return tre;
}
#endif

//This a fstring, in which {..} expressions must be parsed again...
Tamgu* TamguCode::C_fstring(x_node* xn, Tamgu* parent) {
    string thestr = xn->value.substr(1, xn->value.size() - 2);
    
    //First we detect all the {...} expression
    string code;
    Tamgu* action;
    Tamguformat* vect = new Tamguformat(global, parent);
    long beginning = 0;
    long pos = thestr.find("{");
    long posc;
    while (pos != -1) {
        posc = thestr.find("}", pos);
        if (posc == -1) {
            stringstream message;
            message << "Format expression malformed: missing closing '}': '" << thestr << "'";
            throw new TamguRaiseError(message, filename, current_start, current_end);
        }
        if ((pos - beginning))
            action = new Tamgustring(thestr.substr(beginning, pos - beginning), global, vect);
        
        code = "_nop(" + thestr.substr(pos+1, posc - pos - 1) + ");";
        action = CompileFormat(code, vect);
        if (action->isError()) {
            stringstream message;
            message << "Wrong expression: '" << code << "': " << action->String();
            action->Release();
            throw new TamguRaiseError(message, filename, current_start, current_end);
        }
        beginning = posc + 1;
        pos = thestr.find("{", beginning);
    }
    
    if (beginning < (long)thestr.size() -1)
        action = new Tamgustring(thestr.substr(beginning, thestr.size()), global, vect);
    
    return vect;
}


Tamgu* TamguCode::C_rstring(x_node* xn, Tamgu* parent) {
    string thestr = xn->value.substr(1, xn->value.size() - 2);
    
    Au_automate* a = getautomate(thestr);
    
    if (a == NULL) {
        stringstream message;
        message << "Unknown tamgu regular expression: '" << thestr << "'";
        throw new TamguRaiseError(message, filename, current_start, current_end);
    }
    
    Tamguregularexpressionconstant* tre = new Tamguregularexpressionconstant(thestr, a, global, parent);
    return tre;
}

Tamgu* TamguCode::C_ustring(x_node* xn, Tamgu* parent) {
	string thestr;

	if (xn->token == "ustringdouble"  && compilemode)
		replaceescapedcharacters(thestr, xn->value.substr(1, xn->value.size() - 2));
	else
		thestr = xn->value.substr(1, xn->value.size() - 2);

	wstring res;
	s_utf8_to_unicode(res, USTR(thestr), thestr.size());

	//compilemode indicates whether the code is compiled from a file or from a string while executing some code...
	if (compilemode)
		return new TamguConstUString(res, global, parent);

	return new Tamguustring(res, NULL, parent);
}

Tamgu* TamguCode::C_anumber(x_node* xn, Tamgu* parent) {
	string& name = xn->value;
    if (name[0] == '0' && name[1] == 'c') {
        //This is a complex number...
        long p = name.find(":");
        double r = convertfloat(name.substr(2, p));
        name = name.substr(p + 1, name.size() - p - 1);
        double i;
        if (name == "-i")
            i = -1;
        else
            if (name == "i")
                i = 1;
            else
                i = convertfloat(name);
        return new Tamgucomplex(r, i, global, parent);
    }
    
    Tamgu* kv = NULL;
	BLONG v;
	if (name.find(".") == -1 && name.find("e") == -1) {
		v = conversionintegerhexa(STR(name));
        if (compilemode) {
            if (IsLong(v))
                kv = global->ProvideConstlong(v);
            else
                kv = global->ProvideConstint((long)v);
        }
        else {
            if (IsLong(v))
                kv = global->Providelong(v);
            else
                kv = global->Provideint(v);
        }
	}
    else {
        if (compilemode)
            kv = global->ProvideConstfloat(convertfloat(STR(name)));
        else
            kv = global->Providefloat(convertfloat(STR(name)));
    }

    parent->AddInstruction(kv);
    return kv;
}

Tamgu* TamguCode::C_axnumber(x_node* xn, Tamgu* parent) {
	string& name = xn->value;

    if (name[0] == '0' && name[1] == 'c') {
        //This is a complex number...
        long p = name.find(":");
        double r = convertfloat(name.substr(2, p));
        name = name.substr(p + 1, name.size());
        double i;
        if (name == "-i")
            i = -1;
        else
            if (name == "i")
                i = 1;
            else
                i = convertfloat(name);
        return new Tamgucomplex(r, i, global, parent);
    }

	BLONG v = conversionintegerhexa(STR(name));
	Tamgu* kv = NULL;
    if (compilemode) {
        if (IsLong(v))
            kv = global->ProvideConstlong(v);
        else
            kv = global->ProvideConstint((long)v);
    }
    else {
        if (IsLong(v))
            kv = global->Providelong(v);
        else
            kv = global->Provideint(v);
    }
    
    parent->AddInstruction(kv);
    return kv;
}


Tamgu* TamguCode::C_constant(x_node* xn, Tamgu* kf) {
	if (xn->value == "true") {
		kf->AddInstruction(aTRUE);
		return aTRUE;
	}
	if (xn->value == "false") {
		kf->AddInstruction(aFALSE);
		return aFALSE;
	}
	if (xn->value == "null") {
		kf->AddInstruction(aNULL);
		return aNULL;
	}
	return kf;
}


Tamgu* TamguCode::C_intentionvector(x_node* xn, Tamgu* kf) {
	//We will rewrite this instruction into a call to "range"...
	x_node* nstep = NULL;
	if (xn->nodes.back()->token == "step") {
		nstep = xn->nodes.back();
		xn->nodes.pop_back();
	}

	if (xn->nodes.size() == 3) {
		x_node* nop = new x_node("regularcall", "", xn);		
		x_node* nfunc = creationxnode("functioncall", "range", nop);
		creationxnode("word", "range", nfunc);
		x_node* param = creationxnode("parameters", "", nop);

		param->nodes.push_back(xn->nodes[0]);
		param->nodes.push_back(xn->nodes[2]);
		

		if (nstep != NULL)
			//we add the step
			param->nodes.push_back(nstep);

		TamguInstruction ai;
		Traverse(nop, &ai);
		bool getvect = true;
		TamguCall* args = (TamguCall*)ai.instructions[0];
		for (short i = 0; i < args->arguments.size(); i++) {
			if (!args->arguments[i]->isConst()) {
				getvect = false;
				break;
			}
		}
		if (getvect) {
			//If the intention vector is only composed of const values, we can evaluate it now...
			Tamgu* kvect = ai.instructions[0]->Eval(aNULL, aNULL, 0);
			if (kvect == aNOELEMENT)
				kf->AddInstruction(ai.instructions[0]);
			else {
				//We set it to const to prevent any modification of it...
				kvect->Setreference();
				kvect->SetConst();
				kf->AddInstruction(kvect);
				ai.instructions[0]->Remove();
			}
		}
		else
			kf->AddInstruction(ai.instructions[0]);
		param->nodes.clear();
		delete nop;
		return kf;
	}

	Tamgu* kinfvect;
	if (xn->nodes[0]->token == "intentionsep") {
		kinfvect = new TamguInfinitevector(-1, global, kf);
		Traverse(xn->nodes[1], kinfvect);
	}
	else {
		kinfvect = new TamguInfinitevector(1, global, kf);
		Traverse(xn->nodes[0], kinfvect);
	}

	if (nstep != NULL) {
		Traverse(nstep, kinfvect);
		delete nstep;
	}

	return kinfvect;
}


Tamgu* TamguCode::C_intentionwithdouble(x_node* xn, Tamgu* kf) {
	//Different types of expressions: [..x,y]
	TamguInfinitevector* kinfvect;
	if (xn->nodes[0]->token == "intentionsep") {
		kinfvect = new TamguInfinitevector(-1, global, kf);
		kinfvect->compute = true;
		//The initial value
		Traverse(xn->nodes[2], kinfvect);
		//The next value from which the step will be computed...
		Traverse(xn->nodes[1], kinfvect);
		return kinfvect;
	}

	//[x,y..]
	if (xn->nodes.back()->token == "intentionsep") {
		kinfvect = new TamguInfinitevector(1, global, kf);
		kinfvect->compute = true;
		//The initial value
		Traverse(xn->nodes[0], kinfvect);
		//The next value from which the step will be computed...
		Traverse(xn->nodes[1], kinfvect);
		return kinfvect;
	}

	//[x,y..z]

	x_node* nop = new x_node("regularcall", "", xn);
	
	x_node* nfunc = creationxnode("functioncall", "range", nop);
	creationxnode("word", "range", nfunc);

	x_node* param = creationxnode("parameters", "", nop);
	param->nodes.push_back(xn->nodes[0]);
	param->nodes.push_back(xn->nodes[3]);

	TamguCallProcedure* kcf;
	Tamgu* kret = Traverse(nop, kf);
	kret->Addargmode();
	kcf = (TamguCallProcedure*)kret;
	bool getvect = true;
	for (short i = 0; i < kcf->arguments.size(); i++) {
		if (!kcf->arguments[i]->isConst()) {
			getvect = false;
			break;
		}
	}
	

	TamguInstructionAPPLYOPERATIONROOT* kroot = new TamguInstructionAPPLYOPERATIONROOT(global, kret);
	TamguInstruction ki;
	
	short idord = 0;

	Traverse(xn->nodes[0], &ki);
	if (ki.instructions[0]->isString()) {
		idord = global->Getid("ord");
		kcf = new TamguCallProcedure(idord, global);
		kcf->arguments.push_back(ki.instructions[0]);
		kroot->instructions.push_back(kcf);
	}
	else
		kroot->Stacking(ki.instructions[0], false);
	
	ki.instructions.clear();
	Traverse(xn->nodes[1], &ki);
	if (!idord)
		kroot->Stacking(ki.instructions[0], false);
	else {
		kcf = new TamguCallProcedure(idord, global);
		kcf->arguments.push_back(ki.instructions[0]);
		kroot->instructions.push_back(kcf);
	}

	kroot->instructions.push_back(global->actions[a_minus]);
	kroot->Setsize();
	if (getvect && ki.instructions[0]->isConst()) {
		//If the intention vector is only composed of const values, we can evaluate it now...
		Tamgu* kvect = kret->Eval(aNULL, aNULL, 0);
		if (kvect != aNOELEMENT) {
			kvect->Setreference();
			kvect->SetConst(); //we set to const to prevent any modification...
			kf->InstructionRemoveLast();
			kf->AddInstruction(kvect);
			kret->Remove();
			kroot->Remove();
		}

	}

	param->nodes.clear();
	delete nop;
	return kf;

}



Tamgu* TamguCode::C_valvector(x_node* xn, Tamgu* kf) {
	int i;

	if (!compilemode && kf->Type() != a_taskellinstruction) {
		Tamguvector* kvect = new Tamguvector(NULL, kf);
		for (i = 0; i < xn->nodes.size(); i++) {
			if (xn->nodes[i]->nodes.size() == 1)
				Traverse(xn->nodes[i], kvect);
			else {
				TamguInstruction ki;
				Traverse(xn->nodes[i], &ki);
				kvect->AddInstruction(ki.instructions[0]);
			}
		}
		return kvect;
	}


	//First, we check if we are in an assignement...
	short vartype = 0;
	if (kf->Action() == a_assignement) {
		if (kf->InstructionSize()) {
			Tamgu* ins = kf->Instruction(0);
			short ty = ins->Typevariable();
			if (global->newInstance.check(ty) && global->newInstance[ty]->isValueContainer()) {
				vartype = ty;
			}
		}
	}

	TamguConstvector* kvect = new TamguConstvector(global);

	if (xn->token == "valpredicatevector" || xn->token == "valvectortail") {
		for (i = 0; i < xn->nodes.size(); i++) {
			if (xn->nodes[i]->token == "valtail") {
				TamguConstvectormerge* kbv = new TamguConstvectormerge(global, NULL);
				kvect->values.push_back(kbv);
				Traverse(xn->nodes[i], kbv);
			}
			else
				Traverse(xn->nodes[i], kvect);
		}
	}
	else {
		for (i = 0; i < xn->nodes.size(); i++) {
			if (xn->nodes[i]->nodes.size() == 1)
				Traverse(xn->nodes[i], kvect);
			else {
				TamguInstruction ki;
				Traverse(xn->nodes[i], &ki);
				kvect->AddInstruction(ki.instructions[0]);
			}
		}
	}

	bool duplicate = true;
	kvect->evaluate = false;
	Tamgu* a;
	for (i = 0; i < kvect->values.size(); i++) {
		a = kvect->values[i];
		if (!a->baseValue()) {
			kvect->evaluate = true;
			duplicate = false;
			break;
		}
		if (!a->isConst()) {
			duplicate = false;
			continue;
		}
	}

	
	if (vartype && duplicate) {
		Tamgu* v = global->newInstance[vartype]->Newinstance(0);
		v->SetConst();
		v->Setreference();
		for (long i = 0; i < kvect->values.size(); i++)
			v->Push(kvect->values[i]);
		kvect->Remove();
		kf->AddInstruction(v);
		return v;
	}
	
	kf->AddInstruction(kvect);
	return kvect;
}

Tamgu* TamguCode::C_valmap(x_node* xn, Tamgu* kf) {
	TamguConstmap* kmap;
	long i;

	if (xn->token == "maptail" || xn->token == "mapmerging") {
		kmap = (TamguConstmap*)kf;
		kmap->keys.push_back(aPIPE);
		TamguInstruction kbloc;
		Traverse(xn->nodes[0], &kbloc);
		kmap->values.push_back(kbloc.instructions[0]);
		kmap->Setevaluate(true);
		return kf;
	}

	//First, we check if we are in an assignement...
	short vartype = 0;
	if (kf->Action() == a_assignement) {
		if (kf->InstructionSize()) {
			Tamgu* ins = kf->Instruction(0);
			short ty = ins->Typevariable();
			if (global->newInstance.check(ty) && global->newInstance[ty]->isValueContainer()) {
				vartype = ty;
			}
		}
	}

	kmap = new TamguConstmap(global);
	for (i = 0; i < xn->nodes.size(); i++)
		Traverse(xn->nodes[i], kmap);

	Tamgu* val;
	Tamgu* key;
	bool duplicate = true;
	uchar types = 0;
	if (!kmap->isEvaluate()) {
		for (i = 0; i < kmap->values.size(); i++) {
			val = kmap->values[i];
			key = kmap->keys[i];
			if (!key->baseValue() || !val->baseValue()) {
				kmap->Setevaluate(true);
				duplicate = false;
				types = 0;
				break;
			}

			if (!key->isConst() || !val->isConst()) {
				duplicate = false;
				types = 0;
				continue;
			}
			types |= Returnequ(key->Type());
		}
	}
	
	if (types && !vartype) {
		//in that case, if all keys are numbers, well we need to take this into account...
		if ((types&b_allnumbers) == types) {
			if (types & b_floats)
				types = b_float;
			else {
				if (types&b_long)
					types = b_long;
				else
					types = b_int;
			}
			Tamgu* m = global->mapnewInstances[types][b_none]->Newinstance(0);
			m->SetConst();
			m->Setreference();
			for (long i = 0; i < kmap->keys.size(); i++)
				m->Push(kmap->keys[i], kmap->values[i]);
			kmap->Remove();
			kf->AddInstruction(m);
			return m;
		}
	}

	//If we have a map, which can be directly evaluated at this moment (no variable in the map for instance) and we know the type
	//of the variable to store it in, we do it now...	
	if (duplicate && vartype) {
		Tamgu* m = global->newInstance[vartype]->Newinstance(0);
		m->SetConst();
		m->Setreference();
		for (long i = 0; i < kmap->keys.size(); i++)
			m->Push(kmap->keys[i], kmap->values[i]);
		kmap->Remove();
		kf->AddInstruction(m);
		return m;
	}

	kf->AddInstruction(kmap);

	return kmap;
}


Tamgu* TamguCode::C_features(x_node* xn, Tamgu* kf) {
	TamguInstruction kbloc;

	Tamgumapss* kmap = new Tamgumapss(global, kf);
	for (int i = 0; i < xn->nodes.size(); i++) {
		x_node* sub = xn->nodes[i];
		string key;
		string val;
		if (sub->nodes[0]->token == "word")
			key = sub->nodes[0]->value;
		else {
			key = sub->nodes[0]->value;
			if (sub->nodes[0]->token == "afullstring")
				key = key.substr(2, key.size() - 3);
			else
				key = key.substr(1, key.size() - 2);
		}

		if (Tamgusynode::Checkattribute(key) == false) {
			stringstream message;
			message << "Unknown attribute: '" << key << "'";
			throw new TamguRaiseError(message, filename, current_start, current_end);
		}

		if (sub->nodes.size() == 1)
			kmap->values[key] = "+";
		else {
			bool rgx = false;
			if (sub->nodes[1]->value == "=") {

				if (featureassignment == 2) {
					stringstream message;
					message << "Cannot assign a feature to a dependency node";
					throw new TamguRaiseError(message, filename, current_start, current_end);
				}

				key = "=" + key;
			}
			else
			if (sub->nodes[1]->value == "~:")
				key = "~" + key;
			else
			if (sub->nodes[1]->value == "-:") {
				key = ":" + key;
				rgx = true;
			}
			else
			if (sub->nodes[1]->value == "~-:") {
				key = ":~" + key;
				rgx = true;
			}


			if (sub->nodes[2]->value == "~")
				kmap->values[key] = sub->nodes[2]->value;
			else {
				if (sub->nodes[2]->token == "word" || sub->nodes[2]->token == "valplus")
					val = sub->nodes[2]->value;
				else {
					Traverse(sub->nodes[2], &kbloc);
					val = kbloc.instructions[0]->String();
				}
				if (Tamgusynode::Checkfeature(key, val) == false) {
					stringstream message;
					message << "Unknown attribute/value: '" << key << "'/'" << val << "'";
					throw new TamguRaiseError(message, filename, current_start, current_end);
				}

				if (rgx) {
					if (val.empty()) {
						stringstream message;
						message << "Empty string cannot be used as a regular expression: '" << key << "'";
						throw new TamguRaiseError(message, filename, current_start, current_end);
					}

					global->rules[val] = new Au_automaton(val);
				}

				kmap->values[key] = val;
				kbloc.instructions.clear();
			}
		}
	}
	return kmap;
}

Tamgu* TamguCode::C_declarationtaskell(x_node* xn, Tamgu* kf) {
	//The first element is a list of types, the second is the type output...

	string arg;
	short ty;
	TamguFunctionLambda* klambda = (TamguFunctionLambda*)kf;

	long sz = (long)xn->nodes.size() - 1;
	for (long i = 0; i <= sz; i++) {
		if (xn->nodes[i]->token == "word" || xn->nodes[i]->token == "maybe") {
            bool mybe=false;
            if (xn->nodes[i]->token == "maybe") {
                arg = xn->nodes[i]->nodes[0]->value;
                mybe=true;
            }
            else
                arg = xn->nodes[i]->value;
			ty = global->Getid(arg);
			if (ty != a_universal && global->newInstance.check(ty) == false) {
				stringstream message;
				message << "Unknown type: '" << arg << "'";
				throw new TamguRaiseError(message, filename, current_start, current_end);
			}
			if (i == sz) {
				klambda->returntype = ty;
                klambda->maybe = mybe;
				if (ty == a_universal)
					klambda->returntype = a_null;
			}
			else
				klambda->taskelldeclarations.push_back(new Taskelldeclaration(ty, mybe));
			continue;
		}

        //Check Maybe here
		arg = xn->nodes[i]->nodes[0]->nodes[0]->value;
		ty = global->Getid(arg);
		if (ty != a_universal && global->newInstance.check(ty) == false) {
			stringstream message;
			message << "Unknown type: '" << arg << "'";
			throw new TamguRaiseError(message, filename, current_start, current_end);
		}
		if (i == sz) {
			klambda->returntype = ty;
			if (ty == a_universal)
				klambda->returntype = a_null;
		}
		else {
			SubTaskelldeclaration* sub = new SubTaskelldeclaration(ty);
			for (long j = 1; j < xn->nodes[i]->nodes[0]->nodes.size(); j++) {
				arg = xn->nodes[i]->nodes[0]->nodes[j]->value;
				ty = global->Getid(arg);
				if (ty != a_universal && global->newInstance.check(ty) == false) {
					stringstream message;
					message << "Unknown type: '" << arg << "'";
					throw new TamguRaiseError(message, filename, current_start, current_end);
				}
				sub->Push(ty);
			}
			klambda->taskelldeclarations.push_back(sub);
		}
	}

	klambda->hdeclared = true;

	return kf;
}

Tamgu* TamguCode::C_dico(x_node* xn, Tamgu* kf) {
	TamguInstruction kbloc;

	//First the key
	Traverse(xn->nodes[0], &kbloc);
	Tamgu* key = kbloc.instructions[0];
	kbloc.instructions.clear();

	//then the value
	Traverse(xn->nodes[1], &kbloc);
	Tamgu* val = kbloc.instructions[0];

	TamguConstmap* kmap = (TamguConstmap*)kf;
	kmap->keys.push_back(key);
	kmap->values.push_back(val);
	if (xn->nodes.size() == 3)
		Traverse(xn->nodes[2], kf);
	return kf;
}

Tamgu* TamguCode::C_jsondico(x_node* xn, Tamgu* kf) {
    TamguInstruction kbloc;

    //First the key
    Traverse(xn->nodes[0], &kbloc);
    string key = kbloc.instructions[0]->String();

    kbloc.instructions[0]->Remove();
    kbloc.instructions.clear();

    //then the value
    Traverse(xn->nodes[1], &kbloc);
    Tamgu* val = kbloc.instructions[0];

    kf->push(key, val);
    return kf;
}

Tamgu* TamguCode::C_jsonmap(x_node* xn, Tamgu* kf) {
    Tamgumap* kmap = globalTamgu->Providemap();
    if (kf != NULL)
        kf->AddInstruction(kmap);
    
    short idthread = globalTamgu->GetThreadid();
    TamguInstruction kbloc;
    string key;
    long sz = xn->nodes.size();
    for (long i = 0; i < sz; i++) {
        key = xn->nodes[i]->nodes[0]->value;
        if (key != "" && (key[0] == '"' || key[0] == '\''))
            key = key.substr(1,key.size()-2);
        Traverse(xn->nodes[i]->nodes[1], &kbloc);
        kbloc.instructions[0]->Addreference(kf->investigate,idthread);
        kmap->values[key] = kbloc.instructions[0];
        kbloc.instructions.clear();
    }
	return kmap;
}

Tamgu* TamguCode::C_jsonvector(x_node* xn, Tamgu* kf) {
    Tamguvector* kvect = globalTamgu->Providevector();
    if (kf != NULL)
        kf->AddInstruction(kvect);
    long sz = xn->nodes.size();
    for (long i = 0; i < sz; i++)
		Traverse(xn->nodes[i], kvect);
	return kvect;
}

Tamgu* TamguCode::C_affectation(x_node* xn, Tamgu* kf) {
	TamguInstruction* ki = TamguCreateInstruction(NULL, a_assignement);

	for (size_t i = 0; i < xn->nodes.size(); i++)
		Traverse(xn->nodes[i], ki);

	
	if (ki->InstructionSize()) {
		if (ki->Instruction(0)->isConstant()) {
			stringstream message;
			message << "You cannot modify this constant variable";
			throw new TamguRaiseError(message, filename, current_start, current_end);
		}
	}

	short act = ki->Action();
	Tamgu* kfirst;
	Tamgu* func;
	if (act != a_assignement && act != a_forcedaffectation) {
		TamguInstruction* k = TamguCreateInstruction(NULL, act);
        kfirst = ki->Instruction(0);
        if (k->isEQU() && kfirst->isAlias()) {
            k->Remove();
            k = new TamguInstructionAPPLYOPERATIONEQUALIAS((TamguCallAlias*)kfirst, global);
            k->Setaction(act);
        }
		ki->Clone(k);
		ki->Remove();
		ki = k;
		func = kfirst->Function();

		if (act == a_stream)
			ki->Instruction(0)->Setaffectation(true);
        else {
            if (ki->isEQU()) {
                ki->Instruction(0)->Setaffectation(true);
                TamguInstructionAPPLYOPERATIONEQU* kequ = (TamguInstructionAPPLYOPERATIONEQU*)ki;
                kequ->action = global->equto[kequ->action];
                TamguInstructionAPPLYOPERATIONEQU ktmp(NULL);
                ktmp.action = kequ->action;
                for (int i = 1; i < kequ->instructions.size(); i++)
                    ktmp.instructions.push_back(kequ->instructions[i]);
                kequ->recipient = kequ->instructions[0];
                kequ->thetype = Evaluateatomtype(kequ->recipient, true);
                kequ->instructions.clear();
                kequ->Stacking(&ktmp, true);
                kequ->Setsize();
                kequ->instruction = kequ->Returnlocal(global, aONE);
                if (kequ->instruction == kequ) {
                    TamguInstructionAPPLYOPERATIONROOT* kroot = new TamguInstructionAPPLYOPERATIONROOT(global);
                    kequ->instruction = kroot;
                    kroot->thetype = kequ->thetype;
                    kroot->head = kroot->head;
                    kroot->instructions = kequ->instructions;
                    ((TamguInstructionAPPLYOPERATIONROOT*)kequ->instruction)->Setsize();
                }
                else {
                    Tamgu* kloc = kequ->update(kequ->instruction->BType());
                    if (kloc != kequ) {
                        kequ->Remove();
                        ki = (TamguInstruction*)kloc;
                    }
                }
                if (ki == kequ) {
                    short localtype = kequ->recipient->Typevariable();
                    if (global->atomics.check(localtype)) {
                        switch (localtype) {
                            case a_short:
                                ki = new TamguInstructionAPPLYEQUSHORT(kequ, global);
                                kequ->Remove();
                                break;
                            case a_int:
                                ki = new TamguInstructionAPPLYEQUINT(kequ, global);
                                kequ->Remove();
                                break;
                            case a_decimal:
                                ki = new TamguInstructionAPPLYEQUDECIMAL(kequ, global);
                                kequ->Remove();
                                break;
                            case a_float:
                                ki = new TamguInstructionAPPLYEQUFLOAT(kequ, global);
                                kequ->Remove();
                                break;
                            case a_long:
                                ki = new TamguInstructionAPPLYEQULONG(kequ, global);
                                kequ->Remove();
                                break;
                            case a_string:
                                ki = new TamguInstructionAPPLYEQUSTRING(kequ, global);
                                kequ->Remove();
                                break;
                            case a_ustring:
                                ki = new TamguInstructionAPPLYEQUUSTRING(kequ, global);
                                kequ->Remove();
                                break;
                        }
                    }
                }
            }
        }
        
        kf->AddInstruction(ki);
	}
	else {
		if (act == a_assignement || act == a_stream) {
			kfirst = ki->Instruction(1);
			//If the first operation has not been converted into a ROOT, we do it here...
			//For instance, i=-i, where -i is converted into -1*i, does not always go through the conversion process...
			if (kfirst->isOperation()) {
				//we need then to recast it into an actual operation root
				Tamgu* kroot = kfirst->Compile(ki->Instruction(0));
				kfirst->Remove();
				ki->Putinstruction(1, kroot);
			}
		}

		kfirst = ki->Instruction(0);
		func = kfirst->Function();
        if (kfirst->isPredicateVariable()) {
            kfirst = new TamguPredicateVariable(global, kfirst->Name());
            ki->Putinstruction(0, kfirst);
            func = new TamguPredicateVariableASSIGNMENT(global, kfirst, kf);
            ((TamguInstruction*)func)->instructions = ki->instructions;
            ki->Remove();
            ki = (TamguInstruction*)func;
        }
        else {
            if (func == NULL && act == a_assignement) {
                short type = kfirst->Typevariable();
                if (type == a_let || type == a_self)
                    func = new TamguInstructionSelfASSIGNMENT(global, kfirst, kf);
                else {
                    if (global->atomics.check(ki->instructions[0]->Typevariable())) {
                        if (kfirst->isGlobalVariable())
                            func = new TamguInstructionGlobalVariableASSIGNMENT(global,kfirst->Name(), kf);
                        else
                            if (kfirst->isFunctionVariable())
                                func = new TamguInstructionFunctionVariableASSIGNMENT(global, kfirst, kf);
                            else
                                func = new TamguInstructionVariableASSIGNMENT(global, kf);
                    }
                    else
                        func = new TamguInstructionAtomicASSIGNMENT(global, kfirst, kf);
                }
                ((TamguInstruction*)func)->instructions = ki->instructions;
                ki->Remove();
                ki = (TamguInstruction*)func;
            }
            else {
                if (kfirst->isAlias()) {
                    func = new TamguAliasASSIGNMENT((TamguCallAlias*)kfirst, global, kf);
                    ((TamguInstruction*)func)->instructions = ki->instructions;
                    ki->Remove();
                    ki = (TamguInstruction*)func;
                }
                else
                    kf->AddInstruction(ki);
            }
        }
		ki->Instruction(0)->Setaffectation(true);
		if (act == a_forcedaffectation)
			ki->Instruction(0)->Setforced(true);
	}

	if (!kfirst->isAssignable()) {
		stringstream message;
		message << "Cannot assign a value in this configuration";
		if (func != NULL)
			message << ": '" << global->Getsymbol(func->Name()) << "'";
		else {
			if (kfirst->Name() > 1)
				message << ", : '" << global->Getsymbol(kfirst->Name()) << "'";
		}
		throw new TamguRaiseError(message, filename, current_start, current_end);
	}
	kfirst->Setstopindex();
	return ki;
}

Tamgu* TamguCode::C_operator(x_node* xn, Tamgu* parent) {
	short op = global->string_operators[xn->value];
	parent->Setaction(op);
	return parent;
}


Tamgu* TamguCode::C_plusplus(x_node* xn, Tamgu* kf) {
	Tamgu* ki;
	long sz;
	short op = global->string_operators[xn->value];
	switch (op) {
	case a_plusplus:
		return new TamguPLUSPLUS(global, kf);
	case a_minusminus:
		return new TamguMINUSMINUS(global, kf);
	case a_square:
		if (kf->isInstruction()) {
			sz = kf->InstructionSize() - 1;
			ki = new TamguInstructionAPPLYOPERATION(global, NULL);
			ki->Setaction(a_power);
			ki->AddInstruction(kf->Instruction(sz));
			ki->AddInstruction(globalTamgu->ProvideConstint(2));
			kf->Putinstruction(sz, ki);
			return kf;
		}
		return new TamguSQUARE(global, kf);
	case a_cube:
		if (kf->isInstruction()) {
			sz = kf->InstructionSize() - 1;
			ki = new TamguInstructionAPPLYOPERATION(global, NULL);
			ki->Setaction(a_power);
			ki->AddInstruction(kf->Instruction(sz));
			ki->AddInstruction(globalTamgu->ProvideConstint(3));
			kf->Putinstruction(sz, ki);
			return kf;
		}
		return new TamguCUBE(global, kf);
	}
	return kf;
}

Tamgu* TamguCode::C_increment(x_node* xn, Tamgu* kf) {
	TamguInstruction ki;
	Traverse(xn->nodes[0], &ki);
	Traverse(xn->nodes[1], ki.instructions[0]);
	kf->AddInstruction(ki.instructions[0]);
	return kf;
}


Tamgu* TamguCode::C_multiply(x_node* xn, Tamgu* kf) {
	short op = a_multiply;
	//The second parameter is the rest of the operation
	//kf is the TOP instruction
	TamguInstruction* ki;
	if (kf->Action() == a_bloc) {
		//We are in a new bloc, which is our current element
		//In this case, we create a new level
		ki = TamguCreateInstruction(NULL, op);
		//It becomes the new element
		ki->AddInstruction(kf->Instruction(0));
		kf->Putinstruction(0, ki);
		ki->Addparent(kf);
		Traverse(xn->nodes[0], ki);
		if (xn->nodes.size() == 2)
			Traverse(xn->nodes[1], ki);

		ki->Subcontext(true);
		return ki;
	}

	if (kf->Action() == op) {
		Traverse(xn->nodes[0], kf);
		if (xn->nodes.size() == 2)
			Traverse(xn->nodes[1], kf);
		return kf;
	}


	//we create a new level
	ki = TamguCreateInstruction(NULL, op);
	ki->AddInstruction(kf->Lastinstruction());
	kf->Putinstruction(kf->InstructionSize() - 1, ki);
	ki->Addparent(kf);
	Traverse(xn->nodes[0], ki);
    if (xn->nodes.size() >= 2) {
        Traverse(xn->nodes[1], ki);
        if (xn->nodes.size() == 3)
            Traverse(xn->nodes[2], ki);
    }

	//we can merge in this case the content of ki into kf
	if (ki->Action() == kf->Action()) {
		size_t sz = kf->InstructionSize() - 1;
		kf->InstructionRemove(sz);
		for (size_t i = 0; i < ki->InstructionSize(); i++)
			kf->Putinstruction(i + sz, ki->Instruction(i));
		ki->Remove();
	}

	short act = kf->Action();
	if ((act == a_none || act == a_stream || act == a_assignement) && kf->InstructionSize() >= 1) {
		Tamgu* kroot;
		if (act != a_none)
			kroot = ki->Compile(kf->Instruction(0));
		else
			kroot = ki->Compile(NULL);

		kf->InstructionRemoveLast();
		kf->AddInstruction(kroot);
		ki->Remove();
	}

	return kf;
}

Tamgu* TamguCode::C_operation(x_node* xn, Tamgu* kf) {
	//The first parameter is the operator	
    short op = global->string_operators[xn->nodes[0]->value];
	//The second parameter is the rest of the operation
	//kf is the TOP instruction
	TamguInstruction* ki;
	if (kf->Action() == a_bloc) {
		//We are in a new bloc, which is our current element
		//In this case, we create a new level
		ki = TamguCreateInstruction(NULL, op);
		//It becomes the new element
		ki->AddInstruction(kf->Instruction(0));
		Traverse(xn->nodes[1], ki);
		ki->Subcontext(true);
		kf->Putinstruction(0, ki);
		ki->Addparent(kf);
		return ki;
	}

	if (kf->Action() == op) {
		Traverse(xn->nodes[1], kf);
		return kf;
	}

	//In this case, the operator is not the same
	//we still have two cases: if it is
	//if it is a PLUS or a MINUS, we reset the top node with the new information
	//It has to be in the middle of an operation
	Tamgu* kloop;

	if (global->atanOperatorMath.check(kf->Action())) {
		if (op == a_plus || op == a_minus || op == a_merge || op == a_combine ||
            (op == a_multiply && kf->Action() == a_divide)) {
			kloop = kf;
			short kact = kloop->Action();
			while (kloop != NULL && global->atanOperatorMath.check(kact) && kact != a_plus && kact != a_minus && kact != a_merge  && kact != a_combine) {
				kloop = kloop->Parent();
				if (kloop != NULL)
					kact = kloop->Action();
			}

			if (kloop == NULL || global->atanOperatorMath.check(kloop->Action()) == false)
				kloop = kf;
			else {
				if (kloop->Action() == op) {
					Traverse(xn->nodes[1], kloop);
					return kloop;
				}
			}
            
            ki = TamguCreateInstruction(NULL, kloop->Action());
            for (size_t i = 0; i < kloop->InstructionSize(); i++)
                ki->instructions.push_back(kloop->Instruction(i));
            kloop->Setaction(op);
            kloop->InstructionClear();
            kloop->Putinstruction(0, ki);
            ki->Addparent(kf);
            
            Traverse(xn->nodes[1], kloop);
			return kloop;
		}
	}

	//we create a new level
    ki = TamguCreateInstruction(NULL, op);
    
	ki->AddInstruction(kf->Lastinstruction());
	kf->Putinstruction(kf->InstructionSize() - 1, ki);
	ki->Addparent(kf);
	Traverse(xn->nodes[1], ki);

	//we can merge in this case the content of ki into kf
	if (ki->Action() == kf->Action()) {
		size_t sz = kf->InstructionSize() - 1;
		kf->InstructionRemove(sz);
		for (size_t i = 0; i < ki->InstructionSize(); i++)
			kf->Putinstruction(i + sz, ki->Instruction(i));
		ki->Remove();
        ki = (TamguInstruction*)kf;
	}

	short act = kf->Action();
	if ((act == a_none || act == a_stream || act == a_assignement) && kf->InstructionSize() >= 1) {
		Tamgu* kroot;
		if (act != a_none)
			kroot = ki->Compile(kf->Instruction(0));
		else
			kroot = ki->Compile(NULL);

		if (kroot != ki) {
			kf->InstructionRemoveLast();
			kf->AddInstruction(kroot);
			ki->Remove();
		}
	}
	return kf;
}

Tamgu* TamguCode::C_taskcomparison(x_node* xn, Tamgu* kf) {
    if (xn->nodes[0]->token=="returntype") {
        string typeret = xn->nodes[0]->nodes[0]->value;
        if (global->symbolIds.find(typeret) == global->symbolIds.end()) {
            stringstream message;
            message << "Unknown type: '" << typeret << "'";
            throw new TamguRaiseError(message, filename, current_start, current_end);
        }
        
        kf->Setreturntype(global->Getid(typeret));
        Traverse(xn->nodes[1], kf);
    }
    else
        Traverse(xn->nodes[0], kf);

    return kf;
}


Tamgu* TamguCode::C_comparison(x_node* xn, Tamgu* kf) {
	//The first parameter is the operator
	Tamgu* ki = kf;
	short op = global->string_operators[xn->nodes[0]->value];
	if (kf->Action() == a_blocboolean || kf->Action() == a_bloc)
		ki->Setaction(op);
	else
	if (kf->Action() >= a_plus && kf->Action() <= a_and) {
		Tamgu* kparent = kf->Parent();
		while (kparent != NULL) {
			if (kparent->Action() == a_blocboolean || kparent->Action() == a_bloc) {
				kparent->Setaction(op);
				ki = kparent;
				break;
			}
			kparent = kparent->Parent();
		}
		if (kparent == NULL) {
			kparent = kf->Parent();
			ki = TamguCreateInstruction(kparent, op);
		}
	}
	else
	if (kf->Action() == a_predicateelement) {
		ki = TamguCreateInstruction(NULL, op);
		//A modifier pour PROLOG
		((TamguInstruction*)ki)->instructions.push_back(((TamguPredicateRuleElement*)kf)->instructions[0]);
		((TamguPredicateRuleElement*)kf)->instructions[0] = ki;
	}
	else
		ki = TamguCreateInstruction(kf, op);

	for (size_t i = 0; i < xn->nodes.size(); i++)
		Traverse(xn->nodes[i], ki);

	if (ki->Type() == a_sequence && ki->InstructionSize() == 2 && (ki->Instruction(1)->baseValue() || ki->Instruction(1)->isConst())) {
		//Then we invert the comparison to force with the const to compare with itself
		//We do not want 0==empty for instance to be true...
		TamguInstruction* kinst = (TamguInstruction*)ki;
		kf = kinst->instructions[1];
		if (kf->Name() == a_empty) {
			kinst->instructions.pop_back();
			kinst->instructions.insert(0, kf);
		}
	}

    if (!ki->Checkarity()) {
        stringstream message;
        message << "Wrong number of element in an instruction";
        throw new TamguRaiseError(message, filename, current_start, current_end);
    }
    
    return ki;
}

Tamgu* TamguCode::C_negated(x_node* xn, Tamgu* kf) {

	Tamgu* ki = TamguCreateInstruction(NULL, a_multiply);
	ki->AddInstruction(aMINUSONE);
	Traverse(xn->nodes[0], ki);
	Tamgu* kroot = ki->Compile(NULL);
	ki->Remove();
	kf->AddInstruction(kroot);
	return kroot;
}


Tamgu* TamguCode::C_uniquecall(x_node* xn, Tamgu* kf) {
    string& name = xn->value;
    //Looking if it is known as function
    Tamgu* kcf = NULL;
    
    if (name == "break")
        kcf = new TamguBreak(global, kf);
    else
        if (name == "continue")
            kcf = new TamguContinue(global, kf);
        else
            if (name == "return" || name == "_return") {
                kcf = new TamguCallReturn(global, kf);
                if (xn->nodes.size() == 1) {
                    TamguInstruction kbloc;
                    Traverse(xn->nodes[0], &kbloc);
                    kf = kbloc.instructions[0];
                    //Is it a tail recursion?
                    short function_name = kf->Name();
                    short function_on_stack = global->Topstack()->Name();
                    if (function_name && function_name == function_on_stack)
                        ((TamguCallReturn*)kcf)->tail = true;
                    kcf->AddInstruction(kf);
                }
            }
    return kcf;
}

Tamgu* TamguCode::C_operationin(x_node* xn, Tamgu* kf) {
	//The first parameter is the operator
	TamguInstruction* kinst;
	short kcurrentop = global->string_operators[xn->nodes[0]->token];
	if (kf->Action() == a_blocboolean) {
		kinst = (TamguInstruction*)kf;
		kinst->action = kcurrentop;
	}
	else {
		Tamgu* last;
		//In this case, we replace the previous last element in kf with this one
		short kact = kf->Action();
		if (kact == a_assignement || (kact >= a_plusequ && kact <= a_andequ)) {
			last = kf->InstructionRemoveLast();
			kinst = TamguCreateInstruction(kf, kcurrentop);
			kinst->instructions.push_back(last);
		}
		else {
			if (kf->Parent() != NULL) {
				//In this case, we need to keep the full instruction			
				last = kf->Parent()->InstructionRemoveLast();
				kinst = TamguCreateInstruction(kf->Parent(), kcurrentop);
				kinst->instructions.push_back(kf);
				kf->Addparent(kinst);
			}
			else {
				last = kf->InstructionRemoveLast();
				kinst = TamguCreateInstruction(kf, kcurrentop);
				if (last != NULL)
					kinst->instructions.push_back(last);
			}
		}
	}

	Traverse(xn->nodes[1], kinst);

	return kinst;
}


Tamgu* TamguCode::C_alias(x_node* xn, Tamgu* kf) {
    string name = xn->nodes[0]->value;

    short idname = global->Getid(name);
    
    if (!kf->isFrame()) {
        if (global->procedures.check(idname)) {
            stringstream message;
            message << "Error: Predefined procedure, consider choosing another name: '" << name << "'";
            throw new TamguRaiseError(message, filename, current_start, current_end);
        }
    }

    TamguAlias* alias = new TamguAlias(idname, global);
    global->Pushstack(alias);
    kf->Declare(idname, alias);
    
    //First we extract our variables...
    for (long i =  1; i < (long)xn->nodes.size() - 1; i++) {
        idname = global->Getid(xn->nodes[i]->value);
        alias->parameters.push_back(idname);
        alias->Declare(idname, aNULL);
    }
    
    xn = xn->nodes.back();
    
    if (xn->token == "wnexpressions") {
        alias->assignable = true;        
        if (xn->nodes.size() == 1)
            Traverse(xn->nodes[0], alias);
        else {
            Tamgu* ins = new TamguInstruction(a_instructions, global);
            for (size_t i = 0; i < xn->nodes.size(); i++)
                Traverse(xn->nodes[i], ins);
            alias->AddInstruction(ins->Lastinstruction());
            ins->Remove();
        }
    }
    else
        Traverse(xn, alias);

    global->Popstack();
    
    return kf;
}

Tamgu* TamguCode::C_createfunction(x_node* xn, Tamgu* kf) {
	short idname;
	string name = xn->nodes[1]->value;
    
	if (kf->isFunction()) {
		stringstream message;
		message << "Error: You cannot declare a function within a function: '" << name << "'";
		throw new TamguRaiseError(message, filename, current_start, current_end);
	}

	idname = global->Getid(name);
	if (!kf->isFrame()) {
		if (global->procedures.check(idname)) {
			stringstream message;
			message << "Error: Predefined procedure, consider choosing another name: '" << name << "'";
			throw new TamguRaiseError(message, filename, current_start, current_end);
		}
	}

	Tamgu* kprevious = Declaration(idname);
	if (!kf->isMainFrame())
		global->functions[idname] = true;

	size_t last = (long)xn->nodes.size() - 1;


	TamguFunction* kfunc = NULL;
	bool autorun = false;
	bool privatefunction = false;
	bool strictfunction = false;
	bool joinfunction = false;

	TamguFunction* predeclared = NULL;

	if (kprevious != NULL) {
		if (kprevious->isFunction() == false) {
			stringstream message;
			message << "Variable: '" << name << "' already declared";
			throw new TamguRaiseError(message, filename, current_start, current_end);
		}

		//When value==".", it is an actual pre-declaration in a frame otherwise it is a type declaration scan... See TamguParsePredeclareFunctions
		if (xn->nodes[last]->token == "declarationending" && xn->nodes[last]->value == ";") {
			kfunc = (TamguFunction*)kprevious;
			while (kfunc != NULL) {
				if (kfunc->choice == 3) {
					predeclared = kfunc;
					break;
				}
				kfunc = kfunc->next;
			}
			kfunc = NULL;
		}

		if (predeclared == NULL) {
			//To cope with predeclarations, we need to use these predeclarations in the order in which they were found
			//When a predeclaration has been used, it is marked as declared (choice==1), we then jump to the next one
			while (kprevious != NULL && kprevious->Alreadydeclared())
				kprevious = ((TamguFunction*)kprevious)->next;
		}
	}


	string typefunction;
	int protection = 0;
	vector<x_node*>& xsub = xn->nodes[0]->nodes;
	int si = 0;
	if (xsub[si]->value == "joined") {
		joinfunction = true;
		si++;
	}

	if (xsub[si]->value == "protected") {
		protection = 1;
		si++;
	}

	if (xsub[si]->value == "exclusive") {
		protection = 2;
		si++;
	}

	if (xsub[si]->value == "private") {
		privatefunction = true;
		si++;
	}
	if (xsub[si]->value == "strict") {
		strictfunction = true;
		si++;
	}
	if (xsub[si]->token == "functionlabel")
		typefunction = xsub[si]->value;

	//Two cases:
	//If this is the first implementation of that function OR NO predeclaration had been issued OR it is not a function, then we create a new function
	if (kprevious == NULL || xn->nodes[last]->token == "declarationending") {

        if (typefunction == "thread") {
			kfunc = new TamguThread(idname, global, joinfunction, protection);
        }
		else {
			if (protection != 0)
				kfunc = new TamguProtectedFunction(idname, global, protection);
            else {
                if (kprevious != NULL)
                    kfunc = new TamguFunction(idname, global);
                else
                    kfunc = new TamguFunctionMap(idname, global);
            }
			if (typefunction == "autorun")
				autorun = true;
		}
	}

	//If we already have an implementation for that function, either it is a predeclaration, then we simply use it
	//Or this predeclaration is NOT a function
	if (kprevious != NULL && xn->nodes[last]->token != "declarationending") {
		if (kprevious->isFunction()) {
			if (kprevious->Predeclared()) {
				kfunc = (TamguFunction*)kprevious; //we use the predeclaration as our new function
				kprevious = NULL;
			}//else, we will add this new function to a previous declaration...
			else
			if (kprevious->isUsed() == true)
				kprevious = NULL;
			else //if it has been implemented in the mother frame already, then it should not be attached to that set of functions
			if (kf != kprevious->Frame())
				kprevious = NULL;
		}
		else
			kprevious = NULL;
	}

	if (kfunc == NULL) {
		stringstream message;
		message << "Error: This function has already been used in a call: '" << name << "'";
		throw new TamguRaiseError(message, filename, current_start, current_end);
	}

	kfunc->privatefunction = privatefunction;
	kfunc->strict = strictfunction;
	global->Pushstack(kfunc);

	if (xn->nodes[last]->token == "declarationending") {
		kfunc->choice = 0;
		//we process the arguments, if they are available
		if (last != 2)
			Traverse(xn->nodes[2], kfunc);
		kfunc->choice = 2;
	}
	else {
		string typeret;
		if (xn->nodes[2]->token == "returntype") {
			typeret = xn->nodes[2]->nodes[0]->value;
            if (global->symbolIds.find(typeret) == global->symbolIds.end()) {
                if (name_space != "") {
                    typeret = name_space + typeret;
                    if (global->symbolIds.find(typeret) == global->symbolIds.end()) {
                        stringstream message;
                        message << "Unknown type: '" << xn->nodes[2]->nodes[0]->value << "'";
                        throw new TamguRaiseError(message, filename, current_start, current_end);
                    }
                }
                else {
                    stringstream message;
                    message << "Unknown type: '" << typeret << "'";
                    throw new TamguRaiseError(message, filename, current_start, current_end);
                }
			}
			kfunc->returntype = global->Getid(typeret);

			x_node* sub = xn->nodes[2];
			xn->nodes.erase(xn->nodes.begin() + 2);
			delete sub;
		}
		else {
			if (xn->nodes.size() > 3 && xn->nodes[3]->token == "returntype") {
				typeret = xn->nodes[3]->nodes[0]->value;
				if (global->symbolIds.find(typeret) == global->symbolIds.end()) {
                    if (name_space != "") {
                        typeret = name_space + typeret;
                        if (global->symbolIds.find(typeret) == global->symbolIds.end()) {
                            stringstream message;
                            message << "Unknown type: '" << xn->nodes[3]->nodes[0]->value << "'";
                            throw new TamguRaiseError(message, filename, current_start, current_end);
                        }
                    }
                    else {
                        stringstream message;
                        message << "Unknown type: '" << typeret << "'";
                        throw new TamguRaiseError(message, filename, current_start, current_end);
                    }
				}
				kfunc->returntype = global->Getid(typeret);

				x_node* sub = xn->nodes[3];
				xn->nodes.erase(xn->nodes.begin() + 3);
				delete sub;
			}
		}

		if (xn->nodes[2]->token == "instruction") {
			kfunc->choice = 1;
			Traverse(xn->nodes[2], kfunc);
		}
		else {
			//If we have a predeclaration, then the arguments have already been parsed...
			if (kfunc->Predeclared() == false) {
				kfunc->choice = 0;
				Traverse(xn->nodes[2], kfunc);
			}
			kfunc->choice = 1;
			Traverse(xn->nodes[3], kfunc);
		}

		if (kf->isFrame()) {
			//We declare our function within a frame...
			unsigned int a = 1 << kfunc->parameters.size();
			if (global->framemethods.check(idname))
				global->framemethods[idname] |= a;
			else
				global->framemethods[idname] = a;
		}
	}

	if (kprevious != NULL) {
		bool found = false;
		//We need to consume it, an actual pre-declared function in a frame...
		if (predeclared != NULL) {
			if (predeclared->parameters.size() == kfunc->parameters.size()) {
				found = true;
				for (size_t arg = 0; arg < predeclared->parameters.size(); arg++) {
					if (predeclared->parameters[arg]->Type() != kfunc->parameters[arg]->Type()) {
						found = false;
						break;
					}
				}

				if (found) {
					if (kf->Declaration(idname) == NULL)
						kf->Declare(idname, predeclared);
					predeclared->choice = 2;
					predeclared->parameters = kfunc->parameters;
					kfunc = predeclared;
				}
				else {
					stringstream message;
					message << "Error: Cannot find a matching function to a pre-declared function (check the declaration order): '" << name << "'";
					throw new TamguRaiseError(message, filename, current_start, current_end);
				}
			}
		}

		if (found == false)
			kprevious->Addfunction(kfunc);
	}
	else {
		if (kf->Declaration(idname) == NULL)
			kf->Declare(idname, kfunc);
	}

	global->Popstack();

	//Autorun section
	if (kfunc->autorun)
		CreateCallFunction(kfunc, kf);

	if (autorun && loader == NULL) {
		if (kfunc->parameters.size() != 0) {
			stringstream message;
			message << "Error: An AUTORUN cannot have parameters: '" << name << "'";
			throw new TamguRaiseError(message, filename, current_start, current_end);
		}

		if (!kf->isMainFrame()) {
			stringstream message;
			message << "Error: An AUTORUN must be declared as a global function: '" << name << "'";
			throw new TamguRaiseError(message, filename, current_start, current_end);
		}

		kfunc->autorun = true;
	}

    kfunc->Indexing();
	return kfunc;
}

//We preload our libraries...
bool TamguCode::CheckUse(x_node* xn, Tamgu* kf) {
	while (xn->nodes.size()) {
		if (xn->token == "regularcall") {
			if (xn->nodes[0]->token == "functioncall"	&& xn->nodes[0]->value == "use") {
				Traverse(xn, kf);
				xn->token[0] = '_';
				return true;
			}
		}
		xn = xn->nodes[0];
	}

	return false;
}



Tamgu* TamguCode::C_blocs(x_node* xn, Tamgu* kf) {
	hmap<size_t, bool> skip;
	size_t i;
	Tamgu* s;

	if (kf->isFrame() || kf->isMainFrame()) {
		x_node* xend;
		x_node declaration_ending("declarationending", ";", xn);
		size_t last;
		//First we check all the functions present in the blocs and we predeclare them
		for (i = 0; i < xn->nodes.size(); i++) {
			if (CheckUse(xn->nodes[i], kf))
				continue;

			if (xn->nodes[i]->token == "bloc") {
				x_node* nsub = xn->nodes[i]->nodes[0];
				if (nsub->token == "predicatedefinition" || nsub->token == "predicatefact") {
                    if (nsub->token == "predicatedefinition")
                        global->predicate_definitions.insert(global->Getid(nsub->nodes[0]->nodes[0]->value));
					if (nsub->nodes[0]->token == "predicate" || nsub->nodes[0]->token == "dependencyfact") {
						short id = global->Getid(nsub->nodes[0]->nodes[0]->value);
						//A modifier pour PROLOG
						if (!global->predicates.check(id))
							global->predicates[id] = new TamguPredicateFunction(global, NULL, id);
					}
				}
                else {
                    if (nsub->nodes.size() && nsub->nodes[0]->token == "hdata") {//Taskell data declaration
                        try {
                            Traverse(nsub->nodes[0], kf);
                        }
                        catch (TamguRaiseError* m) {
                            throw m;
                        }
                        
                        //We then mark the structure as having been consummed already
                        nsub->nodes[0]->value = "$";
                    }
                    else {
                        if (nsub->nodes.size() && nsub->token == "sousbloc") {
                            //We want lisp "defun" function to be pre-declared as a main function
                            nsub = nsub->nodes[0];
                            if (nsub->token == "tamgulisp") {
                                x_node* root = nsub;
                                nsub = nsub->nodes[0];
                                //We check if we have a defun
                                if (nsub->token == "tlist") {
                                    if (nsub->nodes[0]->value == "defun") {
                                        Tamgulispcode lsp(NULL, NULL);
                                        //Then we precompiled our structure...
                                        Traverse(nsub, &lsp);
                                        Tamgu* l = aNULL;
                                        if (lsp.Size() == 1)
                                            l = lsp.values[0]->Eval(kf, aNULL, 0);
                                        
                                        if (!l->isFunction()) {
                                            stringstream message;
                                            message << "Wrong definition of a lisp 'defun' function";
                                            throw new TamguRaiseError(message, filename, current_start, current_end);
                                        }
                                        //and we remove it from future analysis
                                        delete nsub;
                                        root->nodes.clear();
                                    }
                                }
                            }
                        }
                    }
                }
                continue;
			}

			if (xn->nodes[i]->token == "function" || xn->nodes[i]->token == "frame") {
				last = (long)xn->nodes[i]->nodes.size() - 1;
				xend = xn->nodes[i]->nodes[last];
				if (xend->token == "declarationending") {
					//in this case, we do not need to take these predeclarations into account anymore...
					skip[i] = true;
					if (kf->isMainFrame() && xn->nodes[i]->token == "function")
						continue;

					//we modify the value as a hint of a predeclared function for the actual building of that function
					xn->nodes[i]->nodes[last]->value = ".";
				}
				else
					xn->nodes[i]->nodes[last] = &declaration_ending;

				try {
					s = Traverse(xn->nodes[i], kf);
				}
				catch (TamguRaiseError* m) {
					xn->nodes[i]->nodes[last] = xend;
					throw m;
				}
				xn->nodes[i]->nodes[last] = xend;
				//In this case, this an actual predeclaration from within a frame... We use a specific encoding
				//to isolate it later on... This can only happen in a frame...
				if (skip.find(i) != skip.end()) {
					((TamguFunction*)s)->choice = 3;
					xn->nodes[i]->nodes[last]->value = ";";
				}
			}
		}
	}

	Tamgu* ke = NULL;
	for (i = 0; i < xn->nodes.size(); i++) {
		if (skip.find(i) != skip.end())
			continue;
		s = Traverse(xn->nodes[i], kf);
		if (s != NULL)
			ke = s;
	}
	return ke;

}

Tamgu* TamguCode::C_extension(x_node* xn, Tamgu* kf) {
	string nametype = xn->nodes[0]->value;

	short idtypename = global->Getid(nametype);

	if (!global->newInstance.check(idtypename)) {
		stringstream message;
		message << "Error: cannot extend this type:" << nametype;
		throw new TamguRaiseError(message, filename, current_start, current_end);
	}

	string name;
	name = "_" + nametype;

	short idname = global->Getid(name);

	//A temporary place to store our stuff
	TamguFrame* extension;

	if (global->extensions.check(idtypename))
		extension = global->extensions[idtypename];
	else {
		global->extensions[idtypename] = extension = new TamguFrame(idname, false, global); //the name of the frame is also the name of the inner variable...
        extension->thetype = idtypename;
		extension->idtype = a_extension;
        extension->theextensionvar = idname;
	}

	//A variable, which will be our contact here to our object...
    TamguVariableDeclaration* var;
    if (global->atomics.check(idtypename))
        var = new TamguAtomicVariableDeclaration(global, idname, idtypename);
    else
        var = new TamguVariableDeclaration(global, idname, idtypename);
    
	extension->Declare(idname, var);
	global->Pushstack(extension);

	Traverse(xn->nodes[1], extension);
	global->Popstack();
	//Now we extract every single function from this extension
	basebin_hash<Tamgu*>::iterator it;
	Tamgu* f;
	for (it = extension->declarations.begin(); it != extension->declarations.end(); it++) {
		f = it->second;
		if (f->isFunction()) {
			unsigned long arity = 1 << f->Size();
			global->RecordMethods(idtypename, f->Name(), arity);
		}
	}

	return kf;
}

Tamgu* TamguCode::C_frame(x_node* xn, Tamgu* kf) {
	//We create a frame
	//The name is the next parameter

	bool privated = false;
	int pos = 0;
	if (xn->nodes[0]->token == "private") {
		privated = true;
		pos = 1;
	}
	string name;
    if (name_space.empty())
        name = xn->nodes[pos]->value;
    else
        name = name_space + xn->nodes[pos]->value;

	short idname = global->Getid(name);
	TamguFrame* kframe = NULL;
	Tamgu* ke = NULL;

	if (global->frames.find(idname) != global->frames.end()) {
		ke = global->frames[idname];
		if (privated) {
			stringstream message;
			message << "Error: attempt to use private frame:" << name;
			throw new TamguRaiseError(message, filename, current_start, current_end);
		}
	}
	else
		ke = kf->Declaration(idname);

	if (ke == NULL) {
		kframe = new TamguFrame(idname, privated, global, kf);
		Tamguframeseeder::RecordFrame(idname, kframe, global);
		//We consider each frame as a potential procedure that will create a frame of the same type.
        string info = "Create a frame with: '";
        info += name;
        info += "'";
		global->RecordOneProcedure(name, info, ProcCreateFrame, P_FULL);
		global->returntypes[idname] = idname;
		//We record the compatibilities, which might come as handy to check function argument
		global->SetCompatibilities(idname);
		if (kf->isFrame()) {
			global->compatibilities[idname][kf->Name()] = true;
			global->strictcompatibilities[idname][kf->Name()] = true;
		}
	}
	else {
		if (ke->isFrame())
			kframe = (TamguFrame*)ke;
	}

	if (kframe == NULL) {
		stringstream message;
		message << "Error: This frame cannot be created:" << name;
		throw new TamguRaiseError(message, filename, current_start, current_end);
	}


	if (xn->nodes[pos + 1]->token == "declarationending") {
		kf->Declare(kframe->name, kframe);
		return kframe;
	}

	global->frames[idname] = kframe;

	global->Pushstack(kframe);

	bool inherit = false;
	//If it is a sub-frame definition
	if (kf->isFrame()) {
		//We copy all our declarations in it
		//These declarations, will be replaced by local ones if necessary
		kf->Sharedeclaration(kframe, false);
		inherit = true;
	}

	//We then record this new Frame in our instructions list
	//We also store it at the TOP level, so that others can have access to it...
	mainframe.Declare(kframe->name, kframe);
	if (xn->nodes[pos + 1]->token == "depend") {
		string funcname = xn->nodes[pos + 1]->nodes[0]->value;
		short idf = global->Getid(funcname);
		Tamgu* kfunc = NULL;
		kfunc = Declaration(idf, kf);
		//We have a WITH description
		if (kfunc == NULL) {
			stringstream message;
			message << "Unknown function: '" << funcname << "'";
			throw new TamguRaiseError(message, filename, current_start, current_end);
		}
		if (kfunc->isFunction()) {
			if (kfunc->isVariable()) {
				x_node nvar("variable", "", xn);
				creationxnode("word", global->Getsymbol(kfunc->Name()), &nvar);
				kfunc = Traverse(&nvar, kf);
			}

			kframe->Addfunction(kfunc);
		}
		else {
			stringstream message;
			message << "Unknown function: '" << global->idSymbols[kfunc->Name()] << "'";
			throw new TamguRaiseError(message, filename, current_start, current_end);
		}

		Traverse(xn->nodes[pos + 2], kframe);
	}
	else
		Traverse(xn->nodes[pos + 1], kframe);

	global->Popstack();

	if (inherit)
		kf->Sharedeclaration(kframe, true);

	//The initial function returns the frame itself in the case of a frame...
	Tamgu* callinitial = kframe->Declaration(a_initial);
	while (callinitial != NULL) {
		((TamguFunction*)callinitial)->returntype = idname;
		callinitial = callinitial->Nextfunction();
	}

	return kframe;
}

Tamgu* TamguCode::C_parenthetic(x_node* xn, Tamgu* kf) {
    if (kf->Action() != a_blocboolean) {
        TamguInstruction* ki = new TamguSequence(global);
        Tamgu* ke;
        
        for (size_t i = 0; i < xn->nodes.size(); i++)
            Traverse(xn->nodes[i], ki);
        
        if (ki->action == a_bloc && ki->instructions.size() == 1) {
            ke = ki->instructions[0];
            //Either optional is called from within an arithmetic operation, and then we do nothing
            //or it is called from within a bloc of instructions, then we need to promote it as a ROOT
            if (ke->isFunction() || ke->isInstruction()) {
                if (!kf->isOperation() && ke->isOperation() && kf->Action() != a_assignement) {
                    Tamgu* kroot = ke->Compile(NULL);
                    ke->Remove();
                    ke = kroot;
                }
                
                if (ki->negation)
                    ke->Setnegation(ki->negation - ke->isNegation());
            }
            
            ki->Remove();
            kf->AddInstruction(ke);
            return ke;
        }
        
        ke = CloningInstruction(ki);
        kf->AddInstruction(ke);
        return ke;
    }
    
    
    if (kf->Action() != a_blocboolean || kf->Size()) {
        Tamgu* kbloc = TamguCreateInstruction(NULL, a_blocboolean);
        for (size_t i = 0; i < xn->nodes.size(); i++)
            Traverse(xn->nodes[i], kbloc);
        
        kbloc = CloningInstruction((TamguInstruction*)kbloc);
        kf->AddInstruction(kbloc);
    }
    else {
        for (size_t i = 0; i < xn->nodes.size(); i++)
            Traverse(xn->nodes[i], kf);

    }
    return kf;
}

Tamgu* TamguCode::C_ifcondition(x_node* xn, Tamgu* kf) {
    TamguInstruction* ktest;
    if (kf->Type()==a_taskellinstruction)
        ktest = new TamguInstructionTaskellIF(global, kf);
    else
        ktest = new TamguInstructionIF(global, kf);
    
	Traverse(xn->nodes[0], ktest);
	Tamgu* nxt = ktest->instructions[0];

	//Small improvement, when we have only one element of test
	//It is BOOLEANBLOC, with only one element...
	//We can get rid of it and push it to the top...
	if (nxt->Action() == a_blocboolean && nxt->InstructionSize() == 1) {
		//we push the negation up then... otherwise it would be lost
		ktest->negation = nxt->isNegation();
		nxt = nxt->Instruction(0);
		ktest->instructions[0]->Remove();
		ktest->instructions.vecteur[0] = nxt;
	}

	Tamgu* ktrue = new TamguSequence(global, ktest);
	global->Pushstack(ktrue);
	Traverse(xn->nodes[1], ktrue);
	global->Popstack();
	if (!global->debugmode && ktrue->InstructionSize() == 1 && !ktrue->Instruction(0)->isVariable()) {
		nxt = ktrue->Instruction(0);
		ktest->Putinstruction(1, nxt);
		ktrue->InstructionClear();
		ktrue->Remove();
	}

	if (xn->nodes.size() == 3) {
		Tamgu* kfalse = new TamguSequence(global, ktest);
		global->Pushstack(kfalse);
		Traverse(xn->nodes[2], kfalse);
		global->Popstack();
		if (!global->debugmode && kfalse->InstructionSize() == 1 && !kfalse->Instruction(0)->isVariable()) {
			nxt = kfalse->Instruction(0);
			ktest->Putinstruction(2, nxt);
			kfalse->InstructionClear();
			kfalse->Remove();
		}
	}
    else
        ktest->instructions.push_back(aNULL);

	return ktest;
}

//Basically, if there is an error in the Boolean expression, then we execute the code...
Tamgu* TamguCode::C_catchon(x_node* xn, Tamgu* kf) {
    TamguInstruction* ktest = new TamguInstructionCatchON(global, kf);
    
    Traverse(xn->nodes[0], ktest);
    Tamgu* nxt = ktest->instructions[0];

    //Small improvement, when we have only one element of test
    //It is BOOLEANBLOC, with only one element...
    //We can get rid of it and push it to the top...
    if (nxt->Action() == a_blocboolean && nxt->InstructionSize() == 1) {
        //we push the negation up then... otherwise it would be lost
        ktest->negation = nxt->isNegation();
        nxt = nxt->Instruction(0);
        ktest->instructions[0]->Remove();
        ktest->instructions.vecteur[0] = nxt;
    }

    Tamgu* ktrue = new TamguSequence(global, ktest);
    global->Pushstack(ktrue);
    Traverse(xn->nodes[1], ktrue);
    global->Popstack();
    if (!global->debugmode && ktrue->InstructionSize() == 1 && !ktrue->Instruction(0)->isVariable()) {
        nxt = ktrue->Instruction(0);
        ktest->Putinstruction(1, nxt);
        ktrue->InstructionClear();
        ktrue->Remove();
    }

    return ktest;
}


Tamgu* TamguCode::C_booleanexpression(x_node* xn, Tamgu* kf) {
	Tamgu* kbloc;
	Tamgu* ke;

    if (xn->token == "comparing") {
        //This is a comparison in a function parameter structure
        kbloc = TamguCreateInstruction(NULL, a_blocboolean);
        Traverse(xn->nodes[0], kbloc);
        Traverse(xn->nodes[1], kbloc);
        kbloc = CloningInstruction((TamguInstruction*)kbloc);
        kf->AddInstruction(kbloc);
        return kbloc;
    }
	if (xn->nodes.size() == 1) {
		//It is a test on a function or on a single variable...
		kbloc = TamguCreateInstruction(NULL, a_blocboolean);

		Traverse(xn->nodes[0], kbloc);
		//The test on negation stems from the fact, that negation will be tested through a_blocboolean in TamguInstruction::Exec
		//otherwise, we do not need it...
		if (kbloc->Action() == a_blocboolean && kbloc->InstructionSize() == 1 && !kbloc->isNegation()) {
			//In this case, we can get rid of it...
			ke = kbloc->Instruction(0);
			kbloc->Remove();
			kf->AddInstruction(ke);
			return ke;
		}

		kbloc = CloningInstruction((TamguInstruction*)kbloc);
		kf->AddInstruction(kbloc);
		return kbloc;
	}
	//Else, we have two expressions and an operator
	//If our operator is new to our test
	ke = kf;
	short op = global->string_operators[xn->nodes[1]->value];
	if (op != kf->Action()) {
		if (kf->Action() == a_blocboolean)
			ke->Setaction(op);
		else {
			ke = TamguCreateInstruction(kf, op);
		}
	}

	kbloc = TamguCreateInstruction(NULL, a_blocboolean);
	Traverse(xn->nodes[0], kbloc);
	if (kbloc->Action() == a_blocboolean && kbloc->InstructionSize() == 1 && !kbloc->isNegation()) {
		//In this case, we can get rid of it...
		Tamgu* k = kbloc->Instruction(0);
		kbloc->Remove();
		kbloc = k;
	}
	else
		kbloc = CloningInstruction((TamguInstruction*)kbloc);

	ke->AddInstruction(kbloc);

	if (xn->nodes.size() == 3)
		Traverse(xn->nodes[2], ke);
	else
		Traverse(xn->nodes[1], kbloc);
	return ke;
}

Tamgu* TamguCode::C_negation(x_node* xn, Tamgu* kf) {
	kf->Setnegation(true);
	return kf;
}

Tamgu* TamguCode::C_switch(x_node* xn, Tamgu* kf) {
	//We create a IF section
	TamguInstructionSWITCH* kswitch = new TamguInstructionSWITCH(global, kf);

	Traverse(xn->nodes[0], kswitch);
	int i = 1;
    if (xn->nodes[i]->token == "depend") {
            //The "with" function is detected here.
        string funcname = xn->nodes[i]->nodes[0]->value;
        short idfuncname = global->Getid(funcname);
        Tamgu* func = Declaration(idfuncname);
        if (func == NULL || !func->isFunction()) {
            stringstream message;
            message << "We can only associate a function through 'with' in a 'switch' statement";
            throw new TamguRaiseError(message, filename, current_start, current_end);
        }
        
        kswitch->function = func;
        if (func->Size() != 2) {
            stringstream message;
            message << "Wrong number of arguments or incompatible argument: '" << funcname << "' for 'switch'";
            throw new TamguRaiseError(message, filename, current_start, current_end);
        }
        i = 2;
        kswitch->usekeys = 2;
    }
    
	for (; i < xn->nodes.size(); i++)
		Traverse(xn->nodes[i], kswitch);
    if (kswitch->function == NULL) {
        bool onlybasevalue = true;
        for (i = 1; i < kswitch->instructions.size(); i += 2) {
            if (kswitch->instructions[i] == aDEFAULT)
                break;
            
            if (kswitch->instructions[i]->baseValue() == false) {
                onlybasevalue = false;
                break;
            }
            kswitch->keys[kswitch->instructions[i]->String()] = i;
        }
        kswitch->usekeys = onlybasevalue;
    }
	return kswitch;
}

Tamgu* TamguCode::C_testswitch(x_node* xn, Tamgu* kf) {
	if (xn->nodes[0]->token == "default")
		kf->AddInstruction(aDEFAULT);
	else
		Traverse(xn->nodes[0], kf);

	Tamgu* ktrue = new TamguSequence(global, kf);
	global->Pushstack(ktrue);
	Traverse(xn->nodes[1], ktrue);
	global->Popstack();
	if (!global->debugmode && ktrue->InstructionSize() == 1 && !ktrue->Instruction(0)->isVariable()) {
		kf->Putinstruction(kf->InstructionSize() - 1, ktrue->Instruction(0));
		ktrue->InstructionClear();
		ktrue->Remove();
	}

	return ktrue;
}


Tamgu* TamguCode::C_loop(x_node* xn, Tamgu* kf) {
	TamguInstructionWHILE* kwhile = new TamguInstructionWHILE(global, kf);

	Traverse(xn->nodes[0], kwhile);

	Tamgu* kseq = new TamguSequence(global, kwhile);
	global->Pushstack(kseq);
	Traverse(xn->nodes[1], kseq);
	global->Popstack();
	if (!global->debugmode && kseq->InstructionSize() == 1 && !kseq->Instruction(0)->isVariable()) {
		kwhile->Putinstruction(1, kseq->Instruction(0));
		kseq->InstructionClear();
		kseq->Remove();
	}


	Tamgu* kinst = kwhile->instructions[1];
	if (!global->debugmode && kinst->Type() == a_sequence && kinst->InstructionSize() == 1)
		kwhile->instructions.vecteur[1] = kinst->Instruction(0);

	return kwhile;
}



Tamgu* TamguCode::C_doloop(x_node* xn, Tamgu* kf) {
	TamguInstructionUNTIL* kuntil = new TamguInstructionUNTIL(global, kf);
	Traverse(xn->nodes[1], kuntil);

	Tamgu* kseq = new TamguSequence(global, kuntil);

	global->Pushstack(kseq);
	Traverse(xn->nodes[0], kseq);
	global->Popstack();
	if (!global->debugmode && kseq->InstructionSize() == 1 && !kseq->Instruction(0)->isVariable()) {
		kuntil->Putinstruction(1, kseq->Instruction(0));
		kseq->InstructionClear();
		kseq->Remove();
	}

	Tamgu* kinst = kuntil->instructions[1];
	if (!global->debugmode && kinst->Type() == a_sequence && kinst->InstructionSize() == 1)
		kuntil->instructions.vecteur[1] = kinst->Instruction(0);

	return kuntil;
}



Tamgu* TamguCode::C_for(x_node* xn, Tamgu* kf) {
	Tamgu* kbase = NULL;
	TamguInstructionFOR* kfor;
	bool protecting = false;
	if (xn->nodes[0]->token == "multideclaration") {
		kbase = new TamguSequence(global, kf);
		global->Pushstack(kbase);
		Traverse(xn->nodes[0], kbase);
		kfor = new TamguInstructionFOR(global, kbase);
		protecting = true;
	}
	else
		kfor = new TamguInstructionFOR(global, kf);

	//Initialisation
	if (!protecting)
		Traverse(xn->nodes[0], kfor);
	else
		kfor->AddInstruction(aTRUE);

	//Test
	Traverse(xn->nodes[1], kfor);
	//Increment
	Traverse(xn->nodes[2], kfor);

	Tamgu* kbloc = new TamguSequence(global, kfor);

	//Instruction
	global->Pushstack(kbloc);
	Traverse(xn->nodes[3], kbloc);
	global->Popstack();
	if (protecting)
		global->Popstack();

	if (!global->debugmode && kbloc->InstructionSize() == 1 && !kbloc->Instruction(0)->isVariable()) {
		kfor->Putinstruction(3, kbloc->Instruction(0));
		kbloc->InstructionClear();
		kbloc->Remove();
	}

	return kfor;
}

Tamgu* TamguCode::C_namespace(x_node* xn, Tamgu* kf) {
    string nm = name_space;
    name_space += xn->nodes[0]->value;
    name_space += "#";
    Traverse(xn->nodes[1], kf);
    name_space = nm;
    return kf;
}


Tamgu* TamguCode::C_blocfor(x_node* xn, Tamgu* kf) {

	Tamgu* kbloc = new TamguSequence(global, kf);
	for (int i = 0; i < xn->nodes.size(); i++)
		Traverse(xn->nodes[i], kbloc);
	return kbloc;
}

Tamgu* TamguCode::C_forin(x_node* xn, Tamgu* kf) {

	TamguInstruction* kforin;
	TamguInstruction ktemp;
	Traverse(xn->nodes[1], &ktemp);
	Tamgu* kcontainer = ktemp.instructions[0];
	bool protecting = false;
	Tamgu* kref = kf;
	Tamgu* kbase = NULL;
	if (xn->nodes[0]->nodes.size() == 2 && xn->nodes[0]->token == "declarationfor") {
		kbase = new TamguSequence(global, kf);
		xn->nodes[0]->token = "declaration";
		global->Pushstack(kbase);
		Traverse(xn->nodes[0], kbase);
		protecting = true;
		kref = kbase;
	}

	bool checkrange = false;
	bool checkforinself = false;
	short idvar = -1;
	if (kcontainer->isCallVariable() && global->isFile(kcontainer))
		kforin = new TamguInstructionFILEIN(global, kref);
	else {
		if (xn->nodes[1]->token == "arange") {
			kforin = new TamguInstructionFORINRANGE(global);
			checkrange = true;
		}
		else {
			if (xn->nodes[0]->token == "valvectortail")
				kforin = new TamguInstructionFORVECTORIN(global, kref);
			else {
				if (xn->nodes[0]->token == "valmaptail")
					kforin = new TamguInstructionFORMAPIN(global, kref);
				else {
					idvar = kcontainer->Typevariable();
					if (global->newInstance.check(idvar) && kcontainer->Function() == NULL &&
                        (global->newInstance[idvar]->isVectorContainer() ||
						global->newInstance[idvar]->isValueContainer() ||
						global->newInstance[idvar]->isMapContainer()))
						kforin = new TamguInstructionFORINVALUECONTAINER(global, kref);
					else {
						kforin = new TamguInstructionFORIN(global);										
						checkforinself = true;
					}
				}
			}
		}
	}

	Tamgu* kin = TamguCreateInstruction(kforin, a_blocloopin);
	if (kbase == NULL)
		Traverse(xn->nodes[0], kin);
	else
		Traverse(xn->nodes[0]->nodes[1], kin);

	if (!kin->Instruction(0)->Checkvariable()) {
		stringstream message;
		message << "Expecting variables in FOR to loop in";
		throw new TamguRaiseError(message, filename, current_start, current_end);
	}

	if (checkforinself) {
		
		if (kcontainer->isVectorContainer() && kcontainer->Function() == NULL) {
			if (kin->Instruction(0)->Function() == NULL) {
				kforin->Remove();
				kforin = new TamguInstructionFORINVECTOR(global, kref);
				kforin->AddInstruction(kin);
				checkforinself = false;
			}
		}

		if (checkforinself)
			kref->AddInstruction(kforin);
	}

	kin->Instruction(0)->Setevaluate(true);
	kin->Instruction(0)->Setaffectation(true);

    bool allconst=false;
	if (kforin->Type() == a_forinrange) {
		if (!kin->Instruction(0)->isCallVariable()) {
			stringstream message;
			message << "Expecting a variable in FOR";
			throw new TamguRaiseError(message, filename, current_start, current_end);
		}

		if (!global->isNumber(kin->Instruction(0))) {
			stringstream message;
			message << "Only numerical variable can be used here";
			throw new TamguRaiseError(message, filename, current_start, current_end);
		}
        switch(ktemp.instructions.size()) {
            case 1:
                //the initial value is zero and the increment is one
                kin->AddInstruction(aZERO);
                kin->AddInstruction(ktemp.instructions[0]);
                kin->AddInstruction(aONE);
                if (ktemp.instructions[0]->isConst())
                    allconst=true;
                break;
            case 2:
                //the increment is one
                kin->AddInstruction(ktemp.instructions[0]);
                kin->AddInstruction(ktemp.instructions[1]);
                if (ktemp.instructions[0]->isConst() && ktemp.instructions[1]->isConst())
                    allconst=true;
                kin->AddInstruction(aONE);
            default:
                kin->AddInstruction(ktemp.instructions[0]);
                kin->AddInstruction(ktemp.instructions[1]);
                kin->AddInstruction(ktemp.instructions[2]);
                if (ktemp.instructions[0]->isConst() && ktemp.instructions[1]->isConst() && ktemp.instructions[2]->isConst())
                    allconst=true;
        }
	}
	else
		kin->AddInstruction(kcontainer);


	//We then compile the instruction bloc
	Tamgu* ktrue = new TamguSequence(global, kforin);
	global->Pushstack(ktrue);
	Traverse(xn->nodes[2], ktrue);
	global->Popstack();
	if (protecting)
		global->Popstack();
	if (!global->debugmode && ktrue->InstructionSize() == 1 && !ktrue->Instruction(0)->isVariable()) {
		kforin->Putinstruction(1, ktrue->Instruction(0));
		ktrue->InstructionClear();
		ktrue->Remove();
	}
    
    if (checkrange) {
        Tamgu* value = kforin->instructions.vecteur[0]->Instruction(0);
        if (value->Function() == NULL) {
            switch (value->Typevariable()) {
                case a_int:
                    if (allconst)
                        value = new TamguInstructionFORINRANGECONSTINTEGER(value, kforin, global, kref);
                    else
                        value = new TamguInstructionFORINRANGEINTEGER(value, kforin, global, kref);
                    value->Setinfo(kforin);
                    kforin->Remove();
                    return value;
                case a_float:
                    if (allconst)
                        value = new TamguInstructionFORINRANGECONSTFLOAT(value, kforin, global, kref);
                    else
                        value = new TamguInstructionFORINRANGEFLOAT(value, kforin, global, kref);
                    value->Setinfo(kforin);
                    kforin->Remove();
                    return value;
                case a_decimal:
                    if (allconst)
                        value = new TamguInstructionFORINRANGECONSTDECIMAL(value, kforin, global, kref);
                    else
                        value = new TamguInstructionFORINRANGEDECIMAL(value, kforin, global, kref);
                    value->Setinfo(kforin);
                    kforin->Remove();
                    return value;
                case a_long:
                    if (allconst)
                        value = new TamguInstructionFORINRANGECONSTLONG(value, kforin, global, kref);
                    else
                        value = new TamguInstructionFORINRANGELONG(value, kforin, global, kref);
                    value->Setinfo(kforin);
                    kforin->Remove();
                    return value;
                case a_short:
                    if (allconst)
                        value = new TamguInstructionFORINRANGECONSTSHORT(value, kforin, global, kref);
                    else
                        value = new TamguInstructionFORINRANGESHORT(value, kforin, global, kref);
                    value->Setinfo(kforin);
                    kforin->Remove();
                    return value;
            }
        }
        kref->AddInstruction(kforin);
    }

	return kforin;
}



Tamgu* TamguCode::C_trycatch(x_node* xn, Tamgu* kf) {
	Tamgu* ktry = new TamguInstructionTRY(global, kf);

	string name;
	Tamgu* declaration = NULL;
	TamguCallVariable* ki;

	short id=0;
	Tamgu* dom = NULL;
	if (xn->nodes.size() != 1 && xn->nodes[1]->token == "word") {
		name = xn->nodes[1]->value;
		id = global->Getid(name);
		declaration = Declaration(id, kf);
        
        if (idcode && declaration==NULL) {
            char ch[10];
            sprintf_s(ch,10,"&%d",idcode);
            string nm(name);
            nm+=ch;
            id=global->Getid(nm);
            declaration = Declaration(id, kf);
            global->idSymbols[id]=name;
        }

		if (declaration == NULL || !declaration->isVariable()) {
			stringstream message;
			message << "Unknown variable: '" << name << "'";
			throw new TamguRaiseError(message, filename, current_start, current_end);
		}

		TamguInstruction* kaff = TamguCreateInstruction(ktry, a_assignement);

		dom = Declaror(id, kf);

        if (dom->isMainFrame())
            ki = new TamguCallGlobalVariable(id, declaration->Type(), global, kaff);
        else
            if (dom->isFunction())
                ki = new TamguCallFunctionVariable(id, declaration->Type(), global, kaff);
            else
                if (dom->isFrame())
                    ki = new TamguCallFrameVariable(id, (TamguFrame*)dom, declaration->Typevariable(), global, kaff);
                else
                    ki = new TamguCallVariable(id, declaration->Type(), global, kaff);

		kaff->AddInstruction(aNULL);
		kaff->Instruction(0)->Setaffectation(true);

	}

	Traverse(xn->nodes[0], ktry);


	TamguCallProcedure* kcatchproc = new TamguCallProcedure(global->Getid("catch"), global, ktry);

	if (xn->nodes.size() != 1) {
        if (xn->nodes[1]->token == "word") {
            if (dom->isMainFrame())
                ki = new TamguCallGlobalVariable(id, declaration->Type(), global, kcatchproc);
            else
                if (dom->isFunction())
                    ki = new TamguCallFunctionVariable(id, declaration->Type(), global, kcatchproc);
                else
                    if (dom->isFrame())
                        ki = new TamguCallFrameVariable(id, (TamguFrame*)dom, declaration->Typevariable(), global, kcatchproc);
                    else
                        ki = new TamguCallVariable(id, declaration->Type(), global, kcatchproc);

			if (xn->nodes.size() == 3) {
				if (xn->nodes[2]->token == "blocs")  {
					Tamgu* kbloc = new TamguInstructionCATCH(global, ktry);
					//Instruction
					Traverse(xn->nodes[2], kbloc);
				}
			}
		}
		else {
			if (xn->nodes[1]->token == "blocs")  {
				Tamgu* kbloc = new TamguInstructionCATCH(global, ktry);
				//Instruction
				Traverse(xn->nodes[1], kbloc);
			}
		}
	}

	return ktry;
}


Tamgu* TamguCode::C_parameters(x_node* xn, Tamgu* kcf) {
	TamguInstruction kbloc;
	short id = kbloc.idtype;
	if (kcf->Name() >= a_asserta && kcf->Name() <= a_dependency_remove)
		id = a_parameterpredicate;

    insidecall++;
	Tamgu* ki;
	TamguInstruction* k;
	for (int i = 0; i < xn->nodes.size(); i++) {
		kbloc.idtype = id;

		Traverse(xn->nodes[i], &kbloc);

		if (kbloc.instructions.last == 0) {
			stringstream message;
			message << "Error: Wrong parameter definition" << endl;
			throw new TamguRaiseError(message, filename, current_start, current_end);
		}

		ki = kbloc.instructions[0];

		if (ki->InstructionSize() == 1) {
			k = TamguCreateInstruction(kcf, ki->Action());
			ki->Clone(k);
			ki->Remove();
		}
		else
			kcf->AddInstruction(ki);

		kbloc.instructions.clear();
	}
    
    insidecall--;
	return kcf;
}
//----------------------------------------------------------------------------------------------
//-------------------- HASKELL


bool TamguCode::CheckVariable(x_node* xn, Tamgu* kf) {
	if (xn == NULL)
		return false;

	if (xn->token == "variable" || xn->token == "purevariable") {
		short id = global->Getid(xn->nodes[0]->value);
		if (kf->isDeclared(id))
			return true;
		return false;
	}

	for (size_t i = 0; i < xn->nodes.size(); i++)
	if (CheckVariable(xn->nodes[i], kf) == true)
		return true;

	return false;
}


bool TamguCode::CheckingVariableName(x_node* xn, Tamgu* kf) {
	if (xn == NULL)
		return false;

	if (xn->token == "variable" || xn->token == "purevariable") {
		short id = global->Getid(xn->nodes[0]->value);
		if (kf->Name() == id)
			return true;
		return false;
	}

	for (size_t i = 0; i < xn->nodes.size(); i++)
	if (CheckingVariableName(xn->nodes[i], kf) == true)
		return true;

	return false;
}

Tamgu* TamguCode::C_hbloc(x_node* xn, Tamgu* kf) {
	Tamgu* kseq = new TamguHBloc(global, kf);
	global->Pushstack(kseq);
	Traverse(xn->nodes[0], kseq);
	global->Popstack();
	return kseq;
}

Tamgu* TamguCode::C_taskellbasic(x_node* xn, Tamgu* kf) {
    
    return kf;
}

Tamgu* TamguCode::C_hdata(x_node* xn, Tamgu* kf) {
	//we create a frame, whose name is the value of that data structure...
    //The structure has already been analyzed...
    
    string name = xn->nodes[0]->value;
    short idname = global->Getid(name);
    TamguFrame* kframe;

    bool deriving = false;
    long maxnodes = xn->nodes.size();
    x_node* x_deriving=NULL;
    if (xn->nodes.back()->token == "deriving") {
        deriving = true;
        maxnodes--;
        x_deriving=xn->nodes.back();
        xn->nodes.pop_back();
    }

    //The structure is pre-analysed in blocs. When we reach this "$", it means that the
    //structure has already been successfully created. However, if there is a derivation from an existing frame,
    //we need to copy the function into it...
    if (xn->value == "$") {
        if (!deriving)
            return kf;
        kframe = global->frames[idname];
        x_deriving->token = "*deriving";
    }
    else {
        if (global->frames.check(idname))
            kframe = global->frames[idname];
        else {
            kframe = new TamguFrame(idname, false, global, kf);
            Tamguframeseeder::RecordFrame(idname, kframe, global);
            string info = "Create a frame with: '";
            info += name;
            info += "'";
            //We consider each frame as a potential procedure that will create a frame of the same type.
            global->RecordOneProcedure(name, info, ProcCreateFrame, P_FULL);
            global->returntypes[idname] = idname;
            //We then record this new Frame in our instructions list
            //We also store it at the TOP level, so that others can have access to it...
            mainframe.Declare(kframe->name, kframe);
            
            global->SetCompatibilities(idname);
        }
    }

    global->Pushstack(kframe);

    for (int i = 1; i < maxnodes; i++) {
		if (deriving)
			xn->nodes[i]->nodes.push_back(x_deriving);
		Traverse(xn->nodes[i], kframe);
		if (deriving)
			xn->nodes[i]->nodes.pop_back();
	}

    if (x_deriving != NULL)
        xn->nodes.push_back(x_deriving);
	global->Popstack();
	return kframe;
}

Tamgu* TamguCode::C_hdatadeclaration(x_node* xn, Tamgu* kf) {
    static tokenizer_result<string> xr;
    static bnf_tamgu bnf;

    string name = xn->nodes[0]->value;
    short idname = global->Getid(name);
    long maxnodes = xn->nodes.size();
    
    if (xn->nodes.back()->token == "*deriving") {
        maxnodes--;
        TamguFrame* localframe = NULL;
        int ii;
        if (global->frames.check(idname))
            localframe = global->frames[idname];
        
        string classname;
        for (ii = 0; ii < xn->nodes[maxnodes]->nodes.size(); ii++) {
            classname = xn->nodes[maxnodes]->nodes[ii]->value;
            if (classname == "Show" || classname == "Eq" || classname == "Ord")
                continue;
            
            short idframe = global->Getid(classname);
            localframe = global->frames[idname];
            if (!global->frames[idframe]->Pushdeclaration(localframe)) {
                stringstream message;
                message << "Derivation is limited to one frame. You cannot derive from '" << classname << "'";
                throw new TamguRaiseError(message, filename, current_start, current_end);
            }
        }
        return kf;
    }
    
	//we create a frame, whose name is the value of a subframe to the data structure...

	//We map a Taskell data structure into a frame...
	stringstream framecode;

	framecode << "frame " << name << "{";

	vector<string> variables;
	vector<string> types;
    
	string nm("d");
    if (maxnodes > 9)
        nm += "00_";
    else
        nm += "0_";
	nm += name;

	int i;
	bool subdata = false;
	bool deriving = false;
	if (xn->nodes.back()->token == "deriving") {
		deriving = true;
		maxnodes--;
	}

    char buff[3];

	if (xn->nodes[1]->token == "subdata") {
		subdata = true;
		for (i = 0; i < xn->nodes[1]->nodes.size(); i++) {
			variables.push_back(xn->nodes[1]->nodes[i]->nodes[0]->value);
			types.push_back(xn->nodes[1]->nodes[i]->nodes[1]->value);
			framecode << types[i] << " " << variables[i] << ";";
		}
	}
	else {
        if (maxnodes <= 9) {
            for (i = 1; i < maxnodes; i++) {
                nm[1] = 48 + i;
                framecode << xn->nodes[i]->value << " " << nm << ";";
                types.push_back(xn->nodes[i]->value);
                variables.push_back(nm);
            }
        }
        else {
            for (i = 1; i < maxnodes; i++) {
                sprintf_s(buff,3,"%02d",i);
                nm[1] = buff[0];
                nm[2] = buff[1];
                framecode << xn->nodes[i]->value << " " << nm << ";";
                types.push_back(xn->nodes[i]->value);
                variables.push_back(nm);
            }
        }
	}

    //We need a simple _initial with nothing in it, together with another initial function
    //This is in case of a data assignment...
    framecode << "function _initial() {}";

	//Now we need an initial function
	framecode << "function _initial(";
	nm[0] = 'p';
	for (i = 0; i < variables.size(); i++) {
		if (i)
			framecode << ",";
        if (maxnodes <= 9)
            nm[1] = 49 + i;
        else {
            sprintf_s(buff,3,"%02d",i+1);
            nm[1] = buff[0];
            nm[2] = buff[1];
        }
		framecode << types[i] << " " << nm;
	}

	framecode << ") {";
	for (i = 0; i < variables.size(); i++) {
		framecode << variables[i] << "=";
        if (maxnodes <= 9)
            nm[1] = 49 + i;
        else {
            sprintf_s(buff,3,"%02d",i+1);
            nm[1] = buff[0];
            nm[2] = buff[1];
        }
		framecode << nm << ";";
	}
	framecode << "}";

	TamguFrame* localframe = NULL;
	if (deriving) {
		int ii;		
		if (global->frames.check(idname))
			localframe = global->frames[idname];
		
		string classname;
		for (ii = 0; ii < xn->nodes[maxnodes]->nodes.size(); ii++) {
			classname = xn->nodes[maxnodes]->nodes[ii]->value;
			if (classname == "Show") {
				//The string function...
				framecode << "function string() {return(" << "\"<" << name << '"';
				if (subdata) {
					for (i = 0; i < variables.size(); i++) {
						if (i)
							framecode << "+', '";
						framecode << "+' " << variables[i] << "='+" << variables[i];
					}
				}
				else {
					for (i = 0; i < variables.size(); i++)
						framecode << "+' '+" << variables[i];
				}

				framecode << "+'>');}";
				continue;
			}
			if (classname == "Eq") {
				//We add equality comparison between elements...
				framecode << "function ==(" << name << " x) {if (";
				for (i = 0; i < variables.size(); i++) {
					if (i)
						framecode << " or ";
					framecode << variables[i] << "!= x." << variables[i];
				}
				framecode << ") return(false);return(true);}";

				framecode << "function !=(" << name << " x) {if (";
				for (i = 0; i < variables.size(); i++) {
					if (i)
						framecode << " or ";
					framecode << variables[i] << "== x." << variables[i];
				}
				framecode << ") return(false);return(true);}";
				continue;
			}

			if (classname == "Ord") {
				//We add comparison between elements...
				framecode << "function <(" << name << " x) {if (";
				for (i = 0; i < variables.size(); i++) {
					if (i)
						framecode << " or ";
					framecode << variables[i] << ">= x." << variables[i];
				}
				framecode << ") return(false);return(true);}";
				framecode << "function >(" << name << " x) {if (";
				for (i = 0; i < variables.size(); i++) {
					if (i)
						framecode << " or ";
					framecode << variables[i] << "<= x." << variables[i];
				}
				framecode << ") return(false);return(true);}";
				framecode << "function <=(" << name << " x) {if (";
				for (i = 0; i < variables.size(); i++) {
					if (i)
						framecode << " or ";
					framecode << variables[i] << "> x." << variables[i];
				}
				framecode << ") return(false);return(true);}";
				framecode << "function >=(" << name << " x) {if (";
				for (i = 0; i < variables.size(); i++) {
					if (i)
						framecode << " or ";
					framecode << variables[i] << "< x." << variables[i];
				}
				framecode << ") return(false);return(true);}";
				continue;
			}
			short idframe = global->Getid(classname);
			if (!global->frames.check(idframe)) {
				stringstream message;
				message << "Unknown data structure: '" << classname << "'";
				throw new TamguRaiseError(message, filename, current_start, current_end);
			}
			
			if (localframe == NULL) {
				localframe = new TamguFrame(idname, false, global, &mainframe);
				Tamguframeseeder::RecordFrame(idname, localframe, global);
                string info = "Create a frame with: '";
                info += name;
                info += "'";
				//We consider each frame as a potential procedure that will create a frame of the same type.
				global->RecordOneProcedure(name, info, ProcCreateFrame, P_FULL);
				global->returntypes[idname] = idname;
				//We record the compatibilities, which might come as handy to check function argument
				global->SetCompatibilities(idname);
				if (kf->isFrame()) {
					global->compatibilities[idname][kf->Name()] = true;
					global->strictcompatibilities[idname][kf->Name()] = true;
				}
			}
		}
	}
	
	if (subdata) {
		for (i = 0; i < variables.size(); i++)
			framecode << "function _" << variables[i] << "() :: "<<types[i]<<" { return(" << variables[i] << ");}";
	}

	framecode << "}";
	
    string s_frame = framecode.str();
    global->tamgu_tokenizer.tokenize<string>(s_frame, xr);
		
	x_node* xstring = bnf.x_parsing(&xr, FULL);
	setstartend(xstring, xn);

    //In this way, it will be deleted no matter what...
    std::unique_ptr<x_node> dxnode(xstring);
    
	//Specific case, when the declaration is: <data TOTO = TOTO...>
	//In this case, we do not want this element to be a subframe of itself...
	//So we pop up the element in the stack
	if (idname == kf->Name())
		global->Popstack();

	Traverse(xstring->nodes[0]->nodes[0], &mainframe);
	
	//And we put it back...
	if (idname == kf->Name())
		global->Pushstack(kf);
	else {//otherwise, we add a strictcompatibilities between the mother and the daughter frames...
		global->compatibilities[kf->Name()][idname] = true;
		global->strictcompatibilities[kf->Name()][idname] = true;
	}

	return kf;
}

Tamgu* TamguCode::C_hdeclaration(x_node* xn, Tamgu* kf) {
    
    string name = xn->nodes[0]->value;
    short idname = global->Getid(name);
    
    if (!global->frames.check(idname)) {
        stringstream message;
        message << "Unknown data structure (or frame): '" << name << "'";
        throw new TamguRaiseError(message, filename, current_start, current_end);
    }
    
    TamguFrame* kframe = global->frames[idname];
    TamguFrameParameter* fparam = new TamguFrameParameter(kframe->Name(), global, kf);
    Tamgu* var;
    if (kframe->variables.size() != xn->nodes.size() - 1) {
        stringstream message;
        message << "Data structure mismatch (or frame): '" << name << "'";
        throw new TamguRaiseError(message, filename, current_start, current_end);
    }
    short argtype;
    short id;
    for (long i = 0; i < kframe->variables.size(); i++) {
        argtype = kframe->variables[i]->Typevariable();
        if (xn->nodes[i + 1]->token == "word") {
            id = global->Getid(xn->nodes[i + 1]->value);
            if (id == a_universal)
                continue;
            if (argtype == a_universal || argtype == a_self || argtype == a_let)
                var = new TamguTaskellSelfVariableDeclaration(global, id, a_self);
            else
                var = new TamguTaskellVariableDeclaration(global, id, argtype);
            kf->Declare(id, var);
            fparam->Declare(kframe->variables[i]->Name(), var);
        }
        else {
            //This is a pure value...
            TamguInstruction ti;
            Traverse(xn->nodes[i+1], &ti);
            fparam->Declare(kframe->variables[i]->Name(), ti.instructions[0]);
        }
    }
    return fparam;
}

Tamgu* TamguCode::C_ontology(x_node* xn, Tamgu* kf) {
	//we enrich our ontology
	short idmaster = global->Getid(xn->nodes.back()->value);
	global->hierarchy[idmaster][idmaster] = true;
	short id;
	for (int i = 0; i < (int)xn->nodes.size() - 1; i++) {
		id = global->Getid(xn->nodes[i]->value);
		global->hierarchy[id][idmaster] = true;
	}

	return kf;
}

bool Getlet(x_node* xn, bool& glob) {
    if (xn->token=="letkeyword") {
        if (xn->nodes.size() == 2)
            glob = true;
        return true;
    }
    for (long i = 0; i < xn->nodes.size(); i++) {
        if (Getlet(xn->nodes[i], glob))
            return true;
    }
    return false;
}

Tamgu* TamguCode::C_curryingleft(x_node* xn, Tamgu* kf) {
    TamguOperatorParameter* tmc = new TamguOperatorParameter(true, global, kf);
    for (long i = 0; i < xn->nodes.size(); i++)
        Traverse(xn->nodes[i], tmc);
    return kf;
}

Tamgu* TamguCode::C_curryingright(x_node* xn, Tamgu* kf) {
    TamguOperatorParameter* tmc = new TamguOperatorParameter(false, global, kf);
    for (long i = 0; i < xn->nodes.size(); i++)
        Traverse(xn->nodes[i], tmc);
    return kf;
}

Tamgu* TamguCode::C_telque(x_node* xn, Tamgu* kf) {
	//We deactivate temporarily the instance recording...

	TamguCallFunctionTaskell* kint = NULL;

	if (kf->Type() == a_calltaskell)
		kint = (TamguCallFunctionTaskell*)kf;


	//hmetafunctions: filter, map, zipWith, takeWhile, scan, fold
	if (xn->nodes.size() && xn->nodes[0]->token == "hmetafunctions") {
		if (kint == NULL)
			kint = new TamguCallFunctionTaskell(global, kf);
		kint->Init(NULL);
		kf = Traverse(xn->nodes[0]->nodes[0], kint);
        return kf;
	}

	short idname;
	int i;
	TamguFunctionLambda* kfuncbase = NULL;

	int first = 0;
	short return_type = -1;
	bool onepushtoomany = false;
	
	Tamgu* kprevious = NULL;

	vector<Taskelldeclaration*> localtaskelldeclarations;
	
	bool clearlocaltaskelldeclarations = false;
	bool taskelldeclarationfound = false;
	char concept = 0;
	if (xn->nodes[0]->token == "taskelldeclaration")
		taskelldeclarationfound = true;
	else
	if (xn->nodes[0]->token == "returntaskelldeclaration") {
		x_node* hdecl = xn->nodes[0];
		xn->nodes.erase(xn->nodes.begin());
		TamguFunctionLambda localfunc(0);
		Traverse(hdecl, &localfunc);
		return_type = localfunc.returntype;
		localtaskelldeclarations = localfunc.taskelldeclarations;
		localfunc.taskelldeclarations.clear();
		delete hdecl;
		clearlocaltaskelldeclarations = true;

        //hmetafunctions: filter, map, zipWith, takeWhile, scan, fold
        if (xn->nodes.size() && xn->nodes[0]->token == "hmetafunctions") {
            if (kint == NULL)
                kint = new TamguCallFunctionTaskell(global, kf);
            kint->Init(NULL);
            
            if (return_type != -1 && kint->body->returntype == a_null) {
                kint->returntype = return_type;
                kint->body->returntype = return_type;
                kint->body->hdeclared = true;
            }

            kf = Traverse(xn->nodes[0]->nodes[0], kint);
            return kf;
        }
	}
	else
	if (xn->nodes[0]->token == "hconcept") {
		if (xn->nodes[0]->value == "concept")
			concept = 1;
		else
		if (xn->nodes[0]->value == "property")
			concept = 2;
		else
			concept = 3;
		x_node* hdecl = xn->nodes[0];
		xn->nodes.erase(xn->nodes.begin());
		delete hdecl;
	}
	

	if (xn->nodes[0]->token == "taskell" ||  taskelldeclarationfound) {
        bool maybe = false;
		string name = xn->nodes[0]->nodes[first]->value;
		idname = global->Getid(name);

		if (global->procedures.check(idname)) {
			stringstream message;
			message << "Error: Predefined procedure, consider choosing another name: '" << name << "'";
			throw new TamguRaiseError(message, filename, current_start, current_end);
		}

		kprevious = kf->Declaration(idname);
		if (kprevious == NULL) {
			if (!kf->isFrame() && !kf->isMainFrame()) {
				//we check for a hdeclared declaration function...
				kprevious = mainframe.Declaration(idname);
				//If we have a function declaration with the same name, but without instructions and a hdeclared, we keep it...
				if (kprevious != NULL && !kprevious->Puretaskelldeclaration())
					kprevious = NULL;
			}
		}
		else {
			if (kf->isFrame() && idname == a_string) {
				//In that case, if might be a replacement, only if the name is a_string...
				//We aloow for this potential replacement, because the function string is actually made on the fly
				//in data structure...
				kprevious = NULL;
			}
		}

		if (kprevious != NULL) {
			if (taskelldeclarationfound || kprevious->Type() != a_lambda) {
				stringstream message;
				message << "Error: A function with this name already exists: '" << name << "'";
				throw new TamguRaiseError(message, filename, current_start, current_end);
			}

			kfuncbase = (TamguFunctionLambda*)kprevious;
			if (kfuncbase->hdeclared) {
				if (return_type == -1) {
					return_type = kfuncbase->returntype;
                    maybe = kfuncbase->maybe;
					localtaskelldeclarations = kfuncbase->taskelldeclarations;
				}
			}

			kfuncbase = new TamguFunctionLambda(idname, global);
			
			if (kprevious->Puretaskelldeclaration()) {
				if (kf->hasDeclaration())
					kf->Declare(idname, kfuncbase);
				else //in that case, it means that we are returning a function as result...
					kf->AddInstruction(new TamguGetFunctionLambda(kfuncbase, global));
			}
			//we keep the top function as the reference in the declarations list (which will be available throughout the stack)
			kfuncbase->declarations[idname] = kprevious;
			kprevious->Addfunction(kfuncbase);			
		}
		else {
			kfuncbase = new TamguFunctionLambda(idname, global);
			if (kf->hasDeclaration())
				kf->Declare(idname, kfuncbase);
			else //in that case, it means that we are returning a function as result...
				kf->AddInstruction(new TamguGetFunctionLambda(kfuncbase, global));			

			if (concept) {
				global->concepts[idname] = kfuncbase;
				global->hierarchy[idname][idname] = true;
				if (concept == 2)
					global->properties[idname] = kfuncbase;
				else
				if (concept == 3)
					global->roles[idname] = kfuncbase;
			}
		}

		if (taskelldeclarationfound) {
			Traverse(xn->nodes[0]->nodes[1], kfuncbase);
			return kfuncbase;
		}

		if (kf->isFrame()) {
			//We declare our function within a frame...
			unsigned int a = 1 << kfuncbase->parameters.size();
			if (global->framemethods.check(idname))
				global->framemethods[idname] |= a;
			else
				global->framemethods[idname] = a;
		}
			

		kfuncbase->choice = 0;
		short id;
		Tamgu* var;
		
		first++;
		//If no hdeclared has been declared, we declare each variable as a Self variable
		if (return_type == -1) {
			for (i = first; i < xn->nodes[0]->nodes.size(); i++) {
				if (xn->nodes[0]->nodes[i]->token == "word") {
					id = global->Getid(xn->nodes[0]->nodes[i]->value);
                    if (id == a_universal)
                        kfuncbase->AddInstruction(aUNIVERSAL);
                    else {
                        var = new TamguTaskellSelfVariableDeclaration(global, id, a_self, kfuncbase);
                        kfuncbase->Declare(id, var);
                    }
				}
				else {//it is a taskellvector...
					Traverse(xn->nodes[0]->nodes[i], kfuncbase);
				}
			}
		}
		else {
			//In that case, we know the intput, we need to do something about it...
			kfuncbase->returntype = return_type;
            kfuncbase->hdeclared = true;
            kfuncbase->maybe = maybe;
			if (clearlocaltaskelldeclarations)
				kfuncbase->taskelldeclarations = localtaskelldeclarations;
			else
				kfuncbase->settaskelldeclarations(localtaskelldeclarations);

			long sz = xn->nodes[0]->nodes.size() - first;
			if (sz != localtaskelldeclarations.size()) {
				stringstream message;
				message << "The declaration does not match the argument list of the function";
				throw new TamguRaiseError(message, filename, current_start, current_end);
			}
			short argtype;
			Tamgu* local;
			for (i = first; i < xn->nodes[0]->nodes.size(); i++) {
				argtype = localtaskelldeclarations[i - first]->Type();
				if (xn->nodes[0]->nodes[i]->token == "word") {
					id = global->Getid(xn->nodes[0]->nodes[i]->value);
                    if (id == a_universal)
                        kfuncbase->AddInstruction(aUNIVERSAL);
                    else {
                        if (argtype == a_universal || argtype == a_self || argtype == a_let)
                            var = new TamguTaskellSelfVariableDeclaration(global, id, a_self, kfuncbase);
                        else
                            var = new TamguTaskellVariableDeclaration(global, id, argtype, kfuncbase);
                        kfuncbase->Declare(id, var);
                    }
				}
				else {//it is a taskellvector...					
					Traverse(xn->nodes[0]->nodes[i], kfuncbase);
					local = kfuncbase->parameters.back();
					if (argtype == a_vector) {
						if (!local->isVectorContainer()) {
							stringstream message;
							message << "The argument: " << (i - first) + 1 << " does not match the hdeclared description";
							throw new TamguRaiseError(message, filename, current_start, current_end);
						}
					}
					else if (argtype == a_map) {
						if (!local->isMapContainer()) {
							stringstream message;
							message << "The argument: " << (i - first) + 1 << " does not match the hdeclared description";
							throw new TamguRaiseError(message, filename, current_start, current_end);
						}
					}					
				}
			}
			localtaskelldeclarations.clear();
		}

        //We check if we have constant arguments...
        kfuncbase->Constanttaskelldeclaration();
        
		if (concept) {
			if (concept == 1 && kfuncbase->Size() != 1) {
				stringstream message;
				message << "Concept requires one single parameter:" << name;
				throw new TamguRaiseError(message, filename, current_start, current_end);
			}

			if (concept == 2 && kfuncbase->Size() != 2) {
				stringstream message;
				message << "Property requires two parameters:" << name;
				throw new TamguRaiseError(message, filename, current_start, current_end);
			}

			if (concept == 3 && kfuncbase->Size() == 0) {
				stringstream message;
				message << "Role requires at least one parameter:" << name;
				throw new TamguRaiseError(message, filename, current_start, current_end);
			}
		}

		onepushtoomany = true;
		global->Pushstack(kfuncbase);
		kfuncbase->choice = 1;

        if (xn->nodes.size() == 2) {//we do not have a range or a Boolean expression
			if (xn->nodes[1]->token == "hmetafunctions") {
                if (kint == NULL) //it is a function declaration, we create it out of the main loop...
                    kint = new TamguCallFunctionTaskell(global);
                kint->Init(kfuncbase);
				Traverse(xn->nodes[1], kint);
				global->Popstack();
				return kf;
			}

			if (xn->nodes[1]->token == "hbloc") {
				Traverse(xn->nodes[1], kfuncbase);
				global->Popstack();
				return kf;
			}

			Tamgu* ki = C_ParseReturnValue(xn, kfuncbase);
			TamguInstruction kbloc;

			Traverse(xn->nodes[1], &kbloc); //compiling a return value section
			ki->AddInstruction(kbloc.instructions[0]);
			return_type = kbloc.instructions[0]->Returntype();
			if (return_type != a_none) {
				if (kfuncbase->Returntype() != a_none) {
					if (global->Compatiblestrict(kfuncbase->Returntype(), return_type) == false) {
						stringstream message;
						message << "Type mismatch... Expected: '" << global->Getsymbol(kfuncbase->Returntype()) << "' Proposed: '" << global->Getsymbol(return_type) << "'";
						throw new TamguRaiseError(message, filename, current_start, current_end);
					}
				}
				else
					kfuncbase->returntype = return_type;
			}
            
			//In that case, we do not need anything else...
			global->Popstack();
			return kf;
		}

        if (kint == NULL) //it is a function declaration, we create it out of the main loop...
            kint = new TamguCallFunctionTaskell(global);
        kint->Init(kfuncbase);
	}

	//It is a direct call then
    if (kint == NULL) {
        kint = new TamguCallFunctionTaskell(global, kf);
        kint->Init(NULL);
    }
	TamguFunctionLambda* kfunc = kint->body;
	TamguLambdaDomain* lambdadom = kfunc->lambdadomain;

	//If we have a hdeclared, then we have a return_type...
	if (return_type != -1 && kfunc->returntype == a_null) {		
		if (localtaskelldeclarations.size() != 0) {
			stringstream message;
			message << "Only a return type can declared in a lambda expression";
			throw new TamguRaiseError(message, filename, current_start, current_end);
		}
        kint->returntype = return_type;
		kfunc->returntype = return_type;
		kfunc->hdeclared = true;
	}

	kint->body->choice = 1;

	bool addreturn = false;
	Tamgu* kret = NULL;
	global->Pushstack(kint);
	global->Pushstack(lambdadom);
	global->Pushstack(kfunc);

	//Mapping implementation equivalent to:   < operation with &mapping; | &mapping; <- expressions >;
	/*
	To understand what we are doing here, we need to take an example to show what we are producing...
	For instance, <x+1 | x <- toto, x!=1> produces the following code:

	First, we create a variable for x

	 x <- loop(toto) //this is added to the lambdadom structure, in which the number of elements will later defined which code to call
	 //The return value is the first element... This is why the final code in the return statement processes the first element, that was skipped in the parsing
	 if (x!=1)
		return(x+1);

     //if you have a hbloc {...}, then it is added to the "if"

	 <x+1 | x <- toto, {println(x+1);}, x!=1>

	 produces:
	 x <- loop(toto) //this is added to the lambdadom structure, in which the number of elements will later defined which code to call
	 //The return value is the first element...
	 if (x!=1) {
		 println(x+1);
		 return(x+1);
     }
		  
	*/

	string xname;
	bool conjunction = false;
	int inc = 1;
	string tok;
	//We implement a pre-declaration of all variables...
	std::unique_ptr<x_node> cpnode(new x_node(false));
	std::unique_ptr<x_node> aconstant(new x_node("aconstant", "null"));
	//hrange is a pure expression with no return value: <x <- v, x!=10>...
	//we flatten the structure, and add the remaining elements...
	
	if (xn->nodes[0]->token == "hrange") {		
		cpnode->nodes.push_back(aconstant.get());
		for (i = 0; i < xn->nodes[0]->nodes.size(); i++)
			cpnode->nodes.push_back(xn->nodes[0]->nodes[i]);
		for (i = 1; i < xn->nodes.size(); i++)
			cpnode->nodes.push_back(xn->nodes[i]);
		xn = cpnode.get();
	}

	bool hbloc = false;
    string letname;
	Tamgu* krecipient = kfunc;
	TamguInstructionTaskellIF* ktest = NULL;
	//we predeclare our variables in where and range...
	for (i = first; i < xn->nodes.size(); i++) {
		tok = xn->nodes[i]->token;
		if (tok == "letmin") {
			addreturn = true;
			inc = 0;
			continue;
		}

		if (tok == "hbloc") {
			hbloc = true;
			continue;
		}

		if (tok == "range") {
			xname = xn->nodes[i]->nodes[0]->token;
			if (xname == "hbloc")
				hbloc = true;
			else {
                if (xname == "let") {
                    Getlet(xn->nodes[i], kfunc->storage);
					DeclareVariable(xn->nodes[i]->nodes[0]->nodes[1], kfunc);
                    kfunc->storage = false;
                }
				else
				if (xname != "hbloc")
					DeclareVariable(xn->nodes[i]->nodes[0], lambdadom);
			}
			continue;
		}

		//the hbloc are stored into the return bloc...
		if (tok == "hbooleanexpression" && hbloc) {
			//In that case, we prepare a return TaskellIF, that will be inserted later in the code
			//when the hbooleanexpression will checked against again.
			kfunc->choice = 1;
			//We add our return statement with our variable... However, in this case
			//we do not place our test in the function...
			ktest = new TamguInstructionTaskellIF(global, NULL);
			Traverse(xn->nodes[i], ktest);
			Tamgu* kobj = ktest;
			bool neg = ktest->Instruction(0)->isNegation();
			Tamgu* nxt;
			while (kobj != NULL && kobj->InstructionSize() == 1) {
				if (kobj->isNegation())
					neg = true;
				nxt = kobj;
				kobj = (TamguObject*)kobj->Instruction(0);
				if (nxt != ktest) {
					nxt->InstructionClear();
					nxt->Remove();
				}
			}
			if (kobj != ktest->Instruction(0))
				ktest->instructions.vecteur[0] = kobj;
			ktest->Setnegation(neg);

			//We create an intermediary instruction as "the then" of test
			//this is a specific class, where elements are added before the last one...
			krecipient = new TamguBeforeLast(global, ktest);
			//In order to add it to the test return on true
			kret = new TamguCallReturn(global, krecipient);
			continue;
		}

		if (tok == "whereexpression")
			Traverse(xn->nodes[i], kint);
	}

	//The global loop...
	for (i = first + inc; i < xn->nodes.size(); i++) {
		tok = xn->nodes[i]->token;
		if (tok == "whereexpression")
			continue;

		if (tok == "hoperator") {
			conjunction = false;
			if (xn->nodes[i]->value == ";")
				conjunction = true;
			continue;
		}

		if (tok == "letmin") {
			Traverse(xn->nodes[i], krecipient);
			continue;
		}

		if (tok == "hinexpression") {
			kret = C_ParseReturnValue(xn, kfunc, 1);
			Traverse(xn->nodes[i], kret);
			continue;
		}

		if (tok == "hbooleanexpression") {
			if (hbloc) {
				if (ktest != NULL)
					kfunc->AddInstruction(ktest);
				continue;
			}

			kfunc->choice = 1;
			//We add our return statement with our variable...
			ktest = new TamguInstructionTaskellIF(global, kfunc);
			Traverse(xn->nodes[i], ktest);
			Tamgu* kobj = ktest;
			bool neg = ktest->Instruction(0)->isNegation();
			Tamgu* nxt;
			while (kobj != NULL && kobj->InstructionSize() == 1) {
				if (kobj->isNegation())
					neg = true;
				nxt = kobj;
				kobj = (TamguObject*)kobj->Instruction(0);
				if (nxt != ktest) {
					nxt->InstructionClear();
					nxt->Remove();
				}
			}
			if (kobj != ktest->Instruction(0))
				((TamguInstruction*)ktest)->instructions.vecteur[0] = kobj;
			ktest->Setnegation(neg);

			//In order to add it to the test return on true
			kret = new TamguCallReturn(global, ktest);
			continue;
		}

		if (tok == "hbloc") {
			Traverse(xn->nodes[i], krecipient);
			continue;
		}

		if (tok == "range") {
			xname = xn->nodes[i]->nodes[0]->token;
			if (xname == "hbloc") {
				Traverse(xn->nodes[i]->nodes[0], krecipient);
				continue;
			}
			if (xname == "let") {
				xname = xn->nodes[i]->nodes[0]->nodes[1]->token;
				if (xname == "valvectortail") {
					BrowseVariableVector(xn->nodes[i]->nodes[0]->nodes[1], kfunc);
					delete xn->nodes[i]->nodes[0]->nodes[0];
					xn->nodes[i]->nodes[0]->nodes.erase(xn->nodes[i]->nodes[0]->nodes.begin());
					xn->nodes[i]->nodes[0]->token = "affectation";
					Traverse(xn->nodes[i]->nodes[0], kfunc);
				}
				else {
					if (xname == "valmaptail") {
						BrowseVariableMap(xn->nodes[i]->nodes[0]->nodes[1], kfunc);
						delete xn->nodes[i]->nodes[0]->nodes[0];
						xn->nodes[i]->nodes[0]->nodes.erase(xn->nodes[i]->nodes[0]->nodes.begin());
						xn->nodes[i]->nodes[0]->token = "affectation";
						Traverse(xn->nodes[i]->nodes[0], kfunc);
					}
                    else {
                        Getlet(xn->nodes[i], kfunc->storage);
						Traverse(xn->nodes[i]->nodes[0], kfunc); //a let, a simple variable affectation...
                        kfunc->storage = false;
                    }
				}
				continue;
			}

			if (xname == "valvectortail") {
				lambdadom->adding = false;
				lambdadom->local = true;
				BrowseVariableVector(xn->nodes[i]->nodes[0], lambdadom);
				lambdadom->adding = true;
				Traverse(xn->nodes[i]->nodes[0], lambdadom);
			}
			else {
				if (xname == "hvalmaptail") {
					lambdadom->adding = false;
					lambdadom->local = true;
					BrowseVariableMap(xn->nodes[i]->nodes[0], lambdadom);
					lambdadom->adding = true;
					Traverse(xn->nodes[i]->nodes[0], lambdadom);
				}
				else
					BrowseVariable(xn->nodes[i]->nodes[0], lambdadom);
			}

			if (xn->nodes[i]->nodes[1]->token == "hmetafunctions") {
				TamguCallFunctionTaskell* klocal = new TamguCallFunctionTaskell(global, lambdadom);
				klocal->Init(NULL);
				Traverse(xn->nodes[i]->nodes[1], klocal);
			}
			else
				Traverse(xn->nodes[i]->nodes[1], lambdadom);

			if (conjunction)
				lambdadom->AddInstruction(aZERO);
			else
				lambdadom->AddInstruction(aFALSE);
		}
	}

	if (!addreturn) {
		if (kret == NULL)
			//we create one, whose value is returned by our return(value) that we add to the end of the function
			kret = C_ParseReturnValue(xn, kfunc);

		if (kret != NULL) {
			//This is a case when we do not have a value to return... In that case, we do not need to process it...
			if (xn->nodes[first]->value == "null")
				kret->AddInstruction(aNULL);
			else {
				//When a value is returned by the Taskell expression as in : <x | x <-...>
				//we need to process it here.
                TamguInstruction kbloc(a_taskellinstruction);
                Traverse(xn->nodes[first], &kbloc);
                Tamgu* ki = kbloc.instructions[0];
				kret->AddInstruction(ki);
				//We try to check the return type against the expected one, if one had been provided before with: t -> t ->t
				return_type = ki->Returntype();
				if (return_type != a_none) {
					if (kfunc->Returntype() != a_none) {
						//We check against the expected type
						if (global->Compatiblestrict(kfunc->Returntype(), return_type) == false) {
							stringstream message;
							message << "Type mismatch... Expected: '" << global->Getsymbol(kfunc->Returntype()) << "' Proposed: '" << global->Getsymbol(return_type) << "'";
							throw new TamguRaiseError(message, filename, current_start, current_end);
						}
					}
					else
						kfunc->returntype = return_type;
				}
			}
		}
    }

	global->Popstack();
	global->Popstack();
	global->Popstack();
	if (onepushtoomany)
		//In that case, we need to copy onto the current function (see TAMGUIAPPLY) all the variables that were declared
		//in funcbase, for them to be accessible from the code...			
		global->Popstack();

	return kint;
}

Tamgu* TamguCode::C_ParseReturnValue(x_node* xn, TamguFunctionLambda* kf, char adding) {
	kf->choice = 1;
	if (adding == 1 && kf->instructions.size() != 0 && kf->instructions.back()->isaIF())
		return NULL;

	if (kf->instructions.size() == 0 || (!kf->instructions.back()->isaIF() && kf->instructions.back()->Type() != a_return))
		return new TamguCallReturn(global, kf);


	x_node nvar("variable", "&return;", xn);
	creationxnode("word", nvar.value, &nvar);
	kf->choice = 1;

	Tamgu* returnstatement = kf->instructions.back();
	kf->instructions.pop_back();
	TamguCallReturn* kcallret;
	if (returnstatement->isaIF())
		kcallret = (TamguCallReturn*)returnstatement->Instruction(1);
	else
		kcallret = (TamguCallReturn*)returnstatement;

	if (adding == 2) {
		TamguCallReturn* k = new TamguCallReturn(global, NULL);
		k->argument = kcallret->argument;
		kcallret = k;
	}


	TamguInstruction* ki = NULL;
	//We create our variable if necessary...
	if (!kf->declarations.check(a_idreturnvariable)) {
		Tamgu* kret = kcallret->argument;
		kcallret->argument = aNOELEMENT;

		Tamgu* va = new TamguTaskellSelfVariableDeclaration(global, a_idreturnvariable, a_self, kf);
		kf->Declare(a_idreturnvariable, va);

		ki = TamguCreateInstruction(NULL, a_assignement);
		Traverse(&nvar, ki);
		//We create an accumulator value in the function instructions...
		ki->AddInstruction(kret);
		ki->Instruction(0)->Setaffectation(true);
		//Its value will be set by the next instructions after the call to C_ParseReturnValue
		kf->AddInstruction(ki);
		Traverse(&nvar, kcallret);
	}

	if (adding == 1) {
		ki = TamguCreateInstruction(NULL, a_assignement);
		Traverse(&nvar, ki);
		ki->Instruction(0)->Setaffectation(true);

		kf->AddInstruction(ki);
	}
	kf->AddInstruction(returnstatement);
	return ki;
}


Tamgu* TamguCode::C_hlambda(x_node* xn, Tamgu* kf) {
	int idname = global->Getid("&lambdataskell;");
	TamguFunctionLambda* kfunc = new TamguFunctionLambda(idname, global);
	Tamgu* var;
	for (int i = 0; i < (int)xn->nodes.size() - 1; i++) {
		if (xn->nodes[i]->token == "word") {
			idname = global->Getid(xn->nodes[i]->value);
			var = new TamguTaskellSelfVariableDeclaration(global, idname, a_self, kfunc);
			kfunc->Declare(idname, var);
		}
		else
			Traverse(xn->nodes[i], kfunc);
	}
	kfunc->choice = 1;
	var = new TamguCallReturn(global, kfunc);
	global->Pushstack(kfunc);
	TamguInstruction* kbloc = TamguCreateInstruction(NULL, a_bloc);
	Traverse(xn->nodes.back(), kbloc);

    Tamgu* kroot;
	if (kbloc->action == a_bloc && kbloc->instructions.size() == 1) {
		kroot = kbloc->instructions[0]->Compile(NULL);
		var->AddInstruction(kroot);
		if (kroot != kbloc->instructions[0])
			kbloc->instructions[0]->Remove();
		kbloc->Remove();
	}
	else {
		kroot = CloningInstruction(kbloc);
		var->AddInstruction(kroot);
	}

	global->Popstack();
	return kfunc;
}


Tamgu* TamguCode::C_hcompose(x_node* xn, Tamgu* kbase) {
	if (kbase->Type() != a_calltaskell) {
		TamguCallFunctionTaskell* kf = new TamguCallFunctionTaskell(global, kbase);
		kf->Init(NULL);
		return Traverse(xn->nodes[0], kf);
	}
	return Traverse(xn->nodes[0], kbase);
}

Tamgu* TamguCode::C_hfunctioncall(x_node* xn, Tamgu* kf) {
	//We rebuild a complete tree, in order to benefit from the regular parsing of a function call		
	if (xn->nodes[0]->token == "telque") {
		TamguInstruction ai;
		Traverse(xn->nodes[0], &ai);		
		Tamgu* calllocal = ai.instructions[0];
		
		TamguFunctionLambda* kfunc = ((TamguCallFunctionTaskell*)calllocal)->body;
		TamguLambdaDomain* lambdadom = kfunc->lambdadomain;

		//We are dealing with a simple return function
		//The value is stored in a variable, which is one step before the return statement
		//We store the calculus in a intermediary variable, whose name is &return; and which is created
		//with C_ParseReturnValue function...
		if (lambdadom->instructions.size() == 0 && kfunc->instructions.size() == 1) {
			calllocal = kfunc->instructions.back()->Argument(0);
			ai.instructions[0]->Remove();
			kfunc->Remove();
		}

		//This is a specific case, where we have a call without any parameters that has been falsely identify as a variable
		//We need to rebuilt it...
		if (calllocal->isFunctionParameter()) {
			x_node* nop = new x_node("taskellcall", "", xn);

			//this is a call without any variable, which was misinterpreted... It should be another call...
			x_node* nfunc = creationxnode("functioncall", global->Getsymbol(calllocal->Name()), nop);
			nfunc->nodes.push_back(xn->nodes[0]);

			ai.instructions.clear();
			Tamgu* kcall = Traverse(nop, &ai);
			nfunc->nodes.clear();
			calllocal->Remove();
			calllocal = kcall;
			delete nop;
		}

		TamguGetFunctionThrough* ag = new TamguGetFunctionThrough(calllocal, global, kf);
		for (int i = 1; i < xn->nodes.size(); i++)
			Traverse(xn->nodes[i], ag);
		return ag;
	}

	x_node* nop = new x_node("", "", xn);

	if (xn->nodes[0]->token == "operator") {
		vector<x_node*> nodes;
		nodes.push_back(nop);
		nop->token = "expressions";
		nop->nodes.push_back(xn->nodes[1]);
		x_node* noper;
		x_node* prev = nop;
		int i;
		for (i = 2; i < xn->nodes.size(); i++) {
			noper = new x_node("", "", xn);
			nodes.push_back(noper);
			if (prev == nop) {
				noper->token = "operation";
				prev->nodes.push_back(noper);
			}
			else {
				noper->token = "expressions";
				noper->nodes.push_back(prev->nodes[1]);
				prev->nodes[1] = noper;
				noper->nodes.push_back(new x_node("", "", xn));
				noper = noper->nodes.back();
				nodes.push_back(noper);
				noper->token = "operation";
			}
			noper->nodes.push_back(xn->nodes[0]);
			noper->nodes.push_back(xn->nodes[i]);
			prev = noper;
		}
		Tamgu* kcall = Traverse(nop, kf);
		for (i = 0; i < nodes.size(); i++) {
			nodes[i]->nodes.clear();
			delete nodes[i];
		}
		return kcall;

	}
	
	nop->token = "taskellcall";
	x_node* nfunc = creationxnode("functioncall", xn->nodes[0]->value, nop);
	nfunc->nodes.push_back(xn->nodes[0]);

	x_node* param = NULL;

	//The parameters is a recursive structure...
	for (int i = 1; i < xn->nodes.size(); i++) {
		if (param == NULL)
			param = creationxnode("parameters", "", nop);
		param->nodes.push_back(xn->nodes[i]);
	}

	Tamgu* kcall = Traverse(nop, kf);

	//We check if it is a data structure, for which we have a variable description...
	// <data TOTO :: TOTO Truc Float> for example...
	short idname = global->Getid(xn->nodes[0]->value);
	if (global->frames.check(idname)) {
		TamguFrame* frame = global->frames[idname];
		if (frame->variables.size() != param->nodes.size()) {
			stringstream message;
			message << "The number of parameters does not match the data structure definition";
			throw new TamguRaiseError(message, filename, current_start, current_end);
		}
		//Now for each field in the fname, we need to check if it compatible with the function parameters...
		short ftype, ptype;
		for (int i = 0; i < frame->variables.size(); i++) {
			ftype = frame->variables[i]->Typevariable();
			ptype = kcall->Argument(i)->Typeinfered();
			if (ptype != a_none && !global->Compatiblestrict(ptype, ftype)) {
				stringstream message;
				message << "Type mismatch... Expected: '" << global->Getsymbol(ftype) << "' Proposed: '" << global->Getsymbol(ptype) << "'";
				throw new TamguRaiseError(message, filename, current_start, current_end);
			}
		}
	}
	nfunc->nodes.clear();
	if (param != NULL)
		param->nodes.clear();
	delete nop;
	return kcall;
}

x_node* Composecalls(x_node* nop, x_node* xn) {
	x_node* nfunc = NULL;
	for (long i = 0; i < xn->nodes.size(); i++) {
		if (nfunc != NULL) {			
			x_node* param = creationxnode("parameters", "", nop);
			nop = creationxnode("taskellcall", "", param);
		}		
		if (xn->nodes[i]->token == "power")
			nfunc = creationxnode("power", xn->nodes[i]->value, nop);
		else
			nfunc = creationxnode("functioncall", xn->nodes[i]->value, nop);
		nfunc->nodes.push_back(xn->nodes[i]);
	}

	xn->nodes.clear();
	return nop;
}

Tamgu* TamguCode::C_mapping(x_node* xn, Tamgu* kbase) {
	TamguCallFunctionTaskell* kf;
	if (kbase->Type() == a_calltaskell)
		kf = (TamguCallFunctionTaskell*)kbase;
	else {
		kf = new TamguCallFunctionTaskell(global, kbase);
		kf->Init(NULL);
	}
	TamguFunctionLambda* kfunc = kf->body;
	TamguLambdaDomain* lambdadom = kfunc->lambdadomain;

	x_node nvar("variable", "&common;", xn);
	x_node* nname = creationxnode("word", nvar.value, &nvar);

	global->Pushstack(kfunc);
	global->Pushstack(lambdadom);

	Tamgu* kret;
	//We parse our "list" value
	kf->taskellchoice = 3;
	Traverse(xn->nodes[1], kf);
	//We are dealing with a simple return function
	//The value is stored in a variable, which is one step before the return statement
	//We store the calculus in a intermediary variable, whose name is &return; and which is created
	//with C_ParseReturnValue function...
	if (lambdadom->instructions.size() == 0 && kfunc->instructions.size() == 1) {
		kret = kfunc->instructions.back()->Argument(0);
		if (kret->Type() == a_calltaskell) {
			global->Popstack();
			global->Popstack();
			kf = (TamguCallFunctionTaskell*)kret;
			kfunc = kf->body;
			kfunc->choice = 1;
			lambdadom = kfunc->lambdadomain;
			global->Pushstack(kfunc);
			global->Pushstack(lambdadom);
		}
	}

	//No composition
	if (lambdadom->instructions.size() == 1) {
		kret = lambdadom->instructions[0];
		TamguCallFunctionTaskell* kinit = NULL;
		if (kret->Initialisation()->Instruction(0)->Type() == a_calltaskell) {
			kinit = (TamguCallFunctionTaskell*)kret->Initialisation()->Instruction(0);
			//we look for the most embedded call...
			if (kinit->body->lambdadomain->instructions.size() == 0 && kinit->body->Instruction(0)->Argument(0)->Type() == a_calltaskell)
				kinit = (TamguCallFunctionTaskell*)kinit->body->Instruction(0)->Argument(0);
		}

		if (kinit != NULL && kinit->body->lambdadomain->instructions.size() != 0) {
			//First we copy all our substructures into our main structure...
			kf->body->lambdadomain->instructions = kinit->body->lambdadomain->instructions;
			kf->body->lambdadomain->declarations = kinit->body->lambdadomain->declarations;
			kf->body->lambdadomain->local = kinit->body->lambdadomain->local;
			kf->body->parameters = kinit->body->parameters;
			kf->body->declarations = kinit->body->declarations;
			kf->body->instructions = kinit->body->instructions;
			//kf->declarations = kinit->declarations;
			char adding = 0;
			if (kf != kbase) {
				adding = 2;
				kbase->AddInstruction(kf);
			}
			//if adding is two, then the return section in kinit will be duplicated and left intact...
			kret = C_ParseReturnValue(xn, kfunc, adding);
			if (kret != NULL) {
				nvar.value = "&return;";
				nname->value = "&return;";
			}
		}
		else {
			lambdadom->instructions.clear();
			//we need first to create a variable...
			BrowseVariable(&nvar, lambdadom);
			lambdadom->AddInstruction(kret);
			lambdadom->AddInstruction(aFALSE);

			kret = C_ParseReturnValue(xn, kfunc, 1);

			//This variable becomes our current variable...
			if (kf != kbase)
				kbase->AddInstruction(kf);
		}
	}
	else {
		kret = C_ParseReturnValue(xn, kfunc, 1);
		if (kret == NULL) {
			//Then this is a filter..., A if is detected as a last element in the function instructions set...
			//We create a new TamguCallFunctionTaskell, in which we copy all that was computed with xn->nodes[1]
			TamguCallFunctionTaskell* kcf = new TamguCallFunctionTaskell(global);
			kcf->Init(NULL);
			kcf->body->lambdadomain->instructions = lambdadom->instructions;
			kcf->body->lambdadomain->declarations = lambdadom->declarations;
			kcf->body->lambdadomain->local = lambdadom->local;
			kcf->body->declarations = kfunc->declarations;
			kcf->body->instructions = kfunc->instructions;

			//We clear our current structure, in which we create a CALL to that filter...
			//kcf->declarations = kf->declarations;
			lambdadom->instructions.clear();
			lambdadom->declarations.clear();
			lambdadom->local = false;
			kfunc->instructions.clear();
			kfunc->declarations.clear();

			//The second element, which is the list on which we compute, is then stored
			//in our structure...
			BrowseVariable(&nvar, lambdadom);
			lambdadom->AddInstruction(kcf);
			lambdadom->AddInstruction(aFALSE);

			kret = C_ParseReturnValue(xn, kfunc, 1);
		}
		else {
			nvar.value = "&return;";
			nname->value = "&return;";
		}
	}

	//The next
	//The return value, with either an operation
	if (xn->nodes[0]->token == "inverted") {
		short op = global->string_operators[xn->nodes[0]->nodes[1]->value];
		if (op >= a_plus && op <= a_add) {
			TamguInstructionAPPLYOPERATION kins(NULL);
			kins.action = op;
			Traverse(xn->nodes[0]->nodes[0], &kins);
			Traverse(&nvar, &kins);
			Tamgu* kroot = kins.Compile(NULL);
			kret->AddInstruction(kroot);
		}
		else {
			TamguInstruction* ki = TamguCreateInstruction(kret, op);
			Traverse(xn->nodes[0]->nodes[0], ki);
			Traverse(&nvar, ki);
		}
	}
	else {
		if (xn->nodes[0]->token == "operator") {
			short op = global->string_operators[xn->nodes[0]->value];
			if (op >= a_plus && op <= a_add) {
				TamguInstructionAPPLYOPERATION kins(NULL);
				kins.action = op;
				Traverse(&nvar, &kins);
				Traverse(&nvar, &kins);
				Tamgu* kroot = kins.Compile(NULL);
				kret->AddInstruction(kroot);
			}
			else {
				TamguInstruction* ki = TamguCreateInstruction(kret, op);
				Traverse(&nvar, ki);
				Traverse(&nvar, ki);
			}
		}
		else {
			if (xn->nodes[0]->token == "operation") {
				TamguInstruction* ki = TamguCreateInstruction(NULL, a_bloc);
				Traverse(&nvar, ki);
				Traverse(xn->nodes[0], ki);

				if (ki->action == a_bloc && ki->instructions.size() == 1) {
					Tamgu* kroot = ki->instructions[0]->Compile(NULL);
					kret->AddInstruction(kroot);
					if (kroot != ki->instructions[0])
						ki->instructions[0]->Remove();
					ki->Remove();
				}
				else
					kret->AddInstruction(CloningInstruction(ki));
			}
			else {
				if (xn->nodes[0]->token == "hlambda") {
					Tamgu* kprevious = C_hlambda(xn->nodes[0], kfunc);
					TamguCallFunctionArgsTaskell* kcall = new TamguCallFunctionArgsTaskell((TamguFunctionLambda*)kprevious, global, kret);
					Traverse(&nvar, (Tamgu*)kcall);
				}
				else { // a function call
					x_node* nop = new x_node("taskellcall", "", xn);					
					x_node* nfunc = Composecalls(nop, xn->nodes[0]->nodes[0]);

					x_node* param = creationxnode("parameters", "", nfunc);				

					for (int i = 1; i < xn->nodes[0]->nodes.size(); i++)
						param->nodes.push_back(xn->nodes[0]->nodes[i]);

					param->nodes.push_back(&nvar);					

					Traverse(nop, kret);
					
					param->nodes.clear();
					delete nop;
				}
			}
		}
	}

	global->Popstack();
	global->Popstack();
	return kfunc;
}


Tamgu* TamguCode::C_taskellalltrue(x_node* xn, Tamgu* kbase) {
	TamguCallFunctionTaskell* kf;
	if (kbase->Type() == a_calltaskell)
		kf = (TamguCallFunctionTaskell*)kbase;
	else {
		kf = new TamguCallFunctionTaskell(global, kbase);
		kf->Init(NULL);
	}
	TamguFunctionLambda* kfunc = kf->body;
	TamguLambdaDomain* lambdadom = kfunc->lambdadomain;
	global->Pushstack(kfunc);
	global->Pushstack(lambdadom);

	char buff[50];
	sprintf_s(buff, 50, "&%s;", xn->nodes[0]->value.c_str());
	//we need first to create a variable...
	x_node nvar("variable", buff, xn);
	creationxnode("word", nvar.value, &nvar);

	BrowseVariable(&nvar, lambdadom);
	Traverse(xn->nodes[2], lambdadom);

	kfunc->choice = 1;
	TamguInstructionTaskellIF* ktest = new TamguInstructionTaskellIF(global, kfunc);

	if (xn->nodes[1]->token == "hcomparison" || xn->nodes[1]->token == "taskcomparison" || xn->nodes[1]->token == "operationin") {
		TamguInstruction* ki = TamguCreateInstruction(NULL, a_bloc);
		Traverse(&nvar, ki); // we add our variable to compare with
		Traverse(xn->nodes[1], ki); // we add our comparison operator with its value...

		if (ki->action == a_bloc && ki->instructions.size() == 1) {
			Tamgu* kroot = ki->instructions[0]->Compile(NULL);
			ktest->AddInstruction(kroot);
			if (kroot != ki->instructions[0])
				ki->instructions[0]->Remove();
			ki->Remove();
		}
		else {
            ki->Setpartialtest(true);
			ktest->AddInstruction(CloningInstruction(ki));
		}
	}
	else {
		if (xn->nodes[1]->token == "hboollambda") {
			Tamgu* kprevious = C_hlambda(xn->nodes[1], kfunc);
			TamguCallFunction* kcall = CreateCallFunction(kprevious, ktest);
			Traverse(&nvar, kcall);
		}
		else {
			x_node* nop = new x_node("taskellcall", "", xn);
			x_node* nfunc = Composecalls(nop, xn->nodes[1]->nodes[0]);

			x_node* param = creationxnode("parameters", "", nfunc);

			for (int i = 1; i < xn->nodes[1]->nodes.size(); i++)
				param->nodes.push_back(xn->nodes[1]->nodes[i]);

			param->nodes.push_back(&nvar);
			
			Traverse(nop, ktest);

			param->nodes.clear();

			delete nop;
		}
	}

    //Whenever a value is not true, we break
    if (xn->nodes[0]->value == "all") {
        ktest->AddInstruction(aNULL);
        ktest->AddInstruction(aBREAKFALSE);
    }
    else {
        ktest->AddInstruction(aBREAKTRUE);
        lambdadom->name = 2; //the name 2 corresponds to a ANY
    }
    
	global->Popstack();
	global->Popstack();
	return kfunc;

}

Tamgu* TamguCode::C_taskellboolchecking(x_node* xn, Tamgu* kbase) {
    TamguCallFunctionTaskell* kf;
    if (kbase->Type() == a_calltaskell)
        kf = (TamguCallFunctionTaskell*)kbase;
    else {
        kf = new TamguCallFunctionTaskell(global, kbase);
        kf->Init(NULL);
    }
    TamguFunctionLambda* kfunc = kf->body;
    TamguLambdaDomain* lambdadom = kfunc->lambdadomain;
    global->Pushstack(kfunc);
    global->Pushstack(lambdadom);
    
    char buff[50];
    sprintf_s(buff, 50, "&%s;", xn->nodes[0]->value.c_str());
    //we need first to create a variable...
    x_node nvar("variable", buff, xn);
    creationxnode("word", nvar.value, &nvar);
    
    BrowseVariable(&nvar, lambdadom);
    Traverse(xn->nodes[1], lambdadom);
    
    kfunc->choice = 1;
    TamguInstructionTaskellIF* ktest = new TamguInstructionTaskellIF(global, kfunc);
    Traverse(&nvar, ktest); // we add our variable to compare with
    
    if (xn->nodes[0]->value == "and") {
        //Whenever a value is not true, we break
        ktest->AddInstruction(aNULL);
        ktest->AddInstruction(aBREAKFALSE);
    }
    else {
        if (xn->nodes[0]->value == "or") {
            ktest->AddInstruction(aBREAKTRUE);
            lambdadom->name = 2; //the name 2 corresponds to a OR
        }
        else {
            ktest->AddInstruction(aBREAKONE);
            ktest->AddInstruction(aBREAKZERO);
            lambdadom->name = 3; //the name 3 corresponds to a XOR
        }
    }
    
    global->Popstack();
    global->Popstack();
    return kfunc;
    
}
    

Tamgu* TamguCode::C_folding(x_node* xn, Tamgu* kbase) {
	TamguCallFunctionTaskell* kf;
	if (kbase->Type() == a_calltaskell)
		kf = (TamguCallFunctionTaskell*)kbase;
	else {
		kf = new TamguCallFunctionTaskell(global, kbase);
		kf->Init(NULL);
	}
	TamguFunctionLambda* kfunc = kf->body;
	TamguLambdaDomain* lambdadom = kfunc->lambdadomain;
	global->Pushstack(kfunc);
	global->Pushstack(lambdadom);

	char buff[50];
	sprintf_s(buff, 50, "&%s;", xn->nodes[0]->value.c_str());
	//we need first to create a variable...
	x_node nvar("variable", buff, xn);
	creationxnode("word", nvar.value, &nvar);

	//The &folding; <- expressions part...
	BrowseVariable(&nvar, lambdadom);
	if (xn->token == "folding")
		Traverse(xn->nodes[3], lambdadom);
	else
		Traverse(xn->nodes[2], lambdadom);

	//The initial value for the lambda function
	x_node accuvar("variable", "&iaccu;", xn);
	creationxnode("word", accuvar.value, &accuvar);

	BrowseVariable(&accuvar, lambdadom);
	if (xn->token == "folding")
		Traverse(xn->nodes[2], lambdadom);
	else
		lambdadom->AddInstruction(aNOELEMENT);

	//The iterator direction
	Tamgu* choice;
	string act = xn->nodes[0]->nodes[0]->token;
	if (act[0] == 'l') {
		if (act[1] == 's')
			choice = aZERO; //scanning from left to right
		else
			choice = aFALSE; //folding from left to right
	}
	else {
		if (act[1] == 's')
			choice = aONE; //scanning right to left
		else
			choice = aTRUE; //folding right to left
	}

	lambdadom->AddInstruction(choice);

	kfunc->choice = 1;
	Tamgu* kret = C_ParseReturnValue(xn, kfunc);

	if (xn->nodes[1]->token == "operator") {
		short op = global->string_operators[xn->nodes[1]->value];
		if (op >= a_plus && op <= a_add) {
			TamguInstructionAPPLYOPERATION kins(NULL);
			kins.action = op;
            if (choice->Boolean()) {
                Traverse(&nvar, &kins);
                Traverse(&accuvar, &kins);
            }
            else {
                Traverse(&accuvar, &kins);
                Traverse(&nvar, &kins);
            }
			Tamgu* kroot = kins.Compile(NULL);
			kret->AddInstruction(kroot);
		}
		else {
			TamguInstruction* ki = TamguCreateInstruction(kret, op);
            if (choice->Boolean()) {
                Traverse(&nvar, ki);
                Traverse(&accuvar, ki);
            }
            else {
                Traverse(&accuvar, ki);
                Traverse(&nvar, ki);
            }
		}
	}
	else {
		TamguCallFunctionArgsTaskell* kcall;
		if (xn->nodes[1]->token == "hlambda") {
			Tamgu* kprevious = C_hlambda(xn->nodes[1], kfunc);
            kcall = new TamguCallFunctionArgsTaskell((TamguFunctionLambda*)kprevious, global, kret);
			if (choice->Boolean() == false) {
				Traverse(&accuvar, kcall);
				Traverse(&nvar, kcall);
			}
			else {
				Traverse(&nvar, kcall);
				Traverse(&accuvar, kcall);
			}
		}
		else {
			x_node* nop = new x_node("taskellcall", "", xn);
			x_node* nfunc = Composecalls(nop, xn->nodes[1]->nodes[0]);

			x_node* param = creationxnode("parameters", "", nfunc);
			
			for (int i = 1; i < xn->nodes[1]->nodes.size(); i++)
				param->nodes.push_back(xn->nodes[1]->nodes[i]);

			if (choice->Boolean() == true) {
				param->nodes.push_back(&nvar);
				param->nodes.push_back(&accuvar);
			}
			else {
				param->nodes.push_back(&accuvar);
				param->nodes.push_back(&nvar);
			}
			

			Traverse(nop, kret);

			param->nodes.clear();
			delete nop;
		}
	}

	global->Popstack();
	global->Popstack();
	return kfunc;

}



Tamgu* TamguCode::C_zipping(x_node* xn, Tamgu* kbase) {
	TamguCallFunctionTaskell* kf;
	if (kbase->Type() == a_calltaskell) {
		kf = (TamguCallFunctionTaskell*)kbase;
	}
	else {
		kf = new TamguCallFunctionTaskell(global);
		kf->Init(NULL);
	}
	TamguFunctionLambda* kfunc = kf->body;
	TamguLambdaDomain* lambdadom = kfunc->lambdadomain;
	global->Pushstack(kfunc);
	global->Pushstack(lambdadom);

	vector<x_node*> nvars;
	char buff[50];
	int first = 1;
	int i;
	if (xn->token == "pairing")
		first = 0;

	for (i = first; i < xn->nodes.size(); i++) {
		//we need first to create a variable...
		sprintf_s(buff, 50, "&zipping%d", i);
		x_node* nvar = new x_node("variable", buff, xn);
		creationxnode("word", nvar->value, nvar);

		BrowseVariable(nvar, lambdadom);
		Traverse(xn->nodes[i], lambdadom);
		if (i == first)
			lambdadom->AddInstruction(aFALSE);
		else
			lambdadom->AddInstruction(aZERO);
		nvars.push_back(nvar);
	}

	kfunc->choice = 1;
	Tamgu* kret = C_ParseReturnValue(xn, kfunc);

	if (!first) {
		TamguConstvector* kvect = new TamguConstvector(global, kret);
		kvect->evaluate = true;
		for (i = 0; i < nvars.size(); i++) {
			Traverse(nvars[i], kvect);
			delete nvars[i];
		}
	}
	else {
		if (xn->nodes[0]->token == "operator") {
			short op = global->string_operators[xn->nodes[0]->value];
			if (op >= a_plus && op <= a_add) {
				TamguInstructionAPPLYOPERATION kins(NULL);				
				kins.action = op;
				for (i = 0; i < nvars.size(); i++) {
					Traverse(nvars[i], &kins);
					delete nvars[i];
				}
				Tamgu* kroot = kins.Compile(NULL);
				kret->AddInstruction(kroot);
			}
			else {
				TamguInstruction* ki = TamguCreateInstruction(NULL, op);
				for (i = 0; i < nvars.size(); i++) {
					Traverse(nvars[i], ki);
					delete nvars[i];
				}
				kret->AddInstruction(ki);
			}
		}
		else {
			if (xn->nodes[0]->token == "hlambda") {
                Tamgu* kprevious = C_hlambda(xn->nodes[0], kfunc);
                TamguCallFunctionArgsTaskell* kcall = new TamguCallFunctionArgsTaskell((TamguFunctionLambda*)kprevious, global, kret);
				for (i = 0; i < nvars.size(); i++) {
					Traverse(nvars[i], kcall);
					delete nvars[i];
				}
			}
			else {
				x_node* nop = new x_node("taskellcall", "", xn);
				x_node* nfunc = Composecalls(nop, xn->nodes[0]->nodes[0]);

				x_node* param = creationxnode("parameters", "", nfunc);
				
				for (i = 1; i < xn->nodes[0]->nodes.size(); i++)
					param->nodes.push_back(xn->nodes[0]->nodes[i]);

				for (i = 0; i < nvars.size(); i++)
					param->nodes.push_back(nvars[i]);				

				Traverse(nop, kret);

				param->nodes.clear();

				delete nop;
			}
		}
	}
	global->Popstack();
	global->Popstack();
	return kfunc;
}

Tamgu* TamguCode::C_filtering(x_node* xn, Tamgu* kbase) {
	TamguCallFunctionTaskell* kf;
	TamguFunctionLambda* kfunc;
	TamguLambdaDomain* lambdadom;
	if (kbase->Type() == a_calltaskell) {
		kf = (TamguCallFunctionTaskell*)kbase;
	}
	else {
		kf = new TamguCallFunctionTaskell(global);
		kf->Init(NULL);
	}

	Tamgu* kret = NULL;
	kfunc = kf->body;
	kfunc->choice = 1;
	lambdadom = kfunc->lambdadomain;

	x_node nvar("variable", "&common;", xn);
	x_node* nname = creationxnode("word", nvar.value, &nvar);

	global->Pushstack(kfunc);
	global->Pushstack(lambdadom);

	kf->taskellchoice = 3;
	Traverse(xn->nodes[2], kf);
	if (lambdadom->instructions.size() == 0 && kfunc->instructions.size() == 1) {
		kret = kfunc->instructions.back()->Argument(0);
		if (kret->Type() == a_calltaskell) {
			global->Popstack(); //A call to a Taskell function
			global->Popstack();
			kf = (TamguCallFunctionTaskell*)kret;
			kfunc = kf->body;
			kfunc->choice = 1;
			lambdadom = kfunc->lambdadomain;
			global->Pushstack(kfunc);
			global->Pushstack(lambdadom);
		}
	}

	if (lambdadom->instructions.size() == 1) {
		kret = lambdadom->instructions[0];
		TamguCallFunctionTaskell* kinit = NULL;
		if (kret->Initialisation()->Instruction(0)->Type() == a_calltaskell) {
			kinit = (TamguCallFunctionTaskell*)kret->Initialisation()->Instruction(0);
			//we look for the most embedded call...
			if (kinit->body->lambdadomain->instructions.size() == 0 && kinit->body->Instruction(0)->Argument(0)->Type() == a_calltaskell)
				kinit = (TamguCallFunctionTaskell*)kinit->body->Instruction(0)->Argument(0);
		}

		if (kinit != NULL && kinit->body->lambdadomain->instructions.size() != 0) {
			//First we copy all our substructures into our main structure...
			kf->body->lambdadomain->instructions = kinit->body->lambdadomain->instructions;
			kf->body->lambdadomain->declarations = kinit->body->lambdadomain->declarations;
			kf->body->lambdadomain->local = kinit->body->lambdadomain->local;
			kf->body->parameters = kinit->body->parameters;
			kf->body->declarations = kinit->body->declarations;
			kf->body->instructions = kinit->body->instructions;
			//kf->declarations = kinit->declarations;
			char adding = 0;
			if (kf != kbase) {
				adding = 2;
				kbase->AddInstruction(kf);
			}
			//if adding is two, then the return section in kinit will be duplicated and left intact...
			kret = C_ParseReturnValue(xn, kfunc, adding);
			if (kret != NULL) {
				nvar.value = "&return;";
				nname->value = "&return;";
			}
		}
		else {
			lambdadom->instructions.clear();
			//we need first to create a variable...
			BrowseVariable(&nvar, lambdadom);
			lambdadom->AddInstruction(kret);
			lambdadom->AddInstruction(aFALSE);
			//This variable becomes our current variable...

			kret = C_ParseReturnValue(xn, kfunc);
			Traverse(&nvar, kret);
			if (kf != kbase)
				kbase->AddInstruction(kf);
		}
	}
	else {
		C_ParseReturnValue(xn, kfunc);
		nvar.value = "&return;";
		nname->value = "&return;";
	}

	x_node nvardrop("variable", "&drop;", xn);

	if (xn->nodes[0]->value == "dropWhile") {
		creationxnode("word", nvardrop.value, &nvardrop);
				
		TamguVariableDeclaration* var = new TamguTaskellVariableDeclaration(global, a_drop, a_boolean, false, false, NULL);
		var->initialization = aTRUE;
		lambdadom->Declare(a_drop, var);
		lambdadom->local = true;
	}

	//We remove the last instruction, to insert it into our test
	kret = kfunc->instructions.back();
	kfunc->instructions.pop_back();
	TamguInstructionTaskellIF* ktest;

	//The return statement should be removed and replaced
	TamguInstruction* ki;
	if (xn->nodes[0]->value == "take" || xn->nodes[0]->value == "drop") {//In that case, we need to count the number of elements that were used so far...
		//First we need to declare a variable which will be used as a counter...
		nvar.value = "&counter;"; //Our counter
		nname->value = "&counter;";
		Tamgu* var = new TamguTaskellVariableDeclaration(global, a_counter, a_long, false, false, NULL);
		lambdadom->Declare(a_counter, var);
		var->AddInstruction(aZERO);
		lambdadom->local = true;
		//We add a PLUSPLUS to increment our value...
		var = new TamguCallVariable(a_counter, a_long, global, kfunc);
		var = new TamguPLUSPLUS(global, var);
		//Then we need to add our test
		ktest = new TamguInstructionTaskellIF(global, kfunc);
		
		if (xn->nodes[0]->value == "drop")
			ki = TamguCreateInstruction(ktest, a_more);
		else
			ki = TamguCreateInstruction(ktest, a_lessequal);
		Traverse(&nvar, ki);
		Traverse(xn->nodes[1], ki);
	}
	else {
		ktest = new TamguInstructionTaskellIF(global, kfunc);
		//The only difference is that we process a Boolean expression in a filter
		if (xn->nodes[1]->token == "hcomparison" || xn->nodes[1]->token == "taskcomparison" || xn->nodes[1]->token == "operationin") {
			ki = TamguCreateInstruction(NULL, a_bloc);
			Traverse(&nvar, ki); // we add our variable to compare with
			Traverse(xn->nodes[1], ki); // we add our comparison operator with its value...

			if (ki->action == a_bloc && ki->instructions.size() == 1) {
				Tamgu* kroot = ki->instructions[0]->Compile(NULL);
				ktest->AddInstruction(kroot);
				if (kroot != ki->instructions[0])
					ki->instructions[0]->Remove();
				ki->Remove();
			}
			else {
                ki->Setpartialtest(true);
				ktest->AddInstruction(CloningInstruction(ki));
			}
		}
		else {
			if (xn->nodes[1]->token == "hboollambda") {
				Tamgu* kprevious = C_hlambda(xn->nodes[1], kfunc);
				TamguCallFunction* kcall = CreateCallFunction((TamguFunction*)kprevious, ktest);
				Traverse(&nvar, kcall);
			}
			else {
				x_node* nop = new x_node("taskellcall", "", xn);
				x_node* nfunc = Composecalls(nop, xn->nodes[1]->nodes[0]);

				x_node* param = creationxnode("parameters", "", nfunc);
				
				for (int i = 1; i < xn->nodes[1]->nodes.size(); i++)
					param->nodes.push_back(xn->nodes[1]->nodes[i]);

				param->nodes.push_back(&nvar);
				
				Traverse(nop, ktest);

				param->nodes.clear();
				delete nop;
			}
		}
	}

	if (xn->nodes[0]->value == "dropWhile") {
		//Then, in that case, when the test is positive, we return aNULL else the value
		//First we modify the test in ktest...
		//We need to use a Boolean (&drop;) which will be set to false, when the test will be true...
		ki = TamguCreateInstruction(NULL, a_booleanand);
		Traverse(&nvardrop, ki);
		ki->AddInstruction(ktest->instructions[0]);
		ktest->instructions.vecteur[0] = ki;
		TamguCallReturn* kretdrop = new TamguCallReturn(global, ktest);
		kretdrop->AddInstruction(aNULL);
		//We need now a sequence of instructions
		TamguSequence* kseq = new TamguSequence(global, ktest);
		//the variable &drop; is set to false
		ki = TamguCreateInstruction(kseq, a_assignement);
		Traverse(&nvardrop, ki);
		ki->Instruction(0)->Setaffectation(true);
		ki->AddInstruction(aFALSE);
		//we add our return value...
		kseq->AddInstruction(kret);
	}
	else {
		ktest->AddInstruction(kret); //We add the value to return if test is positive
		if (xn->nodes[0]->value != "filter" && xn->nodes[0]->value != "drop")
			kret = new TamguBreak(global, ktest);
	}
	global->Popstack();
	global->Popstack();
	return kfunc;
}


Tamgu* TamguCode::C_flipping(x_node* xn, Tamgu* kbase) {
	TamguCallFunctionTaskell* kf;
	if (kbase->Type() == a_calltaskell) {
		kf = (TamguCallFunctionTaskell*)kbase;
	}
	else {
		kf = new TamguCallFunctionTaskell(global);
		kf->Init(NULL);
	}
	TamguFunctionLambda* kfunc = kf->body;
	TamguLambdaDomain* lambdadom = kfunc->lambdadomain;
	global->Pushstack(kfunc);
	global->Pushstack(lambdadom);

	int i;
	TamguInstruction kvar;


	kfunc->choice = 1;
	Tamgu* kret = C_ParseReturnValue(xn, kfunc);

	if (xn->nodes[0]->token == "operator") {
		short op = global->string_operators[xn->nodes[0]->value];
		if (op >= a_plus && op <= a_add) {
			TamguInstructionAPPLYOPERATION kins(NULL);
			kins.action = op;
			Traverse(xn->nodes[2], &kins);
			Traverse(xn->nodes[1], &kins);
			Tamgu* kroot = kins.Compile(NULL);
			kret->AddInstruction(kroot);
		}
		else {
			TamguInstruction* ki = TamguCreateInstruction(NULL, op);
			Traverse(xn->nodes[2], ki);
			Traverse(xn->nodes[1], ki);
			kret->AddInstruction(ki);
		}
	}
	else {
		if (xn->nodes[0]->token == "hlambda") {
            Tamgu* kprevious = C_hlambda(xn->nodes[0], kfunc);
            TamguCallFunctionArgsTaskell* kcall = new TamguCallFunctionArgsTaskell((TamguFunctionLambda*)kprevious, global, kret);
			Traverse(xn->nodes[2], kcall);
			Traverse(xn->nodes[1], kcall);
		}
		else {
			x_node* nop = new x_node("taskellcall", "", xn);
			x_node* nfunc = Composecalls(nop, xn->nodes[0]->nodes[0]);

			x_node* param = NULL;

			for (i = 2; i >= 1; i--) {
				if (param == NULL)
					param = creationxnode("parameters", "", nfunc);
				param->nodes.push_back(xn->nodes[i]);
			}

			Traverse(nop, kret);

			if (param != NULL)
				param->nodes.clear();
			
			delete nop;
		}
	}
	global->Popstack();
	global->Popstack();
	return kfunc;
}

Tamgu* TamguCode::C_cycling(x_node* xn, Tamgu* kbase) {//Cycling in a list...
	TamguCallFunctionTaskell* kf;
	if (kbase->Type() == a_calltaskell)
		kf = (TamguCallFunctionTaskell*)kbase;
	else {
		kf = new TamguCallFunctionTaskell(global, kbase);
		kf->Init(NULL);
	}
	TamguFunctionLambda* kfunc = kf->body;
	TamguLambdaDomain* lambdadom = kfunc->lambdadomain;

	x_node nvar("variable", "&common;", xn);
	x_node* nname = creationxnode("word", nvar.value, &nvar);

	global->Pushstack(kfunc);
	global->Pushstack(lambdadom);

	Tamgu* kret;
	//We parse our "list" value
	kf->taskellchoice = 3;
	Traverse(xn->nodes[1], kf);
	//We are dealing with a simple return function
	//The value is stored in a variable, which is one step before the return statement
	//We store the calculus in a intermediary variable, whose name is &return; and which is created
	//with C_ParseReturnValue function...
	if (lambdadom->instructions.size() == 0 && kfunc->instructions.size() == 1) {
		kret = kfunc->instructions.back()->Argument(0);
		if (kret->Type() == a_calltaskell) {
			global->Popstack();
			global->Popstack();
			kf = (TamguCallFunctionTaskell*)kret;
			kfunc = kf->body;
			kfunc->choice = 1;
			lambdadom = kfunc->lambdadomain;
			global->Pushstack(kfunc);
			global->Pushstack(lambdadom);
		}
	}

	//No composition
	if (lambdadom->instructions.size() == 1) {
		kret = lambdadom->instructions[0];
		TamguCallFunctionTaskell* kinit = NULL;
		if (kret->Initialisation()->Instruction(0)->Type() == a_calltaskell) {
			kinit = (TamguCallFunctionTaskell*)kret->Initialisation()->Instruction(0);
			//we look for the most embedded call...
			if (kinit->body->lambdadomain->instructions.size() == 0 && kinit->body->Instruction(0)->Argument(0)->Type() == a_calltaskell)
				kinit = (TamguCallFunctionTaskell*)kinit->body->Instruction(0)->Argument(0);
		}

		if (kinit != NULL && kinit->body->lambdadomain->instructions.size() != 0) {
			//First we copy all our substructures into our main structure...
			kf->body->lambdadomain->instructions = kinit->body->lambdadomain->instructions;
			kf->body->lambdadomain->declarations = kinit->body->lambdadomain->declarations;
			kf->body->lambdadomain->local = kinit->body->lambdadomain->local;
			kf->body->parameters = kinit->body->parameters;
			kf->body->declarations = kinit->body->declarations;
			kf->body->instructions = kinit->body->instructions;
			//kf->declarations = kinit->declarations;
			char adding = 0;
			if (kf != kbase) {
				adding = 2;
				kbase->AddInstruction(kf);
			}
			//if adding is two, then the return section in kinit will be duplicated and left intact...
			kret = C_ParseReturnValue(xn, kfunc, adding);
			if (kret != NULL) {
				nvar.value = "&return;";
				nname->value = "&return;";
			}
		}
		else {
			lambdadom->instructions.clear();
			//we need first to create a variable...
			BrowseVariable(&nvar, lambdadom);
			lambdadom->AddInstruction(kret);
			lambdadom->AddInstruction(aFALSE);

			kret = C_ParseReturnValue(xn, kfunc, 1);

			//This variable becomes our current variable...
			if (kf != kbase)
				kbase->AddInstruction(kf);
		}
	}
	else {
		kret = C_ParseReturnValue(xn, kfunc, 1);
		if (kret == NULL) {
			//Then this is a filter..., A if is detected as a last element in the function instructions set...
			//We create a new TamguCallFunctionTaskell, in which we copy all that was computed with xn->nodes[1]
			TamguCallFunctionTaskell* kcf = new TamguCallFunctionTaskell(global);
			kcf->Init(NULL);
			kcf->body->lambdadomain->instructions = lambdadom->instructions;
			kcf->body->lambdadomain->declarations = lambdadom->declarations;
			kcf->body->lambdadomain->local = lambdadom->local;
			kcf->body->declarations = kfunc->declarations;
			kcf->body->instructions = kfunc->instructions;

			//We clear our current structure, in which we create a CALL to that filter...
			//kcf->declarations = kf->declarations;
			lambdadom->instructions.clear();
			lambdadom->declarations.clear();
			lambdadom->local = false;
			kfunc->instructions.clear();
			kfunc->declarations.clear();

			//The second element, which is the list on which we compute, is then stored
			//in our structure...
			BrowseVariable(&nvar, lambdadom);
			lambdadom->AddInstruction(kcf);
			lambdadom->AddInstruction(aFALSE);

			kret = C_ParseReturnValue(xn, kfunc, 1);
		}
		else {
			nvar.value = "&return;";
			nname->value = "&return;";
		}
	}

	//Now we modify lambdadom a little bit...
	if (xn->nodes[0]->value == "repeat")
		lambdadom->instructions.vecteur[1] = new TamguCycleVector(lambdadom->instructions[1], true, global);
	else
	if (xn->nodes[0]->value == "cycle")
		lambdadom->instructions.vecteur[1] = new TamguCycleVector(lambdadom->instructions[1], false, global);
	else {
		lambdadom->instructions.vecteur[1] = new TamguReplicateVector(lambdadom->instructions[1], global);
		Traverse(xn->nodes[0], lambdadom->instructions[1]);
	}

	//we simply return our value...
	Traverse(&nvar, kret);
	global->Popstack();
	global->Popstack();
	return kfunc;
}



Tamgu* TamguCode::C_taskellexpression(x_node* xn, Tamgu* kf) {
	int id;
	Tamgu* var;
	if (xn->nodes[0]->token == "word") {
		id = global->Getid(xn->nodes[0]->value);
        if (id == a_universal)
            return aUNIVERSAL;
        
		if (kf->isDeclared(id)) {
			//In this case, we do not need to return an error
			//It is a case construction within a function...
			stringstream message;
			message << "Variable: '" << global->Getsymbol(id) << "' has already been declared";
			throw new TamguRaiseError(message, filename, current_start, current_end);
		}

		var = new TamguTaskellSelfVariableDeclaration(global, id);
		kf->Declare(id, var);
		return var;
	}
	else
		var = Traverse(xn->nodes[0], kf);
	return var;
}


Tamgu* TamguCode::C_taskellmap(x_node* xn, Tamgu* kf) {

    char ch = kf->Choice(); //Only the top Taskell Vector can be an argument

    TamguConstmap* kmap;
    
    if (ch == 10) //In this case, we do not want this map to be integrated as an instruction into the lambda...
        kmap = new TamguConstmap(global);
    else
        kmap = new TamguConstmap(global, kf);

    kf->Setchoice(10);
	Tamgu* var;
	for (int i = 0; i < xn->nodes.size(); i++) {
		if (xn->nodes[i]->token == "taskellmaptail") {
			kmap->keys.push_back(aPIPE);
			var = Traverse(xn->nodes[i]->nodes[0], kf);
			kmap->values.push_back(var);
			break;
		}
		var = Traverse(xn->nodes[i]->nodes[0], kf);
		kmap->keys.push_back(var);
		var = Traverse(xn->nodes[i]->nodes[1], kf);
		kmap->values.push_back(var);
	}
	kf->Setchoice(ch);
	return kmap;
}



Tamgu* TamguCode::C_taskellvector(x_node* xn, Tamgu* kf) {

    char ch = kf->Choice(); //Only the top Taskell Vector can be an argument
	
    TamguConstvector* kvect;
    
    if (ch == 10) //In this case, we do not want this vector to be an instruction in kf (which is a lambda)
        kvect = new TamguConstvector(global);
    else
        kvect = new TamguConstvector(global, kf);

    kf->Setchoice(10);
	Tamgu* var;
	for (int i = 0; i < xn->nodes.size(); i++) {
		if (xn->nodes[i]->token == "taskelltail") {
			TamguConstvectormerge* kbv = new TamguConstvectormerge(global, NULL);
			kvect->values.push_back(kbv);
			var = Traverse(xn->nodes[i]->nodes[0], kf);
			kbv->values.push_back(var);
			break;
		}
		var = Traverse(xn->nodes[i], kf);
		var->Setreference();
		kvect->values.push_back(var);
	}
	kf->Setchoice(ch);
	return kvect;
}



Tamgu* TamguCode::C_whereexpression(x_node* xn, Tamgu* kf) {
	TamguVariableDeclaration* var;
	int idname;
	TamguCallFunctionTaskell* kint = (TamguCallFunctionTaskell*)kf;
	kint->body->lambdadomain->local = true;
	for (int i = 0; i < xn->nodes.size(); i++) {
		if (xn->nodes[i]->token == "word") {
			idname = global->Getid(xn->nodes[i]->value);
			if (kint->body->lambdadomain->declarations.check(idname)) {
				stringstream message;
				message << "Variable: '" << xn->nodes[i]->value << "' already declared";
				throw new TamguRaiseError(message, filename, current_start, current_end);
			}

			var = new TamguTaskellSelfVariableDeclaration(global, idname);
			kint->body->lambdadomain->Declare(idname, var);
			kint->body->instructions.push_back(var);
            kint->body->wherevariables.push_back(var);
			TamguInstruction kres;
			Traverse(xn->nodes[i + 1], &kres);
			var->initialization = kres.instructions[0];
			var->Setaffectation(true);
			i++;
		}
		else
		if (xn->nodes[i]->token == "hbloc")
			Traverse(xn->nodes[i], kint->body);
		else
			Traverse(xn->nodes[i], kint);
	}
	return kf;
}



Tamgu* TamguCode::C_hinexpression(x_node* xn, Tamgu* kf) {
	TamguInstruction kbloc;

	//we must feed our return section with this value...
	Traverse(xn->nodes[0], &kbloc);
	kf->AddInstruction(kbloc.instructions[0]);
	if (xn->value == "notin")
		kf->Setnegation(true);
	return kf;
}


Tamgu* TamguCode::C_letmin(x_node* xn, Tamgu* kf) {
	//We do some makeup in order to transform a letmin into a let
	x_node* n = new x_node("letkeyword", "let", xn);
	xn->token = "let";
	xn->nodes.insert(xn->nodes.begin(), n);
	Traverse(xn, kf);//then we can compile it as if it was a known let structure...
	return kf;
}

Tamgu* TamguCode::C_taskellcase(x_node* xn, Tamgu* kf) {
	TamguInstructionTaskellMainCASE* ktest = NULL;
	TamguInstruction* thetest;
	TamguFunctionLambda* kfunc = NULL;
	TamguCallReturn* kret;

	string xname;
	for (int i = 1; i < xn->nodes.size(); i++) {
		if (xn->nodes[i]->token == "taskellcaseotherwise")
			break;
		
		xname = xn->nodes[i]->nodes[0]->token;
		if (xname == "valvectortail") {
			//In this case, we need to create a function, in which everything is going to get executed
			//We might need to declare variables, hence this function declaration...
			if (kfunc == NULL) {
				kfunc = new TamguFunctionLambda(TamguCASEFUNCTION, global);
				kf->AddInstruction(kfunc);
				kfunc->adding = true;
				global->Pushstack(kfunc);
			}
			BrowseVariableVector(xn->nodes[i]->nodes[0], kfunc);
		}

		if (xname == "valmaptail") {
			//In this case, we need to create a function, in which everything is going to get executed
			//We might need to declare variables, hence this function declaration...
			if (kfunc == NULL) {
				kfunc = new TamguFunctionLambda(TamguCASEFUNCTION, global);
				kf->AddInstruction(kfunc);
				kfunc->adding = true;
				global->Pushstack(kfunc);
			}
			BrowseVariableMap(xn->nodes[i]->nodes[0], kfunc);
		}
	}

	ktest = new TamguInstructionTaskellMainCASE(global);

	for (int i = 1; i < xn->nodes.size(); i++) {
		if (xn->nodes[i]->token == "taskellcaseotherwise") {
			ktest->other = 1;
			if (kfunc == NULL)
				Traverse(xn->nodes[i]->nodes[0], ktest); //then the value
			else {//In the case of a function we need to officially return the value...
				kret = new TamguCallReturn(global, ktest);
				thetest = TamguCreateInstruction(NULL, a_bloc);
				Traverse(xn->nodes[i]->nodes[0], thetest);

				if (thetest->action == a_bloc && thetest->instructions.size() == 1) {
					Tamgu* kroot = thetest->instructions[0]->Compile(NULL);
					kret->AddInstruction(kroot);
					if (kroot != thetest->instructions[0])
						thetest->instructions[0]->Remove();
					thetest->Remove();
				}
				else
					kret->AddInstruction(CloningInstruction(thetest));
			}
			break;
		}				
		//It should a be match between two values, it will be a match
		thetest = TamguCreateInstruction(ktest, a_match);
		Traverse(xn->nodes[0], thetest);
		Traverse(xn->nodes[i]->nodes[0], thetest); //the comparison
		if (kfunc == NULL)
			Traverse(xn->nodes[i]->nodes[1], ktest); //then the value
		else {//In the case of a function we need to officially return the value...
			kret = new TamguCallReturn(global, ktest);
			thetest = TamguCreateInstruction(NULL, a_bloc);
			Traverse(xn->nodes[i]->nodes[1], thetest);
			if (thetest->action == a_bloc && thetest->instructions.size() == 1) {
				Tamgu* kroot = thetest->instructions[0]->Compile(NULL);
				kret->AddInstruction(kroot);
				if (kroot != thetest->instructions[0])
					thetest->instructions[0]->Remove();
				thetest->Remove();
			}
			else
				kret->AddInstruction(CloningInstruction(thetest));
		}
	}

	if (kfunc == NULL) //No function, we can add our structure directly...
		kf->AddInstruction(ktest);
	else {
		//We then transform the declarations into instructions
		bin_hash<Tamgu*>::iterator it;
		for (it = kfunc->declarations.begin(); it != kfunc->declarations.end(); it++) {
			if (it->first != TamguCASEFUNCTION)
				kfunc->instructions.push_back(it->second);
		}
		kfunc->instructions.push_back(ktest);
		global->Popstack();
	}

	return kf;
}


//Similar to taskellcase, however we analyse blocs of three elements...
Tamgu* TamguCode::C_guard(x_node* xn, Tamgu* kf) {

	TamguInstructionTaskellMainCASE* ktest = new TamguInstructionTaskellMainCASE(global);

	while (xn->nodes.size() == 3) {		
		Traverse(xn->nodes[0], ktest);//the comparison
		Traverse(xn->nodes[1], ktest); //then the value
		xn = xn->nodes.back();
	}

	if (xn->token == "otherwise") {
		ktest->other = 1;
		Traverse(xn->nodes[0], ktest); //then the value
	}

	kf->AddInstruction(ktest);
	return kf;
}


Tamgu* TamguCode::C_alist(x_node* xn, Tamgu* kf) {
	//TamguInstruction* kbloc=new TamguInstruction(global,kf,a_bloc);

	if (xn->token == "merging") {
		//we create a sub-vector
		TamguConstvector* kbv = new TamguConstvectormerge(global, NULL);
		Traverse(xn->nodes[0], kbv);
		kf->push(kbv);
		return kbv;
	}

	TamguInstruction kbloc;
	Traverse(xn->nodes[0], &kbloc);
	Tamgu* ke = kbloc.instructions[0];
	kf->push(ke);
	if (xn->nodes.size() == 2)
		Traverse(xn->nodes[1], kf);
	return ke;
}

Tamgu* TamguCode::C_cut(x_node* xn, Tamgu* kf) {
	if (xn->token == "cut")
		kf->AddInstruction(aCUT);
	else {
		if (xn->token == "fail")
			kf->AddInstruction(aFAIL);
		else
			kf->AddInstruction(aSTOP);
	}
	return kf;
}

//A predicate container contains the rules that have been declared either at the top level or in a frame...
TamguPredicateContainer* TamguGlobal::Predicatecontainer() {
	if (predicateContainer == NULL)
		predicateContainer = new TamguPredicateContainer(this);
	return predicateContainer;
}

Tamgu* TamguCode::TamguParsePredicateDCGVariable(string& name, Tamgu* kcf, bool param) {
	//if (param)
	//	kcf = new TamguInstructionPARAMETER(global, kcf);

	TamguPredicateVariable* var;
    short idname = global->Getid(name);
	if (!global->predicatevariables.check(idname)) {
		var = new TamguPredicateVariable(global, idname, kcf);
		global->predicatevariables[idname] = var;
	}
	else {
		var = global->predicatevariables[idname];
		kcf->AddInstruction(var);
	}
	return var;
}

void TamguCode::ComputePredicateParameters(x_node* xn, Tamgu* kcf) {
	short id = a_parameterpredicate;
	TamguInstruction kbloc(id);

	for (int i = 0; i < xn->nodes.size(); i++) {
		kbloc.idtype = id;
		Traverse(xn->nodes[i], &kbloc);
		kcf->AddInstruction(kbloc.instructions[0]);
		kbloc.instructions.clear();
	}
}



// A call to a predicate expression
//We have three possibilities, which must be taken into account:
//1)  predicate(x,y):-true; in this case, this predicate is appended to the knowledge base
//2)  predicate(x,y):-false; in this case, it is removed from the knwoledge base
//3) predicate(x,y) :-  expression, then we must evaluate the expression to add this predicate
Tamgu* TamguCode::C_predicatefact(x_node* xn, Tamgu* kf) {
	//We create an expression
	//If it is a boolean value (either true or false)
	TamguPredicateContainer* kpcont = global->Predicatecontainer();

	string sname = xn->nodes[0]->nodes[0]->value;
	short name = global->Getid(sname);
	if (!global->predicates.check(name))
		global->predicates[name] = new TamguPredicateFunction(global, NULL, name);

    long nbpredicates = 0;
    if (kpcont->rules.find(name) != kpcont->rules.end())
        nbpredicates = kpcont->rules[name].size();
    
    char buff[100];
    sprintf_s(buff,100,"%ld_%d",nbpredicates,name);
    currentpredicatename = buff;
    
	global->predicatevariables.clear();
    bool previous = Activategarbage(false);
	if (global->predicate_definitions.find(name) == global->predicate_definitions.end() &&
        kpcont->rules.find(name) == kpcont->rules.end() &&
        (xn->token == "predicatefact" || xn->nodes[1]->value == "true")) {
		Tamgu* kbloc = new TamguInstructionPredicate(name, global);
		TamguPredicateKnowledgeBase* kcf;
        sname = xn->nodes[0]->token;
        if (sname == "dependencyfact") {
			kcf = new TamguDependencyKnowledgeBase(global, name, kbloc);
            kcf->add = true;
            Traverse(xn->nodes[0]->nodes[1], kcf);
            kcf->add = false;
        }
		else
			kcf = new TamguPredicateKnowledgeBase(global, name, kbloc);

		if (xn->nodes[0]->nodes.back()->token == "predicateparameters")
			ComputePredicateParameters(xn->nodes[0]->nodes.back(), kcf);
        
        if (kcf->isUnified(NULL)) {
            TamguPredicate* pv;
            if (sname == "predicate") {
                //In this case, we can already create the right element in the knowledgebase
                pv = new TamguPredicate(global, name);
                pv->parameters = kcf->parameters;
                if (pv->Stringpredicatekey(sname))
                    global->knowledgebase_on_first[sname].push_back(pv);
                
                if (pv->Stringpredicatekeysecond(sname))
                    global->knowledgebase_on_second[sname].push_back(pv);
                
                if (pv->Stringpredicatekeythird(sname))
                    global->knowledgebase_on_third[sname].push_back(pv);
            }
            else {
                pv = new TamguDependency(global, ((TamguDependencyKnowledgeBase*)kcf)->features, name, 0);
                pv->parameters = kcf->parameters;
            }
            
            kcf->parameters.clear();
            kcf->Release();
            kbloc->Remove();

            global->knowledgebase[name].push_back(pv);
            pv->Setreference();
            
            Activategarbage(previous);
            return pv;
        }
        
        //We want to add our value to the knowlegde base
        kcf->add = true;
        kf->AddInstruction(kbloc);
        currentpredicatename = "";
        Activategarbage(previous);
        return kbloc;
    }

	TamguPredicateRule* kbloc = new TamguPredicateRule(name, global, kf);
	TamguPredicate* kcf = new TamguPredicate(name, global);

	if (xn->nodes[0]->nodes.back()->token == "predicateparameters")
		ComputePredicateParameters(xn->nodes[0]->nodes.back(), kcf);
    
	kbloc->head = kcf;

	TamguPredicateRuleElement* kblocelement = new TamguPredicateRuleElement(global, NULL);
	//Else we need to browse our structure...
    if (xn->nodes.size() > 1)
        Traverse(xn->nodes[1], kblocelement);
    else
        kblocelement->AddInstruction(aTRUE);
	kbloc->Addtail(kpcont, kblocelement);
    currentpredicatename = "";
    Activategarbage(previous);
	return kbloc;
}


Tamgu* TamguCode::C_rawfact(x_node* xn, Tamgu* kf) {
    //We load a file containing only facts...
    short name = global->Getid(xn->nodes[0]->value);
    if (!global->predicates.check(name))
        global->predicates[name] = new TamguPredicateFunction(global, NULL, name);

    bool previous = Activategarbage(false);

    TamguPredicate* pv = new TamguPredicate(global, name);
    
    for (long i = 0; i < xn->nodes[1]->nodes.size(); i++)
        Traverse(xn->nodes[1]->nodes[i], pv);
    
    global->knowledgebase[name].push_back(pv);
    
    string argument_key;
    if (pv->Stringpredicatekey(argument_key))
        global->knowledgebase_on_first[argument_key].push_back(pv);
    
    if (pv->Stringpredicatekeysecond(argument_key))
        global->knowledgebase_on_second[argument_key].push_back(pv);
    
    if (pv->Stringpredicatekeythird(argument_key))
        global->knowledgebase_on_third[argument_key].push_back(pv);
    
    Activategarbage(previous);
    pv->Setreference();
    
    return pv;
}


//This is a DCG rule. The first element is the head of the rule. The other elements might be:
//a dcgfinal : [.]
//a predicate or a word...

Tamgu* TamguCode::C_dcg(x_node* xn, Tamgu* kf) {

	//the container is where the rules are stored... 
	TamguPredicateContainer* kpcont = global->Predicatecontainer();
	//We extract our predicate head name
	string sname = xn->nodes[0]->nodes[0]->value;
	short name = global->Getid(sname);
	if (!global->predicates.check(name))
		global->predicates[name] = new TamguPredicateFunction(global, NULL, name);
	//We compute the inner variables: from S0 to Sn
	short endidx = (short)xn->nodes.size() - 1;
	//The last element, if it is a final block should not be counted in...
	if (xn->nodes.back()->token == "finaldcg")
		endidx--;

	//We create our rule:
	global->predicatevariables.clear();
	TamguPredicateRule* krule = new TamguPredicateRule(name, global, kf);
	TamguPredicate* kcf = new TamguPredicate(name, global);
	krule->head = kcf;
	TamguPredicateRuleElement* kblocelement = NULL;
	TamguInstruction kcount;
	x_node* nsub;
	TamguPredicateRuleElement* kpredelement = NULL;
	char buff[] = { '?', '?', 'A', 0 };

	//Two cases: First it is a rule of the sort: det --> [a]...
	if (xn->nodes[1]->token == "finaltoken") {
		//Terminal rule		
		TamguConstvector* kvect = new TamguConstvector(global, kcf);
		//We add our value...
		Traverse(xn->nodes[1], kvect);
		sname = "";
		int i;
		int nb = 0;
		//we try to compute the number of dcgword in the middle... to generate a sequence of variable from ??X0 to ??Xnb
		for (i = 2; i < xn->nodes.size(); i++) {
			if (xn->nodes[2]->token == "dcgword")
				nb++;
			else
				break;
		}

		if (kvect->values.size()) {
			TamguConstvectormerge* kmerge = new TamguConstvectormerge(global, kvect);
			//Basically, we have: det --> [a]. which is transformed into det([a|?X],?X,...)
			sname = buff;
			TamguParsePredicateDCGVariable(sname, kmerge, false); //this part is [a|?X]
			if (nb) {
				buff[2] += nb;
				sname = buff;
			}
			TamguParsePredicateDCGVariable(sname, kcf, true); //this one is simply ?X...
		}
		else
			kcf->AddInstruction(kvect); //In that case, we do not want a variable... det([],[],...).

		//If we have some parameters, it is finally time to handle them, at the level of the root predicate...
		if (xn->nodes[0]->nodes.size() >= 2)
			ComputePredicateParameters(xn->nodes[0]->nodes.back(), kcf);

		for (int i = 2; i < xn->nodes.size(); i++) {
			//If we have a "finaldcg" object, then we create a specific predicate suite
			//which is added to kblocelement...
			if (xn->nodes[i]->token == "finaldcg") {
				kpredelement = new TamguPredicateRuleElement(global, kpredelement);
				Traverse(xn->nodes.back(), kpredelement);
				if (kblocelement == NULL)
					krule->Addtail(kpcont, kpredelement);
				else
					krule->Addtail(kpcont, kblocelement);
				return krule;
			}

			//We have a rule: det --> [a], tst...
			nsub = xn->nodes[i];
			//Then we analyze each element, which should have the same form as the head...
			name = global->Getid(nsub->nodes[0]->value);
			kpredelement = new TamguPredicateRuleElement(global, kpredelement);
			if (kblocelement == NULL)
				kblocelement = kpredelement;
			kcf = new TamguPredicate(name, global, a_predicate, kpredelement);
			if (sname.empty())
				kcf->AddInstruction(kvect); //in that case, we automatically transmit [], we do not have a ?X variable to deal with
			else {
				buff[2] = 'A' + i - 2;
				sname = buff;//previous variable
				TamguParsePredicateDCGVariable(sname, kcf, true); //we add the ?X variable... It is the rest of the list...
			}

			buff[2] = 'A' + i - 1;
			sname = buff;
			TamguParsePredicateDCGVariable(sname, kcf, true); //then we control variable...

			if (nsub->nodes.size() >= 2) //we add the other variables, which might part of the description of the predicate....
				ComputePredicateParameters(nsub->nodes.back(), kcf);
		}

		if (kblocelement == NULL)
			((TamguPredicateRule*)krule)->addfinal(kpcont);
		else
			krule->Addtail(kpcont, kblocelement);
		return krule;
	}

	kblocelement = new TamguPredicateRuleElement(global, NULL);

	//the head...
	//We will implement the head as: predicate(??S0,??Sn...) where n is the number of elements in our expressions
	// p --> sn,vp,pp. here n is 3... p(??S0,??S3...)	
	sname = buff;
	//We add our first variable
	TamguParsePredicateDCGVariable(sname, kcf, true);
	buff[2] = 'A' + endidx;
	sname = buff;
	//and our following one...
	TamguParsePredicateDCGVariable(sname, kcf, true);

	//If we have some parameters, it is finally time to handle them, at the level of the root predicate...
	if (xn->nodes[0]->nodes.size() >= 2)
		ComputePredicateParameters(xn->nodes[0]->nodes.back(), kcf);

	for (int i = 1; i <= endidx; i++) {
		nsub = xn->nodes[i];
		//Then we analyze each element, which should have the same form as the head...
		name = global->Getid(nsub->nodes[0]->value);
		if (kpredelement == NULL)
			kpredelement = kblocelement;
		else
			kpredelement = new TamguPredicateRuleElement(global, kpredelement);

		kcf = new TamguPredicate(name, global, a_predicate, kpredelement);
		buff[2] = 'A' + i - 1;
		sname = buff;
		TamguParsePredicateDCGVariable(sname, kcf, true);
		buff[2] = 'A' + i;
		sname = buff;
		TamguParsePredicateDCGVariable(sname, kcf, true);
		if (nsub->nodes.size() >= 2)
			ComputePredicateParameters(nsub->nodes.back(), kcf);
	}

	if (xn->nodes.back()->token == "finaldcg") {
		if (kpredelement == NULL)
			Traverse(xn->nodes.back(), kblocelement);
		else {
			kpredelement = new TamguPredicateRuleElement(global, kpredelement);
			Traverse(xn->nodes.back(), kpredelement);
		}
	}

	krule->Addtail(kpcont, kblocelement);
	return krule;
}


Tamgu* TamguCode::C_dependency(x_node* xn, Tamgu* kf) {
	int i = 0;
	bool modifcall = false;
	bool negation = false;
	if (xn->nodes[0]->token == "modifcall") {
		if (global->modifieddependency != NULL) {
			stringstream message;
			message << "Error: You can only modify one dependency at a time.";
			throw new TamguRaiseError(message, filename, current_start, current_end);
		}

		modifcall = true;
		i++;
	}
	else
	if (isnegation(xn->nodes[0]->token)) {
		negation = true;
		i++;
	}

	string name = xn->nodes[i++]->value;
	long lst = (long)name.size() - 1;
	short idname = -1;
	short idvar = 0;
	if (name[lst] >= '0' && name[lst] <= '9' && lst >= 1) {
		//we then check, if we have a '_', which is indicative of a dependency variable...
		lst--;
		while (name[lst] >= '0' && name[lst] <= '9')
			lst--;
		if (name[lst] == '_') {
			if (lst != 0) {
				string nm = name.substr(0, lst);
				idname = global->Getid(nm);
				name = name.substr(lst, name.size() - lst);
			}
			else
				idname = a_universal;

			idvar = global->Getid(name);

			//We then add a variable, (which will be stored in main memory)
			if (mainframe.isDeclared(idname) == false) {
				TamguDependency* a = new TamguDependency(global, aNULL, idvar, idvar);
				mainframe.Declare(idvar, a);
			}
		}
	}

	if (idname == -1)
		idname = global->Getid(name);

	if (!modifcall) {
		if (isaFunction(name, idname))
			return C_regularcall(xn, kf);

		if (global->predicates.check(idname)) {
			TamguPredicate* kx = global->predicates[idname];
			if (kx->isPredicateMethod()) {
				kx = (TamguPredicate*)kx->Newinstance(0, kf);
				kf->AddInstruction(kx);

				for (i = 1; i < xn->nodes.size(); i++)
					Traverse(xn->nodes[i], kx);

				if (!kx->Checkarity()) {
					stringstream message;
					message << "Wrong number of arguments or incompatible argument: '" << name << "'";
					throw new TamguRaiseError(message, filename, current_start, current_end);
				}
				kx->Setnegation(negation);
				return kx;
			}
		}
	}

	TamguDependency* a = new TamguDependency(global, aNULL, idname, idvar);
	a->negation = negation;

	if (modifcall) {
		global->modifieddependency = a;
		a->idvar = a_modifydependency;
		TamguDependency* adep = new TamguDependency(global, aNULL, a_modifydependency, a_modifydependency);
		mainframe.Declare(a_modifydependency, adep);
	}


	if (xn->nodes[i]->token == "features") {
		a->chosen = 2;
		Traverse(xn->nodes[i], a);
		i++;
	}

	a->chosen = 0;
	Traverse(xn->nodes[i], a);

	a->chosen = modifcall;
	kf->AddInstruction(a);

	return kf;
}

Tamgu* TamguCode::C_dependencyresult(x_node* xn, Tamgu* kpredelement) {
	TamguDependencyAction* kbloc;
	long idrule = global->Predicatecontainer()->rules[a_dependency].size();

	for (int i = 0; i < xn->nodes.size(); i++) {
		kpredelement = new TamguPredicateRuleElement(global, kpredelement);
		if (isnegation(xn->nodes[i]->token)) {
			if (global->modifieddependency != NULL) {
				kbloc = new TamguDependencyAction(global, a_dependency_remove, idrule, kpredelement);
				kbloc->AddInstruction(global->modifieddependency);
				//We change the idvar to 0, to distinguish between a dependency deletion from a simple modification
				kbloc->idvar = 0;
			}
		}
		else {
			if (xn->nodes[i]->token == "dependency") {
				short nm = 0;
				if (global->modifieddependency != NULL) {
					kbloc = new TamguDependencyAction(global, a_dependency_remove, idrule, kpredelement);
					kbloc->AddInstruction(global->modifieddependency);
					global->modifieddependency = NULL;

					kpredelement = new TamguPredicateRuleElement(global, kpredelement);
					nm = a_modifydependency;
				}

				kbloc = new TamguDependencyAction(global, a_assertz, idrule, kpredelement);

				// We prevent feature assignement within a dependency as result...
				featureassignment = 1;
				Traverse(xn->nodes[i], kbloc);
				featureassignment = 0;
				kbloc->idvar = nm;
			}
			else
				Traverse(xn->nodes[i], kpredelement);
		}
	}
	
	return kpredelement;
}


static void StorePredicateVariables(x_node* xn, Tamgusynode** a) {
	for (int i = 0; i < xn->nodes.size(); i++) {
		if (xn->nodes[i]->token == "predicatevariable") {
			if (xn->nodes[i]->nodes.size() == 0 || xn->nodes[i]->nodes[0]->token != "anumber") //#_ variable
				continue;
			string sid = xn->nodes[i]->nodes[0]->value;
			if (globalTamgu->dependencyvariables.find(sid) == globalTamgu->dependencyvariables.end()) {
				Tamgusynode* as = new Tamgusynode(atoi(STR(sid)), globalTamgu);
				globalTamgu->dependencyvariables[sid] = as;
                as->investigate |= is_constante;
				as->reference = 1;
				if (*a == NULL)
					*a = as;
			}
		}
	}
}

//This is a dependency syntactic rule of the form:
// IF (dep(#1,#2,#3)...) dep(#1,#2,#3).
Tamgu* TamguCode::C_dependencyrule(x_node* xn, Tamgu* kf) {
	//We extract our predicate head name
	if (!global->predicates.check(a_dependency))
		global->predicates[a_dependency] = new TamguPredicateFunction(global, NULL, a_dependency);
	//We compute the inner variables: from S0 to Sn
	short endidx = (short)xn->nodes.size() - 1;
	//The last element, if it is a final block should not be counted in...
	if (xn->nodes.back()->token == "dependencyresult")
		endidx--;

	//We create our rule:
	global->modifieddependency = NULL;
	global->dependencyvariables.clear();
	TamguPredicateRule* krule = new TamguPredicateRule(a_dependency, global, kf);
	TamguPredicate* kcf = new TamguPredicate(a_dependency, global);
	krule->head = kcf;
	x_node* nsub;

	//the head...
	//We will implement the head as: &head&(#1,#n), where #1..#n are extracted from the dependencies...
	int i;
	string sid;
	Tamgusynode* as = NULL;
	for (i = 0; i < (int)xn->nodes.size() - 1; i++) {
		nsub = xn->nodes[i]; //we do not take into account the negated dependencies... Hence the test nsub->nodes[0]->token
		if (nsub->token == "dependance" && nsub->nodes[0]->token == "dependency") {
			nsub = nsub->nodes[0];
			if (isnegation(nsub->nodes[0]->token))
				continue;
			StorePredicateVariables(nsub->nodes.back(), &as);
		}
	}

	switch (global->dependencyvariables.size()) {
	case 0: //no variable detected, we add two empty variables
		kcf->AddInstruction(aUNIVERSAL);
		kcf->AddInstruction(aUNIVERSAL);
		break;
	case 1:
		//we add it twice... if there is only one that was detected
		kcf->AddInstruction(as);
		kcf->AddInstruction(as);
		break;
	default: //else the two first...
	{
				 map<string, Tamgu*>::iterator it = global->dependencyvariables.begin();
				 kcf->AddInstruction(it->second);
				 it++;
				 kcf->AddInstruction(it->second);
	}
	}

	TamguPredicateRuleElement* kblocelement = new TamguPredicateRuleElement(global, NULL);
	TamguPredicateRuleElement* kpredelement = NULL;

	featureassignment = 0;

	for (i = 0; i < (int)xn->nodes.size() - 1; i += 2) {
		nsub = xn->nodes[i];
		if (kpredelement == NULL)
			kpredelement = kblocelement;
		else
			kpredelement = new TamguPredicateRuleElement(global, kpredelement);
		Traverse(nsub, kpredelement);
		kpredelement->Setnegation(kpredelement->instructions[0]->isNegation());
		if (xn->nodes[i + 1]->value == "||" || xn->nodes[i + 1]->value == "or") {
			kpredelement->instructions.back()->Setdisjunction(true);
			kpredelement->Setdisjunction(true);
		}
	}

	Traverse(xn->nodes.back(), kpredelement);

	//the container retrieved by Predicatecontainer is where the rules are stored... 
	krule->Addtail(global->Predicatecontainer(), kblocelement);
	return krule;
}

Tamgu* TamguCode::C_predicate(x_node* xn, Tamgu* kf) {
	short name = global->Getid(xn->nodes[0]->value);
	TamguPredicate* kcf = new TamguPredicate(name, global);

	if (xn->nodes.back()->token == "predicateparameters")
		ComputePredicateParameters(xn->nodes.back(), kcf);
	kf->AddInstruction(kcf);
	return kcf;
}


Tamgu* TamguCode::C_predicateexpression(x_node* xn, Tamgu* kf) {
	//This is where we analyse our structure...
	TamguPredicateRuleElement* kbloc = new TamguPredicateRuleElement(global, kf);
	if (isnegation(xn->nodes[0]->token)) {
		kbloc->negation = true;
		if (xn->nodes[1]->token == "expressions") {
			TamguInstruction klocbloc;
			Traverse(xn->nodes[1], &klocbloc);
			Tamgu* kint = CloningInstruction((TamguInstruction*)klocbloc.instructions[0]);
			kbloc->AddInstruction(kint);
		}
		else
			Traverse(xn->nodes[1], kbloc);
		if (xn->nodes.size() == 4) {
			if (xn->nodes[2]->value == ";")
				kbloc->disjunction = true;
			Traverse(xn->nodes[3], kbloc);
		}
	}
	else {
		if (xn->nodes[0]->token == "expressions") {
			TamguInstruction klocbloc;
			Traverse(xn->nodes[0], &klocbloc);
			Tamgu* kint = CloningInstruction((TamguInstruction*)klocbloc.instructions[0]);
			kbloc->AddInstruction(kint);
		}
		else
			Traverse(xn->nodes[0], kbloc);
		if (xn->nodes.size() == 3) {
			if (xn->nodes[1]->value == ";")
				kbloc->disjunction = true;
			Traverse(xn->nodes[2], kbloc);
		}
	}
	return kbloc;
}

bool Mergingfeatures(Tamgumapss* nodes, Tamgumapss* novel) {
	hmap<string, string>::iterator it;
	for (it = novel->values.begin(); it != novel->values.end(); it++) {
		if (nodes->values.find(it->first) == nodes->values.end()) 
			nodes->values[it->first] = it->second;
		else {
			if (nodes->values[it->first] != it->second)
				return false;
		}
	}

	return true;
}

Tamgu* TamguCode::C_predicatevariable(x_node* xn, Tamgu* kf) {

	string& name = xn->value;

	if (name[0] == '#') {
		//if it is a "#_" variable, then it is a UNIVERSAL variable...
		if (xn->nodes.size() == 0) {
			kf->AddInstruction(aUNIVERSAL);
			return aUNIVERSAL;
		}

		Tamgusynode* as;
		int sz = 2;
		if (xn->nodes[0]->token != "anumber") { //again a '#_' variable but with constraints...
			as = new Tamgusynode(a_universal, global);
            as->investigate |= is_constante;
			sz = 1;
		}
		else {
			string sid = xn->nodes[0]->value;
			as = (Tamgusynode*)global->dependencyvariables[sid];
			if (as == NULL) {
				stringstream message;
				message << "Non instanciated variable in a dependency rule: '" << name << "' ";
				throw new TamguRaiseError(message, filename, current_start, current_end);
			}						
		}

		if (xn->nodes.size() == sz) {
			if (kf->Action() == a_assignement || kf->Type() == a_predicateruleelement) {
				TamguCallsynode* acs = new TamguCallsynode(as, global, kf);
				Traverse(xn->nodes[sz - 1], acs);
				return acs;
			}
			

			if (featureassignment == 1) {
				stringstream message;
				message << "Cannot assign or test a feature to this dependency node";
				throw new TamguRaiseError(message, filename, current_start, current_end);
			}

			featureassignment = 2;

			if (as->features == aNULL)
				Traverse(xn->nodes[sz - 1], as);
			else {
				TamguInstruction kbloc;
				Traverse(xn->nodes[sz - 1], &kbloc);

				if (kbloc.instructions[0]->Type() != a_mapss) {
					stringstream message;
					message << "Wrong feature structure";
					throw new TamguRaiseError(message, filename, current_start, current_end);
				}

				if (!Mergingfeatures((Tamgumapss*)as->features, (Tamgumapss*)kbloc.instructions[0])) {
					stringstream message;
					message << "Incoherent feature testing";
					throw new TamguRaiseError(message, filename, current_start, current_end);
				}
			}			

			featureassignment = 0;
		}

		kf->AddInstruction(as);
		return as;
	}

	TamguPredicateVariable* var;
	short idname;

	if (xn->nodes[0]->token == "goal")
		return Traverse(xn->nodes[0], kf);

    //We build the variable name as a concatenation with the predicate idname...
    //This is a simple way to get individual variables in predicates, which simplifies the matching...
    name += currentpredicatename;
    idname = global->Getid(name);

    if (!global->predicatevariables.check(idname)) {
		var = new TamguPredicateVariable(global, idname, kf);
		global->predicatevariables[idname] = var;
	}
	else {
		var = global->predicatevariables[idname];
		kf->AddInstruction(var);
	}
	return var;
}


Tamgu* TamguCode::C_assertpredicate(x_node* xn, Tamgu* kf) {
	short id = a_assertz;
	if (xn->nodes[0]->token == "assertcommandbefore")
		id = a_asserta;
	else {
		if (xn->nodes[0]->token == "retractcommand")
			id = a_retract;
	}

	Tamgu* kbloc = new TamguPredicateAction(global, id, kf);
	if (xn->nodes.size() != 2)  {
		stringstream message;
		message << "Error: Wrong assert or retract definition";
		throw new TamguRaiseError(message, filename, current_start, current_end);
	}
	Traverse(xn->nodes.back(), kbloc);
	return kbloc;
}


Tamgu* TamguCode::C_term(x_node* xn, Tamgu* kf) {
	string name("_");
	short idname = a_universal;
	if (xn->token == "term") {
		name = xn->value;
		name = name.substr(0, name.size() - 2);
		idname = global->Getid(name);
	}

	TamguPredicateTerm* var = new TamguPredicateTerm(global, idname, kf);
	if (xn->nodes.back()->token == "predicateparameters")
		ComputePredicateParameters(xn->nodes.back(), var);
	return var;
}


Tamgu* TamguCode::C_comparepredicate(x_node* xn, Tamgu* kf) {
	//The first parameter is the operator
	short op = global->string_operators[xn->nodes[1]->value];
	Tamgu* ki = TamguCreateInstruction(kf, op);
	for (size_t i = 0; i < xn->nodes.size(); i++)
		Traverse(xn->nodes[i], ki);
	return ki;
}

//---------------------------- Rule Compiling -------------------------------------
Tamgu* TamguCode::C_annotationrule(x_node* xn, Tamgu* kf) {
    //This is the root call to the creation of an annotation rule...
    //First we need to create our initial root...
    //There is only one rule section...
    if (global->gTheAnnotationRules==NULL)
        global->gTheAnnotationRules=new An_rulesConst(global); //this one cannot be deleted through a Resetreference...
    

    //A rule is composed of a category, a context and different annotation...
    
    global->gTheAnnotationRules->getpos();
    
    //Then the rest...
    An_rule* krule=new An_rule;
    short ipos=0;
    if (xn->nodes[0]->token=="ruletype") {
        switch(xn->nodes[0]->value[0]) {
            case '@':
                krule->lexicon=true;
                break;
            case '~':
                krule->removing=true;
                break;
            case '#':
                krule->classing=true;
                break;
        }
        ipos=1;
    }
    
    s_utf8_to_unicode(krule->category, USTR(xn->nodes[ipos]->value), xn->nodes[ipos]->value.size());
    ++ipos;
    
    for (long i=ipos;i<xn->nodes.size();i++)
        Traverse(xn->nodes[i], krule);
    
    //Then we add our rule to gTheAnnotationRules
    krule->last->status|=an_end;
    if (krule->lexicon==false)
        krule->scanend();
    
    if (!global->gTheAnnotationRules->storerule(krule)) {
        stringstream message;
        message << "Unknown expression: " << xn->nodes[ipos-1]->value;
        throw new TamguRaiseError(message, filename, current_start, current_end);
    }
    
    return krule;
}

Tamgu* TamguCode::C_annotation(x_node* xn, Tamgu* kf) {

    if (xn->nodes[0]->token=="non") {
        //if can reach this level, then it is an error...
        //it can only apply to a token...
        global->gTheAnnotationRules->negation=true;
        Traverse(xn->nodes[1], kf);
        global->gTheAnnotationRules->negation=false;
        return kf;
    }
    
    Traverse(xn->nodes[0], kf);
    return kf;
}

Tamgu* TamguCode::C_listoftokens(x_node* xn, Tamgu* kf) {
    
    An_rule* krule=(An_rule*)kf;
    long mx=xn->nodes.size();
    
    An_state* next=global->gTheAnnotationRules->newstate();
    
    char kleene=0;
    long maxkleene=0;
    if (xn->nodes.back()->token=="kleene") {
        mx--;
        kleene=xn->nodes.back()->value[0];
        if (xn->nodes.back()->nodes.size())
            maxkleene=convertinteger(xn->nodes.back()->nodes[0]->value);
    }

    An_state* current=krule->last;
    for (long i=0;i<mx;i++) {
        if (xn->nodes[i]->nodes[0]->token=="token") {
            krule->last=current;
            krule->next=next;
            Traverse(xn->nodes[i], krule);
            krule->next=NULL;
            continue;
        }
        
        An_rule rl;
        Traverse(xn->nodes[i], &rl);
        for (long a=0;a<rl.first->arcs.size();a++)
            current->arcs.push_back(rl.first->arcs[a]);
        rl.first->addtotail(next);
    }

    if (kleene) {
        //A final state is a temporary end state...
        next->setfinal();

        //first, we can use all our arcs in current in next...
        for (long i=0;i<current->arcs.last;i++) {//this is our loop...
            next->arcs.push_back(current->arcs[i]);
        }

        if (maxkleene) {
            next->setmax(maxkleene);
            krule->hasmax=true;
        }

        if (kleene=='*') {
            //we need now an escape point...
            An_arc* epsilon=global->gTheAnnotationRules->newarc(new An_epsilon,next);
            current->arcs.push_back(epsilon);
            epsilon->state->setfinal();
        }
    }
    
    krule->last=next;
    return krule;
}

Tamgu* TamguCode::C_sequenceoftokens(x_node* xn, Tamgu* kf) {
    An_rule* krule=(An_rule*)kf;
    long mx=xn->nodes.size();
    
    char kleene=0;
    long maxkleene=0;
    if (xn->nodes.back()->token=="kleene") {
        mx--;
        kleene=xn->nodes.back()->value[0];
        if (xn->nodes.back()->nodes.size())
            maxkleene=convertinteger(xn->nodes.back()->nodes[0]->value);
    }

    An_state* current=krule->last;
    for (long i=0;i<mx;i++)
        Traverse(xn->nodes[i], krule);
   
    if (kleene) {
        An_state* next=krule->last;
        next->setfinal();
        //first, we can use all our arcs in current in next...
        for (long i=0;i<current->arcs.last;i++) {//this is our loop...
            next->arcs.push_back(current->arcs[i]);
        }

        if (maxkleene) {
            next->setmax(maxkleene);
            krule->hasmax=true;
        }

        //we need now an escape point...
        if (kleene=='*') {
            An_arc* epsilon=global->gTheAnnotationRules->newarc(new An_epsilon);
            current->arcs.push_back(epsilon);
            epsilon->state->setfinal();
        }
    }

    return krule;
}

Tamgu* TamguCode::C_optionaltokens(x_node* xn, Tamgu* kf) {
    An_rule* krule=(An_rule*)kf;

    An_state* current=krule->last;
    for (long i=0;i<xn->nodes.size();i++)
        Traverse(xn->nodes[i], krule);

    An_arc* epsilon=global->gTheAnnotationRules->newarc(new An_epsilon,krule->last);
    current->arcs.push_back(epsilon);
    epsilon->state->setfinal();

    return krule;
}

Tamgu* TamguCode::C_removetokens(x_node* xn, Tamgu* kf) {
    An_rule* krule=(An_rule*)kf;

    global->gTheAnnotationRules->remove=true;
    krule->hasremove=true;
    for (long i=0;i<xn->nodes.size();i++)
        Traverse(xn->nodes[i], krule);
    global->gTheAnnotationRules->remove=false;
    return krule;
}

Tamgu* TamguCode::C_token(x_node* xn, Tamgu* kf) {
    An_rule* krule=(An_rule*)kf;
    
    An_any* a=NULL;
    bool first=true;
    
    if (xn->nodes[0]->token=="meta") {
        a = new An_meta(xn->nodes[0]->value[1]);
    }
    else {
        if (xn->nodes[0]->token=="metas") {
            string val=xn->nodes[0]->value;
            val=val.substr(1,val.size()-2);
            a = new An_automaton(val);
            if (((An_automaton*)a)->action==NULL) {
                delete a;
                stringstream message;
                message << "Unknown expression: " << xn->nodes[0]->value;
                throw new TamguRaiseError(message, filename, current_start, current_end);
            }
        }
    
#ifdef Tamgu_REGEX
        else
            if (xn->nodes[0]->token=="regex") {
                wstring rgx;
                s_utf8_to_unicode(rgx, USTR(xn->nodes[0]->value), xn->nodes[0]->value.size());
                rgx=rgx.substr(1,rgx.size()-2);
                a = new An_regex(rgx);
                if (((An_regex*)a)->action==NULL) {
                    delete a;
                    stringstream message;
                    message << "Unknown expression: " << xn->nodes[0]->value;
                    throw new TamguRaiseError(message, filename, current_start, current_end);
                }
            }
#endif
            else
                if (xn->nodes[0]->token=="callmatch") {
                    x_node* sub=xn->nodes[0];
                    string funcname = sub->nodes[0]->value;
                    short idf = global->Getid(funcname);
                    Tamgu* kfunc = NULL;
                    kfunc = Declaration(idf, kf);
                    //We have a WITH description
                    if (kfunc == NULL) {
                        stringstream message;
                        message << "Unknown function: '" << funcname << "'";
                        throw new TamguRaiseError(message, filename, current_start, current_end);
                    }
                    if (krule->classing || krule->removing)
                        a = new An_call(kfunc, false); //here only one...
                    else
                        a = new An_call(kfunc, true); // in this case, we need two variables
                    TamguCallFunction* call=&((An_call*)a)->call;
                    
                    for (long i=1;i<sub->nodes.size();i++)
                        Traverse(sub->nodes[i], call);

                    if (!call->Checkarity()) {
                        stringstream message;
                        message << "Wrong number of arguments or incompatible argument: '" << funcname << "'";
                        throw new TamguRaiseError(message, filename, current_start, current_end);
                    }
                }
                else {
                    if (xn->nodes[0]->token=="any") {
                        a=new An_any;
                        first=false;
                    }
                    else {
                        wstring tk;
                        if (xn->nodes[0]->token=="orlabels") {
                            a = new An_orlabels;
                            for (long i=0;i<xn->nodes[0]->nodes.size();i++) {
                                sc_utf8_to_unicode(tk, USTR(xn->nodes[0]->nodes[i]->value), xn->nodes[0]->nodes[i]->value.size());
                                if (tk[0] == '~') {
                                    tk=tk.substr(1,tk.size()-1);
                                    ((An_orlabels*)a)->negations.push_back(true);
                                }
                                else
                                    ((An_orlabels*)a)->negations.push_back(false);
                                ((An_orlabels*)a)->actions.push_back(tk);
                            }
                        }
                        else {
                            if (xn->nodes[0]->token=="andlabels") {
                                a = new An_andlabels;
                                for (long i=0;i<xn->nodes[0]->nodes.size();i++) {
                                    sc_utf8_to_unicode(tk, USTR(xn->nodes[0]->nodes[i]->value), xn->nodes[0]->nodes[i]->value.size());
                                    if (tk[0] == '~') {
                                        tk=tk.substr(1,tk.size()-1);
                                        ((An_andlabels*)a)->negations.push_back(true);
                                    }
                                    else
                                        ((An_andlabels*)a)->negations.push_back(false);
                                    ((An_andlabels*)a)->actions.push_back(tk);
                                }
                            }
                            else {
                                s_utf8_to_unicode(tk, USTR(xn->nodes[0]->value), xn->nodes[0]->value.size());
                                if (xn->nodes[0]->token=="label")
                                    a = new An_label(tk);
                                else
                                    if (xn->nodes[0]->token=="lemma")
                                        a = new An_lemma(tk);
                                    else
                                        a = new An_token(tk);
                            }
                        }
                    }
                }
    }
    
    if (xn->nodes.size()==1) {
        krule->addarc(a,first);
        return krule;
    }
    
    
    An_state* current=krule->last;
    char kleene=xn->nodes[1]->value[0];
    long maxkleene=0;
    if (xn->nodes[1]->nodes.size())
        maxkleene=convertinteger(xn->nodes[1]->nodes[0]->value);
    
    An_arc* arc=krule->addarc(a,false);
    //The arc loops on itself...
    arc->state->arcs.push_back(arc);
    //It becomes a final arc... A final arc is an arc to which attach the next arcs...
    arc->state->setfinal();
    if (maxkleene) {
        arc->state->setmax(maxkleene);
        krule->hasmax=true;
    }
    
    if (kleene=='*') {
        //In this case, we create an epsilon to this state
        An_arc* epsilon=global->gTheAnnotationRules->newarc(new An_epsilon,arc->state);
        current->arcs.push_back(epsilon);
        epsilon->state->setfinal();
    }
    
    return krule;
}

//------------------------------------------------------------------------------------------------------
//--------------------------- Lisp Section
//------------------------------------------------------------------------------------------------------

Tamgu* TamguCode::C_tamgulispquote(x_node* xn, Tamgu* parent) {
    Tamgu* kf = parent;
    if (compilemode) {
        kf = new Tamgulispcode(global, parent);
        kf->Setaction(a_quote);
    }
    else {
        kf = globalTamgu->Providelisp();
        kf->Setaction(a_quote);
        parent->push(kf);
    }
    
    for (long i = 0; i < xn->nodes.size(); i++)
        Traverse(xn->nodes[i], kf);
    
    return kf;
}

Tamgu* TamguCode::C_tamgulispvariable(x_node* xn, Tamgu* parent) {
    string name = xn->nodes[0]->value;
    Tamgu* a = new Tamgulispvariable(name, global, parent);
    Traverse(xn->nodes[1], a);
    return a;
}

Tamgu* TamguCode::C_tamgulispatom(x_node* xn, Tamgu* parent) {
    Tamgu* a;
    if (xn->nodes[0]->token == "word") {
        //We check if it is a variation on car/cdr
        string word = xn->nodes[0]->value;
        if (word[0] == 'c' && word.back() == 'r') {
            bool found = true;
            for (long i = 1; i < word.size()-1; i++) {
                if (word[i] != 'a' && word[i] != 'd') {
                    found = false;
                    break;
                }
            }
            if (found) {
                word = word.substr(1, word.size()-2);
                a = new Tamgucadr(word, global, parent);
            }
            else
                a = global->Providelispsymbols(word, parent);
        }
        else
            a = global->Providelispsymbols(word, parent);
    }
    else
        Traverse(xn->nodes[0], parent);
    return parent;
}

Tamgu* TamguCode::C_tamgulisp(x_node* xn, Tamgu* parent) {
    //We have four cases: tlatom, tlquote, tlist

    Tamgu* kf = parent;

    if (compilemode) {
        if (!xn->nodes.size()) {
            kf = aEMPTYLISP;
            parent->AddInstruction(kf);
        }
        else {
            if (xn->nodes.size() > 1 && parent->isMainFrame() && global->systemfunctions.find(xn->nodes[0]->value) != global->systemfunctions.end()) {
                x_node nx("parameters");
                for (short i = 1; i < xn->nodes.size(); i++)
                    nx.nodes.push_back(xn->nodes[i]);
                try {
                    Callingprocedure(&nx, global->Getid(xn->nodes[0]->value));
                    nx.nodes.clear();
                }
                catch (TamguRaiseError* a) {
                    nx.nodes.clear();
                    throw a;
                }
                
                //if the value stored in systemfunctions is true, then it means that we have
                //a local call otherwise, it means that the function should be called twice.
                //At compile time and at run time
                return parent;
            }
            kf = new Tamgulispcode(global, parent);
        }
    }
    else {
        if (!xn->nodes.size())
            kf = aEMPTYLISP;
        else
            kf = globalTamgu->Providelisp();
        parent->push(kf);
    }
    
    long i;
    for (i = 0; i < xn->nodes.size(); i++)
        Traverse(xn->nodes[i], kf);
    
    //The next section deals with idea of precompiling some function calls
    //there are three cases, when we do not want to evaluate a specific list
    //defun: if the list is 3, then kf is the list of parameters, no evaluation
    //lambda: if the list is 2, then kf is the list of parameters, no evaluation
    //quote: The next element is of course not evaluated
    i = parent->Size();
    short n;
    Tamgu* a;
    if (i > 1 && parent->isLisp() && parent != kf) {
        a = parent->getvalue(0);
        n = a->Action();
        if ((n == a_lambda || n == a_quote) && i == 2)
            return kf;
        if (n == a_defun && i == 3)
            return kf;
    }
    
    
    if (kf->Size()) {
        a = kf->getvalue(0);
        n = a->Name();
        if (n == a_defun) {
            a = kf->getvalue(1);
            if (parent->isMainFrame() && !isDeclared(a->Name())) {
                Tamgu* l = kf->Eval(parent, aNULL, 0);
                if (!l->isFunction()) {
                    stringstream message;
                    message << "Wrong definition of a lisp 'defun' function";
                    throw new TamguRaiseError(message, filename, current_start, current_end);
                }
            }
            return kf;
        }
        
        if (n > a_lisp && a->Type() == a_atom) {
            //This is a call to a function
            if (isDeclared(n)) {
                a = Declaration(n);
                if (a->isFunction()) {
                    if (a->Type() == a_lisp)
                        ((Tamgulisp*)kf)->values[0] = global->Providelispsymbols(n, a_calllisp);
                    else
                        ((Tamgulisp*)kf)->values[0] = global->Providelispsymbols(n, a_callfunction);
                }
            }
            else {
                if (globalTamgu->procedures.check(n)) {
                    ((Tamgulisp*)kf)->values[0] = global->Providelispsymbols(n, a_callprocedure);
                }
                else {
                    if (globalTamgu->commons.check(n)) {
                        ((Tamgulisp*)kf)->values[0] = global->Providelispsymbols(n, a_callcommon);
                    }
                    else {
                        if (globalTamgu->allmethods.check(n)) {
                            ((Tamgulisp*)kf)->values[0] = global->Providelispsymbols(n, a_callmethod);
                        }
                    }
                }
            }
        }
    }
    
    return kf;
}
//------------------------------------------------------------------------------------------------------
//--------------------------- Lisp Section End
//------------------------------------------------------------------------------------------------------

bool TamguCode::Load(tokenizer_result<string>& xr) {
    if (xr.size() == 0)
        return false;
    
	short currentspaceid = global->spaceid;
	bnf_tamgu* previous = global->currentbnf;
    TamguRecordFile(filename, this, global);
	
    global->spaceid = idcode;

	bnf_tamgu bnf;
	bnf.baseline = global->linereference;
	global->lineerror = -1;

	x_node* xn = bnf.x_parsing(&xr, FULL);
	if (xn == NULL) {
		cerr << " in " << filename << endl;
		stringstream message;
		global->lineerror = bnf.lineerror;
		currentline = global->lineerror;
		message << "Error while parsing program file: ";
		if (bnf.errornumber != -1)
			message << bnf.x_errormsg(bnf.errornumber);
		else
			message << bnf.labelerror;
		throw new TamguRaiseError(message, filename, global->lineerror, bnf.lineerror);
	}


	global->currentbnf = &bnf;

    VECTE<Tamgu*> stack;
    stack = global->threads[0].stack;
    global->threads[0].stack.clear();
    
	global->Pushstack(&mainframe);
	Traverse(xn, &mainframe);
	global->Popstack();
    global->threads[0].stack = stack;
    
	global->currentbnf = previous;
	global->spaceid = currentspaceid;

	delete xn;
	return true;
}

//------------------------------------------------------------------------
void InitWindowMode();

static bool lispmode = false;
bool ToggleLispMode() {
    lispmode = 1 - lispmode;
    return lispmode;
}
bool isLispmode() {
    return lispmode;
}

void Setlispmode(bool v) {
    lispmode = v;
}

bool TamguCode::CompileFull(string& body, vector<TamguFullError*>& errors) {
    tokenizer_result<string> xr;
    bnf_tamgu bnf;

    InitWindowMode();
    
    global->threads[0].message.str("");
    global->threads[0].message.clear();
    
    //we store our TamguCode also as an Tamgutamgu...
    filename = NormalizeFileName(filename);
    Tamgu* main = TamguRecordFile(filename, this, global);
    
    global->CreateSystemVariable(main, a_mainframe, a_tamgu);

    global->spaceid = idcode;

    xr.lispmode = lispmode;

    if (body[0] == '(' && body[1] == ')') {
        xr.lispmode = true;
        body[0] = '/';
        body[1] = '/';
    }

    body += "\n";
    global->tamgu_tokenizer.tokenize<string>(body, xr);
    
    if (!xr.size())
        return false;
    
    global->lineerror = -1;


    x_node* xn;
    if (xr.lispmode) {
        bnf.initialize(&xr);
        bnf.baseline = global->linereference;
        string lret;
        xn = new x_node;
        if (bnf.m_tamgupurelisp(lret, &xn) != 1 || bnf.currentpos != bnf.fx->stack.size()) {
            delete xn;
            TamguFullError* err;
            long pos = bnf.currentpos;
            long ln = xr.stackln[pos];

            if (bnf.errornumber != -1)
                err = new TamguFullError(bnf.x_errormsg(bnf.errornumber), filename, xr.bpos[pos], xr.cpos[pos], ln);
            else
                err = new TamguFullError(bnf.labelerror, filename, xr.bpos[pos], xr.cpos[pos], ln);
            
            errors.push_back(err);
            return false;
        }
    }
    else {
        bnf.baseline = global->linereference;
        xn = bnf.x_parsing(&xr, FULL);
    }
    
    if (xn == NULL) {
        TamguFullError* err;
        long pos;
        long ln;
        while (xn == NULL) {
            pos = bnf.currentpos;
            ln = xr.stackln[pos];
            
            if (bnf.errornumber != -1)
                err = new TamguFullError(bnf.x_errormsg(bnf.errornumber), filename, xr.bpos[pos], xr.cpos[pos], ln);
            else
                err = new TamguFullError(bnf.labelerror, filename, xr.bpos[pos], xr.cpos[pos], ln);
            
            errors.push_back(err);
            
            while (pos < xr.stackln.size() && xr.stackln[pos] <= ln) pos++;
            
            if (pos == xr.stackln.size())
                break;
            
            bnf.currentpos = pos;
            xn = bnf.x_call_again(&xr, FULL);
        }
        
        if (xn != NULL)
            delete xn;
        return false;
    }

    firstinstruction = mainframe.instructions.size();
    global->currentbnf = &bnf;

    fullcompilemode = true;
    
    global->Pushstack(&mainframe);
    char loop_on_error = 1;
    while (loop_on_error) {
        try {
            if (loop_on_error == 1)
                Traverse(xn, &mainframe);
            else
                Traverse_in_error(xn, &mainframe, currentline);
        }
        catch (TamguRaiseError* a) {
            global->threads[0].currentinstruction = NULL;
            global->lineerror = a->left;
            TamguFullError* err;
            long pos = -1;
            if (a->left != -1) {
                pos = 0;
                while (pos < xr.stackln.size() && xr.stackln[pos] < a->left) pos++;
                if (pos != xr.stackln.size())
                    err = new TamguFullError(a->message, a->filename, xr.bpos[pos] , xr.cpos[pos], a->left);
                else
                    err = new TamguFullError(a->message, a->filename, -1, -1, a->left);
            }
            else
                err = new TamguFullError(a->message, a->filename, -1, -1, a->left);
            
            errors.push_back(err);
            delete a;
            loop_on_error = 2;
            continue;
        }
        loop_on_error = 0;
    }

    global->Popstack();
    fullcompilemode = false;
    
    delete xn;
    return true;
}


bool TamguCode::Compile(string& body) {
    tokenizer_result<string> xr;
    bnf_tamgu bnf;

    InitWindowMode();
    
    global->threads[0].message.str("");
    global->threads[0].message.clear();
    
	//we store our TamguCode also as an Tamgutamgu...
	filename = NormalizeFileName(filename);
    Tamgu* main = TamguRecordFile(filename, this, global);
    
    global->CreateSystemVariable(main, a_mainframe, a_tamgu);

	global->spaceid = idcode;

    xr.lispmode = lispmode;

    if (body[0] == '(' && body[1] == ')') {
        xr.lispmode = true;
        body[0] = '/';
        body[1] = '/';
    }
    
    std::chrono::high_resolution_clock::time_point before;
    std::chrono::high_resolution_clock::time_point after;

    before = std::chrono::high_resolution_clock::now();
    
    body += "\n";
    global->tamgu_tokenizer.tokenize<string>(body, xr);
    
    after = std::chrono::high_resolution_clock::now();
    double dtok = std::chrono::duration_cast<std::chrono::milliseconds>( after - before ).count();
            
    if (!xr.size())
        return false;
    
	global->lineerror = -1;


    x_node* xn;
    if (xr.lispmode) {
        bnf.initialize(&xr);
        bnf.baseline = global->linereference;
        string lret;
        xn = new x_node;
        if (bnf.m_tamgupurelisp(lret, &xn) != 1 || bnf.currentpos != bnf.fx->stack.size()) {
            delete xn;
            cerr << " in " << filename << endl;
            stringstream& message = global->threads[0].message;
            global->lineerror = bnf.lineerror;
            currentline = global->lineerror;
            message << "Error while parsing program file: ";
            if (bnf.errornumber != -1)
                message << bnf.x_errormsg(bnf.errornumber);
            else
                message << bnf.labelerror;

            global->Returnerror(message.str(), global->GetThreadid());
            return false;
        }
    }
    else {
        bnf.baseline = global->linereference;
        xn = bnf.x_parsing(&xr, FULL);
    }
    
    if (xn == NULL) {
        cerr << " in " << filename << endl;
        stringstream& message = global->threads[0].message;
        global->lineerror = bnf.lineerror;
        currentline = global->lineerror;
        message << "Error while parsing program file: ";
        if (bnf.errornumber != -1)
            message << bnf.x_errormsg(bnf.errornumber);
        else
            message << bnf.labelerror;
        global->Returnerror(message.str(), global->GetThreadid());
        return false;
    }
    
	firstinstruction = mainframe.instructions.size();
    global->currentbnf = &bnf;

    before = std::chrono::high_resolution_clock::now();
	global->Pushstack(&mainframe);
	try {
		Traverse(xn, &mainframe);
	}
	catch (TamguRaiseError* a) {
		global->threads[0].currentinstruction = NULL;
		global->lineerror = a->left;
		global->threads[0].message.str("");
        global->threads[0].message.clear();
		global->threads[0].message << a->message;
		if (a->message.find(a->filename) == string::npos)
			global->threads[0].message << " in " << a->filename;

		if (global->errorraised[0] == NULL)
			global->errorraised[0] = new TamguError(global->threads[0].message.str());
		else
			global->errorraised[0]->error = global->threads[0].message.str();

		global->errors[0] = true;
		TamguCode* c = global->Getcurrentcode();
		if (c->filename != a->filename)
			c->filename = a->filename;
		delete a;
		delete xn;
		global->Popstack();
		return false;
	}

	global->Popstack();

    after = std::chrono::high_resolution_clock::now();
    double dparse = std::chrono::duration_cast<std::chrono::milliseconds>( after - before ).count();

    TamguSystemVariable* vs = globalTamgu->systems[globalTamgu->Getid("_internals")];
    vs->value->storevalue((long)xr.stack.size());
    vs->value->storevalue(dtok);
    vs->value->storevalue(dparse);

	delete xn;
	return true;
}


Tamgu* TamguCode::Compilefunction(string& body, short idthread) {
	//we store our TamguCode also as an Tamgutamgu...
	static bnf_tamgu bnf;
    static tokenizer_result<string> xr;
    
    global->threads[0].message.str("");
    global->threads[0].message.clear();
    
    Locking _lock(global->_parselock);
    
    bnf.baseline = global->linereference;
    global->tamgu_tokenizer.tokenize<string>(body, xr);
    if (xr.size() == 0) {
        cerr << " in " << filename << endl;
        stringstream message;
        message << "Empty body" << endl;
        return global->Returnerror(message.str(), idthread);
    }

	global->lineerror = -1;

	x_node* xn = bnf.x_parsing(&xr, FULL);
	if (xn == NULL) {
		cerr << " in " << filename << endl;
		stringstream& message = global->threads[0].message;
		global->lineerror = bnf.lineerror;
		currentline = global->lineerror;
		message << "Error while parsing program file: ";
		if (bnf.errornumber != -1)
			message << bnf.x_errormsg(bnf.errornumber);
		else
			message << bnf.labelerror;

        return global->Returnerror(message.str(), idthread);
	}

	global->currentbnf = &bnf;
	firstinstruction = mainframe.instructions.size();

	global->Pushstack(&mainframe, idthread);
	Tamgu* compiled = NULL;
	try {
		compiled = Traverse(xn, &mainframe);
	}
	catch (TamguRaiseError* a) {
        global->threads[0].message.str("");
        global->threads[0].message.clear();
		global->threads[0].message << a->message;
		if (a->message.find(a->filename) == string::npos)
			global->threads[0].message << " in " << a->filename;

		if (global->errorraised[0] == NULL)
			global->errorraised[0] = new TamguError(global->threads[0].message.str());
		else
			global->errorraised[0]->error = global->threads[0].message.str();

		global->errors[0] = true;
		TamguCode* c = global->Getcurrentcode();
		if (c->filename != a->filename)
			c->filename = a->filename;
		delete a;
		delete xn;
		global->Popstack();
		return global->errorraised[0];
	}

	global->Popstack(idthread);

	delete xn;
	return compiled;
}

Tamgu* TamguCode::CompileExpression(string& body, short idthread) {
    //we store our TamguCode also as an Tamgutamgu...
    static bnf_tamgu bnf;
    static tokenizer_result<string> xr;
    
    global->threads[0].message.str("");
    global->threads[0].message.clear();
    
    Locking _lock(global->_parselock);
    
    bnf.baseline = global->linereference;
    global->tamgu_tokenizer.tokenize<string>(body, xr);
    if (xr.size() == 0) {
        cerr << " in " << filename << endl;
        stringstream message;
        message << "Empty body" << endl;
        return global->Returnerror(message.str(), idthread);
    }

    global->lineerror = -1;

    x_node* xn = bnf.x_parsing(&xr, FULL);
    if (xn == NULL) {
        cerr << " in " << filename << endl;
        stringstream& message = global->threads[0].message;
        global->lineerror = bnf.lineerror;
        currentline = global->lineerror;
        message << "Error while parsing program file: ";
        if (bnf.errornumber != -1)
            message << bnf.x_errormsg(bnf.errornumber);
        else
            message << bnf.labelerror;

        return global->Returnerror(message.str(), idthread);
    }

    global->currentbnf = &bnf;
    firstinstruction = mainframe.instructions.size();

    Tamgu* topstack = global->Topstack(idthread);

    Tamgu* compiled = NULL;
    try {
        compiled = Traverse(xn, topstack);
    }
    catch (TamguRaiseError* a) {
        global->threads[0].message.str("");
        global->threads[0].message.clear();
        global->threads[0].message << a->message;
        if (a->message.find(a->filename) == string::npos)
            global->threads[0].message << " in " << a->filename;

        if (global->errorraised[0] == NULL)
            global->errorraised[0] = new TamguError(global->threads[0].message.str());
        else
            global->errorraised[0]->error = global->threads[0].message.str();

        global->errors[0] = true;
        TamguCode* c = global->Getcurrentcode();
        if (c->filename != a->filename)
            c->filename = a->filename;
        delete a;
        delete xn;
        return global->errorraised[0];
    }

    delete xn;
    return compiled;
}

Tamgu* TamguCode::CompileFormat(string& body, Tamgu* parent) {
    //we store our TamguCode also as an Tamgutamgu...
    static tokenizer_result<string> xr;
    bnf_tamgu& bnf = *global->currentbnf;

    global->tamgu_tokenizer.tokenize<string>(body, xr);

    global->lineerror = -1;

    x_node* xn = bnf.x_parsing(&xr, FULL);
    if (xn == NULL) {
        cerr << " in " << filename << endl;
        stringstream& message = global->threads[0].message;
        global->lineerror = bnf.lineerror;
        currentline = global->lineerror;
        message << "Error while parsing program file: ";
        if (bnf.errornumber != -1)
            message << bnf.x_errormsg(bnf.errornumber);
        else
            message << bnf.labelerror;

        return globalTamgu->Returnerror(message.str(), 0);
    }


    Tamgu* compiled = NULL;
    try {
        compiled = Traverse(xn, parent);
    }
    catch (TamguRaiseError* a) {
        global->threads[0].message.str("");
        global->threads[0].message.clear();
        global->threads[0].message << a->message;
        if (a->message.find(a->filename) == string::npos)
            global->threads[0].message << " in " << a->filename;

        if (global->errorraised[0] == NULL)
            global->errorraised[0] = new TamguError(global->threads[0].message.str());
        else
            global->errorraised[0]->error = global->threads[0].message.str();

        global->errors[0] = true;
        TamguCode* c = global->Getcurrentcode();
        if (c->filename != a->filename)
            c->filename = a->filename;
        delete a;
        delete xn;
        return global->errorraised[0];
    }

    delete xn;
    return compiled;
}


