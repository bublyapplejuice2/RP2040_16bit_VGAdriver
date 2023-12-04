#ifndef __CONST_ARR_H__
#define __CONST_ARR_H__

#include "cayuga_lake.h"
#include "clock_tower.h"
#include "duffield.h"
#include "duffield_inner.h"
#include "empire_state.h"
#include "flowers.h"

const unsigned short *cayuga_lake = &cayuga_lake_arr[0];
const unsigned short *clock_tower = &clock_tower_arr[0];
const unsigned short *duffield = &duffield_arr[0];
const unsigned short *duffield_inner = &duffield_inner_arr[0];
const unsigned short *empire_state = &empire_state_arr[0];
const unsigned short *flowers = &flowers_arr[0];

const unsigned short *images[] = {cayuga_lake, clock_tower, duffield, duffield_inner, empire_state, flowers};

#endif