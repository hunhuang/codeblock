/******************************************************************
 *	C3Dframe.cpp
 *
 * Description: rtC3D protocol data structures implementation
 *
 * Copyright (C) 2007, Northern Digital Inc. All rights reserved.
 *
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
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#include "rtC3Ddatatypes.h"
#include "rtC3Dframe.h"
#include "utils.h"

#ifdef _DEBUG
#include <crtdbg.h>
#endif

#define NO_3D_RELIABILITY 999.0	// C3D marker position reliability - not available

#ifdef _DEBUG
   //#define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
	#define new new(_CLIENT_BLOCK,__FILE__, __LINE__)

#endif // _DEBUG



DataFrame::~DataFrame()
{
	if( m_p3d )
	{
		delete[] m_p3d ;
	}
	if( m_p6d )
		delete[] m_p6d;
	if( m_pfAnalog)
		delete[] m_pfAnalog;
	if( m_pForce)
		delete[] m_pForce;
	if( m_pEvents )
		delete[] m_pEvents;

}


DataFrame::DataFrame( const Packet &other ):Packet(other)
{
	InitializeInstance();
	if( other.m_nType == m_nType)
	{
		UnPackData();
	}
}/* DataFrame */

DataFrame::DataFrame(  bool bBigEndian  ):Packet( bBigEndian )
{
	InitializeInstance();
}

void DataFrame::InitializeInstance()
{
	m_nType = PKT_TYPE_DATAFRAME;

	m_n3d = 0;
	m_n6d = 0;
	m_nAnalog = 0;
	m_nForce = 0;
	m_nEvent = 0;

	m_lf3DTimeStamp = 0;
	m_lf6DTimeStamp = 0;
	m_lfAnalogTimeStamp = 0;
	m_lfForceTimeStamp = 0;
	m_lfEventTimeStamp = 0;

	m_n3dFrame = -1;
	m_n6dFrame = -1;
	m_nAnalogFrame = -1;
	m_nForceFrame = -1;
	m_nEventFrame = -1;

	m_p3d = NULL;
	m_pfAnalog = NULL;
	m_p6d = NULL;
	m_pForce = NULL;
	m_pEvents = NULL;
}/* InitializeInstance */

/***************************************************************************************
  Name: nReceive
  
  Returns: int 
  Argument: SOCKET soc
  
  Description: Receives the packet, checks if it is of the proper type and extracts data
				frame out of it.
  ***************************************************************************************/
int DataFrame::nReceive( SOCKET soc )
{
	int nRet = 0;

	nRet = Packet::nReceive( soc );

	return nRet;
}/* nReceive */


/***************************************************************************************
  Name: nSend
  
  Returns: int	
  Argument: SOCKET soc
  
  Description: Packs all data in the data buffer m_pData based on m_bNetworkByte order
			   and sends it out
  ***************************************************************************************/
int DataFrame::nSend( SOCKET soc )
{
	int nRet = 0;

	// send the packet out
	nRet = Packet::nSend( soc );

	return nRet;
}/* nSend */



/***************************************************************************************
  Name: PackData
  
  Returns: int		size of packed data in the packet (doesn't include packet header)
  
  Description: Packs the data frame values in the buffer
  ***************************************************************************************/
