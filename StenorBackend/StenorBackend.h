#pragma once

#include <iostream>
#include <vector>

extern "C"
{
	__declspec(dllexport) int EncodeToContainer(const char* inputFileInterOp, const char* containerFileInterOp);

	__declspec(dllexport) int ParseContainer(const char* filepathInterOp);

	__declspec(dllexport) int GetImgSize(const char* pathToFileInterOp);

	__declspec(dllexport) int GetWavSize(const char* pathToFileInterOp);

	__declspec(dllexport) int GetRequiredBytesForEncode(const char* pathToFileInterOp);
}
