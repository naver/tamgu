/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : jagrgx.cxx
 Date       : 2019/03/25
 Purpose    : regular expression implementation for find/replace commands
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#include "conversion.h"
#include "x_node.h"
#include "jagrgx.h"

static Au_automatons* gAutomatons = NULL;

void x_reading::apply(bool keepos, vector<string>* vstack, vector<unsigned char>* vtype) {
    if (!loaded) {
        setrules();
        parserules();
        loaded=true;
    }

    bool storetype=true;
    if (vstack==NULL)
        vstack=&stack;

    if (vtype==NULL) {
        vtype=&stacktype;
        storetype=false;
    }

    vector<string> token_opt;
    vector<string> chr_opt;
    VECTE<long> b_opt;
    VECTE<long> c_opt;
    VECTE<short> lastopt;

    string tmp;

    string chr;
    string currentchr;
    string label;
    string token;

    size_t b, c, bo, co;
    long r,sz,ni=0,k,line=0,i, l;

    short opt=0;
    short skip;
    short type;
    short ty;

    char nxt;
    char esc_char;

    bool getit=false;
    bool found=true;

    parcours.begin();
    currentchr=parcours.next(line);

    while (!parcours.end() || currentchr != "") {
        parcours.getpos(b,c);
        getit=false;
        i=table[(uchar)currentchr[0]];
        if (i==255) //this is not a character, which a rule is indexed for, we jump to the first non character rules...
            i=firstrule;
        else {
            if (verif(ruleelements[i][0],xr_singlebody)) {
                    //if the rule only checks one character, and it is a direct check, we can stop there
                ty = action[i];
                if (ty != -1) {
                    vstack->push_back(currentchr);

                    if (!juststack) {
                        stackln.push_back(line);
                        vtype->push_back(ty);
                        if (keepos) {
                            bpos.push_back(b-1);
                            cpos.push_back(c-1);
                        }
                    }
                    else
                        if (storetype)
                            vtype->push_back(ty);

                }
                currentchr=parcours.next(line);
                continue;
            }
        }

        for (;i<tokenizer.size();i++) {
            if (action[i]==xr_skiprule)
                continue;

            vector<string>& rule=tokenizer[i];
            sz=rule.size();

            opt=0;
            skip=0;

            l=line;
            found=true;

            token="";
            chr=currentchr;
            for (r=0;r<sz;r++) {
                label=rule[r];
                    //There was an error while parsing the content of an optional sequence...
                if (skip>0) {
                    while (r<sz && skip) { //we try to find the closing parenthesis that matches the current context
                        type=ruleelements[i][r];
                        if (verif(type,xr_optional))
                            skip++;
                        else
                            if (verif(type,xr_endoptional))
                                skip--;
                        ++r;
                    }

                    r--;
                    opt--;
                    parcours.setpos(b_opt.back(),c_opt.back());
                    chr=chr_opt.back();

                    token_opt.pop_back();
                    lastopt.pop_back();
                    b_opt.pop_back();
                    c_opt.pop_back();
                    chr_opt.pop_back();

                    continue;
                }

                type=ruleelements[i][r];
                if (r) {
                    if (verif(type,xr_endoptional)) {
                        opt--;
                        if (token_opt.size()>1) {
                            tmp=token_opt.back();
                            token_opt.pop_back();
                            token_opt.back()+=tmp;
                        }
                        else {
                            token+=token_opt.back();
                            token_opt.pop_back();
                        }
                        if (verif(type,xr_plus)) //if we can loop, we try...
                            r=lastopt.back()-1;

                        lastopt.pop_back();
                        b_opt.pop_back();
                        c_opt.pop_back();
                        chr_opt.pop_back();
                        continue;
                    }

                    if (verif(type,xr_optional)) {
                        parcours.getpos(bo,co);

                        b_opt.push_back(bo);
                        c_opt.push_back(co);
                        chr_opt.push_back(chr);
                        token_opt.push_back("");
                        lastopt.push_back(r);

                        opt++;
                        continue;
                    }
                }

                esc_char=check(label,type, chr);
                if (esc_char == false) {
                    if (opt) {
                        skip=1;
                        continue;
                    }
                    else {
                            //if we expected a character and its recognition failed, we jump to rules...
                        if (!r && verif(type,xr_char))
                            i=firstrule-1;
                        found=false;
                        break;
                    }
                }

                if (esc_char == 2) {
                    if (opt)
                        token_opt.back()+=chr;
                    else
                        token+=chr;
                    chr=parcours.next(l);
                }

                if (!verif(type,xr_skip)) { //do not store this character
                    if (opt)
                        token_opt.back()+=chr;
                    else
                        token+=chr;
                }

                chr=parcours.next(l);

                if (verif(type,xr_singlebody)) //this is a single body rule, we can stop here...
                    break;

                if (verif(type,xr_plus)) {
                    nxt=0;
                    if (verif(type,xr_neednext)) {
                        ni=r+1;
                        while (ni<sz) {
                            if (!verif(ruleelements[i][ni],xr_char) && !verif(ruleelements[i][ni],xr_meta))
                                break;
                            nxt++;
                            ni++;
                        }
                    }

                    while ((esc_char=check(label,type,chr))) {
                        if (esc_char==2) {
                            if (opt) //we have an esc_char character detected
                                token_opt.back()+=chr;
                            else
                                token+=chr;
                            chr=parcours.next(l); //the next character should be copied without further analysis
                            if (opt)
                                token_opt.back()+=chr;
                            else
                                token+=chr;
                            chr=parcours.next(l); //then we jump to the next
                            continue; //we loop to check if this character is right
                        }

                        if (nxt) {
                            if (nxt==1) {
                                if (check(rule[r+1],ruleelements[i][r+1],chr))
                                    break;
                            }
                            else {
                                size_t bp, cp;
                                parcours.getpos(bp,cp);
                                string cc=chr;
                                bool f=true;
                                for (k=r+1;k<ni;k++) {
                                    if (!check(rule[k],ruleelements[i][k],cc)) {
                                        f=false;
                                        break;
                                    }
                                    cc=parcours.next();
                                }

                                parcours.setpos(bp,cp);
                                if (f)
                                    break;
                            }
                        }
                        if (opt)
                            token_opt.back()+=chr;
                        else
                            token+=chr;
                        chr=parcours.next(l);
                    }
                }
            }
            if (found) {
                ty=action[i];
                if (ty != -1) {
                    vstack->push_back(token);
                    if (!juststack) {
                        stackln.push_back(line);
                        vtype->push_back(ty);
                        if (keepos) {
                            bpos.push_back(b-currentchr.size());
                            cpos.push_back(c-1);
                        }
                    }
                    else
                        if (storetype)
                            vtype->push_back(ty);
                }
                getit=true;
                currentchr=chr;
                line=l;
                break;
            }
            parcours.setpos(b,c);
        }

        if (!getit) { //Character not taken into account by a rule, we suppose it is a simple UTF8 character...
            vstack->push_back(currentchr);
            stackln.push_back(line);
            vtype->push_back(255);
            currentchr=parcours.next();
        }
    }
}

