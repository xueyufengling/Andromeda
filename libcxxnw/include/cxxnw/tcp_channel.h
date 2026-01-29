#ifndef _CXXNW_TCPCHANNEL
#define _CXXNW_TCPCHANNEL

/**
 * 本头文件依赖于Boost库的ASIO模块
 * pacman -S mingw-w64-ucrt-x86_64-boost
 */
#include <cxxcomm/string_utils.h>
#include <cxxcomm/array.h>
#include <functional>
#include <malloc.h>

#include <boost/asio.hpp>

template<>
__attribute__((always_inline)) inline std::string to_string(const boost::asio::ip::address& value)
{
	return value.to_string();
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
protected:
	bool io_ctx_self_alloc;
	boost::asio::io_context* io_context;
	boost::asio::ip::tcp::socket* comm_socket = nullptr; //通信使用的socket对象，解析IP和端口完成后分配
	//接收缓冲区
	cxxcomm::buffer recv_buffer; //该缓冲区是否是本类自行分配，如果是则析构时释放缓冲区
	uint32_t recv_body_len = 0; //接收到的报文体长度

public:
	typedef std::function<void(boost::system::error_code, boost::asio::ip::tcp::endpoint)> connect_callback;
	typedef std::function<void(boost::system::error_code, boost::asio::ip::tcp::resolver::results_type)> resolve_callback;
	/**
	 * @brief 读取、写入的回调
	 * @param uint32_t 实际读入/写入的字节数
	 * @param unsigned char* 读取/写入的缓存区
	 */
	typedef std::function<void(boost::system::error_code, uint32_t, unsigned char*)> transfer_callback;

	inline tcp_channel(size_t buffer_len, boost::asio::io_context* io_ctx) :
			io_context(io_ctx), host_resolver(*io_ctx), recv_buffer(buffer_len), io_ctx_self_alloc(false)
	{

	}

	inline tcp_channel(size_t buffer_len = __TCP_CHANNEL_DEFAULT__BUFFER_SIZE__) :
			tcp_channel(buffer_len, new boost::asio::io_context()), io_ctx_self_alloc(true)
	{

	}

	template<typename _SettableSocketOption>
	inline void set_option(const _SettableSocketOption& option)
	{
		comm_socket->set_option(option);
	}

	inline bool is_connected()
	{
		return comm_socket;
	}

	void disconnect();

	void read(transfer_callback cb);

	void write(unsigned char* bytes, uint32_t length, transfer_callback cb = transfer_callback());

	template<typename _T>
	void write(std::vector<_T> vec, transfer_callback cb = transfer_callback())
	{
		write(vec.data(), (uint32_t)(sizeof(_T) * vec.size()), cb);
	}
};
}

#endif //_CXXNW_TCPCHANNEL
