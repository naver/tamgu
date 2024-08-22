# Updating the Python library to your local machine

In `libpython` directory, launch: `python findpython.py`.

**Note** that by default, the script works on a Mac OS and suppose the existence of an `opt/anaconda3` directory.

You can also provide your own path to investigate on the command line such as: `python findpython.py mypath`.

This program will then generate a: `Makefile.py`, which contains the necessary Python paths.

## Anaconda3 installed

If you have `anaconda3` installed on your machine, you might have different version of Python. Uncomment the one that you want to execute and comment out the other lines.
For instance, `findpython.py`might have generated the following lines in `Makefile.py`:

```Makefile
FORLINKED = -undefined dynamic_lookup
#LOCALINCLUDEPYTHON = -I$(HOME)/opt/anaconda3/include/python3.9
#LOCALPYTHONLIB = $(FORLINKED) -L$(HOME)/opt/anaconda3/lib/python3.9 -lpython3.9
LOCALINCLUDEPYTHON = -I$(HOME)/opt/anaconda3/envs/chaine/include/python3.11
LOCALPYTHONLIB = $(FORLINKED) -L$(HOME)/opt/anaconda3/envs/chaine/lib/ -lpython3.11
# LOCALINCLUDEPYTHON = -I$(HOME)/opt/anaconda3/envs/llame/include/python3.11
# LOCALPYTHONLIB = $(FORLINKED) -L$(HOME)/opt/anaconda3/envs/llame/lib/ -lpython3.11
```

In this case, we have chosen the version in `chaine`. We need the two lines: `LOCALINCLUDEPYTHON` and `LOCALPYTHONLIB`.

You can then compile your new version: `make local`.

## Update your environment
Once the version is compiled, launch: `sh update.sh` to update the new library in your environment.

## DYLD_LIBRARY_PATH on Mac OS
The `python library` should be accessible through the `DYLD_LIBRARY_PATH` variable.

In your `.zshrc`, please check: `export DYLD_LIBRARY_PATH = python_library_path$DYLD_LIBRARY_PATH`.

_python_library_path should match LOCALPYTHONLIB_

## LD_LIBRARY_PATH on other Linux OS

The `python library` should be accessible through the `LD_LIBRARY_PATH` variable. 
In your `.bashrc` please check: `export LD_LIBRARY_PATH = python_library_path$LD_LIBRARY_PATH`.

