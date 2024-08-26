###################################################################
include ../Makefile.in
#------------------------------------------------------------
sourcelispe = tamgulispe.cxx lispe.cxx eval.cxx elements.cxx lispetools.cxx systems.cxx maths.cxx strings.cxx randoms.cxx rgx.cxx sockets.cxx composing.cxx ontology.cxx sets.cxx lists.cxx dictionaries.cxx straight_eval.cxx tensors.cxx
#------------------------------------------------------------
LBINPATH = ../$(BINPATH)
LOBJPATH = ../$(LIBOBJPATH)
#------------------------------------------------------------
objectlispe = $(sourcelispe:%.cxx=$(LOBJPATH)/lispe/%.o)
#------------------------------------------------------------
TamguBASICFLAGS = -w -c -fPIC $(Optim) -DUNIX $(MULTIGA) $(INTELINT) $(REGEX) $(SPECFLAGS)
TamguCFLAGS = -std=c99 $(TamguBASICFLAGS)
TamguFLAGS = -I../../lispe/include $(C++11Flag) $(TamguBASICFLAGS)
#------------------------------------------------------------
Tamguincludes= -Iinclude -I../include $(INCLUDEPATH)
#------------------------------------------------------------
$(LOBJPATH)/lispe/%.o: src/%.cxx
	$(COMP) -Iinclude $(TamguFLAGS) $(Tamguincludes) $< -o $@
#------------------------------------------------------------
tamgulispe: install $(objectlispe)
	$(COMPPLUSPLUS) -shared -o $(LBINPATH)/libtamgulispe.so $(objectlispe) $(SYSTEMSPATH) $(LIBREGEX) -L$(LBINPATH) -ltamgu

install:
	mkdir -p $(LOBJPATH)/lispe

clean:
	rm -f $(LOBJPATH)/lispe/*.o

