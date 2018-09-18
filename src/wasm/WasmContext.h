#ifndef _WASM_CONTEXT_H
#define _WASM_CONTEXT_H

#include "wasm.h"
using namespace wasm;

#include "../decompiler/DecompilerCtx.h"

namespace wasmdec {
	// Context is a union between WASM functions and modules
	class Context {
	public:
		bool isGlobal;
		bool isIfCondition;
		Function* fn;
		Module* mod;
		Context(Function*, Module*, DecompilerCtx*);
		Context(Module*);
		int depth;

		int lastSetLocal;

		bool hasDecompilerCtx;
		DecompilerCtx* dctx;
		Expression* lastExpr;
		// Whether or not the expression is exactly one layer below a function
		bool functionLevelExpression;
	};
} // namespace wasmdec

#endif // _WASM_CONTEXT_H