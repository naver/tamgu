# Error Handling in Python

In Python, there are several ways to handle errors: `try`, `catch`, `raise`, and `iferror`. These techniques are used to handle and manage errors in a program.

## Try-Catch Statement

The `try` and `catch` statements are used together to catch and handle errors in Python. The `catch` statement can be associated with a string or an integer parameter, which is automatically set to `null` when the `try` block is evaluated. It is also possible to use a `catch` statement without a variable.

```python
s = ""
try:
    # code that may raise an error
    ...
catch (s)
```

When an error is detected, the error message or its number is passed to the specified variable.

## The `raise` Method

The `raise` method is used to raise an error with a specific message. The error message should always start with an error number on three characters, such as "000". This error number should be larger than 200, as error numbers below 200 are reserved for internal KF errors. However, no verification will be made by the language.

```python
raise("201 My error")
```

## The `iferror` Statement

The `iferror` statement is used to check if an instruction fails. If the instruction fails, a specific code block is executed. The error message is then available in the variable `_iferror`.

```python
txt = ""
iferror(txt.read(path)):
    print("Error:", _iferror)
```

## Example

Here is an example that demonstrates the usage of the `raise` and `iferror` statements:

```python
raise("201 My error")
txt = ""
iferror(txt.read(path)):
    print("Error:", _iferror)
```

In this example, the `raise` statement raises an error with the message "201 My error". The `iferror` statement checks if the `txt.read(path)` instruction fails. If it does, the code block is executed and the error message is printed.