void x_wreading::apply(wstring& toparse, bool keepos, vector<wstring>* vstack, vector<unsigned char>* vtype) {
    if (!loaded) {
        setrules();
        parserules();
        loaded=true;
    }

    bool storetype=true;
    if (vstack==NULL)
        vstack=&stack;

    if (vtype==NULL) {
        vtype=&stacktype;
        storetype=false;
    }

    vector<wstring> token_opt;

    vector<wstring> chr_opt;
    VECTE<long> c_opt;
    VECTE<short> lastopt;

    wstring tmp;

    wstring chr;
    wstring currentchr;
    wstring label;
    wstring token;

    long r,sz,ni=0,k,line=0,i, l;
    long pos=0, posc;

    short opt=0, skip=0;
    short type;
    short ty;
    char nxt;
    char esc_char;

    bool getit=false;
    bool found=true;

    currentchr=next(toparse,pos,line);
    long wsz=toparse.size();
    while (pos < wsz || currentchr != L"") {
        getit=false;
        posc=pos;
        if (currentchr[0]>=256)
            i=firstrule;
        else {
            i=table[(uchar)currentchr[0]];
            if (i==255) //this is not a character, which a rule is indexed for, we jump to the first non character rules...
                i=firstrule;
            else {
                if (verif(ruleelements[i][0],xr_singlebody)) {
                        //if the rule only checks one character, and it is a direct check, we can stop there
                    ty = action[i];
                    if (ty != -1) {
                        vstack->push_back(currentchr);

                        if (!juststack) {
                            stackln.push_back(line);
                            vtype->push_back(ty);
                            if (keepos) {
                                cpos.push_back(pos-1);
                            }
                        }
                        else
                            if (storetype)
                                vtype->push_back(ty);

                    }
                    currentchr=next(toparse,pos,line);
                    continue;
                }
            }
        }
        for (;i<tokenizer.size();i++) {
            if (action[i]==xr_skiprule)
                continue;

            vector<wstring>& rule=tokenizer[i];
            sz=rule.size();

            opt=0;
            skip=0;

            l=line;
            found=true;

            token=L"";
            chr=currentchr;
            for (r=0;r<sz;r++) {
                label=rule[r];

                if (skip>0) {
                    while (r<sz && skip) {
                        type=ruleelements[i][r];
                        if (verif(type,xr_optional))
                            skip++;
                        else
                            if (verif(type,xr_endoptional))
                                skip--;
                        ++r;
                    }

                    r--;
                    opt--;
                    posc=c_opt.back();
                    chr=chr_opt.back();

                    token_opt.pop_back();
                    lastopt.pop_back();
                    c_opt.pop_back();
                    chr_opt.pop_back();
                    continue;
                }

                type=ruleelements[i][r];

                if (r) {
                    if (verif(type,xr_endoptional)) {
                        opt--;
                        if (token_opt.size()>1) {
                            tmp=token_opt.back();
                            token_opt.pop_back();
                            token_opt.back()+=tmp;
                        }
                        else {
                            token+=token_opt.back();
                            token_opt.pop_back();
                        }
                        if (verif(type,xr_plus))
                            r=lastopt.back()-1;
                        lastopt.pop_back();
                        c_opt.pop_back();
                        chr_opt.pop_back();

                        continue;
                    }


                    if (verif(type,xr_optional)) {

                        c_opt.push_back(posc);
                        token_opt.push_back(L"");
                        lastopt.push_back(r);
                        chr_opt.push_back(chr);
                        opt++;

                        continue;
                    }
                }

                esc_char=check(label,type, chr);
                if (esc_char == false) {
                    if (opt) {
                        skip=1;
                        continue;
                    }
                    else {
                            //if we expected a character and its recognition failed, we jump to rules...
                        if (!r && verif(type,xr_char))
                            i=firstrule-1;
                        found=false;
                        break;
                    }
                }

                if (esc_char == 2) {
                    if (opt)
                        token_opt.back()+=chr;
                    else
                        token+=chr;
                    chr=next(toparse,posc,l);
                }

                if (!verif(type,xr_skip)) { //do not store this character
                    if (opt)
                        token_opt.back()+=chr;
                    else
                        token+=chr;
                }

                chr=next(toparse,posc,l);

                if (verif(type,xr_singlebody)) //this is a single body rule, we can stop here...
                    break;

                if (verif(type,xr_plus)) {
                    nxt=0;
                    if (verif(type,xr_neednext)) {
                        ni=r+1;
                        while (ni<sz) {
                            if (!verif(ruleelements[i][ni],xr_char) && !verif(ruleelements[i][ni],xr_meta))
                                break;
                            nxt++;
                            ni++;
                        }
                    }

                    while ((esc_char=check(label,type,chr))) {
                        if (esc_char==2) {
                            if (opt) //we have an esc_char character detected
                                token_opt.back()+=chr;
                            else
                                token+=chr;
                            chr=next(toparse,posc,l); //the next character should be copied without further analysis
                            if (opt)
                                token_opt.back()+=chr;
                            else
                                token+=chr;
                            chr=next(toparse,posc,l); //then we jump to the next
                            continue; //we loop to check if this character is right
                        }

                        if (nxt) {
                            if (nxt==1) {
                                if (check(rule[r+1],ruleelements[i][r+1],chr))
                                    break;
                            }
                            else {
                                long cp = posc;
                                wstring cc=chr;
                                bool f=true;
                                for (k=r+1;k<ni;k++) {
                                    if (!check(rule[k],ruleelements[i][k],cc)) {
                                        f=false;
                                        break;
                                    }
                                    cc=toparse[cp++];
                                }

                                if (f)
                                    break;
                            }
                        }
                        if (opt)
                            token_opt.back()+=chr;
                        else
                            token+=chr;
                        chr=next(toparse,posc,l);
                    }
                }
            }
            if (found) {
                ty=action[i];
                if (ty != -1) {
                    vstack->push_back(token);
                    if (!juststack) {
                        stackln.push_back(line);
                        vtype->push_back(ty);
                        if (keepos) {
                            cpos.push_back(pos-1);
                        }
                    }
                    else
                        if (storetype)
                            vtype->push_back(ty);
                }
                getit=true;
                currentchr=chr;
                line=l;
                pos=posc;
                break;
            }
            posc=pos;
        }

        if (!getit) { //Character not taken into account by a rule, we suppose it is a simple UTF8 character...
            vstack->push_back(currentchr);
            stackln.push_back(line);
            vtype->push_back(255);
            currentchr=next(toparse,pos,l);
        }
    }
}
//---------------------------------------AUTOMATON------------------------------------------------------------
static bool tokenize(wstring& rg, vector<wstring>& stack, vector<aut_actions>& vtypes) {
    wstring sub;
    long sz = rg.size();
    uchar type=aut_reg;
    long inbracket = 0;

    for (long i = 0; i < sz; i++) {
        switch (rg[i]) {
            case 10:
            case 13:
                continue;
            case '\\': //escape character
                if ((i + 1) < sz) {
                    sub = rg[++i];
                    //we accept: \ddd, exactly three digits
                    if (c_is_digit(rg[i]) && (i+2)<sz && c_is_digit(rg[i+1]) && c_is_digit(rg[i+2])) {
                        sub+=rg[i+1];
                        sub+=rg[i+2];
                        i+=2;
                        sub = (wchar_t)convertinteger(sub);
                    }
                    else //we also accept four hexa: \xFFFF
                        if (rg[i] == 'x' && (i+4)<sz && c_is_hexa(rg[i+1]) && c_is_hexa(rg[i+2]) && c_is_hexa(rg[i+3]) && c_is_hexa(rg[i+4])) {
                            sub=L"0x";
                            sub+=rg[i+1];
                            sub+=rg[i+2];
                            sub+=rg[i+3];
                            sub+=rg[i+4];
                            i+=4;
                            sub = (wchar_t)convertinteger(sub);
                        }
                    type = aut_reg;
                }
                break;
            case '%':
                sub = L"%";
                if ((i + 1) < sz) {
                    type = aut_meta;
                    i++;
                    sub += rg[i];
                    //Specific for emojis... we insert [%e%E*]
                    if (sub == L"%e") {
                        sub = L'[';
                        type = aut_obrk;
                        stack.push_back(sub);
                        vtypes.push_back((aut_actions)type);
                        stack.push_back(L"%e");
                        vtypes.push_back(aut_meta);
                        stack.push_back(L"%E*");
                        type = aut_meta + 2;
                        vtypes.push_back((aut_actions)type);
                        type = aut_cbrk;
                        sub = L']';
                    }
                }
                break;
            case '~': // negation of the next character
                sub = L'~';
                type = aut_negation;
                break;
            case '?':
                sub = L'?';
                type = aut_any;
                break;
            case '(':
                sub = L'(';
                type = aut_opar;
                break;
            case ')':
                stack.push_back(L")");
                vtypes.push_back(aut_cpar);
                continue; //we cannot have: ..)+ or ..)*
            case '[':
                sub = L'[';
                type = aut_obrk;
                inbracket++;
                break;
            case ']':
                sub = L']';
                type = aut_cbrk;
                inbracket--;
                break;
            case '{': //curly bracket
                sub = L'{';
                type = aut_ocrl_brk;
                inbracket++;
                break;
            case '}':
                sub = L'}';
                type = aut_ccrl_brk;
                inbracket--;
                break;
            case '-':
                if (inbracket && i < sz-1 && vtypes.back() == aut_reg) {
                    //{a-z} or [a-z]
                    //in that case, we build the character list between the current character and the next one...
                    sub = stack.back();
                    wstring nxt;
                    nxt = rg[++i];
                    ++sub[0];
                    while (sub[0] <= nxt[0]) {
                        stack.push_back(sub);
                        vtypes.push_back(aut_reg);
                        ++sub[0];
                    }
                    continue;
                }
            default:
                sub = rg[i];
                type = aut_reg;
        }

        if ((i + 1) < sz) {
            if (rg[i + 1] == L'+') {
                i++;
                type += 1;
                sub += rg[i];
            }
            else {
                if (rg[i + 1] == L'*') {
                    i++;
                    type += 2;
                    sub += rg[i];
                }
            }
        }

        stack.push_back(sub);
        vtypes.push_back((aut_actions)type);
    }

    if (inbracket)
        return false;

    return true;
}
//----------------------------------------------------------------
//an epsilon, which points to a final state with no arcs attached
bool Au_arc::checkfinalepsilon() {
    if (action->Type() == an_epsilon && state->isend() && !state->arcs.last)
        return true;
    return false;
}

