# wchoice

Tamgu ltk는 목록에서 선택 항목을 제안하기 위한 특정 유형을 제공합니다. 이 요소는 나중에 설명 할 특정 메뉴로 초기화되어야 합니다. 다음과 같은 메서드를 노출합니다.

## 메서드

1. `create(int x, int y, int w, int h, string label)`: 선택 항목을 생성합니다.
2. `font(string s)`: 텍스트 글꼴을 설정하거나 반환합니다.
3. `fontsize(int c)`: 텍스트 글꼴 크기를 설정하거나 반환합니다.
4. `menu(vector s)`: 메뉴를 초기화합니다. 이는 wchoice 생성에서 마지막 작업이어야 합니다.
5. `value(int s)`: 선택 항목 초기값을 설정합니다.

## 메뉴

메뉴 설명은 세 개의 요소를 포함하는 벡터의 벡터입니다.

```cpp
vmenu = [["첫 번째", callback, "1"], ["두 번째", callback, "2"], ["세 번째", callback, "3"]];
```

메뉴 항목은 이름, 연관된 콜백 함수 및 이 콜백 함수에 전달될 객체를 포함합니다.

메뉴 항목: `[이름, 콜백, 객체]`

콜백 함수는 다음과 같은 서명을 가져야 합니다.

```cpp
function callback_menu(wchoice c, myobject obj);
```

이 함수는 목록에서 각 선택 항목에 대해 호출됩니다.

## 예시

```cpp
use("gui");
window w;

function callback_menu(wchoice c, string s) {
    println(s);
}

vector vmenu;

// 메뉴 설명
vmenu = [["첫 번째", callback_menu, "RRRR"], ["두 번째", callback_menu, "OOOOOO"], ["세 번째", callback_menu, "BBBBBBB"]];

wchoice wch;

// 창 생성
w.begin(300, 200, 1300, 500, "Fenetre");

// 선택 위젯 생성
wch.create(20, 420, 100, 50, "Choix");
wch.fontsize(20);

// 선택 목록의 마지막 작업이어야 합니다...
wch.menu(vmenu);

w.end();
w.run();
```