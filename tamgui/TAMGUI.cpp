/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : TAMGUI.cpp
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/


// TAMGUI.cpp : Définit les comportements de classe pour l'application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "TAMGUI.h"
#include "MainFrm.h"

#include "tamgu.h"
#include "ChildFrm.h"
#include "TAMGUIDoc.h"
#include "TAMGUIView.h"
#include "wtamgulibs.h"

#include "tamguversion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTAMGUIApp

BEGIN_MESSAGE_MAP(CTAMGUIApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CTAMGUIApp::OnAppAbout)
	// Commandes de fichier standard
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// Commande standard de configuration de l'impression
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
	ON_COMMAND(ID_FICHIER_SETTAMGUILIBS, &CTAMGUIApp::OnFichierSettamgulibs)
END_MESSAGE_MAP()

void fromstring(CString& commande, string res);

// construction CTAMGUIApp

CTAMGUIApp::CTAMGUIApp()
{
	m_bHiColorIcons = TRUE;

	// prend en charge le Gestionnaire de redémarrage
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// Si l'application est créée à l'aide de la prise en charge Common Language Runtime (/clr):
	//     1) Ce paramètre supplémentaire est nécessaire à la prise en charge du Gestionnaire de redémarrage.
	//     2) Dans votre projet, vous devez ajouter une référence à System.Windows.Forms pour la génération.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: remplacer la chaîne d'ID de l'application ci-dessous par une chaîne ID unique ; le format recommandé
	// pour la chaîne est CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("Tamgu.AppID.NoVersion"));

	// TODO: ajoutez ici du code de construction,
	// Placez toutes les initialisations significatives dans InitInstance
}

// Seul et unique objet CTAMGUIApp

CTAMGUIApp theApp;

