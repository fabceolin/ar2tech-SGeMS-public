#ifndef __COMPUTATIONAL_TASK_MANAGER_H__
#define __COMPUTATIONAL_TASK_MANAGER_H__

#include <boost/shared_ptr.hpp>

#include "computational_task.h"

#define DEFAULT_NUM_THREADS 4


class Computational_task_manager
{
public:
  typedef boost::shared_ptr<Computational_task_manager> Ptr;


public:
  /* Add a task to the task pool */
  virtual const Computational_task::Task_handler * task_add(const Computational_task::Ptr & _task) = 0;

  /* Run a task */
  virtual int task_run(const Computational_task::Task_handler & _task_handler) = 0;

  /* Wait for a task to finish */
  virtual int task_join(const Computational_task::Task_handler & _task_handler) = 0;

  /* Finalize a task (send task to exit) */
  virtual int task_finalize(const Computational_task::Task_handler & _task_handler) = 0;

  /* Scheduling: return the nunber of parallel threads/processes that are assigned to a parallel program */
  virtual int number_parallel_tasks() = 0;

  Computational_task_manager(void) {}
  ~Computational_task_manager(void) {}


public:
  static Computational_task_manager::Ptr manager();
};


#endif