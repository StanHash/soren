#ifndef SOREN_AST_STMT_INCLUDED
#define SOREN_AST_STMT_INCLUDED

#include <vector>
#include <memory>

#include "ast/expr.h"

namespace soren {

struct Stmt;

using Ast = std::vector<Stmt>;

struct Stmt
{
	enum class Kind
	{
		Invalid,

		Push, // One child expr
		Expr, // One child expr
		Goto, // One child constant expr
		GotoIf, // Two child exprs (first constant)
		Yield, // No children
		Return, // One child expr
	};

	Kind kind { Kind::Invalid };

	std::string label; //< TODO: better

	std::vector<std::unique_ptr<Expr>> children;
	std::unique_ptr<Ast> childAst;

	static inline
	Stmt make_push(std::unique_ptr<Expr>&& inner)
	{
		Stmt result { Kind::Push, {}, {} };

		result.children.push_back(std::move(inner));

		return result;
	}

	static inline
	Stmt make_goto(std::int32_t target)
	{
		Stmt result { Kind::Goto, {}, {} };

		// FIXME: use (or "register") actual label names instead of generating some on the fly

		result.children.push_back(Expr::make_unique_identifier([&] ()
		{
			std::string name;
			name.append("label_");
			name.append(std::to_string(target));
			return name;
		} ()));

		return result;
	}

	static inline
	Stmt make_goto_if(std::int32_t target, std::unique_ptr<Expr>&& truth)
	{
		Stmt result { Kind::GotoIf, {}, {} };

		// FIXME: do not copy-paste this and reuse same code as in make_goto

		result.children.push_back(Expr::make_unique_identifier([&] ()
		{
			std::string name;
			name.append("label_");
			name.append(std::to_string(target));
			return name;
		} ()));

		result.children.push_back(std::move(truth));

		return result;
	}

	static inline
	Stmt make_yield(void)
	{
		return Stmt { Kind::Yield, {}, {} };
	}

	static inline
	Stmt make_return(std::unique_ptr<Expr>&& inner)
	{
		Stmt result { Kind::Return, {}, {} };

		result.children.push_back(std::move(inner));

		return result;
	}
};

} // namespace soren

#endif // SOREN_AST_STMT_INCLUDED
