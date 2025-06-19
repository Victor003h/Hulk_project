#include "LlvmVisitor.hpp"
#include "llvm/IR/IRBuilder.h"


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

    for (AstNode* attr : node->atributes) 
    {
        auto atr=static_cast<AtributeNode*>(attr);
        llvm::Value* memberPtr = constructorBuilder.CreateStructGEP(structType, typedPtr, memberIndex);
        attr->accept(*this);  
        
        llvm::Value* initValue = lastValue;
        if (!initValue) {
            std::cerr << "Error: atributo sin valor de inicialización.\n";
            initValue = llvm::ConstantFP::get(cgContext.llvmContext, llvm::APFloat(0.0));
        }


        llvm::AllocaInst* varAlloca =constructorBuilder.CreateAlloca(initValue->getType(),nullptr,atr->id.lexeme);
        constructorBuilder.CreateStore(initValue,varAlloca);
        cgContext.addLocalVariable(atr->id.lexeme,varAlloca);

        ++memberIndex;
    }

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
    auto id=dynamic_cast<IdentifierNode*>(node->obj);
    if(id &&id->value.lexeme=="self"  &&  !cgContext.currentType.empty())
    {
        
        if(auto attr=dynamic_cast<IdentifierNode*>(node->member))
        {
            auto selfAlloca=cgContext.getLocalVariable("self");
            auto self=cgContext.irBuilder.CreateLoad(selfAlloca->getAllocatedType(),selfAlloca,"self");

            auto selftype=cgContext.currentType;

            auto typenode=cgContext.namedTypeNode[selftype];
            auto selfStruct=cgContext.namedStruct[selftype];

            int index=cgContext.GetIndexOfMember(typenode,attr->value.lexeme);
            llvm::Value* memberPtr= cgContext.irBuilder.CreateStructGEP(selfStruct,self,index,"mem_ptr");

            auto memberType=selfStruct->getElementType(index);
            lastValue=cgContext.irBuilder.CreateLoad(memberType,memberPtr,"mem_val");
            return;
        }

    }

    node->obj->accept(*this);
    llvm::Value* obj_value=lastValue;
    auto objtype=cgContext.namedStruct[node->obj->getType()];
    auto meth=dynamic_cast<FunCallNode*>(node->member);
    std::string methodName = meth->id.lexeme+ "_" +node->obj->getType();

    
    llvm::Function* calleeFunc = cgContext.llvmModule.getFunction(methodName);
    
    if (!calleeFunc) {
        std::cerr << "Error: función '" << methodName << "' no definida.\n";
        lastValue = nullptr;
        return;
    }

    std::vector<llvm::Value*> args;
    args.push_back(obj_value);


    // Evaluar argumentos
    for (AstNode* argNode : meth->arguments) {
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
    llvm::Value* obj = cgContext.irBuilder.CreateCall(constructorFunc, args, "instCall");
    
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
