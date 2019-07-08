/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgumethods.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#pragma once
#include "afxwin.h"


// tamgumethods dialog

class tamgumethods : public CDialogEx
{
	DECLARE_DYNAMIC(tamgumethods)

public:
	tamgumethods(CWnd* pParent = NULL);   // standard constructor
	virtual ~tamgumethods();
	vector<wstring> listes;
	wstring resultat;
	CString lemessage;
	int x, y;
	map<wstring, wstring> infos;

	BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_TAMGUIMETHODS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CToolTipCtrl m_ToolTip;
	BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
public:
	CString atype;
	CComboBox laliste;
	afx_msg void OnCbnSelchangeCombo1();
	CString letexte;
	CString dernierechaine;
};
