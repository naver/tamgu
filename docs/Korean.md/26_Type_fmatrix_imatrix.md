# fmatrix과 imatrix

이러한 유형은 행렬을 처리하는 데 사용됩니다. 행렬의 크기를 생성 시점에 정의하고, ":" 연산자를 재정의하여 요소를 저장할 수 있습니다. 이 연산자는 저장할 값의 행과 열을 정의하는 데 사용됩니다. 행렬은 오직 실수(float)만 저장할 수 있습니다.

행 r과 열 c에 원소 v를 저장하려면 `m[r:c]=v` 구문을 사용합니다.

특정 행 또는 열을 fvector로 검색하려면 다음 구문을 사용하세요:
- `m[r:]`는 행 r을 fvector로 반환합니다.
- `m[:c]`는 열 c를 fvector로 반환합니다.

## 메서드
1. `determinant()`: 행렬의 행렬식을 반환합니다.
2. `dimension()`: 행렬의 크기를 반환합니다.
3. `dimension(int rowsize, int columnsize)`: 행렬의 크기를 설정합니다.
4. `invert()`: 역행렬을 반환합니다.
5. `product()`: 모든 요소의 곱을 반환합니다.
6. `sum()`: 모든 요소의 합을 반환합니다.
7. `transpose()`: 전치 행렬을 반환합니다.

## 연산자
+, *, /, - 연산자가 모두 사용 가능합니다. 그러나 두 행렬의 곱셈은 행렬 곱셈에 따라 한 행렬을 다른 행렬과 곱합니다. 나눗셈에도 동일한 원리가 적용됩니다.

## 예제
```cpp
// 행과 열의 수를 정의합니다.
fmatrix m(3,3);
fmatrix v(3,1);

// 원소를 저장합니다.
v[0:0] = 3;
v[1:0] = 0;
v[2:0] = 0;

float angle = 56;

// 주어진 각도로 행렬을 로딩하는 함수
function loading(fmatrix mx, float θ) {
    θ = θ.radian();
    mx[0:0] = cos(θ);
    mx[0:1] = 0;
    mx[0:2] = sin(θ);
    mx[1:0] = 0;
    mx[1:1] = 1;
    mx[1:2] = 0;
    mx[2:0] = -sin(θ);
    mx[2:1] = 0;
    mx[2:2] = cos(θ);
}

loading(m, angle);

fmatrix vx;

// 행렬 곱셈
vx = m * v;

m[0:0] = -2;
m[0:1] = 2;
m[0:2] = -3;
m[1:0] = -1;
m[1:1] = 1;
m[1:2] = 3;
m[2:0] = 2;
m[2:1] = 0;
m[2:2] = -1;

// 행렬식 계산
int det = m.determinant();
println(det);

m[0:0] = 1;
m[0:1] = 2;
m[0:2] = -1;
m[1:0] = -2;
m[1:1] = 1;
m[1:2] = 1;
m[2:0] = 0;
m[2:1] = 3;
m[2:2] = -3;

fmatrix inv;

// 행렬 역전
inv = m.invert();
```