###################################################################
include ../Makefile.in
################ To compile Tamgu#################################
SOURCEW2V = tamguword2vec.cxx
#------------------------------------------------------------
LBINPATH = ../$(BINPATH)
LOBJPATH = ../$(LIBOBJPATH)
###################################################################
OBJECTW2V = $(SOURCEW2V:%.cxx=$(LOBJPATH)/wordvec/%.o)
#------------------------------------------------------------
TamguBASICFLAGS = -w -c -fPIC -O3 -DUNIX $(MULTIGA) $(INTELINT) $(REGEX) $(SPECFLAGS)
TamguCFLAGS = -std=c99 $(TamguBASICFLAGS)
TamguFLAGS = $(C++11Flag) $(TamguBASICFLAGS)
Tamguincludes= -Iinclude -I../include
#------------------------------------------------------------
$(LOBJPATH)/wordvec/%.o: src/%.cxx
	$(COMP) -Iinclude $(TamguFLAGS) $(Tamguincludes) $< -o $@
#------------------------------------------------------------
word2vec: install $(OBJECTW2V)
	$(COMPPLUSPLUS) -shared -o $(LBINPATH)/libword2vec.so $(OBJECTW2V) -L$(LBINPATH) -ltamgu

install:
	mkdir -p $(LOBJPATH)/wordvec

clean:
	rm -f $(LOBJPATH)/wordvec/*.o
	rm -f $(LBINPATH)/libword2vec.so

