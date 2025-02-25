////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// other local includes
#include "../util/func.h"
#include "libastro/libastro.h"
#include "libastro/preferences.h"
#include "libastro/astro.h"

// global no of solar object aka planets/sats/sun
int g_nSolarObjects;

// inlcude main header
#include "solar_planets.h"

////////////////////////////////////////////////////////////////////
void InitSolarPlanets( DefPlanet* vectPlanets )
{
	int c=0;

	// mercury
	InitSolar_Planet( vectPlanets[c++], MERCURY, SUN, 0, wxT("Mercury") );
	// venus
	InitSolar_Planet( vectPlanets[c++], VENUS, SUN, 0, wxT("Venus") );
	////////////////////////////////////////////////////////////////
	// MARS
	InitSolar_Planet( vectPlanets[c++], MARS, SUN, 0, wxT("Mars") );
	// :: PHOBOS
	InitSolar_Planet( vectPlanets[c++], M_PHOBOS, MARS, 1, wxT("Phobos") );
	// :: DEIMOS
	InitSolar_Planet( vectPlanets[c++], M_DEIMOS, MARS, 1, wxT("Deimos") );
	////////////////////////////////////////////////////////////////
	// JUPITER
	InitSolar_Planet( vectPlanets[c++], JUPITER, SUN, 0, wxT("Jupiter") );
	// :: IO
	InitSolar_Planet( vectPlanets[c++], J_IO, JUPITER, 1, wxT("Io") );
	// :: EUROPA
	InitSolar_Planet( vectPlanets[c++], J_EUROPA, JUPITER, 1, wxT("Europa") );
	// :: GANYMEDE
	InitSolar_Planet( vectPlanets[c++], J_GANYMEDE, JUPITER, 1, wxT("Ganymede") );
	// :: CALLISTO
	InitSolar_Planet( vectPlanets[c++], J_CALLISTO, JUPITER, 1, wxT("Callisto") );
	////////////////////////////////////////////////////////////////
	// SATURN
	InitSolar_Planet( vectPlanets[c++], SATURN, SUN, 0, wxT("Saturn") );
	// :: MIMAS 
	InitSolar_Planet( vectPlanets[c++], S_MIMAS, SATURN, 1, wxT("Mimas") );
	// :: ENCELADUS
	InitSolar_Planet( vectPlanets[c++], S_ENCELADUS, SATURN, 1, wxT("Enceladus") );
	// :: TETHYS
	InitSolar_Planet( vectPlanets[c++], S_TETHYS, SATURN, 1, wxT("Tethys") );
	// :: DIONE
	InitSolar_Planet( vectPlanets[c++], S_DIONE, SATURN, 1, wxT("Dione") );
	// :: RHEA
	InitSolar_Planet( vectPlanets[c++], S_RHEA, SATURN, 1, wxT("Rhea") );
	// :: HYPERION
	InitSolar_Planet( vectPlanets[c++], S_HYPERION, SATURN, 1, wxT("Hyperion") );
	// :: IAPETUS
	InitSolar_Planet( vectPlanets[c++], S_IAPETUS, SATURN, 1, wxT("Iapetus") );
	////////////////////////////////////////////////////////////////
	// URANUS
	InitSolar_Planet( vectPlanets[c++], URANUS, SUN, 0, wxT("Uranus") );
	// :: ARIEL
	InitSolar_Planet( vectPlanets[c++], U_ARIEL, URANUS, 1, wxT("Ariel") );
	// :: UMBRIEL
	InitSolar_Planet( vectPlanets[c++], U_UMBRIEL, URANUS, 1, wxT("Umbriel") );
	// :: TITANIA
	InitSolar_Planet( vectPlanets[c++], U_TITANIA, URANUS, 1, wxT("Titania") );
	// :: OBERON
	InitSolar_Planet( vectPlanets[c++], U_OBERON, URANUS, 1, wxT("Oberon") );
	// :: MIRANDA
	InitSolar_Planet( vectPlanets[c++], U_MIRANDA, URANUS, 1, wxT("Miranda") );
	////////////////////////////////////////////////////////////////
	// NEPTUNE
	InitSolar_Planet( vectPlanets[c++], NEPTUNE, SUN, 0, wxT("Neptune") );
	// pluto
	InitSolar_Planet( vectPlanets[c++], PLUTO, SUN, 0, wxT("Pluto") );
	// sun
	InitSolar_Planet( vectPlanets[c++], SUN, SUN, 0, wxT("Sun") );
	// moon
	InitSolar_Planet( vectPlanets[c++], MOON, SUN, 0, wxT("Moon") );

	g_nSolarObjects = c;
}

