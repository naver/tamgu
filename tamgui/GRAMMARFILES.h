/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : GRAMMARFILES.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#pragma once
#include "afxcmn.h"


// GRAMMARFILES dialog

class GRAMMARFILES : public CDialogEx
{
	DECLARE_DYNAMIC(GRAMMARFILES)

public:
	GRAMMARFILES(CWnd* pParent = NULL);   // standard constructor
	virtual ~GRAMMARFILES();

// Dialog Data
	enum { IDD = IDD_GRAMMAR_FILES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnDblclkpathfiletree(NMHDR* pNMHDR, LRESULT* pResult);

public:
	CTreeCtrl m_pathtree;

	HTREEITEM featureroot;
	HTREEITEM lexiconroot;
	HTREEITEM ruleroot;
	HTREEITEM paramroot;
	HTREEITEM diversroot;
	HTREEITEM scriptsroot;
	HTREEITEM baseroot;

	vector<string> m_feature_pathnames;
	vector<string> m_lexicon_pathnames;
	vector<string> m_rule_pathnames;
	vector<string> m_param_pathnames;
	vector<string> m_scripts_pathnames;
	vector<string> m_divers_pathnames;	


	CString selectedpath;
};
