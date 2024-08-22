import sys
import os
from os import walk
import subprocess


def traverse(libpath, v):
    for (dirpath, dirnames, filenames) in walk(libpath):
        if 'include/python3.' in dirpath:
            v.append(dirpath)
            return True
        for d in dirnames:
            if traverse(libpath+d+"/", v):
                return True
    return False

# You can provide your own path on the command line
# By default we will investigate the /opt/anaconda3 directory on Mac OS
if len(sys.argv) == 1:
    home = os.getenv("HOME")
    path = home+"/opt/anaconda3/"
    v = []
    traverse(path, v)
    path = home+"/opt/anaconda3/envs/"
else:
    path = sys.argv[1]

ls = os.listdir(path)
for e in ls:
    traverse(path+e, v)

vars = """
#If anaconda is installed on your machine, then you can compile with the following lines
LOCALINCLUDEPYTHON = -I$(HOME)/opt/anaconda3/include/python
FORLINKED = -undefined dynamic_lookup
LOCALPYTHONLIB = $(FORLINKED) -L$(HOME)/opt/anaconda3/lib -lpython

#If ANACONDA is not installed on your machine, uncomment the following lines
#Don't forget to comment the above lines (Only for MAC OS)
#LOCALINCLUDEPYTHON = -I/Library/Frameworks/Python.framework/Headers
#LOCALPYTHONLIB = /Library/Frameworks/Python.framework/Python
"""

with open("Makefile.py", "w") as f:
    if v == []:
        f.write("# Basic link to Python")
        f.write("LOCALINCLUDEPYTHON = -I/Library/Frameworks/Python.framework/Headers\n")
        f.write("LOCALPYTHONLIB = /Library/Frameworks/Python.framework/Python\n")
    else:
        f.write("# Anaconda3 link to Python\n")
        f.write("FORLINKED = -undefined dynamic_lookup\n")
        base = v[0].strip().split("/")
        base = [x for x in base if x]
        f.write("LOCALINCLUDEPYTHON = -I$(HOME)/opt/anaconda3/include/"+base[-1]+"\n")
        f.write("LOCALPYTHONLIB = $(FORLINKED) -L$(HOME)/opt/anaconda3/lib/"+base[-1]+" -l"+base[-1]+"\n")
        for e in v[1:]:
            base = e.strip().split("/")
            base = [x for x in base if x]
            f.write("# LOCALINCLUDEPYTHON = -I$(HOME)/opt/anaconda3/envs/"+base[-3]+"/"+base[-2]+"/"+base[-1]+"\n")
            f.write("# LOCALPYTHONLIB = $(FORLINKED) -L$(HOME)/opt/anaconda3/envs/"+base[-3]+"/lib/" + " -l"+base[-1]+"\n")
