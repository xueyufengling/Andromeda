#include <cxxnw/tcp_client.h>

#include <cxxcomm/log.h>

using namespace cxxnw;

boost::asio::ip::tcp::resolver::results_type tcp_client::resolve(const char* ip, const char* service_name)
{
	return host_resolver.resolve(ip, service_name);
}

void tcp_client::resolve(const char* ip, const char* service_name, resolve_callback cb)
{
	host_resolver.async_resolve(ip, service_name, [this, ip, service_name, cb](boost::system::error_code resolve_ec, boost::asio::ip::tcp::resolver::results_type results) -> void
			{
				if (resolve_ec)
				{
					LogError("resolve ip failed: ", resolve_ec.what().c_str())
				}
				if(cb)
				{
					cb(resolve_ec, results);
				}
			});
	io_context->run();
}

void tcp_client::connect(const char* ip, const char* service_name, connect_callback cb)
{
	resolve(ip, service_name, [this, ip, service_name, cb](boost::system::error_code resolve_ec, boost::asio::ip::tcp::resolver::results_type results) -> void
			{
				this->comm_socket = new boost::asio::ip::tcp::socket(*io_context);
				boost::asio::async_connect(*this->comm_socket, results, [this, ip, service_name, cb](boost::system::error_code connect_ec, boost::asio::ip::tcp::endpoint ep) -> void
						{
							if (connect_ec)
							{
								LogError("connect failed: ", connect_ec.what().c_str());
							}
							else
							{
								LogDebugInfo("connected to server ", ip, ':', service_name);
							}
							if(cb)
							{
								cb(connect_ec, ep);
							}
						});
			});
	io_context->run(); //阻塞等待全部异步操作执行完毕
}
