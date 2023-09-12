# 메소드

이 라이브러리에서 사용 가능한 다음 메소드를 제공합니다. 자세한 내용은 Tamgu 정규 표현식 형식 또는 TREG에 대한 전용 장을 참조하십시오.

1. `base(int b, bool toconvert)`: 문자열의 숫자 내용에 해당하는 숫자 값(base b)을 반환합니다. `toconvert` 매개변수는 선택 사항입니다. false로 설정하면 변환할 숫자가 10진법으로 가정되고 base b로 변환됩니다.

2. `base(vector chrs)`: 주어진 base의 각 자릿수에 대한 인코딩을 설정합니다. 기본 설정은 64개의 문자입니다: 0-9, A-Z, a-z, #, @. 최대 표현은 64진법입니다. 이 기본 문자 집합을 사용자 지정 문자 집합으로 대체할 수 있습니다. 빈 벡터를 제공하면 시스템은 기본 문자 집합으로 재설정됩니다.

3. `bytes()`: 문자열과 일치하는 바이트의 벡터를 반환합니다.

4. `charposition(int pos)`: 바이트 위치를 문자 위치로 변환합니다. 특히 UTF8 문자열에서 유용합니다.

5. `deaccentuate()`: 강세 기호가 있는 문자에서 강세 기호를 제거합니다.

6. `doublemetaphone()`: 문자열의 더블 메타폰 인코딩을 포함하는 벡터를 반환합니다.

7. `dos()`: 문자열을 DOS 인코딩으로 변환합니다.

8. `dostoutf8()`: DOS 문자열을 UTF8 인코딩으로 변환합니다.

9. `emoji()`: 이모지의 텍스트 설명(영어)을 반환합니다.

10. `evaluate()`: 문자열 내에서 메타 문자를 평가하고 평가된 문자열을 반환합니다.

11. `extract(int pos, string from, string up1, string up2, ..., up1..upn)`: 현재 문자열에서 시작 위치 `pos`부터 `from`에서 `up1`, `up2`, ..., `up1..upn` 중 하나까지로 구성된 모든 부분 문자열을 포함하는 벡터를 반환합니다.

12. `fill(int nb, string char)`: `nb`개의 문자로 구성된 문자열을 생성합니다.

13. `find(string sub, int pos)`: 위치 `pos`에서 시작하는 부분 문자열 `sub`의 위치를 반환합니다.

14. `format(p1, p2, p3)`: 현재 문자열에서 `%x`마다 매개변수와 연결된 새 문자열을 생성합니다. 여기서 `x`는 인수 목록에서 해당 매개변수의 위치입니다. `x`는 1부터 시작합니다.

15. `geterr()`: 현재 오류 출력을 캐치합니다. `Printerr` 및 `printlnerr`은 이 문자열 변수에 저장됩니다.

16. `getstd()`: 현재 표준 출력을 캐치합니다. `Print` 및 `println`은 이 문자열 변수에 저장됩니다.

17. `html()`: 문자열을 HTML 호환 문자열 또는 문자열 벡터로 반환합니다.

18. `insert(i, s)`: 문자열 `s`를 위치 `i`에 삽입합니다. `i`가 -1인 경우 입력 문자열의 각 문자 사이에 `s`가 삽입됩니다.

19. `isalpha()`: 문자열이 알파벳 문자만 포함하는지 테스트합니다.

20. `isconsonant()`: 문자열이 자음만 포함하는지 테스트합니다.

21. `isdigit()`: 문자열이 숫자만 포함하는지 테스트합니다.

22. `isemoji()`: 문자열이 이모지만 포함하는지 테스트합니다.

23. `islower()`: 문자열이 소문자만 포함하는지 테스트합니다.

24. `ispunctuation()`: 문자열이 구두점으로 구성되어 있는지 테스트합니다.

25. `isupper()`: 문자열이 대문자만 포함하는지 테스트합니다.

26. `isutf8()`: 문자열이 UTF8 문자를 포함하는지 테스트합니다.

27. `isvowel()`: 문자열이 모음만 포함하는지 테스트합니다.

28. `last()`: 문자열의 마지막 문자를 반환합니다.

29. `latin()`: UTF8 문자열을 LATIN으로 변환합니다.

30. `left(int nb)`: 문자열의 처음 `nb`개의 문자를 반환합니다.

31. `levenshtein(string s)`: Levenshtein 알고리즘에 따라 문자열 `s`와의 편집 거리를 반환합니다.

32. `parenthetic()`: 괄호식을 벡터로 변환합니다.

33. `parenthetic(string opening, string closing, bool comma, bool separator, bool keepwithdigit, svector rules)`: 괄호식을 사용하여 재귀 표현식을 벡터로 변환합니다. `comma`에서 `rules`까지의 매개변수는 모두 선택 사항입니다. 이러한 규칙을 처리하는 방법에 대해서는 아래의 "토큰화 규칙"을 참조하십시오.

34. `lower()`: 문자열을 소문자로 반환합니다.

35. `mid(int pos, int nb)`: 문자열의 위치 `pos`에서 시작하는 `nb`개의 문자를 반환합니다.

36. `ngrams(int n)`: 순위 `n`의 모든 n-gram의 벡터를 반환합니다.

37. `ord()`: 문자열 문자의 코드를 반환합니다. 수신 변수의 유형에 따라 첫 번째 문자의 코드 또는 코드 목록을 보낼 수 있습니다.

38. `parse()`: 문자열을 코드로 구문 분석하고 평가 결과를 벡터로 반환합니다.

39. `pop()`: 문자열의 마지막 문자를 제거합니다.

40. `pop(i)`: 위치 `i`의 문자를 제거합니다.

41. `read(string file)`: 파일을 문자열로 읽어옵니다.

42. `removefirst(int nb)`: 문자열의 처음 `nb`개의 문자를 제거합니다.

43. `removelast(int nb)`: 문자열의 마지막 `nb`개의 문자를 제거합니다.

44. `replace(sub, str)`: `sub`와 일치하는 부분 문자열을 `str`로 대체합니다. `sub`는 TREG일 수 있습니다.

45. `reverse()`: 문자열을 반전시킵니다.

46. `rfind(string sub, int pos)`: 위치 `pos`에서 시작하여 뒤로 이동하면서 부분 문자열 `sub`의 위치를 반환합니다.

47. `right(int nb)`: 문자열의 마지막 `nb`개의 문자를 반환합니다.

48. `scan(rgx, string sep, bool immediatematch, string remaining)`: TREG 형식에 따라 `rgx`와 일치하는 모든 부분 문자열을 반환합니다. `rgx` 매개변수는 TREG 정규식입니다. 수신 변수에 따라 위치(int), 부분 문자열(string), 위치의 벡터(ivector) 또는 부분 문자열의 벡터(svector)를 반환