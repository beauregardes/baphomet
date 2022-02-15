#pragma once

#include "baphomet/app/internal/msgcat.hpp"

#include <any>
#include <functional>
#include <string>
#include <unordered_map>

namespace baphomet {

using MessageFunc = std::function<void(const MsgCat &, const std::any &)>;

class Messenger {
public:
  Messenger() = default;

  bool register_endpoint(const std::string &endpoint_name, const MessageFunc &func);
  bool unregister_endpoint(const std::string &endpoint_name);

  template<MsgCat C, typename... Args>
  bool send_message(
      const std::string &endpoint_name,
      const Args &... args
  );

  template<MsgCat C>
  static auto extract_payload(const std::any &payload);

private:
  std::unordered_map<std::string, MessageFunc> message_funcs_{};
};

template<MsgCat C, typename... Args>
bool Messenger::send_message(
    const std::string &endpoint_name,
    const Args &... args
) {
  auto it = message_funcs_.find(endpoint_name);
  if (it == message_funcs_.end())
    return false;

  it->second(C, typename internal::PayloadMap<C>::type{args...});
  return true;
}

template<MsgCat C>
auto Messenger::extract_payload(const std::any &payload) {
  return std::any_cast<typename internal::PayloadMap<C>::type>(payload);
}

} // namespace baphomet