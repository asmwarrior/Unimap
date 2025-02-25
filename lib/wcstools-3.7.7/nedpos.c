/*** nedpos.c - search object by its name from command line arguments
 *** April 16, 2008
 *** By Doug Mink, after IPAC byname.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libned/ned_client.h"
#include "libwcs/wcs.h"

static char *RevMsg = "NEDPOS 3.7.7, 03 April 2009, Doug Mink SAO";

extern int ned_errno;
static void PrintUsage();

int
main (ac, av)

int  ac;
char *av[];
{
   
    int    st;
    int    no_obj, lobj, lobj1;
    int    i, j, nid;
    double ra, dec, v;
    char *str, *objname, *obj1;
    char *objin, *objout;
    char rastr[32], decstr[32];
    char magnitude[32];
    char velocity[32];
    char zstring[32];
    char newobj[32];
    char *listfile;
    CrossID *cp, *tmpcp;
    ObjInfo *op, *tmpop;
    MoreData     *mdatap;
    FILE *flist;

    int nfobj = 0;
    int verbose = 0;
    int printid = 0;
    int printdeg = 0;
    int printvel = 0;
    int printname = 0;
    int printtype = 0;
    int printmag = 0;
    int tabout = 0;
    int nobj = 0;
    int outsys = WCS_J2000;
    int sysj = WCS_J2000;
    double c = 299792.58;

    listfile = NULL;
    st = ned_connect();
    if (st < 0) {
	fprintf(stderr, "connection failed \n");
	exit(1);
	}

    str = *(av+1);
    if (!str || !strcmp (str, "help") || !strcmp (str, "-help"))
	PrintUsage (NULL);
    if (!strcmp (str, "version") || !strcmp (str, "-version"))
	PrintUsage ("version");

    /* crack arguments */
    for (av++; --ac > 0 && (**av == '-' || **av == '@'); av++) {

	if (**av == '-') {
            char c;
	    str = *av;
            while (c = *++str)
            switch (c) {

        	case 'b':       /* Print coordinates in B1950 */
	            outsys = WCS_B1950;
		    break;

		case 'd':       /* Print coordinates in degrees */
		    printdeg++;
		    break;

		case 'e':       /* Print coordinates in ecliptic coordinates */
		    outsys = WCS_ECLIPTIC;
		    break;

		case 'g':       /* Print coordinates in galactic coordinates */
		    outsys = WCS_GALACTIC;
		    break;

		case 'i':       /* Print all IDs found by NED */
		    printid++;
		    break;

		case 'm':       /* Print object magnitude from NED */
		    printmag++;
		    break;

		case 'n':       /* Print object name from NED*/
		    printname++;
		    break;

		case 'o':       /* Print object type code from NED */
		    printtype++;
		    break;

		case 't':       /* Print output in tab-separated table */
		    tabout++;
		    break;

		case 'v':       /* more verbosity, including first ID */
		    verbose++;
		    break;

		case 'z':       /* Print object velocity from NED */
		    printvel++;
		    break;

		default:
		    PrintUsage(NULL);
		    break;
		}
	    }

	/* File containing a list of object names */
	else if (**av == '@') {
	    listfile = *av + 1;
	    if (isfile (listfile)) {
		nfobj = getfilelines (listfile);
		if (verbose)
		    fprintf (stderr,"NEDPOS: %d objects from file %s\n",
			     nfobj, listfile);
		if ((flist = fopen (listfile, "r")) == NULL) {
		    fprintf (stderr,"NEDPOS: List file %s cannot be read\n",
			     listfile);
		    }
		}
	    else {
		printf ("NEDPOS: List file %s does not exist\n", listfile);
		listfile = NULL;
		}
	    
	    }
	}

    /* There are ac remaining file names starting at av[0] */
    if (ac == 0 && !listfile)
        PrintUsage (NULL);

    if (listfile) {
	ac = nfobj;
	objname = newobj;
	}
    while (ac > 0) {

	if (listfile)
	    fgets (newobj, 32, flist);
	else
	    objname = *av++;
	ac--;

	/* Replace underscores with spaces */
	lobj = strlen (objname);
	for (i = 0; i < lobj; i++) {
	    if (objname[i] == '_')
		objname[i] = ' ';
	    }
	if (verbose)
	    printf ("%s -> ", objname);

	st = ned_obj_byname (objname, &no_obj, &op, &cp);

	/* for simple error message */
	if (st < 0) {
	    /* fprintf(stderr, "%s\n", ned_get_errmsg()); */

	    switch (ned_errno) {
		case NE_NAME:
		    if (verbose)
			printf ("not recognized by NED name interpreter\n");
		    else
			fprintf (stderr, 
			"*** Name %s can't be recognized by NED name interpreter\n",
			objname);
		    break;
		case NE_AMBN:
		    if (verbose)
			printf ("ambiguous name to NED\n");
		    else
			fprintf (stderr, "*** %d ambiguous name to NED: \n", no_obj);
		    for (i=0, tmpcp = cp; i<no_obj; i++, tmpcp++)
			fprintf (stderr, "*** %s \n", tmpcp->objname);
		    break;
		case NE_NOBJ:
		    if (verbose)
			printf ("not in NED database\n");
		    else
			fprintf (stderr, "*** Object %s is not in NED database\n",
				 cp->objname);
		    break;
		case NE_NOSPACE:
		    if (verbose)
			printf ("*** Memory alocation error\n");
		    else
			fprintf (stderr, "*** Memory allocation error happened \n");
		    break;
		case NE_QUERY:
		    if (verbose)
			printf ("*** Cannot send query to the NED server\n");
		    else
			fprintf (stderr, "*** Can't send query to the NED server\n");
		    break;
		case NE_BROKENC:
		    if (verbose)
			printf ("*** Connection to the NED server is broken\n");
		    else
			fprintf (stderr, "*** Connection to the NED server is broken\n");
		    break;
		}
	    } /* -1 return code */
	else {
	    if (no_obj > 1) {
		if (no_obj > 1)
		    printf ("%d objects found in NED: \n", no_obj);
		else
		    printf ("%d object found in NED: \n", no_obj);
		}

	/* Print Starbase header if requested */
	    nobj++;
	    if (tabout && nobj == 1) {
		printf ("catalog	NASA Extragalactic Database\n");
		if (outsys == WCS_GALACTIC)
		    printf ("radecsys	galactic\n");
		else if (outsys == WCS_ECLIPTIC)
		    printf ("radecsys	ecliptic\n");
		else if (outsys == WCS_B1950)
		    printf ("radecsys	B1950\n");
		else
		    printf ("radecsys	J2000\n");
		if (outsys == WCS_B1950) {
		    printf ("equinox	1950.0\n");
		    printf ("epoch	1950.0\n");
		    }
		else {
		    printf ("equinox	2000.0\n");
		    printf ("epoch	2000.0\n");
		    }
		printf ("program	%s\n",RevMsg);
		printf ("id    	ra      	dec       ");
		if (printmag)
		    printf ("	mag ");
		if (printvel)
		    printf ("	velocity");
		printf ("\n");
		printf ("--	------------	------------");
		if (printmag)
		    printf ("	----");
		if (printvel)
		    printf ("	--------");
		printf ("\n");
		}
	    for (i=0, tmpop = op; i<no_obj; i++, tmpop++) {
		if (printid && no_obj > 1) {
		    printf ("%d crossid for object No. %d: \n\n", 
			    tmpop->no_crossid, i+1);
		    }
		if (printid || verbose)
		    nid = tmpop->no_crossid;
		else
		    nid = 1;
		if (printid || printname || verbose) {
		    for (j=0, tmpcp = tmpop->cp; j < nid; j++, tmpcp++) {

			/* Remove trailing spaces from the object name */
			lobj = strlen (tmpcp->objname);
			while (tmpcp->objname[--lobj] == ' ')
			    tmpcp->objname[lobj] = (char) 0;

			/* Drop internal spaces from the current object name */
			lobj = strlen (tmpcp->objname);
			objname = (char *) calloc (lobj+2, 1);
			objin = tmpcp->objname;
			objout = objname;
			while (*objin != (char) 0) {
			    if (*objin != ' ')
				*objout++ = *objin;
			    objin++;
			    }
			*objout = (char) 0;

			/* Print the current object name */
			printf ("%s", objname);
			if (printid && j < nid-1)
			    printf (",");
			}
		    }
		else {
		    printf ("%s", objname);
		    }
		if (tabout)
		    printf ("	");
		else
		    printf (" ");
		ra = tmpop->ra;
		dec = tmpop->dec;
		if (outsys != WCS_J2000)
		    wcscon (sysj, outsys, 0.0, 0.0, &ra, &dec, 0.0);
		if (outsys == WCS_ECLIPTIC || outsys == WCS_GALACTIC) {
		    if (verbose)
			printf ("l= ");
		    printf ("%.6f", ra);
		    if (tabout)
			printf ("	");
		    else
			printf (" ");
		    if (verbose)
			printf ("b= ");
		    if (dec >= 0.0)
			printf ("+");
		    printf ("%.6f", dec);
		    if (!tabout) {
			if (outsys == WCS_GALACTIC)
			    printf (" Galactic");
			else if (outsys == WCS_ECLIPTIC)
			    printf (" Ecliptic");
			}
		    }
		else {
		    if (verbose)
			printf ("ra= ");
		    if (printdeg)
			printf ("%.6f", ra);
		    else {
			ra2str (rastr, 31, ra, 3);
			printf ("%s", rastr);
			}
		    if (tabout)
			printf ("	");
		    else
			printf (" ");
		    if (verbose)
			printf ("dec=");
		    if (printdeg)
			printf ("%.6f", dec);
		    else {
			dec2str (decstr, 31, dec, 2);
			printf ("%s", decstr);
			}
		    if (!tabout) {
			if (outsys == WCS_B1950)
			    printf (" B1950");
			else
			    printf (" J2000");
			}
		    }
		if (printtype || verbose)
		    printf (" %s", tmpop->objtype);

	    /* extract interesting information for this object */
		if (verbose)
		    printf ("\n");
		mdatap = tmpop->mdp;
		strcpy (velocity, "none");
		strcpy (magnitude, "none");
		while (mdatap) {
		    if (verbose)
			printf ("%-10s: %s\n", mdatap->data_typec,mdatap->data);
		    if (!strcmp (mdatap->data_typec, "HRV"))
			strcpy (velocity, mdatap->data);
		    if (!strcmp (mdatap->data_typec, "MAG"))
			strcpy (magnitude, mdatap->data);
		    if (!strcmp (mdatap->data_typec, "Z")) {
			strcpy (zstring, mdatap->data);
			v = c * atof (zstring);
			sprintf (velocity, "%.5f", v);
			}
		    mdatap = mdatap->next;
		    }
		if (printmag) {
		    if (tabout)
			printf ("	");
		    else
			printf (" ");
		    printf ("%s", magnitude);
		    }
		if (printvel) {
		    if (tabout)
			printf ("	");
		    else
			printf (" ");
		    printf ("%s", velocity);
		    }
		printf ("\n");
		}
	    }

	if (cp)
	    ned_free_cp (cp);
	if (op)
	    ned_free_objp (op, no_obj);
	}
    ned_disconnect();
    if (listfile)
	fclose (flist);
}

