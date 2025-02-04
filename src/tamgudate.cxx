/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgudate.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgudate.h"
#include "atoms.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<dateMethod>  Tamgudate::methods;
static ThreadLock classlock;

Exporting short Tamgudate::idtype = 0;

static ThreadLock date_locker;

static void locking_date(Tamgu* c) {
    if (globalTamgu->threadMODE && c->hasLock())
        date_locker.lock->lock();
}

static void unlocking_date(Tamgu* c) {
    if (globalTamgu->threadMODE && c->hasLock())
        date_locker.lock->unlock();
}

Tamgudate::Tamgudate(time_t v)  {
    //Do not forget your variable initialisation
    locking_date(this);
    if (v == 0)
        time(&value);
    else
        value = v;
    unlocking_date(this);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgudate::AddMethod(TamguGlobal* global, string name, dateMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


Tamgu* Tamgudate::CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
    //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
    locking_date(this);
    Tamgu* val = (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    unlocking_date(this);
    return val;
}


void Tamgudate::Setidtype(TamguGlobal* global) {
  Locking lock(classlock);
  if (Tamgudate::methods.isEmpty())
    Tamgudate::InitialisationModule(global,"");
}


   bool Tamgudate::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    

    Tamgudate::idtype = global->Getid("date");

    Tamgudate::AddMethod(global, "_initial", &Tamgudate::MethodInitial, P_NONE | P_ONE | P_TWO | P_THREE | P_FOUR | P_FIVE | P_SIX | P_SEVEN, "_initial(): initialize a date.");
    Tamgudate::AddMethod(global, "setdate", &Tamgudate::MethodInitial, P_NONE | P_ONE | P_TWO | P_THREE | P_FOUR | P_FIVE | P_SIX | P_SEVEN, "setdate(y,m,d,h,m,s): Defines each of the elements of a date");
    Tamgudate::AddMethod(global, "date", &Tamgudate::MethodDate, P_NONE, "date(): the date itself");
    Tamgudate::AddMethod(global, "year", &Tamgudate::MethodYear, P_NONE | P_ONE, "year(int d): return the year or defines the year");
    Tamgudate::AddMethod(global, "month", &Tamgudate::MethodMonth, P_NONE | P_ONE, "month(int d): return the month or defines the month");
    Tamgudate::AddMethod(global, "day", &Tamgudate::MethodDay, P_NONE | P_ONE, "day(int d): return the day or defines the day");
    Tamgudate::AddMethod(global, "yearday", &Tamgudate::MethodYearday, P_NONE, "yearday(): return the number of the day in the year");
    Tamgudate::AddMethod(global, "weekday", &Tamgudate::MethodWeekday, P_NONE, "weekday(): return the day of the week");
    Tamgudate::AddMethod(global, "hour", &Tamgudate::MethodHour, P_NONE | P_ONE, "hour(int d): return the hour or defines the hour");
    Tamgudate::AddMethod(global, "minute", &Tamgudate::MethodMinute, P_NONE | P_ONE, "minute(int d): return the minute or defines the minute");
    Tamgudate::AddMethod(global, "format", &Tamgudate::MethodFormat, P_ONE, "format(string pattern): display the year with a C pattern");
    Tamgudate::AddMethod(global, "second", &Tamgudate::MethodSecond, P_NONE | P_ONE, "second(int d): return the second or defines the second");


    if (version != "") {
        global->newInstance[Tamgudate::idtype] = new Tamgudate(global);
        global->RecordCompatibilities(Tamgudate::idtype);
    }

    return true;
}

Tamgu* Tamgudate::Put(Tamgu* index, Tamgu* val, short idthread) {
    if (val->Type() == Tamgudate::idtype) {
        value = ((Tamgudate*)val)->value;
        return aTRUE;
    }
    
    if (val->Type() == a_ivector)
        return set_the_date((Tamguivector*)val);
    
    if (val->isString()) {
        string v = val->String();
        return setstringdate(v, idthread);
    }
        
    return aFALSE;
}

Tamgu* Tamgudate::Eval(Tamgu* context, Tamgu* index, short idthread) {
    return this;
}

string Tamgudate() {
    time_t value;

    time(&value);

    char buffer[100];
    #ifdef WIN32
    struct tm temps;
    localtime_s(&temps, &value);
    strftime(buffer, 100, "%Y/%m/%d %H:%M:%S", &temps);
    #else
    struct tm* temps = localtime(&value);
    strftime(buffer, 100, "%Y/%m/%d %H:%M:%S", temps);
    #endif
    return buffer;
}

string Tamgudate::String() {
    char buffer[100];
    locking_date(this);
    #ifdef WIN32
    struct tm temps;
    localtime_s(&temps, &value);
    strftime(buffer, 100, "%Y/%m/%d %H:%M:%S", &temps);
    #else
    struct tm* temps = localtime(&value);
    strftime(buffer, 100, "%Y/%m/%d %H:%M:%S", temps);
    #endif
    unlocking_date(this);
    return buffer;
}

void Tamgudate::Setstring(string& s, short idthread) {
    char buffer[100];
    locking_date(this);
    #ifdef WIN32
    struct tm temps;
    localtime_s(&temps, &value);
    strftime(buffer, 100, "%Y/%m/%d %H:%M:%S", &temps);
    #else
    struct tm* temps = localtime(&value);
    strftime(buffer, 100, "%Y/%m/%d %H:%M:%S", temps);
    #endif
    unlocking_date(this);
    s = buffer;
}

void Tamgudate::Setstring(wstring& s, short idthread) {
    char buffer[100];
    long sz;
    locking_date(this);
    #ifdef WIN32
    struct tm temps;
    localtime_s(&temps, &value);
    sz = strftime(buffer, 100, "%Y/%m/%d %H:%M:%S", &temps);
    #else
    struct tm* temps = localtime(&value);
    sz = strftime(buffer, 100, "%Y/%m/%d %H:%M:%S", temps);
    #endif
    unlocking_date(this);
    s=L"";
    for (long i = 0; i < sz; i++)
        s+= (wchar_t)buffer[i];
}


Tamgu* Tamgudate::setstringdate(string& v, short idthread) {
    vector<string> values;
    string dt;
    string mn;
    if (v.find(" ")) {
        v_split(v, " ", values);
        if (values.size() != 2)
            return globalTamgu->Returnerror("Error: Wrong date format", idthread);
        dt = values[0];
        mn = values[1];
    }
    else {
        if (v.find("/"))
            dt = v;
        else {
            if (v.find(":"))
                mn = v;
            else
                return globalTamgu->Returnerror("Error: Wrong date format", idthread);
        }
    }
    Tamguivector* iv = globalTamgu->Provideivector();
    if (dt.size()) {
        values.clear();
        v_split(dt, "/", values);
        if (values.size() != 3)
            return globalTamgu->Returnerror("Error: Wrong date format", idthread);
        for (int i = 0; i < values.size(); i++) {
            dt = values[i];
            iv->values.push_back(conversionintegerhexa(STR(dt)));
        }
    }
    else {
        iv->values.push_back(0);
        iv->values.push_back(0);
        iv->values.push_back(0);
    }
    if (mn.size()) {
        values.clear();
        v_split(mn, ":", values);
        if (values.size() != 3)
            return globalTamgu->Returnerror("Error: Wrong date format", idthread);
        for (int i = 0; i < values.size(); i++) {
            dt = values[i];
            iv->values.push_back(conversionintegerhexa(STR(dt)));
        }
    }
    else {
        iv->values.push_back(0);
        iv->values.push_back(0);
        iv->values.push_back(0);
    }
    return set_the_date(iv);
}

#ifdef WIN32
Tamgu* Tamgudate::set_the_date(Tamguivector* iv) {
    time_t x = 0;
    locking_date(this);
    struct tm temps;
    localtime_s(&temps, &x);
    
    int fulldate = 0;
    
    long sz = iv->values.size();
    if (!sz) {
        unlocking_date(this);
        return aFALSE;
    }
    
    int res = (int)iv->values[0];
    
    if (res > 0) {
        temps.tm_year = res - 1900;
        fulldate = 100;
    }
        
    //Month
    if (sz >= 2) {
        res = (int)iv->values[1];
        if (res > 0) {
            fulldate += 10;
            temps.tm_mon = res - 1;
        }
        if (sz >= 3) {
            res = (int)iv->values[2];
            if (res > 0) {
                fulldate += 1;
                temps.tm_mday = res;
            }
            if (sz >= 4) {
                res = (int)iv->values[3];
                if (res >= 0 && res < 24) {
                    //reference is 1AM
                    if (fulldate == 0)
                        res += 2;
                    else
                        if (fulldate == 1)
                            res += 1;
                    temps.tm_hour = res;
                }
                if (sz >= 5) {
                    res = (int)iv->values[4];
                    if (res >= 0 && res < 60)
                        temps.tm_min = res;
                    if (sz >= 6) {
                        res = (int)iv->values[5];
                        if (res >= 0 && res < 60)
                            temps.tm_sec = res;
                    }
                }
            }
        }
    }
    
    x = mktime(&temps);
    unlocking_date(this);
    if (x <= 0)
        return aFALSE;
    value = x;
    return aTRUE;
}

Tamgu* Tamgudate::MethodInitial(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long szcall = callfunc->Size();
    
    if (szcall == 0) {
        time(&value);
        return aTRUE;
    }

    Tamgu* kinit = callfunc->Evaluate(0, aNULL, idthread);

    if (kinit->Type() == idtype) {
        memcpy((void*)&value, (void*)&((Tamgudate*)kinit)->value, sizeof(time_t));
        kinit->Release();
        return aTRUE;
    }

    locking_date(this);
    time_t x = 0;
    struct tm temps;
    localtime_s(&temps, &x);

    int fulldate = 0;

    if (kinit->isString() && szcall == 1) {
        string v = kinit->String();
        kinit->Release();
        unlocking_date(this);
        return setstringdate(v, idthread);
    }
    
    if (kinit->Type() == a_ivector && szcall == 1) {
        Tamgu* v = set_the_date((Tamguivector*)kinit);
        kinit->Release();
        unlocking_date(this);
        return v;
    }
    
    //Year
    long res = kinit->Integer();
    kinit->Release();

    if (res > 0) {
        temps.tm_year = res - 1900;
        fulldate = 100;
    }


    //Month
    if (szcall >= 2) {
        res = (int)callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (res > 0) {
            fulldate += 10;
            temps.tm_mon = res - 1;
        }
    }

    //Day
    if (szcall >= 3) {
        res = (int)callfunc->Evaluate(2,  aNULL, idthread)->Integer();
        if (res > 0) {
            fulldate += 1;
            temps.tm_mday = res;
        }
    }

    //Hour
    if (szcall >= 4) {
        res = (int)callfunc->Evaluate(3,  aNULL, idthread)->Integer();
        if (res >= 0 && res < 24) {
            //reference is 1AM
            if (fulldate == 0)
                res += 2;
            else
                if (fulldate == 1)
                    res += 1;
            temps.tm_hour = res;
        }
    }


    //Minute
    if (szcall >= 5) {
        res = (int)callfunc->Evaluate(4,  aNULL, idthread)->Integer();
        if (res >= 0 && res < 60)
            temps.tm_min = res;
    }

    //Second
    if (szcall >= 6) {
        res = (int)callfunc->Evaluate(5,  aNULL, idthread)->Integer();
        if (res >= 0 && res < 60)
            temps.tm_sec = res;
    }



    x = mktime(&temps);
    unlocking_date(this);
    if (x <= 0)
        return aFALSE;
    value = x;
    return aTRUE;
}
#else
Tamgu* Tamgudate::set_the_date(Tamguivector* iv) {
    time_t x = 0;
    locking_date(this);
    struct tm* temps = localtime(&x);
    
    int fulldate = 0;
    
    long sz = iv->values.size();
    if (!sz) {
        unlocking_date(this);
        return aFALSE;
    }
    
    int res = (int)iv->values[0];
    
    if (res > 0) {
        temps->tm_year = res - 1900;
        fulldate = 100;
    }
    
    
    //Month
    if (sz >= 2) {
        res = (int)iv->values[1];
        if (res > 0) {
            fulldate += 10;
            temps->tm_mon = res - 1;
        }
        if (sz >= 3) {
            res = (int)iv->values[2];
            if (res > 0) {
                fulldate += 1;
                temps->tm_mday = res;
            }
            if (sz >= 4) {
                res = (int)iv->values[3];
                if (res >= 0 && res < 24) {
                    //reference is 1AM
                    if (fulldate == 0)
                        res += 2;
                    else
                        if (fulldate == 1)
                            res += 1;
                    temps->tm_hour = res;
                }
                if (sz >= 5) {
                    res = (int)iv->values[4];
                    if (res >= 0 && res < 60)
                        temps->tm_min = res;
                    if (sz >= 6) {
                        res = (int)iv->values[5];
                        if (res >= 0 && res < 60)
                            temps->tm_sec = res;
                    }
                }
            }
        }
    }
    
    x = mktime(temps);
    unlocking_date(this);
    if (x <= 0)
        return aFALSE;
    value = x;
    return aTRUE;
}


Tamgu* Tamgudate::MethodInitial(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    locking_date(this);
    long szcall = callfunc->Size();
    if (szcall == 0) {
        time(&value);
        unlocking_date(this);
        return aTRUE;
    }

    Tamgu* kinit = callfunc->Evaluate(0, aNULL, idthread);

    if (kinit->Type() == idtype) {
        memcpy((void*)&value, (void*)&((Tamgudate*)kinit)->value, sizeof(time_t));
        kinit->Release();
        unlocking_date(this);
        return aTRUE;
    }

    if (kinit->isString() && szcall == 1) {
        string v = kinit->String();
        kinit->Release();
        unlocking_date(this);
        return setstringdate(v, idthread);
    }

    time_t x = 0;
    struct tm* temps = localtime(&x);

    int fulldate = 0;
    
    if (kinit->Type() == a_ivector && szcall == 1) {
        Tamgu* v = set_the_date((Tamguivector*)kinit);
        kinit->Release();
        unlocking_date(this);
        return v;
    }

    //Year
    int res = (int)kinit->Integer();
    kinit->Release();

    if (res > 0) {
        temps->tm_year = res - 1900;
        fulldate = 100;
    }


    //Month
    if (szcall >= 2) {
        res = (int)callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (res > 0) {
            fulldate += 10;
            temps->tm_mon = res - 1;
        }
    }

    //Day
    if (szcall >= 3) {
        res = (int)callfunc->Evaluate(2, aNULL, idthread)->Integer();
        if (res > 0) {
            fulldate += 1;
            temps->tm_mday = res;
        }
    }

    //Hour
    if (szcall >= 4) {
        res = (int)callfunc->Evaluate(3, aNULL, idthread)->Integer();
        if (res >= 0 && res < 24) {
            //reference is 1AM
            if (fulldate == 0)
                res += 2;
            else
                if (fulldate == 1)
                    res += 1;
            temps->tm_hour = res;
        }
    }


    //Minute
    if (szcall >= 5) {
        res = (int)callfunc->Evaluate(4, aNULL, idthread)->Integer();
        if (res >= 0 && res < 60)
            temps->tm_min = res;
    }

    //Second
    if (szcall >= 6) {
        res = (int)callfunc->Evaluate(5, aNULL, idthread)->Integer();
        if (res >= 0 && res < 60)
            temps->tm_sec = res;
    }

    x = mktime(temps);
    unlocking_date(this);
    if (x <= 0)
        return aFALSE;
    value = x;
    return aTRUE;
}
#endif

Tamgu* Tamgudate::MethodDate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string d = String();
    return globalTamgu->Providewithstring(d);
}

