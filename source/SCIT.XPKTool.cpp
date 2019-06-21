// SCIT.XPKTool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "filef.h"
#include <memory>
#include <filesystem>


int main(int argc, char* argv[])
{
	if (argc != 2) {
		std::cout << "Usage: xpktool <input>" << std::endl;
		return 1;
	}

	std::ifstream pFile(argv[1], std::ifstream::binary);

	if (!pFile) {
		std::cout << "ERROR: Could not open " << argv[1] << "!" << std::endl;
		return 1;
	}

	if (pFile)
	{
		int files;
		// get files amount
		pFile.read((char*)&files, sizeof(int));
		// skip some unknown int
		pFile.seekg(sizeof(int), pFile.cur);

		// skip string sizes
		for (int i = 0; i < files; i++) 
			pFile.seekg(sizeof(int), pFile.cur);
	 
		// get names
		std::unique_ptr<std::string[]> names = std::make_unique<std::string[]>(files);

		for (int i = 0; i < files; i++)
			std::getline(pFile, names[i], '\0');
		
		
		// skip archive size
		pFile.seekg(sizeof(int), pFile.cur);

		// get sizes
		std::unique_ptr<int[]> sizes = std::make_unique<int[]>(files);

		for (int i = 0; i < files; i++)
			pFile.read((char*)&sizes[i], sizeof(int));

		// skip unknown data
		pFile.seekg(sizeof(int) * files, pFile.cur);

		// get offsets
		std::unique_ptr<int[]> offsets = std::make_unique<int[]>(files);

		for (int i = 0; i < files; i++)
			pFile.read((char*)&offsets[i], sizeof(int));



		// extract data
		for (int i = 0; i < files; i++)
		{
			std::string folder = splitString(names[i], false);
			std::string name = splitString(names[i], true);
			if (checkSlash(names[i])) 
				std::experimental::filesystem::create_directory(folder);

			std::ofstream oFile(names[i], std::ofstream::binary);

			pFile.seekg(offsets[i], pFile.beg);
			std::cout << "Processing: " << names[i] << std::endl;
			auto dataSize = sizes[i];
			auto dataBuff = std::make_unique<char[]>(dataSize);
			pFile.read(dataBuff.get(), dataSize);
			oFile.write(dataBuff.get(), dataSize);

		}
		std::cout << "Finished." << std::endl;
	}
    return 0;
}

