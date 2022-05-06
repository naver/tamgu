/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : pdf.cxx
 Date       : 10/02/2020
 Purpose    : PDF creator (based on http://www.vulcanware.com/cpp_pdf/index.html)
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#include "tamgu.h"
#include <complex>
#include <cmath>
#include "tamguversion.h"
#include "tamgusvector.h"
#include "pdf.h"
#include "tamgupdf.h"

/**************************
 * Using Declarations
 **************************/

using std::ostringstream;
using std::ifstream;
using std::complex;
using std::ios;


    //We need to declare once again our local definitions.
basebin_hash<pdfMethod> Tamgupdf::methods;

short Tamgupdf::idtype = 0;

    //------------------------------------------------------------------------------------------------------------------
    //If you need to implement an external library... Otherwise remove it...
    //When Tamgu (탐구) loads a library, it looks for this method in particular, which then make this object available into Tamgu (탐구)
extern "C" {
Exporting bool tamgupdf_InitialisationModule(TamguGlobal* global, string version) {
    if (version != TamguVersion())
        return false;
    
    global->Update();
    
    return Tamgupdf::InitialisationModule(global, version);
}
}
    //------------------------------------------------------------------------------------------------------------------

    //MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgupdf::AddMethod(TamguGlobal* global, string name, pdfMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamgupdf::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamgupdf::idtype = global->Getid("pdf");
}


bool Tamgupdf::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    
    
        //Each new object has a specific name, which will help recognize it in the code that will exploit pdf...
    Tamgupdf::idtype = global->Getid("pdf");
    
        //You declare your methods here:
        // Argument 1 is a call to global, which will record your method into Tamgu (탐구)
        // Argument 2 is the name of your command
        // Argument 3 links your command with its implementation into the object class
        // Argument 4 is a combination of P_... which defines how many parameters this function can accept.
        //Argument 5 is a description of this method.
    Tamgupdf::AddMethod(global, "save", &Tamgupdf::MethodSave, P_ONE, "save(string filename): save pdf.");
    Tamgupdf::AddMethod(global, "showtextxy", &Tamgupdf::MethodshowTextXY, P_THREE, "showtextxy(string text, int x, int y): showTextXY.");
    Tamgupdf::AddMethod(global, "showtexthexaxy", &Tamgupdf::MethodshowTextHexaXY, P_THREE, "showtexthexaxy(string text, int x, int y): showTextHexaXY.");
    Tamgupdf::AddMethod(global, "rightjustifytextxy", &Tamgupdf::MethodrightJustifyTextXY, P_THREE, "rightjustifytextxy(string text, int x, int y): rightJustifyTextXY.");
    Tamgupdf::AddMethod(global, "wraptext", &Tamgupdf::MethodwrapText, P_THREE, "wraptext(string text, int maxwidth, bool rightjustify): wrapText.");
    Tamgupdf::AddMethod(global, "setlinewidth", &Tamgupdf::MethodsetLineWidth, P_ONE, "setlinewidth(int value): setLineWidth.");
    Tamgupdf::AddMethod(global, "drawline", &Tamgupdf::MethoddrawLine, P_FOUR, "drawline(int x0, int y0, int x1, int y1): drawLine.");
    Tamgupdf::AddMethod(global, "drawrect", &Tamgupdf::MethoddrawRect, P_FOUR, "drawrect(int x, int y, int width, int height): drawRect.");
    Tamgupdf::AddMethod(global, "fillrect", &Tamgupdf::MethodfillRect, P_FOUR, "fillrect(int x, int y, int width, int height): fillRect.");
    Tamgupdf::AddMethod(global, "drawpolygon", &Tamgupdf::MethoddrawPolygon, P_ONE, "drawpolygon(vector points): drawPolygon.");
    Tamgupdf::AddMethod(global, "fillpolygon", &Tamgupdf::MethodfillPolygon, P_ONE, "fillpolygon(vector points): fillPolygon.");
    Tamgupdf::AddMethod(global, "drawellipse", &Tamgupdf::MethoddrawEllipse, P_FOUR, "drawellipse(int xcenter, int ycenter, int xradius, int yradius): drawEllipse.");
    Tamgupdf::AddMethod(global, "fillellipse", &Tamgupdf::MethodfillEllipse, P_FOUR, "fillellipse(int xcenter, int ycenter, int xradius, int yradius): fillEllipse.");
    Tamgupdf::AddMethod(global, "drawcircle", &Tamgupdf::MethoddrawCircle, P_THREE, "drawcircle(int xcenter, int ycenter, int radius): drawCircle.");
    Tamgupdf::AddMethod(global, "fillcircle", &Tamgupdf::MethodfillCircle, P_THREE, "fillcircle(int xcenter, int ycenter, int radius): fillCircle.");
    Tamgupdf::AddMethod(global, "setlinecolor", &Tamgupdf::MethodsetLineColor, P_THREE, "setlinecolor(byte red, byte green, byte blue): setLineColor.");
    Tamgupdf::AddMethod(global, "setfillcolor", &Tamgupdf::MethodsetFillColor, P_THREE, "setfillcolor(byte red, byte green, byte blue): setFillColor.");
    Tamgupdf::AddMethod(global, "setfont", &Tamgupdf::MethodSetfont, P_TWO, "setfont(string name, int size): setFillColor.");
    Tamgupdf::AddMethod(global, "newpage", &Tamgupdf::MethodNewPage, P_NONE, "newpage(): new page.");
    Tamgupdf::AddMethod(global, "width", &Tamgupdf::MethodWidth, P_NONE, "width(): return width.");
    Tamgupdf::AddMethod(global, "height", &Tamgupdf::MethodHeight, P_NONE, "height(): return height.");
    Tamgupdf::AddMethod(global, "setencoding", &Tamgupdf::MethodSetencoding, P_ONE, "setencoding(string e): set the encoding.");
    
        //We need this code, in order to create new instances of our pdf object... DO NOT ALTER
    global->newInstance[Tamgupdf::idtype] = new Tamgupdf(global);
    global->RecordCompatibilities(Tamgupdf::idtype);
    
    return true;
}

