# The directories to check are: /usr/lib64, /usr/lib or /usr/lib/x86_64-linux-gnu/

import sys
import os
from os import walk
import subprocess

############################
ostype = subprocess.Popen("uname", stdout=subprocess.PIPE).stdout.read()
ostype=ostype.strip()

############################ Linux case ###########################################################################
### In the case a library is only available with a mangled name such as libboost_regex.so.1.53,
### the installer will create a "systems" directory in which symbolic links to these libraries will be created...
###################################################################################################################

versiongcc = subprocess.Popen(["gcc","-dumpversion"], stdout=subprocess.PIPE).stdout.read()
nbversion=float(versiongcc[:3])
print("GCC Version=",nbversion)

#### We check some parameters ###

def displayhelp(s):
    if s!="":
        print('')
        print("Unknown command:", s)
    print('')
    print("Options:")
    print(" -withsound: Compile with sound support")
    print(" -noregex: Do not compile with regular expression support")
    print(" -pathregex path: Path to regex include files")
    print(" -pythonpath path: Path to Python library")
    print(" -pythoninclude path: Path to Python include files")
    print(" -pythonversion name: Python version (example: 2.7 or 3.6)")
    print(" -withgui: Compile with GUI support")
    print(" -pathfltk path: path to GUI libraries (if you have a specific version of fltk1.3 in a different directory than /usr/lib)")
    print(" -withfastint: Compile with fast int")
    print(" -intel: Compile with Intel Intrinsics instruction to speed up string conversion and string search. It impacts 'conversion.cxx' compiling...")
    print(" -avx: Compile with Intel Intrinsics instruction to speed up string conversion and string search (same as intel)")
    print(" -avx2: Compile with more advanced Intel Intrinsics instruction to speed up string conversion and string search")
    print(" -java: Prepare compiling to java")
    print(" -gccversion: Directory names for intermediate and final files depend on local gcc version")
    print(" -version name: Directory names for intermediate and final files depend on name (do not use with gccversion)")
    print(" -pathlib path: provides a system path to check for system libraries")
    print(" -static: favor static libraries over dynamic libraries when ambiguity. The default behaviour is to select dynamic libraries.")
    print(" -forcejpeg: force the compiling with jpeg")
    print(" -help: display this help")
    print('')
    exit(-1)

selectstatic = False
withsound=False
gccversion=False
noregex=False
withgui=False
withfastint=False
regexpath=""
compilejava=False
guipath=None
pythoninclude=None
pythonpath=None
pythonversion="python2.7"
versionname=None
libpath="/usr"
i=1
avx=False
avx2=False
forcejpeg = False

compilelibs="""
amaps: install
	$(MAKE) -C allmaps all

libs: install
	$(MAKE) -C allmaps all
	$(MAKE) -C pdf all
	$(MAKE) -C liblinear linear
	$(MAKE) -C libword2vec word2vec
	$(MAKE) -C libwapiti wapiti
	$(MAKE) -C libpython pytamgu
"""

cleanlibs="""
cleanlibs:
	$(MAKE) -C allmaps clean
	$(MAKE) -C pdf clean
	$(MAKE) -C libpython clean
	$(MAKE) -C liblinear clean
	$(MAKE) -C libword2vec clean
	$(MAKE) -C libwapiti clean
"""

