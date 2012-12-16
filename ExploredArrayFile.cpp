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
	strcpy(header, "Explored");
}

//
// ExploredArrayFile::doWriteToFile
//
// Execute writing to file
//
void ExploredArrayFile::doWriteToFile(FILE *f)
{
	fputs(header, f);
	fwrite(explored, sizeof(explored), 1, f);
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
	return 8 + sizeof(explored);
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