/* this file contains routines to support Earth satellites.
 *
 * Orbit propagation is based on the NORAD SGP4/SDP4 code, as converted from
 *   the original FORTRAN to C by Magnus Backstrom. The paper "Spacetrack
 *   Report Number 3: Models for Propagation of NORAD Element Sets" describes
 *   the calculations.
 *   See http://www.celestrak.com/NORAD/documentation/spacetrk.pdf.
 *
 * A few topocentric routines are also used from the 'orbit' program which is
 *   Copyright (c) 1986,1987,1988,1989,1990 Robert W. Berger N3EMO
 *
 */

// define this to use orbit's propagator
//#define USE_ORBIT_PROPAGATOR

// define this to print some stuff
//#define ESAT_TRACE

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

//#include "../sat_code/norad.h"
#include "../sat_code/observe.h"

#include "../solar/afuncs.h"
#include "astro.h"
#include "preferences.h"

#include "vector.h"
#include "sattypes.h"
#include "satlib.h"

#include "misc.h"
#include "probfunc.h"

// main header
#include "earthsat.h"

////////////////////////////////////////////////////////////////////
// Purpose:	calculate and fill in the s_* sky fields describing the 
//			satellite. as usual, we compute the geocentric ra/dec 
//			precessed to np->n_epoch and compute topocentric altitude 
//			accounting for refraction.
// Input:	given a Now and an Obj with info about an earth satellite 
//			in the es_* fields
// Output:	return 0 if all ok, else -1.
////////////////////////////////////////////////////////////////////
int obj_earthsat( Now *np, Obj *op )
{
	double Radius;              // From geocenter                  
	double SatX,SatY,SatZ;	    // In Right Ascension based system 
	double SatVX,SatVY,SatVZ;   // Kilometers/second	       
	double SiteX,SiteY,SiteZ;
	double SiteVX,SiteVY;
	double SiteMatrix[3][3];
	double Height;
	double SSPLat,SSPLong;
	double Azimuth,Elevation,Range;
	double RangeRate;
	double dtmp;
	double CrntTime;
	double ra, dec;

// ESAT_TRACE 
#ifdef ESAT_TRACE
	printf ("\n");
	printf ("Name = %s\n", op->o_name);
	printf ("current jd = %13.5f\n", mjd+MJD0);
	printf ("current mjd = %g\n", mjd);
	printf ("satellite jd = %13.5f\n", op->es_epoch+MJD0);
	printf ("satellite mjd = %g\n", op->es_epoch);
#endif 

	// xephem uses noon 12/31/1899 as 0; orbit uses midnight 1/1/1900.
	// thus, xephem runs 12 hours, or 1/2 day, behind of what orbit wants.
	CrntTime = mjd + 0.5;

	// extract the XEphem data forms into those used by orbit.
	// (we still use some functions and names from orbit, thank you).
	InitOrbitRoutines( CrntTime, 1 );
	GetSatelliteParams( op );
	GetSiteParams( np );

	// propagate to np->n_mjd
	esat_prop( np, op, &SatX, &SatY, &SatZ, &SatVX, &SatVY, &SatVZ );
	Radius = sqrt (SatX*SatX + SatY*SatY + SatZ*SatZ);

	// find geocentric EOD equatorial directly from xyz vector
	dtmp = atan2( SatY, SatX );
	range( &dtmp, 2*PI );
	op->s_gaera = (float) dtmp;
	op->s_gaedec = (float) atan2( SatZ, sqrt(SatX*SatX + SatY*SatY) );

	// find topocentric from site location 
	GetSitPosition( SiteLat, SiteLong, SiteAltitude, CrntTime,
					&SiteX, &SiteY, &SiteZ, &SiteVX, &SiteVY, SiteMatrix );

	GetBearings( SatX, SatY, SatZ, SiteX, SiteY, SiteZ, SiteMatrix,
					&Azimuth, &Elevation );

	op->s_az = Azimuth;
	// apply atmosferic refraction
	refract( pressure, temp, Elevation, &dtmp );
	op->s_alt = dtmp;

	// Range: line-of-site distance to satellite, m
	//RangeRate: m/s
	GetRange( SiteX, SiteY, SiteZ, SiteVX, SiteVY,
	    SatX, SatY, SatZ, SatVX, SatVY, SatVZ, &Range, &RangeRate );

	// we want m 
	op->s_range = (float)(Range*1000);	
	// we want m/s 
	op->s_rangev = (float)(RangeRate*1000);
 
	// SSPLat: sub-satellite latitude, rads 
	// SSPLong: sub-satellite longitude, >0 west, rads 
	// Height: height of satellite above ground, m
	GetSubSatPoint( SatX, SatY, SatZ, CrntTime, &SSPLat, &SSPLong, &Height );

	// we want m 
	op->s_elev = (float)(Height*1000.0);	
	op->s_sublat = (float)SSPLat;
	// we want +E 
	op->s_sublng = (float)(-SSPLong);	

	op->s_eclipsed = Eclipsed( SatX,SatY,SatZ,Radius,CrntTime );

#ifdef ESAT_TRACE
	printf ("CrntTime = %g\n", CrntTime);
	printf ("SatX = %g\n", SatX);
	printf ("SatY = %g\n", SatY);
	printf ("SatZ = %g\n", SatZ);
	printf ("Radius = %g\n", Radius);
	printf ("SatVX = %g\n", SatVX);
	printf ("SatVY = %g\n", SatVY);
	printf ("SatVZ = %g\n", SatVZ);
	printf ("SiteX = %g\n", SiteX);
	printf ("SiteY = %g\n", SiteY);
	printf ("SiteZ = %g\n", SiteZ);
	printf ("SiteVX = %g\n", SiteVX);
	printf ("SiteVY = %g\n", SiteVY);
	printf ("Height = %g\n", Height);
	printf ("SSPLat = %g\n", SSPLat);
	printf ("SSPLong = %g\n", SSPLong);
	printf ("Azimuth = %g\n", Azimuth);
	printf ("Elevation = %g\n", Elevation);
	printf ("Range = %g\n", Range);
	printf ("RangeRate = %g\n", RangeRate);
	fflush (stdout);
#endif	// ESAT_TRACE 

	// find s_ra/dec, depending on current options
	if( pref_get(PREF_EQUATORIAL) == PREF_TOPO )
	{
	    double ha, lst;
	    aa_hadec( lat, Elevation, (double)op->s_az, &ha, &dec );
	    now_lst( np, &lst );
	    ra = hrrad(lst) - ha;
	    range( &ra, 2*PI );

	} else 
	{
	    ra = op->s_gaera;
	    dec = op->s_gaedec;
	}

	if( epoch != EOD ) precess( mjd, epoch, &ra, &dec );

	//////////////////
	// larry :: use calc from SAT_CODE
	// :: calculate observer coordinates
/*	double observer_loc[3];
	double rho_sin_phi, rho_cos_phi, dist_to_satellite;
	double jd = np->n_mjd+JD1900;
	double alt = SiteAltitude*1000.0;
	// Satellite position vector
	double pos[3]; 
	// set from prev calculations
	pos[0]=SatX*1000.0; pos[1]=SatY*1000.0; pos[2]=SatZ*1000.0;
	// set site position from prev calculations
	observer_loc[0] = SiteX; observer_loc[1] = SiteY; observer_loc[2] = SiteZ;

//	lat_alt_to_parallax( np->n_lat, alt, &rho_cos_phi, &rho_sin_phi );
//	observer_cartesian_coords( jd, np->n_lng, rho_cos_phi, rho_sin_phi, observer_loc );

	// calculate satellite RA/DEC & distance to satellite
	get_satellite_ra_dec_delta( observer_loc, pos, &ra, &dec, &dist_to_satellite );
	// transform to J2000
	epoch_of_date_to_j2000( jd, &ra, &dec );
	///////////////////////////////////
*/

//	double jd = np->n_mjd+JD1900;
//	epoch_of_date_to_j2000( jd, &ra, &dec );

/*	DPT ra_dec, alt_az, latlon;
	alt_az.y = op->s_alt;
	alt_az.x = op->s_az;
	latlon.y = lat;
	latlon.x = lng;

	//full_alt_az_to_ra_dec( &ra_dec, &alt_az, mjd+JD1900, &latlon );
	alt_az_to_ra_dec( op->s_alt, op->s_az, &ra, &dec, lat );
	ra = ra + green_sidereal_time( mjd+JD1900 ) + lng;
	ra = fmod( -ra, 2*PI );

	op->s_ra = (float) ra_dec.x;
	op->s_dec = (float) ra_dec.y;

*/

	op->s_ra = (float) ra;
	op->s_dec = (float) dec;


	// just make up a size and brightness 
	set_smag( op, ESAT_MAG );
	op->s_size = (float) 0.0;

	return (0);
}

// Purpose:	find position and velocity vector for given Obj at the 
//			given time. set USE_ORBIT_PROPAGATOR depending on 
//			desired propagator to use.
////////////////////////////////////////////////////////////////////
static void esat_prop( Now *np, Obj *op, double *SatX, double *SatY, 
					  double *SatZ, double *SatVX, double *SatVY, double *SatVZ )
{

#ifdef USE_ORBIT_PROPAGATOR

	double ReferenceOrbit;      // Floating point orbit # at epoch 
	double CurrentOrbit;
	long OrbitNum;
	double RAANPrecession,PerigeePrecession;
	double MeanAnomaly,TrueAnomaly;
	double SemiMajorAxis;
	double AverageMotion,       // Corrected for drag              
	    CurrentMotion;
	double Radius;
	double CrntTime;

	if (crazyOp (np, op)) {
	    *SatX = *SatY = *SatZ = *SatVX = *SatVY = *SatVZ = 0;
	    return;
	}

	SemiMajorAxis = 331.25 * exp(2*log(MinutesPerDay/epochMeanMotion)/3);
	GetPrecession(SemiMajorAxis,Eccentricity,Inclination,&RAANPrecession,
			    &PerigeePrecession);

	ReferenceOrbit = EpochMeanAnomaly/PI2 + EpochOrbitNum;
     
	CrntTime = mjd + 0.5;
	AverageMotion = epochMeanMotion + (CrntTime-EpochDay)*OrbitalDecay/2;
	CurrentMotion = epochMeanMotion + (CrntTime-EpochDay)*OrbitalDecay;

	SemiMajorAxis = 331.25 * exp(2*log(MinutesPerDay/CurrentMotion)/3);
     
	CurrentOrbit = ReferenceOrbit + (CrntTime-EpochDay)*AverageMotion;

	OrbitNum = CurrentOrbit;
     
	MeanAnomaly = (CurrentOrbit-OrbitNum)*PI2;
     
	TrueAnomaly = Kepler(MeanAnomaly,Eccentricity);

	GetSatPosition(EpochDay,EpochRAAN,EpochArgPerigee,SemiMajorAxis,
		Inclination,Eccentricity,RAANPrecession,PerigeePrecession,
		CrntTime,TrueAnomaly,SatX,SatY,SatZ,&Radius,SatVX,SatVY,SatVZ);

#ifdef ESAT_TRACE
	printf ("O Radius = %g\n", Radius);
	printf ("ReferenceOrbit = %g\n", ReferenceOrbit);
	printf ("CurrentOrbit = %g\n", CurrentOrbit);
	printf ("RAANPrecession = %g\n", RAANPrecession);
	printf ("PerigeePrecession = %g\n", PerigeePrecession);
	printf ("MeanAnomaly = %g\n", MeanAnomaly);
	printf ("TrueAnomaly = %g\n", TrueAnomaly);
	printf ("SemiMajorAxis = %g\n", SemiMajorAxis);
	printf ("AverageMotion = %g\n", AverageMotion);
	printf ("CurrentMotion = %g\n", CurrentMotion);
#endif	/* ESAT_TRACE */

// ! USE_ORBIT_PROPAGATOR 
#else	

// minutes per day 
#define	MPD		1440.0		

	SatElem se;
	SatData sd;
	Vec3 posvec, velvec;
	double dy;
	double dt;
	int yr;

	if( crazyOp( np, op ) )
	{
	    *SatX = *SatY = *SatZ = *SatVX = *SatVY = *SatVZ = 0;
	    return;
	}

	// init 
	memset ((void *)&se, 0, sizeof(se));
	memset ((void *)&sd, 0, sizeof(sd));
	sd.elem = &se;

	// se_EPOCH is packed as yr*1000 + dy, where yr is years since 1900
	// and dy is day of year, Jan 1 being 1
	mjd_dayno( op->es_epoch, &yr, &dy );
	yr -= 1900;
	dy += 1;
	se.se_EPOCH = yr*1000 + dy;

	// others carry over with some change in units
	se.se_XNO = op->es_n * (2*PI/MPD);			// revs/day to rads/min 
	se.se_XINCL = (float)degrad(op->es_inc);
	se.se_XNODEO = (float)degrad(op->es_raan);
	se.se_EO = op->es_e;
	se.se_OMEGAO = (float)degrad(op->es_ap);
	se.se_XMO = (float)degrad(op->es_M);
	se.se_BSTAR = op->es_drag;
	se.se_XNDT20 = op->es_decay*(2*PI/MPD/MPD); // rv/dy^^2 to rad/min^^2

	se.se_id.orbit = op->es_orbit;

	dt = (mjd-op->es_epoch)*MPD;

// ESAT_TRACE 
#ifdef ESAT_TRACE
	printf ("se_EPOCH  : %30.20f\n", se.se_EPOCH);
	printf ("se_XNO    : %30.20f\n", se.se_XNO);
	printf ("se_XINCL  : %30.20f\n", se.se_XINCL);
	printf ("se_XNODEO : %30.20f\n", se.se_XNODEO);
	printf ("se_EO     : %30.20f\n", se.se_EO);
	printf ("se_OMEGAO : %30.20f\n", se.se_OMEGAO);
	printf ("se_XMO    : %30.20f\n", se.se_XMO);
	printf ("se_BSTAR  : %30.20f\n", se.se_BSTAR);
	printf ("se_XNDT20 : %30.20f\n", se.se_XNDT20);
	printf ("se_orbit  : %30d\n",    se.se_id.orbit);
	printf ("dt        : %30.20f\n", dt);
#endif 

	/////////////////
	// :: compute the state vectors - based on type 
	if (se.se_XNO >= (1.0/225.0))
	    sgp4( &sd, &posvec, &velvec, dt ); // NEO
	else
	    sdp4( &sd, &posvec, &velvec, dt ); // GEO 

	// free allocator ?
	if (sd.prop.sgp4)
	    free( sd.prop.sgp4 );	// sd.prop.sdp4 is in same union
	if (sd.deep)
	    free( sd.deep );

	*SatX = ERAD*posvec.x/1000.;	/* earth radii to km */
	*SatY = ERAD*posvec.y/1000.;
	*SatZ = ERAD*posvec.z/1000.;
	*SatVX = 100.*velvec.x;		/* ?? */
	*SatVY = 100.*velvec.y;
	*SatVZ = 100.*velvec.z;
#endif

}

// return 1 if op is crazy @ np 
static int crazyOp( Now *np, Obj *op )
{
	// figure its daft if long enough to decay 1 rev/day 
	return (fabs(op->es_epoch - mjd) > 1/fabs(op->es_decay));
}

////////////////////////////////////////////////////////////////////
// Method:	GetSatelliteParams
// Purpose:	grab the xephem stuff from op and copy into orbit's globals.
////////////////////////////////////////////////////////////////////
static void GetSatelliteParams( Obj *op )
{
	// the following are for the orbit functions 
	// xephem uses noon 12/31/1899 as 0; orbit uses midnight 1/1/1900 as 1.
	// thus, xephem runs 12 hours, or 1/2 day, behind of what orbit wants.
	EpochDay = op->es_epoch + 0.5;

	// xephem stores inc in degrees; orbit wants rads 
	Inclination = degrad( op->es_inc );

	// xephem stores RAAN in degrees; orbit wants rads
	EpochRAAN = degrad( op->es_raan );

	Eccentricity = op->es_e;

	// xephem stores arg of perigee in degrees; orbit wants rads
	EpochArgPerigee = degrad( op->es_ap );

	// xephem stores mean anomaly in degrees; orbit wants rads 
	EpochMeanAnomaly = degrad( op->es_M );

	epochMeanMotion = op->es_n;

	OrbitalDecay = op->es_decay;

	EpochOrbitNum = op->es_orbit;
}

