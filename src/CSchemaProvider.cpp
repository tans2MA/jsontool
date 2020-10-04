#include "CSchemaProvider.h"

#include <stdlib.h>

#include "rapidjson/document.h"
#include "JsonUtil.h"

const rapidjson::SchemaDocument* CSchemaProvider::GetRemoteDocument(const char* uri, size_t length)
{
	std::string file = m_baseDir + uri;
	rapidjson::Document sd;
	int iRet = ReadJsonFile(sd, file);
	if (iRet != 0)
	{
		fprintf(stderr, "Fail to read schema json file: %s\n", file.c_str());
		return NULL;
	}

	auto pSchema = new rapidjson::SchemaDocument(sd);
	m_pSchemas.push_back(pSchema);
	return pSchema;
}
