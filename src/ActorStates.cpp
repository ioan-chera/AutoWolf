//
// Copyright (C) 1991-1992  id Software
// Copyright (C) 2007-2011  Moritz Kroll
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
////////////////////////////////////////////////////////////////////////////////
//
// Wolfenstein 3D actor states, moved here from wl_act2.cpp
//
////////////////////////////////////////////////////////////////////////////////

#include "wl_def.h"
#include "wl_act2.h"

#include "ActorStates.h"
//
// projectiles
//

statetype s_rocket              = {true,SPR_ROCKET_1,3,(statefunc)T_Projectile,(statefunc)A_Smoke,&s_rocket};

statetype s_smoke1              = {false,SPR_SMOKE_1,3,NULL,NULL,&s_smoke2};
statetype s_smoke2              = {false,SPR_SMOKE_2,3,NULL,NULL,&s_smoke3};
statetype s_smoke3              = {false,SPR_SMOKE_3,3,NULL,NULL,&s_smoke4};
statetype s_smoke4              = {false,SPR_SMOKE_4,3,NULL,NULL,NULL};

statetype s_boom1               = {false,SPR_BOOM_1,6,NULL,NULL,&s_boom2};
statetype s_boom2               = {false,SPR_BOOM_2,6,NULL,NULL,&s_boom3};
statetype s_boom3               = {false,SPR_BOOM_3,6,NULL,NULL,NULL};

statetype s_hrocket             = {true,SPR_HROCKET_1,3,(statefunc)T_Projectile,(statefunc)A_Smoke,&s_hrocket};

statetype s_hsmoke1             = {false,SPR_HSMOKE_1,3,NULL,NULL,&s_hsmoke2};
statetype s_hsmoke2             = {false,SPR_HSMOKE_2,3,NULL,NULL,&s_hsmoke3};
statetype s_hsmoke3             = {false,SPR_HSMOKE_3,3,NULL,NULL,&s_hsmoke4};
statetype s_hsmoke4             = {false,SPR_HSMOKE_4,3,NULL,NULL,NULL};

statetype s_hboom1              = {false,SPR_HBOOM_1,6,NULL,NULL,&s_hboom2};
statetype s_hboom2              = {false,SPR_HBOOM_2,6,NULL,NULL,&s_hboom3};
statetype s_hboom3              = {false,SPR_HBOOM_3,6,NULL,NULL,NULL};

statetype s_grdstand            = {true,SPR_GRD_S_1,0,(statefunc)T_Stand,NULL,&s_grdstand};
statetype s_grdpath1            = {true,SPR_GRD_W1_1,20,(statefunc)T_Path,NULL,&s_grdpath1s};
statetype s_grdpath1s           = {true,SPR_GRD_W1_1,5,NULL,NULL,&s_grdpath2};
statetype s_grdpath2            = {true,SPR_GRD_W2_1,15,(statefunc)T_Path,NULL,&s_grdpath3};
statetype s_grdpath3            = {true,SPR_GRD_W3_1,20,(statefunc)T_Path,NULL,&s_grdpath3s};
statetype s_grdpath3s           = {true,SPR_GRD_W3_1,5,NULL,NULL,&s_grdpath4};
statetype s_grdpath4            = {true,SPR_GRD_W4_1,15,(statefunc)T_Path,NULL,&s_grdpath1};
statetype s_grdpain             = {2,SPR_GRD_PAIN_1,10,NULL,NULL,&s_grdchase1};
statetype s_grdpain1            = {2,SPR_GRD_PAIN_2,10,NULL,NULL,&s_grdchase1};
statetype s_grdshoot1           = {false,SPR_GRD_SHOOT1,20,NULL,NULL,&s_grdshoot2};
statetype s_grdshoot2           = {false,SPR_GRD_SHOOT2,20,NULL,(statefunc)A_Shoot,&s_grdshoot3, STF_DAMAGING};
statetype s_grdshoot3           = {false,SPR_GRD_SHOOT3,20,NULL,NULL,&s_grdchase1};
statetype s_grdchase1           = {true,SPR_GRD_W1_1,10,(statefunc)T_Chase,NULL,&s_grdchase1s};
statetype s_grdchase1s          = {true,SPR_GRD_W1_1,3,NULL,NULL,&s_grdchase2};
statetype s_grdchase2           = {true,SPR_GRD_W2_1,8,(statefunc)T_Chase,NULL,&s_grdchase3};
statetype s_grdchase3           = {true,SPR_GRD_W3_1,10,(statefunc)T_Chase,NULL,&s_grdchase3s};
statetype s_grdchase3s          = {true,SPR_GRD_W3_1,3,NULL,NULL,&s_grdchase4};
statetype s_grdchase4           = {true,SPR_GRD_W4_1,8,(statefunc)T_Chase,NULL,&s_grdchase1};
statetype s_grddie1             = {false,SPR_GRD_DIE_1,15,NULL,(statefunc)A_DeathScream,&s_grddie2};
statetype s_grddie2             = {false,SPR_GRD_DIE_2,15,NULL,NULL,&s_grddie3};
statetype s_grddie3             = {false,SPR_GRD_DIE_3,15,NULL,NULL,&s_grddie4};
statetype s_grddie4             = {false,SPR_GRD_DEAD,0,NULL,NULL,&s_grddie4};

statetype s_blinkychase1        = {false,SPR_BLINKY_W1,10,(statefunc)T_Ghosts,NULL,&s_blinkychase2};
statetype s_blinkychase2        = {false,SPR_BLINKY_W2,10,(statefunc)T_Ghosts,NULL,&s_blinkychase1};
statetype s_inkychase1          = {false,SPR_INKY_W1,10,(statefunc)T_Ghosts,NULL,&s_inkychase2};
statetype s_inkychase2          = {false,SPR_INKY_W2,10,(statefunc)T_Ghosts,NULL,&s_inkychase1};
statetype s_pinkychase1         = {false,SPR_PINKY_W1,10,(statefunc)T_Ghosts,NULL,&s_pinkychase2};
statetype s_pinkychase2         = {false,SPR_PINKY_W2,10,(statefunc)T_Ghosts,NULL,&s_pinkychase1};
statetype s_clydechase1         = {false,SPR_CLYDE_W1,10,(statefunc)T_Ghosts,NULL,&s_clydechase2};
statetype s_clydechase2         = {false,SPR_CLYDE_W2,10,(statefunc)T_Ghosts,NULL,&s_clydechase1};

