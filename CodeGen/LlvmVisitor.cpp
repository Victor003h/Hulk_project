// #include "LlvmVisitor.hpp"
// #include "CodeGenerationContext.hpp"


// void LlvmVisitor::visit(ProgramNode* node)           {};
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