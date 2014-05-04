/*******************************************************************
 RTC3Dclient.cpp : 

 Description: implementation of the Real Time C3D protocol client
 
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

#include <stdio.h>
#include <stdlib.h>

#include "RTC3Dclient.h"
#include "rtC3Dframe.h"
#include "utils.h"


#ifdef _DEBUG
#include <crtdbg.h>
#endif

#ifdef _DEBUG
   //#define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
	#define new new(_CLIENT_BLOCK,__FILE__, __LINE__)

#endif // _DEBUG

/*****************************************************************
External Variables
*****************************************************************/
extern char 
	szFreezeID[];

RTC3DCLIENT_API int nGetClientVersion( char* pszVersionString, char* pszVersionNumber )
{
	char
		*pszCur;
	char
		szVersion[64];

	/*
	 * retrieve the SMGR freeze ID
	 */

	if( strncmp( szVersion, "..........", 10 ) == 0 )
	{
		sprintf( szVersion, "NDI C3D client - Unfrozen Version" );
	} /* if */

	/*
	 * if the full version string has been requested,
	 * copy the SMGR freeze id
	 */
	if( pszVersionString != NULL )
	{
		sprintf( pszVersionString, "%s", szVersion );
	} /* if */

	/*
	 * if the version number has been requested,
	 * find the word 'Version' in the SMGR freeze id
	 * and copy the information after the word
	 */
	if( pszVersionNumber != NULL )
	{
		sprintf( pszVersionNumber, "Unknown" );

		pszCur = szVersion;
		while( *pszCur != '\0' )
		{
			if( strncmp( pszCur, "Version", 7 ) == 0 )
			{
				pszCur += 8;
				sprintf( pszVersionNumber, "%s", pszCur );
				break;
			} /* if */

			pszCur++;
		} /* while */
	} /* if */

	return RTC3DCLIENT_CODE_NO_ERROR;

} /* nGetClientVersion */