int DataFrame::PackData()
{
	int nBytes = 0;
	int i=0;
	int nMem = 0;
	int nComponents = 0;
	RawFrameHeader	FrameHeader;
	RawFrameComponentHeader CompHeader;

	// calculate the amount of memory needed to pack it up

	nMem += sizeof(FrameHeader);

	if( m_n3d > 0 )
	{
		nMem += sizeof(RawFrame3DData);
		nMem += m_n3d * sizeof( RawFrame3DItem );
		nComponents ++;
	}

	if( m_n6d > 0 )
	{
		nMem += sizeof(RawFrame6DData);
		nMem += m_n6d * sizeof( RawFrame6DItem );
		nComponents ++;
	}
	
	if( m_nAnalog > 0 )
	{
		nMem += sizeof(RawFrameAnalogData);
		nMem += m_nAnalog * sizeof( RawFrameAnalogItem );
		nComponents ++;
	}

	if( m_nForce > 0 )
	{
		nMem += sizeof(RawFrameForceData);
		nMem += m_nForce * sizeof( RawFrameForceItem );
		nComponents ++;
	}

	if( m_nEvent > 0 )
	{
		nMem += sizeof(RawFrameEventData);
		nMem += m_nEvent * sizeof( RawFrameEventItem );
		nComponents ++;
	}

	nMem += nComponents * sizeof( CompHeader );


	// check if we have enough memory already allocated and if not
	// allocate it
	if( m_pData == NULL || m_nSize - (int)sizeof( RawPacketHeader) < nMem )
	{
		if( m_pData )
		{
			delete[] m_pData;
			m_pData = NULL;
		}

		// allocate memory required for data
		m_pData = new char[nMem];
	}	

	// pack in frame header
	if( m_bNetworkByteOrder )
	{
		FrameHeader.m_componentcount = htonl( nComponents );
	}
	else
	{
		FrameHeader.m_componentcount = nComponents ;
	}

	memcpy( m_pData + nBytes, &FrameHeader, sizeof( FrameHeader) );
	nBytes += sizeof( FrameHeader );

	// create components of data frame
	// and pack them in the data buffer

	// 3D component
	if( m_n3d > 0 )
	{
		RawFrame3DData header;
		RawFrame3DItem Item;
		
		// put in component header
		int nCompSize = sizeof( CompHeader ) + sizeof( RawFrame3DData ) + sizeof( RawFrame3DItem ) * m_n3d;
		if( m_bNetworkByteOrder )
		{
			CompHeader.m_frame = htonl( m_n3dFrame );
			CompHeader.m_timestamp = hton64( (UINT64) ( m_lf3DTimeStamp * 1000 + 0.5 ) );
			CompHeader.m_type = htonl( COMP_TYPE_3D );
			CompHeader.m_size = htonl( nCompSize );
		}/* if */
		else
		{
			CompHeader.m_frame = m_n3dFrame;
			CompHeader.m_timestamp = (UINT64) (m_lf3DTimeStamp * 1000 + 0.5) ;
			CompHeader.m_type = COMP_TYPE_3D;
			CompHeader.m_size = nCompSize;
		}/* else */

		memcpy( m_pData + nBytes, & CompHeader, sizeof( CompHeader ) );
		nBytes += sizeof( CompHeader );

		// put in data header
		if( m_bNetworkByteOrder )
		{
			header.m_count = htonl( m_n3d );
		}
		else
			header.m_count = m_n3d;
	
		memcpy( m_pData + nBytes, &header, sizeof( RawFrame3DData ) );
		nBytes += sizeof(RawFrame3DData);
		
		// put in items
		for( i=0; i< m_n3d; i++ )
		{
			if( m_bNetworkByteOrder )
			{
				Item.m_fReliability = htonf( NO_3D_RELIABILITY );
				if( m_p3d[i].x != BAD_FLOAT )
				{
					Item.m_x = htonf( m_p3d[i].x);
					Item.m_y = htonf( m_p3d[i].y);
					Item.m_z = htonf( m_p3d[i].z);
				}
				else
				{
					Item.SetMissing();
				}
			}
			else
			{
				Item.m_fReliability = NO_3D_RELIABILITY;
				if( m_p3d[i].x != BAD_FLOAT )
				{
					Item.m_x = m_p3d[i].x;
					Item.m_y = m_p3d[i].y;
					Item.m_z = m_p3d[i].z;
				}
				else
				{
					Item.SetMissing();
				}
			}
			// copy item in the buffer
			memcpy( m_pData + nBytes, &Item, sizeof( RawFrame3DItem) );
			nBytes += sizeof( RawFrame3DItem);

		}/* for */
		
	}/* if */
	
	// 6D component
	if( m_n6d > 0 )
	{
		RawFrame6DData header;
		RawFrame6DItem Item;

		// put in component header
		int nCompSize = sizeof( CompHeader ) + sizeof( RawFrame6DData ) + sizeof( RawFrame6DItem ) * m_n6d;

		if( m_bNetworkByteOrder )
		{
			CompHeader.m_frame = htonl( m_n6dFrame );
			CompHeader.m_timestamp = hton64( (UINT64) ( m_lf6DTimeStamp * 1000 + 0.5 ) );
			CompHeader.m_type = htonl( COMP_TYPE_6D );
			CompHeader.m_size = htonl( nCompSize );
		}/* if */
		else
		{
			CompHeader.m_frame = m_n6dFrame;
			CompHeader.m_timestamp = (UINT64) (m_lf6DTimeStamp * 1000 + 0.5) ;
			CompHeader.m_type = COMP_TYPE_6D;
			CompHeader.m_size = nCompSize;
		}/* else */

		memcpy( m_pData + nBytes, & CompHeader, sizeof( CompHeader ) );
		nBytes += sizeof( CompHeader );

		// put in header
		if( m_bNetworkByteOrder )
		{
			header.m_count = htonl( m_n6d );
		}
		else
			header.m_count = m_n6d;

		memcpy( m_pData + nBytes, &header, sizeof( RawFrame6DData ) );
		nBytes += sizeof( RawFrame6DData);

		// put in items
		for( i=0; i< m_n6d; i++ )
		{
			if( m_p6d[i].rotation.q0 != BAD_FLOAT )
			{
				if( m_bNetworkByteOrder )
				{
					Item.m_q0 = htonf( m_p6d[i].rotation.q0 );
					Item.m_qx = htonf( m_p6d[i].rotation.qx );
					Item.m_qy = htonf( m_p6d[i].rotation.qy );
					Item.m_qz = htonf( m_p6d[i].rotation.qz );
					Item.m_RMSerror = htonf( m_p6d[i].error );
					Item.m_x = htonf( m_p6d[i].translation.x );
					Item.m_y = htonf( m_p6d[i].translation.y );
					Item.m_z = htonf( m_p6d[i].translation.z );

				}/* if */
				else
				{
					Item.m_q0 = m_p6d[i].rotation.q0;
					Item.m_qx = m_p6d[i].rotation.qx;
					Item.m_qy = m_p6d[i].rotation.qy;
					Item.m_qz = m_p6d[i].rotation.qz;
					Item.m_RMSerror = m_p6d[i].error;
					Item.m_x = m_p6d[i].translation.x;
					Item.m_y = m_p6d[i].translation.y;
					Item.m_z = m_p6d[i].translation.z;
				}/* else */
			}
			else
			{
				Item.SetMissing();
			}
			// copy item in the buffer
			memcpy( m_pData + nBytes, &Item,  sizeof( RawFrame6DItem ) );
			nBytes += sizeof( RawFrame6DItem );
		}/* for */

	}/* if 6D */
	
	// Analog component
	if( m_nAnalog > 0 )
	{
		RawFrameAnalogData header;
		RawFrameAnalogItem Item;

		// put in component header
		int nCompSize = sizeof( CompHeader ) + sizeof( RawFrameAnalogData ) + sizeof( RawFrameAnalogItem ) * m_nAnalog;

		if( m_bNetworkByteOrder )
		{
			CompHeader.m_frame = htonl( m_nAnalogFrame );
			CompHeader.m_timestamp = hton64( (UINT64) ( m_lfAnalogTimeStamp * 1000 + 0.5 ) );
			CompHeader.m_type = htonl( COMP_TYPE_ANALOG );
			CompHeader.m_size = htonl( nCompSize );
		}/* if */
		else
		{
			CompHeader.m_frame = m_nAnalogFrame;
			CompHeader.m_timestamp = (UINT64) (m_lfAnalogTimeStamp * 1000 + 0.5) ;
			CompHeader.m_type = COMP_TYPE_ANALOG;
			CompHeader.m_size = nCompSize;
		}/* else */

		memcpy( m_pData + nBytes, & CompHeader, sizeof( CompHeader ) );
		nBytes += sizeof( CompHeader );

		// put in header
		if( m_bNetworkByteOrder)
		{
			header.m_count = htonl( m_nAnalog );
		}/* else */
		else
		{
			header.m_count = m_nAnalog;
		}/* else */

		memcpy( m_pData + nBytes, &header, sizeof( RawFrameAnalogData ) );
		nBytes += sizeof( RawFrameAnalogData );
		
		// put in items
		for( i = 0; i< m_nAnalog; i++ )
		{
			if( m_pfAnalog[i] != BAD_FLOAT )
			{
				if( m_bNetworkByteOrder )
				{
					Item.m_v = htonf(m_pfAnalog[i]);
				}/* if */
				else
				{
					Item.m_v = m_pfAnalog[i];
				}/* else */
			}
			else
			{
				Item.SetMissing();
			}

			memcpy( m_pData + nBytes, &Item, sizeof( RawFrameAnalogItem ) );
			nBytes += (int)sizeof( RawFrameAnalogItem );
		}/* for */

	}/* if analog */

	// force plate component
	if( m_nForce > 0 )
	{
		RawFrameForceData header;
		RawFrameForceItem Item;

		// put in component header
		int nCompSize = sizeof( CompHeader ) + sizeof( RawFrameForceData ) + sizeof( RawFrameForceItem ) * m_nForce;

		if( m_bNetworkByteOrder )
		{
			CompHeader.m_frame = htonl( m_nForceFrame );
			CompHeader.m_timestamp = hton64( (UINT64) ( m_lfForceTimeStamp * 1000 + 0.5 ) );
			CompHeader.m_type = htonl( COMP_TYPE_FORCE );
			CompHeader.m_size = htonl( nCompSize );
		}/* if */
		else
		{
			CompHeader.m_frame = m_nForceFrame;
			CompHeader.m_timestamp = (UINT64) (m_lfForceTimeStamp * 1000 + 0.5) ;
			CompHeader.m_type = COMP_TYPE_FORCE;
			CompHeader.m_size = nCompSize;
		}/* else */

		memcpy( m_pData + nBytes, & CompHeader, sizeof( CompHeader ) );
		nBytes += sizeof( CompHeader );

		// put in data header
		if( m_bNetworkByteOrder )
		{
			header.m_count = htonl( m_nForce );
		}
		else
			header.m_count = m_nForce;
	
		memcpy( m_pData + nBytes, &header, sizeof( RawFrameForceData ) );
		nBytes += sizeof(RawFrameForceData);

		// put in items
		for( i=0; i< m_nForce; i++ )
		{
			if(! m_pForce[i].bIsMissing() )
			{
				if( m_bNetworkByteOrder )
				{
					Item.m_Fx = htonf( m_pForce[i].m_Fx );
					Item.m_Fy = htonf( m_pForce[i].m_Fy );
					Item.m_Fz = htonf( m_pForce[i].m_Fz );
					Item.m_Mx = htonf( m_pForce[i].m_Mx );
					Item.m_My = htonf( m_pForce[i].m_My );
					Item.m_Mz = htonf( m_pForce[i].m_Mz );
				}
				else
				{
					Item.m_Fx = m_pForce[i].m_Fx;
					Item.m_Fy = m_pForce[i].m_Fy;
					Item.m_Fz = m_pForce[i].m_Fz;
					Item.m_Mx = m_pForce[i].m_Mx;
					Item.m_My = m_pForce[i].m_My;
					Item.m_Mz = m_pForce[i].m_Mz;
				}
			}
			else
			{
				Item.SetMissing();
			}
			// copy item in the buffer
			memcpy( m_pData + nBytes, &Item, sizeof( RawFrameForceItem) );
			nBytes += sizeof( RawFrameForceItem);

		}/* for */

	}/* if Force */

	// events component
	if( m_nEvent > 0 )
	{
		RawFrameEventData header;
		RawFrameEventItem Item;

		// put in component header
		int nCompSize = sizeof( CompHeader ) + sizeof( RawFrameEventData ) + sizeof( RawFrameEventItem ) * m_nEvent;
		if( m_bNetworkByteOrder )
		{
			CompHeader.m_frame = htonl( m_nEventFrame );
			CompHeader.m_timestamp = hton64( (UINT64) ( m_lfEventTimeStamp * 1000 + 0.5 ) );
			CompHeader.m_type = htonl( COMP_TYPE_EVENT );
			CompHeader.m_size = htonl( nCompSize );
		}/* if */
		else
		{
			CompHeader.m_frame = m_nEventFrame;
			CompHeader.m_timestamp = (UINT64) (m_lfEventTimeStamp * 1000 + 0.5) ;
			CompHeader.m_type = COMP_TYPE_EVENT;
			CompHeader.m_size = nCompSize;
		}/* else */		

		memcpy( m_pData + nBytes, & CompHeader, sizeof( CompHeader ) );
		nBytes += sizeof( CompHeader );

		// put in data header
		if( m_bNetworkByteOrder )
		{
			header.m_count = htonl( m_nEvent );
		}
		else
			header.m_count = m_nEvent;
	
		memcpy( m_pData + nBytes, &header, sizeof( RawFrameEventData ) );
		nBytes += sizeof(RawFrameEventData);

		// put in items
		for( i=0; i< m_nEvent; i++ )
		{
			if( m_bNetworkByteOrder )
			{
				// set Item values
			}
			else
			{

			}
			// copy item in the buffer
			memcpy( m_pData + nBytes, &Item, sizeof( RawFrameEventItem) );
			nBytes += sizeof( RawFrameEventItem);
		}/* for */

	}/* if */

	if( nMem != nBytes )
	{
		; // something is wrong
	}

	return nBytes;
}/* PackData */


