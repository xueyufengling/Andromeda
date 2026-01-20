#ifndef _CXXNW_TCPSERVER
#define _CXXNW_TCPSERVER

#include <boost/asio.hpp>

namespace cxxnw
{
class tcp_server
{
private:
	boost::asio::io_context* io_context;
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
