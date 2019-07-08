/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : debogueur.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#pragma once
#include "afxwin.h"
#include "afxdialogex.h"
#include "TAMGUIDoc.h"
#include "TAMGUIView.h"

#define MET_A_JOUR WM_APP+0x10
#define MET_A_JOUR_COULEUR MET_A_JOUR+1
#define UPDATE_FROM_DEBUGGER MET_A_JOUR_COULEUR + 1
#define UPDATE_END_OF_PROGRAM UPDATE_FROM_DEBUGGER + 1

Tamgu* Debug_callback(vector<Tamgu*>& stack, short idthread, void* data);

// debogueur dialog

class debogueur : public CDialogEx
{
	DECLARE_DYNAMIC(debogueur)

public:
	debogueur(CWnd* pParent = NULL);   // standard constructor
	BOOL OnInitDialog();
	virtual ~debogueur();
	void Lapile(string&);
	void Lasortie(string);
	void Variables(string&);
	CString voutput;
	bool launch;
	ostream* oldos;
	
	hmap<string, hmap<long, bool> > breakpoints;
	string localvariables;

	void Locals();
	CTAMGUIView* currentview;

    Tamgu* stacktop;
    string filename;
    long currentline;
    bool nextline;
    bool getintofunction;
    bool getoutfunction;
    long lastfunction;
    long stackline;
    bool shortname;

// Dialog Data
	enum { IDD = IDD_DEBOGUE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CToolTipCtrl m_ToolTip;
	BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSaute();
	afx_msg void OnBnClickedSortir();
	afx_msg void OnBnClickedFin();
	afx_msg void OnBnClickedSuivant();
	CString vvariables;
	CString vpile;
	afx_msg void OnBnClickedArretprg();
	CEdit lecode;
	CString vlecode;
	afx_msg void OnBnClickedPointarret();
	CButton bsuivant;
	CButton bsaute;
	CButton bsortir;
	CButton bbreak;
	CButton barret;
	CButton bfin;
	CString vsortie;
	CString choixvariables;
	afx_msg void OnEnChangeEdit4();
	afx_msg void OnEnChangeEdit5();
	LRESULT MAJ(WPARAM wParam, LPARAM lParam);
	CString locals;
	CEdit localvar;
	CButton bentre;
	afx_msg void OnBnClickedEntre();
	int threadId;
	BOOL sticktothread;
	afx_msg void OnBnClickedSticktoThread();
	afx_msg void OnEnChangeEdit6();

	bool checkbreakpoint(string& filename) {
		if (breakpoints.find(filename) != breakpoints.end() && breakpoints[filename].find(currentline) != breakpoints[filename].end())
			return true;
		return false;
	}


};
