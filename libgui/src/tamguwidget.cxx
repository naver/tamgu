/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguwidget.cxx
 Date       : 2017/09/01
 Purpose    : mother class of all FLTK widgets
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "atoms.h"
#include "tamgumapsi.h"
#include "tamguwidget.h"

void flredraw(void* data) {
	widge* w = (widge*)data;
	if (Stopall() || globalTamgu->Error(w->idthread)) {
		delete w;
		return;
	}
	w->redraw();
	delete w;
}

Tamgu* Tamguwidget::MethodRedraw(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    static std::recursive_mutex _drawlock;
    
    if (Stopall())
		return aFALSE;

    
    Fl_Widget* wdg = Widget();
	//If we are in the main thread, well business as usual		
    if (wdg == NULL) {
        if (idthread)
            return aFALSE;
        
		return Returningerror("WND(678): Widget not initialized", idthread);
    }
    
	if (callfunc->Size() == 1)
		update = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
	else
		update = true;


    if (idthread) {
        _drawlock.lock();
		Fl::awake(flredraw, (void*)new widge(wdg, this, idtracker, idthread));
        _drawlock.unlock();
    }
	else
		wdg->redraw();
    
	return aTRUE;
}

Tamgu* Tamguwidget::MethodTextFont(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	Tamgu* f = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* sz = callfunc->Evaluate(1, contextualpattern, idthread);
	if (f->Type() == a_string) {
		string buf = f->String();
		if (fonts.find(buf) == fonts.end()) {
			buf = "WND(657): Unknown font code";
			return globalTamgu->Returnerror(buf, idthread);
		}
		fl_font(fonts[buf], sz->Integer());
	}
	else {
		int i = f->Integer();
		if (i >= fontmax)
			return Returningerror("WND(658): unknown font", idthread);
		fl_font(i, sz->Integer());
	}
	return aTRUE;
}

Tamgu* Tamguwidget::MethodRGBColor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	if (callfunc->Size() == 3) {
		Tamgu* r = callfunc->Evaluate(0, contextualpattern, idthread);
		Tamgu* g = callfunc->Evaluate(1, contextualpattern, idthread);
		Tamgu* b = callfunc->Evaluate(2, contextualpattern, idthread);
		return globalTamgu->Provideint(fl_rgb_color(r->Integer(), g->Integer(), b->Integer()));
	}
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	if (ke->isVectorContainer()) {
		if (ke->Size() != 3)
			return Returningerror("WND(245): Wrong number of elements in the color vector ", idthread);
		return globalTamgu->Provideint(fl_rgb_color(ke->getvalue(0)->Integer(), ke->getvalue(1)->Integer(), ke->getvalue(2)->Integer()));
	}
	Fl_Color color;
	if (ke->Type() == a_string) {
		//0 is the first parameter and so on...
		string buf = ke->String();
		if (colors.find(buf) == colors.end()) {
			buf = "WND(657): Unknown color code";
			return globalTamgu->Returnerror(buf, idthread);
		}
		color = colors[buf];
	}
	else
		color = ke->Integer();
	uchar r, g, b;
	Fl::get_color(color, r, g, b); // Fl_Color to RGB
    Tamguivector* kvect = globalTamgu->Provideivector();
	kvect->values.push_back(r);
	kvect->values.push_back(g);
	kvect->values.push_back(b);
	return kvect;
}

