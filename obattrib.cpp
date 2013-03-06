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
    {-1,          -1,              NULL,             NULL,       NULL}, // nothing,
    {-1,          -1,              NULL,             NULL,       NULL}, // playerobj,
    {-1,          -1,              NULL,             NULL,       NULL}, // inertobj,
    {SPDPATROL*3, HALTSND,         &s_grdchase1,     &s_grdpain, &s_grdpain1}, // guardobj,
    {SPDPATROL*5, SPIONSND,        &s_ofcchase1,     &s_ofcpain, &s_ofcpain1}, // officerobj,
    {SPDPATROL*4, SCHUTZADSND,     &s_sschase1,      &s_sspain,  &s_sspain1}, // ssobj,
    {SPDDOG*2,    DOGBARKSND,      &s_dogchase1,     NULL,       NULL}, // dogobj,
    {SPDPATROL*3, GUTENTAGSND,     &s_bosschase1,    NULL,       NULL}, // bossobj,
    {SPDPATROL*3, SCHABBSHASND,    &s_schabbchase1,  NULL,       NULL}, // schabbobj,
    {SPDPATROL*3, TOT_HUNDSND,     &s_fakechase1,    NULL,       NULL}, // fakeobj,
    {SPDPATROL*3, DIESND,          &s_mechachase1,   NULL,       NULL}, // mechahitlerobj,
    {SPDPATROL*3, -1,              &s_mutchase1,     &s_mutpain, &s_mutpain1}, // mutantobj,
    {-1,          -1,              NULL,             NULL,       NULL}, // needleobj,
    {-1,          -1,              NULL,             NULL,       NULL}, // fireobj,
    {-1,          -1,              NULL,             NULL,       NULL}, // bjobj,
    {SPDDOG*2,    -1,              &s_blinkychase1,  NULL,       NULL}, // ghostobj,
    {SPDPATROL*5, DIESND,          &s_hitlerchase1,  NULL,       NULL}, // realhitlerobj,
    {SPDPATROL*3, KEINSND,         &s_gretelchase1,  NULL,       NULL}, // gretelobj,
    {SPDPATROL*3, EINESND,         &s_giftchase1,    NULL,       NULL}, // giftobj,
    {SPDPATROL*3, ERLAUBENSND,     &s_fatchase1,     NULL,       NULL}, // fatobj,
    {-1,          -1,              NULL,             NULL,       NULL}, // rocketobj,
    {800,         GHOSTSIGHTSND,   &s_spectrechase1, NULL,       NULL}, // spectreobj,
    {1536,        ANGELSIGHTSND,   &s_angelchase1,   NULL,       NULL}, // angelobj,
    {1536,        TRANSSIGHTSND,   &s_transchase1,   NULL,       NULL}, // transobj,
    {3000,        -1,              &s_uberchase1,    NULL,       NULL}, // uberobj,
    {2048,        WILHELMSIGHTSND, &s_willchase1,    NULL,       NULL}, // willobj,
    {2048,        KNIGHTSIGHTSND,  &s_deathchase1,   NULL,       NULL}, // deathobj,
    {-1,          -1,              NULL,             NULL,       NULL}, // hrocketobj,
    {-1,          -1,              NULL,             NULL,       NULL}, // sparkobj
};
