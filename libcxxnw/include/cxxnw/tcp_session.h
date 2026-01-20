#ifndef _CXXNW_TCPSESSION
#define _CXXNW_TCPSESSION

#include <boost/asio.hpp>

#define TCP_SESSION_BUFFER_SIZE 128

namespace cxxnw
{
class tcp_server;
class tcp_client;
/**
 * 客户端与服务端的一次连接会话上下文，用于数据交换和收发报文
 * 本身不持有socket，仅是socket的使用者
 */
class tcp_session
{
	friend class tcp_server;
	friend class tcp_client;

private:
	boost::asio::ip::tcp::socket* socket = nullptr;
	unsigned char recv_buffer[TCP_SESSION_BUFFER_SIZE];
	std::vector<unsigned char> body; //收到的字节数据

public:

	callback recv_callback;

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

	tcp_session& post(std::vector<unsigned char> response_buf);

	void stop();
};
}

#endif //_CXXNW_TCPSESSION
