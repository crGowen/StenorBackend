// StenorBackend.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <bitset>

#include "StenorBackend.h"

#include <basicwavfileio.h>

// NOTE that binary operations are performed here by coverting each byte to a binary STRING "xxxxxxxx", but it would be more a litle more efficient
// to keep it as a char format and subtract z%4 from the value, then add the 2-bit data to the char as a value from 0-3. But I've kept it at string format
// so that the operations are clear and easily understandable. This is, after all, a hobby project.

// non-exported functions

//DONE
char ConvertBinStringToChar(std::string bin)
{
	std::bitset<8> x = std::bitset<8>(bin);
	return x.to_ulong();
}

//DONE
void ConvertBinaryToFile(std::string binString)
{
	// convert binary string to text message
	std::string buildStr(binString.length() / 8, '2');


	for (int i = 0; i < buildStr.length(); i++)
	{
		buildStr[i] = ConvertBinStringToChar(binString.substr(i * 8, 8));
	}
	std::string fileType = buildStr.substr(0, 10);
	std::string fileTypeCleaned = "";
	buildStr.erase(0, 10);

	for (int i = 0; i < 10; i++)
	{
		if (fileType[i] != '-') fileTypeCleaned = fileTypeCleaned + fileType[i];
	}

	//write to file
	std::ofstream foutstream;
	foutstream.open("./stenor_decoded." + fileTypeCleaned, std::ios::binary);
	foutstream.write((char*)&buildStr[0], buildStr.length());
	foutstream.close();
}

//DONE
std::string ReadBinaryFromImg(std::string containerFile)
{
	cv::Mat container = cv::imread(containerFile, cv::IMREAD_COLOR);
	std::string buildStr(container.cols * container.rows * 6, '2');
	std::string tempBinStr = "";
	std::bitset<8> x;

	for (int i = 0; i < container.rows; i++)
	{
		for (int j = 0; j < container.cols; j++)
		{
			//B
			x = container.at<cv::Vec3b>(i, j).val[0];
			tempBinStr = x.to_string();
			buildStr.replace((i*container.cols + j) * 6, 2, tempBinStr.substr(6, 2));

			//G
			x = container.at<cv::Vec3b>(i, j).val[1];
			tempBinStr = x.to_string();
			buildStr.replace((i*container.cols + j) * 6 + 2, 2, tempBinStr.substr(6, 2));

			//R
			x = container.at<cv::Vec3b>(i, j).val[2];
			tempBinStr = x.to_string();
			buildStr.replace((i*container.cols + j) * 6 + 4, 2, tempBinStr.substr(6, 2));
		}
	}

	return buildStr;
}

//DONE
std::string GetFileType(std::string inputFile)
{
	std::string type = "";
	bool finished = false;
	for (int i = inputFile.length() - 1; i >= 0  && !finished; i--)
	{
		if (inputFile[i] == '.') finished = true;
		else type = inputFile[i] + type;
	}
	if (!finished) return ".";
	else
	{
		for (int i = type.length(); i < 10; i++)
		{
			type = type + "-";
		}
		return type;
	}
}

//DONE
std::string ConvertFileToBinary(std::string inputFile)
{
	// first check file type
	std::string fileType = GetFileType(inputFile);
	if (fileType == ".")
	{
		//throw exception
	}

	// second check if exists

	// third, go to work
	std::ifstream finstream;
	finstream.open(inputFile, std::ios::binary);
	finstream.seekg(0, std::ios_base::end);
	int size = finstream.tellg();
	std::string read(size, 'x');
	finstream.seekg(0, std::ios_base::beg);
	finstream.read((char*)&read[0], size);
	finstream.close();

	read = fileType + read;
	std::string bitStr(read.length() * 8, 'x');
	std::bitset<8> x;
	for (int i = 0; i < read.length(); i++)
	{
		x = read[i];
		bitStr.replace(i * 8, 8, x.to_string());
	}
	return bitStr;
}

