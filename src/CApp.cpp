#include "CApp.h"

#include <string.h>
#include <stdio.h>
#include <fstream> 
//#include <glob.h>

#include "rapidjson/stringbuffer.h"

#include "JsonUtil.h"
#include "CommUtil.h"

int CApp::Run(int argc, char** argv)
{
	if (cli.Parse(argc, argv) != 0)
	{
		return -1;
	}

	if (cli.diffMode)
	{
		return RunDiff();
	}
	else if (cli.checkMode)
	{
		return RunCheckFormat();
	}
	else if (cli.printInPretty || cli.printInLine)
	{
		return RunPrintJson();
	}
	else
	{
		fprintf(stderr, "No action, requeire one of flags in [cdpl], more run with -h\n");
		return -1;
	}

	return 0;
}

int CApp::ReadPrimaryJson()
{
	SplitJsonFile(cli.jsonFile, cli.jsonPath);
	const std::string& file = cli.jsonFile;
	int iRet = 0;
	if (file.empty())
	{
		iRet = ReadJson(json, std::cin);
	}
	else
	{
		iRet = ReadJsonFile(json, file);
	}
	if (iRet != 0)
	{
		fprintf(stderr, "Fail to read primary json file: [%s]\n", PrimaryJsonFile());
	}
	return iRet;
}

int CApp::ReadSecondaryJson()
{
	SplitJsonFile(cli.jsonFile2, cli.jsonPath2);
	std::string file = cli.jsonFile2;
	int iRet = 0;
	if (file.empty())
	{
		fprintf(stderr, "Empty secondary file name to compare to?\n");
		iRet = -1;
	}
	else
	{
		iRet = ReadJsonFile(json2, file);
		if (iRet != 0)
		{
			fprintf(stderr, "Fail to read secondary json file: [%s]\n", file.c_str());
		}
	}

	return iRet;
}

int CApp::WritePrimaryJson()
{
	const rapidjson::Value* pJson = GetJsonNode(json, cli.jsonPath);
	if (nullptr == pJson)
	{
		fprintf(stderr, "Can not resolve the json path pointer: %s#%s\n", PrimaryJsonFile(), cli.jsonPath.c_str());
		return -1;
	}

	bool pretty = cli.printInPretty || !cli.printInLine;
	if (cli.outFile.empty())
	{
		return WriteJson(*pJson, std::cout, pretty);
	}
	else
	{
		std::ofstream ofs(cli.outFile);
		int iRet = 0;
		if (ofs.is_open())
		{
			iRet = WriteJson(*pJson, ofs, pretty);
		}
		else
		{
			fprintf(stderr, "Fail to write file: %s\n", cli.outFile.c_str());
			iRet = -1;
		}
		ofs.close();
		return iRet;
	}
}

int CApp::RunCheckFormat()
{
	if (!cli.jsonSchema.empty())
	{
		if (0 != BuildSchema())
		{
			return -1;
		}
	}

	// move `file` and following extra arguments together
	std::vector<std::string> files = std::move(cli.extraArgs);
	files.insert(files.begin(), std::move(cli.jsonFile));

	int iRet = 0;
	for (auto& file : files)
	{
		cli.jsonFile = std::move(file);
		if (ReadPrimaryJson() != 0)
		{
			fprintf(stderr, "[%s] is NOT valid json\n", PrimaryJsonFile());
			--iRet;
			continue;
		}
		else
		{
			fprintf(stderr, "[%s] is valid json\n", PrimaryJsonFile());
		}
		if (HasSchemaValidator())
		{
			if (ValidatorJson(json) != 0)
			{
				fprintf(stderr, "[FAIL] json [%s] can NOT validate against schema[%s]!\n", PrimaryJsonFile(), cli.jsonSchema.c_str());
				--iRet;
				continue;
			}
			else
			{
				fprintf(stderr, "[PASS] json [%s] can validate against schema[%s].\n", PrimaryJsonFile(), cli.jsonSchema.c_str());
			}
			ResetSchemaValidator();
		}
	}

	return iRet;
}

int CApp::RunPrintJson()
{
	if (ReadPrimaryJson() != 0)
	{
		return -1;
	}

	size_t nExtraFiles = cli.extraArgs.size();
	if (nExtraFiles <= 0)
	{
		return WritePrimaryJson();
	}

	std::vector<std::string> paths = std::move(cli.extraArgs);
	if (!cli.jsonPath.empty())
	{
		paths.insert(paths.begin(), std::move(cli.jsonPath));
	}

	int iRet = 0;
	for (auto& path : paths)
	{
		cli.jsonPath = std::move(path);
		std::string head = PrimaryJsonFile();
		if (!cli.jsonPath.empty())
		{
			head.append("#").append(cli.jsonPath);
		}
		fprintf(stderr, "%s : \n", head.c_str());
		if (WritePrimaryJson() != 0)
		{
			--iRet;
		}
		fprintf(stderr, "\n");
	}

	return 0;
}

