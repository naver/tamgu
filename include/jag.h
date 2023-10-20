/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : jag.h
 Date       : 2017/09/01
 Purpose    : Jag (작) class
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#ifndef jag_h
#define jag_h

#include "tamguboost.h"
#include "messages_error.h"

#define solo_line 0
#define beg_line 1
#define concat_line 2

#define u_del_linked -2
#define u_del -1
#define u_modif 0
#define u_modif_linked 1
#define u_ins 2
#define u_ins_linked 3

#define l_str 1
#define l_com 2
#define l_com_one 3

const char enablemouse_vt100[] = {27,91,'?','1','0','0','3','h',0};
const char enablemouse[] = {27,91,'?','1','0','0','3','h',27,91,'?','1','0','1','5','h',27,91,'?','1','0','1','6','h',0};

const char disablemouse[] = {27,91,'?','1','0','0','0','l',0};


#ifdef DOSOUTPUT
void Setdosoutput(bool d);
bool isDosOutput();
string conversiontodos(char* c);
#else
#define conversiontodos(X) X
#endif


#ifdef APPLE
#define openMode std::ios::in|std::ios::binary
#else
#define openMode ios::in|ios::binary
#endif

class jag_editor;

extern jag_editor* JAGEDITOR;

long VirtualIndentation(string& codestr);
Exporting long GetBlankSize();

#ifdef PASDECOULEUR
const char m_current[] = {0,0};
const char m_red[] = {0,0};
const char m_redital[] = {0,0};
const char m_redbold[] = {0,0};
const char m_green[] = {0,0};
const char m_dore[] = {0,0};
const char m_blue[] = {0,0};
const char m_blueblack[] = {0,0};
const char m_gray[] = {0,0};
const char m_lightgray[] = {0,0};
const char m_selectgray[] = {0,0};
#else
const char m_current[] = {27, '[', '0', 'm', 0};
const char m_redital[] = {27, '[', '3', ';', '3','1', ';','4','9','m',0};
const char m_redbold[] = {27, '[', '1', ';', '3','1', ';','4','9','m',0};
const char m_green[] = {27, '[', '0', ';', '3','2', ';','4','9','m',0};
const char m_dore[] = {27, '[', '0', ';', '3','3', ';','4','9','m',0};
#ifdef WIN32
const char m_red[] = { 27, '[', '1', ';', '3','1', ';','4','9','m',0 };
const char m_blue[] = { 27, '[', '0', ';', '3','6','m',0 };
const char m_blueblack[] = { 27, '[', '0', ';', '3','6', ';','4','9','m',0 };
#else
const char m_red[] = { 27, '[', '0', ';', '3','1', ';','4','9','m',0 };
const char m_blue[] = {27, '[', '0', ';', '3','4', ';','4','9','m',0};
const char m_blueblack[] = {27, '[', '0', ';', '3','6', ';','4','9','m',0};
#endif
const char m_gray[] = {27, '[', '0', ';', '9','0', ';','4','9','m',0};
const char m_lightgray[] = {27, '[', '0', ';', '9','0', ';','4','9','m',0};

const char m_selectgray[] = {27, '[', '7', ';', '9','3', ';','4','0','m',0};
#endif

const string colordenomination[] = {"string", "method", "keyword", "function", "comment", "selection"};
const short nbdenomination = 6;
typedef enum {no_type, clike_type, lisp_type, python_type, tamgu_type} file_types;

//Background
const int m_clbg[] = {40, 41, 42, 43, 44, 45, 46, 47, 49, 100, 101, 102, 103, 104, 105, 106, 107, 0};
//Foreground
const int m_clfg[] =  {30, 31, 32, 33, 34, 35, 36, 37, 39, 90, 91, 92, 93, 94, 95, 96, 97, 0};
//Formatting
const int m_attr[] = {0, 1, 2, 3, 4, 5, 7, -1};

//action, moving the cursor...
extern char m_down[];
const char m_up[] = {27, '[', '1', 65, 0};
//On macos you might need to insert these key combination in the keyboard preference as: \033[1;5A and \033[1;5B

//arrow moving

const char a_right[] = {27, 102, 0};
const char a_left[] = {27, 98, 0};

const char m_clear_line[] = { 27, 91, '0', 'K', 0 };
const char m_clear[] = {27, 91, '2', 'J', 0};
const char m_clear_scrolling[] = {27, 91, '3', 'J', 0};
const char m_scrollup[] = {27, 91, '1', 'S', 0};
const char m_scrollup3[] = { 27, 91, '3', 'S', 0 };
const char m_scrolldown[] = { 27, 91, '1', 'T', 0 };
const char m_insertline[] = { 27, 91, '1', 'L', 0 };
const char m_deleteline[] = { 27, 91, '1', 'M', 0 };
const char m_home[] = {27, 91, 'H', 0};

#ifdef WIN32
const uchar is_up = 72;
const uchar is_down = 80;
const char right[] = { 224, 77, 0 };
const char left[] = { 224, 75, 0 };
const char down[] = {224,80, 0 };
const char up[] = {224,72, 0 };
const char del[] = { 224, 83, 0 };
const char homekey[] = { 224, 71, 0 };
const char endkey[] = { 224, 79, 0 };
const char c_homekey[] = { 224, 119, 0 };
const char c_endkey[] = { 224, 117, 0 };
const char c_up[] = { 224, 73, 0 };
const char c_down[] = { 224,81, 0 };
const char c_right[] = { 224,116, 0 };
const char c_left[] = { 224,115, 0 };
const unsigned char alt_xbis[] = { 226, 'x', 0 };
const unsigned char alt_cbis[] = { 226, 'c', 0 };
const unsigned char alt_vbis[] = { 226, 'v', 0 };

const unsigned char alt_x[] = { 226, 'x', 0 };
const unsigned char alt_c[] = { 226, 'c', 0 };
const unsigned char alt_v[] = { 226, 'v', 0 };

const unsigned char alt_plus[] = { 226, '+', 0 };
const unsigned char alt_minus[] = { 226, '-', 0 };
const unsigned char shift_right[] = { 225, 77, 0 };
const unsigned char shift_left[] = { 225, 75, 0 };
#else
const char c_right[] = { 27, 91, 49, 59, 53, 67, 0 };
const char c_left[] = { 27, 91, 49, 59, 53, 68, 0 };
#ifdef APPLE
const char c_up[] = { 27, 91, 53, 126, 0 };
const char c_down[] = { 27, 91, 54, 126, 0 };
#else
const char c_up[] = { 27, 91, 49, 59, 53, 65, 0 }; //\033[1;5A
const char c_down[] = { 27, 91, 49, 59, 53, 66, 0 }; //\033[1;5B
#endif
const uchar is_up = 65; 
const uchar is_down = 66;
const char right[] = { 27, 91, 67, 0 };
const char left[] = { 27, 91, 68, 0 };
const char down[] = {27, 91, 66, 0};
const char up[] = {27, 91, 65, 0};
const char del[] = { 27, 91, 51, 126, 0 };
const char homekey[] = { 27, 91, 72, 0 };
const char endkey[] = { 27, 91, 70, 0 };

