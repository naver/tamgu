/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : x_tokenize.h
 Date       : 2017/09/01
 Purpose    : Tokenize description classes 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef x_tokenize_h
#define x_tokenize_h
#include "x_node.h"

void getdefaultrules(vector<string>& rules);

class x_tokenize : public x_reading {
public:
    vector<short> points;
    vector<short> separators;
    
    void setrules();
    void resetrules(vector<string>& rules);
    
    void load() {
        if (!loaded) {
            setrules();
            parserules();
            loaded=true;
        }
    }
    
    void selectcomma(bool v) {
        long r;
        if (v) {
            action[separators[0]]=xr_skiprule;
            for (r=0;r<points.size();r+=2)
                tokenizer[points[r]][points[r+1]]=',';
        }
        else {
            action[separators[0]]=0;
            for (r=0;r<points.size();r+=2)
                tokenizer[points[r]][points[r+1]]='.';
        }
    }
    
    void keeprc(bool tr) {
        if (tr) {
            action[1]=0;
        }
        else {
            action[1]=-1;
        }
    }
    
    bool parseexternalrules(string& message) {
        /*
         The rules are parsed and the results is stored both in tokenizer and in ruleelements.
         
         ruleelements uses the following binary encoding to define an instruction
         
         xr_char (1)             -> regular character
         xr_meta (2)             -> meta-character (%x)
         xr_chardisjunction (4)  -> disjunction of characters ([..])
         xr_metadisjunction (8)  -> disjunction of metacharacters ({%...})
         xr_negation (16)        -> negation (~)
         xr_plus (32)            -> Kleene operator (+)
         xr_skip (64)            -> Character not stored (-)
         xr_neednext (128)       -> the metacharacter is a . or a ?
         xr_singlebody (256)     -> Single body rule (only one character)
         xr_optional (512)       -> Optional section
         xr_endoptional (1024)   -> End optional section
         
         -> IMPORTANT: rules that start with a regular character are also indexed with their first character in "table".
         rules that start with %d are also indexed on all possibile 10 digits.
         
         -> IMPORTANT: rules should be ordered with rules starting with a character first,
         THEN rules starting with a meta character (%x) or a disjunction after.
         
         -> VERY IMPORTANT: rules starting with a character should always next to the rules sharing the same first character...
         
         The variable firstrule records the position of the first rule starting with a meta-character...
         
         -> IMPORTANT: we keep tracks of rules that are reduced to one single character check in order to process them immediatly
         
         BEWARE: This parser DOES not check if RULES are VALID...
         
         */
        
        char x_actions[]="?aCcdHnopSsr.";
        
        string line;
        string equal("=");
        string res;
        string sub;
        
        long k,i, pos;
        
        short opening;
        
        char cc;
        char brk=']';
        char typebrk=xr_chardisjunction;
        bool aplus;
        bool neg=false;
        bool addfirstrule;
        unsigned char metakey;
        map<string,string> metalines;
        bool initmetakey=false;
        
        long ocurly = 0;
        long osquare = 0;
        long ccurly = 0;
        long csquare = 0;
        long opar = 0;
        long cpar = 0;
        bool err;

        for (i=0;i<rules.size();i++) {
            line=rules[i];
            vector<short> e;
            ruleelements.push_back(e);
            vector<string> rule;
            
            if (line[1]==':') { //we detect a meta-rule...
                metakey=line[0];
                line=line.c_str()+2;
                string key="%";
                key+=metakey;
                if (initmetakey)
                    replacemetas(metalines, line);
                
                metalines[key] = line;
                initmetakey=true;
                action.push_back(xr_skiprule);
                tokenizer.push_back(rule);
                continue;
            }
            
            neg=false;
            aplus=false;
            opening=0;
            metakey=0;
            
            ocurly = 0;
            osquare = 0;
            ccurly = 0;
            csquare = 0;
            opar = 0;
            cpar = 0;
            //first we check whether brackets are all balanced...
            for (pos =0; pos<line.size(); pos++) {
                if (line[pos]=='%') {
                    pos++;
                    continue;
                }
                switch(line[pos]) {
                    case '{':
                        ocurly++;
                        break;
                    case '}':
                        ccurly++;
                        break;
                    case '[':
                        osquare++;
                        break;
                    case ']':
                        csquare++;
                        break;
                    case '(':
                        opar++;
                        break;
                    case ')':
                        cpar++;
                        break;
                }
            }

            if (ocurly != ccurly) {
                err = true;
                if (ocurly == 1 && line[0] == '{' && ccurly == 0)
                    err=false;
                
                if (ocurly == 0 && line[0] == '}' && ccurly == 1)
                    err=false;
                
                if (err) {
                    message = line;
                    message += ": '{}' Mismatch";
                    return false;
                }
            }

            if (osquare != csquare) {
                err = true;
                if (osquare == 1 && line[0] == '[' && csquare == 0)
                    err=false;
                
                if (osquare == 0 && line[0] == ']' && csquare == 1)
                    err=false;
                
                if (err) {
                    message = line;
                    message += ": '[]' Mismatch";
                    return false;
                }
            }

            if (opar != cpar) {
                err = true;
                if (opar == 1 && line[0] == '(' && cpar == 0)
                    err=false;
                
                if (opar == 0 && line[0] == ')' && cpar == 1)
                    err=false;
                
                if (err) {
                    message = line;
                    message += ": '()' Mismatch";
                    return false;
                }
            }

            
            //Second we look for the = sign at the end of the string...
            
            pos=line.rfind(equal,line.size()-1);
            if (pos == -1 || pos == line.size()-1) {
                message = line;
                message+=": Cannot find code assignment...";
                return false;
            }
            
            res=line.c_str()+pos+1;
            
            short r=-1;
            if (res!="#")
                r=atoi(STR(res));
            action.push_back(r);
            line=line.substr(0,pos);
            
            if (initmetakey)
                replacemetas(metalines, line);
            
            for (long j=0;j<line.size();j++) {
                switch(line[j]) {
                    case '%':
                        //We record the first rule not starting with a character
                        //we check if the character is a metarule...
                        addfirstrule=false;
                        if (!j) {
                            if (line.size()==1) {
                                //The % operator as a single character... %=0
                                if (!metakey && table['%']==255)
                                    table['%']=i;
                                
                                sub="%";
                                rule.push_back(sub);
                                ruleelements[i].push_back(xr_char);
                                break;
                            }
                            
                            addfirstrule=true;
                            if (line[1]=='s' || line[1]=='S' || line[1]=='r') {
                                if (!metakey) {
                                    addfirstrule=false;
                                    if (line[1]!='r') {
                                        if (table[32]==255)
                                            table[32]=i;
                                        if (table[9]==255)
                                            table[9]=i;
                                    }
                                    if (line[1]!='s') {
                                        if (table[10]==255)
                                            table[10]=i;
                                        if (table[13]==255)
                                            table[13]=i;
                                    }
                                }
                            }
                            else
                                //specific case: %d, since we know all possible digits, we can index on them all
                                if (line[1]=='d') {
                                    if (!metakey) {
                                        addfirstrule=false;
                                        for (k=48;k<58;k++) {
                                            if (table[(char)k]==255)
                                                table[(char)k]=i;
                                        }
                                    }
                                }
                        }
                        
                        cc=line[j+1];
                        
                        if (!neg && strchr(x_actions,cc)==NULL) {
                            //this is a direct comparison
                            if (!j && table[cc]==255)
                                table[cc]=i;
                            sub=cc;
                            rule.push_back(sub);
                            ruleelements[i].push_back(xr_char);
                            j++;
                            break;
                        }
                        
                        if (addfirstrule && firstrule==-1 && !metakey)
                            firstrule=i;
                        
                        sub="%";
                        sub+=cc;
                        if (neg) {
                            rule.back()+=sub;
                            ruleelements[i].back() |= xr_meta;
                            neg=false;
                        }
                        else {
                            rule.push_back(sub);
                            ruleelements[i].push_back(xr_meta);
                        }
                        
                        if (sub[1] == '.' || sub[1] == '?')
                            ruleelements[i].back() |= xr_neednext;
                        j++;
                        break;
                    case '{':
                        brk='}';
                        typebrk=xr_metadisjunction;
                    case '[':
                        k=j+1;
                        while (k<line.size() && line[k]!=brk) k++;
                        if (k==line.size()) {
                            //then it is not a disjunction but a simple character recognition
                            neg=false;
                            sub=line[j];
                            if (!j) {
                                if (!metakey && table[sub[0]] == 255)
                                    table[sub[0]]=i;
                            }
                            
                            rule.push_back(sub);
                            ruleelements[i].push_back(xr_char);
                            brk=']';
                            typebrk=xr_chardisjunction;
                            break;
                        }
                        
                        //We record the first rule not starting with a character
                        if (!j && firstrule==-1 && !metakey)
                            firstrule=i;
                        
                        sub=line.substr(j+1,k-j-1);
                        
                        if (typebrk==xr_chardisjunction)
                            sub+=" ";
                        else {
                            vector<string> vsub;
                            //we split at the " "
                            long d=0,e;
                            string sx;
                            for (e=0;e<sub.size();e++) {
                                if (sub[e]==' ') {
                                    sx=sub.substr(d,e-d);
                                    vsub.push_back(sx);
                                    d=e+1;
                                }
                            }
                            sx=sub.substr(d,e-d);
                            vsub.push_back(sx);
                            sub = convertfromnumber((long)disjunctions.size());
                            disjunctions.push_back(vsub);
                        }
                        if (neg) {
                            rule.back()+=sub;
                            ruleelements[i].back() |= typebrk;
                            neg=false;
                        }
                        else {
                            rule.push_back(sub);
                            ruleelements[i].push_back(typebrk);
                        }
                        j=k;
                        brk=']';
                        typebrk=xr_chardisjunction;
                        break;
                    case '+':
                        if (j) { //only if it is not the first character
                            aplus=true;
                            ruleelements[i].back() |= xr_plus;
                            break;
                        }
                    case '-':
                        if (j) { //only if it is not the first character
                            ruleelements[i].back() |= xr_skip;
                            break;
                        }
                    case '~': //only if it is not the first character
                        if (j) { //otherwise, it is a character as the others...
                            neg=true;
                            rule.back()+=line[j];
                            ruleelements[i].back() |= xr_negation;
                            break;
                        }
                    case '(':
                        if (j) {
                            bool found=false;
                            short nb=1;
                            k=j+1;
                            while (k<line.size()) {
                                if (line[k]=='(')
                                    nb++;
                                else
                                    if (line[k]==')') {
                                        nb--;
                                        if (!nb) {
                                            found=true;
                                            break;
                                        }
                                    }
                                k++;
                            }
                            if (found) {
                                sub="(";
                                rule.push_back(sub);
                                ruleelements[i].push_back(xr_optional);
                                opening++;
                                break;
                            }
                        }
                    case ')':
                        if (j) {
                            if (opening) {
                                sub=")";
                                rule.push_back(sub);
                                ruleelements[i].push_back(xr_endoptional);
                                opening--;
                                break;
                            }
                        }
                    default:
                        neg=false;
                        sub=line[j];
                        if (!j) {
                            if (!metakey && table[sub[0]] == 255)
                                table[sub[0]]=i;
                        }
                        rule.push_back(sub);
                        ruleelements[i].push_back(xr_char);
                }
            }

            //one character rules are identified for fast processing (binary code 128)...
            if (rule.size()==1 && !aplus)
                ruleelements[i][0] |= xr_singlebody;
            tokenizer.push_back(rule);
        }
        
        rules.clear();
        return true;
    }
    
