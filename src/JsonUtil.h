#ifndef JSONUTIL_H__
#define JSONUTIL_H__

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

#include <iostream>

int ReadJson(rapidjson::Document& doc, std::istream& stream);
int ReadJsonFile(rapidjson::Document& doc, const std::string& file);
int WriteJson(const rapidjson::Value& doc, std::ostream& stream, bool pretty = true);

// comapre json. bCanLess means that av may be a subset of bv, that
// has less fields of the same object or shorter length of the same array, 
// while scalar value must be equal.
int CompareJsonImpl(const rapidjson::Value& av, const rapidjson::Value& bv, bool bCanLess);

inline
int CompareJsonEQ(const rapidjson::Value& av, const rapidjson::Value& bv)
{
    return CompareJsonImpl(av, bv, false);
}
inline
int compareJsonLE(const rapidjson::Value& av, const rapidjson::Value& bv)
{
    return CompareJsonImpl(av, bv, true);
}
inline
int compareJsonGE(const rapidjson::Value& av, const rapidjson::Value& bv)
{
	return compareJsonLE(bv, av);
}

#endif /* end of include guard: JSONUTIL_H__ */

