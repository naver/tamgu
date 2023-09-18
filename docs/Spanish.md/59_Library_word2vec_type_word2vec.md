# Biblioteca word2vec

La biblioteca **word2vec** en Tamgu proporciona una encapsulación de la herramienta word2vec. Para obtener más información sobre word2vec, visite el [sitio web de Word2Vec](https://code.google.com/p/word2vec/).

Esta biblioteca le permite entrenar el sistema en corpus y utilizar el modelo entrenado para encontrar distancias o analogías entre palabras.

## Métodos

1. `accuracy(vector palabras, int umbral)`: Este método encuentra las precisiones para un vector de palabras. Devuelve un mapa de frecuencias (fmap). Si no se proporciona el umbral, su valor predeterminado es 30000.
2. `analogy(svector palabras)`: Este método encuentra analogías para un grupo de palabras. Devuelve un mapa de frecuencias.
3. `distance(svector palabras)`: Este método encuentra la distancia entre palabras en un vector. Devuelve un mapa de frecuencias.
4. `features()`: Este método devuelve un mapa del vocabulario con sus valores de características.
5. `initialization(mapa m)`: Este método inicializa un conjunto de entrenamiento de word2vec.
6. `loadmodel(string nombre_archivo, bool normalizar)`: Este método carga un modelo de word2vec.
7. `trainmodel(vector v)`: Este método inicia el entrenamiento. Si no se proporciona el vector, el sistema utiliza el archivo de entrada dado en las opciones de inicialización.
8. `vocabulary()`: Este método devuelve un mapa de iteradores del vocabulario cubierto por el entrenamiento.

## Opciones

Las opciones para la biblioteca se suministran como un mapa. Estas opciones son las mismas que las esperadas por la herramienta word2vec.

Ejemplo de opciones:
```cpp
mapa opciones = {
    "train": "input.txt",
    "output": "output.txt",
    "cbow": 1,
    "size": 200,
    "window": 5,
    "negative": 25,
    "hs": 0,
    "sample": 1e-4,
    "threads": 20,
    "binary": 1,
    "iter": 15
};
```

Para obtener una mejor explicación de estas opciones, lea la información correspondiente en el sitio web de Word2Vec. Las opciones más importantes son:
- "train": Esta opción debe asociarse con el archivo que se utilizará como material de entrenamiento.
- "output": El valor para esta clave es el archivo de salida donde se almacenará el modelo de entrenamiento final.
- "window": Este valor define el número de palabras que se tienen en cuenta como contexto adecuado para un token dado.
- "threads": Word2Vec utiliza hilos para acelerar el proceso. Puede definir el número de hilos que el sistema puede utilizar.
- "size": Este valor define el tamaño del vector asociado con cada token.
- "iter": Este valor define el número de iteraciones para construir el modelo.

Una vez que se hayan suministrado estas opciones, llame al método `initialization` para configurarlas.

Ejemplo:
```cpp
use("word2vec");
word2vec wrd;

// La ventana será de 5 palabras alrededor de la palabra principal.
// El tamaño del vector para cada palabra será de 200.
// El sistema utilizará 20 hilos para calcular el modelo final con 15 iteraciones.
mapa opciones = {
    "train": "input.txt",
    "output": "output.bin",
    "cbow": 1,
    "size": 200,
    "window": 5,
    "negative": 25,
    "hs": 0,
    "sample": 1e-4,
    "threads": 20,
    "binary": 1,
    "iter": 15
};

wrd.initialization(opciones);
wrd.trainmodel();
```

## Uso

Para utilizar un modelo entrenado, es necesario cargarlo utilizando el método `loadmodel`. Después de eso, se pueden utilizar los métodos `distance`, `analogy` o `accuracy` para encontrar distancias, analogías o precisiones entre palabras. Todos estos métodos devuelven una lista de palabras con sus distancias a las palabras en los vectores de entrada. El vocabulario con el que se comparan las palabras es el extraído del documento de entrada. Se puede acceder a todas estas palabras con el método `vocabulary`.

Ejemplo:
```cpp
use("word2vec");
word2vec wrd;

// Cargar el modelo obtenido mediante el entrenamiento
wrd.loadmodel("output.bin");

svector v = ["palabra"];
fmap res = wrd.distance(v);
```

## Tipo w2vector

Cada palabra extraída del documento de entrada se asocia con un vector específico cuyo tamaño se define en el momento del entrenamiento con la opción "size". En el ejemplo, el tamaño se establece en 200.

Es posible extraer un vector específico del vocabulario de entrenamiento y almacenarlo en un objeto específico llamado w2vector.

### Métodos

1. `dot(elemento)`: Este método devuelve el producto punto entre dos palabras. El elemento puede ser una cadena o un w2vector.
2. `cosine(elemento)`: Este método devuelve la distancia del coseno entre dos palabras. El elemento puede ser una cadena o un w2vector.
3. `distance(elemento)`: Este método devuelve la distancia entre dos palabras. El elemento puede ser una cadena o un w2vector.
4. `threshold(elemento)`: Este método devuelve o establece el umbral.
5. `norm(elemento)`: Este método devuelve la norma del vector.

### Creación

Para crear un objeto w2vector, primero es necesario cargar un modelo y luego proporcionar tanto la cadena de token como un umbral.

Ejemplo:
```cpp
use("word2vec");
w2vector wrd;

// Cargar el modelo obtenido mediante el entrenamiento
wrd.loadmodel("output.bin");

w2vector w = wrd["tsunami": 0.5];
w2vector ww = wrd["earthquake": 0.5];
println(w.distance(ww));
```

El umbral no es obligatorio. Se utiliza al comparar dos elementos w2vector para ver si son cercanos. El umbral se utiliza para detectar si la distancia entre los dos elementos es mayor o igual al umbral.

En otras palabras, `if (w == ww)` es equivalente a `if (w.distance(ww) >= w.threshold())`.

Ejemplo:
```cpp
if (w == ww)
    println("ok");
if (w == "earthquake") // Se puede comparar con una simple cadena
    println("ok");
```