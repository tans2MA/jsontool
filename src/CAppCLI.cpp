#include "CAppCLI.h"
#include "CLI11.hpp"
#include <stdlib.h>

void CAppCLI::Reset()
{
	checkMode = false;
	diffMode = false;
	printInPretty = false;
	printInLine = false;
	outFile.clear();
	jsonFile.clear();
	jsonFile2.clear();
	jsonDir.clear();
	jsonDir2.clear();
	jsonSchema.clear();
	jsonPath.clear();
	jsonPath2.clear();
}

CAppCLI::CAppCLI()
{
	Reset();
}

CAppCLI::CAppCLI(int argc, char** argv)
{
	Reset();
	Parse(argc, argv);
}

int CAppCLI::Parse(int argc, char** argv)
{
	CLI::App app("Json tools");

	// simple flag with no argument
	app.add_flag("-c,--check", checkMode, "only check syntax");
	app.add_flag("-d,--diff", diffMode, "diff two json files or nodes");
	app.add_flag("-p,--pretty", printInPretty, "print in pretty format [default]");
	app.add_flag("-l,--line", printInLine, "print in condensed line");
	app.add_flag("-E,--eq", equalCompare, "compare json with equalitiy(=) [default]");
	app.add_flag("-L,--le", lessCompare, "compare json with less than(<=) second json");
	app.add_flag("-G,--ge", greatCompare, "compare json with less than(>=) second json");

	// option required argument
	app.add_option("-o,--output", outFile, "print result to output file or STDOUT if not provide");
	app.add_option("-s,--schema", jsonSchema, "the json schema file name");
	app.add_option("-f,--f1,file", jsonFile, "the json file name, will read from STDIN if not provide");
	app.add_option("-F,--f2", jsonFile2, "the second json file name to compare");
	app.add_option("-i,--i1", jsonDir, "the json directory");
	app.add_option("-I,--i2", jsonDir2, "the second json directory to compare");
	app.add_option("-t,--t1", jsonPath, "the target json path pointer");
	app.add_option("-T,--t2", jsonPath2, "the second target json path pointer to compare");
	// app.add_option("-w,--wild", wildPattern, "wild parttern to match json file in compared dir [*.json]");
	// app.add_option("--ws,--subfix", wildSubfix, "subfix for json file names in compared dir [.json]");
	// app.add_option("--wp,--prefix", wildPrefix, "prefix for json file names in compared dir");

	app.add_option("extraArgs", extraArgs, "more arguments, json file or path pointers");

	// CLI11_PARSE(app, argc, argv);
	try
	{
		app.parse(argc, argv);
	}
	catch (const CLI::ParseError& e)
	{
        if(e.get_name() == "CallForHelp") {
			app.exit(e);
			exit(0);
        }
		return app.exit(e);
	}

	return 0;
}

