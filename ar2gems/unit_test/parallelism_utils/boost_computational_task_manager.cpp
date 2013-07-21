#include "boost_computational_task_manager.h"


const Computational_task::Task_handler *
Boost_computational_task_manager::task_add(const boost::shared_ptr<Computational_task> & _task)
{
  Boost_task_handler * task_handler = new Boost_task_handler();
  if ( !task_handler )
  {
    throw MemoryException("Boost_computational_task_manager::task_add(): Tash handler instantiation fails.");
  }
  task_handler->thread = boost::thread(thread_new, _task.get());
  return task_handler;
}

int
Boost_computational_task_manager::task_run(const Computational_task::Task_handler & _task_handler)
{
  return 0;
}

int
Boost_computational_task_manager::task_join(const Computational_task::Task_handler & _task_handler)
{
  ((Boost_task_handler &)_task_handler).thread.join();
  return 0;
}

int 
Boost_computational_task_manager::task_finalize(const Computational_task::Task_handler & _task_handler)
{
  return 0;
}

int 
Boost_computational_task_manager::number_parallel_tasks()
{
  return DEFAULT_NUM_THREADS;
}

Boost_computational_task_manager::~Boost_computational_task_manager()
{}

Boost_computational_task_manager::Boost_computational_task_manager()
{}


/* singleton_instance */
Computational_task_manager::Ptr Boost_computational_task_manager::singleton_instance_ = Computational_task_manager::Ptr();

Computational_task_manager::Ptr
Boost_computational_task_manager::singleton_instance()
{
  if (!singleton_instance_)
  {
    singleton_instance_ = Boost_computational_task_manager::Ptr( new Boost_computational_task_manager() );
  }
  return singleton_instance_;
}