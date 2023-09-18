# Biblioteca wapiti

libwapiti es una encapsulación de la biblioteca wapiti, que está disponible en [http://wapiti.limsi.fr](http://wapiti.limsi.fr).

Derechos de autor (c) 2009-2013 CNRS. Todos los derechos reservados.

Wapiti proporciona una implementación eficiente del método CRF para etiquetado y extracción de entidades. Si necesita información sobre el sistema, consulte el manual en [http://wapiti.limsi.fr/manual.html](http://wapiti.limsi.fr/manual.html).

## Uso

Para usar la biblioteca, debes cargar el módulo wapiti:

```python
use('wapiti')
```

La biblioteca expone los siguientes métodos:

### Métodos

1. `loadmodel(string crfmodel)`: Carga un modelo CRF.
2. `options(svector options)`: Establece opciones. Consulte a continuación las opciones disponibles. Las opciones deben colocarse en el svector como se usa en la línea de comandos de wapiti.
3. `train()`: Inicia el entrenamiento. Requiere que las opciones se hayan establecido previamente.
4. `label(vector words)`: Inicia el etiquetado para un vector de tokens. Devuelve un vector de etiquetas para cada token.
5. `lasterror()`: Devuelve el último error que ocurrió.

### Opciones

Wapiti expone algunas opciones para manejar todas las posibilidades inherentes al sistema. A continuación se muestra una lista de estas opciones, que deben suministrarse como un svector, exactamente como se proporcionarían estas opciones con la versión de línea de comandos de wapiti.

#### Modo de entrenamiento:

- `train [options] [input data] [model file]`
  - `--me`: Forzar el modo maxent
  - `--type STRING`: Tipo de modelo a entrenar
  - `--algo STRING`: Algoritmo de entrenamiento a utilizar
  - `--pattern FILE`: Patrones para extraer características
  - `--model FILE`: Archivo de modelo para precargar
  - `--devel FILE`: Conjunto de datos de desarrollo
  - `--rstate FILE`: Estado del optimizador para restaurar
  - `--sstate FILE`: Estado del optimizador para guardar
  - `--compact`: Compactar el modelo después del entrenamiento
  - `--nthread INT`: Número de hilos de trabajo
  - `--jobsize INT`: Tamaño del trabajo para los hilos de trabajo
  - `--sparse`: Habilitar avance/retroceso disperso
  - `--maxiter INT`: Número máximo de iteraciones
  - `--rho1 FLOAT`: Parámetro de penalización L1
  - `--rho2 FLOAT`: Parámetro de penalización L2
  - `--objwin INT`: Tamaño de la ventana de convergencia
  - `--stopwin INT`: Tamaño de la ventana de parada
  - `--stopeps FLOAT`: Valor de épsilon de parada
  - `--clip`: Recortar el gradiente (l-bfgs)
  - `--histsz INT`: Tamaño del historial (l-bfgs)
  - `--maxls INT`: Máximo de iteraciones de búsqueda de línea (l-bfgs)
  - `--eta0 FLOAT`: Tasa de aprendizaje (sgd-l1)
  - `--alpha FLOAT`: Parámetro de decaimiento exponencial (sgd-l1)
  - `--kappa FLOAT`: Parámetro de estabilidad (bcd)
  - `--stpmin FLOAT`: Tamaño mínimo de paso (rprop)
  - `--stpmax FLOAT`: Tamaño máximo de paso (rprop)
  - `--stpinc FLOAT`: Factor de incremento de paso (rprop)
  - `--stpdec FLOAT`: Factor de decremento de paso (rprop)
  - `--cutoff`: Proyección alternativa (rprop)

#### Modo de etiquetado:

- `label [options] [input data] [output data]`
  - `--me`: Forzar el modo maxent
  - `--model FILE`: Archivo de modelo para cargar
  - `--label`: Solo salida de etiquetas
  - `--check`: La entrada ya está etiquetada
  - `--score`: Agregar puntuaciones a la salida
  - `--post`: Etiquetar usando posteriores
  - `--nbest INT`: Lista n-best de salida
  - `--force`: Usar decodificación forzada

## Entrenamiento

Para entrenar un CRF, necesitas un archivo de texto con anotaciones, donde cada línea es un token con sus etiquetas separadas por una pestaña.

Ejemplo:

```
UNITED STATES  NOUN  LOCATION_b
SECURITIES  NOUN  ORGANISATION_i
AND  CONJ  ORGANISATION_i
EXCHANGE  NOUN  ORGANISATION_i
COMMISSION  NOUN  ORGANISATION_i
Washington  NOUN  ORGANISATION_i
, PUNCT  ORGANISATION_i
D.C. NOUN  LOCATION_b
20549  DIG NUMBER_b
FORM  VERB  null
N NOUN  null
```

En este ejemplo, cada línea representa un token asociado con dos etiquetas diferentes. La etiqueta "null" en este ejemplo es una cadena simple que no tiene una interpretación específica excepto para este ejemplo específico.

También necesitas un archivo "pattern", que debe implementarse de acuerdo con el manual de CRF++ o como se describe en el [manual de Wapiti](http://wapiti.limsi.fr/manual.html).

Ejemplo de archivo de patrón:

```
# Unigram
U00:%x[-2,0]
U01:%x[-1,0]
U02:%x[0,0]
U03:%x[1,0]
U04:%x[2,0]
U05:%x[-2,1]
U06:%x[-1,1]
U07:%x[0,1]
U08:%x[1,1]
U09:%x[2,1]
U10:%x[-2,0]/%x[0,0]
U11:%x[-1,0]/%x[0,0]
U12:%x[0,0]/%x[1,0]
U13:%x[0,0]/%x[2,0]
U14:%x[-2,1]/%x[0,1]
U15:%x[-1,1]/%x[0,1]
U16:%x[0,1]/%x[1,1]
U17:%x[0,1]/%x[2,1]
# Bigram
B
```

## Programa

Aquí hay un pequeño programa que utiliza la biblioteca para entrenar un modelo y etiquetar entidades.

```python
use('wapiti')

wapiti tst

# Cargar el modelo
tst.loadmodel("modelo")

# Etiquetar un vector de tokens
words = ['Growth NOUN', '& CONJ', 'Income NOUN', 'Fund NOUN', '( PUNCT', 'Exact ADJ', 'name NOUN', 'of PREP', 'registrant NOUN', 'as PREP', 'specified ADJ', 'in PREP', 'charter NOUN']
res = tst.label(words)

# Imprimir la lista de etiquetas
println(res)
```

El resultado es: `['ORGANISATION_b', 'ORGANISATION_i', 'ORGANISATION_i', 'ORGANISATION_i', 'null', 'null', 'null', 'null', 'null', 'null', 'null', 'null', 'null']`