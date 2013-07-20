#ifndef __BOOST_COMPUTATIONAL_TASK_MANAGER_H__
#define __BOOST_COMPUTATIONAL_TASK_MANAGER_H__

#include <iostream>
#include <time.h>
#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <utils/gstl_types.h>

#include "computational_task.h"
#include "computational_task_manager.h"
#include "exception.h"


/**
 * Boost_computational_task_manager uses Boost threads to execute tasks. 
 */
class Boost_computational_task_manager : public Computational_task_manager
{
public:
  typedef boost::shared_ptr<Boost_computational_task_manager> Ptr;
  struct Boost_task_handler : Computational_task::Task_handler
  {
    boost::thread thread;
  };


public:
  /* Add a task to the task pool */
  const Computational_task::Task_handler * task_add(const Computational_task::Ptr & _task);

  /* Run a task */
  int task_run(const Computational_task::Task_handler & _task_handler);

  /* Wait for a task to finish */
  int task_join(const Computational_task::Task_handler & _task_handler);

  /* Finalize a task (send task to exit) */
  int task_finalize(const Computational_task::Task_handler & _task_handler);

  /* Scheduling: return the nunber of parallel threads/processes that are assigned to a parallel program */
  int number_parallel_tasks();

  // factory method
  static Computational_task_manager::Ptr singleton_instance();
  /*static Boost_computational_task_manager::Ptr
  Ptr_new()
  {
    Boost_computational_task_manager::Ptr ptr(new Boost_computational_task_manager());
    return ptr;
  }*/

  /* destructor */
  ~Boost_computational_task_manager();


protected:
  // a singleton instance
  static Computational_task_manager::Ptr singleton_instance_;


protected:
  /* Start a new thread and run a task in this thread */
  static void thread_new(Computational_task * _task)
  {
    // record start time
    //time_t seconds = time (NULL);
    //_task->start_time_is(seconds);

    // run
    _task->run();
    
    // record end time
    //seconds = time (NULL);
    //_task->end_time_is(seconds);
    // LOGGING, please !
    //std::cout << "Task " << task << " STARTed at " << task->start_time() << " and ENDed at " << task->end_time() << std::endl;
  }

  /* constructor */
  Boost_computational_task_manager();
};




#endif