const unsigned char alt_plus[] = {226, 137, 160, 0};
const unsigned char alt_minus[] = {226, 128, 148, 0};

const unsigned char alt_xbis[] = { 27, 'x', 0 };
const unsigned char alt_cbis[] = { 27, 'c', 0 };
const unsigned char alt_vbis[] = { 27, 'v', 0 };

const unsigned char alt_x[] = {226, 137, 136, 0};
const unsigned char alt_c[] = {194, 169, 0};
const unsigned char alt_v[] = {226, 151, 138, 0};

const unsigned char shift_right[] = {27, 91, 49, 59, 50, 67, 0};
const unsigned char shift_left[] = {27, 91, 49, 59, 50, 68, 0};
#endif


const char back = 13;

const char cursor_position[] = {27, 91, '6', 'n', 0};

class editor_lines {
public:
    jag_editor* jag;
    vector<wstring> lines;
    vector<char> status;
    vector<char> longlines;
    vector<long> numeros;
    hmap<long, bool> checks;
    
    editor_lines(jag_editor* j) {
        jag = j;
    }
    
    bool check(long p) {
        if (checks.find(p) != checks.end())
            return true;
        return false;
    }
    
    void setcode(wstring& code, bool clean);
    
    bool updatesize();
    
    void numbers() {
        long nb = 0;
        numeros.clear();
        for (long i = 0; i < lines.size(); i++) {
            if (status[i] != 2)
                nb++;
            numeros.push_back(nb);
        }
        
        updatesize();
    }
    
    long getlinenumber(long l) {
        for (long i = 0; i < numeros.size(); i++) {
            if (numeros[i] == l)
                return i;
        }
        return 0;
    }
    
    wstring code() {
        wstring c;
        bool rc = false;
        for (long i = 0; i < lines.size(); i++) {
            if (status[i] != concat_line) {
                if (rc) {
                    c += L"\n";
                    rc = false;
                }
                c += lines[i];
                if (status[i] == solo_line)
                    c += '\n';
                continue;
            }
            c += lines[i];
            rc = true;
        }
        return c;
    }
    
    wstring code(long first, long end) {
        wstring c;
        bool rc = false;
        if (first < 0)
            first = 0;
        if (end > lines.size())
            end = lines.size();
        for (long i = first; i < end; i++) {
            if (status[i] != concat_line) {
                if (rc) {
                    c += L"\n";
                    rc = false;
                }
                c += lines[i];
                if (status[i] == solo_line)
                    c += '\n';
                continue;
            }
            c += lines[i];
            rc = true;
        }
        return c;
    }
    
    //We detect long commented lines or long strings
    //we check if we have spaces from the start of the string and on
    void checkspace(long pos, long l, char code) {
        for (long i = 0; i < pos; i++) {
            if (lines[l][i] > 32) {
                longlines.push_back(0);
                return;
            }
        }
        longlines.push_back(code);
    }
    
    void detectlisp() {
        char l_strings = 0;
        char l_comments = 0;
        long pos;
        
        for (long i = 0; i < lines.size(); i++) {
            if (lines[i].find(L";;") != -1) {
                if (l_comments) {
                    longlines.push_back(2);
                    l_comments = 0;
                }
                else {
                    l_comments = 2;
                    longlines.push_back(l_comments);
                }
                continue;
            }
            
            if (l_comments) {
                longlines.push_back(l_comments);
                continue;
            }
            
            pos = lines[i].find(L"«");
            if (pos != -1) {
                if (lines[i].find(L"»") == -1) {
                    l_strings = 1;
                    checkspace(pos, i, l_strings);
                }
                else
                    longlines.push_back(0);
            }
            else {
                if (lines[i].find(L"»") != -1) {
                    longlines.push_back(l_strings);
                    l_strings = 0;
                }
                else {
                    pos = lines[i].find(L"`");
                    if (pos != -1 && lines[i].find(L"`", pos + 1) == -1) {
                        if (l_strings) {
                            longlines.push_back(1);
                            l_strings = 0;
                        }
                        else {
                            l_strings = 1;
                            checkspace(pos, i, l_strings);
                        }
                    }
                    else
                        longlines.push_back(l_strings);
                }
            }
        }
    }
    
    void detectpython() {
        char l_strings = 0;
        long pos;
        
        for (long i = 0; i < lines.size(); i++) {
            pos = lines[i].find(L"'''");
            if (pos != -1 && lines[i].find(L"'''", pos + 1) == -1) {
                if (l_strings) {
                    longlines.push_back(1);
                    l_strings = 0;
                }
                else {
                    l_strings = 1;
                    checkspace(pos, i, l_strings);
                }
            }
            else {
                pos = lines[i].find(L"\"\"\"");
                if (pos != -1 && lines[i].find(L"\"\"\"", pos + 1) == -1) {
                    if (l_strings) {
                        longlines.push_back(1);
                        l_strings = 0;
                    }
                    else {
                        l_strings = 1;
                        checkspace(pos, i, l_strings);
                    }
                }
                else
                    longlines.push_back(l_strings);
            }
        }
    }
    
    void detectclike() {
        char l_comments = 0;
        long pos;
        for (long i = 0; i < lines.size(); i++) {
            pos = lines[i].find(L"/*");
            if (pos != -1) {
                if (lines[i].find(L"*/") == -1) {
                    l_comments = 2;
                    checkspace(pos, i, l_comments);
                }
                else
                    longlines.push_back(0);
            }
            else {
                if (lines[i].find(L"*/") != -1) {
                    longlines.push_back(l_comments);
                    l_comments = 0;
                }
                else
                    longlines.push_back(l_comments);
            }
        }
    }
    
