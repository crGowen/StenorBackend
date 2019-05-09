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

// non-exported functions

//DONE
std::string DefaultEncode(char c)
{
	//default encoding is 6 bits per char, or 1 char per RGB pixel where each channel's last 2 bits are used
	switch (c)
	{
	case 'a':
	case 'A':
		return "000001";
	case 'b':
	case 'B':
		return "000010";
	case 'c':
	case 'C':
		return "000011";
	case 'd':
	case 'D':
		return "000100";
	case 'e':
	case 'E':
		return "000101";
	case 'f':
	case 'F':
		return "000110";
	case 'g':
	case 'G':
		return "000111";
	case 'h':
	case 'H':
		return "001000";
	case 'i':
	case 'I':
		return "001001";
	case 'j':
	case 'J':
		return "001010";
	case 'k':
	case 'K':
		return "001011";
	case 'l':
	case 'L':
		return "001100";
	case 'm':
	case 'M':
		return "001101";
	case 'n':
	case 'N':
		return "001110";
	case 'o':
	case 'O':
		return "001111";
	case 'p':
	case 'P':
		return "010000";
	case 'q':
	case 'Q':
		return "010001";
	case 'r':
	case 'R':
		return "010010";
	case 's':
	case 'S':
		return "010011";
	case 't':
	case 'T':
		return "010100";
	case 'u':
	case 'U':
		return "010101";
	case 'v':
	case 'V':
		return "010110";
	case 'w':
	case 'W':
		return "010111";
	case 'x':
	case 'X':
		return "011000";
	case 'y':
	case 'Y':
		return "011001";
	case 'z':
	case 'Z':
		return "011010";
	case '.':
		return "011011";
	case ',':
		return "011100";
	case ' ':
		return "011101";
	case '\n':
		return "011110";
	case '0':
		return "011111";
	case '1':
		return "100000";
	case '2':
		return "100001";
	case '3':
		return "100010";
	case '4':
		return "100011";
	case '5':
		return "100100";
	case '6':
		return "100101";
	case '7':
		return "100110";
	case '8':
		return "100111";
	case '9':
		return "101000";
	case '!':
		return "101001";
	case '?':
		return "101010";
	case '/':
		return "101011";
	case '(':
		return "101100";
	case ')':
		return "101101";
	case ':':
		return "101110";
	case '\'':
		return "101111";
	default:
		return "000000";
	}
}

//DONE
std::string DefaultDecode(std::string bin)
{
	std::bitset<8> x = std::bitset<8>(bin);
	switch (x.to_ulong())
	{
	case 1:
		return "a";
	case 2:
		return "b";
	case 3:
		return "c";
	case 4:
		return "d";
	case 5:
		return "e";
	case 6:
		return "f";
	case 7:
		return "g";
	case 8:
		return "h";
	case 9:
		return "i";
	case 10:
		return "j";
	case 11:
		return "k";
	case 12:
		return "l";
	case 13:
		return "m";
	case 14:
		return "n";
	case 15:
		return "o";
	case 16:
		return "p";
	case 17:
		return "q";
	case 18:
		return "r";
	case 19:
		return "s";
	case 20:
		return "t";
	case 21:
		return "u";
	case 22:
		return "v";
	case 23:
		return "w";
	case 24:
		return "x";
	case 25:
		return "y";
	case 26:
		return "z";
	case 27:
		return ".";
	case 28:
		return ",";
	case 29:
		return " ";
	case 30:
		return "\n";
	case 31:
		return "0";
	case 32:
		return "1";
	case 33:
		return "2";
	case 34:
		return "3";
	case 35:
		return "4";
	case 36:
		return "5";
	case 37:
		return "6";
	case 38:
		return "7";
	case 39:
		return "8";
	case 40:
		return "9";
	case 41:
		return "!";
	case 42:
		return "?";
	case 43:
		return "/";
	case 44:
		return "(";
	case 45:
		return ")";
	case 46:
		return ":";
	case 47:
		return "\'";
	default:
		return "*";
	}
}

//DONE
int ConvertBinaryToText(std::string binString, std::string outputFile)
{
	// convert binary string to text message
	std::string buildStr (binString.length()/6, '2');

	for (int i = 0; i*6 < binString.length(); i++)
	{
		buildStr.replace(i, 1, DefaultDecode(binString.substr(i * 6, 6)));
	}

	//write to file
	std::ofstream writeFile;
	writeFile.open(outputFile);
	writeFile << buildStr;
	writeFile.close();

	return 0;
}

//DONE
int ConvertBinaryToImg(std::string binString, std::string outputFile)
{
	// convert binary to img

	std::bitset<8> x;

	int dim_X = std::bitset<16>(binString.substr(6, 16)).to_ulong();
	int dim_Y = std::bitset<16>(binString.substr(22, 16)).to_ulong();

	std::string content = binString.substr(38, dim_X * dim_Y * 24);

	cv::Mat output = cv::Mat::zeros(cv::Size(dim_X, dim_Y), CV_8UC3);

	for (int i = 0; i < output.rows; i++)
	{
		for (int j = 0; j < output.cols; j++)
		{
			//B
			output.at<cv::Vec3b>(i, j).val[0] = std::bitset< 8 >(content.substr((i*output.cols + j) * 24, 8)).to_ulong();
			//G
			output.at<cv::Vec3b>(i, j).val[1] = std::bitset< 8 >(content.substr((i*output.cols + j) * 24 + 8, 8)).to_ulong();
			//R
			output.at<cv::Vec3b>(i, j).val[2] = std::bitset< 8 >(content.substr((i*output.cols + j) * 24 + 16, 8)).to_ulong();
		}
	}

	cv::imwrite(outputFile, output);




	return 0;
}

