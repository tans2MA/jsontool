#ifndef CAPPCLI_H__
#define CAPPCLI_H__

#include <string>
#include <vector>

class CApp;

class CAppCLI
{
public:
	CAppCLI();
	CAppCLI(int argc, char** argv);

private:
	void Reset();
	int Parse(int argc, char** argv);

private:
	friend CApp;

	bool checkMode;
	bool diffMode;
	bool printInPretty;
	bool printInLine;
	bool equalCompare;
	bool lessCompare;
	bool greatCompare;

	std::string outFile;
	std::string jsonSchema;
	std::string jsonFile;
	std::string jsonFile2;
	std::string jsonDir;
	std::string jsonDir2;
	std::string jsonPath;
	std::string jsonPath2;

	std::string wildPattern;
	std::string wildSubfix;
	std::string wildPrefix;

	std::vector<std::string> extraArgs;
};

#endif /* end of include guard: CAPPCLI_H__ */
