/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : ViewTree.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/


#pragma once

/////////////////////////////////////////////////////////////////////////////
// Fenêtre de CViewTree

class CViewTree : public CTreeCtrl
{
// Construction
public:
	CViewTree();

// Substitutions
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// Implémentation
public:
	virtual ~CViewTree();

protected:
	DECLARE_MESSAGE_MAP()
};
