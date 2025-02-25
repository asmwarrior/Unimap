/* soapStub.h
   Generated by gSOAP 2.7.16 from unimap_service.h
   Copyright(C) 2000-2010, Robert van Engelen, Genivia Inc. All Rights Reserved.
   This part of the software is released under one of the following licenses:
   GPL, the gSOAP public license, or Genivia's license for commercial use.
*/

#ifndef soapStub_H
#define soapStub_H
#include <vector>
#define SOAP_NAMESPACE_OF_ns	"urn:UnimapSoapXml"
#include "stdsoap2.h"

/******************************************************************************\
 *                                                                            *
 * Enumerations                                                               *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Types with Custom Serializers                                              *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Classes and Structs                                                        *
 *                                                                            *
\******************************************************************************/


#if 0 /* volatile type: do not redeclare here */

#endif

#if 0 /* volatile type: do not redeclare here */

#endif

#ifndef SOAP_TYPE_ns__ResponseStatusType
#define SOAP_TYPE_ns__ResponseStatusType (10)
/* ns:ResponseStatusType */
class SOAP_CMAC ns__ResponseStatusType
{
public:
	std::string ResponseResult;	/* required element of type xsd:string */
	short ResponseCode;	/* required element of type xsd:short */
	std::vector<std::string >ResponseMessage;	/* optional element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 10; } /* = unique id SOAP_TYPE_ns__ResponseStatusType */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns__ResponseStatusType(): soap(NULL) { ns__ResponseStatusType::soap_default(NULL); }
	virtual ~ns__ResponseStatusType() { }
};
#endif

#ifndef SOAP_TYPE_ns__ImgDataHeaderType
#define SOAP_TYPE_ns__ImgDataHeaderType (11)
/* ns:ImgDataHeaderType */
class SOAP_CMAC ns__ImgDataHeaderType
{
public:
	std::string ObjDetected;	/* required element of type xsd:integer */
	std::string ObjMatched;	/* required element of type xsd:integer */
	std::string TargetName;	/* required element of type xsd:string */
	double OrigRa;	/* required element of type xsd:double */
	double OrigDec;	/* required element of type xsd:double */
	double FieldWidth;	/* required element of type xsd:double */
	double FieldHeight;	/* required element of type xsd:double */
	double MinMag;	/* required element of type xsd:double */
	double MaxMag;	/* required element of type xsd:double */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 11; } /* = unique id SOAP_TYPE_ns__ImgDataHeaderType */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns__ImgDataHeaderType(): soap(NULL) { ns__ImgDataHeaderType::soap_default(NULL); }
	virtual ~ns__ImgDataHeaderType() { }
};
#endif

#ifndef SOAP_TYPE_ns__ImgObjDetectedType
#define SOAP_TYPE_ns__ImgObjDetectedType (12)
/* ns:ImgObjDetectedType */
class SOAP_CMAC ns__ImgObjDetectedType
{
public:
	std::string id;	/* required element of type xsd:integer */
	std::string name;	/* required element of type xsd:string */
	double x;	/* required element of type xsd:double */
	double y;	/* required element of type xsd:double */
	double ra;	/* required element of type xsd:double */
	double dec;	/* required element of type xsd:double */
	double mag;	/* required element of type xsd:double */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 12; } /* = unique id SOAP_TYPE_ns__ImgObjDetectedType */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns__ImgObjDetectedType(): soap(NULL) { ns__ImgObjDetectedType::soap_default(NULL); }
	virtual ~ns__ImgObjDetectedType() { }
};
#endif

#ifndef SOAP_TYPE_ns__ImageSolveRequestType
#define SOAP_TYPE_ns__ImageSolveRequestType (13)
/* ns:ImageSolveRequestType */
class SOAP_CMAC ns__ImageSolveRequestType
{
public:
	std::string ImageName;	/* required element of type xsd:string */
	std::string ImagePath;	/* required element of type xsd:string */
	std::string SearchCatalog;	/* required element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 13; } /* = unique id SOAP_TYPE_ns__ImageSolveRequestType */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns__ImageSolveRequestType(): soap(NULL) { ns__ImageSolveRequestType::soap_default(NULL); }
	virtual ~ns__ImageSolveRequestType() { }
};
#endif

