###################################################################
include ../Makefile.in
#------------------------------------------------------------
sourcebinary = tamgubinary.cxx
#------------------------------------------------------------
LBINPATH = ../$(BINPATH)
LOBJPATH = ../$(LIBOBJPATH)
#------------------------------------------------------------
objectbinary = $(sourcebinary:%.cxx=$(LOBJPATH)/binary/%.o)
#------------------------------------------------------------
TamguBASICFLAGS = -w -c -fPIC -O3 -DUNIX $(MULTIGA) $(INTELINT) $(REGEX) $(SPECFLAGS)
TamguCFLAGS = -std=c99 $(TamguBASICFLAGS)
TamguFLAGS = $(C++11Flag) $(TamguBASICFLAGS)
TAMGULIBPATH = ../$(BINPATH)
#------------------------------------------------------------
Tamguincludes= -I../include $(INCLUDEPATH)
#------------------------------------------------------------
$(LOBJPATH)/binary/%.o: src/%.cxx
	$(COMP) -Iinclude $(TamguFLAGS) $(Tamguincludes) $< -o $@
#------------------------------------------------------------
all: install $(objectbinary)
	$(COMPPLUSPLUS) -o $(LBINPATH)/binary $(objectbinary) $(SYSTEMSPATH) $(LIBREGEX) -L$(LBINPATH) -ltamgu

install:
	mkdir -p $(LOBJPATH)/binary

clean:
	rm -f $(LOBJPATH)/binary/*.o