statetype s_dogpath1            = {true,SPR_DOG_W1_1,20,(statefunc)T_Path,NULL,&s_dogpath1s};
statetype s_dogpath1s           = {true,SPR_DOG_W1_1,5,NULL,NULL,&s_dogpath2};
statetype s_dogpath2            = {true,SPR_DOG_W2_1,15,(statefunc)T_Path,NULL,&s_dogpath3};
statetype s_dogpath3            = {true,SPR_DOG_W3_1,20,(statefunc)T_Path,NULL,&s_dogpath3s};
statetype s_dogpath3s           = {true,SPR_DOG_W3_1,5,NULL,NULL,&s_dogpath4};
statetype s_dogpath4            = {true,SPR_DOG_W4_1,15,(statefunc)T_Path,NULL,&s_dogpath1};
statetype s_dogjump1            = {false,SPR_DOG_JUMP1,10,NULL,NULL,&s_dogjump2};
statetype s_dogjump2            = {false,SPR_DOG_JUMP2,10,NULL,(statefunc)A_Bite,&s_dogjump3};
statetype s_dogjump3            = {false,SPR_DOG_JUMP3,10,NULL,NULL,&s_dogjump4};
statetype s_dogjump4            = {false,SPR_DOG_JUMP1,10,NULL,NULL,&s_dogjump5};
statetype s_dogjump5            = {false,SPR_DOG_W1_1,10,NULL,NULL,&s_dogchase1};
statetype s_dogchase1           = {true,SPR_DOG_W1_1,10,(statefunc)T_DogChase,NULL,&s_dogchase1s};
statetype s_dogchase1s          = {true,SPR_DOG_W1_1,3,NULL,NULL,&s_dogchase2};
statetype s_dogchase2           = {true,SPR_DOG_W2_1,8,(statefunc)T_DogChase,NULL,&s_dogchase3};
statetype s_dogchase3           = {true,SPR_DOG_W3_1,10,(statefunc)T_DogChase,NULL,&s_dogchase3s};
statetype s_dogchase3s          = {true,SPR_DOG_W3_1,3,NULL,NULL,&s_dogchase4};
statetype s_dogchase4           = {true,SPR_DOG_W4_1,8,(statefunc)T_DogChase,NULL,&s_dogchase1};
statetype s_dogdie1             = {false,SPR_DOG_DIE_1,15,NULL,(statefunc)A_DeathScream,&s_dogdie2};
statetype s_dogdie2             = {false,SPR_DOG_DIE_2,15,NULL,NULL,&s_dogdie3};
statetype s_dogdie3             = {false,SPR_DOG_DIE_3,15,NULL,NULL,&s_dogdead};
statetype s_dogdead             = {false,SPR_DOG_DEAD,15,NULL,NULL,&s_dogdead};

statetype s_ofcstand            = {true,SPR_OFC_S_1,0,(statefunc)T_Stand,NULL,&s_ofcstand};
statetype s_ofcpath1            = {true,SPR_OFC_W1_1,20,(statefunc)T_Path,NULL,&s_ofcpath1s};
statetype s_ofcpath1s           = {true,SPR_OFC_W1_1,5,NULL,NULL,&s_ofcpath2};
statetype s_ofcpath2            = {true,SPR_OFC_W2_1,15,(statefunc)T_Path,NULL,&s_ofcpath3};
statetype s_ofcpath3            = {true,SPR_OFC_W3_1,20,(statefunc)T_Path,NULL,&s_ofcpath3s};
statetype s_ofcpath3s           = {true,SPR_OFC_W3_1,5,NULL,NULL,&s_ofcpath4};
statetype s_ofcpath4            = {true,SPR_OFC_W4_1,15,(statefunc)T_Path,NULL,&s_ofcpath1};
statetype s_ofcpain             = {2,SPR_OFC_PAIN_1,10,NULL,NULL,&s_ofcchase1};
statetype s_ofcpain1            = {2,SPR_OFC_PAIN_2,10,NULL,NULL,&s_ofcchase1};
statetype s_ofcshoot1           = {false,SPR_OFC_SHOOT1,6,NULL,NULL,&s_ofcshoot2};
statetype s_ofcshoot2           = {false,SPR_OFC_SHOOT2,20,NULL,(statefunc)A_Shoot,&s_ofcshoot3,STF_DAMAGING};
statetype s_ofcshoot3           = {false,SPR_OFC_SHOOT3,10,NULL,NULL,&s_ofcchase1};
statetype s_ofcchase1           = {true,SPR_OFC_W1_1,10,(statefunc)T_Chase,NULL,&s_ofcchase1s};
statetype s_ofcchase1s          = {true,SPR_OFC_W1_1,3,NULL,NULL,&s_ofcchase2};
statetype s_ofcchase2           = {true,SPR_OFC_W2_1,8,(statefunc)T_Chase,NULL,&s_ofcchase3};
statetype s_ofcchase3           = {true,SPR_OFC_W3_1,10,(statefunc)T_Chase,NULL,&s_ofcchase3s};
statetype s_ofcchase3s          = {true,SPR_OFC_W3_1,3,NULL,NULL,&s_ofcchase4};
statetype s_ofcchase4           = {true,SPR_OFC_W4_1,8,(statefunc)T_Chase,NULL,&s_ofcchase1};
statetype s_ofcdie1             = {false,SPR_OFC_DIE_1,11,NULL,(statefunc)A_DeathScream,&s_ofcdie2};
statetype s_ofcdie2             = {false,SPR_OFC_DIE_2,11,NULL,NULL,&s_ofcdie3};
statetype s_ofcdie3             = {false,SPR_OFC_DIE_3,11,NULL,NULL,&s_ofcdie4};
statetype s_ofcdie4             = {false,SPR_OFC_DIE_4,11,NULL,NULL,&s_ofcdie5};
statetype s_ofcdie5             = {false,SPR_OFC_DEAD,0,NULL,NULL,&s_ofcdie5};

statetype s_mutstand            = {true,SPR_MUT_S_1,0,(statefunc)T_Stand,NULL,&s_mutstand};
statetype s_mutpath1            = {true,SPR_MUT_W1_1,20,(statefunc)T_Path,NULL,&s_mutpath1s};
statetype s_mutpath1s           = {true,SPR_MUT_W1_1,5,NULL,NULL,&s_mutpath2};
statetype s_mutpath2            = {true,SPR_MUT_W2_1,15,(statefunc)T_Path,NULL,&s_mutpath3};
statetype s_mutpath3            = {true,SPR_MUT_W3_1,20,(statefunc)T_Path,NULL,&s_mutpath3s};
statetype s_mutpath3s           = {true,SPR_MUT_W3_1,5,NULL,NULL,&s_mutpath4};
statetype s_mutpath4            = {true,SPR_MUT_W4_1,15,(statefunc)T_Path,NULL,&s_mutpath1};
statetype s_mutpain             = {2,SPR_MUT_PAIN_1,10,NULL,NULL,&s_mutchase1};
statetype s_mutpain1            = {2,SPR_MUT_PAIN_2,10,NULL,NULL,&s_mutchase1};
statetype s_mutshoot1           = {false,SPR_MUT_SHOOT1,6,NULL,(statefunc)A_Shoot,&s_mutshoot2,STF_DAMAGING};
statetype s_mutshoot2           = {false,SPR_MUT_SHOOT2,20,NULL,NULL,&s_mutshoot3,STF_DAMAGING};
statetype s_mutshoot3           = {false,SPR_MUT_SHOOT3,10,NULL,(statefunc)A_Shoot,&s_mutshoot4,STF_DAMAGING};
statetype s_mutshoot4           = {false,SPR_MUT_SHOOT4,20,NULL,NULL,&s_mutchase1};
statetype s_mutchase1           = {true,SPR_MUT_W1_1,10,(statefunc)T_Chase,NULL,&s_mutchase1s};
statetype s_mutchase1s          = {true,SPR_MUT_W1_1,3,NULL,NULL,&s_mutchase2};
statetype s_mutchase2           = {true,SPR_MUT_W2_1,8,(statefunc)T_Chase,NULL,&s_mutchase3};
statetype s_mutchase3           = {true,SPR_MUT_W3_1,10,(statefunc)T_Chase,NULL,&s_mutchase3s};
statetype s_mutchase3s          = {true,SPR_MUT_W3_1,3,NULL,NULL,&s_mutchase4};
statetype s_mutchase4           = {true,SPR_MUT_W4_1,8,(statefunc)T_Chase,NULL,&s_mutchase1};
statetype s_mutdie1             = {false,SPR_MUT_DIE_1,7,NULL,(statefunc)A_DeathScream,&s_mutdie2};
statetype s_mutdie2             = {false,SPR_MUT_DIE_2,7,NULL,NULL,&s_mutdie3};
statetype s_mutdie3             = {false,SPR_MUT_DIE_3,7,NULL,NULL,&s_mutdie4};
statetype s_mutdie4             = {false,SPR_MUT_DIE_4,7,NULL,NULL,&s_mutdie5};
statetype s_mutdie5             = {false,SPR_MUT_DEAD,0,NULL,NULL,&s_mutdie5};