    void detecttamgu() {
        char l_strings = 0;
        char l_comments = 0;
        long pos;
        
        for (long i = 0; i < lines.size(); i++) {
            if (lines[i].find(L"/@") != -1) {
                if (lines[i].find(L"@/") == -1) {
                    l_comments = 2;
                    longlines.push_back(l_comments);
                }
                else
                    longlines.push_back(0);
            }
            else {
                if (lines[i].find(L"@/") != -1) {
                    longlines.push_back(l_comments);
                    l_comments = 0;
                }
                else {
                    if (l_comments)
                        longlines.push_back(l_comments);
                    else {
                        pos = lines[i].find(L"@\"");
                        if (pos != -1) {
                            if (lines[i].find(L"\"@") == -1) {
                                l_strings = 1;
                                checkspace(pos, i, l_strings);
                            }
                            else
                                longlines.push_back(0);
                        }
                        else {
                            if (lines[i].find(L"\"@") != -1) {
                                longlines.push_back(l_strings);
                                l_strings = 0;
                            }
                            else
                                longlines.push_back(l_strings);
                        }
                    }
                }
            }
        }
    }
    
    void detectlongstrings(file_types ftype) {
        longlines.clear();
        switch (ftype) {
            case lisp_type:
                detectlisp();
                break;
            case python_type:
                detectpython();
                break;
            case tamgu_type:
                detecttamgu();
                break;
            case clike_type:
                detectclike();
                break;
            default:
                for (long i = 0; i < lines.size(); i++)
                    longlines.push_back(0);
        }
    }
    
    wstring& operator[](long pos) {
        if (pos == lines.size())
            push(L"");
        return lines[pos];
    }
    
    long size() {
        return lines.size();
    }
    
    long indent(long p) {
        long i = p;
        wchar_t c;
        while (i >= 0) {
            c = lines[i][0];
            if (c != 32 && c!= 9 && c)
                break;
            i--;
        }
        wstring cd = code(i, p + 1);
        string ccd;
        s_unicode_to_utf8(ccd, cd);
        return VirtualIndentation(ccd);
    }
    
    long splitline(wstring& l, long linenumber, vector<wstring>& subs);
    
    char Status(long pos) {
        if (pos >= 0 && pos < lines.size())
            return status[pos];
        return 0;
    }
    
    //Check if we can go up to the end of the line
    bool eol(long p) {
        if (Status(p) != solo_line &&  Status(p+1) == concat_line)
            return false;
        return true;
    }
    //the line is cut after pos (either for destruction of copy)
    //the line is cut after pos (either for destruction of copy)
    char updatestatus(long pos);
    
    void erase(long pos) {
        lines.erase(lines.begin()+pos);
        status.erase(status.begin()+pos);
    }
    
    void erase(long pos, long end) {
        if (end >= lines.size())
            end = -1;
        
        if (end == -1) {
            lines.erase(lines.begin()+pos, lines.end());
            status.erase(status.begin()+pos, status.end());
        }
        else {
            lines.erase(lines.begin()+pos, lines.begin() + end);
            status.erase(status.begin()+pos, status.begin() + end);
        }
    }
    
    void insert(long pos, wstring& sub, char st) {
        lines.insert(lines.begin() + pos, sub);
        status.insert(status.begin()+pos, st);
        updatesize();
    }
    
    void insert(long pos, wstring& sub) {
        lines.insert(lines.begin() + pos, sub);
        status.insert(status.begin()+pos, solo_line);
        updatesize();
    }
    
    void inserting(long pos, wstring sub) {
        lines.insert(lines.begin() + pos, sub);
        status.insert(status.begin()+pos, solo_line);
        updatesize();
    }
    
    void push_back(wstring& sub) {
        lines.push_back(sub);
        status.push_back(solo_line);
        if (numeros.size())
            numeros.push_back(numeros.back()+1);
        else
            numeros.push_back(1);
        updatesize();
    }
    
    void push(wstring sub) {
        lines.push_back(sub);
        status.push_back(solo_line);
        if (numeros.size())
            numeros.push_back(numeros.back()+1);
        else
            numeros.push_back(1);
        updatesize();
    }
    
    void pop_back() {
        lines.pop_back();
        status.pop_back();
        numeros.pop_back();
    }
    
    wstring getoneline(long pos, long& end) {
        if (status[pos] == solo_line) {
            end = pos;
            return lines[pos];
        }
        
        wstring line = lines[pos++];
        while (pos < lines.size() && Status(pos) == concat_line)
            line += lines[pos++];
        
        end = pos-1;
        return line;
    }
    
    void clear() {
        checks.clear();
        lines.clear();
        status.clear();
        numeros.clear();
    }
    
    wstring back() {
        return lines.back();
    }
    
    bool checkfullsize(wstring&, bool&);
    bool checksize(long p);
    void undo(wstring& l, long p, char a);
    
    void replaceline(long p, long end, wstring& line) {
        bool equal = false;
        if (checkfullsize(line, equal)) {
            //We need to protect the last line
            vector<wstring> subs;
            splitline(line, numeros[p], subs);
            long u;
            for (u = 0; u < subs.size(); u++) {
                if ((p+u) < end)
                    lines[p+u] =  subs[u];
                else {
                    lines.insert(lines.begin()+p+u, subs[u]);
                    status.insert(status.begin()+p+u, concat_line);
                }
            }
            
            if ((p+u) < end)
                erase(p+u, end);
            numbers();
            return;
        }
        lines[p] = line;
    }
    
    bool refactoring(long p);
    
    
};
///------------------------------------------------------------------------------------

class editor_keep {
public:
    list<wstring> l_keeplines;
    list<long> l_keeppos;
    list<char> l_keepactions;
    list<long> l_keepposinstring;
    list<long> l_keepcurrentline;
    list<long> l_keeptop;
    list<char> l_keepstatus;
    
    void pop() {
        l_keeplines.pop_back();
        l_keeppos.pop_back();
        l_keepactions.pop_back();
        l_keepcurrentline.pop_back();
        l_keepposinstring.pop_back();
        l_keeptop.pop_back();
        l_keepstatus.pop_back();
    }
    
    void move(wstring& l, editor_keep& e) {
        l_keeplines.push_back(l);
        l_keeptop.push_back(e.l_keeptop.back());
        l_keeppos.push_back(e.l_keeppos.back());
        l_keepactions.push_back(e.l_keepactions.back());
        l_keepposinstring.push_back(e.l_keepposinstring.back());
        l_keepcurrentline.push_back(e.l_keepcurrentline.back());
        l_keepstatus.push_back(e.l_keepstatus.back());
        e.pop();
    }
    
    void clear() {
        l_keeplines.clear();
        l_keeppos.clear();
        l_keepactions.clear();
        l_keepcurrentline.clear();
        l_keepposinstring.clear();
        l_keeptop.clear();
        l_keepstatus.clear();
    }
    