void Au_state::removeepsilon() {
    if (mark)
        return;

    mark=true;
    vector<long> removals;
    long i;
    for (i=0;i<arcs.size();i++) {
        if (arcs[i]->checkfinalepsilon()) {
            //We can remove it...
            removals.push_back(i);
        }
        else
            arcs[i]->state->removeepsilon();
    }

    for (i = removals.size()-1; i>=0; i--) {
        status|=an_end;
        arcs.erase(removals[i]);
    }
}
//----------------------------------------------------------------

void Au_state::addrule(Au_arc* r) {
    if (mark)
        return;

    mark=true;
    //This is a terminal node...
    if (isend())
        arcs.push_back(r);

    for (long i=0;i<arcs.size();i++)
        arcs[i]->state->addrule(r);

    return;
}

//----------------------------------------------------------------
//A bit of explanation... If the string is UTF16, then getachar can modify "i", when it returns
//This is why we need to keep the current position before calling the function
//In the case of 32 bits wstring, this variable is useless... i won't budge after a call to getachar...
bool Au_state::match(wstring& w, long i) {
    if (i==w.size()) {
        if (isend())
            return true;
        return false;
    }

    TAMGUCHAR c = getachar(w, i);

    for (long j=0;j<arcs.last;j++) {
        switch(arcs[j]->action->compare(c)) {
            case 0:
                break;
            case 1:
                if (arcs[j]->state->match(w,i+1))
                    return true;
                break;
            case 2:
                if (arcs[j]->state->match(w,i))
                    return true;
        }
    }

    return false;
}

