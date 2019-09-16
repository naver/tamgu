/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : jag.cxx
 Date       : 2019/03/25
 Purpose    : Main of Jag (작) executable
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#include <stdio.h>

#include <unistd.h>   //_getch
#include <termios.h>  //_getch
#include <sys/ioctl.h>
#include <signal.h>
#include <iomanip>

#include <string>

#include "conversion.h"
#include "x_tokenize.h"
#include "jagrgx.h"
#include <iomanip>
#include "jag.h"


static char* _keywords[] = { "GPSdistance","True", "False","None","def","char","import","double","wstring","Maybe","Nothing","_breakpoint","_dependencies","_erroronkey","_eval","_evalfunction","_exit","_forcelocks","_getdefaulttokenizerules","_info","_initial","_mirrordisplay","_nbthreads","_poolstats","_setenv","_setmaxrange","_setmaxthreads","_setstacksize","_setvalidfeatures","_stdin","_symbols","_threadhandle","_threadid","a_bool","a_change","a_delete","a_features","a_first","a_float","a_fvector","a_insert","a_int","a_ivector","a_longest","a_mapff","a_mapfi","a_mapfs","a_mapfu","a_mapif","a_mapii","a_mapis","a_mapiu","a_mapsf","a_mapsi","a_mapss","a_mapuf","a_mapui","a_mapuu","a_nocase","a_offsets","a_random","a_repetition","a_string","a_surface","a_svector","a_switch","a_ustring","a_uvector","a_vector","a_vowel","abs","absent","acos","acosh","activate","add","addchild","addnext","addprevious","addstyle","after","alert","align","all","allobjects","and","annotate","annotator","any","aopen","append","apply","arc","asin","asinh","ask","assert","asserta","assertz","atan","atanh","attributes","autorun","backgroundcolor","barcolor","base","before","begin","begincomplexpolygon","beginline","beginloop","beginpoints","beginpolygon","binmap","binmapf","binmapi","binmapl","binmaps","binmapu","bit","bitmap","bits","block","blocking","bloop","bool","border","boundaries","bounds","box","boxtype","break","browser","buffersize","build","button","bvector","byte","byteposition","bytes","call","case","cast","catch","cbrt","cell","cellalign","cellalignheadercol","cellalignheaderrow","charposition","check","checklabel","checkword","child","children","choice","chr","circle","clear","close","color","colorbg","colorfg","column","columnchar","columnheader","columnheaderwidth","columnwidth","command","commit","common","compact","compile","compilelexicons","compilergx","concept","connect","const","constmap","constvector","content","continue","copy","cos","cosh","count","counter","create","created","createdirectory","createserver","curl","current","cursor","cursorchar","cursorstyle","curve","cut","cycle","data","date","day","deaccentuate","decimal","decode","default","definitions","degree","delete","dependencies","dependency","deriving","deselect","determinant","dimension","dloop","do","document","dos","dostoutf8","doublemetaphone","drawcolor","drawtext","drop","dropWhile","dthrough","dvector","e_arabic","e_baltic","e_celtic","e_cp1252","e_cyrillic","e_greek","e_hebrew","e_latin_ce","e_latin_ffe","e_latin_ne","e_latin_se","e_latin_see","e_latin_we","e_nordic","e_thai","e_turkish","e_windows","editdistance","editor","elif","else","emoji","emojis","empty","encode","end","endcomplexpolygon","endian","endline","endloop","endpoints","endpolygon","env","eof","erf","erfc","evaluate","even","exclusive","execute","exit","exp","exp2","expm1","exponent","extension","extract","factorize","factors","fail","false","features","fibre","file","filebrowser","fileinfo","fill","filter","find","findall","first","flatten","flip","float","floop","floor","flush","fmatrix","focus","foldl","foldr","font","fontnumber","fontsize","for","format","formatchar","fraction","frame","frameinstance","from","fthrough","function","fvector","gap","get","geterr","getfont","getfontsizes","gethostname","getpeername","getstd","getstyle","gotoline","grammar","grammar_macros","group","hash","hide","highlight","hour","html","hvector","id","if","ifnot","iloop","image","imatrix","in","indent","info","initializefonts","insert","insertbind","int","invert","is","isa","isalpha","isconsonant","iscontainer","isdigit","isdirectory","isemoji","ishangul","isjamo","islower","ismap","isnumber","isprime","ispunctuation","isstring","isupper","isutf8","isvector","isvowel","items","iterator","ithrough","ivector","jamo","join","joined","json","key","keys","kget","label","labelcolor","labelfont","labels","labelsize","labeltype","last","latin","leaves","left","length","let","levenshtein","lexicon","lgamma","line","linebounds","linecharbounds","lineshape","lisp","list","listdirectory","lloop","ln","load","loadgif","loadin","loadjpeg","lock","log","log1p","log2","logb","long","lookdown","lookup","loop","lower","ls","lstep","lthrough","lvector","mantissa","map","mapf","mapff","mapfi","mapfl","mapfs","mapfu","mapi","mapif","mapii","mapis","mapiu","mapl","maplf","mapll","mapls","maplu","mapsf","mapsi","mapsl","mapss","mapu","mapuf","mapui","mapul","mapuu","mark","match","max","maximum","memory","menu","merge","method","methods","mid","min","minimum","minute","mkdir","modal","month","mp3","mthrough","multisplit","multmatrix","name","namespace","nbchildren","nd","nearbyint","new","next","ngrams","node","nope","normalizehangul","not","notin","null","odd","of","ok","onclose","onclosing","ondragdrop","onhscroll","onkey","onmouse","ontime","ontology","onvscroll","open","openappend","openread","openwrite","options","or","ord","otherwise","padding","parameters","parent","parse","password","paste","pause","permute","pie","play","plot","plotcoords","point","polygon","polynomial","ponder","pop","popclip","popfirst","poplast","popmatrix","port","position","post","precede","pred","predicate","predicatedump","predicatevar","preg","previous","primemap","primemapf","primemapff","primemapfi","primemapfl","primemapfs","primemapfu","primemapi","primemapif","primemapii","primemapis","primemapiu","primemapl","primemaplf","primemapll","primemapls","primemaplu","primemapsf","primemapsi","primemapsl","primemapss","primemapu","primemapuf","primemapui","primemapul","primemapuu","print","printerr","printj","printjerr","printjln","printjlnerr","println","printlnerr","private","product","progress","properties","property","protected","proxy","push","pushclip","pushfirst","pushlast","pushmatrix","radian","raise","random","range","rawstring","read","readline","real","realpath","receive","rectangle","rectanglefill","redirectoutput","redraw","redrawing","remove","removefirst","removelast","repeat","replace","replaceall","replicate","reserve","reset","resizable","resize","restateoutput","retract","retractall","return","reverse","rfind","rgbcolor","right","ring","rint","role","romanization","root","ropen","rotate","rotation","round","row","rowheader","rowheaderheight","rowheight","rule","run","save","scale","scan","scanl","scanr","scrollbar","second","seek","select","selection","selectioncolor","self","send","serialize","serializestring","setdate","setstyle","settimeout","short","shortcut","show","shuffle","sibling","signature","simplify","sin","sinh","sisters","size","sizeb","sizerange","sleep","slice","slider","sloop","socket","sort","sortfloat","sortint","sortstring","sortustring","sound","spans","split","splite","sqlite","sqrt","static","step","steps","sthrough","stokenize","stop","store","strict","string","succ","succeed","sum","svector","switch","synode","sys","table","tabs","tags","take","takeWhile","tamgu","tan","tanh","tell","term","test","textcolor","textsize","tgamma","this","thread","time","tohtml","token","tokenize","tokens","tooltip","toxml","trace","transducer","transformdx","transformdy","transformedvertex","transformx","transformy","translate","transpose","treemap","treemapf","treemapff","treemapfi","treemapfl","treemapfs","treemapfu","treemapi","treemapif","treemapii","treemapis","treemapiu","treemapl","treemaplf","treemapll","treemapls","treemaplu","treemapsf","treemapsi","treemapsl","treemapss","treemapu","treemapuf","treemapui","treemapul","treemapuu","treg","trim","trimleft","trimright","true","trunc","try","type","ufile","uloop","unblock","unget","unhighlight","unique","unlink","unlock","unmark","upper","url","use","ustring","utf8","uthrough","uvector","value","values","vector","version","vertex","vthrough","wait","waitonfalse","waitonjoined","weekday","wfile","when","where","while","window","winput","with","wopen","word","words","woutput","wrap","write","writebin","writeln","writeutf16","wtable","xml","xmldoc","xmlstring","xmltype","xor","xpath","year","yearday","zip","zipWith","∏","∑","√","∛","define", "ifdef", "endif", "include", ""};


