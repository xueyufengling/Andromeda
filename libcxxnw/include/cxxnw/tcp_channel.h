#ifndef _CXXNW_TCPCHANNEL
#define _CXXNW_TCPCHANNEL

/**
 * 本头文件依赖于Boost库的ASIO模块
 * pacman -S mingw-w64-ucrt-x86_64-boost
 */
#include <cxxcomm/string_utils.h>
#include <functional>
#include <malloc.h>

#include <boost/asio.hpp>

template<>
__attribute__((always_inline)) inline std::string to_string(const boost::asio::ip::address& value)
{
	std::stringstream ss;
	ss << value.ip() << ':' << value.port();
	return ss.str();
}

//默认接收缓冲区大小
#define __TCP_CHANNEL_DEFAULT__BUFFER_SIZE__ 65536

namespace cxxnw
{
/**
 * @brief TCP通道，可以写入和读取
 */
class tcp_channel
{
private:
	boost::asio::ip::tcp::endpoint end_point;
	boost::asio::ip::tcp::resolver host_resolver; //解析IP和端口
	bool io_ctx_self_alloc = false;
	boost::asio::io_context* io_context;
	boost::asio::ip::tcp::socket* comm_socket = nullptr; //通信使用的socket对象，解析IP和端口完成后分配
	//接收缓冲区
	boost::asio::streambuf recv_buffer;	//该缓冲区是否是本类自行分配，如果是则析构时释放缓冲区
	size_t buffer_len; //缓冲区大小

public:
	typedef std::function<void(boost::system::error_code, boost::asio::ip::tcp::endpoint)> connect_callback;
	typedef std::function<void(boost::system::error_code, boost::asio::ip::tcp::resolver::results_type)> resolve_callback;
	typedef std::function<void(boost::system::error_code, uint32_t body_length,)> receive_callback;

	inline tcp_channel(unsigned char* recv_buffer, size_t buffer_len, boost::asio::io_context* io_ctx = new boost::asio::io_context()) :
			io_context(io_ctx), host_resolver(*io_ctx), self_alloc(false), recv_buffer(recv_buffer), buffer_len(buffer_len)
	{

	}

	inline tcp_channel(size_t buffer_len = __TCP_CHANNEL_DEFAULT__BUFFER_SIZE__, boost::asio::io_context* io_ctx = new boost::asio::io_context()) :
			io_context(io_ctx), host_resolver(*io_ctx), self_alloc(true), recv_buffer((unsigned char*)malloc(buffer_len)), buffer_len(buffer_len)
	{

	}

	inline bool set_option()
	{
		comm_socket->set_option(boost::asio::ip::tcp::no_delay(true));
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

	inline bool is_connected()
	{
		return comm_socket;
	}

	void disconnect();

	/**
	 * @brief 当已连接时，获取IP和端口
	 */
	inline boost::asio::ip::address get_address()
	{
		return end_point.address();
	}

	void read(receive_callback cb);

	tcp_channel& write(std::vector<unsigned char> bytes);
};
}

#endif //_CXXNW_TCPCHANNEL
