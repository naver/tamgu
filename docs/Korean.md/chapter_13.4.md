## treg을 문자열에서 사용하기

treg을 사용하여 문자열에서 내용을 추출하거나 수정하는 것도 가능합니다.

```python
uu = "That was: 0x1.0d0e44bfeec9p -3, 0x3.456aebp -1 in here."
print("Tst: <", uu[-r"%d" :" "], ">\\n")  # Tst: <1 >
println("Tst2:", uu["x":r"%d %c"])  # Tst2: x1.0d0e44bfeec9p -3, 0x3.456aebp -1 i
```

treg 식을 treg 타입의 변수로 대체하는 것도 가능합니다.

```python
subt = treg(r"%d %c")
println("Tst3:", uu["x":subt])  # Tst3: x1.0d0e44bfeec9p -3, 0x3.456aebp -1 i
```

## Taskell

treg 표현식을 Taskell 함수와 함께 사용할 수도 있습니다.

```python
mytreg(r"%C+") = "uppercase"
mytreg([r"%d+":v]) = "ok"
mytreg(v) = "missed"

println(mytreg("ABC"))  # uppercase
println(mytreg(["3a4", 1, 4, 5]))  # ok
```

## Prolog

Prolog에서도 사용할 수 있습니다.

```prolog
tst(r"%d+").  % treg 식이 여기 있습니다...
tst("machin").
truc("Ok", ?P) :- tst(?P).
vector vpred.
vpred = truc(?P, "machin").
println(vpred)  % [truc("Ok",machin)]
vpred = truc(?P, "12").
println(vpred)  % [truc("Ok",12)]
vpred = truc(?P, "nope").
println(vpred)  % []
```