bool Au_automaton::match(string& v) {
    wstring w;
    s_utf8_to_unicode(w, USTR(v), v.size());
    return first->match(w,0);
}

bool Au_automaton::match(wstring& w) {
    return first->match(w,0);
}

//----------------------------------------------------------------

void Au_state::storerulearcs(hmap<long,bool>& rules) {
    if (mark)
        return;

    if (isrule())
        rules[idx()]=true;

    mark=true;
    for (long j=0;j<arcs.last;j++)
        arcs[j]->state->storerulearcs(rules);
    mark=false;
}

bool Au_arc::get(wstring& w, long i, hmap<long,bool>& rules) {
    if (i==w.size())
        return false;

    TAMGUCHAR c = getachar(w, i);

    switch(action->compare(c)) {
        case 0:
            return false;
        case 1:
            return state->get(w,i+1,rules);
        case 2:
            return state->get(w,i,rules);
    }
    return false;
}

bool Au_state::get(wstring& w, long i, hmap<long,bool>& rules) {
    long j;

    if (negation) {
        for (j=0;j<arcs.last;j++) {
            if (arcs[j]->get(w,i,rules))
                return false;
        }

        for (j=0;j<arcs.last;j++) {
            if (arcs[j]->state->get(w,i+1,rules))
                return true;
        }
    }
    else {
        for (j=0;j<arcs.last;j++) {
            if (arcs[j]->get(w,i,rules))
                return true;
        }
    }

    if (i==w.size() && isend()) {
        storerulearcs(rules);
        return true;
    }
    return false;
}

