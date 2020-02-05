/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : TAMGUIView.cpp
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/


// TAMGUIView.cpp : implémentation de la classe CTAMGUIView
//

#include "stdafx.h"
// SHARED_HANDLERS peuvent être définis dans les gestionnaires d'aperçu, de miniature
// et de recherche d'implémentation de projet ATL et permettent la partage de code de document avec ce projet.
#ifndef SHARED_HANDLERS
#include "TAMGUI.h"
#endif

#include "tamgu.h"
#include "globaltamgu.h"
#include "TAMGUIDoc.h"
#include "TAMGUIView.h"
#include "MainFrm.h"
#include "wtamgulibs.h"

#include "debogueur.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "debogueur.h"

const int nbcolorlines = 5000;


#include "conversion.h"
#include "Arguments.h"

#include <regex>

#define ENDL "\n"
#define WCSTR(x) (wchar_t*)LPCWSTR(x)

CString basearguments;
extern CMainFrame* leMain;


static map<string, string> theinfos;

//-------------------------------------------------------------------------------------------------------------------------------------
//void ResetGetEditor();
wstring GetKeyVal(wstring key);
void SetKeyVal(wstring key, CString value);
Exporting void SetBlankSize(long sz);
Exporting long GetBlankSize();
//-------------------------------------------------------------------------------------------------------------------------------------
LockedThread* loquet = NULL;
//-------------------------------------------------------------------------------------------------------------------------------------

static hmap<short, vector<wstring> > thevectorstrings;
static hmap<short, map<wstring, wstring> > thevectorinfos;
static hmap<wstring, short> tamgutypes;
static hmap<short, short> equivalences;

static bool comparestr(wstring& e1, wstring& e2) {
	if (e1 < e2)
		return(true);
	return(false);
}

