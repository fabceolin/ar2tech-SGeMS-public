#ifndef __COMPUTATIONAL_TASK_H__
#define __COMPUTATIONAL_TASK_H__

#include <boost/shared_ptr.hpp>
#include <utils/gstl_types.h>



/**
 * Computational_task encapsulates data and a processing method, run(),
 * that can be executed by a computational resource, such as thread or
 * process.
 */
class Computational_task
{
public:
  /* smart pointer of this class object */
  typedef boost::shared_ptr<Computational_task> Ptr;

  /*
   * Task_handler represents a unique identity of a task in the system.
   * Task_handler is exactly the same thread/process handler provided by platforms.
   * For example, Windows Thread uses HANDLE as unique id for windows threads,
   * Boost library uses boost::thread as unique id for boost threads.
   *
   * Different platforms have different ways to identify thread/process, and thus
   * have to derive a class from base class Task_handler.
   *
   * The Task_handler attribute of each task is assigned by the specific platform
   * that executes the task.
   */
  struct Task_handler {};

  /*
   * Task_sequence_id is used to identify tasks in each parallel program separately.
   * For example, if a program spawns 4 parallel tasks (threads/processes), it can 
   * assign a unique Task_sequence_id to each task, such as 0, 1, 2, and 3. The 
   * sequence ids will then be used when each individual task (thread/process)
   * sends the data to the master task (thread/process).
   *
   * Task_sequence_id is thus not unique acrossing programs in the system.
   *
   * Derived classes of Computational_task can implement different data structure 
   * to identify tasks and thus do not need to use Task_sequence_id.
   */
  typedef GsTLInt Task_sequence_id;
  
  /* Time is used to track the performance of tasks. */
  typedef GsTLLong Time;


public:
  /*
   * This is the main function that executes the task.
   * A thread or process will call this run() function.
   */
  virtual int run() = 0;

  /* Get and set starting time */
  Time start_time() const { return start_time_; }
  void start_time_is(Time _start_time) { start_time_ = _start_time; }

  /* Get and set ending time */
  Time end_time() const { return end_time_; }
  void end_time_is(Time _end_time) { end_time_ = _end_time; }

protected:
  /* Time attributes are used to track the performance of tasks. */
  Time start_time_; // in seconds
  Time end_time_; // in seconds
};


#endif // __COMPUTATIONAL_TASK_H__