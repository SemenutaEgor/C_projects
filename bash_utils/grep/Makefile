CC=gcc
CFLAGS=-Wall -Wextra -Werror
GDB=-g
STD=-std=c11 -pedantic
SRC_GREP=s21_grep.c s21_grep_funcs.c
HDR=s21_grep_funcs.h
EXE_GREP=s21_grep

.PHONY: all clean rebuild clang style valgrind cpp

all: s21_grep

s21_grep:
	$(CC) $(CFLAGS) $(STD) $(GDB) $(SRC_GREP) -o $(EXE_GREP)

clean:
	rm -rf $(EXE_GREP)

rebuild: clean all

clang:
	clang-format -style=google -i $(SRC_GREP)
	clang-format -style=google -i $(HDR)

cpp:
	cppcheck --enable=all --suppress=missingIncludeSystem $(SRC_GREP)

leaks: $(EXE_GREP)
	leaks -atExit -- $(word 1,$^)

valgrind: 
	valgrind --tool=memcheck --leak-check=full --track-origins=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./$(EXE_GREP) 

