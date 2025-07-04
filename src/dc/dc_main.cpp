//Wolf4SDL\DC
//dc_main.cpp
//2009 - Cyle Terry

#if defined(_arch_dreamcast)

//TODO: Use Port A Only

#include <string.h>
#include "../wl_def.h"
#include "dc/video.h"
#include "kos/dbglog.h"
#include "kos/fs.h"
#include "zlib/zlib.h"

char dcwolf3dpath[1024];

void DC_CheckArguments() {
    FILE *fp;
    char *buf;
    char *result = NULL;

    bool  sampleRateGiven = false;
    bool  audioBufferGiven = false;
    int   length = 0;

    fp = fopen("/cd/data/wolf3d/args.txt", "r");
    if (!fp)
        return;
    fseek(fp, 0, SEEK_END);
    length = ftell (fp);
    fseek(fp, 0, SEEK_SET);
    buf = (char *)malloc(length + 2);
    fread(buf, 1, length, fp);
    buf[length] = 0;
    fclose(fp);
    result = strtok(buf, " ");

    while (result != NULL) {
#ifndef SPEAR
        if (!strcmp(result, "--goobers"))
#else
        if (!strcmp(result, "--debugmode"))
#endif
            param_debugmode = true;
        else if (!strcmp(result, "--baby"))
            param_difficulty = 0;
        else if (!strcmp(result, "--easy"))
            param_difficulty = 1;
        else if (!strcmp(result, "--normal"))
            param_difficulty = 2;
        else if (!strcmp(result, "--hard"))
            param_difficulty = 3;
        else if (!strcmp(result, "--nowait"))
            param_nowait = true;
        else if (!strcmp(result, "--tedlevel")) {
            result = strtok(NULL, " ");
            param_tedlevel = atoi(result);
        } else if (!strcmp(result, "--res")) {
            result = strtok(NULL, " ");
            screenWidth = atoi(result);
            result = strtok(NULL, " ");
            screenHeight = atoi(result);
            if ( screenWidth % 320 && screenHeight % 200) {
                dbglog(DBG_DEBUG, "Screen height\\width must be a multiple of 320x200\n");
                dbglog(DBG_DEBUG, "Defaulting to 320x200\n");
                screenWidth = 320;
                screenHeight = 200;
            }
        } else if (!strcmp(result, "--resf")) {
            result = strtok(NULL, " ");
            screenWidth = atoi(result);
            result = strtok(NULL, " ");
            screenHeight = atoi(result);
            if (screenWidth < 320 && screenHeight < 200) {
                dbglog(DBG_DEBUG, "Screen height\\width must be at least 320x200\n");
                dbglog(DBG_DEBUG, "Defaulting to 320x200\n");
                screenWidth = 320;
                screenHeight = 200;
            }
        } else if (!strcmp(result, "--bits")) {
            result = strtok(NULL, " ");
            screenBits = atoi(result);
            switch (screenBits) {
                case 8:
                case 16:
                case 24:
                case 32:
                    break;
                default:
                    dbglog(DBG_DEBUG, "Screen bits must be either 8, 16, 24 or 32\n");
                    dbglog(DBG_DEBUG, "Defaulting to 8\n");
                    screenBits = 8;
                    break;
            }
            result = strtok(NULL, " ");
            param_samplerate = atoi(result);
            sampleRateGiven = true;
        } else if (!strcmp(result, "--dblbuf")) {   //Was --nodblbuf
            usedoublebuffering = true;
        } else if (!strcmp(result, "--extravbls")) {
            result = strtok(NULL, " ");
            extravbls = atoi(result);
            if(extravbls < 0) {
                dbglog(DBG_DEBUG, "Extravbls must be positive!\n");
                dbglog(DBG_DEBUG, "Defaulting to 0\n");
                extravbls = 0;
            }
        } else if (!strcmp(result, "--samplerate")) {
            result = strtok(NULL, " ");
            param_samplerate = atoi(result);
            sampleRateGiven = true;
        } else if (!strcmp(result, "--audiobuffer")) {
            result = strtok(NULL, " ");
            param_audiobuffer = atoi(result);
            audioBufferGiven = true;
        } else if (!strcmp(result, "--goodtimes"))
            param_goodtimes = true;

        result = strtok(NULL, " ");
    }

    free(buf);

    if (sampleRateGiven && !audioBufferGiven)
        param_audiobuffer = 4096 / (44100 / param_samplerate);
}


