#ifndef ANDROMEDA_NETWORK_TCPSERVER
#define ANDROMEDA_NETWORK_TCPSERVER

#include <boost/asio.hpp>

namespace andromeda {
	namespace network {
		class TcpServer
		{
		private:
			boost::asio::io_context* io_context;
			boost::asio::ip::tcp::acceptor acceptor;

		public:
			TcpServer(int port,boost::asio::io_context* io_ctx=new boost::asio::io_context()) :
					io_context(io_ctx), acceptor(*io_ctx,boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),port))
			{

			}

			void start();
		};
	}
}

#endif //ANDROMEDA_NETWORK_TCPSERVER
