/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : Arguments.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#pragma once


// Arguments dialog

class Arguments : public CDialogEx
{
	DECLARE_DYNAMIC(Arguments)

public:
	Arguments(CWnd* pParent = NULL);   // standard constructor
	virtual ~Arguments();

// Dialog Data
	enum { IDD = IDD_arguments };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString arguments;
};
