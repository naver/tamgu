# 38 랜덤 함수 Tamgu

이 저장소에는 Tamgu 프로그래밍 언어로 구현된 38개의 랜덤 함수가 포함되어 있습니다.

## Tamgu란 무엇인가요?

Tamgu는 C++의 강력함과 스크립팅 언어의 간단함을 결합한 오픈 소스 프로그래밍 언어입니다. 효율적이고 유연하며 사용하기 쉽도록 설계되었습니다. Tamgu는 문자열, 리스트, 맵 등 다양한 데이터 유형을 지원합니다.

## 포함된 함수

이 저장소에는 다음과 같은 Tamgu로 구현된 랜덤 함수가 포함되어 있습니다:

1. `random_boolean` - 랜덤한 부울 값 (참 또는 거짓)을 생성합니다.
2. `random_integer` - 지정된 범위 내에서 랜덤한 정수를 생성합니다.
3. `random_float` - 지정된 범위 내에서 랜덤한 부동 소수점 숫자를 생성합니다.
4. `random_string` - 지정된 길이의 랜덤한 문자열을 생성합니다.
5. `random_list` - 지정된 길이와 랜덤한 요소로 구성된 랜덤한 리스트를 생성합니다.
6. `random_map` - 지정된 키-값 쌍 수로 구성된 랜덤한 맵을 생성합니다.
7. `random_shuffle` - 주어진 리스트의 요소를 랜덤하게 섞습니다.
8. `random_choice` - 주어진 리스트에서 랜덤하게 요소를 선택합니다.
9. `random_sample` - 주어진 리스트에서 중복 없이 여러 요소를 랜덤하게 선택합니다.
10. `random_seed` - 랜덤 숫자 생성기의 시드를 설정합니다.
11. `random_gaussian` - 가우시안 분포에서 랜덤한 숫자를 생성합니다.
12. `random_poisson` - 포아송 분포에서 랜덤한 숫자를 생성합니다.
13. `random_uniform` - 균등 분포에서 랜덤한 숫자를 생성합니다.
14. `random_exponential` - 지수 분포에서 랜덤한 숫자를 생성합니다.
15. `random_gamma` - 감마 분포에서 랜덤한 숫자를 생성합니다.
16. `random_beta` - 베타 분포에서 랜덤한 숫자를 생성합니다.
17. `random_binomial` - 이항 분포에서 랜덤한 숫자를 생성합니다.
18. `random_negative_binomial` - 음이항 분포에서 랜덤한 숫자를 생성합니다.
19. `random_geometric` - 기하 분포에서 랜덤한 숫자를 생성합니다.
20. `random_hypergeometric` - 초기하 분포에서 랜덤한 숫자를 생성합니다.
21. `random_logarithmic` - 로그 분포에서 랜덤한 숫자를 생성합니다.
22. `random_triangular` - 삼각 분포에서 랜덤한 숫자를 생성합니다.
23. `random_vonmises` - 폰 미제스 분포에서 랜덤한 숫자를 생성합니다.
24. `random_zipf` - 지프 분포에서 랜덤한 숫자를 생성합니다.
25. `random_weibull` - 와이블 분포에서 랜덤한 숫자를 생성합니다.
26. `random_chisquare` - 카이 제곱 분포에서 랜덤한 숫자를 생성합니다.
27. `random_dirichlet` - 디리클레 분포에서 랜덤한 숫자를 생성합니다.
28. `random_exponential_power` - 지수 파워 분포에서 랜덤한 숫자를 생성합니다.
29. `random_fisher` - 피셔 분포에서 랜덤한 숫자를 생성합니다.
30. `random_gumbel` - 균벨 분포에서 랜덤한 숫자를 생성합니다.
31. `random_laplace` - 라플라스 분포에서 랜덤한 숫자를 생성합니다.
32. `random_logistic` - 로지스틱 분포에서 랜덤한 숫자를 생성합니다.
33. `random_lognormal` - 로그-정규 분포에서 랜덤한 숫자를 생성합니다.
34. `random_multinomial` - 다항 분포에서 랜덤한 숫자를 생성합니다.
35. `random_noncentral_chisquare` - 비중심 카이 제곱 분포에서 랜덤한 숫자를 생성합니다.
36. `random_noncentral_fisher` - 비중심 피셔 분포에서 랜덤한 숫자를 생성합니다.
37. `random_pareto` - 파레토 분포에서 랜덤한 숫자를 생성합니다.
38. `random_rayleigh` - 레일리 분포에서 랜덤한 숫자를 생성합니다.

## 사용법

Tamgu 프로그램에서 이러한 랜덤 함수를 사용하려면 저장소에서 원하는 함수를 가져오기만 하면 됩니다. 예를 들어, 랜덤한 정수를 생성하려면 `random_integer` 함수를 가져와 다음과 같이 코드에서 사용할 수 있습니다:

```tamgu
import "random_integer"

my_random_integer = random_integer(1, 10)
```

## 라이선스

이 저장소는 MIT 라이선스로 제공됩니다. 여기에서 제공되는 코드를 어떤 목적으로든 자유롭게 사용할 수 있습니다.

## 기여

이 저장소에 대한 기여를 환영합니다. Tamgu에서 구현된 추가적인 랜덤 함수가 있다면 언제든지 풀 리퀘스트를 제출해 주세요.

## 참고 자료

- [Tamgu 공식 웹사이트](https://www.tamgu.org/)
- [Tamgu GitHub 저장소](https://github.com/tamgu/tamgu)