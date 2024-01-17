/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : procedures.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

//Definition of procedures
#ifdef WIN32
#define NOMINMAX
#include "Windows.h"
#define _USE_MATH_DEFINES
#include <math.h>
#endif

#include <random>
#include "tamgu.h"
#include "tamgutreemapls.h"
#include "tamguversion.h"
#include "instructions.h"
#include "tamgustring.h"
#include "tamguvector.h"
#include "tamguivector.h"
#include "tamgudvector.h"
#include "tamgufvector.h"
#include "tamgusvector.h"
#include "tamguuvector.h"
#include "tamgubvector.h"
#include "tamgumap.h"
#include "constobjects.h"
#include "tamguprimemap.h"
#include "compilecode.h"
#include "tamgutamgu.h"
#include "tamgulvector.h"
#include "tamgumapss.h"
#include "tamguframeinstance.h"
#include "tamgumapsi.h"
#include "x_tokenize.h"
#include "codeparse.h"

#ifdef UNIX
#include <sys/time.h>
#endif

//---------------------------------------------------------
bool store_pattern(uchar,wstring&);
//---------------------------------------------------------
Tamgu* ProcThreadhandle(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

Tamgu* ProcThreadid(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    return globalTamgu->ProvideConstint(idthread);
}

Tamgu* ProcNbthreads(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    return new Tamgushort(globalTamgu->threadcounter);
}

//---------------------------------------------------------
// RANDOM FUNCTIONS
//---------------------------------------------------------
Tamgu* ProcRandom(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long mx = 100;
    if (callfunc->Size() == 1)
        mx = callfunc->Evaluate(0, aNULL, idthread)->Float();
    return globalTamgu->ProvideConstfloat(localrandom(mx));
}

Tamgu* Proca_Random(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long mx = 100;
    if (callfunc->Size() == 1)
        mx = callfunc->Evaluate(0, aNULL, idthread)->Float();
    return globalTamgu->ProvideConstfloat(a_localrandom(mx));
}


Tamgu* Proc_uniform_int(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()

    long nb = callfunc->Evaluate(0, aNULL, idthread)->Integer();

    long alpha = 0;

    long beta = std::numeric_limits<long>::max();
    
    if (callfunc->Size() >= 2) {
        alpha = callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (callfunc->Size() == 3)
            beta = callfunc->Evaluate(2, aNULL, idthread)->Integer();
    }

    std::uniform_int_distribution<long> dis(alpha, beta);
    if (nb == 1)
        return globalTamgu->ProvideConstint((long)dis(gen));

    Tamguivector* iv = (Tamguivector*)Selectaivector(contextualpattern);
    
    for (long i = 0; i < nb; i++) {
        alpha = dis(gen);
        iv->storevalue(alpha);
    }
    return iv;
}

Tamgu* Proc_uniform_real(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    
    long nb = callfunc->Evaluate(0, aNULL, idthread)->Integer();

    double alpha = 0;
    double beta = 1;
    
    if (callfunc->Size() >= 2) {
        alpha = callfunc->Evaluate(1, aNULL, idthread)->Float();
        if (callfunc->Size() == 3)
            beta = callfunc->Evaluate(2, aNULL, idthread)->Float();
    }

    std::uniform_real_distribution<double> d(alpha, beta);
    if (nb == 1)
        return globalTamgu->ProvideConstfloat((double)d(gen));

    Tamgufvector* iv = (Tamgufvector*)Selectafvector(contextualpattern);
    
    for (long i = 0; i < nb; i++) {
        alpha = d(gen);
        iv->storevalue(alpha);
    }
    return iv;
}

Tamgu* Proc_binomial_distribution(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()

    long nb = callfunc->Evaluate(0, aNULL, idthread)->Integer();

    long alpha = 1;
    double beta = 0.5;
    
    if (callfunc->Size() >= 2) {
        alpha = callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (callfunc->Size() == 3)
            beta = callfunc->Evaluate(2, aNULL, idthread)->Float();
    }

    std::binomial_distribution<long> dis(alpha, beta);
    if (nb == 1)
        return globalTamgu->ProvideConstint((long)dis(gen));

    Tamguivector* iv = (Tamguivector*)Selectaivector(contextualpattern);
    
    for (long i = 0; i < nb; i++) {
        alpha = dis(gen);
        iv->storevalue(alpha);
    }
    return iv;
}


Tamgu* Proc_bernoulli_distribution(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    
    long nb = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    double alpha = 0.5;
    if (callfunc->Size() == 2)
        alpha = callfunc->Evaluate(1, aNULL, idthread)->Float();
    
    std::bernoulli_distribution d(alpha);
    if (nb == 1)
        return globalTamgu->ProvideConstint((long)d(gen));

    Tamguivector* iv = (Tamguivector*)Selectaivector(contextualpattern);
    
    bool v;
    for (long i = 0; i < nb; i++) {
        v = d(gen);
        iv->storevalue((long)v);
    }
    return iv;
}

Tamgu* Proc_normal_distribution(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    
    long nb = callfunc->Evaluate(0, aNULL, idthread)->Integer();

    double alpha = 0;
    double beta = 1;
    
    if (callfunc->Size() >= 2) {
        alpha = callfunc->Evaluate(1, aNULL, idthread)->Float();
        if (callfunc->Size() == 3)
            beta = callfunc->Evaluate(2, aNULL, idthread)->Float();
    }

    std::normal_distribution<double> d(alpha, beta);
    if (nb == 1)
        return globalTamgu->ProvideConstfloat((double)d(gen));

    Tamgufvector* iv = (Tamgufvector*)Selectafvector(contextualpattern);

    double v;
    for (long i = 0; i < nb; i++) {
        v = d(gen);
        iv->storevalue(v);
    }
    return iv;
}

Tamgu* Proc_negative_binomial_distribution(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    
    long nb = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    
    long alpha = 1;
    double beta = 0.5;
    
    if (callfunc->Size() >= 2) {
        alpha = callfunc->Evaluate(1, aNULL, idthread)->Integer();
        if (callfunc->Size() == 3)
            beta = callfunc->Evaluate(2, aNULL, idthread)->Float();
    }

    std::negative_binomial_distribution<long> d(alpha, beta);
    if (nb == 1)
        return globalTamgu->ProvideConstint((long)d(gen));

    Tamguivector* iv = (Tamguivector*)Selectaivector(contextualpattern);

    long v;
    for (long i = 0; i < nb; i++) {
        v = d(gen);
        iv->storevalue(v);
    }
    return iv;
}

#ifdef WIN32
long unaryfunc(double v) {
	return v;
}
#endif

Tamgu* Proc_random_choice(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    
    long nb = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    Tamgu* valuevect = callfunc->Evaluate(1, aNULL, idthread);

    if (!valuevect->isVectorContainer())
        return globalTamgu->Returnerror(e_expecting_a_vector02, idthread);
    
    long i;
    size_t sz = valuevect->Size();
    

#ifdef WIN32
	std::discrete_distribution<long> d(sz, 0, 100, unaryfunc);
#else
	double val = 100 / sz;
	vector<double> vect;
	for (i = 0; i < sz; i++)
		vect.push_back(val);

	std::discrete_distribution<long> d(vect.begin(), vect.end());
#endif

    if (nb == 1) {
        i = d(gen);
        return valuevect->getvalue(i)->Atom(true);
    }

    Tamguvector* iv = (Tamguvector*)Selectavector(contextualpattern);

    long v;
    Tamgu* vtamgu;
    for (i = 0; i < nb; i++) {
        v = d(gen);
        vtamgu = valuevect->getvalue(v);
        iv->Push(vtamgu);
    }
    return iv;
}


Tamgu* Proc_discrete_distribution(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    
    long nb = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    Tamgu* tvect = callfunc->Evaluate(1, aNULL, idthread);

    vector<double> vect;

    long i;
    if (tvect->Type() == a_fvector)
        vect = ((Tamgufvector*)tvect)->values;
    else {
        for (i = 0; i < tvect->Size(); i++)
            vect.push_back(tvect->getfloat(i));
    }

#ifdef WIN32
	std::discrete_distribution<long> d;
	d._Par._Pvec = vect;
	d._Par._Init();
#else
    std::discrete_distribution<long> d(vect.begin(), vect.end());
#endif
    if (nb == 1)
        return globalTamgu->ProvideConstint((long)d(gen));

    Tamguivector* iv = (Tamguivector*)Selectaivector(contextualpattern);

    long v;
    for (i = 0; i < nb; i++) {
        v = d(gen);
        iv->storevalue(v);
    }
    return iv;
}


Tamgu* Proc_piecewise_constant_distribution(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    
    long nb = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    Tamgu* tvect = callfunc->Evaluate(1, aNULL, idthread);
    Tamgu* tinter = callfunc->Evaluate(2, aNULL, idthread);

    vector<double> vect;
    vector<double> inter;

    long i;
    if (tvect->Type() == a_fvector)
        vect = ((Tamgufvector*)tvect)->values;
    else {
        for (i = 0; i < tvect->Size(); i++)
            vect.push_back(tvect->getfloat(i));
    }

    if (tinter->Type() == a_fvector)
        inter = ((Tamgufvector*)tinter)->values;
    else {
        for (i = 0; i < tinter->Size(); i++)
            inter.push_back(tinter->getfloat(i));
    }

    std::piecewise_constant_distribution<double> d(vect.begin(), vect.end(), inter.begin());
    if (nb == 1)
        return globalTamgu->ProvideConstfloat((double)d(gen));

    Tamgufvector* iv = (Tamgufvector*)Selectafvector(contextualpattern);

    double v;
    for (i = 0; i < nb; i++) {
        v = d(gen);
        iv->storevalue(v);
    }
    return iv;
}

Tamgu* Proc_piecewise_linear_distribution(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    
    long nb = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    Tamgu* tvect = callfunc->Evaluate(1, aNULL, idthread);
    Tamgu* tinter = callfunc->Evaluate(2, aNULL, idthread);

    vector<double> vect;
    vector<double> inter;

    long i;
    if (tvect->Type() == a_fvector)
        vect = ((Tamgufvector*)tvect)->values;
    else {
        for (i = 0; i < tvect->Size(); i++)
            vect.push_back(tvect->getfloat(i));
    }

    if (tinter->Type() == a_fvector)
        inter = ((Tamgufvector*)tinter)->values;
    else {
        for (i = 0; i < tinter->Size(); i++)
            inter.push_back(tinter->getfloat(i));
    }

    std::piecewise_linear_distribution<double> d(vect.begin(), vect.end(), inter.begin());
    if (nb == 1)
        return globalTamgu->ProvideConstfloat((double)d(gen));

    Tamgufvector* iv = (Tamgufvector*)Selectafvector(contextualpattern);

    double v;
    for (i = 0; i < nb; i++) {
        v = d(gen);
        iv->storevalue(v);
    }
    return iv;
}


Tamgu* Proc_lognormal_distribution(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    
    long nb = callfunc->Evaluate(0, aNULL, idthread)->Integer();

    double alpha = 0;
    double beta = 1;
    
    if (callfunc->Size() >= 2) {
        alpha = callfunc->Evaluate(1, aNULL, idthread)->Float();
        if (callfunc->Size() == 3)
            beta = callfunc->Evaluate(2, aNULL, idthread)->Float();
    }

    std::lognormal_distribution<double> d(alpha, beta);

    if (nb == 1)
        return globalTamgu->ProvideConstfloat((double)d(gen));

    Tamgufvector* iv = (Tamgufvector*)Selectafvector(contextualpattern);
    
    for (long i = 0; i < nb; i++) {
        alpha = d(gen);
        iv->storevalue(alpha);
    }
    return iv;
}

Tamgu* Proc_geometric_distribution(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    
    long nb = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    double alpha = 0.5;
    if (callfunc->Size()==2)
        alpha = callfunc->Evaluate(1, aNULL, idthread)->Float();
    
    std::geometric_distribution<long> d(alpha);
    if (nb == 1)
        return globalTamgu->ProvideConstint((long)d(gen));

    Tamguivector* iv = (Tamguivector*)Selectaivector(contextualpattern);

    long v;
    for (long i = 0; i < nb; i++) {
        v = d(gen);
        iv->storevalue(v);
    }
    return iv;
}

Tamgu* Proc_cauchy_distribution(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    
    long nb = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    
    double alpha = 0;
    double beta = 1;
    
    if (callfunc->Size() >= 2) {
        alpha = callfunc->Evaluate(1, aNULL, idthread)->Float();
        if (callfunc->Size() == 3)
            beta = callfunc->Evaluate(2, aNULL, idthread)->Float();
    }

    std::cauchy_distribution<double> d(alpha, beta);

    if (nb == 1)
        return globalTamgu->ProvideConstfloat((double)d(gen));

    Tamgufvector* iv = (Tamgufvector*)Selectafvector(contextualpattern);

    double v;
    for (long i = 0; i < nb; i++) {
        v = d(gen);
        iv->storevalue(v);
    }
    return iv;
}

Tamgu* Proc_fisher_distribution(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    
    long nb = callfunc->Evaluate(0, aNULL, idthread)->Integer();

    double alpha = 1;
    double beta = 1;
    
    if (callfunc->Size() >= 2) {
        alpha = callfunc->Evaluate(1, aNULL, idthread)->Float();
        if (callfunc->Size() == 3)
            beta = callfunc->Evaluate(2, aNULL, idthread)->Float();
    }

    std::fisher_f_distribution<double> d(alpha, beta);
    if (nb == 1)
        return globalTamgu->ProvideConstfloat((double)d(gen));

    Tamgufvector* iv = (Tamgufvector*)Selectafvector(contextualpattern);

    double v;
    for (long i = 0; i < nb; i++) {
        v = d(gen);
        iv->storevalue(v);
    }
    return iv;
}

Tamgu* Proc_student_distribution(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    
    long nb = callfunc->Evaluate(0, aNULL, idthread)->Integer();

    double alpha = 1;
    
    if (callfunc->Size() == 2)
        alpha = callfunc->Evaluate(1, aNULL, idthread)->Float();

    std::student_t_distribution<double> d(alpha);
    if (nb == 1)
        return globalTamgu->ProvideConstfloat((double)d(gen));

    Tamgufvector* iv = (Tamgufvector*)Selectafvector(contextualpattern);

    double v;
    for (long i = 0; i < nb; i++) {
        v = d(gen);
        iv->storevalue(v);
    }
    return iv;
}

Tamgu* Proc_extreme_value_distribution(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    
    long nb = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    
    double alpha = 0;
    double beta = 1;
    
    if (callfunc->Size() >= 2) {
        alpha = callfunc->Evaluate(1, aNULL, idthread)->Float();
        if (callfunc->Size() == 3)
            beta = callfunc->Evaluate(2, aNULL, idthread)->Float();
    }

    std::extreme_value_distribution<double> d(alpha, beta);
    if (nb == 1)
        return globalTamgu->ProvideConstfloat((double)d(gen));

    Tamgufvector* iv = (Tamgufvector*)Selectafvector(contextualpattern);

    double v;
    for (long i = 0; i < nb; i++) {
        v = d(gen);
        iv->storevalue(v);
    }
    return iv;
}


Tamgu* Proc_poisson_distribution(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    
    long nb = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    double alpha = 1.0;
    
    if (callfunc->Size()==2)
        alpha = callfunc->Evaluate(1, aNULL, idthread)->Float();

    std::poisson_distribution<long> d(alpha);
    if (nb == 1)
        return globalTamgu->ProvideConstint((long)d(gen));

    Tamguivector* iv = (Tamguivector*)Selectaivector(contextualpattern);

    long v;
    for (long i = 0; i < nb; i++) {
        v = d(gen);
        iv->storevalue(v);
    }
    return iv;
}

Tamgu* Proc_exponential_distribution(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    
    long nb = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    double alpha = callfunc->Evaluate(1, aNULL, idthread)->Float();

    std::exponential_distribution<double> d(alpha);

    Tamgufvector* iv = (Tamgufvector*)Selectafvector(contextualpattern);

    double v;
    for (long i = 0; i < nb; i++) {
        v = d(gen);
        iv->storevalue(v);
    }
    return iv;
}

Tamgu* Proc_gamma_distribution(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()

    long nb = callfunc->Evaluate(0, aNULL, idthread)->Integer();

    double alpha = callfunc->Evaluate(1, aNULL, idthread)->Float();
    double beta = callfunc->Evaluate(2, aNULL, idthread)->Float();

    std::gamma_distribution<double> d(alpha, beta);
    if (nb == 1)
        return globalTamgu->ProvideConstfloat((double)d(gen));

    Tamgufvector* iv = (Tamgufvector*)Selectafvector(contextualpattern);

    double v;
    for (long i = 0; i < nb; i++) {
        v = d(gen);
        iv->storevalue(v);
    }
    return iv;
}

Tamgu* Proc_weibull_distribution(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()

    long nb = callfunc->Evaluate(0, aNULL, idthread)->Integer();

    double alpha = 1;
    double beta = 1;
    
    if (callfunc->Size() >= 2) {
        alpha = callfunc->Evaluate(1, aNULL, idthread)->Float();
        if (callfunc->Size() == 3)
            beta = callfunc->Evaluate(2, aNULL, idthread)->Float();
    }

    std::weibull_distribution<double> d(alpha, beta);
    if (nb == 1)
        return globalTamgu->ProvideConstfloat((double)d(gen));

    Tamgufvector* iv = (Tamgufvector*)Selectafvector(contextualpattern);

    double v;
    for (long i = 0; i < nb; i++) {
        v = d(gen);
        iv->storevalue(v);
    }
    return iv;
}


Tamgu* Proc_chi_squared_distribution(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()

    long nb = callfunc->Evaluate(0, aNULL, idthread)->Integer();

    double alpha = 1;
    
    if (callfunc->Size() == 2)
        alpha = callfunc->Evaluate(1, aNULL, idthread)->Float();

    std::chi_squared_distribution<double> d(alpha);
    if (nb == 1)
        return globalTamgu->ProvideConstfloat((double)d(gen));

    Tamgufvector* iv = (Tamgufvector*)Selectafvector(contextualpattern);

    double v;
    for (long i = 0; i < nb; i++) {
        v = d(gen);
        iv->storevalue(v);
    }
    return iv;
}

//---------------------------------------------------------
// UUID
//---------------------------------------------------------
uint32_t random32() {
    static bool lisperestrandom = true;
    static uint64_t x = 123456789;
    static uint64_t y = 362436069;
    static uint64_t z = 521288629;
    static int64_t w = time(0);
    if (lisperestrandom) {
        w *= (int64_t)&x;
        lisperestrandom = false;
    }

    unsigned long t;
    
    t = x ^ (x << 11);
    x = y; y = z; z = w;
    w = w ^ (w >> 19) ^ (t ^ (t >> 8));
    return (uint32_t)w;
}

uint16_t random16() {
    static bool lisperestrandom = true;
    static uint64_t x = 123456789;
    static uint64_t y = 362436069;
    static uint64_t z = 521288629;
    static int64_t w = time(0);
    if (lisperestrandom) {
        w *= (int64_t)&y;
        lisperestrandom = false;
    }

    unsigned long t;
    
    t = x ^ (x << 11);
    x = y; y = z; z = w;
    w = w ^ (w >> 19) ^ (t ^ (t >> 8));
    return (uint16_t)w;
}

struct UUID_base {
public:
    uint32_t premier;
    uint16_t second;
    uint16_t troisieme;
    uint16_t quatrieme;
    uint32_t dernier;
    
    UUID_base() {
        premier = random32();
        dernier = random32();
        second = random16();
        troisieme = random16();
        quatrieme = random16();
    }
    
    string str() {
        char res[132];
        sprintf_s(res, 128, "%08x-%04x-%04x-%04x-%08x",premier,second,troisieme,quatrieme,dernier);
        res[9] = '4';
        res[14] = '1';
        return res;
    }
};

Tamgu* Proc_UUID(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    UUID_base base;
    string str = base.str();
    return globalTamgu->Providestring(str);
}

//------------------------------------------------------------------------------------------------------------------------
Tamgu* ProcCreate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    short n = callfunc->Name();
    
    if (callfunc->Size() == 1) {
        Tamgu* value = NULL;
        Tamgu* object;
        
        if (n == a_tamgu) {
            value = callfunc->Evaluate(0, aNULL, idthread);
            if (value->isError())
                return value;

            object = TamguRecordedFiles(value->String());
            if (object != NULL)
                return object;
        }

        object = globalTamgu->newInstance[n]->Newinstance(idthread);

        if (value == NULL) {
            value = callfunc->Evaluate(0, object, idthread);            
            if (value->isError()) {
                object->Release();
                return value;
            }
        }
        
        object->Put(contextualpattern, value, idthread);
        return object;
    }

    return globalTamgu->newInstance[n]->Newinstance(idthread);
}

