#include <iostream>

#include <yail/pubsub/service.h>
#include <yail/pubsub/data_writer.h>

#include "topics.h"

#define LOG_INFO(msg) std::cout << msg << std::endl
#define LOG_ERROR(msg) std::cerr << msg << std::endl

using transport = yail::pubsub::transport::shmem;

int main(int argc, char* argv[])
{
	try
	{
		boost::asio::io_service io_service;
		yail::pubsub::service<transport> pubsub_service (io_service);
		yail::pubsub::topic<messages::hello> hello_topic ("greeting");
		yail::pubsub::data_writer<messages::hello, transport> hello_dw (pubsub_service, hello_topic);

		messages::hello value;
		value.set_msg ("hello");
		value.set_seq (1);
		value.set_data (std::string (1024, 'A'));

		hello_dw.async_write (value,
			[ &value ] (const boost::system::error_code &ec)
			{
				if (!ec)
				{
						LOG_INFO (
						"sent: " << std::endl <<
						"msg: " << value.msg () << std::endl <<
						"seq: " << value.seq () << std::endl <<
						"data: " << value.data ());
				}
				else if (ec != boost::asio::error::operation_aborted)
				{
					LOG_ERROR ("error: " << ec);
				}
			});

		boost::asio::signal_set signals (io_service, SIGINT, SIGTERM);
		signals.async_wait (
			[&] (const boost::system::error_code &ec, int signal) 
				{
					io_service.stop (); 
				});

		io_service.run ();
	} 
	catch (const std::exception &ex)
	{
		LOG_ERROR (ex.what ());
	}

  return 0;
}

