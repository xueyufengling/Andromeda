#include <andromeda/network/session.h>

#include <andromeda/util/log.h>

using namespace andromeda::network;

// @formatter:off

TcpSession& TcpSession::post(std::vector<unsigned char> response_buf)
{
    std::vector<boost::asio::const_buffer> buffers;
    uint32_t resp_length = htonl((uint32_t)(response_buf.size()));
    buffers.push_back(boost::asio::buffer(&resp_length, sizeof(resp_length)));
    buffers.push_back(boost::asio::buffer(response_buf));
    boost::asio::async_write(*socket, buffers,[this](boost::system::error_code ec_write, std::size_t)
	{
    	if(!ec_write)
    	{
    		start(socket);
        }
    	else
    		LOG_ERROR("Write response failed, ",ec_write.what().c_str());
    });
	return *this;
}

void TcpSession::start(boost::asio::ip::tcp::socket* connection_socket)
{
	socket=connection_socket;
	boost::asio::async_read(*socket,boost::asio::buffer(recv_buffer,sizeof(uint32_t)),[this](boost::system::error_code ec_header,std::size_t)
	{
		if (!ec_header)
		{
			uint32_t body_length = ntohl(*((uint32_t*)recv_buffer)); //获取报文body长度
			body.resize(body_length,0);
			asio::async_read(*socket, boost::asio::buffer(body),
			[this](boost::system::error_code ec_body, std::size_t)
			{
				if (!ec_body)
				{
					if(recv_callback)
					{
						recv_callback(*this,body);//处理收到的TCP消息
					}
				}
				else
					LOG_ERROR("Read body failed, ",ec_body.what().c_str());
			});
		}
		else
			LOG_ERROR("Read header length failed, ",ec_header.what().c_str());
	});
}

void TcpSession::stop()
{
	socket=nullptr;
}

// @formatter:on
