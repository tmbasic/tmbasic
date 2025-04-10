#include "compileGlobals.h"
#include "CompilerException.h"
#include "parse.h"
#include "tokenize.h"
#include "typeCheck.h"
#include "shared/cast.h"
#include "shared/strings.h"

namespace compiler {

static boost::local_shared_ptr<TypeNode> getTypeForLiteralToken(const Token& token) {
    Kind kind = {};
    switch (token.type) {
        case TokenKind::kTrue:
        case TokenKind::kFalse:
            kind = Kind::kBoolean;
            break;
        case TokenKind::kNumberLiteral:
            kind = Kind::kNumber;
            break;
        case TokenKind::kStringLiteral:
            kind = Kind::kString;
            break;
        default:
            return nullptr;
    }

    return boost::make_local_shared<TypeNode>(kind, token);
}

static std::string getConstString(const LiteralStringExpressionNode& node) {
    return node.value;
}

static std::string getConstObject(const ConstValueExpressionNode& node) {
    switch (node.getConstValueExpressionType()) {
        case ConstValueExpressionType::kString:
            return getConstString(dynamic_cast<const LiteralStringExpressionNode&>(node));

        default:
            throw std::runtime_error("Invalid const value expression type.");
    }
}

static decimal::Decimal getConstValue(const ConstValueExpressionNode& node) {
    switch (node.getConstValueExpressionType()) {
        case ConstValueExpressionType::kBoolean: {
            return { dynamic_cast<const LiteralBooleanExpressionNode&>(node).value ? 1 : 0 };
        }

        case ConstValueExpressionType::kNumber:
            return dynamic_cast<const LiteralNumberExpressionNode&>(node).value;

        default:
            throw std::runtime_error("Invalid const value expression type.");
    }
}

static void compileGlobal(const SourceMember& sourceMember, CompiledProgram* compiledProgram) {
    auto lowercaseIdentifier = shared::to_lower_copy(sourceMember.identifier);
    CompiledGlobalVariable* compiledGlobalVariable = nullptr;

    // see if the compiled global already exists
    for (auto& g : compiledProgram->globalVariables) {
        if (g->lowercaseName == lowercaseIdentifier) {
            throw CompilerException(
                CompilerErrorCode::kDuplicateSymbolName,
                fmt::format("The global variable name \"{}\" already exists.", sourceMember.identifier), {});
        }
    }

    // if not then create a new one
    auto g = std::make_unique<CompiledGlobalVariable>();
    g->lowercaseName = lowercaseIdentifier;
    compiledGlobalVariable = g.get();
    compiledProgram->globalVariables.push_back(std::move(g));

    auto tokens = tokenize(sourceMember.source + "\n", TokenizeType::kCompile, &sourceMember);
    auto parserResult = parse(&sourceMember, ParserRootProduction::kMember, tokens);
    if (!parserResult.isSuccess) {
        throw CompilerException(CompilerErrorCode::kSyntax, parserResult.message, *parserResult.token);
    }

    // figure out the type of the variable. it must be a literal initializer.
    if (parserResult.node->getMemberType() == MemberType::kConstStatement) {
        auto& constNode = dynamic_cast<ConstStatementNode&>(*parserResult.node);
        parserResult.node->evaluatedType = getTypeForLiteralToken(constNode.value->token);
    } else if (parserResult.node->getMemberType() == MemberType::kDimStatement) {
        const auto* dimNode = shared::dynamic_cast_borrow<DimStatementNode>(parserResult.node);
        assert(dimNode->value || dimNode->type);
        if (dimNode->value) {
            if (dimNode->value->getExpressionType() != ExpressionType::kConstValue) {
                throw CompilerException(
                    CompilerErrorCode::kInvalidGlobalVariableType,
                    "Global variable initial values must be a Boolean, Number, or String literal.",
                    dimNode->value->token);
            }
            const auto* constValueNode = shared::dynamic_cast_borrow<ConstValueExpressionNode>(dimNode->value);
            parserResult.node->evaluatedType = getTypeForLiteralToken(constValueNode->token);
            if (parserResult.node->evaluatedType == nullptr) {
                throw CompilerException(
                    CompilerErrorCode::kInvalidGlobalVariableType,
                    "Global variable initial values must be a Boolean, Number, or String literal.",
                    dimNode->value->token);
            }
        } else {
            parserResult.node->evaluatedType = dimNode->type;
        }
    } else {
        throw CompilerException(
            CompilerErrorCode::kWrongMemberType,
            "This member must be a global variable (dim) or constant value (const).", tokens[0]);
    }

    compiledGlobalVariable->isValue = parserResult.node->evaluatedType->isValueType();
    decimal::Decimal initialValue{ 0 };
    std::pair<shared::ObjectType, std::string> initialObject{ shared::ObjectType::kString, "" };

    ExpressionNode* valueExpr = nullptr;
    if (parserResult.node->getMemberType() == MemberType::kDimStatement) {
        auto& dimNode = dynamic_cast<DimStatementNode&>(*parserResult.node);
        valueExpr = dimNode.value.get();
    } else if (parserResult.node->getMemberType() == MemberType::kConstStatement) {
        auto& constNode = dynamic_cast<ConstStatementNode&>(*parserResult.node);
        valueExpr = constNode.value.get();
    }

    if (valueExpr != nullptr) {
        // This is the form "dim x = 5" or "const x = 5"
        if (valueExpr->getExpressionType() != ExpressionType::kConstValue) {
            throw CompilerException(
                CompilerErrorCode::kInvalidGlobalVariableType,
                "The initial value of a global variable must be a boolean, number, or string literal.",
                valueExpr->token);
        }
        auto& constValueExpr = dynamic_cast<ConstValueExpressionNode&>(*valueExpr);
        if (compiledGlobalVariable->isValue) {
            initialValue = getConstValue(constValueExpr);
        } else {
            initialObject = { shared::ObjectType::kString, getConstObject(constValueExpr) };
        }
    }

    if (compiledGlobalVariable->isValue) {
        compiledGlobalVariable->index = static_cast<int>(compiledProgram->vmGlobalValues.size());
        compiledProgram->vmGlobalValues.push_back(initialValue);
        parserResult.node->globalValueIndex = compiledGlobalVariable->index;
    } else {
        compiledGlobalVariable->index = static_cast<int>(compiledProgram->vmGlobalObjects.size());
        compiledProgram->vmGlobalObjects.push_back(std::move(initialObject));
        parserResult.node->globalObjectIndex = compiledGlobalVariable->index;
    }

    compiledGlobalVariable->dimOrConstStatementNode = std::move(parserResult.node);
}

void compileGlobals(const SourceProgram& sourceProgram, CompiledProgram* compiledProgram) {
    for (const auto& member : sourceProgram.members) {
        if (member->memberType == SourceMemberType::kGlobal) {
            compileGlobal(*member, compiledProgram);
        }
    }
}

}  // namespace compiler
