////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CATALOG_XGAMMA_H
#define _CATALOG_XGAMMA_H

// local include
#include "./star.h"
#include "catalog.h"
#include "catalog_types.h"
//#include "catalog_xgamma_assoc.h"
//#include "catalog_xgamma_names.h"

// catalogs path
#define DEFAULT_ROSAT_BINARY_FILENAME		unMakeAppPath(wxT("data/catalog/xgamma/rosat.adm"))
#define DEFAULT_BATSE_BINARY_FILENAME		unMakeAppPath(wxT("data/catalog/xgamma/batse.adm"))
#define DEFAULT_INTEGRAL_BINARY_FILENAME	unMakeAppPath(wxT("data/catalog/xgamma/integral.adm"))
// memory allocation
#define VECT_START_MEM_ALLOC_BLOCK_SIZE		200
#define VECT_XGAMMA_MEM_ALLOC_BLOCK_SIZE	200

// external classes
class CUnimapWorker;
class CSky;

// radio catalog structure
typedef struct
{
//#ifdef _DEBUG
	char cat_name[30];
//#else
//	char* cat_name;
//#endif

	// catalogs numbers
	unsigned long cat_no;

	// x/y in image
	double x;
	double y;

	// ra/dec - coordinates
	double ra;
	double dec;

	// count ct/s
	double count; 
	// Background countrate
	double bg_count;
	// iExposure time
	double exp_time;

	// Flux :: Peak flux in the 50-300keV range  
	// Flux :: Mean flux in 0.2-12keV band   mW/m2 
	double flux;
	// Start of the event within the day 
	unsigned long start_time;
	// T90 :: Duration rounded to integer seconds 
	unsigned long duration;
	// Burst :: Terrestrial Date and Time of event start 
	unsigned long burst_date_time;

	///////////////////////
	// F3-10keV :: 3-10 keV energy flux (JEMX) mW/m2  ... etc
	// Flux1 ::  Mean flux in 0.2-0.5keV band   mW/m2
	double flux_band_1;
	// F10-30keV :: 10-30 keV energy flux (JEMX) mW/m2
	// Flux2 :: Mean flux in 0.5-1.0keV band   mW/m2 
	double flux_band_2;
	// F20-60keV :: 20-60 keV energy flux (ISGRI) mW/m2 
	// Flux3 :: Mean flux in 1.0-2.0keV band   mW/m2 
	double flux_band_3;
	// F60-200keV :: 60-200 keV energy flux (ISGRI) mW/m2
	// Flux4 :: Mean flux in 2.0-4.5keV band   mW/m2 
	double flux_band_4;
	// Flux5 :: Mean flux in 4.5-12keV band   mW/m2 
	double flux_band_5;
	// Flux14 :: Mean flux in 0.5-4.5keV band
	double flux_band_6;

	////////////////////////////////
	// COUNTS by BAND
	// CR3-10keV    JEMX soft band counting rates
	double count_band_1;
	// CR10-30keV   JEMX hard band counting rates
	double count_band_2;
	// CR20-60keV   ISGRI soft band counting rates
	double count_band_3;
	// CR60-200keV  ISGRI hard band counting rates
	double count_band_4;

	// Type :: Source type
	unsigned char source_type;

	// Tdet :: Time elapsed from the earliest GRB 
	unsigned long time_det;
	// Na :: Number of intervals
	unsigned int interval_no;
	// fluence :: 40-700keV fluence in 10-6erg/cm2   nJ/m2 
	double fluence;
	// Fpeak :: 40-700keV peak flux in 10-7erg/cm2/s 10-10W/m2
	double peak_flux;
	// Gamma :: Best fit photon index  
	double gamma;

	// Nd :: Number of detections
	unsigned int no_detections;

	// catalog type
	unsigned char cat_type;
	// object type
	unsigned char type;

} DefCatBasicXGamma;

// reset to zero function
void ResetObjCatXGamma( DefCatBasicXGamma& src );

// global sorting methods
//static bool OnSortCatXGamma( DsoDefCatBasic t1, DsoDefCatBasic t2 ) { return( t1.mag<t2.mag ); }
//static bool OnSortCatXGammaByA( DsoDefCatBasic t1, DsoDefCatBasic t2 ) { return( t1.a>t2.a ); }

class CSkyCatalogXGamma : public CSkyCatalog
{
// methods:
public:
	CSkyCatalogXGamma( CSky* pSky );
	~CSkyCatalogXGamma( );

	// general dso methods
	void UnloadCatalog( );

//	long SortByMagnitude( );
//	long SortByA( );
	int GetRaDec( unsigned long nCatNo, double& nRa, double& nDec );
	int GetName( DefCatBasicXGamma& xgammaSource, wxString& strXGammaCatName );

	unsigned long Load( int nType, int bRegion=0, double nCenterRa=0, double nCenterDec=0, double nRadius=0 );
	unsigned long LoadBinary( double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	unsigned long LoadBinary( const wxString& strFile, double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	int ExportBinary( DefCatBasicXGamma* vectCatalog, unsigned int nSize );

	// method to call to download catalogs from a remote location
	unsigned long DownloadCatalog( int nCatId, int nCatSource );

//////////////////////////
// DATA
public:
	CSky* m_pSky;
	CUnimapWorker* m_pUnimapWorker;

	// DSO catalog - general purpose
	DefCatBasicXGamma*	m_vectData;

	// mag references
	double m_nMinMag;
	double m_nMaxMag;

	////////////////////
	// last region loaded
	int m_bLastLoadRegion;
	double m_nLastRegionLoadedCenterRa;
	double m_nLastRegionLoadedCenterDec;
	double m_nLastRegionLoadedWidth;
	double m_nLastRegionLoadedHeight;

};

#endif

