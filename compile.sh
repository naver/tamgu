#!/usr/bin/sh
echo ------------------------------------------------------------------
echo
echo To compile the Python library: provide the python version as: "3.7m"
echo
echo ------------------------------------------------------------------

if [ "-h" = "$1" ]; then
   python install.py -help
   exit
fi

if [ "--help" = "$1" ]; then
   python install.py -help
   exit
fi

if [ "" = "$1" ]; then
   python install.py -withfastint -avx
else
   python install.py -withfastint -avx -pythoninclude /usr/include/python$1 -pythonversion $1 $2 $3 $4 $5 $6 $7 $8 $9
fi

