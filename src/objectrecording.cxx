/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : objectrecording.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamguversion.h"

//The objects we need to record
#include "tamgustring.h"
#include "tamgufloat.h"
#include "tamgudecimal.h"
#include "tamguint.h"
#include "tamgushort.h"
#include "tamguustring.h"
#include "tamgurawstring.h"
#include "tamgulong.h"
#include "tamguframeinstance.h"
#include "tamgubyte.h"
#include "tamguiterator.h"
#include "tamgufile.h"
#include "tamguufile.h"
#include "constobjects.h"
#include "tamgubool.h"
#include "tamgudate.h"
#include "tamgutime.h"
#include "tamgufraction.h"
#include "tamgusocket.h"
#include "tamgutransducer.h"
#include "tamgusys.h"
#include "tamgugrammar.h"
#include "tamguimatrix.h"
#include "tamgufmatrix.h"
#include "tamgutamgu.h"
#include "predicate.h"
#include "tamgustdin.h"
#include "tamgusynode.h"
#include "tamguannotator.h"
#include "tamgutaskell.h"
#include "tamguchrono.h"
#include "globaltamgu.h"
//-------------------------------------------------------

void TamguGlobal::RecordObjects() {

    newInstance[a_self] = new TamguSelf(this);
    newInstance[a_let] = new TamguLet(this);

    newInstance[a_call] = new TamguFunctionCall(this);

    newInstance[a_term] = new TamguPredicateTerm(this, a_term);

    //These objects do not automatically record into the tracker for late removal...
    Tamgu* a = new TamguLoopString(this);
    newInstance[a_sloop] = a;
    RecordInTracker(a);

    a = new TamguLoopUString(this);
    newInstance[a_uloop] = a;
    RecordInTracker(a);

    a = new TamguLoopInteger(this);
    newInstance[a_iloop] = a;
    RecordInTracker(a);

    a = new TamguLoopLong(this);
    newInstance[a_lloop] = a;
    RecordInTracker(a);

    a = new TamguLoopDecimal(this);
    newInstance[a_dloop] = a;
    RecordInTracker(a);

    a = new TamguLoopFloat(this);
    newInstance[a_floop] = a;
    RecordInTracker(a);

    a = new TamguLoopBoolean;
    newInstance[a_bloop] = a;
    RecordInTracker(a);


    TamguCallFibre::InitialisationModule(this, TamguVersion());

    Tamgutamgu::InitialisationModule(this, TamguVersion());

    Tamgustring::InitialisationModule(this, TamguVersion());
    Tamgurawstring::InitialisationModule(this, TamguVersion());
    Tamguint::InitialisationModule(this, TamguVersion());
    Tamgushort::InitialisationModule(this, TamguVersion());
    Tamgulong::InitialisationModule(this, TamguVersion());
    Tamgudecimal::InitialisationModule(this, TamguVersion());
    Tamgufloat::InitialisationModule(this, TamguVersion());
    Tamgubyte::InitialisationModule(this, TamguVersion());
    Tamguustring::InitialisationModule(this, TamguVersion());
    Tamgubool::InitialisationModule(this, TamguVersion());

    Tamguframeseeder::InitialisationModule(this, TamguVersion());

    Tamguiterator::InitialisationModule(this, TamguVersion());

    Tamgufile::InitialisationModule(this, TamguVersion());
    Tamguufile::InitialisationModule(this, TamguVersion());

    Tamgudate::InitialisationModule(this, TamguVersion());
    Tamgutime::InitialisationModule(this, TamguVersion());
    Tamguchrono::InitialisationModule(this, TamguVersion());

    Tamgufraction::InitialisationModule(this, TamguVersion());
    Tamgusocket::InitialisationModule(this, TamguVersion());

    Tamgutransducer::InitialisationModule(this, TamguVersion());
    Tamgugrammar::InitialisationModule(this, TamguVersion());

    Tamgusys::InitialisationModule(this, TamguVersion());

    Tamguimatrix::InitialisationModule(this, TamguVersion());
    Tamgufmatrix::InitialisationModule(this, TamguVersion());
    Tamgustdin::InitialisationModule(this, TamguVersion());

    //------------------ Predicates ------------------
    TamguPredicate::InitialisationModule(this, TamguVersion());
    Tamgusynode::InitialisationModule(this, TamguVersion());

    Tamguannotator::InitialisationModule(this, TamguVersion());
    Tamguregularexpression::InitialisationModule(this, TamguVersion());
    Tamguposixregularexpression::InitialisationModule(this, TamguVersion());
}


void TamguGlobal::RecordObjectsId() {
    
    TamguCallFibre::Setidtype(this);
    
    Tamgustring::Setidtype(this);
    Tamgurawstring::Setidtype(this);
    Tamguint::Setidtype(this);
    Tamgushort::Setidtype(this);
    Tamgulong::Setidtype(this);
    Tamgudecimal::Setidtype(this);
    Tamgufloat::Setidtype(this);
    Tamgubyte::Setidtype(this);
    Tamguustring::Setidtype(this);
    Tamgubool::Setidtype(this);
    
    Tamguframeseeder::Setidtype(this);
    
    Tamguiterator::Setidtype(this);
    
    Tamgufile::Setidtype(this);
    Tamguufile::Setidtype(this);
    
    Tamgudate::Setidtype(this);
    Tamgutime::Setidtype(this);
    
    Tamgufraction::Setidtype(this);
    Tamgusocket::Setidtype(this);
    
    Tamgutransducer::Setidtype(this);
    Tamgugrammar::Setidtype(this);
    
    Tamgusys::Setidtype(this);
    
    Tamguimatrix::Setidtype(this);
    Tamgufmatrix::Setidtype(this);
    Tamgustdin::Setidtype(this);
    
        //------------------ Predicates ------------------
    TamguPredicate::Setidtype(this);
    Tamgusynode::Setidtype(this);
    
    Tamguannotator::Setidtype(this);
    Tamguregularexpression::Setidtype(this);
}

