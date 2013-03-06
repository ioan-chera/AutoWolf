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

#include "wl_def.h"

const objattrib objattribs[] =
{
    {{-1,   -1,   -1,   -1},   -1,        -1,          -1,              NULL,            NULL,        NULL,             NULL,       NULL}, // nothing,
    {{-1,   -1,   -1,   -1},   -1,        -1,          -1,              NULL,            NULL,        NULL,             NULL,       NULL}, // playerobj,
    {{-1,   -1,   -1,   -1},   -1,        -1,          -1,              NULL,            NULL,        NULL,             NULL,       NULL}, // inertobj,
    {{25,   25,   25,   25},   SPDPATROL, SPDPATROL*3, HALTSND,         &s_grdstand,     &s_grdpath1, &s_grdchase1,     &s_grdpain, &s_grdpain1}, // guardobj,
    {{50,   50,   50,   50},   SPDPATROL, SPDPATROL*5, SPIONSND,        &s_ofcstand,     &s_ofcpath1, &s_ofcchase1,     &s_ofcpain, &s_ofcpain1}, // officerobj,
    {{100,  100,  100,  100},  SPDPATROL, SPDPATROL*4, SCHUTZADSND,     &s_ssstand,      &s_sspath1,  &s_sschase1,      &s_sspain,  &s_sspain1}, // ssobj,
    {{1,    1,    1,    1,},   SPDDOG,    SPDDOG*2,    DOGBARKSND,      NULL,            &s_dogpath1, &s_dogchase1,     NULL,       NULL}, // dogobj,
    {{850,  950,  1050, 1200}, -1,        SPDPATROL*3, GUTENTAGSND,     &s_bossstand,    NULL,        &s_bosschase1,    NULL,       NULL}, // bossobj,
    {{850,  950,  1550, 2400}, -1,        SPDPATROL*3, SCHABBSHASND,    &s_schabbstand,  NULL,        &s_schabbchase1,  NULL,       NULL}, // schabbobj,
    {{200,  300,  400,  500},  -1,        SPDPATROL*3, TOT_HUNDSND,     &s_fakestand,    NULL,        &s_fakechase1,    NULL,       NULL}, // fakeobj,
    {{800,  950,  1050, 1200}, -1,        SPDPATROL*3, DIESND,          &s_mechastand,   NULL,        &s_mechachase1,   NULL,       NULL}, // mechahitlerobj,
    {{45,   55,   55,   65},   SPDPATROL, SPDPATROL*3, -1,              &s_mutstand,     &s_mutpath1, &s_mutchase1,     &s_mutpain, &s_mutpain1}, // mutantobj,
    {{-1,   -1,   -1,   -1},   -1,        -1,          -1,              NULL,            NULL,        NULL,             NULL,       NULL}, // needleobj,
    {{-1,   -1,   -1,   -1},   -1,        -1,          -1,              NULL,            NULL,        NULL,             NULL,       NULL}, // fireobj,
    {{-1,   -1,   -1,   -1},   -1,        -1,          -1,              NULL,            NULL,        NULL,             NULL,       NULL}, // bjobj,
    {{25,   25,   25,   25},   -1,        SPDDOG*2,    -1,              NULL,            NULL,        &s_blinkychase1,  NULL,       NULL}, // ghostobj,
    {{-1,   -1,   -1,   -1},   -1,        SPDPATROL*5, DIESND,          NULL,            NULL,        &s_hitlerchase1,  NULL,       NULL}, // realhitlerobj,
    {{850,  950,  1050, 1200}, -1,        SPDPATROL*3, KEINSND,         &s_gretelstand,  NULL,        &s_gretelchase1,  NULL,       NULL}, // gretelobj,
    {{850,  950,  1050, 1200}, -1,        SPDPATROL*3, EINESND,         &s_giftstand,    NULL,        &s_giftchase1,    NULL,       NULL}, // giftobj,
    {{850,  950,  1050, 1200}, -1,        SPDPATROL*3, ERLAUBENSND,     &s_fatstand,     NULL,        &s_fatchase1,     NULL,       NULL}, // fatobj,
    {{-1,   -1,   -1,   -1},   -1,        -1,          -1,              NULL,            NULL,        NULL,             NULL,       NULL}, // rocketobj,
    {{5,    10,   15,   25},   -1,        800,         GHOSTSIGHTSND,   &s_spectrewait1, NULL,        &s_spectrechase1, NULL,       NULL}, // spectreobj,
    {{1450, 1550, 1650, 2000}, -1,        1536,        ANGELSIGHTSND,   &s_angelstand,   NULL,        &s_angelchase1,   NULL,       NULL}, // angelobj,
    {{850,  950,  1050, 1200}, -1,        1536,        TRANSSIGHTSND,   &s_transstand,   NULL,        &s_transchase1,   NULL,       NULL}, // transobj,
    {{1050, 1150, 1250, 1400}, -1,        3000,        -1,              &s_uberstand,    NULL,        &s_uberchase1,    NULL,       NULL}, // uberobj,
    {{950,  1050, 1150, 1300}, -1,        2048,        WILHELMSIGHTSND, &s_willstand,    NULL,        &s_willchase1,    NULL,       NULL}, // willobj,
    {{1250, 1350, 1450, 1600}, -1,        2048,        KNIGHTSIGHTSND,  &s_deathstand,   NULL,        &s_deathchase1,   NULL,       NULL}, // deathobj,
    {{-1,   -1,   -1,   -1},   -1,        -1,          -1,              NULL,            NULL,        NULL,             NULL,       NULL}, // hrocketobj,
    {{-1,   -1,   -1,   -1},   -1,        -1,          -1,              NULL,            NULL,        NULL,             NULL,       NULL}, // sparkobj
};
