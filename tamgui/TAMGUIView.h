/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : TAMGUIView.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/


// TAMGUIView.h : interface de la classe CTAMGUIView
//

#pragma once

//comments is 5
//long string is 10
//double quote string is 1
//simple quote string is 2
// ?var is 9
// #var or $var is 8
// Colorkeywords is 3
// Normal token is 4
// . is 6
// $ [ { is 7
// 


class x_colorreading : public x_wreading {
public:

	x_colorreading();
	vector<long> stacksize;

	void setrules() {

        lookforquotes = true;
        
		//Spaces, skipped in the parsing string
		rules.push_back(" =#");                         //0     space (not kept)
		rules.push_back("\t=#");                        //1     tab (not kept)
		rules.push_back("\n=#");                        //2     cr (not kept)
		rules.push_back("\r=#");                        //3     cr (not kept)

        rules.push_back("1:{%d #A-F #a-f}");            //2     metarule on 1, for hexadecimal digits

        //Fast tracks for recurrent punctations
		rules.push_back(";=0");                         //4     ;
		rules.push_back(",=0");                         //5     ,
		rules.push_back("==0");                         //6     =
		rules.push_back("~=0");                         //7     ~
		rules.push_back("!=0");                         //8     !
		rules.push_back("(=7");                         //9     (
		rules.push_back(")=0");                         //10    )
		rules.push_back("]=0");                         //11    ]
		rules.push_back("}=0");                         //12    }
		rules.push_back("..=0");                        //13    ..
		rules.push_back("^=0");                         //14    ^
		rules.push_back("+=0");                         //15    +
		rules.push_back("-=0");                         //16    -
		rules.push_back("*=0");                         //17    *
		rules.push_back("%=0");                         //18    %
		rules.push_back("<=0");                         //19    <
		rules.push_back(">=0");                         //20    >
		rules.push_back("|=0");                         //21    |
		rules.push_back("&=0");                         //22    &
		rules.push_back(":=0");                         //23    :
		
        rules.push_back("#%d+=8");                      //24    rules for #d+
		rules.push_back("#=0");                         //25    #

		rules.push_back("?{%a %d}+=9");                 //26    rules for ?var
		rules.push_back("?=0");                         //27    ?

		rules.push_back(".=6");                         //28    .
		rules.push_back("[=7");                         //29    [
		rules.push_back("{=7");                         //30    {

		rules.push_back("$%d+=8");                      //31    rules for $d+
		rules.push_back("$=7");                         //32    :

														//Comments
		rules.push_back("//%.~%r+=5");                  //33    comments starting with // with no carriage return (CR) inside (not kept)
		rules.push_back("//=5");                        //34    empty comment starting with // with no carriage return (CR) inside (not kept)
		rules.push_back("/@%.+@/=5");                   //35    long comments starting with /@ and finishing with @/ (not kept)
		rules.push_back("/@@/=5");                      //36    empty long comment starting with /@ and finishing with @/
		rules.push_back("/=0");                         //37    /

														//Strings
														//Double quote
		rules.push_back("\"\"=1");                      //38    empty string ""
		rules.push_back("\"%?~%r+\"=1");                //39    string "" does not contain CR and can escape characters (%?)

														//Single quote
		rules.push_back("''=2");                        //40    empty string ''
		rules.push_back("'%.~%r+'=2");                  //41    string '' does not contain CR and does not process escape characters

		rules.push_back("r\"%?+\"=2");                 //44    tamgu regular expressions...
		rules.push_back("r'%?+'=2");                   //44    tamgu regular expressions...
		rules.push_back("p\"%?+\"=2");                 //44    tamgu regular expressions...
		rules.push_back("p'%?+'=2");                   //44    tamgu regular expressions...
		
		//Long quotes
		rules.push_back("@\"\"@=5");                  //42    empty string @""@
		rules.push_back("@\"%?+\"@=10");              //43    string @" "@ can contain CR and escape characters (we do not keep the @s)

														//Unicode double quote strings
		rules.push_back("u\"\"=6");                    //44    empty string u""
		rules.push_back("u\"%?~%r+\"=6");              //45    string u"" unicode string (we do not keep the u in the parse)

														//Unicode single quote strings
		rules.push_back("u''=1");						//46    empty string u''
		rules.push_back("u'%.~%r+'=2");					//47    string u'' unicode string

														//Unicode long quote strings
		rules.push_back("u@\"%?+\"@=10");				//48    empty string u@""@
		rules.push_back("u@\"%?+\"@=10");				//49    string u@".."@ unicode string

        rules.push_back("0x%1+(.%1+)(p([- +])%d+)=#"); //47 hexadecimal
		rules.push_back("%d+(.%d+)(e([- +])%d+)=#");    //47    exponential digits

														// Rules start here
		rules.push_back("{%a %d %h}+=4");               //52    label a combination of alpha, digits and hangul characters
        rules.push_back("%n=#");                        //53    non-breaking space
        rules.push_back("%o=0");                        //54    operators
		rules.push_back("%p=0");                        //53    punctuation
		rules.push_back("%.~{%S %p %o}+=4");            //55    An unknown UTF8 token separated with spaces, punctuation or operators...
	}