    void store(long postop, wstring& line, long pos, char action, long currentline, long posinstring, char status) {
        l_keeptop.push_back(postop);
        l_keeplines.push_back(line);
        l_keeppos.push_back(pos);
        l_keepactions.push_back(action); //deletion in this case in a modification
        l_keepcurrentline.push_back(currentline);
        l_keepposinstring.push_back(posinstring);
        l_keepstatus.push_back(status);
    }
    
    void storein(editor_keep& e) {
        e.l_keeptop = l_keeptop;
        e.l_keeplines = l_keeplines;
        e.l_keeppos = l_keeppos;
        e.l_keepactions = l_keepactions;
        e.l_keepcurrentline = l_keepcurrentline;
        e.l_keepposinstring = l_keepposinstring;
        e.l_keepstatus = l_keepstatus;
    }
    
    void prune() {
        if (l_keepactions.size() >= 10000) {
            list<wstring>::iterator srange_end = l_keeplines.begin();
            std::advance(srange_end,1000);
            l_keeplines.erase(l_keeplines.begin(), srange_end);
            
            list<long>::iterator prange_end = l_keeppos.begin();
            std::advance(prange_end,1000);
            l_keeppos.erase(l_keeppos.begin(), prange_end);
            
            list<char>::iterator arange_end = l_keepactions.begin();
            std::advance(arange_end,1000);
            l_keepactions.erase(l_keepactions.begin(), arange_end);
            
            list<long>::iterator psrange_end = l_keepposinstring.begin();
            std::advance(psrange_end,1000);
            l_keepposinstring.erase(l_keepposinstring.begin(), psrange_end);
            
            list<long>::iterator crange_end = l_keepcurrentline.begin();
            std::advance(crange_end,1000);
            l_keepcurrentline.erase(l_keepcurrentline.begin(), crange_end);
            
            list<long>::iterator trange_end = l_keeptop.begin();
            std::advance(trange_end,1000);
            l_keeptop.erase(l_keeptop.begin(), trange_end);
            
            list<char>::iterator strange_end = l_keepstatus.begin();
            std::advance(strange_end,1000);
            l_keepstatus.erase(l_keepstatus.begin(), strange_end);
        }
    }
    
    bool empty() {
        if (l_keeplines.size() == 0)
            return true;
        return false;
    }
    
    void display() {
        string s;
        wstring w = l_keeplines.back();
        s_unicode_to_utf8(s, w);
        cout << s << endl;
        cout << l_keeppos.back() << endl;
        cout << l_keepactions.back() << endl;
        cout << l_keepcurrentline.back() << endl;
        cout << l_keepposinstring.back() << endl;
        cout << l_keeptop.back() << endl;
        cout << l_keepstatus.back() << endl;
    }
    
};

typedef enum { x_none, x_goto, x_find, x_replace, x_write, x_count, x_delete, x_copy, x_cut, x_copying, x_deleting, x_cutting, x_load, x_exitprint} x_option;
class Jag_automaton;

class jag_editor {
public:
    
    vector<wstring> commandlines;
    vector<string> colors;
    vector<long> poslines;
    
    editor_lines lines;
    
    editor_keep undos;
    editor_keep redos;
    
#ifndef WIN32
    struct termios oldterm;
    struct winsize wns;
#endif
    
    std::stringstream localhelp;
    std::wstringstream st;
    
    file_types filetype;
    
    long linematch;
    
    string thecurrentfilename;
    string prefix;
    
    wstring wprefix;
    wstring line;
    wstring currentfind;
    wstring currentreplace;
    wstring kbuffer;
    wstring copybuffer;
    
#ifdef Tamgu_REGEX
    wstring wpatternexpression;
    wregex* wpattern;
#endif
    
    Jag_automaton* findrgx;
    
    
    long poscommand;
    long row_size, col_size;
    
#ifdef WIN32
    long size_row, size_col;
#endif
    long pos;
    long posinstring;
    long currentposinstring;
    long currentfindpos;
    long currentline;
    
    long margin;
    
    int prefixsize;
    int xcursor, ycursor;
    
    x_option option;
    
    bool echochar;
    bool replaceall;
    bool modified;
    bool tobesaved;
    bool tooglehelp;
    bool updateline;
    bool noprefix;
    bool insertaline;
    bool taskel;
    
    //Mouse Section
    long selected_x, selected_y, selected_pos, selected_posnext, selected_firstline, double_click;
    int nbclicks;
    
    bool activate_mouse;
    bool mouse_status;
    bool vt100;
    
    
    char regularexpressionfind;
    
    jag_editor();
    ~jag_editor();
    
    
    void switch_dark_mode() {
        if (colors[2] == m_blue)
            colors[2] = m_blueblack;
        else
            colors[2] = m_blue;
    }
    
    virtual void displaythehelp(long noclear = 0);
    
    void setpathname(string path) {
        thecurrentfilename =  path;
        if (thecurrentfilename.find(".lisp") != -1)
            filetype = lisp_type;
        else {
            if (thecurrentfilename.find(".py") != -1)
                filetype = python_type;
            else {
                if (thecurrentfilename.find(".tmg") != -1)
                    filetype = tamgu_type;
                else
                    if (thecurrentfilename.find(".java") != -1 ||
                        thecurrentfilename.find(".cpp") != -1 ||
                        thecurrentfilename.find(".cxx") != -1 ||
                        thecurrentfilename.find(".hpp") != -1 ||
                        thecurrentfilename.find(".h") != -1)
                        filetype = clike_type;
                    else
                        filetype = no_type;
            }
        }
    }
    
    string pathname() {
        return thecurrentfilename;
    }
    
    wstring wpathname() {
        wstring name = wconvert(thecurrentfilename);
        return name;
    }
    
    void resetterminal();
    void screensizes();
    
    void setscrolling();
    void reset();
    
    void resetscrolling();
    
    long colsize() {
        return col_size;
    }
    
    void clearst() {
        st.str(L"");
        st.clear();
    }
    
    virtual bool emode() {
        if (option == x_none)
            return true;
        return false;
    }
    
    //------------------------------------------------------------------------------------------------
    //Undo/Redo
    //------------------------------------------------------------------------------------------------
    
    void setnoprefix() {
        noprefix = 1-noprefix;
        if (noprefix) {
            prefix = "";
            wprefix = L"";
            prefixsize = 0;
            margin = 2;
        }
        else {
            margin = 3;
#ifdef WIN32
            prefix = "<>";
            wprefix = L"<>";
#else
            prefix = "작";
            wprefix = L"작";
#endif
            setprefixesize(lines.size());
        }
        resetscreen();
    }
    
