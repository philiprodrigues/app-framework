/**
 *
 * @file FollyQueue class Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "app-framework/Queues/FollyQueue.hh"

#define BOOST_TEST_MODULE FollyQueue_test // NOLINT
#include <boost/test/included/unit_test.hpp>

#include <chrono>

// For a first look at the code, you may want to skip past the
// contents of the unnamed namespace and move ahead to the actual test
// cases

namespace {

constexpr double fractional_timeout_tolerance = 0.1;

// Don't set the timeout to zero, otherwise the tests will fail since they'd
// expect the push/pop functions to execute instananeously
constexpr auto timeout = std::chrono::milliseconds(1);
constexpr auto timeout_in_ms =
std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count();

appframework::FollyMPMCQueue<int> Queue("foo", 1000);

} // namespace ""

// This test case should run first. Make sure all other test cases depend on
// this.

BOOST_AUTO_TEST_CASE(sanity_checks)
{

  BOOST_REQUIRE(!Queue.can_pop());

  auto starttime = std::chrono::steady_clock::now();
  Queue.push(999, timeout);
  auto push_time = std::chrono::steady_clock::now() - starttime;

  if (push_time > timeout) {
    auto push_time_in_ms =
      std::chrono::duration_cast<std::chrono::microseconds>(push_time).count();

    BOOST_TEST_REQUIRE(false,
                       "Test failure: pushing element onto empty Queue "
                       "resulted in a timeout (function exited after "
                         << push_time_in_ms << " milliseconds, timeout is "
                         << timeout_in_ms << " milliseconds)");
  }

  BOOST_REQUIRE(Queue.can_pop());

  starttime = std::chrono::steady_clock::now();
  auto popped_value = Queue.pop(timeout);
  auto pop_time = std::chrono::steady_clock::now() - starttime;

  if (pop_time > timeout) {
    auto pop_time_in_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(pop_time).count();
    BOOST_TEST_REQUIRE(false,
                       "Test failure: popping element off Queue "
                       "resulted in a timeout (function exited after "
                         << pop_time_in_ms << " milliseconds, timeout is "
                         << timeout_in_ms << " milliseconds)");
  }

  BOOST_REQUIRE_EQUAL(popped_value, 999);
}

BOOST_AUTO_TEST_CASE(empty_checks,
                     *boost::unit_test::depends_on("sanity_checks"))
{

  try {
    while (Queue.can_pop()) {
      Queue.pop(timeout);
    }
  } catch (const std::runtime_error& err) {
    BOOST_WARN_MESSAGE(true, err.what());
    BOOST_TEST(false,
               "Exception thrown in call to FollyQueue::pop(); unable "
               "to empty the Queue");
  }

  BOOST_REQUIRE(!Queue.can_pop());

  // pop off of an empty Queue

  auto starttime = std::chrono::steady_clock::now();
  BOOST_CHECK_THROW(Queue.pop(timeout), std::runtime_error);
  auto pop_duration = std::chrono::steady_clock::now() - starttime;

  const double fraction_of_pop_timeout_used = pop_duration / timeout;

  BOOST_CHECK_GT(fraction_of_pop_timeout_used,
                 1 - fractional_timeout_tolerance);
  BOOST_CHECK_LT(fraction_of_pop_timeout_used,
                 1 + fractional_timeout_tolerance);
}

