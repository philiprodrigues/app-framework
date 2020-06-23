/**
 * @file FanOutDAQModule_test.cxx FanOutDAQModule class Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "appfwk/FanOutDAQModule.hpp"
#include "appfwk/QueueRegistry.hpp"

#define BOOST_TEST_MODULE FanOutDAQModule_test // NOLINT

#include <boost/test/unit_test.hpp>
#include <nlohmann/json.hpp>

constexpr auto queue_timeout = std::chrono::milliseconds(10);
using namespace dunedaq::appfwk;

BOOST_AUTO_TEST_SUITE(FanOutDAQModule_test)

/**
 * @brief Initializes the QueueRegistry for use by the FanOutDAQModule test
 */
struct FanOutDAQModuleTestFixture
{
  FanOutDAQModuleTestFixture() {}
  void setup()
  {

    std::map<std::string, QueueConfig> queue_config;
    queue_config["input"].kind = QueueConfig::queue_kind::kStdDeQueue;
    queue_config["input"].capacity = 10;
    queue_config["output1"].kind = QueueConfig::queue_kind::kStdDeQueue;
    queue_config["output1"].capacity = 5;
    queue_config["output2"].kind = QueueConfig::queue_kind::kStdDeQueue;
    queue_config["output2"].capacity = 5;

    QueueRegistry::get().configure(queue_config);
  }
};

BOOST_TEST_GLOBAL_FIXTURE(FanOutDAQModuleTestFixture);

BOOST_AUTO_TEST_CASE(Construct)
{
  dunedaq::appfwk::FanOutDAQModule<int> foum("test");
}

BOOST_AUTO_TEST_CASE(Configure)
{
  dunedaq::appfwk::FanOutDAQModule<dunedaq::appfwk::NonCopyableType> foum("test");

  auto module_config = R"({"input": "input", "fanout_mode": "round_robin", "outputs": []})"_json;
  foum.do_init(module_config);

  foum.execute_command("configure");
}

BOOST_AUTO_TEST_CASE(InvalidConfigure)
{
  dunedaq::appfwk::FanOutDAQModule<dunedaq::appfwk::NonCopyableType> foum("test");

  // Wrongly-capitalized fanout_mode
  auto module_config = R"({"input": "input", "fanout_mode": "Round_robin", "outputs": []})"_json;
  foum.do_init(module_config);

  BOOST_REQUIRE_THROW(foum.execute_command("configure"), dunedaq::appfwk::ConfigureFailed);
}

BOOST_AUTO_TEST_CASE(NonCopyableTypeTest)
{
  dunedaq::appfwk::FanOutDAQModule<dunedaq::appfwk::NonCopyableType> foum("test");

  nlohmann::json module_config = R"(
        {
                    "input": "input",
                    "outputs": ["output1", "output2" ],
                    "fanout_mode": "round_robin"
        }
    )"_json;
  foum.do_init(module_config);

  // This test assumes RoundRobin mode. Once configurability is implemented,
  // we'll have to configure it appropriately.
  foum.execute_command("configure");
  foum.execute_command("start");

  DAQSink<NonCopyableType> inputbuf("input");
  DAQSource<NonCopyableType> outputbuf1("output1");
  DAQSource<NonCopyableType> outputbuf2("output2");

  auto start_push = std::chrono::steady_clock::now();
  inputbuf.push(dunedaq::appfwk::NonCopyableType(1), queue_timeout);
  inputbuf.push(dunedaq::appfwk::NonCopyableType(2), queue_timeout);
  auto after_push = std::chrono::steady_clock::now();

  while (!outputbuf1.can_pop() && !outputbuf2.can_pop()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  auto after_sleep = std::chrono::steady_clock::now();

  foum.execute_command("stop");
  BOOST_TEST_MESSAGE(
    "It took " << std::chrono::duration_cast<std::chrono::milliseconds>(after_push - start_push).count()
               << " ms to push values onto the input queue");
  BOOST_TEST_MESSAGE(
    "It took " << std::chrono::duration_cast<std::chrono::milliseconds>(after_sleep - after_push).count()
               << " ms for FanOutDAQModule to process input");

  BOOST_REQUIRE_EQUAL(outputbuf1.can_pop(), true);
  dunedaq::appfwk::NonCopyableType res(0);
  outputbuf1.pop(res, queue_timeout);
  BOOST_REQUIRE_EQUAL(res.data, 1);

  BOOST_REQUIRE_EQUAL(outputbuf2.can_pop(), true);
  outputbuf2.pop(res, queue_timeout);
  BOOST_REQUIRE_EQUAL(res.data, 2);
}

BOOST_AUTO_TEST_SUITE_END()
