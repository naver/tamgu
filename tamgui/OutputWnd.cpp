/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : OutputWnd.cpp
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/


#include "stdafx.h"

#include "OutputWnd.h"
#include "Resource.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar
COutputWnd* fsortie=NULL;

COutputWnd::COutputWnd()
{
	if (fsortie==NULL)
		fsortie=this;
}

COutputWnd::~COutputWnd()
{
}

BEGIN_MESSAGE_MAP(COutputWnd, CMFCTasksPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


void COutputWnd::OnPressForwardButton() {
	int i=10;
	i=10;
}

void COutputWnd::OnPressBackButton() {
	int i=10;
	i=10;
}

void COutputWnd::OnForward() {
	int i=10;
	i=10;
}

void COutputWnd::OnBack() {
	int i=10;
	i=10;
}

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCTasksPane::OnCreate(lpCreateStruct) == -1)
		return -1;
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	EnableNavigationToolbar (TRUE);

	EnableWrapLabels (TRUE);
	EnableOffsetCustomControls (FALSE);

	// Créer la fenêtre d'onglets :
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
	{
		TRACE0("Impossible de créer la fenêtre d'onglets de sortie\n");
		return -1;      // échec de la création
	}

	// Créer les volets de sortie :
	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	if (!m_wndOutputBuild.Create(dwStyle, rectDummy, &m_wndTabs, 2) ||
		!m_wndOutputDebug.Create(dwStyle, rectDummy, &m_wndTabs, 3) ||
		!m_wndOutputFind.Create(dwStyle, rectDummy, &m_wndTabs, 4))
	{
		TRACE0("Impossible de créer les fenêtres Sortie\n");
		return -1;      // échec de la création
	}
	

	UpdateFonts();

	CString strTabName;
	BOOL bNameValid;

	// Attacher les fenêtres de listes à l'onglet :
	bNameValid = strTabName.LoadString(IDS_BUILD_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputBuild, strTabName, (UINT)0);
	bNameValid = strTabName.LoadString(IDS_DEBUG_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputDebug, strTabName, (UINT)1);
	bNameValid = strTabName.LoadString(IDS_FIND_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputFind, strTabName, (UINT)2);

	return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CMFCTasksPane::OnSize(nType, cx, cy);

	// Le contrôle onglet doit couvrir toute la zone cliente :
	m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

	int cxExtentMax = 0;

	for (int i = 0; i < wndListBox.GetCount(); i ++)
	{
		CString strItem;
		wndListBox.GetText(i, strItem);

		cxExtentMax = max(cxExtentMax, dc.GetTextExtent(strItem).cx);
	}

	wndListBox.SetHorizontalExtent(cxExtentMax);
	dc.SelectObject(pOldFont);
}

void COutputWnd::MetMessage(CString& msg) {
	FillBuildWindow(msg);
}

void COutputWnd::MetDebug(CString& msg) {
	FillDebugWindow(msg);
}

void COutputWnd::FillBuildWindow(CString& msg)
{
	m_wndOutputBuild.AddString(msg);
}

void COutputWnd::CleanMessageWindow() {
	for (int i=0;i<m_wndOutputBuild.GetCount();i++)
		m_wndOutputBuild.DeleteString(i);
}

void COutputWnd::CleanDebugWindow() {
	for (int i=0;i<m_wndOutputDebug.GetCount();i++)
		m_wndOutputDebug.DeleteString(i);
}

void COutputWnd::FillDebugWindow(CString& msg)
{
	m_wndOutputDebug.AddString(msg);
}

void COutputWnd::FillFindWindow()
{
	m_wndOutputFind.AddString(_T("La sortie de la recherche s'affiche ici."));
	m_wndOutputFind.AddString(_T("La sortie s'affiche dans les lignes d'une vue Liste"));
	m_wndOutputFind.AddString(_T("mais vous pouvez changer le mode d'affichage à votre guise..."));
}

void COutputWnd::UpdateFonts()
{
	m_wndOutputBuild.SetFont(&afxGlobalData.fontRegular);
	m_wndOutputDebug.SetFont(&afxGlobalData.fontRegular);
	m_wndOutputFind.SetFont(&afxGlobalData.fontRegular);
}

/////////////////////////////////////////////////////////////////////////////
// COutputList1

COutputList::COutputList()
{
}

COutputList::~COutputList()
{
}

BEGIN_MESSAGE_MAP(COutputList, CListBox)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_VIEW_OUTPUTWND, OnViewOutput)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// Gestionnaires de messages de COutputList

void COutputList::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_OUTPUT_POPUP);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}

	SetFocus();
}

void COutputList::OnEditCopy()
{
	MessageBox(_T("Copier la sortie"));
}

void COutputList::OnEditClear()
{
	MessageBox(_T("Effacer la sortie"));
}

void COutputList::OnViewOutput()
{
	CMFCTasksPane* pParentBar = DYNAMIC_DOWNCAST(CMFCTasksPane, GetOwner());
	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetTopLevelFrame());

	if (pMainFrame != NULL && pParentBar != NULL)
	{
		pMainFrame->SetFocus();
		pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
		pMainFrame->RecalcLayout();

	}
}