//TODO
int ConvertBinaryToAudio(std::string binString, std::string outputFile)
{
	// convert binary string to text message

	return 0;
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
std::string ConvertTextToBinary(std::string textFile)
{
	// convert text message to binary string
	std::ifstream input{ textFile };
	std::string text{ std::istreambuf_iterator <char>(input), std::istreambuf_iterator<char>() };
	std::string buildStr(text.length() * 6, '2');

	for (int i = 0; i < text.length(); i++)
	{
		buildStr.replace(i * 6, 6, DefaultEncode(text[i]));
	}

	//21 extra characters
	return buildStr + "011110011110000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011110011110011110";
}

//DONE
std::string ConvertImgToBinary(std::string storingImg)
{
	// convert png to binary string
	cv::Mat input = cv::imread(storingImg, cv::IMREAD_COLOR);

	std::bitset<8> x;
	std::string buildStr(input.cols * input.rows * 24, '2');

	for (int i = 0; i < input.rows; i++)
	{
		for (int j = 0; j < input.cols; j++)
		{
			//B
			x = input.at<cv::Vec3b>(i, j).val[0];
			buildStr.replace((i*input.cols + j) * 24, 8, x.to_string());

			//G

			x = input.at<cv::Vec3b>(i, j).val[1];
			buildStr.replace((i*input.cols + j) * 24 + 8, 8, x.to_string());

			//R
			x = input.at<cv::Vec3b>(i, j).val[2];
			buildStr.replace((i*input.cols + j) * 24 + 16, 8, x.to_string());
		}
	}
	buildStr = "111000" + std::bitset<16>(input.rows).to_string() + std::bitset<16>(input.cols).to_string() + buildStr;

	return buildStr;
}

//TODO
std::string ConvertAudioToBinary(std::string storingAudio)
{
	// convert wav to binary string

	return "placeholder";
}

//DONE
int OutputBinaryToImg(std::string text, std::string containerFile, std::string outputFile)
{
	// output image containing a text message

	cv::Mat container = cv::imread(containerFile, cv::IMREAD_COLOR);

	if (text.length() > container.cols * container.rows * 6)
		return 2;

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
			tempBinStr = tempBinStr.substr(0, 6) + text.substr(textStep % text.length(), 2);
			x = std::bitset< 8 >(tempBinStr);
			container.at<cv::Vec3b>(i, j).val[0] = x.to_ulong();
			textStep = textStep + 2;

			//G

			x = container.at<cv::Vec3b>(i, j).val[1];
			tempBinStr = x.to_string();
			tempBinStr = tempBinStr.substr(0, 6) + text.substr(textStep % text.length(), 2);
			x = std::bitset< 8 >(tempBinStr);
			container.at<cv::Vec3b>(i, j).val[1] = x.to_ulong();
			textStep = textStep + 2;

			//R
			x = container.at<cv::Vec3b>(i, j).val[2];
			tempBinStr = x.to_string();
			tempBinStr = tempBinStr.substr(0, 6) + text.substr(textStep % text.length(), 2);
			x = std::bitset< 8 >(tempBinStr);
			container.at<cv::Vec3b>(i, j).val[2] = x.to_ulong();
			textStep = textStep + 2;

		}
	}
	cv::imwrite(outputFile, container);


	return 50;
}


// exported functions

//DONE
int EncodeToContainer(const char* inputFileInterOp, const char* containerFileInterOp)
{
	std::string inputFile = inputFileInterOp;
	std::string containerFile = containerFileInterOp;
	if (inputFile.substr(inputFile.length() - 4, 4) == ".png")
	{
		return OutputBinaryToImg(ConvertImgToBinary(inputFile), containerFile, "./stenor_encoded.png");
	}
	else if (inputFile.substr(inputFile.length() - 4, 4) == ".wav")
	{
		return OutputBinaryToImg(ConvertAudioToBinary(inputFile), containerFile, "./stenor_encoded.png");
	}
	else if (inputFile.substr(inputFile.length() - 4, 4) == ".txt")
	{
		return OutputBinaryToImg(ConvertTextToBinary(inputFile), containerFile, "./stenor_encoded.png");
	}
	else return 1;
	return 0;
}

//DONE
int ParseBinaryInput(const char* filepathInterOp)
{
	std::string filepath = filepathInterOp;
	std::string binString = ReadBinaryFromImg(filepath);
	std::bitset<8> x = std::bitset<8>(binString.substr(0, 6));
	int stat = 0;
	switch (x.to_ulong())
	{
	case 56:
		stat = ConvertBinaryToImg(binString, "./stenor_decoded.png");
		break;
	case 63:
		stat = ConvertBinaryToAudio(binString, "./stenor_decoded.wav");
		break;
	default:
		stat = ConvertBinaryToText(binString, "./stenor_decoded.txt");
	}
	return stat;
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
int GetTxtSize(const char* pathToFileInterOp)
{
	std::string pathToFile = pathToFileInterOp;
	// convert text message to binary string
	std::ifstream input{ pathToFile };
	std::string text{ std::istreambuf_iterator <char>(input), std::istreambuf_iterator<char>() };

	return text.length() + 21;

	// if error, return 0
}
