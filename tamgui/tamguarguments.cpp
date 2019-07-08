/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguarguments.cpp
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

// tamguarguments.cpp : implementation file
//

#include "stdafx.h"
#include "TAMGUI.h"
#include "tamguarguments.h"
#include "afxdialogex.h"


// tamguarguments dialog

IMPLEMENT_DYNAMIC(tamguarguments, CDialogEx)

tamguarguments::tamguarguments(CWnd* pParent /*=NULL*/)
	: CDialogEx(tamguarguments::IDD, pParent)
	, args(_T(""))
	, indication(_T(""))
{

}

tamguarguments::~tamguarguments()
{
}

void tamguarguments::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, args);
	DDX_Text(pDX, IDC_Indication, indication);
}


BEGIN_MESSAGE_MAP(tamguarguments, CDialogEx)
END_MESSAGE_MAP()


// tamguarguments message handlers
