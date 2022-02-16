#include "baphomet/app/internal/messenger.hpp"

#include "spdlog/spdlog.h"

namespace baphomet {

void Messenger::setup_endpoint(const MsgEndpoint &name, const MessageFunc &func) {
  if (message_funcs_.contains(name))
    return;

  message_funcs_[name] = func;
}

std::optional<MessageFunc> Messenger::get_message_func(const MsgEndpoint &name) {
  auto it = message_funcs_.find(name);
  if (it != message_funcs_.end())
    return it->second;
  return std::nullopt;
}

void Endpoint::initialize_endpoint(std::shared_ptr<Messenger> messenger, const MsgEndpoint &name) {
  messenger_ = messenger;
  name_ = name;

  messenger_->setup_endpoint(name, [&](const MsgCategory &category, const std::any &payload){
    received_msg(category, payload);
  });
}

void Endpoint::received_msg(const MsgCategory &category, const std::any &payload) {
  spdlog::error("({}) Unhandled message category: '{}'", name_, category);
}


} // namespace baphomet;