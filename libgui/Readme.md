# FLTK
The following libraries have been downloaded from: fltk.org (version 1.3.5) and have been compiled with Visual 2013

## Windows: the directory _libfltk/libs/windows_ should contain:

fltk.lib
fltkimages.lib
fltkjpeg.lib
fltkpng.lib
fltkzlib.lib

### Includes

The _FL include directory_ should be copied in the libfltk/include/windows/FL

### Important

We have modified the following files:

File: __Fl_win32.cxx__ (after line _1113_)

```C++
//Code added to Fl_win32.cxx after void delete_timer, line 1113
void FlResetTimer() {
    if (s_TimerWnd)
        s_TimerWnd = 0;
}
```

File: __Fl_lock.cxx__ (after line: _211_)

```C++
//We launch FLTK in a thread, but in that case the mainthread variable (see Fl_lock.cxx) is no longer set to the proper value
//This function is used to do that...

Code added to Fl_lock.cxx after implementation of void lock_ring(), line 211
void Flresetmainthread() {
    main_thread = GetCurrentThreadId();
}

void Fltkclearmainthread() {
    main_thread = 0;
}
```

## Mac OS

We have compiled the following static libs on Mac OS:
The following libs have been stored in: libfltk/libs/macos
```
libfltk.a
libfltk_forms.a
libfltk_gl.a
libfltk_images.a
libfltk_jpeg.a
libfltk_png.a
```
### Includes

There are different files to copy in: _libfltk/include/macos/fltk_
```
-config.h
-config.in
-forms.h
-mac_endianness.h
+FL
    include files from FLTK
+src 
    -FL_Font.H
```

Furthermore the directory _libsfltk/include/macos/src_ should contain: _Fl_Font.H_

## Linux

In most cases,  fltk is already installed on your machine. However, you still need the include files to compile. You can either install a development version of FLTK for apt-get or yum. Or you can download a version from _fltk.org_ and copy the FL directory in: _libfltk/include/linux/FL_


# Compiling Linux and Mac OS
First, you need to launch _install.py_ with the option _-withfltk_:

python install.py -withfltk

Once, you have installed the necessary files, you need to execute: _make all_ in the folder _libfltk_.

__Note__ that the Makefile will also create an new version of _tamgu_: _tamguconsole_, which gives you access to GUI for _tamgu_.

