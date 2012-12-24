//
//  MapArrayFile.cpp
//  Wolf4SDL
//
//  Created by Ioan on 14.12.2012.
//
//

#include "ExploredArrayFile.h"

//
// ExploredArrayFile::ExploredArrayFile
//
ExploredArrayFile::ExploredArrayFile()
{
	memset(explored, 0, sizeof(explored));
	strcpy(header, DATAFILE_EXPLORED_HEADER);
}

//
// ExploredArrayFile::doWriteToFile
//
// Execute writing to file. How is it done?
// Bits are big-endian: first from the table is the MSB.
//
void ExploredArrayFile::doWriteToFile(FILE *f)
{
	fwrite(header, sizeof(char), FILE_HEADER_LENGTH, f);
	// for each component from explored, write the bits
	size_t pos;
	uint8_t charac = 0;

	boolean *baseaddress = &(explored[0][0]);

	for(pos = 0; pos < maparea; ++pos)
	{
		charac = (charac << 1) + *(baseaddress + pos);
		if((pos + 1) % 8 == 0)	// reached eight bits
		{
			fwrite(&charac, sizeof(uint8_t), 1, f);
			charac = 0;
		}
	}
//	fwrite(explored, sizeof(explored), 1, f);
}

//
// ExploredArrayFile::doReadFromFile
//
// Execute reading from file
//
bool ExploredArrayFile::doReadFromFile(FILE *f)
{
	int j;
	size_t pos;
	uint8_t mbyte;

	boolean *baseaddress = &(explored[0][0]);

	for(pos = 0; pos < maparea; pos += 8)
	{
		fread(&mbyte, sizeof(uint8_t), 1, f);
		for(j = 7; j >= 0; --j)
		{
			*(baseaddress + pos + j) = mbyte & 1;
			mbyte >>= 1;
		}
	}
	//fread(explored, sizeof(explored), 1, f);	// that's it for now
	return true;
}

//
// ExploredArraySize::getSize
//
// Return the file size
//
// As usual: header + content
//
uint64_t ExploredArrayFile::getSize()
{
	// FIXME: NOT ROBUST TO HAVE SIZE SEPARATE!
	return FILE_HEADER_LENGTH + (sizeof(explored) >> 3);
}

//
// ExploredArrayFile::getData
//
// copy explored data into target
//
void ExploredArrayFile::getData(void *putTarget) const
{
	memcpy(putTarget, explored, sizeof(explored));
}

//
// ExploredArrayFile::puttData
//
// copy explored data into target
//
void ExploredArrayFile::putData(const void *getSource)
{
	memcpy(explored, getSource, sizeof(explored));
}