static bool TamguLaunching() {
	if (globalTamgu != NULL)
		return false;

	TamguGlobal* global = TamguCreate();
	if (global == NULL)
		return false;
	return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------

static vector<string> tamgukeywords;

static void Getkeywords() {
	if (tamgukeywords.size())
		return;
	TamguLaunching();
	TamguAllObjects(tamgukeywords);
	TamguExtinguish();
}

static void MapInitInfos() {
	static bool first = true;

	if (first == false)
		return;
	first = false;

	Getkeywords();
	for (size_t i = 0; i < tamgukeywords.size(); i++)
		theinfos[tamgukeywords[i]] = tamgukeywords[i];

}

void fromCString(string& f, CString l) {
	s_unicode_to_utf8(f, WCSTR(l), l.GetLength());
}

void fromstring(CString& commande, string res) {
	wstring base;
	s_utf8_to_unicode(base, USTR(res), res.size());
	commande = (WCHAR*)base.c_str();
}

void fromaddstring(CString& commande, string res) {
	wstring base;
	s_utf8_to_unicode(base, USTR(res), res.size());
	commande += (WCHAR*)base.c_str();
}

static vector<CTAMGUIView*> thebreaks;

// CTAMGUIView
static UINT FindDialogMessage = ::RegisterWindowMessage(FINDMSGSTRING);
#define MET_SUGGERE_METHODE MET_A_JOUR_COULEUR+1

IMPLEMENT_DYNCREATE(CTAMGUIView, CRichEditView)

BEGIN_MESSAGE_MAP(CTAMGUIView, CRichEditView)
	// Commandes d'impression standard
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_COMMAND(ID_FILE_PRINT, &CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CTAMGUIView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CHAR()
	ON_COMMAND(ID_TAMGUI_EXECUTER, &CTAMGUIView::OnKifExecuter)
	ON_COMMAND(ID_TAMGUI_REEXECUTER, &CTAMGUIView::OnKifReexecuter)
	ON_COMMAND(ID_TAMGUI_LISTER, &CTAMGUIView::OnKifLister)
	ON_COMMAND(ID_TAMGUI_RAZ, &CTAMGUIView::OnKifRaz)
	ON_COMMAND(ID_TAMGUI_PASAPAS, &CTAMGUIView::OnKifPasaPas)
	ON_COMMAND(ID_TAMGUI_POINTARRET, &CTAMGUIView::OnKifPointarret)
	ON_COMMAND(ID_TAMGUI_NETTOYERP, &CTAMGUIView::OnKifNettoyerp)
	ON_COMMAND(ID_TAMGUI_INDENTER, &CTAMGUIView::OnKifIndenter)
	ON_REGISTERED_MESSAGE(FindDialogMessage, OnFindDialogMessage)
	ON_COMMAND(ID_EDITION_CHERCHE, &CTAMGUIView::OnEditionCherche)
	ON_COMMAND(ID_EDITION_REMPLACE, &CTAMGUIView::OnEditionRemplace)
	ON_COMMAND(ID_TAMGUI_COMMENTER, &CTAMGUIView::OnKifCommenter)
	ON_COMMAND(ID_TAMGUI_BREAK, &CTAMGUIView::OnKifBreak)
	ON_COMMAND(ID_TAMGUI_REEXECUTE, &CTAMGUIView::OnKifAsThread)
	ON_COMMAND(ID_TAMGUI_CORRESPONDANCE, &CTAMGUIView::OnKifCorrespondance)
	ON_COMMAND(ID_TAMGUI_COMPILER, &CTAMGUIView::OnKifCompiler)
	ON_COMMAND(ID_EDITION_SUIVANT, &CTAMGUIView::OnEditionSuivant)
	ON_COMMAND(ID_EDITION_ARGUMENTS, &CTAMGUIView::OnEditionArguments)
	ON_COMMAND(ID_EDITION_ALLERLIGNE, &CTAMGUIView::OnEditionAllerligne)
	ON_MESSAGE(MET_A_JOUR, &CTAMGUIView::MAJ)
	ON_MESSAGE(MET_A_JOUR_COULEUR, &CTAMGUIView::MAJCOULEUR)
	ON_MESSAGE(UPDATE_FROM_DEBUGGER, &CTAMGUIView::MAJDEPUISDEBUG)
	ON_MESSAGE(UPDATE_END_OF_PROGRAM, &CTAMGUIView::ENDOFPROGRAM)
	ON_MESSAGE(MET_SUGGERE_METHODE, &CTAMGUIView::SUGGERE)
	ON_MESSAGE(ID_AFFICHER_ERREUR, &CTAMGUIView::afficher)
	ON_MESSAGE(ID_OUVRIR_DOCUMENT, &CTAMGUIView::ouvrir)
	ON_MESSAGE(IDS_AFFICHAGE_COURANT, &CTAMGUIView::AfficheCourant)
	ON_UPDATE_COMMAND_UI(ID_TAMGUI_EXECUTER, &CTAMGUIView::OnUpdateKifExecuter)
	ON_UPDATE_COMMAND_UI(ID_TAMGUI_REEXECUTE, &CTAMGUIView::OnUpdateKifReexecute)
	ON_UPDATE_COMMAND_UI(ID_TAMGUI_PASAPAS, &CTAMGUIView::OnUpdateKifPasapas)
	ON_UPDATE_COMMAND_UI(ID_TAMGUI_COMPILER, &CTAMGUIView::OnUpdateKifCompiler)
	ON_UPDATE_COMMAND_UI(ID_TAMGUI_BREAK, &CTAMGUIView::OnUpdateKifBreak)
	ON_COMMAND(ID_EDITION_ABANDONNE, &CTAMGUIView::OnEditionAbandonne)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CTAMGUIView::OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &CTAMGUIView::OnUpdateEditRedo)
	ON_COMMAND(ID_TAMGUI_SANSINTERUPTION, &CTAMGUIView::OnKifSansinteruption)
	ON_UPDATE_COMMAND_UI(ID_TAMGUI_SANSINTERUPTION, &CTAMGUIView::OnUpdateKifSansinteruption)
	ON_COMMAND(ID_EDIT_CUT, &CTAMGUIView::OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &CTAMGUIView::OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_PASTE, &CTAMGUIView::OnEditPaste)
	ON_COMMAND(ID_EDITION_SUGGESTION, &CTAMGUIView::OnEditionSuggestion)
	ON_UPDATE_COMMAND_UI(ID_EDITION_SUGGESTION, &CTAMGUIView::OnUpdateEditionSuggestion)
	ON_COMMAND(ID_EDITION_TAILLEINDENTATION, &CTAMGUIView::OnEditionTailleindentation)
	ON_COMMAND(ID_EDITION_PI, &CTAMGUIView::OnEditionPi)
	ON_COMMAND(ID_EDITION_DIVIDE, &CTAMGUIView::OnEditionDivide)
	ON_COMMAND(ID_EDITION_GOLDEN, &CTAMGUIView::OnEditionGolden)
	ON_COMMAND(ID_EDITION_MULTIPLY, &CTAMGUIView::OnEditionMultiply)
	ON_COMMAND(ID_EDITION_SQARE, &CTAMGUIView::OnEditionSqare)
	ON_COMMAND(ID_EDITION_CUBE, &CTAMGUIView::OnEditionCube)
	ON_COMMAND(ID_Menu32863, &CTAMGUIView::OnMenuSquareRoot)
	ON_COMMAND(ID_EDITION_CUBICROOT, &CTAMGUIView::OnEditionCubicroot)
	ON_COMMAND(ID_EDITION_SUM, &CTAMGUIView::OnEditionSum)
	ON_COMMAND(ID_EDITION_PRODUCT, &CTAMGUIView::OnEditionProduct)
	ON_COMMAND(ID_EDITION_EULER, &CTAMGUIView::OnEditionEuler)
	ON_COMMAND(ID_FORMAT_FONT, &CTAMGUIView::OnFormatFont)
	ON_COMMAND(ID_EDIT_DIFFERENT, &CTAMGUIView::OnEditDifferent)
	ON_COMMAND(ID_EDIT_DISJUNCTION, &CTAMGUIView::OnEditDisjunction)
	ON_COMMAND(ID_EDIT_CONJUNCTION, &CTAMGUIView::OnEditConjunction)
	ON_COMMAND(ID_EDIT_NEGATION32889, &CTAMGUIView::OnEditNegation32889)
	ON_COMMAND(ID_EDIT_RULEARROW, &CTAMGUIView::OnEditRulearrow)
	ON_COMMAND(ID_EDIT_RULERIGHTARROW, &CTAMGUIView::OnEditRulerightarrow)
	ON_COMMAND(ID_EDITION_TAU, &CTAMGUIView::OnEditTau)
END_MESSAGE_MAP()

//--------------------------------------------------------------------------------------------------
//-------------------------------COMMUNES-----------------------------------------------------------
// construction ou destruction de CTAMGUIView
static char affichecursor = 0;
static echange lexchange;

class KifViewLock {
public:
	ThreadLock* _locker;

	KifViewLock(ThreadLock* l) : _locker(l) {
		_locker->Locking();
	}

	~KifViewLock() {
		_locker->Unlocking();
	}
};

void echange::initialisation(CTAMGUIView*v, CString& x) {
	compilation = false;
	encours = true;
	vue = v->GetSafeHwnd();
	wvue = v;
	s = x;
}


hmap<wstring, bool> colorkeywords;

x_colorreading::x_colorreading() {
	if (colorkeywords.size() == 0) {
		Getkeywords();
		for (size_t i = 0; i < tamgukeywords.size(); i++) {
			wstring s;
			s_utf8_to_unicode(s, USTR(tamgukeywords[i]), tamgukeywords[i].size());
			colorkeywords[s] = true;
		}
	}
}

void x_colorreading::tokenize(wstring& w, bool keeppos) {
	stack.clear();
	stackln.clear();
	stacktype.clear();
	stacksize.clear();
	cpos.clear();
	apply(w, true, NULL, NULL);

	for (long i = 0; i < stack.size(); i++) {
		stacksize.push_back(stack[i].size());
		stackln[i] = cpos[i] + 1;
		if (stacktype[i] == 4 && colorkeywords.find(stack[i]) != colorkeywords.end())
			stacktype[i] = 3;
	}
}

static bool w_alpha(wchar_t c) {
	if ((c >= 65 && c <= 90) || (c >= 97 && c <= 122) || (c >= 48 && c <= 57) || c == '_' || c == '#' || c > 128)
		return true;
	return false;
}

int Findcomments(CString& ligne) {
	int i;
	int mx = ligne.GetLength();
	for (i = 0; i < mx; i++) {
		if (ligne[i] == '"') {
			i++;
			while (i < mx && ligne[i] != '"') {
				if (ligne[i] == '\\')
					i++;
				i++;
			}
		}
		else
		if (ligne[i] == '\'') {
			i++;
			while (i < mx && ligne[i] != '\'')
				i++;
		}
		else
		if (i < mx - 1 && ligne[i] == '/' && ligne[i + 1] == '/')
			return i;
	}
	return -1;
}

//-------------------------------------------------------------------------------
bool CTAMGUIView::FindDeclarations(wstring& wlocalcode, int& letype) {
	if (wlocalcode == L"")
		return false;


	bnfxs.tokenize(wlocalcode);

	if (bnfxs.stack.size() == 0 || bnfxs.stacktype.back() != 4)
		return false;

	wstring var = bnfxs.stack.back();
	bnfxs.stack.pop_back();
	bnfxs.stackln.pop_back();
	bnfxs.stacktype.pop_back();

	//D'abord on compte les '{}' pour savoir combien il en faut pour avoir une analyse complete...

	wchar_t fnc;
	int j;
	int sz = bnfxs.stack.size();
	vector<wstring> vars;
	int declaration = 0;
	for (j = 0; j < sz; j++) {
		if (bnfxs.stacktype[j] == 4) {
			if (bnfxs.stack[j] == var) {
				int i = j - 1;
				if (i >= 0) {
					if (bnfxs.stack[i][0] == ',') {
						i = j - 2;
						bool stop = false;
						while (i >= 0 && tamgutypes.find(bnfxs.stack[i]) == tamgutypes.end()) {
							if (bnfxs.stack[i][0] == ';') {
								stop = true;
								break;
							}
							i--;
						}
						if (i < 0 || stop)
							continue;
					}

					if (tamgutypes.find(bnfxs.stack[i]) != tamgutypes.end()) {
						if (declaration)
							vars.push_back(L"#");
						vars.push_back(bnfxs.stack[i]);
					}
				}
			}
			continue;
		}

		fnc = bnfxs.stack[j][0];
		switch (fnc) {
		case '(':
			declaration++;
			break;
		case ')':
			declaration--;
			break;
		case '{':
			if (vars.size() >= 2 && vars[vars.size() - 2] == L"#") {
				vars[vars.size() - 2] = L"{";
				break;
			}
			vars.push_back(L"{");
			break;
		case '}':
			while (vars.size() > 0 && vars.back() != L"{")
				vars.pop_back();

			if (vars.size() > 0)
				vars.pop_back();
		}
	}

	if (vars.size()) {
		stype = L"";
		while (vars.size() && vars.back() == L"{")
			vars.pop_back();
		if (vars.size()) {
			stype = vars.back();
			letype = tamgutypes[stype];
			if (equivalences.find(letype) != equivalences.end())
				letype = equivalences[letype];
			return true;
		}
	}

	return false;
}

//-------------------------------------------------------------------------------

CTAMGUIView* firstview = NULL;

//debogueur* deb=NULL;

map<string, CTAMGUIView*> lesvues;

CTAMGUIView* Lavue(string& titre) {
	if (lesvues.find(titre) == lesvues.end())
		return NULL;
	return lesvues[titre];
}


static map<string, bool> coloractives;
static map<string, ThreadLock*> colorlocks;

CTAMGUIView::CTAMGUIView()
{
	// TODO: ajoutez ici du code de construction

	police = _T("Arial Unicode MS");
	tailleHauteur = 240;
	changepolice = false;

	derniereposition = 0;
	begincolor = false;
	suggestion = true;
	firstdisplay = true;
	dernierelongueur = 0;
	initcf = false;
	nbcurly = 0;
	nbsquare = 0;
	affichecursor = 0;
	if (firstview == NULL)
		firstview = this;
}

CTAMGUIView::~CTAMGUIView()
{
	for (int i = 0; i < thebreaks.size(); i++) {
		if (thebreaks[i] == this) {
			thebreaks.erase(thebreaks.begin() + i);
			break;
		}
	}

	lesvues.erase(stitre);
	if (colorlocks.find(stitre) != colorlocks.end()) {
		KifViewLock _kvl(colorlocks[stitre]);
		coloractives[stitre] = true;
	}
}

bool testactivecolor(string& t) {
	KifViewLock _kvl(colorlocks[t]);
	return coloractives[t];
}

void colorthread(void* arg) {
	CTAMGUIView* cview = (CTAMGUIView*)arg;
	string titre = cview->stitre;
	CRichEditCtrl& e = cview->GetRichEditCtrl();
	long db, d, dl, fl;
	long nx = -1;
	bool b = true;
	int i, lng, ps;
	bool test = false;
	bool stopall = true;
	int szc;

	while (1) {

		if (testactivecolor(titre))
			return;
		szc = cview->colorblocks.size();
		db = e.GetFirstVisibleLine();
		if (testactivecolor(titre))
			return;
		dl = e.LineIndex(db + 100);
		if (testactivecolor(titre))
			return;
		db = e.LineIndex(db);
		if (testactivecolor(titre))
			return;
		d = cview->colorblocks[0];
		if (testactivecolor(titre))
			return;
		b = cview->activecolorblocks[d];


		ps = 0;
		test = false;
		stopall = true;
		nx = -1;

		for (i = 0; i < szc; i++) {
			if (testactivecolor(titre))
				return;
			lng = cview->colorblocks[i];
			test = cview->activecolorblocks[lng];
			if (!test)
				stopall = false;

			if (db < lng)
				continue;

			ps = i;
			d = lng;
			b = test;
		}

		if (stopall)
			return;

		if (testactivecolor(titre))
			return;

		ps++;
		if (ps < szc) {
			if (testactivecolor(titre))
				return;

			fl = cview->colorblocks[ps];
			if (fl <= dl) {
				test = cview->activecolorblocks[fl];
				if (testactivecolor(titre))
					return;
				if (!test) {
					ps++;
					if (b) {
						d = fl;
						b = false;
					}
					else {
						if (testactivecolor(titre))
							return;
						cview->activecolorblocks[fl] = true;
					}
				}
			}
		}

		if (b == false) {
			if (testactivecolor(titre))
				return;

			if (ps < szc)
				nx = cview->colorblocks[ps];
			else
				nx = e.GetWindowTextLength();
			if (testactivecolor(titre))
				return;
			cview->activecolorblocks[d] = true;
			if (testactivecolor(titre))
				return;
			HWND hw = cview->GetSafeHwnd();

			::SendMessage(hw, MET_A_JOUR_COULEUR, d, nx);
		}
	}
}

LRESULT CTAMGUIView::SUGGERE(WPARAM wParam, LPARAM lParam) {

	return 0;
}

LRESULT CTAMGUIView::MAJCOULEUR(WPARAM wParam, LPARAM lParam) {
	CRichEditCtrl& e = GetRichEditCtrl();
	int longueur = e.GetWindowTextLength();
	long it = (long)wParam;
	long nx = (long)lParam;

	long fl = e.GetFirstVisibleLine();
	long d = e.LineIndex(e.LineFromChar(it));

	CString prev;
	long bloc;
	bloc = e.LineIndex(e.LineFromChar(nx));
	if (bloc > longueur)
		bloc = longueur;
	e.GetTextRange(d, bloc, prev);
	wstring ws = WCSTR(prev);
	LockWindowUpdate();

	e.HideSelection(TRUE, FALSE);
	ApplyColor(ws, d);
	e.HideSelection(FALSE, FALSE);

	e.SetSel(0, 0);
	e.LineScroll(fl);

	return 1;
}

LRESULT CTAMGUIView::ENDOFPROGRAM(WPARAM wParam, LPARAM lParam) {
	lexchange.encours = false;
	return 1;
}


LRESULT CTAMGUIView::MAJDEPUISDEBUG(WPARAM wParam, LPARAM lParam) {
	string path = (char*)wParam;
	long line = (long)lParam;

	CTAMGUIView* vuecourante = Lavue(path);
	if (vuecourante == NULL)
		return 0;

	CFrameWnd* pFrame = vuecourante->GetParentFrame();
	if (pFrame != NULL)
		pFrame->ActivateFrame();
	vuecourante->SetFocus();
	vuecourante->SelectionneLigne(line);

	return 1;
}

LRESULT CTAMGUIView::MAJ(WPARAM wParam, LPARAM lParam) {
	SetFocus();
	CFrameWnd* pFrame = GetParentFrame();
	if (pFrame != NULL)
		pFrame->ActivateFrame();
	return 1;
}

void CTAMGUIView::Addundos(CString s, long d, long f) {

	listdos.clear();
	ModifItem md(s, d, f);
	listundos.push_back(md);

	if (listundos.size() > 100) {
		for (int i = 0; i < 10; i++)
			listundos.pop_front();
	}
}


void CTAMGUIView::Keepundos(CString& r) {
	CRichEditCtrl& e = GetRichEditCtrl();
	CString s;

	long d, f;
	e.GetSel(d, f);
	s = e.GetSelText();

	Addundos(s, d, f);
	Addundos(r, d, d + r.GetLength());
}

LRESULT CTAMGUIView::OnFindDialogMessage(WPARAM wParam, LPARAM lParam)
{
	ASSERT(m_pFindDialog != NULL);

	// If the FR_DIALOGTERM flag is set,
	// invalidate the handle identifying the dialog box.
	if (m_pFindDialog->IsTerminating())
	{
		m_pFindDialog = NULL;
		return 0;
	}

	CRichEditCtrl& e = GetRichEditCtrl();
	// If the FR_FINDNEXT flag is set,
	// call the application-defined search routine
	// to search for the requested string.
	if (m_pFindDialog->FindNext())
	{
		//read data from dialog
		findString = m_pFindDialog->GetFindString();
		bMatchCase = m_pFindDialog->MatchCase() == TRUE;
		bMatchWholeWord = m_pFindDialog->MatchWholeWord() == TRUE;
		bSearchDown = m_pFindDialog->SearchDown() == TRUE;
		//with given name do search
		FindText(findString, bMatchCase, bMatchWholeWord, bSearchDown);
	}
	else {
		long l, r;
		BOOL search = TRUE;
		if (m_pFindDialog->ReplaceCurrent()) {
			findString = m_pFindDialog->GetFindString();
			e.GetSel(l, r);
			if (l == r)
				search = FindText(findString, bMatchCase, bMatchWholeWord, bSearchDown);
			if (search) {
				replacementString = m_pFindDialog->GetReplaceString();
				e.HideSelection(TRUE, FALSE);
				Keepundos(replacementString);
				e.ReplaceSel(replacementString);
				e.HideSelection(FALSE, FALSE);
			}
		}
		else
		if (m_pFindDialog->ReplaceAll()) {
			findString = m_pFindDialog->GetFindString();
			bMatchCase = m_pFindDialog->MatchCase() == TRUE;
			bMatchWholeWord = m_pFindDialog->MatchWholeWord() == TRUE;
			bSearchDown = m_pFindDialog->SearchDown() == TRUE;
			replacementString = m_pFindDialog->GetReplaceString();
			e.HideSelection(TRUE, FALSE);
			while (FindText(findString, bMatchCase, bMatchWholeWord, bSearchDown)) {
				Keepundos(replacementString);
				e.ReplaceSel(replacementString);
			}
			e.HideSelection(FALSE, FALSE);
		}
	}
	return 0;
}


BOOL CTAMGUIView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: changez ici la classe ou les styles Window en modifiant
	//  CREATESTRUCT cs

	return CRichEditView::PreCreateWindow(cs);
}

