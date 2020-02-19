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

#ifdef UNIX
#include <sys/time.h>
#endif

//---------------------------------------------------------
bool store_pattern(uchar,wstring&);
//---------------------------------------------------------
Tamgu* ProcThreadhandle(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

Tamgu* ProcThreadid(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    return globalTamgu->Provideint(idthread);
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
    return globalTamgu->Providefloat(localrandom(mx));
}

Tamgu* Proca_Random(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long mx = 100;
    if (callfunc->Size() == 1)
        mx = callfunc->Evaluate(0, aNULL, idthread)->Float();
    return globalTamgu->Providefloat(a_localrandom(mx));
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
        return globalTamgu->Provideint((long)dis(gen));

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
        return globalTamgu->Providefloat((double)d(gen));

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
        return globalTamgu->Provideint((long)dis(gen));

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
        return globalTamgu->Provideint((long)d(gen));

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
        return globalTamgu->Providefloat((double)d(gen));

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
        return globalTamgu->Provideint((long)d(gen));

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
        return globalTamgu->Returnerror("Expecting a vector as second parameter", idthread);
    
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
        return globalTamgu->Provideint((long)d(gen));

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

    if (tvect->Type() == a_fvector)
        inter = ((Tamgufvector*)tinter)->values;
    else {
        for (i = 0; i < tinter->Size(); i++)
            inter.push_back(tinter->getfloat(i));
    }

    std::piecewise_constant_distribution<double> d(vect.begin(), vect.end(), inter.begin());
    if (nb == 1)
        return globalTamgu->Providefloat((double)d(gen));

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

    if (tvect->Type() == a_fvector)
        inter = ((Tamgufvector*)tinter)->values;
    else {
        for (i = 0; i < tinter->Size(); i++)
            inter.push_back(tinter->getfloat(i));
    }

    std::piecewise_linear_distribution<double> d(vect.begin(), vect.end(), inter.begin());
    if (nb == 1)
        return globalTamgu->Providefloat((double)d(gen));

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

    Tamgufvector* iv = (Tamgufvector*)Selectafvector(contextualpattern);
    if (nb == 1)
        return globalTamgu->Providefloat((double)d(gen));

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
        return globalTamgu->Provideint((long)d(gen));

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

    Tamgufvector* iv = (Tamgufvector*)Selectafvector(contextualpattern);
    if (nb == 1)
        return globalTamgu->Providefloat((double)d(gen));

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
        return globalTamgu->Providefloat((double)d(gen));

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
        return globalTamgu->Providefloat((double)d(gen));

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
        return globalTamgu->Providefloat((double)d(gen));

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
        return globalTamgu->Provideint((long)d(gen));

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
        return globalTamgu->Providefloat((double)d(gen));

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
        return globalTamgu->Providefloat((double)d(gen));

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
        return globalTamgu->Providefloat((double)d(gen));

    Tamgufvector* iv = (Tamgufvector*)Selectafvector(contextualpattern);

    double v;
    for (long i = 0; i < nb; i++) {
        v = d(gen);
        iv->storevalue(v);
    }
    return iv;
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
        //callfunc->name = a_initial;
        object->Setreference();

        Tamgu* a = ((Tamguframeinstance*)object)->MethodInitial(contextualpattern, idthread, callfunc);
        //Tamgu* a = object->CallMethod(a_initial, contextualpattern, idthread, callfunc);

        if (globalTamgu->Error(idthread)) {
            object->Resetreference();
            a->Release();
            return globalTamgu->Errorobject(idthread);
        }

        object->Protect();
        a->Release();
    }
    else {
        if (callfunc->Size() != 0) {
            object->Release();
            return globalTamgu->Returnerror("Wrong frame initialization", idthread);
        }
    }
    object->Postinstantiation(idthread, true);
    return object;
}
//------------------------------------------------------------------------------------------------------------------------
Tamgu* ProcAllObjects(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgusvector* vs = (Tamgusvector*)Selectasvector(contextualpattern);
    globalTamgu->TamguAllObjects(vs->values);
    map<string, bool> inter;
    for (long i = 0; i < vs->values.size(); i++)
        inter[vs->values[i]] = true;


    vs->values.clear();
    for (auto& it : inter)
        vs->values.push_back(it.first);
    return vs;
}

