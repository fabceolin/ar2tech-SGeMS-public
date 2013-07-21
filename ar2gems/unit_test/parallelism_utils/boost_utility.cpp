#include "boost_utility.h"

void 
Boost_utility::sleep(int _milliseconds)
{
  boost::this_thread::sleep(boost::posix_time::milliseconds(_milliseconds)); 
}