## Uso de bibliotecas dinámicas en programas Tamgu

La función `use(OS, biblioteca)` en Tamgu se utiliza para cargar bibliotecas compatibles dinámicas en un programa. Esto permite agregar nuevas funcionalidades como interfaces gráficas y gestión de bases de datos.

La bandera `OS` es opcional y puede tomar uno de los siguientes valores: "WINDOWS", "MACOS", "UNIX", "UNIX64". Se utiliza para cargar bibliotecas específicas según la arquitectura de la plataforma.

La biblioteca se puede especificar tanto por su nombre simple como por su ruta completa. Si se utiliza el nombre simple, debe coincidir con un nombre de biblioteca almacenado en el directorio especificado por la variable de entorno `TAMGULIBS`.

Las convenciones de nombres de bibliotecas varían según la plataforma:
- En plataformas Unix, los nombres de las bibliotecas suelen tener la forma `libminombre.so`. Para cargar dicha biblioteca, simplemente llame a `use("minombre")`.
- En Windows, los nombres de las bibliotecas suelen tener la forma `minombre.dll`. Para cargar dicha biblioteca, simplemente llame a `use("minombre")`.

Generalmente se recomienda utilizar el formato de nombre simple (`use("minombre")`) para garantizar la compatibilidad entre plataformas. Sin embargo, si es necesario, se puede especificar la ruta completa de la biblioteca, limitando el uso del código a plataformas específicas. En estos casos, se puede utilizar la bandera `OS` para reintroducir cierto nivel de generalización.

Ejemplo de uso:
```python
use("WINDOWS", "Tamgu sqlite")
```