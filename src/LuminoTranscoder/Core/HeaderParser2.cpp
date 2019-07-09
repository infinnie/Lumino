﻿/*
	clang -Xclang -ast-dump Test.cpp
*/
#include <memory>
#include <LuminoEngine/Base/Regex.hpp>
#include "HeaderParser2.hpp"

#ifdef _MSC_VER     // start of disabling MSVC warnings
#pragma warning(push)
#pragma warning(disable:4146 4180 4244 4258 4267 4291 4345 4351 4355 4456 4457 4458 4459 4503 4624 4722 4800 4996)
#endif
#include <iostream>
#include "clang/AST/AST.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/DeclVisitor.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/MacroArgs.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

#include "clang/AST/TypeLoc.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/AST/Comment.h"
#include "clang/AST/CommentVisitor.h"

#ifdef _MSC_VER     // end of disabling MSVC warnings
#pragma warning(pop)
#endif


class SymbolDatabase;

namespace local
{
using namespace clang;
using namespace clang::tooling;
using namespace llvm;
using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;
using namespace clang::comments;

class LWIGVisitor : public DeclVisitor<LWIGVisitor, bool>
{
private:
	CompilerInstance * m_ci;
	const SourceManager& m_sm;
	::HeaderParser2* m_parser;
	//Ref<DocumentSymbol> m_lastDocument;
	PITypeInfo* m_currentRecord;

public:
	LWIGVisitor(CompilerInstance* CI, ::HeaderParser2* parser)
		: m_ci(CI)
		, m_sm(CI->getASTContext().getSourceManager())
		, m_parser(parser)
	{
	}

	// OK:
	// class [[deprecated("please use new_func() function"]] Engine
	// NG: (そもそも取れない)
	// class [[ln_loca_attr(a=10, b="")]] Engine
	std::vector<std::string> getAnnotation(Decl* decl)
	{
		std::vector<std::string> attrs;
		//if (decl->hasAttrs())
		{
			//AttributeDeclKind::
			//AnnotateAttr::getKind
			//clang::attr::Kind::
			//decl->attrs
			for (auto& attr : decl->attrs())
			{
				auto kind = attr->getKind();

				auto str = getSourceText(attr->getRange());

				auto* anno = dyn_cast<AnnotateAttr>(attr);
				if (anno != nullptr)
				{
					attrs.push_back(anno->getAnnotation().str());
				}
			}
		}
		return attrs;
	}

	static unsigned getOffsetOnRootFile(const SourceManager& sm, SourceLocation loc)
	{
		if (sm.getFileID(loc) == sm.getMainFileID())
		//auto ploc = sm.getPresumedLoc(loc);
		//if (ploc.getIncludeLoc().isInvalid())
		{
			auto eloc = sm.getDecomposedExpansionLoc(loc);
			return eloc.second;
		}
		return 0;
	}


	std::string getSourceText(SourceRange range) const
	{
		auto stringRef = Lexer::getSourceText(
			CharSourceRange::getTokenRange(range),
			m_ci->getSourceManager(),
			m_ci->getLangOpts());
		return stringRef.str();
	}

	ln::String getLocString(SourceLocation loc)
	{
		return ln::String::fromStdString(loc.printToString(m_ci->getSourceManager()));
	}
	ln::String getLocString(Decl* decl)
	{
		return getLocString(decl->getLocation());
	}

	Ref<PIDocument> parseDocument(Decl* decl)
	{
		if (const FullComment *Comment = decl->getASTContext().getLocalCommentForDeclUncached(decl))
			return parseDocument2(Comment);
			//return HeaderParser2::parseDocument(getSourceText(Comment->getSourceRange()));
		return ln::makeRef<PIDocument>();
	}

	ln::String tlanslateAccessLevel(AccessSpecifier ac)
	{
		switch (ac)
		{
		case clang::AS_public:
			return u"Public";
		case clang::AS_protected:
			return u"Protected";
		case clang::AS_private:
			return u"Private";
		default:
			LN_UNREACHABLE();
			return u"Private";
		}
	}

