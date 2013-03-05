#include "wl_def.h"

const objattrib objattribs[] =
{
    {-1,                NULL,           NULL}, // nothing,
    {-1,                NULL,           NULL}, // playerobj,
    {-1,                NULL,           NULL}, // inertobj,
    {SPDPATROL, HALTSND,           &s_grdpath1,    &s_grdchase1}, // guardobj,
    {SPDPATROL, SPIONSND,          &s_ofcpath1,    &s_ofcchase1}, // officerobj,
    {SPDPATROL, SCHUTZADSND,       &s_sspath1,     &s_sschase1}, // ssobj,
    {SPDDOG, DOGBARKSND,        &s_dogpath1,    &s_dogchase1}, // dogobj,
    {GUTENTAGSND,       NULL,           &s_bosschase1}, // bossobj,
    {SCHABBSHASND,      NULL,           &s_schabbchase1}, // schabbobj,
    {TOT_HUNDSND,       NULL,           &s_fakechase1}, // fakeobj,
    {DIESND,            NULL,           &s_mechachase1}, // mechahitlerobj,
    {SPDPATROL, -1,                &s_mutpath1,    &s_mutchase1}, // mutantobj,
    {-1,                NULL,           NULL}, // needleobj,
    {-1,                NULL,           NULL}, // fireobj,
    {-1,                NULL,           NULL}, // bjobj,
    {-1,                NULL,           &s_blinkychase1}, // ghostobj,
    {DIESND,            NULL,           &s_hitlerchase1}, // realhitlerobj,
    {KEINSND,           NULL,           &s_gretelchase1}, // gretelobj,
    {EINESND,           NULL,           &s_giftchase1}, // giftobj,
    {ERLAUBENSND,       NULL,           &s_fatchase1}, // fatobj,
    {-1,                NULL,           NULL}, // rocketobj,
    {GHOSTSIGHTSND,     NULL,           &s_spectrechase1}, // spectreobj,
    {ANGELSIGHTSND,     NULL,           &s_angelchase1}, // angelobj,
    {TRANSSIGHTSND,     NULL,           &s_transchase1}, // transobj,
    {-1,                NULL,           &s_uberchase1}, // uberobj,
    {WILHELMSIGHTSND,   NULL,           &s_willchase1}, // willobj,
    {KNIGHTSIGHTSND,    NULL,           &s_deathchase1}, // deathobj,
    {-1,                NULL,           NULL}, // hrocketobj,
    {-1,                NULL,           NULL}, // sparkobj
};
