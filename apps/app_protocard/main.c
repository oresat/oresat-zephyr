/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "protocard.h"

int main(void)
{
  startThreads();
  
  while(1)
  {
    k_msleep(SLEEP_TIME_MS);
  }

	return 0;
}
