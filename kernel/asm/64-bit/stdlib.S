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



.global						put_word, get_word, delay_loop, move1k, blank1k




put_word:
						// todo
						STR 		w1		, [x0]
    						RET


get_word:
    						// todo
						LDR 		w0		, [x0]
    						RET



							//
							// delay_loop
							//
							// Simple delay loop.
							//
							// On entry:
							// x0 = delay counter
							//
							// On exit:
							// x0 = 0
							//
delay_loop:
							STR			lr			, [sp, #-16]!
delay_loop2:
							subs x0,x0,#1
							b.ne			delay_loop2 // not bhi
							LDR			lr			, [sp]		, #16

							RET


							
move1k:
						// todo							



blank1k:
						// todo

