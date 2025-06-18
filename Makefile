CXX = g++
CXXFLAGS = -std=c++17 -I./src -Wall -g
SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)
BIN = hulk/hulk.exe
LLFILE = hulk/hulk.ll

all: execute

# Compilar el ejecutable
$(BIN): main.cpp
	@mkdir -p hulk
	$(CXX) $(CXXFLAGS) main.cpp -o $(BIN) `llvm-config --cxxflags --ldflags --libs core` -Wl,-subsystem,console

# Regla para compilar y generar el .ll (ejecutando el binario)
compile: $(BIN)
	@echo "Generando archivo LLVM IR: $(LLFILE)"
	$(BIN)

# Ejecutar el archivo .ll, si no existe, se genera primero
execute: $(LLFILE)
	@echo "Ejecutando LLVM IR: $(LLFILE)"
	lli $(LLFILE)

# El archivo .ll depende del ejecutable y se genera al ejecutarlo
$(LLFILE): $(BIN)
	@echo "Archivo LLVM IR no existe, generando..."
	$(BIN)

clean:
	rm -f hulk/*.exe hulk/*.ll
