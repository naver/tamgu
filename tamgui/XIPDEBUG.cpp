/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : XIPDEBUG.cpp
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

// XIPDEBUG.cpp : implementation file
//

#include "stdafx.h"
#include "TAMGUI.h"
#include "XIPDEBUG.h"
#include "GRAMMARFILES.h"
#include "afxdialogex.h"
#include "pasofficiel.h"
#include <map>
#include "conversion.h"
extern "C" {
#include "tamguextern.h"
}

#define WCSTR(x) (wchar_t*)LPCWSTR(x)

vector<string> feature_pathnames;
vector<string> lexicon_pathnames;
vector<string> rule_pathnames;
vector<string> param_pathnames;
vector<string> scripts_pathnames;
vector<string> divers_pathnames;
string languageFilename;
string grmFilename;

void fromstring(CString&,string res);
Exportation GlobalParseur* XipGetParser(unsigned int iParseur);

void XIPDebug(int parseurCourant,XipResult* xr,void* data)  {	
	XIPDEBUG* xdeb=(XIPDEBUG*)data;
	xdeb->xipsent=xr;
}

// XIPDEBUG dialog

IMPLEMENT_DYNAMIC(XIPDEBUG, CDialogEx)


XIPDEBUG::XIPDEBUG(CWnd* pParent /*=NULL*/)
	: CDialogEx(XIPDEBUG::IDD, pParent)
	, currentrule(_T(""))
	, rulenumber(_T(""))
	, ruleslider(100)
	, position(0)
	, laphrase(_T(""))
{
}



XIPDEBUG::~XIPDEBUG()
{
}

void XIPDEBUG::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, currentrule);
	DDX_Text(pDX, IDC_EDIT3, rulenumber);
	DDX_Control(pDX, IDC_TREE1, treenode);
	DDX_Slider(pDX, IDC_SLIDER1, ruleslider);
	DDX_Control(pDX, IDC_Dependencies, thedependencies);
	DDX_Text(pDX, IDC_phrase, laphrase);
}


BEGIN_MESSAGE_MAP(XIPDEBUG, CDialogEx)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, &XIPDEBUG::OnNMReleasedcaptureSlider1)
	ON_BN_CLICKED(IDC_BUTTON1, &XIPDEBUG::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &XIPDEBUG::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &XIPDEBUG::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &XIPDEBUG::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &XIPDEBUG::OnBnClickedButton5)
END_MESSAGE_MAP()


// XIPDEBUG message handlers
BOOL XIPDEBUG::OnInitDialog() {
        
    XipReturnRules(parseurCourant,rulenumbers);
	labels=XipReturnInfoRuleIndex(parseurCourant);
	XipSetCallBack(parseurCourant,XIPDebug,this);
	if (XipTestFlagDisplayMode(parseurCourant,DISPLAY_GENERATION)) {
		currentstring="";
		unsigned char* buf=XipGetCurrentInternalStringBuffer(parseurCourant);
		if (buf!=NULL)
			currentstring=(char*)buf;
	}
	else
		currentstring=XipCurrentSentence(parseurCourant);
	//if (XipGetCurrentInternalStringBuffer(parseurCourant)!=NULL)
	//	currentstring=(char*)XipGetCurrentInternalStringBuffer(parseurCourant);
	fromstring(laphrase,currentstring);	
	position=rulenumbers.size()-1;
	if (position>=0) {
		char ch[10];
		sprintf(ch,"%d",rulenumbers[position]);
		rulenumber=ch;
	}
	else
		rulenumber="-1";
	BOOL v=CDialogEx::OnInitDialog();
    //Construction recursive
    HTREEITEM racine;	
	HTREEITEM rdependance;

	CString val(xipsent->sentences[0]->root->category.c_str());
    racine=treenode.InsertItem(val);    
    treenode.Expand(racine,TVE_EXPAND);
	Onseerules();
	currentindex=0;
	val="Dependencies";
	rdependance=thedependencies.InsertItem(val);
	
    ConstructTree(treenode,xipsent->sentences[0]->root,racine);
	for (int i=0;i<xipsent->sentences[0]->dependencies.size();i++)
		ConstructDependency(xipsent->sentences[0]->dependencies[i],rdependance);
	thedependencies.Expand(rdependance,TVE_EXPAND);
	return v;
}

