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
Exporting hmap<string, string> Tamgudate::infomethods;
Exporting bin_hash<unsigned long> Tamgudate::exported;

Exporting short Tamgudate::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgudate::AddMethod(TamguGlobal* global, string name, dateMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgudate::Setidtype(TamguGlobal* global) {
        Tamgudate::idtype = global->Getid("date");
    }

   bool Tamgudate::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

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


    global->newInstance[Tamgudate::idtype] = new Tamgudate(global);
    global->RecordMethods(Tamgudate::idtype,Tamgudate::exported);

    return true;
}

Tamgu* Tamgudate::Put(Tamgu* index, Tamgu* val, short idthread) {
    if (val->Type() == Tamgudate::idtype) {
        value = ((Tamgudate*)val)->value;
        return aTRUE;
    }
    return aFALSE;
}

Tamgu* Tamgudate::Get(Tamgu* context, Tamgu* index, short idthread) {
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

#ifdef WIN32
Tamgu* Tamgudate::MethodInitial(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    if (callfunc->Size() == 0) {
        time(&value);
        return aTRUE;
    }

    Tamgu* kinit = callfunc->Evaluate(0, aNULL, idthread);

    if (kinit->Type() == idtype) {
        memcpy((void*)&value, (void*)&((Tamgudate*)kinit)->value, sizeof(time_t));
        kinit->Release();
        return aTRUE;
    }

    time_t x = 0;
    struct tm temps;
    localtime_s(&temps, &x);

    int fulldate = 0;


    //Year
    long res = kinit->Integer();
    kinit->Release();

    if (res > 0) {
        temps.tm_year = res - 1900;
        fulldate = 100;
    }


    //Month
    if (callfunc->Size() >= 2) {
        res = callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (res > 0) {
            fulldate += 10;
            temps.tm_mon = res - 1;
        }
    }

    //Day
    if (callfunc->Size() >= 3) {
        res = callfunc->Evaluate(2,  aNULL, idthread)->Integer();
        if (res > 0) {
            fulldate += 1;
            temps.tm_mday = res;
        }
    }

    //Hour
    if (callfunc->Size() >= 4) {
        res = callfunc->Evaluate(3,  aNULL, idthread)->Integer();
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
    if (callfunc->Size() >= 5) {
        res = callfunc->Evaluate(4,  aNULL, idthread)->Integer();
        if (res >= 0 && res < 60)
            temps.tm_min = res;
    }

    //Second
    if (callfunc->Size() >= 6) {
        res = callfunc->Evaluate(5,  aNULL, idthread)->Integer();
        if (res >= 0 && res < 60)
            temps.tm_sec = res;
    }



    x = mktime(&temps);
    if (x <= 0)
        return aFALSE;
    value = x;
    return aTRUE;
}
#else
Tamgu* Tamgudate::MethodInitial(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    if (callfunc->Size() == 0) {
        time(&value);
        return aTRUE;
    }

    Tamgu* kinit = callfunc->Evaluate(0, aNULL, idthread);

    if (kinit->Type() == idtype) {
        memcpy((void*)&value, (void*)&((Tamgudate*)kinit)->value, sizeof(time_t));
        kinit->Release();
        return aTRUE;
    }

    time_t x = 0;
    struct tm* temps = localtime(&x);

    int fulldate = 0;


    //Year
    long res = kinit->Integer();
    kinit->Release();

    if (res > 0) {
        temps->tm_year = res - 1900;
        fulldate = 100;
    }


    //Month
    if (callfunc->Size() >= 2) {
        res = callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (res > 0) {
            fulldate += 10;
            temps->tm_mon = res - 1;
        }
    }

    //Day
    if (callfunc->Size() >= 3) {
        res = callfunc->Evaluate(2, aNULL, idthread)->Integer();
        if (res > 0) {
            fulldate += 1;
            temps->tm_mday = res;
        }
    }

    //Hour
    if (callfunc->Size() >= 4) {
        res = callfunc->Evaluate(3, aNULL, idthread)->Integer();
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
    if (callfunc->Size() >= 5) {
        res = callfunc->Evaluate(4, aNULL, idthread)->Integer();
        if (res >= 0 && res < 60)
            temps->tm_min = res;
    }

    //Second
    if (callfunc->Size() >= 6) {
        res = callfunc->Evaluate(5, aNULL, idthread)->Integer();
        if (res >= 0 && res < 60)
            temps->tm_sec = res;
    }



    x = mktime(temps);
    if (x <= 0)
        return aFALSE;
    value = x;
    return aTRUE;
}
#endif

Tamgu* Tamgudate::MethodDate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string d = String();
    return globalTamgu->Providestring(d);
}

#ifdef WIN32
Tamgu* Tamgudate::MethodYear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    struct tm temps;
    localtime_s(&temps, &value);
    long vl;
    if (callfunc->Size() == 2) {
        vl = callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (vl < 100)
            return globalTamgu->Returnerror("Wrong value for a year", idthread);
        temps.tm_year = vl - 1900;
        value = mktime(&temps);
        return aTRUE;
    }

    vl = temps.tm_year + 1900;
    return globalTamgu->Provideint(vl);
}

Tamgu* Tamgudate::MethodMonth(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    struct tm temps;
    localtime_s(&temps, &value);
    long vl;
    if (callfunc->Size() == 2) {
        vl = callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (vl < 1 || vl>12)
            return globalTamgu->Returnerror("Wrong value for a day", idthread);
        temps.tm_mon = vl - 1;
        value = mktime(&temps);
        return aTRUE;
    }
    vl = temps.tm_mon + 1;
    return globalTamgu->Provideint(vl);
}

Tamgu* Tamgudate::MethodDay(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    struct tm temps;
    localtime_s(&temps, &value);
    long vl;
    if (callfunc->Size() == 2) {
        vl = callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (vl < 1 || vl>31)
            return globalTamgu->Returnerror("Wrong value for a day", idthread);
        temps.tm_mday = vl;
        value = mktime(&temps);
        return aTRUE;
    }

    vl = temps.tm_mday;
    return globalTamgu->Provideint(vl);
}

Tamgu* Tamgudate::MethodHour(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    struct tm temps;
    localtime_s(&temps, &value);
    if (callfunc->Size() == 2) {
        long vl = callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (vl < 0 || vl>23)
            return globalTamgu->Returnerror("Wrong value for an hour", idthread);
        temps.tm_hour = vl;
        value = mktime(&temps);
        return aTRUE;
    }

    return globalTamgu->Provideint(temps.tm_hour);
}

Tamgu* Tamgudate::MethodMinute(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    struct tm temps;
    localtime_s(&temps, &value);
    if (callfunc->Size() == 2) {
        long vl = callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (vl < 0 || vl>59)
            return globalTamgu->Returnerror("Wrong value for a minute", idthread);
        temps.tm_min = vl;
        value = mktime(&temps);
        return aTRUE;
    }
    return globalTamgu->Provideint(temps.tm_min);
}

Tamgu* Tamgudate::MethodSecond(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    struct tm temps;
    localtime_s(&temps, &value);
    if (callfunc->Size() == 2) {
        long vl = callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (vl < 0 || vl>59)
            return globalTamgu->Returnerror("Wrong value for a second", idthread);
        temps.tm_sec = vl;
        value = mktime(&temps);
        return aTRUE;
    }
    return globalTamgu->Provideint(temps.tm_sec);
}

Tamgu* Tamgudate::MethodWeekday(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    struct tm temps;
    localtime_s(&temps, &value);
    long vl = temps.tm_wday;
    return globalTamgu->Provideint(vl);
}

Tamgu* Tamgudate::MethodYearday(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    struct tm temps;
    localtime_s(&temps, &value);
    long vl = temps.tm_yday;
    return globalTamgu->Provideint(vl);
}

Tamgu* Tamgudate::MethodFormat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* kformat = callfunc->Evaluate(0, aNULL, idthread);
    char buffer[101];
    string sformat = kformat->String();
    struct tm temps; localtime_s(&temps, &value);
    strftime(buffer, 100, STR(sformat), &temps);
    return globalTamgu->Providestring(buffer);
}
#else
Tamgu* Tamgudate::MethodYear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    struct tm* temps = localtime(&value);
    long vl;
    if (callfunc->Size() == 2) {
        vl = callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (vl < 100)
            return globalTamgu->Returnerror("Wrong value for a year", idthread);
        temps->tm_year = vl - 1900;
        value = mktime(temps);
        return aTRUE;
    }

    vl = temps->tm_year + 1900;
    return globalTamgu->Provideint(vl);
}

