/******************************************************************
 	testDlg.h
 
  Description: application main dialog definition
 
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
******************************************************************/

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

class Server;
class Client;

// CtestDlg dialog
class CtestDlg : public CDialog
{
// Construction
public:
	CtestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TESTSERVERCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
// Implementation
protected:
	HICON m_hIcon;
#ifdef SERVER_SUPPORTED
	Server m_Server;
#endif
	UINT	m_nStateTimer;
	UINT	m_nDataTimer;
	Client m_Client;
	int m_nBytes;
	int m_nFrames;
	CListCtrl m_List3d;
	CListCtrl m_List6d;
	CListCtrl m_ListAnalog;
	vector<ErrorTransformation> m_v6d ;
	vector<Position3d> m_v3d;
	vector<float> m_vAnalog;
	unsigned int m_uFrame3d;
	unsigned int m_uFrame6d;
	unsigned int m_uFrameAnalog;
protected:
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnPacket(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPacketString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPacketProperties( WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
	void UpdateControls();
	void Update3dList( vector<Position3d> & v3d );
	void Update6dList( vector<ErrorTransformation> & v6d );
	void UpdateAnalogList( vector<float> & vAnalog );
	void SetText( char *, float value );
public:
	afx_msg void OnBnCConnect();
	CString m_sServerState;
	afx_msg void OnBnServerStart();
	afx_msg void OnBnServerStop();
	int m_nIPSelection;
	afx_msg void OnClickedRadio();
	CString m_sClientState;
	CString m_sHostname;
	DWORD m_dwIP;
	CIPAddressCtrl m_CtrlIP;
	afx_msg void OnBnCDisconnect();
	afx_msg void OnTimer(UINT nIDEvent);
	CStatic m_CtrlServerStatus;
	CStatic m_CtrlClientStatus;
	CString m_sCommand;
	afx_msg void OnBnSendCommand();
	CString m_sPerformance;
	CStatic m_CtrlPerf;
	CStatic m_CtrlServerText;
	CStatic m_CtrlClientString;
	CComboBox m_CtrlComboCommand;

	CStatic m_CtrlFrame3d;
	CStatic m_CtrlFrame6d;
	CStatic m_CtrlFrameAnalog;
	CStatic m_CtrlTime3d;
	CStatic m_CtrlTime6d;
	CStatic m_CtrlTimeAnalog;

};
