/******************************************************************************
 *	rtC3Dframe.h
 *	
 * Description: Definition of data structures used in Real Time C3D protocol
 *				based on a Protocol Draft version 003
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
 ********************************************************************************/
#ifndef __C3DFRAME_HEADER_____DEFINED_
#define __C3DFRAME_HEADER_____DEFINED_

// defines needed for ndtypes.h
#ifndef PLATFORM_X86
#define PLATFORM_X86
#endif

#ifndef __WINDOWS_H
#define __WINDOWS_H
#endif

#include <string>
#include "ndtypes-sample.h"

using namespace std;

// packet type definitions
#define PKT_TYPE_ERROR			0
#define PKT_TYPE_COMMAND		1
#define PKT_TYPE_XML			2
#define PKT_TYPE_DATAFRAME		3
#define PKT_TYPE_EMPTY			4
#define PKT_TYPE_C3DFILE		5

// component type definitions
#define COMP_TYPE_3D			1
#define COMP_TYPE_ANALOG		2
#define COMP_TYPE_FORCE			3
#define COMP_TYPE_6D			4
#define COMP_TYPE_EVENT			5

#define PACKET_SEND_BUFF_SIZE	10240	// 10 kB
#define PACKET_RECV_BUFF_SIZE	10240	// 10 kB

#define PACKET_MAX_SIZE			50000	// maximum packet size for sanity check

#define HTTP_REQUEST			-12345
#define ERROR_STRING_PREFIX		"Error-"

// packet class
class Packet
{
public:
	Packet(bool bBigEndian = true);
	Packet( const Packet & packet );
	virtual ~Packet();

	virtual int nReceive( SOCKET soc );
	virtual int nSend( SOCKET soc );
	virtual Packet& operator=(const Packet& other); 

public:
	int m_nSize;
	int m_nType;
	char *m_pData;
	bool m_bNetworkByteOrder;
protected:
	char m_cSendBuffer[PACKET_SEND_BUFF_SIZE];
	char m_cRecvBuffer[PACKET_RECV_BUFF_SIZE];
protected:
	virtual int PackData();
	virtual bool UnPackData();
private:
	void InitializeInstance();
};

typedef struct RawPacketHeader
{
	UINT32 m_size;
	UINT32 m_type;
}RawPacketHeader;

typedef struct RawFrameHeader
{
	UINT32 m_componentcount;
}RawFrameHeader;

typedef struct RawFrameComponentHeader
{
	UINT32 m_size;
	UINT32 m_type;
	UINT32 m_frame;		// frame number
	UINT64 m_timestamp;	// microseconds since the start
}RawFrameComponentHeader;

typedef struct RawFrame3DData
{
	UINT32 m_count;
}RawFrame3DData;

typedef struct RawFrame3DItem
{
	float m_x;
	float m_y;
	float m_z;
	float m_fReliability;

	void SetMissing()
	{
		memset( &m_x, 0xff, sizeof( m_x ) );
		memset( &m_y, 0xff, sizeof( m_y ) );
		memset( &m_z, 0xff, sizeof( m_z ) );
	}

	bool bIsMissing() const
	{
		bool bRet = false;
		float ftmp;
		memset( &ftmp, 0xff, sizeof( ftmp ) );
		bRet = ( memcmp( &m_x, &ftmp, sizeof( ftmp ) ) == 0 );
		return bRet;
	}
}RawFrame3DItem;

typedef struct RawFrameAnalogData
{
	UINT32 m_count;
}RawFrameAnalogData;

typedef struct RawFrameAnalogItem
{
	float m_v;

	void SetMissing()
	{
		memset( &m_v, 0xff, sizeof( m_v ) );
	}

	bool bIsMissing() const
	{
		bool bRet = false;
		float ftmp;
		memset( &ftmp, 0xff, sizeof( ftmp ) );
		bRet = ( memcmp( &m_v, &ftmp, sizeof( ftmp ) ) == 0 );
		return bRet;
	}
}RawFrameAnalogItem;

typedef struct RawFrame6DData
{
	UINT32 m_count;
}RawFrame6DData;

typedef struct RawFrame6DItem
{
	float m_q0;
	float m_qx;
	float m_qy;
	float m_qz;
	float m_x;
	float m_y;
	float m_z;
	float m_RMSerror;

	void SetMissing()
	{
		memset( &m_q0, 0xff, sizeof( m_q0 ) );
		memset( &m_qx, 0xff, sizeof( m_qx ) );
		memset( &m_qy, 0xff, sizeof( m_qy ) );
		memset( &m_qz, 0xff, sizeof( m_qz ) );
		memset( &m_x, 0xff, sizeof( m_x ) );
		memset( &m_y, 0xff, sizeof( m_y ) );
		memset( &m_z, 0xff, sizeof( m_z ) );
		memset( &m_RMSerror, 0xff, sizeof( m_RMSerror ) );
	}

	bool bIsMissing() const
	{
		bool bRet = false;
		float ftmp;
		memset( &ftmp, 0xff, sizeof( ftmp ) );
		bRet = ( memcmp( &m_q0, &ftmp, sizeof( ftmp ) ) == 0 );
		return bRet;
	}
}RawFrame6DItem;

typedef struct RawFrameForceData
{
	UINT32 m_count;		// number of plates
}RawFrameForceData;

typedef struct RawFrameForceItem
{
	float m_Fx;
	float m_Fy;
	float m_Fz;
	float m_Mx;
	float m_My;
	float m_Mz;

	void SetMissing()
	{
		memset( &m_Fx, 0xff, sizeof( m_Fx ) );
		memset( &m_Fy, 0xff, sizeof( m_Fy ) );
		memset( &m_Fz, 0xff, sizeof( m_Fz ) );
		memset( &m_Mx, 0xff, sizeof( m_Mx ) );
		memset( &m_My, 0xff, sizeof( m_My ) );
		memset( &m_Mz, 0xff, sizeof( m_Mz ) );
	}

	bool bIsMissing() const
	{
		bool bRet = false;
		float ftmp;
		memset( &ftmp, 0xff, sizeof( ftmp ) );
		bRet = ( memcmp( &m_Fx, &ftmp, sizeof( ftmp ) ) == 0 );
		return bRet;
	}
}RawFrameForceItem;

typedef struct RawFrameEventData
{
	UINT32 m_count;
}RawFrameEventData;

typedef struct RawFrameEventItem
{
	UINT32 m_id;
	UINT64 m_TimeStamp;		// micro seconds
	UINT32 m_param1;
	UINT32 m_param2;
	UINT32 m_param3;
	
	void SetMissing()
	{
		memset( &m_param1, 0xff, sizeof( m_param1 ) );
		memset( &m_param2, 0xff, sizeof( m_param2 ) );
		memset( &m_param3, 0xff, sizeof( m_param3 ) );
	}

	bool bIsMissing() const
	{
		bool bRet = false;
		float ftmp;
		memset( &ftmp, 0xff, sizeof( ftmp ) );
		bRet = ( memcmp( &m_param1, &ftmp, sizeof( ftmp ) ) == 0 );
		return bRet;
	}

}RawFrameEvenetItem;

class DataFrame: public Packet
{
public:
	DataFrame( const Packet & );
	DataFrame( bool bBigEndian = 1);
	virtual ~DataFrame();
	virtual DataFrame & operator=(const DataFrame &);
	virtual int PackData();

	void SetNum3d( int n3d);
	void SetNum6d( int n6d);
	void SetNumAnalog( int nAnalog);
	void SetNumForce( int nForce );
	void SetNumEvent( int nEvent );

	int GetNum3d() const;
	int GetNum6d() const;
	int GetNumForce() const;
	int GetNumAnalog() const;
	int GetNumEvent() const;

	virtual int nReceive( SOCKET soc );
	virtual int nSend( SOCKET soc );

	int m_n3dFrame;
	int m_n6dFrame;
	int m_nForceFrame;
	int m_nAnalogFrame;
	int m_nEventFrame;
	
	double m_lf3DTimeStamp;		// ms
	double m_lf6DTimeStamp;		// ms
	double m_lfAnalogTimeStamp;	// ms
	double m_lfForceTimeStamp;	// ms
	double m_lfEventTimeStamp;	// ms

	Position3d *m_p3d;
	QuatErrorTransformation *m_p6d;
	float *m_pfAnalog;
	ForcePlate *m_pForce;
	C3DEvent *m_pEvents;
protected:
	int m_n3d;
	int m_n6d;
	int m_nForce;
	int m_nAnalog;
	int m_nEvent;
protected:
	virtual bool UnPackData();
private:
	void InitializeInstance();

};

class StringPacket: public Packet
{
public:
	StringPacket( bool bBigEndian = true );
	StringPacket( const Packet &packet );
	StringPacket( const string& str, bool bBigEndian = true );
	StringPacket( const char * str, bool bBigEndian = true );

	string GetString();
	virtual void SetString( const string& str );
	virtual void SetString( const char *pStr);
protected:
	virtual int PackData();
	virtual bool UnPackData();
private:
	virtual void InitInstance();
	string m_sString;
};

class XMLPacket: public StringPacket
{
public:
	XMLPacket( bool bBigEndian = true );
	XMLPacket( const Packet &packet );
	XMLPacket( const string& str, bool bBigEndian = true );
	XMLPacket( const char * str, bool bBigEndian = true );
private:
	virtual void InitInstance();
};

class ErrorPacket: public StringPacket
{
public:
	ErrorPacket( bool bBigEndian = true );
	ErrorPacket( const Packet &packet );
	ErrorPacket( const string& str, bool bBigEndian = true );
	ErrorPacket( const char * str, bool bBigEndian = true );
	virtual void SetString( const char *pStr);
	virtual void SetString( const string& str );
private:
	virtual void InitInstance();
};

class EmptyPacket: public Packet
{
public:
	EmptyPacket();
};

#endif // __C3DFRAME_HEADER_____DEFINED_