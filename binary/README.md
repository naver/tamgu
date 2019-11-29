# BINARY

This program shows how to build a binary that compile and execute some tamgu code.

# First

For Mac OS and Linux releases (see https://github.com/naver/tamgu/releases), the library *libtamgu.so* is already provided. You need to get it from the right package for the Release section, if you do not want to compile it yourself.

Now, you still need to launch the *install.py* program to extract the specifics of your platform. A *Makefile.in* is then produced, which is needed to compile your binary.

This operation will be required only once for a given platform.

* On Mac do: **python install.py**
* On Linux do: **python install.py -withfastint -avx** (fedora, ubunto or centos)

## If you have downloaded the library from the release section: libtamgu.so

* On Mac create a directory: *bin/mac* at the same level as *install.py*. Put your library there.
* On Linux create a directory: *bin/linux* at the same level as *install.py*. Put your library there.

## If you compile your own version

In that case, all of the above will be automatically created when compiling your own version.

Of course, do not forget to launch the adequate: *python install.py* to suit your platform needs

# Second

Now, you can compile the version in this directory: **make all**

The result of your compilation will be stored in:

* On Mac: **bin/mac**
* On Linux: **bin/linux**
