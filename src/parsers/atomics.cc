#include "parser.h"
using namespace wasmdec;

string wasmdec::parsers::binary(Context* ctx, Expression* ex) {
	return "/* Atomic operation unsupported */\n";
}