void XIPDEBUG::Recreate() {
	XipCleanCurrentFullXipResult(parseurCourant);
	XipSetRuleLimit(parseurCourant,rulenumbers[position]);
	if (XipTestFlagDisplayMode(parseurCourant,DISPLAY_GENERATION)) {
		if (currentstring.size()) {
			XipGenerateFromStringXipResult(currentstring,xipsent,parseurCourant,NULL);
			string generee=XipLastGeneratedString(parseurCourant);
			fromstring(laphrase,generee);	
		}
	}
	else
		XipParseString(currentstring,parseurCourant,XipGetCurrentXipResult(parseurCourant),1);

    treenode.DeleteAllItems();
	thedependencies.DeleteAllItems();
	treenode.SetRedraw(TRUE);
	thedependencies.SetRedraw(TRUE);

    HTREEITEM racine;
	HTREEITEM rdependance;
    CString val(xipsent->sentences[0]->root->category.c_str());
    racine=treenode.InsertItem(val);    
    treenode.Expand(racine,TVE_EXPAND);
	Onseerules();
	currentindex=0;
    ConstructTree(treenode,xipsent->sentences[0]->root,racine);
	val="Dependencies";	
	rdependance=thedependencies.InsertItem(val);
	for (int i=0;i<xipsent->sentences[0]->dependencies.size();i++)
		ConstructDependency(xipsent->sentences[0]->dependencies[i],rdependance);
	thedependencies.Expand(rdependance,TVE_EXPAND);	
}

void XIPDEBUG::LeafAsFeature(CTreeCtrl& tree,XipLeaf* leaf,HTREEITEM racine) {        
    CString surf(leaf->surface.c_str());
    //First we look for <features> node
    HTREEITEM feat=NULL;
    HTREEITEM hItem;
    HTREEITEM element;
    
    
    //We loop to find if there is such a node attached to racine
    if (treenode.ItemHasChildren(racine)) {
        hItem = treenode.GetChildItem(racine);
        
        while (hItem != NULL) {
            if (treenode.GetItemText(hItem)=="<features>") {
                feat=hItem;
                break;
            }
            hItem = treenode.GetNextSiblingItem(hItem);
        }
    }
    
    //If there no such node, we create it
    if (feat==NULL) {
		CString fts("<features>");
        feat=tree.InsertItem(fts,racine);                           
	}
    
    
    CString valsl("SURFACE");
    valsl+=":\"";
    valsl+=leaf->surface.c_str();
    valsl+="\"";
    BOOL found=FALSE;
    //We loop to find if there is already a surface node attached to features
    if (treenode.ItemHasChildren(feat)) {
        hItem = treenode.GetChildItem(feat);
        
        while (hItem != NULL) {
            if (treenode.GetItemText(hItem)==valsl) {
                found=TRUE;
                break;
            }
            hItem = treenode.GetNextSiblingItem(hItem);
        }
    }
    if (found==FALSE)
        element=tree.InsertItem(valsl,feat);                            
    
    valsl="LEMMA";
    valsl+=":\"";
    valsl+=leaf->lemma.c_str();
    valsl+="\"";
    found=FALSE;
    //We loop to find if there is already a surface node attached to features
    if (treenode.ItemHasChildren(feat)) {
        hItem = treenode.GetChildItem(feat);
        
        while (hItem != NULL) {
            if (treenode.GetItemText(hItem)==valsl) {
                found=TRUE;
                break;
            }
            hItem = treenode.GetNextSiblingItem(hItem);
        }
    }
    if (found==FALSE) {
        element=tree.InsertItem(valsl,feat);            
    }
}