int CApp::RunDiff()
{
	if (!cli.jsonDir2.empty())
	{
		return CompareJsonDir();
	}
	else if (!cli.jsonFile2.empty()) // jsonFile can read from stdin
	{
		return CompareJsonFile();
	}
	else if (!cli.jsonPath.empty() && !cli.jsonPath2.empty())
	{
		return CompareJsonNode();
	}
	else if (cli.extraArgs.size() == 1) // exe -d file file2
	{
		cli.jsonFile2 = cli.extraArgs[0];
		return CompareJsonFile();
	}
	else if (cli.extraArgs.size() == 2) // exe -d file path1 pathe2
	{
		cli.jsonPath = cli.extraArgs[0];
		cli.jsonPath2 = cli.extraArgs[1];
		return CompareJsonNode();
	}
	else
	{
		fprintf(stderr, "Compare what, arguments loss, please run with -h\n");
		return -1;
	}

	return 0;
}

int CApp::CompareJsonDir()
{
#if 0
	std::string wild;
	if (cli.wildPattern.empty() && cli.wildSubfix.empty() && cli.wildPrefix.empty())
	{
		// wild = "*.json";
	}
	else if (!cli.wildPattern.empty())
	{
		wild = cli.wildPattern;
		if (wild.find_first_of("*?") == std::string::npos)
		{
			wild = "*" + wild + "*";
		}
	}
	else if (!cli.wildSubfix.empty())
	{
		wild = "*" + cli.wildSubfix;
	}
	else
	{
		wild = cli.wildPrefix + "*";
	}

	if (!wild.empty())
	{
		fprintf(stderr, "Wild pattern to match json file: %s\n", wild.c_str());
	}
#endif

	FixPathTail(cli.jsonDir);
	FixPathTail(cli.jsonDir2);
	NormalizePath(cli.jsonDir);
	NormalizePath(cli.jsonDir2);

	if (!cli.jsonDir.empty())
	{
		fprintf(stderr, "Compare json from two dirctory: [%s] vs [%s]\n", cli.jsonDir.c_str(), cli.jsonDir2.c_str());
	}
	else
	{
		fprintf(stderr, "Compare json with those in dirctory: [%s]\n", cli.jsonDir2.c_str());
	}

	// move `file` and following extra arguments together
	std::vector<std::string> files = std::move(cli.extraArgs);
	if (!cli.jsonFile.empty())
	{
		files.insert(files.begin(), std::move(cli.jsonFile));
	}

	// read compared file from stdin if not provided in cmd line arguments
	if (files.empty())
	{
		std::string file;
		while (std::getline(std::cin, file))
		{
			if (file.empty() || file[0] == '#')
			{
				continue;
			}
			files.push_back(file);
		}
	}

	int iRet = 0;
	for (auto& file : files)
	{
		NormalizePath(file);
		if (!cli.jsonDir.empty()) // --i1 --i2 both provided
		{
			RemovePrefix(file, cli.jsonDir) || RemovePrefix(file, cli.jsonDir2);
			cli.jsonFile = cli.jsonDir + file;
			cli.jsonFile2 = cli.jsonDir2 + file;
		}
		else // --i2 only provided
		{
			cli.jsonFile = file;
			RemoveDir(file);
			cli.jsonFile2 = cli.jsonDir2 + file;
		}

		if (CompareJsonFile() != 0)
		{
			--iRet;
		}
	}

	return iRet;
}

int CApp::CompareJsonFile()
{
	fprintf(stderr, "Compare tow json files: [%s] vs [%s]\n", PrimaryJsonFile(), cli.jsonFile2.c_str());
	if (ReadPrimaryJson() != 0 || ReadSecondaryJson() != 0)
	{
		return -1;
	}

	const rapidjson::Value* pJson = GetJsonNode(json, cli.jsonPath);
	if (nullptr == pJson)
	{
		fprintf(stderr, "Can not resolve the json path pointer: %s#%s\n", PrimaryJsonFile(), cli.jsonPath.c_str());
		return -1;
	}

	const rapidjson::Value* pJson2 = GetJsonNode(json2, cli.jsonPath2);
	if (nullptr == pJson2)
	{
		fprintf(stderr, "Can not resolve the json path pointer: %s#%s\n", cli.jsonFile2.c_str(), cli.jsonPath2.c_str());
		return -1;
	}

	if (CompareJsonOper(*pJson, *pJson2) != 0)
	{
		fprintf(stderr, "[FAIL] to compare the two json files.\n");
		return -1;
	}
	else
	{
		fprintf(stderr, "[PASS] to compare the two json files.\n");
		return 0;
	}
}

