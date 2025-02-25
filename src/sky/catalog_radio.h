////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CATALOG_RADIO_H
#define _CATALOG_RADIO_H

// local include
#include "./star.h"
#include "catalog.h"
#include "catalog_types.h"
//#include "catalog_radio_assoc.h"
//#include "catalog_radio_names.h"

// catalogs path
#define DEFAULT_SPECFIND_BINARY_FILENAME	unMakeAppPath(wxT("data/catalog/radio/specfind.adm"))
#define DEFAULT_VLSS_BINARY_FILENAME	unMakeAppPath(wxT("data/catalog/radio/vlss.adm"))
// memory allocation
#define VECT_START_MEM_ALLOC_BLOCK_SIZE	200
#define VECT_RADIO_MEM_ALLOC_BLOCK_SIZE	200

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

	// frequency - in mhz
	double frequency;
	// Flux density
	double flux_density;
	// integrated flux density
	double i_flux_density;

	// number of points
	unsigned int no_spec_points;

	// a/b slope and abscissa - - The coefficients of a and b 
	// of the flux density parametrization log(S(gamma)) = a.log(gamma) + b 
	// where the log is decimal (log10), S expressed in mJy and gamma in MHz.
	// Thus b represents log(S(1MHz)/1mJy). 
	double spec_slope;
	double spec_abscissa;

	// catalog type
	unsigned char cat_type;
	// object type
	unsigned char type;

} DefCatBasicRadio;

// global sorting methods
//static bool OnSortCatRadio( DsoDefCatBasic t1, DsoDefCatBasic t2 ) { return( t1.mag<t2.mag ); }
//static bool OnSortCatRAdioByA( DsoDefCatBasic t1, DsoDefCatBasic t2 ) { return( t1.a>t2.a ); }

class CSkyCatalogRadio : public CSkyCatalog
{
// methods:
public:
	CSkyCatalogRadio( CSky* pSky );
	~CSkyCatalogRadio( );

	// general dso methods
	void UnloadCatalog( );

//	long SortByMagnitude( );
//	long SortByA( );
	int GetRaDec( unsigned long nCatNo, double& nRa, double& nDec );
	int GetName( DefCatBasicRadio& radioSource, wxString& strRadioCatName );

	unsigned long Load( int nType, int bRegion=0, double nCenterRa=0, double nCenterDec=0, double nRadius=0 );
	unsigned long LoadBinary( double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	unsigned long LoadBinary( const wxString& strFile, double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	int ExportBinary( DefCatBasicRadio* vectCatalog, unsigned int nSize );

	// method to call to download catalogs from a remote location
	unsigned long DownloadCatalog( int nCatId, int nCatSource );

//////////////////////////
// DATA
public:
	CSky* m_pSky;
	CUnimapWorker* m_pUnimapWorker;

	// DSO catalog - general purpose
	DefCatBasicRadio*	m_vectData;

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