void XIPDEBUG::LeafAsNode(CTreeCtrl& tree,XipLeaf* leaf,HTREEITEM racine) {        

    LeafAsFeature(tree,leaf,racine);

    CString surf(leaf->surface.c_str());
    //First we look for <features> node
    HTREEITEM feat=NULL;
    HTREEITEM catitem=NULL;
    HTREEITEM hItem;
    HTREEITEM element;
    
    CString cat(leaf->category.c_str());
	char ch[100];
	sprintf(ch,":%d(reading)",leaf->index);
    cat+=ch;
    int i;

    //First we try to find a node with the same category
    if (treenode.ItemHasChildren(racine)) {
        hItem = treenode.GetChildItem(racine);
        
        while (hItem != NULL) {
            if (treenode.GetItemText(hItem)==cat) {
                catitem=hItem;
                break;
            }
            hItem = treenode.GetNextSiblingItem(hItem);
        }
    }

	catitem=NULL;
    //If this category does not exist, we do create it
    if (catitem==NULL) {
        catitem=tree.InsertItem(cat,racine);
    }
    
    //Then we add the features    
    if (leaf->features.size()>0 && tree==treenode) {
        for (i=0;i<leaf->features.size();i++) {            
            CString att(leaf->features[i]->attribute.c_str());            
            att+=":";
            att+=leaf->features[i]->value.c_str();        
            feat=tree.InsertItem(att,catitem);                 
        }
    }
       
    CString valsl("LEMMA");
    valsl+=":\"";
    valsl+=leaf->lemma.c_str();
    valsl+="\"";
    BOOL found=FALSE;
    //We loop to find if there is already a surface node attached to features
    if (treenode.ItemHasChildren(catitem)) {
        hItem = treenode.GetChildItem(catitem);
        
        while (hItem != NULL) {
            if (treenode.GetItemText(hItem)==valsl) {
                found=TRUE;
                break;
            }
            hItem = treenode.GetNextSiblingItem(hItem);
        }
    }
    if (found==FALSE) {
        element=tree.InsertItem(valsl,catitem);            
    }  
}



void XIPDEBUG::ConstructTree(CTreeCtrl& tree,XipNode* node,HTREEITEM racine) {    
    //We add the features
    int i;
    HTREEITEM element;
    HTREEITEM feat;
	char ch[10];
    
    if (node->Type()==XIPNODE) {                
        if (node->features.size()>0) {
			CString fts("<features>");
            element=tree.InsertItem(fts,racine);                    

            for (i=0;i<node->features.size();i++) {
                CString att(node->features[i]->attribute.c_str()); 
                att+=":";
                att+=node->features[i]->value.c_str();

                feat=tree.InsertItem(att,element);                 
            }
        }

        if (tree==treenode && node->daughters.size()!=0)
            treenode.Expand(racine,TVE_EXPAND);		
        for (i=0;i<node->daughters.size();i++) {
			node->daughters[i]->index=currentindex++;
            if (node->daughters[i]->Type()==XIPNODE) {
                sprintf(ch,":%d",node->daughters[i]->index);
                CString val(node->daughters[i]->category.c_str()); 
				val+=ch;
                if (node->daughters[i]->daughters.size()>0 &&
                    node->daughters[i]->daughters[0]->Type()==XIPLEAF) {
                    val+="_";
                    val+=((XipLeaf*)node->daughters[i]->daughters[0])->surface.c_str();
                }
                element=tree.InsertItem(val,racine);                
                
            }
            else {
                element=racine;
            }
            ConstructTree(tree,node->daughters[i],element);
        }
    }
	else
		LeafAsNode(tree,(XipLeaf*)node,racine);
}

void XIPDEBUG::ConstructDependency(XipDependency* dep,HTREEITEM racine) {
	CString dependence(dep->name.c_str());
	int i;

    dependence+="(";
	char ch[10];
    for (i=0;i<dep->parameters.size();i++) {
		if (i)
			dependence+=",";
        XipNode* nodex=dep->parameters[i];
        CString val(nodex->category.c_str());        
		sprintf(ch,":%d",nodex->index);
		val+=ch;
        if (nodex->daughters[0]->Type()==XIPLEAF) {
            val+="_";
            val+=((XipLeaf*)nodex->daughters[0])->surface.c_str();
        }
		dependence+=val;
    }
	
	dependence+=")";
	HTREEITEM element=thedependencies.InsertItem(dependence,racine);

	if (dep->features.size()>0) {
        for (i=0;i<dep->features.size();i++) {
            CString att(dep->features[i]->attribute.c_str());
            att+=":";
            att+=dep->features[i]->value.c_str();
			if (att[0]=='#')
				att=att.Right(att.GetLength()-1);
			thedependencies.InsertItem(att,element);
        }
    }
}


