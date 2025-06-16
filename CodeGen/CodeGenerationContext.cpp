
// #include "CodeGenerationContext.hpp"
// #include "llvm/Support/FileSystem.h"
// #include "LlvmVisitor.hpp"

// class LlvmVisitor:public Visitor
// {
// public:
//     CodeGenerationContext &cgContext;
//     llvm::Value* lastValue;

//     LlvmVisitor(CodeGenerationContext &cgContext)
//     : cgContext(cgContext),lastValue(nullptr){}

//     void visit(ProgramNode* node)     ;
//     void visit(TypeNode* node)            ;
//     void visit(BlockNode* node)            ;

//     void visit(BinaryExpression* node)     ;
//     void visit(LiteralNode* node)          ;
//     void visit(IdentifierNode* node)    ;
//     void visit(AtributeNode* node)          ;
//     void visit(MethodNode* node)        ;
//     void visit(IfExpression* node)          ;
//     void visit(WhileExpression* node)       ;
//     void visit(ForExression* node)          ;
//     void visit(LetExpression* node)         ;
//     void visit(UnaryExpression* node)       ;    


// };


// void CodeGenerationContext::generateIR(AstNode* root)
// {
//     LlvmVisitor visitor(*this);
//     root->accept(visitor);

// }

// void CodeGenerationContext::dumpIR(const std::string &filename) {
//         std::error_code EC;
//         llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);
//         if (EC) {
//             llvm::errs() << "Error al abrir el archivo: " << EC.message();
//             return;
//         }
//         llvmModule.print(dest, nullptr);
// }



// void LlvmVisitor::visit(ProgramNode* node)     
// {
//     for(auto stmt:node->stmts)
//     {
//         stmt->accept(*this);
//     }
// };
// void LlvmVisitor::visit(TypeNode* node)              {};
// void LlvmVisitor::visit(BlockNode* node)             {};

// void LlvmVisitor::visit(BinaryExpression* node)     
//     {
//          node->left->accept(*this);
//         llvm::Value* leftvalue=lastValue;
//         node->right->accept(*this);
//         llvm::Value* rightvalue=lastValue;

//         if(node->op.lexeme=="+")
//             lastValue=cgContext.irBuilder.CreateAdd(leftvalue,rightvalue,"addtmp");
//         else if(node->op.lexeme=="-")
//             lastValue=cgContext.irBuilder.CreateAdd(leftvalue,rightvalue,"subtmp");
//         else if(node->op.lexeme=="*")
//             lastValue=cgContext.irBuilder.CreateMul(leftvalue,rightvalue,"multmp");
//         else if(node->op.lexeme=="*")
//             lastValue=cgContext.irBuilder.CreateSDiv(leftvalue,rightvalue,"divtmp");
//         else
//         {
//             llvm::errs() << "Operador binario no soportado: " << node->op.lexeme << "\n";
//                 lastValue = nullptr;   
//         }

//     };
// void LlvmVisitor::visit(IdentifierNode* node)        {};
// void LlvmVisitor::visit(AtributeNode* node)          {};
// void LlvmVisitor::visit(MethodNode* node)            {};
// void LlvmVisitor::visit(IfExpression* node)          {};
// void LlvmVisitor::visit(WhileExpression* node)       {};
// void LlvmVisitor::visit(ForExression* node)          {};
// void LlvmVisitor::visit(LetExpression* node)         {};
// void LlvmVisitor::visit(UnaryExpression* node)       {};    

// void LlvmVisitor::visit(LiteralNode* node)
// {
//     if(node->type=="Number")
//     {
//         double numVal = std::stod(node->value.lexeme);
//         lastValue = llvm::ConstantFP::get(cgContext.llvmContext, llvm::APFloat(numVal));
//     }

// }