Tamgu* Tamguwidget::MethodShow(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	Fl_Widget* wdg = Widget();
	if (wdg == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	wdg->show();
	return aTRUE;
}

Tamgu* Tamguwidget::MethodFocus(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	Fl_Widget* wdg;
	//If we are in the main thread, well business as usual
	wdg = Widget();
	if (wdg == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	wdg->take_focus();
	return aTRUE;
}

Tamgu* Tamguwidget::MethodAlign(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	Fl_Widget* wdg = Widget();
	if (wdg == NULL)
		return Returningerror("WND(805): Widget not initialized", idthread);
	Tamgu* ke = callfunc->Evaluate(0, aNULL, idthread);
	if (ke->Type() == a_string) {
		string align = ke->String();
		if (aligns.find(align) == aligns.end()) {
			align = "WND(202): Unknown alignement";
			return globalTamgu->Returnerror(align, idthread);
		}
		wdg->align(aligns[align]);
	}
	else
		wdg->align(ke->Integer());
	return aTRUE;
}

Tamgu* Tamguwidget::MethodCreated(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() != NULL)
		return aTRUE;
	return aFALSE;
}

Tamgu* Tamguwidget::MethodCopy(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	string tocopy = callfunc->Evaluate(0, aNULL, idthread)->String();
	Fl::copy(STR(tocopy), tocopy.length(), 1);
	return aTRUE;
}

Tamgu* Tamguwidget::MethodPaste(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	Fl::paste(*Widget(), 1);
	return aTRUE;
}

Tamgu* Tamguwidget::MethodCoordinates(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {


	Fl_Widget* wdg = Widget();
	if (wdg == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	Tamgu* kvect;
	if (callfunc->Size() == 0) {
		kvect = Selectaivector(contextualpattern);
		kvect->storevalue((long)wdg->x());
		kvect->storevalue((long)wdg->y());
		kvect->storevalue((long)wdg->w());
		kvect->storevalue((long)wdg->h());
		return kvect;
	}

	int x, y, w, h;
	if (callfunc->Size() == 1) {
		Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
		if (ke->isVectorContainer() == false)
			return Returningerror("WND(904): Expecting a vector", idthread);
		kvect = ke;
		if (kvect->Size() != 4)
			return Returningerror("WND(905): Missing coordinates", idthread);
		x = kvect->getvalue(0)->Integer();
		y = kvect->getvalue(1)->Integer();
		w = kvect->getvalue(2)->Integer();
		h = kvect->getvalue(3)->Integer();
	}
	else {
		x = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
		y = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
		w = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
		h = callfunc->Evaluate(3, contextualpattern, idthread)->Integer();
	}
	if (x >= w || y >= h)
		return Returningerror("WND(905): Incoherent coordinates", idthread);
	wdg->resize(x, y, w, h);
	return aTRUE;
}

Tamgu* Tamguwidget::MethodSelectionColor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	Fl_Widget* wdg = Widget();
	if (wdg == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalTamgu->Provideint(wdg->selection_color());
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	Fl_Color color;
	if (ke->Type() == a_string) {
		//0 is the first parameter and so on...
		string buf = ke->String();
		if (colors.find(buf) == colors.end()) {
			buf = "WND(657): Unknown color code";
			return globalTamgu->Returnerror(buf, idthread);
		}
		color = colors[buf];
	}
	else
		color = ke->Integer();
	wdg->selection_color(color);
	return aTRUE;
}

Tamgu* Tamguwidget::MethodLabel(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	Fl_Widget* wdg = Widget();
	if (wdg == NULL)
		return Returningerror("WND(102): wdg not created", idthread);
	string label;
	if (callfunc->Size() == 0) {
		label = wdg->label();
		return globalTamgu->Providestring(label);
	}
	label = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	char* ltxt = (char*)malloc(label.size() + 1);
	memcpy(ltxt, STR(label), label.size());
	ltxt[label.size()] = 0;
	strcpy_s(ltxt, label.size() + 1, label.c_str());
	wdg->label(ltxt);
	return aTRUE;
}

Tamgu* Tamguwidget::MethodLabelType(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	Fl_Widget* wdg = Widget();
	if (wdg == NULL)
		return Returningerror("WND(102): wdg not created", idthread);
	if (callfunc->Size() == 0)
		return globalTamgu->Provideint(wdg->labeltype());
	Tamgu* labeltype = callfunc->Evaluate(0, contextualpattern, idthread);
	wdg->labeltype((Fl_Labeltype)labeltype->Integer());
	return aTRUE;
}

Tamgu* Tamguwidget::MethodLabelColor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	//In our example, we have only two parameters
	Fl_Widget* wdg = Widget();
	if (wdg == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalTamgu->Provideint(wdg->labelcolor());
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	if (ke->Type() == a_string) {
		//0 is the first parameter and so on...
		string shortcut = ke->String();
		if (colors.find(shortcut) == colors.end()) {
			shortcut = "WND(657): Unknown color code";
			return globalTamgu->Returnerror(shortcut, idthread);
		}
		wdg->labelcolor(colors[shortcut]);
	}
	else {
		unsigned int c = ke->Integer();
		wdg->labelcolor(c);
	}
	return aTRUE;
}

Tamgu* Tamguwidget::MethodLabelFont(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	Fl_Widget* wdg = Widget();
	if (wdg == NULL)
		return Returningerror("WND(102): wdg not created", idthread);
	if (callfunc->Size() == 0)
		return globalTamgu->Provideint(wdg->labelfont());
	Tamgu* labelfont;
	int font = 0;
	for (int i = 0; i < callfunc->Size(); i++) {
		labelfont = callfunc->Evaluate(i, contextualpattern, idthread);
		if (labelfont->Type() == a_string) {
			if (fonts.find(labelfont->String()) == fonts.end()) {
				string mes = "WND(765): Unknown font:";
				mes += labelfont->String();
				return globalTamgu->Returnerror(mes, idthread);
			}
			font = fonts[labelfont->String()];
		}
		else
			font = labelfont->Integer();
	}
	if (font >= fontmax)
		return Returningerror("WND(658): unknown font", idthread);
	wdg->labelfont(font);
	return aTRUE;
}

Tamgu* Tamguwidget::MethodLabelSize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	Fl_Widget* wdg = Widget();
	if (wdg == NULL)
		return Returningerror("WND(678): wdg not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalTamgu->Provideint(wdg->labelsize());
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	//0 is the first parameter and so on...
	wdg->labelsize(ke->Integer());
	return aTRUE;
}

Tamgu* Tamguwidget::MethodDrawText(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
#ifdef WIN32
	//On WIN32, if no font is given beforehand, the whole stuff crashes...
	if (fl_graphics_driver->font_descriptor() == NULL)
		fl_font(FL_HELVETICA, 12);
#endif
	Tamgu* t = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* x = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(2, contextualpattern, idthread);
	string buf = t->String();
	char* label = new char[buf.size() + 1];
	strcpy_s(label, buf.size() + 1, buf.c_str());
	fl_draw(label, x->Integer(), y->Integer());
	delete[] label;
	return aTRUE;
}

Tamgu* Tamguwidget::MethodDrawColor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	Fl_Color color;
	if (ke->Type() == a_string) {
		//0 is the first parameter and so on...
		string buf = ke->String();
		if (colors.find(buf) == colors.end()) {
			buf = "WND(657): Unknown color code";
			return globalTamgu->Returnerror(buf, idthread);
		}
		color = colors[buf];
	}
	else
		color = ke->Integer();
	fl_color(color);
	return aTRUE;
}

Tamgu* Tamguwidget::MethodRectangle(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* wx = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* hy = callfunc->Evaluate(3, contextualpattern, idthread);
	if (callfunc->Size() == 5) {
		Fl_Color color;
		Tamgu* ke = callfunc->Evaluate(4, contextualpattern, idthread);
		if (ke->Type() == a_string) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = "WND(657): Unknown color code";
				return globalTamgu->Returnerror(buf, idthread);
			}
			color = colors[buf];
		}
		else
			color = ke->Integer();
		fl_rect(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), color);
	}
	else
		fl_rect(x->Integer(), y->Integer(), wx->Integer(), hy->Integer());
	return aTRUE;
}

