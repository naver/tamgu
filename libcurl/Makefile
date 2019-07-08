include ../Makefile.in
################ To compile Tamgu#################################
SOURCECURL = tamgucurl.cxx
#------------------------------------------------------------
LBINPATH = ../$(BINPATH)
LOBJPATH = ../$(LIBOBJPATH)
###################################################################
OBJECTCURL = $(SOURCECURL:%.cxx=$(LOBJPATH)/curl/%.o)
#------------------------------------------------------------
TamguBASICFLAGS = -w -c -fPIC -O3 -DUNIX $(MULTIGA) $(INTELINT) $(REGEX) $(SPECFLAGS) -DCURL_STATICLIB
TamguFLAGS = $(C++11Flag) $(TamguBASICFLAGS)
Tamguincludes= -Iinclude -I../include $(INCLUDEPATH)
TamguSYSTEMLIBS = $(SYSTEMSPATH) -lcurl
#------------------------------------------------------------
$(LOBJPATH)/curl/%.o: src/%.cxx
	$(COMP) -Iinclude $(TamguFLAGS) $(Tamguincludes) $< -o $@
#------------------------------------------------------------
curl: install $(OBJECTCURL) $(OBJECTTamgu)
	$(COMPPLUSPLUS) -shared -o $(LBINPATH)/libtamgucurl.so $(OBJECTCURL) $(TamguSYSTEMLIBS) $(LIBREGEX) -L$(LBINPATH) -ltamgu $(CRYPTOLIB) $(SSLLIB)

install:
	mkdir -p $(LOBJPATH)/curl

clean:
	rm -f $(LOBJPATH)/curl/*.o
	rm -f $(LBINPATH)/libtamgucurl.so

