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

#ifdef WIN32
#include <windows.h>
#endif

#include <stdio.h>

#ifdef WIN32
#include "conversion.h"
#include <conio.h>
#include <ctype.h>

//Handling console tune up

#ifdef DOSOUTPUT
static bool dosoutput = false;
void Setdosoutput(bool d) { dosoutput = d; }
bool isDosOutput() {
	return dosoutput;
}

string conversiontodos(char* c) {
	if (dosoutput)
		return s_utf8_to_dos(c);
	return c;
}
#endif
#else
#include <unistd.h>   //_getch
#include <termios.h>  //_getch
#include <sys/ioctl.h>
#endif

#include <signal.h>
#include <iomanip>

#include <string>

#include "conversion.h"
#include "x_tokenize.h"
#include "jagrgx.h"
#include <iomanip>
#include "jag.h"

bool check_large_char(wchar_t* src, long lensrc, long& i);

#ifdef WIN32
//these methods have been implemented in tamgusys.cxx
void ResetWindowsConsole();
void Getscreensizes();
bool checkresize();
void Returnscreensize(long& rs, long& cs, long& sr, long& sc);
#endif

#ifdef APPLE_COPY
void quoted_string(string& value) {
    if (value == "")
        return;

    value = s_replacestrings(value, "\\", "\\\\");
    value = s_replacestrings(value, "\"", "\\\"");
    value = s_replacestrings(value, "\\t", "\\\\t");
    value = s_replacestrings(value, "\\n", "\\\\n");
    value = s_replacestrings(value, "\\r", "\\\\r");

}

string exec_command(const char* cmd) {
    FILE* pipe = popen(cmd, "r");

    if (!pipe)
        return "";

    char buffer[256];
    string result = "";
    while(!feof(pipe))
    {
        if(fgets(buffer, 256, pipe) != NULL)
        {
            result += buffer;
        }
    }
    pclose(pipe);
    return result;
}

string paste_from_clipboard() {
    return exec_command("pbpaste");
}

void copy_to_clipboard(string buffer) {
    quoted_string(buffer);
    stringstream cmd;
    cmd << "echo \"" << STR(buffer) << "\" | pbcopy";
    exec_command(cmd.str().c_str());
}
#else
string paste_from_clipboard() {
    return "";
}
void copy_to_clipboard(string buffer) {}
#endif



static char m_scrollmargin[] = { 27, 91, '0', '0', '0', ';', '0', '0','0', 'r', 0 };
static char m_right[] = {27, '[', '0', '0', '1', 67, 0};
char m_down[] = {27, '[', '0', '0', '1', 66, 0};

//Moving to a specific line/column
char sys_row_column[] = { 27, 91, '0', '0', '0', ';', '0', '0','0', 'H', 0 };
static const char* localn999[] = { "000","001","002","003","004","005","006","007","008","009","010","011","012","013","014","015","016","017","018","019","020","021","022","023","024","025","026","027","028","029","030","031","032","033","034","035","036","037","038","039","040","041","042","043","044","045","046","047","048","049","050","051","052","053","054","055","056","057","058","059","060","061","062","063","064","065","066","067","068","069","070","071","072","073","074","075","076","077","078","079","080","081","082","083","084","085","086","087","088","089","090","091","092","093","094","095","096","097","098","099","100","101","102","103","104","105","106","107","108","109","110","111","112","113","114","115","116","117","118","119","120","121","122","123","124","125","126","127","128","129","130","131","132","133","134","135","136","137","138","139","140","141","142","143","144","145","146","147","148","149","150","151","152","153","154","155","156","157","158","159","160","161","162","163","164","165","166","167","168","169","170","171","172","173","174","175","176","177","178","179","180","181","182","183","184","185","186","187","188","189","190","191","192","193","194","195","196","197","198","199","200","201","202","203","204","205","206","207","208","209","210","211","212","213","214","215","216","217","218","219","220","221","222","223","224","225","226","227","228","229","230","231","232","233","234","235","236","237","238","239","240","241","242","243","244","245","246","247","248","249","250","251","252","253","254","255","256","257","258","259","260","261","262","263","264","265","266","267","268","269","270","271","272","273","274","275","276","277","278","279","280","281","282","283","284","285","286","287","288","289","290","291","292","293","294","295","296","297","298","299","300","301","302","303","304","305","306","307","308","309","310","311","312","313","314","315","316","317","318","319","320","321","322","323","324","325","326","327","328","329","330","331","332","333","334","335","336","337","338","339","340","341","342","343","344","345","346","347","348","349","350","351","352","353","354","355","356","357","358","359","360","361","362","363","364","365","366","367","368","369","370","371","372","373","374","375","376","377","378","379","380","381","382","383","384","385","386","387","388","389","390","391","392","393","394","395","396","397","398","399","400","401","402","403","404","405","406","407","408","409","410","411","412","413","414","415","416","417","418","419","420","421","422","423","424","425","426","427","428","429","430","431","432","433","434","435","436","437","438","439","440","441","442","443","444","445","446","447","448","449","450","451","452","453","454","455","456","457","458","459","460","461","462","463","464","465","466","467","468","469","470","471","472","473","474","475","476","477","478","479","480","481","482","483","484","485","486","487","488","489","490","491","492","493","494","495","496","497","498","499","500","501","502","503","504","505","506","507","508","509","510","511","512","513","514","515","516","517","518","519","520","521","522","523","524","525","526","527","528","529","530","531","532","533","534","535","536","537","538","539","540","541","542","543","544","545","546","547","548","549","550","551","552","553","554","555","556","557","558","559","560","561","562","563","564","565","566","567","568","569","570","571","572","573","574","575","576","577","578","579","580","581","582","583","584","585","586","587","588","589","590","591","592","593","594","595","596","597","598","599","600","601","602","603","604","605","606","607","608","609","610","611","612","613","614","615","616","617","618","619","620","621","622","623","624","625","626","627","628","629","630","631","632","633","634","635","636","637","638","639","640","641","642","643","644","645","646","647","648","649","650","651","652","653","654","655","656","657","658","659","660","661","662","663","664","665","666","667","668","669","670","671","672","673","674","675","676","677","678","679","680","681","682","683","684","685","686","687","688","689","690","691","692","693","694","695","696","697","698","699","700","701","702","703","704","705","706","707","708","709","710","711","712","713","714","715","716","717","718","719","720","721","722","723","724","725","726","727","728","729","730","731","732","733","734","735","736","737","738","739","740","741","742","743","744","745","746","747","748","749","750","751","752","753","754","755","756","757","758","759","760","761","762","763","764","765","766","767","768","769","770","771","772","773","774","775","776","777","778","779","780","781","782","783","784","785","786","787","788","789","790","791","792","793","794","795","796","797","798","799","800","801","802","803","804","805","806","807","808","809","810","811","812","813","814","815","816","817","818","819","820","821","822","823","824","825","826","827","828","829","830","831","832","833","834","835","836","837","838","839","840","841","842","843","844","845","846","847","848","849","850","851","852","853","854","855","856","857","858","859","860","861","862","863","864","865","866","867","868","869","870","871","872","873","874","875","876","877","878","879","880","881","882","883","884","885","886","887","888","889","890","891","892","893","894","895","896","897","898","899","900","901","902","903","904","905","906","907","908","909","910","911","912","913","914","915","916","917","918","919","920","921","922","923","924","925","926","927","928","929","930","931","932","933","934","935","936","937","938","939","940","941","942","943","944","945","946","947","948","949","950","951","952","953","954","955","956","957","958","959","960","961","962","963","964","965","966","967","968","969","970","971","972","973","974","975","976","977","978","979","980","981","982","983","984","985","986","987","988","989","990","991","992","993","994","995","996","997","998","999" };

