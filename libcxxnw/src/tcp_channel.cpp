#include <cxxnw/tcp_channel.h>

#include <cxxcomm/log.h>

using namespace cxxnw;

void tcp_channel::disconnect()
{
	delete comm_socket;
	comm_socket = nullptr;
}

void tcp_channel::read(transfer_callback cb)
{
	boost::asio::async_read(*this->comm_socket, boost::asio::buffer(&this->recv_body_len, sizeof(uint32_t)), [this, cb](boost::system::error_code ec_header, size_t) -> void
			{
				if(ec_header)
				{
					LogError("read header length failed: ", ec_header.what().c_str());
					if(cb)
					{
						cb(ec_header, 0, (unsigned char*)this->recv_buffer);
					}
				}
				else
				{
					recv_body_len = ntohl(this->recv_body_len);
					recv_buffer.reserve(recv_body_len);
					boost::asio::async_read(*this->comm_socket, boost::asio::buffer((char*)recv_buffer, recv_body_len), boost::asio::transfer_exactly(recv_body_len),
							[this, cb](boost::system::error_code ec_body, size_t) -> void
							{
								if(ec_body)
								{
									LogError("read body failed: ",ec_body.what().c_str());
								}
								if(cb)
								{
									cb(ec_body, this->recv_body_len, (unsigned char*)this->recv_buffer);
								}
							});
				}
			});
	io_context->run();
}

void tcp_channel::write(unsigned char* bytes, uint32_t length, transfer_callback cb)
{
	uint32_t write_length = htonl(length);
	boost::asio::async_write(*comm_socket, boost::asio::buffer((char*)bytes, write_length), [bytes, write_length, cb](boost::system::error_code write_ec, size_t) -> void
			{
				if(write_ec)
				{
					LogError("write tcp channel ", get_address(), " failed: ",write_ec.what().c_str());
				}
				if(cb)
				{
					cb(write_ec, write_length, bytes);
				}
			});
	io_context->run();
}