void jag_editor::displaythehelp(long noclear) {
    if (!noclear) {
        cout << m_clear << m_home;
        cerr << m_redital << "jag editor help" << m_current << endl << endl;
    }
    
    cerr << "   " << m_redbold << "Commands" << m_current << endl;
    cerr << "   \t- " << m_redbold << "Ctrl-k:" << m_current << " delete from cursor up to the end of the line" << endl;
    cerr << "   \t- " << m_redbold << "Ctrl-d:" << m_current << " delete a full line" << endl;
    cerr << "   \t- " << m_redbold << "Ctrl-p:" << m_current << " insert k-buffer (from Ctrl-d or Ctrl-k)" << endl;
    cerr << "   \t- " << m_redbold << "Ctrl-u:" << m_current << " undo last modifications" << endl;
    cerr << "   \t- " << m_redbold << "Ctrl-r:" << m_current << " redo last modifications" << endl;
    cerr << "   \t- " << m_redbold << "Ctrl-f:" << m_current << " find a string (on the same line: " <<  m_redital << "Ctrl-r" << m_current <<" for replacement)" << endl;
    cerr << "   \t- " << m_redbold << "Ctrl-n:" << m_current << " find next" << endl;
    cerr << "   \t- " << m_redbold << "Ctrl-g:" << m_current << " move to a specific line, '$' is the end of the code" << endl;
    cerr << "   \t- " << m_redbold << "Ctrl-l:" << m_current << " toggle between top and bottom of the screen" << endl;
    cerr << "   \t- " << m_redbold << "Ctrl-t:" << m_current << " reindent the code" << endl;
    cerr << "   \t- " << m_redbold << "Ctrl-h:" << m_current << " local help" << endl;
    cerr << "   \t- " << m_redbold << "Ctrl-w:" << m_current << " write file to disk" << endl;
    cerr << "   \t- " << m_redbold << "Ctrl-c:" << m_current << " exit the editor" << endl;
    cerr << "   \t- " << m_redbold << "Ctrl-x:" << m_redital << " Combined Commands" << m_current << endl;
    cerr << "   \t\t- " << m_redital << "C:" << m_current << " count a pattern" << endl;
    cerr << "   \t\t- " << m_redital << "f:" << m_current << " find with a RGX" << endl;
    cerr << "   \t\t- " << m_redital << "H:" << m_current << " convert HTML entities to Unicode characters" << endl;
    cerr << "   \t\t- " << m_redital << "D:" << m_current << " delete a bloc of lines" << endl;
    cerr << "   \t\t- " << m_redital << "c:" << m_current << " copy a bloc of lines" << endl;
    cerr << "   \t\t- " << m_redital << "x:" << m_current << " cut a bloc of lines" << endl;
    cerr << "   \t\t- " << m_redital << "v:" << m_current << " paste a bloc of lines" << endl;
    cerr << "   \t\t- " << m_redital << "w:" << m_current << " write and quit" << endl;
    cerr << "   \t\t- " << m_redital << "l:" << m_current << " load a file" << endl;
    cerr << "   \t\t- " << m_redital << "h:" << m_current << " full help" << endl;
    cerr << "   \t\t- " << m_redital << "q:" << m_current << " quit" << endl << endl;

    cerr << "   " << m_redbold << "Regular expressions for " << m_redital << "'find'" << m_current << endl;
    cerr << "   \t- " << m_redbold << "%d" << m_current <<"\t\tstands for any digit" << m_current << endl;
    cerr << "   \t- " << m_redbold << "%x" << m_current <<"\t\tstands for a hexadecimal digit (abcdef0123456789ABCDEF)" << m_current << endl;
    cerr << "   \t- " << m_redbold << "%p" << m_current <<"\t\tstands for any punctuation" << m_current << endl;
    cerr << "   \t- " << m_redbold << "%c" << m_current <<"\t\tstands for any lower case letter" << m_current << endl;
    cerr << "   \t- " << m_redbold << "%C" << m_current <<"\t\tstands for any upper case letter" << m_current << endl;
    cerr << "   \t- " << m_redbold << "%a" << m_current <<"\t\tstands for any letter" << m_current << endl;
    cerr << "   \t- " << m_redbold << "?" << m_current <<"\t\tstands for any character" << m_current << endl;
    cerr << "   \t- " << m_redbold << "%?" << m_current <<"\t\tstand for the character “?” itself" << m_current << endl;
    cerr << "   \t- " << m_redbold << "%%" << m_current <<"\t\tstand for the character “%” itself" << m_current << endl;
    cerr << "   \t- " << m_redbold << "%s" << m_current <<"\t\tstand for any space character include the non-breaking space" << m_current << endl;
    cerr << "   \t- " << m_redbold << "%r" << m_current <<"\t\tstand a carriage return" << m_current << endl;
    cerr << "   \t- " << m_redbold << "%n" << m_current <<"\t\tstand for a non-breaking space" << m_current << endl;
    cerr << "   \t- " << m_redbold << "~" << m_current <<"\t\tnegation" << m_current << endl;
    cerr << "   \t- " << m_redbold << "\\x" << m_current <<"\t\tescape character" << m_current << endl;
    cerr << "   \t- " << m_redbold << "\\ddd" << m_current <<"\t\tcharacter code across 3 digits exactly" << m_current << endl;
    cerr << "   \t- " << m_redbold << "\\xFFFF" << m_current <<"\tcharacter code across 4 hexas exactly" << m_current << endl;
    cerr << "   \t- " << m_redbold << "{a-z}" << m_current <<"\t\tbetween a and z included" << m_current << endl;
    cerr << "   \t- " << m_redbold << "[a-z]" << m_current <<"\t\tsequence of characters" << m_current << endl;
    cerr << "   \t- " << m_redbold << "^" << m_current <<"\t\tthe expression should start at the beginning of the string" << m_current << endl;
    cerr << "   \t- " << m_redbold << "$" << m_current <<"\t\tthe expression should match up to the end of the string" << m_current << endl << endl;

    cerr << "   " << m_redbold << "Examples:" << m_current << endl;
    cerr << "   \t- " << m_redbold << "dog%c    matches dogs or dogg" << m_current << endl;
    cerr << "   \t- " << m_redbold << "m%d      matches m0, m1,…,m9" << m_current << endl;
    cerr << "   \t- " << m_redbold << "{%dab}   matches 1, a, 2, b" << m_current << endl;
    cerr << "   \t- " << m_redbold << "{%dab}+  matches 1111a, a22a90ab" << m_current << endl;
}

//--------------------------------------------------------
static void keywords(hmap<string,bool>& names) {
    for (int i = 0; _keywords[i][0] != 0; i++)
        names[_keywords[i]]=true;
}

//--------------------------------------------------------
static const short _getbuffsize = 128;
static int xcursor = 0, ycursor = 0;

static int getxcursor() {
    return xcursor;
}

static int getycursor() {
    return ycursor;
}

static string _getch(){
    static char buf[_getbuffsize+2];
    memset(buf,0, _getbuffsize);

    struct termios old={0};
    fflush(stdout);
    if(tcgetattr(0, &old)<0) {
        //we reset the input stream
        freopen("/dev/tty", "rw", stdin);
        //and we try again...
        if(tcgetattr(0, &old)<0) {
            perror("tcsetattr()");
            exit(-1);
        }
    }
    
    old.c_lflag&=~ICANON;
    old.c_lflag&=~ECHO;
    old.c_cc[VMIN]=1;
    old.c_cc[VTIME]=0;
    if(tcsetattr(0, TCSANOW, &old)<0) {
        perror("tcsetattr ICANON");
        return "";
    }
    
    //If you need to get the absolute cursor position, you can decomment these lines
    //cout << cursor_position;
    //scanf("\033[%d;%dR", &xcursor, &ycursor);

    string res;
    long nb;
    
    do {
        nb = read(0,buf,_getbuffsize);
        if (nb < 0)
            perror("read()");
        buf[nb] = 0;
        res += buf;
    }
    while (nb == _getbuffsize);
    
    old.c_lflag|=ICANON;
    old.c_lflag|=ECHO;
    if(tcsetattr(0, TCSADRAIN, &old)<0) {
        perror ("tcsetattr ~ICANON");
        return "";
    }
    
    return res;
}

//--------------------------------------------------------
void jag_editor::colorring(const char* txt, vector<long>& limits) {
    static hmap<string,bool> keys;
    static x_coloringrule xr;
    static bool init=false;

    if (!init) {
        init=true;
        keywords(keys);
    }

    xr.tokenize(txt, true);
    char type;
    long gauche,droite,i;
    long sz=xr.stack.size();
    string sub;

    for (i=0;i<sz;i++) {
        type=xr.stacktype[i];
        gauche=xr.bpos[i];
        droite = gauche + xr.stack[i].size();

        switch(type) {
            case 1:
            case 2:
            case 3:
                //strings
                limits.push_back(type);
                limits.push_back(gauche);
                limits.push_back(droite);
                break;
            case 4://regular token
                if (keys.find(xr.stack[i])!=keys.end()) {
                    limits.push_back(5);
                    limits.push_back(gauche);
                    limits.push_back(droite);
                    break;
                }
                break;
            case 5://comments
                limits.push_back(7);
                limits.push_back(gauche);
                limits.push_back(droite);
                break;
            case 10:
                //special variables: #d+ ?label $d+
                limits.push_back(8);
                limits.push_back(gauche);
                limits.push_back(droite);
                break;
            case 11:
                 // .method(
                limits.push_back(4);
                limits.push_back(gauche+1);
                limits.push_back(droite-1);
                break;
            case 12:
                // function(
                sub=xr.stack[i].substr(0,xr.stack[i].size()-1);
                if (keys.find(sub)!=keys.end()) {
                    limits.push_back(5);
                    limits.push_back(gauche);
                    limits.push_back(droite-1);
                    break;
                }
                limits.push_back(6);
                limits.push_back(gauche);
                limits.push_back(droite-1);
                break;
            case 13:
                    // <function
                sub=xr.stack[i].substr(1,xr.stack[i].size()-1);
                if (keys.find(sub)!=keys.end()) {
                    limits.push_back(5);
                    limits.push_back(gauche);
                    limits.push_back(droite-1);
                    break;
                }
                limits.push_back(6);
                limits.push_back(gauche+1);
                limits.push_back(droite);
                break;
            case 14:
                    //annotation lexicon head rule
                sub=xr.stack[i];
                limits.push_back(6);
                limits.push_back(gauche);
                limits.push_back(droite);
                break;
        }
    }
}