typedef struct WorkerInfoClient
{
	Client &m_Client;
	HWND m_hWnd;

	WorkerInfoClient( Client &client ): m_Client( client ), m_hWnd( client.m_hWnd)
	{
	}

	bool bBigEndian()
	{
		return m_Client.m_bNetworkByteOrder;
	}

	/***************************************************************************************
	Name: ReceivePacket
	  
	Returns: Packet 
	  
	Description: Receives the packet from the TCP stream
	***************************************************************************************/
	Packet * ReceivePacket( bool bBigEndian )
	{
		Packet *pPacket = new Packet( bBigEndian );

		if( pPacket )
		{
			if( pPacket->nReceive( m_Client.m_socket ) != 0 )
			{
				delete pPacket;
				pPacket = NULL;
			}/* if */
			else
			{
				// make sure the packet is using the current byte order setting
				pPacket->m_bNetworkByteOrder = m_Client.m_bNetworkByteOrder;
			}
		}/* if */

		return pPacket;
	}/* ReceivePacket */

	/***************************************************************************************
	Name: UpdateData
	  
	Returns: void 
	Argument: StringFrame &packet		string frame containing new data
	  
	Description: Adds the string contained in the string packet to the log of commands
	***************************************************************************************/
	void UpdateData(  StringPacket &packet )
	{
		EnterCriticalSection( & m_Client.m_DataLock );

		string str = packet.GetString();
		if( (int)str.length() > 0 )
		{
			m_Client.m_vCommands.push_back( str );
		}/* if */

		LeaveCriticalSection( &m_Client.m_DataLock );
	}/* UpdateData */


	/***************************************************************************************
	Name: UpdateData
	  
	Returns:	void 
	Argument: const XMLPacket &packet
	  
	Description: 
	***************************************************************************************/
	void UpdateData( XMLPacket &packet )
	{
		EnterCriticalSection( & m_Client.m_DataLock );
		
		string str = packet.GetString();
		if( (int) str.length() > 0 )
		{
			m_Client.m_sLastParameters.clear();
			m_Client.m_sLastParameters = str;
		}/* if */

		LeaveCriticalSection( & m_Client.m_DataLock );
	}/* UpdateData */


	/***************************************************************************************
  Name: UpdateData
  
  Returns: void 
  Argument: const EmptyPacket & empty
  
  Description: Removes all the data when an empty packet has arrived
  ***************************************************************************************/
	void UpdateData( const EmptyPacket & empty )
	{
		EnterCriticalSection( &m_Client.m_DataLock);
		m_Client.m_vLast3d.clear();
		m_Client.m_vLast6d.clear();
		m_Client.m_vLastAnalog.clear();
		m_Client.m_vLastPlate.clear();
		LeaveCriticalSection( &m_Client.m_DataLock );
	}

	/***************************************************************************************
	Name: UpdateData
	  
	Returns: void 
	Input: const DataFrame &frame		frame containing new data
	  
	Description: Updates the user data with the data in the lastest frame
	***************************************************************************************/
	void UpdateData( const DataFrame &frame)
	{
		int i=0;
		int nSize = 0;

		EnterCriticalSection( &m_Client.m_DataLock);
		int n3d = frame.GetNum3d();
		int n6d = frame.GetNum6d();
		int nAnalog = frame.GetNumAnalog();
		int nForce = frame.GetNumForce();
		int nEvent = frame.GetNumEvent();

		if( n3d > 0)
		{
			// let waiting threads know we've got new 3d
			// they have to wait until the data is copied
			// because the access to the frame is locked
			// so it doesn't matter we do it before copying
			m_Client.SetEventNew3d();

			// check the number of items
			m_Client.m_lf3dTimeStamp = frame.m_lf3DTimeStamp;
			m_Client.m_uFrame3d = frame.m_n3dFrame;
			nSize = (int) m_Client.m_vLast3d.size();
			if( nSize != n3d )
			{
				if( nSize > n3d )
				{
					// remove last X vector items
					for( i = nSize; i > n3d; i-- )
						//m_Client.m_vLast3d.erase( &m_Client.m_vLast3d[i - 1]); // TURINIG
						m_Client.m_vLast3d.erase( m_Client.m_vLast3d.begin() + (i - 1) ); // TURINIG
				}
				else
				{
					int nAdd = n3d - nSize;
					Position3d pos;
					// add X vector items
					for( i=0; i< nAdd; i++ )
						m_Client.m_vLast3d.push_back( pos );
				}
			}/* if */

			// update the 3D data from frame
			for(i=0; i< n3d; i++ )
			{

				m_Client.m_vLast3d[i] = frame.m_p3d[i];

			}/* for */

		}/* if */

		if( n6d > 0 )
		{
			// let waiting threads know we've got new 6d
			// they have to wait until the data is copied
			// because the access to the frame is locked
			// so it doesn't matter we do it before copying
			m_Client.SetEventNew6d();

			// update 6D data 
			// check the number of items
			m_Client.m_lf6dTimeStamp = frame.m_lf6DTimeStamp;
			m_Client.m_uFrame6d = frame.m_n6dFrame;
			nSize = (int) m_Client.m_vLast6d.size();

			if( nSize != n6d  )
			{
				if( nSize > n6d )
				{
					// remove last X vector items
					for( i = nSize; i > n6d; i-- )
						//m_Client.m_vLast6d.erase( &m_Client.m_vLast6d[i-1] ); // TURINIG
						m_Client.m_vLast6d.erase( m_Client.m_vLast6d.begin() + (i - 1) ); // TURINIG
				}
				else
				{
					int nAdd = n6d - nSize;
					QuatErrorTransformation transf;
					// add X vector items
					for( i=0; i< nAdd; i++ )
						m_Client.m_vLast6d.push_back( transf );
				}
			}/* if */
			
			// update the 6D data from frame
			for(i=0; i< n6d; i++ )
			{

				m_Client.m_vLast6d[i] = frame.m_p6d[i];

			}/* for */
		}/* if */
		
		if( nAnalog > 0 )
		{
			// let waiting threads know we've got new analog data
			// they have to wait until the data is copied
			// because the access to the frame is locked
			// so it doesn't matter we do it before copying
			m_Client.SetEventNewAnalog();

			// check the number of items
			m_Client.m_lfAnalogTimeStamp = frame.m_lfAnalogTimeStamp;
			m_Client.m_uFrameAnalog = frame.m_nAnalogFrame;
			nSize = (int) m_Client.m_vLastAnalog.size();
			if( nSize != nAnalog )
			{
				if( nSize > nAnalog )
				{
					// remove last X vector items
					for( i = nSize; i > nAnalog; i-- )
						//m_Client.m_vLastAnalog.erase( &m_Client.m_vLastAnalog[i-1] ); // TURINIG
						m_Client.m_vLastAnalog.erase( m_Client.m_vLastAnalog.begin() + (i - 1) ); // TURINIG
				}
				else
				{
					int nAdd = nAnalog - nSize;
					float fval;
					// add X vector items
					for( i=0; i< nAdd; i++ )
						m_Client.m_vLastAnalog.push_back( fval );
				}
			}/* if */

			// update the analog data from frame
			for(i=0; i< nAnalog; i++ )
			{

				m_Client.m_vLastAnalog[i] = frame.m_pfAnalog[i];

			}/* for */
		}/* if */

		if( nForce > 0 )
		{
			m_Client.m_lfForceTimeStamp = frame.m_lfForceTimeStamp;
			m_Client.m_uFrameForce = frame.m_nForceFrame;
			nSize = (int) m_Client.m_vLastPlate.size();
			if( nSize != nForce )
			{
				if( nSize > nForce )
				{
					// remove last X vector items
					for( i = nSize; i > nForce; i-- )
						//m_Client.m_vLastPlate.erase( & m_Client.m_vLastPlate[i-1] ); // TURINIG
						m_Client.m_vLastPlate.erase( m_Client.m_vLastPlate.begin() + (i - 1) ); // TURINIG
				}/* if */
				else
				{
					int nAdd = nForce - nSize;
					ForcePlate plate;
					// add X vector items
					for( i=0; i < nAdd; i++ )
						m_Client.m_vLastPlate.push_back( plate );
				}/* else */
			}/* if */

			// update force data from frame
			for( i=0; i< nForce; i++ )
			{

				m_Client.m_vLastPlate[i] = frame.m_pForce[i];

			}/* for */
			
		}/* if */

		if( nEvent > 0 )
		{
			// add events in the vector of events
			// events are expected to be removed after they were processed

			for(i=0; i< nEvent; i++ )
			{
				m_Client.m_vEvents.push_back( frame.m_pEvents[i] );
			}/* for */
		}

		LeaveCriticalSection( &m_Client.m_DataLock );
		
		// by this time all treads waiting for GetNextData have been notified by the
		// signal about a fresh meat...
		// prepare for waiting for another data frame
		m_Client.ResetEventNew3d();
		m_Client.ResetEventNew6d();
		m_Client.ResetEventNewAnalog();
	}/* UpdateData */

}WorkerInfoClient;

