#include "computational_task_manager.h"
#include "boost_computational_task_manager.h"


/* a static method */
Computational_task_manager::Ptr
Computational_task_manager::manager()
{
  /* Windows Thread */
  //return Win_computational_task_manager::singleton_instance();
  /* Boost Thread */
  return Boost_computational_task_manager::singleton_instance();
}
