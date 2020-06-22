/@
 Project    : Spreadsheet
 filename   : tamgucalc.tmg
 Date       : 2020/06/15
 Purpose    : spreadsheet for terminals with Lisp expressions
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
@/

if (version() < "Tamgu 1.2020.06.21.09") {
    _sys.cls();
    println();
    println("Please, accept all our apologies...");
    println("Alas, your version of Tamgu is too old to run 'tamgucalc'...");
    println("We needed to adjust tamgu to run some of the new stuff in tamgucalc...");
    print("Please update it at: ");
    _sys.colors(7,31,107);
    print("https://github.com/naver/tamgu/releases");
    _sys.colors(0,0,0);
    println("\n");
   _exit();
}

//The Lisp interpreter
lisp lisp_interpreter;

string msgerr;

//Color definition to highlight parenthesis balancing
string colorparenth = _sys.colors(7,31,49, false);
string colorsel = _sys.colors(1, 31, 49, false);
string colornrm = _sys.colors(0,0,0,false);


int inputsection = 6;
//Initialisation: coords contains your terminal dimensions
ivector coords = _sys.screensize();

//columnsize is the size of columns. You can modify this value
int columnsize = 10;
//We initialise the number of lines and columns according to your terminal size and columnsize
//We keep 'inputsection' lines below the last line to display messages
int x_viewsize = coords[0]-inputsection;
//The number of columns depends on the number of columns on the screen / the chosen columnsize
int y_viewsize = coords[1]/columnsize;
//We keep these values for comparison when the size is modified
int x_maxview = x_viewsize;
int y_maxview = y_viewsize;
//These two values define the actual matrix size, which is different from viewsize
//If the view port is larger than 101 and 51, then the matrix is equal to the viewport
int x_max = max(101,x_maxview);
int y_max = max(51,y_maxview);
//codeline is used to position messages
//'inputsection' lines down after the last line of the matrix
int codeline = x_viewsize+inputsection;

//Actual boundaries of the table
//These bounderies define the square in which values have been entered
int x_bound=0;
int y_bound=0;

//v_matrix is the visualisation matrix
vector v_matrix;
//formulas records the different Lisp expressions
mapss formulas;
mapss referencedefuns;

//the line 0 and the column 0 are used to display coordinates...
int i = 1;
int j = 1;
int I, J;
//Since, the matrix might be larger than the viewport,
//we need to record the offsets to position 0,0
int off_x = 0;
int off_y = 0;
bool modedit = false;

//A few predefined methods
\(defun put (i j k) (key (key v_matrix i) j (string k)))
\(defun average (x) (/ (sum x) (size x)))

//You can call a regular function from a Lisp formula:
// (fillcolumn mat[:2][1:6] 1 10): fill column 10, starting at row 1 with values from mat[:2][1:6]
//fill a row with values, column c is fixed, it defines the starting point
function fillcolumn(vector v, int r, int c) {
    int vi = 0;
    for (int i in <r, r+v.size()>) {
        v_matrix[i][c] = v[vi];
        vi++;
    }
    return ("C"+r+","+c);
}

//fill a column with values, row r is fixed, it defines the starting point
function fillrow(vector v, int r, int c) {
    int vi = 0;
    for (int i in <c, c+v.size()>) {
        v_matrix[r][i] = v[vi];
        vi++;
    }
    return ("R"+r+","+c);
}

//Return all values up to a certain value: (from m[:1] 0)
function upto(fvector v, float val) {
    long vi = 0;
    for (long i in <1,v.size()>) {
        //When we found our value, we stop
        if (v[i] == val) {
            vi = i;
            break;
        }
    }
    //If the value is not found, the list is empty...
    return (v[:vi]);
}
//mat is the actual matrix in which computing is done...
fmatrix mat(x_max,y_max);

