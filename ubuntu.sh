#!/usr/bin/sh
echo ------------------------------------------------------------------
echo
echo Compiling tamgu: -help for help
echo
echo To compile the Python library on Linux: -pythoninclude /usr/include/python3.7m -pythonversion 3.7m
echo
echo ------------------------------------------------------------------

if [ "-h" = "$1" ]; then
   python install.py -help
   exit
fi

if [ "-help" = "$1" ]; then
   python install.py -help
   exit
fi

if [ "--help" = "$1" ]; then
   python install.py -help
   exit
fi

if [ "clean" = "$1" ]; then
	make clean cleanlibs
fi

python install.py -withgui -withsound -withfastint -version ubuntu -avx2 -pythoninclude $2 -pythonversion $3

if [ "libs" = "$1" ]; then
	make libs -j16
else
	make all libs -j16
fi

