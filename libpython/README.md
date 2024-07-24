# Updating the Python library to your local machine on Mac OS

In `libpython` directory, launch: `python findpython.py`.

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

You can then compile your new version: `make mac`.

## Update your environment
Once the version is compiled, launch: `sh update.sh` to update the new library in your environment.

## DYLD_LIBRARY_PATH
The `python library` should be accessible through the `DYLD_LIBRARY_PATH` variable.
In your `.zshrc`, please check: `export DYLD_LIBRARY_PATH = your_python_path$DYLD_LIBRARY_PATH`.
