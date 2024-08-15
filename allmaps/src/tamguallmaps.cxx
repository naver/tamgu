/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguallmaps.cxx
 Date       : 2019/05/24
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamguversion.h"

#include "tamgua_mapff.h"
#include "tamgua_mapfi.h"
#include "tamgua_mapfs.h"
#include "tamgua_mapfu.h"
#include "tamgua_mapif.h"
#include "tamgua_mapii.h"
#include "tamgua_mapis.h"
#include "tamgua_mapiu.h"
#include "tamgua_mapsf.h"
#include "tamgua_mapsi.h"
#include "tamgua_mapss.h"
#include "tamgua_mapuf.h"
#include "tamgua_mapui.h"
#include "tamgua_mapuu.h"
#include "tamgubinmap.h"
#include "tamgubinmapf.h"
#include "tamgubinmapi.h"
#include "tamgubinmaps.h"
#include "tamgubinmapu.h"
#include "tamgubinmapl.h"
#include "tamguprimemapf.h"
#include "tamguprimemapff.h"
#include "tamguprimemapfi.h"
#include "tamguprimemapfl.h"
#include "tamguprimemapfs.h"
#include "tamguprimemapfu.h"
#include "tamguprimemapi.h"
#include "tamguprimemapif.h"
#include "tamguprimemapii.h"
#include "tamguprimemapis.h"
#include "tamguprimemapiu.h"
#include "tamguprimemapl.h"
#include "tamguprimemaplf.h"
#include "tamguprimemapll.h"
#include "tamguprimemapls.h"
#include "tamguprimemaplu.h"
#include "tamguprimemapu.h"
#include "tamguprimemapuf.h"
#include "tamguprimemapui.h"
#include "tamguprimemapul.h"
#include "tamguprimemapuu.h"
#include "tamgutreemapff.h"
#include "tamgutreemapfi.h"
#include "tamgutreemapfl.h"
#include "tamgutreemapfs.h"
#include "tamgutreemapfu.h"
#include "tamgutreemapii.h"
#include "tamgutreemapis.h"
#include "tamgutreemapiu.h"
#include "tamgutreemapl.h"
#include "tamgutreemaplf.h"
#include "tamgutreemapll.h"
#include "tamgutreemaplu.h"
#include "tamgutreemapu.h"
#include "tamgutreemapuf.h"
#include "tamgutreemapui.h"
#include "tamgutreemapul.h"
#include "tamgutreemapuu.h"


//------------------------------------------------------------------------------------------------------------------
//If you need to implement an external library... Otherwise remove it...
//When Tamgu (탐구) loads a library, it looks for this method in particular, which then make this object available into Tamgu (탐구)
extern "C" {
    Exporting bool allmaps_InitialisationModule(TamguGlobal* global, string version) {
        if (version != TamguVersion())
            return false;

        global->Update();

        Tamgua_mapff::InitialisationModule(global, version);
        Tamgua_mapfi::InitialisationModule(global, version);
        Tamgua_mapfs::InitialisationModule(global, version);
        Tamgua_mapfu::InitialisationModule(global, version);
        Tamgua_mapif::InitialisationModule(global, version);
        Tamgua_mapii::InitialisationModule(global, version);
        Tamgua_mapis::InitialisationModule(global, version);
        Tamgua_mapiu::InitialisationModule(global, version);
        Tamgua_mapsf::InitialisationModule(global, version);
        Tamgua_mapsi::InitialisationModule(global, version);
        Tamgua_mapss::InitialisationModule(global, version);
        Tamgua_mapuf::InitialisationModule(global, version);
        Tamgua_mapui::InitialisationModule(global, version);
        Tamgua_mapuu::InitialisationModule(global, version);
        Tamgubinmap::InitialisationModule(global, version);
        Tamgubinmapf::InitialisationModule(global, version);
        Tamgubinmapi::InitialisationModule(global, version);
        Tamgubinmaps::InitialisationModule(global, version);
        Tamgubinmapu::InitialisationModule(global, version);
        Tamgubinmapl::InitialisationModule(global, version);
        Tamguprimemapf::InitialisationModule(global, version);
        Tamguprimemapff::InitialisationModule(global, version);
        Tamguprimemapfi::InitialisationModule(global, version);
        Tamguprimemapfl::InitialisationModule(global, version);
        Tamguprimemapfs::InitialisationModule(global, version);
        Tamguprimemapfu::InitialisationModule(global, version);
        Tamguprimemapi::InitialisationModule(global, version);
        Tamguprimemapif::InitialisationModule(global, version);
        Tamguprimemapii::InitialisationModule(global, version);
        Tamguprimemapis::InitialisationModule(global, version);
        Tamguprimemapiu::InitialisationModule(global, version);
        Tamguprimemapl::InitialisationModule(global, version);
        Tamguprimemaplf::InitialisationModule(global, version);
        Tamguprimemapll::InitialisationModule(global, version);
        Tamguprimemapls::InitialisationModule(global, version);
        Tamguprimemaplu::InitialisationModule(global, version);
        Tamguprimemapu::InitialisationModule(global, version);
        Tamguprimemapuf::InitialisationModule(global, version);
        Tamguprimemapui::InitialisationModule(global, version);
        Tamguprimemapul::InitialisationModule(global, version);
        Tamguprimemapuu::InitialisationModule(global, version);
        Tamgutreemapff::InitialisationModule(global, version);
        Tamgutreemapfi::InitialisationModule(global, version);
        Tamgutreemapfl::InitialisationModule(global, version);
        Tamgutreemapfs::InitialisationModule(global, version);
        Tamgutreemapfu::InitialisationModule(global, version);
        Tamgutreemapii::InitialisationModule(global, version);
        Tamgutreemapis::InitialisationModule(global, version);
        Tamgutreemapiu::InitialisationModule(global, version);
        Tamgutreemapl::InitialisationModule(global, version);
        Tamgutreemaplf::InitialisationModule(global, version);
        Tamgutreemapll::InitialisationModule(global, version);
        Tamgutreemaplu::InitialisationModule(global, version);
        Tamgutreemapu::InitialisationModule(global, version);
        Tamgutreemapuf::InitialisationModule(global, version);
        Tamgutreemapui::InitialisationModule(global, version);
        Tamgutreemapul::InitialisationModule(global, version);
        Tamgutreemapuu::InitialisationModule(global, version);

        global->RecordCompatibilities();
        return true;
    }
}
