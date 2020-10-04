#include "CSchemaProvider.h"

#include <stdlib.h>

#include "rapidjson/document.h"
#include "JsonUtil.h"
#include "CommUtil.h"

const rapidjson::SchemaDocument* CSchemaProvider::GetRemoteDocument(const char* uri, size_t length)
{
	// fprintf(stderr, "Try to get remote schema refer: %s\n", uri);
	std::string uriKey(uri);
	std::string jsonPath;
	BreakString(uriKey, "#", jsonPath);

	auto it = m_mapSchemas.find(uriKey);
	if (it != m_mapSchemas.end())
	{
		// fprintf(stderr, "Get the cached schema!\n");
		return it->second;
	}

	std::string jsonFile = m_baseDir + uriKey;
	rapidjson::Document docSchema;
	int iRet = ReadJsonFile(docSchema, jsonFile);
	if (iRet != 0)
	{
		fprintf(stderr, "Fail to read schema json file: %s\n", jsonFile.c_str());
		return nullptr;
	}

	// the required is the whole SchemaDocument, not further parse #path or #id self.
	rapidjson::SchemaDocument* pSchema = new rapidjson::SchemaDocument(docSchema);
	if (pSchema != nullptr)
	{
		m_mapSchemas[uriKey] = pSchema;
		// fprintf(stderr, "Save cache for schema json file: %s\n", uriKey.c_str());
	}
	else
	{
		fprintf(stderr, "Fail to create new rapidjson::SchemaDocument\n");
	}

	return pSchema;
}
