/*
 				sexhead1.h

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	SExtractor
*
*	Author:		E.BERTIN (IAP)
*
*	Contents:	header (FITS format #1) and templates for catalog data.
*
*	Last modify:	16/12/2002
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

#ifndef _SEXTRACTOR_SEXHEADONE_H
#define _SEXTRACTOR_SEXHEADONE_H

//int	idummy;
//double	ddummy;

/*
keystruct	headkey1[] = {
  {"EPOCH   ", "",
	&thefield1.epoch, H_FLOAT, T_DOUBLE, "%7.2f"},
  {"OBJECT  ", "",
	thefield1.ident, H_STRING, T_STRING, "%18s"},
  {"ORIGIN  ", "",
	"SExtractor", H_STRING, T_STRING, "%18s"},
  {"CRVAL1", "WORLD X COORD. OF REFERENCE PIXEL",
	&ddummy, H_EXPO, T_DOUBLE, "%15G"},
  {"CRVAL2", "WORLD Y COORD. OF REFERENCE PIXEL",
	&ddummy, H_EXPO, T_DOUBLE, "%15G"},
  {"CRPIX1", "IMAGE X COORD. OF REFERENCE PIXEL",
	&idummy, H_INT, T_LONG, "%5d"},
  {"CRPIX2", "IMAGE Y COORD. OF REFERENCE PIXEL",
	&idummy, H_INT, T_LONG, "%5d"},
  {"CDELT1", "WORLD PIXEL STEP ALONG X",
	&ddummy, H_EXPO, T_DOUBLE, "%15G"},
  {"CDELT2", "WORLD PIXEL STEP ALONG Y",
	&ddummy, H_EXPO, T_DOUBLE, "%15G"},
  {"CROTA1", "CCW ANGLE FROM X-IMAGE TO X-WORLD",
	&ddummy, H_EXPO, T_DOUBLE, "%15G"},
  {"CROTA2", "CCW ANGLE FROM Y-IMAGE TO Y-WORLD",
	&ddummy, H_EXPO, T_DOUBLE, "%15G"},
  {"FITSFILE", "File name of the analysed image",
	thecat.image_name, H_STRING, T_STRING, "%-18s"},
  {"FITSEXT ", "FITS Extension number",
	&thecat.currext, H_INT, T_LONG, "%3d"},
  {"FITSNEXT", "Number of FITS image extensions in file",
	&thecat.next, H_INT, T_LONG, "3d"},
  {"SEXIMASX", "IMAGE WIDTH (PIXELS)",
	&thefield1.width, H_INT, T_LONG, "%10d"},
  {"SEXIMASY", "IMAGE HEIGHT (PIXELS)",
	&thefield1.height, H_INT, T_LONG, "%10d"},
  {"SEXSTRSY", "STRIP HEIGHT (LINES)",
	&thefield1.stripheight, H_INT, T_LONG, "%10d"},
  {"SEXIMABP", "FITS IMAGE BITPIX",
	&thefield1.bitpix, H_INT, T_LONG, "%3d"},
  {"SEXPIXS", "PIXEL SCALE (ARCSEC)",
	&thefield1.pixscale, H_EXPO, T_DOUBLE, "%12G"},
  {"SEXSFWHM", "SEEING FWHM (ARCSEC)",
	&prefs.seeing_fwhm, H_EXPO, T_DOUBLE, "%12G"},
  {"SEXNNWF ", "CLASSIFICATION NNW FILENAME",
	thecat.nnw_name, H_STRING, T_STRING, "%18s"},
  {"SEXGAIN ", "GAIN (IN E- PER ADU)",
	&prefs.gain, H_EXPO, T_DOUBLE, "%7.3F"},
  {"SEXBKGND", "MEDIAN BACKGROUND (ADU)",
	&thefield1.backmean, H_EXPO, T_FLOAT, "%12G"},
  {"SEXBKDEV", "MEDIAN RMS (ADU)",
	&thefield1.backsig, H_EXPO, T_FLOAT, "%12G"},
  {"SEXBKTHD", "EXTRACTION THRESHOLD (ADU)",
	&thefield2.thresh, H_EXPO, T_FLOAT, "%12G"},
  {"SEXCONFF", "CONFIGURATION FILENAME",
	thecat.prefs_name, H_STRING, T_STRING, "%18s"},
  {"SEXDETT ", "DETECTION TYPE",
	"CCD", H_STRING, T_STRING, "%s"},
  {"SEXTHLDT", "THRESHOLD TYPE",
	"SIGMA", H_STRING, T_STRING, "%s"},
  {"SEXTHLD ", "THRESHOLD",
	&prefs.dthresh[0], H_EXPO, T_DOUBLE, "%12G"},
  {"SEXMINAR", "EXTRACTION MINIMUM AREA (PIXELS)",
	&prefs.ext_minarea, H_INT, T_LONG, "%6d"},
  {"SEXCONV ", "CONVOLUTION FLAG",
	&prefs.filter_flag, H_BOOL, T_LONG, "%1s"},
  {"SEXCONVN", "CONVOLUTION NORM. FLAG",
	&prefs.filter_flag, H_BOOL, T_LONG, "%1s"},
  {"SEXCONVF", "CONVOLUTION FILENAME",
	thecat.filter_name, H_STRING, T_STRING, "%18s"},
  {"SEXDBLDN", "NUMBER OF SUB-THRESHOLDS",
	&prefs.deblend_nthresh, H_INT, T_LONG, "%3d"},
  {"SEXDBLDC", "CONTRAST PARAMETER",
	&prefs.deblend_mincont, H_FLOAT, T_DOUBLE, "%8f"},
  {"SEXCLN  ", "CLEANING FLAG",
	&prefs.clean_flag, H_BOOL, T_LONG, "%1s"},
  {"SEXCLNPA", "CLEANING PARAMETER",
	&prefs.clean_param, H_FLOAT, T_DOUBLE, "%8f"},
  {"SEXCLNST", "CLEANING OBJECT-STACK",
	&prefs.deblend_nthresh, H_INT, T_LONG, "%6d"},
  {"SEXAPERD", "APERTURE DIAMETER (PIXELS)",
	&prefs.apert[0], H_INT, T_LONG, "%7.1f"},
  {"SEXAPEK1", "KRON PARAMETER",
	&prefs.autoparam[0], H_FLOAT, T_DOUBLE, "%4.1f"},
  {"SEXAPEK2", "KRON ANALYSIS RADIUS",
	&prefs.autoparam[0], H_FLOAT, T_DOUBLE, "%4.1f"},
  {"SEXAPEK3", "KRON MINIMUM RADIUS",
	&prefs.autoparam[1], H_FLOAT, T_DOUBLE, "%4.1f"},
  {"SEXSATLV", "SATURATION LEVEL (ADU)",
	&prefs.satur_level, H_EXPO, T_DOUBLE, "%12G"},
  {"SEXMGZPT", "MAGNITUDE ZERO-POINT",
	&prefs.mag_zeropoint, H_FLOAT, T_DOUBLE, "%8.4f"},
  {"SEXMGGAM", "MAGNITUDE GAMMA",
	&prefs.mag_gamma, H_FLOAT, T_DOUBLE, "%4.2f"},
  {"SEXBKGSX", "BACKGROUND MESH WIDTH (PIXELS)",
	&thefield1.backw, H_INT, T_LONG, "%5d"},
  {"SEXBKGSY", "BACKGROUND MESH HEIGHT (PIXELS)",
	&thefield1.backh, H_INT, T_LONG, "%5d"},
  {"SEXBKGFX", "BACKGROUND FILTER WIDTH",
	&thefield1.nbackfx, H_INT, T_LONG, "%3d"},
  {"SEXBKGFY", "BACKGROUND FILTER HEIGHT",
	&thefield1.nbackfy, H_INT, T_LONG, "%3d"},
  {"SEXPBKGT", "PHOTOM BACKGROUND TYPE",
	"GLOBAL", H_STRING, T_STRING, "%s"},
  {"SEXPBKGS", "LOCAL AREA THICKNESS (PIXELS)",
	&prefs.pback_size, H_INT, T_LONG, "%3d"},
  {"SEXPIXSK", "PIXEL STACKSIZE (PIXELS)",
	&prefs.mem_pixstack, H_INT, T_LONG, "%8d"},
  {"SEXFBUFS", "FRAME-BUFFER SIZE (LINES)",
	&prefs.mem_bufsize, H_INT, T_LONG, "%5d"},
  {"SEXISAPR", "ISO-APER RATIO",
	 &ddummy, H_FLOAT, T_DOUBLE, "%4.2f"},
  {"SEXNDET ", "NB OF DETECTIONS",
	&thecat.ndetect, H_INT, T_LONG, "%9d"},
  {"SEXNFIN ", "NB OF FINAL EXTRACTED OBJECTS",
	&thecat.ntotal, H_INT, T_LONG, "%9d"},
  {"SEXNPARA", "NB OF PARAMETERS PER OBJECT",
	&thecat.nparam, H_INT, T_LONG, "%3d"},
  {""}};
*/

#endif