// dessin de CTAMGUIView

void CTAMGUIView::OnDraw(CDC* /*pDC*/)
{

	CTAMGUIDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	// TODO: ajoutez ici le code de dessin pour les données natives
}


// impression de CTAMGUIView


void CTAMGUIView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CTAMGUIView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// préparation par défaut
	return DoPreparePrinting(pInfo);
}

void CTAMGUIView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ajoutez une initialisation supplémentaire avant l'impression
}

void CTAMGUIView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ajoutez un nettoyage après l'impression
}


void CTAMGUIView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	MapInitInfos();
	//ClientToScreen(&point);
	CRichEditCtrl& e = GetRichEditCtrl();
	long g = e.CharFromPos(point);
	int l = e.LineFromChar(g);
	TCHAR sx[1024];
	int ln = e.GetLine(l, sx, 512);
	sx[ln] = 0;
	long deb = e.LineIndex(l);
	g = g - deb;
	//We isolate the string...
	//to the left
	inittableutf8();
	l = g;
	while (l > 0 && sx[l] > 32 && !c_is_punctuation(sx[l])) l--;
	deb = g;
	while (sx[deb] > 32 && !c_is_punctuation(sx[deb])) deb++;
	CString s;
	for (g = l + 1; g < deb; g++)
		s += sx[g];
	if (s.GetLength() != 0) {
		string code;
		fromCString(code, s);
		if (theinfos.find(code) != theinfos.end()) {
			fromstring(s, theinfos[code]);
			AfxMessageBox(s);
		}
	}
}

void CTAMGUIView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

void CleanFromSession(HWND xwnd) {
	CWnd* w32 = CWnd::FromHandle(xwnd);
	if (w32 != NULL)
		w32->DestroyWindow();
}

// diagnostics pour CTAMGUIView

#ifdef _DEBUG
void CTAMGUIView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CTAMGUIView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}

CTAMGUIDoc* CTAMGUIView::GetDocument() const // la version non Debug est inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTAMGUIDoc)));
	return (CTAMGUIDoc*)m_pDocument;
}
#endif //_DEBUG


// gestionnaires de messages pour CTAMGUIView

void CTAMGUIView::Choixligne() {
	if (firstview == this)
		return;
	CTAMGUIView* vuecourante = Lavue(TamguCurrentFilename());
	if (vuecourante == NULL) {
		firstview->currentname = TamguCurrentFilename();
		::SendMessage(firstview->GetSafeHwnd(), ID_OUVRIR_DOCUMENT, 0, 0);
		vuecourante = Lavue(TamguCurrentFilename());
		if (vuecourante == NULL)
			return;
	}

	vuecourante->SetFocus();
	vuecourante->SelectionneLigne(TamguCurrentLine());
}

void CTAMGUIView::Afficheerreur() {
	string lecode = TamguErrorMessage();
	if (lecode.size() == 0)
		return;
	wstring base;
	s_utf8_to_unicode(base, USTR(lecode), lecode.size());
	AfxMessageBox((WCHAR*)base.c_str());
	Choixligne();
}

void CTAMGUIView::Affichage(CString& s, bool thread) {
	if (firstview == NULL)
		return;
	CRichEditCtrl& e = firstview->GetRichEditCtrl();
	s += ENDL;
	int nLength = e.GetWindowTextLength();
	// put the selection at the end of text
	e.SetSel(nLength, nLength);
	// replace the selection
	char* res = NULL;// displaytamgusession();
	long f;
	if (res != NULL && res[0] != 0) {
		f = firstview->derniereposition - 1;
		wstring base;
		s_utf8_to_unicode(base, (unsigned char*)res, 0);
		commande = (WCHAR*)base.c_str();
		e.ReplaceSel(commande);
		::SendMessage(firstview->GetSafeHwnd(), MET_A_JOUR, 0, 0);
	}
}

LRESULT CTAMGUIView::AfficheCourant(WPARAM wParam, LPARAM lParam) {

	CString* s = (CString*)wParam;
	CRichEditCtrl& er = GetRichEditCtrl();
	//int nLength = er.GetWindowTextLength();
	// put the selection at the end of text

	er.SetSel(-1, -1);
	er.ReplaceSel(*s);

	if (firstdisplay) {
		//lexchange.encours = true;
		SetFocus();
		CFrameWnd* pFrame = GetParentFrame();
		if (pFrame != NULL)
			pFrame->ActivateFrame();
		firstdisplay = false;
	}

	delete s;
	return 1;
}

void DisplayValue(string s, void* o) {
	if (firstview == NULL || s.size() == 0)
		return;
	CString* cs = new CString;
	fromstring(*cs, s);
	SendMessage(firstview->GetSafeHwnd(), IDS_AFFICHAGE_COURANT, (WPARAM)cs, 0);
}

void Displaystring(CString& s) {
	if (firstview == NULL || s.GetLength() == 0)
		return;
	CString* cs = new CString(s);
	SendMessage(firstview->GetSafeHwnd(), IDS_AFFICHAGE_COURANT, (WPARAM)cs, 0);
}


LRESULT CTAMGUIView::ouvrir(WPARAM wParam, LPARAM lParam) {
	wstring code;
	s_utf8_to_unicode(code, USTR(currentname), currentname.size());
	if (code.size() == 0)
		return 0;
	CTAMGUIDoc* doc = (CTAMGUIDoc*)theApp.OpenDocumentFile((WCHAR*)code.c_str());
	return 1;
}

LRESULT CTAMGUIView::afficher(WPARAM wParam, LPARAM lParam) {
	if (wParam == 1)
		Afficheerreur();
	else
		Affichage(lexchange.s, true);
	return 1;
}

void executetamgu(echange& e) {
	if (!TamguRun(e.idcode))
		SendMessage(e.vue, ID_AFFICHER_ERREUR, (WPARAM)1, 0);
	else
		SendMessage(e.vue, ID_AFFICHER_ERREUR, (WPARAM)0, 0);
	e.encours = false;
	affichecursor = 2;
}

void executetamguinthread(void* arg) {
	echange* e = (echange*)arg;
	if (!TamguRun(e->idcode))
		SendMessage(e->vue, ID_AFFICHER_ERREUR, (WPARAM)1, 0);
	else
		SendMessage(e->vue, ID_AFFICHER_ERREUR, (WPARAM)0, 0);

	e->encours = false;
	affichecursor = 2;

	_endthread();
}

void CTAMGUIView::runtamgu(CString& s) {
	if (lexchange.encours)
		return;
	if (s != "") {
		if (!TamguLaunching())
			return;

		TamguDisplayFunction(DisplayValue, NULL);
		//ResetGetEditor();

		firstview->firstdisplay = true;
		string lecode;
		wchar_t* base = WCSTR(s);
		s_unicode_to_utf8(lecode, base, s.GetLength());

		string args;
		wchar_t* baseargs = WCSTR(basearguments);
		s_unicode_to_utf8(args, baseargs, basearguments.GetLength());
		TamguDisplayFunction(DisplayValue, NULL);

		TamguSetArguments(args);

		int i = TamguCompile(lecode, stitre);
		if (i == -1) {
			Afficheerreur();
			lexchange.encours = false;
			return;
		}

		if (lexchange.compilation) {
			lexchange.compilation = false;
			lexchange.encours = false;
			return;
		}

		lexchange.idcode = i;
		affichecursor = 1;
		lexchange.initialisation(this, s);
		//if (windowmode() && lexchange.debugging==false)
		//	executetamgu(lexchange);
		//else
		_beginthread(executetamguinthread, 0, (void*)&lexchange);
	}
}