bool Au_automaton::get(wstring& w, hmap<long,bool>& rules) {
    if (first == NULL)
        return false;
    rules.clear();
    return first->get(w,0,rules);
}
//----------------------------------------------------------------

long Au_state::loop(wstring& w, long i) {
    if (i==w.size()) {
        if (isend())
            return i;
        return -1;
    }

    if (status & an_beginning) {
        if (i)
            return -1;
    }

    long l=-1;
    long j;

    TAMGUCHAR c = getachar(w, i);

    if (negation) {
        //All is 0 or all is 1...
        for (j=0;j<arcs.last;j++) {
            switch(arcs[j]->action->compare(c)) {
                case 0:
                    continue;
                case 1:
                    return -1;
                case 2:
					l = arcs[j]->state->loop(w, i);
                    if (l!=-1)
                        return -1;
            }
        }

        for (j=0;j<arcs.last;j++) {
            if (!arcs[j]->action->compare(c)) {
                l=arcs[j]->state->loop(w,i+1);
                if (l != -1)
                    return l;
            }
        }

        if (isend())
            return i;

        return -1;
    }

    for (j=0;j<arcs.last;j++) {
        switch(arcs[j]->action->compare(c)) {
            case 0:
                l=-1;
                continue;
            case 1:
                l=arcs[j]->state->loop(w,i+1);
                break;
            case 2:
				l = arcs[j]->state->loop(w, i);
        }
        if (l!=-1)
            return l;
    }


    if (isend()) {
        if (status & an_ending) {
            if (i != w.size())
                return -1;
        }
        return i;
    }

    return -1;
}

long Au_automaton::find(wstring& w) {
    long sz = w.size();
    for (long f=0;f<sz;f++) {
        if (first->loop(w,f)!=-1)
            return f;
    }
    return -1;
}

long Au_automaton::find(wstring& w, long i) {
    long sz = w.size();
    for (long f = i ; f < sz; f++) {
        if (first->loop(w,f)!=-1)
            return f;
    }
    return -1;
}

bool Au_automaton::search(wstring& w) {
    long sz = w.size();
    for (long f=0;f<sz;f++) {
        if (first->loop(w,f)!=-1)
            return true;
    }
    return false;
}

bool Au_automaton::search(wstring& w, long& f, long& l, long init) {
    long sz = w.size();
    for (f=init;f<sz;f++) {
        l=first->loop(w,f);
        if (l!=-1)
            return true;
    }
    f=-1;
    return false;
}

bool Au_automaton::search(string& s, long& f, long& l, long init) {
    wstring w;
    long sz = s.size();
    s_utf8_to_unicode(w, USTR(s), sz);
    if (init)
        init = c_bytetocharposition(USTR(s), init);
    if (search(w, f, l, init)) {
        long fc=f;
        f = c_chartobyteposition(USTR(s),sz, f);
        //we only need to scan the characters between f and l
        l = f + c_chartobyteposition(USTR(s)+f, sz-f, l-fc);
        return true;
    }
    return false;
}

bool Au_automaton::searchc(string& s, long& f, long& l, long& fc, long init) {
    wstring w;
    long sz = s.size();
    s_utf8_to_unicode(w, USTR(s), sz);
    if (init)
        init = c_bytetocharposition(USTR(s), init);
    if (search(w, f, l, init)) {
        fc=f;
        f = c_chartobyteposition(USTR(s),sz, f);
        //we only need to scan the characters between f and l
        l = f + c_chartobyteposition(USTR(s)+f, sz-f, l-fc);
        return true;
    }
    return false;
}

//The string is not a UTF8 string, no need for char to byte position conversion, nor to complex string conversion
bool Au_automaton::searchraw(string& s, long& f, long& l, long init) {
    wstring w;
    long sz = s.size();
    w.reserve(sz);
    for (long i =0; i< sz; i++)
        w +=  (wchar_t)(uchar)s[i] ;

    if (search(w, f, l, init))
        return true;
    return false;
}

bool Au_automaton::searchlast(wstring& w, long& b, long& e, long init) {
    b=-1;
    long l;
    long sz = w.size();
    for (long f=init;f<sz;f++) {
        l=first->loop(w,f);
        if (l!=-1) {
            b=f;
            e=l;
            f=l-1;
        }
    }

    if (b!=-1)
        return true;
    return false;
}

bool Au_automaton::searchlast(string& s, long& f, long& l, long init) {
    wstring w;
    long sz = s.size();

    s_utf8_to_unicode(w, USTR(s), sz);
    if (init)
        init = c_bytetocharposition(USTR(s), init);

    if (searchlast(w, f, l, init)) {
        long fc = f;
        f = c_chartobyteposition(USTR(s),sz, f);
        //we only need to scan the characters between f and l
        l = f + c_chartobyteposition(USTR(s)+f, sz-f, l-fc);
        return true;
    }

    return false;
}

