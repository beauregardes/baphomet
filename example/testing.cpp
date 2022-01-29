#include "baphomet/baphomet.hpp"
#include <fmt/format.h>

int main(int, char *[]) {
    fmt::print("2 + 3 = {}\n", add(2, 3));
}