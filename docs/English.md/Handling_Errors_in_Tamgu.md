## Handling Errors in Tamgu

In Tamgu, you can use `try`, `catch`, and `raise` to handle errors.

### `try` and `catch`

The `try` block is used to enclose the code that may potentially generate an error. If an error occurs within the `try` block, it is caught by the `catch` block.

You can associate the `catch` block with a string or an integer parameter. This variable is automatically set to null when the `try` block is evaluated. It is also possible to use a `catch` block without a variable.

Example:

```tamgu
string s;
try {
    // code that may generate an error
}
catch (s);
```

When an error is detected, the error string or its number is passed to the specified variable.

### `raise` Method

The `raise` method is used to raise an error with a specific message. The error message should always start with a three-character error number. It is recommended to use an error number greater than 200, as numbers below 200 are reserved for internal KF errors. However, the language does not perform any verification on the error number.

Syntax:

```tamgu
raise(string s);
```

Example:

```tamgu
raise("201 My error");
```

In this example, the `raise` method raises an error with the message "201 My error".