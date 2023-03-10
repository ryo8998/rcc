#!/bin/bash
assert(){
    expected="$1"
    input="$2"

    ./rcc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected but got $actual"
        exit 1
    fi
}

assert 0 "0;"
assert 42 "42;"
assert 21 '5+20-4;'
assert 41 " 12 + 34 - 5;"
assert 47 '5 + 6 *7;'
assert 15 '5 *(9-6);'
assert 4  '(3+5)/2;'
assert 10 '-10+20;'
assert 15 '-5*+2+25;'
assert 1 '10==10;'
assert 0 '1==2;'
assert 0 '3 > 5;'
assert 1 '(1+4) < (2*5);'
assert 0 '3*3 != 9*1;'
assert 1 '5*3 >= 12/4;'
assert 1 '5*2 <= 20/2;'
assert 10 "a = 10;"
assert 11 "a =10;b=a;b+1;"
assert 10 "foo=10;"
# assert 20 'a=b=10;' このテストケースが通らない
assert 1  "return 1;"
assert 15 "return 15;return 3;"
assert 14 "a=3;b=5*6-8;return a+b/2;"
assert 5  "returnx = 5;"
assert 3  "if(1==1) return 3;"
assert 5  "if(0) return 3; return 5;"
assert 2  "if(1==1) a=2;"

echo OK