	//std::string getQualTypeNameSymple(QualType* type)
	//{
	//	type->getAsString()
	//	type->dump()
	//	PrintingPolicy PrintPolicy;

	//	SplitQualType T_split = type->split();
	//	std::string name = QualType::getAsString(T_split/*, PrintPolicy*/);

	//	if (Desugar && !T.isNull()) {
	//		// If the type is sugared, also dump a (shallow) desugared type.
	//		SplitQualType D_split = T.getSplitDesugaredType();
	//		if (T_split != D_split)
	//			OS << ":'" << QualType::getAsString(D_split, PrintPolicy) << "'";
	//	}
	//}

	ln::String getRawTypeFullName(const QualType& type)
	{
		// type.getAsString() だと完全週修飾名になる。"struct ln::Vector3" など。
		auto name = ln::String::fromStdString(type.getAsString());
		name = name.replace(_T("struct"), "");
		name = name.replace(_T("class"), "");
		name = name.replace(_T("enum"), "");
		name = name.replace(_T("const"), "");
		name = name.replace(_T("*"), "");
		name = name.replace(_T("&"), "");

		if (name == _T("_Bool"))
		{
			return _T("bool");
		}

		return name.trim();
		//SplitQualType st = type.split();
		//if (st.Ty->isRecordType())
		//{
		//	// type.getAsString() だと完全週修飾名になる。"struct ln::Vector3" など。
		//	// Decl から定義名をとると、"Vector3" などが取れる。
		//	CXXRecordDecl* rd = st.Ty->getAsCXXRecordDecl();
		//	DeclarationName name = rd->getDeclName();
		//	return ln::String::fromStdString(name.getAsString());
		//}
		//else
		//{
		//	return ln::String::fromStdString(type.getAsString());
		//}
	}

	void EnumerateDecl(DeclContext* aDeclContext)
	{
		for (DeclContext::decl_iterator i = aDeclContext->decls_begin(), e = aDeclContext->decls_end(); i != e; i++)
		{
			Decl *D = *i;
			//if (indentation.IndentLevel == 0) {
			//	errs() << "TopLevel : " << D->getDeclKindName();                                    // Declの型表示
			//	if (NamedDecl *N = dyn_cast<NamedDecl>(D))  errs() << " " << N->getNameAsString();  // NamedDeclなら名前表示
			//	errs() << " (" << D->getLocation().printToString(SM) << ")\n";                      // ソース上の場所表示
			//}

			//errs() << "TopLevel : " << D->getDeclKindName();
			//if (NamedDecl *N = dyn_cast<NamedDecl>(D))  errs() << " " << N->getNameAsString();
			//errs() << "\n";

			Visit(D);
		}
	}


	// class/struct/unionの処理
	bool VisitCXXRecordDecl(CXXRecordDecl* decl)
	{
		if (!decl->isCompleteDefinition())
		{
			// 宣言
			return true;
		}

		if (unsigned offset = getOffsetOnRootFile(m_sm, decl->getLocation()))
		{
			auto* attr = m_parser->findUnlinkedAttrMacro(offset);
			if (attr)
			{
				attr->linked = true;

				auto info = ln::makeRef<PITypeInfo>();
				info->rawFullName = getRawTypeFullName(QualType(decl->getTypeForDecl(), 0));

				// documentation
				info->document = parseDocument(decl);

				// metadata
				info->metadata = HeaderParser2::parseMetadata(attr->name, attr->args);

				getAnnotation(decl);

				// base classes
				auto bases = decl->getDefinition()->bases();
				for (auto& base : bases)
				{
					info->baseClassRawName = getRawTypeFullName(base.getType());
					break;	// first only
				}

				if (decl->getDefinition()->isStruct()) {
					info->kind = u"Struct";
				}
				else {
					info->kind = u"Class";
				}
				m_parser->getDB()->types.add(info);

				m_currentRecord = info;
				EnumerateDecl(decl);
				m_currentRecord = nullptr;
			}
		}

		return true;
	}