//------------------------------------------------------------------------------------------------------------------------
#ifdef GARBAGEFORDEBUG
void Garbaging(vector<Tamgu*>& issues, vector<long>& idissues);
#endif

Tamgu* ProcPoolStats(Tamgu* context, short idthread, TamguCall* callfunc) {
    Tamgumapsi* stats;


    if (context->Type() == Tamgumapsi::idtype && context->isAffectation()) {
        context->Clear();
        stats = (Tamgumapsi*)context;
    }
    else
        stats = new Tamgumapsi;

#ifdef GARBAGEFORDEBUG
    vector<Tamgu*> issues;
    vector<long> idissues;
    Garbaging(issues, idissues);
    stats->values["garbage"] = idissues.size();
#endif

    stats->values["index"] = globalTamgu->indexreservoire.size();
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
    stats->values["index_idx"] = globalTamgu->indexidx;
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

    return stats;
}

//------------------------------------------------------------------------------------------------------------------------

Tamgu* ProcBreakPoint(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long i = callfunc->Evaluate(0, aNULL, idthread)->Integer();

    return globalTamgu->Provideint(i);
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
Tamgu* ProcExit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    executionbreak = true;
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
    globalTamgu->globalLOCK = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
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
        message << "Cannot open file: " << filename;
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
    acode->Compile(code);

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
//------------------------------------------------------------------------------------------------------------------------
Tamgu* ProcEvalFunction(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string code = callfunc->Evaluate(0, contextualpattern, idthread)->String();

    Locking _lock(globalTamgu);
    TamguCode* acode = globalTamgu->Getcurrentcode();
    long lastrecorded;
    
    lastrecorded = globalTamgu->Trackedsize();
    Tamgu* compiled = acode->Compilefunction(code);
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
        return globalTamgu->Returnerror("Cannot compile this function declaration", idthread);
    }

    return compiled;
}