bool Au_automaton::searchlastc(string& s, long& f, long& l, long& fc, long init) {
    wstring w;
    long sz = s.size();

    s_utf8_to_unicode(w, USTR(s), sz);
    if (init)
        init = c_bytetocharposition(USTR(s), init);

    if (searchlast(w, f, l, init)) {
        fc=f;
        f = c_chartobyteposition(USTR(s),sz, f);
        //we only need to scan the characters between f and l
        l = f + c_chartobyteposition(USTR(s)+f, sz-f, l-fc);
        return true;
    }

    return false;
}

//The string is not a UTF8 string, no need for char to byte position conversion, nor to complex string conversion
bool Au_automaton::searchlastraw(string& s, long& f, long& l, long init) {
    wstring w;
    long sz = s.size();
    w.reserve(sz);
    for (long i =0; i< sz; i++)
        w +=  (wchar_t)(uchar)s[i] ;

    return searchlast(w, f, l, init);
}

//----------------------------------------------------------------
void Au_automaton::searchall(wstring& w, vector<long>& res, long init) {
    long l;
    long sz = w.size();
    for (long f=init;f<sz;f++) {
        l=first->loop(w,f);
        if (l!=-1) {
            res.push_back(f);
            res.push_back(l);
            f=l-1;
        }
    }
}

void Au_automaton::searchall(string& s, vector<long>& res, long init) {
    wstring w;
    s_utf8_to_unicode(w, USTR(s), s.size());
    if (init)
        init = c_bytetocharposition(USTR(s), init);
    searchall(w, res, init);
    v_convertchartobyteposition(USTR(s), res);
}

//The string is not a UTF8 string, no need for char to byte position conversion
void Au_automaton::searchallraw(string& s, vector<long>& res, long init) {
    wstring w;
    long sz = s.size();
    w.reserve(sz);
    for (long i =0; i< sz; i++)
        w+=(wchar_t)(uchar)s[i];
    searchall(w, res, init);
}

//----------------------------------------------------------------
bool Au_arc::find(wstring& w, wstring& wsep, long i, vector<long>& res) {
    if (i==w.size())
        return false;

    TAMGUCHAR c = getachar(w, i);

    switch(action->compare(c)) {
        case 0:
            return false;
        case 1:
            if (w[i] == wsep[0])
                res.push_back(i+1);
            return state->find(w, wsep, i+1, res);
        case 2:
			return state->find(w, wsep, i, res);
    }
    return false;
}

bool Au_state::find(wstring& w, wstring& wsep, long i, vector<long>& res) {
    long ps=res.size();
    long j;

    if (status & an_beginning) {
        if (i)
            return -1;
    }

    if (negation) {
        for (j=0;j<arcs.last;j++) {
            if (arcs[j]->find(w, wsep, i, res))
                return false;

            while (ps != res.size()) {
                res.pop_back();
            }
        }

        for (j=0;j<arcs.last;j++) {
            res.push_back(i + 1);
            if (!arcs[j]->state->find(w, wsep, i + 1, res)) {
                while (ps != res.size()) {
                    res.pop_back();
                }
            }
            else
                return true;
        }
    }
    else {
        for (j=0;j<arcs.last;j++) {
            if (!arcs[j]->find(w, wsep, i, res)) {
                while (ps != res.size()) {
                    res.pop_back();
                }
            }
            else
                return true;
        }
    }

    if (isend()) {
        if (status & an_ending) {
            if (i != w.size())
                return false;
        }

        if (res[res.size()-1]!=i+1)
            res.push_back(i+1);
        return true;
    }

    return false;
}

void Au_automaton::find(string& s, string& sep, vector<long>& res) {
    wstring w;
    s_utf8_to_unicode(w, USTR(s), s.size());
    wstring wsep;
    s_utf8_to_unicode(wsep, USTR(sep), sep.size());
    first->find(w,wsep,0,res);
}

void Au_automaton::find(wstring& w, wstring& wsep, vector<long>& res) {
    first->find(w,wsep,0,res);
}

//----------------------------------------------------------------

void Au_state::merge(Au_state* a) {
    if (a->mark)
        return;
    a->mark=true;

    status |= a->status;

    long sz=arcs.size();
    for (long i=0;i<a->arcs.size();i++) {
        if (a->arcs[i]->state->isrule()) {
            arcs.push_back(a->arcs[i]);
            continue;
        }

        long found=-1;
        for (long j=0;j<sz;j++) {
            if (a->arcs[i]->same(arcs[j])) {
                found=j;
                break;
            }
        }
        if (found==-1)
            arcs.push_back(a->arcs[i]);
        else {
            arcs[found]->state->merge(a->arcs[i]->state);
            a->arcs[i]->state->mark=an_remove;
            a->arcs[i]->mark=an_remove;
        }
    }
}
//----------------------------------------------------------------

Au_automate::Au_automate(string rgx) {
    wstring wrgx;
    s_utf8_to_unicode(wrgx, USTR(rgx), rgx.size());
    first=NULL;
    parse(wrgx,&garbage);
}