	// メンバ関数
	bool VisitCXXMethodDecl(CXXMethodDecl* decl)
	{
		if (unsigned offset = getOffsetOnRootFile(m_sm, decl->getLocation()))	// 入力ファイルの内側で定義されているか？
		{
			//ln::String localSigneture;
			//{
			//	for (unsigned int iParam = 0; iParam < decl->getNumParams(); iParam++) {
			//		ParmVarDecl* paramDecl = decl->getParamDecl(iParam);
			//		QualType& type = paramDecl->getType();
			//		auto typeString = ln::String::fromStdString(type.getAsString());
			//		if (!localSigneture.isEmpty()) localSigneture += u",";
			//		localSigneture += typeString;
			//	}
			//	localSigneture = ln::String::fromStdString(decl->getNameAsString()) + u"(" + localSigneture + u")";
			//}

			auto* attr = m_parser->findUnlinkedAttrMacro(offset);
			if (attr)
			{
				attr->linked = true;

				auto info = ln::makeRef<PIMethod>();
				//info->owner = m_currentRecord;
				info->name = ln::String::fromStdString(decl->getNameAsString());
				info->accessLevel = tlanslateAccessLevel(decl->getAccess());

				//auto declText3 = getSourceText(SourceRange(decl->getInnerLocStart(), decl->getSourceRange().getEnd()));
				//auto declText2 = getSourceText(SourceRange(decl->getQualifierLoc().getBeginLoc(), decl->getSourceRange().getEnd()));
				//auto declText1 = getSourceText(SourceRange(decl->getSourceRange().getBegin(), decl->getSourceRange().getEnd()));
				//auto declText4 = getSourceText(SourceRange(, decl->getSourceRange().getEnd()));
				//auto declText = getSourceText(SourceRange(decl->getReturnTypeSourceRange().getEnd(), decl->getSourceRange().getEnd()));
				//info->localSignature = PIDatabase::parseLocalSigneture(ln::String::fromStdString(declText), info->name);

				// documentation
				info->document = parseDocument(decl);

				// metadata
				info->metadata = HeaderParser2::parseMetadata(attr->name, attr->args);

				// return type
				info->returnTypeRawName = getRawTypeFullName(decl->getReturnType());

				// qualifiers
				info->isVirtual = decl->isVirtual();
				info->isStatic = decl->isStatic();
				info->isConst = decl->isConst();
				info->isConstructor = (decl->getDeclKind() == Decl::CXXConstructor);

				// check sema error (未定義の型など)
				if (decl->isInvalidDecl())
				{
					// TODO: add location
					//m_parser->diag()->reportError(ln::String::format(_T("Invalid declaration {0}"), ln::String::fromStdString(getSourceText(decl->getSourceRange()))), getLocString(decl));
					m_parser->diag()->reportError(ln::String::format(u"Invalid declaration {0}", ln::String::fromStdString(getSourceText(decl->getSourceRange()))));
				}

				m_currentRecord->methods.add(info);

				for (unsigned int iParam = 0; iParam < decl->getNumParams(); iParam++)
				{
					ParmVarDecl* paramDecl = decl->getParamDecl(iParam);
					QualType& type = paramDecl->getType();

					SplitQualType sp = type.split();

					
					auto paramInfo = ln::makeRef<PIMethodParameter>();
					paramInfo->name = ln::String::fromStdString(paramDecl->getNameAsString());
					paramInfo->typeRawName = getRawTypeFullName(type);

					bool hasConst = type.getQualifiers().hasConst();
					if (hasConst) {
						paramInfo->isConst = true;
					}

					if (sp.Ty->isPointerType()) {
						paramInfo->isPointer = true;
						//paramInfo->isIn = hasConst;
						//paramInfo->isOut = !hasConst;
					}
					else {
						//paramInfo->isIn = true;
						//paramInfo->isOut = false;
					}

					info->parameters.add(paramInfo);

					// check sema error
					if (paramDecl->isInvalidDecl()) {
						// TODO: add location
						//m_parser->diag()->reportError(ln::String::format(_T("Invalid declaration {0}"), ln::String::fromStdString(getSourceText(paramDecl->getSourceRange()))), getLocString(paramDecl));
						m_parser->diag()->reportError(ln::String::format(u"Invalid declaration {0}", ln::String::fromStdString(getSourceText(paramDecl->getSourceRange()))));
					}
				}
			}
			else
			{
				auto name = ln::String::fromStdString(decl->getNameAsString());
				auto document = parseDocument(decl);
				if (!document->summary.isEmpty()) {
					auto declText = getSourceText(decl->getSourceRange());
					document->copydocLocalSignature = PIDatabase::parseLocalSigneture(ln::String::fromStdString(declText), name);

					m_parser->getDB()->relativeDocuments.add(document);
				}
			}
		}

		return true;
	}

