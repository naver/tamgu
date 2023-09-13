## 10.2 - 초기 함수

프레임에는 생성자 함수를 연결할 수 있습니다. 이 함수는 해당 프레임의 새로운 인스턴스가 생성될 때 자동으로 호출됩니다.

### 예시

```cpp
frame myframe {
    int i = 10; // 모든 새로운 프레임은 i를 10으로 초기화합니다.
    string s = "initial";
    
    function _initial(int ij) {
        i = ij;
    }
    
    function display() {
        print("IN MYFRAME: " + s + "\n");
    }
}

// myframe의 새로운 인스턴스를 생성합니다:
myframe second(10); // 매개변수는 _initial 함수로 전달되며, C++과 유사합니다.
```

이 예시에서는 `_initial`이라는 초기 함수를 가진 `myframe`이라는 프레임이 있습니다. `myframe`의 새로운 인스턴스가 생성되면, 제공된 매개변수와 함께 `_initial` 함수가 호출됩니다. `display` 함수는 프레임 내의 `s`의 값을 출력합니다.