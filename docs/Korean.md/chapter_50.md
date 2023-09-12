# Tamgu SQLite 라이브러리

Tamgu는 SQLite 데이터베이스를 처리하기 위한 간단한 라이브러리를 제공합니다. SQLite는 SQL 명령을 실행하기 위해 간단한 파일을 사용하는 인기있는 데이터베이스 시스템입니다. SQLite에 대한 자세한 정보는 웹에서 다양한 자료를 찾을 수 있습니다.

Tamgu에서 SQLite 라이브러리를 사용하려면 다음 명령을 사용하여 가져와야 합니다:

```python
use("sqlite")
```

## 메소드

SQLite 라이브러리는 다음과 같은 메소드를 제공합니다:

1. `begin()`: DEFERRED 모드로 커밋 모드로 진입합니다.
2. `begin(string mode)`: 지정된 모드(DEFERRED, EXCLUSIVE, 또는 IMMEDIATE)로 커밋 모드로 진입합니다.
3. `close()`: 데이터베이스를 닫습니다.
4. `commit()`: SQL 명령을 처리합니다. 이는 `begin()`으로 시작된 일련의 명령 후에 호출되어야 합니다.
5. `create(x1,x2,x3)`: 지정된 인자로 데이터베이스에 테이블을 생성합니다.
6. `execute(string sqlcommand)`: 콜백 없이 SQL 명령을 실행합니다.
7. `insert(table,column,value,...)`: 테이블에 행을 삽입합니다.
8. `open(string pathname)`: 데이터베이스를 엽니다.
9. `run(string sqlcommand)`: 콜백을 사용하여 SQL 명령을 실행하고 결과를 저장합니다. 입력 변수가 벡터인 경우, 모든 가능한 값이 그 안에 저장됩니다. 입력 변수가 이터레이터인 경우, SQL 명령의 결과를 반복할 수 있습니다. 각 결과는 각 키가 열 이름인 맵입니다.

## 예제

다음은 SQLite 라이브러리를 사용하는 예제입니다:

```python
use("sqlite")

sqlite mydb

# 데이터베이스를 엽니다. 존재하지 않으면 생성됩니다.
mydb.open('test.db')

# 데이터베이스에서 작업을 수행합니다...
```

이는 Tamgu의 SQLite 라이브러리에 대한 간단한 개요에 불과합니다. 더 자세한 정보는 공식 문서를 참조해 주세요.