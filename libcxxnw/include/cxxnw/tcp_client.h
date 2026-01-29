#ifndef _CXXNW_TCPCLIENT
#define _CXXNW_TCPCLIENT

#include <cxxnw/tcp_channel.h>

namespace cxxnw
{
class tcp_client: public tcp_channel
{
private:
	boost::asio::ip::tcp::endpoint end_point;
	boost::asio::ip::tcp::resolver host_resolver; //解析IP和端口

public:
	tcp_client(size_t buffer_len = __TCP_CHANNEL_DEFAULT__BUFFER_SIZE__, boost::asio::io_context* io_ctx = new boost::asio::io_context()) :
			channel(buffer_len, io_ctx)
	{

	}

	/**
	 * @brief 同步解析IP
	 */
	boost::asio::ip::tcp::resolver::results_type resolve(const char* ip, const char* service_name);

	/**
	 * @brief 异步解析IP
	 */
	void resolve(const char* ip, const char* service_name, resolve_callback cb);

	void connect(const char* ip, const char* service_name, connect_callback cb = connect_callback());

	inline void connect(const char* ip, int port, connect_callback cb = connect_callback())
	{
		connect(ip, to_string(port).c_str(), cb);
	}

	/**
	 * @brief 当已连接时，获取IP和端口
	 */
	inline boost::asio::ip::address get_address()
	{
		return end_point.address();
	}
};
}

#endif //_CXXNW_TCPCLIENT
