#include "codeparse.h"


static const char* errorlabels[]={"","Wrong index format",
    "Wrong expression after an operator",
    "Wrong list of parameters",
    "Wrong expressions in parentheses",
    "Wrong FRAME declaration (check the name)",
    "Wrong argument declaration",
    "Wrong FUNCTION declaration",
    "Wrong BOOLEAN EXPRESSION",
    "Wrong BOOLEAN expression",
    "Wrong IF declaration",
    "Wrong WHILE declaration",
    "Wrong FOR declaration",
    "Wrong FUNCTION CALL",
    "Wrong Expression",
    "Wrong Instruction",
    "Wrong SWITCH",
    "Wrong test in a SWITCH",
    "Wrong IN expression",
    "Wrong WITH expression",
    "Wrong PREDICATE declaration",
    "Wrong PATTERN in A RULE",
    "Wrong TRY-CATCH expression",
    "Wrong INTERVAL",
    "Wrong ELSE section",
    "Wrong VECTOR definition",
    "Wrong MAP definition",
    "Wrong Function call in an index reference",
    "Wrong comments",
    "Wrong dotted expression",
    "Wrong expression or Missing ';'",
    "Missing '}'",
    "Wrong declaration",
    "Wrong FOR IN declaration",
    "Wrong PREDICATE definition",
    "Wrong PREDICATE expression",
    "Wrong PREDICATE parameter list",
    "Wrong PREDICATE term",
    "Wrong tuple",
    "Wrong ASSERT definition",
    "Wrong PREDICATE VARIABLE",
    "Wrong RANGE definition",
    "Wrong PARENTHETIC expression",
    "Wrong TAG expression",
    "Wrong DCG declaration",
    "Wrong DCG or Missing '.'",
    "Wrong Predicate Rule or Missing '.'",
    "Wrong LAMBDA use",
    "Wrong FUNCTIONAL description",
    "Wrong FILTER",
    "Wrong FOLD or SCAN",
    "Wrong MAP",
    "Wrong CYCLE or REPEAT",
    "Wrong REPLICATE",
    "Wrong taskell LAMBDA",
    "Wrong taskell CASE",
    "Wrong taskell Boolean Use",
    "Wrong FLIP",
    "Wrong EXTENSION implementation",
    "Expecting a meta-character",
    "Expecting a '.'",
    "Wrong tuple definition",
    "Wrong 'IF' expression",
    "Wrong 'data' expression",
    "Wrong 'ontology' expression",
    "Wrong 'range' definition",
    "Wrong annotation rule",
    "Wrong lisp expression",
    "Wrong shape access",
    "Wrong alias declaration",
    "Wrong 'iferror' declaration",
    "Wrong 'frame vector' declaration"
};

static char tab0[] = {'&','|','+','-','*','/','%','^',0};
static char tab1[] = {'<','>',0};
static char tab2[] = {'!','~',0};
static char tab3[] = {':','=',0};
static char tab4[] = {',',';',0};
static char tab5[] = {'|',':',0};
static char tab6[] = {'|',':',0};
static char tab7[] = {':','=',0};
static char tab8[] = {',',';',0};
static char tab9[] = {'&','+','-','*','/','%','^',0};
static char tab10[] = {',',';',0};
static char tab11[] = {',',';',0};
static char tab12[] = {'d','p','c','C','H','a','r','s','S','?','%',0};
static char tab13[] = {'*','+',0};
static char tab14[] = {'+','*',0};
static char tab15[] = {'@','~','#',0};
static const char* varstring0="IF";
static const char* varstring1="If";
static const char* varstring2="alias";
static const char* varstring3="and";
static const char* varstring4="autorun";
static const char* varstring5="break";
static const char* varstring6="continue";
static const char* varstring7="deriving";
static const char* varstring8="do";
static const char* varstring9="elif";
static const char* varstring10="else";
static const char* varstring11="for";
static const char* varstring12="frame";
static const char* varstring13="function";
static const char* varstring14="if";
static const char* varstring15="in";
static const char* varstring16="not";
static const char* varstring17="or";
static const char* varstring18="otherwise";
static const char* varstring19="polynomial";
static const char* varstring20="return";
static const char* varstring21="switch";
static const char* varstring22="thread";
static const char* varstring23="where";
static const char* varstring24="while";
static const char* varstring25="xor";
static const char* varstring26="iferror";
static const char* varstring27="ifnot";
static const char* varstring28="∧";
static const char* varstring29="∨";
static const char* varstring30="×";
static const char* varstring31="÷";
static const char* varstring32="is";
static const char* varstring33="≠";
static const char* varstring34="private";
static const char* varstring35="common";
static const char* varstring36="const";
static const char* varstring37="with";
static const char* varstring38="vector";
static const char* varstring39="map";
static const char* varstring40="treemap";
static const char* varstring41="primemap";
static const char* varstring42="mapi";
static const char* varstring43="treemapi";
static const char* varstring44="mapf";
static const char* varstring45="extension";
static const char* varstring46="...";
static const char* varstring47="strict";
static const char* varstring48="joined";
static const char* varstring49="protected";
static const char* varstring50="exclusive";
static const char* varstring51="²";
static const char* varstring52="³";
static const char* varstring53="notin";
static const char* varstring54="¬";
static const char* varstring55="..";
static const char* varstring56="null";
static const char* varstring57="fail";
static const char* varstring58="stop";
static const char* varstring59="retract";
static const char* varstring60="asserta";
static const char* varstring61="assertz";
static const char* varstring62="assert";
static const char* varstring63="findall";
static const char* varstring64="static";
static const char* varstring65="←";
static const char* varstring66="Maybe";
static const char* varstring67="→";
static const char* varstring68="foldl";
static const char* varstring69="foldr";
static const char* varstring70="scanl";
static const char* varstring71="scanr";
static const char* varstring72="all";
static const char* varstring73="any";
static const char* varstring74="foldl1";
static const char* varstring75="foldr1";
static const char* varstring76="scanl1";
static const char* varstring77="scanr1";
static const char* varstring78="takeWhile";
static const char* varstring79="filter";
static const char* varstring80="dropWhile";
static const char* varstring81="take";
static const char* varstring82="drop";
static const char* varstring83="zipWith";
static const char* varstring84="zip";
static const char* varstring85="repeat";
static const char* varstring86="cycle";
static const char* varstring87="replicate";
static const char* varstring88="flip";
static const char* varstring89="case";
static const char* varstring90="of";
static const char* varstring91="let";
static const char* varstring92="data";
static const char* varstring93="ontology";
static const char* varstring94="concept";
static const char* varstring95="role";
static const char* varstring96="property";
static const char* varstring97="true";
static const char* varstring98="false";
static const char* varstring99="namespace";
static const char* varstring100="try";
static const char* varstring101="catch";
static const char* varstring102="maybe";
static const char* varstring103="default";


char bnf_tamgu::m_path_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_path(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_path(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="path";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'/')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_path_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_shell_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_path(lret,&subtree) || m_word(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_shell(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'#')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'!')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_shell_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_pop_node(tree,addsubtree);
    return(1);
}


char bnf_tamgu::m_signature(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,239)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,187)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,191)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_pop_node(tree,addsubtree);
    return(1);
}


char bnf_tamgu::m_notreserved(string& lreturn,x_node** tree) {
    static std::set<string> keywords;
    static bool init = true;
    if (init) {
        init = false;
        keywords.insert(varstring0);
        keywords.insert(varstring1);
        keywords.insert(varstring2);
        keywords.insert(varstring3);
        keywords.insert(varstring4);
        keywords.insert(varstring5);
        keywords.insert(varstring6);
        keywords.insert(varstring7);
        keywords.insert(varstring8);
        keywords.insert(varstring9);
        keywords.insert(varstring10);
        keywords.insert(varstring11);
        keywords.insert(varstring12);
        keywords.insert(varstring13);
        keywords.insert(varstring14);
        keywords.insert(varstring15);
        keywords.insert(varstring16);
        keywords.insert(varstring17);
        keywords.insert(varstring18);
        keywords.insert(varstring19);
        keywords.insert(varstring20);
        keywords.insert(varstring21);
        keywords.insert(varstring22);
        keywords.insert(varstring23);
        keywords.insert(varstring24);
        keywords.insert(varstring25);
        keywords.insert(varstring26);
        keywords.insert(varstring27);
    }

    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (keywords.find(fx->stack[currentpos]) != keywords.end()) {
        lret+=fx->stack[currentpos];
        incrementpos();
        x_init_tree(tree,subtree,addsubtree);
    }
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}

char bnf_tamgu::m_operator_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'&') || !x_test_char(lret,'&') || !x_test_char(lret,'&')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operator_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'|') || !x_test_char(lret,'|') || !x_test_char(lret,'|')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operator_0_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,':') || !x_test_char(lret,':')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operator_0_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'<') || !x_test_char(lret,'<')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operator_0_5(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'>') || !x_test_char(lret,'>')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operator_0_6(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'^') || !x_test_char(lret,'^')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operator_0_7(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'%') || !x_test_char(lret,'%')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operator_0_8(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'*') || !x_test_char(lret,'*')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operator(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="operator";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;

    if (currentpos>=fx->stack.size())
        return 0;
    unsigned char xu = fx->stack[currentpos][intoken];
    bool found = false;
    switch (xu) {
        case '&':
            if (m_operator_0_1(lret,&subtree) || x_test_in(lret,tab0)) 
                found=true;
            break;
        case '|':
            if (m_operator_0_2(lret,&subtree) || x_test_in(lret,tab0)) 
                found=true;
            break;
        case ':':
            if (m_operator_0_3(lret,&subtree)) 
                found=true;
            break;
        case '<':
            if (m_operator_0_4(lret,&subtree)) 
                found=true;
            break;
        case '>':
            if (m_operator_0_5(lret,&subtree)) 
                found=true;
            break;
        case '^':
            if (m_operator_0_6(lret,&subtree) || x_test_in(lret,tab0)) 
                found=true;
            break;
        case '%':
            if (m_operator_0_7(lret,&subtree) || x_test_in(lret,tab0)) 
                found=true;
            break;
        case '*':
            if (m_operator_0_8(lret,&subtree) || x_test_in(lret,tab0)) 
                found=true;
            break;
        case '+':
            if (x_test_in(lret,tab0)) 
                found=true;
            break;
        case '-':
            if (x_test_in(lret,tab0)) 
                found=true;
            break;
        case '/':
            if (x_test_in(lret,tab0)) 
                found=true;
            break;
        default:
            if (x_test_string(lret,varstring27) || x_test_string(lret,varstring28) || x_test_string(lret,varstring29) || x_test_string(lret,varstring30) || x_test_string(lret,varstring31)) 
                found=true;
    }
    if (found)
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}

char bnf_tamgu::m_operatoraffectation_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring30)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'=')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operatoraffectation_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring31)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'=')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operatoraffectation_0_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'&') || !x_test_char(lret,'&') || !x_test_char(lret,'&') || !x_test_char(lret,'=')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operatoraffectation_0_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'|') || !x_test_char(lret,'|') || !x_test_char(lret,'|') || !x_test_char(lret,'=')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operatoraffectation_0_5(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,':') || !x_test_char(lret,':') || !x_test_char(lret,'=')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operatoraffectation_0_6(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,':') || !x_test_char(lret,'=')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operatoraffectation_0_7_8(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (x_test_string(lret,varstring16))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operatoraffectation_0_7(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring32)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_operatoraffectation_0_7_8(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operatoraffectation_0_9(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'<') || !x_test_char(lret,'<') || !x_test_char(lret,'=')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operatoraffectation_0_10(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'>') || !x_test_char(lret,'>') || !x_test_char(lret,'=')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operatoraffectation_0_11(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'+') || !x_test_char(lret,'=')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operatoraffectation_0_12(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'-') || !x_test_char(lret,'=')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operatoraffectation_0_13(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'*') || !x_test_char(lret,'*') || !x_test_char(lret,'=')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operatoraffectation_0_14(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'%') || !x_test_char(lret,'%') || !x_test_char(lret,'=')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operatoraffectation_0_15(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'/') || !x_test_char(lret,'=')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operatoraffectation_0_16(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'%') || !x_test_char(lret,'=')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operatoraffectation_0_17(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'&') || !x_test_char(lret,'=')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operatoraffectation_0_18(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'|') || !x_test_char(lret,'=')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operatoraffectation_0_19(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'^') || !x_test_char(lret,'^') || !x_test_char(lret,'=')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operatoraffectation_0_20(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'*') || !x_test_char(lret,'=')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operatoraffectation_0_21(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'^') || !x_test_char(lret,'=')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operatoraffectation_0_22(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'<') || !x_test_char(lret,'<') || !x_test_char(lret,'<')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_operatoraffectation(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="operatoraffectation";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;

    if (currentpos>=fx->stack.size())
        return 0;
    unsigned char xu = fx->stack[currentpos][intoken];
    bool found = false;
    switch (xu) {
        case '&':
            if (m_operatoraffectation_0_3(lret,&subtree) || m_operatoraffectation_0_17(lret,&subtree)) 
                found=true;
            break;
        case '|':
            if (m_operatoraffectation_0_4(lret,&subtree) || m_operatoraffectation_0_18(lret,&subtree)) 
                found=true;
            break;
        case ':':
            if (m_operatoraffectation_0_5(lret,&subtree) || m_operatoraffectation_0_6(lret,&subtree)) 
                found=true;
            break;
        case '<':
            if (m_operatoraffectation_0_9(lret,&subtree) || m_operatoraffectation_0_22(lret,&subtree)) 
                found=true;
            break;
        case '>':
            if (m_operatoraffectation_0_10(lret,&subtree)) 
                found=true;
            break;
        case '+':
            if (m_operatoraffectation_0_11(lret,&subtree)) 
                found=true;
            break;
        case '-':
            if (m_operatoraffectation_0_12(lret,&subtree)) 
                found=true;
            break;
        case '*':
            if (m_operatoraffectation_0_13(lret,&subtree) || m_operatoraffectation_0_20(lret,&subtree)) 
                found=true;
            break;
        case '%':
            if (m_operatoraffectation_0_14(lret,&subtree) || m_operatoraffectation_0_16(lret,&subtree)) 
                found=true;
            break;
        case '/':
            if (m_operatoraffectation_0_15(lret,&subtree)) 
                found=true;
            break;
        case '^':
            if (m_operatoraffectation_0_19(lret,&subtree) || m_operatoraffectation_0_21(lret,&subtree)) 
                found=true;
            break;
        case '=':
            if (x_test_char(lret,'=')) 
                found=true;
            break;
        default:
            if (m_operatoraffectation_0_1(lret,&subtree) || m_operatoraffectation_0_2(lret,&subtree) || m_operatoraffectation_0_7(lret,&subtree)) 
                found=true;
    }
    if (found)
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}

char bnf_tamgu::m_orand_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'|') || !x_test_char(lret,'|')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_orand_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'&') || !x_test_char(lret,'&')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_orand(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="orand";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_orand_0_1(lret,&subtree) || m_orand_0_2(lret,&subtree) || x_test_string(lret,varstring17) || x_test_string(lret,varstring3) || x_test_string(lret,varstring25))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_comparator_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'<') || !x_test_char(lret,'>')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_comparator_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'~') || !x_test_char(lret,'=')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_comparator_0_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'!') || !x_test_char(lret,'=')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_comparator_0_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'=') || !x_test_char(lret,'=')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_comparator_0_5(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'<') || !x_test_char(lret,'=')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_comparator_0_6(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'>') || !x_test_char(lret,'=')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_comparator(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="comparator";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;

    if (currentpos>=fx->stack.size())
        return 0;
    unsigned char xu = fx->stack[currentpos][intoken];
    bool found = false;
    switch (xu) {
        case '<':
            if (m_comparator_0_1(lret,&subtree) || m_comparator_0_5(lret,&subtree) || x_test_in(lret,tab1)) 
                found=true;
            break;
        case '~':
            if (m_comparator_0_2(lret,&subtree)) 
                found=true;
            break;
        case 33:
            if (m_comparator_0_3(lret,&subtree)) 
                found=true;
            break;
        case '=':
            if (m_comparator_0_4(lret,&subtree)) 
                found=true;
            break;
        case '>':
            if (m_comparator_0_6(lret,&subtree) || x_test_in(lret,tab1)) 
                found=true;
            break;
        default:
            if (x_test_string(lret,varstring33)) 
                found=true;
    }
    if (found)
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}

