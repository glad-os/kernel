/**
 * Copyright 2019 AbbeyCatUK
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include "const.h"
#include "process.h"


/**
 *
 * _kernel_process_init_cpu_state
 *
 * Initialises the CPU state of a given process (ready to run for the first time)
 *
 */
int _kernel_process_init_cpu_state( cpu_state *state ) {

    int i;

    for ( i = 0; i <= 15; i++ ) {
        state->r[i] = 0;
    }

    // sp, lr, pc and cpsr require specific values at startup
    state->r[16] = 8 * MBYTE;       // sp
    state->r[17] = 0;               // lr
    state->r[18] = 0;               // pstate
    state->r[19] = 4 * MBYTE;       // pc

}