Tamgu* ProcCreateFrame(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* object = globalTamgu->newInstance.get(callfunc->Name())->Newinstance(idthread);
    if (object->Frame()->isDeclared(a_initial)) {
        object->Setreference();

        Tamgu* a = ((Tamguframeinstance*)object)->MethodInitial(contextualpattern, idthread, callfunc);

        if (globalTamgu->Error(idthread)) {
            ((Tamguframemininstance*)object)->Popframe(idthread);
            object->Resetreference();
            a->Release();
            return globalTamgu->Errorobject(idthread);
        }

        object->Protect();
        a->Release();
    }
    else {
        if (callfunc->Size() != 0) {
            ((Tamguframemininstance*)object)->Popframe(idthread);
            object->Release();
            return globalTamgu->Returnerror(e_wrong_frame_initialization, idthread);
        }
    }
    
    object->Postinstantiation(idthread, true);
    return object;
}
//------------------------------------------------------------------------------------------------------------------------
Tamgu* ProcAllDefinitions(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    hmap<string, hmap<string, string> > information;
    string key;
    string subkey;

    for (const auto& info : globalTamgu->infomethods) {
        for (const auto& types : info.second) {
            key = globalTamgu->Getsymbol(info.first);
            subkey = "." + types.first;
            information[subkey][key] = types.second;
            if (globalTamgu->returntypes.check(info.first)) {
                string returntype = globalTamgu->Getsymbol(globalTamgu->returntypes[info.first]);
                if (returntype[0] == 'a' && returntype[1] == '_')
                    returntype = returntype.substr(2, returntype.size() - 2);
                information[subkey][".return"] = returntype;
            }
            information[key][types.first] = types.second;
            information["auto"][types.first] = types.second;
            information["self"][types.first] = types.second;
            information["let"][types.first] = types.second;
            for (const auto& itp : globalTamgu->commoninfos) {
                subkey = "." + itp.first;
                information[subkey][types.first] = itp.second;
            }
        }
    }
    
    for (const auto& itp : globalTamgu->commoninfos) {
        key = itp.first;
        subkey = "." + key;
        information[subkey]["common"] = key;
    }
    
    for (const auto& itp : globalTamgu->procedureinfos) {
        information[itp.first]["procedure"]  = itp.second;
    }
    
    basebin_hash<TamguFrame*>::iterator itf;
    TamguFrame* tframe;
    for (itf = globalTamgu->frames.begin(); itf != globalTamgu->frames.end(); itf++) {
        key = globalTamgu->Getsymbol(itf->first);
        tframe = itf->second;
        basebin_hash<Tamgu*>::iterator itt;
        for (itt = tframe->declarations.begin(); itt != tframe->declarations.end(); itt++) {
            if (itt->second->isFunction()) {
                subkey = globalTamgu->Getsymbol(itt->first);
                information[key][subkey] = "method";
            }
        }
    }
    
    for (itf = globalTamgu->framecontainers.begin(); itf != globalTamgu->framecontainers.end(); itf++) {
        key = globalTamgu->Getsymbol(itf->first);
        for (const auto& types : globalTamgu->infomethods[a_vector]) {
            subkey = "." + types.first;
            information[subkey][key] = types.second;
            if (globalTamgu->returntypes.check(itf->first)) {
                string returntype = globalTamgu->Getsymbol(globalTamgu->returntypes[itf->first]);
                if (returntype[0] == 'a' && returntype[1] == '_')
                    returntype = returntype.substr(2, returntype.size() - 2);
                information[subkey][".return"] = returntype;
            }
            information[key][types.first] = types.second;
            information["auto"][types.first] = types.second;
            information["self"][types.first] = types.second;
            information["let"][types.first] = types.second;
            for (const auto& itp : globalTamgu->commoninfos) {
                subkey = "." + itp.first;
                information[subkey][types.first] = itp.second;
            }
        }
    }
    
    Tamgumap* themap = globalTamgu->Providemap();
    Tamgumapss* submap;
    
    for (const auto& values: information) {
        submap = globalTamgu->Providemapss();
        submap->values = values.second;
        themap->Push(values.first, submap);
    }
    
    return themap;
}

Tamgu* ProcAllObjects(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgusvector* vs = (Tamgusvector*)Selectasvector(contextualpattern);
    globalTamgu->TamguAllObjects(vs->values);
    map<string, bool> inter;
    for (long i = 0; i < vs->values.size(); i++)
        inter[vs->values[i]] = true;


    vs->values.clear();
    for (const auto& it : inter)
        vs->values.push_back(it.first);
    return vs;
}

Tamgu* ProcAllObjectByType(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    map<string, bool> inter;
    Tamgusvector* vs = globalTamgu->Providesvector();
    
    globalTamgu->TamguAllObjects(vs->values);
    for (long i = 0; i < vs->values.size(); i++)
        inter[vs->values[i]] = true;
    
    Tamgumap* m = (Tamgumap*)Selectamap(contextualpattern);
    vs->values.clear();

    bin_hash<Tamgu*>::iterator it;
    string value;
    for (it = globalTamgu->newInstance.begin(); it != globalTamgu->newInstance.end(); it++) {
        value = globalTamgu->Getsymbol(it->first);
        vs->values.push_back(value);
        inter[value] = false;
    }
    value = "types";
    m->pushing(value, vs);
    
    vs = globalTamgu->Providesvector();
    bin_hash<unsigned long>::iterator itl;
    for (itl = globalTamgu->allmethods.begin(); itl != globalTamgu->allmethods.end(); itl++) {
        if (globalTamgu->procedures.check(itl->first))
            continue;
        value = globalTamgu->Getsymbol(itl->first);
        vs->values.push_back(value);
        inter[value] = false;
    }
    bin_hash<TamguProcedure>::iterator itp;
    for (itp = globalTamgu->commons.begin(); itp != globalTamgu->commons.end(); itp++) {
        if (!itp->first)
            continue;
        value = globalTamgu->Getsymbol(itp->first);
        vs->values.push_back(value);
        inter[value] = false;
    }
    value = "methods";
    m->pushing(value, vs);
    
    vs = globalTamgu->Providesvector();
    for (itp = globalTamgu->procedures.begin(); itp != globalTamgu->procedures.end(); itp++) {
        if (!itp->first || globalTamgu->newInstance.check(itp->first))
            continue;
        value = globalTamgu->Getsymbol(itp->first);
        vs->values.push_back(value);
        inter[value] = false;
    }
    
    value = "procedures";
    m->pushing(value, vs);
    
    vs = globalTamgu->Providesvector();
    for (const auto& it : inter) {
        if (it.second)
            vs->values.push_back(it.first);
    }
    value = "instructions";
    m->pushing(value, vs);

    return m;
}

//------------------------------------------------------------------------------------------------------------------------
#if defined(MULTIGLOBALTAMGU) || defined(GARBAGEINDEBUG)
Exporting void Garbaging(hmap<std::string, long>& issues);
bool Is_keep_track_garbage();
#endif

