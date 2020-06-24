#!/usr/bin/sh
echo ------------------------------------------------------------------
echo
echo Compiling tamgu
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

python install.py -withfastint -avx $1 $2 $3 $4 $5 $6 $7 $8 $9

