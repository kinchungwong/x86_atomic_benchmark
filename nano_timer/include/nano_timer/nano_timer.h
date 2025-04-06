#ifndef NANO_TIMER_H
#define NANO_TIMER_H

#include "nano_timer/nano_timer_detail.h"
#include "nano_timer/nano_timer_impl.h"
#include "nano_timer/ez_timer.h"

#ifdef SINGLE_TRANSLATION_UNIT_BUILD_MODE
#include "nano_timer/nano_timer_detail.cpp"
#include "nano_timer/nano_timer_impl.cpp"
#include "nano_timer/ez_timer.cpp"
#endif

#endif // NANO_TIMER_H