statetype s_ssstand             = {true,SPR_SS_S_1,0,(statefunc)T_Stand,NULL,&s_ssstand};
statetype s_sspath1             = {true,SPR_SS_W1_1,20,(statefunc)T_Path,NULL,&s_sspath1s};
statetype s_sspath1s            = {true,SPR_SS_W1_1,5,NULL,NULL,&s_sspath2};
statetype s_sspath2             = {true,SPR_SS_W2_1,15,(statefunc)T_Path,NULL,&s_sspath3};
statetype s_sspath3             = {true,SPR_SS_W3_1,20,(statefunc)T_Path,NULL,&s_sspath3s};
statetype s_sspath3s            = {true,SPR_SS_W3_1,5,NULL,NULL,&s_sspath4};
statetype s_sspath4             = {true,SPR_SS_W4_1,15,(statefunc)T_Path,NULL,&s_sspath1};
statetype s_sspain              = {2,SPR_SS_PAIN_1,10,NULL,NULL,&s_sschase1};
statetype s_sspain1             = {2,SPR_SS_PAIN_2,10,NULL,NULL,&s_sschase1};
statetype s_ssshoot1            = {false,SPR_SS_SHOOT1,20,NULL,NULL,&s_ssshoot2};
statetype s_ssshoot2            = {false,SPR_SS_SHOOT2,20,NULL,(statefunc)A_Shoot,&s_ssshoot3,STF_DAMAGING};
statetype s_ssshoot3            = {false,SPR_SS_SHOOT3,10,NULL,NULL,&s_ssshoot4,STF_DAMAGING};
statetype s_ssshoot4            = {false,SPR_SS_SHOOT2,10,NULL,(statefunc)A_Shoot,&s_ssshoot5,STF_DAMAGING};
statetype s_ssshoot5            = {false,SPR_SS_SHOOT3,10,NULL,NULL,&s_ssshoot6,STF_DAMAGING};
statetype s_ssshoot6            = {false,SPR_SS_SHOOT2,10,NULL,(statefunc)A_Shoot,&s_ssshoot7,STF_DAMAGING};
statetype s_ssshoot7            = {false,SPR_SS_SHOOT3,10,NULL,NULL,&s_ssshoot8,STF_DAMAGING};
statetype s_ssshoot8            = {false,SPR_SS_SHOOT2,10,NULL,(statefunc)A_Shoot,&s_ssshoot9,STF_DAMAGING};
statetype s_ssshoot9            = {false,SPR_SS_SHOOT3,10,NULL,NULL,&s_sschase1};
statetype s_sschase1            = {true,SPR_SS_W1_1,10,(statefunc)T_Chase,NULL,&s_sschase1s};
statetype s_sschase1s           = {true,SPR_SS_W1_1,3,NULL,NULL,&s_sschase2};
statetype s_sschase2            = {true,SPR_SS_W2_1,8,(statefunc)T_Chase,NULL,&s_sschase3};
statetype s_sschase3            = {true,SPR_SS_W3_1,10,(statefunc)T_Chase,NULL,&s_sschase3s};
statetype s_sschase3s           = {true,SPR_SS_W3_1,3,NULL,NULL,&s_sschase4};
statetype s_sschase4            = {true,SPR_SS_W4_1,8,(statefunc)T_Chase,NULL,&s_sschase1};
statetype s_ssdie1              = {false,SPR_SS_DIE_1,15,NULL,(statefunc)A_DeathScream,&s_ssdie2};
statetype s_ssdie2              = {false,SPR_SS_DIE_2,15,NULL,NULL,&s_ssdie3};
statetype s_ssdie3              = {false,SPR_SS_DIE_3,15,NULL,NULL,&s_ssdie4};
statetype s_ssdie4              = {false,SPR_SS_DEAD,0,NULL,NULL,&s_ssdie4};

statetype s_bossstand           = {false,SPR_BOSS_W1,0,(statefunc)T_Stand,NULL,&s_bossstand};
statetype s_bosschase1          = {false,SPR_BOSS_W1,10,(statefunc)T_Chase,NULL,&s_bosschase1s};
statetype s_bosschase1s         = {false,SPR_BOSS_W1,3,NULL,NULL,&s_bosschase2};
statetype s_bosschase2          = {false,SPR_BOSS_W2,8,(statefunc)T_Chase,NULL,&s_bosschase3};
statetype s_bosschase3          = {false,SPR_BOSS_W3,10,(statefunc)T_Chase,NULL,&s_bosschase3s};
statetype s_bosschase3s         = {false,SPR_BOSS_W3,3,NULL,NULL,&s_bosschase4};
statetype s_bosschase4          = {false,SPR_BOSS_W4,8,(statefunc)T_Chase,NULL,&s_bosschase1};
statetype s_bossdie1            = {false,SPR_BOSS_DIE1,15,NULL,(statefunc)A_DeathScream,&s_bossdie2};
statetype s_bossdie2            = {false,SPR_BOSS_DIE2,15,NULL,NULL,&s_bossdie3};
statetype s_bossdie3            = {false,SPR_BOSS_DIE3,15,NULL,NULL,&s_bossdie4};
statetype s_bossdie4            = {false,SPR_BOSS_DEAD,0,NULL,NULL,&s_bossdie4};
statetype s_bossshoot1          = {false,SPR_BOSS_SHOOT1,20,NULL,NULL,&s_bossshoot1b};
statetype s_bossshoot1b         = {false,SPR_BOSS_SHOOT1,10,NULL,NULL,&s_bossshoot2,STF_DAMAGING};
statetype s_bossshoot2          = {false,SPR_BOSS_SHOOT2,10,NULL,(statefunc)A_Shoot,&s_bossshoot3,STF_DAMAGING};
statetype s_bossshoot3          = {false,SPR_BOSS_SHOOT3,10,NULL,(statefunc)A_Shoot,&s_bossshoot4,STF_DAMAGING};
statetype s_bossshoot4          = {false,SPR_BOSS_SHOOT2,10,NULL,(statefunc)A_Shoot,&s_bossshoot5,STF_DAMAGING};
statetype s_bossshoot5          = {false,SPR_BOSS_SHOOT3,10,NULL,(statefunc)A_Shoot,&s_bossshoot6,STF_DAMAGING};
statetype s_bossshoot6          = {false,SPR_BOSS_SHOOT2,10,NULL,(statefunc)A_Shoot,&s_bossshoot7,STF_DAMAGING};
statetype s_bossshoot7          = {false,SPR_BOSS_SHOOT3,10,NULL,(statefunc)A_Shoot,&s_bossshoot8,STF_DAMAGING};
statetype s_bossshoot8          = {false,SPR_BOSS_SHOOT1,10,NULL,NULL,&s_bosschase1};