/***************************************************************************************
  Name: UnPackData
  
  Returns: bool				true - no errors
  
  Description: Unpacks the data frame values from the m_pData buffer
  ***************************************************************************************/
bool DataFrame::UnPackData()
{
	bool bRet = true;
	RawFrameHeader header;
	unsigned int nTotalCompSize = 0;
	int nBytes = 0;
	int i =0;

	if( m_pData == NULL )
		return false;

	// unpack frame header
	memcpy( &header, m_pData + nBytes, sizeof( header ) );
	nBytes += sizeof( header );
	if( m_bNetworkByteOrder )
	{
		header.m_componentcount = ntohl( header.m_componentcount );
	}

	// unpack components
	RawFrameComponentHeader CompHeader;
	for( int nComp = 0; nComp < (int)header.m_componentcount && bRet; nComp ++ )
	{
		memcpy( &CompHeader, m_pData + nBytes, sizeof( CompHeader ) );
		nBytes += sizeof( CompHeader );
		if( m_bNetworkByteOrder )
		{
			CompHeader.m_frame = ntohl( CompHeader.m_frame);
			CompHeader.m_size = ntohl( CompHeader.m_size );
			CompHeader.m_timestamp = ntoh64( CompHeader.m_timestamp );
			CompHeader.m_type = ntohl( CompHeader.m_type );
		}/* if */

		nTotalCompSize += CompHeader.m_size;

		//sanity check
		if( (int)nTotalCompSize > m_nSize - (int)sizeof(RawPacketHeader) )
		{
			bRet = false;
			break;
		}/* if */

		// unpack component data values
		if( bRet )
		{
			switch( CompHeader.m_type )
			{
			case COMP_TYPE_3D:
				{
					RawFrame3DData header;
					RawFrame3DItem Item;

					// update frame number and timestamp
					m_n3dFrame = CompHeader.m_frame;
					m_lf3DTimeStamp = (double) CompHeader.m_timestamp;

					memcpy( &header, m_pData + nBytes, sizeof( header ));
					nBytes += sizeof( header );

					if( m_bNetworkByteOrder )
					{
						header.m_count = ntohl( header.m_count );
					}

					// sanity check
					if( header.m_count * sizeof( Item ) > CompHeader.m_size )
					{
						bRet = false;
						break;
					}
					else
					{
						if( m_n3d != (int) header.m_count)
						{
							// reallocate memory for data
							m_n3d = (int) header.m_count;

							if( m_p3d )
							{
								delete[] m_p3d;
								m_p3d = NULL;
							}/* if */
							if( m_n3d > 0 )
							{
								m_p3d = new Position3d[m_n3d];
							}/* if */
						}
					}/* else */

					// unpack data items
					for( i=0; i< m_n3d && bRet; i++ )
					{
						memcpy( &Item, m_pData + nBytes, sizeof( Item ));
						nBytes += sizeof( Item );

						if( m_bNetworkByteOrder )
						{
							Item.m_fReliability = ntohf( Item.m_fReliability );
							Item.m_x = ntohf( Item.m_x );
							Item.m_y = ntohf( Item.m_y );
							Item.m_z = ntohf( Item.m_z );
						}/* if */
						
						if( ! Item.bIsMissing() )
						{
							m_p3d[i].x = Item.m_x;
							m_p3d[i].y = Item.m_y;
							m_p3d[i].z = Item.m_z;
						}
						else
						{
							m_p3d[i].x = BAD_FLOAT;
							m_p3d[i].y = BAD_FLOAT;
							m_p3d[i].z = BAD_FLOAT;
						}
					}/* for */
				}/* block */
				break;
			case COMP_TYPE_6D:
				{
					RawFrame6DData header;
					RawFrame6DItem Item;

					// update frame number and timestamp
					m_n6dFrame = CompHeader.m_frame;
					m_lf6DTimeStamp = (double) CompHeader.m_timestamp;

					memcpy( &header, m_pData + nBytes, sizeof( header ));
					nBytes += sizeof( header );

					if( m_bNetworkByteOrder )
					{
						header.m_count = ntohl( header.m_count );
					}/* if */

					// sanity check
					if( header.m_count * sizeof( Item ) > CompHeader.m_size )
					{
						bRet = false;
						break;
					}
					else
					{
						if( m_n6d != (int ) header.m_count )
						{
							// reallocate memory for data
							m_n6d = (int ) header.m_count;

							if( m_p6d )
							{
								delete[] m_p6d;
								m_p6d = NULL;
							}/* if */
							if( m_n6d > 0 )
							{
								m_p6d = new QuatErrorTransformation[m_n6d];
							}/* if */
						}
					}/* else */

					// unpack data items
					for( i=0; i< m_n6d && bRet; i++)
					{
						memcpy( &Item, m_pData + nBytes, sizeof( Item ));
						nBytes += sizeof( Item );

						if( m_bNetworkByteOrder )
						{
							Item.m_q0 = ntohf( Item.m_q0 );
							Item.m_qx = ntohf( Item.m_qx );
							Item.m_qy = ntohf( Item.m_qy );
							Item.m_qz = ntohf( Item.m_qz );
							Item.m_RMSerror = ntohf( Item.m_RMSerror );
							Item.m_x = ntohf( Item.m_x );
							Item.m_y = ntohf( Item.m_y );
							Item.m_z = ntohf( Item.m_z );
						}/* if */
						
						if( ! Item.bIsMissing() )
						{
							m_p6d[i].error = Item.m_RMSerror;
							m_p6d[i].rotation.q0 = Item.m_q0;
							m_p6d[i].rotation.qx = Item.m_qx;
							m_p6d[i].rotation.qy = Item.m_qy;
							m_p6d[i].rotation.qz = Item.m_qz;
							m_p6d[i].translation.x = Item.m_x;
							m_p6d[i].translation.y = Item.m_y;
							m_p6d[i].translation.z = Item.m_z;
						}
						else
						{
							m_p6d[i].error = BAD_FLOAT;
							m_p6d[i].rotation.q0 = BAD_FLOAT;
							m_p6d[i].rotation.qx = BAD_FLOAT;
							m_p6d[i].rotation.qy = BAD_FLOAT;
							m_p6d[i].rotation.qz = BAD_FLOAT;
							m_p6d[i].translation.x = BAD_FLOAT;
							m_p6d[i].translation.y = BAD_FLOAT;
							m_p6d[i].translation.z = BAD_FLOAT;
						}
					}/* for */

				}/* block */
				break;
			case COMP_TYPE_ANALOG:
				{
					RawFrameAnalogData header;
					RawFrameAnalogItem Item;

					// update frame number and timestamp
					m_nAnalogFrame = CompHeader.m_frame;
					m_lfAnalogTimeStamp = (double) CompHeader.m_timestamp;

					memcpy( &header, m_pData + nBytes, sizeof( header ));
					nBytes += sizeof( header );

					if( m_bNetworkByteOrder )
					{
						header.m_count = ntohl( header.m_count );
					}

					// sanity check
					if( header.m_count * sizeof( Item ) > CompHeader.m_size )
					{
						bRet = false;
						break;
					}
					else
					{
						if( m_nAnalog != header.m_count )
						{
							// reallocate memory for data
							m_nAnalog = (int) header.m_count;
							if( m_pfAnalog )
							{
								delete[] m_pfAnalog;
								m_pfAnalog = NULL;
							}
							if( m_nAnalog > 0 )
							{
								m_pfAnalog = new float[m_nAnalog];
							}/* if */
						}
					}/* else */
					
					// unpack data items
					for(i=0; i< m_nAnalog && bRet; i++)
					{
						memcpy( &Item, m_pData + nBytes, sizeof( Item ));
						nBytes += sizeof( Item );

						if( m_bNetworkByteOrder )
						{
							Item.m_v = ntohf( Item.m_v );
						}
						if( ! Item.bIsMissing()  )
						{
							m_pfAnalog[i] = Item.m_v;
						}
						else
						{
							m_pfAnalog[i] = BAD_FLOAT;
						}
					}/* for */
				}/* block */
				break;
			case COMP_TYPE_FORCE:
				{
					RawFrameForceData header;
					RawFrameForceItem Item;

					// update frame number and timestamp
					m_nForceFrame = CompHeader.m_frame;
					m_lfForceTimeStamp = (double) CompHeader.m_timestamp;

					memcpy( &header, m_pData + nBytes, sizeof( header));
					nBytes += sizeof( header );

					if( m_bNetworkByteOrder )
					{
						header.m_count = ntohl( header. m_count );
					}

					// sanity check
					if( header.m_count * sizeof( Item ) > CompHeader.m_size )
					{
						bRet = false;
						break;
					}
					else
					{
						if( m_nForce != (int) header.m_count )
						{
							// reallocate memory for data
							m_nForce = (int) header.m_count;
							if( m_pForce )
							{
								delete [] m_pForce;
								m_pForce = NULL;
							}
							if( m_nForce > 0 )
							{
								m_pForce = new ForcePlate[m_nForce];
							}/* if */
						}
					}/* else */
					
					// unpack data items
					for(i=0; i< m_nForce && bRet; i++)
					{
						memcpy( &Item, m_pData + nBytes, sizeof( Item ));
						nBytes += sizeof( Item );

						if( m_bNetworkByteOrder )
						{
							m_pForce[i].m_Fx = ntohf( Item.m_Fx );
							m_pForce[i].m_Fy = ntohf( Item.m_Fy );
							m_pForce[i].m_Fz = ntohf( Item.m_Fz );
							m_pForce[i].m_Mx = ntohf( Item.m_Mx );
							m_pForce[i].m_My = ntohf( Item.m_My );
							m_pForce[i].m_Mz = ntohf( Item.m_Mz );
						}
						else
						{
							m_pForce[i].m_Fx = Item.m_Fx;
							m_pForce[i].m_Fy = Item.m_Fy;
							m_pForce[i].m_Fz = Item.m_Fz;
							m_pForce[i].m_Mx = Item.m_Mx;
							m_pForce[i].m_My = Item.m_My;
							m_pForce[i].m_Mz = Item.m_Mz;
						}/* else */

						// change the "protocol missing value" to NDI BAD_FLOAT
						if( m_pForce[i].bIsMissing() )
						{
							m_pForce[i].m_Fx = BAD_FLOAT;
							m_pForce[i].m_Fy = BAD_FLOAT;
							m_pForce[i].m_Fz = BAD_FLOAT;
							m_pForce[i].m_Mx = BAD_FLOAT;
							m_pForce[i].m_My = BAD_FLOAT;
							m_pForce[i].m_Mz = BAD_FLOAT;
						}
					}/* for */
			
				}/* block */
				break;
			case COMP_TYPE_EVENT:
				{
					RawFrameEventData header;
					RawFrameEventItem Item;

					// update frame number and timestamp
					m_nEventFrame = CompHeader.m_frame;
					m_lfEventTimeStamp = (double) CompHeader.m_timestamp;

					memcpy( &header, m_pData + nBytes, sizeof( header ));
					nBytes += sizeof( header );

					if( m_bNetworkByteOrder )
					{
						header.m_count = ntohl( header.m_count );
					}

					// sanity check
					if( header.m_count * sizeof( Item ) > CompHeader.m_size )
					{
						bRet = false;
						break;
					}
					else
					{
						if( m_nEvent != header.m_count )
						{
							// reallocate memory for data
							m_nEvent = (int) header.m_count;
							if( m_pEvents )
							{
								delete[] m_pEvents;
								m_pEvents = NULL;
							}
							if( m_nEvent > 0 )
							{
								m_pEvents = new C3DEvent[m_nEvent];
							}/* if */
						}
					}/* else */
					
					// unpack data items
					for(i=0; i< m_nEvent && bRet; i++)
					{
						memcpy( &Item, m_pData + nBytes, sizeof( Item ));
						nBytes += sizeof( Item );

						if( m_bNetworkByteOrder )
						{
							m_pEvents[i].m_nID = ntohl( Item.m_id );
							m_pEvents[i].m_TimeStamp = ntoh64( Item.m_TimeStamp );
							m_pEvents[i].Params.m_IntegerParams[0] = ntohl( Item.m_param1 );
							m_pEvents[i].Params.m_IntegerParams[1] = ntohl( Item.m_param2 );
							m_pEvents[i].Params.m_IntegerParams[2] = ntohl( Item.m_param3 );
						}
						else
						{
							m_pEvents[i].m_nID = Item.m_id;
							m_pEvents[i].m_TimeStamp = Item.m_TimeStamp;
							m_pEvents[i].Params.m_IntegerParams[0] = Item.m_param1;
							m_pEvents[i].Params.m_IntegerParams[1] = Item.m_param2;
							m_pEvents[i].Params.m_IntegerParams[2] = Item.m_param3;
						}/* else */
					}/* for */
				}/* block */
				break;
			default:
				// unknown type
				break;
			}/* switch */
		}/* if */
	}/* for */

	return bRet;
}/* UnPackData */