while i < len(sys.argv):
    if sys.argv[i]=="-forcejpeg":
        forcejpeg = True
    elif sys.argv[i]=="-java":
        compilejava=True
    elif sys.argv[i]=="-withsound":
        withsound=True
    elif sys.argv[i]=="-intel" or sys.argv[i] == "-avx":
        avx=True
    elif sys.argv[i]=="-static":
        selectstatic=True
    elif sys.argv[i]=="-avx2":
        avx=True
        avx2=True
    elif sys.argv[i]=="-gccversion":
        if versionname != None:
            print("It is either gccversion or version, not both")
            exit(-1)
        gccversion=True
    elif sys.argv[i]=="-noregex":
        noregex=True
    elif sys.argv[i]=="-withgui":
        withgui=True
    elif sys.argv[i]=="-withfastint":
        withfastint=True
    elif sys.argv[i]=="-help":
        displayhelp("")
    elif sys.argv[i]=="-version":
        if gccversion == True:
            print("It is either gccversion or version, not both")
            exit(-1)
        if i >= len(sys.argv):
            print("Missing name")
            exit(-1)
        versionname=sys.argv[i+1]
        versiongcc="."+versionname
        gccversion = True
        i+=1
    elif sys.argv[i]=="-pythoninclude":
        if i >= len(sys.argv):
            print("Missing python include path on command line")
            exit(-1)
        pythoninclude=sys.argv[i+1]
        i+=1
    elif sys.argv[i]=="-pythonpath":
        if i >= len(sys.argv):
            print("Missing python path on command line")
            exit(-1)
        pythonpath=sys.argv[i+1]
        i+=1
    elif sys.argv[i]=="-pythonversion":
        if i >= len(sys.argv):
            print("Missing python version on command line")
            exit(-1)
        pythonversion="python"+sys.argv[i+1]
        i+=1
    elif sys.argv[i]=="-pathfltk":
        if i >= len(sys.argv):
            print("Missing GUI path on command line")
            exit(-1)
        guipath=sys.argv[i+1]
        i+=1
    elif sys.argv[i]=="-pathregex":
        if i >= len(sys.argv):
            print("Missing regex path on command line")
            exit(-1)
        regexpath="-I"+sys.argv[i+1]
        i+=1
    elif sys.argv[i]=="-pathlib":
        if i >= len(sys.argv):
            print("Missing library path on command line")
            exit(-1)
        libpath=sys.argv[i+1]
        i+=1
    else:
        displayhelp(sys.argv[i])
    i+=1

versiongcc=versiongcc.strip()

