# INDEX:
`_args`: [Tamgu an introduction](https://github.com/naver/tamgu/tree/master/docs/English.md/3_Tamgu_an_introduction.md)

`_current`: [Tamgu an introduction](https://github.com/naver/tamgu/tree/master/docs/English.md/3_Tamgu_an_introduction.md)

`_dropwhile`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`_endl`: [Tamgu an introduction](https://github.com/naver/tamgu/tree/master/docs/English.md/3_Tamgu_an_introduction.md)

`_erroronkey(bool)`: [Tamgu an introduction](https://github.com/naver/tamgu/tree/master/docs/English.md/3_Tamgu_an_introduction.md)

`_exit()`

This function is used to exit the program.

### Error on Key: `_erroronkey(bool)`: [Tamgu an introduction](https://github.com/naver/tamgu/tree/master/docs/English.md/3_Tamgu_an_introduction.md)

`_filter`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`_final`: [Frame](https://github.com/naver/tamgu/tree/master/docs/English.md/10_Frame.md)

`_foldl`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`_foldr`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`_iferror`: [try catch raise iferror](https://github.com/naver/tamgu/tree/master/docs/English.md/39_try_catch_raise_iferror.md)

`_initial`: [Frame](https://github.com/naver/tamgu/tree/master/docs/English.md/10_Frame.md)

`_loosecompatibility(bool v)`: [Tamgu an introduction](https://github.com/naver/tamgu/tree/master/docs/English.md/3_Tamgu_an_introduction.md)

`_loosecompatibility(true)`: [Tamgu an introduction](https://github.com/naver/tamgu/tree/master/docs/English.md/3_Tamgu_an_introduction.md)

`_map`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`_paths`: [Tamgu an introduction](https://github.com/naver/tamgu/tree/master/docs/English.md/3_Tamgu_an_introduction.md)

`_phi`: [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`_pi`: [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`_poolstats()`

This function is used to return the current state of different object pools. For efficiency reasons, some objects are managed in pools, which allows for the reusability of recurrent objects (such as strings, integers, floats, vectors, or maps). These objects are not deleted but cleared and yielded back for reuse when needed. The result of this function is a `mapsi` object.

## Passing Arguments to a Tamgu Program

When a Tamgu program is called with a list of arguments, each of these arguments becomes available to the Tamgu program through three specific system variables: `_args`, `_current`, and `_paths`.

### Example:

```bash
tamgu myprogram c:\test\mytext.txt
```

### `_args`: Argument Vector

Tamgu provides a specific variable called `_args`, which is actually a string vector that stores each argument according to its position in the declaration list.

### Example (from the call above):

```cpp
file f;
f.openread(_args[0]);
```

### `_paths` and `_current`: Path Management

Tamgu provides a vector variable called `_paths`, which stores the pathnames of the different Tamgu programs that have been loaded. The first element of this vector, `_paths[0]`, stores the current directory pathname, while `_paths[1]` stores the path of the current program file.

Tamgu also provides another variable called `_current`, which stores the path of the program file that is currently being run. The path stored in `_current` always finishes with a final separator. Actually, `_current` points to the same path as `_paths[1]`.

### `_endl`: Carriage Return

Windows and Unix use different carriage return characters. `_endl` returns the proper carriage return according to the platform value.

### `_sep`: Separator in Pathnames

Unix-based systems and Windows use different separators in pathnames between directory names. Unix requires a "/" while Windows requires a "\". Tamgu provides a specific variable called `_sep`, which returns the right separator according to your system.

### `_seteventvariable(var)`: [Tamgu an introduction](https://github.com/naver/tamgu/tree/master/docs/English.md/3_Tamgu_an_introduction.md)

`_pushevent(v1,v2,v3...)`: [Tamgu an introduction](https://github.com/naver/tamgu/tree/master/docs/English.md/3_Tamgu_an_introduction.md)

`_pushevent`: [Tamgu an introduction](https://github.com/naver/tamgu/tree/master/docs/English.md/3_Tamgu_an_introduction.md)

`_scanl`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`_scanr`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`_sep`: [Tamgu an introduction](https://github.com/naver/tamgu/tree/master/docs/English.md/3_Tamgu_an_introduction.md)

`_setenv(varname, value)`: [Tamgu an introduction](https://github.com/naver/tamgu/tree/master/docs/English.md/3_Tamgu_an_introduction.md)

`_setjoinedlimit(nb)`: [Tamgu an introduction](https://github.com/naver/tamgu/tree/master/docs/English.md/3_Tamgu_an_introduction.md)

`_setstacksize(size)`: [Tamgu an introduction](https://github.com/naver/tamgu/tree/master/docs/English.md/3_Tamgu_an_introduction.md)

`_setthreadlimit(nb)`: [Tamgu an introduction](https://github.com/naver/tamgu/tree/master/docs/English.md/3_Tamgu_an_introduction.md)

`_setvalidfeatures(mapss features)`: [Tamgu an introduction](https://github.com/naver/tamgu/tree/master/docs/English.md/3_Tamgu_an_introduction.md)

`_sys_keybackspace`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`_sys_keyc_down`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`_sys_keyc_endkey`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`_sys_keyc_homekey`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`_sys_keyc_left`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`_sys_keyc_right`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`_sys_keyc_up`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`_sys_keydel`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`_sys_keydown`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`_sys_keyendkey`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`_sys_keyescape`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`_sys_keyhomekey`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`_sys_keyleft`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`_sys_keyright`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`_sys_keyup`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`_sys`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`_takewhile`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`_tau`: [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`_trace(bool)`: [Inference engine](https://github.com/naver/tamgu/tree/master/docs/English.md/45_Inference_engine.md)

`_version()`

Returns a string with version information about Tamgu.

### Mirror Display: `_mirrordisplay(bool)`: [Tamgu an introduction](https://github.com/naver/tamgu/tree/master/docs/English.md/3_Tamgu_an_introduction.md)

`_zip`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`_zipwith`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`a_change`: [Type  transducer](https://github.com/naver/tamgu/tree/master/docs/English.md/28_Type_transducer.md)

`a_delete`: [Type  transducer](https://github.com/naver/tamgu/tree/master/docs/English.md/28_Type_transducer.md)

`a_first`: [Type  transducer](https://github.com/naver/tamgu/tree/master/docs/English.md/28_Type_transducer.md)

`a_insert`: [Type  transducer](https://github.com/naver/tamgu/tree/master/docs/English.md/28_Type_transducer.md)

`a_int`: [Function autorun thread](https://github.com/naver/tamgu/tree/master/docs/English.md/7_Function_autorun_thread.md)

`a_mapii`: [Function autorun thread](https://github.com/naver/tamgu/tree/master/docs/English.md/7_Function_autorun_thread.md)

`a_nocase`: [Type  transducer](https://github.com/naver/tamgu/tree/master/docs/English.md/28_Type_transducer.md)

`a_repetition`: [Type  transducer](https://github.com/naver/tamgu/tree/master/docs/English.md/28_Type_transducer.md)

`a_string`: [Function autorun thread](https://github.com/naver/tamgu/tree/master/docs/English.md/7_Function_autorun_thread.md)

`a_surface`: [Type  transducer](https://github.com/naver/tamgu/tree/master/docs/English.md/28_Type_transducer.md)

`a_switch`: [Type  transducer](https://github.com/naver/tamgu/tree/master/docs/English.md/28_Type_transducer.md)

`a_vowel`: [Type  transducer](https://github.com/naver/tamgu/tree/master/docs/English.md/28_Type_transducer.md)

`aaaed`: [Type  Tamgu, Regular, Expression, treg](https://github.com/naver/tamgu/tree/master/docs/English.md/15_Type_Tamgu_Regular_Expression_treg.md)

`aaed`: [Type  Tamgu, Regular, Expression, treg](https://github.com/naver/tamgu/tree/master/docs/English.md/15_Type_Tamgu_Regular_Expression_treg.md)

`abb`: [Type  Tamgu, Regular, Expression, treg](https://github.com/naver/tamgu/tree/master/docs/English.md/15_Type_Tamgu_Regular_Expression_treg.md)

`abbbe`: [Type  Tamgu, Regular, Expression, treg](https://github.com/naver/tamgu/tree/master/docs/English.md/15_Type_Tamgu_Regular_Expression_treg.md)

`abe`: [Type  Tamgu, Regular, Expression, treg](https://github.com/naver/tamgu/tree/master/docs/English.md/15_Type_Tamgu_Regular_Expression_treg.md)

`abebe`: [Type  Tamgu, Regular, Expression, treg](https://github.com/naver/tamgu/tree/master/docs/English.md/15_Type_Tamgu_Regular_Expression_treg.md)

`abs()`: Return the absolute value.
3. `acos()`: [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`accuracy(vector words, int threshold)`: [Library word2vec type word2vec](https://github.com/naver/tamgu/tree/master/docs/English.md/59_Library_word2vec_type_word2vec.md)

`accuracy`: [Library word2vec type word2vec](https://github.com/naver/tamgu/tree/master/docs/English.md/59_Library_word2vec_type_word2vec.md)

`acosh()`: Return the area hyperbolic cosine.
5. `asin()`: [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`activate()`: Activate the current element
2. `bgcolor(int c)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`add(container, bool norm, int encoding)`: [Type  transducer](https://github.com/naver/tamgu/tree/master/docs/English.md/28_Type_transducer.md)

`add(label)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`add(string path)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`add(wtreeitem e, string n)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`add`: [Type  transducer](https://github.com/naver/tamgu/tree/master/docs/English.md/28_Type_transducer.md); [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`addchild`: [dependency and synode](https://github.com/naver/tamgu/tree/master/docs/English.md/46_dependency_and_synode.md)

`aed`: [Type  Tamgu, Regular, Expression, treg](https://github.com/naver/tamgu/tree/master/docs/English.md/15_Type_Tamgu_Regular_Expression_treg.md)

`aeeeb`: [Type  Tamgu, Regular, Expression, treg](https://github.com/naver/tamgu/tree/master/docs/English.md/15_Type_Tamgu_Regular_Expression_treg.md)

`alert(string msg)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`align(int a)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`align(int)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.3_GUI_ToolKit_library_libgui.md)

`allmaps`: [Specialized maps](https://github.com/naver/tamgu/tree/master/docs/English.md/23_Specialized_maps.md)

`alpha`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`amap`: [Type  tree, bin, prime, map](https://github.com/naver/tamgu/tree/master/docs/English.md/22_Type_tree_bin_prime_map.md)

`analogy(svector words)`: [Library word2vec type word2vec](https://github.com/naver/tamgu/tree/master/docs/English.md/59_Library_word2vec_type_word2vec.md)

`analogy`: [Library word2vec type word2vec](https://github.com/naver/tamgu/tree/master/docs/English.md/59_Library_word2vec_type_word2vec.md)

`and`: [Frame](https://github.com/naver/tamgu/tree/master/docs/English.md/10_Frame.md); [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md); [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`annotator`: [Type  annotator](https://github.com/naver/tamgu/tree/master/docs/English.md/29_Type_annotator.md)

`append`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`apply(annotator a, bool keeplex)`: [Type  annotator](https://github.com/naver/tamgu/tree/master/docs/English.md/29_Type_annotator.md)

`apply(string|vector)`: [Type  grammar](https://github.com/naver/tamgu/tree/master/docs/English.md/30_Type_grammar.md)

`apply(uvector tokens, bool keeplex)`: [Type  annotator](https://github.com/naver/tamgu/tree/master/docs/English.md/29_Type_annotator.md)

`apply(vector morphos, bool keeplex)`: [Type  annotator](https://github.com/naver/tamgu/tree/master/docs/English.md/29_Type_annotator.md)

`apply`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`arc(float x, float y, float rad, float a1, float a2)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`arc(int x, int y, int x1, int y1, float a1, float a2)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`arguments`: [Python library pytamgu](https://github.com/naver/tamgu/tree/master/docs/English.md/56_Python_library_pytamgu.md)

`asinh()`: Return the area hyperbolic sine.
7. `atan()`: [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`ask(string msg, string buttonmsg2, string buttonmsg1, ...)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`assertz(pred(...))`: [Inference engine](https://github.com/naver/tamgu/tree/master/docs/English.md/45_Inference_engine.md)

`assertz`: [dependency and synode](https://github.com/naver/tamgu/tree/master/docs/English.md/46_dependency_and_synode.md)

`atanh()`: Return the area hyperbolic tangent.
9. `base(int b)`: [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`atomp`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`attribute`: [dependency and synode](https://github.com/naver/tamgu/tree/master/docs/English.md/46_dependency_and_synode.md)

`auaed`: [Type  Tamgu, Regular, Expression, treg](https://github.com/naver/tamgu/tree/master/docs/English.md/15_Type_Tamgu_Regular_Expression_treg.md)

`aubased`: [Type  Tamgu, Regular, Expression, treg](https://github.com/naver/tamgu/tree/master/docs/English.md/15_Type_Tamgu_Regular_Expression_treg.md)

`aued`: [Type  Tamgu, Regular, Expression, treg](https://github.com/naver/tamgu/tree/master/docs/English.md/15_Type_Tamgu_Regular_Expression_treg.md)

`autorun`: [Function autorun thread](https://github.com/naver/tamgu/tree/master/docs/English.md/7_Function_autorun_thread.md)

`backgroundcolor(int color)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md); [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`base(int b, bool toconvert)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`base(vector chrs)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md); [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`begin()`.
5. `create(x1, x2, x3)`: [Library sqlite type sqlite](https://github.com/naver/tamgu/tree/master/docs/English.md/52_Library_sqlite_type_sqlite.md)

`begin()`: Enters the commit mode with DEFERRED mode.
2. `begin(string mode)`: [Library sqlite type sqlite](https://github.com/naver/tamgu/tree/master/docs/English.md/52_Library_sqlite_type_sqlite.md)

`begin()`: Initializes the iterator to the beginning of the collection.
2. `end()`: [Type  iterator, riterator](https://github.com/naver/tamgu/tree/master/docs/English.md/31_Type_iterator_riterator.md)

`begin(int x, int y, int w, int h, string title)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`begin`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`begincomplexpolygon()`
22. `gap()`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`beginline()`
16. `endline()`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`beginloop()`
18. `endloop()`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`beginpoints()`
14. `endpoints()`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`beginpolygon()`
20. `endpolygon()`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`bernoulli_distribution()` function constructs a `bernoulli_distribution` object, adopting the distribution parameters specified by `t` and `p`.

Parameters: `t`, `p`
- `t`: The upper bound of the range `[0, t]` of possible values the distribution can generate. This represents the number of independent Bernoulli-distributed experiments each generated value is said to simulate.
- `p`: Probability of success. This represents the probability of success on each of the independent Bernoulli-distributed experiments each generated value is said to simulate. This shall be a value between 0.0 and 1.0 (both included).

It returns an `ivector` of Boolean values (0 or 1) of size `nb`.

#### Binomial Distribution

The `binomial_distribution()`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`bernoulli_distribution`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`beta`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`binomial_distribution`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`bit(int ith)`: [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`bitmap(bitmap im, int color, string label, int labelalign)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.3_GUI_ToolKit_library_libgui.md)

`bitmap(bitmap image, int color, int x, int y)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`bitmap(bitmap image, int color, int x, int y, int w, int h)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`bits(int v)`: [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`block()` is called again with a different current terminal element, then the previous boundary is moved to this new terminal element.
3. `unblock()`: [Type  fibre](https://github.com/naver/tamgu/tree/master/docs/English.md/43_Type_fibre.md)

`block()` is used to define the current terminal element of that list as the iteration boundary. New fibres can still be appended, but they will not be executed until `unblock()`: [Type  fibre](https://github.com/naver/tamgu/tree/master/docs/English.md/43_Type_fibre.md)

`block`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`bloop`: [Type  iloop, floop, bloop, sloop, uloop](https://github.com/naver/tamgu/tree/master/docs/English.md/17_Type_iloop_floop_bloop_sloop_uloop.md)

`body`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`border(bool b)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`break`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md); [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md); [Type  fibre](https://github.com/naver/tamgu/tree/master/docs/English.md/43_Type_fibre.md)

`browser`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`build(string input, string output, bool norm, int encoding)`: [Type  transducer](https://github.com/naver/tamgu/tree/master/docs/English.md/28_Type_transducer.md)

`build`: [Type  transducer](https://github.com/naver/tamgu/tree/master/docs/English.md/28_Type_transducer.md)

`bytes()`: Returns an ivector of bytes matching the string.
- `charposition(int pos)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`c_microsecond`: [Type  chrono](https://github.com/naver/tamgu/tree/master/docs/English.md/34_Type_chrono.md)

`c_millisecond`: [Type  chrono](https://github.com/naver/tamgu/tree/master/docs/English.md/34_Type_chrono.md)

`c_nanosecond`: [Type  chrono](https://github.com/naver/tamgu/tree/master/docs/English.md/34_Type_chrono.md)

`c_second`: [Type  chrono](https://github.com/naver/tamgu/tree/master/docs/English.md/34_Type_chrono.md)

`car`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`cast()`: This instruction releases all threads, regardless of their string state.
3. `lock(string s)`: [Synchronization](https://github.com/naver/tamgu/tree/master/docs/English.md/44_Synchronization.md)

`cast(string)`: [Synchronization](https://github.com/naver/tamgu/tree/master/docs/English.md/44_Synchronization.md)

`cast`: [Synchronization](https://github.com/naver/tamgu/tree/master/docs/English.md/44_Synchronization.md)

`catch`: [try catch raise iferror](https://github.com/naver/tamgu/tree/master/docs/English.md/39_try_catch_raise_iferror.md)

`catching`: [Type  annotator](https://github.com/naver/tamgu/tree/master/docs/English.md/29_Type_annotator.md)

`cauchy_distribution()` function constructs a `cauchy_distribution` object, adopting the distribution parameters specified by `a` and `b`.

Parameters: `a`, `b`
- `a`: Distribution parameter `a`, which specifies the location of the peak (its mode).
- `b`: Distribution parameter `b`, which defines the scale of the distribution. This shall be a positive value (b > 0).

It returns a `float` or an `fvector` of size `nb`.

#### Fisher F-distribution

The `fisher_distribution()`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`cauchy_distribution`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`cbrt()`: Return the cubic root.
13. `chr()`: [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`cdr`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`chi_squared_distribution`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`child()`: return the first child node under the current node.
2. `child(xml)`: [Library xml type xml](https://github.com/naver/tamgu/tree/master/docs/English.md/51_Library_xml_type_xml.md)

`child(int i)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`children()`: Return number of children
5. `clean()`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`chrono`: [Type  chrono](https://github.com/naver/tamgu/tree/master/docs/English.md/34_Type_chrono.md); [Type  chrono](https://github.com/naver/tamgu/tree/master/docs/English.md/34_Type_chrono.md)

`circle(int x, int y, int r, int color)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`cleandata()`: Cleans internal data.
2. `crossvalidation()`: [Library LINEAR type linear](https://github.com/naver/tamgu/tree/master/docs/English.md/57_Library_LINEAR_type_linear.md)

`clear()`: Clear the browser from all values
3. `columnchar()`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`clear()`: Clear the screen.

32. `eraseline(int nb)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`clear()`: Clears the container.
2. `convert()`: [Type  vector](https://github.com/naver/tamgu/tree/master/docs/English.md/19_Type_vector.md)

`clear()`: Clears the container.
2. `find(value)`: [Type  tree, bin, prime, map](https://github.com/naver/tamgu/tree/master/docs/English.md/22_Type_tree_bin_prime_map.md)

`clear()`: Clears the container.
2. `first()`: [Type  list](https://github.com/naver/tamgu/tree/master/docs/English.md/20_Type_list.md)

`clear()`: Clears the label selection.
- `selection()`: [Type  annotator](https://github.com/naver/tamgu/tree/master/docs/English.md/29_Type_annotator.md)

`clear()`: Delete the tree items
4. `clicked()`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`clearscreen(int nb)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`clientid`: [socket](https://github.com/naver/tamgu/tree/master/docs/English.md/48_socket.md)

`clock`: [Type  chrono](https://github.com/naver/tamgu/tree/master/docs/English.md/34_Type_chrono.md)

`close()`: Close the current Python session.
2. `execute(string funcname, p1, p2...)`: [Python library pytamgu](https://github.com/naver/tamgu/tree/master/docs/English.md/56_Python_library_pytamgu.md)

`close()`: Close the current XML document and clean the memory from all XML values.
2. `create(string topnode)`: [Library xml type xmldoc](https://github.com/naver/tamgu/tree/master/docs/English.md/50_Library_xml_type_xmldoc.md)

`close()`: Closes a sound channel.
2. `decode(ivector soundbuffer)`: [Library sound type sound](https://github.com/naver/tamgu/tree/master/docs/English.md/54_Library_sound_type_sound.md)

`close()`: Closes the database.
4. `commit()`: [Library sqlite type sqlite](https://github.com/naver/tamgu/tree/master/docs/English.md/52_Library_sqlite_type_sqlite.md)

`close()`: closes the socket.
2. `close(clientid)`: [socket](https://github.com/naver/tamgu/tree/master/docs/English.md/48_socket.md)

`close()`: Closes the window.
11. `create(int x, int y, int w, int h, string title)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`close(string path)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`close(wtreeitem e)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`cmd`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`color(int c)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.3_GUI_ToolKit_library_libgui.md)

`color(int code)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.3_GUI_ToolKit_library_libgui.md)

`column(int nb)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`columnchar(string)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`combine`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`command(string s, string outputfile)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`command`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`compile(string rgx)`: [Type  Tamgu, Regular, Expression, treg](https://github.com/naver/tamgu/tree/master/docs/English.md/15_Type_Tamgu_Regular_Expression_treg.md)

`compile(string rules)`: [Type  annotator](https://github.com/naver/tamgu/tree/master/docs/English.md/29_Type_annotator.md)

`compilelexicons()`: Pre-compiles the lexicons. If not called before parsing, the first parse will compile them, which may introduce delays.
- `spans(bool)`: [Type  annotator](https://github.com/naver/tamgu/tree/master/docs/English.md/29_Type_annotator.md)

`compilergx(string rgx, svector features, string filename)`: [Type  transducer](https://github.com/naver/tamgu/tree/master/docs/English.md/28_Type_transducer.md)

`cond`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`connect(string hostname, int port)`: [socket](https://github.com/naver/tamgu/tree/master/docs/English.md/48_socket.md)

`connectorcolor(int c)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`connectorstyle(int s)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`connectorwidth(int s)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`cons`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`consp`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`content()`: Return the content of a node.
4. `content(string n)`: [Library xml type xml](https://github.com/naver/tamgu/tree/master/docs/English.md/51_Library_xml_type_xml.md)

`continue`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`coords()`: Returns a vector of the widget coordinates.
4. `coords(int x, int y, int w, int h)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`cos()`: Return the cosine.
15. `cosh()`: [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`cosine(element)`: [Library word2vec type word2vec](https://github.com/naver/tamgu/tree/master/docs/English.md/59_Library_word2vec_type_word2vec.md)

`create(int x, int y, int h, int w, string label)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`create(int x, int y, int w, int h, boolean multiline, string label)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.3_GUI_ToolKit_library_libgui.md)

`create(int x, int y, int w, int h, string label)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.3_GUI_ToolKit_library_libgui.md)

`create(int x, int y, int w, int h, string type, int shape, string label)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.3_GUI_ToolKit_library_libgui.md)

`create(x, y, w, h, label)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`create`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`created()`: Returns true if the object has been correctly created.
6. `hide()`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`createdirectory(string path)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`createServer(int port, int nbclients)`: [socket](https://github.com/naver/tamgu/tree/master/docs/English.md/48_socket.md)

`createServer(string hostname, int port, int nbclients)`: [socket](https://github.com/naver/tamgu/tree/master/docs/English.md/48_socket.md)

`curl`: [Library curl type curl WEB](https://github.com/naver/tamgu/tree/master/docs/English.md/55_Library_curl_type_curl_WEB.md)

`cursor()`: Return the cursor position.

38. `inmouse()`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`cursor(int cursortype, int color1, int color2)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`curve(float X0, float Y0, float X1, float Y1, float X2, float Y2, float X3, float Y3)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`cut`: [Inference engine](https://github.com/naver/tamgu/tree/master/docs/English.md/45_Inference_engine.md)

`d()`: Returns the denominator of the fraction.
2. `d(int v)`: [Type  fraction](https://github.com/naver/tamgu/tree/master/docs/English.md/18_Type_fraction.md)

`date()`: Returns the date as a string.
2. `day()`: [Type  date](https://github.com/naver/tamgu/tree/master/docs/English.md/32_Type_date.md)

`Date`: [Type  date](https://github.com/naver/tamgu/tree/master/docs/English.md/32_Type_date.md)

`deaccentuate()`: Removes the accents from accented characters.
- `doublemetaphone()`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`deactivate()`: Deactivate the current element
7. `depth()`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`decode`: [Library sound type sound](https://github.com/naver/tamgu/tree/master/docs/English.md/54_Library_sound_type_sound.md)

`defun`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`delete()`: delete the current internal node.
6. `line()`: [Library xml type xml](https://github.com/naver/tamgu/tree/master/docs/English.md/51_Library_xml_type_xml.md)

`deletechar(int nb)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`dependency`: [dependency and synode](https://github.com/naver/tamgu/tree/master/docs/English.md/46_dependency_and_synode.md)

`deselect()`: Deselect all items
7. `deselect(int i)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`determinant()`: Returns the matrix determinant
2. `dimension()`: [Type  fmatrix, imatrix](https://github.com/naver/tamgu/tree/master/docs/English.md/26_Type_fmatrix_imatrix.md)

`different`: [Frame](https://github.com/naver/tamgu/tree/master/docs/English.md/10_Frame.md)

`dimension(int rowsize, int columnsize)`: [Type  fmatrix, imatrix](https://github.com/naver/tamgu/tree/master/docs/English.md/26_Type_fmatrix_imatrix.md)

`display(int s)`: [Function autorun thread](https://github.com/naver/tamgu/tree/master/docs/English.md/7_Function_autorun_thread.md)

`display`: [Type  call](https://github.com/naver/tamgu/tree/master/docs/English.md/36_Type_call.md)

`distance(element)`: [Library word2vec type word2vec](https://github.com/naver/tamgu/tree/master/docs/English.md/59_Library_word2vec_type_word2vec.md)

`distance(svector words)`: [Library word2vec type word2vec](https://github.com/naver/tamgu/tree/master/docs/English.md/59_Library_word2vec_type_word2vec.md)

`distance`: [Library word2vec type word2vec](https://github.com/naver/tamgu/tree/master/docs/English.md/59_Library_word2vec_type_word2vec.md)

`divide`: [Frame](https://github.com/naver/tamgu/tree/master/docs/English.md/10_Frame.md)

`dogg`: [Type  Tamgu, Regular, Expression, treg](https://github.com/naver/tamgu/tree/master/docs/English.md/15_Type_Tamgu_Regular_Expression_treg.md)

`dogs`: [Type  Tamgu, Regular, Expression, treg](https://github.com/naver/tamgu/tree/master/docs/English.md/15_Type_Tamgu_Regular_Expression_treg.md)

`dos()`: Converts a string to DOS encoding.
- `dostoutf8()`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`down(int nb)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`drawcolor(int c)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`drawtext(string l, int x, int y)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`e_arabic`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`e_baltic`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`e_celtic`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`e_cyrillic`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`e_greek`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`e_hebrew`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`e_latin_ce`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`e_latin_ffe`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`e_latin_ne`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`e_latin_se`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`e_latin_see`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`e_latin_we`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`e_nordic`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`e_thai`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`e_turkish`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`e_windows`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`editdistance(v)`: [Type  vector](https://github.com/naver/tamgu/tree/master/docs/English.md/19_Type_vector.md)

`element`: [Type  grammar](https://github.com/naver/tamgu/tree/master/docs/English.md/30_Type_grammar.md)

`emoji()`: Return the textual description (in English) of an emoji based on its Unicode code.
17. `erf()`: [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`emoji()`: Returns the textual description (in English) of an emoji.
- `evaluate()`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`encode(ivector soundbuffer)`: [Library sound type sound](https://github.com/naver/tamgu/tree/master/docs/English.md/54_Library_sound_type_sound.md)

`encode`: [Library sound type sound](https://github.com/naver/tamgu/tree/master/docs/English.md/54_Library_sound_type_sound.md)

`end()`: Ends creation.
16. `flush()`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`end`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`endcomplexpolygon()`
24. `vertex(float x, float y)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`env(string var)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`env(string var, string value)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`eof()`: Returns true when the end of the file is reached.
2. `file f(string filename, string mode_read)`: [Type  file, wfile](https://github.com/naver/tamgu/tree/master/docs/English.md/35_Type_file_wfile.md)

`equal`: [Frame](https://github.com/naver/tamgu/tree/master/docs/English.md/10_Frame.md)

`erfc()`: Return the complementary error function.
19. `even()`: [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`err`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`erroronkey`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`eval()` function in Tamgu can evaluate and run Tamgu code on the fly. The result of the evaluation is returned according to what was evaluated.

## JSON Evaluation: `evaljson(string code)`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`eval(string code)`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`eval(string)`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`eval`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`evaljson()` function in Tamgu takes a JSON string as input and compiles it into either a vector or a map according to the inner structure.

## Printing Instructions

Tamgu provides several printing instructions to display results on the current display port. The `print` and `println` instructions are used to display results, while the `printerr` and `printlnerr` instructions display the results on the standard error output. The `println` and `printlnerr` versions add two features to the output: an additional space is added for values separated with a `,`, and a carriage return is added at the end of the line.

## Join Printing Instructions

The `printj`, `printjln`, `printjerr`, and `printjlnerr` instructions are used to display container values that are "joined" beforehand. They accept two, three, or four arguments. The first parameter should be a container, and the second one should be a separator string. If the container is a map, a key separator can also be supplied as the third parameter. A fourth numerical parameter can be provided to add a carriage return every n values.

Example:

```python
ivector v = [1..10];
printj(v, "-"); // Output: 1-2-3-4-5-6-7-8-9-10

map m = {1: 2, 2: 3, 4: 5, 6: 7};
printjln(m, ",", "-"); // Output: 1-2,2-3,4-5,6-7
```

## IO Redirect Functions

The `ioredirect` and `iorestate` functions are used to capture the output from stderr or stdout into a file. The `ioredirect` function redirects either stderr (if `err` is true) or stdout (if `err` is false) to a file. It returns an ID, which is used to set the output back to normal using the `iorestate` function.

Example:

```python
int o = ioredirect('C:\Tamgu\test\test.txt', true);
printlnerr("This string is now stored in file: test.txt");
iorestate(o, true); // Back to normal
```

## Pause and Sleep Functions

The `pause` and `sleep` functions are used to put a thread in pause or sleep mode. The `pause` function does not suspend the execution of a thread, while the `sleep` function does. The `pause` function takes a float as input, representing the pause duration in seconds. It can also take a second boolean parameter to display a small animation. The `sleep` function is based on the OS sleep instruction and its behavior depends on the local implementation. It takes an integer as input, representing the sleep duration in seconds.

Example:

```python
pause(0.1); // The thread will pause for 10 ms
pause(2, true); // The thread will pause for 2s, with a small animation
sleep(1); // The thread will sleep for 1s (depending on the platform)
```

## Emojis Function

The `emojis()`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`event`: [Tamgu an introduction](https://github.com/naver/tamgu/tree/master/docs/English.md/3_Tamgu_an_introduction.md)

`exclusive`: [Function autorun thread](https://github.com/naver/tamgu/tree/master/docs/English.md/7_Function_autorun_thread.md)

`execute()`: Executes a curl query. Options should have been provided.
2. `execute(string filename)`: [Library curl type curl WEB](https://github.com/naver/tamgu/tree/master/docs/English.md/55_Library_curl_type_curl_WEB.md)

`execute(handle, function_name, [arg1, arg2, ..., argn])`: [Python library pytamgu](https://github.com/naver/tamgu/tree/master/docs/English.md/56_Python_library_pytamgu.md)

`execute(string sqlcommand)`: [Library sqlite type sqlite](https://github.com/naver/tamgu/tree/master/docs/English.md/52_Library_sqlite_type_sqlite.md)

`execute`: [Python library pytamgu](https://github.com/naver/tamgu/tree/master/docs/English.md/56_Python_library_pytamgu.md)

`exp()`: Return the exponential function.
21. `exp2()`: [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`exponent()`: Return the exponent of a floating-point value, such as value = mantissa() * 2^exponent().
48. `mantissa()`: [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`exponential_distribution`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`extract(int pos, string from, string up1, string up2...)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`extreme_value_distribution()` function constructs an `extreme_value_distribution` object, adopting the distribution parameters specified by `a` and `b`.

Parameters: `a`, `b`
- `a`: Distribution parameter `a`, which defines the location (shift) of the distribution.
- `b`: Distribution parameter `b`, which defines the scale of the distribution. This shall be a positive value (b > 0).

It returns a `float` or an `fvector` of size `nb`.

### Normal Distributions

#### Normal Distribution

The `normal_distribution()`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`extreme_value_distribution`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`factors()`: Return the prime factor decomposition as an ivector.
24. `floor()`: [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`fail`: [Inference engine](https://github.com/naver/tamgu/tree/master/docs/English.md/45_Inference_engine.md)

`false`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md); [Type  iloop, floop, bloop, sloop, uloop](https://github.com/naver/tamgu/tree/master/docs/English.md/17_Type_iloop_floop_bloop_sloop_uloop.md); [Type  annotator](https://github.com/naver/tamgu/tree/master/docs/English.md/29_Type_annotator.md); [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`features()`: This method returns a map of the vocabulary with their feature values.
5. `initialization(map m)`: [Library word2vec type word2vec](https://github.com/naver/tamgu/tree/master/docs/English.md/59_Library_word2vec_type_word2vec.md)

`fgcolor(int c)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`fibre`: [Type  fibre](https://github.com/naver/tamgu/tree/master/docs/English.md/43_Type_fibre.md)

`file`: [Type  file, wfile](https://github.com/naver/tamgu/tree/master/docs/English.md/35_Type_file_wfile.md); [Python library pytamgu](https://github.com/naver/tamgu/tree/master/docs/English.md/56_Python_library_pytamgu.md)

`fileinfo(string path)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`fill(int nb, string char)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`find(string path)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`find(string s, bool case_insensitive)`: [Type  file, wfile](https://github.com/naver/tamgu/tree/master/docs/English.md/35_Type_file_wfile.md)

`find(string sub, int pos)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`find`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`findall(pred)`: [Inference engine](https://github.com/naver/tamgu/tree/master/docs/English.md/45_Inference_engine.md)

`first()`: Return the first element
13. `insert(wtreeitem e, string label, int pos)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`firstB`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`firstW`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`fisher_f_distribution`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`FL_TREE_CONNECTOR_DOTTED`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`FL_TREE_CONNECTOR_NONE`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`FL_TREE_CONNECTOR_SOLID`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`FL_TREE_REASON_CLOSED`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`FL_TREE_REASON_DESELECTED`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`FL_TREE_REASON_NONE`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`FL_TREE_REASON_OPENED`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`FL_TREE_REASON_SELECTED`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`FL_TREE_SELECT_MULTI`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`FL_TREE_SELECT_NONE`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`FL_TREE_SELECT_SINGLE`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`FL_TREE_SORT_ASCENDING`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`FL_TREE_SORT_DESCENDING`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`FL_TREE_SORT_NONE`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`flatten()`: Flattens a vector structure.
4. `insert(i, v)`: [Type  list](https://github.com/naver/tamgu/tree/master/docs/English.md/20_Type_list.md)

`flatten()`: Flattens a vector structure.
5. `insert(i,v)`: [Type  vector](https://github.com/naver/tamgu/tree/master/docs/English.md/19_Type_vector.md)

`float`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`floop`: [Type  iloop, floop, bloop, sloop, uloop](https://github.com/naver/tamgu/tree/master/docs/English.md/17_Type_iloop_floop_bloop_sloop_uloop.md)

`font(int c)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`font(int f, int sz)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`font(string s)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.3_GUI_ToolKit_library_libgui.md)

`fontnumber()`: Returns the number of available fonts.
19. `get(string msg)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`fontsize(int c)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md); [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.3_GUI_ToolKit_library_libgui.md)

`for`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md); [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`foregroundcolor(int color)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`format(p1, p2, p3)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`format(string f)`: [Type  date](https://github.com/naver/tamgu/tree/master/docs/English.md/32_Type_date.md)

`format(string fmt)`: [Type  chrono](https://github.com/naver/tamgu/tree/master/docs/English.md/34_Type_chrono.md)

`format(string form)`: [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`formatchar()`: Return the format char
9. `formatchar(string)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`fraction()`: Return the value as a fraction.
27. `isemoji()`: [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`Fraction`: [Type  fraction](https://github.com/naver/tamgu/tree/master/docs/English.md/18_Type_fraction.md)

`frame`: [Tamgu an introduction](https://github.com/naver/tamgu/tree/master/docs/English.md/3_Tamgu_an_introduction.md); [Frame](https://github.com/naver/tamgu/tree/master/docs/English.md/10_Frame.md); [Frame](https://github.com/naver/tamgu/tree/master/docs/English.md/10_Frame.md)

`frames`: [Frame](https://github.com/naver/tamgu/tree/master/docs/English.md/10_Frame.md)

`from`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`fthrough`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`function_name`: [Python library pytamgu](https://github.com/naver/tamgu/tree/master/docs/English.md/56_Python_library_pytamgu.md)

`function`: [Tamgu an introduction](https://github.com/naver/tamgu/tree/master/docs/English.md/3_Tamgu_an_introduction.md); [Basic  Types](https://github.com/naver/tamgu/tree/master/docs/English.md/5_Basic_Types.md); [Function autorun thread](https://github.com/naver/tamgu/tree/master/docs/English.md/7_Function_autorun_thread.md)

`fvector`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`gamma_distribution()` function constructs a `gamma_distribution` object, adopting the distribution parameters specified by `alpha` and `beta`.

Parameters: `alpha`, `beta`
- `alpha`: Parameter `alpha` (α), that defines the shape of the distribution. This shall be a positive value (α > 0).
- `beta`: Parameter `beta` (β), that defines the scale of the distribution. This shall be a positive value (β > 0).

It returns a `float` or an `fvector` of size `nb`.

#### Weibull Distribution

The `weibull_distribution()`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`gamma_distribution`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`geometric_distribution`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`get()`: gets one character from a socket.
7. `get(int clientid)`: [socket](https://github.com/naver/tamgu/tree/master/docs/English.md/48_socket.md)

`get()`: Reads one character from the file.
5. `get_signature()`: [Type  file, wfile](https://github.com/naver/tamgu/tree/master/docs/English.md/35_Type_file_wfile.md)

`getchar()`: Return the last character keyed in. `getchar` does not echo the characters on screen.

12. `colors(int style, int code1, int code2, bool disp)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`getchar`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`geterr()`: Catches the current error output.
- `getstd()`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`getfont(int num)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`getfontsizes(int num)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`getFrame(string name)`: [socket](https://github.com/naver/tamgu/tree/master/docs/English.md/48_socket.md)

`getFunction(string name)`: [socket](https://github.com/naver/tamgu/tree/master/docs/English.md/48_socket.md)

`getHostname()`: returns the current host name. The socket does not need to be activated to get this information.
11. `read()`: [socket](https://github.com/naver/tamgu/tree/master/docs/English.md/48_socket.md)

`GPSdistance`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`grammar_macros(key, pattern)`: [Type  Tamgu, Regular, Expression, treg](https://github.com/naver/tamgu/tree/master/docs/English.md/15_Type_Tamgu_Regular_Expression_treg.md)

`grammar_macros`: [Type  Tamgu, Regular, Expression, treg](https://github.com/naver/tamgu/tree/master/docs/English.md/15_Type_Tamgu_Regular_Expression_treg.md)

`grammar`: [Type  grammar](https://github.com/naver/tamgu/tree/master/docs/English.md/30_Type_grammar.md)

`handle`: [Python library pytamgu](https://github.com/naver/tamgu/tree/master/docs/English.md/56_Python_library_pytamgu.md)

`hide(bool v)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`home()`: Move cursor to home.

28. `cls()`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`hor_vert(int hor, int vert)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`hour()`: Returns the hour as an integer.
5. `min()`: [Type  date](https://github.com/naver/tamgu/tree/master/docs/English.md/32_Type_date.md)

`html()`: Returns the string into an HTML compatible string or as a vector of strings.
- `insert(i, s)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`id()`: return the id of the current node (only with call functions).
8. `name()`: [Library xml type xml](https://github.com/naver/tamgu/tree/master/docs/English.md/51_Library_xml_type_xml.md)

`iferror`: [try catch raise iferror](https://github.com/naver/tamgu/tree/master/docs/English.md/39_try_catch_raise_iferror.md)

`ifnot`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`iloop`: [Type  iloop, floop, bloop, sloop, uloop](https://github.com/naver/tamgu/tree/master/docs/English.md/17_Type_iloop_floop_bloop_sloop_uloop.md)

`image(image im, string label, int labelalign)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.3_GUI_ToolKit_library_libgui.md)

`image(image image, int x, int y, int w, int h)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`image`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`import(string python)`: [Python library pytamgu](https://github.com/naver/tamgu/tree/master/docs/English.md/56_Python_library_pytamgu.md)

`import`: [Python library pytamgu](https://github.com/naver/tamgu/tree/master/docs/English.md/56_Python_library_pytamgu.md)

`inferior`: [Frame](https://github.com/naver/tamgu/tree/master/docs/English.md/10_Frame.md)

`info(string name)`: [Predefined  Types](https://github.com/naver/tamgu/tree/master/docs/English.md/13_Predefined_Types.md)

`infos`: [Library LINEAR type linear](https://github.com/naver/tamgu/tree/master/docs/English.md/57_Library_LINEAR_type_linear.md)

`initialization`: [Library word2vec type word2vec](https://github.com/naver/tamgu/tree/master/docs/English.md/59_Library_word2vec_type_word2vec.md)

`initializefonts()`: Loads fonts from the system. Returns the number of available fonts.
25. `line(int x, int y, int x1, int y1, int x2, int y2)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`insert(l, label)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`insert(string s, int p)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.3_GUI_ToolKit_library_libgui.md)

`insert(table, column, value, ...)`: [Library sqlite type sqlite](https://github.com/naver/tamgu/tree/master/docs/English.md/52_Library_sqlite_type_sqlite.md)

`insertabove(wtreeitem e, string label)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`int`: [Function autorun thread](https://github.com/naver/tamgu/tree/master/docs/English.md/7_Function_autorun_thread.md); [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`invert()`: Returns a map with key/value inverted.
4. `items()`: [Type  tree, bin, prime, map](https://github.com/naver/tamgu/tree/master/docs/English.md/22_Type_tree_bin_prime_map.md)

`invert()`: Returns the inverted matrix
5. `product()`: [Type  fmatrix, imatrix](https://github.com/naver/tamgu/tree/master/docs/English.md/26_Type_fmatrix_imatrix.md)

`invert()`: Switches the denominator with the numerator of a fraction.
5. `n()`: [Type  fraction](https://github.com/naver/tamgu/tree/master/docs/English.md/18_Type_fraction.md)

`ioredirect`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`iorestate`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`isa(typename)`: [Predefined  Types](https://github.com/naver/tamgu/tree/master/docs/English.md/13_Predefined_Types.md)

`isactive()`: Return true if element is active
12. `isclosed()`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`isalpha()`: Tests if a string only contains alphabetical characters.
- `isconsonant()`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`isclosed(string path)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`isclosed(wtreeitem e)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`isdigit()`: Tests if a string only contains digits.
- `isemoji()`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`ishangul()`: Returns `true` if it is a Hangul character.
- `isjamo()`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`islower()`: Tests if a string only contains lowercase characters.
- `ispunctuation()`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`isopen()`: Return true if element is open
14. `isroot()`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`isselected()`: Return true if element is selected
16. `label()`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`isupper()`: Tests if a string only contains uppercase characters.
- `isutf8()`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`isvowel()`: Tests if a string only contains vowels.
- `last()`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`itembgcolor(int c)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`itemfgcolor(int c)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`itemfont(int c)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`itemsize(int c)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`ithrough`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`ivector`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`jamo(bool combine)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`join(string sep)`: [Type  vector](https://github.com/naver/tamgu/tree/master/docs/English.md/19_Type_vector.md); [Type  list](https://github.com/naver/tamgu/tree/master/docs/English.md/20_Type_list.md)

`join(string sepkey, string sepvalue)`: [Type  tree, bin, prime, map](https://github.com/naver/tamgu/tree/master/docs/English.md/22_Type_tree_bin_prime_map.md)

`join`: [Synchronization](https://github.com/naver/tamgu/tree/master/docs/English.md/44_Synchronization.md)

`json()`: Returns a JSON compatible string matching the container.
7. `keys()`: [Type  tree, bin, prime, map](https://github.com/naver/tamgu/tree/master/docs/English.md/22_Type_tree_bin_prime_map.md)

`json()`: Returns a JSON compatible string matching the container.
7. `last()`: [Type  list](https://github.com/naver/tamgu/tree/master/docs/English.md/20_Type_list.md)

`json()`: Returns a JSON compatible string matching the container.
8. `last()`: [Type  vector](https://github.com/naver/tamgu/tree/master/docs/English.md/19_Type_vector.md)

`key()`: Returns the key of the current element.
4. `next()`: [Type  iterator, riterator](https://github.com/naver/tamgu/tree/master/docs/English.md/31_Type_iterator_riterator.md)

`key`: [Type  tree, bin, prime, map](https://github.com/naver/tamgu/tree/master/docs/English.md/22_Type_tree_bin_prime_map.md); [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md); [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`keys`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`kget()`

The `kget()`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`label(string txt)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`label`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`labelcolor(int c)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`labelfont(int f)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`labels`: [Type  annotator](https://github.com/naver/tamgu/tree/master/docs/English.md/29_Type_annotator.md); [Library LINEAR type linear](https://github.com/naver/tamgu/tree/master/docs/English.md/57_Library_LINEAR_type_linear.md)

`labelsize(int i)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`labeltype(int i)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`lambda`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md); [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`last()`: Return the last element as a `wtreeitem`
22. `marginleft(int s)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`latin()`: Converts an UTF8 string to LATIN.
- `left(int nb)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`launch`: [Synchronization](https://github.com/naver/tamgu/tree/master/docs/English.md/44_Synchronization.md)

`left(int nb)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`let`: [Predefined  Types](https://github.com/naver/tamgu/tree/master/docs/English.md/13_Predefined_Types.md)

`levenshtein(string s)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`lexicon(transducer t)`: [Type  annotator](https://github.com/naver/tamgu/tree/master/docs/English.md/29_Type_annotator.md)

`lgamma()`: Return the log-gamma function.
29. `log()`: [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`lineshape(int type, int width)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`lisp`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`lispmode(bool)`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`lispmode`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`list`: [Type  list](https://github.com/naver/tamgu/tree/master/docs/English.md/20_Type_list.md); [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`listdirectory(string path)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`load(file, arguments, mapping)`: [Python library pytamgu](https://github.com/naver/tamgu/tree/master/docs/English.md/56_Python_library_pytamgu.md)

`load(filename)`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md); [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`load(ivector bm, int w, int h)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`load(rule, int skipblanks)`: [Type  grammar](https://github.com/naver/tamgu/tree/master/docs/English.md/30_Type_grammar.md)

`load(string file)`: [Type  transducer](https://github.com/naver/tamgu/tree/master/docs/English.md/28_Type_transducer.md)

`load(string filename)`: [Library xml type xmldoc](https://github.com/naver/tamgu/tree/master/docs/English.md/50_Library_xml_type_xmldoc.md)

`load(string pathname)`: [Library sound type sound](https://github.com/naver/tamgu/tree/master/docs/English.md/54_Library_sound_type_sound.md)

`load`: [Library LINEAR type linear](https://github.com/naver/tamgu/tree/master/docs/English.md/57_Library_LINEAR_type_linear.md)

`loadgif(string filename)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`loadin(pathname)`: [Loading  external, files, Type, Tamgu, vs, loadin](https://github.com/naver/tamgu/tree/master/docs/English.md/9_Loading_external_files_Type_Tamgu_vs_loadin.md)

`loadin(string pathname)`: [Loading  external, files, Type, Tamgu, vs, loadin](https://github.com/naver/tamgu/tree/master/docs/English.md/9_Loading_external_files_Type_Tamgu_vs_loadin.md)

`loadin`: [Loading  external, files, Type, Tamgu, vs, loadin](https://github.com/naver/tamgu/tree/master/docs/English.md/9_Loading_external_files_Type_Tamgu_vs_loadin.md)

`loadjpeg(string filename)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`loadmodel(string crfmodel)`: [Library wapiti type wapiti](https://github.com/naver/tamgu/tree/master/docs/English.md/58_Library_wapiti_type_wapiti.md)

`loadmodel(string filename)`: [Library LINEAR type linear](https://github.com/naver/tamgu/tree/master/docs/English.md/57_Library_LINEAR_type_linear.md)

`loadmodel(string filename, bool normalize)`: [Library word2vec type word2vec](https://github.com/naver/tamgu/tree/master/docs/English.md/59_Library_word2vec_type_word2vec.md)

`loadmodel`: [Library word2vec type word2vec](https://github.com/naver/tamgu/tree/master/docs/English.md/59_Library_word2vec_type_word2vec.md)

`lock()`: FLTK lock.
28. `menu(vector, int x, int y, int w, int h)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`logb()`: Return the floating-point base logarithm.
33. `nearbyint()`: [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`lognormal_distribution()` function constructs a `lognormal_distribution` object, adopting the distribution parameters specified by `m` and `s`.

Parameters: `m`, `s`
- `m`: Mean of the underlying normal distribution formed by the logarithm transformations of the possible values in this distribution.
- `s`: Standard deviation of the underlying normal distribution formed by the logarithm transformations of the possible values in this distribution. This shall be a positive value (s > 0).

It returns a `float` or an `fvector` of size `nb`.

#### Chi-squared Distribution

The `chi_squared_distribution()`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`lognormal_distribution`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`lookdown(string lemFeat, byte lemma)`: [Type  transducer](https://github.com/naver/tamgu/tree/master/docs/English.md/28_Type_transducer.md)

`lookdown`: [Type  transducer](https://github.com/naver/tamgu/tree/master/docs/English.md/28_Type_transducer.md)

`lookup(string wrd, int threshold, int flags)`: [Type  transducer](https://github.com/naver/tamgu/tree/master/docs/English.md/28_Type_transducer.md)

`lookup`: [Type  transducer](https://github.com/naver/tamgu/tree/master/docs/English.md/28_Type_transducer.md)

`lower()`: Returns the string in lowercase characters.
- `mid(int pos, int nb)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`ls(string path)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`map()`: Returns the predicate as a map
- `name()`: [Inference engine](https://github.com/naver/tamgu/tree/master/docs/English.md/45_Inference_engine.md)

`MapF`: [Specialized maps](https://github.com/naver/tamgu/tree/master/docs/English.md/23_Specialized_maps.md)

`MapI`: [Specialized maps](https://github.com/naver/tamgu/tree/master/docs/English.md/23_Specialized_maps.md)

`mapping`: [Python library pytamgu](https://github.com/naver/tamgu/tree/master/docs/English.md/56_Python_library_pytamgu.md)

`mapsi`: [Tamgu an introduction](https://github.com/naver/tamgu/tree/master/docs/English.md/3_Tamgu_an_introduction.md)

`MapU`: [Specialized maps](https://github.com/naver/tamgu/tree/master/docs/English.md/23_Specialized_maps.md)

`margintop(int s)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`mark()`: This method returns the mark on an object container.

c) `unmark()`: [Selfincluded containers](https://github.com/naver/tamgu/tree/master/docs/English.md/24_Selfincluded_containers.md)

`mark(byte v)`: [Selfincluded containers](https://github.com/naver/tamgu/tree/master/docs/English.md/24_Selfincluded_containers.md)

`match(string s)`: [Type  Tamgu, Regular, Expression, treg](https://github.com/naver/tamgu/tree/master/docs/English.md/15_Type_Tamgu_Regular_Expression_treg.md)

`mean`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`merge(v)`: [Type  vector](https://github.com/naver/tamgu/tree/master/docs/English.md/19_Type_vector.md); [Type  list](https://github.com/naver/tamgu/tree/master/docs/English.md/20_Type_list.md); [Type  tree, bin, prime, map](https://github.com/naver/tamgu/tree/master/docs/English.md/22_Type_tree_bin_prime_map.md)

`minus`: [Frame](https://github.com/naver/tamgu/tree/master/docs/English.md/10_Frame.md)

`mkdir(string path)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`mod`: [Frame](https://github.com/naver/tamgu/tree/master/docs/English.md/10_Frame.md)

`modal(bool b)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`mode_read`: [Type  file, wfile](https://github.com/naver/tamgu/tree/master/docs/English.md/35_Type_file_wfile.md)

`month()`: Returns the month as an integer.
7. `sec()`: [Type  date](https://github.com/naver/tamgu/tree/master/docs/English.md/32_Type_date.md)

`mousescrolldown(string key)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`mousescrollup(string key)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`mousetrack(string key)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`mouseup(string key)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`mousexy(string key)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`mthrough`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`multiply`: [Frame](https://github.com/naver/tamgu/tree/master/docs/English.md/10_Frame.md)

`multisplit(string sp1, string sp2...)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`multmatrix(float a, float b, float c, float d, float x, float y)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`myfunc`: [Type  call](https://github.com/naver/tamgu/tree/master/docs/English.md/36_Type_call.md); [Type  fibre](https://github.com/naver/tamgu/tree/master/docs/English.md/43_Type_fibre.md)

`n(int v)`: [Type  fraction](https://github.com/naver/tamgu/tree/master/docs/English.md/18_Type_fraction.md)

`name()`: Returns the predicate label
- `size()`: [Inference engine](https://github.com/naver/tamgu/tree/master/docs/English.md/45_Inference_engine.md)

`name(string n)`: [Library xml type xml](https://github.com/naver/tamgu/tree/master/docs/English.md/51_Library_xml_type_xml.md)

`name`: [socket](https://github.com/naver/tamgu/tree/master/docs/English.md/48_socket.md)

`namespace()`: Return the namespace of the current node as a vector.
11. `new(string n)`: [Library xml type xml](https://github.com/naver/tamgu/tree/master/docs/English.md/51_Library_xml_type_xml.md)

`nd(int n, int d)`: [Type  fraction](https://github.com/naver/tamgu/tree/master/docs/English.md/18_Type_fraction.md)

`negation`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`negative_binomial_distribution()` function constructs a `negative_binomial_distribution` object, adopting the distribution parameters specified by `k` and `p`.

Parameters: `k`, `p`
- `k`: Parameter `k` of the negative binomial distribution. This represents the number of unsuccessful trials that stops the count of successful Bernoulli-distributed experiments each generated value is said to simulate.
- `p`: Probability of success. This represents the probability of success on each of the independent Bernoulli-distributed experiments each generated value is said to simulate. This shall be a value between 0.0 and 1.0 (both included).

It returns an `int` or an `ivector` of size `nb`.

#### Geometric Distribution

The `geometric_distribution()`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`negative_binomial_distribution`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`new`: [Library xml type xml](https://github.com/naver/tamgu/tree/master/docs/English.md/51_Library_xml_type_xml.md)

`next()`: Return the next element
18. `parent()`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`next()`: return the next XML node.
13. `next(xml)`: [Library xml type xml](https://github.com/naver/tamgu/tree/master/docs/English.md/51_Library_xml_type_xml.md)

`next(wtreeitem e)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`next_line(int nb)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`ngrams(int n)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`node()`: Return the top node of the document.
5. `onclosing(function f, myobject o)`: [Library xml type xmldoc](https://github.com/naver/tamgu/tree/master/docs/English.md/50_Library_xml_type_xmldoc.md)

`norm(element)`: [Library word2vec type word2vec](https://github.com/naver/tamgu/tree/master/docs/English.md/59_Library_word2vec_type_word2vec.md)

`normal_distribution`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`normalizehangul()`: Normalizes different UTF8 encoding of Hangul characters.
- `romanization()`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`null`: [try catch raise iferror](https://github.com/naver/tamgu/tree/master/docs/English.md/39_try_catch_raise_iferror.md)

`nullp`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`numberp`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`odd()`: Return true if the value is odd.
35. `prime()`: [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`onclose(function, object)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`onkey(int action, function, object)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`onkey`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`onmouse(int action, function, object)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`onmouse`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`ontime(function, float t, object o)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`open(map params)`: [Library sound type sound](https://github.com/naver/tamgu/tree/master/docs/English.md/54_Library_sound_type_sound.md)

`open(string path)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`open(string pathname)`: [Library sqlite type sqlite](https://github.com/naver/tamgu/tree/master/docs/English.md/52_Library_sqlite_type_sqlite.md)

`open(wtreeitem e)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`open_append(string filename)`: [Type  file, wfile](https://github.com/naver/tamgu/tree/master/docs/English.md/35_Type_file_wfile.md)

`open_read(string filename)`: [Type  file, wfile](https://github.com/naver/tamgu/tree/master/docs/English.md/35_Type_file_wfile.md)

`open_write(string filename)`: [Type  file, wfile](https://github.com/naver/tamgu/tree/master/docs/English.md/35_Type_file_wfile.md)

`options(smap actions)`: [Library LINEAR type linear](https://github.com/naver/tamgu/tree/master/docs/English.md/57_Library_LINEAR_type_linear.md)

`options(string option, string|int parameter)`: [Library curl type curl WEB](https://github.com/naver/tamgu/tree/master/docs/English.md/55_Library_curl_type_curl_WEB.md)

`options(svector options)`: [Library wapiti type wapiti](https://github.com/naver/tamgu/tree/master/docs/English.md/58_Library_wapiti_type_wapiti.md)

`ord()`: Returns the code of a string character.
- `parse()`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`outmouse()`: Disable mouse tracking.

40. `ismouseaction(string key)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`outputfile`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`outputfilename`: [Library LINEAR type linear](https://github.com/naver/tamgu/tree/master/docs/English.md/57_Library_LINEAR_type_linear.md)

`parameters()`: Returns the parameters of the current sound file as a map.
7. `parameters(map modifs)`: [Library sound type sound](https://github.com/naver/tamgu/tree/master/docs/English.md/54_Library_sound_type_sound.md)

`parent()`: return the parent node above the current node.
15. `previous()`: [Library xml type xml](https://github.com/naver/tamgu/tree/master/docs/English.md/51_Library_xml_type_xml.md)

`parenthetic()`: Converts a parenthetic expression into a vector.
- `parenthetic(string opening, string closing, bool comma, bool separator, bool keepwithdigit, svector rules)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`parse(string buffer)`: [Library xml type xmldoc](https://github.com/naver/tamgu/tree/master/docs/English.md/50_Library_xml_type_xmldoc.md)

`parse(string sentence, int option, int threshold, int flags)`: [Type  transducer](https://github.com/naver/tamgu/tree/master/docs/English.md/28_Type_transducer.md)

`parse(ustring txt, bool keeplex)`: [Type  annotator](https://github.com/naver/tamgu/tree/master/docs/English.md/29_Type_annotator.md)

`parse`: [Type  transducer](https://github.com/naver/tamgu/tree/master/docs/English.md/28_Type_transducer.md)

`part`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`password(string msg)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`password(string user, string psswrd)`: [Library curl type curl WEB](https://github.com/naver/tamgu/tree/master/docs/English.md/55_Library_curl_type_curl_WEB.md)

`path`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`pathname`: [Library sound type sound](https://github.com/naver/tamgu/tree/master/docs/English.md/54_Library_sound_type_sound.md)

`pause`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`pie(int x, int y, int x1, int y1, float a1, float a2)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`piecewise_constant_distribution()` function uses the values in the range defined within `firstB` as the bounds for the subintervals (`bi`), and the sequence beginning at `firstW` as the weights (`wi`) for each subinterval.

It returns a `float` or an `fvector` of size `nb`.

Example:
```cpp
fvector intervals = [0.0, 2.0, 4.0, 6.0, 8.0, 10.0];
fvector weights = [2.0, 1.0, 2.0, 1.0, 2.0];
fvector res = piecewise_constant_distribution(100, intervals, weights);
```

#### Piecewise Linear Distribution

The `piecewise_linear_distribution()`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`pipe(string cmd)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`play()`: Plays the sound.
9. `play(bool beg)`: [Library sound type sound](https://github.com/naver/tamgu/tree/master/docs/English.md/54_Library_sound_type_sound.md)

`play(ivector soundbuffer)`: [Library sound type sound](https://github.com/naver/tamgu/tree/master/docs/English.md/54_Library_sound_type_sound.md)

`plot(fvector xy, int thickness, fvector landmark)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`plotcoords(fvector landmark, float x, float y)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`plus`: [Frame](https://github.com/naver/tamgu/tree/master/docs/English.md/10_Frame.md)

`point(int x, int y)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`poisson_distribution()` function constructs a `poisson_distribution` object, adopting the distribution parameter specified by `mean`.

Parameter: `mean`
- `mean`: Expected number of events in the interval (μ). This represents the rate at which the events being counted are observed, on average. Its value shall be positive (μ > 0).

It returns an `int` or an `ivector` of size `nb`.

#### Exponential Distribution

The `exponential_distribution()`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`poisson_distribution`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`polygon(int x, int y, int x1, int y1, int x2, int y2, int x3, int y3)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`pop()`: Removes the last character.
- `pop(i)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`pop(i)`: [Type  vector](https://github.com/naver/tamgu/tree/master/docs/English.md/19_Type_vector.md); [Type  list](https://github.com/naver/tamgu/tree/master/docs/English.md/20_Type_list.md)

`pop(key)`: [Type  tree, bin, prime, map](https://github.com/naver/tamgu/tree/master/docs/English.md/22_Type_tree_bin_prime_map.md)

`popclip()`: Releases a clip region.
41. `position()`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`popclip(int x, int y, int w, int h)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`popfirst(i)`: [Type  list](https://github.com/naver/tamgu/tree/master/docs/English.md/20_Type_list.md)

`poplast(i)`: [Type  list](https://github.com/naver/tamgu/tree/master/docs/English.md/20_Type_list.md)

`popmatrix()`
3. `scale(float x, float y)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`pos`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md); [Type  grammar](https://github.com/naver/tamgu/tree/master/docs/English.md/30_Type_grammar.md); [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.3_GUI_ToolKit_library_libgui.md)

`position(int x, int y)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`post(function f, …)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`power`: [Frame](https://github.com/naver/tamgu/tree/master/docs/English.md/10_Frame.md)

`predicatedump(pred)`: [Inference engine](https://github.com/naver/tamgu/tree/master/docs/English.md/45_Inference_engine.md)

`predict(fvector labels, vector data, bool predict_probability, bool infos)`: [Library LINEAR type linear](https://github.com/naver/tamgu/tree/master/docs/English.md/57_Library_LINEAR_type_linear.md)

`predictfromfile(string input, bool predict_probability, bool infos)`: [Library LINEAR type linear](https://github.com/naver/tamgu/tree/master/docs/English.md/57_Library_LINEAR_type_linear.md)

`preg`: [Type  Tamgu, Regular, Expression, treg](https://github.com/naver/tamgu/tree/master/docs/English.md/15_Type_Tamgu_Regular_Expression_treg.md)

`previous()`: Return the previous element
20. `value()`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`previous(wtreeitem e)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`previous(xml)`: [Library xml type xml](https://github.com/naver/tamgu/tree/master/docs/English.md/51_Library_xml_type_xml.md)

`previous_line(int nb)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`PrimeMapF`: [Specialized maps](https://github.com/naver/tamgu/tree/master/docs/English.md/23_Specialized_maps.md)

`PrimeMapI`: [Specialized maps](https://github.com/naver/tamgu/tree/master/docs/English.md/23_Specialized_maps.md)

`PrimeMapU`: [Specialized maps](https://github.com/naver/tamgu/tree/master/docs/English.md/23_Specialized_maps.md)

`principal`: [Synchronization](https://github.com/naver/tamgu/tree/master/docs/English.md/44_Synchronization.md)

`print`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`printerr`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`printj`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`printjerr`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`printjln`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`printjlnerr`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`println`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`printlnerr`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`private`: [Function autorun thread](https://github.com/naver/tamgu/tree/master/docs/English.md/7_Function_autorun_thread.md)

`product()`: Returns the product of elements.
11. `size()`: [Type  tree, bin, prime, map](https://github.com/naver/tamgu/tree/master/docs/English.md/22_Type_tree_bin_prime_map.md)

`product()`: Returns the product of elements.
12. `∏(v,i,j)`: [Type  vector](https://github.com/naver/tamgu/tree/master/docs/English.md/19_Type_vector.md)

`product()`: Returns the product of elements.
13. `push(v)`: [Type  list](https://github.com/naver/tamgu/tree/master/docs/English.md/20_Type_list.md)

`properties()`: Return the properties of the XML node.
18. `properties(map props)`: [Library xml type xml](https://github.com/naver/tamgu/tree/master/docs/English.md/51_Library_xml_type_xml.md)

`protected`: [Function autorun thread](https://github.com/naver/tamgu/tree/master/docs/English.md/7_Function_autorun_thread.md)

`proxy(string proxy)`: [Library curl type curl WEB](https://github.com/naver/tamgu/tree/master/docs/English.md/55_Library_curl_type_curl_WEB.md)

`push(v)`: [Type  vector](https://github.com/naver/tamgu/tree/master/docs/English.md/19_Type_vector.md)

`pushclip(int x, int y, int w, int h)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`pushevent`: [Tamgu an introduction](https://github.com/naver/tamgu/tree/master/docs/English.md/3_Tamgu_an_introduction.md)

`pushfirst(v)`: [Type  list](https://github.com/naver/tamgu/tree/master/docs/English.md/20_Type_list.md)

`pytamgu`: [Python library pytamgu](https://github.com/naver/tamgu/tree/master/docs/English.md/56_Python_library_pytamgu.md)

`python`: [Python library pytamgu](https://github.com/naver/tamgu/tree/master/docs/English.md/56_Python_library_pytamgu.md)

`PYTHONPATH`: [Python library pytamgu](https://github.com/naver/tamgu/tree/master/docs/English.md/56_Python_library_pytamgu.md)

`query(predicate|name,v1,v2,v3)`: [Inference engine](https://github.com/naver/tamgu/tree/master/docs/English.md/45_Inference_engine.md)

`quote`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`raise`: [try catch raise iferror](https://github.com/naver/tamgu/tree/master/docs/English.md/39_try_catch_raise_iferror.md)

`random()` function returns a long value. You can also provide a maximum boundary value as an argument. `a_random()`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`random()`.

Example:
```cpp
float rd = random(); // value between 0 and 99
rd = random(999); // value between 0 and 999
```

## Distributions

### Random Choice

The `random_choice()`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`rawstring`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`read()`, but extracts only "nb" characters or bytes from the file.
11. `readln()`: [Type  file, wfile](https://github.com/naver/tamgu/tree/master/docs/English.md/35_Type_file_wfile.md)

`read()`: Reads the whole file into a variable, which can be:
   - string: the whole document is stored in one string
   - svector: the document is split into strings along carriage returns, with each string stored in the container
   - bvector: the document is stored byte by byte in the container
   - ivector: the document is stored byte by byte in the container
10. `read(int nb)`: [Type  file, wfile](https://github.com/naver/tamgu/tree/master/docs/English.md/35_Type_file_wfile.md)

`read(int clientid)`: [socket](https://github.com/naver/tamgu/tree/master/docs/English.md/48_socket.md)

`read(string file)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`read(string path)`: [Type  vector](https://github.com/naver/tamgu/tree/master/docs/English.md/19_Type_vector.md)

`real()`: Returns the real part of the complex number.
2. `imaginary(int v)`: [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`realpath(string path)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`receive(int clientid, int nb)`: [socket](https://github.com/naver/tamgu/tree/master/docs/English.md/48_socket.md)

`receive(int nb)`: [socket](https://github.com/naver/tamgu/tree/master/docs/English.md/48_socket.md)

`rectangle(int x, int y, int w, int h, int c)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`rectanglefill(int x, int y, int w, int h, int c)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`redraw()`
34. `redrawing(float t)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`redraw()`: Redraws the window.
48. `redrawing(float t)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`remove()`: Removes the predicate from memory
- `store()`: [Inference engine](https://github.com/naver/tamgu/tree/master/docs/English.md/45_Inference_engine.md)

`remove(e)`: [Type  vector](https://github.com/naver/tamgu/tree/master/docs/English.md/19_Type_vector.md); [Type  list](https://github.com/naver/tamgu/tree/master/docs/English.md/20_Type_list.md)

`remove(wtreeitem e)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`removefirst(int nb)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`removelast(int nb)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`replace(sub, str)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`reserve(int sz)`: [Type  vector](https://github.com/naver/tamgu/tree/master/docs/English.md/19_Type_vector.md)

`reset()`: Reset mouse mode and return to initial termios values. To be used on Unix machines when `getchar` has been used.

49. `isescapesequence(string key)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`reset()`: Resets the clock.
2. `start()`: [Type  chrono](https://github.com/naver/tamgu/tree/master/docs/English.md/34_Type_chrono.md)

`reset()`: Resets the sound file to the beginning.
12. `stop()`: [Library sound type sound](https://github.com/naver/tamgu/tree/master/docs/English.md/54_Library_sound_type_sound.md)

`reset`: [Type  chrono](https://github.com/naver/tamgu/tree/master/docs/English.md/34_Type_chrono.md)

`resizable(object)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`resizecallback(function f)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`retract(pred(...))`: [Inference engine](https://github.com/naver/tamgu/tree/master/docs/English.md/45_Inference_engine.md)

`retractall(pred)`: [Inference engine](https://github.com/naver/tamgu/tree/master/docs/English.md/45_Inference_engine.md)

`return`: [Function autorun thread](https://github.com/naver/tamgu/tree/master/docs/English.md/7_Function_autorun_thread.md); [Alias](https://github.com/naver/tamgu/tree/master/docs/English.md/8_Alias.md); [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`reverse()`: Reverses a vector.
17. `size()`: [Type  list](https://github.com/naver/tamgu/tree/master/docs/English.md/20_Type_list.md)

`reverse()`: Reverses the string.
- `rfind(string sub, int pos)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`reverse()`: Reverses the vector.
18. `size()`: [Type  vector](https://github.com/naver/tamgu/tree/master/docs/English.md/19_Type_vector.md)

`rgbbackgroundcolor(int red, int green, int blue)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`rgbcolor(int color)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`rgbcolor(int r, int g, int b)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`rgbcolor(vector rgb)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`rgbcolor`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`rgbforegroundcolor(int red, int green, int blue)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`rgx`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`right(int nb)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md); [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`Ring`: [Type  chrono](https://github.com/naver/tamgu/tree/master/docs/English.md/34_Type_chrono.md)

`rint()`: Round to the nearest integral value.
37. `round()`: [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`root()`: Return the root element as a `wtreeitem`
30. `rootlabel(string r)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`root()`: return the root node of the XML tree.
20. `xmlstring()`: [Library xml type xml](https://github.com/naver/tamgu/tree/master/docs/English.md/51_Library_xml_type_xml.md)

`rotate(float d)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`rotation(float x, float y, float distance, float angle, bool draw)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`row_column(int row, int column)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`rules`: [Type  annotator](https://github.com/naver/tamgu/tree/master/docs/English.md/29_Type_annotator.md)

`run()`
37. `scrollbar(int x, int y, int wscroll, int hscroll, int vwscroll, int vhscroll)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`run()`: Executes the fibres that have been recorded.
2. `block()`: [Type  fibre](https://github.com/naver/tamgu/tree/master/docs/English.md/43_Type_fibre.md)

`run()`: Launches the GUI.
55. `scrollbar(int x, int y, int wscroll, int hscroll, int vwscroll, vhscroll)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`run(int client, string stopstring)`: [socket](https://github.com/naver/tamgu/tree/master/docs/English.md/48_socket.md)

`run(string code)`: [Python library pytamgu](https://github.com/naver/tamgu/tree/master/docs/English.md/56_Python_library_pytamgu.md)

`run(string sqlcommand)`: [Library sqlite type sqlite](https://github.com/naver/tamgu/tree/master/docs/English.md/52_Library_sqlite_type_sqlite.md)

`s("CURLOPT_URL", uri)`: [Library curl type curl WEB](https://github.com/naver/tamgu/tree/master/docs/English.md/55_Library_curl_type_curl_WEB.md)

`save(string filename, string encoding)`: [Library xml type xmldoc](https://github.com/naver/tamgu/tree/master/docs/English.md/50_Library_xml_type_xmldoc.md)

`savemodel(string outputfilename)`: [Library LINEAR type linear](https://github.com/naver/tamgu/tree/master/docs/English.md/57_Library_LINEAR_type_linear.md)

`scale(float x)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`scan(rgx, string sep, bool immediatematch, string remaining)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`scan(string grammar, string separator)`: [Type  file, wfile](https://github.com/naver/tamgu/tree/master/docs/English.md/35_Type_file_wfile.md)

`scan`: [Type  Tamgu, Regular, Expression, treg](https://github.com/naver/tamgu/tree/master/docs/English.md/15_Type_Tamgu_Regular_Expression_treg.md)

`screensizes()`: Return the screen size for rows and columns.

36. `hasresized()`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`scroll_down(int nb)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`scroll_up(int nb)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`scrollmargin(int top, int bottom)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`seek(int p)`: [Type  file, wfile](https://github.com/naver/tamgu/tree/master/docs/English.md/35_Type_file_wfile.md)

`select()`: Return selected string
13. `select(int i)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`select(uvector labels)`: [Type  annotator](https://github.com/naver/tamgu/tree/master/docs/English.md/29_Type_annotator.md)

`selection()`: Return the selected text in the input
9. `value()|(string v)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.3_GUI_ToolKit_library_libgui.md)

`selectioncolor(int color)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`selectmode(int s)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`self`: [Function autorun thread](https://github.com/naver/tamgu/tree/master/docs/English.md/7_Function_autorun_thread.md); [Predefined  Types](https://github.com/naver/tamgu/tree/master/docs/English.md/13_Predefined_Types.md); [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`send(int clientid, string s)`: [socket](https://github.com/naver/tamgu/tree/master/docs/English.md/48_socket.md)

`send(string s)`: [socket](https://github.com/naver/tamgu/tree/master/docs/English.md/48_socket.md)

`serialize(object)`: [Library xml type xmldoc](https://github.com/naver/tamgu/tree/master/docs/English.md/50_Library_xml_type_xmldoc.md)

`serializestring(object)`: [Library xml type xmldoc](https://github.com/naver/tamgu/tree/master/docs/English.md/50_Library_xml_type_xmldoc.md)

`set_signature(bool s)`: [Type  file, wfile](https://github.com/naver/tamgu/tree/master/docs/English.md/35_Type_file_wfile.md)

`setdate(year, month, day, hour, min, sec)`: [Type  date](https://github.com/naver/tamgu/tree/master/docs/English.md/32_Type_date.md)

`setpath(string path1, string path2, etc...)`: [Python library pytamgu](https://github.com/naver/tamgu/tree/master/docs/English.md/56_Python_library_pytamgu.md)

`setpath`: [Python library pytamgu](https://github.com/naver/tamgu/tree/master/docs/English.md/56_Python_library_pytamgu.md)

`setq`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`setTimeout(int i)`: [socket](https://github.com/naver/tamgu/tree/master/docs/English.md/48_socket.md)

`shape(d1,d2,d3)`: [Type  vector](https://github.com/naver/tamgu/tree/master/docs/English.md/19_Type_vector.md)

`shortcut(string keycode)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.3_GUI_ToolKit_library_libgui.md)

`show()`: Shows a widget.
14. `timeout(float f)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`showcursor(bool show)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`shuffle()`: Shuffles the values in the list.
19. `sum()`: [Type  list](https://github.com/naver/tamgu/tree/master/docs/English.md/20_Type_list.md)

`shuffle()`: Shuffles the values in the vector.
21. `sort(bool reverse | function | method)`: [Type  vector](https://github.com/naver/tamgu/tree/master/docs/English.md/19_Type_vector.md)

`sin()`: Return the sine.
39. `sinh()`: [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`size()`
39. `size(int x, int y, int w, int h)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`size()`: Return the number of values within the browser
15. `value()`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`size()`: Returns a 4-value vector of the window size.
57. `size(int x, int y, int w, int h)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`size()`: Returns the length of a string.
- `slice(int n)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`size`: [Type  vector](https://github.com/naver/tamgu/tree/master/docs/English.md/19_Type_vector.md)

`sizerange(int minw, int minh, int maxw, int maxh)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`skipblanks`: [Type  grammar](https://github.com/naver/tamgu/tree/master/docs/English.md/30_Type_grammar.md)

`sleep`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`sloop`: [Type  iloop, floop, bloop, sloop, uloop](https://github.com/naver/tamgu/tree/master/docs/English.md/17_Type_iloop_floop_bloop_sloop_uloop.md)

`Socket`: [socket](https://github.com/naver/tamgu/tree/master/docs/English.md/48_socket.md)

`sortfloat(bool reverse)`: [Type  vector](https://github.com/naver/tamgu/tree/master/docs/English.md/19_Type_vector.md)

`sortint(bool reverse | function)`: [Type  vector](https://github.com/naver/tamgu/tree/master/docs/English.md/19_Type_vector.md)

`sortorder(int s)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`sortstring(bool reverse)`: [Type  vector](https://github.com/naver/tamgu/tree/master/docs/English.md/19_Type_vector.md)

`sortustring(bool reverse | function)`: [Type  vector](https://github.com/naver/tamgu/tree/master/docs/English.md/19_Type_vector.md)

`soundbuffer`: [Library sound type sound](https://github.com/naver/tamgu/tree/master/docs/English.md/54_Library_sound_type_sound.md)

`split(string splitter)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`split`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`splite(string splitter)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`splitter`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`sqrt()`: Return the square root.
41. `tan()`: [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`stddev`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`stdin`: [Type  file, wfile](https://github.com/naver/tamgu/tree/master/docs/English.md/35_Type_file_wfile.md)

`sthrough`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`stokenize(map keeps)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`stop()`: Stops the chrono and returns the intermediate time.
4. `unit(int i)`: [Type  chrono](https://github.com/naver/tamgu/tree/master/docs/English.md/34_Type_chrono.md)

`stop`: [Inference engine](https://github.com/naver/tamgu/tree/master/docs/English.md/45_Inference_engine.md); [dependency and synode](https://github.com/naver/tamgu/tree/master/docs/English.md/46_dependency_and_synode.md)

`store(string output, bool norm, int encoding)`: [Type  transducer](https://github.com/naver/tamgu/tree/master/docs/English.md/28_Type_transducer.md)

`str`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`strict`: [Function autorun thread](https://github.com/naver/tamgu/tree/master/docs/English.md/7_Function_autorun_thread.md)

`string`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`student_distribution()` function constructs a `student_t_distribution` object, adopting the distribution parameter specified by `n`.

Parameter: `n`
- `n`: Degrees of freedom. Its value shall be positive (n > 0).

It returns a `float` or an `fvector` of size `nb`.

### Sampling Distributions

#### Discrete Distribution

The `discrete_distribution()`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`student_t_distribution`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`sub`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`sum()`: Returns the sum of all elements
7. `transpose()`: [Type  fmatrix, imatrix](https://github.com/naver/tamgu/tree/master/docs/English.md/26_Type_fmatrix_imatrix.md)

`sum()`: Returns the sum of elements.
13. `test(key)`: [Type  tree, bin, prime, map](https://github.com/naver/tamgu/tree/master/docs/English.md/22_Type_tree_bin_prime_map.md)

`sum()`: Returns the sum of elements.
27. `∑(v,i,j)`: [Type  vector](https://github.com/naver/tamgu/tree/master/docs/English.md/19_Type_vector.md)

`superior`: [Frame](https://github.com/naver/tamgu/tree/master/docs/English.md/10_Frame.md)

`switch`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`tags(string o, string c, bool comma, bool separator, bool keepwithdigit, svector rules)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`tail`: [Type  vector](https://github.com/naver/tamgu/tree/master/docs/English.md/19_Type_vector.md)

`tamgu`: [Loading  external, files, Type, Tamgu, vs, loadin](https://github.com/naver/tamgu/tree/master/docs/English.md/9_Loading_external_files_Type_Tamgu_vs_loadin.md)

`tamgucurl`: [Library curl type curl WEB](https://github.com/naver/tamgu/tree/master/docs/English.md/55_Library_curl_type_curl_WEB.md)

`TAMGULIBS`: [useOSlibrary](https://github.com/naver/tamgu/tree/master/docs/English.md/49_useOSlibrary.md)

`tanh()`: Return the hyperbolic tangent.
43. `tgamma()`: [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`tell()`: Returns the position of the file cursor.
15. `unget()`: [Type  file, wfile](https://github.com/naver/tamgu/tree/master/docs/English.md/35_Type_file_wfile.md)

`Test`: [Frame](https://github.com/naver/tamgu/tree/master/docs/English.md/10_Frame.md); [Type  tree, bin, prime, map](https://github.com/naver/tamgu/tree/master/docs/English.md/22_Type_tree_bin_prime_map.md)

`textsize(string l)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`thread`: [Function autorun thread](https://github.com/naver/tamgu/tree/master/docs/English.md/7_Function_autorun_thread.md)

`threshold(element)`: [Library word2vec type word2vec](https://github.com/naver/tamgu/tree/master/docs/English.md/59_Library_word2vec_type_word2vec.md)

`time`: [Type  chrono](https://github.com/naver/tamgu/tree/master/docs/English.md/34_Type_chrono.md)

`timezone(string tm)`: [Type  chrono](https://github.com/naver/tamgu/tree/master/docs/English.md/34_Type_chrono.md)

`toconvert`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`token`: [Type  annotator](https://github.com/naver/tamgu/tree/master/docs/English.md/29_Type_annotator.md)

`tokenize(bool comma, bool separator, svector rules)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`tooltip(string txt)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`toto`: [Function autorun thread](https://github.com/naver/tamgu/tree/master/docs/English.md/7_Function_autorun_thread.md)

`train()`: Launches training. Requires options to have been set in advance.
4. `label(vector words)`: [Library wapiti type wapiti](https://github.com/naver/tamgu/tree/master/docs/English.md/58_Library_wapiti_type_wapiti.md)

`train(string inputdata, smap options, string outputfilename)`: [Library LINEAR type linear](https://github.com/naver/tamgu/tree/master/docs/English.md/57_Library_LINEAR_type_linear.md)

`trainingset(fvector labels, vector data)`: [Library LINEAR type linear](https://github.com/naver/tamgu/tree/master/docs/English.md/57_Library_LINEAR_type_linear.md)

`trainmodel(vector v)`: [Library word2vec type word2vec](https://github.com/naver/tamgu/tree/master/docs/English.md/59_Library_word2vec_type_word2vec.md)

`transformdx(float x, float y)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`transformdy(float x, float y)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`transformedvertex(float xf, float yf)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`transformx(float x, float y)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`transformy(float x, float y)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`translate(float x, float y)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`TreeMapF`: [Specialized maps](https://github.com/naver/tamgu/tree/master/docs/English.md/23_Specialized_maps.md)

`TreeMapI`: [Specialized maps](https://github.com/naver/tamgu/tree/master/docs/English.md/23_Specialized_maps.md)

`TreeMapU`: [Specialized maps](https://github.com/naver/tamgu/tree/master/docs/English.md/23_Specialized_maps.md)

`treg`: [Type  Tamgu, Regular, Expression, treg](https://github.com/naver/tamgu/tree/master/docs/English.md/15_Type_Tamgu_Regular_Expression_treg.md)

`trim()`: Removes the trailing characters.
- `trimleft()`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`trimright()`: Removes the trailing characters on the right.
- `upper()`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`true`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md); [Type  iloop, floop, bloop, sloop, uloop](https://github.com/naver/tamgu/tree/master/docs/English.md/17_Type_iloop_floop_bloop_sloop_uloop.md); [Type  annotator](https://github.com/naver/tamgu/tree/master/docs/English.md/29_Type_annotator.md); [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`trunc()`: Return the value truncated to an integer.

### Specific to Floating Point Values

For floating-point values (float and decimal), the following methods are available:

45. `bits()`: [Type  byte, short, int, float, real, long](https://github.com/naver/tamgu/tree/master/docs/English.md/16_Type_byte_short_int_float_real_long.md)

`try`: [try catch raise iferror](https://github.com/naver/tamgu/tree/master/docs/English.md/39_try_catch_raise_iferror.md)

`type()`: Return the type of a variable as a string.

c) `methods()`: [Predefined  Types](https://github.com/naver/tamgu/tree/master/docs/English.md/13_Predefined_Types.md)

`type(int boxtype)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.3_GUI_ToolKit_library_libgui.md)

`typename`: [Predefined  Types](https://github.com/naver/tamgu/tree/master/docs/English.md/13_Predefined_Types.md)

`uloop`: [Type  iloop, floop, bloop, sloop, uloop](https://github.com/naver/tamgu/tree/master/docs/English.md/17_Type_iloop_floop_bloop_sloop_uloop.md)

`unget(nb)`: [Type  file, wfile](https://github.com/naver/tamgu/tree/master/docs/English.md/35_Type_file_wfile.md)

`uniform_int()` function produces random integer values `i`, uniformly distributed on the closed interval `[a, b]`, that is, distributed according to the discrete probability function.

Parameters: `a`, `b`
- `a`: Lower bound of the range of possible values the distribution can generate.
- `b`: Upper bound of the range of possible values the distribution can generate.

Note that `b` shall be greater than or equal to `a` (`a < b`).

It returns an `int` or an `ivector` of size `nb`.

#### Uniform Real

The `uniform_real()`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`uniform_real_distribution`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`unique()`: Removes duplicate elements.
29. `write(string file)`: [Type  vector](https://github.com/naver/tamgu/tree/master/docs/English.md/19_Type_vector.md)

`unlock()`

### Colors

Tamgu ltk library implements a few simple ways to select colors. Colors are implemented as int.

The predefined colors are as follows:

- FL_GRAY0
- FL_DARK3
- FL_DARK2
- FL_DARK1
- FL_LIGHT1
- FL_LIGHT2
- FL_LIGHT3
- FL_BLACK
- FL_RED
- FL_GREEN
- FL_YELLOW
- FL_BLUE
- FL_MAGENTA
- FL_CYAN
- FL_DARK_RED
- FL_DARK_GREEN
- FL_DARK_YELLOW
- FL_DARK_BLUE
- FL_DARK_MAGENTA
- FL_DARK_CYAN
- FL_WHITE

To define your own colors, use the `rgbcolor` method:

- `vector rgb = rgbcolor(int c)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`unlock()`: FLTK unlock.

### Drawing Complex Shape

The following instructions extend the above instructions:

1. `pushmatrix()`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`unlock(string s)`: [Synchronization](https://github.com/naver/tamgu/tree/master/docs/English.md/44_Synchronization.md)

`up(int nb)`: [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`url(string html)`: [Library curl type curl WEB](https://github.com/naver/tamgu/tree/master/docs/English.md/55_Library_curl_type_curl_WEB.md)

`url(string uri)`: [Library curl type curl WEB](https://github.com/naver/tamgu/tree/master/docs/English.md/55_Library_curl_type_curl_WEB.md)

`url(string uri, string filename)`: [Library curl type curl WEB](https://github.com/naver/tamgu/tree/master/docs/English.md/55_Library_curl_type_curl_WEB.md)

`url_callback`: [Library curl type curl WEB](https://github.com/naver/tamgu/tree/master/docs/English.md/55_Library_curl_type_curl_WEB.md)

`url`: [Library curl type curl WEB](https://github.com/naver/tamgu/tree/master/docs/English.md/55_Library_curl_type_curl_WEB.md)

`use("myname")`: [useOSlibrary](https://github.com/naver/tamgu/tree/master/docs/English.md/49_useOSlibrary.md)

`use("sound")`: [Library sound type sound](https://github.com/naver/tamgu/tree/master/docs/English.md/54_Library_sound_type_sound.md)

`use("tamgucurl")`: [Library curl type curl WEB](https://github.com/naver/tamgu/tree/master/docs/English.md/55_Library_curl_type_curl_WEB.md)

`use("xml")`: [Library xml type xmldoc](https://github.com/naver/tamgu/tree/master/docs/English.md/50_Library_xml_type_xmldoc.md); [Library xml type xml](https://github.com/naver/tamgu/tree/master/docs/English.md/51_Library_xml_type_xml.md)

`use(OS, library)`: [useOSlibrary](https://github.com/naver/tamgu/tree/master/docs/English.md/49_useOSlibrary.md)

`use`: [Specialized maps](https://github.com/naver/tamgu/tree/master/docs/English.md/23_Specialized_maps.md)

`ustring`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md); [Type  annotator](https://github.com/naver/tamgu/tree/master/docs/English.md/29_Type_annotator.md)

`utc()`: Returns the clock in UTC format.
2. `utc(string fmt)`: [Type  chrono](https://github.com/naver/tamgu/tree/master/docs/English.md/34_Type_chrono.md)

`uthrough`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`uuid()`

The `uuid()`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`value()`: Return the value of the current button
8. `when(int when1, int when2, ...)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.3_GUI_ToolKit_library_libgui.md)

`value(object)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`value(string v)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.3_GUI_ToolKit_library_libgui.md)

`values()`: Returns the map container values as a vector.

## Initialization
A map can be initialized with a description such as: { "k1":v1, "k2":v2...}
```cpp
map toto = { "a":1, "b":2 };
```

## Operator
- `x in amap`: Returns true or a list of indexes, according to the receiving variable. If the map contains string values, then the system will return true or its index only if a value is the same string as the one tested. The "in" operation is not performed in this case within the local strings.
- Important: `x` is tested against the keys of the map as for `test`.
- `for (s in amap) { ... }`: Loops among all keys. At each iteration, "s" contains a key from `amap`.
- `+`, `*`, `-`, `/` etc.: Adds a value to each element of a map or adds each element of a map to another along keys.
- `&`, `|`: Intersection or union of two maps along keys.

## Indexes
- `map[key]`: Returns the element whose key is `key`. If `key` is not a key from the map, then return null.
- As an integer or a float: Returns the size of the map.
- As a string: Returns a string that mimics the map initialization structure.

## Example
```cpp
map vmap;
vmap["toto"] = 1;
vmap[10] = 27;
print(vmap); // display: {'10':27,'toto':1}
```

## Testing Keys
There are different ways to test whether a map possesses a specific key. The first way is to use the `test` operator, which will return true or false. The other way is to catch the error when a wrong index is provided with the container. However, it is faster and more efficient to use `test` instead of the above equality.
```cpp
if (m.test("ee"))
    println("ee is not a key in m");
```
If you want to avoid an exception whenever a wrong key is used, place `erroronkey(false)`: [Type  tree, bin, prime, map](https://github.com/naver/tamgu/tree/master/docs/English.md/22_Type_tree_bin_prime_map.md)

`var`: [Synchronization](https://github.com/naver/tamgu/tree/master/docs/English.md/44_Synchronization.md); [ sys](https://github.com/naver/tamgu/tree/master/docs/English.md/47__sys.md)

`vector()`: Returns the predicate as a vector

```csharp
test(?X,?Q) :- ancestor(?X,?Q), female(?Q).
?:- var = test(?X,?Z);
println(var);
```

## Clauses

A basic clause structure:

```csharp
predicate(arg1, arg2, ..., argn) :- pred(arg...), pred(arg...), etc.
```

Facts can be declared as:

```csharp
predicate(val1, val2, ...) :- true.
// or
predicate(val1, val2, ...).
```

### Disjunction

Tamgu supports disjunctions in clauses using the ";" operator:

```csharp
parent(?X,?Y) :- mere(?X,?Y); pere(?X,?Y).
```

### Cut, fail, and stop

- `cut`: Expressed with "!"
- `fail`: Forces the failure of a clause
- `stop`: Stops the whole evaluation

## Functions

Tamgu provides Prolog-like functions such as:

- `asserta(pred(...))`: [Inference engine](https://github.com/naver/tamgu/tree/master/docs/English.md/45_Inference_engine.md)

`vector`: [Type  iloop, floop, bloop, sloop, uloop](https://github.com/naver/tamgu/tree/master/docs/English.md/17_Type_iloop_floop_bloop_sloop_uloop.md); [Type  list](https://github.com/naver/tamgu/tree/master/docs/English.md/20_Type_list.md)

`vlist`: [Type  list](https://github.com/naver/tamgu/tree/master/docs/English.md/20_Type_list.md)

`vocabulary()`: This method returns an iterator map of the vocabulary covered by the training.

## Options

The options for the library are supplied as a map. These options are the same as the ones expected by the word2vec tool.

Example options:
```cpp
map options = {
    "train": "input.txt",
    "output": "output.txt",
    "cbow": 1,
    "size": 200,
    "window": 5,
    "negative": 25,
    "hs": 0,
    "sample": 1e-4,
    "threads": 20,
    "binary": 1,
    "iter": 15
};
```

For a better explanation of these options, please read the appropriate information on the Word2Vec website. The most important options are:
- "train": This option should be associated with the file that will be used as training material.
- "output": The value for this key is the output file where the final training model will be stored.
- "window": This value defines the number of words taken into account as a proper context for a given token.
- "threads": Word2Vec utilizes threads to speed up the process. You can define the number of threads the system can use.
- "size": This value defines the size of the vector associated with each token.
- "iter": This value defines the number of iterations to build the model.

Once these options have been supplied, call the `initialization` method to set them in.

Example:
```cpp
use("word2vec");
word2vec wrd;

// Window will be 5 words around the main word.
// Vector size for each word will be 200.
// The system will use 20 threads to compute the final model with 15 iterations.
map options = {
    "train": "input.txt",
    "output": "output.bin",
    "cbow": 1,
    "size": 200,
    "window": 5,
    "negative": 25,
    "hs": 0,
    "sample": 1e-4,
    "threads": 20,
    "binary": 1,
    "iter": 15
};

wrd.initialization(options);
wrd.trainmodel();
```

## Usage

To use a trained model, you need to load it using the `loadmodel` method. After that, you can use the `distance`, `analogy`, or `accuracy` methods to find distances, analogies, or accuracies between words. All these methods return a list of words with their distances to the words in the input vectors. The vocabulary against which the words are compared is the one extracted from the input document. You can access all these words with the `vocabulary` method.

Example:
```cpp
use("word2vec");
word2vec wrd;

// Load the model obtained through training
wrd.loadmodel("output.bin");

svector v = ["word"];
fmap res = wrd.distance(v);
```

## Type w2vector

Each word extracted from the input document is associated with a specific vector whose size is defined at training time with the "size" option. In the example, the size is set to 200.

It is possible to extract a specific vector from the training vocabulary and store it into a specific object called w2vector.

### Methods

1. `dot(element)`: [Library word2vec type word2vec](https://github.com/naver/tamgu/tree/master/docs/English.md/59_Library_word2vec_type_word2vec.md)

`vocabulary`: [Library word2vec type word2vec](https://github.com/naver/tamgu/tree/master/docs/English.md/59_Library_word2vec_type_word2vec.md)

`vthrough`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md)

`wait()`: the server waits for a client to connect. It returns the client identifier, which will be used to communicate with the client.
20. `write(clientid, o1, o2...)`: [socket](https://github.com/naver/tamgu/tree/master/docs/English.md/48_socket.md)

`wait`: [Synchronization](https://github.com/naver/tamgu/tree/master/docs/English.md/44_Synchronization.md)

`waitonfalse(var)`: [Synchronization](https://github.com/naver/tamgu/tree/master/docs/English.md/44_Synchronization.md)

`waitonfalse`: [Synchronization](https://github.com/naver/tamgu/tree/master/docs/English.md/44_Synchronization.md)

`waitonjoined()` instruction after the thread calls to ensure proper synchronization:

```csharp
thread thread_name(parameters, ?_ result) {
    // Thread operations
    return true;
}

predicate predicate_name;
predicate_name(args) :-
    thread_name(args1, ?result1),
    thread_name(args2, ?result2),
    // More thread calls if needed
    waitonjoined(),
    // Further processing with results.
```

### Example: Parallel API Calls

```csharp
thread t_prompt(string model, string p, ?_ res) {
    res = ollama.promptnostream(uri, model, p);
    return true;
}

string p1 = @"Return a Python function that iterates from 1 to 100 and displays each value on screen."@;
string p2 = @"Return a Python function that computes the square of all odd numbers between 1 and 100 and stores them in a list."@;
string p3 = @"Return a Python function that returns the last 10 characters of a string given as input."@;

predicate appel;
appel(?P1, ?P2, ?P3, [?Y1, ?Y2, ?Y3]) :-
    t_prompt("phi3", ?P1, ?Y1),
    t_prompt("phi3", ?P2, ?Y2),
    t_prompt("phi3", ?P3, ?Y3),
    waitonjoined().

vector v = appel(p1, p2, p3, ?V);
println(v);
```

### Important Considerations

1. **Use of `waitonjoined()`: [Inference engine](https://github.com/naver/tamgu/tree/master/docs/English.md/45_Inference_engine.md)

`waitonjoined()` only waits for joined threads that were launched within a given thread.

### Atomic Values

Most data structures (maps, vectors, strings, etc.) are protected in threads against concurrent access through a lock. However, these locks can be costly in terms of time and space. To address this, Tamgu provides lock-free data structures (atomic types) such as `a_int`, `a_string`, or `a_mapii`. These data structures allow for a more efficient use of the machine, as threads accessing these types are not put on hold. However, their implementation makes them slower than other types in a non-threading context. They are only useful when shared across threads.

### Stream Operator '<<<'

When you launch a thread, the result of that thread cannot be directly stored in a variable using the `=` operator, as a thread lives its own life without any links to the calling code. Tamgu provides a specific operator for this task: `<<<`, also known as the stream operator. A stream is a variable that is connected to the thread in such a way that the values returned by the thread can be stored within that variable. Of course, the variable must exist independently from the thread.

Example:

```cpp
// We create a thread as a "join" thread, in order to be able to use waitonjoined.
// This thread simply returns 2 * x
joined thread Test(int i) {
    return (i * 2);
}

// Our launch function, which will launch 10 threads
function launch() {
    // We first declare our map storage variable within this function
    treemapi m;
    int i = 0;

    // We then launch 10 threads and store the result of each into m at a specific position
    for (i in <0,10,1>)
        m[i] <<< Test(i);

    // We wait for all threads to finish
    waitonjoined();

    // We display our final value
    println(m); // {0:0,1:2,2:4,3:6,4:8,5:10,6:12,7:14,8:16,9:18}
}

launch();
```

## Multiple Definitions

Tamgu allows for multiple definitions of functions sharing the same name but differing in their parameter definition. For example, you can implement a `display(string s)`: [Function autorun thread](https://github.com/naver/tamgu/tree/master/docs/English.md/7_Function_autorun_thread.md)

`waitonjoined()` to wait for these threads to finish. You can use as many `waitonjoined()`: [Function autorun thread](https://github.com/naver/tamgu/tree/master/docs/English.md/7_Function_autorun_thread.md)

`waitonjoined()` with flag `join`

When a thread must wait for other threads to finish before continuing, the simplest solution is to declare each of these threads as `join` and then use the method `waitonjoined()`: [Synchronization](https://github.com/naver/tamgu/tree/master/docs/English.md/44_Synchronization.md)

`waitonjoined()`: This function waits for threads launched within the current thread to terminate. These threads must be declared with the `join` flag.
7. `wait(string)`: [Synchronization](https://github.com/naver/tamgu/tree/master/docs/English.md/44_Synchronization.md)

`weibull_distribution`: [Random](https://github.com/naver/tamgu/tree/master/docs/English.md/38_Random.md)

`wfile`: [Type  file, wfile](https://github.com/naver/tamgu/tree/master/docs/English.md/35_Type_file_wfile.md)

`while`: [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md); [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`winput`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.3_GUI_ToolKit_library_libgui.md)

`with`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md); [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.1_GUI_ToolKit_library_libgui.md)

`word(int pos)`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.3_GUI_ToolKit_library_libgui.md)

`write(o1, o2...)`: [socket](https://github.com/naver/tamgu/tree/master/docs/English.md/48_socket.md)

`write(string file)`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`write(string s1, string s2, ...)`: [Type  file, wfile](https://github.com/naver/tamgu/tree/master/docs/English.md/35_Type_file_wfile.md)

`writebin(int s1, int s2, ...)`: [Type  file, wfile](https://github.com/naver/tamgu/tree/master/docs/English.md/35_Type_file_wfile.md)

`writelen(string s1, string s2, ...)`: [Type  file, wfile](https://github.com/naver/tamgu/tree/master/docs/English.md/35_Type_file_wfile.md)

`wstring`: [Type  rawstring, string, ustring](https://github.com/naver/tamgu/tree/master/docs/English.md/14_Type_rawstring_string_ustring.md)

`wtree`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`wtreeitem`: [GUI ToolKit library libgui](https://github.com/naver/tamgu/tree/master/docs/English.md/53.2_GUI_ToolKit_library_libgui.md)

`xml`: [Library xml type xml](https://github.com/naver/tamgu/tree/master/docs/English.md/51_Library_xml_type_xml.md)

`xmlNodePtr`: [Library xml type xml](https://github.com/naver/tamgu/tree/master/docs/English.md/51_Library_xml_type_xml.md)

`xmlstring()`: Return an XML document as a string.
11. `xpath(string myxpath)`: [Library xml type xmldoc](https://github.com/naver/tamgu/tree/master/docs/English.md/50_Library_xml_type_xmldoc.md)

`xor`: [Frame](https://github.com/naver/tamgu/tree/master/docs/English.md/10_Frame.md); [Specific instructions](https://github.com/naver/tamgu/tree/master/docs/English.md/37_Specific_instructions.md); [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

`year()`: Returns the year as an integer.
10. `yearday()`: [Type  date](https://github.com/naver/tamgu/tree/master/docs/English.md/32_Type_date.md)

`zerop`: [Functional Language à la Lisp](https://github.com/naver/tamgu/tree/master/docs/English.md/41_Functional_Language_à_la_Lisp.md)

