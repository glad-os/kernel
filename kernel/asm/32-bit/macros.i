.macro                                      _PUSH_CPU_STATE
                                            STMIA               r0!             , {r1-r12}                              @ PUSH: r1..r12
                                            
                                            _SET_MODE           SYS                                                     @ PUSH: sp, lr
                                            MOV                 r4              , sp
                                            MOV                 r5              , lr
                                            _SET_MODE           SVC
                                            STMIA               r0!             , {r4,r5}
                                            
                                            MRS                 r4              , spsr                                  @ PUSH: cpsr, pc
                                            STMIA               sp!             , {r4,lr}                               
.endm



.macro                                      _POP_CPU_STATE
                                            ADD                 r0              , r0        , #4                        @ (r0 is not restored)
                                            LDMIA               r0!             , {r1-r12}                              @ POP: r1..r12
                                            
                                            _SET_MODE           SYS                                                     @ POP: sp, lr
                                            LDMIA               r0!             , {sp, lr}
                                            _SET_MODE           SVC

                                            LDMIA               r0!             , {r4,lr}                               @ POP: cpsr, pc
                                            MSR                 spsr            , r4

                                            MOVS                pc              , lr
.endm
