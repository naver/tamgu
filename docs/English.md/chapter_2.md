# Command Line (Unix platforms) in Tamgu

You can use Tamgu as a command line tool on Linux or Mac OS platforms. Tamgu offers the following commands:

- `-i`: This command allows you to pre-declare variables. For example: `bool a, b, c; int i, j, k; float f, g, h; string s, t, u; map m; vector v; self x, y, z;`

- `-c`: Allows you to execute a piece of Tamgu code provided after the command. For example: `tamgu -c 'println(10+20);'`

- `-a`: Intercepts the sendings on stdio and records them in the vector `_args`. It is possible to provide a piece of code.

- `-p`: Intercepts line-by-line sending on stdio and allows the execution of a piece of code where the pre-declared variable `l` contains the current stdio line. For example: `ls -l | tamgu -p 'println(l);'`

Tamgu can also be called with a program name and a list of arguments. This list of arguments is then available in `_args`. For example: `tamgu mycode.tmg a1 a2 a3 a3`. The `_args` vector will contain `["a1", "a2", "a3"]`.

It is also possible to combine stdio lines and the call of a program with its arguments. The `-a` option must then be placed after the program name. For example: `ls -1 | tamgu mycode.tmg -a a1 a2 a3`.

## Integrated IDE

Tamgu provides an integrated text mode IDE that you can use to create, modify, or debug your programs. You can call this environment in two different ways:

a) If you run `tamgu` without arguments, you can then type the `edit` command which will put you in edit mode.

b) You can also launch `tamgu -e myfile.tmg` which will open your file and automatically put you in edit mode. If `myfile.tmg` does not exist, you can create it the first time you save it.

### Edit/Run Mode

You can use Tamgu either to edit your file or execute them. You can launch the execution of your file from within the editor with `Ctrl-Xr`. Once your code has been executed, you will switch to the "run mode" where you can test your variables. You can also switch to the "run mode" with `Ctrl-c`. To switch back to "edit mode" type: `edit`.

### Help

The IDE offers many commands including the following online help, which can be called either with the command `help` in run mode or with `Ctrl-Xh` in edit mode.

Commands:

1. Programs:
   - `create filename`: Create a file space with a specific file name.
   - `open filename`: Load a program (use `run` to execute it).
   - `open`: Reload the program (use `run` to execute it).
   - `save filename`: Save the buffer content in a file.
   - `save`: Save the buffer content with the current filename.
   - `run filename`: Load and run a program filename.
   - `run`: Execute the buffer content.
   - `debug`: Debug mode.
   - `n!`: Next line.
   - `l!`: Display local variables.
   - `a!`: Display all active variables.
   - `s!`: Display the stack.
   - `g!`: Go to next breakpoint.
   - `e!`: Execute up to the end.
   - `S!`: Stop the execution.
   - `name`: Display the content of a variable (just type its name).
   - `h!`: Display debug help.