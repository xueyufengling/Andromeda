#ifndef ANDROMEDA_NETWORK_TCPCLIENT
#define ANDROMEDA_NETWORK_TCPCLIENT

#include <boost/asio.hpp>

#include "session.h"

namespace andromeda {
	namespace network {
		class TcpClient
		{
		private:
			boost::asio::io_context* io_context;
			boost::asio::ip::tcp::resolver resolver;
			TcpSession session;

		public:
			std::function<void(boost::system::error_code ec, boost::asio::ip::tcp::endpoint)> connect_callback;

			TcpClient(boost::asio::io_context* io_ctx=new boost::asio::io_context()) :
					io_context(io_ctx), resolver(*io_ctx)
			{

			}

			void connect(const char* ip,const char* service_name);
			inline void connect(const char* ip,int port)
			{
				connect(ip,std::to_string(port).c_str());
			}

			inline TcpClient& post(std::vector<unsigned char> msg)
			{
				session.post(msg);
				return *this;
			}

			inline TcpClient& setRecvCallback(std::function<void(boost::system::error_code ec, boost::asio::ip::tcp::endpoint)> recv_cb)
			{
				session.recv_callback=recv_cb;
				return *this;
			}
		};

	}
}

#endif //ANDROMEDA_NETWORK_TCPCLIENT
