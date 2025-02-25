
#include <string.h>
#include <math.h>

#include "astro.h"

#include "misc.h"
#include "probfunc.h"

// Purpose:	convert the given apparent RA/Dec to astrometric precessed
//			to Mjd IN PLACE. we have no un-abberation etc so to find 
//			the correction: assume *rap and *decp are astrometric@EOD,
//			convert to apparent and back out the difference; then 
//			precess to Mjd.
////////////////////////////////////////////////////////////////////
void ap_as( Now *np, double Mjd, double *rap, double *decp )
{
	double r0 = *rap, d0 = *decp;
	Obj o;
	Now n;

	// as -> ap 
	zero_mem ((void *)&o, sizeof(o));
	o.o_type = FIXEDOBJ;
	o.f_RA = (float)*rap;
	o.f_dec = (float)*decp;
	o.f_epoch = (float)mjd;
	memcpy ((void *)&n, (void *)np, sizeof(Now));
	n.n_epoch = EOD;

	Sol_CalcObjOrbitPos( &n, &o );

	*rap -= o.s_ra - *rap;
	*decp -= o.s_dec - *decp;

	// then back to start for second order correction
	o.o_type = FIXEDOBJ;
	o.f_RA = (float)*rap;
	o.f_dec = (float)*decp;
	o.f_epoch = (float)mjd;
	memcpy ((void *)&n, (void *)np, sizeof(Now));
	n.n_epoch = EOD;

	Sol_CalcObjOrbitPos( &n, &o );
	
	*rap -= o.s_ra - r0;
	*decp -= o.s_dec - d0;

	radecrange( rap, decp );
	precess( mjd, Mjd, rap, decp );
	radecrange( rap, decp );
}

// Purpose:	convert the given astrometric RA/Dec which are precessed
//			to Mjd into apparent @ EOD IN PLACE.
////////////////////////////////////////////////////////////////////
void as_ap( Now *np, double Mjd, double *rap, double *decp )
{
	Obj o;
	Now n;

	zero_mem ((void *)&o, sizeof(o));
	o.o_type = FIXEDOBJ;
	o.f_RA = (float)*rap;
	o.f_dec = (float)*decp;
	o.f_epoch = (float)Mjd;
	memcpy ((void *)&n, (void *)np, sizeof(Now));
	n.n_epoch = EOD;

	Sol_CalcObjOrbitPos( &n, &o );

	*rap = o.s_ra;
	*decp = o.s_dec;
}
