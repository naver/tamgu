/*
 * Copyright © 2008 Red Hat, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Soft-
 * ware"), to deal in the Software without restriction, including without
 * limitation the rights to use, copy, modify, merge, publish, distribute,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, provided that the above copyright
 * notice(s) and this permission notice appear in all copies of the Soft-
 * ware and that both the above copyright notice(s) and this permission
 * notice appear in supporting documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABIL-
 * ITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY
 * RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN
 * THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSE-
 * QUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFOR-
 * MANCE OF THIS SOFTWARE.
 *
 * Except as contained in this notice, the name of a copyright holder shall
 * not be used in advertising or otherwise to promote the sale, use or
 * other dealings in this Software without prior written authorization of
 * the copyright holder.
 *
 * Authors:
 *   Kristian Høgsberg (krh@redhat.com)
 */

#ifndef _DRI2_PROTO_H_
#define _DRI2_PROTO_H_

#define DRI2_NAME			"DRI2"
#define DRI2_MAJOR			1
#define DRI2_MINOR			4

#define DRI2NumberErrors		0
#define DRI2NumberEvents		2
#define DRI2NumberRequests		14

#define X_DRI2QueryVersion		0
#define X_DRI2Connect			1
#define X_DRI2Authenticate		2
#define X_DRI2CreateDrawable		3
#define X_DRI2DestroyDrawable		4
#define X_DRI2GetBuffers		5
#define X_DRI2CopyRegion		6
#define X_DRI2GetBuffersWithFormat	7
#define X_DRI2SwapBuffers		8
#define X_DRI2GetMSC			9
#define X_DRI2WaitMSC			10
#define X_DRI2WaitSBC			11
#define X_DRI2SwapInterval		12
#define X_DRI2GetParam			13

/*
 * Events
 */
#define DRI2_BufferSwapComplete	0
#define DRI2_InvalidateBuffers	1

typedef struct {
    CARD32  attachment B32;
    CARD32  name B32;
    CARD32  pitch B32;
    CARD32  cpp B32;
    CARD32  flags B32;
} xDRI2Buffer;

typedef struct {
    CARD8   reqType;
    CARD8   dri2ReqType;
    CARD16  length B16;
    CARD32  majorVersion B32;
    CARD32  minorVersion B32;
} xDRI2QueryVersionReq;
#define sz_xDRI2QueryVersionReq   12

typedef struct {
    BYTE    type;   /* X_Reply */
    BYTE    pad1;
    CARD16  sequenceNumber B16;
    CARD32  length B32;
    CARD32  majorVersion B32;
    CARD32  minorVersion B32;
    CARD32  pad2 B32;
    CARD32  pad3 B32;
    CARD32  pad4 B32;
    CARD32  pad5 B32;
} xDRI2QueryVersionReply;
#define sz_xDRI2QueryVersionReply	32

typedef struct {
    CARD8   reqType;
    CARD8   dri2ReqType;
    CARD16  length B16;
    CARD32  window B32;
    CARD32  driverType B32;
} xDRI2ConnectReq;
#define sz_xDRI2ConnectReq	12

typedef struct {
    BYTE    type;   /* X_Reply */
    BYTE    pad1;
    CARD16  sequenceNumber B16;
    CARD32  length B32;
    CARD32  driverNameLength B32;
    CARD32  deviceNameLength B32;
    CARD32  pad2 B32;
    CARD32  pad3 B32;
    CARD32  pad4 B32;
    CARD32  pad5 B32;
} xDRI2ConnectReply;
#define sz_xDRI2ConnectReply	32

typedef struct {
    CARD8   reqType;
    CARD8   dri2ReqType;
    CARD16  length B16;
    CARD32  window B32;
    CARD32  magic B32;
} xDRI2AuthenticateReq;
#define sz_xDRI2AuthenticateReq   12

typedef struct {
    BYTE    type;   /* X_Reply */
    BYTE    pad1;
    CARD16  sequenceNumber B16;
    CARD32  length B32;
    CARD32  authenticated B32;
    CARD32  pad2 B32;
    CARD32  pad3 B32;
    CARD32  pad4 B32;
    CARD32  pad5 B32;
    CARD32  pad6 B32;
} xDRI2AuthenticateReply;
#define sz_xDRI2AuthenticateReply	32

typedef struct {
    CARD8   reqType;
    CARD8   dri2ReqType;
    CARD16  length B16;
    CARD32  drawable B32;
} xDRI2CreateDrawableReq;
#define sz_xDRI2CreateDrawableReq   8

typedef struct {
    CARD8   reqType;
    CARD8   dri2ReqType;
    CARD16  length B16;
    CARD32  drawable B32;
} xDRI2DestroyDrawableReq;
#define sz_xDRI2DestroyDrawableReq   8

typedef struct {
    CARD8   reqType;
    CARD8   dri2ReqType;
    CARD16  length B16;
    CARD32  drawable B32;
    CARD32  count B32;
} xDRI2GetBuffersReq;
#define sz_xDRI2GetBuffersReq   12

typedef struct {
    BYTE    type;   /* X_Reply */
    BYTE    pad1;
    CARD16  sequenceNumber B16;
    CARD32  length B32;
    CARD32  width B32;
    CARD32  height B32;
    CARD32  count B32;
    CARD32  pad2 B32;
    CARD32  pad3 B32;
    CARD32  pad4 B32;
} xDRI2GetBuffersReply;
#define sz_xDRI2GetBuffersReply	32

