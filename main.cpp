#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "./Parser/parser.cpp"
#include "./Lexer/Lexer.cpp"
#include "./Semantic/SemanticAnalizer.cpp"
#include "./CodeGen/CodeGenerationContext.hpp"


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <archivo.hulk>" << std::endl;
        return 1;
    }


    // 1. Obtener el nombre del archivo .hulk desde la línea de comandos.
    std::string filename = argv[1];

    // 2. Abrir el archivo y leer su contenido en un std::string.
    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Error al abrir el archivo: " << filename << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    std::string input = buffer.str();
    inputFile.close();

    
    ErrorHandler error;
    Lexer lexer(error);
    auto tokens=lexer.scanTokens(input);
    if(lexer.errorHandler.hasErrors())
    {
        return 0;
    }


     Parser parser(error);
     auto cst= parser.parse(tokens);
     
    if(cst==nullptr || parser.errorHandler.hasErrors())
    {
        parser.errorHandler.printErrors();
        return 0;
    }

    AstBuilderVisitor collector;
    AstNode* ast=cst->accept(collector);

    ast->print();

    SemanticAnalizer semantic;
    semantic.errorHandler=error;

    semantic.check(ast);
    if(semantic.errorHandler.hasErrors())
    {
      
      return 0;
    };


    CodeGenerationContext codegen;

    codegen.generateIR(ast);

    codegen.dumpIR();
    return 0;


    return 0;
}
