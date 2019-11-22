/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : jagmain.cxx
 Date       : 2019/03/25
 Purpose    : Main of Jag (작) executable
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include <stdio.h>

#include <unistd.h>   //_getch
#include <termios.h>  //_getch
#include <sys/ioctl.h>
#include <signal.h>
#include <iomanip>

#include <string>

#include "conversion.h"
#include "x_tokenize.h"
#include "jag.h"

static string version = "0.98 build 7";


int main(int argc, char *argv[]) {
    JAGEDITOR = new jag_editor;

    if (argc == 2) {
        string cmd = argv[1];
        if (cmd == "-h" || cmd == "-help" || cmd == "--help" || cmd == "--h") {
            cout << m_clear << m_home;
            cerr << m_red << "jag(작): micro text processor (version: " << version << ")" << m_current << endl;
            cerr << m_red << "Copyright 2019-present NAVER Corp." << m_current << endl;
            cerr << m_redital << "Ctrl-xh:" << m_current << " to display this help from within" << endl << endl;
            JAGEDITOR->displaythehelp(true);
            cerr << endl;
            exit(0);
        }

        if (JAGEDITOR->loadfile(cmd))
            JAGEDITOR->launchterminal(true);
        else
            JAGEDITOR->launchterminal(false);
    }
    else
        JAGEDITOR->launchterminal(false);
}

