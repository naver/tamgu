# Biblioteca Tamgu SQLite

Tamgu proporciona una biblioteca simple para manejar bases de datos SQLite. SQLite es un sistema de base de datos popular que utiliza archivos simples para ejecutar comandos SQL. Si necesita más información sobre SQLite, puede encontrar muchos recursos en la web.

Para usar la biblioteca SQLite en Tamgu, debe importarla utilizando el siguiente comando:

```python
use("sqlite")
```

## Métodos

La biblioteca SQLite proporciona los siguientes métodos:

1. `begin()`: Entra en el modo de confirmación con el modo DEFERRED.
2. `begin(string mode)`: Entra en el modo de confirmación con el modo especificado (DEFERRED, EXCLUSIVE, IMMEDIATE).
3. `close()`: Cierra la base de datos.
4. `commit()`: Procesa los comandos SQL. Esto se debe usar para finalizar una serie de comandos iniciados con `begin()`.
5. `create(x1, x2, x3)`: Crea una tabla en la base de datos con los argumentos especificados.
6. `execute(string sqlcommand)`: Ejecuta un comando SQL sin un callback.
7. `insert(table, column, value, ...)`: Inserta una línea en una tabla.
8. `open(string pathname)`: Abre una base de datos.
9. `run(string sqlcommand)`: Ejecuta un comando SQL con un callback para almacenar resultados. Si la variable de entrada es un vector, todos los valores posibles se almacenarán en él. Si la variable de entrada es un iterador, es posible iterar sobre los resultados del comando SQL. Cada resultado es un mapa donde cada clave es un nombre de columna.

## Ejemplo

Aquí hay un ejemplo de cómo usar la biblioteca SQLite:

```python
// Importar la biblioteca SQLite
use("sqlite")

// Declarar una nueva variable SQLite
sqlite mydb;

// Abrir una base de datos. Si no existe, se creará
mydb.open('test.db');

try {
    // Crear una nueva tabla en la base de datos actual
    mydb.create("table1", "nom TEXT PRIMARY KEY", "age INTEGER");
    println("table1 se ha creado");
} catch () {
    // La base de datos ya existe
    println("Ya creada");
}

int i;
string nm;

// Insertar valores en la base de datos usando el modo de confirmación (más rápido)
mydb.begin();

// Insertar 5000 elementos
for (i = 0; i < 5000; i += 1) {
    nm = "tiia_" + i;
    try {
        // Insertar dos valores en table1, uno para 'nom' y otro para 'age'
        // Observa la alternancia entre los nombres de columna y los valores
        mydb.insert("table1", "nom", nm, "age", i);
        println(i);
    } catch () {
        println("Ya insertado");
    }
}

// Confirmar los comandos
mydb.commit();

// Iterar sobre los valores para un comando SQL dado
iterator it = mydb.run("select * from table1 where age > 10;");
for (it.begin(); it.end() == false; it.next()) {
    println("Valor: ", it.value());
}

// Alternativamente, podríamos haber obtenido el mismo resultado con:
// vector v = mydb.execute("select * from table1 where age > 10;");
// Sin embargo, existe el riesgo de desbordar nuestro vector, por lo que no se recomienda.

// Cerrar la base de datos
mydb.close();
```