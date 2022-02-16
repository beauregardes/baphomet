#pragma once

#include "baphomet/app/internal/messenger_config.hpp"

#include <any>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>

namespace baphomet {

using MessageFunc = std::function<void(const MsgCategory &, const std::any &)>;

class Messenger {
public:
  Messenger() = default;

  void setup_endpoint(const MsgEndpoint &name, const MessageFunc &func);

  std::optional<MessageFunc> get_message_func(const MsgEndpoint &name);

private:
  std::unordered_map<MsgEndpoint, MessageFunc> message_funcs_{};
};

class Endpoint {
protected:
  Endpoint() = default;

  void initialize_endpoint(std::shared_ptr<Messenger> messenger, const MsgEndpoint &name);

  template<MsgCategory C, typename... Args>
  void send_msg(
      const MsgEndpoint &destination,
      const Args &... args
  );

  virtual void received_msg(const MsgCategory &category, const std::any &payload);

  template<MsgCategory C>
  auto extract_msg_payload(const std::any &payload);

private:
  std::shared_ptr<Messenger> messenger_{nullptr};
  MsgEndpoint name_{MsgEndpoint::Undefined};
};

template<MsgCategory C, typename... Args>
void Endpoint::send_msg(
    const MsgEndpoint &destination,
    const Args &... args
) {
  auto func = messenger_->get_message_func(destination);
  if (func)
    func.value()(C, typename PayloadMap<C>::type{args..., name_});
}

template<MsgCategory C>
auto Endpoint::extract_msg_payload(const std::any &payload) {
  return std::any_cast<typename PayloadMap<C>::type>(payload);
}

} // namespace baphomet