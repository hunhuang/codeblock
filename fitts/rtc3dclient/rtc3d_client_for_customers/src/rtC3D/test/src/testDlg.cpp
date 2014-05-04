/******************************************************************************
testDlg.cpp

	Description: application main dialog implementation

	Copyright (C) 2007, Northern Digital Inc. All rights reserved.

	All Northern Digital Inc. (“NDI”) Media and/or Sample Code and/or Sample Code
	Documentation (collectively referred to as “Sample Code”) is licensed and provided "as
	is” without warranty of any kind. The licensee, by use of the Sample Code, warrants to
	NDI that the Sample Code is fit for the use and purpose for which the licensee intends to
	use the Sample Code. NDI makes no warranties, express or implied, that the functions
	contained in the Sample Code will meet the licensee’s requirements or that the operation
	of the programs contained therein will be error free. This warranty as expressed herein is
	exclusive and NDI expressly disclaims any and all express and/or implied, in fact or in
	law, warranties, representations, and conditions of every kind pertaining in any way to
	the Sample Code licensed and provided by NDI hereunder, including without limitation,
	each warranty and/or condition of quality, merchantability, description, operation,
	adequacy, suitability, fitness for particular purpose, title, interference with use or
	enjoyment, and/or non infringement, whether express or implied by statute, common law,
	usage of trade, course of dealing, custom, or otherwise. No NDI dealer, distributor, agent
	or employee is authorized to make any modification or addition to this warranty.
	In no event shall NDI nor any of its employees be liable for any direct, indirect,
	incidental, special, exemplary, or consequential damages, sundry damages or any
	damages whatsoever, including, but not limited to, procurement of substitute goods or
	services, loss of use, data or profits, or business interruption, however caused. In no
	event shall NDI’s liability to the licensee exceed the amount paid by the licensee for the
	Sample Code or any NDI products that accompany the Sample Code. The said limitations
	and exclusions of liability shall apply whether or not any such damages are construed as
	arising from a breach of a representation, warranty, guarantee, covenant, obligation,
	condition or fundamental term or on any theory of liability, whether in contract, strict
	liability, or tort (including negligence or otherwise) arising in any way out of the use of
	the Sample Code even if advised of the possibility of such damage. In no event shall
	NDI be liable for any claims, losses, damages, judgments, costs, awards, expenses or
	liabilities of any kind whatsoever arising directly or indirectly from any injury to person
	or property, arising from the Sample Code or any use thereof
**********************************************************************************/

#include "stdafx.h"

/***************************************************
	App includes
*****************************************************/
#ifdef SERVER_SUPPORTED
#include "RTC3Dsvr.h"
#endif

#include "RTC3Dclient.h"
#include "test-server-client.h"
#include ".\testdlg.h"
#include "afxwin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
static _CrtMemState stateNow;
static _CrtMemState stateOld;
static _CrtMemState stateDiff;
#endif

#define STATE_REFRESH_TIMER	1
#define DATA_REFRESH_TIMER 2
#define TIMER_PERIOD_MS 500

#define SEL_IP			0
#define SEL_HOSTNAME	1

#define SIZE_DATA_COLUMN_VALID 16

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_CtrlNotice;
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_NOTICE, m_CtrlNotice);
}