    void separator(bool s) {
        if (s) {
            action[separators[0]]=0;
            action[separators[1]]=0;
        }
        else {
            action[separators[0]]=xr_skiprule;
            action[separators[1]]=xr_skiprule;
        }
    }
};


class x_wtokenize : public x_wreading {
public:
    vector<short> points;
    vector<short> separators;
    
    void setrules();
    void resetrules(vector<string>& rules);

    void load() {
        if (!loaded) {
            setrules();
            parserules();
            loaded=true;
        }
    }
    
    void selectcomma(bool v) {
        long r;
        if (v) {
            action[separators[0]]=xr_skiprule;
            for (r=0;r<points.size();r+=2)
                tokenizer[points[r]][points[r+1]]=',';
        }
        else {
            action[separators[0]]=0;
            for (r=0;r<points.size();r+=2)
                tokenizer[points[r]][points[r+1]]='.';
        }
    }
    
    void keeprc(bool tr) {
        if (tr) {
            action[1]=0;
        }
        else {
            action[1]=-1;
        }
    }
    
    void separator(bool s) {
        if (s) {
            action[separators[0]]=0;
            action[separators[1]]=0;
        }
        else {
            action[separators[0]]=xr_skiprule;
            action[separators[1]]=xr_skiprule;
        }
    }
    
