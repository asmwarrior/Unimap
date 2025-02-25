////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#ifndef _CATALOG_AES_H
#define _CATALOG_AES_H

// system libs
#include <vector>
#include <cmath>

// local include
#include "./star.h"
#include "catalog.h"
#include "catalog_types.h"
//#include "catalog_aes_assoc.h"
//#include "catalog_aes_names.h"

// catalogs path
#define DEFAULT_AES_CELESTRAK_BINARY_FILENAME		unMakeAppPath(wxT("data/catalog/aes/celestrak.adm"))
// memory allocation
#define VECT_AES_START_MEM_ALLOC_BLOCK_SIZE		20000
#define VECT_AES_MEM_ALLOC_BLOCK_SIZE			200
#define VECT_AES_TLE_MEM_ALLOC_BLOCK_SIZE		200

////////////
// satellite types
#define EARTH_SAT_TYPE_UNKNOWN					0
#define EARTH_SAT_TYPE_AMATEUR_RADIO			1
#define EARTH_SAT_TYPE_CUBESATS					2
#define EARTH_SAT_TYPE_DISASTER_MONITORING		3
#define EARTH_SAT_TYPE_EDUCATION				4
#define EARTH_SAT_TYPE_ENGINEERING				5
#define EARTH_SAT_TYPE_GALILEO					6
#define EARTH_SAT_TYPE_GEOSTATIONARY			7
#define EARTH_SAT_TYPE_GEODETIC					8
#define EARTH_SAT_TYPE_GLO_OPS					9
#define EARTH_SAT_TYPE_GLOBALSTAR				10
#define EARTH_SAT_TYPE_GOES						11
#define EARTH_SAT_TYPE_GORIZONT					12
#define EARTH_SAT_TYPE_GPS_OPS					13
#define EARTH_SAT_TYPE_INTELSAT					14
#define EARTH_SAT_TYPE_IRIDIUM					15
#define EARTH_SAT_TYPE_MISC_MILITARY			16
#define EARTH_SAT_TYPE_MOLNIYA					17
#define EARTH_SAT_TYPE_MUSSON					18
#define EARTH_SAT_TYPE_NNSS						19
#define EARTH_SAT_TYPE_NOAA						20
#define EARTH_SAT_TYPE_ORBCOMM					21
#define EARTH_SAT_TYPE_OTHER					22
#define EARTH_SAT_TYPE_OTHER_COMM				23
#define EARTH_SAT_TYPE_RADAR					24
#define EARTH_SAT_TYPE_RADUGA					25
#define EARTH_SAT_TYPE_EARTH_RESOURCES			26
#define EARTH_SAT_TYPE_SEARCH_AND_RESCUE		27
#define EARTH_SAT_TYPE_SBAS						28
#define EARTH_SAT_TYPE_SCIENCE					29
#define EARTH_SAT_TYPE_SPACE_STATION			30
#define EARTH_SAT_TYPE_TDRSS					31
#define EARTH_SAT_TYPE_WEATHER					32
#define EARTH_SAT_TYPE_XCOMM					33
#define EARTH_SAT_TYPE_30DAYS_NEW				34
#define EARTH_SAT_TYPE_VISUAL					35
#define EARTH_SAT_TYPE_FENGYUN_1C				36
#define EARTH_SAT_TYPE_IRIDUM_33				37
#define EARTH_SAT_TYPE_COSMOS_2251				38
// other typs
#define EARTH_SAT_TYPE_CUSTOM_MIXED				39

////////////
// earth orbit object types
#define EARTH_ORBIT_OBJECT_UNKNOWN				0
#define ARTIFICIAL_EARTH_SATELLITE				1
#define ARTIFICIAL_EARTH_SPACE_STATION			2
#define ARTIFICIAL_EARTH_SATELLITE_DEBRIS		3
#define ARTIFICIAL_EARTH_SHUTLE_DEBRIS			4
#define ARTIFICIAL_EARTH_LAUNCH_DEBRIS			5
#define ARTIFICIAL_EARTH_MIXED_DEBRIS			6
#define EARTH_ORBIT_OBJECT_MIXED				7

// external classes 
class CUnimapWorker;
class CSky;

////////////////////
// TLE record type
typedef struct
{
	// epoch time - from YYDDD.farction of 24h day - eso_epoch - reference epoch, as an mjd 
	double epoch_time;

	// time derivation of mean motion
	// orbit decay rate, rev/day^2 
	double orbit_decay_rate;
	// this ? don't know what for ???
	double mean_motion_second_time_deriv;

	// BSTAR drag term - object drag coefficient
	double object_drag_coeff;

	// just element number ??? 
	unsigned int element_number;

	// :: LINE 2 of TLE ---
	// Inclination
	double inclination;
	// Right Ascension of the Ascending Node [Degrees] - RA of ascending node, degs 
	double asc_node_ra;
	// Eccentricity (decimal point assumed)
	double eccentricity;
	// Argument of Perigee [Degrees]
	double arg_of_perigee;
	// Mean Anomaly [Degrees]
	double mean_anomaly;
	// Mean Motion [Revs per day]
	double mean_motion;
	// Revolution number at epoch [Revs] - integer orbit number of epoch 
	unsigned int orb_no_epoch;

} DefTLERecord;

////////////////////////
// aes catalog structure
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

	// ra/dec - star coordinates
	double ra;
	double dec;

	////////////////////////////////
	// general use fields
	unsigned char class_of_orbit;
	unsigned char type_of_orbit;
	// object/sat types
	unsigned char obj_type;
	unsigned char vectSatType[100];
	unsigned char nSatType;
