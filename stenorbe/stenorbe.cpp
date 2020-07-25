#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "stenorbe.h"

cv::Mat container;
std::string bin;

// self-explanatory
std::string StringToLower(std::string str) {
	for (int i = 0; i < str.length(); i++) {
		str[i] = std::tolower(str[i]);
	}
	return str;
}

// returns file type padded out to 8 bytes long
std::string GetFileType(std::string inputFile)
{
	std::string type = "";
	bool finished = false;
	for (int i = 1; i < 9 && !finished; i++)
	{
		__int32 index = inputFile.length() - i;
		if (inputFile[index] == '.') finished = true;
		else type = inputFile[index] + type;
	}
	if (!finished) return ".";
	else
	{
		while(type.length() < 8)
		{
			type = type + "-";
		}
		return StringToLower(type);
	}
}

// reads file and stores it in a string (along with the size of the data and the file type)
__int32 FileToBin(std::string inputFile) {
	std::string fileType = GetFileType(inputFile);

	try {
		std::ifstream finstream;
		finstream.open(inputFile, std::ios::binary);
		finstream.seekg(0, std::ios_base::end);
		__int32 size = finstream.tellg();
		std::string read(size, 'x');
		finstream.seekg(0, std::ios_base::beg);
		finstream.read((char*)&read[0], size);
		finstream.close();

		char* chars = reinterpret_cast<char*>(&size);
		std::string sizeStr(4, 'x');
		for (int i = 0; i < 4; i++) {
			sizeStr[i] = chars[i];
		}
		bin = fileType + sizeStr + read;
		return 0;
	}
	catch (std::exception e) {
		return 40;
	}
}

// for multithreading
void ThreadOutputBin(__int32 start, __int32 end) {

	char byte;
	__int32 binStrOffset = start;
	__int32 bitOffset = 0;

	__int32 rowStart = (start * 4) / 3;
	rowStart /= container.cols;
	
	__int32 colStart = ((start * 4) / 3) % container.cols;

	const unsigned char mask = 252;

	for (int i = rowStart; i < container.rows && binStrOffset < end; i++) {
		for (int j = colStart; j < container.cols && binStrOffset < end; j++) {
			for (int c = 0; c < 3 && binStrOffset < end; c++) {
				byte = container.at<cv::Vec3b>(i, j).val[c];
				container.at<cv::Vec3b>(i, j).val[c] = (byte & mask) + ( (bin[binStrOffset] >> (6 - bitOffset)) & ~mask);

				bitOffset = (bitOffset + 2) % 8;
				if (bitOffset == 0) binStrOffset++;
			}
		}
		colStart = 0;
	}
}

// Encode string obtained from "FileToBin" into an image
__int32 OutputFileToImg(const char* inputFile, const char* containerFile) {
	try {

		__int32 res = FileToBin(inputFile);
		if (res != 0) return res;

		container = cv::imread(containerFile, cv::IMREAD_COLOR);

		// number of pixels required
		__int32 pix = 1;
		if ((bin.length() * 4) % 3 == 0) pix = 0;
		pix += (bin.length() * 4 / 3);

		if (pix > container.cols * container.rows) {
			return 20;
		}

		if (bin.length() >= 800) {
			std::thread t1(ThreadOutputBin, 0, (bin.length() / 4) - ((bin.length() / 4) % 3));
			std::thread t2(ThreadOutputBin, (bin.length() / 4) - ((bin.length() / 4) % 3), (bin.length() / 2) - ((bin.length() / 2) % 3));
			std::thread t3(ThreadOutputBin, (bin.length() / 2) - ((bin.length() / 2) % 3), ((bin.length() * 3) / 4) - (((bin.length() * 3) / 4) % 3));
			std::thread t4(ThreadOutputBin, ((bin.length() * 3) / 4) - (((bin.length() * 3) / 4) % 3), bin.length());

			t1.join();
			t2.join();
			t3.join();
			t4.join();
		}
		else {
			std::thread t1(ThreadOutputBin, 0, bin.length());
			t1.join();
		}
		

		cv::imwrite("./stenor_encoded.png", container);
		return 0;

	}
	catch (std::exception e) {
		return 20;
	}
}