    bool isc() {
        if (thecurrentfilename != "") {
            if (thecurrentfilename.find(".java") != -1) {
                taskel = false;
                return true;
            }
            if (thecurrentfilename.find(".c") != -1) {
                taskel = false;
                return true;
            }
            if (thecurrentfilename.find(".h") != -1) {
                taskel = false;
                return true;
            }
        }
        return false;
    }
    
    bool istamgu() {
        if (thecurrentfilename != "") {
            if (thecurrentfilename.find(".tmg") != -1)
                return true;
        }
        return false;
    }
    
    bool ispy() {
        if (thecurrentfilename != "") {
            if (thecurrentfilename.find(".py") != -1)
                return true;
        }
        return false;
    }
    
    void undo(wstring& l, long p, char a) {
        if (!emode() || p >= lines.size())
            return;
        
        modified = true;
        redos.clear();
        undos.prune();
        undos.store(poslines[0], l, p, a, currentline, posinstring, lines.Status(p));
    }
    
    void processredos();
    
    void processundos();
    
    //------------------------------------------------------------------------------------------------
    //Syntactic coloration...
    //------------------------------------------------------------------------------------------------
    
    void colorring(string& l, vector<long>& limits);
    virtual string coloringline(wstring& l, long p = -1, bool select = false);
    void vsplit(wstring& thestr, wstring thesplitter, vector<wstring>& vs);
    
    //------------------------------------------------------------------------------------------------
    //Cursor movements...
    //------------------------------------------------------------------------------------------------
    
    void selectfound(long l, long r);
    void movetoposition();
    void movetobeginning();
    void movetoend(bool remove = true);
    void movetolastline();
    void movetoline(long e);
    void gotoline(long p);
    virtual bool updown(char drt, long& pos);
    
    void getcursor();
    
    void toggletopbottom() {
        if (poslines.size() == 0)
            return;
        
        if (currentline == 0)
            currentline = poslines.size() -1;
        else
            currentline = 0;
        
        movetoline(currentline);
        pos = poslines[currentline];
        line = lines[pos];
        posinstring = line.size();
        movetoend();
    }
    
    //------------------------------------------------------------------------------------------------
    //Size calculations...
    //------------------------------------------------------------------------------------------------
    
    //Since there is always a prefix at the beginning of the line, we compute it here...
    
    long prefixesize(long sz) {
        if (noprefix)
            return 0;
        return (sz > 9999 ? 5 : sz > 999 ? 4: sz > 99 ? 3 : sz > 9 ? 2 : 1);
    }
    
    void setprefixesize(long sz) {
        if (noprefix) {
            prefixsize = 0;
            return;
        }
        prefixsize = sz > 9999 ? 5 : sz > 999 ? 4: sz > 99 ? 3 : sz > 9 ? 2 : 1 ;
    }
    
    long prefixe() {
        if (noprefix)
            return 0;
        return (4 + prefixsize);
    }
    
    virtual long prefixego() {
        wstring s;
        switch(option) {
            case x_goto:
                s = L"Line:";
                break;
            case x_find:
                if (regularexpressionfind)
                    s = L"Find(rgx):";
                else
                    s = L"Find:";
                break;
            case x_replace:
                if (regularexpressionfind)
                    s = L"Find(rgx):";
                else
                    s = L"Find:";
                s += currentfind;
                s += L"  Replace:";
                break;
            case x_write:
                s = L"File:";
                break;
            case x_count:
                s = L"Count:";
                break;
            case x_delete:
            case x_copy:
            case x_cut:
            case x_deleting:
            case x_copying:
            case x_cutting:
            case x_load:
                s = st.str();
                break;
            default:
                return prefixe();
        }
        return s.size();
    }
    
    long computeparenthesis(string& ln, char checkcar, long limit) {
        long posmatch = -1;
        vector<long> positions;
        char check;
        if (checkcar == ')')
            check = '(';
        else
            check = checkcar - 2;
        
        for (long i = 0; i < limit; i++) {
            switch (ln[i]) {
                case '"':
                    i++;
                    while (i < limit && ln[i] != '"') {
                        if (ln[i] == '\\')
                            i++;
                        i++;
                    }
                    break;
                case '`':
                    i++;
                    while (i < limit && ln[i] != '`') {
                        i++;
                    }
                    break;
                case '(':
                case '{':
                case '[':
                    if (check == ln[i])
                        positions.push_back(i);
                    break;
                case ')':
                case '}':
                case ']':
                    if (checkcar == ln[i]) {
                        if (positions.size())
                            positions.pop_back();
                    }
                    break;
            }
        }
        if (positions.size())
            posmatch = positions.back();
        return posmatch;
    }
    
    long splitline(wstring& l, long linenumber, vector<wstring>& subs);
    
    //The main problem here is that emojis can be composed...
    //An emoji can be composed of up to 7 emojis...
    void forwardemoji();
    void backwardemoji();
    
    //We find the beginning of each emoji, skipping composed ones...
    //We build a string with no composed emoji, to match the position of the cursor...
    void cleanlongemoji(wstring& s, wstring& cleaned, long p);
    
    //This size is computed to take into account Chinese/Japanese/Korean characters...
    //These characters can occupy up to two columns... We also take into account the tab size
    long taille(wstring& s);
    
    long cjk_size(wstring& l) {
        wstring cleaned;
        cleanlongemoji(l, cleaned, l.size());
        long sz = taille(cleaned);
        return (sz - cleaned.size());
    }
    
    
    
    inline long fullsize(wstring& l) {
        return taille(l);
    }
    
    long size_upto(wstring& l, long p);
    
    long linesize() {
        if (emode())
            return lines[poslines[currentline]].size();
        return line.size();
    }
    
    //--------------------------------------------------------------------------------
    //Display methods....
    //--------------------------------------------------------------------------------
    void clearscreen();
    void clearline();
    void clearlastline();
    void displayonlast(bool bck);
    void displayonlast(wstring w, bool bck = false);
    void displayonlast(string s, bool bck);
    virtual void displaygo(bool full);
    
    
    
    void resetlist(long i) {
        poslines.clear();
        long mx = i + row_size;
        
        while (i <= mx) {
            poslines.push_back(i);
            i++;
        }
    }
    
    void displaylist(long beg);
    
    virtual void printline(long n, string l) {
        if (noprefix)
            cout << back << l;
        else
            cout << back << m_dore << prefix << m_current << m_lightgray << std::setw(prefixsize) << n << "> " << m_current << l;
    }
    
