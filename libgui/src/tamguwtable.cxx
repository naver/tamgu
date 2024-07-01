/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguwtable.cxx
 Date       : 2017/09/01
 Purpose    : table implementation (based on FLTK)
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamguwtable.h"
#include "tamguint.h"
#include "tamgustring.h"
#include "instructions.h"
#include "tamgumap.h"
#include "tamgumapss.h"

//We need to declare once again our local definitions.
basebin_hash<wtableMethod>  Tamguwtable::methods;



short Tamguwtable::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguwtable::AddMethod(TamguGlobal* global, string name, wtableMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamguwtable::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamguwtable::InitialisationModule(global,"");
}

bool Tamguwtable::InitialisationModule(TamguGlobal* global, string version) {
	methods.clear();
	
	

	Tamguwtable::idtype = global->Getid("wtable");

	Tamguwtable::AddMethod(global, "_initial", &Tamguwidget::MethodInitial, P_ONE, "_initial(variable): Creation of a window with a call back object");

	Tamguwtable::AddMethod(global, "create", &Tamguwtable::MethodCreate, P_FIVE, "create(int x, int y, int w, int h, string label): Create a table of objects and starts adding");
	Tamguwtable::AddMethod(global, "clear", &Tamguwtable::MethodClear, P_NONE, "clear(): Clear the table");
	Tamguwtable::AddMethod(global, "add", &Tamguwtable::MethodAdd, P_THREE, "add(int R, int C, string v): Add a value on row R and column C");
	Tamguwtable::AddMethod(global, "cell", &Tamguwtable::MethodCell, P_TWO, "cell(int R, int C): Return the value of the cell on row R and column C");
	Tamguwtable::AddMethod(global, "cellalign", &Tamguwtable::MethodCellAlignment, P_ONE, "cellalign(align): Set the inner cell alignement");
	Tamguwtable::AddMethod(global, "cellalignheadercol", &Tamguwtable::MethodCellAlignmentHeaderCol, P_ONE, "cellalignheadercol(align): Set the header column alignement");
	Tamguwtable::AddMethod(global, "cellalignheaderrow", &Tamguwtable::MethodCellAlignmentHeaderRow, P_ONE, "cellalignheaderrow(align): Set the header row alignement");
	Tamguwtable::AddMethod(global, "row", &Tamguwtable::MethodRow, P_ONE | P_NONE, "row()|(int nb): Define the number of rows");
	Tamguwtable::AddMethod(global, "column", &Tamguwtable::MethodColumn, P_ONE | P_NONE, "column()|(int nb): Define the number of columns");
	Tamguwtable::AddMethod(global, "rowheight", &Tamguwtable::MethodRowHeight, P_ONE, "rowheight(int height): Define the row height in pixel");
	Tamguwtable::AddMethod(global, "columnwidth", &Tamguwtable::MethodColumnWidth, P_ONE, "columnwidth(int width): Define the column width in pixel");
	Tamguwtable::AddMethod(global, "font", &Tamguwtable::MethodFont, P_ONE | P_NONE, "font(int s): set or return the text font");
	Tamguwtable::AddMethod(global, "fontsize", &Tamguwtable::MethodFontSize, P_ONE | P_NONE, "fontsize(int c): set or return the text font size");
	Tamguwtable::AddMethod(global, "colorbg", &Tamguwtable::MethodColorbg, P_ONE | P_NONE, "colorbg(int c): set or return the cell color background");
	Tamguwtable::AddMethod(global, "colorfg", &Tamguwtable::MethodColorfg, P_ONE | P_NONE, "colorfg(int c): set or return the cell color foreground");
	Tamguwtable::AddMethod(global, "when", &Tamguwtable::MethodWhen, 65534, "when(string when1 string when2...): Type of event to trigger the callback");
	Tamguwtable::AddMethod(global, "selectioncolor", &Tamguwtable::MethodTableSelectionColor, P_ONE | P_NONE, "colorselection(int color): Color for the selected elements");
	Tamguwtable::AddMethod(global, "boxtype", &Tamguwtable::MethodBoxType, P_ONE, "boxtype(int boxtype): box type");
	Tamguwtable::AddMethod(global, "rowheader", &Tamguwtable::MethodRowHeader, P_TWO, "rowheader(int pos, string label): Set the row header label at row pos");
	Tamguwtable::AddMethod(global, "columnheader", &Tamguwtable::MethodColumnHeader, P_TWO, "columnheader(int pos, string label): Set the column header label at column pos");
	Tamguwtable::AddMethod(global, "rowheaderheight", &Tamguwtable::MethodRowHeaderHeight, P_ONE, "rowheaderheight(int height): the size in pixel of the row header");
	Tamguwtable::AddMethod(global, "columnheaderwidth", &Tamguwtable::MethodColumnHeaderWidth, P_ONE, "columnheaderwidth(int width): the size in pixel of the column header");
	Tamguwtable::AddMethod(global, "tooltip", &Tamguwidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
	Tamguwtable::AddMethod(global, "show", &Tamguwidget::MethodShow, P_NONE, "show(): Show the widget");
	Tamguwtable::AddMethod(global, "focus", &Tamguwidget::MethodFocus, P_NONE, "focus(): Get the focus");
	Tamguwtable::AddMethod(global, "hide", &Tamguwidget::MethodHide, P_NONE, "hide(): Hide the widget");
	Tamguwtable::AddMethod(global, "labelsize", &Tamguwidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
	Tamguwtable::AddMethod(global, "labelfont", &Tamguwidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
	Tamguwtable::AddMethod(global, "labelcolor", &Tamguwidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
	Tamguwtable::AddMethod(global, "labeltype", &Tamguwidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
	Tamguwtable::AddMethod(global, "label", &Tamguwidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
	Tamguwtable::AddMethod(global, "boundaries", &Tamguwidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "boundaries(ivector crds): return a vector of the widget coordinates");
	Tamguwtable::AddMethod(global, "redraw", &Tamguwidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
	Tamguwtable::AddMethod(global, "backgroundcolor", &Tamguwidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
	Tamguwtable::AddMethod(global, "created", &Tamguwidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
	Tamguwtable::AddMethod(global, "copy", &Tamguwidget::MethodCopy, P_ONE, "copy(string): Copy string to clipboard");
	Tamguwtable::AddMethod(global, "paste", &Tamguwidget::MethodPaste, P_NONE, "paste(): paste string");
	Tamguwtable::AddMethod(global, "align", &Tamguwidget::MethodAlign, P_ONE, "align(int): define the label alignment");




	global->newInstance[Tamguwtable::idtype] = new Tamguwtable(global);
	global->RecordCompatibilities(Tamguwtable::idtype);

	return true;
}


Tamgu* Tamguwtable::Put(Tamgu* index, Tamgu* value, short idthread) {
	return this;
}

Tamgu* Tamguwtable::Eval(Tamgu* context, Tamgu* index, short idthread) {
	return this;
}

TamguIteration* Tamguwtable::Newiteration(bool direction) {
	return aITERNULL;
}


void TableRow::draw_cell(TableContext context,
	int R, int C, int X, int Y, int W, int H)
{
	string buffer;

	switch (context)
	{
	case CONTEXT_STARTPAGE:
		fl_font(font, fontsize);
		return;

	case CONTEXT_COL_HEADER:
		fl_push_clip(X, Y, W, H);
		{
			fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, col_header_color());
			fl_color(FL_BLACK);
			if (colheader.find(R) != colheader.end())
				buffer = colheader[C];
			fl_draw(buffer.c_str(), X, Y, W, H, cell_alignement_header_col);
		}
		fl_pop_clip();
		return;

	case CONTEXT_ROW_HEADER:
		fl_push_clip(X, Y, W, H);
		{
			fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, row_header_color());
			fl_color(FL_BLACK);
			if (rowheader.find(R) != rowheader.end())
				buffer = rowheader[R];
			fl_draw(buffer.c_str(), X, Y, W, H, cell_alignement_header_row);
		}
		fl_pop_clip();
		return;

	case CONTEXT_CELL:
	{
						 fl_push_clip(X, Y, W, H);
						 {
							 // BG COLOR
							 fl_color(is_selected(R, C) ? selection_color() : cell_bgcolor);
							 fl_rectf(X, Y, W, H);

							 // TEXT
							 fl_color(cell_fgcolor);
							 buffer = cell(R, C);
							 fl_draw(buffer.c_str(), X, Y, W, H, cell_alignement);
							 // BORDER
							 fl_color(color());
							 fl_rect(X, Y, W, H);
						 }
						 fl_pop_clip();
						 return;
	}

	case CONTEXT_TABLE:
	case CONTEXT_ENDPAGE:
	case CONTEXT_RC_RESIZE:
	case CONTEXT_NONE:
		return;
	}
}

void TableRow::table_callback(Fl_Widget*, void *data) {
	Tamguwtable* o = (Tamguwtable*)data;
	if (o->function != NULL && o->table != NULL) {
		Tamgu* func = o->function;
		TamguCallFunction kfunc(func);

        short idthread = globalTamgu->GetThreadid();

		Tamgumap* kmap = globalTamgu->Providemap();
		int rtop, cleft, rbot, cright;
		o->table->get_selection(rtop, cleft, rbot, cright);
		kmap->Push("top", globalTamgu->Provideint(rtop));
		kmap->Push("left", globalTamgu->Provideint(cleft));
		kmap->Push("bottom", globalTamgu->Provideint(rbot));
		kmap->Push("right", globalTamgu->Provideint(cright));
        Tamgumapss* valmap = globalTamgu->Providemapss();
		kmap->Push("values", valmap);
		char buff[20];
		for (int i = rtop; i <= rbot; i++) {
			for (int j = cleft; j <= cright; j++) {
				sprintf_s(buff, 20, "%d:%d", i, j);
				valmap->values[buff] = o->table->cell(i, j);
			}
		}

		o->Setreference();
		o->object->Setreference();
		kmap->Setreference();

		kfunc.arguments.push_back(o);
		kfunc.arguments.push_back(kmap);
		kfunc.arguments.push_back(o->object);
		kfunc.Eval(aNULL, aNULL, idthread);

		o->Resetreference();
		o->object->Resetreference();
		kmap->Resetreference();

	}
}


Tamgu* Tamguwtable::MethodCreate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	
	Tamgu* x1 = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y1 = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* x2 = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* y2 = callfunc->Evaluate(3, contextualpattern, idthread);
	Tamgu* text = callfunc->Evaluate(4, contextualpattern, idthread);
	if (table != NULL)
		return Returningerror("WND(101): table already created", idthread);
	if (function != NULL && function->Size() != 3)
		return globalTamgu->Returnerror("TAMGUI(881): Error, wrong function arguments", idthread);
	string thetxt = text->String();
	if (thetxt.size() < 1024)
		strcpy_s(txt, 1024, thetxt.c_str());
	
	table = new TableRow(this, x1->Integer(), y1->Integer(), x2->Integer(), y2->Integer(), txt);

	SaveItem(this);
	table->selection_color(FL_YELLOW);
	table->when(FL_WHEN_RELEASE);
	table->table_box(FL_NO_BOX);
	table->col_resize_min(4);
	table->row_resize_min(4);
	table->row_header(0);
	table->col_header(0);
	table->row_resize(1);
	table->col_resize(1);
	table->end();
	return aTRUE;
}

Tamgu* Tamguwtable::MethodClear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (table == NULL)
		return Returningerror("WND(102): table has not been created yet", idthread);
	table->clear();
	return aTRUE;
}

Tamgu* Tamguwtable::MethodAdd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (table == NULL)
		return Returningerror("WND(102): table has not been created yet", idthread);
	Tamgu* kR = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* kC = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* kvalue = callfunc->Evaluate(2, contextualpattern, idthread);
	table->cell(kR->Integer(), kC->Integer(), kvalue->String());
	return aTRUE;
}

Tamgu* Tamguwtable::MethodCell(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (table == NULL)
		return Returningerror("WND(102): table has not been created yet", idthread);
	Tamgu* kR = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* kC = callfunc->Evaluate(1, contextualpattern, idthread);
	string buff = table->cell(kR->Integer(), kC->Integer());
	return globalTamgu->Providestring(buff);
}

Tamgu* Tamguwtable::MethodCellAlignment(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (table == NULL)
		return Returningerror("WND(102): table has not been created yet", idthread);
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	if (ke->Type() == a_string) {
		string align = ke->String();
		if (aligns.find(align) == aligns.end()) {
			align = "WND(202): Unknown alignement";
			return globalTamgu->Returnerror(align, idthread);
		}
		table->setalignment(aligns[align]);
	}
	else
		table->setalignment(ke->Integer());
	return aTRUE;
}

Tamgu* Tamguwtable::MethodCellAlignmentHeaderCol(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (table == NULL)
		return Returningerror("WND(102): table has not been created yet", idthread);
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	if (ke->Type() == a_string) {
		string align = ke->String();
		if (aligns.find(align) == aligns.end()) {
			align = "WND(202): Unknown alignement";
			return globalTamgu->Returnerror(align, idthread);
		}
		table->setalignmentheader(aligns[align], true);
	}
	else
		table->setalignmentheader(ke->Integer(), true);
	return aTRUE;
}

Tamgu* Tamguwtable::MethodCellAlignmentHeaderRow(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (table == NULL)
		return Returningerror("WND(102): table has not been created yet", idthread);
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	if (ke->Type() == a_string) {
		string align = ke->String();
		if (aligns.find(align) == aligns.end()) {
			align = "WND(202): Unknown alignement";
			return globalTamgu->Returnerror(align, idthread);
		}
		table->setalignmentheader(aligns[align], false);
	}
	else
		table->setalignmentheader(ke->Integer(), false);
	return aTRUE;
}

Tamgu* Tamguwtable::MethodRow(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (table == NULL)
		return Returningerror("WND(102): table has not been created yet", idthread);
	if (callfunc->Size() == 0)
		return globalTamgu->Provideint(table->rows());
	Tamgu* kR = callfunc->Evaluate(0, contextualpattern, idthread);
	table->rows(kR->Integer());
	return aTRUE;
}

Tamgu* Tamguwtable::MethodColumn(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (table == NULL)
		return Returningerror("WND(102): table has not been created yet", idthread);
	if (callfunc->Size() == 0)
		return globalTamgu->Provideint(table->cols());
	Tamgu* kC = callfunc->Evaluate(0, contextualpattern, idthread);
	table->cols(kC->Integer());
	return aTRUE;
}

Tamgu* Tamguwtable::MethodRowHeight(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (table == NULL)
		return Returningerror("WND(102): table has not been created yet", idthread);
	Tamgu* kHeight = callfunc->Evaluate(0, contextualpattern, idthread);
	table->row_height_all(kHeight->Integer());
	return aTRUE;
}

Tamgu* Tamguwtable::MethodColumnWidth(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (table == NULL)
		return Returningerror("WND(102): table has not been created yet", idthread);
	Tamgu* kWidth = callfunc->Evaluate(0, contextualpattern, idthread);
	table->col_width_all(kWidth->Integer());
	return aTRUE;
}

Tamgu* Tamguwtable::MethodFont(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (table == NULL)
		return Returningerror("WND(678): table not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalTamgu->Provideint(table->Font());
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	//0 is the first parameter and so on...
	if (ke->Type() == a_string) {
		string buf = ke->String();
		if (fonts.find(buf) == fonts.end()) {
			buf = "WND(657): Unknown font code";
			return globalTamgu->Returnerror(buf, idthread);
		}
		table->Font(fonts[buf]);
	}
	else {
		int font = ke->Integer();
		if (font >= fontmax)
			return Returningerror("WND(658): unknown font", idthread);
		table->Font(font);
	}
	return aTRUE;
}

Tamgu* Tamguwtable::MethodFontSize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (table == NULL)
		return Returningerror("WND(678): table not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalTamgu->Provideint(table->Fontsize());
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	//0 is the first parameter and so on...
	table->Fontsize(ke->Integer());
	return aTRUE;
}

Tamgu* Tamguwtable::MethodColorbg(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (table == NULL)
		return Returningerror("WND(678): table not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalTamgu->Provideint(table->GetCellBGColor());
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
	table->SetCellBGColor(color);
	return aTRUE;
}

Tamgu* Tamguwtable::MethodColorfg(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (table == NULL)
		return Returningerror("WND(678): table not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalTamgu->Provideint(table->GetCellFGColor());
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
	table->SetCellFGColor(color);
	return aTRUE;
}

Tamgu* Tamguwtable::MethodWhen(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (table == NULL)
		return Returningerror("WND(805): table not created", idthread);
	Tamgu* ke;
	int action = 0;
	for (int i = 0; i < callfunc->Size(); i++) {
		ke = callfunc->Evaluate(i, contextualpattern, idthread);
		if (ke->Type() == a_string) {
			if (buttonAction.find(ke->String()) == buttonAction.end()) {
				string mess = "WND(654): Unknown action";
				return globalTamgu->Returnerror(mess, idthread);
			}
			action |= buttonAction[ke->String()];
		}
		else
			action |= ke->Integer();
	}
	table->when(action);
	return aTRUE;
}

Tamgu* Tamguwtable::MethodTableSelectionColor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (table == NULL)
		return Returningerror("WND(678): table not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalTamgu->Provideint(table->GetCellBGColor());
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
	table->selection_color(color);
	return aTRUE;
}

Tamgu* Tamguwtable::MethodBoxType(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (table == NULL)
		return Returningerror("WND(102): table not created", idthread);
	Tamgu* pos = callfunc->Evaluate(0, contextualpattern, idthread);
	if (pos->Type() == a_string) {
		if (boxTypes.find(pos->String()) == boxTypes.end()) {
			string mes = "WND(767): Unknown type:";
			mes += pos->String();
			return globalTamgu->Returnerror(mes, idthread);
		}
		else
			table->table_box((Fl_Boxtype)boxTypes[pos->String()]);
	}
	else
		table->table_box((Fl_Boxtype)pos->Integer());
	return aTRUE;
}

Tamgu* Tamguwtable::MethodRowHeader(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (table == NULL)
		return Returningerror("WND(102): table has not been created yet", idthread);
	Tamgu* position = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* label = callfunc->Evaluate(1, contextualpattern, idthread);
	//we then activate the row header, since we have something in it to show
	table->row_header(1);
	table->addrowheader(position->Integer(), label->String());
	return aTRUE;
}

Tamgu* Tamguwtable::MethodColumnHeader(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (table == NULL)
		return Returningerror("WND(102): table has not been created yet", idthread);
	Tamgu* position = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* label = callfunc->Evaluate(1, contextualpattern, idthread);
	table->addcolumnheader(position->Integer(), label->String());
	//we activate then column header
	table->col_header(1);
	return aTRUE;
}

Tamgu* Tamguwtable::MethodRowHeaderHeight(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (table == NULL)
		return Returningerror("WND(102): table has not been created yet", idthread);
	Tamgu* kwidth = callfunc->Evaluate(0, contextualpattern, idthread);
	table->row_header_width(kwidth->Integer());
	return aTRUE;
}

Tamgu* Tamguwtable::MethodColumnHeaderWidth(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (table == NULL)
		return Returningerror("WND(102): table has not been created yet", idthread);
	Tamgu* kheight = callfunc->Evaluate(0, contextualpattern, idthread);
	table->col_header_height(kheight->Integer());
	return aTRUE;
}





