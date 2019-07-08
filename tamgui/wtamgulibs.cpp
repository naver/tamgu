/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : wtamgulibs.cpp
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

// wtamgulibs.cpp : implementation file
//

#include "stdafx.h"
#include "TAMGUI.h"
#include "wtamgulibs.h"
#include "afxdialogex.h"


// wtamgulibs dialog

IMPLEMENT_DYNAMIC(wtamgulibs, CDialogEx)

wtamgulibs::wtamgulibs(CWnd* pParent /*=NULL*/)
	: CDialogEx(wtamgulibs::IDD, pParent)
	, libspath(_T(""))
{

}

wtamgulibs::~wtamgulibs()
{
}

void wtamgulibs::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TAMGUILIBSPATH, libspath);
}


BEGIN_MESSAGE_MAP(wtamgulibs, CDialogEx)
END_MESSAGE_MAP()


// wtamgulibs message handlers