    bool parseexternalrules(wstring& message) {
        
        /*
         
         The rules are parsed and the results is stored both in tokenizer and in ruleelements.
         
         ruleelements uses the following binary encoding to define an instruction
         
         xr_char (1)             -> regular character
         xr_meta (2)             -> meta-character (%x)
         xr_chardisjunction (4)  -> disjunction of characters ([..])
         xr_metadisjunction (8)  -> disjunction of metacharacters ({%...})
         xr_negation (16)        -> negation (~)
         xr_plus (32)            -> Kleene operator (+)
         xr_skip (64)            -> Character not stored (-)
         xr_neednext (128)       -> the metacharacter is a . or a ?
         xr_singlebody (256)     -> Single body rule (only one character)
         xr_optional (512)       -> Optional section
         xr_endoptional (1024)   -> End optional section
         
         -> IMPORTANT: rules that start with a regular character are also indexed with their first character in "table".
         rules that start with %d are also indexed on all possibile 10 digits.
         
         -> IMPORTANT: rules should be ordered with rules starting with a character first,
         THEN rules starting with a meta character (%x) or a disjunction after.
         
         -> VERY IMPORTANT: rules starting with a character should always next to the rules sharing the same first character...
         
         The variable firstrule records the position of the first rule starting with a meta-character...
         
         -> IMPORTANT: we keep tracks of rules that are reduced to one single character check in order to process them immediatly
         
         BEWARE: This parser DOES not check if RULES are VALID...
         
         */
        
        char x_actions[]="?aCcdHnopSsr.";
        
        wstring line;
        wstring sub;
        wstring equal(L"=");
        wstring res;
        
        long k;
        long i, pos;
        
        wchar_t brk=L']', metakey;
        wchar_t cc;
        
        short opening;
        
        char typebrk=xr_chardisjunction;
        bool aplus;
        bool neg=false;
        bool addfirstrule;
        map<wstring, wstring> metalines;
        bool initmetakey=false;
        
        long ocurly = 0;
        long osquare = 0;
        long ccurly = 0;
        long csquare = 0;
        long opar = 0;
        long cpar = 0;
        bool err;

        ocurly = 0;
        osquare = 0;
        ccurly = 0;
        csquare = 0;
        opar = 0;
        cpar = 0;
        //first we check whether brackets are all balanced...
        for (pos =0; pos<line.size(); pos++) {
            if (line[pos]=='%') {
                pos++;
                continue;
            }
            switch(line[pos]) {
                case '{':
                    ocurly++;
                    break;
                case '}':
                    ccurly++;
                    break;
                case '[':
                    osquare++;
                    break;
                case ']':
                    csquare++;
                    break;
                case '(':
                    opar++;
                    break;
                case ')':
                    cpar++;
                    break;
            }
        }
        
        if (ocurly != ccurly) {
            err = true;
            if (ocurly == 1 && line[0] == '{' && ccurly == 0)
                err=false;
            
            if (ocurly == 0 && line[0] == '}' && ccurly == 1)
                err=false;
            
            if (err) {
                message = line;
                message += L": '{}' Mismatch";
                return false;
            }
        }
        
        if (osquare != csquare) {
            err = true;
            if (osquare == 1 && line[0] == '[' && csquare == 0)
                err=false;
            
            if (osquare == 0 && line[0] == ']' && csquare == 1)
                err=false;
            
            if (err) {
                message = line;
                message += L": '[]' Mismatch";
                return false;
            }
        }
        
        if (opar != cpar) {
            err = true;
            if (opar == 1 && line[0] == '(' && cpar == 0)
                err=false;
            
            if (opar == 0 && line[0] == ')' && cpar == 1)
                err=false;
            
            if (err) {
                message = line;
                message += L": '()' Mismatch";
                return false;
            }
        }
        for (i=0;i<rules.size();i++) {
            sc_utf8_to_unicode(line, USTR(rules[i]), rules[i].size());
            vector<short> e;
            ruleelements.push_back(e);
            vector<wstring> rule;
            
            
            if (line[1]==':') { //we detect a meta-rule...
                metakey=line[0];
                line=line.c_str()+2;
                wstring key=L"%";
                key+=metakey;
                if (initmetakey)
                    replacemetas(metalines,line);
                metalines[key] = line;
                initmetakey=true;
                action.push_back(xr_skiprule);
                tokenizer.push_back(rule);
                continue;
            }
            
            
            neg=false;
            aplus=false;
            opening=0;
            metakey=0;
            
            short r=-1;
            
            //first we look for the = sign at the end of the string...
            
            pos=line.rfind(equal,line.size()-1);
            if (pos == -1 || pos == line.size()-1) {
                message = line;
                message += L": Cannot find code assignment...";
                return false;
            }

            res=line.c_str()+pos+1;
            
            if (res!=L"#")
                r=convertinteger(res);
            
            action.push_back(r);
            line=line.substr(0,pos);
            
            if (initmetakey)
                replacemetas(metalines,line);
            
            for (long j=0;j<line.size();j++) {
                
                switch(line[j]) {
                    case L'%':
                        addfirstrule=false;
                        //We record the first rule not starting with a character
                        if (!j) {
                            if (line.size()==1) {
                                //The % operator as a single character... %=0
                                if (!metakey && table['%']==255)
                                    table['%']=i;
                                
                                sub=L"%";
                                rule.push_back(sub);
                                ruleelements[i].push_back(xr_char);
                                break;
                            }
                            
                            addfirstrule=true;
                            
                            if (line[1]=='s' || line[1]=='S' || line[1]=='r') {
                                if (!metakey) {
                                    addfirstrule=false;
                                    if (line[1]!='r') {
                                        if (table[32]==255)
                                            table[32]=i;
                                        if (table[9]==255)
                                            table[9]=i;
                                    }
                                    if (line[1]!='s') {
                                        if (table[10]==255)
                                            table[10]=i;
                                        if (table[13]==255)
                                            table[13]=i;
                                    }
                                }
                            }
                            else
                                //specific case: %d, since we know all possible digits, we can index on them all
                                if (line[1]=='d') {
                                    if (!metakey) {
                                        addfirstrule=false;
                                        for (k=48;k<58;k++) {
                                            if (table[(char)k]==255)
                                                table[(char)k]=i;
                                        }
                                    }
                                }
                        }
                        
                        cc=line[j+1];
                        
                        if (!neg && cc < 128 && strchr(x_actions,cc)==NULL) {
                            //this is a direct comparison
                            if (!j && table[cc]==255)
                                table[cc]=i;
                            sub=cc;
                            rule.push_back(sub);
                            ruleelements[i].push_back(xr_char);
                            j++;
                            break;
                        }
                        
                        if (addfirstrule && firstrule==-1 && !metakey)
                            firstrule=i;
                        
                        sub= L"%";
                        sub+=cc;
                        if (neg) {
                            rule.back()+=sub;
                            ruleelements[i].back() |= xr_meta;
                            neg=false;
                        }
                        else {
                            rule.push_back(sub);
                            ruleelements[i].push_back(xr_meta);
                        }
                        
                        if (sub[1] == '.' || sub[1] == '?')
                            ruleelements[i].back() |= xr_neednext;
                        j++;
                        break;
                    case L'{':
                        brk='}';
                        typebrk=xr_metadisjunction;
                    case L'[':
                        k=j+1;
                        while (k<line.size() && line[k]!=brk) k++;
                        if (k==line.size()) {
                            //then it is not a disjunction but a simple character recognition
                            neg=false;
                            sub=line[j];
                            if (!j && sub[0]<256) {
                                if (!metakey && table[sub[0]] == 255)
                                    table[sub[0]]=i;
                            }
                            rule.push_back(sub);
                            ruleelements[i].push_back(xr_char);
                            brk=']';
                            typebrk=xr_chardisjunction;
                            break;
                        }
                        //We record the first rule not starting with a character
                        if (!j && firstrule==-1 && !metakey)
                            firstrule=i;
                        
                        sub=line.substr(j+1,k-j-1);
                        
                        if (typebrk==xr_chardisjunction)
                            sub+=L" ";
                        else {
                            vector<wstring> vsub;
                            //we split at the " "
                            long d=0,e;
                            wstring sx;
                            for (e=0;e<sub.size();e++) {
                                if (sub[e]==' ') {
                                    sx=sub.substr(d,e-d);
                                    vsub.push_back(sx);
                                    d=e+1;
                                }
                            }
                            sx=sub.substr(d,e-d);
                            vsub.push_back(sx);
                            sub = wconvertfromnumber((long)disjunctions.size());
                            disjunctions.push_back(vsub);
                        }
                        
                        if (neg) {
                            rule.back()+=sub;
                            ruleelements[i].back() |= typebrk;
                            neg=false;
                        }
                        else {
                            rule.push_back(sub);
                            ruleelements[i].push_back(typebrk);
                        }
                        j=k;
                        brk=']';
                        typebrk=xr_chardisjunction;
                        break;
                    case L'+':
                        if (j) { //only if it is not the first character
                            aplus=true;
                            ruleelements[i].back() |= xr_plus;
                            break;
                        }
                    case L'-':
                        if (j) { //only if it is not the first character
                            ruleelements[i].back() |= xr_skip;
                            break;
                        }
                    case L'~': //only if it is not the first character
                        if (j) { //otherwise, it is a character as the others...
                            neg=true;
                            rule.back()+=line[j];
                            ruleelements[i].back() |= xr_negation;
                            break;
                        }
                    case L'(':
                        if (j) {
                            bool found=false;
                            short nb=1;
                            k=j+1;
                            while (k<line.size()) {
                                if (line[k]=='(')
                                    nb++;
                                else
                                    if (line[k]==')') {
                                        nb--;
                                        if (!nb) {
                                            found=true;
                                            break;
                                        }
                                    }
                                k++;
                            }
                            if (found) {
                                sub=L"(";
                                rule.push_back(sub);
                                ruleelements[i].push_back(xr_optional);
                                opening++;
                                break;
                            }
                        }
                    case L')':
                        if (j) {
                            if (opening) {
                                sub=L")";
                                rule.push_back(sub);
                                ruleelements[i].push_back(xr_endoptional);
                                opening--;
                                break;
                            }
                        }
                    default:
                        neg=false;
                        sub=line[j];
                        if (!j && sub[0]<256) {
                            if (!metakey && table[sub[0]] == 255)
                                table[sub[0]]=i;
                        }
                        rule.push_back(sub);
                        ruleelements[i].push_back(xr_char);
                }
            }
            
            //one character rules are identified for fast processing (binary code 128)...
            if (rule.size()==1 && !aplus)
                ruleelements[i][0] |= xr_singlebody;
            tokenizer.push_back(rule);
        }
        rules.clear();
        return true;
    }

};

