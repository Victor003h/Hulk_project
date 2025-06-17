#pragma once
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/FileSystem.h"
#include <string>
#include <unordered_map>
#include <vector>
#include "../Parser/AstNodes.h"
#include "llvm/IR/Value.h"
#include <llvm/IR/Verifier.h>
#include "llvm/IR/Function.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Support/raw_ostream.h"

// Declaraciones anticipadas
//class FunctionDeclarationNode;


// Esta clase agrupa todo el estado y contexto para generar el LLVM IR
class CodeGenerationContext {
public:
    // Componentes LLVM globales
    llvm::LLVMContext llvmContext;               // Contexto global de LLVM (tipos, constantes, etc.)
    llvm::IRBuilder<> irBuilder;                 // Constructor para crear instrucciones IR
    llvm::Module llvmModule;                     // Módulo IR de LLVM (una unidad de compilación)

    std::unordered_map<std::string, llvm::StructType*> namedTypes;

    // Tabla de símbolos globales para variables (por ejemplo, "self") o constantes globales
    std::unordered_map<std::string, llvm::Value*> globalVariables;
    
    // Constructor: inicializa el IRBuilder y el módulo
    CodeGenerationContext()
        : irBuilder(llvmContext),
          llvmModule("main_module", llvmContext) {}

    // Función principal de entrada: genera el IR a partir de la raíz del AST
    void generateIR(AstNode*root);
    // Vuelca el IR a un archivo (por defecto "hulk-low-code.ll")
   
    void dumpIR(const std::string& filename = "hulk-low-code.ll");

    // Manejo de ámbitos (scopes) para variables locales y declaraciones de funciones
    std::vector<std::unordered_map<std::string, llvm::Value*>> variableScopes;
    std::vector<std::unordered_map<std::string, MethodNode*>> functionScopes;

    // Manejo de ámbitos para variables
    inline void pushLocalScope() { variableScopes.emplace_back(); }
    inline void popLocalScope()  { if (!variableScopes.empty()) variableScopes.pop_back(); }
    inline void addLocalVariable(const std::string& name, llvm::Value* value) {
        if (!variableScopes.empty())
            variableScopes.back()[name] = value;
    }
    llvm::Value* getLocalVariable(const std::string& name) const {
        for (auto it = variableScopes.rbegin(); it != variableScopes.rend(); ++it) {
            auto found = it->find(name);
            if (found != it->end())
                return found->second;
        }
        return nullptr;
    }

    llvm::AllocaInst* createEntryBlockAlloca(llvm::Function *function, llvm::Type* type,const std::string &varName) 
    {
        llvm::IRBuilder<> tmpBuilder(&function->getEntryBlock(),function->getEntryBlock().begin());
        return tmpBuilder.CreateAlloca(type, nullptr, varName);
    }


    //Manejo de ámbitos para declaraciones de funciones
    inline void pushFunctionScope() { functionScopes.emplace_back(); }
    inline void popFunctionScope()  { if (!functionScopes.empty()) functionScopes.pop_back(); }
    inline void declareFunction(const std::string& name, MethodNode* decl) {
        if (!functionScopes.empty())
            functionScopes.back()[name] = decl;
    }
    MethodNode* getFunctionDeclaration(const std::string& name) const {
        for (auto it = functionScopes.rbegin(); it != functionScopes.rend(); ++it) {
            auto found = it->find(name);
            if (found != it->end())
                return found->second;
        }
        return nullptr;
    }

    // Esta función te permite obtener (o crear) la cadena de formato "%g\n" de forma global.
    llvm::Value* getFormatString()
    {
        llvm::GlobalVariable* formatVar = llvmModule.getNamedGlobal("formatStr");
        if (!formatVar) 
        {
            llvm::Constant* formatConst = llvm::ConstantDataArray::getString(llvmModule.getContext(), "%g\n", true);
            formatVar = new llvm::GlobalVariable(
                llvmModule,
                formatConst->getType(),
                true, // Es una constante
                llvm::GlobalValue::PrivateLinkage,
                formatConst,
                "formatStr"
            );
        }
        // Asegurarse de obtener un tipo pointer a i8
        return irBuilder.CreateBitCast(formatVar, llvm::PointerType::get(llvm::Type::getInt8Ty(llvmModule.getContext()), 0));
    }

};