#ifndef SOAP_TYPE_ns__ImageSolveResponseType
#define SOAP_TYPE_ns__ImageSolveResponseType (14)
/* ns:ImageSolveResponseType */
class SOAP_CMAC ns__ImageSolveResponseType
{
public:
	ns__ResponseStatusType *ResponseStatus;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type ns:ResponseStatusType */
	ns__ImgDataHeaderType *ImgDataHeader;	/* required element of type ns:ImgDataHeaderType */
	std::vector<ns__ImgObjDetectedType * >vectImgObjDetected;	/* optional element of type ns:ImgObjDetectedType */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 14; } /* = unique id SOAP_TYPE_ns__ImageSolveResponseType */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns__ImageSolveResponseType(): ResponseStatus(NULL), ImgDataHeader(NULL), soap(NULL) { ns__ImageSolveResponseType::soap_default(NULL); }
	virtual ~ns__ImageSolveResponseType() { }
};
#endif

#ifndef SOAP_TYPE_ns__ScopePositionSetRequestType
#define SOAP_TYPE_ns__ScopePositionSetRequestType (15)
/* ns:ScopePositionSetRequestType */
class SOAP_CMAC ns__ScopePositionSetRequestType
{
public:
	std::string ScopeId;	/* required element of type xsd:integer */
	double RA;	/* required element of type xsd:double */
	double DEC;	/* required element of type xsd:double */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 15; } /* = unique id SOAP_TYPE_ns__ScopePositionSetRequestType */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns__ScopePositionSetRequestType(): soap(NULL) { ns__ScopePositionSetRequestType::soap_default(NULL); }
	virtual ~ns__ScopePositionSetRequestType() { }
};
#endif

#ifndef SOAP_TYPE_ns__ScopePositionSetResponseType
#define SOAP_TYPE_ns__ScopePositionSetResponseType (16)
/* ns:ScopePositionSetResponseType */
class SOAP_CMAC ns__ScopePositionSetResponseType
{
public:
	ns__ResponseStatusType *ResponseStatus;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type ns:ResponseStatusType */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 16; } /* = unique id SOAP_TYPE_ns__ScopePositionSetResponseType */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns__ScopePositionSetResponseType(): ResponseStatus(NULL), soap(NULL) { ns__ScopePositionSetResponseType::soap_default(NULL); }
	virtual ~ns__ScopePositionSetResponseType() { }
};
#endif

#ifndef SOAP_TYPE_ns__ScopePositionGetRequestType
#define SOAP_TYPE_ns__ScopePositionGetRequestType (17)
/* ns:ScopePositionGetRequestType */
class SOAP_CMAC ns__ScopePositionGetRequestType
{
public:
	std::string ScopeId;	/* required element of type xsd:integer */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 17; } /* = unique id SOAP_TYPE_ns__ScopePositionGetRequestType */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns__ScopePositionGetRequestType(): soap(NULL) { ns__ScopePositionGetRequestType::soap_default(NULL); }
	virtual ~ns__ScopePositionGetRequestType() { }
};
#endif

#ifndef SOAP_TYPE_ns__ScopePositionGetResponseType
#define SOAP_TYPE_ns__ScopePositionGetResponseType (18)
/* ns:ScopePositionGetResponseType */
class SOAP_CMAC ns__ScopePositionGetResponseType
{
public:
	ns__ResponseStatusType *ResponseStatus;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type ns:ResponseStatusType */
	double RA;	/* required element of type xsd:double */
	double DEC;	/* required element of type xsd:double */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 18; } /* = unique id SOAP_TYPE_ns__ScopePositionGetResponseType */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns__ScopePositionGetResponseType(): ResponseStatus(NULL), soap(NULL) { ns__ScopePositionGetResponseType::soap_default(NULL); }
	virtual ~ns__ScopePositionGetResponseType() { }
};
#endif

