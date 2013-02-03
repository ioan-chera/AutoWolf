//
//  audioabstract.c
//  Wolf4SDL
//
//  Created by ioan on 03.02.2013.
//
//

#include "audioabstract.h"

#include "audiosod.h"
#include "audiowl6.h"

unsigned int *soundmap;

void MakeSoundMap()
{
     soundmap = new unsigned int[LASTSNDABSTRACT];
     
     soundmap[AHHHGSND] =           SPEAR ? AHHHGSND_sod        :AHHHGSND_wl6 ;
    soundmap[ANGELDEATHSND] =     ANGELDEATHSND_sod;
    soundmap[ANGELFIRESND] =      ANGELFIRESND_sod ;
    soundmap[ANGELSIGHTSND] =   ANGELSIGHTSND_sod;
    soundmap[ANGELTIREDSND] =     ANGELTIREDSND_sod  ;
     soundmap[ATKGATLINGSND] =      SPEAR ? ATKGATLINGSND_sod   :ATKGATLINGSND_wl6 ;
     soundmap[ATKKNIFESND] =        SPEAR ? ATKKNIFESND_sod     :ATKKNIFESND_wl6 ;
     soundmap[ATKMACHINEGUNSND] =   SPEAR ? ATKMACHINEGUNSND_sod:ATKMACHINEGUNSND_wl6 ;
     soundmap[ATKPISTOLSND] =       SPEAR ? ATKPISTOLSND_sod    :ATKPISTOLSND_wl6 ;
     soundmap[BONUS1SND] =          SPEAR ? BONUS1SND_sod       :BONUS1SND_wl6 ;
     soundmap[BONUS1UPSND] =        SPEAR ? BONUS1UPSND_sod     :BONUS1UPSND_wl6 ;
     soundmap[BONUS2SND] =          SPEAR ? BONUS2SND_sod       :BONUS2SND_wl6 ;
     soundmap[BONUS3SND] =          SPEAR ? BONUS3SND_sod       :BONUS3SND_wl6 ;
     soundmap[BONUS4SND] =          SPEAR ? BONUS4SND_sod       :BONUS4SND_wl6 ;
     soundmap[BOSSACTIVESND] =      SPEAR ? BOSSACTIVESND_sod   :BOSSACTIVESND_wl6 ;
     soundmap[BOSSFIRESND] =        SPEAR ? BOSSFIRESND_sod     : BOSSFIRESND_wl6 ;
     soundmap[CLOSEDOORSND] =       SPEAR ? CLOSEDOORSND_sod    :CLOSEDOORSND_wl6 ;
     soundmap[DEATHSCREAM1SND] =    SPEAR ? DEATHSCREAM1SND_sod :DEATHSCREAM1SND_wl6 ;
     soundmap[DEATHSCREAM2SND] =    SPEAR ? DEATHSCREAM2SND_sod :DEATHSCREAM2SND_wl6 ;
     soundmap[DEATHSCREAM3SND] =    SPEAR ? DEATHSCREAM3SND_sod :DEATHSCREAM3SND_wl6 ;
     soundmap[DEATHSCREAM4SND] =    SPEAR ? DEATHSCREAM4SND_sod :DEATHSCREAM4SND_wl6 ;
     soundmap[DEATHSCREAM5SND] =    SPEAR ? DEATHSCREAM5SND_sod :DEATHSCREAM5SND_wl6 ;
     soundmap[DEATHSCREAM6SND] =    SPEAR ? DEATHSCREAM6SND_sod :DEATHSCREAM6SND_wl6 ;
     soundmap[DEATHSCREAM7SND] =    SPEAR ? DEATHSCREAM7SND_sod :DEATHSCREAM7SND_wl6 ;
     soundmap[DEATHSCREAM8SND] =    SPEAR ? DEATHSCREAM8SND_sod :DEATHSCREAM8SND_wl6 ;
     soundmap[DEATHSCREAM9SND] =    SPEAR ? DEATHSCREAM9SND_sod :DEATHSCREAM9SND_wl6 ;
     soundmap[DIESND] =            DIESND_wl6 ;
     soundmap[DOGATTACKSND] =       SPEAR ? DOGATTACKSND_sod    :DOGATTACKSND_wl6 ;
     soundmap[DOGBARKSND] =         SPEAR ? DOGBARKSND_sod      :DOGBARKSND_wl6 ;
     soundmap[DOGDEATHSND] =        SPEAR ? DOGDEATHSND_sod     :DOGDEATHSND_wl6 ;
     soundmap[DONNERSND] =   DONNERSND_wl6 ;
     soundmap[DONOTHINGSND] =       SPEAR ? DONOTHINGSND_sod    :DONOTHINGSND_wl6 ;
     soundmap[EINESND] =     EINESND_wl6 ;
     soundmap[ENDBONUS1SND] =       SPEAR ? ENDBONUS1SND_sod    :ENDBONUS1SND_wl6 ;
     soundmap[ENDBONUS2SND] =       SPEAR ? ENDBONUS2SND_sod    :ENDBONUS2SND_wl6 ;
     soundmap[ERLAUBENSND] =     ERLAUBENSND_wl6 ;
     soundmap[ESCPRESSEDSND] =      SPEAR ? ESCPRESSEDSND_sod   :ESCPRESSEDSND_wl6 ;
     soundmap[EVASND] =    EVASND_wl6 ;
     soundmap[FLAMETHROWERSND] = FLAMETHROWERSND_wl6 ;
     soundmap[GAMEOVERSND] =        SPEAR ? GAMEOVERSND_sod     :GAMEOVERSND_wl6 ;
    soundmap[GETAMMOBOXSND] =    GETAMMOBOXSND_sod ;
     soundmap[GETAMMOSND] =         SPEAR ? GETAMMOSND_sod      :GETAMMOSND_wl6 ;
     soundmap[GETGATLINGSND] =      SPEAR ? GETGATLINGSND_sod   :GETGATLINGSND_wl6 ;
     soundmap[GETKEYSND] =          SPEAR ? GETKEYSND_sod       :GETKEYSND_wl6 ;
     soundmap[GETMACHINESND] =      SPEAR ? GETMACHINESND_sod   :GETMACHINESND_wl6 ;
    soundmap[GETSPEARSND] =       GETSPEARSND_sod  ;
    soundmap[GHOSTFADESND] =   GHOSTFADESND_sod ;
    soundmap[GHOSTSIGHTSND] =   GHOSTSIGHTSND_sod;
     soundmap[GOOBSSND] =    GOOBSSND_wl6 ;
     soundmap[GUTENTAGSND] =       GUTENTAGSND_wl6 ;
     soundmap[HALTSND] =            SPEAR ? HALTSND_sod         :HALTSND_wl6 ;
     soundmap[HEALTH1SND] =         SPEAR ? HEALTH1SND_sod      :HEALTH1SND_wl6 ;
     soundmap[HEALTH2SND] =         SPEAR ? HEALTH2SND_sod      :HEALTH2SND_wl6 ;
     soundmap[HEARTBEATSND] =      HEARTBEATSND_wl6 ;
     soundmap[HITENEMYSND] =        SPEAR ? HITENEMYSND_sod     :HITENEMYSND_wl6 ;
     soundmap[HITLERHASND] =        HITLERHASND_wl6 ;
     soundmap[HITWALLSND] =         SPEAR ? HITWALLSND_sod      :HITWALLSND_wl6 ;
     soundmap[KEINSND] =    KEINSND_wl6 ;
    soundmap[KNIGHTDEATHSND] =     KNIGHTDEATHSND_sod  ;
    soundmap[KNIGHTMISSILESND] =  KNIGHTMISSILESND_sod;
    soundmap[KNIGHTSIGHTSND] =     KNIGHTSIGHTSND_sod;
     soundmap[LEBENSND] =           SPEAR ? LEBENSND_sod        :LEBENSND_wl6 ;
     soundmap[LEVELDONESND] =       SPEAR ? LEVELDONESND_sod    :LEVELDONESND_wl6 ;
     soundmap[MECHSTEPSND] =  MECHSTEPSND_wl6 ;
     soundmap[MEINGOTTSND] =MEINGOTTSND_wl6 ;
     soundmap[MEINSND] =       MEINSND_wl6 ;
     soundmap[MISSILEFIRESND] =     SPEAR ? MISSILEFIRESND_sod  :MISSILEFIRESND_wl6 ;
     soundmap[MISSILEHITSND] =      SPEAR ? MISSILEHITSND_sod   :MISSILEHITSND_wl6 ;
     soundmap[MOVEGUN1SND] =        SPEAR ? MOVEGUN1SND_sod     :MOVEGUN1SND_wl6 ;
     soundmap[MOVEGUN2SND] =        SPEAR ? MOVEGUN2SND_sod     :MOVEGUN2SND_wl6 ;
     soundmap[MUTTISND] =          MUTTISND_wl6 ;
     soundmap[NAZIFIRESND] =        SPEAR ? NAZIFIRESND_sod     :NAZIFIRESND_wl6 ;
     soundmap[NAZIHITPLAYERSND] =   SPEAR ? NAZIHITPLAYERSND_sod:NAZIHITPLAYERSND_wl6 ;
     soundmap[NEINSOVASSND] =       SPEAR ? NEINSOVASSND_sod    :NEINSOVASSND_wl6 ;
     soundmap[NOBONUSSND] =         SPEAR ? NOBONUSSND_sod      :NOBONUSSND_wl6 ;
     soundmap[NOITEMSND] =          SPEAR ? NOITEMSND_sod       :NOITEMSND_wl6 ;
     soundmap[NOWAYSND] =           SPEAR ? NOWAYSND_sod        :NOWAYSND_wl6 ;
     soundmap[OPENDOORSND] =        SPEAR ? OPENDOORSND_sod     :OPENDOORSND_wl6 ;
     soundmap[PERCENT100SND] =      SPEAR ? PERCENT100SND_sod   :PERCENT100SND_wl6 ;
     soundmap[PLAYERDEATHSND] =     SPEAR ? PLAYERDEATHSND_sod  :PLAYERDEATHSND_wl6 ;
     soundmap[PUSHWALLSND] =        SPEAR ? PUSHWALLSND_sod     :PUSHWALLSND_wl6 ;
     soundmap[ROSESND] =       ROSESND_wl6 ;
     soundmap[SCHABBSHASND] =     SCHABBSHASND_wl6 ;
     soundmap[SCHABBSTHROWSND] = SCHABBSTHROWSND_wl6 ;
     soundmap[SCHEISTSND] =        SCHEISTSND_wl6 ;
     soundmap[SCHUTZADSND] =        SPEAR ? SCHUTZADSND_sod     :SCHUTZADSND_wl6 ;
     soundmap[SELECTITEMSND] =      SPEAR ? SELECTITEMSND_sod   :SELECTITEMSND_wl6 ;
     soundmap[SELECTWPNSND] =    SELECTWPNSND_wl6 ;
     soundmap[SHOOTDOORSND] =       SPEAR ? SHOOTDOORSND_sod    :SHOOTDOORSND_wl6 ;
     soundmap[SHOOTSND] =           SPEAR ? SHOOTSND_sod        :SHOOTSND_wl6 ;
     soundmap[SLURPIESND] =         SPEAR ? SLURPIESND_sod      :SLURPIESND_wl6 ;
     soundmap[SPIONSND] =           SPEAR ? SPIONSND_sod        :SPIONSND_wl6 ;
     soundmap[SSFIRESND] =          SPEAR ? SSFIRESND_sod       :SSFIRESND_wl6 ;
     soundmap[TAKEDAMAGESND] =      SPEAR ? TAKEDAMAGESND_sod   :TAKEDAMAGESND_wl6 ;
     soundmap[TOT_HUNDSND] =    TOT_HUNDSND_wl6 ;
     soundmap[TRANSDEATHSND] =       TRANSDEATHSND_sod  ;
     soundmap[TRANSSIGHTSND] =      TRANSSIGHTSND_sod    ;
     soundmap[UBERDEATHSND] =        UBERDEATHSND_sod    ;
     soundmap[WALK1SND] =           SPEAR ? WALK1SND_sod        :WALK1SND_wl6 ;
     soundmap[WALK2SND] =           SPEAR ? WALK2SND_sod        :WALK2SND_wl6 ;
     soundmap[WILHELMDEATHSND] =    WILHELMDEATHSND_sod ;
     soundmap[WILHELMSIGHTSND] =    WILHELMSIGHTSND_sod  ;
     soundmap[YEAHSND] =           YEAHSND_wl6 ;
}