Tamgu* Tamgupdf::Put(Tamgu* idx, Tamgu* kval, short idthread) {
        //If you want to put some element into your object
    return aTRUE;
}

Tamgu* Tamgupdf::Eval(Tamgu* context, Tamgu* idx, short idthread) {
        //if your object is called from within an expression...
    return this;
}


Tamgu* Tamgupdf::MethodSave(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string filename = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    string errMsg;
    
    if(!writeToFile(filename, errMsg))
        return globalTamgu->Returnerror(errMsg, idthread);
    
    return aTRUE;
}

Tamgu* Tamgupdf::MethodWidth(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (!ready)
        return globalTamgu->Returnerror("Select a font first", idthread);
    
    return globalTamgu->Provideint(getWidth());
}

Tamgu* Tamgupdf::MethodHeight(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (!ready)
        return globalTamgu->Returnerror("Select a font first", idthread);
    
    return globalTamgu->Provideint(getHeight());
}

Tamgu* Tamgupdf::MethodNewPage(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    newPage();
    return aTRUE;
}

Tamgu* Tamgupdf::MethodSetfont(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    inittableutf8();
    
    string name = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    int sz = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    
    ready = true;
    
    name = s_to_lower(name);
    
    if (name == "courier") {
        setFont(PDF::COURIER, sz);
        return aTRUE;
    }
    
    if (name == "courier_bold") {
        setFont(PDF::COURIER_BOLD, sz);
        return aTRUE;
    }
    
    if (name == "courier_oblique") {
        setFont(PDF::COURIER_OBLIQUE, sz);
        return aTRUE;
    }
    
    if (name == "courier_bold_oblique") {
        setFont(PDF::COURIER_BOLD_OBLIQUE, sz);
        return aTRUE;
    }
    
    if (name == "helvetica") {
        setFont(PDF::HELVETICA, sz);
        return aTRUE;
    }
    
    if (name == "helvetica_bold") {
        setFont(PDF::HELVETICA_BOLD, sz);
        return aTRUE;
    }
    
    if (name == "helvetica_oblique") {
        setFont(PDF::HELVETICA_OBLIQUE, sz);
        return aTRUE;
    }
    
    if (name == "helvetica_bold_oblique") {
        setFont(PDF::HELVETICA_BOLD_OBLIQUE, sz);
        return aTRUE;
    }
    
    if (name == "symbol") {
        setFont(PDF::SYMBOL, sz);
        return aTRUE;
    }
    
    if (name == "times") {
        setFont(PDF::TIMES, sz);
        return aTRUE;
    }
    
    if (name == "times_bold") {
        setFont(PDF::TIMES_BOLD, sz);
        return aTRUE;
    }
    
    if (name == "times_italic") {
        setFont(PDF::TIMES_ITALIC, sz);
        return aTRUE;
    }
    
    if (name == "tiles_bold_italic") {
        setFont(PDF::TIMES_BOLD_ITALIC, sz);
        return aTRUE;
    }
    
    if (name == "zapf_dingbats") {
        setFont(PDF::ZAPF_DINGBATS, sz);
        return aTRUE;
    }
    
    ready = false;
    return globalTamgu->Returnerror("Unknown font name", idthread);
}

Tamgu* Tamgupdf::MethodshowTextXY(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring text = callfunc->Evaluate(0, contextualpattern, idthread)->UString();
    int x = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    int y = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
    char buffer[10];
    string res = "";
    
    wchar_t c;
    uint32_t v;
    for (long i = 0; i < text.size(); i++) {
        c =  text[i];
        if (c > 127) {
            sprintf_s(buffer, 10, "U+%04X", c);
            res += c;
        }
        else
            res += c;
    }
    showTextXY(res,x,y);
    return aTRUE;
}

Tamgu* Tamgupdf::MethodSetencoding(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string text = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    setEncoding(text);
    return aTRUE;
}

