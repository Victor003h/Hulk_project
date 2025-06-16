CXX=clang++
CXXFLAGS=-std=c++17 -I. -Wall

# Ajusta esto con el path donde instalaste LLVM
LLVM_DIR="C:/Program Files/LLVM"

# Paths y flags para LLVM
LLVM_INCLUDE=-I$(LLVM_DIR)/include
LLVM_LIB=-L$(LLVM_DIR)/lib
LLVM_FLAGS=-lLLVM

# Targets individuales
lexer:
	$(CXX) $(CXXFLAGS) Lexer/main.cpp -o lexer.exe

parser:
	$(CXX) $(CXXFLAGS) Parser/main.cpp -o parser.exe

semantic:
	$(CXX) $(CXXFLAGS) Semantic/main.cpp -o semantic.exe

codegen:
	$(CXX) $(CXXFLAGS) $(LLVM_INCLUDE) CodeGen/main.cpp -o codegen.exe $(LLVM_LIB) $(LLVM_FLAGS)

# Target para compilar todo
all:	lexer	parser	semantic	codegen

# Limpiar binarios
clean:
	del /Q *.exe