statetype s_gretelstand         = {false,SPR_GRETEL_W1,0,(statefunc)T_Stand,NULL,&s_gretelstand};
statetype s_gretelchase1        = {false,SPR_GRETEL_W1,10,(statefunc)T_Chase,NULL,&s_gretelchase1s};
statetype s_gretelchase1s       = {false,SPR_GRETEL_W1,3,NULL,NULL,&s_gretelchase2};
statetype s_gretelchase2        = {false,SPR_GRETEL_W2,8,(statefunc)T_Chase,NULL,&s_gretelchase3};
statetype s_gretelchase3        = {false,SPR_GRETEL_W3,10,(statefunc)T_Chase,NULL,&s_gretelchase3s};
statetype s_gretelchase3s       = {false,SPR_GRETEL_W3,3,NULL,NULL,&s_gretelchase4};
statetype s_gretelchase4        = {false,SPR_GRETEL_W4,8,(statefunc)T_Chase,NULL,&s_gretelchase1};
statetype s_greteldie1          = {false,SPR_GRETEL_DIE1,15,NULL,(statefunc)A_DeathScream,&s_greteldie2};
statetype s_greteldie2          = {false,SPR_GRETEL_DIE2,15,NULL,NULL,&s_greteldie3};
statetype s_greteldie3          = {false,SPR_GRETEL_DIE3,15,NULL,NULL,&s_greteldie4};
statetype s_greteldie4          = {false,SPR_GRETEL_DEAD,0,NULL,NULL,&s_greteldie4};
statetype s_gretelshoot1        = {false,SPR_GRETEL_SHOOT1,20,NULL,NULL,&s_gretelshoot1b};
statetype s_gretelshoot1b       = {false,SPR_GRETEL_SHOOT1,10,NULL,NULL,&s_gretelshoot2,STF_DAMAGING};
statetype s_gretelshoot2        = {false,SPR_GRETEL_SHOOT2,10,NULL,(statefunc)A_Shoot,&s_gretelshoot3,STF_DAMAGING};
statetype s_gretelshoot3        = {false,SPR_GRETEL_SHOOT3,10,NULL,(statefunc)A_Shoot,&s_gretelshoot4,STF_DAMAGING};
statetype s_gretelshoot4        = {false,SPR_GRETEL_SHOOT2,10,NULL,(statefunc)A_Shoot,&s_gretelshoot5,STF_DAMAGING};
statetype s_gretelshoot5        = {false,SPR_GRETEL_SHOOT3,10,NULL,(statefunc)A_Shoot,&s_gretelshoot6,STF_DAMAGING};
statetype s_gretelshoot6        = {false,SPR_GRETEL_SHOOT2,10,NULL,(statefunc)A_Shoot,&s_gretelshoot7,STF_DAMAGING};
statetype s_gretelshoot7        = {false,SPR_GRETEL_SHOOT3,10,NULL,(statefunc)A_Shoot,&s_gretelshoot8,STF_DAMAGING};
statetype s_gretelshoot8        = {false,SPR_GRETEL_SHOOT1,10,NULL,NULL,&s_gretelchase1};

statetype s_transstand          = {false,SPR_TRANS_W1,0,(statefunc)T_Stand,NULL,&s_transstand};
statetype s_transchase1         = {false,SPR_TRANS_W1,10,(statefunc)T_Chase,NULL,&s_transchase1s};
statetype s_transchase1s        = {false,SPR_TRANS_W1,3,NULL,NULL,&s_transchase2};
statetype s_transchase2         = {false,SPR_TRANS_W2,8,(statefunc)T_Chase,NULL,&s_transchase3};
statetype s_transchase3         = {false,SPR_TRANS_W3,10,(statefunc)T_Chase,NULL,&s_transchase3s};
statetype s_transchase3s        = {false,SPR_TRANS_W3,3,NULL,NULL,&s_transchase4};
statetype s_transchase4         = {false,SPR_TRANS_W4,8,(statefunc)T_Chase,NULL,&s_transchase1};
statetype s_transdie0           = {false,SPR_TRANS_W1,1,NULL,(statefunc)A_DeathScream,&s_transdie01};
statetype s_transdie01          = {false,SPR_TRANS_W1,1,NULL,NULL,&s_transdie1};
statetype s_transdie1           = {false,SPR_TRANS_DIE1,15,NULL,NULL,&s_transdie2};
statetype s_transdie2           = {false,SPR_TRANS_DIE2,15,NULL,NULL,&s_transdie3};
statetype s_transdie3           = {false,SPR_TRANS_DIE3,15,NULL,NULL,&s_transdie4};
statetype s_transdie4           = {false,SPR_TRANS_DEAD,0,NULL,NULL,&s_transdie4};
statetype s_transshoot1         = {false,SPR_TRANS_SHOOT1,20,NULL,NULL,&s_transshoot1b};
statetype s_transshoot1b        = {false,SPR_TRANS_SHOOT1,10,NULL,NULL,&s_transshoot2,STF_DAMAGING};
statetype s_transshoot2         = {false,SPR_TRANS_SHOOT2,10,NULL,(statefunc)A_Shoot,&s_transshoot3,STF_DAMAGING};
statetype s_transshoot3         = {false,SPR_TRANS_SHOOT3,10,NULL,(statefunc)A_Shoot,&s_transshoot4,STF_DAMAGING};
statetype s_transshoot4         = {false,SPR_TRANS_SHOOT2,10,NULL,(statefunc)A_Shoot,&s_transshoot5,STF_DAMAGING};
statetype s_transshoot5         = {false,SPR_TRANS_SHOOT3,10,NULL,(statefunc)A_Shoot,&s_transshoot6,STF_DAMAGING};
statetype s_transshoot6         = {false,SPR_TRANS_SHOOT2,10,NULL,(statefunc)A_Shoot,&s_transshoot7,STF_DAMAGING};
statetype s_transshoot7         = {false,SPR_TRANS_SHOOT3,10,NULL,(statefunc)A_Shoot,&s_transshoot8,STF_DAMAGING};
statetype s_transshoot8         = {false,SPR_TRANS_SHOOT1,10,NULL,NULL,&s_transchase1};

statetype s_uberstand           = {false,SPR_UBER_W1,0,(statefunc)T_Stand,NULL,&s_uberstand};
statetype s_uberchase1          = {false,SPR_UBER_W1,10,(statefunc)T_Chase,NULL,&s_uberchase1s};
statetype s_uberchase1s         = {false,SPR_UBER_W1,3,NULL,NULL,&s_uberchase2};
statetype s_uberchase2          = {false,SPR_UBER_W2,8,(statefunc)T_Chase,NULL,&s_uberchase3};
statetype s_uberchase3          = {false,SPR_UBER_W3,10,(statefunc)T_Chase,NULL,&s_uberchase3s};
statetype s_uberchase3s         = {false,SPR_UBER_W3,3,NULL,NULL,&s_uberchase4};
statetype s_uberchase4          = {false,SPR_UBER_W4,8,(statefunc)T_Chase,NULL,&s_uberchase1};
statetype s_uberdie0            = {false,SPR_UBER_W1,1,NULL,(statefunc)A_DeathScream,&s_uberdie01};
statetype s_uberdie01           = {false,SPR_UBER_W1,1,NULL,NULL,&s_uberdie1};
statetype s_uberdie1            = {false,SPR_UBER_DIE1,15,NULL,NULL,&s_uberdie2};
statetype s_uberdie2            = {false,SPR_UBER_DIE2,15,NULL,NULL,&s_uberdie3};
statetype s_uberdie3            = {false,SPR_UBER_DIE3,15,NULL,NULL,&s_uberdie4};
statetype s_uberdie4            = {false,SPR_UBER_DIE4,15,NULL,NULL,&s_uberdie5};
statetype s_uberdie5            = {false,SPR_UBER_DEAD,0,NULL,NULL,&s_uberdie5};
statetype s_ubershoot1          = {false,SPR_UBER_SHOOT1,22,NULL,NULL,&s_ubershoot1b};
statetype s_ubershoot1b         = {false,SPR_UBER_SHOOT1,8, NULL,NULL,&s_ubershoot2,STF_DAMAGING};
statetype s_ubershoot2          = {false,SPR_UBER_SHOOT2,12,NULL,(statefunc)A_UShoot,&s_ubershoot3,STF_DAMAGING};
statetype s_ubershoot3          = {false,SPR_UBER_SHOOT3,12,NULL,(statefunc)A_UShoot,&s_ubershoot4,STF_DAMAGING};
statetype s_ubershoot4          = {false,SPR_UBER_SHOOT4,12,NULL,(statefunc)A_UShoot,&s_ubershoot5,STF_DAMAGING};
statetype s_ubershoot5          = {false,SPR_UBER_SHOOT3,12,NULL,(statefunc)A_UShoot,&s_ubershoot6,STF_DAMAGING};
statetype s_ubershoot6          = {false,SPR_UBER_SHOOT2,12,NULL,(statefunc)A_UShoot,&s_ubershoot7,STF_DAMAGING};
statetype s_ubershoot7          = {false,SPR_UBER_SHOOT1,12,NULL,NULL,&s_uberchase1};

