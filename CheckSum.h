//
//  CheckSum.h
//  Wolf4SDL
//
//  Created by Ioan on 13.12.2012.
//
//

#ifndef CHECKSUM_H_
#define CHECKSUM_H_

void CalculateMapsegsChecksum();
void GetExploredData(void *exploredTarget);
void PutExploredData(const void *explored);

extern char digeststring[17];

#endif
