/* given a Now and an Obj with the object definition portion filled in,
 * fill in the sky position (s_*) portions.
 * calculation of positional coordinates reworked by
 * Michael Sternberg <sternberg@physik.tu-chemnitz.de>
 *  3/11/98: deflect was using op->s_hlong before being set in cir_pos().
 *  4/19/98: just edit a comment
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

//#include "libastro.h"
#include "astro.h"
#include "preferences.h"

#include "earthsat.h"
#include "misc.h" 
#include "probfunc.h"

////////////////////////////////////////////////////////////////////
// Method:	Sol_CalcObjOrbitPos
// Class:	CSolarOrbit
// Purpose:	given a Now and an Obj, fill in the approprirate s_* 
//			fields within Obj.
// Input:	Now - as obsever position/time, Obj - object orbit params
// Output:	return 0 if all ok, else -1.
////////////////////////////////////////////////////////////////////
int Sol_CalcObjOrbitPos( Now *np, Obj *op )
{
	op->o_flags &= ~NOCIRCUM;

	switch (op->o_type)
	{
		case BINARYSTAR: return( obj_binary( np, op ) );
		case FIXEDOBJ:	 return( obj_fixed( np, op ) );
		case ELLIPTICAL: return( Sol_CalcObjEllipticalPos( np, op ) );
		case HYPERBOLIC: return( obj_hyperbolic( np, op ) );
		case PARABOLIC:  return( Sol_CalcObjParabolicPos( np, op ) );
		case EARTHSAT:   return( obj_earthsat( np, op ) );
		case PLANET:     return( obj_planet( np, op ) );
		default:
			printf ( "obj_cir() called with type %d %s\n", op->o_type, op->o_name );
			abort();
			return( -1 );	/* just for lint */
	}
}

////////////////////////////////////////////////////////////////////
static int obj_planet( Now *np, Obj *op )
{
	double lsn, rsn;	// true geoc lng of sun; dist from sn to earth
	double lpd, psi;	// heliocentric ecliptic long and lat 
	double rp;			// dist from sun
	double rho;			// dist from earth 
	double lam, bet;	// geocentric ecliptic long and lat
	double dia, mag;	// angular diameter at 1 AU and magnitude
	PLCode p;

	// validate code and check for a few special cases 
	p = op->pl_code;
	if( p == SUN )
	    return( sun_cir( np, op ) );
	if( p == MOON )
	    return( moon_cir( np, op ) );
	if( op->pl_moon != X_PLANET )
	    return( plmoon_cir( np, op ) );

	if( p < 0 || p > MOON )
	{
	    printf ( "unknown planet code: %d\n", p );
	    abort();
	}

	// planet itself 

	// find solar ecliptical longitude and distance to sun from earth
	sunpos( mjed, &lsn, &rsn, 0 );

	// find helio long/lat; sun/planet and earth/planet dist; ecliptic
	// long/lat; diameter and mag.
	plans( mjed, p, &lpd, &psi, &rp, &rho, &lam, &bet, &dia, &mag );

	// fill in all of op->s_* stuff except s_size and s_mag 
	cir_sky( np, lpd, psi, rp, &rho, lam, bet, lsn, rsn, op );

	// set magnitude and angular size 
	set_smag (op, mag);
	op->s_size = (float)(dia/rho);

	return (0);
}

////////////////////////////////////////////////////////////////////
static int obj_binary( Now *np, Obj *op )
{
	/* always compute circumstances of primary */
	if (obj_fixed (np, op) < 0)
	    return (0);

	/* compute secondary only if requested, and always reset request flag */
	if (!op->b_2compute)
	    return (0);
	op->b_2compute = 0;
	return (obj_2binary (np, op));
}

// Purpose:	compute position of secondary component of a BINARYSTAR
static int obj_2binary( Now *np, Obj *op )
{
	if (op->b_nbp > 0)
	{
	    // we just have discrete pa/sep, project each from primary
	    int i;
	    for (i = 0; i < op->b_nbp; i++)
		{
			BinPos *bp = &op->b_bp[i];
			bp->bp_dec = op->s_dec + bp->bp_sep*cos(bp->bp_pa);
			bp->bp_ra = op->s_ra + bp->bp_sep*sin(bp->bp_pa)/cos(op->s_dec);
	    }
	} else
	{
	    BinOrbit *bp = &op->b_bo;
	    double t, theta, rho;

	    mjd_year (mjd, &t);
	    binaryStarOrbit (t, bp->bo_T, bp->bo_e, bp->bo_o, bp->bo_O,
				bp->bo_i, bp->bo_a, bp->bo_P, &theta, &rho);
	    bp->bo_pa = (float)theta;
	    bp->bo_sep = (float)rho;
		// arc secs to rads 
	    rho = degrad(rho/3600.);	
	    bp->bo_dec = op->s_dec + rho*cos(theta);
	    bp->bo_ra =  op->s_ra  + rho*sin(theta)/cos(op->s_dec);
	}

	return (0);
}

