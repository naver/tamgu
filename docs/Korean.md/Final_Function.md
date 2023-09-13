# _final 함수

`_final` 함수는 프레임 객체가 삭제될 때마다 호출됩니다. 일반적으로 함수나 루프에서 선언된 객체는 해당 함수나 루프가 종료되면 삭제됩니다.

## 중요사항

- 이 함수는 매개변수를 가지지 않습니다.
- 이 함수를 호출해도 객체는 삭제되지 않습니다.
- 이 함수의 내용은 소멸자 내에서 호출되므로 코드의 나머지 부분과 독립적으로 디버깅할 수 없습니다.

## 예시

```cpp
frame myframe {
    int i = 10; // 새로운 프레임이 생성될 때마다 i는 10으로 초기화됩니다.
    string s = "initial";

    function _initial(int ij) {
        i = ij;
    }

    function _final() {
        print("IN MYFRAME: " + s + "\n");
    }
}

int i = 10;
while (i >= 0) {
    // myframe의 새 인스턴스가 생성됩니다:
    // 각 반복의 끝에서 _final 함수가 호출됩니다.
    myframe item(i);
    i--;
}
```

참고: 이 텍스트는 GitHub README.md 파일로 변환되었습니다.