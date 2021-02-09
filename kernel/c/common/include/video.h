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



#ifndef KERNEL_VIDEO_H
#define KERNEL_VIDEO_H



	#define			VIDEO_WIDTH		640
	#define			VIDEO_HEIGHT	480



	int 			_kernel_video_init 					( void );
	void 			_kernel_video_set_mode 					( unsigned int n );
	void 			_kernel_video_cls 					( void );
	void 			_kernel_video_set_colour 				( unsigned int foreground, unsigned int colour );
	void 			_kernel_video_set_character_position			( unsigned int x, unsigned int y );
	int 			_kernel_video_print_string				( char *string );
	void 			_kernel_video_print_decimal				( unsigned int number );
	void 			_kernel_video_print_char				( char c );
	void 			_kernel_video_plot_pixel				( unsigned int x, unsigned int y, unsigned int colour );
	unsigned int		_kernel_video_get_pixel					( unsigned int x, unsigned int y );
	unsigned int		_kernel_video_get_mode					( void );
	void			_kernel_video_print_hex					( unsigned int h );
	void 			_video_display_memory					( unsigned int tag );

	extern void 			_fastcharplot( unsigned int );


#endif /*KERNEL_VIDEO_H*/
