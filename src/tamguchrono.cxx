/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguchrono.cxx
 Date       : 2017/09/01
 Purpose    :
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#include "tamgu.h"
#include "instructions.h"
#include "constobjects.h"
#include "tamguchrono.h"
#include "tamgufvector.h"

#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>

//We need to declare once again our local definitions.
Exporting basebin_hash<chronoMethod>  Tamguchrono::methods;

Exporting short Tamguchrono::idtype = 0;
static ThreadLock clock_locker;

static void locking_clock(Tamgu* c) {
    if (globalTamgu->threadMODE && c->hasLock())
        clock_locker.lock->lock();
}

static void unlocking_clock(Tamgu* c) {
    if (globalTamgu->threadMODE && c->hasLock())
        clock_locker.lock->unlock();
}


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguchrono::AddMethod(TamguGlobal* global, string name, chronoMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
        global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
        return;
    
    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}

void Tamguchrono::Setidtype(TamguGlobal* global) {
    if (methods.isEmpty())
        Tamguchrono::InitialisationModule(global,"");
}

bool Tamguchrono::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();

    Tamguchrono::idtype = global->Getid("chrono");
    
    Tamguchrono::AddMethod(global, "_initial", &Tamguchrono::MethodUnit, P_ONE | P_NONE, "unit(int i): 1 is second. 2 is milliseconds. 3 is microsecond. 4 is nanosecond.");
    Tamguchrono::AddMethod(global, "reset", &Tamguchrono::MethodReset, P_NONE, "reset(): reset the chrono");
    Tamguchrono::AddMethod(global, "start", &Tamguchrono::MethodReset, P_NONE, "start(): start the chrono");
    Tamguchrono::AddMethod(global, "stop", &Tamguchrono::MethodStop, P_NONE, "stop(): stop the chrono and returns the intermediate time");
    Tamguchrono::AddMethod(global, "unit", &Tamguchrono::MethodUnit, P_ONE, "unit(int i): 1 is second. 2 is milliseconds. 3 is microsecond. 4 is nanosecond.");

    if (version != "") {
        global->newInstance[Tamguchrono::idtype] = new Tamguchrono(global);
        global->RecordCompatibilities(Tamguchrono::idtype);
        
        global->CreateSystemVariable(new TamguConstInt(1), "c_second", a_float);
        global->CreateSystemVariable(new TamguConstInt(2), "c_millisecond", a_float);
        global->CreateSystemVariable(new TamguConstInt(3), "c_microsecond", a_float);
        global->CreateSystemVariable(new TamguConstInt(4), "c_nanosecond", a_float);
    }
    
    return true;
}

Tamgu* Tamguchrono::Put(Tamgu* index, Tamgu* val, short idthread) {
    if (val->Type() == Tamguchrono::idtype) {
        value = ((Tamguchrono*)val)->value;
        return aTRUE;
    }
    return aFALSE;
}

Tamgu* Tamguchrono::Eval(Tamgu* context, Tamgu* index, short idthread) {
    return this;
}

string Tamguchrono::String() {
    char buffer[100];
    double chronoval = value.time_since_epoch().count();
    sprintf_s(buffer, 100, "%fs", chronoval);
    return buffer;
}

Tamgu* Tamguchrono::MethodUnit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (callfunc->Size() == 0)
        return aFALSE;
    unit = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    return aTRUE;
}

Tamgu* Tamguchrono::MethodReset(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a chrono
    value = std::chrono::high_resolution_clock::now();
    return this;
}

Tamgu* Tamguchrono::MethodStop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a chrono
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    switch (unit) {
        case 1:
            return globalTamgu->ProvideConstfloat(std::chrono::duration_cast<std::chrono::seconds>( end - value ).count());
        case 2:
            return globalTamgu->ProvideConstfloat(std::chrono::duration_cast<std::chrono::milliseconds>( end - value ).count());
        case 3:
            return globalTamgu->ProvideConstfloat(std::chrono::duration_cast<std::chrono::microseconds>( end - value ).count());
        case 4:
            return globalTamgu->ProvideConstfloat(std::chrono::duration_cast<std::chrono::nanoseconds>( end - value ).count());
    }
    return globalTamgu->ProvideConstfloat(std::chrono::duration_cast<std::chrono::milliseconds>( end - value ).count());
}

