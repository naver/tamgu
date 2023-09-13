## 프레임의 확장 또는 프레임의 풍부화

Tamgu는 프로그래머가 기존의 프레임을 풍부화하거나 하위 프레임을 만들 수 있게 합니다. 프레임 설명은 몇 가지 단계로 구현할 수 있습니다. 예를 들어, 초기 설명으로 시작하고 나중에 프로그램에서 풍부화할 수 있습니다.

### 풍부화

```cpp
// 우리는 프레임의 제한된 정의로 시작합니다...
frame myframe {
    int i = 10; // 모든 새로운 프레임은 i를 10으로 인스턴스화합니다.
}

// 여기에 일부 코드를 추가합니다...
...

// 그런 다음 이 프레임을 몇 가지 새로운 코드로 풍부화합니다.
// 위와 동일한 프레임 명령을 사용하면 새로운 내용을 추가할 수 있습니다.
frame myframe {
    function display() {
        println(i);
    }
}
```

### 하위 프레임

하위 프레임은 다른 프레임(상위 프레임) 내에서 선언된 프레임으로, 변수와 함수를 상속받습니다. 하위 프레임은 상위 프레임의 함수를 새로운 버전으로 대체하고 자체 변수를 추가할 수 있습니다.

```cpp
// 하위 프레임을 추가하려면...
frame myframe {
    // 이제 하위 프레임을 추가할 수 있습니다...
    frame subframe { ... }
}
```

### 상위 정의 사용: frame::function

현재 스레드가 아닌 부모 프레임의 정의를 사용해야 하는 경우, Tamgu는 C++ 또는 Java와 유사한 메커니즘을 제공합니다. 함수 이름 앞에 프레임 이름을 두고 "::"를 뒤에 붙여야 합니다.

**예시:**

```cpp
// 하위 프레임 호출...
// 하위 테스트 프레임을 정의한 테스트 프레임을 정의합니다.
frame test {
    int i;

    function _initial(int k) {
        i = k;
    }

    function display() {
        println("In test", i);
    }

    frame subtest {
        string x;

        function display() {
            println("In subtest", i);
            test::display(); // test의 다른 display 정의를 호출합니다.
        }
    }
}

// 두 개의 객체를 생성합니다.
test t(1);
subtest st(2);

// 다른 메소드를 호출합니다.
t.display(); // display: "In test, 1"
st.display(); // display: "In subtest, 2" and "In test, 2"
st.test::display(); // display: "In test, 2"
```