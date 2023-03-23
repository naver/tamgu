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
#include "tokens.h"

void getdefaultrules(vector<string>& rules);

class x_forindent : public tokenizer_automaton {
public:
    
    
    //We keep all characters...
    void setrules() {
        
        //Spaces, skipped in the parsing string
        rules.push_back(U"%S=0");                         //0     space (not kept)
        
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
        rules.push_back(U"//?*%r=9");                  //30    comments starting with // with no carriage return (CR) inside (not kept)
        rules.push_back(U"/@?*@/=9");                   //32    long comments starting with /@ and finishing with @/ (not kept)
        rules.push_back(U"/=0");                         //34    /
        
        //Strings
        //Double quote
        rules.push_back(U"\"{[\\-\"] ~%r}*\"=:1");     //string "" does not contain CR and can escape characters

        //Single quote
        rules.push_back(U"'~%r*'=:2");                    //38    string '' does not contain CR and does not process escape characters
        
        //Long quotes
        rules.push_back(U"@\"?*\"@=5");               //40    string @" "@ can contain CR and escape characters (we do not keep the @s)
        
        //tamgu regular expression strings
        rules.push_back(U"r\"~%r+\"=2");              //42    string r"" tamgu regular expression (we do not keep the r in the parse)
        rules.push_back(U"r'~%r+'=2");               //42    string r"" tamgu regular expression (we do not keep the r in the parse)
        rules.push_back(U"p\"~%r+\"=2");             //42    string p"" tamgu posix expression (we do not keep the p in the parse)
        rules.push_back(U"p'~%r+'=2");               //42    string p"" tamgu posix expression (we do not keep the p in the parse)

        //Unicode double quote strings
        rules.push_back(U"u\"{[\\-\"] ~%r}*\"=1");     //string "" does not contain CR and can escape characters
        
        //Unicode single quote strings
        rules.push_back(U"u'~%r*'=7");                //44    string u'' unicode string
        
        //Unicode long quote strings
        rules.push_back(U"u@\"?+\"@=8");             //46    string u@".."@ unicode string
        
        rules.push_back(U"#27%[{;%d}+m=0");
        
        rules.push_back(U"0b{1 0}+=3");                       //binary numbers
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
    
};

//------------------------------------------------------------------------------------------------Coloring tokenizer rules

class x_coloringrule : public tokenizer_automaton {
public:
    
    void setrules() {
            //spaces
        rules.push_back(U"%S=#");                         //0     space
        
        rules.push_back(U"#27%[{%d;}+m=#");        // color detection

            //Fast tracks for recurrent punctuations
        //Fast tracks for recurrent punctations
        rules.push_back(U";=#");                         //4     ;
        rules.push_back(U",=#");                         //5     ,
        rules.push_back(U"==#");                         //6     =
        rules.push_back(U"!=#");                         //8     !

        //Special characters that need to be escaped
        rules.push_back(U"%)=#");                         //10    )
        rules.push_back(U"%[=#");                         //11    [
        rules.push_back(U"%]=#");                         //12    ]
        rules.push_back(U"%{=#");                         //13    {
        rules.push_back(U"%}=#");                         //14    }
        rules.push_back(U"%%=#");                         //21    %
        rules.push_back(U"%+=#");                         //18    +
        rules.push_back(U"%-=#");                         //19    -
        rules.push_back(U"%*=#");                         //20    *
        rules.push_back(U"%~=#");                         //7     ~

        rules.push_back(U"..=#");                        //15    ..
        rules.push_back(U".=#");                         //16    .
        rules.push_back(U"^=#");                         //17    ^
        rules.push_back(U">=#");                         //23    >
        rules.push_back(U"|=#");                         //24    |
        rules.push_back(U"&=#");                         //25    &
        rules.push_back(U":=#");                         //26    :
        rules.push_back(U"\\=#");                        //29    ?

        rules.push_back(U"${%d %a %H}+=10");             //24    $%d+
        rules.push_back(U"$=#");                         //25    $
        
        rules.push_back(U"%#{%d %a %H}+=10");             //26    #label
        rules.push_back(U"%#=#");                         //27    #
        
        rules.push_back(U"%?{%a %d %H}+=10");             //28    ?label
        rules.push_back(U"%?=#");                         //29    ?
        
            // Characters that we need
        rules.push_back(U"<%a{%a %d %H}+=13");           //30    <label
        rules.push_back(U"<=#");                         //19    <
        rules.push_back(U".{%a %d %H}+%(=11");            //30    .label(
        rules.push_back(U".=#");                         //31    .
        rules.push_back(U"%(=#");                         //32    (
        
            //Comments
        //Comments
        rules.push_back(U"//?*%r=5");                  //30    comments starting with // with no carriage return (CR) inside (not kept)
        rules.push_back(U"/@?*@/=5");                   //32    long comments starting with /@ and finishing with @/ (not kept)
        rules.push_back(U"/=#");                         //34    /
        
            //Strings
            //Double quotes
        //Strings
        //Double quote
        rules.push_back(U"\"{[\\-\"] ~%r}*\"=:1");     //string "" does not contain CR and can escape characters

        //Single quote
        rules.push_back(U"'~%r*'=:2");                    //38    string '' does not contain CR and does not process escape characters
        
        //Long quotes
        rules.push_back(U"@\"?*\"@=3");               //40    string @" "@ can contain CR and escape characters (we do not keep the @s)
        rules.push_back(U"@{%a %d}+=14");                 //43    annotation rule head

        //tamgu regular expression strings
        rules.push_back(U"r\"~%r+\"=2");              //42    string r"" tamgu regular expression (we do not keep the r in the parse)
        rules.push_back(U"r'~%r+'=2");               //42    string r"" tamgu regular expression (we do not keep the r in the parse)
        rules.push_back(U"p\"~%r+\"=2");             //42    string p"" tamgu posix expression (we do not keep the p in the parse)
        rules.push_back(U"p'~%r+'=2");               //42    string p"" tamgu posix expression (we do not keep the p in the parse)

        //Unicode double quote strings
        rules.push_back(U"u\"{[\\-\"] ~%r}*\"=1");     //string "" does not contain CR and can escape characters
        
        //Unicode single quote strings
        rules.push_back(U"u'~%r*'=2");                //44    string u'' unicode string
        
        //Unicode long quote strings
        rules.push_back(U"u@\"?+\"@=3");             //46    string u@".."@ unicode string

        
        rules.push_back(U"0b{1 0}+=0");                       //binary numbers
        rules.push_back(U"1:{%d #A-F #a-f}");            //2     metarule on 1, for hexadecimal digits
        rules.push_back(U"0x%1+(.%1+)({pP}({%- %+})%d+)=0");  //47 hexadecimal: can handle 0x1.16bca4f9165dep-3
        rules.push_back(U"%d+(.%d+)({eE}({%- %+})%d+)=0");    //48    exponential digits

            //Rules start here
        rules.push_back(U"{%a %d}+%(=12");                //52    label(
        rules.push_back(U"{%a %d %H}+=4");               //53    label
        rules.push_back(U"%n=#");                        //52    non-breaking space (not kept)
        rules.push_back(U"%o=#");                        //55    operators
        rules.push_back(U"%p=#");                        //54    punctuation
        rules.push_back(U"~{%S %p %o}+=4");            //56    An unknown token separated with spaces or punctuation...
        rules.push_back(U"%=#");                         //56    An unknown character...
    }
};
#endif

