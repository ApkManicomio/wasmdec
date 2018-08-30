#ifndef _MULTI_DECOMP_H
#define _MULTI_DECOMP_H

#include "Decompiler.h"
using namespace std;

namespace wasmdec {
	class MultiDecompiler {
	public:
		MultiDecompiler(vector<string>, DisasmConfig);
		string getOutput(void);
		bool failed;
	protected:
		stringstream codeStream;
		bool readFile(vector<char>*, string);
		string getFileExt(string);
		string getEverythingButFileExt(string);
		DisasmMode getDisasmMode(string);

		vector<string> infiles;
	};
};

#endif // _MULTI_DECOMP_H