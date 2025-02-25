////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _SOLAR_PLANETS_H
#define _SOLAR_PLANETS_H

// local include
//#include "./star.h"
#include "catalog.h"
#include "catalog_types.h"

////////////////////////
// planet structure
typedef struct
{
	wxChar obj_name[255];

	unsigned int obj_id;
	// type: 0 - planet, 1 - satellite of a planet
	unsigned char obj_type;
	// object relation - 0 sun - else the if of planet orbiting
	unsigned int obj_orb_id;

	// x/y in image
	double x;
	double y;

	// ra/dec - sky coordinates
	double ra;
	double dec;

	// extact time shift for detection
	double detect_time_shift;
	// distance from observer
	double dist_from_obs;

	// other properties
	double mass;
	double volume;

} DefPlanet;

// global no of solar object aka planets/sats/sun
extern int g_nSolarObjects;

// initialize solar planets vector
void InitSolarPlanets( DefPlanet* vectPlanets );
void InitSolar_Planet( DefPlanet& planet, int obj_id, int obj_orb_id, int obj_type, const wxChar* obj_name );
DefPlanet* GetSolarPlanetObjById( DefPlanet* vectPlanets, int nPlanetId );

// reset to zero function
//void ResetObjPlanet( DefPlanet& src );

int Get_Solar_Object_Name( DefPlanet& solar, wxString& strSolarName );

// calculate ra/dec for a solar planet
int SolarPlanet_CalcRaDec( DefPlanet& planet, double nTime, double nLat, double nLon, double nAlt, 
							double& nRa, double& nDec, double& nDist, int& bVis );

#endif

