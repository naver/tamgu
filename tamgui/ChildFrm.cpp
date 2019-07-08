/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : ChildFrm.cpp
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/


// ChildFrm.cpp : implémentation de la classe CChildFrame
//

#include "stdafx.h"
#include "TAMGUI.h"

#include "tamgu.h"
#include "ChildFrm.h"
#include "MainFrm.h"
#include "TAMGUIDoc.h"
#include "TAMGUIView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
END_MESSAGE_MAP()

// construction ou destruction de CChildFrame

CChildFrame::CChildFrame()
{
	// TODO: ajoutez ici le code d'une initialisation de membre
}

CChildFrame::~CChildFrame()
{
}

extern CTAMGUIView* firstview;


BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: changez ici la classe ou les styles Window en modifiant CREATESTRUCT cs
	if( !CMDIChildWndEx::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

// diagnostics pour CChildFrame

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG

// gestionnaires de messages pour CChildFrame