Tamgu* ProcEval(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (contextualpattern->Action() == a_call)
        return ProcEvalFunction(contextualpattern, idthread, callfunc);

    string code = callfunc->Evaluate(0, contextualpattern, idthread)->String();

    Locking _lock(globalTamgu);
    TamguCode* acode = globalTamgu->Getcurrentcode();
    long lastinstruction = acode->InstructionSize();
    Tamgu* ci = globalTamgu->threads[idthread].currentinstruction;
    long lastrecorded;
    
    lastrecorded = globalTamgu->Trackedsize();
    Tamgu* compiled = acode->Compilefunction(code);
    if (compiled == NULL || compiled->isError() || globalTamgu->Error(idthread)) {
        globalTamgu->threads[idthread].currentinstruction = callfunc;

        if (compiled != NULL && compiled->isError())
            return compiled;
        if (globalTamgu->Error(idthread))
            return globalTamgu->Errorobject(idthread);
        return globalTamgu->Returnerror("Cannot compile this code", idthread);
    }

    if (compiled->isFunction() || compiled->Type() == a_calltaskell)
        return compiled;

    acode->compilemode = true;
    Tamgu* a = acode->Execute(lastinstruction, idthread);

    globalTamgu->Cleanfrom(lastrecorded);
    globalTamgu->threads[idthread].currentinstruction = ci;
    
    if (a->investigate == is_return)
        return a->Returned(idthread);

    return a;
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
    return globalTamgu->Provideint(output);
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
            return globalTamgu->Returnerror("Argument shoud be a function", idthread);
        if (callfunc->Size() == 2) {
            object = callfunc->Evaluate(1, aNULL, idthread);
            object->Setreference();
        }
    }

    
    while (globalTamgu->threads[idthread].nbjoined) {
        
        if (func != NULL) {
            TamguCallFunction call(func);
            if (object != NULL)
                call.arguments.push_back(object);
            
            Tamgu* a = call.Eval(aNULL, aNULL, idthread);
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

Tamgu* ProcSum(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
    if (!v->isVectorContainer() && v->Type() != a_list)
        return globalTamgu->Returnerror("Expecting a vector container", idthread);

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
        return globalTamgu->Returnerror("Expecting a vector container", idthread);

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

Tamgu* ProcPatterns(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string key= callfunc->Evaluate(0, contextualpattern, idthread)->String();
    if (key.size() != 1)
        return globalTamgu->Returnerror("Expecting a key of 1 ASCII character");
    
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
    Tamgu* val = globalTamgu->Providestring(resstr);
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
    
    if (kcont->isVectorContainer()) {
        if (callfunc->Size() == 3) {
            nb = callfunc->Evaluate(2, context, idthread)->Integer();
            if (nb <= 0)
                nb = -1;
        }
        
        long it;
        for (it = 0; it < kcont->Size(); it++) {
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
        if (callfunc->Size() == 4) {
            nb = callfunc->Evaluate(3, context, idthread)->Integer();
            if (nb <= 0)
                nb = -1;
        }
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
            return globalTamgu->Returnerror("RANGE over actual capacity", idthread);
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
        return globalTamgu->Returnerror("RANGE over actual capacity", idthread);
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
            return globalTamgu->Returnerror("RANGE over actual capacity", idthread);
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
        return globalTamgu->Returnerror("RANGE over actual capacity", idthread);
    }

    if (initial->Typevariable() == a_ustring) {
        wstring l = initial->UString();
        wstring r = boundary->UString();
        if (l.size() != 1 || r.size() != 1)
            return globalTamgu->Returnerror("String should be one character long in RANGE", idthread);
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
        return globalTamgu->Returnerror("String should be one character long in RANGE", idthread);
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
            return globalTamgu->Provideint((int)s[0]);

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

        return globalTamgu->Provideint((int)s[0]);
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
        return globalTamgu->Returnerror("Unknown lock", idthread);
    
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
        return globalTamgu->Returnerror("Unknown variable", idthread);
    
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
    return globalTamgu->Providestring(s);
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
            return globalTamgu->Provideint(b);
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
        return globalTamgu->Provideint(v);
    }

    n = ke->Integer();

    while (n) {
        w = caracs[n%b] + w;
        n /= b;
    }

    return globalTamgu->Provideustring(w);
}

//----------------------------------------------------------------------------------------------------------
//<distance(L1,l1,L2,l2) = r | let a=L1.radian(), let b=L2.radian(), let c=l1.radian(), let d=l2.radian(), let r= acos(cos(a)*cos(b)*cos(abs(c-d))+sin(a)*sin(b))*6371>

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

    return globalTamgu->Providefloat(sqrt(distance));
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
    return globalTamgu->Providefloat(globalTamgu->mathFunctions[callfunc->Name()](v));
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
    return globalTamgu->Provideint(object->Integer());
}

Tamgu* CommonFloat(Tamgu* object, short idthread, TamguCall* callfunc) {
    return globalTamgu->Providefloat(object->Float());
}

Tamgu* CommonShort(Tamgu* object, short idthread, TamguCall* callfunc) {
    return new Tamgushort(object->Short());
}

Tamgu* CommonLong(Tamgu* object, short idthread, TamguCall* callfunc) {
    return new Tamgulong(object->Long());
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
        return globalTamgu->Provideint(object->CommonSize());
    return globalTamgu->Provideint(object->Size());
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
    if (globalTamgu->commoninfos.find(s) != globalTamgu->commoninfos.end())
        return globalTamgu->Providestring(globalTamgu->commoninfos[s]);

    if (globalTamgu->symbolIds.find(s) == globalTamgu->symbolIds.end())
        return globalTamgu->Providestring("Unknown method");

    short idmethod = globalTamgu->Getid(s);

    s = object->Info(s);
    
    if (s == "Unknown method")
        return globalTamgu->Providestring(s);


    string val;
    if (globalTamgu->returntypes.check(idmethod)) {
        val = globalTamgu->idSymbols[globalTamgu->returntypes[idmethod]];
        val += " : ";
    }
    else
        val = "any : ";
    s = val + s;
    
    return globalTamgu->Providestring(s);
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
        if (globalTamgu->methods.check(t) && Arity(globalTamgu->methods[t][name], callfunc->Size()))
            return callfunc->Execute(aNULL, object, idthread);

        stringstream message;
        message << "No 'extension' exists for a '" << globalTamgu->Getsymbol(t) << "' object";
        return globalTamgu->Returnerror(message.str(), idthread);
    }

    if (frame->Declaration(name) == NULL) {
        //Second an extension exists for this type, however, the variable might have been accessed through a self or a let variable
        //we check if actually there is a method with that name, which is part of the object itself...
        //Again, the CommonExtension could have been attributed by error...
        //The "Execute" here has been implemented to call the TamguCallMethod::Get...
        if (globalTamgu->methods.check(t) && Arity(globalTamgu->methods[t][name], callfunc->Size()))
            return callfunc->Execute(aNULL, object, idthread);

        stringstream message;
        message << "The method '" << globalTamgu->Getsymbol(name)
            << "' does not exist for a '" << globalTamgu->Getsymbol(t) << "' object";
        return globalTamgu->Returnerror(message.str(), idthread);
    }


    //Then we intialize this variable with object
    globalTamgu->Storevariable(idthread, frame->Name(), object);
    globalTamgu->Storevariable(idthread, frame->Typeframe(), object);

    TamguCallFunction call(frame->Declaration(name));
    call.arguments = callfunc->arguments;

    object = call.Eval(aNULL, aNULL, idthread);

    globalTamgu->Removevariable(idthread, frame->Name());
    globalTamgu->Removevariable(idthread, frame->Typeframe());

    //We have returned our value in object, which we protect...
    return object;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------

Exporting void TamguGlobal::RecordOneProcedure(string name, TamguProcedure p, unsigned long arity, short retype) {
    short idname = Getid(name);
    procedures[idname] = p;
    arities[idname] = arity;

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

Exporting void TamguGlobal::RecordMethods(short type, bin_hash<unsigned long>& exported) {
    bin_hash<unsigned long>::iterator it;
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
    systemfunctions["_erroronkey"] = true;
    systemfunctions["_setstacksize"] = true;
    systemfunctions["_setthreadlimit"] = true;
    systemfunctions["_setjoinedlimit"] = true;
    systemfunctions["_setenv"] = true;
    systemfunctions["_setmaxrange"] = true;
    systemfunctions["use"] = true;
    systemfunctions["loadin"] = true;
    systemfunctions["_setvalidfeatures"] = true;
    systemfunctions["grammar_macros"] = true;

    RecordOneProcedure("_setstacksize", ProcStackSize, P_ONE);
    RecordOneProcedure("_setmaxthreads", ProcMaxThreads, P_ONE);
    RecordOneProcedure("_setmaxrange", ProcMaxMaxRange, P_ONE);
    RecordOneProcedure("_exit", ProcExit, P_NONE);
    RecordOneProcedure("_erroronkey", ProcErrorOnKey, P_ONE);
    RecordOneProcedure("_setenv", ProcSetEnv, P_TWO);
    RecordOneProcedure("_forcelocks", ProcForceLocks, P_ONE);
    RecordOneProcedure("_lispmode", ProcLispmode, P_NONE | P_ONE);

    //Randomized distribution
    RecordOneProcedure("random_choice", Proc_random_choice, P_TWO);

    //Uniform distributions
    RecordOneProcedure("uniform_int", Proc_uniform_int, P_ONE | P_TWO | P_THREE);
    RecordOneProcedure("uniform_real", Proc_uniform_real, P_ONE | P_TWO | P_THREE);

    //Bernoulli distributions
    RecordOneProcedure("bernoulli_distribution", Proc_bernoulli_distribution, P_ONE | P_TWO);
    RecordOneProcedure("binomial_distribution", Proc_binomial_distribution, P_ONE|P_TWO|P_THREE);
    RecordOneProcedure("negative_binomial_distribution", Proc_negative_binomial_distribution, P_ONE|P_TWO|P_THREE);
    RecordOneProcedure("geometric_distribution", Proc_geometric_distribution, P_ONE | P_TWO);

    //Poisson distributions
    RecordOneProcedure("poisson_distribution", Proc_poisson_distribution, P_ONE | P_TWO);
    RecordOneProcedure("exponential_distribution", Proc_exponential_distribution, P_TWO);
    RecordOneProcedure("gamma_distribution", Proc_gamma_distribution, P_THREE);
    RecordOneProcedure("weibull_distribution", Proc_weibull_distribution, P_ONE|P_TWO|P_THREE);
    RecordOneProcedure("extreme_value_distribution", Proc_extreme_value_distribution, P_ONE|P_TWO|P_THREE);

    //Normal distributions
    RecordOneProcedure("normal_distribution", Proc_normal_distribution, P_ONE|P_TWO|P_THREE);
    RecordOneProcedure("lognormal_distribution", Proc_lognormal_distribution, P_ONE|P_TWO|P_THREE);
    RecordOneProcedure("chi_squared_distribution", Proc_chi_squared_distribution, P_ONE | P_TWO);
    RecordOneProcedure("cauchy_distribution", Proc_cauchy_distribution, P_ONE | P_TWO | P_THREE);
    RecordOneProcedure("fisher_distribution", Proc_fisher_distribution, P_ONE|P_TWO|P_THREE);
    RecordOneProcedure("student_distribution", Proc_student_distribution, P_ONE|P_TWO);

    //Sampling distributions
    RecordOneProcedure("discrete_distribution", Proc_discrete_distribution, P_TWO);
    RecordOneProcedure("piecewise_constant_distribution", Proc_piecewise_constant_distribution, P_THREE);
    RecordOneProcedure("piecewise_linear_distribution", Proc_piecewise_linear_distribution, P_THREE);


    //-------------------------
    RecordOneProcedure("_nbthreads", ProcNbthreads, P_NONE);

    RecordOneProcedure("_setvalidfeatures", ProcSetValidFeatures, P_ONE);

    RecordOneProcedure("grammar_macros", &ProcPatterns, P_TWO);

    RecordOneProcedure("_breakpoint", ProcBreakPoint, P_ONE);
    RecordOneProcedure("_getdefaulttokenizerules", ProcGetTokenizeRules, P_NONE);

    RecordOneProcedure("_symbols", ProcSymbols, P_NONE);

    RecordOneProcedure("_poolstats", ProcPoolStats, P_NONE);

    RecordOneProcedure("nope", ProcNope, P_NONE);
    RecordOneProcedure("use", ProcUse, P_ONE);
    RecordOneProcedure("loadin", ProcLoadin, P_ONE);

    RecordOneProcedure("allobjects", ProcAllObjects, P_NONE);

    RecordOneProcedure("version", ProcVersion, P_NONE);
    RecordOneProcedure("_mirrordisplay", ProcMirrorDisplay, P_NONE);

    RecordOneProcedure("emojis", ProcEmojis, P_NONE);

    RecordOneProcedure("print", ProcPrint, P_FULL);
    RecordOneProcedure("println", ProcPrintLN, P_FULL);
    RecordOneProcedure("printerr", ProcPrinterr, P_FULL);
    RecordOneProcedure("printlnerr", ProcPrinterrLN, P_FULL);
    RecordOneProcedure("printj", &ProcPrintJoin, P_ONE | P_TWO | P_THREE | P_FOUR);
    RecordOneProcedure("printjerr", &ProcPrintJoinErr, P_ONE | P_TWO | P_THREE | P_FOUR);
    RecordOneProcedure("printjln", &ProcPrintJoinLN, P_ONE | P_TWO | P_THREE | P_FOUR);
    RecordOneProcedure("printjlnerr", &ProcPrintJoinErrLN, P_ONE | P_TWO | P_THREE | P_FOUR);

    RecordOneProcedure("_eval", ProcEval, P_ONE);
    RecordOneProcedure("_evalfunction", ProcEvalFunction, P_ONE);
    
    RecordOneProcedure("abs", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("acos", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("acosh", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("asin", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("asinh", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("atan", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("atanh", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("cbrt", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("∛", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("cos", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("cosh", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("erf", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("erfc", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("exp", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("exp2", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("expm1", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("floor", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("lgamma", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("ln", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("log", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("log1p", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("log2", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("logb", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("nearbyint", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("rint", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("round", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("sin", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("sinh", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("sqrt", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("√", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("tan", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("tanh", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("tgamma", &ProcMath, P_ONE, a_float);
    RecordOneProcedure("trunc", &ProcMath, P_ONE, a_float);

    RecordOneProcedure("∑", &ProcSum, P_ONE | P_TWO | P_THREE);
    RecordOneProcedure("∏", &ProcProduct, P_ONE | P_TWO | P_THREE);

    RecordOneProcedure("sum", &ProcSum, P_ONE | P_TWO | P_THREE);
    RecordOneProcedure("product", &ProcProduct, P_ONE | P_TWO | P_THREE);


    //Error management...
    RecordOneProcedure("catch", ProcCatch, P_ONE);
    RecordOneProcedure("raise", ProcRaise, P_ONE);

    RecordOneProcedure("GPSdistance", &ProcGPSDistance, P_FOUR | P_FIVE);

    //Range including interval definition
    RecordOneProcedure("range", ProcRange, P_TWO | P_THREE);
    RecordOneProcedure("ord", ProcOrd, P_ONE); //this specific one is used with range...

    RecordOneProcedure("random", ProcRandom, P_NONE | P_ONE, a_float);
    RecordOneProcedure("a_random", Proca_Random, P_NONE | P_ONE, a_float);

    RecordOneProcedure("pause", ProcPause, P_ONE);

    RecordOneProcedure("kget", ProcKeyboardGet, P_NONE | P_ONE);

    RecordOneProcedure("redirectoutput", ProcRedictectOutput, P_TWO);
    RecordOneProcedure("restateoutput", ProcRestateOutput, P_TWO);

    RecordOneProcedure("max", ProcMax, P_ATLEASTTWO);
    RecordOneProcedure("min", ProcMin, P_ATLEASTTWO);
    RecordOneProcedure("not", &ProcNegation, P_ONE, a_boolean);

    RecordOneProcedure("base", ProcBase, P_TWO);

    //Threading
    RecordOneProcedure("_threadhandle", ProcThreadhandle, P_NONE);
    RecordOneProcedure("_threadid", ProcThreadid, P_NONE);
    RecordOneProcedure("waitonjoined", ProcWaitOnJoin, P_NONE | P_ONE | P_TWO);
    RecordOneProcedure("lock", ProcLock, P_ONE);
    RecordOneProcedure("unlock", ProcUnlock, P_ONE);
    RecordOneProcedure("wait", ProcWaiton, P_ONE);
    RecordOneProcedure("waitonfalse", ProcWaitonfalse, P_ONE);
    RecordOneProcedure("cast", ProcCast, P_NONE | P_ONE);
    RecordOneProcedure("sleep", ProcSleep, P_ONE);

    RecordOneProcedure("_info", ProcGetinfo, P_ONE);

    //Common methods to all objects...
    RecordCommon("json", "return the json container string", CommonJSon, P_NONE, a_string);
    RecordCommon("string", "return the string conversion", CommonString, P_NONE, a_string);
    RecordCommon("ustring", "return the ustring conversion", CommonUString, P_NONE, a_ustring);
    RecordCommon("int", "return the integer conversion", CommonInt, P_NONE, a_int);
    RecordCommon("float", "return the float conversion", CommonFloat, P_NONE, a_float);
    RecordCommon("long", "return the long conversion", CommonLong, P_NONE, a_long);
    RecordCommon("decimal","return the decimal conversion", CommonDecimal, P_NONE, a_decimal);
    RecordCommon("short","return the short conversion", CommonShort, P_NONE, a_short);
    RecordCommon("vector", "return the vector conversion", CommonVector, P_NONE);
    RecordCommon("map", "return the map conversion", CommonMap, P_NONE);

    RecordCommon("size", "return the size of the object", CommonSize, P_NONE, a_int);
    RecordCommon("info", "return information of the method", CommonInfo, P_ONE, a_string);
    RecordCommon("methods", "return the list of methods associated to the object", CommonMethods, P_NONE);
    RecordCommon("type", "return the type of the object", CommonType, P_NONE, a_string);
    RecordCommon("isa", "check if the object is of the same type as the value", CommonISA, P_ONE);

    RecordCommon("isstring", "check if the object is a string", CommonIsstring, P_NONE);
    RecordCommon("isvector", "check if the object is a vector", CommonIsvector, P_NONE);
    RecordCommon("ismap", "check if the object is a map", CommonIsmap, P_NONE);
    RecordCommon("iscontainer", "check if the object is a container", CommonIscontainer, P_NONE);
    RecordCommon("isnumber", "check if the object is a number", CommonIsnumber, P_NONE);

    RecordCommon("mark", "mark this object while traversing an object container", CommonMark, P_NONE | P_ONE);
    RecordCommon("unmark", "unmark this object while traversing an object container", CommonUnMark, P_NONE);

}
