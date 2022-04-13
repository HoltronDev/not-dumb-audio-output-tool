#pragma once
#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>
#include "resource.h"
#include <mmdeviceapi.h>
#include <stdlib.h>
#include <functiondiscoverykeys_devpkey.h>
#include <vector>
#include <string>
#include <atlstr.h>
#endif