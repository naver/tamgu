/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgusuggestion.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#pragma once
#include "afxwin.h"


// tamgusuggestion dialog

class tamgusuggestion : public CDialogEx
{
	DECLARE_DYNAMIC(tamgusuggestion)

public:
	tamgusuggestion(CWnd* pParent = NULL);   // standard constructor
	virtual ~tamgusuggestion();
	vector<wstring> listes;
	wstring resultat;
	CComboBox laliste;

	BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_Suggestion };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support	

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangelistesuggestion();
	afx_msg void OnCbnEditchangelistesuggestion();
};