class LlvmVisitor:public Visitor
{
public:
    CodeGenerationContext &cgContext;
    llvm::Value* lastValue;
    llvm::Function* currentFunction;

    LlvmVisitor(CodeGenerationContext &cgContext)
    : cgContext(cgContext),lastValue(nullptr){}

    void visit(ProgramNode* node)     ;
    void visit(TypeNode* node)            ;
    void visit(BlockNode* node)            ;

    void visit(BinaryExpression* node)     ;
    void visit(LiteralNode* node)          ;
    void visit(IdentifierNode* node)    ;
    void visit(AtributeNode* node)          ;
    void visit(MethodNode* node)        ;
    void visit(IfExpression* node)          ;
    void visit(WhileExpression* node)       ;
    void visit(ForExression* node)          ;
    void visit(LetExpression* node)         ;
    void visit(UnaryExpression* node)       ;    
    void visit(FunCallNode* node)            ;
    void visit(MemberCall* node)        ;
    void visit(DestructiveAssignNode* node)        ;

};



void CodeGenerationContext::generateIR(AstNode* root)
{
    pushFunctionScope();

    llvm::FunctionType* printfType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(llvmContext),
        {llvm::PointerType::get(llvm::Type::getInt8Ty(llvmContext), 0)},
        true // Variadic function
    );
    llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", &llvmModule);

 

    llvm::FunctionType* putsType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(llvmContext),
        {llvm::PointerType::get(llvm::Type::getInt8Ty(llvmContext), 0)},
        false
    );
    llvm::Function::Create(putsType, llvm::Function::ExternalLinkage, "puts", &llvmModule);


    llvm::FunctionType* mainType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(llvmContext), false
    );
    llvm::Function* mainFunc = llvm::Function::Create(
        mainType, llvm::Function::ExternalLinkage, "main", llvmModule
    );
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(llvmContext, "entry", mainFunc);
    irBuilder.SetInsertPoint(entry);

    LlvmVisitor visitor(*this);
    visitor.currentFunction=mainFunc;
    root->accept(visitor);


        auto val=visitor.lastValue;
        if(!val)    return;
        if (val->getType()->isDoubleTy()) { // Number
            llvm::Value* format = getFormatString();
            irBuilder.CreateCall(llvmModule.getFunction("printf"), {format, val});
        } else if (val->getType()->isIntegerTy(1)) { // Boolean
            llvm::Value* str = irBuilder.CreateSelect(
                val,
                irBuilder.CreateGlobalStringPtr("true\n"),
                irBuilder.CreateGlobalStringPtr("false\n")
            );
            irBuilder.CreateCall(llvmModule.getFunction("puts"), {str});
        } else if (val->getType()->isPointerTy()) { // String
            irBuilder.CreateCall(llvmModule.getFunction("puts"), {val});
        }
    
   

    irBuilder.CreateRet(llvm::ConstantInt::get(llvmContext, llvm::APInt(32, 0)));
    llvm::verifyFunction(*mainFunc);

}

void CodeGenerationContext::dumpIR(const std::string &filename) {
        std::error_code EC;
        llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);
        if (EC) {
            llvm::errs() << "Error al abrir el archivo: " << EC.message();
            return;
        }
        llvmModule.print(dest, nullptr);
}



void LlvmVisitor::visit(ProgramNode* node)          
 {
    for(auto stmt:node->stmts)
    {
        stmt->accept(*this);
    }


 };