// for multithreading
void ThreadDecode(__int32 start, __int32 end) {
	char byte;
	__int32 binStrOffset = start;
	__int32 bitOffset = 0;

	const unsigned char mask = 3;

	__int32 rowStart = ((start + 12) * 4) / 3;
	rowStart /= container.cols;

	__int32 colStart = (((start + 12) * 4) / 3) % container.cols;


	for (int i = rowStart; i < container.rows && binStrOffset < end; i++) {
		for (int j = colStart; j < container.cols && binStrOffset < end; j++) {
			for (int c = 0; c < 3 && binStrOffset < end; c++) {
				byte = container.at<cv::Vec3b>(i, j).val[c];
				bin[binStrOffset] += ((byte & mask) << (6 - bitOffset));


				bitOffset = (bitOffset + 2) % 8;
				if (bitOffset == 0) binStrOffset++;
			}
		}
		colStart = 0;
	}

}

// Decode data from an image
__int32 DecodeFromImg(const char* containerFile) {
	try {
		container = cv::imread(containerFile, cv::IMREAD_COLOR);

		__int32 binStrOffset = 0;
		__int32 bitOffset = 0;
		const unsigned char nc = 0;
		std::string str(12, nc);
		char byte;
		const unsigned char mask = 3;

		// container.at<cv::Vec3b>(i, j).val[c] = (byte & mask) + ((bin[binStrOffset] >> 6 - bitOffset) & ~mask);

		for (int i = 0; i < container.rows && binStrOffset < 12; i++) {
			for (int j = 0; j < container.cols && binStrOffset < 12; j++) {
				for (int c = 0; c < 3 && binStrOffset < 12; c++) {
					byte = container.at<cv::Vec3b>(i, j).val[c];
					str[binStrOffset] += (((byte & mask) << (6 - bitOffset)));


					bitOffset = (bitOffset + 2) % 8;
					if (bitOffset == 0) binStrOffset++;
				}
			}
		}

		if (binStrOffset != 12) throw 60;

		std::string fileType = "";

		for (int i = 0; i < 8; i++)
		{
			if (str[i] != '-') fileType = fileType + str[i];
		}

		__int32 dataSize =  *reinterpret_cast<__int32*>(&str[8]);

		// PREPARATION OVER!

		bin = std::string(dataSize, nc);

		if (bin.length() >= 800) {
			std::thread t1(ThreadDecode, 0, (bin.length() / 4) - ((bin.length() / 4) % 3));
			std::thread t2(ThreadDecode, (bin.length() / 4) - ((bin.length() / 4) % 3), (bin.length() / 2) - ((bin.length() / 2) % 3));
			std::thread t3(ThreadDecode, (bin.length() / 2) - ((bin.length() / 2) % 3), ((bin.length() * 3) / 4) - (((bin.length() * 3) / 4) % 3));
			std::thread t4(ThreadDecode, ((bin.length() * 3) / 4) - (((bin.length() * 3) / 4) % 3), bin.length());

			t1.join();
			t2.join();
			t3.join();
			t4.join();
		}
		else {
			std::thread t1(ThreadDecode, 0, bin.length());
			t1.join();
		}

		//write to file
		std::ofstream foutstream;
		foutstream.open("./stenor_decoded." + fileType, std::ios::binary);
		foutstream.write((char*)&bin[0], dataSize);
		foutstream.close();

		return 0;
;	}
	catch (std::exception e) {
		return 60;
	}
}

// return the size of the encoding required
__int32 GetRequiredBytesForEncode(const char* file)
{
	try {
		std::ifstream finstream;
		finstream.open(file, std::ios::binary);
		finstream.seekg(0, std::ios_base::end);
		int size = finstream.tellg();
		finstream.close();

		//required bytes for encoding: bytes * 1.35 for number of image pixels; bytes / 20000 for audio file seconds

		return size + 12;
	}
	catch (std::exception e)
	{
		return 0;
	}
}

// return how many bytes an image can store as encoding
__int32 GetImgBytesStorable(const char* file)
{
	try
	{
		//returns number of bytes that the image can store
		cv::Mat img = cv::imread(file, cv::IMREAD_COLOR);
		return img.cols * img.rows * 3 / 4;
	}
	catch (std::exception e)
	{
		return 0;
	}

	// if error, return 0
}

