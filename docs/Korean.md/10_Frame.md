# 10 프레임

`Frame` 클래스는 복잡한 데이터 구조와 함께 함수를 선언하기 위해 사용됩니다. 다음과 같은 기능을 제공합니다:

- 멤버 변수는 프레임 내에서 인스턴스화 될 수 있습니다.
- `_initial` 메소드를 선언할 수 있으며, 이 메소드는 프레임의 각 새 인스턴스에 자동으로 호출됩니다.
- 프레임 내부에 하위 프레임을 선언할 수 있으며, 이 하위 프레임은 상위 프레임에서 메소드와 변수를 자동으로 상속받습니다.
- 하위 프레임 내에서 함수의 재정의가 가능합니다.
- 프레임 내부에 비공개 함수와 변수를 선언할 수도 있습니다.

## 예제

```cpp
frame myframe {
    int i = 10; // 모든 새 프레임은 i를 10으로 인스턴스화합니다.
    string s = "initial";
    
    function display() {
        print("IN MYFRAME: " + s + "\n");
    }
    
    frame mysubframe {
        function display() {
            print("IN MYSUBFRAME: " + s + "\n");
        }
    }
}
```

## 10.1 프레임 사용하기

프레임 객체는 프레임의 이름을 타입으로 지정하여 선언됩니다.

예제:

```cpp
myframe first; // 첫 번째 인스턴스를 생성합니다.
mysubframe subfirst; // 하위 프레임 인스턴스를 생성합니다.

// 새 인스턴스를 다시 생성할 수 있습니다.
first = myframe; // C++ 또는 Java의 "new myframe"과 동일합니다.

// 프레임의 함수를 실행하기 위해
myframe.display();
```

## 10.2 `_initial` 함수

프레임과 연결된 생성자 함수를 선언할 수 있습니다. 이 함수는 해당 프레임의 새 인스턴스가 생성될 때 자동으로 호출됩니다.

예제:

```cpp
frame myframe {
    int i = 10; // 모든 새 프레임은 i를 10으로 인스턴스화합니다.
    string s = "initial";
    
    function _initial(int ij) {
        i = ij;
    }
    
    function display() {
        print("IN MYFRAME: " + s + "\n");
    }
}

// myframe의 새 인스턴스를 생성합니다.
myframe second(10); // 매개 변수는 그대로 _initial 함수로 전달됩니다. (C++의 경우와 동일)
```

## 10.3 `_final` 함수

`_final` 함수는 프레임 객체가 삭제될 때마다 호출됩니다. 일반적으로 함수나 루프 내에서 선언된 객체는 해당 함수나 루프가 종료될 때 삭제됩니다.

중요:
- 이 함수는 매개 변수가 없습니다.
- 이 함수의 호출로 인해 객체가 삭제되지는 않습니다.
- 이 함수의 내용은 소멸자 내부에서 호출되므로 코드의 나머지 부분과는 독립적으로 디버깅할 수 없습니다.

예제:

```cpp
frame myframe {
    int i = 10; // 모든 새 프레임은 i를 10으로 인스턴스화합니다.
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
    // myframe의 새 인스턴스를 생성합니다.
    // 각 반복의 끝에서 _final 함수가 호출됩니다.
    myframe item(i);
    i--;
}
```

## 10.4 초기화 순서

프레임 내에서 항목을 선언할 때, `_initial` 함수의 호출은 상위 프레임에서 하위 프레임으로 내려가면서 수행됩니다. 또한, 프레임 F 내의 항목이 프레임 F의 `_initial` 함수 내에서 인스턴스화된 경우, 이 선언은 다른 선언보다 우선합니다.

예제:

```cpp
// 두 개의 프레임을 선언합니다.
frame within {
    int i;
    
    // 특정 생성자 함수와 함께
    function _initial(int j) {
        i = j * 2;
        println("within _initial", i);
    }
}

// 이 프레임은 특정 "within" 프레임을 선언합니다.
frame test {
    int i;
    
    // 이 경우, 변수 i에 의존하는 특정 프레임을 선언합니다.
    within w(i);
    
    // 해당 프레임에 대한 _initial 함수...
    function _initial(int k) {
        i = k;
        println("test _initial", k);
    }
}

// 초기값이 20인 test 인스턴스 t1을 생성합니다.
test t1(20);
```

## 10.5 생성자 내에서 생성하기

프레임 요소를 생성하는 데에는 생성자의 필드 목록에 직접 초기화하거나 생성자 내에서 생성할 수 있습니다. 프레임 요소의 생성이 생성자에서 이루어지는 경우, 간단한 선언만으로 충분하며 다른 선언은 중복적입니다.

예제:

```cpp
// 이 프레임은 특정 "within" 프레임을 선언합니다.
frame test {
    int i;
    
    // 이 경우, 요소의 실제 생성을 생성자인 _initial로 연기합니다.
    within w;
    
    // 해당 프레임에 대한 _initial 함수...
    function _initial(int k) {
        i = k;
        // 이전 설명을 새로운 것으로 대체합니다.
        // 이 선언은 위의 다른 선언을 대체합니다.
        w = within(100);
        println("test _initial", k);
    }
}

// 초기값이 20인 test 인스턴스 t1을 생성합니다.
test t1(20);
```

중요:
"w"에 대한 생성자 매개 변수가 필요하고 생성자에서 해당 요소 "w"의 생성이 이루어지지 않은 경우, Tamgu는 매개 변수가 누락되었다는 오류를 표시합니다.

## 10.6 공통 변수

Tamgu는 클래스 변수를 선언하는 간단한 방법을 제공합니다. 클래스 변수는 주어진 프레임의 모든 인스턴스에서 공유되는 값입니다.

예제:

```cpp
frame myframe {
    common int i; // 모든 프레임은 이 변수의 동일한 공통 인스턴스에 액세스할 수 있습니다.
}

myframe t1;
myframe t2;
t1.i = 10;
t2.i = 15;
println(t1.i, t2.i); // 두 변수에 대한 출력: 15 15
```

## 10.7 비공개 함수와 멤버

프레임 내에서 특정 함수나 변수를 비공개로 선언할 수 있습니다. 비공개 함수나 비공개 변수는 프레임 내부에서만 액세스할 수 있습니다.

예제:

```cpp
frame myframe {
    int i = 10; // 모든 새 프레임은 i를 10으로 인스턴스화합니다.
    
    // 비공개 변수
    private string s = "initial";
    
    function _initial(int ij) {
        i = ij;
    }
    
    // 비공개 함수
    private function modify(int x) {
        i = x;
        s = "Modified with: " + x; // 여기서 "s"를 수정할 수 있습니다.
    }
    
    function display() {
        modify(1000); // 여기서 "modify"를 호출할 수 있습니다.
        print("IN MYFRAME: " + s + "\n");
    }
}

myframe test;

// 비공개 프레임 멤버에 대한 잘못된 명령...
test.modify(100); // "modify"는 비공개이기 때문에 이 명령은 잘못된 것입니다.
println(test.s); // "s"는 비공개이기 때문에 이 명령은 잘못된 것입니다.
```

## 10.8 프레임의 하위 프레임 또는 프레임 확장

Tamgu는 프로그래머가 기존 프레임을 확장하거나 하위 프레임을 선언할 수 있도록 합니다. 프레임 설명은 몇 단계로 구현할 수 있습니다. 예를 들어, 처음에는 첫 번째 설명으로 시작한 다음 프로그램에서 나중에 확장할 수 있습니다.

### 확장

```cpp
// 우리는 프레임의 제한된 정의로 시작합니다...
frame myframe {
    int i = 10; // 모든 새 프레임은 i를 10으로 인스턴스화합니다.
}

// 여기에 코드를 추가합니다...

// 그런 다음 이 프레임을 새로운 내용과 함께 확장합니다.
// 위와 동일한 프레임 명령을 사용하면 새로운 내용을 추가할 수 있습니다.
frame myframe {
    function display() {
        println(i);
    }
}
```

### 하위 프레임

하위 프레임은 다른 프레임(상위 프레임) 내에서 선언된 프레임으로, 상위 프레임에서 변수와 함수를 상속받습니다. 하위 프레임은 상위 프레임의 함수를 새로운 버전으로 대체하고 자체 변수를 추가할 수 있습니다.

```cpp
// 하위 프레임을 추가하려면...
frame myframe {
    // 이제 하위 프레임을 추가할 수 있습니다...
    frame subframe { ... }
}
```

### 상위 정의 사용: frame::function

현재 스레드 대신 상위 프레임의 정의를 사용해야 하는 경우, Tamgu는 C++ 또는 Java와 유사한 메커니즘을 제공합니다. 함수 이름은 프레임 이름과 "::"로 시작해야 합니다.

예제:

```cpp
// 하위 프레임 호출...
// test 프레임을 정의하고, subtest 프레임을 정의합니다.
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

// 두 개의 객체를 만듭니다.
test t(1);
subtest st(2);

// 다른 메소드를 호출합니다.
t.display(); // display: "In test, 1"
st.display(); // display: "In subtest, 2" and "In test, 2"
st.test::display(); // display: "In test, 2"
```

## 10.9 시스템 함수

시스템 함수는 Tamgu 언어의 다양한 연산자를 프레임 메소드에 매핑하는 데 사용됩니다.

