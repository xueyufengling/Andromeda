#include <cxxnw/tcp_session.h>

#include <cxxcomm/log.h>

using namespace cxxnw;

void tcp_session::start(boost::asio::ip::tcp::socket* connection_socket)
{
	socket = connection_socket;
	boost::asio::async_read(*socket, boost::asio::buffer(recv_buffer, sizeof(uint32_t)), [this](boost::system::error_code ec_header, std::size_t) -> void
			{
				if (ec_header)
				{
					LogError("read header length failed: ", ec_header.what().c_str());
				}
				else
				{
					uint32_t body_length = ntohl(*((uint32_t*)recv_buffer));
					body.resize(body_length, 0);
					boost::asio::async_read(*socket, boost::asio::buffer(body),
							[this](boost::system::error_code ec_body, std::size_t) -> void
							{
								if (!ec_body)
								{
									if(recv_callback)
									{
										recv_callback(*this,body);
									}
								}
								else
								LogError("read body failed: ",ec_body.what().c_str());
							});
				}
			});
}

void tcp_session::stop()
{
	socket = nullptr;
}

