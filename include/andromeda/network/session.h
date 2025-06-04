#ifndef ANDROMEDA_NETWORK_SESSION
#define ANDROMEDA_NETWORK_SESSION

#include <boost/asio.hpp>

#define TCP_SESSION_BUFFER_SIZE 128

namespace andromeda {
	namespace network {
		class TcpServer;
		class TcpClient;
		class TcpSession
		{
			friend class TcpServer;
			friend class TcpClient;
		private:
			boost::asio::ip::tcp::socket* socket=nullptr;
			unsigned char recv_buffer[TCP_SESSION_BUFFER_SIZE];
			std::vector<unsigned char> body; //收到的字节数据

		public:
			std::function<void(TcpSession&,const std::vector<unsigned char>&)> recv_callback;

			operator boost::asio::ip::tcp::socket&()
			{
				return *socket;
			}

			operator boost::asio::ip::tcp::socket*()
			{
				return socket;
			}

			void start(boost::asio::ip::tcp::socket* connection_socket);
			inline void start(boost::asio::ip::tcp::socket& connection_socket)
			{
				start(&connection_socket);
			}

			TcpSession& post(std::vector<unsigned char> response_buf);

			void stop();
		};
	}
}

#endif //ANDROMEDA_NETWORK_SESSION
