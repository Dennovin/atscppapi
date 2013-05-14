/*
 * Copyright (c) 2013 LinkedIn Corp. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of the license at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the
 * License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied.
 *
 */

/**
 * @file AsyncTimer.cc
 * @author Brian Geffon
 * @author Manjesh Nilange
 */
#include "atscppapi/AsyncTimer.h"
#include <ts/ts.h>
#include "logging_internal.h"

using namespace atscppapi;

struct atscppapi::AsyncTimerState {
  TSCont cont_;
  int period_in_ms_;
  int initial_period_in_ms_;
  TSAction initial_timer_action_;
  TSAction periodic_timer_action_;
  AsyncTimer *timer_;
  shared_ptr<AsyncDispatchControllerBase> dispatch_controller_;
  AsyncTimerState(int period_in_ms, int initial_period_in_ms, AsyncTimer *timer)
    : period_in_ms_(period_in_ms), initial_period_in_ms_(initial_period_in_ms),
      initial_timer_action_(NULL), periodic_timer_action_(NULL), timer_(timer) { }
};

namespace {

int handleTimerEvent(TSCont cont, TSEvent event, void *edata) {
  AsyncTimerState *state = static_cast<AsyncTimerState *>(TSContDataGet(cont));
  if (state->initial_timer_action_) {
    LOG_DEBUG("Received initial timer event. Scheduling periodic event now");
    state->initial_timer_action_ = NULL;
    state->periodic_timer_action_ = TSContScheduleEvery(state->cont_, state->period_in_ms_, TS_THREAD_POOL_DEFAULT);
  }
  if (!state->dispatch_controller_->dispatch()) {
    LOG_DEBUG("Receiver has died. Destroying timer");
    delete state->timer_;
  }
  return 0;
}

}

AsyncTimer::AsyncTimer(int period_in_ms, int initial_period_in_ms) {
  state_ = new AsyncTimerState(period_in_ms, initial_period_in_ms, this);
  TSMutex null_mutex = NULL;
  state_->cont_ = TSContCreate(handleTimerEvent, null_mutex);
  TSContDataSet(state_->cont_, static_cast<void *>(state_));
}

void AsyncTimer::run(shared_ptr<AsyncDispatchControllerBase> dispatch_controller) {
  if (state_->initial_period_in_ms_) {
    LOG_DEBUG("Scheduling initial event");
    state_->initial_timer_action_ = TSContSchedule(state_->cont_, state_->initial_period_in_ms_,
                                                   TS_THREAD_POOL_DEFAULT);
  } else {
    LOG_DEBUG("Scheduling regular timer events");
    TSContScheduleEvery(state_->cont_, state_->period_in_ms_, TS_THREAD_POOL_DEFAULT);
  }
  state_->dispatch_controller_ = dispatch_controller;
}

AsyncTimer::~AsyncTimer() {
  if (state_->initial_timer_action_) {
    TSActionCancel(state_->initial_timer_action_);
  }
  if (state_->periodic_timer_action_) {
    TSActionCancel(state_->periodic_timer_action_);
  }
  TSContDestroy(state_->cont_);
  delete state_;
}