	// メンバ変数
	bool VisitFieldDecl(FieldDecl* decl)
	{
		if (unsigned offset = getOffsetOnRootFile(m_sm, decl->getLocation()))
		{
			if (auto* attr = m_parser->findUnlinkedAttrMacro(offset))
			{
				attr->linked = true;

				auto info = ln::makeRef<PIField>();
				info->name = ln::String::fromStdString(decl->getNameAsString());
				info->document = parseDocument(decl);
				info->typeRawName = getRawTypeFullName(decl->getType());
				m_currentRecord->fields.add(info);
			}
		}

		return true;
	}

	bool VisitEnumDecl(EnumDecl* decl)
	{
		if (unsigned offset = getOffsetOnRootFile(m_sm, decl->getLocation()))
		{
			if (auto* attr = m_parser->findUnlinkedAttrMacro(offset))
			{
				attr->linked = true;

				auto symbol = ln::makeRef<PITypeInfo>();
				symbol->rawFullName = ln::String::fromStdString(decl->getQualifiedNameAsString());
				symbol->kind = u"Enum";

				// documentation
				symbol->document = parseDocument(decl);

				// metadata
				symbol->metadata = HeaderParser2::parseMetadata(attr->name, attr->args);

				m_parser->getDB()->types.add(symbol);

				m_currentRecord = symbol;
				EnumerateDecl(decl);
				m_currentRecord = nullptr;
			}
		}
		return true;
	}

	bool VisitEnumConstantDecl(EnumConstantDecl* decl)
	{
		if (m_currentRecord)
		{
			auto symbol = ln::makeRef<PIConstant>();
			symbol->document = parseDocument(decl);
			symbol->name = ln::String::fromStdString(decl->getNameAsString());
			symbol->value = ln::makeVariant(decl->getInitVal().getSExtValue());
			//symbol->type = m_currentRecord;
			m_currentRecord->constants.add(symbol);
		}

		return true;
	}

	// typedef
	bool VisitTypedefDecl(TypedefDecl *aTypedefDecl)
	{
		return true;
	}

	// namespace
	bool VisitNamespaceDecl(NamespaceDecl* decl)
	{
		EnumerateDecl(decl);
		return true;
	}

	// using
	bool VisitUsingDirectiveDecl(UsingDirectiveDecl *aUsingDirectiveDecl)
	{
		return true;
	}