int CApp::CompareJsonNode()
{
	if (ReadPrimaryJson() != 0)
	{
		return -1;
	}

	fprintf(stderr, "Compare tow nodes: [%s] vs [%s] in json file [%s]\n", cli.jsonPath.c_str(), cli.jsonPath2.c_str(), PrimaryJsonFile());

	const rapidjson::Value* pJson = GetJsonNode(json, cli.jsonPath);
	if (nullptr == pJson)
	{
		fprintf(stderr, "Can not resolve the json path pointer: %s\n", cli.jsonPath.c_str());
		return -1;
	}

	const rapidjson::Value* pJson2 = GetJsonNode(json, cli.jsonPath2);
	if (nullptr == pJson2)
	{
		fprintf(stderr, "Can not resolve the json path pointer: %s\n", cli.jsonPath2.c_str());
		return -1;
	}

	if (CompareJsonOper(*pJson, *pJson2) != 0)
	{
		fprintf(stderr, "[FAIL] to compare the two json nodes.\n");
		return -1;
	}
	else
	{
		fprintf(stderr, "[PASS] to compare the two json nodes.\n");
		return 0;
	}
}

int CApp::CompareJsonOper(const rapidjson::Value& a, const rapidjson::Value& b)
{
	int iRet = 0;
	bool bEqual = (cli.equalCompare || (!cli.lessCompare && !cli.greatCompare));
	if (bEqual)
	{
		iRet = CompareJsonEQ(a, b, cli.jsonPath, cli.jsonPath2);
	}
	else if (cli.lessCompare)
	{
		iRet = CompareJsonLE(a, b, cli.jsonPath, cli.jsonPath2);
	}
	else if (cli.greatCompare)
	{
		iRet = CompareJsonGE(a, b, cli.jsonPath, cli.jsonPath2);
	}
	else
	{
		fprintf(stderr, "Invalid compare flag, should not reach here!\n");
		return -1;
	}

	return iRet;
}

void CApp::SplitJsonFile(std::string& jsonFile, std::string& jsonPath)
{
	BreakString(jsonFile, "#:?", jsonPath);
}

void CApp::ClearSchema()
{
	DELETE_OBJECT(m_pSchema);
	DELETE_OBJECT(m_pSchemaProvider);
	DELETE_OBJECT(m_pSchemaValidator);
}

int CApp::BuildSchema()
{
	rapidjson::Document sd;
	int iRet = ReadJsonFile(sd, cli.jsonSchema);
	if (iRet != 0)
	{
		fprintf(stderr, "Fail to read schema json file: %s\n", cli.jsonSchema.c_str());
		return -1;
	}

	m_pSchemaProvider = new CSchemaProvider;
	if (nullptr == m_pSchemaProvider)
	{
		fprintf(stderr, "Fail to new object CSchemaProvider!\n");
		return -1;
	}
	m_pSchemaProvider->SetBaseDir(GetPathDir(cli.jsonSchema));

	m_pSchema = new rapidjson::SchemaDocument(sd, m_pSchemaProvider);
	if (nullptr == m_pSchema)
	{
		fprintf(stderr, "Fail to new object rapidjson::SchemaDocument!\n");
		return -1;
	}

	m_pSchemaValidator = new rapidjson::SchemaValidator(*m_pSchema);
	if (nullptr == m_pSchemaValidator)
	{
		fprintf(stderr, "Fail to new object rapidjson::SchemaValidator!\n");
		return -1;
	}

	return 0;
}

void CApp::ResetSchemaValidator()
{
	if (m_pSchemaValidator != nullptr)
	{
		m_pSchemaValidator->Reset();
	}
}

// todo: move to JsonUtil.cpp?  but output not centralize
int CApp::ValidatorJson(const rapidjson::Value& doc)
{
	if (m_pSchemaValidator == nullptr)
	{
		fprintf(stderr, "Not build schema validator!\n");
		return -1;
	}

	if (!doc.Accept(*m_pSchemaValidator))
	{
		rapidjson::StringBuffer sb;
		m_pSchemaValidator->GetInvalidSchemaPointer().StringifyUriFragment(sb);
		fprintf(stderr, "Invalid schema: %s\n", sb.GetString());
		fprintf(stderr, "Invalid keyword: %s\n", m_pSchemaValidator->GetInvalidSchemaKeyword());
		sb.Clear();
		m_pSchemaValidator->GetInvalidDocumentPointer().StringifyUriFragment(sb);
		fprintf(stderr, "Invalid document: %s\n", sb.GetString());
		return -1;
	}

	return 0;
}
