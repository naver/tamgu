/*
*  Tamgu (탐구)
    *
* Copyright 2019-present NAVER Corp.
* under BSD 3-clause
*/
/* --- CONTENTS ---
Project    : Tamgu (탐구)
    Version    : See tamgu.cxx for the version number
filename   : pdf.h
Date       : 2017/09/01
Purpose    :
Programmer : Claude ROUX (claude.roux@naverlabs.com)
    Reviewer   :
*/

#ifndef tamgupdf_h
#define tamgupdf_h

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgupdf;
//This typedef defines a type "pdfMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgupdf::*pdfMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------

class Tamgupdf : public TamguObject, PDF {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static basebin_hash<pdfMethod> methods;
    static hmap<string, string> infomethods;
    static bin_hash<unsigned long> exported;

    static short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    bool ready;
    //---------------------------------------------------------------------------------------------------------------------
    Tamgupdf(TamguGlobal* g = NULL, Tamgu* parent = NULL) : TamguObject(g, parent) {
        //Do not forget your variable initialisation
        ready = false;
    }

    //----------------------------------------------------------------------------------------------------------------------
    Tamgu* Put(Tamgu* v, Tamgu* i, short idthread);

    Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread);


    short Type() {
        return Tamgupdf::idtype;
    }

    void Setidtype(TamguGlobal* global);

    string Typename() {
        return "pdf";
    }

    //The next two functions work in conjunction...
    //If your object is very atomic (such as a number or a string)
    //Then when it is passed to a function or stored into a container,
    //it might be worth duplicating it.
    // In that case: duplicateForCall should return true...

    //However if your object is complex and probably unique through out the code
    //Then duplicateForCall should return false, and Atom should always reduce to a "return this;" only...
    Tamgu* Atom(bool forced=false) {
        return this;
    }

    bool duplicateForCall() {
        return false;
    }

    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        if (exported.find(n) != exported.end())
            return true;
        return false;
    }

    Tamgu* Newinstance(short, Tamgu* f = NULL) {
        return new Tamgupdf;
    }

    static void AddMethod(TamguGlobal* g, string name, pdfMethod func, unsigned long arity, string infos);
    static bool InitialisationModule(TamguGlobal* g, string version);

    void Methods(Tamgu* v) {
        hmap<string, string>::iterator it;
        for (it = infomethods.begin(); it != infomethods.end(); it++)
            v->storevalue(it->first);
    }

    string Info(string n) {

        if (infomethods.find(n) != infomethods.end())
            return infomethods[n];
        return "Unknown method";
    }


    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is a sample  of a function that could be implemented for your needs.

    Tamgu* MethodSave(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodSetfont(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    Tamgu* MethodSetencoding(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    Tamgu* MethodshowTextXY(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodshowTextHexaXY(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodrightJustifyTextXY(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodwrapText(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    Tamgu* MethodsetLineWidth(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethoddrawLine(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    Tamgu* MethoddrawRect(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodfillRect(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    Tamgu* MethoddrawPolygon(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodfillPolygon(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    Tamgu* MethoddrawEllipse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodfillEllipse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    Tamgu* MethoddrawCircle(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodfillCircle(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    Tamgu* MethodsetLineColor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodsetFillColor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    Tamgu* MethodNewPage(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodWidth(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodHeight(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }

    string String() {
        return "pdf";
    }

    wstring UString() {
        return L"pdf";
    }



};
#endif