//inputs is used to handle modifications on: filenames and size.
//If you want to enrich tamgucalc, you might add a new key
mapss inputs;
string inputkey;
string inputvalue;
inputs["Size"] = ""+(x_viewsize-1)+":"+(y_viewsize-1);
inputs["File"] = _args[0];
inputs["Go(r,c)"] = "1,1";

if (_args[0] != "")
    inputs["Data(raw csv)"] = _args[0]+".csv";
else
    inputs["Data(raw csv)"] = "";


//initialisation of v_matrix
function initialisation() {
    int i,j;
    for (i in <x_max>) {
        v_matrix[i] = [];
        for (j in <y_max>) {
            if (!i) {
                v_matrix[i].push(string(j));
            }
            elif (!j) {
                v_matrix[i].push(string(i));
            }
            else
                v_matrix[i].push("0");
        }
    }
}

//Loading a file
function readtable(string f) {
    int i,j;
    for (i in <1,x_max>) {
        for (j in <1,y_max>) {
            v_matrix[i][j] = "0";
        }
    }

    try {
        svector sv;
        svector cut;
        int nb;
        string s,k,e;
        sv.read(f);
        bool beg = true;
        i=0;
        for (s in sv) {
            cut=s.split("\t");
            if (beg == true) {
                //we have stored some parameters in the three first cells
                x_viewsize = cut[0];
                y_viewsize = cut[1];
                columnsize = cut[2];
                x_max = cut[3];
                y_max = cut[4];
                x_bound = cut[5];
                y_bound = cut[6];
                inputs["Size"] = ""+(x_viewsize-1)+":"+(y_viewsize-1);
                codeline = x_viewsize+inputsection;
                mat.dimension(x_max,y_max);
                beg=false;
                //we need to reset them
                cut[0] = '0';
                cut[1] = '1';
                cut[2] = '2';
                cut[3] = '3';
                cut[4] = '4';
                cut[5] = '5';
                cut[6] = '6';
            }
            j = 0;
            for (e in cut) {
                e=e.trim();
                if (e[0] == "(") {
                    k = i+"_"+j;
                    formulas[k] = e;
                    v_matrix[i][j] = "0";
                }
                else
                    v_matrix[i][j] = e;
                j++;
            }
            i++;
        }
        evaluation(0,0);
    }
    catch {
        initialisation();
        displayall(0,0);
    }
}

//Writing the visual matrix content to a file
function writetable(string f) {
    if (f=="") {
        displaymessage("Abort");
        return;
    }
    string k;
    int x_bound = x_bound;
    int y_bound=max(y_bound,7);
    displaymessage("Writting: "+f);
    file sv(f,"w");
    int i,j;
    string w;
    v_matrix[0][0] = string(x_viewsize);
    v_matrix[0][1] = string(y_viewsize);
    v_matrix[0][2] = string(columnsize);
    v_matrix[0][3] = string(x_max);
    v_matrix[0][4] = string(y_max);
    v_matrix[0][5] = string(x_bound);
    v_matrix[0][6] = string(y_bound);

    for (i in <x_bound>) {
        for (j in <y_bound>) {
            if (j)
                sv.write("\t");
            k = i+"_"+j;
            if (formulas.test(k))
                sv.write(formulas[k]);
            else {
                w = v_matrix[i][j];
                sv.write(w.trim());
            }
        }
        sv.write("\n");
    }
    v_matrix[0][0] = "0";
    v_matrix[0][1] = "1";
    v_matrix[0][2] = "2";
    v_matrix[0][3] = "3";
    v_matrix[0][4] = "4";
    v_matrix[0][5] = "5";
    v_matrix[0][6] = "6";
}