void CTAMGUIView::AfficheAnalyse() {
	if (firstview == NULL)
		return;
	char* res = NULL;// displaytamgusession();
	if (res != NULL && res[0] != 0) {
		CRichEditCtrl& e = firstview->GetRichEditCtrl();
		wstring base;
		s_utf8_to_unicode(base, (unsigned char*)res, 0);
		commande = (WCHAR*)base.c_str();
		e.ReplaceSel(commande);
	}
}

void CTAMGUIView::executelecode() {
	if (lexchange.encours)
		return;
	if (accumulecode != "") {
		TamguLaunching();

		firstview->firstdisplay = true;
		//char* code=wcstombs( pmbhello, pwchello, MB_CUR_MAX );
		string lecode;
		wchar_t* base = WCSTR(accumulecode);
		s_unicode_to_utf8(lecode, base, accumulecode.GetLength());

		string args;
		wchar_t* baseargs = WCSTR(basearguments);
		s_unicode_to_utf8(args, baseargs, basearguments.GetLength());

		string tamguname = stitre;
		bool dsp = false;
		if (tamguname == "")
			tamguname = "Console";

		if (firstview == this)
			dsp = true;

		TamguDisplayFunction(DisplayValue, NULL);

		TamguSetArguments(args);

		int i = TamguCompile(lecode, tamguname, dsp);

		if (i != -1) {
			lexchange.idcode = i;
			affichecursor = 1;
			//DWORD tid;
			lexchange.initialisation(this, accumulecode);
			_beginthread(executetamguinthread, 0, (void*)&lexchange);
		}
		else
			Afficheerreur();
		accumulecode = "";
		nbcurly = 0;
		nbsquare = 0;
	}
}
bool CTAMGUIView::Couleur(int r, int g, int b) {
	if (begincolor) {
		CHARFORMAT2 cfbis = GetCharFormatSelection();
		if (cfbis.crTextColor == RGB(r, g, b))
			return false;
	}

	if (changepolice)
		_tcscpy(cf.szFaceName, police);

	cf.yHeight = tailleHauteur;

	cf.dwEffects &= ~CFE_ITALIC;
	cf.crTextColor = RGB(r, g, b);

	GetRichEditCtrl().SetSelectionCharFormat(cf);
	return true;
}


void CTAMGUIView::OnInitialUpdate() {
	CRichEditView::OnInitialUpdate();
	CRichEditCtrl& e = GetRichEditCtrl();
	cf = GetCharFormatSelection();
	initcf = true;
	cf.dwMask |= CFM_COLOR;
	cf.dwEffects &= ~CFE_AUTOCOLOR;
	cf.cbSize = sizeof(CHARFORMAT2);

	cf.dwMask |= CFM_BOLD;
	cf.dwEffects &= ~CFE_BOLD;

	cf.yHeight = tailleHauteur;
	_tcscpy(cf.szFaceName, police);

	e.SetDefaultCharFormat(cf);
	if (firstview != this)
		e.SetTargetDevice(NULL, 1);
}

void CTAMGUIView::Italique(bool add) {
	CHARFORMAT2 cfbis = GetCharFormatSelection();
	cfbis.dwMask &= ~CFM_COLOR;
	cfbis.dwEffects |= CFE_AUTOCOLOR;
	if (add)
		cfbis.dwEffects |= CFE_ITALIC;
	else
		cfbis.dwEffects &= ~CFE_ITALIC;

	GetRichEditCtrl().SetSelectionCharFormat(cfbis);
}

void CTAMGUIView::colore1000() {

	begincolor = true;
	bnfxs.stackln.erase(bnfxs.stackln.begin() + 5000, bnfxs.stackln.end());
	bnfxs.stacksize.erase(bnfxs.stacksize.begin() + 5000, bnfxs.stacksize.end());
	bnfxs.stacktype.erase(bnfxs.stacktype.begin() + 5000, bnfxs.stacktype.end());

	stacktype.clear();
	CRichEditCtrl& e = GetRichEditCtrl();
	long mlx = bnfxs.stackln.back();
	mlx = e.LineFromChar(mlx);
	mlx = e.LineFromChar(e.LineIndex(mlx));
	long fin = e.GetTextLength() - 1;
	fin = e.LineFromChar(fin);

	for (int i = 0; i < fin; i++) {
		if (i < mlx)
			stacktype.push_back(0);
		else
			stacktype.push_back(1);
	}

	wstring ws;
	ApplyColor(ws, 0, -1, false);
}

void CTAMGUIView::colorisation() {
	if (!begincolor)
		return;

	int i = 0;
	CRichEditCtrl& e = GetRichEditCtrl();

	long debut = e.GetFirstVisibleLine();
	if (debut >= stacktype.size())
		return;

	int linenumber = debut;
	long d, f;
	e.GetSel(d, f);
	if (d == f) {
		d = e.LineIndex(linenumber);
		f = d;
	}

	long fin = e.GetTextLength() - 1;
	fin = e.LineFromChar(fin);
	int nb = min(debut + 120, stacktype.size());
	if (fin < stacktype.size()) {
		stacktype.erase(stacktype.begin() + debut, stacktype.end());
		for (i = debut; i < fin; i++)
			stacktype.push_back(1);
	}
	else {
		for (i = debut; i < nb; i++) {
			if (stacktype[i] == 1) {
				debut = i;
				break;
			}
		}
		if (debut != i)
			return;
	}

	fin = nb;
	for (i = debut; i < nb; i++) {
		if (stacktype[i] == 0) {
			fin = i;
			break;
		}
	}

	fin = e.LineIndex(fin);
	debut = e.LineIndex(debut);
	CString txt;

	if (fin == -1 || fin >= e.GetTextLength() - 1)
		fin = e.GetTextLength() - 1;

	e.GetTextRange(debut, fin, txt);
	wstring ws = LPCWSTR(txt);

	SetRedraw(FALSE);
	bool toupdate = ApplyColor(ws, debut, fin);
	SetRedraw(TRUE);

	for (i = linenumber; i < nb; i++)
		stacktype[i] = 0;

	if (toupdate) {
		Invalidate();
		UpdateWindow();
	}
	e.SetSel(d, f);
	for (i = 0; i < stacktype.size(); i++) {
		if (stacktype[i] == 1)
			return;
	}
	begincolor = false;
}


bool CTAMGUIView::ApplyColor(wstring& sw, int offset, int mxpos, bool rebuilt) {
	if (rebuilt)
		bnfxs.tokenize(sw);

	if (bnfxs.stacktype.size() >= 10000) {
		colore1000();
		return false;
	}

	CRichEditCtrl& e = GetRichEditCtrl();
	CHARRANGE positions;
	int nb = bnfxs.stacktype.size();
	size_t istack;
	bool colorized = false;
	for (istack = 0; istack < nb; istack++) {
		switch (bnfxs.stacktype[istack]) {
		case 1:
			positions.cpMin = bnfxs.stackln[istack] + offset - 1;
			positions.cpMax = positions.cpMin + bnfxs.stacksize[istack];
			if (mxpos != -1 && positions.cpMax>mxpos)
				break;
			e.SetSel(positions);
			if (Couleur(255, 0, 0))
				colorized = true;
			break;
		case 2:
			positions.cpMin = bnfxs.stackln[istack] + offset - 1;
			positions.cpMax = positions.cpMin + bnfxs.stacksize[istack];
			if (mxpos != -1 && positions.cpMax > mxpos)
				break;
			e.SetSel(positions);
			if (Couleur(150, 150, 250))
				colorized = true;
			break;
		case 10: //string between @"..."@
			positions.cpMin = bnfxs.stackln[istack] + offset - 1;
			positions.cpMax = positions.cpMin + bnfxs.stacksize[istack];
			if (mxpos != -1 && positions.cpMax > mxpos)
				break;
			e.SetSel(positions);
			if (Couleur(150, 150, 150))
				colorized = true;
			break;
		case 3: //keywords in colorkeywords
			positions.cpMin = bnfxs.stackln[istack] + offset - 1;
			positions.cpMax = positions.cpMin + bnfxs.stacksize[istack];
			if (mxpos != -1 && positions.cpMax > mxpos)
				break;
			e.SetSel(positions);
			if (Couleur(0, 0, 255))
				colorized = true;
			break;
		case 4: //regular keyword in context
			//we skip if the file size is too large...
			if (istack<bnfxs.stacktype.size() - 1) {//regular keyword preceded either by a: '[' or '(' (previous code is 7) or a '.' (previous code is 6)
				if (istack>0 && bnfxs.stacktype[istack - 1] == 6 && bnfxs.stacktype[istack + 1] == 7) {
					positions.cpMin = bnfxs.stackln[istack] + offset - 1;
					positions.cpMax = positions.cpMin + bnfxs.stacksize[istack];
					if (mxpos != -1 && positions.cpMax > mxpos)
						break;
					e.SetSel(positions);
					if (Couleur(160, 100, 10))
						colorized = true;
				}
				else
				if (bnfxs.stacktype[istack + 1] == 7) {//regular keyword followed with: '[' or '('
					positions.cpMin = bnfxs.stackln[istack] + offset - 1;
					positions.cpMax = positions.cpMin + bnfxs.stacksize[istack];
					if (mxpos != -1 && positions.cpMax > mxpos)
						break;
					e.SetSel(positions);
					if (Couleur(140, 0, 160))
						colorized = true;
				}
			}
			break;
		case 5://comments
			positions.cpMin = bnfxs.stackln[istack] + offset - 1;
			positions.cpMax = positions.cpMin + bnfxs.stacksize[istack];
			if (mxpos != -1 && positions.cpMax > mxpos)
				break;
			e.SetSel(positions);
			if (Couleur(85, 180, 85))
				colorized = true;
			break;
		case 8:// '#' or '$'
			positions.cpMin = bnfxs.stackln[istack] + offset - 1;
			positions.cpMax = positions.cpMin + bnfxs.stacksize[istack];
			if (mxpos != -1 && positions.cpMax > mxpos)
				break;
			e.SetSel(positions);
			if (Couleur(255, 120, 120))
				colorized = true;
			break;
		case 9:// '?'
			positions.cpMin = bnfxs.stackln[istack] + offset - 1;
			positions.cpMax = positions.cpMin + bnfxs.stacksize[istack];
			if (mxpos != -1 && positions.cpMax > mxpos)
				break;
			e.SetSel(positions);
			if (Couleur(160, 100, 10))
				colorized = true;
			break;
		}
	}
	m_bSyncCharFormat = TRUE;
	m_bSyncParaFormat = TRUE;
	return colorized;
}