#ifdef WIN32
Tamgu* Tamgudate::MethodYear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    struct tm temps;
    locking_date(this);
    localtime_s(&temps, &value);
    int vl;
    if (callfunc->Size() == 2) {
        vl = (int)callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (vl < 100)
            return globalTamgu->Returnerror(e_wrong_value_for02, idthread);
        temps.tm_year = vl - 1900;
        value = mktime(&temps);
        unlocking_date(this);
        return aTRUE;
    }

    vl = temps.tm_year + 1900;
    unlocking_date(this);
    return globalTamgu->ProvideConstint(vl);
}

Tamgu* Tamgudate::MethodMonth(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    locking_date(this);
    struct tm temps;
    localtime_s(&temps, &value);
    int vl;
    if (callfunc->Size() == 2) {
        vl = (int)callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (vl < 1 || vl>12)
            return globalTamgu->Returnerror(e_wrong_value_for05, idthread);
        temps.tm_mon = vl - 1;
        value = mktime(&temps);
        unlocking_date(this);
        return aTRUE;
    }
    vl = temps.tm_mon + 1;
    unlocking_date(this);
    return globalTamgu->ProvideConstint(vl);
}

Tamgu* Tamgudate::MethodDay(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    struct tm temps;
    locking_date(this);
    localtime_s(&temps, &value);
    int vl;
    if (callfunc->Size() == 2) {
        vl = (int)callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (vl < 1 || vl>31)
            return globalTamgu->Returnerror(e_wrong_value_for05, idthread);
        temps.tm_mday = vl;
        value = mktime(&temps);
        unlocking_date(this);
        return aTRUE;
    }

    vl = temps.tm_mday;
    unlocking_date(this);
    return globalTamgu->ProvideConstint(vl);
}

