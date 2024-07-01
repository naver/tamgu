###################################################################
include ../Makefile.in
################ To compile Tamgu#################################
#------------------------------------------------------------
LBINPATH = ../$(BINPATH)
LOBJPATH = ../$(LIBOBJPATH)
LSRCPATH = ../src
#------------------------------------------------------------
SOURCEGUI= tamgubitmap.cxx tamgubox.cxx tamgubrowser.cxx tamgubutton.cxx tamguchoice.cxx tamguconsole.cxx \
tamgucounter.cxx tamgueditor.cxx tamgufilebrowser.cxx tamgugui.cxx tamgugroup.cxx tamguimage.cxx \
tamguprogress.cxx tamguslider.cxx tamgutabs.cxx tamguwidget.cxx tamguwindow.cxx tamguwinput.cxx tamguwoutput.cxx \
tamguwtable.cxx
#------------------------------------------------------------
SOURCEMAIN = maintamgu.cxx jag.cxx
##################################################################
OBJECTGUICXX = $(SOURCEGUI:%.cxx=$(LOBJPATH)/fltk/%.o)
OBJECTGUIMAINCXX = $(SOURCEMAIN:%.cxx=$(LOBJPATH)/fltkmain/%.o)
#------------------------------------------------------------
TamguBASICFLAGS = -w -c -fPIC $(Optim) -DUNIX $(MULTIGA) $(INTELINT) $(REGEX) $(SPECFLAGS)
TamguFLAGS = $(C++11Flag) $(TamguBASICFLAGS) $(FLTKFLAG) $(JPEGFLAG) -DFL_INTERNALS
Tamguincludes= -Iinclude -I../include $(INCLUDEPATH)
TamguSYSTEMLIBS = $(SYSTEMSPATH) -lpthread $(FLTKX11LIBS) $(JPEGLIB) $(GMPLIBS) $(MACLIBS) -ldl
#------------------------------------------------------------
$(LOBJPATH)/fltk/%.o: src/%.cxx
	$(COMP) -Iinclude $(TamguFLAGS) -DGUIPARSE $(Tamguincludes) $< -o $@

$(LOBJPATH)/fltkmain/%.o: $(LSRCPATH)/%.cxx
	$(COMP) -Iinclude $(TamguFLAGS) -DGUIPARSE -DWITHCONSOLE $(Tamguincludes) $< -o $@
#------------------------------------------------------------
fltk: install $(OBJECTGUICXX)
	$(COMPPLUSPLUS) -shared -o $(LBINPATH)/libgui.so $(OBJECTGUIC) $(OBJECTGUICPP) $(OBJECTGUICXX) -L$(LBINPATH) -l tamgu $(FLTKLIBS) $(TamguSYSTEMLIBS) $(LIBREGEX)

#we add a new option to tamgu: the console
tamgu: $(OBJECTGUIMAINCXX)
	$(COMPPLUSPLUS) -o $(LBINPATH)/$(TAMGUCONSOLENAME) $(OBJECTGUIMAINCXX) $(OBJECTGUICXX) ../$(OBJPATH)/*.o $(FLTKLIBS) $(TamguSYSTEMLIBS) $(LIBREGEX)

all: fltk tamgu
	$(fltk)
	$(tamgu)

install:
	mkdir -p $(LOBJPATH)/fltk
	mkdir -p $(LOBJPATH)/fltkmain

clean:
	rm -f $(LBINPATH)/libgui.so
	rm -f $(LOBJPATH)/fltk/*.o
	rm -f $(LOBJPATH)/fltkmain/*.o

