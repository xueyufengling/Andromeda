#ifndef _CXXNW_TCPSERVER
#define _CXXNW_TCPSERVER

#include <cxxnw/tcp_channel.h>

namespace cxxnw
{
class tcp_server: public tcp_channel
{
private:
	boost::asio::ip::tcp::acceptor acceptor;

public:
	tcp_server(int port, boost::asio::io_context* io_ctx = new boost::asio::io_context()) :
			io_context(io_ctx), acceptor(*io_ctx, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
	{

	}

	void start();
};
}

#endif //_CXXNW_TCPSERVER
