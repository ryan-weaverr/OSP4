all: 
	gcc -g -Wall Iru.c
	./a.out < test_1.in | diff - test_1.out
clean: 
	$(RM) ./a.out Iru.h.gch