BOOL CAboutDlg::OnInitDialog()
{
	BOOL bRet =	CDialog::OnInitDialog();
	CString text;
	
	text.Format("%s%s", "All Northern Digital Inc. (“NDI”) Media and/or Sample Code and/or Sample Code Documentation \r\n \
(collectively referred to as “Sample Code”) is licensed and provided “as is” without warranty of any kind. \r\n \
The licensee, by use of the Sample Code, warrants to NDI that the Sample Code is fit for the use and purpose \r\n \
for which the licensee intends to use the Sample Code. NDI makes no warranties, express or implied, \r\n \
that the functions contained in the Sample Code will meet the licensee’s requirements or that the \r\n \
operation of the programs contained therein will be error free. This warranty as expressed herein \r\n \
is exclusive and NDI expressly disclaims any and all express and/or implied, in fact or in law, \r\n \
warranties, representations, and conditions of every kind pertaining in any way to the Sample Code \r\n \
licensed and provided by NDI hereunder, including without limitation, each warranty and/or condition \r\n \
of quality, merchantability, description, operation, adequacy, suitability, fitness for particular \r\n \
purpose, title, interference with use or enjoyment, and/or non infringement, whether express or \r\n \
implied by statute, common law, usage of trade, course of dealing, custom, or otherwise. \r\n \
No NDI dealer, distributor, agent or employee is authorized to make any modification or addition \r\n \
to this warranty.",
"\r\n\r\nIn no event shall NDI nor any of its employees be liable for any direct, indirect, \r\n \
incidental, special, exemplary, or consequential damages, sundry damages or any damages whatsoever, \r\n \
including, but not limited to, procurement of substitute goods or services, loss of use, data or profits, \r\n \
or business interruption, however caused. In no event shall NDI’s liability to the licensee exceed the \r\n \
amount paid by the licensee for the Sample Code or any NDI products that accompany the Sample Code. \r\n \
The said limitations and exclusions of liability shall apply whether or not any such damages are \r\n \
construed as arising from a breach of a representation, warranty, guarantee, covenant, obligation, \r\n \
condition or fundamental term or on any theory of liability, whether in contract, strict liability, \r\n \
or tort (including negligence or otherwise) arising in any way out of the use of the Sample Code even \r\n \
if advised of the possibility of such damage. In no event shall NDI be liable for any claims, losses, \r\n \
damages, judgments, costs, awards, expenses or liabilities of any kind whatsoever arising directly \r\n \
or indirectly from any injury to person or property, arising from the Sample Code or any use thereof" );
	m_CtrlNotice.SetWindowText( text );

	return bRet;
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CtestDlg dialog



CtestDlg::CtestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CtestDlg::IDD, pParent)
	, m_sServerState(_T(""))
	, m_nIPSelection(SEL_IP)
	, m_sClientState(_T(""))
	, m_sHostname(_T(""))
	, m_dwIP(0)
	, m_sCommand(_T(""))
	, m_sPerformance(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nBytes = 0;
	m_nFrames = 0;
	m_uFrame3d = m_uFrame6d = m_uFrameAnalog = m_nStateTimer = m_nDataTimer = 0;
}

void CtestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_SERVER_STATUS, m_sServerState);
	DDX_Radio(pDX, IDC_RADIO1, m_nIPSelection);
	DDX_Text(pDX, IDC_STATIC_CLIENT_STATUS, m_sClientState);
	DDX_Text(pDX, IDC_EDIT_HOSTNAME, m_sHostname);
	DDX_IPAddress(pDX, IDC_IPADDRESS1, m_dwIP);
	DDX_Control(pDX, IDC_IPADDRESS1, m_CtrlIP);
	DDX_Control(pDX, IDC_STATIC_SERVER_STATUS, m_CtrlServerStatus);
	DDX_Control(pDX, IDC_STATIC_CLIENT_STATUS, m_CtrlClientStatus);
	DDX_Text(pDX, IDC_EDIT3, m_sCommand);
	DDX_Text(pDX, IDC_STATIC_FPS, m_sPerformance);
	DDX_Control(pDX, IDC_STATIC_FPS, m_CtrlPerf);
	DDX_Control(pDX, IDC_STATIC_COMMAND, m_CtrlServerText);
	DDX_Control(pDX, IDC_STATIC_COMMAND_CLIENT, m_CtrlClientString);
	DDX_Control(pDX, IDC_COMBO_COMMAND, m_CtrlComboCommand);
	DDX_Control(pDX, IDC_LIST_3D, m_List3d);
	DDX_Control(pDX, IDC_LIST_6D, m_List6d);
	DDX_Control(pDX, IDC_LIST_ANALOG, m_ListAnalog);
	DDX_Control(pDX, IDC_STATIC_FRAME3D, m_CtrlFrame3d);
	DDX_Control(pDX, IDC_STATIC_FRAME6D, m_CtrlFrame6d);
	DDX_Control(pDX, IDC_STATIC_FRAMEANALOG, m_CtrlFrameAnalog);
	DDX_Control(pDX, IDC_STATIC_TIME3D, m_CtrlTime3d);
	DDX_Control(pDX, IDC_STATIC_TIME6D, m_CtrlTime6d);
	DDX_Control(pDX, IDC_STATIC_TIMEANALOG, m_CtrlTimeAnalog);
}