/***************************************************************************************
  Name: SetNum3d
  
  Returns: void 
  Argument: int n3d
  
  Description: Sets new number of 3d items and reallocates memory
  ***************************************************************************************/
void DataFrame::SetNum3d( int n3d)
{
	if( m_n3d != n3d )
	{
		m_n3d = n3d;
		if( m_p3d != NULL )
		{
			delete [] m_p3d;
			m_p3d = NULL;
		}
		if( m_n3d > 0 )
		{
			m_p3d = new Position3d[m_n3d];
		}
	}/* if */
}/* SetNum3d */


/***************************************************************************************
  Name: SetNum6d
  
  Returns: void 
  Argument: int n6d
  
  Description: Sets new number of 6d items and reallocates memory
  ***************************************************************************************/
void DataFrame::SetNum6d( int n6d)
{
	if( m_n6d != n6d )
	{
		m_n6d = n6d;
		if( m_p6d != NULL )
		{
			delete [] m_p6d;
			m_p6d = NULL;
		}
		if( m_n6d > 0 )
		{
			m_p6d = new QuatErrorTransformation[m_n6d];
		}
	}/* if */
}/* SetNum6d */


/***************************************************************************************
  Name: SetNumAnalog
  
  Returns: void 
  Argument: int nAnalog
  
  Description: Sets new number of analog items and reallocates memory
  ***************************************************************************************/