#ifndef SOAP_TYPE_ns__CameraImageTakeRequestType
#define SOAP_TYPE_ns__CameraImageTakeRequestType (19)
/* ns:CameraImageTakeRequestType */
class SOAP_CMAC ns__CameraImageTakeRequestType
{
public:
	std::string ExposureTime;	/* required element of type xsd:integer */
	std::string NoOfExposures;	/* required element of type xsd:integer */
	std::string ExposuresDelay;	/* required element of type xsd:integer */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 19; } /* = unique id SOAP_TYPE_ns__CameraImageTakeRequestType */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns__CameraImageTakeRequestType(): soap(NULL) { ns__CameraImageTakeRequestType::soap_default(NULL); }
	virtual ~ns__CameraImageTakeRequestType() { }
};
#endif

#ifndef SOAP_TYPE_ns__CameraImageTakeResponseType
#define SOAP_TYPE_ns__CameraImageTakeResponseType (20)
/* ns:CameraImageTakeResponseType */
class SOAP_CMAC ns__CameraImageTakeResponseType
{
public:
	ns__ResponseStatusType *ResponseStatus;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type ns:ResponseStatusType */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 20; } /* = unique id SOAP_TYPE_ns__CameraImageTakeResponseType */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns__CameraImageTakeResponseType(): ResponseStatus(NULL), soap(NULL) { ns__CameraImageTakeResponseType::soap_default(NULL); }
	virtual ~ns__CameraImageTakeResponseType() { }
};
#endif

#ifndef SOAP_TYPE_ns__ScopeFocusAdjustRequestType
#define SOAP_TYPE_ns__ScopeFocusAdjustRequestType (21)
/* ns:ScopeFocusAdjustRequestType */
class SOAP_CMAC ns__ScopeFocusAdjustRequestType
{
public:
	std::string Position;	/* required element of type xsd:string */
	std::string Speed;	/* required element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 21; } /* = unique id SOAP_TYPE_ns__ScopeFocusAdjustRequestType */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns__ScopeFocusAdjustRequestType(): soap(NULL) { ns__ScopeFocusAdjustRequestType::soap_default(NULL); }
	virtual ~ns__ScopeFocusAdjustRequestType() { }
};
#endif

#ifndef SOAP_TYPE_ns__ScopeFocusAdjustResponseType
#define SOAP_TYPE_ns__ScopeFocusAdjustResponseType (22)
/* ns:ScopeFocusAdjustResponseType */
class SOAP_CMAC ns__ScopeFocusAdjustResponseType
{
public:
	ns__ResponseStatusType *ResponseStatus;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type ns:ResponseStatusType */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 22; } /* = unique id SOAP_TYPE_ns__ScopeFocusAdjustResponseType */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns__ScopeFocusAdjustResponseType(): ResponseStatus(NULL), soap(NULL) { ns__ScopeFocusAdjustResponseType::soap_default(NULL); }
	virtual ~ns__ScopeFocusAdjustResponseType() { }
};
#endif

#ifndef SOAP_TYPE___ns__ImageSolve
#define SOAP_TYPE___ns__ImageSolve (34)
/* Operation wrapper: */
struct __ns__ImageSolve
{
public:
	ns__ImageSolveRequestType *ns__ImageSolveRequest;	/* optional element of type ns:ImageSolveRequestType */
};
#endif

#ifndef SOAP_TYPE___ns__ScopePositionSet
#define SOAP_TYPE___ns__ScopePositionSet (38)
/* Operation wrapper: */
struct __ns__ScopePositionSet
{
public:
	ns__ScopePositionSetRequestType *input;	/* optional element of type ns:ScopePositionSetRequestType */
};
#endif

#ifndef SOAP_TYPE___ns__ScopePositionGet
#define SOAP_TYPE___ns__ScopePositionGet (42)
/* Operation wrapper: */
struct __ns__ScopePositionGet
{
public:
	ns__ScopePositionGetRequestType *input;	/* optional element of type ns:ScopePositionGetRequestType */
};
#endif

