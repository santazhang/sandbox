#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

void print(const boost::system::error_code& e)
{
  std::cout << "Hello, world!\n";
}

int main()
{
  boost::asio::io_service io;
  boost::asio::deadline_timer t(io, boost::posix_time::seconds(5));
  t.async_wait(print);
  sleep(2);
  std::cout << "LINE" << std::endl;
  io.run();
  return 0;
}
