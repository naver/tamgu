###################################################################
include ../Makefile.in
################ To compile Tamgu#################################
SOURCEPYTHON = tamgupython.cxx
#------------------------------------------------------------
LBINPATH = ../$(BINPATH)
LOBJPATH = ../$(LIBOBJPATH)
###################################################################
OBJECTPYTHON = $(SOURCEPYTHON:%.cxx=$(LOBJPATH)/pytamgu/%.o)
#------------------------------------------------------------
TamguBASICFLAGS = -w -c -fPIC -O3 -DUNIX $(MULTIGA) $(INTELINT) $(REGEX) $(SPECFLAGS)
TamguCFLAGS = -std=c99 $(TamguBASICFLAGS)
TamguFLAGS = $(C++11Flag) $(TamguBASICFLAGS)
Tamguincludes=  $(INCLUDEPYTHON) -Iinclude -I../include
#------------------------------------------------------------
$(LOBJPATH)/pytamgu/%.o: src/%.cxx
	$(COMP) -Iinclude $(TamguFLAGS) $(Tamguincludes) $< -o $@
#------------------------------------------------------------
pytamgu: install $(OBJECTPYTHON) $(OBJECTTamgu)
	$(COMPPLUSPLUS) -shared -o $(LBINPATH)/pytamgu.so $(OBJECTPYTHON) $(SYSTEMSPATH) $(PYTHONPATH) $(PYTHONLIB) $(LIBREGEX) -L$(LBINPATH) -ltamgu

mac: install $(OBJECTPYTHON) $(OBJECTTamgu)
	$(COMPPLUSPLUS) -shared -o $(LBINPATH)/pytamgu.so $(OBJECTPYTHON) $(PYTHONPATH) $(PYTHONLIB) -L/usr/local/lib -ltamgu

install:
	mkdir -p $(LOBJPATH)/pytamgu

clean:
	rm -f $(LOBJPATH)/pytamgu/*.o

