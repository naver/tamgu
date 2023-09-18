## 5 Tipos Básicos

Tamgu requiere que todos los elementos se declaren con un tipo específico. Los tipos pueden ser predefinidos o definidos por el usuario como un marco.

### 5.1 Tipos predefinidos

Tamgu propone algunos tipos muy básicos:

#### Objetos básicos:
- self
- string
- int
- decimal
- float
- long
- bit
- short
- fraction
- bool
- date
- time
- call

#### Contenedores:
- vector
- map
- imatrix
- fmatrix
- file
- iterator

#### Alias
Un alias es una función en la que los argumentos se declaran sin tipos. Un alias se puede usar para acceder a una estructura o para ejecutar algún código.

Nota: Todas las variables están sujetas a modificación en un alias.

#### Función
Una función se declara en cualquier parte del código utilizando la palabra clave `function`.

#### Marco
Un marco es un tipo definido por el usuario que es muy similar a una clase en otros lenguajes. Un marco contiene tantas variables o definiciones de funciones como sea necesario.

### Declaración de variables
Una variable se declara de manera similar a muchos otros lenguajes. Primero, se proporciona el tipo de la variable, seguido de una lista de nombres de variables separados por comas y terminando con un punto y coma.

Ejemplo:
```cpp
// Cada variable se puede instanciar individualmente en la lista
int i, j, k = 10;
string s1 = "s1", s2 = "s2";
private type name;
```

Una variable se puede declarar como privada, lo que prohíbe su acceso desde fuentes externas:
```cpp
private test toto;
```