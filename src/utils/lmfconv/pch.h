#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstdarg>
#include <intsafe.h>
#include <string.h>

#ifdef _DEBUG
#	include <debugapi.h>
#endif

//#define TEST_NO_OUTPUT
#ifdef _DEBUG
#	define dbg_print print
#else
#	define dbg_print(...) ((void)(__VA_ARGS__))
#endif

#define NL "\n"

#define OUT_EXTENSION		".lmf"
#define TEXTURE_EXTENSION	".tga"
#define DEFAULT_NO_TEXTURE	"data/textures/system/notex" TEXTURE_EXTENSION

enum ConversionOptions_
{
	ConversionOptions_FBX_Triangulate = (1 << 0)
};
typedef uint32_t ConversionOptions;

bool fbxconv_perform(const char* inFileName, const char* outFileName, ConversionOptions options);
bool daeconv_perform(const char* inFileName, const char* outFileName, ConversionOptions options);

void print(const char* fmt, ...);
