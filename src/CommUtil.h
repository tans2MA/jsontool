#ifndef COMMUTIL_H__
#define COMMUTIL_H__

#include <string>

#define DELETE_OBJECT(p) do{ if (nullptr != p) { delete p; p = nullptr; } } while(0)

const char SLASH_WIN = '\\';
const char SLASH_UIX = '/';
#define SLASH_ALL "/\\"

// add slash to tail of path string if needed, return true if really added
bool FixPathTail(std::string& path);

// replace all slash to / as unix style, return the count slash replaced
int NormalizePath(std::string& path);

// remove matched prefix string, return true if really removed
bool RemovePrefix(std::string& path, const std::string& prefix);

// remove directory part in a path string. /path/to/filename.ext --> filename.ext
bool RemoveDir(std::string& path);

// get te directory path of a path.
std::string GetPathDir(const std::string& path);

// break string `source` by an char within `any` like `find_first_of()` in string
// trim `source` to left part, save the right part to `rest`
// return ture if actually took such operation.
bool BreakString(std::string& source, const char* any, std::string& rest);

#endif /* end of include guard: COMMUTIL_H__ */
