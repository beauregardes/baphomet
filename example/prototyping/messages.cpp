#include <any>
#include <functional>
#include <string>
#include <unordered_map>

#include "fmt/format.h"

using MessageFunc = std::function<void(const std::string &, const std::any &)>;

class Messenger {
public:
  Messenger() = default;

  bool register_endpoint(const std::string &endpoint_name, const MessageFunc &func);
  bool unregister_endpoint(const std::string &endpoint_name);

  bool send_message(
      const std::string &endpoint_name,
      const std::string &text, const std::any &payload
  );

private:
  std::unordered_map<std::string, MessageFunc> message_funcs_{};
};

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

bool Messenger::send_message(
    const std::string &endpoint_name,
    const std::string &text, const std::any &payload
) {
  auto it = message_funcs_.find(endpoint_name);
  if (it == message_funcs_.end())
    return false;

  it->second(text, payload);
  return true;
}

class Foo {
public:
  Messenger &msg;

  Foo(Messenger &msg) : msg(msg) {
    msg.register_endpoint("Foo", [&](const auto &text, const auto &payload) {
      process_msg_(text, payload);
    });
  }

private:
  void process_msg_(const std::string &text, const std::any &payload) {
    fmt::print("From Foo's private member: {}\n", text);
  }
};

class Bar {
public:
  Messenger &msg;

  Bar(Messenger &msg) : msg(msg) {
    msg.register_endpoint("Bar", [&](const auto &text, const auto &payload) {
      process_msg_(text, payload);
    });
  }

private:
  void process_msg_(const std::string &text, const std::any &payload) {
    fmt::print("From Bar's private member: {}\n", text);
  }
};

int main(int, char *[]) {
  Messenger msg = Messenger();

  Foo f = Foo(msg);
  Bar b = Bar(msg);

  f.msg.send_message("Bar", "hello bar", nullptr);
  b.msg.send_message("Foo", "hello foo", nullptr);
}