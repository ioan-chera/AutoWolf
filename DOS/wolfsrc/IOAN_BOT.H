#ifndef IOAN_BOT_H_
#define IOAN_BOT_H_

typedef struct SData_s
{
    int tilex, tiley;
    int f_score, g_score, h_score;
    int prev, next;
    boolean open;
} SData;

void Bot_DoCommand(void);
void Bot_MapInit(void);

#endif
