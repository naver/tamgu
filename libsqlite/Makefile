###################################################################
include ../Makefile.in
################ To compile Tamgu#################################
SOURCESQLITE = tamgusqlite.cxx
#------------------------------------------------------------
LBINPATH = ../$(BINPATH)
LOBJPATH = ../$(LIBOBJPATH)
###################################################################
OBJECTSQLITE = $(SOURCESQLITE:%.cxx=$(LOBJPATH)/sqlite/%.o)
#------------------------------------------------------------
TamguBASICFLAGS = -w -c -fPIC -O3 -DUNIX $(MULTIGA) $(INTELINT) $(REGEX) $(SPECFLAGS) -DSQLITE_ENABLE_COLUMN_METADATA -DSQLITE_THREADSAFE
TamguFLAGS = $(C++11Flag) $(TamguBASICFLAGS)
Tamguincludes= -Iinclude -I../include
#------------------------------------------------------------
$(LOBJPATH)/sqlite/%.o: src/%.cxx
	$(COMP) -Iinclude $(TamguFLAGS) $(Tamguincludes) $< -o $@
#------------------------------------------------------------
sqlite: install $(OBJECTSQLITE) $(OBJECTTamgu)
	$(COMPPLUSPLUS) -shared -o $(LBINPATH)/libsqlite.so $(OBJECTSQLITE) $(SYSTEMSPATH) -lsqlite3  $(LIBREGEX) -L$(LBINPATH) -ltamgu

install:
	mkdir -p $(LOBJPATH)/sqlite

clean:
	rm -f $(LOBJPATH)/sqlite/*.o