Au_automate::Au_automate(wstring& wrgx) {
    first=NULL;
    parse(wrgx,&garbage);
}

Au_automaton::Au_automaton(string rgx) {
    wstring wrgx;
    s_utf8_to_unicode(wrgx, USTR(rgx), rgx.size());
    first=NULL;
    parse(wrgx);
}

bool Au_automaton::parse(wstring& w, Au_automatons* aus) {
    //static x_wautomaton xtok;
    //first we tokenize

    //First we detect the potential %X, where X is a macro

    vector<wstring> toks;
    vector<aut_actions> types;
    if (!tokenize(w,toks, types))
        return false;

    if (aus==NULL) {
        if (gAutomatons==NULL)
            gAutomatons=new Au_automatons;
        aus=gAutomatons;
    }

    if (first==NULL)
        first=aus->state();

    Au_state base;
    long ab,sb;
    aus->boundaries(sb,ab);

    if (base.build(aus, 0, toks,types,NULL)==NULL)
        return false;

    base.removeepsilon();
    base.mark=false;
    aus->clear(sb,ab);
    first->merge(&base);

    //we delete the elements that have been marked for deletion...
    aus->clean(sb,ab);
    return true;
}

bool Au_automate::compiling(wstring& w,long r) {
    //static x_wautomaton xtok;
    //first we tokenize

    vector<wstring> toks;
    vector<aut_actions> types;
    if (!tokenize(w,toks, types))
        return false;
    if (first==NULL)
        first=garbage.state();

    Au_state base;
    long ab,sb;
    garbage.boundaries(sb,ab);

    if (base.build(&garbage, 0, toks,types,NULL)==NULL)
        return false;

    Au_state* af=garbage.statefinal(r);
    Au_arc* fin=garbage.arc(new Au_epsilon(),af);
    base.addrule(fin);

    base.mark=false;
    garbage.clear(sb,ab);
    first->merge(&base);

    garbage.clean(sb,ab);
    return true;
}

//----------------------------------------------------------------------------------------

