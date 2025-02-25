////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CATALOG_ASTEROIDS_H
#define _CATALOG_ASTEROIDS_H

// local include
#include "./star.h"
#include "catalog.h"
#include "catalog_types.h"
//#include "catalog_asteroids_assoc.h"
//#include "catalog_asteroids_names.h"

// catalogs path
#define DEFAULT_ASTEROIDS_OMP_BINARY_FILENAME		unMakeAppPath(wxT("data/catalog/asteroids/omp.adm"))
// memory allocation
#define VECT_ASTEROIDS_START_MEM_ALLOC_BLOCK_SIZE		200
#define VECT_ASTEROIDS_MEM_ALLOC_BLOCK_SIZE				200

// external classes 
class CUnimapWorker;
class CSky;

////////////////////
// asteroids catalog structure
typedef struct
{
//#ifdef _DEBUG
	char cat_name[20];
//#else
//	char* cat_name;
//#endif

	// catalogs numbers
	unsigned long cat_no;

	// x/y in image
	double x;
	double y;

	// ra/dec - star coordinates
	double ra;
	double dec;

	// asteroid properties
	unsigned long ast_no;
	char comp_name[18];

	double abs_mag_h;
	double slope_mag;
	double color_index;
	double iras_diam;
	char iras_class[6];

	unsigned long no_arc;
	unsigned int no_obs;

	double epoch;
	double mean_anomaly; 
	double arg_perihelion;
	double lon_asc_node;

	double date_orb_comp;
	double update_date;

	double inclination;
	double eccentricity;
	double smaj_axis;

	// catalog type
	unsigned char cat_type;
	// object type
	unsigned char type;

} DefCatAsteroid;

// reset to zero function
void ResetObjCatAsteroid( DefCatAsteroid& src );

// global sorting methods
//static bool OnSortCatAsteroid( DefCatAsteroid t1, DefCatAsteroid t2 ) { return( t1.mass>t2.mass ); }
//static bool OnSortCatAsteroidByA( DefCatAsteroid t1, DefCatAsteroid t2 ) { return( t1.a>t2.a ); }

class CSkyCatalogAsteroids : public CSkyCatalog
{
// methods:
public:
	CSkyCatalogAsteroids( CSky* pSky );
	~CSkyCatalogAsteroids( );

	// general dso methods
	void UnloadCatalog( );

//	long SortBy???( );
//	long SortByA( );
	int GetRaDec( unsigned long nCatNo, double& nRa, double& nDec );
	int CalcRaDec( DefCatAsteroid& asteroid, double nTime, double nLat, double nLon, double& nRa, double& nDec );
	int CalcRaDecX( DefCatAsteroid& asteroid, double nTime, double nLat, double nLon, double nAlt, double& nRa, double& nDec );
	int GetName( DefCatAsteroid& asteroid, wxString& strAsteroidCatName );

	unsigned long Load( int nType, int bRegion=0, double nCenterRa=0, double nCenterDec=0, double nRadius=0 );
	unsigned long LoadBinary( double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	unsigned long LoadBinary( const wxChar* strFile, double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	int ExportBinary( DefCatAsteroid* vectCatalog, unsigned int nSize );

	// method to call to download catalogs from a remote location
	unsigned long DownloadCatalog( int nCatId, int nCatSource );

//////////////////////////
// DATA
public:
	CSky* m_pSky;
	CUnimapWorker* m_pUnimapWorker;

	// asteroids catalog - general purpose
	DefCatAsteroid*	m_vectData;

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

