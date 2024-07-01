/*
 * Copyright 2002-2004 Red Hat Inc., Durham, North Carolina.
 *
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation on the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT.  IN NO EVENT SHALL RED HAT AND/OR THEIR SUPPLIERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * Authors:
 *   Rickard E. (Rik) Faith <faith@redhat.com>
 *
 */

/** \file
 * This file describes the structures necessary to implement the wire
 * protocol for the DMX protocol extension.  It should be included only
 * in files that implement the client-side (or server-side) part of the
 * protocol (i.e., client-side applications should \b not include this
 * file). */

#ifndef _DMXSTR_H_
#define _DMXSTR_H_

#define DMX_EXTENSION_NAME  "DMX"
#define DMX_EXTENSION_MAJOR 2
#define DMX_EXTENSION_MINOR 2
#define DMX_EXTENSION_PATCH 20040604

/* These values must be larger than LastExtensionError.
   The values in dmxext.h and dmxproto.h *MUST* match. */
#define DMX_BAD_XINERAMA     1001
#define DMX_BAD_VALUE        1002

#define X_DMXQueryVersion                   0
#define X_DMXGetScreenCount                 1
#define X_DMXGetScreenInformationDEPRECATED 2
#define X_DMXGetWindowAttributes            3
#define X_DMXGetInputCount                  4
#define X_DMXGetInputAttributes             5
#define X_DMXForceWindowCreationDEPRECATED  6
#define X_DMXReconfigureScreenDEPRECATED    7
#define X_DMXSync                           8
#define X_DMXForceWindowCreation            9
#define X_DMXGetScreenAttributes           10
#define X_DMXChangeScreensAttributes       11
#define X_DMXAddScreen                     12
#define X_DMXRemoveScreen                  13
#define X_DMXGetDesktopAttributes          14
#define X_DMXChangeDesktopAttributes       15
#define X_DMXAddInput                      16
#define X_DMXRemoveInput                   17

/** Wire-level description of DMXQueryVersion protocol request. */
typedef struct {
    CARD8   reqType;            /* dmxcode */
    CARD8   dmxReqType;         /* X_DMXQueryVersion */
    CARD16  length B16;
} xDMXQueryVersionReq;
#define sz_xDMXQueryVersionReq 4

/** Wire-level description of DMXQueryVersion protocol reply. */
typedef struct {
    BYTE    type;               /* X_Reply */
    CARD8   ununsed;
    CARD16  sequenceNumber B16;
    CARD32  length B32;
    CARD32  majorVersion B32;
    CARD32  minorVersion B32;
    CARD32  patchVersion B32;
    CARD32  pad0 B32;
    CARD32  pad1 B32;
    CARD32  pad2 B32;
} xDMXQueryVersionReply;
#define sz_xDMXQueryVersionReply 32

/** Wire-level description of DMXSync protocol request. */
typedef struct {
    CARD8   reqType;            /* DMXCode */
    CARD8   dmxReqType;         /* X_DMXSync */
    CARD16  length B16;
} xDMXSyncReq;
#define sz_xDMXSyncReq 4

/** Wire-level description of DMXSync protocol reply. */
typedef struct {
    BYTE    type;               /* X_Reply */
    CARD8   unused;
    CARD16  sequenceNumber B16;
    CARD32  length B32;
    CARD32  status B32;
    CARD32  pad0 B32;
    CARD32  pad1 B32;
    CARD32  pad2 B32;
    CARD32  pad3 B32;
    CARD32  pad4 B32;
} xDMXSyncReply;
#define sz_xDMXSyncReply 32

/** Wire-level description of DMXForceWindowCreation protocol request. */
typedef struct {
    CARD8   reqType;            /* DMXCode */
    CARD8   dmxReqType;         /* X_DMXForceWindowCreation */
    CARD16  length B16;
    CARD32  window B32;
} xDMXForceWindowCreationReq;
#define sz_xDMXForceWindowCreationReq 8

