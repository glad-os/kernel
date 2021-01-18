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


#define                             SYS                 0b00011111
#define                             SVC                 0b00010011


.macro                              _SET_MODE           mode
                                    MRS                 r4         	, cpsr
                                    BIC                 r4         	, r4        , #0b00011111
                                    ORR                 r4       	, r4        , #\mode
                                    MSR                 cpsr        , r4
.endm


.macro                              _PUSH_CPU_STATE
                                    STMIA               r0!         , {r1-r12}                              @ PUSH: r1..r12

                                    _SET_MODE           SYS                                                 @ PUSH: sp, lr
                                    MOV                 r4          , sp
                                    MOV                 r5          , lr
                                    _SET_MODE           SVC
                                    STMIA               r0!         , {r4,r5}

                                    MRS                 r4          , spsr                                  @ PUSH: cpsr, pc
                                    STMIA               r0!         , {r4, lr}                               
.endm


.macro                              _POP_CPU_STATE
                                    LDMIA               r0!         , {r1-r12}                              @ POP: r1..r12

                                    _SET_MODE           SYS                                                 @ POP: sp, lr
                                    LDMIA               r0!         , {sp, lr}
                                    _SET_MODE           SVC

                                    LDMIA               r0!         , {r4,lr}                               @ POP: cpsr, pc
                                    MSR                 spsr        , r4

.endm