Tamgu* Tamgudate::MethodHour(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    struct tm temps;
    locking_date(this);
    localtime_s(&temps, &value);
    if (callfunc->Size() == 2) {
        int vl = (int)callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (vl < 0 || vl>23)
            return globalTamgu->Returnerror(e_wrong_value_for, idthread);
        temps.tm_hour = vl;
        value = mktime(&temps);
        unlocking_date(this);
        return aTRUE;
    }

    unlocking_date(this);
    return globalTamgu->ProvideConstint(temps.tm_hour);
}

Tamgu* Tamgudate::MethodMinute(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    struct tm temps;
    locking_date(this);
    localtime_s(&temps, &value);
    if (callfunc->Size() == 2) {
        int vl = (int)callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (vl < 0 || vl>59)
            return globalTamgu->Returnerror(e_wrong_value_for04, idthread);
        temps.tm_min = vl;
        value = mktime(&temps);
        unlocking_date(this);
        return aTRUE;
    }
    unlocking_date(this);
    return globalTamgu->ProvideConstint(temps.tm_min);
}

Tamgu* Tamgudate::MethodSecond(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    struct tm temps;
    locking_date(this);
    localtime_s(&temps, &value);
    if (callfunc->Size() == 2) {
        int vl = (int)callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (vl < 0 || vl>59)
            return globalTamgu->Returnerror(e_wrong_value_for03, idthread);
        temps.tm_sec = vl;
        value = mktime(&temps);
        unlocking_date(this);
        return aTRUE;
    }
    unlocking_date(this);
    return globalTamgu->ProvideConstint(temps.tm_sec);
}

