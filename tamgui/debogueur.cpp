/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : debogueur.cpp
 Date       : 2017/09/01
 Purpose    :
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

// debogueur.cpp : implementation file
//

#include "stdafx.h"
#include "tamgu.h"
#include "TAMGUI.h"
#include "debogueur.h"
#include "globaltamgu.h"

extern CTAMGUIView* firstview;
extern LockedThread* loquet;

// debogueur dialog
#define WCSTR(x) (wchar_t*)LPCWSTR(x)

void Launchtamgu();
void DisplayValue(string s, void*);

IMPLEMENT_DYNAMIC(debogueur, CDialogEx)

debogueur::debogueur(CWnd* pParent /*=NULL*/)
	: CDialogEx(debogueur::IDD, pParent)
	, vvariables(_T(""))
	, vpile(_T(""))
	, vsortie(_T(""))
	, choixvariables(_T(""))
	, locals(_T(""))
	, threadId(0)
	, sticktothread(FALSE)
{
    stacktop=NULL;
    stackline=-1;
	currentview=NULL;
	currentline = -1;
	nextline = false;
	getintofunction = false;
	getoutfunction = false;
	lastfunction = -1;
	shortname = true;
	launch = false;
}


debogueur::~debogueur()
{
	if (!globalTamgu->executionbreak)
		loquet->Released();


	globalTamgu->Setdebugmode(false);
	globalTamgu->Setdebugfunction(NULL);
	DisplayValue(((stringstream*)globalTamgu->os)->str(), NULL);
	delete globalTamgu->os;
	globalTamgu->os = oldos;
	TamguDisplayFunction(DisplayValue, NULL);
}

void debogueur::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, vvariables);
	DDX_Text(pDX, IDC_EDIT2, vpile);
	DDX_Control(pDX, IDSUIVANT, bsuivant);
	DDX_Control(pDX, IDSAUTE, bsaute);
	DDX_Control(pDX, IDSORTIR, bsortir);
	DDX_Control(pDX, IDPOINTARRET, bbreak);
	DDX_Control(pDX, IDARRETPRG, barret);
	DDX_Control(pDX, IDFIN, bfin);
	DDX_Text(pDX, IDC_EDIT5, vsortie);
	DDX_Text(pDX, IDC_LOCAL_VARIABLES, locals);
	DDX_Control(pDX, IDENTRE, bentre);
	DDX_Text(pDX, IDC_EDIT6, threadId);
	DDX_Check(pDX, IDC_CHECK1, sticktothread);
}


BEGIN_MESSAGE_MAP(debogueur, CDialogEx)
	ON_BN_CLICKED(IDSAUTE, &debogueur::OnBnClickedSaute)
	ON_BN_CLICKED(IDSORTIR, &debogueur::OnBnClickedSortir)
	ON_BN_CLICKED(IDFIN, &debogueur::OnBnClickedFin)
	ON_BN_CLICKED(IDSUIVANT, &debogueur::OnBnClickedSuivant)
	ON_BN_CLICKED(IDARRETPRG, &debogueur::OnBnClickedArretprg)
	ON_BN_CLICKED(IDPOINTARRET, &debogueur::OnBnClickedPointarret)
	ON_EN_CHANGE(IDC_EDIT4, &debogueur::OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT5, &debogueur::OnEnChangeEdit5)
	ON_MESSAGE(MET_A_JOUR, &debogueur::MAJ)
	ON_BN_CLICKED(IDENTRE, &debogueur::OnBnClickedEntre)
	ON_BN_CLICKED(IDC_CHECK1, &debogueur::OnBnClickedSticktoThread)
	ON_EN_CHANGE(IDC_EDIT6, &debogueur::OnEnChangeEdit6)
END_MESSAGE_MAP()


// debogueur message handlers

LRESULT debogueur::MAJ(WPARAM wParam, LPARAM lParam) {
	UpdateData(FALSE);
	firstview->PostMessage(UPDATE_FROM_DEBUGGER, (WPARAM)filename.c_str(), (LPARAM)currentline);
	CWnd* wnd = GetDlgItem(IDC_EDIT5);
	HWND hEdit = wnd->GetSafeHwnd();
	long nb = SendMessageA(hEdit, EM_GETLINECOUNT, 0, 0);
	SendMessageA(hEdit, EM_LINESCROLL, 0, nb);

	return 0;
}

