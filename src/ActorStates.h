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

#ifndef __Wolf4SDL__ActorStates__
#define __Wolf4SDL__ActorStates__

extern  statetype s_angelchase1;
extern  statetype s_angelchase1s;
extern  statetype s_angelchase2;
extern  statetype s_angelchase3;
extern  statetype s_angelchase3s;
extern  statetype s_angelchase4;
extern  statetype s_angeldie11;
extern  statetype s_angeldie1;
extern  statetype s_angeldie2;
extern  statetype s_angeldie3;
extern  statetype s_angeldie4;
extern  statetype s_angeldie5;
extern  statetype s_angeldie6;
extern  statetype s_angeldie7;
extern  statetype s_angeldie8;
extern  statetype s_angeldie9;
extern  statetype s_angelshoot1;
extern  statetype s_angelshoot1;
extern  statetype s_angelshoot2;
extern  statetype s_angelshoot3;
extern  statetype s_angelshoot4;
extern  statetype s_angelshoot5;
extern  statetype s_angelshoot6;
extern  statetype s_angelstand;
extern  statetype s_angeltired2;
extern  statetype s_angeltired3;
extern  statetype s_angeltired4;
extern  statetype s_angeltired5;
extern  statetype s_angeltired6;
extern  statetype s_angeltired7;
extern  statetype s_angeltired;
extern  statetype s_bjjump1;
extern  statetype s_bjjump2;
extern  statetype s_bjjump3;
extern  statetype s_bjjump4;
extern  statetype s_bjrun1;
extern  statetype s_bjrun1s;
extern  statetype s_bjrun2;
extern  statetype s_bjrun3;
extern  statetype s_bjrun3s;
extern  statetype s_bjrun4;
extern  statetype s_blinkychase1;
extern  statetype s_blinkychase2;
extern  statetype s_boom1;
extern  statetype s_boom2;
extern  statetype s_boom2;
extern  statetype s_boom3;
extern  statetype s_boom3;
extern  statetype s_bosschase1;
extern  statetype s_bosschase1s;
extern  statetype s_bosschase2;
extern  statetype s_bosschase3;
extern  statetype s_bosschase3s;
extern  statetype s_bosschase4;
extern  statetype s_bossdie1;
extern  statetype s_bossdie2;
extern  statetype s_bossdie3;
extern  statetype s_bossdie4;
extern  statetype s_bossshoot1;
extern  statetype s_bossshoot1b;
extern  statetype s_bossshoot2;
extern  statetype s_bossshoot3;
extern  statetype s_bossshoot4;
extern  statetype s_bossshoot5;
extern  statetype s_bossshoot6;
extern  statetype s_bossshoot7;
extern  statetype s_bossshoot8;
extern  statetype s_bossstand;
extern  statetype s_clydechase1;
extern  statetype s_clydechase2;
extern  statetype s_deathcam;
extern  statetype s_deathchase1;
extern  statetype s_deathchase1s;
extern  statetype s_deathchase2;
extern  statetype s_deathchase3;
extern  statetype s_deathchase3s;
extern  statetype s_deathchase4;
extern  statetype s_deathdie1;
extern  statetype s_deathdie2;
extern  statetype s_deathdie3;
extern  statetype s_deathdie4;
extern  statetype s_deathdie5;
extern  statetype s_deathdie6;
extern  statetype s_deathdie7;
extern  statetype s_deathdie8;
extern  statetype s_deathdie9;
extern  statetype s_deathshoot1;
extern  statetype s_deathshoot1;
extern  statetype s_deathshoot2;
extern  statetype s_deathshoot3;
extern  statetype s_deathshoot4;
extern  statetype s_deathshoot5;
extern  statetype s_deathstand;
extern  statetype s_dogchase1;
extern  statetype s_dogchase1s;
extern  statetype s_dogchase2;
extern  statetype s_dogchase3;
extern  statetype s_dogchase3s;
extern  statetype s_dogchase4;
extern  statetype s_dogdead;
extern  statetype s_dogdie1;
extern  statetype s_dogdie1d;
extern  statetype s_dogdie2;
extern  statetype s_dogdie3;
extern  statetype s_dogjump1;
extern  statetype s_dogjump2;
extern  statetype s_dogjump3;
extern  statetype s_dogjump4;
extern  statetype s_dogjump5;
extern  statetype s_dogpath1;
extern  statetype s_dogpath1s;
extern  statetype s_dogpath2;
extern  statetype s_dogpath3;
extern  statetype s_dogpath3s;
extern  statetype s_dogpath4;
extern  statetype s_fakechase1;
extern  statetype s_fakechase1s;
extern  statetype s_fakechase2;
extern  statetype s_fakechase3;
extern  statetype s_fakechase3s;
extern  statetype s_fakechase4;
extern  statetype s_fakedie1;
extern  statetype s_fakedie2;
extern  statetype s_fakedie3;
extern  statetype s_fakedie4;
extern  statetype s_fakedie5;
extern  statetype s_fakedie6;
extern  statetype s_fakeshoot1;
extern  statetype s_fakeshoot2;
extern  statetype s_fakeshoot3;
extern  statetype s_fakeshoot4;
extern  statetype s_fakeshoot5;
extern  statetype s_fakeshoot6;
extern  statetype s_fakeshoot7;
extern  statetype s_fakeshoot8;
extern  statetype s_fakeshoot9;
extern  statetype s_fakestand;
extern  statetype s_fatchase1;
extern  statetype s_fatchase1s;
extern  statetype s_fatchase2;
extern  statetype s_fatchase3;
extern  statetype s_fatchase3s;
extern  statetype s_fatchase4;
extern  statetype s_fatdeathcam;
extern  statetype s_fatdie1;
extern  statetype s_fatdie2;
extern  statetype s_fatdie3;
extern  statetype s_fatdie4;
extern  statetype s_fatdie5;
extern  statetype s_fatdie6;
extern  statetype s_fatshoot1;
extern  statetype s_fatshoot2;
extern  statetype s_fatshoot3;
extern  statetype s_fatshoot4;
extern  statetype s_fatshoot5;
extern  statetype s_fatshoot6;
extern  statetype s_fatstand;
extern  statetype s_fire1;
extern  statetype s_fire2;
extern  statetype s_giftchase1;
extern  statetype s_giftchase1s;
extern  statetype s_giftchase2;
extern  statetype s_giftchase3;
extern  statetype s_giftchase3s;
extern  statetype s_giftchase4;
extern  statetype s_giftdeathcam;
extern  statetype s_giftdie1;
extern  statetype s_giftdie2;
extern  statetype s_giftdie3;
extern  statetype s_giftdie4;
extern  statetype s_giftdie5;
extern  statetype s_giftdie6;
extern  statetype s_giftshoot1;
extern  statetype s_giftshoot2;
extern  statetype s_giftstand;
extern  statetype s_grdchase1;
extern  statetype s_grdchase1s;
extern  statetype s_grdchase2;
extern  statetype s_grdchase3;
extern  statetype s_grdchase3s;
extern  statetype s_grdchase4;
extern  statetype s_grddie1;
extern  statetype s_grddie1d;
extern  statetype s_grddie2;
extern  statetype s_grddie3;
extern  statetype s_grddie4;
extern  statetype s_grdgiveup;
extern  statetype s_grdpain1;
extern  statetype s_grdpain;
extern  statetype s_grdpath1;
extern  statetype s_grdpath1s;
extern  statetype s_grdpath2;
extern  statetype s_grdpath3;
extern  statetype s_grdpath3s;
extern  statetype s_grdpath4;
extern  statetype s_grdshoot1;
extern  statetype s_grdshoot2;
extern  statetype s_grdshoot3;
extern  statetype s_grdshoot4;
extern  statetype s_grdstand;
extern  statetype s_gretelchase1;
extern  statetype s_gretelchase1s;
extern  statetype s_gretelchase2;
extern  statetype s_gretelchase3;
extern  statetype s_gretelchase3s;
extern  statetype s_gretelchase4;
extern  statetype s_greteldie1;
extern  statetype s_greteldie2;
extern  statetype s_greteldie3;
extern  statetype s_greteldie4;
extern  statetype s_gretelshoot1;
extern  statetype s_gretelshoot1b;
extern  statetype s_gretelshoot2;
extern  statetype s_gretelshoot3;
extern  statetype s_gretelshoot4;
extern  statetype s_gretelshoot5;
extern  statetype s_gretelshoot6;
extern  statetype s_gretelshoot7;
extern  statetype s_gretelshoot8;
extern  statetype s_gretelstand;
extern  statetype s_hboom2;
extern  statetype s_hboom3;
extern  statetype s_hitlerchase1;
extern  statetype s_hitlerchase1s;
extern  statetype s_hitlerchase2;
extern  statetype s_hitlerchase3;
extern  statetype s_hitlerchase3s;
extern  statetype s_hitlerchase4;
extern  statetype s_hitlerdeathcam;
extern  statetype s_hitlerdie10;
extern  statetype s_hitlerdie1;
extern  statetype s_hitlerdie2;
extern  statetype s_hitlerdie3;
extern  statetype s_hitlerdie4;
extern  statetype s_hitlerdie5;
extern  statetype s_hitlerdie6;
extern  statetype s_hitlerdie7;
extern  statetype s_hitlerdie8;
extern  statetype s_hitlerdie9;
extern  statetype s_hitlershoot1;
extern  statetype s_hitlershoot1b;
extern  statetype s_hitlershoot2;
extern  statetype s_hitlershoot3;
extern  statetype s_hitlershoot4;
extern  statetype s_hitlershoot5;
extern  statetype s_hitlershoot6;
extern  statetype s_hrocket;
extern  statetype s_hsmoke1;
extern  statetype s_hsmoke2;
extern  statetype s_hsmoke3;
extern  statetype s_hsmoke4;
extern  statetype s_inkychase1;
extern  statetype s_inkychase2;
extern  statetype s_mechachase1;
extern  statetype s_mechachase1s;
extern  statetype s_mechachase2;
extern  statetype s_mechachase3;
extern  statetype s_mechachase3s;
extern  statetype s_mechachase4;
extern  statetype s_mechadie1;
extern  statetype s_mechadie2;
extern  statetype s_mechadie3;
extern  statetype s_mechadie4;
extern  statetype s_mechashoot1;
extern  statetype s_mechashoot1b;
extern  statetype s_mechashoot2;
extern  statetype s_mechashoot3;
extern  statetype s_mechashoot4;
extern  statetype s_mechashoot5;
extern  statetype s_mechashoot6;
extern  statetype s_mutchase1;
extern  statetype s_mutchase1s;
extern  statetype s_mutchase2;
extern  statetype s_mutchase3;
extern  statetype s_mutchase3s;
extern  statetype s_mutchase4;
extern  statetype s_mutdie1;
extern  statetype s_mutdie2;
extern  statetype s_mutdie3;
extern  statetype s_mutdie4;
extern  statetype s_mutdie5;
extern  statetype s_mutgiveup;
extern  statetype s_mutpain1;
extern  statetype s_mutpain;
extern  statetype s_mutpath1;
extern  statetype s_mutpath1s;
extern  statetype s_mutpath2;
extern  statetype s_mutpath3;
extern  statetype s_mutpath3s;
extern  statetype s_mutpath4;
extern  statetype s_mutshoot1;
extern  statetype s_mutshoot2;
extern  statetype s_mutshoot3;
extern  statetype s_mutshoot4;
extern  statetype s_mutstand;
extern  statetype s_needle1;
extern  statetype s_needle1;
extern  statetype s_needle1;
extern  statetype s_needle2;
extern  statetype s_needle2;
extern  statetype s_needle2;
extern  statetype s_needle3;
extern  statetype s_needle3;
extern  statetype s_needle3;
extern  statetype s_needle4;
extern  statetype s_needle4;
extern  statetype s_needle4;
extern  statetype s_ofcchase1;
extern  statetype s_ofcchase1s;
extern  statetype s_ofcchase2;
extern  statetype s_ofcchase3;
extern  statetype s_ofcchase3s;
extern  statetype s_ofcchase4;
extern  statetype s_ofcdie1;
extern  statetype s_ofcdie2;
extern  statetype s_ofcdie3;
extern  statetype s_ofcdie4;
extern  statetype s_ofcdie5;
extern  statetype s_ofcgiveup;
extern  statetype s_ofcpain1;
extern  statetype s_ofcpain;
extern  statetype s_ofcpath1;
extern  statetype s_ofcpath1s;
extern  statetype s_ofcpath2;
extern  statetype s_ofcpath3;
extern  statetype s_ofcpath3s;
extern  statetype s_ofcpath4;
extern  statetype s_ofcshoot1;
extern  statetype s_ofcshoot2;
extern  statetype s_ofcshoot3;
extern  statetype s_ofcshoot4;
extern  statetype s_ofcstand;
extern  statetype s_pinkychase1;
extern  statetype s_pinkychase2;
extern  statetype s_rocket;
extern  statetype s_schabbchase1;
extern  statetype s_schabbchase1s;
extern  statetype s_schabbchase2;
extern  statetype s_schabbchase3;
extern  statetype s_schabbchase3s;
extern  statetype s_schabbchase4;
extern  statetype s_schabbdeathcam;
extern  statetype s_schabbdie1;
extern  statetype s_schabbdie2;
extern  statetype s_schabbdie3;
extern  statetype s_schabbdie4;
extern  statetype s_schabbdie5;
extern  statetype s_schabbdie6;
extern  statetype s_schabbshoot1;
extern  statetype s_schabbshoot2;
extern  statetype s_schabbstand;
extern  statetype s_smoke1;
extern  statetype s_smoke2;
extern  statetype s_smoke3;
extern  statetype s_smoke4;
extern  statetype s_spark1;
extern  statetype s_spark1;
extern  statetype s_spark2;
extern  statetype s_spark3;
extern  statetype s_spark4;
extern  statetype s_spectrechase1;
extern  statetype s_spectrechase2;
extern  statetype s_spectrechase3;
extern  statetype s_spectrechase4;
extern  statetype s_spectredie1;
extern  statetype s_spectredie2;
extern  statetype s_spectredie3;
extern  statetype s_spectredie4;
extern  statetype s_spectrewait1;
extern  statetype s_spectrewait2;
extern  statetype s_spectrewait3;
extern  statetype s_spectrewait4;
extern  statetype s_spectrewake;
extern  statetype s_sschase1;
extern  statetype s_sschase1s;
extern  statetype s_sschase2;
extern  statetype s_sschase3;
extern  statetype s_sschase3s;
extern  statetype s_sschase4;
extern  statetype s_ssdie1;
extern  statetype s_ssdie2;
extern  statetype s_ssdie3;
extern  statetype s_ssdie4;
extern  statetype s_sspain1;
extern  statetype s_sspain;
extern  statetype s_sspath1;
extern  statetype s_sspath1s;
extern  statetype s_sspath2;
extern  statetype s_sspath3;
extern  statetype s_sspath3s;
extern  statetype s_sspath4;
extern  statetype s_ssshoot1;
extern  statetype s_ssshoot2;
extern  statetype s_ssshoot3;
extern  statetype s_ssshoot4;
extern  statetype s_ssshoot5;
extern  statetype s_ssshoot6;
extern  statetype s_ssshoot7;
extern  statetype s_ssshoot8;
extern  statetype s_ssshoot9;
extern  statetype s_ssstand;
extern  statetype s_transchase1;
extern  statetype s_transchase1s;
extern  statetype s_transchase2;
extern  statetype s_transchase3;
extern  statetype s_transchase3s;
extern  statetype s_transchase4;
extern  statetype s_transdie01;
extern  statetype s_transdie0;
extern  statetype s_transdie1;
extern  statetype s_transdie2;
extern  statetype s_transdie3;
extern  statetype s_transdie4;
extern  statetype s_transshoot1;
extern  statetype s_transshoot1b;
extern  statetype s_transshoot2;
extern  statetype s_transshoot3;
extern  statetype s_transshoot4;
extern  statetype s_transshoot5;
extern  statetype s_transshoot6;
extern  statetype s_transshoot7;
extern  statetype s_transshoot8;
extern  statetype s_transstand;
extern  statetype s_uberchase1;
extern  statetype s_uberchase1s;
extern  statetype s_uberchase2;
extern  statetype s_uberchase3;
extern  statetype s_uberchase3s;
extern  statetype s_uberchase4;
extern  statetype s_uberdie01;
extern  statetype s_uberdie0;
extern  statetype s_uberdie1;
extern  statetype s_uberdie2;
extern  statetype s_uberdie3;
extern  statetype s_uberdie4;
extern  statetype s_uberdie5;
extern  statetype s_ubershoot1;
extern  statetype s_ubershoot1b;
extern  statetype s_ubershoot2;
extern  statetype s_ubershoot3;
extern  statetype s_ubershoot4;
extern  statetype s_ubershoot5;
extern  statetype s_ubershoot6;
extern  statetype s_ubershoot7;
extern  statetype s_uberstand;
extern  statetype s_willchase1;
extern  statetype s_willchase1s;
extern  statetype s_willchase2;
extern  statetype s_willchase3;
extern  statetype s_willchase3s;
extern  statetype s_willchase4;
extern  statetype s_willdie1;
extern  statetype s_willdie2;
extern  statetype s_willdie3;
extern  statetype s_willdie4;
extern  statetype s_willdie5;
extern  statetype s_willdie6;
extern  statetype s_willshoot1;
extern  statetype s_willshoot2;
extern  statetype s_willshoot3;
extern  statetype s_willshoot4;
extern  statetype s_willshoot5;
extern  statetype s_willshoot6;
extern  statetype s_willstand;

#endif /* defined(__Wolf4SDL__ActorStates__) */
