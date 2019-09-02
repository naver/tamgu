/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguxml.cxx
 Date       : 2017/09/01
 Purpose    : xml nodes management (based on libxml2)
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamguxml.h"
#include "atoms.h"
#include "tamguprimemapss.h"
#include "tamguconstants.h"
#include "tamguxmldoc.h"

void XmlCleaningNode(xmlNodePtr x) {
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

			if (kxml->node == x)
				kxml->node = NULL;
		}
        it.next();
	}
}

void xmlnodeclear(xmlNodePtr n) {
	n->doc = NULL;
	n->ns = NULL;
	if (n->children != NULL)
		xmlnodeclear(n->children);
	if (n->next != NULL)
		xmlnodeclear(n->next);
}


//We need to declare once again our local definitions.
basebin_hash<xmlMethod>  Tamguxml::methods;
hmap<string, string> Tamguxml::infomethods;
bin_hash<unsigned long> Tamguxml::exported;

short Tamguxml::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguxml::AddMethod(TamguGlobal* global, string name, xmlMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Tamguxml::InitialisationModule(TamguGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Tamguxml::idtype = global->Getid("xml");

	Tamguxml::AddMethod(global, "_initial", &Tamguxml::MethodNew, P_ONE, "_initial(string name): Create a new XML node");
	Tamguxml::AddMethod(global, "root", &Tamguxml::MethodRoot, P_NONE, "root(): return the root node");
	Tamguxml::AddMethod(global, "document", &Tamguxml::MethodDocument, P_NONE, "document(): return the xmldoc associated with this node");
	Tamguxml::AddMethod(global, "next", &Tamguxml::MethodNext, P_NONE | P_ONE, "next(): next xml node");
	Tamguxml::AddMethod(global, "unlink", &Tamguxml::MethodUnlink, P_NONE, "unlink(): remove a node from a tree structure");
	Tamguxml::AddMethod(global, "delete", &Tamguxml::MethodDelete, P_NONE, "delete(): delete the internal representation of the XML node");
	Tamguxml::AddMethod(global, "previous", &Tamguxml::MethodPrevious, P_NONE | P_ONE, "previous(): previous xml node");
	Tamguxml::AddMethod(global, "parent", &Tamguxml::MethodParent, P_NONE, "parent(): parent xml node");
	Tamguxml::AddMethod(global, "child", &Tamguxml::MethodChild, P_NONE | P_ONE, "child(): child xml node");
	Tamguxml::AddMethod(global, "name", &Tamguxml::MethodName, P_NONE | P_ONE, "name(): name node");
	Tamguxml::AddMethod(global, "line", &Tamguxml::MethodLine, P_NONE, "line(): return the line position of the node");
	Tamguxml::AddMethod(global, "id", &Tamguxml::MethodId, P_NONE, "id(): return the node id (only available with callback functions)");
	Tamguxml::AddMethod(global, "xmltype", &Tamguxml::MethodxmlType, P_NONE, "xmltype(): return the XML node type");
	Tamguxml::AddMethod(global, "properties", &Tamguxml::MethodProperties, P_NONE | P_ONE, "properties(): return the xml node properties");
	Tamguxml::AddMethod(global, "content", &Tamguxml::MethodContent, P_NONE | P_ONE, "content(): return the text content of an xml node");
	Tamguxml::AddMethod(global, "namespace", &Tamguxml::MethodNamespace, P_NONE, "namespace(): return the namespace vector of an xml node");
	Tamguxml::AddMethod(global, "new", &Tamguxml::MethodNew, P_ONE, "new(string name): Create a new XML node");
	Tamguxml::AddMethod(global, "xmlstring", &Tamguxml::Methodxmlstring, P_NONE, "xmlstring(): Return the whole tree from the current XML node as a string");



	global->newInstance[Tamguxml::idtype] = new Tamguxml(global, NULL);
	global->RecordMethods(Tamguxml::idtype,Tamguxml::exported);

	return true;
}



Tamgu* Tamguxml::Put(Tamgu* idx, Tamgu* kval, short idthread) {
	if (kval->isNULL()) {
		node = NULL;
		return aTRUE;
	}

	if (kval->Type() != idtype) {
		if (node != NULL && idx != NULL && !idx->isConst()) {
			string att = idx->String();
			string val = kval->String();
			xmlSetProp(node, USTR(att), USTR(val));
		}
		return aTRUE;
	}

	Tamguxml* xml = (Tamguxml*)kval;
	node = xml->node;
	return aTRUE;
}

Tamgu* Tamguxml::Eval(Tamgu* context, Tamgu* idx, short idthread) {
	if (node == NULL)
		return aNOELEMENT;
	if (idx != NULL && !idx->isConst()) {
		string att = idx->String();
		xmlAttrPtr propriete;
		if (node->properties == NULL)
			return aNOELEMENT;
		propriete = node->properties;
		string p;
		while (propriete != NULL) {
			p = (char*)propriete->name;
			if (att == p)
				return globalTamgu->Providestring((char*)propriete->children->content);
			propriete = propriete->next;
		}
		return aNOELEMENT;
	}
	return this;

}



Tamgu* Tamguxml::MethodRoot(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (node == NULL)
		return globalTamgu->Returnerror("XML(097): Empty node", idthread);
	return new Tamguxml(globalTamgu, node->doc->children);
}

Tamgu* Tamguxml::MethodNext(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (node == NULL)
		return globalTamgu->Returnerror("XML(097): Empty node", idthread);
	if (callfunc->Size() == 0) {
		if (node->next == NULL)
			return aNULL;
		return new Tamguxml(globalTamgu, node->next);
	}
	Tamgu* knode = callfunc->Evaluate(0, aNULL, idthread);
	if (knode->Type() != idtype)
		return globalTamgu->Returnerror("XML(098): Expecting an XML node", idthread);
	Tamguxml* xml = (Tamguxml*)knode;
	if (xml->node == NULL)
		return globalTamgu->Returnerror("XML(097): Empty node", idthread);
	xmlUnlinkNode(xml->node);
	xmlnodeclear(xml->node);
	xmlAddNextSibling(node, xml->node);
	return aTRUE;
}

Tamgu* Tamguxml::MethodUnlink(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (node == NULL)
		return globalTamgu->Returnerror("XML(097): Empty node", idthread);
	xmlUnlinkNode(node);
	xmlnodeclear(node);
	return aTRUE;
}

Tamgu* Tamguxml::MethodDelete(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (node == NULL)
		return globalTamgu->Returnerror("XML(097): Empty node", idthread);
	xmlUnlinkNode(node);
	xmlFreeNode(node);
	node = NULL;
	return aTRUE;
}

Tamgu* Tamguxml::MethodPrevious(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (node == NULL)
		return globalTamgu->Returnerror("XML(097): Empty node", idthread);
	if (callfunc->Size() == 0) {
		if (node->prev == NULL)
			return aNULL;
		return new Tamguxml(globalTamgu, node->prev);
	}
	Tamgu* knode = callfunc->Evaluate(0, aNULL, idthread);
	if (knode->Type() != idtype)
		return globalTamgu->Returnerror("XML(098): Expecting an XML node", idthread);
	Tamguxml* xml = (Tamguxml*)knode;
	if (xml->node == NULL)
		return globalTamgu->Returnerror("XML(097): Empty node", idthread);
	xmlUnlinkNode(xml->node);
	xmlnodeclear(xml->node);
	xmlAddPrevSibling(node, xml->node);
	return aTRUE;
}

Tamgu* Tamguxml::MethodParent(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (node == NULL)
		return globalTamgu->Returnerror("XML(097): Empty node", idthread);
	if (node->parent == NULL || node->parent->type == XML_DOCUMENT_NODE || node->parent->name == NULL)
		return aNULL;
	return new Tamguxml(globalTamgu, node->parent);
}

Tamgu* Tamguxml::MethodChild(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (node == NULL)
		return globalTamgu->Returnerror("XML(097): Empty node", idthread);
	if (callfunc->Size() == 0) {
		if (node->children == NULL)
			return aNULL;
		return new Tamguxml(globalTamgu, node->children);
	}
	Tamgu* knode = callfunc->Evaluate(0, aNULL, idthread);
	if (knode->Type() != idtype)
		return globalTamgu->Returnerror("XML(098): Expecting an XML node", idthread);
	Tamguxml* xml = (Tamguxml*)knode;
	if (xml->node == NULL)
		return globalTamgu->Returnerror("XML(097): Empty node", idthread);
	xmlUnlinkNode(xml->node);
	xmlnodeclear(xml->node);
	xmlAddChild(node, xml->node);
	return aTRUE;
}

Tamgu* Tamguxml::MethodName(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (node == NULL)
		return globalTamgu->Returnerror("XML(097): Empty node", idthread);
	if (callfunc->Size() == 0) {
		if (node->name == NULL)
			return aNULL;
		return globalTamgu->Providestring((char*)node->name);
	}
	string name = callfunc->Evaluate(0, aNULL, idthread)->String();
	xmlFree((xmlChar*)node->name);
	node->name = xmlCharStrdup((const char*)USTR(name));
	return aTRUE;
}

Tamgu* Tamguxml::MethodLine(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (node == NULL)
		return globalTamgu->Returnerror("XML(097): Empty node", idthread);
	return globalTamgu->Provideint(node->line);
}

Tamgu* Tamguxml::MethodId(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (node == NULL)
		return globalTamgu->Returnerror("XML(097): Empty node", idthread);
	return globalTamgu->Provideint((long)node->_private);
}

Tamgu* Tamguxml::MethodxmlType(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (node == NULL)
		return globalTamgu->Returnerror("XML(097): Empty node", idthread);
	return globalTamgu->Provideint(node->type);
}

Tamgu* Tamguxml::MethodProperties(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (node == NULL)
		return globalTamgu->Returnerror("XML(097): Empty node", idthread);
	if (callfunc->Size() == 0) {
		Tamguprimemapss* kmap = (Tamguprimemapss*)Selectaprimemapss(contextualpattern);
		xmlAttrPtr propriete;
		if (node->properties != NULL) {
			propriete = node->properties;
			while (propriete != NULL) {
				kmap->values[(char*)propriete->name] = (char*)propriete->children->content;
				propriete = propriete->next;
			}
		}
		return kmap;
	}
	Tamgu* km = callfunc->Evaluate(0, aNULL, idthread);
	if (!km->isMapContainer())
		return globalTamgu->Returnerror("XML(123): Expecting a map", idthread);
	xmlAttrPtr propriete;
	xmlAttrPtr base;
	//Cleaning existing properties
	if (node->properties != NULL) {
		propriete = node->properties;
		while (propriete != NULL) {
			base = propriete;
			propriete = propriete->next;
			xmlFreeProp(base);
		}
		node->properties = NULL;
	}
	string v;
	string k;
	TamguIteration* itr = km->Newiteration(false);
	for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
		k = itr->Keystring();
		v = itr->Valuestring();
		xmlSetProp(node, USTR(k), USTR(v));
	}
	itr->Release();
	return aTRUE;
}