BEGIN_MESSAGE_MAP(CtestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_CONNECT, OnBnCConnect)
	ON_BN_CLICKED(IDC_BUTTON_SERVER_START, OnBnServerStart)
	ON_BN_CLICKED(IDC_BUTTON_SERVER_STOP, OnBnServerStop)
	ON_BN_CLICKED(IDC_RADIO1, OnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO2, OnClickedRadio)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_DISCONNECT, OnBnCDisconnect)
	ON_WM_TIMER()
	ON_MESSAGE( MSG_CLIENT_ARRIVED_EMPTY, OnPacket )
	ON_MESSAGE( MSG_CLIENT_ARRIVED_DATA, OnPacket )
	ON_MESSAGE( MSG_CLIENT_ARRIVED_STRING, OnPacketString )
	ON_MESSAGE( MSG_CLIENT_ARRIVED_ERROR, OnPacketString )
	ON_MESSAGE( MSG_CLIENT_ARRIVED_PROPS, OnPacketProperties )
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnBnSendCommand )
END_MESSAGE_MAP()


// CtestDlg message handlers

BOOL CtestDlg::OnInitDialog()
{
	RECT rect;
	char szHost[100];
	
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	m_nIPSelection = SEL_IP;

	// set this dialog as the recipient of client's messages about arrived packets
	m_Client.SetMsgTarget( GetSafeHwnd() );
	
	gethostname( szHost, 100);
	m_sHostname = szHost;
	m_dwIP = ntohl( m_Client.ulGetIPAddress( szHost ) );
	UpdateData(false);

	// add commands in a combo box
	m_CtrlComboCommand.AddString("SetByteOrder LittleEndian");
	m_CtrlComboCommand.AddString("SetByteOrder BigEndian");
	m_CtrlComboCommand.AddString("Version 1.0");
	m_CtrlComboCommand.AddString("StreamFrames AllFrames");
	m_CtrlComboCommand.AddString("StreamFrames Frequency:20 3d Analog");
	m_CtrlComboCommand.AddString("StreamFrames FrequencyDivisor:4 6d");
	m_CtrlComboCommand.AddString("StreamFrames AllFrames Force");
	m_CtrlComboCommand.AddString("SendCurrentFrame");
	m_CtrlComboCommand.AddString("SendCurrentFrame 6d");
	m_CtrlComboCommand.AddString("SendCurrentFrame 3d");
	m_CtrlComboCommand.AddString("SendParameters All");
	m_CtrlComboCommand.AddString("SendParameters 3d 6d");
	m_CtrlComboCommand.AddString("StreamFrames Stop");
	m_CtrlComboCommand.AddString( "Bye" );

	int
		nBorderX = 0,
		nFrameX = 0,
		nScrollX = 0,
		nWindowX = 0,
		nColData = 0,
		nColMin = 0;

	nBorderX = GetSystemMetrics( /*SM_CXBORDER*/ SM_CXEDGE );
	nFrameX = GetSystemMetrics( SM_CXFRAME );
	nScrollX = GetSystemMetrics( SM_CXVSCROLL );
	nWindowX = 2 * nBorderX + 2 * nFrameX + nScrollX + 2;


	// setup list controls
	m_List3d.GetWindowRect( &rect );
	int nWidth = (int)(0.32 * (rect.right - rect.left - SIZE_DATA_COLUMN_VALID - nWindowX));

	m_List3d.InsertColumn( 0, "#", LVCFMT_RIGHT, SIZE_DATA_COLUMN_VALID, 0 );
	m_List3d.InsertColumn( 1, "x", LVCFMT_RIGHT, nWidth);
	m_List3d.InsertColumn( 2, "y", LVCFMT_RIGHT, nWidth );
	m_List3d.InsertColumn( 3, "z", LVCFMT_RIGHT, nWidth );

	m_List6d.GetWindowRect( &rect );
	nWidth = (int)(0.14 * (rect.right - rect.left - SIZE_DATA_COLUMN_VALID - nWindowX));

	m_List6d.InsertColumn( 0, "#", LVCFMT_RIGHT, SIZE_DATA_COLUMN_VALID, 0 );
	m_List6d.InsertColumn( 1, "Rz[deg]", LVCFMT_RIGHT, nWidth);
	m_List6d.InsertColumn( 2, "Ry[deg]", LVCFMT_RIGHT, nWidth );
	m_List6d.InsertColumn( 3, "Rx[deg]", LVCFMT_RIGHT, nWidth );	
	m_List6d.InsertColumn( 4, "x", LVCFMT_RIGHT, nWidth);
	m_List6d.InsertColumn( 5, "y", LVCFMT_RIGHT, nWidth );
	m_List6d.InsertColumn( 6, "z", LVCFMT_RIGHT, nWidth );
	m_List6d.InsertColumn( 7, "error", LVCFMT_RIGHT, nWidth );

	m_ListAnalog.GetWindowRect( &rect );
	nWidth = (int)(0.14 * (rect.right - rect.left - SIZE_DATA_COLUMN_VALID - nWindowX));
	m_ListAnalog.InsertColumn( 0, "#", LVCFMT_RIGHT, SIZE_DATA_COLUMN_VALID, 0 );
	m_ListAnalog.InsertColumn( 1, "Volt", LVCFMT_RIGHT, nWidth);

	UpdateControls();

	// start timer refreshing the state of the client
	m_nStateTimer = SetTimer( STATE_REFRESH_TIMER, TIMER_PERIOD_MS, NULL);
	// start timer updating the data
	m_nDataTimer = SetTimer( DATA_REFRESH_TIMER, 75, NULL );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CtestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CtestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CtestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


/***************************************************************************************
  Name: OnBnCConnect
  
  Returns: void 
  
  Description: Connects client to the server
  ***************************************************************************************/
void CtestDlg::OnBnCConnect()
{
	if( UpdateData( true ) )
	{
		if( m_nIPSelection == SEL_IP )
		{
			// IP is selected
			m_Client.bConnect( m_dwIP );
		}
		else
		{
			// hostname is selected
			if( m_sHostname.GetLength() > 0 )
			{
				m_Client.bConnect( m_sHostname.GetBuffer(500) );
				m_sHostname.ReleaseBuffer();
				m_nBytes = 0;
				m_nFrames = 0;
			}
		}

		UpdateControls();
	}

}/* OnBnCConnect */


/***************************************************************************************
  Name: OnBnCDisconnect
  
  Returns: void 
  
  Description: Disconnect client from server
  ***************************************************************************************/
void CtestDlg::OnBnCDisconnect()
{
	m_Client.bDisconnect();

	UpdateControls();
}


/***************************************************************************************
  Name: OnBnServerStart
  
  Returns: void 
  
  Description: Starts the server
  ***************************************************************************************/
void CtestDlg::OnBnServerStart()
{
#ifdef SERVER_SUPPORTED
	m_Server.nStart();
#endif
	UpdateControls();
}/* OnBnServerStart */

/***************************************************************************************
  Name: OnBnServerStop
  
  Returns: void 
  
  Description: Stops the server
  ***************************************************************************************/
void CtestDlg::OnBnServerStop()
{
#ifdef SERVER_SUPPORTED
	m_Server.nStop();
#endif
	UpdateControls();
}/* OnBnServerStop */

/***************************************************************************************
  Name: UpdateControls
  
  Returns: void 
  
  Description: Updates client / server controls
  ***************************************************************************************/
void CtestDlg::UpdateControls()
{
	CString sTmp;
	static DWORD dwLastUpdate = 0L;
	DWORD dwNow = 0L;
	DWORD dwElapsed = 0L;

#ifdef SERVER_SUPPORTED
	int nNumConnections = m_Server.nGetNumConnections();
	// server part
	if( m_Server.bIsUp() )
		sTmp.Format( "Server is up. Connections=%d", nNumConnections );
	else
#endif
		sTmp = "Server is down";


	// m_sServerState = sTmp;
	m_CtrlServerStatus.SetWindowText( sTmp.GetBuffer( sTmp.GetLength() ) );
	sTmp.ReleaseBuffer();


	// client part
	if( m_Client.bIsConnected() )
		sTmp = "Client is connected";
	else
		sTmp = "Client is disconnected";

	// m_sClientState = sTmp;
	m_CtrlClientStatus.SetWindowText( sTmp.GetBuffer( sTmp.GetLength() ) );
	sTmp.ReleaseBuffer();


	if( m_nFrames > 0 )
	{
		dwNow = GetTickCount();
		dwElapsed = dwNow - dwLastUpdate;
		// update the last update time in ms
		dwLastUpdate = dwNow;
		float fElapsed = dwElapsed;
		int nFrames = m_nFrames;
		int nBytes = m_nBytes;

		m_nFrames = 0;
		m_nBytes = 0;
		
		sTmp.Format("fps = %3.2f, Bps = %3.2f", nFrames* 1000.0f / fElapsed , nBytes * 1000.0f / fElapsed );
		
		m_CtrlPerf.SetWindowText( sTmp.GetBuffer( sTmp.GetLength() ));
		sTmp.ReleaseBuffer();
	}

}/* UpdateControls */


/***************************************************************************************
  Name: OnClickedRadio
  
  Returns: void 
  
  Description: 
  ***************************************************************************************/
void CtestDlg::OnClickedRadio()
{
	UpdateData(true);
}



/***************************************************************************************
  Name: OnTimer
  
  Returns: void 
  Argument: UINT nIDEvent
  
  Description: Update controls based on client/server state.
  ***************************************************************************************/
void CtestDlg::OnTimer(UINT nIDEvent)
{
	if( nIDEvent == m_nStateTimer )
	{
		UpdateControls();
	}
	else if( nIDEvent == m_nDataTimer )
	{
		// update list controls with new data
		Update3dList( m_v3d );
		Update6dList( m_v6d );
		UpdateAnalogList( m_vAnalog );

		// update frame number and timestamps
		char szText[200];
		sprintf( szText, "%u", m_uFrame3d ); 
		m_CtrlFrame3d.SetWindowText( szText );
		sprintf( szText, "%u", m_uFrame6d );
		m_CtrlFrame6d.SetWindowText( szText );
		sprintf( szText, "%u", m_uFrameAnalog );
		m_CtrlFrameAnalog.SetWindowText( szText );

		sprintf( szText, "%6.3lf", m_Client.Get3dTimeStamp()  );
		m_CtrlTime3d.SetWindowText( szText );
		sprintf( szText, "%6.3lf", m_Client.Get6dTimeStamp()  );
		m_CtrlTime6d.SetWindowText( szText );
		sprintf( szText, "%6.3lf", m_Client.GetAnalogTimeStamp()  );
		m_CtrlTimeAnalog.SetWindowText( szText );
	}

	CDialog::OnTimer(nIDEvent);
}/* OnTimer */


/***************************************************************************************
  Name: OnPacket
  
  Returns: LRESULT 
  Argument: WPARAM wParam
  Argument: LPARAM lParam
  
  Description: Handle the message indicating a new packet. Updates statistics on number of bytes received
  ***************************************************************************************/
LRESULT CtestDlg::OnPacket(WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = 0;
	int nPacketSize = (int)wParam;

	// update values for fps and Bps statistics
	m_nFrames++;
	m_nBytes += nPacketSize;

	m_v3d.resize( m_Client.nGetNum3d() );
	m_v6d.resize( m_Client.nGetNum6d() );
	m_vAnalog.resize( m_Client.nGetNumAnalog() );

	// get lastest marker data from client
	m_uFrame3d = m_Client.GetLatest3d( m_v3d );
	// get lastest tool data from client
	m_uFrame6d = m_Client.GetLatest6d( m_v6d );
	// get latest analog data from client
	m_uFrameAnalog = m_Client.GetLatestAnalog( m_vAnalog );

	return lRet;
}/* OnPacket */


/***************************************************************************************
  Name: Update3dList
  
  Returns: void 
  Argument: vector<Position3d> & v3d
  
  Description: Updates marker items in the list control
  ***************************************************************************************/
void CtestDlg::Update3dList( vector<Position3d> & v3d )
{
	char szText[100];
	int i;
	int nDataItems = (int ) v3d.size();
	if( nDataItems >= 0 )
	{
		int nRows = m_List3d.GetItemCount();
		if( nRows != nDataItems )
		{
			// adjust number if list items based on new data
			if( nRows < nDataItems )
			{
				for( i=nRows; i< nDataItems; i++)
					m_List3d.InsertItem( LVIF_TEXT | LVIF_STATE, i, "", 0, LVIS_SELECTED, 0, 0 );
			}
			else
			{
				if( nRows > 0 )
				{
					for( i=nRows - 1; i >= nDataItems; i-- )
						m_List3d.DeleteItem( i );
				}
			}/* else */
		}/* if */
	}/* if */

	for( i=0; i< nDataItems; i++ )
	{
		
		sprintf( szText, "%d", i+1 );
		m_List3d.SetItemText( i, 0, szText );
		SetText( szText,  v3d[i].x );
		m_List3d.SetItemText( i, 1, szText );
		SetText( szText,  v3d[i].y );
		m_List3d.SetItemText( i, 2, szText );
		SetText( szText, v3d[i].z );
		m_List3d.SetItemText( i, 3, szText );
	}

}/* Update3dList */


/***************************************************************************************
  Name: Update6dList
  
  Returns: void 
  Argument: vector<ErrorTransformation> & v6d
  
  Description: Updates tool items in the control list
  ***************************************************************************************/
void CtestDlg::Update6dList( vector<ErrorTransformation> & v6d )
{
	char szText[100];
	int i;
	int nDataItems = (int ) v6d.size();
	if( nDataItems >= 0 )
	{
		int nRows = m_List6d.GetItemCount();
		if( nRows != nDataItems )
		{
			// adjust number if list items based on new data
			if( nRows < nDataItems )
			{
				for( i=nRows; i< nDataItems; i++)
					m_List6d.InsertItem( LVIF_TEXT | LVIF_STATE, i, "", 0, LVIS_SELECTED, 0, 0 );
			}
			else
			{
				if( nRows > 0 )
				{
					for( i=nRows - 1; i >= nDataItems; i-- )
						m_List6d.DeleteItem( i );
				}
			}/* else */
		}/* if */
	}/* if */

	ErrorTransformation TmpErrTrans;

	for( i=0; i< nDataItems; i++ )
	{
		
		sprintf( szText, "%d", i+1 );
		m_List6d.SetItemText( i, 0, szText );

		TmpErrTrans = v6d[i];

		// transform rads to deg
		if( TmpErrTrans.rotation.roll >= MAX_NEGATIVE )
		{
			TmpErrTrans.rotation.roll =  v6d[i].rotation.roll * 180.0f/3.14159f;
			TmpErrTrans.rotation.pitch =  v6d[i].rotation.pitch * 180.0f/3.14159f;
			TmpErrTrans.rotation.yaw =  v6d[i].rotation.yaw * 180.0f/3.14159f;
		}


		SetText( szText, TmpErrTrans.rotation.roll );
		m_List6d.SetItemText( i, 1, szText );
		SetText( szText, TmpErrTrans.rotation.pitch);
		m_List6d.SetItemText( i, 2, szText );
		SetText( szText,  TmpErrTrans.rotation.yaw);
		m_List6d.SetItemText( i, 3, szText );
		SetText( szText,  TmpErrTrans.translation.x );
		m_List6d.SetItemText( i, 4, szText );
		SetText( szText,  TmpErrTrans.translation.y );
		m_List6d.SetItemText( i, 5, szText );
		SetText( szText,  TmpErrTrans.translation.z );
		m_List6d.SetItemText( i, 6, szText );
		SetText( szText,  TmpErrTrans.error );
		m_List6d.SetItemText( i, 7, szText );
	}
}/* Update6dList */


/***************************************************************************************
  Name: UpdateAnalogList
  
  Returns: void 
  Argument: vector<float> & vAnalog
  
  Description: Updates analog values in list control
  ***************************************************************************************/
void CtestDlg::UpdateAnalogList( vector<float> & vAnalog )
{
	char szText[100];
	int i;
	int nDataItems = (int ) vAnalog.size();
	
	if( nDataItems >= 0 )
	{
		int nRows = m_ListAnalog.GetItemCount();
		if( nRows != nDataItems )
		{
			// adjust number if list items based on new data
			if( nRows < nDataItems )
			{
				for( i=nRows; i< nDataItems; i++)
					m_ListAnalog.InsertItem( LVIF_TEXT | LVIF_STATE, i, "", 0, LVIS_SELECTED, 0, 0 );
			}
			else
			{
				if( nRows > 0 )
				{
					for( i=nRows - 1; i >= nDataItems; i-- )
						m_ListAnalog.DeleteItem( i );
				}
			}/* else */
		}/* if */
	}/* if */

	for( i=0; i< nDataItems; i++ )
	{
		
		sprintf( szText, "%d", i+1 );
		m_ListAnalog.SetItemText( i, 0, szText );


		SetText( szText, vAnalog[i] );
		m_ListAnalog.SetItemText( i, 1, szText );
	}
}

void CtestDlg::SetText( char *szText, float value )
{
	if ( value != BAD_FLOAT )
	{
		sprintf( szText, "%6.3f", value );
	}
	else
	{
		sprintf( szText, "---");
	}
}/* SetText */

/***************************************************************************************
  Name: OnPacketString
  
  Returns: LRESULT 
  Argument: WPARAM wParam
  Argument: LPARAM lParam
  
  Description: Handle the message indicating a new string packet has arrived. Show its text.
  ***************************************************************************************/
LRESULT CtestDlg::OnPacketString(WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = 0;

	CString sResponse = m_Client.sGetLatestResponse();

	m_CtrlClientString.SetWindowText( sResponse);


	return lRet;
}/* OnPacketCommand */


/***************************************************************************************
  Name: OnPacketProperties
  
  Returns: LRESULT 
  Argument:  WPARAM wParam
  Argument: LPARAM lParam
  
  Description: Handle the message indicating a new parameters have arrived
				Dump them to the file on a D:\ drive
  ***************************************************************************************/
LRESULT CtestDlg::OnPacketProperties( WPARAM wParam, LPARAM lParam)
{
	LRESULT  lRet = 0;
	int nPacketSize = (int)wParam;

	CString sResponse = m_Client.sGetLastParameters();

#ifdef SERVER_SUPPORTED
	// debug - write properties to the file
	FILE *fp = fopen( "d:\\test.xml", "w" );
	if( fp )
	{
		fprintf( fp, "%s", sResponse );
		fclose( fp );
	}
#endif
	AfxMessageBox( sResponse );
	return lRet;
}/* OnPacketProperties */



/***************************************************************************************
  Name: OnBnSendCommand
  
  Returns: void 
  
  Description: Sends a command selected/typed in the combobox
  ***************************************************************************************/
void CtestDlg::OnBnSendCommand()
{

	UpdateData( true );

	CString sCommand;

	m_CtrlComboCommand.GetWindowText( sCommand );
	if( m_Client.nSendCommand( sCommand ) == 0 )
	{
		// command seems to be OK
		int nIndex = m_CtrlComboCommand.SelectString(0, sCommand );
		
		if( nIndex < 0 )
		{
			// add a new command in the combobox
			m_CtrlComboCommand.AddString( sCommand );
		}/* if */
	}/* if */
}/* OnBnSendCommand */


