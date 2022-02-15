#include "baphomet/app/internal/messenger.hpp"

namespace baphomet {

bool Messenger::register_endpoint(const std::string &endpoint_name, const MessageFunc &func) {
  if (message_funcs_.contains(endpoint_name))
    return false;

  message_funcs_[endpoint_name] = func;
  return true;
}

bool Messenger::unregister_endpoint(const std::string &endpoint_name) {
  auto it = message_funcs_.find(endpoint_name);
  if (it == message_funcs_.end())
    return false;

  message_funcs_.erase(it);
  return true;
}

} // namespace baphomet;