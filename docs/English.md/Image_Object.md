# Image Object

The `Image` object is used to load an image from a GIF or JPEG file, which can then be used with a `Window` object or a `Button` object through the `image` method.

## Methods

1. `loadJPEG(string filename)`: Load a JPEG image.
2. `loadGIF(string filename)`: Load a GIF image.

## Usage

Once an `Image` object has been declared, you can load your file and use this object in different image methods, mainly with `Button` and `Window` objects.

## Example

```python
use('gui')

image im
im.loadJPEG(_current + "witch.jpg")

window w
w.begin(30, 30, 1000, 1000, "Image")
w.image(im, 100, 100, 500, 500)
w.end()
w.run()
```