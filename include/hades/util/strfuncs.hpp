#ifndef HADES_STRFUNCS_HPP
#define HADES_STRFUNCS_HPP

#include "fmt/format.h"

#include <string>
#include <string_view>
#include <vector>

namespace hades {

std::vector<std::string> split(
    std::string_view strv,
    std::string_view delim = " "
);

template <typename T>
std::string join(
    std::string_view delim,
    std::vector<T> v,
    std::string_view fmtspec = "{}"
) {
  std::string output;

  if (!v.empty())
    output += fmt::vformat(fmtspec, fmt::make_format_args(v[0]));

  // {}{} means we will substitute our format spec in, then use that to format v[i],
  // so if fmtspec is *bad*, then this is just going to segfault
  for (std::size_t i = 1; i < v.size(); i++)
    output += fmt::vformat("{}{}", fmt::make_format_args(delim, v[i], fmtspec));

  return output;
}

} // namespace hades

#endif //HADES_STRFUNCS_HPP