    virtual void printline(long n) {
        if (noprefix)
            cout << back;
        else
            cout << back << m_dore << prefix << m_current << m_lightgray << std::setw(prefixsize) << n << "> " << m_current;
    }
    
    virtual void printline(long n, wstring& l, long i = -1) {
        if (noprefix)
            cout << back << coloringline(l, i);
        else {
            if (n == -1) {
                string space(prefixe(), ' ');
                cout << back << space << coloringline(l, i);
            }
            else
                cout << back << m_dore << prefix << m_current << m_lightgray << std::setw(prefixsize) << n << "> " << m_current << coloringline(l, i);
        }
    }
    
    //------------------------------------------------------------------------------------------------
    //Deletion methods...
    //------------------------------------------------------------------------------------------------
    
    long sizestring(wstring& s);
    
    //The deletion of a character is different if it is an emoji...
    long deleteachar(wstring& l, bool last, long pins);
    void deletechar(bool);
    
    //Delete all characters after the cursor
    void deleteallafter() {
        undo(lines[pos],pos, u_modif); //The value is negative to indicate a deletion
        
        wstring code = lines[poslines[currentline]];
        kbuffer = code.substr(posinstring, code.size());
        code = code.substr(0, posinstring);
        lines[poslines[currentline]] = code;
        lines.refactoring(poslines[currentline]);
        displaylist(poslines[0]);
        movetoline(currentline);
        movetoposition();
    }
    
    //moveup means that the cursor must be positionned on the line above...
    void deleteline(char moveup);
    
    //------------------------------------------------------------------------------------------------
    //formating method...
    //------------------------------------------------------------------------------------------------
    
    void indentcode(long ps, bool lisp) {
        if (!lines.size())
            return;
        
        wstring code = lines.code();
        
        string codeindente;
        string cd = convert(code);
        IndentCode(cd, codeindente, GetBlankSize(), lisp, taskel);
        lines.clear();
        code = wconvert(codeindente);
        code += L"\n\n";
        
        lines.setcode(code, true);
        
        displaylist(poslines[0]);
        movetoline(currentline);
        movetoposition();
    }
    
    virtual void setcode(string& c) {
        wstring code = wconvert(c);
        lines.setcode(code, true);
        displaylist(0);
        line = L"";
        currentline = 0;
        movetoline(0);
        if (poslines.size()) {
            line = lines[0];
            posinstring = line.size();
            movetoend();
        }
    }
    
    //------------------------------------------------------------------------------------------------
    //search method...
    //------------------------------------------------------------------------------------------------
    
    bool search(wstring& l, long& first, long& last, long ps);
    void processgo();
    bool processfind();
    long processcount();
    void processreplace();
    bool findnext();
    
    bool resetsearch();
    
    //------------------------------------------------------------------------------------------------
    //command methods...
    //------------------------------------------------------------------------------------------------
    virtual bool writetofile() {
        wstring code = lines.code();
        
        ofstream wd(thecurrentfilename, ios::binary);
        if (wd.fail())
            return false;
        wd << convert(code);
        wd.close();
        tobesaved = false;
        return true;
    }
    
    string getch();
    long getbuffsize();
    bool check_utf8(string& buff, string& buffer);
    
    virtual bool reloadfile() {
        if (thecurrentfilename.empty())
            return false;
        
        ifstream rd(thecurrentfilename, openMode);
        if (rd.fail()) {
            cerr << m_redbold << e_cannot_load02 << thecurrentfilename << m_current << endl;
            return false;
        }
        string ln;
        string cde;
        while (!rd.eof()) {
            getline(rd, ln);
            ln = Trimright(ln);
            cde += ln + "\n";
        }
        
        wstring code = wconvert(cde);
        lines.setcode(code, true);
        
        currentline = 0;
        posinstring = 0;
        pos = 0;
        option = x_none;
        line = lines[0];
        displaylist(0);
        movetoline(currentline);
        movetobeginning();
        return true;
    }
    
    virtual bool loadfile(wstring& name) {
        if (!loadfile(convert(name))) {
            clearst();
            st << L"Cannot load:" << name;
            displayonlast(true);
            return false;
        }
        return true;
    }
    
    virtual bool loadfile(string name) {
        setpathname(name);
        ifstream rd(pathname(), openMode);
        if (rd.fail())
            return false;
        
        string code = "";
        string line;
        while (!rd.eof()) {
            getline(rd, line);
            line = Trimright(line);
            code += line + "\n";
        }
        Trimright(code);
        code += "\n\n";
        setcode(code);
        return true;
    }
    
    string convert(wstring& w) {
        string s;
        s_unicode_to_utf8(s, w);
#ifdef WIN32
        if (isDosOutput())
            return s_utf8_to_dos(STR(s));
#endif
        return s;
    }
    
    wstring wconvert(string& s) {
        wstring w;
        s_utf8_to_unicode(w, USTR(s), s.size());
        return w;
    }
    
    
    //A CR was hit, we need either to modify the current line or to add a new one...
    //If the cursor was in the middle of a line, then we split it in two
    
    long handlemultiline() ;
    long handlingeditorline(bool computespace = true);
    void Scrolldown();
    
    bool evaluateescape(string& buff);
    virtual void init();
    virtual void clear();
    
    
    //This is the main instruction to add characters into the editor
    void addabuffer(wstring& b, bool instring);
    
    void handleblock(wstring& bl) {
        
        //We keep track of the initial form of the line...
        undo(lines[pos],pos, u_modif); //The value is negative to indicate a deletion
        
        wstring b;
        for (long j = 0; j < bl.size(); j++) {
            b = bl[j];
            if (b[0] == 10) {
                pos = handlingeditorline(false);
                continue;
            }
            addabuffer(b, false);
        }
        displaylist(poslines[0]);
        movetoline(currentline);
        movetoposition();
    }
    
    void convertmetas();
    //This section handles combined commands introduced with Ctrl-x
    virtual bool checkcommand(char);
    void handlecommands();
    
    //This a case of copy/paste within the editor, we need to remove the prefixes
    void cleanheaders(wstring& w);
    //This is the main method that launches the terminal
    virtual void launchterminal(char loadedcode, vector<string>& newcolors);
    bool checkaction(string&, long& first, long& last, bool lsp = false);
    
    virtual void addcommandline(wstring& w) {}
    
    virtual bool terminate();
    
    virtual void resetscreen() {
        modified = true;
        screensizes();
        wstring code = lines.code();
        lines.setcode(code, false);
        displaylist(poslines[0]);
        movetoline(currentline);
        posinstring = 0;
        movetobeginning();
    }
    