void DataFrame::SetNumAnalog( int nAnalog)
{
	if( m_nAnalog != nAnalog )
	{
		m_nAnalog = nAnalog;
		if( m_pfAnalog != NULL )
		{
			delete [] m_pfAnalog;
			m_pfAnalog = NULL;
		}
		if( m_nAnalog > 0 )
		{
			m_pfAnalog = new float[m_nAnalog];
		}
	}/* if */
}/* SetNumAnalog */


/***************************************************************************************
  Name: SetNumForce
  
  Returns: void 
  Argument: int nForce
  
  Description: 
  ***************************************************************************************/
void DataFrame::SetNumForce( int nForce )
{
	if( m_nForce != nForce )
	{
		m_nForce = nForce;
		if( m_pForce!= NULL )
		{
			delete [] m_pForce;
			m_pForce = NULL;
		}
		if( m_nForce > 0 )
		{
			m_pForce = new ForcePlate[m_nForce];
		}
	}/* if */
}

/***************************************************************************************
  Name: SetNumEvent
  
  Returns: void 
  Argument: int nEvent
  
  Description: 
  ***************************************************************************************/
void DataFrame::SetNumEvent( int nEvent )
{
	if( m_nEvent != nEvent )
	{
		m_nEvent = nEvent;
		if( m_pEvents != NULL )
		{
			delete [] m_pEvents;
			m_pEvents = NULL;
		}
		if( m_nEvent > 0 )
		{
			m_pEvents = new C3DEvent[m_nEvent];
		}
	}/* if */
}/* SetNumEvent */

