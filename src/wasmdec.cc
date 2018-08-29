#define __WASMDEC_VERSION "1.1.0"

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <iterator>

#include "cxxopts.hpp"
#include "decompiler/Decompiler.h"

// Global variables to be passed to the decompiler
bool debugging = false,
		extra = false,
		memdump = false;
string infile,
	outfile;
DisasmMode dmode;

// Helper functions
bool readFile(vector<char>* data, string path) {
	ifstream file(path);
	if (!file.eof() && !file.fail()) {
	    file.seekg(0, ios_base::end);
	    streampos fileSize = file.tellg();
	    data->resize(fileSize);
	    file.seekg(0, ios_base::beg);
	    file.read(&(data->operator[](0)), fileSize);
		return true;
	} else {
		return false;
	}
}
bool writeFile(string path, string data) {
	ofstream file(path);
	if (!file.eof() && !file.fail()) {
		file << data;
		file.close();
		return true;
	} else {
		return false;
	}
}
string getFileExt(string fname) {
	string::size_type idx = fname.rfind('.');
	if(idx != string::npos) {
	    string extension = fname.substr(idx + 1);
	    return extension;
	} else {
	    return "";
	}
}
DisasmMode getDisasmMode(string infile) {
	// Convert file extension to disassembler mode
	string ext = getFileExt(infile);
	if (ext == "wasm") {
		return DisasmMode::Wasm;
	} else if (ext == "wast") {
		return DisasmMode::Wast;
	} else if (ext == "js") {
		return DisasmMode::AsmJs;
	} else {
		return DisasmMode::None;
	}
}
int printVersion() {
	cerr << "wasmdec v" << __WASMDEC_VERSION << endl;
	return 0;
}
void enableDebugging() {
	debugging = true;
}
void enableExtra() {
	extra = true;
}
void enableMemdump() {
	memdump = true;
}
void setOutfile(string _outf) {
	outfile = _outf;
}
void setInfile(string _inf) {
	infile = _inf;
}
int main(int argc, char* argv[]) {
	// Set up options
	cxxopts::Options opt("wasmdec", "WebAssembly to C decompiler");
	opt.add_options()
		("v,version", "Print wasmdec version")
		("d,debug", "Enable debug output")
		("m,memdump", "Dump memory instead of decompiling")
		("e,extra", "Output extra information to decompiled binary")
		("o,output", "Output C file", cxxopts::value<string>(outfile))
		("positional", "Input file", cxxopts::value<std::vector<std::string>>())
		("h,help", "Print usage")
		;
	opt.parse_positional({"positional"});
	auto res = opt.parse(argc, argv);
	// Help and version, boring
	if (res.count("v")) {
		// version argument passed
		return printVersion();
	}
	if (res.count("h")) {
		std::cout << opt.help({"", "Group"}) << std::endl;
		return 0;
	}
	// Fail if there's no output file
	if (!res.count("o")) {
		std::cout << "ERROR: no output file provided!" << std::endl
		<< opt.help({"", "Group"}) << std::endl;
		return 1;
	}
	// Parse decompiler flags
	if (res.count("d")) {
		enableDebugging();
	}
	if (res.count("m")) {
		enableMemdump();
	}

	if (res.count("e")) {
		enableExtra();
	}
	// Parse input file(s)
	if (res.count("positional")) {
		std::vector<std::string> infiles;
		try {
			infiles = res["positional"].as<std::vector<std::string>>();
		} catch (std::exception& e) {
			std::cout << "ERROR: invalid input files!" << std::endl
				<< opt.help({"", "Group"}) << std::endl;
			return 1;
		}
		if (infiles.size() > 1) {
			std::cout << "ERROR: wasmdec currently doesn't support more than one input file." << std::endl
				<< opt.help({"", "Group"}) << std::endl;
			return 1;
		}
		infile = infiles.at(0);
	} else {
		std::cout << "ERROR: no input file provided!" << std::endl
			<< opt.help({"", "Group"}) << std::endl;
		return 1;
	}
	// Configure the decompiler
	dmode = getDisasmMode(infile);
	DisasmConfig conf(debugging, extra, dmode);
	std::vector<char> input;
	if (!readFile(&input, infile)) {
		std::cout << "ERROR: failed to read the input file!" << std::endl;
		return 0;
	}

	// Now that everything is parsed, initialize the decompiler
	Decompiler decompiler(conf, infile);
	decompiler.decompile();
	if (decompiler.failed()) {
		std::cout << "ERROR: failed to decompile the binary." << std::endl;
		return 0;
	}
	string decompiledCode = decompiler.getEmittedCode();

	return 0;
}