Tamgu* Tamguchrono::minus(Tamgu* bb, bool autoself) {
    if (bb->Type() != idtype)
        return aNULL;
    
    Tamguchrono* b = (Tamguchrono*)bb;
    
    switch (unit) {
        case 1:
            return globalTamgu->ProvideConstfloat(std::chrono::duration_cast<std::chrono::seconds>( value - b->value ).count());
        case 2:
            return globalTamgu->ProvideConstfloat(std::chrono::duration_cast<std::chrono::milliseconds>( value - b->value ).count());
        case 3:
            return globalTamgu->ProvideConstfloat(std::chrono::duration_cast<std::chrono::microseconds>( value - b->value ).count());
        case 4:
            return globalTamgu->ProvideConstfloat(std::chrono::duration_cast<std::chrono::nanoseconds>( value - b->value ).count());
    }
    return globalTamgu->ProvideConstfloat(std::chrono::duration_cast<std::chrono::seconds>( value - b->value ).count());
}



//We need to declare once again our local definitions.
Exporting basebin_hash<clockMethod>  Tamguclock::methods;

Exporting short Tamguclock::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguclock::AddMethod(TamguGlobal* global, string name, clockMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
        global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
        return;
    
    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}

void Tamguclock::Setidtype(TamguGlobal* global) {
    if (methods.isEmpty())
        Tamguclock::InitialisationModule(global,"");
}

bool Tamguclock::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();

    Tamguclock::idtype = global->Getid("clock");
    
    Tamguclock::AddMethod(global, "_initial", &Tamguclock::MethodUTC, P_NONE | P_ONE, "utc(string utc_str): Initializes a clock with a string.");
    Tamguclock::AddMethod(global, "reset", &Tamguclock::MethodReset, P_NONE, "reset(): reset the chrono");
    Tamguclock::AddMethod(global, "start", &Tamguclock::MethodReset, P_NONE, "start(): start the chrono");
    Tamguclock::AddMethod(global, "stop", &Tamguclock::MethodStop, P_NONE, "stop(): stop the chrono and returns the intermediate time");
    Tamguclock::AddMethod(global, "unit", &Tamguclock::MethodUnit, P_ONE, "unit(int i): 1 is second. 2 is milliseconds. 3 is microsecond. 4 is nanosecond.");
    Tamguclock::AddMethod(global, "timezone", &Tamguclock::MethodTimezone, P_NONE | P_ONE, "timezone(int time_zone): Defines the time difference with GM time.");
    Tamguclock::AddMethod(global, "format", &Tamguclock::MethodFormat, P_ONE, "format(string frm): frm describes the format, which is used to return a string.");
    Tamguclock::AddMethod(global, "microseconds", &Tamguclock::MethodMicroseconds, P_NONE, "microseconds(): returns the microseconds.");
    Tamguclock::AddMethod(global, "utc", &Tamguclock::MethodUTC, P_NONE|P_ONE, "utc(string utc_str|bool time_zone): utc() returns the UTC time. utc('...') uses the date to instantiate itself.");
    Tamguclock::AddMethod(global, "iso", &Tamguclock::MethodISO8601, P_NONE|P_ONE, "iso(string utc_str|bool time_zone): iso() returns the ISO8601 time. iso('...') uses the date to instantiate itself.");
    
    if (version != "") {
        global->newInstance[Tamguclock::idtype] = new Tamguclock(global);
        global->RecordCompatibilities(Tamguclock::idtype);
    }
    
    return true;
}

Tamgu* Tamguclock::Put(Tamgu* index, Tamgu* val, short idthread) {
    if (val->Type() == Tamguclock::idtype) {
        value = ((Tamguclock*)val)->value;
        return aTRUE;
    }
    return aFALSE;
}

Tamgu* Tamguclock::Eval(Tamgu* context, Tamgu* index, short idthread) {
    return this;
}

string Tamguclock::String() {
    return ISO8601(false);
}

Tamgu* Tamguclock::MethodFormat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    locking_clock(this);
    std::ostringstream os;
    string frm = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(value);
    long pos;
    os << std::put_time(std::gmtime(&currentTime), STR(frm));
    
    string s = os.str();
    pos = frm.find("%fm");
    if (pos != -1) {
        auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(value);
        auto epoch = now_ms.time_since_epoch();
        auto val = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
        long vali = val.count();
        os.str("");
        if (isdigit(s[pos+2])) {
            int nb = s[pos+2] - 48;
            long p10 = 1;
            while (nb) {
                p10 *= 10;
                nb--;
            }
            nb = s[pos+2] - 48;
            os << std::setw(nb) << std::setfill('0') << vali % p10;
            s.replace(pos, 3, os.str());
        }
        else {
            os << std::setw(3) << std::setfill('0') << vali % 1000;
            s.replace(pos, 2, os.str());
        }
    }
    pos =frm.find("%fc");
    if (pos != -1) {
        auto now_ms = std::chrono::time_point_cast<std::chrono::microseconds>(value);
        auto epoch = now_ms.time_since_epoch();
        auto val = std::chrono::duration_cast<std::chrono::microseconds>(epoch);
        os.str("");
        if (isdigit(s[pos+2])) {
            int nb = s[pos+2] - 48;
            long p10 = 1;
            while (nb) {
                p10 *= 10;
                nb--;
            }
            nb = s[pos+2] - 48;
            os << std::setw(nb) << std::setfill('0') << val.count() % p10;
            s.replace(pos, 3, os.str());
        }
        else {
            os << std::setw(6) << std::setfill('0') << val.count() % 1000000;
            s.replace(pos, 2, os.str());
        }
    }

    unlocking_clock(this);
    return globalTamgu->Providestring(s);
}

Tamgu* Tamguclock::MethodUnit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (callfunc->Size() == 0)
        return aFALSE;
    unit = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    return aTRUE;
}

string Tamguclock::compute_time_zone() {
    locking_clock(this);
    std::time_t currentTime = std::chrono::system_clock::to_time_t(value);
    std::tm* local_tm = std::localtime(&currentTime);
    char buffer[101];
    strftime(buffer, 100, "%z", local_tm);
    unlocking_clock(this);
    
    string res;
    res = buffer[0];
    res += buffer[1];
    res += buffer[2];
    res += ":";
    res += buffer[3];
    res += buffer[4];
    return res;
}

Tamgu* Tamguclock::MethodTimezone(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (callfunc->Size() == 0) {
        timezone_offset = compute_time_zone();
        return aFALSE;
    }
    
    string timezone = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    long sz = timezone.size();
    if (!sz)
        return globalTamgu->Returnerror("Error: missing value for time zone", idthread);
    
    vector<string> sv;
    char sgn = timezone[0];
    string dt;
    long i = 0;
    if (sgn != '+' && sgn != '-')
        sgn = '+';
    else
        i = 1;
    for (; i < sz; i++) {
        if (isdigit(timezone[i]))
            dt += timezone[i];
        else {
            if (dt != "") {
                sv.push_back(dt);
                dt = "";
            }
        }
    }
    
    if (dt.size())
        sv.push_back(dt);
    
    if (sv.size() >= 1) {
        timezone = sgn;
        if (sv[0].size() == 1)
            timezone += "0" + sv[0];
        else {
            if (sv[0].size() == 2)
                timezone += sv[0];
            else
                globalTamgu->Returnerror("Error: wrong value for time zone", idthread);
        }
        if (sv.size() == 2) {
            timezone += ":";
            if (sv[1].size() == 1)
                timezone += sv[1] + "0";
            else {
                if (sv[1].size() == 2)
                    timezone += sv[1];
                else
                    globalTamgu->Returnerror("Error: wrong value for time zone", idthread);
            }
        }
        else {
            if (sv.size() == 1)
                timezone += ":00";
            else
                globalTamgu->Returnerror("Error: wrong value for time zone", idthread);
        }
    }
    else
        globalTamgu->Returnerror("Error: wrong value for time zone", idthread);

    timezone_offset = timezone;
    return aTRUE;
}

Tamgu* Tamguclock::MethodReset(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a chrono
    value = std::chrono::system_clock::now();
    return this;
}

