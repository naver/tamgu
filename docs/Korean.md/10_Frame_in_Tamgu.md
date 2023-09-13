# Tamgu에서의 10 프레임

프레임은 복잡한 데이터 구조와 함께 함수를 선언하기 위해 사용되는 클래스 설명입니다.

- 멤버 변수는 프레임 내에서 인스턴스화될 수 있습니다.
- `_initial` 메소드는 프레임의 각 새 인스턴스에 자동으로 호출될 수 있습니다.
- 프레임 내에서 하위 프레임을 선언할 수 있으며, 이는 자동으로 최상위 프레임에서 메소드와 변수를 상속받습니다.
- 하위 프레임 내에서 함수의 재정의가 가능합니다.
- 프레임 내에서 비공개 함수와 변수를 선언할 수도 있습니다.

**예시:**

```tamgu
frame myframe {
    int i = 10; // 각 새 프레임은 i를 10으로 인스턴스화합니다.
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