static const char* _keywords[] = { "GPSdistance","True", "False","None","def","char","import","double","wstring","Maybe","Nothing","_breakpoint","_dependencies","_erroronkey","_eval","_evalfunction","_exit","_forcelocks","_getdefaulttokenizerules","_info","_initial","_mirrordisplay","_nbthreads","_poolstats","_setenv","_setmaxrange","_setmaxthreads","_setstacksize","_setvalidfeatures","_stdin","_symbols","_threadhandle","_threadid","a_bool","a_change","a_delete","a_features","a_first","a_float","a_fvector","a_insert","a_int","a_ivector","a_longest","a_mapff","a_mapfi","a_mapfs","a_mapfu","a_mapif","a_mapii","a_mapis","a_mapiu","a_mapsf","a_mapsi","a_mapss","a_mapuf","a_mapui","a_mapuu","a_nocase","a_offsets","a_random","a_repetition","a_string","a_surface","a_svector","a_switch","a_ustring","a_uvector","a_vector","a_vowel","abs","absent","acos","acosh","activate","add","addchild","addnext","addprevious","addstyle","after","alert","align","all","allobjects","and","annotate","annotator","any","aopen","append","apply","arc","asin","asinh","ask","assert","asserta","assertz","atan","atanh","attributes","autorun","backgroundcolor","barcolor","base","before","begin","begincomplexpolygon","beginline","beginloop","beginpoints","beginpolygon","binmap","binmapf","binmapi","binmapl","binmaps","binmapu","bit","bitmap","bits","block","blocking","bloop","bool","border","boundaries","bounds","box","boxtype","break","browser","buffersize","build","button","bvector","byte","byteposition","bytes","call","case","cast","catch","cbrt","cell","cellalign","cellalignheadercol","cellalignheaderrow","charposition","check","checklabel","checkword","child","children","choice","chr","circle","clear","close","color","colorbg","colorfg","column","columnchar","columnheader","columnheaderwidth","columnwidth","command","commit","common","compact","compile","compilelexicons","compilergx","concept","connect","const","constmap","constvector","content","continue","copy","cos","cosh","count","counter","create","created","createdirectory","createserver","curl","current","cursor","cursorchar","cursorstyle","curve","cut","cycle","data","date","day","deaccentuate","decimal","decode","default","definitions","degree","delete","dependencies","dependency","deriving","deselect","determinant","dimension","dloop","do","document","dos","dostoutf8","doublemetaphone","drawcolor","drawtext","drop","dropWhile","dthrough","dvector","e_arabic","e_baltic","e_celtic","e_cp1252","e_cyrillic","e_greek","e_hebrew","e_latin_ce","e_latin_ffe","e_latin_ne","e_latin_se","e_latin_see","e_latin_we","e_nordic","e_thai","e_turkish","e_windows","editdistance","editor","elif","else","emoji","emojis","empty","encode","end","endcomplexpolygon","endian","endline","endloop","endpoints","endpolygon","env","eof","erf","erfc","evaluate","even","exclusive","execute","exit","exp","exp2","expm1","exponent","extension","extract","factorize","factors","fail","false","features","fibre","file","filebrowser","fileinfo","fill","filter","find","findall","first","flatten","flip","float","floop","floor","flush","fmatrix","focus","foldl","foldr","font","fontnumber","fontsize","for","format","formatchar","fraction","frame","frameinstance","from","fthrough","function","fvector","gap","get","geterr","getfont","getfontsizes","gethostname","getpeername","getstd","getstyle","gotoline","grammar","grammar_macros","group","hash","hide","highlight","hour","html","hvector","id","if","ifnot","iloop","image","imatrix","in","indent","info","initializefonts","insert","insertbind","int","invert","is","isa","isalpha","isconsonant","iscontainer","isdigit","isdirectory","isemoji","ishangul","isjamo","islower","ismap","isnumber","isprime","ispunctuation","isstring","isupper","isutf8","isvector","isvowel","items","iterator","ithrough","ivector","jamo","join","joined","json","key","keys","kget","label","labelcolor","labelfont","labels","labelsize","labeltype","last","latin","leaves","left","length","let","levenshtein","lexicon","lgamma","line","linebounds","linecharbounds","lineshape","lisp","list","listdirectory","lloop","ln","load","loadfacts","loadgif","loadin","loadjpeg","lock","log","log1p","log2","logb","long","lookdown","lookup","loop","lower","ls","lstep","lthrough","lvector","mantissa","map","mapf","mapff","mapfi","mapfl","mapfs","mapfu","mapi","mapif","mapii","mapis","mapiu","mapl","maplf","mapll","mapls","maplu","mapsf","mapsi","mapsl","mapss","mapu","mapuf","mapui","mapul","mapuu","mark","match","max","maximum","memory","menu","merge","method","methods","mid","min","minimum","minute","mkdir","modal","month","mp3","mthrough","multisplit","multmatrix","name","namespace","nbchildren","nd","nearbyint","new","next","ngrams","node","nope","normalizehangul","not","notin","null","odd","of","ok","onclose","onclosing","ondragdrop","onhscroll","onkey","onmouse","ontime","ontology","onvscroll","open","openappend","openread","openwrite","options","or","ord","otherwise","padding","parameters","parent","parse","password","paste","pause","permute","pie","play","plot","plotcoords","point","polygon","polynomial","ponder","pop","popclip","popfirst","poplast","popmatrix","port","position","post","precede","pred","predicate","predicatedump","predicatevar","preg","previous","primemap","primemapf","primemapff","primemapfi","primemapfl","primemapfs","primemapfu","primemapi","primemapif","primemapii","primemapis","primemapiu","primemapl","primemaplf","primemapll","primemapls","primemaplu","primemapsf","primemapsi","primemapsl","primemapss","primemapu","primemapuf","primemapui","primemapul","primemapuu","print","printerr","printj","printjerr","printjln","printjlnerr","println","printlnerr","private","product","progress","properties","property","protected","proxy","push","pushclip","pushfirst","pushlast","pushmatrix","radian","raise","random","range","rawstring","read","readline","real","realpath","receive","rectangle","rectanglefill","redirectoutput","redraw","redrawing","remove","removefirst","removelast","repeat","replace","replaceall","replicate","reserve","reset","resizable","resize","restateoutput","retract","retractall","return","reverse","rfind","rgbcolor","right","ring","rint","role","romanization","root","ropen","rotate","rotation","round","row","rowheader","rowheaderheight","rowheight","rule","run","save","scale","scan","scanl","scanr","scrollbar","second","seek","select","selection","selectioncolor","self","send","serialize","serializestring","setdate","setstyle","settimeout","short","shortcut","show","shuffle","sibling","signature","simplify","sin","sinh","sisters","size","sizeb","sizerange","sleep","slice","slider","sloop","socket","sort","sortfloat","sortint","sortstring","sortustring","sound","spans","split","splite","sqlite","sqrt","static","step","steps","sthrough","stokenize","stop","store","strict","string","succ","succeed","sum","svector","switch","synode","sys","table","tabs","tags","take","takeWhile","tamgu","tan","tanh","tell","term","test","textcolor","textsize","tgamma","this","thread","time","tohtml","token","tokenize","tokens","tooltip","toxml","trace","transducer","transformdx","transformdy","transformedvertex","transformx","transformy","translate","transpose","treemap","treemapf","treemapff","treemapfi","treemapfl","treemapfs","treemapfu","treemapi","treemapif","treemapii","treemapis","treemapiu","treemapl","treemaplf","treemapll","treemapls","treemaplu","treemapsf","treemapsi","treemapsl","treemapss","treemapu","treemapuf","treemapui","treemapul","treemapuu","treg","trim","trimleft","trimright","true","trunc","try","type","ufile","uloop","unblock","unget","unhighlight","unique","unlink","unlock","unmark","upper","url","use","ustring","utf8","uthrough","uvector","value","values","vector","version","vertex","vthrough","wait","waitonfalse","waitonjoined","weekday","wfile","when","where","while","window","winput","with","wopen","word","words","woutput","wrap","write","writebin","writeln","writeutf16","wtable","xml","xmldoc","xmlstring","xmltype","xor","xpath","year","yearday","zip","zipWith","∏","∑","√","∛","define", "ifdef", "endif", "include", "defun", "lambda", "cons", "cond", "eq", "car", "cdr", "list", "append", ""};


static void moveto_row_column(long r, long c) {
    sys_row_column[2] = localn999[r][0];
    sys_row_column[3] = localn999[r][1];
    sys_row_column[4] = localn999[r][2];
    sys_row_column[6] = localn999[c][0];
    sys_row_column[7] = localn999[c][1];
    sys_row_column[8] = localn999[c][2];
    cout << sys_row_column;
}

