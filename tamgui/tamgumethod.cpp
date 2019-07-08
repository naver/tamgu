/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgumethod.cpp
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

// tamgumethod.cpp : implementation file
//

#include "stdafx.h"
#include "TAMGUI.h"
#include "tamgu.h"
#include "afxdialogex.h"


// tamgumethod dialog

IMPLEMENT_DYNAMIC(tamgumethod, CDialogEx)

tamgumethod::tamgumethod(CWnd* pParent /*=NULL*/)
	: CDialogEx(tamgumethod::IDD, pParent)
	, atype(_T(""))
	, letexte(_T(""))
	, lemessage(_T("empty"))
{
	x = 0; y = 0; 
}

tamgumethod::~tamgumethod()
{
}

void tamgumethod::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBOLISTE, laliste);
	DDX_Text(pDX, IDC_EDIT2, atype);
	DDX_CBString(pDX, IDC_COMBOLISTE, letexte);
}


BEGIN_MESSAGE_MAP(tamgumethod, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBOLISTE, &tamgumethod::OnCbnSelchangeComboliste)	
END_MESSAGE_MAP()

BOOL tamgumethod::PreTranslateMessage(MSG* pMsg) {
	int i = laliste.GetCurSel();
	if (i >= 0 && i <= listes.size()) {
		CString cmsg;
		laliste.GetLBText(i, cmsg);
		wstring s = cmsg;
		if (infos.find(s) != infos.end()) {
			s = infos[s];
			lemessage = s.c_str();
			if (lemessage != dernierechaine) {
				dernierechaine = lemessage;
				m_ToolTip.UpdateTipText(lemessage, &laliste);
			}
		}
		else {
			lemessage = "No description";
			if (lemessage != dernierechaine) {
				dernierechaine = lemessage;
				m_ToolTip.UpdateTipText(lemessage, &laliste);
			}
		}
	}

	m_ToolTip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}


// tamgumethod message handlers


void tamgumethod::OnCbnSelchangeComboliste()
{
	// TODO: Add your control notification handler code here	
	if (laliste.GetDroppedState()) {
		int i = laliste.GetCurSel();
		if (i >= 0 && i <= listes.size()) {
			CString cmsg;
			laliste.GetLBText(i, cmsg);
			resultat = cmsg;
		}
		CDialogEx::OnOK();
	}
}

BOOL tamgumethod::OnInitDialog() {
	BOOL v = CDialogEx::OnInitDialog();
	m_ToolTip.Create(this);
	m_ToolTip.AddTool(&laliste, lemessage);
	SetWindowPos(NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	for (int i = 0; i < listes.size(); i++)
		laliste.AddString(listes[i].c_str());
	laliste.SetCurSel(0);
	return v;
}


