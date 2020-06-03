#ifndef APP_FRAMEWORK_INCLUDE_APP_FRAMEWORK_QUEUEREGISTRY_HH_
#define APP_FRAMEWORK_INCLUDE_APP_FRAMEWORK_QUEUEREGISTRY_HH_

#include <map>
#include <memory>
#include <string>
#include "app-framework-base/NamedObject.hh"
#include "app-framework/Queues/NamedStdDeQueue.hh"
#include "app-framework/Queues/NamedFollyQueue.hh"
#include "ers/Issue.h"

// ERS Issues declaration
/** \def ers::File This is the base class for all file related issues.
 */
ERS_DECLARE_ISSUE(  appframework,      // namespace
                        QueueTypeMismatch,       // issue class name
                        "Requested queue \"" << queue_name << "\" of type '" << target_type << "' already declared as type '" << source_type << "'",    // no message
                        ((std::string)queue_name ) 
                        ((std::string)source_type )
                        ((std::string)target_type )
                     )

namespace appframework {

struct QueueConfig
{
    enum queue_kind { std_deque, folly_spsc, folly_mpmc };

  QueueConfig::queue_kind kind;
  size_t size;
};

class QueueRegistry {
  public:


    ~QueueRegistry();

    static QueueRegistry* get();

    template<typename T>
    std::shared_ptr<NamedQueueI<T>> get_queue(std::string name);

    void configure( const std::map<std::string, QueueConfig>& );

  private:

    struct QueueEntry
    {
      const std::type_info* type;
      std::shared_ptr<NamedObject> instance;
    };

    QueueRegistry();

    template<typename T>
    std::shared_ptr<NamedObject> create_queue(std::string name, const QueueConfig& config);

    std::map<std::string,QueueEntry> queue_registry_;
    std::map<std::string,QueueConfig> queue_configmap_;

    bool configured_;

    static QueueRegistry* me_;

    QueueRegistry(const QueueRegistry&) = delete;
    QueueRegistry& operator=(const QueueRegistry&) = delete;
    QueueRegistry(QueueRegistry&&) = delete;
    QueueRegistry& operator=(QueueRegistry&&) = delete;

};

} // namespace appframework

#include "QueueRegistry.icc"

#endif // APP_FRAMEWORK_INCLUDE_APP_FRAMEWORK_QUEUEREGISTRY_HH_
