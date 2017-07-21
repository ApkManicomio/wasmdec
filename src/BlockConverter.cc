#ifndef _CONVER_H
#include "Conversion.h"
#endif
using namespace std;
using namespace wasm;

string wdis::Convert::parseExpr(Module* mod, Expression* ex, int depth) {
	string ret("");
	if (ex->is<Unary>()) {
		// TODO
	} else if (ex->is<Block>()) {
		// Recursively parse blocks
		Block* blck = ex->cast<Block>();
		ret += getBlockBody(mod, blck, depth);
	} else if (ex->is<Binary>()) {
		// Binary operations, including conditionals and arithmetic
		Binary* spex = ex->cast<Binary>();
		string e1 = parseExpr(mod, spex->left, depth);
		string e2 = parseExpr(mod, spex->right, depth);
		string operation = getBinOperator(spex->op);
		ret += e1;
		ret += " ";
		ret += operation;
		ret += " ";
		ret += e2;
	} else if (ex->is<GetLocal>()) {
		// Convert WASM local variable to C local variable
		GetLocal* spex = ex->cast<GetLocal>();
		ret += getLocal(spex->index);
	} else if (ex->is<Return>()) {
		Return* spex = ex->cast<Return>();
		if (spex->value) {
			// Insert expression as function return value
			ret += util::tab(depth);
			ret += "return ";
			ret += parseExpr(mod, spex->value, depth);
			ret += ";\n";
		} else {
			ret += "return;\n"; // For void functions
		}
	} else if (ex->is<If>()) {
		If* ife = ex->cast<If>();
		string cond = parseExpr(mod, ife->condition, depth);
		string trueBlock = parseExpr(mod, ife->ifTrue, depth);
		ret += util::tab(depth);
		ret += "if (";
		ret += cond;
		ret += ") {\n";
		depth++;
		ret += util::tab(depth);
		ret += trueBlock;
		depth--;
		ret += "\n";
		ret += util::tab(depth);
		ret += "} ";
		if (ife->ifFalse) {
			// Insert else block
			string falseBlock = parseExpr(mod, ife->ifFalse, depth);
			ret += "else {\n";
			depth++;
			ret += util::tab(depth);
			ret += falseBlock;
			ret += "\n";
			depth--;
			ret += util::tab(depth);
			ret += "}";
		} else {
			// No else statement
			ret += "// <No else block>\n";
		}
	} else if (ex->is<Const>()) {
		Const* cex = ex->cast<Const>();
		Literal val = cex->value;
		// Resolve constant's type
		string stype = resolveType(val.type);
		string sval;
		// All the possible types the constant could be
		int32_t conv_i32;
		int64_t conv_i64;
		float conv_f32;
		double conv_f64;
		switch (val.type) {
			// Convert constant literal type to string
			case WasmType::none:
			case WasmType::unreachable:
				sval = "0";
				break;
			case WasmType::i32:
				conv_i32 = val.geti32();
				sval = to_string(conv_i32);
				break;
			case WasmType::i64:
				conv_i64 = val.geti64();
				sval = to_string(conv_i64);
				break;
			case WasmType::f32:
				conv_f32 = val.getf32();
				sval = to_string(conv_f32);
				break;
			case WasmType::f64:
				conv_f64 = val.getf64();
				sval = to_string(conv_f64);
				break;
		}
		ret += sval;
	} else if (ex->is<Nop>()) {
		ret = "// <Nop expression>\n"; // Nop expressions do nothing
	} else if (ex->is<GetGlobal>()) {
		// Global variable lookup
		ret += ex->cast<GetGlobal>()->name.str;
		// ret += "\n";
	} else if (ex->is<SetGlobal>()) {
		// Set global variable
		SetGlobal* gex = ex->cast<SetGlobal>();
		ret += util::tab(depth);
		ret += gex->name.str;
		ret += " = ";
		// The value is an expression
		ret += parseExpr(mod, gex->value, depth);
		ret += ";\n";
	} else if (ex->is<Break>()) {
		Break* br = ex->cast<Break>();
		ret += util::tab(depth);
		if (br->condition) {
			// Conditional breaking
			ret += "if (";
			ret += parseExpr(mod, br->condition, depth);
			ret += ") break;";
		} else {
			// Literal breaking
			ret = "break;";
		}
		ret += "\n";
	}
	cout << "Parsed expr to '" << ret << "' ";
	return ret;
}
string wdis::Convert::getBlockBody(Module* mod, Block* blck, int depth) {
	// Stream all block expressions and components into a string
	stringstream s;
	depth++;
	for (auto& expr : blck->list) {
		s << parseExpr(mod, expr, depth);
	}
	depth--;
	return s.str();
}
string wdis::Convert::getFuncBody(Module* mod, Function* fn) {
	string fnBody;
	fnBody += " {\n";
	// Function bodies are block expressions
	fnBody += parseExpr(mod, fn->body, -1);
	fnBody += "}";
	return fnBody;
}