Tamgu* ProcPoolStats(Tamgu* context, short idthread, TamguCall* callfunc) {
    Tamgumapsi* stats;


    if (context->Type() == Tamgumapsi::idtype && context->isAffectation()) {
        context->Clear();
        stats = (Tamgumapsi*)context;
    }
    else
        stats = new Tamgumapsi;

#if defined(MULTIGLOBALTAMGU) || defined(GARBAGEINDEBUG)
    if (Is_keep_track_garbage())
        Garbaging(stats->values);
    else
#endif
    {
        stats->values["mapss"] = globalTamgu->mapssreservoire.size();
        stats->values["map"] = globalTamgu->mapreservoire.size();
        stats->values["vector"] = globalTamgu->vectorreservoire.size();
        stats->values["ivector"] = globalTamgu->ivectorreservoire.size();
        stats->values["fvector"] = globalTamgu->fvectorreservoire.size();
        stats->values["svector"] = globalTamgu->svectorreservoire.size();
        stats->values["uvector"] = globalTamgu->uvectorreservoire.size();
        stats->values["int"] = globalTamgu->intreservoire.size();
        stats->values["float"] = globalTamgu->floatreservoire.size();
        stats->values["string"] = globalTamgu->stringreservoire.size();
        stats->values["ustring"] = globalTamgu->ustringreservoire.size();
        stats->values["declaration"] = globalTamgu->declarationreservoire.size();
        stats->values["declarationclean"] = globalTamgu->declarationcleanreservoire.size();
        stats->values["self"] = globalTamgu->slfreservoire.size();
        stats->values["mapss_idx"] = globalTamgu->mapssidx;
        stats->values["map_idx"] = globalTamgu->mapidx;
        stats->values["vector_idx"] = globalTamgu->vectoridx;
        stats->values["ivector_idx"] = globalTamgu->ivectoridx;
        stats->values["fvector_idx"] = globalTamgu->fvectoridx;
        stats->values["svector_idx"] = globalTamgu->svectoridx;
        stats->values["uvector_idx"] = globalTamgu->uvectoridx;
        stats->values["int_idx"] = globalTamgu->intidx;
        stats->values["float_idx"] = globalTamgu->floatidx;
        stats->values["string_idx"] = globalTamgu->stringidx;
        stats->values["ustring_idx"] = globalTamgu->ustringidx;
        stats->values["declaration_idx"] = globalTamgu->declarationidx;
        stats->values["declarationclean_idx"] = globalTamgu->declarationcleanidx;
        stats->values["self_idx"] = globalTamgu->slfidx;
    }
    
    return stats;
}

//------------------------------------------------------------------------------------------------------------------------

Tamgu* ProcBreakPoint(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long i = callfunc->Evaluate(0, aNULL, idthread)->Integer();

    return globalTamgu->ProvideConstint(i);
}
//------------------------------------------------------------------------------------------------------------------------

Tamgu* ProcMax(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* current = callfunc->Evaluate(0, aNULL, idthread);
    Tamgu* right;

    for (size_t i = 1; i < callfunc->Size(); i++) {
        right = callfunc->Evaluate(i, aNULL, idthread);

        if (current->less(right)->Boolean())
            current = right;
    }

    return current;
}

Tamgu* ProcMin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* current = callfunc->Evaluate(0, aNULL, idthread);
    Tamgu* right;

    for (size_t i = 1; i < callfunc->Size(); i++) {
        right = callfunc->Evaluate(i, aNULL, idthread);

        if (current->more(right)->Boolean())
            current = right;
    }

    return current;
}

Tamgu* ProcNegation(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    return booleantamgu[1-callfunc->Evaluate(0, contextualpattern, idthread)->Boolean()];
}

//------------------------------------------------------------------------------------------------------------------------
Tamgu* ProcVersion(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    return globalTamgu->Providestring(TamguVersion());
}
//------------------------------------------------------------------------------------------------------------------------
Tamgu* ProcMirrorDisplay(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    globalTamgu->doubledisplay = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
    return aTRUE;
}
//------------------------------------------------------------------------------------------------------------------------
Tamgu* ProcErrorOnKey(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    bool v = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
    globalTamgu->erroronkey = v;
    return aTRUE;
}
//------------------------------------------------------------------------------------------------------------------------
Tamgu* ProcLooseCompatibility(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    bool v = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
    globalTamgu->loosecompability = v;
    globalTamgu->set_loose_compatibilities();
    return aTRUE;
}
//------------------------------------------------------------------------------------------------------------------------
Tamgu* ProcExit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    globalTamgu->executionbreak = true;
    return aEND;
}

//------------------------------------------------------------------------------------------------------------------------
//Initialize the official valid features for synodes
Tamgu* ProcSetValidFeatures(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    short idname = globalTamgu->Getid("definitions");
    TamguCallFromCall call(idname);
    return globalTamgu->newInstance[a_synode]->CallMethod(idname, contextualpattern, idthread, callfunc);
}
//------------------------------------------------------------------------------------------------------------------------
Tamgu* ProcForceLocks(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    globalTamgu->threadMODE = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
    return aTRUE;
}

Tamgu* ProcLispmode(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (callfunc->Size() == 1) {
        bool v = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
        Setlispmode(v);
        return booleantamgu[v];
    }
        
    return booleantamgu[ToggleLispMode()];
}

//------------------------------------------------------------------------------------------------------------------------
void SetEnvironmentVariable(string command, string val);
Tamgu* ProcSetEnv(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string key = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    string val = callfunc->Evaluate(1, contextualpattern, idthread)->String();
    SetEnvironmentVariable(key, val);
    return aTRUE;
}
//------------------------------------------------------------------------------------------------------------------------
Tamgu* ProcStackSize(Tamgu* domain, short idthread, TamguCall* callfunc) {
    long sz = callfunc->Evaluate(0, domain, idthread)->Integer();
    if (sz > globalTamgu->maxstack)
        globalTamgu->maxstack = sz;
    return aTRUE;
}
//------------------------------------------------------------------------------------------------------------------------
Tamgu* ProcMaxThreads(Tamgu* domain, short idthread, TamguCall* callfunc) {
    long sz = callfunc->Evaluate(0, domain, idthread)->Integer();
    globalTamgu->Reallocatemaxthreads(sz);
    return aTRUE;
}
//------------------------------------------------------------------------------------------------------------------------
Tamgu* ProcMaxMaxRange(Tamgu* domain, short idthread, TamguCall* callfunc) {
    BLONG sz = callfunc->Evaluate(0, domain, idthread)->Long();
    globalTamgu->maxrange = sz;
    return aTRUE;
}
//------------------------------------------------------------------------------------------------------------------------
Tamgu* ProcUse(Tamgu* domain, short idthread, TamguCall* callfunc) {
    string filename = callfunc->Evaluate(0, domain, idthread)->String();
    TamguCode* acode = globalTamgu->Getcurrentcode();
    string name;
    acode->Loadlibrary(filename, name);
    return aTRUE;
}

Tamgu* ProcNop(Tamgu* value, short idthread, TamguCall* callfunc) {
    if (!value->isString())
        value = globalTamgu->Providestring();
    Tamgu* e;
    for (long i = 0; i < callfunc->Size(); i++) {
        e = callfunc->Evaluate(i, aNULL, idthread);
        ((Tamgustring*)value)->value += e->String();
        e->Release();
    }
    return value;
}

Tamgu* ProcNope(Tamgu* domain, short idthread, TamguCall* callfunc) {
    return aTRUE;
}

//------------------------------------------------------------------------------------------------------------------------
//Loading a file into the current tamgu code... See TamguCode::C_regularcall to check the call to this procedure
Tamgu* ProcLoadin(Tamgu* domain, short idthread, TamguCall* callfunc) {

    string filename = callfunc->Evaluate(0, domain, idthread)->String();
    //It has already been loaded once for all...
    for (int i = 0; i < globalTamgu->filenames.size(); i++) {
        if (globalTamgu->filenames[i] == filename)
            return aTRUE;
    }

    string code;
    ifstream file(filename, openMode);
    if (file.fail()) {
        stringstream message;
        message << e_cannot_open_file << filename;
        TamguError* err = new TamguError(message.str());
        return err;
    }

    string line;
    char c = file.get();
    if (!file.eof()) {
        do {
            code += c;
            c = file.get();

        } while (!file.eof());
    }

    file.close();

    //We will store our content into the current code space
    TamguCode* acode = globalTamgu->Getcurrentcode();

    //A loadin takes place into the loading of another file, whose index we save
    short id = acode->currentfileid;
    string currentfilename = acode->filename;
    //Our new filename is pushed into the current code space
    acode->SetFilename(filename);
    //We protect the current parse tree
    bnf_tamgu* current = globalTamgu->currentbnf;
    size_t previousfirstinstruction = acode->firstinstruction;
    //We compile our code
    bool push_in_stack = false;
    if (globalTamgu->Topstack() != &acode->mainframe) {
        globalTamgu->Pushstack(&acode->mainframe);
        push_in_stack = true;
    }
        
    acode->Compile(code);

    if (push_in_stack)
        globalTamgu->Popstack();
    
    acode->firstinstruction = previousfirstinstruction;

    //we reset our our parse tree
    globalTamgu->currentbnf = current;
    //and we put back the current filename id...
    acode->currentfileid = id;
    acode->filename = currentfilename;
    if (globalTamgu->Error(idthread)) {
        stringstream err;
        err << globalTamgu->Errorstring(idthread);
        if (globalTamgu->lineerror < 0)
            globalTamgu->lineerror = 0;

        throw new TamguRaiseError(err, filename, globalTamgu->lineerror, globalTamgu->lineerror);
    }
    return aTRUE;
}


Tamgu* ProcLoadfacts(Tamgu* domain, short idthread, TamguCall* callfunc) {

    string filename = callfunc->Evaluate(0, domain, idthread)->String();
    //It has already been loaded once for all...
    for (int i = 0; i < globalTamgu->filenames.size(); i++) {
        if (globalTamgu->filenames[i] == filename)
            return aTRUE;
    }

    string code;
    ifstream file(filename, openMode);
    if (file.fail()) {
        stringstream message;
        message << e_cannot_open_file << filename;
        TamguError* err = new TamguError(message.str());
        return err;
    }

    string line;
    char c = file.get();
    if (!file.eof()) {
        do {
            code += c;
            c = file.get();

        } while (!file.eof());
    }

    file.close();

    //We will store our content into the current code space
    TamguCode* acode = globalTamgu->Getcurrentcode();

    //A loadin takes place into the loading of another file, whose index we save
    short id = acode->currentfileid;
    string currentfilename = acode->filename;
    //Our new filename is pushed into the current code space
    acode->SetFilename(filename);
    //We protect the current parse tree
    bnf_tamgu* current = globalTamgu->currentbnf;

    tokenizer_result<string> xr;
    bnf_tamgu bnf;

    globalTamgu->tamgu_tokenizer.tokenize<string>(code,xr);
    x_node* xn = new x_node;

    bnf.baseline = globalTamgu->linereference;
    bnf.initialize(&xr);
    string lret;
    
    if (bnf.m_facts(lret, &xn) != 1 || bnf.currentpos != xr.stack.size()) {
        delete xn;
        cerr << " in " << filename << endl;
        stringstream& message = globalTamgu->threads[0].message;
        globalTamgu->lineerror = bnf.lineerror;
        acode->currentline = globalTamgu->lineerror;
        message << e_error_while_parsing02;
        if (bnf.errornumber != -1)
            message << bnf.x_errormsg(bnf.errornumber);
        else
            message << bnf.labelerror;
        
       return globalTamgu->Returnerror(message.str(), globalTamgu->GetThreadid());
    }
    
    globalTamgu->currentbnf = &bnf;
    
    size_t previousfirstinstruction = acode->firstinstruction;

    globalTamgu->Pushstack(&acode->mainframe);
    try {
        for (long i = 0; i < xn->nodes.size(); i++)
            acode->C_rawfact(xn->nodes[i], &acode->mainframe);
    }
    catch (TamguRaiseError* a) {
        globalTamgu->threads[0].currentinstruction = NULL;
        globalTamgu->lineerror = a->left;
        globalTamgu->threads[0].message.str("");
        globalTamgu->threads[0].message.clear();
        globalTamgu->threads[0].message << a->message;
        if (a->message.find(a->filename) == string::npos)
            globalTamgu->threads[0].message << " in " << a->filename;
        
        if (globalTamgu->errorraised[0] == NULL)
            globalTamgu->errorraised[0] = new TamguError(globalTamgu->threads[0].message.str());
        else
            globalTamgu->errorraised[0]->error = globalTamgu->threads[0].message.str();
        
        globalTamgu->errors[0] = true;
        TamguCode* c = globalTamgu->Getcurrentcode();
        if (c->filename != a->filename)
            c->filename = a->filename;
        delete a;
        delete xn;
        globalTamgu->Popstack();
        return aFALSE;
    }
    
    globalTamgu->Popstack();
    
    acode->firstinstruction = previousfirstinstruction;
    globalTamgu->currentbnf = current;
    //and we put back the current filename id...
    acode->currentfileid = id;
    acode->filename = currentfilename;

    delete xn;
    return aTRUE;
}

//------------------------------------------------------------------------------------------------------------------------
Tamgu* ProcEvalFunction(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (globalTamgu->threadMODE)
        return globalTamgu->Returnerror(e_cannot_launch__eval, idthread);

    string code = callfunc->Evaluate(0, contextualpattern, idthread)->String();

    Locking _lock(globalTamgu);
    TamguCode* acode = globalTamgu->Getcurrentcode();
    long lastrecorded;
    
    lastrecorded = globalTamgu->Trackedsize();
    Tamgu* compiled = acode->Compilefunction(code, idthread);
    if (compiled == NULL || !compiled->isFunction()) {
        globalTamgu->threads[idthread].currentinstruction = callfunc;
        if (compiled != NULL) {
            if (compiled->Type() == a_calltaskell)
                return compiled->Body(idthread);
            if (compiled->isError())
                return compiled;
        }
        if (globalTamgu->Error(idthread))
            return globalTamgu->Errorobject(idthread);
        return globalTamgu->Returnerror(e_cannot_compile_this, idthread);
    }

    return compiled;
}

Tamgu* ProcEval(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (contextualpattern->Action() == a_call)
        return ProcEvalFunction(contextualpattern, idthread, callfunc);

    string code = callfunc->Evaluate(0, contextualpattern, idthread)->String();

    Locking _lock(globalTamgu);
    TamguCode* acode = globalTamgu->Getcurrentcode();
    Tamgu* topstack = globalTamgu->Topstack(idthread);
    
    if (callfunc->Size() == 2) {
        if (callfunc->Evaluate(1, contextualpattern, idthread)->Boolean())
            topstack = &acode->mainframe;
    }

    TamguLocalEvaluation local(topstack);
    
    //Return the current position in the garbage and set the condition to record
    //new elements...
    //We switch to threadMODE so that Provide will create elements and not store them
    long position = globalTamgu->initialize_local_garbage(idthread);
    Tamgu* ci = globalTamgu->threads[idthread].currentinstruction;
    
    if (code.find(";") == -1 && code.find("{") == -1)
        code = "return ("+code+");";

    long lastrecorded = globalTamgu->Trackedsize();

    globalTamgu->Pushstack(&local, idthread);
    Tamgu* compiled = acode->CompileExpression(code, idthread);
    globalTamgu->Popstack(idthread);
    
    if (compiled == NULL || compiled->isError() || globalTamgu->Error(idthread)) {
        globalTamgu->clean_from_garbage_position(topstack, idthread, position, compiled, lastrecorded);
        globalTamgu->threads[idthread].currentinstruction = ci;
        if (compiled != NULL && compiled->isError())
            return compiled;
        if (globalTamgu->Error(idthread))
            return globalTamgu->Errorobject(idthread);
        return globalTamgu->Returnerror(e_cannot_compile_this02, idthread);
    }

    if (compiled->isFunction()) {
        globalTamgu->set_garbage_mode(false);
        return compiled;
    }

    long maxrecorded = globalTamgu->threads[idthread].localgarbage.size();

    acode->compilemode = true;
    globalTamgu->threads[idthread].currentinstruction = ci;
    Tamgu* a = acode->ExecuteExpression(local,idthread);
    globalTamgu->threads[idthread].currentinstruction = ci;

    globalTamgu->clean_from_garbage_position(topstack, idthread, position, a, lastrecorded, maxrecorded);
    return a;
}