/***************************************************************************************
  Name: DllMain
  
  Returns: BOOL APIENTRY 
  Argument:  HANDLE hModule
  Argument: DWORD  ul_reason_for_call
  Argument: LPVOID lpReserved
  
  Description: Activates the winsock library when attaching to the process
  ***************************************************************************************/
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{

	BOOL bRet = TRUE;

    switch( ul_reason_for_call )
	{
		case DLL_PROCESS_ATTACH:
			{
				// activate winsock library
				WORD wVersionRequested;
				WSADATA wsaData;
				int err;
				char cVersion[]={2,0};	// request winsock version 1.1
				 
				wVersionRequested = MAKEWORD( cVersion[0], cVersion[1]);		
				 
				err = WSAStartup( wVersionRequested, &wsaData );
				if ( err != 0 ) {
					/* Tell the user that we could not find a usable */
					/* WinSock DLL.                                  */
					bRet = FALSE;
					SetLastError( C_ERR_WSSTART );
				}
				else
				{
					/* Confirm that the WinSock DLL supports requested version.*/
					/* Note that if the DLL supports versions greater    */
					/* than requested it will still return */
					/* requested version in wVersion since that is the version we      */
					/* requested.                                        */
					 
					if ( LOBYTE( wsaData.wVersion ) != cVersion[0] ||
							HIBYTE( wsaData.wVersion ) != cVersion[1] ) 
					{
						/* Tell the user that we could not find a usable */
						/* WinSock DLL.                                  */
						WSACleanup( );
						bRet = FALSE;
						SetLastError( C_ERR_WSSTART );
					}
					else
						bRet = TRUE;  // winsock is acceptable
				}/* else */
			}/* block */
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			WSACleanup();
			break;
    } /* switch */

    return bRet;

} /* DllMain */


/***************************************************************************************
  Name: RecevingThread
  
  Returns: DWORD WINAPI 
  Argument: LPVOID pParam		WorkerInfoClient & information structure, part of m_vConnections
								vector
  
  Description: working thread receiving and sending data between server and client
			The connection should last until either client disconnects or a disconnect
			command comes from the client on a command channel.
  ***************************************************************************************/
DWORD WINAPI RecevingThread(LPVOID pParam )
{
	WorkerInfoClient * pWorkerInfoClient = reinterpret_cast<WorkerInfoClient *>(pParam);

	Client &Client = pWorkerInfoClient->m_Client;
	DataFrame* pframe = NULL;
	Packet *pPacket = NULL;
	DWORD dwErr = 0;
	char szName[200];

	sprintf( szName, "Client Connection" );
	// set thread name for debuging reasons
	SetThreadName(szName);

	while( Client.bIsConnected() )
	{
		pPacket = pWorkerInfoClient->ReceivePacket( pWorkerInfoClient->bBigEndian() );
		if( pPacket == NULL )
		{
			Client.bDisconnect();
			break;
		}
		else
		{
			// process the packet
			switch( pPacket->m_nType)
			{
			case PKT_TYPE_EMPTY:
				{
					EmptyPacket empty;
					pWorkerInfoClient->UpdateData( empty );

					// send a message to the GUI thread about the arrived packet
					if( pWorkerInfoClient->m_hWnd )
					{
						// if the target window is specified, send there a message
						if( ! PostMessage( pWorkerInfoClient->m_hWnd, MSG_CLIENT_ARRIVED_EMPTY, (WPARAM)empty.m_nSize, 0L) )
						{
							dwErr = GetLastError();
							if( dwErr == ERROR_INVALID_THREAD_ID )
							{
								// no message queue or invalid thread ID
							}
						}
					}/* if */
					delete pPacket;
				}
				break;
			case PKT_TYPE_DATAFRAME:
				{
					// create an instance of the DataFrame
					DataFrame *pDataFrame = new DataFrame(*pPacket);
					delete pPacket;

					// update client with new data
					pWorkerInfoClient->UpdateData( *pDataFrame );

					if( pWorkerInfoClient->m_hWnd )
					{
						if( ! PostMessage( pWorkerInfoClient->m_hWnd, MSG_CLIENT_ARRIVED_DATA, (WPARAM)pDataFrame->m_nSize, 0) )
						{
							dwErr = GetLastError();
							if( dwErr == ERROR_INVALID_THREAD_ID )
							{
								// no message queue or invalid thread ID
							}
						}/* if */
					}

					delete pDataFrame;
				}/* block */
				break;
			case PKT_TYPE_COMMAND:
				{
					// create an instance of the StringPacket
					StringPacket *pStringPacket = new StringPacket(*pPacket);
					delete pPacket;

					// update client with new data
					pWorkerInfoClient->UpdateData( *pStringPacket );

					if( pWorkerInfoClient->m_hWnd )
					{
						if( ! PostMessage( pWorkerInfoClient->m_hWnd, MSG_CLIENT_ARRIVED_STRING, (WPARAM)pStringPacket->m_nSize, 0) )
						{
							dwErr = GetLastError();
							if( dwErr == ERROR_INVALID_THREAD_ID )
							{
								// no message queue or invalid thread ID
							}
						}/* if */
					}

					// if the server sends "bye" command and the response to client's "bye"
					// disconnect
					if( pStringPacket->GetString() == "bye" )
					{
						Client.bDisconnect();
					}

					delete pStringPacket;
				}/* block */
				break;
			
			case PKT_TYPE_XML:
				{
					XMLPacket *pXMLPacket = new XMLPacket( *pPacket );
					delete pPacket;

					// update client with new data
					pWorkerInfoClient->UpdateData( *pXMLPacket );

					if( pWorkerInfoClient->m_hWnd )
					{
						if(! PostMessage( pWorkerInfoClient->m_hWnd, MSG_CLIENT_ARRIVED_PROPS, (WPARAM)pXMLPacket->m_nSize, 0) )
						{
							dwErr = GetLastError();
							if( dwErr == ERROR_INVALID_THREAD_ID )
							{
								// no message queue or invalid thread ID
							}
						}/* if */
					}/* if */

					delete pXMLPacket;

				}/* block */
				break;
			case PKT_TYPE_ERROR:
				{
					// create an instance of the StringPacket
					ErrorPacket *pErrorPacket = new ErrorPacket(*pPacket);
					delete pPacket;

					// update client with new data
					pWorkerInfoClient->UpdateData( *pErrorPacket );

					if( pWorkerInfoClient->m_hWnd )
					{
						if( ! PostMessage( pWorkerInfoClient->m_hWnd, MSG_CLIENT_ARRIVED_ERROR, (WPARAM)pErrorPacket->m_nSize, 0) )
						{
							dwErr = GetLastError();
							if( dwErr == ERROR_INVALID_THREAD_ID )
							{
								// no message queue or invalid thread ID
							}
						}/* if */
					}

					delete pErrorPacket;
					break;
				}/* block */
			default:
				delete pPacket;
				break;
			}/* switch */
			
		}/* else */

		Sleep(0);	// slow down receive side to see what will happen
	}/* while */
	
	// free memory allocated when creating the thread
	delete pWorkerInfoClient;

#ifdef _DEBUG
	_CrtDumpMemoryLeaks( );
#endif
	return 0;
}/* RecevingThread */



