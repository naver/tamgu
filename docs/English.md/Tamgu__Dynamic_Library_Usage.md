# Tamgu - Dynamic Library Usage

The `use(OS, library)` function in Tamgu allows you to load dynamic compatible libraries in a Tamgu program, enabling you to add new functionalities like graphical interfaces and database management. 

The `OS` flag is optional and can take one of the following values: "WINDOWS", "MACOS", "UNIX", "UNIX64". It is used to load specific libraries based on the platform architecture.

The library can be specified either by a simple name or by providing the full path to the library. If you choose to use a simple name, it must match a library name stored in the directory specified by the `TAMGULIBS` environment variable.

Library name conventions vary depending on the platform:

- On Unix platforms, library names are usually of the form `libmyname.so`. To load such a library, simply call `use("myname")`.
- On Windows, library names are usually of the form `myname.dll`. To load such a library, simply call `use("myname")`.

It is generally recommended to use the simple name format (`use("myname")`) to ensure compatibility across all platforms. However, if needed, you can provide the full path to limit the usage of the code to specific platforms. In such cases, the `OS` flag can be used to reintroduce a level of generalization: `use("WINDOWS", "Tamgu sqlite")`.