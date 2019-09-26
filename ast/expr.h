#ifndef SOREN_AST_EXPR_INCLUDED
#define SOREN_AST_EXPR_INCLUDED

#include <vector>
#include <memory>

namespace soren {

struct Expr
{
	enum class Kind
	{
		Invalid,

		// No children
		IntLiteral,
		StrLiteral,
		Named,

		// One child (unary operators)
		Neg,
		Not,
		BitwiseNot,
		Deref, // value-at-address ([])
		Addrof, // address-of-value (&)

		// Two child (binary operators)
		Assign,
		Add,
		Sub,
		Mul,
		Div,
		Mod,
		Or,
		And,
		Xor,
		Lsl,
		Lsr,
		Eq,
		Ne,
		Lt, // Maybe
		Le,
		Gt, // Maybe
		Ge, // Maybe
		EqStr,
		NeStr,
		LogicalAnd,
		LogicalOr,

		Func, // Name + Variable Children
	};

	Kind kind { Kind::Invalid };

	// TODO: embed expression source location? (either bytecode offset or file:line:col)
	// TODO (C++17): use std::variant

	// Literal
	std::int32_t literal {};

	// Named/String/FnName
	std::string named;

	std::vector<std::unique_ptr<Expr>> children;

	static inline
	std::unique_ptr<Expr> make_unique_intlit(std::int32_t value)
	{
		std::unique_ptr<Expr> result = std::make_unique<Expr>();

		result->kind = Kind::IntLiteral;
		result->literal = value;

		return result;
	}

	static inline
	std::unique_ptr<Expr> make_unique_strlit(std::string&& value)
	{
		std::unique_ptr<Expr> result = std::make_unique<Expr>();

		result->kind = Kind::StrLiteral;
		result->named = std::move(value);

		return result;
	}

	static inline
	std::unique_ptr<Expr> make_unique_identifier(std::string&& value)
	{
		std::unique_ptr<Expr> result = std::make_unique<Expr>();

		result->kind = Kind::Named;
		result->named = std::move(value);

		return result;
	}

	static inline
	std::unique_ptr<Expr> make_unique_unop(Kind kind, std::unique_ptr<Expr>&& inner)
	{
		std::unique_ptr<Expr> result = std::make_unique<Expr>();

		result->kind = kind;
		result->children.push_back(std::move(inner));

		return result;
	}

	static inline
	std::unique_ptr<Expr> make_unique_binop(Kind kind, std::unique_ptr<Expr>&& lexpr, std::unique_ptr<Expr>&& rexpr)
	{
		std::unique_ptr<Expr> result = std::make_unique<Expr>();

		result->kind = kind;
		result->children.push_back(std::move(lexpr));
		result->children.push_back(std::move(rexpr));

		return result;
	}

	static inline
	std::unique_ptr<Expr> make_unique_copy(const Expr& expr)
	{
		std::unique_ptr<Expr> result = std::make_unique<Expr>();

		result->kind = expr.kind;
		result->literal = expr.literal;
		result->named = expr.named;

		for (auto& child : expr.children)
			result->children.push_back(make_unique_copy(*child));

		return result;
	}
};

} // namespace soren

#endif // SOREN_AST_EXPR_INCLUDED
