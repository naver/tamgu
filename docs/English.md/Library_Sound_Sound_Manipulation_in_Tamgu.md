# Library Sound: Sound Manipulation in Tamgu

Tamgu provides a convenient way to work with various types of sound files (WAV, MP3, FLAC, OGG, etc.). By loading a file, you can easily play it anywhere in your code using the "sound" type.

Please note that Tamgu relies on the following libraries for decoding and playing: libao4, libsndfile-1, and libmpg123. Make sure these libraries are properly installed before using the "sound" library.

## Calling the Library

To use the "sound" library, simply call `use("sound")` in your code.

## Methods

The API exposes the following methods:

1. `close()`: Closes a sound channel.
2. `decode(ivector soundbuffer)`: Decodes the sound file and returns its content buffer by buffer into `soundbuffer`. Returns false when the end of the file is reached.
3. `encode(ivector soundbuffer)`: Plays a sound buffer returned by `decode`.
4. `load(string pathname)`: Loads the sound file at the specified `pathname`.
5. `open(map params)`: Opens a sound channel with the parameters of the current sound file (see parameters).
6. `parameters()`: Returns the parameters of the current sound file as a map.
7. `parameters(map modifs)`: Modifies the "rate" and "channels" parameters of the current sound file.
8. `play()`: Plays the sound.
9. `play(bool beg)`: Plays the sound from the beginning if `beg` is true.
10. `play(ivector soundbuffer)`: Plays the sound buffer (see `encode`).
11. `reset()`: Resets the sound file to the beginning.