	void tokenize(wstring& thestr, bool keeppos = false);

};

class CTAMGUIView;

class echange {
public:
	CString s;
	bool encours;
	bool compilation;
	bool debugging;
	HWND vue;
	CTAMGUIView* wvue;
	short idcode;
	
	echange() {
		idcode = 0;
		debugging=false;
		encours=false;
		compilation=false;
		vue=NULL;
	}

	void initialisation(CTAMGUIView*v,CString& x);
};

class ModifItem {
public:
	CString s;
	long d,f;

	ModifItem(CString x,int D,int F): s(x) {				
		d=D;f=F;
	}
};


class CTAMGUIView : public CRichEditView
{
protected: // création à partir de la sérialisation uniquement
	CTAMGUIView();
	DECLARE_DYNCREATE(CTAMGUIView)

// Attributs
public:

	CTAMGUIDoc* GetDocument() const;
	int derniereposition;
	CString police;
	long tailleHauteur;
	CString commande;
	int fincommande;
	long dernierelongueur;
	CHARFORMAT2 cf;
	bool initcf;
	string stitre;
	CString accumulecode;
	int nbcurly;
	int nbsquare;
	bool firstdisplay;
	bool bMatchCase;
	bool bMatchWholeWord;
	bool bSearchDown;
	x_colorreading bnfxs;
	string currentname;	
	list<ModifItem> listundos;
	list<ModifItem> listdos;
	map<long,bool> activecolorblocks;
	vector<long> colorblocks;
	wstring stype;
	bool suggestion;
	bool changepolice;
	bool begincolor;
	vector<char> stacktype;
	void colore1000();
	void colorisation();


// Opérations
public:

// Substitutions
public:
	bool FindDeclarations(wstring& localcode, int& letype);
	void Keepundos(CString&);
	void Addundos(CString r,long d,long f);
	virtual void OnDraw(CDC* pDC);  // substitué pour dessiner cette vue
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	BOOL PreTranslateMessage(MSG* pMsg);
	void Colorie(long d=-1,long f=-1,int nFirstLine=-1);
	void Chemin(CString s);
	CFindReplaceDialog *m_pFindDialog;
	CString findString;
	CString replacementString;
	bool findMode;
	void comptebrackets(CString& s);
	void Affichage(CString& s,bool);
	void Executetamgu(CString& s);
	CString GetLineText(long l);
	bool ApplyColor(wstring& sw, int offset, int mxpos = -1, bool rebuilt = true);
	long FindCorrespondance(long d,long f,int tofind);
	long FirstChar(long d,long f,CString&);
	long FirstEnd(long d,long f);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	void OnInitialUpdate();
	//virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

// Implémentation
public:
	virtual ~CTAMGUIView();
	void runtamgu(CString& s);
	void executelecode();
	void Afficheerreur();
	void AfficheAnalyse();
	void Choixligne();
	bool Couleur(int r,int g,int b);
	void SelectionneLigne(long l);
	void Italique(bool);	
	void Pointarret(long l);

