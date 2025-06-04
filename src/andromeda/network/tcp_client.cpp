#include <andromeda/network/tcp_client.h>

#include <andromeda/util/log.h>

using namespace andromeda::network;

// @formatter:off

void TcpClient::connect(const char* ip,const char* service_name)
{
	resolver.async_resolve(ip,service_name,[this](boost::system::error_code ec,boost::asio::ip::tcp::resolver::results_type results)
	{
		if (!ec)
		{
			boost::asio::ip::tcp::socket socket(*io_context);//为客户端创建socket
			session.socket=&socket;
			boost::asio::async_connect(session.socket, results,[this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint endpoint)
			{
				if(connect_callback)
					connect_callback(ec,endpoint);
				if (!ec)
				{
					LOG_DEBUG("Connected to server ",ip,':',service_name);
				}
				else
				    LOG_ERROR("Connect failed, ",ec.what().c_str());
			});
		}
	});
	io_context->run();
}

// @formatter:on
