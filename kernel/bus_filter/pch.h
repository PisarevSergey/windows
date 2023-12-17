#pragma once

#include <ntifs.h>

#include <nt/irp.h>
#include <nt/remove_lock.h>
#include <nt/event.h>
#include <nt/device.h>

#include <trace_logging/trace.h>

#include "driver.h"
#include "filter_device_extension.h"
