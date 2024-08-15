###################################################################
include ../Makefile.in
#------------------------------------------------------------
sourceallmaps = tamgua_mapff.cxx tamgua_mapfi.cxx tamgua_mapfs.cxx tamgua_mapfu.cxx \
tamgua_mapif.cxx tamgua_mapii.cxx tamgua_mapis.cxx tamgua_mapiu.cxx \
tamgua_mapsf.cxx tamgua_mapsi.cxx tamgua_mapss.cxx tamgua_mapuf.cxx \
tamgua_mapui.cxx tamgua_mapuu.cxx tamguallmaps.cxx tamguprimemapf.cxx \
tamgubinmap.cxx tamgubinmapf.cxx tamgubinmapi.cxx tamgubinmapl.cxx	tamgubinmaps.cxx tamgubinmapu.cxx \
tamguprimemapff.cxx tamguprimemapfi.cxx tamguprimemapfl.cxx tamguprimemapfs.cxx \
tamguprimemapfu.cxx tamguprimemapi.cxx tamguprimemapif.cxx tamguprimemapii.cxx \
tamguprimemapis.cxx tamguprimemapiu.cxx tamguprimemapl.cxx tamguprimemaplf.cxx \
tamguprimemapll.cxx tamguprimemapls.cxx tamguprimemaplu.cxx tamguprimemapu.cxx \
tamguprimemapuf.cxx tamguprimemapui.cxx tamguprimemapul.cxx tamguprimemapuu.cxx \
tamgutreemapff.cxx tamgutreemapfi.cxx tamgutreemapfl.cxx \
tamgutreemapfs.cxx tamgutreemapfu.cxx \
tamgutreemapii.cxx tamgutreemapis.cxx tamgutreemapiu.cxx tamgutreemapl.cxx \
tamgutreemaplf.cxx tamgutreemapll.cxx tamgutreemaplu.cxx tamgutreemapu.cxx \
tamgutreemapuf.cxx tamgutreemapui.cxx tamgutreemapul.cxx tamgutreemapuu.cxx
#------------------------------------------------------------
LBINPATH = ../$(BINPATH)
LOBJPATH = ../$(LIBOBJPATH)
#------------------------------------------------------------
objectallmaps = $(sourceallmaps:%.cxx=$(LOBJPATH)/allmaps/%.o)
#------------------------------------------------------------
TamguBASICFLAGS = -w -c -fPIC -O3 -DUNIX $(MULTIGA) $(INTELINT) $(REGEX) $(SPECFLAGS)
TamguCFLAGS = -std=c99 $(TamguBASICFLAGS)
TamguFLAGS = $(C++11Flag) $(TamguBASICFLAGS)
TAMGULIBPATH = ../$(BINPATH)
#------------------------------------------------------------
Tamguincludes= -I../include $(INCLUDEPATH)
#------------------------------------------------------------
$(LOBJPATH)/allmaps/%.o: src/%.cxx
	$(COMP) -Iinclude $(TamguFLAGS) $(Tamguincludes) $< -o $@
#------------------------------------------------------------
all: install $(objectallmaps)
	$(COMPPLUSPLUS) -shared -o $(LBINPATH)/liballmaps.so $(objectallmaps) $(SYSTEMSPATH) $(LIBREGEX) -L$(LBINPATH) -ltamgu

install:
	mkdir -p $(LOBJPATH)/allmaps

clean:
	rm -f $(LOBJPATH)/allmaps/*.o
	rm -f $(LBINPATH)/liballmaps.so

