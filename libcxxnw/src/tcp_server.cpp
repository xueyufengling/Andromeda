#include <cxxnw/tcp_server.h>

#include <cxxcomm/log.h>
#include <cxxnw/tcp_session.h>

using namespace cxxnw;

// @formatter:off

void tcp_server::start()
{
	boost::asio::ip::tcp::socket socket(*io_context);
	acceptor.async_accept(socket,[this](boost::system::error_code ec,boost::asio::ip::tcp::socket socket)
	{
		if (!ec)
		{
			tcp_session().start(socket);
		}
		start();
	});
	io_context->run();
}

// @formatter:on