Tamgu* Tamguwidget::MethodVertex(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (!begins.size())
		return Returningerror("WND(610): You cannot use this function outside a complex shape construction", idthread);
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	fl_vertex(x->Float(), y->Float());
	return aTRUE;
}

Tamgu* Tamguwidget::MethodRectangleFill(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* wx = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* hy = callfunc->Evaluate(3, contextualpattern, idthread);
	if (callfunc->Size() == 5) {
		Fl_Color color;
		Tamgu* ke = callfunc->Evaluate(4, contextualpattern, idthread);
		if (ke->Type() == a_string) {
			//0 is the first parameter and so on...
			string buf = ke->String();
			if (colors.find(buf) == colors.end()) {
				buf = "WND(657): Unknown color code";
				return globalTamgu->Returnerror(buf, idthread);
			}
			color = colors[buf];
		}
		else
			color = ke->Integer();
		fl_rectf(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), color);
	}
	else
		fl_rectf(x->Integer(), y->Integer(), wx->Integer(), hy->Integer());
	return aTRUE;
}

Tamgu* Tamguwidget::MethodLineShape(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	int w = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
	//0 is the first parameter and so on...
	if (ke->Type() == a_string) {
		string buf = ke->String();
		if (lineshape.find(buf) == lineshape.end()) {
			buf = "WND(607): Unknown line shape code";
			return globalTamgu->Returnerror(buf, idthread);
		}
		fl_line_style(lineshape[buf], w);
	}
	else
		fl_line_style(ke->Integer(), w);
	return aTRUE;
}