//Writing only data to a CSV file. The result of formulas is stored
function writecsv(string f) {
    if (f=="") {
        displaymessage("Abort");
        return;
    }
    if ('.csv' not in f)
        f+=".csv";
    displaymessage("Writting (csv): "+f);
    file sv(f,"w");
    int i,j;
    string k;
    string dsp;
    for (i in <1,x_bound>) {
        for (j in <1,y_bound>) {
            if (j!=1)
                sv.write("\t");
            dsp = v_matrix[i][j];
            dsp = dsp.trim();
            if (dsp[-1] == '!')
                dsp=dsp[:-1];
            if (!dsp[0].isdigit())
                sv.write(dsp.json());
            else
                sv.write(dsp);
        }
        sv.write("\n");
    }
}


//------------------- The Code ---------
//Formulas evaluation
function evaluation(int off_x, int off_y) {
    mat.clear();
    string ky;
    int i,j;
    for (i in <1,x_bound>) {
        for (j in <1,y_bound>) {
            ky = i+"_"+j;
            if (formulas.test(ky)) {
                mat[i:j] = 0;
            }
            else {
                mat[i:j] = float(v_matrix[i][j]);
            }
        }
    }
    if (formulas.size()==0) {
        displayall(off_x,off_y);
        return;
    }
    msgerr="";
    string msg;
    self val;
    float value;
    int o;
    bool stop=false;

    //We record function beforehand
    for (ky in formulas) {
        i = ky[:"_"][:-1];
        j = ky["_":][1:];
        if ("defun " in formulas[ky]) {
            try {
                if (referencedefuns[ky] != formulas[ky]) {
                    //We only reaevaluate a function, if it has been modified
                    referencedefuns[ky] = formulas[ky];
                    lisp_interpreter.eval(formulas[ky]);
                    v_matrix[i][j] = "#"+formulas[ky][" ":"("][:-1].trim();
                }
            }
            catch(msg) {
                msgerr=msg;
                v_matrix[i][j] = "#FCERR";
            }
        }
    }

    //Not very efficient, we apply until nothing is modified again
    for (o in <formulas.size()>) {
        stop=true;
        for (ky in formulas) {
            //functions are skipped
            if ("defun " in formulas[ky])
                continue;
            i = ky[:"_"][:-1];
            j = short(ky["_":][1:]);
            try {
                val = lisp_interpreter.eval(formulas[ky]);
                value = val.float();
                if (value != mat[i:j])
                    stop=false;
                mat[i:j] = value;
                v_matrix[i][j] = val.string();
                v_matrix[i][j]+="!";
            }
            catch(msg) {
                msgerr=msg;
                v_matrix[i][j] = "#ERR";
            }
        }
        if (stop)
            break;
    }
    displayall(off_x, off_y);
    displayerr(msgerr);
}

//Display methods
//Display one element, we check its size
function dispelement(int i,int j, int off_x, int off_y) {
    string u = v_matrix[i+off_x][j+off_y];
    if (u.size() >= columnsize-1)
        u=u[:columnsize-2]+"_";
    print(u);
}

//Displaying all elements on screen
function displayall(int off_x, int off_y) {
    int i,j;
    _sys.cls();
    _sys.colors(7,96,40);
    for (i in <x_viewsize>) {
        _sys.row_column(i+1,0);
        dispelement(i,0, off_x, 0);
    }

    for (j in <y_viewsize>) {
        _sys.row_column(0,columnsize*j);
        dispelement(0,j, 0, off_y);
    }

    _sys.colors(0,0,0);
    for (i in <1,x_viewsize>) {
        for (j in <1,y_viewsize>) {
            _sys.row_column(i+1,columnsize*j);
            dispelement(i,j, off_x, off_y);
        }
    }
}

//Error message is displayed on line 4
function displayerr(string s) {
    _sys.row_column(codeline-4,0);
    _sys.eraseline(2);
    print(s);
}

//Messages are displayed on line 3
function displaymessage(string s) {
    _sys.row_column(codeline-3,0);
    _sys.eraseline(2);
    print(s);
}

//Highlight the current element
function showelement(int i, int j, int off_x, int off_y) {
    string dsp;
    int I = i+off_x;
    int J = j+off_y;
    string ky = I+"_"+J;
    //We check if we are in a formula mode
    if (inputkey != "")
        dsp = inputkey+": "+inputvalue;
    elif (formulas.test(ky)) {
        dsp = I+","+J+": "+formulas[ky];
    }
    else {//When we have a 0, we do not display it
        dsp = v_matrix[I][J];
        if (dsp.trim()=="")
            v_matrix[I][J]="0";
        if (v_matrix[I][J] == '0')
            dsp = I+","+J+": ";
        else {
            dsp = I+","+J+": "+v_matrix[I][J];
        }
    }

    //we use spaces to clean the cell first
    string space;
    space.padding(columnsize," ");
    _sys.row_column(i+1,columnsize*j);
    print(space);
    //Then we position our cursor again to display our value
    _sys.row_column(i+1,columnsize*j);
    //We use a specific color to show that it has been selected
    _sys.colors(7,31,49);
    dispelement(i,j, off_x, off_y);
    //Colors are reset, we then display on the last line the current value
    _sys.colors(0,0,0);
    _sys.row_column(codeline-1,0);
    _sys.eraseline(2);
    print(dsp);
}

//----------------------------------------------------------------------------------------------------
//MAIN LOOP

function handlingmovement() {
    bool redisplay=false;
    if (i < 1) {
        i=1;
        if (off_x > 0) {
            off_x--;
            redisplay=true;
        }
    }
    else
        if (i > x_viewsize-1) {
            i = x_viewsize-1;
            if ((off_x+i) < x_max-1) {
                off_x++;
                redisplay=true;
            }
        }
    if (j < 1) {
        j = 1;
        if (off_y > 0) {
            off_y--;
            redisplay=true;
        }
    }
    else
        if (j > y_viewsize-1) {
            j = y_viewsize-1;
            if ((off_y+j) < y_max-1) {
                off_y++;
                redisplay=true;
            }
        }
    if (redisplay)
        displayall(off_x, off_y);
    showelement(i,j, off_x, off_y);
}

//If a filename is provided, we try to read the file, otherwise we initialise our structure
initialisation();
if (inputs["File"]=="")
    displayall(0,0);
else
    readtable(inputs["File"]);

string msgform="%1Arrows%2 to select cell. %1Enter%2 to record. %1..%2 to define a range.";
msgform=msgform.format(colorsel,colornrm);
string helpmsg = "%1'('%2:Formula %1Ctrl-s%2:Save %1Ctrl-w%2:Save as %1Crld-d%2:Save Data %1Ctrl-r%2:Resize %1Ctrl-g%2:Goto %1Ctrl-q%2:Quit";
helpmsg = helpmsg.format(colorsel,colornrm);
string msgbase = "%1Ctrl-b%2: Help %1Ctrl-e%2: Edit %1Ctrl-q%2: Quit";
msgbase =  msgbase.format(colorsel,colornrm);
displaymessage(msgbase);

string msgedit = "%1Edit Mode%2 %1Esc%2=abort %1Enter%2=record";
msgedit =  msgedit.format(colorsel,colornrm);

showelement(i,j, off_x, off_y);

string s=_sys.getchar();

bool selection = false;
string forme, ky;
int ci=-1,cj=-1;
int lasti, lastj;
int posinstring;