///------------------------------------------------------------------------------------
//We check if the buffer ends in an incomplete utf8 character...
//In that case, we remove the ending and return it as a value to be added later
bool jag_editor::check_utf8(string& buff, string& buffer) {
    long sz = buff.size()-1;
    unsigned char utf[4];
    utf[2] = buff[sz];
    utf[1] = buff[sz-1];
    utf[0] = buff[sz-2];
    
    if (utf[2] < 0x80)
        return false;
    
    if ((utf[2] & 0xF0)== 0xF0 || (utf[2] & 0xE0) == 0xE0 || (utf[2] & 0xC0) == 0xC0) {
        buff = buff.substr(0, sz);
        buffer = utf[2];
        return true;
    }
    
    if ((utf[2] & 0xC0) == 0xC0)
        return false;
    
    if ((utf[1] & 0xF0) == 0xF0 || (utf[1] & 0xE0) == 0xE0) {
        buff = buff.substr(0, sz - 1);
        buffer = utf[1];
        buffer += utf[2];
        return true;
    }
    
    if ((utf[0] & 0xE0)== 0xE0)
        return false;
    
    if ((utf[0] & 0xF0)== 0xF0) {
        buff = buff.substr(0, sz - 2);
        buffer = utf[1];
        buffer += utf[2];
        buffer += utf[3];
        return true;
    }
    
    return false;
}

static void convertmeta(wstring& w, wstring& wsub) {
    Au_automate a("{[%%%x%x][&%c+;][&#%d+;][u%x%x%x%x][\\u%x%x%x%x][\\%d%d%d][\\?]}");
    
    vector<long> vectr;
    a.searchall(wsub,vectr);
    if (vectr.size()==0) {
        w=wsub;
        return;
    }
    
    w=wsub;
    wstring s;
    for (long i=vectr.size()-2;i>=0; i-=2) {
        s=wsub.substr(vectr[i], vectr[i+1]-vectr[i]);
        s_EvaluateMetaCharacters(s);
        w=w.substr(0,vectr[i])+s+w.substr(vectr[i+1],w.size()-vectr[i+1]);
    }
}


//The actual size of the displayed string, the problem here is that multibyte characters are sometimes displayed with an extra-space...
//Especially for CJK characters.... (Chinese, Japanese, Korean)... We need to integrate this extra-space into our calculus...

//Lines extracted from the function "mk_wcwidth": https://www.cl.cam.ac.uk/~mgk25/ucs/wcwidth.c
static inline bool ckjchar(wchar_t ucs) {
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

///------------------------------------------------------------------------------------
void jag_editor::vsplit(wstring& thestr, wstring thesplitter, vector<wstring>& vs) {
    s_split(thestr, thesplitter, vs, true);
}

///------------------------------------------------------------------------------------
static void displaychar(string& bf) {
    for (int i=0; i < bf.size(); i++)
        cout << (int)bf[i] << " ";
    cout << endl;
}

///------------------------------------------------------------------------------------
jag_editor* JAGEDITOR = NULL;
///------------------------------------------------------------------------------------
static void resizewindow(int theSignal) {
    JAGEDITOR->resetscreen();
}

static void handle_ctrl_c(int theSignal) {
    JAGEDITOR->terminate();
}

///------------------------------------------------------------------------------------

jag_editor::jag_editor() : lines(this) {
    tcgetattr(0, &oldterm);

    tooglehelp = false;
    regularexpressionfind = false;
    findrgx = NULL;
    
    prefixsize = 1;
    
    xcursor = 0;
    ycursor = 0;
    
    signal(SIGWINCH, resizewindow);
    colors.push_back(m_red);
    colors.push_back(m_dore);
    colors.push_back(m_blue);
    colors.push_back(m_gray);
    colors.push_back(m_green);

    poscommand = 0;
    echochar = false;
    option = x_none;
    pos = 0;
    posinstring  = 0;
    currentline = 0;
    currentfindpos = 0;
    currentposinstring = -1;
    prefix = "작";
    wprefix = L"작";
    replaceall = false;
    modified = true;
    inittableutf8();
    screensizes();
    localhelp << m_red<< "^xh" << m_current << ":help " << m_red<< "^k" << m_current << ":del after " << m_red<< "^p" << m_current << ":k-buffer " <<  m_red<< "^d" << m_current << ":del line " << m_red<< "^u/^r" << m_current << ":un/redo " << m_red<< "^f" << m_current << ":find " << m_red<< "^n" << m_current << ":next " << m_red<< "^g" << m_current << ":go " << m_red<< "^l" << m_current << ":top/bottom " << m_red<< "^t" << m_current << ":indent " << m_red<< "^w" << m_current << ":write " << m_red<< "^x" << m_current << ":commands ";
    
    updateline = true;
    
    JAGEDITOR = this;
}

jag_editor::~jag_editor() {
    if (findrgx != NULL)
        delete findrgx;
}

///------------------------------------------------------------------------------------

void jag_editor::setscrolling() {
    getcursor();
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &wns);
    char buffer[20];
    sprintf(buffer, "\33[%d,%dr", xcursor, wns.ws_row);
    cout << buffer;
}

void jag_editor::resetscrolling() {
    char buffer[10];
    strcpy(buffer, "\33[r");
    cout << buffer;
}

long jag_editor::taille(wstring& s) {
    long sz = s.size();
    long pref = prefixego() + 1;
    long pos = pref;
    for (long i = 0; i < sz; i++) {
        if (c_is_emoji(s[i])) {
            pos += 2;
            continue;
        }
        if (ckjchar(s[i])) {
            pos += 2;
            continue;
        }
        if (s[i] == 9) //tab position
            pos += (8 - (pos%8))%8;
        pos++;
    }
    return (pos-pref);
}

string jag_editor::coloringline(wstring& l, long p, bool select) {
    string line = convert(l);
    
    string sub;
    string substring;
    long ps;
    
    if (p != -1) {
        
        switch (longstrings[p]) {
            case l_str:
                sub = m_red;
                sub += line;
                sub += m_current;
                return sub;
            case l_com:
                sub = m_green;
                sub += line;
                sub += m_current;
                return sub;
            case l_com_one:
                if (line.find("//") == -1) { //Ok, it is always green
                    sub = m_green;
                    sub += line;
                    sub += m_current;
                    return sub;
                }
                break;
            default:
                if (longstrings[p] < 0 && !select) {
                    ps = longstrings[p]*-1;
                    ps--;
                    substring = line.substr(0, ps);
                    sub = substring;
                    Trim(sub);
                    if (sub == "") {//it is alone on a line
                        sub = m_red;
                        sub += line;
                        sub += m_current;
                        return sub;
                    }
                    
                    substring = m_red;
                    substring += line.substr(ps, line.size());
                    substring += m_current;
                    line = line.substr(0, ps);
                }
        }
    }
    
    vector<long> tobecolored;
    colorring(STR(line), tobecolored);
    
    long i = 0;
    
    long left, right;
    bool add;
    for (i = tobecolored.size()-3; i>= 0; i-=3) {
        add = false;
        
        left = tobecolored[i+1];
        right = tobecolored[i+2];
        
        sub = line.substr(0, left);
        
        switch (tobecolored[i]) {
            case 1: //strings
            case 2:
            case 3:
                sub += colors[0];
                add = true;
                break;
            case 4: //methods
                sub += colors[1];
                add = true;
                break;
            case 5://keywords
                sub += colors[2];
                add = true;
                break;
            case 6: //function call
                sub += colors[3];
                add = true;
                break;
            case 7: //comments
                sub += colors[4];
                add = true;
                break;
            case 8: //labels
                sub += colors[3];
                add = true;
                break;
        }
        
        if (add) {
            sub += line.substr(left, right-left);
            sub += m_current;
            sub += line.substr(right, line.size() - right);
            line = sub;
        }
    }
    line += substring;
    return line;
}

void jag_editor::backtoline() {
    long i = row_size + 1;
    while (i > currentline) {
        i--;
        cout << m_up;
    }
    movetoend();
}

void jag_editor::selectfound(long l, long r) {
    wstring ln = lines[pos];
    
    wstring lsub = ln.substr(0,l);
    lsub += L"<!@!<";
    
    if (r > ln.size())
        r = ln.size();
    else
        lsub += ln.substr(r, ln.size());
    
    string line = coloringline(lsub, pos, true);
    
    lsub = ln.substr(l, r-l);
    string inter = m_selectgray;
    inter += convert(lsub);
    inter += m_current;
    
    l = line.find("<!@!<");
    if (l==-1) {
        movetoposition();
        return;
    }
    line.replace(l, 5, inter);
    
    if (lines.status[pos] == concat_line) {
        string space(prefixe(), ' ');
        cout << back << space << line;
    }
    else
        printline(lines.numeros[pos], line);
    
    movetoposition();
}

void jag_editor::getcursor() {
    xcursor = getxcursor();
    ycursor = getycursor();
}

void jag_editor::movetoposition() {
    long sc;
    if (emode())
        sc = size_upto(lines[poslines[currentline]], posinstring) + prefixe();
    else
        sc = size_upto(line, posinstring) + prefixego();
    cout << back;
    while (sc != 0) {
        cout << m_right;
        sc--;
    }
}

void jag_editor::movetobeginning() {
    long sc = prefixego();
    cout << back;
    while (sc) {
        cout << m_right;
        sc--;
    }
}

void jag_editor::movetoend() {
    long sc;
    if (emode())
        sc = fullsize(lines[poslines[currentline]]) + prefixe();
    else
        sc = fullsize(line) + prefixego();
    
    cout << back;
    while (sc != 0) {
        cout << m_right;
        sc--;
    }
}

void jag_editor::movetolastline() {
    long e = row_size + 2;
    cout << m_home;
    while (e) {
        cout << m_down;
        e--;
    }
    cout << back;
}

void jag_editor::movetoline(long e) {
    if (e >= poslines.size()) {
        currentline = poslines.size()-1;
        e =  currentline;
    }

    cout << m_home;
    while (e) {
        cout << m_down;
        e--;
    }
}

void jag_editor::gotoline(long p) {
    bool fnd = false;
    currentline = 0;
    if (p >= poslines[0] && p <= poslines.back()) {
        for (long u = 0; u < poslines.size(); u++) {
            if (p == poslines[u]) {
                currentline = u;
                fnd = true;
            }
        }
    }
    
    if (!fnd)
        displaylist(p);
    else
        displaylist(poslines[0]);
    
    movetoline(currentline);
}

bool jag_editor::updown(char drt, long& pos) {
    long sz = lines.size();
    bool moveup = false;
    moveup = true;
    
    char exec = 0;
    if (drt == 65) { // we are going up
        currentline--;
        if (currentline < 0) {
                //we need to scroll down...
            currentline = 0;
            if (pos > 0) {
                //the current position in lines should not be the top line...
                //we check one position up...
                --pos;
                resetlist(poslines[0]-1);
                cout << m_scrolldown;
                line = lines[pos];
                displaygo(true);
                if (posinstring > linesize())
                    posinstring = linesize();
                movetoposition();
                return true;
            }
            return false;
        }
        else {
            pos = poslines[currentline];
            exec = 1;
        }
    }
    else {
        if ((currentline+1) >= poslines.size()) {
            if (pos < (sz -1)) {
                resetlist(poslines[0]+1);
                pos = poslines.back();
                cout << m_scrollup;
                line = lines[pos];
                displaygo(true);
                if (posinstring > linesize())
                    posinstring = linesize();
                movetoposition();
                return true;
            }
            return false;
        }
        pos = poslines[++currentline];
        exec = 2;
    }
    
    if (exec) {
        if (moveup) {
            if (drt == 65)
                cout << m_up;
            else
                cout << m_down;
        }
        line = lines[pos];
        if (posinstring > line.size())
            posinstring = line.size();
        movetoposition();
        return true;
    }
    
    return false;
}

void jag_editor::clearscreen() {
    cout << m_clear << m_home;
}

void jag_editor::clearline() {
    string space(col_size + spacemargin, ' ');
    cout << back << space;
}


//We build a string with no composed emoji, to match the position of the cursor...
void jag_editor::cleanlongemoji(wstring& s, wstring& cleaned, long p) {
    long i = 0;
    if (!check_large(WSTR(line), line.size(), i)) {
        cleaned = s.substr(0, p);
        return;
    }

    if (p < i) {
        cleaned = s.substr(0, p);
        return;
    }


    cleaned = s.substr(0,i);
    bool emoji = false;
    for (; i < p; i++) {
        if (emoji && c_is_emojicomp(s[i])) {
            continue;
        }
        if (c_is_emoji(s[i])) {
            emoji = true;
            cleaned += s[i];
            continue;
        }
        emoji = false;
        cleaned += s[i];
    }
}

    //The deletion of a character is different if it is an emoji...
long jag_editor::deleteachar(wstring& l, bool last, long pins) {
    if (l == L"")
        return pins;
    long emoji = 0;
    
    long sz = size_c(l, emoji);
    if (sz == l.size() || pins < emoji) {
        if (last)
            l.pop_back();
        else
            l.erase(pins, 1);
        return pins;
    }

    //i is the first position of a potential emoji...
    //emojis are after the current character,
    long nb = 1;
    if (last) {
        emoji = line.size() - 1;
        if (c_is_emojicomp(l[emoji])) {
            emoji--;
            while (emoji > 0 && c_is_emojicomp(l[emoji])) {
                nb++;
                emoji--;
            }
            if (emoji >= 0 && !c_is_emoji(l[emoji]))
                nb = 1;
        }
        line.erase(pins, nb);
        return pins;
    }
    
    emoji = pins;
    
    //Now we need to compute the exact position in characters
    if (c_is_emoji(l[emoji])) {
        emoji++;
        while (c_is_emojicomp(l[emoji])) {
            nb++;
            emoji++;
        }
    }
    l.erase(pins, nb);
    return pins;
}

void jag_editor::deletechar() {
    long sz = line.size();
    bool last = false;
    if (posinstring >= sz - 1) {
            //We need to known if we are in the middle of a large string across more than one line...
        if (!emode())
            last = true;
        else {
            if (currentline >= poslines.size() - 1 || poslines[currentline + 1] != pos)
                last = true;
        }
    }
    
    if (!emode() || (posinstring >= 0 && posinstring < sz)) {

        if (emode()) {
            line = lines[pos];
            undo(line, pos, u_modif);
        }
        else
            clearline();
        
        posinstring = deleteachar(line, last, posinstring);
        
        
        if (option != x_none) {
            displaygo(false);
            return;
        }
        
        //We update our line
        if (emode()) {
            clearline();
            lines[pos] = line;
            
            if (lines.Status(pos))
                lines.refactoring(pos);
            
            displaylist(poslines[0]);
            movetoline(currentline);
        }
        else {
            printline(pos+1, line);
            if (updateline && pos < lines.size())
                lines[pos] = line;
        }

        movetoposition();
    }
}

void jag_editor::deleteline(char moveup) {
    
    if (lines.size() == 1 && lines[0].size() == 0) {
        init();
        return;
    }
    
    
    if (!moveup) {
        //we delete the full line
        undo(lines[pos],pos, u_del); //deletion
        char update = lines.updatestatus(pos);

        kbuffer = lines[pos] + L"\n"; //we keep track of that line...
        if (pos >= lines.size())
            lines.pop_back();
        else {
            lines.erase(pos);
            lines.numbers();
        }
        
        if (update != -1) {
            lines.status[pos] = concat_line;
            undo(lines[pos],pos, u_modif_linked); //status modification
            lines.status[pos] = update;
        }

        displaylist(poslines[0]);
        
        movetoline(currentline);
        posinstring = 0;
        movetobeginning();
        return;
    }
    
        //left top, cannot do anything
    
        //We need to kill the current line, however it can be merged with the previous one if moveup is true...
        //We are at position currentline...
        //if the line above already belong to the same line, we kill the last character of that libe above
    wstring code;
    if (moveup == -1) { // the destruction was done with backspace
        if (!pos && !posinstring && !currentline)
            return;
        
        char stat = lines.status[pos];
        
        code =  lines[pos]; //our current line...
        
            //We need to know if we are deleting a line or a subset of a line
        if (stat != concat_line) {
            if (stat == beg_line) {
                undo(lines[pos], pos, u_modif); //modification
                undo(lines[pos+1],pos+1, u_modif_linked); //we are modifying a multiple line
            }
            else
                undo(lines[pos],pos, u_del); //deletion
            
            lines.erase(pos); //here the line is deleted from our list of lines
            if (stat == beg_line)// it is a long line...
                lines.status[pos] = concat_line;
            
            if (currentline > 0)
                pos = poslines[--currentline];
            else
                resetlist(--pos);
            
            undo(lines[pos], pos, u_modif_linked); //modification
            if (stat == beg_line) {
                if (lines.Status(pos) == solo_line)
                    lines.status[pos] = beg_line;
            }
            
            posinstring = lines[pos].size();
        }
        else {
            undo(lines[pos], pos, u_modif); //modification
            undo(lines[pos+1],pos+1, u_modif_linked); //we are modifying a multiple line
            
            lines.erase(pos); //here the line is deleted from our list of lines
            if (currentline > 0)
                pos = poslines[--currentline];
            else
                resetlist(--pos);
            
            undo(lines[pos],pos, u_modif_linked); //we are modifying a multiple line
            
                //we delete the last character on the previous line since this is one single string across different lines.
            posinstring = lines[pos].size() - 1;
            deleteachar(lines[pos], true, posinstring);
        }
        
        
        lines[pos] += code;
        line = lines[pos];
        //we merge the current line and the above line...
        lines.refactoring(pos);
    }
    else { //the destruction was done with the delete key, we stay on the same line
        line = lines[pos];
        undo(line, pos, u_modif); //modification...
                                  //Three cases, the next line is actually part of the current line
        long p = pos+1;
        if (lines.Status(p) == concat_line) {
            undo(lines[p], p, u_modif_linked);
            code = lines[p];
            deleteachar(code, false, 0);
            lines[p] = code;
            lines.refactoring(p);
        }
        else {
            if (pos < lines.size() - 1) {
                code = lines[p];
                undo(code, p, u_del_linked);
                line += code;
                lines[pos] = line;
                lines.erase(p);
                lines.refactoring(pos);
            }
            else
                return;
        }
    }
    
    displaylist(poslines[0]);
    movetoline(currentline);
    movetoposition();
}

void jag_editor::displayonlast(bool bck) {
    movetolastline();
    wstring w = st.str();
    string s = convert(w);
    clearline();
    cout << back << s;
    
    if (bck) {
        movetoline(currentline);
        if (currentposinstring != -1)
            posinstring = currentposinstring;
        currentposinstring = -1;
        if (posinstring > lines[poslines[currentline]].size()) {
            posinstring = lines[poslines[currentline]].size();
            movetoend();
        }
        else
            movetoposition();
    }
}

void jag_editor::displayonlast(wstring w, bool bck) {
    movetolastline();
    string s = convert(w);
    clearline();
    cout << back << s;
    
    if (bck) {
        movetoline(currentline);
        if (currentposinstring != -1)
            posinstring = currentposinstring;
        currentposinstring = -1;
        if (posinstring > lines[poslines[currentline]].size()) {
            posinstring = lines[poslines[currentline]].size();
            movetoend();
        }
        else
            movetoposition();
    }
}

