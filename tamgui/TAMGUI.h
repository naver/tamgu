/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : TAMGUI.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/


// TAMGUI.h : fichier d'en-tête principal pour l'application TAMGUI
//
#pragma once

#ifndef __AFXWIN_H__
	#error "incluez 'stdafx.h' avant d'inclure ce fichier pour PCH"
#endif

#include "resource.h"       // symboles principaux


// CTAMGUIApp:
// Consultez TAMGUI.cpp pour l'implémentation de cette classe
//

class CTAMGUIApp : public CWinAppEx
{
public:
	CTAMGUIApp();


// Substitutions
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implémentation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnFichierSettamgulibs();
};

extern CTAMGUIApp theApp;