Tamgu* Tamguwidget::MethodPolygon(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* x1 = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* y1 = callfunc->Evaluate(3, contextualpattern, idthread);
	Tamgu* x2 = callfunc->Evaluate(4, contextualpattern, idthread);
	Tamgu* y2 = callfunc->Evaluate(5, contextualpattern, idthread);
	if (callfunc->Size() == 6)
		fl_polygon(x->Integer(), y->Integer(), x1->Integer(), y1->Integer(), x2->Integer(), y2->Integer());
	else {
		Tamgu* x3 = callfunc->Evaluate(6, contextualpattern, idthread);
		Tamgu* y3 = callfunc->Evaluate(7, contextualpattern, idthread);
		fl_polygon(x->Integer(), y->Integer(), x1->Integer(), y1->Integer(), x2->Integer(), y2->Integer(), x3->Integer(), y3->Integer());
	}
	return aTRUE;
}

Tamgu* Tamguwidget::MethodLoop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* x1 = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* y1 = callfunc->Evaluate(3, contextualpattern, idthread);
	Tamgu* x2 = callfunc->Evaluate(4, contextualpattern, idthread);
	Tamgu* y2 = callfunc->Evaluate(5, contextualpattern, idthread);
	if (callfunc->Size() == 6)
		fl_loop(x->Integer(), y->Integer(), x1->Integer(), y1->Integer(), x2->Integer(), y2->Integer());
	else {
		Tamgu* x3 = callfunc->Evaluate(6, contextualpattern, idthread);
		Tamgu* y3 = callfunc->Evaluate(7, contextualpattern, idthread);
		fl_loop(x->Integer(), y->Integer(), x1->Integer(), y1->Integer(), x2->Integer(), y2->Integer(), x3->Integer(), y3->Integer());
	}
	return aTRUE;
}

Tamgu* Tamguwidget::MethodLine(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* x1 = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* y1 = callfunc->Evaluate(3, contextualpattern, idthread);
	if (callfunc->Size() == 4)
		fl_line(x->Integer(), y->Integer(), x1->Integer(), y1->Integer());
	else {
		Tamgu* x2 = callfunc->Evaluate(4, contextualpattern, idthread);
		Tamgu* y2 = callfunc->Evaluate(5, contextualpattern, idthread);
		fl_line(x->Integer(), y->Integer(), x1->Integer(), y1->Integer(), x2->Integer(), y2->Integer());
	}
	return aTRUE;
}

Tamgu* Tamguwidget::MethodLineRotation(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	double x = callfunc->Evaluate(0, contextualpattern, idthread)->Float();
	double y = callfunc->Evaluate(1, contextualpattern, idthread)->Float();
	double distance = callfunc->Evaluate(2, contextualpattern, idthread)->Float();
	double angle = callfunc->Evaluate(3, contextualpattern, idthread)->Float();
	bool draw = false;
	if (callfunc->Size() == 5)
		draw = callfunc->Evaluate(4, contextualpattern, idthread)->Boolean();
	double x1, y1;
	x1 = x + cos(angle)*distance;
	y1 = y - sin(angle)*distance;
	if (draw)
		fl_line((long)x, (long)y, (long)x1, (long)y1);
	Tamgu* kvect = Selectafvector(contextualpattern);
	kvect->storevalue(x1);
	kvect->storevalue(y1);
	return kvect;
}

Tamgu* Tamguwidget::MethodPushMatrix(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	begins.push_back(6);
	fl_push_matrix();
	return aTRUE;
}

Tamgu* Tamguwidget::MethodPopMatrix(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (!begins.size() || begins.back() != 6)
		return Returningerror("WND(612): Cannot call this method here. Have you called beginpoints before?", idthread);
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	begins.pop_back();
	fl_pop_matrix();
	return aTRUE;
}

Tamgu* Tamguwidget::MethodScale(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	if (callfunc->Size() == 2) {
		Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
		fl_scale(x->Float(), y->Float());
	}
	else
		fl_scale(x->Float());
	return aTRUE;
}

Tamgu* Tamguwidget::MethodTranslate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	fl_translate(x->Float(), y->Float());
	return aTRUE;
}

Tamgu* Tamguwidget::MethodRotate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	Tamgu* d = callfunc->Evaluate(0, contextualpattern, idthread);
	fl_rotate(d->Float());
	return aTRUE;
}