void LlvmVisitor::visit(TypeNode* node) 
{
    auto& ctx = cgContext;

    // 1. Crear estructura con atributos
    std::vector<llvm::Type*> memberTypes;
    for (auto* attr : node->atributes) {
        // Supongamos que todos los atributos son double
        memberTypes.push_back(llvm::Type::getDoubleTy(ctx.llvmContext));
    }

    llvm::StructType* classType = llvm::StructType::create(ctx.llvmContext, memberTypes, node->name.lexeme);
    ctx.namedTypes[node->name.lexeme] = classType;

    // 2. Crear función constructor
    std::vector<llvm::Type*> ctorArgs(memberTypes);
    llvm::FunctionType* ctorType = llvm::FunctionType::get(
        llvm::PointerType::getUnqual(classType), // return: pointer to struct
        ctorArgs,
        false
    );

    llvm::Function* ctorFunc = llvm::Function::Create(
        ctorType,
        llvm::Function::ExternalLinkage,
        node->name.lexeme + "_ctor",
        ctx.llvmModule
    );

    llvm::BasicBlock* entry = llvm::BasicBlock::Create(ctx.llvmContext, "entry", ctorFunc);
    ctx.irBuilder.SetInsertPoint(entry);

    llvm::Value* self = ctx.irBuilder.CreateAlloca(classType, nullptr, "self");

    int index = 0;
    for (llvm::Argument& arg : ctorFunc->args()) {
        arg.setName("arg" + std::to_string(index));
        llvm::Value* fieldPtr = ctx.irBuilder.CreateStructGEP(classType, self, index, "ptr");
        ctx.irBuilder.CreateStore(&arg, fieldPtr);
        ++index;
    }

    ctx.irBuilder.CreateRet(self);

    // 3. Visitar los métodos (cada uno se convierte en función)
    for (auto* method : node->methods) {
        method->accept(*this);  // Delegamos a visit(FunctionNode*)
    }
}

void LlvmVisitor::visit(BlockNode* node) 
{
    llvm::Value* value=nullptr;

    for(auto exp:node->exprs)
    {
        exp->accept(*this);
        
        value=lastValue;
    }

    lastValue=value;

    
};

void LlvmVisitor::visit(BinaryExpression* node)     
    {
        node->left->accept(*this);
        llvm::Value* leftvalue=lastValue;
        node->right->accept(*this);
        llvm::Value* rightvalue=lastValue;

        if(node->op.lexeme=="+")
            lastValue=cgContext.irBuilder.CreateFAdd(leftvalue,rightvalue,"addtmp");
        else if(node->op.lexeme=="-")
            lastValue=cgContext.irBuilder.CreateFSub(leftvalue,rightvalue,"subtmp");
        else if(node->op.lexeme=="*")
            lastValue=cgContext.irBuilder.CreateFMul(leftvalue,rightvalue,"multmp");
        else if(node->op.lexeme=="/")
            lastValue=cgContext.irBuilder.CreateFDiv(leftvalue,rightvalue,"divtmp");
        else if(node->op.lexeme=="%")
            lastValue=cgContext.irBuilder.CreateFRem(leftvalue,rightvalue,"modtmp");
        else if(node->op.lexeme=="^")
        {
             llvm::Function *PowFunc = llvm::Intrinsic::getDeclaration(&(cgContext.llvmModule), llvm::Intrinsic::pow,
                                                                { llvm::Type::getDoubleTy(cgContext.llvmContext) });

            lastValue=cgContext.irBuilder.CreateCall(PowFunc, {leftvalue, rightvalue}, "powtmp");
        }
        else if(node->op.lexeme==">")
            lastValue = cgContext.irBuilder.CreateFCmpUGT(leftvalue, rightvalue, "gttmp");
        else if(node->op.lexeme==">=")
            lastValue = cgContext.irBuilder.CreateFCmpUGE(leftvalue, rightvalue, "gttmp");
        else if(node->op.lexeme=="<")
            lastValue = cgContext.irBuilder.CreateFCmpULT(leftvalue, rightvalue, "gttmp");
        else if(node->op.lexeme=="<=")
            lastValue = cgContext.irBuilder.CreateFCmpULE(leftvalue, rightvalue, "gttmp");
       
        else if(node->op.lexeme=="=="||node->op.lexeme=="!=" )
        {
            
            if(node->left->getType()=="Number")
            {
                if(node->op.lexeme=="==")
                    lastValue=cgContext.irBuilder.CreateFCmpUEQ(leftvalue, rightvalue, "eqtmp");
                else
                    lastValue=cgContext.irBuilder.CreateFCmpUNE(leftvalue, rightvalue, "netmp");
            }
             if(node->left->getType()=="Boolean")
            {
                if(node->op.lexeme=="==")
                    lastValue=cgContext.irBuilder.CreateICmpEQ(leftvalue, rightvalue, "beqtmp");
                else
                    lastValue=cgContext.irBuilder.CreateICmpNE(leftvalue, rightvalue, "bnetmp");
            }
        }
        else if(node->op.lexeme=="&&" )
            lastValue=cgContext.irBuilder.CreateAnd(leftvalue,rightvalue,"andtmp");
        else if(node->op.lexeme=="||" )
            lastValue=cgContext.irBuilder.CreateOr(leftvalue,rightvalue,"ortmp");



        else
        {
            llvm::errs() << "Operador binario no soportado: " << node->op.lexeme << "\n";
                lastValue = nullptr;   
        }

        

    };

