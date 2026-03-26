all:
	gcc -O0 -g3 main.c split.c hash.c -lm

tests:
	gcc -O0 -g3 test/split_test.c split.c hash.c

clean:
	rm -f a.out