//DONE
void OutputBinaryToImg(std::string bin, std::string containerFile, std::string outputFile)
{
	// output image containing a text message

	cv::Mat container = cv::imread(containerFile, cv::IMREAD_COLOR);

	if (bin.length() > container.cols * container.rows * 6)
	{
		//throw exception
	}

	int textStep = 0;
	std::bitset<8> x;
	std::string tempBinStr;

	for (int i = 0; i < container.rows; i++)
	{
		for (int j = 0; j < container.cols; j++)
		{
			//B
			x = container.at<cv::Vec3b>(i, j).val[0];
			tempBinStr = x.to_string();
			tempBinStr = tempBinStr.substr(0, 6) + bin.substr(textStep % bin.length(), 2);
			x = std::bitset< 8 >(tempBinStr);
			container.at<cv::Vec3b>(i, j).val[0] = x.to_ulong();
			textStep = textStep + 2;

			//G

			x = container.at<cv::Vec3b>(i, j).val[1];
			tempBinStr = x.to_string();
			tempBinStr = tempBinStr.substr(0, 6) + bin.substr(textStep % bin.length(), 2);
			x = std::bitset< 8 >(tempBinStr);
			container.at<cv::Vec3b>(i, j).val[1] = x.to_ulong();
			textStep = textStep + 2;

			//R
			x = container.at<cv::Vec3b>(i, j).val[2];
			tempBinStr = x.to_string();
			tempBinStr = tempBinStr.substr(0, 6) + bin.substr(textStep % bin.length(), 2);
			x = std::bitset< 8 >(tempBinStr);
			container.at<cv::Vec3b>(i, j).val[2] = x.to_ulong();
			textStep = textStep + 2;

		}
	}
	cv::imwrite(outputFile, container);
}

void OutputBinaryToWav(std::string bin, std::string containerFile, std::string outputFile)
{
	wavio::WavFileData wfile;
	wfile.ConstructFromFinstream(containerFile);

	int textStep = 0;
	std::bitset<16> x;
	std::string tempBinStr;

	for (int i = 0; i < wfile.head.chunkSize / 2; i++)
	{
		x = wfile.shortDataArray[i];
		tempBinStr = x.to_string();
		tempBinStr = tempBinStr.substr(0, 14) + bin.substr(textStep % bin.length(), 2);
		x = std::bitset< 16 >(tempBinStr);
		wfile.shortDataArray[i] = x.to_ulong();
		textStep = textStep + 2;
	}

	wavio::WavFileData::OutputWavObjToFile(wfile, outputFile);

	wfile.DestroyDynamicVars();
}

// exported functions

//DONE
int EncodeToContainer(const char* inputFileInterOp, const char* containerFileInterOp)
{
	std::string inputFile = inputFileInterOp;
	std::string containerFile = containerFileInterOp;
	OutputBinaryToImg(ConvertFileToBinary(inputFile), containerFile, "./stenor_encoded.png");
	return 0;
}

//DONE
int ParseImage(const char* filepathInterOp)
{
	std::string filepath = filepathInterOp;
	// exceptions and file checks!
	ConvertBinaryToFile(ReadBinaryFromImg(filepath));
	return 0;
}

//DONE
int GetImgSize(const char* pathToFileInterOp)
{
	std::string pathToFile = pathToFileInterOp;
	cv::Mat img = cv::imread(pathToFile, cv::IMREAD_COLOR);
	return img.cols * img.rows;

	// if error, return 0
}

//DONE
int GetRequiredPixelsForEncode(const char* pathToFileInterOp)
{
	std::string pathToFile = pathToFileInterOp;
	std::ifstream finstream;
	finstream.open(pathToFile, std::ios::binary);
	finstream.seekg(0, std::ios_base::end);
	int size = finstream.tellg();
	finstream.close();
	
	size = size * 1.35;

	return size;

	// if error, return 0
}
