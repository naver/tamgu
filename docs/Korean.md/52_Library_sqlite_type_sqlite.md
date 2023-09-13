# 탐구 SQLite 라이브러리

탐구는 SQLite 데이터베이스를 처리하기 위한 간단한 라이브러리를 제공합니다. SQLite는 SQL 명령을 실행하기 위해 간단한 파일을 사용하는 인기있는 데이터베이스 시스템입니다. SQLite에 대한 자세한 정보가 필요한 경우 웹에서 많은 자료를 찾을 수 있습니다.

탐구에서 SQLite 라이브러리를 사용하려면 다음 명령을 사용하여 가져와야 합니다:

```python
use("sqlite")
```

## 메소드

SQLite 라이브러리는 다음과 같은 메소드를 제공합니다:

1. `begin()`: DEFERRED 모드로 커밋 모드로 진입합니다.
2. `begin(string mode)`: 지정된 모드(DEFERRED, EXCLUSIVE, IMMEDIATE)로 커밋 모드로 진입합니다.
3. `close()`: 데이터베이스를 닫습니다.
4. `commit()`: SQL 명령을 처리합니다. 이는 `begin()`으로 시작된 일련의 명령을 완료하는 데 사용되어야 합니다.
5. `create(x1, x2, x3)`: 지정된 인수로 데이터베이스에 테이블을 생성합니다.
6. `execute(string sqlcommand)`: 콜백 없이 SQL 명령을 실행합니다.
7. `insert(table, column, value, ...)`: 테이블에 행을 삽입합니다.
8. `open(string pathname)`: 데이터베이스를 엽니다.
9. `run(string sqlcommand)`: 콜백을 사용하여 SQL 명령을 실행하여 결과를 저장합니다. 입력 변수가 벡터인 경우 모든 가능한 값이 해당 벡터에 저장됩니다. 입력 변수가 반복자인 경우 SQL 명령의 결과를 반복할 수 있습니다. 각 결과는 각 키가 열 이름인 맵입니다.

## 예제

다음은 SQLite 라이브러리를 사용하는 예제입니다:

```python
// SQLite 라이브러리 가져오기
use("sqlite")

// 새로운 SQLite 변수 선언
sqlite mydb;

// 데이터베이스 열기. 존재하지 않으면 생성됩니다.
mydb.open('test.db');

try {
    // 현재 데이터베이스에 새로운 테이블 생성
    mydb.create("table1", "nom TEXT PRIMARY KEY", "age INTEGER");
    println("table1이 생성되었습니다");
} catch () {
    // 데이터베이스가 이미 존재합니다.
    println("이미 생성되었습니다");
}

int i;
string nm;

// 커밋 모드를 사용하여 데이터베이스에 값 삽입 (더 빠름)
mydb.begin();

// 5000개의 요소 삽입
for (i = 0; i < 5000; i += 1) {
    nm = "tiia_" + i;
    try {
        // table1에 두 개의 값 삽입, 'nom'과 'age'에 대한 값 간 교대 주목
        mydb.insert("table1", "nom", nm, "age", i);
        println(i);
    } catch () {
        println("이미 삽입되었습니다");
    }
}

// 명령 커밋
mydb.commit();

// 주어진 SQL 명령에 대한 값 반복
iterator it = mydb.run("select * from table1 where age > 10;");
for (it.begin(); it.end() == false; it.next()) {
    println("값: ", it.value());
}

// 대안으로 다음과 같은 결과를 얻을 수 있습니다:
// vector v = mydb.execute("select * from table1 where age > 10;");
// 그러나 벡터가 오버플로될 위험이 있으므로 권장되지 않습니다.

// 데이터베이스 닫기
mydb.close();
```