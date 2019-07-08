###################################################################
include ../Makefile.in
################ To compile Tamgu#################################
SOURCEWAPITIC = bcd.c decoder.c gradient.c lbfgs.c model.c options.c pattern.c progress.c quark.c reader.c rprop.c sgdl1.c thread.c tools.c vmath.c wapiti.c
SOURCEWAPITICPP = puretagger.cpp
SOURCEWAPITICXX = tamguwapiti.cxx
#------------------------------------------------------------
LBINPATH = ../$(BINPATH)
LOBJPATH = ../$(LIBOBJPATH)
###################################################################
OBJECTWAPITIC = $(SOURCEWAPITIC:%.c=$(LOBJPATH)/wapiti/%.o)
OBJECTWAPITICXX = $(SOURCEWAPITICXX:%.cxx=$(LOBJPATH)/wapiti/%.o)
OBJECTWAPITICPP = $(SOURCEWAPITICPP:%.cpp=$(LOBJPATH)/wapiti/%.o)
#------------------------------------------------------------
TamguBASICFLAGS = -w -c -fPIC -O3 -DUNIX $(MULTIGA) $(INTELINT) $(REGEX) $(SPECFLAGS)
TamguCFLAGS = -std=c99 $(TamguBASICFLAGS)
TamguFLAGS = $(C++11Flag) $(TamguBASICFLAGS)
Tamguincludes= -Iinclude -I../include
#------------------------------------------------------------
$(LOBJPATH)/wapiti/%.o: src/%.c
	$(COMP) -Iinclude $(TamguCFLAGS) -DWAPITIPARSE$(Tamguincludes) $< -o $@

$(LOBJPATH)/wapiti/%.o: src/%.cxx
	$(COMP) -Iinclude $(TamguFLAGS) -DWAPITIPARSE $(Tamguincludes) $< -o $@

$(LOBJPATH)/wapiti/%.o: src/%.cpp
	$(COMP) -Iinclude $(TamguFLAGS) -DWAPITIPARSE $(Tamguincludes) $< -o $@
#------------------------------------------------------------
wapiti: install $(OBJECTWAPITIC) $(OBJECTWAPITICXX) $(OBJECTWAPITICPP)
	$(COMPPLUSPLUS) -shared -o $(LBINPATH)/libwapiti.so $(OBJECTWAPITIC) $(OBJECTWAPITICPP) $(OBJECTWAPITICXX) -L$(LBINPATH) -ltamgu

install:
	mkdir -p $(LOBJPATH)/wapiti

clean:
	rm -f $(LOBJPATH)/wapiti/*.o


