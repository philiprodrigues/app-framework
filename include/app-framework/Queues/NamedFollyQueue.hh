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

namespace appframework {

// TODO: I'd really like FollyQueueType to have a `T` in it somewhere,
// so we can't instantiate this class with a folly queue containing a
// type other than T, but I can't work out the necessary C++
// incantation, so this way it is, for now
template<class T, class FollyQueueType>
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
  FollyQueueType fQueue;
};

template<typename T>
using NamedFollySPSCQueue = NamedFollyQueue<T, folly::DSPSCQueue<T, false>>;

template<typename T>
using NamedFollyMPMCQueue = NamedFollyQueue<T, folly::DMPMCQueue<T, false>>;

} // namespace appframework

#endif // APP_FRAMEWORK_INCLUDE_APP_FRAMEWORK_QUEUES_NAMEDFOLLYQUEUE_HH_


// Local Variables:
// c-basic-offset: 2
// End:
