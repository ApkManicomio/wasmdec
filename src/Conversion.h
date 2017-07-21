#include "wasm.h"
#include "wasm-binary.h"
#include "wasm-printing.h"
#include "wasm-s-parser.h"
#include "WasmUtils.h"
#pragma once

using namespace std;

namespace wdis {
	class Convert {
	public:
		static string getFName(wasm::Name name) {
			// Convert WASM names to C function names
			return "fn_" + string(name.str);
		}
		static string getVName(wasm::Name name) {
			// Convert standard WASM variable names to C variable names
			return "v" + string(name.str);
		}
		static string getLocal(wasm::Index argIdx) {
			// Convert WASM function locals to C variable names
			return "local_" + to_string((int)argIdx);
		}
		static string voidCall(wasm::Function* fn) {
			// Call a void function
			return getFName(fn->name) + "();";
		}
		static string getBinOperator(BinaryOp op) { // TODO : Add more binary operations
			// Convert WASM binary operations to their respective C representation
			switch (op) {
				case AddInt32:
				case AddInt64:
				case AddFloat32:
				case AddFloat64:
					return "+";
					break;
				case SubInt32:
				case SubInt64:
				case SubFloat32:
				case SubFloat64:
					return "-";
					break;
				case XorInt64:
				case XorInt32:
					return "^";
					break;
				case OrInt64:
				case OrInt32:
					return "||";
					break;
				case MulInt32:
				case MulInt64:
				case MulFloat32:
				case MulFloat64:
					return "*";
					break;
				case EqInt32:
				case EqInt64:
				case EqFloat32:
				case EqFloat64:
					return "==";
					break;
				case NeInt32:
				case NeInt64:
				case NeFloat32:
				case NeFloat64:
					return "!=";
					break;
				case AndInt32:
				case AndInt64:
					return "&&";
					break;
				case LeSInt64:
				case LeSInt32:
					return "<=";
					break;
				case LtUInt32:
				case LtSInt32:
				case LtSInt64:
				case LtFloat32:
				case LtFloat64:
					return "<";
					break;
				case DivSInt32:
				case DivUInt32:
				case DivSInt64:
				case DivUInt64:
				case DivFloat32:
				case DivFloat64:
					return "/";
					break;
				case GtSInt64:
				case GtUInt64:
				case GtFloat64:
				case GtFloat32:
					return ">";
					break;
				case GeSInt64:
				case GeSInt32:
				case GeFloat64:
					return ">=";
			}
			return "NONE"; // Operation unimplemented or an unknown enumeration
		}
		static string resolveType(wasm::WasmType typ) {
			// Resolve wasm::WasmType to a C type
			switch (typ) {
				case wasm::WasmType::none:
				case wasm::WasmType::unreachable:
					return "void";
					break;
				case wasm::WasmType::i32:
					return "int32_t";
					break;
				case wasm::WasmType::i64:
					return "int64_t";
					break;
				case wasm::WasmType::f32:
					return "float32_t";
					break;
				case wasm::WasmType::f64:
					return "float64_t";
					break;
			}
		}
		static string getDecl(wasm::FunctionType* typ, string fname) {
			// Get a C function decloration from a FunctionType and name
			string ret = resolveType(typ->result); // Return type
			ret += " "; // Space between ret type and name
			ret += fname;
			ret += "("; // Argument list
			for (int i = 0; i < typ->params.size(); ++i) {
				ret += resolveType(typ->params.at(i));
				ret += " ";
				ret += getLocal(i);
				if (i != (typ->params.size() - 1)) {
					ret += ", ";
				}
			}
			ret += ")";
			return ret;
		}
		static string getDecl(wasm::FunctionType* typ, wasm::Name fname) {
			// Overload to support WASM names
			string ret = resolveType(typ->result); // Return type
			ret += " "; // Space between ret type and name
			ret += fname.str;
			ret += "("; // Argument list
			for (int i = 0; i < typ->params.size(); ++i) {
				ret += resolveType(typ->params.at(i));
				ret += " ";
				ret += getLocal(i);
				if (i != (typ->params.size() - 1)) {
					ret += ", ";
				}
			}
			ret += ")";
			return ret;
		}
		static string getDecl(wasm::Function* fn) {
			// Get C function decloration from WASM function
			string ret = resolveType(fn->result); // Return type
			ret += " "; // Space between ret type and name
			ret += getFName(fn->name); // name
			ret += "("; // Argument list
			for (int i = 0; i < fn->params.size(); ++i) {
				ret += resolveType(fn->params.at(i));
				ret += " ";
				ret += getLocal(i);
				if (i != (fn->params.size() - 1)) {
					// Only append comma if the argument list isn't finished
					ret += ", ";
				}
			}
			ret += ")";
			return ret;
		}
		static string getDecl(wasm::Module* m, unique_ptr<wasm::Import>& i) {
			// Convert WASM imports to their respective C declorations as C externs
			string ret = "extern ";
			if (i->kind == wasm::ExternalKind::Function) {
				// Function
				wasm::FunctionType* ft = util::resolveFType(m, i->functionType); // Lookup module types for this function's type
				ret += getDecl(ft, i->name); // Append its decloration
			} else if (i->kind == wasm::ExternalKind::Global) {
				// Global import variable
				ret += resolveType(i->globalType);
				ret += " ";
				ret += i->name.str;
				ret += ";";
			} else if (i->kind == wasm::ExternalKind::Memory) {
				// WASM memory is just a giant char array
				ret += "const char* ";
				ret += i->name.str;
				ret += "; // <Imported memory>";
			} else if (i->kind == wasm::ExternalKind::Table) {
				// TODO: More advanced table functionality
				ret += "void* (*";
				ret += i->name.str;
				ret += ")(void)";
				ret += "; // <Imported table>";
			}
			return ret;
		}
		// Declared in BlockConverter.cc
		static string parseExpr(wasm::Module*, wasm::Expression*);
		static string getBlockBody(wasm::Module*, wasm::Block*);
		static string getFuncBody(wasm::Module*, wasm::Function*);
	};
} // namespace wdis