/** Wire-level description of DMXForceWindowCreation protocol reply. */
typedef struct {
    BYTE    type;               /* X_Reply */
    CARD8   unused;
    CARD16  sequenceNumber B16;
    CARD32  length B32;
    CARD32  status B32;
    CARD32  pad0 B32;
    CARD32  pad1 B32;
    CARD32  pad2 B32;
    CARD32  pad3 B32;
    CARD32  pad4 B32;
} xDMXForceWindowCreationReply;
#define sz_xDMXForceWindowCreationReply 32

/** Wire-level description of DMXGetScreenCount protocol request. */
typedef struct {
    CARD8   reqType;            /* DMXCode */
    CARD8   dmxReqType;         /* X_DMXGetScreenCount */
    CARD16  length B16;
} xDMXGetScreenCountReq;
#define sz_xDMXGetScreenCountReq 4

/** Wire-level description of DMXGetScreenCount protocol reply. */
typedef struct {
    BYTE    type;               /* X_Reply */
    CARD8   unused;
    CARD16  sequenceNumber B16;
    CARD32  length B32;
    CARD32  screenCount B32;
    CARD32  pad0 B32;
    CARD32  pad1 B32;
    CARD32  pad2 B32;
    CARD32  pad3 B32;
    CARD32  pad4 B32;
} xDMXGetScreenCountReply;
#define sz_xDMXGetScreenCountReply 32

/** Wire-level description of DMXGetScreenAttributes protocol request. */
typedef struct {
    CARD8   reqType;            /* DMXCode */
    CARD8   dmxReqType;         /* X_DMXGetScreenAttributes */
    CARD16  length B16;
    CARD32  physicalScreen B32;
} xDMXGetScreenAttributesReq;
#define sz_xDMXGetScreenAttributesReq 8

/** Wire-level description of DMXGetScreenAttributes protocol reply. */
typedef struct {
    BYTE    type;               /* X_Reply */
    CARD8   unused;
    CARD16  sequenceNumber B16;
    CARD32  length B32;
    CARD32  displayNameLength B32;
    CARD32  logicalScreen B32;

    CARD16  screenWindowWidth B16;
    CARD16  screenWindowHeight B16;
    INT16   screenWindowXoffset B16;
    INT16   screenWindowYoffset B16;

    CARD16  rootWindowWidth B16;
    CARD16  rootWindowHeight B16;
    INT16   rootWindowXoffset B16;
    INT16   rootWindowYoffset B16;
    INT16   rootWindowXorigin B16;
    INT16   rootWindowYorigin B16;
} xDMXGetScreenAttributesReply;
#define sz_xDMXGetScreenAttributesReply 36

/** Wire-level description of DMXChangeScreensAttributes protocol request. */
typedef struct {
    CARD8   reqType;            /* DMXCode */
    CARD8   dmxReqType;         /* X_DMXChangeScreensAttributes */
    CARD16  length B16;
    CARD32  screenCount B32;
    CARD32  maskCount B32;
} xDMXChangeScreensAttributesReq;
#define sz_xDMXChangeScreensAttributesReq 12

/** Wire-level description of DMXChangeScreensAttributes protocol reply. */
typedef struct {
    BYTE    type;               /* X_Reply */
    CARD8   unused;
    CARD16  sequenceNumber B16;
    CARD32  length B32;
    CARD32  status B32;
    CARD32  errorScreen B32;
    CARD32  pad0 B32;
    CARD32  pad1 B32;
    CARD32  pad2 B32;
    CARD32  pad3 B32;
} xDMXChangeScreensAttributesReply;
#define sz_xDMXChangeScreensAttributesReply 32

/** Wire-level description of DMXAddScreen protocol request. */
typedef struct {
    CARD8   reqType;            /* DMXCode */
    CARD8   dmxReqType;         /* X_DMXAddScreen */
    CARD16  length B16;
    CARD32  displayNameLength B32;
    CARD32  physicalScreen B32;
    CARD32  valueMask B32;
} xDMXAddScreenReq;
#define sz_xDMXAddScreenReq 16

