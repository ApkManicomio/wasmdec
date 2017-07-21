#ifndef _WASM_CONTEXT_H
#define _WASM_CONTEXT_H

#include "wasm.h"
using namespace wasm;

namespace wdis {
	// Context is a union between WASM functions and modules
	class Context {
	public:
		bool isGlobal;
		Function* fn;
		Module* mod;
		Context(Function*, Module*);
		Context(Module*);
	};
} // namespace wdis

#endif // _WASM_CONTEXT_H