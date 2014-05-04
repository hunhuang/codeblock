/******************************************************************
	utils.cpp

	Description: rtC3D protocol utilities implementation

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
*****************************************************************/

#include <winsock2.h>
#include <algorithm>
#include <functional>

#include "utils.h"

/***************************************************************************************
  Name: SetThreadName
  
  Returns: void 
  Argument:  LPCSTR szThreadName
  Argument: DWORD dwThreadID		 =-1 

Description: Sets threads name for debugging purposes
***************************************************************************************/
void SetThreadName( LPCSTR szThreadName, DWORD dwThreadID /* =-1 */ )
{
  THREADNAME_INFO info;
  {
    info.szName = szThreadName;
    info.dwThreadID = dwThreadID;
  }
  __try
  {
    //RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD), (DWORD*)&info ); // TURINIG
	RaiseException(0x406D1388, 0, sizeof(info) / sizeof(DWORD), (ULONG_PTR*)&info);
  }
  __except (EXCEPTION_CONTINUE_EXECUTION)
  {
  }
}/* SetThreadName */



/***************************************************************************************
  Name: nSetSocketBlocking
  
  Returns: int 
  Argument:  SOCKET sd
  Argument: bool bBlocking

Description: 
***************************************************************************************/
int nSetSocketBlocking( SOCKET sd, bool bBlocking /* = true */)
{
	unsigned long cmd = (bBlocking ? 0L: 1L) ;
	return ioctlsocket( sd, FIONBIO, &cmd );
}

/***************************************************************************************
  Name: nSetSendSocketBuffer
  
  Returns: int 
  Argument:  SOCKET sd
  Argument: int nBytes
  
  Description: Returns the new set value
  ***************************************************************************************/
int nSetSendSocketBuffer( SOCKET sd, int nBytes )
{
	int nRet = -1;
	int nSize = nBytes;
	int nNewSize = 0;

	if(nBytes > 0)
	{ 
		nRet = setsockopt(sd, SOL_SOCKET, SO_SNDBUF, (char *)&nSize, (int)sizeof(nSize)); 
		
		int size = sizeof(nNewSize); 
		nRet = getsockopt(sd, SOL_SOCKET, SO_SNDBUF, (char *)&nNewSize, &size);
	}

	return nNewSize;
}


/***************************************************************************************
  Name: nSetRecvSocketBuffer
  
  Returns: int 
  Argument:  SOCKET sd
  Argument: int nBytes
  
  Description: Returns the new set value
  ***************************************************************************************/
int nSetRecvSocketBuffer( SOCKET sd, int nBytes )
{
	int nRet = -1;
	int nSize = nBytes;
	int nNewSize = 0;

	if(nBytes > 0)
	{ 
		nRet = setsockopt(sd, SOL_SOCKET, SO_RCVBUF, (char *)&nSize, (int)sizeof(nSize));
		int size = sizeof(nNewSize); 
		nRet = getsockopt(sd, SOL_SOCKET, SO_RCVBUF, (char *)&nNewSize, &size);		
	}

	return nNewSize;
}


/***************************************************************************************
  Name: nSetNagle
  
  Returns: int 
  Argument: SOCKET sd
  Argument: bool bOn 

Description: Turns Nagle algorithm ON/OFF
***************************************************************************************/
int nSetNagle(SOCKET sd, bool bOn /* = true */ )
{
	int nRet = 0;
	BOOL bValue = ( bOn ? 0: 1);

	nRet = setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, (char *)&bValue, (int)sizeof(bValue));

	return nRet;
}/* nSetNagle */



/***************************************************************************************
  Name: ntohf
  
  Returns: float
  Argument: float
  
  Description: Converts float from network to host byte order
  ***************************************************************************************/
float ntohf( float val)
{
	float fRet;
	unsigned long ulRet;
	ulRet = ntohl( *((unsigned long*)(&val)));
	memcpy( &fRet, &ulRet, sizeof( float ) );
	return fRet;
}


/***************************************************************************************
  Name: htonf
  
  Returns: float
  Argument: float
  
  Description: converts float from host to network byte order
				Assumes sizeof(float) == sizeof( long )
  ***************************************************************************************/
//float htonf( float val )
//{
//	float fRet;
//	unsigned long ulRet;
//	ulRet = htonl( *((unsigned long*)(&val)));
//	memcpy( &fRet, &ulRet, sizeof( float ) );
//	return fRet;
//}/* htonf */

UINT64 hton64( UINT64 val )
{
	UINT64 nRet = val;
	bool bHisN = false;    // Host byte order is Network byte order
	int nTest = 0xffff0000;

	if( nTest == htonl( nTest ) )
		bHisN = true;

	if( !bHisN )
	{
		unsigned long H = (unsigned long) ( (val >> 32)& 0x00000000ffffffff );
		unsigned long L = (unsigned long) ( val & 0x00000000ffffffff );

		nRet = ntohl( L );
		nRet <<= 32;
		nRet += ntohl( H );
	}
	else
	{
		// do nothing
	}

	return nRet;
}/* hton64 */

UINT64 ntoh64( UINT64 val )
{
	UINT64 nRet = val;
	bool bHisN = false;    // Host byte order is Network byte order
	int nTest = 0xffff0000;

	if( nTest == htonl( nTest ) )
		bHisN = true;

	if( !bHisN )
	{
		unsigned long L = (unsigned long) ((val >> 32)& 0x00000000ffffffff);
		unsigned long H = (unsigned long) (val & 0x00000000ffffffff);

		nRet = htonl( H );
		nRet <<= 32;
		nRet += htonl( L );
	}
	else
	{
		// do nothing
	}

	return nRet;
}/* ntoh64 */

/***************************************************************************************
  Name: ToLower
  
  Returns: void 
  Argument: string & str
  
  Description: Changes the string to lowercase
  ***************************************************************************************/
void ToLower( string & str )
{
	transform( str.begin() ,str.end() ,str.begin(), tolower);
}/* ToLower */

/***************************************************************************************
  Name: sStripSpaces
  
  Returns: string 
  Argument: const string &str
  
  Description: Removes spaces in the begining and at the end of the string
  ***************************************************************************************/
string sStripSpaces( const string &str )
{
	string sRet;
	string sTemp = str.substr( str.find_first_not_of(' '), str.length() - 1 );

	sRet.clear();
	sRet = sTemp;

	int nLastChar = (int) sTemp.find_last_not_of(' ');
	
	if( nLastChar < (int) sTemp.length() )
	{
		sRet.clear();
		sRet = sTemp.substr( 0, nLastChar + 1 );
	}/* if */

	return sRet;
}/* sStripSpaces */