void jag_editor::displayonlast(string s, bool bck) {
    movetolastline();
    clearline();
    cout << back << s;
    
    if (bck) {
        movetoline(currentline);
        if (currentposinstring != -1)
            posinstring = currentposinstring;
        currentposinstring = -1;
        if (posinstring > lines[poslines[currentline]].size()) {
            posinstring = lines[poslines[currentline]].size();
            movetoend();
        }
        else
            movetoposition();
    }
}

void jag_editor::displaygo(bool full) {
    wstring val;
    switch(option) {
        case x_goto:
            cout << back << "Line:" << convert(line);
            break;
        case x_find:
            if (regularexpressionfind)
                cout << back << "Find(rgx):" << convert(line);
            else
                cout << back << "Find:" << convert(line);
            break;
        case x_replace:
            if (regularexpressionfind)
                cout << back << "Find(rgx):" << convert(currentfind) << "  Replace:" << convert(line);
            else
                cout << back << "Find:" << convert(currentfind) << "  Replace:" << convert(line);
            break;
        case x_write:
            cout << back << "File:" << coloringline(line);
            break;
        case x_count:
            cout << back << "Count:" << convert(line);
            break;
        case x_delete:
        case x_copy:
        case x_cut:
        case x_copying:
        case x_deleting:
        case x_cutting:
        case x_load:
            val = st.str();
            cout << back << convert(val) << convert(line);
            break;
        default:
            if (full) {
                if (lines.status[pos] == concat_line) {
                    string space(prefixe(), ' ');
                    cout << back << space << coloringline(line, pos);
                }
                else
                    printline(lines.numeros[pos], line, pos);
            }
    }
}

void jag_editor::scanforlonglines() {
    longstrings.clear();
        //We check for comments and long lines
    long r;
    for (long i = 0; i < lines.size(); i++) {
        if (lines[i].find(L"//") != -1) {
            longstrings.push_back(l_com_one);
            continue;
        }
        
        if (ispy()) {
            r = lines[i].find(L"\"\"\"");
            if (r != -1) {
                r++;
                if (lines[i].find(L"\"\"\"", r) != -1) {
                    longstrings.push_back(l_str);
                    continue;
                }
                longstrings.push_back(r*-1);
                i++;
                while (i < lines.size() && lines[i].find(L"\"\"\"") == -1) {
                    longstrings.push_back(l_str);
                    i++;
                }
                longstrings.push_back(l_str);
                continue;
            }
            
            r = lines[i].find(L"'''");
            if (r != -1) {
                r++;
                if (lines[i].find(L"'''", r) != -1) {
                    longstrings.push_back(l_str);
                    continue;
                }
                longstrings.push_back(r*-1);
                i++;
                while (i < lines.size() && lines[i].find(L"'''") == -1) {
                    longstrings.push_back(l_str);
                    i++;
                }
                longstrings.push_back(l_str);
                continue;
            }
        }
        
        if (isc()) {
            r = lines[i].find(L"/*");
            if (r != -1) {
                longstrings.push_back(l_com);
                if (lines[i].find(L"*/", r) != -1)
                    continue;
                i++;
                while (i < lines.size() && lines[i].find(L"*/") == -1) {
                    longstrings.push_back(l_com);
                    i++;
                }
                longstrings.push_back(l_com);
                continue;
            }
        }
        else {
            if (lines[i][0] == '#') {
                longstrings.push_back(l_com);
                continue;
            }
        }
        
        if (istamgu()) {
            r = lines[i].find(L"/@");
            if (r != -1) {
                longstrings.push_back(l_com);
                if (lines[i].find(L"@/", r) != -1)
                    continue;
                i++;
                while (i < lines.size() && lines[i].find(L"@/") == -1) {
                    longstrings.push_back(l_com);
                    i++;
                }
                longstrings.push_back(l_com);
                continue;
            }
            
            r = lines[i].find(L"@\"");
            if (r != -1) {
                r++;
                if (lines[i].find(L"\"@", r) != -1) {
                    longstrings.push_back(l_str);
                    continue;
                }
                
                longstrings.push_back(r*-1);
                i++;
                while (i < lines.size() && lines[i].find(L"\"@") == -1) {
                    longstrings.push_back(l_str);
                    i++;
                }
                longstrings.push_back(l_str);
                continue;
            }
        }
        longstrings.push_back(0);
    }
}

void jag_editor::displaylist(long beg) {
    if (beg < 0)
        beg = 0;
    
    long nb = 0;
    x_option g = option;
    option = x_none;
    
    poslines.clear();
    
    if (modified) {
        scanforlonglines();
        modified = false;
    }
    
    lines.updatesize();
    
    stringstream blk;
    for (long i = beg; i < lines.size(); i++) {
        poslines.push_back(i);
        string space(prefixe(), ' ');
        if (lines.status[i] == concat_line)
            blk << space << coloringline(lines[i], i) << endl;
        else
            blk << m_dore << prefix << m_current << m_lightgray << std::setw(prefixsize) << lines.numeros[i] << "> " << m_current << coloringline(lines[i], i) << endl;
        nb++;
        if (nb > row_size) // we have displayed all lines
            break;
    }
    option = g;
    clearscreen();
    cout << blk.str();
}

void jag_editor::processgo() {
    long i;
    if (line == L"$") {
        pos = lines.size() - 1;
        option = x_none;
        displaylist(pos - row_size);
        line = lines[pos];
        posinstring = line.size();
        currentline = poslines.size() - 1;
        movetoline(currentline);
        movetoend();
        return;
    }
    
    i = convertinteger(line);
    if (i >= 0 && i < lines.size()) {
        pos = lines.getlinenumber(i);
        option = x_none;
        displaylist(pos);
        currentline = 0;
        line = lines[poslines[currentline]];
        posinstring = 0;
        cout << m_home;
        movetobeginning();
        return;
    }
    
    clearline();
    cout << back << "Unknown line";
    movetoline(currentline);
    movetoend();
}

void jag_editor::resetsearch() {
    if (regularexpressionfind) {
        if (findrgx == NULL)
            findrgx = new Jag_automaton(currentfind);
        else
            if (findrgx->regularexpression != currentfind) {
                delete findrgx;
                findrgx = new Jag_automaton(currentfind);
            }
    }
}

bool jag_editor::search(wstring& l, long& first, long& last, long ps) {
    if (regularexpressionfind)
        return findrgx->search(l, first, last, ps);
    first = l.find(currentfind, ps);
    if (first == -1)
        return false;
    last = first + currentfind.size();
    return true;
}

bool jag_editor::processfind() {
    long i;
        //Search part...
    long first, last, end;
    long ps = posinstring;
    currentfindpos = 0;
    if (currentfind !=  L"") {
        wstring l;
        resetsearch();
        
        for (i = pos; i < lines.size(); i++) {
            l = lines.getoneline(i, end);
            if (search(l, first, last, ps)) {
                pos = i;
                if (first > col_size) {
                    ps = first/col_size;
                    pos += ps;
                    ps *= col_size;
                    first -= ps;
                    last -= ps;
                    resetlist(pos);
                }
                
                posinstring = first;
                currentfindpos = last;
                gotoline(pos);
                x_option g = option;
                option = x_none;
                selectfound(first, last);
                option = g;
                return true;
            }
            ps = 0;
            i = end;
        }
    }
    
    clearline();
    cout << back << "Not found";
    movetoline(currentline);
    movetoend();
    currentfind = L"";
    return false;
}

long jag_editor::processcount() {
    if (currentfind == L"")
        return 0;
    
    wstring code = lines.code();
    
    vector<long> alls;
    resetsearch();
    findrgx->searchall(code, alls, 0);
    return (alls.size() >> 1);
}

void jag_editor::processreplace() {
    string resp;
    option = x_none;
    movetoposition();
    if (!replaceall) {
        displayonlast(L"Replace: Y/N/A", true);
        resp = _getch();
        if (resp == "A" || resp == "a")
            replaceall = true;
    }
    
    if (replaceall || resp == "Y" || resp == "y") {
        //We know where the word is...
        resetsearch();
        long first, last, end;
        wstring ws = lines.getoneline(pos, end);
        if (search(ws, first, last, posinstring)) {
            ws.replace(first, last-first, currentreplace);
            lines.replaceline(pos, end+1, ws);
            
            if (first > col_size) {
                long ps = first/col_size;
                pos += ps;
                ps *= col_size;
                first -= ps;
                last -= ps;
                resetlist(pos);
                gotoline(pos);
            }
            else
                gotoline(poslines[0]);
            
            if (!findnext()) {
                replaceall = false;
                posinstring = first;
            }
        }
        return;
    }
    
    if (resp == "N" || resp == "n") {
        if (!findnext())
            replaceall = false;
        
    }
    else
        displayonlast("", true);
}

bool jag_editor::findnext() {
    if (currentfind != L"") {
        resetsearch();
        long i, ps = currentfindpos;
        long first, last, end;
        wstring l;
        for (i = pos; i < lines.size(); i++) {
            l = lines.getoneline(i, end);
            if (search(l, first, last, ps)) {
                pos = i;
                if (first > col_size) {
                    ps = first/col_size;
                    pos += ps;
                    ps *= col_size;
                    first -= ps;
                    last -= ps;
                    posinstring = first;
                    resetlist(pos);
                }
                
                posinstring = first;
                currentfindpos = last;
                gotoline(pos);
                selectfound(first, last);
                if (currentreplace != L"")
                    processreplace();
                return true;
            }
            i = end;
            ps = 0;
        }
        
        if (!replaceall) {
            for (i = 0; i < lines.size(); i++) {
                l = lines.getoneline(i, end);
                if (search(l, first, last, 0)) {
                    pos = i;
                    if (first > col_size) {
                        ps = first/col_size;
                        pos += ps;
                        ps *= col_size;
                        first -= ps;
                        last -= ps;
                        posinstring %= col_size;
                        resetlist(pos);
                    }
                    
                    posinstring = first;
                    currentfindpos = last;
                    gotoline(pos);
                    selectfound(first, last);
                    if (currentreplace != L"")
                        processreplace();
                    return true;
                }
            }
        }
    }
    return false;
}


    //A CR was hit, we need either to modify the current line or to add a new one...
    //If the cursor was in the middle of a line, then we split it in two

