/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguxmldoc.cxx
 Date       : 2017/09/01
 Purpose    : xml document management (based on libxml2)
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamguversion.h"
#include "tamguxmldoc.h"
#include "tamguvector.h"
#include "tamguxml.h"
#include "instructions.h"

ThreadLock _xmlDocLock;

static bool atanxmlStop = false;

void startElementNs(void *ctx,
	const xmlChar *localname,
	const xmlChar *prefix,
	const xmlChar *URI,
	int nb_namespaces,
	const xmlChar **namespaces,
	int nb_attributes,
	int nb_defaulted,
	const xmlChar **attributes) {

	if (atanxmlStop)
		return;

	Tamguxml* knode;
	Tamgu* func;
	Tamguxmldoc* xmldoc;
	{
		Locking _lock(_xmlDocLock);
		xmlSAX2StartElementNs(ctx, localname, prefix, URI, nb_namespaces, namespaces, nb_attributes, nb_defaulted, attributes);
		xmlParserCtxtPtr x = (xmlParserCtxtPtr)ctx;
		xmldoc = (Tamguxmldoc*)x->_private;

		if (xmldoc->stop || globalTamgu->Error(0)) {
			xmldoc->stop = true;
			return;
		}
		func = xmldoc->function;
		knode = new Tamguxml(globalTamgu, x->node);
		if (x->node != NULL && x->node->_private == NULL)
			x->node->_private = (void*)xmldoc->idnode++;
	}

	if (func != NULL) {
		TamguCallFunction kcf(func);
		knode->Setreference();
		kcf.arguments.push_back(knode);
		kcf.arguments.push_back(xmldoc->object);
		kcf.Eval(aNULL, aNULL, 0);
		knode->Resetreference();
	}
}

void endElementNs(void *ctx,
	const xmlChar * name,
	const xmlChar * prefix,
	const xmlChar * URI) {


	if (atanxmlStop)
		return;

	Tamguxml* knode;
	Tamgu* func;
	Tamguxmldoc* xmldoc;

	{
		Locking _lock(_xmlDocLock);
		xmlParserCtxtPtr x = (xmlParserCtxtPtr)ctx;
		xmlSAX2EndElementNs(ctx, name, prefix, URI);
		xmldoc = (Tamguxmldoc*)x->_private;

		if (xmldoc->stop || globalTamgu->Error(0)) {
			xmldoc->stop = true;
			return;
		}

		func = xmldoc->functionend;
		knode = new Tamguxml(globalTamgu, x->node);
		if (x->node != NULL && x->node->_private == NULL)
			x->node->_private = (void*)xmldoc->idnode++;
	}

	TamguCallFunction kcf(func);
	knode->Setreference();
	kcf.arguments.push_back(knode);
	kcf.arguments.push_back(xmldoc->objectend);
	kcf.Eval(aNULL, aNULL, 0);
	knode->Resetreference();
}

extern "C" {
	int xmlXPathRegisterNs(xmlXPathContextPtr ctxt, const xmlChar *prefix, const xmlChar *ns_uri);
}

static void XmlCleaningDoc(xmlDocPtr dx) {
	Tamgu* a;
    atomic_vector_iterator<Tamgu*> it(globalTamgu->tracked);
    while (!it.end()) {
        a = it.second;
		if (a != NULL && a->Type() == Tamguxml::idtype) {
			Tamguxml* kxml = (Tamguxml*)a;
            if (kxml->node == NULL) {
                it.next();
				continue;
            }

			if (kxml->node->doc == dx)
				kxml->node = NULL;
		}
        it.next();
	}
}

//We need to declare once again our local definitions.
basebin_hash<xmldocMethod>  Tamguxmldoc::methods;
hmap<string, string> Tamguxmldoc::infomethods;
bin_hash<unsigned long> Tamguxmldoc::exported;

short Tamguxmldoc::idtype = 0;


    //------------------------------------------------------------------------------------------------------------------
    //If you want to implement an external library... Otherwise remove it...
