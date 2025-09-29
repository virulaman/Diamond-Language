# DMO Programming Language Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = dmo
SRCDIR = .
EXAMPLEDIR = examples

# Source files
SOURCES = main.c lexer.c parser.c ast.c interpreter.c modules.c stdlib_funcs.c dmo_graphs.c math_module.c request_module.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Header files
HEADERS = lexer.h parser.h ast.h interpreter.h modules.h stdlib_funcs.h dmo_graphs.h math_module.h request_module.h

.PHONY: all clean examples test install

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) -lm -lcurl

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) *.svg

examples: $(TARGET)
	@echo "Running DMO language examples..."
	@echo "=================================="
	@echo "Example 1: Hello World"
	./$(TARGET) $(EXAMPLEDIR)/hello.dmo
	@echo ""
	@echo "Example 2: Graphics Demo"
	./$(TARGET) $(EXAMPLEDIR)/graphics_demo.dmo
	@echo ""
	@echo "Example 3: Modules Demo"
	./$(TARGET) $(EXAMPLEDIR)/modules_demo.dmo
	@echo ""

test: examples

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/
	mkdir -p /usr/local/lib/dmo
	cp -r $(EXAMPLEDIR) /usr/local/lib/dmo/

debug: CFLAGS += -DDEBUG
debug: $(TARGET)

# Individual file compilation rules
main.o: main.c lexer.h parser.h interpreter.h modules.h
lexer.o: lexer.c lexer.h
parser.o: parser.c parser.h lexer.h ast.h
ast.o: ast.c ast.h lexer.h
interpreter.o: interpreter.c interpreter.h ast.h stdlib_funcs.h dmo_graphs.h modules.h
modules.o: modules.c modules.h interpreter.h stdlib_funcs.h dmo_graphs.h math_module.h request_module.h
stdlib_funcs.o: stdlib_funcs.c stdlib_funcs.h interpreter.h dmo_graphs.h
dmo_graphs.o: dmo_graphs.c dmo_graphs.h interpreter.h
math_module.o: math_module.c math_module.h interpreter.h
request_module.o: request_module.c request_module.h interpreter.h

help:
	@echo "DMO Programming Language Build System"
	@echo "====================================="
	@echo "Available targets:"
	@echo "  all      - Build the DMO compiler"
	@echo "  clean    - Remove build artifacts"
	@echo "  examples - Run example programs"
	@echo "  test     - Run tests (same as examples)"
	@echo "  install  - Install DMO system-wide"
	@echo "  debug    - Build with debug flags"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Usage: make [target]"
	@echo "Example: make all && make examples"
