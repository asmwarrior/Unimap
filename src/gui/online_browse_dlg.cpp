////////////////////////////////////////////////////////////////////
// Created by:	Larry Lart 
////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include <wx/image.h>
#include <wx/filedlg.h>
#include <wx/datetime.h>

// local headers
#include "../util/folders.h"
#include "../online/unimap_online.h"
#include "../unimap_worker.h"
#include "../image/imagedb.h"
#include "../image/imagegroup.h"
#include "simple_common_dlg.h"
#include "img_view_dlg.h"
#include "online_comments_dlg.h"
#include "waitdialog.h"

// main header
#include "online_browse_dlg.h"

//		***** CLASS COnlineBrowseDlg *****
////////////////////////////////////////////////////////////////////
// COnlineBrowseDlg EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(COnlineBrowseDlg, wxDialog)
	// start remote list fetch
	EVT_BUTTON( wxID_ONLINE_BROWSE_GETLIST, COnlineBrowseDlg::OnGetImageList )
	// list sort
	EVT_LIST_COL_CLICK( wxID_ONLINE_BROWSE_IMAGE_LIST, COnlineBrowseDlg::OnListColumn )
	// event list fetched from online done
//	EVT_COMMAND( wxID_ONLINE_EVENT_LIST_GET_DONE, wxEVT_CUSTOM_MESSAGE_EVENT, COnlineBrowseDlg::OnListFetchDone )
	EVT_LIST_ITEM_SELECTED( wxID_ONLINE_BROWSE_IMAGE_LIST, COnlineBrowseDlg::OnSelectImage ) 
//	EVT_COMMAND( wxID_ONLINE_EVENT_PREVIEW_GET_DONE, wxEVT_CUSTOM_MESSAGE_EVENT, COnlineBrowseDlg::OnPreviewGetDone )
	// download action
	EVT_BUTTON( wxID_ONLINE_BROWSE_ACT_DOWNLOAD, COnlineBrowseDlg::OnActDownload )
//	EVT_COMMAND( wxID_ONLINE_EVENT_DOWNLOAD_IMAGE_DATA_DONE, wxEVT_CUSTOM_MESSAGE_EVENT, COnlineBrowseDlg::OnDownloadImageDataDone )
	// view image action
	EVT_BUTTON( wxID_ONLINE_BROWSE_ACT_VIEW_FULL, COnlineBrowseDlg::OnActViewFull )
	// when select destitation type
	EVT_CHOICE( wxID_ONLINE_BROWSE_DEST_TYPE, COnlineBrowseDlg::OnSelectDestType )
	EVT_BUTTON( wxID_ONLINE_BROWSE_DEST_NEW_GROUP_BUTTON, COnlineBrowseDlg::OnNewDestGroup )
	EVT_BUTTON( wxID_ONLINE_BROWSE_DEST_FOLDER_BUTTON, COnlineBrowseDlg::OnSetDestFolder )
	// rate image
	EVT_BUTTON( wxID_ONLINE_BROWSE_SCORE_RATE_IT, COnlineBrowseDlg::OnRateImage )
	// comments
	EVT_BUTTON( wxID_ONLINE_BROWSE_ACT_COMMENTS, COnlineBrowseDlg::OnActComments )
	// delete images
	EVT_BUTTON( wxID_ONLINE_BROWSE_ACT_DELETE, COnlineBrowseDlg::OnActDeleteImage )