extern "C" {
    Exporting bool xml_InitialisationModule(TamguGlobal* global, string version) {
        if (version != TamguVersion())
            return false;
        
            //Global variables are quite complex to export to dlls
        global->Update();
        
        Tamguxmldoc::InitialisationModule(global, TamguVersion());
        return Tamguxml::InitialisationModule(global, TamguVersion());
    }
}


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguxmldoc::AddMethod(TamguGlobal* global, string name, xmldocMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Tamguxmldoc::InitialisationModule(TamguGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Tamguxmldoc::idtype = global->Getid("xmldoc");

	Tamguxmldoc::AddMethod(global, "_initial", &Tamguxmldoc::MethodInitial, P_ONE, "_initial(variable): Interception of a callback object");

	Tamguxmldoc::AddMethod(global, "load", &Tamguxmldoc::MethodLoadXML, P_ONE, "load(string filename): load an xml file");
	Tamguxmldoc::AddMethod(global, "onclosing", &Tamguxmldoc::MethodOnClosing, P_TWO, "onclosing(functionobject): function to call on closing markup tag");
	Tamguxmldoc::AddMethod(global, "parse", &Tamguxmldoc::MethodLoadXMLBuffer, P_ONE, "parse(string buffer): parse an xml buffer");
	Tamguxmldoc::AddMethod(global, "create", &Tamguxmldoc::MethodCreate, P_ONE, "create(string topnode): create an XML file with topnode as the top node or with topnode if topdenode is a full XML structure");
	Tamguxmldoc::AddMethod(global, "xpath", &Tamguxmldoc::MethodXPath, P_ONE, "xpath(string myxpath): Return a vector of xml nodes matching myxpath");
	Tamguxmldoc::AddMethod(global, "save", &Tamguxmldoc::MethodSave, P_ONE | P_TWO, "save(string filename, string encoding): save an XML file. Encoding default is 'utf8'");
	Tamguxmldoc::AddMethod(global, "close", &Tamguxmldoc::MethodClose, P_NONE, "close(): close an xml file");
	Tamguxmldoc::AddMethod(global, "node", &Tamguxmldoc::MethodNode, P_NONE, "node(): return the top node of the file");
	Tamguxmldoc::AddMethod(global, "xmlstring", &Tamguxmldoc::MethodxmlEncoding, P_NONE, "xmlstring(): Return an XML document as a string");
	Tamguxmldoc::AddMethod(global, "serialize", &Tamguxmldoc::MethodSerialize, P_ONE, "serialize(object): Serialize as an XML document any Tamgu object");
	Tamguxmldoc::AddMethod(global, "serializestring", &Tamguxmldoc::MethodSerializeString, P_ONE, "serialize, string(object): Serialize as an XML document any Tamgu object and return the corresponding string");


	global->newInstance[Tamguxmldoc::idtype] = new Tamguxmldoc(global, NULL);
	global->RecordMethods(Tamguxmldoc::idtype, Tamguxmldoc::exported);

	return true;
}



Tamgu* Tamguxmldoc::Put(Tamgu* index, Tamgu* kval, short idthread) {
	if (doc != NULL) {
		Locking _lock(_xmlDocLock);
		XmlCleaningDoc(doc);
		xmlFreeDoc(doc);
		doc = NULL;
	}
	if (sax != NULL)
		xmlFree(sax);
	sax = NULL;
	filename = "";

	string topnode = kval->String();
	stop = false;

	if (topnode.size() > 4 && topnode[0] == '<' && topnode[topnode.size() - 1] == '>')
		doc = xmlSAXParseMemory(NULL, STR(topnode), (int)topnode.size(), 0);

	return aTRUE;
}

Tamgu* Tamguxmldoc::Eval(Tamgu* context, Tamgu* index, short idthread) {
	return this;
}


Tamgu* Tamguxmldoc::MethodOnClosing(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	functionend = callfunc->Evaluate(0, aNULL, idthread);
	if (!functionend->isFunction())
		return globalTamgu->Returnerror("XML(211): Expecting a function", idthread);
	objectend = callfunc->arguments[1];
	if (!objectend->isCallVariable())
		objectend = objectend->Eval(aNULL, aNULL, idthread);
	return aTRUE;
}

Tamgu* Tamguxmldoc::MethodCreate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (doc != NULL)
		return globalTamgu->Returnerror("XML(234): Already created", idthread);
	string topnode = callfunc->Evaluate(0, aNULL, idthread)->String();
	stop = false;
	if (topnode.size() > 4 && topnode[0] == '<' && topnode[topnode.size() - 1] == '>')
		doc = xmlSAXParseMemory(NULL, STR(topnode), (int)topnode.size(), 0);
	else {
		string buf = "<";
		buf += topnode;
		buf += "/>";
		doc = xmlSAXParseMemory(NULL, STR(buf), (int)buf.size(), 0);
	}
	return aTRUE;
}

