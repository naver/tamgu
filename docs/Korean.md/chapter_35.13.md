## GPSdistance

`GPSdistance` 함수는 경도와 위도 좌표로 제공된 두 개의 GPS 지점 사이의 거리를 계산합니다. 함수의 시그니처는 다음과 같습니다:

```cpp
float GPSdistance(float Longitude1, float Latitude1, float Longitude2, float Latitude2, float radius)
```

첫 번째 네 개의 매개변수인 `Longitude1`, `Latitude1`, `Longitude2`, `Latitude2`는 두 개의 GPS 지점의 경도와 위도 좌표를 나타냅니다. 다섯 번째 매개변수인 `radius`는 선택적이며 지구의 반지름을 나타냅니다. `radius`의 기본값은 6371 km입니다. 이 값은 최종 거리 값을 계산하는 데 사용되는 단위도 결정합니다.

### 예시

```cpp
float distance = GPSdistance(46.9001, 7.1201, 47.01, 6.1);
```

이 예시에서는 GPS 지점 (46.9001, 7.1201)과 (47.01, 6.1) 사이의 거리를 계산합니다. 계산된 거리는 78.382 km입니다.