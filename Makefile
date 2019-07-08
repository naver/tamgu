###################################################################
include Makefile.in
################ To compile Tamgu#################################
SOURCETamgu = tamgu.cxx conversion.cxx tamgutamgu.cxx tamgubool.cxx tamgubvector.cxx tamgubyte.cxx tamgudate.cxx tamgudecimal.cxx\
tamgufile.cxx tamguufile.cxx tamgufloat.cxx tamgufmatrix.cxx tamgufraction.cxx tamguframeinstance.cxx tamgufvector.cxx tamgudvector.cxx\
tamgusynode.cxx tamgutable.cxx\
tamgugrammar.cxx tamgutaskell.cxx tamguimatrix.cxx tamguint.cxx tamguiterator.cxx tamguivector.cxx tamgulist.cxx tamgulvector.cxx\
tamgulong.cxx tamgumap.cxx tamgumapf.cxx tamgumapff.cxx tamgumapfi.cxx tamgumapfs.cxx tamgumapfu.cxx tamgumapi.cxx tamgutreemapi.cxx tamgutreemapif.cxx\
tamgumapif.cxx tamgumapii.cxx tamgumapis.cxx tamgumapiu.cxx tamgumapsf.cxx tamgumapsi.cxx tamgumapss.cxx tamgumapu.cxx\
tamgumapuf.cxx tamgumapui.cxx tamgumapuu.cxx tamguprimemap.cxx tamguprimemapsf.cxx tamguprimemapsi.cxx tamguprimemapss.cxx\
tamgushort.cxx tamgusocket.cxx tamgustdin.cxx tamgustring.cxx tamgusvector.cxx tamgusys.cxx tamgutime.cxx tamgutransducer.cxx tamgutreemap.cxx\
tamguustring.cxx tamguuvector.cxx tamguvector.cxx automaton.cxx codeparse.cxx codecompile.cxx codeexecute.cxx codeoperations.cxx\
tamgurawstring.cxx containerrecording.cxx globaltamgu.cxx objectrecording.cxx predicate.cxx predicatefunctions.cxx procedures.cxx tools.cxx\
tamguhvector.cxx tamgumapfl.cxx tamgumapl.cxx tamgumaplf.cxx tamgumapll.cxx tamgumapls.cxx tamgumaplu.cxx tamgumapsl.cxx tamgumapul.cxx\
tamgutreemapsf.cxx tamgutreemapsi.cxx tamgutreemapss.cxx tamguprimemapsl.cxx tamgutreemapls.cxx tamgutreemapsl.cxx tamguautomaton.cxx tamguannotator.cxx
#------------------------------------------------------------
SOURCETAMGUMAIN = maintamgu.cxx jag.cxx
SOURCEJAG = jagmain.cxx jag.cxx conversion.cxx jagrgx.cxx
#------------------------------------------------------------
OBJECTTamgu = $(SOURCETamgu:%.cxx=$(OBJPATH)/%.o)
#------------------------------------------------------------
OBJECTTAMGUMAIN = $(SOURCETAMGUMAIN:%.cxx=$(LIBOBJPATH)/%.o)
OBJECTJAG = $(SOURCEJAG:%.cxx=$(LIBOBJPATH)/jag/%.o)
#------------------------------------------------------------
TamguBASICFLAGS = -w -c -fPIC -O3 -DUNIX $(MULTIGA) $(INTELINT) $(REGEX) $(SPECFLAGS)
TamguCFLAGS = -std=c99 $(TamguBASICFLAGS)
TamguFLAGS = $(C++11Flag) $(TamguBASICFLAGS)

Tamguincludes= -Iinclude -Iinclude/libs $(INCLUDEPATH)

TamguSYSTEMLIBS = $(SYSTEMSPATH) -lpthread $(MACLIBS) $(LIBREGEX) -ldl $(SSLLIB) $(CRYPTOLIB) -lldap

#------------------------------------------------------------
$(OBJPATH)/%.o: src/%.cxx
	$(COMPPLUSPLUS) $(TamguFLAGS) $(Tamguincludes) $< -o $@

$(LIBOBJPATH)/%.o: src/%.cxx
	$(COMPPLUSPLUS) $(TamguFLAGS) $(Tamguincludes) $< -o $@

$(LIBOBJPATH)/%.o: src/libs/%.cxx
	$(COMPPLUSPLUS) $(TamguFLAGS) $(Tamguincludes) $< -o $@

$(LIBOBJPATH)/jag/%.o: src/%.cxx
	$(COMP) -Iinclude $(TamguFLAGS) $< -o $@

#------------------------------------------------------------
libtamgu: $(OBJECTTamgu)
	$(COMPPLUSPLUS) -shared -o $(BINPATH)/libtamgu.so $(OBJECTTamgu) $(SYSTEMSPATH)

# For those who prefer a small executable linked with a dynamic library
#tamgu: $(OBJECTTAMGUMAIN)
#	$(COMPPLUSPLUS) -o $(BINPATH)/tamgu $(OBJECTTAMGUMAIN) -L$(BINPATH) -ltamgu $(TamguSYSTEMLIBS)

# For those who do not want to be bothered with a dynamic library link...
tamgu: $(OBJECTTAMGUMAIN) $(OBJECTTamgu)
	$(COMPPLUSPLUS) -o $(BINPATH)/tamgu $(OBJECTTAMGUMAIN) $(OBJECTTamgu) $(TamguSYSTEMLIBS)

jag: install $(OBJECTJAG)
	$(COMPPLUSPLUS) -o $(BINPATH)/jag $(OBJECTJAG)

lib: install libtamgu
	$(libtamgu)

all: install libtamgu tamgu
	$(libtamgu)
	$(tamgu)

cleanlibs: install
	$(MAKE) -C allmaps clean
	$(MAKE) -C libpython clean
	$(MAKE) -C libsqlite clean
	$(MAKE) -C libcurl clean
	$(MAKE) -C libxml clean
	$(MAKE) -C liblinear clean
	$(MAKE) -C libword2vec clean
	$(MAKE) -C libwapiti clean

install:
	mkdir -p bin
	mkdir -p $(BINPATH)
	mkdir -p objs
	mkdir -p $(OBJPATH)
	mkdir -p libobjs
	mkdir -p $(LIBOBJPATH)
	mkdir -p $(LIBOBJPATH)/jag

cleanall:
	rm -f $(OBJPATH)/*.o
	rm -f $(LIBOBJPATH)/*.o
	rm -f $(BINPATH)/*.so
	rm -f $(BINPATH)/tamgu*
	rm -f $(BINPATH)/jag
	rm -f $(LIBOBJPATH)/jag/*.o

clean:
	rm -f $(OBJPATH)/*.o
	rm -f $(LIBOBJPATH)/*.o
	rm -f $(LIBOBJPATH)/jag/*.o

