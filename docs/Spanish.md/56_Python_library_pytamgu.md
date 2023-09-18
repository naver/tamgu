# pytamgu

La biblioteca `pytamgu` es una biblioteca dual que se puede utilizar tanto como una biblioteca de Python como una biblioteca de Tamgu. Permite ejecutar código Python desde un programa Tamgu o ejecutar un programa Tamgu desde código Python.

## Como una biblioteca de Tamgu

Cuando se utiliza como una biblioteca de Tamgu, `pytamgu` expone un nuevo tipo llamado `python`. Los tipos base de Tamgu (booleano, entero, largo, flotante, fracción, cadena, contenedores vectoriales y contenedores de mapas) se mapean automáticamente a los tipos correspondientes de Python.

El tipo `python` expone los siguientes métodos:

1. `close()`: Cierra la sesión actual de Python.
2. `execute(string funcname, p1, p2...)`: Ejecuta una función de Python con `p1`, `p2`, etc. como parámetros.
3. `import(string python)`: Importa un archivo de Python.
4. `run(string code)`: Ejecuta código Python.
5. `setpath(string path1, string path2, etc...)`: Agrega rutas del sistema a Python.

El método `setpath` es crucial para utilizar el método `import`, que funciona de la misma manera que la palabra clave `import` en Python. Si desea importar un programa de Python en una ubicación específica que no se ha referenciado a través de `PYTHONPATH`, primero debe agregarlo con `setpath`.

### Ejemplo

Primero, implementamos un pequeño programa de Python llamado `testpy.py`:

```python
val = "aquí"
# Las variables de entrada se traducen automáticamente de Tamgu a variables de Python
def Iteste(s, v):
    v.append(s)
    v.append(val)
    return v
```

Luego, implementamos nuestro propio programa Tamgu, que llamará a este archivo (asumiendo que está en el mismo directorio que nuestro programa Tamgu):

```tamgu
// Necesitamos usar Pytamgu por nuestra cuenta
use("pytamgu");

// Necesitamos una variable para manejar el manejo de Python
python p;

// Suponemos que nuestro programa de Python está en el mismo directorio que nuestro programa Tamgu
p.setpath(_paths[1]);

// Luego importamos nuestro programa
p.import("testpy");

vector v;
string s = "kkk";

// Ejecutamos la función de Python Itest, que toma como entrada una cadena y un vector,
// que se convertirán automáticamente en objetos de Python sobre la marcha.
// La salida se convierte automáticamente en un vector Tamgu (del vector Python)
vector vv = p.execute("Itest", s, v);

println(vv); // La salida es: ['kkk', 'aquí']

p.close(); // Cerramos la sesión
```

## Como una biblioteca de Python

Cuando se utiliza como una biblioteca de Python, se puede importar la biblioteca `pytamgu`, que expone dos métodos.

1. `load(file, arguments, mapping)`
   - `file` es el nombre de archivo del archivo Tamgu que se cargará.
   - `arguments` es una cadena que proporciona argumentos al archivo Tamgu separados por un espacio.
   - Si `mapping` está configurado en 1, se crea un método de Python para cada función en el archivo Tamgu con el mismo nombre que las funciones de Tamgu.
   - Este método devuelve un identificador.

2. `execute(handle, function_name, [arg1, arg2, ..., argn])`
   - `handle` es el identificador del archivo que contiene la función que queremos ejecutar.
   - `function_name` es el nombre de la función en el archivo Tamgu.
   - `[arg1, arg2, ..., argn]` es la lista de argumentos que se proporcionarán al programa Tamgu como un vector de cadenas.

Si utiliza la opción de mapeo, el método `execute` es opcional. Los valores devueltos por el programa Tamgu se traducen automáticamente en objetos de Python, y lo mismo se aplica a los argumentos transmitidos al programa Tamgu.

**Nota:** Tamgu siempre devuelve cadenas Unicode de Python.

### Ejemplo

Programa Tamgu:

```tamgu
vector v = [1..10];

function rappel(string s, int j) {
    j += 10;
    v.push(j);
    v.push(s);
    return v;
}
```

Programa Python:

```python
import pytamgu

h0 = pytamgu.load("rappel.kif", "", 1)
# Usamos el mapeo a una función de Python
v = rappel("Test", 10)
for i in v:
    print(i)
# Esto es equivalente a
v = pytamgu.execute(h0, "rappel", ["Test", 10])
```