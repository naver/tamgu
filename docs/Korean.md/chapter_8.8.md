## 개인 함수 및 멤버

프레임 내에서 특정 함수나 변수를 개인으로 선언할 수 있습니다. 개인 함수나 변수는 프레임 내에서만 접근할 수 있습니다.

### 예시

```cpp
frame myframe {
    int i = 10; // 새로운 프레임이 생성될 때마다 i는 10으로 초기화됩니다.
    
    // 개인 변수
    private string s = "initial";
    
    function _initial(int ij) {
        i = ij;
    }
    
    // 개인 함수
    private function modify(int x) {
        i = x;
        s = "Modified with: " + x; // 여기에서 "s"를 수정할 수 있습니다.
    }
    
    function display() {
        modify(1000); // 여기에서 "modify"를 호출할 수 있습니다.
        print("IN MYFRAME: " + s + "\n");
    }
}

myframe test;

// 개인 프레임 멤버에 대한 잘못된 명령어...
test.modify(100); // "modify"가 개인으로 선언되어 있기 때문에 이 명령어는 잘못된 것입니다.
println(test.s); // "s"가 개인으로 선언되어 있기 때문에 이 명령어는 잘못된 것입니다.
```

위의 예시에서 `myframe` 프레임은 개인 변수 `s`와 개인 함수 `modify`로 정의되어 있습니다. 이러한 개인 멤버는 `myframe` 프레임 내에서만 접근할 수 있습니다.

`test` 객체와 같이 `myframe` 프레임 외부에서 개인 함수 `modify` 또는 개인 변수 `s`에 접근하려고 시도하면 잘못된 명령어로 처리됩니다.