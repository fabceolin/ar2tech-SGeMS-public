#ifndef __BOOST_UTILITY_H__
#define __BOOST_UTILITY_H__

#include <iostream>
#include <time.h>
#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <utils/gstl_types.h>


/**
 * Boost_utility
 */
class Boost_utility
{
public:
  static void sleep(int _milliseconds);

  ~Boost_utility() {}
  Boost_utility() {}
};



#endif