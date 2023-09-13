# pytamgu

`pytamgu` 라이브러리는 Python 라이브러리로 사용할 수도 있고 Tamgu 라이브러리로 사용할 수도 있는 이중 라이브러리입니다. 이 라이브러리를 사용하면 Tamgu 프로그램 내에서 Python 코드를 실행하거나 Python 코드 내에서 Tamgu 프로그램을 실행할 수 있습니다.

## Tamgu 라이브러리로 사용할 때

Tamgu 라이브러리로 사용할 때, `pytamgu`는 `python`이라는 새로운 타입을 노출시킵니다. 기본적인 Tamgu 타입 (boolean, integer, long, float, fraction, string, vector containers, map containers)은 자동으로 해당하는 Python 타입으로 매핑됩니다.

`python` 타입은 다음과 같은 메소드를 노출시킵니다:

1. `close()`: 현재의 Python 세션을 종료합니다.
2. `execute(string funcname, p1, p2...)`: `p1`, `p2` 등을 매개변수로 사용하여 Python 함수를 실행합니다.
3. `import(string python)`: Python 파일을 임포트합니다.
4. `run(string code)`: Python 코드를 실행합니다.
5. `setpath(string path1, string path2, etc...)`: 시스템 경로를 Python에 추가합니다.

`setpath` 메소드는 `import` 메소드를 사용하기 위해 필수적입니다. `import` 메소드는 Python의 `import` 키워드와 동일한 방식으로 작동합니다. `PYTHONPATH`를 통해 참조되지 않은 특정 위치에 있는 Python 프로그램을 임포트하려면 먼저 `setpath`로 추가해야 합니다.

### 예제

먼저, `testpy.py`라는 작은 Python 프로그램을 구현합니다:

```python
val = "here"
# 입력 변수들은 자동으로 Tamgu에서 Python 변수로 변환됩니다
def Iteste(s, v):
    v.append(s)
    v.append(val)
    return v
```

그런 다음, Python 프로그램과 동일한 디렉토리에 있는 것으로 가정하고 이 파일을 호출하는 Tamgu 프로그램을 구현합니다:

```tamgu
// 우리는 우리 자신을 위해 Pytamgu를 사용해야 합니다
use("pytamgu");

// Python 처리를 다루기 위한 변수가 필요합니다
python p;

// 우리의 Python 프로그램이 Tamgu 프로그램과 동일한 디렉토리에 있다고 가정합니다
p.setpath(_paths[1]);

// 그런 다음 우리의 프로그램을 임포트합니다
p.import("testpy");

vector v;
string s = "kkk";

// 우리는 Python 함수 Itest를 실행합니다. 이 함수는 문자열과 벡터를 입력으로 받으며,
// 이들은 실시간으로 Python 객체로 변환됩니다.
// 출력은 Python 벡터에서 Tamgu 벡터로 자동으로 변환됩니다
vector vv = p.execute("Itest", s, v);

println(vv); // 출력: ['kkk', 'here']

p.close(); // 세션을 종료합니다
```

## Python 라이브러리로 사용할 때

Python 라이브러리로 사용할 때, `pytamgu` 라이브러리를 임포트할 수 있으며, 두 개의 메소드가 노출됩니다.

1. `load(file, arguments, mapping)`
   - `file`은 로드할 Tamgu 파일의 파일명입니다.
   - `arguments`는 Tamgu 파일에 전달할 인수들을 공백으로 구분하여 제공하는 문자열입니다.
   - `mapping`이 1로 설정되면, Tamgu 파일의 각 함수에 대해 Python 함수와 동일한 이름의 Python 메소드가 생성됩니다.
   - 이 메소드는 핸들을 반환합니다.

2. `execute(handle, function_name, [arg1, arg2, ..., argn])`
   - `handle`은 함수를 실행하려는 파일의 핸들입니다.
   - `function_name`은 Tamgu 파일에서 함수의 이름입니다.
   - `[arg1, arg2, ..., argn]`은 문자열 벡터로 제공되는 Tamgu 프로그램에 전달될 인수 목록입니다.

매핑 옵션을 사용하면 `execute` 메소드는 선택 사항입니다. Tamgu 프로그램에서 반환되는 값은 자동으로 Python 객체로 변환되며, 동일하게 Tamgu 프로그램에 전달되는 인수에도 동일하게 적용됩니다.

**참고:** Tamgu는 항상 Python 유니코드 문자열을 반환합니다.

### 예제

Tamgu 프로그램:

```tamgu
vector v = [1..10];

function rappel(string s, int j) {
    j += 10;
    v.push(j);
    v.push(s);
    return v;
}
```

Python 프로그램:

```python
import pytamgu

h0 = pytamgu.load("rappel.kif", "", 1)
# Python 함수에 매핑을 사용합니다
v = rappel("Test", 10)
for i in v:
    print(i)
# 이는 다음과 동일합니다
v = pytamgu.execute(h0, "rappel", ["Test", 10])
```