END_EVENT_TABLE()
////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	COnlineBrowseDlg
// Purpose:	Initialize add devicedialog
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
COnlineBrowseDlg::COnlineBrowseDlg( wxWindow *parent, const wxString& title ) : wxDialog(parent, -1,
                          title,
                          wxDefaultPosition,
                          wxDefaultSize,
                          wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL)
{
	m_bNewImageToGroup = 0;
	m_nParentGroupId = -1;
	m_nSelectedImage = -1;
	m_nSortField = 6;
	// default sort orver flas for list fields
	m_vectSortField[LIST_COLUMN_ID_USER]=0;
	m_vectSortField[LIST_COLUMN_ID_GROUP]=0;
	m_vectSortField[LIST_COLUMN_ID_IMAGE]=0;
	m_vectSortField[LIST_COLUMN_ID_SCORE_Q]=1;
	m_vectSortField[LIST_COLUMN_ID_SCORE_A]=1;
	m_vectSortField[LIST_COLUMN_ID_SCORE_S]=1;
	m_vectSortField[LIST_COLUMN_ID_DATETIME]=1;

	wxString vectScoreSelect[6];
	vectScoreSelect[0] = wxT("0");
	vectScoreSelect[1] = wxT("1");
	vectScoreSelect[2] = wxT("2");
	vectScoreSelect[3] = wxT("3");
	vectScoreSelect[4] = wxT("4");
	vectScoreSelect[5] = wxT("5");
	wxString vectListSource[2];
	vectListSource[0] = wxT("Any");
	vectListSource[1] = wxT("Current");
	wxString vectDestType[2];
	vectDestType[0] = wxT("Group");
	vectDestType[1] = wxT("Folder");

	wxFlexGridSizer *topsizer = new wxFlexGridSizer( 2,1,0,0 );
	topsizer->AddGrowableRow( 1 );

	/////////////////////////////
	// :: main sizer
	wxFlexGridSizer* sizerMain = new wxFlexGridSizer( 1,2,0,0 );
	sizerMain->AddGrowableCol( 1 );
	// :: list sizer
	wxFlexGridSizer* sizerListGet = new wxFlexGridSizer( 2,1,0,0 );
	sizerListGet->AddGrowableRow( 1 );
	// list action header
	wxStaticBox* pListHeadBox = new wxStaticBox( this, -1, wxT("Filter"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* pListBoxSizer = new wxStaticBoxSizer( pListHeadBox, wxVERTICAL );
	wxFlexGridSizer* sizerListGetHead = new wxFlexGridSizer( 1,7,0,3 );
	sizerListGetHead->AddGrowableCol( 0 );
	sizerListGetHead->AddGrowableCol( 2 );
	sizerListGetHead->AddGrowableCol( 4 );
	sizerListGetHead->AddGrowableCol( 6 );
	pListBoxSizer->Add( sizerListGetHead, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND );
	sizerListGet->Add( pListBoxSizer, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND|wxBOTTOM, 6 );
	// :: image action sizer
	wxFlexGridSizer* sizerImageAction = new wxFlexGridSizer( 2,1,0,0 );
	sizerImageAction->AddGrowableRow( 0 );
	sizerImageAction->AddGrowableRow( 1 );
	// box - action 
	wxStaticBox* pActionBox = new wxStaticBox( this, -1, wxT("Action"), wxPoint(-1,-1), wxDefaultSize );
	wxStaticBoxSizer* pActionBoxSizer = new wxStaticBoxSizer( pActionBox, wxVERTICAL );
	wxFlexGridSizer* sizerAction = new wxFlexGridSizer( 2, 1, 5, 5 );
	sizerAction->AddGrowableCol( 1 );
	// action head
	wxFlexGridSizer* sizerActionHead = new wxFlexGridSizer( 1, 4, 5, 5 );
	// action body
	wxFlexGridSizer* sizerActionBody = new wxFlexGridSizer( 3, 2, 5, 5 );
	sizerAction->Add( sizerActionHead, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	sizerAction->Add( sizerActionBody, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	pActionBoxSizer->Add( sizerAction, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	// add to main
	sizerMain->Add( sizerListGet, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxEXPAND|wxRIGHT, 10  );
	sizerMain->Add( sizerImageAction, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxEXPAND  );

	// fetch button
	m_pListMaxNo = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize(40,-1) );
	m_pListSource = new wxChoice( this, wxID_ONLINE_BROWSE_LIST_SOURCE,
										wxDefaultPosition, wxSize(-1,-1), 2, vectListSource,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pListSource->SetSelection( 0 );
	m_pListKey = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize(100,-1) );
	m_pGetListButton = new wxButton( this, wxID_ONLINE_BROWSE_GETLIST, wxT("Get"), wxPoint(-1, -1), wxSize(60,-1) );

	// list of images
	m_pImagesList = new wxListCtrl( this, wxID_ONLINE_BROWSE_IMAGE_LIST, wxPoint(-1,-1), wxSize(417,-1), wxLC_REPORT|wxLC_HRULES|wxLC_VRULES );

	/////
	// action controls
	m_pDownloadButton = new wxButton( this, wxID_ONLINE_BROWSE_ACT_DOWNLOAD, wxT("Download"), wxPoint(-1, -1), wxSize(60,-1) );
	m_pViewFullButton = new wxButton( this, wxID_ONLINE_BROWSE_ACT_VIEW_FULL, wxT("View Full"), wxPoint(-1, -1), wxSize(60,-1) );
	m_pAddCommentButton = new wxButton( this, wxID_ONLINE_BROWSE_ACT_COMMENTS, wxT("Comments"), wxPoint(-1, -1), wxSize(-1,-1) );
	m_pDeleteButton = new wxButton( this, wxID_ONLINE_BROWSE_ACT_DELETE, wxT("Delete"), wxPoint(-1, -1), wxSize(-1,-1) );
	m_pDeleteButton->Disable();
	// destination folder
	m_pDestType = new wxChoice( this, wxID_ONLINE_BROWSE_DEST_TYPE,
										wxDefaultPosition, wxSize(-1,-1), 2, vectDestType,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pDestType->SetSelection( 0 );
	// group widgets
	m_pDestGroup = new wxChoice( this, wxID_ONLINE_BROWSE_DEST_SELECT_GROUP,
										wxDefaultPosition, wxSize(110,-1), 0, NULL,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pDestGroup->SetSelection( 0 );
	m_pDestNewGroupButton = new wxButton( this, wxID_ONLINE_BROWSE_DEST_NEW_GROUP_BUTTON, wxT("New"), wxPoint(-1, -1), wxSize(35,-1) );
	// folder widgets
	m_pDestFolderEntry = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize(120,-1) );
	m_pDestFolderButton = new wxButton( this, wxID_ONLINE_BROWSE_DEST_FOLDER_BUTTON, wxT("Set"), wxPoint(-1, -1), wxSize(35,-1) );
	// hide folder options by default
	m_pDestFolderEntry->Hide();
	m_pDestFolderButton->Hide();

	// score/rate it
	m_pScoreQ = new wxChoice( this, wxID_ONLINE_BROWSE_SCORE_Q_SELECT,
										wxDefaultPosition, wxSize(-1,-1), 6, vectScoreSelect,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pScoreQ->SetSelection( 0 );
	m_pScoreA = new wxChoice( this, wxID_ONLINE_BROWSE_SCORE_A_SELECT,
										wxDefaultPosition, wxSize(-1,-1), 6, vectScoreSelect,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pScoreA->SetSelection( 0 );
	m_pScoreS = new wxChoice( this, wxID_ONLINE_BROWSE_SCORE_S_SELECT,
										wxDefaultPosition, wxSize(-1,-1), 6, vectScoreSelect,
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pScoreS->SetSelection( 0 );
	m_pRateitButton = new wxButton( this, wxID_ONLINE_BROWSE_SCORE_RATE_IT, wxT("Rate it!"), wxPoint(-1, -1), wxSize(60,-1) );

	// image preview
	wxBitmap bmpPreview( UNIMAP_OLNINE_PREVIEW_IMAGE_WIDTH, UNIMAP_OLNINE_PREVIEW_IMAGE_HEIGHT ); //( "./resources/nophoto_s.gif", wxBITMAP_TYPE_ANY );
	m_pImgPreview = new wxStaticBitmap( this, -1, bmpPreview, wxDefaultPosition, wxSize( UNIMAP_OLNINE_PREVIEW_IMAGE_WIDTH, UNIMAP_OLNINE_PREVIEW_IMAGE_HEIGHT ) );
	

	///////////////////////////////////
	// list get header
	// :: max no of images
	sizerListGetHead->Add( new wxStaticText( this, -1, wxT("Max:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerListGetHead->Add( m_pListMaxNo, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: source
	sizerListGetHead->Add( new wxStaticText( this, -1, wxT("Source:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerListGetHead->Add( m_pListSource, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: key
	sizerListGetHead->Add( new wxStaticText( this, -1, wxT("Key:")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerListGetHead->Add( m_pListKey, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// :: get button
	sizerListGetHead->Add( m_pGetListButton, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	// list get
	sizerListGet->Add( m_pImagesList, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND );

	///////////////////////////////////
	// Image
	sizerImageAction->Add( m_pImgPreview, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	// action
	sizerImageAction->Add( pActionBoxSizer, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL|wxEXPAND );
	// add controls to action
	sizerActionHead->Add( m_pDownloadButton, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	sizerActionHead->Add( m_pViewFullButton, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	sizerActionHead->Add( m_pAddCommentButton, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	sizerActionHead->Add( m_pDeleteButton, 1, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	// destination folder
	sizerActionBody->Add( new wxStaticText( this, -1, wxT("Destination:")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	wxFlexGridSizer* sizerActionDest = new wxFlexGridSizer( 1,5,0,0 );
	sizerActionDest->Add( m_pDestType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxRIGHT, 10 );
	sizerActionDest->Add( m_pDestGroup, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxRIGHT, 10 );
	sizerActionDest->Add( m_pDestNewGroupButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerActionDest->Add( m_pDestFolderEntry, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxRIGHT, 10 );
	sizerActionDest->Add( m_pDestFolderButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerActionBody->Add( sizerActionDest, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	// rate/score
	sizerActionBody->Add( new wxStaticText( this, -1, wxT("Score(Q/A/S):")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	wxFlexGridSizer* sizerActionScore = new wxFlexGridSizer( 1,4,0,0 );
	sizerActionScore->Add( m_pScoreQ, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxRIGHT, 10 );
	sizerActionScore->Add( m_pScoreA, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxRIGHT, 10 );
	sizerActionScore->Add( m_pScoreS, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxRIGHT, 10 );
	sizerActionScore->Add( m_pRateitButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	sizerActionBody->Add( sizerActionScore, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	// add to main sizer
	topsizer->Add( sizerMain, 1, wxGROW | wxALL, 10 );

	// set table
//	topsizer->Add( m_pCatalogTable, 1, wxGROW | wxALL, 10 );

//	topsizer->Add( CreateButtonSizer(wxOK | wxCANCEL), 0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 10 );

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);
//	sizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	COnlineBrowseDlg
// Purpose:	Delete my object
// Input:	mothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
COnlineBrowseDlg::~COnlineBrowseDlg( )
{
	delete( m_pListMaxNo );
	delete( m_pListSource );
	delete( m_pListKey );
	delete( m_pGetListButton );
	delete( m_pImagesList );
	delete( m_pImgPreview );
	// action
	delete( m_pDownloadButton );
	delete( m_pViewFullButton );
	delete( m_pAddCommentButton );
	delete( m_pDeleteButton );
	delete( m_pDestType );
	delete( m_pDestGroup );
	delete( m_pDestFolderEntry );
	delete( m_pDestFolderButton );
	delete( m_pDestNewGroupButton );
	delete( m_pScoreQ );
	delete( m_pScoreA );
	delete( m_pScoreS );
	delete( m_pRateitButton );
}

////////////////////////////////////////////////////////////////////
// Method:		SetConfig
// Class:		COnlineBrowseDlg
// Purpose:		set local configuration
// Input:		pointer to unimap online object and worker
// Output:		nothing
////////////////////////////////////////////////////////////////////
void COnlineBrowseDlg::SetConfig( CUnimapOnline* pUnimapOnline, CUnimapWorker* pUnimapWorker,
									CImageDb* pImageDb, int nSelectGroup )
{
	int i=0;

	m_pUnimapOnline = pUnimapOnline;
	m_pUnimapWorker = pUnimapWorker;
	m_pImageDb = pImageDb;
	m_nParentGroupId = nSelectGroup;

	// set filter
	m_pListMaxNo->SetValue( wxT("500") );

	// populate image 
	for( i=0; i<pImageDb->m_nImageGroup; i++ )
	{
		m_pDestGroup->Append( wxString(pImageDb->m_vectImageGroup[i]->m_strGroupNameId,wxConvUTF8) );
	}
	m_pDestGroup->SetSelection(nSelectGroup);

	// do call to get image list
	GetImageList( );

	Refresh( FALSE );
	Update( );
}

////////////////////////////////////////////////////////////////////
// Method:		OnGetImageList
////////////////////////////////////////////////////////////////////
void COnlineBrowseDlg::OnGetImageList( wxCommandEvent& pEvent )
{
	// check filter source
	GetImageList( );

	return;
}

// Method:	GetImageList
////////////////////////////////////////////////////////////////////
void COnlineBrowseDlg::GetImageList( )
{
//	m_pImagesList->ClearAll();

	// check filter source
	int nSource = m_pListSource->GetSelection();

	DefCmdAction cmd;
	cmd.id = THREAD_ACTION_BROWSE_ONLINE_IMAGES;
	// this out not to be overwritten by waitdialog
	//cmd.handler = this->GetEventHandler();

	// set max number
	cmd.vectInt[0] = wxAtoi( m_pListMaxNo->GetLineText(0) );
	// set sort by fields
	cmd.vectInt[1] = m_nSortField;
	cmd.vectInt[2] = m_vectSortField[m_nSortField];
	// all other ???
/*	cmd.vectNumber[2] = (double) m_bSortUser;
	cmd.vectNumber[3] = (double) m_bSortGroup;
	cmd.vectNumber[4] = (double) m_bSortImage;
	cmd.vectNumber[5] = (double) m_bSortScoreQ;
	cmd.vectNumber[6] = (double) m_bSortScoreA;
	cmd.vectNumber[7] = (double) m_bSortScoreS;
	cmd.vectNumber[8] = (double) m_bSortDateTime;
*/
	// set source
	cmd.vectInt[9] = nSource;
	if( nSource == 1 )
	{
		// todo: implement source extract image center ra/dec
		//cmd.vectNumber[10] = (double) pImageDb->current image.ra;
		//cmd.vectNumber[11] = (double) pImageDb->current image.dec;
	}
	// set key
	cmd.vectStr[0] = m_pListKey->GetLineText(0);

//	m_pUnimapWorker->SetAction( cmd );

	/////////////////
	// show wait dialog
	CWaitDialog* pWaitDialog = new CWaitDialog( this, wxT("Fetch Image List"), 
						wxT("Retreive File List ..."), 1, m_pUnimapWorker, &cmd );
	pWaitDialog->ShowModal();
	// delete/reset wait dialog
	delete( pWaitDialog );

	// call to set image list
	SetImageList( );
}

////////////////////////////////////////////////////////////////////
void COnlineBrowseDlg::SetImageList( )
{
	int nListSize = m_pUnimapOnline->m_vectRemoteFileList.size();
	if( nListSize <= 0 ) return;
	int i=0;
	wxString strTmp;

	/////////////////////
	// set image list
	m_pImagesList->ClearAll();
	// add columns
	m_pImagesList->InsertColumn( 1, wxT("User"), wxLIST_FORMAT_LEFT, 60 );
	m_pImagesList->InsertColumn( 2, wxT("Group"), wxLIST_FORMAT_LEFT, 60 );
	m_pImagesList->InsertColumn( 2, wxT("Image"), wxLIST_FORMAT_LEFT, 100 );
	m_pImagesList->InsertColumn( 3, wxT("Q"), wxLIST_FORMAT_LEFT, 20 );
	m_pImagesList->InsertColumn( 4, wxT("A"), wxLIST_FORMAT_LEFT, 20 );
	m_pImagesList->InsertColumn( 5, wxT("S"), wxLIST_FORMAT_LEFT, 20 );
	m_pImagesList->InsertColumn( 6, wxT("Date/Time"), wxLIST_FORMAT_LEFT, 116 );

	// for every image in the list
	for( i=0; i<nListSize; i++ )
	{
		long nIndex = m_pImagesList->InsertItem( 55551+i, wxT("") );
		// here we check and set color based on if it was loaded or not
		//m_pImagesList->SetItemTextColour( nIndex, wxColour( 0, 255, 0 ) );

		// set user
		m_pImagesList->SetItem( nIndex, 0, m_pUnimapOnline->m_vectRemoteFileList[i].strUser );
		// set group
		m_pImagesList->SetItem( nIndex, 1, m_pUnimapOnline->m_vectRemoteFileList[i].strGroup );
		// set name
		m_pImagesList->SetItem( nIndex, 2, m_pUnimapOnline->m_vectRemoteFileList[i].strImage );

		// set quality score
		SetScoreValues( nIndex, i );

		// set date time
		wxDateTime myDateTime( m_pUnimapOnline->m_vectRemoteFileList[i].datetime );	
		m_pImagesList->SetItem( nIndex, 6, myDateTime.Format( wxT("%d/%m/%Y %H:%M:%S") ) );
	}

	// set selection to first 
	m_pImagesList->SetItemState( 0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
	
	Update();
	Refresh( false );

	return;
}

////////////////////////////////////////////////////////////////////
void COnlineBrowseDlg::SetScoreValues( int nIndex, int nId )
{
	wxString strTmp;

	strTmp.Printf( wxT("%d"), m_pUnimapOnline->m_vectRemoteFileList[nId].score_q );
	m_pImagesList->SetItem( nIndex, 3, strTmp );

	// set artistic score
	strTmp.Printf( wxT("%d"), m_pUnimapOnline->m_vectRemoteFileList[nId].score_a );
	m_pImagesList->SetItem( nIndex, 4, strTmp );
	// set scientific score
	strTmp.Printf( wxT("%d"), m_pUnimapOnline->m_vectRemoteFileList[nId].score_s );
	m_pImagesList->SetItem( nIndex, 5, strTmp );
}

// Method:	OnSelectImage
////////////////////////////////////////////////////////////////////
void COnlineBrowseDlg::OnSelectImage( wxListEvent& pEvent )
{
	long nItem = pEvent.GetItem();
	long nImg = m_pUnimapOnline->m_vectRemoteFileList.size();
	if( nItem >= nImg ) return;
	
	wxString strFile;
	wxString strPath;
	wxString strTmp;

	// set current selection
	m_nSelectedImage = nItem;

	// build file name
	strFile.Printf( unMakeAppPath(wxT("data/temp/uonline/preview_icons/%s/%s")), 
				m_pUnimapOnline->m_vectRemoteFileList[nItem].strGroup,
				m_pUnimapOnline->m_vectRemoteFileList[nItem].strPreview );
	// set path to entry folder
	strPath.Printf( unMakeAppPath(wxT("data/temp/uonline/astro_images/%s")), m_pUnimapOnline->m_vectRemoteFileList[nItem].strGroup );
	m_pDestFolderEntry->SetValue( strPath );
	// now also try to do the select ... if there
	int nGroupId = m_pImageDb->FindGroupIdByName( m_pUnimapOnline->m_vectRemoteFileList[nItem].strGroup );
	if( nGroupId >= 0 ) m_pDestGroup->SetSelection( nGroupId );

	/////////////////////////
	// set score q
	int score_q = m_pUnimapOnline->m_vectRemoteFileList[nItem].score_q;
	if( score_q >= 0 && score_q <= 5 )
		m_pScoreQ->SetSelection( score_q );
	else
		m_pScoreQ->SetSelection( 0 );
	// set score a
	int score_a = m_pUnimapOnline->m_vectRemoteFileList[nItem].score_a;
	if( score_a >= 0 && score_a <= 5 )
		m_pScoreA->SetSelection( score_a );
	else
		m_pScoreA->SetSelection( 0 );
	// set score s
	int score_s = m_pUnimapOnline->m_vectRemoteFileList[nItem].score_s;
	if( score_s >= 0 && score_s <= 5 )
		m_pScoreS->SetSelection( score_s );
	else
		m_pScoreS->SetSelection( 0 );

	////////////////////
	// set delete button enable/disable
	if( m_pUnimapOnline->m_strUsername.CmpNoCase( m_pUnimapOnline->m_vectRemoteFileList[nItem].strUser ) == 0 )
		m_pDeleteButton->Enable();
	else
		m_pDeleteButton->Disable();

	////////////////////
	// set button label for comments
	strTmp.Printf( wxT("Comments(%d)"), m_pUnimapOnline->m_vectRemoteFileList[nItem].nComments );
	m_pAddCommentButton->SetLabel( strTmp );

	///////////////////////
	// check if file already exists in the temp folder
	if( wxFileExists( strFile ) )
	{
		wxBitmap bmpPreview( strFile, wxBITMAP_TYPE_ANY );
		m_pImgPreview->SetBitmap( bmpPreview );

//		GetSizer()->Layout();
		Layout();
//		Fit();
//		Update( );
//		Refresh( false );

	} else
	{
		// send action to load
		DefCmdAction cmd;
		cmd.id = THREAD_ACTION_BROWSE_ONLINE_GET_IMAGE_PREVIEW;
		//cmd.handler = this->GetEventHandler();
		cmd.vectObj[0] = (void*) m_pUnimapOnline;
		cmd.vectStr[0] = m_pUnimapOnline->m_vectRemoteFileList[nItem].strUser;
		cmd.vectStr[1] = m_pUnimapOnline->m_vectRemoteFileList[nItem].strGroup;
		cmd.vectStr[2] = m_pUnimapOnline->m_vectRemoteFileList[nItem].strPreview;

		//m_pUnimapWorker->SetAction( cmd );

		/////////////////
		// show wait dialog
		CWaitDialog* pWaitDialog = new CWaitDialog( this, wxT("Get Image Preview"), 
							wxT("Loading Image Preview Icon ..."), 1, m_pUnimapWorker, &cmd );
		pWaitDialog->ShowModal();
		// delete/reset wait dialog
		delete( pWaitDialog );

		///////////
		// when done...
		long nItem = m_nSelectedImage;
		wxString strFile;

		strFile.Printf( unMakeAppPath(wxT("data/temp/uonline/preview_icons/%s/%s")), 
				m_pUnimapOnline->m_vectRemoteFileList[nItem].strGroup,
				m_pUnimapOnline->m_vectRemoteFileList[nItem].strPreview );
		// check if file already exists in the temp folder
		if( wxFileExists( strFile ) )
		{
			wxBitmap bmpPreview( strFile, wxBITMAP_TYPE_ANY );
			m_pImgPreview->SetBitmap( bmpPreview );

			GetSizer()->Layout();
			Layout();
			Fit();
			Update( );
			Refresh( false );

		}
	}

	return;
}


// Method:	OnSelectDestType
////////////////////////////////////////////////////////////////////
void COnlineBrowseDlg::OnSelectDestType( wxCommandEvent& pEvent )
{
	int nId = m_pDestType->GetSelection();

	// by type: 0=folder, 1=group
	if( nId == 1 )
	{
		m_pDestGroup->Hide();
		m_pDestNewGroupButton->Hide();
		m_pDestFolderEntry->Show();
		m_pDestFolderButton->Show();

	} else if( nId == 0 )
	{
		m_pDestFolderEntry->Hide();
		m_pDestFolderButton->Hide();
		m_pDestGroup->Show();
		m_pDestNewGroupButton->Show();
	}

	// update layout
//	GetSizer()->Layout();
	Layout();
//	Fit();
	Update( );
	Refresh( false );

	return;
}

// Method:	OnSetDestFolder
////////////////////////////////////////////////////////////////////
void COnlineBrowseDlg::OnSetDestFolder( wxCommandEvent& pEvent )
{
/***** this was used to set the full path with file chooser *******
	wxString strFilename = m_pDestFolderEntry->GetLineText(0);

	wxFileDialog fileDlg( this, wxT("Select Destinatin Path/File for Download..."),
							wxT(""), wxT(""), wxT("*.*"), 
							wxFD_SAVE|wxFD_OVERWRITE_PROMPT|wxFD_CHANGE_DIR );
	fileDlg.SetPath( strFilename );
	if( fileDlg.ShowModal() == wxID_OK )
	{
		wxString strPath = fileDlg.GetPath();
		m_pDestFolderEntry->SetValue( strPath );
	}
*/

	wxString strPath = m_pDestFolderEntry->GetLineText(0);
	wxDirDialog pathDlg( this, wxT("Select destinatin folder to download..."), strPath );

	if( pathDlg.ShowModal() == wxID_OK )
	{
		wxString strOPath = pathDlg.GetPath();
		m_pDestFolderEntry->SetValue( strOPath );
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnListColumn
////////////////////////////////////////////////////////////////////
void COnlineBrowseDlg::OnListColumn( wxListEvent& pEvent )
{
	int i = 0;
	// get column
	int nColId = pEvent.GetColumn();

	// set sort field
	m_nSortField = nColId;

	// now by column
	m_vectSortField[nColId] = !m_vectSortField[nColId];

	// call get list with the new vars
	GetImageList( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnNewDestGroup
////////////////////////////////////////////////////////////////////
void COnlineBrowseDlg::OnNewDestGroup( wxCommandEvent& pEvent )
{
	// todo: perhaps i should use custom group add dlg - with name, path and basic settings 
	// - also use in menu->group->new
	CSimpleEntryAddDlg *pDgl = new CSimpleEntryAddDlg( this, wxT("Add a new group"), wxT("Group Name:") );
	if ( pDgl->ShowModal( ) == wxID_OK )
	{
		// check if entry
		wxString strGroupName = pDgl->m_pName->GetLineText(0);
		if( strGroupName != wxT("") )
		{
			// todo: add group
			//CImageDb::AddGroup( const char* strGroupName, const char* strGroupPath, int bInclude );
		}
	}
	delete( pDgl );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnActDownload
////////////////////////////////////////////////////////////////////
void COnlineBrowseDlg::OnActDownload( wxCommandEvent& pEvent )
{
	wxString strPath = unMakeAppPath(wxT("data/temp/uonline/astro_images"));
	long nItem = m_nSelectedImage;
	int nPathType = m_pDestType->GetSelection();
	int nGroupId = -2;
	CImageGroup* pSelectedGroup = NULL;
	// process path by type: 0=group, 1=folder
	if( nPathType == 0 )
	{
		// extract path from group
		nGroupId = m_pDestGroup->GetSelection();
		pSelectedGroup = m_pImageDb->m_vectImageGroup[nGroupId];
		if( pSelectedGroup == NULL ) return; 
		// get path
		strPath = pSelectedGroup->m_strGroupPath;

	} else if( nPathType == 1 )
	{
		// get path from entry
		strPath = m_pDestFolderEntry->GetLineText(0);
	}
	// for now just overwrite all :) todo: test/think @ this latter
	strPath = unMakeAppPath(wxT("data/temp/uonline/astro_images"));

	// send action to load
	DefCmdAction cmd;
	cmd.id = THREAD_ACTION_BROWSE_ONLINE_DOWNLOAD_IMAGE;
	//cmd.handler = this->GetEventHandler();
	cmd.vectObj[0] = (void*) m_pUnimapOnline;
	cmd.vectStr[0] = strPath;
	cmd.vectStr[1] = m_pUnimapOnline->m_vectRemoteFileList[nItem].strUser;
	cmd.vectStr[2] = m_pUnimapOnline->m_vectRemoteFileList[nItem].strGroup;
	cmd.vectStr[3] = m_pUnimapOnline->m_vectRemoteFileList[nItem].strImage;
	cmd.vectStr[4] = m_pUnimapOnline->m_vectRemoteFileList[nItem].strIni;

//	m_pUnimapWorker->SetAction( cmd );

	/////////////////
	// show wait dialog
	CWaitDialog waitDialog( this, wxT("Wait..."), wxT("Downloading Image Data ..."), 1, m_pUnimapWorker, &cmd );
	waitDialog.ShowModal();

	//////////
	// when download done - if set to group, add to group
//    nItem = m_nSelectedImage;

	if( nPathType == 0 )
	{
		wxString strLocalFolder=wxT("");
		wxString strLocalFile=wxT("");
		strLocalFolder.Printf( wxT("%s/%s"), strPath, m_pUnimapOnline->m_vectRemoteFileList[nItem].strGroup );
		strLocalFile.Printf( wxT("%s/%s"), strLocalFolder, m_pUnimapOnline->m_vectRemoteFileList[nItem].strImage );
		int nImageId = pSelectedGroup->AddImage( strLocalFile );
		// set flag to update list of group images on close
		if( nGroupId == m_nParentGroupId ) m_bNewImageToGroup = 1;
	}

	// set item to green ...
	m_pImagesList->SetItemTextColour( nItem, wxColour( 0, 160, 40 ) );

	return;
}

// Method:	OnActViewFull
////////////////////////////////////////////////////////////////////
void COnlineBrowseDlg::OnActViewFull( wxCommandEvent& pEvent )
{
	wxString strImageFile;
	wxString strTitle;

	long nItem = m_nSelectedImage;
	strImageFile.Printf( unMakeAppPath(wxT("data/temp/uonline/astro_images/%s/%s")), 
				m_pUnimapOnline->m_vectRemoteFileList[nItem].strGroup,
				m_pUnimapOnline->m_vectRemoteFileList[nItem].strImage );

	if( wxFileExists( strImageFile ) )
	{
		strTitle.Printf( wxT("Image view :: %s"), m_pUnimapOnline->m_vectRemoteFileList[nItem].strImage );
		CImageViewDlg *pDlg = new CImageViewDlg( this, strTitle );
		pDlg->SetImageFromFile( strImageFile );

		if ( pDlg->ShowModal( ) == wxID_OK )
		{
			// do something here ? - maybe marked as viewed ?
		}
		delete( pDlg );
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnRateImage
////////////////////////////////////////////////////////////////////
void COnlineBrowseDlg::OnRateImage( wxCommandEvent& pEvent )
{
	if( m_nSelectedImage < 0 ) return;

	// get image id, groupid, userid
	long nImageId = m_pUnimapOnline->m_vectRemoteFileList[m_nSelectedImage].nImageId;

	// get scores
	int nScoreQ = m_pScoreQ->GetSelection();
	int nScoreA = m_pScoreA->GetSelection();
	int nScoreS = m_pScoreS->GetSelection();

	// rate directly from here ... should be fast enough to tolerate 
	if( m_pUnimapOnline->RateAstroImage( nImageId, nScoreQ, nScoreA, nScoreS, 0 ) )
	{
		m_pUnimapOnline->m_vectRemoteFileList[nImageId].score_q = nScoreQ;
		m_pUnimapOnline->m_vectRemoteFileList[nImageId].score_a = nScoreA;
		m_pUnimapOnline->m_vectRemoteFileList[nImageId].score_s = nScoreS;
		SetScoreValues( m_nSelectedImage, nImageId );
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnActComments
////////////////////////////////////////////////////////////////////
void COnlineBrowseDlg::OnActComments( wxCommandEvent& pEvent )
{
	// get comments for this image
	if( !m_pUnimapOnline->GetImageComments( m_nSelectedImage, 
		m_pUnimapOnline->m_vectRemoteFileList[m_nSelectedImage].nImageId, 0 ) ) return;

	// create the dialog
	COnlineCommnetsDlg *pDlg = new COnlineCommnetsDlg( this, wxT("View/Add Comments") );
	pDlg->SetConfig( m_pUnimapOnline, m_nSelectedImage );
	if( pDlg->ShowModal( ) == wxID_OK )
	{
		// if last type = new
		if( pDlg->m_nViewType == 0 )
		{
			wxString strTitle = pDlg->m_pTitle->GetLineText(0);
			wxString strMessage = pDlg->m_pBody->GetValue();

			// get image id, groupid, userid
			long nImageId = m_pUnimapOnline->m_vectRemoteFileList[m_nSelectedImage].nImageId;

			// post my comment
			if( m_pUnimapOnline->PostComment( nImageId, strTitle, strMessage ) )
			{
				// set button coments new label
				wxString strTmp;
				m_pUnimapOnline->m_vectRemoteFileList[m_nSelectedImage].nComments++;
				strTmp.Printf( wxT("Comments(%d)"), m_pUnimapOnline->m_vectRemoteFileList[m_nSelectedImage].nComments );
				m_pAddCommentButton->SetLabel( strTmp );

			}
		}
	}
	delete( pDlg );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnActDeleteImage
////////////////////////////////////////////////////////////////////
void COnlineBrowseDlg::OnActDeleteImage( wxCommandEvent& pEvent )
{
	// display check notification
	int nConfirm = wxMessageBox( wxT("Delete remote astro-image?"), wxT("Confirm"), wxYES_NO|wxICON_QUESTION, this );
	// check if ok
	if( nConfirm == wxYES )
	{
		// call to delete from remote
		if( m_pUnimapOnline->DeleteImage( m_pUnimapOnline->m_vectRemoteFileList[m_nSelectedImage].nImageId, 0 ) )
		{
			// also delete from local list
			m_pUnimapOnline->m_vectRemoteFileList.erase( m_pUnimapOnline->m_vectRemoteFileList.begin() + m_nSelectedImage );

			// call to set image list
			SetImageList( );

		}
	}

	return;
}