#ifndef SOAP_TYPE___ns__CameraImageTake
#define SOAP_TYPE___ns__CameraImageTake (46)
/* Operation wrapper: */
struct __ns__CameraImageTake
{
public:
	ns__CameraImageTakeRequestType *input;	/* optional element of type ns:CameraImageTakeRequestType */
};
#endif

#ifndef SOAP_TYPE___ns__ScopeFocusAdjust
#define SOAP_TYPE___ns__ScopeFocusAdjust (50)
/* Operation wrapper: */
struct __ns__ScopeFocusAdjust
{
public:
	ns__ScopeFocusAdjustRequestType *input;	/* optional element of type ns:ScopeFocusAdjustRequestType */
};
#endif

#ifndef SOAP_TYPE_SOAP_ENV__Header
#define SOAP_TYPE_SOAP_ENV__Header (51)
/* SOAP Header: */
struct SOAP_ENV__Header
{
#ifdef WITH_NOEMPTYSTRUCT
private:
	char dummy;	/* dummy member to enable compilation */
#endif
};
#endif

#ifndef SOAP_TYPE_SOAP_ENV__Code
#define SOAP_TYPE_SOAP_ENV__Code (52)
/* SOAP Fault Code: */
struct SOAP_ENV__Code
{
public:
	char *SOAP_ENV__Value;	/* optional element of type xsd:QName */
	struct SOAP_ENV__Code *SOAP_ENV__Subcode;	/* optional element of type SOAP-ENV:Code */
};
#endif

#ifndef SOAP_TYPE_SOAP_ENV__Detail
#define SOAP_TYPE_SOAP_ENV__Detail (54)
/* SOAP-ENV:Detail */
struct SOAP_ENV__Detail
{
public:
	int __type;	/* any type of element <fault> (defined below) */
	void *fault;	/* transient */
	char *__any;
};
#endif

#ifndef SOAP_TYPE_SOAP_ENV__Reason
#define SOAP_TYPE_SOAP_ENV__Reason (57)
/* SOAP-ENV:Reason */
struct SOAP_ENV__Reason
{
public:
	char *SOAP_ENV__Text;	/* optional element of type xsd:string */
};
#endif

#ifndef SOAP_TYPE_SOAP_ENV__Fault
#define SOAP_TYPE_SOAP_ENV__Fault (58)
/* SOAP Fault: */
struct SOAP_ENV__Fault
{
public:
	char *faultcode;	/* optional element of type xsd:QName */
	char *faultstring;	/* optional element of type xsd:string */
	char *faultactor;	/* optional element of type xsd:string */
	struct SOAP_ENV__Detail *detail;	/* optional element of type SOAP-ENV:Detail */
	struct SOAP_ENV__Code *SOAP_ENV__Code;	/* optional element of type SOAP-ENV:Code */
	struct SOAP_ENV__Reason *SOAP_ENV__Reason;	/* optional element of type SOAP-ENV:Reason */
	char *SOAP_ENV__Node;	/* optional element of type xsd:string */
	char *SOAP_ENV__Role;	/* optional element of type xsd:string */
	struct SOAP_ENV__Detail *SOAP_ENV__Detail;	/* optional element of type SOAP-ENV:Detail */
};
#endif

/******************************************************************************\
 *                                                                            *
 * Typedefs                                                                   *
 *                                                                            *
\******************************************************************************/

#ifndef SOAP_TYPE__QName
#define SOAP_TYPE__QName (5)
typedef char *_QName;
#endif

#ifndef SOAP_TYPE__XML
#define SOAP_TYPE__XML (6)
typedef char *_XML;
#endif

#ifndef SOAP_TYPE_xsd__integer
#define SOAP_TYPE_xsd__integer (9)
typedef std::string xsd__integer;
#endif


/******************************************************************************\
 *                                                                            *
 * Externals                                                                  *
 *                                                                            *
\******************************************************************************/


#endif

/* End of soapStub.h */
