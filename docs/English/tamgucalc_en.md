# A Spreadsheet in Character Mode With Lisp Formulas: TAMGUCALC

[Version française](https://github.com/naver/tamgu/wiki/tamgucalc-(fr))

This extremely lightweight spreadsheet launches instantly into your terminal. It allows you to enter numbers, define labels, and most importantly, to enter numerical formulas in Lisp. You can then save your spreadsheets with their formulas in a CSV file. You can also export your worksheets, keeping only the raw data.

The code is less than 700 lines long and is entirely modifiable at your convenience, if you want to add your own options.

## Initialization

By default, tamgucalc uses the dimensions of your terminal to define the number of rows and columns.

### Column size

The columns have a default size of 10 characters. To change this size, you will have to modify the "columnsize" variable in the code. Note that your spreadsheets save this size as well as the number of rows and columns in its file.

## Enter a value

To enter a value, simply position the cursor on a cell with the arrows and enter your value. You can then move to the next cell with the arrows or press the "enter" key. A cell can contain numbers, formulas or labels.

## Enter a Lisp formula

To enter a formula, simply position on a cell and type: '('. When your formula is complete, the system checks that the parentheses balance and displays the message: "Ready to compute". 

*Just press: "enter" and it will be calculated.*

### Selecting a cell

When you type a formula, you can use the arrows to select the cells that go into your formula. When you have selected a cell, press "enter" to save it in your formula. 

### Selecting a cell range

To choose a cell range, you must first choose a first cell, then type ".." (two points) and then choose a second cell. tamgucalc then replaces your selection automatically with the following rules:

The choice of a cell appears in the following form:

* mat[i:j]: a single cell at positions i,j.
* mat[:c][a:b]: 'c' is the column number, 'a,b' are the rows from 'a' to 'b' in column 'c'.
* mat[r:][a:b]: 'r' is the row number, 'a,b' are the columns from 'a' to 'b' in row 'r'.

Note, the use of "r:" and ":c" to identify rows and columns.

**Important**: Column and row selections are translated as Tamgu vectors, which explains that in formulas that translate your selections, 'b' will always be "selection + 1".

### Multiple rows and columns

Note that when you define a domain that includes several rows and columns, tamgucalc introduces an "&&&" operator to merge all the selected rows into a single data vector. In this way, your selection becomes a single element.

#### Example:

```Lisp
(sum (&&& mat[1:][1:3] mat[2:][1:3])
```

## Formulas

Formulas in tamgucalc must be written in the Lisp defined for Tamgu [(see documentation)](https://github.com/naver/tamgu/blob/master/docs/tamgu.pdf). You can define lambdas functions, functions (defun) or simply use the basic operators.

* By default, all numerical values are "double" in the C direction (float type for tamgu).

### Operators

Traditional operators such as: +,-,/, * are of course available.

You can also use: "sum" and "product". Note that "+" is used for atomic values and that "sum" applies to both lists and atomic values.

The set of Lisp methods defined in Tamgu is also available, as well as the methods available for lists, such as "size" for example.

```Lisp
# addition
(+ mat[6:1] mat[7:1] mat[10:1] ) 

# Rest of a division
(% mat[6:1] mat[7:1])

# logarithm Nepean
(ln mat[9:1] )

# You can encapsulate your formulas
(* 2 (+ mat[6:1] mat[7:1] mat[10:1]))

# Apply a change to a list and add it up #
(sum (_map '(* 2) mat[:1][6:10])) 

```

### Lambdas

You can also define lambda functions in your cells and perform any calculation you want including recursions. 

```Lisp

# A simple function to calculate the average of a list
((lambda (x) (/ (sum x) (size x))) mat[1:][1:10])

# You can also implement recursions

( lambda (x) 
     (if (eq x ()) 
         0 
         (+ 
            (* 2 (car x)) 
            (self (cdr (cdr x)))) 
  mat[:1] [6:10])

```

### Functions

When you define a function with "defun" in a cell such as:

```Lisp
(defun avg(x) (/ (sum x) (size x))
```

The cell then takes the name of this function. You can now use it in your sheet as a new operator:

```Lisp
(avg mat[:1][1:8]) 
 ```


**Warning**: A function can only be defined once. Once a function has been compiled, it cannot be changed. To take into account the modification of a function, you must save your spreadsheet, exit the interpreter and restart it. It is better to write this function first as a lambda before making it final as a function.

## Basic commands

tamgucalc offers some options:

* Control+s: Save
* Control+w: Save as 
* Control+d: Save raw data.
* Control+r: allows you to change the number of rows and columns.
* Control+q: Quit

# Install tamgucalc

You can find "tamgucalc" at: [tamgucalc.tmg](https://github.com/naver/tamgu/blob/master/examples/miscelleaneous/tamgucalc.tmg)

To run "tamgucalc" you must also get the "tamgu" interpreter, precompiled versions of which are available at: 

* **Mac OS**: [Version for macos](https://github.com/naver/tamgu/releases/tag/tamgu.macos)
* ** **Linux** [Versions for linux:fedora, ubuntu, centos](https://github.com/naver/tamgu/releases/tag/tamgu.linux)


To launch tamgucalc: *tamgu tamgucalc.tmg (file)*

Note that *file* is optional...
