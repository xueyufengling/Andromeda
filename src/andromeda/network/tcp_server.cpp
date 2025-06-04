#include <andromeda/network/tcp_server.h>

#include <andromeda/network/session.h>
#include <andromeda/util/log.h>

using namespace andromeda::network;

// @formatter:off

void TcpServer::start()
{
	boost::asio::ip::tcp::socket socket(*io_context);
	acceptor.async_accept(socket,[this](boost::system::error_code ec,boost::asio::ip::tcp::socket socket)
	{
		if (!ec)
		{
			TcpSession().start(socket);
		}
		start();
	});
	io_context->run();
}

// @formatter:on
