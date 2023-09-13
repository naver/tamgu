# Library Sound

The Library Sound in Tamgu provides a way to play various types of sound files such as WAV, MP3, FLAC, OGG, etc. You can easily load a file and play it anywhere in your code using the "sound" type. 

**Note:** Tamgu relies on the following libraries for decoding and playing: libao4, libsndfile-1, and libmpg123.

To use this library, you can call it using `use("sound")`.

## Methods

The API exposes the following methods:

1. `close()`: Closes a sound channel.
2. `decode(ivector soundbuffer)`: Decodes the sound file and returns the content buffer by buffer into `soundbuffer`. Returns false when the end of the file is reached.
3. `encode(ivector soundbuffer)`: Plays a sound buffer returned by `decode`.
4. `load(string pathname)`: Loads the sound from the specified `pathname`.
5. `open(map params)`: Opens a sound channel with the parameters of the current sound file (see parameters).
6. `parameters()`: Returns the parameters of the current sound file as a map.
7. `parameters(map modifs)`: Only "rate" and "channels" can be modified.
8. `play()`: Plays the sound.
9. `play(bool beg)`: Plays the sound from the beginning.
10. `play(ivector soundbuffer)`: Plays the sound buffer (see `encode`).
11. `reset()`: Resets the sound file to the beginning.
12. `stop()`: Stops the sound. It is necessary to play the sound file in a thread in order to use this instruction.

## Example

```cpp
use("sound");
sound s;
s.load("C:\TAMGU\TAMGU7\sound\Kalimba.mp3");
s.play();
```

You can also load a sound with the following declaration:

```cpp
sound s("C:\TAMGU\TAMGU7\sound\Kalimba.mp3");
```

## Decoding Example

```cpp
use("sound");

// Open a sound file
sound s("C:\TAMGU\TAMGU7\sound\Kalimba.mp3");

// Open a second sound channel
sound c;

// Get the sound parameters
map params = s.parameters();

// Use the sound parameters to open a channel
c.open(params);

// Loop with decode in the sound file
// and for each new buffer, play the sound
// We could use "play" instead of "encode",
// but it is a little bit slower
ivector snd;
while (s.decode(snd))
    c.encode(snd);

// Close the channel
c.close();
```