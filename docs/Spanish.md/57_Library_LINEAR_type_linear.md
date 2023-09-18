# Biblioteca LINEAR: Tipo Lineal

Tamgu proporciona una encapsulación de la biblioteca liblinear, que se utiliza para implementar clasificadores. Para obtener más información sobre esta biblioteca, visite [http://www.csie.ntu.edu.tw/~cjlin/liblinear/](http://www.csie.ntu.edu.tw/~cjlin/liblinear/).

## Métodos

La biblioteca expone los siguientes métodos:

1. `cleandata()`: Limpia los datos internos.
2. `crossvalidation()`: Vuelve a lanzar la validación cruzada con nuevos parámetros. El resultado es un fmap.
3. `loadmodel(string filename)`: Carga un modelo. Un modelo también se puede cargar automáticamente con un constructor.
4. `options(smap actions)`: Establece las opciones de entrenamiento (consulte a continuación).
5. `predict(fvector labels, vector data, bool predict_probability, bool infos)`: Predice a partir de un vector de treemapif. El parámetro `labels` es opcional. Cuando se proporciona, se utiliza para probar la etiqueta predicha frente a la etiqueta objetivo almacenada en `labels`. Si `infos` es verdadero, el primer elemento de este vector es un mapa de información.
6. `predictfromfile(string input, bool predict_probability, bool infos)`: Predice a partir de un archivo de entrada. El resultado es un vector. Si `infos` es verdadero, el primer elemento de este vector es un mapa de información.
7. `savemodel(string outputfilename)`: Guarda el modelo en un archivo.
8. `trainingset(fvector labels, vector data)`: Crea el conjunto de entrenamiento a partir de un vector de treemapif.
9. `train(string inputdata, smap options, string outputfilename)`: Entrena los datos de entrenamiento con algunas opciones. `outputfilename` es opcional. Se utilizará para almacenar el modelo final si se proporciona (el método también se puede llamar con el nombre `load`).

## Opciones de entrenamiento

Las opciones de entrenamiento deben proporcionarse como un smap, con las siguientes claves: `s`, `e`, `c`, `p`, `B`, `wi`, `M` y `v`.

1. `s` type: Establece el tipo de solucionador (por defecto 1). Los valores posibles son:
   - Para clasificación multiclase:
     - 0: Regresión logística regularizada por L2 (primal)
     - 1: Clasificación de vectores de soporte con pérdida L2 regularizada por L2 (dual)
     - 2: Clasificación de vectores de soporte con pérdida L2 regularizada por L2 (primal)
     - 3: Clasificación de vectores de soporte con pérdida L1 regularizada por L2 (dual)
     - 4: Clasificación de vectores de soporte por Crammer y Singer
     - 5: Clasificación de vectores de soporte con pérdida L2 regularizada por L1
     - 6: Regresión logística regularizada por L1
     - 7: Regresión logística regularizada por L2 (dual)
   - Para regresión:
     - 11: Regresión de vectores de soporte con pérdida L2 regularizada por L2 (primal)
     - 12: Regresión de vectores de soporte con pérdida L2 regularizada por L2 (dual)
     - 13: Regresión de vectores de soporte con pérdida L1 regularizada por L2 (dual)
2. `c` cost: Establece el parámetro C (por defecto 1).
3. `p` epsilon: Establece el epsilon en la función de pérdida de SVR (por defecto 0.1).
4. `e` epsilon: Establece la tolerancia del criterio de terminación. Los valores dependen del tipo `s`:
   - Para `s` 0 y 2: `|f'(w)|_2 <= eps*min(pos,neg)/l*|f'(w0)|_2`, donde `f` es la función primal y `pos/neg` son el número de datos positivos/negativos (por defecto 0.01).
   - Para `s` 11: `|f'(w)|_2 <= eps*|f'(w0)|_2` (por defecto 0.001).
   - Para `s` 1, 3, 4 y 7: Violación máxima dual <= eps; similar a libsvm (por defecto 0.1).
   - Para `s` 5 y 6: `|f'(w)|_1 <= eps*min(pos,neg)/l*|f'(w0)|_1`, donde `f` es la función primal (por defecto 0.01).
   - Para `s` 12 y 13: `|f'(alpha)|_1 <= eps |f'(alpha0)|`, donde `f` es la función dual (por defecto 0.1).
5. `B` bias: Si bias >= 0, la instancia x se convierte en [x; bias]; si < 0, no se agrega el término de sesgo (por defecto -1).
6. `wi` weight: Los pesos ajustan el parámetro C de las diferentes clases. 'i' aquí representa un índice. Una clave podría ser "w10", por ejemplo.
7. `M` type: Establece el tipo de clasificación multiclase (por defecto 0). Los valores posibles son:
   - `M` 0: uno-contra-todos
   - `M` 1: uno-contra-uno
8. `v` n: modo de validación cruzada de n pliegues.

Tenga en cuenta que es posible usar las siguientes cadenas en lugar de enteros para el solucionador:
- "L2R_LR" es 0
- "L2R_L2LOSS_SVC_DUAL" es 1
- "L2R_L2LOSS_SVC" es 2
- "L2R_L1LOSS_SVC_DUAL" es 3
- "MCSVM_CS" es 4
- "L1R_L2LOSS_SVC" es 5
- "L1R_LR" es 6
- "L2R_LR_DUAL" es 7
- "L2R_L2LOSS_SVR = 11" es 8
- "L2R_L2LOSS_SVR_DUAL" es 9
- "L2R_L1LOSS_SVR_DUAL" es 10

## Ejemplos

### Ejemplo de entrenamiento

```cpp
use("linear");
// Cargar la biblioteca
use("liblinear");

string trainFile = "output.dat";
// Declarar una variable liblinear
liblinear train;
// Establecer las opciones
map options = {"c": 100, "s": 'L2R_LR', "B": 1, "e": 0.01};
// Cargar el modelo y almacenar la salida del entrenamiento en el archivo "model_test"
train.load(trainFile, options, "model_test");
```

### Ejemplo de predicción

```cpp
use("linear");
// El archivo de entrada
string testFile = "trainData.dat";
// Declarar una variable liblinear con su modelo (también podríamos usar loadmodel en su lugar)
liblinear predict("model_test");
// Realizar la predicción desde un archivo
vector result = predict.predictfromfile(testFile, true);
```