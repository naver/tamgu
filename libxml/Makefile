###################################################################
include ../Makefile.in
################ To compile Tamgu#################################
SOURCEXMLCXX = tamguxml.cxx tamguxmldoc.cxx
#------------------------------------------------------------
LBINPATH = ../$(BINPATH)
LOBJPATH = ../$(LIBOBJPATH)
###################################################################
OBJECTXMLCXX = $(SOURCEXMLCXX:%.cxx=$(LOBJPATH)/xml/%.o)
#------------------------------------------------------------
TamguBASICFLAGS = -w -c -fPIC -O3 -DUNIX $(MULTIGA) $(INTELINT) $(REGEX) $(SPECFLAGS)
TamguCFLAGS = -std=c99 $(TamguBASICFLAGS)
TamguFLAGS = $(C++11Flag) $(TamguBASICFLAGS)
Tamguincludes= -Iinclude -I../include
#------------------------------------------------------------
$(LOBJPATH)/xml/%.o: src/%.cxx
	$(COMP) -Iinclude $(TamguFLAGS) -DXMLPARSE $(Tamguincludes) $< -o $@
#------------------------------------------------------------
xml: install $(OBJECTXMLCXX)
	$(COMPPLUSPLUS) -shared -o $(LBINPATH)/libxml.so $(OBJECTXMLCXX) -L$(LBINPATH) -ltamgu $(SYSTEMSPATH) -lxml2

install:
	mkdir -p $(LOBJPATH)/xml

clean:
	rm -f $(LOBJPATH)/xml/*.o