Tamgu* Tamguxml::MethodContent(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	if (node == NULL)
		return globalTamgu->Returnerror("XML(097): Empty node", idthread);
	string content;
	if (callfunc->Size() == 0) {
		TraverseXML(node, content, true);
		return globalTamgu->Providestring(content);
	}
	content = callfunc->Evaluate(0, aNULL, idthread)->String();
	if (node->content == NULL) {
		xmlNodePtr nouveau = xmlNewDocText(node->doc, (xmlChar*)content.c_str());
		xmlAddChild(node, nouveau);
		return aTRUE;
	}
	xmlFree((xmlChar*)node->content);
	node->content = xmlStrdup((xmlChar*)content.c_str());
	return aTRUE;
}

Tamgu* Tamguxml::MethodNamespace(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	Tamgu* kvect = Selectasvector(contextualpattern);
	if (node != NULL && node->nsDef != NULL) {
		xmlNsPtr cur = node->nsDef;
		while (cur != NULL) {
			if (cur->href != NULL)
				kvect->storevalue((char*)cur->href);
			else
				kvect->storevalue("");
			kvect->storevalue((char*)cur->prefix);
			cur = cur->next;
		}
	}
	return kvect;
}

Tamgu* Tamguxml::MethodNew(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	string name = callfunc->Evaluate(0, aNULL, idthread)->String();
	xmlChar* n = xmlCharStrdup((const char*)name.c_str());
	node = xmlNewNodeEatName(NULL, n);
	return this;
}

Tamgu* Tamguxml::Methodxmlstring(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (node == NULL)
		return aNULL;
	xmlDocPtr doc = node->doc;
	xmlNodePtr topnode = doc->children;
	doc->children = node;
	xmlNodePtr thenext = node->next;
	node->next = NULL;
	xmlChar* buff = NULL;
	int longueur;
	xmlDocDumpMemoryEnc(doc, &buff, &longueur, "UTF-8");
	Tamgu* kstr = globalTamgu->Providestring((char*)buff);
	xmlFree(buff);
	doc->children = topnode;
	node->next = thenext;
	return kstr;
}


Tamgu* Tamguxml::MethodDocument(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (node == NULL)
		return globalTamgu->Returnerror("XML(097): Empty node", idthread);
	if (node->doc == NULL)
		return globalTamgu->Returnerror("XML(099): No document", idthread);
	Tamguxmldoc* xdoc = new Tamguxmldoc(node->doc);
	xdoc->filename = xdoc->doc->name;
	return xdoc;
}





