/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : Arguments.cpp
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

// Arguments.cpp : implementation file
//

#include "stdafx.h"
#include "TAMGUI.h"
#include "Arguments.h"
#include "afxdialogex.h"


// Arguments dialog

IMPLEMENT_DYNAMIC(Arguments, CDialogEx)

Arguments::Arguments(CWnd* pParent /*=NULL*/)
	: CDialogEx(Arguments::IDD, pParent)
	, arguments(_T(""))
{
	
}

Arguments::~Arguments()
{
}

void Arguments::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, arguments);
}


BEGIN_MESSAGE_MAP(Arguments, CDialogEx)
END_MESSAGE_MAP()


// Arguments message handlers