void LlvmVisitor::visit(IdentifierNode* node) 
{

    llvm::Value* varAlloca=cgContext.getLocalVariable(node->value.lexeme);
    if(!varAlloca)
    {
        return ; ////// add error here
    }

    llvm::Type* varType = llvm::cast<llvm::AllocaInst>(varAlloca)->getAllocatedType();
     
    llvm::Value* value = cgContext.irBuilder.CreateLoad(varType, varAlloca, node->value.lexeme);
    lastValue = value;
    

};


void LlvmVisitor::visit(AtributeNode* node)          {};

void LlvmVisitor::visit(MethodNode* node) 
{
    llvm::BasicBlock* prevBlock = cgContext.irBuilder.GetInsertBlock();

    std::vector<llvm::Type*> paramTypes(node->params.size(), llvm::Type::getDoubleTy(cgContext.llvmContext)); // Usamos double por defecto

    llvm::FunctionType* funcType = llvm::FunctionType::get(
        llvm::Type::getDoubleTy(cgContext.llvmContext), // tipo de retorno
        paramTypes,
        false
    );

    llvm::Function* function = llvm::Function::Create(
        funcType,
        llvm::Function::ExternalLinkage,
        node->id.lexeme,
        cgContext.llvmModule
    );

    // Crear bloque de entrada
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(cgContext.llvmContext, "entry", function);
    cgContext.irBuilder.SetInsertPoint(entry);

    // Scope para variables locales
    cgContext.pushLocalScope();

    // Asignar parámetros a variables locales
    size_t idx = 0;
    for (auto& arg : function->args()) {
        arg.setName(dynamic_cast<IdentifierNode*>(node->params[idx])->value.lexeme);

        llvm::AllocaInst* alloca = cgContext.createEntryBlockAlloca(function, llvm::Type::getDoubleTy(cgContext.llvmContext), arg.getName().str());
        cgContext.irBuilder.CreateStore(&arg, alloca);
        cgContext.addLocalVariable(arg.getName().str(), alloca);

        ++idx;

    }

    // Generar cuerpo
    node->body->accept(*this);

    // Manejo de retorno
    if (lastValue) {
        cgContext.irBuilder.CreateRet(lastValue);
    } else {
        cgContext.irBuilder.CreateRet(llvm::ConstantFP::get(cgContext.llvmContext, llvm::APFloat(0.0))); // default
    }

    

    cgContext.popLocalScope(); // restaurar scope anterior

    cgContext.irBuilder.SetInsertPoint(prevBlock);


}


// void LlvmVisitor::visit(IfExpression* node) {

//     llvm::Function* function = cgContext.irBuilder.GetInsertBlock()->getParent();// currentFunction

//     llvm::BasicBlock* endBlock = llvm::BasicBlock::Create(cgContext.llvmContext, "ifend", function);
//     llvm::PHINode* phi = nullptr;

//     std::vector<std::pair<llvm::BasicBlock*, llvm::Value*>> results;