    bool checksize(wstring& l) {
        if (fullsize(l) > col_size)
            return true;
        return false;
    }
    
    bool checksizeequal(wstring& l, bool& equal) {
        long ll = fullsize(l);
        if (ll == col_size) {
            equal = true;
            return true;
        }
        
        if (ll > col_size)
            return true;
        return false;
    }
    
    string thecode() {
        wstring c = lines.code();
        return convert(c);
    }
    
    void indentplus();
    void deindentminus();
    
    //-----------------------------------------------------------------------------
    //Mouse Section
    //-----------------------------------------------------------------------------
    void handlemousectrl(string& mousectrl);
    void selectlines(long from_line, long to_line, long from_pos, long to_pos);
    void unselectlines(long from_line, long to_line, long from_pos, long to_pos);
    void displayextract(wstring& sub, long pos, long from_pos, long to_pos, bool select = true);
    void deleteselection();
    void computeposition(int& p, long line);
    
    inline void resetselection() {
        selected_firstline = -1;
        selected_x = -1;
        selected_y = -1;
        selected_pos = -1;
        selected_posnext = -1;
    }
    
    void sendcommand(string s) {
        cout.flush();
        cout << s;
    }
    
#ifdef WIN32
    void mouseon() {
        activate_mouse = true;
        mouse_status = true;
    }
    
    void mouseoff() {
        mouse_status = false;
    }
    
    void togglemouse() {
        mouse_status = 1 - mouse_status;
        if (mouse_status)
            activate_mouse = true;
    }
    
#else
    
    void mouseon() {
        if (vt100)
            sendcommand(enablemouse_vt100);
        else
            sendcommand(enablemouse);
        mouse_status = true;
        activate_mouse = true;
    }
    
    void mouseoff() {
        sendcommand(disablemouse);
        mouse_status = false;
    }
    
    void togglemouse() {
        if (!mouse_status) {
            if (vt100)
                sendcommand(enablemouse_vt100);
            else
                sendcommand(enablemouse);
            mouse_status = true;
            activate_mouse = true;
        }
        else {
            sendcommand(disablemouse);
            mouse_status = false;
        }
    }
#endif
    
    bool isScrollingUp_VT100(vector<int>& vect, string& mousectrl) {
        int action, mxcursor, mycursor;
        if (mouse_status && mousectrl.size() >= 6 && mousectrl[0] == 27 && mousectrl[1] == '[' && mousectrl[2] == 'M') {
            //This a move
            action = mousectrl[3];
            mxcursor = mousectrl[4] - 32;
            mycursor = mousectrl[5] - 32;
            if (action == 96) {
                vect.push_back(mycursor);
                vect.push_back(mxcursor);
                return true;
            }
        }
        return false;
    }
    
    bool isScrollingDown_VT100(vector<int>& vect, string& mousectrl) {
        int action, mxcursor, mycursor;
        if (mouse_status && mousectrl.size() >= 6 && mousectrl[0] == 27 && mousectrl[1] == '[' && mousectrl[2] == 'M') {
            //This a move
            action = mousectrl[3];
            mxcursor = mousectrl[4] - 32;
            mycursor = mousectrl[5] - 32;
            if (action == 97) {
                vect.push_back(mycursor);
                vect.push_back(mxcursor);
                return true;
            }
        }
        return false;
    }
    
    long nbClicks() {
        return nbclicks;
    }
    
    bool isClickFirstMouseDown_VT100(vector<int>& vect, string& mousectrl) {
        int action, mxcursor, mycursor;
        if (mouse_status && mousectrl.size() >= 6 && mousectrl[0] == 27 && mousectrl[1] == '[' && mousectrl[2] == 'M') {
            action = mousectrl[3];
            mxcursor = mousectrl[4] - 32;
            mycursor = mousectrl[5] - 32;
            if (action == 32) {
                vect.push_back(mycursor);
                vect.push_back(mxcursor);
                return true;
            }
        }
        return false;
    }
    
    bool checkMouseup_VT100(string& mousectrl) {
        string mouse_up;
        mouse_up = 27;
        mouse_up += '[';
        mouse_up += 35;
        
        if (mouse_status && mousectrl.size() >= 6 && mousectrl.back() == 'M') {
            return (mousectrl.find(mouse_up) != -1);
        }
        return false;
    }
    
    bool isClickMouseUp_VT100(vector<int>& vect, string& mousectrl) {
        int action, mxcursor, mycursor;
        if (mouse_status && mousectrl.size() >= 6 && mousectrl[0] == 27 && mousectrl[1] == '[' && mousectrl[2] == 'M') {
            //This a move
            action = mousectrl[3];
            mxcursor = mousectrl[4] - 32;
            mycursor = mousectrl[5] - 32;
            if (action == 35) {
                vect.push_back(mycursor);
                vect.push_back(mxcursor);
                return true;
            }
        }
        return false;
    }
    
    bool isClickSecondMouseDown_VT100(vector<int>& vect, string& mousectrl) {
        int action, mxcursor, mycursor;
        if (mouse_status && mousectrl.size() >= 6 && mousectrl[0] == 27 && mousectrl[1] == '[' && mousectrl[2] == 'M') {
            //This a move
            action = mousectrl[3];
            mxcursor = mousectrl[4] - 32;
            mycursor = mousectrl[5] - 32;
            if (action == 34) {
                vect.push_back(mycursor);
                vect.push_back(mxcursor);
                return true;
            }
        }
        return false;
    }
    
    bool mouseTracking_VT100(string& mousectrl, int& mxcursor, int& mycursor) {
        int action;
        if (mouse_status && mousectrl.size() >= 6 && mousectrl[0] == 27 && mousectrl[1] == '[' && mousectrl[2] == 'M') {
            //This a move
            action = mousectrl[3];
            mxcursor = mousectrl[5] - 32;
            mycursor = mousectrl[4] - 32;
            if (action == 64)
                return true;
        }
        return false;
    }
    
    bool isMouseAction_VT100(string mousectrl) {
        return (mouse_status && mousectrl.size() >= 6 && mousectrl[0] == 27 && mousectrl[1] == '[' && mousectrl[2] == 'M');
    }
    
    bool isMouseAction_VT100(std::wstring& mousectrl) {
        return (mouse_status && mousectrl.size() >= 6 && mousectrl[0] == 27 && mousectrl[1] == '[' && mousectrl[2] == 'M');
    }
    