void TraverseFrame(string& name, Tamgumap* values, Tamgu* value) {
    vector<short> localvariables;
    value->Variables(localvariables);
    string framename = globalTamgu->Getsymbol(value->Type())+ " " + name;
    Tamgumap* attributes = globalTamgu->Providemap();
    values->pushing(framename, attributes);
    Tamgu* localval;
    for (long l = 0; l < localvariables.size(); l++) {
        localval = value->Declaration(localvariables[l])->GetLetValue();
        if (localval->isFrameinstance())
            TraverseFrame(framename, attributes, localval);
        else {
            framename = globalTamgu->Getsymbol(localval->Type()) + " ";
            framename += globalTamgu->Getsymbol(localvariables[l]);
            attributes->pushing(framename, localval);
        }
    }
}

Tamgu* ProcVariables(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Locking _lock(globalTamgu);
    TamguCode* acode = globalTamgu->Getcurrentcode();
    Tamgu* topstack = NULL;
    
    if (callfunc->Evaluate(0, contextualpattern, idthread)->Boolean())
        topstack = &acode->mainframe;
    else
        topstack = globalTamgu->Topstack(idthread);
    
    Tamgumap* values = (Tamgumap*)Selectamap(contextualpattern);
    vector<short> variables;
    topstack->Variables(variables);
    Tamgu* value;
    string name;
    short t_ype;
    for (long i = 0; i < variables.size(); i++) {
        value = topstack->Declaration(variables[i]);
        if (value != NULL) {
            if (value->isFrameinstance()) {
                name = globalTamgu->Getsymbol(variables[i]);
                TraverseFrame(name, values, value);
            }
            else {
                t_ype = value->Type();
                if (t_ype != a_iteration_java && t_ype != a_java_vector && values != value) {
                    name = globalTamgu->Getsymbol(t_ype) + " ";
                    name += globalTamgu->Getsymbol(variables[i]);
                    values->pushing(name, value);
                }
            }
        }
    }
    
    return values;
}

//------------------------------------------------------------------------------------------------------------------------
Tamgu* ProcRedictectOutput(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string filename = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    bool iserr = callfunc->Evaluate(1, contextualpattern, idthread)->Boolean();
    int output = 1;
    if (iserr)
        output = 2;
    output = redirect_output(filename, output);
    if (output == -2)
        return globalTamgu->Returnerror("SYS(010): Stream has already been redirected", idthread);
    if (output == -1)
        return globalTamgu->Returnerror("SYS(001): Cannot open output file", idthread);
    return globalTamgu->ProvideConstint(output);
}

Tamgu* ProcRestateOutput(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long output = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    bool iserr = callfunc->Evaluate(1, contextualpattern, idthread)->Boolean();
    if (iserr)
        output = restate_output(output, 2);
    else
        output = restate_output(output, 1);
    if (output == -1)
        return globalTamgu->Returnerror("SYS(011): Stream not opened", idthread);
    return aTRUE;
}


//------------------------------------------------------------------------------------------------------------------------
Tamgu* ProcWaitOnJoin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    Tamgu* func = NULL;
    Tamgu* object = NULL;

    if (callfunc->Size() != 0) {
        func = callfunc->Evaluate(0, aNULL, idthread);
        if (!func->isFunction())
            return globalTamgu->Returnerror(e_argument_should_be_function, idthread);
        if (callfunc->Size() == 2) {
            object = callfunc->Evaluate(1, aNULL, idthread);
            object->Setreference();
        }
    }

    
    while (globalTamgu->threads[idthread].nbjoined) {
        
        if (func != NULL) {
            Tamgu* a;
            if (object != NULL) {
                TamguCallFunction1 call(func);
                call.arguments.push_back(object);
                a = call.Eval(aNULL, aNULL, idthread);
            }
            else {
                TamguCallFunction0 call(func);
                a = call.Eval(aNULL, aNULL, idthread);
            }            
            a->Release();
        }
        
        if (globalTamgu->Error(idthread))
            break;
    }
    
    if (object != NULL)
        object->Resetreference();

    if (globalTamgu->Error(idthread))
        return globalTamgu->Errorobject(idthread);

    return aTRUE;
}

Tamgu* ProcGetinfo(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //We read the method name
    Tamgumapss* mapss = globalTamgu->Providemapss();

    string name = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    if (globalTamgu->procedureinfos.find(name) != globalTamgu->procedureinfos.end()) {
        mapss->values[globalTamgu->procedureinfos[name]] = "procedure";
        return mapss;
    }

    if (globalTamgu->commoninfos.find(name) != globalTamgu->commoninfos.end()) {
        mapss->values[globalTamgu->commoninfos[name]] = "all";
        return mapss;
    }


    //Else we need to check with which type it is associated...
    bin_hash<Tamgu* >::iterator itl;
    string info;
    string symb;
    for (itl = globalTamgu->newInstance.begin(); itl != globalTamgu->newInstance.end(); itl++) {
        info = itl->second->Info(name);
        if (info != "Unknown method") {
            symb = globalTamgu->Getsymbol(itl->first);
            if (symb.find("const") != -1)
                continue;
            if (mapss->values.find(info) == mapss->values.end())
                mapss->values[info] = symb;
        }
    }

    return mapss;
}

Tamgu* ProcSleep(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long v = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    std::this_thread::sleep_for(std::chrono::milliseconds(v));
    return aTRUE;
}

Tamgu* ProcPause(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* kfirst = callfunc->Evaluate(0, aNULL, idthread);
    double res = kfirst->Float() * 1000;
    char car = 8;
    bool display = false;
    if (callfunc->Size() == 2)
        display = callfunc->Evaluate(1, aNULL, idthread)->Boolean();
    timeval tempsfinal;
    gettimeofday(&tempsfinal, NULL);
    double init = (((unsigned long)tempsfinal.tv_sec) * 1000 + ((unsigned long)tempsfinal.tv_usec) / 1000.0);

    while (timeminus(init, tempsfinal) < res) {
        if (display) {
            cerr << "/" << car << "/" << car << "/" << car;
            cerr << "-" << car << "-" << car << "-" << car;
            cerr << "\\" << car << "\\" << car << "\\" << car;
        }
        gettimeofday(&tempsfinal, NULL);
    }
    return aTRUE;
}

//___________________________________________________________________________________________________

Tamgu* ProcFullSum(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* v;
    double val = 0;
    for (long i = 0; i < callfunc->Size(); i++) {
        v = callfunc->Evaluate(i, contextualpattern, idthread);
        if (v->isVectorContainer() || v->Type() == a_list) {
            if (i)
                val += v->Sum();
            else
                val = v->Sum();
        }
        else {
            if (i)
                val += v->Float();
            else
                val = v->Float();
        }
    }
    return globalTamgu->ProvideConstfloat(val);
}
 
Tamgu* ProcFullProduct(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* v;
    double val = 0;
    for (long i = 0; i < callfunc->Size(); i++) {
        v = callfunc->Evaluate(0, contextualpattern, idthread);
        if (v->isVectorContainer() || v->Type() == a_list) {
            if (i)
                val *= v->Product();
            else
                val = v->Product();
        }
        else {
            if (i)
                val *= v->Product();
            else
                val = v->Float();
        }
    }
    return globalTamgu->ProvideConstfloat(val);
}
 

Tamgu* ProcSum(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
    if (!v->isVectorContainer() && v->Type() != a_list)
        return globalTamgu->Returnerror(e_expecting_a_vector, idthread);

    long sz = v->Size();
    if (!sz)
        return aZERO;

    long i = 0;
    long j = sz;
    if (callfunc->Size() > 1) {
        i = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
        if (callfunc->Size() == 3)
            j = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
    }

    return v->Thesum(i, j);
}

Tamgu* ProcProduct(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
    if (!v->isVectorContainer() && v->Type() != a_list)
        return globalTamgu->Returnerror(e_expecting_a_vector, idthread);

    long sz = v->Size();
    if (!sz)
        return aZERO;

    long i = 0;
    long j = sz;
    if (callfunc->Size() > 1) {
        i = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
        if (callfunc->Size() == 3)
            j = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
    }

    return v->Theproduct(i, j);
}

//___________________________________________________________________________________________________
Tamgu* ProcSymbols(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgumapsi* tr = new Tamgumapsi();
    hmap<string, short>::iterator it;

    for (it=globalTamgu->symbolIds.begin(); it != globalTamgu->symbolIds.end(); it++)
        tr->values[it->first]=it->second;
    
    return tr;
}

Tamgu* ProcTypes(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgusvector* tr = globalTamgu->Providesvector();
    bin_hash<Tamgu*>::iterator it;
    string value;

    for (it = globalTamgu->newInstance.begin(); it != globalTamgu->newInstance.end(); it++) {
        value = globalTamgu->Getsymbol(it->first);
        tr->values.push_back(value);
    }
    return tr;
}


Tamgu* ProcPatterns(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string key= callfunc->Evaluate(0, contextualpattern, idthread)->String();
    if (key.size() != 1)
        return globalTamgu->Returnerror(e_expecting_a_key);
    
    wstring pattern= callfunc->Evaluate(1, contextualpattern, idthread)->UString();
    
    if (!store_pattern(key[0], pattern))
        return globalTamgu->Returnerror("key is not valid.");
    return aTRUE;
}
//___________________________________________________________________________________________________

Tamgu* ProcCatch(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (globalTamgu->Error(idthread) == false)
        return aFALSE;

    if (callfunc->Size() != 1) {
        globalTamgu->Cleanerror(idthread);
        return aTRUE;
    }
    string resstr = globalTamgu->Errorstring(idthread);
    globalTamgu->Cleanerror(idthread);
    Tamgu* val = globalTamgu->Providewithstring(resstr);
    Tamgu* res = callfunc->Evaluate(0, contextualpattern, idthread);

    res->Put(aNULL, val, idthread);
    val->Release();
    return aTRUE;

}
//___________________________________________________________________________________________________
Tamgu* ProcEmojis(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgutreemapls* amap = new Tamgutreemapls;
    l_emojis(amap->values);
    return amap;
}

//___________________________________________________________________________________________________
Tamgu* ProcJSon(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string json = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    Trim(json);
    return globalTamgu->EvaluateJSON(json, idthread);
}
//___________________________________________________________________________________________________
Tamgu* ProcPrint(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* res = aNULL;
    Tamgustring* kval = (Tamgustring*)globalTamgu->stringbuffer;
    string todisplay;
    #ifdef DOSOUTPUT
    bool convert = false;
    if (globalTamgu->os == &cout)
        convert = true;
    #endif

    for (int i = 0; i < callfunc->Size(); i++) {
        res = callfunc->Evaluate(i, aNULL, idthread);
        if (globalTamgu->Error(idthread))
            return globalTamgu->Errorobject(idthread);
        todisplay += res->StringToDisplay();
    }

    Locking _lock(globalTamgu->_printlock);
    if (kval == NULL) {
        if (globalTamgu->displayfunction == NULL) {
            #ifdef DOSOUTPUT
            if (convert) {
                string sdos = conversion2dos(todisplay);
                *globalTamgu->os << sdos;
            }
            else
                *globalTamgu->os << todisplay;
            #else
            *globalTamgu->os << todisplay;
            #endif
        }
        else
            globalTamgu->displayfunction(todisplay, globalTamgu->displayobject);
        if (globalTamgu->doubledisplay) {
            #ifdef DOSOUTPUT
            string sdos = conversion2dos(todisplay);
            cout << sdos;
            #else
            cout << todisplay;
            #endif
        }
    }
    else
        kval->value += todisplay;

    return aTRUE;
}

Tamgu* ProcPrintLN(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* res = aNULL;
    Tamgustring* kval = (Tamgustring*)globalTamgu->stringbuffer;
    string todisplay;
    #ifdef DOSOUTPUT
    bool convert = false;
    if (globalTamgu->os == &cout)
        convert = true;
    #endif

    for (int i = 0; i < callfunc->Size(); i++) {
        if (i)
            todisplay += " ";
        res = callfunc->Evaluate(i, contextualpattern, idthread);
        if (globalTamgu->Error(idthread))
            return globalTamgu->Errorobject(idthread);

        todisplay += res->StringToDisplay();
    }

    todisplay += Endl;

    Locking _lock(globalTamgu->_printlock);
    if (kval == NULL) {
        if (globalTamgu->doubledisplay) {
            #ifdef DOSOUTPUT
            string sdos = conversion2dos(todisplay);
            cout << sdos;
            #else
            cout << todisplay;
            #endif
        }
        if (globalTamgu->displayfunction == NULL) {
            #ifdef DOSOUTPUT
            if (convert) {
                string sdos = conversion2dos(todisplay);
                *globalTamgu->os << sdos;
            }
            else
                *globalTamgu->os << todisplay;
            #else
            *globalTamgu->os << todisplay;
            #endif
        }
        else
            globalTamgu->displayfunction(todisplay, globalTamgu->displayobject);
    }
    else
        kval->value += todisplay;

    return aTRUE;
}

void PrintALine(TamguGlobal* g, string s) {
    TamguCall call(g->Getid("println"));
    call.arguments.push_back(g->Providestring(s));
    ProcPrintLN(aNULL, 0, &call);
}


Tamgu* ProcPrintFlush(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    globalTamgu->os->flush();
    return aTRUE;
}