############################ MAC OS case...
if ostype==b"Darwin":
    compilelibs = """
libs: install
	$(MAKE) -C allmaps all
	$(MAKE) -C libpython pytamgu
	$(MAKE) -C libsqlite sqlite
	$(MAKE) -C libcurl curl
	$(MAKE) -C libxml xml
	$(MAKE) -C liblinear linear
	$(MAKE) -C libword2vec word2vec
	$(MAKE) -C libwapiti wapiti
	$(MAKE) -C pdf all
"""

    cleanlibs = """
cleanlibs:
	$(MAKE) -C allmaps clean
	$(MAKE) -C libpython clean
	$(MAKE) -C libsqlite clean
	$(MAKE) -C libcurl clean
	$(MAKE) -C libxml clean
	$(MAKE) -C liblinear clean
	$(MAKE) -C libword2vec clean
	$(MAKE) -C libwapiti clean
	$(MAKE) -C pdf clean
"""
    incpath = "INCLUDEPATH=-I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/libxml2"
    if withsound:
        compilelibs += "\t$(MAKE) -C libsound sound\n"
        cleanlibs += "\t$(MAKE) -C libsound clean\n"
        incpath += " -Iinclude/macos/ao"

    ostype = subprocess.Popen(["uname", "-a"], stdout=subprocess.PIPE).stdout.read()
    if withgui:
        compilelibs += "\t$(MAKE) -C libgui all\n"
        cleanlibs += "\t$(MAKE) -C libgui clean\n"
        if b"arm64" in ostype.lower():
            incpath += " -Iinclude/macarm/fltk"
        else:
            incpath += " -Iinclude/macos/fltk"

    incpath += "\n"
    compilelibs += "\n\n"
    cleanlibs += "\n\n"

    vname = "mac"
    if compilejava:
        vname = "mac.java"
    f=open("Makefile.in","w")
    print("MAC OS", vname)
    f.write("COMPPLUSPLUS = clang++\n")
    f.write("Optim = -Ofast\n")
    f.write("COMP = clang\n")
    f.write("BINPATH = bin/"+vname+"\n")
    f.write("OBJPATH = objs/"+vname+"\n")
    f.write("LIBOBJPATH = libobjs/"+vname+"\n")
    f.write(incpath)
    MACLIBS= "MACLIBS= -framework Cocoa -framework AudioToolbox -framework AudioUnit -framework CoreAudio\n"
    if compilejava:
        f.write("MULTIGA=-stdlib=libc++ -DMULTIGLOBALTAMGU\n");
    f.write("# MAC OS support\n")
    f.write(compilelibs)
    f.write(cleanlibs)
    f.write(MACLIBS)
    if compilejava:
        f.write("SYSTEMSPATH = -Llibs/macos -L../libs/macos -target x86_64-apple-macos11\n")
    else:
        f.write("SYSTEMSPATH = -Llibs/macos -L../libs/macos\n")
    f.write("TAMGUCONSOLENAME = tamgu\n")

    if withgui:
        if b"arm64" in ostype.lower():
            f.write("FLTKLIBS=-Llibs/macarm -lfltk -lfltk_images -lfltk_jpeg\n")
        else:
            f.write("FLTKLIBS=-Llibs/macos -lfltk -lfltk_images\n")
            f.write("JPEGLIB = -lfltk_jpeg\n\n")

    if withsound:
        f.write("SOURCEMM = macsound.mm\n")
        f.write("OBJECTLIBMM = $(SOURCEMM:%.mm=$(LOBJPATH)/sound/%.o)\n")
        f.write("LIBSOUND=-lao -lsndfile\n")
        f.write("SOUNDFILE=tamgusound.cxx tamgump3.cxx\n")
        f.write("SOUNDFLAG= -DTamguSOUND -DMACSOUND -Iinclude/macos -Iinclude/macos/ao\n")
        f.write("FLAGMPG123=-DUSEMPG123\n")
        f.write("LIBMPG123=-lmpg123\n\n")
    f.write("#Python\n")
    if pythonversion=="python2.7":
        f.write("INCLUDEPYTHON = -I/System/Library/Frameworks/Python.framework/Versions/Current/include/python2.7\n")
        f.write("PYTHONLIB = /System/Library/Frameworks/Python.framework/Versions/Current/Python\n\n")
    else:
        pversion=pythonversion[6:]
        if pythoninclude==None:
           f.write("INCLUDEPYTHON = -I/Library/Frameworks/Python.framework/Versions/"+pversion+"/Headers\n")
        else:
           f.write("INCLUDEPYTHON = "+pythoninclude+"\n")
        if pythonpath==None:
            if os.path.exists("/opt/anaconda3/lib"):
                f.write("PYTHONLIB = -L/opt/anaconda3/lib -lpython"+pversion+"\n")
            else:
                f.write("PYTHONLIB = /Library/Frameworks/Python.framework/Versions/"+pversion+"/Python\n")
        else:
           f.write("PYTHONLIB = "+pythonpath+"\n")

    # AVX instructions are not available on arm64 machines
    if b"arm64" in ostype.lower():
        if compilejava:
           f.write("C++11Flag = -std=c++14 -target x86_64-apple-macos11  -DTamgu_REGEX -DMAVERICK -DAPPLE\n")
        else:
           f.write("C++11Flag = -std=c++14 -DTamgu_REGEX -DMAVERICK -DAPPLE -DFLTK14\n")
        f.write("INTELINT =\n")
    else:
        f.write("C++11Flag = -std=c++14 -DTamgu_REGEX -DMAVERICK -DAPPLE\n")
        if compilejava:
            f.write("INTELINT =\n")
        else:
            f.write("INTELINT = -DINTELINTRINSICS -mavx2 -DAVXSUPPORT\n")
    f.close();
    print("You can launch 'make all libs' now")
    sys.exit(0)
##############################################
sourcegui = ""
if guipath != None:
    sourcegui = "GUIPATH=-L"+guipath+"\nFLTKX11LIBS = -lXext -lXft -lXinerama -lX11 -lfontconfig -lXfixes -lXcursor\n"