Tamgu* Tamguclock::MethodStop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //First parameter is a chrono
    std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
    switch (unit) {
        case 1:
            return globalTamgu->ProvideConstfloat(std::chrono::duration_cast<std::chrono::seconds>( end - value ).count());
        case 2:
            return globalTamgu->ProvideConstfloat(std::chrono::duration_cast<std::chrono::milliseconds>( end - value ).count());
        case 3:
            return globalTamgu->ProvideConstfloat(std::chrono::duration_cast<std::chrono::microseconds>( end - value ).count());
        case 4:
            return globalTamgu->ProvideConstfloat(std::chrono::duration_cast<std::chrono::nanoseconds>( end - value ).count());
    }
    return globalTamgu->ProvideConstfloat(std::chrono::duration_cast<std::chrono::milliseconds>( end - value ).count());
}


Tamgu* Tamguclock::plus(Tamgu* bb, bool autoself) {
    long val = bb->Integer();
    
    Tamguclock* chrono;
    if (autoself)
        chrono = this;
    else
        chrono = new Tamguclock(value);
    
    switch (unit) {
        case 1: {
            auto add = std::chrono::seconds(val);
            chrono->value += add;
            break;
        }
        case 2: {
            auto add = std::chrono::milliseconds(val);
            chrono->value += add;
            break;
        }
        case 3: {
            auto add = std::chrono::microseconds(val);
            chrono->value += add;
            break;
        }
        case 4: {
            val /= 1000;
            auto add = std::chrono::microseconds(val);
            chrono->value += add;
            break;
        }
    }
    return chrono;
}

Tamgu* Tamguclock::minus(Tamgu* bb, bool autoself) {
    Tamguclock* chrono;
    if (autoself)
        chrono = this;
    else
        chrono = new Tamguclock(value);
    
    if (bb->Type() == idtype) {
        //In this case this is a time difference
        Tamguclock* b = (Tamguclock*)bb;
        
        switch (unit) {
            case 1:
                return globalTamgu->ProvideConstfloat(std::chrono::duration_cast<std::chrono::seconds>( value - b->value ).count());
            case 2:
                return globalTamgu->ProvideConstfloat(std::chrono::duration_cast<std::chrono::milliseconds>( value - b->value ).count());
            case 3:
                return globalTamgu->ProvideConstfloat(std::chrono::duration_cast<std::chrono::microseconds>( value - b->value ).count());
            case 4:
                return globalTamgu->ProvideConstfloat(std::chrono::duration_cast<std::chrono::nanoseconds>( value - b->value ).count());
        }
        return globalTamgu->ProvideConstfloat(std::chrono::duration_cast<std::chrono::seconds>( value - b->value ).count());
    }
    
    long val = bb->Integer();
    
    
    switch (unit) {
        case 1: {
            auto add = std::chrono::seconds(val);
            chrono->value -= add;
            break;
        }
        case 2: {
            auto add = std::chrono::milliseconds(val);
            chrono->value -= add;
            break;
        }
        case 3: {
            auto add = std::chrono::microseconds(val);
            chrono->value -= add;
            break;
        }
        case 4: {
            val /= 1000;
            auto add = std::chrono::microseconds(val);
            chrono->value -= add;
            break;
        }
    }
    return chrono;
}

Tamgu* Tamguclock::multiply(Tamgu* bb, bool autoself) {
    double v = bb->Float();

    auto epoch = value.time_since_epoch();
    
    
    switch (unit) {
        case 1: {
            auto val = std::chrono::duration_cast<std::chrono::seconds>(epoch);
            val *= v;
            return globalTamgu->Providefloat(val.count());
        }
        case 2: {
            auto val = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
            val *= v;
            return globalTamgu->Providefloat(val.count());
        }
        case 3: {
            auto val = std::chrono::duration_cast<std::chrono::microseconds>(epoch);
            val *= v;
            return globalTamgu->Providefloat(val.count());
        }
    }

    auto val = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
    val *= v;
    return globalTamgu->Providefloat(val.count());
}

