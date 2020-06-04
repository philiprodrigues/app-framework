#ifndef APP_FRAMEWORK_INCLUDE_APP_FRAMEWORK_QUEUES_FOLLYQUEUE_HH_
#define APP_FRAMEWORK_INCLUDE_APP_FRAMEWORK_QUEUES_FOLLYQUEUE_HH_

/**
 *
 * @file QueueI wrapper around folly::UnboundedQueue
 *
 * The relevant types for users are FollySPSCQueue and FollyMPMCQueue,
 * which use the corresponding SPSC/MPMC specializations of
 * folly::UnboundedQueue
 *
 * Folly provides a few queue implementations (ProducerConsumerQueue,
 * MPMCQueue, and DynamicBoundedQueue). We use UnboundedQueue here
 * because it's the only one that provides a pop function with
 * signature like `T pop()` rather than `bool pop(T&)`. The latter
 * signature is difficult to make work with QueueI<T>'s pop signature
 * and non-default-constructible types, so for now I've gone with
 * UnboundedQueue in order to have *something*. In the long term, we
 * probably don't want unbounded queues in the application framework
 * though...
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "app-framework/QueueI.hh"

#include "folly/concurrency/UnboundedQueue.h"

namespace appframework {

// TODO: I'd really like FollyQueueType to have a `T` in it somewhere,
// so we can't instantiate this class with a folly queue containing a
// type other than T, but I can't work out the necessary C++
// incantation, so this way it is, for now
template<class T, class FollyQueueType>
class FollyQueue
  : public QueueI<T>
{
public:
  using value_type = T;
  using duration_type = typename QueueI<T>::duration_type;

  FollyQueue(const std::string& name, size_t /* size */)
    : QueueI<T>(name)
  {}

  bool can_pop() const noexcept override { return fQueue.size() > 0; }
  value_type pop(const duration_type& dur) override // Throws std::runtime_error if a timeout occurs
  {
    folly::Optional<value_type> t=fQueue.try_dequeue_for(dur);
    if(!t){
      throw std::runtime_error("pop timed out");
    }
    return std::move(*t);
  }
  
  bool can_push() const noexcept override
  {
    return true; // Queue is unbounded, so we can always push
  }

  void push(value_type&& t, const duration_type& /* dur */)  override
  {
    // Push can't time out because the queue is unbounded
    fQueue.enqueue(std::move(t));
  }
   
  // Delete the copy and move operations
  FollyQueue(const FollyQueue&) = delete;
  FollyQueue& operator=(const FollyQueue&) = delete;
  FollyQueue(FollyQueue&&) = delete;
  FollyQueue& operator=(FollyQueue&&) = delete;

private:
  FollyQueueType fQueue;
};

template<typename T>
using FollySPSCQueue = FollyQueue<T, folly::USPSCQueue<T, false>>;

template<typename T>
using FollyMPMCQueue = FollyQueue<T, folly::UMPMCQueue<T, false>>;

} // namespace appframework

#endif // APP_FRAMEWORK_INCLUDE_APP_FRAMEWORK_QUEUES_FOLLYQUEUE_HH_


// Local Variables:
// c-basic-offset: 2
// End:
