## Command Line (Unix Platforms)

You can use Tamgu as a command line tool on Linux or Mac OS platforms. Tamgu offers the following commands:

- **-i**: This command allows you to pre-declare variables. For example, `bool a,b,c; int i,j,k; float f,g,h; string s,t,u; map m; vector v; self x,y,z;`
- **-c**: Allows you to execute a piece of Tamgu code provided after the command. For example, `tamgu -c 'println(10+20);'`
- **-a**: Intercepts the sendings on stdio and records them in the vector _args. It is possible to provide a piece of code.
- **-p**: Intercepts line-by-line sending on stdio and allows the execution of a piece of code where the pre-declared variable l contains the current stdio line. For example, `ls -l | tamgu -p 'println(l);'`
- Tamgu can also be called with a program name and a list of arguments. This list of arguments is then available in _args. For example, `tamgu mycode.tmg a1 a2 a3 a3`. _args contains: ["a1", "a2", "a3"]
- It is also possible to combine the stdio lines and the call of a program with its arguments. The "-a" option must then be placed after the program name. For example, `ls -1 | tamgu mycode.tmg -a a1 a2 a3`

## Integrated IDE

Tamgu provides an integrated text mode IDE that you can use to create, modify, or even debug your programs. You can call this environment in two different ways:

- If you run "tamgu" without arguments, you can then type the "edit" command which will put you in edit mode.
- You can also launch: "tamgu -e myfile.tmg" which will open your file and automatically put you in edit mode. If "myfile.tmg" does not exist, you can create it the first time you save it.

### Edit/Run Mode

You can use Tamgu either to edit your file or execute them. You can launch the execution of your file from within the editor with "Ctrl-Xr". Once your code has been executed, you will switch to the "run mode" where you can test your variables. You can also switch to the "run mode" with "Ctrl-c". To switch back to "edit mode" type: edit.

### Help

The IDE offers many commands including the following online help, which can be called either with the command "help" in run mode or with "Ctrl-Xh" in edit mode.

#### Commands:

1. **Programs**:
   - create filename: create a file space with a specific file name
   - open filename: load a program (use run to execute it)
   - open: reload the program (use run to execute it)
   - save filename: save the buffer content in a file
   - save: save the buffer content with the current filename
   - run filename: load and run a program filename
   - run: execute the buffer content
   - debug: debug mode
   - n!: next line
   - l!: display local variables
   - a!: display all active variables
   - s!: display the stack
   - g!: go to next breakpoint
   - e!: execute up to the end
   - S!: stop the execution
   - name: display the content of a variable (just type its name)
   - h!: display debug help
   - args v1 v2...vn: set a list of values to the argument list
   - filename: display the current filename

2. **Command Line Mode**:
   - help: display the help
   - help n: display one of the help sections (from 1 to 5)
   - history: display the command history
   - load filename: load a command history file
   - store filename: store the command history in a file
   - filename: display the current filename
   - filespace: display all the files stored in memory with their file space id
   - select idx: select a file space
   - metas: display meta-characters
   - rm: clear the buffer content
   - rm b:e: remove the lines from b to e (b: or :e is also possible)
   - list: list the buffer content
   - list b:e: display the buffer content from line b to e (b: or :e is also possible)
   - Ctrl-t: up in the code listing
   - Ctrl-g: down in the code listing
   - Ctrl-f: force the current line to be appended at the end of the code
   - ?method: return a description for 'method'
   - ?o.method: return a description for 'method' for variable 'o'

3. **Edit (idx)**: edit mode. You can optionally select also a file space
   - Ctrl-b: toggle breakpoint
   - Ctrl-k: delete from cursor up to the end of the line
   - Ctrl-d: delete a full line
   - Ctrl-u: undo last modification
   - Ctrl-r: redo last modification
   - Ctrl-f: find a string
   - Ctrl-n: find next
   - Ctrl-g: move to a specific line, '$' is the end of the code
   - Ctrl-l: toggle between top and bottom of the screen
   - Ctrl-t: re-indent the code
   - Ctrl-h: local help
   - Ctrl-w: write file to disk
   - Ctrl-c: exit the editor
   - Ctrl-x: Combined Commands
   - C: count a pattern
   - H: convert HTML entities to Unicode characters
   - D: delete a block of lines
   - c: copy a block of lines
   - x: cut a block of lines
   - v: paste a block of lines
   - d: run in debug mode
   - r: run the code
   - w: write and quit
   - l: load a file
   - m: display meta-characters
   - h: full help
   - q: quit

4. **Environment**:
   - cls: clear screen
   - color: display terminal colors
   - color att fg bg: display a color out of attribute, foreground, background
   - colors: display the colors for code coloring
   - colors name att fg bg: modify the color for one of the following denominations = strings, methods, keywords, functions, comments

5. **System**:
   - !unix: what follows the '!' will be executed as a Unix command (ex: !ls)
   - clear (idx): clear the current environment or a specific file space
   - reinit: clear the buffer content and initialize predeclared variables
   - Ctrl-d: end the session and exit Tamgu
   - exit: end the session and exit Tamgu

6. **Regular expressions for 'find'**:
   - %d stands for any digit
   - %x stands for a hexadecimal digit (abcdef0123456789ABCDEF)
   - %p stands for any punctuation
   - %c stands for any lowercase letter
   - %C stands for any uppercase letter
   - %a stands for any letter
   - ? stands for any character
   - %? stands for the character "?" itself
   - %% stands for the character "%" itself
   - %s stands for any space character including the non-breaking space
   - %r stands for a carriage return
   - %n stands for a non-breaking space
   - ~ negation
   - \x escape character
   - \ddd character code across 3 digits exactly
   - \xFFFF character code across 4 hexadecimals exactly
   - {a-z} between a and z included
   - [a-z] sequence of characters
   - ^ the expression should start at the beginning of the string
   - $ the expression should match up to the end of the string

#### Examples:

- `dog%c` matches dogs or dogg
- `m%d` matches m0, m1,...,m9
- `{%dab}` matches 1, a, 2, b
- `{%dab}+` matches 1111a, a22a90ab