/************************************************************

Copyright (c) 2003, Oracle and/or its affiliates. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice (including the next
paragraph) shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

************************************************************/

#ifndef _XEVIEPROTO_H_
#define _XEVIEPROTO_H_

#define XEVIENAME "XEVIE"

#define XEVIE_MAJOR_VERSION 1
#define XEVIE_MINOR_VERSION 0

#define X_XevieQueryVersion			0
#define X_XevieStart				1
#define X_XevieEnd				2
#define X_XevieSend				3
#define X_XevieSelectInput			4

#define XevieNumberErrors			0

typedef struct _XevieQueryVersion {
    CARD8   reqType;
    CARD8   xevieReqType;
    CARD16  length B16;
    CARD16  client_major_version B16;
    CARD16  client_minor_version B16;
} xXevieQueryVersionReq;
#define sz_xXevieQueryVersionReq            8

typedef struct {
    BYTE    type;
    BOOL    pad1;
    CARD16  sequence_number B16;
    CARD32  length B32;
    CARD16  server_major_version B16;
    CARD16  server_minor_version B16;
    CARD32  pad2 B32;
    CARD32  pad3 B32;
    CARD32  pad4 B32;
    CARD32  pad5 B32;
    CARD32  pad6 B32;
} xXevieQueryVersionReply;
#define sz_xXevieQueryVersionReply  32

typedef struct _XevieStart {
    CARD8   reqType;
    CARD8   xevieReqType;
    CARD16  length B16;
    CARD32  screen B32;
} xXevieStartReq;
#define sz_xXevieStartReq 8

typedef struct {
    BYTE    type;
    BOOL    pad1;
    CARD16  sequence_number B16;
    CARD32  length B32;
    CARD32  pad2 B32;
    CARD32  pad3 B32;
    CARD32  pad4 B32;
    CARD32  pad5 B32;
    CARD32  pad6 B32;
    CARD32  pad7 B32;
} xXevieStartReply;
#define sz_xXevieStartReply         32

typedef struct _XevieEnd {
    CARD8   reqType;
    CARD8   xevieReqType;
    CARD16  length B16;
    CARD32  cmap B32;
} xXevieEndReq;
#define sz_xXevieEndReq             8

typedef struct {
    BYTE    type;           /* X_Reply */
    BOOL    pad1;
    CARD16  sequence_number B16;
    CARD32  length B32;
    CARD32  pad2 B32;
    CARD32  pad3 B32;
    CARD32  pad4 B32;
    CARD32  pad5 B32;
    CARD32  pad6 B32;
    CARD32  pad7 B32;
} xXevieEndReply;
#define sz_xXevieEndReply   32

typedef struct _XevieSend {
    CARD8       reqType;
    CARD8       xevieReqType;
    CARD16      length B16;
    xEvent      event;
    CARD32      dataType B32;
} xXevieSendReq;
#define sz_xXevieSendReq           104

typedef struct {
    BYTE        type;
    BOOL        pad1;
    CARD16      sequence_number B16;
    CARD32      length B32;
    CARD32      pad2 B32;
    CARD32      pad3 B32;
    CARD32      pad4 B32;
    CARD32      pad5 B32;
    CARD32      pad6 B32;
    CARD32      pad7 B32;
} xXevieSendReply;
#define sz_xXevieSendReply        32

typedef struct _XevieSelectInput {
    CARD8  reqType;
    CARD8  xevieReqType;
    CARD16      length B16;
    CARD32 event_mask B32;
} xXevieSelectInputReq;
#define sz_xXevieSelectInputReq          8

typedef struct {
    BYTE   type;
    BOOL   pad1;
    CARD16 sequence_number B16;
    CARD32 length B32;
    CARD32 pad2 B32;
    CARD32 pad3 B32;
    CARD32 pad4 B32;
    CARD32 pad5 B32;
    CARD32 pad6 B32;
    CARD32 pad7 B32;
} xXevieSelectInputReply;
#define sz_xXevieSelectInputReply        32

#endif /* } _XEVIEPROTO_H_ */