////////////////////////////////////////////////////////////////////
static void GetSiteParams( Now *np )
{
	SiteLat = lat;
     
	// xephem stores longitude as >0 east; orbit wants >0 west
	SiteLong = 2.0*PI - lng;
     
	// what orbit calls altitude xephem calls elevation and stores it from
	// sea level in earth radii; orbit wants km
	SiteAltitude = elev*ERAD/1000.0;
     
	/* we don't implement a minimum horizon altitude cutoff
	SiteMinElev = 0;
	 */

#ifdef ESAT_TRACE
	printf ("SiteLat = %g\n", SiteLat);
	printf ("SiteLong = %g\n", SiteLong);
	printf ("SiteAltitude = %g\n", SiteAltitude);
	fflush (stdout);
#endif
}

 
/* Solve Kepler's equation                                      */
/* Inputs:                                                      */
/*      MeanAnomaly     Time Since last perigee, in radians.    */
/*                      PI2 = one complete orbit.               */
/*      Eccentricity    Eccentricity of orbit's ellipse.        */
/* Output:                                                      */
/*      TrueAnomaly     Angle between perigee, geocenter, and   */
/*                      current position.                       */
 
static double Kepler( double MeanAnomaly, double Eccentricity )
{
register double E;              /* Eccentric Anomaly                    */
register double Error;
register double TrueAnomaly;
 
    E = MeanAnomaly ;/*+ Eccentricity*sin(MeanAnomaly);  -- Initial guess */
    do
        {
        Error = (E - Eccentricity*sin(E) - MeanAnomaly)
                / (1 - Eccentricity*cos(E));
        E -= Error;
        }
   while (ABS(Error) >= Epsilon);

    if (ABS(E-PI) < Epsilon)
        TrueAnomaly = PI;
      else
        TrueAnomaly = 2*atan(sqrt((1+Eccentricity)/(1-Eccentricity))
                                *tan(E/2));
    if (TrueAnomaly < 0)
        TrueAnomaly += PI2;
 
    return TrueAnomaly;
}

////////////////////////////////////////////////////////////////////
static void GetSubSatPoint( double SatX, double SatY, double SatZ, double T,
							double *Latitude, double *Longitude, double *Height )
{
    double r;
    // ECD: long i; 

    r = sqrt(SQR(SatX) + SQR(SatY) + SQR(SatZ));

    *Longitude = PI2*((T-SidDay)*SiderealSolar + SidReference)
		    - atan2(SatY,SatX);

    // ECD: want Longitude in range -PI to PI , +W
    range (Longitude, 2*PI);
    if (*Longitude > PI)
	*Longitude -= 2*PI;

    *Latitude = atan(SatZ/sqrt(SQR(SatX) + SQR(SatY)));

#if SSPELLIPSE
#else
    *Height = r - EarthRadius;
#endif
}
 
// USE_ORBIT_PROPAGATOR 
#ifdef USE_ORBIT_PROPAGATOR
static void
GetPrecession(double SemiMajorAxis, double Eccentricity, double Inclination,
double *RAANPrecession, double *PerigeePrecession)
{
  *RAANPrecession = 9.95*pow(EarthRadius/SemiMajorAxis,3.5) * cos(Inclination)
                 / SQR(1-SQR(Eccentricity)) * RadiansPerDegree;
 
  *PerigeePrecession = 4.97*pow(EarthRadius/SemiMajorAxis,3.5)
         * (5*SQR(cos(Inclination))-1)
                 / SQR(1-SQR(Eccentricity)) * RadiansPerDegree;
}
#endif 
 
/* Compute the satellite postion and velocity in the RA based coordinate
 * system.
 * ECD: take care not to let Radius get below EarthRadius.
 */
////////////////////////////////////////////////////////////////////
static void GetSatPosition( double EpochTime, double EpochRAAN, double EpochArgPerigee,
							double SemiMajorAxis, double Inclination, double Eccentricity,
							double RAANPrecession, double PerigeePrecession, double T,
							double TrueAnomaly, double *X, double *Y, double *Z, double *Radius,
							double *VX, double *VY, double *VZ )
{
    double RAAN,ArgPerigee;
 
	// In orbital plane 
    double Xw,Yw,VXw,VYw;	
    double Tmp;
	// Escobal transformation 31 
    double Px,Qx,Py,Qy,Pz,Qz;	
    double CosArgPerigee,SinArgPerigee;
    double CosRAAN,SinRAAN,CoSinclination,SinInclination;

    *Radius = SemiMajorAxis*(1-SQR(Eccentricity))/(1+Eccentricity*cos(TrueAnomaly));

    if (*Radius <= EarthRadius) *Radius = EarthRadius;

    Xw = *Radius * cos(TrueAnomaly);
    Yw = *Radius * sin(TrueAnomaly);
    
    Tmp = sqrt(GM/(SemiMajorAxis*(1-SQR(Eccentricity))));

    VXw = -Tmp*sin(TrueAnomaly);
    VYw = Tmp*(cos(TrueAnomaly) + Eccentricity);

    ArgPerigee = EpochArgPerigee + (T-EpochTime)*PerigeePrecession;
    RAAN = EpochRAAN - (T-EpochTime)*RAANPrecession;

    CosRAAN = cos(RAAN); SinRAAN = sin(RAAN);
    CosArgPerigee = cos(ArgPerigee); SinArgPerigee = sin(ArgPerigee);
    CoSinclination = cos(Inclination); SinInclination = sin(Inclination);
    
    Px = CosArgPerigee*CosRAAN - SinArgPerigee*SinRAAN*CoSinclination;
    Py = CosArgPerigee*SinRAAN + SinArgPerigee*CosRAAN*CoSinclination;
    Pz = SinArgPerigee*SinInclination;
    Qx = -SinArgPerigee*CosRAAN - CosArgPerigee*SinRAAN*CoSinclination;
    Qy = -SinArgPerigee*SinRAAN + CosArgPerigee*CosRAAN*CoSinclination;
    Qz = CosArgPerigee*SinInclination;

	// Escobal, transformation #31 
    *X = Px*Xw + Qx*Yw;		
    *Y = Py*Xw + Qy*Yw;
    *Z = Pz*Xw + Qz*Yw;

    *VX = Px*VXw + Qx*VYw;
    *VY = Py*VXw + Qy*VYw;
    *VZ = Pz*VXw + Qz*VYw;
}

/* Compute the site postion and velocity in the RA based coordinate
   system. SiteMatrix is set to a matrix which is used by GetTopoCentric
   to convert geocentric coordinates to topocentric (observer-centered)
    coordinates. */
static void GetSitPosition( double SiteLat, double SiteLong, double SiteElevation,
							double CrntTime, double *SiteX, double *SiteY, double *SiteZ, 
							double *SiteVX, double *SiteVY, MAT3x3 SiteMatrix )
{
    static double G1,G2; /* Used to correct for flattening of the Earth */
    static double CosLat,SinLat;
    static double OldSiteLat = -100000;  /* Used to avoid unneccesary recomputation */
    static double OldSiteElevation = -100000;
    double Lat;
    double SiteRA;	/* Right Ascension of site			*/
    double CosRA,SinRA;

    if ((SiteLat != OldSiteLat) || (SiteElevation != OldSiteElevation))
	{
		OldSiteLat = SiteLat;
		OldSiteElevation = SiteElevation;
		Lat = atan(1/(1-SQR(EarthFlat))*tan(SiteLat));

		CosLat = cos(Lat);
		SinLat = sin(Lat);

		G1 = EarthRadius/(sqrt(1-(2*EarthFlat-SQR(EarthFlat))*SQR(SinLat)));
		G2 = G1*SQR(1-EarthFlat);
		G1 += SiteElevation;
		G2 += SiteElevation;
	}


    SiteRA = PI2*((CrntTime-SidDay)*SiderealSolar + SidReference)
	         - SiteLong;
    CosRA = cos(SiteRA);
    SinRA = sin(SiteRA);
    

    *SiteX = G1*CosLat*CosRA;
    *SiteY = G1*CosLat*SinRA;
    *SiteZ = G2*SinLat;
    *SiteVX = -SidRate * *SiteY;
    *SiteVY = SidRate * *SiteX;

    SiteMatrix[0][0] = SinLat*CosRA;
    SiteMatrix[0][1] = SinLat*SinRA;
    SiteMatrix[0][2] = -CosLat;
    SiteMatrix[1][0] = -SinRA;
    SiteMatrix[1][1] = CosRA;
    SiteMatrix[1][2] = 0.0;
    SiteMatrix[2][0] = CosRA*CosLat;
    SiteMatrix[2][1] = SinRA*CosLat;
    SiteMatrix[2][2] = SinLat;
}

////////////////////////////////////////////////////////////////////
static void GetRange( double SiteX, double SiteY, double SiteZ, double SiteVX,
					double SiteVY, double SatX, double SatY, double SatZ, double SatVX,
					double SatVY, double SatVZ, double *Range, double *RangeRate )
{
    double DX,DY,DZ;

    DX = SatX - SiteX; DY = SatY - SiteY; DZ = SatZ - SiteZ;

    *Range = sqrt(SQR(DX)+SQR(DY)+SQR(DZ));    

    *RangeRate = ((SatVX-SiteVX)*DX + (SatVY-SiteVY)*DY + SatVZ*DZ) / *Range;
}

// Purpose:	Convert from geocentric RA based coordinates to topocentric 
//			(observer centered) coordinates 
////////////////////////////////////////////////////////////////////
static void GetTopocentric( double SatX, double SatY, double SatZ, double SiteX,
							double SiteY, double SiteZ, 
							MAT3x3 SiteMatrix, double *X, double *Y, double *Z )
{
    SatX -= SiteX;
    SatY -= SiteY;
    SatZ -= SiteZ;

    *X = SiteMatrix[0][0]*SatX + SiteMatrix[0][1]*SatY
	+ SiteMatrix[0][2]*SatZ; 
    *Y = SiteMatrix[1][0]*SatX + SiteMatrix[1][1]*SatY
	+ SiteMatrix[1][2]*SatZ; 
    *Z = SiteMatrix[2][0]*SatX + SiteMatrix[2][1]*SatY
	+ SiteMatrix[2][2]*SatZ; 
}

////////////////////////////////////////////////////////////////////
static void GetBearings( double SatX, double SatY, double SatZ, double SiteX,
						double SiteY, double SiteZ, MAT3x3 SiteMatrix, 
						double *Azimuth, double *Elevation )
{
    double x,y,z;

    GetTopocentric(SatX,SatY,SatZ,SiteX,SiteY,SiteZ,SiteMatrix,&x,&y,&z);

    *Elevation = atan(z/sqrt(SQR(x) + SQR(y)));

    *Azimuth = PI - atan2(y,x);

    if (*Azimuth < 0)
	*Azimuth += PI;
}

////////////////////////////////////////////////////////////////////
static int Eclipsed( double SatX, double SatY, double SatZ, double SatRadius, double CrntTime )
{
    double MeanAnomaly, TrueAnomaly;
    double SunX, SunY, SunZ, SunRad;
    double vx, vy, vz;
    double CosTheta;

    MeanAnomaly = SunMeanAnomaly+ (CrntTime-SunEpochTime)*SunMeanMotion*PI2;
    TrueAnomaly = Kepler(MeanAnomaly,SunEccentricity);

    GetSatPosition( SunEpochTime,SunRAAN,SunArgPerigee,SunSemiMajorAxis,
		SunInclination, SunEccentricity, 0.0, 0.0, CrntTime,
		TrueAnomaly, &SunX, &SunY, &SunZ, &SunRad, &vx, &vy, &vz );

    CosTheta = (SunX*SatX + SunY*SatY + SunZ*SatZ)/(SunRad*SatRadius)
		 *CosPenumbra + (SatRadius/EarthRadius)*SinPenumbra;

    if (CosTheta < 0)
        if (CosTheta < -sqrt(SQR(SatRadius)-SQR(EarthRadius))/SatRadius
	    		*CosPenumbra + (SatRadius/EarthRadius)*SinPenumbra)
	    return 1;
    return 0;
}

// Purpose:	Initialize the Sun's keplerian elements for a given epoch.
//			Formulas are from "Explanatory Supplement to the 
//			Astronomical Ephemeris". Also init the sidereal reference
////////////////////////////////////////////////////////////////////
static void InitOrbitRoutines( double EpochDay, int AtEod )
{
    double T, T2, T3, Omega;
    int n;
    double SunTrueAnomaly, SunDistance;

    T = (floor(EpochDay)-0.5)/36525;
    T2 = T*T;
    T3 = T2*T;

    SidDay = floor(EpochDay);

    SidReference = (6.6460656 + 2400.051262*T + 0.00002581*T2)/24;
    SidReference -= floor(SidReference);

    // Omega is used to correct for the nutation and the abberation 
    Omega = AtEod ? (259.18 - 1934.142*T) * RadiansPerDegree : 0.0;
    n = (int)(Omega/PI2);
    Omega -= n*PI2;

    SunEpochTime = EpochDay;
    SunRAAN = 0;

    SunInclination = (23.452294 - 0.0130125*T - 0.00000164*T2 + 0.000000503*T3 +0.00256*cos(Omega)) * RadiansPerDegree;
    SunEccentricity = (0.01675104 - 0.00004180*T - 0.000000126*T2);
    SunArgPerigee = (281.220833 + 1.719175*T + 0.0004527*T2 + 0.0000033*T3) * RadiansPerDegree;
    SunMeanAnomaly = (358.475845 + 35999.04975*T - 0.00015*T2 - 0.00000333333*T3) * RadiansPerDegree;
    n = (int)(SunMeanAnomaly/PI2);
    SunMeanAnomaly -= n*PI2;

    SunMeanMotion = 1/(365.24219879 - 0.00000614*T);

    SunTrueAnomaly = Kepler( SunMeanAnomaly, SunEccentricity );
    SunDistance = SunSemiMajorAxis*(1-SQR(SunEccentricity)) / (1+SunEccentricity*cos(SunTrueAnomaly));

    SinPenumbra = (SunRadius-EarthRadius)/SunDistance;
    CosPenumbra = sqrt(1-SQR(SinPenumbra));
}

