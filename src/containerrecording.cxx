/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 * This file can only be used with the Tamgu (탐구) library or the executable
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : containerrecording.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamguversion.h"

#include "tamguvector.h"
#include "tamgulist.h"
#include "tamgutable.h"

#include "tamguivector.h"
#include "tamgubvector.h"
#include "tamguuvector.h"
#include "tamgusvector.h"
#include "tamgufvector.h"
#include "tamgudvector.h"
#include "tamguhvector.h"
#include "tamgulvector.h"

#include "tamgumap.h"
#include "tamgutreemap.h"
#include "tamguprimemap.h"

#include "tamgumapsf.h"
#include "tamgumapsi.h"
#include "tamgumapss.h"
#include "tamgumapsl.h"
#include "tamgumapf.h"
#include "tamgumapff.h"
#include "tamgumapfi.h"
#include "tamgumapfl.h"
#include "tamgumapfs.h"
#include "tamgumapfu.h"
#include "tamgumapi.h"
#include "tamgumapif.h"
#include "tamgumapii.h"
#include "tamgumapis.h"
#include "tamgumapiu.h"
#include "tamgumapl.h"
#include "tamgumaplf.h"
#include "tamgumapll.h"
#include "tamgumapls.h"
#include "tamgumaplu.h"
#include "tamgumapu.h"
#include "tamgumapuf.h"
#include "tamgumapui.h"
#include "tamgumapul.h"
#include "tamgumapuu.h"

#include "tamgutreemapsf.h"
#include "tamgutreemapsi.h"
#include "tamgutreemapss.h"
#include "tamgutreemapsl.h"
#include "tamgutreemapls.h"
#include "tamgutreemapif.h"
#include "tamgutreemapi.h"

#include "tamguprimemapsf.h"
#include "tamguprimemapsi.h"
#include "tamguprimemapss.h"
#include "tamguprimemapsl.h"

#include "tamgulisp.h"

