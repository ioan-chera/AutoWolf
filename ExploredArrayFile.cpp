//
// Copyright (C) 2013  Ioan Chera
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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
#ifdef DEBUG_EXPLORED
    int i, j;
    puts("---");
    for(i = 0; i < MAPSIZE; ++i)
    {
        for(j = 0; j < MAPSIZE; ++j)
        {
            printf("%c", explored[i][j] ? 'O' : ' ');
        }
        printf("\n");
    }
#endif
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