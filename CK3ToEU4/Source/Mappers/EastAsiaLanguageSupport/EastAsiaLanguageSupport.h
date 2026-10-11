#ifndef EAST_ASIALANGUAGE_SUPPORT
#define EAST_ASIALANGUAGE_SUPPORT
#include <windows.h>
namespace mappers
{
class EastAsiaLanguageSupport
{
  public:
	static std::string utf8StringToSpecial(const std::string& s, bool toUtf8, bool isEU4);
  private:
	static std::wstring utf8ToWString(const std::string& utf8);
	static bool internalChars(byte highOrLow, bool toUTF8, bool isEU4);
	static wchar_t cp1252ToUCS2(byte c);
	static wchar_t UCS2ToCP1252(int c);
	
};
} // namespace mappers
#endif