void jag_editor::displaythehelp(long noclear) {
    if (!noclear) {
        cout << m_clear << m_clear_scrolling << m_home;
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
    cerr << "   \t\t- " << m_redital << "P:" << m_current << " find with a POSIX Regular Expression" << endl;
    cerr << "   \t\t- " << m_redital << "R:" << m_current << " find with a Tamgu Regular Expression" << endl;
    cerr << "   \t\t- " << m_redital << "f:" << m_current << " find with previous find string" << endl;
    cerr << "   \t\t- " << m_redital << "m:" << m_current << " toggle mouse on/off" << endl;
    cerr << "   \t\t- " << m_redital << "H:" << m_current << " convert HTML entities to Unicode characters" << endl;
    cerr << "   \t\t- " << m_redital << "D:" << m_current << " delete a bloc of lines" << endl;
    cerr << "   \t\t- " << m_redital << "n:" << m_current << " hide/display line numbers" << endl;
    cerr << "   \t\t- " << m_redital << "c:" << m_current << " copy a bloc of lines" << endl;
    cerr << "   \t\t- " << m_redital << "x:" << m_current << " cut a bloc of lines" << endl;
    cerr << "   \t\t- " << m_redital << "v:" << m_current << " paste a bloc of lines" << endl;
    cerr << "   \t\t- " << m_redital << "w:" << m_current << " write and quit" << endl;
    cerr << "   \t\t- " << m_redital << "l:" << m_current << " load a file" << endl;
    cerr << "   \t\t- " << m_redital << "b:" << m_current << " black mode (better suited for black background)" << endl;
    cerr << "   \t\t- " << m_redital << "h:" << m_current << " full help" << endl;
    cerr << "   \t\t- " << m_redital << "q:" << m_current << " quit" << endl << endl;

    cerr << "   " << m_redbold << e_regular_expressions_for << m_redital << "'find'" << m_current << endl;
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

static bool checkcar(wchar_t c) {
    if (c <= 32)
        return false;

    switch (c) {
        case '(':
        case ')':
        case ']':
        case '{':
        case '}':
        case '"':
        case ':':
        case ',':
        case ';':
            return false;
        default:
            return true;
    }
}

static void scrollingdown(long rowsize) {
	char buff[] = { 0,0,0,0,0,0 };
	sprintf_s(buff, 4, "%0.3ld", rowsize + 1);
	m_scrollmargin[6] = buff[0];
	m_scrollmargin[7] = buff[1];
	m_scrollmargin[8] = buff[2];
	cout << m_scrollmargin << m_scrolldown;
	sprintf_s(buff, 4, "%0.3ld", rowsize + 2);
	m_scrollmargin[6] = buff[0];
	m_scrollmargin[7] = buff[1];
	m_scrollmargin[8] = buff[2];
	cout << m_scrollmargin;
}

#ifdef WIN32
static char check_size_utf8(int utf) {
	if ((utf & 0xF0) == 0xF0)
		return 3;

	if ((utf & 0xE0) == 0xE0)
		return 2;

	if ((utf & 0xC0) == 0xC0)
		return 1;

	return 0;
}

string getwinchar(void(*f)());
void resizewindow();
string jag_editor::getch() {
	return getwinchar(resizewindow);
}
#else
string jag_editor::getch(){
    static char buf[_getbuffsize+2];
    memset(buf,0, _getbuffsize);

    struct termios old={0};
    fflush(stdout);
    if(tcgetattr(0, &old)<0) {
        perror("tcsetattr()");
        exit(-1);
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
        memset(buf,0, _getbuffsize);
    }
    while (nb == _getbuffsize);

    old.c_lflag|=ICANON;
    if (!isMouseAction(res))
        old.c_lflag|=ECHO;
    if(tcsetattr(0, TCSADRAIN, &old)<0) {
        perror ("tcsetattr ~ICANON");
        return "";
    }

    return res;
}
#endif

//--------------------------------------------------------
void jag_editor::indentplus() {
    wstring blanks(GetBlankSize(), ' ');
    tobesaved = true;
    if (selected_pos != -1 && selected_posnext > selected_pos) {
        uchar modif = u_modif;
        for (long i = selected_posnext - 1; i >= selected_pos; i--) {
            line = lines[i];
            line = blanks + line;
            undo(lines[i],i, modif);
            modif = u_modif_linked;
            lines[i] = line;
        }
        selectlines(selected_pos, selected_posnext, selected_x, selected_y);
        return;
    }
    line = lines[pos];
    line = blanks + line;
    undo(lines[pos],pos, u_modif);
    lines[pos] = line;
    printline(pos, line, -1);
    movetoline(currentline);
    movetoposition();
}

void jag_editor::deindentminus() {
    //We remove some blanks from lines...
    long nb = GetBlankSize();
    long u = 0;
    if (selected_pos != -1 && selected_posnext > selected_pos) {
        uchar modif = u_modif;
        for (long i = selected_posnext - 1; i >= selected_pos; i--) {
            line = lines[i];
            //can we remove nb blanks from the beginning of the line
            u = 0;
            for (; u < line.size() && line[u] == ' '; u++) {}
            if (u >= nb) {
                tobesaved = true;
                line = line.substr(nb, line.size());
                undo(lines[i],i, modif);
                modif = u_modif_linked;
                lines[i] = line;
            }
        }
        selectlines(selected_pos, selected_posnext, selected_x, selected_y);
        return;
    }

    line = lines[pos];
    for (; u < line.size() && line[u] == ' '; u++) {}
    if (u >= nb) {
        tobesaved = true;
        long sz = line.size();
        line = line.substr(nb, line.size());
        undo(lines[pos],pos, u_modif);
        lines[pos] = line;
        wstring blanks(sz, ' ');
        printline(pos, blanks, -1);
        printline(pos, line, -1);
        movetoline(currentline);
        movetoposition();
    }
}

void jag_editor::colorring(string& txt, vector<long>& limits) {
    static hmap<string,bool> keys;
    static x_coloringrule color_tok;
    static tokenizer_result<string> xr;
    static bool init=false;

    if (txt == "")
        return;

    if (!init) {
        init=true;
        keywords(keys);
    }

    long sztxt = txt.size();

    txt += "\n";
    color_tok.tokenize<string>(txt, xr);
    txt.pop_back();

    char type;
    long gauche,droite,i;
    long sz=xr.stack.size();
    string sub;

    for (i=0;i<sz;i++) {
        type=xr.stacktype[i];
        gauche=xr.bpos[i];
        droite = gauche + xr.stack[i].size();
        if (droite > sztxt)
            droite = sztxt;

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
#ifdef WIN32
void resizewindow() {
	JAGEDITOR->resetscreen();
}
#else
void resizewindow(int theSignal) {
    JAGEDITOR->resetscreen();
}
#endif
///------------------------------------------------------------------------------------

jag_editor::jag_editor() : lines(this) {
#ifndef WIN32
    //we reset the input stream
    freopen("/dev/tty", "rw", stdin);

    tcgetattr(0, &oldterm);
    promptmode = false;
    
    //We enable ctrl-s and ctrl-q within the editor
    termios theterm;
    tcgetattr(0, &theterm);
    theterm.c_iflag &= ~IXON;
    theterm.c_iflag |= IXOFF;
    theterm.c_cc[VSTART] = NULL;
    theterm.c_cc[VSTOP] = NULL;
    theterm.c_cc[VSUSP] = NULL;
    tcsetattr(0, TCSADRAIN, &theterm);
#endif

    activate_mouse = false;
    vt100 = false;
    mouse_status = false;
    
    linematch = -1;
    selected_x = -1;
    selected_firstline = -1;
    selected_y = -1;
    selected_pos = -1;
    selected_posnext = -1;
    double_click = 0;
    nbclicks = 0;

	insertaline = false;
    margin = 3;

    noprefix = false;
    current_no_prefix = false;
    tooglehelp = false;
    regularexpressionfind = false;
    findrgx = NULL;

    #ifdef Tamgu_REGEX
        wpattern = NULL;
    #endif

    prefixsize = 1;

    xcursor = 0;
    ycursor = 0;

#ifndef WIN32
    signal(SIGWINCH, resizewindow);
#endif
    colors.push_back(m_red);
    colors.push_back(m_dore);
    colors.push_back(m_blue);
    colors.push_back(m_gray);
    colors.push_back(m_green);
    colors.push_back(m_selectgray);

    poscommand = 0;
    echochar = false;
    option = x_none;
    pos = 0;
    posinstring  = 0;
    currentline = 0;
    currentfindpos = 0;
    currentposinstring = -1;
#ifdef WIN32
	prefix = "<>";
	wprefix = L"<>";
#else
    prefix = "작";
    wprefix = L"작";
#endif
    replaceall = false;
    modified = true;
    tobesaved = false;
    inittableutf8();
	row_size = -1;
	col_size = -1;
    screensizes();
    localhelp << m_red<< "^xh" << m_current << ":help " << m_red<< "^k" << m_current << ":del after " << m_red<< "^p" << m_current << ":k-buffer " <<  m_red<< "^d" << m_current << ":del line " << m_red<< "^uz/^r" << m_current << ":un/redo " << m_red<< "^f" << m_current << ":find " << m_red<< "^n" << m_current << ":next " << m_red<< "^g" << m_current << ":go " << m_red<< "^l" << m_current << ":top/bottom " << m_red<< "^t" << m_current << ":indent " << m_red<< "^s/w" << m_current << ":write " << m_red<< "^x" << m_current << ":commands ";

    updateline = true;
    taskel = true;

    JAGEDITOR = this;
}


jag_editor::jag_editor(string prf) : lines(this) {
    activate_mouse = false;
    vt100 = false;
    mouse_status = false;
    
    linematch = -1;
    selected_x = -1;
    selected_firstline = -1;
    selected_y = -1;
    selected_pos = -1;
    selected_posnext = -1;
    double_click = 0;
    nbclicks = 0;

    insertaline = false;
    margin = 3;

    noprefix = false;
    current_no_prefix = false;
    tooglehelp = false;
    regularexpressionfind = false;
    findrgx = NULL;

    #ifdef Tamgu_REGEX
        wpattern = NULL;
    #endif

    promptmode = true;

    xcursor = 0;
    ycursor = 0;

    colors.push_back(m_red);
    colors.push_back(m_dore);
    colors.push_back(m_blue);
    colors.push_back(m_gray);
    colors.push_back(m_green);
    colors.push_back(m_selectgray);

    poscommand = 0;
    echochar = false;
    option = x_none;
    pos = 0;
    posinstring  = 0;
    currentline = 0;
    currentfindpos = 0;
    currentposinstring = -1;

    prefix = prf;
    wprefix = wconvert(prf);
    prefixsize = (int)size_of_prefix();
    
    replaceall = false;
    modified = true;
    tobesaved = false;
    inittableutf8();
    row_size = -1;
    col_size = -1;
    screensizes();
    updateline = true;
    taskel = true;
}

jag_editor::~jag_editor() {
    #ifdef Tamgu_REGEX
        if (wpattern != NULL)
            delete wpattern;
    #endif

    if (findrgx != NULL)
        delete findrgx;
}

//------------------------------------------------------------------------------------
void jag_editor::deleteselection() {
    if (selected_pos == -1)
        return;

    if (selected_pos == selected_posnext) {
        if (selected_x == 0 && selected_y == line.size()) {
            deleteline(0);
            resetselection();
            return;
        }

        undo(lines[pos],pos, u_modif);
        deleteachar(line, false, selected_x);
        lines[selected_pos] = line;
        clearline();
        printline(pos+1, line, -1);
        posinstring = selected_x;
        movetoposition();
        resetselection();
        return;
    }

    long first = selected_pos;
    long last = selected_posnext;
    wstring savedline = lines[selected_posnext];

    if (selected_x == 0) {
        //first line must be deleted, last line is modified or deleted
        if (selected_y < savedline.size()) {
            undo(savedline, selected_posnext, u_modif);
            last--;
        }
        else {
            undo(savedline, selected_posnext, u_del);
            last--;
        }
    }
    else {
        undo(savedline, selected_posnext, u_del);
        last--;
    }

    while (last > first) {
        undo(lines[last], last, u_del_linked);
        last--;
    }

    if (selected_x == 0)
        undo(lines[selected_pos], selected_pos, u_del_linked);
    else
        undo(lines[selected_pos], selected_pos, u_modif_linked);

    last = selected_posnext;

    posinstring  = 0;

    if (selected_y >= savedline.size())
        selected_y = savedline.size();


    if (selected_x == 0) {
        if (selected_y != savedline.size()) {
            lines[selected_posnext] = savedline.substr(selected_y,savedline.size());
            posinstring = selected_y;
            last--;
        }
    }
    else {
        lines[selected_pos] = lines[selected_pos].substr(0, selected_x);
        if (selected_y != savedline.size()) {
            lines[selected_pos] += savedline.substr(selected_y,savedline.size());
        }
        posinstring = lines[selected_pos].size();
        first++;
    }

    lines.erase(first, last+1);

    displaylist(poslines[0]);
    movetoline(selected_firstline);
    movetoposition();
    resetselection();
    double_click = 0;
}


void jag_editor::displayextract(wstring& sub, long pos, long from_pos, long to_pos, bool select) {
    if (from_pos > sub.size())
        return;

    if (to_pos > sub.size())
        to_pos = sub.size();

    posinstring = to_pos;
    clearline();
    if (select) {
        wstring start = sub.substr(0, from_pos);
        wstring middle = sub.substr(from_pos, to_pos-from_pos);
        wstring end = sub.substr(to_pos, sub.size());
        kbuffer += middle;
        string inter = convert(start);
        inter += colors[5];
        inter += convert(middle);
        inter +=  m_current;
        inter += convert(end);
        printline(pos + 1, inter);
    }
    else
        printline(pos + 1, sub, -1);
}

void jag_editor::selectlines(long from_line, long to_line, long from_pos, long to_pos) {
    wstring sub;

    if (to_line < from_line)
        return;

    kbuffer = L"";
    sub = lines[from_line];
    char stat = lines.Status(from_line);
    if (from_line == to_line) {
        movetoline(currentline);
        displayextract(sub, from_line, from_pos, to_pos);
        if (stat == solo_line || lines.Status(from_line+1)!= concat_line) {
            if (to_pos == sub.size())
                kbuffer += L"\n";
        }
        movetoposition();
        return;
    }

    long current = selected_firstline;
    //Else we display first, the missing part
    movetoline(current++);
    displayextract(sub, from_line++, from_pos, sub.size());
    if (stat == solo_line || lines.Status(from_line)!= concat_line)
        kbuffer += L"\n";

    while (from_line < to_line) {
        sub = lines[from_line];
        movetoline(current++);
        stat = lines.Status(from_line);
        displayextract(sub, from_line++, 0, sub.size());
        if (stat == solo_line || lines.Status(from_line) != concat_line) {
            kbuffer += L"\n";
        }
    }

    stat = lines.Status(from_line);
    movetoline(current);
    sub = lines[from_line];
    displayextract(sub, from_line, 0, to_pos);
    if (stat == solo_line || lines.Status(from_line+1) != concat_line) {
        if (to_pos == sub.size())
            kbuffer += L"\n";
    }
    movetoposition();
}

void jag_editor::unselectlines(long from_line, long to_line, long from_pos, long to_pos) {
    wstring sub;

    if (to_line < from_line)
        return;

    sub = lines[from_line];
    if (from_line == to_line) {
        movetoline(currentline);
        displayextract(sub, from_line, from_pos, to_pos, false);
        return;
    }

    long current = currentline;
    //Else we display first, the missing part
    movetoline(current++);
    displayextract(sub, from_line++, from_pos, sub.size(), false);
    while (from_line < to_line) {
        sub = lines[from_line];
        movetoline(current++);
        displayextract(sub, from_line++, 0, sub.size(), false);
    }
    movetoline(current);
    sub = lines[from_line];
    displayextract(sub, from_line, 0, to_pos, false);
}

void jag_editor::computeposition(int& p, long position) {
    wstring s = lines[position];
    p -= prefixe() + 1;
    long i = 0;
    int pos = 0;
    TAMGUCHAR c;
    while (pos < p) {
        if (scan_emoji(s, i))
            pos+=2;
        else {
            c = getachar(s, i);
            if (ckjchar(c)) {
                pos+=2;
            }
            else {
                if (c == 9) //tab position
                    pos += 8;
                else
                    pos++;
            }
        }
        i++;
    }
    p = i;
}

void jag_editor::handlemousectrl(string& mousectrl) {
    vector<int> location;

    if (isScrollingUp(location,mousectrl)) {
        if (selected_pos != -1) {
            unselectlines(selected_pos, selected_posnext, selected_x, selected_y);
            resetselection();
            double_click = 0;
        }

        pos = poslines[0];
        currentline = 0;
        updown(is_up, pos);
        return;
    }

    if (isScrollingDown(location,mousectrl)) {
        if (selected_pos != -1) {
            unselectlines(selected_pos, selected_posnext, selected_x, selected_y);
            resetselection();
            double_click = 0;
        }

        pos = poslines.back();
        currentline = poslines.size() - 1;
        updown(is_down, pos);
        return;
    }

    if (isClickFirstMouseDown(location, mousectrl)) {
        if (selected_pos != -1)
            unselectlines(selected_pos, selected_posnext, selected_x, selected_y);

        currentline = location[0] - 1;

        if (currentline < 0)
            currentline = 0;
        if (currentline >= poslines.size())
            currentline = poslines.size()-1;

        pos = poslines[currentline];
        line = lines[pos];
        long endofstring = line.size();
        if (endofstring && !lines.eol(pos))
            endofstring--;


        int mxcursor = 0, mycursor;
        long l, r;
        computeposition(location[1], pos);
        if (location[1] < endofstring)
            l = location[1];
        else
            l = endofstring;

        if (l < 0)
            l = 0;

        mycursor = location[1];
        int cursor_y;
        long posnext = pos;
        r = l;


        resetselection();
        selected_firstline = currentline;

        if (!checkMouseup(mousectrl)) {
            mousectrl = getch();

            while (mouseTracking(mousectrl, mxcursor, cursor_y)) {
                if (cursor_y != mycursor)
                    unselectlines(pos, posnext, l, r);
                double_click = 0;
                if (mxcursor >= poslines.size())
                    mxcursor = (int)poslines.size();

                posnext = poslines[mxcursor - 1];
                computeposition(cursor_y, posnext);

                if (cursor_y < 0)
                    cursor_y = 0;
                mycursor = cursor_y;
                r = cursor_y;
                selectlines(pos, posnext, l, r);
                mousectrl = getch();
            }
        }
        else {
            mxcursor = location[0];
            if (mxcursor >= poslines.size())
                mxcursor = (int)poslines.size();

        }

        //a simple click
#ifdef WIN32
        if (location[0] == mxcursor && location[1] == mycursor) {
            double_click += nbClicks();
#else
        if (location[0] == mxcursor && location[1] == mycursor) {
            double_click++;
#endif
            posinstring = l;

            if (double_click >= 2) {
                if (double_click >= 3) {
                    double_click = 0;
                    nbclicks = 0;
                    l = 0;
                    r = endofstring;
                    selected_pos = pos;
                    selected_posnext = pos;

                    selected_x = l;
                    selected_y = r;
                    selectlines(pos, pos, l,r);
                    posinstring = r;
                    movetoposition();
                    return;
                }

                while (l >= 0 && l < line.size() && checkcar(line[l])) l--;
                if (l < 0)
                    l = 0;
                else {
                    if (!checkcar(line[l]))
                        l++;
                }
                r = l+1;
                while (r < endofstring && checkcar(line[r])) r++;
                if (l < r && l < endofstring) {
                    selected_pos = pos;
                    selected_posnext = pos;
                    selected_x = l;
                    selected_y = r;
                    selectlines(pos, pos, l, r);
                    posinstring = r;
                    movetoposition();
                    return;
                }
            }

            movetoline(currentline);
            movetoposition();
            return;
        }

        //a selection
        selected_pos = pos;
        selected_posnext = posnext;
        double_click = 0;
        nbclicks = 0;
        selected_x = l;
        selected_y = r;
        return;
    }

    double_click = 0;
}

///------------------------------------------------------------------------------------
void jag_editor::screensizes() {
#ifdef WIN32
	if (row_size == -1 && col_size == -1)
		Getscreensizes();
	Returnscreensize(row_size, col_size, size_row, size_col);
	row_size -= 1;
	col_size -= margin;
#else
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &wns);
	row_size = wns.ws_row - 2; //we need to keep a final line on screen for operators
	col_size = wns.ws_col - margin;
	if (row_size < 0)
		row_size = 55;
	if (col_size < 0)
		col_size = 190;
#endif
    if (!promptmode) {
        //We set the botton limit for scrolling
        char buff[] = { 0,0,0,0,0,0 };
        sprintf_s(buff,4, "%0.3ld", row_size+2);
        m_scrollmargin[6] = buff[0];
        m_scrollmargin[7] = buff[1];
        m_scrollmargin[8] = buff[2];
        cout << m_scrollmargin;
    }
}

void jag_editor::reset() {
#ifndef WIN32
    tcsetattr(0, TCSADRAIN, &oldterm);

    termios theterm;
    tcgetattr(0, &theterm);
    theterm.c_iflag &= ~IXON;
    theterm.c_iflag |= IXOFF;
    //The next modifications allows for the use of ctrl-q and ctrl-s
    theterm.c_cc[VSTART] = 0;
    theterm.c_cc[VSTOP] = 0;
    theterm.c_cc[VSUSP] = 0;
    tcsetattr(0, TCSADRAIN, &theterm);
#endif
}

void jag_editor::setscrolling() {
    getcursor();
#ifndef WIN32
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &wns);
    char buffer[20];
    sprintf_s(buffer, 20, "\33[%d,%dr", xcursor, wns.ws_row);
    cout << buffer;
#endif
}

void jag_editor::resetscrolling() {
    char buffer[10];
    strcpy(buffer, "\33[r");
    cout << buffer;
}

long jag_editor::sizestring(wstring& s) {
    long sz = s.size();
    long szstr = 0;
    for (long i = 0; i < sz; i++) {
        if (s[i] == 27) {
            while (i < sz && s[i] != 'm') i++;
            continue;
        }
        if (!scan_emoji(s, i)) {
            getachar(s, i);
        }
        szstr++;
    }
    return szstr;
}

long jag_editor::size_upto(wstring& s, long p) {
    long pref = prefixego() + 1;
    long pos = pref;
    TAMGUCHAR c;
#ifdef WIN32
	p = min(p, (long)s.size());
#else
    p = std::min(p, (long)s.size());
#endif
    for (long i = 0; i < p; i++) {
        if (s[i] == 27) {
            while (i < p && s[i] != 'm') i++;
            continue;
        }
        if (scan_emoji(s, i))
            pos += 2;
        else {
            c = getachar(s, i);
            if (ckjchar(c)) {
                pos += 2;
            }
            else {
                if (c == 9) //tab position
                    pos += (8 - (pos%8))%8;
                pos++;
            }
        }
    }
    return (pos-pref);
}

long jag_editor::taille(wstring& s) {
    long sz = s.size();
    long pref = prefixego() + 1;
    long pos = pref;
    TAMGUCHAR c;
    for (long i = 0; i < sz; i++) {
        if (s[i] == 27) {
            while (i < sz && s[i] != 'm') i++;
            continue;
        }

        if (scan_emoji(s, i))
            pos += 2;
        else {
            c = getachar(s, i);
            if (ckjchar(c)) {
                pos += 2;
            }
            else {
                if (c == 9) //tab position
                    pos += (8 - (pos%8))%8;
                pos++;
            }
        }
    }
    return (pos-pref);
}

long jag_editor::size_of_prefix() {
    long sz = 0;
    TAMGUCHAR c;
    for (long i = 0; i < wprefix.size(); i++) {
        if (wprefix[i] == 27) {
            while (i < wprefix.size() && wprefix[i] != 'm') i++;
            continue;
        }
        if (scan_emoji(wprefix, i))
            sz += 2;
        else {
            c = getachar(wprefix, i);
            if (ckjchar(c)) {
                sz += 2;
            }
            else {
                if (c == 9) //tab position
                    sz += (8 - (pos%8))%8;
                sz++;
            }
        }
    }
    return sz;
}


string jag_editor::coloringline(wstring& l, long current_pos, bool select) {
    string line = convert(l);
    string sub;
    string substring;
    long i;
    
    if (current_pos >= 0 && current_pos < lines.longlines.size()) {
        char long_line = lines.longlines[current_pos];
        if (long_line == 1) {
            line = colors[0] + line + m_current;
            return line;
        }
        else {
            if (long_line == 2) {
                line = colors[4] + line + m_current;
                return line;
            }
            else {
                if (long_line == 10) {
                    i = line.find("@\"");
                    line.insert(i, colors[0]);
                }
            }
        }
    }

    vector<long> tobecolored;
    colorring(line, tobecolored);

    i = 0;

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
            if (right > left)
                sub += line.substr(left, right-left);
            sub += m_current;
            if (right < line.size())
                sub += line.substr(right, line.size() - right);
            line = sub;
        }
    }
    line += substring;
    return line;
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
    string inter = colors[5];
    inter += convert(lsub);
    inter += m_current;

    l = line.find("<!@!<");
    if (l==-1) {
        movetoposition();
        return;
    }
    line.replace(l, 5, inter);
    if (noprefix)
        printline(lines.numeros[pos], line);
    else {
        if (lines.status[pos] == concat_line) {
            string space(prefixe(), ' ');
            cout << back << space << line;
        }
        else
            printline(lines.numeros[pos], line);
    }
    movetoposition();
}

void jag_editor::getcursor() {
    xcursor = getxcursor();
    ycursor = getycursor();
}

void jag_editor::getcursorxy(int& x, int& y) {
    cout << cursor_position;
    string buff = getch();
    sscanf(STR(buff), "\033[%d;%dR" , &x, &y);
    clearline();
}
    
void jag_editor::movetoposition() {
    long sc;
    if (emode())
        sc = size_upto(lines[poslines[currentline]], posinstring) + prefixe();
    else
        sc = size_upto(line, posinstring) + prefixego();
    cout << back;
    if (!sc)
        return;

	m_right[2] = localn999[sc][0];
	m_right[3] = localn999[sc][1];
	m_right[4] = localn999[sc][2];
	cout << m_right;
    m_right[2] = '0';
	m_right[3] = '0';
	m_right[4] = '1';
}

void jag_editor::movetobeginning() {
    long sc = prefixego();
    cout << back;
    if (!sc)
        return;

	m_right[2] = localn999[sc][0];
	m_right[3] = localn999[sc][1];
	m_right[4] = localn999[sc][2];
	cout << m_right;
	m_right[2] = '0';
	m_right[3] = '0';
	m_right[4] = '1';
}

void jag_editor::movetoend(bool remove) {
    long sc;
    if (emode()) {
        long p = poslines[currentline];
        sc = fullsize(lines[p]) + prefixe();
        if (remove && lines.eol(p))
            sc--;
    }
    else
        sc = fullsize(line) + prefixego();

    if (sc <= 0)
        return;

    cout << back;
	m_right[2] = localn999[sc][0];
	m_right[3] = localn999[sc][1];
	m_right[4] = localn999[sc][2];
	cout << m_right;
	m_right[2] = '0';
	m_right[3] = '0';
	m_right[4] = '1';
}

void jag_editor::movetolastline() {
	moveto_row_column(row_size + 2, 1);
}

void jag_editor::movetoline(long e) {
    if (e >= poslines.size()) {
        currentline = poslines.size()-1;
        e =  currentline;
    }
    //first line is 0
	moveto_row_column(e + 1, 1);
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
    if (drt == is_up) { // we are going up
        if (promptmode && currentline == initial_x)
            return false;
        currentline--;
        if (currentline < 0) {
            //we need to scroll down...
            currentline = 0;
            if (pos > 0) {
                //the current position in lines should not be the top line...
                //we check one position up...
                --pos;
                resetlist(poslines[0]-1);
				scrollingdown(row_size);
				line = lines[pos];
				movetoline(0);
				displaygo(true);
				if (posinstring > linesize())
                    posinstring = linesize();
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
				resetlist(poslines[0] + 1);
                pos = poslines.back();
				clearlastline();
				cout << m_scrollup;
				line = lines[pos];
				moveto_row_column(currentline+1, 1);
				displaygo(true);
                if (posinstring > linesize())
                    posinstring = linesize();
				return true;
            }
            return false;
        }
        pos = poslines[++currentline];
        exec = 2;
    }

    if (exec) {
        if (moveup) {
            if (drt == is_up)
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
#ifdef WIN32
	system("cls");
#else
    cout << m_clear << m_clear_scrolling << m_home;
#endif
}

void jag_editor::clearline() {
	cout << back << m_clear_line << back;
}


//We build a string with no composed emoji, to match the position of the cursor...
void jag_editor::cleanlongemoji(wstring& s, wstring& cleaned, long p) {
    long i = 0;
    if (!check_large_char(WSTR(line), line.size(), i)) {
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

    long mx = 1;
    if (selected_pos != -1) {
        pins = selected_x;
        mx = selected_y - selected_x;
    }

    if (last) {
        while (mx) {
            if (c_is_emojicomp(l.back())) {
                long sz = l.size() - 2;
                long i = sz;
                while (sz >= 0 && !scan_emoji(l, i)) {
                    sz--;
                    i = sz;
                }
                if (sz >= 0) {
                    pins -= i - sz;
                    l.erase(i, sz - i + 1);
                }
                else
                    l.pop_back();
            }
            mx--;
        }
        l.pop_back();
    }
    else {
        long nb = 0;
        long i = pins;
        long j;
        while (mx) {
            j = i;
            if (scan_emoji(l, j)) {
                nb += j - i;
                i = j;
            }
            else
                i++;
            nb++;
            mx--;
        }
        l.erase(pins, nb);
    }
    return pins;
}

void jag_editor::deletechar(bool left) {
    long sz = line.size();
    bool last = false;
    if (posinstring >= sz - 1) {
            //We need to know if we are in the middle of a large string across more than one line...
        if (!emode())
            last = true;
        else {
            if (currentline >= poslines.size() - 1 || poslines[currentline + 1] != pos)
                last = true;
        }
    }
    bool emoji = false;
    if (sizestring(line) != line.size())
        emoji = true;

    if (!emode() || (posinstring >= 0 && posinstring < sz)) {

        if (emode()) {
            line = lines[pos];
            undo(line, pos, u_modif);
        }
        else
            clearline();

        long pins = deleteachar(line, last, posinstring);

        if (option != x_none) {
            displaygo(false);
            return;
        }

        //We update our line
        if (emode()) {
            long dif = pins - posinstring;
            posinstring = pins;
            if (!dif)
                dif = 1;

            tobesaved = true;
            lines[pos] = line;

            long cl;
            long p;
            sz = lines.size();
            if (lines.Status(pos)) {
                if (lines.refactoring(pos)) {
                    if (promptmode) {
                        cl = currentline;
                        for (long p = pos; p < lines.size(); p++) {
                            clearline();
                            printline(p + 1, lines[p], -1);
                            movetoline(cl++);
                        }
                        if (sz > lines.size())
                            clearline();
                        if (pos >= lines.size()) {
                            currentline--;
                            pos--;
                            line  = lines[pos];
                            posinstring = line.size();
                        }
                        movetoline(currentline-1);
                        movetoposition();
                    }
                    else {
                        displaylist(poslines[0]);
                        movetoline(currentline);
                    }
                }
                else {
                    clearline();
                    p = pos + 1;
                    if (lines.Status(pos) == concat_line)
                        printline(-1, lines[pos], -1);
                    else
                        printline(p, lines[pos], -1);
                    cl = currentline + (1 - promptmode);
                    while (lines.Status(p) == concat_line) {
                        movetoline(cl++);
                        clearline();
                        printline(-1, lines[p], -1);
                        p++;
                    }
                    movetoline(currentline - promptmode);
                }
            }
            else {
                clearline();
                printline(pos+1, line, -1);
            }
        }
        else {
            posinstring = pins;
            printline(pos+1, line, -1);
        }

        movetoposition();
    }
}

void jag_editor::deleteline(char movingup) {

    if (lines.size() == 1 && lines[0].size() == 0) {
        if (!promptmode)
            init();
        return;
    }

    tobesaved = true;
    if (!movingup) {
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
        if (pos < lines.size())
            line = lines[pos];
        else
            line = L"";
        movetoline(currentline);
        posinstring = 0;
        movetobeginning();
        return;
    }

        //left top, cannot do anything

        //We need to kill the current line, however it can be merged with the previous one if moveup is true...
        //We are at position currentline...
        //if the line above already belong to the same line, we kill the last character of that line above
    wstring code;
    if (movingup == -1) { // the destruction was done with backspace
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

void jag_editor::clearlastline() {
	movetolastline();
	clearline();
}

void jag_editor::displayonlast(bool bck) {
    clearlastline();
    wstring w = st.str();
    string s = convert(w);
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
	clearlastline();
    string s = convert(w);
    cout << back << s;

    if (bck) {
        movetoline(currentline);
        if (currentposinstring != -1)
            posinstring = currentposinstring;
        currentposinstring = -1;
        if (posinstring > lines[poslines[currentline]].size()) {
            posinstring = lines[poslines[currentline]].size();
            movetoend(false);
        }
        else
            movetoposition();
    }
}

void jag_editor::displayonlast(string s, bool bck) {
    clearlastline();
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
			clearline();
            cout << back << "Line:" << convert(line);
            break;
        case x_find:
			clearline();
            if (regularexpressionfind == 2)
                cout << back << "Find(pgx):" << convert(line);
            else {
                if (regularexpressionfind == true)
                    cout << back << "Find(rgx):" << convert(line);
                else
                    cout << back << "Find:" << convert(line);
            }
            break;
        case x_replace:
			clearline();
            if (regularexpressionfind == 2)
                cout << back << "Find(pgx):" << convert(currentfind) << "  Replace:" << convert(line);
            else {
                if (regularexpressionfind == true)
                    cout << back << "Find(rgx):" << convert(currentfind) << "  Replace:" << convert(line);
                else
                    cout << back << "Find:" << convert(currentfind) << "  Replace:" << convert(line);
            }
            break;
        case x_write:
			clearline();
            cout << back << "File:" << coloringline(line);
            break;
        case x_count:
			clearline();
            cout << back << "Count:" << convert(line);
            break;
        case x_delete:
        case x_copy:
        case x_cut:
        case x_copying:
        case x_deleting:
        case x_cutting:
        case x_load:
			clearline();
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

void jag_editor::Scrolldown() {
	//From currentline down, pos is the new line number...
    long ps = currentline + 1;
	m_scrollmargin[2] = localn999[ps][0];
	m_scrollmargin[3] = localn999[ps][1];
	m_scrollmargin[4] = localn999[ps][2];
	cout << m_scrollmargin << m_scrolldown;
	m_scrollmargin[2] = '0';
	m_scrollmargin[3] = '0';
	m_scrollmargin[4] = '0';
	cout << m_scrollmargin;
	movetoline(currentline - 1);
	ps = pos;
	//We simply change the line numbers
	long i;
	if (poslines.size() < row_size)
		poslines.push_back(poslines.size());

	for (i = currentline - 1; i <= row_size && i < poslines.size(); i++) {
		printline(ps);
		cout << m_down;
		ps++;
	}
	clearline();
	movetoline(currentline);
}

void jag_editor::displaylist(long beg) {
	if (!lines.size()) {
		clearline();
		if (!noprefix)
			cout << back << m_dore << prefix << m_current << m_lightgray << std::setw(prefixsize) << "1> " << endl;
		return;
	}

	if (beg < 0)
		beg = 0;

    long nb = 0;
    x_option g = option;
    option = x_none;

    modified = false;
    lines.detectlongstrings(filetype);
    if (!lines.updatesize()) {
        if (poslines.size() && beg == poslines[0] && currentline && insertaline) {
            Scrolldown();
            return;
        }
    }

	poslines.clear();

    stringstream blk;
    for (long i = beg; i < lines.size(); i++) {
        poslines.push_back(i);
        string space(prefixe(), ' ');
        if (noprefix)
            blk << coloringline(lines[i], i) << endl;
        else {
            if (lines.status[i] == concat_line)
                blk << space << coloringline(lines[i], i) << endl;
            else {
                blk << m_dore << prefix << m_current << m_lightgray << std::setw(prefixsize) << lines.numeros[i] << "> " << m_current << coloringline(lines[i], i) << endl;
            }
        }
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
    cout << back << e_unknown_line;
    movetoline(currentline);
    movetoend();
}

bool jag_editor::resetsearch() {
#ifdef Tamgu_REGEX
    if (regularexpressionfind == 2) {
        try {
            if (wpattern == NULL) {
                wpatternexpression = currentfind;
                wpattern = new wregex(wpatternexpression);
            }
            else
                if (wpatternexpression != currentfind) {
                    delete wpattern;
                    wpatternexpression = currentfind;
                    wpattern = new wregex(wpatternexpression);
                }
        }
        catch(...) {
            return false;
        }
    }
    else
#endif
    if (regularexpressionfind == true) {
        if (findrgx == NULL)
            findrgx = new Jag_automaton(currentfind);
        else
            if (findrgx->regularexpression != currentfind) {
                delete findrgx;
                findrgx = new Jag_automaton(currentfind);
            }
        if (findrgx->first == NULL) {
            delete findrgx;
            findrgx = NULL;
            return false;
        }
    }
    return true;
}

bool jag_editor::search(wstring& l, long& first, long& last, long ps) {
    if (regularexpressionfind == true)
        return findrgx->search(l, first, last, ps);
#ifdef Tamgu_REGEX
    if (regularexpressionfind == 2) {
        wsmatch result;
        wstring w = l.substr(ps,l.size()-ps);
        if (regex_search(w, result, *wpattern)) {
            first = result.position() + ps;
            last = first + result.length();
            return true;
        }
        return false;
    }
#endif
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
    long ps = currentposinstring;
    currentfindpos = 0;
    if (currentfind !=  L"") {
        wstring l;
        if (!resetsearch()) {
            displayonlast("Wrong expression", true);
            return false;
        }

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
    cout << back << e_not_found;
    movetoline(currentline);
    movetoend();
    return false;
}

long jag_editor::processcount() {
    if (currentfind == L"")
        return 0;

    wstring code = lines.code();

    vector<long> alls;
    if (!resetsearch()) {
        displayonlast("Wrong expression");
        return false;
    }
    findrgx->searchall(code, alls, 0);
    return (alls.size() >> 1);
}

void jag_editor::processreplace() {
    string resp;
    option = x_none;
    movetoposition();
    if (!replaceall) {
        currentposinstring = posinstring;
        displayonlast(L"Replace: Y/N/A", true);
        resp = getch();
        if (resp == "A" || resp == "a")
            replaceall = true;
    }

#ifdef Tamgu_REGEX
    wstring w;
#endif

    if (replaceall || resp == "Y" || resp == "y") {
        //We know where the word is...
        long first, last, end;
        wstring ws = lines.getoneline(pos, end);
        if (search(ws, first, last, posinstring)) {
#ifdef Tamgu_REGEX
            if (regularexpressionfind == 2) {
                w = ws.substr(first,last-first);
                w = regex_replace(w, *wpattern, currentreplace);
                ws.replace(first, last-first, w);
            }
            else
#endif
                ws.replace(first, last-first, currentreplace);
            lines.replaceline(pos, end+1, ws);

            if (first > col_size) {
                long ps = first/col_size;
                pos += ps;
                ps *= col_size;
                first -= ps;
                last -= ps;
                resetlist(pos);
                posinstring = first;
                gotoline(pos);
            }
            else {
                posinstring = first;
                gotoline(poslines[0]);
            }

            if (!findnext())
                replaceall = false;
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
    long sz = line.size() - posinstring;
    //We cut our line at the right position.
    //sub is the right part of the string
    if (sz > 0)
        sub = line.substr(posinstring, sz);

    //line keeps the left part...
    line = line.substr(0, posinstring);

    lines[pos] = line;
    long sp = lines.indent(pos);
    if (sub == L")" || sub == L"}" || sub == L"]")
        sp -= GetBlankSize();

    if (sp > 0) {
        wstring space(sp, L' ');
        sub = space + sub;
    }
    else
        sp = 0;

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
        else {
            lines.status[pos] = solo_line;
            lines.numbers();
        }
    }

    if (currentline > row_size) {
        displaylist(poslines[0] + 1);
        currentline = row_size;
    }
	else {
		if (pos > 0 && stat == solo_line) {
			clearline();
			printline(pos, lines[pos - 1]);
			insertaline = true;
			displaylist(poslines[0]);
			printline(pos + 1, lines[pos]);
			insertaline = false;
		}
		else
			displaylist(poslines[0]);
	}


    movetoline(currentline);

    posinstring = sp;
    movetoposition();
    line = sub;
    return pos;
}

long jag_editor::handlingeditorline(bool computespace) {
    long i;


    long sz = lines.size();
    long szl = lines[pos].size();

    //if we are in the middle of a line...
    if (pos < sz) {
        if ((posinstring > 0 && posinstring < line.size()) || lines.status[pos] == concat_line)
            return handlemultiline();
    }
    else
        pos = sz;

    //We keep track of our line before splitting...
    undo(lines[pos],pos, u_modif); //modification

    long sp = 0;
    char spa = ' ';
    //we record the number of space on the current line...
    //which we will use as a seed to display the following line
    if (ispy()) {
        if (sz && computespace && szl) {
            long inc = GetBlankSize();
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
    if (sp < 0)
        sp = 0;

    wstring space(sp, spa);

        //we have different cases...
    if (!posinstring) {
        int prf = prefixsize;
        //beginning of the line, we insert before...
        lines.insert(pos, space); //we insert it into our line...
        undo(space, pos, u_ins_linked); //The value is inserted
        lines.numbers();
        if (currentline < row_size) {
			//We need to insert a line at this position
			insertaline = true;
            prefixsize = prf;
			displaylist(poslines[0]);
			insertaline = false;
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
    if (action == u_ins && sp) {
        posinstring = space.size();
        movetoend(false);
    }
    else {
        posinstring = 0;
        movetobeginning();
    }
    line = space;
    return pos;
}

long jag_editor::getbuffsize() {
    return _getbuffsize;
}

void jag_editor::clear() {
    mouseoff();
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
    if (emode())
        tobesaved = true;

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
    if (emode())
        tobesaved = true;
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
    scan_emoji(line, posinstring);
    posinstring++;
}

void jag_editor::backwardemoji() {
    posinstring--;
    if (posinstring < 0)
        return;

    long p = 0;
    for (long i = 0; i < line.size(); i++) {
        p = i;
        if (!scan_emoji(line, p)) {
            getachar(line, p);
            if (p >= posinstring) {
                posinstring = i;
                return;
            }
        }
        else {
            if (p >= posinstring) {
                posinstring = i;
                return;
            }
        }
        i = p;
    }
}


bool jag_editor::evaluateescape(string& buff) {

#ifdef WIN32
	if (buff == c_homekey) {
        pos = 0;
        posinstring = 0;
        currentline = 0;
        line = lines[pos];
        displaylist(0);
        movetoline(0);
        movetobeginning();
		return true;
	}

	if (buff == c_endkey) {
		long ps = lines.size() - row_size;
		if (ps > 0) {
            pos = ps;
            currentline = 0;
            posinstring = 0;
            line = lines[pos];
            displaylist(pos);
            movetoline(0);
            movetobeginning();
			return true;
		}
		return true;
	}

    if (buff == homekey) {
        movetobeginning();
        posinstring = 0;
        return true;
    }

    if (buff == endkey) {
		posinstring = line.size();
		if (posinstring && !lines.eol(pos))
			posinstring--;
		movetoposition();
		return true;
    }
#else
    if (buff == homekey) {
        pos = 0;
        posinstring = 0;
        currentline = 0;
        line = lines[pos];
        displaylist(0);
        movetoline(0);
        movetobeginning();
        return true;
    }

    if (buff == endkey) {
        long ps = lines.size() - row_size;
        if (ps > 0) {
            pos = ps;
            currentline = 0;
            posinstring = 0;
            line = lines[pos];
            displaylist(pos);
            movetoline(0);
            movetobeginning();
            return true;
        }
        return true;
    }

#endif
        //ctrl-up, up 10 lines
    if (buff == c_up) {
        if ((pos - row_size) < 0) {
            pos = 0;
            currentline = 0;
        }
        else
            pos -= row_size;
        posinstring = 0;
        line = lines[pos];
        displaylist(pos);
        movetoline(currentline);
        movetobeginning();
        return true;
    }

        //ctrl-down, down 10 lines
    if (buff == c_down) {
        long mxline = poslines.size() - 1;

        posinstring = 0;
        if ((pos + mxline) == lines.size()) {
            currentline = mxline;
            movetoline(currentline);
            movetobeginning();
            return true;
        }

        if ((pos + mxline) > lines.size()) {
            pos = lines.size()-mxline;
            if (pos < 0)
                pos = lines.size()-1;
        }
        else
            pos += mxline;

        line = lines[pos];
        displaylist(pos);
        movetoline(currentline);
        movetobeginning();
        return true;
    }

    if (buff == up || buff == down) {
#ifdef WIN32
		updown(buff[1], pos);
#else
		updown(buff[2], pos);
#endif
		return true;
    }

    if (buff == c_right || buff == a_right) {
        long ipos = posinstring;
        long sz = line.size();
        if (ipos < sz-1 && c_is_separator(line[ipos+1]))
            ipos += 2;
        bool fnd = false;
        while (ipos < sz) {
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

        return true;
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
        return true;
    }

    if (buff == right) {
        long mx = line.size();
        if (!lines.eol(pos))
            mx--;
        if (posinstring < mx) {
            forwardemoji();
            movetoposition();
        }
        else {//we go down at the beginning of the next line
            if (emode()) {
                updown(is_down, pos);
                posinstring = 0;
                movetobeginning();
            }
        }

        return true;
    }

    if (buff == (char*)alt_plus) {
        indentplus();
        return true;
    }

    if (buff == (char*)alt_minus) {
        deindentminus();
        return true;
    }

    if (buff == (char*)alt_v || buff == (char*)alt_vbis) {
        handleblock(copybuffer);
        return true;
    }

    if (buff == (char*)alt_x || buff == (char*)alt_xbis) {
        if (selected_pos == pos) {
            copybuffer = kbuffer;
            copy_to_clipboard(convert(copybuffer));
            kbuffer = L"";
            deleteselection();
            line = lines[pos];
        }
        return true;
    }

    if (buff == (char*)alt_c || buff == (char*)alt_cbis) {
        if (selected_pos == pos) {
            copybuffer = kbuffer;
            copy_to_clipboard(convert(copybuffer));
            kbuffer = L"";
            currentline += selected_posnext-selected_pos;
            pos = selected_posnext;
            movetoline(currentline);
            line = lines[pos];
            posinstring = selected_y;
            movetoposition();
        }
        return true;
    }

    if (buff == left) {
        if (posinstring > 0) {
            backwardemoji();
            movetoposition();
        }
        else {//we go up at the end of the previous line
            if (emode() && pos > 0) {
                updown(is_up, pos);
                posinstring = line.size();
                if (posinstring && !lines.eol(pos))
                    posinstring--;
                movetoposition();
            }
        }
        return true;
    }

    if (buff == del) {
        if (selected_pos != -1) {
            deleteselection();
            return true;
        }

        if (emode() && pos < lines.size()) {
            if (posinstring >= lines[pos].size()) {
                deleteline(1);
                return true;
            }
        }
        deletechar(false);
        return true;
    }
    return false;
}

void jag_editor::init() {
    promptmode = false;
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
        case 'b': //black mode
            switch_dark_mode();
            displaylist(poslines[0]);
            movetoline(currentline);
            movetoposition();
            return true;
        case '-': {
            deindentminus();
            displayonlast("", true);
            return true;
        }
        case '+': {//we indent all the lines up
            indentplus();
            displayonlast("", true);
            return true;
        }
        case 'C': // we count pattern
            displayonlast("Count:", false);
            line = currentfind;
            currentreplace = L"";
            posinstring = currentfind.size();
            option = x_count;
            return true;
        case 'l': //reload a file
            reloadfile();
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
        case 'm':
            if (emode()) {
                togglemouse();
                clearst();
                st << "mouse:";
                if (mouse_status)
                    st << "on";
                else
                    st << "off";
                displayonlast(true);
            }
            return true;
        case 'f':
            if (emode()) {
                regularexpressionfind = false;
                string sub = "Find:";
                sub += convert(currentfind);
                displayonlast(sub, false);
                line = currentfind;
                posinstring = currentfind.size();
                currentreplace = L"";
                option = x_find;
            }
            return true;
        case 'P':
            if (emode()) {
                regularexpressionfind = 2; //posix search
                currentfind = L"";
                displayonlast("Find(pgx):", false);
                line = currentfind;
                posinstring = currentfind.size();
                currentreplace = L"";
                option = x_find;
            }
            return true;
        case 'R':
            if (emode()) {
                regularexpressionfind = true;
                currentfind = L"";
                displayonlast("Find(rgx):", false);
                line = currentfind;
                posinstring = currentfind.size();
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
            return !terminate();
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
        case 'n':
            setnoprefix();
            displaylist(poslines[0]);
            movetoline(currentline);
            movetoposition();
            return true;
        case 'h': {
            bool m = mouse_status;
            if (m)
                mouseoff();
            displaythehelp();
            getch();
            displaylist(poslines[0]);
            if (m)
                mouseon();
            movetoline(currentline);
            movetoposition();
            return true;
        }
    }
    return false;
}

//This section handles combined commands introduced with Ctrl-x
void jag_editor::handlecommands() {
    currentposinstring = posinstring;
    string buff = getch();

    if (checkcommand(buff[0]))
        return;

    displayonlast("", true);
}

void jag_editor::resetterminal() {
    mouseoff();
#ifdef WIN32
	ResetWindowsConsole();
#else
	tcsetattr(0, TCSADRAIN, &oldterm);
#endif
}

bool jag_editor::terminate() {
    replaceall = false;

    if (tobesaved) {
        tobesaved = false;
        displayonlast("File not saved... ctrl-c again to quit", true);
        return false;
    }

    movetolastline();
    string space(colsize(), ' ');
    cout << back << space << back << m_redbold << "Salut!!!" << m_current << endl;

    fflush(stdout);
	resetterminal();
    exit(0);
    return true;
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

bool jag_editor::checkaction(string& buff, long& first, long& last, bool lisp) {
    wstring code;
    long i;
    switch (buff[0]) {
        case 1: //ctrl-a, moving to beginning of line
            movetobeginning();
            posinstring = 0;
            return true;
#ifdef WIN32
		case 3:
			return !terminate();
#endif
        case 4: //ctrl-d exiting
            if (emode())
                deleteline(0);
            return true;
        case 5://ctrl-e, moving to the end of the line...
            posinstring = line.size();
            if (posinstring && !lines.eol(pos))
                posinstring--;
            movetoposition();
            return true;
        case 6: // ctrl-f find
            if (emode()) {
                currentposinstring = posinstring;
                regularexpressionfind = false;
                string sub = "Find:";
                currentfind = L"";
                displayonlast(sub, false);
                line = currentfind;
                posinstring = currentfind.size();
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
#ifndef WIN32
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
#else
		case 13:
#endif
                 //we need to check the different options...
            switch (option) {
                case x_none: {
                    if (promptmode) {
                        option = x_exitprompt;
                        return true;
                    }
                    break;
                }
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
                    if (!emode()) {
                        option = x_none;
                        displayonlast("", true);
                    }
                    else {
                        displaylist(0);
                        movetoline(currentline);
                        movetobeginning();
                    }
                    return true;
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
            displaygo(true);
            return true;
        case 12: //ctrl-l: load a file
            if (emode()) {
                clearst();
                st << "load:";
                displayonlast(false);
                line = currentfind;
                currentreplace = L"";
                posinstring = currentfind.size();
                option = x_load;
            }
            return true;
        case 14:
            if (emode()) { //in edit mode looking for the next find
                findnext();
                line = lines[poslines[currentline]];
            }
            return true;
        case 16: //ctrl-p insert kbuffer
            modified = true;
            if (emode())
                tobesaved = true;
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
        case 17: //ctrl-q terminate
            return !terminate();
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
                indentcode(pos, lisp);
            return true;
        case 21: //undo ctrl-u
        case 26: //ctrl-z
            if (emode()) {
                processundos();
            }
            return true;
        case 19: //ctrl-s
            if (thecurrentfilename != "") {
                if (writetofile())
                    code = L"written to: ";
                else
                    code = L"cannot write to: ";
                code += wconvert(thecurrentfilename);
                displayonlast(code, true);
                return true;
            }
        case 23: //ctrl-w write the content in a file
            if (emode()) {
                line = wconvert(thecurrentfilename);
                code = L"File:";
                if (line.size())
                    code += line;

                displayonlast(code, false);
                posinstring = line.size();
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

            return evaluateescape(buff);
#ifdef WIN32
		case 8:
#else
#ifndef APPLE
		case 15:
#endif
		case 127: //back delete
#endif
            if (selected_pos != -1) {
                deleteselection();
                return true;
            }

			if (posinstring > 0) {
                backwardemoji();
                deletechar(true);
            }
            else {
                if (emode())
                    deleteline(-1);
            }
            return true;
        default:
            if (buff.size() > 1 && emode())
                return evaluateescape(buff);
    }

    return false;
}

static bool isitempty(wstring& w, wchar_t c) {
    char nb = false;
    for (long i = 0; i < w.size(); i++) {
        if (w[i] > 32) {
            if (w[i] != c || nb)
                return false;
            nb = true;
        }
    }
    return true;
}

void jag_editor::addabuffer(wstring& b, bool instring) {
    if (isMouseAction(b) || b == L"")
        return;

    //We only keep displayable characters
    wchar_t c;
    wstring code;
    for (long i = 0; i < b.size(); i++) {
        c = b[i];
        //We replace these characters with a blank
        if (c < 32 && c != 9 && c != 10 && c != 13)
            continue;
        code += c;
    }
    if (code == L"")
        return;
    
    b = code;

    if (emode())
        tobesaved = true;
    modified = true;
    //We are in the middle of a line... Insertion mode...
    if (line.size() && posinstring < line.size()) {
        if (emode())
            line = lines[poslines[currentline]];

        //We insert the character within our current line...
        code = line.substr(0, posinstring);
        code += b;
        code += line.substr(posinstring,line.size());

        if (promptmode) {
            pos = poslines[currentline];
            lines[pos] = code;
            lines.refactoring(pos);
            line = lines[pos];
            posinstring += b.size();
            long c = currentline;
            for (long p = pos; p < lines.size(); p++) {
                printline(p + 1, lines[p], -1);
                movetoline(c++);
            }
            movetoline(currentline-1);
            movetoposition();
            return;
        }
        else {
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
        }
        
        clearline();
        line = code;
        displaygo(true);
        if (b[0] == ')' || b[0] == '}' || b[0] == ']') {
            string ln = convert(line);
            long posmatch = computeparenthesis(ln, b[0], posinstring);
            if (posmatch != -1) {
                linematch = pos;
                lines[pos] = line;
                string res = ln.substr(0, posmatch);
                res += m_redbold;
                res += ln[posmatch];
                res += m_current;
                res += ln.substr(posmatch+1,ln.size());
                printline(pos+1, res);
            }
        }
        posinstring += b.size();
        movetoposition();
        return;
    }


    //We extend our line...
    line += b;

    bool fndchr =  false;
    
    if (!promptmode) {
        if (b[0]=='"' || b[0]=='\'')
            instring = 1 - instring;
        
        if (b[0] == ')' || b[0] == '}' || b[0] == ']') {
            fndchr = true;
            if (!instring && emode() && isitempty(line, b[0])) {
                long sp = lines.indent(pos) - GetBlankSize();
                if (sp > 0) {
                    wstring space(sp, ' ');
                    line = space;
                    posinstring = sp;
                    line += b;
                }
                else {
                    line = b;
                    posinstring = 0;
                }
                printline(pos, line, -1);
            }
            else {
                string ln = convert(line);
                long posmatch = computeparenthesis(ln, b[0], ln.size()-1);
                
                if (posmatch != -1) {
                    if (emode()) {
                        linematch = pos;
                        lines[pos] = line;
                    }
                    else
                        linematch = -2;
                    string res = ln.substr(0, posmatch);
                    res += m_redbold;
                    res += ln[posmatch];
                    res += m_current;
                    res += ln.substr(posmatch+1,ln.size());
                    wstring lsave = line;
                    line = L"";
                    s_utf8_to_unicode(line, USTR(res), res.size());
                    displaygo(true);
                    line = lsave;
                    posinstring += b.size();
                    movetoposition();
                    return;
                }
            }
        }
    }
    
    posinstring += b.size();
    if (promptmode) {
        lines[pos] = line;
        lines.refactoring(pos);
        promptmode = false;
        long sz = fullsize(line) + 1;
        promptmode = true;
        if (sz >= col_size) {
            line = line.substr(0, line.size()-1);
            lines[pos] = line;
            line = b;
            currentline++;
            pos++;
            posinstring = 1;
            lines[pos] = line;
            if (currentline >= row_size) {
                cout << endl;
                currentline--;
                for (long i = 0; i < poslines.size();i++)
                    poslines[i]++;
                printline(pos+1, line, -1);
            }
            else {
                poslines.push_back(pos);
                moveto_row_column(currentline, 1);
                printline(pos+1, line, -1);
                movetoposition();
            }
            return;
        }
    }
    else {
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
    }

    if (fndchr) {
        clearline();
        displaygo(true);
        movetoposition();
    }
    else {
        if (option == x_none) {
            printline(pos+1, line, -1);
            movetoposition();
        }
        else
            cout << convert(b);
    }
}

wstring jag_editor::kbget(string prf) {
    
    getcursorxy(initial_x, initial_y);
    prefix = prf;
    wprefix = wconvert(prf);
    prefixsize = (int)size_of_prefix();
    line = L"";
    noprefix = true;
    promptmode = true;
    currentline = initial_x;

    pos = 0;
    lines.clear();
    poslines.clear();
    for (pos = 0; pos <= currentline; pos++)
        poslines.push_back(0);
    
    pos = 0;
    lines.push(L"");

    long current_pos = pos;
    
    posinstring = 0;
    kbuffer = L"";
    printline(pos);

    option = x_none;
    
    string buffer;
    wstring bl;
    wstring b;
    
    wstring code;
    bool inbuffer = false;
    
    bool instring = false;
    long first = 0, last;

    string buff = "";
    double_click = 0;
    selected_x = -1;
    selected_y = -1;
    selected_pos = -1;

    
    while (1) {
        if (option == x_exitprompt) {
            cout << endl;
            code = L"";
            while (current_pos < lines.size()) {
                code += lines[current_pos++];
            }
            currentline = initial_x + lines.size();
            movetoline(currentline);
            return code;
        }

        buff = getch();
        
        if (checkaction(buff, first, last)) {
            double_click = 0;
            if (buff[0] != 24) {
                selected_x = -1;
                selected_y = -1;
                selected_pos = -1;
            }
            continue;
        }
        
        if (inbuffer) {
            buffer += buff;
            buff = buffer;
            inbuffer = false;
        }
        
        if (buff.size() == _getbuffsize)
            inbuffer = check_utf8(buff, buffer);
        
        bl = wconvert(buff);
        cleanheaders(bl);
        
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
    return L"";
}
    
    
//This is the main method that launches the terminal
void jag_editor::launchterminal(char loadedcode, vector<string>& newcolors, bool no_numbering) {

    if (newcolors.size())
        colors = newcolors;

    localhelp << m_red << "^c/q" << m_current << ":exit";

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

    wstring code;
    string buffer;
    bool inbuffer = false;

    bool instring = false;
    string buff;
    long first = 0, last;

    if (no_numbering)
        noprefix = true;
    
    while (1) {
        buff = getch();

        if (echochar) {
            displaychar(buff);
            continue;
        }

        if (emode()) {
            while (isMouseAction(buff)) {
                handlemousectrl(buff);
                buff = getch();
            }
        }

        if (selected_pos != -1 && buff[0] != 24)
            unselectlines(selected_pos, selected_posnext, selected_x, selected_y);

        if (checkaction(buff, first, last)) {
            double_click = 0;
            if (buff[0] != 24) {
                selected_x = -1;
                selected_y = -1;
                selected_pos = -1;
            }
            continue;
        }

        if (option == x_exitprint)
            return;

        if (selected_pos == pos) {
            //We are going to replace a sequence of characters
            //we delete it first
            deleteselection();
        }

        double_click = 0;
        selected_x = -1;
        selected_y = -1;
        selected_pos = -1;

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

void editor_lines::setcode(wstring& code, bool clean) {
    if (code == L"")
        return;

    vector<wstring> buff;
    status.clear();
    lines.clear();
    numeros.clear();
    longlines.clear();

    jag->vsplit(code, L"\n", buff);
    jag->setprefixesize(buff.size());
    if (!buff.size())
        return;

    long u;
    vector<wstring> subs;

    if (clean) {
        while (buff.size() && buff.back() == L"") {
            buff.pop_back();
        }

        buff.push_back(L"");
    }

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

bool editor_lines::refactoring(long pos) {
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

    if (baseline == L"") {
        return false;
    }

    //We then clean all these lines...

    editor_lines sublines(jag);
    sublines.setcode(baseline, false);
    //same number of lines
    long i;
    for (i = 0; i < sublines.size(); i++) {
        if (first != pos && first < lines.size())
            jag->undo(lines[first], first, u_modif_linked);
        if (first < p) {
            lines[first] = sublines[i];
            status[first] = sublines.status[i];
        }
        else
            insert(first, sublines[i], sublines.status[i]);
        first++;
    }

    bool delete_line = false;
    while (first < p) {
        delete_line = true;
        jag->undo(lines[first], first, u_del_linked);
        erase(first);
        p--;
    }

    numbers();
    return delete_line;
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

bool editor_lines::updatesize() {
    long prf = jag->prefixsize;
    jag->setprefixesize(lines.size());
    if (jag->prefixsize != prf)
        return true;
    return false;
}

///------------------------------------------------------------------------------------
long jag_editor::splitline(wstring& l, long linenumber, vector<wstring>& subs) {
    //we compute the position of each segment of l on string...

    long sz = prefixe();

    if ((l.size() + sz) < (col_size / 2)) {
        subs.push_back(l);
        return 1;
    }

    wstring code;
    TAMGUCHAR c;

    for (long i = 0; i < l.size(); i++) {
        if (store_emoji(l, code, i))
            sz++;
        else {
            c = getachar(l, i);
            concat_char_check_utf16(code, c);
            if (c == 9) {//tab position
                sz += (8 - (sz%8))%8;
                sz--;
            }
            else {
                if (ckjchar(c)) //double space on screen
                    sz++;
            }
        }
        sz++;
        if (sz >= col_size) {
            subs.push_back(code);
            code = L"";
            sz = prefixe();
        }
    }

    if (code != L"" || !subs.size())
        subs.push_back(code);

    return subs.size();
}

