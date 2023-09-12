## Autorun

`autorun` 함수는 선언된 후 자동으로 실행됩니다. Autorun 함수는 메인 파일에서만 실행됩니다. 다른 파일 내에서 호출되는 파일에 autorun 함수가 있는 경우에는 실행되지 않습니다.

**참고:** Autorun 함수는 프레임에서는 사용할 수 없습니다.

### 예시

```python
autorun waitOnRed() {
  println("로딩 중:", _paths[1]);
}
```