Tamgu* Tamguxmldoc::MethodXPath(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (doc == NULL)
		return globalTamgu->Returnerror("XML(235): Empty doc", idthread);
	xmlNodePtr nn;
	if (xpathcontext == NULL) {
		xpathcontext = xmlXPathNewContext(doc);
		nn = doc->children;
		xmlNsPtr cur = NULL;
		while (nn != NULL && nn->type != XML_ELEMENT_NODE)
			nn = nn->next;
		if (nn != NULL)
			cur = nn->nsDef;
		//pour le contexte xpath a partir du namespace du document
		while (cur != NULL) {
			xmlXPathRegisterNs(xpathcontext, cur->prefix, cur->href);
			cur = cur->next;
		}
	}
	xmlXPathObjectPtr nodeList;
	string theXPath = callfunc->Evaluate(0, aNULL, idthread)->String();
	nodeList = xmlXPathEval((xmlChar*)theXPath.c_str(), xpathcontext);
	Tamgu* kvect = Selectavector(contextualpattern);
	if (nodeList != NULL) {
		int nbelements = 0;
		if (nodeList->nodesetval != NULL)
			nbelements = nodeList->nodesetval->nodeNr;
		Tamguxml* xml;
		//On jette un coup d'oeil sur la structure construite en memoire
		for (int j = 0; j < nbelements; j++) {
			nn = nodeList->nodesetval->nodeTab[j];
			xml = new Tamguxml(globalTamgu, nn);
			kvect->Push(xml);
		}
		xmlXPathFreeObject(nodeList);
	}
	return kvect;
}

Tamgu* Tamguxmldoc::MethodSave(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (doc == NULL)
		return globalTamgu->Returnerror("XML(235): Empty doc", idthread);
	string encoding("UTF-8");
	if (callfunc->Size() == 2)
		encoding = callfunc->Evaluate(1, aNULL, idthread)->String();
	string filename = callfunc->Evaluate(0, aNULL, idthread)->String();
	xmlKeepBlanksDefault(0);
	xmlSaveFormatFileEnc(STR(filename), doc, STR(encoding), 1);
	return aTRUE;
}

Tamgu* Tamguxmldoc::MethodClose(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (doc != NULL) {
		Locking _lock(_xmlDocLock);
		XmlCleaningDoc(doc);
		xmlFreeDoc(doc);
		doc = NULL;
	}
	if (sax != NULL)
		xmlFree(sax);
	object->Resetreference();
	sax = NULL;
	filename = "";
	return aTRUE;

}

Tamgu* Tamguxmldoc::MethodNode(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (doc != NULL)
		return new Tamguxml(globalTamgu, doc->children);

	return aNULL;
}

Tamgu* Tamguxmldoc::MethodxmlEncoding(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (doc == NULL)
		return globalTamgu->Returnerror("XML(235): Empty doc", idthread);
	xmlChar* buff = NULL;
	int longueur;
	xmlDocDumpMemoryEnc(doc, &buff, &longueur, "UTF-8");
	Tamgu* kstr = globalTamgu->Providestring((char*)buff);
	xmlFree(buff);
	return kstr;
}




Tamgu* Tamguxmldoc::MethodLoadXML(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (doc != NULL)
		return globalTamgu->Returnerror("XML(234): Already created", idthread);
	stop = false;
	atanxmlStop = false;
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	Tamgu* kname = callfunc->Evaluate(0, aNULL, idthread);
	filename = kname->String();
	strcpy_s(fname, 1024, STR(filename));
	if (function != NULL || functionend != NULL) {
		sax = (xmlSAXHandler *)xmlMalloc(sizeof(xmlSAXHandler));
		sax->initialized = 0;
		//initxmlDefaultSAXHandler(sax, xmlGetWarningsDefaultValue);       
		xmlSAX2InitDefaultSAXHandler(sax, xmlGetWarningsDefaultValue);
		sax->initialized = XML_SAX2_MAGIC;
		//you may return any value of course...
		if (function != NULL)
			sax->startElementNs = startElementNs;
		
		if (functionend != NULL)
			sax->endElementNs = endElementNs;
		if (atanxmlStop)
			return aNULL;
		//On conserve un pointeur sur l'objet GlobalParseur
		//Puis on analyse le document
		doc = xmlSAXParseFileWithData(sax, fname, 1, this);
	}
	else {
		sax = NULL;
		if (atanxmlStop)
			return aNULL;
		doc = xmlParseFile(fname);
	}
	if (doc == NULL)
		return globalTamgu->Returnerror("XML(102): Unknown file", idthread);
	return aTRUE;
}

xmlNodePtr Tamguxmldoc::buildnode(xmlNodePtr r, Tamgu* e) {
	xmlNodePtr sub;
	xmlChar* name;
	if (e->isVectorContainer()) {
		name = xmlCharStrdup("vector");
		sub = xmlNewNodeEatName(NULL, name);
		xmlAddChild(r, sub);
		TamguIteration* itr = e->Newiteration(false);
		for (itr->Begin(); itr->End() == aFALSE; itr->Next())
			buildnode(sub, itr->IteratorValue());
		itr->Release();
		return sub;
	}
	if (e->isMapContainer()) {
		xmlNodePtr n;
		name = xmlCharStrdup("map");
		sub = xmlNewNodeEatName(NULL, name);
		xmlAddChild(r, sub);
		TamguIteration* itr = e->Newiteration(false);
		string k;
		xmlChar ky[] = { 'k', 'e', 'y', 0 };
		for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
			k = itr->Keystring();
			n = buildnode(sub, itr->IteratorValue());
			xmlSetProp(n, ky, (xmlChar*)k.c_str());
		}
		itr->Release();
		return sub;
	}

	string stype = globalTamgu->Getsymbol(e->Type());
	string v = e->String();
	name = xmlCharStrdup(stype.c_str());
	sub = xmlNewNodeEatName(NULL, name);
	xmlAddChild(r, sub);
	xmlNodePtr txt = xmlNewDocText(doc, (xmlChar*)v.c_str());
	xmlAddChild(sub, txt);
	return sub;
}