Tamgu* Tamguwidget::MethodMultmatrix(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	Tamgu* a = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* b = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* c = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* d = callfunc->Evaluate(3, contextualpattern, idthread);
	Tamgu* x = callfunc->Evaluate(4, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(5, contextualpattern, idthread);
	fl_mult_matrix(a->Float(), b->Float(), c->Float(), d->Float(), x->Float(), y->Float());
	return aTRUE;
}

Tamgu* Tamguwidget::MethodTransform_x(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	double v = fl_transform_x(x->Float(), y->Float());
	return globalTamgu->Providefloat(v);
}

Tamgu* Tamguwidget::MethodTransform_y(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	double v = fl_transform_y(x->Float(), y->Float());
	return globalTamgu->Providefloat(v);
}

Tamgu* Tamguwidget::MethodTransform_dx(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	double v = fl_transform_dx(x->Float(), y->Float());
	return globalTamgu->Providefloat(v);
}

Tamgu* Tamguwidget::MethodTransform_dy(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	double v = fl_transform_dy(x->Float(), y->Float());
	return globalTamgu->Providefloat(v);
}

Tamgu* Tamguwidget::MethodTransform_vertex(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	fl_transformed_vertex(x->Float(), y->Float());
	return aTRUE;
}

Tamgu* Tamguwidget::MethodBeginPoints(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	begins.push_back(1);
	fl_begin_points();
	return aTRUE;
}

Tamgu* Tamguwidget::MethodEndPoints(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (!begins.size() || begins.back() != 1)
		return Returningerror("WND(612): Cannot call this method here. Have you called beginpoints before?", idthread);
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	begins.pop_back();
	fl_end_points();
	return aTRUE;
}

Tamgu* Tamguwidget::MethodBeginLine(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	begins.push_back(5);
	fl_begin_line();
	return aTRUE;
}

Tamgu* Tamguwidget::MethodEndLine(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (!begins.size() || begins.back() != 5)
		return Returningerror("WND(612): Cannot call this method here. Have you called beginline before?", idthread);
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	begins.pop_back();
	fl_end_line();
	return aTRUE;
}

Tamgu* Tamguwidget::MethodBeginLoop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	begins.push_back(2);
	fl_begin_loop();
	return aTRUE;
}

Tamgu* Tamguwidget::MethodEndLoop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (!begins.size() || begins.back() != 2)
		return Returningerror("WND(612): Cannot call this method here. Have you called beginloop before?", idthread);
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	begins.pop_back();
	fl_end_loop();
	return aTRUE;
}

Tamgu* Tamguwidget::MethodBeginPolygon(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	begins.push_back(3);
	fl_begin_polygon();
	return aTRUE;
}

Tamgu* Tamguwidget::MethodEndPolygon(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (!begins.size() || begins.back() != 3)
		return Returningerror("WND(612): Cannot call this method here. Have you called beginpolygon before?", idthread);
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	begins.pop_back();
	fl_end_polygon();
	return aTRUE;
}

Tamgu* Tamguwidget::MethodBeginComplexPolygon(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	begins.push_back(4);
	fl_begin_complex_polygon();
	return aTRUE;
}

Tamgu* Tamguwidget::MethodGap(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (!begins.size() || begins.back() != 4)
		return Returningerror("WND(612): Cannot call this method here. Have you called begincomplexpolygon before?", idthread);
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	fl_gap();
	return aTRUE;
}

Tamgu* Tamguwidget::MethodEndComplexPolygon(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (!begins.size() || begins.back() != 4)
		return Returningerror("WND(612): Cannot call this method here. Have you called begincomplexpolygon before?", idthread);
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	begins.pop_back();
	fl_end_complex_polygon();
	return aTRUE;
}

Tamgu* Tamguwidget::MethodArc(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* w = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* h = callfunc->Evaluate(3, contextualpattern, idthread);
	Tamgu* a1 = callfunc->Evaluate(4, contextualpattern, idthread);
	if (begins.size() != 0) {
		if (callfunc->Size() == 6)
			return Returningerror("WND(608): Wrong number of parameters=expected 5 in a complex shape construction", idthread);
		fl_arc(x->Float(), y->Float(), w->Float(), h->Float(), a1->Float());
	}
	else {
		if (callfunc->Size() != 6)
			return Returningerror("WND(609): Wrong number of parameters=expected 6 in a regular construction", idthread);
		Tamgu* a2 = callfunc->Evaluate(5, contextualpattern, idthread);
		fl_arc(x->Integer(), y->Integer(), w->Integer(), h->Integer(), a1->Float(), a2->Float());
	}
	return aTRUE;
}