void CTAMGUIView::Colorie(long d, long f, int nFirstVisible) {
	if (firstview == this)
		return;

	firstview->SetFocus();
	BOOL modif = GetDocument()->IsModified();
	CRichEditCtrl& e = GetRichEditCtrl();
	e.SetUndoLimit(0);

	if (nFirstVisible == -1) {
		nFirstVisible = e.GetFirstVisibleLine();
		e.GetSel(d, f);
	}
	long longueur = e.GetWindowTextLength();

	LockWindowUpdate();
	e.HideSelection(TRUE, FALSE);
	SetRedraw(FALSE);

	e.SetSel(0, longueur);
	Couleur(0, 0, 0);
	dernierelongueur = longueur;
	CString s;
	GetWindowText(s);

	wstring ws = WCSTR(s);
	ws = s_replacestring(ws, L"\r\n", L"\r");
	longueur = ws.size();

	CWaitCursor wait;
	ApplyColor(ws, 0, -1);

	for (int i = 0; i < thebreaks.size(); i++) {
		if (thebreaks[i] == this) {
			s.Replace(L"\r\n", L"\n");
			long pos = 0;
			TCHAR c = XCAR;
			while (pos != -1) {
				pos = s.Find(c, pos);
				if (pos != -1) {
					long d = e.LineIndex(e.LineFromChar(pos));
					long f = e.LineIndex(e.LineFromChar(pos) + 1);
					e.SetSel(d, f);
					Italique(true);
					pos++;
				}
			}
			break;
		}
	}

	GetDocument()->SetModifiedFlag(modif);
	SetFocus();

	e.SetSel(0, 0);
	if (nFirstVisible > 0)
		e.LineScroll(nFirstVisible, 0);
	e.SetSel(d, f);

	e.HideSelection(FALSE, FALSE);
	SetRedraw(TRUE);
	UnlockWindowUpdate();
	Invalidate();
	UpdateWindow();
}

void CTAMGUIView::OnKifExecuter()
{
	//// TODO: Add your command handler code here
	long d, f;
	CString s;
	TCHAR sx[1024];

	if (TamguRunning())
		return;

	if (firstview == this) {
		CRichEditCtrl& e = GetRichEditCtrl();
		s = e.GetSelText();
		s = s.Trim();
		if (s == "") {
			e.GetSel(d, f);
			int l = e.LineFromChar(d) - 1;
			int ln = e.GetLine(l, sx, 512);
			sx[ln] = 0;
			s = sx;
			s = s.Trim();
		}
	}
	else
		GetWindowText(s);


	runtamgu(s);
}


void CTAMGUIView::OnKifSansinteruption()
{
	// TODO: Add your command handler code here
	if (!TamguExtinguish())
		return;

	if (firstview != NULL) {
		CRichEditCtrl& e = firstview->GetRichEditCtrl();
		e.SetSel(0, -1);
		firstview->SetWindowText(_T(""));
	}
	lexchange.debugging = false;
	OnKifExecuter();
}


void CTAMGUIView::OnKifReexecuter()
{
	// TODO: Add your command handler code here
	if (!TamguExtinguish())
		return;
	CString txt;
	TCHAR c = XCAR;
	lexchange.debugging = false;
	for (int i = 0; i < thebreaks.size(); i++) {
		CRichEditCtrl& e = thebreaks[i]->GetRichEditCtrl();
		string titre = thebreaks[i]->stitre;
		e.GetWindowText(txt);
		txt.Replace(L"\r\n", L"\n");
		long pos = 0;
		while (pos != -1) {
			pos = txt.Find(c, pos);
			if (pos != -1) {
				//ajoutebreak(STR(titre), e.LineFromChar(pos) + 1);
				lexchange.debugging = true;
				pos++;
			}
		}
	}
	OnKifExecuter();
}

void Launchtamgu() {
	_beginthread(executetamguinthread, 0, (void*)&lexchange);
}


void debuggage(void* arg) {
	CTAMGUIView* view = (CTAMGUIView*)arg;

	TamguDisplayFunction(NULL, NULL);

	debogueur deb;
	deb.oldos = globalTamgu->os;
	globalTamgu->os = new stringstream;

	globalTamgu->globalLOCK = true;
	globalTamgu->Setdebugmode(true);
	globalTamgu->Setdebugfunction(Debug_callback, &deb);
	if (loquet != NULL)
		delete loquet;
	loquet = new LockedThread;

	CString txt;
	TCHAR c = XCAR;
	for (int i = 0; i < thebreaks.size(); i++) {
		CRichEditCtrl& e = thebreaks[i]->GetRichEditCtrl();
		string titre = thebreaks[i]->stitre;
		e.GetWindowText(txt);
		txt.Replace(L"\r\n", L"\n");
		long pos = 0;
		while (pos != -1) {
			pos = txt.Find(c, pos);
			if (pos != -1) {
				deb.breakpoints[titre][e.LineFromChar(pos) + 1] = true;
				pos++;
			}
		}
	}
	deb.DoModal();
	firstview->PostMessageW(UPDATE_END_OF_PROGRAM, 0, 0);
}

void CTAMGUIView::OnKifPasaPas() {
	long d, f;
	CString s;
	TCHAR sx[1024];

	if (TamguRunning())
		return;

	if (firstview == this) {
		CRichEditCtrl& e = GetRichEditCtrl();
		s = e.GetSelText();
		s = s.Trim();
		if (s == "") {
			e.GetSel(d, f);
			int l = e.LineFromChar(d) - 1;
			int ln = e.GetLine(l, sx, 512);
			sx[ln] = 0;
			s = sx;
			s = s.Trim();
		}
	}
	else
		GetWindowText(s);

	if (!TamguLaunching()) {
		TamguExtinguish();
		TamguLaunching();
	}

	TamguDisplayFunction(DisplayValue, NULL);
	firstview->firstdisplay = true;
	string lecode;
	wchar_t* base = WCSTR(s);
	s_unicode_to_utf8(lecode, base, s.GetLength());

	string args;
	wchar_t* baseargs = WCSTR(basearguments);
	s_unicode_to_utf8(args, baseargs, basearguments.GetLength());
	TamguDisplayFunction(DisplayValue, NULL);

	TamguSetArguments(args);

	int i = TamguCompile(lecode, stitre);
	if (i == -1) {
		Afficheerreur();
		lexchange.encours = false;
		return;
	}

	if (lexchange.compilation) {
		lexchange.compilation = false;
		lexchange.encours = false;
		return;
	}

	lexchange.idcode = i;
	affichecursor = 1;
	lexchange.initialisation(this, s);

	_beginthread(debuggage, 0, (void*)this);
}



void CTAMGUIView::OnKifLister()
{
	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();

	string res = TamguListing();
	wstring base;
	s_utf8_to_unicode(base, USTR(res), res.size());
	commande = (WCHAR*)base.c_str();
	e.ReplaceSel(commande);
	derniereposition = e.GetWindowTextLength();
	e.SetSel(derniereposition, derniereposition);
	Colorie();
}


void CTAMGUIView::OnKifRaz()
{
	// TODO: Add your command handler code here
	TamguExtinguish();
	accumulecode = "";
	nbcurly = 0;
	nbsquare = 0;
}

CString CTAMGUIView::GetLineText(long l) {
	CRichEditCtrl& e = GetRichEditCtrl();
	TCHAR sx[1024];
	int ln = e.GetLine(l, sx, 512);
	sx[ln] = 0;
	return sx;
}


void CTAMGUIView::SelectionneLigne(long l) {
	CRichEditCtrl& e = GetRichEditCtrl();
	long d = e.LineIndex(l - 1);
	long f = d + e.LineLength(d);
	e.SetSel(d, f);
}

void CTAMGUIView::Chemin(CString titre) {
	wchar_t* base = WCSTR(titre);
	string ti;
	inittableutf8();
	s_unicode_to_utf8(ti, base, titre.GetLength());
	if (stitre == "") {
		stitre = s_to_lower(ti);
		lesvues[stitre] = this;
	}
	else {
		if (stitre != ti) {
			lesvues.erase(ti);
			stitre = s_to_lower(ti);
			lesvues[stitre] = this;
		}
	}
}

void CTAMGUIView::comptebrackets(CString& s) {
	char instr = 0;
	for (int i = 0; i < s.GetLength(); i++) {
		if (s[i] == '\'') {
			if (instr == 0)
				instr = 1;
			else
			if (instr == 1)
				instr = 0;
			continue;
		}

		if (s[i] == '"') {
			if (instr == 0)
				instr = 2;
			else
			if (instr == 2) {
				if (s[i - 1] != '\\')
					instr = 0;
			}
			continue;
		}

		if (instr)
			continue;

		switch (s[i]) {
		case '{':
			nbcurly++;
			break;
		case '}':
			nbcurly--;
			break;
		case '[':
			nbsquare++;
			break;
		case ']':
			nbsquare--;
			break;
		}
	}
}


bool Teststring(CString& s, const wchar_t* ch, wchar_t fin) {
	bool tr = true;
	int i = 0;
	int ln = wcslen(ch);
	for (; i < ln; i++) {
		if (s[i] != ch[i])
			return false;
	}
	if (s[i] <= 32 || s[i] == fin || s[i] == '/')
		return true;
	return false;
}

static bool testKifKeyword(wchar_t* wcom) {
	static std::wregex* pattern = NULL;

	if (pattern == NULL) {
		string keywords("\\b");
		keywords += "for";
		keywords += " *\\(|\\b";
		keywords += "while";
		keywords += " *\\(|\\b";
		keywords += "switch";
		keywords += " *\\(|\\b";
		keywords += "if";
		keywords += " *\\(";
		wstring reg;
		s_utf8_to_unicode(reg, USTR(keywords), keywords.size());
		pattern = new std::wregex(reg);
	}

	return std::regex_search(wcom, *pattern);
}

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
void CTAMGUIView::CouleurDepuisDebutLigne(long d, long f) {
	//On recupere la position
	CRichEditCtrl& e = GetRichEditCtrl();
	long l = e.LineFromChar(d);
	//Puis le debut de cette ligne...
	long debut = e.LineIndex(l);
	if (f - debut > 0) {
		CString prev;
		e.GetTextRange(debut, f, prev);

		e.HideSelection(TRUE, FALSE);

		//Puis les caracteres de la ligne que l'on transforme
		wstring xs = LPCWSTR(prev);
		e.SetSel(debut, f);
		Couleur(0, 0, 0);
		ApplyColor(xs, debut, f);
		e.SetSel(d, f);

		e.HideSelection(FALSE, FALSE);
	}
}

