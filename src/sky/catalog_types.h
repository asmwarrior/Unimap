////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CATALOG_TYPES_H
#define _CATALOG_TYPES_H

// define star catalogs types
#define CATALOG_ID_NONE				0
// SAO Star Catalog J2000 (SAO Staff 1966; USNO, ADC 1990) (258997 rows) 
#define CATALOG_ID_SAO				1
// Hipparcos Input Catalogue, Version 2 (Turon+ 1993) (118209 rows) 
#define CATALOG_ID_HIPPARCOS		2
// The Tycho-2 Catalogue (Hog+ 2000) (2539913 rows)
#define CATALOG_ID_TYCHO_2			3
// The USNO-B1.0 Catalog (Monet+ 2003) (1045913669 rows) 
#define CATALOG_ID_USNO_B1			4
// NOMAD Catalog (Zacharias+ 2005) (1117612732 rows) 
#define CATALOG_ID_NOMAD			5
// 2MASS All-Sky Catalog of Point Sources (Cutri+ 2003) (470992970 rows) 
#define CATALOG_ID_2MASS			6
// The Guide Star Catalog, Version 2.3.2 (GSC2.3) (STScI, 2006) (945592683 objects)
#define CATALOG_ID_GSC				7

//////////////////////////
// define dso catalog type
#define CATALOG_ID_MESSIER			100
#define CATALOG_ID_NGC				101
#define CATALOG_ID_IC				102
// HYPERLEDA. I. Catalog of galaxies (Paturel+, 2003) (983261 rows)
#define CATALOG_ID_PGC				103
// Uppsala General Catalogue of Galaxies (UGC) (Nilson 1973) (12939 rows) 
#define CATALOG_ID_UGC				104
// Morphological Cat. of Gal. (MCG) (Vorontsov-Velyaminov+, 1962-1974) (29003 rows)
#define CATALOG_ID_MCG				105
// The Millennium Galaxy Catalogue 
// - http://www.eso.org/~jliske/mgc/
// - http://cdsarc.u-strasbg.fr/viz-bin/Cat?VII/240
#define CATALOG_ID_MGC				106
// Catalogue of Principal Galaxies (PGC) (Paturel+ 1989)
#define CATALOG_ID_PGC_				107
// http://leda.univ-lyon1.fr/
#define CATALOG_ID_HYPERLEDA		108

/////////////
// :: gamma/x-ray catalogs	
// The ROSAT All Sky Survey Faint Source Catalog (105924 rows) 
#define CATALOG_ID_ROSAT			150
// The Fourth BATSE Burst Revised Catalog (Paciesas+ 1999)  (2704 rows) 
// 3906 gamma-ray bursts BATSE triggers (Stern+, 2001) - http://www.batse.msfc.nasa.gov/batse/
#define CATALOG_ID_BATSE			151
// INTEGRAL reference catalog (Ebisawa+, 2003) (1122 rows)
#define CATALOG_ID_INTEGRAL			152
// BeppoSAX/GRBM gamma-ray Burst Catalog (Frontera+, 2009)(1082 rows)
#define CATALOG_ID_BEPPOSAX_GRBM	153
// The XMM-Newton 2nd Incremental Source Catalogue (2XMMi) (XMM-SSC, 2008) - 2XMMi Catalogue -(221012 rows) 
#define CATALOG_ID_2XMMi			154
// The GUSBAD catalog of gamma-ray bursts (Schmidt+, 2004) ReadMe  
// The HEAO A-1 X-Ray Source Catalog (Wood+, 1984) (842)
#define CATALOG_ID_HEAO_A1			155
// Exposure Catalog from Chandra X-Ray Observatory 
// The WGACAT version of ROSAT sources (White+ 2000) ??????

///////////////
// :: radio catalogs
// SPECFIND V2.0 Catalog of radio continuum spectra (Vollmer+ 2009) (551852  rows)  
#define CATALOG_ID_SPECFIND			160
// The VLA Low-frequency Sky Survey at 74MHz (Cohen+ 2007) (68311 rows)
// - http://www.cv.nrao.edu/4mass/VLSSlist.shtml
#define CATALOG_ID_VLSS				161
// 1.4GHz NRAO VLA Sky Survey (NVSS) (Condon+ 1998) (1773484 rows)
#define CATALOG_ID_NVSS				162
// Master list of radio sources, updated 1978 (Dixon, 1970) (79493 rows) 
#define CATALOG_ID_MSL				163