Tamgu* Tamgudate::MethodWeekday(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    struct tm temps;
    locking_date(this);
    localtime_s(&temps, &value);
    int vl = temps.tm_wday;
    unlocking_date(this);
    return globalTamgu->ProvideConstint(vl);
}

Tamgu* Tamgudate::MethodYearday(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    struct tm temps;
    locking_date(this);
    localtime_s(&temps, &value);
    int vl = temps.tm_yday;
    unlocking_date(this);
    return globalTamgu->ProvideConstint(vl);
}

Tamgu* Tamgudate::MethodFormat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* kformat = callfunc->Evaluate(0, aNULL, idthread);
    char buffer[101];
    string sformat = kformat->String();
    struct tm temps;
    locking_date(this);
    localtime_s(&temps, &value);
    strftime(buffer, 100, STR(sformat), &temps);
    unlocking_date(this);
    return globalTamgu->Providestring(buffer);
}
#else
Tamgu* Tamgudate::MethodYear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    locking_date(this);
    struct tm* temps = localtime(&value);
    int vl;
    if (callfunc->Size() == 2) {
        vl = (int)callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (vl < 100)
            return globalTamgu->Returnerror(e_wrong_value_for02, idthread);
        temps->tm_year = vl - 1900;
        value = mktime(temps);
        unlocking_date(this);
        return aTRUE;
    }

    vl = temps->tm_year + 1900;
    unlocking_date(this);
    return globalTamgu->ProvideConstint(vl);
}