/***************************************************************************************
  Name: operator=
  
  Returns: DataFrame 
  Argument: const DataFrame & other
  
  Description: 
  ***************************************************************************************/
DataFrame & DataFrame::operator=(const DataFrame & other)
{
	Packet::operator =( other );
	UnPackData();
	return *this;
}

int DataFrame::GetNum3d() const
{
	return m_n3d;
}
int DataFrame::GetNum6d() const
{
	return m_n6d;
}
int DataFrame::GetNumForce() const
{
	return m_nForce;
}
int DataFrame::GetNumAnalog() const
{
	return m_nAnalog;
}
int DataFrame::GetNumEvent() const
{
	return m_nEvent;
}


//------------------------------------------ PACKET ----------------------------------------------
Packet::Packet(bool bBigEndian /* = true */)
{
	InitializeInstance();
	m_bNetworkByteOrder = bBigEndian;
}

Packet::Packet( const Packet & packet )
{
	InitializeInstance();
	*this = packet;
}

Packet::~Packet()
{
	if( m_pData != NULL )
	{
		delete[] m_pData;
	}
}

void Packet::InitializeInstance()
{
	m_pData = NULL;
	m_nSize = sizeof( RawPacketHeader );
	m_nType = PKT_TYPE_EMPTY;
	m_bNetworkByteOrder = true;	// default Big-Endian
	memset( &m_cSendBuffer, 0, sizeof( m_cSendBuffer)); 
	memset( &m_cRecvBuffer, 0, sizeof( m_cRecvBuffer)); 
}

/***************************************************************************************
  Name: operator=
  
  Returns: Packet& 
  Argument: const Packet& other
  
  Description: 
  ***************************************************************************************/
Packet& Packet::operator=(const Packet& other)
{
	m_nSize = other.m_nSize;
	m_nType = other.m_nType;
	m_bNetworkByteOrder = other.m_bNetworkByteOrder;
	
	if( m_pData != NULL )
	{
		delete[] m_pData;
	}

	if( other.m_pData )
	{
		int nDataSize = m_nSize - (int)sizeof(RawPacketHeader);
		m_pData = new char[nDataSize];
		memcpy( m_pData, other.m_pData, nDataSize );
	}/* if */
	
	return *this;
}/* operator= */


/***************************************************************************************
  Name: nReceive
  
  Input: SOCKET remoteSocket
  Returns: int 
  
  Description: Receives the packet from the client. First reads the header to find out the
				type and the size. Then it reads the data part
  ***************************************************************************************/
