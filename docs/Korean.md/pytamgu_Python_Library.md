# pytamgu Python 라이브러리

pytamgu 라이브러리는 Python 라이브러리로 사용되거나 Tamgu 라이브러리로 사용될 수 있는 이중 라이브러리입니다. 이 라이브러리를 사용하면 Tamgu 프로그램 내에서 Python 코드를 실행하거나 Python 코드 내에서 Tamgu 프로그램을 실행할 수 있습니다.

## 1. Tamgu 라이브러리

Tamgu 라이브러리로 사용될 때, "python"이라는 새로운 타입을 노출합니다. 기본 Tamgu 타입 (boolean, int, long, float, fraction, string, vector, map 컨테이너)은 자동으로 해당하는 Python 타입으로 매핑됩니다.

python 타입은 다음과 같은 메서드를 노출합니다:

1. `close()`: 현재 Python 세션을 닫습니다.
2. `execute(string funcname, p1, p2...)`: p1, p2를 매개변수로 사용하여 Python 함수를 실행합니다.
3. `import(string python)`: Python 파일을 가져옵니다.
4. `run(string code)`: Python 코드를 실행합니다.
5. `setpath(string path1, string path2, etc...)`: 시스템 경로를 Python에 추가합니다.

`setpath` 메서드는 `import` 메서드를 사용하기 위해 중요한 역할을 합니다. `import` 메서드는 Python의 `import` 키워드와 동일하게 작동합니다. PYTHONPATH를 통해 참조되지 않은 특정 위치에 있는 Python 프로그램을 가져오려면 먼저 `setpath`를 사용하여 해당 위치를 추가해야 합니다.

### 예제

먼저, `testpy.py`라는 작은 Python 프로그램을 구현합니다:

```python
val = "here"

# 입력 변수는 자동으로 Tamgu에서 Python 변수로 변환됩니다
def Iteste(s, v):
    v.append(s)
    v.append(val)
    return v
```

그런 다음, Tamgu 프로그램에서 이 파일을 호출하는 자체 Tamgu 프로그램을 구현합니다 (Tamgu 프로그램과 동일한 디렉토리에 있다고 가정합니다):

```python
// 우리 자신을 위해 Pytamgu를 사용해야 합니다
use("pytamgu")

// Python 처리를 다룰 변수가 필요합니다
python p

// 우리의 Python 프로그램이 Tamgu 프로그램과 동일한 디렉토리에 있다고 가정합니다
p.setpath(_paths[1])

// 그런 다음 프로그램을 가져옵니다
p.import("testpy")

vector v
string s = "kkk"

// 우리는 Python 함수 Itest를 실행합니다. 이 함수는 문자열과 벡터를 입력으로 받으며,
// 입력은 Python 객체로 변환됩니다.
// 출력은 자동으로 Tamgu 벡터로 다시 변환됩니다 (Python 벡터에서)
vector vv = p.execute("Itest", s, v)

println(vv) // 출력: ['kkk', 'here']

p.close() // 세션을 닫습니다
```

## 2. Python 라이브러리

Python 라이브러리로 사용할 수 있으며, `pytamgu` 라이브러리를 가져올 수 있습니다. 이 라이브러리는 두 가지 메서드를 노출합니다:

1. `load(file, arguments, mapping)`
   - `file`은 로드할 Tamgu 파일의 파일 이름입니다.
   - `arguments`는 Tamgu 파일에 전달할 인수를 공백으로 구분하여 제공하는 문자열입니다.