Tamgu* Tamgudate::MethodMonth(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    locking_date(this);
    struct tm* temps = localtime(&value);
    int vl;
    if (callfunc->Size() == 2) {
        vl = (int)callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (vl < 1 || vl>12)
            return globalTamgu->Returnerror(e_wrong_value_for05, idthread);
        temps->tm_mon = vl - 1;
        value = mktime(temps);
        unlocking_date(this);
        return aTRUE;
    }
    vl = temps->tm_mon + 1;
    unlocking_date(this);
    return globalTamgu->ProvideConstint(vl);
}

Tamgu* Tamgudate::MethodDay(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    locking_date(this);
    struct tm* temps = localtime(&value);
    int vl;
    if (callfunc->Size() == 2) {
        vl = (int)callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (vl < 1 || vl>31)
            return globalTamgu->Returnerror(e_wrong_value_for05, idthread);
        temps->tm_mday = vl;
        value = mktime(temps);
        unlocking_date(this);
        return aTRUE;
    }

    vl = temps->tm_mday;
    unlocking_date(this);
    return globalTamgu->ProvideConstint(vl);
}

Tamgu* Tamgudate::MethodHour(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    locking_date(this);
    struct tm* temps = localtime(&value);
    if (callfunc->Size() == 2) {
        int vl = (int)callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (vl < 0 || vl>23)
            return globalTamgu->Returnerror(e_wrong_value_for, idthread);
        temps->tm_hour = vl;
        value = mktime(temps);
        unlocking_date(this);
        return aTRUE;
    }

    unlocking_date(this);
    return globalTamgu->ProvideConstint(temps->tm_hour);
}