statetype s_willstand           = {false,SPR_WILL_W1,0,(statefunc)T_Stand,NULL,&s_willstand};
statetype s_willchase1          = {false,SPR_WILL_W1,10,(statefunc)T_ProjectileBossChase,NULL,&s_willchase1s};
statetype s_willchase1s         = {false,SPR_WILL_W1,3,NULL,NULL,&s_willchase2};
statetype s_willchase2          = {false,SPR_WILL_W2,8,(statefunc)T_ProjectileBossChase,NULL,&s_willchase3};
statetype s_willchase3          = {false,SPR_WILL_W3,10,(statefunc)T_ProjectileBossChase,NULL,&s_willchase3s};
statetype s_willchase3s         = {false,SPR_WILL_W3,3,NULL,NULL,&s_willchase4};
statetype s_willchase4          = {false,SPR_WILL_W4,8,(statefunc)T_ProjectileBossChase,NULL,&s_willchase1};
statetype s_willdeathcam        = {false,SPR_WILL_W1,1,NULL,NULL,&s_willdie1};
statetype s_willdie1            = {false,SPR_WILL_W1,1,NULL,(statefunc)A_DeathScream,&s_willdie2};
statetype s_willdie2            = {false,SPR_WILL_W1,10,NULL,NULL,&s_willdie3};
statetype s_willdie3            = {false,SPR_WILL_DIE1,10,NULL,NULL,&s_willdie4};
statetype s_willdie4            = {false,SPR_WILL_DIE2,10,NULL,NULL,&s_willdie5};
statetype s_willdie5            = {false,SPR_WILL_DIE3,10,NULL,NULL,&s_willdie6};
statetype s_willdie6            = {false,SPR_WILL_DEAD,20,NULL,NULL,&s_willdie6};
statetype s_willshoot1          = {false,SPR_WILL_SHOOT1,30,NULL,NULL,&s_willshoot2};
statetype s_willshoot2          = {false,SPR_WILL_SHOOT2,10,NULL,(statefunc)A_Launch,&s_willshoot3,STF_DAMAGING};
statetype s_willshoot3          = {false,SPR_WILL_SHOOT3,10,NULL,(statefunc)A_Shoot,&s_willshoot4,STF_DAMAGING};
statetype s_willshoot4          = {false,SPR_WILL_SHOOT4,10,NULL,(statefunc)A_Shoot,&s_willshoot5,STF_DAMAGING};
statetype s_willshoot5          = {false,SPR_WILL_SHOOT3,10,NULL,(statefunc)A_Shoot,&s_willshoot6,STF_DAMAGING};
statetype s_willshoot6          = {false,SPR_WILL_SHOOT4,10,NULL,(statefunc)A_Shoot,&s_willchase1,STF_DAMAGING};

statetype s_deathstand          = {false,SPR_DEATH_W1,0,(statefunc)T_Stand,NULL,&s_deathstand};
statetype s_deathchase1         = {false,SPR_DEATH_W1,10,(statefunc)T_ProjectileBossChase,NULL,&s_deathchase1s};
statetype s_deathchase1s        = {false,SPR_DEATH_W1,3,NULL,NULL,&s_deathchase2};
statetype s_deathchase2         = {false,SPR_DEATH_W2,8,(statefunc)T_ProjectileBossChase,NULL,&s_deathchase3};
statetype s_deathchase3         = {false,SPR_DEATH_W3,10,(statefunc)T_ProjectileBossChase,NULL,&s_deathchase3s};
statetype s_deathchase3s        = {false,SPR_DEATH_W3,3,NULL,NULL,&s_deathchase4};
statetype s_deathchase4         = {false,SPR_DEATH_W4,8,(statefunc)T_ProjectileBossChase,NULL,&s_deathchase1};
statetype s_deathdeathcam       = {false,SPR_DEATH_W1,1,NULL,NULL,&s_deathdie1};
statetype s_deathdie1           = {false,SPR_DEATH_W1,1,NULL,(statefunc)A_DeathScream,&s_deathdie2};
statetype s_deathdie2           = {false,SPR_DEATH_W1,10,NULL,NULL,&s_deathdie3};
statetype s_deathdie3           = {false,SPR_DEATH_DIE1,10,NULL,NULL,&s_deathdie4};
statetype s_deathdie4           = {false,SPR_DEATH_DIE2,10,NULL,NULL,&s_deathdie5};
statetype s_deathdie5           = {false,SPR_DEATH_DIE3,10,NULL,NULL,&s_deathdie6};
statetype s_deathdie6           = {false,SPR_DEATH_DIE4,10,NULL,NULL,&s_deathdie7};
statetype s_deathdie7           = {false,SPR_DEATH_DIE5,10,NULL,NULL,&s_deathdie8};
statetype s_deathdie8           = {false,SPR_DEATH_DIE6,10,NULL,NULL,&s_deathdie9};
statetype s_deathdie9           = {false,SPR_DEATH_DEAD,0,NULL,NULL,&s_deathdie9};
statetype s_deathshoot1         = {false,SPR_DEATH_SHOOT1,30,NULL,NULL,&s_deathshoot2};
statetype s_deathshoot2         = {false,SPR_DEATH_SHOOT2,10,NULL,(statefunc)A_Launch,&s_deathshoot3,STF_DAMAGING};
statetype s_deathshoot3         = {false,SPR_DEATH_SHOOT4,10,NULL,(statefunc)A_Shoot,&s_deathshoot4,STF_DAMAGING};
statetype s_deathshoot4         = {false,SPR_DEATH_SHOOT3,10,NULL,(statefunc)A_Launch,&s_deathshoot5,STF_DAMAGING};
statetype s_deathshoot5         = {false,SPR_DEATH_SHOOT4,10,NULL,(statefunc)A_Shoot,&s_deathchase1,STF_DAMAGING};