/***************************************************************************************
  Name: Client
  
  Returns: 
  
  Description: 
  ***************************************************************************************/
Client::Client()
{ 
	m_hWnd = NULL;
	m_hRecevingThread = 0;
	m_socket = INVALID_SOCKET;
	m_status = disconnected;
	// Initialize the critical section one time only.
	InitializeCriticalSection(&m_DataLock);
	m_h3dArrivedEvent		= CreateEvent( NULL, true /* manual reset */, false, NULL );
	m_h6dArrivedEvent		= CreateEvent( NULL, true /* manual reset */, false, NULL );
	m_hAnalogArrivedEvent	= CreateEvent( NULL, true /* manual reset */, false, NULL );

	m_lf3dTimeStamp = 0;
	m_lf6dTimeStamp = 0;
	m_lfAnalogTimeStamp = 0;
	m_lfForceTimeStamp = 0;

	m_uFrame3d = 0;
	m_uFrame6d = 0;
	m_uFrameAnalog = 0;
	m_uFrameForce = 0;

	m_bNetworkByteOrder = true;
}/* Client */

Client::~Client()
{
	bDisconnect();
	// wait until the receiving thread stops
	WaitForSingleObject( m_hRecevingThread, INFINITE);

	if( m_h3dArrivedEvent != NULL )
	{
		CloseHandle( m_h3dArrivedEvent );
	}
	if( m_h6dArrivedEvent != NULL )
	{
		CloseHandle( m_h6dArrivedEvent );
	}
	if( m_hAnalogArrivedEvent != NULL )
	{
		CloseHandle( m_hAnalogArrivedEvent );
	}
	DeleteCriticalSection(&m_DataLock);
}


/***************************************************************************************
  Name: bIsConnected
  
  Returns: bool 
  
  Description: 
  ***************************************************************************************/
bool Client::bIsConnected() const
{
	return (m_status == connected || m_status == streaming); 
}

/***************************************************************************************
  Name: bConnect
  
  Returns: bool 
  Argument:  DWORD dwAddress	address in host byte order
  Argument: int nPort
  
  Description: 
  ***************************************************************************************/
bool Client::bConnect( DWORD dwAddress, int nPort )
{
	unsigned long ulRemoteAddr = htonl( dwAddress );

	bool bRet = false;
	int nRet = 0;

	if( m_status == disconnected )
	{
	
		// send the byteorder to default
		m_bNetworkByteOrder = true;

		if( m_socket != INVALID_SOCKET )
		{
			closesocket( m_socket );
		}

		m_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (m_socket != INVALID_SOCKET) 
		{
			// set the socket to the blocking mode for now
			nSetSocketBlocking( m_socket, true );
			// turn off Nagle algorithm
			nSetNagle( m_socket, false );

			if(ulRemoteAddr != INADDR_NONE )
			{
				sockaddr_in sinRemote;
				sinRemote.sin_family = AF_INET;
				sinRemote.sin_addr.s_addr = ulRemoteAddr;
				sinRemote.sin_port = htons( nPort );
				if ( connect( m_socket, (sockaddr*)&sinRemote, sizeof(sockaddr_in)) == SOCKET_ERROR ) 
				{
					nRet = WSAGetLastError();
					m_socket = INVALID_SOCKET;
				}
				else 
				{
					// Connected successfuly
					bRet = true;
					m_status = connected;
				}/* else */
			}
		}/* if socket OK*/
	}

	if( m_status == connected )
	{
		// if successfuly connected
		// start streaming thread for now

		WorkerInfoClient *pNewWorkerInfo = new WorkerInfoClient( *this );

		HANDLE hThread = CreateThread(0, 0, RecevingThread, (void*)(pNewWorkerInfo), CREATE_SUSPENDED, NULL);
		
		// duplicate thread handles to keep them valid even after threads finish
		HANDLE hProcess = GetCurrentProcess();
		DuplicateHandle(	hProcess, hThread, 
								hProcess, &m_hRecevingThread, 
								0 /*ignored*/,
								FALSE /* no inherit */,
								DUPLICATE_SAME_ACCESS);

		ResumeThread( m_hRecevingThread );

		// send the first command
		// set data frame communication to little endian 
		// which saves conversion from big to little endian
		// for all windows based applications
		nSendBigEndian( false );
	}/* if Connected */

	return bRet;
}/* bConnect */

/***************************************************************************************
  Name: bConnect
  
  Returns: 
  Input: const char *szHost			address or hostname of the server
  Input: int nPort					port of the server (default 3020)
  
  Description: 
  ***************************************************************************************/