Tamgu* Tamgudate::MethodMinute(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    locking_date(this);
    struct tm* temps = localtime(&value);
    if (callfunc->Size() == 2) {
        int vl = (int)callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (vl < 0 || vl>59)
            return globalTamgu->Returnerror(e_wrong_value_for04, idthread);
        temps->tm_min = vl;
        value = mktime(temps);
        unlocking_date(this);
        return aTRUE;
    }
    unlocking_date(this);
    return globalTamgu->ProvideConstint(temps->tm_min);
}

Tamgu* Tamgudate::MethodSecond(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    locking_date(this);
    struct tm* temps = localtime(&value);
    if (callfunc->Size() == 2) {
        int vl = (int)callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (vl < 0 || vl>59)
            return globalTamgu->Returnerror(e_wrong_value_for03, idthread);
        temps->tm_sec = vl;
        value = mktime(temps);
        unlocking_date(this);
        return aTRUE;
    }
    unlocking_date(this);
    return globalTamgu->ProvideConstint(temps->tm_sec);
}

Tamgu* Tamgudate::MethodWeekday(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    locking_date(this);
    struct tm* temps = localtime(&value);
    int vl = temps->tm_wday;
    unlocking_date(this);
    return globalTamgu->ProvideConstint(vl);
}

Tamgu* Tamgudate::MethodYearday(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    locking_date(this);
    struct tm* temps = localtime(&value);
    int vl = temps->tm_yday;
    unlocking_date(this);
    return globalTamgu->ProvideConstint(vl);
}

Tamgu* Tamgudate::MethodFormat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* kformat = callfunc->Evaluate(0, aNULL, idthread);
    char buffer[101];
    locking_date(this);
    string sformat = kformat->String();
    struct tm* temps = localtime(&value);
    strftime(buffer, 100, STR(sformat), temps);
    unlocking_date(this);
    return globalTamgu->Providestring(buffer);
}

#endif
