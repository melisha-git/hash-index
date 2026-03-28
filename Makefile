all:
	gcc -O0 -g3 main.c split.c hash.c data.c disk_logic.c -lm

tests:
	gcc -O0 -g3 test/split_test.c split.c hash.c data.c disk_logic.c -lm

clean:
	rm -f a.out