bool Client::bConnect(const char * szHost, int nPort /* =3020 */ )
{
	unsigned long ulAddr = ulGetIPAddress( szHost );
	DWORD dwIP = ntohl( ulAddr );
	
    return bConnect( dwIP, nPort );
}/* bConnect */


/***************************************************************************************
  Name: bDisconnect
  
  Returns: bool 
  
  Description: Shutdown the connection
  ***************************************************************************************/
bool Client::bDisconnect()
{
	bool bRet = false;
	int nRet = 0;
	char rcvBuffer[10000];


	if( m_status == connected || m_status == streaming )
	{
		// Disallow any further data sends.  This will tell the other side
		// that we want to go away now.  If we skip this step, we don't
		// shut the connection down nicely.
		if (shutdown(m_socket, SD_BOTH) == SOCKET_ERROR) 
		{
			nRet = WSAGetLastError();
			return false;
		}

		// Receive any extra data still sitting on the socket.  After all
		// data is received, this call will block until the remote host
		// acknowledges the TCP control packet sent by the shutdown above.
		// Then we'll get a 0 back from recv, signalling that the remote
		// host has closed its side of the connection.

		while ( nRet != WSAESHUTDOWN && nRet!= WSAECONNABORTED && nRet != WSAECONNRESET ) {
			int nNewBytes = recv(m_socket, rcvBuffer, sizeof( rcvBuffer ), 0);
			if (nNewBytes == SOCKET_ERROR) 
			{
				nRet = WSAGetLastError();
				if( nRet != WSAESHUTDOWN && nRet!= WSAECONNABORTED && nRet != WSAECONNRESET)
					return false;
			}
			else if (nNewBytes != 0) 
			{
	//			cerr << endl << "FYI, received " << nNewBytes <<
	//					" unexpected bytes during shutdown." << endl;
			}
			else {
				// Okay, we're done!
				break;
			}
		}

		// Close the socket.
		if (closesocket(m_socket) == SOCKET_ERROR) 
		{
			nRet = WSAGetLastError();
			bRet = false;
		}

		m_status = disconnected;
		m_socket = INVALID_SOCKET;
		bRet = true;

	}
    return bRet;
}/* bDisconnect */



/***************************************************************************************
  Name: Client:nSendCommand
  
  Returns: int 
  Argument: const char *cString
  
  Description: 
  ***************************************************************************************/
int Client::nSendCommand( const char *cString )
{
	int nRet = 0;

	if( cString == NULL || strlen( cString ) < 1 )
		return -1;

	string sTmp = cString;
	int nPos = 0;
	// set byte order according to the command so that
	// client and server expect the same
	ToLower( sTmp );
	nPos = (int)sTmp.find("littleendian" );
	if( nPos >= 0 )
		m_bNetworkByteOrder = false;
	
	nPos = (int)sTmp.find("bigendian");
	if( nPos >= 0 )
		m_bNetworkByteOrder = true;

	StringPacket spacket(cString, m_bNetworkByteOrder);
	
	nRet = spacket.nSend( m_socket );

	return nRet;

}/* SendCommand */


/***************************************************************************************
  Name: nSendBigEndian
  
  Returns: int 
  Argument: bool bBigEndian
  
  Description: 
  ***************************************************************************************/
int Client::nSendBigEndian( bool bBigEndian )
{
	int nRet = 0;

	if( bBigEndian != m_bNetworkByteOrder )
	{
		if( nSendStopStreaming() == 0 )
		{
			DataLock();
			m_bNetworkByteOrder = bBigEndian;
			if( bBigEndian )
				nRet = nSendCommand("SetByteOrder BigEndian");
			else
				nRet = nSendCommand("SetByteOrder LittleEndian" );
			DataUnlock();
		}/* if */
	}/* if */

	return nRet;
}/* nSendBigEndian */

/***************************************************************************************
  Name: ulGetIPAddress
  
  Returns: unsigned long			IP address
  Input:  char *szHost				IP address or a hostname
  Output: char * szIPAddressOut		string version of the IP address ( if supplied )
  
  Description: Identifies if the string contains the IP address or a hostname. If later,
				it tries to resolve it and returns both numerical IP address and 
				the string version as well
  ***************************************************************************************/
unsigned long Client::ulGetIPAddress( const char *szHostName, char *szIPAddressOut /* =NULL*/)
{
    unsigned long ulRemoteAddr = INADDR_NONE;

	if( szHostName != NULL )
	{
		// try if the string contains IP address
		ulRemoteAddr = inet_addr( szHostName );
		if (ulRemoteAddr == INADDR_NONE) 
		{

			HOSTENT *pHostEnt = gethostbyname(szHostName);
			SOCKADDR_IN sockAddr;
			if( pHostEnt )
			{
				ulRemoteAddr = *((unsigned long*)(pHostEnt->h_addr_list[0]));
				if( szIPAddressOut )
				{
					sockAddr.sin_addr.s_addr = ulRemoteAddr;
					strcpy(szIPAddressOut, inet_ntoa(sockAddr.sin_addr));  //this is your ip address 
				}/* if */
			}/* if */

		}/* if */
	}/* if */

	return ulRemoteAddr;
}/* ulGetIPAddress */


/***************************************************************************************
  Name: SetMsgTarget
  
  Returns: void 
  Argument: HWND hWnd
  
  Description: Sets the window handle all the messages about arrived packets will be sent to
  ***************************************************************************************/
void Client::SetMsgTarget( HWND hWnd )
{
	m_hWnd = hWnd;
}


/***************************************************************************************
  Name: nGetNumResponses
  
  Returns: int 
  
  Description: get number of unprocessed responses
  ***************************************************************************************/
int Client::nGetNumResponses()
{
	return (int ) m_vCommands.size();
}


