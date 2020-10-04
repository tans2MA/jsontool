#ifndef JSONUTIL_H__
#define JSONUTIL_H__

#include <string>
#include <iostream>

#include "JsonConfig.h"

int ReadJson(rapidjson::Document& doc, std::istream& stream);
int ReadJsonFile(rapidjson::Document& doc, const std::string& file);
int WriteJson(const rapidjson::Value& doc, std::ostream& stream, bool pretty = true);

// get json node by path
const rapidjson::Value* GetJsonNode(const rapidjson::Value& json, const std::string& path);

// comapre json. bCanLess means that av may be a subset of bv, that
// has less fields of the same object or shorter length of the same array, 
// while scalar value must be equal.
int CompareJsonImpl(const rapidjson::Value& av, const rapidjson::Value& bv, bool bCanLess,
    const std::string& apath = std::string(), const std::string& bpath = std::string());

inline
int CompareJsonEQ(const rapidjson::Value& av, const rapidjson::Value& bv, const std::string& apath, const std::string& bpath)
{
    return CompareJsonImpl(av, bv, false, apath, bpath);
}
inline
int CompareJsonLE(const rapidjson::Value& av, const rapidjson::Value& bv, const std::string& apath, const std::string& bpath)
{
    return CompareJsonImpl(av, bv, true, apath, bpath);
}
inline
int CompareJsonGE(const rapidjson::Value& av, const rapidjson::Value& bv, const std::string& apath, const std::string& bpath)
{
	return CompareJsonLE(bv, av, bpath, apath);
}

#endif /* end of include guard: JSONUTIL_H__ */

