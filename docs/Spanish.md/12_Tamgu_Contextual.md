# Tamgu Contextual

## Tamgu es un lenguaje de programación contextual.

En Tamgu, la forma en que se maneja una variable depende de su contexto de utilización. Cuando dos variables se utilizan juntas a través de un operador, el resultado de la operación depende del tipo de la variable a la izquierda, que introduce la operación. En el caso de una asignación, el tipo de la variable receptora determina el tipo de todo el grupo.

### Ejemplo

Si declaramos dos variables, una cadena y un entero, entonces el operador "+" puede actuar como una concatenación o una operación aritmética.

a) En este caso, "i" es la variable receptora, lo que hace que toda la instrucción sea una operación aritmética.

```
int i=10;
string s="12";
i=s+i;       // la "s" se convierte automáticamente en un número.
print("I="+i+"\n");
```

Salida:
```
I=22
```

b) En este otro caso, "s" es la variable receptora. La operación es ahora una concatenación.

```
int i=10;
string s="12";
s=s+i;       // la "i" se convierte automáticamente en una cadena.
print("S="+s+"\n");
```

Salida:
```
S=1210
```

## Conversión implícita

La noción de contexto es muy importante en Tamgu, ya que define cómo se debe interpretar cada variable. Las conversiones implícitas se procesan automáticamente para ciertos tipos. Por ejemplo, un entero se transforma automáticamente en una cadena, con sus propios dígitos como valor. En el caso de una cadena, el contenido se transforma en un número si la cadena solo contiene dígitos, de lo contrario es 0.

Para casos más específicos, como un vector o un mapa, las conversiones implícitas pueden ser un poco más complejas. Por ejemplo, un vector como un entero devuelve su tamaño, y como una cadena, una representación del vector. Un archivo como una cadena devuelve su nombre de archivo y como un entero, su tamaño en bytes.