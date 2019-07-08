/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgusuggestion.cpp
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

// tamgusuggestion.cpp : implementation file
//

#include "stdafx.h"
#include "TAMGUI.h"
#include "tamgu.h"
#include "tamgusuggestion.h"
#include "afxdialogex.h"


// tamgusuggestion dialog

IMPLEMENT_DYNAMIC(tamgusuggestion, CDialogEx)

tamgusuggestion::tamgusuggestion(CWnd* pParent /*=NULL*/)
: CDialogEx(tamgusuggestion::IDD, pParent)
{

}

tamgusuggestion::~tamgusuggestion()
{
}

void tamgusuggestion::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_liste_suggestion, laliste);
}


BEGIN_MESSAGE_MAP(tamgusuggestion, CDialogEx)
	ON_CBN_SELCHANGE(IDC_liste_suggestion, &tamgusuggestion::OnCbnSelchangelistesuggestion)
	ON_CBN_EDITCHANGE(IDC_liste_suggestion, &tamgusuggestion::OnCbnEditchangelistesuggestion)
END_MESSAGE_MAP()


// tamgusuggestion message handlers

BOOL tamgusuggestion::OnInitDialog() {
	BOOL v = CDialogEx::OnInitDialog();
	for (int i = 0; i < listes.size(); i++)
		laliste.AddString(listes[i].c_str());
	return v;
}

void tamgusuggestion::OnCbnSelchangelistesuggestion()
{
	// TODO: Add your control notification handler code here
	int i = laliste.GetCurSel();
	if (i >= 0 && i <= listes.size())
		resultat = listes[i];
	CDialogEx::OnOK();
}


void tamgusuggestion::OnCbnEditchangelistesuggestion()
{
	// TODO: Add your control notification handler code here
	cerr << "";
}
