#include "JsonUtil.h"

#include <stdlib.h>
#include <cmath>
#include <limits>
#include <type_traits>
#include <fstream>

#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include "rapidjson/prettywriter.h"
//#include "rapidjson/filereadstream.h"
//#include "rapidjson/filewritestream.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/error/en.h"


// refer to: https://blog.csdn.net/davidhopper/article/details/79609064
// Test whether two float or double numbers are equal.
// ulp: units in the last place.
template <typename T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
IsAlmostEqual(T x, T y, int ulp = 2)
{
    // the machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return std::fabs(x - y) < std::numeric_limits<T>::epsilon() * std::fabs(x + y) * ulp
           // unless the result is subnormal
           || std::fabs(x - y) < std::numeric_limits<T>::min();
}

int ReadJson(rapidjson::Document& doc, std::istream& stream)
{
	rapidjson::IStreamWrapper is(stream);
	doc.ParseStream(is);
	if (doc.HasParseError())
	{
		// fprintf(stderr, "Input is not a valid JSON\n");
		fprintf(stderr, "Error(offset %u): %s\n",
				static_cast<unsigned>(doc.GetErrorOffset()),
				GetParseError_En(doc.GetParseError()));
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int ReadJsonFile(rapidjson::Document& doc, const std::string& file)
{
    int iRet = 0;
	std::ifstream ifs(file);
	if (ifs.is_open())
	{
		iRet = ReadJson(doc, ifs);
	}
	else
	{
		iRet = -1;
	}
	ifs.close();
    return iRet;
}

int WriteJson(const rapidjson::Value& doc, std::ostream& stream, bool pretty)
{
	rapidjson::OStreamWrapper os(stream);

	if (pretty)
	{
		rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(os);
		doc.Accept(writer);
	}
	else
	{
		rapidjson::Writer<rapidjson::OStreamWrapper> writer(os);
		doc.Accept(writer);
	}

	return EXIT_SUCCESS;
}

#define ASSERT_CMP(expr) do { if(!(expr)) return -1; } while(0)
#define ASSERT_CMPSTR(a, b) do { if(strcmp((a), (b))) return -1; } while(0)
#define ASSERT_CMPNUM(a, b) do { if(!IsAlmostEqual((a), (b))) return -1; } while(0)

int CompareJsonImpl(const rapidjson::Value& av, const rapidjson::Value& bv, bool bCanLess)
{
    if (av.IsObject())
    {
        ASSERT_CMP(bv.IsObject());
        if (!bCanLess)
        {
			ASSERT_CMP(av.MemberCount() == bv.MemberCount());
			for (auto ai = av.MemberBegin(), bi = bv.MemberBegin(); ai != av.MemberEnd(); ++ai, ++bi)
			{
				ASSERT_CMPSTR(ai->name.GetString(), bi->name.GetString());
				CompareJsonImpl(ai->value, bi->value, bCanLess);
			}
        }
        else
        {
			ASSERT_CMP(av.MemberCount() <= bv.MemberCount());
			for (auto ai = av.MemberBegin(); ai != av.MemberEnd(); ++ai)
			{
                auto bi = bv.FindMember(ai->name.GetString());
                ASSERT_CMP(bi != bv.MemberEnd());
				CompareJsonImpl(ai->value, bi->value, bCanLess);
			}
        }
    }
    else if (av.IsArray())
    {
        ASSERT_CMP(bv.IsArray());
        if (!bCanLess)
        {
        }
        else
        {
			ASSERT_CMP(av.Size() <= bv.Size());
        }
		for (auto ai = av.Begin(), bi = bv.Begin(); ai != av.End(); ++ai, ++bi)
		{
			CompareJsonImpl(*ai, *bi, bCanLess);
		}
    }
    else if (av.IsString())
    {
        ASSERT_CMP(bv.IsString());
        ASSERT_CMPSTR(av.GetString(), bv.GetString());
    }
    else if (av.IsBool())
    {
        ASSERT_CMP(bv.IsBool());
        ASSERT_CMP(av.GetBool() == bv.GetBool());
    }
    else if (av.IsInt())
    {
        ASSERT_CMP(bv.IsInt());
        ASSERT_CMP(av.GetInt() == bv.GetInt());
    }
    else if (av.IsDouble())
    {
        ASSERT_CMP(bv.IsDouble());
        ASSERT_CMPNUM(av.GetDouble(), bv.GetDouble());
    }
    else if (av.IsInt64())
    {
        ASSERT_CMP(bv.IsInt64());
        ASSERT_CMP(av.GetInt64() == bv.GetInt64());
    }
    else if (av.IsUint())
    {
        ASSERT_CMP(bv.IsUint());
        ASSERT_CMP(av.GetUint() == bv.GetUint());
    }
    else if (av.IsUint64())
    {
        ASSERT_CMP(bv.IsUint64());
        ASSERT_CMP(av.GetUint64() == bv.GetUint64());
    }
    else if (av.IsNull())
    {
        ASSERT_CMP(bv.IsNull());
    }
    else
    {
        ASSERT_CMP(false);
    }

	return 0;
}

