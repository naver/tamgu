/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgumethods.cpp
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

// tamgumethods.cpp : implementation file
//

#include "stdafx.h"
#include "TAMGUI.h"
#include "tamgu.h"
#include "tamgumethods.h"
#include "afxdialogex.h"


// tamgumethods dialog

IMPLEMENT_DYNAMIC(tamgumethods, CDialogEx)

tamgumethods::tamgumethods(CWnd* pParent /*=NULL*/)
	: CDialogEx(tamgumethods::IDD, pParent)
	, atype(_T(""))
	, letexte(_T(""))
	, lemessage(_T(""))
{
}

tamgumethods::~tamgumethods()
{
}

void tamgumethods::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, atype);
	DDX_Control(pDX, IDC_COMBO1, laliste);
	DDX_CBString(pDX, IDC_COMBO1, letexte);
}


BEGIN_MESSAGE_MAP(tamgumethods, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO1, &tamgumethods::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// tamgumethods message handlers


void tamgumethods::OnCbnSelchangeCombo1()
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

BOOL tamgumethods::OnInitDialog() {
	BOOL v = CDialogEx::OnInitDialog();
	m_ToolTip.Create(this);
	m_ToolTip.AddTool(&laliste, lemessage);
	SetWindowPos(NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	for (int i = 0; i < listes.size(); i++)
		laliste.AddString(listes[i].c_str());
	laliste.SetCurSel(0);
	return v;
}

BOOL tamgumethods::PreTranslateMessage(MSG* pMsg) {
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