v=['libfltk', 'libfltk_images', 'libfltk_jpeg', 'libcurl', 'libboost_regex', 'libxml2', 'libssl',
'libsqlite3', 'libmpg123', 'libao', 'libsndfile', 'libldap','libcrypto','libldap', 'libgmp', "lib"+pythonversion,
"libXext", "libXrender", "libXft", "libXinerama", "libX11", "libfontconfig", "libXfixes", "libXcursor"]

def traverse(libpath):
    for (dirpath, dirnames, filenames) in walk(libpath):
        for s in filenames:
            if "libgcc" in s:
                return libpath
        for d in dirnames:
            s=traverse(libpath+d+"/")
            if s!=None:
                return s
    return None


#### We look for the library path that contains all our libraries
##if libpath=="/usr":#we look for libgcc
##    print("Looking into /usr/lib/x86_64-linux-gnu/")
##    libpath=traverse("/usr/lib/x86_64-linux-gnu/")
##    if libpath==None:
##        print("Looking into /usr/lib64/")
##        libpath=traverse("/usr/lib64/")
##    if libpath==None:
##        print("Looking into  /usr/lib/")
##        libpath=traverse("/usr/lib/")
##    if libpath==None:
##        sys.exit(0)

############################
### We check the system path lib directories for the requested libraries
if libpath[-1] != '/':
    libpath+='/'

if libpath == "/usr/":
    libpaths = ["/usr/lib/x86_64-linux-gnu/","/usr/lib64/","/usr/lib/", "/usr"]
else:
    libpaths = [libpath, "/usr/lib/x86_64-linux-gnu/","/usr/lib64/","/usr/lib/"]

f = []
libpath = None
for lb in libpaths:
    fd = []
    for (dirpath, dirnames, filenames) in walk(lb):
        fd.extend(filenames)
        break

    if guipath != None:
        for (dirpath, dirnames, filenames) in walk(guipath):
            fd.extend(filenames)
            break
    if fd != []:
            if len(fd) > len(f):
                f = fd
                libpath = lb

if libpath == None:
    exit(0)

print("Investigating:",libpath )
for s in f:
    if s in v:
        v.remove(s)

found={}
ret=[]
for s in v:
    for libname in f:
        if s in libname:
            if s in found:
###we favor dynamic libraries over static ones...
                checklib = ".a"
                checkfound = ".so"
                if selectstatic:
                    checklib = ".so"
                    checkfound = ".a"
                if checklib in libname and checkfound in found[s]:
                    continue
                if checkfound in libname and checklib in found[s]:
                   found[s] = libname
                elif len(libname) < len(found[s]):
                    found[s]=libname
            else:
                found[s]=libname
                ret.append(s)

for s in ret:
    while s in v:
        v.remove(s)

### If we have some mangled names, we need to create a "systems" directory
### to store symbolic links to them, which will be used to link our final application

os.system("mkdir -p systems")

if "libcurl" in found:
    compilelibs += "	$(MAKE) -C libcurl curl\n"
    cleanlibs += "	$(MAKE) -C libcurl clean\n"

if "libsqlite3" in found:
    compilelibs += "	$(MAKE) -C libsqlite sqlite\n"
    cleanlibs += "	$(MAKE) -C libsqlite clean\n"

if "libxml2" in found:
    compilelibs += "	$(MAKE) -C libxml xml\n"
    cleanlibs += "	$(MAKE) -C libxml clean\n"

objpath=None
javalibpath=None
if gccversion:
    if compilejava:
        objpath='objs/linux'+versiongcc
        print(objpath)
        javalibpath='<property name="platformArch" 	value="linux'+versiongcc+'" />'
        print(javalibpath)

    os.system("mkdir -p systems/linux"+versiongcc)

    for u in found:
        if ".a" not in found[u] and ".dylib" not in found[u]:
            ky = u
            if u == "libfltk":
                sourcegui += "\nFLTKLIBS=-lfltk1.3 -lfltk_images\n"
                ky += "1.3"
            command="rm systems/linux"+versiongcc+"/"+ky+".so"
            os.system(command)
            command="ln -s "+libpath+found[u]+" systems/linux"+versiongcc+"/"+ky+".so"
            os.system(command)
        elif ".a" in found[u] and u == "libfltk":
            sourcegui += "\nFLTKLIBS= -Wl,-Bstatic -lfltk -lfltk_images -Wl,-Bdynamic\n"
            if "FLTKX11LIBS" not in sourcegui:
                sourcegui += "\nFLTKX11LIBS = -lXext -lXft -lXinerama -lXrender -lX11 -lfontconfig -lXfixes -lXcursor\n"