int DC_CheckForMaps(char *path) {
    file_t dir;
    dirent_t *dirent;
    char fpath[1024];
    int disc_status;

    for(;;) {
        SDL_Delay(5);
        disc_status = DC_CheckDrive();
#ifdef SPEAR
        DC_DrawString(4, 1, "AutoSpr\\DC");	// IOAN 20130116: changed to AutoWolf
#else
        DC_DrawString(4, 1, "AutoWolf\\DC");
#endif
        switch(disc_status) {
            //case CD_STATUS_BUSY:
            //case CD_STATUS_OPEN:
            //    DC_DrawString(4, 6, "Please insert your Wolfenstein 3D CD.");
            //    break;
            default:
                dir = fs_open(path, O_DIR);
                while(dirent = fs_readdir(dir)) {
#ifdef SPEAR
#ifdef SPEARDEMO
                    if(!strcmp(dirent->name, "AUDIOHED.SDM")) {
                        fs_close(dir);
                        strcpy(dcwolf3dpath, path);
                        return 0;
                    }
#else
                    if(!strcmp(dirent->name, "AUDIOHED.SOD")) {
                        fs_close(dir);
                        strcpy(dcwolf3dpath, path);
                        param_mission = DC_SetMission(path);
                        return 0;
                    }
#endif
#else
#ifdef UPLOAD
                    if(!strcmp(dirent->name, "AUDIOHED.WL1")) {
                        fs_close(dir);
                        strcpy(dcwolf3dpath, path);
                        return 0;
                    }
#else
                    if(!strcmp(dirent->name, "AUDIOHED.WL6")) {
                        fs_close(dir);
                        strcpy(dcwolf3dpath, path);
                        return 0;
                    }
#endif
#endif
                    strcpy(fpath, path);
                    snprintf(fpath, sizeof(fpath), "%s/%s", fpath, dirent->name);
                    DC_CheckForMaps(fpath);
                }
                fs_close(dir);
                return -1;
        }
        DC_Flip();
    }
}

void DC_LoadMaps() {
    DC_CheckForMaps("/cd");
    DC_CLS();

    fs_chdir(dcwolf3dpath);

#ifdef SPEAR
#ifndef SPEARDEMO
	// IOAN 20130116: make them uppercase
	// FIXME: act case insensitive.
    fs_copy("AUDIOHED.SOD", "/ram/AUDIOHED.SOD");
    fs_copy("AUDIOT.SOD", "/ram/AUDIOT.SOD");
    fs_copy("VGADICT.SOD", "/ram/VGADICT.SOD");
    fs_copy("VGAGRAPH.SOD", "/ram/VGAGRAPH.SOD");
    fs_copy("VGAHEAD.SOD", "/ram/VGAHEAD.SOD");
    switch(param_mission) {
        case 0:
            fs_copy("GAMEMAPS.SOD", "/ram/GAMEMAPS.SOD");
            fs_copy("MAPHEAD.SOD", "/ram/MAPHEAD.SOD");
            fs_copy("VSWAP.SOD", "/ram/VSWAP.SOD");
            break;
        case 1:
            fs_copy("GAMEMAPS.SD1", "/ram/GAMEMAPS.SD1");
            fs_copy("MAPHEAD.SD1", "/ram/MAPHEAD.SD1");
            fs_copy("VSWAP.SD1", "/ram/VSWAP.SD1");
            break;
        case 2:
            fs_copy("GAMEMAPS.SD2", "/ram/GAMEMAPS.SD2");
            fs_copy("MAPHEAD.SD2", "/ram/MAPHEAD.SD2");
            fs_copy("VSWAP.SD2", "/ram/VSWAP.SD2");
            break;
        case 3:
            fs_copy("GAMEMAPS.SD3", "/ram/GAMEMAPS.SD3");
            fs_copy("MAPHEAD.SD3", "/ram/MAPHEAD.SD3");
            fs_copy("VSWAP.SD3", "/ram/VSWAP.SD3");
            break;
    }
#else
    fs_copy("AUDIOHED.SDM", "/ram/AUDIOHED.SDM");
    fs_copy("AUDIOT.SDM", "/ram/AUDIOT.SDM");
    fs_copy("VGADICT.SDM", "/ram/VGADICT.SDM");
    fs_copy("VGAGRAPH.SDM", "/ram/VGAGRAPH.SDM");
    fs_copy("VGAHEAD.SDM", "/ram/VGAHEAD.SDM");
    fs_copy("GAMEMAPS.SDM", "/ram/GAMEMAPS.SDM");
    fs_copy("MAPHEAD.SDM", "/ram/MAPHEAD.SDM");
    fs_copy("VSWAP.SDM", "/ram/VSWAP.SDM");
#endif
#else
#ifndef UPLOAD
    fs_copy("AUDIOHED.WL6", "/ram/AUDIOHED.WL6");
    fs_copy("AUDIOT.WL6", "/ram/AUDIOT.WL6");
    fs_copy("VGADICT.WL6", "/ram/VGADICT.WL6");
    fs_copy("VGAGRAPH.WL6", "/ram/VGAGRAPH.WL6");
    fs_copy("VGAHEAD.WL6", "/ram/VGAHEAD.WL6");
    fs_copy("GAMEMAPS.WL6", "/ram/GAMEMAPS.WL6");
    fs_copy("MAPHEAD.WL6", "/ram/MAPHEAD.WL6");
    fs_copy("VSWAP.WL6", "/ram/VSWAP.WL6");
#else
    fs_copy("AUDIOHED.WL1", "/ram/AUDIOHED.WL1");
    fs_copy("AUDIOT.WL1", "/ram/AUDIOT.WL1");
    fs_copy("VGADICT.WL1", "/ram/VGADICT.WL1");
    fs_copy("VGAGRAPH.WL1", "/ram/VGAGRAPH.WL1");
    fs_copy("VGAHEAD.WL1", "/ram/VGAHEAD.WL1");
    fs_copy("GAMEMAPS.WL1", "/ram/GAMEMAPS.WL1");
    fs_copy("maphead.WL1", "/ram/maphead.WL1");
    fs_copy("VSWAP.WL1", "/ram/VSWAP.WL1");
#endif
#endif

    fs_chdir("/ram");
}


