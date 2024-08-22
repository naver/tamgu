# tamgulispe

tamgulispe is a powerful Lisp interpreter integrated into the Tamgu programming language. It provides a seamless way to incorporate Lisp functionality into Tamgu programs, offering a unique blend of Tamgu's features with Lisp's expressive power.

## Overview

tamgulispe allows you to execute Lisp code within Tamgu, providing a bridge between the two languages. When you use tamgulispe, it replaces the current Lisp implementation in Tamgu. This means that all Lisp operations within your Tamgu program will use this new implementation, ensuring consistency and providing access to the full range of LispE features.

## Key Features

1. **Lisp Execution**: Execute Lisp code directly within Tamgu programs.
2. **Seamless Integration**: Easily pass Tamgu variables and data structures to Lisp code and vice versa.
3. **Full Lisp Power**: Access to a wide range of Lisp functions and capabilities.
4. **Tamgu Interoperability**: Combine Tamgu's unique features with Lisp's functional programming paradigm.
5. **Regular Function Calls**: Call regular Tamgu functions from within LispE code, further enhancing the integration between the two languages.

## Argument Passing in eval and execute

Understanding how arguments are passed to `eval` and `execute` is crucial for effectively integrating Tamgu variables with LispE code. Both methods handle Tamgu variables by recreating them in the LispE space, allowing for seamless interaction between the two languages.

### Variable Handling in LispE Space

The key point to understand is that when Tamgu variables are passed to either `eval` or `execute`, they are not directly used. Instead:

1. **Variable Recreation**: The Tamgu variables are recreated as new variables within the LispE environment.
2. **Type Conversion**: The Tamgu variable types are converted to their closest LispE equivalents.
3. **Scope**: These recreated variables exist only within the scope of the LispE evaluation and do not affect the original Tamgu variables.

This approach ensures a clean separation between the Tamgu and LispE environments while still allowing for data exchange.

### eval Method

The `eval` method in tamgulispe is versatile in how it handles arguments:

1. **Multiple Arguments**: `eval` can take multiple arguments, which are processed sequentially.

   ```csharp
   int a = 10;
   int b = 20;
   l.eval("(+ ", a, " ", b, ")");  // Evaluates to 30 in LispE
   ```

2. **String Concatenation**: When passing multiple arguments, they are effectively concatenated into a single LispE expression before evaluation.

3. **Complex Data Structures**: Tamgu containers (like vectors, maps) are recreated as corresponding LispE structures.

   ```csharp
   vector v = [1, 2, 3];
   l.eval("(map (lambda (x) (* x 2)) ", v, ")");  // Returns (2 4 6) in LispE
   ```

4. Note the result of an `eval` is automatically converted into the corresponding Tamgu type.

   ```csharp
   vector v = [1, 2, 3];
   v = l.eval("(map (lambda (x) (* x 2)) ", v, ")");  // Returns (2 4 6) in LispE
   ```


### execute Method

The `execute` method is typically used for evaluating expressions without any cleaning afterward. For instance, if you want to record a function, you can use `execute` to do so. The argument passing is the same as `eval`.

   ```csharp
   //Records a tst function in LispE.
   l.execute("defun tst(a) (+ a 10))");
   ```


### Key Points to Remember

- **Variable Isolation**: Changes to recreated variables in LispE do not affect the original Tamgu variables.
- **Type Matching**: LispE attempts to match Tamgu types with the closest LispE equivalent.
- **Scope Limitation**: Variables recreated in LispE exist only for the duration of the `eval` or `execute` call.

### Examples of Variable Handling

```csharp
// Tamgu code
int x = 5;
string s = "hello";
vector v = [1, 2, 3];

// Using eval
l.eval("(println ", x, " ", s, " ", v, ")");
// In LispE, this recreates x as a number, s as a string, and v as a list

// Using execute
l.execute("(defun double (x) (* x 2))");
l.execute("(double " + x + ")");  // Returns 10, but Tamgu's x is still 5
```

By understanding this recreation process, you can effectively design your code to leverage both Tamgu and LispE capabilities while maintaining clear boundaries between the two environments.


## Usage

To use tamgulispe in your Tamgu programs:

1. Import the tamgulispe module:
   ```csharp
   use('tamgulispe');
   ```

2. Create a lispe object:
   ```csharp
   lispe l;
   ```

3. Execute Lisp code using the `execute` or `eval` methods:
   ```csharp
   l.execute("(defun xx (u) (+ u 3))");
   println(l.execute("(xx 10)"));
   ```

   Note: The main difference between `execute` and `eval` is that `eval` cleans up structures after use, while `execute` does not. This means that `eval` is more memory-efficient for one-time evaluations, while `execute` may be more performant for repeated calls to the same Lisp code.

4. Use the backslash (`\`) to directly embed Lisp expressions in Tamgu code:
   ```csharp
   println(\(xx a));
   println(\(+ (xx a) 3));
   ```

5. Pass Tamgu variables to Lisp code:
   ```csharp
   int a = 100;
   int b = 1000;
   println(l.eval("(+", a, b, "(xx", a, "))"));
   ```

6. Work with Tamgu data structures in Lisp:
   ```csharp
   treemap m;
   m["test"] = 10;
   m["truc"] = 20;
   l.eval(m, a, b);
   println(l.eval("(type m)"));
   ```

7. Call regular Tamgu functions from LispE:
   ```csharp
   // Assuming 'my_tamgu_function' is a defined Tamgu function
   l.eval("(my_tamgu_function 10 20)");
   ```

## Advanced Features

tamgulispe allows for complex interactions between Tamgu and Lisp, including:

- Defining and calling Lisp functions from Tamgu
- Using Tamgu data structures in Lisp code
- Evaluating Lisp expressions with Tamgu variables
- Seamless type conversion between Tamgu and Lisp
- Calling regular Tamgu functions from within LispE code

## LispE

tamgulispe is based on LispE, a powerful and flexible Lisp implementation. When you use tamgulispe, it completely replaces the current Lisp implementation in Tamgu with LispE. This ensures that you have access to all LispE features and functionalities throughout your Tamgu program.

For a comprehensive understanding of LispE's capabilities and syntax, please refer to the official LispE documentation:

[LispE Documentation](https://github.com/naver/lispe/wiki)

The LispE documentation provides in-depth information about:
- LispE syntax and semantics
- Available functions and macros
- Advanced features and optimizations
- Examples and use cases

By leveraging tamgulispe, you can harness the full power of LispE within your Tamgu programs, creating a uniquely powerful programming environment that combines the strengths of both languages. The ability to call regular Tamgu functions from LispE code further enhances this integration, allowing for seamless interoperability between the two languages.