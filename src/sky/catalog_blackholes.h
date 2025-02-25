////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CATALOG_BLACKHOLES_H
#define _CATALOG_BLACKHOLES_H

// local include
#include "./star.h"
#include "catalog.h"
#include "catalog_types.h"
//#include "catalog_blackholes_assoc.h"
//#include "catalog_blackholes_names.h"

// catalogs path
#define DEFAULT_SDSS_DR5_BINARY_FILENAME	unMakeAppPath(wxT("data/catalog/blackholes/sdss_dr5.adm"))
#define DEFAULT_RXE_AGN_BINARY_FILENAME		unMakeAppPath(wxT("data/catalog/blackholes/rxe_agn.adm"))
// memory allocation
#define VECT_START_MEM_ALLOC_BLOCK_SIZE		200
#define VECT_BLACKHOLES_MEM_ALLOC_BLOCK_SIZE	200

// external classes
class CUnimapWorker;
class CSky;

// blackholes catalog structure
typedef struct
{
//#ifdef _DEBUG
	wxChar cat_name[30];
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

	// mass
	double mass;
	double mass_hbeta;
	double mass_mgii;
	double mass_civ;

	// luminosity;
	double luminosity;
	double radio_luminosity;
	double xray_luminosity;

	// redshit
	double redshift;

	// target
	unsigned char target_type;

	// catalog type
	unsigned char cat_type;
	// object type
	unsigned char type;

} DefCatBlackhole;

// reset to zero function
void ResetObjCatBlackhole( DefCatBlackhole& src );

// global sorting methods
static bool OnSortCatBlackholes( DefCatBlackhole t1, DefCatBlackhole t2 ) { return( t1.mass>t2.mass ); }
//static bool OnSortCatSupernovasByA( DsoDefCatBasic t1, DsoDefCatBasic t2 ) { return( t1.a>t2.a ); }

class CSkyCatalogBlackholes : public CSkyCatalog
{
// methods:
public:
	CSkyCatalogBlackholes( CSky* pSky );
	~CSkyCatalogBlackholes( );

	// general dso methods
	void UnloadCatalog( );

	long SortByMass( );
//	long SortByA( );
	int GetRaDec( unsigned long nCatNo, double& nRa, double& nDec );
	int GetName( DefCatBlackhole& blackhole, wxString& strBlackholeCatName );

	unsigned long Load( int nType, int bRegion=0, double nCenterRa=0, double nCenterDec=0, double nRadius=0 );
	unsigned long LoadBinary( double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	unsigned long LoadBinary( const wxChar* strFile, double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	int ExportBinary( DefCatBlackhole* vectCatalog, unsigned int nSize );

	// method to call to download catalogs from a remote location
	unsigned long DownloadCatalog( int nCatId, int nCatSource );

//////////////////////////
// DATA
public:
	CSky* m_pSky;
	CUnimapWorker* m_pUnimapWorker;

	// blackholes catalog - general purpose
	DefCatBlackhole*	m_vectData;

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