void TamguGlobal::RecordContainers() {

    Tamgulisp::InitialisationModule(this, TamguVersion());

    Tamguvector::InitialisationModule(this, TamguVersion());

    Tamguivector::InitialisationModule(this, TamguVersion());
	Tamgubvector::InitialisationModule(this, TamguVersion());
	Tamgufvector::InitialisationModule(this, TamguVersion());
	Tamgudvector::InitialisationModule(this, TamguVersion());
	Tamguhvector::InitialisationModule(this, TamguVersion());
	Tamgulvector::InitialisationModule(this, TamguVersion());
	Tamguuvector::InitialisationModule(this, TamguVersion());
	Tamgusvector::InitialisationModule(this, TamguVersion());

	Tamgulist::InitialisationModule(this, TamguVersion());
	Tamgutable::InitialisationModule(this, TamguVersion());

	Tamgumap::InitialisationModule(this, TamguVersion());

	Tamgumapsf::InitialisationModule(this, TamguVersion());
	Tamgumapsi::InitialisationModule(this, TamguVersion());
	Tamgumapss::InitialisationModule(this, TamguVersion());
	Tamgumapsl::InitialisationModule(this, TamguVersion());
    Tamgumapf::InitialisationModule(this, TamguVersion());
    Tamgumapff::InitialisationModule(this, TamguVersion());
    Tamgumapfi::InitialisationModule(this, TamguVersion());
    Tamgumapfl::InitialisationModule(this, TamguVersion());
    Tamgumapfs::InitialisationModule(this, TamguVersion());
    Tamgumapfu::InitialisationModule(this, TamguVersion());
    Tamgumapi::InitialisationModule(this, TamguVersion());
    Tamgumapif::InitialisationModule(this, TamguVersion());
    Tamgumapii::InitialisationModule(this, TamguVersion());
    Tamgumapis::InitialisationModule(this, TamguVersion());
    Tamgumapiu::InitialisationModule(this, TamguVersion());
    Tamgumapl::InitialisationModule(this, TamguVersion());
    Tamgumaplf::InitialisationModule(this, TamguVersion());
    Tamgumapll::InitialisationModule(this, TamguVersion());
    Tamgumapls::InitialisationModule(this, TamguVersion());
    Tamgumaplu::InitialisationModule(this, TamguVersion());
    Tamgumapu::InitialisationModule(this, TamguVersion());
    Tamgumapuf::InitialisationModule(this, TamguVersion());
    Tamgumapui::InitialisationModule(this, TamguVersion());
    Tamgumapul::InitialisationModule(this, TamguVersion());
    Tamgumapuu::InitialisationModule(this, TamguVersion());
    
    Tamgutreemap::InitialisationModule(this, TamguVersion());
    Tamgutreemapi::InitialisationModule(this, TamguVersion());
    Tamgutreemapls::InitialisationModule(this, TamguVersion());
    Tamgutreemapif::InitialisationModule(this, TamguVersion());

	Tamgutreemapsf::InitialisationModule(this, TamguVersion());
	Tamgutreemapsi::InitialisationModule(this, TamguVersion());
    Tamgutreemapss::InitialisationModule(this, TamguVersion());
	Tamgutreemapsl::InitialisationModule(this, TamguVersion());

    Tamguprimemap::InitialisationModule(this, TamguVersion());
    Tamguprimemapsf::InitialisationModule(this, TamguVersion());
	Tamguprimemapsi::InitialisationModule(this, TamguVersion());
	Tamguprimemapss::InitialisationModule(this, TamguVersion());
	Tamguprimemapsl::InitialisationModule(this, TamguVersion());
    
	mapnewInstances[b_int][b_int] = newInstance[Tamgumapii::idtype];
	mapnewInstances[b_int][b_long] = newInstance[Tamgumapll::idtype];
	mapnewInstances[b_int][b_ustring] = newInstance[Tamgumapiu::idtype];
	mapnewInstances[b_int][b_float] = newInstance[Tamgumapif::idtype];
	mapnewInstances[b_int][b_string] = newInstance[Tamgumapis::idtype];
	mapnewInstances[b_int][b_none] = newInstance[Tamgumapi::idtype];
	mapnewInstances[b_long][b_int] = newInstance[Tamgumapll::idtype];
	mapnewInstances[b_long][b_long] = newInstance[Tamgumapll::idtype];
	mapnewInstances[b_long][b_ustring] = newInstance[Tamgumaplu::idtype];
	mapnewInstances[b_long][b_float] = newInstance[Tamgumaplf::idtype];
	mapnewInstances[b_long][b_string] = newInstance[Tamgumapls::idtype];
	mapnewInstances[b_long][b_none] = newInstance[Tamgumapl::idtype];
	mapnewInstances[b_ustring][b_int] = newInstance[Tamgumapui::idtype];
	mapnewInstances[b_ustring][b_long] = newInstance[Tamgumapul::idtype];
	mapnewInstances[b_ustring][b_ustring] = newInstance[Tamgumapuu::idtype];
	mapnewInstances[b_ustring][b_float] = newInstance[Tamgumapuf::idtype];
	mapnewInstances[b_ustring][b_string] = newInstance[Tamgumapuu::idtype];
	mapnewInstances[b_ustring][b_none] = newInstance[Tamgumapu::idtype];
	mapnewInstances[b_float][b_int] = newInstance[Tamgumapfi::idtype];
	mapnewInstances[b_float][b_long] = newInstance[Tamgumapfl::idtype];
	mapnewInstances[b_float][b_ustring] = newInstance[Tamgumapfu::idtype];
	mapnewInstances[b_float][b_float] = newInstance[Tamgumapff::idtype];
	mapnewInstances[b_float][b_string] = newInstance[Tamgumapfs::idtype];
	mapnewInstances[b_float][b_none] = newInstance[Tamgumapf::idtype];
	mapnewInstances[b_string][b_int] = newInstance[Tamgumapsi::idtype];
	mapnewInstances[b_string][b_long] = newInstance[Tamgumapsl::idtype];
	mapnewInstances[b_string][b_ustring] = newInstance[Tamgumapuu::idtype];
	mapnewInstances[b_string][b_float] = newInstance[Tamgumapsf::idtype];
	mapnewInstances[b_string][b_string] = newInstance[a_mapss];
	mapnewInstances[b_string][b_none] = newInstance[a_map];

    framecontainer_predeclared["vector<int>"] = a_ivector;
    framecontainer_predeclared["vector<float>"] = a_fvector;
    framecontainer_predeclared["vector<long>"] = a_lvector;
    framecontainer_predeclared["vector<short>"] = a_hvector;
    framecontainer_predeclared["vector<string>"] = a_svector;
    framecontainer_predeclared["vector<ustring>"] = a_uvector;

    framecontainer_predeclared["map<int>"] = Tamgumapsi::idtype;
    framecontainer_predeclared["map<float>"] = Tamgumapsf::idtype;
    framecontainer_predeclared["map<long>"] = Tamgumapsl::idtype;
    framecontainer_predeclared["map<string>"] = a_mapss;
    framecontainer_predeclared["map<ustring>"] = Tamgumapuu::idtype;

    framecontainer_predeclared["treemap<int>"] = Tamgutreemapsi::idtype;
    framecontainer_predeclared["treemap<float>"] = Tamgutreemapsf::idtype;
    framecontainer_predeclared["treemap<long>"] = Tamgutreemapsl::idtype;
    framecontainer_predeclared["treemap<string>"] = Tamgutreemapss::idtype;

    framecontainer_predeclared["primemap<int>"] = Tamguprimemapsi::idtype;
    framecontainer_predeclared["primemap<float>"] = Tamguprimemapsf::idtype;
    framecontainer_predeclared["primemap<long>"] = Tamguprimemapsl::idtype;
    framecontainer_predeclared["primemap<string>"] = Tamguprimemapss::idtype;
}
