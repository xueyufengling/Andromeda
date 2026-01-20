#ifndef _CXXNW_TCPCLIENT
#define _CXXNW_TCPCLIENT

#include <cxxcomm/string_utils.h>

#include <boost/asio.hpp>

#include <cxxnw/tcp_session.h>

namespace cxxnw
{
class tcp_client
{
private:
	boost::asio::io_context* io_context;
	boost::asio::ip::tcp::resolver resolver;
	tcp_session session;

public:
	tcp_session::callback connect_callback;

	tcp_client(boost::asio::io_context* io_ctx = new boost::asio::io_context()) :
			io_context(io_ctx), resolver(*io_ctx)
	{

	}

	void connect(const char* ip, const char* service_name);

	inline void connect(const char* ip, int port)
	{
		connect(ip, to_string(port).c_str());
	}

	inline tcp_client& post(std::vector<unsigned char> msg)
	{
		session.post(msg);
		return *this;
	}

	inline tcp_client& setRecvCallback(tcp_session::callback recv_cb)
	{
		session.recv_callback = recv_cb;
		return *this;
	}
};
}

#endif //_CXXNW_TCPCLIENT