//--------------------------------------------------------------------------------------------------------
afx_msg void CTAMGUIView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {
	listdos.clear();
	if (nChar < 32 && nChar != 9)
		return CWnd::OnChar(nChar, nRepCnt, nFlags);

	CRichEditCtrl& e = GetRichEditCtrl();
	long D, F;
	e.GetSel(D, F);
	CString prev;

	if (nChar == '.' && suggestion) {
		e.GetTextRange(0, D, prev);
		wstring wtxt = WCSTR(prev);
		int letype;
		if (FindDeclarations(wtxt, letype)) {
			PostMessage(MET_SUGGERE_METHODE, letype, F);
			return;
		}
		prev = "";
	}

	if (nChar == 125) {
		int l = e.LineFromChar(F);
		TCHAR sx[1024];
		e.LineFromChar(D);
		int ln = e.GetLine(l, sx, 512);
		sx[ln] = 0;
		CString sn(sx);
		sx[ln - 1] = 0;
		prev = sx;
		sn.Trim();
		if (!sn.GetLength() && ln > 4) {
			long firstline = FindCorrespondance(D, F, 1);
			if (firstline != -1) {
				int firstlength = e.GetLine(firstline, sx, 512);
				if (firstlength > 0) {
					sx[firstlength] = 0;
					CString sfirst(sx);
					sfirst.TrimLeft();
					ln = firstlength - sfirst.GetLength();
				}
				else
					ln = 0;

				while (ln > 0) {
					sn += " ";
					ln--;
				}

				long d = e.LineIndex(l);
				long f = d + e.LineLength(d);

				Addundos(prev, d, f);

				sn += "}";
				e.SetSel(d, f);
				e.ReplaceSel(sn);

				f = d + sn.GetLength();
				Addundos(sn, d, f);
				return;
			}
		}
	}

	if (D != F)
		e.GetTextRange(D, F, prev);
	else
		CouleurDepuisDebutLigne(D, F);

	CWnd::OnChar(nChar, nRepCnt, nFlags);
	Addundos(prev, D, F);
	char n[] = { nChar, 0 };
	CString s(n);
	Addundos(s, D, D + 1);
}
//--------------------------------------------------------------------------------------------------------

BOOL CTAMGUIView::PreTranslateMessage(MSG* pMsg) {
	static long lastd;

	long d, f;

	CRichEditCtrl& e = GetRichEditCtrl();
	e.GetSel(d, f);
	int l;


	l = e.LineFromChar(f);
	char ch[20];
	sprintf_s(ch, 20, "%d", l + 1);
	commande = ch;
	leMain->AfficheLigne(commande);

	if (pMsg->message != 256 && pMsg->message != 258 && dernierelongueur) {
		if (pMsg->wParam != VK_OEM_PERIOD && pMsg->wParam != VK_BACK && pMsg->wParam != VK_DELETE && pMsg->wParam != VK_RETURN) {
			colorisation();
			return CRichEditView::PreTranslateMessage(pMsg);
		}
	}

	int longueur = e.GetWindowTextLength();
	if (!longueur) {
		nbcurly = 0;
		nbsquare = 0;
		accumulecode = "";
	}

	if (pMsg->message == 256) {
		CString prev;
		if (pMsg->wParam == VK_BACK) {
			if (d || d != f) {
				if (d != f)
					e.GetTextRange(d, f, prev);
				else
					e.GetTextRange(--d, f, prev);
				Addundos(prev, d, f);
				Addundos(_T(""), d, d);
			}
		}
		else
		if (pMsg->wParam == VK_DELETE) {
			if (f < longueur || d != f) {
				if (d == f)
					e.GetTextRange(d, ++f, prev);
				else {
					e.GetTextRange(d, f, prev);
					//In the case of partial chunk removal from the editor, the remaining is replaced by the system with a CR
					//In that case, we must remove it, unless we started our deletion from the beginning of the line...
					if (d && prev.Right(1) == 13) {
						//first we check, if it is the beginning of the line...
						CString lcar;
						e.GetTextRange(d - 1, d, lcar);
						if (lcar[0] != 13) //it is not the case, we need to remove the trailing CR from the string
							e.GetTextRange(d, --f, prev);
					}
				}
				Addundos(prev, d, f);
				Addundos(_T(""), d, d);
			}
		}
		else
		if (pMsg->wParam == VK_RETURN) {
			if (firstview != this) {
				//we record this last carriage return for further treatment
				//we are in the pre-handling of the carriage return since pMsg->message==256
				prev = e.GetSelText();
				Addundos(prev, d, f);
				lastd = d;
				if (stacktype.size() != 0) {
					l = e.LineFromChar(d);
					stacktype.insert(stacktype.begin() + l, 0);
				}
			}
		}
	}

	if (longueur != dernierelongueur) {
		if (dernierelongueur == 0 || (pMsg->message == 258 && pMsg->wParam == VK_RETURN)) {
			if (dernierelongueur == 0 && firstview != this)
				Colorie();
			if (pMsg->message == 258 && pMsg->wParam == VK_RETURN) {
				TCHAR sx[1024];

				l = e.LineFromChar(d) - 1;
				int ln = e.GetLine(l, sx, 512);
				sx[ln] = 0;
				int sxlen = ln;
				bool exec = false;
				if (firstview == this) {
					commande = sx;
					commande = commande.TrimRight();
					if (commande != "") {
						if (accumulecode != "")
							accumulecode += "\n";
						accumulecode += commande;
						wchar_t lst = commande[commande.GetLength() - 1];
						switch (lst) {
						case ',':
						case ':':
							break;
						case ')':
							if (testKifKeyword(WCSTR(commande)) == true)
								break;
						default:
							comptebrackets(commande);
							if (nbcurly == 0 && nbsquare == 0)
								exec = true;
						}
					}
					else {
						nbcurly = 0;
						nbsquare = 0;
						accumulecode = "";
						dernierelongueur = longueur;
						return CRichEditView::PreTranslateMessage(pMsg);
					}
				}

				//We get the trailing white chars from command
				e.HideSelection(TRUE, FALSE);

				wstring xs(sx);
				xs += '\r';
				CString selse(sx);
				selse.Trim();
				int lnw;
				bool belse = false;
				commande = "";
				if (Teststring(selse, L"else", L'{') || Teststring(selse, L"elif", L'{')) {
					//we push up to previous "if"
					lnw = FirstEnd(d, f);
					belse = true;
					if (lnw != -1) {
						int firstlength = e.GetLine(lnw, sx, 512) - 1;
						if (firstlength > 0) {
							sx[firstlength] = 0;
							CString sfirst(sx);
							sfirst.TrimLeft();
							lnw = firstlength - sfirst.GetLength();
							while (lnw > 0) {
								commande += " ";
								lnw--;
							}
						}
					}
				}
				else {
					lnw = FirstChar(d, f, commande);
					while (lnw > 0) {
						commande += " ";
						lnw--;
					}
				}

				if (belse) {
					d = e.LineIndex(l);
					e.SetSel(d, f);
					selse.Insert(0, commande);
					selse += "\r";
					selse += commande;
					lnw = GetBlankSize();
					while (lnw > 0) {
						selse += " ";
						lnw--;
					}
					commande = selse;
					xs = commande;
				}

				if (commande != "")
					e.ReplaceSel(commande);


				commande = _T("\r") + commande;
				Addundos(commande, lastd, lastd + commande.GetLength());

				e.GetSel(d, f);
				long dx = e.LineIndex(l);
				e.SetSel(dx, d);
				bool bg = begincolor;
				begincolor = false;
				Couleur(0, 0, 0);
				ApplyColor(xs, dx);
				e.SetSel(d, f);
				begincolor = bg;

				e.HideSelection(FALSE, FALSE);

				if (exec) {
					if (!lexchange.encours) {
						accumulecode += "\n";
						executelecode();
					}
					else {
						if (accumulecode != "") {
							nbcurly = 0;
							nbsquare = 0;
							accumulecode = _T("");
							DisplayValue("Cannot execute instructions\r", NULL);
						}
					}
				}
			}
		}
		dernierelongueur = longueur;
	}
	return CRichEditView::PreTranslateMessage(pMsg);
}

extern CTAMGUIApp theApp;
//DWORD WINAPI debuggage(LPVOID arg) {
void CTAMGUIView::OnKifPointarret()
{
	// TODO: Add your command handler code here

	if (firstview != this) {
		CRichEditCtrl& e = GetRichEditCtrl();

		bool fnd = false;
		for (int i = 0; i < thebreaks.size(); i++) {
			if (thebreaks[i] == this) {
				fnd = true;
				break;
			}
		}
		if (!fnd)
			thebreaks.push_back(this);

		long d, f;
		TCHAR c = XCAR;
		CString sub;
		e.GetSel(d, f);
		long l = e.LineFromChar(d);
		d = e.LineIndex(l);
		f = e.LineIndex(l + 1);
		e.SetSel(d, f);
		sub = e.GetSelText();
		if (sub.Find(c) != -1) {
			sub.Remove(c);
			e.ReplaceSel(sub);
			e.SetSel(d, f - 1);
			Italique(false);
		}
		else {
			e.SetSel(d, d);
			sub = c;
			e.ReplaceSel(sub);
			e.SetSel(d, f - 1);
			Italique(true);
		}
		e.SetSel(d, d);
	}
}

void CTAMGUIView::Pointarret(long l) {

	bool fnd = false;
	for (int i = 0; i < thebreaks.size(); i++) {
		if (thebreaks[i] == this) {
			fnd = true;
			break;
		}
	}
	if (!fnd)
		thebreaks.push_back(this);

	CRichEditCtrl& e = GetRichEditCtrl();
	TCHAR c = XCAR;
	long d = e.LineIndex(l);
	long f = e.LineIndex(l + 1);
	e.SetSel(d, f);
	CString sub = e.GetSelText();
	if (sub.Find(c) != -1) {
		sub.Remove(c);
		e.ReplaceSel(sub);
		e.SetSel(d, f - 1);
		Italique(false);
	}
	else {
		e.SetSel(d, d);
		sub = c;
		e.ReplaceSel(sub);
		e.SetSel(d, f - 1);
		Italique(true);
	}
	e.SetSel(d, d);
}

void CTAMGUIView::OnKifNettoyerp()
{
	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();
	long d, f;
	e.GetSel(d, f);
	TCHAR c = XCAR;
	CString txt;
	e.GetWindowText(txt);
	if (txt.Find(c)) {
		txt.Remove(c);
		e.SetWindowText(txt);
		Colorie();
		//nettoyerpointarrets();
	}
	thebreaks.clear();
	e.SetSel(d, f);
}