/** Wire-level description of DMXAddScreen protocol reply. */
typedef struct {
    BYTE    type;               /* X_Reply */
    CARD8   unused;
    CARD16  sequenceNumber B16;
    CARD32  length B32;
    CARD32  status B32;
    CARD32  physicalScreen B32;
    CARD32  pad0 B32;
    CARD32  pad1 B32;
    CARD32  pad2 B32;
    CARD32  pad3 B32;
} xDMXAddScreenReply;
#define sz_xDMXAddScreenReply 32

/** Wire-level description of DMXRemoveScreen protocol request. */
typedef struct {
    CARD8   reqType;            /* DMXCode */
    CARD8   dmxReqType;         /* X_DMXRemoveScreen */
    CARD16  length B16;
    CARD32  physicalScreen B32;
} xDMXRemoveScreenReq;
#define sz_xDMXRemoveScreenReq 8

/** Wire-level description of DMXRemoveScreen protocol reply. */
typedef struct {
    BYTE    type;               /* X_Reply */
    CARD8   unused;
    CARD16  sequenceNumber B16;
    CARD32  length B32;
    CARD32  status B32;
    CARD32  pad0 B32;
    CARD32  pad1 B32;
    CARD32  pad2 B32;
    CARD32  pad3 B32;
    CARD32  pad4 B32;
} xDMXRemoveScreenReply;
#define sz_xDMXRemoveScreenReply 32

/** Wire-level description of DMXGetWindowAttributes protocol request. */
typedef struct {
    CARD8   reqType;            /* DMXCode */
    CARD8   dmxReqType;         /* X_DMXGetWindowAttributes */
    CARD16  length B16;
    CARD32  window B32;
} xDMXGetWindowAttributesReq;
#define sz_xDMXGetWindowAttributesReq 8

/** Wire-level description of DMXGetWindowAttributes protocol reply. */
typedef struct {
    BYTE    type;               /* X_Reply */
    CARD8   unused;
    CARD16  sequenceNumber B16;
    CARD32  length B32;
    CARD32  screenCount B32;
    CARD32  pad0 B32;
    CARD32  pad1 B32;
    CARD32  pad2 B32;
    CARD32  pad3 B32;
    CARD32  pad4 B32;
} xDMXGetWindowAttributesReply;
#define sz_xDMXGetWindowAttributesReply 32

/** Wire-level description of DMXGetDesktopAttributes protocol request. */
typedef struct {
    CARD8   reqType;            /* DMXCode */
    CARD8   dmxReqType;         /* X_DMXGetDesktopAttributes */
    CARD16  length B16;
} xDMXGetDesktopAttributesReq;
#define sz_xDMXGetDesktopAttributesReq 4

/** Wire-level description of DMXGetDesktopAttributes protocol reply. */
typedef struct {
    BYTE    type;               /* X_Reply */
    CARD8   unused;
    CARD16  sequenceNumber B16;
    CARD32  length B32;
    INT16   width;
    INT16   height;
    INT16   shiftX;
    INT16   shiftY;
    CARD32  pad0 B32;
    CARD32  pad1 B32;
    CARD32  pad2 B32;
    CARD32  pad3 B32;
} xDMXGetDesktopAttributesReply;
#define sz_xDMXGetDesktopAttributesReply 32

/** Wire-level description of DMXChangeDesktopAttributes protocol request. */
typedef struct {
    CARD8   reqType;            /* DMXCode */
    CARD8   dmxReqType;         /* X_DMXChangeDesktopAttributes */
    CARD16  length B16;
    CARD32  valueMask B32;
} xDMXChangeDesktopAttributesReq;
#define sz_xDMXChangeDesktopAttributesReq 8

