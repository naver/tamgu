###################################################################
include ../Makefile.in
#------------------------------------------------------------
sourcepdf = tamgupdf.cxx pdf.cxx metrics.cxx
#------------------------------------------------------------
LBINPATH = ../$(BINPATH)
LOBJPATH = ../$(LIBOBJPATH)
#------------------------------------------------------------
objectpdf = $(sourcepdf:%.cxx=$(LOBJPATH)/pdf/%.o)
#------------------------------------------------------------
TamguBASICFLAGS = -w -c -fPIC -O3 -DUNIX $(MULTIGA) $(INTELINT) $(REGEX) $(SPECFLAGS)
TamguCFLAGS = -std=c99 $(TamguBASICFLAGS)
TamguFLAGS = $(C++11Flag) $(TamguBASICFLAGS)
TAMGULIBPATH = ../$(BINPATH)
#------------------------------------------------------------
Tamguincludes= -I../include $(INCLUDEPATH)
#------------------------------------------------------------
$(LOBJPATH)/pdf/%.o: src/%.cxx
	$(COMP) -Iinclude $(TamguFLAGS) $(Tamguincludes) $< -o $@
#------------------------------------------------------------
all: install $(objectpdf)
	$(COMPPLUSPLUS) -shared -o $(LBINPATH)/libtamgupdf.so $(objectpdf) $(SYSTEMSPATH) $(LIBREGEX) -L$(LBINPATH) -ltamgu

install:
	mkdir -p $(LOBJPATH)/pdf

clean:
	rm -f $(LOBJPATH)/pdf/*.o