	LRESULT XYPOSCARET(WPARAM wParam, LPARAM lParam);
	LRESULT SUGGERE(WPARAM wParam, LPARAM lParam);
	LRESULT MAJCOULEUR(WPARAM wParam, LPARAM lParam);
	LRESULT MAJ(WPARAM wParam, LPARAM lParam);
	LRESULT MAJDEPUISDEBUG(WPARAM wParam, LPARAM lParam);
	LRESULT ENDOFPROGRAM(WPARAM wParam, LPARAM lParam);

	LRESULT afficher(WPARAM wParam, LPARAM lParam);
	LRESULT ouvrir(WPARAM wParam, LPARAM lParam);
	LRESULT AfficheCourant(WPARAM wParam, LPARAM lParam);
	void CouleurDepuisDebutLigne(long d,long f);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	
// Fonctions générées de la table des messages
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKifExecuter();
	afx_msg void OnKifReexecuter();
	afx_msg void OnKifLister();
	afx_msg void OnKifRaz();
	afx_msg void OnKifPasaPas();
	afx_msg void OnKifPointarret();
	afx_msg void OnKifNettoyerp();
	afx_msg void OnKifIndenter();
	afx_msg LRESULT OnFindDialogMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEditionCherche();
	afx_msg void OnEditionRemplace();
	afx_msg void OnKifCommenter();
	afx_msg void OnKifBreak();
	afx_msg void OnKifAsThread();
	afx_msg void OnKifCorrespondance();
	afx_msg void OnKifCompiler();
	afx_msg void OnChar(UINT nChar,UINT nRepCnt,UINT nFlags);
	afx_msg void OnEditionSuivant();
	afx_msg void OnEditionArguments();
	afx_msg void OnEditionAllerligne();
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateKifExecuter(CCmdUI *pCmdUI);
	afx_msg void OnUpdateKifReexecute(CCmdUI *pCmdUI);
	afx_msg void OnUpdateKifPasapas(CCmdUI *pCmdUI);
	afx_msg void OnUpdateKifCompiler(CCmdUI *pCmdUI);
	afx_msg void OnUpdateKifBreak(CCmdUI *pCmdUI);	
	afx_msg void OnEditionAbandonne();
	afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditRedo(CCmdUI *pCmdUI);
	afx_msg void OnKifSansinteruption();
	afx_msg void OnUpdateKifSansinteruption(CCmdUI *pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI *pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnEditionSuggestion();
	afx_msg void OnUpdateEditionSuggestion(CCmdUI *pCmdUI);
	afx_msg void OnEditionTailleindentation();
	afx_msg void OnEditionPi();
	afx_msg void OnEditionDivide();
	afx_msg void OnEditionGolden();
	afx_msg void OnEditionMultiply();
	afx_msg void OnEditionSqare();
	afx_msg void OnEditionCube();
	afx_msg void OnMenuSquareRoot();
	afx_msg void OnEditionCubicroot();
	afx_msg void OnEditionSum();
	afx_msg void OnEditionProduct();
	afx_msg void OnEditionEuler();
	afx_msg void OnFormatFont();
	afx_msg void OnEditDifferent();
	afx_msg void OnEditDisjunction();
	afx_msg void OnEditConjunction();
	afx_msg void OnEditNegation32889();
	afx_msg void OnEditRulearrow();
	afx_msg void OnEditRulerightarrow();
	afx_msg void OnEditTau();
};

#ifndef _DEBUG  // version debug dans TAMGUIView.cpp
inline CTAMGUIDoc* CTAMGUIView::GetDocument() const
   { return reinterpret_cast<CTAMGUIDoc*>(m_pDocument); }
#endif