Tamgu* Tamguclock::divide(Tamgu* bb, bool autoself) {
    double v = bb->Float();
    if (!v)
        return globalTamgu->Returnerror("MAT(203): Cannot divide by 0");
    
    auto epoch = value.time_since_epoch();
    
    
    switch (unit) {
        case 1: {
            auto val = std::chrono::duration_cast<std::chrono::seconds>(epoch);
            val /= v;
            return globalTamgu->Providefloat(val.count());
        }
        case 2: {
            auto val = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
            val /= v;
            return globalTamgu->Providefloat(val.count());
        }
        case 3: {
            auto val = std::chrono::duration_cast<std::chrono::microseconds>(epoch);
            val /= v;
            return globalTamgu->Providefloat(val.count());
        }
    }

    auto val = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
    val /= v;
    return globalTamgu->Providefloat(val.count());
}

Tamgu* Tamguclock::mod(Tamgu* bb, bool autoself) {
    long v = bb->Integer();
    if (!v)
        return globalTamgu->Returnerror("MAT(203): Cannot divide by 0");
    
    auto epoch = value.time_since_epoch();
    
    
    switch (unit) {
        case 1: {
            auto val = std::chrono::duration_cast<std::chrono::seconds>(epoch);
            val %= v;
            return globalTamgu->Providefloat(val.count());
        }
        case 2: {
            auto val = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
            val %= v;
            return globalTamgu->Providefloat(val.count());
        }
        case 3: {
            auto val = std::chrono::duration_cast<std::chrono::microseconds>(epoch);
            val %= v;
            return globalTamgu->Providefloat(val.count());
        }
    }

    auto val = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
    val %= v;
    return globalTamgu->Providefloat(val.count());
}

#ifdef WIN32
Tamgu* Tamguclock::set_the_utc_date(Tamgufvector* iv) {
    locking_clock(this);
    time_t x = 0;
    struct tm temps;
    localtime_s(&temps, &x);
    
    int fulldate = 0;
    
    long sz = iv->values.size();
    if (!sz)
        return aFALSE;
    
    temps.tm_year = iv->values[0] - 1900;
    temps.tm_mon = iv->values[1] - 1;
    temps.tm_mday = iv->values[2];
    temps.tm_hour = iv->values[3];
    temps.tm_min = iv->values[4];
    temps.tm_sec = iv->values[5];
    
    x = mktime(&temps);
    unlocking_clock(this);
    if (x <= 0)
        return aFALSE;
    
    value = std::chrono::system_clock::from_time_t(x);
    int v = (int)iv->values[7];
    
    double tm = (iv->values[7] - (double)v)*60;

    char buffer[100];
    if (v >= 0)
        sprintf_s(buffer, 100, "+%02d:%02d", v, (int)tm);
    else
        sprintf_s(buffer, 100, "%02d:%02d", v - 1, (int)tm);
    timezone_offset = buffer;

    auto now_ms = std::chrono::time_point_cast<std::chrono::microseconds>(value);
    auto epoch = now_ms.time_since_epoch();
    auto val = std::chrono::duration_cast<std::chrono::microseconds>(epoch);
    
    int diff = iv->values[6] - (val.count()%1000000);
    value += std::chrono::microseconds(diff);
    return aTRUE;
}

Tamgu* Tamguclock::set_the_iso8601_date(Tamgufvector* iv) {
    locking_clock(this);
    time_t x = 0;
    struct tm temps;
    localtime_s(&temps, &x);
    
    int fulldate = 0;
    
    long sz = iv->values.size();
    if (!sz)
        return aFALSE;
    
    temps.tm_year = iv->values[0] - 1900;
    temps.tm_mon = iv->values[1] - 1;
    temps.tm_mday = iv->values[2];
    temps.tm_hour = iv->values[3];
    temps.tm_min = iv->values[4];
    temps.tm_sec = iv->values[5];
    
    x = mktime(&temps);
    unlocking_clock(this);
    if (x <= 0)
        return aFALSE;
    
    value = std::chrono::system_clock::from_time_t(x);
    int v = (int)iv->values[7];
    
    double tm = (iv->values[7] - (double)v)*60;
    char buffer[100];
    if (v >= 0)
        sprintf_s(buffer, 100, "+%02d:%02d", v, (int)tm);
    else
        sprintf_s(buffer, 100, "%02d:%02d", v - 1, (int)tm);
    timezone_offset = buffer;

    auto now_ms = std::chrono::time_point_cast<std::chrono::microseconds>(value);
    auto epoch = now_ms.time_since_epoch();
    auto val = std::chrono::duration_cast<std::chrono::microseconds>(epoch);
    
    int diff = iv->values[6] - (val.count()%1000000);
    value += std::chrono::microseconds(diff);
    return aTRUE;
}

