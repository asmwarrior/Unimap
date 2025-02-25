////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CATALOG_EXOPLANETS_H
#define _CATALOG_EXOPLANETS_H

// local include
#include "./star.h"
#include "catalog.h"
#include "catalog_types.h"
//#include "catalog_exoplanets_assoc.h"
//#include "catalog_exoplanets_names.h"

// catalogs path
#define DEFAULT_EXOPLANET_EU_BINARY_FILENAME	unMakeAppPath(wxT("data/catalog/exoplanets/exoplanet_eu.adm"))
// memory allocation
#define VECT_EXOPLANETS_START_MEM_ALLOC_BLOCK_SIZE		400
#define VECT_EXOPLANETS_MEM_ALLOC_BLOCK_SIZE			400

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

	// ra/dec - star coordinates
	double ra;
	double dec;

	// planet properties
	double mass;
	double radius;
	double period;
	double sm_axis;
	double eccentricity;
	double inclination;
	double ang_dist;

	unsigned char status;
	unsigned int discovery_year;
	double update_date;

	// star magnitude
	double star_mag_v;
	double star_mag_i;
	double star_mag_h;
	double star_mag_j;
	double star_mag_k;
	// star distance pc
	double star_dist;
	// star metalicity
	double star_feh;
	// star mass
	double star_mass;
	// star radius
	double star_radius;

	// catalog type
	unsigned char cat_type;
	// object type
	unsigned char type;

} DefCatExoPlanet;

// reset to zero function
void ResetObjCatExoplanet( DefCatExoPlanet& src );

// global sorting methods
//static bool OnSortCatExoPlanets( DefCatExoPlanet t1, DefCatExoPlanet t2 ) { return( t1.mass>t2.mass ); }
//static bool OnSortCatExoPlanetsByA( DefCatExoPlanet t1, DefCatExoPlanet t2 ) { return( t1.a>t2.a ); }

class CSkyCatalogExoplanets : public CSkyCatalog
{
// methods:
public:
	CSkyCatalogExoplanets( CSky* pSky );
	~CSkyCatalogExoplanets( );

	// general dso methods
	void UnloadCatalog( );

//	long SortBy???( );
//	long SortByA( );
	int GetRaDec( unsigned long nCatNo, double& nRa, double& nDec );
	int GetName( DefCatExoPlanet& exoplanet, wxString& strExoPlanetCatName );

	unsigned long Load( int nType, int bRegion=0, double nCenterRa=0, double nCenterDec=0, double nRadius=0 );
	unsigned long LoadBinary( double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	unsigned long LoadBinary( const wxChar* strFile, double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	int ExportBinary( DefCatExoPlanet* vectCatalog, unsigned int nSize );

	// method to call to download catalogs from a remote location
	unsigned long DownloadCatalog( int nCatId, int nCatSource );

//////////////////////////
// DATA
public:
	CSky* m_pSky;
	CUnimapWorker* m_pUnimapWorker;

	// blackholes catalog - general purpose
	DefCatExoPlanet*	m_vectData;

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