////////////////////////////////////////////////////////////////////
void InitSolar_Planet( DefPlanet& planet, int obj_id, int obj_orb_id,
								int obj_type, const wxChar* obj_name )
{
	memset( &planet, 0, sizeof(DefPlanet) );

	planet.obj_id = obj_id;
	planet.obj_type = obj_type;
	planet.obj_orb_id = obj_orb_id;
	wxStrcpy( planet.obj_name, obj_name);

	// other reset to zero
/*	planet.ra = 0.0;
	planet.dec = 0.0;
	planet.x = 0.0;
	planet.y = 0.0;
	planet.dist_from_obs = 0.0;
	planet.detect_time_shift = 0.0;
	planet.mass = 0.0;
	planet.volume = 0.0;
*/
}

////////////////////////////////////////////////////////////////////
DefPlanet* GetSolarPlanetObjById( DefPlanet* vectPlanets, int nPlanetId )
{
	for( int i=0; i<g_nSolarObjects; i++ )
	{
		if( vectPlanets[i].obj_id == nPlanetId ) return( &(vectPlanets[i]) );
	}

	return( NULL );
}

////////////////////////////////////////////////////////////////////
int Get_Solar_Object_Name( DefPlanet& solar, wxString& strSolarName )
{
	strSolarName = solar.obj_name;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
int SolarPlanet_CalcRaDec( DefPlanet& planet, double nTime,
									double nLat, double nLon, double nAlt,
									double& nRa, double& nDec, double& nDist, int& bVis )
{
	Now earth_loc;
	Obj planet_obj;
	wxDateTime mydate;

//	pref_set( PREF_EQUATORIAL, PREF_GEO );
	////////////////////////////////////////////////////////////
	// set location/conditions on earth where the obs was taken
	earth_loc.n_mjd = nTime-MJD0;
	earth_loc.n_lat = nLat * DEG2RAD;
	earth_loc.n_lng = nLon * DEG2RAD;
	// stores it in earth radii from sea level
	earth_loc.n_elev = (double) nAlt/6378160.0;
	// time zone to set
	earth_loc.n_tz = 0;
	// dip of sun below hzn at twilight, >0 below, rads
	earth_loc.n_dip = 0.0;
	// desired precession display ep as an mjd, or EOD
	earth_loc.n_epoch = EOD;
	// atmospheric pressure, mBar
	earth_loc.n_pressure = 0; //1010;
	// atmospheric temp, degrees C
	earth_loc.n_temp = 15;
	// time zone name; 7 chars or less, always 0 at end
	strcpy( earth_loc.n_tznm, "" );

	/////////////////////////////////////////////
	// set planet paramas

	planet_obj.pl.plo_code = (PLCode) planet.obj_id;

	if( planet.obj_type == 0 )
	{
		planet_obj.pl.plo_code = (PLCode) planet.obj_id;
		planet_obj.pl.plo_moon = X_PLANET;

	} else if( planet.obj_type == 1 )
	{
		planet_obj.pl.plo_code = (PLCode) planet.obj_orb_id;
		planet_obj.pl.plo_moon = (MCode) planet.obj_id;
	}
	// set type of calculation to solar planet
	planet_obj.any.co_type = PLANET;

	////////////////////////////////
	// calculate planet position 
	Sol_CalcObjOrbitPos( &earth_loc, &planet_obj );

	// set back the values - maybe here use object to set the other properties as well ??!
	nRa = planet_obj.any.co_ra * RAD2DEG;
	nDec = planet_obj.any.co_dec * RAD2DEG;
	// set visibility
	bVis = planet_obj.pl.plo_evis;

	return( 1 );
}