#else
Tamgu* Tamguclock::set_the_utc_date(Tamgufvector* iv) {
    locking_clock(this);
    time_t x = 0;
    struct tm* temps = localtime(&x);
    
    temps->tm_year = iv->values[0] - 1900;
    //Month
    temps->tm_mon = iv->values[1] - 1;
    temps->tm_mday = iv->values[2];
    temps->tm_hour = iv->values[3];
    temps->tm_min = iv->values[4];
    temps->tm_sec = iv->values[5];
    x = mktime(temps);
    unlocking_clock(this);
    
    if (x <= 0)
        return aFALSE;
    
    
    value = std::chrono::system_clock::from_time_t(x);
    int v = (int)iv->values[7];
    
    double tm = (iv->values[7] - (double)v)*60;
    char buffer[100];
    if (v >= 0)
        sprintf_s(buffer, 100, "+%02d:%02d", v, (int)tm);
    else
        sprintf_s(buffer, 100, "%02d:%02d", v - 1, (int)tm);
    timezone_offset = buffer;

    auto now_ms = std::chrono::time_point_cast<std::chrono::microseconds>(value);
    auto epoch = now_ms.time_since_epoch();
    auto val = std::chrono::duration_cast<std::chrono::microseconds>(epoch);
    
    int diff = iv->values[6] - (val.count()%1000000);
    value += std::chrono::microseconds(diff);
    
    return aTRUE;
}

Tamgu* Tamguclock::set_the_iso8601_date(Tamgufvector* iv) {
    locking_clock(this);
    time_t x = 0;
    struct tm* temps = localtime(&x);
    
    temps->tm_year = iv->values[0] - 1900;
    //Month
    temps->tm_mon = iv->values[1] - 1;
    temps->tm_mday = iv->values[2];
    temps->tm_hour = iv->values[3];
    temps->tm_min = iv->values[4];
    temps->tm_sec = iv->values[5];
    x = mktime(temps);
    unlocking_clock(this);
    
    if (x <= 0)
        return aFALSE;
    
    
    value = std::chrono::system_clock::from_time_t(x);
    int v = (int)iv->values[7];
    
    double tm = (iv->values[7] - (double)v)*60;
    char buffer[100];
    if (v >= 0)
        sprintf_s(buffer, 100, "+%02d:%02d", v, (int)tm);
    else
        sprintf_s(buffer, 100, "%02d:%02d", v - 1, (int)tm);
    timezone_offset = buffer;

    auto now_ms = std::chrono::time_point_cast<std::chrono::microseconds>(value);
    auto epoch = now_ms.time_since_epoch();
    auto val = std::chrono::duration_cast<std::chrono::microseconds>(epoch);
    
    int diff = iv->values[6] - (val.count()%1000000);
    value += std::chrono::microseconds(diff);

    return aTRUE;
}
#endif

Tamgu* Tamguclock::getstringdate(string& v) {
    vector<string> values;
    string dt;
    string mn;
    Tamgufvector* fv = globalTamgu->Providefvector();
    double tm = -1;
    char sep = '-';
    bool timezone = false;
    
    for (long i = 0; i < v.size(); i++) {
        if (timezone == true && v[i] == ':') {
            tm = conversionfloathexa(STR(dt));
            dt = "";
        }
        else {
            if (v[i] == sep) {
                fv->values.push_back(conversionfloathexa(STR(dt)));
                if (timezone)
                    dt = sep;
                else
                    dt = "";
            }
            else {
                if (v[i] == 'T') {
                    fv->values.push_back(conversionfloathexa(STR(dt)));
                    dt = "";
                    sep = ':';
                }
                else {
                    if (v[i] == '.') {
                        fv->values.push_back(conversionfloathexa(STR(dt)));
                        if (v.find("+", i) != -1)
                            sep = '+';
                        else
                            sep = '-';
                        dt = "";
                        timezone = true;
                    }
                    else
                        dt += v[i];
                }
            }
        }
    }
    
    if (dt != "") {
        double v = conversionfloathexa(STR(dt));
        if (timezone) {
            tm += v / 60;
            v = tm;
        }
        fv->values.push_back(v);
    }
    return fv;
}