char bnf_tamgu::m_fulloperation(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="fulloperation";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_operator(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_operation(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="operation";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_operator(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_nocomparisonexpressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_multiply_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_power(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_multiply_0_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_operation(lret,&subtree) || m_operationin(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_multiply_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_multiply_0_2_3(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_multiply(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="multiply";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_variable(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_multiply_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_multiply_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_comparison(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="comparison";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_comparator(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_nocomparisonexpressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_private(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="private";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_string(lret,varstring34))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_common(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="common";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_string(lret,varstring35))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_const(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="const";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_string(lret,varstring36))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_instance_variable_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,':') || !x_test_char(lret,'-')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_instance_variable_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_char(lret,'_') || m_instance_variable_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_instance_variable(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="instance";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'?')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_instance_variable_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_feature_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_common(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_feature_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_private(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_feature_0_1_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_feature_0_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_common(lret,&subtree) || m_const(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_feature(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="feature";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_feature_0_1(lret,&subtree) || m_feature_0_3(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_type_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_feature(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_type_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_instance_variable(lret,&subtree) || m_typename(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_type(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="type";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_type_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYFAIL
    subtree=NULL;
    if (m_notreserved(lret,&subtree)) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_type_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_depend(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="depend";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring37)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,19);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_declaration_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_parameters(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_declaration_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_declaration_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_declaration_0_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_depend(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_declaration_0_4_5(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'=')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_wnexpressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_declaration_0_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_declaration_0_4_5(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_declaration(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="declaration";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_type(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_declaration_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_declaration_0_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_declaration_0_4(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_declarationlist_0_4_5(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="declarationlist";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'=')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,32);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_wnexpressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_declarationlist_0_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_declarationlist_0_4_5(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_declarationlist_0_6_7(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="declarationlist";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,32);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_declarationlist(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_declarationlist_0_6(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_declarationlist_0_6_7(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_declarationlist(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="declarationlist";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_declaration_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_declaration_0_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_declarationlist_0_4(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_declarationlist_0_6(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_multideclaration_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_parameters(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_multideclaration_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_multideclaration_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_multideclaration_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_multideclaration_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_multideclaration(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="multideclaration";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_type(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_multideclaration_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_declaration_0_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_declarationlist_0_4(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_declarationlist_0_6(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_container(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="container";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_string(lret,varstring38) || x_test_string(lret,varstring39) || x_test_string(lret,varstring40) || x_test_string(lret,varstring41) || x_test_string(lret,varstring42) || x_test_string(lret,varstring43) || x_test_string(lret,varstring44))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_framecontainer_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_feature(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_framecontainer(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="framecontainer";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_framecontainer_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_container(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'<')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'>')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,71);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_declaration_0_4(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_declarationending(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="declarationending";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_char(lret,';'))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_frame_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_private(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_frame_0_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_declarationending(lret,&subtree) || m_instruction(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_frame(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="frame";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_frame_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring12)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,5);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_typename(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_declaration_0_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_frame_0_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_extension(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="extension";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring45)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,58);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_typename(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_instruction(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_instruction_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_blocs(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_instruction(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="instruction";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'{')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,15);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_instruction_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'}')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_nonlimited(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="nonlimited";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_string(lret,varstring46))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_arguments_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_nonlimited(lret,&subtree) || m_arguments(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_arguments_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="arguments";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,6);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_arguments_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_arguments_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_arguments_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_arguments(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="arguments";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_declaration(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_arguments_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_strict(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="strict";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_string(lret,varstring47))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_join(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="join";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_string(lret,varstring48))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_protecclusive(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="protecclusive";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_string(lret,varstring49) || x_test_string(lret,varstring50))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_functionlabel(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="functionlabel";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_string(lret,varstring19) || x_test_string(lret,varstring13) || x_test_string(lret,varstring4) || x_test_string(lret,varstring22))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_typefunction_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_join(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_typefunction_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_protecclusive(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_typefunction_0_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_private(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_typefunction_0_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_strict(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_typefunction(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="typefunction";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_typefunction_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_typefunction_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_typefunction_0_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_typefunction_0_4(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_functionlabel(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_indexname(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="indexname";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'[') || !x_test_char(lret,']')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_intervalname(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="intervalname";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'[') || !x_test_char(lret,':') || !x_test_char(lret,']')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_returntype(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="returntype";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,':')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,':')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_space(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="space";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'@')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_function_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_word(lret,&subtree) || m_plusplus(lret,&subtree) || m_operator(lret,&subtree) || m_comparator(lret,&subtree) || m_indexname(lret,&subtree) || m_intervalname(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_function_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_arguments(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_function_0_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_returntype(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_function(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="function";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_typefunction(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,7);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_function_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_function_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_function_0_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_frame_0_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_subfunc(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="subfunc";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_dot(lret)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,27);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_regularcall(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_subfuncbis(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="subfuncbis";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_dot(lret)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_regularcall(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_framevariable(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="framevariable";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_dot(lret)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,29);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_variable(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_framesep(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="framesep";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (m_operator_0_3(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_frameup(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="frameup";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_framesep(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_functioncall_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_frameup(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_functioncall(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="functioncall";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_functioncall_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYFAIL
    subtree=NULL;
    if (m_notreserved(lret,&subtree)) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_regularcall_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_comparing(lret,&subtree) || m_parameters(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_regularcall_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_regularcall_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_regularcall_0_3_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_shapeindexes(lret,&subtree) || m_interval(lret,&subtree) || m_indexes(lret,&subtree) || m_subfuncbis(lret,&subtree) || m_framevariable(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_regularcall_0_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_regularcall_0_3_4(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_regularcall(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="regularcall";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_functioncall(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,13);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_regularcall_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_regularcall_0_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_purecall_0_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_shapeindexes(lret,&subtree) || m_interval(lret,&subtree) || m_indexes(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_purecall_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_purecall_0_2_3(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_purecall(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="purecall";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_functioncall(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_multideclaration_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_purecall_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_returncall_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_comparing(lret,&subtree) || m_nocomparisonexpressions(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_returncall_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_returncall_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_returncall(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="returncall";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring20)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_returncall_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_breakcall(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="breakcall";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_string(lret,varstring5))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_continuecall(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="continuecall";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_string(lret,varstring6))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_call(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="call";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_breakcall(lret,&subtree) || m_continuecall(lret,&subtree) || m_regularcall(lret,&subtree) || m_returncall(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_sep(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="sep";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_char(lret,':'))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_parameters_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="parameters";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,3);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_nocomparisonexpressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_parameters_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYWHILE
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_parameters_0_1_2(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    return(1);
}


char bnf_tamgu::m_parameters(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="parameters";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_nocomparisonexpressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_parameters_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_aliasdeclaration_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_aliasdeclaration_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //CONSTRAINT
    int foundsolution=0;
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (x_test_char(lret,')')) {
            foundsolution=1;
            x_init_tree(tree,subtree,addsubtree);
            break;
        }
        subtree=NULL;
        if (m_aliasdeclaration_0_1_2(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    if (foundsolution==0) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_aliasdeclaration_0_3_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_wnexpressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,';')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_aliasdeclaration_0_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_instruction(lret,&subtree) || m_aliasdeclaration_0_3_4(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_aliasdeclaration(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="aliasdeclaration";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring2)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_aliasdeclaration_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'=')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,69);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_aliasdeclaration_0_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_blocs_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_extension(lret,&subtree) || m_frame(lret,&subtree) || m_complexbloc(lret,&subtree) || m_function(lret,&subtree) || m_aliasdeclaration(lret,&subtree) || m_bloc(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_blocs(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="blocs";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYWHILE
    subtree=NULL;
    if (m_blocs_0_1(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_blocs_0_1(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_bloc_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="bloc";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_dcg(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,45);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_dot(lret)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_bloc_0_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_dependencyrule(lret,&subtree) || m_predicatedefinition(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_bloc_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="bloc";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_bloc_0_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,46);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_dot(lret)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_bloc(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="bloc";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_annotationrule(lret,&subtree) || m_predicatefact(lret,&subtree) || m_bloc_0_1(lret,&subtree) || m_bloc_0_2(lret,&subtree) || m_sousbloc(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_sousbloc_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_tamgulisp(lret,&subtree) || m_hdata(lret,&subtree) || m_hontology(lret,&subtree) || m_telque(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_sousbloc_0_1_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (x_test_char(lret,';'))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_sousbloc_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_sousbloc_0_1_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_sousbloc_0_1_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_sousbloc_0_4_5(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_conceptfunction(lret,&subtree) || m_framecontainer(lret,&subtree) || m_multideclaration(lret,&subtree) || m_increment(lret,&subtree) || m_affectation(lret,&subtree) || m_call(lret,&subtree) || m_variable(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_sousbloc_0_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="sousbloc";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_sousbloc_0_4_5(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,30);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,';')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_sousbloc(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="sousbloc";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_sousbloc_0_1(lret,&subtree) || m_sousbloc_0_4(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_complexbloc(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="complexbloc";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_switch(lret,&subtree) || m_trymaybe(lret,&subtree) || m_trycatch(lret,&subtree) || m_loop(lret,&subtree) || m_doloop(lret,&subtree) || m_iftest(lret,&subtree) || m_forin(lret,&subtree) || m_for(lret,&subtree) || m_namespace(lret,&subtree) || m_catchon(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_singleinstruction(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="singleinstruction";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_tamgulisp(lret,&subtree) || m_hdata(lret,&subtree) || m_hontology(lret,&subtree) || m_telque(lret,&subtree) || m_increment(lret,&subtree) || m_affectation(lret,&subtree) || m_call(lret,&subtree) || m_variable(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_analyse_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_signature(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_analyse_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_shell(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_analyse(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="analyse";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_analyse_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_analyse_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_blocs(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_variable_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_shapeindexes(lret,&subtree) || m_interval(lret,&subtree) || m_indexes(lret,&subtree) || m_subfuncbis(lret,&subtree) || m_framevariable(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_variable_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_variable_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_variable(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="variable";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYFAIL
    subtree=NULL;
    if (m_notreserved(lret,&subtree)) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_variable_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_allvariable(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="allvariable";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_predicatevariable(lret,&subtree) || m_variable(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_purevariable_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_shapeindexes(lret,&subtree) || m_interval(lret,&subtree) || m_indexes(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_purevariable_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_purevariable_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_purevariable(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="purevariable";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYFAIL
    subtree=NULL;
    if (m_notreserved(lret,&subtree)) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_purevariable_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_predvariable_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_predicatevariable(lret,&subtree) || m_purevariable(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_predvariable(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="predvariable";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYFAIL
    subtree=NULL;
    if (m_notreserved(lret,&subtree)) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_predvariable_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_plusplus_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'+') || !x_test_char(lret,'+')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_plusplus_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'-') || !x_test_char(lret,'-')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_plusplus(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="plusplus";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_plusplus_0_1(lret,&subtree) || m_plusplus_0_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_power(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="power";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_string(lret,varstring51) || x_test_string(lret,varstring52))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_affectation_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_valmaptail(lret,&subtree) || m_valvectortail(lret,&subtree) || m_variable(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_affectation_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="affectation";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_operatoraffectation(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,14);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_wnexpressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_affectation(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="affectation";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_affectation_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_affectation_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_increment_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_plusplus(lret,&subtree) || m_power(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_increment(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="increment";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_variable(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_increment_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_notin_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring16)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring15)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_notin(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="notin";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_string(lret,varstring53) || m_notin_0_1(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_in(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="in";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_string(lret,varstring15))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_operatorin(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="operatorin";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_notin(lret,&subtree) || m_in(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_operationin(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="operationin";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_operatorin(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,18);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_negation(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="negation";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_in(lret,tab2) || x_test_string(lret,varstring16) || x_test_string(lret,varstring54))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_minus_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree) || m_astringsimple(lret,&subtree) || m_apreg(lret,&subtree) || m_aspreg(lret,&subtree) || m_atreg(lret,&subtree) || m_astreg(lret,&subtree) || m_fstring(lret,&subtree) || m_festring(lret,&subtree) || m_variable(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_minus(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="minus";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'-')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_minus_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_shapeindexes_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_nocomparisonnorinexpressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_shapeindexes_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //CONSTRAINT
    int foundsolution=0;
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (x_test_char(lret,']')) {
            foundsolution=1;
            x_init_tree(tree,subtree,addsubtree);
            break;
        }
        subtree=NULL;
        if (m_shapeindexes_0_1_2(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    if (foundsolution==0) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_shapeindexes_0_3_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_interval(lret,&subtree) || m_indexes(lret,&subtree) || m_subfuncbis(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_shapeindexes_0_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_shapeindexes_0_3_4(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_shapeindexes(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="shapeindexes";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'[')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'*')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,68);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_nocomparisonnorinexpressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_shapeindexes_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_shapeindexes_0_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_indexes_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_minus(lret,&subtree) || m_apreg(lret,&subtree) || m_aspreg(lret,&subtree) || m_atreg(lret,&subtree) || m_astreg(lret,&subtree) || m_fstring(lret,&subtree) || m_festring(lret,&subtree) || m_expressions(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_indexes(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="indexes";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'[')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,1);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_indexes_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,']')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_variable_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_interval_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_indexes_0_1(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_interval(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="interval";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'[')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_interval_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_sep(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,23);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_interval_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,']')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_variable_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_valtuple_0_1_2_3_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_wnexpressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_valtuple_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYWHILE
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_valtuple_0_1_2_3_4(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    return(1);
}


char bnf_tamgu::m_valtuple_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_wnexpressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_valtuple_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_valtuple_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_valtuple_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_valtuple(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="valtuple";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_wnexpressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,61);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_valtuple_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_valvector(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="valvector";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'[')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,25);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_valtuple_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,']')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_valtail(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="valtail";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'|')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_valvectortail_0_1_2_3_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_valvectortail_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYWHILE
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_valvectortail_0_1_2_3_4(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    return(1);
}


char bnf_tamgu::m_valvectortail_0_1_2_5(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_valtail(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_valvectortail_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_valvectortail_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_valvectortail_0_1_2_5(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_valvectortail_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_valvectortail_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_valvectortail(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="valvectortail";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'[')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,25);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_valvectortail_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,']')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_maptail(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="maptail";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'|')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_variable(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_dicoval(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="dicoval";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,':')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_valmaptail_0_1_2_3_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_dicoval(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_valmaptail_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYWHILE
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_valmaptail_0_1_2_3_4(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    return(1);
}


char bnf_tamgu::m_valmaptail_0_1_2_5(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_maptail(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_valmaptail_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_dicoval(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_valmaptail_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_valmaptail_0_1_2_5(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_valmaptail_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_valmaptail_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_valmaptail(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="valmaptail";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'{')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,26);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_valmaptail_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'}')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_intentionsep(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="intentionsep";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_string(lret,varstring55))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_step(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="step";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,':')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_wnexpressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_intentionvector_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="intentionvector";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_intentionsep(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,25);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_wnexpressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_intentionvector_0_1_3_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_wnexpressions(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_intentionvector_0_1_3_5(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_step(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_intentionvector_0_1_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="intentionvector";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_wnexpressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_intentionsep(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,25);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_intentionvector_0_1_3_4(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_intentionvector_0_1_3_5(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_intentionvector_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_intentionvector_0_1_2(lret,&subtree) || m_intentionvector_0_1_3(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_intentionvector(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="intentionvector";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'[')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_intentionvector_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,']')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_intentionwithdouble_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="intentionwithdouble";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_intentionsep(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,25);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_wnexpressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_wnexpressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_intentionwithdouble_0_1_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="intentionwithdouble";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_wnexpressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_wnexpressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_intentionsep(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,25);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_intentionvector_0_1_3_4(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_intentionwithdouble_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_intentionwithdouble_0_1_2(lret,&subtree) || m_intentionwithdouble_0_1_3(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_intentionwithdouble(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="intentionwithdouble";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'[')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_intentionwithdouble_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,']')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_intentionvect(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="intentionvect";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    bool fnd=false;
    bool nbcomma=false;
    unsigned char c;
    for (long ix=currentpos+1;ix<fx->stack.size();ix++) {
        c=fx->stack[ix][0];
        if (c=='[' || c==']')
            return 0;
        if (c==',') {
            if (nbcomma)
                return 0;
            nbcomma=true;
        }
        if (fx->stack[ix] == "..") {
            fnd=true;
            break;
        }
    }
    if (!fnd)
        return 0;

    //BODYOR
    subtree=NULL;
    if (m_intentionvector(lret,&subtree) || m_intentionwithdouble(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}
char bnf_tamgu::m_dico(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="dico";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_wnexpressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,':')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_wnexpressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_valmap_0_1_2_3_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_dico(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_valmap_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYWHILE
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_valmap_0_1_2_3_4(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    return(1);
}


char bnf_tamgu::m_valmap_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_dico(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_valmap_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_valmap_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_valmap_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_valmap(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="valmap";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'{')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,26);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_valmap_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'}')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_jvector_0_1_2_3_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_jexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_jvector_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYWHILE
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_jvector_0_1_2_3_4(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    return(1);
}


char bnf_tamgu::m_jvector_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_jexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_jvector_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_jvector_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_jvector_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_jvector(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="jvector";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'[')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,25);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_jvector_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,']')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_jdico_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;

    if (currentpos>=fx->stack.size())
        return 0;
    unsigned char xu = fx->stack[currentpos][intoken];
    bool found = false;
    switch (xu) {
        case 34:
            if (m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree)) 
                found=true;
            break;
        case 39:
            if (m_astringsimple(lret,&subtree)) 
                found=true;
            break;
        case '0':
            if (m_anumber(lret,&subtree)) 
                found=true;
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
            if (m_anumber(lret,&subtree)) 
                found=true;
            break;
        case 't':
        case 'f':
            if (m_abool(lret,&subtree)) {
            found=true;
            break;
        }
    }
    if (found)
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}

char bnf_tamgu::m_jdico(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="jdico";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_jdico_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,':')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_jexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_jmap_0_1_2_3_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_jdico(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_jmap_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYWHILE
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_jmap_0_1_2_3_4(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    return(1);
}


char bnf_tamgu::m_jmap_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_jdico(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_jmap_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_jmap_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_jmap_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_jmap(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="jmap";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'{')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,26);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_jmap_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'}')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_jexpression(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="jexpression";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;

    if (currentpos>=fx->stack.size())
        return 0;
    unsigned char xu = fx->stack[currentpos][intoken];
    bool found = false;
    switch (xu) {
        case 34:
            if (m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree)) 
                found=true;
            break;
        case 39:
            if (m_astringsimple(lret,&subtree)) 
                found=true;
            break;
        case '[':
            if (m_jvector(lret,&subtree)) 
                found=true;
            break;
        case '{':
            if (m_jmap(lret,&subtree)) 
                found=true;
            break;
        case '0':
            if (m_anumber(lret,&subtree)) 
                found=true;
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
            if (m_anumber(lret,&subtree)) 
                found=true;
            break;
        case 't':
        case 'f':
            if (m_abool(lret,&subtree)) {
            found=true;
            break;
        }
        default:
            if (m_aconstant(lret,&subtree)) 
                found=true;
    }
    if (found)
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}

char bnf_tamgu::m_aconstant(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="aconstant";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_string(lret,varstring56))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_cut(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="cut";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_char(lret,'!'))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_fail(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="fail";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_string(lret,varstring57))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_stop(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="stop";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_string(lret,varstring58))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_not(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="not";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_char(lret,'~') || x_test_string(lret,varstring16))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_predicate_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_predicateparameters(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_predicate(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="predicate";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYFAIL
    subtree=NULL;
    if (m_notreserved(lret,&subtree)) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_predicate_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_predicatecall(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="predicatecall";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_functioncall(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_predicate_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_retractcommand(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="retractcommand";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_string(lret,varstring59))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_assertcommandbefore(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="assertcommandbefore";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_string(lret,varstring60))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_assertcommandafter(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="assertcommandafter";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_string(lret,varstring61) || x_test_string(lret,varstring62))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_findallpredicate_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_predicate(lret,&subtree) || m_expression(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_findallpredicate(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="findallpredicate";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring63)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,39);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_findallpredicate_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_predicate(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_assertpredicate_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_retractcommand(lret,&subtree) || m_assertcommandbefore(lret,&subtree) || m_assertcommandafter(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_assertpredicate_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_dependencyfact(lret,&subtree) || m_predicate(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_assertpredicate_0_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_valvectortail_0_1_2_3_4(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_assertpredicate(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="assertpredicate";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_assertpredicate_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,39);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_assertpredicate_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_assertpredicate_0_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_compfeat_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'~') || !x_test_char(lret,'-') || !x_test_char(lret,':')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_compfeat_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'-') || !x_test_char(lret,':')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_compfeat_0_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'~') || !x_test_char(lret,':')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_compfeat(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="compfeat";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_compfeat_0_1(lret,&subtree) || m_compfeat_0_2(lret,&subtree) || m_compfeat_0_3(lret,&subtree) || x_test_in(lret,tab3))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_valplus(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="valplus";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_char(lret,'+'))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_feat_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree) || m_astringsimple(lret,&subtree) || m_word(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_feat_0_2_3_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_not(lret,&subtree) || m_valplus(lret,&subtree) || m_anumber(lret,&subtree) || m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree) || m_astringsimple(lret,&subtree) || m_word(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_feat_0_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_compfeat(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_feat_0_2_3_4(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_feat_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_feat_0_2_3(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_feat(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="feat";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_feat_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_feat_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_features_0_1_2_3_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_feat(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_features_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYWHILE
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_features_0_1_2_3_4(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_features_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_feat(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_features_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_features_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_features_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_features(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="features";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'[')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_features_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,']')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_modifcall(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="modifcall";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_char(lret,'^'))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_dependencyparameters_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_predicatevariable(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_dependencyparameters_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYWHILE
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_dependencyparameters_0_1_2(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    return(1);
}


char bnf_tamgu::m_dependencyparameters(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="dependencyparameters";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_predicatevariable(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_dependencyparameters_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_dependencyfact(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="dependencyfact";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_functioncall(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_features(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_predicate_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_dependency_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_modifcall(lret,&subtree) || m_not(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_dependency_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_dependency_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_dependency_0_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_features(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_dependency_0_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //CONSTRAINT
    int foundsolution=0;
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (x_test_char(lret,')')) {
            foundsolution=1;
            x_init_tree(tree,subtree,addsubtree);
            break;
        }
        subtree=NULL;
        if (m_dependencyparameters(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    if (foundsolution==0) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_dependency(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="dependency";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_dependency_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_functioncall(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_dependency_0_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_dependency_0_4(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_dependance(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="dependance";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_fail(lret,&subtree) || m_cut(lret,&subtree) || m_stop(lret,&subtree) || m_dependency(lret,&subtree) || m_predplusplus(lret,&subtree) || m_affectationpredicate(lret,&subtree) || m_comparepredicate(lret,&subtree) || m_valpredicatevector(lret,&subtree) || m_valmappredicate(lret,&subtree) || m_purecall(lret,&subtree) || m_wnexpressions(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_dependencyresult_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_predicatevariable(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_dependencyparameters_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_dependencyresult_0_4_5_6(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_dependency(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_dependencyresult_0_4_5(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYWHILE
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_dependencyresult_0_4_5_6(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    return(1);
}


char bnf_tamgu::m_dependencyresult_0_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_dependency(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_dependencyresult_0_4_5(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_dependencyresult(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="dependencyresult";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_not(lret,&subtree) || m_dependencyresult_0_1(lret,&subtree) || m_dependencyresult_0_4(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_dependencyrule_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_string(lret,varstring0) || x_test_string(lret,varstring1))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_dependencyrule_0_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_orand(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_dependance(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_dependencyrule_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //CONSTRAINT
    int foundsolution=0;
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (x_test_char(lret,')')) {
            foundsolution=1;
            x_init_tree(tree,subtree,addsubtree);
            break;
        }
        subtree=NULL;
        if (m_dependencyrule_0_2_3(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    if (foundsolution==0) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_dependencyrule(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="dependencyrule";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_dependencyrule_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,62);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_dependance(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_dependencyrule_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_dependencyresult(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_predicatevariable_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_interval(lret,&subtree) || m_indexes(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_predicatevariable_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_predicatevariable_0_1_2_3(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_predicatevariable_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="predicatevariable";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'?')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,40);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_predicatevariable_0_1_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_predicatevariable_0_4_5(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_char(lret,'_') || m_anumber(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_predicatevariable_0_4_6(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_features(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_predicatevariable_0_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="predicatevariable";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'#')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,40);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_predicatevariable_0_4_5(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_predicatevariable_0_4_6(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_predicatevariable(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="predicatevariable";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_predicatevariable_0_1(lret,&subtree) || m_predicatevariable_0_4(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_term_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_predicateparameters(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_term(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="term";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'?')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,37);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_term_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_tuple(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="tuple";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_predicateparameters(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_pformula_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_anumber(lret,&subtree) || m_pexpression(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_pformula_0_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_operation(lret,&subtree) || m_operationin(lret,&subtree) || m_multiply(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_pformula_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_pformula_0_2_3(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_pformula(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="pformula";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_pformula_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_pformula_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_comparepredicate_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="comparepredicate";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_comparator(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,14);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_pformula(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_comparepredicate(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="comparepredicate";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_pformula(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_comparepredicate_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_affectationpredicate_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="affectationpredicate";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_operatoraffectation(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,14);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_pformula(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_affectationpredicate(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="affectationpredicate";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_allvariable(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_affectationpredicate_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_predplusplus(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="predplusplus";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_predvariable(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_plusplus(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_pintentionvector_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="pintentionvector";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_intentionsep(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,25);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_pexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_pintentionvector_0_1_3_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_pexpression(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_pintentionvector_0_1_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="pintentionvector";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_pexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_intentionsep(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,25);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_pintentionvector_0_1_3_4(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_intentionvector_0_1_3_5(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_pintentionvector_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_pintentionvector_0_1_2(lret,&subtree) || m_pintentionvector_0_1_3(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_pintentionvector(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="pintentionvector";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'[')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_pintentionvector_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,']')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_pintentionwithdouble_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="pintentionwithdouble";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_intentionsep(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,25);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_pexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_pexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_pintentionwithdouble_0_1_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="pintentionwithdouble";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_pexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_pexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_intentionsep(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,25);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_pintentionvector_0_1_3_4(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_pintentionwithdouble_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_pintentionwithdouble_0_1_2(lret,&subtree) || m_pintentionwithdouble_0_1_3(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_pintentionwithdouble(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="pintentionwithdouble";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'[')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_pintentionwithdouble_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,']')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_pintentionvect(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="pintentionvect";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    bool fnd=false;
    bool nbcomma=false;
    unsigned char c;
    for (long ix=currentpos+1;ix<fx->stack.size();ix++) {
        c=fx->stack[ix][0];
        if (c=='[' || c==']')
            return 0;
        if (c==',') {
            if (nbcomma)
                return 0;
            nbcomma=true;
        }
        if (fx->stack[ix] == "..") {
            fnd=true;
            break;
        }
    }
    if (!fnd)
        return 0;

    //BODYOR
    subtree=NULL;
    if (m_pintentionvector(lret,&subtree) || m_pintentionwithdouble(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}
char bnf_tamgu::m_merging(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="merging";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (m_pexpression(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_apredicatelist_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="apredicatelist";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,25);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_apredicatelist(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_apredicatelist_0_1_2_4_5(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYCONTEXTUAL
    long poscontext=currentpos;
    int itokcontext=intoken;
    string lretbis;
    if (x_test_char(lretbis,']')) {
        currentpos=poscontext;
        intoken=itokcontext;
    }
    else {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_apredicatelist_0_1_2_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="apredicatelist";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'|')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,25);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_merging(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_apredicatelist_0_1_2_4_5(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_apredicatelist_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_apredicatelist_0_1_2_3(lret,&subtree) || m_apredicatelist_0_1_2_4(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_apredicatelist_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_apredicatelist_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_apredicatelist(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="apredicatelist";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_pexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_apredicatelist_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_valpredicatevector_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_apredicatelist(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_valpredicatevector(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="valpredicatevector";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'[')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_valpredicatevector_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,']')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_pexpression(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="pexpression";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;

    if (currentpos>=fx->stack.size())
        return 0;
    unsigned char xu = fx->stack[currentpos][intoken];
    bool found = false;
    switch (xu) {
        case '?':
            if (m_term(lret,&subtree) || m_predicatevariable(lret,&subtree)) 
                found=true;
            break;
        case '(':
            if (m_tuple(lret,&subtree) || m_parenthetic(lret,&subtree)) 
                found=true;
            break;
        case '#':
            if (m_predicatevariable(lret,&subtree)) 
                found=true;
            break;
        case '[':
            if (m_pintentionvect(lret,&subtree) || m_valpredicatevector(lret,&subtree)) 
                found=true;
            break;
        case '{':
            if (m_valmappredicate(lret,&subtree)) 
                found=true;
            break;
        case 34:
            if (m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree) || m_apreg(lret,&subtree) || m_atreg(lret,&subtree) || m_fstring(lret,&subtree)) 
                found=true;
            break;
        case 39:
            if (m_astringsimple(lret,&subtree) || m_aspreg(lret,&subtree) || m_astreg(lret,&subtree) || m_festring(lret,&subtree)) 
                found=true;
            break;
        case '\\':
            if (m_tamgulisp(lret,&subtree)) 
                found=true;
            break;
        case '<':
            if (m_telque(lret,&subtree)) 
                found=true;
            break;
        case '0':
            if (m_anumber(lret,&subtree)) 
                found=true;
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
            if (m_anumber(lret,&subtree)) 
                found=true;
            break;
        case 't':
        case 'f':
            if (m_abool(lret,&subtree)) {
            found=true;
            break;
        }
        default:
            if (m_purecall(lret,&subtree) || m_purevariable(lret,&subtree)) 
                found=true;
    }
    if (found)
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}

char bnf_tamgu::m_predicateparameters_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_pexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_predicateparameters_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYWHILE
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_predicateparameters_0_1_2(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    return(1);
}


char bnf_tamgu::m_predicateparameters(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="predicateparameters";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_pexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_predicateparameters_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_mapmerging(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="mapmerging";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (m_pexpression(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_mappredicatelist_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_mappredicatelist(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_mappredicatelist_0_1_2_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'|')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_mapmerging(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_mappredicatelist_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_mappredicatelist_0_1_2_3(lret,&subtree) || m_mappredicatelist_0_1_2_4(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_mappredicatelist_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_mappredicatelist_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_mappredicatelist(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="mappredicatelist";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_predicatedico(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_mappredicatelist_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_valmappredicate_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_mappredicatelist(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_valmappredicate(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="valmappredicate";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'{')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,26);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_valmappredicate_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'}')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_predicatedico(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="predicatedico";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_pexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,':')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_pexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_predicateoperator(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="predicateoperator";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_in(lret,tab4))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_predicatesep_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (x_test_char(lret,':'))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_predicatesep(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="predicatesep";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,':')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_predicatesep_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'-')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_predicateexpression_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_not(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_predicateexpression_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_predicateexpression_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_predicatecall(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_predicateexpression_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_fail(lret,&subtree) || m_cut(lret,&subtree) || m_stop(lret,&subtree) || m_findallpredicate(lret,&subtree) || m_assertpredicate(lret,&subtree) || m_predicateexpression_0_1_2(lret,&subtree) || m_predplusplus(lret,&subtree) || m_affectationpredicate(lret,&subtree) || m_comparepredicate(lret,&subtree) || m_valpredicatevector(lret,&subtree) || m_valmappredicate(lret,&subtree) || m_expressions(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_predicateexpression_0_4_5(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="predicateexpression";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_predicateoperator(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,35);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_predicateexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_predicateexpression_0_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_predicateexpression_0_4_5(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_predicateexpression(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="predicateexpression";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_predicateexpression_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_predicateexpression_0_4(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_predicatedefinition_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_abool(lret,&subtree) || m_predicateexpression(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_predicatedefinition(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="predicatedefinition";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_predicate(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_predicatesep(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,20);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_predicatedefinition_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_predicatefact(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="predicatefact";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_assertpredicate_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_dot(lret)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_dcgword(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="dcgword";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_predicate(lret,&subtree) || m_word(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_finaltoken_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;

    if (currentpos>=fx->stack.size())
        return 0;
    unsigned char xu = fx->stack[currentpos][intoken];
    bool found = false;
    switch (xu) {
        case '#':
            if (m_predicatevariable(lret,&subtree)) 
                found=true;
            break;
        case '?':
            if (m_predicatevariable(lret,&subtree)) 
                found=true;
            break;
        case 34:
            if (m_astringdouble(lret,&subtree) || m_apreg(lret,&subtree) || m_atreg(lret,&subtree) || m_fstring(lret,&subtree)) 
                found=true;
            break;
        case 39:
            if (m_astringsimple(lret,&subtree) || m_aspreg(lret,&subtree) || m_astreg(lret,&subtree) || m_festring(lret,&subtree)) 
                found=true;
            break;
        case '0':
            if (m_anumber(lret,&subtree)) 
                found=true;
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
            if (m_anumber(lret,&subtree)) 
                found=true;
            break;
        default:
            if (m_astring(lret,&subtree)) 
                found=true;
    }
    if (found)
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}

char bnf_tamgu::m_finaltoken_0_1_2_4_5(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_finaltoken_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_finaltoken_0_1_2_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYWHILE
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_finaltoken_0_1_2_4_5(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_finaltoken_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_finaltoken_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_finaltoken_0_1_2_4(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_finaltoken_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_finaltoken_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_finaltoken(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="finaltoken";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'[')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_finaltoken_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,']')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_finaldcg(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="finaldcg";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'{')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_predicateexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'}')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_dcg_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_finaltoken(lret,&subtree) || m_dcgword(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_dcg_0_1_3_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_dcgword(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_dcg_0_1_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYWHILE
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_dcg_0_1_3_4(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    return(1);
}


char bnf_tamgu::m_dcg_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_dcg_0_1_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_dcg_0_1_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_dcg_0_5_6(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_finaldcg(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_dcg_0_5(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_dcg_0_5_6(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_dcg(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="dcg";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_dcgword(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'-')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'-')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'>')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,44);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_dcg_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_dcg_0_5(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_achar(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="achar";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (m_word(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_factvalue(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="factvalue";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_abool(lret,&subtree) || m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree) || m_astringsimple(lret,&subtree) || m_anumber(lret,&subtree) || m_achar(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_factparameters_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_factvalue(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_factparameters_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYWHILE
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_factparameters_0_1_2(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    return(1);
}


char bnf_tamgu::m_factparameters(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="factparameters";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_factvalue(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_factparameters_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_rawfact_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,')') || !x_test_dot(lret)) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_rawfact(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="rawfact";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYFAIL
    subtree=NULL;
    if (m_notreserved(lret,&subtree)) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_asimple(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_factparameters(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_rawfact_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_facts(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="facts";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYWHILE
    subtree=NULL;
    if (m_rawfact(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_rawfact(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_taskelltail_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_in(lret,tab5))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_taskelltail(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="taskelltail";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskelltail_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskellexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_taskellexpression(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="taskellexpression";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;

    if (currentpos>=fx->stack.size())
        return 0;
    unsigned char xu = fx->stack[currentpos][intoken];
    bool found = false;
    switch (xu) {
        case '{':
            if (m_taskellmap(lret,&subtree)) 
                found=true;
            break;
        case '[':
            if (m_taskellvector(lret,&subtree)) 
                found=true;
            break;
        case 34:
            if (m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree) || m_apreg(lret,&subtree) || m_atreg(lret,&subtree) || m_fstring(lret,&subtree)) 
                found=true;
            break;
        case 39:
            if (m_astringsimple(lret,&subtree) || m_aspreg(lret,&subtree) || m_astreg(lret,&subtree) || m_festring(lret,&subtree)) 
                found=true;
            break;
        case '0':
            if (m_anumber(lret,&subtree)) 
                found=true;
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
            if (m_anumber(lret,&subtree)) 
                found=true;
            break;
        case 't':
        case 'f':
            if (m_abool(lret,&subtree)) {
            found=true;
            break;
        }
        default:
            if (m_word(lret,&subtree)) 
                found=true;
    }
    if (found)
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}

char bnf_tamgu::m_taskellvector_0_1_2_3_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskellexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_taskellvector_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYWHILE
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_taskellvector_0_1_2_3_4(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    return(1);
}


char bnf_tamgu::m_taskellvector_0_1_2_5(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_taskelltail(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_taskellvector_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskellexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskellvector_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskellvector_0_1_2_5(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_taskellvector_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_taskellvector_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_taskellvector(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="taskellvector";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'[')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskellvector_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,']')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_taskellmaptail_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_in(lret,tab6))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_taskellmaptail(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="taskellmaptail";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskellmaptail_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskellexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_taskellkeymap(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="taskellkeymap";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;

    if (currentpos>=fx->stack.size())
        return 0;
    unsigned char xu = fx->stack[currentpos][intoken];
    bool found = false;
    switch (xu) {
        case 34:
            if (m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree) || m_apreg(lret,&subtree) || m_atreg(lret,&subtree) || m_fstring(lret,&subtree)) 
                found=true;
            break;
        case 39:
            if (m_astringsimple(lret,&subtree) || m_aspreg(lret,&subtree) || m_astreg(lret,&subtree) || m_festring(lret,&subtree)) 
                found=true;
            break;
        case '0':
            if (m_anumber(lret,&subtree)) 
                found=true;
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
            if (m_anumber(lret,&subtree)) 
                found=true;
            break;
        case 't':
        case 'f':
            if (m_abool(lret,&subtree)) {
            found=true;
            break;
        }
        default:
            if (m_word(lret,&subtree)) 
                found=true;
    }
    if (found)
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}

char bnf_tamgu::m_taskelldico(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="taskelldico";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskellkeymap(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,':')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskellexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_taskellmap_0_1_2_3_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskelldico(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_taskellmap_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYWHILE
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_taskellmap_0_1_2_3_4(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    return(1);
}


char bnf_tamgu::m_taskellmap_0_1_2_5(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_taskellmaptail(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_taskellmap_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskelldico(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskellmap_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskellmap_0_1_2_5(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_taskellmap_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_taskellmap_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_taskellmap(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="taskellmap";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'{')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskellmap_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'}')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_static(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="static";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_string(lret,varstring64))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_letkeyword_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_static(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_letkeyword(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="letkeyword";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_letkeyword_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYFAIL
    subtree=NULL;
    if (m_notreserved(lret,&subtree)) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_let_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_valmaptail(lret,&subtree) || m_valvectortail(lret,&subtree) || m_purevariable(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_let_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_hmetafunctions(lret,&subtree) || m_hoperations(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_let(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="let";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_letkeyword(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_let_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'=')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_let_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_letmin_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_hmetafunctions(lret,&subtree) || m_nocomparisonnorinexpressions(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_letmin(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="letmin";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_purevariable(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'=')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_letmin_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_hvalmaptail(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hvalmaptail";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'{')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_valmaptail_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'}')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_range_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_hvalmaptail(lret,&subtree) || m_valvectortail(lret,&subtree) || m_variable(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_range_0_1_3_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'<') || !x_test_char(lret,'-')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_range_0_1_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_string(lret,varstring65) || m_range_0_1_3_4(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_range_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="range";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_range_0_1_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_range_0_1_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,65);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_let_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_range(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="range";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_let(lret,&subtree) || m_range_0_1(lret,&subtree) || m_hbloc(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_hdeclaration_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_taskellmap(lret,&subtree) || m_taskellvector(lret,&subtree) || m_abool(lret,&subtree) || m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree) || m_astringsimple(lret,&subtree) || m_apreg(lret,&subtree) || m_aspreg(lret,&subtree) || m_atreg(lret,&subtree) || m_astreg(lret,&subtree) || m_fstring(lret,&subtree) || m_festring(lret,&subtree) || m_anumber(lret,&subtree) || m_word(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_hdeclaration_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYWHILE
    subtree=NULL;
    if (m_hdeclaration_0_1_2_3(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_hdeclaration_0_1_2_3(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_hdeclaration_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_subdata(lret,&subtree) || m_hdeclaration_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_hdeclaration(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hdeclaration";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hdeclaration_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_subtaskelldeclaration(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="subtaskelldeclaration";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hdeclaration(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_maybe(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="maybe";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring66)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_declarationtaskell_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_subtaskelldeclaration(lret,&subtree) || m_word(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_declarationtaskell_0_2_3_4_5(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'-') || !x_test_char(lret,'>')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_declarationtaskell_0_2_3_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_string(lret,varstring67) || m_declarationtaskell_0_2_3_4_5(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_declarationtaskell_0_2_3_6(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_subtaskelldeclaration(lret,&subtree) || m_maybe(lret,&subtree) || m_word(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_declarationtaskell_0_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_declarationtaskell_0_2_3_4(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_declarationtaskell_0_2_3_6(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_declarationtaskell_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYWHILE
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_declarationtaskell_0_2_3(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_declarationtaskell(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="declarationtaskell";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_declarationtaskell_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_declarationtaskell_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_taskelldeclaration(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="taskelldeclaration";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,':')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,':')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_declarationtaskell(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_taskell_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;

    if (currentpos>=fx->stack.size())
        return 0;
    unsigned char xu = fx->stack[currentpos][intoken];
    bool found = false;
    switch (xu) {
        case '{':
            if (m_taskellmap(lret,&subtree)) 
                found=true;
            break;
        case '[':
            if (m_taskellvector(lret,&subtree)) 
                found=true;
            break;
        case 34:
            if (m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree) || m_apreg(lret,&subtree) || m_atreg(lret,&subtree) || m_fstring(lret,&subtree)) 
                found=true;
            break;
        case 39:
            if (m_astringsimple(lret,&subtree) || m_aspreg(lret,&subtree) || m_astreg(lret,&subtree) || m_festring(lret,&subtree)) 
                found=true;
            break;
        case '0':
            if (m_anumber(lret,&subtree)) 
                found=true;
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
            if (m_anumber(lret,&subtree)) 
                found=true;
            break;
        case 't':
        case 'f':
            if (m_abool(lret,&subtree)) {
            found=true;
            break;
        }
        default:
            if (m_hdeclaration(lret,&subtree) || m_word(lret,&subtree)) 
                found=true;
    }
    if (found)
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}

char bnf_tamgu::m_taskell_0_1_2_4_5(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskell_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_taskell_0_1_2_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYWHILE
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_taskell_0_1_2_4_5(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    return(1);
}


char bnf_tamgu::m_taskell_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskell_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskell_0_1_2_4(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_taskell_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_taskell_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_taskell_0_7(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_in(lret,tab7))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_taskell(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="taskell";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskell_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskell_0_7(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_otherwise(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="otherwise";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'|')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring18)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'=')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_guard_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_otherwise(lret,&subtree) || m_guard(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_guard(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="guard";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'|')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hbooleanexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'=')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,48);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_guard_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_hlambda_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_taskellmap(lret,&subtree) || m_taskellvector(lret,&subtree) || m_word(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_hlambda_0_2_4_5(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    //x_node* subtree=NULL;
    //int addsubtree=0;
    bool exitonfail=false;
    //BODYSYMBOL
    if (!x_test_char(lret,'-') || !x_test_char(lret,'>')) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_hlambda_0_2_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_string(lret,varstring67) || m_hlambda_0_2_4_5(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_hlambda_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //CONSTRAINT
    int foundsolution=0;
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_hlambda_0_2_4(lret,&subtree)) {
            foundsolution=1;
            x_init_tree(tree,subtree,addsubtree);
            break;
        }
        subtree=NULL;
        if (m_hlambda_0_1(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    if (foundsolution==0) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_hlambda(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hlambda";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'\\')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,54);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hlambda_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hlambda_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_wnexpressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_hboollambda(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hboollambda";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'\\')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,54);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hlambda_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hlambda_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hbooleanexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_composefunction_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_power(lret,&subtree) || m_word(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_composefunction_0_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_dot(lret)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_composefunction_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_composefunction_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYWHILE
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_composefunction_0_2_3(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_composefunction(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="composefunction";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_composefunction_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_composefunction_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_hfunction_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYWHILE
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_expression(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    return(1);
}


char bnf_tamgu::m_hfunction(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hfunction";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_composefunction(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hfunction_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_lfold(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="lfold";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_string(lret,varstring68))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_rfold(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="rfold";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_string(lret,varstring69))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_lscan(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="lscan";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_string(lret,varstring70))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_rscan(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="rscan";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_string(lret,varstring71))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_fold(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="fold";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_lfold(lret,&subtree) || m_rfold(lret,&subtree) || m_rscan(lret,&subtree) || m_lscan(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_folding_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_hlambda(lret,&subtree) || m_hfunction(lret,&subtree) || m_operator(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_folding_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_hbloc(lret,&subtree) || m_hcompose(lret,&subtree) || m_hcomposecall(lret,&subtree) || m_expression(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_folding(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="folding";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_fold(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,50);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_folding_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_folding_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_taskellalloperator(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="taskellalloperator";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_string(lret,varstring72) || x_test_string(lret,varstring73))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_taskellboolcheck(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="taskellboolcheck";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_string(lret,varstring3) || x_test_string(lret,varstring17) || x_test_string(lret,varstring25))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_lfold1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="lfold1";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_string(lret,varstring74))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_rfold1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="rfold1";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_string(lret,varstring75))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_lscan1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="lscan1";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_string(lret,varstring76))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_rscan1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="rscan1";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_string(lret,varstring77))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_fold1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="fold1";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_lfold1(lret,&subtree) || m_rfold1(lret,&subtree) || m_rscan1(lret,&subtree) || m_lscan1(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_folding1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="folding1";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_fold1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,50);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_folding_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_folding_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_filterkeyword(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="filterkeyword";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;

    bool found=false;
    unsigned char xs = fx->stack[currentpos][intoken];
    switch (xs) {
        case 'd' :
            if (x_test_string(lret,varstring80))
                found=true;
            break;
        case 'f' :
            if (x_test_string(lret,varstring79))
                found=true;
            break;
        case 't' :
            if (x_test_string(lret,varstring78))
                found=true;
            break;
    }
    if (found)
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}

char bnf_tamgu::m_takekeyword(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="takekeyword";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_string(lret,varstring81) || x_test_string(lret,varstring82))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_inverted(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="inverted";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_operator(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_taskcomparison(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="taskcomparison";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_function_0_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_comparison(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_mapping_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_hlambda(lret,&subtree) || m_operation(lret,&subtree) || m_inverted(lret,&subtree) || m_hfunction(lret,&subtree) || m_operator(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_mapping(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="mapping";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring39)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,51);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_mapping_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_folding_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_filtering_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_hboollambda(lret,&subtree) || m_taskcomparison(lret,&subtree) || m_operationin(lret,&subtree) || m_hfunction(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_filtering(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="filtering";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_filterkeyword(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,49);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_filtering_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_folding_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_taskellalltrue(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="taskellalltrue";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskellalloperator(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,56);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_filtering_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_folding_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_taskellboolchecking(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="taskellboolchecking";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskellboolcheck(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,56);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_folding_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_taking(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="taking";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_takekeyword(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,49);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_folding_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_zipping_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_hlambda(lret,&subtree) || m_operator(lret,&subtree) || m_hfunction(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_zipping_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYWHILE
    subtree=NULL;
    if (m_expression(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_expression(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    return(1);
}


char bnf_tamgu::m_zipping(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="zipping";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring83)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_zipping_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_zipping_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_pairing(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="pairing";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring84)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_zipping_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_cycle(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="cycle";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_string(lret,varstring85) || x_test_string(lret,varstring86))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_cycling(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="cycling";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_cycle(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,52);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_repeating(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="repeating";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring87)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,53);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_flipping(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="flipping";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring88)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,57);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_mapping_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_taskellcaseotherwise_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_hbloc(lret,&subtree) || m_hoperations(lret,&subtree) || m_expression(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_taskellcaseotherwise(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="taskellcaseotherwise";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring18)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskellcaseotherwise_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_taskellresult_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_valmaptail(lret,&subtree) || m_valvectortail(lret,&subtree) || m_booleanexpression(lret,&subtree) || m_expression(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_taskellresult(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="taskellresult";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskellresult_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hlambda_0_2_4(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskellcaseotherwise_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_taskellcase_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskellresult(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_taskellcase_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYWHILE
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_taskellcase_0_1_2(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    return(1);
}


char bnf_tamgu::m_taskellcase_0_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_taskellcaseotherwise(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_taskellcase(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="taskellcase";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring89)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,55);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_purevariable(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring90)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskellresult(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskellcase_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskellcase_0_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_curryingleft_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_hcompose(lret,&subtree) || m_hcomposecall(lret,&subtree) || m_currying(lret,&subtree) || m_expression(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_curryingleft(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="curryingleft";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hoper(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_curryingleft_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_curryingright(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="curryingright";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //CONSTRAINT
    subtree=NULL;
    if (m_hoper(lret,&subtree)) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    subtree=NULL;
    if (m_curryingleft_0_1(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    int foundsolution=0;
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_hoper(lret,&subtree)) {
            foundsolution=1;
            x_init_tree(tree,subtree,addsubtree);
            break;
        }
        subtree=NULL;
        if (m_curryingleft_0_1(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    if (foundsolution==0) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_currying_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_curryingleft(lret,&subtree) || m_curryingright(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_currying(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="currying";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_currying_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_hfunctionoperation_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_subtelque(lret,&subtree) || m_hcompose(lret,&subtree) || m_hcomposecall(lret,&subtree) || m_expression(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_hfunctionoperation_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYWHILE
    subtree=NULL;
    if (m_hfunctionoperation_0_1(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_hfunctionoperation_0_1(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    return(1);
}


char bnf_tamgu::m_hfunctionoperation(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hfunctionoperation";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_operator(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hfunctionoperation_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hfunctionoperation_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_hfunctioncall_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_telque(lret,&subtree) || m_word(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_hfunctioncall_0_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_subtelque(lret,&subtree) || m_hcompose(lret,&subtree) || m_hcomposecall(lret,&subtree) || m_currying(lret,&subtree) || m_expression(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_hfunctioncall_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYWHILE
    subtree=NULL;
    if (m_hfunctioncall_0_2_3(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_hfunctioncall_0_2_3(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    return(1);
}


char bnf_tamgu::m_hfunctioncall(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hfunctioncall";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hfunctioncall_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hfunctioncall_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_hoperator(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hoperator";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_in(lret,tab8))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_hmetafunctions(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static map<string,int> keys;
    static bool init=false;
    if (!init) {
        keys["take"]=11;
        keys["all"]=6;
        keys["and"]=4;
        keys["drop"]=11;
        keys["repeat"]=8;
        keys["replicate"]=10;
        keys["zip"]=9;
        keys["cycle"]=8;
        keys["xor"]=4;
        keys["foldl"]=2;
        keys["map"]=7;
        keys["foldr"]=2;
        keys["scanl1"]=3;
        keys["any"]=6;
        keys["filter"]=1;
        keys["or"]=4;
        keys["scanr1"]=3;
        keys["flip"]=5;
        keys["scanl"]=2;
        keys["scanr"]=2;
        keys["foldl1"]=3;
        keys["foldr1"]=3;
        keys["dropWhile"]=1;
        keys["takeWhile"]=1;
        keys["zipWith"]=0;
        init=true;
    }
    static const char* label="hmetafunctions";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;

    if (currentpos>=fx->stack.size())
        return 0;
    string x=fx->stack[currentpos];
    if (keys.find(x)==keys.end())
        return 0;
    int i=keys[x];
    bool found = false;
    switch (i) {
        case 2:
            if (m_folding(lret,&subtree)) 
                found=true;
            break;
        case 3:
            if (m_folding1(lret,&subtree)) 
                found=true;
            break;
        case 7:
            if (m_mapping(lret,&subtree)) 
                found=true;
            break;
        case 1:
            if (m_filtering(lret,&subtree)) 
                found=true;
            break;
        case 4:
            if (m_taskellboolchecking(lret,&subtree)) 
                found=true;
            break;
        case 6:
            if (m_taskellalltrue(lret,&subtree)) 
                found=true;
            break;
        case 11:
            if (m_taking(lret,&subtree)) 
                found=true;
            break;
        case 0:
            if (m_zipping(lret,&subtree)) 
                found=true;
            break;
        case 9:
            if (m_pairing(lret,&subtree)) 
                found=true;
            break;
        case 8:
            if (m_cycling(lret,&subtree)) 
                found=true;
            break;
        case 10:
            if (m_repeating(lret,&subtree)) 
                found=true;
            break;
        case 5:
            if (m_flipping(lret,&subtree)) 
                found=true;
            break;
    }
    if (found)
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}

char bnf_tamgu::m_hcompose_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static map<string,int> keys;
    static bool init=false;
    if (!init) {
        keys["take"]=11;
        keys["all"]=6;
        keys["and"]=4;
        keys["drop"]=11;
        keys["repeat"]=8;
        keys["replicate"]=10;
        keys["zip"]=9;
        keys["cycle"]=8;
        keys["xor"]=4;
        keys["foldl"]=2;
        keys["map"]=7;
        keys["foldr"]=2;
        keys["scanl1"]=3;
        keys["any"]=6;
        keys["filter"]=1;
        keys["or"]=4;
        keys["scanr1"]=3;
        keys["flip"]=5;
        keys["scanl"]=2;
        keys["scanr"]=2;
        keys["foldl1"]=3;
        keys["foldr1"]=3;
        keys["dropWhile"]=1;
        keys["takeWhile"]=1;
        keys["zipWith"]=0;
        init=true;
    }
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;

    if (currentpos>=fx->stack.size())
        return 0;
    string x=fx->stack[currentpos];
    if (keys.find(x)==keys.end())
        return 0;
    int i=keys[x];
    bool found = false;
    switch (i) {
        case 7:
            if (m_mapping(lret,&subtree)) 
                found=true;
            break;
        case 1:
            if (m_filtering(lret,&subtree)) 
                found=true;
            break;
        case 4:
            if (m_taskellboolchecking(lret,&subtree)) 
                found=true;
            break;
        case 6:
            if (m_taskellalltrue(lret,&subtree)) 
                found=true;
            break;
        case 11:
            if (m_taking(lret,&subtree)) 
                found=true;
            break;
        case 0:
            if (m_zipping(lret,&subtree)) 
                found=true;
            break;
        case 9:
            if (m_pairing(lret,&subtree)) 
                found=true;
            break;
        case 2:
            if (m_folding(lret,&subtree)) 
                found=true;
            break;
        case 3:
            if (m_folding1(lret,&subtree)) 
                found=true;
            break;
        case 8:
            if (m_cycling(lret,&subtree)) 
                found=true;
            break;
        case 10:
            if (m_repeating(lret,&subtree)) 
                found=true;
            break;
        case 5:
            if (m_flipping(lret,&subtree)) 
                found=true;
            break;
    }
    if (found)
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}

char bnf_tamgu::m_hcompose(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hcompose";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_dot(lret)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hcompose_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_hcomposecall(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hcomposecall";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_dot(lret)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hfunctioncall(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_hoper(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hoper";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;

    if (currentpos>=fx->stack.size())
        return 0;
    unsigned char xu = fx->stack[currentpos][intoken];
    bool found = false;
    switch (xu) {
        case '&':
            if (m_operator_0_1(lret,&subtree) || x_test_in(lret,tab9)) 
                found=true;
            break;
        case '|':
            if (m_operator_0_2(lret,&subtree)) 
                found=true;
            break;
        case ':':
            if (m_operator_0_3(lret,&subtree)) 
                found=true;
            break;
        case '^':
            if (m_operator_0_6(lret,&subtree) || x_test_in(lret,tab9)) 
                found=true;
            break;
        case '*':
            if (m_operator_0_8(lret,&subtree) || x_test_in(lret,tab9)) 
                found=true;
            break;
        case '+':
            if (x_test_in(lret,tab9)) 
                found=true;
            break;
        case '-':
            if (x_test_in(lret,tab9)) 
                found=true;
            break;
        case '/':
            if (x_test_in(lret,tab9)) 
                found=true;
            break;
        case '%':
            if (x_test_in(lret,tab9)) 
                found=true;
            break;
        default:
            if (x_test_string(lret,varstring28) || x_test_string(lret,varstring29) || x_test_string(lret,varstring30) || x_test_string(lret,varstring31)) 
                found=true;
    }
    if (found)
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}

char bnf_tamgu::m_hoperation(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hoperation";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hoper(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hoperations(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_negcall(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="negcall";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_negation(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_call(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_hitem(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hitem";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_taskellcase(lret,&subtree) || m_localif(lret,&subtree) || m_abool(lret,&subtree) || m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree) || m_astringsimple(lret,&subtree) || m_apreg(lret,&subtree) || m_aspreg(lret,&subtree) || m_atreg(lret,&subtree) || m_astreg(lret,&subtree) || m_fstring(lret,&subtree) || m_festring(lret,&subtree) || m_anumber(lret,&subtree) || m_negcall(lret,&subtree) || m_call(lret,&subtree) || m_valtuple(lret,&subtree) || m_parenthetic(lret,&subtree) || m_increment(lret,&subtree) || m_variable(lret,&subtree) || m_predicatevariable(lret,&subtree) || m_subtelque(lret,&subtree) || m_telque(lret,&subtree) || m_intentionvect(lret,&subtree) || m_valvector(lret,&subtree) || m_valmap(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_hnegated(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hnegated";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'-')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hitem(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_hmultiply_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_hoperation(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_hmultiply(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hmultiply";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_variable(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_multiply_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hmultiply_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_hoperations_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_hnegated(lret,&subtree) || m_hitem(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_hoperations_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hoperations_0_1_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_multiply_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hmultiply_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_hoperations_0_5(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_anumber(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hmultiply(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_hoperations(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hoperations";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    long max=0;
    vector<x_couple*> results;
    lret="";
    subtree=NULL;
    currentpos=pos;
    intoken=itok;
    if (m_hoperations_0_1(lret,&subtree)) {
        results.push_back(new x_couple(currentpos,intoken,subtree,lret));
        if (max<currentpos) {
            max=currentpos;
        }
    }
    gFail=0;
    lret="";
    subtree=NULL;
    currentpos=pos;
    intoken=itok;
    if (m_hoperations_0_5(lret,&subtree)) {
        results.push_back(new x_couple(currentpos,intoken,subtree,lret));
        if (max<currentpos) {
            max=currentpos;
        }
    }
    gFail=0;
    if (results.size()==0) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    char add=0;
    for (int i=0;i<results.size();i++) {
        if (add==0 && max==results[i]->pos) {
            add=1;
            currentpos=results[i]->pos;
            intoken=results[i]->intoken;
            results[i]->set(lret);
            x_init_tree(tree,results[i]->tree,addsubtree);
        }
        else 
            delete results[i]->tree;
        delete results[i];
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_hcomparator(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hcomparator";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_comparator_0_2(lret,&subtree) || x_test_string(lret,varstring33) || m_comparator_0_3(lret,&subtree) || m_comparator_0_4(lret,&subtree) || m_comparator_0_5(lret,&subtree) || m_comparator_0_6(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_hcomparison(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hcomparison";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hcomparator(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hoperations(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_hcompare_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_hoperations(lret,&subtree) || m_expression(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_hcompare_0_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_operationin(lret,&subtree) || m_hcomparison(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_hcompare_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_hcompare_0_2_3(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_hcompare(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hcompare";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hcompare_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hcompare_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_hforcecompare_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hcompare_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hcompare_0_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_hforcecompare(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hforcecompare";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_hoptionalboolean(lret,&subtree) || m_hforcecompare_0_1(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_hoptionalboolean_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_negation(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_hoptionalboolean(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hoptionalboolean";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hoptionalboolean_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_booleanexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_hbooleanexpression_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    long max=0;
    vector<x_couple*> results;
    lret="";
    subtree=NULL;
    currentpos=pos;
    intoken=itok;
    if (m_hoptionalboolean(lret,&subtree)) {
        results.push_back(new x_couple(currentpos,intoken,subtree,lret));
        if (max<currentpos) {
            max=currentpos;
        }
    }
    gFail=0;
    lret="";
    subtree=NULL;
    currentpos=pos;
    intoken=itok;
    if (m_hcompare(lret,&subtree)) {
        results.push_back(new x_couple(currentpos,intoken,subtree,lret));
        if (max<currentpos) {
            max=currentpos;
        }
    }
    gFail=0;
    if (results.size()==0) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    char add=0;
    for (int i=0;i<results.size();i++) {
        if (add==0 && max==results[i]->pos) {
            add=1;
            currentpos=results[i]->pos;
            intoken=results[i]->intoken;
            results[i]->set(lret);
            x_init_tree(tree,results[i]->tree,addsubtree);
        }
        else 
            delete results[i]->tree;
        delete results[i];
    }
    return(1);
}


char bnf_tamgu::m_hbooleanexpression_0_2_3_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    long max=0;
    vector<x_couple*> results;
    lret="";
    subtree=NULL;
    currentpos=pos;
    intoken=itok;
    if (m_hoptionalboolean(lret,&subtree)) {
        results.push_back(new x_couple(currentpos,intoken,subtree,lret));
        if (max<currentpos) {
            max=currentpos;
        }
    }
    gFail=0;
    lret="";
    subtree=NULL;
    currentpos=pos;
    intoken=itok;
    if (m_hbooleanexpression(lret,&subtree)) {
        results.push_back(new x_couple(currentpos,intoken,subtree,lret));
        if (max<currentpos) {
            max=currentpos;
        }
    }
    gFail=0;
    if (results.size()==0) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    char add=0;
    for (int i=0;i<results.size();i++) {
        if (add==0 && max==results[i]->pos) {
            add=1;
            currentpos=results[i]->pos;
            intoken=results[i]->intoken;
            results[i]->set(lret);
            x_init_tree(tree,results[i]->tree,addsubtree);
        }
        else 
            delete results[i]->tree;
        delete results[i];
    }
    return(1);
}


char bnf_tamgu::m_hbooleanexpression_0_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hbooleanexpression";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_orand(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,8);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hbooleanexpression_0_2_3_4(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_hbooleanexpression_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_hbooleanexpression_0_2_3(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_hbooleanexpression(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hbooleanexpression";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hbooleanexpression_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hbooleanexpression_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_hforcebooleanexpression(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hforcebooleanexpression";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hbooleanexpression_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_orand(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,8);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hbooleanexpression_0_2_3_4(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_hbloc(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hbloc";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'{')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_blocs(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'}')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_whereexpression_0_1_2_3_4_5(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (x_test_string(lret,varstring91))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_whereexpression_0_1_2_3_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_whereexpression_0_1_2_3_4_5(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'=')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_whereexpression_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_whereexpression_0_1_2_3_4(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_whereexpression_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_whereexpression_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hoperations(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_whereexpression_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_hbloc(lret,&subtree) || m_whereexpression_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_whereexpression_0_6_7_8_9(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_in(lret,tab10))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_whereexpression_0_6_7_8(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_whereexpression_0_6_7_8_9(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_whereexpression_0_6_7(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_whereexpression_0_6_7_8(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_whereexpression_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_whereexpression_0_6(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYWHILE
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_whereexpression_0_6_7(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    return(1);
}


char bnf_tamgu::m_whereexpression(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="whereexpression";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring23)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_whereexpression_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_whereexpression_0_6(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_hinexpression(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hinexpression";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring15)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hoperations(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_notadeclaration(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_string(lret,varstring7))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_deriving_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_deriving_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //CONSTRAINT
    int foundsolution=0;
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (x_test_char(lret,')')) {
            foundsolution=1;
            x_init_tree(tree,subtree,addsubtree);
            break;
        }
        subtree=NULL;
        if (m_deriving_0_1_2(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    if (foundsolution==0) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_deriving(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="deriving";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring7)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_deriving_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_subdeclaration(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="subdeclaration";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,':')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,':')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_subdata_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_subdeclaration(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_subdata_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //CONSTRAINT
    int foundsolution=0;
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (x_test_char(lret,'}')) {
            foundsolution=1;
            x_init_tree(tree,subtree,addsubtree);
            break;
        }
        subtree=NULL;
        if (m_subdata_0_1_2(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    if (foundsolution==0) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_subdata(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="subdata";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'{')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_subdeclaration(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_subdata_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_hdatadeclaration_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYFAIL
    subtree=NULL;
    if (m_notadeclaration(lret,&subtree)) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_hdatadeclaration_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYWHILE
    subtree=NULL;
    if (m_hdatadeclaration_0_1_2_3(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_hdatadeclaration_0_1_2_3(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_hdatadeclaration_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_subdata(lret,&subtree) || m_hdatadeclaration_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_hdatadeclaration(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hdatadeclaration";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYFAIL
    subtree=NULL;
    if (m_notadeclaration(lret,&subtree)) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hdatadeclaration_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_hdata_0_1_2_3_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'|')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hdatadeclaration(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_hdata_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYWHILE
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_hdata_0_1_2_3_4(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    return(1);
}


char bnf_tamgu::m_hdata_0_1_2_5(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_deriving(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_hdata_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hdatadeclaration(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hdata_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hdata_0_1_2_5(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_hdata_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_telque(lret,&subtree) || m_hdata_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_hdata(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hdata";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'<')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring92)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,63);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'=')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hdata_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'>')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_hontology_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYWHILE
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_aliasdeclaration_0_1_2(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    return(1);
}


char bnf_tamgu::m_hontology_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hontology_0_1_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_hontology(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hontology";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'<')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring93)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,64);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hontology_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'<')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'=')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'>')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_hconcept(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hconcept";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_string(lret,varstring94) || x_test_string(lret,varstring95) || x_test_string(lret,varstring96))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_conceptfunction(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="conceptfunction";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hconcept(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring37)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_returntaskelldeclaration(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="returntaskelldeclaration";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,':')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,':')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_declarationtaskell(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_assignfield(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="assignfield";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'=')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_wnexpressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_dataassignment_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_assignfield(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_dataassignment_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //CONSTRAINT
    int foundsolution=0;
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (x_test_char(lret,'}')) {
            foundsolution=1;
            x_init_tree(tree,subtree,addsubtree);
            break;
        }
        subtree=NULL;
        if (m_dataassignment_0_1_2(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    if (foundsolution==0) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_dataassignment(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="dataassignment";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'{')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_assignfield(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_dataassignment_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_hrange_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hoperator(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_range(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_hrange_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYWHILE
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_hrange_0_1_2(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_hrange_0_3_4_5_6_7_8(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_whereexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_hrange_0_3_4_5_6_7(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_hrange_0_3_4_5_6_7_8(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_hrange_0_3_4_5_6(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hbooleanexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hrange_0_3_4_5_6_7(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_hrange_0_3_4_5(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_whereexpression(lret,&subtree) || m_hrange_0_3_4_5_6(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_hrange_0_3_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hrange_0_3_4_5(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_hrange_0_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_hrange_0_3_4(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_hrange(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="hrange";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_range(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hrange_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hrange_0_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_telque_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_returntaskelldeclaration(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_telque_0_1_2_4_5_6(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_hconcept(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_telque_0_1_2_4_5(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_telque_0_1_2_4_5_6(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_taskell(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_telque_0_1_2_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_telque_0_1_2_4_5(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_telque_0_1_2_7_8_9_10_11(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_in(lret,tab11))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_telque_0_1_2_7_8_9_10(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_telque_0_1_2_7_8_9_10_11(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_letmin(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_telque_0_1_2_7_8_9(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //CONSTRAINT
    int foundsolution=0;
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_hinexpression(lret,&subtree)) {
            foundsolution=1;
            x_init_tree(tree,subtree,addsubtree);
            break;
        }
        subtree=NULL;
        if (m_telque_0_1_2_7_8_9_10(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    if (foundsolution==0) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_telque_0_1_2_7_8_12_13(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_whereexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_telque_0_1_2_7_8_12(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_telque_0_1_2_7_8_12_13(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_telque_0_1_2_7_8(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring91)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_letmin(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_telque_0_1_2_7_8_9(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_telque_0_1_2_7_8_12(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_telque_0_1_2_7_14_15_16(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    long max=0;
    vector<x_couple*> results;
    lret="";
    subtree=NULL;
    currentpos=pos;
    intoken=itok;
    if (m_hforcecompare(lret,&subtree)) {
        results.push_back(new x_couple(currentpos,intoken,subtree,lret));
        if (max<currentpos) {
            max=currentpos;
        }
    }
    gFail=0;
    lret="";
    subtree=NULL;
    currentpos=pos;
    intoken=itok;
    if (m_hoperations(lret,&subtree)) {
        results.push_back(new x_couple(currentpos,intoken,subtree,lret));
        if (max<currentpos) {
            max=currentpos;
        }
    }
    gFail=0;
    lret="";
    subtree=NULL;
    currentpos=pos;
    intoken=itok;
    if (m_hfunctioncall(lret,&subtree)) {
        results.push_back(new x_couple(currentpos,intoken,subtree,lret));
        if (max<currentpos) {
            max=currentpos;
        }
    }
    gFail=0;
    lret="";
    subtree=NULL;
    currentpos=pos;
    intoken=itok;
    if (m_hrange(lret,&subtree)) {
        results.push_back(new x_couple(currentpos,intoken,subtree,lret));
        if (max<currentpos) {
            max=currentpos;
        }
    }
    gFail=0;
    if (results.size()==0) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    char add=0;
    for (int i=0;i<results.size();i++) {
        if (add==0 && max==results[i]->pos) {
            add=1;
            currentpos=results[i]->pos;
            intoken=results[i]->intoken;
            results[i]->set(lret);
            x_init_tree(tree,results[i]->tree,addsubtree);
        }
        else 
            delete results[i]->tree;
        delete results[i];
    }
    return(1);
}


char bnf_tamgu::m_telque_0_1_2_7_14_15(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_hfunctionoperation(lret,&subtree) || m_telque_0_1_2_7_14_15_16(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_telque_0_1_2_7_14_17_18_19(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_char(lret,'|') || x_test_string(lret,varstring23))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_telque_0_1_2_7_14_17_18_20_21_22_23(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hoperator(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_range(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_telque_0_1_2_7_14_17_18_20_21_22(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYWHILE
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_telque_0_1_2_7_14_17_18_20_21_22_23(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    return(1);
}


char bnf_tamgu::m_telque_0_1_2_7_14_17_18_20_21_24_25_26_27(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hbooleanexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_telque_0_1_2_7_8_12(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_telque_0_1_2_7_14_17_18_20_21_24_25_26(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_whereexpression(lret,&subtree) || m_telque_0_1_2_7_14_17_18_20_21_24_25_26_27(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_telque_0_1_2_7_14_17_18_20_21_24_25(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_telque_0_1_2_7_14_17_18_20_21_24_25_26(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_telque_0_1_2_7_14_17_18_20_21_24(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_telque_0_1_2_7_14_17_18_20_21_24_25(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_telque_0_1_2_7_14_17_18_20_21(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_range(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_telque_0_1_2_7_14_17_18_20_21_22(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_telque_0_1_2_7_14_17_18_20_21_24(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_telque_0_1_2_7_14_17_18_20(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_telque_0_1_2_7_14_17_18_20_21(lret,&subtree) || m_whereexpression(lret,&subtree) || m_hbooleanexpression(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_telque_0_1_2_7_14_17_18(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_telque_0_1_2_7_14_17_18_19(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_telque_0_1_2_7_14_17_18_20(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_telque_0_1_2_7_14_17(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_telque_0_1_2_7_14_17_18(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_telque_0_1_2_7_14(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_telque_0_1_2_7_14_15(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_telque_0_1_2_7_14_17(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_telque_0_1_2_7(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_hmetafunctions(lret,&subtree) || m_guard(lret,&subtree) || m_telque_0_1_2_7_8(lret,&subtree) || m_hforcebooleanexpression(lret,&subtree) || m_telque_0_1_2_7_14(lret,&subtree) || m_hbloc(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_telque_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_telque_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_telque_0_1_2_4(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_telque_0_1_2_7(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_telque_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_dataassignment(lret,&subtree) || m_taskelldeclaration(lret,&subtree) || m_telque_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_telque(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="telque";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'<')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,48);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_telque_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'>')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_subtelque_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    long max=0;
    vector<x_couple*> results;
    lret="";
    subtree=NULL;
    currentpos=pos;
    intoken=itok;
    if (m_hoperations(lret,&subtree)) {
        results.push_back(new x_couple(currentpos,intoken,subtree,lret));
        if (max<currentpos) {
            max=currentpos;
        }
    }
    gFail=0;
    lret="";
    subtree=NULL;
    currentpos=pos;
    intoken=itok;
    if (m_hfunctioncall(lret,&subtree)) {
        results.push_back(new x_couple(currentpos,intoken,subtree,lret));
        if (max<currentpos) {
            max=currentpos;
        }
    }
    gFail=0;
    if (results.size()==0) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    char add=0;
    for (int i=0;i<results.size();i++) {
        if (add==0 && max==results[i]->pos) {
            add=1;
            currentpos=results[i]->pos;
            intoken=results[i]->intoken;
            results[i]->set(lret);
            x_init_tree(tree,results[i]->tree,addsubtree);
        }
        else 
            delete results[i]->tree;
        delete results[i];
    }
    return(1);
}


char bnf_tamgu::m_subtelque_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_hfunctionoperation(lret,&subtree) || m_subtelque_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_subtelque(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="subtelque";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'$')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,48);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_subtelque_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_parenthetic(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="parenthetic";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,4);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_wnexpressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_expression_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;

    if (currentpos>=fx->stack.size())
        return 0;
    unsigned char xu = fx->stack[currentpos][intoken];
    bool found = false;
    switch (xu) {
        case '\\':
            if (m_tamgulisp(lret,&subtree)) 
                found=true;
            break;
        case '(':
            if (m_valtuple(lret,&subtree) || m_parenthetic(lret,&subtree)) 
                found=true;
            break;
        case '<':
            if (m_telque(lret,&subtree)) 
                found=true;
            break;
        default:
            if (m_call(lret,&subtree) || m_allvariable(lret,&subtree)) 
                found=true;
    }
    if (found)
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}

char bnf_tamgu::m_expression_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_negation(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expression_0_1_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_expression_0_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;

    if (currentpos>=fx->stack.size())
        return 0;
    unsigned char xu = fx->stack[currentpos][intoken];
    bool found = false;
    switch (xu) {
        case '\\':
            if (m_tamgulisp(lret,&subtree)) 
                found=true;
            break;
        case 34:
            if (m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree) || m_apreg(lret,&subtree) || m_atreg(lret,&subtree) || m_fstring(lret,&subtree)) 
                found=true;
            break;
        case 39:
            if (m_astringsimple(lret,&subtree) || m_aspreg(lret,&subtree) || m_astreg(lret,&subtree) || m_festring(lret,&subtree)) 
                found=true;
            break;
        case '(':
            if (m_valtuple(lret,&subtree) || m_parenthetic(lret,&subtree)) 
                found=true;
            break;
        case '<':
            if (m_telque(lret,&subtree)) 
                found=true;
            break;
        case '[':
            if (m_intentionvect(lret,&subtree) || m_valvector(lret,&subtree)) 
                found=true;
            break;
        case '{':
            if (m_valmap(lret,&subtree)) 
                found=true;
            break;
        case '0':
            if (m_anumber(lret,&subtree)) 
                found=true;
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
            if (m_anumber(lret,&subtree)) 
                found=true;
            break;
        case 'c':
            if (m_taskellcase(lret,&subtree) || m_call(lret,&subtree) || m_increment(lret,&subtree) || m_allvariable(lret,&subtree)) 
                found=true;
            break;
        case 'i':
            if (m_localif(lret,&subtree) || m_call(lret,&subtree) || m_increment(lret,&subtree) || m_allvariable(lret,&subtree)) 
                found=true;
            break;
        case 't':
        case 'f':
            if (m_abool(lret,&subtree) || m_call(lret,&subtree) || m_increment(lret,&subtree) || m_allvariable(lret,&subtree)) 
                found=true;
            break;
        default:
            if (m_call(lret,&subtree) || m_increment(lret,&subtree) || m_allvariable(lret,&subtree)) 
                found=true;
    }
    if (found)
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}

char bnf_tamgu::m_expression(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="expression";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_expression_0_1(lret,&subtree) || m_expression_0_3(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_expressions_0_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_fulloperation(lret,&subtree) || m_comparison(lret,&subtree) || m_operationin(lret,&subtree) || m_multiply(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_expressions_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_expressions_0_2_3(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_expressions(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="expressions";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_multiply_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expressions_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_comparing_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_comparison(lret,&subtree) || m_operationin(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_comparing(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="comparing";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_comparing_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_wnexpressions_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_anumber(lret,&subtree) || m_negated(lret,&subtree) || m_expression(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_wnexpressions_0_3_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_operation(lret,&subtree) || m_comparison(lret,&subtree) || m_operationin(lret,&subtree) || m_multiply(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_wnexpressions_0_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_wnexpressions_0_3_4(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_wnexpressions(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="wnexpressions";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_wnexpressions_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_multiply_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_wnexpressions_0_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_nocomparisonexpressions(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="nocomparisonexpressions";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_wnexpressions_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_multiply_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_pformula_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_nocomparisonnorinexpressions_0_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_operation(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_nocomparisonnorinexpressions(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="nocomparisonnorinexpressions";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_wnexpressions_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_multiply_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_nocomparisonnorinexpressions_0_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_numbers_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_operation(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_numbers(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="numbers";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_numbers_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_negated(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="negated";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'-')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_abool(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="abool";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_string(lret,varstring97) || x_test_string(lret,varstring98))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_blocfor_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_affectation(lret,&subtree) || m_expression(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_blocfor_0_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_blocfor(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_blocfor_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_blocfor_0_2_3(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_blocfor(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="blocfor";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_blocfor_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_blocfor_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_localif_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring10)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_localif_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_localif_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_localif(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="localif";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring14)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,10);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_booleanexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_localif_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_declarationfor_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_variable(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring15)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_declarationfor_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_type(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_variable(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring15)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_declarationfor(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="declarationfor";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_declarationfor_0_1(lret,&subtree) || m_declarationfor_0_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_optionalboolean(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="optionalboolean";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_booleanexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_booleanexpression_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    long max=0;
    vector<x_couple*> results;
    lret="";
    subtree=NULL;
    currentpos=pos;
    intoken=itok;
    if (m_optionalboolean(lret,&subtree)) {
        results.push_back(new x_couple(currentpos,intoken,subtree,lret));
        if (max<currentpos) {
            max=currentpos;
        }
    }
    gFail=0;
    lret="";
    subtree=NULL;
    currentpos=pos;
    intoken=itok;
    if (m_expressions(lret,&subtree)) {
        results.push_back(new x_couple(currentpos,intoken,subtree,lret));
        if (max<currentpos) {
            max=currentpos;
        }
    }
    gFail=0;
    if (results.size()==0) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    char add=0;
    for (int i=0;i<results.size();i++) {
        if (add==0 && max==results[i]->pos) {
            add=1;
            currentpos=results[i]->pos;
            intoken=results[i]->intoken;
            results[i]->set(lret);
            x_init_tree(tree,results[i]->tree,addsubtree);
        }
        else 
            delete results[i]->tree;
        delete results[i];
    }
    return(1);
}


char bnf_tamgu::m_booleanexpression_0_2_3_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    long max=0;
    vector<x_couple*> results;
    lret="";
    subtree=NULL;
    currentpos=pos;
    intoken=itok;
    if (m_optionalboolean(lret,&subtree)) {
        results.push_back(new x_couple(currentpos,intoken,subtree,lret));
        if (max<currentpos) {
            max=currentpos;
        }
    }
    gFail=0;
    lret="";
    subtree=NULL;
    currentpos=pos;
    intoken=itok;
    if (m_booleanexpression(lret,&subtree)) {
        results.push_back(new x_couple(currentpos,intoken,subtree,lret));
        if (max<currentpos) {
            max=currentpos;
        }
    }
    gFail=0;
    if (results.size()==0) {
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    char add=0;
    for (int i=0;i<results.size();i++) {
        if (add==0 && max==results[i]->pos) {
            add=1;
            currentpos=results[i]->pos;
            intoken=results[i]->intoken;
            results[i]->set(lret);
            x_init_tree(tree,results[i]->tree,addsubtree);
        }
        else 
            delete results[i]->tree;
        delete results[i];
    }
    return(1);
}


char bnf_tamgu::m_booleanexpression_0_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="booleanexpression";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_orand(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,8);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_booleanexpression_0_2_3_4(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_booleanexpression_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_booleanexpression_0_2_3(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_booleanexpression(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="booleanexpression";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_booleanexpression_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_booleanexpression_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_arange_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_valtuple_0_1_2_3_4(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_arange_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_wnexpressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_arange_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_arange_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_arange_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_arange(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="arange";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'<')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,41);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_wnexpressions(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_arange_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'>')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_iftest_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_instruction(lret,&subtree) || m_complexbloc(lret,&subtree) || m_bloc(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_iftest_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_testelif(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_iftest_0_3_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="iftest";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring10)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,24);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_iftest_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_iftest_0_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_iftest_0_3_4(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_iftest(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="iftest";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring14)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,10);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_booleanexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_iftest_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_iftest_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_iftest_0_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_testelif(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="testelif";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring9)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,10);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_booleanexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_iftest_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_iftest_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_iftest_0_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_loop(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="loop";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring24)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,11);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_booleanexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_iftest_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_doloop(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="doloop";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring8)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_iftest_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring24)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,11);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_booleanexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_declarationending(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_for_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_multideclaration(lret,&subtree) || m_blocfor(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_for(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="for";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring11)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,12);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_for_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,';')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_booleanexpression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,';')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_blocfor(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_iftest_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_forin_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;

    if (currentpos>=fx->stack.size())
        return 0;
    unsigned char xu = fx->stack[currentpos][intoken];
    bool found = false;
    switch (xu) {
        case '{':
            if (m_valmaptail(lret,&subtree)) 
                found=true;
            break;
        case '[':
            if (m_valvectortail(lret,&subtree)) 
                found=true;
            break;
    }
    if (found)
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}

char bnf_tamgu::m_forin_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_forin_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring15)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_forin_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_forin_0_1_2(lret,&subtree) || m_declarationfor(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_forin_0_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_arange(lret,&subtree) || m_expressions(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_forin(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="forin";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring11)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_forin_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,33);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_forin_0_4(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_iftest_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_namespace(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="namespace";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring99)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_instruction(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_switch_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //CONSTRAINT
    subtree=NULL;
    if (x_test_char(lret,'}')) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    subtree=NULL;
    if (m_testswitch(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    int foundsolution=0;
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (x_test_char(lret,'}')) {
            foundsolution=1;
            x_init_tree(tree,subtree,addsubtree);
            break;
        }
        subtree=NULL;
        if (m_testswitch(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    if (foundsolution==0) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_switch(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="switch";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring21)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,16);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_expression(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_declaration_0_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'{')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,31);
    bnf_tamgu_pop pop1(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_switch_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_trycatch_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_word(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_trycatch_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_trycatch_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_trycatch_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_trycatch_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_trycatch_0_4_5(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="trycatch";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'{')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,22);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_blocs(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'}')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_trycatch_0_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_char(lret,';') || m_trycatch_0_4_5(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_trycatch(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="trycatch";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring100)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'{')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,22);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_blocs(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'}')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring101)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,22);
    bnf_tamgu_pop pop1(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_trycatch_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_trycatch_0_4(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_trymaybe(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="trymaybe";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring102)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'{')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,22);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_blocs(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'}')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_testswitch_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;

    if (currentpos>=fx->stack.size())
        return 0;
    unsigned char xu = fx->stack[currentpos][intoken];
    bool found = false;
    switch (xu) {
        case 34:
            if (m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree)) 
                found=true;
            break;
        case 39:
            if (m_astringsimple(lret,&subtree)) 
                found=true;
            break;
        case '0':
            if (m_anumber(lret,&subtree)) 
                found=true;
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
            if (m_anumber(lret,&subtree)) 
                found=true;
            break;
        case 't':
        case 'f':
            if (m_abool(lret,&subtree)) {
            found=true;
            break;
        }
        default:
            if (m_default(lret,&subtree)) 
                found=true;
    }
    if (found)
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}

char bnf_tamgu::m_testswitch(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="testswitch";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_testswitch_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,':')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,17);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_iftest_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_catchon(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="catchon";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,varstring26)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,70);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_singleinstruction(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_iftest_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_default(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="default";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_string(lret,varstring103))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_parentheticexpression(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="parentheticexpression";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_astringdouble(lret,&subtree) || m_afullstring(lret,&subtree) || m_astringsimple(lret,&subtree) || m_anumber(lret,&subtree) || m_astring(lret,&subtree) || m_punct(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_parentheticexpressions(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="parentheticexpressions";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_oneparenthetic(lret,&subtree) || m_parentheticexpression(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_oneparenthetic_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //CONSTRAINT
    int foundsolution=0;
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (x_test_char(lret,Y_var_1)) {
            foundsolution=1;
            x_init_tree(tree,subtree,addsubtree);
            break;
        }
        subtree=NULL;
        if (m_parentheticexpressions(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    if (foundsolution==0) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_oneparenthetic(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="oneparenthetic";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,Y_var_0)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_oneparenthetic_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_parenthetique(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="parenthetique";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYWHILE
    subtree=NULL;
    if (m_oneparenthetic(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_oneparenthetic(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_tagexpression(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="tagexpression";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_anumber(lret,&subtree) || m_astring(lret,&subtree) || m_punct(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_tagexpressions(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="tagexpressions";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_onetag(lret,&subtree) || m_tagexpression(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_onetag_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //CONSTRAINT
    int foundsolution=0;
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (x_test_string(lret,VS_var_3)) {
            foundsolution=1;
            x_init_tree(tree,subtree,addsubtree);
            break;
        }
        subtree=NULL;
        if (m_tagexpressions(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    if (foundsolution==0) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_onetag(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="onetag";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_string(lret,VS_var_2)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_onetag_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_tag(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="tag";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYWHILE
    subtree=NULL;
    if (m_onetag(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_onetag(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_tlvariable(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="tlvariable";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYFAIL
    subtree=NULL;
    if (m_notreserved(lret,&subtree)) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_predicatevariable_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_tlatom(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="tlatom";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;

    if (currentpos>=fx->stack.size())
        return 0;
    unsigned char xu = fx->stack[currentpos][intoken];
    bool found = false;
    switch (xu) {
        case '#':
            if (m_predicatevariable(lret,&subtree)) 
                found=true;
            break;
        case '?':
            if (m_predicatevariable(lret,&subtree)) 
                found=true;
            break;
        case 34:
            if (m_astringdouble(lret,&subtree) || m_apreg(lret,&subtree) || m_atreg(lret,&subtree) || m_fstring(lret,&subtree)) 
                found=true;
            break;
        case 39:
            if (m_aspreg(lret,&subtree) || m_astreg(lret,&subtree) || m_festring(lret,&subtree)) 
                found=true;
            break;
        case '0':
            if (m_anumber(lret,&subtree)) 
                found=true;
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
            if (m_anumber(lret,&subtree)) 
                found=true;
            break;
        default:
            if (m_tlvariable(lret,&subtree) || m_word(lret,&subtree)) 
                found=true;
    }
    if (found)
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}

char bnf_tamgu::m_tlquote_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;

    if (currentpos>=fx->stack.size())
        return 0;
    unsigned char xu = fx->stack[currentpos][intoken];
    bool found = false;
    switch (xu) {
        case 39:
            if (m_tlquote(lret,&subtree)) 
                found=true;
            break;
        case '{':
            if (m_tlkeys(lret,&subtree)) 
                found=true;
            break;
        case '(':
            if (m_tlist(lret,&subtree)) 
                found=true;
            break;
        default:
            if (m_tlatom(lret,&subtree) || m_comparator(lret,&subtree) || m_operator(lret,&subtree)) 
                found=true;
    }
    if (found)
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}

char bnf_tamgu::m_tlquote(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="tlquote";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,39)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_tlquote_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_tlkey_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;

    if (currentpos>=fx->stack.size())
        return 0;
    unsigned char xu = fx->stack[currentpos][intoken];
    bool found = false;
    switch (xu) {
        case 39:
            if (m_tlquote(lret,&subtree)) 
                found=true;
            break;
        case '{':
            if (m_tlkeys(lret,&subtree)) 
                found=true;
            break;
        case '(':
            if (m_tlist(lret,&subtree)) 
                found=true;
            break;
        default:
            if (m_tlatom(lret,&subtree)) 
                found=true;
    }
    if (found)
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}

char bnf_tamgu::m_tlkey(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="tlkey";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_tlatom(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,':')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_tlkey_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_tlkeys_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //CONSTRAINT
    int foundsolution=0;
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (x_test_char(lret,'}')) {
            foundsolution=1;
            x_init_tree(tree,subtree,addsubtree);
            break;
        }
        subtree=NULL;
        if (m_tlkey(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    if (foundsolution==0) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_tlkeys(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="tlkeys";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'{')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,26);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_tlkeys_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_tlist_0_1_2_3_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_operator(lret,&subtree) || m_comparator(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_tlist_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_tlist_0_1_2_3_4(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_tlist_0_1_2_5(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYWHILE
    subtree=NULL;
    if (m_tlkey_0_1(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_tlkey_0_1(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    return(1);
}


char bnf_tamgu::m_tlist_0_1_2_7(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_operator(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_tlist_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_tlist_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_tlist_0_1_2_5(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_tlist_0_1_2_7(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_tlist_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;

    if (currentpos>=fx->stack.size())
        return 0;
    unsigned char xu = fx->stack[currentpos][intoken];
    bool found = false;
    switch (xu) {
        case ')':
            if (x_test_char(lret,')')) 
                found=true;
            break;
        default:
            if (m_tlist_0_1_2(lret,&subtree)) 
                found=true;
    }
    if (found)
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}

char bnf_tamgu::m_tlist(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="tlist";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,67);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_tlist_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_tamgulisp(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="tamgulisp";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'\\')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_tlkey_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_tamgupurelisp(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="tamgupurelisp";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (m_tlist_0_1_2_5(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_ameta_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_in(lret,tab12))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_ameta_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="ameta";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'%')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,59);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_ameta_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_ameta_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_char(lret,'?') || m_ameta_0_1_2(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_ameta_0_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (x_test_char(lret,'_'))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_ameta(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="ameta";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_ameta_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_ameta_0_4(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_amulti(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="amulti";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'$')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_asimple(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="asimple";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (m_word(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_atoken_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_subgram(lret,&subtree) || m_ameta(lret,&subtree) || m_amulti(lret,&subtree) || m_astringdouble(lret,&subtree) || m_astringsimple(lret,&subtree) || m_anumber(lret,&subtree) || m_abool(lret,&subtree) || m_word(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_atoken(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="atoken";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_hoptionalboolean_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_atoken_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_anitem(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="anitem";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_ameta(lret,&subtree) || m_amulti(lret,&subtree) || m_astringdouble(lret,&subtree) || m_astringsimple(lret,&subtree) || m_anumber(lret,&subtree) || m_abool(lret,&subtree) || m_asimple(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_dj(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="dj";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_char(lret,';'))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_aoptional(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="aoptional";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_atoken(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_aoptitem(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="aoptitem";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_anitem(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,')')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_akleene(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="akleene";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_operator_0_8(lret,&subtree) || m_plusplus_0_1(lret,&subtree) || x_test_in(lret,tab13))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_anelement_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_akleene(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_anelement_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_atoken(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_anelement_0_1_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_anelement(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="anelement";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_aoptional(lret,&subtree) || m_anelement_0_1(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_anitemelement_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_anitem(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_anelement_0_1_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_anitemelement(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="anitemelement";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_aoptitem(lret,&subtree) || m_anitemelement_0_1(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_body_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_dj(lret,&subtree) || x_test_char(lret,','))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_body_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_body_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_anitemelement(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_body_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYWHILE
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_body_0_1_2(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    return(1);
}


char bnf_tamgu::m_body(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="body";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_anitemelement(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_body_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_rule_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_body_0_1_2_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_anelement(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_rule_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //CONSTRAINT
    int foundsolution=0;
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (x_test_dot(lret)) {
            foundsolution=1;
            x_init_tree(tree,subtree,addsubtree);
            break;
        }
        subtree=NULL;
        if (m_rule_0_1_2(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    if (foundsolution==0) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_rule(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="rule";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,':')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'=')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_anelement(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_rule_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_subgram_0_1_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'|')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_body(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_subgram_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_subgram_0_1_2_3(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_subgram_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //CONSTRAINT
    int foundsolution=0;
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (x_test_char(lret,']')) {
            foundsolution=1;
            x_init_tree(tree,subtree,addsubtree);
            break;
        }
        subtree=NULL;
        if (m_subgram_0_1_2(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    if (foundsolution==0) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_subgram(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="subgram";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'[')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_body(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_subgram_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_rules(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="rules";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYWHILE
    subtree=NULL;
    if (m_rule(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_rule(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_non(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="non";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_char(lret,'~'))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_kleene_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_in(lret,tab14))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_kleene_0_2_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,':')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_anumber(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_kleene_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_kleene_0_2_3(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_kleene(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="kleene";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_kleene_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_kleene_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_any(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="any";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    if (x_test_char(lret,'?'))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_meta(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="meta";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'%')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_any(lret)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_alabel_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_non(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_alabel(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="alabel";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_alabel_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_orlabels_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_alabel(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_orlabels_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //CONSTRAINT
    int foundsolution=0;
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (x_test_char(lret,'}')) {
            foundsolution=1;
            x_init_tree(tree,subtree,addsubtree);
            break;
        }
        subtree=NULL;
        if (m_orlabels_0_1_2(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    if (foundsolution==0) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_orlabels(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="orlabels";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'#')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'{')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_alabel(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_orlabels_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_andlabels_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //CONSTRAINT
    int foundsolution=0;
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (x_test_char(lret,']')) {
            foundsolution=1;
            x_init_tree(tree,subtree,addsubtree);
            break;
        }
        subtree=NULL;
        if (m_orlabels_0_1_2(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    if (foundsolution==0) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_andlabels(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="andlabels";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'#')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'[')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_alabel(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_andlabels_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_label(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="label";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'#')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_lemma(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="lemma";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'$')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_callmatch_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //CONSTRAINT
    int foundsolution=0;
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (x_test_char(lret,'>')) {
            foundsolution=1;
            x_init_tree(tree,subtree,addsubtree);
            break;
        }
        subtree=NULL;
        if (m_parameters(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    if (foundsolution==0) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_callmatch(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="callmatch";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'<')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_callmatch_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_token_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_callmatch(lret,&subtree) || m_andlabels(lret,&subtree) || m_orlabels(lret,&subtree) || m_label(lret,&subtree) || m_lemma(lret,&subtree) || m_word(lret,&subtree) || m_any(lret,&subtree) || m_meta(lret,&subtree) || m_metas(lret,&subtree) || m_rgx(lret,&subtree) || m_punct(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_token_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_kleene(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    lreturn+=lret;
    return(1);
}


char bnf_tamgu::m_token(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="token";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_token_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_token_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_listoftokens_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,',')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_annotation(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_listoftokens_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //CONSTRAINT
    int foundsolution=0;
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (x_test_char(lret,'}')) {
            foundsolution=1;
            x_init_tree(tree,subtree,addsubtree);
            break;
        }
        subtree=NULL;
        if (m_listoftokens_0_1_2(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    if (foundsolution==0) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_listoftokens_0_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_kleene(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_listoftokens(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="listoftokens";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'{')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_annotation(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_listoftokens_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_listoftokens_0_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_sequenceoftokens_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //CONSTRAINT
    int foundsolution=0;
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (x_test_char(lret,']')) {
            foundsolution=1;
            x_init_tree(tree,subtree,addsubtree);
            break;
        }
        subtree=NULL;
        if (m_listoftokens_0_1_2(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    if (foundsolution==0) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_sequenceoftokens(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="sequenceoftokens";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'[')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_annotation(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_sequenceoftokens_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_listoftokens_0_3(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_optionaltokens_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //CONSTRAINT
    int foundsolution=0;
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (x_test_char(lret,')')) {
            foundsolution=1;
            x_init_tree(tree,subtree,addsubtree);
            break;
        }
        subtree=NULL;
        if (m_listoftokens_0_1_2(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    if (foundsolution==0) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_optionaltokens(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="optionaltokens";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'(')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_annotation(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_optionaltokens_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_removetokens_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //CONSTRAINT
    int foundsolution=0;
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (x_test_char(lret,'<')) {
            foundsolution=1;
            x_init_tree(tree,subtree,addsubtree);
            break;
        }
        subtree=NULL;
        if (m_listoftokens_0_1_2(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    if (foundsolution==0) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_removetokens(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="removetokens";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'>')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_annotation(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_removetokens_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_annotation_0_1_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_non(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_annotation_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_annotation_0_1_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_listoftokens(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_annotation_0_3(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_annotation_0_1_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_token(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_annotation(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="annotation";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_annotation_0_1(lret,&subtree) || m_sequenceoftokens(lret,&subtree) || m_optionaltokens(lret,&subtree) || m_removetokens(lret,&subtree) || m_annotation_0_3(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}


char bnf_tamgu::m_ruletype(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="ruletype";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_in(lret,tab15))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    x_init_node(lreturn,lret,tree,label,pos,true);
    return(1);
}


char bnf_tamgu::m_annotationrule_0_1(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    //long pos=currentpos;
    //int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    //bool exitonfail=false;
    //BODYOPTIONAL
    subtree=NULL;
    if (m_ruletype(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else
        lret="";
    return(1);
}


char bnf_tamgu::m_annotationrule_0_2(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (m_range_0_1_3_4(lret,&subtree) || x_test_string(lret,varstring65))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_annotationrule_0_4_6_7(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_char(lret,'|')) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_annotation(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (x_test_dot(lret)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_annotationrule_0_4_6(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYOR
    subtree=NULL;
    if (x_test_char(lret,'.') || m_annotationrule_0_4_6_7(lret,&subtree))
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return 0;
    }
    return(1);
}


char bnf_tamgu::m_annotationrule_0_4(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //CONSTRAINT
    int foundsolution=0;
    while (currentpos<fx->stack.size()) {
        subtree=NULL;
        if (m_annotationrule_0_4_6(lret,&subtree)) {
            foundsolution=1;
            x_init_tree(tree,subtree,addsubtree);
            break;
        }
        subtree=NULL;
        if (m_listoftokens_0_1_2(lret,&subtree))
            x_init_tree(tree,subtree,addsubtree);
        else
            break;
    }
    if (foundsolution==0) {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    return(1);
}


char bnf_tamgu::m_annotationrule(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    static const char* label="annotationrule";
    string lret;
    long pos=currentpos;
    int itok=intoken;
    x_node* subtree=NULL;
    int addsubtree=0;
    bool exitonfail=false;
    //BODYSEQUENCE
    subtree=NULL;
    if (m_annotationrule_0_1(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_word(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_annotationrule_0_2(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    exitonfail=true;
    initfail(label,currentpos,66);
    bnf_tamgu_pop pop0(this);
    //BODYSEQUENCE
    subtree=NULL;
    if (m_annotation(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    //BODYSEQUENCE
    subtree=NULL;
    if (m_annotationrule_0_4(lret,&subtree)) 
        x_init_tree(tree,subtree,addsubtree);
    else {
        x_pop_node(tree,addsubtree);
        currentpos=pos;
        intoken=itok;
        setfail(exitonfail);
        return(0);
    }
    x_init_node(lreturn,lret,tree,label,pos,false);
    return(1);
}




string bnf_tamgu::x_errormsg(long i) {
    return errorlabels[i];
}


x_node* bnf_tamgu::x_parsing(tokenizer_result<string>* xr,x_parsing_mode mode,bool display) {
    fx=xr;
    string lret;
    x_node* tree=new x_node;
    gFail=0;
    lineerror=-1;
    labelerror="";
    errornumber=-1;
    currentpos=0;
    intoken=0;
    labelerrors.clear();
    lineerrors.clear();
    errornumbers.clear();

    char res=m_analyse(lret,&tree);

    if (currentpos!=fx->stack.size() && mode==FULL) {
        if (display) {
            if (errornumber!=-1)
                cerr<<endl<<"Error line:"<<lineerror<<" => "<<errorlabels[errornumber];
            else
                cerr<<endl<<"Error line:"<<lineerror;
        }
        if (tree!=NULL)
            delete tree;
        return NULL;
    }

    if (res==1)
        return tree;

    delete tree;
    return NULL;
}

x_node* bnf_tamgu::x_call_again(tokenizer_result<string>* xr,x_parsing_mode mode,bool display) {
    fx=xr;
    string lret;
    x_node* tree=new x_node;
    gFail=0;
    lineerror=-1;
    labelerror="";
    errornumber=-1;
    intoken=0;
    labelerrors.clear();
    lineerrors.clear();
    errornumbers.clear();

    char res=m_analyse(lret,&tree);

    if (currentpos!=fx->stack.size() && mode==FULL) {
        if (display) {
            if (errornumber!=-1)
                cerr<<endl<<"Error line:"<<lineerror<<" => "<<errorlabels[errornumber];
            else
                cerr<<endl<<"Error line:"<<lineerror;
        }
        if (tree!=NULL)
            delete tree;
        return NULL;
    }

    if (res==1)
        return tree;

    delete tree;
    return NULL;
}


char bnf_tamgu::x_test_dot(string& lret) {
    if (currentpos>=fx->stack.size())
        return(0);
    if (fx->stack[currentpos]==".") {
        incrementpos();
        lret+=".";
        return 1;
    }
    return 0;
}


char bnf_tamgu::x_test_string(string& lret,string sub) {
    if (currentpos>=fx->stack.size())
        return(0);
    if (fx->stack[currentpos]==sub) {
        incrementpos();
        lret+=sub;
        return 1;
    }
    return 0;
}


char bnf_tamgu::m_punct(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    if (currentpos>=fx->stack.size())
        return(0);
    static const char* label="punct";
    if (fx->stacktype[currentpos]==0) {
        x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
        incrementpos();
        return(1);
    }
    return(0);
}

char bnf_tamgu::m_word(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    if (currentpos>=fx->stack.size())
        return(0);
    static const char* label="word";
    if (fx->stacktype[currentpos]==4) {
        x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
        incrementpos();
        return(1);
    }
    return(0);
}

char bnf_tamgu::m_typename(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    if (currentpos>=fx->stack.size())
        return(0);
    static const char* label="typename";
    if (fx->stacktype[currentpos]==4) {
        x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
        incrementpos();
        return(1);
    }
    return(0);
}

char bnf_tamgu::m_astring(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    if (currentpos>=fx->stack.size())
        return(0);
    static const char* label="astring";
    if (fx->stacktype[currentpos]==4) {
        x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
        incrementpos();
        return(1);
    }
    return(0);
}

char bnf_tamgu::m_anumber(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    if (currentpos>=fx->stack.size())
        return(0);
    static const char* label="anumber";
    if (fx->stacktype[currentpos]==3) {
        x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
        incrementpos();
        return(1);
    }
    if ((fx->stack[currentpos]=="-" || fx->stack[currentpos]=="+") && fx->stacktype[currentpos+1]==3) {
        string s=fx->stack[currentpos];
        s+=fx->stack[currentpos+1];
        x_init_node(lreturn,s,tree,label,currentpos,true);
        currentpos+=2;
        return(1);
    }
    return(0);
}

char bnf_tamgu::m_astringsimple(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    if (currentpos>=fx->stack.size())
        return(0);
    if (fx->stacktype[currentpos]==2) {
        static const char* label="astringsimple";
        x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
        incrementpos();
        return(1);
    }
    if (fx->stacktype[currentpos]==7) {
        static const char* label="ustringsimple";
        x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
        incrementpos();
        return(1);
    }
    return(0);
}

char bnf_tamgu::m_rgx(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    if (currentpos>=fx->stack.size())
        return(0);
    if (fx->stacktype[currentpos]==2) {
        static const char* label="rgx";
        x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
        incrementpos();
        return(1);
    }
    if (fx->stacktype[currentpos]==7) {
        static const char* label="ugx";
        x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
        incrementpos();
        return(1);
    }
    return(0);
}

char bnf_tamgu::m_astringdouble(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    if (currentpos>=fx->stack.size())
        return(0);
    if (fx->stacktype[currentpos]==1) {
        static const char* label="astringdouble";
        x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
        incrementpos();
        return(1);
    }
    if (fx->stacktype[currentpos]==6) {
        static const char* label="ustringdouble";
        x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
        incrementpos();
        return(1);
    }
    return(0);
}

char bnf_tamgu::m_metas(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    if (currentpos>=fx->stack.size())
        return(0);
    if (fx->stacktype[currentpos]==1) {
        static const char* label="metas";
        x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
        incrementpos();
        return(1);
    }
    if (fx->stacktype[currentpos]==6) {
        static const char* label="uetas";
        x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
        incrementpos();
        return(1);
    }
    return(0);
}

char bnf_tamgu::m_afullstring(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    if (currentpos>=fx->stack.size())
        return(0);
    if (fx->stacktype[currentpos]==5) {
        static const char* label="afullstring";
        x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
        incrementpos();
        return(1);
    }
    if (fx->stacktype[currentpos]==8) {
        static const char* label="ufullstring";
        x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
        incrementpos();
        return(1);
    }
    return(0);
}

char bnf_tamgu::m_atreg(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    if (currentpos>=fx->stack.size())
        return(0);
    static const char* label="atreg";
    if (fx->stacktype[currentpos]==9) {
        x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
        incrementpos();
        return(1);
    }
    return(0);
}

char bnf_tamgu::m_astreg(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    if (currentpos>=fx->stack.size())
        return(0);
    static const char* label="astreg";
    if (fx->stacktype[currentpos]==10) {
        x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
        incrementpos();
        return(1);
    }
    return(0);
}

char bnf_tamgu::m_apreg(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    if (currentpos>=fx->stack.size())
        return(0);
    static const char* label="apreg";
    if (fx->stacktype[currentpos]==11) {
        x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
        incrementpos();
        return(1);
    }
    return(0);
}

char bnf_tamgu::m_aspreg(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    if (currentpos>=fx->stack.size())
        return(0);
    static const char* label="aspreg";
    if (fx->stacktype[currentpos]==12) {
        x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
        incrementpos();
        return(1);
    }
    return(0);
}

char bnf_tamgu::m_fstring(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    if (currentpos>=fx->stack.size())
        return(0);
    static const char* label="fstring";
    if (fx->stacktype[currentpos]==13) {
        x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
        incrementpos();
        return(1);
    }
    return(0);
}

char bnf_tamgu::m_festring(string& lreturn,x_node** tree) {
    if (gFail==1) return(0);
    if (currentpos>=fx->stack.size())
        return(0);
    static const char* label="festring";
    if (fx->stacktype[currentpos]==14) {
        x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
        incrementpos();
        return(1);
    }
    return(0);
}