void CTAMGUIView::OnKifIndenter()
{
	// TODO: Add your command handler code here
	if (firstview != this) {
		CRichEditCtrl& e = GetRichEditCtrl();
		int nFirstVisible = e.GetFirstVisibleLine();
		long d, f;
		e.GetSel(d, f);
		CString s;
		CString sinit;
		string blanc;
		if (d != f)
			e.SetSel(d, d);

		GetWindowText(sinit);

		vector<long> breaks;
		int i;
		TCHAR c = XCAR;
		for (i = 0; i < thebreaks.size(); i++) {
			if (thebreaks[i] == this) {
				s = sinit;
				s.Replace(L"\r\n", L"\n");
				long pos = 0;
				while (pos != -1) {
					pos = s.Find(c, pos);
					if (pos != -1) {
						breaks.push_back(e.LineFromChar(pos));
						pos++;
					}
				}
				sinit.Remove(c);
				break;
			}
		}

		string ti;
		fromCString(ti, sinit);
		string res = TamguIndentation(ti, blanc);
		fromstring(s, res);

		SetRedraw(FALSE);

		if (s != sinit) {
			Addundos(sinit, 0, sinit.GetLength());
			SetWindowText(s);
			if (breaks.size()) {
				sinit = c;
				for (i = breaks.size() - 1; i >= 0; i--) {
					long d = e.LineIndex(breaks[i]);
					e.SetSel(d, d);
					e.ReplaceSel(sinit);
				}
			}

			Colorie(d, f, nFirstVisible);
			Addundos(s, 0, s.GetLength());
		}

		e.SetSel(f, f);
		SetRedraw(TRUE);
		GetDocument()->SetModifiedFlag(TRUE);
	}
}


void CTAMGUIView::OnEditionCherche()
{
	// TODO: Add your command handler code here
	m_pFindDialog = new CFindReplaceDialog();
	CString s(findString);
	findMode = true;
	m_pFindDialog->Create(findMode, s, NULL, FR_DOWN, this);
}



void CTAMGUIView::OnEditionRemplace()
{
	// TODO: Add your command handler code here
	m_pFindDialog = new CFindReplaceDialog();
	CString s(findString);
	CString sr(replacementString);
	findMode = false;
	m_pFindDialog->Create(findMode, s, sr, FR_DOWN, this);
}




void CTAMGUIView::OnKifCommenter()
{
	if (firstview == this)
		return;

	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();
	long d, f;
	e.GetSel(d, f);
	commande = e.GetSelText();
	CString res;
	bool blanc = true;
	char uncommente = 0;
	for (int i = 0; i < commande.GetLength(); i++) {
		if (commande[i] == '\n' || commande[i] == '\r') {
			blanc = true;
			res += commande[i];
		}
		else {
			if (blanc) {
				if (commande[i] <= 32) {
					res += commande[i];
					continue;
				}
				if (commande[i] == '/' && commande[i + 1] == '/') {
					if (uncommente != 1) {
						blanc = false;
						uncommente = 2;
						i++;
						continue;
					}
					else
						res += "//";
				}
				else
				if (uncommente != 2) {
					uncommente = 1;
					res += "//";
				}
			}
			blanc = false;
			res += commande[i];
		}
	}


	wchar_t* base = WCSTR(res);
	string ti;
	s_unicode_to_utf8(ti, base, res.GetLength());
	string ires = TamguIndentation(ti, "");
	wstring idt;
	s_utf8_to_unicode(idt, USTR(ires), ires.size());

	res = (WCHAR*)idt.c_str();
	e.ReplaceSel(res);
	e.SetSel(f, f);
	Addundos(commande, d, f);
	Addundos(res, d, d + res.GetLength());
	Colorie(-1, -1, -1);
	GetDocument()->SetModifiedFlag(TRUE);
}

static void stoppingprocess(void* arg) {
	TamguStop();
	_endthread();
}

void CTAMGUIView::OnKifBreak()
{
	_beginthread(stoppingprocess, 0, 0);
}


void CTAMGUIView::OnKifAsThread()
{
	// TODO: Add your command handler code here
	if (!TamguExtinguish())
		return;
	CString txt;
	TCHAR c = XCAR;
	lexchange.debugging = false;
	for (int i = 0; i < thebreaks.size(); i++) {
		CRichEditCtrl& e = thebreaks[i]->GetRichEditCtrl();
		string titre = thebreaks[i]->stitre;
		e.GetWindowText(txt);
		txt.Replace(L"\r\n", L"\n");
		long pos = 0;
		while (pos != -1) {
			pos = txt.Find(c, pos);
			if (pos != -1) {
				///ajoutebreak(STR(titre), e.LineFromChar(pos) + 1);
				lexchange.debugging = true;
				pos++;
			}
		}
	}
	OnKifExecuter();
}

wchar_t Accolade(CString& ligne, bool virgpar) {
	int i;
	char trouve = 0;
	int mx = ligne.GetLength();
	wchar_t c;
	bool acc = false;
	for (i = 0; i < mx; i++) {
		c = ligne[i];
		if (c <= 32)
			continue;
		switch (c) {
		case '{':
			acc = true;
			trouve = '{';
			break;
		case '}':
			if (acc)
				return 0;
			return c;
		case ',':
			if (virgpar) {
				if (acc)
					trouve = '(';
				else
					trouve = ',';
			}
			break;
		case ')':
			if (virgpar)
				trouve = ')';
			break;
		case '"':
			if (trouve != '{')
				trouve = 0;
			i++;
			while (i < mx && ligne[i] != '"') {
				if (ligne[i] == '\\')
					i++;
				i++;
			}
			break;
		case '\'':
			if (trouve != '{')
				trouve = 0;
			i++;
			while (i < mx && ligne[i] != '\'')
				i++;
			break;
		case ';':
			if (trouve != '{')
				trouve = 0;
			break;
		case '/':
			if (i < mx - 1 && ligne[i + 1] == '/')
				return trouve;
		default:
			if (trouve != '{')
				trouve = 0;
		}
	}
	if (acc && !trouve)
		return '{';
	return trouve;
}

long CTAMGUIView::FindCorrespondance(long d, long f, int tofind) {
	CRichEditCtrl& e = GetRichEditCtrl();
	TCHAR sx[1024];
	int l, ln;
	l = e.LineFromChar(f);
	int inc = 0;
	int fin = -1;
	wchar_t finchar;

	CString s;
	int compte = 0;
	if (tofind == 2) {
		compte = 1;
		ln = e.GetLine(l, sx, 512);
		sx[ln] = 0;
		s = sx;
		finchar = Accolade(s, false);
		if (finchar == '{') {
			inc = 1;
			long length = e.GetTextLengthEx(GTL_NUMBYTES, 1200);
			fin = e.LineFromChar(length - 1);
		}
		else
		if (finchar == '}')
			inc = -1;
		else
			return -1;
	}
	else {
		inc = -1;
		finchar = '}';
		compte = tofind;
	}
	l += inc;

	wchar_t c;
	while (l != fin) {
		ln = e.GetLine(l, sx, 512);
		sx[ln] = 0;
		s = sx;
		s = s.Trim();
		if (s.GetLength()) {
			c = Accolade(s, false);
			if (c) {
				if (finchar == c)
					compte++;
				else
					compte--;
			}
		}
		if (!compte)
			break;
		l += inc;
	}

	if (!compte)
		return l;
	return -1;
}

long CTAMGUIView::FirstChar(long d, long f, CString& commande) {
	CRichEditCtrl& e = GetRichEditCtrl();
	TCHAR sx[1024];
	int l, ln;
	l = e.LineFromChar(f);
	int fin = -1;
	l--;
	vector<string> lines;
	vector<long> blancs;
	CString s;

	while (l != -1) {
		ln = e.GetLine(l, sx, 512);
		sx[ln] = 0;
		s = sx;
		s = s.Trim();
		lines.insert(lines.begin(), "");
		s_unicode_to_utf8(lines.front(), WCSTR(s), s.GetLength());
		if (sx[0] > 32)
			break;
		l--;
	}

	if (lines.size() == 0)
		return 0;
	bool lisp = false;
	if (lines[0].size() >= 2 && lines[0][0] == '(' && lines[0][1] == ')')
		lisp = true;

	lines.push_back("@;");
	string codeindente;
	IndentationCode(codeindente, lines, blancs, 0, false, lisp);
	return blancs.back();
}


long CTAMGUIView::FirstEnd(long d, long f) {
	CRichEditCtrl& e = GetRichEditCtrl();
	TCHAR sx[1024];
	int l, ln;
	l = e.LineFromChar(f);
	int fin = -1;
	CString s;

	l -= 2;
	int saute = 0;
	int compte = 0;
	while (l != -1) {
		ln = e.GetLine(l, sx, 512);
		sx[ln] = 0;
		s = sx;
		s = s.Trim();
		if (s.GetLength()) {
			wchar_t c = Accolade(s, false);
			if (c == '}')
				compte++;
			else
			if (c == '{')
				compte--;
			if (!compte) {
				if (Teststring(s, L"else", L'{')) {
					l--;
					saute++;
					continue;
				}
				if (Teststring(s, L"if", L'(') || Teststring(s, L"elif", L'(')) {
					if (!saute)
						return l;
					saute--;
				}
			}
		}
		l--;
	}
	return -1;
}


void CTAMGUIView::OnKifCorrespondance()
{
	// TODO: Add your command handler code here
	long d, f;
	//fermertab();
	CRichEditCtrl& e = GetRichEditCtrl();
	e.GetSel(d, f);

	long l = FindCorrespondance(d, f, 2);
	if (l != -1)
		SelectionneLigne(++l);
}


void CTAMGUIView::OnKifCompiler()
{
	// TODO: Add your command handler code here
	// TODO: Add your command handler code here
	if (!TamguExtinguish())
		return;

	lexchange.compilation = true;
	OnKifExecuter();
}



void CTAMGUIView::OnEditionSuivant()
{
	// TODO: Add your command handler code here
	if (findString == "")
		return;
	if (FindText(findString, bMatchCase, bMatchWholeWord, bSearchDown)) {
		if (findMode == false)
			GetRichEditCtrl().ReplaceSel(replacementString);
	}
}

void CTAMGUIView::OnEditionArguments()
{
	// TODO: Add your command handler code here
	Arguments argums;
	argums.arguments = basearguments;
	if (argums.DoModal() == IDOK)
		basearguments = argums.arguments;
}


void CTAMGUIView::OnEditionAllerligne()
{
	// TODO: Add your command handler code here
	Arguments argums;
	long l;
	if (argums.DoModal() == IDOK) {
		l = wcstol(LPCWSTR(argums.arguments), NULL, 10);
		SelectionneLigne(l);
	}
}

void CTAMGUIView::OnEditUndo()
{
	if (listundos.size() <= 1)
		return;

	ASSERT_VALID(this);
	CRichEditCtrl& e = GetRichEditCtrl();
	ModifItem md(listundos.back().s, listundos.back().d, listundos.back().f);
	listundos.pop_back();
	ModifItem& mdp = listundos.back();
	//In that case, we need to get the CR in the selection
	e.HideSelection(TRUE, FALSE);
	e.SetSel(md.d, md.f);
	e.ReplaceSel(mdp.s);
	e.SetSel(mdp.f, mdp.f);

	e.HideSelection(FALSE, FALSE);
	listdos.push_back(md);
	listdos.push_back(mdp);
	listundos.pop_back();
	m_bSyncCharFormat = m_bSyncParaFormat = TRUE;
	if ((mdp.f - mdp.d) > 50)
		Colorie();

}

void CTAMGUIView::OnEditRedo()
{
	if (listdos.size() <= 1)
		return;

	ASSERT_VALID(this);
	CRichEditCtrl& e = GetRichEditCtrl();
	ModifItem md(listdos.back().s, listdos.back().d, listdos.back().f);
	listdos.pop_back();
	ModifItem& mdp = listdos.back();
	e.HideSelection(TRUE, FALSE);
	e.SetSel(md.d, md.f);
	e.ReplaceSel(mdp.s);
	e.SetSel(mdp.f, mdp.f);
	e.HideSelection(FALSE, FALSE);
	listundos.push_back(md);
	listundos.push_back(mdp);
	listdos.pop_back();
	m_bSyncCharFormat = m_bSyncParaFormat = TRUE;
	if ((mdp.f - mdp.d) > 50)
		Colorie();
}


void CTAMGUIView::OnUpdateKifExecuter(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(1 - lexchange.encours);
	if (affichecursor == 2) {
		affichecursor = 0;
		EndWaitCursor();
	}
	else
	if (affichecursor == 1)
		BeginWaitCursor();
}


void CTAMGUIView::OnUpdateKifReexecute(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(1 - lexchange.encours);
}


void CTAMGUIView::OnUpdateKifPasapas(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(1 - lexchange.encours);
}


void CTAMGUIView::OnUpdateKifCompiler(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(1 - lexchange.encours);
}


void CTAMGUIView::OnUpdateKifBreak(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(lexchange.encours);
}



void CTAMGUIView::OnEditionAbandonne()
{
	// TODO: Add your command handler code here
	nbcurly = 0;
	nbsquare = 0;
	accumulecode = _T("");
}


void CTAMGUIView::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(listundos.size() != 0);
}


void CTAMGUIView::OnUpdateEditRedo(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(listdos.size() != 0);
}




void CTAMGUIView::OnUpdateKifSansinteruption(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(1 - lexchange.encours);
}


void CTAMGUIView::OnEditCut()
{
	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();
	CString s = e.GetSelText();
	long d, f;
	e.GetSel(d, f);
	if (d && s.Right(1) == 13) {
		CString lcar;
		e.GetTextRange(d - 1, d, lcar);
		if (lcar[0] != 13) //it is not the case, we need to remove the trailing CR from the string
			e.GetTextRange(d, --f, s);
	}
	Addundos(s, d, f);
	CRichEditView::OnEditCut();
	Addundos(_T(""), d, d);
}



void CTAMGUIView::OnEditPaste()
{
	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();
	CString s = e.GetSelText();
	long d, f;
	e.GetSel(d, f);
	if (d && s.Right(1) == 13) {
		CString lcar;
		e.GetTextRange(d - 1, d, lcar);
		if (lcar[0] != 13) //it is not the case, we need to remove the trailing CR from the string
			e.GetTextRange(d, --f, s);
	}
	Addundos(s, d, f);
	CRichEditView::OnEditPaste();
	long dp, fp;
	e.GetSel(dp, fp);
	e.SetSel(d, fp);
	s = e.GetSelText();
	Addundos(s, d, fp);
	e.SetSel(dp, fp);
}


void CTAMGUIView::OnUpdateEditCut(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(1);
}



void CTAMGUIView::OnEditionSuggestion()
{
	// TODO: Add your command handler code here
	suggestion = 1 - suggestion;

}


void CTAMGUIView::OnUpdateEditionSuggestion(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(suggestion);
}


void CTAMGUIView::OnEditionTailleindentation()
{
	// TODO: Add your command handler code here
	wtamgulibs argums;
	int l;
	CString ind("Blank size");
	wstring val = GetKeyVal(L"SZBLANKS");
	argums.libspath = val.c_str();
	if (argums.DoModal() == IDOK) {
		l = wcstol(LPCWSTR(argums.libspath), NULL, 10);
		SetBlankSize(l);
		SetKeyVal(L"SZBLANKS", argums.libspath);
	}
}


void CTAMGUIView::OnEditionPi()
{
	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();
	long dp, fp;
	e.GetSel(dp, fp);
	e.SetSel(fp, fp);
	CString w;
	fromstring(w, "π");
	e.ReplaceSel(w);
}


void CTAMGUIView::OnEditTau()
{
	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();
	long dp, fp;
	e.GetSel(dp, fp);
	e.SetSel(fp, fp);
	CString w;
	fromstring(w, "τ");
	e.ReplaceSel(w);
}

void CTAMGUIView::OnEditionDivide()
{
	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();
	long dp, fp;
	e.GetSel(dp, fp);
	e.SetSel(fp, fp);
	string ws = "÷";
	CString w;
	fromstring(w, ws);
	e.ReplaceSel(w);
}


void CTAMGUIView::OnEditionGolden()
{
	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();
	long dp, fp;
	e.GetSel(dp, fp);
	e.SetSel(fp, fp);
	string ws = "φ";
	CString w;
	fromstring(w, ws);
	e.ReplaceSel(w);
}


void CTAMGUIView::OnEditionMultiply()
{
	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();
	long dp, fp;
	e.GetSel(dp, fp);
	e.SetSel(fp, fp);
	string ws = "×";
	CString w;
	fromstring(w, ws);
	e.ReplaceSel(w);
}


void CTAMGUIView::OnEditionSqare()
{
	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();
	long dp, fp;
	e.GetSel(dp, fp);
	e.SetSel(fp, fp);
	string ws = "²";
	CString w;
	fromstring(w, ws);
	e.ReplaceSel(w);
}


void CTAMGUIView::OnEditionCube()
{
	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();
	long dp, fp;
	e.GetSel(dp, fp);
	e.SetSel(fp, fp);
	string ws = "³";
	CString w;
	fromstring(w, ws);
	e.ReplaceSel(w);
}


void CTAMGUIView::OnMenuSquareRoot()
{
	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();
	long dp, fp;
	e.GetSel(dp, fp);
	e.SetSel(fp, fp);
	string ws = "√(";
	CString w;
	fromstring(w, ws);
	e.ReplaceSel(w);
}


void CTAMGUIView::OnEditionCubicroot()
{
	// TODO: Add your command handler code here
	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();
	long dp, fp;
	e.GetSel(dp, fp);
	e.SetSel(fp, fp);
	string ws = "∛(";
	CString w;
	fromstring(w, ws);
	e.ReplaceSel(w);
}


void CTAMGUIView::OnEditionSum()
{
	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();
	long dp, fp;
	e.GetSel(dp, fp);
	e.SetSel(fp, fp);
	string ws = "∑(";
	CString w;
	fromstring(w, ws);
	e.ReplaceSel(w);
}


void CTAMGUIView::OnEditionProduct()
{
	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();
	long dp, fp;
	e.GetSel(dp, fp);
	e.SetSel(fp, fp);
	string ws = "∏(";
	CString w;
	fromstring(w, ws);
	e.ReplaceSel(w);
}


void CTAMGUIView::OnEditionEuler()
{
	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();
	long dp, fp;
	e.GetSel(dp, fp);
	e.SetSel(fp, fp);
	string ws = "ℯ";
	CString w;
	fromstring(w, ws);
	e.ReplaceSel(w);
}


void CTAMGUIView::OnFormatFont()
{
	// TODO: Add your command handler code here
	CHARFORMAT2 cfbase = GetCharFormatSelection();
	CRichEditView::OnFormatFont();
	CHARFORMAT2 cfbis = GetCharFormatSelection();
	if (cfbis.szFaceName == cfbase.szFaceName && cfbis.yHeight == cfbase.yHeight && cfbis.dwEffects == cfbase.dwEffects)
		return;

	tailleHauteur = cfbis.yHeight;
	police = cfbis.szFaceName;
	changepolice = true;
	Colorie();
	changepolice = false;
}


void CTAMGUIView::OnEditDifferent()
{
	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();
	long dp, fp;
	e.GetSel(dp, fp);
	e.SetSel(fp, fp);
	string ws = "≠";
	CString w;
	fromstring(w, ws);
	e.ReplaceSel(w);
}


void CTAMGUIView::OnEditDisjunction()
{
	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();
	long dp, fp;
	e.GetSel(dp, fp);
	e.SetSel(fp, fp);
	CString w;
	fromstring(w, "∨");
	e.ReplaceSel(w);
}


void CTAMGUIView::OnEditConjunction()
{
	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();
	long dp, fp;
	e.GetSel(dp, fp);
	e.SetSel(fp, fp);
	CString w;
	fromstring(w, "∧");
	e.ReplaceSel(w);
}


void CTAMGUIView::OnEditNegation32889()
{
	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();
	long dp, fp;
	e.GetSel(dp, fp);
	e.SetSel(fp, fp);
	string ws = "¬";
	CString w;
	fromstring(w, ws);
	e.ReplaceSel(w);
}


void CTAMGUIView::OnEditRulearrow()
{
	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();
	long dp, fp;
	e.GetSel(dp, fp);
	e.SetSel(fp, fp);
	string ws = "←";
	CString w;
	fromstring(w, ws);
	e.ReplaceSel(w);
}


void CTAMGUIView::OnEditRulerightarrow()
{
	// TODO: Add your command handler code here
	CRichEditCtrl& e = GetRichEditCtrl();
	long dp, fp;
	e.GetSel(dp, fp);
	e.SetSel(fp, fp);
	string ws = "→";
	CString w;
	fromstring(w, ws);
	e.ReplaceSel(w);
}