typedef struct {
    CARD8   reqType;
    CARD8   dri2ReqType;
    CARD16  length B16;
    CARD32  drawable B32;
    CARD32  region B32;
    CARD32  dest B32;
    CARD32  src B32;
} xDRI2CopyRegionReq;
#define sz_xDRI2CopyRegionReq   20

typedef struct {
    BYTE    type;   /* X_Reply */
    BYTE    pad1;
    CARD16  sequenceNumber B16;
    CARD32  length B32;
    CARD32  pad2 B32;
    CARD32  pad3 B32;
    CARD32  pad4 B32;
    CARD32  pad5 B32;
    CARD32  pad6 B32;
    CARD32  pad7 B32;
} xDRI2CopyRegionReply;
#define sz_xDRI2CopyRegionReply	32

typedef struct {
    CARD8   reqType;
    CARD8   dri2ReqType;
    CARD16  length B16;
    CARD32  drawable B32;
    CARD32  target_msc_hi B32;
    CARD32  target_msc_lo B32;
    CARD32  divisor_hi B32;
    CARD32  divisor_lo B32;
    CARD32  remainder_hi B32;
    CARD32  remainder_lo B32;
} xDRI2SwapBuffersReq;
#define sz_xDRI2SwapBuffersReq  32

typedef struct {
    BYTE    type;   /* X_Reply */
    BYTE    pad1;
    CARD16  sequenceNumber B16;
    CARD32  length B32;
    CARD32  swap_hi B32;
    CARD32  swap_lo B32;
    CARD32  pad2 B32;
    CARD32  pad3 B32;
    CARD32  pad4 B32;
    CARD32  pad5 B32;
} xDRI2SwapBuffersReply;
#define sz_xDRI2SwapBuffersReply 32

typedef struct {
    CARD8   reqType;
    CARD8   dri2ReqType;
    CARD16  length B16;
    CARD32  drawable B32;
} xDRI2GetMSCReq;
#define sz_xDRI2GetMSCReq 8

typedef struct {
    CARD8   reqType;
    CARD8   dri2ReqType;
    CARD16  length B16;
    CARD32  drawable B32;
    CARD32  target_msc_hi B32;
    CARD32  target_msc_lo B32;
    CARD32  divisor_hi B32;
    CARD32  divisor_lo B32;
    CARD32  remainder_hi B32;
    CARD32  remainder_lo B32;
} xDRI2WaitMSCReq;
#define sz_xDRI2WaitMSCReq 32

typedef struct {
    CARD8   reqType;
    CARD8   dri2ReqType;
    CARD16  length B16;
    CARD32  drawable B32;
    CARD32  target_sbc_hi B32;
    CARD32  target_sbc_lo B32;
} xDRI2WaitSBCReq;
#define sz_xDRI2WaitSBCReq 16

typedef struct {
    CARD8   type;
    CARD8   pad1;
    CARD16  sequenceNumber B16;
    CARD32  length B32;
    CARD32  ust_hi B32;
    CARD32  ust_lo B32;
    CARD32  msc_hi B32;
    CARD32  msc_lo B32;
    CARD32  sbc_hi B32;
    CARD32  sbc_lo B32;
} xDRI2MSCReply;
#define sz_xDRI2MSCReply 32

typedef struct {
    CARD8   reqType;
    CARD8   dri2ReqType;
    CARD16  length B16;
    CARD32  drawable B32;
    CARD32  interval B32;
} xDRI2SwapIntervalReq;
#define sz_xDRI2SwapIntervalReq 12

typedef struct {
    CARD8 type;
    CARD8 pad;
    CARD16 sequenceNumber B16;
    CARD16 event_type B16;
    CARD16 pad2;
    CARD32 drawable B32;
    CARD32 ust_hi B32;
    CARD32 ust_lo B32;
    CARD32 msc_hi B32;
    CARD32 msc_lo B32;
    CARD32 sbc_hi B32;
    CARD32 sbc_lo B32;
} xDRI2BufferSwapComplete;
#define sz_xDRI2BufferSwapComplete 32

typedef struct {
    CARD8 type;
    CARD8 pad;
    CARD16 sequenceNumber B16;
    CARD16 event_type B16;
    CARD16 pad2;
    CARD32 drawable B32;
    CARD32 ust_hi B32;
    CARD32 ust_lo B32;
    CARD32 msc_hi B32;
    CARD32 msc_lo B32;
    CARD32 sbc B32;
} xDRI2BufferSwapComplete2;
#define sz_xDRI2BufferSwapComplete2 32

typedef struct {
    CARD8 type;
    CARD8 pad;
    CARD16 sequenceNumber B16;
    CARD32 drawable B32;
    CARD32 pad1 B32;
    CARD32 pad2 B32;
    CARD32 pad3 B32;
    CARD32 pad4 B32;
    CARD32 pad5 B32;
    CARD32 pad6 B32;
} xDRI2InvalidateBuffers;
#define sz_xDRI2InvalidateBuffers 32

typedef struct {
    CARD8   reqType;
    CARD8   dri2ReqType;
    CARD16  length B16;
    CARD32  drawable B32;
    CARD32  param B32;
} xDRI2GetParamReq;
#define sz_xDRI2GetParamReq 12

typedef struct {
    BYTE    type; /*X_Reply*/
    BOOL    is_param_recognized;
    CARD16  sequenceNumber B16;
    CARD32  length B32;
    CARD32  value_hi B32;
    CARD32  value_lo B32;
    CARD32  pad1 B32;
    CARD32  pad2 B32;
    CARD32  pad3 B32;
    CARD32  pad4 B32;
} xDRI2GetParamReply;
#define sz_xDRI2GetParamReply 32

#endif