/***************************************************************************************
  Name: sGetResponse
  
  Returns: string 
  Argument: int nIndex
  
  Description: retrieves the response from the list
 ***************************************************************************************/
const char * Client::sGetResponse(int nIndex)
{
	const char *pRet = NULL;

	if( nIndex <= (int)m_vCommands.size() && nIndex > 0 )
	{
		pRet = m_vCommands[ nIndex ].c_str();
	}/* if */

	return pRet;
}/* sGetResponse */


/***************************************************************************************
  Name: sGetLatestResponse
  
  Returns: string 
  
  Description: 
  ***************************************************************************************/
const char * Client::sGetLatestResponse()
{
	const char *pRet = NULL;

	pRet = sGetResponse( (int) m_vCommands.size() -1 );

	return pRet;
}/* sGetLatestResponse */

void Client::SetEventNew3d()
{
	SetEvent( m_h3dArrivedEvent );
}

void Client::ResetEventNew3d()
{
	ResetEvent( m_h3dArrivedEvent );
}

void Client::SetEventNew6d()
{
	SetEvent( m_h6dArrivedEvent );
}

void Client::ResetEventNew6d()
{
	ResetEvent( m_h6dArrivedEvent );
}

void Client::SetEventNewAnalog()
{
	SetEvent( m_hAnalogArrivedEvent );
}

void Client::ResetEventNewAnalog()
{
	ResetEvent( m_hAnalogArrivedEvent );
}

/***************************************************************************************
  Name: RemoveResponse
  
  Returns: 
  Argument: int nIndex
  
  Description: removes the response from the list
***************************************************************************************/
void Client::RemoveResponse( int nIndex )
{
	if( nIndex <= (int)m_vCommands.size() )
	{
		//m_vCommands.erase( &m_vCommands[nIndex] ); // TURINIG
		m_vCommands.erase( m_vCommands.begin() + nIndex ); // TURINIG
	}/* if */
}/* RemoveResponse */


/***************************************************************************************
  Name: nSendVersion
  
  Returns: int 
  Argument: const char *cVer
  
  Description: 
  ***************************************************************************************/
int Client::nSendVersion( const char *cVer )
{
	int nRet = 0;
	string str = "Version ";

	if( cVer )
	{
		str += cVer;
		nRet = nSendCommand( str.c_str() );
	}
	else
		nRet = -1;
	
	return nRet;
}/* nSendVersion */


/***************************************************************************************
  Name: nSendStopStreaming
  
  Returns: int 
  
  Description: Sends a command to stop streaming
  ***************************************************************************************/
int Client::nSendStopStreaming()
{
	int nRet = 0;

	if( m_status == streaming )
	{
		nRet = nSendCommand("StreamFrames Stop");

		if( nRet == 0 )
		{
			m_status = connected;
		}
	}/* if */

	return nRet;
}/* nSendStopStreaming */


/***************************************************************************************
  Name: nSendStartStreaming
  
  Returns: int 
  Argument:  bool b3d
  Argument: bool b6d
  Argument: bool bAnalog
  Argument: bool bForce
  Argument: bool bEvents
  Argument: int nFreq		// ignored
  Argument: int nFreqDivisor // ignored
  
  Description: 
  ***************************************************************************************/
int Client::nSendStartStreaming( bool b3d, bool b6d, bool bAnalog, bool bForce, bool bEvents, int nFreq, int nFreqDivisor )
{
	int nRet =0;
	char sMsg[500];

	sprintf( sMsg, "StreamFrames ");

	if( nFreq > 0 || nFreqDivisor > 0 )
	{
		if( nFreq > 0)
		{
			sprintf( sMsg + strlen(sMsg), "Frequency:%d ", nFreq );
		}/* if */
		else if( nFreqDivisor > 0 )
		{
			sprintf( sMsg + strlen(sMsg), "FrequencyDivisor:%d ", nFreqDivisor );
		}/* else */
	}
	else
	{
		sprintf( sMsg + strlen(sMsg), "AllFrames ");
	}/* else */

	if( b3d )
		sprintf( sMsg + strlen(sMsg), "3D ");
	if( b6d )
		sprintf( sMsg + strlen(sMsg), "6D ");
	if( bAnalog)
		sprintf( sMsg + strlen(sMsg), "Analog ");
	if( bForce )
		sprintf( sMsg + strlen(sMsg), "Force ");
	if( bEvents )
		sprintf( sMsg + strlen(sMsg), "Events ");

	nRet = nSendCommand( sMsg );

	return nRet;
}/* nSendStartStreaming */


/***************************************************************************************
  Name: nSendStartStreamingAll
  
  Returns: int 
  Argument: int nFreq
  Argument: int nFreqDiv
  
  Description: 
  ***************************************************************************************/
int Client::nSendStartStreamingAll(int nFreq, int nFreqDivisor )
{
	int nRet = 0;
	char sMsg[500];

	sprintf( sMsg, "StreamFrames ");

	if( nFreq > 0 || nFreqDivisor > 0 )
	{
		if( nFreq > 0)
		{
			sprintf( sMsg + strlen(sMsg), "Frequency:%d ", nFreq );
		}/* if */
		else if( nFreqDivisor > 0 )
		{
			sprintf( sMsg + strlen(sMsg), "FrequencyDivisor:%d ", nFreqDivisor );
		}/* else */
	}
	else
	{
		sprintf( sMsg + strlen(sMsg), "AllFrames ");
	}/* else */

	sprintf( sMsg + strlen(sMsg), "All");

	nRet = nSendCommand( sMsg );

	return nRet;
}/* nSendStartStreamingAll */


