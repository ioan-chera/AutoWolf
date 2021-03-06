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


#ifndef CHECKSUM_H_
#define CHECKSUM_H_

//
// CheckSum
//
// Namespace for checksums
//
class CheckSum
{
    uint8_t digeststring[16];
public:
    void CalculateMD5Checksum(const void *message, size_t messize);
    const uint8_t *DigestBuffer() const
    {
        return digeststring;
    }
    void GetHumanReadableHex(PString &dest) const;
};

extern CheckSum mapsegsChecksum;
void CalculateMapsegsChecksum();

#endif
