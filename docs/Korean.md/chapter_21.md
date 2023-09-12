# Tamgu에서 특화된 맵

## 23.1 (Tree|Prime|Bin)Map[s|i|f|u|l]

이러한 타입들은 "Map"과 "TreeMap"과 매우 유사하지만 한 가지 예외가 있습니다: 키로 정수 (MapI, TreeMapI, PrimeMapI), 실수 (MapF, TreeMapF, PrimeMapF) 또는 유니코드 문자열 (MapU, TreeMapU, PrimeMapU)을 사용합니다. 반면, "Map", "TreeMap" 및 "PrimeMap"은 문자열을 사용합니다.

실제로 일관성을 유지하기 위해 "Map", "TreeMap" 및 "PrimeMap"은 "Maps", "BinMaps", "TreeMaps", "PrimeMaps"로도 불릴 수 있습니다.

**중요:** 

이러한 맵은 "allmaps" 라이브러리를 통해서만 액세스할 수 있습니다. 이러한 확장된 맵 중 하나를 사용하려면, 먼저 "use" 명령을 사용하여 "allmaps"를 로드해야 합니다.

```javascript
use("allmaps");
```

예시 사용법:

```javascript
TreeMapLL t;
```

하지만 일부 맵은 이미 주요 인터프리터의 일부이므로 "allmaps" 라이브러리를 먼저 로드할 필요가 없습니다. 다음은 전체 목록입니다:

1) Map에 대한 모든 변형.
2) TreeMaps[S|F|I|L], TreeMapI, TreeMapIS, TreeMapF.
3) PrimeMaps[S|F|I|L].

나머지 맵은 "allmaps"를 먼저 로드해야 합니다.

## 23.2 특화된 값 맵

이 특정 맵들은 키로 문자열, 정수 또는 실수를 사용하며, 값으로도 문자열, 정수 또는 실수를 사용합니다. 이 경우의 네이밍 규칙은 다음과 같습니다:

(Tree|Prime)Map[S|I|F|U][S|I|F|U].

예를 들어, TreeMapIF는 키가 정수이고 값이 실수인 TreeMap입니다.