static void
PrintUsage (command)

char	*command;	/* Command where error occurred or NULL */

{
    /* Print program name and version */
    fprintf (stderr,"%s\n", RevMsg);
    if (command != NULL && !strncasecmp (command, "ver", 3))
        exit (0);

    if (command != NULL) {
	if (command[0] == '*')
	    fprintf (stderr, "%s\n", command);
	else
	    fprintf (stderr,"* Missing argument for command: %c\n", command[0]);
	exit (1);
	}

    fprintf (stderr,"nedpos: Return RA and Dec for object name using NED, the NASA/IPAC\n");
    fprintf (stderr,"        Extragalactic Database from JPL at Caltech\n");
    fprintf (stderr,"Usage:  nedpos [-dimoptvz][b|e|g] name1 name2 ...\n");
    fprintf (stderr,"        nedpos [-dimoptvz][b|e|g] @namelist ...\n");
    fprintf (stderr,"name(n): Objects for which to search (space -> _)\n");
    fprintf (stderr,"namelist: File with one object name per line\n");
    fprintf (stderr,"-b: Print coordinates in B1950 instead of J2000\n");
    fprintf (stderr,"-d: Print coordinates in degrees instead of sexigesimal\n");
    fprintf (stderr,"-e: Print coordinates in ecliptic instead of J2000\n");
    fprintf (stderr,"-g: Print coordinates in galactic instead of J2000\n");
    fprintf (stderr,"-i: Print IDs returned from NED\n");
    fprintf (stderr,"-m: Print magnitude for each object returned from NED\n");
    fprintf (stderr,"-n: Print first name for each object returned from NED\n");
    fprintf (stderr,"-o: Print object type for each object returned from NED\n");
    fprintf (stderr,"-p: Print position for each object returned from NED\n");
    fprintf (stderr,"-t: Print output as tab-separated table\n");
    fprintf (stderr,"-v: Print extra descriptive info\n");
    fprintf (stderr,"-z: Print velocity for each object returned from NED\n");
    exit (1);
}

/* Apr 22 2002	New program based on IPAC byname.c (Xiuqin Wu, 1996)
 * Jun  6 2002	Add conversion to B1950, ecliptic, and galactic coordinates
 * Jun 11 2002	Add reference to NED
 * Sep 26 2002	Allow _ to replace spaces in object names for scripting
 * Oct 25 2002	Fix object number printout
 *
 * Jul 10 2003	Fix error message display
 *
 * Jul 29 2004	Add options to print additional information from NED
 * Aug  2 2004	Fix bug which truncated object names on output
 * Aug 23 2004	Read velocity from Z as well as HRV
 * Sep 19 2007	Add -t option for Starbase output and @ for lists of names
 *
 * Apr 16 2008	Fix @ option for processing a list of objects
 */