long jag_editor::handlemultiline() {
        //We are in the middle of a very long line across multiple lines on screen
        //We need to cut it in half...
    
    line = lines[poslines[currentline]]; //our current line...
                                         //we need to cut it in half
    wstring sub;
    
    char stat = lines.Status(pos);

    //We keep track of our line before splitting...
    undo(lines[pos],pos, u_modif); //modification

    //We are in the middle of our substring... We extract some sub-elements...
    
    //We cut our line at the right position.
    //sub is the right part of the string
    sub = line.substr(posinstring, line.size());
    
    //line keeps the left part...
    line = line.substr(0, posinstring);

    lines[pos] = line;
    currentline++;
    pos++;
    lines.insert(pos, sub);
    undo(sub, pos, u_ins_linked); //insertion on following line

    if (stat == solo_line)
        //In this case, the solution is easy...
        lines.numbers();
    else {
        if (stat == beg_line)
            lines.status[pos-1] = solo_line;
        if (lines.Status(pos+1) == concat_line) {
            lines.status[pos] = beg_line;
            lines.refactoring(pos);
        }
        else
            lines.status[pos] = solo_line;
    }
    
    if (currentline > row_size) {
        displaylist(poslines[0] + 1);
        currentline = row_size;
    }
    else
        displaylist(poslines[0]);
    
    movetoline(currentline);
    
    posinstring = 0;
    movetobeginning();
    line = sub;
    return pos;
}

long jag_editor::handlingeditorline(bool computespace) {
    long i;
    

    long sz = lines.size();
    long szl = lines[pos].size();
    
    //if we are in the middle of a line...
    if ((posinstring > 0 && posinstring < line.size()) || lines.status[pos] == concat_line)
        return handlemultiline();

    //We keep track of our line before splitting...
    undo(lines[pos],pos, u_modif); //modification

    long sp = 0;
    char spa = ' ';
    //we record the number of space on the current line...
    //which we will use as a seed to display the following line
    if (ispy()) {
        if (sz && computespace && szl) {
            long inc = 4;
            if (line[0] == '\t') {
                spa = '\t';
                inc = 1;
            }
            
            for (i = 0; i < szl; i++) {
                if (line[i] != spa)
                    break;
                sp++;
            }
            if (sp && line.find(L"else") != -1) {
                wstring l = line;
                l = Trim(l);
                if (l == L"else:") {
                    if (sp > inc) {
                        wstring space(sp-inc, spa);
                        line = space;
                        line += l;
                    }
                    else
                        line = l;
                    lines[pos] = line;
                    printline(pos, line);
                }
            }
            else {
                if (line.back() == ':')
                    sp+=inc;
            }
        }
    }
    else {
        //We need to find the first line not starting with a blank...
        sp = lines.indent(pos);
    }
    
    wstring space(sp, spa);
    
        //we have different cases...
    if (!posinstring) {
            //beginning of the line, we insert before...
        lines.insert(pos, space); //we insert it into our line...
        undo(space, pos, u_ins_linked); //The value is inserted
        lines.numbers();
        if (currentline < row_size) {
            displaylist(poslines[0]);
            currentline++;
        }
        else //in this case, we are at the end of the process...
            displaylist(poslines[0] + 1);
        
        pos = poslines[currentline];
        line = lines[pos];
        posinstring = 0;
        movetoline(currentline);
        movetobeginning();
        return pos;
    }
    
    char action = u_ins;
    if (pos >= sz - 1) //we already are at the end...
        lines.push_back(space);
    else {
        lines.insert(pos+1, space); //we insert it into our line...
        lines.numbers();
    }
    
    pos++;
    undo(space, pos, action); //The value is negative to indicate a deletion
    
    if (currentline < row_size) {
        displaylist(poslines[0]);
        currentline++;
    }
    else //in this case, we are at the end of the process...
        displaylist(poslines[0] + 1);
    
    movetoline(currentline);
    if (action == u_ins) {
        posinstring = space.size();
        movetoend();
    }
    else {
        posinstring = 0;
        movetobeginning();
    }
    line = space;
    return pos;
}

string jag_editor::getch() {
    return _getch();
}

long jag_editor::getbuffsize() {
    return _getbuffsize;
}

void jag_editor::clear() {
    echochar = false;
    pos = lines.size();
    currentline = 0;
    movetolastline();
    printline(pos+1);
    fflush(stdout);
    line = L"";
    posinstring = 0;
    currentline = 0;
}

void jag_editor::processredos() {
    if (!emode() || redos.empty())
        return;
    
    line = redos.l_keeplines.back();
    pos = redos.l_keeppos.back();
    char a = redos.l_keepactions.back();
    currentline = redos.l_keepcurrentline.back();
    posinstring = redos.l_keepposinstring.back();
    char status = redos.l_keepstatus.back();
    
    long posfirst = redos.l_keeptop.back();
    
    modified = true;
    bool refact = false;

    wstring l;
    
    switch (a) {
        case u_del_linked:
        case u_del:
            l = lines[pos];
            lines.erase(pos);
            refact = true;
            break;
        case u_modif: //modification
        case u_modif_linked:
            l = lines[pos];
            lines[pos] = line;
            lines.status[pos] = status;
            break;
        case u_ins:
        case u_ins_linked:
            lines.insert(pos, line);
            lines.status[pos] = status;
            refact = true;
            break;
    }
    
    undos.move(l, redos);
    
    if (!redos.empty()) {
            //the linked actions are in other way around...
        a = redos.l_keepactions.back();
        if (a == u_del_linked || a == u_modif_linked || a == u_ins_linked) {
            processredos();
            return;
        }
    }
    
    if (refact)
        lines.refactoring(pos);
    
    displaylist(posfirst);
    
    movetoline(currentline);
    movetoposition();
}

void jag_editor::processundos() {
    if (!emode() || undos.empty())
        return;
    
    line = undos.l_keeplines.back();
    pos = undos.l_keeppos.back();
    char a = undos.l_keepactions.back();
    currentline = undos.l_keepcurrentline.back();
    posinstring = undos.l_keepposinstring.back();
    char status = undos.l_keepstatus.back();
    
    long posfirst = undos.l_keeptop.back();
    
    modified = true;
    bool refact = false;
    wstring l;

    switch (a) {
        case u_del_linked: //delete a line
        case u_del:
                //the line was removed from lines
            lines.insert(pos, line);
            lines.status[pos] = status;
            refact = true;
            break;
        case u_modif: //modification
        case u_modif_linked:
            l = lines[pos];
            lines[pos] = line;
            lines.status[pos] = status;
            break;
        case u_ins: //the line was inserted, we remove it...
        case u_ins_linked:
            l = lines[pos];
            lines.erase(pos);
            refact = true;
            break;
    }
    
    redos.move(l, undos);
    
        //if we have linked actions (two undos done for one action), we need to apply them in sequence...
    if (a == u_del_linked || a == u_modif_linked || a == u_ins_linked) {
        processundos();
        return;
    }
    
    if (refact)
        lines.refactoring(pos);

    displaylist(posfirst);
    
    movetoline(currentline);
    movetoposition();
}

    //The main problem here is that emojis can be composed...
    //An emoji can be composed of up to 7 emojis...
void jag_editor::forwardemoji() {
    if (c_is_emoji(line[posinstring])) {
        posinstring++;
        while (c_is_emojicomp(line[posinstring]))
            posinstring++;
    }
    else
        posinstring++;
}

void jag_editor::backwardemoji() {
    posinstring--;
    long i = 0;
    if (!check_large(WSTR(line), line.size(), i))
        return;

    if (posinstring < i)
        return;
    
    i = posinstring;
    if (c_is_emojicomp(line[i])) {
        i--;
        while (i > 0 && c_is_emojicomp(line[i]))
            i--;
        if (i >= 0 && c_is_emoji(line[i]))
            posinstring = i;
    }
}

void jag_editor::evaluateescape(string& buff) {
    
    if (buff == homekey) {
        movetobeginning();
        posinstring = 0;
        return;
    }
    
    if (buff == endkey) {
        posinstring = line.size();
        movetoend();
        return;
    }
    
        //ctrl-up, up 10 lines
    if (buff == c_up) {
        if ((pos - row_size) < 0) {
            pos = 0;
            currentline = 0;
        }
        else
            pos -= row_size;
        displaylist(pos);
        posinstring = 0;
        movetoline(currentline);
        movetobeginning();
        return;
    }
    
        //ctrl-down, down 10 lines
    if (buff == c_down) {
        long mxline = poslines.size() - 1;
        
        posinstring = 0;
        if ((pos + mxline) == lines.size()) {
            currentline = mxline;
            movetoline(currentline);
            movetobeginning();
            return;
        }
        
        if ((pos + mxline) > lines.size()) {
            pos = lines.size()-mxline;
            if (pos < 0)
                pos = lines.size()-1;
        }
        else
            pos += mxline;
        displaylist(pos);
        movetoline(currentline);
        movetobeginning();
        return;
    }
    
    if (buff == up || buff == down) {
        updown(buff[2], pos);
        return;
    }
    
    if (buff == c_right || buff == a_right) {
        long ipos = posinstring;
        if (ipos < line.size() - 1 && c_is_separator(line[ipos+1]))
            ipos += 2;
        bool fnd = false;
        while (ipos < line.size()) {
            if (c_is_separator(line[ipos])) {
                fnd = true;
                break;
            }
            ipos++;
        }
        
        if (!fnd) {
            posinstring = line.size();
            movetoend();
        }
        else {
            posinstring = ipos + 1;
            movetoposition();
        }
        
        return;
    }
    
    if (buff == c_left || buff == a_left) {
        long ipos = posinstring;
        bool fnd = false;
        if (ipos > 1 && c_is_separator(line[ipos-1]))
            ipos -= 2;
        while (ipos > 0) {
            if (c_is_separator(line[ipos])) {
                fnd = true;
                break;
            }
            ipos--;
        }
        
        if (!fnd)
            posinstring = 0;
        else
            posinstring = ipos + 1;
        
        movetoposition();
        return;
    }
    
    if (buff == right) {
        if (posinstring < line.size()) {
            forwardemoji();
            movetoposition();
        }
        else {//we go down at the beginning of the next line
            updown(66, pos);
            posinstring = 0;
            movetobeginning();
        }
        
        return;
    }
    
    if (buff == left) {
        if (posinstring > 0) {
            backwardemoji();
            movetoposition();
        }
        else {//we go up at the end of the previous line
            updown(65, pos);
            posinstring = line.size();
            movetoend();
        }
        return;
    }
    
    if (buff == del) {
        if (emode() && pos < lines.size()) {
            if (posinstring >= lines[pos].size()) {
                deleteline(1);
                return;
            }
        }
        
        deletechar();
    }
}