/***************************************************************************************
  Name: nSendFrame
  
  Returns: int 
  Argument: bool b3d = true
  Argument: bool b6d = false
  Argument: bool bAnalog = false
  Argument: bool bForce = false
  
  Description: Requests the latest frame of the specific data 
  ***************************************************************************************/
int Client::nSendFrame(bool b3d, bool b6d, bool bAnalog, bool bForce, bool bEvents)
{
	int nRet = 0;
	char sMsg[500];

	sprintf( sMsg, "SendCurrentFrame ");

	if( b3d )
		sprintf( sMsg + strlen(sMsg), "3D ");
	if( b6d )
		sprintf( sMsg + strlen(sMsg), "6D ");
	if( bAnalog)
		sprintf( sMsg + strlen(sMsg), "Analog ");
	if( bForce )
		sprintf( sMsg + strlen(sMsg), "Force ");
	if( bEvents )
		sprintf( sMsg + strlen(sMsg), "Events" );

	nRet = nSendCommand( sMsg );

	return nRet;
}/* nSendFrame */


/***************************************************************************************
  Name: nSendFrameAll
  
  Returns: int 
  
  Description: Requests all new available data
  ***************************************************************************************/
int Client::nSendFrameAll()
{
	int nRet = 0;

	nRet = nSendCommand( "SendCurrentFrame All" );

	return nRet;
}/* nSendFrameAll */


/***************************************************************************************
  Name: nSendParameters
  
  Returns: int 
  Argument: bool b3d = true
  Argument: bool b6d = false
  Argument: bool bAnalog = false
  Argument: bool bForce = false
  Argument: bool bEvents = false
  
  Description: 
  ***************************************************************************************/
int Client::nSendParameters(bool b3d, bool b6d, bool bAnalog, bool bForce, bool bEvents)
{
	int nRet = 0;
	char sMsg[500];

	sprintf( sMsg , "SendParameters ");

	if( b3d )
		sprintf( sMsg + strlen(sMsg), "3D ");
	if( b6d )
		sprintf( sMsg + strlen(sMsg), "6D ");
	if( bAnalog)
		sprintf( sMsg + strlen(sMsg), "Analog ");
	if( bForce )
		sprintf( sMsg + strlen(sMsg), "Force ");
	if( bEvents )
		sprintf( sMsg + strlen(sMsg), "Events ");

	nRet = nSendCommand( sMsg );

	return nRet;
}/* nSendParameters */


/***************************************************************************************
  Name: nSendParametersAll
  
  Returns: int 
  
  Description: 
  ***************************************************************************************/
int Client::nSendParametersAll()
{
	int nRet = 0;

	nRet = nSendCommand( "SendParameters All" );

	return nRet;

}/* nSendParametersAll */



/***************************************************************************************
  Name: GetLatest3d
  
  Returns: unsigned int			frame number 
  Argument: vector<Position3d> & vLatest3d			output vector with enough items
  
  Description: 
  ***************************************************************************************/
unsigned int Client::GetLatest3d( vector<Position3d> & vLatest3d ) 
{
	unsigned int uRet = 0;
	if( vLatest3d.size() >= m_vLast3d.size() )
	{
		DataLock();
		copy(m_vLast3d.begin(), m_vLast3d.end(), vLatest3d.begin() );
		uRet = m_uFrame3d;
		DataUnlock();
	}/* if */
	else
	{
		// the vector provided has to contain enough items
		// find out how many items the server is streaming using nSendParameters
		// and parsing XML reply
		uRet = 0;
	}

	return uRet;
}


/***************************************************************************************
  Name: GetNext3d
  
  Returns: unsigned 
  Argument: vector<Position3d> & vNext3d			output vector with enough items
  
  Description: 
  ***************************************************************************************/
unsigned int Client::GetNext3d( vector<Position3d> & vNext3d )
{
	// wait until the newly arrived data event is set
	WaitForSingleObject( m_h3dArrivedEvent, 2000 );
	return GetLatest3d( vNext3d );
}

void Client::DataLock() 
{
	EnterCriticalSection( &m_DataLock );
}/* DataLock */

void Client::DataUnlock() 
{
	LeaveCriticalSection( &m_DataLock );
}/* DataUnlock */

/***************************************************************************************
  Name: GetLatest6d
  
  Returns: unsigned int			frame number 
  Argument: vector<QuatErrorTransformation> & vLatest6d			output vector with enough items
  
  Description: Returns the latest 6d frame
  ***************************************************************************************/
unsigned int Client::GetLatest6d( vector<QuatErrorTransformation> & vLatest6d )
{
	unsigned int uRet = 0;
	if( vLatest6d.size() >= m_vLast6d.size() )
	{
		DataLock();
		copy(m_vLast6d.begin(), m_vLast6d.end(), vLatest6d.begin() );
		uRet = m_uFrame6d;
		DataUnlock();
	}
	else
	{
		// The vector provided has to contain enough items.
		// Find out how many items the server is streaming using nSendParameters
		// and parsing XML reply
		uRet = 0;
	}

	return uRet;
}


/***************************************************************************************
  Name: GetNext6d
  
  Returns: unsigned 
  Argument: vector<QuatErrorTransformation> & vLatest6d			output vector with enough items
  
  Description: Waits for the moment of the data arrival and returns with new data
  ***************************************************************************************/
unsigned int Client::GetNext6d( vector<QuatErrorTransformation> & vLatest6d )
{
	WaitForSingleObject( m_h6dArrivedEvent, 2000 );
	return GetLatest6d( vLatest6d );
}

double Client::Get3dTimeStamp() const
{
	return m_lf3dTimeStamp;
}
double Client::Get6dTimeStamp() const
{
	return m_lf6dTimeStamp;
}
double Client::GetAnalogTimeStamp() const
{
	return m_lfAnalogTimeStamp;
}