statetype s_angelstand          = {false,SPR_ANGEL_W1,0,(statefunc)T_Stand,NULL,&s_angelstand};
statetype s_angelchase1         = {false,SPR_ANGEL_W1,10,(statefunc)T_ProjectileBossChase,NULL,&s_angelchase1s};
statetype s_angelchase1s        = {false,SPR_ANGEL_W1,3,NULL,NULL,&s_angelchase2};
statetype s_angelchase2         = {false,SPR_ANGEL_W2,8,(statefunc)T_ProjectileBossChase,NULL,&s_angelchase3};
statetype s_angelchase3         = {false,SPR_ANGEL_W3,10,(statefunc)T_ProjectileBossChase,NULL,&s_angelchase3s};
statetype s_angelchase3s        = {false,SPR_ANGEL_W3,3,NULL,NULL,&s_angelchase4};
statetype s_angelchase4         = {false,SPR_ANGEL_W4,8,(statefunc)T_ProjectileBossChase,NULL,&s_angelchase1};
statetype s_angeldie1           = {false,SPR_ANGEL_W1,1,NULL,(statefunc)A_DeathScream,&s_angeldie11};
statetype s_angeldie11          = {false,SPR_ANGEL_W1,1,NULL,NULL,&s_angeldie2};
statetype s_angeldie2           = {false,SPR_ANGEL_DIE1,10,NULL,(statefunc)A_Slurpie,&s_angeldie3};
statetype s_angeldie3           = {false,SPR_ANGEL_DIE2,10,NULL,NULL,&s_angeldie4};
statetype s_angeldie4           = {false,SPR_ANGEL_DIE3,10,NULL,NULL,&s_angeldie5};
statetype s_angeldie5           = {false,SPR_ANGEL_DIE4,10,NULL,NULL,&s_angeldie6};
statetype s_angeldie6           = {false,SPR_ANGEL_DIE5,10,NULL,NULL,&s_angeldie7};
statetype s_angeldie7           = {false,SPR_ANGEL_DIE6,10,NULL,NULL,&s_angeldie8};
statetype s_angeldie8           = {false,SPR_ANGEL_DIE7,10,NULL,NULL,&s_angeldie9};
statetype s_angeldie9           = {false,SPR_ANGEL_DEAD,130,NULL,(statefunc)A_Victory,&s_angeldie9};
statetype s_angelshoot1         = {false,SPR_ANGEL_SHOOT1,10,NULL,(statefunc)A_StartAttack,&s_angelshoot2};
statetype s_angelshoot2         = {false,SPR_ANGEL_SHOOT2,20,NULL,(statefunc)A_Launch,&s_angelshoot3};
statetype s_angelshoot3         = {false,SPR_ANGEL_SHOOT1,10,NULL,(statefunc)A_Relaunch,&s_angelshoot2};
statetype s_angeltired          = {false,SPR_ANGEL_TIRED1,40,NULL,(statefunc)A_Breathing,&s_angeltired2};
statetype s_angeltired2         = {false,SPR_ANGEL_TIRED2,40,NULL,NULL,&s_angeltired3};
statetype s_angeltired3         = {false,SPR_ANGEL_TIRED1,40,NULL,(statefunc)A_Breathing,&s_angeltired4};
statetype s_angeltired4         = {false,SPR_ANGEL_TIRED2,40,NULL,NULL,&s_angeltired5};
statetype s_angeltired5         = {false,SPR_ANGEL_TIRED1,40,NULL,(statefunc)A_Breathing,&s_angeltired6};
statetype s_angeltired6         = {false,SPR_ANGEL_TIRED2,40,NULL,NULL,&s_angeltired7};
statetype s_angeltired7         = {false,SPR_ANGEL_TIRED1,40,NULL,(statefunc)A_Breathing,&s_angelchase1};

statetype s_spark1              = {false,SPR_SPARK1,6,(statefunc)T_Projectile,NULL,&s_spark2};
statetype s_spark2              = {false,SPR_SPARK2,6,(statefunc)T_Projectile,NULL,&s_spark3};
statetype s_spark3              = {false,SPR_SPARK3,6,(statefunc)T_Projectile,NULL,&s_spark4};
statetype s_spark4              = {false,SPR_SPARK4,6,(statefunc)T_Projectile,NULL,&s_spark1};

statetype s_spectrewait1        = {false,SPR_SPECTRE_W1,10,(statefunc)T_Stand,NULL,&s_spectrewait2};
statetype s_spectrewait2        = {false,SPR_SPECTRE_W2,10,(statefunc)T_Stand,NULL,&s_spectrewait3};
statetype s_spectrewait3        = {false,SPR_SPECTRE_W3,10,(statefunc)T_Stand,NULL,&s_spectrewait4};
statetype s_spectrewait4        = {false,SPR_SPECTRE_W4,10,(statefunc)T_Stand,NULL,&s_spectrewait1};
statetype s_spectrechase1       = {false,SPR_SPECTRE_W1,10,(statefunc)T_Ghosts,NULL,&s_spectrechase2};
statetype s_spectrechase2       = {false,SPR_SPECTRE_W2,10,(statefunc)T_Ghosts,NULL,&s_spectrechase3};
statetype s_spectrechase3       = {false,SPR_SPECTRE_W3,10,(statefunc)T_Ghosts,NULL,&s_spectrechase4};
statetype s_spectrechase4       = {false,SPR_SPECTRE_W4,10,(statefunc)T_Ghosts,NULL,&s_spectrechase1};
statetype s_spectredie1         = {false,SPR_SPECTRE_F1,10,NULL,NULL,&s_spectredie2};
statetype s_spectredie2         = {false,SPR_SPECTRE_F2,10,NULL,NULL,&s_spectredie3};
statetype s_spectredie3         = {false,SPR_SPECTRE_F3,10,NULL,NULL,&s_spectredie4};
statetype s_spectredie4         = {false,SPR_SPECTRE_F4,300,NULL,NULL,&s_spectrewake};
statetype s_spectrewake         = {false,SPR_SPECTRE_F4,10,NULL,(statefunc)A_Dormant,&s_spectrewake};

statetype s_schabbstand         = {false,SPR_SCHABB_W1,0,(statefunc)T_Stand,NULL,&s_schabbstand};
statetype s_schabbchase1        = {false,SPR_SCHABB_W1,10,(statefunc)T_ProjectileBossChase,NULL,&s_schabbchase1s};
statetype s_schabbchase1s       = {false,SPR_SCHABB_W1,3,NULL,NULL,&s_schabbchase2};
statetype s_schabbchase2        = {false,SPR_SCHABB_W2,8,(statefunc)T_ProjectileBossChase,NULL,&s_schabbchase3};
statetype s_schabbchase3        = {false,SPR_SCHABB_W3,10,(statefunc)T_ProjectileBossChase,NULL,&s_schabbchase3s};
statetype s_schabbchase3s       = {false,SPR_SCHABB_W3,3,NULL,NULL,&s_schabbchase4};
statetype s_schabbchase4        = {false,SPR_SCHABB_W4,8,(statefunc)T_ProjectileBossChase,NULL,&s_schabbchase1};
statetype s_schabbdeathcam      = {false,SPR_SCHABB_W1,1,NULL,NULL,&s_schabbdie1};
statetype s_schabbdie1          = {false,SPR_SCHABB_W1,10,NULL,(statefunc)A_DeathScream,&s_schabbdie2};
statetype s_schabbdie2          = {false,SPR_SCHABB_W1,10,NULL,NULL,&s_schabbdie3};
statetype s_schabbdie3          = {false,SPR_SCHABB_DIE1,10,NULL,NULL,&s_schabbdie4};
statetype s_schabbdie4          = {false,SPR_SCHABB_DIE2,10,NULL,NULL,&s_schabbdie5};
statetype s_schabbdie5          = {false,SPR_SCHABB_DIE3,10,NULL,NULL,&s_schabbdie6};
statetype s_schabbdie6          = {false,SPR_SCHABB_DEAD,20,NULL,(statefunc)A_StartDeathCam,&s_schabbdie6};
statetype s_schabbshoot1        = {false,SPR_SCHABB_SHOOT1,30,NULL,NULL,&s_schabbshoot2};
statetype s_schabbshoot2        = {false,SPR_SCHABB_SHOOT2,10,NULL,(statefunc)A_SchabbThrow,&s_schabbchase1};

statetype s_needle1             = {false,SPR_HYPO1,6,(statefunc)T_Projectile,NULL,&s_needle2};
statetype s_needle2             = {false,SPR_HYPO2,6,(statefunc)T_Projectile,NULL,&s_needle3};
statetype s_needle3             = {false,SPR_HYPO3,6,(statefunc)T_Projectile,NULL,&s_needle4};
statetype s_needle4             = {false,SPR_HYPO4,6,(statefunc)T_Projectile,NULL,&s_needle1};