Tamgu* Tamgudate::MethodMonth(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    struct tm* temps = localtime(&value);
    long vl;
    if (callfunc->Size() == 2) {
        vl = callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (vl < 1 || vl>12)
            return globalTamgu->Returnerror("Wrong value for a day", idthread);
        temps->tm_mon = vl - 1;
        value = mktime(temps);
        return aTRUE;
    }
    vl = temps->tm_mon + 1;
    return globalTamgu->Provideint(vl);
}

Tamgu* Tamgudate::MethodDay(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    struct tm* temps = localtime(&value);
    long vl;
    if (callfunc->Size() == 2) {
        vl = callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (vl < 1 || vl>31)
            return globalTamgu->Returnerror("Wrong value for a day", idthread);
        temps->tm_mday = vl;
        value = mktime(temps);
        return aTRUE;
    }

    vl = temps->tm_mday;
    return globalTamgu->Provideint(vl);
}

Tamgu* Tamgudate::MethodHour(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    struct tm* temps = localtime(&value);
    if (callfunc->Size() == 2) {
        long vl = callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (vl < 0 || vl>23)
            return globalTamgu->Returnerror("Wrong value for an hour", idthread);
        temps->tm_hour = vl;
        value = mktime(temps);
        return aTRUE;
    }

    return globalTamgu->Provideint(temps->tm_hour);
}

Tamgu* Tamgudate::MethodMinute(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    struct tm* temps = localtime(&value);
    if (callfunc->Size() == 2) {
        long vl = callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (vl < 0 || vl>59)
            return globalTamgu->Returnerror("Wrong value for a minute", idthread);
        temps->tm_min = vl;
        value = mktime(temps);
        return aTRUE;
    }
    return globalTamgu->Provideint(temps->tm_min);
}

Tamgu* Tamgudate::MethodSecond(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    struct tm* temps = localtime(&value);
    if (callfunc->Size() == 2) {
        long vl = callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (vl < 0 || vl>59)
            return globalTamgu->Returnerror("Wrong value for a second", idthread);
        temps->tm_sec = vl;
        value = mktime(temps);
        return aTRUE;
    }
    return globalTamgu->Provideint(temps->tm_sec);
}

Tamgu* Tamgudate::MethodWeekday(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    struct tm* temps = localtime(&value);
    long vl = temps->tm_wday;
    return globalTamgu->Provideint(vl);
}

Tamgu* Tamgudate::MethodYearday(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a string
    struct tm* temps = localtime(&value);
    long vl = temps->tm_yday;
    return globalTamgu->Provideint(vl);
}

Tamgu* Tamgudate::MethodFormat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* kformat = callfunc->Evaluate(0, aNULL, idthread);
    char buffer[101];
    string sformat = kformat->String();
    struct tm* temps = localtime(&value);
    strftime(buffer, 100, STR(sformat), temps);
    return globalTamgu->Providestring(buffer);
}

#endif
