/*
 *	http.h
 *	wxCURL
 *
 *	Created by Casey O'Donnell on Tue Jun 29 2004.
 *	Copyright (c) 2004 Casey O'Donnell. All rights reserved.
 *
 *	Licence: wxWidgets Licence
 */

#ifndef _WXCURLHTTP_H__INCLUDED_
#define _WXCURLHTTP_H__INCLUDED_

// system includes
#include <vector>

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "curl/http.h"
#endif

#include "wxcurl_base.h"

// C Function Declarations for LibCURL
//
//////////////////////////////////////////////////////////////////////

extern "C"
{
}

// struct :: form field types
typedef struct 
{
	wxString field_name;
	wxString field_value;
	int field_type;

} DefFormField;
// init methods
static DefFormField _MakewxCurlFormField( const char* name, const char* value, int type ){ DefFormField f; wxString strWxName(name,wxConvUTF8); f.field_name=strWxName; wxString strWxValue(value,wxConvUTF8); f.field_value=strWxValue; f.field_type=type; return(f); }
static DefFormField _MakewxCurlFormField( const char* name, long value, int type ){ DefFormField f; wxString strWxName(name,wxConvUTF8); f.field_name=strWxName; f.field_value.sprintf(wxT("%d"),value); f.field_type=type; return(f); }
static DefFormField _MakewxCurlFormField( const char* name, double value, int type ){ DefFormField f; wxString strWxName(name,wxConvUTF8); f.field_name=strWxName; f.field_value.sprintf(wxT("%lf"),value); f.field_type=type; return(f); }

// http.h: interface for the wxCurlHTTP class.
//
//////////////////////////////////////////////////////////////////////

class WXDLLIMPEXP_CURL wxCurlHTTP : public wxCurlBase
{
public:
	wxCurlHTTP(const wxString& szURL = wxEmptyString, const wxString& szUserName = wxEmptyString, const wxString& szPassword = wxEmptyString, wxEvtHandler* pEvtHandler = NULL, const bool& bSendUpdateEvents = false, const bool& bSendBeginEndEvents = false);
	virtual ~wxCurlHTTP();

	// Post Data Methods
	bool	AddForm(const bool& bClear, const wxString& szName, struct curl_forms* pForms);

	// Member Data Access Methods (MDA)

	// MDA - Get/Set Cookie Use
	void		UseCookies(const bool& bUseCookies);
	bool		UseCookies() const;

	// MDA - Get/Set Cookie File
	void		SetCookieFile(const wxString& szFilePath);
	wxString	GetCookieFile() const;

	// Action Methods - These All Make Calls To: curl_easy_perform()

	// OPTIONS - This Method Allows a Caller to Query the Capabilities of a Resource
	bool		Options(const wxString& szRemoteFile = wxEmptyString);

	// HEAD - This Method is Similar to GET, Though no Message Body is Retrieved
	bool		Head(const wxString& szRemoteFile = wxEmptyString);

	// POST - These Methods Post Data Passed In
	bool		Post(const char* buffer, size_t size, const wxString& szRemoteFile = wxEmptyString);
	bool		Post(wxInputStream& buffer, const wxString& szRemoteFile = wxEmptyString);

	// POST - FORM + FILE(UPLOAD)
	bool		PostDF( std::vector<DefFormField>& vectField, const wxString& szRemoteFile = wxEmptyString);

	// POST - This Method Uses Data As Defined By AddForm(...)
	bool		Post(const wxString& szRemoteFile = wxEmptyString);

	// TRACE - This Method Allows a Caller to Perform Diagnostic Tests
	bool		Trace(const wxString& szRemoteFile = wxEmptyString);

	// GET - These Methods Allow a Caller to Retrieve a Given Resource
	bool		Get(const wxString& szFilePath, const wxString& szRemoteFile = wxEmptyString);
	size_t		Get(char*& buffer, const wxString& szRemoteFile = wxEmptyString);
	bool		Get(wxOutputStream& buffer, const wxString& szRemoteFile = wxEmptyString);

	// PUT - These Methods Allow a Caller to Place a Resource on the Host
	bool		Put(const wxString& szFilePath, const wxString& szRemoteFile = wxEmptyString);
	bool		Put(const char* buffer, size_t size, const wxString& szRemoteFile = wxEmptyString);
	bool		Put(wxInputStream& buffer, const wxString& szRemoteFile = wxEmptyString);

	// DELETE - This Method Allows a Caller to Remove a Resource
	bool		Delete(const wxString& szRemoteLoc = wxEmptyString);

protected:

	virtual void ResetPostData();
	virtual void SetCurlHandleToDefaults();

	struct curl_httppost*	m_pPostHead;
	struct curl_httppost*	m_pPostTail;

	bool					m_bUseCookies;

	wxString				m_szCookieFile;

	const char*				m_pszPostFieldsData;
	size_t					m_iPostDataSize;

private:

};

#endif // _WXCURLHTTP_H__INCLUDED_
