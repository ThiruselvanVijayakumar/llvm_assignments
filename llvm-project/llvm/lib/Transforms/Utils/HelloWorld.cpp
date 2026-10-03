
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Transforms/Utils/HelloWorld.h"

using namespace llvm;

static cl::opt<std::string> HelloOpt(
    "hello-opt",
    cl::desc("Select HelloWorld optimization"),
    cl::init("all"));

namespace{


//constant folding
static bool ConstantPropagation(Function &F){
    bool Changed = false;
    for (BasicBlock &BB : F){
        for (auto It = BB.begin(); It != BB.end();){
            Instruction *I = &*It++;
            auto *BO = dyn_cast<BinaryOperator>(I);
            if (!BO)
                continue;
            auto *C1 = dyn_cast<Constant>(BO->getOperand(0));
            auto *C2 = dyn_cast<Constant>(BO->getOperand(1));
            if (!C1 || !C2)
                continue;
            Constant *Result = ConstantExpr::get(BO->getOpcode(), C1, C2);
            if (Result){
                BO->replaceAllUsesWith(Result);
                BO->eraseFromParent();
                Changed = true;
            }
        }
    }
    return Changed;
}


//instruction combining
static bool InstructionCombining(Function &F) {
    bool Changed = false;
    for (BasicBlock &BB : F) {
        for (auto It = BB.begin(); It != BB.end(); ) {
            Instruction *I = &*It++;
            auto *BO = dyn_cast<BinaryOperator>(I);
            if (!BO)
                continue;
            Value *LHS = BO->getOperand(0);
            Value *RHS = BO->getOperand(1);
            //Constant folding.
            if (auto *C1 = dyn_cast<Constant>(LHS)) {
                if (auto *C2 = dyn_cast<Constant>(RHS)) {
                    Constant *Result =
                        ConstantExpr::get(BO->getOpcode(), C1, C2);
                    if (Result) {
                        BO->replaceAllUsesWith(Result);
                        BO->eraseFromParent();
                        Changed = true;
                        continue;
                    }
                }
            }
            if (auto *C = dyn_cast<ConstantInt>(RHS)) {
                if (C->isZero() &&
                    (BO->getOpcode() == Instruction::Add || BO->getOpcode() == Instruction::Sub)) {
                    BO->replaceAllUsesWith(LHS);
                    BO->eraseFromParent();
                    Changed = true;
                    continue;
                }
                if (C->isOne() && BO->getOpcode() == Instruction::Mul) {
                    BO->replaceAllUsesWith(LHS);
                    BO->eraseFromParent();
                    Changed = true;
                    continue;
                }
                if (C->equalsInt(2) && BO->getOpcode() == Instruction::Mul) {
                    IRBuilder<> Builder(BO);
                    Value *ShiftAmount = ConstantInt::get(BO->getType(), 1);
                    Value *NewValue =Builder.CreateShl(LHS, ShiftAmount, "mul_to_shift");
                    BO->replaceAllUsesWith(NewValue);
                    BO->eraseFromParent();
                    Changed = true;
                    continue;
                }
            }
        }
    }
    return Changed;
}


//dead code elimination
static bool DeadCodeElimination(Function &F){
    bool Changed = false;

    //repeat because removing one dead instruction can make an earlier instruction dead as well.
    bool LocalChange = true;
    while (LocalChange){
        LocalChange = false;
        for (BasicBlock &BB : F){
            for (auto It = BB.begin(); It != BB.end(); ) {
                Instruction *I = &*It++;
                if (I->use_empty() && !I->isTerminator() && !I->mayHaveSideEffects()){
                    I->eraseFromParent();
                    Changed = true;
                    LocalChange = true;
                }
            }
        }
    }
    return Changed;
}


//strenght reduction
static bool StrengthReduction(Function &F) {
    bool Changed = false;

    for (BasicBlock &BB : F){
        for (Instruction &I : BB){
            auto *BO = dyn_cast<BinaryOperator>(&I);
            if (!BO)
                continue;
            if (BO->getOpcode() != Instruction::Mul)
                continue;
            ConstantInt *C = dyn_cast<ConstantInt>(BO->getOperand(1));
            if (!C)
                continue;
            uint64_t Factor = C->getZExtValue();
            if (Factor == 0 || (Factor & (Factor - 1)) != 0)
                continue;
            unsigned Shift = llvm::Log2_64(Factor);
            IRBuilder<> Builder(BO);
            Value *ShiftAmount = ConstantInt::get(BO->getType(), Shift);
            Value *NewValue = Builder.CreateShl(BO->getOperand(0), ShiftAmount, "strength_reduced");
            BO->replaceAllUsesWith(NewValue);
            BO->eraseFromParent();
            Changed = true;
            break;
        }
    }
    return Changed;
}

//Common subexpression elimination
static bool CommonSubexpressionElimination(Function &F) {
    bool Changed = false;
    for (BasicBlock &BB : F) {
        SmallVector<Instruction *, 32> Instructions;
        for (Instruction &I : BB)
            Instructions.push_back(&I);
        for (size_t i = 0; i < Instructions.size(); ++i) {
            Instruction *I = Instructions[i];
            if (!isa<BinaryOperator>(I))
                continue;
            for (size_t j = 0; j < i; ++j) {
                Instruction *Previous = Instructions[j];
                if (!isa<BinaryOperator>(Previous))
                    continue;
                if (I->isIdenticalTo(Previous)) {
                    I->replaceAllUsesWith(Previous);
                    I->eraseFromParent();
                    Changed = true;
                    break;
                }
            }
        }
    }
    return Changed;
}

} // namespace


PreservedAnalyses HelloWorldPass::run(
    Function &F,
    FunctionAnalysisManager &AM) {
    bool Changed = false;

    if (HelloOpt == "constprop") {
        Changed = ConstantPropagation(F);
    }
    else if (HelloOpt == "instcombine") {
        Changed = InstructionCombining(F);
    }
    else if (HelloOpt == "dce") {
        Changed = DeadCodeElimination(F);
    }
    else if (HelloOpt == "strengthreduction") {
        Changed = StrengthReduction(F);
    }
    else if (HelloOpt == "cse") {
        Changed = CommonSubexpressionElimination(F);
    }
    else if (HelloOpt == "all") {
        Changed |= ConstantPropagation(F);
        Changed |= InstructionCombining(F);
        Changed |= DeadCodeElimination(F);
        Changed |= StrengthReduction(F);
        Changed |= CommonSubexpressionElimination(F);
    }
    else {
        report_fatal_error(
            "Unknown -hello-opt value. "
            "Use: constprop, instcombine, dce, strengthreduction, cse, or all.");
    }

    return Changed
        ? PreservedAnalyses::none()
        : PreservedAnalyses::all();
}