Tamgu* Tamguclock::MethodMicroseconds(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    auto now_ms = std::chrono::time_point_cast<std::chrono::microseconds>(value);
    auto epoch = now_ms.time_since_epoch();
    auto val = std::chrono::duration_cast<std::chrono::microseconds>(epoch);
    std::ostringstream os;
    os << std::setw(6) << std::setfill('0') << val.count() % 1000000;
    return globalTamgu->Providestring(os.str());
}

string Tamguclock::UTC(bool compute) {
    std::ostringstream os;
    auto now_ms = std::chrono::time_point_cast<std::chrono::microseconds>(value);
    auto epoch = now_ms.time_since_epoch();
    auto val = std::chrono::duration_cast<std::chrono::microseconds>(epoch);
    
    std::time_t currentTime = std::chrono::system_clock::to_time_t(value);
    std::tm* utc_tm = std::gmtime(&currentTime);
    
    string tmoff = timezone_offset;

    if (compute)
        tmoff = compute_time_zone();

    os << std::put_time(utc_tm, "%FT%T");
    os << '.' << std::setw(6) << std::setfill('0') << val.count() % 1000000;
    
    os << tmoff;
    return os.str();
}

string Tamguclock::ISO8601(bool compute) {
    std::ostringstream os;
    auto now_ms = std::chrono::time_point_cast<std::chrono::microseconds>(value);
    auto epoch = now_ms.time_since_epoch();
    auto val = std::chrono::duration_cast<std::chrono::microseconds>(epoch);
    
    std::time_t currentTime = std::chrono::system_clock::to_time_t(value);
    std::tm* utc_tm = std::localtime(&currentTime);
    
    string tmoff = timezone_offset;

    if (compute)
        tmoff = compute_time_zone();

    os << std::put_time(utc_tm, "%FT%T");
    os << '.' << std::setw(6) << std::setfill('0') << val.count() % 1000000;
    
    os << tmoff;
    return os.str();
}

Tamgu* Tamguclock::MethodUTC(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* argument = NULL;
    if (callfunc->Size() == 1)
        argument = callfunc->Evaluate(0, contextualpattern, idthread);
    
    if (callfunc->Size() == 0 || argument->Type() == a_boolean) {
        string utc_string;
        if (argument != NULL && argument->Boolean())
            utc_string = UTC(true);
        else
            utc_string = UTC(false);
        return globalTamgu->Providestring(utc_string);
    }
    
    if (argument->Type() == Tamguclock::idtype) {
        value = ((Tamguclock*)argument)->value;
        return aTRUE;
    }
    
    string dte = argument->String();
    Tamgufvector* fv = (Tamgufvector*)getstringdate(dte);
    if (fv->Size() != 8)
        return globalTamgu->Returnerror("Error: Wrong format", idthread);
    
    
    Tamgu* res = set_the_utc_date(fv);
    fv->Release();
    return res;
}

Tamgu* Tamguclock::MethodISO8601(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* argument = NULL;
    if (callfunc->Size() == 1)
        argument = callfunc->Evaluate(0, contextualpattern, idthread);
    
    if (callfunc->Size() == 0 || argument->Type() == a_boolean) {
        string utc_string;
        if (argument != NULL && argument->Boolean())
            utc_string = ISO8601(true);
        else
            utc_string = ISO8601(false);
        return globalTamgu->Providestring(utc_string);
    }
    
    if (argument->Type() == Tamguclock::idtype) {
        value = ((Tamguclock*)argument)->value;
        return aTRUE;
    }
    
    string dte = argument->String();
    Tamgufvector* fv = (Tamgufvector*)getstringdate(dte);
    if (fv->Size() != 8)
        return globalTamgu->Returnerror("Error: Wrong format", idthread);
    
    
    Tamgu* res = set_the_iso8601_date(fv);
    fv->Release();
    return res;
}
