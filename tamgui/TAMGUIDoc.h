/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : TAMGUIDoc.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/


// TAMGUIDoc.h : interface de la classe CTAMGUIDoc
//

#include <afxrich.h>
#pragma once
const TCHAR XCAR = 26;

class CTAMGUIDoc : public CRichEditDoc
{
protected: // création à partir de la sérialisation uniquement
	CTAMGUIDoc();
	DECLARE_DYNCREATE(CTAMGUIDoc)

// Attributs
public:
		CString contenu;
		CString chemin;
		bool utf8encoding;

// Opérations
public:

// Substitutions
public:
	BOOL OnNewDocument();
	BOOL OnSaveDocument(LPCTSTR lpszPathName);
	void OnCloseDocument();
	void Serialize(CArchive& ar);
	BOOL OnOpenDocument(LPCTSTR lpszPathName);
	CRichEditCntrItem* CreateClientItem(REOBJECT* preo) const {
		return new CRichEditCntrItem(preo,(CRichEditDoc*)this);
	}	

	CString Chemin() {
		return chemin;
	}

#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implémentation
public:
	virtual ~CTAMGUIDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Fonctions générées de la table des messages
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Fonction d'assistance qui définit le contenu de recherche pour un gestionnaire de recherche
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