Tamgu* Tamguwidget::MethodCurve(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (!begins.size())
		return Returningerror("WND(610): You cannot use this function outside a complex shape construction", idthread);
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* x1 = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* y1 = callfunc->Evaluate(3, contextualpattern, idthread);
	Tamgu* x2 = callfunc->Evaluate(4, contextualpattern, idthread);
	Tamgu* y2 = callfunc->Evaluate(5, contextualpattern, idthread);
	Tamgu* x3 = callfunc->Evaluate(6, contextualpattern, idthread);
	Tamgu* y3 = callfunc->Evaluate(7, contextualpattern, idthread);
	fl_curve(x->Float(), y->Float(), x1->Float(), y1->Float(), x2->Float(), y2->Float(), x3->Float(), y3->Float());
	return aTRUE;
}

Tamgu* Tamguwidget::MethodPushClip(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	//0 is the first parameter and so on...
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* wx = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* hy = callfunc->Evaluate(3, contextualpattern, idthread);
	fl_push_clip(x->Integer(), y->Integer(), wx->Integer(), hy->Integer());
	return aTRUE;
}

Tamgu* Tamguwidget::MethodPopClip(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	fl_pop_clip();
	return aTRUE;
}

Tamgu* Tamguwidget::MethodPie(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* w = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* h = callfunc->Evaluate(3, contextualpattern, idthread);
	Tamgu* a1 = callfunc->Evaluate(4, contextualpattern, idthread);
	Tamgu* a2 = callfunc->Evaluate(5, contextualpattern, idthread);
	fl_pie(x->Integer(), y->Integer(), w->Integer(), h->Integer(), a1->Float(), a2->Float());
	return aTRUE;
}

Tamgu* Tamguwidget::MethodPoint(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	fl_point(x->Integer(), y->Integer());
	return aTRUE;
}

Tamgu* Tamguwidget::MethodCircle(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	int x = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	int y = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
	int r = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
	if (callfunc->Size() == 3) {
		fl_circle(x, y, r);
		return aTRUE;
	}
	Tamgu* kcolor = callfunc->Evaluate(3, contextualpattern, idthread);
	Fl_Color color;
	if (kcolor->Type() == a_string) {
		//0 is the first parameter and so on...
		string buf = kcolor->String();
		if (colors.find(buf) == colors.end()) {
			buf = "WND(657): Unknown color code";
			return globalTamgu->Returnerror(buf, idthread);
		}
		color = colors[buf];
	}
	else
		color = kcolor->Integer();
	//we set the color
	fl_color(color);
	fl_circle(x, y, r);
	return aTRUE;
}

Tamgu* Tamguwidget::MethodTextMeasure(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (Widget() == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	Tamgu* t = callfunc->Evaluate(0, contextualpattern, idthread);
	string buf = t->String();
	int w;
	int h;
	fl_measure(buf.c_str(), w, h, 1);
	if (contextualpattern->isVectorContainer()) {
		Tamgu* iv = Selectaivector(contextualpattern);
		iv->storevalue((long)w);
		iv->storevalue((long)h);
		return iv;
	}
	Tamgumapsi* map = (Tamgumapsi*)globalTamgu->Provideinstance("mapsi", idthread);
	map->values["W"] = w;
	map->values["H"] = h;
	return map;
}


Tamgu* Tamguwidget::MethodTooltip(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	Fl_Widget* wdg = Widget();
	if (wdg == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	string buf = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	char* msg = (char*)malloc(buf.size() + 1);
	memcpy(msg, STR(buf), buf.size());
	msg[buf.size()] = 0;
	wdg->tooltip(msg);
	return aTRUE;
}

Tamgu* Tamguwidget::MethodHide(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	Fl_Widget* wdg = Widget();
	if (wdg == NULL)
		return Returningerror("WND(678): Widget not initialized", idthread);
	wdg->hide();
	return aTRUE;
}

Tamgu* Tamguwidget::MethodBackgroundColor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	Fl_Widget* window = Widget();
	if (window == NULL)
		return Returningerror("WND(805): Widget not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalTamgu->Provideint(window->color());
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	Fl_Color color;
	if (ke->Type() == a_string) {
		//0 is the first parameter and so on...
		string buf = ke->String();
		if (colors.find(buf) == colors.end()) {
			buf = "WND(657): Unknown color code";
			return globalTamgu->Returnerror(buf, idthread);
		}
		color = colors[buf];
	}
	else
		color = ke->Integer();
	window->color(color);
	return aTRUE;
}

Tamgu* Tamguwidget::Returningerror(string msg, short idthread) {
    if (Stopall)
        return aNULL;
    return globalTamgu->Returnerror(msg, idthread);
}

