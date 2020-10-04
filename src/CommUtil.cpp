#include "CommUtil.h"

bool FixPathTail(std::string& path)
{
#ifdef _MSC_VER
	static char cSlash = SLASH_WIN;
#else
	static char cSlash = SLASH_UIX;
#endif // _MSV_VER

	if (path.empty() || path.back() == cSlash)
	{
		return false;
	}
	else
	{
		path.push_back(cSlash);
		return true;
	}
}

int NormalizePath(std::string& path)
{
	if (path.empty())
	{
		return 0;
	}

	int iCount = 0;
#ifdef _MSC_VER
	for (auto& ch : path)
	{
		if (ch == SLASH_WIN)
		{
			ch = SLASH_UIX;
			++iCount;
		}
	}
#endif // _MSC_VER

	return iCount;
}

bool RemovePrefix(std::string& path, const std::string& prefix)
{
	if (path.size() < prefix.size())
	{
		return false;
	}

	if (path.compare(0, prefix.size(), prefix) == 0)
	{
		path.erase(0, prefix.size());
		return true;
	}

	return false;
}

bool RemoveDir(std::string& path)
{
	size_t pos = path.find_last_of(SLASH_UIX);
	if (pos != std::string::npos)
	{
		path.erase(0, pos + 1);
		return true;
	}
	return false;
}

std::string GetPathDir(const std::string& path)
{
	size_t pos = path.find_last_of(SLASH_ALL);
	if (pos != std::string::npos)
	{
		return path.substr(0, pos + 1);
	}
	return std::string();
}

bool BreakString(std::string& source, const char* any, std::string& rest)
{
	if (source.empty())
	{
		return false;
	}

	size_t pos = source.find_first_of(any);
	if (pos != std::string::npos)
	{
		rest = source.substr(pos + 1);
		source.erase(pos);
		return true;
	}

	return false;
}
