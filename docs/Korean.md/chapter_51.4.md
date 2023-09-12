# 이미지 객체

`Image` 객체는 GIF 또는 JPEG 파일에서 이미지를 로드하는 데 사용되며, 이를 `Window` 객체 또는 `Button` 객체와 함께 `image` 메서드를 통해 사용할 수 있습니다.

## 메서드

1. `loadJPEG(string filename)`: JPEG 이미지를 로드합니다.
2. `loadGIF(string filename)`: GIF 이미지를 로드합니다.

## 사용법

`Image` 객체가 선언되면 파일을 로드하고 이 객체를 주로 `Button` 및 `Window` 객체와 함께 다른 이미지 메서드에서 사용할 수 있습니다.

## 예제

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