    void mouseLocation(vector<int>& vect, string& mousectrl) {
        int action, mxcursor, mycursor;
        if (mouse_status && mousectrl.size() >= 8 && mousectrl.back() == 'M' && mousectrl[0] == 27 && mousectrl[1] == '[') {
            //This a move
            sscanf(STR(mousectrl), "\033[%d;%d;%dM", &action, &mycursor, &mxcursor);
            if (action == 67) {
                vect.push_back(mxcursor);
                vect.push_back(mycursor);
            }
        }
    }
    
    bool isScrollingUp(vector<int>& vect, string& mousectrl) {
        if (vt100)
            return isScrollingUp_VT100(vect, mousectrl);
        
        int action, mxcursor, mycursor;
        if (mouse_status && mousectrl.size() >= 8 && mousectrl.back() == 'M' && mousectrl[0] == 27 && mousectrl[1] == '[') {
            //This a move
            sscanf(STR(mousectrl), "\033[%d;%d;%dM", &action, &mycursor, &mxcursor);
            if (action == 96) {
                vect.push_back(mxcursor);
                vect.push_back(mycursor);
                return true;
            }
        }
        return false;
    }
    
    bool isScrollingDown(vector<int>& vect, string& mousectrl) {
        if (vt100)
            return isScrollingDown_VT100(vect, mousectrl);
        
        int action, mxcursor, mycursor;
        if (mouse_status && mousectrl.size() >= 8 && mousectrl.back() == 'M' && mousectrl[0] == 27 && mousectrl[1] == '[') {
            //This a move
            sscanf(STR(mousectrl), "\033[%d;%d;%dM", &action, &mycursor, &mxcursor);
            if (action == 97) {
                vect.push_back(mxcursor);
                vect.push_back(mycursor);
                return true;
            }
        }
        return false;
    }
    
    
    bool isClickFirstMouseDown(vector<int>& vect, string& mousectrl) {
        if (vt100)
            return isClickFirstMouseDown_VT100(vect, mousectrl);
        
        int action, mxcursor, mycursor;
        if (mouse_status && mousectrl.size() >= 8 && mousectrl.back() == 'M' && mousectrl[0] == 27 && mousectrl[1] == '[') {
            //This a move
#ifdef WIN32
            //On Windows, a double-click contains a D
            if (mousectrl[mousectrl.size() - 2] == 'D') {
                sscanf(STR(mousectrl), "\033[%d;%d;%dDM", &action, &mycursor, &mxcursor);
                nbclicks = 2;
            }
            else {
                sscanf(STR(mousectrl), "\033[%d;%d;%dM", &action, &mycursor, &mxcursor);
                nbclicks = 1;
            }
#else
            sscanf(STR(mousectrl), "\033[%d;%d;%dM", &action, &mycursor, &mxcursor);
#endif
            if (action == 32) {
                vect.push_back(mxcursor);
                vect.push_back(mycursor);
                return true;
            }
        }
        return false;
    }
    
    bool checkMouseup(string& mousectrl) {
        if (vt100)
            return checkMouseup_VT100(mousectrl);
        
        string mouse_up;
        mouse_up = 27;
        mouse_up += '[';
        mouse_up += "35";
        
        
        if (mouse_status && mousectrl.size() >= 8 && mousectrl.back() == 'M') {
            return (mousectrl.find(mouse_up) != -1);
        }
        return false;
    }
    
    bool isClickMouseUp(vector<int>& vect, string& mousectrl) {
        if (vt100)
            return isClickMouseUp_VT100(vect, mousectrl);
        
        int action, mxcursor, mycursor;
        if (mouse_status && mousectrl.size() >= 8 && mousectrl.back() == 'M' && mousectrl[0] == 27 && mousectrl[1] == '[') {
            //This a move
#ifdef WIN32
            //On Windows, a double-click contains a D
            if (mousectrl[mousectrl.size() - 2] == 'D')
                sscanf(STR(mousectrl), "\033[%d;%d;%dDM", &action, &mycursor, &mxcursor);
            else
                sscanf(STR(mousectrl), "\033[%d;%d;%dM", &action, &mycursor, &mxcursor);
#else
            sscanf(STR(mousectrl), "\033[%d;%d;%dM", &action, &mycursor, &mxcursor);
#endif
            
            if (action == 35) {
                vect.push_back(mxcursor);
                vect.push_back(mycursor);
                return true;
            }
        }
        return false;
    }
    
    bool isClickSecondMouseDown(vector<int>& vect, string& mousectrl) {
        if (vt100)
            return isClickSecondMouseDown_VT100(vect, mousectrl);
        
        int action, mxcursor, mycursor;
        if (mouse_status && mousectrl.size() >= 8 && mousectrl.back() == 'M' && mousectrl[0] == 27 && mousectrl[1] == '[') {
            //This a move
#ifdef WIN32
            //On Windows, a double-click contains a D
            if (mousectrl[mousectrl.size() - 2] == 'D') {
                sscanf(STR(mousectrl), "\033[%d;%d;%dDM", &action, &mycursor, &mxcursor);
                nbclicks = 2;
            }
            else {
                sscanf(STR(mousectrl), "\033[%d;%d;%dM", &action, &mycursor, &mxcursor);
                nbclicks = 1;
            }
#else
            sscanf(STR(mousectrl), "\033[%d;%d;%dM", &action, &mycursor, &mxcursor);
#endif
            if (action == 34) {
                vect.push_back(mxcursor);
                vect.push_back(mycursor);
                return true;
            }
        }
        return false;
    }
    
    bool mouseTracking(string& mousectrl, int& mxcursor, int& mycursor) {
        if (vt100)
            return mouseTracking_VT100(mousectrl, mxcursor, mycursor);
        
        int action;
        if (mouse_status && mousectrl.size() >= 8 && mousectrl.back() == 'M' && mousectrl[0] == 27 && mousectrl[1] == '[') {
            //This a move
            sscanf(STR(mousectrl), "\033[%d;%d;%dM", &action, &mycursor, &mxcursor);
            if (action == 64)
                return true;
        }
        return false;
    }
    
    bool isMouseAction(string& mousectrl) {
        if (vt100)
            return isMouseAction_VT100(mousectrl);
        
        return (mouse_status && mousectrl.size() >= 8 && mousectrl.back() == 'M' && mousectrl[0] == 27 && mousectrl[1] == '[');
    }
    
    bool isMouseAction(std::wstring& mousectrl) {
        if (vt100)
            return isMouseAction_VT100(mousectrl);
        
        return (mouse_status && mousectrl.size() >= 8 && mousectrl.back() == L'M' && mousectrl[0] == 27 && mousectrl[1] == L'[');
    }
    
};

#endif