else:
    for u in found:
        if ".a" not in found[u] and ".dylib" not in found[u]:
            ky = u
            if u == "libfltk":
                sourcegui += "\nFLTKLIBS=-lfltk1.3 -lfltk_images\n"
                ky += "1.3"
            command="rm systems/"+ky+".so"
            os.system(command)
            command="ln -s "+libpath+found[u]+" systems/"+ky+".so"
            os.system(command)
        elif ".a" in found[u] and u == "libfltk":
            sourcegui += "\nFLTKLIBS= -Wl,-Bstatic -lfltk -lfltk_images -Wl,-Bdynamic\n"
            if "FLTKX11LIBS" not in sourcegui:
                sourcegui += "\nFLTKX11LIBS = -lXext -lXft -lXinerama -lXrender -lX11 -lfontconfig -lXfixes -lXcursor\n"

############################
f=open("Makefile.in","w")

f.write("TAMGUCONSOLENAME = tamguconsole\n")
f.write("Optim = -O3\n")
f.write("# Uncomment the next line, if mouse does not work in the editor\n")
f.write("# MOUSESUPPORT = -DXTERM_MOUSE_VT100\n")

if avx2:
    f.write("INTELINT = -DINTELINTRINSICS -msse4.2 -mavx2 -DAVXSUPPORT\n")
elif avx:
    f.write("INTELINT = -DINTELINTRINSICS -msse4.1\n")

f.write("COMPPLUSPLUS = g++\n")
f.write("COMP = gcc\n")

if gccversion:
    f.write("BINPATH = bin/linux"+versiongcc+"\n")
    f.write("OBJPATH = objs/linux"+versiongcc+"\n")
    f.write("LIBOBJPATH = libobjs/linux"+versiongcc+"\n")
    f.write("SYSTEMSPATH = -L../systems/linux"+versiongcc+" -Lsystems/linux"+versiongcc+" -L"+libpath+"\n")
else:
    f.write("BINPATH = bin/linux\n")
    f.write("OBJPATH = objs/linux\n")
    f.write("LIBOBJPATH = libobjs/linux\n")
    f.write("SYSTEMSPATH = -L../systems -Lsystems -Llibs/linux -L"+libpath+"\n")

############################
if compilejava:
    f.write("MULTIGA=-DMULTIGLOBALTAMGU\n");
############################
specflags="SPECFLAGS =";
if not withfastint:
    specflags+=" -DNOFASTTYPE"

#We cannot use the CODECVT include to read/write UTF16 files...
if nbversion<= 5.1:
    specflags+=" -DNOCODECVT"

if specflags!="SPECFLAGS =":
    specflags+="\n"
    f.write(specflags)

############################
if len(v)!=0:
    print("--------------------------------")
    print('')
    print("Missing libraries:", v)
    if "libfltk" in v or withgui==False:
        print('')
        print("Sorry tamgu will have no GUI functionalities. Install FLTK package for GUI features...")
        withgui=False

if withgui:
    f.write(sourcegui)
    compilelibs += "	$(MAKE) -C libgui all\n"
    cleanlibs += "	$(MAKE) -C libgui clean\n"
############################
if pythoninclude!=None:
    f.write("\n\n#Python support to compile tamgu python library: 'pytamgu'\n")
    f.write("INCLUDEPYTHON = -I"+pythoninclude+"\n")
    f.write("PYTHONLIB = -l"+pythonversion+"\n")
    print('')
    print("You can compile the pytamgu library (tamgu python library)")
    print('')