void XIPDEBUG::OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
    UpdateData(TRUE);    
    float max=rulenumbers.size()*ruleslider/100;    
	if ((int)max>=rulenumbers.size())
		position=rulenumbers.size()-1;
	else
		position=(int)max;
	char ch[10];
	sprintf(ch,"%d",rulenumbers[position]);
	rulenumber=ch;	
	Recreate();
    UpdateData(FALSE);
	*pResult = 0;

}

void XIPDEBUG::Onseerules() 
{
	// TODO: Add your control notification handler code here
	//first we loop on the activated rules
	if (XipIsGrammarCrypted(parseurCourant)==1)
		return;

	if (position>=0 && position<rulenumbers.size()) {
		ostringstream os;

		XipDisplayActivatedRules(parseurCourant,&os,rulenumbers[position], 5);

		currentrule="";
		int l=os.str().size();

		for (int j=0;j<l;j++) {
			if (os.str()[j]=='\n' && os.str()[j-1]!='\r')
				currentrule+="\r";
			currentrule+=os.str()[j];
		}
	}
	UpdateData(FALSE);   
}


void XIPDEBUG::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	position--;
	if (position<0) {
		ruleslider=0;
		position=0;
	}
	else
		ruleslider=(position*100)/rulenumbers.size();
	char ch[10];
	sprintf(ch,"%d",rulenumbers[position]);
	rulenumber=ch;
	Recreate();
    UpdateData(FALSE);
}


void XIPDEBUG::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	position++;
	if (position>=rulenumbers.size()) {
		position=rulenumbers.size()-1;
		ruleslider=100;		
	}
	else
		ruleslider=(position*100)/rulenumbers.size();
	char ch[10];
	sprintf(ch,"%d",rulenumbers[position]);
	rulenumber=ch;
	Recreate();
    UpdateData(FALSE);
}



void OuvrirDocument(string nom,long p,CString cr);
void XIPDEBUG::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
	if (XipIsGrammarCrypted(parseurCourant)==1)
		return;
	if (position>=rulenumbers.size())
		return;

	Label* la=(*labels)[rulenumbers[position]];
	if (la==NULL)
		return;

	string nom=la->dr->nom_fichier;
	OuvrirDocument(nom,la->dr->ligne,currentrule);
	//No more debugging...
	XipSetCallBack(parseurCourant,NULL,NULL);
	//arretprg();
	//CDialogEx::OnOK();
}

void fromCString(string&,CString l);

void XIPDEBUG::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
    UpdateData(TRUE);
	string s;
	fromCString(s,rulenumber);
	int num=atoi((char*)s.c_str());
	int i;
	for (i=0;i<rulenumbers.size();i++) {
		if (num<=rulenumbers[i])
			break;
	}
	position=i;
	ruleslider=(position*100)/rulenumbers.size();
	Recreate();
    UpdateData(FALSE);
}



void XIPDEBUG::OnBnClickedButton5()
{
	// TODO: Add your control notification handler code here
	GRAMMARFILES grm;
	feature_pathnames.clear();
	lexicon_pathnames.clear();
	rule_pathnames.clear();
	param_pathnames.clear();
	divers_pathnames.clear();
	scripts_pathnames.clear();
	long offset_begins[3];	

	XipGrammarPathnamesFull(parseurCourant,
		grmFilename,
		languageFilename,
		feature_pathnames,
		offset_begins[0],
		lexicon_pathnames,
		offset_begins[1],
		rule_pathnames,
		offset_begins[2],
		param_pathnames,
		scripts_pathnames,
		divers_pathnames);
	
	grm.DoModal();
	CString cpath = grm.selectedpath;
	if (cpath != L"") {
		string path;
		s_unicode_to_utf8(path, WCSTR(cpath));
		OuvrirDocument(path, 0, L"");
	}
}