class x_forindent : public x_reading {
public:
    
    
    //We keep all characters...
    void setrules() {
        
        lookforquotes = true;
        
        //Spaces are now all kept
        rules.push_back(" =0");                         //0     space
        rules.push_back("\t=0");                        //1     tab
        rules.push_back("\n=0");                        //2     cr
        rules.push_back("\r=0");                        //3     cr
        
        rules.push_back("1:{%d #A-F #a-f}");            //2     metarule on 1, for hexadecimal digits

        //Fast tracks for recurrent punctations
        rules.push_back(";=0");                         //4     ;
        rules.push_back(",=0");                         //5     ,
        rules.push_back("==0");                         //6     =
        rules.push_back("~=0");                         //7     ~
        rules.push_back("!=0");                         //8     !
        rules.push_back("(=0");                         //9     (
        rules.push_back(")=0");                         //10    )
        rules.push_back("[=0");                         //11    [
        rules.push_back("]=0");                         //12    ]
        rules.push_back("{=0");                         //13    {
        rules.push_back("}=0");                         //14    }
        rules.push_back(".=0");                         //15    .
        rules.push_back("^=0");                         //16    ^
        rules.push_back("+=0");                         //17    +
        rules.push_back("-=0");                         //18    -
        rules.push_back("*=0");                         //19    *
        rules.push_back("%=0");                         //20    %
        rules.push_back("<=0");                         //21    <
        rules.push_back(">=0");                         //22    >
        rules.push_back("|=0");                         //23    |
        rules.push_back("&=0");                         //24    &
        rules.push_back(":=0");                         //25    :
        rules.push_back("$=0");                         //26    $
        rules.push_back("#=0");                         //27    #
        rules.push_back("?=0");                         //28    ?
        
        
        //Comments are also kept here...
        rules.push_back("//%.~%r+=9");                  //29    comments starting with // with no carriage return (CR) inside
        rules.push_back("//=9");                        //30    empty comment
        rules.push_back("/@%.+@/=9");                   //31    long comments starting with /@ and finishing with @/
        rules.push_back("/@@/=9");                      //32    empty long comment starting with /@ and finishing with @/
        rules.push_back("/=0");                         //33    /
        
        //Strings
        //Double quote
        rules.push_back("\"\"=1");                      //34    empty string ""
        rules.push_back("\"%?~%r+\"=1");                //35    string "" does not contain CR and can escape characters (%?)
        
        //Single quote
        rules.push_back("''=2");                        //36    empty string ''
        rules.push_back("'%.~%r+'=2");                  //37    string '' does not contain CR and does not process escape characters
        
        rules.push_back("r\"%?+\"=2");                 //44    tamgu regular expressions...
        rules.push_back("r'%?+'=2");                   //44    tamgu regular expressions...
        rules.push_back("p\"%?+\"=2");                 //44    posix regular expressions...
        rules.push_back("p'%?+'=2");                   //44    posix regular expressions...

        //Long quotes
        rules.push_back("@\"\"@=5");                  //38    empty string @""@
        rules.push_back("@\"%?+\"@=5");               //39    string @" "@ can contain CR and escape characters (we do not keep the @s)
        
        //Unicode double quote strings
        rules.push_back("u\"\"=6");                    //40    empty string u""
        rules.push_back("u\"%?~%r+\"=6");              //41    string u"" unicode string (we do not keep the u in the parse)
        
        //Unicode single quote strings
        rules.push_back("u''=7");                      //42    empty string u''
        rules.push_back("u'%.~%r+'=7");                //43    string u'' unicode string
        
        //Unicode long quote strings
        rules.push_back("u@\"%?+\"@=8");             //44    empty string u@""@
        rules.push_back("u@\"%?+\"@=8");             //45    string u@".."@ unicode string
        
        rules.push_back("0x%1+(.%1+)([p P]([- +])%d+)=3"); //47 hexadecimal
        rules.push_back("%d+(.%d+)([e E]([- +])%d+)=3");         //47    exponential digits
        
        // Rules start here        
        rules.push_back("{%a %d %H}+=4");               //48    label a combination of alpha, digits and hangul characters
        rules.push_back("%n=0");
        rules.push_back("%o=0");                        //50    operators
        rules.push_back("%p=0");                        //49    punctuation
        rules.push_back("%.~{%S %p %o}+=4");            //51    An unknown UTF8 token separated with spaces, punctuation or operators...
    }
    