/** Wire-level description of DMXChangeDesktopAttributes protocol reply. */
typedef struct {
    BYTE    type;               /* X_Reply */
    CARD8   unused;
    CARD16  sequenceNumber B16;
    CARD32  length B32;
    CARD32  status B32;
    CARD32  pad0 B32;
    CARD32  pad1 B32;
    CARD32  pad2 B32;
    CARD32  pad3 B32;
    CARD32  pad4 B32;
} xDMXChangeDesktopAttributesReply;
#define sz_xDMXChangeDesktopAttributesReply 32

/** Wire-level description of DMXGetInputCount protocol request. */
typedef struct {
    CARD8   reqType;            /* DMXCode */
    CARD8   dmxReqType;         /* X_DMXGetInputCount */
    CARD16  length B16;
} xDMXGetInputCountReq;
#define sz_xDMXGetInputCountReq 4

/** Wire-level description of DMXGetInputCount protocol reply. */
typedef struct {
    BYTE    type;               /* X_Reply */
    CARD8   unused;
    CARD16  sequenceNumber B16;
    CARD32  length B32;
    CARD32  inputCount B32;
    CARD32  pad0 B32;
    CARD32  pad1 B32;
    CARD32  pad2 B32;
    CARD32  pad3 B32;
    CARD32  pad4 B32;
} xDMXGetInputCountReply;
#define sz_xDMXGetInputCountReply 32

/** Wire-level description of DMXGetInputAttributes protocol request. */
typedef struct {
    CARD8   reqType;            /* DMXCode */
    CARD8   dmxReqType;         /* X_DMXGetInputAttributes */
    CARD16  length B16;
    CARD32  deviceId B32;
} xDMXGetInputAttributesReq;
#define sz_xDMXGetInputAttributesReq 8

/** Wire-level description of DMXGetInputAttributes protocol reply. */
typedef struct {
    BYTE    type;               /* X_Reply */
    CARD8   unused;
    CARD16  sequenceNumber B16;
    CARD32  length B32;
    CARD32  inputType B32;
    CARD32  physicalScreen B32;
    CARD32  physicalId B32;
    CARD32  nameLength B32;
    BOOL    isCore;
    BOOL    sendsCore;
    BOOL    detached;
    CARD8   pad0;
    CARD32  pad1 B32;
} xDMXGetInputAttributesReply;
#define sz_xDMXGetInputAttributesReply 32

/** Wire-level description of DMXAddInput protocol request. */
typedef struct {
    CARD8   reqType;            /* DMXCode */
    CARD8   dmxReqType;         /* X_DMXAddInput */
    CARD16  length B16;
    CARD32  displayNameLength B32;
    CARD32  valueMask;
} xDMXAddInputReq;
#define sz_xDMXAddInputReq 12

/** Wire-level description of DMXAddInput protocol reply. */
typedef struct {
    BYTE    type;               /* X_Reply */
    CARD8   unused;
    CARD16  sequenceNumber B16;
    CARD32  length B32;
    CARD32  status B32;
    CARD32  physicalId B32;
    CARD32  pad0 B32;
    CARD32  pad1 B32;
    CARD32  pad2 B32;
    CARD32  pad3 B32;
} xDMXAddInputReply;
#define sz_xDMXAddInputReply 32

/** Wire-level description of DMXRemoveInput protocol request. */
typedef struct {
    CARD8   reqType;            /* DMXCode */
    CARD8   dmxReqType;         /* X_DMXRemoveInput */
    CARD16  length B16;
    CARD32  physicalId B32;
} xDMXRemoveInputReq;
#define sz_xDMXRemoveInputReq 8

/** Wire-level description of DMXRemoveInput protocol reply. */
typedef struct {
    BYTE     type;
    CARD8    unused;
    CARD16   sequenceNumber B16;
    CARD32   length B32;
    CARD32   status B32;
    CARD32   pad0 B32;
    CARD32   pad1 B32;
    CARD32   pad2 B32;
    CARD32   pad3 B32;
    CARD32   pad4 B32;
} xDMXRemoveInputReply;
#define sz_xDMXRemoveInputReply 32

#endif
