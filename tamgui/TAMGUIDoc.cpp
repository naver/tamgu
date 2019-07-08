/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : TAMGUIDoc.cpp
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/


// TAMGUIDoc.cpp : implémentation de la classe CTAMGUIDoc
//

#include "stdafx.h"
// SHARED_HANDLERS peuvent être définis dans les gestionnaires d'aperçu, de miniature
// et de recherche d'implémentation de projet ATL et permettent la partage de code de document avec ce projet.
#ifndef SHARED_HANDLERS
#include "TAMGUI.h"
#endif

#include "afxwinappex.h"

#include "TAMGUIDoc.h"
#include "MainFrm.h"

#include <propkey.h>

#include "tamgu.h"
#include "globaltamgu.h"
#include "TAMGUIView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CTAMGUIView* firstview;

// CTAMGUIDoc

IMPLEMENT_DYNCREATE(CTAMGUIDoc, CRichEditDoc)

BEGIN_MESSAGE_MAP(CTAMGUIDoc, CRichEditDoc)
END_MESSAGE_MAP()

void fromCString(string& s,CString l);

bool TestUtf8EncodingFile(CString lefichier) {
	string fichier_en_cours;
	fromCString(fichier_en_cours,lefichier);

	ifstream fichier(STR(fichier_en_cours), openMode);
	if (fichier.fail())
		return false;

	string ligne;
	bool utf=false;

	while (!fichier.eof()) {
		getline(fichier,ligne);
		if (s_is_utf8(USTR(ligne),ligne.size())==true) {
			utf=true;
			break;
		}
	}
	return utf;
}

// construction ou destruction de CTAMGUIDoc
CTAMGUIDoc::CTAMGUIDoc()
{
	// TODO: ajoutez ici le code d'une construction unique
	utf8encoding=true;
}

CTAMGUIDoc::~CTAMGUIDoc()
{
}


CTAMGUIDoc* firstdoc=NULL;

BOOL CTAMGUIDoc::OnNewDocument()
{

	if (!CRichEditDoc::OnNewDocument())
		return FALSE;

	// TODO: ajoutez ici le code de réinitialisation
	// (les documents SDI réutiliseront ce document)

	if (firstdoc==NULL) {
		firstdoc=this;		
		chemin="Console";
		SetTitle(chemin);
	}
	return TRUE;
}



BOOL CTAMGUIDoc::OnOpenDocument(LPCTSTR lpszPathName) {
	BOOL v=CRichEditDoc::OnOpenDocument(lpszPathName);

	if (v) {		
		chemin=lpszPathName;
		POSITION pos = GetFirstViewPosition();
		CTAMGUIView* vue= (CTAMGUIView*)GetNextView(pos);
		vue->Chemin(lpszPathName);
		vue->SetWindowText(contenu);    
		contenu = "";
	}

	return v;
}


void ExitMFCApp()
{
	// same as double-clicking on main window close box
	ASSERT(AfxGetMainWnd() != NULL);
	AfxGetMainWnd()->SendMessage(WM_CLOSE);
}


void CTAMGUIDoc::OnCloseDocument()
{
	bool dest=false;
	if (firstdoc==this) {
		dest = TamguExtinguish();
		SetModifiedFlag(FALSE);
	}

	CRichEditDoc::OnCloseDocument();
	if (firstdoc==this) {
		firstdoc=NULL;
		firstview=NULL;
		//if (dest)
		//	ExitMFCApp();
	}
}

BOOL CTAMGUIDoc::OnSaveDocument(LPCTSTR lpszPathName) {
	if (CRichEditDoc::OnSaveDocument(lpszPathName)==TRUE) {
		chemin=lpszPathName;
		POSITION pos = GetFirstViewPosition();
		CTAMGUIView* vue= (CTAMGUIView*)GetNextView(pos);
		vue->Chemin(lpszPathName);
		long d, f;
		int nFirstVisible = vue->GetRichEditCtrl().GetFirstVisibleLine();
		vue->GetRichEditCtrl().GetSel(d, f);
		vue->Colorie(d, f, nFirstVisible);
		return TRUE;
	}
	return FALSE;
}


// sérialisation de CTAMGUIDoc
void fromstring(CString& commande, string res);
void CTAMGUIDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())	{
		// TODO: ajoutez ici le code de stockage
		CFile* fichier=ar.GetFile();
		string s;
		//Nous sauvegardons en utf8 pour etre lisible sur d'autres plate-formes
		POSITION pos = GetFirstViewPosition();
		CTAMGUIView* vue = (CTAMGUIView*)GetNextView(pos);
		vue->GetWindowText(contenu);    
		TCHAR c = XCAR;
		contenu.Remove(c);
		fromCString(s,contenu);
		if (utf8encoding == false) {
			//we try to detect whether a character is > 255...
			wchar_t w;
			for (int i = 0; i < contenu.GetLength(); i++) {
				w = contenu[i];
				if (w>255) {
					utf8encoding = true;
					break;
				}
			}

			if (utf8encoding==false)
				s = conversion_utf8_to_latin(s);
		}
		contenu = "";
		fichier->Write((char*)(s.c_str()), s.size());
		fichier->Flush();
	}
	else	{
		// TODO: ajoutez ici le code de chargement
		char cars[4100];
		string contnu = "";
		//Nous sauvegardons en utf8 pour etre lisible sur d'autres plate-formes
		//mais lui lit en UNICODE
		utf8encoding = true;

		int nb = ar.Read(cars, 4096);
		bool first = true;
		while (nb == 4096) {
			cars[nb] = 0;
			if (first) {
				//we check the utf8 signature
				if ((uchar)(cars[0]) == 239 && (uchar)(cars[1]) == 187 && (uchar)(cars[2]) == 191)
					contnu += (cars + 3);
				else
					contnu += cars;
				first = false;
			}
			else
				contnu += cars;
			nb = ar.Read(cars, 4096);
		}
		cars[nb] = 0;
		if (first) {
			//we check the utf8 signature
			if ((uchar)(cars[0]) == 239 && (uchar)(cars[1]) == 187 && (uchar)(cars[2]) == 191)
				contnu += (cars + 3);
			else
				contnu += cars;
		}
		else
			contnu += cars;
		s_replacestrings(contnu, "\r\n", "\n");
		s_replacestrings(contnu, "\r", "\n");
		fromstring(contenu, contnu);
	}
}

#ifdef SHARED_HANDLERS

// Prise en charge des miniatures
void CTAMGUIDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modifier ce code pour dessiner les données du document
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support pour les gestionnaires de recherche
void CTAMGUIDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Définir le contenu de recherche à partir des données du document. 
	// Les parties du contenu doivent être séparées par ";"

	// Par exemple :  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CTAMGUIDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// diagnostics pour CTAMGUIDoc

#ifdef _DEBUG
void CTAMGUIDoc::AssertValid() const
{
	CRichEditDoc::AssertValid();
}

void CTAMGUIDoc::Dump(CDumpContext& dc) const
{
	CRichEditDoc::Dump(dc);
}
#endif //_DEBUG


// commandes pour CTAMGUIDoc
