CC ?= gcc
PROG ?= t4

OPTIM   = -O3 -march=native -mtune=native -pipe -fivopts
OPTIM  += -fmodulo-sched -flto -fwhole-program -fno-plt
CFLAGS ?= -std=c99 -Wall -Werror -Wpedantic -Wunused-result $(OPTIM)

VGFLAGS ?= --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --error-exitcode=10


.PHONY: all clean debug run

all: clean main

debug: OPTIM = -O0 -ggdb3 -DDEBUG
debug: all

main: $(PROG).c
	$(CC) $(CFLAGS) $< -o $@

run: main
	valgrind $(VGFLAGS) ./$<

test: main
	@for arq in $$(ls tests/*.in); do \
		./$< <$$arq | diff -qs - $$(echo $$arq | sed s/.in/.out/g); \
	done

memcheck: main
	@for arq in $$(ls tests/*.in); do \
		logfile=$$(echo $$arq | sed s-tests/--g | sed s/.in/.valgrind/g); \
		valgrind $(VGFLAGS) ./$< <$$arq &> $$logfile; \
		if [ $$? -eq 10 ]; then \
			echo $$arq ERROR; \
		else \
			echo $$arq OK; \
		fi \
	done

clean:
	rm -f main *.valgrind vgcore.*