//     // Generar bloques para cada condicional (if, else if, etc.)
//     for (size_t i = 0; i < node->exprs_cond.size(); ++i) {
//         auto [condNode, thenNode] = node->exprs_cond[i];

//         llvm::BasicBlock* condBlock = llvm::BasicBlock::Create(cgContext.llvmContext, "ifcond", function);
//         llvm::BasicBlock* thenBlock = llvm::BasicBlock::Create(cgContext.llvmContext, "ifthen", function);

//         // Saltar al bloque de la condición
//         cgContext.irBuilder.CreateBr(condBlock);
//         cgContext.irBuilder.SetInsertPoint(condBlock);

//         // Generar condición
//         condNode->accept(*this);
//         llvm::Value* condValue = lastValue;

//         // Convertir a booleano si es necesario  Si la condición es un double, comparamos con 0.0 para convertirla a booleano (true si diferente de cero).
//         if (condValue->getType()->isDoubleTy()) {
//             condValue = cgContext.irBuilder.CreateFCmpONE(
//                 condValue, llvm::ConstantFP::get(cgContext.llvmContext, llvm::APFloat(0.0)), "booltmp");
//         }

//         // Crear bloque else o siguiente condicional
//         llvm::BasicBlock* nextBlock = (i == node->exprs_cond.size() - 1 && node->defaultExp)
//                                       ? llvm::BasicBlock::Create(cgContext.llvmContext, "ifelse", function)
//                                       : llvm::BasicBlock::Create(cgContext.llvmContext, "ifnext", function);

//         // Branch basado en condición
//         cgContext.irBuilder.CreateCondBr(condValue, thenBlock, nextBlock);

//         // THEN block
//         cgContext.irBuilder.SetInsertPoint(thenBlock);
//         thenNode->accept(*this);
//         llvm::Value* thenValue = lastValue;
//         cgContext.irBuilder.CreateBr(endBlock);
//         results.push_back({thenBlock, thenValue});

//         // Siguiente bloque condicional (condBlock ya está enlazado)
//         cgContext.irBuilder.SetInsertPoint(nextBlock);
//     }

//     // ELSE final
//     if (node->defaultExp) {
//         llvm::BasicBlock* elseBlock = cgContext.irBuilder.GetInsertBlock();
//         node->defaultExp->accept(*this);
//         llvm::Value* elseValue = lastValue;
//         cgContext.irBuilder.CreateBr(endBlock);
//         results.push_back({elseBlock, elseValue});
//     }

//     // END block
//     function->insert(function->end(), endBlock);
//     cgContext.irBuilder.SetInsertPoint(endBlock);

//     // PHI para juntar los resultados
//     if (!results.empty()) {
//         phi = cgContext.irBuilder.CreatePHI(results[0].second->getType(), results.size(), "iftmp");
//         for (auto& [block, value] : results) {
//             phi->addIncoming(value, block);
//         }
//         lastValue = phi;
//     } else {
//         lastValue = nullptr;
//     }

//     cgContext.valueStack.push_back(lastValue);

// }