Tamgu* Tamguxmldoc::MethodSerialize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//first we clean everything, to replace it with the new structure...
	if (doc != NULL) {
		Locking _lock(_xmlDocLock);

		XmlCleaningDoc(doc);
		xmlFreeDoc(doc);
		doc = NULL;
	}
	if (sax != NULL)
		xmlFree(sax);
	sax = NULL;
	filename = "";
	string buf;
	Tamgu* atanobject = callfunc->arguments[0]; //would match i as in the example above: mynewtype toto(i,j);
	if (atanobject->isCallVariable()) {
		string name = globalTamgu->Getsymbol(atanobject->Name());
		buf = "<atan name=\"" + name + "\"/>";
	}
	else {
		atanobject = atanobject->Eval(aNULL, aNULL, idthread);
		buf = "<atan/>";
	}
	stop = false;
	doc = xmlSAXParseMemory(NULL, STR(buf), (int)buf.size(), 0);
	buildnode(doc->children, atanobject->Eval(aNULL, aNULL, idthread));
	return aTRUE;
}

Tamgu* Tamguxmldoc::MethodSerializeString(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//first we clean everything, to replace it with the new structure...
	if (doc != NULL) {
		Locking _lock(_xmlDocLock);
		XmlCleaningDoc(doc);
		xmlFreeDoc(doc);
		doc = NULL;
	}
	if (sax != NULL)
		xmlFree(sax);
	sax = NULL;
	filename = "";

	Tamgu* atanobject = callfunc->arguments[0]; //would match i as in the example above: mynewtype toto(i,j);
	if (!atanobject->isCallVariable())
		return globalTamgu->Returnerror("Expecting a variable", idthread);
	string name = globalTamgu->Getsymbol(atanobject->Name());
	string buf = "<atan name=\"" + name + "\"/>";

	stop = false;
	doc = xmlSAXParseMemory(NULL, STR(buf), (int)buf.size(), 0);
	buildnode(doc->children, atanobject->Eval(aNULL, aNULL, idthread));
	xmlChar* buff = NULL;
	int longueur;
	xmlDocDumpMemoryEnc(doc, &buff, &longueur, "UTF-8");
	atanobject = globalTamgu->Providestring((char*)buff);
	xmlFree(buff);
	if (doc != NULL) {
		Locking _lock(_xmlDocLock);
		XmlCleaningDoc(doc);
		xmlFreeDoc(doc);
		doc = NULL;
	}
	if (sax != NULL)
		xmlFree(sax);
	sax = NULL;
	return atanobject;
}

Tamgu* Tamguxmldoc::MethodLoadXMLBuffer(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (doc != NULL)
		return globalTamgu->Returnerror("XML(234): Already created", idthread);
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	stop = false;
	atanxmlStop = false;
	Tamgu* kbuffer = callfunc->Evaluate(0, aNULL, idthread);
	string buff = kbuffer->String();
	if (function != NULL || functionend != NULL) {
		sax = (xmlSAXHandler *)xmlMalloc(sizeof(xmlSAXHandler));
		sax->initialized = 0;
		//initxmlDefaultSAXHandler(sax, xmlGetWarningsDefaultValue);       
		xmlSAX2InitDefaultSAXHandler(sax, xmlGetWarningsDefaultValue);
		//you may return any value of course...
		if (function != NULL) {
			sax->startElementNs = startElementNs;
		}
		if (functionend != NULL) {
			sax->endElementNs = endElementNs;
		}
		if (atanxmlStop)
			return aNULL;
		//On conserve un pointeur sur l'objet GlobalParseur
		//Puis on analyse le document
		doc = xmlSAXParseMemoryWithData(sax, STR(buff), (int)buff.size(), 1, this);
	}
	else {
		sax = NULL;
		if (atanxmlStop)
			return aNULL;
		doc = xmlSAXParseMemory(NULL, STR(buff), (int)buff.size(), 1);
	}
	if (doc == NULL)
		return globalTamgu->Returnerror("XML(106): Not an XML buffer", idthread);
	return aTRUE;
}





