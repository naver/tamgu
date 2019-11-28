/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgudebug.h
 Date       : 2017/09/01
 Purpose    : debug info class 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgudebug_h
#define tamgudebug_h

class Debuginfo {
    public:
    LockedThread loquet;

    string filename;
    string localvariables;
    string allvariables;
    string sstack;

    Tamgu* stacktop;

    long currentline;
    long lastfunction;
    long stackline;

    bool shortname;
    bool nextline;
    bool getintofunction;
    bool getoutfunction;
    bool running;
    bool displaymode;

    hmap<string, hmap<long, bool> > breakpoints;

    Debuginfo() {
        displaymode = true;
        stacktop=NULL;
        stackline=-1;
        currentline = -1;
        nextline = false;
        getintofunction = false;
        getoutfunction = false;
        lastfunction = -1;
        shortname = true;
        running = false;
    }

    void clearall() {
        stacktop=NULL;
        stackline=-1;
        currentline = -1;
        nextline = false;
        getintofunction = false;
        getoutfunction = false;
        lastfunction = -1;
        shortname = true;
        running = true;
    }

    void clear() {
        nextline = false;
        getintofunction = false;
        getoutfunction = false;
    }
    
    void wait() {
        loquet.Blocked();
    }
    
    void display(string msg) {
        if (displaymode)
            cout << msg << endl;
    }
    
    bool next() {
        if (executionbreak) {
            display("end of process (in)");
            return false;
        }
        nextline = true;
        loquet.Released();
        return true;
    }
    
    bool gotonext() {
        if (executionbreak) {
            display("end of process (next)");
            return false;
        }
        getintofunction = false;
        getoutfunction = false;
        nextline = false;
        loquet.Released();
        return true;
    }
    
    bool getin() {
        if (executionbreak) {
            display("end of process (in)");
            return false;
        }
        getintofunction = true;
        getoutfunction = false;
        nextline = true;
        loquet.Released();
        return true;
    }

    bool getout() {
        if (executionbreak) {
            display("end of process (out)");
            return false;
        }
        lastfunction = -1;
        getintofunction = false;
        getoutfunction = true;
        nextline = true;
        loquet.Released();
        return true;
    }

    bool stopexecution() {
        globalTamgu->Setdebugmode(false);
        globalTamgu->Setdebugfunction(NULL);

        if (executionbreak) {
            display("end of process (end)");
            return true;
        }
        clear();
        executionbreak = true;
        display("end of process (stop)");
        running = false;
        loquet.Released();
        return true;
    }

    bool gotoend() {
        if (executionbreak) {
            display("end of process (end)");
            return false;
        }
        clear();
        globalTamgu->Setdebugmode(false);
        globalTamgu->Setdebugfunction(NULL);
        running = false;
        loquet.Released();
        return true;
    }

    Tamgu* debugger(vector<Tamgu*>& stack, short idthread, void* data) {
        static ThreadLock _locker;
        
        Locking _lock(_locker);
        
        Tamgu* top=stack.back();
        
        
        short idcode = top->Currentspace();
        
        if (idcode != -1) {
            if (nextline) {
                if (stackline!=-1 && stackline<stack.size() && stack[stackline]==stacktop)
                    return aNULL;
                
                stackline=-1;
                stacktop=NULL;
                
                if (getoutfunction==true) {
                        //First we need to check if we are in a function...
                    getoutfunction=false;
                    for (long i=stack.size()-1;i>=0; i--) {
                        if (stack[i]->isaFunction()) {
                                //Then the previous element should be a call..
                            stacktop=stack[i];
                            stackline=i;
                            currentline=-1;
                            return aNULL;
                        }
                    }
                }
                else {
                    if (getintofunction==false && top->isaFunction()) {
                            //Then in this case, we need to jump over it...
                        stacktop=top;
                        stackline=stack.size()-1;
                        currentline=-1;
                        return aNULL;
                    }
                }
            }
            
            short fileid = top->Currentfile();
            filename = globalTamgu->Getfilename(fileid);
            
            currentline = top->Currentline();
            
            if (nextline || checkbreakpoint(filename)) {
                localvariables = "";
                allvariables = "";
                sstack = "";
                getintofunction=false;
                getoutfunction=false;
                globalTamgu->Getdebuginfo(localvariables, allvariables, sstack, shortname, 50, idthread);
                //When it returns aTRUE, we know that variables have been set...
                return aTRUE;
            }
        }
        
        globalTamgu->Cleardebug(idthread);
        return aFALSE;
    }
    
    void clearbreakpoints() {
        breakpoints.clear();
    }
    
    void addabreakpoint(string filename, long numline, char add) {
        if (add == 1)
            breakpoints[filename][numline] = true;
        else {
            if (breakpoints[filename].find(numline) != breakpoints[filename].end()) {
                breakpoints[filename].erase(numline);
                if (breakpoints[filename].size() == 0) {
                    breakpoints.erase(filename);
                }
            }
        }
    }
    
    bool checkbreakpoint(string& filename) {
        if (breakpoints.find(filename) != breakpoints.end() && breakpoints[filename].find(currentline) != breakpoints[filename].end())
            return true;
        return false;
    }
    
    bool shortdisplay() {
        shortname = 1-shortname;
        return shortname;
    }
};

#endif