statetype s_giftstand           = {false,SPR_GIFT_W1,0,(statefunc)T_Stand,NULL,&s_giftstand};
statetype s_giftchase1          = {false,SPR_GIFT_W1,10,(statefunc)T_ProjectileBossChase,NULL,&s_giftchase1s};
statetype s_giftchase1s         = {false,SPR_GIFT_W1,3,NULL,NULL,&s_giftchase2};
statetype s_giftchase2          = {false,SPR_GIFT_W2,8,(statefunc)T_ProjectileBossChase,NULL,&s_giftchase3};
statetype s_giftchase3          = {false,SPR_GIFT_W3,10,(statefunc)T_ProjectileBossChase,NULL,&s_giftchase3s};
statetype s_giftchase3s         = {false,SPR_GIFT_W3,3,NULL,NULL,&s_giftchase4};
statetype s_giftchase4          = {false,SPR_GIFT_W4,8,(statefunc)T_ProjectileBossChase,NULL,&s_giftchase1};
statetype s_giftdeathcam        = {false,SPR_GIFT_W1,1,NULL,NULL,&s_giftdie1};
statetype s_giftdie1            = {false,SPR_GIFT_W1,1,NULL,(statefunc)A_DeathScream,&s_giftdie2};
statetype s_giftdie2            = {false,SPR_GIFT_W1,10,NULL,NULL,&s_giftdie3};
statetype s_giftdie3            = {false,SPR_GIFT_DIE1,10,NULL,NULL,&s_giftdie4};
statetype s_giftdie4            = {false,SPR_GIFT_DIE2,10,NULL,NULL,&s_giftdie5};
statetype s_giftdie5            = {false,SPR_GIFT_DIE3,10,NULL,NULL,&s_giftdie6};
statetype s_giftdie6            = {false,SPR_GIFT_DEAD,20,NULL,(statefunc)A_StartDeathCam,&s_giftdie6};
statetype s_giftshoot1          = {false,SPR_GIFT_SHOOT1,30,NULL,NULL,&s_giftshoot2};
statetype s_giftshoot2          = {false,SPR_GIFT_SHOOT2,10,NULL,(statefunc)A_GiftThrow,&s_giftchase1};

statetype s_fatstand            = {false,SPR_FAT_W1,0,(statefunc)T_Stand,NULL,&s_fatstand};
statetype s_fatchase1           = {false,SPR_FAT_W1,10,(statefunc)T_ProjectileBossChase,NULL,&s_fatchase1s};
statetype s_fatchase1s          = {false,SPR_FAT_W1,3,NULL,NULL,&s_fatchase2};
statetype s_fatchase2           = {false,SPR_FAT_W2,8,(statefunc)T_ProjectileBossChase,NULL,&s_fatchase3};
statetype s_fatchase3           = {false,SPR_FAT_W3,10,(statefunc)T_ProjectileBossChase,NULL,&s_fatchase3s};
statetype s_fatchase3s          = {false,SPR_FAT_W3,3,NULL,NULL,&s_fatchase4};
statetype s_fatchase4           = {false,SPR_FAT_W4,8,(statefunc)T_ProjectileBossChase,NULL,&s_fatchase1};
statetype s_fatdeathcam         = {false,SPR_FAT_W1,1,NULL,NULL,&s_fatdie1};
statetype s_fatdie1             = {false,SPR_FAT_W1,1,NULL,(statefunc)A_DeathScream,&s_fatdie2};
statetype s_fatdie2             = {false,SPR_FAT_W1,10,NULL,NULL,&s_fatdie3};
statetype s_fatdie3             = {false,SPR_FAT_DIE1,10,NULL,NULL,&s_fatdie4};
statetype s_fatdie4             = {false,SPR_FAT_DIE2,10,NULL,NULL,&s_fatdie5};
statetype s_fatdie5             = {false,SPR_FAT_DIE3,10,NULL,NULL,&s_fatdie6};
statetype s_fatdie6             = {false,SPR_FAT_DEAD,20,NULL,(statefunc)A_StartDeathCam,&s_fatdie6};
statetype s_fatshoot1           = {false,SPR_FAT_SHOOT1,30,NULL,NULL,&s_fatshoot2};
statetype s_fatshoot2           = {false,SPR_FAT_SHOOT2,10,NULL,(statefunc)A_GiftThrow,&s_fatshoot3,STF_DAMAGING};
statetype s_fatshoot3           = {false,SPR_FAT_SHOOT3,10,NULL,(statefunc)A_Shoot,&s_fatshoot4,STF_DAMAGING};
statetype s_fatshoot4           = {false,SPR_FAT_SHOOT4,10,NULL,(statefunc)A_Shoot,&s_fatshoot5,STF_DAMAGING};
statetype s_fatshoot5           = {false,SPR_FAT_SHOOT3,10,NULL,(statefunc)A_Shoot,&s_fatshoot6,STF_DAMAGING};
statetype s_fatshoot6           = {false,SPR_FAT_SHOOT4,10,NULL,(statefunc)A_Shoot,&s_fatchase1,STF_DAMAGING};

statetype s_fakestand           = {false,SPR_FAKE_W1,0,(statefunc)T_Stand,NULL,&s_fakestand};
statetype s_fakechase1          = {false,SPR_FAKE_W1,10,(statefunc)T_Fake,NULL,&s_fakechase1s};
statetype s_fakechase1s         = {false,SPR_FAKE_W1,3,NULL,NULL,&s_fakechase2};
statetype s_fakechase2          = {false,SPR_FAKE_W2,8,(statefunc)T_Fake,NULL,&s_fakechase3};
statetype s_fakechase3          = {false,SPR_FAKE_W3,10,(statefunc)T_Fake,NULL,&s_fakechase3s};
statetype s_fakechase3s         = {false,SPR_FAKE_W3,3,NULL,NULL,&s_fakechase4};
statetype s_fakechase4          = {false,SPR_FAKE_W4,8,(statefunc)T_Fake,NULL,&s_fakechase1};
statetype s_fakedie1            = {false,SPR_FAKE_DIE1,10,NULL,(statefunc)A_DeathScream,&s_fakedie2};
statetype s_fakedie2            = {false,SPR_FAKE_DIE2,10,NULL,NULL,&s_fakedie3};
statetype s_fakedie3            = {false,SPR_FAKE_DIE3,10,NULL,NULL,&s_fakedie4};
statetype s_fakedie4            = {false,SPR_FAKE_DIE4,10,NULL,NULL,&s_fakedie5};
statetype s_fakedie5            = {false,SPR_FAKE_DIE5,10,NULL,NULL,&s_fakedie6};
statetype s_fakedie6            = {false,SPR_FAKE_DEAD,0,NULL,NULL,&s_fakedie6};
statetype s_fakeshoot1          = {false,SPR_FAKE_SHOOT,8,NULL,(statefunc)A_FakeFire,&s_fakeshoot2};
statetype s_fakeshoot2          = {false,SPR_FAKE_SHOOT,8,NULL,(statefunc)A_FakeFire,&s_fakeshoot3};
statetype s_fakeshoot3          = {false,SPR_FAKE_SHOOT,8,NULL,(statefunc)A_FakeFire,&s_fakeshoot4};
statetype s_fakeshoot4          = {false,SPR_FAKE_SHOOT,8,NULL,(statefunc)A_FakeFire,&s_fakeshoot5};
statetype s_fakeshoot5          = {false,SPR_FAKE_SHOOT,8,NULL,(statefunc)A_FakeFire,&s_fakeshoot6};
statetype s_fakeshoot6          = {false,SPR_FAKE_SHOOT,8,NULL,(statefunc)A_FakeFire,&s_fakeshoot7};
statetype s_fakeshoot7          = {false,SPR_FAKE_SHOOT,8,NULL,(statefunc)A_FakeFire,&s_fakeshoot8};
statetype s_fakeshoot8          = {false,SPR_FAKE_SHOOT,8,NULL,(statefunc)A_FakeFire,&s_fakeshoot9};
statetype s_fakeshoot9          = {false,SPR_FAKE_SHOOT,8,NULL,NULL,&s_fakechase1};

statetype s_fire1               = {false,SPR_FIRE1,6,NULL,(statefunc)T_Projectile,&s_fire2};
statetype s_fire2               = {false,SPR_FIRE2,6,NULL,(statefunc)T_Projectile,&s_fire1};