Tamgu* ProcPrinterr(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* res = aNULL;
    Tamgustring* kval = (Tamgustring*)globalTamgu->stringbuffererror;
    string todisplay;

    for (int i = 0; i < callfunc->Size(); i++) {
        res = callfunc->Evaluate(i, aNULL, idthread);
        if (globalTamgu->Error(idthread))
            return globalTamgu->Errorobject(idthread);
        todisplay += res->StringToDisplay();
    }

    Locking _lock(globalTamgu->_printlock);
    if (kval == NULL) {
        #ifdef DOSOUTPUT
        todisplay = conversion2dos(todisplay);
        #endif
        cerr << todisplay;
    }
    else
        kval->value += todisplay;

    return aTRUE;
}

Tamgu* ProcPrinterrLN(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* res = aNULL;
    Tamgustring* kval = (Tamgustring*)globalTamgu->stringbuffererror;

    string todisplay;
    for (int i = 0; i < callfunc->Size(); i++) {
        if (i)
            todisplay += " ";
        res = callfunc->Evaluate(i, aNULL, idthread);
        if (globalTamgu->Error(idthread))
            return globalTamgu->Errorobject(idthread);
        todisplay += res->StringToDisplay();
    }

    todisplay += Endl;

    Locking _lock(globalTamgu->_printlock);
    if (kval == NULL) {
        #ifdef DOSOUTPUT
        todisplay = conversion2dos(todisplay);
        #endif
        cerr << todisplay;
    }
    else
        kval->value += todisplay;
    return aTRUE;
}


void toDisplayJN(string& todisplay, Tamgu* context, TamguCall* callfunc, short idthread) {
    //First the container...
    Tamgu* kcont = callfunc->Evaluate(0, context, idthread);
    string sep = Endl;
    long nb = -1;
    long count = -1;
    bool beg = true;

    if (callfunc->Size() > 1)
        sep = callfunc->Evaluate(1, context, idthread)->String();

    if (kcont->isContainer() == false) {
        if (callfunc->Size() == 3) {
            nb = callfunc->Evaluate(2, context, idthread)->Integer();
            if (nb <= 0)
                nb = -1;
        }
        agnostring s = kcont->String();
        s.begin();
        while (!s.end()) {
            if (beg == false)
                todisplay += sep;
            beg = false;
            if (nb != -1) {
                count++;
                if (count == nb) {
                    todisplay += Endl;
                    count = 0;
                }
            }
            todisplay += s.next();
        }
        return;
    }
    
    long element_size = 0;
    if (kcont->isVectorContainer()) {
        if (callfunc->Size() >= 3) {
            nb = callfunc->Evaluate(2, context, idthread)->Integer();
            if (nb <= 0)
                nb = -1;
            if (callfunc->Size() == 4) {
                element_size = callfunc->Evaluate(3, context, idthread)->Integer();
                if (element_size < 0)
                    element_size = 0;
            }
        }
        
        long it;
        if (element_size) {
            long sz = kcont->Size() - 1;
            long szsep = size_c(sep);
            long all = 0;
            vector<string> elements;
            string inter;
            long thesize;
            for (it = 0; it < sz; it++) {
                inter = kcont->getstring(it);
                thesize = size_c(inter) + szsep;
                if (element_size >= all + thesize) {
                    all += thesize;
                    elements.push_back(inter);
                }
                else {
                    nb = element_size - all;
                    for (all  = 0; all < (long)elements.size() - 1; all++) {
                        todisplay += elements[all];
                        todisplay += sep;
                        if (nb) {
                            todisplay += " ";
                            nb--;
                        }
                    }
                    while (nb) {
                        todisplay += " ";
                        nb--;
                    }
                    todisplay += elements[all];
                    todisplay += sep;
                    todisplay += Endl;
                    elements.clear();
                    elements.push_back(inter);
                    all = thesize;
                }
            }
            inter = kcont->getstring(sz);
            thesize = size_c(inter);
            nb = 0;
            beg = false;
            if (element_size < all + thesize) {
                nb = element_size - all;
                beg = true;
            }
            
            for (all = 0; all < elements.size(); all++) {
                todisplay += elements[all];
                todisplay += sep;
                if (nb) {
                    todisplay += " ";
                    nb--;
                }
            }
            if (beg)
                todisplay += Endl;
            todisplay += inter;
            return;
        }
        
        long sz = kcont->Size();
        for (it = 0; it < sz; it++) {
            if (beg == false)
                todisplay += sep;
            beg = false;
            if (nb != -1) {
                count++;
                if (count == nb) {
                    todisplay += Endl;
                    count = 0;
                }
            }
            todisplay += kcont->getstring(it);
        }
        return;
    }
    
    TamguIteration* itr = kcont->Newiteration(false);
    
    string valsep = ":";
    
    if (callfunc->Size() >= 3) {
        valsep = callfunc->Evaluate(2, context, idthread)->String();
        if (callfunc->Size() >= 4) {
            nb = callfunc->Evaluate(3, context, idthread)->Integer();
            if (nb <= 0)
                nb = -1;
            if (callfunc->Size() == 5) {
                nb = -1;
                element_size = callfunc->Evaluate(4, context, idthread)->Integer();
            }
        }
    }
    
    if (element_size) {
        long szsep = size_c(sep);
        vector<string> elements;
        string item;
        long thesize;
        long all = 0;
        
        for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
            item  = itr->Keystring();
            item += valsep;
            item += itr->Valuestring();
            thesize = size_c(item) + szsep;
            if (element_size >= all + thesize) {
                all += thesize;
                elements.push_back(item);
            }
            else {
                nb = element_size - all;
                for (all  = 0; all < (long)elements.size() - 1; all ++) {
                    todisplay += elements[all];
                    todisplay += sep;
                    if (nb) {
                        todisplay += " ";
                        nb--;
                    }
                }
                while (nb) {
                    todisplay += " ";
                    nb--;
                }
                todisplay += elements[all];
                todisplay += sep;
                todisplay += Endl;
                elements.clear();
                elements.push_back(item);
                all = thesize;
            }
        }
        nb = elements.size() - 2;
        for (all  = 0; all < nb; all += 1) {
            todisplay += elements[all];
            todisplay += sep;
        }
        if (nb >= 0) {
            todisplay += elements[nb];
        }
        return;
    }
    
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        if (beg == false)
            todisplay += sep;
        beg = false;
        if (nb != -1) {
            count++;
            if (count == nb) {
                todisplay += Endl;
                count = 0;
            }
        }
        todisplay += itr->Keystring();
        todisplay += valsep;
        todisplay += itr->Valuestring();
    }
    itr->Release();
}

Tamgu* ProcPrettify(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string todisplay;
    #ifdef DOSOUTPUT
    bool convert = false;
    if (globalTamgu->os == &cout)
        convert = true;
    #endif


    toDisplayJN(todisplay, contextualpattern, callfunc, idthread);
    return globalTamgu->Providestring(todisplay);
}

Tamgu* ProcPrintJoin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgustring* kval = (Tamgustring*)globalTamgu->kstd;
    string todisplay;
    #ifdef DOSOUTPUT
    bool convert = false;
    if (globalTamgu->os == &cout)
        convert = true;
    #endif


    toDisplayJN(todisplay, contextualpattern, callfunc, idthread);


    Locking _lock(globalTamgu->_printlock);
    if (kval == NULL) {
        if (globalTamgu->doubledisplay) {
            #ifdef DOSOUTPUT
            string sdos = conversion2dos(todisplay);
            cout << sdos;
            #else
            cout << todisplay;
            #endif
        }
        if (globalTamgu->displayfunction == NULL) {
            #ifdef DOSOUTPUT
            if (convert) {
                string sdos = conversion2dos(todisplay);
                *globalTamgu->os << sdos;
            }
            else
                *globalTamgu->os << todisplay;
            #else
            *globalTamgu->os << todisplay;
            #endif
        }
        else
            globalTamgu->displayfunction(todisplay, globalTamgu->displayobject);
    }
    else
        kval->value += todisplay;

    return aTRUE;
}

Tamgu* ProcPrintJoinErr(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgustring* kval = (Tamgustring*)globalTamgu->kerr;
    string todisplay;

    toDisplayJN(todisplay, contextualpattern, callfunc, idthread);

    Locking _lock(globalTamgu->_printlock);
    if (kval == NULL) {
        #ifdef DOSOUTPUT
        todisplay = conversion2dos(todisplay);
        #endif
        cerr << todisplay;
    }
    else
        kval->value += todisplay;

    return aTRUE;
}

Tamgu* ProcPrintJoinLN(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgustring* kval = (Tamgustring*)globalTamgu->kstd;
    string todisplay;
    #ifdef DOSOUTPUT
    bool convert = false;
    if (globalTamgu->os == &cout)
        convert = true;
    #endif

    toDisplayJN(todisplay, contextualpattern, callfunc, idthread);

    todisplay += Endl;
    Locking _lock(globalTamgu->_printlock);
    if (kval == NULL) {
        if (globalTamgu->doubledisplay) {
            #ifdef DOSOUTPUT
            string sdos = conversion2dos(todisplay);
            cout << sdos;
            #else
            cout << todisplay;
            #endif
        }
        if (globalTamgu->displayfunction == NULL) {
            #ifdef DOSOUTPUT
            if (convert) {
                string sdos = conversion2dos(todisplay);
                *globalTamgu->os << sdos;
            }
            else
                *globalTamgu->os << todisplay;
            #else
            *globalTamgu->os << todisplay;
            #endif
        }
        else
            globalTamgu->displayfunction(todisplay, globalTamgu->displayobject);
    }
    else
        kval->value += todisplay;

    return aTRUE;
}

Tamgu* ProcPrintJoinErrLN(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgustring* kval = (Tamgustring*)globalTamgu->kerr;
    string todisplay;

    toDisplayJN(todisplay, contextualpattern, callfunc, idthread);

    todisplay += Endl;

    Locking _lock(globalTamgu->_printlock);
    if (kval == NULL) {
        #ifdef DOSOUTPUT
        todisplay = conversion2dos(todisplay);
        #endif
        cerr << todisplay;
    }
    else
        kval->value += todisplay;

    return aTRUE;
}

//___________________________________________________________________________________________________

Tamgu* ProcRange(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    Tamgu* initial = aZERO;
    Tamgu* increment = aONE;
    Tamgu* boundary;
    
    if (callfunc->Size() == 1) {
        boundary = callfunc->Evaluate(0, contextualpattern, idthread);
        if (boundary == aNOELEMENT)
            return aNOELEMENT;
    }
    else {
        initial = callfunc->Evaluate(0, contextualpattern, idthread);
            //whenever a aNOELEMENT is returned, we can assume that a variable is missing to fullfil the range...
        if (initial == aNOELEMENT)
            return aNOELEMENT;
        
        boundary = callfunc->Evaluate(1, contextualpattern, idthread);
        if (boundary == aNOELEMENT)
            return aNOELEMENT;
    }
    
    Tamgu* kvect;

    if (callfunc->Size() == 3) {
        increment = callfunc->Evaluate(2, contextualpattern, idthread);
        if (increment == aNOELEMENT)
            return aNOELEMENT;
    }

    if (initial->isInteger() && increment->isInteger()) {
        if (contextualpattern == aNULL)
            kvect = globalTamgu->Provideivector();
        else
            kvect = Selectavector(contextualpattern);

        if (kvect->Type() == a_lvector) {
            BLONG l = initial->Long();
            BLONG r = boundary->Long();
            BLONG inc = increment->Long();

            BLONG d = (r - l) / inc;
            if (d<0)
                d *= -1;

            if (l>r && inc > 0)
                inc *= -1;

            if (d <= globalTamgu->maxrange) {
                if (inc == 0)
                    return kvect;

                kvect->Reserve((long)d);

                if (inc > 0) {
                    for (BLONG i = l; i <= r; i += inc)
                        kvect->storevalue(i);
                }
                else {
                    for (BLONG i = l; i >= r; i += inc)
                        kvect->storevalue(i);
                }
                return kvect;
            }
            kvect->Release();
            return globalTamgu->Returnerror(e_range_over_actual, idthread);
        }

        long l = initial->Integer();
        long r = boundary->Integer();
        long d = r - l + 1;
        long inc = increment->Integer();

        if (d<0)
            d *= -1;
        if (l>r && inc > 0)
            inc *= -1;

        if (d <= globalTamgu->maxrange) {
            if (inc == 0)
                return kvect;

            kvect->Reserve(d);

            if (inc > 0) {
                for (long i = l; i <= r; i += inc)
                    kvect->storevalue(i);
            }
            else {
                for (long i = l; i >= r; i += inc)
                    kvect->storevalue(i);
            }
            return kvect;
        }

        kvect->Release();
        return globalTamgu->Returnerror(e_range_over_actual, idthread);
    }

    if (initial->isFloat() || increment->isFloat()) {
        if (contextualpattern == aNULL)
            kvect = globalTamgu->Providefvector();
        else
            kvect = Selectavector(contextualpattern);

        if (kvect->Type() == a_dvector) {
            float l = initial->Decimal();
            float r = boundary->Decimal();
            float inc = increment->Decimal();

            float d = (r - l) / inc;
            if (d<0)
                d *= -1;

            if (l>r && inc > 0)
                inc *= -1;

            if (d <= globalTamgu->maxrange) {
                if (inc == 0)
                    return kvect;

                kvect->Reserve((long)d);

                if (inc > 0) {
                    for (float i = l; i <= r; i += inc)
                        kvect->storevalue(i);
                }
                else {
                    for (float i = l; i >= r; i += inc)
                        kvect->storevalue(i);
                }
                return kvect;
            }
            kvect->Release();
            return globalTamgu->Returnerror(e_range_over_actual, idthread);
        }

        double l = initial->Float();
        double r = boundary->Float();
        double inc = increment->Float();

        double d = (r - l) / inc;
        if (d<0)
            d *= -1;
        if (l>r && inc > 0)
            inc *= -1;

        if (d <= globalTamgu->maxrange) {
            if (inc == 0)
                return kvect;

            kvect->Reserve((long)d);
            if (inc > 0) {
                for (double i = l; i <= r; i += inc)
                    kvect->storevalue(i);
            }
            else {
                for (double i = l; i >= r; i += inc)
                    kvect->storevalue(i);
            }
            return kvect;
        }
        kvect->Release();
        return globalTamgu->Returnerror(e_range_over_actual, idthread);
    }

    if (initial->Typevariable() == a_ustring) {
        wstring l = initial->UString();
        wstring r = boundary->UString();
        if (l.size() != 1 || r.size() != 1)
            return globalTamgu->Returnerror(e_string_should_be, idthread);
        long inc = increment->Integer();

        char cl = l[0];
        char cr = r[0];
        int d = cr - cl;
        if (d<0)
            d *= -1;

        if (cl>cr && inc > 0)
            inc *= -1;

        kvect = Selectauvector(contextualpattern);
        if (inc == 0)
            return kvect;

        kvect->Reserve((long)d);
        if (inc > 0) {
            for (uchar i = cl; i <= cr; i += inc) {
                l = (char)i;
                kvect->storevalue(l);
            }
        }
        else {
            for (uchar i = cl; i >= cr; i += inc) {
                l = (char)i;
                kvect->storevalue(l);
            }
        }
        return kvect;
    }

    string l = initial->String();
    string r = boundary->String();
    if (l.size() != 1 || r.size() != 1)
        return globalTamgu->Returnerror(e_string_should_be, idthread);
    long inc = increment->Integer();

    char cl = l[0];
    char cr = r[0];
    int d = cr - cl;
    if (d<0)
        d *= -1;

    if (cl>cr && inc > 0)
        inc *= -1;

    kvect = Selectasvector(contextualpattern);
    if (inc == 0)
        return kvect;

    kvect->Reserve((long)d);
    if (inc > 0) {
        for (uchar i = cl; i <= cr; i += inc) {
            l = (char)i;
            kvect->storevalue(l);
        }
    }
    else {
        for (uchar i = cl; i >= cr; i += inc) {
            l = (char)i;
            kvect->storevalue(l);
        }
    }
    return kvect;
}

