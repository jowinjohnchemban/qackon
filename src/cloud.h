#pragma once
#include "gps.h"

// Posts {"device":..., "lat":..., "lng":..., ...} to cfg.cloudURL every
// cfg.cloudIntervalSec, but only while WiFi is up. GSM-based upload is not
// implemented yet (see README roadmap) — SMS is the offline fallback today.

void cloudLoop(const GpsFix& fix);