////////////////////////////////////////////////////////////////////
// Method:	binaryStarOrbit
// Purpose:	from W. M. Smart
////////////////////////////////////////////////////////////////////
static void binaryStarOrbit(
double t,		/* desired ephemeris epoch, year */
double T,		/* epoch of periastron, year */
double e,		/* eccentricity */
double o,		/* argument of periastron, degrees */
double O,		/* ascending node, degrees */
double i,		/* inclination, degrees */
double a,		/* semi major axis, arcsecs */
double P,		/* period, years */
double *thetap,		/* position angle, rads E of N */
double *rhop)		/* separation, arcsecs */
{
	double M, E, cosE, nu, cosnu, r, rho, theta;

	/* find mean anomaly, insure 0..2*PI */
	M = 2*PI/P*(t-T);
	range (&M, 2*PI);

	/* solve for eccentric anomaly */
	E = solveKepler (M, e);
	cosE = cos(E);

	/* find true anomaly and separation */
	cosnu = (cosE - e)/(1.0 - e*cosE);
	r = a*(1.0 - e*e)/(1.0 + e*cosnu);
	nu = acos(cosnu);
	if (E > PI)
	    nu = -nu;

	/* project onto sky */
	theta = atan(tan(nu+degrad(o))*cos(degrad(i))) + degrad(O);
	rho = r*cos(nu+degrad(o))/cos(theta-degrad(O));
	if (rho < 0) {
	    theta += PI;
	    rho = -rho;
	}
	range (&theta, 2*PI);

	*thetap = theta;
	*rhop = rho;
}

////////////////////////////////////////////////////////////////////
// Method:	solveKepler
// Purpose:	solve kepler equation using Newton-Raphson search. 
//			Charles and Tatum have shown it always converges 
//			starting with PI.
////////////////////////////////////////////////////////////////////
static double solveKepler( double M, double e )
{
	double E, Eprime = PI;

	do {
	    double cosE = cos(Eprime);
	    E = Eprime;
	    Eprime = (M - e*(E*cosE - sin(E)))/(1.0 - e*cosE);
	} while (fabs(E-Eprime) > 1e-7);

	return (Eprime);
}

static int obj_fixed( Now *np, Obj *op )
{
	double lsn, rsn;	/* true geoc lng of sun, dist from sn to earth*/
	double lam, bet;	/* geocentric ecliptic long and lat */
	double ha;		/* local hour angle */
	double el;		/* elongation */
	double alt, az;		/* current alt, az */
	double ra, dec;		/* ra and dec at equinox of date */
	double rpm, dpm; 	/* astrometric ra and dec with PM to now */
	double lst;

	/* on the assumption that the user will stick with their chosen display
	 * epoch for a while, we move the defining values to match and avoid
	 * precession for every call until it is changed again.
	 * N.B. only compare and store jd's to lowest precission (f_epoch).
	 * N.B. maintaining J2k ref (which is arbitrary) helps avoid accum err
	 */
	if (epoch != EOD && (float)epoch != (float)op->f_epoch) {
	    double pr = op->f_RA, pd = op->f_dec, fe = (float)epoch;
	    /* first bring back to 2k */
	    precess (op->f_epoch, MJ2000, &pr, &pd);
	    pr += op->f_pmRA*(MJ2000-op->f_epoch);
	    pd += op->f_pmdec*(MJ2000-op->f_epoch);
	    /* then to epoch */
	    pr += op->f_pmRA*(fe-MJ2000);
	    pd += op->f_pmdec*(fe-MJ2000);
	    precess (MJ2000, fe, &pr, &pd);
	    op->f_RA = (float)pr;
	    op->f_dec = (float)pd;
	    op->f_epoch = (float)fe;
	}

	/* apply proper motion .. assume pm epoch reference equals equinox */
	rpm = op->f_RA + op->f_pmRA*(mjd-op->f_epoch);
	dpm = op->f_dec + op->f_pmdec*(mjd-op->f_epoch);

	/* set ra/dec to astrometric @ equinox of date */
	ra = rpm;
	dec = dpm;
	precess (op->f_epoch, mjed, &ra, &dec);

	/* convert equatoreal ra/dec to mean geocentric ecliptic lat/long */
	eq_ecl (mjed, ra, dec, &bet, &lam);

	/* find solar ecliptical long.(mean equinox) and distance from earth */
	sunpos (mjed, &lsn, &rsn, NULL);

	/* allow for relativistic light bending near the sun */
	deflect (mjed, lam, bet, lsn, rsn, 1e10, &ra, &dec);

	/* TODO: correction for annual parallax would go here */

	/* correct EOD equatoreal for nutation/aberation to form apparent 
	 * geocentric
	 */
	nut_eq(mjed, &ra, &dec);
	ab_eq(mjed, lsn, &ra, &dec);
	op->s_gaera = (float)ra;
	op->s_gaedec = (float)dec;

	/* set s_ra/dec -- apparent if EOD else astrometric */
	if (epoch == EOD) {
	    op->s_ra = (float)ra;
	    op->s_dec = (float)dec;
	} else {
	    /* annual parallax at time mjd is to be added here, too, but
	     * technically in the frame of equinox (usually different from mjd)
	     */
	    op->s_ra = rpm;
	    op->s_dec = dpm;
	}

	/* compute elongation from ecliptic long/lat and sun geocentric long */
	elongation (lam, bet, lsn, &el);
	el = raddeg(el);
	op->s_elong = (float)el;

	/* these are really the same fields ...
	op->s_mag = op->f_mag;
	op->s_size = op->f_size;
	*/

	/* alt, az: correct for refraction; use eod ra/dec. */
	now_lst (np, &lst);
	ha = hrrad(lst) - ra;
	hadec_aa (lat, ha, dec, &alt, &az);
	refract (pressure, temp, alt, &alt);
	op->s_alt = alt;
	op->s_az = az;

	return (0);
}

// Method:	Sol_CalcObjEllipticalPos
// Purpose:	compute sky circumstances of an object in heliocentric 
//			elliptic orbit at *np.
////////////////////////////////////////////////////////////////////
static int Sol_CalcObjEllipticalPos( Now *np, Obj *op )
{
	double lsn=0.0, rsn=0.0;		// true geoc lng of sun; dist from sn to earth
	double dt=0.0;				// light travel time to object 
	double lg=0.0;				// helio long of earth 
	double nu=0.0;				// true anomaly 
	double rp=0;			// distance from the sun 
	double lo=0.0, slo=0.0, clo=0.0;	// angle from ascending node 
	double inc=0.0;				// inclination 
	double psi=0;			// heliocentric latitude 
	double spsi=0, cpsi=0;	// trig of heliocentric latitude 
	double lpd=0.0; 			// heliocentric longitude 
	double rho=0;			// distance from the Earth 
	double om=0.0;				// arg of perihelion 
	double Om=0.0;				// long of ascending node. 
	double lam=0.0;				// geocentric ecliptic longitude 
	double bet=0.0;				// geocentric ecliptic latitude 
	double ll=0, sll=0.0, cll=0.0;	// helio angle between object and earth 
	double mag=0.0;				// magnitude 
	double e_n=0.0;				// mean daily motion 
	double tp=0.0;				// time from perihelion (days) 
	double rpd=0;
	double y=0.0;
	int pass=0;

	// find location of earth from sun now 
	sunpos( mjed, &lsn, &rsn, 0 );
	lg = lsn + PI;

	// mean daily motion is derived fro mean distance
	e_n = 0.9856076686/pow((double)op->e_a, 1.5);

	// correct for light time by computing position at time mjd, then
	//   again at mjd-dt, where
	//   dt = time it takes light to travel earth-object distance.
	dt = 0;
	for( pass = 0; pass < 2; pass++ )
	{

	    Sol_ReduceElements( op->e_epoch, mjd-dt, degrad(op->e_inc),
						degrad(op->e_om), degrad(op->e_Om),
						&inc, &om, &Om );

	    tp = mjed - dt - (op->e_cepoch - op->e_M/e_n);

	    if( vrc( &nu, &rp, tp, op->e_e, op->e_a*(1-op->e_e) ) < 0 ) op->o_flags |= NOCIRCUM;
	    nu = degrad( nu );
	    lo = nu + om;
	    slo = sin(lo);
	    clo = cos(lo);
	    spsi = slo*sin(inc);
	    y = slo*cos(inc);
	    psi = asin(spsi);
	    lpd = atan(y/clo)+Om;

	    if (clo<0) lpd += PI;
	    range (&lpd, 2*PI);
	    cpsi = cos(psi);
	    rpd = rp*cpsi;
	    ll = lpd-lg;
	    rho = sqrt( rsn*rsn+rp*rp-2*rsn*rp*cpsi*cos(ll) );

		// light travel time, in days / AU 
	    dt = rho*LTAU/3600.0/24.0;
	}

	// compute sin and cos of ll 
	sll = sin(ll);
	cll = cos(ll);

	// find geocentric ecliptic longitude and latitude 
	if( rpd < rsn )
	    lam = atan(-1*rpd*sll/(rsn-rpd*cll))+lg+PI;
	else
	    lam = atan(rsn*sll/(rpd-rsn*cll))+lpd;
	range( &lam, 2*PI );
	bet = atan(rpd*spsi*sin(lam-lpd)/(cpsi*rsn*sll));

	// fill in all of op->s_* stuff except s_size and s_mag 
	cir_sky( np, lpd, psi, rp, &rho, lam, bet, lsn, rsn, op );

	// compute magnitude and size
	if( op->e_mag.whichm == MAG_HG )
	{
	    // the H and G parameters from the Astro. Almanac.
	    if( op->e_size )
			op->s_size = (float)(op->e_size / rho);
	    else
		{
			hg_mag( op->e_mag.m1, op->e_mag.m2, rp, rho, rsn, &mag );
			op->s_size = (float)(h_albsize (op->e_mag.m1)/rho);
	    }

	} else
	{
	    // the g/k model of comets 
	    gk_mag( op->e_mag.m1, op->e_mag.m2, rp, rho, &mag );
	    op->s_size = (float)(op->e_size / rho);
	}
	set_smag (op, mag);

	return (0);
}

// compute sky circumstances of an object in heliocentric hyperbolic orbit.
////////////////////////////////////////////////////////////////////
static int obj_hyperbolic( Now *np, Obj *op )
{
	double lsn=0.0, rsn=0.0;	/* true geoc lng of sun; dist from sn to earth*/
	double dt=0.0;		/* light travel time to object */
	double lg=0.0;		/* helio long of earth */
	double nu=0.0;		/* true anomaly and eccentric anomaly */
	double rp=0;		/* distance from the sun */
	double lo=0.0, slo=0.0, clo=0.0;	/* angle from ascending node */
	double inc=0.0;		/* inclination */
	double psi=0.0;		/* heliocentric latitude */
	double spsi=0.0, cpsi=0.0;	/* trig of heliocentric latitude */
	double lpd=0.0; 		/* heliocentric longitude */
	double rho=0.0;		/* distance from the Earth */
	double om=0.0;		/* arg of perihelion */
	double Om=0.0;		/* long of ascending node. */
	double lam=0.0;    		/* geocentric ecliptic longitude */
	double bet=0.0;    		/* geocentric ecliptic latitude */
	double e=0.0;		/* fast eccentricity */
	double ll=0.0, sll=0.0, cll=0.0;	/* helio angle between object and earth */
	double mag=0.0;		/* magnitude */
	double a=0.0;		/* mean distance */
	double tp=0.0;		/* time from perihelion (days) */
	double rpd=0.0;
	double y=0.0;
	int pass=0;

	/* find solar ecliptical longitude and distance to sun from earth */
	sunpos (mjed, &lsn, &rsn, 0);

	lg = lsn + PI;
	e = op->h_e;
	a = op->h_qp/(e - 1.0);

	/* correct for light time by computing position at time mjd, then
	 *   again at mjd-dt, where
	 *   dt = time it takes light to travel earth-object distance.
	 */
	dt = 0;
	for (pass = 0; pass < 2; pass++)
	{

	    Sol_ReduceElements( op->h_epoch, mjd-dt, degrad(op->h_inc),
			    degrad (op->h_om), degrad (op->h_Om), &inc, &om, &Om );

	    tp = mjed - dt - op->h_ep;
	    if (vrc (&nu, &rp, tp, op->h_e, op->h_qp) < 0)
		op->o_flags |= NOCIRCUM;
	    nu = degrad(nu);
	    lo = nu + om;
	    slo = sin(lo);
	    clo = cos(lo);
	    spsi = slo*sin(inc);
	    y = slo*cos(inc);
	    psi = asin(spsi);
	    lpd = atan(y/clo)+Om;
	    if (clo<0) lpd += PI;
	    range (&lpd, 2*PI);
	    cpsi = cos(psi);
	    rpd = rp*cpsi;
	    ll = lpd-lg;
	    rho = sqrt(rsn*rsn+rp*rp-2*rsn*rp*cpsi*cos(ll));

	    dt = rho*5.775518e-3;	/* light travel time, in days */
	}

	/* compute sin and cos of ll */
	sll = sin(ll);
	cll = cos(ll);

	/* find geocentric ecliptic longitude and latitude */
	if (rpd < rsn)
	    lam = atan(-1*rpd*sll/(rsn-rpd*cll))+lg+PI;
	else
	    lam = atan(rsn*sll/(rpd-rsn*cll))+lpd;
	range (&lam, 2*PI);
	bet = atan(rpd*spsi*sin(lam-lpd)/(cpsi*rsn*sll));

	/* fill in all of op->s_* stuff except s_size and s_mag */
	cir_sky (np, lpd, psi, rp, &rho, lam, bet, lsn, rsn, op);

	/* compute magnitude and size */
	gk_mag (op->h_g, op->h_k, rp, rho, &mag);
	set_smag (op, mag);
	op->s_size = (float)(op->h_size / rho);

	return (0);
}

// Purpose:	compute sky circumstances of an object in heliocentric 
//			parabolic orbit.
////////////////////////////////////////////////////////////////////
static int Sol_CalcObjParabolicPos( Now *np, Obj *op )
{
	double lsn=0.0, rsn=0.0;		// true geoc lng of sun; dist from sn to earth
	double lam=0.0;				// geocentric ecliptic longitude 
	double bet=0.0;				// geocentric ecliptic latitude 
	double mag=0.0;				// magnitude 
	double inc=0.0, om=0.0, Om=0.0;
	double lpd=0.0, psi=0.0, rp=0.0, rho=0.0;
	double dt=0.0;
	int pass=0;

	// find solar ecliptical longitude and distance to sun from earth 
	sunpos( mjed, &lsn, &rsn, 0 );

	// two passes to correct lam and bet for light travel time. 
	dt = 0.0;
	for( pass = 0; pass < 2; pass++ )
	{
	    Sol_ReduceElements( op->p_epoch, mjd-dt, degrad(op->p_inc),
							degrad(op->p_om), degrad(op->p_Om), &inc, &om, &Om );
	    GetCometPos( mjed-dt, op->p_ep, inc, om, op->p_qp, Om,
				    &lpd, &psi, &rp, &rho, &lam, &bet );
		// light travel time, in days / AU 
	    dt = rho*LTAU/3600.0/24.0;
	}

	// fill in all of op->s_* stuff except s_size and s_mag 
	cir_sky( np, lpd, psi, rp, &rho, lam, bet, lsn, rsn, op );

	// compute magnitude and size 
	gk_mag( op->p_g, op->p_k, rp, rho, &mag );
	set_smag( op, mag );
	op->s_size = (float)(op->p_size / rho);

	return (0);
}

// Purpose:	find sun's circumstances now.
////////////////////////////////////////////////////////////////////
static int sun_cir( Now *np, Obj *op )
{
	double lsn, rsn;	/* true geoc lng of sun; dist from sn to earth*/
	double bsn;		/* true latitude beta of sun */
	double dhlong;

	sunpos (mjed, &lsn, &rsn, &bsn);/* sun's true coordinates; mean ecl. */

	op->s_sdist = 0.0;
	op->s_elong = 0.0;
	op->s_phase = 100.0;
	set_smag (op, -26.8);	/* TODO */
	// geo- to helio- centric 
	dhlong = lsn-PI;	
	range (&dhlong, 2*PI);
	op->s_hlong = (float)dhlong;
	op->s_hlat = (float)(-bsn);

	/* fill sun's ra/dec, alt/az in op */
	cir_pos (np, bsn, lsn, &rsn, op);
	op->s_edist = (float)rsn;
	op->s_size = (float)(raddeg(4.65242e-3/rsn)*3600*2);

	return (0);
}

// Puspose:	find moon's circumstances now.
////////////////////////////////////////////////////////////////////
static int moon_cir( Now *np, Obj *op )
{
	double lsn, rsn;	// true geoc lng of sun; dist from sn to earth
	double lam;    		// geocentric ecliptic longitude 
	double bet;    		// geocentric ecliptic latitude 
	double edistau;		// earth-moon dist, in au 
	double el;			// elongation, rads east 
	double ms;			// sun's mean anomaly 
	double md;			// moon's mean anomaly 
	double i;

	// mean ecliptic & EOD
	moon( mjed, &lam, &bet, &edistau, &ms, &md );
	// mean ecliptic & EOD
	sunpos( mjed, &lsn, &rsn, NULL );		

	// save geo in helio fields 
	op->s_hlong = (float)lam;		
	op->s_hlat = (float)bet;

	// find angular separation from sun 
	elongation (lam, bet, lsn, &el);
	// want degrees 
	op->s_elong = (float)raddeg(el);		

	// solve triangle of earth, sun, and elongation for moon-sun dist
	op->s_sdist = (float) sqrt (edistau*edistau + rsn*rsn - 2.0*edistau*rsn*cos(el));

	// TODO: improve mag; this is based on a flat moon model. - dist 
	i = -12.7 + 2.5*(log10(PI) - log10(PI/2*(1+1.e-6-cos(el)))) + 5*log10(edistau/.0025);
	set_smag (op, i);

	// find phase -- allow for projection effects 
	i = 0.1468*sin(el)*(1 - 0.0549*sin(md))/(1 - 0.0167*sin(ms));
	op->s_phase = (float)((1+cos(PI-el-degrad(i)))/2*100);

	// fill moon's ra/dec, alt/az in op and update for topo dist 
	cir_pos( np, bet, lam, &edistau, op );

	op->s_edist = (float)edistau;
	// moon angular dia, seconds 
	op->s_size = (float)(3600*2.0*raddeg(asin(MRAD/MAU/edistau)));
						
	return (0);
}

////////////////////////////////////////////////////////////////////
// Purpose:	fill in all of op->s_* stuff except s_size and s_mag.
//			this is used for sol system objects (except sun and moon);
//			never FIXEDOBJ.
////////////////////////////////////////////////////////////////////
static void cir_sky( Now *np,
					double lpd,		// heliocentric ecliptic longitude 
					double psi,		// heliocentric ecliptic lat 
					double rp,		// dist from sun 
					double *rho,	// dist from earth: in as geo, back as geo or topo 
					double lam,		// true geocentric ecliptic long 
					double bet,		// true geocentric ecliptic lat 
					double lsn,		// true geoc lng of sun 
					double rsn,		// dist from sn to earth
					Obj *op )
{
	// elongation 
	double el;		
	// fractional phase from earth 
	double f;		

	// compute elongation and phase 
	elongation (lam, bet, lsn, &el);
	el = raddeg(el);
	op->s_elong = (float)el;
	f = 0.25 * ((rp+ *rho)*(rp+ *rho) - rsn*rsn)/(rp* *rho);
	// percent 
	op->s_phase = (float)(f*100.0); 

	// set heliocentric long/lat; mean ecliptic and EOD 
	op->s_hlong = (float)lpd;
	op->s_hlat = (float)psi;

	// fill solar sys body's ra/dec, alt/az in op
	// updates rho 
	cir_pos( np, bet, lam, rho, op );        

	// set earth/planet and sun/planet distance 
	op->s_edist = (float)(*rho);
	op->s_sdist = (float)rp;
}

////////////////////////////////////////////////////////////////////
/* fill equatoreal and horizontal op-> fields; stern
 *
 *    input:          lam/bet/rho geocentric mean ecliptic and equinox of day
 * 
 * algorithm at EOD:
 *   ecl_eq	--> ra/dec	geocentric mean equatoreal EOD (via mean obliq)
 *   deflect	--> ra/dec	  relativistic deflection
 *   nut_eq	--> ra/dec	geocentric true equatoreal EOD
 *   ab_eq	--> ra/dec	geocentric apparent equatoreal EOD
 *					if (PREF_GEO)  --> output
 *   ta_par	--> ra/dec	topocentric apparent equatoreal EOD
 *					if (!PREF_GEO)  --> output
 *   hadec_aa	--> alt/az	topocentric horizontal
 *   refract	--> alt/az	observed --> output
 *
 * algorithm at fixed equinox:
 *   ecl_eq	--> ra/dec	geocentric mean equatoreal EOD (via mean obliq)
 *   deflect	--> ra/dec	  relativistic deflection [for alt/az only]
 *   nut_eq	--> ra/dec	geocentric true equatoreal EOD [for aa only]
 *   ab_eq	--> ra/dec	geocentric apparent equatoreal EOD [for aa only]
 *   ta_par	--> ra/dec	topocentric apparent equatoreal EOD
 *	 precess --> ra/dec	topocentric equatoreal fixed equinox [eq only]
 *					--> output
 *   hadec_aa	--> alt/az	topocentric horizontal
 *   refract	--> alt/az	observed --> output
 */
static void cir_pos( Now *np,
double bet,		// geo lat (mean ecliptic of date) 
double lam,		// geo long (mean ecliptic of date) 
double *rho,	// in: geocentric dist in AU; out: geo- or topocentic dist 
Obj *op )		// object to set s_ra/dec as per equinox 
{
	double ra, dec;			/* apparent ra/dec, corrected for nut/ab */
	double tra, tdec;		/* astrometric ra/dec, no nut/ab */
	double lsn, rsn;		/* solar geocentric (mean ecliptic of date) */
	double ha_in, ha_out;	/* local hour angle before/after parallax */
	double dec_out;			/* declination after parallax */
	double dra, ddec;		/* parallax correction */
	double alt, az;			/* current alt, az */
	double lst;				/* local sidereal time */
	double rho_topo;		/* topocentric distance in earth radii */

	// convert to equatoreal [mean equator, with mean obliquity]
	ecl_eq( mjed, bet, lam, &ra, &dec );
	// keep mean coordinates 
	tra = ra;
	tdec = dec;

	// get sun position 
	sunpos( mjed, &lsn, &rsn, NULL );

	// allow for relativistic light bending near the sun.
	// (avoid calling deflect() for the sun itself).
	if( !is_planet( op, SUN ) && !is_planet( op, MOON ) )
	    deflect( mjed, op->s_hlong, op->s_hlat, lsn, rsn, *rho, &ra, &dec );

	// correct ra/dec to form geocentric apparent
	nut_eq (mjed, &ra, &dec);
	if (!is_planet(op,MOON))
	    ab_eq (mjed, lsn, &ra, &dec);
	op->s_gaera = (float)ra;
	op->s_gaedec = (float)dec;

	// find parallax correction for equatoreal coords
	now_lst( np, &lst );
	ha_in = hrrad(lst) - ra;
	// convert to earth radii 
	rho_topo = *rho * MAU/ERAD;
	ta_par( ha_in, dec, lat, elev, &rho_topo, &ha_out, &dec_out );

	// transform into alt/az and apply refraction 
	hadec_aa( lat, ha_out, dec_out, &alt, &az );
	refract (pressure, temp, alt, &alt);
	op->s_alt = alt;
	op->s_az = az;

	// Get parallax differences and apply to apparent or astrometric place
	// as needed.  For the astrometric place, rotating the CORRECTIONS
	// back from the nutated equator to the mean equator will be
	// neglected.  This is an effect of about 0.1" at moon distance.
	// We currently don't have an inverse nutation rotation.
	if( pref_get(PREF_EQUATORIAL) == PREF_GEO )
	{
	    // no topo corrections to eq. coords
	    dra = ddec = 0.0;

	} else
	{
		// ra sign is opposite of ha 
	    dra = ha_in - ha_out;
	    ddec = dec_out - dec;
		// return topocentric distance in AU 
	    *rho = rho_topo * ERAD/MAU; 
	}

	// fill in ra/dec fields 
	if( epoch == EOD )
	{
		// apparent geo/topocentric 
	    ra = ra + dra;
	    dec = dec + ddec;

	} else 
	{
		// astrometric geo/topocent 
	    ra = tra + dra;
	    dec = tdec + ddec;
	    precess (mjed, epoch, &ra, &dec);
	}
	range( &ra, 2*PI );
	op->s_ra = (float)ra;
	op->s_dec = (float)dec;
}