elif pythonversion not in v:
    f.write("\n\n#Python support to compile tamgu python library: 'pytamgu'\n")
    for (dirpath, dirnames, filenames) in walk("/usr/include"):
        if pythonversion in dirnames:
            f.write("INCLUDEPYTHON = -I/usr/include/"+pythonversion+"\n")
            f.write("PYTHONLIB = -l"+pythonversion+"\n")
            print('')
            print("You can compile the pytamgu library (tamgu python library)")
            print('')
        else:
            f.write("INCLUDEPYTHON = \n")
            f.write("PYTHONLIB = -l"+pythonversion+"\n")
            print('')
            print("Could not find python 2.7 include and library paths. ")
            print("Modify 'INCLUDEPYTHON' and 'PYTHONLIB' in Makefile.in if you want to compile pytamgu (the tamgu python library)")
            print('')
        break
if pythonpath != None:
    f.write("PYTHONPATH = -L"+pythonpath+"\n")
############################
includepath="INCLUDEPATH = -Iinclude/linux"
############################
jpegflag="#JPEGFLAG = -DFLTKNOJPEG"
jpeglib="#JPEGLIB = -lfltk_jpeg"

if "libfltk_jpeg" not in v or forcejpeg:
    jpeglib=jpeglib.replace("#","")
else:
    jpegflag=jpegflag.replace("#","")
    print("No fltk_jpeg library available")

f.write("\n\n# JPEG support\n")
f.write(jpegflag+"\n")
f.write(jpeglib+"\n")
f.write("\n")


############################

flagmpg123="""
#FLAGMPG123=-DUSEMPG123
#LIBMPG123=-lmpg123
"""

if withsound==True:
    if "libmpg123" not in v:
        flagmpg123=flagmpg123.replace("#","")
    else:
        print("MPG123 will not be available in tamgu")
    f.write("# MPG3 support")
    f.write(flagmpg123)
    f.write("\n")

############################
soundao="""
#LIBAO=-lao
#LIBSOUNDFILE=-lsndfile
"""
soundflag="""
#LIBSOUND=$(LIBAO) $(LIBSOUNDFILE)
#SOUNDFILE=tamgusound.cxx
#SOUNDFLAG= -DTamguSOUND
"""

if withsound==True:
    sndbool=False
    compilelibs += "	$(MAKE) -C libsound sound\n"
    cleanlibs += "	$(MAKE) -C libsound clean\n"
    if "libao" not in v:
        soundao=soundao.replace("#LIBAO","LIBAO")
        soundflag=soundflag.replace("#","")
        includepath+=" -Iinclude/linux/ao"
        sndbool=True

    if "libsndfile" not in v:
        soundao=soundao.replace("#LIBSOUNDFILE","LIBSOUNDFILE")
        soundflag=soundflag.replace("#","")
        includepath+=" -Iinclude/linux/ao"

    if not sndbool:
        print("No sound available")
    else:
        f.write("# SOUND (ao and sndfile) support")
        f.write(soundao)
        f.write(soundflag)
        f.write("\n")


############################
sslflag="""
#SSLLIB= -lssl
"""

if "libssl" not in v:
    sslflag=sslflag.replace("#","")

f.write("# SSL support")
f.write(sslflag)
f.write("\n")

############################
cryptoflag="""
#CRYPTOLIB= -lcrypto
"""

if "libcrypto" not in v:
    cryptoflag=cryptoflag.replace("#","")

f.write("# CRYPTO support")
f.write(cryptoflag)
f.write("\n")

############################
gmpflag="""
#GMPLIB= -lgmp
"""

if "libgmp" not in v:
    gmpflag=gmpflag.replace("#","")

f.write("# GMP support")
f.write(gmpflag)
f.write("\n")

############################
f.write(includepath+"\n")
############################
# We test now if the compiler onboard is compatible with C++11 norm
#C++11Flag = -std=c++0x
#C++11Flag = -std=gnu++0x
#C++11Flag = -std=c++11

pathtest= os.getcwd()
if pathtest[-1]!='/':
    pathtest+='/src/testcompile.cxx'
else:
    pathtest+='src/testcompile.cxx'

