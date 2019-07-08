/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : OutputWnd.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/


#pragma once

/////////////////////////////////////////////////////////////////////////////
// Fenêtre de COutputList

class COutputList : public CListBox
{
// Construction
public:
	COutputList();

// Implémentation
public:
	virtual ~COutputList();

protected:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnViewOutput();

	DECLARE_MESSAGE_MAP()
};

class COutputWnd : public CMFCTasksPane
{
// Construction
public:
	COutputWnd();

	void UpdateFonts();
	void MetMessage(CString& msg);
	void MetDebug(CString& msg);
	void CleanDebugWindow();
	void CleanMessageWindow();
	void OnPressForwardButton();
	void OnPressBackButton();
// Attributs
protected:
	CMFCTabCtrl	m_wndTabs;

	COutputList m_wndOutputBuild;
	COutputList m_wndOutputDebug;
	COutputList m_wndOutputFind;

protected:
	void FillBuildWindow(CString& msg);
	void FillDebugWindow(CString& msg);
	void FillFindWindow();
	void AdjustHorzScroll(CListBox& wndListBox);

// Implémentation
public:
	virtual ~COutputWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnForward();
	afx_msg void OnBack();
	DECLARE_MESSAGE_MAP()
};