    //We tokenize our string...
    void tokenize(string& thestr, bool keeppos=false, vector<string>* vstack=NULL, vector<unsigned char>* vtype=NULL) {
        //only stack is necessary
        if (vstack==NULL)
            stack.clear();
        if (vtype==NULL)
            stacktype.clear();
        parcours=thestr;
        juststack=true;
        apply(keeppos, vstack,vtype);
    }
};

//------------------------------------------------------------------------------------------------Coloring tokenizer rules

class x_coloringrule : public x_reading {
public:
    
    void setrules() {
        lookforquotes = true;
            //spaces
        rules.push_back(" =#");                         //0     space
        rules.push_back("\t=#");                        //1     tab
        rules.push_back("\n=#");                        //2     cr
        rules.push_back("\r=#");                        //3     cr
        
        rules.push_back("1:{%d #A-F #a-f}");            //2     metarule on 1, for hexadecimal digits
        
            //Fast tracks for recurrent punctuations
        rules.push_back(";=#");                         //4     ;
        rules.push_back(",=#");                         //5     ,
        rules.push_back("==0");                         //6     =
        rules.push_back(")=#");                         //7     )
        rules.push_back("[=#");                         //8     [
        rules.push_back("]=#");                         //9     ]
        rules.push_back("{=#");                         //10    {
        rules.push_back("}=#");                         //11    }
        rules.push_back("~=#");                         //12    ~
        rules.push_back("!=#");                         //13    !
        rules.push_back("^=#");                         //14    ^
        rules.push_back("+=#");                         //15    +
        rules.push_back("-=#");                         //16    -
        rules.push_back("*=#");                         //17    *
        rules.push_back("%=#");                         //18    %
        rules.push_back(">=#");                         //20    >
        rules.push_back("|=#");                         //21    |
        rules.push_back("&=#");                         //22    &
        rules.push_back(":=#");                         //23    :
        
        rules.push_back("${%d %a %H}+=10");             //24    $%d+
        rules.push_back("$=#");                         //25    $
        
        rules.push_back("#{%d %a %H}+=10");             //26    #label
        rules.push_back("#=#");                         //27    #
        
        rules.push_back("?{%a %d %H}+=10");             //28    ?label
        rules.push_back("?=#");                         //29    ?
        
            // Characters that we need
        rules.push_back("<%a{%a %d %H}+=13");           //30    <label
        rules.push_back("<=#");                         //19    <
        rules.push_back(".{%a %d %H}+(=11");            //30    .label(
        rules.push_back(".=#");                         //31    .
        rules.push_back("(=#");                         //32    (
        
            //Comments
        rules.push_back("//%.~%r+=5");                  //33    comments
        rules.push_back("//+=5");                       //34    empty comment
        rules.push_back("/@%.+@/=5");                   //35    long comments
        rules.push_back("/@@/=5");                      //36    empty long comments
        rules.push_back("/=#");                         //37    /
        
            //Strings
            //Double quotes
        rules.push_back("\"\"=1");                      //38    empty string ""
        rules.push_back("\"%?~%r+\"=1");                //39    string ""
                                                        
            //Single quote
        rules.push_back("''=2");                        //40    empty string ''
        rules.push_back("'%.~%r+'=2");                  //41    string ''
        
        rules.push_back("r\"%?+\"=2");                 //44    tamgu regular expressions...
        rules.push_back("r'%?+'=2");                   //44    tamgu regular expressions...
        rules.push_back("p\"%?+\"=2");                 //44    posix regular expressions...
        rules.push_back("p'%?+'=2");                   //44    posix regular expressions...
        
            //Long quotes
        rules.push_back("@\"\"@=3");                    //42    empty string @""@
        rules.push_back("@\"%?+\"@=3");                 //43    string @" "@
        rules.push_back("@{%a %d}+=14");                 //43    annotation rule head
        
            //Unicode double quote strings
        rules.push_back("u\"\"=1");                     //44    empty string u""
        rules.push_back("u\"%?~%r+\"=1");               //45    string u"" unicode string (we do not keep the u in the parse)
        
            //Unicode single quote strings
        rules.push_back("u''=2");                       //46    empty string
        rules.push_back("u'%.~%r+'=2");                 //47    string u'' unicode string
        
            //Unicode long quote strings
        rules.push_back("u@\"%?+\"@=3");                //48    empty string u@""@
        rules.push_back("u@\"%?+\"@=3");                //49    string u@".."@ unicode string
        
        rules.push_back("0x%1+(.%1+)([p P]([- +])%d+)=0"); //47 hexadecimal
        rules.push_back("%d+(.%d+)([e E]([- +])%d+)=0");    //51    exponential digits
        
            //Rules start here
        rules.push_back("{%a %d}+(=12");                //52    label(
        rules.push_back("{%a %d %H}+=4");               //53    label
        rules.push_back("%n=#");                        //52    non-breaking space (not kept)
        rules.push_back("%o=#");                        //55    operators
        rules.push_back("%p=#");                        //54    punctuation
        rules.push_back("%.~{%S %p %o}+=4");            //56    An unknown token separated with spaces or punctuation...
        rules.push_back("%=#");                         //56    An unknown character...
    }
};

#endif

