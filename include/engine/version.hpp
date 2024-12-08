#pragma once

#include <engine/macros.hpp>

#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION_DEV 0

#define VERSION_STR \
XSTR(VERSION_MAJOR) "." \
XSTR(VERSION_MINOR) "." \
XSTR(VERSION_DEV)