void jag_editor::init() {
    lines.clear();
    lines.push(L"");
    poslines.clear();
    poslines.push_back(0);
    
    displaylist(0);
    pos = 0;
    posinstring = 0;
    kbuffer = L"";
    currentline = 0;
    clearscreen();
    printline(1);
}

void jag_editor::convertmetas() {
    wstring sub;
    wstring l;
    for (long i = 0; i < lines.size(); i++) {
        l = lines[i];
        convertmeta(sub, l);
        lines[i] = sub;
    }
    displaylist(poslines[0]);
    movetoline(currentline);
    posinstring = 0;
    movetobeginning();
}


bool jag_editor::checkcommand(char cmd) {
    switch (cmd) {
        case 'C': // we count pattern
            displayonlast("Count:", false);
            line = L"";
            currentfind = L"";
            currentreplace = L"";
            posinstring = 0;
            option = x_count;
            return true;
        case 'l':
            clearst();
            st << "load:";
            displayonlast(false);
            line = L"";
            currentfind = L"";
            currentreplace = L"";
            posinstring = 0;
            option = x_load;
            return true;
        case 'H': //we convert HMTL entities to characters
            convertmetas();
            return false;
        case 'c': //copy
            clearst();
            st << "copy from:";
            displayonlast(false);
            line = L"";
            option = x_copy;
            return true;
        case 'f':
            if (emode()) {
                regularexpressionfind = true;
                displayonlast("Find(rgx):", false);
                line = L"";
                currentfind = L"";
                currentreplace = L"";
                option = x_find;
            }
            return true;
        case 'x': //cut
            clearst();
            st << "cut from:";
            displayonlast(false);
            line = L"";
            option = x_cut;
            return true;
        case 'v': //paste
            handleblock(copybuffer);
            return true;
        case 'D': //delete a bloc of lines...
            clearst();
            st << "delete from:";
            displayonlast(false);
            line = L"";
            option = x_delete;
            return true;
        case 'q':
            currentposinstring = -1;
            terminate();
            return false;
        case 'w': //write and quit
            if (thecurrentfilename == "")
                return true;
            if (writetofile()) {
                currentposinstring = -1;
                terminate();
            }
            else
                displayonlast("Error while writing file", true);
            return true;
        case 'h':
            displaythehelp();
            _getch();
            displaylist(poslines[0]);
            movetoline(currentline);
            movetoposition();
            return true;
    }
    return false;
}

//This section handles combined commands introduced with Ctrl-x
void jag_editor::handlecommands() {
    currentposinstring = posinstring;
    string buff = _getch();

    if (checkcommand(buff[0]))
        return;
    
    displayonlast("", true);
}

void jag_editor::terminate() {
    movetolastline();
    string space(colsize(), ' ');
    cout << back << space << back << m_redbold << "Salut!!!" << m_current << endl;
    
    fflush(stdout);
    
    tcsetattr(0, TCSADRAIN, &oldterm);
    
    exit(0);
}

void jag_editor::cleanheaders(wstring& w) {
    
    if (w.find(wprefix) != -1) {
            //we might try to copy stuff from the editor itself...
            //We will remove anything which is: "작%d+> " from our string
        Au_automate rgx("작%s*%d+> ");
        vector<long> values;
        rgx.searchall(w,values);
        for (long i=values.size()-2; i>=0; i-=2) {
            w.erase(values[i], values[i+1]-values[i]);
        }
    }
}

bool jag_editor::checkaction(string& buff, long& first, long& last) {
    wstring code;
    long i;
    switch (buff[0]) {
        case 1: //ctrl-a, moving to beginning of line
            movetobeginning();
            posinstring = 0;
            return true;
        case 4: //ctrl-d exiting
            if (emode())
                deleteline(0);
            return true;
        case 5://ctrl-e, moving to the end of the line...
            posinstring = line.size();
            movetoend();
            return true;
        case 6: // ctrl-f find
            if (emode()) {
                regularexpressionfind = false;
                displayonlast("Find:", false);
                line = L"";
                currentfind = L"";
                currentreplace = L"";
                option = x_find;
            }
            return true;
        case 7: //ctrl-g go in edit mode
            if (emode()) {
                displayonlast("Line:", false);
                line = L"";
                option = x_goto;
            }
            return true;
        case 8: //ctrl-h display help
            if (emode()) {
                option = x_none;
                if (!tooglehelp)
                    displayonlast(localhelp.str(), true);
                else {
                    displaylist(poslines[0]);
                    movetoline(currentline);
                    movetoposition();
                }
                tooglehelp = 1 - tooglehelp;
            }
            return true;
        case 10: //this is a carriage return
                 //we need to check the different options...
            switch (option) {
                case x_none:
                    break;
                case x_goto: //go to
                    processgo();
                    option = x_none;
                    return true;
                case x_find: //find
                    currentfind = line;
                    processfind();
                    line = lines[poslines[currentline]];
                    option = x_none;
                    return true;
                case x_replace: //replace
                    if (processfind()) {
                        currentreplace = line;
                        replaceall = false;
                        processreplace();
                        line = lines[poslines[currentline]];
                    }
                    option = x_none;
                    return true;
                case x_write: //write to a file
                    code = line;
                    thecurrentfilename = convert(code);
                    if (writetofile())
                        code = L"written to: ";
                    else
                        code = L"cannot write to: ";
                    code += wconvert(thecurrentfilename);
                    option = x_none;
                    displayonlast(code, true);
                    line = lines[poslines[currentline]];
                    return true;
                case x_count: //count the number of occurrences of a pattern
                    currentfind = line;
                    clearst();
                    i = processcount();
                    if (i == 0)
                        st << "no occurence of " << m_redbold << currentfind << m_current << " found";
                    else
                        if (i == 1)
                            st << m_redbold << i << m_current << " occurence of " << m_redbold << currentfind << m_current << " found";
                        else
                            st << m_redbold << i << m_current << " occurences of " << m_redbold << currentfind << m_current << " found";
                    option = x_none;
                    displayonlast(true);
                    return true;
                case x_delete: //delete a bloc of line (6)
                    option = x_deleting;
                    first = convertinteger(line) - 1;
                    st << line << "  to:";
                    line = L"";
                    displayonlast(false);
                    return true;
                case x_copy: //copy (8)
                    first = convertinteger(line) - 1;
                    st << line << "  to:";
                    line = L"";
                    displayonlast(false);
                    option = x_copying;
                    return true;
                case x_cut: //cut (9)
                    first = convertinteger(line) - 1;
                    st << line << "  to:";
                    line = L"";
                    displayonlast(false);
                    option = x_cutting;
                    return true;
                case x_copying: //the pasting
                    copybuffer = L"";
                    if (line == L"$")
                        last = lines.size();
                    else
                        last = convertinteger(line);
                    line = L"";
                    if (first >= 0 && first < last && last < lines.size())
                        copybuffer = lines.code(first, last);
                    
                    st << line << " copied";
                    line = lines[poslines[currentline]];
                    option = x_none;
                    displayonlast(true);
                    return true;
                case x_deleting: //delete
                case x_cutting: //cut
                    if (line == L"$") {
                        if (first >= 0 && first < lines.size()) {
                            last = lines.size() - 1;
                            if (option == x_cutting) {
                                copybuffer = lines.code(first, last+1);
                                st << line << " cut";
                                displayonlast(true);
                            }
                            undo(lines[last], last, u_del);
                            for (i = last-1; i > first; i--)
                                undo(lines[i], i, u_del_linked);
                            undo(lines[first], first, u_del_linked);
                            lines.erase(first, -1);
                        }
                        else
                            break;
                    }
                    else {
                        last = convertinteger(line);
                        if (first >= 0 && first < last && last < lines.size()) {
                            if (option == x_cutting) {
                                copybuffer = lines.code(first, last);
                                st << line << " cut";
                                displayonlast(true);
                            }
                            undo(lines[last-1], last-1, u_del);
                            for (i = last-2; i > first; i--)
                                undo(lines[i], i, u_del_linked);
                            undo(lines[first], first, u_del_linked);
                            lines.erase(first, last);
                        }
                        else
                            break;
                    }
                    
                    line = L"";
                    posinstring = 0;
                    if (lines.size()) {
                        option = x_none;
                        displaylist(poslines[0]);
                        currentline = poslines.size()-1;
                        pos = poslines[currentline];
                        line = lines[poslines[currentline]];
                        displayonlast("", true);
                        return true;
                    }
                    break;
                case x_load:
                    loadfile(line);
                    break;
                default:
                    break;
            }
            
            if (!emode()) {
                option = x_none;
                displayonlast("", true);
                return true;
            }
            
            pos = handlingeditorline();
            return true;
        case 11: //ctrl-k: delete trailing characters
            if (emode()) {
                deleteallafter();
                return true;
            }
            
            clearline();
            kbuffer = line.substr(posinstring, line.size());
            line = line.substr(0, posinstring);
            cout << back << coloringline(line);
            return true;
        case 12: //ctrl-l: display one line down in the command history or toggle between top/bottom in edit mode
            if (emode())
                toggletopbottom();
            return true;
        case 14:
            if (emode()) { //in edit mode looking for the next find
                findnext();
                line = lines[poslines[currentline]];
            }
            return true;
        case 16: //ctrl-p insert kbuffer
            modified = true;
            if (kbuffer.find(10) == -1) {
                undo(lines[pos],pos, u_modif);
                addabuffer(kbuffer, false);
                return true;
            }
            line = kbuffer;
            line.pop_back();
            lines.insert(pos, line);
            undo(lines[pos],pos, u_ins);
            lines.numbers();
            posinstring = 0;
            displaylist(poslines[0]);
            movetoline(currentline);
            movetobeginning();
            return true;
        case 18: //ctrl-r: redo...
            if (option == x_find) { //replace mode called after a ctrl-f
                currentfind = line;
                line = L"";
                cout << "  Replace:";
                option = x_replace;
                return true;
            }
            processredos();
            return true;
        case 20: //ctrl-t
            if (emode())
                indentcode(pos);
            return true;
        case 21: //undo
            if (emode()) {
                processundos();
            }
            return true;
        case 23: //ctrl-w write the content in a file
            if (emode()) {
                line = wconvert(thecurrentfilename);
                code = L"File:";
                if (line.size())
                    code += line;
                
                displayonlast(code, false);
                option = x_write;
            }
            return true;
        case 24: //Insert commands
            displayonlast("Ctrl-X", true);
            handlecommands();
            if (option == x_exitprint)
                return false;
            return true;
        case 27: //Escape...
                 //we clear the current line if it is the only character...
            if (buff.size() == 1) {
                if (!emode() || tooglehelp) {
                    displayonlast("", true);
                    tooglehelp = false;
                    option = x_none;
                    return true;
                }
                return true;
            }
            
            evaluateescape(buff);
            return true;
        case 127: //back delete
            if (posinstring > 0) {
                backwardemoji();
                deletechar();
            }
            else {
                if (emode())
                    deleteline(-1);
            }
            return true;
    }
    
    return false;
}