//17 is ctrl-q
while (s[0].ord() != 17) {
    I = i+off_x;
    J = j+off_y;
    //We use this position to display displaymessages
    if (modedit)
        displaymessage(msgedit);
    else
        displaymessage(msgbase);
    ky = I+"_"+J;
    if (s[0].ord() == 2) {
        displaymessage(helpmsg);
        showelement(i,j, off_x, off_y);
        s=_sys.getchar();
        continue;
    }
    
    //Modification of a formula
    if (modedit) {
        string dsp = I+","+J+": ";
        if (s.ord() == 27) {
            modedit=false;
            inputvalue = formulas[ky];
            displaymessage(msgbase);
            showelement(i,j, off_x, off_y);
        }
        elif (s.ord() == 11) {
            //ctrl-k
            inputvalue =inputvalue[:posinstring-1];
            posinstring = inputvalue.size()+1;
        }
        elif (s == "\n") {
            modedit=false;
            formulas[ky] = inputvalue;
            posinstring = inputvalue.size()+1;
            evaluation(off_x, off_y);
            displaymessage(msgbase);
            showelement(i,j, off_x, off_y);
        }
        elif (s.ord() == 127) {
            if (posinstring > 1) {
                inputvalue = inputvalue[:posinstring-2]+inputvalue[posinstring-1:];
                posinstring--;
            }
        }
        elif (s == _sys_keydel) {
            //We delete the left character
            if (posinstring <= inputvalue.size())
                inputvalue = inputvalue[:posinstring-1]+inputvalue[posinstring:];
        }
        elif (s == _sys_keyright)
            posinstring++;
        elif (s == _sys_keyleft)
            posinstring--;
        else {
            inputvalue = inputvalue[:posinstring-1]+s+inputvalue[posinstring-1:];
            posinstring++;
        }
            
        if (posinstring > inputvalue.size()+1)
            posinstring = inputvalue.size()+1;
        if (posinstring <= 0)
            posinstring = 1;
        _sys.row_column(codeline-1,0);
        _sys.eraseline(2);
        _sys.row_column(codeline-1,0);
        print(dsp+inputvalue);
        _sys.row_column(codeline-1,dsp.size()+posinstring);
        s=_sys.getchar();
        continue;
    }

    //ctrl-e: edit mode
    if (s[0].ord() == 5 && formulas.test(ky)) {
        modedit = true;
        string dsp = I+","+J+": ";
        inputvalue = formulas[ky];
        posinstring = inputvalue.size()+1;
        displaymessage(msgedit);
        _sys.row_column(codeline-1,dsp.size()+posinstring);
        s=_sys.getchar();
        continue;
    }


    //Using arrows
    if (s[0].ord() == 27) {
        if (s.size() == 1) {
            //We abort an inputkey...
            if (inputkey != "") {
                inputkey="";
                inputvalue="";
            }
            showelement(i,j, off_x, off_y);
            s=_sys.getchar();
            continue;
        }
        _sys.row_column(i+1,columnsize*j);
        dispelement(i,j, off_x, off_y);
        if (s == _sys_keydel) {
            if (not selection) {
                v_matrix[I][J] = "0";
                if (formulas.test(ky))
                    formulas.pop(ky);
            }
            elif (I == ci and J == cj) {
                v_matrix[I][J] = "0";
                formulas.pop(ky);
                selection = false;
            }
            showelement(i,j, off_x, off_y);
            s = _sys.getchar();
            continue;
        }
        elif (s == _sys_keyright)
            j++;
        elif (s == _sys_keyleft)
            j--;
        elif (s == _sys_keyup)
            i--;
        elif (s == _sys_keydown)
            i++;

        handlingmovement();
        s = _sys.getchar();
        continue;
    }

    //character deletion with the delete key
    if (s.ord() == 127) {
        if (inputkey!="") {
            inputvalue=inputvalue[:-1];
        }
        else {
            //We are either modifying a formula or a cell
            if (formulas.test(ky)) {
                formulas[ky] = formulas[ky][:-1];
                if (formulas[ky] == "") {
                    //The formula has been deleted
                    formulas.pop(ky);
                    v_matrix[I][J] = "0";
                    selection=false;
                    ci =-1;
                    cj =-1;
                    _sys.row_column(i+1,columnsize*j);
                    //We display the "0" instead
                    dispelement(i,j, off_x, off_y);
                }
                else {
                    selection=true;
                    ci = I;
                    cj = J;
                }
            }
            else {
                if (!selection) {
                    if (v_matrix[I][J].size() > 1)
                        v_matrix[I][J] = v_matrix[I][J][:-1];
                    else
                        v_matrix[I][J] = "0";
                }
            }
        }
        showelement(i,j, off_x, off_y);
        s=_sys.getchar();
        continue;
    }

    //When introducing a formula, selection is true and the key is the formula position
    if (selection) {
        displaymessage(msgform);
        ky = ci+"_"+cj;
    }

    //When hitting the Carriage Return key
    if (s == "\n") {
        if (modedit) {
            modedit = false;
        }
        elif (inputkey=="Go(r,c)") {
            int pi = inputvalue[:","][:-1];
            int pj = inputvalue[",":][1:];
            pi = max(1,pi);
            pj = max(1,pj);
            pi = min(x_max-1,pi);
            pj = min(y_max-1,pj);
            inputkey="";
            inputvalue="";
            off_x = max(off_x+pi-I,0);
            off_y = max(off_y+pj-J,0);
            I = pi;
            J = pj;
            i = I-off_x;
            j = J-off_y;
            displayall(off_x,off_y);
        }
        elif (inputkey=="Size") {
            ivector iv = inputvalue.split(":");
            if (iv == 2) {
                int x = iv[0];
                int y = iv[1];
                x++;
                y++;
                if (x > x_maxview) {
                    x = x_maxview;
                }
                else {
                    if (x <= 1) {
                        x = 2;
                    }
                }
                if (y > y_maxview) {
                    y = y_maxview;
                }
                else {
                    if (y <= 1) {
                        y = 2;
                    }
                }
                x_viewsize = x;
                y_viewsize = y;
                inputs[inputkey] = ""+(x_viewsize-1)+":"+(y_viewsize-1);
                codeline = x_viewsize+inputsection;
                i = 1;
                I = off_x+1;
                j = 1;
                J = off_y+1;
                displayall(off_x, off_y);
            }
        }
        elif (inputkey == "Data(raw csv)") {
            inputs[inputkey] = inputvalue;
            writecsv(inputs[inputkey]);
        }
        elif (inputkey=="File") {
            inputs[inputkey] = inputvalue;
            writetable(inputs[inputkey]);
        }
        else {
            //If we are in selection mode (formula) then we enable the cell selection with the arrows
            if (selection) {
                string subkey;
                //We test if a ".." has been introduced, in that case, we compute the interval between the structures
                if (lasti != -1 && formulas[ky][-2:] == '..') {
                    if (lasti > I)
                        [lasti,I] = [I,lasti];
                    if (lastj > J)
                        [lastj,J] = [J,lastj];

                    string frm;
                    int n;
                    bool beg=true;

                    formulas[ky] = formulas[ky][:-2];
                    //a single line
                    if (lasti == I) {
                        frm = "mat["+I+":]["+lastj+":"+(J+1)+"]";
                        formulas[ky][-"mat":] = frm;
                    }
                    elif (lastj == J) { //a single column
                        frm = "mat[:"+J+"]["+lasti+":"+(I+1)+"]";
                        formulas[ky][-"mat":] = frm;
                    }
                    else { //a square
                        n = lasti;
                        //&&& is the merge operator
                        frm = "(&&& ";
                        //we will add several lines together
                        while (n <= I) {
                            if (!beg)
                                frm += " ";
                            else
                                beg=false;
                            frm += "mat["+n+":]["+lastj+":"+(J+1)+"]";
                            n++;
                        }
                        frm += ")";
                        formulas[ky][-"mat":] = frm;
                    }
                }
                else
                    //One element at a time
                    formulas[ky] += "mat["+I+":"+J+"] ";
                lasti=I;
                lastj=J;
                i = ci-off_x;
                j = cj-off_y;
            }
            else {
                displaymessage("eval");
                evaluation(off_x, off_y);
                _sys.row_column(i+1,columnsize*j);
                //You can add new values with the CR key
                //We showelement it to remove the selection color
                dispelement(i,j, off_x, off_y);
                //We move to the next cell below
                i++;
                handlingmovement();
            }
        }

        inputkey="";
        inputvalue="";
        showelement(i,j, off_x, off_y);
        s=_sys.getchar();
        continue;
    }

    if (s[0].ord() < 27) {
        if (s[0].ord() == 4) {
            //ctrl-d
            inputkey="Data(raw csv)";
            inputvalue=inputs[inputkey];
            print(inputkey+": ",inputvalue);
        }
        elif (s[0].ord() == 7) {
            inputkey = "Go(r,c)";
            inputvalue = "";
            print(inputkey+": ",inputvalue);
        }
        elif (s[0].ord() == 19 and inputs["File"] != "") //ctrl-s
            writetable(inputs["File"]);
        elif (s[0].ord() == 19 or s[0].ord() == 23) { //ctrl-w/s
            _sys.row_column(codeline-1,0);
            inputkey="File";
            inputvalue=inputs[inputkey];
            print(inputkey+": ",inputvalue);
        }
        elif (s[0].ord() == 18) { //ctrl-r
            inputkey="Size";
            inputvalue=inputs[inputkey];
            print(inputkey+": ",inputvalue);
        }
        showelement(i,j, off_x, off_y);
        s=_sys.getchar();
        continue;
    }

    if (inputkey == "" and formulas.test(ky) and ci == I and cj == J) {
        formulas[ky]+=s;
        formulas[ky] = formulas[ky].replace(colorparenth,"");
        formulas[ky] = formulas[ky].replace(colornrm,"");
        //When the "(...)" balances, we exit the selection mode
        if (formulas[ky].count('(') > formulas[ky].count(")")) {
            if (s == ')') {
                int e = formulas[ky].size()-2;
                while (e>0 and formulas[ky][e:].count("(") != formulas[ky][e:].count(")")) e--;
                formulas[ky] = formulas[ky][:e]+colorparenth+formulas[ky][e]+colornrm+formulas[ky][e+1:];
            }
            selection = true;
            displaymessage(msgform);
        }
        else {
            selection=false;
            displaymessage('Ready to compute!!!');
        }
        showelement(i,j, off_x, off_y);
        s=_sys.getchar();
        continue;
    }

    //We detect a '(', we enter the formula mode
    if (s=='(' and !selection) {
        ci = I;
        cj = J;
        lasti=-1;
        lastj=-1;
        formulas[ky] = s;
        x_bound = max(I+1,x_bound);
        y_bound = max(J+1,y_bound);
        selection = true;
        displaymessage(msgform);
        showelement(i,j, off_x, off_y);
        s=_sys.getchar();
        continue;
    }

    //In case of pasting code
    if (s.size() > 2 and s[0] == '(') {
        s=s.trim();
        string u,k;
        //We try to detect from which place it was copied
        for (k in formulas) {
            if (formulas[k].trim() == s) {
                u=k;
                break;
            }
        }
        if (u != "") {
            u["_"]=":";
            string l= u[:":"];
            string c= u[":":];
            string rl = I+":";
            string rc = ":"+J;
            s=s.replace(l,rl);
            s=s.replace(c,rc);
            formulas[ky]=s;
            showelement(i,j, off_x, off_y);
            s=_sys.getchar();
            continue;
        }
    }

    if (inputkey != "")
        inputvalue += s;
    else {
        if (not selection) {
            if (v_matrix[I][J] == '0')
                v_matrix[I][J]=s;
            else
                v_matrix[I][J]+=s;
            x_bound = max(I+1,x_bound);
            y_bound = max(J+1,y_bound);
        }
    }
    showelement(i,j, off_x, off_y);
    s=_sys.getchar();
    continue;
}