command=["g++","-std=c++11",pathtest]
ostype = subprocess.call(command)
if ostype:
    command[1]="-std=c++0x"
    ostype = subprocess.call(command)
    if ostype:
        command[1]="-std=gnu++0x"
        ostype = subprocess.call(command)
        if ostype:
            print('')
            print('')
            print("Error: You need a compiler compatible with C++11")
            print('')
            sys.exit(-1)

f.write("\n# Compiler C++11 option\n")
f.write("C++11Flag = "+command[1]+"\n")


############################
regexflag="""
#REGEXFLAG = %%%
#REGEX=-DTamgu_REGEX $(REGEXFLAG)

"""

regexflag+="#LIBREGEX= -lboost_regex\n"


#Properties...
p1='<!--property name="regexcpp"     value="true" /-->'
p2='<!--property name="regexboost"     value="true" /-->'
p3='<!--property name="nofastype"     value="true" /-->'
p4='<!--property name="nocodecvt"     value="true" /-->'

rep1='<property name="regexcpp"     value="true" />'
rep2='<property name="regexboost"     value="true" />'
rep3='<property name="nofastype"     value="true" />'
rep4='<property name="nocodecvt"     value="true" />'



brep1=False
brep2=False

if "libboost_regex" not in v and noregex==False:
    #we check now how to include regex... either #include "boost/regex.hpp" or <regex>
    pathtest= os.getcwd()
    if pathtest[-1]!='/':
        pathtest+='/src/testregex.cxx'
    else:
        pathtest+='src/testregex.cxx'

    systempath="-Lsystems"
    if gccversion:
        systempath+="/linux"+versiongcc

    cmd=["g++","-o","testregex",command[1],"-DREGEXCPP",pathtest,systempath,"-lboost_regex"]
    print('')
    print("------------------------------------------")
    print("Warning, error messages might show as the system is checking, which version of regex is implemented on your machine.")
    print("They are normal and will not prevent any actual compiling")
    print('')
    ostype = subprocess.call(cmd)
    if ostype:
        if regexpath!="":
            cmd=["g++","-o","testregex",command[1],regexpath,pathtest,systempath,"-lboost_regex"]
        else:
            cmd=["g++","-o", "testregex",command[1],pathtest,systempath,"-lboost_regex"]
        ostype = subprocess.call(cmd)
        if ostype:
            noregex=True
        else:
            print('')
            print("Using boost::regex")
            if compilejava:
                brep2=True
            if regexpath!="":
                regexflag=regexflag.replace("%%%",regexpath)
    else:
        print('')
        print("Using std::regex")
        regexflag=regexflag.replace("%%%","-DREGEXCPP")
        regexflag=regexflag.replace("-lboost_regex","")
        if compilejava:
            brep1=True
            brep2=True

if compilejava:
    fb=open('java/build.base')
    txt=fb.read()
    fb.close()
    if brep1:
        txt=txt.replace(p1,rep1)
    if brep2:
        txt=txt.replace(p2,rep2)
    if brep2:
        txt=txt.replace(p2,rep2)
    if not withfastint:
        txt=txt.replace(p3,rep3)
    if nbversion<= 5.1:
        txt=txt.replace(p4,rep4)

    if objpath != None:
        txt=txt.replace("objs/linux",objpath)
    if javalibpath != None:
        txt=txt.replace('<property name="platformArch" 	value="linux" />',javalibpath)
    bsv=open('java/build.xml',"w")
    bsv.write(txt)
    bsv.close()

if noregex==False:
    os.system("rm testregex")
    regexflag=regexflag.replace("%%%","")
    regexflag=regexflag.replace("#","")
else:
    print("Regex will not be available in tamgu")
    noregex=False

f.write("# boost regex support")
f.write(regexflag)
f.write("\n")

f.write(compilelibs)
f.write("\n")

f.write(cleanlibs)
f.write("\n")

f.close()

print('')
print('')
print("All is ok... You can compile now")
print(" 'make all'  to compile main lib and executable")
print(" 'make libs' to compile the libs")
print('')

