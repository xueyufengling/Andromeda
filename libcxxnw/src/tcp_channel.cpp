#include <cxxnw/tcp_channel.h>

#include <cxxcomm/log.h>

using namespace cxxnw;

boost::asio::ip::tcp::resolver::results_type tcp_channel::resolve(const char* ip, const char* service_name)
{
	return host_resolver.resolve(ip, service_name);
}

void tcp_channel::resolve(const char* ip, const char* service_name, resolve_callback cb)
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
}

void tcp_channel::connect(const char* ip, const char* service_name, connect_callback cb)
{
	resolve(ip, service_name, [this, ip, service_name, cb](boost::system::error_code resolve_ec, boost::asio::ip::tcp::resolver::results_type results) -> void
			{
				this->comm_socket = new boost::asio::ip::tcp::socket(*io_context);
				boost::asio::async_connect(*this->comm_socket, results, [this, ip, service_name](boost::system::error_code connect_ec, boost::asio::ip::tcp::endpoint ep) -> void
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

void tcp_channel::disconnect()
{
	delete comm_socket;
	comm_socket = nullptr;
}

void tcp_channel::read(receive_callback cb)
{
	boost::asio::async_read(*this->comm_socket, boost::asio::buffer(this->recv_buffer, sizeof(uint32_t)), [this, cb](boost::system::error_code ec_header, std::size_t) -> void
			{
				if(ec_header)
				{
					LogError("read header length failed: ", ec_header.what().c_str());
				}
				else
				{
					uint32_t body_length = ntohl(*((uint32_t*)recv_buffer));
					body.resize(body_length, 0);
					boost::asio::async_read(*this->comm_socket, boost::asio::buffer(body), body_length,
							[this, cb](boost::system::error_code ec_body, size_t) -> void
							{
								if(ec_body)
								{
									LogError("read body failed: ",ec_body.what().c_str());
								}
								else
								{
									if(cb)
									{
										cb(body_length, body);
									}
								}
							});
				}
			});
}

tcp_channel& tcp_channel::write(std::vector<unsigned char> bytes)
{
	std::vector < boost::asio::const_buffer > buffers;
	uint32_t resp_length = htonl((uint32_t)(bytes.size()));
	buffers.push_back(boost::asio::buffer(&resp_length, sizeof(resp_length)));
	buffers.push_back(boost::asio::buffer(bytes));
	boost::asio::async_write(*comm_socket, buffers, [this](boost::system::error_code write_ec, std::size_t) -> void
			{
				if(write_ec)
				{
					LogError("write tcp channel ", get_address(), " failed: ",write_ec.what().c_str());
				}
				else
				{
					start(socket);
				}
			});
	return *this;
}
