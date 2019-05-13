CC=clang-7
CFLAGS=-g `llvm-config --cflags`
LD=clang++-7
LDFLAGS=`llvm-config --cxxflags --ldflags --libs core executionengine mcjit interpreter analysis native bitwriter --system-libs`
objects=littlecc.o codegen.o parser.o ast.o lexer.o symtable.o

all: littlecc littlecc.ll

littlecc.o: littlecc.c ast.h codegen.h parser.h
	$(CC) $(CFLAGS) -c $^

symtable.o: symtable.c symtable.h
	$(CC) $(CFLAGS) -c $^

codegen.o: codegen.c codegen.h
	$(CC) $(CFLAGS) -c $^

parser.o: parser.c ast.h lexer.h parser.h
	$(CC) -c $^

ast.o: ast.c ast.h
	$(CC) -c $^

lexer.o: lexer.c lexer.h
	$(CC) -c $^

littlecc: $(objects)
	$(LD) $^ $(LDFLAGS) -o $@

littlecc.ll: littlecc.bc
	llvm-dis $<

.PHONY: clean
clean:
	rm -f $(objects) littlecc.bc littlecc.ll littlecc *.gch