int Packet::nReceive(SOCKET remoteSocket )
{
	bool bStop = false;
	// receive the packet header first
	RawPacketHeader PH;
	char *pBuffer = NULL;
	char *pTempBuffer = NULL;
	int nDataReceived = 0;

	int nRecv;
	int nReceived = 0;
	int nRequired = sizeof(RawPacketHeader);
	int nRet = 0;

	// read the packet header first to find out the packet size
	while( nReceived < nRequired )
	{
		nRecv = recv( remoteSocket, m_cRecvBuffer + nReceived, nRequired - nReceived, 0 );
	
		if( nRecv < 0)
		{
			nRet = WSAGetLastError();
			bStop = true;
			break;
		}

		if( nRecv  == 0 )
		{
			// the connection was shutdown
			nRet = -2;		// nothing was received, the packet stays unchanged
			bStop = true;
			break;
		}/* if */

		nReceived += nRecv;

	}/* while */

	
	if( !bStop )
	{
		// interpret data as packet header - always Big-Endian
		PH.m_size = ntohl( *((u_long*)(m_cRecvBuffer)) );
		PH.m_type = ntohl( *((u_long*)(m_cRecvBuffer + sizeof(u_long))) );


		// allocate memory for the incoming data
		int nDataSize = PH.m_size - (int)sizeof( RawPacketHeader );

		// sanity check
		if( PH.m_size > PACKET_MAX_SIZE )
		{
			bStop = true;

			// check for http request
			if( strncmp( m_cRecvBuffer, "GET /H", 5 ) == 0 )
			{
				// signal http request came to this port
				nRet = HTTP_REQUEST;
				// read the rest of the request
				recv( remoteSocket, m_cRecvBuffer + nReceived, sizeof( m_cRecvBuffer ), 0 );
			}
			else
			{
				nRet = -1;
			}
		}/* if */
		else
		{

			if( PH.m_size > PACKET_RECV_BUFF_SIZE )
			{
				// if the data doesn't fit our receive buffer, allocate a new memory for it
				pTempBuffer = new char[PH.m_size];
				pBuffer = pTempBuffer;
			}/* if */
			else
			{
				pBuffer = m_cRecvBuffer;
			}

			if( pBuffer != NULL )
			{

				nRequired = PH.m_size - sizeof( RawPacketHeader );		// size minus header
				nReceived = nDataReceived;

				while( nReceived < nRequired && !bStop)
				{
					// receive the rest of the data
					nRecv = recv( remoteSocket, pBuffer + nReceived, nRequired - nReceived, 0 );
				
					if( nRecv > 0 )
					{
						nReceived += nRecv;
					}/* if */
					else if( nRecv == SOCKET_ERROR )
					{
						nRet = WSAGetLastError();	
						if( nRet != WSAEWOULDBLOCK )
						{
							// the connection was shutdown or error
							bStop = true;
							m_nSize = sizeof(RawPacketHeader) + nReceived;
							m_nType = PH.m_type;
							break;
						}/* if */
						else
						{
							Sleep(1);
						}/* else */
					}/* else if */
					else
					{
						// nTemp is 0
						// Client closed connection before we could reply to
						// all the data it sent, so bomb out early.
						nRet = -2;
						break;	
					}/* else */
				}/* while */
				
				if( nRet == 0 )
				{
					// whole packet was received OK
					m_nSize = PH.m_size;
					m_nType = PH.m_type;
				}

				// update data pointer to link to newly arrived data
				if( m_pData != NULL )
				{
					delete[] m_pData;
					m_pData = NULL;
				}/* if */

				if( m_nSize - (int)sizeof(RawPacketHeader) > 0 )
				{
					m_pData = new char[m_nSize - (int)sizeof(RawPacketHeader)];
					memcpy( m_pData, pBuffer, m_nSize - (int)sizeof(RawPacketHeader) );
				}

			}/* if */

			if( pTempBuffer != NULL )
			{
				// free a temp buffer if it was used
				delete[] pTempBuffer;
			}
		}/* if sanity check */
	}/* if */

	if( nRet == 0 )
	{
		if( ! UnPackData() )
		{
			nRet = -1;	// problem unpacking
		}
	}/* if */

	return nRet;
}/* Packet::nReceive */


/***************************************************************************************
  Name: PackData
  
  Returns: int     size of packed data in the packet (doesn't include packet header) 
  
  Description: Returns the size of bytes packed in m_pData buffer
  ***************************************************************************************/
int Packet::PackData()
{
	int nHeaderSize = (int) sizeof( RawPacketHeader );
	int nRet = 0;
	if( m_pData && m_nSize > nHeaderSize )
		nRet = m_nSize - nHeaderSize;
	else
		nRet = 0;
	return nRet;
}/* PackData */

bool Packet::UnPackData()
{
	return true;
}/* UnPackData */

/***************************************************************************************
  Name: nSend
  
  Returns: int 
  Argument: SOCKET remoteSocket
  
  Description: Sends the packet to the socket. 
			   It assumes the packet data is already ready
			   to send out in the proper byte order defined by m_bNetworkByteOrder.
			   It also assumes m_nSize already indicates the total number of bytes
			   to be sent out - header + data;

  ***************************************************************************************/
int Packet::nSend( SOCKET remoteSocket )
{
	int nRet = -1;
	int nSentBytes = 0;
	int nSizeToSend = 0;
	int nTemp = 0;
	char *pBuffer = NULL;
	char *pTempBuffer = NULL;

	// pack Data portion of the packet into a buffer m_pData
	// and calculate the total size of the packet to be sent
	m_nSize = PackData() + (int)sizeof( RawPacketHeader );

	nSizeToSend = m_nSize;
	
	// check if the size make sense
	if( m_nSize >= (int) sizeof( RawPacketHeader ) )
	{
		nRet = 0;
		// prepare data packet to be sent out
		// put in header
		RawPacketHeader header;

		// packet header information is always big-endian
		header.m_size = htonl( m_nSize );
		header.m_type = htonl( m_nType );


		if( m_nSize > PACKET_SEND_BUFF_SIZE)
		{
			// special case - the packet doesn't fit our send buffer
			// create a special, bigger buffer
			pTempBuffer = new char[m_nSize];
			pBuffer = pTempBuffer;
		}/* if */
		else
		{
			pBuffer = m_cSendBuffer;
		}

		// copy all packet into send buffer
		memcpy( pBuffer, &header, sizeof(header) );
		memcpy( pBuffer + sizeof(header), m_pData, m_nSize - sizeof(header) );

		while (nSentBytes < nSizeToSend) 
		{

			nTemp = send( remoteSocket, pBuffer + nSentBytes, nSizeToSend - nSentBytes, 0);

			if (nTemp > 0) 
			{
				nSentBytes += nTemp;
			}
			else if (nTemp == SOCKET_ERROR) 
			{
				nRet = WSAGetLastError();
				if( nRet != WSAEWOULDBLOCK )
				{
					break;
				}
				else
				{
					Sleep(1);
				}/* else */
			}/* else if */
			else 
			{
				// nTemp is 0
				// Client closed connection before we could reply to
				// all the data it sent, so bomb out early.
				// cout << "Peer unexpectedly dropped connection!" << 
				//       endl;
				nRet = -2;
				break;
			}/* else */
		}/* while */
	}/* if */

	if( pTempBuffer != NULL )
	{
		// free a temp buffer if it was used
		delete[] pTempBuffer;
	}

	return nRet;
}/* Packet::nSend */