void LlvmVisitor::visit(IfExpression* node) {
    llvm::Function* function = cgContext.irBuilder.GetInsertBlock()->getParent();
    llvm::BasicBlock* afterBlock = llvm::BasicBlock::Create(cgContext.llvmContext, "ifcont", function);

    std::vector<std::pair<llvm::BasicBlock*, llvm::BasicBlock*>> branches;
    for (size_t i = 0; i < node->exprs_cond.size(); ++i) {
        llvm::BasicBlock* condBlock = llvm::BasicBlock::Create(cgContext.llvmContext, "ifcond", function);
        llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create(cgContext.llvmContext, "ifbody", function);
        branches.emplace_back(condBlock, bodyBlock);
    }

    llvm::BasicBlock* elseBlock = node->defaultExp
        ? llvm::BasicBlock::Create(cgContext.llvmContext, "elsebody", function)
        : afterBlock;

    // Salto inicial
    if (!branches.empty()) {
        cgContext.irBuilder.CreateBr(branches[0].first);
    } else {
        cgContext.irBuilder.CreateBr(elseBlock);
    }

    std::vector<std::pair<llvm::BasicBlock*, llvm::Value*>> valueBlocks;

    for (size_t i = 0; i < node->exprs_cond.size(); ++i) {
        // Condición
        cgContext.irBuilder.SetInsertPoint(branches[i].first);
        node->exprs_cond[i].first->accept(*this);
        llvm::Value* condValue = lastValue;

        condValue = cgContext.irBuilder.CreateICmpNE(
            condValue,
            llvm::ConstantInt::get(condValue->getType(), 0),
            "ifcondbool"
        );

        llvm::BasicBlock* nextCond = (i + 1 < branches.size()) ? branches[i + 1].first : elseBlock;
        cgContext.irBuilder.CreateCondBr(condValue, branches[i].second, nextCond);

        // Cuerpo
        cgContext.irBuilder.SetInsertPoint(branches[i].second);
        node->exprs_cond[i].second->accept(*this);
        if (lastValue)
            valueBlocks.emplace_back(cgContext.irBuilder.GetInsertBlock(), lastValue);
        cgContext.irBuilder.CreateBr(afterBlock);
    }

    // ELSE
    if (node->defaultExp) {
        cgContext.irBuilder.SetInsertPoint(elseBlock);
        node->defaultExp->accept(*this);
        if (lastValue)
            valueBlocks.emplace_back(cgContext.irBuilder.GetInsertBlock(), lastValue);
        cgContext.irBuilder.CreateBr(afterBlock);
    }

    // AFTER
    cgContext.irBuilder.SetInsertPoint(afterBlock);

    // PHI si hay valores de retorno
    if (!valueBlocks.empty()) {
        llvm::Type* phiType = valueBlocks[0].second->getType();

        // Verificamos si todos tienen el mismo tipo
        bool allSameType = std::all_of(
            valueBlocks.begin(), valueBlocks.end(),
            [&](auto& vb) { return vb.second->getType() == phiType; }
        );

        // Si no, usamos double por defecto
        if (!allSameType) {
            phiType = llvm::Type::getDoubleTy(cgContext.llvmContext);
        }

        llvm::PHINode* phi = cgContext.irBuilder.CreatePHI(phiType, valueBlocks.size(), "iftmp");

        for (auto& [block, val] : valueBlocks) {
            if (val->getType() != phiType) {
                if (val->getType()->isIntegerTy() && phiType->isDoubleTy()) {
                    val = cgContext.irBuilder.CreateSIToFP(val, phiType, "int_to_double");
                } else if (val->getType()->isDoubleTy() && phiType->isIntegerTy()) {
                    val = cgContext.irBuilder.CreateFPToSI(val, phiType, "double_to_int");
                }
            }
            phi->addIncoming(val, block);
        }
        lastValue = phi;
    } else {
        lastValue = nullptr;
    }

    
    
}

void LlvmVisitor::visit(WhileExpression* node) 
{

    llvm::Function* function = cgContext.irBuilder.GetInsertBlock()->getParent();

    // Crear bloques para el ciclo while
    llvm::BasicBlock* condBlock = llvm::BasicBlock::Create(cgContext.llvmContext, "while.cond", function);
    llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create(cgContext.llvmContext, "while.body");
    llvm::BasicBlock* endBlock  = llvm::BasicBlock::Create(cgContext.llvmContext, "while.end");

    // Brincar al bloque de condición
    cgContext.irBuilder.CreateBr(condBlock);
    cgContext.irBuilder.SetInsertPoint(condBlock);

    // Generar el código de la condición
    node->condition->accept(*this); // Asume que el resultado está en lastValue
    llvm::Value* condValue = lastValue;

    // Convertir condición a booleano (i1)
    if (condValue->getType()->isIntegerTy(1) == false) {
        condValue = cgContext.irBuilder.CreateICmpNE(
            condValue,
            llvm::ConstantInt::get(condValue->getType(), 0),
            "while.cond.bool"
        );
    }

    // Branch dependiendo del resultado de la condición
    cgContext.irBuilder.CreateCondBr(condValue, bodyBlock, endBlock);

   // Insertar y entrar al cuerpo del while

    bodyBlock->insertInto(function); // Agrega manualmente
    cgContext.irBuilder.SetInsertPoint(bodyBlock);
    node->body->accept(*this);
    cgContext.irBuilder.CreateBr(condBlock); // Loop back

    // Insertar y moverse al bloque final
    endBlock->insertInto(function);
    cgContext.irBuilder.SetInsertPoint(endBlock);

    lastValue = nullptr; 

    
};

