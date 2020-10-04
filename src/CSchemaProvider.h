#ifndef CSCHEMAPROVIDER_H__
#define CSCHEMAPROVIDER_H__

#include <string>
#include <vector>
#include <map>

#include "JsonConfig.h"
#include "rapidjson/schema.h"

class CSchemaProvider : public rapidjson::IRemoteSchemaDocumentProvider
{
public:
	virtual const rapidjson::SchemaDocument* GetRemoteDocument(const char* uri, size_t length);

	void SetBaseDir(const std::string& baseDir)
	{
		m_baseDir = baseDir;
	}

	CSchemaProvider(){}
	~CSchemaProvider()
	{
		for (auto& item : m_mapSchemas)
		{
			if (item.second != nullptr)
			{
				delete item.second;
				item.second = nullptr;
			}
		}
	}

private:
	std::string m_baseDir;
	std::map<std::string, rapidjson::SchemaDocument*>m_mapSchemas;
};

#endif /* end of include guard: CSCHEMAPROVIDER_H__ */