Tamgu* Tamgupdf::MethodshowTextHexaXY(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring text = callfunc->Evaluate(0, contextualpattern, idthread)->UString();
    int x = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    int y = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
    char buffer[10];
    string res = "";
    wchar_t c;
    for (long i = 0; i < text.size(); i++) {
        if (i)
            res += ' ';
        c =  text[i];
        sprintf_s(buffer,10,"%X",c);
        res += buffer;
    }
    showTextHexaXY(res,x,y);
    return aTRUE;
}

Tamgu* Tamgupdf::MethodrightJustifyTextXY(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (!ready)
        return globalTamgu->Returnerror("Select a font first", idthread);
    
    string text = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    int x = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    int y = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
    rightJustifyTextXY(text,x,y);
    return aTRUE;
}

Tamgu* Tamgupdf::MethodwrapText(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (!ready)
        return globalTamgu->Returnerror("Select a font first", idthread);
    
    string text = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    int maxwidth = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    bool rightjustify = callfunc->Evaluate(2, contextualpattern, idthread)->Boolean();
    Tamgusvector* vs = (Tamgusvector*)Selectasvector(contextualpattern);
    vs->values = wrapText(text,maxwidth,rightjustify);
    return vs;
}

Tamgu* Tamgupdf::MethodsetLineWidth(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    int value = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    setLineWidth(value);
    return aTRUE;
}

Tamgu* Tamgupdf::MethoddrawLine(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    int x0 = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    int y0 = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    int x1 = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
    int y1 = callfunc->Evaluate(3, contextualpattern, idthread)->Integer();
    drawLine(x0,y0,x1,y1);
    return aTRUE;
}

Tamgu* Tamgupdf::MethoddrawRect(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    int x = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    int y = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    int width = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
    int height = callfunc->Evaluate(3, contextualpattern, idthread)->Integer();
    drawRect(x,y,width,height);
    return aTRUE;
}

Tamgu* Tamgupdf::MethodfillRect(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    int x = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    int y = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    int width = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
    int height = callfunc->Evaluate(3, contextualpattern, idthread)->Integer();
    fillRect(x,y,width,height);
    return aTRUE;
}

Tamgu* Tamgupdf::MethoddrawPolygon(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* tamgupoints = callfunc->Evaluate(0, contextualpattern, idthread);
    vector<XY> points;
    long sz = tamgupoints->Size();
    if ((sz%2) == 1)
        return globalTamgu->Returnerror("Vector should contain an odd number of values", idthread);
    for (long i = 0; i < sz; i+= 2) {
        XY v(tamgupoints->getinteger(i),tamgupoints->getinteger(i+1));
        points.push_back(v);
    }
    drawPolygon(points);
    return aTRUE;
}

Tamgu* Tamgupdf::MethodfillPolygon(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* tamgupoints = callfunc->Evaluate(0, contextualpattern, idthread);
    vector<XY> points;
    long sz = tamgupoints->Size();
    if ((sz%2) == 1)
        return globalTamgu->Returnerror("Vector should contain an odd number of values", idthread);
    for (long i = 0; i < sz; i+= 2) {
        XY v(tamgupoints->getinteger(i),tamgupoints->getinteger(i+1));
        points.push_back(v);
    }
    fillPolygon(points);
    return aTRUE;
}

Tamgu* Tamgupdf::MethoddrawEllipse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    int xcenter = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    int ycenter = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    int xradius = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
    int yradius = callfunc->Evaluate(3, contextualpattern, idthread)->Integer();
    drawEllipse(xcenter,ycenter,xradius,yradius);
    return aTRUE;
}

Tamgu* Tamgupdf::MethodfillEllipse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    int xcenter = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    int ycenter = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    int xradius = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
    int yradius = callfunc->Evaluate(3, contextualpattern, idthread)->Integer();
    fillEllipse(xcenter,ycenter,xradius,yradius);
    return aTRUE;
}

Tamgu* Tamgupdf::MethoddrawCircle(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    int xcenter = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    int ycenter = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    int radius = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
    drawCircle(xcenter,ycenter,radius);
    return aTRUE;
}

Tamgu* Tamgupdf::MethodfillCircle(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    int xcenter = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    int ycenter = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
    int radius = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
    fillCircle(xcenter,ycenter,radius);
    return aTRUE;
}

Tamgu* Tamgupdf::MethodsetLineColor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    uchar red = callfunc->Evaluate(0, contextualpattern, idthread)->Byte();
    uchar green = callfunc->Evaluate(1, contextualpattern, idthread)->Byte();
    uchar blue = callfunc->Evaluate(2, contextualpattern, idthread)->Byte();
    setLineColor(red,green,blue);
    return aTRUE;
}

Tamgu* Tamgupdf::MethodsetFillColor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    uchar red = callfunc->Evaluate(0, contextualpattern, idthread)->Byte();
    uchar green = callfunc->Evaluate(1, contextualpattern, idthread)->Byte();
    uchar blue = callfunc->Evaluate(2, contextualpattern, idthread)->Byte();
    setFillColor(red,green,blue);
    return aTRUE;
}

