CC				= gcc
OBJFLAGS		= -c -Wall -pedantic -ansi -g -std=c99
all: Example

Example: Example.c jongine.c
	@echo "Creando objetos para $@... "
	$(CC) $(OBJFLAGS) $^
	@echo "Creando ejecutable $@..."
	$(CC) $(^:.c=.o) -o $@ -lSDL2
	@rm -f *.o
	@echo == OK ==

clean:
	@echo "Borrando objetos, ejecutables, etc."
	@rm -fr Doxyfile latex html
	@rm -f *~ *.o Example