//	unsigned int nSatTypeAllocated;

	//////////////////////////
	// :: TLE 2 lines transmition format
	// artificial earth satellite properties - from TLE
	unsigned long norad_cat_no;
	unsigned char security_class;

	// International Designator (COSPAR)
	unsigned int int_id_year;
	unsigned int int_id_launch_no;
	char int_id_launch_piece[4];

	// ephemeris type - used to tell what module of computation to use
	unsigned char ephemeris_type;

	// detect time shift
	double detect_time_shift;

	//////////////
	// TLE RECORDS - updated from celestrak or http://www.space-track.org
	DefTLERecord* vectTLERec;
	unsigned long nTLERec;
	unsigned long nTLERecAllocated;
	unsigned int nTLERecInUse;

	//////////////////////
	// OTHER PROPS
	double vmag;
	double dist_from_obs;

	// catalog type
	unsigned char cat_type;
	// object type
	unsigned char type;

} DefCatAes;

//////////////////////////////////////////////////
// UCS_Satellite_Database details and other.....
typedef struct
{
	wxChar sat_name_full[255];
	wxChar country_of_operator_owner[100];
	wxChar operator_owner[255];
	wxChar users_type[50];
	wxChar purpose[100];

	// ... see the xls for extra details - I have not included here

	// Launch Mass (kg.)
	double launch_mass;
	// Dry Mass (kg.)
	double dry_mass;
	// Power (watts)
	double sat_power;
	// Date of Launch
	double launch_date;
	// Expected Lifetime
	unsigned int expected_lifetime;

	// Contractor name
	wxChar contractor_name[255];
	// Country of Contractor
	wxChar country_of_contractor[100];
	// Launch Site
	wxChar launch_site[255];
	// Launch Vehicle
	wxChar launch_vehicle[255];
	// sat comments
	wxChar sat_comments[500];
	// Source Used for Orbital Data
	wxChar source_used_for_orbital_data[255];

	////////////
	// :: VARIOUS SOURCES FOR DATA
	// Source URLs
	wxChar** vect_source_url;
	unsigned char n_source_url;

} DefCatAesDetails;

// celestrak part/satellite type
typedef struct
{
	wxString		desc;
	wxString		tle_file;
	unsigned char	sat_type;
	unsigned char	obj_type;

} CelestrakPartType;

// aes object type
typedef struct
{
	wxString		desc;
	unsigned char	obj_type;

} AesObjectType;

// reset to zero function
void ResetObjCatAes( DefCatAes& src );
void ResetObjCatAesTLERec( DefTLERecord& src );

// global sorting methods
static bool OnSortCatAesByNoradNo( DefCatAes t1, DefCatAes t2 ) { return( t1.norad_cat_no<t2.norad_cat_no ); }
static bool OnSortCatAesTLERecords( DefTLERecord t1, DefTLERecord t2 ) { return( t1.epoch_time>t2.epoch_time ); }

// class :: CSkyCatalogAes
////////////////////////////////////////////////////////////////////
class CSkyCatalogAes : public CSkyCatalog
{
// methods:
public:
	CSkyCatalogAes( CSky* pSky );
	~CSkyCatalogAes( );

	// general dso methods
	void UnloadCatalog( );

	// add new sat type to object
	int AddSatTypeToObject( DefCatAes* aes, unsigned char nSatType );

	long SortByNoradCatNo( );
	long SortTLERecords( );

	// calculate sat position with ecs-ephem
	int CalcRaDec( DefCatAes& aes, double nTime, double nLat, double nLon, double nAlt, 
					double& nRa, double& nDec, double& nDist, int bEpochSet=0 );

	int GetName( DefCatAes& aes, wxString& strAesCatName );

	DefCatAes* GetSatTypeByNoradNo( unsigned long nCatNo );
	// check if this object has category
	int IsObjCategory( DefCatAes* src, unsigned char nObjType );
	// check if object has ecpoch tle already
	int IsObjTLEEpoch( DefCatAes* src, double nEpoch );

	// init tle ecpoch range
	int InitObjTleEpoch( DefCatAes* src, double nStartTime, double nEndTime );
	// get best tle epoch id
	long GetBestTleId( DefCatAes* src, double nObsTime );
	// add new TLE RECORD
	int AddTLERecordToObject( DefCatAes* aes, DefTLERecord& tle );

	unsigned long Load( int nType, int bRegion=0, double nCenterRa=0, double nCenterDec=0, double nRadius=0 );
	unsigned long LoadBinary( double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	unsigned long LoadBinary( const wxChar* strFile, double nCenterRa, double nCenterDec, double nRadius, int bRegion );
	int ExportBinary( DefCatAes* vectCatalog, unsigned int nSize );

	// method to call to download catalogs from a remote location
	unsigned long DownloadCatalog( int nCatId, int nCatSource, int bClear=0 );

//////////////////////////
// DATA
public:
	CSky* m_pSky;
	CUnimapWorker* m_pUnimapWorker;

	// comets catalog - general purpose
	DefCatAes* m_vectData;

	// object tle start/end search epoch indexes
	long m_nObjTleSearchEpochStart;
	long m_nObjTleSearchEpochEnd;

	// site parts
	CelestrakPartType m_vectSatType[100];
	int m_nSatType;
	//  object types
	AesObjectType m_vectObjType[20];
	int m_nObjType;

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

