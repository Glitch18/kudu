#pragma once

#include <liblangutil/SourceLocation.h>
#include <libyul/AST.h>

#include <boost/variant.hpp>
#include <optional>
#include <vector>

using namespace std;
using namespace solidity;

class YulVisitor : public boost::static_visitor<void> {
   public:
    enum class PassType {
        CaseRemoval,
        Constructor,
        AddrCleanerRemoval,
        Skip,
    };

    YulVisitor(string src, string warpConstructorSelector,
               string warpConstructorName);

    template <class T>
    void visitVectorOfVariants(const vector<T>& _vec);
    void printSrcAtLocation(langutil::SourceLocation loc);
    void writeModifiedYul();
    void refreshYulAST();

    void operator()(const yul::Block& _node);
    void operator()(const yul::TypedName& _node);
    void operator()(const yul::Literal& _node);
    void operator()(const yul::Identifier& _node);
    void operator()(const yul::Assignment& _node);
    void operator()(const yul::VariableDeclaration& _node);
    void operator()(const yul::FunctionDefinition& _node);
    void operator()(const yul::FunctionCall& _node);
    void operator()(const yul::If& _node);
    void operator()(const yul::Switch& _node);
    void operator()(const yul::Case& _node);
    void operator()(const yul::ForLoop& _node);
    void operator()(const yul::Break& _node);
    void operator()(const yul::Continue& _node);
    void operator()(const yul::Leave& _node);
    void operator()(const yul::ExpressionStatement& _node);

    bool m_inEntryFunction;
    bool m_dynamicArgsInConstructor;
    PassType m_currentPass;
    string m_src;
    const string dynamicEntrySeq = R"(
                let _1 := 128
                let _2 := 64
                mstore(_2, _1)
                let _3 := 4
                let _4 := calldatasize()
				let _7 := 0
				let _8 := calldataload(_7)
				let _9 := 224
				let _10 := shr(_9, _8) )";
    string m_srcModified;
    optional<string> m_deletedCase;
    string m_currentFunction;
    string m_currentDeclr = "";
    int m_currDeclrLocation[2] = {0, 0};
    int m_prevDeclrLocation[2];
    string m_prevDeclr;
    string m_constructorSelector;
    string m_constructorName;
};