Au_state* Au_state::build(Au_automatons* aus, long i,vector<wstring>& toks, vector<aut_actions>& types, Au_state* common) {
    mark=false;
    Au_arc* ar;

    if (i==toks.size()) {
        status |= an_end;
        if (common != NULL) {
            ar=aus->arc(new Au_epsilon(), common);
            arcs.push_back(ar);
        }

        return this;
    }

    if (types[i] == aut_negation) {
        ++i;
        negation = true;
    }

    long j;
    bool stop;
    short count;
    vector<wstring> ltoks;
    vector<aut_actions> ltypes;
    Au_state* ret;

    switch(types[i]) {
        case aut_ocrl_brk: { //{..}
            i++;
            Au_state* commonend=aus->state();
            VECTE<Au_arc*> locals;
            stop=false;
            while (i<toks.size() && !stop) {
                switch(types[i]) {
                    case aut_ccrl_brk_plus: //}+
                    case aut_ccrl_brk_star: //}*
                    case aut_ccrl_brk: //}
                        stop=true;
                        break;
                    case aut_ocrl_brk:
                    case aut_obrk:
                    case aut_opar: { //a sub expression introduced with (), {} or []
                        uchar current_action=types[i]; // the current type...
                        uchar lbound=current_action+1;
                        uchar hbound=current_action+3;
                        if (current_action==aut_opar)
                            hbound=lbound; //the only value is aut_cpar...
                        ltoks.clear(); //we extract the sub-element itself...
                        ltypes.clear();
                        count=1;
                        ltoks.push_back(toks[i]);
                        ltypes.push_back(types[i]);
                        i++;
                        while (i<toks.size() && count) {
                            ltoks.push_back(toks[i]);
                            ltypes.push_back(types[i]);
                            if (types[i]==current_action) //which can other sub-elements of the same kind...
                                count++;
                            else {
                                if (types[i]>=lbound && types[i]<=hbound) //the stop value, with a control over the potential embedding...
                                    count--;
                            }
                            i++;
                        }
                        if (i==toks.size() || !ltoks.size()) //We could not find the closing character, this is an error...
                            return NULL;

                        Au_state s;
                        ret=s.build(aus, 0,ltoks,ltypes,commonend);
                        if (ret==NULL)
                            return NULL;

                        //It cannot be an end state, commonend will be...
                        ret->removeend();

                        for (j=0;j<s.arcs.last;j++) {
                            if (s.arcs[j]->state == commonend)
                                continue;

                            locals.push_back(s.arcs[j]);
                            arcs.push_back(s.arcs[j]);
                        }
                        break;
                    }
                    default:
                        ar=build(aus, toks[i],types[i],commonend);
                        if (ar==NULL)
                            return NULL;
                        locals.push_back(ar);
                        i++;
                }
            }

            if (i==toks.size())
                return NULL;

            if (types[i]==aut_ccrl_brk_plus || types[i]==aut_ccrl_brk_star) {//The plus and the star
                for (j=0;j<locals.size();j++)
                    commonend->arcs.push_back(locals[j]);
                if (types[i]==aut_ccrl_brk_star) {
                    ar=aus->arc(new Au_epsilon());
                    arcs.push_back(ar);
                    commonend->arcs.push_back(ar);
                    commonend=ar->state;
                }
            }
            return commonend->build(aus, i+1,toks,types,common);
        }
        case aut_opar: {//(..)
            i++;
            count=1;
            while (i<toks.size()) {
                if (types[i]==aut_opar) //embeddings
                    count++;
                else {
                    if (types[i]==aut_cpar) {
                        count--;
                        if (!count)
                            break;
                    }
                }
                ltoks.push_back(toks[i]);
                ltypes.push_back(types[i]);
                i++;
            }
            if (i==toks.size() || !ltoks.size())
                return NULL;
            ret=build(aus, 0,ltoks,ltypes,NULL);
            if (ret==NULL)
                return NULL;

            ret->removeend();
            //We jump...
            ar=aus->arc(new Au_epsilon(), ret);
            arcs.push_back(ar);
            //These are the cases, when we have a x* at the end of an expression...
            //The current node can be an end too
            return ret->build(aus, i+1,toks,types,common);
        }
        case aut_obrk: { //[..]
            i++;
            count=1;
            ltoks.clear();
            ltypes.clear();
            while (i<toks.size()) {
                if (types[i]==aut_obrk) //embeddings
                    count++;
                else {
                    if (types[i]==aut_cbrk_plus || types[i]==aut_cbrk_star || types[i]==aut_cbrk) {
                        count--;
                        if (!count)
                            break;
                    }
                }
                ltoks.push_back(toks[i]);
                ltypes.push_back(types[i]);
                i++;
            }
            if (i==toks.size() || !ltoks.size())
                return NULL;

            Au_state s;
            ret=s.build(aus, 0,ltoks,ltypes,NULL);
            if (ret==NULL)
                return NULL;

            ret->removeend();

            if (types[i]!=aut_cbrk) {//the plus
                //s is our starting point, it contains all the arcs we need...
                for (j=0;j<s.arcs.last;j++) {
                    arcs.push_back(s.arcs[j]);
                    ret->arcs.push_back(s.arcs[j]);
                } //we need to jump back to our position...

                if (types[i]==aut_cbrk_star) {//this is a star, we need an epsilon to escape it...
                    ar=aus->arc(new Au_epsilon(), ret);
                    arcs.push_back(ar);
                }
            }
            else {
                for (j=0;j<s.arcs.last;j++)
                    arcs.push_back(s.arcs[j]);
            }

            //These are the cases, when we have a x* at the end of an expression...
            //The current node can be an end too
            return ret->build(aus, i+1,toks,types,common);
        }
    }

    Au_state* next;
    if ((i+1)==toks.size())
        next=common;
    else
        next=NULL;

    if (toks[i] == L"^") {
        status |= an_beginning;
        return build(aus, i+1,toks,types,common);
    }

    if (toks[i] == L"$") {
        ret = build(aus, i+1,toks,types,common);
        ret->status |= an_ending;
        return ret;
    }

    Au_arc* retarc=build(aus, toks[i], types[i],next);
    if (retarc==NULL)
        return NULL;

    next = retarc->state->build(aus, i+1,toks,types,common);
    if (next != NULL && next->isend()) {
        //If the current element is a *, then it can be skipped up to the end...
        switch(types[i]) {
            case aut_meta_star:
            case aut_reg_star:
            case aut_any_star:
                status |= an_end;
        }
    }
    return next;
}

Au_arc* Au_state::build(Au_automatons* aus, wstring& token, uchar type, Au_state* common) {
    //First we scan the arcs, in case, it was already created...
    Au_any* a=NULL;

    //value arc: meta or character...
    switch(type) {
        case aut_any: //?
        case aut_any_plus: //?+
        case aut_any_star: //?*
            a=new Au_any(type);
            break;
        case aut_meta:
        case aut_meta_plus: //%x+
        case aut_meta_star: //%x*
            a=new Au_meta(token[1], type);
            break;
        case aut_reg:
        case aut_reg_plus: //x+
        case aut_reg_star: //x*
            a=new Au_char(token[0], type);
            break;
        default:
            return NULL;
    }

    //we check if we already have such an arc...
    for (long j=0;j<arcs.size();j++) {
        if (arcs[j]->action->same(a)) {
            delete a;
            arcs[j]->mark=false;
            return arcs[j];
        }
    }

    //Different case...
    //first if a is not NULL and a is a loop
    Au_arc* current=aus->arc(a, common);
    Au_arc* ar;
    switch(type) {
        case aut_meta_plus: //+
        case aut_reg_plus:
        case aut_any_plus:
            current->state->arcs.push_back(current); //the loop
            arcs.push_back(current);
            return current;
        case aut_meta_star: //*
        case aut_reg_star:
        case aut_any_star:
            ar=aus->arc(new Au_epsilon(),current->state);
            arcs.push_back(ar);
            current->state->arcs.push_back(current); //the loop
            arcs.push_back(current);
            return current;
        default:
            if (arcs.last>0) //We always insert our arc at the top to force its recognition before any loop...
                arcs.insert(0,current);
            else
                arcs.push_back(current);
    }
    return current;
}


