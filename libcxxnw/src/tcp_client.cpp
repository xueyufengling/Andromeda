#include <cxxnw/tcp_client.h>

#include <cxxcomm/log.h>

using namespace cxxnw;

void tcp_client::connect(const char* ip, const char* service_name)
{
	resolver.async_resolve(ip, service_name, [this, ip, service_name](boost::system::error_code ec, boost::asio::ip::tcp::resolver::results_type results) -> void
			{
				if (ec)
				{
					LogError("resolve ip failed: ", ec.what().c_str())
				}
				else
				{
					boost::asio::ip::tcp::socket* sckt = new boost::asio::ip::tcp::socket(*io_context);
					session.socket = sckt;
					boost::asio::async_connect(*sckt, results, [this, ip, service_name](boost::system::error_code e, boost::asio::ip::tcp::endpoint endpoint) -> void
							{
								if (e)
								{
									LogError("connect failed: ", e.what().c_str());
								}
								else
								{
									LogDebugInfo("connected to server ", ip, ':', service_name);
									if(this->connect_callback)
									{
										connect_callback(e, endpoint);
									}
								}
							});
				}
			});
	io_context->run();
}
