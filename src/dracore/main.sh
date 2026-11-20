#ifndef DC_MAIN_H
#define DC_MAIN_H

// Merges all Dracore functions into a single file and provides the global
// config. shaderc optimizes away unused functions per material.

#include "config.h"
#include "functions/tonemap.h"
#include "functions/detection.h"
#include "functions/sky.h"
#include "functions/sunmoon.h"
#include "functions/clouds.h"
#include "functions/water.h"
#include "functions/underwater.h"
#include "functions/endsky.h"
#include "functions/fog.h"
#include "functions/wave.h"
#include "functions/wet.h"

#endif
