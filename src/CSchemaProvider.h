#ifndef CSCHEMAPROVIDER_H__
#define CSCHEMAPROVIDER_H__

#include <string>
#include <vector>

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
		for (auto& pSchema : m_pSchemas)
		{
			if (nullptr != pSchema)
			{
				delete pSchema;
				pSchema = nullptr;
			}
		}
	}

private:
	std::string m_baseDir;
	std::vector<rapidjson::SchemaDocument*> m_pSchemas;
};

#endif /* end of include guard: CSCHEMAPROVIDER_H__ */
