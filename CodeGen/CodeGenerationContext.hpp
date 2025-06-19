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

    std::unordered_map<std::string, llvm::StructType*> namedStruct;
    std::unordered_map<std::string, TypeNode*> namedTypeNode;
    std::unordered_map<std::string, std::vector<std::string>> classMemberNames;

    std::string currentType;
    
      
    // Constructor: inicializa el IRBuilder y el módulo
    CodeGenerationContext()
        : irBuilder(llvmContext),
          llvmModule("main_module", llvmContext) {}

    // Función principal de entrada: genera el IR a partir de la raíz del AST
    void generateIR(AstNode*root);
    // Vuelca el IR a un archivo (por defecto "hulk-low-code.ll")
   
    void dumpIR(const std::string& filename = "output.ll");

    

    // Manejo de ámbitos (scopes) para variables locales y declaraciones de funciones
    std::vector<std::unordered_map<std::string, llvm::AllocaInst*>> variableScopes;
    std::vector<std::unordered_map<std::string, MethodNode*>> functionScopes;

    // Manejo de ámbitos para variables
    inline void pushLocalScope() { variableScopes.emplace_back(); }
    inline void popLocalScope()  { if (!variableScopes.empty()) variableScopes.pop_back(); }
    inline void addLocalVariable(const std::string& name, llvm::AllocaInst* value) {
        if (!variableScopes.empty())
            variableScopes.back()[name] = value;
    }
    llvm::AllocaInst* getLocalVariable(const std::string& name) const {
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


    llvm::Type* getInteralType(std::string type)
    {
        if (type == "Number") 
            return llvm::Type::getDoubleTy(llvmContext);
        if (type == "String") 
            return llvm::PointerType::get(llvm::Type::getInt8Ty(llvmContext), 0);
        if (type == "Boolean") 
            return llvm::Type::getInt1Ty(llvmContext);
        if (type == "Object") 
            return llvm::PointerType::get(llvm::Type::getInt8Ty(llvmContext), 0);
        if (type == "Void" ) 
            return llvm::Type::getVoidTy(llvmContext);

        return nullptr;
    }

    int GetIndexOfMember(TypeNode*node,std::string member)
    {
        for (size_t i = 0; i < node->atributes.size(); i++)
        {
            auto attr= static_cast<AtributeNode*>(node->atributes[i]);
            if(attr->id.lexeme==member) return i;
        }
        
        return -1;
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
    void visit(TypeInstantiation* node)        ;

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
        llvm::raw_fd_ostream out("hulk/"+filename, EC, llvm::sys::fs::OF_Text);
        if (EC) {
            llvm::errs() << "Error al abrir el archivo: " << EC.message();
            return;
        }
        llvmModule.print(out, nullptr);
}



void LlvmVisitor::visit(ProgramNode* node)          
 {
    for(auto stmt:node->stmts)
    {
        stmt->accept(*this);
    }


 };


void LlvmVisitor::visit(TypeNode* node) {

   
    std::vector<std::string> attrNames;
    std::vector<llvm::Type*> attrTypes;
   
    for (AstNode* attr : node->atributes) {
        std::string attrTypeName = attr->getType();

        
        if ( cgContext.getInteralType(attrTypeName)) {
            llvm::Type* type = cgContext.getInteralType(attrTypeName);
            attrTypes.push_back(type);
            attrNames.push_back(static_cast<AtributeNode*>(attr)->id.lexeme);
           
        }
        else
        {
            llvm::Type* type = cgContext.namedStruct[attrTypeName];
            attrTypes.push_back(type);
            attrNames.push_back(static_cast<AtributeNode*>(attr)->id.lexeme);
        
        }
    
    }
 
    if (attrTypes.empty()) 
    {
        attrTypes.push_back(llvm::Type::getInt8Ty(cgContext.llvmContext));
    }
    cgContext.classMemberNames[node->name.lexeme]=attrNames;

   

    llvm::StructType* structType = llvm::StructType::create(cgContext.llvmContext, attrTypes, node->name.lexeme);
    cgContext.namedStruct[node->name.lexeme] = structType;
    cgContext.namedTypeNode[node->name.lexeme]=node;
    
    auto lasttype=cgContext.currentType;
    cgContext.currentType=node->name.lexeme;

    std::vector<llvm::Type*> ParamTypes;

    for (AstNode* arg : node->args) {
        std::string argTypeName = arg->getType();
        ParamTypes.push_back(cgContext.namedStruct[argTypeName]);
    }

    std::string constructorName = "new_" + node->name.lexeme;
    llvm::FunctionType* constructorType = llvm::FunctionType::get(
        llvm::PointerType::get(structType, 0),
        ParamTypes,
        false
    );
    llvm::Function* constructorFunc = llvm::Function::Create(
        constructorType,
        llvm::Function::ExternalLinkage,
        constructorName,
        cgContext.llvmModule
    );

    // 6. Construir el cuerpo del constructor.
    llvm::BasicBlock* constructorBB = llvm::BasicBlock::Create(cgContext.llvmContext, "entry", constructorFunc);
    llvm::IRBuilder<> constructorBuilder(constructorBB);

    cgContext.pushLocalScope();
    unsigned argIndex = 0;
    for (auto& arg : constructorFunc->args()) {
        arg.setName(dynamic_cast<IdentifierNode*>(node->args[argIndex])->value.lexeme);

        llvm::AllocaInst* alloca = cgContext.createEntryBlockAlloca(constructorFunc, arg.getType(), arg.getName().str());
        constructorBuilder.CreateStore(&arg, alloca);
        cgContext.addLocalVariable(arg.getName().str(), alloca);
        ++argIndex;
    }

    llvm::Value* structSize = llvm::ConstantExpr::getSizeOf(structType);
    llvm::Function* mallocFunc = cgContext.llvmModule.getFunction("malloc");  // todo:que es

    if (!mallocFunc) {
        llvm::FunctionType* mallocType = llvm::FunctionType::get(llvm::PointerType::get(llvm::Type::getInt8Ty(cgContext.llvmContext), 0),
            { llvm::Type::getInt64Ty(cgContext.llvmContext) },
            false
        );
        mallocFunc = llvm::Function::Create(mallocType, llvm::Function::ExternalLinkage, "malloc", cgContext.llvmModule);
    }
    
    llvm::Value* rawPtr = constructorBuilder.CreateCall(mallocFunc, { structSize });  //todo: que es 
    llvm::Value* typedPtr = constructorBuilder.CreateBitCast(rawPtr, llvm::PointerType::get(structType, 0));    //todo: que es

  //  llvm::AllocaInst* selfAlloca = cgContext.createEntryBlockAlloca(constructorFunc, typedPtr->getType(), "self");
  //  constructorBuilder.CreateStore(typedPtr, selfAlloca);
  //  cgContext.addLocalVariable("self", selfAlloca);

    unsigned memberIndex = 0;
    lastValue = nullptr;

    auto prevInsertPoint = cgContext.irBuilder.GetInsertBlock();
    cgContext.irBuilder.SetInsertPoint(constructorBB);

    for (AstNode* attr : node->atributes) 
    {
        auto atr=static_cast<AtributeNode*>(attr);
        
        attr->accept(*this);  
        
        llvm::Value* initValue = lastValue;
        if (!initValue) {
            std::cerr << "Error: atributo sin valor de inicialización.\n";
            initValue = llvm::ConstantFP::get(cgContext.llvmContext, llvm::APFloat(0.0));
        }

        llvm::Value* memberPtr =constructorBuilder.CreateStructGEP(structType,typedPtr,memberIndex,atr->id.lexeme);
        constructorBuilder.CreateStore(initValue,memberPtr);

       
        ++memberIndex;
    }
    cgContext.irBuilder.SetInsertPoint(prevInsertPoint);

    constructorBuilder.CreateRet(typedPtr);
     

    for (AstNode* method : node->methods) {
        if (auto methodNode = dynamic_cast<MethodNode*>(method)) {
            std::string methName = methodNode->id.lexeme;
            methodNode->id.lexeme =  methName+ "_" + node->name.lexeme   ;

           
            auto selfP=new IdentifierNode(Token("self",TokenType::Identifier,0,0));
            methodNode->params.insert(methodNode->params.begin(),selfP);
            

            methodNode->accept(*this);

        }
    }

    cgContext.popLocalScope(); 
    cgContext.namedStruct[node->name.lexeme]=structType;
    cgContext.currentType=lasttype;
    lastValue = nullptr;
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
    
    lastValue = cgContext.irBuilder.CreateLoad(varType, varAlloca, node->value.lexeme);
    return;
    
          
};


void LlvmVisitor::visit(MethodNode* node) 
{
    llvm::BasicBlock* prevBlock = cgContext.irBuilder.GetInsertBlock();

    std::vector<llvm::Type*> paramTypes;
    std::vector<std::string> paramNames;
    for (auto* paramNode : node->params) 
    {
        auto id=static_cast<IdentifierNode*>(paramNode);
        std::string typeStr = paramNode->getType();
        llvm::Type* paramType = nullptr;

        if(id->value.lexeme=="self" && !cgContext.currentType.empty())
        {
            auto selfType=cgContext.namedStruct[cgContext.currentType];
            paramType=llvm::PointerType::get(selfType,0);
        }
        
        else if(cgContext.getInteralType(typeStr))
            paramType=cgContext.getInteralType(typeStr);
        else
        {
            paramType=cgContext.namedStruct[typeStr];
        }

        paramTypes.push_back(paramType);
        paramNames.push_back(id->value.lexeme);
    }


    llvm::FunctionType* functionType = llvm::FunctionType::get(
        llvm::Type::getDoubleTy(cgContext.llvmContext), // tipo de retorno por ahora es double ,cambiar
        paramTypes,
        false
    );

    llvm::Function* function = llvm::Function::Create(
        functionType,
        llvm::Function::ExternalLinkage,
        node->id.lexeme,
        cgContext.llvmModule
    );


    // Crear bloque de entrada
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(cgContext.llvmContext, "entry", function);
    cgContext.irBuilder.SetInsertPoint(entry);


    cgContext.pushLocalScope();

    // Asignar parámetros a variables locales
    size_t idx = 0;
    for (auto& arg : function->args()) {
       
        std::string paramName = paramNames[idx];
        auto paramType = paramTypes[idx];
        arg.setName(paramName);
    
        llvm::AllocaInst* alloca = cgContext.createEntryBlockAlloca(function, paramType, paramName);
        cgContext.irBuilder.CreateStore(&arg, alloca);
        cgContext.addLocalVariable(paramName, alloca);
        
        idx++;
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

 

void LlvmVisitor::visit(FunCallNode* node) {
    // Buscar la función por su nombre
    std::string methname=node->id.lexeme;
    if(cgContext.currentType!="")
    {
        methname=methname+"_"+cgContext.currentType;
    }
    llvm::Function* calleeFunc = cgContext.llvmModule.getFunction(methname);
    
    if (!calleeFunc) {
        std::cerr << "Error: función '" << node->id.lexeme << "' no definida.\n";
        lastValue = nullptr;
        return;
    }

   
    // Evaluar argumentos
    std::vector<llvm::Value*> args;
    if(cgContext.currentType!="")
    {
        auto sel=new IdentifierNode(Token("self",TokenType::Identifier,0,0));
        node->arguments.insert(node->arguments.begin(),sel);
    }
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


void LlvmVisitor::visit(MemberCall* node) 
{
    

    node->obj->accept(*this);
    llvm::Value* objValue=lastValue;

    if (!objValue) 
      {
        std::cerr << "Error: objeto de MemberCall no válido\n";
        lastValue = nullptr;
        return;
    }
    std::string objTypeStr = node->obj->getType();
    llvm::StructType* objStruct = cgContext.namedStruct[objTypeStr];

    if (!objStruct) {
        std::cerr << "Error: tipo del objeto no encontrado: " << objTypeStr << "\n";
        lastValue = nullptr;
        return;
    }

    
    if(auto attr=dynamic_cast<IdentifierNode*>(node->member))
    {
        // auto selfAlloca=cgContext.getLocalVariable("self");
        // auto self=cgContext.irBuilder.CreateLoad(selfAlloca->getAllocatedType(),selfAlloca,"self");

        // auto selftype=cgContext.currentType;

        // auto typenode=cgContext.namedTypeNode[selftype];
        // auto selfStruct=cgContext.namedStruct[selftype];

        // int index=cgContext.GetIndexOfMember(typenode,attr->value.lexeme);
        // llvm::Value* memberPtr= cgContext.irBuilder.CreateStructGEP(selfStruct,self,index,"mem_ptr");

        // auto memberType=selfStruct->getElementType(index);
        // lastValue=cgContext.irBuilder.CreateLoad(memberType,memberPtr,"mem_val");
        // return;
        int index = cgContext.GetIndexOfMember(cgContext.namedTypeNode[objTypeStr], attr->value.lexeme);
        if (index < 0) {
            std::cerr << "Error: el atributo '" << attr->value.lexeme << "' no existe en " << objTypeStr << "\n";
            lastValue = nullptr;
            return;

        }


        llvm::Value* gep = cgContext.irBuilder.CreateStructGEP(objStruct, objValue, index, attr->value.lexeme + "_ptr");
        llvm::Type* attrType = objStruct->getElementType(index);
        
        auto temp=cgContext.namedTypeNode[objTypeStr]->atributes[index];
        auto memberTypeStr=static_cast<AtributeNode*>(temp);
        if(cgContext.getInteralType(memberTypeStr->type)==nullptr)
        {
            auto ptr=llvm::PointerType::get(attrType,0);
            lastValue = cgContext.irBuilder.CreateLoad(ptr, gep, attr->value.lexeme);
            return;
        }

        lastValue = cgContext.irBuilder.CreateLoad(attrType, gep, attr->value.lexeme);
        return;
        
        

       
    }

    
   
 //   auto objtype=cgContext.namedStruct[node->obj->getType()];
    auto methcall=dynamic_cast<FunCallNode*>(node->member);
    std::string methodName = methcall->id.lexeme+ "_" +node->obj->getType();

    
    llvm::Function* calleeFunc = cgContext.llvmModule.getFunction(methodName);
    
    if (!calleeFunc) {
        std::cerr << "Error: función '" << methodName << "' no definida.\n";
        lastValue = nullptr;
        return;
    }

    std::vector<llvm::Value*> args;
    args.push_back(objValue);


    // Evaluar argumentos
    for (AstNode* argNode : methcall->arguments) {
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
        std::cerr << "Error: número de argumentos no coincide con la función '" << methodName << "'\n";
        lastValue = nullptr;
        return;
    }

    // Generar llamada
    lastValue = cgContext.irBuilder.CreateCall(calleeFunc, args, "calltmp");
  

};

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

void LlvmVisitor::visit(TypeInstantiation* node) {
    // Construir el nombre del constructor a llamar, por ejemplo "new_Object" si el token es "Object"
    std::string constructorName = "new_" + node->typeName.lexeme;
    llvm::Function* constructorFunc = cgContext.llvmModule.getFunction(constructorName);
    if (!constructorFunc) {
        std::cerr << "Error: constructor '" << constructorName << "' no definido.\n";
        lastValue = nullptr;
        return;
    }
    
    // Evaluar los argumentos de la instanciación y recolectarlos en un vector.
    std::vector<llvm::Value*> args;
    for (AstNode* arg : node->arguments) {
        arg->accept(*this);  // Se espera que lastValue se actualice con el valor del argumento
        if (!lastValue) {
            std::cerr << "Error: argumento inválido en instanciación.\n";
            lastValue = nullptr;
            return;
        }
        args.push_back(lastValue);
    }
    
    // Generar la llamada al constructor. Se obtiene un pointer al objeto instanciado.
    auto obj = cgContext.irBuilder.CreateCall(constructorFunc, args, "calltmp");
    
    // El valor resultante se guarda para continuar la generación del código.
    lastValue = obj;
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

void LlvmVisitor::visit(AtributeNode* node) {
    if (node->expression) {
        node->expression->accept(*this);  // Esto debería dejar el resultado en `lastValue`
    } else {
        // Default value si no hay inicializador explícito
        llvm::Type* ty = cgContext.getInteralType(node->getType());
        if (ty->isDoubleTy()) {
            lastValue = llvm::ConstantFP::get(ty, 0.0);
        } else if (ty->isIntegerTy()) {
            lastValue = llvm::ConstantInt::get(ty, 0);
        } else {
            lastValue = llvm::Constant::getNullValue(ty);
        }
    }
}

void LlvmVisitor::visit(UnaryExpression* node)       {};
void LlvmVisitor::visit(ForExression* node)          {};

