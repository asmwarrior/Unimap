////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CATALOG_COMETS_H
#define _CATALOG_COMETS_H

// local include
#include "./star.h"
#include "catalog.h"
#include "catalog_types.h"
//#include "catalog_comets_assoc.h"
//#include "catalog_comets_names.h"

// catalogs path
#define DEFAULT_COMETS_COCOM_BINARY_FILENAME		unMakeAppPath(wxT("data/catalog/comets/cocom.adm"))
// memory allocation
#define VECT_COMETS_START_MEM_ALLOC_BLOCK_SIZE		200
#define VECT_COMETS_MEM_ALLOC_BLOCK_SIZE			200

// external classes 
class CUnimapWorker;
class CSky;

////////////////////////
// comets catalog structure
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

	// comet properties
	double note_update_date;
	wxChar iau_code[14];
	wxChar comp_name[11];

	double epoch_comp;
	unsigned char rel_effect;

/*
	double x_pos_vect;
	double y_pos_vect;
	double z_pos_vect;

	double x_vel_vect;
	double y_vel_vect;
	double z_vel_vect;

	double x_nongrav_force;
	double y_nongrav_force;
	double z_nongrav_force;
*/

	double perihelion_date;

	double perihelion_distance;
	double arg_perihelion;
	double lon_orbital_node;

	double inclination;
	double eccentricity;

/*	
	double h1, r1, d1;
	double h2, r2, d2;
*/

	// catalog type
	unsigned char cat_type;
	// object type
	unsigned char type;

} DefCatComet;

// reset to zero function
void ResetObjCatComet( DefCatComet& src );

// global sorting methods
//static bool OnSortCatComet( DefCatComet t1, DefCatComet t2 ) { return( t1.mass>t2.mass ); }
//static bool OnSortCatCometByA( DefCatComet t1, DefCatComet t2 ) { return( t1.a>t2.a ); }

// class :: CSkyCatalogComets
////////////////////////////////////////////////////////////////////
class CSkyCatalogComets : public CSkyCatalog
{
// methods:
public:
	CSkyCatalogComets( CSky* pSky );
	~CSkyCatalogComets( );

	// general dso methods
	void UnloadCatalog( );

//	long SortBy???( );
//	long SortByA( );
	int GetRaDec( unsigned long nCatNo, double& nRa, double& nDec );
	int CalcRaDec( DefCatComet& comet, double nTime, double nLat, double nLon, double nAlt, double& nRa, double& nDec );
	int GetName( DefCatComet& comet, wxString& strCometCatName );

	unsigned long Load( int nType, int bRegion=0, double nCenterRa=0, double nCenterDec=0, double nRadius=0 );
	unsigned long LoadBinary( double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	unsigned long LoadBinary( const wxChar* strFile, double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	int ExportBinary( DefCatComet* vectCatalog, unsigned int nSize );

	// method to call to download catalogs from a remote location
	unsigned long DownloadCatalog( int nCatId, int nCatSource );

//////////////////////////
// DATA
public:
	CSky* m_pSky;
	CUnimapWorker* m_pUnimapWorker;

	// comets catalog - general purpose
	DefCatComet* m_vectData;

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