/* given geocentric ecliptic longitude and latitude, lam and bet, of some object
 * and the longitude of the sun, lsn, find the elongation, el. this is the
 * actual angular separation of the object from the sun, not just the difference
 * in the longitude. the sign, however, IS set simply as a test on longitude
 * such that el will be >0 for an evening object <0 for a morning object.
 * to understand the test for el sign, draw a graph with lam going from 0-2*PI
 *   down the vertical axis, lsn going from 0-2*PI across the hor axis. then
 *   define the diagonal regions bounded by the lines lam=lsn+PI, lam=lsn and
 *   lam=lsn-PI. the "morning" regions are any values to the lower left of the
 *   first line and bounded within the second pair of lines.
 * all angles in radians.
 */
static void elongation( double lam, double bet, double lsn, double *el )
{
	*el = acos(cos(bet)*cos(lam-lsn));
	if (lam>lsn+PI || (lam>lsn-PI && lam<lsn)) *el = - *el;
}

/* apply relativistic light bending correction to ra/dec; stern
 *
 * The algorithm is from:
 * Mean and apparent place computations in the new IAU 
 * system. III - Apparent, topocentric, and astrometric 
 * places of planets and stars
 * KAPLAN, G. H.;  HUGHES, J. A.;  SEIDELMANN, P. K.;
 * SMITH, C. A.;  YALLOP, B. D.
 * Astronomical Journal (ISSN 0004-6256), vol. 97, April 1989, p. 1197-1210.
 *
 * This article is a very good collection of formulea for geocentric and
 * topocentric place calculation in general.  The apparent and
 * astrometric place calculation in this file currently does not follow
 * the strict algorithm from this paper and hence is not fully correct.
 * The entire calculation is currently based on the rotating EOD frame and
 * not the "inertial" J2000 frame.
 */
static void deflect(
double mjd1,				// equinox
double lpd, double psi,		// heliocentric ecliptical long / lat
double rsn, double lsn,		// distance and longitude of sun
double rho,					// geocentric distance
double *ra, double *dec )	// geocentric equatoreal
{
	double hra, hdec;	/* object heliocentric equatoreal */
	double el;			/* HELIOCENTRIC elongation object--earth */
	double g1, g2;		/* relativistic weights */
	double u[3];		/* object geocentric cartesian */
	double q[3];		/* object heliocentric cartesian unit vect */
	double e[3];		/* earth heliocentric cartesian unit vect */
	double qe, uq, eu;	/* scalar products */
	int i;				/* counter */

#define G	1.32712438e20	/* heliocentric grav const; in m^3*s^-2 */
#define c	299792458.0	/* speed of light in m/s */

	elongation(lpd, psi, lsn-PI, &el);
	el = fabs(el);
	/* only continue if object is within about 10 deg around the sun,
	 * not obscured by the sun's disc (radius 0.25 deg) and farther away
	 * than the sun.
	 *
	 * precise geocentric deflection is:  g1 * tan(el/2)
	 *	radially outwards from sun;  the vector munching below
	 *	just applys this component-wise
	 *	Note:	el = HELIOCENTRIC elongation.
	 *		g1 is always about 0.004 arc seconds
	 *		g2 varies from 0 (highest contribution) to 2
	 */
	if (el<degrad(170) || el>degrad(179.75) || rho<rsn) return;

	// get cartesian vectors 
	sphcart(*ra, *dec, rho, u, u+1, u+2);

	ecl_eq(mjd1, psi, lpd, &hra, &hdec);
	sphcart(hra, hdec, 1.0, q, q+1, q+2);

	ecl_eq(mjd1, 0.0, lsn-PI, &hra, &hdec);
	sphcart(hra, hdec, 1.0, e, e+1, e+2);

	// evaluate scalar products 
	qe = uq = eu = 0.0;
	for( i=0; i<=2; ++i )
	{
	    qe += q[i]*e[i];
	    uq += u[i]*q[i];
	    eu += e[i]*u[i];
	}

	g1 = 2*G/(c*c*MAU)/rsn;
	g2 = 1 + qe;

	// now deflect geocentric vector
	g1 /= g2;
	for(i=0; i<=2; ++i)
	    u[i] += g1*(uq*e[i] - eu*q[i]);
	
	// back to spherical - rho thrown away 
	cartsph( u[0], u[1], u[2], ra, dec, &rho );	
}

/* estimate size in arc seconds @ 1AU from absolute magnitude, H, and assuming
 * an albedo of 0.1. With this assumption an object with diameter of 1500m
 * has an absolute mag of 18.
 */
static double h_albsize( double H )
{
	return (3600.*raddeg(.707*1500.*pow(2.51,(18.-H)/2)/MAU));
}
