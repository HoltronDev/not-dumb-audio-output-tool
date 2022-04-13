#pragma once

#include "platform.h"

class DeviceSnagginator : IMMDeviceEnumerator
{
public:
	void GetDevices();
};