	Ref<PIDocument> parseDocument2(const FullComment* fullComment)
	{
		auto doc = ln::makeRef<PIDocument>();

		for (auto& block : fullComment->getBlocks()) {
			
			switch (block->getCommentKind())
			{
			case Comment::ParagraphCommentKind:
			{
				auto text = ln::String::fromStdString(getSourceText(block->getSourceRange()), ln::TextEncoding::utf8Encoding());
				if (int index = text.indexOf(u"@copydoc") >= 0) {
					doc->copydocLocalSignature = PIDatabase::parseLocalSigneture(ln::String(text.substr(index + 8)).trim());
				}
				else
				{
					auto fc = PIDocument::formatComment(text);
					if (!fc.isEmpty()) {
						if (doc->summary.isEmpty()) {
							doc->summary = PIDocument::formatComment(text);
						}
						else {
							if (!doc->details.isEmpty()) doc->details += u"\n";
							doc->details += fc;
						}
					}
				}
				break;
			}
			case Comment::ParamCommandCommentKind:	// @param
			{
				auto pcc = static_cast<ParamCommandComment*>(block);
				auto param = ln::makeRef<PIParamDocument>();
				param->name = ln::String::fromStdString(pcc->getArgText(0).str()).trim();

				param->io = ln::String::fromStdString(pcc->getDirectionAsString(pcc->getDirection())).remove(u"[").remove(u"]");

				param->description = PIDocument::formatComment(ln::String::fromStdString(getSourceText(pcc->getParagraph()->getSourceRange()), ln::TextEncoding::utf8Encoding()));

				// ↓全体を解析したいときはこれを使う
				//ln::String::fromStdString(getSourceText(block->getSourceRange()), ln::TextEncoding::utf8Encoding());

				doc->params.add(param);

				break;
			}
			case Comment::BlockCommandCommentKind:	// @return, etc
			{
				auto bcc = static_cast<BlockCommandComment*>(block);
				if (bcc->getCommandID() == CommandTraits::KCI_return) {
					doc->returns = PIDocument::formatComment(ln::String::fromStdString(getSourceText(bcc->getParagraph()->getSourceRange()), ln::TextEncoding::utf8Encoding()));
				}
				else if (bcc->getCommandID() == CommandTraits::KCI_brief) {
				}
				else if (bcc->getCommandID() == CommandTraits::KCI_details) {
				}
				break;
			}
			default:
				break;
			}
		}

		return doc;
	}
};

// このインタフェースは、プリプロセッサの動作を観察する方法を提供します。
// https://clang.llvm.org/doxygen/classclang_1_1PPCallbacks.html
// #XXXX を見つけたときや、マクロ展開が行われたときに呼ばれるコールバックを定義したりする。
// ★C++のクラスの属性構文は、class キーワードと名前の間に書く。Lumino のは class の前に書くスタイルなので、
//   clang と属性構文の機能を使うことができない。そのためマクロを自分で解析する。
class LocalPPCallbacks : public PPCallbacks
{
public:
	LocalPPCallbacks(Preprocessor& pp, CompilerInstance* ci, ::HeaderParser2* parser)
		: m_pp(pp)
		, m_ci(ci)
		, m_parser(parser)
	{
	}

