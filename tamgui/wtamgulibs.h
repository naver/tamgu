/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : wtamgulibs.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#pragma once


// wtamgulibs dialog

class wtamgulibs : public CDialogEx
{
	DECLARE_DYNAMIC(wtamgulibs)

public:
	wtamgulibs(CWnd* pParent = NULL);   // standard constructor
	virtual ~wtamgulibs();

// Dialog Data
	enum { IDD = IDD_TAMGUILIBSDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString libspath;
};