void LlvmVisitor::visit(ForExression* node)          {};

void LlvmVisitor::visit(LetExpression* node)      
{
    cgContext.pushLocalScope();
    for(auto asg:node->assignments)
    {
        auto atrib=dynamic_cast<AtributeNode*>(asg);

        atrib->expression->accept(*this);
        llvm::Value* expValue=lastValue;

        llvm::AllocaInst* varAlloca =cgContext.createEntryBlockAlloca(currentFunction,expValue->getType() ,atrib->id.lexeme);

        cgContext.irBuilder.CreateStore(expValue,varAlloca);

        cgContext.addLocalVariable(atrib->id.lexeme,varAlloca);
   
    }
    
    node->body->accept(*this);
    auto bodyvalue=lastValue;
   
    lastValue=bodyvalue;
    cgContext.popLocalScope();

};

void LlvmVisitor::visit(UnaryExpression* node)       {}; 

void LlvmVisitor::visit(FunCallNode* node) {
    // Buscar la función por su nombre
    llvm::Function* calleeFunc = cgContext.llvmModule.getFunction(node->id.lexeme);
    
    if (!calleeFunc) {
        std::cerr << "Error: función '" << node->id.lexeme << "' no definida.\n";
        lastValue = nullptr;
        return;
    }

    // Evaluar argumentos
    std::vector<llvm::Value*> args;
    for (AstNode* argNode : node->arguments) {
        argNode->accept(*this);  // llena lastValue
        if (!lastValue) {
            std::cerr << "Error: argumento inválido para llamada a función\n";
            lastValue = nullptr;
            return;
        }
        args.push_back(lastValue);
    }

    // Comprobación de aridad
    if (calleeFunc->arg_size() != args.size()) {
        std::cerr << "Error: número de argumentos no coincide con la función '" << node->id.lexeme << "'\n";
        lastValue = nullptr;
        return;
    }

    // Generar llamada
    lastValue = cgContext.irBuilder.CreateCall(calleeFunc, args, "calltmp");
  
    // Puedes establecer el tipo de retorno si tienes un sistema de tipos
    // node->setType(...);
}


void LlvmVisitor::visit(MemberCall* node)        {};

void LlvmVisitor::visit(LiteralNode* node)
{
    if(node->type=="Number")
    {
        double numVal = std::stod(node->value.lexeme);
        lastValue = llvm::ConstantFP::get(cgContext.llvmContext, llvm::APFloat(numVal));

      
        return;
    }
    else if(node->type=="String")
    {
        
    }
    else if(node->type=="Boolean")
    {
        bool b= node->value.lexeme=="true";
        lastValue=llvm::ConstantInt::get(llvm::Type::getInt1Ty(cgContext.llvmContext),b);
        return;
    }


}


void LlvmVisitor::visit(DestructiveAssignNode* node) 
{
    node->rhs->accept(*this);
    llvm::Value* right=lastValue;

    llvm::Value* ptr = nullptr;


     if (auto idNode = dynamic_cast<IdentifierNode*>(node->lhs)) 
     {
       
        ptr = cgContext.getLocalVariable(idNode->value.lexeme);
        if (!ptr) 
        {
            std::cerr << "Error: variable '" << idNode->value.lexeme << "' no definida\n";
            return;
        }

    } 
    else {
        std::cerr << "Error: LHS de := no soportado\n";
        return;
    }

    cgContext.irBuilder.CreateStore(right,ptr);

    lastValue=right;
    
}