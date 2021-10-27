#pragma once

#include <libsolidity/ast/AST.h>
#include <libsolidity/ast/ASTVisitor.h>
#include <libsolidity/interface/OptimiserSettings.h>
#include <libsolidity/parsing/Parser.h>
#include <solc/CommandLineInterface.h>

#include <string>

#include "common/library.hpp"
#include "yul_prepass/YulCleaner.hpp"

using namespace solidity::frontend;
using namespace solidity;

struct MarkedFunctions
{
	std::vector<std::string>			  names;
	std::vector<std::string>			  selectors;
	std::vector<std::vector<Type const*>> parameters;
};


class SourceData: public ASTConstVisitor
{
public:
	enum class PassType
	{
		AddrTypePass,
		ConstructorPass,
		FunctionCallPass,
		FunctionDefinitionPass,
		StorageVarPass,
		StorageFunctionPass,
	};

	SourceData(std::string main_contract,
			   std::string src,
			   std::string filepath,
			   bool		   print_ir);

	CommandLineInterface	  getCli(char const* sol_filepath);
	FunctionDefinition const* resolveFunctionCall(const ContractDefinition& c,
												  FunctionCall const&		f);
	FunctionDefinition const*
		 insideWhichFunction(langutil::SourceLocation const& location);
	void getInheritedConstructorCalls(
		std::vector<ASTPointer<ModifierInvocation>> const& modifiers);

	std::string getSaferYul();
	void		removeComments();
	bool		visit(FunctionDefinition const& _node) override;
	bool		visit(FunctionCall const& _node) override;
	bool		visit(VariableDeclaration const& _node) override;
	bool		visit(Identifier const& _node) override;
	bool		visitNode(ASTNode const& node) override;
	void		prepareSoliditySource(const char* sol_filepath);
	void		setCompilerOptions(std::shared_ptr<CompilerStack> compiler);
	void		writeModifiedSolidity();
	void		dynFuncArgsPass(const char* solFilepath);
	void		constrcutorPass(const char* solFilepath);
	void		addressPass();
	void		addressTypePass();
	void		setYulOptimizerSettings();
	void		functionCallPass();
	void		refreshStateAfterModification();
	void		generateWarpConstructor();

	std::vector<std::pair<std::string, std::string>> m_addrMarkedFuncs;
	std::vector<std::string>						 m_srcSplit;
	std::vector<std::string>						 m_srcSplitOriginal;
	boost::filesystem::path							 m_baseFileName;

	bool					 m_willGenerateConstructor = false;
	std::string				 m_warpConstructor;
	std::string				 m_warpConstructorName;
	std::string				 m_warpConstructorSig;
	std::string				 m_warpConstructorSelector;
	std::string				 m_mainContract;
	std::string				 m_currentFunction;
	std::string				 m_modifiedContractName;
	std::string				 m_currentFunctionModified;
	std::vector<std::string> m_currentFunctionParams;
	std::vector<std::string> m_contractNames;
	std::vector<std::string> m_interfaceNames;
	std::map<std::string, std::vector<std::string>> m_interfaces;
	std::string										m_filepath;
	std::string										m_modifiedSolFilepath;
	std::string										m_srcModified;
	std::string										m_src;
	std::string										m_srcOriginal;
	bool											m_print_ir;

	std::shared_ptr<CompilerStack>			m_compiler;
	OptimiserSettings						m_compilerOptimizerSettings;
	FileReader								m_fileReader;
	std::shared_ptr<CommandLineInterface>	m_cli;
	CommandLineOptions						m_options;
	std::vector<FunctionDefinition const*>	m_definedFunctions;
	std::vector<std::string>				m_storageVars_str;
	std::vector<const VariableDeclaration*> m_storageVars_astNodes;
	PassType								m_currentPass;

private:
	bool isPublic(Visibility _visibility);
	bool hasDynamicArgs(std::string params);
	bool checkTypeEqaulity(std::vector<Type const*> const& t1,
						   std::vector<Type const*>		   t2);

	std::vector<std::string> m_storageVars;
	std::vector<std::string> m_hashNames;
	MarkedFunctions			 m_dynArgFunctions;
};