BOOL debogueur::OnInitDialog() {
	BOOL v=CDialogEx::OnInitDialog();
	HICON  hIcn= (HICON)LoadImage(
		AfxGetApp()->m_hInstance,
		MAKEINTRESOURCE(IDI_SUIVANT),
		IMAGE_ICON,
		0,0, // use actual size
		LR_DEFAULTCOLOR
		);
	bsuivant.SetIcon(hIcn);
	hIcn= (HICON)LoadImage(
		AfxGetApp()->m_hInstance,
		MAKEINTRESOURCE(IDI_SAUTE),
		IMAGE_ICON,
		0,0, // use actual size
		LR_DEFAULTCOLOR
		);
	bsaute.SetIcon(hIcn);

	hIcn= (HICON)LoadImage(
		AfxGetApp()->m_hInstance,
		MAKEINTRESOURCE(IDI_SORTIR),
		IMAGE_ICON,
		0,0, // use actual size
		LR_DEFAULTCOLOR
		);
	bsortir.SetIcon(hIcn);

	hIcn= (HICON)LoadImage(
		AfxGetApp()->m_hInstance,
		MAKEINTRESOURCE(IDI_ENTRE),
		IMAGE_ICON,
		0,0, // use actual size
		LR_DEFAULTCOLOR
		);
	bentre.SetIcon(hIcn);

	hIcn= (HICON)LoadImage(
		AfxGetApp()->m_hInstance,
		MAKEINTRESOURCE(IDI_BREAK),
		IMAGE_ICON,
		0,0, // use actual size
		LR_DEFAULTCOLOR
		);
	bbreak.SetIcon(hIcn);

	hIcn= (HICON)LoadImage(
		AfxGetApp()->m_hInstance,
		MAKEINTRESOURCE(IDI_GO),
		IMAGE_ICON,
		0,0, // use actual size
		LR_DEFAULTCOLOR
		);
	bfin.SetIcon(hIcn);

	hIcn= (HICON)LoadImage(
		AfxGetApp()->m_hInstance,
		MAKEINTRESOURCE(IDI_TERMINER),
		IMAGE_ICON,
		0,0, // use actual size
		LR_DEFAULTCOLOR
		);
	barret.SetIcon(hIcn);

	m_ToolTip.Create(this);

	m_ToolTip.AddTool( &bsuivant, _T("Next line"));
	m_ToolTip.AddTool( &barret, _T("Stop program"));
	m_ToolTip.AddTool( &bsaute, _T("Skip to next breakpoint"));
	m_ToolTip.AddTool( &bfin, _T("To the end"));
	m_ToolTip.AddTool( &bbreak, _T("Add or remove a breakpoint"));
	m_ToolTip.AddTool( &bsortir, _T("Out from current function"));
	m_ToolTip.AddTool( &localvar, _T("Add variables to filter"));

	return v;
}

