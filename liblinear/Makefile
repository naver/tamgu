###################################################################
include ../Makefile.in
################ To compile Tamgu##################################
LBINPATH = ../$(BINPATH)
LOBJPATH = ../$(LIBOBJPATH)
###################################################################
SOURCELINEARC = daxpy.c  ddot.c  dnrm2.c  dscal.c
SOURCELINEARCXX = liblinear.cxx
SOURCELINEARCPP = linear.cpp  tron.cpp
#------------------------------------------------------------
OBJECTLINEARC = $(SOURCELINEARC:%.c=$(LOBJPATH)/linear/%.o)
OBJECTLINEARCXX = $(SOURCELINEARCXX:%.cxx=$(LOBJPATH)/linear/%.o)
OBJECTLINEARCPP = $(SOURCELINEARCPP:%.cpp=$(LOBJPATH)/linear/%.o)
#------------------------------------------------------------
TamguBASICFLAGS = -w -c -fPIC -O3 -DUNIX $(MULTIGA) $(INTELINT) $(REGEX) $(SPECFLAGS)
TamguCFLAGS = -std=c99 $(TamguBASICFLAGS)
TamguFLAGS = $(C++11Flag) $(TamguBASICFLAGS)
Tamguincludes= -Iinclude -I../include
#------------------------------------------------------------
$(LOBJPATH)/linear/%.o: src/%.c
	$(COMP) -Iinclude $(TamguCFLAGS) $(Tamguincludes) $< -o $@

$(LOBJPATH)/linear/%.o: src/%.cxx
	$(COMP) -Iinclude $(TamguFLAGS) $(Tamguincludes) $< -o $@

$(LOBJPATH)/linear/%.o: src/%.cpp
	$(COMP) -Iinclude $(TamguFLAGS) $(Tamguincludes) $< -o $@
#------------------------------------------------------------
linear: install $(OBJECTLINEARC) $(OBJECTLINEARCXX) $(OBJECTLINEARCPP)
	$(COMPPLUSPLUS) -shared -o $(LBINPATH)/liblinear.so $(OBJECTLINEARC) $(OBJECTLINEARCPP) $(OBJECTLINEARCXX) -L$(LBINPATH) -ltamgu

install:
	mkdir -p $(LOBJPATH)/linear

clean:
	rm -f $(LOBJPATH)/linear/*.o
	rm -f $(LBINPATH)/liblinear.so


