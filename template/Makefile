###################################################################
include ../Makefile.in
#------------------------------------------------------------
source%1 = tamgu%1.cxx
#------------------------------------------------------------
LBINPATH = ../$(BINPATH)
LOBJPATH = ../$(LIBOBJPATH)
#------------------------------------------------------------
object%1 = $(source%1:%.cxx=$(LOBJPATH)/%1/%.o)
#------------------------------------------------------------
TamguBASICFLAGS = -w -c -fPIC $(Optim) -DUNIX $(MULTIGA) $(INTELINT) $(REGEX) $(SPECFLAGS)
TamguCFLAGS = -std=c99 $(TamguBASICFLAGS)
TamguFLAGS = $(C++11Flag) $(TamguBASICFLAGS)
TAMGULIBPATH = ../$(BINPATH)
#------------------------------------------------------------
Tamguincludes= -I../include $(INCLUDEPATH)
#------------------------------------------------------------
$(LOBJPATH)/%1/%.o: src/%.cxx
	$(COMP) -Iinclude $(TamguFLAGS) $(Tamguincludes) $< -o $@
#------------------------------------------------------------
all: install $(object%1)
	$(COMPPLUSPLUS) -shared -o $(LBINPATH)/lib%1.so $(object%1) $(SYSTEMSPATH) $(LIBREGEX) -L$(LBINPATH) -ltamgu

install:
	mkdir -p $(LOBJPATH)/%1

clean:
	rm -f $(LOBJPATH)/%1/*.o

