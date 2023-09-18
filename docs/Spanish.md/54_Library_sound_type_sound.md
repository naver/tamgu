# Biblioteca Sound

La Biblioteca Sound en Tamgu proporciona una forma de reproducir varios tipos de archivos de sonido como WAV, MP3, FLAC, OGG, etc. Puedes cargar fácilmente un archivo y reproducirlo en cualquier parte de tu código utilizando el tipo "sound".

**Nota:** Tamgu depende de las siguientes bibliotecas para decodificar y reproducir: libao4, libsndfile-1 y libmpg123.

Para usar esta biblioteca, puedes llamarla usando `use("sound")`.

## Métodos

La API expone los siguientes métodos:

1. `close()`: Cierra un canal de sonido.
2. `decode(ivector soundbuffer)`: Decodifica el archivo de sonido y devuelve el contenido en búfer en `soundbuffer`. Devuelve falso cuando se alcanza el final del archivo.
3. `encode(ivector soundbuffer)`: Reproduce un búfer de sonido devuelto por `decode`.
4. `load(string pathname)`: Carga el sonido desde la ruta de acceso especificada.
5. `open(map params)`: Abre un canal de sonido con los parámetros del archivo de sonido actual (ver parámetros).
6. `parameters()`: Devuelve los parámetros del archivo de sonido actual como un mapa.
7. `parameters(map modifs)`: Solo se pueden modificar "rate" y "channels".
8. `play()`: Reproduce el sonido.
9. `play(bool beg)`: Reproduce el sonido desde el principio.
10. `play(ivector soundbuffer)`: Reproduce el búfer de sonido (ver `encode`).
11. `reset()`: Restablece el archivo de sonido al principio.
12. `stop()`: Detiene el sonido. Es necesario reproducir el archivo de sonido en un hilo para poder usar esta instrucción.

## Ejemplo

```cpp
use("sound");
sound s;
s.load("C:\TAMGU\TAMGU7\sound\Kalimba.mp3");
s.play();
```

También puedes cargar un sonido con la siguiente declaración:

```cpp
sound s("C:\TAMGU\TAMGU7\sound\Kalimba.mp3");
```

## Ejemplo de Decodificación

```cpp
use("sound");

// Abrir un archivo de sonido
sound s("C:\TAMGU\TAMGU7\sound\Kalimba.mp3");

// Abrir un segundo canal de sonido
sound c;

// Obtener los parámetros de sonido
map params = s.parameters();

// Usar los parámetros de sonido para abrir un canal
c.open(params);

// Bucle con decodificación en el archivo de sonido
// y para cada nuevo búfer, reproducir el sonido
// Podríamos usar "play" en lugar de "encode",
// pero es un poco más lento
ivector snd;
while (s.decode(snd))
    c.encode(snd);

// Cerrar el canal
c.close();
```