	virtual void MacroExpands(const Token& MacroNameTok, const MacroDefinition& MD, SourceRange range, const MacroArgs* args) override
	{
		const SourceManager& sm = m_ci->getSourceManager();
		const LangOptions& opts = m_ci->getLangOpts();
		const MacroInfo* macroInfo = MD.getMacroInfo();

		//clang::FileEntry
		//sm.getOrCreateFileID

		// マクロが書かれている場所は input のルートであるか？ (include ファイルは解析したくない)
		//auto ploc = sm.getPresumedLoc(range.getBegin());
		//if (ploc.getIncludeLoc().isInvalid())
		if (sm.getFileID(range.getBegin()) == sm.getMainFileID())
		{
			std::string name = Lexer::getSourceText(CharSourceRange::getTokenRange(range.getBegin()), sm, opts).str();
			if (name == "LN_CLASS" ||
				name == "LN_STRUCT" ||
				name == "LN_FIELD" ||
				name == "LN_METHOD" ||
				name == "LN_ENUM")
			{
				::HeaderParser2::AttrMacro attrMacro;
				attrMacro.name = name;

				std::string args = Lexer::getSourceText(CharSourceRange::getTokenRange(range), sm, opts).str();
				args = args.substr(args.find('(') + 1);
				args = args.substr(0, args.find(')'));
				attrMacro.args = args;

				//
				//for (int iArg = 0; iArg < args->getNumArguments(); iArg++)
				//{
				//	auto a = args->getPreExpArgument(iArg, macroInfo, m_pp);
				//	//const Token* tok = args->getPreExpArgument(iArg);
				//	//if (tok->is(tok::eof))
				//	//{
				//	//	break;
				//	//}

				//	attrMacro.args.push_back(Lexer::getSourceText(CharSourceRange::getTokenRange(tok->getLocation()), sm, opts));
				//}

				//int a2 = args->getArgLength();

				//for (int iArg = 0; i < )

				//std::string tokens = 

				//for (int iArg = 0U; iArg < macroInfo->getNumArgs(); iArg++)
				//{
				//	const IdentifierInfo* param = *(macroInfo->arg_begin() + iArg);	// params がほしいときはこっち
				//	const Token* argToks = args->getUnexpArgument(iArg);				// args がほしいときはこっち

				//	if (argToks->is(tok::eof))
				//	{
				//		// マクロを使っている側の実引数の数が少ない。
				//		// あるいは #define AAA(...) のような定義で、実引数が省略されている。
				//	}

				//	attrMacro.args.push_back(Lexer::getSourceText(CharSourceRange::getTokenRange(argToks->getLocation()), sm, opts));
				//}

				auto eloc = sm.getDecomposedExpansionLoc(range.getBegin());
				attrMacro.offset = eloc.second;
				m_parser->lnAttrMacros.push_back(attrMacro);
			}
		}
	}

private:
	Preprocessor & m_pp;
	CompilerInstance* m_ci;
	::HeaderParser2* m_parser;
};

//------------------------------------------------------------------------------
// 以下、決まり文句

// ASTConsumer は、AST のエントリポイントとなる何らかの要素を見つけたときにそれを通知する。
// 通常は HandleTranslationUnit() だけ実装すればよい。
// https://clang.llvm.org/docs/RAVFrontendAction.html
// clang は色々な AST の作成方法を持っているらしく、HandleTranslationUnit() 以外は
// 通常の翻訳単位以外の解析で何かしたいときに使うようだ。
class LocalASTConsumer : public ASTConsumer
{
private:
	std::unique_ptr<LWIGVisitor> m_visitor;

public:
	explicit LocalASTConsumer(CompilerInstance* CI, ::HeaderParser2* parser)
		: m_visitor(std::make_unique<LWIGVisitor>(CI, parser))
	{
		//auto fe = CI->getSourceManager().getFileManager().getFile(, false);
		//CI->getSourceManager().file

		Preprocessor &PP = CI->getPreprocessor();
		PP.addPPCallbacks(llvm::make_unique<LocalPPCallbacks>(PP, CI, parser));
	}

	virtual void HandleTranslationUnit(ASTContext& Context)
	{
		m_visitor->EnumerateDecl(Context.getTranslationUnitDecl());
	}
};

// FrontendAction は、コンパイラフロントエンドの共通のエントリポイント。具体的に何したいの？を表すために使う。
// https://clang.llvm.org/docs/RAVFrontendAction.html
// https://clang.llvm.org/doxygen/classclang_1_1ASTFrontendAction.html
// 標準だと ASTFrontendAction の派生として、単に AST をダンプしたり、HTML に変換したりといったアクションが用意されている。
// 今回は AST を全部自分でトラバースしたいので ASTFrontendAction を使う。
class LocalFrontendAction : public ASTFrontendAction
{
public:
	::HeaderParser2* m_parser;

