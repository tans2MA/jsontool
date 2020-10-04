#ifndef CAPP_H__
#define CAPP_H__

#include "CAppCLI.h"

#include "JsonConfig.h"
#include "rapidjson/schema.h"

#include "CSchemaProvider.h"

class CApp
{
public:
	int Run(int argc, char** argv);

	CApp() : m_pSchema(nullptr), m_pSchemaProvider(nullptr), m_pSchemaValidator(nullptr) {}
	~CApp() { ClearSchema(); }

private:
	int ReadPrimaryJson();
	int ReadSecondaryJson();
	int WritePrimaryJson();

	const char* PrimaryJsonFile()
	{
		return cli.jsonFile.empty() ? "STDIN" : cli.jsonFile.c_str();
	}

	int RunCheckFormat();
	int RunPrintJson();

	int RunDiff();
	int CompareJsonDir();
	int CompareJsonFile();
	int CompareJsonNode();
	int CompareJsonOper(const rapidjson::Value& a, const rapidjson::Value& b);

	// spile file to two parts: filename.json#/root/path/to/node
	static void SplitJsonFile(std::string& jsonFile, std::string& jsonPath);

	int BuildSchema();
	void ClearSchema();
	void ResetSchemaValidator();
	bool HasSchemaValidator() { return m_pSchemaValidator != nullptr; }
	int ValidatorJson(const rapidjson::Value& doc);
private:
	CAppCLI cli;

	rapidjson::Document json;
	rapidjson::Document json2;

	// schema related object
	rapidjson::SchemaDocument* m_pSchema;
	rapidjson::SchemaValidator* m_pSchemaValidator;
	CSchemaProvider* m_pSchemaProvider;
};

#endif /* end of include guard: CAPP_H__ */
