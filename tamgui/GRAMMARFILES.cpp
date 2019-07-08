/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : GRAMMARFILES.cpp
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

// GRAMMARFILES.cpp : implementation file
//

#include "stdafx.h"
#include "TAMGUI.h"
#include "pasofficiel.h"
#include "GRAMMARFILES.h"
#include "afxdialogex.h"

extern vector<string> feature_pathnames;
extern vector<string> lexicon_pathnames;
extern vector<string> rule_pathnames;
extern vector<string> param_pathnames;
extern vector<string> scripts_pathnames;
extern vector<string> divers_pathnames;
extern string languageFilename;
extern string grmFilename;

// GRAMMARFILES dialog

IMPLEMENT_DYNAMIC(GRAMMARFILES, CDialogEx)

GRAMMARFILES::GRAMMARFILES(CWnd* pParent /*=NULL*/)
: CDialogEx(GRAMMARFILES::IDD, pParent)
{

}

BOOL GRAMMARFILES::OnInitDialog() {
	BOOL v = CDialogEx::OnInitDialog();

	baseroot = m_pathtree.InsertItem(L"BASES");
	featureroot = m_pathtree.InsertItem(L"FEATURES");
	lexiconroot = m_pathtree.InsertItem(L"LEXICONS");
	ruleroot = m_pathtree.InsertItem(L"RULES");
	scriptsroot = m_pathtree.InsertItem(L"SCRIPTS");
	diversroot = m_pathtree.InsertItem(L"MISCELLANEOUS");
	paramroot = m_pathtree.InsertItem(L"PARAMETERS");

	int i;
	CString s;
	s = grmFilename.c_str();
	m_pathtree.InsertItem(s, baseroot);

	s = languageFilename.c_str();
	m_pathtree.InsertItem(s, baseroot);

	for (i = 0; i<feature_pathnames.size(); i++) {
		m_feature_pathnames.push_back(feature_pathnames[i]);
		s = feature_pathnames[i].c_str();
		m_pathtree.InsertItem(s, featureroot);
	}

	for (i = 0; i<lexicon_pathnames.size(); i++) {
		m_lexicon_pathnames.push_back(lexicon_pathnames[i]);
		s = lexicon_pathnames[i].c_str();
		m_pathtree.InsertItem(s, lexiconroot);
	}

	for (i = 0; i<rule_pathnames.size(); i++) {
		m_rule_pathnames.push_back(rule_pathnames[i]);
		s = rule_pathnames[i].c_str();
		m_pathtree.InsertItem(s, ruleroot);
	}

	for (i = 0; i<param_pathnames.size(); i++) {
		m_param_pathnames.push_back(param_pathnames[i]);
		s = param_pathnames[i].c_str();
		m_pathtree.InsertItem(s, paramroot);
	}

	for (i = 0; i<divers_pathnames.size(); i++) {
		m_divers_pathnames.push_back(divers_pathnames[i]);
		s = divers_pathnames[i].c_str();
		m_pathtree.InsertItem(s, diversroot);
	}

	for (i = 0; i<scripts_pathnames.size(); i++) {
		m_scripts_pathnames.push_back(scripts_pathnames[i]);
		s = scripts_pathnames[i].c_str();
		m_pathtree.InsertItem(s, scriptsroot);
	}

	m_pathtree.Expand(baseroot, TVE_EXPAND);
	m_pathtree.Expand(featureroot, TVE_EXPAND);
	m_pathtree.Expand(ruleroot, TVE_EXPAND);
	m_pathtree.Expand(lexiconroot, TVE_EXPAND);
	m_pathtree.Expand(paramroot, TVE_EXPAND);
	m_pathtree.Expand(diversroot, TVE_EXPAND);
	m_pathtree.Expand(scriptsroot, TVE_EXPAND);
	
	return(v);
}

GRAMMARFILES::~GRAMMARFILES()
{
}

void GRAMMARFILES::OnDblclkpathfiletree(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_pathtree.GetSelectedItem();
	if (hItem!=NULL)
		selectedpath = m_pathtree.GetItemText(hItem);
	OnOK();	
}


void GRAMMARFILES::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_pathtree);
}


BEGIN_MESSAGE_MAP(GRAMMARFILES, CDialogEx)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, OnDblclkpathfiletree)
END_MESSAGE_MAP()


// GRAMMARFILES message handlers