	LocalFrontendAction(::HeaderParser2* parser)
		: m_parser(parser)
	{
	}

	virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, llvm::StringRef file)
	{
		return llvm::make_unique<LocalASTConsumer>(&CI, m_parser);
	}
};


// SilClangAnalyzer のポインタを ↑のクラスたちにわたすためのファクトリ
std::unique_ptr<FrontendActionFactory> NewLocalFrontendActionFactory(::HeaderParser2* parser)
{
	class SimpleFrontendActionFactory : public FrontendActionFactory
	{
	public:
		::HeaderParser2* m_parser;

		SimpleFrontendActionFactory(::HeaderParser2* parser)
			: m_parser(parser)
		{}

		clang::FrontendAction *create() override { return new LocalFrontendAction(m_parser); }
	};

	return std::unique_ptr<FrontendActionFactory>(new SimpleFrontendActionFactory(parser));
}

} // namespace

//------------------------------------------------------------------------------

int HeaderParser2::parse(const ln::Path& filePath, PIDatabase* db, ln::DiagnosticsManager* diag)
{
	LN_CHECK(db);
	m_db = db;
	m_diag = diag;

	//Path tempFilePath(filePath.getString() + _T(".cpp"));
	//FileSystem::copyFile(filePath, tempFilePath, true);



	// TODO: Path から直接 toLocalPath
	std::string localFilePath = filePath.str().toStdString();

	std::vector<std::string> args;
	args.push_back("");	// program name
	args.push_back(localFilePath.c_str());
	args.push_back("--");
	
	args.push_back("-x");
	args.push_back("c++");

	//args.push_back("-target");
	//args.push_back("unknown-unknown-unknown-msvc");
	

	for (auto& path : m_includePathes)
	{
		args.push_back("-I");
		args.push_back(path.str().toStdString());
	}

	for (auto& path : m_forceIncludeFiles)
	{
		args.push_back("-include");
		args.push_back(path.str().toStdString());
	}

	

	
	//args.push_back("-include");
	//args.push_back(LUMINO_ROOT_DIR"/Source/LuminoEngine/Source/LuminoEngine.PCH.h");

	
	args.push_back("-Xclang");


	args.push_back("-fsyntax-only");
	args.push_back("-fms-compatibility");		// Enable full Microsoft Visual C++ compatibility
	args.push_back("-fms-extensions");			// Enable full Microsoft Visual C++ compatibility
	args.push_back("-fmsc-version=1910");		// Microsoft compiler version number to report in _MSC_VER (0 = don't define it (default))

	// 基本的にヘッダを入力するので、warning: #pragma once in main file を隠す。https://clang.llvm.org/docs/UsersManual.html#options-to-control-error-and-warning-messages
	args.push_back("-Wno-pragma-once-outside-header");

	//const char* argv[] =
	//{
	//	"",	
	//	localFilePath.c_str(),
	//	"--",
	//	//"-I", "C:/Proj/LN/Lumino/Source/LuminoCore/Include",
	//	//"-I", "C:/Proj/LN/Lumino/Source/LuminoEngine/Include",
	//	"-D", "LN_NAMESPACE_BEGIN",
	//	"-D", "LN_NAMESPACE_END",
	//	"-fms-compatibility",
	//	"-fms-extensions",
	//	"-fmsc-version=1900",
	//};
	std::vector<const char*> argv;
	for (auto& arg : args)
	{
		argv.push_back(arg.c_str());
	}
	int argc = argv.size();

	std::vector<std::string> sourcePathList = { localFilePath.c_str() };

	::clang::tooling::CommonOptionsParser op(argc, argv.data(), ::llvm::cl::GeneralCategory);
	::clang::tooling::ClangTool Tool(op.getCompilations(), sourcePathList/*op.getSourcePathList()*/);
	int result = Tool.run(local::NewLocalFrontendActionFactory(this).get());

	//FileSystem::deleteFile(tempFilePath);
	return result;
}

