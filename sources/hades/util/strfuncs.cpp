#include "hades/util/strfuncs.hpp"

namespace hades {

std::vector<std::string> split(
    std::string_view strv,
    std::string_view delim
) {
  std::vector<std::string> output{};

  std::size_t first = 0;
  do {
    const auto second = strv.find(delim, first);

    if (first != second)
      output.emplace_back(strv.substr(first, second - first));

    if (second == std::string_view::npos)
      break;

    first = second + delim.size();
  } while (first < strv.size());

  return output;
}

} // namespace hades