/***************************************************************************************
  Name: GetLatest6d
  
  Returns: unsigned int			frame number 
  Argument: vector<ErrorTransformation> & vLatest6d			output vector with enough items
  
  Description: Returns latest transformation in euler format
  ***************************************************************************************/
unsigned int Client::GetLatest6d( vector<ErrorTransformation> & vLatest6d )
{
	unsigned int uRet = 0;
	vector<QuatErrorTransformation> vQLatest6d;

	// The quat array must be of the expected size
	vQLatest6d.resize( vLatest6d.size() );

	// get Quaternion data
	uRet = GetLatest6d( vQLatest6d );

	if( uRet != 0 )
	{
		// check the number of items
		int nSize = (int) m_vLast6d.size();

		// fill in 6D data 
		// change Quaternions to  Euler
		for(int i=0; i< nSize; i++ )
		{
			// translation
			vLatest6d[i].translation = vQLatest6d[i].translation;
			// rotation
			QuatRotation & qr = vQLatest6d[i].rotation;
			rotation & rot  = vLatest6d[i].rotation;
			vLatest6d[i].error = vQLatest6d[i].error;

			if( qr.q0 >= MAX_NEGATIVE )
			{
				// transform Quaternion to Euler

	// proprietary functions 
//				RotationMatrixType matSource;
//				CvtQuatToRotationMatrix(&qr, matSource);
//				DetermineEuler(matSource, &rot);
			}/* if */
			else
			{
				rot.pitch = BAD_FLOAT;
				rot.roll = BAD_FLOAT;
				rot.yaw = BAD_FLOAT;
				vLatest6d[i].translation.x = BAD_FLOAT;
				vLatest6d[i].translation.y = BAD_FLOAT;
				vLatest6d[i].translation.z = BAD_FLOAT;
				vLatest6d[i].error = BAD_FLOAT;
			}/* else */
		}/* for */
	}/* if */

	return uRet;
}/* GetLatest6d */


/***************************************************************************************
  Name: GetNext6d
  
  Returns: unsigned 
  Argument: vector<ErrorTransformation> & vNext6d			output vector with enough items
  
  Description: Returns latest transformation in euler format. 
			   It waits for the moment of the data arrival and returns with new data
  ***************************************************************************************/
unsigned int Client::GetNext6d( vector<ErrorTransformation> & vNext6d )
{
	WaitForSingleObject( m_h6dArrivedEvent, 2000 );
	return GetLatest6d( vNext6d );
}

/***************************************************************************************
  Name: GetLatestAnalog
  
  Returns: unsigned int			frame number 
  Argument: vector<float> & vLatestAnalog			output vector with enough items
  
  Description: Returns the latest analog frame
  ***************************************************************************************/
unsigned int Client::GetLatestAnalog( vector<float> & vLatestAnalog ) 
{
	unsigned int uRet = 0;
	if( vLatestAnalog.size() >= m_vLastAnalog.size() )
	{
		DataLock();
		copy(m_vLastAnalog.begin(), m_vLastAnalog.end(), vLatestAnalog.begin() );
		uRet = m_uFrameAnalog;
		//vLatestAnalog = m_vLastAnalog;
		DataUnlock();
	}
	else
	{
		// The vector provided has to contain enough items.
		// Find out how many items the server is streaming using nSendParameters
		// and parsing XML reply
		uRet = 0;
	}

	return uRet;
}/* GetLatestAnalog */


/***************************************************************************************
  Name: GetNextAnalog
  
  Returns: unsigned 
  Argument: vector<float> & vNextAnalog				output vector with enough items
  
  Description: Returns the latest analog frame.
				It waits for the moment of the data arrival and returns with new data
  ***************************************************************************************/
unsigned int Client::GetNextAnalog( vector<float> & vNextAnalog ) 
{
	WaitForSingleObject( m_hAnalogArrivedEvent, 2000 );
	return GetLatestAnalog( vNextAnalog );
}/* GetNextAnalog */

int Client::nGetNumEvents( )
{
	int nSize  = 0;
	DataLock();
	nSize = (int) m_vEvents.size();
	DataUnlock();
	return nSize;
}/* nGetNumEvents */


/***************************************************************************************
  Name: GetEvent
  
  Returns: C3DEvent 
  Argument: int nIndex
  
  Description: 
  ***************************************************************************************/
C3DEvent Client::GetEvent( int nIndex )
{
	return m_vEvents.at(nIndex);
}/* GetEvent */

/***************************************************************************************
  Name: GetLatestEvent
  
  Returns: C3DEvent 
  
  Description: 
  ***************************************************************************************/
C3DEvent Client::GetLatestEvent()
{
	return ( m_vEvents.back() );
}/* GetLatestEvent */


/***************************************************************************************
  Name: RemoveEvent
  
  Returns: void 
  Argument: int nIndex
  
  Description: 
  ***************************************************************************************/
void Client::RemoveEvent( int nIndex )
{
	if( nIndex < (int) m_vEvents.size() )
	{
		//m_vEvents.erase( &m_vEvents[nIndex] ); // TURINIG
		m_vEvents.erase( m_vEvents.begin() + nIndex );
	}/* if */
}/* RemoveEvent */


/***************************************************************************************
  Name: sGetLastParameters
  
  Returns: const 
  
  Description: 
  ***************************************************************************************/
const char * Client::sGetLastParameters()
{
	return m_sLastParameters.c_str();
}/* sGetLastParameters */

int Client::nGetNum3d() const
{
	return (int) m_vLast3d.size();
}

int Client::nGetNum6d() const
{
	return (int) m_vLast6d.size();
}

int Client::nGetNumAnalog() const
{
	return (int) m_vLastAnalog.size();
}