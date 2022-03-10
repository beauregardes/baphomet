/* This has a lot of code taken directly from https://github.com/kfsone/imguiwrap
 *
 * I don't need the extra stuff for making a mainloop and all that, so I
 * figured it would be easier to just take the parts I liked and leave the parts I don't
 * I also made it so everything has a std::string overload because this is C++ and
 * you shouldn't be using const char * unless you have a really good reason.
 */

#pragma once

#include "imgui.h"

#include <string>

namespace dear {

static const ImVec2 zero(0.0f, 0.0f);

template <typename Base, bool ForceDtor = false>
struct ScopeWrapper {
protected:
  const bool should_execute_dtor_;

public:
  constexpr ScopeWrapper(bool should_execute_dtor) noexcept
      : should_execute_dtor_(should_execute_dtor) {}

  ~ScopeWrapper() noexcept {
    if constexpr (ForceDtor || should_execute_dtor_)
      Base::dtor();
  }

  template <typename Func>
  constexpr bool operator&&(Func f) const noexcept {
    if (should_execute_dtor_)
      f();
    return should_execute_dtor_;
  }

  constexpr operator bool() const noexcept {
    return should_execute_dtor_;
  }

protected:
  ScopeWrapper(const ScopeWrapper &) = delete;
  ScopeWrapper &operator=(const ScopeWrapper &) = delete;
};

struct begin : public ScopeWrapper<begin, true> {
  begin(const char *title, bool *open = nullptr, ImGuiWindowFlags flags = 0) noexcept
      : ScopeWrapper(ImGui::Begin(title, open, flags)) {}
  static void dtor() noexcept {
    ImGui::End();
  }
};

struct child : public ScopeWrapper<child, true> {
  child(const char* str_id, const ImVec2 &size = zero, bool border = false, ImGuiWindowFlags flags = 0) noexcept
      : ScopeWrapper(ImGui::BeginChild(str_id, size, border, flags)) {}
  child(ImGuiID id, const ImVec2 &size = zero, bool border = false, ImGuiWindowFlags flags = 0) noexcept
      : ScopeWrapper(ImGui::BeginChild(id, size, border, flags)) {}
  static void dtor() noexcept {
    ImGui::EndChild();
  }
};

} // namespace dear