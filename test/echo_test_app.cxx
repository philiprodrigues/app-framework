/**
 * @file echo_test_app.cxx
 *
 * echo_test_app shows the basic functionality of DAQProcess by loading a
 * DebugLoggingDAQModule
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "DebugLoggingDAQModule.hpp"
#include "app-framework/CommandLineInterpreter.hpp"
#include "app-framework/DAQProcess.hpp"

namespace appframework {
/**
 * @brief ModuleList for the echo_test_app
*/
class echo_test_app_contructor : public GraphConstructor
{
  // Inherited via ModuleList
  void ConstructGraph(DAQModuleMap& user_module_map,
                      CommandOrderMap& command_order_map) override
  {
    user_module_map["debugLogger"].reset(
      new DebugLoggingDAQModule("debugLogger"));
  }
};
} // namespace appframework

/**
 * @brief echo_test_app main entry point
 * @param argc Number of arguments
 * @param argv Arguments
 * @return Status code from DAQProcess::listen
*/
int
main(int argc, char* argv[])
{
  auto args =
    appframework::CommandLineInterpreter::ParseCommandLineArguments(argc, argv);

  appframework::DAQProcess theDAQProcess(args);

  appframework::echo_test_app_contructor gc;
  theDAQProcess.register_modules(gc);

  return theDAQProcess.listen();
}