BOOL debogueur::PreTranslateMessage(MSG* pMsg) {
	m_ToolTip.RelayEvent(pMsg);
	if (!launch) {
		launch = true;
		Launchtamgu();
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void debogueur::Lapile(string& s) {
	s = s_replacestrings(s, "\r\n", "\n");
	s = s_replacestrings(s, "\n", "\r\n");
	wstring base;
	s_utf8_to_unicode(base, (unsigned char*)s.c_str(), s.size());
	vpile = (WCHAR*)base.c_str();
}

void debogueur::Lasortie(string s) {
	s = s_replacestrings(s, "\r\n", "\n");
	s = s_replacestrings(s, "\n", "\r\n");
	wstring base;
	s_utf8_to_unicode(base, (unsigned char*)s.c_str(), s.size());
	vsortie = (WCHAR*)base.c_str();
}

void debogueur::Variables(string& s) {
	s = s_replacestrings(s, "\r\n", "\n");
	s = s_replacestrings(s, "\n", "\r\n");
	wstring base;
	s_utf8_to_unicode(base,(unsigned char*)s.c_str(), s.size());
	vvariables=(WCHAR*)base.c_str();
}

void debogueur::OnBnClickedSaute()
{
	// TODO: Add your control notification handler code here
	if (globalTamgu->executionbreak) {
		CDialogEx::OnOK();
		return;
	}

	nextline = false;
	loquet->Released();
}


void debogueur::OnBnClickedSortir()
{
	// TODO: Add your control notification handler code here
	if (globalTamgu->executionbreak) {
		CDialogEx::OnOK();
		return;
	}

	lastfunction = -1;
	getintofunction = false;
	getoutfunction = true;
	nextline = false;

	loquet->Released();
}


void debogueur::OnBnClickedFin()
{
	if (globalTamgu->executionbreak) {
		CDialogEx::OnOK();
		return;
	}

	// TODO: Add your control notification handler code here
	globalTamgu->Setdebugmode(false);
	globalTamgu->Setdebugfunction(NULL);
	loquet->Released();

	CDialogEx::OnOK();
}


void debogueur::OnBnClickedSuivant()
{
	// TODO: Add your control notification handler code here
	if (globalTamgu->executionbreak) {
		CDialogEx::OnOK();
		return;
	}

	nextline = true;
	loquet->Released();
}

void debogueur::OnBnClickedEntre()
{
	// TODO: Add your control notification handler code here
	if (globalTamgu->executionbreak) {
		CDialogEx::OnOK();
		return;
	}

	getintofunction = true;
	getoutfunction = false;
	nextline = true;
	loquet->Released();
}


void debogueur::OnBnClickedArretprg()
{
	// TODO: Add your control notification handler code here
	if (!globalTamgu->executionbreak)
		loquet->Released();

	globalTamgu->Setdebugmode(false);
	globalTamgu->Setdebugfunction(NULL);
	globalTamgu->executionbreak = true;
	CDialogEx::OnOK();
}


void debogueur::OnBnClickedPointarret()
{
	if (currentline != -1)
		currentview->Pointarret(currentline);
}

void debogueur::Locals() {
	localvariables = s_replacestrings(localvariables, "\n", "\r\n");
	wstring base;
	s_utf8_to_unicode(base, (unsigned char*)localvariables.c_str(), localvariables.size());
	locals=(WCHAR*)base.c_str();
}

void debogueur::OnEnChangeEdit4()
{
	UpdateData(TRUE);

	Locals();
	UpdateData(FALSE);
}


void debogueur::OnEnChangeEdit5()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	UpdateData(TRUE);
}



void debogueur::OnBnClickedSticktoThread()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	//if (sticktothread==FALSE)
	//	tamgusticktothread(-99);
	//else
	//	tamgusticktothread(threadId);
}


void debogueur::OnEnChangeEdit6()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	//sticktothread=TRUE;
	//tamgusticktothread(threadId);
	UpdateData(FALSE);
}

Tamgu* Debug_callback(vector<Tamgu*>& stack, short idthread, void* data) {
	static ThreadLock _locker;

	Locking _lock(_locker);

	debogueur* infodebug = (debogueur*)data;

	Tamgu* top = stack.back();

	short idcode = top->Currentspace();

	if (idcode != -1) {
		if (infodebug->nextline) {
			if (infodebug->stackline != -1 && infodebug->stackline<stack.size() && stack[infodebug->stackline] == infodebug->stacktop)
				return aNULL;

			infodebug->stackline = -1;
			infodebug->stacktop = NULL;

			if (infodebug->getoutfunction == true) {
				//First we need to check if we are in a function...
				infodebug->getoutfunction = false;
				for (long i = stack.size() - 1;i >= 0; i--) {
					if (stack[i]->isaFunction()) {
						//Then the previous element should be a call..
						infodebug->stacktop = stack[i];
						infodebug->stackline = i;
						infodebug->currentline = -1;
						return aNULL;
					}
				}
			}
			else {
				if (infodebug->getintofunction == false && top->isaFunction()) {
					//Then in this case, we need to jump over it...
					infodebug->stacktop = top;
					infodebug->stackline = stack.size() - 1;
					infodebug->currentline = -1;
					return aNULL;
				}
			}
		}

		short fileid = top->Currentfile();
		string filename = globalTamgu->Getfilename(fileid);

		infodebug->filename = filename;
		infodebug->currentline = top->Currentline();

		if (infodebug->nextline || infodebug->checkbreakpoint(filename)) {
			string stacks;
			string allvariables;

			globalTamgu->Getdebuginfo(infodebug->localvariables, allvariables, stacks, false, 50, idthread);

			infodebug->Lapile(stacks);
			infodebug->Locals();
			infodebug->Variables(allvariables);
			infodebug->Lasortie(((stringstream*)globalTamgu->os)->str());
			infodebug->PostMessage(MET_A_JOUR, 0, 0);

			loquet->Blocked();
			return aTRUE;
		}
	}

	globalTamgu->Cleardebug(idthread);
	return aFALSE;
}

