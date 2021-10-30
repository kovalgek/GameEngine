#pragma once

#include <vector>
#include "Shared.h"
#include <memory>

class UDPSocket;
class TCPSocket;

typedef std::shared_ptr<UDPSocket> UDPSocketPtr;
typedef std::shared_ptr<TCPSocket> TCPSocketPtr;

enum SocketAddressFamily
{
	INET = AF_INET,
	INET6 = AF_INET6
};

class SocketUtil
{
public:

	static bool			StaticInit();
	static void			CleanUp();

	static void			ReportError( const char* inOperationDesc );
	static int			GetLastError();

	static int			Select(
		const std::vector<TCPSocketPtr>* inReadSet,
		std::vector< TCPSocketPtr >* outReadSet,
		const std::vector< TCPSocketPtr >* inWriteSet,
		std::vector< TCPSocketPtr >* outWriteSet,
		const std::vector< TCPSocketPtr >* inExceptSet,
		std::vector< TCPSocketPtr >* outExceptSet);

	static UDPSocketPtr	CreateUDPSocket( SocketAddressFamily inFamily );
	static TCPSocketPtr	CreateTCPSocket( SocketAddressFamily inFamily );

private:

	inline static fd_set* FillSetFromVector( fd_set& outSet, const std::vector< TCPSocketPtr >* inSockets, int& ioNaxNfds );
	inline static void FillVectorFromSet( std::vector< TCPSocketPtr >* outSockets, const std::vector< TCPSocketPtr >* inSockets, const fd_set& inSet );
};