Tamgu* ProcOrd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    wstring s = callfunc->Evaluate(0, contextualpattern, idthread)->UString();
    if (s.size() >= 1) {
        if (contextualpattern->isNumber())
            return globalTamgu->ProvideConstint((int)s[0]);

        if (contextualpattern->isVectorContainer() || s.size() > 1) {
            Tamgu* kvect = Selectaivector(contextualpattern);
            if (kvect->Type() == a_ivector) {
                Locking _lock((TamguObject*)kvect);
                for (int i = 0; i < s.size(); i++)
                    ((Tamguivector*)kvect)->values.push_back((long)s[i]);
            }
            else {
                TamguConstInt loc(0);
                for (int i = 0; i < s.size(); i++) {
                    loc.value = (long)s[i];
                    kvect->Push(&loc);
                }
            }
            return kvect;
        }

        return globalTamgu->ProvideConstint((int)s[0]);
    }

    return aNULL;
}

//----------------------------------------------------------------------------------------------------------
Tamgu* ProcLock(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //we create our lock
    if (globalTamgu->Error(idthread))
        return aFALSE;

    ThreadLock* ktl = NULL;
    string lock = callfunc->Evaluate(0, aNULL, idthread)->String();
    
    if (globalTamgu->locktables.check(lock) == false) {
        Locking _lock(globalTamgu->_locktableslock);
        if (globalTamgu->locktables.check(lock) == false) {
            ktl = new ThreadLock;
            globalTamgu->locktables.setpointer(lock,ktl);
        }
    }
    
    if (ktl == NULL)
        ktl = globalTamgu->locktables.getpointer(lock);
    
    globalTamgu->threads[idthread].locks[lock] = ktl;
    ktl->Locking();
    return aTRUE;
}

Tamgu* ProcUnlock(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //we create our lock
    if (globalTamgu->Error(idthread))
        return aFALSE;

    ThreadLock* ktl = NULL;
    string lock = callfunc->Evaluate(0, aNULL, idthread)->String();

    if (globalTamgu->locktables.check(lock) == false)
        return globalTamgu->Returnerror(e_unknown_lock, idthread);
    
    ktl = globalTamgu->locktables.getpointer(lock);

    if (globalTamgu->threads[idthread].locks.find(lock) != globalTamgu->threads[idthread].locks.end())
        globalTamgu->threads[idthread].locks.erase(lock);

    //ktl->idthread = 0;
    ktl->Unlocking();
    return aTRUE;
}

//----------------------------------------------------------------------------------------------------------
Tamgu* ProcWaiton(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //we create our lock
    if (globalTamgu->Error(idthread))
        return aFALSE;

    LockedThread* var = NULL;
    Tamgu* varlock = callfunc->Evaluate(0, aNULL, idthread);
    string lock = varlock->String();
    if (globalTamgu->waitstrings.check(lock) == false) {
        Locking _lock(globalTamgu->_waitstringslock);
        if (globalTamgu->waitstrings.check(lock) == false) {
            var = new LockedThread();
            globalTamgu->waitstrings.setpointer(lock,var);
        }
    }
    
    if (var == NULL)
        var = globalTamgu->waitstrings.getpointer(lock);

    var->Blocked();
    return aTRUE;
}

//----------------------------------------------------------------------------------------------------------
// We wait for a given variable to return FALSE as value for Protectedboolean...
Tamgu* ProcWaitonfalse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (globalTamgu->Error(idthread))
        return aFALSE;

    Tamgu* varlock = callfunc->Evaluate(0, aNULL, idthread);
    
    //The value is already false... We return to avoid an unnecessary lock
    if (!varlock->Protectedboolean())
        return aTRUE;

    long id = varlock->idtracker;

    LockedThread* var = NULL;

    if (id == -1) {
        id = globalTamgu->RecordInTrackerProtected(varlock);
        var = new LockedThread();
        globalTamgu->threadvariables.setpointer(id, var);
    }
    else
        var = globalTamgu->threadvariables.getpointer(id);
    
    if (var != NULL) {
        globalTamgu->waitingonfalse = true;
        var->Blocked();
        return aTRUE;
    }
    
    return aFALSE;
}
//----------------------------------------------------------------------------------------------------------
Tamgu* ProcCast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //we create our lock
    if (globalTamgu->Error(idthread))
        return aFALSE;

    LockedThread* var;
    if (callfunc->Size() == 0) {
        atomic_iterator<string, LockedThread*> it(globalTamgu->waitstrings);
        while (!it.end()) {
            it.second->Released();
            it.next();
        }

        return aTRUE;
    }

    Tamgu* varlock = callfunc->Evaluate(0, aNULL, idthread);
    string lock = varlock->String();
    if (globalTamgu->waitstrings.check(lock) ==false)
        return globalTamgu->Returnerror(e_unknown_variable02, idthread);
    
    var = globalTamgu->waitstrings.getpointer(lock);

    var->Released();
    return aTRUE;
}

//----------------------------------------------------------------------------------------------------------
Tamgu* ProcKeyboardGet(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (callfunc->Size()) {
        string label = callfunc->Evaluate(0, aNULL, idthread)->String();
        cerr << label;
    }

    string s;
    getline(cin, s);
    return globalTamgu->Providewithstring(s);
}
//----------------------------------------------------------------------------------------------------------
Tamgu* ProcRaise(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (globalTamgu->Error(idthread))
        return aFALSE;

    string resstr = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    return globalTamgu->Returnerror(resstr, idthread);
}
//----------------------------------------------------------------------------------------------------------
Tamgu* ProcBase(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static vector<wstring> caracs;
    static hash_bin<wchar_t, long> mcaracs;
    wstring w;
    long n, b = -1;

    Tamgu* ke = callfunc->Evaluate(1, contextualpattern, idthread);
    //We can reset the chararacter list...
    if (ke->isVectorContainer()) {
        caracs.clear();
        mcaracs.clear();
        b = ke->Size();
        for (n = 0; n < b; n++) {
            w = ke->getustring(n);
            mcaracs[w[0]] = caracs.size();
            caracs.push_back(w);
        }
        if (b)
            return globalTamgu->ProvideConstint(b);
    }

    if (caracs.size() == 0) {
        w = L"0";
        for (n = 0; n < 10; n++) {
            mcaracs[w[0]] = caracs.size();
            caracs.push_back(w);
            w[0]++;
        }
        w = L"A";
        for (n = 10; n < 36; n++) {
            mcaracs[w[0]] = caracs.size();
            caracs.push_back(w);
            w[0]++;
        }
        w = L"a";
        for (n = 36; n < 62; n++) {
            mcaracs[w[0]] = caracs.size();
            caracs.push_back(w);
            w[0]++;
        }
        w = L"#";
        mcaracs[w[0]] = caracs.size();
        caracs.push_back(w);
        w = L"@";
        mcaracs[w[0]] = caracs.size();
        caracs.push_back(w);
        w = L"";
        if (!b)
            return aTRUE;
    }

    b = ke->Integer();
    ke = callfunc->Evaluate(0, contextualpattern, idthread);

    if (b > caracs.size()) {
        char message[1024];
        sprintf_s(message, 1024, "TAMGUI(510): base too large, max is: %ld", caracs.size());
        return globalTamgu->Returnerror(message, idthread);
    }

    if (ke->isString()) {
        w = ke->UString();
        long v = 0;
        wchar_t wc;
        for (n = 0; n < w.size(); n++) {
            wc = w[n];
            if (mcaracs.check(wc) == false || mcaracs[wc] >= b)
                return globalTamgu->Returnerror("TAMGUI(511): Cannot analyze this string in this base.", idthread);

            v *= b;
            v += mcaracs[wc];
        }
        return globalTamgu->ProvideConstint(v);
    }

    n = ke->Integer();

    while (n) {
        w = caracs[n%b] + w;
        n /= b;
    }

    return globalTamgu->Providewithustring(w);
}

//----------------------------------------------------------------------------------------------------------
/*
<distance(L1,l1,L2,l2) = r | let a=L1.radian(), let b=L2.radian(), let c=l1.radian(), let d=l2.radian(), let r= acos(cos(a)*cos(b)*cos(abs(c-d))+sin(a)*sin(b))*6371>
*/

static inline double Radian(double num) {
    return(M_PI*(num / 180));
}
/*
double startAlt = 0;  // possible de prendre en compte ou pas la pente. Si pas pris en compte alors 0
double endAlt = 0;

double latDistance = Math.toRadians(endLat - startLat);
double lonDistance = Math.toRadians(endLong - startLong);
double a = Math.sin(latDistance / 2) * Math.sin(latDistance / 2)
    + Math.cos(Math.toRadians(startLat)) * Math.cos(Math.toRadians(endLat))
        * Math.sin(lonDistance / 2) * Math.sin(lonDistance / 2);
double c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
double distance = meanEarthRadius * c * 1000; // convert to meters

double height = startAlt - endAlt;

distance = Math.pow(distance, 2) + Math.pow(height, 2);

return (new Double (Math.sqrt(distance))).floatValue();        // resultat en metre
*/