### 비교 함수

Tamgu는 프레임 요소 간의 특정 비교 함수를 정의하는 데 도움이 되는 방법을 제공합니다. 이러한 함수는 이름은 특정하지만 다음 연산자에 의해 트리거됩니다: `>`, `<`, `==`, `!=`, `<=`, `>=`.

각 함수는 현재 요소와 하나의 매개 변수를 가지며, 이 매개 변수와 현재 요소를 비교합니다.

다음은 이러한 함수의 목록입니다:

1. `equal`: function ==(frame b);
2. `different`: function !=(frame b);
3. `inferior`: function <(frame b);
4. `superior`: function >(frame b);
5. `inferior equal`: function <=(frame b);
6. `superior equal`: function >=(frame b);

각 함수는 해당 테스트에 따라 true 또는 false를 반환해야 합니다.

예제:

```cpp
// 프레임에서 비교 연산자 구현
frame comp {
    int k;
    
    // inferior 연산자를 구현합니다.
    function <(autre b) {
        if (k < b)
            return true;
        return false;
    }
}

// 두 개의 요소를 생성합니다.
comp one;
comp two;

// one은 10이고 two는 20입니다.
one.k = 10;
two.k = 20;

// one은 two보다 작으며 위의 inferior 메소드가 호출됩니다.
if (one < two)
    println("OK");
```

### 산술 함수

Tamgu는 다양한 수학 연산자에 대한 특정 함수를 구현하는 메커니즘을 제공합니다. 이 함수는 두 개의 요소를 가져야 하지만 `++`와 `--`를 제외한 모든 함수는 두 요소와 같은 프레임의 요소를 반환해야 합니다.

1. `plus`: function +(frame a, frame b);
2. `minus`: function -(frame a, frame b);
3. `multiply`: function *(frame a, frame b);
4. `divide`: function /(frame a, frame b);
5. `power`: function ^^(frame a, frame b);
6. `shift left`: function <<(frame a, frame b);
7. `shift right`: function >>(frame a, frame b);
8. `mod`: function %(frame a, frame b);
9. `or`: function |(frame a, frame b);
10. `xor`: function ^(frame a, frame b);
11. `and`: function &(frame a, frame b);
12. `++`: function ++();
13. `--`: function --();

예제:

```cpp
frame test {
    int k;
    
    function ++() {
        k++;
    }
    
    // 중요한 것은 함수가 새 값을 반환해야 한다는 점입니다.
    function +(test a, test b) {
        test res;
        res.k = a.k + b.k;
        return res;
    }
}

test a, b, c;
c = a + b; // 위의 plus 구현이 호출됩니다.
```

### 숫자 및 문자열 함수

프레임 객체를 산술 식에서 문자열이나 숫자로 해석할 수도 있습니다. 이 경우, 개발자는 프레임 인스턴스가 문자열 또는 숫자로 해석되기를 원하는 객체의 유형 이름과 동일한 이름의 함수를 구현해야 합니다.

예제:

```cpp
frame test {
    int k;
    string s;
    
    // "test" 인스턴스를 문자열로 반환합니다...
    function string() {
        return s;
    }
    
    // "test" 인스턴스를 float로 반환합니다...
    function float() {
        return k;
    }
    
    // "test" 인스턴스를 int로 반환합니다...
    function int() {
        return k;
    }
}
```

### 구간 및 인덱스

프레임 객체를 벡터나 맵으로 사용할 수도 있습니다. 이 경우, 구간을 통해 요소에 액세스하거나 인덱스를 통해 값을 설정할 수 있습니다. 객체를 이러한 방식으로 사용하려면 개발자는 다음 함수를 구현해야 합니다:

1. function [](self idx, self value): 이 함수는 벡터의 idx 위치에 요소를 삽입합니다.
2. function [](self idx): 이 함수는 idx 위치의 값을 반환합니다.
3. function [:](self left, self right): 이 함수는 left와 right 사이의 값을 반환합니다.

예제:

```cpp
frame myvect {
    vector kj;
    
    // 이 함수는 벡터의 idx 위치에 값을 삽입합니다.
    function [](int idx, self value) {
        kj[idx] = value;
    }
    
    // 이 함수는 idx 위치의 값을 반환합니다.
    function [](int idx) {
        return kj[idx];
    }
    
    // 이 함수는 l과 r 사이의 값을 반환합니다.
    function [:](int l, int r) {
        return kj[l:r];
    }
}

myvect test;
test[0] = 10; // 함수 [](...)를 호출합니다.
test[1] = 5; // 함수 [](...)를 호출합니다.
// 함수 [:](...)를 호출합니다.
println(test[0], test[1], test[0:]); // 출력: 10 5 [10, 5]
```