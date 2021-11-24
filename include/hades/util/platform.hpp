#ifndef HADES_PLATFORM_HPP
#define HADES_PLATFORM_HPP

#if defined(_WIN32) || defined(__CYGWIN__)
#define HADES_PLATFORM WINDOWS
#elif defined(__linux__)
#define HADES_PLATFORM LINUX
#endif

#endif //HADES_PLATFORM_HPP
