## Using Dynamic Libraries in Tamgu Programs

The `use(OS, library)` function in Tamgu is used to load dynamic compatible libraries into a program. This allows for the addition of new functionalities such as graphical interfaces and database management. 

The `OS` flag is optional and can take one of the following values: "WINDOWS", "MACOS", "UNIX", "UNIX64". It is used to load specific libraries based on the platform architecture.

The library can be specified either by its simple name or by its full path. If using the simple name, it must match a library name stored in the directory specified by the `TAMGULIBS` environment variable.

Library name conventions vary depending on the platform:
- On Unix platforms, library names are typically in the form `libmyname.so`. To load such a library, simply call `use("myname")`.
- On Windows, library names are usually in the form `myname.dll`. To load such a library, simply call `use("myname")`.

It is generally recommended to use the simple name format (`use("myname")`) to ensure compatibility across platforms. However, if needed, the full path to the library can be specified, limiting the use of the code to specific platforms. In such cases, the `OS` flag can be used to reintroduce some level of generalization.

Example usage:
```python
use("WINDOWS", "Tamgu sqlite")
```