//------------------------------------------ string packet ------------------------------------------------------

StringPacket::StringPacket( bool bBigEndian /* = true */ ) : Packet( bBigEndian )
{
	InitInstance();
}/* StringPacket */

StringPacket::StringPacket( const Packet & packet ) : Packet( packet )
{
}

StringPacket::StringPacket( const string& str, bool bBigEndian /* = true */ ) : Packet( bBigEndian )
{
	InitInstance();
	SetString( str );
}

StringPacket::StringPacket( const char * str, bool bBigEndian /* = true */ ) : Packet( bBigEndian )
{
	InitInstance();
	SetString( str );
}

void StringPacket::InitInstance()
{
	m_nType = PKT_TYPE_COMMAND;
	m_sString.clear();
}/* InitializeInstance */

/***************************************************************************************
  Name: GetString
  
  Returns: string 
  
  Description: Returns a string contained in the packet. Since the string doesn't have to be
				ended with a NULL character, takes in account size indicated in the header.
  ***************************************************************************************/
string StringPacket::GetString() 
{
	UnPackData();
	return m_sString;
}/* GetString */


/***************************************************************************************
  Name: SetString
  
  Returns: void 
  Argument: const char *pStr
  
  Description: 
  ***************************************************************************************/
void StringPacket::SetString( const char *pStr)
{
	if( pStr )
	{
		m_sString = pStr;
	}
	else
	{
		m_sString.clear();
	}

	PackData();

}/* SetString */


/***************************************************************************************
  Name: PackData
  
  Returns: int			size of packed data in the packet (doesn't include packet header) 
  
  Description: Packs string into a data buffer. NULL termination is included.
  ***************************************************************************************/
int StringPacket::PackData()
{
	int nLen = 0;

	m_nSize = (int)sizeof(RawPacketHeader);
	
	// free the old buffer first
	if( m_pData )
	{
		delete [] m_pData;
		m_pData = NULL;
	}

	// allocate a new buffer for a new string
	nLen = (int) m_sString.length() + 1;

	m_pData = new char[nLen + 1];
	strncpy(m_pData, m_sString.c_str(), nLen );	

	m_nSize += nLen;

	return nLen;
}/* PackData */


/***************************************************************************************
  Name: UnPackData
  
  Returns: bool 
  
  Description: 
  ***************************************************************************************/
bool StringPacket::UnPackData()
{
	bool bRet = true;
	int nLen = m_nSize - (int)sizeof(RawPacketHeader);

	if( m_pData == NULL )
		return false;

	m_sString.clear();

	if( nLen > 0 )
	{
	   for( int i=0; i< nLen; i++ )
	   {
		   m_sString.push_back( m_pData[i] );
	   }/* for */
	}/* if */

	return bRet;
}/* UnPackData */


/***************************************************************************************
  Name: SetString
  
  Returns: void 
  Argument: const string& str
  
  Description: 
  ***************************************************************************************/
void StringPacket::SetString( const string& str )
{
	SetString( str.c_str() );
}/* SetString */

//------------------------------------------ XML packet ------------------------------------------------------

XMLPacket::XMLPacket( bool bBigEndian /* = true */ ) : StringPacket( bBigEndian )
{
	InitInstance();
}/* XMLPacket */

XMLPacket::XMLPacket( const Packet & packet ) : StringPacket( packet )
{
}

XMLPacket::XMLPacket( const string& str, bool bBigEndian /* = true */ ) : StringPacket( str, bBigEndian )
{
	InitInstance();
}

XMLPacket::XMLPacket( const char * str, bool bBigEndian /* = true */ ) : StringPacket( str, bBigEndian )
{
	InitInstance();
}

void XMLPacket::InitInstance()
{
	m_nType = PKT_TYPE_XML;
}/* InitializeInstance */

// ---------------------------------------------- EMPTY PACKET ---------------------------------------
EmptyPacket::EmptyPacket()
{
	m_nType = PKT_TYPE_EMPTY;
}

//------------------------------------------ Error packet ------------------------------------------------------

ErrorPacket::ErrorPacket( bool bBigEndian /* = true */ ) : StringPacket( bBigEndian )
{
	InitInstance();
}/* ErrorPacket */

ErrorPacket::ErrorPacket( const Packet & packet ) : StringPacket( packet )
{
}

ErrorPacket::ErrorPacket( const string& str, bool bBigEndian /* = true */ ) : StringPacket( str, bBigEndian )
{
	InitInstance();
	SetString( str );
}

ErrorPacket::ErrorPacket( const char * str, bool bBigEndian /* = true */ ) : StringPacket( str, bBigEndian )
{
	InitInstance();
	SetString( str );
}

void ErrorPacket::InitInstance()
{
	m_nType = PKT_TYPE_ERROR;
}/* InitializeInstance */


/***************************************************************************************
  Name: SetString
  
  Returns: void 
  Argument: const char *pStr
  
  Description: adds "ERROR-" in front of the string 
  ***************************************************************************************/
void ErrorPacket::SetString( const char *pStr)
{
	char *pModString = new char[ strlen( pStr ) + 1 + strlen( ERROR_STRING_PREFIX ) ];
	
	sprintf( pModString, "%s%s", ERROR_STRING_PREFIX, pStr );
	StringPacket::SetString( pModString );

	delete pModString;
}/* SetString */


/***************************************************************************************
  Name: SetString
  
  Returns: void 
  Argument: const string& str
  
  Description: 
  ***************************************************************************************/
void ErrorPacket::SetString( const string& str )
{
	SetString( str.c_str() );
}/* SetString */