HeaderParser2::AttrMacro* HeaderParser2::findUnlinkedAttrMacro(unsigned offset)
{
	for (size_t i = 0; i < lnAttrMacros.size(); i++)
	{
		if (!lnAttrMacros[i].linked)
		{
			unsigned o1 = lnAttrMacros[i].offset;
			unsigned o2 = (i < lnAttrMacros.size() - 1) ? lnAttrMacros[i + 1].offset : UINT32_MAX;
			if (o1 <= offset && offset < o2)
			{
				return &lnAttrMacros[i];
			}
		}
	}
	return nullptr;
}

Ref<PIDocument> HeaderParser2::parseDocument(const std::string& comment)
{
	auto info = ln::makeRef<PIDocument>();


	ln::String doc = ln::String::fromStdString(comment, ln::TextEncoding::utf8Encoding());

	// 改行コード統一し、コメント開始終了を削除する
	doc = doc
		.replace(_T("\r\n"), _T("\n"))
		.replace(_T("\r"), _T("\n"))
		.replace(_T("/**"), _T(""))
		.replace(_T("*/"), _T(""));


	ln::List<ln::String> lines = doc.split(_T("\n"));
	ln::String* target = &info->summary;
	for (ln::String line : lines)
	{
		line = line.trim();

		// 行頭が * であれば取り除く
		if (line.indexOf(u"*") == 0) {
			line = line.substr(1);
		}

		ln::MatchResult result;
		if (ln::Regex::search(line, _T("@(\\w+)"), &result))
		{
			auto section = result.groupValue(1);
			if (section == _T("brief"))
			{
				target = &info->summary;
				line = line.substr(result.length());
			}
			else if (section == _T("param"))
			{
				ln::String con = line.substr(result.length());
				if (ln::Regex::search(con, _T(R"(\[(\w+)\]\s+(\w+)\s*\:\s*)"), &result))
				{
					auto paramInfo = ln::makeRef<PIParamDocument>();
					info->params.add(paramInfo);
					paramInfo->io = result.groupValue(1);
					paramInfo->name = result.groupValue(2);
					target = &paramInfo->description;
					line = con.substr(result.length());
				}
			}
			else if (section == _T("return"))
			{
				target = &info->returns;
				line = line.substr(result.length());
			}
			else if (section == _T("details"))
			{
				target = &info->details;
				line = line.substr(result.length());
			}
			else if (section == _T("copydoc"))
			{
				ln::String con = line.substr(result.length());
				if (ln::Regex::search(con, _T(R"((\w+)(.*))"), &result))
				{
					////info->copydocMethodName = result.groupValue(1);
					////info->copydocSignature = result.groupValue(2);
					//info->localSignature = result.groupValue(1);
					//info->localSignature += ln::String(result.groupValue(2));
					//info->localSignature = info->localSignature.remove(' ').remove('\t');
					info->copydocLocalSignature = PIDatabase::parseLocalSigneture(con);
					target = &info->details;
					line.clear();
				}
			}
		}

		(*target) += line.trim();
	}

	return info;
}

Ref<PIMetadata> HeaderParser2::parseMetadata(std::string name, const std::string& args)
{
	auto metadata = ln::makeRef<PIMetadata>();
	metadata->name = ln::String::fromStdString(name);
	auto argEntries = ln::String::fromStdString(args).split(_T(","), ln::StringSplitOptions::RemoveEmptyEntries);
	for (auto& arg : argEntries)
	{
		ln::String key, value;
		auto tokens = arg.split(_T("="), ln::StringSplitOptions::RemoveEmptyEntries);
		key = tokens[0].trim();
		if (tokens.size() >= 2)
		{
			value = tokens[1].trim()
				.replace(_T("'"), _T(""))
				.replace(_T("\""), _T(""));
		}
		metadata->values.insert({ ln::String(key), ln::String(value) });
	}
	return metadata;
}

