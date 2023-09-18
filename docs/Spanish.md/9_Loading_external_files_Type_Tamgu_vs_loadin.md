## Cargando archivos externos: Tipo Tamgu vs Loadin

El tipo `tamgu` se utiliza para cargar dinámicamente un programa Tamgu específico. Por otro lado, `loadin` se utiliza para cargar el contenido de un archivo en el programa actual.

### 9.1 Métodos

1. `tamgu var(string Tamgu pathname)`: Crea y carga un programa Tamgu.

### 9.2 Ejecución de funciones externas

Las funciones disponibles en el archivo Tamgu se pueden llamar a través de una variable `tamgu`.

#### Ejemplo

```cpp
// En nuestro programa test.tmg, implementamos la función: Read
// En test.Tamgu
function Read(string s) {
    return (s + "_toto");
}
//-------------------------------------------
// En call.Tamgu
// En nuestro programa de llamada, primero cargamos test.Tamgu, luego ejecutamos Read
tamgu kf('c:\test.tmg'); // Cargamos un programa que implementa Read
string s = kf.Read("xxx"); // Podemos ejecutar Read en nuestro programa local.
```

### Funciones privadas

Si no desea que los programas externos accedan a funciones específicas, puede protegerlas declarándolas como privadas.

#### Ejemplo

```cpp
// Implementamos una función que no puede ser llamada desde el exterior
private function CannotBeCalled(string s) { ... }
```

### `loadin(string pathname)`

Tamgu también proporciona otra forma de cargar archivos en otro programa. `loadin(pathname)` carga el contenido de un archivo en el programa actual. En otras palabras, `loadin` funciona exactamente como si el archivo actual contuviera el código de lo que se carga con `loadin`.

El código se carga en el lugar exacto en el que se escribe la llamada a `loadin`.

#### Ejemplo

```cpp
// En nuestro programa test.tmg, implementamos la función: Read
// En test.Tamgu
function Read(string s) {
    return (s + "_toto");
}
//-------------------------------------------
// En call.Tamgu
// En nuestro programa de llamada, primero cargamos test.Tamgu, luego ejecutamos Read
loadin('c:\test.tmg'); // Cargamos el código que implementa Read
string s = Read("xxx"); // Podemos ejecutar Read en nuestro programa local directamente.
```