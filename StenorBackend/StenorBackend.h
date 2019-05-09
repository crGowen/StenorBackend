#pragma once

#include <iostream>
#include <vector>

extern "C"
{
	__declspec(dllexport) int EncodeToContainer(const char* inputFileInterOp, const char* containerFileInterOp);

	__declspec(dllexport) int ParseBinaryInput(const char* filepathInterOp);

	__declspec(dllexport) int GetImgSize(const char* pathToFileInterOp);

	__declspec(dllexport) int GetTxtSize(const char* pathToFileInterOp);
}
