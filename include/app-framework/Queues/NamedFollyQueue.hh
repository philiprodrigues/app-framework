#ifndef APP_FRAMEWORK_INCLUDE_APP_FRAMEWORK_QUEUES_NAMEDFOLLYQUEUE_HH_
#define APP_FRAMEWORK_INCLUDE_APP_FRAMEWORK_QUEUES_NAMEDFOLLYQUEUE_HH_

/**
 *
 * @file NamedQueue wrapper around folly::DMPMCQueue
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "app-framework-base/NamedQueueI.hh"

#include "folly/concurrency/DynamicBoundedQueue.h"

#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <functional>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>

namespace appframework {

template<class T>
class NamedFollyQueue
  : public NamedQueueI<T>
{
public:
  using value_type = T;
  using duration_type = typename NamedQueueI<T>::duration_type;

  NamedFollyQueue(const std::string& name, size_t size)
    : NamedQueueI<T>(name),
      fMaxSize(size),
      fQueue(size)
  {}

  bool can_pop() const noexcept override { return fQueue.size() > 0; }
  value_type pop(const duration_type& dur) override // Throws std::runtime_error if a timeout occurs
  {
    value_type t;
    if(!fQueue.try_dequeue_for(t, dur)){
      throw std::runtime_error("pop timed out");
    }
    return t;
  }
  
  bool can_push() const noexcept override
  {
    return fQueue.size() < fMaxSize;
  }

  void push(value_type&& t, const duration_type& dur)  override // Throws std::runtime_error if a timeout occurs
  {
    if(!fQueue.try_enqueue_for(t, dur)){
      throw std::runtime_error("push timed out");
    }
  }
   

  // Delete the copy and move operations since various member data instances
  // (e.g., of std::mutex or of std::atomic) aren't copyable or movable

  NamedFollyQueue(const NamedFollyQueue&) = delete;
  NamedFollyQueue& operator=(const NamedFollyQueue&) = delete;
  NamedFollyQueue(NamedFollyQueue&&) = delete;
  NamedFollyQueue& operator=(NamedFollyQueue&&) = delete;

private:
  
  const size_t fMaxSize;
  folly::DMPMCQueue<T, false> fQueue;
};

} // namespace appframework

#endif // APP_FRAMEWORK_INCLUDE_APP_FRAMEWORK_QUEUES_NAMEDFOLLYQUEUE_HH_


// Local Variables:
// c-basic-offset: 2
// End:
