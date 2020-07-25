#pragma once

#ifdef MAKE_DLL
#define DLL_IMEX __declspec(dllexport)
#else
#define DLL_IMEX __declspec(dllimport)
#endif

// add DLL_IMEX to each for dynamic
extern "C"
{

	DLL_IMEX __int32 OutputFileToImg(const char* inputFile, const char* containerFile);

	DLL_IMEX __int32 DecodeFromImg(const char* containerFile);

	DLL_IMEX __int32 GetImgBytesStorable(const char* file);

	DLL_IMEX __int32 GetRequiredBytesForEncode(const char* file);
}