statetype s_mechastand          = {false,SPR_MECHA_W1,0,(statefunc)T_Stand,NULL,&s_mechastand};
statetype s_mechachase1         = {false,SPR_MECHA_W1,10,(statefunc)T_Chase,(statefunc)A_MechaSound,&s_mechachase1s};
statetype s_mechachase1s        = {false,SPR_MECHA_W1,6,NULL,NULL,&s_mechachase2};
statetype s_mechachase2         = {false,SPR_MECHA_W2,8,(statefunc)T_Chase,NULL,&s_mechachase3};
statetype s_mechachase3         = {false,SPR_MECHA_W3,10,(statefunc)T_Chase,(statefunc)A_MechaSound,&s_mechachase3s};
statetype s_mechachase3s        = {false,SPR_MECHA_W3,6,NULL,NULL,&s_mechachase4};
statetype s_mechachase4         = {false,SPR_MECHA_W4,8,(statefunc)T_Chase,NULL,&s_mechachase1};
statetype s_mechadie1           = {false,SPR_MECHA_DIE1,10,NULL,(statefunc)A_DeathScream,&s_mechadie2};
statetype s_mechadie2           = {false,SPR_MECHA_DIE2,10,NULL,NULL,&s_mechadie3};
statetype s_mechadie3           = {false,SPR_MECHA_DIE3,10,NULL,(statefunc)A_HitlerMorph,&s_mechadie4};
statetype s_mechadie4           = {false,SPR_MECHA_DEAD,0,NULL,NULL,&s_mechadie4};
statetype s_mechashoot1         = {false,SPR_MECHA_SHOOT1,20,NULL,NULL,&s_mechashoot1b};
statetype s_mechashoot1b        = {false,SPR_MECHA_SHOOT1,10,NULL,NULL,&s_mechashoot2,STF_DAMAGING};
statetype s_mechashoot2         = {false,SPR_MECHA_SHOOT2,10,NULL,(statefunc)A_Shoot,&s_mechashoot3,STF_DAMAGING};
statetype s_mechashoot3         = {false,SPR_MECHA_SHOOT3,10,NULL,(statefunc)A_Shoot,&s_mechashoot4,STF_DAMAGING};
statetype s_mechashoot4         = {false,SPR_MECHA_SHOOT2,10,NULL,(statefunc)A_Shoot,&s_mechashoot5,STF_DAMAGING};
statetype s_mechashoot5         = {false,SPR_MECHA_SHOOT3,10,NULL,(statefunc)A_Shoot,&s_mechashoot6,STF_DAMAGING};
statetype s_mechashoot6         = {false,SPR_MECHA_SHOOT2,10,NULL,(statefunc)A_Shoot,&s_mechachase1,STF_DAMAGING};

statetype s_hitlerchase1        = {false,SPR_HITLER_W1,6,(statefunc)T_Chase,NULL,&s_hitlerchase1s};
statetype s_hitlerchase1s       = {false,SPR_HITLER_W1,4,NULL,NULL,&s_hitlerchase2};
statetype s_hitlerchase2        = {false,SPR_HITLER_W2,2,(statefunc)T_Chase,NULL,&s_hitlerchase3};
statetype s_hitlerchase3        = {false,SPR_HITLER_W3,6,(statefunc)T_Chase,NULL,&s_hitlerchase3s};
statetype s_hitlerchase3s       = {false,SPR_HITLER_W3,4,NULL,NULL,&s_hitlerchase4};
statetype s_hitlerchase4        = {false,SPR_HITLER_W4,2,(statefunc)T_Chase,NULL,&s_hitlerchase1};
statetype s_hitlerdeathcam      = {false,SPR_HITLER_W1,10,NULL,NULL,&s_hitlerdie1};
statetype s_hitlerdie1          = {false,SPR_HITLER_W1,1,NULL,(statefunc)A_DeathScream,&s_hitlerdie2};
statetype s_hitlerdie2          = {false,SPR_HITLER_W1,10,NULL,NULL,&s_hitlerdie3};
statetype s_hitlerdie3          = {false,SPR_HITLER_DIE1,10,NULL,(statefunc)A_Slurpie,&s_hitlerdie4};
statetype s_hitlerdie4          = {false,SPR_HITLER_DIE2,10,NULL,NULL,&s_hitlerdie5};
statetype s_hitlerdie5          = {false,SPR_HITLER_DIE3,10,NULL,NULL,&s_hitlerdie6};
statetype s_hitlerdie6          = {false,SPR_HITLER_DIE4,10,NULL,NULL,&s_hitlerdie7};
statetype s_hitlerdie7          = {false,SPR_HITLER_DIE5,10,NULL,NULL,&s_hitlerdie8};
statetype s_hitlerdie8          = {false,SPR_HITLER_DIE6,10,NULL,NULL,&s_hitlerdie9};
statetype s_hitlerdie9          = {false,SPR_HITLER_DIE7,10,NULL,NULL,&s_hitlerdie10};
statetype s_hitlerdie10         = {false,SPR_HITLER_DEAD,20,NULL,(statefunc)A_StartDeathCam,&s_hitlerdie10};
statetype s_hitlershoot1        = {false,SPR_HITLER_SHOOT1,20,NULL,NULL,&s_hitlershoot1b};
statetype s_hitlershoot1b       = {false,SPR_HITLER_SHOOT1,10,NULL,NULL,&s_hitlershoot2,STF_DAMAGING};
statetype s_hitlershoot2        = {false,SPR_HITLER_SHOOT2,10,NULL,(statefunc)A_Shoot,&s_hitlershoot3,STF_DAMAGING};
statetype s_hitlershoot3        = {false,SPR_HITLER_SHOOT3,10,NULL,(statefunc)A_Shoot,&s_hitlershoot4,STF_DAMAGING};
statetype s_hitlershoot4        = {false,SPR_HITLER_SHOOT2,10,NULL,(statefunc)A_Shoot,&s_hitlershoot5,STF_DAMAGING};
statetype s_hitlershoot5        = {false,SPR_HITLER_SHOOT3,10,NULL,(statefunc)A_Shoot,&s_hitlershoot6,STF_DAMAGING};
statetype s_hitlershoot6        = {false,SPR_HITLER_SHOOT2,10,NULL,(statefunc)A_Shoot,&s_hitlerchase1,STF_DAMAGING};

statetype s_bjrun1              = {false,SPR_BJ_W1,12,(statefunc)T_BJRun,NULL,&s_bjrun1s};
statetype s_bjrun1s             = {false,SPR_BJ_W1,3, NULL,NULL,&s_bjrun2};
statetype s_bjrun2              = {false,SPR_BJ_W2,8,(statefunc)T_BJRun,NULL,&s_bjrun3};
statetype s_bjrun3              = {false,SPR_BJ_W3,12,(statefunc)T_BJRun,NULL,&s_bjrun3s};
statetype s_bjrun3s             = {false,SPR_BJ_W3,3, NULL,NULL,&s_bjrun4};
statetype s_bjrun4              = {false,SPR_BJ_W4,8,(statefunc)T_BJRun,NULL,&s_bjrun1};
statetype s_bjjump1             = {false,SPR_BJ_JUMP1,14,(statefunc)T_BJJump,NULL,&s_bjjump2};
statetype s_bjjump2             = {false,SPR_BJ_JUMP2,14,(statefunc)T_BJJump,(statefunc)A_BJYell,&s_bjjump3};
statetype s_bjjump3             = {false,SPR_BJ_JUMP3,14,(statefunc)T_BJJump,NULL,&s_bjjump4};
statetype s_bjjump4             = {false,SPR_BJ_JUMP4,300,NULL,(statefunc)A_BJDone,&s_bjjump4};

statetype s_deathcam            = {false,0,0,NULL,NULL,NULL};