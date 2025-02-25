////////////////////////////////////////////////////////////////////
// Author: E.BERTIN (IAP)
// Updated: Larry Lart
// Purpose: functions for extraction of connected pixels from a pixmap
//
// Last modify:	29/11/2005, 05/032010
////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include        "config.h"
#endif

// system 
#include	<math.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include <omp.h>

// include local headers
//#include "../logger/logger.h"
#include "../unimap_worker.h"

// include main header
#define NOCTYPES
#include "sextractor.h"
#undef NOCTYPES

////////////////////////////////////////////////////////////////////
// Method:		ScanImage
// Class:		CSextractor
// Purpose:		Scan of the large pixmap(s). Main loop and heart of 
//				the program.
// Input:		Measurement field pointer, Detection field pointer,
//				Flag field pointer, Measurement weight-map field pointer,
//				Detection weight-map field pointer,
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::ScanImage( picstruct *field, picstruct *dfield, 
							picstruct **pffield, int nffield, 
							picstruct *wfield, picstruct *dwfield )
{
	static infostruct	curpixinfo, *info, *store, initinfo, freeinfo, *victim;
	picstruct		*ffield;
	checkstruct		*check;
	objliststruct	objlist;
	objstruct		*cleanobj;
	pliststruct		*pixel, *pixt;
	picstruct		*cfield, *cdwfield;

	char			*marker, newmarker, *blankpad, *bpt,*bpt0;
	int				co, i,j, flag, luflag,pstop, xl,xl2,yl, cn, nposize, 
					stacksize, w, h, blankh, maxpixnb, varthreshflag;
	short	       	trunflag;
	PIXTYPE			thresh, relthresh, cdnewsymbol, cdvar, *scan,*dscan,*cdscan,
					*dwscan,*cdwscan,*cdwscanp, *scant, *wscan,*wscanp;
	FLAGTYPE		*pfscan[MAXFLAG];
	status			cs, ps, *psstack;
	int				*start, *end, ymax;

	wxString strLog;

	// reset image data stream pointer to zero
	m_nGlobImgPointer = 0;

	// init to null
	scan = dscan = cdscan = cdwscan = cdwscanp = wscan = wscanp = NULL;
	victim = NULL;			
	blankh = 0;				

	// Beginning of the main loop: Initialisations 
	thecat.ntotal = thecat.ndetect = 0;

	// cfield is the detection field in any case
	cfield = dfield? dfield:field;

	// cdwfield is the detection weight-field if available 
	cdwfield = dwfield? dwfield:(prefs.dweight_flag?wfield:NULL);

	// If WEIGHTing and no absolute thresholding, activate threshold scaling 
	varthreshflag = (cdwfield && prefs.thresh_type[0]!=THRESH_ABSOLUTE);
	relthresh = varthreshflag ? (float) prefs.dthresh[0] : (float) 0.0;
	w = cfield->width;
	h = cfield->height;
	objlist.dthresh = cfield->dthresh;
	objlist.thresh = cfield->thresh;
	cfield->yblank = 1;
	field->y = field->stripy = 0;
	field->ymin = field->stripylim = 0;
	field->stripysclim = 0;
	// init dfield
	if( dfield )
	{
		dfield->y = dfield->stripy = 0;
		dfield->ymin = dfield->stripylim = 0;
		dfield->stripysclim = 0;
	}
	// init nffield
	if (nffield)
	{
		for (i=0; i<nffield; i++)
		{
			ffield = pffield[i];
			ffield->y = ffield->stripy = 0;
			ffield->ymin = ffield->stripylim = 0;
			ffield->stripysclim = 0;
		}
	}
	// init wfield
	if (wfield)
	{
		wfield->y = wfield->stripy = 0;
		wfield->ymin = wfield->stripylim = 0;
		wfield->stripysclim = 0;
	}
	// init dwfield
	if (dwfield)
	{
		dwfield->y = dwfield->stripy = 0;
		dwfield->ymin = dwfield->stripylim = 0;
		dwfield->stripysclim = 0;
	}

	// Allocate memory for buffers
	stacksize = w+1;
	QMALLOC(info, infostruct, stacksize);
	QMALLOC(store, infostruct, stacksize);
	QMALLOC(marker, char, stacksize);
	QMALLOC(dumscan, PIXTYPE, stacksize);
	QMALLOC(psstack, status, stacksize);
	QMALLOC(start, int, stacksize);
	QMALLOC(end, int, stacksize);
	blankpad = bpt = NULL;
	LutzAlloc(w,h);
	AllocParcelout( );

	// check if there is a filter flag ???
	if( prefs.filter_flag )
	{
		QMALLOC(cdscan, PIXTYPE, stacksize);
		if (cdwfield)
		{
			QCALLOC(cdwscan, PIXTYPE, stacksize);
			if (PLISTEXIST(wflag))
			{
				QMALLOC(cdwscanp, PIXTYPE, stacksize);
			}
		}

		// One needs a buffer to protect filtering if source-blanking applies
		if (prefs.blank_flag)
		{
			blankh = thefilter->convh/2+1;
			QMALLOC(blankpad, char, w*blankh);
			cfield->yblank -= blankh;
			if (dfield) field->yblank = cfield->yblank;
			bpt = blankpad;
		}
	}

	// Some initializations 
	thresh = objlist.dthresh;
	initinfo.pixnb = 0;
	initinfo.flag = 0;
	initinfo.firstpix = initinfo.lastpix = -1;

	for (xl=0; xl<stacksize; xl++)
	{
		marker[xl]  = 0 ;
		dumscan[xl] = (float) -BIG ;
	}

	co = pstop = 0;
	objlist.nobj = 1;
	curpixinfo.pixnb = 1;

	// Init cleaning procedure
	InitClean( );

	// Allocate memory for the pixel list */
	InitPlist();

	if( !(pixel = objlist.plist = (char*) malloc(nposize=prefs.mem_pixstack*plistsize)) )
	{
		strLog = wxT("Not enough memory to store the pixel stack: Try to decrease MEMORY_PIXSTACK" );
		m_pUnimapWorker->Log( strLog );
	}

	// at the beginning, "free" object fills the whole pixel list 
	freeinfo.firstpix = 0;
	freeinfo.lastpix = nposize-plistsize;
	pixt = pixel;
	for (i=plistsize; i<nposize; i += plistsize, pixt += plistsize)
	{
		PLIST(pixt, nextpix) = i;
	}

	PLIST(pixt, nextpix) = -1;

	/////////////////////////////////////////////////////////////
	// START LOOP on Y  - Here we go - MAIN SCAN - here to parallelize
//	omp_set_num_threads(4);

//	#pragma omp parallel for ordered schedule(dynamic)
	for (yl=0; yl<=h;)
	{
		ps = COMPLETE;
		cs = NONOBJECT;
		if( yl==h )
		{
			// Need an empty line for Lutz' algorithm to end gracely 
			if( prefs.filter_flag )
			{
				free(cdscan);
				if (cdwfield)
				{
					free(cdwscan);
					if (PLISTEXIST(wflag)) free(cdwscanp);
				}
			}
			cdwscan = cdwscanp = cdscan = dumscan;

		} else
		{
			if (nffield)
			{
				for (i=0; i<nffield; i++)
				{
					ffield = pffield[i];
					pfscan[i] = (ffield->stripy==ffield->stripysclim)?
								(FLAGTYPE *)LoadImgStrip(ffield, (picstruct *)NULL  )
								: &ffield->fstrip[ffield->stripy*ffield->width];
				}
			}

			if (wfield)
			{
				// Copy the previous weight line to track bad pixel limits 
				wscan = (wfield->stripy==wfield->stripysclim)?
						(PIXTYPE *) LoadStrip( wfield, (picstruct *)NULL )
						: &wfield->strip[wfield->stripy*wfield->width];
				if (yl && PLISTEXIST(wflag))
					wscanp = &wfield->strip[((yl-1)%wfield->stripheight)*wfield->width];

			} else
				wscan = NULL;

			//////////////////////////////////////////////////////
			// larry :: here we read a strip from the image
			scan = (field->stripy==field->stripysclim)?
					(PIXTYPE *) LoadImgStrip(field, wfield )
					: &field->strip[field->stripy*field->width];

			if (dwfield)
			{
				dwscan = (dwfield->stripy==dwfield->stripysclim)?
						(PIXTYPE *) LoadStrip(dwfield,
						dfield?(picstruct *)NULL:dwfield)
						: &dwfield->strip[dwfield->stripy*dwfield->width];
			} else
				dwscan = wscan;

			if (dfield)
			{
				dscan = (dfield->stripy==dfield->stripysclim)?
						(PIXTYPE *) LoadStrip(dfield, dwfield)
						: &dfield->strip[dfield->stripy*dfield->width];
			} else
				dscan = scan;

			if (PLISTEXIST(wflag) && cdwfield)
			{
				// Copy the previous filtered weight line to track bad pixel limits 
				memcpy(cdwscanp, cdwscan, wfield->width*sizeof(PIXTYPE));
			}

			if (prefs.filter_flag)
			{
				Filter(cfield, cdscan);
				if (cdwfield) Filter(cdwfield, cdwscan);
			} else
			{
				cdscan = dscan;
				cdwscan = dwscan;
			}

			if( (check=prefs.check[CHECK_FILTERED]) ) WriteCheck(check, cdscan, w);
		}

		trunflag = (yl==0 || yl==h-1)? OBJ_TRUNC:0;

		/////////////////////////////////////////////////////////
		// START LOOP OVER X
//		omp_set_num_threads(4);

//		#pragma omp parallel for ordered schedule(dynamic)
		for (xl=0; xl<=w; xl++)
		{
			if (xl == w)
				cdnewsymbol = (float) -BIG;
			else
				cdnewsymbol = cdscan[xl];

			newmarker = marker[xl];
			marker[xl] = 0;
			curpixinfo.flag = trunflag;

			if (varthreshflag) 
				thresh =  relthresh*sqrt( cdvar =  ((xl==w || yl==h)? 0.0 : cdwscan[xl]) );

			luflag = cdnewsymbol > thresh?1:0;

			if (luflag)
			{
				if (xl==0 || xl==w-1) curpixinfo.flag |= OBJ_TRUNC;

				pixt = pixel + (cn=freeinfo.firstpix);
				freeinfo.firstpix = PLIST(pixt, nextpix);

				// Running out of pixels, the largest object becomes a "victim"
				if( freeinfo.firstpix==freeinfo.lastpix )
				{
					sprintf( gstr, "%d,%d", xl+1, yl+1 );
					strLog.Printf( wxT("WARNING :: Pixel stack overflow at position=%d,%d"), xl+1, yl+1 );
					m_pUnimapWorker->Log( strLog, GUI_LOGGER_ATTENTION_STYLE );

					maxpixnb = 0;
					// scan line on width ???????
					for(i=0; i<=w; i++)
					{
						if(store[i].pixnb>maxpixnb)
						{
							if(marker[i]=='S' || (newmarker=='S' && i==xl))
							{
								flag = 0;
								if( i<xl )
								{
									for(j=0; j<=co; j++)
										flag |= (start[j]==i);
								}

								if(!flag) maxpixnb = (victim = &store[i])->pixnb;
							}
						}
					}

					for(j=1; j<=co; j++)
						if (info[j].pixnb>maxpixnb) maxpixnb = (victim = &info[j])->pixnb;

					if( !maxpixnb ) error(EXIT_FAILURE, "*Fatal Error*: something is badly bugged in ", "scanimage()!");
					if( maxpixnb <= 1 ) error(EXIT_FAILURE, "Pixel stack overflow in ", "scanimage()");

					freeinfo.firstpix = PLIST(pixel+victim->firstpix, nextpix);
					PLIST(pixel+victim->lastpix, nextpix) = freeinfo.lastpix;
					PLIST(pixel+(victim->lastpix=victim->firstpix), nextpix) = -1;
					victim->pixnb = 1;
					victim->flag |= OBJ_OVERFLOW;
				}

				//////////////////////////////////////////////////??? section ???
				curpixinfo.lastpix = curpixinfo.firstpix = cn;
				PLIST(pixt, nextpix) = -1;
				PLIST(pixt, x) = xl;
				PLIST(pixt, y) = yl;
				PLIST(pixt, value) = scan[xl];

				if (PLISTEXIST(dvalue)) PLISTPIX(pixt, dvalue) = dscan[xl];
				if (PLISTEXIST(cdvalue)) PLISTPIX(pixt, cdvalue) = cdnewsymbol;
				if (PLISTEXIST(flag))
				{
					for (i=0; i<nffield; i++)
					{
						PLISTFLAG(pixt, flag[i]) = pfscan[i][xl];
					}
				}

				if (PLISTEXIST(wflag) && wscan)
				{
					PLISTFLAG(pixt, wflag) = 0;
					if (xl>0)
					{
						if (wscan[xl-1] >= BIG) PLISTFLAG(pixt, wflag) |= OBJ_WEIGHTZERO;
						if (cdwscan[xl-1] >= BIG) PLISTFLAG(pixt, wflag) = OBJ_DWEIGHTZERO;
					}

					PLISTFLAG(pixt, wflag) = 0;
					if( xl<w-1 )
					{
						if (wscan[xl+1] >= BIG) PLISTFLAG(pixt, wflag) |= OBJ_WEIGHTZERO;
						if (cdwscan[xl+1] >= BIG) PLISTFLAG(pixt, wflag) = OBJ_DWEIGHTZERO;
					}

					if( yl>0 )
					{
						if( wscanp[xl] >= BIG ) PLISTFLAG(pixt, wflag) |= OBJ_WEIGHTZERO;
						if(cdwscanp[xl] >= BIG ) PLISTFLAG(pixt, wflag) = OBJ_DWEIGHTZERO;
					}
				}

				if (PLISTEXIST(dthresh)) PLISTPIX(pixt, dthresh) = thresh;
				if (PLISTEXIST(var)) PLISTPIX(pixt, var) = wscan[xl];

				///////////////////////////////////////////////////////
				// Start Segment 
				if (cs != OBJECT)
				{
					cs = OBJECT;
					if (ps == OBJECT)
					{
						if (start[co] == UNKNOWN)
						{
							marker[xl] = 'S';
							start[co] = xl;
						} else
							marker[xl] = 's';
					} else
					{
						psstack[pstop++] = ps;
						marker[xl] = 'S';
						start[++co] = xl;
						ps = COMPLETE;
						info[co] = initinfo;
					}
				}

				//////////////////////////////////////////////////////////
			}

			/////////////////////////////////////////////////////////////
			// Process New Marker
			if( newmarker )
			{
				if (newmarker == 'S')
				{
					psstack[pstop++] = ps;
					if (cs == NONOBJECT)
					{
						psstack[pstop++] = COMPLETE;
						info[++co] = store[xl];
						start[co] = UNKNOWN;
					} else
						Update( &info[co],&store[xl], pixel );

					ps = OBJECT;

				} else if (newmarker == 's')
				{
					if ((cs == OBJECT) && (ps == COMPLETE))
					{
						pstop--;
						xl2 = start[co];
						Update( &info[co-1],&info[co], pixel );

						if (start[--co] == UNKNOWN)
							start[co] = xl2;
						else
							marker[xl2] = 's';
					}

					ps = OBJECT;

				} else if (newmarker == 'f')
				{
					ps = INCOMPLETE;

				} else if (newmarker == 'F')
				{
					ps = psstack[--pstop];
					if ((cs == NONOBJECT) && (ps == COMPLETE))
					{
						if (start[co] == UNKNOWN)
						{
							if ((int)info[co].pixnb >= prefs.ext_minarea)
							{
								Sortit(field, dfield, wfield, cdwfield, &info[co], &objlist,
								cdwscan, wscan);
							}

							// free the chain-list 
							PLIST(pixel+info[co].lastpix, nextpix) = freeinfo.firstpix;
							freeinfo.firstpix = info[co].firstpix;

						} else
						{
							marker[end[co]] = 'F';
							store[start[co]] = info[co];
						}

						co--;
						ps = psstack[--pstop];
					}
				}
			}
			// End of Process new maker 
			//////////////////////////////////////////////////////////////////

			if( luflag )
			{
				Update(&info[co],&curpixinfo, pixel);
			} else
			{
				// End Segment - process that end ???????
				if (cs == OBJECT)
				{
					cs = NONOBJECT;
					if (ps != COMPLETE)
					{
						marker[xl] = 'f';
						end[co] = xl;
					} else
					{
						ps = psstack[--pstop];
						marker[xl] = 'F';
						store[start[co]] = info[co];
						co--;
					}
				}
			}

			if( prefs.blank_flag && xl<w )
			{
				if( prefs.filter_flag )
					*(bpt++) = (luflag)?1:0;
				else if( luflag )
					dscan[xl] = (float) -BIG;

				if( dfield && luflag )
					scan[xl] = (float) -BIG;
			}
		}
		// End of the loop over the x's 
		//////////////////////////////////////////////////

		// Detected pixel removal at the end of each line 
		if (prefs.blank_flag && yl<h)
		{
			if (prefs.filter_flag)
			{
				bpt = bpt0 = blankpad + w*((yl+1)%blankh);
				if (cfield->yblank >= 0)
				{
					scant = &PIX(cfield, 0, cfield->yblank);

					for( i=w; i--; scant++ )
						if( *(bpt++) ) *scant = (float) -BIG;

					if( dfield )
					{
						bpt = bpt0;
						scant = &PIX(field, 0, cfield->yblank);

						for( i=w; i--; scant++ )
							if (*(bpt++)) *scant = (float) -BIG;
					}
					bpt = bpt0;
				}
			}

			cfield->yblank++;

			if (dfield) field->yblank = cfield->yblank;
		}

		// Prepare markers for the next line 
		yl++;
		field->stripy = (field->y=yl)%field->stripheight;

		if( dfield ) dfield->stripy = (dfield->y=yl)%dfield->stripheight;
		if( nffield )
		{
			for( i=0; i<nffield; i++ )
			{
				ffield = pffield[i];
				ffield->stripy = (ffield->y=yl)%ffield->stripheight;
			}
		}
		if( wfield ) wfield->stripy = (wfield->y=yl) % wfield->stripheight;
		if( dwfield ) dwfield->stripy = (dwfield->y=yl) % dwfield->stripheight;

		// Remove objects close to the ymin limit if ymin is ready to increase 
		if( cfield->stripy==cfield->stripysclim )
		{
			cleanobj = cleanobjlist->obj + cleanobjlist->nobj - 1;
			for( i=cleanobjlist->nobj; i--; cleanobj-- )
			{
				if( cleanobj->ycmin <= cfield->ymin )
				{
					// Warn if there is a possibility for any aperture to be truncated 
					if( (ymax=cleanobj->ycmax) > cfield->ymax )
					{
						sprintf(gstr, "Object at position %.0f,%.0f ",
								cleanobj->mx+1, cleanobj->my+1);
//						QWARNING(gstr, "may have some apertures truncated:\n"
//									"You might want to increase MEMORY_BUFSIZE");

					} else if( ymax>cfield->yblank && prefs.blank_flag )
					{
						sprintf(gstr, "Object at position %.0f,%.0f ",
								cleanobj->mx+1, cleanobj->my+1);
//						QWARNING(gstr, "may have some unBLANKed neighbours:\n"
//								"You might want to increase MEMORY_PIXSTACK");
					}

//					m_pImgView->AddStar( (int)(cleanobj->mx+0.49999), (int)(cleanobj->my+0.49999) );
					//AnalyseEndObject( field, dfield, wfield, cdwfield, i, cleanobjlist );
					SubCleanObj( i );
					// realloc in subcleanobj() 
					cleanobj = cleanobjlist->obj + i;	
				}
			}
		}

		if( !((yl+1)%16) )
		{
			NPRINTF( OUTPUT, "\33[1M> Line:%5d  "
					"Objects: %8d detected / %8d sextracted\n\33[1A",
					yl+1, thecat.ndetect, thecat.ntotal );
		}
	}
	// End of the loop over the y's 
	////////////////////////////////////////////////

	// Removal or the remaining pixels
	if (prefs.blank_flag && prefs.filter_flag && (cfield->yblank >= 0))
	{
		for (j=blankh-1; j--; yl++)
		{
			bpt = bpt0 = blankpad + w*(yl%blankh);
			scant = &PIX(cfield, 0, cfield->yblank);
			for (i=w; i--; scant++)
				if (*(bpt++)) *scant = (float) -BIG;

			if (dfield)
			{
				bpt = bpt0;
				scant = &PIX(field, 0, cfield->yblank);
				for (i=w; i--; scant++)
				  if (*(bpt++)) *scant = (float) -BIG;
			}
			cfield->yblank++;
			if (dfield)
			field->yblank = cfield->yblank;
		}
	}

	// Now that all "detected" pixels have been removed, analyse detections 
	objstruct	*obj;
	for (j=cleanobjlist->nobj; j--;)
    {
		obj = &cleanobjlist->obj[0];
//		if( obj->cxx > 2 && obj->cyy > 2 )
//			m_pImgView->AddStar( (int)(obj->mx+0.49999), (int)(obj->my+0.49999) );
		AnalyseEndObject( field, dfield, wfield, cdwfield, 0, cleanobjlist );
		SubCleanObj(0);
    }

	EndClean();

	// Free memory
	FreeParcelout();
	free(pixel);
	LutzFree();
	free(info);
	free(store);
	free(marker);
	free(dumscan);
	free(psstack);
	free(start);
	free(end);
	if (prefs.blank_flag && prefs.filter_flag) free(blankpad);

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		Update
// Class:		CSextractor
// Purpose:		update object's properties each time one of its pixels 
//				is scanned by lutz()
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
inline void CSextractor::Update( infostruct *infoptr1, infostruct *infoptr2, 
													pliststruct *pixel )
{
	infoptr1->pixnb += infoptr2->pixnb;
	infoptr1->flag |= infoptr2->flag;

	if (infoptr1->firstpix == -1)
	{
		infoptr1->firstpix = infoptr2->firstpix;
		infoptr1->lastpix = infoptr2->lastpix;

	} else if (infoptr2->lastpix != -1)
	{
		PLIST(pixel+infoptr1->lastpix, nextpix) = infoptr2->firstpix;
		infoptr1->lastpix = infoptr2->lastpix;
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		Sortit
// Class:		CSextractor
// Purpose:		build the object structure.
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
inline void CSextractor::Sortit( picstruct *field, picstruct *dfield, picstruct *wfield,
							picstruct *dwfield, infostruct *info, objliststruct *objlist,
							PIXTYPE *cdwscan, PIXTYPE *wscan )
{
	picstruct		*cfield;
	objliststruct	objlistout, *objlist2;
	static objstruct	obj;
	objstruct		*cobj;
	pliststruct		*pixel;
	int i=0, j=0, n=0;

	cfield = dfield? dfield: field;

	pixel = objlist->plist;
	objlistout.obj = NULL;
	objlistout.plist = NULL;
	objlistout.nobj = objlistout.npix = 0;

	// Allocate memory to store object data

	objlist->obj = &obj;
	objlist->nobj = 1;

	memset(&obj, 0, (size_t)sizeof(objstruct));
	objlist->npix = info->pixnb;
	obj.firstpix = info->firstpix;
	obj.lastpix = info->lastpix;
	obj.flag = info->flag;
	obj.dthresh = objlist->dthresh;
	obj.thresh = objlist->thresh;

	PreAnalyse(0, objlist, ANALYSE_FAST);

	// Check if the current strip contains the lower isophote... 
	if( (int) obj.ymin < cfield->ymin ) obj.flag |= OBJ_ISO_PB;

	if( !(obj.flag & OBJ_OVERFLOW) && (CreateSubMap(objlist, 0) == RETURN_OK) )
	{
		if (Parcelout(objlist, &objlistout) == RETURN_OK)
			objlist2 = &objlistout;
		else
		{
			objlist2 = objlist;
			for( i=0; i<objlist2->nobj; i++ )
				objlist2->obj[i].flag |= OBJ_DOVERFLOW;

			sprintf( gstr, "%.0f,%.0f", obj.mx+1, obj.my+1 );
			warning( "Deblending overflow for detection at ", gstr );
		}

		free(obj.submap);

	} else
		objlist2 = objlist;

	for( i=0; i<objlist2->nobj; i++ )
	{
		PreAnalyse( i, objlist2, ANALYSE_FULL|ANALYSE_ROBUST );
		Analyse( field, dfield, i, objlist2 );
		cobj = objlist2->obj + i;
		if( prefs.blank_flag )
		{
			if( CreateBlank(objlist2,i) != RETURN_OK )
			{
				// Not enough mem. for the BLANK vignet: flag the object now 
				cobj->flag |= OBJ_OVERFLOW;
				cobj->blank = cobj->dblank = NULL;
				sprintf(gstr, "%.0f,%.0f", cobj->mx+1, cobj->my+1);
				warning("Memory overflow during masking for detection at ", gstr);
			}
		}

		if( (n=cleanobjlist->nobj) >= prefs.clean_stacksize )
		{
			objstruct	*cleanobj;
			int		ymin, ymax, victim=0;

			// No image is expected to be that tall ! 
			ymin = 2000000000;	
			cleanobj = cleanobjlist->obj;
			for (j=0; j<n; j++, cleanobj++)
			{
				if (cleanobj->ycmax < ymin)
				{
					victim = j;
					ymin = cleanobj->ycmax;
				}
			}

			// Warn if there is a possibility for any aperture to be truncated
			if (field->ymax < field->height)
			{
				cleanobj = &cleanobjlist->obj[victim];
				if( (ymax=cleanobj->ycmax) > field->ymax )
				{
					sprintf(gstr, "Object at position %.0f,%.0f ", cleanobj->mx+1, cleanobj->my+1);
					QWARNING( gstr, "may have some apertures truncated:\n"
					"          You might want to increase MEMORY_OBJSTACK" );

				} else if( ymax>field->yblank && prefs.blank_flag )
				{
					sprintf(gstr, "Object at position %.0f,%.0f ", cleanobj->mx+1, cleanobj->my+1);
					QWARNING( gstr, "may have some unBLANKed neighbours\n"
					"          You might want to increase MEMORY_OBJSTACK" );
				}
			}

			AnalyseEndObject(field, dfield, wfield, dwfield, victim, cleanobjlist);
			SubCleanObj(victim);
		}

		// Only add the object if it is not swallowed by cleaning 
		if (!prefs.clean_flag || Clean(field, dfield, i, objlist2))
		AddCleanObj(cobj);
	}

	free(objlistout.plist);
	free(objlistout.obj);

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		PreAnalyse
// Class:		CSextractor
// Purpose:		Compute basic image parameters from the pixel-list 
//				for each detection.
// Input:		objlist number,objlist pointer,analysis switch flag.
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CSextractor::PreAnalyse( int no, objliststruct *objlist, int analyse_type )
{
	objstruct	*obj = &objlist->obj[no];
	pliststruct	*pixel = objlist->plist, *pixt;
	PIXTYPE	peak, cpeak, val, cval, minthresh, thresht;
	double	thresh=0., thresh2=0., t1t2=0., darea=0., mx=0., my=0., mx2=0., my2=0., 
			mxy=0., rv=0., tv=0., xm=0., ym=0., xm2=0., ym2=0., xym=0., temp=0., 
			temp2=0., theta=0., pmx2=0., pmy2=0.;
	int		x=0, y=0, xmin=0, xmax=0, ymin=0, ymax=0, area2=0, fdnpix=0, dnpix=0;

	// initialize stacks and bounds
	thresh = obj->dthresh;
	if( PLISTEXIST(dthresh) )
		minthresh = BIG;
	else
		minthresh = 0.0;
	fdnpix = dnpix = 0;
	rv = 0.0;
	peak = cpeak = -BIG;
	// to be really sure!! 
	ymin = xmin = 2*MAXPICSIZE;    
	ymax = xmax = 0;

	// integrate results
	for (pixt=pixel+obj->firstpix; pixt>=pixel; pixt=pixel+PLIST(pixt,nextpix))
	{
		x = PLIST(pixt, x);
		y = PLIST(pixt, y);
		val = PLISTPIX(pixt, dvalue);
		if( cpeak < (cval=PLISTPIX(pixt, cdvalue)) ) cpeak = cval;
		if( PLISTEXIST(dthresh) && (thresht=PLISTPIX(pixt, dthresh))<minthresh ) minthresh = thresht;
		if( peak < val ) peak = val;
		rv += cval;
		if( xmin > x ) xmin = x;
		if( xmax < x ) xmax = x;
		if( ymin > y ) ymin = y;
		if( ymax < y ) ymax = y;
		fdnpix++;
	}    

	if( PLISTEXIST(dthresh) ) obj->dthresh = thresh = minthresh;

	// copy some data to "obj" structure 
	obj->fdnpix = (LONG) fdnpix;
	obj->fdflux = (float) rv;
	obj->fdpeak = cpeak;
	obj->dpeak = peak;
	obj->xmin = xmin;
	obj->xmax = xmax;
	obj->ymin = ymin;
	obj->ymax = ymax;

	if( analyse_type & ANALYSE_FULL )
	{
		mx = my = tv = 0.0;
		mx2 = my2 = mxy = 0.0;
		thresh2 = (thresh + peak)/2.0;
		area2 = 0;
		for( pixt=pixel+obj->firstpix; pixt>=pixel; pixt=pixel+PLIST(pixt,nextpix) )
		{
			// avoid roundoff errors on big images 
			x = PLIST(pixt,x)-xmin;	
			// avoid roundoff errors on big images 
			y = PLIST(pixt,y)-ymin;	
			cval = PLISTPIX(pixt, cdvalue);
			tv += (val = PLISTPIX(pixt, dvalue));
			if( val>thresh ) dnpix++;
			if( val > thresh2 ) area2++;
			mx += cval * x;
			my += cval * y;
			mx2 += cval * x*x;
			my2 += cval * y*y;
			mxy += cval * x*y;
		}

		// compute object's properties
		// mean x 
		xm = mx / rv;			
		// mean y 
		ym = my / rv;			

		// In case of blending, use previous barycenters 
		if( (analyse_type&ANALYSE_ROBUST) && (obj->flag&OBJ_MERGED) )
		{
			double	xn,yn;

			xn = obj->mx-xmin;
			yn = obj->my-ymin;
			xm2 = mx2 / rv + xn*xn - 2*xm*xn;
			ym2 = my2 / rv + yn*yn - 2*ym*yn;
			xym = mxy / rv + xn*yn - xm*yn - xn*ym;
			xm = xn;
			ym = yn;

		} else
		{
			// variance of x
			xm2 = mx2 / rv - xm * xm;	
			// variance of y
			ym2 = my2 / rv - ym * ym;	
			// covariance
			xym = mxy / rv - xm * ym;	
		}

		// Handle fully correlated x/y (which cause a singularity...)
		if( (temp2=xm2*ym2-xym*xym)<0.00694 )
		{
			xm2 += 0.0833333;
			ym2 += 0.0833333;
			temp2 = xm2*ym2-xym*xym;
			obj->singuflag = 1;

		} else
			obj->singuflag = 0;

		if( (fabs(temp=xm2-ym2)) > 0.0 )
			theta = atan2(2.0 * xym,temp) / 2.0;
		else
			theta = PI/4.0;

		temp = sqrt(0.25*temp*temp+xym*xym);
		pmy2 = pmx2 = 0.5*(xm2+ym2);
		pmx2+=temp;
		pmy2-=temp;

		obj->dnpix = (obj->flag & OBJ_OVERFLOW)? obj->fdnpix:(LONG)dnpix;
		obj->dflux = tv;
		// add back xmin 
		obj->mx = xm+xmin;	
		// add back ymin 
		obj->my = ym+ymin;	
		obj->mx2 = xm2;
		obj->my2 = ym2;
		obj->mxy = xym;
		obj->a = (float) sqrt(pmx2);
		obj->b = (float) sqrt(pmy2);
		obj->theta = theta*180.0/PI;

		obj->cxx = (float)(ym2/temp2);
		obj->cyy = (float)(xm2/temp2);
		obj->cxy = (float)(-2*xym/temp2);

		darea = (double) (area2 - dnpix);
		t1t2 = thresh/thresh2;
		if( t1t2>0.0 && !prefs.dweight_flag )
		{
			obj->abcor = (darea<0.0?darea:-1.0)/(2*PI*log(t1t2<1.0?t1t2:0.99)
			*obj->a*obj->b);
			if( obj->abcor > 1.0 ) obj->abcor = 1.0;

		} else
			obj->abcor = 1.0;
	}

	return;
}