void DC_Init() {
    DC_CheckArguments();
    DC_VideoInit();
    DC_LoadMaps();
    DC_CLS();
}

#ifdef  SPEAR
#ifndef SPEARDEMO

int DC_SetMission(char *path) {
    int mission1 = 0;
    int mission2 = 0;
    int mission3 = 0;
    int missions = 0;
    int last_mission = 0;
    int current_mission = 0;
    int previous_mission = 0;
    int font_y = 0;
    char fname[1024];
    bool finished = false;
    FILE *fp;

    sprintf(fname, "%s/MAPHEAD.SOD", path);
    fp = fopen(fname, "r");
    if(fp) {
        fclose(fp);
        last_mission = 1;
        mission1 = 1;
        missions++;
    }

    sprintf(fname, "%s/MAPHEAD.SD2", path);
    fp = fopen(fname, "r");
    if(fp) {
        fclose(fp);
        last_mission = 2;
        mission2 = 1;
        missions++;
    }

    sprintf(fname, "%s/MAPHEAD.SD3", path);
    fp = fopen(fname, "r");
    if(fp) {
        fclose(fp);
        last_mission = 3;
        mission3 = 1;
        missions++;
    }

    if(missions > 1) {

        while(!finished) {
            SDL_Delay(5);
            DC_CLS();

            DC_DrawString(2, 6 + current_mission, ">");

            font_y = 6;
            DC_DrawString(4, 1, "AutoSpr\\DC");	// IOAN 20130116: changed name
            if(mission1 == 1) {
                DC_DrawString(4, font_y, "Spear of Destiny (Original Mission)");
                font_y++;
            }
            if(mission2 == 1) {
                DC_DrawString(4, font_y, "Mission 2 - Return to Danger");
                font_y++;
            }
            if(mission3 == 1) {
                DC_DrawString(4, font_y, "Mission 3 - Ultimate Challenge");
                font_y++;
            }

            if(DC_ButtonPress(CONT_A)) {
                finished = true;
                break;
            } else if(DC_ButtonPress(CONT_DPAD_DOWN)) {
                current_mission++;
                previous_mission = -1;
                if(current_mission > (missions - 1))
                    current_mission = 0;
                DC_WaitButtonRelease(CONT_DPAD_DOWN);
            } else if(DC_ButtonPress(CONT_DPAD_UP)) {
                current_mission--;
                previous_mission = -1;
                if(current_mission < 0)
                    current_mission = missions - 1;
                DC_WaitButtonRelease(CONT_DPAD_UP);
            }
            DC_Flip();
        }

        /* Return Selected Mission */
        // XXX: What does this do? Are the fall throughs intended?!
        switch(current_mission) {
            case 1:
                if(mission1) return 1;
                if(mission2 && !mission1) return 2;
            case 2:
                if(mission2 && mission1) return 2;
                if(mission3 && mission1 && !mission2) return 3;
                if(mission3 && mission2 && !mission1) return 3;
            case 3:
                if(mission3 && mission2 && mission1) return 3;
        }
    }

    return last_mission;
}

#endif
#endif

#endif