static HKEY OpenKey(HKEY hRootKey, wchar_t* strKey) {
	HKEY hKey;
	LONG nError = RegOpenKeyEx(hRootKey, strKey, NULL, KEY_ALL_ACCESS, &hKey);

	if (nError == ERROR_FILE_NOT_FOUND)
	{
		nError = RegCreateKeyEx(hRootKey, strKey, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	}

	return hKey;
}

static inline void SetValKey(HKEY hKey, LPCTSTR lpValue, LPCTSTR data, int sz)
{
	DWORD pathLenInBytes = 4 * sz;
	RegSetValueEx(hKey, lpValue, NULL, REG_SZ, (LPBYTE)data, pathLenInBytes);
}

wstring GetKeyVal(wstring key)
{
	HKEY happkey = theApp.GetAppRegistryKey();
	HKEY hKey = OpenKey(happkey, (wchar_t*)key.c_str());
	TCHAR value[2048];
	value[0] = 0;
	DWORD value_length = 2048;
	DWORD res = 1;
	DWORD ret = RegQueryValueEx(hKey, key.c_str(), NULL, &res, (LPBYTE)&value, &value_length);
	RegCloseKey(hKey);
	return value;
}

void SetKeyVal(wstring key, CString value)
{
	HKEY happkey = theApp.GetAppRegistryKey();
	HKEY hKey = OpenKey(happkey, (wchar_t*)key.c_str());
	SetValKey(hKey, key.c_str(), value, value.GetLength());
	RegCloseKey(hKey);
}

static wstring GetValKey(HKEY hKey)
{
	WCHAR value[2048];
	value[0] = 0;
	DWORD value_length = 2048;
	DWORD res = 1;
	RegQueryValueEx(hKey, L"TAMGULIBS", NULL, &res, (LPBYTE)&value, &value_length);
	return value;
}

static void SetTAMGUILIBS(CString& chemin) {
	HKEY happkey = theApp.GetAppRegistryKey();
	HKEY hKey = OpenKey(happkey, L"TAMGULIBS");
	SetValKey(hKey, L"TAMGULIBS", chemin, chemin.GetLength());
	RegCloseKey(hKey);
	string command("TAMGULIBS=");
	string chm;
	s_unicode_to_utf8(chm, (wchar_t*)LPCWSTR(chemin), chemin.GetLength());
	command += chm;
	_putenv(STR(command));
}

static void InitTAMGUILIBS() {
	HKEY happkey = theApp.GetAppRegistryKey();
	HKEY hKey = OpenKey(happkey, L"TAMGULIBS");
	wstring val = GetValKey(hKey);

	if (val.size() == 0) {
		//If TAMGUILIBS was not set originaly
		char localpath[4096];
		_fullpath(localpath, ".", 4096);
		if (localpath[strlen(localpath) - 1] != '\\')
			strcat(localpath, "\\");
		sc_utf8_to_unicode(val, (unsigned char*)localpath, strlen(localpath));
		CString chemin = val.c_str();
		SetTAMGUILIBS(chemin);
	}
	else
		RegCloseKey(hKey);

	string chm;
	if (val.size()) {
		string command("TAMGULIBS=");
		s_unicode_to_utf8(chm, val);
		command += chm;
		_putenv(STR(command));
	}

	string pth = getenv("Path");
	if (pth.find(chm) == string::npos) {
		pth = "Path=" + chm + ";" + pth;
		_putenv(STR(pth));
	}

	val = GetKeyVal(L"SZBLANKS");
	if (val == L"") {
		CString sz = L"5";
		SetKeyVal(L"SZBLANKS", sz);
	}
}

// initialisation de CTAMGUIApp

BOOL CTAMGUIApp::InitInstance()
{
	// InitCommonControlsEx() est requis sur Windows XP si le manifeste de l'application
	// spécifie l'utilisation de ComCtl32.dll version 6 ou ultérieure pour activer les
	// styles visuels.  Dans le cas contraire, la création de fenêtres échouera.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// À définir pour inclure toutes les classes de contrôles communs à utiliser
	// dans votre application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// Initialiser les bibliothèques OLE
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	
	//A UTILISER ABSOLUMENT SINON PLANTAGE ALEATOIRE. Gere la concurence entre THREADS
	CoInitializeEx(NULL,COINIT_MULTITHREADED);

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// AfxInitRichEdit2() est requis pour utiliser un contrôle RichEdit	
	AfxInitRichEdit2();

	// Initialisation standard
	// Si vous n'utilisez pas ces fonctionnalités et que vous souhaitez réduire la taille
	// de votre exécutable final, vous devez supprimer ci-dessous
	// les routines d'initialisation spécifiques dont vous n'avez pas besoin.
	// Changez la clé de Registre sous laquelle nos paramètres sont enregistrés
	// TODO: modifiez cette chaîne avec des informations appropriées,
	// telles que le nom de votre société ou organisation
	SetRegistryKey(_T("Tamgu"));
	LoadStdProfileSettings(10);  // Charge les options de fichier INI standard (y compris les derniers fichiers utilisés)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Inscrire les modèles de document de l'application. Ces modèles
	//  lient les documents, fenêtres frame et vues entre eux
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_TAMGUITYPE,
		RUNTIME_CLASS(CTAMGUIDoc),
		RUNTIME_CLASS(CChildFrame), // frame enfant MDI personnalisé
		RUNTIME_CLASS(CTAMGUIView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// crée la fenêtre frame MDI principale
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	// appelle DragAcceptFiles uniquement s'il y a un suffixe
	//  Dans une application MDI, cet appel doit avoir lieu juste après la définition de m_pMainWnd
	// Activer les ouvertures via glisser-déplacer
	m_pMainWnd->DragAcceptFiles();

	// Analyser la ligne de commande pour les commandes shell standard, DDE, ouverture de fichiers
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Activer les ouvertures d'exécution DDE
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	if (cmdInfo.m_nShellCommand==CCommandLineInfo::FileOpen)
		OnFileNew();

	// Commandes de dispatch spécifiées sur la ligne de commande. Retournent FALSE si
	// l'application a été lancée avec /RegServer, /Register, /Unregserver ou /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// La fenêtre principale a été initialisée et peut donc être affichée et mise à jour
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
	InitTAMGUILIBS();

	return TRUE;
}


int CTAMGUIApp::ExitInstance()
{
	//TODO: gérez les ressources supplémentaires que vous avez ajoutées
	AfxOleTerm(FALSE);
	//CoUninitialize();
	return CWinAppEx::ExitInstance();
}

// gestionnaires de messages pour CTAMGUIApp


// boîte de dialogue CAboutDlg utilisée pour la boîte de dialogue 'À propos de' pour votre application

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Données de boîte de dialogue
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

// Implémentation
protected:
	DECLARE_MESSAGE_MAP()
public:
	CString version;
};


CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
	, version(_T("Tamgu(64): "))
{
	string v = TamguVersion();
	CString cv;
	fromstring(cv, v);
	version += cv;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, version);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// Commande App pour exécuter la boîte de dialogue
void CTAMGUIApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CTAMGUIApp, méthodes de chargement/d'enregistrement de la personnalisation

void CTAMGUIApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CTAMGUIApp::LoadCustomState()
{
}

void CTAMGUIApp::SaveCustomState()
{
}

// gestionnaires de messages pour CTAMGUIApp


void CTAMGUIApp::OnFichierSettamgulibs()
{
	// TODO: Add your command handler code here
	wtamgulibs getpath;
	HKEY happkey = theApp.GetAppRegistryKey();
	HKEY hKey = OpenKey(happkey, L"TAMGULIBS");
	wstring val = GetValKey(hKey);
	RegCloseKey(hKey);
	getpath.libspath = val.c_str();
	if (getpath.DoModal() == IDOK)
		SetTAMGUILIBS(getpath.libspath);
}

