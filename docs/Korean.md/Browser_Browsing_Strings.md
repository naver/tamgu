# 브라우저 (브라우징 문자열)

`Browser` 객체는 문자열을 표시하고 필요한 경우 목록으로 선택할 수 있는 상자를 정의합니다.

## 메소드

1. `add(label)`: 브라우저에 문자열을 추가합니다.
2. `clear()`: 브라우저에서 모든 값을 지웁니다.
3. `columnChar()`: 열 문자 구분자를 반환합니다.
4. `columnChar(string)`: 열 문자 구분자를 설정합니다.
5. `create(x, y, w, h, label)`: 브라우저를 생성합니다.
6. `deselect()`: 모든 항목의 선택을 해제합니다.
7. `deselect(int i)`: 항목 `i`의 선택을 해제합니다.
8. `formatChar()`: 형식 문자를 반환합니다.
9. `formatChar(string)`: 형식 문자를 설정합니다.
10. `insert(l, label)`: `l` 라인 앞에 레이블을 삽입합니다.
11. `load(filename)`: 파일을 브라우저에 로드합니다.
12. `select()`: 선택된 문자열을 반환합니다.
13. `select(int i)`: `i` 위치의 문자열을 반환합니다.
14. `size()`: 브라우저 내의 값의 수를 반환합니다.
15. `value()`: 현재 선택된 값을 인덱스로 반환합니다.

## 선택

브라우저를 선택 모드로 사용하는 유일한 방법은 다음과 같은 시그니처와 일치하는 콜백 함수와 연결하는 것입니다:

```python
function browser_callback(browser b, myobject o);
```

콜백 함수는 `with`로 선언됩니다.

## 예제

```python
use("gui");
// 콜백 함수
function avec(browser b, self n) {
    println("선택:", b.select(), b.value());
}

window w;
w.begin(40, 40, 400, 500, "브라우징");
browser b with avec;
b.create(10, 10, 100, 150, "테스트");
b.add("첫번째");
b.add("두번째");
b.add("세번째");
b.add("네번째");
w.end();
w.run();
```