void jag_editor::addabuffer(wstring& b, bool instring) {
    //We only keep displayable characters
    wchar_t c;
    for (long i = 0; i < b.size(); i++) {
        c = b[i];
        //We replace these characters with a blank
        if (c < 32 && c != 9 && c != 10 && c != 13)
            b[i] = 32;
    }
    
    modified = true;
    //We are in the middle of a line... Insertion mode...
    if (line.size() && posinstring < line.size()) {
        if (emode())
            line = lines[poslines[currentline]];
        
            //We insert the character within our current line...
        wstring code = line.substr(0, posinstring);
        code += b;
        code += line.substr(posinstring,line.size());
        
        if (emode()) {
            pos = poslines[currentline];
            lines[pos] = code;
            lines.refactoring(pos);

            line = lines[pos];
            if (fullsize(code) > col_size) {
                if (currentline == row_size)
                    displaylist(poslines[0] + 1);
                else
                    displaylist(poslines[0]);
                
                if (posinstring >= col_size) {
                    currentline++;
                    posinstring = 0;
                    pos++;
                }
                
                posinstring += b.size();
                movetoline(currentline);
                movetoposition();
                return;
            }
        }
        
        clearline();
        line = code;
        displaygo(true);
        posinstring += b.size();
        movetoposition();
        
        return;
    }
    
    
        //We extend our line...
    line += b;
    
    if (b[0]=='"' || b[0]=='\'')
        instring = 1 - instring;
    
    bool fndchr =  false;
    switch (b[0]) {
        case ' ':
        case ':':
        case ';':
        case ',':
        case ')':
        case '{':
        case '}':
        case '(':
            fndchr = true;
            if (!instring && emode() && b[0] == '}') {
                long sp = lines.indent(pos) - 4;
                line = Trimleft(line);
                if (sp > 0) {
                    wstring space(sp, ' ');
                    line = space;
                    line += b;
                }
                printline(pos, line);
            }
    }
    
    posinstring += b.size();
    
    if (emode()) {
        lines[pos] = line;
        lines.refactoring(pos);
        
            //our line is now too long... we need to split it...
        if (fullsize(line) > col_size) {
            currentline++;
            
            if (currentline >= row_size) {
                displaylist(poslines[0] + 1);
                currentline = row_size;
            }
            else
                displaylist(poslines[0]);
            
            pos++;
            posinstring = 1;
            line = lines[pos];
            movetoline(currentline);
            movetoposition();
            return;
        }
    }
    
    
    if (fndchr) {
        clearline();
        displaygo(true);
    }
    else
        cout << convert(b);
}

//This is the main method that launches the terminal
void jag_editor::launchterminal(char loadedcode) {

    localhelp << m_red << "^c" << m_current << ":exit";
    
    if (loadedcode) {
        displaylist(0);
        movetoline(currentline);
        posinstring = 0;
        movetobeginning();
    }
    else {
        init();
    }
    
    option = x_none;
    
    wstring bl;
    wstring b;
    
    signal(SIGINT,handle_ctrl_c);
    
    wstring code;
    string buffer;
    bool inbuffer = false;
    
    bool instring = false;
    string buff;
    long first = 0, last;

    while (1) {
        buff = _getch();
        
        if (echochar) {
            displaychar(buff);
            continue;
        }
        

        if (checkaction(buff, first, last))
            continue;
        
        if (option == x_exitprint)
            return;
        
        if (inbuffer) {
            buffer += buff;
            buff = buffer;
            inbuffer = false;
        }
        
        if (buff.size() == _getbuffsize)
            inbuffer = check_utf8(buff, buffer);
        
        bl = wconvert(buff);
        cleanheaders(bl);
        
        if (emode()) {
            //We keep track of the initial form of the line...
            undo(lines[pos],pos, u_modif); //The value is negative to indicate a deletion
        }
        
            //Only one character to add, no need for further inspection, no CR in the string as well
        if (bl.size() == 1 || buff.find(10) == -1) {
            addabuffer(bl, instring);
            continue;
        }
        
        for (long j = 0; j < bl.size(); j++) {
            b = bl[j];
            if (b[0] == 10) {
                pos = handlingeditorline(false);
                continue;
            }
            addabuffer(b, instring);
        }
    }
}

//---------------------------------------------------------------------------------

void editor_lines::setcode(wstring& code) {
    vector<wstring> buff;
    status.clear();
    jag->vsplit(code, L"\n", buff);
    lines.clear();
    numeros.clear();
    long u;
    vector<wstring> subs;
    if (buff.back() == L"")
        buff.pop_back();

    for (long i = 0; i < buff.size(); i++) {
        numeros.push_back(i + 1);
        if (splitline(buff[i], i, subs) == 1) {
            lines.push_back(buff[i]);
            status.push_back(solo_line);
            subs.clear();
            continue;
        }
        
        lines.push_back(subs[0]);
        status.push_back(beg_line);
        for (u = 1; u < subs.size(); u++) {
            lines.push_back(subs[u]);
            status.push_back(concat_line);
            numeros.push_back(i + 1);
        }
        subs.clear();
    }
    
    updatesize();
}

long editor_lines::splitline(wstring& l, long linenumber, vector<wstring>& subs) {
    return jag->splitline(l,linenumber, subs);
}

bool editor_lines::checksize(long p) {
    return jag->checksize(lines[p]);
}

bool editor_lines::checkfullsize(wstring& l, bool& equal) {
    return jag->checksizeequal(l, equal);
}

void editor_lines::undo(wstring& l, long p, char a) {
    jag->undo(l,p,a);
}

void editor_lines::refactoring(long pos) {
    long p = pos;
        //First we look for the first line of our group
    if (Status(p) == concat_line) {
        while (Status(p) != beg_line) p--;
    }
    
        //We keep this track...
    long first = p;
    wstring baseline = lines[p++];
    
        //It was not at the beginning of a line, we concatenate our block
    while (Status(p) == concat_line)
        baseline += lines[p++];
    
    if (baseline == L"")
        return;
    
    //We then clean all these lines...
    
    editor_lines sublines(jag);
    sublines.setcode(baseline);

    
    for (long i = 0; i < sublines.size(); i++) {
        if (first != pos)
            jag->undo(lines[first], first, u_modif_linked);
        if (first == lines.size())
            push_back(sublines[i]);
        else
            lines[first] = sublines[i];
        status[first] = sublines.status[i];
        first++;
    }
    
    while (first < p) {
        jag->undo(lines[first], first, u_del_linked);
        erase(first);
        p--;
    }
    
    numbers();
}

char editor_lines::updatestatus(long pos) {
    if (Status(pos) == beg_line) {
        status[pos] = solo_line;
        if (Status(pos+2) == concat_line)
            status[pos+1] = beg_line;
        else
            status[pos+1] = solo_line;
        return status[pos+1];
    }
    return -1;
}

void editor_lines::updatesize() {
    jag->setprefixesize(lines.size());
}

///------------------------------------------------------------------------------------
