all:
	gcc -O0 -g3 main.c split.c hash.c data.c disk_logic.c -lm

tests:
	gcc -O0 -g3 test/tests.c test/split_test.c split.c hash.c data.c disk_logic.c -lm -Wl,--wrap=calloc

clean:
	rm -f a.out