////////////
// :: binary - double stars
// The Washington Visual Double Star Catalog (Mason+ 2001-2009) 104223 rows)
#define CATALOG_ID_WDS				170
// CCDM (Catalog of Components of Double & Multiple stars (Dommanget+ 2002) (105838 rows)
#define CATALOG_ID_CCDM				171

/////////////
// :: SUPERNOVAS
// Asiago Supernova Catalogue (Barbon et al., 1999-) - List of SNe in chronological order on 2009-03-15 (4857 rows) 
#define CATALOG_ID_ASC				180
// Sternberg Supernova Catalogue, 2004 version (Tsvetkov+, 2004) (2991 rows) 
#define CATALOG_ID_SSC				181

/////////////
// :: BLACKHOLES
// SDSS DR5 virial black hole masses (Shen+, 2008) (77429 rows)
#define CATALOG_ID_SDSS_DR5			190
// Radio and X-ray-emitting broad-line AGNs (Wang+, 2006) (115 rows)
#define CATALOG_ID_RXE_AGN			191

/////////////////////////
// :: EXO PLANETS
// Search for transiting extrasolar planets (Mallen-Ornelas+, 2003) 
// The Extrasolar Planets Encyclopaedia
#define CATALOG_ID_EXOPLANET_EU		200
// Catalog of nearby exoplanets 
// "Catalog of Nearby Exoplanets" - http://exoplanets.org/planets.shtml
//#define CATALOG_ID_EXOPLANET???	201

/////////////
// :: ASTEROIDS
// Orbits of Minor Planets (Bowell+ 2009) (444016 rows)
#define CATALOG_ID_OMP				210

/////////////
// :: COMETS
// The catalog of Comets (2009-04-05) (Rocher, 2007) (780 rows) 
#define CATALOG_ID_COCOM			220

/////////////
// :: ARTIFICIAL EART SATELLITES
// http://celestrak.com/NORAD/elements/master.asp
#define CATALOG_ID_CELESTRAK		230

/////////////////////////////////////////////////////////////////
// cat load type
#define CATALOG_LOAD_FILE_BINARY	0
#define CATALOG_LOAD_FILE_ASCII		1
#define CATALOG_LOAD_REMOTE			2

/////////////////
// catalog types by what it contains
#define CATALOG_TYPE_OTHER			0
#define CATALOG_TYPE_STARS			1
#define CATALOG_TYPE_DSO			2
#define CATALOG_TYPE_GALAXY			3
#define CATALOG_TYPE_NEBULAES		4
#define CATALOG_TYPE_SOLAR			5
#define CATALOG_TYPE_MEASUREMENTS	6
#define CATALOG_TYPE_RADIO			7
#define CATALOG_TYPE_XGAMMA			8
#define CATALOG_TYPE_BLACKHOLES		9
#define CATALOG_TYPE_SUPERNOVAS		10
#define CATALOG_TYPE_DSTARS			11
#define CATALOG_TYPE_MSTARS			12
#define CATALOG_TYPE_ASTEROIDS		13
#define CATALOG_TYPE_XRAY			14
#define CATALOG_TYPE_GAMMA			15
#define CATALOG_TYPE_EXOPLANETS		16
#define CATALOG_TYPE_COMETS			17
#define CATALOG_TYPE_AES			18

///////////////////////////////////////////
// catalog sources for downloading
#define CATALOG_DOWNLOAD_SOURCE_VIZIER			0
#define CATALOG_DOWNLOAD_SOURCE_UNIMAP			1
#define CATALOG_DOWNLOAD_SOURCE_LEDA			2
#define CATALOG_DOWNLOAD_SOURCE_EXOPLANET_EU	3
#define CATALOG_DOWNLOAD_SOURCE_CELESTRAK		4

///////////////////////////////////////////
// catalog sources for query
#define CATALOG_QUERY_SOURCE_VIZIER				0
#define CATALOG_QUERY_SOURCE_NOFS_NAVY_MIL		1
#define CATALOG_QUERY_SOURCE_LEDA				2
#define CATALOG_QUERY_SOURCE_EXOPLANET_EU		3
#define CATALOG_QUERY_SOURCE_CELESTRAK			4

#endif