Tamgu* ProcGPSDistance(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    double R = 6371;

    double startAlt = 0;  // possible de prendre en compte ou pas la pente. Si pas pris en compte alors 0
    double endAlt = 0;

    double startLong = callfunc->Evaluate(0, contextualpattern, idthread)->Float();
    double startLat = callfunc->Evaluate(1, contextualpattern, idthread)->Float();
    double endLong = callfunc->Evaluate(2, contextualpattern, idthread)->Float();
    double endLat = callfunc->Evaluate(3, contextualpattern, idthread)->Float();

    if (callfunc->Size() == 5)
        R = callfunc->Evaluate(4, contextualpattern, idthread)->Float();

    double latDistance = Radian(endLat-startLat);
    double lonDistance = Radian(endLong-startLong);
    double a = sin(latDistance / 2) * sin(latDistance / 2) + cos(Radian(startLat)) * cos(Radian(endLat))* sin(lonDistance / 2) * sin(lonDistance / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double distance = R * c * 1000; // convert to meters
    double height = startAlt - endAlt;
    distance = (distance*distance) + (height*height);

    return globalTamgu->ProvideConstfloat(sqrt(distance));
}


Tamgu* ProcMath(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* av = callfunc->Evaluate(0, contextualpattern, idthread);
    double v;

    if (av->isContainer()) {
        short n = callfunc->Name();
        TamguIteration* it = av->Newiteration(false);
        av = av->Newinstance(idthread);
        for (it->Begin(); !it->End()->Boolean(); it->Next()) {
            v = globalTamgu->mathFunctions[n](it->Valuefloat());
            av->storevalue(v);
        }
        it->Release();
        return av;
    }

    v = av->Float();
    return globalTamgu->ProvideConstfloat(globalTamgu->mathFunctions[callfunc->Name()](v));
}

Tamgu* ProcGetTokenizeRules(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgusvector* vect = (Tamgusvector*)Selectasvector(contextualpattern);
    getdefaultrules(vect->values);

    return vect;
}

//----------------------------------------------------------------------------------------------------------
Tamgu* CommonJSon(Tamgu* object, short idthread, TamguCall* callfunc) {
    return globalTamgu->Providestring(object->JSonString());
}

Tamgu* CommonString(Tamgu* object, short idthread, TamguCall* callfunc) {
    return globalTamgu->Providestring(object->String());
}

Tamgu* CommonUString(Tamgu* object, short idthread, TamguCall* callfunc) {
    return globalTamgu->Provideustring(object->UString());
}

Tamgu* CommonInt(Tamgu* object, short idthread, TamguCall* callfunc) {
    return globalTamgu->ProvideConstint(object->Integer());
}

Tamgu* CommonFloat(Tamgu* object, short idthread, TamguCall* callfunc) {
    return globalTamgu->ProvideConstfloat(object->Float());
}

Tamgu* CommonShort(Tamgu* object, short idthread, TamguCall* callfunc) {
    return new Tamgushort(object->Short());
}

Tamgu* CommonLong(Tamgu* object, short idthread, TamguCall* callfunc) {
    return globalTamgu->Providelong(object->Long());
}

Tamgu* CommonDecimal(Tamgu* object, short idthread, TamguCall* callfunc) {
    return new Tamgudecimal(object->Decimal());
}


Tamgu* CommonVector(Tamgu* object, short idthread, TamguCall* callfunc) {
    return object->Vector(idthread);
}

Tamgu* CommonMap(Tamgu* object, short idthread, TamguCall* callfunc) {
    return object->Map(idthread);
}

Tamgu* CommonSize(Tamgu* object, short idthread, TamguCall* callfunc) {
    if (object->isString())
        return globalTamgu->ProvideConstint(object->CommonSize());
    return globalTamgu->ProvideConstint(object->Size());
}

Tamgu* CommonType(Tamgu* object, short idthread, TamguCall* callfunc) {
    return globalTamgu->Providestring(object->Typename());
}

Tamgu* CommonISA(Tamgu* object, short idthread, TamguCall* callfunc) {
    string ty = callfunc->Evaluate(0, aNULL, idthread)->String();
    string cp = globalTamgu->Getsymbol(object->Type());
    if (ty == cp)
        return aTRUE;
    return aFALSE;
}

Tamgu* CommonIsstring(Tamgu* object, short idthread, TamguCall* callfunc) {
    if (object->isString())
        return aTRUE;
    return aFALSE;
}

Tamgu* CommonIscontainer(Tamgu* object, short idthread, TamguCall* callfunc) {
    if (object->isContainer())
        return aTRUE;
    return aFALSE;
}

Tamgu* CommonMark(Tamgu* object, short idthread, TamguCall* callfunc) {
    if (callfunc->Size()==0)
        return new Tamgubyte(object->checkmark());
    object->mark(callfunc->Evaluate(0, aNULL, idthread)->Byte());
    return aTRUE;
}

Tamgu* CommonUnMark(Tamgu* object, short idthread, TamguCall* callfunc) {
    object->unmark();
    return aTRUE;
}

Tamgu* CommonIsnumber(Tamgu* object, short idthread, TamguCall* callfunc) {
    if (object->isNumber())
        return aTRUE;
    return aFALSE;
}

Tamgu* CommonIsvector(Tamgu* object, short idthread, TamguCall* callfunc) {
    if (object->isVectorContainer())
        return aTRUE;
    return aFALSE;
}

Tamgu* CommonIsmap(Tamgu* object, short idthread, TamguCall* callfunc) {
    if (object->isMapContainer())
        return aTRUE;
    return aFALSE;
}


Tamgu* CommonInfo(Tamgu* object, short idthread, TamguCall* callfunc) {
    Tamgu* methodname = callfunc->Evaluate(0, aNULL, idthread);
    string s = methodname->String();

    if (globalTamgu->procedureinfos.find(s) != globalTamgu->procedureinfos.end())
        return globalTamgu->Providestring(globalTamgu->procedureinfos[s]);

    if (globalTamgu->commoninfos.find(s) != globalTamgu->commoninfos.end())
        return globalTamgu->Providestring(globalTamgu->commoninfos[s]);

    if (globalTamgu->symbolIds.find(s) == globalTamgu->symbolIds.end())
        return globalTamgu->Providestring("Unknown method");

    short idmethod = globalTamgu->Getid(s);

    s = object->Info(s);
    
    if (s == "Unknown method")
        return globalTamgu->Providewithstring(s);


    string val;
    if (globalTamgu->returntypes.check(idmethod)) {
        val = globalTamgu->idSymbols[globalTamgu->returntypes[idmethod]];
        val += " : ";
    }
    else
        val = "any : ";
    s = val + s;
    
    return globalTamgu->Providewithstring(s);
}

Tamgu* CommonMethods(Tamgu* object, short idthread, TamguCall* callfunc) {
    Tamgusvector* vs = globalTamgu->Providesvector();
    object->Methods(vs);
    vs->Sort(false);
    return vs;
}

Tamgu* CommonExtension(Tamgu* object, short idthread, TamguCall* callfunc) {
    //First we need to find the right function from the right frame...
    short name = callfunc->Name();
    short t = object->Type();
    TamguFrame* frame = globalTamgu->extensions[t];

    //We have two cases of error
    if (frame == NULL) {
        //One no extension exists for this type, however, the variable might have been accessed through a self or a let variable
        //we check if the Extension was attributed by error...
        //The "Execute" here has been implemented to call the TamguCallMethod::Get...
        if (globalTamgu->checkarity(t,name, callfunc->Size()))
            return callfunc->Execute(aNULL, object, idthread);

        stringstream message;
        message << e_no_extension_exists << globalTamgu->Getsymbol(t) << "' object";
        return globalTamgu->Returnerror(message.str(), idthread);
    }

    if (frame->Declaration(name) == NULL) {
        //Second an extension exists for this type, however, the variable might have been accessed through a self or a let variable
        //we check if actually there is a method with that name, which is part of the object itself...
        //Again, the CommonExtension could have been attributed by error...
        //The "Execute" here has been implemented to call the TamguCallMethod::Get...
        if (globalTamgu->checkarity(t,name, callfunc->Size()))
            return callfunc->Execute(aNULL, object, idthread);

        stringstream message;
        message << "The method '" << globalTamgu->Getsymbol(name)
            << "' does not exist for a '" << globalTamgu->Getsymbol(t) << "' object";
        return globalTamgu->Returnerror(message.str(), idthread);
    }


    //Then we intialize this variable with object
    globalTamgu->Storevariable(idthread, frame->Name(), object);
    globalTamgu->Storevariable(idthread, frame->Typeframe(), object);

    if (!callfunc->arguments.size()) {
        TamguCallFunction0 call(frame->Declaration(name));        
        object = call.Eval(aNULL, aNULL, idthread);
    }
    else {
        TamguCallFunction call(frame->Declaration(name));
        call.arguments = callfunc->arguments;
        
        object = call.Eval(aNULL, aNULL, idthread);
    }
    globalTamgu->Removevariable(idthread, frame->Name());
    globalTamgu->Removevariable(idthread, frame->Typeframe());

    //We have returned our value in object, which we protect...
    return object;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------

Exporting void TamguGlobal::RecordOneProcedure(string name, string info, TamguProcedure p, unsigned long arity, short retype) {
    short idname = Getid(name);
    procedures[idname] = p;
    arities[idname] = arity;
    procedureinfos[name] = info;

    if (retype != a_null)
        returntypes[idname] = retype;
}

Exporting void TamguGlobal::RecordCommon(string name, string info, TamguProcedure p, unsigned long arity, short retype) {
    short idname = Getid(name);
    commons[idname] = p;
    commoninfos[name] = info;
    
    arities[idname] = arity;
    if (allmethods.find(idname) == allmethods.end())
        allmethods[idname] = arity;
    else
        allmethods[idname] |= arity;

    if (retype != a_null)
        returntypes[idname] = retype;
}

Exporting void TamguGlobal::RecordArity(short type, short name, unsigned long arity) {
    methods[type][name] = arity;
    if (allmethods.check(name))
        allmethods[name] |= arity;
    else
        allmethods[name] = arity;
}

Exporting void TamguGlobal::RecordCompatibilities(short type) {
    compatibilities[type][type] = true;
    strictcompatibilities[type][type] = true;

    compatibilities[a_call][type] = true;
    compatibilities[a_universal][type] = true;
    compatibilities[a_let][type] = true;
    compatibilities[a_self][type] = true;

    strictcompatibilities[a_call][type] = true;
    strictcompatibilities[a_universal][type] = true;
    strictcompatibilities[a_let][type] = true;
    strictcompatibilities[a_self][type] = true;
}

Exporting void TamguGlobal::RecordMethods(short type, basebin_hash<unsigned long>& exported) {
    basebin_hash<unsigned long>::iterator it;
    for (it = exported.begin(); it != exported.end(); it++) {
        methods[type][it->first] = it->second;
        if (allmethods.check(it->first))
            allmethods[it->first] |= it->second;
        else
            allmethods[it->first] = it->second;
    }

    compatibilities[type][type] = true;
    strictcompatibilities[type][type] = true;

    compatibilities[a_call][type] = true;
    compatibilities[a_universal][type] = true;
    compatibilities[a_let][type] = true;
    compatibilities[a_self][type] = true;

    strictcompatibilities[a_call][type] = true;
    strictcompatibilities[a_universal][type] = true;
    strictcompatibilities[a_let][type] = true;
    strictcompatibilities[a_self][type] = true;
}

Exporting void TamguGlobal::RecordMethods(short type, short name, unsigned long arity) {
    methods[type][name] = arity;
    if (allmethods.check(name))
        allmethods[name] |= arity;
    else
        allmethods[name] = arity;
    
    RecordCommon(Getsymbol(name), "extension", CommonExtension, arity);
    extensionmethods[name] = true;

    compatibilities[type][type] = true;
    strictcompatibilities[type][type] = true;

    compatibilities[a_call][type] = true;
    compatibilities[a_universal][type] = true;
    compatibilities[a_let][type] = true;
    compatibilities[a_self][type] = true;

    strictcompatibilities[a_call][type] = true;
    strictcompatibilities[a_universal][type] = true;
    strictcompatibilities[a_let][type] = true;
    strictcompatibilities[a_self][type] = true;
}

Exporting void TamguGlobal::RecordProcedures() {
    systemfunctions["_loosecompatibility"] = true;
    systemfunctions["_erroronkey"] = true;
    systemfunctions["_setstacksize"] = true;
    systemfunctions["_setmaxthreads"] = true;
    systemfunctions["_setjoinedlimit"] = true;
    systemfunctions["_setenv"] = true;
    systemfunctions["_setmaxrange"] = true;
    systemfunctions["use"] = true;
    systemfunctions["loadin"] = true;
    systemfunctions["loadfacts"] = true;
    systemfunctions["_setvalidfeatures"] = true;
    systemfunctions["grammar_macros"] = true;

    RecordOneProcedure("_setstacksize", "Set the maximum size of the stack", ProcStackSize, P_ONE);
    RecordOneProcedure("_setmaxthreads", "Set the maximum number of threads in memory",ProcMaxThreads, P_ONE);
    RecordOneProcedure("_setmaxrange", "Set the maximum value to build a value 'range'", ProcMaxMaxRange, P_ONE);
    RecordOneProcedure("_exit", "Exit the current program", ProcExit, P_NONE);
    RecordOneProcedure("_erroronkey", "When true, a wrong key access will throw an exception", ProcErrorOnKey, P_ONE);
    RecordOneProcedure("_loosecompatibility","When true, functions can be called with some loose compatibility between 'string' and 'number' types", ProcLooseCompatibility, P_ONE);
    RecordOneProcedure("_setenv", "Set an environment variable at compile time.", ProcSetEnv, P_TWO);
    RecordOneProcedure("_forcelocks", "If false, it can remove locks on containers in threads", ProcForceLocks, P_ONE);
    RecordOneProcedure("_lispmode", "Activate Lisp Mode. Every instruction will be analysed as a Lisp instruction", ProcLispmode, P_NONE | P_ONE);

    //Randomized distribution
    RecordOneProcedure("random_choice", "random_choice(int nb, vector v): returns a list of nb values picked out of v", Proc_random_choice, P_TWO);

    //Uniform distributions
    RecordOneProcedure("uniform_int", "uniform_int(int nb, int a = 0, int b = max_value): Produces random integer values i, uniformly distributed on the closed interval [a, b], that is, distributed according to the discrete probability function.", Proc_uniform_int, P_ONE | P_TWO | P_THREE);
    RecordOneProcedure("uniform_real", "uniform_real(int nb, float a=0, float b=1): Produces random floating-point values x, uniformly distributed on the interval [a, b), that is, distributed according to the probability density function.", Proc_uniform_real, P_ONE | P_TWO | P_THREE);

    //Bernoulli distributions
    RecordOneProcedure("bernoulli_distribution", "bernoulli_distribution(int nb, float probability = 0.5): A discrete distribution having two possible outcomes labelled by n=0 and n=1 in which n=1 (\"success\") occurs with probability p and n=0 (\"failure\") occurs with probability q=1-p, where 0<p<1.", Proc_bernoulli_distribution, P_ONE | P_TWO);
    RecordOneProcedure("binomial_distribution", "binomial_distribution(int nb,int t = 1, float p = 0.5): The discrete probability distribution of the number of successes in a sequence of n independent experiments, each asking a yes–no question.", Proc_binomial_distribution, P_ONE|P_TWO|P_THREE);
    RecordOneProcedure("negative_binomial_distribution", "negative_binomial_distribution(int nb, int k = 1, float p=0.5): A discrete probability distribution that models the number of failures in a sequence of independent and identically distributed Bernoulli trials before a specified (non-random) number of successes (denoted r) occurs.", Proc_negative_binomial_distribution, P_ONE|P_TWO|P_THREE);
    RecordOneProcedure("geometric_distribution", "geometric_distribution(int nb, float p = 0.5):A discrete probability distribution that describes the chances of achieving success in a series of independent trials, each having two possible outcomes.", Proc_geometric_distribution, P_ONE | P_TWO);

    //Poisson distributions
    RecordOneProcedure("poisson_distribution", "poisson_distribution(int nb, float mean = 1): The discrete probability distribution of obtaining exactly k successes out of Bernoulli trials (where the result of each Bernoulli trial is true with probability p and false with probability q=1-p).", Proc_poisson_distribution, P_ONE | P_TWO);
    RecordOneProcedure("exponential_distribution", "exponential_distribution(int nb,float lambda = 1): The probability distribution of the time between events in a Poisson point process, i.e., a process in which events occur continuously and independently at a constant average rate.", Proc_exponential_distribution, P_TWO);
    RecordOneProcedure("gamma_distribution", "gamma_distribution(int nb, float alpha = 1, float beta = 1): A general type of statistical distribution that is related to the beta distribution and arises naturally in processes for which the waiting times between Poisson distributed events are relevant.", Proc_gamma_distribution, P_THREE);
    RecordOneProcedure("weibull_distribution", "weibull_distribution(int nb, float a = 1, float b = 1): A continuous probability distribution that can fit an extensive range of distribution shapes. It can also model skewed data.", Proc_weibull_distribution, P_ONE|P_TWO|P_THREE);
    
    RecordOneProcedure("extreme_value_distribution", "extreme_value_distribution(int nb, float a = 0, float b = 1): The Extreme Value Distribution usually refers to the distribution of the minimum of a large number of unbounded random observations", Proc_extreme_value_distribution, P_ONE|P_TWO|P_THREE);

    //Normal distributions
    RecordOneProcedure("normal_distribution", "normal_distribution(int nb, float mean = 0, float stddev = 1): In a normal distribution, data is symmetrically distributed with no skew. When plotted on a graph, the data follows a bell shape", Proc_normal_distribution, P_ONE|P_TWO|P_THREE);
    RecordOneProcedure("lognormal_distribution", "lognormal_distribution(int nb, float m = 0, float s = 1): The log-normal distribution is the probability distribution of a random variable whose logarithm follows a normal distribution", Proc_lognormal_distribution, P_ONE|P_TWO|P_THREE);
    RecordOneProcedure("chi_squared_distribution", "chi_squared_distribution(int nb, float n = 1): The chi-squared distribution with degrees of freedom is the distribution of a sum of the squares of independent standard normal random variables", Proc_chi_squared_distribution, P_ONE | P_TWO);
    RecordOneProcedure("cauchy_distribution", "cauchy_distribution(int nb, float a=0, float b=1): The Cauchy distribution, named after Augustin Cauchy, is a continuous probability distribution. It is also known as the Lorentz distribution (after Hendrik Lorentz), Cauchy–Lorentz distribution, Lorentz(ian) function, or Breit–Wigner distribution", Proc_cauchy_distribution, P_ONE | P_TWO | P_THREE);
    RecordOneProcedure("fisher_distribution", "fisher_distribution(int nb, float m=1.0, float n=1.0): Fisher distribution may refer to any of several probability distributions named after Ronald Fisher: Behrens–Fisher distribution; Fisher's noncentral hypergeometric distribution; Fisher's z-distribution; Fisher's fiducial distribution; Fisher–Bingham distribution; F-distribution, also called Fisher–Snedecor distribution or Fisher F-distribution; Fisher–Tippett distribution; Von Mises–Fisher distribution on a sphere", Proc_fisher_distribution, P_ONE|P_TWO|P_THREE);
    RecordOneProcedure("student_distribution", "student_distribution(int nb, float n=1.0): The Student's t-distribution is a continuous probability distribution that is often encountered in statistics (e.g., in hypothesis tests about the mean)", Proc_student_distribution, P_ONE|P_TWO);

    //Sampling distributions
    RecordOneProcedure("discrete_distribution", "discrete_distribution(int nb, ivector il): A discrete distribution is a distribution of data in statistics that has discrete values. Discrete values are countable, finite, non-negative integers", Proc_discrete_distribution, P_TWO);
    RecordOneProcedure("piecewise_constant_distribution", "piecewise_constant_distribution(int nb, fvector firstb, fvector firstw):: piecewise_constant_distribution produces random floating-point numbers, which are uniformly distributed within each of the several subintervals [b i, b i+1), each with its own weight w i.", Proc_piecewise_constant_distribution, P_THREE);
    RecordOneProcedure("piecewise_linear_distribution", "piecewise_linear_distribution(int nb, fvector firstb, fvector firstw): piecewise_linear_distribution produces random floating-point numbers, which are distributed according to a linear probability density function within each of the several subintervals [b i, b i+1)", Proc_piecewise_linear_distribution, P_THREE);

    RecordOneProcedure("uuid", "Generate a Universally Unique ID", Proc_UUID, P_NONE);

    //-------------------------
    RecordOneProcedure("_nop", "Generation a concatenation of string (internal use only)", ProcNop, P_FULL);
    RecordOneProcedure("_nbthreads", "Returns the number of active threads in memory", ProcNbthreads, P_NONE);

    RecordOneProcedure("_setvalidfeatures", "Defines the valid features for a synode", ProcSetValidFeatures, P_ONE);

    RecordOneProcedure("grammar_macros", "Definition of macros in grammars", &ProcPatterns, P_TWO);

    RecordOneProcedure("_breakpoint", "break point (internal use)", ProcBreakPoint, P_ONE);
    RecordOneProcedure("_getdefaulttokenizerules","Returns the list of default tokenization rules", ProcGetTokenizeRules, P_NONE);

    RecordOneProcedure("_symbols", "Returns a list of all symbols (variables and functions) declared in the code", ProcSymbols, P_NONE);
    RecordOneProcedure("_types", "Returns a list of all available types in Tamgu", ProcTypes, P_NONE);
    
    RecordOneProcedure("_poolstats", "Returns statistics information about pools. Many data structures such as vectors and maps are stored in pools of values", ProcPoolStats, P_NONE);

    RecordOneProcedure("nope", "Do nothing", ProcNope, P_NONE);
    RecordOneProcedure("use", "Load a library", ProcUse, P_ONE);
    RecordOneProcedure("loadin", "Load the code of a file into the current memory space (merging)", ProcLoadin, P_ONE);
    RecordOneProcedure("loadfacts", "Fast loadings of facts for the internal predicate engine", ProcLoadfacts, P_ONE);
    
    RecordOneProcedure("alldefinitions", "Returns the list of all methods and their definition", ProcAllDefinitions, P_NONE);
    RecordOneProcedure("allobjects", "Returns the list of all pre-defined symbols", ProcAllObjects, P_NONE);
    RecordOneProcedure("allobjectsbytype", "Returns the list of all pre-defined symbols as a map indexed on types",  ProcAllObjectByType, P_NONE);
    
    RecordOneProcedure("version", "Returns Tamgu version", ProcVersion, P_NONE);
    RecordOneProcedure("_mirrordisplay", "internal use", ProcMirrorDisplay, P_NONE);


    RecordOneProcedure("prettystring", "Returns a string that corresponds to a 'print'", ProcPrettify, P_ONE | P_TWO | P_THREE | P_FOUR | P_FIVE);
    RecordOneProcedure("print", "Displays a list of values on screen", ProcPrint, P_FULL);
    RecordOneProcedure("println", "Displays a list of values on screen, with a CR at the end. Commas separate elements with a space.", ProcPrintLN, P_FULL);
    RecordOneProcedure("printerr", "Displays a list of values on the error output.", ProcPrinterr, P_FULL);
    RecordOneProcedure("printlnerr", "Displays a list of values on the error output, with a CR at the end. Commas separate elements with a space.", ProcPrinterrLN, P_FULL);
    RecordOneProcedure("printj", "Displays with a join on values given by the arguments.", &ProcPrintJoin, P_ONE | P_TWO | P_THREE | P_FOUR | P_FIVE);
    RecordOneProcedure("printjerr", "Displays on the error output with a join on values given by the arguments.", &ProcPrintJoinErr, P_ONE | P_TWO | P_THREE | P_FOUR | P_FIVE);
    RecordOneProcedure("printjln", "Displays with a join on values given by the arguments.", &ProcPrintJoinLN, P_ONE | P_TWO | P_THREE | P_FOUR | P_FIVE);
    RecordOneProcedure("printjlnerr", "Displays on the error output with a join on values given by the arguments.", &ProcPrintJoinErrLN, P_ONE | P_TWO | P_THREE | P_FOUR | P_FIVE);

    RecordOneProcedure("_variables", "Returns the list of active variables", ProcVariables, P_ONE);
    RecordOneProcedure("_eval", "Evaluates a string as code", ProcEval, P_ONE | P_TWO);
    RecordOneProcedure("_evalfunction", "Evaluates a string as a function call", ProcEvalFunction, P_ONE);
    
    RecordOneProcedure("abs", "Returns the absolute value of a number", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("acos", "Returns the arccosine of a number", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("acosh", "Returns the inverse hyperbolic cosine of a number", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("asin", "Returns the arcsine of a number", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("asinh", "Returns the inverse hyperbolic sine of a number", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("atan", "Returns the arctangent of a number", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("atanh", "Returns the inverse hyperbolic tangent of a number", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("cbrt", "Returns the cube root of a number", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("∛", "Returns the cube root of a number", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("cos", "Returns the cosine of an angle (in radians)", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("cosh", "Returns the hyperbolic cosine of an angle (in radians)", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("erf", "Returns the error function value for a given number", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("erfc", "Returns the complementary error function value for a given number", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("exp", "Returns Euler's number raised to the power of a given number", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("exp2", "Returns 2 raised to the power of a given number", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("expm1", "Returns exp(x) - 1 for a given number x", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("floor", "Rounds down to the nearest integer value that is less than or equal to x.", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("lgamma", "Returns the natural logarithm of the absolute value of gamma function for x.", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("ln", "Returns the natural logarithm (base e) of x.", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("log", "Returns the natural logarithm (base e) of x.", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("log1p","Returns log(1 + x) for x.",&ProcMath,P_ONE,a_float);
    RecordOneProcedure("log2","Returns log base 2 of x.",&ProcMath,P_ONE,a_float);
    RecordOneProcedure("logb","Extracts exponent from floating-point numbers.",&ProcMath,P_ONE,a_float);
    RecordOneProcedure("nearbyint","Rounds to nearest integer value in floating-point format.",&ProcMath,P_ONE,a_float);
    RecordOneProcedure("rint","Rounds to nearest integer value in floating-point format.",&ProcMath,P_ONE,a_float);
    RecordOneProcedure("round","Rounds to nearest integer value in floating-point format.",&ProcMath,P_ONE,a_float);
    RecordOneProcedure("sin","Returns sine of an angle (in radians).",&ProcMath,P_ONE,a_float);
    RecordOneProcedure("sinh","Returns hyperbolic sine of an angle (in radians).",&ProcMath,P_ONE,a_float);
    RecordOneProcedure("sqrt","Returns square root of x.",&ProcMath,P_ONE,a_float);
    RecordOneProcedure("√","Returns square root of x.",&ProcMath,P_ONE,a_float);
    RecordOneProcedure("tan","Returns tangent of an angle (in radians).",&ProcMath,P_ONE,a_float);
    RecordOneProcedure("tanh","Returns hyperbolic tangent of an angle (in radians).",&ProcMath,P_ONE,a_float);
    RecordOneProcedure("tgamma","Gamma function for x.",&ProcMath,P_ONE,a_float);
    RecordOneProcedure("trunc","Truncates fractional part from floating-point numbers.",&ProcMath,P_ONE,a_float);

    RecordOneProcedure("∑", "Sum of the values of a vector", &ProcSum, P_ONE | P_TWO | P_THREE);
    RecordOneProcedure("∏", "Product of the values of a vector", &ProcProduct, P_ONE | P_TWO | P_THREE);

    RecordOneProcedure("sum", "Sum of the values of a vector", &ProcFullSum, P_ATLEASTONE);
    RecordOneProcedure("product", "Product of the values of a vector", &ProcFullProduct, P_ATLEASTONE);

    //Error management...
    RecordOneProcedure("catch", "Catches an exception", ProcCatch, P_ONE);
    RecordOneProcedure("raise", "Raises an exception", ProcRaise, P_ONE);

    RecordOneProcedure("GPSdistance", "Returns the GPS distance between two points on Earth", ProcGPSDistance, P_FOUR | P_FIVE);

    //Range including interval definition
    RecordOneProcedure("range", "Returns a range of values", ProcRange, P_TWO | P_THREE);
    RecordOneProcedure("ord", "Returns the list of Unicode code for a string of characters", ProcOrd, P_ONE); //this specific one is used with range...

    RecordOneProcedure("random", "Returns a random value", ProcRandom, P_NONE | P_ONE, a_float);
    RecordOneProcedure("a_random", "Returns a random value", Proca_Random, P_NONE | P_ONE, a_float);

    RecordOneProcedure("pause", "Pauses the code in seconds", ProcPause, P_ONE);

    RecordOneProcedure("kget", "Reads a value on keyboard", ProcKeyboardGet, P_NONE | P_ONE);

    RecordOneProcedure("redirectoutput", "Redirects the output to a file", ProcRedictectOutput, P_TWO);
    RecordOneProcedure("restateoutput", "Puts back the output to the screen", ProcRestateOutput, P_TWO);

    RecordOneProcedure("max", "Maximum value in a list of arguments", ProcMax, P_ATLEASTTWO);
    RecordOneProcedure("min", "Minimum value in a list of arguments", ProcMin, P_ATLEASTTWO);
    RecordOneProcedure("not", "Negation of a Boolean expression", &ProcNegation, P_ONE, a_boolean);

    RecordOneProcedure("base", "base(value, base): Converts an element from base 10 to base b", ProcBase, P_TWO);

    //Threading
    RecordOneProcedure("_threadhandle", "Returns the internal handle of the current thread", ProcThreadhandle, P_NONE);
    RecordOneProcedure("_threadid", "Returns the internal id of the current thread", ProcThreadid, P_NONE);
    RecordOneProcedure("waitonjoined", "Waits until all joinded threads are terminated", ProcWaitOnJoin, P_NONE | P_ONE | P_TWO);
    RecordOneProcedure("lock", "Lock", ProcLock, P_ONE);
    RecordOneProcedure("unlock", "Unlock", ProcUnlock, P_ONE);
    RecordOneProcedure("wait", "Waits until a cast is sent (a string)", ProcWaiton, P_ONE);
    RecordOneProcedure("waitonfalse", "Waits until a variable has a false interpretation", ProcWaitonfalse, P_ONE);
    RecordOneProcedure("cast", "Activates all threads that have been waiting on this 'string'.", ProcCast, P_NONE | P_ONE);
    RecordOneProcedure("sleep", "Sleeps for a given amount of ms", ProcSleep, P_ONE);

    RecordOneProcedure("_info", "Returns the infos of a method",ProcGetinfo, P_ONE);

    //Common methods to all objects...
    RecordCommon("json", "Returns the json container string", CommonJSon, P_NONE, a_string);
    RecordCommon("string", "Returns the string conversion", CommonString, P_NONE, a_string);
    RecordCommon("ustring", "Returns the ustring conversion", CommonUString, P_NONE, a_ustring);
    RecordCommon("int", "Returns the integer conversion", CommonInt, P_NONE, a_int);
    RecordCommon("float", "Returns the float conversion", CommonFloat, P_NONE, a_float);
    RecordCommon("long", "Returns the long conversion", CommonLong, P_NONE, a_long);
    RecordCommon("decimal","Returns the decimal conversion", CommonDecimal, P_NONE, a_decimal);
    RecordCommon("short","Returns the short conversion", CommonShort, P_NONE, a_short);
    RecordCommon("vector", "Returns the vector conversion", CommonVector, P_NONE);
    RecordCommon("map", "Returns the map conversion", CommonMap, P_NONE);

    RecordCommon("size", "Returns the size of the object", CommonSize, P_NONE, a_int);
    RecordCommon("info", "Returns information of the method", CommonInfo, P_ONE, a_string);
    RecordCommon("methods", "Returns the list of methods associated to the object", CommonMethods, P_NONE);
    RecordCommon("type", "Returns the type of the object", CommonType, P_NONE, a_string);
    RecordCommon("isa", "Checks if the object is of the same type as the value", CommonISA, P_ONE);

    RecordCommon("isstring", "Checks if the object is a string", CommonIsstring, P_NONE);
    RecordCommon("isvector", "Checks if the object is a vector", CommonIsvector, P_NONE);
    RecordCommon("ismap", "Checks if the object is a map", CommonIsmap, P_NONE);
    RecordCommon("iscontainer", "Checks if the object is a container", CommonIscontainer, P_NONE);
    RecordCommon("isnumber", "Checks if the object is a number", CommonIsnumber, P_NONE);

    RecordCommon("mark", "Marks this object while traversing an object container", CommonMark, P_NONE | P_ONE);
    RecordCommon("unmark", "Unmarks this object while traversing an object container", CommonUnMark, P_NONE);

}
